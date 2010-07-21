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

/* 
   etch_hash.c -- implementation of an underlying hashtable.
   provided herein are implementations for the jenkins hashtable APIs.
*/

#include "etch_hash.h"
#include "etch_cache.h"
#include "etch_mem.h"
#include "etch_mutex.h"
#include "etch_objecttypes.h"
#include "etch_arraylist.h"
#include "jenkhtab.h"
#include "jenklook.h"

#define ETCH_HASH_MAX_KEYLENGTH 1024 /* arbitrary max byte length of a hashtable key */

// extern types
extern apr_pool_t* g_etch_main_pool;

/**
 * implementation of etch_hashtable.insert() for the jenkins hashtable. 
 * key and data are pointers to memory owned by the caller. The hashtable does 
 * not make copies of this data. The caller is responsible for freeing said 
 * memory, however note that etch_hashtable.clear() frees this memory.
 * key cannot be null but data can be null.  
 * datalen parameter is ignored for this implementation.
 * if out is non_null, *out is assumed to point at a etch_hashitem struct,
 * and this struct is populated with pointers to the inserted item.
 * result is zero if OK, otherwise -1;
 */
int jenkins_insert(void* realtable, void* key, const int keylen, 
    void* data, const int datalen, void* in, void** out) 
{
    etch_status_t status = ETCH_SUCCESS;
    int  result = 0;

    etch_hashtable* ht = (etch_hashtable*)in; 

    if (!realtable || !key || !keylen)
        return -1;

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_lock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    if (FALSE == hadd((htab*)realtable, key, keylen, data)) /* jenkhash.lib */
        result = -1;    /* key already exists most likely */
    else
    if (out)
     {  etch_hashitem* outentry = (etch_hashitem*) *out; 
        outentry->key   = hkey  ((htab*)realtable);
        outentry->value = hstuff((htab*)realtable);
        outentry->hash  = ((htab*)realtable)->ipos->hval;         
     }

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return result;
}


/**
 * implementation of etch_hashtable.inserth() for the jenkins hashtable. 
 * key and data are pointers to memory owned by the caller. the hashtable does 
 * not make copies of this data. the caller is responsible for freeing said 
 * memory, however note that etch_hashtable.clear() frees this memory.
 * key cannot be null but data can be null. 
 * key object is expected to contain its hashkey value in its first 4 bytes. 
 * jenkins will use this value, rather than compute a hash value.
 * if out is non_null, *out is assumed to point at a etch_hashitem struct,
 * and this struct is populated with pointers to the inserted item.
 * result is zero if OK, otherwise -1;
 */
int jenkins_inserth(void* realtable, void* key, void* data, void* in, void** out) 
{
    etch_status_t status = ETCH_SUCCESS;
    int  result = 0;
    etch_hashtable* ht = (etch_hashtable*) in; 
    
    if (!realtable || !key)
        return -1;

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_lock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    if (FALSE == haddx((htab*)realtable, key, data)) /* jenkhash.lib */
        result = -1;    /* key already exists most likely */
    else
    if (out)
    {
        etch_hashitem* outentry = (etch_hashitem*) *out; 
        outentry->key   = hkey  ((htab*)realtable);
        outentry->value = hstuff((htab*)realtable);
        outentry->hash  = ((htab*)realtable)->ipos->hval;         
     }

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return result;
}


/**
 * implementation of etch_hashtable.find() for the jenkins hashtable. 
 * moves the current position on the underlying table to that of supplied key.
 * if out is non_null, *out is assumed to point at a etch_hashitem struct,
 * and this struct is populated with pointers to the located item's key and value.
 * result is zero if OK, otherwise -1;
 */
int jenkins_find(void* realtable, void* key, const int keylen, 
    void* in, void** out) 
{
    etch_status_t status = ETCH_SUCCESS;
    int  result = 0;
    etch_hashtable* ht = (etch_hashtable*) in; 

    if (!realtable || !key || !keylen)
        return -1;

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_lock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    if (FALSE == hfind((htab*)realtable, (unsigned char*)key, keylen))  
        result = -1;
    else
    if (out)
    {   etch_hashitem* outentry = (etch_hashitem*) *out; 
        outentry->key   = hkey  ((htab*)realtable);
        outentry->value = hstuff((htab*)realtable);
        outentry->hash  = ((htab*)realtable)->ipos->hval; 
    }

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return result;  
}


/**
 * implementation of etch_hashtable.findh() for the jenkins hashtable. 
 * Implements a find by hashed key, otherwise see comments for find().
 * result is zero if OK, otherwise -1;
 */
