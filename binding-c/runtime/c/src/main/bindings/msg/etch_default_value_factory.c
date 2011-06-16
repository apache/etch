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
 * etch_defvalufact.c 
 * default value factory from which all others inherit
 */

#include "etch_default_value_factory.h"
#include "etch_serializer.h"
#include "etch_map.h"
#include "etch_log.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include <wchar.h>

char* ETCHVALF = "VALF";

void etchvf_instantiate_builtins();

etch_type*    defvf_add_type (void*, etch_type*);
etch_id_name* get_idname_by_id (etch_hashtable* map, const unsigned id); 
etch_type*    defvf_get_type_by_id (void*,  const unsigned id);
etch_type*    defvf_get_type_by_name(void*, const wchar_t* name); 
etch_type*    defvf_get_type_by_name(void*, const wchar_t* name); 
wchar_t*      defvf_get_string_encoding (void*);
etch_type*    defvf_get_custom_struct_type (void*, const unsigned);
etch_int64*   defvf_get_message_id  (void*, etch_message*);
int           defvf_set_message_id  (void*, etch_message*, etch_int64* id);
etch_int64*   defvf_get_in_reply_to (void*, etch_message*);
int           defvf_set_in_reply_to (void*, etch_message*, etch_int64* id);
etch_type*    defvf_get_mt_exception(void*);
etch_type*    defvf_get_mt_rutime_exception(void*);
etch_type*    defvf_get_mt_auth_exception(void*);
etch_object*      defvf_import_custom_value (void*, etch_structvalue*);
etch_structvalue* defvf_export_custom_value (void*, etch_object* value);
etch_arraylist* new_vf_mixin_collection(void*);
etch_arraylist* defvf_get_types (void*);


/* 
 * built-in (etch-global, quasi-static) objects.
 * these singleton objects are global to all vfs, instantiated with the 
 * initial vf, and destroyed at etch teardown.
 */
defvf_statics  builtins;
unsigned char  is_builtins_instantiated;

const wchar_t* str_etch_runtime_exception;
const wchar_t* str_etch_auth_exception;        
const wchar_t* str_exception; 
const wchar_t* str_etch_list; 
const wchar_t* str_etch_map;
const wchar_t* str_etch_set;
const wchar_t* str_etch_datetime; 

const wchar_t* str_msg;
const wchar_t* str_message_id; 
const wchar_t* str_in_reply_to; 
const wchar_t* str_result; 

const wchar_t* str_utf8; 
const wchar_t* str_keys; 
const wchar_t* str_values;  
const wchar_t* str_date_time;
const wchar_t* str_keys_values;

const wchar_t* str_msgizervf;
const wchar_t* str_msgizerfmt;

/* - - - - - - - - - - - - - - - - - - - -
 * constructors/destructors
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * new_default_value_factory()
 * constructor for default_value_factory
 * @param typemap the types map. if caller supplies this object, caller
 * retains ownership, unless vf.is_own_types flag is subsequently set false.
 * @param c2tmap the class to type map. if caller supplies this object, caller
 * retains ownership, unless vf.is_own_class_to_type flag is subsequently set false.
 */
default_value_factory* new_default_value_factory(vf_idname_map* typemap, class_to_type_map* c2tmap)
{
    return new_default_value_factory_ex(sizeof(default_value_factory), typemap, c2tmap);
}

/**
 * destroy_default_value_factory()
 * destructor for default_value_factory
 */
int destroy_default_value_factory(void* data)  
{   
    default_value_factory* vf = (default_value_factory*)data;

    if (!is_etchobj_static_content(vf))
    {   
        etch_object_destroy(vf->mixins);
        etch_object_destroy(vf->impl);
    }

   return destroy_objectex((etch_object*) vf);
}

/**
 * new_default_value_factory_ex()
 * constructor for default_value_factory
 * @param objsize the size of the value factory
 * @param typemap the types map. if caller supplies this object, caller
 * retains ownership, unless vf.is_own_types flag is subsequently set false.
 * @param c2tmap the class to type map. if caller supplies this object, caller
 * retains ownership, unless vf.is_own_class_to_type flag is subsequently set false.
 */
