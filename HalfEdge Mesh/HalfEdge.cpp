# include <GL/gl.h>
# include <GL/glu.h>

#include "HalfEdge.h"
#include "kdTree.h"

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <cmath>
#include <vector>
#include <math.h>
#include <thread>
#include <algorithm>

#define M_PI    3.14159265358979323846

int Edge::edges = 0;
int Face::faces = 0;

///ver displaylist para dejarlo en el gpu
/// o pasar ver 4.6
/// ver en tutorial como tranformar con una libreria
/// gl algo

HalfEdge::HalfEdge() {
	massCenter.resize(3);
	massCenter[0] = 0;
	massCenter[1] = 0;
	massCenter[2] = 0;
	beta.resize(20);
	beta[0] = 0;
	//cout<<"cos: "<<5.0f/8.0f-pow(3.0f/8.0f+1.0f/4.0f*cos(2.0f*M_PI/2.0f),2)<<endl;
	for (float i = 1; i < 20; ++i) {
		float potencia = pow(3.0f / 8.0f + 1.0f / 4.0f * cos(2.0f * M_PI / i), 2);
		beta[i] = (1.0f / i) * (5.0f / 8.0f - potencia);
	}
}

bool HalfEdge::getWorked() {
	return worked;
}

/// returns x, y and z
float *HalfEdge::sep(string line, string del) {
	string a = line.substr(0, line.find(del));
	line = line.substr(line.find(del) + 1);
	string b = line.substr(0, line.find(del));
	line = line.substr(line.find(del) + 1);
	string c = line;
	float *vec = new float[3];
	vec[0] = stof(a);
	vec[1] = stof(b);
	vec[2] = stof(c);
	return vec;
}

/// returns the vertexs by id
vector<Vertex *> HalfEdge::sep2(string line, string del) {
	string a = line.substr(0, line.find(del));
	line = line.substr(line.find(del) + 1);
	string b = line.substr(0, line.find(del));
	line = line.substr(line.find(del) + 1);
	string c = line;
	a = a.substr(0, a.find('/'));
	b = b.substr(0, b.find('/'));
	c = c.substr(0, c.find('/'));
	vector<Vertex *> vec(3);
	vec[0] = findVertex(stoi(a));
	vec[1] = findVertex(stoi(b));
	vec[2] = findVertex(stoi(c));
	return vec;
}

/// Returns the 3 ids of vertexs passed on
vector<int> HalfEdge::findVertexF(int start, Vertex *ver1, Vertex *ver2, Vertex *ver3) {
	vector<int> m(3, 0);
	bool cond1 = false, cond2 = false, cond3 = false;
	for (int i = start; i < LVertex.size(); ++i) {
		if (cond1 and cond2 and cond3) return m;
		if (ver1->data[0] == LVertex[i]->data[0] and ver1->data[1] == LVertex[i]->data[1] and
			ver1->data[2] == LVertex[i]->data[2]) {
			m[0] = LVertex[i]->id;
			cond1 = true;
		} else if (ver2->data[0] == LVertex[i]->data[0] and ver2->data[1] == LVertex[i]->data[1] and
				   ver2->data[2] == LVertex[i]->data[2]) {
			m[1] = LVertex[i]->id;
			cond2 = true;
			} else if (ver3->data[0] == LVertex[i]->data[0] and ver3->data[1] == LVertex[i]->data[1] and
					   ver3->data[2] == LVertex[i]->data[2]) {
				m[2] = LVertex[i]->id;
				cond3 = true;
				   }
	}
	return m;
}

/// borrar un dato dentro de mi hash table
void erasefrom(vector<Edge *> &notMatched, Edge *e2) {
	for (int i = 0; i < notMatched.size(); ++i)
		if (notMatched[i] == e2)
			notMatched.erase(notMatched.begin() + i);
}

