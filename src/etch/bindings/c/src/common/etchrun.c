/**
 * etchrun.c -- runtime data management.
 * the runtime cache database is where we cache object vtables etc.
 */

#include <stdio.h>
#include "etchrun.h"
#include "etchmem.h"
#include "etchhash.h"

#define ETCH_CACHE_USES_ALPHA_KEY


/*
 * ETCH_CACHE_USES_ALPHA_KEY switches on alpha cache keys rather than the normal
 * 4-byte integer. experiencing a problem with either the hfirst or hnext 
 * hashtable functionality, this was an attempt to determine if the 4-byte keys
 * were causing the problem. it appears not, so we can probably use int keys again.
 */
#ifdef ETCH_CACHE_USES_ALPHA_KEY

char ckey[16];

/*
 * make_cache_key() 
 * constructs global alpha key for the cache returning key byte length
 */
int make_cache_key(const unsigned int ikey)
{
   char* pkey = ckey;
   /* memcpy(pkey+=4,"ca$h",4); */
   _itoa_s(ikey, pkey, 16, 10);
   return (int)strlen(ckey);
}


/*
 * cache_find() 
 * locate cached object with specified key, returning it or NULL
 */
void* cache_find(const unsigned int objtype)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;
    unsigned keylen = make_cache_key(objtype);

    int result = jenkins_find
       (global_cache->realtable, ckey, keylen, 0, &founditem);

    return result == -1? NULL: founditem->value;
}


/*
 * cache_del() 
 * Remove specified object from cache.
 * Return pointer to cached object, which is not freed here.
 */
void* cache_del (const unsigned int objtype)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;
    unsigned keylen = make_cache_key(objtype);

    int result = jenkins_remove
       (global_cache->realtable, ckey, keylen, 0, &founditem);

    if (result == -1) return NULL;
    free(founditem->key); /* free 4-byte key allocated in cache_add() */
    return founditem->value;
}


/*
 * cache_add() 
 * Add specified object to cache.
 * Return pointer to item's data entry, which is not freed here.
 */
int cache_add (const unsigned int objtype, void* data)
{
    int result = 0, keylen = 0;
    char* pkey = 0;
    void* pval = cache_find(objtype);
    if (pval) return 0; /* entry exists */

    keylen = make_cache_key(objtype);
    pkey = malloc(keylen); /* add new entry */
    memcpy(pkey, ckey, keylen);
    /* printf("cache_add key %d addr %08x\n", objtype, pkey); */

    result = jenkins_insert
       (global_cache->realtable, pkey, keylen, data, 0, 0, 0);
   
    /* cache_dump(); */
    return result;
} 

#else // #ifdef ETCH_CACHE_USES_ALPHA_KEY


/*
 * cache_find() 
 * locate cached object with specified key, returning it or NULL
 */
void* cache_find(const unsigned int objtype)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;
    int result = jenkins_find
       (global_cache->realtable, (char*)&objtype, sizeof(int), 0, &founditem);
    return result == -1? NULL: founditem->value;
}


/*
 * cache_del() 
 * Remove specified object from cache.
 * Return pointer to cached object, which is not freed here.
 */
void* cache_del (const unsigned int objtype)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;
    int result = jenkins_remove
       (global_cache->realtable, (char*)&objtype, sizeof(int), 0, &founditem);
    if (result == -1) return NULL;
    free(founditem->key); /* free 4-byte key allocated in cache_add() */
    return founditem->value;
}


*
 * cache_add() 
 * Add specified object to cache.
 * Return pointer to item's data entry, which is not freed here.
 */
int cache_add (const unsigned int objtype, void* data)
{
    int result = 0;
    char* pkey = 0;
    void* pval = cache_find(objtype);
    if (pval) return 0; /* entry exists */

    pkey = malloc(sizeof(int)); /* add new entry */
    memcpy(pkey, &objtype, sizeof(int));

    result = jenkins_insert
       (global_cache->realtable, pkey, sizeof(int), data, 0, 0, 0);
    return result;
} 


#endif // #ifdef ETCH_CACHE_USES_ALPHA_KEY 


/*
 * cache_create() 
 * create the global cache as an untracked hashtable
 */
etch_hashtable* cache_create() 
{
    global_cache = new_systemhashtable(INITIAL_CACHE_SIZE_ITEMS);
    return global_cache; 
}


/*
 * cache_clear() 
 * remove all objects from the runtime list, freeing both key memory and
 * data object memory for each. Returns the count of buckets so cleared.
 */
int cache_clear()
{
    int result = jenkins_clear(global_cache->realtable, TRUE, TRUE, 0, 0);
    return result;
}


/*
 * cache_destroy() 
 * clear the cache and destroy the cache object
 */
int cache_destroy()
{
    int result = cache_clear();
    result = jenkins_destroy(global_cache->realtable, 0, 0);
    return result;
}


/*
 * cache_count() 
 * return number of items in the runtime cache
 */
int cache_count()
{
    return jenkins_count(global_cache->realtable, 0, 0);
} 