int jenkins_findh(void* realtable, const unsigned int hashed, 
    void* in, void** out) 
{
    etch_status_t status = ETCH_SUCCESS;
    int result = 0;
    etch_hashtable* ht = (etch_hashtable*) in; 

    if (!realtable || !hashed)
        return -1;

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_lock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    if (FALSE == hfindx((htab*)realtable, hashed)) 
        result = -1;
    else     
    if (out)
    {   char* tkey = hkey  ((htab*)realtable);
        void* data = hstuff((htab*)realtable);
        etch_hashitem* outentry = (etch_hashitem*) *out; 
        outentry->key   = tkey;
        outentry->value = data;
    }

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return result;  
}


/**
 * implementation of etch_hashtable.first() for the jenkins hashtable. 
 * moves the current position on the underlying table to that of the first item.
 * If out is non_null, *out is assumed to point at a etch_hashitem struct,
 * and this struct is populated with pointers to the located item's key and value.
 * in parameter is ignored for this implementation.
 * result is zero if OK, otherwise -1, indicating bad params or an empty table.
 * @note this method is NOT SYNCHRONIZED, since in most or all cases it is invoked 
 * only during iteration of the map, and in that case the map is locked explicitly 
 * by the caller (hashtable_setlock()), and the mutex may not support nesting. 
 * if there is a need to synch it otherwise, wrap the call as follows:
 *    map->synchook(ETCH_SYNC_SET_, ((etch_object*)map)->synclock);
 *    map->first(...);
 *    map->synchook(ETCH_SYNC_REL_, ((etch_object*)map)->synclock);
 */
int jenkins_first(void* realtable, void* in, void** out) 
{
    int result = 0;

    if (FALSE == hfirst((htab*)realtable))  
        result = -1; /* table is empty most likely */
    else            
    if (out)
    {   char* tkey = hkey  ((htab*)realtable);
        void* data = hstuff((htab*)realtable);
        etch_hashitem* outentry = (etch_hashitem*) *out; 
        outentry->key   = tkey;
        outentry->value = data;
    }

    return result;  
}


/**
 * implementation of etch_hashtable.next() for the jenkins hashtable. 
 * Moves the current position on the underlying table to that of the next item 
 * in the table. If out is non_null, *out is assumed to point at a etch_hashitem,
 * and this struct is populated with pointers to the located item's key and value.
 * in parameter is ignored for this implementation.
 * result is zero if OK, otherwise -1, indicating either bad params, or that 
 * there are no more entries, in which case the current position will have wrapped
 * to the first item, if any entries in fact still remain.
 * @note this method is NOT SYNCHRONIZED, since in most or all cases it is invoked 
 * only during iteration of the map, and in that case the map is locked explicitly 
 * by the caller (hashtable_setlock()), and the mutex may not support nesting. 
 * if there is a need to synch it otherwise, wrap the call as follows:
 *    map->synchook(ETCH_SYNC_SET_, ((etch_object*)map)->synclock);
 *    map->next(...);
 *    map->synchook(ETCH_SYNC_REL_, ((etch_object*)map)->synclock);
 */
int jenkins_next(void* realtable, void* in, void** out) 
{
    int is_next_found = 0, is_table_empty = 0;
    char* tkey = NULL; 
    void* data = NULL;
    if (!realtable) return -1;

    /* hnext returns 1 if there is a next entry, or 0 if there is no next entry
     * and the position has wrapped to the beginning of the table. However if  
     * the table is now empty, there is no current position, and so we test for
     * that condition before attempting to reference the current position.
     */
    is_next_found  = hnext((htab*)realtable);  /* jenkhash.h */
    is_table_empty = NULL == ((htab*)realtable)->ipos;

    if (out)  /* return data at current position if requested */
    {   etch_hashitem* outentry = (etch_hashitem*) *out; 

        if (!is_table_empty)
        {   tkey = hkey  ((htab*)realtable);
            data = hstuff((htab*)realtable);
        }
        outentry->key   = tkey;
        outentry->value = data;
    }

    return is_next_found? 0: -1; 
}


/**
 * implementation of etch_hashtable.current() for the jenkins hashtable. 
 * retrieves data for the entry at the current hashtable position.
 * *out is assumed to point at a etch_hashitem struct; this struct is populated 
 * with pointers to the current item's key and value.
 * in parameter is ignored for this implementation.
 * result is zero if OK, otherwise -1, indicating bad params or an empty table.
 * @note this method is NOT SYNCHRONIZED, since it is not meaningful for a shared
 * hashtable (current position will change with every operation).  
 * if there is a need to synch it, wrap the call as follows:
 *    map->synchook(ETCH_SYNC_SET_, ((etch_object*)map)->synclock);
 *    map->current(...);
 *    map->synchook(ETCH_SYNC_REL_, ((etch_object*)map)->synclock);
 */
