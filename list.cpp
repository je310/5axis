
#include "list.h"

list::~list() {
	node *current,*temp;
	current = head;
	temp = head;
	while(current != NULL) {
		current = current->next;
		delete temp;
		temp = current;
	}
}

struct list::node* list::initNode(Triangle triangle) {
	struct node *ptr = new node;
 
	// error? then just return
	if( ptr == NULL )                         
		return static_cast<struct node *>(NULL);  
	// assign it 
	// then return pointer to ne node
	else {  
		ptr->triangle =triangle;                        
		return ptr;                         
	}
}

// adding to the end of list  
void list::addNode( struct node *newNode )  {
	// if there is no node, put it to head
	if( head == NULL ) {
		head = newNode;  
		tail = newNode;
	}
   
	// link in the new_node to the tail of the list
	// then mark the next field as the end of the list
	// adjust tail to point to the last node

	tail->next = newNode;       
	newNode->next = NULL;       
	tail = newNode;               
}


void list::deleteNode( struct list::node *ptr )
{
  struct node *temp, *prev;
   temp = ptr;    // node to be deleted 
   prev = head;   // start of the list, will cycle to node before temp    

   if( temp == prev ) {                    // deleting first node?  
       head = head->next;                  // moves head to next node     
       if( tail == temp )                  // is it end, only one node?   
          tail = tail->next;               // adjust end as well          
       delete temp ;                       // free up space 
   }
   else {                                  // if not the first node, then 
       while( prev->next != temp ) {       // move prev to the node before
           prev = prev->next;              // the one to be deleted       
       }
       prev->next = temp->next;            // link previous node to next  
       if( tail == temp )                  // if this was the end node,   
           tail = prev;                    // then reset the end pointer  
      delete temp;                         // free up space
   }
}

void list::deleteList( struct node *ptr )
{
   struct node *temp;

   if( head == NULL ) return;   	// don't try to delete an empty list      

   if( ptr == head ) {			// if we are deleting the entire list    
       head = NULL;			// then reset head and    
       tail = NULL;			// end to empty                                     
   }
   else {
       temp = head;			// if it's not the entire list, readjust end   
       while( temp->next != ptr )       // locate previous node to ptr   
           temp = temp->next;
       tail = temp;                     // set end to node before ptr   
   }

   while( ptr != NULL ) {		// whilst there are still nodes to delete      
      temp = ptr->next;			// record address of next node                 
      delete ptr;			// free this node                             
      ptr = temp;			// point to next node to be deleted           
   }
}