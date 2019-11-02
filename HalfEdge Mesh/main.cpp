#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else

# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>

#endif

# include "HalfEdge.h"

# include <vector>
# include <math.h>
# include <string>
# include <fstream>
# include <chrono>

# define colorRed glColor3f(1,0,0)
# define colorYellow glColor3f(1,0.843,0)
# define colorPurple glColor3f(0.502, 0.000, 0.502)
# define colorGreen glColor3f(0,0.502,0)
# define colorBlue glColor3f(0,0,0.804)
# define colorBlack glColor3f(0,0,0)

# define KEY_ESC 27
# define KEY_PLUS 43
# define KEY_MINUS 45

# define KEY_A 97
# define KEY_B 98
# define KEY_C 99
# define KEY_D 100
# define KEY_E 101
# define KEY_L 108
# define KEY_P 112
# define KEY_R 114
# define KEY_S 115
# define KEY_T 116
# define KEY_V 118
# define KEY_W 119
# define KEY_X 120
# define KEY_Z 122
# define KEY_DOWN 103
# define KEY_LEFT 100
# define KEY_RIGHT 102
# define KEY_UP 101
# define KEY_ENTER 13

# define KEY_BACKSPACE 8

GLfloat width = 600, height = 600, angle = 0,
	movex = 0, movey = 1, movez = 0,
	rangez = -10.0f, rangex = 0.0f, rangey = 1.0f;
int index = 0;
string figure[2];
float movePos[2][3];
int scalePos[2][2];

bool drawT = false, drawL = true, drawP = true, drawR = true;

float mx, my, ax, ay;

vector<HalfEdge *> Malla;
vector<float> range;

GLvoid idle() {
	glutPostRedisplay();
}

GLvoid OnMouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mx = x;
		my = y;
	} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		angle = 0;
		movex = 0;
		movey = 1;
		movez = 0;
	} else if (button == 3) { // forward
		++rangez;
	} else if (button == 4) { // backward
		--rangez;
	}
}

GLvoid OnMouseMotion(int x, int y) {
	float dx = x - mx;
	float dy = y - my;
	mx = x;
	my = y;
	ax += dx;
	ay += dy;
}

GLvoid numberKeys(char key) {
	if (key == '1' or key == '2') {
		index = key - '0' - 1;
		cout << "Figura " << index + 1 << " seleccionada: " << figure[index] << endl << endl;
	}
}

/// duhhh
void lower(string &name) {
	for (auto &c : name)
		c = tolower(c);
}

/// verifico si el nombre ingresado existe
bool findName(string name) {
	ifstream file;
	string line;
	bool found = false;
	file.open("Files.txt");
	while (getline(file, line)) {
		lower(line);
		if (strcmp(line.c_str(), name.c_str()))
			return true;
	}
	return false;
}

/// Cambio una de las mallas cargadas

void showFigureNames() {
	ifstream file;
	string line;
	cout << endl << "FILE NAMES" << endl;
	file.open("Files.txt");
	while (getline(file, line)) {
		cout << line << endl;
	}
	cout << endl;
}

void changeFigure() {
	string newName;
	cout << "La figura que cambiara es " << figure[index] << endl;
	try {
		while (true) {
			cout << "Ingrese el nombre del archivo OBJ: ";
			cin >> newName;
			lower(newName);
			if (newName == "help") {
				showFigureNames();
			} else if (newName == "exit")
				return;
			else if (!findName(newName))
				cout << "El nombre ingresado no existe" << endl;
			else
				break;
		}
		figure[index] = newName;
		
		delete Malla[index];
		Malla[index] = new HalfEdge();
		Malla[index]->readOBJ(figure[index]);
		Malla[index]->savePos();
	} catch (exception &e) {
		cout << "Error type: " << e.what() << endl;
	}
}