int jenkins_current(void* realtable, void* in, void** out) 
{
    unsigned hash = 0;
    char* tkey = NULL; 
    void* data = NULL;
    etch_hashitem* outentry = NULL; 
    if (!realtable || !out || !((htab*)realtable)->count) return -1;

    tkey = hkey  ((htab*)realtable);    
    data = hstuff((htab*)realtable);
    hash =((htab*)realtable)->ipos->hval;

    outentry = (etch_hashitem*) *out; 
    outentry->key   = tkey;
    outentry->value = data;
    outentry->hash  = hash; 

    return tkey? 0: -1;
}

/**
 * Implementation of etch_hashtable.remove() for the jenkins hashtable. 
 * Frees the entry for the key supplied , but neither the key nor the value, 
 * are freed, recall that neither of these is a copy but are instead pointers.
 * To actually free memory for these items, pass etch_hashitem** &out),
 * and you can then free(out->key), and free(out->value), at your leisure. 
 * Result is zero if OK, otherwise -1;
 */
int jenkins_remove(void* realtable, void* key, const int keylen, 
    void* in, void** out) 
{
    etch_status_t status = ETCH_SUCCESS;

    int result = 0;
    etch_hashtable* ht = (etch_hashtable*) in; 

    if (!realtable || !key || !keylen)
        return -1;

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_lock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    if (FALSE == hfind((htab*)realtable, key, keylen)) /* locate entry */
        result = -1;                    /* key nonexistent most likely */
    else
    {   if (out)           /* save off the entry contents if requested */
        {   etch_hashitem* outentry = (etch_hashitem*) *out; 
            outentry->key   = hkey  ((htab*)realtable);
            outentry->value = hstuff((htab*)realtable);
            outentry->hash  = ((htab*)realtable)->ipos->hval;   
        }

        hdel((htab*)realtable);    /* delete entry at current position */
    } 

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return result;  
}

/**
 * jenkins_removeh()
 */
int jenkins_removeh(void* realtable, const unsigned key, void* in, void** out) 
{
    etch_status_t status = ETCH_SUCCESS;
    int result = 0;
    etch_hashtable* ht = (etch_hashtable*) in; 
    
    if (!realtable || !key)
        return -1;

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_lock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }
    
    if (FALSE == hfindx((htab*)realtable, key)) /* locate entry */
        result = -1;
    else  
    {   if (out)
        {   etch_hashitem* outentry = (etch_hashitem*) *out; 
            outentry->key   = hkey  ((htab*)realtable);
            outentry->value = hstuff((htab*)realtable);
        }

        hdel((htab*)realtable);    /* delete entry at current position */
    }

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return result;  
}

/**
 * implementation of etch_hashtable.clear() for the jenkins hashtable. 
 * empties the table and, if requested, frees memory for keys/and/or values. 
 * out parameter is ignored for this implementation.
 * Result is count of table entries freed, or -1 if error.
 * Use the freeuser parameter with care. recall that the hashtable stores
 * pointers to keys and data, plus key length. If user did not allocate each
 * key separately then setting freeuser would cause a crash. for example,  
 * if I used a vector of keys and a vector of key lengths; setting freeuser
 * would ask to free (key length) bytes at some vector offset, obviously
 * an error. also, currently freeuser does not check the memtable, so if
 * allocations are being tracked, freeuser should not be used. we could
 * change this code to query the memtable first, or alternatively, change
 * etch_free to not complain about allegedly missing memtable entries.
 */
