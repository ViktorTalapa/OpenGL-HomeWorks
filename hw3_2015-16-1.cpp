//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Talapa Viktor György
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
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)                                                                                                                                                                                                            
#include <OpenGL/gl.h>                                                                                                                                                                                                            
#include <OpenGL/glu.h>                                                                                                                                                                                                           
#include <GLUT/glut.h>                                                                                                                                                                                                            
#else                                                                                                                                                                                                                             
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)                                                                                                                                                                       
#include <windows.h>                                                                                                                                                                                                              
#endif                                                                                                                                                                                                                            
#include <GL/gl.h>                                                                                                                                                                                                                
#include <GL/glu.h>                                                                                                                                                                                                               
#include <GL/glut.h>                                                                                                                                                                                                              
#endif          


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

const int sWidth = 600;			// alkalmazás ablak felbontása
const int sHeight = 600;

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z, t;

	Vector() {
		x = y = z = t = 0;
	}
	Vector(float x0, float y0, float z0 = 0, float t0 = 0) {
		x = x0;
		y = y0;
		z = z0;
		t = t0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) { 	// dot product
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) { 	// cross product
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	Vector operator/(float a) {
		return Vector(x / a, y / a, z / a);
	}
	float length() {
		return sqrt(x * x + y * y + z * z);
	}
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color() {
	   r = g = b = 0;
   }
   Color(float r0, float g0, float b0) {
	   r = r0; g = g0; b = b0;
   }
   Color operator*(float a) {
	   return Color(r * a, g * a, b * a);
   }
   Color operator*(const Color& c) {
	   return Color(r * c.r, g * c.g, b * c.b);
   }
   Color operator+(const Color& c) {
	   return Color(r + c.r, g + c.g, b + c.b);
   }
};

struct Math {
	static int fact(int n) {
	    int m = 1;
	    for (int i = 2; i <= n; i++) m*= i;
	    return m;
	}
	static float bin(int n, int k) {
	    return fact(n) / (float)(fact(k) * fact(n - k));
	}
};

class Drawable {
  protected:
	Vector pos, ang;

	virtual void draw() = 0;

  public:
	Drawable() {
		pos = ang = Vector();
	}
	virtual void setPosition(float x, float y, float z) {
		pos = Vector(x, y, z);
	}
	virtual void setPosition(Vector p) {
		pos = p;
	}
	virtual void setRotation(float xAngle, float yAngle, float zAngle) {
		ang = Vector(xAngle, yAngle, zAngle);
	}
	virtual Vector getPosition() {
		return pos;
	}
	virtual Vector getRotation() {
		return ang;
	}
	void Rotate(float xAngle, float yAngle, float zAngle) {
		ang = ang + Vector(xAngle, yAngle, zAngle);
	}
	void setGlMatrix() {
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(ang.x, 1, 0, 0);
		glRotatef(ang.y, 0, 1, 0);
		glRotatef(ang.z, 0, 0, 1);
	}
	virtual ~Drawable() {};
};

class Shape : public Drawable {
  protected:
	float u_max, v_max, i_max, j_max;

	virtual Vector point(float u, float v) = 0;
	virtual Vector normal(float u, float v) = 0;

  public:
	Shape() : Drawable() {
		u_max = v_max = i_max = j_max = 0;
	}
	void drawPoint(float u, float v) {
		Vector p = point(u, v);
		Vector n = normal(u, v);
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(p.x, p.y, p.z);
	}
	void draw() {
		glPushMatrix();
		setGlMatrix();
		glBegin(GL_TRIANGLE_FAN);
		for (int i = 0; i < i_max; i++) {
			float u = i / (i_max - 1) * u_max;
			float ue = u_max / (i_max - 1);
			for (int j = 0; j < j_max; j++) {
				float v = j / (j_max - 1) * v_max;
				float ve = v_max / (j_max - 1);
				drawPoint(u, v);
				drawPoint(u + ue, v);
				drawPoint(u, v + ve);
				drawPoint(u + ue, v + ve);
			}
		}
		glEnd();
		glPopMatrix();
	}
	virtual ~Shape() {};
};

