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

struct Rec;

class RTree;

struct Point;

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
	bool cond;
	int maxSize;
	Point *P1, *P2;
	vector<Point *> dots;
	
	void draw(int index);
	
	Rec(int size);
	
	void copy(Rec *rec);
	
	void adjustRec(Point *p);
	
	void adjustRec();
	
	void addPoint(Point *p);
	
	~Rec();
	
};

struct Box { /// I
	bool leaf;
	int index;
	int maxSize;
	Point *P1, *P2;
	Rec *recPadre; /// rec de arriba
	vector<Rec *> nodes;
	
	Box(int size);
	
	void reset(Box *box);
	
	void draw(Box *box, int index);
	
	void find(Box *box, Rec *recAux);
	
	bool overlaps(Rec *rec1, Rec *rec2);
	
	bool doOverlap(Point *l1, Point *r1, Point *l2, Point *r2);
	
	~Box();
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
	void find();
	
	void reset();
	
	Box *getRoot();
	
	RTree(int size);
	
	void find(Rec *recAux);
	
	bool search(Rec *fig);
	
	void insert(Rec *fig);
	
	~RTree();
};

#endif
