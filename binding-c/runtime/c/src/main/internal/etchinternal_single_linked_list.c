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

#include "etchinternal_single_linked_list.h"

#include <string.h>
#include "etch_mem.h"


etchinternal_single_linked_list_node* etchinternal_single_linked_list_node_create(void* data, etchinternal_single_linked_list_node* next) {
  etchinternal_single_linked_list_node* res = (etchinternal_single_linked_list_node*)etch_malloc(sizeof(etchinternal_single_linked_list_node), 0);
  res->data = data;
  res->next = next;

  return res;
}

etchinternal_single_linked_list* etchinternal_single_linked_list_create() {
  etchinternal_single_linked_list* res = (etchinternal_single_linked_list*)etch_malloc(sizeof(etchinternal_single_linked_list), 0);
  memset(res, 0, sizeof(etchinternal_single_linked_list));
  return res;
}


void etchinternal_single_linked_list_destroy(etchinternal_single_linked_list* list) {
  etchinternal_single_linked_list_node* help = list->head;
  etchinternal_single_linked_list_node* help2;
  while (help) {
    help2 = help->next;

    free(help->data);
    free(help);

    help = help2;
  }
  free(list);
}

void etchinternal_single_linked_list_add(etchinternal_single_linked_list* list, void* data, size_t size) {
  void* new_data = etch_malloc(size, 0);
  etchinternal_single_linked_list_node* new_node;
  memcpy(new_data, data, size);
  new_node = etchinternal_single_linked_list_node_create(new_data, list->head);
  list->head = new_node;
}

void* etchinternal_single_linked_list_find(etchinternal_single_linked_list* list, etchinternal_find_func find, void* find_data) {
  etchinternal_single_linked_list_node* help = list->head;
  while (help) {
    if (find(help->data, find_data)) {
      return help->data;
    }

    help = help->next;
  }
  return 0;
}