class Ellipsoid : public Shape {
	float a, b, c;

  public:
	Ellipsoid() : Shape() {
		a = b = c = 0;
	}
	Ellipsoid(float xRadius, float yRadius, float zRadius) {
		a = xRadius;
		b = yRadius;
		c = zRadius;
		u_max = 2 * M_PI;
		v_max = M_PI;
		i_max = 100;
		j_max = 100;
	}
	Vector point(float u, float v) {
		float x, y, z;
		x = a * cos(u) * sin(v);
		y = b * sin(u) * sin(v);
		z = c * cos(v);
		return Vector(x, y, z);
	}
	Vector normal(float u, float v) {
		float x, y, z;
		x = a * sin(u) * -sin(v);
		y = b * cos(u) * sin(v);
		z = 0;
		Vector n_u = Vector(x, y, z);
		x = a * cos(u) * cos(v);
		y = b * sin(u) * cos(v);
		z = c * -sin(v);
		Vector n_v = Vector(x, y, z);
		return (n_v % n_u);
	}
};

class Sphere : public Ellipsoid {
	public:
		Sphere() : Ellipsoid() {}
		Sphere(float radius) : Ellipsoid(radius, radius, radius) {}
};

class Cylinder : public Shape {
	float r;

  public:
	Cylinder() : Shape() {
		r = 0;
	}
	Cylinder(float radius, float height) {
		r = radius;
		u_max = 2 * M_PI;
		v_max = height;
		i_max = 100;
		j_max = 100;
	}
	Vector point(float u, float v) {
		float x = r * cos(u);
		float y = v;
		float z = r * sin(u);
		return Vector(x, y, z);
	}
	Vector normal(float u, float v) {
		float x, y, z;
		x = r * -sin(u);
		y = 0;
		z = r * cos(u);
		Vector n_u = Vector(x, y, z);
		x = 0;
		y = 1;
		z = 0;
		Vector n_v = Vector(x, y, z);
		return (n_v % n_u);
	}
};

class Cone : public Shape {
	float a;

  public:
	Cone() : Shape() {
		a = 0;
	}
	Cone(float angle, float height) : Shape() {
		a = angle * M_PI / 180;
		u_max = 2 * M_PI;
		v_max = height;
		i_max = 150;
		j_max = 150;
	}
	Vector point(float u, float v) {
		float x = v * tan(a) * cos(u);
		float y = v;
		float z = v * tan(a) * sin(u);
		return Vector(x, y, z);
	}
	Vector normal(float u, float v) {
		float x, y, z;
		x = v * tan(a) * -sin(u);
		y = 0;
		z = v * tan(a) * cos(u);
		Vector n_u = Vector(x, y, z);
		x = tan(a) * cos(u);
		y = 1;
		z = tan(a) * sin(u);
		Vector n_v = Vector(x, y, z);
		return (n_v % n_u);
	}
};

class Texture {
	public: static void setTexture(Color* texture, int xSize, int ySize) {
		unsigned int id;
    	glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xSize, ySize, 0, GL_RGB, GL_FLOAT, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
};

//Forrás: https://msdn.microsoft.com/en-us/library/windows/desktop/dd373945.aspx
class Material {
  public:
	static void setAmbient(float param1, float param2, float param3, float param4) {
		float k[] = {param1, param2, param3, param4};
		glMaterialfv(GL_FRONT, GL_AMBIENT, k);
	}
	static void setDiffuse(float param1, float param2, float param3, float param4) {
		float k[] = {param1, param2, param3, param4};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, k);
	}
	static void setSpecular(float param1, float param2, float param3, float param4) {
		float k[] = {param1, param2, param3, param4};
		glMaterialfv(GL_FRONT, GL_SPECULAR, k);
	}
	static void setShininess(float param) {
		glMaterialf(GL_FRONT, GL_SHININESS, param);
	}
};

