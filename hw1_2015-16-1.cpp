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

/*
 * A feladat megoldásához felhasználtam előző két félévben készített első
 * házi feladataim forrásait, ezeket külön nem jelöltem meg.
 */

const int sWidth = 600;			// alkalmazás ablak felbontása
const int sHeight = 600;

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z;
	float t;

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
	Vector rotate(const Vector& cent, const float& ang) {
		float x0 = (cent.x - x) * cos(ang) - (cent.y - y) * sin(ang) + cent.x;
		float y0 = (cent.x - x) * sin(ang) + (cent.y - y) * cos(ang) + cent.y;
		return Vector(x0, y0);
	}
};

class Window {
	Vector start, end;
	float time;
	bool move;
	Vector speed;

  public:
	Window() {
		start = end = speed = Vector();
		move = false;
		time = 0;
	}
	void setSize(const float& xStart, const float& xEnd, const float& yStart, const float& yEnd) {
		start = Vector(xStart, yStart);
		end = Vector(xEnd, yEnd);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(xStart, xEnd, yStart, yEnd);
	}
	void setTime(const float& t) {
		time = t;
	}
	void setMove(const bool& m) {
		move = m;
	}
	void setSpeed(const float& xSpeed, const float& ySpeed)  {
		speed = Vector(xSpeed, ySpeed);
	}
	Vector getStart() {
		return start;
	}
	Vector getEnd() {
		return end;
	}
	bool isMoving() {
		return move;
	}
	Vector convertPixToM(const Vector& pPos) {
		float x = start.x + pPos.x * (end.x - start.x) / sWidth;
		float y = start.y + (sHeight - pPos.y) * (end.y - start.y) / sHeight;
		return Vector(x, y);
	}
	Vector convertMToPix(const Vector& mPos) {
		float x = (mPos.x - start.x) * sWidth / (end.x - start.x);
		float y = (mPos.y - start.y) * sHeight / (end.y - start.y);
		return Vector(x, y);
	}
	void calculateMove(float& start, float& end, float& speed, const float& cTime) {
  		float s = speed * (cTime - time);
  		if (end + s > 1000) {
  			s = 2000 - 2 * end - s;
  			speed *= -1;
  		} else if (start + s < 0) {
  			s = -2 * start - s;
  			speed *= -1;
  		}
  		start += s;
  		end += s;
  	}
  	void moveIt(const float& cTime) {
		calculateMove(start.x, end.x, speed.x, cTime);
		calculateMove(start.y, end.y, speed.y, cTime);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(start.x, end.x, start.y, end.y);
		time = cTime;
	}
};

//Forrás: http://blog.johnryding.com/post/3087060819/opengl-circles
class Circle {
	Vector cent;
	float rad;

  public:
	Circle() {
		cent = Vector();
		rad = 0;
	}
	Circle(const Vector& center, const float& radius) {
		cent = center;
		rad = radius;
	}
	void draw() {
		int lines = 180;
		glBegin(GL_POLYGON);
		for (int i = 0; i <= lines; i++) {
			float x = cent.x + rad * cos(i * 2 * M_PI / lines);
			float y = cent.y + rad * sin(i * 2 * M_PI / lines);
			glVertex2f(x, y);
		}
		glEnd();
	}
};

class Line {
	Vector pnt, norm;

