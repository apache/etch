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

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__
#include "capu/Error.h"
#include "capu/Config.h"
#include "capu/container/Comparator.h"
#include "capu/container/List.h"
#include "capu/container/Pair.h"
#include "capu/container/Hash.h"

namespace capu {

  template <class Key, class T, class C = Comparator, class Hash = Hash >
  class HashTable {
  private:

    class HashTableIterator {
    public:

      /**
       * constructor
       *
       * @param list     array of linked list which provide channing for hashtable
       * @param listSize size of hash table (size of linked list array)
       */
      HashTableIterator(List<Pair<Key, T> > * list, uint_t listSize);

      /**
       * destructor
       */
      ~HashTableIterator();

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
      status_t next(Pair<Key, T>* value);

    private:
      uint_t mCurrentListIndex;
      typename List<Pair<Key, T> >::Iterator mCurrentListIterator;
      List<Pair<Key, T> > * mList;
      uint_t mMaxListSize;
    };

  public:

    typedef HashTableIterator Iterator;

    /**
     * Constructs HashTable.
     */
    HashTable();

    /**
     * Constructs HashTable.
     */
    HashTable(uint_t size);

    /**
     * Destructure.
     */
    virtual ~HashTable();

    /**
     * put a new value to the hashtable.
     * @param key               Key value
     * @param value             new value that will be put to hash table
     * @param value_old         buffer which will be used to store value of old element
     *
     * @return CAPU_OK if remove is successful
               CAPU_ENO_MEMORY if allocation of element is failed
     *         CAPU_EINVAL if value_old is null
     *
     */
    status_t put(const Key &key, const T &value, T* value_old = NULL);

    /**
     * Get value associated with key in the hashtable.
     * @param key       Key
     * @param value     buffer which will be used to return the found element
     *
     * @return CAPU_OK if get is successful performed
     *         CAPU_EINVAL if value is null
     *         CAPU_ENOT_EXIST if there is no pair with specified key
     */
    status_t get(const Key &key, T* value);

    /**
     * Remove value associated with key in the hashtable.
     *
     * @param key               Key value
     * @param value_old         buffer which will be used to store value of removed element
     *
     * @return CAPU_OK if remove is successful
     *         CAPU_EINVAL if value_old is null
     *         CAPU_ERANGE if the pair with specified key does not exist in hash table
     *
     */
    status_t remove(const Key &key, T* value_old);

    /**
     * Returns count of the hashtable.
     * @return number of element in hash table
     */
    uint_t count();

