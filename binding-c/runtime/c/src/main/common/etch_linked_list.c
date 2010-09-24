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
 * etch_linklist.c -- implementation of linklist.
 */

#include "etch_linked_list.h"

/*
TODO:
- no etch_object currently
*/


struct etch_linked_list_node
{
    void* data;
    struct etch_linked_list_node* next;
};

typedef struct etch_linked_list_node etch_linked_list_node;


struct etch_linked_list_t
{
    etch_object object;

    void**   impl;                    /*TODO: check why this is a ** here */
    unsigned short content_obj_type;  /* etch obj_type of a native value */
    unsigned short content_class_id;  /* etch class_id of a native value */
    unsigned int count;

    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    etch_linked_list_node* head;
    etch_linked_list_node* tail;
    uint8                  flags;
};

etch_status_t etch_linked_list_create(etch_linked_list_t** list, uint8 flags)
{
    etch_status_t       rv      = ETCH_SUCCESS;
    etch_linked_list_t* newlist = NULL;

    if(list == NULL) {
        return ETCH_EINVAL;
    }

    newlist = etch_malloc(sizeof(etch_linked_list_t), ETCHTYPEB_COLLECTION);
    memset(newlist, 0, sizeof(etch_linked_list_t));
    newlist->flags = flags;
    
    *list = newlist;

    return rv;
}

etch_status_t etch_linked_list_add(etch_linked_list_t* list, void* data)
{
    etch_status_t          rv      = ETCH_SUCCESS;
    etch_linked_list_node* newnode = NULL;

    if(list == NULL || data == NULL) {
        return ETCH_EINVAL;
    }

    newnode = etch_malloc(sizeof(etch_linked_list_node), 0);
    newnode->data = data;
    newnode->next = NULL;

    if(list->head == NULL) {
        list->head = newnode;
    }
    else {
        ETCH_ASSERT(list->tail != NULL);
        list->tail->next = newnode;
    }
    list->tail = newnode;
    list->count++;

    return rv;
}



etch_status_t etch_linked_list_insert(etch_linked_list_t* list, int32 index, void* data)
{
    etch_status_t          rv      = ETCH_SUCCESS;
    etch_linked_list_node* newnode = NULL;
    etch_linked_list_node* node    = NULL;
    uint32                 i       = 0;

    if(list == NULL ||data == NULL) {
        return ETCH_EINVAL;
    }

    if((uint32)index >= list->count || index < 0) {
        return ETCH_EOUTOFBOUNDS;
    }

    /* check if index is fist element in the list or last position */
    if(list->head == NULL) {
        return etch_linked_list_add(list, data);
    }

    newnode = etch_malloc(sizeof(etch_linked_list_node), 0);
    newnode->data = data;
    newnode->next = NULL;

    rv = ETCH_ERROR;
    node = list->head;
    while(node != NULL) {
        if(index == 0) {
            newnode->next = list->head;
            list->head = newnode;
            rv = ETCH_SUCCESS;
            break;
        }
        if(index - 1 == i) {
            newnode->next = node->next;
            node->next = newnode;
            rv = ETCH_SUCCESS;
            break;
        }
        node = node->next;
        i++;
    }

    ETCH_ASSERT(rv == ETCH_SUCCESS);
    list->count++;

    return rv;
}

etch_status_t etch_linked_list_get(etch_linked_list_t* list, int32 index, void** data)
{
    etch_status_t          rv      = ETCH_SUCCESS;
    etch_linked_list_node* node    = NULL;
    uint32                 i       = 0;

    if(list == NULL ||data == NULL) {
        return ETCH_EINVAL;
    }

    if((uint32)index >= list->count || index < 0) {
        return ETCH_EINVAL;
    }

    node = list->head;
    while(node != NULL) {
        if(index == i) {
            *data = node->data;
            break;
        }
        node = node->next;
        i++;
    }
    return rv;
}

int32 etch_linked_list_index_of(etch_linked_list_t* list, const void* data)
{
    etch_linked_list_node* node    = NULL;
    int32                  i       = 0;
    int32                  index   = -1;

    if(list == NULL ||data == NULL) {
        return ETCH_EINVAL;
    }

    node = list->head;
    while(node != NULL) {
        if(node->data == data) {
            index = i;
            break;
        }
        node = node->next;
        i++;
    }
    return index;
}

