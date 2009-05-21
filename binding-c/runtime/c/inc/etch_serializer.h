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
 * etch_serializer.h 
 * builtin custom type serializers
 */

#ifndef ETCH_SERIALIZER_H
#define ETCH_SERIALIZER_H

#include "etchobj.h"
#include "etch_id_name.h"

/* we restate typedefs stated elsewhere, here, 
 * since we can't include those headers in this one.
 */
typedef struct etch_id_name    etch_type; 
typedef struct etch_id_name    etch_field; 
typedef struct etch_hashtable  etch_hashtable;
typedef struct etch_validator  etch_validator;
typedef struct etch_serializer etch_serializer;
typedef etch_serializer* (*etch_serializer_ctor) (etch_type*, etch_field*);


/**
 * etch_serializer
 * import/export helper
 */
typedef struct etch_serializer
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    struct i_value_factory* vtab;       
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

    objmask* impl;

    objmask* (*export_value) (struct etch_serializer* thisx, objmask* objval); 
  
    objmask* (*import_value) (struct etch_serializer* thisx, objmask* structval); 

    etch_id_name*  type;  /* not owned */
    etch_id_name* field;  /* not owned */

} etch_serializer;


etch_serializer* new_etch_serializer(const int objsize);

int etch_serializer_init(etch_type*, const wchar_t*, const unsigned, 
    etch_hashtable*, etch_validator*, etch_serializer_ctor);


int etch_serializer_exception_init(etch_type*, etch_hashtable* c2tmap);
int etch_serializer_rtxcp_init    (etch_type*, etch_hashtable* c2tmap);
int etch_serializer_authxcp_init  (etch_type*, etch_hashtable* c2tmap);
int etch_serializer_list_init     (etch_type*, etch_hashtable* c2tmap);
int etch_serializer_map_init      (etch_type*, etch_hashtable* c2tmap);
int etch_serializer_set_init      (etch_type*, etch_hashtable* c2tmap);
int etch_serializer_date_init     (etch_type*, etch_hashtable* c2tmap);


#endif /* #ifndef ETCH_SERIALIZER_H */