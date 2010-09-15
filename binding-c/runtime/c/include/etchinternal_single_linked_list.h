/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#ifndef etch_thread_help_h_included
#define etch_thread_help_h_included

#include <stdlib.h>


typedef struct etchinternal_single_linked_list_node {
  void* data;
  struct etchinternal_single_linked_list_node* next;
} etchinternal_single_linked_list_node;

typedef struct etchinternal_single_linked_list {
  etchinternal_single_linked_list_node* head;
} etchinternal_single_linked_list;

etchinternal_single_linked_list_node* etchinternal_single_linked_list_node_create(void* data, etchinternal_single_linked_list_node* next);
etchinternal_single_linked_list* etchinternal_single_linked_list_create();
void etchinternal_single_linked_list_destroy(etchinternal_single_linked_list* list);
void etchinternal_single_linked_list_add(etchinternal_single_linked_list* list, void* data, size_t size);

typedef int (*etchinternal_find_func)(void*, void*);
void* etchinternal_single_linked_list_find(etchinternal_single_linked_list* list, etchinternal_find_func find, void* find_data);

#endif
