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

#ifndef __ETCHSET_H__
#define __ETCHSET_H__
#include "common/EtchObject.h"
#include "common/EtchObjectType.h"
#include "common/EtchError.h"
#include "common/EtchObjectHash.h"
#include "common/EtchComparator.h"
#include "capu/container/HashSet.h"



template <class T, class H = EtchObjectHash, class C = EtchComparator<T> >
class EtchHashSet : public EtchObject {

public:

  typedef typename capu::HashSet<T, C, H>::Iterator Iterator;

  /**
   * EtchObjectType for EtchHashSet.
   */
  static const EtchObjectType* TYPE();

  /**
   * Default Constructor
   */
  inline EtchHashSet();

  /**
   * Parameterized Constructor
   * @param size size of initial HashSet
   */
  inline EtchHashSet(capu::uint32_t size);

  /**
   * Destructor
   */
  inline ~EtchHashSet();

  /**
   * put a new value to the hashset.
   *
   * @param value             new value that will be put to hash table
   *
   * @return ETCH_OK if remove is successful
   *         ETCH_ERROR if value already exists in the set
   *
   */
  inline status_t put(const T &value);

  /**
   * Remove value associated with key in the hashset.
   *
   * @param value             value that will be removed
   *
   * @return ETCH_OK if remove is successful
   *         ETCH_ERANGE if specified value does not exist in hashset
   *
   */
  inline status_t remove(const T &value);

  /**
   * Returns count of the hashset.
   * @return number of element in hash table
   */
  inline capu::uint32_t count();

  /**
   * Clear all values of the hashset.
   *
   * @return ETCH_OK if all elements in list have been deleted
   */
  inline status_t clear();

  /**
   * Return iterator for iterating key value tuples.
   * @return Iterator
   */
  inline Iterator begin();

private:

  capu::HashSet<T, C, H> mHashSet;

};

template <class T, class H, class C>
inline EtchHashSet<T, H, C>::EtchHashSet()
: EtchObject(EtchHashSet<T, H, C>::TYPE()) {

}

template <class T, class H, class C>
inline EtchHashSet<T, H, C>::EtchHashSet(capu::uint32_t size)
: EtchObject(&EtchHashSet<T, H, C>::TYPE), mHashSet(size) {

}

template <class T, class H, class C>
const EtchObjectType* EtchHashSet<T, H, C>::TYPE() {
  const static EtchObjectType TYPE(EOTID_SET, NULL);
  return &TYPE;
}

template <class T, class H, class C>
inline EtchHashSet<T, H, C>::~EtchHashSet() {

}

template <class T, class H, class C>
inline status_t EtchHashSet<T, H, C>::clear() {
  return mHashSet.clear();
}

template <class T, class H, class C>
inline capu::uint32_t EtchHashSet<T, H, C>::count() {
  return mHashSet.count();
}

template <class T, class H, class C>
inline status_t EtchHashSet<T, H, C>::put(const T &value) {
  return mHashSet.put(value);
}

template <class T, class H, class C>
inline status_t EtchHashSet<T, H, C>::remove(const T &value) {
  return mHashSet.remove(value);
}

template <class T, class H, class C>
inline typename EtchHashSet<T, H, C>::Iterator EtchHashSet<T, H, C>::begin() {
  return mHashSet.begin();
}

typedef capu::SmartPointer<EtchHashSet<EtchObjectPtr> > EtchHashSetPtr;

#endif /* ETCHSET_H */

