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
 * type.c -- methods on the etch_type object.
 * type denotes the type of a struct or message. when used with a message
 * it typically denotes an action or event. an etch_type is a typedef of 
 * etch_id_name, however it has additional instance data specific to type.
 */

#include "etch_type.h"
#include "etch_validator.h"
#include "etch_map.h"
#include "etch_serializer.h"
#include "etch_objecttypes.h"
#include "etch_general.h"

int fieldmap_clear_handler (void* key, void* value);
etch_hashtable* new_etchtype_fieldmap();
etch_hashtable* new_etchtype_vtormap();


/* - - - - - - - - - - - - - - - - - -
 * constructors and destructors
 * - - - - - - - - - - - - - - - - - -  
 */


/** 
 * destroy_type()
 * etch_type destructor
 */
int destroy_type(void* data)
{
    etch_type* type = (etch_type*)data;
    

    if (!is_etchobj_static_content(type)) /* e.g., not cloned */
    {          
        if (type->impl)
        {   etch_type_impl* impl = (etch_type_impl*) type->impl;
            etch_object_destroy(impl);
            type->impl = NULL;
        }
    }

    return destroy_id_name(type);
} 

/** 
 * clone_type()
 * etch_type quasi copy constructor
 * originally, type was simply a name and id. now a type has considerable extra
 * content. we do not clone that content here, but rather copy its reference 
 * from the source and mark the clone as having non-disposable content such that 
 * the type destructor won't try to free it. so the object is not a true clone.
 * note also that if the original were to be destroyed prior to the clone, the
 * clone's content memory reference would be hosed; thus we must ensure that we
 * only clone static types, i.e. types which are instantiated with the service,
 * (or in the case of unit tests, emulated as such), and destroyed only at 
 * service teardown.
 *
 * note finally that etch_type and etch_idname still have the same footprint, 
 * with the type instantiating its extra content at the impl* of the id_name.
 */
void* clone_type(void* data)
{
    const etch_type* type = (const etch_type*)data;
    etch_type* newtype = clone_id_name((etch_type*)type);
    newtype->impl = type->impl; 
    set_etchobj_static_content(newtype);
    return newtype;
} 

/** 
 * destroy_type_impl()
 * etch_type_impl destructor
 */
int destroy_type_impl(void* data)
{
    etch_type_impl* impl = (etch_type_impl*)data;

    if (!is_etchobj_static_content(impl))
    {   /* destruction of the maps causes all fields and non-cached validator
         * objects, to be destroyed. see etchtype_fieldmap_clear_handler, 
         * and etchtype_vtormap_clear_handler, below. */
        etch_object_destroy(impl->vtormap);
        etch_object_destroy(impl->fieldmap);
        etch_object_destroy(impl->impexphelper);
    }

    return destroy_objectex((etch_object*)impl);
} 


/**
 * new_type() 
 * etch_type constructor
 */
etch_type* new_type(const wchar_t* name) 
{
    etch_type_impl* impl = NULL;
    etchparentinfo* inheritlist = NULL;
    etch_type* newtype = new_id_name(name);
    if (NULL == newtype) return NULL;
    ((etch_object*)newtype)->obj_type = ETCHTYPEB_TYPE;
    ((etch_object*)newtype)->class_id = CLASSID_ID_TYPE;

    /* ensure parent type keys exist in (one-based) inheritance list */
    inheritlist = get_vtab_inheritance_list((etch_object*)newtype,
       2, 1, CLASSID_VTAB_FIELD);
    inheritlist[1].o.obj_type = ETCHTYPEB_ID_NAME;
    inheritlist[1].c.class_id = CLASSID_ID_NAME;

    /* instantiate instance data */
    impl = (etch_type_impl*) new_object(sizeof(etch_type_impl), 
            ETCHTYPEB_IDNAMEIMPL, CLASSID_TYPEIMPL);

    ((etch_object*)impl)->destroy    = destroy_type_impl;
    impl->async_mode = ETCH_ASYNCMODE_NONE; /* where is this default reset? */

    impl->fieldmap   = new_etchtype_fieldmap();
    impl->vtormap    = new_etchtype_vtormap();

    newtype->impl    = (etch_object*) impl;

    ((etch_object*)newtype)->destroy = destroy_type;
    ((etch_object*)newtype)->clone   = clone_type;    

    return newtype;
}