int jenkins_clear (void* realtable, const int freekey, const int freeval, void* in, void** out) 
{
    etch_status_t status = ETCH_SUCCESS;
    int   freecount = 0, currcount = 0, freehandled = 0;
    int   is_static_keys = 0, is_static_values = 0, is_etch_free = 0;
    mapcallback callback = NULL;
    char* key, *value;
    htab* jenktable;
    etch_hashtable* ht = (etch_hashtable*) in; 

    if (!(jenktable = (htab*)realtable))
        return -1;

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_lock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    if (ht) 
    {   is_static_keys   = ht->is_readonly_keys;
        is_static_values = ht->is_readonly_values;
        is_etch_free     = ht->is_tracked_memory;
        callback         = ht->freehook;
    }   

    while (0 < (currcount = hcount(jenktable)))
    {         
        key   = hkey(jenktable);    /* free entry's key if asked */
        value = hstuff(jenktable);
                                    /* optional callback to handle free */
        freehandled = callback? callback(key, value): FALSE; 

        if (freekey && !is_static_keys && !freehandled)  
            if(is_etch_free)
                etch_free(key);
            else
                free(key);

        if (freeval && !is_static_values && !freehandled)
            if  (is_etch_free)
                etch_free(value);
            else free(value); 
    
        hdel(jenktable);  /* free current table slot */
        freecount++;
     }

    if (ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return freecount;     /* return count of items freed */
}

/**
 * implementation of etch_hashtable.count() for the jenkins hashtable. 
 * in and out parameters are ignored for this implementation.
 * result is current number of table entries, or -1 if error.
 */
int jenkins_count(void* realtable, void* in, void** out) 
{                                   
    const int count = realtable? ((htab*)realtable)->count: -1;   
    return count;  
}

/**
 * implementation of etch_hashtable.size() for the jenkins hashtable. 
 * in and out parameters are ignored for this implementation.
 * result is current maximum number of table entries, or -1 if error.
 */
int jenkins_size(void* realtable, void* in, void** out) 
{                                  
    const int count = realtable? 1 << ((htab*)realtable)->logsize: -1;   
    return count;  
}

/**
 * implementation of etch_hashtable.stats() for the jenkins hashtable. 
 * in and out parameters are ignored for this implementation.
 * result is current maximum number of table entries, or -1 if error.
 */
int jenkins_stats(void* realtable, void* in, void** out) 
{                                 
    if (realtable) hstat((htab*)realtable);     
    return realtable? 0: -1;  
}

/**
 * jenkins_hash
 * implementation of etch_hashtable.hash() for the jenkins hashtable. 
 * priorhash is result of the previous operation, or any arbitrary value.
 * in and out parameters are ignored for this implementation. result is a 
 * hash of the supplied key, as used by the jenkins hashtable, or zero
 * if parameters were in error.
 * author's comments: If you need less than 32 bits, use a bitmask.  
 * for example, if you need only 10 bits, do h = (h & hashmask(10)),
 * in which case, the hash table should have hashsize(10) elements.
 * if you are hashing n strings (unsigned char**)k, do it like this:
 * for (i=0, h=0; i < n; ++i) h = lookup(k[i], len[i], h);   
 */
int jenkins_hash(void* realtable, char* key, const int keylen, const int priorhash, void* in, void** out)  
{
    if (!realtable || !key || keylen < 1 || keylen > ETCH_HASH_MAX_KEYLENGTH) {
        //TODO: log error
        return 0;
    }
    return lookup(key, keylen, priorhash); 
}

/**
 * jenkins_hashx
 * see comments at jenkins_hash
 */
int jenkins_hashx(char* key, const int keylen, const int priorhash)  
{                                 
    if (!key || keylen < 1 || keylen > ETCH_HASH_MAX_KEYLENGTH) return 0;
    return lookup(key, keylen, priorhash); 
}

/* - - - - - - - - - - - - - - - - - - - - - 
 * constructors, destructors
 * - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * constructor for a hashtable implementation. implements iterable.
 * creates the underlying hashtable and returns a populated etch_hashtable
 * interface to it. initialsize is the number of items the hashtable can hold 
 * before it reallocates itself. note that this value may be altered by the
 * implementation, e.g. if it is out of range or if it is not a power of 2.
 */
etch_hashtable* new_hashtable(const unsigned int initialsize)
{
    etch_hashtable* hashtable = ctor_jenkins_hashtable(initialsize);

    new_iterable(&hashtable->iterable, NULL, hashtable_iterable_first, hashtable_iterable_next, hashtable_iterable_has_next); 

    hashtable->is_readonly_keys   = HASHTABLE_DEFAULT_READONLY_KEYS;
    hashtable->is_readonly_values = HASHTABLE_DEFAULT_READONLY_VALUES;
    hashtable->is_tracked_memory  = HASHTABLE_DEFAULT_TRACKED_MEMORY;
    hashtable->content_type       = HASHTABLE_DEFAULT_CONTENT_TYPE;

    return hashtable; 
}

etch_hashtable* new_hashtable_synchronized(const unsigned int initialsize)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_hashtable* hashtable = NULL;

    hashtable = new_hashtable(initialsize);
    // TODO: pool
    status = etch_mutex_create(&((etch_object*)hashtable)->synclock, ETCH_MUTEX_NESTED, NULL);
    if(status != ETCH_SUCCESS) {
        // error
    }
    return hashtable;
}

/**
 * constructor for etch_set, which is a hashtable containing object keys
 * and null values
 */
