#ifndef RTREE_H
#define RTREE_H

#include <iostream>
#include <vector>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

# define colorRed glColor3f(1,0,0)
# define colorYellow glColor3f(1,0.843,0)
# define colorPurple glColor3f(0.502, 0.000, 0.502)
# define colorGreen glColor3f(0,0.502,0)
# define colorBlue glColor3f(0,0,0.804)
# define colorBlack glColor3f(0,0,0)

using namespace std;

class Box;

class RTree;

struct Point;
struct Rec;

struct Point {
	int size;
	vector<int> data;
	
	Point();
	
	void draw();
	
	void print() {
		cout << data[0] << " " << data[1];
	}
	
	Point(int size);
	
	void copy(Point *p);
};

struct Rec {
	Box *boxHijo; /// box de abajo
	Box *boxPadre; /// box al que pertenece
	bool leaf;
	int maxSize;
	Point *P1, *P2;
	vector<Point *> dots;
	
	void draw(int index);
	
	Rec(int size);
	
	void copy(Rec *rec);
	
	void adjustRec(Point *p);
	
	void adjustRec();
	
	void addPoint(Point *p);
};

struct Box { /// I
	bool leaf;
	int index;
	int maxSize;
	Point *P1, *P2;
	Rec *recPadre; /// rec de arriba
	vector<Rec *> nodes;
	
	Box(int size);
	
	void draw(Box *box, int index);
};

//RTree
class RTree {
	Box *root;
	
	void adjustTree(Box *box);
	
	Box *chooseLeaf(Rec *fig);
	
	double computeArea(Rec *rec);
	
	double computeArea(Rec *rec, Rec *fig);
	
	Rec *chooseNext(vector<Rec *> &looseRec, Rec *R1, Rec *R2);
	
public:
	Box *getRoot();
	
	RTree(int size);
	
	bool search(Rec *fig);
	
	void insert(Rec *fig);
	
	void find();
	
};

#endif
