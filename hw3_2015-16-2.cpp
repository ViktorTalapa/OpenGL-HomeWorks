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

// 3D vector
class Vec3 {

public:
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
    Vec3 operator+(const Vec3& vec) const {
        return Vec3(x + vec.x, y + vec.y, z + vec.z);
    }
    Vec3 operator-(const Vec3& vec) const {
        return Vec3(x - vec.x, y - vec.y, z - vec.z);
    }
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    float operator*(const Vec3& vec) const {
        return x * vec.x + y * vec.y + z * vec.z;
    }
    Vec3 operator*(const float& s) const {
        return Vec3(x * s, y * s, z * s);
    }
    Vec3 operator/(const float& s) const {
        return Vec3(x / s, y / s, z / s);
    }
	friend Vec3 operator/(const float& s, const Vec3& vec) {
		return Vec3(s / vec.x, s / vec.y, s / vec.z);
	}
    Vec3 operator%(const Vec3& vec) const {
        return Vec3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
    }
    float length() const {
        return sqrtf(x * x + y * y + z * z);
    }
    Vec3& normalize() {
    	float s = length();
    	if (s != 0)
    		*this = *this / s;
        return *this;
    }
    float* toArray() const {
        float* vec = new float[3];
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        return vec;
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
    Vec4(const Vec3& vec, const float& d = 0) {
		v[0] = vec.x;
		v[1] = vec.y;
		v[2] = vec.z;
		v[3] = d;
    }
    float get(const int& index) const {
        return v[index];
    }
    void set(const int& index, const float& value) {
        v[index] = value;
    }
    Vec4& operator=(const Vec4& vec) {
        for (int i = 0; i < 4; i++)
            set(i, vec.get(i));
        return *this;
    }
    float operator*(const Vec4& vec) const {
    	float res = 0;
    	for (int i = 0; i < 4; i++)
    		res += v[i] * vec.v[i];
    	return res;
    }
	float* toArray() const {
        float* vec = new float[4];
        for (int i = 0; i < 4; i++)
            vec[i] = v[i];
        return vec;
	}
};

// 4X4 row-major matrix
class Mat4 {
    Vec4 m[4];

public:
    Mat4(const Vec4& a = Vec4(), const Vec4& b = Vec4(), const Vec4& c = Vec4(), const Vec4& d = Vec4()) {
        m[0] = a;
        m[1] = b;
        m[2] = c;
        m[3] = d;
    }
    float get(const int& rIndex, const int& cIndex) const {
        return m[rIndex].get(cIndex);
    }
    Vec4 getRow(const int& rIndex) const {
        return m[rIndex];
    }
    Vec4 getColumn(const int& cIndex) const {
        Vec4 column;
        for (int i = 0; i < 4; i++)
            column.set(i, get(i, cIndex));
        return column;
    }
    void set(const int& rIndex, const int& cIndex, const float& value) {
        m[rIndex].set(cIndex, value);
    }
    void setRow(const int& rIndex, const Vec4& values) {
        m[rIndex] = values;
    }
    void setColumn(const int& cIndex, const Vec4& values) {
        for (int i = 0; i < 4; i++)
            set(i, cIndex, values.get(i));
    }
    Mat4& operator=(const Mat4& mat) {
        for (int i = 0; i < 4; i++)
            setRow(i, mat.getRow(i));
        return *this;
    }
    Mat4 operator*(const Mat4& mat) const {
        Mat4 res;
        for (int i = 0; i < 4; i++)
        	for (int j = 0; j < 4; j++)
            	res.set(i, j, getRow(i) * mat.getColumn(j));
        return res;
    }
    float* toArray() const {
        float* mat = new float[16];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                mat[i * 4 + j] = get(i, j);
        return mat;
    }
    static Mat4 diagonal(const Vec4& vec) {
    	Mat4 res;
    	for (int i = 0; i < 4; i++)
    		res.set(i, i, vec.get(i));
    	return res;
    }
    static Mat4 identity(const float& s = 1) {
    	return diagonal(Vec4(s, s, s, s));
    }
    static void print(const Mat4& mat) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++)
                printf("%f ", mat.get(i, j));
            printf("\n");
        }
        printf("\n");
    }
};

