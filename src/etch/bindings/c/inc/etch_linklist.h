/*
 * etch_linklist.h 
 * linked list implementation.
 */

#ifndef ETCHLINKLIST_H
#define ETCHLINKLIST_H

#include "etch.h"


/** 
 *  linklist_node: an entry in an etch_linklist
 */
struct linklist_node 
{
    char* content;
    struct linklist_node* next;      
};

typedef struct linklist_node linklist_node; 


/** 
 *  etch_linklist: data structure encapsulating the linkist
 */
typedef struct etch_linklist   
{
    linklist_node* head;
    linklist_node* tail;
    unsigned int count;

} etch_linklist;


etch_linklist* new_linklist();
linklist_node* linklist_get(etch_linklist* list, const unsigned i);
int  linklist_add      (etch_linklist* list, void* content);
int  linklist_insert   (etch_linklist* list, const unsigned i, void* content);
int  linklist_containsp(etch_linklist* list, void* content, const unsigned startat); /* pointer compare */
int  linklist_contains (etch_linklist* list, void* content, const unsigned startat, etch_comparator);
int  linklist_indexofp (etch_linklist* list, void* content, const unsigned startat); /* pointer compare */
int  linklist_indexof  (etch_linklist* list, void* content, const unsigned startat, etch_comparator);
int  linklist_remove   (etch_linklist* list, const unsigned i, const int is_free_content);
void linklist_clear    (etch_linklist* list, const int is_free_content);
void linklist_destroy  (etch_linklist* list, const int is_free_content);


#endif /* #ifndef ETCHLINKLIST_H */