default_value_factory* new_default_value_factory_ex(const int objsize, vf_idname_map* typemap, class_to_type_map* c2tmap)
{
    i_value_factory*        vtab    = NULL;
    default_value_factory*  newvf   = NULL;

    ETCH_ASSERT("typemap==NULL" && typemap);
    ETCH_ASSERT("c2tmap==NULL" && c2tmap);
    
    newvf = (default_value_factory*) new_value_factory(objsize);
    ((etch_object*)newvf)->destroy = destroy_default_value_factory;

    vtab = (i_value_factory*)((etch_object*)newvf)->vtab;
    vtab->add_type              = defvf_add_type;
    vtab->export_custom_value   = defvf_export_custom_value;
    vtab->get_custom_struct_type= defvf_get_custom_struct_type;
    vtab->get_in_reply_to       = defvf_get_in_reply_to;
    vtab->get_message_id        = defvf_get_message_id;
    vtab->get_string_encoding   = defvf_get_string_encoding;
    vtab->get_type_by_id        = defvf_get_type_by_id;
    vtab->get_type_by_name      = defvf_get_type_by_name;
    vtab->get_types             = defvf_get_types;
    vtab->import_custom_value   = defvf_import_custom_value;
    vtab->set_in_reply_to       = defvf_set_in_reply_to;
    vtab->set_message_id        = defvf_set_message_id;

    newvf->mixins = new_vf_mixin_collection(newvf);
    newvf->types = typemap;
    newvf->class_to_type = c2tmap;
    return newvf;
}



int
defvf_initialize_static(vf_idname_map* typemap, class_to_type_map* c2tmap){
    int result = 0;
    
    struct i_hashtable* vtab =((struct i_hashtable*)((etch_object*)typemap)->vtab);

    etchvf_instantiate_builtins();
    
    vtab->inserth
        (typemap->realtable, builtins._mt__etch_runtime_exception, NULL,typemap, 0); 
    vtab->inserth
        (typemap->realtable, builtins._mt__etch_auth_exception, NULL,typemap, 0); 
    vtab->inserth
        (typemap->realtable, builtins._mt__exception, NULL,typemap, 0); 
    vtab->inserth
        (typemap->realtable, builtins._mt__etch_list, NULL,typemap, 0);
    vtab->inserth
        (typemap->realtable, builtins._mt__etch_map, NULL,typemap, 0);
    vtab->inserth
        (typemap->realtable, builtins._mt__etch_set, NULL,typemap, 0);
    vtab->inserth
        (typemap->realtable, builtins._mt__etch_datetime, NULL,typemap, 0);

    etch_serializer_exception_init(builtins._mt__exception, c2tmap);
    etch_serializer_authxcp_init  (builtins._mt__etch_auth_exception, c2tmap);
    etch_serializer_rtxcp_init    (builtins._mt__etch_runtime_exception, c2tmap);
    etch_serializer_list_init     (builtins._mt__etch_list,c2tmap);
    etch_serializer_map_init      (builtins._mt__etch_map, c2tmap);
    etch_serializer_set_init      (builtins._mt__etch_set, c2tmap);
    etch_serializer_date_init     (builtins._mt__etch_datetime, c2tmap);
    return result;
}


/* - - - - - - - - - - - - - - - - - - - -
 * vf class methods
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * defvf_add_type()
 * adds a type to set of types
 * @param type a non-disposable etch_type object
 * @return the effective type. if there was a name collision, the existing type
 * is returned in place of the supplied type, AND the supplied type is destroyed.
 * this simplifies logic up the line, and is consistent with caller expecting to
 * relinquish memory management of the type passed.
 */
etch_type* defvf_add_type (void* data, etch_type* type) 
{
  default_value_factory* vf = (default_value_factory*)data;
    etch_type *effective_type = type;

    const int result = ((struct i_hashtable*)((etch_object*)vf->types)->vtab)->inserth
        (vf->types->realtable, type, NULL, vf->types, 0); 

    if (-1 == result)
        effective_type = defvf_get_type_by_name(vf, type->name);

    if (effective_type != type && NULL != effective_type)
        destroy_static_type(type);

    if(effective_type == NULL)
    {
        ETCH_LOG("etch_default_value_factory", ETCH_LOG_ERROR, "error adding type '%s'\n", type->aname);
    }
    return effective_type;   
}


/**
 * get_idname_by_id()
 * given a id_name map and a numeric id, return the id_name for that key. 
 * @return a non-disposable *reference*, not a copy, or null if not found.
 */
etch_id_name* get_idname_by_id (etch_hashtable* map, const unsigned id)
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
 * defvf_get_type_by_id()
 * @return a non-disposable reference to the type matching the supplied id,
 * or null if no match. all mixed-in vfs are recursed for a match.
 */