class Geometry {
	unsigned int vertexArrayObject, numberOfVertexes;

public:
	Geometry() {
		vertexArrayObject = numberOfVertexes = 0;
	}
	void setVertexes(const unsigned int& nVtx) {
        numberOfVertexes = nVtx;
	}
	virtual void create() {
        glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);
	}
	void draw() {
		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLES, 0, numberOfVertexes);
	}
	virtual ~Geometry() {}
};

struct VertexData {
    Vec3 pos, norm;
    float u, v;
};

class ParamSurface : public Geometry {
    float maxU, maxV;

public:
    ParamSurface() {
        maxU = maxV = 1;
    }
    virtual Vec3 getPosition(float u, float v) = 0;
    virtual Vec3 getNormal(float u, float v) = 0;
    void setBoundaries(const float& u, const float& v) {
    	maxU = u;
    	maxV = v;
    }
    VertexData generateVertexData(const float& u, const float& v) {
        VertexData vData;
        vData.pos = getPosition(u * maxU, v * maxV);
        vData.norm = getNormal(u * maxU, v * maxV);
        vData.u = u;
        vData.v = v;
        return vData;
    }
    void create(const unsigned int& nTesselation, const unsigned int& mTesselation) {
        unsigned int nVertexes = nTesselation * mTesselation * 6;
        setVertexes(nVertexes);
        Geometry::create();
        unsigned int vertexBufferObject = 0;
        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

        VertexData* vertexData = new VertexData[nVertexes];
        float n = nTesselation;
        float m = mTesselation;
        for (unsigned int i = 0, num = 0; i < nTesselation; i++)
            for (unsigned int j = 0; j < mTesselation; j++) {
                vertexData[num++] = generateVertexData(i / n, j / m);
                vertexData[num++] = generateVertexData((i + 1) / n, j / m);
                vertexData[num++] = generateVertexData(i / n, (j + 1) / m);
                vertexData[num++] = generateVertexData((i + 1) / n, j / m);
                vertexData[num++] = generateVertexData((i + 1) / n, (j + 1) / m);
                vertexData[num++] = generateVertexData(i / n, (j + 1) / m);
            }
        int sVData = sizeof(VertexData);
        int sVec3 = sizeof(Vec3);
        glBufferData(GL_ARRAY_BUFFER, nVertexes * sVData, vertexData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0); 			// AttribArray 0 = POSITION
        glEnableVertexAttribArray(1); 			// AttribArray 1 = NORMAL
        glEnableVertexAttribArray(2); 			// AttribArray 2 = UV
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sVData, reinterpret_cast<void*>(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sVData, reinterpret_cast<void*>(sVec3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sVData, reinterpret_cast<void*>(2 * sVec3));
        delete[] vertexData;
    }
};

class Ellipsoid : public ParamSurface {
	Vec3 radius;

public:
	Ellipsoid(const float& xRadius = 1, const float& yRadius = 1, const float& zRadius = 1) {
		setBoundaries(2 * M_PI, M_PI);
        radius = Vec3(xRadius, yRadius, zRadius);
	}
	Vec3 getPosition(float u, float v) {
		float x, y, z;
		x = radius.x * cosf(u) * sinf(v);
		y = radius.y * sinf(u) * sinf(v);
		z = radius.z * cosf(v);
		return Vec3(x, y, z);
	}
	Vec3 getNormal(float u, float v) {
		float x, y, z = 0;
		x = radius.x * -sinf(u) * sinf(v);
		y = radius.y * cosf(u) * sinf(v);
		Vec3 n_u = Vec3(x, y, z);
		x = radius.x * cosf(u) * cosf(v);
		y = radius.y * sinf(u) * cosf(v);
		z = radius.z * -sinf(v);
		Vec3 n_v = Vec3(x, y, z);
		return (n_v % n_u).normalize();
	}
};

class Sphere : public Ellipsoid {

public:
    Sphere(const float& radius = 1) : Ellipsoid(radius, radius, radius) { }
};

class Torus : public ParamSurface {
	float majorRadius, minorRadius;

public:
	Torus(const float& major = 1, const float& minor = 1) {
		setBoundaries(2 * M_PI, 2 * M_PI);
        majorRadius = major;
        minorRadius = minor;
	}
	Vec3 getPosition(float u, float v) {
		float x = (majorRadius + minorRadius * cosf(u)) * cosf(v);
		float y = (majorRadius + minorRadius * cosf(u)) * sinf(v);
		float z = minorRadius * sinf(u);
		return Vec3(x, y, z);
	}
	Vec3 getNormal(float u, float v) {
		float x, y, z = 0;
		x = -sinf(u) * cosf(v);
		y = -sinf(u) * sinf(v);
        z = cosf(u);
		Vec3 n_u = Vec3(x, y, z);
		x = -sinf(v);
		y = cosf(v);
		z = 0;
		Vec3 n_v = Vec3(x, y, z);
		return (n_u % n_v).normalize();
	}
};

struct Material {
    Vec3 ambient, diffuse, specular;
    float shine;
};

struct Light {
    Vec3 position, color, ambient;
};

class Texture {
	unsigned int id;

public:
	Texture() {
		id = 0;
	}
    unsigned int getId() const {
        return id;
    }
    virtual float* createImage(const int& width, const int& height) {
        float* image = new float[3 * width * height];
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                for (int k = 0; k < 3; k++)
                    image[3 * (i * width + j) + k] = 0;
        return image;
    }
	void create(const int& width, const int& height) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		float* image = createImage(width, height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        delete image;
	}
	virtual ~Texture() {}
};

class ChessBoardTexture : public Texture {
	Vec3 aColor, bColor;