/// calcular la normal de cada vertice segun sus caras vecinas
bool HalfEdge::calculateNormal() {
	for (auto &ver : LVertex) {
		Edge *e = ver->edge;
		ver->normal[0] = 0;
		ver->normal[1] = 0;
		ver->normal[2] = 0;
		do {
			Face *f = e->leftFace;
			ver->normal[0] += f->normal[0];
			ver->normal[1] += f->normal[1];
			ver->normal[2] += f->normal[2];
			if (!e->twin) {
				cout << "The mesh has loose edges" << endl;
				return false;
			}
			e = e->twin->next;
			
		} while (e != ver->edge);
		float promedio = sqrt(pow(ver->normal[0], 2) + pow(ver->normal[1], 2) + pow(ver->normal[2], 2));
		ver->normal[0] /= promedio;
		ver->normal[1] /= promedio;
		ver->normal[2] /= promedio;
	}
	return true;
}

/// Guardo las posiciones originales de los vertices en px
/// lo utilizo a la hora de dividir la malla
void HalfEdge::savePos() {
	for (auto &ver : LVertex) {
		ver->pData[0] = ver->data[0];
		ver->pData[1] = ver->data[1];
		ver->pData[2] = ver->data[2];
		ver->cond = false;
	}
}

void HalfEdge::scale(float factor) {
	for (auto &vec : LVertex) {
		for (int i = 0; i < 3; ++i)
			vec->data[i] *= factor;//= massCenter[i] + factor * vec->data[i];
	}
}

