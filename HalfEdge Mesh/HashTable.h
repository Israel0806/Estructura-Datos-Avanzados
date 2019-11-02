#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "LittleStruc.h"

#include <vector>
#include <map>

using namespace std;

struct str {
	int tail, head;
	Edge *e;
	
	str(int t, int h, Edge *e) : tail(t), head(h), e(e) {}
};

struct Hash {
	vector<vector<str> > vec;
	int n = 0;
	
public:
	Hash();
	
	void add(Edge *e);
	
	void erase(int id, int i);
	
	int loose();
	
	~Hash();
};


#endif
