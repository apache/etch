/*
 * etchhash.h 
 * hashtable interface
 */

#ifndef ETCHHASH_H
#define ETCHHASH_H

#include "etch.h"
#include "etch_collection.h"

#define MAX_INITIAL_HASHTABLE_SIZE 32768
#define MIN_INITIAL_HASHTABLE_SIZE  16
#define ETCHHASHTABLE_CONTENT_OBJECT 1


/** 
 *  etch_hashitem object
 *  An entry in a hashtable. 
 */
typedef struct etch_hashitem
{
    char* key;
    void* value; 
       
} etch_hashitem;


/** 
 * The etch C hashtable interface.
 * This is an interface to an underlying hash table implementation, such that some
 * other C "object" can "derive" from a hashtable, by including an etch_hashtable
 * struct as a part of that "object".  For example:
 *
 *   typedef struct foo {
 *     etch_hashtable* foohash;
 *     int some_other_data;
 *   } foo;
 *   foo* myfoo = malloc(sizeof(foo));
 *   myfoo->foohash = new_hashtable(64);
 *   myfoo->foohash->vtab->insert(myfoo->foohash->realtable,"sesame",6,NULL,0,0,0));
 *   destroy_hashtable(myfoo->foohash, FALSE);
 *
 * All methods of this interface should have implementations (i.e. not be null).
 */
typedef struct etch_hashtable_vtable    
{
    int (*create)  (const int size,  void* in, void** out);

    int (*destroy) (void* realtable, void* in, void** out);

    int (*insert)  (void* realtable, void* key, const int keylen, 
                    void* data, const int datalen, void* in, void** out);

    int (*find)    (void* realtable, void* key, const int keylen, 
                    void* in, void** out);

    int (*findh)   (void* realtable, const unsigned int hashed, 
                    void* in, void** out);

    int (*first)   (void* realtable, void* in, void** out);

    int (*next)    (void* realtable, void* in, void** out);

    int (*current) (void* realtable, void* in, void** out);

    int (*remove)  (void* realtable, void* key, const int keylen, 
                    void* in, void** out);

    int (*clear)   (void* realtable, const int freekey, const int freeval,
                    void* in, void** out);

    int (*count)   (void* realtable, void* in, void** out);

    int (*size)    (void* realtable, void* in, void** out);

    int (*stats)   (void* realtable, void* in, void** out);

    int (*hash)    (void* realtable, void* key, const int keylen, 
                    const int priorhash, void* in, void** out); 

} etch_hashtable_vtable;


typedef struct etch_hashtable   
{
    /* virtual methods */
    etch_hashtable_vtable* vtab;
    i_iterable iterable;
 
    /* instance data */ 
    void*      realtable;  /* implementation's hashtable object */
    byte       is_readonly_keys;
    byte       is_readonly_values;
    byte       is_tracked_memory;
    byte       content_type;

} etch_hashtable;

 

etch_hashtable* new_hashtable(const int initialsize);
etch_hashtable* new_etch_hashtable();
void destroy_hashtable(etch_hashtable* hashtable, const int is_free_k, const int is_free_v);

int jenkins_insert (void* realtable, void* key, const int keylen, 
    void* data, const int datalen, void* in, void** out);
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
int jenkins_hash   (void* realtable, char* key, const int keylen, 
    const int priorhash, void* in, void** out);
int jenkins_destroy(void* realtable, void* in, void** out); 
int jenkins_create (const int initialsize_items, void* in, void** out);
etch_hashtable* ctor_jenkins_hashtable(const int initialsize_bits);
etch_hashtable* new_systemhashtable   (const int initialsize_bits);
void delete_systemhashtable(etch_hashtable*);

/* i_iterable function overrides */
int   hashtable_iterable_first   (etch_iterator*);
int   hashtable_iterable_next    (etch_iterator*);
int   hashtable_iterable_has_next(etch_iterator*);


#endif /* #ifndef ETCHHASH_H */