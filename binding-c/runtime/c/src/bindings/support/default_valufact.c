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
 * default_valufact.c 
 * default value factory from which all others inherit
 */

#include <stdio.h>
#include "etch_global.h"
#include "etch_objects.h"
#include "etchexcp.h"
#include "etchutl.h"

#include "default_valufact.h"

etch_hashtable* new_vf_id_name_map(const int initialsize);


/**
 * def_vf_impl_destructor
 * destructor for def_vf_impl
 */
int def_vf_impl_destructor(def_vf_impl* impl)  
{
    impl_destroy_handler destroy = NULL; /* TODO ifdef sanity checks */
    int result = verify_object((objmask*)impl, ETCHTYPEB_VALUEFACTIMP, CLASSID_DEF_VF_IMPL, NULL);
    if (result == -1) return -1; /* specified object was not expected object */

    /* invoke base class destructor */
    destroy_instancedata(impl->base_impl, impl->base_type); 

    /* destroy this class' instance data */
    //impl->mf_message_id_obj ->vtab->destroy(impl->mf_message_id_obj);
    //impl->mf_in_reply_to_obj->vtab->destroy(impl->mf_in_reply_to_obj);

    destroy_hashtable(impl->types,  TRUE, TRUE);
    destroy_hashtable(impl->fields, TRUE, TRUE);

    /* destroy this object */
    etch_free(impl); 
    return 0;
}


/**
 * new_def_vf_impl()
 * constructor for default vf instance data
 */
