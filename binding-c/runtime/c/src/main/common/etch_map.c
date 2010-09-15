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
 * etchmap.c -- generic int or string to object map.
 * note that this code does not implement an etch_map type, that type was added
 * to etch after we had already defined this particular API.
 * todo: convert global cache to use this code.
 */

#include "etch_map.h"
#include "etch_mem.h"
#include <wchar.h>

#ifdef WIN32
#pragma warning (disable:4996)
#endif

/*
 * make_etchmap_key() 
 * given 32-bit key constructs alpha key for the map returning key byte length
 */
int make_etchmap_key(const unsigned int key, char* buf, const int buflen)
{
   //_itoa_s(key, buf, buflen, 10);
   sprintf(buf, "%d", key);
   return (int)strlen(buf);
}


/*
 * etchmap_populate_out() 
 * populate caller's out struct
 */
void etchmap_populate_out(etch_hashitem* useritem, etch_hashitem* curritem)
{
    useritem->key   = curritem->key;
    useritem->value = curritem->value;
    useritem->hash  = curritem->hash;
}


/*
 * etchmap_findxl() 
 * @return a reference to map content, not owned by caller.
 * if the etch_hashitem out parameter is specified, it is populated on success.
 */
void* etchmap_findxl(etch_hashtable* map, char* key, unsigned keylen, void** out)
{
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

    int result = jenkins_find(map->realtable, key, keylen, 0, (void**)&founditem);

    #if ETCHMAP_DEBUG
    #pragma warning(disable:4313) 
    if  (result == 0) 
         printf("etchmap_found key %s len %d addr %08x\n", 
                 key, keylen, founditem->value);
    else printf("etchmap_notfound key %s len %d\n", key, keylen);
    #endif 
  
    if (result == -1) return NULL;

    if (out)
        etchmap_populate_out(*out, founditem);        

    return founditem->value;
}


/*
 * etchmap_find() 
 * locate object with specified key, returning it or NULL.
 * @return a reference to map content, not owned by caller.
 * if the etch_hashitem out parameter is specified, it is populated on success.
 */
void* etchmap_find(etch_hashtable* map, const unsigned int objkey, void** out)
{
    char ckey[ETCHMAP_MAX_IKEYSIZE+1];

    unsigned keylen = make_etchmap_key(objkey, ckey, ETCHMAP_MAX_IKEYSIZE);

    return etchmap_findxl(map, ckey, keylen, out);
}


/*
 * etchmap_findx() 
 * locate object with specified ansi char key, returning it or NULL
 */
void* etchmap_findx(etch_hashtable* map, char* key, void** out)
{
    unsigned keylen = (unsigned)strlen(key);

    return etchmap_findxl(map, key, keylen, out);
}


/*
 * etchmap_findxw() 
 * locate object with specified unicode key, returning it or NULL
 * @return a reference to map content, not owned by caller.
 * if the etch_hashitem out parameter is specified, it is populated on success.
 */
void* etchmap_findxw(etch_hashtable* map, wchar_t* key, void** out)
{
    unsigned keylen = (unsigned)(wcslen(key) * sizeof(wchar_t));

    return etchmap_findxl(map, (char*) key, keylen, out);
}


/*
 * etchmap_find_by_hash() 
 * locate object with specified hashkey, returning it or NULL.
 * @return a reference to map content, not owned by caller.
 * if the etch_hashitem out parameter is specified, it is populated on success.
 */
void* etchmap_find_by_hash(etch_hashtable* map, const unsigned hash, void** out)
{
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

int result = jenkins_findh(map->realtable, hash, map, (void**)&founditem);
    if (result == -1) return NULL;

    if (out)
        etchmap_populate_out(*out, founditem);       
    
    return founditem->value;
}


/*
 * etchmap_current() 
 * return object at current position
 */
etch_hashitem* etchmap_current(etch_hashtable* map)
{
    etch_hashitem  hashbucket;
    etch_hashitem* founditem = &hashbucket;

int result = jenkins_current(map->realtable, 0, (void**)&founditem);
    return (result == -1)? NULL: founditem;
}


/*
 * etchmap_delxl() 
 * remove specified object from map given ansi char key and length.
 * return pointer to object, which is not freed here, it is owned by caller.
 */
void* etchmap_delxl (etch_hashtable* map, char* ckey, const unsigned keylen)
{
    etch_hashitem hashbucket;
    etch_hashitem* founditem = &hashbucket;

int result = jenkins_remove(map->realtable, ckey, keylen, 0, (void**)&founditem);
    if (result == -1) return NULL;

    free(founditem->key); /* free 4-byte key allocated in etchmap_add() */
    return founditem->value;
}


/*
 * etchmap_del() 
 * remove specified object from map given integer key.
 * return pointer to object, which is not freed here, it is owned by caller.
 */
