include "kernel.h"
struct linkedList {
	TCB * head;
	TCBtail;
	int size;
}

int addFirst(linkedList link, int data){
	node * newNode = malloc(sizeof(node_t));
	newNode->next =  link->head;
	
}

struct linkedList link;
int main(){
	addFirst(link, 5);
}