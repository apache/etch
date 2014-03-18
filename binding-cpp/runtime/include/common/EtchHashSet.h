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

#include "common/EtchObjectHash.h"
#include "common/EtchComparator.h"
#include "capu/container/HashSet.h"

template <class T, class C = EtchComparator<T>, class H = EtchObjectHash>
class EtchHashSet : public EtchObject {

public:

  class EtchHashSetIterator {
  public:
      friend class EtchHashSet;

      /**
      * destructor
      */
      ~EtchHashSetIterator();

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
      status_t next(T *element = 0);


  private:
    /**
      * Constructor
      */
    EtchHashSetIterator(typename capu::HashSet<T, C, H>::Iterator beginCapuIterator, typename capu::HashSet<T, C, H>::Iterator endCapuIterator);
    typename capu::HashSet<T, C, H>::Iterator mBeginCapuIterator;
    typename capu::HashSet<T, C, H>::Iterator mEndCapuIterator;
      

  };

  class EtchHashSetConstIterator {
  public:
      friend class EtchHashSet;

      /**
      * destructor
      */
      ~EtchHashSetConstIterator();

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
      status_t next(T *element = 0);


  private:
    /**
      * Constructor
      */
    EtchHashSetConstIterator(typename capu::HashSet<T, C, H>::Iterator beginCapuIterator, typename capu::HashSet<T, C, H>::Iterator endCapuIterator);
    typename capu::HashSet<T, C, H>::ConstIterator mBeginCapuConstIterator;
    typename capu::HashSet<T, C, H>::ConstIterator mEndCapuConstIterator;
      

  };
  typedef typename EtchHashSet<T, C, H>::EtchHashSetIterator Iterator;
  typedef typename EtchHashSet<T, C, H>::EtchHashSetConstIterator ConstIterator;

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
   * Copy Constructor
   */
  inline EtchHashSet(const EtchHashSet& other);


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
   * Return const iterator for iterating key value tuples.
   * @return Iterator
   */
  inline ConstIterator begin() const;

  /**
   * Return iterator for iterating key value tuples.
   * @return Iterator
   */
  inline Iterator begin();

private:

  capu::HashSet<T, C, H> mHashSet;

};

template <class T, class C, class H>
inline EtchHashSet<T, C, H>::EtchHashSet() {
  addObjectType(EtchHashSet<T, C, H>::TYPE());
}

template <class T, class C, class H>
inline EtchHashSet<T, C, H>::EtchHashSet(capu::uint32_t size)
 : mHashSet(size) {
  addObjectType(EtchHashSet<T, C, H>::TYPE());
}

template <class T, class C, class H>
const EtchObjectType* EtchHashSet<T, C, H>::TYPE() {
  const static EtchObjectType TYPE(EOTID_SET, NULL);
  return &TYPE;
}

template <class T, class C, class H>
inline EtchHashSet<T, C, H>::EtchHashSet(const EtchHashSet& other)
 : EtchObject(other), mHashSet(other.mHashSet) {

}

template <class T, class C, class H>
inline EtchHashSet<T, C, H>::~EtchHashSet() {

}

template <class T, class C, class H>
inline status_t EtchHashSet<T, C, H>::clear() {
  return mHashSet.clear();
}

template <class T, class C, class H>
inline capu::uint32_t EtchHashSet<T, C, H>::count() {
  return mHashSet.count();
}

template <class T, class C, class H>
inline status_t EtchHashSet<T, C, H>::put(const T &value) {
  return mHashSet.put(value);
}

template <class T, class C, class H>
inline status_t EtchHashSet<T, C, H>::remove(const T &value) {
  return mHashSet.remove(value);
}

template <class T, class C, class H>
inline typename EtchHashSet<T, C, H>::ConstIterator EtchHashSet<T, C, H>::begin() const{
  EtchHashSetConstIterator it(mHashSet.begin(),mHashSet.end());
  return it;
}

template <class T, class C, class H>
inline typename EtchHashSet<T, C, H>::Iterator EtchHashSet<T, C, H>::begin(){
	EtchHashSetIterator it(mHashSet.begin(),mHashSet.end());
	return it;
}

template<class T, class C, class H>
EtchHashSet<T, C, H>::EtchHashSetIterator::EtchHashSetIterator(typename capu::HashSet<T, C, H>::Iterator beginCapuIterator, typename capu::HashSet<T, C, H>::Iterator endCapuIterator) :
  mBeginCapuIterator(beginCapuIterator), mEndCapuIterator(endCapuIterator) { 

}

template<class T, class C, class H>
EtchHashSet<T, C, H>::EtchHashSetConstIterator::EtchHashSetConstIterator(typename capu::HashSet<T, C, H>::Iterator beginCapuConstIterator, typename capu::HashSet<T, C, H>::Iterator endCapuConstIterator) :
	mBeginCapuConstIterator(beginCapuConstIterator), mEndCapuConstIterator(endCapuConstIterator) { 

}

template<class T, class C, class H>
EtchHashSet<T, C, H>::EtchHashSetIterator::~EtchHashSetIterator() { 

}

template<class T, class C, class H>
EtchHashSet<T, C, H>::EtchHashSetConstIterator::~EtchHashSetConstIterator() { 

}

template<class T, class C, class H>
capu::bool_t EtchHashSet<T, C, H>::EtchHashSetIterator::hasNext() { 
  return mBeginCapuIterator != mEndCapuIterator;
}

template<class T, class C, class H>
capu::bool_t EtchHashSet<T, C, H>::EtchHashSetConstIterator::hasNext() { 
	return mBeginCapuConstIterator != mEndCapuConstIterator;
}


template<class T, class C, class H>
status_t EtchHashSet<T, C, H>::EtchHashSetIterator::next(T *element) { 
  if (!hasNext()) {
    return ETCH_ERANGE;
  }

  if (element != NULL) {
    *element = *mBeginCapuIterator;
  }
  mBeginCapuIterator++;
  
  return ETCH_OK;
}

template<class T, class C, class H>
status_t EtchHashSet<T, C, H>::EtchHashSetConstIterator::next(T *element) { 
	if (!hasNext()) {
		return ETCH_ERANGE;
	}

	if (element != NULL) {
		*element = *mBeginCapuConstIterator;
	}
	mBeginCapuConstIterator++;

	return ETCH_OK;
}

typedef capu::SmartPointer<EtchHashSet<EtchObjectPtr> > EtchHashSetPtr;

#endif /* ETCHSET_H */

