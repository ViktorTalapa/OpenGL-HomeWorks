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
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
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
    Shader(GLuint shaderType, const GLchar* source) {
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
    static void getErrorInfo(GLuint handle) {
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
    VertexShader() : Shader(
        GL_VERTEX_SHADER,
        R"(
            #version 130
            precision highp float;
 
            uniform mat4 MVP;           // Model-View-Projection matrix in row-major format
 
            in vec2 vertexPosition;     // variable input from Attrib Array selected by glBindAttribLocation
            in vec3 vertexColor;        // variable input from Attrib Array selected by glBindAttribLocation
            out vec3 color;             // output attribute
 
            void main() {
                color = vertexColor;                                                        // copy color from input to output
                gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1) * MVP;         // transform to clipping space
            }
        )") {
    }
};

class FragmentShader: public Shader {

  public:
    FragmentShader() : Shader(
        GL_FRAGMENT_SHADER,
        R"(
            #version 130
            precision highp float;
 
            in vec3 color;                // variable input: interpolated color of vertex shader
            out vec4 fragmentColor;       // output that goes to the raster memory as told by glBindFragDataLocation
 
            void main() {
                fragmentColor = vec4(color, 1);     // extend RGB to RGBA
            }
        )") {
    }
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
    void create(Shader vertexShader, Shader fragmentShader) {
        program = glCreateProgram();
        if (program == 0) {
            printf("Error in shader program creation\n");
            exit(1);
        }
        glAttachShader(program, vertexShader.id());
        glAttachShader(program, fragmentShader.id());

        // Connect Attrib Arrays to input variables of the vertex shader
        glBindAttribLocation(program, 0, "vertexPosition");         // vertexPosition gets values from Attrib Array 0
        glBindAttribLocation(program, 1, "vertexColor");             // vertexColor gets values from Attrib Array 1

        // Connect the fragmentColor to the frame buffer memory
        glBindFragDataLocation(program, 0, "fragmentColor");         // fragmentColor goes to the frame buffer memory

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

// row-major matrix 4x4
struct Mat4 {
    float m[4][4];

    Mat4(float m00 = 0, float m01 = 0, float m02 = 0, float m03 = 0,
         float m10 = 0, float m11 = 0, float m12 = 0, float m13 = 0,
         float m20 = 0, float m21 = 0, float m22 = 0, float m23 = 0,
         float m30 = 0, float m31 = 0, float m32 = 0, float m33 = 0) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }
    Mat4 operator*(const Mat4& right) {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; k++)
                    result.m[i][j] += m[i][k] * right.m[k][j];
            }
        }
        return result;
    }
    operator float*() {
        return &m[0][0];
    }
};

// 3D vector in homogeneous coordinates
struct Vec4 {
    float x, y, z, t;

    Vec4(float x0 = 0, float y0 = 0, float z0 = 0, float t0 = 0) {
        x = x0;
        y = y0;
        z = z0;
        t = t0;
    }
    Vec4 operator=(const Vec4& w) {
        x = w.x;
        y = w.y;
        z = w.z;
        t = w.t;
        return w;
    }
    Vec4 operator+(const Vec4& w) {
        return Vec4(x + w.x, y + w.y, z + w.z);
    }
    Vec4 operator-(const Vec4& w) {
        return Vec4(x - w.x, y - w.y, z - w.z);
    }
    float operator*(const Vec4& w) {
        return x * w.x + y * w.y + z * w.z;
    }
    Vec4 operator*(float a) {
        return Vec4(x * a, y * a, z * a);
    }
    Vec4 operator/(float a) {
        return Vec4(x / a, y / a, z / a);
    }
    Vec4 operator*(const Mat4& right) {
        float v[] = { x, y, z, t };
        float w[] = { 0, 0, 0, 0 };
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                w[i] += v[j] * right.m[j][i];
        return Vec4(w[0], w[1], w[2], w[3]);
    }
    float length() {
        return sqrtf(x * x + y * y);
    }
};

struct Color {
    float r, g, b;

    Color() {
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) {
        r = r0;
        g = g0;
        b = b0;
    }
    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }
};

