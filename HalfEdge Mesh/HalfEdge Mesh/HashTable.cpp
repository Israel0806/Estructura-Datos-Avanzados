#include "HashTable.h"

Hash::Hash() {
	vec.resize(1e6);
}

void Hash::add(Edge *e) {
	++n;
	int id = e->tail->id;
	str s(e->tail->id, e->head->id, e);
	vec[id].push_back(s);
}

void Hash::erase(int id, int i) {
	--n;
	vec[id].erase(vec[id].begin() + i);
}

int Hash::loose() {
	return n;
}

Hash::~Hash() {
	vec.clear();
}
