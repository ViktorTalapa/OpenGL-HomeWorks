//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fĂĽggvĂ©nyt kivĂ©ve, a lefoglalt adat korrekt felszabadĂ­tĂˇsa nĂ©lkĂĽl
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjeloles kommentjeit kiveve
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
// Nev    : TALAPA VIKTOR GYÖRGY
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

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y, z;
   long t;

   Vector( ) { 
    x = y = z = 0;
    t = 0;
   }
   Vector(float x0, float y0, float z0 = 0, long t0 = 0) {
    x = x0; y = y0; z = z0; t = t0;
   }
   Vector operator=(const Vector& v) {
       x = v.x; y = v.y; z = v.z; t = v.t;
       return v;
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
   float operator*(const Vector& v) {     // dot product
    return (x * v.x + y * v.y + z * v.z);
   }
   Vector operator%(const Vector& v) {     // cross product
    return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   Vector operator/(float a) {
       return Vector(x / a, y / a, z / a);
   }
   float length() { return sqrt(x * x + y * y + z * z); }
};
 
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) { 
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

struct Camera {
	Vector point;
	int index;
	float width, height;
	bool enabled;
};

const int screenWidth = 600;    // alkalmazĂˇs ablak felbontĂˇsa
const int screenHeight = 600;

Camera camera;
Color image[screenWidth * screenHeight];    // egy alkalmazĂˇs ablaknyi kĂ©p
Vector controlPoints[10];
Vector circleCenters[10];
float cRadius;
short cpSize;
long elapsedTime;
bool move;
long moveStartTime;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
    glViewport(0, 0, screenWidth, screenHeight);
    // Peldakent keszitunk egy kepet az operativ memoriaba
    for(int i= 0; i < screenHeight; i++)
        for(int j= 0; j < screenWidth; j++)
            image[i * screenWidth + j] = Color(0.83f, 0.83f, 0.83f);
    cRadius= 5;
    cpSize= 0;
    elapsedTime= 0;
    move= false;
    moveStartTime= 0;
    camera.width= 58;
    camera.height= 68;
    camera.enabled= false;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, camera.width, 0,  camera.height);
}

Vector convertPos(Vector pos) {
    float x= pos.x * camera.width / screenWidth;
    float y= camera.height - pos.y * camera.height / screenHeight;
    return Vector(x, y);
}

void sortArrayByX(Vector* array, int size) {
    for (int i= 0; i < size - 1; i++)
        for (int j= 0; j < size - i - 1; j++)
            if (array[j].x > array[j + 1].x || (array[j].x == array[j + 1].x && array[j].y > array[j + 1].y)) {
                Vector temp= array[j];
                array[j]= array[j + 1];
                array[j + 1]= temp;
            }
}

void drawPoint(Vector& point) {
    glBegin(GL_POINTS);
        glVertex2f(point.x, point.y);
    glEnd();
}

//Forrás: http://blog.johnryding.com/post/3087060819/opengl-circles
void drawCircle(Vector& center) {
    float radius= 2;
    int lines= 180;
    glBegin(GL_POLYGON);
        for (int i= 0; i <= lines; i++) {
            float x= center.x + radius * cos(i * 2 * M_PI / lines);
            float y= center.y + radius * sin(i * 2 * M_PI / lines);
            glVertex2f(x, y);
        }
    glEnd();
}

//Forrás: http://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain
float cross(Vector& o, Vector& a, Vector& b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}
void drawConvexHull(Vector* cPoints) {
    Vector cHull[cpSize];
    int k= 0;
    for (int i= 0; i < cpSize; i++) {
        while (k > 1 && cross(cHull[k - 2], cHull[k - 1], cPoints[i]) <= 0) k--;
        cHull[k]= cPoints[i];
        k++;
    }
    for (int i= cpSize - 2, l= k + 1; i >= 0; i--) {
            while (k >= l && cross(cHull[k - 2], cHull[k - 1], cPoints[i]) <= 0) k--;
            cHull[k] = cPoints[i];
            k++;
        }
    glBegin(GL_POLYGON);
        for (int i= 0; i < k - 1; i++) {
            glVertex2f(cHull[i].x, cHull[i].y);
        }
    glEnd();

}

int fact(int n) {
    int m= 1;
    for (int i= 2; i <= n; i++) m*= i;
    return m;
}

float bin(int n, int k) {
    return fact(n) / (float)(fact(k) * fact(n - k));
}

//Forrás: http://hu.wikipedia.org/wiki/Bézier-görbe
void drawBezierCurve(Vector* cPoints) {
    glBegin(GL_LINE_STRIP);
    for (float t= 0; t <= 1; t+= 0.01f) {
        Vector bezier;
        for (int i= 0; i < cpSize; i++) {
            bezier= bezier + cPoints[i] * bin(cpSize - 1, i) * pow(t, i) * pow(1 - t, cpSize - i - 1);
        }
        glVertex2f(bezier.x, bezier.y);
    }
    glEnd();
}

