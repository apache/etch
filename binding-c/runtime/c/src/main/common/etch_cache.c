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
 * etch_cache.h -- etch cache methods.
 */

#include "etch_cache.h"
#include "etch_hash.h"

#define ETCH_CACHE_USES_ALPHA_KEY
#define ETCH_CACHE_DEBUG FALSE
#define ETCH_CACHE_INITIAL_SIZE 32
#define ETCH_CACHE_KEYBUFLEN 20

etch_cache_t* g_etch_cache = NULL;
etch_mutex* g_etch_cache_mutex = NULL;

/*
 * ETCH_CACHE_USES_ALPHA_KEY switches on alpha cache keys rather than the normal
 * 4-byte integer. experiencing a problem with either the hfirst or hnext 
 * hashtable functionality, this was an attempt to determine if the 4-byte keys
 * were causing the problem. it appears not, so we can probably use int keys again.
 */
#ifdef ETCH_CACHE_USES_ALPHA_KEY

char ckey[ETCH_CACHE_KEYBUFLEN];

/*
 * make_cache_key() 
 * given 32-bit key constructs alpha key for the cache returning key byte length
 */
static int etch_make_cache_key(const unsigned int ikey)
{
   char* pkey = ckey;
   sprintf(pkey,"%d", ikey);
   return (int)strlen(ckey);
}

/*
 * etch_cache_populate_out() 
 * populate caller's out struct
 */
static void etch_cache_populate_out(etch_hashitem* useritem, etch_hashitem* curritem)
{
    useritem->key   = curritem->key;
    useritem->value = curritem->value;
    useritem->hash  = curritem->hash;
}

/*
 * cache_findxl() 
 */
void* etch_cache_findxl(char* key, unsigned keylen, void** out)
{
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

    int result = jenkins_find
      (g_etch_cache->realtable, key, keylen, 0, (void**)&founditem);

    #if ETCH_CACHE_DEBUG
    #pragma warning(disable:4313) 
    if  (result == 0) 
         printf("cache_found key %s len %d addr %08x\n", 
                 key, keylen, founditem->value);
    else printf("cache_notfound key %s len %d\n", key, keylen);
    #endif 
  
    if (result == -1) return NULL;

    if (out)
        etch_cache_populate_out(*out, founditem);

    return founditem->value;
}


/*
 * cache_find() 
 * locate cached object with specified key, returning it or NULL
 */
void* etch_cache_find(const unsigned int objtype, void** out)
{
    unsigned keylen = 0;
    void* res = NULL;
    etch_mutex_lock(g_etch_cache_mutex);
    keylen = etch_make_cache_key(objtype);
    res = etch_cache_findxl(ckey, keylen, out);
    etch_mutex_unlock(g_etch_cache_mutex);
    return res;
}


/*
 * cache_findx() 
 * locate cached object with specified ansi char key, returning it or NULL
 */
void* etch_cache_findx(char* key, void** out)
{
    unsigned keylen = (unsigned)strlen(key);

    return etch_cache_findxl(key, keylen, out);
}


/*
 * cache_find_by_hash() 
 * locate cached object with specified hashkey, returning it or NULL
 */
void* etch_cache_find_by_hash(const unsigned hash, void** out)
{
    int result;
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

    result = jenkins_findh(g_etch_cache->realtable, hash, 0, (void**)&founditem);

    if (result == -1) return NULL;

    if (out)
        etch_cache_populate_out(*out, founditem);       
    
    return founditem->value;
}


/*
 * cache_current() 
 * return cached object at current position
 */
etch_hashitem* etch_cache_current()
{
    int result;
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

    result = jenkins_current(g_etch_cache->realtable, 0, (void**)&founditem);

    return (result == -1)? NULL: founditem;
}


/*
 * cache_delxl() 
 * Remove specified object from cache given ansi char key and length.
 * Return pointer to cached object, which is not freed here.
 */
void* etch_cache_delxl(char* key, const unsigned keylen)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;

    int result = jenkins_remove
      (g_etch_cache->realtable, ckey, keylen, 0, (void**)&founditem);

    if (result == -1) return NULL;
    free(founditem->key); /* free 4-byte key allocated in cache_add() */
    return founditem->value;
}


/*
 * cache_del() 
 * Remove specified object from cache given integer key.
 * Return pointer to cached object, which is not freed here.
 */
void* etch_cache_del(const unsigned int objtype)
{
    unsigned keylen = 0;
    void* res = NULL;
    
    etch_mutex_lock(g_etch_cache_mutex);
    keylen = etch_make_cache_key(objtype);
    res = etch_cache_delxl(ckey, keylen);
    etch_mutex_unlock(g_etch_cache_mutex);
    return res;
}


/*
 * cache_delx() 
 * Remove specified object from cache given ansi char key.
 * Return pointer to cached object, which is not freed here.
 */
void* etch_cache_delx (char* key)
{
    unsigned keylen = (unsigned)strlen(key);

    return etch_cache_delxl(key, keylen);
}


/*
 * cache_insertxl() 
 * Add specified object to cache given ansi char key and char length, 
 * with existence test optional.
 * Return hash of supplied key, or zero.
 */