def_vf_impl* new_def_vf_impl(void* base_impl, const short base_type)
{
    def_vf_impl* data = etch_malloc(sizeof(def_vf_impl), ETCHTYPEB_INSTANCEDATA);
    memset(data, 0, sizeof(def_vf_impl));
    data->obj_type   = ETCHTYPEB_VALUEFACTIMP;
    data->class_id   = CLASSID_DEF_VF_IMPL;
    data->base_impl  = base_impl;
    data->base_type  = base_type;
    data->destructor = def_vf_impl_destructor;

    data->types  = new_vf_id_name_map(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    data->fields = new_vf_id_name_map(ETCH_DEFVF_IDNMAP_DEFINITSIZE);

    //data->mf_message_id_obj  = new_etch_field(L"_messageId"); 
    //data->mf_in_reply_to_obj = new_etch_field(L"_inReplyTo");  
    //data->mf_message_id  = data->mf_message_id_obj->value_ptr_to;
    //data->mf_in_reply_to = data->mf_in_reply_to_obj->value_ptr_to;    

    return data;
}


/**
 * defvf_add_type()
 * adds a type to set of types
 * @param type caller must supply a disposable type object
 * @return the argument. If there is a collision with an id and name, 
 * the existing type is returned in place of the supplied type. 
 * @throws IllegalArgumentException (NULL return indicates as such)
 */
etch_type* defvf_add_type (etch_value_factory* vf, etch_type* type) 
{
    def_vf_impl* data = vf->impl; /* TODO unit test the defvf */
    
    const int result = data->types->vtab->insert
        (data->types->realtable, type, HASHSIZE_TYPE, NULL, 0, 0, 0); 

    return result == 0? type: NULL;  
}


etch_type* defvf_get_type_by_id (etch_value_factory* vf, const unsigned id)
{
    etch_hashitem  hashbucket; 
    etch_hashitem* thisitem = &hashbucket;
    def_vf_impl* data = vf->impl;  
    
    const int result = data->types->vtab->findh
        (data->types->realtable, id, 0, &thisitem);     

    return result == 0? (etch_type*) thisitem->key: NULL;  
}


etch_type* defvf_get_type_by_name (etch_value_factory* vf, const wchar_t* name) 
{
    def_vf_impl* data = vf->impl; 
    unsigned bytelen  =(unsigned)(wcslen(name) + 1) * sizeof(wchar_t);
    unsigned namehash = etchhash(name, bytelen, 0); 

    return defvf_get_type_by_id(vf, namehash);  
}
	

etch_arraylist* defvf_get_types (etch_value_factory* vf)
{    
    def_vf_impl* data = vf->impl; 

    return get_map_keys(data->types);
}


/**
 * defvf_add_field()
 * adds a type to set of types
 * @param type caller must supply a disposable field object
 * @return the argument. If there is a collision with an id and name, 
 * the existing field is returned in place of the supplied field. 
 * @throws IllegalArgumentException (NULL return indicates as such)
 */  
etch_field* defvf_add_field (etch_value_factory* vf, etch_field* field) 
{
    def_vf_impl* data = vf->impl; /* TODO unit test the defvf */
    
    int result = data->fields->vtab->insert
        (data->fields->realtable, field, HASHSIZE_FIELD, NULL, 0, 0, 0); 

    return result == 0? field: NULL;  
}


etch_field* defvf_get_field_by_id (etch_value_factory* vf, const unsigned id)
{
    etch_hashitem  hashbucket; 
    etch_hashitem* thisitem = &hashbucket;
    def_vf_impl* data = vf->impl;  
    
    const int result = data->fields->vtab->findh
        (data->fields->realtable, id, 0, &thisitem);     

    return result == 0? (etch_field*) thisitem->key: NULL;
}


etch_field* defvf_get_field_by_name (etch_value_factory* vf, const wchar_t* name)
{
    def_vf_impl* data = vf->impl; 
    unsigned bytelen  =(unsigned)(wcslen(name) + 1) * sizeof(wchar_t);
    unsigned namehash = etchhash(name, bytelen, 0); 

    return defvf_get_field_by_id(vf, namehash);  
}


etch_arraylist* defvf_get_fields (etch_value_factory* vf)
{
    def_vf_impl* data = vf->impl; 

    return get_map_keys(data->fields);
}


wchar_t* defvf_get_string_encoding (etch_value_factory* vf)
{
    const static wchar_t* ETCH_STR_UTF8 = L"utf-8";   
    return clone_wchar(ETCH_STR_UTF8);
}

  
ETCH_STRUCTVALUE* defvf_export_custom_value (etch_value_factory* vf, etchobject* value)
{
    return NULL;
}
	

etchobject* defvf_import_custom_value (etch_value_factory* vf, ETCH_STRUCTVALUE* svobj)
{
    return NULL;
}
	

ETCH_TYPE* defvf_get_custom_struct_type (etch_value_factory* vf, unsigned class_id)
{
    return NULL;
}
	

ETCH_CLASS* defvf_get_custom_type (etch_value_factory* vf, etch_type* t)
{
    return NULL;
}


/**
 * defvf_get_message_id() -- valuefactory.get_message_id() implementation.
 */
ETCH_INT64* defvf_get_message_id (etch_value_factory* vf, ETCH_MESSAGE* msgobj)
{
    ETCH_INT64* idobj  = NULL;
    def_vf_impl* data = vf->impl;

    // idobj = message_get(msgobj, data->mf_message_id);

    return verifyx(idobj, ETCHTYPE_BOXED_INT64, EXCPTYPE_INTERNALERR);
}


/**
 * defvf_set_message_id() -- valuefactory.set_message_id() implementation.
 */
int defvf_set_message_id (etch_value_factory* vf, ETCH_MESSAGE* msgobj, ETCH_INT64* idobj)
{
    def_vf_impl* data = vf->impl;
    // etch_type* keycopy = clone_type(data->mf_message_id);
    etch_type* keycopy = 0; // *******

    /* FYI this copy of the key is put to ETCH_MESSAGE* sent message, and gets   
     * freed in msgobj.destroy() (recall that message owns its memory). 
     * TODO clones should happen at a lower level */
    message_put(msgobj, keycopy, idobj);   

    return is_exception(msgobj)? -1: 0;
}


/**
 * defvf_get_in_reply_to() -- valuefactory.get_in_reply_to() implementation.
 */
ETCH_INT64* defvf_get_in_reply_to (etch_value_factory* vf, ETCH_MESSAGE* msgobj)
{
    ETCH_INT64* idobj  = NULL;
    def_vf_impl* data = vf->impl;

    //idobj = message_get(msgobj, data->mf_in_reply_to);

    return verifyx(idobj, ETCHTYPE_BOXED_INT64, EXCPTYPE_INTERNALERR);
}


/**
 * defvf_set_in_reply_to() -- valuefactory.set_message_id() implementation.
 */
int defvf_set_in_reply_to (etch_value_factory* vf, ETCH_MESSAGE* msgobj, ETCH_INT64* idobj)
{
    def_vf_impl* data = vf->impl;
    //etch_type* keycopy = clone_type(data->mf_in_reply_to);
    etch_type* keycopy = 0; //*************

    /* FYI this copy of the key is put to ETCH_MESSAGE* reply message, and gets  
     * freed in newobj.destroy(), since recall that message owns its memory. 
     * TODO clones should happen at a lower level */
    message_put(msgobj, keycopy, idobj);  
    return is_exception(msgobj)? -1: 0;
}

/*  
 * value factory constructors/destructors
 */



/**
 * new_default_value_factory()
 * constructor for value factory implementation  
 */
ETCH_VALUEFACTORY* new_default_value_factory()  
{
    ETCH_VALUEFACTORY*  boxvf  = NULL;
    etch_value_factory* defvf = NULL;
    i_value_factory* vtab = NULL;
    const int VTABSIZE = sizeof(i_value_factory);
    const int VTAB_KEY = ETCHTYPE_VTABLE_DEF_VF;
    int result = 0;
   
    boxvf  = new_boxed_value_factory();
   
    defvf = boxvf->value_ptr_to;

    vtab = cache_find(VTAB_KEY,0);

    if(!vtab)  
    {    
        vtab = new_vtable(defvf->vtab, VTABSIZE, ETCHTYPEB_VTABLE, CLASSID_DEF_VF_VTAB);

        /* override four i_value_factory methods */

        vtab->add_field             = defvf_add_field;
        vtab->add_type              = defvf_add_type;
        vtab->export_custom_value   = defvf_export_custom_value;
        vtab->get_custom_type       = defvf_get_custom_type;
        vtab->get_field_by_id       = defvf_get_field_by_id;
        vtab->get_field_by_name     = defvf_get_field_by_name;
        vtab->get_fields            = defvf_get_fields;
        vtab->get_in_reply_to       = defvf_get_in_reply_to;
        vtab->get_message_id        = defvf_get_message_id;
        vtab->get_string_encoding   = defvf_get_string_encoding;
        vtab->get_type_by_id        = defvf_get_type_by_id;
        vtab->get_type_by_name      = defvf_get_type_by_name;
        vtab->get_types             = defvf_get_types;
        vtab->import_custom_value   = defvf_import_custom_value;
        vtab->set_in_reply_to       = defvf_set_in_reply_to;
        vtab->set_message_id        = defvf_set_message_id;
        vtab->get_custom_struct_type = defvf_get_custom_struct_type;

        vtab->vtab = defvf->vtab;       /* chain parent vtab to override vtab */
        cache_insert(VTAB_KEY, vtab, 0); /* cache the new vtab */
    } 

    defvf->vtab = vtab;  /* set override vtab */

    defvf->impl = new_def_vf_impl(NULL, 0); /* create vf instance data */

    return boxvf;
}


/**
 * new_vf_id_name_map()
 * return a hashtable configured as expected for a vf  
 */
etch_hashtable* new_vf_id_name_map(const int initialsize)
{
    etch_hashtable* map = new_etch_hashtable(initialsize);  
    if (map == NULL) return NULL;
    map->content_type       = ETCHHASHTABLE_CONTENT_OBJECT;
    map->is_tracked_memory  = TRUE;
    map->is_readonly_keys   = FALSE;
    map->is_readonly_values = FALSE;
    map->callback           = NULL;
    return map;
}