	float* createImage(const int& width, const int& height) {
		float* image = new float[3 * width * height];
		float* c_a = aColor.toArray();
		float* c_b = bColor.toArray();
	    for (int i = 0; i < height; i++)
	        for (int j = 0; j < width; j++) {
	        	float* c;
	        	if ((i + j) % 2 == 0)
	        		c = c_a;
	        	else
	        		c = c_b;
	            for (int k = 0; k < 3; k++)
	                image[3 * (i * width + j) + k] = c[k];
	        }
	    delete c_a;
	    delete c_b;
	    return image;
	}

public:
	ChessBoardTexture(const Vec3& a = Vec3(), const Vec3& b = Vec3()) {
		aColor = a;
		bColor = b;
	}
};

struct RenderState {
	Mat4 modelMatrix, mInvMatrix, viewMatrix, projectionMatrix;
	Vec3 eyePosition;
	Light* light1;
    Light* light2;
	Material* material;
	Texture* texture;

	Mat4 getMVP() {
		return modelMatrix * viewMatrix * projectionMatrix;
	}
};

class ShaderProgram {

    class Shader {
        GLuint id;

    public:
        Shader() {
            id = 0;
        }
        GLuint getId() const {
            return id;
        }
        void create(const GLuint& shaderType, const GLchar* source, const GLchar* name) {
            // Create shader from string
            id = glCreateShader(shaderType);
            if (id == 0) {
                printf("Error in %s creation.\n", name);
                exit(1);
            }
            glShaderSource(id, 1, &source, NULL);
            glCompileShader(id);
            GLint ack;
            glGetShaderiv(id, GL_COMPILE_STATUS, &ack);
            if (!ack) {
                printf("Error in %s creation\n", name);
                getErrorInfo(id, name);
            }
        }
        static void getErrorInfo(const GLuint& handle, const GLchar* name) {
            int logLen;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0) {
                char* log = new char[logLen];
                int written;
                glGetShaderInfoLog(handle, logLen, &written, log);
                printf("%s log:\n%s", name, log);
                delete log;
            }
        }
    };
    GLuint id;

public:
    ShaderProgram() {
        id = 0;
    }
    virtual void bind(RenderState& state) {
        glUseProgram(id);
    }
    virtual void create() {};
    void create(const char* vertexSource, const char* vsAttributeNames[], const unsigned int& vsAttrNum,
                        const char* fragmentSource, const char* fsOutputName) {
        Shader vertexShader, fragmentShader;
        vertexShader.create(GL_VERTEX_SHADER, vertexSource, "vertex shader");
        fragmentShader.create(GL_FRAGMENT_SHADER, fragmentSource, "fragment shader");
        id = glCreateProgram();
        if (id == 0) {
            printf("Error in shader program creation\n");
            exit(1);
        }
        glAttachShader(id, vertexShader.getId());
        glAttachShader(id, fragmentShader.getId());
        for (unsigned int i = 0; i < vsAttrNum; i++)
            glBindAttribLocation(id, i, vsAttributeNames[i]);
        glBindFragDataLocation(id, 0, fsOutputName);
        glLinkProgram(id);
        GLint ack;
        glGetProgramiv(id, GL_LINK_STATUS, &ack);
        if (ack == 0) {
            printf("Failed to link shader program!\n");
            Shader::getErrorInfo(id, "Shaderprogram");
        }
    }
    GLuint getId() const {
        return id;
    }
    int getUniformLocation(const char* name) {
		int loc = glGetUniformLocation(id, name);
		if (loc < 0)
			printf("Uniform %s cannot be set\n", name);
		return loc;
	}
    void setUniform(const char* name, const int& uniform) {
        int loc = getUniformLocation(name);
        glUniform1i(loc, uniform);
    }
	void setUniform(const char* name, const float& uniform) {
		int loc = getUniformLocation(name);
		glUniform1f(loc, uniform);
	}
	void setUniform(const char* name, const Vec3& uniform) {
		int loc = getUniformLocation(name);
		float* vec = uniform.toArray();
        glUniform3fv(loc, 1, &vec[0]);
        delete[] vec;
	}
    void setUniform(const char* name, const Vec4& uniform) {
        int loc = getUniformLocation(name);
        float* vec = uniform.toArray();
        glUniform4fv(loc, 1, &vec[0]);
        delete[] vec;
    }
    void setUniform(const char* name, const Mat4& uniform) {
        int loc = getUniformLocation(name);
        float* mat = uniform.toArray();
        glUniformMatrix4fv(loc, 1, GL_TRUE, &mat[0]);
        delete[] mat;
    }
    void deleteIt() {
        glDeleteProgram(id);
    }
    virtual ~ShaderProgram() {}
};