int etch_cache_insertxl (char* key, const unsigned keylen, void* data, const int is_check)
{
    int   result = 0, keylent = 0;
    char* pkey = NULL;
    void* pval = NULL;
    etch_hashitem  hashbucket; 
    etch_hashitem* inserteditem = &hashbucket;
    memset(&hashbucket, 0, sizeof(etch_hashitem));

    if (is_check)
      {   pval = etch_cache_findxl(key, keylen, (void**)&inserteditem);
        if (pval) return inserteditem->hash; /* entry exists */
    }

    keylent = keylen + 1;/* add new entry */
    pkey    = etch_malloc(keylent, 0); 
	strcpy(pkey, key);

    #if CACHE_DEBUG
    #pragma warning(disable:4313) 
    printf("cache_insertxl key %s len %d addr %08x\n", pkey, keylen, data); 
    #endif

    result = jenkins_insert
      (g_etch_cache->realtable, pkey, keylen, data, 0, 0, (void**)&inserteditem);

    /* cache_dump(); */

    return inserteditem->hash;
} 


/*
 * cache_insert() 
 * Add specified object to cache with existence test optional.
 * Return inserted item hash, or zero.
 */
int etch_cache_insert (const unsigned int key, void* data, const int is_check)
{
    int keylen = 0;
    int res = 0;
    etch_mutex_lock(g_etch_cache_mutex);
    keylen = etch_make_cache_key(key);
    res = etch_cache_insertxl(ckey, keylen, data, is_check);
    etch_mutex_unlock(g_etch_cache_mutex);
    return res;
} 


/*
 * cache_insertx() 
 * Add specified object to cache with existence test optional.
 * Return inserted item hash, or zero.
 */
int etch_cache_insertx (char* key, void* data, const int is_check)
{
    unsigned keylen = (unsigned)strlen(key);
    
    return etch_cache_insertxl(key, keylen, data, is_check);
} 


/*
 * cache_add() 
 * Add specified object to cache given integer key.
 * Return 0 or -1.
 */
int etch_cache_add(const unsigned int objtype, void* data)
{
    return etch_cache_insert (objtype, data, TRUE);
} 


/*
 * cache_addx() 
 * Add specified object to cache.
 * Return 0 or -1.
 */
int etch_cache_addx(char* key, void* data)
{
    return etch_cache_insertx(key, data, TRUE);
}


#else // #ifdef ETCH_CACHE_USES_ALPHA_KEY


/*
 * cache_find() 
 * locate cached object with specified key, returning it or NULL
 */
void* etch_cache_find(const unsigned int objtype)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;
    int result = jenkins_find
       (g_etch_cache->realtable, (char*)&objtype, sizeof(int), 0, &founditem);
    return result == -1? NULL: founditem->value;
}


/*
 * cache_del() 
 * Remove specified object from cache.
 * Return pointer to cached object, which is not freed here.
 */
void* etch_cache_del (const unsigned int objtype)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;
    int result = jenkins_remove
       (g_etch_cache->realtable, (char*)&objtype, sizeof(int), 0, &founditem);
    if (result == -1) return NULL;
    free(founditem->key); /* free 4-byte key allocated in cache_add() */
    return founditem->value;
}


*
 * cache_add() 
 * Add specified object to cache.
 * Return pointer to item's data entry, which is not freed here.
 */
int etch_cache_add (const unsigned int objtype, void* data)
{
    int result = 0;
    char* pkey = 0;
    void* pval = cache_find(objtype);
    if (pval) return 0; /* entry exists */

    pkey = etch_malloc(sizeof(int), 0); /* add new entry */
    memcpy(pkey, &objtype, sizeof(int));

    result = jenkins_insert
       (g_etch_cache->realtable, pkey, sizeof(int), data, 0, 0, 0);
    return result;
} 


#endif // #ifdef ETCH_CACHE_USES_ALPHA_KEY 


int etch_cache_freehook(void* key, void* value)
{
    etch_free(key);
    etch_object_destroy((vtabmask*)value);

    return 1;
}

/*
 * cache_create() 
 * create the global cache as an untracked hashtable
 */
etch_status_t etch_cache_create()
{
    etch_status_t etch_status = ETCH_SUCCESS;
    
    assert(g_etch_cache == NULL);
    
    etch_status = etch_mutex_create(&g_etch_cache_mutex, ETCH_MUTEX_NESTED, NULL);
    if(etch_status != ETCH_SUCCESS) {
        // error
        return etch_status;
    }
    g_etch_cache = new_systemhashtable(ETCH_CACHE_INITIAL_SIZE);
    g_etch_cache->freehook = etch_cache_freehook;

    return etch_status;
}


etch_status_t etch_cache_set_freehook(etch_cache_freehook_func freehook)
{
    etch_status_t rv = ETCH_SUCCESS;
    g_etch_cache->freehook = freehook;
    return rv;
}


/*
 * cache_clear() 
 * remove all objects from the runtime list, freeing both key memory and
 * data object memory for each. Returns the count of buckets so cleared.
 */
int etch_cache_clear()
{
    int result = jenkins_clear(g_etch_cache->realtable, TRUE, TRUE, g_etch_cache, 0);
    return result;
}

/*
 * cache_destroy() 
 * clear the cache and destroy the cache object
 */
int etch_cache_destroy()
{
    int result = etch_cache_clear();
    result = jenkins_destroy(g_etch_cache->realtable, 0, 0);
    etch_free(g_etch_cache);
    g_etch_cache = NULL;
    etch_object_destroy(g_etch_cache_mutex);
    g_etch_cache_mutex = NULL;
    return result;
}

/*
 * cache_count() 
 * return number of items in the runtime cache
 */
int etch_cache_count()
{
    return jenkins_count(g_etch_cache->realtable, 0, 0);
} 

