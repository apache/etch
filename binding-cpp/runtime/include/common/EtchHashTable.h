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

#ifndef __ETCHHASHTABLE_H__
#define __ETCHHASHTABLE_H__

#include "common/EtchObjectHash.h"
#include "common/EtchComparator.h"
#include "capu/container/HashTable.h"

template <class Key, class T, class C = EtchComparator<Key>, class H = EtchObjectHash >
class EtchHashTable : public EtchObject {
private:
  capu::HashTable<Key, T, C, H> mHashTable;

public:
  typedef typename capu::HashTable<Key, T, C, H>::HashTableEntry HashTableEntry;
  class EtchHashTableIterator {
  public:
      friend class EtchHashTable;

      /**
      * destructor
      */
      ~EtchHashTableIterator();

      /**
      * Check if iterator has next element.
      * @return false if the next of current node that is pointed, is null otherwise true
      */
      capu::bool_t hasNext();

      /**
      * Shifts the iterator to the next position and returns the element if next != NULL
      * @param element
      * @return CAPU_OK if the next element has been gotten
      *
      */
      status_t next(HashTableEntry *element = 0);

  private:
    /**
      * Constructor
      */
    EtchHashTableIterator(typename capu::HashTable<Key, T, C, H>::Iterator mBeginCapuIterator, typename capu::HashTable<Key, T, C, H>::Iterator endCapuIterator);
    typename capu::HashTable<Key, T, C, H>::Iterator mBeginCapuIterator;
    typename capu::HashTable<Key, T, C, H>::Iterator mEndCapuIterator;
      

  };

  class EtchHashTableConstIterator {
  public:
      friend class EtchHashTable;

      /**
      * destructor
      */
      ~EtchHashTableConstIterator();

      /**
      * Check if iterator has next element.
      * @return false if the next of current node that is pointed, is null otherwise true
      */
      capu::bool_t hasNext();

      /**
      * Shifts the iterator to the next position and returns the element if next != NULL
      * @param element
      * @return CAPU_OK if the next element has been gotten
      *
      */
      status_t next(HashTableEntry *element = 0);

  private:
    /**
      * Constructor
      */
    EtchHashTableConstIterator(typename capu::HashTable<Key, T, C, H>::ConstIterator mBeginCapuConstIterator, typename capu::HashTable<Key, T, C, H>::ConstIterator endCapuConstIterator);
    typename capu::HashTable<Key, T, C, H>::ConstIterator mBeginCapuConstIterator;
    typename capu::HashTable<Key, T, C, H>::ConstIterator mEndCapuConstIterator;
      

  };

  typedef typename EtchHashTable<Key, T, C, H>::EtchHashTableIterator Iterator;
  typedef typename EtchHashTable<Key, T, C, H>::EtchHashTableConstIterator ConstIterator;
  

  /**
   * EtchObjectType for EtchHashTable.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs EtchHashTable.
   */
  inline EtchHashTable();

  /**
   * Constructs EtchHashTable.
   */
  inline EtchHashTable(capu::uint32_t size);

  /**
   * Copy Constructor
   */
  inline EtchHashTable(const EtchHashTable& other);

  /**
   * Destructure.
   */
  inline virtual ~EtchHashTable();

  /**
   * put a new value to the EtchHashTable.
   * @param key               Key value
   * @param value             new value that will be put to hash table
   * @param value_old         buffer which will be used to store value of old element
   *
   * @return ETCH_OK if remove is successful
   *         ETCH_EINVAL if value_old is null
   *
   */
  inline status_t put(const Key &key, T value, T* value_old = NULL);

  /**
   * Get value associated with key in the EtchHashTable.
   * @param key       Key
   * @param value     buffer which will be used to return the found element
   *
   * @return ETCH_OK if get is successful performed
   *         ETCH_EINVAL if value is null
   *         ETCH_ENOT_EXIST if there is no existing pair with specified key
   *
   */
  inline status_t get(const Key &key, T* value);

  /**
   * Remove value associated with key in the EtchHashTable.
   *
   * @param key               Key value
   * @param value_old         buffer which will be used to store value of removed element
   *
   * @return ETCH_OK if remove is successful
   *         ETCH_EINVAL if value_old is null
   *         ETCH_ERANGE if the pair with specified key does not exist in hash table
   *
   */
  inline status_t remove(const Key &key, T* value_old);

  /**
   * Returns count of the EtchHashTable.
   * @return number of element in hash table
   */
  inline capu::uint_t count();

  /**
   * Clear all key and values of the EtchHashTable.
   *
   * @return ETCH_OK if all elements in list have been deleted
   */
  inline status_t clear();

  /**
   * Return iterator for iterating key value tuples.
   * @return Const Iterator
   */
  inline ConstIterator begin() const;


  /**
   * Return iterator for iterating key value tuples.
   * @return Iterator
   */
  inline Iterator begin();


};

template <class Key, class T, class C, class H>
const EtchObjectType* EtchHashTable<Key, T, C, H>::TYPE() {
  const static EtchObjectType TYPE(EOTID_HASHTABLE, NULL);
  return &TYPE;
}

