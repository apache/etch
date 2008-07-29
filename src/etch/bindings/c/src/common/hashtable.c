/**
   hashtable.c -- implementation of an underlying hashtable.
   Provided herein are implementations for the Jenkins hashtable APIs.
*/

#include "etchhash.h"
#include "etchmem.h"
#include "etchrun.h"
#include "..\..\3rdparty\jenkins\jenkhash\jenkhtab.h"
#include "..\..\3rdparty\jenkins\jenkhash\jenklook.h"


/**
 * Constructor for a hashtable implementation. Implements iterable.
 * Creates the underlying hashtable and returns a populated etch_hashtable
 * interface to it. initialsize is the number of items the hashtable can hold 
 * before it reallocates itself. Note that this value may be altered by the
 * implementation, e.g. if it is out of range or if it is not a power of 2.
 */
etch_hashtable* new_hashtable(const int initialsize)
{
    etch_hashtable* hashtable = ctor_jenkins_hashtable(initialsize);

    new_iterable(&hashtable->iterable, NULL, hashtable_iterable_first, 
        hashtable_iterable_next, hashtable_iterable_has_next); 

    return hashtable; 
}


/**
 * Destructor for a hashtable implementation.
 * is_free_keys parameter asks that memory pointed to by hashbucket keys be freed.
 * Use this with care, since this obviously requires that you have malloc'ed keys 
 * individually, and did not, for example, hash your keys out of a memory vector, 
 * static variables, or the like. is_free_values likewise for the table content. 
 * The readonly flags set on the hashtable take precedence over either. 
 * Use of either of course means your pointers to content will now be dangling.  
 */
void destroy_hashtable(etch_hashtable* hashtable, 
    const int is_free_keys, const int is_free_values)
{
    int is_free_keymem = 0, is_free_valmem = 0;
    if (!hashtable) return;

    is_free_keymem = !hashtable->is_readonly_keys   && is_free_keys;
    is_free_valmem = !hashtable->is_readonly_values && is_free_values;

    /* free all buckets, and also contents only if is_free_contents is set */
    if (hashtable->realtable && hashtable->vtab) 
    {   /* 11/12/07 made the hashtable shell the in parameter to clear() */
        hashtable->vtab->clear  (hashtable->realtable, is_free_keymem, is_free_valmem, hashtable, 0);  
        hashtable->vtab->destroy(hashtable->realtable, 0, 0);
    }

    if  (is_memtable_instance)/* free the wrapper */ 
         free(hashtable);
    else etch_free(hashtable);  
}


/**
 * Constructor for the etch_hashtable interface. Constructs and initializes 
 * the interface shell, but not the underlying hashtable.
 */
etch_hashtable* new_etch_hashtable()
{
    etch_hashtable* hashtable_object = 0;
    /* we do not track the allocation of the tracking hashtable      
     * since the tracking table does of course not yet exist. 
     */
    if  (is_memtable_instance)
         hashtable_object = malloc(sizeof(etch_hashtable));        
    else hashtable_object = etch_malloc(sizeof(etch_hashtable), TODO_OBJTYPE);

    memset(hashtable_object, 0, sizeof(etch_hashtable)); 
    return hashtable_object; 
}


/**
 * Implementation of etch_hashtable.insert() for the jenkins hashtable. 
 * key and data are pointers to memory owned by the caller. The hashtable does 
 * not make copies of this data. The caller is responsible for freeing said 
 * memory, however note that etch_hashtable.clear() frees this memory.
 * key cannot be null but data can be null.  
 * datalen, and in and out parameters, are ignored for this implementation.
 * Result is zero if OK, otherwise -1;
 */
int __cdecl jenkins_insert(void* realtable, void* key, const int keylen, 
    void* data, const int datalen, void* in, void** out) 
{
    int  result = 0;
    if (!realtable || !key || !keylen) return -1;

    if (FALSE == hadd((htab*)realtable, key, keylen, data)) /* jenkhash.lib */
        result = -1;    /* key already exists most likely */

    return result;  
}


/**
 * Implementation of etch_hashtable.find() for the jenkins hashtable. 
 * Moves the current position on the underlying table to that of supplied key.
 * if out is non_null, *out is assumed to point at a etch_hashitem struct,
 * and this struct is populated with pointers to the located item's key and value.
 * in parameter is ignored for this implementation.
 * Result is zero if OK, otherwise -1;
 */
int __cdecl jenkins_find(void* realtable, void* key, const int keylen, 
    void* in, void** out) 
{
    int result = 0;
    char* tkey = NULL; 
    void* data = NULL;
    if (!realtable || !key || !keylen) return -1;

    if (TRUE == hfind((htab*)realtable, (unsigned char*)key, keylen)) /* jenkhash.lib */
    {
        tkey = hkey  ((htab*)realtable);
        data = hstuff((htab*)realtable);

        if (out)
        {   etch_hashitem* outentry = (etch_hashitem*) *out; 
            outentry->key   = tkey;
            outentry->value = data;
        }
    } 
    else result = -1; /* key nonexistent most likely */

    return result;  
}


