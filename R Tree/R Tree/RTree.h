#ifndef RTREE_H
#define RTREE_H

#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

using namespace std;

class Tupla;
class RTree;
class Node;
struct Point;
struct Figure;

struct Point{
	int size;
	vector<int> data;
	
	Point();
	Point(int size);
};

struct Figure{
	vector<Point*> dots;
	
	void addPoint(Point* p);
	void draw();
};

class Tupla{
	int maxSize;
	int index;
	vector<Node*> tupla;
public:
	Tupla(int size);
};

class Node{
	vector<Figure*> figuras;
	Point *P1,*P2;
	Tupla* padre;
	Tupla* hijo;
	int maxSize;
	double area;
	bool leaf;
public:
	Node(int size);
};

//RTree
class RTree{
	Tupla *root;

	Tupla* chooseLeaf();
public:
	RTree(int size);
	Figure* search();
	void insert(Figure* fig);
	void del();
	void find();
	
};

#endif
