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
 * etch_message.h 
 * a message is modeled as a command and some arguments. command is an arbitrary
 * integer value, and the arguments are key / value pairs, where the key is an
 * arbitrary integer value and the value is any one of the standard java objects
 * an array value, a struct value, or any type serializable by value factory.
 */

#ifndef ETCHMESSAGE_H
#define ETCHMESSAGE_H

#include "etch_structval.h"
#include "etch_valufact.h"


/* 
 * etch_message  
 * this class is "final" 
 */
typedef struct etch_message  
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

    etch_structvalue* sv;   /* owned */            
    etch_value_factory* vf; /* not owned */
     
} etch_message;

  
/*
 * etch_unwanted_message
 */
typedef struct etch_unwanted_message
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

    etch_message* message;   /* owned */
    etch_who*     whofrom;   /* not owned */

} etch_unwanted_message;

etch_unwanted_message* new_unwanted_message(etch_who* whofrom, etch_message* msg);


etch_message* new_message (etch_type*, const int, etch_value_factory*);  
int destroy_message (etch_message*);

objmask*      message_get   (etch_message*, etch_field* key);
objmask*      message_remove(etch_message*, etch_field* key); 
int           message_put   (etch_message*, etch_field* key, objmask* value);
int           message_putc  (etch_message*, etch_field* key, void** valref);
etch_message* message_reply (etch_message*, etch_type* type);
etch_type*    message_type  (etch_message*);
char*         message_aname (etch_message*);
etch_int64*   message_get_id(etch_message*);
etch_int64*   message_get_in_reply_to(etch_message*);
int           message_set_in_reply_to(etch_message*, etch_int64* id);
int           message_set_id(etch_message*, etch_int64* id);
int           message_size  (etch_message*);


#endif /* #ifndef ETCHMESSAGE_H*/ 