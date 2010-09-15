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
 * etch_arrayval.h 
 */

#ifndef ETCHARRAYVAL_H
#define ETCHARRAYVAL_H

#include "etch_msgutl.h"
#include "etch_arraylist.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ETCHARRAYVAL_READONLY TRUE
#define ETCHARRAYVAL_NOTREADONLY FALSE

/**
 * etch_arrayvalue
 * an array of values, where each value is a pointer to an etch object.
 */
typedef struct etch_arrayvalue
{
    etch_object object;
 
    etch_arraylist* list;
    unsigned short content_obj_type;  /* etch obj_type of a native value */
    unsigned short content_class_id;  /* etch class_id of a native value */
    int  dim;

    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    unsigned short content_item_size; /* byte count of a native value */ 
    unsigned char  is_array_owned;    /* arrayvalue owns natarray? */ 
    signed   char  type_code;         /* external content type */

    etch_type* custom_struct_type;     /* not owned */
    struct etch_nativearray* natarray; /* see is_array_owned */
} etch_arrayvalue;

/**
 * new_arrayvalue_arraylist()
 * allocates and returns an arraylist configured appropriately for use as arrayvalue backing store
 */
etch_arraylist* new_arrayvalue_arraylist(const int initsize, const int deltsize, 
    const int is_readonly, const int is_synchronized);

/* public methods on etch_arrayvalue */
etch_arrayvalue* new_arrayvalue(const byte type_code, etch_type* custom_struct_type, 
    const int dim, const int initsize, const int deltsize, 
    const int is_readonly, const int is_synchronized);

etch_arrayvalue* new_arrayvalue_from(struct etch_nativearray* natarray, 
    const signed char type_code, etch_type* custom_struct_type, 
    const int initsize, const int deltsize, 
    const int is_readonly);

etch_arrayvalue* new_arrayvalue_default();

ETCH_ARRAY_ELEMENT* new_array_element(const int objtype);

int   arrayvalue_add (etch_arrayvalue* thisp, ETCH_ARRAY_ELEMENT* content);

int   arrayvalue_count(etch_arrayvalue*);
void* arrayvalue_get(etch_arrayvalue*, const int index);

int   arrayvalue_set_iterator(etch_arrayvalue*, etch_iterator*);
void  arrayvalue_set_static_content(etch_arrayvalue*, const int is_set);

int   arrayvalue_to_nativearray(etch_arrayvalue*);

signed char arrayvalue_get_external_typecode (unsigned short, unsigned short);
signed char etch_classid_to_arraytype (const unsigned short class_id);
unsigned short etch_arraytype_to_classid (const signed char typecode);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHARRAYVAL_H */
