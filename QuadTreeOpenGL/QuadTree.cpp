#include "QuadTree.h"

# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>

# define colorRed glColor3f(1,0,0)
# define colorYellow glColor3f(1,0.843,0)
# define colorPurple glColor3f(0.502, 0.000, 0.502)
# define colorGreen glColor3f(0,0.502,0)
# define colorBlue glColor3f(0,0,0.804)
# define colorBlack glColor3f(0,0,0)
# define colorOrange glColor3f(1,0.4,0.1);

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <time.h>

using namespace std;

QuadTree::QuadTree() {
	node = true;
	for (auto &i : Q)
		i = nullptr;
}

QuadTree::~QuadTree(){
	for(int i=0;i<4;++i)
		if(Q[i])
			delete Q[i];
}

bool QuadTree::insert(Point2D p) {
	if (!isInBoundary(p))
		return false;
	
	// limite 6 puntos por nodo
	if(!node){
		Q[getQuadrant(this,p)]->insert(p);
		return true;
	}
	points.push_back(p);
	if (points.size() >= 6) {
		Q[0] = new QuadTree;
		Q[0]->P1.x = P1.x;
		Q[0]->P1.y = P1.y;
		Q[0]->P2.x = (P1.x + P2.x) / 2;
		Q[0]->P2.y = (P1.y + P2.y) / 2;
		
		Q[1] = new QuadTree;
		Q[1]->P1.x = (P1.x + P2.x) / 2;
		Q[1]->P1.y = P1.y;
		Q[1]->P2.x = P2.x;
		Q[1]->P2.y = (P1.y + P2.y) / 2;
		
		Q[2] = new QuadTree;
		Q[2]->P1.x = P1.x;
		Q[2]->P1.y = (P1.y + P2.y) / 2;
		Q[2]->P2.x = (P1.x + P2.x) / 2;
		Q[2]->P2.y = P2.y;
		
		Q[3] = new QuadTree;
		Q[3]->P1.x = (P1.x + P2.x) / 2;
		Q[3]->P1.y = (P1.y + P2.y) / 2;
		Q[3]->P2.x = P2.x;
		Q[3]->P2.y = P2.y;
		
		node = false;
		while (!points.empty()) {
			Q[getQuadrant(this,points.back())]->points.push_back(points.back());
			points.pop_back();
		}		
		for (auto &i:this->Q)
			if (i->points.size() >= 6) 
				Decom(i);
	}
}

bool QuadTree::findBF(QuadTree* QT,float x, float y){
	while(true){
		if(!QT->node){
			Point2D p(x,y);
			QT = QT->Q[getQuadrant(QT,p)];
		}
		else{
			for(int i=0;i<QT->points.size();++i){
				if((QT->points[i].x >= x - 5 and QT->points[i].x <= x + 5) and 
					(QT->points[i].y >= y - 5 and QT->points[i].y <= y + 5)){
					QT->points[i].marked = true;
					return true;
				}
			}
			return false;
		}
	}
}

bool QuadTree::findF(QuadTree* QT, float x, float y){
	while(true){
		if(!QT->node){
			Point2D p(x,y);
			QT = QT->Q[getQuadrant(QT,p)];
		}
		else {
			for(int i=0;i<QT->points.size();++i){
				if((QT->points[i].x >= x - 1  and QT->points[i].x <= x + 1) and 
				   (QT->points[i].y >= y - 1 and QT->points[i].y < y + 1)){
					return QT->points[i].marked = true;
				}
			}
			return false;
		}
	}	
}

void QuadTree::Decom(QuadTree *QT) {
	QT->Q[0] = new QuadTree;
	QT->Q[0]->P1.x = QT->P1.x;
	QT->Q[0]->P1.y = QT->P1.y;
	QT->Q[0]->P2.x = (QT->P1.x + QT->P2.x) / 2;
	QT->Q[0]->P2.y = (QT->P1.y + QT->P2.y) / 2;
	
	QT->Q[1] = new QuadTree;
	QT->Q[1]->P1.x = (QT->P1.x + QT->P2.x) / 2;
	QT->Q[1]->P1.y = QT->P1.y;
	QT->Q[1]->P2.x = QT->P2.x;
	QT->Q[1]->P2.y = (QT->P1.y + QT->P2.y) / 2;
	
	QT->Q[2] = new QuadTree;
	QT->Q[2]->P1.x = QT->P2.x;
	QT->Q[2]->P1.y = (QT->P1.y + QT->P2.y) / 2;
	QT->Q[2]->P2.x = (QT->P1.x + QT->P2.x) / 2;
	QT->Q[2]->P2.y = QT->P1.y;
	
	QT->Q[3] = new QuadTree;
	QT->Q[3]->P1.x = (QT->P1.x + QT->P2.x) / 2;
	QT->Q[3]->P1.y = (QT->P1.y + QT->P2.y) / 2;
	QT->Q[3]->P2.x = QT->P1.x;
	QT->Q[3]->P2.y = QT->P2.y;
	
	QT->node = false;
	while (!QT->points.empty()) {
		QT->Q[getQuadrant(QT,QT->points.back())]->points.push_back(QT->points.back());
		QT->points.pop_back();
	}
	
	for (auto i:(QT->Q)) {
		if (i->points.size() >= 6) {
			i->node = false;
			Decom(i);
		}
	}
}

