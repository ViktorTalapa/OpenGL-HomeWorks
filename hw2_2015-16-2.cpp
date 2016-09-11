//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2016-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - new operatort hivni a lefoglalt adat korrekt felszabaditasa nelkul
// - felesleges programsorokat a beadott programban hagyni
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Talapa Viktor
// Neptun : QXKA9R
// ---------------------------------------------------------------------------------------------
// Ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
 
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>        // must be downloaded
#include <GL/freeglut.h>    // must be downloaded unless you have an Apple
#endif
 
const unsigned int windowWidth = 600, windowHeight = 600;
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...
 
// OpenGL major and minor versions
int majorVersion = 3, minorVersion = 0;
 
class Shader {
    GLuint shader;
 
public:
    Shader() {
        shader = 0;
    }
    Shader(const GLuint& shaderType, const GLchar* source) {
        // Create shader from string
        shader = glCreateShader(shaderType);
        if (shader == 0) {
            printf("Error in shader creation\n");
            exit(1);
        }
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        checkShader("Shader error");
    }
    GLuint id() {
        return shader;
    }
    static void getErrorInfo(const GLuint& handle) {
        int logLen;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char * log = new char[logLen];
            int written;
            glGetShaderInfoLog(handle, logLen, &written, log);
            printf("Shader log:\n%s", log);
            delete log;
        }
    }
    // check if shader could be compiled
    void checkShader(const char* message) {
        int ack;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ack);
        if (!ack) {
            printf("%s!\n", message);
            getErrorInfo(shader);
        }
    }
};
 
class VertexShader: public Shader {
 
public:
    //vertex shader in GLSL
    VertexShader() : Shader(
            GL_VERTEX_SHADER,
            R"(
                #version 130
                precision highp float;
 
                in vec2 vertexPosition;        // variable input from Attrib Array selected by glBindAttribLocation
                out vec2 texcoord;            // output attribute: texture coordinate
 
                void main() {
                    texcoord = (vertexPosition + vec2(1, 1))/2;                            // -1,1 to 0,1
                    gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1);         // transform to clipping space
                }
            )") {}
};
 
class FragmentShader: public Shader {
 
public:
    //fragment shader in GLSL
    FragmentShader() : Shader(
            GL_FRAGMENT_SHADER,
            R"(
                #version 130
                precision highp float;
 
                uniform sampler2D textureUnit;
                in  vec2 texcoord;            // interpolated texture coordinates
                out vec4 fragmentColor;        // output that goes to the raster memory as told by glBindFragDataLocation
 
                void main() {
                    fragmentColor = texture(textureUnit, texcoord);
                }
            )") {}
};
 
// handler of the shader program
class ShaderProgram {
    GLuint program;
 
public:
    ShaderProgram() {
        program = 0;
    }
    GLuint id() {
        return program;
    }
    void create(Shader& vertexShader, Shader& fragmentShader) {
        program = glCreateProgram();
        if (program == 0) {
            printf("Error in shader program creation\n");
            exit(1);
        }
        glAttachShader(program, vertexShader.id());
        glAttachShader(program, fragmentShader.id());
 
        // Connect Attrib Arrays to input variables of the vertex shader
        glBindAttribLocation(program, 0, "vertexPosition");             // vertexPosition gets values from Attrib Array 0
        glBindAttribLocation(program, 1, "vertexColor");                // vertexColor gets values from Attrib Array 1
 
        // Connect the fragmentColor to the frame buffer memory
        glBindFragDataLocation(program, 0, "fragmentColor");            // fragmentColor goes to the frame buffer memory
 
        // program packaging
        glLinkProgram(program);
        checkLinking();
 
        // make this program run
        glUseProgram(program);
    }
    // check if shader could be linked
    void checkLinking() {
        GLint ack;
        glGetProgramiv(program, GL_LINK_STATUS, &ack);
        if (ack == 0) {
            printf("Failed to link shader program!\n");
            Shader::getErrorInfo(program);
        }
    }
    void deleteIt() {
        glDeleteProgram(program);
    }
} shaderProgram;
 
