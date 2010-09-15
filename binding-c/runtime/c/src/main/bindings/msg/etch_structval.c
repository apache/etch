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
 * etch_structval.c -- etch_structvalue implementation.
 */

#include "etch_runtime.h"
#include "etch_structval.h"
#include "etch_validator.h"
#include "etch_encoding.h"
#include "etch_exception.h"
#include "etch_map.h"
#include "etch_log.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"

char* ETCHSVAL = "SVAL";

etch_structvalue* new_structvalue_init();

/* 
 * structvalue_clear_handler()
 * this callback is set to handle freeing of key and value memory during a clear()
 * of the structvalue map. structs own all their memory, so if there is a problem
 * here, it should be resolved at the source, not by modifying this code.
 */
int structvalue_clear_handler (void* data1, void* data2)
{
    etch_field* key = (etch_field*)data1;
    etch_object* value = (etch_object*)data2;
    etch_object_destroy(value);
	destroy_field(key);
    return TRUE;
}



/**
 * new_structvalue()  
 * primary constructor for etch_structvalue. 
 * @param etch_type object, caller retains ownership of the type as usual
 */
etch_structvalue* new_structvalue(etch_type* type, const int initialsize)
{
    etch_structvalue* newobj = new_structvalue_init(initialsize);

    newobj->struct_type = type;

    return newobj;
}


/**
 * destroy_structvalue()
 * destructor for an etch_structvalue object
 *
 * a structvalue owns all its memory *except* its type object, which is global  
 * to the vf. this means that (a) the etch_type supplied on construction must be   
 * a reference to a type owned by the service vf (or the unit test); and (b) all 
 * struct keys must be etch_field* allocated on the heap and not referenced again 
 * outside of that structvalue scope; and (c) all struct values must be etch object
 * references allocated on the heap and not referenced again outside of the 
 * structvalue scope.
 */
int destroy_structvalue(void* data) 
{
    etch_structvalue* thisp = (etch_structvalue*)data;
    if (NULL == thisp) return 0;
    
        
    if (!is_etchobj_static_content(thisp)){
        etch_object_destroy(thisp->items);
		thisp->items = NULL;
	}

    /* see comments above as to why we don't destroy type */

    destroy_objectex((etch_object*)thisp);   
    return 0;
}


/**
 * new_structvalue_init() (private)
 * common initialization on etch_structvalue construction.
 */
etch_structvalue* new_structvalue_init(const int initialsize)
{
    etch_structvalue* newobj = etch_malloc(sizeof(etch_structvalue), ETCHTYPEB_STRUCTVAL);
    memset(newobj, 0, sizeof(etch_structvalue));
    ((etch_object*)newobj)->obj_type = ETCHTYPEB_STRUCTVAL;
    ((etch_object*)newobj)->class_id = CLASSID_STRUCTVALUE; /* for now anyway */

    ((etch_object*)newobj)->destroy = destroy_structvalue;
    ((etch_object*)newobj)->clone   = clone_null;

    newobj->items = new_structvalue_hashtable(initialsize);
    /* mark map such that it knows its keys and values are etch objects */
    newobj->items->content_type = ETCHHASHTABLE_CONTENT_OBJECT_OBJECT;

    return newobj;
}





/*  
 * new_structvalue_hashtable
 * create the backing store for a struct value
 */
etch_hashtable* new_structvalue_hashtable(const int initialsize) 
{    
    etch_hashtable* ht = new_hashtable(initialsize);  
    if (ht == NULL) return NULL;
    ht->content_type       = ETCH_STRUCT_DEFAULT_CONTENT_TYPE;
    ht->is_tracked_memory  = ETCH_STRUCT_DEFAULT_TRACKED_MEM;
    ht->is_readonly_keys   = ETCH_STRUCT_DEFAULT_READONLY_KEY;
    ht->is_readonly_values = ETCH_STRUCT_DEFAULT_READONLY_VAL;
    ht->freehook = structvalue_clear_handler;
    return ht;
}