// 2D camera
class Camera {
    Vec4 wC;                // center in world coordinates
    float wWx, wWy;            // width and height in world coordinates
    bool linked;

public:
    Camera() {
        wWx = wWy = 20;
        linked = false;
    }
    Mat4 v() {                 // view matrix: translates the center to the origin
        return Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -wC.x, -wC.y, 0, 1);
    }
    Mat4 p() {                 // projection matrix: scales it to be a square of edge length 2
        return Mat4(2 / wWx, 0, 0, 0, 0, 2 / wWy, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }
    Mat4 vInv() {             // inverse view matrix
        return Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, wC.x, wC.y, 0, 1);
    }
    Mat4 pInv() {             // inverse projection matrix
        return Mat4(wWx / 2, 0, 0, 0, 0, wWy / 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }
    bool isLinked() {
        return linked;
    }
    void setCenter(Vec4 center) {
        wC = center;
    }
    void setLinked(bool l) {
        linked = l;
    }
    Vec4 convert(float pX, float pY) {
        float cX = 2 * pX / (float) windowWidth - 1;    // flip y axis
        float cY = 1 - 2 * pY / (float) windowHeight;
        return Vec4(cX, cY) * pInv() * vInv();
    }
} camera;

class Triangle {
    GLuint vao;                // vertex array object id
    float size, ang;
    Vec4 pos;

public:
    Triangle() {
        vao = ang = 0;
        size = 1;
    }
    void create(Color cl = Color(1, 1, 1)) {
        glGenVertexArrays(1, &vao);                      // create 1 vertex array object
        glBindVertexArray(vao);                            // make it active

        GLuint vbo[2];                                    // vertex buffer objects
        glGenBuffers(2, &vbo[0]);                        // Generate 2 vertex buffer objects

        // vertex coordinates: vbo[0] -> Attrib Array 0 -> vertexPosition of the vertex shader
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);             // make it active, it is an array
        float vertexCoords[] =
                { 0, 1, -sqrt(3) * 0.5, -0.5, sqrt(3) * 0.5, -0.5 }; // vertex data on the CPU
        glBufferData(GL_ARRAY_BUFFER,                      // copy to the GPU
                sizeof(vertexCoords),                      // number of the vbo in bytes
                vertexCoords,                            // address of the data array on the CPU
                GL_STATIC_DRAW);                         // copy to that part of the memory which is not modified
        // Map Attribute Array 0 to the current bound vertex buffer (vbo[0])
        glEnableVertexAttribArray(0);
        // Data organization of Attribute Array 0
        glVertexAttribPointer(0,                        // Attribute Array 0
                2, GL_FLOAT,                             // components/attribute, component type
                GL_FALSE,                                // not in fixed point format, do not normalized
                0, NULL);                                 // stride and offset: it is tightly packed

        // vertex colors: vbo[1] -> Attrib Array 1 -> vertexColor of the vertex shader
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);             // make it active, it is an array
        float vertexColors[] = { cl.r, cl.g, cl.b, cl.r, cl.g, cl.b, cl.r, cl.g, cl.b };    // vertex data on the CPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors,
        GL_STATIC_DRAW);                                // copy to the GPU

        // Map Attribute Array 1 to the current bound vertex buffer (vbo[1])
        glEnableVertexAttribArray(1);                      // Vertex position
        // Data organization of Attribute Array 1
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL); // Attribute Array 1, components/attribute, component type, normalize?, tightly packed
    }
    Vec4 getPos() {
        return pos;
    }
    void setPos(Vec4 nPos) {
        pos = nPos;
    }
    void setSize(float s) {
        size = s;
    }
    void rotate(float alpha) {
        ang = alpha * M_PI / 180;
    }
    void draw() {
        Mat4 m(size * cos(ang), size * sin(ang), 0, 0,
              -size * sin(ang), size * cos(ang), 0, 0,
                             0,               0, 1, 0,
                         pos.x,           pos.y, 0, 1);             // model matrix

        Mat4 mvpTransform = m * camera.v() * camera.p();

        // set GPU uniform matrix variable MVP with the content of CPU variable MVPTransform
        int location = glGetUniformLocation(shaderProgram.id(), "MVP");
        if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, mvpTransform); // set uniform variable MVP to the MVPTransform
        else printf("uniform MVP cannot be set\n");

        glBindVertexArray(vao);                                     // make the vao and its vbos active playing the role of the data source
        glDrawArrays(GL_TRIANGLES, 0, 3);                             // draw a single triangle with vertices defined in vao
    }
};