/// Dividir la malla usando tabla hash
void HalfEdge::divideLoopHash() {
	// 1 guardar prev positions
	chrono::system_clock::time_point t;
	chrono::duration<float, milli> r;
	t = chrono::system_clock::now();
	
	// 2 mover los vertices segun sus vecinos
	auto *coef = new float[3];
	
	/// Muevo los vertices originales
	for (auto vertex : LVertex) {
		int n = vertex->getLevel();
		// sumatoria de los vecinos B*P_i
		Edge *e = vertex->edge;
		Vertex *v;
		coef[0] = 0;
		coef[1] = 0;
		coef[2] = 0;
		do {
			v = e->head;
			coef[0] += v->pData[0] * beta[n];
			coef[1] += v->pData[1] * beta[n];
			coef[2] += v->pData[2] * beta[n];
			e = e->twin->next;
		} while (e != vertex->edge);
		// sumatoria de los vecinos + (1-nB)*Vertex
		vertex->data[0] = vertex->pData[0] * (1 - n * beta[n]) + coef[0];
		vertex->data[1] = vertex->pData[1] * (1 - n * beta[n]) + coef[1];
		vertex->data[2] = vertex->pData[2] * (1 - n * beta[n]) + coef[2];
		
		massCenter[0] += vertex->pData[0];
		massCenter[1] += vertex->pData[1];
		massCenter[2] += vertex->pData[2];
	}
	delete[] coef;
	//	r = chrono::system_clock::now()-t;
	//	cout<<"Move points time: "<<r.count()/1000<<" seg"<<endl;
	//	t = chrono::system_clock::now();
	
	/// bajo esta malla creo las nuevas caras y aristas asi como su tabla hash
	auto *newMalla = new HalfEdge();
	
	massCenter[0] = 0;
	massCenter[1] = 0;
	massCenter[2] = 0;
	
	chrono::system_clock::time_point t1, t2;
	chrono::duration<float, milli> r1, r2;
	float sum = 0, sum2 = 0, sumFind = 0;
	int cont = 0;
	int size = LVertex.size();
	
	KdTree *Tree = new KdTree(3);
	
	/// crear vertices segun las caras
	int id = LVertex[LVertex.size() - 1]->id + 1;
	for (auto &face : LFace) {
		t2 = chrono::system_clock::now();
		vector<vector<int>> newFaces;
		vector<int> f(3);
		Edge *e1 = face->first;
		Vertex *v1 = e1->tail;
		Vertex *v2 = e1->next->tail;
		Vertex *v3 = e1->prev->tail;
		auto *mv1 = new Vertex((v1->pData[0] + v2->pData[0]) / 2.0f, (v1->pData[1] + v2->pData[1]) / 2.0f,
			(v1->pData[2] + v2->pData[2]) / 2.0f, id++);
		auto *mv2 = new Vertex((v2->pData[0] + v3->pData[0]) / 2.0f, (v2->pData[1] + v3->pData[1]) / 2.0f,
			(v2->pData[2] + v3->pData[2]) / 2.0f, id++);
		auto *mv3 = new Vertex((v3->pData[0] + v1->pData[0]) / 2.0f, (v3->pData[1] + v1->pData[1]) / 2.0f,
			(v3->pData[2] + v1->pData[2]) / 2.0f, id++);
		/// cambiar la forma de buscar
		/// busco mediante mi Kd-Tree
		
		/// Se puede mejorar con threads pero no funciona con zinjai
		vector<int> m(3);
		m[0] = Tree->RecFind(mv1);
		m[1] = Tree->RecFind(mv2);
		m[2] = Tree->RecFind(mv3);
		/** m
		Si m es diferente de 0 es que ya existe el vertice
		sino debe ser agregado tanto a la lista de vertices como al Kd-Tree
		**/
		
		//		auto m = findVertexF(size,mv1,mv2,mv3);
		//		cout<<"M: "<<m[0]<< " "<<m[1]<<" "<<m[2]<<endl;
		//		cout<<"Antes: "<<mv1->id<<" "<<mv2->id<<" "<<mv3->id<<endl;
		
		/// check if vertex already exists
		
		////////HACER UN FOR
		bool cond1 = false, cond2 = false, cond3 = false;
		if (m[0]) {
			delete mv1;
			--id;
		} else {
			massCenter[0] += mv1->data[0];
			massCenter[1] += mv1->data[1];
			massCenter[2] += mv1->data[2];
			LVertex.push_back(mv1);
			mv1->cond = true;
			cond1 = true;
			m[0] = mv1->id;
		}
		if (m[1]) {
			delete mv2;
			--id;
		} else {
			massCenter[0] += mv2->data[0];
			massCenter[1] += mv2->data[1];
			massCenter[2] += mv2->data[2];
			LVertex.push_back(mv2);
			mv2->cond = true;
			cond2 = true;
			m[1] = mv2->id;
		}
		if (m[2]) {
			delete mv3;
			--id;
		} else {
			massCenter[0] += mv3->data[0];
			massCenter[1] += mv3->data[1];
			massCenter[2] += mv3->data[2];
			LVertex.push_back(mv3);
			mv3->cond = true;
			cond3 = true;
			m[2] = mv3->id;
		}
		
		
		/// reduce id to fix vertex id
		/// fix i mean reduce id number if necessary
		if (!cond1) {
			if (cond2) {
				--m[1];
				mv2->id = m[1];
			}
			if (cond3) {
				--m[2];
				mv3->id = m[2];
			}
		} else
			Tree->recIns(mv1);
		
		if (!cond2) {
			if (cond3) {
				--m[2];
				mv3->id = m[2];
			}
		} else
			Tree->recIns(mv2);
		
		if (cond3) {
			Tree->recIns(mv3);
		}
		
		//		cout<<"Antes: "<<mv1->id<<" "<<mv2->id<<" "<<mv3->id<<endl;
		//		cout<<"M: "<<m[0]<< " "<<m[1]<<" "<<m[2]<<endl;
		//		cout<<"Despues: "<<mv1->id<<" "<<mv2->id<<" "<<mv3->id<<endl;
		
		/// assign faces
		f[0] = v1->id;
		f[1] = m[0];
		f[2] = m[2];
		newFaces.push_back(f);
		f[0] = m[0];
		f[1] = v2->id;
		f[2] = m[1];
		newFaces.push_back(f);
		f[0] = m[0];
		f[1] = m[1];
		f[2] = m[2];
		newFaces.push_back(f);
		f[0] = m[2];
		f[1] = m[1];
		f[2] = v3->id;
		newFaces.push_back(f);
		
		r2 = chrono::system_clock::now() - t2;
		sum2 += r2.count() / 1000;
		/// create faces
		t1 = chrono::system_clock::now();
		/// segun las 4 caras creadas vamos a anhadirlas a nuestra "newMalla"
		for (auto &face : newFaces) {
			vector<Vertex *> vertices(3);// = find3Vertex(face[0],face[1],face[2]);
			/// asign Vertexs
			vertices[0] = LVertex[face[0] - 1];
			vertices[1] = LVertex[face[1] - 1];
			vertices[2] = LVertex[face[2] - 1];
			Edge *e1 = new Edge(vertices[0], vertices[1], 0, 0, 0, 0);
			Edge *e2 = new Edge(vertices[1], vertices[2], e1, 0, 0, 0);
			Edge *e3 = new Edge(vertices[2], vertices[0], e2, e1, 0, 0);
			e1->prev = e3;
			e1->next = e2;
			e2->next = e3;
			
			///asignar edge a Vertex
			vertices[0]->edge = e1;
			vertices[1]->edge = e2;
			vertices[2]->edge = e3;
			
			Face *fa = new Face(e1);
			e1->leftFace = fa;
			e2->leftFace = fa;
			e3->leftFace = fa;
			normalFace(fa);
			
			newMalla->LEdge.push_back(e1);
			newMalla->LEdge.push_back(e2);
			newMalla->LEdge.push_back(e3);
			newMalla->LFace.push_back(fa);
			
			/// anhadir o encontrar un twin en el hash table
			vector<Edge *> newEdges(3);
			newEdges[0] = e1;
			newEdges[1] = e2;
			newEdges[2] = e3;
			for (auto &e : newEdges) {
				if (!e->twin) {
					vector<str> vec = newMalla->notMatched.vec[e->head->id];
					int i;
					for (i = 0; i < vec.size(); ++i)
						if (vec[i].tail == e->head->id and vec[i].head == e->tail->id)
							break;
					
					if (i == vec.size()) {
						str struc(e->tail->id, e->head->id, e);
						newMalla->notMatched.vec[e->tail->id].push_back(struc);
					} else {
						vec[i].e->twin = e;
						e->twin = vec[i].e;
						newMalla->notMatched.erase(e->head->id, i);
					}
				}
			}
		}
		r1 = chrono::system_clock::now() - t1;
		sum += r1.count() / 1000;
	}
	id = LVertex[LVertex.size() - 1]->id;
	massCenter[0] /= id;
	massCenter[1] /= id;
	massCenter[2] /= id;
	
	deleteOldMesh();
	LEdge = newMalla->LEdge;
	LFace = newMalla->LFace;
	Tree->deleteAll(Tree->getRoot());
	delete Tree;
	calculateNormal();
	r = chrono::system_clock::now() - t;
	cout << "Time to build vertexs: " << sum2 << endl;
	cout << "Time to build face: " << sum - sumFind << endl;
	cout << "Time to find vertexs: " << sumFind << endl;
	cout << "Hash table build time: " << r.count() / 1000 << " seg" << endl;
	cout << "Number of vertex: " << LVertex.size() << endl << endl;
}

