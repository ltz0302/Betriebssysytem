#include <stdlib.h>
#include "plist.h"


void walkList(list *list, int (*callback) (pid_t, const char *) ) {
	list_element *lauf = list->head;
	while(lauf != NULL){
		if(callback(lauf->pid, lauf->cmdLine) != 0){
			return;
		}
		lauf = lauf -> next;
	}
	return;
}

