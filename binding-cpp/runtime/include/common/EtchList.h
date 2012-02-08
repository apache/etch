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
#include "common/EtchComparator.h"
#include "capu/container/List.h"
#include "capu/util/Traits.h"

template <class T, class C = EtchComparator<T> >
class EtchList : public EtchObject {
private:
  typedef typename capu::ReferenceType<T>::Type Reference;

  capu::List<T, C> mList;

public:

  typedef typename capu::List<T, C>::Iterator Iterator;
  static const capu::int32_t TYPE_ID = EOTID_LIST;

  /**
   * Default Constructor
   */
  EtchList();

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
  status_t add(const Reference element);

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
  status_t add(capu::int32_t index, const Reference element);

  /**
   * remove the element in the specified index and if the element_old
   * parameter is not NULL, the removed element will be put to element_old
   * @param index index of element that will be removed
   * @param elementOld the buffer which will keep the copy of the removed element
   * @return ETCH_EINVAL invalid index
   *         ETCH_OK if the element is successfully removed
   */
  status_t removeAt(capu::int32_t index, T* elementOld = NULL);

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
  capu::bool_t isEmpty();

  /**
   * returns an iterator pointing to the beginning of list
   * @return iterator
   */
  Iterator begin();

  /**
   * finds the index of given element in the link list
   * if you are using an object you need to overload == operator
   *
   * @param element the value that will be searched
   * @return -1 if the value either does not exist or given value is NULL
   *          otherwise index of value on linked list
   */
  capu::int32_t find(const Reference element);

  /**
   *
   * @param index the index of element that will be replaced
   * @param element element that will be overwritten to existing place
   * @param elementOld the buffer to keep the existing
   * @return ETCH_EINVAL if the index is not valid
   *         ETCH_OK otherwise
   */
  status_t set(capu::int32_t index, const Reference element, T* elementOld = NULL);

  /**
   * check that if the list contains the given parameter or not
   * if you are using an object you need to overload == operator
   * @param element element that will be checked
   * @return true list contains it
   *         false otherwise
   */
  capu::bool_t contains(const Reference element);

  /**
   * removes all elements from linked list
   *
   * @return ETCH_OK if all of the elements are deleted
   *
   */
  status_t clear();

};

template<class T, class C>
EtchList<T, C>::EtchList()
: EtchObject(EtchList<T, C>::TYPE_ID) {

}

template<class T, class C>
EtchList<T, C>::~EtchList() {

}

template<class T, class C>
status_t EtchList<T, C>::add(const Reference element) {
  return mList.add(element);
}

template<class T, class C>
status_t EtchList<T, C>::add(capu::int32_t index, const Reference element) {
  return mList.add(index, element);
}

template<class T, class C>
typename EtchList<T, C>::Iterator EtchList<T, C>::begin() {
  return mList.begin();
}

template<class T, class C>
status_t EtchList<T, C>::clear() {
  return mList.clear();
}

template<class T, class C>
capu::bool_t EtchList<T, C>::contains(const Reference element) {
  return mList.contains(element);
}

template<class T, class C>
capu::int32_t EtchList<T, C>::find(const Reference element) {
  return mList.find(element);
}

template<class T, class C>
status_t EtchList<T, C>::get(capu::int32_t index, T* result) {
  return mList.get(index, result);
}

template<class T, class C>
capu::bool_t EtchList<T, C>::isEmpty() {
  return mList.isEmpty();
}

template<class T, class C>
status_t EtchList<T, C>::removeAt(capu::int32_t index, T* elementOld) {
  return mList.removeAt(index, elementOld);
}

template<class T, class C>
capu::int32_t EtchList<T, C>::size() {
  return mList.size();
}

template<class T, class C>
capu::int32_t EtchList<T, C>::set(capu::int32_t index, const Reference element, T* elementOld) {
  return mList.set(index, element, elementOld);
}
#endif /* ETCHDOUBLELINKEDLIST_H */

