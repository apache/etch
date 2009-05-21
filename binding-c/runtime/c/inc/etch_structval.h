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
 * etch_structval.h -- defines the etch_structvalue object.
 * etch_structvalue is a typed map of key/value pairs, where type is an etch_type,
 * a key is an etch_field, and value is an etch object. all values are the same 
 * class, corresponding to the struct type.
 */

#ifndef ETCHSTRUCTVAL_H
#define ETCHSTRUCTVAL_H

#include "etchmsgutl.h"
#include "etch_tagdata_inp.h"
#include "etch_tagdata_out.h"
#include "etchhash.h"


/**
 * etch_structvalue
 */
typedef struct etch_structvalue
{
    unsigned int    hashkey;    
    unsigned short  obj_type;  
    unsigned short  class_id;   
    struct objmask* vtab;       
    int  (*destroy)(void*);     
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;   
    struct objmask* parent;    
    etchresult*     result;    
    unsigned int    refcount;      
    unsigned int    length;     
    unsigned char   is_null;   
    unsigned char   is_copy;  
    unsigned char   is_static;  
    unsigned char   reserved;

    etch_hashtable* items;       /* owned */
    etch_type*      struct_type; /* not owned */

} etch_structvalue;


etch_structvalue* new_structvalue(etch_type*, const int initialsize);
int destroy_structvalue (etch_structvalue*);
int structvalue_put     (etch_structvalue*, etch_field* key, objmask* value);
int structvalue_is_type (etch_structvalue*, etch_type*);
objmask* structvalue_get(etch_structvalue*, etch_field* key);
objmask* structvalue_remove(etch_structvalue*, etch_field* key); 
etch_hashtable* new_structvalue_hashtable();
int structvalue_count(etch_structvalue*);


#endif /* #ifndef ETCHSTRUCTVAL_H */ 