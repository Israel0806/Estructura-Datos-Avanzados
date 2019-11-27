#include "Rtree.h"

/// size means the dimension of the point
Point::Point(int size) {
	this->size = size;
	data.resize(size);
}

/// size means the number of rectangles accepted in a Box
Box::Box(int size) {
	index = 0;
	leaf = true;
	recPadre = NULL;
	maxSize = size;
	P1 = new Point(2);
	P2 = new Point(2);
	nodes.resize(size + 1);
	for (int i = 0; i < size + 1; ++i)
		nodes[i] = NULL; //new Node(size);
}

/// size means the number of figures accepted in a rectangle
Rec::Rec(int size) {
	leaf = true;
	cond = false;
	maxSize = size;
	boxHijo = NULL;
	boxPadre = NULL;
	P1 = new Point(2);
	P1->data[0] = 2e9;
	P1->data[1] = -2e9;
	
	P2 = new Point(2);
	P2->data[0] = -2e9;
	P2->data[1] = 2e9;
}

/// size means the number of rectangles accepted in a Box
RTree::RTree(int size) {
	root = new Box(size);
}

void Point::copy(Point *p) {
	for (int i = 0; i < size; ++i)
		data[i] = p->data[i];
}

void Rec::addPoint(Point *p) {
	dots.push_back(p);
	adjustRec(p);
}

void Rec::adjustRec(Point *p) {
	if (dots.size() == 1) {
		P1->data[0] = p->data[0];
		P1->data[1] = p->data[1];
		P2->data[0] = p->data[0];
		P2->data[1] = p->data[1];
	} else {
		/// para P1 x
		if (p->data[0] < P1->data[0])
			P1->data[0] = p->data[0];
		/// para P2 x
		if (p->data[0] > P2->data[0])
			P2->data[0] = p->data[0];
		/// para P1 y
		if (p->data[1] > P1->data[1])
			P1->data[1] = p->data[1];
		/// para P2 y
		if (p->data[1] < P2->data[1])
			P2->data[1] = p->data[1];
	}
}

void chooseColor(int index) {
	switch (index) {
	case 0:
		colorRed;
		break;
	case 1:
		colorYellow;
		break;
	case 2:
		colorPurple;
		break;
	case 3:
		colorBlue;
		break;
	case 4:
		colorBlack;
		break;
	}
}

void Point::draw() {
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	glVertex2i(data[0], data[1]);
	glEnd();
}

Box *RTree::getRoot() {
	return root;
}

void Rec::draw(int index) {
	glLineWidth(2);
	//	colorGreen;
	
	if(cond){
		colorGreen;
		cond = false;
	}
	else {
		chooseColor(index);
	}

	glBegin(GL_QUADS);
	for (int i = 0; i < dots.size(); ++i)
		glVertex2f(dots[i]->data[0], dots[i]->data[1]);
	glEnd();
}

void Box::draw(Box *box, int index) {
	if (!box) return;
	/// if its leaf print figures in its rectangles
	if (box->leaf) {
		for (auto &rec : box->nodes)
			if (rec)
				rec->draw(index);
	}
	/// else call draw from the Rec of the nodes of Box 
	/// and go to their boxes
			else {
		glLineWidth(2);
		for (auto &rec : box->nodes) {
			if (rec and rec->boxHijo->index) {
				/// print its frame
				//				index = 1;
				index = (index + 1) % 5;
				chooseColor(index);
				glBegin(GL_LINE_LOOP);
					glVertex2f(rec->P1->data[0], rec->P1->data[1]);
					glVertex2f(rec->P2->data[0], rec->P1->data[1]);
					glVertex2f(rec->P2->data[0], rec->P2->data[1]);
					glVertex2f(rec->P1->data[0], rec->P2->data[1]);
				glEnd();
				rec->boxHijo->draw(rec->boxHijo, index);
			}
		}
	}
}

double RTree::computeArea(Rec *rec) {
	return (rec->P1->data[1] - rec->P2->data[1]) *
		(rec->P1->data[0] - rec->P1->data[0]);
}

double RTree::computeArea(Rec *rec, Rec *fig) {
	Point *p1 = new Point(3);
	p1->copy(rec->P1);
	Point *p2 = new Point(3);
	p2->copy(rec->P2);
	for (auto &dot : fig->dots) {
		/// para p1 x
		if (dot->data[0] < p1->data[0])
			p1->data[0] = dot->data[0];
		/// para p1 y
		if (dot->data[1] > p1->data[1])
			p1->data[1] = dot->data[1];
		
		/// para p2 x
		if (dot->data[0] > p2->data[0])
			p2->data[0] = dot->data[0];
		/// para p1 y
		if (dot->data[1] < p2->data[1])
			p2->data[1] = dot->data[1];
	}
	double resul = (p1->data[1] - p2->data[1]) * (p2->data[0] - p1->data[0]);
	delete p1, p2;
	return resul;
}

