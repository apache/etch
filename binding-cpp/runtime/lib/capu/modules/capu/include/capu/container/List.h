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

#ifndef __List_H__
#define __List_H__

#include "capu/Error.h"
#include "capu/Config.h"
#include "capu/container/Comparator.h"

namespace capu
{
    template <class T, class C = Comparator>
    class List
    {
    private:
        class ListNode
        {
        public:

            ListNode()
                : mNext(0)
                , mPrev(0)
            {
            }

            ListNode(const T& data)
                : mData(data)
                , mNext(0)
                , mPrev(0)
            {
            }

            T mData;
            ListNode* mNext;
            ListNode* mPrev;
        };

        class ListIterator
        {
        public:
            friend class List<T, C>;

            /**
            * destructor
            */
            ~ListIterator();

            /**
            * Check if iterator has next element.
            * @return false if the next of current node that is pointed, is null otherwise true
            */
            bool_t hasNext();

            /**
            * Shifts the iterator to the next position and returns the element if next != NULL
            * @param element
            * @return CAPU_OK if the next element has been gotten
            *
            */
            status_t next(T* element = 0);

            /**
            * Get current iterator element.
            * @param element
            * @return CAPU_OK if the current element has been gotten
            */
            status_t current(T* element);

            /**
            * Returns the index of the current element.
            * @return The index of the current element. If there is no current element, the return value is undefined.
            */
            uint32_t currentIndex();

        private:
            ListIterator(ListNode* boundary); // creation only in list.begin()
            ListNode* mInitialNode;
            ListNode* mCurrentNode;
            uint32_t mIndex;
        };

        ListNode* mBoundary;
        uint_t mSize;
        C mComparator;

        void insertElement(ListNode* addPosition, const T& element);
        void deleteElement(ListNode* deletePosition);
        ListNode* findElement(uint_t index) const;

   public:
        typedef typename List<T, C>::ListIterator Iterator;

        /**
        * Default Constructor
        */
        List();

        /**
        * Copy constructor
        */
        List(const List<T, C>& other);

        /**
        * Destructor
        */
        virtual ~List();


        List<T,C>& operator=(List<T, C> const& other);

        /**
        * Add element to the end of list
        * @param element element that will be added
        * @return CAPU_ENO_MEMORY if allocation of element is failed
        *         CAPU_OK if the element is successfully added
        */
        status_t add(const T& element);

        /**
        * Add element to specified position
        *
        * @param index index of element which will be inserted
        * @param element new value that will replace the old value
        *
        * @return CAPU_EINVAL if given index is invalid.
        *         CAPU_ENO_MEMORY if allocation of element is failed
        *         CAPU_OK if the element is successfully added
        *         CAPU_ERROR otherwise
        */
        status_t add(uint_t index, const T& element);

        /**
        * Add element to specified position
        *
        * @param iterator with the position to insert
        * @param element new value that will replace the old value
        *
        * @return CAPU_ENO_MEMORY memory allocation failed.
        *         CAPU_OK otherwise
        */
        status_t add(Iterator& iter, const T &element);

        /**
        * Sets the element at the specified index.
        * @param index the index of element that will be replaced
        * @param element element that will be overwritten to existing place
        * @param elementOld the buffer to keep the existing
        * @return CAPU_EINVAL if the index is not valid
        *         CAPU_OK otherwise
        */
        status_t set(uint_t index, const T &element, T* elementOld = 0);

        /**
        * remove the element in the specified index and if the element_old
        * parameter is not NULL, the removed element will be put to element_old
        * @param index index of element that will be removed
        * @param elementOld the buffer which will keep the copy of the removed element
        * @return CAPU_EINVAL invalid index
        *         CAPU_OK if the element is successfully removed
        */
        status_t removeAt(uint_t index, T* elementOld = 0);

        /**
        * remove the element in the specified iterator position and if the element_old
        * parameter is not NULL, the removed element will be put to element_old
        * @param iterator of element that will be removed
        * @param element_old the buffer which will keep the copy of the removed element
        * @return CAPU_EINVAL invalid iterator
        *         CAPU_OK if the element is successfully removed
        *
        */
        status_t removeAt(Iterator& listIterator, T* elementOld = 0);

        /**
        * Removes an element.
        * @param element The element that will get removed.
        * @return CAPU_EINVAL if element was not found
        *         CAPU_OK if element was removed.
        */
        status_t remove(const T& element);