/**
 * Implementation of etch_hashtable.findh() for the jenkins hashtable. 
 * Implements a find by hashed key, otherwise see comments for find().
 * Result is zero if OK, otherwise -1;
 */
int __cdecl jenkins_findh(void* realtable, const unsigned int hashed, 
    void* in, void** out) 
{
    int result = 0;
    char* tkey = NULL; 
    void* data = NULL;
    if (!realtable || !hashed) return -1;

    if (TRUE == hfindx((htab*)realtable, hashed)) /* jenkhash.lib */
    {
        tkey = hkey  ((htab*)realtable);
        data = hstuff((htab*)realtable);

        if (out)
        {   etch_hashitem* outentry = (etch_hashitem*) *out; 
            outentry->key   = tkey;
            outentry->value = data;
        }
    } 
    else result = -1; /* key nonexistent most likely */

    return result;  
}


/**
 * Implementation of etch_hashtable.first() for the jenkins hashtable. 
 * Moves the current position on the underlying table to that of the first item.
 * If out is non_null, *out is assumed to point at a etch_hashitem struct,
 * and this struct is populated with pointers to the located item's key and value.
 * in parameter is ignored for this implementation.
 * Result is zero if OK, otherwise -1, indicating bad params or an empty table.
 */
int __cdecl jenkins_first(void* realtable, void* in, void** out) 
{
    int result = 0;
    char* tkey = NULL; 
    void* data = NULL;
    if (!realtable) return -1;

    if (TRUE == hfirst((htab*)realtable))  /* jenkhash.lib */
    {
        tkey = hkey  ((htab*)realtable);
        data = hstuff((htab*)realtable);

        if (out)
        {   etch_hashitem* outentry = (etch_hashitem*) *out; 
            outentry->key   = tkey;
            outentry->value = data;
        }
    } 
    else result = -1; /* table is empty most likely */

    return result;  
}


/**
 * Implementation of etch_hashtable.next() for the jenkins hashtable. 
 * Moves the current position on the underlying table to that of the next item 
 * in the table. If out is non_null, *out is assumed to point at a etch_hashitem,
 * and this struct is populated with pointers to the located item's key and value.
 * in parameter is ignored for this implementation.
 * Result is zero if OK, otherwise -1, indicating either bad params, or that 
 * there are no more entries, in which case the current position will have wrapped
 * to the first item, if any entries in fact still remain.
 */
int __cdecl jenkins_next(void* realtable, void* in, void** out) 
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
 * Implementation of etch_hashtable.current() for the jenkins hashtable. 
 * Retrieves data for the entry at the current hashtable position.
 * *out is assumed to point at a etch_hashitem struct; this struct is populated 
 * with pointers to the current item's key and value.
 * in parameter is ignored for this implementation.
 * Result is zero if OK, otherwise -1, indicating bad params or an empty table.
 */
int __cdecl jenkins_current(void* realtable, void* in, void** out) 
{
    int result = 0;
    char* tkey = NULL; 
    void* data = NULL;
    etch_hashitem* outentry = NULL; 
    if (!realtable || !out || !((htab*)realtable)->count) return -1;

    tkey = hkey  ((htab*)realtable);
    data = hstuff((htab*)realtable);

    outentry = (etch_hashitem*) *out; 
    outentry->key   = tkey;
    outentry->value = data;

    return tkey? 0: -1;
}


/**
 * Implementation of etch_hashtable.remove() for the jenkins hashtable. 
 * Frees the entry for the key supplied , but neither the key nor the value, 
 * are freed, recall that neither of these is a copy but are instead pointers.
 * To actually free memory for these items, pass etch_hashitem** &out),
 * and you can then free(out->key), and free(out->value), at your leisure. 
 * in parameter is ignored for this implementation.
 * Result is zero if OK, otherwise -1;
 */
int __cdecl jenkins_remove(void* realtable, void* key, const int keylen, 
    void* in, void** out) 
{
    int result = 0;
    char* tkey = NULL; 
    void* data = NULL;
    if (!realtable || !key || !keylen) return -1;

    if (TRUE == hfind((htab*)realtable, key, keylen)) /* locate entry */
    {
        tkey = hkey  ((htab*)realtable);              /* jenkhash.lib */
        data = hstuff((htab*)realtable);

        if (out)          /* save off the entry contents if requested */
        {   etch_hashitem* outentry = (etch_hashitem*) *out; 
            outentry->key   = tkey;
            outentry->value = data;
        }

        hdel((htab*)realtable);   /* delete entry at current position */
    } 
    else result = -1; /* key nonexistent most likely */

    return result;  
}


