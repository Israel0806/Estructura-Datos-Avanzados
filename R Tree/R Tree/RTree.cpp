#include "Rtree.h"

Point::Point(int size){
	this->size=size;
	data.resize(size);
}

Tupla::Tupla(int size){
	maxSize = size;
	index = 0;
	tupla.resize(size);
	for(int i=0;i<size;++i)
		tupla[i] = NULL;//new Node(size);
}

Node::Node(int size){
	P1 = new Point(size); 
	P2 = new Point(size);
	area = 0;
	maxSize = 1;
	leaf = true;
	hijo = NULL;
	padre = NULL;
}

RTree::RTree(int size){
	root = new Tupla(size);
}

Figure* search(){
	
}

Tupla* RTree::chooseLeaf(Figure){
	Tupla* tup = root;
	if(tup->index==0)
		return root;
	while(true){
		/// por cada node debo ver quien tiene el menor cambio
		vector<float> areas(tup->index,0);
		/// necesito hallar las areas cambiadas y devolver el minimo
		/// solo asi veo al cual ir
		for(int i=0;i<tup->index;++i){
			
		}
	}
	
}

/// insertamos una figura(rectangulo) dentro de mi arbol
void RTree::insert(Figure* fig){
	RTree* tree = chooseLeaf();
	
}

void RTree::del(){
	
}
	
void RTree::find(){
	
}