GLvoid window_key(unsigned char key, int x, int y) {
	numberKeys(key);
	double cociente;
	chrono::system_clock::time_point t;
	chrono::duration<float, milli> r;
	switch (key) {
	case KEY_ENTER:
		changeFigure();
		break;
	case KEY_ESC:
		for (auto &mesh : Malla)
			delete mesh;
		exit(0);
		break;
	case KEY_B:
		if (!(Malla[0]->getWorked()) or !(Malla[1]->getWorked())) {
			cout << "One of the meshes is done incorrectly" << endl;
			break;
		}
		Malla[0]->deColor();
		Malla[1]->deColor();
		t = chrono::system_clock::now();
		Malla[0]->colorVertex(Malla[1], movePos, 0);
		Malla[1]->colorVertex(Malla[0], movePos, 1);
		r = chrono::system_clock::now() - t;
		cout << "Find inside vertex time: " << r.count() / 1000 << " seconds" << endl;
		
		t = chrono::system_clock::now();
		Malla[0]->colorLine(Malla[1], movePos, 0);
//		Malla[1]->colorLine(Malla[0], movePos, 1);
		r = chrono::system_clock::now() - t;
		cout << "Find line intersections time: " << r.count() / 1000 << " seconds" << endl;
		break;
	case KEY_C:
		Malla[index]->savePos();
		if (Malla[index]->getWorked()) {
			Malla[index]->divideLoopHash();
			Malla[index]->computeMassCenter(movePos[index]);
		} else
			cout << "Can't do that mate" << endl;
		break;
	case KEY_R:
		drawR = !drawR;
		break;
	case KEY_P:
		drawP = !drawP;
		break;
	case KEY_T:
		drawT = !drawT;
		break;
	case KEY_L:
		drawL = !drawL;
		break;
	case KEY_W:
		movePos[index][1] += 0.5;
		//Malla[index]->modifyPos(1, 0.5f);
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_S:
		movePos[index][1] -= 0.5;
		//Malla[index]->modifyPos(1, -0.5f);
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_A:
		movePos[index][0] -= 0.5;
		//		Malla[index]->modifyPos(0, -0.5f);
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_X:
		movePos[index][2] -= 0.5;
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_Z:
		movePos[index][2] += 0.5;
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_D:
		movePos[index][0] += 0.5;
		//		Malla[index]->modifyPos(0, 0.5f);
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_PLUS:
		++scalePos[index][1];
		Malla[index]->scale(1.1);
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_MINUS:
		++scalePos[index][0];
		Malla[index]->scale(0.9);
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	case KEY_BACKSPACE:
		cout << "Reset" << endl;
		/// Para volver a la pos original debo multiplicar 1.1 x numero de veces que agrande la figura
		/// y debo multiplicar 0.9 x numero de veces que reduci la figura
		cociente = pow(1.1, scalePos[index][1]) * pow(0.9, scalePos[index][0]);
		
		for (auto &vec : Malla[index]->getLVertex())
			for (int i = 0; i < 3; ++i)
				vec->data[i] /= cociente;
		
		for (int i = 0; i < 3; ++i) {
			scalePos[index][i] = 0;
			movePos[index][i] = 0;
		}
		Malla[index]->computeMassCenter(movePos[index]);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

GLvoid specialKey(int key, int x, int y) {
	switch (key) {
	case KEY_UP:
		--rangey;
		break;
	case KEY_DOWN:
		++rangey;
		break;
	case KEY_LEFT:
		++rangex;
		break;
	case KEY_RIGHT:
		--rangex;
		break;
	}
	glutPostRedisplay();
}

GLvoid reshape_cb(int w, int h) {
	if (w == 0 || h == 0) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GLvoid display_cb() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	
	gluPerspective(45.0f, 1, 1, 1000.0f);
	glTranslatef(rangex, rangey, rangez);
	glRotatef(ax, 0.0f, 1.0f, 0.0f);  // Rotate about the (1,1,0)-axis [NEW]
	//glRotatef(ay, 1.0f, 0.0f, 0.0f);  // Rotate about the (1,1,0)-axis [NEW]
	
	colorPurple;
	glLineWidth(3);
	
	//	glutSolidTeapot(2);
	/// 3D axis
	glBegin(GL_LINES);
	/// axis x
	colorRed;
	glVertex3f(-500, 0, 0);
	glVertex3f(500, 0, 0);
	
	/// axis y
	colorGreen;
	glVertex3f(0, -500, 0);
	glVertex3f(0, 500, 0);
	
	/// axis z
	colorBlue;
	glVertex3f(0, 0, -500);
	glVertex3f(0, 0, 500);
	glEnd();
	
	/// draw Rays
	if (drawR) {
		for (auto &ver : Malla[0]->getLVertex()) {
			glBegin(GL_LINES);
			glVertex3f(ver->data[0] + movePos[0][0],
				ver->data[1] + movePos[0][1],
					ver->data[2] + movePos[0][2]);
			glVertex3f(Malla[1]->massCenter[0],
					   Malla[1]->massCenter[1],
					   Malla[1]->massCenter[2]);
			
			glEnd();
		}
	}
	
	glLineWidth(1);
	Malla[0]->draw(drawT, drawL, drawP, movePos[0]);
	Malla[1]->draw(drawT, drawL, drawP, movePos[1]);
	
	glPopMatrix();
	glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}

GLvoid initialize() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("hay que hacerlo simple y evidente");
	
	glutDisplayFunc(display_cb);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutReshapeFunc(&reshape_cb);
	glutKeyboardFunc(&window_key);
	glutSpecialFunc(&specialKey);
	glutIdleFunc(&idle);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	GLfloat qLight[] = {5, 5, -.5f, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, qLight);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	initialize();
	memset(movePos, 0, 6 * sizeof(float));
	memset(scalePos, 0, 4 * sizeof(int));
	Malla.resize(2);
	Malla[0] = new HalfEdge();
	Malla[1] = new HalfEdge();
	figure[0] = "cube";
	figure[1] = "ico";
	Malla[0]->readOBJ(figure[0]);
	Malla[1]->readOBJ(figure[1]);
	Malla[0]->savePos();
	Malla[1]->savePos();
	glutMainLoop();
	return 0;
}