template <class Key, class T, class C, class H>
inline EtchHashTable<Key, T, C, H>::EtchHashTable()
: mHashTable(ETCH_DEFAULT_HASH_TABLE_BIT_SIZE) {
  addObjectType(EtchHashTable<Key, T, C, H>::TYPE());
}

template <class Key, class T, class C, class H>
inline EtchHashTable<Key, T, C, H>::EtchHashTable(capu::uint32_t size)
: mHashTable(size) {
  addObjectType(EtchHashTable<Key, T, C, H>::TYPE());
}

template <class Key, class T, class C, class H>
inline EtchHashTable<Key, T, C, H>::EtchHashTable(const EtchHashTable& other)
: EtchObject(other), mHashTable(other.mHashTable) {
}

template <class Key, class T, class C, class H>
inline EtchHashTable<Key, T, C, H>::~EtchHashTable() {

}

template <class Key, class T, class C, class H>
inline status_t EtchHashTable<Key, T, C, H>::put(const Key &key, T value, T* value_old) {
  return mHashTable.put(key, value, value_old);
}

template <class Key, class T, class C, class H>
inline status_t EtchHashTable<Key, T, C, H>::get(const Key &key, T* value) {
  status_t status = ETCH_EINVAL;
  if (value != NULL) {
    *value = mHashTable.at(key,&status);
  }
  return status;
}

template <class Key, class T, class C, class H>
inline status_t EtchHashTable<Key, T, C, H>::remove(const Key &key, T* value_old) {
  return mHashTable.remove(key, value_old);
}

template <class Key, class T, class C, class H>
inline capu::uint_t EtchHashTable<Key, T, C, H>::count() {
  return mHashTable.count();
}

template <class Key, class T, class C, class H>
inline status_t EtchHashTable<Key, T, C, H>::clear() {
  mHashTable.clear();
  return ETCH_OK;
}

template <class Key, class T, class C, class H>
inline typename EtchHashTable<Key, T, C, H>::ConstIterator EtchHashTable<Key, T, C, H>::begin() const {
  EtchHashTableConstIterator it(mHashTable.begin(),mHashTable.end());
  return it;
}

template <class Key, class T, class C, class H>
inline typename EtchHashTable<Key, T, C, H>::Iterator EtchHashTable<Key, T, C, H>::begin() {
	EtchHashTableIterator it(mHashTable.begin(),mHashTable.end());
	return it;
}

template<class Key, class T, class C, class H>
EtchHashTable<Key, T, C, H>::EtchHashTableIterator::EtchHashTableIterator(typename capu::HashTable<Key, T, C, H>::Iterator beginCapuIterator, typename capu::HashTable<Key, T, C, H>::Iterator endCapuIterator) :
  mBeginCapuIterator(beginCapuIterator), mEndCapuIterator(endCapuIterator) { 

}

template<class Key, class T, class C, class H>
EtchHashTable<Key, T, C, H>::EtchHashTableConstIterator::EtchHashTableConstIterator(typename capu::HashTable<Key, T, C, H>::ConstIterator beginCapuConstIterator, typename capu::HashTable<Key, T, C, H>::ConstIterator endCapuConstIterator) :
	mBeginCapuConstIterator(beginCapuConstIterator), mEndCapuConstIterator(endCapuConstIterator) { 

}

template<class Key, class T, class C, class H>
EtchHashTable<Key, T, C, H>::EtchHashTableIterator::~EtchHashTableIterator() { 

}

template<class Key, class T, class C, class H>
EtchHashTable<Key, T, C, H>::EtchHashTableConstIterator::~EtchHashTableConstIterator() { 

}

template<class Key, class T, class C, class H>
capu::bool_t EtchHashTable<Key, T, C, H>::EtchHashTableIterator::hasNext() { 
  return mBeginCapuIterator != mEndCapuIterator;
}

template<class Key, class T, class C, class H>
capu::bool_t EtchHashTable<Key, T, C, H>::EtchHashTableConstIterator::hasNext() { 
	return mBeginCapuConstIterator != mEndCapuConstIterator;
}

template<class Key, class T, class C, class H>
status_t EtchHashTable<Key, T, C, H>::EtchHashTableIterator::next(HashTableEntry *element) { 
  if (!hasNext()) {
    return ETCH_ERANGE;
  }

  if (element != NULL) {
    *element = *mBeginCapuIterator;
  }
  mBeginCapuIterator++;
  
  return ETCH_OK;
}

template<class Key, class T, class C, class H>
status_t EtchHashTable<Key, T, C, H>::EtchHashTableConstIterator::next(HashTableEntry *element) { 
	if (!hasNext()) {
		return ETCH_ERANGE;
	}

	if (element != NULL) {
		*element = *mBeginCapuConstIterator;
	}
	mBeginCapuConstIterator++;

	return ETCH_OK;
}

typedef capu::SmartPointer<EtchHashTable<EtchObjectPtr, EtchObjectPtr> > EtchHashTablePtr;

#endif
