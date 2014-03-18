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

#include "capu/container/List.h"
#include "common/EtchComparator.h"

template <class T, class A = capu::Allocator<capu::GenericListNode<T> >, class C = EtchComparator<T> >
class EtchList : public EtchObject {
private:
  capu::List<T, A, C> mList;

public:

  class EtchListIterator {
  public:
      friend class EtchList;

      /**
      * destructor
      */
      ~EtchListIterator();

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
      status_t next(T* element = 0);

      /**
        * Get current iterator element.
        * @param element
        * @return ETCH_OK if the current element has been gotten
        */
        status_t current(T& element);

      /**
      * Returns the index of the current element.
      * @return The index of the current element. If there is no current element, the return value is undefined.
      */
      capu::uint32_t currentIndex();

  private:
    /**
      * Constructor
      */
    EtchListIterator(typename capu::List<T, A, C>::Iterator beginCapuListIterator, typename capu::List<T, A, C>::Iterator endCapuListIterator);
    typename capu::List<T, A, C>::Iterator mBeginCapuListIterator;
    typename capu::List<T, A, C>::Iterator mEndCapuListIterator;      

  };

  class EtchListConstIterator {
  public:
      friend class EtchList;

      /**
      * destructor
      */
      ~EtchListConstIterator();

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
      status_t next(T* element = 0);

      /**
        * Get current iterator element.
        * @param element
        * @return ETCH_OK if the current element has been gotten
        */
        status_t current(T& element);

      /**
      * Returns the index of the current element.
      * @return The index of the current element. If there is no current element, the return value is undefined.
      */
      capu::uint32_t currentIndex();

  private:
    /**
      * Constructor
      */
    EtchListConstIterator(typename capu::List<T, A, C>::ConstIterator beginCapuListConstIterator, typename capu::List<T, A, C>::ConstIterator endCapuListConstIterator);
    typename capu::List<T, A, C>::ConstIterator mBeginCapuListConstIterator;
    typename capu::List<T, A, C>::ConstIterator mEndCapuListConstIterator;      

  };

public:
  typedef typename EtchList<T, A, C>::EtchListIterator Iterator;
  typedef typename EtchList<T, A, C>::EtchListConstIterator ConstIterator;

  /**
   * EtchObjectType for EtchList.
   */
  static const EtchObjectType* TYPE();

  /**
   * Default Constructor
   */
  EtchList();

  /**
   * Copy Constructor
   */
  EtchList(const EtchList& other);

  /**
   * Destructor
   */
  ~EtchList();

  /**
   * Add element to the end of list
   * @param element element that will be added
   * @return ETCH_ENO_MEMORY if allocation of element is failed
   *         ETCH_OK if the element is successfully added
   */
  status_t add(const T &element);

  /**
   * It will insert new element to specified position only the element at specified index with given element
   *
   * @param index index of element which will be inserted
   * @param element new value that will replace the old value
   *
   * @return ETCH_EINVAL if given index is invalid.
   *         ETCH_ENO_MEMORY if allocation of element is failed
   *         ETCH_OK if the element is successfully added
   *         ETCH_ERROR otherwise
   */
  status_t add(capu::int32_t index, const T &element);

  /**
   * remove the element in the specified index and if the element_old
   * parameter is not NULL, the removed element will be put to element_old
   * @param index index of element that will be removed
   * @param elementOld the buffer which will keep the copy of the removed element
   * @return ETCH_EINVAL invalid index
   *         ETCH_OK if the element is successfully removed
   */
  status_t removeAt(Iterator& it, T* elementOld = NULL);

  /**
   * get a single element on specified index
   * @param index index of element that will be get
   * @param result the buffer that the retrieved element will be stored
   * @return ETCH_EINVAL invalid index
   *         ETCH_OK otherwise
   */
  status_t get(capu::int32_t index, T* result);

  /**
   * return size of list
   * @return return the size of list
   */
  capu::int32_t size();

  /**
   * check the list is empty or not
   * @return true if empty
   *         false otherwise
   */
  capu::bool_t isEmpty() const;

  /**
   * returns an iterator pointing to the beginning of list
   * @return iterator
   */
  Iterator begin();

    /**
   * returns a const iterator pointing to the beginning of list
   * @return const iterator
   */
  ConstIterator begin() const;

  /**
   * finds the index of given element in the link list
   * if you are using an object you need to overload == operator
   *
   * @param element the value that will be searched
   * @return Iterator to the searched element if the element is found
   *         otherwise Iterator to the end of the list
   */
  Iterator find(const T &element);


  /**
   * finds the index of given element in the link list
   * if you are using an object you need to overload == operator
   *
   * @param element the value that will be searched
   * @return Const Iterator to the searched element if the element is found
   *         otherwise Iterator to the end of the list
   */
  ConstIterator find(const T &element) const;

  /**
   *
   * @param index the index of element that will be replaced
   * @param element element that will be overwritten to existing place
   * @param elementOld the buffer to keep the existing
   * @return ETCH_EINVAL if the index is not valid
   *         ETCH_OK otherwise
   */
  status_t set(capu::int32_t index, const T &element, T* elementOld = NULL);

  /**
   * check that if the list contains the given parameter or not
   * if you are using an object you need to overload == operator
   * @param element element that will be checked
   * @return true list contains it
   *         false otherwise
   */
  capu::bool_t contains(const T &element);

  /**
   * removes all elements from linked list
   *
   * @return ETCH_OK if all of the elements are deleted
   *
   */
  status_t clear();

};

