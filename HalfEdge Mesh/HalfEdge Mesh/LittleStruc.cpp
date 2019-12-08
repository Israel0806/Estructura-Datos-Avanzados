#include "LittleStruc.h"

Point::Point(double* ver) {
	for(int i = 0; i < 3; ++i)
		data[i] = ver[i];
	marked = false;
}

Point::Point() {
	for(int i = 0; i < 3; ++i)
		data[i] = 0;
	marked = false;
}

Vertex::Vertex(float x, float y, float z, int id) {
	this->id = id;
	cond = false;
	data[0] = x;
	data[1] = y;
	data[2] = z;
	this->edge = NULL;
}

void Poly::addPoint(Point * point) {
	points.push_back(point);
}

void Poly::addPoint(Point point) {
	Point* P = new Point(point.data);
	points.push_back(P);
}

Poly::~Poly() {
	while (!points.empty() ) {
		delete points.back();
		points.pop_back();
	}
}

Vertex::~Vertex() {
	//--vertexs;
}

int Vertex::getLevel() {
	int n = 0;
	Edge *e = edge;
	do {
		e = e->twin->next;
		++n;
	} while (e and e != edge);
	return n;
}

bool Vertex::operator==(Vertex *v) {
	return data[0] == v->data[0] and data[1] == v->data[1] and data[2] == v->data[2];
}

double* Vertex::operator+(Vertex *v) {
	double *point = new double[3];
	for(int i = 0; i < 3; ++i)
		point[i] = data[i] + v->data[i];
	return point;
}

double* Vertex::operator-(Vertex *v) {
	double *point = new double[3];
	for(int i = 0; i < 3; ++i)
		point[i] = data[i] - v->data[i];
	return point;
}


Edge::Edge() {
	id = edges++;
	cond = false;
};

Edge::Edge(Vertex *tail, Vertex *head, Edge *prev, Edge *next, Edge *twin, Face *face) {
	id = edges++;
	this->tail = tail;
	this->head = head;
	this->prev = prev;
	this->next = next;
	this->twin = twin;
	this->leftFace = face;
	cond = false;
}

double* Edge::find(Face* face) {
	for (int i = 0; i < iLFace.size(); ++i)
		if(iLFace[i] == face)
			return iLPoint[i]->data;
}

Edge* Edge::nextJump(Edge* edge, Face* face){
	Edge* edgeCopy = edge;
	edge = edge->next;
	do {
		for (int i = 0; i < edge->iLFace.size(); ++i)
			if(edge->iLFace[i] == face)
				return edge;
		
		edge = edge->next;
	} while(edge != edgeCopy);
	return edge;
}

Edge::~Edge() {
	--edges;
	while (!iLPoint.empty() ) {
		delete iLPoint.back();
		iLPoint.pop_back();
	}
}

Face::Face(Edge *edge) {
	id = faces++;
	this->edge = edge;
}

void Point::set(double *ver) {
	for (int i = 0; i < 3; ++i) 
		data[i] = ver[i];
}

bool areEqual(Point* P1, Point P2) {
	for (int i = 0; i < 3; ++i) 
		if(P1->data[i] == P2.data[i]) 
			return true;
	return false;	
}

/// halla el siguiente punto que debo conectar el poligono
double* Face::findClosest(Point* P, Point P1) {
	int i, minIdx = 0;
	Point *minPoint = new Point();
	
	/// guardo el indice con la menor distancia
	for (i = 1; i < iLEdge.size(); ++i)
		if ( !areEqual(iLPoint[i], P1) ) /// obvio, el P1
			if (distance(P, iLPoint[i]) < distance(P, iLPoint[minIdx]) )
				minIdx = i;
	
	return iLPoint[minIdx]->data;
}

Face::~Face() {
	--faces;
}
