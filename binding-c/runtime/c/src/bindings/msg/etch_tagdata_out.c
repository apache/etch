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
 * etch_tagdata_out.c -- tagged_data_output base class default implementation.
 */

#include <stdio.h>
#include "etch_tagdata_out.h"
#include "etch_global.h"


int tdo_start_message(tagged_data_output* tdo, etch_message* msg)
{
    return -1;
}


int tdo_write_message(tagged_data_output* tdo, etch_message* msg, etch_flexbuffer* fb)
{
    return -1;
}



int tdo_end_message(tagged_data_output* tdo, etch_message* msg)
{
    return NULL;
}


int tdo_start_struct(tagged_data_output* tdo, struct etch_structvalue* sv)
{
    return -1;
}


int tdo_write_struct(tagged_data_output* tdo, struct etch_structvalue* sv)
{
    return -1;
}


int tdo_end_struct(tagged_data_output* tdo, struct etch_structvalue* sv)
{
    return -1;
}


int tdo_start_array(tagged_data_output* tdo, etch_arrayvalue* x)
{
    return -1;
}


int tdo_write_array(tagged_data_output* tdo, etch_arrayvalue* x, etch_validator* v)
{
    return -1;
}


int tdo_end_array(tagged_data_output* tdo, etch_arrayvalue* x)
{
    return -1;
}


/**
 * destroy_tagged_data_output()
 */
int destroy_tagged_data_output(tagged_data_output* tdo)
{
    if (tdo->refcount > 0 && --tdo->refcount > 0) return -1;  

    if (!is_etchobj_static_content(tdo))
        if (tdo->impl)
            tdo->impl->destroy(tdo->impl);

    destroy_objectex((objmask*)tdo);
    return 0;
}


/**
 * clone_tagged_data_output()
 */
tagged_data_output* clone_tagged_data_output(tagged_data_output* tdo)
{
    tagged_data_output* newtdo = (tagged_data_output*) clone_object((objmask*) tdo);

    if (tdo->impl)
        newtdo->impl = tdo->impl->clone(tdo->impl);

    return newtdo;
}


i_tagged_data_output* new_tdo_vtable()
{
    i_tagged_data_output* vtab 
        = new_vtable(NULL, sizeof(i_tagged_data_output), CLASSID_TDO_VTAB);

    vtab->start_message = tdo_start_message;
    vtab->write_message = tdo_write_message;
    vtab->end_message   = tdo_end_message;
    vtab->start_struct  = tdo_start_struct; 
    vtab->write_struct  = tdo_write_struct;   
    vtab->end_struct    = tdo_end_struct;
    vtab->start_array   = tdo_start_array;
    vtab->write_array   = tdo_write_array;
    vtab->end_array     = tdo_end_array;
    return vtab;
}


/**
 * new_tagged_data_output()
 * tagged_data_output constructor 
 */
tagged_data_output* new_tagged_data_output(const short objtype, const short class_id)
{
    i_tagged_data_output* vtab = NULL;

    tagged_data_output* tdo = (tagged_data_output*) new_object
        (sizeof(tagged_data_output), ETCHTYPEB_TAGDATAOUT, CLASSID_TAGDATAOUT);

    tdo->destroy = destroy_tagged_data_output;
    tdo->clone   = clone_tagged_data_output;

    vtab = cache_find(get_vtable_cachehkey(CLASSID_TDO_VTAB), 0);

    if(!vtab)  
    {   
        vtab = new_tdo_vtable();
        cache_insert(vtab->hashkey, vtab, FALSE);
    } 
 
    tdo->vtab = vtab;   
    return tdo; 
}