etch_set* new_set (const int initialsize)
{
    etch_set* newset = new_hashtable_synchronized(initialsize);
    newset->content_type = ETCHHASHTABLE_CONTENT_OBJECT_NONE;
    newset->is_readonly_values = TRUE;
    return newset;
}

/**
 * destructor for a hashtable implementation.
 * is_free_keys parameter asks that memory pointed to by hashbucket keys be freed.
 * Use this with care, since this obviously requires that you have malloc'ed keys 
 * individually, and did not, for example, hash your keys out of a memory vector, 
 * static variables, or the like. is_free_values likewise for the table content. 
 * The readonly flags set on the hashtable take precedence over either. 
 * Use of either of course means your pointers to content will now be dangling.  
 */
int destroy_hashtable(etch_hashtable* map, const int is_free_keys, const int is_free_values)
{
    etch_status_t status = ETCH_SUCCESS;
    int is_free_keymem = 0, is_free_valmem = 0;
    if (NULL == map) return -1;

    if (((etch_object*)map)->synclock) {
        status = etch_mutex_trylock(((etch_object*)map)->synclock);
        if(status != ETCH_SUCCESS) {
            return -1;
        }
    }

    if (!is_etchobj_static_content(map))
    {
        struct i_hashtable* vtab = (struct i_hashtable*)((etch_object*)map)->vtab;
        is_free_keymem = !map->is_readonly_keys   && is_free_keys;
        is_free_valmem = !map->is_readonly_values && is_free_values;
        
        /* free all buckets, and also contents only if is_free_contents is set */
        if (map->realtable && vtab && !is_etchobj_static_content(map)) 
        {   
            vtab->clear(map->realtable, is_free_keymem, is_free_valmem, map, 0);  
            vtab->hdestroy(map->realtable, map, 0);
        }
    } else {
    }

    if (((etch_object*)map)->synclock) {
        status = etch_mutex_unlock(((etch_object*)map)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
        if (!is_etchobj_static_content(map)) {
            status = etch_mutex_destroy(((etch_object*)map)->synclock);
            ETCH_ASSERT(status == ETCH_SUCCESS);
        }
    }

    //TODO:
    // cleanup synclock
    // maybe it is done by map->synchook(ETCH_SYNC_DEL

    if (!is_etchobj_static_shell(map)) {
        etch_free(map);
    }

    return 0;
}


/**
 * implementation of etch_hashtable.destroy() for the jenkins hashtable.
 * destroys the table, but not the memory allocated for the individual item 
 * keys and values. use clear(), not destroy(), for that purpose.
 * out parameter is ignored for this implementation.
 * result is zero if OK, otherwise -1;
 */
int jenkins_destroy(void* realtable, void* in, void** out)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_hashtable* ht = (etch_hashtable*) in; 
    
    if (!realtable)
        return -1;

    if(ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_trylock(((etch_object*)ht)->synclock);
        if(status != ETCH_SUCCESS) {
            return -1;
        }
    }

    if (((htab*)realtable)->table)
        hdestroy((htab*) realtable);  /* jenkhash.lib */

    if(ht && ((etch_object*)ht)->synclock) {
        status = etch_mutex_unlock(((etch_object*)ht)->synclock);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }

    return 0;
}


/**
 * implementation of etch_hashtable.create() for the jenkins hashtable.
 * this is the constructor for the underlying hashtable.
 * we receive initial size in items and convert this to bit width.
 * If initial size supplied is not a power of 2 we make it so.
 * jenkins takes a log2 value as size, e.g. size 6 means size is 6 bits wide = 64.
 * returns in *out, a pointer to a jenkins htab struct describing the underlying table.
 * in parameter is ignored for this implementation.
 * result is zero if OK, otherwise -1;
 */
int jenkins_create(const int initialsize_items, void* in, void** out)
{
    htab* hashtable = NULL;
    int initialsize_bits_plus1 = 0, initialsize, divby2;
    if (out == NULL) return -1; 

    if  (initialsize_items <= 0)
         initialsize = ETCH_DEFAULT_HASHTABLE_SIZE;
    else
    if  (initialsize_items < MIN_INITIAL_HASHTABLE_SIZE)
         initialsize = MIN_INITIAL_HASHTABLE_SIZE;
    else initialsize = initialsize_items;

    if  (initialsize > MAX_INITIAL_HASHTABLE_SIZE) 
         initialsize = MAX_INITIAL_HASHTABLE_SIZE;

    for (divby2 = initialsize; divby2; divby2 >>= 1)      
         initialsize_bits_plus1++; 

    hashtable = hcreate(initialsize_bits_plus1 - 1); /* jenkhash.lib */

    *out = hashtable;
    return hashtable? 0: -1;
}