/// liberar la memoria de las antiguas aristas y caras
void HalfEdge::deleteOldMesh() {
	bool cond = true;
	while (cond) {
		cond = false;
		if (!LEdge.empty()) {
			delete LEdge.back();
			LEdge.pop_back();
			cond = true;
		}
		if (!LFace.empty()) {
			delete LFace.back();
			LFace.pop_back();
			cond = true;
		}
	}
}

/// retorna el vertice segun el id buscado, se usa en readOBJ
Vertex *HalfEdge::findVertex(int id) {
	for (auto &vertex : LVertex)
		if (vertex->id == id)
			return vertex;
	return 0;
}

/// Busqueda lineal de 3 vertices segun sus ids, no me acuerdo donde se usa
vector<Vertex *> HalfEdge::find3Vertex(int id1, int id2, int id3) {
	vector<Vertex *> vec(3);
	for (auto &vertex : LVertex) {
		if (vertex->id == id1)
			vec[0] = vertex;
		else if (vertex->id == id2)
			vec[1] = vertex;
		else if (vertex->id == id3)
			vec[2] = vertex;
	}
	return vec;
}


void HalfEdge::crossProduct(double *normal, double *AB, double *AC) {
	normal[0] = (AB[1] * AC[2]) - (AB[2] * AC[1]);
	normal[1] = (AB[2] * AC[0]) - (AB[0] * AC[2]);
	normal[2] = (AB[0] * AC[1]) - (AB[1] * AC[0]);
}

double HalfEdge::dotProduct(double *p1, double *p2) {
	return p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2];
}