  public:
	Line() {
		pnt = norm = Vector();
	}
	Line(const Vector& point) {
		pnt = point;
		norm = Vector();
	}
	Line(const Vector& aPoint, const Vector& bPoint) {
		pnt = aPoint;
		setNormal(aPoint, bPoint);
	}
	void setNormal(const Vector& n) {
		norm = n;
	}
	void setNormal(const Vector& a,  const Vector& b) {
		norm = Vector(b.y - a.y, a.x - b.x);
	}
	Vector getNormal() {
		return norm;
	}
	float getSum() {
		return pnt.x * norm.x + pnt.y * norm.y;
	}
	bool isPointOn(const Vector& p) {
		return (p.x * norm.x + p.y * norm.y == getSum());
	}
	Vector getCrossPoint(Line l) {
		float x = (getSum() * l.norm.y - l.getSum() * norm.y) / (norm.x * l.norm.y - norm.y * l.norm.x);
		float y = (getSum() - norm.x * x) / norm.y;
		return Vector(x, y);
	}
	float getDistanceTo(const Vector& p) {
		return abs(norm.x * p.x + norm.y * p.y - getSum()) / norm.length();
	}
	void draw(Window win) {
  		float sum = getSum();
  		Vector start = win.getStart();
  		Vector end = win.getEnd();
		glBegin(GL_LINE_STRIP);
  			float z = (sum - start.y * norm.y) / norm.x;
  			if (z >= start.x)
  				glVertex2f(z, start.y);
  			z = (sum - start.x * norm.x) / norm.y;
  			if (z >= start.y)
  				glVertex2f(start.x, z);
  			z = (sum - end.y * norm.y) / norm.x;
  			if (z <= end.x)
  				glVertex2f(z, end.y);
  			z = (sum - end.x * norm.x) / norm.y;
  			if (z <= end.y)
  				glVertex2f(end.x, z);
  		glEnd();
  	}
};

//Forrás: https://hu.wikipedia.org/wiki/Parabola_%28g%C3%B6rbe%29
class Parabola {
	Vector foc, vert;
	float angle, dist;
	Line dix;

  public:
	Parabola() {
		foc = vert = Vector();
		dix = Line();
		angle = 0;
		dist = 0;
	}
	Parabola(Vector& aDix, Vector& bDix, Vector& focus) {
		foc = focus;
		dix = Line(aDix, bDix);
		Vector nDix = dix.getNormal();
		Line focL = Line(foc);
		focL.setNormal(Vector(nDix.y, -nDix.x));
		vert = (foc + dix.getCrossPoint(focL)) / 2;
		angle = atan(-nDix.x / nDix.y);
		dist = dix.getDistanceTo(foc) / 2;
		if (foc.y > vert.y) dist *= -1;
	}
	Vector rotate(Vector p, const short& n = 1) {
		return p.rotate(vert, angle * n);
	}
	void draw() {
		glBegin(GL_POLYGON);
		for (float t = -25; t <= 25; t += 0.01f) {
			float x = 2 * dist * t + vert.x;
			float y = dist * pow(t, 2) + vert.y;
			Vector p = rotate(Vector(x, y));
			glVertex2f(p.x, p.y);
		}
		glEnd();
	}
	bool isCrossAround(Vector& p, Vector& q) {
		float pDist = dix.getDistanceTo(p);
		float qDist = dix.getDistanceTo(q);
		return ((p - foc).length() >= pDist && (q - foc).length() <= qDist) ||
				((q - foc).length() < pDist && (q - foc).length() > qDist);
	}
	void drawTangent(Vector& cr, Window& win) {
		Line crL = Line(cr);
		crL.setNormal(Vector(dix.getNormal().y, -dix.getNormal().x));
		Vector tanP = (foc + dix.getCrossPoint(crL)) * 0.5;
		Line tan = Line(cr, tanP);
		tan.draw(win);
	}
};

class Spline {
	Vector* cp;
	int cps;
	Vector v[11];
	float ts[11];