Box *RTree::chooseLeaf(Rec *fig) {
	Box *box = root;
	double maxArea, newArea;
	int index;
	while (true) {
		//verifico si la Box es hoja
		if (box->leaf)
			return box;
		
		/// por cada node debo ver quien tiene el menor cambio
		maxArea = 1e9;
		/// necesito hallar las areas cambiadas y devolver el minimo
		/// solo asi veo al cual ir
		for (int i = 0; i < box->index; ++i) {
			newArea = computeArea(box->nodes[i], fig);
			if (newArea < maxArea) {
				index = i;
				maxArea = newArea;
			}
		}
		box = box->nodes[index]->boxHijo;
	}
}

/// resize all boxes because of the new child
void RTree::adjustTree(Box *box) {
	if (box->leaf) {
		/// si el box esta vacio entonces debemos hacer un resize de la caja a 0
		if (box->index == 0)
			cout << "Empty" << endl;
		else if (box->index == 1) {
			box->P1->copy(box->nodes[0]->P1);
			box->P2->copy(box->nodes[0]->P2);
		} else {
			for (auto &rec : box->nodes) {
				if (!rec) break;
				/// para P1 x
				if (box->P1->data[0] > rec->P1->data[0])
					box->P1->data[0] = rec->P1->data[0];
				/// para P2 x
				if (box->P2->data[0] < rec->P2->data[0])
					box->P2->data[0] = rec->P2->data[0];
				/// para P1 y
				if (box->P1->data[1] < rec->P1->data[1])
					box->P1->data[1] = rec->P1->data[1];
				/// para P2 y
				if (box->P2->data[1] > rec->P2->data[1])
					box->P2->data[1] = rec->P2->data[1];
			}
		}
	} else {
		/// nodes son Rec
		for (auto &rec : box->nodes) {
			if (!rec) break;
			/// recHijos son los nodes(Rec) del boxHijo
			/// le hago el adjust al rec segun los hijos de boxHijo
			rec->adjustRec();
			
			/// le hago el adjust al Box
			if (box->P1->data[0] > rec->P1->data[0])
				box->P1->data[0] = rec->P1->data[0];
			/// para P1 y
			if (box->P1->data[1] < rec->P1->data[1])
				box->P1->data[1] = rec->P1->data[1];
			
			/// para P2 x
			if (box->P2->data[0] < rec->P2->data[0])
				box->P2->data[0] = rec->P2->data[0];
			/// para P2 y
			if (box->P2->data[1] > rec->P2->data[1])
				box->P2->data[1] = rec->P2->data[1];
		}
	}
	if (box == root) return;
	adjustTree(box->recPadre->boxPadre);
}

/// copy all values
void Rec::copy(Rec *rec) {
	boxPadre = rec->boxHijo;
	boxHijo = NULL;
	leaf = true;
	maxSize = rec->maxSize;
	P1->copy(rec->P1);
	P2->copy(rec->P2);
	dots = rec->dots;
}

/// check which Rectangle has 	
Rec *RTree::chooseNext(vector<Rec *> &looseRec, Rec *R1, Rec *R2) {
	int index;
	double d1, d2;
	double newArea, maxArea = -1e9;
	/// get biggest |d1 - d2|
	for (int i = 0; i < looseRec.size(); ++i) {
		d1 = computeArea(looseRec[i], R1);
		d2 = computeArea(looseRec[i], R2);
		newArea = abs(d1 - d2);
		if (maxArea < newArea) {
			maxArea = newArea;
			index = i;
		}
	}
	
	/// erase the selected from the vector and return it
	Rec *next = looseRec[index];
	looseRec.erase(looseRec.begin() + index);
	return next;
}

/// ajusto el Rec segun los Rec de su boxHijo
void Rec::adjustRec() {
	if (boxHijo->index == 1) {
		P1->copy(boxHijo->P1);
		P2->copy(boxHijo->P2);
		return;
	}
	//	cout<<"Yo que se"<<endl;
	for (auto &rec : boxHijo->nodes) {
		if (!rec) return;
		/// para P1 x
		if (P1->data[0] > rec->P1->data[0])
			P1->data[0] = rec->P1->data[0];
		/// para P2 x
		if (P2->data[0] < rec->P2->data[0])
			P2->data[0] = rec->P2->data[0];
		/// para P1 y
		if (P1->data[1] < rec->P1->data[1])
			P1->data[1] = rec->P1->data[1];
		/// para P2 y
		if (P2->data[1] > rec->P2->data[1])
			P2->data[1] = rec->P2->data[1];
	}
}

