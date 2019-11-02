#include "LittleStruc.h"

Vertex::Vertex(float x, float y, float z, int id) {
	this->id = id;
	cond = false;
	data[0] = x;
	data[1] = y;
	data[2] = z;
	this->edge = NULL;
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

Edge::~Edge() {
	--edges;
}

Face::Face(Edge *first) {
	id = faces++;
	this->first = first;
}

Face::~Face() {
	--faces;
}