/**
 * destroy_hashtable_default()
 * default destructor for jenkins hashtable
 */
int destroy_hashtable_default(etch_hashtable* map)
{
    destroy_hashtable(map, !map->is_readonly_keys, !map->is_readonly_values);
    return 0;
}


/**
 * clone_hashtable_default()
 * default copy constructor for jenkins hashtable.
 * we won't do an implementation at this level, since we would need to also clone
 * content, and only the instantiator knows key/value sizes
 */
etch_hashtable* clone_hashtable_default(etch_hashtable* map)
{
    return NULL;
}

/**
 * constructor for the etch_hashtable interface. constructs and initializes 
 * the interface shell, but not the underlying hashtable.
 */
etch_hashtable* _new_etch_hashtable()
{
    etch_hashtable* newht = 0;
    newht = (etch_hashtable*) new_object(sizeof(etch_hashtable),ETCHTYPEB_HASHTABLE,CLASSID_HASHTABLE);
    return newht; 
}

/**
 * ctor_jenkins_hashtable()
 * constructor for jenkins hashtable interface. 
 * populates interface implementation methods and creates the underlying hashtable.
 * returns pointer to etch_hashtable, or NULL if table could not be created.
 */
etch_hashtable* ctor_jenkins_hashtable(const int initialsize_items)
{
    htab* jenkins_hashtable   = NULL;
    etch_hashtable* hashtable = NULL;
    i_etch_hashtable* vtab    = NULL;
    const unsigned short CLASS_ID = CLASSID_HASHTABLE_VTAB;
    int result = 0, is_just_cached = FALSE;

    hashtable = _new_etch_hashtable(); 

    vtab = etch_cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    { 
        vtab = new_vtable(((etch_object*)hashtable)->vtab, sizeof(i_etch_hashtable), CLASS_ID);
        vtab->create   = jenkins_create;
        vtab->hdestroy = jenkins_destroy; 
        vtab->insert   = jenkins_insert;
        vtab->inserth  = jenkins_inserth;
        vtab->find     = jenkins_find;
        vtab->findh    = jenkins_findh;
        vtab->first    = jenkins_first;
        vtab->next     = jenkins_next;
        vtab->current  = jenkins_current;
        vtab->remove   = jenkins_remove;
        vtab->removeh  = jenkins_removeh;
        vtab->clear    = jenkins_clear;
        vtab->count    = jenkins_count;
        vtab->size     = jenkins_size;
        vtab->stats    = jenkins_stats;

        etch_cache_insert(((etch_object*)vtab)->get_hashkey(vtab), vtab, FALSE);
        is_just_cached = TRUE;
    }

    ((etch_object*)hashtable)->vtab = (vtabmask*)vtab;

    /* create the underlying real hashtable */
    result = vtab->create(initialsize_items, NULL, &jenkins_hashtable);
    hashtable->realtable = jenkins_hashtable;

    ((etch_object*)hashtable)->destroy = destroy_hashtable_default;
    ((etch_object*)hashtable)->clone   = clone_hashtable_default;

    if (result == -1)   
    {   
        if (is_just_cached) {
            etch_cache_del(CLASS_ID);
        }
        etch_object_destroy(hashtable);
    }

    return hashtable; 
}


/*
 * new_systemhashtable()
 * for such a hashtable we will not use the vtab interface
 * but rather will call the implementation methods directly.
 */
etch_hashtable* new_systemhashtable(const int initialsize_items)
{
    int result = 0;
    htab* jenkins_hashtable = NULL;
    etch_hashtable* hashtable = 0;
  
    hashtable = etch_malloc(sizeof(etch_hashtable), 0); 
    memset(hashtable, 0, sizeof(etch_hashtable)); 

    result = jenkins_create(initialsize_items, NULL, &jenkins_hashtable);
    hashtable->realtable = jenkins_hashtable;

    if (result == 0) 
        new_iterable(&hashtable->iterable, NULL, hashtable_iterable_first, 
            hashtable_iterable_next, hashtable_iterable_has_next); 

    else /* some problem creating hashtable */  
    {
        delete_systemhashtable(hashtable);
        hashtable = NULL;
    }
 
    return hashtable; 
}


/*
 * delete_systemhashtable()
 * delete an untracked hashtable
 */
void delete_systemhashtable(etch_hashtable* hashtable)
{
   if (!hashtable) return;
   jenkins_destroy(hashtable->realtable, NULL, NULL);
   free(hashtable);
}


/* - - - - - - - - - - - - - - - - - - - - - 
 * hashtable synchronization
 * - - - - - - - - - - - - - - - - - - - - -
 */