bool inline QuadTree::isInBoundary(Point2D p) {
	return p.x >= P1.x and p.x <= P2.x and p.y <= P1.y and p.y >= P2.y;
}

int QuadTree::getQuadrant(QuadTree* QT,Point2D p) {
	Point2D mid((QT->P1.x + QT->P2.x) / 2, (QT->P1.y + QT->P2.y) / 2);
	if (p.x > mid.x)
		if (p.y > mid.y)
			return 1;
		else
			return 3;
	else if (p.y > mid.y)
			return 0;
		else
			return 2;
}

vector<float> QuadTree::loadTree(){
	ifstream f;
	string line;
	float x,y;
	f.open("points.txt");
	vector <float> vec(4);
	vec[0] = -1;
	vec[1] = 2e9;
	vec[2] = -1;
	vec[3] = 2e9;
	
	while(getline(f,line)){
		x = stof( line.substr(0,line.find(',')) );
		y = stof( line.substr(line.find(',')+1) );
		if(x>vec[0])
			vec[0] = x;
		else if(x<vec[1])
			vec[1] = x;
		if(y>vec[2])
			vec[2] = y;
		else if(y<vec[3])
			vec[3] = y;
	}
	//no es un cuadrado
/*	x = vec[1]+vec[0];
	y = vec[2]+vec[3];
	cout<<"0: "<<vec[0]<<" 1: "<<vec[1]<<endl;
	cout<<"2: "<<vec[2]<<" 3: "<<vec[3]<<endl;
	cout<<"x: "<<x<<" y: "<<y<<endl;
	if(x != y){
		if(x>y){
			vec[1] = -vec[0];
			vec[3] = vec[1];
		}
		else{
			vec[0] = vec[2];
			vec[1] = vec[3];
		}
	}*/
	P1.x = vec[1];
	P1.y = vec[2];
	P2.x = vec[0];
	P2.y = vec[3];
	f.close();
	f.open("points.txt");
	while(getline(f,line)){
		x = stof( line.substr(0,line.find(',')) );
		y = stof( line.substr(line.find(',')+1) );
		Point2D p(x,y);
		insert(p);
		
	}
	return vec;
}

void QuadTree::drawRec(){
	colorBlue;
	//dibujar recuadro
	glBegin(GL_LINE_LOOP);
	glVertex2f(P1.x,P2.y);
	glVertex2f(P1.x,P1.y);
	glVertex2f(P2.x,P1.y);
	glVertex2f(P2.x,P2.y);
	glEnd();
}

void QuadTree::draw(QuadTree* QT, int rightClick){
	if(!QT->node){
		//dibujar cuadrantes internos
		colorGreen;
		// linea vertical
		glBegin(GL_LINES);
		glVertex2f((QT->P2.x+QT->P1.x)/2,QT->P2.y);
		glVertex2f((QT->P2.x+QT->P1.x)/2,QT->P1.y);
		glEnd();
		// linea horizontal
		glBegin(GL_LINES);
		glVertex2f(QT->P1.x,(QT->P1.y+QT->P2.y)/2);
		glVertex2f(QT->P2.x,(QT->P1.y+QT->P2.y)/2);
		glEnd();
		
		for(auto Q : QT->Q){
			draw(Q,rightClick);
		}
	} else {
		//dibujar puntos del qt
		glBegin(GL_POINTS);
		for(int i=0;i<QT->points.size();++i){
			colorOrange;
			if(QT->points[i].marked){
				colorYellow;
				//if(!rightClick)
				QT->points[i].marked = false;
			}
			glVertex2f(QT->points[i].x,QT->points[i].y);
		}
		glEnd();
	}
}


void QuadTree::generatePoints(int n, float minX,float maxX,float minY,float maxY){
	srand(time(0));	
	ofstream f;
	remove("points.txt");
	f.open("points.txt");
	float x = abs(minX)+abs(maxX),fx,fy;
	float y = abs(minY)+abs(maxY);
	for(int i=0;i<n;++i){
		fx = rand()%int(x) - abs(minX);
		fy = rand()%int(y) - abs(minY);
		f<<fx<<","<<fy<<endl;
	}
	f.close();
}

float QuadTree::getP1x(){
	return P1.x;
}

float QuadTree::getP1y(){
	return P1.y;
}

float QuadTree::getP2x(){
	return P2.x;
}

float QuadTree::getP2y(){
	return P2.y;
}