etch_type* defvf_get_type_by_id (void* data, const unsigned id)
{
  default_value_factory* vf = (default_value_factory*)data;
    etch_type* type = get_idname_by_id(vf->types, id);
    if (type) return type;

    if (vf->mixins->count) 
    {
        etch_iterator iterator; 
        set_iterator(&iterator, vf->mixins, &vf->mixins->iterable);
 
        while(iterator.has_next(&iterator))
        {
            default_value_factory* mvf 
                = (default_value_factory*) iterator.current_value;

            if (is_etch_valuefact(mvf) && (mvf != vf))
            {
                type = defvf_get_type_by_id(mvf, id);
                if (type) return type; 
            }

            iterator.next(&iterator);
        }
    }

    return NULL;
}


/**
 * get_idname_by_name()
 * given a id_name map and a name string, return the id_name for that name. 
 * @return a non-disposable *reference*, not a copy, or null if not found.
 * recall that etch_type and etch_field each are typedefs of etch_id_name. 
 */
etch_id_name* get_idname_by_name(etch_hashtable* map, const wchar_t* name)
{
    etch_hashitem  hashbucket, *thisitem = &hashbucket; 
    const unsigned hash = etch_get_wchar_hashkey(name);
  
    const int result = ((struct i_hashtable*)((etch_object*)map)->vtab)->findh(map->realtable, hash, map, (void**)&thisitem); 
 
    return result == 0? (etch_id_name*) thisitem->key: NULL;
}


/**
 * etchtypemap_get_by_name()
 * given a type map and a name, looks up the type matching that name, 
 * adding a new type to the map if no match was found.
 * @return a non-disposable reference to the type matching the supplied  
 * name, or null if no match.  
 */
etch_type* etchtypemap_get_by_name(etch_hashtable* map, const wchar_t* name)
{
    etch_type* thistype = get_idname_by_name(map, name);

    if (NULL == thistype) 
    {
        /* TODO handle return value */
        ((struct i_hashtable*)((etch_object*)map)->vtab)->inserth(map->realtable, thistype = new_type(name), NULL, map, 0);
    }
       
    return thistype;
}


/**
 * defvf_get_type_by_name()
 * searches vf and all mixed-in vfs for type matching specified name.
 * @return a non-disposable reference to the type corresponding to 
 * specified name, or null if no match.
 */
etch_type* defvf_get_type_by_name (void* data, const wchar_t* name) 
{
  default_value_factory* vf = (default_value_factory*)data;
    etch_type* type = get_idname_by_name(vf->types, name);
    if (type) return type;

    if (vf->mixins->count) 
    {
        etch_iterator iterator; 
        set_iterator(&iterator, vf->mixins, &vf->mixins->iterable);
 
        while(iterator.has_next(&iterator))
        {
            default_value_factory* mvf 
                = (default_value_factory*) iterator.current_value;

            if (is_etch_valuefact(mvf) && (mvf != vf)) 
            {
                type = defvf_get_type_by_name(mvf, name);
                if (type) return type; 
            }

            iterator.next(&iterator);
        }
    }

    return NULL;
}

	
/**
 * defvf_get_types()
 * @return a disposable arraylist of non-disposable references to all types 
 * resident in this vf, and in all its mixed-in vfs. caller should invoke 
 * the returned list's destructor, which will destroy the list shell.
 */
etch_arraylist* defvf_get_types (void* data)
{    
  default_value_factory* vf = (default_value_factory*)data;
    etch_arraylist* typeslist = get_map_keys(vf->types);

    if (vf->mixins->count) 
    {
        int  newmixintypes = 0;
        etch_iterator iterator; 
        set_iterator(&iterator, vf->mixins, &vf->mixins->iterable);
 
        while(iterator.has_next(&iterator))
        {
            etch_arraylist* mvflist;
 
            default_value_factory* mvf 
                = (default_value_factory*) iterator.current_value;

            if (is_etch_valuefact(mvf) && (mvf != vf)) 
            {
                mvflist = get_map_keys(mvf->types);

                newmixintypes += etch_arraylist_add_from(typeslist, mvflist, etchtypelist_comparator); 

                etch_object_destroy(mvflist);
            }

            iterator.next(&iterator);
        }
    }

    return typeslist; /* caller must dispose */
}