void drawCatmullRomSpline(Vector* cPoints) {
    Vector v[10];
    float ts[10];
    ts[0]= 0;
    for (int i= 1; i < cpSize - 1; i++) {
        ts[i]= (cPoints[i].t - cPoints[0].t) / (float)(cPoints[cpSize - 1].t - cPoints[0].t);
        Vector a= ((cPoints[i] - cPoints[i - 1]) / (ts[i] - ts[i - 1]));
        Vector b= ((cPoints[i + 1] - cPoints[i]) / (ts[i + 1] - ts[i]));
        v[i]= (a + b) * 0.5;
    }
    ts[cpSize - 1]= 1;
    glBegin(GL_LINE_STRIP);
    int i= 0;
    for (float t= 0; t <= 1; t+= 0.001f) {
        float td= ts[i + 1] - ts[i];
        Vector a_i= (v[i + 1] + v[i]) / pow(td, 2) - (cPoints[i + 1] - cPoints[i]) * 2 / pow(td, 3);
        Vector b_i= (cPoints[i + 1] - cPoints[i]) * 3 / pow(td, 2) - (v[i + 1] + v[i] * 2) / td;
        Vector c_i= v[i];
        Vector d_i= cPoints[i];
        td= t - ts[i];
        Vector f= a_i * pow(td, 3) + b_i * pow(td, 2) + c_i * td + d_i;
        glVertex2f(f.x, f.y);
        if (ts[i + 1] < t) i++;
    }
    glEnd();
}

void drawCatmullClarkCurve(Vector* cPoints, short iteration) {
	Vector* oldPoints= new Vector[cpSize];
	for (int i= 0; i < cpSize; i++) oldPoints[i]= cPoints[i];
	int oldSize= cpSize;
    for (int it= 1; it <= iteration; it++) {
    	int newSize= 2 * oldSize - 1;
    	Vector* newPoints= new Vector[newSize];
    	newPoints[0]= oldPoints[0];
    	newPoints[1]= (oldPoints[0] + oldPoints[1]) * 0.5;
    	for (int i= 1; i < oldSize - 1; i++) {
    		newPoints[2 * i + 1]= (oldPoints[i] + oldPoints[i + 1]) * 0.5;
    		newPoints[2 * i]= (newPoints[2 * i - 1] + newPoints[2 * i + 1]) * 0.5;
    	}
    	newPoints[newSize - 1]= oldPoints[oldSize - 1];
    	delete oldPoints;
    	oldPoints= newPoints;
    	oldSize= newSize;
	}
    glBegin(GL_LINE_STRIP);
    for (int i= 0; i < oldSize; i++)
    	glVertex2f(oldPoints[i].x, oldPoints[i].y);
    glEnd();
    delete oldPoints;
}

void linkCameraToPoint() {
	float left= controlPoints[camera.index].x - camera.point.x;
	float bottom= controlPoints[camera.index].y - camera.point.y;
	float right= left + camera.width;
	float top= bottom + camera.height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, right, bottom, top);
}

void putPointsOnCircle(float radius, int rTime) {
    for (int i= 0; i < cpSize; i++) {
        long startTime= moveStartTime;
        if (moveStartTime < controlPoints[i].t) startTime= controlPoints[i].t;
        float cTime= (elapsedTime - startTime) % rTime;
        float ratio= cTime / rTime;
        if (i % 2 == 0) ratio= 1 - ratio;
        controlPoints[i].x= circleCenters[i].x + radius * sin(2 * M_PI * ratio);
        controlPoints[i].y= circleCenters[i].y + radius * cos(2 * M_PI * ratio);
    }
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
	elapsedTime= glutGet(GLUT_ELAPSED_TIME);
	glClearColor(0, 0, 0, 0);        // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image); // kép másolása rasztertárba

    if (camera.enabled) linkCameraToPoint();
    if (cpSize > 0) {
        if (cpSize > 1) {
            Vector sortedPoints[10]= controlPoints;
            sortArrayByX(sortedPoints, cpSize);
            glColor3f(0.255f, 0.898f, 0.969f);
            drawConvexHull(sortedPoints);
            glColor3f(1, 0, 0);
            drawBezierCurve(controlPoints);
            //drawBezierCurve(sortedPoints);
            glColor3f(0, 1, 0);
            drawCatmullRomSpline(controlPoints);
            glColor3f(0, 0, 1);
            drawCatmullClarkCurve(controlPoints, 4);
        }
        glColor3f(0, 0, 0);
        for (int i= 0; i < cpSize; i++) {
               drawPoint(controlPoints[i]);
               drawCircle(controlPoints[i]);
           }
        if (move) putPointsOnCircle(cRadius, 5000);
    }
    glutSwapBuffers();                     // Buffercsere: rajzolas vege
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	elapsedTime= glutGet(GLUT_ELAPSED_TIME);
	if (key == ' ' && !move) {
        move= true;
        moveStartTime= elapsedTime;
    }
    glutPostRedisplay();
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
	glutPostRedisplay();
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	elapsedTime= glutGet(GLUT_ELAPSED_TIME);
	Vector click= convertPos(Vector(x, y));
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
        if (cpSize < 10) {
            controlPoints[cpSize]= click;
            controlPoints[cpSize].t= elapsedTime;
            circleCenters[cpSize].x= click.x;
            circleCenters[cpSize].y= click.y - cRadius;
            cpSize++;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        for (int i= 0; i < cpSize; i++)
            if (controlPoints[i].x == click.x && controlPoints[i].y == click.y) {
            	camera.point= controlPoints[i];
            	camera.index= i;
                camera.enabled= true;
                break;
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
    glutInit(&argc, argv);                 // GLUT inicializalasa
    glutInitWindowSize(600, 600);            // Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);            // Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");        // Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);                // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);            // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();                    // Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);                // Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();                    // Esemenykezelo hurok
    
    return 0;
}

