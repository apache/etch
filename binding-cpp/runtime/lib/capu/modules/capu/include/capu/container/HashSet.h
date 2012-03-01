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

#ifndef __HASHSET_H__
#define __HASHSET_H__

#include "capu/container/Comparator.h"
#include "capu/container/Hash.h"
#include "capu/container/List.h"

#define DEFAULT_HASH_SET_SIZE 1000


namespace capu {

  template <class T, class C = Comparator, class H = Hash >
  class HashSet {

  private:
    class HashSetIterator {
    public:

      /**
       * constructor
       *
       * @param list     array of linked list which provide channing for hash set
       * @param listSize size of hash set (size of linked list array)
       */
      HashSetIterator(List<T, C> * list, uint64_t listSize);

      /**
       * destructor
       */
      ~HashSetIterator();

      /**
       * Check if iterator has next element.
       * @return false if the next of current node that is pointed, is null otherwise true
       */
      bool_t hasNext();

      /**
       * Get next iterator element.
       * @param value
       * @return CAPU_ERANGE if the next of current node that is pointed, is null
       *         CAPU_EINVAL if the value is NULL
       *         CAPU_OK if the next element has been gotten
       *
       */
      status_t next(T* value);

    private:
      uint64_t mCurrentListIndex;
      typename List<T, C>::Iterator mCurrentListIterator;
      List<T, C> * mList;
      uint64_t mMaxListSize;
    };

  public:

    typedef HashSetIterator Iterator;

    /**
     * Default Constructor
     */
    HashSet();

    /**
     * Parameterized Constructor
     * @param size size of initial HashSet
     */
    HashSet(uint64_t size);

    /**
     * Destructor
     */
    ~HashSet();

    /**
     * put a new value to the hash set.
     *
     * @param value             new value that will be put to hash set
     *
     * @return CAPU_OK if remove is successful
     *         CAPU_ERROR if value already exists in the set
     *
     */
    status_t put(const T &value);

    /**
     * Remove value associated with key in the hash set.
     *
     * @param value             value that will be removed
     *
     * @return CAPU_OK if remove is successful
     *         CAPU_ERANGE if specified value does not exist in hash set
     *
     */
    status_t remove(const T &value);

    /**
     * Returns count of the hash set.
     * @return number of element in hash set
     */
    uint64_t count();

    /**
     * Clear all values of the hash set.
     *
     * @return CAPU_OK if all elements in list have been deleted
     */
    status_t clear();

    /**
     * Return iterator for iterating key value tuples.
     * @return Iterator
     */
    Iterator begin();

  private:

    /**
     * internally used function to check the same key exist in the specified linked list
     *
     * @param index   specify which link list will be checked
     * @param k specify which key will be searched
     * @return -1 if the key is unique
     *          otherwise the index in the linked list
     */
    int32_t __check_duplicate_value(uint64_t index, T k) {
      int32_t count = 0;
      typename List<T, C>::Iterator it = mLists[index].begin();
      T tmp;
      C compare;
      while (it.hasNext()) {
        it.next(&tmp);
        //NOT UNIQUE VALUE
        if (compare(tmp, k))
          return count;
        count++;
      }
      //UNIQUE VALUE
      return -1;
    }

    List<T, C> *mLists;
    uint32_t mSize;
    uint32_t mCount;
  };

  template <class T, class C, class H>
  HashSet<T, C, H>::HashSet()
  : mSize(DEFAULT_HASH_SET_SIZE)
  , mCount(0) {
    mLists = new List<T, C>[mSize];
  }

  template <class T, class C, class H>
  HashSet<T, C, H>::~HashSet() {
    delete [] mLists;
  }

  template <class T, class C, class H>
  HashSet<T, C, H>::HashSet(uint64_t size)
  : mSize(size)
  , mCount(0) {
    mLists = new List<T, C>[mSize];
  }