struct Light0 {
	static void setup(Vector pos) {
		glEnable(GL_LIGHTING);
		float position[] = { pos.x, pos.y, pos.z, 0 };
		float diffuse[] = { 1, 1, 0.9, 1 };
		float specular[] = { 1, 1, 0.9, 1 };
		float ambient[] = { 0.3, 0.3, 0.27, 1 };
		float shine = 0;
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightf(GL_LIGHT0, GL_SHININESS, shine);
		glEnable(GL_LIGHT0);
	}
};

struct Shadow {
	static void setup(Vector pos) {
		float matrix[4][4] = {
				{ 1, 0, 0, 0 },
				{ 0, 1, 0, 0 },
				{ -pos.x / pos.y, -pos.y / pos.z, 0, 0 },
				{ 0, 0, 0.1, 1 }
		};
		glMultMatrixf(&matrix[0][0]);
		glDisable(GL_LIGHTING);
		glColor3f(0, 0, 0);
	}
};

class Field : public Material, Texture {
	public: void draw() {
		glEnable(GL_TEXTURE_2D);
		Color texture[] = {
				Color(0.004f, 0.554f, 0.055f),
				Color(0, 0.48f, 0.047f),
				Color(0, 0.48f, 0.047f),
				Color(0.004f, 0.554f, 0.055f)
		};
		setTexture(texture, 2, 2);
		setAmbient(1, 1, 1, 1);
		setDiffuse(1, 1, 1, 1);
		setSpecular(1, 1, 1, 1);
		setShininess(0);
		glBegin(GL_QUADS); {
			glNormal3f(0, 1, 0);
			glTexCoord2f(0, 0);
			glVertex3f(-500, 500, 0);
			glTexCoord2f(5, 0);
			glVertex3f(500, 500, 0);
			glTexCoord2f(0, 5);
			glVertex3f(500, -500, 0);
			glTexCoord2f(5, 5);
			glVertex3f(-500, -500, 0);
		} glEnd();
		glDisable(GL_TEXTURE_2D);
	}
};

class Bomb : public Sphere, Material {
	enum BombState {
		IDLE = 1,
		FALL = 2,
		EXPLODE = 3
	} state;
	float time;
	Vector oPos;

  public:
	Bomb() : Sphere(10) {
		state = IDLE;
		time = 0;
		oPos = pos;
	}
	void setFalling(long fallTime) {
		state = FALL;
		time = fallTime / 1000.0f;
		oPos = pos;
	}
	short setup() {
		float cTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		switch (state) {
		case FALL:
			pos.z -= 4.905f * pow(time - cTime, 2);
			if (pos.z <= 10) {
				state = EXPLODE;
				time = cTime;
			} break;
		case EXPLODE:
			if (cTime - time >= 0.1f) {
				state = IDLE;
				pos = oPos;
			}
			break;
		case IDLE:
			oPos = pos;
			time = 0;
		}
		return state;
	}
	short getState() {
		return state;
	}
	void draw(bool shadow = false) {
		setup();
		if (state == EXPLODE && !shadow) {
			Sphere explosion = Sphere(10 + glutGet(GLUT_ELAPSED_TIME) - time * 1000);
			explosion.setPosition(pos.x, pos.y, pos.z);
			explosion.setRotation(ang.x, ang.y, ang.z);
			setAmbient(1, 0.2f, 0, 1);
			setDiffuse(1, 0.6f, 0, 1);
			setSpecular(1, 0.8, 0, 1);
			setShininess(1000);
			explosion.draw();
		} else {
			setAmbient(0, 0, 0, 1);
			setDiffuse(0.4f, 0.4f, 0.4f, 1);
			setSpecular(0.4f, 0.4f, 0.4f, 1);
			setShininess(10);
			Sphere::draw();
		}
	}
};

class Spline {
  protected:
	Vector cps[8];
	int n;

  public:
	Spline(int size = 1) {
		n = size;
	}
	void setCP(int index, Vector element) {
		cps[index] = element;
	}
	void setCP(int index, float x, float y, float z) {
		cps[index] = Vector(x, y, z);
	}
	Vector getCP(int index) {
		return cps[index];
	}
	int getSize() {
		return n;
	}
};

class ClosedBezier : public Spline {
  public:
	ClosedBezier(int size = 1) : Spline(size) {}

