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

class Pos {
public:

  Pos(capu::int32_t i1, capu::int32_t i2 = -1, capu::int32_t i3 = -1, capu::int32_t i4 = -1, capu::int32_t i5 = -1, capu::int32_t i6 = -1, capu::int32_t i7 = -1, capu::int32_t i8 = -1, capu::int32_t i9 = -1) : size(0) {
    if (i1 != -1) {
      pos[0] = i1;
      size = 1;
    } else {
      return;
    }
    if (i2 != -1) {
      pos[1] = i2;
      size = 2;
    } else {
      return;
    }
    if (i3 != -1) {
      pos[2] = i3;
      size = 3;
    } else {
      return;
    }
    if (i4 != -1) {
      pos[3] = i4;
      size = 4;
    } else {
      return;
    }
    if (i5 != -1) {
      pos[4] = i5;
      size = 5;
    } else {
      return;
    }
    if (i6 != -1) {
      pos[5] = i6;
      size = 6;
    } else {
      return;
    }
    if (i7 != -1) {
      pos[6] = i7;
      size = 7;
    } else {
      return;
    }
    if (i8 != -1) {
      pos[7] = i8;
      size = 8;
    } else {
      return;
    }
    if (i9 != -1) {
      pos[8] = i9;
      size = 9;
    } else {
      return;
    }
  }

  capu::int32_t pos[9];
  capu::int32_t size;
};

template <class T>
class EtchArray {
public:
  capu::int32_t mDim;
  capu::int32_t mLen;
  void* mArray;

  EtchArray()
  : mDim(0), mLen(0), mArray(NULL) {

  }

  EtchArray(capu::int32_t dim, capu::int32_t length)
  : mDim(dim), mLen(length), mArray(NULL) {
    if (dim == 1) {
      mArray = new T[length];
    } else {
      mArray = new EtchArray[length];
    }
  }

  EtchArray(capu::int32_t dim, capu::int32_t length, T* array)
  : mDim(dim), mLen(length), mArray(array) {

  }

  virtual ~EtchArray() {
  }
};

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
  EtchNativeArray(capu::int32_t dim, capu::int32_t length);

  /**
   * Constructs a EtchNativeArray object.
   */
  EtchNativeArray(capu::int32_t dim, capu::int32_t length, T* array);

  /**
   * Destructor for Etch Nativearray.
   * Frees array if content_owned was set.
   */
  virtual ~EtchNativeArray();

  /**
   * gets the EtchObject at index i in result
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t get(Pos pos, T *result);

  status_t get(Pos pos, capu::int32_t length, T* data, capu::int32_t offset);

  /**
   * sets the EtchObject at index i in result
   * if content_owned, old value will be freed.
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t set(Pos pos, T result);

  status_t set(Pos pos, capu::int32_t length, T* data, capu::int32_t offset);

  /**
   * Returns the length of array
   */
  capu::int32_t getLength();

  /**
   * Returns the length of array
   */
  capu::int32_t getDim();

  /**
   * Returns the pointer to the beginning of array
   */
  T* getArray();

  status_t createArray(Pos pos, capu::int32_t dims, capu::int32_t length);

private:

  void deallocate(EtchArray<T> *array);
  EtchArray<T> mArray;
};

template<class T>
const EtchObjectType* EtchNativeArray<T>::TYPE() {
  const static EtchObjectType TYPE(EOTID_NATIVE_ARRAY, EtchObjectType::getType<T>());
  return &TYPE;
}

template<class T>
EtchNativeArray<T>::EtchNativeArray(capu::int32_t dim, capu::int32_t length)
: EtchObject(EtchNativeArray<T>::TYPE()), mArray(dim, length) {

}

template<class T>
EtchNativeArray<T>::EtchNativeArray(capu::int32_t dim, capu::int32_t length, T* array)
: EtchObject(EtchNativeArray<T>::TYPE()), mArray(dim, length, array) {

}

template<class T>
EtchNativeArray<T>::~EtchNativeArray() {
  deallocate(&mArray);
}

template<class T>
T * EtchNativeArray<T>::getArray() {
  return mArray;
}

