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
 * etch_hash.h 
 * etch hashtable 
 */

#ifndef ETCHHASH_H
#define ETCHHASH_H

#include "etch.h"
#include "etch_mutex.h"
#include "etch_collection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INITIAL_HASHTABLE_SIZE          32768
#define MIN_INITIAL_HASHTABLE_SIZE          4
#define ETCH_DEFAULT_HASHTABLE_SIZE         16
#define ETCHHASHTABLE_CONTENT_OPAQUE        0
#define ETCHHASHTABLE_CONTENT_OBJECT        1   /* opaque key */
#define ETCHHASHTABLE_CONTENT_INT_OBJECT    2   
#define ETCHHASHTABLE_CONTENT_LONG_OBJECT   3
#define ETCHHASHTABLE_CONTENT_STRING_OBJECT 4
#define ETCHHASHTABLE_CONTENT_OBJECT_OBJECT 5   /* etch_map */
#define ETCHHASHTABLE_CONTENT_OBJECT_NONE   6   /* etch_set */

#define HASHTABLE_DEFAULT_READONLY_KEYS     TRUE
#define HASHTABLE_DEFAULT_READONLY_VALUES   TRUE
#define HASHTABLE_DEFAULT_TRACKED_MEMORY    TRUE
#define HASHTABLE_DEFAULT_CONTENT_TYPE      0

//TODO: clean up BEGIN
typedef int (*mapcallback) (void*, void*); /* hashtable callback signature */
int string_to_object_clear_handler (void*, void*);
int object_to_object_clear_handler (void*, void*);
int etch_noop_clear_handler (void* key, void* value);
//TODO: clean up END 

/** 
 *  etch_hashitem  
 *  an entry in a hashtable. 
 */
typedef struct etch_hashitem
{
    char* key;
    void* value;
    unsigned hash;

} etch_hashitem;

/** 
 * the etch C hashtable interface.
 * all methods of this interface should have implementations (i.e. not be null).
 */
typedef struct i_hashtable
{
    etch_object object;

    etchparentinfo* inherits_from; 

    int (*create)  (const int size,  void* in, void** out);
    int (*hdestroy)(void* realtable, void* in, void** out);
    int (*insert)  (void* realtable, void* key, const int keylen, void* data, const int datalen, void* in, void** out);
    int (*inserth) (void* realtable, void* key, void* data, void* in, void** out);
    int (*find)    (void* realtable, void* key, const int keylen, void* in, void** out);
    int (*findh)   (void* realtable, const unsigned key, void* in, void** out);
    int (*first)   (void* realtable, void* in, void** out);
    int (*next)    (void* realtable, void* in, void** out);
    int (*current) (void* realtable, void* in, void** out);
    int (*remove)  (void* realtable, void* key, const int keylen, void* in, void** out);
    int (*removeh) (void* realtable, const unsigned key, void* in, void** out);
    int (*clear)   (void* realtable, const int freekey, const int freeval, void* in, void** out);
    int (*count)   (void* realtable, void* in, void** out);
    int (*size)    (void* realtable, void* in, void** out);
    int (*stats)   (void* realtable, void* in, void** out);
    int (*hash)    (void* realtable, void* key, const int keylen, const int priorhash, void* in, void** out); 

} i_etch_hashtable;

/**
 * etch_hashtable
 * hashtable object
 */
typedef struct etch_hashtable
{
    etch_object object;

    void* realtable; /* implementation's hashtable object */
    unsigned short content_obj_type; /* todo: populate */
    unsigned short content_class_id; /* todo: populate */
    
    unsigned char is_readonly_keys;
    unsigned char is_readonly_values;
    unsigned char is_tracked_memory;
    unsigned char content_type;
      
    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    i_iterable  iterable;   /* iterable interface */

    mapcallback freehook;   /* clear() callback if any */

} etch_hashtable;

/**
 * create a new instance of etch_hashtable.
 */
etch_hashtable* new_hashtable(const unsigned int initialsize);

/**
 * create a new instance of etch_hashtable
 * that is synchronized.
 */
etch_hashtable* new_hashtable_synchronized(const unsigned int initialsize);

/**
 * create a new instancec of etch_map.
 */
etch_hashtable* new_etch_map(const unsigned int initialsize);

/**
 * create a new instance of etch_set.
 */
etch_hashtable* new_etch_set(const unsigned int initialsize);

/**
 * destory the instance of etch_hashtable.
 */
int destroy_hashtable(etch_hashtable* hashtable, const int is_free_k, const int is_free_v);

/* 
 * explicit synchronization locking methods.
 * these should be used only for locking a map during map iteration.   
 * for synchronization of map operations, the presence of map.synchook  
 * and map.synclock is sufficient. 
 */
int   hashtable_getlock (etch_hashtable*);
int   hashtable_trylock (etch_hashtable*);
int   hashtable_rellock (etch_hashtable*);

//int   hashtable_defsynchook(void* action, void* mutex);

/* i_iterable function overrides */
int   hashtable_iterable_first   (etch_iterator*);
int   hashtable_iterable_next    (etch_iterator*);
int   hashtable_iterable_has_next(etch_iterator*);

//TODO: cleanup

typedef etch_hashtable etch_set;
etch_set* new_set(const int initialsize);

#define is_etch_set(x) (x && (((etch_object*)x)->obj_type == ETCHTYPEB_HASHTABLE) \
 && (((etch_hashtable*)x)->content_type == ETCHHASHTABLE_CONTENT_OBJECT_NONE))

int jenkins_insert (void* realtable, void* key, const int keylen, void* data, const int datalen, void* in, void** out);
int jenkins_inserth(void* realtable, void* key, void* data, void* in, void** out);
int jenkins_find   (void* realtable, void* key, const int keylen, void* in, void** out);
int jenkins_findh  (void* realtable, const unsigned int hashed, void* in, void** out);
int jenkins_first  (void* realtable, void* in, void** out);
int jenkins_next   (void* realtable, void* in, void** out);
int jenkins_current(void* realtable, void* in, void** out);
int jenkins_remove (void* realtable, void* key, const int keylen, void* in, void** out);
int jenkins_clear  (void* realtable, const int freekey, const int freeval, void* in, void** out);
int jenkins_count  (void* realtable, void* in, void** out);
int jenkins_size   (void* realtable, void* in, void** out); 
int jenkins_stats  (void* realtable, void* in, void** out);
int jenkins_hash   (void* realtable, char* key, const int keylen, const int priorhash, void* in, void** out);
int jenkins_hashx  (char* key, const int keylen, const int priorhash);  
int jenkins_destroy(void* realtable, void* in, void** out); 
int jenkins_create (const int initialsize_items, void* in, void** out);
etch_hashtable* ctor_jenkins_hashtable(const int initialsize_bits);
etch_hashtable* new_systemhashtable   (const int initialsize_items);
void delete_systemhashtable(etch_hashtable*);


struct etch_hashtable;

struct etch_arraylist* get_map_keys(struct etch_hashtable*);
struct etch_arraylist* get_map_values(struct etch_hashtable*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHHASH_H */