void* etchmap_del (etch_hashtable* map, const unsigned int objkey)
{
    char ckey[ETCHMAP_MAX_IKEYSIZE+1];

    unsigned keylen = make_etchmap_key(objkey, ckey, ETCHMAP_MAX_IKEYSIZE);

    return etchmap_delxl(map, ckey, keylen);
}


/*
 * etchmap_delx() 
 * remove specified object from map given ansi char key.
 * return pointer to object, which is not freed here, it is owned by caller.
 */
void* etchmap_delx (etch_hashtable* map, char* key)
{
    const unsigned keylen = (unsigned)strlen(key);

    return etchmap_delxl(map, key, keylen);
}


/*
 * etchmap_delxw() 
 * remove specified object from map given unicode key.
 * return pointer to object, which is not freed here, it is owned by caller.
 */
void* etchmap_delxw (etch_hashtable* map, wchar_t* key)
{
    const unsigned keylen = (unsigned)(wcslen(key) * sizeof(wchar_t));

    return etchmap_delxl(map, (char*) key, keylen);
}


/*
 * etchmap_insertxl() 
 * add specified object to map given ansi char key and char length, 
 * with preexistence test optional.
 * @param key a string for which caller retains ownership, map makes a copy.
 * @param data an object owned by map or not depending on map attributes.
 * @return hash of supplied key, or zero if insertion error.
 */
int etchmap_insertxl (etch_hashtable* map, 
    char* key, const unsigned keylen, void* data, const int is_check)
{
    int   result = 0, keylent = 0;
    char* pkey = NULL;
    void* pval = NULL;
    etch_hashitem  hashbucket; 
    etch_hashitem* inserteditem = &hashbucket;
    memset(&hashbucket, 0, sizeof(etch_hashitem));

    if (is_check)
      {   pval = etchmap_findxl(map, key, keylen, (void**)&inserteditem);
        if (pval) return inserteditem->hash; /* entry exists */
    }

    keylent = keylen + 1;/* add new entry */
    pkey    = etch_malloc(keylent, 0); 
    //strcpy_s(pkey, keylent, key);
	strcpy(pkey, key);

    #if ETCHMAP_DEBUG
    #pragma warning(disable:4313) 
    printf("etchmap_insertxl key %s len %d addr %08x\n", pkey, keylen, data); 
    #endif

    result = jenkins_insert
      (map->realtable, pkey, keylen, data, 0, 0, (void**)&inserteditem);

    /* etchmap_dump(); */

    return inserteditem->hash;
} 


/*
 * etchmap_insertxlw() 
 * add specified object to map given unicode key and char length, 
 * with preexistence test optional.
 * @param key a string for which caller retains ownership, map makes a copy.
 * @param data an object owned by map or not depending on map attributes.
 * @return hash of supplied key, or zero if insertion error.
 */
int etchmap_insertxlw (etch_hashtable* map, 
    wchar_t* key, const unsigned keylen, void* data, const int is_check)
{
    int result = 0, keylent = 0;
    wchar_t* pkey = NULL;
    void*    pval = NULL;
    etch_hashitem  hashbucket; 
    etch_hashitem* inserteditem = &hashbucket;
    memset(&hashbucket, 0, sizeof(etch_hashitem));

    if (is_check)
      {   pval = etchmap_findxl(map, (char*) key, keylen, (void**)&inserteditem);
        if (pval) return inserteditem->hash; /* entry exists */
    }

    keylent = keylen + sizeof(wchar_t);     /* add new entry */
    /* watch this spot. issue observed here where etch_malloc reports that it 
     * "could not add x to heap tracking store". my guess is that a heap item
     * at address x had been etch_freed, but key x was for some reason not
     * removed from the tracking table. memory address x was subsequently
     * re-issued by malloc, and when etch_malloc attempts to add it to the
     * tracking table, the address already exists there (or there is a hash
     * collision?). regardless, this is a tracking error, is probably not a 
     * leak (because etch_free frees the memory regardless of this error), 
     * however the error messages are a problem so we need to address this.
     */
    pkey = etch_malloc(keylent, 0);   
    wcscpy(pkey, key);

    result = jenkins_insert
      (map->realtable, pkey, keylen, data, 0, 0, (void**)&inserteditem);

    return inserteditem->hash;
} 


/*
 * etchmap_insert() 
 * add specified object to map with preexistence test optional.
 * return inserted item hash, or zero if insertion error.
 */
int etchmap_insert (etch_hashtable* map, 
    const unsigned int key, void* data, const int is_check)
{
    char  ckey[ETCHMAP_MAX_IKEYSIZE+1];
    unsigned keylen = make_etchmap_key(key, ckey, ETCHMAP_MAX_IKEYSIZE);

    return etchmap_insertxl(map, ckey, keylen, data, is_check);
} 


/*
 * etchmap_insertx() 
 * add specified object to map with preexistence test optional.
 * param key a string for which caller retains ownership, map makes a copy.
 * @param data the value of the key/value pair, owned by map or not depending
 * on map attributes.
 * return inserted item hash, or zero if insertion error.
 */
