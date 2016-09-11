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
// Nev    : Talapa Viktor Gy�rgy
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

const int sWidth = 600;
const int sHeight = 600;

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
class Vector {
  public:
	float x, y, z;
	long t;

	Vector() {
		x = 0; y = 0; z = 0; t = 0;
	}
	Vector(float x0, float y0, float z0 = 0, long t0 = 0) {
		x = x0; y = y0; z = z0; t = t0;
	}
    Vector operator=(const Vector& v) {
    	x = v.x; y = v.y; z = v.z; t = v.t; return v;
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
	float operator*(const Vector& v) {
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) {
		return Vector(y  *v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
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
class Color {
  public:
	float r, g, b;

    Color() {
		r = 0; g = 0; b = 0;
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

class Window {
  private:
	float x0, x1, y0, y1;
	int mode;

  public:
	Window() {
		x0 = 0; y0 = 0; x1 = 0; y1 = 0; mode = 0;
	}
	Window(float x, float w, float y, float h, int m) {
		x0 = x; x1 = w; y0 = y;  y1 = h; mode = m;
	}
	Vector to2D(const Vector& v) {
		float x, y;
		switch (mode) {
		  case 0:
			x = v.x;
			y = v.y;
			break;
		  case 1:
			x = v.z;
			y = v.y;
			break;
		  case 2:
			x = v.x;
			y = v.z;
			break;
		  default:
			x = y = 0;
		}
		return Vector(x, y);
	}
	Vector to3D(const Vector& v) {
		float x, y, z;
		switch (mode) {
		  case 0:
			x = v.x;
			y = v.y;
			z = 0;
			break;
		  case 1:
			x = 0;
			y = v.y;
			z = v.x;
			break;
		  case 2:
			x = v.x;
			y = 0;
			z = v.y;
			break;
		  default:
			  x = y = z = 0;
		}
		return Vector(x, y, z);
	}
	Vector convertPixToM(const Vector& v) {
		float x = x0 + v.x * (x1 - x0) / sWidth;
		float y = y0 + (sHeight - v.y) * (y1 - y0) / sHeight;
		return to3D(Vector(x, y));
	}
	Vector convertMToPix(const Vector& v) {
		Vector u = to2D(v);
		float x = (u.x - x0) * sWidth / (x1 - x0);
		float y = (u.y - y0) * sHeight / (y1 - y0);
		return Vector(x, y);
	}
	void moveNearestPoint(Vector* cPoints, const int& size, const Vector& pos) {
		Vector min_v = to2D(cPoints[0]) - pos;
		int min_i = 0;
		for (int i = 1; i < size; i++) {
			Vector temp = to2D(cPoints[i]) - pos;
			if (temp.length() < min_v.length()) {
				min_v = temp;
				min_i = i;
			}
		}
		switch (mode) {
		  case 0:
		    cPoints[min_i].x = pos.x;
			cPoints[min_i].y = pos.y;
			break;
		  case 1:
		    cPoints[min_i].y = pos.y;
			cPoints[min_i].z = pos.z;
			break;
		  case 2:
		  	cPoints[min_i].x = pos.x;
		  	cPoints[min_i].z = pos.z;
		}

	}
	//Forr�s: http://blog.johnryding.com/post/3087060819/opengl-circles
	void drawPointCircle(const Vector& point, float radius) {
	    Vector center = convertMToPix(point);
		int lines = 180;
	    glBegin(GL_POLYGON);
	        for (int i= 0; i <= lines; i++) {
	            float x = center.x + radius * cos(i * 2 * M_PI / lines);
	            float y = center.y + radius * sin(i * 2 * M_PI / lines);
	            glVertex2f(x, y);
	        }
	    glEnd();
	}
	float A(Vector a, Vector b, Vector c) {
		return ((b - a)%(c - b)).length() / 2;
	}
	Vector* setupRennerSpline(Vector* r, Vector* v, const int& n) {
		if (n < 2) return v;
		r[0].t = 0;
		for (int i = 1; i < n; i++) r[i].t = r[i - 1].t + (r[i] - r[i - 1]).length();
		if (n > 3) {
			float a[2] = { 0, 0 };
			for (int i = 1; i < n - 1; i++) {
				a[0] = A(r[i - 2], r[i - 1], r[i]);
				if (i < n - 2) a[1] = A(r[i], r[i + 1], r[i + 2]);
				else a[1] = 0;
				v[i] = ((r[i] - r[i - 1]) * a[1] + (r[i + 1] - r[i]) * a[0]) / (a[1] + a[0]);
			}
		}
		return v;
	}
	bool drawHermite(Vector* r, Vector* v, const int& n, const long& bTime, Vector& bc, Vector& bv) {
		int i = 0;
		bool ball = false;
		glBegin(GL_LINE_STRIP);
		for (float t = 0; t <= r[n - 1].t; t += 0.01) {
		    float td = r[i + 1].t - r[i].t;
			Vector a_0 = r[i];
			Vector a_1 = v[i];
		    Vector a_2 = (r[i + 1] - r[i]) * 3 / pow(td, 2) - (v[i + 1] + v[i] * 2) / td;
		    Vector a_3 = (v[i + 1] + v[i]) / pow(td, 2) - (r[i + 1] - r[i]) * 2 / pow(td, 3);
		    td = t - r[i].t;
		    Vector u = a_3 * pow(td, 3) + a_2 * pow(td, 2) + a_1 * td + a_0;
		    if (fabs(u.x) <= 3 && fabs(u.y) <= 3 && fabs(u.z) <= 3) {
			    if (canDrawBall(t, bTime)) {
			    	bc = u;
			    	bv = a_3 * 3 * pow(td, 2) + a_2 * 2 * td + a_1;
			    	ball = true;
			    }
		    	u = convertMToPix(u);
			    glVertex2f(u.x, u.y);
		    }
		    if (r[i + 1].t < t) i++;
		}
		glEnd();
		return ball;
	}
	bool canDrawBall(float s, long t0) {
		float td = (glutGet(GLUT_ELAPSED_TIME) - t0) / 10;
		if (round(s * 100) == td) return true;
		return false;
	}
	void drawBallTail(Vector& p, const Vector& v) {
		drawPointCircle(p, 5);
		glBegin(GL_LINE_STRIP);
			Vector u = convertMToPix(p);
			glVertex2f(u.x, u.y);
			u = convertMToPix(p - v);
			glVertex2f(u.x, u.y);
		glEnd();
	}
};

class WindowSelector {
  public:
	bool firstClick;
	int index;

  	WindowSelector() {
		firstClick = true; index = 0;
	}
	void setSelected(int i) {
		firstClick = false; index = i;
	}
};

Color image[sWidth * sHeight];
Vector cPoints[100];
int cpSize = 0;
long bTime = 0;
Window windows[3];
WindowSelector wSelector;

void onInitialization() {
	glViewport(0, 0, sWidth, sHeight);
	gluOrtho2D(0, sWidth, 0, sHeight);
	windows[0] = Window(-3, 9, -9, 3, 0);
	windows[1] = Window(-9, 3, -9, 3, 1);
	windows[2] = Window(-3, 9, -3, 9, 2);
}


void onDisplay() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels(sWidth, sHeight, GL_RGB, GL_FLOAT, image);
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	  glVertex2f(sWidth / 2, 0);
	  glVertex2f(sWidth / 2, sHeight);
	  glVertex2f(0, sHeight / 2);
	  glVertex2f(sWidth, sHeight / 2);
	glEnd();
	for (int i = 0; i < 3; i++) {
		glColor3f(1, 1, 0);
		for (int j = 0; j < cpSize; j++) windows[i].drawPointCircle(cPoints[j], 3);
		glColor3f(1, 1, 1);
		Vector vectors[cpSize];
		Vector ballCenter, ballVector;
		windows[i].setupRennerSpline(cPoints, vectors, cpSize);
		bool ball = windows[i].drawHermite(cPoints, vectors, cpSize, bTime, ballCenter, ballVector);
		glColor3f(1, 0, 0);
		if (bTime != 0 && ball) windows[i].drawBallTail(ballCenter, ballVector);
	}
    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
	if (key == ' ') bTime = glutGet(GLUT_ELAPSED_TIME);
	glutPostRedisplay();
}

void onKeyboardUp(unsigned char key, int x, int y) {
	glutPostRedisplay();
}


void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
    	int i;
    	int sx = sWidth / 2;
    	int sy = sHeight / 2;
    	if (x < sx && y < sy) i = 0;
    	else if (x > sx && y < sy) i = 1;
    	else if (x < sx && y > sy) i = 2;
    	else i = 3;
    	if (i < 3) {
    		Vector click = windows[i].convertPixToM(Vector(x, y));
    		if (wSelector.firstClick || (wSelector.index == i && cpSize < 100)) {
    			cPoints[cpSize] = click;
    			cpSize++;
    			if (wSelector.firstClick) wSelector.setSelected(i);
    		} else windows[i].moveNearestPoint(cPoints, cpSize, click);
    	}
    }
}

void onMouseMotion(int x, int y) {
	glutPostRedisplay();
}

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