/**
 * defvf_get_string_encoding()
 * @returns a non-disposable reference to the encoding type
 */
wchar_t* defvf_get_string_encoding (void* data)
{
    return (wchar_t*) str_utf8;
}


/**
 * get_etch_string_encoding()
 * return etch code indicating current string encoding
 */
int get_etch_string_encoding(etch_value_factory* vf)
{
    wchar_t* encoding = ((struct i_value_factory*)((etch_object*)vf)->vtab)->get_string_encoding(vf);
    if (0 == wcscmp(encoding, L"utf-8"))  return ETCH_ENCODING_UTF8; 
    if (0 == wcscmp(encoding, L"utf-16")) return ETCH_ENCODING_UTF16;
    return ETCH_ENCODING_ASCII;
}


/**
 * defvf_get_custom_struct_type()
 * @return a non-disposable reference to the etch type type corresponding   
 * to the specified class. invoked recursively via mixed in vfs. 
 */
etch_type* defvf_get_custom_struct_type(void* data, const unsigned etchclass)
{
  default_value_factory* vf = (default_value_factory*)data;
    etch_type* type = (etch_type*) etchmap_find(vf->class_to_type, etchclass, 0);
    if (type) return type;

    if (vf->mixins->count) 
    {
        etch_iterator iterator; 
        set_iterator(&iterator, vf->mixins, &vf->mixins->iterable);
 
        while(iterator.has_next(&iterator))
        {
            default_value_factory* mvf 
                = (default_value_factory*) iterator.current_value;
                  
            if (is_etch_valuefact(mvf) && (mvf != vf)) /* ensure no cycle */
            {
                type = ((struct i_value_factory*)((etch_object*)mvf)->vtab)->get_custom_struct_type(mvf, etchclass);
                if (type) break;
            }

            iterator.next(&iterator);
        }
    }

    return type;
}
	

/**
 * defvf_import_custom_value()
 * get helper method from the struct's type to import the custom value associated
 * with the type. if found, invoke the method to create the custom object.
 * @param sv the *disposable* raw key-value pairs read in by tdi,
 * from which to reconstruct the expected custom value object.
 * caller relinquishes ownership of this object regardless of outcome.
 * @return the *disposable* custom object, which must be cast by caller, or null.
 */
etch_object* defvf_import_custom_value (void* data, etch_structvalue* sv)
{  
    etch_object* custom_value = NULL;

    etch_serializer* impxhelper = etchtype_get_impexphelper(sv->struct_type);

    if (impxhelper)
        custom_value = impxhelper->import_value(impxhelper, (etch_object*) sv);
     
    etch_object_destroy(sv);

    return custom_value;
}


/**
 * defvf_export_custom_value()
 * establishes the etch type of the custom struct to be exported to, gets the
 * export helper method from the type, and invokes that method to do the export.
 * @param value the custom value object to be exported to a struct for tdo.
 * caller retains ownership of this object.
 * @return the exported struct, or null. caller owns the returned struct.
 */
etch_structvalue* defvf_export_custom_value(void* data, etch_object* value)
{
  default_value_factory* vf = (default_value_factory*)data;
    etch_type* custom_type = NULL;
    etch_serializer*  impxhelper = NULL; 
    etch_structvalue* exported_value = NULL;
    if (NULL == value) return NULL;

    custom_type = defvf_get_custom_struct_type(vf, ETCHOBJCLASS(value));

    if (NULL == custom_type)
    {
        switch(((etch_object*)value)->obj_type)
        { 
          case ETCHTYPEB_STRUCTVAL:
               custom_type = ((etch_structvalue*)value)->struct_type;
               break;
          case ETCHTYPEB_EXCEPTION:
               custom_type = builtins._mt__etch_runtime_exception;
               break;
          case ETCHTYPEB_ETCHLIST:
               custom_type = builtins._mt__etch_list;
               break;
          case ETCHTYPEB_ETCHMAP:
               custom_type = builtins._mt__etch_map;
               break;
          case ETCHTYPEB_ETCHSET:
               custom_type = builtins._mt__etch_set;
               break;
        }
    }

    /* fetch non-disposable helper object from custom type */
    impxhelper = etchtype_get_impexphelper(custom_type);

    if (impxhelper && value)
    {   /* export value object to struct */
        exported_value = (etch_structvalue*) 
            impxhelper->export_value(impxhelper, value);

        /* changed to not destroy value 6/23, since the tdo itself does not own 
         * the value, and the export custom value is invoked from tdo write_value. */
        /* value->destroy(value); */
    }

    return exported_value;
}


