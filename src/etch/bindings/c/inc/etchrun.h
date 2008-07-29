/**
 * etchrun.h -- runtime data management 
 */

#ifndef ETCHRUN_H
#define ETCHRUN_H

#include "etch.h"
#include "etchhash.h"


/*
 * objrec is a node in a runtime cache of instantiated objects, specifically
 * object vtables. The idea is, when we create an object, such as an etch_
 * hashtable, for example, we check this cache to see if that object has
 * been implemented previously; if not, we allocate the abstract part of
 * the object (the vtable), implement it with pointers to concrete methods,  
 * allocate a bucket for it, and add it to this cache. Thus each etch object
 * carries around its own instance data of course, but points to its vtable.
 */
struct objrec /* DEPRECATED */
{
   unsigned obj_type;    /* four 8-bit fields -- see objtypes.h */
   void*    obj_impl;    /* "value" of the object */
   struct objrec* next;  /* link pointer */
}; 
typedef struct objrec objrec; /* DEPRECATED */


/*
 * runtable is the runtime list of instantiated objects, specifically
 * object vtables. See comments above. 
 */
objrec* runtable, *runtable_tail;  /* DEPRECATED */ 


/*
 * runtime cache
 */
etch_hashtable* global_cache;


/*
 * method signatures
 */
objrec* objrec_find(const unsigned int typ); /* DEPRECATED */
void*   objrec_get (const unsigned int typ); /* DEPRECATED */
void*   objrec_del (const unsigned int typ); /* DEPRECATED */
objrec* objrec_add (const unsigned int typ, void* data);  /* DEPRECATED */
int     objtable_clear(); /* DEPRECATED */
int     objtable_count(); /* DEPRECATED */

/* inline abstraction wrappers for above */
/* if we code here we could e.g. swap out list for a hashtable */
etch_hashtable* cache_create();
void* cache_find(const unsigned int typ);
void* cache_del (const unsigned int typ); 
int   cache_add (const unsigned int typ, void* data);  
int   cache_destroy();
int   cache_clear();   
int   cache_count();  
int   cache_dump();  


#endif /* #ifndef ETCHRUN_H*/ 