/// hallar la normal de una cara
void HalfEdge::normalFace(Face *f) {
	Edge *e = f->first;
	Vertex *v1 = e->tail;
	Vertex *v2 = e->head;
	Vertex *v3 = e->next->head;
	double AB[3];
	AB[0] = v2->data[0] - v1->data[0];
	AB[1] = v2->data[1] - v1->data[1];
	AB[2] = v2->data[2] - v1->data[2];
	
	double AC[3];
	AC[0] = v3->data[0] - v1->data[0];
	AC[1] = v3->data[1] - v1->data[1];
	AC[2] = v3->data[2] - v1->data[2];
	
	//float deter = sqrt(pow();
	crossProduct(f->normal, AB, AC);
	//	f->normal[0] = (AB[1] * AC[2]) - (AB[2] * AC[1]);
	//	f->normal[1] = (AB[0] * AC[2]) - (AB[2] * AC[0]);
	//	f->normal[2] = (AB[0] * AC[1]) - (AB[1] * AC[0]);
	double norm = sqrt(pow(f->normal[0], 2) + pow(f->normal[1], 2) + pow(f->normal[2], 2));
	f->normal[0] /= norm;
	f->normal[1] /= norm;
	f->normal[2] /= norm;
}

void HalfEdge::deColor() {
	for (auto &ver : LVertex)
		ver->cond = false;
	for(auto &edge : LEdge)
		edge->cond = false;
}

/****** Fusion de dos mallas
1) marcar los vertices que deben ser suprimidos( los que estan dentro de ambas mallas) con ray tracing
Se puede mejorar con una particion espacial como un octree 90% del tiempo ahorrado
2)
3)
4)
5)
6)

*******/


vector<Face *> HalfEdge::getLFace() {
	return LFace;
}

/// Colorea marcando los puntos que deben ser eliminados
float* HalfEdge::rayTriangleIntersect(double orig[3],
									double dir[3],
									double vertices[3][3],
										double normal[3]) {
	double epsilon = 1e-7, NdotRayDirction, d, t, normalOrgin;
	int i;
	
	//Finding the point P(Intersection between plane and line)
	NdotRayDirction = dotProduct(normal, dir);
	if (fabs(NdotRayDirction) < epsilon)
		return NULL;
	
	d = dotProduct(normal, vertices[0]);
	
	normalOrgin = dotProduct(normal, orig);
	
	t = (d - normalOrgin) / NdotRayDirction;
	
	if (t < 0)
		return NULL;
	
	
	// punto intersecado en el plano(triangulo)
	float *p = new float[3];
	for (i = 0; i < 3; ++i)
		p[i] = orig[i] + t * dir[i];
	
	double NdotProductC = 0.0;
	
	//Inside-Outside triangule
	double edge[3], vp[3], C[3];
	
	for (int j = 0; j < 3; ++j) {
		for (i = 0; i < 3; ++i) {
			edge[i] = vertices[(j + 1) % 3][i] - vertices[j][i];
			vp[i] = p[i] - vertices[j][i];
		}
		crossProduct(C, edge, vp);
		NdotProductC = dotProduct(normal, C);
		if (NdotProductC < 0) {
			delete p;
			return NULL;
		}
	}
	return p;
}

void HalfEdge::colorVertex(HalfEdge *Mesh, float movePos[2][3], int index) {
	double dir[3];
	double *v0;
	double *v1;
	double *v2;
	double orig[3];
	int iMesh = (index + 1) % 2;
	for (auto &ver : LVertex) {
		for (int i = 0; i < 3; ++i) {
			dir[i] = Mesh->massCenter[i] + movePos[iMesh][i] -
				(ver->data[i] + movePos[index][i]);
			orig[i] = ver->data[i] + movePos[index][i];
		}
		//		dir[0] = Mesh->massCenter[0] - (ver->data[0];
		//		dir[1] = Mesh->massCenter[1] - ver->data[1];
		//		dir[2] = Mesh->massCenter[2] - ver->data[2];
		int par = 0;
		double vertices[3][3];
		for (auto &face : Mesh->LFace) {
			Edge *e = face->first;
			v0 = e->head->data;
			v1 = e->tail->data;
			v2 = e->next->head->data;
			for (int i = 0; i < 3; ++i) {
				vertices[0][i] = v0[i] + movePos[iMesh][i];
				vertices[1][i] = v1[i] + movePos[iMesh][i];
				vertices[2][i] = v2[i] + movePos[iMesh][i];
			}
			// hallar la cara no normalizada
			// necesitamos volver a hallarla porque la otra esta normalizada
			double v0v1[3];
			double v0v2[3];
			v0v1[0] = vertices[1][0] - vertices[0][0];
			v0v1[1] = vertices[1][1] - vertices[0][1];
			v0v1[2] = vertices[1][2] - vertices[0][2];
			
			v0v2[0] = vertices[2][0] - vertices[0][0];
			v0v2[1] = vertices[2][1] - vertices[0][1];
			v0v2[2] = vertices[2][2] - vertices[0][2];
			
			double normal[3];
			crossProduct(normal, v0v1, v0v2);
			
			float* p = rayTriangleIntersect(orig, dir, vertices, normal);
			if (p) {
				++par;
				delete p;
			}
		}
		if (par % 2)
			ver->cond = true;
	}
}