        /**
        * removes all elements from linked list
        *
        * @return CAPU_OK if all of the elements are deleted
        */
        status_t clear();

        /**
        * get a single element on specified index
        * @param index index of element that will be get
        * @param result the buffer that the retrieved element will be stored
        * @return CAPU_EINVAL invalid index
        *         CAPU_OK otherwise
        */
        status_t get(uint_t index, T* result) const;

        /**
        * return size of list
        * @return return the size of list
        */
        int_t size() const;

        /**
        * check the list is empty or not
        * @return true if empty
        *         false otherwise
        */
        bool_t isEmpty() const;

        /**
        * returns an iterator pointing to the beginning of list
        * @return iterator
        */
        Iterator begin() const;

        /**
        * finds the index of given element in the link list
        * if you are using an object you need to overload == operator
        *
        * @param element the value that will be searched
        * @return -1 if the value either does not exist or given value is NULL
        *          otherwise index of value on linked list
        */
        int_t find(const T &element) const;

        /**
        * check that if the list contains the given parameter or not
        * if you are using an object you need to overload == operator
        * @param element element that will be checked
        * @return true list contains it
        *         false otherwise
        */
        bool_t contains(const T &element) const;
    };

    template <class T, class C>
    List<T, C>::List()
        : mBoundary(new ListNode())
        , mSize(0)
    {
        mBoundary->mNext = mBoundary;
        mBoundary->mPrev = mBoundary;
    }

    template <class T, class C>
    List<T, C>::List(const List<T, C>& other)
        : mBoundary(new ListNode())
        , mSize(0)
    {
        mBoundary->mNext = mBoundary;
        mBoundary->mPrev = mBoundary;

        // add all items from the other list
        Iterator it = other.begin();
        T current;
        while(it.hasNext())
        {
            it.next(&current);
            add(current);
        }
    }

    template <class T, class C>
    List<T, C>::~List()
    {
        clear();
        delete mBoundary;
    }

    template <class T, class C>
    status_t List<T, C>::clear()
    {
        ListNode* current = mBoundary->mNext;
        ListNode* toDelete = 0;
        while (current != mBoundary)
        {
            toDelete = current;
            current = current->mNext;
            delete toDelete;
        }
        mSize = 0;
        mBoundary->mNext = mBoundary;
        mBoundary->mPrev = mBoundary;
        return CAPU_OK;
    }

    template <class T, class C>
    List<T,C>& List<T, C>::operator=(List<T, C> const& other)
    {
        clear();
        mBoundary->mNext = mBoundary;
        mBoundary->mPrev = mBoundary;

        // add all items from the other list
        Iterator it = other.begin();
        T current;
        while(it.hasNext())
        {
            it.next(&current);
            add(current);
        }
        return *this;
    }

    template <class T, class C>
    int_t List<T, C>::find(const T &element) const
    {
        int_t counter = 0;
        ListNode* current = mBoundary->mNext;
        while (current != mBoundary)
        {
            if (mComparator(current->mData, element))
            {
                // element was found, return index
                return counter;
            }
            current = current->mNext;
            ++counter;
        }

        // not found
        return -1;
    }

    template <class T, class C>
    inline bool_t List<T, C>::contains(const T &element) const
    {
        return find(element) != -1;
    }

    template <class T, class C>
    inline status_t List<T, C>::add(Iterator& iter, const T &element)
    {
        insertElement(iter.mCurrentNode->mPrev, element); // insert before found position
        return CAPU_OK;
    }

    template <class T, class C>
    inline status_t List<T, C>::add(uint_t index, const T &element)
    {
        if (index > mSize) // if index == mSize, element is added at the end
        {
            // invalid index
            return CAPU_EINVAL;
        }

        insertElement(findElement(index)->mPrev, element); // insert before found position
        return CAPU_OK;
    }

    template <class T, class C>
    inline status_t List<T, C>::add(const T &element)
    {
        insertElement(mBoundary->mPrev, element); // insert at list end (boundary->mPrev)
        return CAPU_OK;
    }

    template <class T, class C>
    inline void List<T, C>::insertElement(typename List<T, C>::ListNode* addPosition, const T& element)
    {
        ListNode* newNode = new ListNode(element); // copy in
        newNode->mNext = addPosition->mNext;
        newNode->mPrev = addPosition;
        addPosition->mNext->mPrev = newNode;
        addPosition->mNext = newNode;
        ++mSize;
    }