/**
 * Implementation of etch_hashtable.clear() for the jenkins hashtable. 
 * Empties the table and, if requested, frees memory for keys/and/or values. 
 * in and out parameters are ignored for this implementation.
 * Result is count of table entries freed, or -1 if error.
 * Use the freeuser parameter with care. Recall that the hashtable stores
 * pointers to keys and data, plus key length. If user did not allocate each
 * key separately then setting freeuser would cause a crash. For example,  
 * if I used a vector of keys and a vector of key lengths; setting freeuser
 * would ask to free (key length) bytes at some vector offset, obviously
 * an error. Also, currently freeuser does not check the memtable, so if
 * allocations are being tracked, freeuser should not be used. We could
 * change this code to query the memtable first, or alternatively, change
 * etch_free to not complain about allegedly missing memtable entries.
 * 11/12/07: quickest fix for gettting at memory tracking flag without
 * adding parameters or adding functions is to now look for an in* which
 * will be the etch_hashtable*. In the future if we need additional input
 * parameters, if we ensure that the etch_hashtable* is always the first
 * item in the parameter structure we will not break.
 */
int __cdecl jenkins_clear(void* realtable, const int freekey, const int freeval,
    void* in, void** out) 
{
    int   freecount = 0, currcount = 0;
    int   is_static_keys = 0, is_static_values = 0, is_etch_free = 0;
    char* key, *value;
    htab* jenktable;
    etch_hashtable* ht = NULL;
    if (!(jenktable = (htab*)realtable)) return -1;

    ht = (etch_hashtable*) in; 
    if (ht) 
    {   is_static_keys   = ht->is_readonly_keys;
        is_static_values = ht->is_readonly_values;
        is_etch_free     = ht->is_tracked_memory;
    }   
                                           /* while table not empty ... */
    while (0 < (currcount = hcount(jenktable)))       
    {         
        key   = hkey(jenktable);           /* free entry's key if asked */
        value = hstuff(jenktable);

        if (freekey && key && !is_static_keys)                       
            if  (is_etch_free) 
                 etch_free(key);
            else free(key); 
                        
        if (freeval && value && !is_static_values)
            if  (is_etch_free) 
                 etch_free(value);
            else free(value); 
    
        hdel(jenktable);                   /* free current table slot */                  
        freecount++;
     }

    return freecount;                      /* return count of items freed */
}


/**
 * Implementation of etch_hashtable.count() for the jenkins hashtable. 
 * in and out parameters are ignored for this implementation.
 * Result is current number of table entries, or -1 if error.
 */
int __cdecl jenkins_count(void* realtable, void* in, void** out) 
{                                  /* jenkhash.lib */
    const int count = realtable? ((htab*)realtable)->count: -1;   
    return count;  
}


/**
 * Implementation of etch_hashtable.size() for the jenkins hashtable. 
 * in and out parameters are ignored for this implementation.
 * Result is current maximum number of table entries, or -1 if error.
 */
int __cdecl jenkins_size(void* realtable, void* in, void** out) 
{                                 /* jenkhash.lib */
    const int count = realtable? 1 << ((htab*)realtable)->logsize: -1;   
    return count;  
}


/**
 * Implementation of etch_hashtable.stats() for the jenkins hashtable. 
 * in and out parameters are ignored for this implementation.
 * Result is current maximum number of table entries, or -1 if error.
 */
int __cdecl jenkins_stats(void* realtable, void* in, void** out) 
{                                 /* jenkhash.lib */
    if (realtable) hstat((htab*)realtable);     
    return realtable? 0: -1;  
}


/**
 * jenkins_hash
 * Implementation of etch_hashtable.hash() for the jenkins hashtable. 
 * priorhash is result of the previous operation, or any arbitrary value.
 * in and out parameters are ignored for this implementation. Result is a 
 * hash of the supplied key, as used by the jenkins hashtable, or zero
 * if parameters were in error.
 * Author's comments: If you need less than 32 bits, use a bitmask.  
 * For example, if you need only 10 bits, do h = (h & hashmask(10)),
 * in which case, the hash table should have hashsize(10) elements.
 * If you are hashing n strings (unsigned char**)k, do it like this:
 * for (i=0, h=0; i < n; ++i) h = lookup(k[i], len[i], h);   
 */
int __cdecl jenkins_hash(void* realtable, char* key, const int keylen, 
                         const int priorhash, void* in, void** out)  
{                                 /* jenkhash.lib */
    if (!realtable || !key || keylen < 1 || keylen > MAX_ETCHHASHKEY) return 0;    
    return lookup(key, keylen, priorhash); 
}