/**
 * new__static_type() 
 * create a type object whose destructor will have no effect.
 */
etch_type* new_static_type(const wchar_t* name)
{
    etch_type* newtype = new_type(name);
    set_etchobj_static_all(newtype);
    return newtype;
} 



/** 
 * destroy_static_type()
 * etch_type destructor.
 * this should not be set as the virtual dtor for a type, since the type
 * would then not be quasi-static as desired. it should be invoked explicitly
 */
int destroy_static_type(etch_type* type)
{
    clear_etchobj_static_all(type);
    return destroy_type(type); 
} 






/* - - - - - - - - - - - - - - - - - -
 * get/set
 * - - - - - - - - - - - - - - - - - -  
 */

/* mutators are implemented only for instance data specific to type, i.e.
 * resident in the etch_type_impl, following the c binding convention that we
 * implement a mutator only when the datum can't safely be get/set directly. 
 */

/** 
 * etchtype_set_type_stubhelper()
 * set type's 'stub helper', returning existing helper if any.
 * the stub helper is a function pointer, not an object.
 */
opaque_stubhelper etchtype_set_type_stubhelper(etch_type* type, opaque_stubhelper helper)
{
    /* sets a callback from message type to the particular API method implementation.
     * note that in the the java binding this is indirect, that is, this callback
     * calls a wrapper method which calls the implementation. however in our case,
     * the methods have same signature, so we call the implementation directly.
     * keep an eye on this though in case I have missed something.
     */
    opaque_stubhelper oldhelper = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   oldhelper = impl->stubhelper;
        impl->stubhelper = helper;
    }
    return oldhelper;
} 


/** 
 * etchtype_get_type_stubhelper()
 * getter for stub helper - see comments at set_type_stubhelper()
 */
opaque_stubhelper etchtype_get_type_stubhelper(etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->stubhelper: NULL;
} 


/** 
 * etchtype_set_result_type()
 * set result type, returning existing result type if any.
 * @param type a non-disposable reference to a type.
 * todo: if result type is set only at construction, lose this.
 */
etch_type* etchtype_set_result_type(etch_type* type, etch_type* rtype)
{
    etch_type* oldtype = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   oldtype = impl->result_type;
        impl->result_type = rtype;
    }
    return oldtype;
} 


/** 
 * etchtype_get_result_type()
 * returns a non-disposable reference to result type
 */
etch_type* etchtype_get_result_type(etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->result_type: NULL;
} 


/** 
 * etchtype_set_super_type()
 * todo: if set only at construction, lose this.
 * @param a non-disposable type
 * @return the previous type, not disposable.
 */
etch_type* etchtype_set_super_type(etch_type* type, etch_type* stype)
{
    etch_type* oldtype = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   oldtype = impl->super_type;
        impl->super_type = stype;
    }
    return oldtype;
} 


/** 
 * etchtype_get_super_type()
 * returns a non-disposable reference to super type
 */
etch_type* etchtype_get_super_type(etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->super_type: NULL;
} 


/** 
 * etchtype_set_component_type()
 * set associated component type and class for an array of this class.
 * @param type a non-disposable type object. neither owned nor stored here.
 * @return the existing objtype/classid representing component type.  
 */
unsigned int etchtype_set_component_type(etch_type* type, unsigned int typeclass)
{
    unsigned int old_class = 0;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   old_class = impl->component_class;
        impl->component_class = typeclass;
    }
    return old_class;
} 


/** 
 * etchtype_get_component_type()
 * returns component obj_type and class_id or zero indicating none set.
 */
unsigned int etchtype_get_component_type(etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->component_class: 0;
} 


/** 
 * etchtype_set_async_mode()
 * set async_mode, which determines if requests are run on the queued or
 * free thread pool.
 */
unsigned char etchtype_set_async_mode (etch_type* type, unsigned char mode)
{
    unsigned char old_mode = 0;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   old_mode = impl->async_mode;
        impl->async_mode = mode;
    }
    return old_mode;
} 


