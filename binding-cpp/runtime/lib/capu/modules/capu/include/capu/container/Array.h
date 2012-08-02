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

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "capu/Config.h"
#include "capu/Error.h"
#include "capu/os/Memory.h"

namespace capu
{

  template<typename T>
  class Array
  {
  public:

     /**
     * default constructor
     * @param size fix size of the array
     */
    Array<T>();

    /**
     * constructor
     * @param size fix size of the array
     */
    Array<T>(uint_t size);

    /**
     * constructor
     * @param size fix size of the array
     * @param value for initialization of the array
     */
    Array<T>(uint_t size, const T& value);

    /**
     * constructor for conversion from raw data
     * @param size fix size of the array
     * @param value for initialization of the array
     */
    Array<T>(const T other[], uint_t size);

    /*
     *	Copy constructor
     */
    Array<T>(const Array<T>& other);

    /*
     *	Assignment operator
     */
    Array<T>& operator=(const Array<T>& other);

    /**
     * destructor
     */
    virtual ~Array(void);

    /**
     * return size of the array
     * @return returns the size of the array
     */
    uint_t size() const;

    /**
     * initializes the array with the given value
     * @param value initialization value
     */
    void set(const T& value);

    /**
     * sets every array index between start and start+count to value
     * @param value initialization value
     * @param start start index
     * @param count number of elements
     * @return ETCH_ERANGE if out of bounds
     *         ETCH_OK otherwise
     */
    status_t set(const T& value, const uint_t start, const uint_t count);

    /**
     * swaps to elements
     * @param index1 index of the first element
     * @param index2 index of the second element
     * @return ETCH_ERANGE if out of bounds
     *         ETCH_OK otherwise
     */
    status_t swap(uint_t index1, uint_t index2);

    /**
     * moves elements inside the array
     * @param start start index
     * @param count number of elements
     * @param dst destination index
     * @return ETCH_ERANGE if out of bounds
     *         ETCH_OK otherwise
     */
    status_t move(uint_t start, uint_t count, uint_t dst);

    /**
     * copies the given values into the array
     * @param other values for the contents of the array
     * @param size fix size of the array
     * @return ETCH_ERANGE if size does not match array size
     *         ETCH_OK otherwise
     */
    status_t copy(const T other[], uint_t size);

    /**
     * Allows access of an array element
     * @param the index of the element
     * @return reference to the element at the given position
     */
    T& operator [](const uint_t index) const;

    /**
     * Sets the raw data of the array to the given value
     * @param the byte value for all array elements
     */
    void setRawData(int32_t value);

  private:
    uint_t mSize;
    T* mData;
    status_t setData(const T& value, const uint_t start, const uint_t count);
};

  template<typename T>
  Array<T>::Array()
      : mSize(0)
      , mData(0)
  {

  }

  template<typename T>
  Array<T>::Array(uint_t size)
    : mSize(size)
    , mData(new T[size])
  {
  }

  template<typename T>
  Array<T>::Array(uint_t size, const T& value)
      : mSize(size)        
      , mData(new T[size])
  {
        set(value);
  }

  template<typename T>
  Array<T>::Array(const T other[], uint_t size)
      :mSize(size)
  {
      mData = new T[mSize];
      Memory::Copy(mData, other, size * sizeof(T));
  }


  template<typename T>
  Array<T>::Array(const Array<T>& other)
      : mSize(other.mSize)
  {
      mData = new T[mSize];
      Memory::Copy(mData, other.mData, other.size() * sizeof(T));
  }

  template<typename T>
  Array<T>&  Array<T>::operator=(const Array<T>& other)
  {
    if(mSize != other.mSize)
    {
        if(mData != 0)
        {
            delete[] mData;
        }
        mSize = other.mSize;
        mData = new T[mSize];
    }

    Memory::Copy(mData, other.mData, other.size() * sizeof(T));
    return *this;
  }

  template<typename T>
  Array<T>::~Array(void)
  {
    delete[] mData;
  }

  template<typename T>
  void Array<T>::set(const T& value)
  {
    setData(value, 0, mSize);
  }

  template<typename T>
  status_t Array<T>::set(const T& value, uint_t index, uint_t count)
  {
    return setData(value, index, count);
  }

  template<typename T>
  status_t Array<T>::swap(uint_t index1, uint_t index2)
  {
    if (index1 >= mSize || index2 >= mSize){
      return CAPU_ERANGE;
    }
    T tmp = mData[index1];
    mData[index1] = mData[index2];
    mData[index2] = tmp;

    return CAPU_OK;
  }

  template<typename T>
  status_t Array<T>::move(uint_t start, uint_t count, uint_t dst)
  {
    if ((start >= mSize) || ((count+start) > mSize) || ((mSize-count) + start < 0) || ((dst+count) > mSize)) {
      return CAPU_ERANGE;
    }
    Memory::Move(mData + dst, mData + start, sizeof(T) * count);

    return CAPU_OK;
  }

  template<typename T>
  status_t Array<T>::copy(const T other[], uint_t size)
  {
    if (size != mSize) {
      return CAPU_ERANGE;
    }

    Memory::Copy(mData, other, size * sizeof(T));

    return CAPU_OK;
  }  

  template<typename T>
  T& Array<T>::operator[](const uint_t index) const
  {
    return mData[index];
  }

  template<typename T>
  uint_t Array<T>::size() const
  {
    return mSize;
  }

  template<typename T>
  status_t Array<T>::setData(const T& value, const uint_t index, const uint_t count) {

    if ((index >= mSize) || ((count+index) > mSize) || ((mSize-count) + index < 0)) {
      return CAPU_ERANGE;
    }
    T* start = &mData[mSize-(mSize-count) + index];
    
    switch(count)
    {
      default:
      {
        T* end = start - (count - 10); 
        while(start > end){ *(--start) = value; }
      }
      case 10 : *(--start) = value;
      case 9  : *(--start) = value;
      case 8  : *(--start) = value;
      case 7  : *(--start) = value;
      case 6  : *(--start) = value;
      case 5  : *(--start) = value;
      case 4  : *(--start) = value;
      case 3  : *(--start) = value;
      case 2  : *(--start) = value;
      case 1  : *(--start) = value;
    }

    return CAPU_OK;
  }

  template<typename T>
  void Array<T>::setRawData(int32_t value)
  {
      Memory::Set(mData, value, sizeof(T) * mSize);
  }
}


#endif // __ARRAY_H__
