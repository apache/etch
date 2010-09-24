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
 * etch_valufact.c
 * value factory
 */

#include "etch_value_factory.h"
#include "etch_tagdata_out.h"
#include "etch_cache.h"
#include "etch_message.h"
#include "etch_objecttypes.h"

/**
 * value factory virtual function default implementations
 */

etch_type* vf_add_type (void* vf, etch_type* t) 
{
    return NULL;
}


etch_type* vf_get_type_by_id (void* vf, const unsigned id)
{
    return NULL;
}


etch_type* vf_get_type_by_name (void* vf, const wchar_t* name) 
{
    return NULL;
}
	

etch_arraylist* vf_get_types (void* vf)
{
    return NULL;
}


wchar_t* vf_get_string_encoding (void* vf)
{
    return NULL;
}


etch_int64* vf_get_message_id (void* vf, etch_message* msgobj)
{
    return NULL;
}


int vf_set_message_id (void* vf, etch_message* msgobj, etch_int64* msgid)
{
    return 0;
}

  
etch_int64* vf_get_in_reply_to (void* vf, etch_message* msgobj)
{
    return NULL;
}
	

int vf_set_in_reply_to (void* vf, etch_message* msgobj, etch_int64* msgid)
{
    return 0;
}
   
  
etch_structvalue* vf_export_custom_value (void* vf, etch_object* value)
{
    return NULL;
}
	

etch_object* vf_import_custom_value (void* vf, etch_structvalue* svobj)
{
    return NULL;
}


etch_type* vf_get_custom_struct_type (void* vf, const unsigned etchclass)
{
    return NULL;
}
	

etch_type* vf_get_mt_exception (void* vf)
{
    return NULL;
}


int destroy_valuefactory(void* data)
{
    etch_value_factory* vf = (etch_value_factory*)data;

    if (!is_etchobj_static_content(vf)) {
        etch_object_destroy(vf->impl);
    }

    return destroy_objectex((etch_object*)vf);
}

/**
 * new_value_factory()
 * value factory constructor 
 */
etch_value_factory* new_value_factory(const int objlen)
{
    etch_value_factory* vf = NULL;
    i_value_factory*  vtab = NULL;
    const unsigned short CLASS_ID = CLASSID_VALUEFACTORY;
    int objsize = objlen? objlen: sizeof(etch_value_factory);

    vf = (etch_value_factory*) new_object(objsize, ETCHTYPEB_VALUEFACTORY, CLASSID_VALUEFACTORY);
    ((etch_object*)vf)->destroy = destroy_valuefactory;
    ((etch_object*)vf)->clone   = clone_null;

    vtab = etch_cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    {   
        vtab = new_vtable(NULL, sizeof(i_value_factory), CLASS_ID);

        /* default virtual function implementations */
        vtab->add_type              = vf_add_type;
        vtab->export_custom_value   = vf_export_custom_value;
        vtab->get_in_reply_to       = vf_get_in_reply_to;
        vtab->get_message_id        = vf_get_message_id;
        vtab->get_string_encoding   = vf_get_string_encoding;
        vtab->get_type_by_id        = vf_get_type_by_id;
        vtab->get_type_by_name      = vf_get_type_by_name;
        vtab->get_types             = vf_get_types;
        vtab->import_custom_value   = vf_import_custom_value;
        vtab->set_in_reply_to       = vf_set_in_reply_to;
        vtab->set_message_id        = vf_set_message_id;
        vtab->get_custom_struct_type= vf_get_custom_struct_type;
        vtab->get_mt_exception      = vf_get_mt_exception;

        etch_cache_insert(((etch_object*)vtab)->get_hashkey(vtab), vtab, FALSE);
    } 

    ((etch_object*)vf)->vtab = (vtabmask*)vtab;
    return vf; 
}