/*
 * hashtable_defsynchook()
 * hashtable synchronization hook usable for most purposes.
 * to enable synchronization, set map.synchook to this function, 
 * and set map.synclock to an instantiated mutex.
 */
//int hashtable_defsynchook(void* action, void* mutex)
//{
//    /* all the casting is to quash pointer cast warnings */
//    return etch_mutex_default_hookproc((int)(((size_t) action) & 0xf), mutex);
//}


/* 
 * hashtable_getlock()
 * explicitly set this map's synchronization lock, waiting if unavailable.
 * this should be used only for locking a map prior to iterating the map.   
 * for synchronization of map operations, the presence of map.synchook  
 * and map.synclock is sufficient. 
 */
int hashtable_getlock (etch_hashtable* map)
{
    etch_status_t status = ETCH_SUCCESS;
    ETCH_ASSERT(map && ((etch_object*)map)->synclock);
    if(((etch_object*)map)->synclock) {
        status = etch_mutex_lock(((etch_object*)map)->synclock);
        if(status != ETCH_SUCCESS) {
            return -1;
        }
    }
    return 0;
}

/* 
 * hashtable_trylock()
 * explicitly set this map's synchronization lock, failing if unavailable.
 * this should be used only for locking a map prior to iterating the map.   
 * for synchronization of map operations, the presence of map.synchook  
 * and map.synclock is sufficient. 
 */
int hashtable_trylock (etch_hashtable* map)
{
    etch_status_t status = ETCH_SUCCESS;
    ETCH_ASSERT(map && ((etch_object*)map)->synclock);
    if(((etch_object*)map)->synclock) {
        status = etch_mutex_trylock(((etch_object*)map)->synclock);
        if(status != ETCH_SUCCESS) {
            return -1;
        }
    }
    return 0;
}

/* 
 * hashtable_rellock()
 * release explicitly set this map's synchronization lock.
 * this should be used only for unlocking a map after iterating the map.   
 * for synchronization of map operations, the presence of map.synchook  
 * and map.synclock is sufficient. 
 */