/** 
 * etchtype_get_async_mode()
 * returns component async_mode
 */
unsigned char etchtype_get_async_mode (etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->async_mode: 0;
} 


/** 
 * etchtype_set_timeout()
 * set timeout to wait for response, in milliseconds.
 */
unsigned int etchtype_set_timeout(etch_type* type, unsigned int ms)
{
    unsigned int old_timeout = 0;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   old_timeout = impl->timeout;
        impl->timeout = ms;
    }
    return old_timeout;
} 


/** 
 * etchtype_get_timeout()
 * get timeout to wait for response, in milliseconds.
 */
unsigned int etchtype_get_timeout(etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->timeout: 0;
} 


/** 
 * etchtype_set_run_validators()
 * set boolean run validators flag, returning existing value.
 */
unsigned char etchtype_set_run_validators(etch_type* type, unsigned char val)
{
    unsigned char old_flag = 0;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   old_flag = impl->is_run_validators;
        impl->is_run_validators = val;
    }
    return old_flag;
} 


/** 
 * etchtype_get_response_field()
 */
etch_field* etchtype_get_response_field(etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->response_field: NULL;
} 


/** 
 * etchtype_set_response_field()
 */
etch_field* etchtype_set_response_field(etch_type* type, etch_field* field)
{
    etch_field* old_field = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl)  
    {   old_field = impl->response_field;
        impl->response_field = field;
    }
    return old_field;
} 


/** 
 * etchtype_get_run_validators()
 * get boolean run validators flag.
 */
unsigned char etchtype_get_run_validators(etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return impl? impl->is_run_validators: 0;
} 


/** 
 * etchtype_set_impexphelper()
 * setter for import/export helper object
 * @param helper the helper object to be assigned, or null.
 * caller relinquishes ownership of this object if present.
 * @return the *disposable* prior helper object if any. 
 * if present, caller now owns this object.
 */
etch_serializer* etchtype_set_impexphelper(etch_type* type, etch_serializer* helper)
{
    etch_serializer* oldhelper = NULL;
    etch_type_impl*  impl = (etch_type_impl*) type->impl;
    if (impl) 
    {   oldhelper = impl->impexphelper;
        impl->impexphelper = helper;
    }
    return oldhelper;
} 


/** 
 * etchtype_get_impexphelper()
 * getter for import/export helper object
 * @return a *non-disposable* reference to helper object.
 * the type retains ownership of this object.
 */
etch_serializer* etchtype_get_impexphelper(etch_type* type)
{
    etch_type_impl* impl = type? (etch_type_impl*) type->impl: NULL;
    return impl? impl->impexphelper: NULL;
} 


/** 
 * etchtype_is_assignable_from()
 * indicate if this type is assignable from other, i.e. other a subclass of this
 */
int etchtype_is_assignable_from(etch_type* type, etch_type* othertype)
{
    etch_type* other_supertype;
    if (NULL == othertype) return FALSE;
    if (is_equal_types(type, othertype)) return TRUE;
    other_supertype = othertype->impl? 
       ((etch_type_impl*)othertype->impl)->super_type: NULL;
    return etchtype_is_assignable_from(type, other_supertype);
}


/* - - - - - - - - - - - - - - - - - 
 * fieldmap accessors
 * - - - - - - - - - - - - - - - - - 
 */

/**
 * get_key_by_name()
 * look up an etch_id_name key object by a name string. recall that etch_type 
 * and etch_field each are typedefs of etch_id_name. id_name derivations are
 * keyed by hash of name, so lookup is direct;
 */
etch_id_name* etchtype_get_key_by_name(etch_hashtable* map, const wchar_t* name)
{
    etch_hashitem hashbucket, *thisitem = &hashbucket; 
    const unsigned hashkey = etch_get_wchar_hashkey(name);
    const int result = ((struct i_hashtable*)((etch_object*)map)->vtab)->findh(map->realtable, hashkey, map, (void**)&thisitem);  
    return result == 0? (etch_id_name*) thisitem->key: NULL;
}


/**
 * get_idname_by_id()
 * given a hashtable and an id_name "id", return the map's id_name key having that id.
 * note that a non-disposable *reference* is returned, not a copy.
 */