	//Forrás: https://hu.wikipedia.org/wiki/Bézier-görbe
	Vector getP(float t) {
		Vector p, q;
		for (int i = 0; i <= n; i++) {
			if (i == n) q = cps[0];
			else q = cps[i];
			p = p + q * Math::bin(n, i) * pow(t, i) * pow(1 - t, n - i);
		}
		return p;
	}
};

class CatmullRom : public Spline {
	Vector v[8];
	float ts[8];

  public:
	CatmullRom(int size = 2) : Spline(size) {}
	void setup() {
		ts[0] = 0;
		for (int i = 1; i < n - 1; i++) {
			ts[i] = (cps[i].t - cps[0].t) / (cps[n - 1].t - cps[0].t);
			Vector a = (cps[i + 1] - cps[i]) / (ts[i + 1] - ts[i]);
			Vector b = (cps[i] - cps[i - 1]) / (ts[i] - ts[i - 1]);
			v[i] = (a + b) * 0.5;
		}
		ts[n - 1] = 1;
	}
	Vector getA(int i) {
		float t = ts[i + 1] - ts[i];
		return (cps[i] - cps[i + 1]) * 2 / pow(t, 3) + (v[i + 1] + v[i]) / pow(t, 2);
	}
	Vector getB(int i) {
		float t = ts[i + 1] - ts[i];
		return (cps[i + 1] - cps[i]) * 3 / pow(t, 2) - (v[i + 1] + v[i] * 2) / t;
	}
	Vector getP(int i, float t) {
		Vector a = getA(i);
		Vector b = getB(i);
		Vector c = v[i];
		Vector d = cps[i];
		return a * pow(t - ts[i], 3) + b * pow(t - ts[i], 2) + c * (t - ts[i]) + d;
	}
	Vector getN(int i, float t) {
		Vector a = getA(i);
		Vector b = getB(i);
		Vector c = v[i];
		return a * 3 * pow(t - ts[i], 2) + b * 2 * (t - ts[i]) + c;
	}
	float getT(int i) {
		return ts[i];
	}
};

class Csirguru : public Drawable, Material {

	class Head : public Drawable, Material {
		Ellipsoid skull;
		Sphere eye1, eye2;
		Cone beak;
		Cone rowels[3];

	  public:
		Head() : Drawable() {
			skull = Ellipsoid(10, 12, 10);
			eye1 = eye2 = Sphere(2);
			beak = Cone(18, 15);
			eye1.setPosition(3, 8, 5);
			eye2.setPosition(-3, 8, 5);
			beak.setPosition(0, 23, 0);
			beak.setRotation(0, 0, 180);
			rowels[0] = rowels[1] = rowels[2] = Cone(8, 5);
			rowels[0].setPosition(0, -3, 14);
			rowels[1].setPosition(0, 0, 15);
			rowels[2].setPosition(0, 3, 14);
			rowels[0].setRotation(-85, 0, 0);
			rowels[1].setRotation(-90, 0, 0);
			rowels[2].setRotation(-95, 0, 0);
		}
		void draw() {
			glPushMatrix();
			setGlMatrix();
			setAmbient(1, 1, 1, 1);
			setDiffuse(1, 1, 1, 1);
			setSpecular(1, 1, 1, 1);
			setShininess(1);
			eye1.draw();
			eye2.draw();
			setAmbient(0.3f, 0.15f, 0, 1);
			setDiffuse(0.543f, 0.27f, 0.074f, 1);
			setSpecular(0.543f, 0.27f, 0.074f, 1);
			skull.draw();
			setAmbient(0.7f, 0, 0, 1);
			setDiffuse(1, 0, 0, 1);
			setSpecular(1, 0, 0, 1);
			for (int i = 0; i < 3; i++ ) rowels[i].draw();
			setAmbient(0.5f, 0.42f, 0, 1);
			setDiffuse(1, 0.84f, 0, 1);
			setSpecular(1, 0.84f, 0, 1);
			setShininess(10);
			beak.draw();
			glPopMatrix();
		}
	};