/**
 * defvf_add_mixin()
 * add a mixed in value factory to this value factory.
 * @return 0 or -1.
 */
int defvf_add_mixin(default_value_factory* vf, etch_value_factory* mixedin_vf)
{
    return etch_arraylist_add(vf->mixins, mixedin_vf);
}


/**
 * defvf_get_message_id()
 * valuefactory.get_message_id() implementation.
 * @return a non-disposable reference to the etch_int64 id object, or null. 
 */
etch_int64* defvf_get_message_id (void* data, etch_message* msg)
{
    return (etch_int64*) etch_message_get(msg, builtins._mf__message_id);
}


/**
 * message_get_id32()
 * get and return message id in 32 bits. used for debugging, logging, etc.
 */
unsigned message_get_id32 (etch_message* msg)
{
    etch_int64* id64 = (etch_int64*) etch_message_get (msg, builtins._mf__message_id);
    const int id32 = id64? (unsigned) id64->value: 0;
    return id32;
}


/**
 * defvf_set_message_id()
 * valuefactory.set_message_id() implementation.
 * @param id a *disposable* etch_int64* wrapping the message id. 
 * regardless of outcome, caller relinquishes ownership of this object.
 * @return 0 or -1.
 */
int defvf_set_message_id (void* data, etch_message* msg, etch_int64* id)
{
    return etch_message_put(msg, clone_field(builtins._mf__message_id), (etch_object*) id);
}


/**
 * defvf_get_in_reply_to() 
 * gets the id of the message to which this message is a reply.
 * returns a non-disposable reference to an etch_int64*, not a copy, or null.
 */
etch_int64* defvf_get_in_reply_to (void* data, etch_message* msg)
{
    return (etch_int64*) etch_message_get(msg, builtins._mf__in_reply_to);
}


/**
 * defvf_set_in_reply_to() 
 * sets the id of the message to which this message is a reply.
 * @param id a *disposable* etch_int64* wrapping the message id.  
 * regardless of outcome, caller relinquishes ownership of this object.
 * @return 0 or -1.
 */
int defvf_set_in_reply_to (void* data, etch_message* msg, etch_int64* id)
{
    return etch_message_put (msg, clone_field(builtins._mf__in_reply_to), (etch_object*) id);
}


/* - - - - - - - - - - - - - - - - - - - -
 * vf types collection
 * - - - - - - - - - - - - - - - - - - - -
 */

/* 
 * defvf_typescollection_clear_handler()
 * callback set to handle freeing of key memory during a clear() of the vf
 * types collection. such a map owns its content, which consists of both builtin
 * types, which are marked static, and of user types, which are generally not
 * marked static. the type's destructor is invoked here; however the destructor
 * will take no action on the static types (these being freed later when the 
 * builtins are destroyed. those types which are added as a result of a runtime
 * get() call, e.g., etchtypemap_get_by_name(vf->types, L"my_newtype"),
 * are not marked static and so will be destroyed with this map.
 */
int defvf_typescollection_clear_handler (void* data1, void* data2)
{
    #if(0)
    etch_type* type = (etch_type*) key;
    if  (is_etchobj_static_shell(type))     
         wprintf(L"vf types map NOT destroying %s\n", type->name);
    else wprintf(L"vf types map destroying %s\n", type->name);
    fflush(stdout);  
    #endif

    //key->destroy(key); /* see comments above re static and nonstatic types */
    return TRUE; 
}
 

/**
 * new_vf_types_collection()
 * return a hashtable configured as expected for a set of types 
 */
etch_set* new_vf_types_collection(const int initialsize)
{
    etch_set* map = new_set(initialsize);  
    map->content_obj_type = ETCHTYPEB_TYPE;
    map->content_class_id = CLASSID_ID_TYPE;  
    map->freehook = defvf_typescollection_clear_handler;
    return map;
}


/* - - - - - - - - - - - - - - - - - - - -
 * vf_idname_map
 * - - - - - - - - - - - - - - - - - - - -
 */

/* 
 * defvf_idnmap_clear_handler()
 * callback set to handle freeing of key memory during a clear() of a vf
 * id_name map. map keys are etch_field owned by the map. however the map does
 * not know how to destroy such a key so we handle that here. values in this
 * map are etch_type owned elsewhere (presumably quasi-static built-in types).
 */
