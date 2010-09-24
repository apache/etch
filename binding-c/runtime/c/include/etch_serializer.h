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

#include "etch_object.h"
#include "etch_id_name.h"
#include "etch_type.h"
#include "etch_field.h"

#ifdef __cplusplus
extern "C" {
#endif


/* we restate typedefs stated elsewhere, here, 
 * since we can't include those headers in this one.
 */
struct etch_hashtable;
struct etch_validator;
struct etch_serializer;
typedef struct etch_serializer* (*etch_serializer_ctor) (etch_type*, etch_field*);


/**
 * etch_serializer
 * import/export helper
 */
typedef struct etch_serializer
{
    etch_object object;

    etch_object* impl;

    etch_object* (*export_value) (struct etch_serializer* thisx, etch_object* objval); 
  
    etch_object* (*import_value) (struct etch_serializer* thisx, etch_object* structval); 

    etch_id_name*  type;  /* not owned */
    etch_id_name* field;  /* not owned */

} etch_serializer;

etch_serializer* new_etch_serializer(const int objsize);

int etch_serializer_init(etch_type*, const wchar_t*, const unsigned, 
    struct etch_hashtable*, struct etch_validator*, etch_serializer_ctor);

int etch_serializer_exception_init(etch_type*, struct etch_hashtable* c2tmap);
int etch_serializer_rtxcp_init    (etch_type*, struct etch_hashtable* c2tmap);
int etch_serializer_authxcp_init  (etch_type*, struct etch_hashtable* c2tmap);
int etch_serializer_list_init     (etch_type*, struct etch_hashtable* c2tmap);
int etch_serializer_map_init      (etch_type*, struct etch_hashtable* c2tmap);
int etch_serializer_set_init      (etch_type*, struct etch_hashtable* c2tmap);
int etch_serializer_date_init     (etch_type*, struct etch_hashtable* c2tmap);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_SERIALIZER_H */
