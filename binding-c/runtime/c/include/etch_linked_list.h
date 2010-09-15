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

#ifndef _ETCH_LINKED_LIST_H_
#define _ETCH_LINKED_LIST_H_

#include "etch_collection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_LINKED_LIST_SYNCHRONIZED 1
#define ETCH_LINKED_LIST_DATA_FREE    2

typedef struct etch_linked_list_t etch_linked_list_t;

/**
 * create a new linked list instance.
 */
etch_status_t etch_linked_list_create(etch_linked_list_t** list, uint8 flags);

/**
 * add a new element to the list.
 */
etch_status_t etch_linked_list_add(etch_linked_list_t* list, void* data);

/**
 * get element at index.
 */
etch_status_t etch_linked_list_get(etch_linked_list_t* list, int32 index, void** data);

/**
 * get element at index.
 */
int32 etch_linked_list_index_of(etch_linked_list_t* list, const void* data);

/**
 * check the list if the element is inside.
 */
uint8 etch_linked_list_contains(etch_linked_list_t* list, const void* data);

/**
 * insert a new element at the given index to the list.
 */
etch_status_t etch_linked_list_insert(etch_linked_list_t* list, int32 index, void* data);

/**
 * remove an element from the list.
 */
etch_status_t etch_linked_list_remove_at(etch_linked_list_t* list, const int32 index);

/**
 * remove an element from the list.
 */
etch_status_t etch_linked_list_remove(etch_linked_list_t* list, void* data);

/**
 * clear the list.
 */
etch_status_t etch_linked_list_clear(etch_linked_list_t* list);

/**
 * get count of the list.
 */
uint32 etch_linked_list_count(etch_linked_list_t* list);

/**
 * destroy a linked list instance
 */
etch_status_t etch_linked_list_destroy(etch_linked_list_t* list);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ETCH_LINKED_LIST_H_ */