int defvf_idnmap_clear_handler (void* data1, void* data2)
{
  etch_object* key = (etch_object*)data1;
    #ifdef ETCH_DEBUG_VF
    etch_id_name* idn = (etch_id_name*) key;
    wprintf(L"destroy idn %08x '%s'\n", (size_t) (void*) key, idn->name);
    #endif
 
    etch_object_destroy(key);
    return TRUE; 
}
 

/**
 * new_vf_idname_map()
 * return a hashtable configured as expected for a vf 
 * todo lose this method and the clear handler we don't use it 
 */
etch_hashtable* new_vf_idname_map(const int initialsize)
{
    etch_hashtable* map = new_hashtable(initialsize);  
    map->is_tracked_memory  = TRUE; 
    map->is_readonly_keys   = FALSE; /* keys are etch_field* owned by the map */ 
    map->is_readonly_values = FALSE; /* values are etch_type* owned elsewhere */ 
    map->content_type       = ETCHHASHTABLE_CONTENT_OBJECT;
    map->content_obj_type   = ETCHTYPEB_TYPE;
    map->content_class_id   = CLASSID_ID_TYPE;  
    map->freehook = defvf_idnmap_clear_handler; /* key memory free hook */
    return map;
}


/* - - - - - - - - - - - - - - - - - - - -
 * class_to_type_map
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * class_to_type_map_put()
 * put the specified type to the supplied map, keyed by specified class.
 * the map does not own the specified type object.
 * @return 0 or -1;
 */
int class_to_type_map_put(class_to_type_map* map, const unsigned thisclass, 
    etch_type* type)
{
    int hashvalue = etchmap_insert (map, thisclass, type, TRUE);
    return hashvalue? 0: -1;
}


/**
 * class_to_type_map_get()
 * get the type associated with the specified class from the supplied map.
 * @return a non-disposable reference to the matching type, or null.
 */
etch_type* class_to_type_map_get(class_to_type_map* map, const unsigned thisclass) 
{
    etch_type* foundobj = (etch_type*) etchmap_find (map, thisclass, NULL);
    return foundobj;
}


/**
 * new_class_to_type_map()
 * class_to_type_map constructor.
 * such a map associates a "class", which in this context is a 32-bit unsigned
 * integer whose high 16 bits is the etch object obj_type, and the low 16 bits
 * the etch object class_id, to an etch_type. 
 */
class_to_type_map* new_class_to_type_map(const int initialsize)
{
    /* this map calls no etch destructors when destroyed. the key strings are 
     * freed automatically by the etch_hashtable, and the values are pointers
     * to etch_type objects owned elsewhere. it obviously follows that this 
     * map should be destroyed prior to any such referenced type object. 
     */
    class_to_type_map* map  = new_hashtable(initialsize);  
    map->is_tracked_memory  = TRUE; 
    map->is_readonly_keys   = TRUE; /* keys are strings owned by the hashtable */ 
    map->is_readonly_values = TRUE; /* values are non-disposable etch_type* */ 
    map->content_type       = ETCHHASHTABLE_CONTENT_OBJECT;
    map->content_obj_type   = ETCHTYPEB_TYPE;
    map->content_class_id   = CLASSID_ID_TYPE;  
    return map;
}


/* - - - - - - - - - - - - - - - - - - - - - - -
 * built-in (etch-global, quasi-static) objects
 * - - - - - - - - - - - - - - - - - - - - - - -
 */

/* these objects are global to all vfs.
 * for each such object, a corresponding entry should exist both in 
 * etchvf_instantiate_builtins(), and in etchvf_free_builtins()
 */
const wchar_t* str_etch_runtime_exception = L"_Etch_RuntimeException";   
const wchar_t* str_etch_auth_exception    = L"_Etch_AuthException";         
const wchar_t* str_exception     = L"_exception"; 
const wchar_t* str_etch_list     = L"_Etch_List"; 
const wchar_t* str_etch_map      = L"_Etch_Map";
const wchar_t* str_etch_set      = L"_Etch_Set";
const wchar_t* str_etch_datetime = L"_Etch_Datetime";
 
const wchar_t* str_msg           = L"msg";   
const wchar_t* str_message_id    = L"_messageId";  
const wchar_t* str_in_reply_to   = L"_inReplyTo";  
const wchar_t* str_result        = L"result";
   