class PhongShaderProgram : public ShaderProgram {

public:
    PhongShaderProgram() {}
	void create() {
		const char* vertexSource = R"(
			#version 130
			precision highp float;

			uniform mat4 mvp;			    // Model-View-Projection matrix in row-major format
			uniform mat4 m, mInv; 		    // Model, Model-inverse matrix
		    uniform vec4 wLiPosA, wLiPosB;	// Positions of the light sources
            uniform vec3 wEye;              // Position of the camera

			in vec3 vtxPos;				    // Position in modeling space
			in vec3 vtxNorm;                // Normal in modeling space
            in vec2 vtxUV;
            out vec2 texCoord;              // Texture
			out vec3 wNorm;				    // Normal in world space
			out vec3 wView;				    // View in world space
			out vec3 wLightA;
            out vec3 wLightB;  	            // Lights directions in world space

			void main() {
				gl_Position = vec4(vtxPos, 1) * mvp; 		// to NDC
                texCoord = vtxUV;
				vec4 wPos = vec4(vtxPos, 1) * m;
				wLightA = wLiPosA.xyz * wPos.w - wPos.xyz * wLiPosA.w;
                wLightB = wLiPosB.xyz * wPos.w - wPos.xyz * wLiPosB.w;
				wView = wEye * wPos.w - wPos.xyz;
				wNorm = (mInv * vec4(vtxNorm, 0)).xyz;
			})";
		const char* fragmentSource = R"(
			#version 130
			precision highp float;