  public:
	void setup(Vector* cPoints, const int& cpSize) {
		cp = cPoints;
		cps = cpSize;
		ts[0] = 0;
		for (int i = 1; i < cps - 1; i++) {
			ts[i] = (cp[i].t - cp[0].t) / (cp[cps - 1].t - cp[0].t);
			Vector a = (cp[i + 1] - cp[i]) / (ts[i + 1] - ts[i]);
			Vector b = (cp[i] - cp[i - 1]) / (ts[i] - ts[i - 1]);
			v[i] = (a + b) * 0.5;
		}
		ts[cps - 1] = 1;
	}
	Vector getA(const int& i) {
		float t = ts[i + 1] - ts[i];
		return (cp[i] - cp[i + 1]) * 2 / pow(t, 3) + (v[i + 1] + v[i]) / pow(t, 2);
	}
	Vector getB(const int& i) {
		float t = ts[i + 1] - ts[i];
		return (cp[i + 1] - cp[i]) * 3 / pow(t, 2) - (v[i + 1] + v[i] * 2) / t;
	}
	Vector getSPoint(const int& i, const float& t) {
		Vector a = getA(i);
		Vector b = getB(i);
		Vector c = v[i];
		Vector d = cp[i];
		return a * pow(t - ts[i], 3) + b * pow(t - ts[i], 2) + c * (t - ts[i]) + d;
	}
	Vector getLNormal(const int& i, const float& t) {
		Vector a = getA(i);
		Vector b = getB(i);
		Vector c = v[i];
		Vector dir = a * 3 * pow(t - ts[i], 2) + b * 2 * (t - ts[i]) + c;
		return Vector(dir.y, -dir.x);
	}

	void draw() {
		setup(cp, cps);
		glBegin(GL_LINE_STRIP);
		int i = 0;
		for (float t = 0; t < 1; t += 0.001f) {
			Vector p = getSPoint(i, t);
			glVertex2f(p.x, p.y);
			if (ts[i + 1] <= t) i++;
		}
		glEnd();
	}
	Vector getCross(Parabola& pb) {
		setup(cp, cps);
		int i = 1;
		Vector p0 = getSPoint(1, ts[1]);
		for (float t = ts[1]; t < 1; t += 0.0001f) {
			Vector p = getSPoint(i, t);
			if (pb.isCrossAround(p0, p)) {
				Vector cr = (p0 + p) / 2;
				cr.t = t;
				return cr;
			}
			p0 = p;
			if (ts[i + 1] <= t) i++;
		}
		return Vector();
	}
	void drawTangent(Vector& cr, Window& win) {
		int i = 0;
		for (int j = 0; j < cps; j++)
			if (ts[j] < cr.t) i = j;
		Line l = Line(cr);
		l.setNormal(getLNormal(i, cr.t));
		l.draw(win);
	}
};

Vector controlPoints[11];
int cpSize = 0;
Window window;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glViewport(0, 0, sWidth, sHeight);
	window.setSize(0, 1000, 0, 1000);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
	glClearColor(0.254f, 0.895f, 0.965f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 		// kepernyo torles
	if (window.isMoving())
		window.moveIt(glutGet(GLUT_ELAPSED_TIME) / 1000.0f);
	Spline spline;
	spline.setup(controlPoints, cpSize + 1);
	if (cpSize > 2) {
		Parabola parabola = Parabola(controlPoints[0], controlPoints[1], controlPoints[2]);
		glColor3f(1, 1, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		parabola.draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0, 1, 0);
		Vector cross = spline.getCross(parabola);
		parabola.drawTangent(cross, window);
		spline.drawTangent(cross, window);
	}
	glColor3f(1, 1, 1);
	spline.draw();
	for (int i = 0; i < cpSize; i++) {
		Circle circle = Circle(controlPoints[i], 5);
		glColor3f(1, 0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		circle.draw();
		glColor3f(1, 1, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		circle.draw();
	}
	glutSwapBuffers();
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	if (key == ' ') {
		window.setSize(250, 750, 250, 750);
		window.setTime(glutGet(GLUT_ELAPSED_TIME) / 1000.0f);
		window.setMove(true);
		window.setSpeed(2, 3);
	}
	glutPostRedisplay();
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
	glutPostRedisplay();
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		Vector click = window.convertPixToM(Vector(x, y));
		click.t = glutGet(GLUT_ELAPSED_TIME) / 100.0f;
		if (cpSize < 10) {
			controlPoints[cpSize] = click;
			cpSize++;
			controlPoints[cpSize] = controlPoints[0];
			controlPoints[cpSize].t = click.t + (click.t - controlPoints[0].t) / cpSize;
		}
	}
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
	glutInitWindowSize(600, 600);// Alkalmazas ablak kezdeti merete 600x600 pixel 
	glutInitWindowPosition(100, 100);// Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);// 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat");// Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);	// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();				// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}

