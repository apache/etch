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
 * etch_tagdata_inp.c -- tagged_data_input implementation.
 */

#include "etch_tagdata_inp.h"
#include "etch_global.h"

/**
 * tdi_start_message()
 */
etch_message* tdi_start_message(tagged_data_input* tdi)
{
    return NULL;
}


etch_message* tdi_read_message(tagged_data_input* tdi, etch_flexbuffer* f)
{
    return NULL;
}


/**
 * tdi_end_message()
 */
int tdi_end_message(tagged_data_input* tdi, etch_message* msg)
{
    return NULL;
}


/**
 * tdi_start_struct()
 */
etch_structvalue* tdi_start_struct(tagged_data_input* tdi)
{
    return NULL;
}


/**
 * tdi_read_struct()
 */
etch_structvalue* tdi_read_struct(tagged_data_input* tdi)
{
    return NULL;
}


/**
 * tdi_end_struct()
 * ends a struct currently being read
 */
int tdi_end_struct(tagged_data_input* tdi, etch_structvalue* sv)
{
    return 0;
}


/**
 * tdi_start_array()
 * starts reading an array from the stream 
 */
etch_arrayvalue* tdi_start_array(tagged_data_input* tdi)
{
    return NULL;
}


etch_arrayvalue* tdi_read_array(tagged_data_input* tdi, etch_validator* v)
{
    return NULL;
}


/**
 * tdi_end_array()
 */
int tdi_end_array(tagged_data_input* tdi, etch_arrayvalue* x)
{
    return 0;
}


/**
 * destroy_tagged_data_input()
 */
int destroy_tagged_data_input(tagged_data_input* tdi)
{
    if (tdi->refcount > 0 && --tdi->refcount > 0) return -1;  

    if (!is_etchobj_static_content(tdi))
        if (tdi->impl)
            tdi->impl->destroy(tdi->impl);

    destroy_objectex((objmask*)tdi);
    return 0;
}


/**
 * clone_tagged_data_input()
 */
tagged_data_input* clone_tagged_data_input(tagged_data_input* tdi)
{
    tagged_data_input* newtdi = (tagged_data_input*) clone_object((objmask*) tdi);

    if (tdi->impl)
        newtdi->impl = tdi->impl->clone(tdi->impl);

    return newtdi;
}


/**
 * new_tdi_vtable()
 */
i_tagged_data_input* new_tdi_vtable()
{
    i_tagged_data_input* vtab 
        = new_vtable(NULL, sizeof(i_tagged_data_input), CLASSID_TDI_VTAB);

    vtab->start_message = tdi_start_message;
    vtab->read_message  = tdi_read_message;
    vtab->end_message   = tdi_end_message;
    vtab->start_struct  = tdi_start_struct;
    vtab->read_struct   = tdi_read_struct;
    vtab->end_struct    = tdi_end_struct;
    vtab->start_array   = tdi_start_array;
    vtab->read_array    = tdi_read_array;
    vtab->end_array     = tdi_end_array;
    return vtab;
}


/**
 * new_tagged_data_input()
 * tagged_data_input constructor 
 */
tagged_data_input* new_tagged_data_input(const short objtype, const short class_id)
{
    i_tagged_data_input* vtab = NULL;

    tagged_data_input* tdi = (tagged_data_input*) new_object
        (sizeof(tagged_data_input), ETCHTYPEB_TAGDATAINP, CLASSID_TAGDATAINP);

    tdi->destroy = destroy_tagged_data_input;
    tdi->clone   = clone_tagged_data_input;

    vtab = cache_find(get_vtable_cachehkey(CLASSID_TDI_VTAB), 0);

    if(!vtab)  
    {    
        vtab = new_tdi_vtable();
        cache_insert(vtab->hashkey, vtab, FALSE);
    } 
 
    tdi->vtab = vtab;   
    return tdi; 
}