const wchar_t* str_utf8          = L"utf-8"; 
const wchar_t* str_keys          = L"keys";    
const wchar_t* str_values        = L"values";  
const wchar_t* str_date_time     = L"dateTime";
const wchar_t* str_keys_values   = L"keysAndValues"; 

const wchar_t* str_msgizervf     = L"Messagizer.valueFactory";
const wchar_t* str_msgizerfmt    = L"Messagizer.format";


/* 
 * etchvf_free_builtins()
 * frees memory for etch-global quasi-static builtin objects,
 * and for the validators cache and its validator content.
 * it should be invoked at etch teardown, after last vf is destroyed.
 * unit tests will show memory leaks unless they invoke this post-test.
 */

void etchvf_free_builtins()
{
    if (is_builtins_instantiated)  
    {
        destroy_static_type(builtins._mt__etch_runtime_exception);
        destroy_static_type(builtins._mt__etch_auth_exception);
        destroy_static_type(builtins._mt__exception);
        destroy_static_type(builtins._mt__etch_list); 
        destroy_static_type(builtins._mt__etch_map); 
        destroy_static_type(builtins._mt__etch_set); 
        destroy_static_type(builtins._mt__etch_datetime); 

        destroy_static_field(builtins._mf_msg);
        destroy_static_field(builtins._mf_result);
        destroy_static_field(builtins._mf__message_id);
        destroy_static_field(builtins._mf__in_reply_to);
    }

    etchvtor_clear_cache();  /* destroy cached validators */

    is_builtins_instantiated = FALSE;
}


/**
 * etchvf_instantiate_builtins()
 * instantiate built-in objects such as default types and fields.
 * these singleton objects are destroyed explicitly by invoking  
 * etchvf_free_builtins().
 * todo: mark these objects immutable and unmark in destructor.
 * todo: determine how we handle case of multiple vfs.
 */
void etchvf_instantiate_builtins()
{  
    if (is_builtins_instantiated) return;

    builtins._mt__etch_runtime_exception = new_static_type(str_etch_runtime_exception);
    builtins._mt__etch_auth_exception = new_static_type(str_etch_auth_exception);
    builtins._mt__exception = new_static_type(str_exception);
    builtins._mt__etch_list = new_static_type(str_etch_list); 
    builtins._mt__etch_map  = new_static_type(str_etch_map); 
    builtins._mt__etch_set  = new_static_type(str_etch_set); 
    builtins._mt__etch_datetime = new_static_type(str_etch_datetime); 

    builtins._mf_msg = new_static_field(str_msg); 
    builtins._mf_result = new_static_field(str_result); 
    builtins._mf__message_id  = new_static_field(str_message_id); 
    builtins._mf__in_reply_to = new_static_field(str_in_reply_to); 

    etchtype_put_validator(builtins._mt__exception, 
        clone_field(builtins._mf_result), (etch_object*) etchvtor_exception_get());
    etchtype_put_validator(builtins._mt__exception, 
        clone_field(builtins._mf__message_id), (etch_object*) etchvtor_int64_get(0));
    etchtype_put_validator(builtins._mt__exception, 
        clone_field(builtins._mf__in_reply_to),(etch_object*) etchvtor_int64_get(0));

    is_builtins_instantiated = TRUE;
}


/* - - - - - - - - - - - - - - - - - - - -
 * other methods
 * - - - - - - - - - - - - - - - - - - - -
 */
	
/**
 * etchtypelist_comparator()
 * etch_comparator comparing two etch_types
 * hook for arraylist_add_from to determine if a type from list b
 * already exists in list a.
 */
int etchtypelist_comparator(void* a, void* b)
{
    return a && b && (((etch_type*)a)->id == ((etch_type*)b)->id);
}


/**
 * new_vf_mixin_collection()
 * instantiate and return a collection configured appropriately for the
 * storage of value factory objects of mixed-in classes.
 */
etch_arraylist* new_vf_mixin_collection(void* data)
{
  default_value_factory* vf = (default_value_factory*)data;
    //TODO: eheck if arraylist have to be synchronized
    etch_arraylist* list = new_etch_arraylist(ETCH_DEVVF_MIXINS_DEFINITSIZE, 0);
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    list->is_readonly  = FALSE; /* list dtor will destroy list content */
    list->content_obj_type = ((etch_object*)vf)->obj_type;
    list->content_class_id = ((etch_object*)vf)->class_id;
    return list;
}
