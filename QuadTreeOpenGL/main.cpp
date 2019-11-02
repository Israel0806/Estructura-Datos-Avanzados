#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#endif

#include "QuadTree.h"
#include <iostream>
#include <math.h>

# define colorRed glColor3f(1,0,0)
# define colorYellow glColor3f(1,0.843,0)
# define colorPurple glColor3f(0.502, 0.000, 0.502)
# define colorGreen glColor3f(0,0.502,0)
# define colorBlue glColor3f(0,0,0.804)
# define colorBlack glColor3f(0,0,0)

# define KEY_ESC 27
# define KEY_PLUS 43
# define KEY_MINUS 45

using namespace std;

QuadTree* QT;
vector<float> vec;

float width = 600;
float height = 600;

bool rightClick = false;
float mx,my=0;
int rangeF = 50;

GLvoid idle(){ // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

GLvoid reshape_cb (int w, int h) {
	if (w==0||h==0) return;
	glViewport(0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	//gluOrtho2D(0,w,0,h);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

GLvoid display_cb() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glOrtho(vec[1]-100,vec[0]+100,vec[3]-100,vec[2]+100,0.0f, 1.0f);
	glColor3f(1,1,0); glLineWidth(1);
	colorBlack;
		
	if(rightClick){
		glLineWidth(2);
		glBegin(GL_LINE_LOOP);
		glVertex2d(mx-rangeF,my-rangeF);
		glVertex2d(mx-rangeF,my+rangeF);
		glVertex2d(mx+rangeF,my+rangeF);
		glVertex2d(mx+rangeF,my-rangeF);
		glEnd();
	}
	glLineWidth(2);
	
	QT->drawRec();
	// eje x
	glBegin(GL_LINES);
		glVertex2f(vec[1]-100,0);
		glVertex2f(vec[2]+100,0);
	glEnd();
	// eje y
	glBegin(GL_LINES);
		glVertex2f(0,vec[3]-100);
		glVertex2f(0,vec[2]+100);
	glEnd();
	
	glLineWidth(2);
	glPointSize(4);
	QT->draw(QT,rightClick);
	
	glutSwapBuffers();
}

GLvoid OnMouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		cout<<vec[1]-100<<endl<<vec[0]+100<<endl;
		//x = round(    -700    *  (x-300)  * (x-600) /   (300*600)   +    700     *x*  (x-300)  /   (600*300));
		x = round((vec[1]-100)*(x-width/2)*(x-width)/(width*width/2)+(vec[0]+100)*x*(x-width/2)/(width*width/2));
		y = round((vec[2]+100)*(y-width/2)*(y-width)/(width*width/2)+(vec[3]-100)*y*(y-width/2)/(width*width/2));
		// y=(vec[0]+100)/100*(300-y)/3;
		Point2D p(x,y);
		cout<<endl<<"Mouse: "<<x<<" "<<y<<endl;
		//insertar un nuevo punto en el quadtree
		//Point2D p(x-abs(int(width/2)),abs(int(height/2))-y);
		QT->insert(p);
	}
	else if(button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN){
		x = round((vec[1]-100)*(x-width/2)*(x-width)/(width*width/2)+(vec[0]+100)*x*(x-width/2)/(width*width/2));
		y = round((vec[2]+100)*(y-width/2)*(y-width)/(width*width/2)+(vec[3]-100)*y*(y-width/2)/(width*width/2));
		cout<<"Range: "<<rangeF<<endl;
		cout<<endl<<"Mouse: "<<x<<" "<<y<<endl;
		rightClick = true;
	}
	else if(button == GLUT_MIDDLE_BUTTON and state == GLUT_DOWN){
		system("cls");
	}
	else if(button == GLUT_RIGHT_BUTTON and state == GLUT_UP){
		rightClick=false;
	}
	glutPostRedisplay();
}

GLvoid OnMouseMotion(int x, int y) {
	if(rightClick){
		mx = round((vec[1]-100)*(x-width/2)*(x-width)/(width*width/2)+(vec[0]+100)*x*(x-width/2)/(width*width/2));
		my = round((vec[2]+100)*(y-width/2)*(y-width)/(width*width/2)+(vec[3]-100)*y*(y-width/2)/(width*width/2));
		for(float i = mx-rangeF;i<mx+rangeF;i++)
			for(float j = my-rangeF;j<my+rangeF;j++)
				QT->findF(QT,i,j);
		
		glutPostRedisplay();
	}
}

GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		exit(0);
		break;
	case KEY_PLUS:
		rangeF+=10;
		break;
	case KEY_MINUS:
		if(rangeF>20)
			rangeF-=10;
		break;
	default:
		break;
	}
}

GLvoid initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (600,600);
	glutInitWindowPosition (400,100);
	glutCreateWindow ("QuadTree");
	glutDisplayFunc (display_cb);
	glutReshapeFunc (reshape_cb);
	glutMotionFunc(&OnMouseMotion);
	glutKeyboardFunc(&window_key);
//	glutIdleFunc(&idle);
	glClearColor(1.f,1.f,1.f,1.f);
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	QT = new QuadTree;
	int s=0;
	QT->generatePoints(s,-700,700,-700,700);
	
	vec = QT->loadTree();
	if(!s){
		QT->P1.x = -700;
		QT->P1.y = 700;
		QT->P2.x = 700;
		QT->P2.y = -700;
		vec[0] = 700;
		vec[1] = -700;
		vec[2] = 700;
		vec[3] = -700;
	}
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	initialize();
	glutMouseFunc(&OnMouseClick);
	glutMainLoop();
	return 0;
}