class Star {
    Triangle a, b, c;
    float size;
    bool enabled, move;
    Vec4 speed;

public:
    Star() {
        move = false;
        enabled = true;
        size = 1;
        speed = 0;
        b.rotate(40);
        c.rotate(80);
    }
    void create(Color cl = Color(1, 1, 1)) {
        a.create(cl);
        b.create(cl);
        c.create(cl);
    }
    bool isEnabled() {
        return enabled;
    }
    bool isMoving() {
        return move;
    }
    void setEnabled(bool l) {
        enabled = l;
    }
    Vec4 getPos() {
        return a.getPos();
    }
    float getSize() {
        return size;
    }
    Vec4 getSpeed() {
        return speed;
    }
    void setMove(bool l) {
        move = l;
    }
    void setPos(Vec4 nPos) {
        a.setPos(nPos);
        b.setPos(nPos);
        c.setPos(nPos);
    }
    void setPos(float wX, float wY) {
        setPos(Vec4(wX, wY, 0, 0));
    }
    void setSize(float s) {
        size = s;
        a.setSize(s);
        b.setSize(s);
        c.setSize(s);
    }
    void setSpeed(Vec4 v) {
        speed = v;
    }
    void rotate(float d) {
        a.rotate(d);
        b.rotate(d + 40);
        c.rotate(d + 80);
    }
    void animate(float t, float pSeed = 1, float rSpeed = 1) {
        float anim = size + 0.2 * size * sinf(pSeed * t);
        a.setSize(anim);
        b.setSize(anim);
        c.setSize(anim);
        rotate(fmod(30 * rSpeed * t, 360));
    }
    void draw() {
        if (!enabled)
            return;
        a.draw();
        b.draw();
        c.draw();
    }
};

class Spline {
    GLuint vao, vbo;
    Vec4 cps[13], v[13];
    int res;
    Color color;

public:
    Spline() {
        vao = vbo = 0;
        res = 1000;
    }
    void create(Color cl) {
        color = cl;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);                         // Generate 1 vertex buffer object
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Enable the vertex attribute arrays
        glEnableVertexAttribArray(0);                  // attribute array 0
        glEnableVertexAttribArray(1);                  // attribute array 1
        // Map attribute array 0 to the vertex data of the interleaved vbo
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));  // attribute array, components/attribute, component type, normalize?, stride, offset
        // Map attribute array 1 to the color data of the interleaved vbo
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    }
    void setup(Vec4* cPoints, int cpSize) {
    	//Segített a megértésben: https://epistemex.github.io/cardinal-spline-js/
        float td = cPoints[cpSize - 1].t - cPoints[0].t + 0.5;
        cps[0] = cPoints[cpSize - 1];
        cps[0].t = -fabs(cPoints[0].t - 0.5) / td;
        for (int i = 1; i <= cpSize; i++) {
            cps[i] = cPoints[i - 1];
            cps[i].t = (cPoints[i - 1].t - cPoints[0].t) / td;
        }
        cps[cpSize + 1] = cps[1];
        cps[cpSize + 1].t = 1;
        cps[cpSize + 2] = cps[2];
        cps[cpSize + 2].t = 1 + cps[2].t;

        for (int i = 2; i <= cpSize + 1; i++) {
            Vec4 a = (cps[i + 1] - cps[i]) / (cps[i + 1].t - cps[i].t);
            Vec4 b = (cps[i] - cps[i - 1]) / (cps[i].t - cps[i - 1].t);
            v[i] = (a + b) * 0.45;
        }
        v[1] = v[cpSize + 1];
    }
    Vec4 getR(float t, int i = -1) {
        if (i == -1) for (i = 0; t > cps[i + 1].t; i++);
        float td = cps[i + 1].t - cps[i].t;
        Vec4 a = (cps[i] - cps[i + 1]) * 2 / pow(td, 3) + (v[i + 1] + v[i]) / pow(td, 2);
        Vec4 b = (cps[i + 1] - cps[i]) * 3 / pow(td, 2) - (v[i + 1] + v[i] * 2) / td;
        return a * pow(t - cps[i].t, 3) + b * pow(t - cps[i].t, 2) + v[i] * (t - cps[i].t) + cps[i];
    }
    void setVertex() {
        // fill interleaved data
        float* vertexData = new float[5 * res];
        for (int i = 1, j = 0; j < res; j++) {
            float t = j / (float) res;
            Vec4 r = getR(t, i);
            vertexData[5 * j] = r.x;
            vertexData[5 * j + 1] = r.y;
            vertexData[5 * j + 2] = color.r;
            vertexData[5 * j + 3] = color.g;
            vertexData[5 * j + 4] = color.b;
            if (cps[i + 1].t <= t) i++;
        }
        // copy data to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 5 * res * sizeof(float), vertexData, GL_DYNAMIC_DRAW);
        delete[] vertexData;
    }
    void draw() {
        Mat4 vpTransform = camera.v() * camera.p();
        int location = glGetUniformLocation(shaderProgram.id(), "MVP");
        if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, vpTransform);
        else printf("uniform MVP cannot be set\n");

        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_LOOP, 0, res);
    }
};