void HalfEdge::colorLine(HalfEdge* Mesh, float movePos[2][3], int index){
	double dir[3];
	double *v0;
	double *v1;
	double *v2;
	double *_v0;
	double *_v1;
	double orig[3];
	int iMesh = (index + 1) % 2;
	double vertices[3][3];
	for(auto &edge : LEdge) {
		_v0 = edge->tail->data;
		_v1 = edge->head->data;
		for(int i = 0; i < 3; ++i)
			dir[i] = _v1[i] - _v0[i];
		
		for (auto &face : Mesh->LFace) {
			Edge *e = face->first;
			v0 = e->head->data;
			v1 = e->tail->data;
			v2 = e->next->head->data;
			for (int i = 0; i < 3; ++i) {
				vertices[0][i] = v0[i] + movePos[iMesh][i];
				vertices[1][i] = v1[i] + movePos[iMesh][i];
				vertices[2][i] = v2[i] + movePos[iMesh][i];
			}
			
			// hallar la cara no normalizada
			// necesitamos volver a hallarla porque la otra esta normalizada
			double v0v1[3];
			double v0v2[3];
			v0v1[0] = vertices[1][0] - vertices[0][0];
			v0v1[1] = vertices[1][1] - vertices[0][1];
			v0v1[2] = vertices[1][2] - vertices[0][2];
			
			v0v2[0] = vertices[2][0] - vertices[0][0];
			v0v2[1] = vertices[2][1] - vertices[0][1];
			v0v2[2] = vertices[2][2] - vertices[0][2];
			
			double normal[3];
			crossProduct(normal, v0v1, v0v2);
			
			float* p = rayTriangleIntersect(orig, dir, vertices, normal);
			if (p) {
				double a[3],b[3];
				for(int i = 0; i < 3; ++i){
					a[i] = _v1[i] - _v0[i];
					b[i] = p[i] - _v0[0];
				}
				double product = dotProduct(a,b);
				double square = pow(a[0],2) + pow(a[1],2) + pow(a[2],2);
				if(product < square)
					edge->cond = true;
				
				delete p;
			}
		}
	}
}