// 3D vector
struct Vec3 {
    float x, y, z;
 
    Vec3(const float& x0 = 0, const float& y0 = 0, const float& z0 = 0) {
        x = x0;
        y = y0;
        z = z0;
    }
    Vec3& operator=(const Vec3& vec) {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }
    Vec3 operator+(const Vec3& vec) {
        return Vec3(x + vec.x, y + vec.y, z + vec.z);
    }
    Vec3 operator-(const Vec3& vec) {
        return Vec3(x - vec.x, y - vec.y, z - vec.z);
    }
    Vec3 operator-() {
        return Vec3(-x, -y, -z);
    }
    float operator*(const Vec3& vec) {
        return x * vec.x + y * vec.y + z * vec.z;
    }
    Vec3 operator*(const float& s) {
        return Vec3(x * s, y * s, z * s);
    }
    Vec3 operator/(const float& s) {
        return Vec3(x / s, y / s, z / s);
    }
    Vec3 operator%(const Vec3& vec) {
        return Vec3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
    }
    float length() {
        return sqrtf(x * x + y * y + z * z);
    }
    Vec3& normalize() {
        *this = *this * (1 / length());
        return *this;
    }
};
 
struct Color {
    float r, g, b;
 
    Color(float r0 = 0, float g0 = 0, float b0 = 0) {
        r = r0;
        g = g0;
        b = b0;
    }
    Color& operator=(const Color& clr) {
        r = clr.r;
        g = clr.g;
        b = clr.b;
        return *this;
    }
    Color operator+(const Color& clr) {
        return Color(r + clr.r, g + clr.g, b + clr.b);
    }
    Color& operator+=(const Color& clr) {
        *this = *this + clr;
        return *this;
    }
    Color operator-(const Color& clr) {
        return Color(r - clr.r, g - clr.g, b - clr.b);
    }
    Color& operator-=(const Color& clr) {
        *this = *this - clr;
        return *this;
    }
    Color operator*(const Color& clr) {
        return Color(r * clr.r, g * clr.g, b * clr.b);
    }
    Color operator/(const Color& clr) {
        return Color(r / clr.r, g / clr.g, b / clr.b);
    }
    Color operator*(const float& s) {
        return Color(r * s, g * s, b * s);
    }
};
 
// 4D vector
class Vec4 {
    float v[4];
 
public:
    Vec4(const float& a = 0, const float& b = 0, const float& c = 0, const float& d = 0) {
        v[0] = a;
        v[1] = b;
        v[2] = c;
        v[3] = d;
    }
};
 
class Material {
    Color kA, kD, kS;
    float sh;
    Color n, k;
    bool reflect, refract;
 
public:
    Material() {
        sh = 1;
        reflect = refract = false;
    }
    Color getAmbient() const {
        return kA;
    }
    Color getDiffuse() const {
        return kD;
    }
    Color getSpecular() const {
        return kS;
    }
    float getShine() const {
        return sh;
    }
    bool isReflective() const {
        return reflect;
    }
    bool isRefractive() const {
        return refract;
    }
    void setAmbient(const float& rA, const float& gA, const float& bA) {
        kA = Color(rA, gA, bA);
    }
    void setDiffuse(const float& rD, const float& gD, const float& bD) {
        kD = Color(rD, gD, bD);
    }
    void setSpecular(const float& rS, const float& gS, const float& bS) {
        kS = Color(rS, gS, bS);
    }
    void setShine(const float& shine) {
        sh = shine;
    }
    void setExtractCoeff(const float& kR, const float& kG = 0, const float& kB = 0) {
        k = Color(kR, kG, kB);
    }
    void setRefractIndex(const float& nR, const float& nG = 0, const float& nB = 0) {
        n = Color(nR, nG, nB);
    }
    void setReflective(const bool& l) {
        reflect = l;
    }
    void setRefractive(const bool& l) {
        refract = l;
    }
    Vec3 getReflection(Vec3& inDir, Vec3& normal) {
        return inDir - normal * (normal * inDir) * 2;
    }
    Vec3 getRefraction(Vec3& inDir, Vec3& normal) {
        float n0 = n.r;
        float cosA = -(inDir * normal);
        if (cosA < 0) {
            cosA = -cosA;
            normal = -normal;
            n0 = 1 / n0;
        }
        float disc = 1 - (1 - cosA * cosA) / (n0 * n0);
        if (disc < 0)
            return getReflection(inDir, normal);
        else
            return inDir / n0 + normal * (cosA / n0 - sqrtf(disc));
    }
    Color getFresnel(Vec3& inDir, Vec3& normal) {
        float cosA = fabsf(normal * inDir);
        Color c1 = Color(1, 1, 1);
        Color f0 = ((n - c1) * (n - c1) + k * k) / ((n + c1) * (n + c1) + k * k);
        return f0 + (c1 - f0) * pow(1 - cosA, 5);
    }
    Color getShade(Vec3& normal, Vec3& viewDir, Vec3& lightDir, Color& inRad) {
        Color reflRad;
        float cosT = normal * lightDir;
        if (cosT < 0) return reflRad;
        reflRad = inRad * kD * cosT;
        Vec3 halfWay = (viewDir + lightDir).normalize();
        float cosD = normal * halfWay;
        if (cosD < 0) return reflRad;
        return reflRad + inRad * kS * pow(cosD, sh);
    }
};
 