int etchmap_insertx (etch_hashtable* map, char* key, void* data, const int is_check)
{
    unsigned keylen = (unsigned)strlen(key);
    
    return etchmap_insertxl(map, key, keylen, data, is_check);
} 


/*
 * etchmap_insertxw() 
 * add specified object to map with preexistence test optional.
 * param key a string for which caller retains ownership, map makes a copy.
 * @param data the value of the key/value pair, owned by map or not depending
 * on map attributes.
 * return inserted item hash, or zero if insertion error.
 */
int etchmap_insertxw (etch_hashtable* map, wchar_t* key, void* data, const int is_check)
{
    unsigned keylen = (unsigned)(wcslen(key) * sizeof(wchar_t));
    
    return etchmap_insertxlw(map, key, keylen, data, is_check);
} 


/*
 * etchmap_add() 
 * add specified object to map given integer key.
 * return 0 or -1.
 */
int etchmap_add (etch_hashtable* map, const unsigned int objkey, void* data)
{
    /* zero back from etchmap_insert() indicates insert error */ 
    return etchmap_insert (map, objkey, data, TRUE)? 0: -1;
} 


/*
 * etchmap_addx() 
 * add specified object to map.
 * return 0 or -1.
 */
int etchmap_addx(etch_hashtable* map, char* key, void* data)
{
    /* zero back from etchmap_insert() indicates insert error */ 
    return etchmap_insertx (map, key, data, TRUE)? 0: -1;
} 


/*
 * etchmap_addxw() 
 * add specified object to map.
 * return 0 or -1.
 */
int etchmap_addxw(etch_hashtable* map, wchar_t* key, void* data)
{
    /* zero back from etchmap_insert() indicates insert error */ 
    return etchmap_insertxw (map, key, data, TRUE)? 0: -1;
} 


/*
 * etchmap_count() 
 * return count of items in map.
 */
int etchmap_count(etch_hashtable* map)
{
    return map? ((struct i_hashtable*)((etch_object*)map)->vtab)->count(map->realtable, 0, 0): 0;
}


/* 
 * string_to_etchobject_clear_handler()
 * callback set to handle freeing of key memory during a clear() of  
 * a string to object type map, where the objects are etch_objects. 
 * handlers return FALSE to indicate memory free NOT handled.
 */
int string_to_etchobject_clear_handler (char* key, etch_object* value)  
{
    etch_free(key);
    etch_object_destroy(value);
    return TRUE; 
}


/* 
 * string_to_genericobject_clear_handler()
 * callback set to handle freeing of key memory during a clear() of  
 * a string to object type map, where the objects are not etch_objects. 
 * handlers return FALSE to indicate memory free NOT handled.
 */
int string_to_genericobject_clear_handler (char* key, void* value)  
{
    etch_free(key);
    etch_free(value);
    return TRUE; 
}


/* - - - - - - - - - - - - - - - -
 * etch_map (object to object map)
 * - - - - - - - - - - - - - - - -
 */

/**
 * etchmap_is_object_key
 * @return boolean value indicating whether hashtable key is an etch object
 */  
int etchmap_is_object_key(etch_hashtable* map)
{
    int result = FALSE;
    if (map) switch(map->content_type)
    {   case ETCHHASHTABLE_CONTENT_OBJECT_OBJECT:
        case ETCHHASHTABLE_CONTENT_OBJECT_NONE:
             result = TRUE;
    }
    return result;
}


/**
 * etchmap_map_add
 * inserts item to object/object hashtable
 * it is assumed that hashkey has been pre-computed on the key object.
 */  
int etchmap_map_add (etch_hashtable* map, etch_object* key, etch_object* value) 
{    
    int result = 0;
    key->get_hashkey(key);
    result = ((struct i_hashtable*)((etch_object*)map)->vtab)->inserth(map->realtable, key, value, map, 0); 
    return result;  
}


/**
 * etchmap_map_find()
 * finds object with specified object key .
 * it is assumed that hashkey has been pre-computed on the key object.
 * @return 0 or -1
 * found item is returned via out parameter, if supplied.
 */  
int etchmap_map_find(etch_hashtable* map, etch_object* key, etch_hashitem** out)
{
    const int result = ((struct i_hashtable*)((etch_object*)map)->vtab)->findh(map->realtable, key->get_hashkey(key), map, out); 
    return result;
}


/**
 * etchmap_set_add
 * inserts item to object/null hashtable
 * it is assumed that hashkey has been pre-computed on the key object.
 */  
int etchmap_set_add (etch_hashtable* set, etch_object* key) 
{   
    int result = 0; 
    key->get_hashkey(key);
    result = ((struct i_hashtable*)((etch_object*)set)->vtab)->inserth(set->realtable, key, NULL, set, 0); 
    return result;
}





