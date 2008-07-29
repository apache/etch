/**
 * etch_arraylist.c -- implementation of arraylist.
 */

#include <stdio.h>
#include "etch_arraylist.h"
#include "etch_global.h"
#include "etch_objects.h"


/**
 * new_arraylist()
 * constructor for an etch_arraylist, which implements i_iterable.
 * Creates the underlying list and returns a pointer to the list.
 * Passed initial size, and initial expansion delta, both expressed as number of entries.
 */
etch_arraylist* new_arraylist(const unsigned initsize, const unsigned deltsize)
{
    etch_arraylist* list = etch_malloc(sizeof(etch_arraylist), ETCHTYPEB_ARRAYLIST);
    memset(list, 0, sizeof(etch_arraylist));
    list->size  = initsize > 0? initsize: ETCHARRAYLIST_DEFSIZE;
    list->delta = deltsize > 0? deltsize: list->size;
    list->size *= sizeof(void**); list->delta *= sizeof(void**);
    list->base  = etch_malloc(list->size, ETCHTYPEB_BYTES);
    memset(list->base, 0, list->size);

    new_iterable(&list->iterable, NULL, arraylist_iterable_first, 
        arraylist_iterable_next, arraylist_iterable_has_next); 

    return list;
}


/**
 * arraylist_destroy()
 * Destructor for an etch_arraylist.
 * Destroys the underlying list, the list shell, and the list content if requested.
 */
void arraylist_destroy(etch_arraylist* list, const int is_free_content) 
{    
    if (NULL == list) return;
    arraylist_clear(list, is_free_content);
    etch_free(list->base); /* free buffer memory */
    etch_free(list);       /* free list object */
}


/**
 * arraylist_realloc()
 * private method to resize an arraylist.
 * if a size greater than current size is specified, the list is realloced to that size,
 * otherwise it is realloced to current size plus the delta size specified at construction.
 */
int arraylist_realloc(etch_arraylist* list, const unsigned size)    
{
  unsigned int copylength = 0, newsize = 0;
  void* newbase = NULL;

  newsize = size > list->size? size: list->size + list->delta;
  newbase = etch_malloc(newsize, ETCHTYPEB_BYTES);
  memset(newbase, 0, newsize);

  copylength = list->count * sizeof(void**);
  memcpy(newbase, list->base, copylength); 

  etch_free(list->base);
  list->base = newbase;
  list->size = newsize;
  return 0;
}


/**
 * arraylist_checksize()
 * private method ensures list has space required for pending content insertion.
 */
int arraylist_checksize(etch_arraylist* list, const int pendingitems)
{
    int result = 1;
    unsigned currsize = 0, newspace = pendingitems * sizeof(void**);
    currsize = list->count * sizeof(void**);

    if  (currsize + newspace > list->size)
         arraylist_realloc(list, newspace > list->delta? newspace: 0);
    else result = 0;

    return result;
}


/*
 * arraylist_clear() 
 * remove all content from the arraylist, freeing content memory only if requested. 
 * note that the list buffer does not shrink, rather retaining its current size.
 */
void arraylist_clear(etch_arraylist* list, const int is_free_content) 
{
    void** p = 0;
    int items = 0, is_obj_content = 0, i = 0;
    if (NULL == list || NULL == list->base) return;
    is_obj_content = list->content_type == ETCHARRAYLIST_CONTENT_OBJECT;
    items = list->count;
    p = list->base;

    if (is_free_content && !list->is_readonly)
        for(; i < (const int) items; i++, p++)
        {
            /* dicey spot, this. we've marked the array as having etchobject
             * content, so we're invoking the alleged object's destructor, for 
             * example if the object is an etch_string, the dtor would free the  
             * object's wchar_t*, and then free the object shell. so if *p does 
             * not point at an etchobject, we would be royally hosed here.
             */
            if  (is_obj_content)
                ((etchobject*)*p)->vtab->destroy(*p);            
            else etch_free(*p);
        }

    memset(list->base, 0, items * sizeof(void**)); 
    list->count = 0;        
}


/*
 * arraylist_add() 
 * add an entry to the end of the list.
 */
int arraylist_add(etch_arraylist* list, void* content) 
{
    if (!list || !list->base) return -1;  
    arraylist_checksize(list, 1);
    list->base[list->count++] = content;
    return 0;
}


/*
 * arraylist_insert() 
 * add a node anywhere in the list, returning 0 if OK, or -1 if error.
 * this implementation cannot insert past the current end of list,
 * in other words to insert at index n, there must be at least n
 * entries currently in the list (list->count >= n), that is to say,
 * the insertion must expand the size of the list by exactly one slot.
 */