			uniform vec3 kD, kS, kA;	    // Diffuse, specular and ambient ref
            uniform vec3 lAA, lEA;
			uniform vec3 lAB, lEB;		    // Ambient and point source rad
			uniform float shine;		    // Shininess for specular ref
            uniform sampler2D samplerUnit;

			in vec2 texCoord;
            in vec3 wNorm;				    // Interpolated World space norm
			in vec3 wView;				    // Interpolated World space view
			in vec3 wLightA;
			in vec3 wLightB;			    // Interpolated World space illumination direction
			out vec4 fragmentColor;		    // output that goes to the raster memory

			void main() {
	   			vec3 n = normalize(wNorm);
	   			vec3 v = normalize(wView);  
	   			vec3 l = normalize(wLightA);
	   			vec3 h = normalize(l + v);
	   			float cosT = max(dot(n, l), 0);
	   			float cosD = max(dot(n, h), 0);
                vec3 tex = texture(samplerUnit, texCoord).xyz;
	   			vec3 color1 = (tex + kA) * lAA + ((kD + tex) * cosT + kS * pow(cosD, shine)) * lEA;
                l = normalize(wLightB);
                h = normalize(l + v);
                cosT = max(dot(n, l), 0);
                cosD = max(dot(n, h), 0);
                vec3 color2 = (tex + kA) * lAB + ((kD + tex) * cosT + kS * pow(cosD, shine)) * lEB;
				fragmentColor = vec4(color1 + color2, 1); 			// extend RGB to RGBA
			})";
		const char* vsAttrNames[] = { "vtxPos", "vtxNorm" };
		ShaderProgram::create(vertexSource, vsAttrNames, 2, fragmentSource, "fragmentColor");
	}
	void bindLightA(Light* light) {
		setUniform("wLiPosA", Vec4(light->position, 1));
		setUniform("lEA", light->color);
        setUniform("lAA", light->ambient);
	}
	void bindLightB(Light* light) {
		setUniform("wLiPosB", Vec4(light->position, 1));
		setUniform("lEB", light->color);
        setUniform("lAB", light->ambient);
	}
	void bindMaterial(Material* material) {
		setUniform("kD", material->diffuse);
		setUniform("kS", material->specular);
		setUniform("kA", material->ambient);
		setUniform("shine", material->shine);
	}
	void bindTexture(Texture* texture) {
        int samplerUnit = GL_TEXTURE0;
        setUniform("samplerUnit", samplerUnit);
        glActiveTexture(samplerUnit);
        glBindTexture(GL_TEXTURE_2D, texture->getId());
	}
	void bind(RenderState& state) {
        ShaderProgram::bind(state);
		setUniform("mvp", state.getMVP());
		setUniform("m", state.modelMatrix);
		setUniform("mInv", state.mInvMatrix);
        setUniform("wEye", state.eyePosition);
        Light* light = new Light();
		if (state.light1 != NULL)
			bindLightA(state.light1);
		else
			bindLightA(light);
		if (state.light2 != NULL)
			bindLightB(state.light2);
		else
			bindLightB(light);
		delete light;
		Material* material = new Material();
		if (state.material != NULL)
			bindMaterial(state.material);
		else
			bindMaterial(material);
		delete material;
		Texture* texture = new Texture();
		if (state.texture != NULL)
			bindTexture(state.texture);
		else
			bindTexture(texture);
		delete texture;

	}
};

class Object {

protected:
    Geometry* geometry;
    Material* material;
    Texture* texture;
    ShaderProgram* shader;

public:
    Vec3 position, size, rotationAxis;
    float rotationAngle;