    template <class T, class C>
    inline void List<T, C>::deleteElement(typename List<T, C>::ListNode* deletePosition)
    {
        deletePosition->mPrev->mNext = deletePosition->mNext;
        deletePosition->mNext->mPrev = deletePosition->mPrev;
        delete deletePosition;
        --mSize;
    }

    template <class T, class C>
    typename List<T, C>::ListNode* List<T, C>::findElement(uint_t index) const
    {
        // search element by running through list from the first element
        uint_t counter = 0;
        ListNode* current = mBoundary->mNext;
        while(counter < index)
        {
            ++counter;
            current = current->mNext;
        }

        // current is the element that was requested
        return current;
    }


    template <class T, class C>
    status_t List<T, C>::remove(const T& element)
    {
        ListNode* current = mBoundary->mNext;
        while (current != mBoundary)
        {
            if (mComparator(current->mData, element))
            {
                // deletion element found
                deleteElement(current);
                return CAPU_OK;
            }
            current = current->mNext;
        }
        return CAPU_EINVAL;
    }

    template <class T, class C>
    status_t List<T, C>::removeAt(uint_t index, T* elementOld)
    {
        if (index >= mSize)
        {
            // invalid index
            return CAPU_EINVAL;
        }

        ListNode* toDelete = findElement(index);
        if (elementOld)
        {
            *elementOld = toDelete->mData; // copy out
        }
        deleteElement(toDelete);
        return CAPU_OK;
    }

    template <class T, class C>
    status_t List<T, C>::removeAt(Iterator& listIterator, T* elementOld)
    {
        if (listIterator.hasNext())
        {
            if (elementOld)
            {
                *elementOld = listIterator.mCurrentNode->mData; // copy out
            }
            ListNode* node = listIterator.mCurrentNode;
            listIterator.mCurrentNode = listIterator.mCurrentNode->mNext;
            deleteElement(node);
            return CAPU_OK;
        }
        return CAPU_EINVAL;
    }

    template <class T, class C>
    inline status_t List<T, C>::set(uint_t index, const T &element, T* elementOld)
    {
        if (index >= mSize)
        {
            // invalid index
            return CAPU_EINVAL;
        }

        ListNode* node = findElement(index);
        if (elementOld)
        {
            *elementOld = node->mData; // copy out
        }
        node->mData = element; // copy in
        return CAPU_OK;
    }

    template <class T, class C>
    status_t List<T, C>::get(uint_t index, T* result) const
    {
        if (index >= mSize)
        {
            // invalid index
            return CAPU_EINVAL;
        }

        *result = findElement(index)->mData; // copy out
        return CAPU_OK;
    }

    template <class T, class C>
    inline int_t List<T, C>::size() const
    {
        return mSize;
    }

    template <class T, class C>
    inline bool_t List<T, C>::isEmpty() const
    {
        return mSize == 0;
    }

    template <class T, class C>
    typename List<T, C>::Iterator List<T, C>::begin() const
    {
        return ListIterator(mBoundary);
    }

    template <class T, class C>
    List<T, C>::ListIterator::ListIterator(ListNode* boundary)
        : mInitialNode(boundary)
        , mCurrentNode(boundary->mNext)
        , mIndex(0)
    {
    }

    template <class T, class C>
    List<T, C>::ListIterator::~ListIterator()
    {
    }

    template <class T, class C>
    bool_t List<T, C>::ListIterator::hasNext()
    {
        return mInitialNode != mCurrentNode;
    }

    template <class T, class C>
    status_t List<T, C>::ListIterator::next(T* element)
    {
        if (element)
        {
            status_t ret = current(element);
            if (ret != CAPU_OK)
            {
                return ret; // don't continue
            }
        }
        mCurrentNode = mCurrentNode->mNext;
        ++mIndex;
        return CAPU_OK;
    }

    template <class T, class C>
    uint32_t List<T, C>::ListIterator::currentIndex()
    {
        return mIndex;
    }

    template <class T, class C>
    status_t List<T, C>::ListIterator::current(T* element)
    {
        if (!hasNext())
        {
            return CAPU_ERANGE;
        }
        if (!element)
        {
            return CAPU_EINVAL;
        }
        *element = mCurrentNode->mData; // copy out
        return CAPU_OK;
    }
}

#endif /* __List_H__ */