class Materializable {
 
protected:
    Material* mat;
 
public:
    Materializable(Material* material = NULL) {
        mat = material;
    }
    Material* getMaterial() const {
        return mat;
    }
    void setMaterial(Material* material) {
        mat = material;
    }
};
 
class Positionable {
 
protected:
    Vec3 pos;
 
public:
    Vec3 getPosition() const {
        return pos;
    }
    void setPosition(const Vec3& position) {
        pos = position;
    }
    void setPosition(const float& xPos, const float& yPos, const float& zPos) {
        pos = Vec3(xPos, yPos, zPos);
    }
};
 
class Directable {
 
protected:
    Vec3 dir;
 
public:
    Vec3 getDirection() const {
        return dir;
    }
    void setDirection(const Vec3& direction) {
        dir = direction;
        dir.normalize();
    }
};
 
class Normalizable {
 
protected:
    Vec3 norm;
 
public:
    Vec3 getNormal() const {
        return norm;
    }
    void setNormal(const Vec3& normal) {
        norm = normal;
    }
};
 
class Ray : public Positionable, public Directable { };
 
class Hit : public Positionable, public Normalizable, public Materializable {
    float t;
 
public:
    Hit(const float& rayParameter = -1) {
        t = rayParameter;
    }
    float getRayParam() const {
        return t;
    }
    void setRayParam(const float& rayParameter) {
        t = rayParameter;
    }
};
 
class Camera : public Positionable {
    Vec3 dir, lookAt, up, right;
    float fov;
 
public:
    Camera(const float& fieldOfView = 60) {
        lookAt = Vec3(0, 1, 0);
        fov = fieldOfView;
    }
    void setFieldOfView(const float& fieldOfView) {
        fov = fieldOfView;
    }
    void setLookAt(const float& xLook, const float& yLook, const float& zLook) {
        lookAt = Vec3(xLook, yLook, zLook);
    }
    void setup() {
        dir = (lookAt - pos).normalize();
        up = Vec3(0, 0, 1);
        right = (dir % up).normalize();
        up = (right % dir).normalize();
        lookAt = pos + dir * (0.5 * windowWidth / tan(0.5 * fov * M_PI / 180.0f));
    }
    Ray getRay(const int& x, const int& y) {
        Ray ray;
        ray.setPosition(pos);
        Vec3 rayDir = (lookAt + right * (2 * x / (float)(windowWidth - 1))
                       + up * (2 * y / (float)(windowHeight - 1)) - pos).normalize();
        ray.setDirection(rayDir);
        return ray;
    }
};
 
class Light : public Positionable {
 
protected:
    Color lE;
 
public:
    void setIntensity(const float &rC, const float &gC, const float &bC) {
        lE = Color(rC, gC, bC);
    }
    virtual Color getIntensityAt(const Vec3 &pos) {
        return lE;
    }
    virtual Vec3 getDirectionAt(const Vec3& pos) = 0;
    virtual ~Light() {}
};
 
