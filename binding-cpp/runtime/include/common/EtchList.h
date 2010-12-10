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

#ifndef __ETCHLIST_H__
#define __ETCHLIST_H__

#include "common/EtchObject.h"
#include "common/EtchError.h"

/**
 * String type.
 */
template <class T>
class EtchList :
    public EtchObject
{
private:
    /**
     * Internal ListItem
     */
    class Item
    {
    public:
        Item()
        {
            next = NULL;
        }

        T data;
        Item* next;
    };

public:

    /**
     * TypeId for EtchList.
     */
    static const int32_t TYPE_ID = EOTID_LIST;

    /**
     * Constructs EtchList.
     */
    EtchList();

    /**
     * Destructure.
     */
    virtual ~EtchList();

    /**
     *  Add a new element to the list.
     */
    void add(T data);

    /**
     * Get element at index.
     */
    status_t get(int32_t index, T* data);

private:
    Item* m_head;
    Item* m_tail;
};

template<class T>
EtchList<T>::EtchList()
    : EtchObject(EtchList<T>::TYPE_ID)
    , m_head(NULL)
    , m_tail(NULL)
{
}

template<class T>
EtchList<T>::~EtchList()
{
    if(m_head != NULL) {
        Item* item = m_head;
        while(item != NULL) {
            Item* delete_item = item;
            item = item->next;
            delete delete_item;
        }
    }
}

template<class T>
void EtchList<T>::add(T data)
{
    Item* item = new Item();
    item->data = data;
    item->next = NULL;

    if(m_tail == NULL) {
        m_head = item;
        m_tail = item;
    } else {
        m_tail->next = item;
        m_tail = item;
    }
}

template<class T>
status_t EtchList<T>::get(int32_t index, T* data)
{
    return ETCH_NO_ERROR;
}

#endif