  template <class T, class C, class H>
  status_t HashSet< T, C, H>::put(const T &value) {
    status_t result;
    uint64_t index = H::Digest(value) % mSize;
    if (mLists[index].isEmpty()) {
      result = mLists[index].add(value);
      if (result != CAPU_OK) {
        return result;
      }
      mCount++;
      //THERE IS NO OLD VALUE
    } else {
      int32_t key_duplicate_index = this->__check_duplicate_value(index, value);
      if (key_duplicate_index < 0) {
        result = mLists[index].add(value);
        if (result != CAPU_OK) {
          return result;
        }
        mCount++;
        //THERE IS NO OLD VALUE
      } else {
        //THERE IS A NEW VALUE
        return CAPU_ERROR;
      }
    }
    return CAPU_OK;
  }

  template <class T, class C, class H>
  status_t HashSet< T, C, H>::remove(const T &value) {
    status_t result;
    uint64_t index = H::Digest(value) % mSize;

    int32_t key_index = this->__check_duplicate_value(index, value);
    if (key_index < 0) {
      return CAPU_ERANGE;
    } else {
      //delete the existing file
      result = mLists[index].removeAt(key_index);
      if (result != CAPU_OK) {
        return result;
      }
      mCount--;
    }
    return CAPU_OK;
  }

  template <class T, class C, class H>
  uint64_t HashSet< T, C, H>::count() {
    return mCount;
  }

  template <class T, class C, class H>
  status_t HashSet< T, C, H>::clear() {
    for (uint64_t i = 0; i < mSize; ++i) {
      mLists[i].clear();
    }
    mCount = 0;
    return CAPU_OK;
  }

  template <class T, class C, class H>
  HashSet< T, C, H>::HashSetIterator::HashSetIterator(List<T, C> * list, uint64_t list_size) {
    mCurrentListIndex = 0;
    this->mList = list;
    mMaxListSize = list_size;
    this->mCurrentListIterator = list[mCurrentListIndex].begin();

    //to point the first non-empty one
    for (uint64_t i = 0; i < list_size; ++i) {
      if (!mList[i].isEmpty()) {
        mCurrentListIndex = i;
        this->mCurrentListIterator = list[i].begin();
        break;
      }
    }
  }

  template <class T, class C, class H>
  HashSet< T, C, H>::HashSetIterator::~HashSetIterator() {

  }

  template <class T, class C, class H>
  bool_t HashSet< T, C, H>::HashSetIterator::hasNext() {
    if (mCurrentListIterator.hasNext()) {
      return true;
    } else {
      do {
        mCurrentListIndex++;
        if (mCurrentListIndex >= mMaxListSize) {
          return false;
        }
      } while (mList[mCurrentListIndex].isEmpty());

      if (mCurrentListIndex < mMaxListSize) {
        return true;
      } else {
        return false;
      }
    }
  }

  template <class T, class C, class H>
  status_t HashSet< T, C, H>::HashSetIterator::next(T* value) {
    if (value == NULL) {
      return CAPU_EINVAL;
    } else if (mCurrentListIndex >= mMaxListSize) {
      return CAPU_ERANGE;
    } else {
      status_t result = mCurrentListIterator.next(value);
      if (result != CAPU_OK)
        return result;

      if (!mCurrentListIterator.hasNext()) {
        do {
          mCurrentListIndex++;
          if (mCurrentListIndex >= mMaxListSize) {
            break;
          }
        } while (mList[mCurrentListIndex].isEmpty());

        if (mCurrentListIndex < mMaxListSize) {
          mCurrentListIterator = mList[mCurrentListIndex].begin();
        }
      }
    }
    return CAPU_OK;
  }

  template <class T, class C, class H>
  typename HashSet<T, C, H>::Iterator HashSet<T, C, H>::begin() {
    return HashSet<T, C, H>::Iterator(mLists, mSize);
  }

}

#endif /* HASHSET_H */