class DirectionalLight : public Light, public Directable {
 
public:
    Vec3 getDirectionAt(const Vec3& pos) {
        return getDirection();
    }
};
 
class Object : public Positionable, public Materializable {
 
public:
    static constexpr float EPSILON = 0.01f;
    virtual Hit intersect(const Ray& ray) = 0;
    virtual ~Object() {}
};
 
class Ellipsoid : public Object {
    Vec3 r;
 
public:
    void setRadius(const float& xRad, const float& yRad, const float& zRad) {
        r = Vec3(xRad, yRad, zRad);
    }
    Hit intersect(const Ray& ray) {
        
        Hit hit = Hit(-1);
        Vec3 rDir = ray.getDirection();
        Vec3 dist = ray.getPosition() - pos;
        Vec3 r2 = Vec3(r.x * r.x, r.y * r.y, r.z * r.z);
        float a = rDir.x * rDir.x / r2.x + rDir.y * rDir.y / r2.y + rDir.z * rDir.z / r2.z;
        float b = 2 * dist.x * rDir.x / r2.x + 2 * dist.y * rDir.y / r2.y + 2 * dist.z * rDir.z / r2.z;
        float c = dist.x * dist.x / r2.x + dist.y * dist.y / r2.y + dist.z * dist.z / r2.z;
        float d = b * b - 4 * a * c;
 
        if (d < 0 || a == 0 || b == 0 || c == 0)
            return hit;
        d = sqrtf(d);
        float t1 = (-b + d) / (2 * a);
        float t2 = (-b - d) / (2 * a);
 
        float t = 0;
        if (t1 < EPSILON && t2 < EPSILON)
            return hit;
        else if (t1 < EPSILON)
            t = t2;
        else if (t2 < EPSILON)
            t = t1;
        else
            t = (t1 < t2) ? t1 : t2;
        hit.setRayParam(t);
        hit.setPosition(ray.getPosition() + rDir * t);
        Vec3 norm = hit.getPosition() - pos;
        norm.x = norm.x / r2.x;
        norm.y = norm.y / r2.y;
        norm.z = norm.z / r2.z;
        hit.setNormal(norm.normalize());
        hit.setMaterial(mat);
        return hit;
    }
};
 
class Plain : public Object, public Normalizable {
 
public:
    void setPlain(Vec3& p1, Vec3& p2, Vec3& p3) {
        pos = p1;
        norm = ((p1 - p2) % (p1 - p3)).normalize();
    }
    Hit intersect(const Ray& ray) {
        Hit hit = Hit(-1);
        Vec3 rDir = ray.getDirection();
        Vec3 rPos = ray.getPosition();
        float t = norm * (pos - rPos) / (norm * rDir);
        if (t < EPSILON)
            return hit;
        hit.setRayParam(t);
        hit.setPosition(rPos + rDir * t);
        hit.setNormal(norm);
        hit.setMaterial(mat);
        return hit;
    }
};
 
class Scene {
    Camera* camera;
    Object* objects[10];
    Light* lights[10];
    Color lA;
    unsigned short oCount, lCount;
 