/// insertamos una figura(rectangulo) dentro de mi arbol
void RTree::insert(Rec *fig) {
	Box *box = chooseLeaf(fig);
	if (box->index < box->maxSize) {
		cout << "Se inserto la figura sin cambios" << endl;
		box->nodes[box->index++] = fig;
		fig->boxPadre = box;
		adjustTree(box);
	} else {
		cout << "Se hizo un split" << endl;
		box->nodes[box->index++] = fig;
		fig->boxPadre = box;
		box->leaf = false;
		
		/// 1) Choose Seeds the two that makes the bigger area
		double newArea, maxArea = -1e9;
		int indexR1, indexR2;
		/// Formula: Area(E1,E2) - (Area(E1) + Area(E2))
		for (int i = 0; i < box->index; ++i) {
			for (int j = i + 1; j < box->index; ++j) {
				newArea = computeArea(box->nodes[i], box->nodes[j]) -
					(computeArea(box->nodes[i]) + computeArea(box->nodes[j]));
				if (maxArea < newArea) {
					maxArea = newArea;
					indexR1 = i;
					indexR2 = j;
				}
			}
		}
		/// save R1 and R2
		Rec *R1, *R2;
		R1 = box->nodes[indexR1];
		R2 = box->nodes[indexR2];
		
		/// create new boxes for R1 and R2
		R1->boxHijo = new Box(box->maxSize);
		R2->boxHijo = new Box(box->maxSize);
		R1->boxHijo->recPadre = R1;
		R2->boxHijo->recPadre = R2;
		
		/// añadir los dos hijos a cada uno
		Rec *newR1 = new Rec(1);
		Rec *newR2 = new Rec(1);
		newR1->copy(R1);
		newR2->copy(R2);
		R1->boxHijo->nodes[R1->boxHijo->index++] = newR1;
		R2->boxHijo->nodes[R2->boxHijo->index++] = newR2;
		
		int index = 0;
		/// Save the ones I have to put somewhere
		vector < Rec * > looseRec(box->index - 2);
		for (int i = 0; i < box->index; ++i)
			if (i != indexR1 and i != indexR2)
				looseRec[index++] = box->nodes[i];
		
		/// empty the vector
		while (!box->nodes.empty())
			box->nodes.pop_back();
		box->nodes.resize(4);
		box->index = 2;
		box->nodes[0] = R1;
		box->nodes[1] = R2;
		
		///Pick the next who's enlargement area will be the least
		double areaR1, areaR2;
		Rec *next;
		while (!looseRec.empty()) {
			/// if one side needs the rest entries, give them all
			if (R1->boxHijo->index == R1->boxHijo->maxSize - R1->boxHijo->index + 1) {
				while (!looseRec.empty()) {
					R2->boxHijo->nodes[R2->boxHijo->index++] = looseRec.back();
					looseRec.back()->boxPadre = R2->boxHijo;
					looseRec.pop_back();
					R2->adjustRec();
				}
			} else if (R2->boxHijo->index == R2->boxHijo->maxSize - R2->boxHijo->index + 1) {
				while (!looseRec.empty()) {
					R1->boxHijo->nodes[R1->boxHijo->index++] = looseRec.back();
					looseRec.back()->boxPadre = R1->boxHijo;
					looseRec.pop_back();
					R1->adjustRec();
				}
			} else {
				/// the next one is deleted in chooseNext
				next = chooseNext(looseRec, R1, R2);
				areaR1 = computeArea(next, R1);
				areaR2 = computeArea(next, R2);
				if (areaR1 < areaR2) {
					R1->boxHijo->nodes[R1->boxHijo->index++] = next;
					R1->adjustRec();
				} else {
					R2->boxHijo->nodes[R2->boxHijo->index++] = next;
					R2->adjustRec();
				}
			}
		}
		adjustTree(R1->boxHijo);
		adjustTree(R2->boxHijo);
	}
}

void Box::reset(Box* box) {
	if (box->leaf) {
		for(auto &rec : box->nodes) {
			if(!rec) break;
			rec->cond = false;
		}
	}
	else {
		for(auto &rec : box->nodes) {
			if(!rec) break;
			reset(rec->boxHijo);
		}
	}
}

void RTree::reset() {
	Box* box = root;
	box->reset(box);
}

bool Box::doOverlap(Point* l1, Point* r1, Point* l2, Point* r2) {
	/// If one rectangle is on left side of other 
	if (l1->data[0] > r2->data[0] or l2->data[0] > r1->data[0]) 
		return false; 	
	/// If one rectangle is above other 
	if (l1->data[1] < r2->data[1] or l2->data[1] < r1->data[1]) 
		return false; 
	return true;
}

bool Box::overlaps(Rec* rec1, Rec* rec2) {
	return doOverlap(rec1->P1, rec1->P2, rec2->P1, rec2->P2);
}

void RTree::find(Rec* recAux) {
	Box* box = root;
	box->find(box, recAux);
}

void Box::find(Box* box, Rec* recAux) {
	if(box->leaf){
		for (auto &rec : box->nodes) {
			if(!rec) return;
				rec->cond = overlaps(rec, recAux);
		}
	}
	else {
		for (auto &rec : box->nodes) {
			if(!rec) return;
			if(overlaps(rec,recAux) )
				find(rec->boxHijo, recAux);
		}
	}
}

Rec::~Rec() {
	if (!leaf) 
		delete boxHijo;
	
	for (int i = 0; i < dots.size(); ++i) 
		delete dots[i];
	
	dots.clear();
	delete P1, P2;
}

Box::~Box() {
	for (int i = 0; i < index; ++i) {
		delete nodes[i];
	}
	delete P1, P2;
}

RTree::~RTree() {
	delete root;
}


