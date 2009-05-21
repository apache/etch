/* $Id$ 
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more 
 * contributor license agreements. See the NOTICE file distributed with  
 * this work for additional information regarding copyright ownership. 
 * The ASF licenses this file to you under the Apache License, Version  
 * 2.0 (the "License"); you may not use this file except in compliance  
 * with the License. You may obtain a copy of the License at 
 * 
 * http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */ 

/**
 * etchrun.c -- runtime data management.
 * the runtime cache database is where we cache object vtables etc.
 */

#include <stdio.h>
#include "etch_global.h"

#define ETCH_CACHE_USES_ALPHA_KEY
#define CACHE_DEBUG FALSE

/*
 * ETCH_CACHE_USES_ALPHA_KEY switches on alpha cache keys rather than the normal
 * 4-byte integer. experiencing a problem with either the hfirst or hnext 
 * hashtable functionality, this was an attempt to determine if the 4-byte keys
 * were causing the problem. it appears not, so we can probably use int keys again.
 */
#ifdef ETCH_CACHE_USES_ALPHA_KEY

#define ETCHCACHEKEYBUFLEN 20
char ckey[ETCHCACHEKEYBUFLEN];


/*
 * make_cache_key() 
 * given 32-bit key constructs alpha key for the cache returning key byte length
 */
int make_cache_key(const unsigned int ikey)
{
   char* pkey = ckey;
   /* memcpy(pkey+=4,"ca$h",4); */
   _itoa_s(ikey, pkey, ETCHCACHEKEYBUFLEN, 10);
   return (int)strlen(ckey);
}


/*
 * cache_populate_out() 
 * populate caller's out struct
 */
void cache_populate_out(etch_hashitem* useritem, etch_hashitem* curritem)
{
    useritem->key   = curritem->key;
    useritem->value = curritem->value;
    useritem->hash  = curritem->hash;
}


/*
 * cache_findxl() 
 */
void* cache_findxl(char* key, unsigned keylen, void** out)
{
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

    int result = jenkins_find
       (global_cache->realtable, key, keylen, 0, &founditem);

    #if CACHE_DEBUG
    #pragma warning(disable:4313) 
    if  (result == 0) 
         printf("cache_found key %s len %d addr %08x\n", 
                 key, keylen, founditem->value);
    else printf("cache_notfound key %s len %d\n", key, keylen);
    #endif 
  
    if (result == -1) return NULL;

    if (out)
        cache_populate_out(*out, founditem);        

    return founditem->value;
}


/*
 * cache_find() 
 * locate cached object with specified key, returning it or NULL
 */
void* cache_find(const unsigned int objtype, void** out)
{
    unsigned keylen = make_cache_key(objtype);

    return cache_findxl(ckey, keylen, out);
}


/*
 * cache_findx() 
 * locate cached object with specified ansi char key, returning it or NULL
 */
void* cache_findx(char* key, void** out)
{
    unsigned keylen = (unsigned)strlen(key);

    return cache_findxl(key, keylen, out);
}


/*
 * cache_find_by_hash() 
 * locate cached object with specified hashkey, returning it or NULL
 */
void* cache_find_by_hash(const unsigned hash, void** out)
{
    int result;
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

    result = jenkins_findh(global_cache->realtable, hash, 0, &founditem);

    if (result == -1) return NULL;

    if (out)
        cache_populate_out(*out, founditem);       
    
    return founditem->value;
}


/*
 * cache_current() 
 * return cached object at current position
 */
etch_hashitem* cache_current()
{
    int result;
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

    result = jenkins_current(global_cache->realtable, 0, &founditem);

    return (result == -1)? NULL: founditem;
}


/*
 * cache_delxl() 
 * Remove specified object from cache given ansi char key and length.
 * Return pointer to cached object, which is not freed here.
 */
void* cache_delxl (char* key, const unsigned keylen)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;

    int result = jenkins_remove
       (global_cache->realtable, ckey, keylen, 0, &founditem);

    if (result == -1) return NULL;
    free(founditem->key); /* free 4-byte key allocated in cache_add() */
    return founditem->value;
}


/*
 * cache_del() 
 * Remove specified object from cache given integer key.
 * Return pointer to cached object, which is not freed here.
 */
void* cache_del (const unsigned int objtype)
{
    unsigned keylen = make_cache_key(objtype);

    return cache_delxl(ckey, keylen);
}


/*
 * cache_delx() 
 * Remove specified object from cache given ansi char key.
 * Return pointer to cached object, which is not freed here.
 */
void* cache_delx (char* key)
{
    unsigned keylen = (unsigned)strlen(key);

    return cache_delxl(key, keylen);
}


/*
 * cache_insertxl() 
 * Add specified object to cache given ansi char key and char length, 
 * with existence test optional.
 * Return hash of supplied key, or zero.
 */
int cache_insertxl (char* key, const unsigned keylen, void* data, const int is_check)
{
    int   result = 0, keylent = 0;
    char* pkey = NULL;
    void* pval = NULL;
    etch_hashitem  hashbucket; 
    etch_hashitem* inserteditem = &hashbucket;
    memset(&hashbucket, 0, sizeof(etch_hashitem));

    if (is_check)
    {   pval = cache_findxl(key, keylen, &inserteditem);
        if (pval) return inserteditem->hash; /* entry exists */
    }

    keylent = keylen + 1;/* add new entry */
    pkey    = malloc(keylent); 
    strcpy_s(pkey, keylent, key);

    #if CACHE_DEBUG
    #pragma warning(disable:4313) 
    printf("cache_insertxl key %s len %d addr %08x\n", pkey, keylen, data); 
    #endif

    result = jenkins_insert
       (global_cache->realtable, pkey, keylen, data, 0, 0, &inserteditem);

    /* cache_dump(); */

    return inserteditem->hash;
} 


/*
 * cache_insert() 
 * Add specified object to cache with existence test optional.
 * Return inserted item hash, or zero.
 */
int cache_insert (const unsigned int key, void* data, const int is_check)
{
    int   keylen = 0;
    void* pval = NULL;

    keylen = make_cache_key(key);

    return cache_insertxl(ckey, keylen, data, is_check);
} 


/*
 * cache_insertx() 
 * Add specified object to cache with existence test optional.
 * Return inserted item hash, or zero.
 */
int cache_insertx (char* key, void* data, const int is_check)
{
    unsigned keylen = (unsigned)strlen(key);
    
    return cache_insertxl(key, keylen, data, is_check);
} 


/*
 * cache_add() 
 * Add specified object to cache given integer key.
 * Return 0 or -1.
 */
int cache_add (const unsigned int objtype, void* data)
{
    return cache_insert (objtype, data, TRUE);
} 


/*
 * cache_addx() 
 * Add specified object to cache.
 * Return 0 or -1.
 */
int cache_addx(char* key, void* data)
{
    return cache_insertx(key, data, TRUE);
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