    float sign(const float& value) {
        if (value < 0)
            return -1;
        return 1;
    }
 
public:
    Scene() {
        camera = NULL;
        oCount = lCount = 0;
    }
    void addCamera(Camera* cam) {
        camera = cam;
    }
    void addObject(Object* object) {
        if (oCount == 10)
            return;
        objects[oCount++] = object;
    }
    void addLight(Light* light) {
        if (lCount == 10)
            return;
        lights[lCount++] = light;
    }
    void setAmbientLight(const float& rA, const float& gA, const float& bA) {
        lA = Color(rA, gA, bA);
    }
    Hit firstIntersect(const Ray& ray) {
        Hit bestHit;
        for (unsigned short i = 0; i < oCount; i++) {
            Hit hit = objects[i]->intersect(ray);
            if (hit.getRayParam() > 0 && (bestHit.getRayParam() < 0 || hit.getRayParam() < bestHit.getRayParam()))
                bestHit = hit;
        }
        return bestHit;
    }
    Color trace(const Ray& ray, const int& depth) {
        if (depth > 3)
            return lA;
        Hit hit = firstIntersect(ray);
        if (hit.getRayParam() < 0)
            return lA;
        Material* hitMat = hit.getMaterial();
        Vec3 r = hit.getPosition();
        Vec3 v = ray.getDirection();
        Vec3 n = hit.getNormal();
        float eps = Object::EPSILON;
 
        Color outRad = hitMat->getAmbient() * lA;
        for (unsigned short i = 0; i < lCount; i++) {
            Color lE = lights[i]->getIntensityAt(r);
            Vec3 lL = lights[i]->getDirectionAt(r);
            Vec3 yL = lights[i]->getPosition();
            Ray shadowRay;
            shadowRay.setPosition(r + n * eps * sign(n * v));
            shadowRay.setDirection(lL);
            Hit shadowHit = firstIntersect(shadowRay);
            if (shadowHit.getRayParam() < 0 || shadowHit.getRayParam() > (r - yL).length())
                outRad += hit.getMaterial()->getShade(n, v, lL, lE);
        }
        Ray refRay;
        if (hitMat->isReflective()) {
            refRay.setDirection(hitMat->getReflection(v, n).normalize());
            refRay.setPosition(r + n * eps * sign(n * v));
            outRad += trace(refRay, depth + 1) * hitMat->getFresnel(v, n);
        }
        if (hitMat->isRefractive()) {
            refRay.setDirection(hitMat->getRefraction(v, n).normalize());
            refRay.setPosition(r + n * eps * sign(n * v));
            outRad += trace(refRay, depth + 1) * (Color(1, 1, 1) - hitMat->getFresnel(v, n));
        }
        return outRad;
    }
    void render(Vec4* image, const unsigned int& width, const unsigned int& height) {
        for (unsigned int x = 0; x < width; x++)
            for (unsigned int y = 0; y < height; y++) {
                Color clr = trace(camera->getRay(x, y), 0);
                image[y * width + x] = Vec4(clr.r, clr.g, clr.b, 1);
            }
    }
};
 
class TexturedQuad {
    GLuint vao, id;                            // vertex array object id and texture id
 
public:
    TexturedQuad() {
        vao = id = 0;
    }
    void create(Vec4* image) {
        glGenVertexArrays(1, &vao);            // create 1 vertex array object
        glBindVertexArray(vao);                // make it active
 
        GLuint vbo = 0;                        // vertex buffer objects
        glGenBuffers(1, &vbo);                // Generate 1 vertex buffer objects
 
        // Vertex coordinates: vbo[0] -> Attrib Array 0 -> vertexPosition of the vertex shader
        glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it active, it is an array
        static float vertexCoords[] = { -1, -1,  1, -1,  -1, 1,
                                         1, -1,  1,  1,  -1, 1 };    // two triangles forming a quad
        // Copy to that part of the memory which is not modified
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
        // Map Attribute Array 0 to the current bound vertex buffer (vbo[0])
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,    0, NULL);     // stride and offset: it is tightly packed
 
        // Create objects by setting up their vertex data on the GPU
        glGenTextures(1, &id);               // id generation
        glBindTexture(GL_TEXTURE_2D, id);    // binding
 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, image); // To GPU
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);                               // sampling
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
 
    void draw() {
        glBindVertexArray(vao);                    // make the vao and its vbos active playing the role of the data source
        int location = glGetUniformLocation(shaderProgram.id(), "textureUnit");
        if (location >= 0) {
            glUniform1i(location, 0);            // texture sampling unit is TEXTURE0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, id);    // connect the texture to the sampler
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);        // draw two triangles forming a quad
    }
};
 
// The virtual world: collection of objects
TexturedQuad fullScreenTexturedQuad;
 