int hashtable_rellock (etch_hashtable* map) 
{
    etch_status_t status = ETCH_SUCCESS;
    ETCH_ASSERT(map && ((etch_object*)map)->synclock);
    if(((etch_object*)map)->synclock) {
        status = etch_mutex_unlock(((etch_object*)map)->synclock);
        if(status != ETCH_SUCCESS) {
            return -1;
        }
    }
    return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - 
 * i_iterable implementations
 * - - - - - - - - - - - - - - - - - - - - -
 */

/*
 * hashtable_iterable_first() 
 * i_iterable first() implementation
 */
int hashtable_iterable_first(etch_iterator* iter)
{
    etch_hashtable* hash = NULL;
    etch_hashitem   hashbucket, *outentry = &hashbucket;
    if (!iter || !iter->collection) return -1;
    iter->current_key = iter->current_value = NULL;
    hash = iter->collection;

    if (-1 == ((struct i_hashtable*)((etch_object*)hash)->vtab)->first(hash->realtable, 0, &outentry))
        return -1;

    iter->current_key   = outentry->key;
    iter->current_value = outentry->value;     
    iter->ordinal++;
    return 0;
}


/*
 * hashtable_iterable_next() 
 * i_iterable next() implementation
 * functions as first() if there is no current position.
 */
int hashtable_iterable_next(etch_iterator* iter)
{
    etch_hashtable* hash = NULL;
    etch_hashitem   hashbucket, *outentry = &hashbucket;
    if (!iter || !iter->collection || !iter->ordinal) return -1;
    iter->current_key = iter->current_value = NULL;
    hash = iter->collection;

    if (-1 == ((struct i_hashtable*)((etch_object*)hash)->vtab)->next(hash->realtable, 0, &outentry))   
        return -1;

    iter->current_key   = outentry->key;
    iter->current_value = outentry->value;     
    iter->ordinal++;
    return 0;
}


/*
 * hashtable_iterable_has_next() 
 * i_iterable has_next() implementation.
 */
int hashtable_iterable_has_next(etch_iterator* iter)
{
    return iter && iter->collection && iter->current_key && iter->ordinal;
}


/* - - - - - - - - - - - - - - - - - - - - - 
 * clear() callbacks
 * - - - - - - - - - - - - - - - - - - - - -
 */

/* 
 * string_to_object_clear_handler()
 * callback set to handle freeing of key/value memory during destroy()  
 * and subsequent clear() of a string-to-etch_object hashtable.
 * handlers return FALSE to indicate memory management NOT handled,
 */
int string_to_object_clear_handler (void* data1, void* data2)  
{
  wchar_t* key = (wchar_t*)data1;
  etch_object* value = (etch_object*)data2;
    etch_free(key); /* free string key */
    etch_object_destroy(value); /* free etch object value */
    return TRUE;
}


/* 
 * object_to_object_clear_handler()
 * callback set to handle freeing of key/value memory during destroy()  
 * and subsequent clear() of a etch_object-to-etch_object hashtable.
 * handlers return FALSE to indicate memory management NOT handled.
 */
int object_to_object_clear_handler (void* data1, void* data2)
{
    etch_object* key = (etch_object*)data1;
    etch_object* value = (etch_object*)data2;
    etch_object_destroy(key);     /* free etch object key */
    etch_object_destroy(value); /* free etch object value */
    return TRUE;
}


/* 
 * etch_noop_clear_handler()
 * callback set to handle freeing of key/value memory during destroy()  
 * and subsequent clear() of a etch_object-to-etch_object hashtable.
 */
int etch_noop_clear_handler (void* key, void* value)  
{
    return TRUE;
}


/* - - - - - - - - - - - - - - - - - - - - - 
 * etch_map, etch_set
 * - - - - - - - - - - - - - - - - - - - - -
 */

/* 
 * new_etch_map()
 * an etch_map is a hashtable having object keys and object values. 
 * an object key should of course have its hashkey pre-computed appropriately.
 * if instantiator wants to use non-disposable objects as keys and/or values,
 * is_readonly_keys and/or is_readonly_values should be set, and the freehook
 * callback overridden. furthermore if caller chooses to use the same object  
 * as both key and value, similar steps should be taken to ensure that code  
 * does not try to destroy both key and value.
 */
etch_hashtable* new_etch_map(const unsigned int initialsize)
{
    etch_hashtable* newmap = new_hashtable(initialsize);
    newmap->content_type = ETCHHASHTABLE_CONTENT_OBJECT_OBJECT;
    newmap->freehook = object_to_object_clear_handler;
    newmap->is_readonly_keys = newmap->is_readonly_values = FALSE;
    return newmap;
}

/* 
 * new_etch_set()
 * an etch_set is a hashtable having object keys and null values. 
 */
etch_hashtable* new_etch_set(const unsigned int initialsize)
{
    etch_hashtable* newset = new_hashtable(initialsize);
    ((etch_object*)newset)->class_id = CLASSID_ETCH_SET; /* serializer will expect this */
    newset->content_type = ETCHHASHTABLE_CONTENT_OBJECT_NONE;
    newset->freehook = object_to_object_clear_handler; 
    newset->is_readonly_keys   = FALSE;
    newset->is_readonly_values = TRUE; 
    return newset;
}


/**
 * get_map_keys()
 * return a collection of the specified map's keys.
 * caller must invoke the destructor on the returned list. the returned list 
 * is marked as readonly content, in order that arraylist_destroy() will not 
 * attempt to free memory for the list content, which remains owned by the map.
 */
etch_arraylist* get_map_keys(etch_hashtable* map)
{    
    etch_iterator iterator; 
    etch_arraylist* list = NULL;

    const int typecount = ((struct i_hashtable*)((etch_object*)map)->vtab)->count(map->realtable,0,0);
    list = new_etch_arraylist(typecount, 0);
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    list->is_readonly  = TRUE; /* content is refs to objects owned by map */

    set_iterator(&iterator, map, &map->iterable);

    while(((struct i_iterable*)iterator.object.vtab)->has_next(&iterator))
    {
        etch_arraylist_add(list, iterator.current_key);
        ((struct i_iterable*)iterator.object.vtab)->next(&iterator);
    }

    return list;
}

/**
 * get_map_values()
 * return a collection of the specified map's values.
 * caller must invoke the destructor on the returned list. the returned list 
 * is marked as readonly content, in order that arraylist_destroy() will not 
 * attempt to free memory for the list content, which remains owned by the map.
 */
etch_arraylist* get_map_values(etch_hashtable* map)
{    
    etch_iterator iterator; 
    etch_arraylist* list = NULL;

    const int typecount = ((struct i_hashtable*)((etch_object*)map)->vtab)->count(map->realtable,0,0);
    list = new_etch_arraylist(typecount, 0);
    list->content_type = map->content_type;
    list->is_readonly  = TRUE;  /* content is refs to objects owned by map */

    set_iterator(&iterator, map, &map->iterable);

    while(((struct i_iterable*)iterator.object.vtab)->has_next(&iterator))
    {
        etch_arraylist_add(list, iterator.current_value);
        ((struct i_iterable*)iterator.object.vtab)->next(&iterator);
    }

    return list;
}


