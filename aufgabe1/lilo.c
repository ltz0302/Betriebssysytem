#include <stdio.h>
#include <stdlib.h>

typedef struct list_element {
        int data;
        struct list_element *next;
} list_element_t;

typedef struct list {
        int size;
        list_element_t *head;
} list_t;



int list_append(list_t *list, int value) {
        list_element_t *element_Temp=list->head;
        list_element_t *new_element=NULL;

        new_element=(list_element_t *)malloc(sizeof(list_element_t));
        if(new_element==NULL) return -1;

        new_element->data=value;
        new_element->next=NULL;

        if(value<0) return -1;

        if(list->size==0){
                list->head = new_element;
                list->size++;
                return value;
         }
        else{
                if(element_Temp->data==value) return -1;
                while(element_Temp->next!=NULL){
                        if(element_Temp->next->data==value) return -1;
                        element_Temp = element_Temp->next;
                        }
                element_Temp->next = new_element;
                list->size++;
                return value;
        }
}

int list_pop(list_t *list) {
        int value;
        list_element_t *element_Temp=NULL;
	if(list->size==0) return -1;
        value=list->head->data;
        element_Temp = (list_element_t *)list->head->next;
        list->size--;
        free(list->head);
        list->head=element_Temp;
        return value;

}

int main (int argc, char* argv[]) {
        list_t list;
        list.size=0;
        list.head=NULL;


        printf("insert 47: %d\n", list_append(&list, 47));
        printf("insert 11: %d\n", list_append(&list, 11));
        printf("insert 23: %d\n", list_append(&list, 23));
        printf("insert 11: %d\n", list_append(&list, 11));

        printf("remove: %d\n", list_pop(&list));
        printf("remove: %d\n", list_pop(&list));
        exit(EXIT_SUCCESS);
}