	Object() {
        texture = NULL;
        material = NULL;
        shader = NULL;
        geometry = NULL;
		rotationAngle = 0;
		position = Vec3(0, 0, 0);
        rotationAxis = Vec3(0, 0, 1);
        size = Vec3(1, 1, 1);

	}
    virtual void create() {
    	if (shader != NULL)
    		shader->create();
    }
    void setGeometry(Geometry* geom) {
        geometry = geom;
    }
    void setMaterial(Material* mat) {
        material = mat;
    }
    void setTexture(Texture* tex) {
        texture = tex;
    }
    void setShader(ShaderProgram* shaderprogram) {
    	shader = shaderprogram;
    }
	void setSize(const float& xSize, const float& ySize = 0, const float& zSize = 0) {
		if (ySize > 0 && zSize > 0)
			size = Vec3(xSize, ySize, zSize);
		else
			size = Vec3(xSize, xSize, xSize);
	}
    Mat4 scale(const Vec3& scale) {
        return Mat4::diagonal(Vec4(scale, 1));
    }
    Mat4 rotate(const float& angle, const Vec3& axis) {
        float rAngle = angle * (float)M_PI / 180;
        float cosA = cosf(rAngle);
        float sinA = sinf(rAngle);
        Mat4 rMat = Mat4::identity();
        rMat.setRow(0, Vec4(cosA, -axis.z * sinA, axis.y * sinA, 0));
        rMat.setRow(1, Vec4(axis.z * sinA, cosA, -axis.x * sinA, 0));
        rMat.setRow(2, Vec4(-axis.y * sinA, axis.x * sinA, cosA, 0));
        float v[] = { axis.x, axis.y, axis.z };
        for (int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                rMat.set(i, j, rMat.get(i, j) + v[i] * v[j] * (1 - cosA));
        return rMat;
    }
    Mat4 translate(const Vec3& pos) {
        Mat4 pMat = Mat4::identity();
        pMat.setRow(3, Vec4(pos, 1));
        return pMat;
    }
	void draw(RenderState state) {
		state.modelMatrix = scale(size) * rotate(rotationAngle, rotationAxis) * translate(position);
		state.mInvMatrix = translate(-position) * rotate(-rotationAngle, rotationAxis) * scale(1 / size);
		state.material = material;
		state.texture = texture;
        shader->bind(state);
		geometry->draw();
	}
    virtual void destroy() {
        shader->deleteIt();
    }
    virtual ~Object() {
        destroy();
    }
};

class Camera {

public:
	Vec3 position, lookAt, upDir;
	float fieldOfView, aspect, pFront, pBack;