etch_id_name* etchtype_get_key_by_id (etch_hashtable* map, const unsigned id)
{
    etch_iterator iterator;

    hashtable_getlock(map);
    set_iterator(&iterator, map, &map->iterable);
    while(iterator.has_next(&iterator))
    {
        etch_id_name* this_idname = (etch_id_name*) iterator.current_key;
		if (this_idname->id == id) {
			hashtable_rellock(map);
			return this_idname;
		}
        iterator.next(&iterator);
    }
	hashtable_rellock(map);
    return NULL;
}


/**
 * etchtype_add_field()
 * adds a field to set of fields
 * @param field caller must supply a disposable field object.
 * @return the argument. If there is a name collision, the existing field
 * is returned in place of the supplied field, AND the supplied field is
 * DESTROYED. this simplifies logic up the line, and is consistent with caller
 * expecting to relinquish responsibility for the field passed.
 */  
etch_field* etchtype_add_field (etch_type* type, etch_field* field) 
{    
    etch_field *effective_field = field;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    etch_hashtable* map  = impl->fieldmap;

    int result = ((struct i_hashtable*)((etch_object*)impl->fieldmap)->vtab)->inserth
       (map->realtable, field, NULL, map, 0); 

    if (-1 == result)
        effective_field = etchtype_get_field_by_name(type, field->name);

    if (effective_field != field)
        etch_object_destroy(field);
     
    #ifdef ETCHTYPE_DEBUG
    if  (effective_field) 
         wprintf(L"add field %08x '%s'\n", 
            (size_t) (void*) effective_field, effective_field->name);
    else wprintf(L"error adding field '%s'\n", field->name);
    #endif

    return effective_field;  
}


/**
 * etchtype_get_field_by_id()
 * @return a non-disposable reference to the requested field, or null
 */
etch_field* etchtype_get_field_by_id (etch_type* type, const unsigned id)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return etchtype_get_key_by_id(impl->fieldmap, id);
}


/**
 * etchtype_get_field_by_name()
 * works as in the java binding, in that if the type does not include   
 * a field with that name, a new field is created and added to the type.
 * @return a non-disposable reference to the requested field, or null
 */
etch_field* etchtype_get_field_by_name (etch_type* type, const wchar_t* name)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    etch_field* field = etchtype_get_key_by_name(impl->fieldmap, name);
    if (NULL == field)
        field = etchtype_add_field (type, new_field(name));
    return field;
}


/**
 * etchtype_get_fields()
 * returns a disposable arraylist of references. the list is marked
 * such that list->destroy() will not attempt to free content.
 * caller must cast result to etch_arraylist*
 */
void* etchtype_get_fields (etch_type* type)
{
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    return get_map_keys(impl->fieldmap);
}


/* 
 * etchtype_fields_count()
 * return count of fields resident in the type.
 */
int etchtype_fields_count(etch_type* type)
{
    int  count = 0;
    etch_hashtable* map = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl) map = impl->fieldmap;
    if (map)  count = ((struct i_hashtable*)((etch_object*)map)->vtab)->count(map->realtable, 0, 0);
    return count;
}


/* 
 * etchtype_set_fields_iterator()
 * initialize iterator over fields.
 */
int etchtype_set_fields_iterator(etch_type* type, etch_iterator* iterator)
{
    etch_hashtable* map = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl) map = impl->fieldmap;
    return map? set_iterator(iterator, map, &map->iterable): -1;
}


/* - - - - - - - - - - - - - - - - - - - 
 * validator insert/lookup
 * - - - - - - - - - - - - - - - - - - - 
 */

/**
 * etchtype_get_validator_by_id()
 * caller will want to cast result to etch_validator*.
 * note that the etch_type header can't include etch_validator header,
 * thus the anonymous pointers to etch_validator in these methods.  
 */
etch_object* etchtype_get_validator_by_id (etch_type* type, const unsigned id)
{
    etch_hashtable* map;
    etch_iterator iterator;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (!impl || !id) return NULL;
    map = impl->vtormap;

    set_iterator(&iterator, map, &map->iterable);

    while(iterator.has_next(&iterator))
    {
        etch_field* this_field = (etch_field*) iterator.current_key;
        if (this_field->id == id) 
            return iterator.current_value;
        iterator.next(&iterator);
    }

    return NULL;
}