	class Body : public Drawable {
		CatmullRom crs[8];
		ClosedBezier bez[8];
		int bNum, cNum;

	  public:
		Body(Vector head = Vector()) : Drawable() {
			bNum = 4;
			cNum = 6;
			for (int i = 0; i < bNum; i++) bez[i] = ClosedBezier(cNum);
			for (int i = 0; i < cNum; i++) crs[i] = CatmullRom(bNum + 1);
			bez[0].setCP(0, 0, 0, 0);
			bez[0].setCP(1, 15, 0, 10);
			bez[0].setCP(2, 10, 0, 20);
			bez[0].setCP(3, 0, 0, 30);
			bez[0].setCP(4, -10, 0, 20);
			bez[0].setCP(5, -15, 0, 10);
			for (int i = 0; i < cNum; i++){
				Vector temp = bez[0].getCP(i);
				bez[1].setCP(i, 1.2 * temp.x, 10, 1.4 * temp.z - 10);
				bez[2].setCP(i, 1.5 * temp.x, 50, 1.7 * temp.z - 15);
				bez[3].setCP(i, 0.5 * temp.x, 80, 0.5 * temp.z + 10);
			}
			for (int i = 0; i < cNum; i++) {
				crs[i].setCP(0, head);
				for (int j = 1; j <= bNum; j++) {
					Vector p = bez[j - 1].getP(i / (float)cNum);
					p.t = j;
					crs[i].setCP(j, p);
				}
			}
		}
		Vector getHip() {
			Vector a = crs[1].getCP(3);
			Vector b = crs[5].getCP(3);
			return (a + b) / 2;
		}
		void draw() {
			glPushMatrix();
			setGlMatrix();
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i < cNum; i++) {
				int j, k, l;
				j = k = l = 0;
				if (i != cNum - 1) j = i + 1;
				crs[i].setup();
				crs[j].setup();
				for (float t = 0; t < 1; t += 0.001f) {
					Vector p = crs[i].getP(k, t);
					Vector q = crs[i].getN(k, t);
					glVertex3f(p.x, p.y, p.z);
					glNormal3f(q.x, q.y, q.z);
					p = crs[j].getP(l, t);
					q = crs[j].getN(l, t);
					glVertex3f(p.x, p.y, p.z);
					glNormal3f(q.x, q.y, q.z);
					if (crs[i].getT(k + 1) <= t) k++;
					if (crs[j].getT(l + 1) <= t) l++;
				}
			}
			glEnd();
			glPopMatrix();
		}
	};

	enum State {
		IDLE = 0,
		CROUCHING = 1,
		STANDING = 2,
		JUMPING = 3,
		EXPLODING = 4
	} state;
	Body body;
	Cylinder thigh, shin, foot;
	Cone toe;
	Head head;
	Sphere knee, ankle, phal;
	Vector defPos;
	Vector start;

  public:
	Csirguru() {
		Vector cHead = Vector(0, -5, 15, 0);
		head.setPosition(cHead.x, cHead.y, cHead.z);
		head.setRotation(20, 0, 180);
		body = Body(cHead);
		Vector p = body.getHip();
		float length = 25;
		thigh = shin = foot = Cylinder(3, length);
		knee = ankle = phal =  Sphere(3);
		thigh.setPosition(p);
		thigh.setRotation(-135, 0, 0);
		knee.setPosition(p.x, p.y - length * cosf(M_PI / 4), p.z - length * sinf(M_PI / 4));
		ankle.setPosition(p.x, p.y, p.z - length * sqrtf(2));
		phal.setPosition(p.x, p.y - length * cosf(M_PI / 4), p.z - length * sqrtf(2) - length * sinf(M_PI / 4));

		shin.setPosition(knee.getPosition());
		shin.setRotation(-45, 0, 0);
		foot.setPosition(ankle.getPosition());
		foot.setRotation(-135, 0, 0);
		toe = Cone(atanf(3 / length) * 180 / M_PI, length);
		p = phal.getPosition();
		toe.setPosition(p.x, p.y - length, p.z);
		defPos = phal.getPosition() * -1;
		defPos.z += 3;
		pos = defPos;

		state = IDLE;
		start = knee.getPosition();
		start.t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	}
	void setPosition(float x, float y, float z) {
		pos = defPos + Vector(x, y, z);
	}
	void setPosition(Vector p) {
		pos = defPos + p;
	}
	Vector getPosition() {
		return pos - defPos;
	}
	int getState(Bomb bomb) {
		float z, t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		Vector temp;
		if (bomb.getState() == 3 && (bomb.getPosition() - pos).length() < 50) {
			start = bomb.getPosition();
			start.t = t;
			state = EXPLODING;
		}
		switch (state) {
		case CROUCHING:
			//Todo...
			break;

		case STANDING:
			z = start.z + 5 * pow(t - start.t, 2);
			if (z - 70 >= phal.getPosition().z) state = JUMPING;
			else {
				temp = knee.getPosition();
				temp.z = z;
				knee.setPosition(temp);
				shin.setPosition(temp);
				//Todo...
			}
			break;

		case JUMPING:
			//Todo...
			break;

		case EXPLODING:

			head.setRotation(0, 0, head.getRotation().z + 15);
			break;
		case IDLE:
			;
		}
		return state;
	}
	void draw() {
		glPushMatrix();
		setGlMatrix();
		head.draw();
		setAmbient(0.3f, 0.15f, 0, 1);
		setDiffuse(0.543f, 0.27f, 0.074f, 1);
		setSpecular(0.543f, 0.27f, 0.074f, 1);
		setShininess(1);
		body.draw();
		setAmbient(0.3f, 0.3f, 0, 1);
		setDiffuse(0.5f, 0.5f, 0, 1);
		setSpecular(0.6f, 0.6f, 0, 1);
		setShininess(1);
		thigh.draw();
		shin.draw();
		knee.draw();
		foot.draw();
		ankle.draw();
		toe.draw();
		phal.draw();
		glPopMatrix();
	}
};

