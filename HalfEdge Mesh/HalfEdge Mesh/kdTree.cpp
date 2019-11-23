#include "kdTree.h"
#include <iostream>
#include <bits/stdc++.h>

int DVertex::ver = 0;

DVertex::DVertex(float x, float y, float z, int id) {
	point[0] = x;
	point[1] = y;
	point[2] = z;
	this->id = id;
	++ver;
	left = NULL;
	right = NULL;
}

DVertex::~DVertex() {
	--ver;
}

void DVertex::operator=(Vertex *v1) {
	point[0] = v1->data[0];
	point[1] = v1->data[1];
	point[2] = v1->data[2];
	id = v1->id;
	left = NULL;
	right = NULL;
}

KdTree::~KdTree() {
	//	while(root)
	//		del(root);
}

KdTree::KdTree(int dim) {
	k = dim;
	root = NULL;
}

bool inline KdTree::isEqual(DVertex *v1, DVertex *v2) {
	return v1->point[0] == v2->point[0] and
		v1->point[1] == v2->point[1] and
		v1->point[2] == v2->point[2];
}

DVertex *KdTree::min(DVertex *v1, DVertex *v2, int cd) {
	if (v1 and v2)
		return (v1->point[cd] < v2->point[cd]) ? v1 : v2;
	if (v1 and !v2)
		return v1;
	if (!v1 and v2)
		return v2;
	else
		return 0;
}

DVertex *KdTree::findMin(DVertex *root, int dim, int prof = 0) {
	if (!root)
		return 0;
	int cd = prof % k;
	if (dim == cd)
		return min(root, findMin(root->left, dim, prof + 1), cd);
	else
		return min(root, min(findMin(root->left, dim, prof + 1),
							 findMin(root->right, dim, prof + 1), cd), cd);
}

int KdTree::RecFind(Vertex *v) {
	DVertex *point = new DVertex(v->data[0], v->data[1], v->data[2], v->id);
	DVertex **node = &root;
	int prof = 0;
	//point = find(root,point,prof);
	//delete point;
	while (true) {
		if (!*node)
			return 0;
		if (isEqual(*node, point))
			return (*node)->id;
		int cd = prof % k;
		if ((*node)->point[cd] > point->point[cd])
			node = &((*node)->left);
		else
			node = &((*node)->right);
		++prof;
	}
}

DVertex *KdTree::find(DVertex *root, DVertex *point, int &prof) {
	while (true) {
		if (!root)
			return NULL;
		if (isEqual(root, point))
			return root;
		int cd = prof % k;
		if (root->point[cd] < point->point[cd])
			root = root->right;
		else
			root = root->left;
		++prof;
	}
}

void KdTree::recIns(Vertex *mv1) {
	if (!root)
		root = new DVertex(mv1->data[0], mv1->data[1], mv1->data[2], mv1->id);
	else
		ins(this->root, mv1);
}

void KdTree::ins(DVertex *root, Vertex *ver) {
	DVertex **node = &root;
	DVertex *mv = new DVertex(ver->data[0], ver->data[1], ver->data[2], ver->id);
	int prof = 0;
	while (true) {
		if (!*node) {
			*node = mv;
			return;
		}
		int cd = prof % k;
		if ((*node)->point[cd] > mv->point[cd])
			node = &((*node)->left);
		else
			node = &((*node)->right);
		++prof;
	}
}

void DVertex::set(DVertex *ver) {
	for (int i = 0; i < 3; ++i)
		point[i] = ver->point[i];
	id = ver->id;
}

void KdTree::deleteAll(DVertex *node) {
	if (node->left)
		deleteAll(node->left);
	if (node->right)
		deleteAll(node->right);
	delete node;
}

bool KdTree::del(DVertex *point) {
	int prof = 0;
	DVertex *p = find(root, point, prof);
	if (p) {
		int pr = 0;
		if (p->left) {
			DVertex *min = findMin(p->left, prof % k, pr);
			p->set(min);
			if (!p->right)
				p->right = min->left;
			else
				while (p->right) {
					del(p->right);
			}
				delete min;
				min = NULL;
		} else if (p->right) {
			DVertex *min = findMin(p->right, prof % k, pr);
			p->set(min);
			delete min;
			min = NULL;
		}
	}
}