	Camera() {
		fieldOfView = aspect = pFront = pBack = 0;
        upDir = Vec3(0, 0, 1);
	}
	void setPerspective(const float& fov, const float& asp, const float& front, const float& back) {
		fieldOfView = fov;
		aspect = asp;
		pFront = front;
		pBack = back;

	}
	Mat4 getViewMatrix() const {
		Vec3 dir = (position - lookAt).normalize();
		Vec3 right = (upDir % dir).normalize();
		Vec3 up = dir % right;
		Mat4 v1, v2;
		v1 = v2 = Mat4::identity();
		v1.setRow(3, Vec4(-position, 1));
		v2.setColumn(0, Vec4(right));
        v2.setColumn(1, Vec4(up));
        v2.setColumn(2, Vec4(dir));
		return v1 * v2;
	}
	Mat4 getProjectionMatrix() const {
		float sy = 1 / tanf(fieldOfView * 0.5f);
		Mat4 p;
		p.set(0, 0, sy / aspect);
        p.set(1, 1, sy);
        p.set(2, 2, -(pFront + pBack) / (pBack - pFront));
        p.set(2, 3, -1);
        p.set(3, 2, -2 * pFront * pBack / (pBack - pFront));
		return p;
	}
};

class Scene {
    Camera* camera;
    Light* lights[2];
    Object* objects[10];
    int objectCount, lightCount;
    RenderState state;

public:
    Scene() {
        objectCount = lightCount = 0;
        camera = NULL;
    }
    void setCamera(Camera* cam) {
        camera = cam;
    }
    void addObject(Object* object) {
        if (objectCount < 10)
            objects[objectCount++] = object;
    }
    void addLight(Light* light) {
        if (lightCount < 2)
            lights[lightCount++] = light;
    }
    void animate(const float& dt) {

    }
    void render() {
        state.eyePosition = camera->position;
        state.viewMatrix = camera->getViewMatrix();
        state.projectionMatrix = camera->getProjectionMatrix();
        state.light1 = lights[0];
        state.light2 = lights[1];
        for (int i = 0; i < objectCount; i++)
            objects[i]->draw(state);
    }
    void destroy() {
        for (int i = 0; i < objectCount; i++)
            objects[i]->destroy();
    }
    ~Scene() {
    	destroy();
    }
};

class MainTorus : public Object {

public:
	void create() {
        Torus* torus = new Torus(1, 0.3f);
        torus->create(100, 50);
        geometry = torus;
    	texture = new ChessBoardTexture(Vec3(1, 1, 0), Vec3(0, 0, 0));
    	texture->create(30, 60);
        shader = new PhongShaderProgram();
        Object::create();
	}
	void destroy() {
		shader->deleteIt();
		delete shader;
		delete texture;
		delete geometry;
	}
};

class Ball : public Object {

public:
	void create() {
		Sphere* sphere = new Sphere();
        sphere->create(50, 50);
        geometry = sphere;
    	texture = new ChessBoardTexture(Vec3(1, 0, 0), Vec3(0, 0, 1));
    	texture->create(30, 30);
        shader = new PhongShaderProgram();
        Object::create();
	}
	void destroy() {
		shader->deleteIt();
		delete shader;
		delete texture;
		delete geometry;
	}
};

// The virtual world: collection of objects
Scene scene;
Camera camera;
Light light_a, light_b;
Material silver;
MainTorus torus;
Ball ball;
 
// Initialization, create an OpenGL context
void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

	silver.ambient = Vec3(0.192f, 0.192f, 0.192f);
	silver.diffuse = Vec3(0.508f, 0.508f, 0.508f);
	silver.specular = Vec3(0.508f, 0.508f, 0.508f);
	silver.shine = 51.2f;

    light_a.position = Vec3(5, -35, 6);
    light_a.color = Vec3(0, 1, 1);
    light_a.ambient = Vec3(0.1f, 0.1f, 0.1f);
    light_b.position = Vec3(2, -25, -5);
    light_b.color = Vec3(1, 1, 0);
    light_b.ambient = Vec3(0, 0, 0);

    torus.create();
    torus.setSize(30);
    torus.setMaterial(&silver);

    ball.create();
    ball.setSize(3);
    ball.position = Vec3(21, -23, 3);
    ball.setMaterial(&silver);

    camera.position = Vec3(0, -30, 3);
    camera.lookAt = ball.position;
    camera.setPerspective(45, 1, 2, 200);

    scene.setCamera(&camera);
    scene.addLight(&light_a);
    scene.addLight(&light_b);
    scene.addObject(&ball);
    scene.addObject(&torus);
}

// Window has become invalid: Redraw
void onDisplay() {
    glClearColor(0, 0, 0, 0);                                // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // clear the screen
    scene.render();
    glutSwapBuffers();                                       // exchange the two buffers
}
 
void onExit() {
    scene.destroy();
    printf("exit");
}
// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;
	scene.animate(time);
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
    glutPostRedisplay();
}
 
// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {}
 
// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
    glutPostRedisplay();
}
 
// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {}

// OpenGL major and minor versions
int majorVersion = 3, minorVersion = 0;
 
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