template<class T>
status_t EtchNativeArray<T>::get(Pos pos, T * result) {
  if (mArray.mArray == NULL || result == NULL || (pos.size != mArray.mDim)) {
    return ETCH_EINVAL;
  }
  capu::int32_t i = 0;
  void * array = mArray.mArray;
  for (i = 1; i < mArray.mDim; i++) {
    array = ((EtchArray<T>*) array)[pos.pos[i]].mArray;
  }
  *result = ((T*) array)[pos.pos[i - 1]];
  return ETCH_OK;
}

template<class T >
status_t EtchNativeArray<T>::set(Pos pos, T result) {
  if (mArray.mArray == NULL || (pos.size != mArray.mDim)) {
    return ETCH_EINVAL;
  }
  capu::int32_t i = 0;
  void * array = mArray.mArray;
  for (i = 1; i < mArray.mDim; i++) {
    array = ((EtchArray<T>*) array)[pos.pos[i - 1]].mArray;
  }
  ((T*) array)[pos.pos[i - 1]] = result;
  return ETCH_OK;
}

template<class T>
status_t EtchNativeArray<T>::set(Pos pos, capu::int32_t length, T* data, capu::int32_t offset) {
  if (mArray.mArray == NULL || (pos.size != mArray.mDim) || data == NULL) {
    return ETCH_EINVAL;
  }
  if (offset >= length) {
    return ETCH_ERANGE;
  }
  capu::int32_t i = 0;
  void * array = mArray.mArray;
  for (i = 1; i < mArray.mDim; i++) {
    array = ((EtchArray<T>*) array)[pos.pos[i - 1]].mArray;
  }

  for (capu::int32_t z = offset; z < length; z++) {
    ((T*) array)[pos.pos[i - 1] + z] = data[z];
  }
  return ETCH_OK;
}

template<class T>
status_t EtchNativeArray<T>::get(Pos pos, capu::int32_t length, T* data, capu::int32_t offset) {
  if (mArray.mArray == NULL || (pos.size != mArray.mDim) || data == NULL) {
    return ETCH_EINVAL;
  }
  if (offset >= length) {
    return ETCH_ERANGE;
  }
  capu::int32_t i = 0;
  void * array = mArray.mArray;
  for (i = 1; i < mArray.mDim; i++) {
    array = ((EtchArray<T>*) array)[pos.pos[i - 1]].mArray;
  }
  for (capu::int32_t z = offset; z < length; z++) {
    data[z] = ((T*) array)[pos.pos[i - 1] + z];
  }
  return ETCH_OK;
}

template<class T>
capu::int32_t EtchNativeArray<T>::getLength() {
  return mArray.mLen;
}

template<class T>
status_t EtchNativeArray<T>::createArray(Pos pos, capu::int32_t dim, capu::int32_t length) {
  if (mArray.mArray == NULL || (pos.size >= mArray.mDim)) {
    return ETCH_EINVAL;
  }
  capu::int32_t i = 0;
  void * array = mArray.mArray;
  for (i = 0; i < pos.size - 1; i++) {
    array = ((EtchArray<T>*) array)[pos.pos[i]].mArray;
  }

  ((EtchArray<T>*) array)[pos.pos[i]].mDim = dim;
  ((EtchArray<T>*) array)[pos.pos[i]].mLen = length;

  if (dim > 1) {
    ((EtchArray<T>*) array)[pos.pos[i]].mArray = new EtchArray<T>[length];
  } else if (dim == 1) {
    ((EtchArray<T>*) array)[pos.pos[i]].mArray = new T[length];
  }
  return ETCH_OK;
}

template<class T>
void EtchNativeArray<T>::deallocate(EtchArray<T> *array) {

  if (array == NULL)
    return;
  else if (array->mDim == 1)
    delete [] (T*) array->mArray;
  else if (array->mDim < 0)
    return;
  else {
    capu::int32_t i;
    for (i = 0; i < array->mLen; i++) {
      deallocate(&((EtchArray<T>*) array->mArray)[i]);
    }
    delete [] (EtchArray<T>*) array->mArray;
  }
}

template<class T>
capu::int32_t EtchNativeArray<T>::getDim() {
  return mArray.mDim;
}

#endif //__ETCHNATIVEARRAY_H__