/**
 * structvalue_put()
 * inserts (or removes) specified key/value pair to/from struct store.
 * @param key an etch_field whose destructor will be invoked when the struct is
 * destroyed. presumably this etch_field is disposable; if not, the object must
 * be marked as immutable using set_etchobj_static_all.
 * @param value a *disposable* object which is the value of the key/val pair.
 * this object's destructor will be invoked when the struct is destroyed.
 * presumably this object and its content are disposable; if not, the object must
 * be marked accordingly using set_etchobj_static_all or set_etchobj_static_content.
 * returns 0 or -1.
 */
int structvalue_put(etch_structvalue* thisp, etch_field* key, etch_object* value) 
{   
    etch_config_t*  config    = NULL;
    int32           propvalue = 0;
    etch_hashtable* map       = NULL;

    etch_runtime_get_config(&config);
    ETCH_ASSERT(config);

    map = thisp->items;
    if (NULL == key) return -1;

    if (NULL == value)  /* per contract, no value implies removal desired */
        return ((struct i_hashtable*)((etch_object*)map)->vtab)->removeh (map->realtable, ((etch_object*)key)->get_hashkey(key), key, 0);

    etch_config_get_property_int(config, "etch.validate.write", &propvalue);
    if(propvalue == 1) {
        etch_type* thistype = thisp->struct_type;
        char *errmsg = NULL;

        etch_validator* vtor = (etch_validator*)
            etchtype_get_validator_by_name(thistype, key->name);

        if (NULL == vtor)
            errmsg = "validator missing";
        else
        if (0 != vtor->validate (vtor, (etch_object*) value))
            errmsg = "validation failed";

        if (errmsg)
        {   ETCH_LOG(ETCHSVAL, ETCH_LOG_ERROR, "%s for type '%s' field '%s'\n", errmsg, thistype->aname, key->aname);
            return -1;
        }
    }
    return ((struct i_hashtable*)((etch_object*)map)->vtab)->inserth (map->realtable, key, value, map, 0);    
}


/**
 * structvalue_get()
 * access an element from the struct.
 * returns a reference not a copy.
 */
etch_object* structvalue_get (etch_structvalue* thisp, etch_field* key) 
{
    int  result = 0;
    etch_hashitem   hashbucket;
    etch_hashitem*  thisitem = &hashbucket; 
    etch_hashtable* map = thisp? thisp->items: NULL;
    if (NULL == map) return NULL;

    result = ((struct i_hashtable*)((etch_object*)map)->vtab)->findh(map->realtable, ((etch_object*)key)->get_hashkey(key), map, (void**)&thisitem);
    
    return result == 0? thisitem->value: NULL;
}


/**
 * structvalue_remove
 * removes an element from the struct, returning the element.
 * if the element is found, its key is destroyed, and the object is returned.
 * caller owns returned object.
 */
etch_object* structvalue_remove(etch_structvalue* thisp, etch_field* key) 
{
    int  result = 0;
    etch_hashitem   hashbucket;
    etch_hashitem*  thisitem = &hashbucket; 
    etch_hashtable* map = thisp? thisp->items: NULL;
    memset(thisitem,0,sizeof(etch_hashitem));
    if (NULL == map) return NULL;

    /* remove specified item from hashtable without destroying content */
    result = ((struct i_hashtable*)((etch_object*)map)->vtab)->removeh(map->realtable, ((etch_object*)key)->get_hashkey(key), map, (void**)&thisitem);
    if (-1 == result) return NULL;

    /* free entry key */
    if (thisitem->key) 
        if  (etchmap_is_object_key(map))
            ((etch_object*)thisitem->key)->destroy(thisitem->key);
        else etch_free(thisitem->key);
    
    return (etch_object*) thisitem->value;
}


/**
 * structvalue_is_type()
 * indicates if type of this struct is the same as the specified type
 */
int structvalue_is_type(etch_structvalue* thisp, etch_type* type)
{
    return is_equal_types(thisp->struct_type, type);
}


/**
 * structvalue_count()
 * returns number of pairs in the struct
 */
int structvalue_count(etch_structvalue* sv)
{
    return etchmap_count(sv->items);
}