uint8 etch_linked_list_contains(etch_linked_list_t* list, const void* data)
{
    etch_linked_list_node* node = NULL;

    if(list == NULL || data == NULL) {
        return FALSE;
    }

    node = list->head;
    while(node != NULL) {
        if(node->data == data) {
            return TRUE;
        }
        node = node->next;
    }
    return FALSE;
}

etch_status_t etch_linked_list_remove_at(etch_linked_list_t* list, const int32 index)
{
    etch_status_t          rv     = ETCH_SUCCESS;
    etch_linked_list_node* rmnode = NULL;
    etch_linked_list_node* node   = NULL;
    uint32                 i      = 0;

    if(list == NULL) {
        return ETCH_EINVAL;
    }

    if((uint32)index >= list->count || index < 0) {
        return ETCH_EOUTOFBOUNDS;
    }

    /* check if only one element available */
    if(list->count == 1) {
        if(list->flags & ETCH_LINKED_LIST_DATA_FREE) {
            etch_free(list->head->data);
        }
        etch_free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->count = 0;
        return ETCH_SUCCESS;
    }

    node = list->head;
    while(node != NULL) {
        if(index == 0) {
            rmnode = node;
            list->head = node->next;
            break;
        }
        if(index - 1 == i && index == list->count - 1 ) {
            rmnode = node->next;
            node->next = NULL;
            list->tail = node;
            break;
        }
        if(index - 1 == i) {
            rmnode = node->next;
            node->next = node->next->next;
            break;
        }
        node = node->next;
        i++;
    }

    /* delete node */
    ETCH_ASSERT(rmnode != NULL);
    if(rmnode != NULL) {
        if(list->flags & ETCH_LINKED_LIST_DATA_FREE) {
            etch_free(rmnode->data);
        }
        etch_free(rmnode);
        list->count--;
    } else {
        rv = ETCH_ERROR;
    }

    return rv;
}

etch_status_t etch_linked_list_remove(etch_linked_list_t* list, void* data)
{
    etch_status_t rv    = ETCH_SUCCESS;
    int32         index = -1;

    if(list == NULL || data == NULL) {
        return ETCH_EINVAL;
    }

    index = etch_linked_list_index_of(list, data);
    rv = etch_linked_list_remove_at(list, index);

    return rv;
}