/**
 * Implementation of etch_hashtable.destroy() for the jenkins hashtable.
 * Destroys the table, but not the memory allocated for the individual item 
 * keys and values. use clear(), not destroy(), for that purpose.
 * in and out parameters are ignored for this implementation.
 * Result is zero if OK, otherwise -1;
 */
int __cdecl jenkins_destroy(void* realtable, void* in, void** out)
{
    if (realtable)
        hdestroy((htab*) realtable);  /* jenkhash.lib */
    
    return realtable? 0: -1;
}


/**
 * Implementation of etch_hashtable.create() for the jenkins hashtable.
 * This is the constructor for the underlying hashtable.
 * We receive initial size in items and convert this to bit width.
 * If initial size supplied is not a power of 2 we make it so.
 * Jenkins takes a log2 value as size, e.g. size 6 means size is 6 bits wide = 64.
 * Returns in *out, a pointer to a jenkins htab struct describing the underlying table.
 * in parameter is ignored for this implementation.
 * Result is zero if OK, otherwise -1;
 */
int __cdecl jenkins_create(const int initialsize_items, void* in, void** out)
{
    htab* hashtable = NULL;
    int initialsize_bits_plus1 = 0, initialsize, divby2;
    if (out == NULL) return -1; 

    initialsize = (initialsize_items < MIN_INITIAL_HASHTABLE_SIZE)?
        MIN_INITIAL_HASHTABLE_SIZE: initialsize_items;
    if (initialsize > MAX_INITIAL_HASHTABLE_SIZE) 
        initialsize = MAX_INITIAL_HASHTABLE_SIZE;

    for(divby2 = initialsize; divby2; divby2 >>= 1)      
        initialsize_bits_plus1++; 

    hashtable = hcreate(initialsize_bits_plus1 - 1); /* jenkhash.lib */

    *out = hashtable;
    return hashtable? 0: -1;
}


/**
 * Constructor for jenkins hashtable interface. 
 * Populates interface implementation methods and creates the underlying hashtable.
 * Returns pointer to etch_hashtable, or NULL if table could not be created.
 */
etch_hashtable* ctor_jenkins_hashtable(const int initialsize_items)
{
    htab* jenkins_hashtable = NULL;
    etch_hashtable* hashtable = NULL;
    etch_hashtable_vtable* vtab = NULL;
    int result = 0, is_just_cached = FALSE;

    hashtable = new_etch_hashtable(); 

    vtab = cache_find(ETCHTYPE_VTABLE_JENK);

    if(!vtab)  
    {   /* vtab not cached yet: we do not track this allocation since the   
         * tracking table will not exist until this constructor is complete */
        vtab = malloc(sizeof(etch_hashtable_vtable));
        vtab->create  = jenkins_create;
        vtab->destroy = jenkins_destroy;
        vtab->insert  = jenkins_insert;
        vtab->find    = jenkins_find;
        vtab->findh   = jenkins_findh;
        vtab->first   = jenkins_first;
        vtab->next    = jenkins_next;
        vtab->current = jenkins_current;
        vtab->remove  = jenkins_remove;
        vtab->clear   = jenkins_clear;
        vtab->count   = jenkins_count;
        vtab->size    = jenkins_size;
        vtab->stats   = jenkins_stats;

        cache_add(ETCHTYPE_VTABLE_JENK, vtab);
        is_just_cached = TRUE;
    } 
 
    hashtable->vtab = vtab;

    /* create the underlying real hashtable */
    result = vtab->create(initialsize_items, NULL, &jenkins_hashtable);
    hashtable->realtable = jenkins_hashtable;

    if (result == -1)   
    {   if (is_just_cached)
            cache_del(ETCHTYPE_VTABLE_JENK);
        vtab->destroy(jenkins_hashtable, NULL, NULL);
        free(hashtable); hashtable = NULL;
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
  
    hashtable = malloc(sizeof(etch_hashtable)); 
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


/*
 * hashtable_iterable_first() 
 * i_iterable first() implementation
 */
int hashtable_iterable_first(etch_iterator* iter)
{
    int result = 0;
    etch_hashtable* hash = NULL;
    etch_hashitem   hashbucket;
    etch_hashitem*  outentry = &hashbucket;
    if (!iter || !iter->collection) return -1;
    iter->current_key = iter->current_value = NULL;
    hash = iter->collection;

    result = hash->vtab->first(hash->realtable, 0, &outentry);
    if (result == -1) return -1;

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
    int result = 0;
    etch_hashtable* hash = NULL;
    etch_hashitem   hashbucket;
    etch_hashitem*  outentry = &hashbucket;
    if (!iter || !iter->collection) return -1;
    iter->current_key = iter->current_value = NULL;
    hash = iter->collection;

    result = hash->vtab->next(hash->realtable, 0, &outentry);
    if (result == -1) return -1;

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


