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
 * etch_sourceint.h
 * source interface
 */
#ifndef ETCHISOURCE_H
#define ETCHISOURCE_H

#include "etch_transportint.h"

typedef void* (*etch_source_get_handler) (void* thisx);
typedef void  (*etch_source_set_handler) (void* thisx, void* handler);


/**
 * i_source
 * source interface
 * common interface to sources of data, packets, or messages.
 * models the various handlers' view of a communication channel
 */
typedef struct i_source
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

    void* thisx; /* object which is the i_source */

    /**
     * get_handler(this) returns an object implementing i_sourcehandler
     */
    etch_source_get_handler get_handler;

    /**
     * set_handler(this, handler) expects an object implementing i_sourcehandler
     */
    etch_source_set_handler set_handler;

    /**
     * transport interface (query, control, notify), not an etch object
     */
    i_transport* itransport;

} i_source;


i_source* new_default_source_interface();

i_source* new_source_interface(void* thisx,
    etch_source_get_handler gh, etch_source_set_handler sh, i_transport* xp); 


#endif /* ETCHISOURCE_H */