Vector camera, focus, sun;
Field field;
Csirguru csGurus[3];
Bomb bomb;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glViewport(0, 0, sWidth, sHeight);
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 2, 500);
	camera = Vector(100, 0, 50);
	focus = Vector(0, 0, 50);
    sun = Vector(50, 50, 15);
    bomb.setPosition(0, 0, 200);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
    glClearColor(0.527f, 0.805f, 0.977f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.x, camera.y, camera.z, focus.x, focus.y, focus.z, 0, 0, 1);

    Light0::setup(sun);
    field.draw();
    bomb.setup();
    bomb.draw();
    /*
    long time = glutGet(GLUT_ELAPSED_TIME);
    for (int i = 0; i < 3; i++)
    	if (time >= i * 1000) {
    		csGurus[i].draw();
    		csGurus[i].setRotation(0, 0, i * 30);
    	}
   	*/
    csGurus[0].draw();
    glDisable(GL_DEPTH_TEST);
    Shadow::setup(sun);
    bomb.draw(true);
    /*
    for (int i = 0; i < 3; i++)
       	if (time >= i * 1000)
       		csGurus[i].draw();
       		csGurus[i].setRotation(0, 0, i * 30);
    */
    csGurus[0].draw();
    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	Vector bPos = bomb.getPosition();
	if (bomb.getState() == 1) {
		switch (key) {
		case 'w':
			bomb.setPosition(bPos.x, bPos.y + 1, bPos.z);
			break;
		case 'y':
			bomb.setPosition(bPos.x, bPos.y - 1, bPos.z);
			break;
		case 'a':
			bomb.setPosition(bPos.x - 1, bPos.y, bPos.z);
			break;
		case 'd':
			bomb.setPosition(bPos.x + 1, bPos.y, bPos.z);
			break;
		case ' ':
			bomb.setFalling(glutGet(GLUT_ELAPSED_TIME));
			break;
		}
	}
	glutPostRedisplay();
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
	glutPostRedisplay();
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	glutPostRedisplay();
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y) {
	glutPostRedisplay();
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {
	glutPostRedisplay();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok

    return 0;
}

