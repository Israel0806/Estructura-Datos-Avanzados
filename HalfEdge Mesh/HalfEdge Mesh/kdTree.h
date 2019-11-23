#ifndef KDTREE_H
#define KDTREE_H

#include "LittleStruc.h"

struct DVertex {
	static int ver;
	int id;
	float point[3];
	DVertex *left;
	DVertex *right;
	
	DVertex(float x, float y, float z, int id);
	
	void operator=(Vertex *v1);
	
	void set(DVertex *ver);
	
	~DVertex();
};

class KdTree {
	DVertex *root;
	int k;
	
	DVertex *findMin(DVertex *root, int dim, int prof);
	
	DVertex *find(DVertex *root, DVertex *point, int &prof);
	
	bool inline isEqual(DVertex *v1, DVertex *v2);
	
	DVertex *min(DVertex *v1, DVertex *v2, int cd);
	
	void ins(DVertex *root, Vertex *ver);
	
public:
	KdTree(int dim);
	
	DVertex *getRoot() { return root; }
	
	void recIns(Vertex *mv1);
	
	int RecFind(Vertex *v);
	
	bool del(DVertex *point);
	
	void deleteAll(DVertex *node);
	
	~KdTree();
};

#endif
