#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "HashTable.h"
#include "LittleStruc.h"

# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#include <iostream>
#include <vector>

# define colorRed glColor3f(1,0,0)
# define colorYellow glColor3f(1,0.843,0)
# define colorPurple glColor3f(0.502, 0.000, 0.502)
# define colorGreen glColor3f(0,0.502,0)
# define colorBlue glColor3f(0,0,0.804)
# define colorBlack glColor3f(0,0,0)
# define colorWhite glColor3f(1,1,1)

using namespace std;

class HalfEdge {
	bool worked;
	Hash notMatched;
	vector<float> beta;
	vector<Face *> LFace;
	vector<Edge *> LEdge;
	vector<Vertex *> LVertex;
	
	Vertex *findVertex(int id);
	
	float *sep(string line, string s);
	
	vector<Vertex *> sep2(string line, string del);
	
public:
	HalfEdge();
	
	vector<float> massCenter;
	void savePos();
	
	bool getWorked();
	
	void justDivide();
	
	void reSet();
	
	void divideLoopBF();
	
	void deleteOldMesh();
	
	void divideLoopHash();
	
	bool calculateNormal();
	
	void insertFace(int e);
	
	void normalFace(Face *f);
	
	void scale(float factor);
	
	vector<Face* > getLFace();
	
	void readOBJ(string figure);
	
	void insertEdge(int t, int h);
	
	vector<Vertex* > getLVertex();
	
	void computeMassCenter(float *newPos);
	
	void modifyPos(int index, float factor);
	
	double dotProduct(double* p1, double* p2);
	
	void insertVertex(float x, float y, float z, int id);
	
	HalfEdge* triangulatePoly(vector<Poly* > poligonos);
	
	HalfEdge* fusionDBZ(HalfEdge* mesh1, HalfEdge* mesh2, float movePos[2][3]);
	
	vector<Vertex *> find3Vertex(int id1, int id2, int id3);
	
	void crossProduct(double *normal, double *AB, double *AC);
	
	void draw(bool drawT, bool drawL, bool drawP, float *movePos);
	
	void colorVertex(HalfEdge* Mesh, float movePos[2][3], int index);
	
	void colorLine(HalfEdge* Mesh, float movePos[2][3], int index);
	
	vector<int> findVertexF(int start, Vertex *ver1, Vertex *ver2, Vertex *ver3);
	
	float* rayTriangleIntersect(double orig[3], double dir[3],double vertices[3][3], double snormal[3]);
	
	~HalfEdge();
};

#endif
