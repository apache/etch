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
 * etch_sourceint.c
 * source interface
 */

#include "etch_sourceint.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"

/*
static const char* LOG_CATEGORY = "etch_source_intf";
*/

void* etch_source_def_get_handler (void* thisx);
void  etch_source_def_set_handler (void* thisx, void* handler);
int destroy_i_source(void*);


/**
 * new_default_source_interface
 * return a source interface populated with defaults for virtuals.
 * this interface *is* an etch object, use destroy()
 */
i_source* new_default_source_interface()
{
    i_source* newi = (i_source*) new_object(sizeof(i_source), ETCHTYPEB_SOURCE, CLASSID_SOURCE);

    ((etch_object*)newi)->destroy     = destroy_i_source;
    newi->get_handler = etch_source_def_get_handler;
    newi->set_handler = etch_source_def_set_handler;
    newi->itransport  = new_default_transport_interface();
    return newi;    
}


/**
 * new_source_interface
 * return a source interface populated with specified virtuals.
 * this interface object *is* an etch object, use destroy()
 */
i_source* new_source_interface(void* thisx,
    etch_source_get_handler gh, etch_source_set_handler sh, i_transport* xp) 
{
    i_source* newi = (i_source*) new_object(sizeof(i_source), 
        ETCHTYPEB_SOURCE, CLASSID_SOURCE);

    ((etch_object*)newi)->destroy     = destroy_i_source;
    newi->get_handler = gh? gh: etch_source_def_get_handler;
    newi->set_handler = sh? sh: etch_source_def_set_handler;
    newi->itransport  = xp? xp: new_default_transport_interface();
    newi->thisx = thisx;
    return newi;    
}


/**
 * destroy_i_source
 * i_source destructor
 */
int destroy_i_source(void* data)
{
    i_source* thisx = (i_source*)data;
    if (NULL == thisx) return -1;

    if (!is_etchobj_static_content(thisx))
    {   etch_free(thisx->itransport);
    }
            
    return destroy_objectex((etch_object*)thisx);      
}


void* etch_source_def_get_handler (void* thisx) 
{
    return NULL;
}


void  etch_source_def_set_handler (void* thisx, void* handler) 
{ 
   
}

