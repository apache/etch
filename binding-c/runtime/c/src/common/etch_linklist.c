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

#include <stdio.h>
#include "etch_linklist.h"
#include "etchmem.h"



/**
 * new_linkist()
 * constructor for an etch_linklist.
 * Creates the underlying list and returns a pointer to the list.
 */
etch_linklist* new_linklist()
{
    etch_linklist* list = etch_malloc(sizeof(etch_linklist), ETCHTYPEB_COLLECTION);
    memset(list, 0, sizeof(etch_linklist));
    return list;
}


/*
 * linklist_clear() 
 * remove all content from the arraylist, freeing bucket memory always, 
 * and content memory if requested.  
 */
void linklist_clear(etch_linklist* list, const int is_free_content) 
{
    linklist_node *p = NULL, *nextp = NULL;
    if (NULL == list) return;

    for(p = list->head; p; p = nextp)
    {   
        if (is_free_content && p->content) 
            etch_free(p->content);
        nextp = p->next;
        free(p);               
    }

    memset(list, 0, sizeof(etch_linklist));          
}


/**
 * linklist_destroy()
 * Destructor for an etch_linklist.
 * Destroys the underlying list, the list shell, and the list content if requested.
 */
void linklist_destroy(etch_linklist* list, const int is_free_content) 
{    
    if (NULL == list) return;
    linklist_clear(list, is_free_content);
    etch_free(list);
}


/*
 * linklist_add() 
 * add a node to the end of the list, returning the new node's index
 */
int linklist_add(etch_linklist* list, void* content) 
{
    linklist_node* node = NULL;
    if (NULL == list) return -1;

    node = etch_malloc(sizeof(linklist_node), ETCHTYPEB_LINKLIST);
    node->content = content;
    
    if (list->head == NULL) /* first entry? */
        list->head = node;
    else                   
    {   assert(list->tail);    
        list->tail->next = node;
    }

    list->tail = node;
    return ++list->count;
}


/*
 * linklist_insert() 
 * add a node anywhere in the list, returning the new node's index,
 * or -1 if a parameter was in error.
 * this implementation cannot insert past the current end of list,
 * in other words to insert at index n, there must be at least n
 * entries currently in the list.
 */
int linklist_insert(etch_linklist* list, const unsigned int i, void* content)
{
    unsigned count = 0;
    linklist_node* newnode = NULL, *p = NULL, *priorp = NULL;
    if ((NULL == list) || (i > list->count) || (i < 0)) return -1;

    newnode = etch_malloc(sizeof(linklist_node), ETCHTYPEB_LINKLIST);
    newnode->content = content;
    p = priorp = list->head;

    if (list->head == NULL)
        list->head = list->tail = newnode;
    else
    if (i == list->count)
        list->tail->next = list->tail = newnode;
    else
    {   for(p = list->head; p, count < i; p = p->next, count++)
            priorp = p;
        
        newnode->next = priorp->next;
        priorp->next = newnode;
    }

    ++list->count;
    return i;
}


/*
 * linklist_moveto() 
 * private method to move to an indicated index in the list.
 * returns the index moved to, or -1 if a parameter was in error.
 * returns also in outp, a pointer to the node at the requested position.
 */
int linklist_moveto(etch_linklist* list, const unsigned startat, const linklist_node** outp)
{
    unsigned count = 0;
    linklist_node* p = NULL;
    if ((NULL == list) || (startat > list->count) || (startat < 0)) return -1;
  
    for(p = list->head; p, count < startat; p = p->next, count++);
    *outp = p;
    return 0;
}


/*
 * linklist_containsp() 
 * return 1 or 0 indicating if the list contains the supplied content pointer,
 * or -1 if a parameter was in error.
 */
int linklist_containsp(etch_linklist* list, void* content, const unsigned startat)
{
    struct linklist_node *p = NULL;

    if (-1 == linklist_moveto(list, startat, &p)) return -1;

    for(; p; p = p->next) 
        if (p->content == content) 
            return TRUE; 
     
    return FALSE;
}


/*
 * linklist_indexofp() 
 * if the list contains the supplied content pointer, return its index;
 * return -1 if not found or if a parameter was in error.
 */
int linklist_indexofp(etch_linklist* list, void* content, const unsigned startat)
{
    struct linklist_node *p = NULL;
    int count = startat, foundat = -1;

    if (-1 == linklist_moveto(list, startat, &p)) return -1;

    for(; p; p = p->next) 
    {    
        if (p->content == content) 
        {   foundat = count;
            break;
        }        
        else count++;
    }
     
    return foundat;
}


/*
 * linklist_contains() 
 * return 1 or 0 indicating if the list contains the supplied content 
 * or -1 if a parameter was in error.
 * caller must supply a comparator function of signature int f(void* this, void* that), 
 * which returns -1 if less, 0 if equal, 1 if greater. 
 */
int linklist_contains(etch_linklist* list, void* content, const unsigned startat, etch_comparator compare)
{
    struct linklist_node *p = NULL;
    int result = 0;

    if (!list || !compare) return -1;
    if (-1 == linklist_moveto(list, startat, &p)) return -1;

    for(; p; p = p->next) 
    {
        result = compare(content, p->content);
        if (result == 0) return TRUE;
    }
     
    return FALSE;
}


/*
 * linklist_indexof() 
 * if the list contains the supplied content pointer, return its index.
 * return -1 if a parameter was in error.
 * caller must supply a comparator function of signature: int (*f)(void* this, void* that); 
 * which must return -1 if less, 0 if equal, 1 if greater. 
 */
int linklist_indexof(etch_linklist* list, void* content, const unsigned startat, etch_comparator compare)
{
    struct linklist_node *p = NULL;
    int result = 0, count = startat, foundat = -1;

    if (!list || !compare) return -1;
    if (-1 == linklist_moveto(list, startat, &p)) return -1;

    for(; p; p = p->next) 
    {
        result = compare(content, p->content);
        if (result == 0) 
        {   foundat = count;
            break;
        }        
        else count++;
    }
     
    return foundat;
}


/*
 * linklist_get() 
 * return entry at specified index position, or NULL if parameter error.
 * if we find the lists are not generally short, we'll supply an indexed version of this list,
 * and/or implement a resizable array of pointers which we block memcpy to insert and delete.
 */
linklist_node* linklist_get(etch_linklist* list, const unsigned i)
{
    unsigned count = 0;
    struct linklist_node *p = NULL;
    if ((NULL == list) || (i >= list->count) || (i < 0)) return NULL;

    for(p = list->head; p; p = p->next, count++)
        if (count == i) return p;

    return NULL; /* can't arrive here */
}


/*
 * linklist_remove() 
 * remove entry at specified index position, free bucket memory; free content memory on request
 * return -1 if a parameter was in error, or zero if OK.
 */
int linklist_remove(etch_linklist* list, const unsigned i, const int is_free_content)
{
    int priori = i - 1;
    struct linklist_node *p = NULL, *priorp = NULL;
    if (!list || (i >= list->count) || (i < 0)) return -1;
           
    /* move to node prior to requested node */
    if (-1 == linklist_moveto(list, priori, &priorp)) 
    {   
        p = list->head;  /* asked to remove node[0] */
        list->head = p->next;  
        if (list->tail == p)
            list->tail = NULL;
    }
    else
    {   p = priorp->next;
        priorp->next = p->next;
        if (list->tail == p)
            list->tail = priorp;
    }

    if (is_free_content && p->content)
        etch_free(p->content);

    etch_free(p);
    list->count--; 
    return 0;
}

