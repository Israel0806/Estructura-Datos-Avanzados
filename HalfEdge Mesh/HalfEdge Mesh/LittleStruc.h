#ifndef LITTLESTRUC_H
#define LITTLESTRUC_H

#include <iostream>
#include <vector>

struct Vertex;
struct Edge;
struct Face;
struct Point;

using namespace std;

struct Point{
	double data[3];// x, y, z;
	bool marked;
	
	Point(float* ver);
};

struct Poly{
	vector<Point *> points;
	
	Poly(){};
	
	void addPoint(Point* );
	
	~Poly();
};

struct Vertex {
	int id;
	bool cond;
	Edge *edge;
	double data[3];// x, y, z;
	double pData[3];//px, py, pz;
	double normal[3];
	
	bool operator==(Vertex *v);
	
	double* operator+(Vertex *v);
	
	double* operator-(Vertex*v);
	
	Vertex(float x, float y, float z, int id);
	
	int getLevel();
	
	~Vertex();
};

struct Edge {
	static int edges;
	int id;
	bool cond;
	Edge *twin;
	Edge *next;
	Edge *prev;
	Vertex *head;
	Vertex *tail;
	Face *leftFace;
	
	/// nuevo para fusion
	vector<Face* > iLFace;
	vector<Point* > iLPoint;
	
	Edge();
	
	Edge(Vertex *tail, Vertex *head, Edge *prev, Edge *next, Edge *twin, Face *face);
	
	~Edge();
};

struct Face {
	int id;
	Edge *first;
	double normal[3];
	static int faces;
	
	/// nuevo para fusion
	vector<Edge* > iLEdge;
	vector<Point* > iLPoint;
	
	Face(Edge *first);
	
	~Face();
};

#endif

