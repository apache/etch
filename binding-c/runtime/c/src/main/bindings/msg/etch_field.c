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
 * field.c -- methods on the etch_field object.
 * an etch_field is an etch_id_name representing a field of a struct or message 
 * (i.e. a key for a value).
 */
#include "etch_field.h"
#include "etch_objecttypes.h"

/** 
 * an etch_field is for now simply a typedef of etch_id_name.
 * all its methods except ctor are #defined as those of etch_id_name.
 */

etch_field* new_field(const wchar_t* name)
{
    etchparentinfo* inheritlist = NULL;

    etch_field* newobj = (etch_field*) new_id_name(name);
    if (NULL == newobj) return NULL;
    ((etch_object*)newobj)->obj_type = ETCHTYPEB_FIELD;
    ((etch_object*)newobj)->class_id = CLASSID_ID_FIELD;

    /* fetch cached inheritance list, or create if initial instantiation, 
     * and ensure id_name parent keys exist in the (one-based) list */
    inheritlist = get_vtab_inheritance_list((etch_object*)newobj, 2, 1, CLASSID_VTAB_FIELD);
    ETCH_ASSERT(((etch_object*)newobj)->vtab && ((etch_object*)newobj)->vtab->inherits_from);
    inheritlist[1].o.obj_type = ((etch_object*)newobj)->obj_type;
    inheritlist[1].c.class_id = CLASSID_ID_NAME;

    return newobj;
}


/**
 * new__static_field() 
 * create a field object whose destructor will have no effect.
 */
etch_field* new_static_field(const wchar_t* name)
{
    etch_field* newfield = new_field(name);
    set_etchobj_static_all(newfield);
    return newfield;
} 

 
/** 
 * destroy_static_field()
 * etch_field destructor.
 * this should not be set as the virtual dtor for a field, since the field
 * would then not be quasi-static as desired. it should be invoked explicitly
 */
int destroy_static_field(etch_field* field)
{
    clear_etchobj_static_all(field);
    return destroy_field(field); 
} 
