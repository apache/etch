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

#ifndef __ETCHNATIVEARRAY_H__
#define __ETCHNATIVEARRAY_H__

#include "common/EtchObject.h"
#include "common/EtchError.h"

template<class T>
class EtchNativeArray :
public EtchObject {
public:

  /**
   * EtchObjectType for EtchNativeArray.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a EtchNativeArray object.
   */
  EtchNativeArray(capu::int32_t length, const EtchObjectType* content_type_id);

  /**
   * Constructs a EtchNativeArray object.
   */
  EtchNativeArray(capu::int32_t length, T* array, const EtchObjectType* content_type_id);

  /**
   * Destructor for Etch Nativearray.
   * Frees array if content_owned was set.
   */
  virtual ~EtchNativeArray();

  /**
   * returns the TypeID of the content of this native array.
   */
  const EtchObjectType* getContentType();

  /**
   * gets the EtchObject at index i in result
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t get(capu::int32_t index, T* result);

  /**
   * gets a block of EtchObject at index i in result
   * @param index The start index of the array
   * @param buffer The destination buffer
   * @param buffer_size The size of the destination buffer
   * @param offset The offset of the buffer where the data should be copied to
   * @param count The number of elements
   * returns ETCH_INVAL if the buffer is NULL
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t get(capu::int32_t index, T* buffer, capu::int32_t buffer_size, capu::int32_t offset, capu::int32_t count);


  /**
   * sets the EtchObject at index i in result
   * if content_owned, old value will be freed.
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t set(capu::int32_t index, T value);

  /**
   * sets a block of the EtchObject at index i in result
   * if content_owned, old value will be freed.
   * @param index The start index of the array
   * @param buffer The source buffer
   * @param buffer_size The size of the source buffer
   * @param offset The offset of the buffer from where on the data should be copied
   * @param count The number of elements
   * returns ETCH_INVAL if the buffer is NULL
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t set(capu::int32_t index, T* buffer, capu::int32_t buffer_size, capu::int32_t offset, capu::int32_t count);

  /**
   * Returns the length of array
   */
  capu::int32_t getLength();

  /**
   * Returns the pointer to the beginning of array
   */
  T* getArray();
  /**
   * set the content type id will be deleted or not
   */
  void setDeleteMode(capu::bool_t isDelete);

private:
  T* mArray;
  capu::bool_t mIsDeleteContentType;
  capu::int32_t mLength;
  const EtchObjectType* mContentType;
};

template<class T>
const EtchObjectType* EtchNativeArray<T>::TYPE() {
  const static EtchObjectType TYPE(EOTID_NATIVE_ARRAY, EtchObjectType::getType<T>());
  return &TYPE;
}

template<class T>
EtchNativeArray<T>::EtchNativeArray(capu::int32_t length, const EtchObjectType* content_type_id)
: EtchObject(EtchNativeArray::TYPE()), mIsDeleteContentType(false),
mLength(length) {
  mArray = new T[length];
  mContentType = content_type_id;
}

template<class T>
EtchNativeArray<T>::EtchNativeArray(capu::int32_t length, T* array, const EtchObjectType* content_type_id)
: EtchObject(EtchNativeArray::TYPE()), mArray(array), mIsDeleteContentType(false),
mLength(length) {
  mContentType = content_type_id;
}

template<class T>
EtchNativeArray<T>::~EtchNativeArray() {
  delete[] mArray;

  if (mIsDeleteContentType && (mContentType != NULL)) {
    const EtchObjectType * content = mContentType->getObjectComponentType();
    while (content) {
      const EtchObjectType * tmp = content;
      content = content->getObjectComponentType();
      delete tmp;
    }
    delete mContentType;
  }
}

template<class T>
status_t EtchNativeArray<T>::set(capu::int32_t index, T value) {
  if (0 <= index && index < mLength) {
    mArray[index] = value;
    return ETCH_OK;
  }
  return ETCH_ERANGE;
}

template<class T>
const EtchObjectType * EtchNativeArray<T>::getContentType() {
  return mContentType;
}

template<class T >
T * EtchNativeArray<T>::getArray() {
  return mArray;
}

template<class T >
status_t EtchNativeArray<T>::get(capu::int32_t index, T * result) {
  if (result == NULL) {
    return ETCH_EINVAL;
  } else if (0 <= index && index < mLength) {
    *result = mArray[index];
    return ETCH_OK;
  }
  return ETCH_ERANGE;
}

template<class T >
status_t EtchNativeArray<T>::set(capu::int32_t index, T* buffer, capu::int32_t buffer_size, capu::int32_t offset, capu::int32_t count) {
  if (buffer == NULL) {
    return ETCH_EINVAL;
  }
  if (0 <= index && index < mLength && (mLength - index) >= count && buffer_size <= (mLength - index) && buffer_size >= count && (offset + count) <= buffer_size) {
    for (int i = 0; i < count - offset; i++, index++) {
      mArray[index] = buffer[offset + i];
    }

    return ETCH_OK;
  }
  return ETCH_ERANGE;
}

template<class T >
status_t EtchNativeArray<T>::get(capu::int32_t index, T* buffer, capu::int32_t buffer_size, capu::int32_t offset, capu::int32_t count) {
  if (buffer == NULL) {
    return ETCH_EINVAL;
  }
  if (0 <= index && index < mLength && (mLength - index) >= count && buffer_size >= (mLength - index) && buffer_size >= count && (offset + count) <= buffer_size) {

    for (int i = 0; i < count - offset; i++, index++) {
      buffer[offset + i] = mArray[index];
    }

    return ETCH_OK;
  }
  return ETCH_ERANGE;
}

template<class T >
capu::int32_t EtchNativeArray<T>::getLength() {
  return mLength;
}

template<class T>
void EtchNativeArray<T>::setDeleteMode(capu::bool_t isDelete) {
  mIsDeleteContentType = isDelete;
}
#endif //__ETCHNATIVEARRAY_H__