template<class T, class A, class C>
const EtchObjectType* EtchList<T, A, C>::TYPE() {
  const static EtchObjectType TYPE(EOTID_LIST, NULL);
  return &TYPE;
}

template<class T, class A, class C>
EtchList<T, A, C>::EtchList() {
  addObjectType(EtchList<T, A, C>::TYPE());
}

template<class T, class A, class C>
EtchList<T, A, C>::EtchList(const EtchList& other)
 : EtchObject(other), mList(other.mList) {
}


template<class T, class A, class C>
EtchList<T, A, C>::~EtchList() {

}

template<class T, class A, class C>
status_t EtchList<T, A, C>::add(const T &element) {
  return mList.insert(element);
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::add(capu::int32_t index, const T &element) {
  return mList.insert(index, element);
}

template<class T, class A, class C>
typename EtchList<T, A, C>::Iterator EtchList<T, A, C>::begin() {
  return EtchListIterator(mList.begin(), mList.end());
}

template<class T, class A, class C>
typename EtchList<T, A, C>::ConstIterator EtchList<T, A, C>::begin() const {
  return EtchListConstIterator(mList.begin(), mList.end());
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::clear() {
  mList.clear();
  return ETCH_OK;
}

template<class T, class A, class C>
capu::bool_t EtchList<T, A, C>::contains(const T &element) {
  return mList.contains(element);
}

template<class T, class A, class C>
typename EtchList<T, A, C>::Iterator EtchList<T, A, C>::find(const T &element) {
  return EtchListIterator(mList.find(element), mList.end());
}

template<class T, class A, class C>
typename EtchList<T, A, C>::ConstIterator EtchList<T, A, C>::find(const T &element) const {
	return EtchListConstIterator(mList.find(element), mList.end());
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::get(capu::int32_t index, T* result) {
  status_t status = ETCH_EINVAL;
  if (result != NULL) {
    *result = mList.get(index, &status);
  }
  return status;
}

template<class T, class A, class C>
capu::bool_t EtchList<T, A, C>::isEmpty() const {
  return mList.isEmpty();
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::removeAt(typename EtchList<T, A, C>::Iterator& it, T* elementOld) {
  status_t status = mList.erase(it.mBeginCapuListIterator, elementOld);
  return status;
}

template<class T, class A, class C>
capu::int32_t EtchList<T, A, C>::size() {
  return mList.size();
}

template<class T, class A, class C>
capu::int32_t EtchList<T, A, C>::set(capu::int32_t index, const T &element, T* elementOld) {
  return mList.set(index, element, elementOld);
}


template<class T, class A, class C>
EtchList<T, A, C>::EtchListIterator::EtchListIterator(typename capu::List<T, A, C>::Iterator beginCapuListIterator, typename capu::List<T, A, C>::Iterator endCapuListIterator) :
  mBeginCapuListIterator(beginCapuListIterator), mEndCapuListIterator(endCapuListIterator) { 

}

template<class T, class A, class C>
EtchList<T, A, C>::EtchListConstIterator::EtchListConstIterator(typename capu::List<T, A, C>::ConstIterator beginCapuListConstIterator, typename capu::List<T, A, C>::ConstIterator endCapuListConstIterator) :
	mBeginCapuListConstIterator(beginCapuListConstIterator), mEndCapuListConstIterator(endCapuListConstIterator) { 

}

template<class T, class A, class C>
EtchList<T, A, C>::EtchListIterator::~EtchListIterator() { 

}

template<class T, class A, class C>
EtchList<T, A, C>::EtchListConstIterator::~EtchListConstIterator() { 

}

template<class T, class A, class C>
capu::bool_t EtchList<T, A, C>::EtchListIterator::hasNext() { 
  return mBeginCapuListIterator != mEndCapuListIterator;
}

template<class T, class A, class C>
capu::bool_t EtchList<T, A, C>::EtchListConstIterator::hasNext() { 
	return mBeginCapuListConstIterator != mEndCapuListConstIterator;
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::EtchListIterator::next(T *element) { 
  if (!hasNext()) {
    return ETCH_ERROR;
  }
  if (element != NULL) {
    *element = *mBeginCapuListIterator;
  }
  mBeginCapuListIterator++;
  return ETCH_OK;
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::EtchListConstIterator::next(T *element) { 
	if (!hasNext()) {
		return ETCH_ERROR;
	}
	if (element != NULL) {
		*element = *mBeginCapuListConstIterator;
	}
	mBeginCapuListConstIterator++;
	return ETCH_OK;
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::EtchListIterator::current(T& element) { 
  if (hasNext()) {
    element = *mBeginCapuListIterator;
    return ETCH_OK;
  }
  return ETCH_ERROR;
}

template<class T, class A, class C>
status_t EtchList<T, A, C>::EtchListConstIterator::current(T& element) { 
	if (hasNext()) {
		element = *mBeginCapuListConstIterator;
		return ETCH_OK;
	}
	return ETCH_ERROR;
}

template<class T, class A, class C>
capu::uint32_t EtchList<T, A, C>::EtchListIterator::currentIndex() { 
  return mBeginCapuListIterator.currentIndex();
}

template<class T, class A, class C>
capu::uint32_t EtchList<T, A, C>::EtchListConstIterator::currentIndex() { 
	return mBeginCapuListConstIterator.currentIndex();
}

typedef capu::SmartPointer<EtchList<EtchObjectPtr> > EtchListPtr;

#endif /* ETCHDOUBLELINKEDLIST_H */