/**
 * etchtype_get_validator_by_name()
 * @param name the name of the etch_field keying the validator.
 * caller will want to cast result to etch_validator*  
 */
etch_object* etchtype_get_validator_by_name (etch_type* type, const wchar_t* name)
{
    etch_type_impl* impl = type? (etch_type_impl*) type->impl: NULL;

    if (impl && name) 
    {  
        etch_hashitem hashbucket, *thisitem = &hashbucket; 
        etch_hashtable* map = impl->vtormap;
        const unsigned  key = etch_get_wchar_hashkey(name);
        if (0 == ((struct i_hashtable*)((etch_object*)map)->vtab)->findh(map->realtable, key, map, (void**)&thisitem)) 
            return thisitem->value;
    }

    return NULL;
}


/**
 * etchtype_put_validator()
 * adds a validator to validator chain for specified key
 * @param field relinquished regardless of result.
 * @param new_vtor relinquished regardless of result.
 */  
int etchtype_put_validator (etch_type* type, etch_field* field, etch_object* new_vtor) 
{    
    int result = -1;
    etch_hashtable *fmap, *vmap;
    etch_hashitem hashbucket, *mapentry = &hashbucket; 
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    ETCH_ASSERT(impl);

    do  /* validate parameters */
    {   
        if (!is_etch_validator(new_vtor)) {
    	    etch_object_destroy(field);
            field = NULL;
            break;
        }
        if (NULL == field) {   
	        etch_object_destroy(new_vtor);
	        new_vtor = NULL;
            break;
        }
        result = 0;
    } while(0);

    if (0 != result) return result;
    
    if (!impl || !is_etch_validator(new_vtor) || !field) return -1;
    memset(mapentry, 0, sizeof(etch_hashitem));
    fmap = impl->fieldmap;
    vmap = impl->vtormap;

    /* add field to fieldmap. if an eponymous field was present, it is returned
     * in place of caller's field, and caller's field has been destroyed. */
    field = etchtype_add_field (type, field); 

    /* if a validator exists under the specified key, we'll chain the new 
     * validator to the existing validator with a new combo validator object,
     * and replace the current vtor map entry with the new combo validator.
     */
    result = ((struct i_hashtable*)((etch_object*)vmap)->vtab)->removeh   /* if already in vtormap, remove it */
      (vmap->realtable, ((etch_object*)field)->get_hashkey(field), vmap, (void**)&mapentry);

    if (result == -1)   /* if it was not already in vtormap, insert it */
        result = ((struct i_hashtable*)((etch_object*)vmap)->vtab)->inserth (vmap->realtable, 
                    etch_object_clone_func(field), new_vtor, vmap, 0);
    else                /* ... otherwise insert a new chain head */
    {   etch_field* existing_key = (etch_field*) mapentry->key;
        etch_validator* existing_vtor = (etch_validator*) mapentry->value;

        etch_validator* vcombo       /* chain new vtor to existing vtor ... */
            = new_combo_validator(existing_vtor, (etch_validator*) new_vtor);

        result = ((struct i_hashtable*)((etch_object*)vmap)->vtab)->inserth /* ... and insert the new chained vtor */ 
           (vmap->realtable, existing_key, vcombo, vmap, 0); 
    }

    return result;  
}


/* 
 * etchtype_clear_validator()
 * remove the validator chain for specified key.
 */
int etchtype_clear_validator(etch_type* type, etch_field* key)
{
    int  result = -1;
    etch_hashtable* map = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl) map = impl->vtormap;

    if (map)
    {   etch_validator* removed_vtor = NULL;
        etch_hashitem hashbucket, *mapentry = &hashbucket; 
        memset(mapentry, 0, sizeof(etch_hashitem));
 
        result = ((struct i_hashtable*)((etch_object*)map)->vtab)->removeh(map->realtable, ((etch_object*)key)->get_hashkey(key), map, (void**)&mapentry);

        if (result == 0)  /* returned content is head of validator chain */ {
            removed_vtor = (etch_validator*) mapentry->value;
            destroy_type((etch_type*) mapentry->key);
        }

        if (removed_vtor) /* destructors are called up the chain */
            etch_object_destroy(removed_vtor);
    }

    return 0;
}


