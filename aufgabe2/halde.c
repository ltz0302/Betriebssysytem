#include "halde.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

/// Magic value for occupied memory chunks.
#define MAGIC ((void*)0xbaadf00d)

/// Size of the heap (in bytes).
#define SIZE (1024*1024*1)

/// Memory-chunk structure.
struct mblock {
	struct mblock *next;
	size_t size;
	char memory[];
};

/// Heap-memory area.
char memory[SIZE];

/// Pointer to the first element of the free-memory list.
static struct mblock *head;

void init() {
	head = (struct mblock*)memory;
	head->next = NULL;
	head->size = SIZE - sizeof(struct mblock);
}

/// Helper function to visualise the current state of the free-memory list.
void halde_print(void) {
	struct mblock* lauf = head;

	// Empty list
	if ( head == NULL ) {
		fprintf(stderr, "(empty)\n");
		return;
	}

	// Print each element in the list
    fprintf(stderr, "HEAD:  ");
	while ( lauf ) {
		fprintf(stderr, "(addr: 0x%08zx, off: %7zu, ", (uintptr_t) lauf, (uintptr_t)lauf - (uintptr_t)memory);
		fflush(stderr);
		fprintf(stderr, "size: %7zu)", lauf->size);
		fflush(stderr);

		if ( lauf->next != NULL ) {
			fprintf(stderr, "\n  -->  ");
			fflush(stderr);
		}
		lauf = lauf->next;
	}
	fprintf(stderr, "\n");
	fflush(stderr);
}




void *halde_malloc (size_t size) {
	struct mblock *curr,*prev, *new_mblock;
        void *result;

	if(size==0) return NULL;
	
	
	if(head==NULL) init();

	curr = head;
	prev = head;

	while((curr->size<size)&&(curr->next!=NULL)) { 
		prev = curr;	
       		curr = curr->next;  
	}

	if(curr->size==size) {
		//Delete the middle node of the linked list
		if(curr!=head){
			prev->next = curr->next;
       			curr->next=MAGIC;
        		result = (void *)((size_t)curr+sizeof(struct mblock));
       			return result;
			}
		//Delete the head node of the linked list
		else {
			curr = curr->next;
			head->next = MAGIC;
			result = (void *)((size_t)head+sizeof(struct mblock));
			head = curr;
        	        return result;
			}
		}

	else if((curr->size>=(size+sizeof(struct mblock)))) {
		//Create a new mblock node
		new_mblock = (void *)((size_t)curr + size + sizeof(struct mblock));
       		new_mblock->size = (curr->size) - size - sizeof(struct mblock);
	
		if(curr!=head) {
			//New node instead middle node
			new_mblock->next = curr->next;
			prev->next = new_mblock;
        		curr->size = size;
			curr->next = MAGIC;
			result = (void *)((size_t)curr+sizeof(struct mblock));     	
			return result;
			}
		else{	
			//New node instead head node
			new_mblock->next = head->next;
			curr->size = size;
			curr->next = MAGIC;
			result = (void *)((size_t)head+sizeof(struct mblock));
			head = new_mblock;
			return result;
			}
		}	

	else{
		errno=ENOMEM;
                return NULL;
	}
}

void merge() {
	struct mblock *prev = head, *curr = head;
	
	while(curr!=NULL){
		//Check if two nodes are neighbor
		if(((size_t)prev+prev->size+sizeof(struct mblock))==(size_t)curr){
			prev->size += sizeof(struct mblock) + curr->size;
		       	prev->next = curr->next;
			curr = curr->next;
		}
		else{
			prev = curr;
			curr = curr->next;
		}
	}
	return;
}

void halde_free (void *ptr) {
	
	struct mblock *mbp = (struct mblock *)((size_t)(ptr)-sizeof(struct mblock));
	struct mblock *curr = head;    

        if(ptr==NULL) return;

 	    	
                if(mbp->next==MAGIC){
			//Add a node before the head node
			if(mbp<head){
				mbp->next = head;
				head = mbp;
				merge();
				return;
			}
			//Add a node after the head node
			else if(mbp>head){
				//Find the position for the node
				while(!((mbp>curr)&&(mbp<curr->next)))	curr = curr->next;

				mbp->next = curr->next;
				curr->next = mbp;
				merge();
				return;
			}
                }
	
               else{
		     
                        abort();
                }
        

}

