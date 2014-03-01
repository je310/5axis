#ifndef __LIST_INCLUDED__   // if x.h hasn't been included yet...
#define __LIST_INCLUDED__   //   #define this so the compiler knows it has been included


#include <iostream>
#include "triangle.h"

//using namespace irr;


class list
{
public:
	struct node {
		Triangle triangle;
		//core::triangle3d<f32>* PrevTriangle;
		struct node *next;
	};

	node *head, *tail, *ptr;	

	list():head(NULL),tail(NULL){}	// constructor	
	~list();			// destructor

	struct list::node* initNode(Triangle);
	void addNode( struct list::node*);
	void insertNode( struct list::node*);
	void deleteNode( struct list::node*);
	void deleteList( struct list::node*);

};



#endif