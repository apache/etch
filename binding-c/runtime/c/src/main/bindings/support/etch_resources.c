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
 * etch_resources.c
 */

#include "etch_resources.h"
#include "etch_map.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"



const wchar_t* ETCH_RESXKEY_SOCKET          = L"SOCKET";
const wchar_t* ETCH_RESXKEY_ACCEPTED_CONX   = L"ACCEPTED_CONX";
const wchar_t* ETCH_RESXKEY_POOLTYPE_FREE   = L"FREE_POOL";
const wchar_t* ETCH_RESXKEY_POOLTYPE_QUEUED = L"QUEUED_POOL";
const wchar_t* ETCH_RESXKEY_MSGIZER_FORMAT  = L"Messagizer.format";
const wchar_t* ETCH_RESXKEY_MSGIZER_VALUFACT= L"Messagizer.valueFactory";
const wchar_t* ETCH_RESXVAL_XPORTFMT_BINARY = L"binary";;
const wchar_t* ETCH_RESXVAL_XPORTFMT_XML    = L"xml";
const wchar_t* ETCH_XFACTKEY_TCP   = L"tcp";
const wchar_t* ETCH_XPORTKEY_START = L"START";
const wchar_t* ETCH_XPORTKEY_IS_UP = L"IS_UP";
const wchar_t* ETCH_XPORTKEY_STOP  = L"STOP";
const wchar_t* ETCH_XPORTKEY_START_AND_WAIT_UP  = L"START_AND_WAIT_UP";
const wchar_t* ETCH_XPORTKEY_STOP_AND_WAIT_DOWN = L"STOP_AND_WAIT_DOWN";

// TODO - lose the above strings, replacing them with integer constants

const int ETCH_RESKEY_SOCKET           = 0x100;
const int ETCH_RESKEY_POOLTYPE_FREE    = 0x101;
const int ETCH_RESKEY_POOLTYPE_QUEUED  = 0x102;
const int ETCH_RESKEY_MSGIZER_FORMAT   = 0x104;
const int ETCH_RESKEY_MSGIZER_VALUFACT = 0x105;
const int ETCH_RESVAL_XPORTFMT_BINARY  = 0x140;
const int ETCH_RESVAL_XPORTFMT_XML     = 0x141;
const int ETCH_XPKEY_TCP               = 0x142;
const int ETCH_XPKEY_START             = 0x160;
const int ETCH_XPKEY_IS_UP             = 0x161;
const int ETCH_XPKEY_STOP              = 0x162;
const int ETCH_XPKEY_START_AND_WAITUP  = 0x163;
const int ETCH_XPKEY_STOP_AND_WAITDOWN = 0x164;
const int ETCH_XPKEY_XPORTFMT_XML      = 0x165;

// TODO - I don't think we need these at all, we should use class_id
// since in this way the object can be identified and we don't need
// to assume that the enclosing object is an etch_int32. however not
// all of these IDs are objects, some are table IDs
 
const wchar_t* ETCH_RESXKEY_SOCKET;
const wchar_t* ETCH_RESXKEY_ACCEPTED_CONX;
const wchar_t* ETCH_RESXKEY_POOLTYPE_FREE;
const wchar_t* ETCH_RESXKEY_POOLTYPE_QUEUED;
const wchar_t* ETCH_RESXKEY_MSGIZER_FORMAT;
const wchar_t* ETCH_RESXKEY_MSGIZER_VALUFACT;
const wchar_t* ETCH_RESXVAL_XPORTFMT_BINARY;
const wchar_t* ETCH_RESXVAL_XPORTFMT_XML;
const wchar_t* ETCH_XFACTKEY_TCP;
const wchar_t* ETCH_XPORTKEY_START;
const wchar_t* ETCH_XPORTKEY_START_AND_WAIT_UP;
const wchar_t* ETCH_XPORTKEY_IS_UP;
const wchar_t* ETCH_XPORTKEY_STOP;
const wchar_t* ETCH_XPORTKEY_STOP_AND_WAIT_DOWN;


/*
 * new_etch_resources 
 * etch_resources constructor
 */
etch_resources* new_etch_resources(const int initialsize)
{
    etch_resources* resx   = new_hashtable(initialsize);
    resx->content_type     = ETCHHASHTABLE_CONTENT_OBJECT; 
    resx->content_obj_type = ETCHTYPEB_ETCHOBJECT;
    resx->content_class_id = CLASSID_ANY;
    resx->is_readonly_keys = resx->is_readonly_values = FALSE;
    resx->freehook = string_to_object_clear_handler;  /* frees memory on clear */
    return resx;
}


/*
 * etch_resources_add()  
 * adds specified key/value pair to specified resource map
 */
int etch_resources_add (etch_resources* resources, const wchar_t* key, etch_object* val)
{
    const int hashkey = etchmap_insertxw(resources, (wchar_t*) key, val, TRUE);
    return hashkey? 0: -1;   
}


/*
 * etch_resources_get()  
 */
etch_object* etch_resources_get (etch_resources* resources, const wchar_t* key)
{
    etch_object* founditem = etchmap_findxw(resources, (wchar_t*) key, NULL);
    return founditem;
}


/*
 * etch_resources_clear()  
 * clear a resources map, assuming all objects are owned unless marked as a static resource.
 * @return count of items both removed from the map and freed.
 */
int etch_resources_clear (etch_resources* resources)
{         // TODO lose this method or augment it to delete the hashtable entry memory also
          // it leaves the memory the hashtable allocated for the key intact
    int  freedcount = 0;
    etch_iterator iterator;
    wchar_t* thiskey = NULL;
    if (!is_etch_hashtable(resources)) return 0;
    set_iterator(&iterator, resources, &resources->iterable);

    while(iterator.has_next(&iterator)) 
    {
        etch_object* value = iterator.current_value;

        if (value && !is_etchobj_static_resource(value))
        {   
            etch_object_destroy(value);
            freedcount++;
        }

        thiskey = iterator.current_key;
        etch_free(thiskey);

        iterator.next(&iterator);
    }

    return freedcount;
}


/*
 * etch_resources_replace()  
 * adds specified key/value pair to specified resource map, first removing prior item.
 */
int etch_resources_replace (etch_resources* resources, const wchar_t* key, etch_object* val)
{
    if (etch_resources_get (resources, key))
    {
        etch_object* removedobj = etchmap_delxw (resources, (wchar_t*) key);

        if (removedobj)
            if (!is_etchobj_static_resource(removedobj))
                etch_object_destroy(removedobj);
    }

    return etch_resources_add (resources, key, val);
}
