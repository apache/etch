/*
 * etch_arrarylist.h 
 * arraylist implementation.
 */

#ifndef ETCHARRAYLIST_H
#define ETCHARRAYLIST_H

#include "etch.h"
#include "etch_collection.h"

#define ETCHARRAYLIST_DEFSIZE 128
#define ETCHARRAYLIST_CONTENT_SIMPLE 0  /* content memory freed as a unit */
#define ETCHARRAYLIST_CONTENT_OBJECT 1  /* content is etchobject */

/** 
 *  etch_arraylist: data structure encapsulating the arraylist
 *  if is_readonly is set, content is read only and therefore is_free_content  
 *  will have no effect -- list items may still be inserted and removed. This
 *  guards against accidentally freeing memory during arraylist destruction 
 *  for arrays of references which are still intended to remain in use. 
 */
typedef struct etch_arraylist   
{
    void**     base;
    unsigned   delta;
    unsigned   size;
    unsigned   count;
    i_iterable iterable;

    byte is_readonly;  
    byte content_type;

} etch_arraylist;


etch_arraylist* new_arraylist(const unsigned initialsize, const unsigned deltasize);
int   arraylist_add      (etch_arraylist* list, void* content);
int   arraylist_containsp(etch_arraylist* list, void* content, const unsigned startat); /* pointer compare */
int   arraylist_contains (etch_arraylist* list, void* content, const unsigned startat, etch_comparator);
int   arraylist_indexofp (etch_arraylist* list, void* content, const unsigned startat); /* pointer compare */
int   arraylist_indexof  (etch_arraylist* list, void* content, const unsigned startat, etch_comparator);
int   arraylist_insert   (etch_arraylist* list, const unsigned i, void* content);
int   arraylist_remove   (etch_arraylist* list, const unsigned i, const int is_free_content);
int   arraylist_set      (etch_arraylist* list, const unsigned i, void* content);
void* arraylist_get      (etch_arraylist* list, const unsigned i);
void  arraylist_clear    (etch_arraylist* list, const int is_free_content);
void  arraylist_destroy  (etch_arraylist* list, const int is_free_content);

/* i_iterable function overrides */
int   arraylist_iterable_first   (etch_iterator*);
int   arraylist_iterable_next    (etch_iterator*);
int   arraylist_iterable_has_next(etch_iterator*);


#endif /* #ifndef ETCHARRAYLIST_H */