/* 
 * etchtype_clear_validators()
 * clear all type validators. non-cached validators are destroyed.
 * returns count of items cleared, or -1 if error.
 */
int etchtype_clear_validators(etch_type* type)
{
    int result = -1;
    etch_hashtable* map = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl) map = impl->vtormap;
    if (map) /* we ask hashtable clear() to call content destructors */
        result = ((struct i_hashtable*)((etch_object*)map)->vtab)->clear(map->realtable, FALSE, TRUE, map, 0);
    return result;
}


/* 
 * etchtype_validators_count()
 * return count of validators in chain.
 */
int etchtype_validators_count(etch_type* type)
{
    int  count = 0;
    etch_hashtable* map = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl) map = impl->vtormap;
    if (map)  count = ((struct i_hashtable*)((etch_object*)map)->vtab)->count(map->realtable, 0, 0);
    return count;
}


/* 
 * etchtype_set_validators_iterator()
 * initialize iterator over validators.
 */
int etchtype_set_validators_iterator(etch_type* type, etch_iterator* iterator)
{
    etch_hashtable* map = NULL;
    etch_type_impl* impl = (etch_type_impl*) type->impl;
    if (impl) map = impl->vtormap;
    return map? set_iterator(iterator, map, &map->iterable): -1;
}


/* - - - - - - - - - - - - - - - - - -
 * utility methods
 * - - - - - - - - - - - - - - - - - -  
 */

/* 
 * etchtype_fieldmap_clear_handler()
 * callback set to handle freeing of key memory during a clear() of the fields
 * map. note that this map is used as a quasi set, so the value is always null. 
 * the etch_fields are owned by the map and destroyed as the map is destroyed.
 * handlers return FALSE to indicate memory free NOT handled.
 */
int etchtype_fieldmap_clear_handler (void* key, void* value)  
{
   ((etch_object*)key)->destroy(key);
    return TRUE; 
}


/**
 * new_etchtype_fieldmap()
 * construct and return a hashtable configured as expected for the fieldmap  
 */
etch_hashtable* new_etchtype_fieldmap()
{
    etch_hashtable* map = new_hashtable_synchronized(ETCHTYPE_DEFSIZE_FIELDMAP); 
    if (NULL == map) return NULL; 
    map->content_type       = ETCHHASHTABLE_CONTENT_OBJECT;
    map->is_tracked_memory  = TRUE;
    map->is_readonly_keys   = FALSE; /* keys are disposable field objects */ 
    map->is_readonly_values = FALSE; /* value is always null */
    map->freehook = etchtype_fieldmap_clear_handler;
    return map;
}


/* 
 * etchtype_vtormap_clear_handler()
 * callback set to handle freeing of field and validator memory   
 * during a clear() of the validators map.  
 */
int etchtype_vtormap_clear_handler (void* key, void* value)  
{
    /* note that value->destroy() is invoking the validator destructor,
     * and that this will have no effect on a validator marked as cached */
    etch_object_destroy(value);
    etch_object_destroy(key);
    return TRUE; 
}


/**
 * new_etchtype_vtormap()
 * construct and return a hashtable configured as expected for validators.
 * this is a map of non-disposable etch_field keys to etch_validator* 
 * object references. the validators are considered as disposable, in that
 * clearing the map will result in validator destructor calls; however 
 * note that destroy() has no effect on a cached validator - these are 
 * not destroyed until such time as the validator cache is cleared.  
 */
etch_hashtable* new_etchtype_vtormap()
{
    etch_hashtable* map = new_hashtable_synchronized(ETCHTYPE_DEFSIZE_VTORMAP);
    if (NULL == map) return NULL;   
    map->content_type       = ETCHHASHTABLE_CONTENT_OBJECT;
    map->is_tracked_memory  = TRUE;
    map->is_readonly_keys   = TRUE;  /* keys are nondisposable field objects */ 
    map->is_readonly_values = FALSE; /* values are validator function pointers */
    map->freehook = etchtype_vtormap_clear_handler;
    return map;
}