#pragma warning(disable:4311)

/*
 * cache_dump() 
 * debug console display of pointers to each entry in the cache  
 */
int cache_dump()
{
    etch_hashitem  hashbucket;
    etch_hashitem* myentry = &hashbucket;
    const int count = jenkins_count(global_cache->realtable, 0, 0);
    printf("\n%d entries in cache\n",count);
    if (count == 0) return 0;

    if  (0 == jenkins_first(global_cache->realtable, NULL, &myentry))   
         printf("%08x: %08x\n", *(unsigned*)myentry->key, (unsigned)myentry->value);
    else printf("cache.first() failed\n");  
     
    while(0 == jenkins_next(global_cache->realtable, NULL, &myentry))          
          printf("%08x: %08x\n", *(unsigned*)myentry->key, (unsigned)myentry->value);
  
    return count;
}
 

/*
 * - - - - -  old list cache code from here on  - - - - -
 */

#ifdef ETCH_CACHE_USES_LINKEDLIST

objrec* cache_find(const unsigned int objtype)
{
    return objrec_find(objtype);
}

void* cache_get (const unsigned int objtype)
{
    return objrec_get(objtype);
} 

void* cache_del (const unsigned int objtype)
{
    return objrec_del(objtype); 
}

objrec* cache_add (const unsigned int objtype, void* data)
{
    return objrec_add(objtype, data);
} 
 
int cache_clear()
{
    return objtable_clear();
}

int cache_count()
{
    return objtable_count();
}  


/*
 * objrec_find 
 * look up an object in the runtable
 */
objrec* objrec_find(const unsigned int objtype)   
{
    objrec* p = runtable;

    for(; p; p = p->next)     
        if (p->obj_type == objtype) 
            break;    

    return p;
}


/*
 * objrec_get 
 * look up an object in the runtable, returning its data, or null.
 */
void* objrec_get(const unsigned int objtype)   
{
    objrec* p = objrec_find(objtype);
    return  p?  p->obj_impl: NULL;
}


#pragma warning (disable:4311) // todo debug stmts


/*
 * objrec_add 
 * if obj rec exists, return it, otherwise add a new obj rec and return it.
 */
objrec* objrec_add(const unsigned int objtype, void* data)   
{
    objrec* p = objrec_find(objtype);
    if (p) return p;

    /* we do not track allocations for the tracking hashtable      
     * since the tracking table does of course not yet exist. */
    p = is_memtable_instance?   
       malloc(sizeof(objrec)):
       malloc(sizeof(objrec));  /* we are currently not tracking the cache at all */ 
       /* malloc(sizeof(objrec), ETCHTYPEA_CACHEREC); */

    p->obj_type = objtype;  p->obj_impl = data;  p->next = NULL;

    if (runtable == NULL) /* first entry? */
        runtable = runtable_tail = p;
    else                  /* chain entry */
    {   assert(runtable_tail);    
        runtable_tail->next = p;
        runtable_tail = p; /* added to end - new entry is new tail */
    }

    /* printf("cache: add %08x\n",(unsigned)p); */

    return p; 
}


/*
 * objrec_del 
 * Remove specified object record from list and free its bucket memory.
 * Return pointer to item's data entry, which is not freed here.
 */
void* objrec_del(const unsigned int objtype)   
{
    void* data = NULL;
    objrec* p = runtable, *prior = NULL;

    for(; p; p = p->next)
    {
        if (p->obj_type != objtype)
        {   prior = p;   /* not found yet */
            continue;
        }

        if (prior)       /* found: chain around this entry */
            prior->next = p->next;
                         /* if last entry, adjust tail ptr */
        if (runtable_tail == p)
            runtable_tail = prior;
                        /* if first entry, adjust head ptr */
        if (runtable == p)
            runtable = prior;
                         /* save off data, free the bucket */
        data = p->obj_impl;
        free(p);
        /* printf("cache: del %08x\n",(unsigned)p); */
        break;       
    }     
        
  return data;
}


/*
 * objtable_count 
 * returns the current count of cache buckets.
 */
int objtable_count()   
{
    objrec *p = NULL;
    int count = 0;
    for(p = runtable; p; p = p->next, count++);        
    return count;
}


/*
 * objtable_clear 
 * remove all objects from the runtime list, freeing bucket memory and
 * data memory for each. Returns the count of buckets so cleared.
 */
int objtable_clear()   
{
    objrec *p = NULL, *nextp = NULL;
    int count = 0;

    for(p = runtable; p; p = nextp, count++)
    {   
        /* note that we must be careful to not clear the cache before we 
           are done accessing object vtables */
        free(p->obj_impl);    /* free data memory */
        nextp = p->next;
        /* printf("cache: del %08x\n",(unsigned)p); */
        free(p);              /* free bucket memory */
    }

  /* printf("cache: %d items cleared\n",count); */
  runtable = runtable_tail = NULL;           
  return count;
}

#endif // #ifdef ETCH_CACHE_USES_LINKEDLIST