    /**
     * Clear all key and values of the hashtable.
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
    int_t getKeyIndexFromBucket(uint_t index, const Key &k) {
      int_t count = 0;
      typename List<Pair<Key, T> >::Iterator it = mLists[index].begin();
      Pair<Key, T> pair;
      C compare;
      while (it.hasNext()) {
        it.next(&pair);
        //NOT UNIQUE KEY
        if (compare(pair.first, k))
          return count;
        count++;
      }
      //UNIQUE KEY
      return -1;
    }

    List<Pair<Key, T>, Comparator > *mLists;
    uint_t mSize;
    uint_t mCount;

  };

  template <class Key, class T, class C, class Hash>
  HashTable<Key, T, C, Hash>::HashTable()
  : mSize(HASH_TABLE_DEFAULT_SIZE)
  , mCount(0) {
    mLists = new List<Pair<Key, T>, Comparator >[mSize];
  }

  template <class Key, class T, class C, class Hash>
  HashTable<Key, T, C, Hash>::HashTable(uint_t size)
  : mCount(0) {
    if (size <= 0) {
      mSize = HASH_TABLE_DEFAULT_SIZE;
    } else {
      mSize = size;
    }
    mLists = new List<Pair<Key, T>, Comparator >[mSize];
  }

  template <class Key, class T, class C, class Hash>
  HashTable<Key, T, C, Hash>::~HashTable() {
    delete [] mLists;
  }

  template <class Key, class T, class C, class Hash>
  status_t HashTable<Key, T, C, Hash>::put(const Key &key, const T &value, T* value_old) {
    status_t result;
    uint_t index = Hash::Digest(key) % mSize;
    if (mLists[index].isEmpty()) {
      Pair<Key, T> pair(key, value);
      result = mLists[index].add(pair);
      if (result != CAPU_OK) {
        return result;
      }
      mCount++;
      //THERE IS NO OLD VALUE
    } else {
      Pair<Key, T> new_pair(key, value);
      int_t key_duplicate_index = this->getKeyIndexFromBucket(index, key);
      if (key_duplicate_index < 0) {
        result = mLists[index].add(new_pair);
        if (result != CAPU_OK) {
          return result;
        }
        mCount++;
        //THERE IS NO OLD VALUE
      } else {
        Pair <Key, T> old_pair;
        result = mLists[index].get(key_duplicate_index, &old_pair);
        if (result != CAPU_OK) {
          return result;
        }
        //OLD VALUE
        if (value_old == NULL) {
          return CAPU_EINVAL;
        } else {
          *value_old = old_pair.second;
        }

        result = mLists[index].set(key_duplicate_index, new_pair);
        if (result != CAPU_OK) {
          return result;
        }
      }
    }
    return CAPU_OK;
  }

  template <class Key, class T, class C, class Hash>
  status_t HashTable<Key, T, C, Hash>::get(const Key &key, T* value) {
    if (value == NULL)
      return CAPU_EINVAL;

    status_t result;
    uint_t index = Hash::Digest(key) % mSize;

    int_t key_index = this->getKeyIndexFromBucket(index, key);
    if (key_index < 0) {
      return CAPU_ENOT_EXIST;
    } else {
      Pair<Key, T> pair;
      result = mLists[index].get(key_index, &pair);
      if (result != CAPU_OK) {
        return result;
      }
      //GET THE VALUE
      *value = pair.second;
    }
    return CAPU_OK;
  }

  template <class Key, class T, class C, class Hash>
  status_t HashTable<Key, T, C, Hash>::remove(const Key &key, T* value_old) {
    if (value_old == NULL) {
      return CAPU_EINVAL;
    }
    status_t result;
    uint_t index = Hash::Digest(key) % mSize;

    int_t key_index = this->getKeyIndexFromBucket(index, key);
    if (key_index < 0) {
      return CAPU_ERANGE;
    } else {
      Pair<Key, T> pair;
      //retrieve the value
      result = mLists[index].get(key_index, &pair);
      if (result != CAPU_OK) {
        return result;
      }
      //delete
      result = mLists[index].removeAt(key_index);
      if (result != CAPU_OK) {
        return result;
      }
      mCount--;
      //GET THE VALUE
      *value_old = pair.second;
    }
    return CAPU_OK;
  }

  template <class Key, class T, class C, class Hash>
  uint_t HashTable<Key, T, C, Hash>::count() {
    return mCount;
  }

  template <class Key, class T, class C, class Hash>
  status_t HashTable<Key, T, C, Hash>::clear() {
    for (uint_t i = 0; i < mSize; ++i) {
      mLists[i].clear();
    }
    mCount = 0;
    return CAPU_OK;
  }

  template <class Key, class T, class C, class Hash>
  typename HashTable<Key, T, C, Hash>::Iterator HashTable<Key, T, C, Hash>::begin() {
    return typename HashTable<Key, T, C, Hash>::Iterator(mLists, mSize);
  }

  template <class Key, class T, class C, class Hash>
  HashTable<Key, T, C, Hash>::HashTableIterator::HashTableIterator(List<Pair<Key, T> > * list, uint_t list_size) {
    mCurrentListIndex = 0;
    this->mList = list;
    mMaxListSize = list_size;
    this->mCurrentListIterator = list[mCurrentListIndex].begin();

    //to point the first non-empty one
    for (uint_t i = 0; i < list_size; ++i) {
      if (!mList[i].isEmpty()) {
        mCurrentListIndex = i;
        this->mCurrentListIterator = list[i].begin();
        break;
      }
    }
  }

  template <class Key, class T, class C, class Hash>
  HashTable<Key, T, C, Hash>::HashTableIterator::~HashTableIterator() {

  }

  template <class Key, class T, class C, class Hash>
  bool_t HashTable<Key, T, C, Hash>::HashTableIterator::hasNext() {
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

  template <class Key, class T, class C, class Hash>
  status_t HashTable<Key, T, C, Hash>::HashTableIterator::next(Pair<Key, T>* value) {
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
}

#endif
