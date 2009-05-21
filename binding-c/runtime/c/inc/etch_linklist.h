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
 * etch_linklist.h 
 * linked list implementation.
 */

#ifndef ETCHLINKLIST_H
#define ETCHLINKLIST_H

#include "etch_collection.h"

#define ETCHLINKLIST_DEFSIZE 128
#define ETCHLINKLIST_CONTENT_SIMPLE 0  /* content memory freed as a unit */
#define ETCHLINKLIST_CONTENT_OBJECT 1  /* content is etchobject */
#define ETCHLINKLIST_SYNCHRONIZED TRUE

typedef int (*linklistcallback) (int, void*);  


/** 
 *  linklist_node: an entry in an etch_linklist
 */
struct linklist_node 
{
    char* content;
    struct linklist_node* next;      
};

typedef struct linklist_node linklist_node; 


/** 
 *  etch_linklist: data structure encapsulating the linkist
 */
typedef struct etch_linklist   
{
    unsigned int     hashkey;
    unsigned short   obj_type; 
    unsigned short   class_id;
    struct vtabmask* vtab;       
    int  (*destroy) (void*);
    void*(*clone)   (void*); 
    obj_gethashkey   get_hashkey;
    struct objmask*  parent;
    etchresult*      result;
    unsigned int     refcount;
    unsigned int     length;
    unsigned char    is_null;
    unsigned char    is_copy;
    unsigned char    is_static;
    unsigned char    reserved;

    void**   impl;
    unsigned short content_obj_type;  /* etch obj_type of a native value */
    unsigned short content_class_id;  /* etch class_id of a native value */
    unsigned int count;

    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    linklist_node* head;
    linklist_node* tail;

    i_iterable iterable;

    byte is_readonly;  
    byte content_type;

    linklistcallback freehook; /* hoook for free list content */

} etch_linklist;


etch_linklist* new_linklist();
linklist_node* linklist_get(etch_linklist* list, const unsigned i);
int  linklist_add      (etch_linklist* list, void* content);
int  linklist_insert   (etch_linklist* list, const unsigned i, void* content);
int  linklist_containsp(etch_linklist* list, void* content, const unsigned startat); /* pointer compare */
int  linklist_contains (etch_linklist* list, void* content, const unsigned startat, etch_comparator);
int  linklist_indexofp (etch_linklist* list, void* content, const unsigned startat); /* pointer compare */
int  linklist_indexof  (etch_linklist* list, void* content, const unsigned startat, etch_comparator);
int  linklist_remove   (etch_linklist* list, const unsigned i, const int is_free_content);
void linklist_clear    (etch_linklist* list, const int is_free_content);
void linklist_destroy  (etch_linklist* list, const int is_free_content);


#endif /* #ifndef ETCHLINKLIST_H */