etch_status_t etch_linked_list_clear(etch_linked_list_t* list)
{
    etch_status_t           rv   = ETCH_SUCCESS;
    etch_linked_list_node*  node = NULL;
    etch_linked_list_node*  temp = NULL;

    if(list == NULL) {
        return ETCH_EINVAL;
    }

    node = list->head;
    while(node != NULL) {
        temp = node;
        node = node->next;

        if(list->flags & ETCH_LINKED_LIST_DATA_FREE) {
            etch_free(temp->data);
        }
        etch_free(temp);
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    return rv;
}

uint32 etch_linked_list_count(etch_linked_list_t* list)
{
    if(list == NULL) {
        return 0;
    }
    return list->count;
}

static etch_status_t etch_linked_list_cleanup(void* p)
{
    etch_status_t       rv   = ETCH_SUCCESS;
    etch_linked_list_t* list = NULL;

    if(p == NULL) {
        return ETCH_EINVAL;
    }
    list = p;
    etch_linked_list_clear(list);
    etch_free(list);

    return rv;
}

etch_status_t etch_linked_list_destroy(etch_linked_list_t* list)
{
    etch_status_t rv = ETCH_SUCCESS;
    rv = etch_linked_list_cleanup(list);
    return rv;
}









///**
// * new_linkist()
// * constructor for an etch_linklist.
// * Creates the underlying list and returns a pointer to the list.
// */
//etch_linklist* new_linklist()
//{
//    etch_linklist* list = etch_malloc(sizeof(etch_linklist), ETCHTYPEB_COLLECTION);
//    memset(list, 0, sizeof(etch_linklist));
//    return list;
//}
//
//
///*
// * linklist_clear() 
// * remove all content from the arraylist, freeing bucket memory always, 
// * and content memory if requested.  
// */
//void linklist_clear(etch_linklist* list, const int is_free_content) 
//{
//    linklist_node *p = NULL, *nextp = NULL;
//    if (NULL == list) return;
//
//    for(p = list->head; p; p = nextp)
//    {   
//        if (is_free_content && p->content) 
//            etch_free(p->content);
//        nextp = p->next;
//        free(p);               
//    }
//
//    memset(list, 0, sizeof(etch_linklist));          
//}
//
//
///**
// * linklist_destroy()
// * Destructor for an etch_linklist.
// * Destroys the underlying list, the list shell, and the list content if requested.
// */
//void linklist_destroy(etch_linklist* list, const int is_free_content) 
//{    
//    if (NULL == list) return;
//    linklist_clear(list, is_free_content);
//    etch_free(list);
//}
//
//
///*
// * linklist_add() 
// * add a node to the end of the list, returning the new node's index
// */
//int linklist_add(etch_linklist* list, void* content) 
//{
//    linklist_node* node = NULL;
//    if (NULL == list) return -1;
//
//    node = etch_malloc(sizeof(linklist_node), ETCHTYPEB_LINKLIST);
//    node->content = content;
//    node->next = NULL;
//    
//    if (list->head == NULL) /* first entry? */
//        list->head = node;
//    else                   
//    {   assert(list->tail);    
//        list->tail->next = node;
//    }
//
//    list->tail = node;
//    return ++list->count;
//}
//
//
///*
// * linklist_insert() 
// * add a node anywhere in the list, returning the new node's index,
// * or -1 if a parameter was in error.
// * this implementation cannot insert past the current end of list,
// * in other words to insert at index n, there must be at least n
// * entries currently in the list.
// */
//int linklist_insert(etch_linklist* list, const unsigned int i, void* content)
//{
//    unsigned count = 0;
//    linklist_node* newnode = NULL, *p = NULL, *priorp = NULL;
//    if ((NULL == list) || (i > list->count) || (i < 0)) return -1;
//
//    newnode = etch_malloc(sizeof(linklist_node), ETCHTYPEB_LINKLIST);
//    newnode->content = content;
//    p = priorp = list->head;
//
//    if (list->head == NULL)
//        list->head = list->tail = newnode;
//    else
//    if (i == list->count)
//        list->tail->next = list->tail = newnode;
//    else
//    {   for(p = list->head; p, count < i; p = p->next, count++)
//            priorp = p;
//        
//        newnode->next = priorp->next;
//        priorp->next = newnode;
//    }
//
//    ++list->count;
//    return i;
//}
//
//
///*
// * linklist_moveto() 
// * private method to move to an indicated index in the list.
// * returns -1 if a parameter was in error, else 0;
// * returns also in outp, a pointer to the node at the requested position.
// */
//int linklist_moveto(etch_linklist* list, const unsigned startat, const linklist_node** outp)
//{
//    unsigned count = 0;
//    linklist_node* p = NULL;
//    if ((NULL == list) || (startat > list->count) || (startat < 0)) return -1;
//  
//    p = list->head;
//    while(p != NULL && count < startat){
//        p = p->next;
//        count++;
//    }
//    if(p != NULL) {
//        *outp = p;
//        return 0;
//    }else{
//        *outp = NULL;
//        return -1;
//    }
//}
//
//
///*
// * linklist_containsp() 
// * return 1 or 0 indicating if the list contains the supplied content pointer,
// * or -1 if a parameter was in error.
// */
//int linklist_containsp(etch_linklist* list, void* content, const unsigned startat)
//{
//    struct linklist_node *p = NULL;
//
//    if (-1 == linklist_moveto(list, startat, &p)) return -1;
//
//    for(; p; p = p->next) 
//        if (p->content == content) 
//            return TRUE; 
//     
//    return FALSE;
//}
//
//
///*
// * linklist_indexofp() 
// * if the list contains the supplied content pointer, return its index;
// * return -1 if not found or if a parameter was in error.
// */
//int linklist_indexofp(etch_linklist* list, void* content, const unsigned startat)
//{
//    struct linklist_node *p = NULL;
//    int count = startat, foundat = -1;
//
//    if (-1 == linklist_moveto(list, startat, &p)) return -1;
//
//    for(; p; p = p->next) 
//    {    
//        if (p->content == content) 
//        {   foundat = count;
//            break;
//        }        
//        else count++;
//    }
//     
//    return foundat;
//}
//
//
///*
// * linklist_contains() 
// * return 1 or 0 indicating if the list contains the supplied content 
// * or -1 if a parameter was in error.
// * caller must supply a comparator function of signature int f(void* this, void* that), 
// * which returns -1 if less, 0 if equal, 1 if greater. 
// */
//int linklist_contains(etch_linklist* list, void* content, const unsigned startat, etch_comparator compare)
//{
//    struct linklist_node *p = NULL;
//    int result = 0;
//
//    if (!list || !compare) return -1;
//    if (-1 == linklist_moveto(list, startat, &p)) return -1;
//
//    for(; p; p = p->next) 
//    {
//        result = compare(content, p->content);
//        if (result == 0) return TRUE;
//    }
//     
//    return FALSE;
//}
//
//
///*
// * linklist_indexof() 
// * if the list contains the supplied content pointer, return its index.
// * return -1 if a parameter was in error.
// * caller must supply a comparator function of signature: int (*f)(void* this, void* that); 
// * which must return -1 if less, 0 if equal, 1 if greater. 
// */
//int linklist_indexof(etch_linklist* list, void* content, const unsigned startat, etch_comparator compare)
//{
//    struct linklist_node *p = NULL;
//    int result = 0, count = startat, foundat = -1;
//
//    if (!list || !compare) return -1;
//
//    if (-1 == linklist_moveto(list, startat, &p)) return -1;
//
//    for(; p; p = p->next) 
//    {
//        result = compare(content, p->content);
//        if (result == 0) 
//        {   foundat = count;
//            break;
//        }        
//        else count++;
//    }
//     
//    return foundat;
//}
//
//
///*
// * linklist_get() 
// * return entry at specified index position, or NULL if parameter error.
// * if we find the lists are not generally short, we'll supply an indexed version of this list,
// * and/or implement a resizable array of pointers which we block memcpy to insert and delete.
// */
//linklist_node* linklist_get(etch_linklist* list, const unsigned i)
//{
//    unsigned count = 0;
//    struct linklist_node *p = NULL;
//    if ((NULL == list) || (i >= list->count) || (i < 0)) return NULL;
//
//    if(linklist_moveto(list, i, &p) != -1){
//        return p;
//    }
//
//    return NULL; /* can't arrive here */
//}
//
//
///*
// * linklist_remove() 
// * remove entry at specified index position, free bucket memory; free content memory on request
// * return -1 if a parameter was in error, or zero if OK.
// */
//int linklist_remove(etch_linklist* list, const unsigned i, const int is_free_content)
//{
//    int count = 0;
//    struct linklist_node *p = NULL, *priorp = NULL, *tmp = NULL;
//    if (!list || (i >= list->count) || (i < 0)) return -1;
//
//    if(i == 0 && list->head){
//        if(list->head == list->tail) 
//            list->tail = NULL;
//        tmp = list->head;
//        list->head = list->head->next;
//    } else {
//        p = linklist_get(list,i-1);
//        tmp = p->next;
//        if(p && p->next && p->next->next){
//            p->next = p->next->next;
//        } else if (p->next && !p->next->next){
//            list->tail = p;
//            p->next = NULL;
//        } else {
//            return -1;
//        }
//    }
//
//    if(tmp) {
//        if (is_free_content && tmp->content)
//            etch_free(tmp->content);
//        etch_free(tmp);
//        if(list->count) {
//            list->count--; 
//        }
//    }
//
//    return 0;
//}
//
//
//int linklist_size(etch_linklist* list)
//{
//    linklist_node* cur = list->head;
//    int length = 0;
//    while(cur != NULL) {
//        length++;
//        cur = cur->next;
//    }
//    return length;
//}
//
