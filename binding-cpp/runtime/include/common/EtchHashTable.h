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

#include "common/EtchObject.h"
#include "common/EtchError.h"
#include "common/EtchHash.h"
#include "common/EtchComparator.h"
#include "capu/container/HashTable.h"

template <class Key, class T, class H = EtchHash, class C = EtchComparator<Key> >
class EtchHashTable : public EtchObject {
private:
  capu::HashTable<Key, T, C, H> mHashTable;

public:
  typedef typename capu::Pair<Key, T> Pair;
  typedef typename capu::HashTable<Key, T, C, H >::Iterator Iterator;
  static const EtchObjectType TYPE;

  /**
   * Constructs EtchHashTable.
   */
  inline EtchHashTable();

  /**
   * Constructs EtchHashTable.
   */
  inline EtchHashTable(capu::uint64_t size);

  /**
   * Destructure.
   */
  inline ~EtchHashTable();

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
  inline capu::uint64_t count();

  /**
   * Clear all key and values of the EtchHashTable.
   *
   * @return ETCH_OK if all elements in list have been deleted
   */
  inline status_t clear();

  /**
   * Return iterator for iterating key value tuples.
   * @return Iterator
   */
  inline Iterator begin();

};

template <class Key, class T, class H, class C>
const EtchObjectType EtchHashTable<Key, T, H, C>::TYPE(EOTID_HASHTABLE, NULL);

template <class Key, class T, class H, class C>
inline EtchHashTable<Key, T, H, C>::EtchHashTable()
: EtchObject(&EtchHashTable<Key, T, H, C>::TYPE), mHashTable(ETCH_DEFAULT_HASH_TABLE_SIZE) {

}

template <class Key, class T, class H, class C>
inline EtchHashTable<Key, T, H, C>::EtchHashTable(capu::uint64_t size)
: EtchObject(&EtchHashTable<Key, T, H, C>::TYPE), mHashTable(size) {

}

template <class Key, class T, class H, class C>
inline EtchHashTable<Key, T, H, C>::~EtchHashTable() {

}

template <class Key, class T, class H, class C>
inline status_t EtchHashTable<Key, T, H, C>::put(const Key &key, T value, T* value_old) {
  return mHashTable.put(key, value, value_old);
}

template <class Key, class T, class H, class C>
inline status_t EtchHashTable<Key, T, H, C>::get(const Key &key, T* value) {
  return mHashTable.get(key, value);
}

template <class Key, class T, class H, class C>
inline status_t EtchHashTable<Key, T, H, C>::remove(const Key &key, T* value_old) {
  return mHashTable.remove(key, value_old);
}

template <class Key, class T, class H, class C>
inline capu::uint64_t EtchHashTable<Key, T, H, C>::count() {
  return mHashTable.count();
}

template <class Key, class T, class H, class C>
inline status_t EtchHashTable<Key, T, H, C>::clear() {
  return mHashTable.clear();
}

template <class Key, class T, class H, class C>
inline typename EtchHashTable<Key, T, H, C>::Iterator EtchHashTable<Key, T, H, C>::begin() {
  return mHashTable.begin();
}

#endif