/// leo el OBJ creando los vertices y caras con una tabla hash, no tan optimizado
void HalfEdge::readOBJ(string figure) {
	/** lista de nombre de OBJs
	cube
	orange
	icoalgo
	face
	sword
	fox
	lion
	barrel
	canteen 300 300
	demon 4k 2k
	EnclaveBot 5k 3k
	VaultDoor 5k 4k
	elephant
	NCRHelmet 16k 16k
	doom 80k 40k
	T45Suit 260k 260k  lines:786k
	Gun 400k 260k
	pelico 400k 450k
	isaac 700k
	**/
	ifstream read;
	read.open("OBJ/" + figure + ".obj", ios::in);
	string line;
	// 0=xmin 1=xmax 2=ymin 3=ymax 4=zmin 5=zmax
	int id = 1;
	cout << "\nFigura nueva" << endl;
	chrono::system_clock::time_point t;
	std::chrono::duration<float, std::milli> r;
	t = chrono::system_clock::now();
	while (getline(read, line)) {
		string sub = line.substr(0, line.find(" "));
		line = line.substr(line.find(' ') + 1);
		if (sub == "v") {
			float *punto = sep(line, " "); // get x y z
			insertVertex(punto[0], punto[1], punto[2], id++);
			massCenter[0] += punto[0];
			massCenter[1] += punto[1];
			massCenter[2] += punto[2];
		} else if (sub == "f") {
			vector<Vertex *> vertices = sep2(line, " "); // get v1 v2 v3
			//create edges between each vertex in order
			//tail head prev next twin face
			Edge *e1 = new Edge(vertices[0], vertices[1], 0, 0, 0, 0);
			Edge *e2 = new Edge(vertices[1], vertices[2], e1, 0, 0, 0);
			Edge *e3 = new Edge(vertices[2], vertices[0], e2, e1, 0, 0);
			e1->prev = e3;
			e1->next = e2;
			e2->next = e3;
			LEdge.push_back(e1);
			LEdge.push_back(e2);
			LEdge.push_back(e3);
			
			//asignar edge a Vertex
			vertices[0]->edge = e1;
			vertices[1]->edge = e2;
			vertices[2]->edge = e3;
			
			Face *fa = new Face(e1);
			//calcular la normal
			normalFace(fa);
			
			//asignar padre
			e1->leftFace = fa;
			e2->leftFace = fa;
			e3->leftFace = fa;
			LFace.push_back(fa);
			
			// anhadir o encontrar un twin en el hash table
			vector<Edge *> newEdges(3);
			newEdges[0] = e1;
			newEdges[1] = e2;
			newEdges[2] = e3;
			for (auto &e : newEdges) {
				vector<str> vec = notMatched.vec[e->head->id];
				int i;
				for (i = 0; i < vec.size(); ++i)
					if (vec[i].tail == e->head->id and vec[i].head == e->tail->id)
						break;
				
				if (i == vec.size()) {
					notMatched.n++;
					str struc(e->tail->id, e->head->id, e);
					notMatched.vec[e->tail->id].push_back(struc);
				} else {
					vec[i].e->twin = e;
					e->twin = vec[i].e;
					notMatched.erase(e->head->id, i);
				}
				
			}
		}
	}
	--id;
	for (int i = 0; i < 3; ++i)
		massCenter[i] /= id;
	worked = calculateNormal();
	r = chrono::system_clock::now() - t;
	cout << "With hash Total Time: " << r.count() / 1000 << " seg" << endl;
}

void HalfEdge::insertVertex(float x, float y, float z, int id) {
	Vertex *v = new Vertex(x, y, z, id);
	LVertex.push_back(v);
}

void HalfEdge::modifyPos(int index, float factor) {
	for (auto &ver : LVertex) {
		//massCenter[index]+=factor;
		ver->data[index] += factor;
	}
}

void HalfEdge::computeMassCenter(float *newPos) {
	massCenter[0] = 0;
	massCenter[1] = 0;
	massCenter[2] = 0;
	for (auto &vec : LVertex) {
		for (int i = 0; i < 3; ++i)
			massCenter[i] += vec->data[i] + newPos[i];
	}
	for (int i = 0; i < 3; ++i)
		massCenter[i] /= LVertex[LVertex.size() - 1]->id;
}

