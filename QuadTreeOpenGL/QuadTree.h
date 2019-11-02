#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>

using namespace std;

struct Point2D {
	float x, y;
	bool marked = false;
	Point2D() {};
	
	Point2D(float x, float y) : x(x), y(y) {};
	
	void set(float x, float y) {
		this->x = x;
		this->y = y;
	}
	
};

class QuadTree {
	bool node;
	
	QuadTree *Q[4]{};
	vector<Point2D> points;
	void Decom(QuadTree *QT);
	
public:
	Point2D P1, P2;
	QuadTree();
	~QuadTree();
	int getQuadrant(QuadTree* QT,Point2D p);
	bool inline isInBoundary(Point2D p);
	bool insert(Point2D p);
	vector<float> loadTree();
	bool findBF(QuadTree* QT, float x, float y);
	bool findF(QuadTree* QT, float x, float y);
	void draw(QuadTree* QT, int rightClick);
	void drawRec();
	void generatePoints(int n, float minX,float maxX,float minY,float maxY);
	float getP1x();
	float getP1y();
	float getP2x();
	float getP2y();
};

#endif
