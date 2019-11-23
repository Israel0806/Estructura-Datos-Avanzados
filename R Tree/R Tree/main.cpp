#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else

# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>

#endif


#include "Rtree.h"

#define KEY_BACKSPACE 8
#define KEY_ESC 27

using namespace std;

int numClicks = 0;
int width = 600;
int height = 600;

bool rightClick = false;
float mx, my;
int rangeF = 50;

RTree arbolCochino(3);
Rec *rec;
Point *P1;
Point *P2;

GLvoid reshape_cb(int w, int h) {
	if (w == 0 || h == 0) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GLvoid OnMouseClick(int button, int state, int x, int y) {
	x = round(-width * (x - width / 2) * (x - width) / (width * width / 2) +
			  width * x * (x - width / 2) / (width * width / 2));
	y = round(width * (y - width / 2) * (y - width) / (width * width / 2) -
			  width * y * (y - width / 2) / (width * width / 2));
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		cout << "Mouse: " << x << " " << y << endl;
		if (numClicks == 0) {
			P1 = new Point(3);
			P2 = new Point(3);
			P1->data[0] = x;
			P1->data[1] = y;
		} else {
			P2->data[0] = x;
			P2->data[1] = y;
		}
		++numClicks;
	} else if (button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN) {
		cout << "Range: " << rangeF << endl;
		cout << endl << "Mouse: " << x << " " << y << endl;
		rightClick = true;
	} else if (button == GLUT_RIGHT_BUTTON and state == GLUT_UP) {
		rightClick = false;
	}
	glutPostRedisplay();
}

GLvoid OnMouseMotion(int x, int y) {
	if (rightClick) {
		mx = round(-width * (x - width / 2) * (x - width) / (width * width / 2) +
				   width * x * (x - width / 2) / (width * width / 2));
		my = round(width * (y - width / 2) * (y - width) / (width * width / 2) -
				   width * y * (y - width / 2) / (width * width / 2));
		glutPostRedisplay();
	}
}

GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_BACKSPACE:
		numClicks = 0;
		break;
	case KEY_ESC:
		delete P1, P2, rec;
		exit(0);
	}
}

GLvoid idle() { // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

void display_cb() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 0);
	glLineWidth(3);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-600, 600, -600, 600, -1, 1);
	if (rightClick) {
		glLineWidth(2);
		glBegin(GL_LINE_LOOP);
			glVertex2d(mx - rangeF, my + rangeF);
			glVertex2d(mx + rangeF, my + rangeF);
			glVertex2d(mx + rangeF, my - rangeF);
			glVertex2d(mx - rangeF, my - rangeF);
		glEnd();
		Rec* recAux = new Rec(1);
		
		/// creo 4 puntos 
		Point* p1 = new Point(2);
		p1->data[0] = mx - rangeF; p1->data[1] = my + rangeF;
		recAux->dots.push_back(p1);
		
		Point* p2 = new Point(2);
		p2->data[0] = mx + rangeF; p2->data[1] = my + rangeF;
		recAux->dots.push_back(p2);
		
		Point* p3 = new Point(2);
		p3->data[0] = mx + rangeF; p3->data[1] = my - rangeF;
		recAux->dots.push_back(p3);
		
		Point* p4 = new Point(2);
		p4->data[0] = mx - rangeF; p4->data[1] = my - rangeF;
		recAux->dots.push_back(p4);
		recAux->P1->copy(p1);
		recAux->P2->copy(p3);
		
		arbolCochino.find(recAux);
		delete recAux, p1, p2, p3, p4;
	}
	
	glPointSize(5);
	if (numClicks == 1)
		P1->draw();
	else if (numClicks == 2) {
		cout << "Reset" << endl << endl;
		numClicks = 0;
		rec = new Rec(1);
		rec->addPoint(P1);
		Point *p = new Point(3);
		p->data[0] = P2->data[0];
		p->data[1] = P1->data[1];
		rec->addPoint(p);
		rec->addPoint(P2);
		p = new Point(3);
		p->data[0] = P1->data[0];
		p->data[1] = P2->data[1];
		rec->addPoint(p);
		arbolCochino.insert(rec);
	}
	arbolCochino.getRoot()->draw(arbolCochino.getRoot(), 0);
	
	glutSwapBuffers();
}

GLvoid initialize() {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("R Tree");
	glutDisplayFunc(display_cb);
	glutReshapeFunc(reshape_cb);
	glutMotionFunc(&OnMouseMotion);
	glutMouseFunc(&OnMouseClick);
	glutKeyboardFunc(&window_key);
	glutIdleFunc(&idle);
	glClearColor(1.f, 1.f, 1.f, 1.f);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	initialize();
	glutMainLoop();
	delete P1, P2, rec;
	return 0;
}