/** Parametros draw
drawT = dibujar las triangulos(caras)
drawL = dibujar las lineas(aristas)
drawP = dibujar los puntos(vertices)
movePos = La posicion en la que los vertices deben ser movidos
**/
void HalfEdge::draw(bool drawT, bool drawL, bool drawP, float *movePos) {
	Vertex *v1;
	Vertex *v2;
	Vertex *v3;
	glPointSize(15);
	glColor3f(1, 1, 1);
	glBegin(GL_POINTS);
	glVertex3f(massCenter[0], massCenter[1], massCenter[2]);
	glEnd();
	
	/*for(auto e : LEdge){
	glColor3f(1,1,1);
	if(!e->twin){
	glBegin(GL_LINES);
	glVertex3d(e->head->data[0] + movePos[0], e->head->data[1] + movePos[1], e->head->data[2] + movePos[2]);
	glVertex3d(e->tail->data[0] + movePos[0], e->tail->data[1] + movePos[1], e->tail->data[2] + movePos[2]);
	glEnd();
	}
	}*/
	
	if (drawP) {
		for (auto &v : LVertex) {
			if (v->cond)
				glColor3f(1.0f, 0.0f, 0.0f);
			else
				glColor3f(1.0f, 1.0f, 1.0f);
			glPointSize(8);
			glBegin(GL_POINTS);
			glVertex3f(v->data[0] + movePos[0], v->data[1] + movePos[1], v->data[2] + movePos[2]);
			glEnd();
		}
	}
	
	for(auto &e : LEdge){
		if(e->cond){
			glLineWidth(5);
			glColor3f(0.5f, 0.0f, 0.f);
			glBegin(GL_LINES);
				glVertex3d(e->head->data[0] + movePos[0], e->head->data[1] + movePos[1], e->head->data[2] + movePos[2]);
				glVertex3d(e->tail->data[0] + movePos[0], e->tail->data[1] + movePos[1], e->tail->data[2] + movePos[2]);
			glEnd();
		} else {
			glLineWidth(1);
			glColor3f(1, 1, 1);
			glBegin(GL_LINES);
				glVertex3d(e->head->data[0] + movePos[0], e->head->data[1] + movePos[1], e->head->data[2] + movePos[2]);
				glVertex3d(e->tail->data[0] + movePos[0], e->tail->data[1] + movePos[1], e->tail->data[2] + movePos[2]);
			glEnd();
		}
	}
	
	for (auto &f : LFace) {
		Edge *e1 = f->first;
		Vertex *v1 = e1->head;
		e1 = e1->next;
		Vertex *v2 = e1->head;
		e1 = e1->next;
		Vertex *v3 = e1->head;
		glPointSize(4);
		if (drawT) {
			glColor3f(0.5f, 0.0f, 0.5f);
			//glNormal3f(f->normal[0],f->normal[1],f->normal[2]);
			if (worked) {
				glBegin(GL_TRIANGLES);
				glNormal3f(v1->normal[0], v1->normal[1], v1->normal[2]);
				glVertex3f(v1->data[0] + movePos[0], v1->data[1] + movePos[1], v1->data[2] + movePos[2]);
				glNormal3f(v2->normal[0], v2->normal[1], v2->normal[2]);
				glVertex3f(v2->data[0] + movePos[0], v2->data[1] + movePos[1], v2->data[2] + movePos[2]);
				glNormal3f(v3->normal[0], v3->normal[1], v3->normal[2]);
				glVertex3f(v3->data[0] + movePos[0], v3->data[1] + movePos[1], v3->data[2] + movePos[2]);
				glEnd();
			} else {
				glNormal3f(f->normal[0], f->normal[1], f->normal[2]);
				glBegin(GL_TRIANGLES);
				glVertex3f(v1->data[0] + movePos[0], v1->data[1] + movePos[1], v1->data[2] + movePos[2]);
				glVertex3f(v2->data[0] + movePos[0], v2->data[1] + movePos[1], v2->data[2] + movePos[2]);
				glVertex3f(v3->data[0] + movePos[0], v3->data[1] + movePos[1], v3->data[2] + movePos[2]);
				glEnd();
			}
		}
		if (drawL) {
			glLineWidth(1);
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_LINE_LOOP);
			glVertex3f(v1->data[0] + movePos[0], v1->data[1] + movePos[1], v1->data[2] + movePos[2]);
			glVertex3f(v2->data[0] + movePos[0], v2->data[1] + movePos[1], v2->data[2] + movePos[2]);
			glVertex3f(v3->data[0] + movePos[0], v3->data[1] + movePos[1], v3->data[2] + movePos[2]);
			glEnd();
		}
	}
}

vector<Vertex *> HalfEdge::getLVertex() {
	return LVertex;
}

/// liberar la memoria de la malla
HalfEdge::~HalfEdge() {
	bool cond;
	do {
		cond = false;
		if (!LVertex.empty()) {
			Vertex *v = LVertex.back();
			delete v;
			LVertex.pop_back();
			cond = true;
		}
		if (!LEdge.empty()) {
			Edge *e = LEdge.back();
			delete e;
			LEdge.pop_back();
			cond = true;
		}
		if (!LFace.empty()) {
			Face *f = LFace.back();
			delete f;
			LFace.pop_back();
			cond = true;
		}
	} while (cond);
}
