#ifndef LITTLESTRUC_H
#define LITTLESTRUC_H

#include <iostream>
#include <vector>

struct Vertex;
struct Edge;
struct Face;

using namespace std;

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
	
	float *setCoef(int n);
	
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
	
	Edge();
	
	Edge(Vertex *tail, Vertex *head, Edge *prev, Edge *next, Edge *twin, Face *face);
	
	~Edge();
};

struct Face {
	static int faces;
	int id;
	Edge *first;
	double normal[3];
	
	Face(Edge *first);
	
	~Face();
};

#endif