int arraylist_insert(etch_arraylist* list, const unsigned int i, void* content)
{
    unsigned shiftcount = 0, j = 0;
    void **p = NULL, **q = NULL;
    if ((NULL == list) || (i > list->count) || (i < 0)) return -1;
    arraylist_checksize(list, 1);

    shiftcount = list->count - i;  /* shift higher entries up */
    p = &list->base[list->count - 1];  
    q = &list->base[list->count];

    for(j = 0; j < (const unsigned) shiftcount; j++, p--, q--)
        *q = *p;
    
    list->base[i] = content;
    ++list->count;
    return 0;
}


/*
 * arraylist_containsp() 
 * return 1 or 0 indicating if the list contains the supplied content pointer.
 */
int arraylist_containsp(etch_arraylist* list, void* content, const unsigned startat)
{
    return arraylist_indexofp(list, content, startat) == -1? FALSE: TRUE;
}


/*
 * arraylist_indexofp() 
 * if the list contains the supplied content pointer, return its index;
 * return -1 if not found or if a parameter was in error.
 */
int arraylist_indexofp(etch_arraylist* list, void* content, const unsigned startat)
{
    void** p = NULL;
    unsigned int i = startat;
    if ((NULL == list) || (i >= list->count) || (i < 0)) return -1;
    p = &list->base[i];

    for(; i < list->count; i++, p++)         
        if (*p == content) 
            return i;
                 
    return -1;
}


/*
 * arraylist_contains() 
 * return 1 or 0 indicating if the list contains the supplied content. 
 * caller must supply a comparator function int (*f)(void* this, void* that);  
 * which should return -2 bad params, -1 compares less, 0 equal, 1 greater. 
 */
int arraylist_contains(etch_arraylist* list, void* content, const unsigned startat, etch_comparator compare)
{
    return arraylist_indexof(list, content, startat, compare) == -1? FALSE: TRUE;
}


/*
 * arraylist_indexof() 
 * if the list contains the supplied content, return its index.
 * return -1 if a parameter was in error.
 * caller must supply a comparator function int (*f)(void* this, void* that); 
 * which should return -2 bad params, -1 compares less, 0 equal, 1 greater. 
 */
int arraylist_indexof(etch_arraylist* list, void* content, const unsigned startat, etch_comparator compare)
{
    void** p = NULL;
    unsigned i = startat;
    if ((NULL == list) || (i >= list->count) || (i < 0)) return -1;
    p = &list->base[i];

    for(; i < list->count; i++, p++)  
        if (0 == compare(content, *p)) 
            return (int)i;
     
    return -1;
}


/*
 * arraylist_set() 
 * replace content at specified index position.
 */
int arraylist_set(etch_arraylist* list, const unsigned i, void* content)
{
    if ((NULL == list) || (i >= list->count) || (i < 0)) return -1;
    list->base[i] = content;
    return 0;
}


/*
 * arraylist_get() 
 * return content at specified index position, or NULL if parameter error.
 */
void* arraylist_get(etch_arraylist* list, const unsigned i)
{
    if ((NULL == list) || (i >= list->count) || (i < 0)) return NULL;
    return list->base[i];
}


/*
 * arraylist_remove() 
 * remove entry at specified index position, freeing content memory if requested.
 * return -1 if a parameter was in error, or zero if OK.
 */
int arraylist_remove(etch_arraylist* list, const unsigned i, const int is_free_content)
{
    unsigned bytecount = 0, j = 0;
    void *content = NULL, **p = NULL, **q = NULL;
    if ((NULL == list) || (i >= list->count) || (i < 0)) return -1;
    
    if (is_free_content && !list->is_readonly)
        if (content = list->base[i])
            etch_free(content);
     
    p = &list->base[i];  /* shift higher entries down */
    q = &list->base[i + 1];

    for(j = i; j < (const unsigned)(list->count - i); j++, p++, q++)
        *p = *q;

    list->base[--list->count] = NULL;  /* zero the now-extra slot */
    return 0;
}


/*
 * arraylist_iterable_first() 
 * i_iterable first() implementation
 */
int arraylist_iterable_first(etch_iterator* iter)
{
    etch_arraylist* list = NULL;
    if (!iter || !iter->collection)  return -1;
    list = iter->collection;
    if (!list->base || !list->count) return -1;

    iter->current_value = arraylist_get(iter->collection, 0);   
    iter->ordinal = iter->current_value? 1: 0;
    return iter->ordinal? 0: -1;
}


/*
 * arraylist_iterable_next() 
 * i_iterable next() implementation
 * functions as first() if there is no current position.
 */
int arraylist_iterable_next(etch_iterator* iter)
{
    etch_arraylist* list = NULL;
    if (!iter || !iter->collection)  return -1;
    list = iter->collection;
    if (!list->base || !list->count) return -1;
    
    iter->current_value = arraylist_get(iter->collection, iter->ordinal); 
    iter->ordinal = iter->current_value? ++iter->ordinal: 0;
    return iter->ordinal? 0: -1;
}


/*
 * arraylist_iterable_has_next() 
 * i_iterable has_next() implementation.
 */
int arraylist_iterable_has_next(etch_iterator* iter)
{
    return iter && iter->collection && iter->ordinal;
}