// Initialization, create an OpenGL context
void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);
 
    // Create vertex and fragment shading
    VertexShader vShader;
    FragmentShader fShader;
 
    // Attach shaders to a single program
    shaderProgram.create(vShader, fShader);
 
    // Create objects by setting up their vertex data on the GPU
    Material gold, silver, water, grass;
    gold.setRefractIndex(0.17f, 0.35f, 1.5f);
    gold.setExtractCoeff(3.1f, 2.7f, 1.9f);
    gold.setReflective(true);
    gold.setRefractive(false);
    silver.setRefractIndex(0.14f, 0.16f, 0.13f);
    silver.setExtractCoeff(4.1f, 2.3f, 3.1f);
    silver.setReflective(true);
    silver.setRefractive(false);
    water.setRefractIndex(1.3f);
    water.setReflective(true);
    water.setRefractive(true);
    grass.setAmbient(0, 0.4824f, 0.0571f);
    grass.setDiffuse(0, 0.4824f, 0.0571f);
    grass.setReflective(false);
    grass.setRefractive(false);
 
    Camera camera;
    camera.setFieldOfView(60);
    camera.setPosition(0, -50, 10);
    camera.setLookAt(0, 0, 0);
    camera.setup();
 
    DirectionalLight sun;
    sun.setIntensity(0.9882f, 0.8314f, 0.251f);
    float azim = 155.25f;                
    float elev = 64.29f;
    azim += 90;
    Vec3 sunDir = Vec3(sinf(elev) * cosf(azim), sinf(elev) * sinf(azim), cosf(elev));
    sun.setDirection(sunDir.normalize());
    sun.setPosition(sunDir * 1000.0f);
 
    Plain plain;
    Vec3 p1 = Vec3(0, 0, 1);
    Vec3 p2 = Vec3(1, 0, 1);
    Vec3 p3 = Vec3(0, 1, 1);
    plain.setPlain(p1, p2, p3);
    plain.setMaterial(&grass);
 
    Ellipsoid goldBall;
    goldBall.setPosition(0, 0, 10);
    goldBall.setRadius(7, 5, 8);
    goldBall.setMaterial(&gold);
 
    Scene scene;
    scene.addCamera(&camera);
    scene.setAmbientLight(0.5294f, 0.8078f, 0.9216f);
    scene.addLight(&sun);
    scene.addObject(&plain);
    scene.addObject(&goldBall);
 
    static Vec4 image[windowWidth * windowHeight];
    scene.render(image, windowWidth, windowHeight);
    fullScreenTexturedQuad.create(image);
}
 
// Window has become invalid: Redraw
void onDisplay() {
    glClearColor(0, 0, 0, 0);                                // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // clear the screen
 
    fullScreenTexturedQuad.draw();
 
    glutSwapBuffers();                                       // exchange the two buffers
}
 
void onExit() {
    shaderProgram.deleteIt();
    printf("exit");
}
// Idle event indicating that some time elapsed: do animation here
void onIdle() {
    glutPostRedisplay();                        // redraw the scene
}
// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {}
 
// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {}
 
// Mouse click event
void onMouse(int button, int state, int pX, int pY) {}
 
// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {}
 
// Idaig modosithatod...
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
#if !defined(__APPLE__)
    glutInitContextVersion(majorVersion, minorVersion);
#endif
    glutInitWindowSize(windowWidth, windowHeight); // Application window is initially of resolution 600x600
    glutInitWindowPosition(100, 100); // Relative location of the application window
#if defined(__APPLE__)
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE); // 8 bit R,G,B,A + double buffer + depth buffer
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutCreateWindow(argv[0]);
#if !defined(__APPLE__)
    glewExperimental = (GLboolean) true;    // magic
    glewInit();
#endif
    printf("GL Vendor    : %s\n", glGetString(GL_VENDOR));
    printf("GL Renderer  : %s\n", glGetString(GL_RENDERER));
    printf("GL Version (string)  : %s\n", glGetString(GL_VERSION));
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    printf("GL Version (integer) : %d.%d\n", majorVersion, minorVersion);
    printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
 
    onInitialization();
 
    glutDisplayFunc(onDisplay);                // Register event handlers
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
 
    glutMainLoop();
    onExit();
    return 1;
}