// The virtual world: collection of objects
int cpSize = 0;
Vec4 cPoints[10];
Spline spline;
Star stars[3];

// Initialization, create an OpenGL context
void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);

    // Create vertex and fragment shading
    Shader vShader = VertexShader();
    Shader fShader = FragmentShader();

    // Attach shaders to a single program
    shaderProgram.create(vShader, fShader);

    // Create objects by setting up their vertex data on the GPU
    spline.create(Color(0.2, 0.2, 0.2));
    stars[0].create(Color(1, 0.5, 0));
    stars[0].setPos(5, 5);
    stars[0].setSize(0.4);
    stars[1].create(Color(0.988, 0.719, 0.074));
    stars[1].setPos(-5, -5);
    stars[1].setSize(0.7);
    stars[2].create(Color(1, 1, 0));
    stars[2].setSize(1.5);
    stars[2].setEnabled(false);
}

void onExit() {
    shaderProgram.deleteIt();
    printf("exit");
}

// Window has become invalid: Redraw
void onDisplay() {
    glClearColor(0, 0, 0, 0);                                // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the screen
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    if (camera.isLinked())
        camera.setCenter(stars[2].getPos());
    spline.draw();

    if (stars[2].isMoving()) stars[2].setPos(spline.getR(fmod(time / cpSize, 1)));

    for (int i = 0; i < 3; i++) {
        Vec4 v0 = stars[i].getSpeed();
        float dt = time - v0.t;
        float iSize = stars[i].getSize();

        if (i < 2 && stars[i].isMoving() && dt > 0.01) {
            Vec4 d, f, fg, fs, v;
            float bSize = stars[2].getSize();
            d = stars[2].getPos() - stars[i].getPos();
            fg = d * 0.00000067 * iSize * bSize / pow(d.length(), 3);
            if (d.length() < 3 * bSize) fg = fg * d.length() / (3 * bSize);
            fs = v0 * -0.0008 * iSize / dt;
            f = fg + fs;
            v = v0 + f * dt / iSize;
            Vec4 nPos = stars[i].getPos() + (v0 + v) * 0.5 * dt;
            v.t += dt;
            stars[i].setSpeed(v);
            stars[i].setPos(nPos);
        }
        if (stars[i].isEnabled()) {
            stars[i].draw();
            stars[i].animate(time, 3 / iSize, 2 / iSize);
        }
    }

    glutSwapBuffers();                               // exchange the two buffers
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
    if (key == ' ' && stars[2].isEnabled())
        camera.setLinked(true);
    glutPostRedisplay();
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
    glutPostRedisplay();
}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && cpSize < 10) {
        Vec4 point = camera.convert(pX, pY);
        point.t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        if (cpSize < 10)
            cPoints[cpSize++] = point;
        spline.setup(cPoints, cpSize);
        spline.setVertex();
        if (cpSize == 2) {
            stars[2].setPos(point);
            stars[2].setEnabled(true);
            for (int i = 0; i < 3; i++) {
                stars[i].setMove(true);
                stars[i].setSpeed(Vec4(0, 0, 0, point.t));
            }
        }
        glutPostRedisplay();     // redraw
    }
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {
    glutPostRedisplay();
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
    glutPostRedisplay();                    // redraw the scene
}

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
    glewExperimental = true;    // magic
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
