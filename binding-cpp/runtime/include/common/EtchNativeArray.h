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

#include "capu/util/SmartPointer.h"
#include "common/EtchTypes.h"


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

  //internal helper classes
  template <class T>
  class EtchArray;

  template <class T>
  class EtchArrayBase {
  public:

    /**
     * Creates a new instance of EtchArrayBase
     */
    EtchArrayBase(capu::int32_t length, capu::int32_t dim = 1)
      : mLength(length), mDim(dim) {
    }

    virtual ~EtchArrayBase() {
    }

    /**
     * Returns the dimension
     */
    virtual capu::int32_t getDim() {
      return mDim;
    }

    /**
     * Returns the length
     */
    virtual capu::int32_t getLength() {
      return mLength;
    }

    /**
     * Sets an element at the given position
     */
    virtual status_t set(Pos pos, capu::int32_t index, const T &data) = 0;

    /**
     * Sets an array at the given position
     */
    virtual status_t set(Pos pos, capu::int32_t index, const T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *written) = 0;

    /**
     * Sets the sub array at the given position
     */
    virtual status_t set(Pos pos, capu::int32_t index, const capu::SmartPointer<EtchArray<T> > &data) = 0;

    /**
     * Returns the element at the given position
     */
    virtual status_t get(Pos pos, capu::int32_t index, T *result) = 0;

    /**
     * Returns the array at the given position
     */
    virtual status_t get(Pos pos, capu::int32_t index, T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *read) = 0;

    /**
     * Returns the subarray at the given position
     */
    virtual status_t get(Pos pos, capu::int32_t index, capu::SmartPointer<EtchArray<T> > *result) = 0;

    /**
     * creates a new nested array at the given position with the given length and dim
     */
    virtual status_t createArray(Pos pos, capu::int32_t index, capu::int32_t length, capu::int32_t dim) = 0;

    /**
     * resizes the array to the given size
     */
    virtual status_t resize(capu::int32_t newSize) = 0;


  protected:
    capu::int32_t mLength;
    capu::int32_t mDim;
  };

  template <class T>
  class EtchArrayData : public EtchArrayBase<T> {
  public:

    /**
     * Create a new instance of EtchArrayData
     */
    EtchArrayData(capu::int32_t length)
      : EtchArrayBase<T>(length) {
        mData = new T[EtchArrayBase<T>::mLength];
    }

    /**
     * Destructure
     */
    virtual ~EtchArrayData() {
      delete[] mData;
    }

    /**
     *@see EtchArrayBase
     */
    status_t set(Pos pos, capu::int32_t index, const T &data) {
      if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength || pos.size-1 != index) {
        return ETCH_ERANGE;
      }
      mData[pos.pos[index]] = data;
      return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
    status_t set(Pos pos, capu::int32_t index, const T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *written) {
      if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength || pos.size-1 != index || dataSize < offset + length) {
        return ETCH_ERANGE;
      }
      if(data != NULL) {
        //TODO: replace with memcpy
        capu::int32_t i;
        for (i = 0; i < EtchArrayBase<T>::mLength && i < length; i++) {
          mData[pos.pos[index]+i] = data[offset + i];
        }
        if (written != NULL) {
          *written = i;
        }
      } else {
        return ETCH_EINVAL;
      }
      return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
    status_t set(Pos pos, capu::int32_t index, const capu::SmartPointer<EtchArray<T> > &data) {
      return ETCH_ERANGE;
    }

    /**
     *@see EtchArrayBase
     */
    status_t get(Pos pos, capu::int32_t index, T *result) {
      if(pos.size < index || pos.size-1 != index || pos.pos[index] > EtchArrayBase<T>::mLength) {
        return ETCH_ERANGE;
      }
      if(result != NULL) {
        *result = mData[pos.pos[index]];
      } else {
        return ETCH_EINVAL;
      }
       return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
    status_t get(Pos pos, capu::int32_t index, T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *read) {
      if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength || pos.size-1 != index || dataSize < offset + length) {
        return ETCH_ERANGE;
      }
      if(data != NULL) {
        //TODO: replace with memcpy
        capu::int32_t i;
        for (i = 0; i < EtchArrayBase<T>::mLength && i < length; i++) {
          data[offset + i] = mData[pos.pos[index]+i];
        }
        if (read != NULL) {
          *read = i;
        }
      } else {
        return ETCH_EINVAL;
      }
      return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
    status_t get(Pos pos, capu::int32_t index, capu::SmartPointer<EtchArray<T> > *result) {
      return ETCH_ERANGE;
    }

    /**
     *@see EtchArrayBase
     */
    status_t createArray(Pos pos, capu::int32_t index, capu::int32_t length, capu::int32_t dim) {
      return ETCH_ERANGE;
    }

    /**
     *@see EtchArrayBase
     */
    status_t resize(capu::int32_t newSize) {
      if (newSize <= EtchArrayBase<T>::mLength) {
        return ETCH_ERANGE;
      }
      T *newData = new T[newSize];
      for (capu::int32_t i = 0; i<EtchArrayBase<T>::mLength; i++) {
        newData[i] = mData[i];
      }
      delete[] mData;
      mData = newData;
      EtchArrayBase<T>::mLength = newSize;
      return ETCH_OK;
    }
  private:
    T* mData;
  };

  template <class T>
  class EtchArray : public EtchArrayBase<T> {
  public:

    /**
     * Creats a new instance of the EtchArray class
     */
    EtchArray(capu::int32_t length, capu::int32_t dim)
      : EtchArrayBase<T>(length, dim) {
      mData = new capu::SmartPointer<EtchArrayBase<T> >[EtchArrayBase<T>::mLength];
    }

    /**
     * Destructure
     */
    virtual ~EtchArray() {
      delete[] mData;
    }

    /**
     *@see EtchArrayBase
     */
    status_t set(Pos pos, capu::int32_t index, const T &data) {
      if(mData[pos.pos[index]].get() != NULL) {
        return mData[pos.pos[index]]->set(pos, index+1, data);
      }
      return ETCH_ERANGE;
    }

  /**
     *@see EtchArrayBase
     */
    status_t set(Pos pos, capu::int32_t index, const T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *written) {
      if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength) {
        return ETCH_ERANGE;
      }

      if(mData[pos.pos[index]].get() != NULL) {
        if ((pos.size -1) == index) {
          //set subarray
          //TODO: enhance performance avoiding for-loop-copy
          for (capu::int8_t i = 0; i < length; i++) {
            mData[pos.pos[index]]->set(i,0,data[i]);
          }
        } else {
          //redirect to next sub array
          return mData[pos.pos[index]]->set(pos, index+1, data, dataSize, offset, length, written);
        }
      } else {
        return ETCH_EINVAL;
      }
      return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
    status_t set(Pos pos, capu::int32_t index, const capu::SmartPointer<EtchArray<T> > &data) {
      if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength) {
        return ETCH_ERANGE;
      }

      if (pos.size - index == 1) {
        if (EtchArrayBase<T>::mDim -1 == data->getDim()) { //check if we set a subarray with correct dimension
          mData[pos.pos[index]] = data;
        } else {
          return ETCH_ERANGE;
        }
      } else {
        return mData[pos.pos[index]]->set(pos,index+1,data);
      }

      return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
    status_t get(Pos pos, capu::int32_t index, T *result) {
      if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength) {
        return ETCH_ERANGE;
      }

      if(mData[pos.pos[index]].get() != NULL) {
        return mData[pos.pos[index]]->get(pos, index+1, result);
      }
      else {
        return ETCH_EINVAL;
      }
      return ETCH_OK;
    }

  /**
     *@see EtchArrayBase
     */
    status_t get(Pos pos, capu::int32_t index, T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *read) {
      if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength) {
        return ETCH_ERANGE;
      }

      if(mData[pos.pos[index]].get() != NULL) {
        return mData[pos.pos[index]]->get(pos, index+1, data, dataSize, offset, length, read);
      } else {
        return ETCH_EINVAL;
      }
      return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
      status_t get(Pos pos, capu::int32_t index, capu::SmartPointer<EtchArray<T> > *result) {
        if(pos.size < index || pos.pos[index] > EtchArrayBase<T>::mLength || mData[pos.pos[index]].get() == NULL) {
          return ETCH_ERANGE;
        }
        if (pos.size - index == 1) {
          *result = capu::smartpointer_cast<EtchArray<T> >(mData[pos.pos[index]]);
        } else {
          return mData[pos.pos[index]]->get(pos,index+1,result);
        }

        return ETCH_OK;
      }

    /**
     *@see EtchArrayBase
     */
    status_t createArray(Pos pos, capu::int32_t index, capu::int32_t length, capu::int32_t dim) {
      if(mData[pos.pos[index]].get() == NULL) {
        if(dim == 1) {
          mData[pos.pos[index]] = new EtchArrayData<T>(length);
        } else {
          if (pos.size - index == 1) {
            mData[pos.pos[index]] = new EtchArray<T>(length, dim);
          }
        }
      } else {
        if (dim == 1 || pos.size - index == 1) {
          return ETCH_ERANGE;
        }
        return mData[pos.pos[index]]->createArray(pos, index+1, length, dim-1);
      }
      return ETCH_OK;
    }

    /**
     *@see EtchArrayBase
     */
    status_t resize(capu::int32_t newSize) {
      if (newSize <= EtchArrayBase<T>::mLength) {
        return ETCH_ERANGE;
      }
      capu::SmartPointer<EtchArrayBase<T> > *newData = new capu::SmartPointer<EtchArrayBase<T> >[newSize];
      for (capu::int32_t i = 0; i<EtchArrayBase<T>::mLength; i++) {
        newData[i] = mData[i];
      }
      delete[] mData;
      mData = newData;
      EtchArrayBase<T>::mLength = newSize;
      return ETCH_OK;
    }
  private:
    capu::SmartPointer<EtchArrayBase<T> > *mData;
  };

template<class T>
class EtchNativeArray;

class EtchNativeArrayBase :
public EtchObject {
public:

  EtchNativeArrayBase() {
  }

  EtchNativeArrayBase(const EtchNativeArrayBase& other)
   : EtchObject(other) {

  }

  /**
   * gets the EtchObject at index i in result
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t getBase(Pos pos, capu::SmartPointer<EtchObject> &result);

  /**
   * sets the EtchObject at index i
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t setBase(Pos pos, capu::SmartPointer<EtchObject> data);

  /**
   * resize current array to newSize
   * returns ETCH_ERANGE, if newSize is smaller then current size
   * returns ETCH_OK otherwise
   */
  virtual status_t resize(capu::int32_t newSize) = 0;

  /**
   * Returns the length of array
   */
  virtual capu::int32_t getLength() = 0;

  /**
   * Returns the dim of array
   */
  virtual capu::uint32_t getDim() = 0;

};

template<class T>
class EtchNativeArray :
public EtchNativeArrayBase {
public:

  /**
   * EtchObjectType for EtchNativeArray.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a EtchNativeArray object.
   */
  EtchNativeArray(capu::int32_t length, capu::int32_t dim = 1);

  /**
   * Constructs a EtchNativeArray object from native array.
   */
  EtchNativeArray(capu::int32_t length, capu::int32_t dim, T* array);

  /**
   * Copy Constructor
   */
  EtchNativeArray(const EtchNativeArray& other);

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

  status_t get(Pos pos, T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *read  = NULL);

  /**
   * gets the array stored at the given position
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t get(Pos pos, capu::SmartPointer<EtchNativeArray> &subarray);

  /**
   * sets the EtchObject at index i in result
   * if content_owned, old value will be freed.
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t set(Pos pos, const T &data);

  status_t set(Pos pos, const T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *written = NULL);

  /**
   * sets the array at the given position
   * returns ETCH_ERANGE, if out of bounds.
   * returns ETCH_OK otherwise
   */
  status_t set(Pos pos, EtchNativeArray *subarray);

  //status_t set(Pos pos, capu::int32_t length, T* data, capu::int32_t offset);

  /**
   * Returns the length of array
   */
  capu::int32_t getLength();

  /**
   * Returns the dim of array
   */
  capu::uint32_t getDim();

  /**
   * Creates a new nested array
   * @pos of the neseted array
   * @length length of the nested array
   */
  status_t createArray(Pos pos, capu::int32_t length);

  /**
   * @see EtchNativeArrayBase
   */
  status_t resize(capu::int32_t newSize);

private:
  capu::SmartPointer<EtchArrayBase<T> > mData;

  /**
   * private constructor
   * creates a new native array from an existing subarray
   */
  EtchNativeArray(capu::SmartPointer<EtchArray<T> > array);

};

template<class T>
const EtchObjectType* EtchNativeArray<T>::TYPE() {
  const static EtchObjectType TYPE(EOTID_NATIVE_ARRAY, EtchObjectType::getType<T>(), EtchObjectType::getTypeTrait<T>());

  return &TYPE;
}

template<class T>
EtchNativeArray<T>::EtchNativeArray(capu::int32_t length, capu::int32_t dim) {
  addObjectType(EtchNativeArray<T>::TYPE());
  if (dim == 1) {
    mData = new EtchArrayData<T>(length);
  } else {
    mData = new EtchArray<T>(length, dim);
  }
}

template<class T>
EtchNativeArray<T>::EtchNativeArray(capu::int32_t length, capu::int32_t dim, T* array) {
  addObjectType(EtchNativeArray<T>::TYPE());
  capu::int32_t bytesWritten;
  if (dim == 1) {
    mData = new EtchArrayData<T>(length);
    mData->set(0, 0, array, length, 0, length, &bytesWritten);
  } else if (dim == 2) {
    mData = new EtchArray<T>(length, dim);
    capu::int32_t offset = 0;
    for (capu::int32_t i = 0; i < length; i++) {
      mData->createArray(i, 0, 2,dim-1);
      mData->set(i, 0, &array[offset], length, 0, length, &bytesWritten);
      offset += length;
    }
  } else if (dim >= 3 || dim <= 9) {
    //the construction of EtchNativeArrays of more than 2 dimensions is not supported
  }
}

template<class T>
EtchNativeArray<T>::EtchNativeArray(capu::SmartPointer<EtchArray<T> > array) {
  addObjectType(EtchNativeArray<T>::TYPE());
  mData = capu::smartpointer_cast<EtchArrayBase<T> > (array);
}

template<class T>
EtchNativeArray<T>::EtchNativeArray(const EtchNativeArray& other)
 : EtchNativeArrayBase(other), mData(other.mData) {
}

template<class T>
EtchNativeArray<T>::~EtchNativeArray() {
}

template<class T>
status_t EtchNativeArray<T>::get(Pos pos, T *result) {
  return mData->get(pos, 0, result);
}

template<class T>
status_t EtchNativeArray<T>::get(Pos pos, T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *read) {
  return mData->get(pos, 0, data, dataSize, offset, length, read);
}

template <class T>
status_t EtchNativeArray<T>::get(Pos pos, capu::SmartPointer<EtchNativeArray<T> > &subarray) {
  capu::SmartPointer<EtchArray<T> > temp = NULL;
  status_t res = mData->get(pos, 0, &temp);
  if (res != ETCH_OK) {
    return res;
  }

  subarray = new EtchNativeArray(temp);
  return ETCH_OK;
}

template<class T>
status_t EtchNativeArray<T>::set(Pos pos, const T &data) {
  return mData->set(pos, 0, data);
}

template<class T>
status_t EtchNativeArray<T>::set(Pos pos, const T* data, capu::int32_t dataSize, capu::int32_t offset, capu::int32_t length, capu::int32_t *written) {
  return mData->set(pos, 0, data, dataSize, offset, length, written);
}

template<class T>
status_t EtchNativeArray<T>::set(Pos pos, EtchNativeArray<T> *subarray) {
  if (subarray == NULL) {
    return ETCH_ERANGE;
  }
  capu::SmartPointer<EtchArray<T> > temp = capu::smartpointer_cast<EtchArray<T> >(subarray->mData);
  return mData->set(pos, 0, temp);
}


template<class T>
capu::int32_t EtchNativeArray<T>::getLength() {
  return mData->getLength();
}

template<class T>
capu::uint32_t EtchNativeArray<T>::getDim() {
  return mData->getDim();
}

template<class T>
status_t EtchNativeArray<T>::createArray(Pos pos, capu::int32_t length) {
  return mData->createArray(pos, 0, length, mData->getDim()-1);
}

template<class T>
status_t EtchNativeArray<T>::resize(capu::int32_t newSize) {
  return mData->resize(newSize);
}

//getBaseMacro used in NativeArrayBase::getBase()
#define GETBASE_CAST_OBJECT_TO_NA(type) { \
  switch(trait) { \
    case EtchObjectType::VALUE: { \
      EtchNativeArray<type>* na = (EtchNativeArray<type>*)(this); \
      if(dims == 1) { \
        type value; \
        ret = na->get(pos, &value); \
        if (ret == ETCH_OK) { \
          result = new type(value); \
        } \
      } else { \
        capu::SmartPointer<EtchNativeArray<type> > subarray; \
        ret = na->get(pos,subarray); \
        if (ret == ETCH_OK) { \
          result = subarray; \
        } \
      } \
      break; \
    } \
    case EtchObjectType::POINTER: { \
      EtchNativeArray<type*>* na = (EtchNativeArray<type*>*)(this); \
      if(dims == 1) { \
        type* value = NULL; \
        ret = na->get(pos, &value); \
        if(ret == ETCH_OK && value != NULL) { \
          result = new type(*value); \
        } \
      } else { \
        capu::SmartPointer<EtchNativeArray<type*> > subarray; \
        ret = na->get(pos,subarray); \
        if (ret == ETCH_OK) { \
          result = subarray; \
        } \
      } \
      break; \
    } \
    case EtchObjectType::SMART_POINTER: { \
      EtchNativeArray<capu::SmartPointer<type> >* na = (EtchNativeArray<capu::SmartPointer<type> >*)(this); \
      if(dims == 1) { \
        capu::SmartPointer<type> value = NULL; \
        ret = na->get(pos, &value); \
        if (ret == ETCH_OK) { \
          result = value; \
        } \
      } else { \
        capu::SmartPointer<EtchNativeArray<capu::SmartPointer<type> > > subarray; \
        ret = na->get(pos,subarray); \
        if (ret == ETCH_OK) { \
          result = subarray; \
        } \
      } \
      break; \
    } \
    default: { \
      return ETCH_ERROR; \
    } \
  } \
}

//setBaseMacro used in NativeArrayBase::setBase()
#define SETBASE_CAST_OBJECT_TO_NA(type) { \
  switch(trait) { \
    case EtchObjectType::VALUE: { \
      EtchNativeArray<type>* na = (EtchNativeArray<type>*)(this); \
      if(dims == 1) { \
        capu::SmartPointer<type> value = capu::smartpointer_cast<type> (data); \
        type *newValue = value.get(); \
        if (newValue != NULL) { \
          return na->set(pos, *newValue); \
        } \
      } else { \
        capu::SmartPointer<EtchNativeArray<type> > subarray = capu::smartpointer_cast<EtchNativeArray<type> >(data); \
        return na->set(pos,subarray.get()); \
      } \
      break; \
    } \
    case EtchObjectType::POINTER: { \
      EtchNativeArray<type*>* na = (EtchNativeArray<type*>*)(this); \
      if(dims == 1) { \
        capu::SmartPointer<type> value = capu::smartpointer_cast<type> (data); \
        type *newValue = value.get(); \
        return na->set(pos, newValue); \
      } else { \
        capu::SmartPointer<EtchNativeArray<type*> > subarray = capu::smartpointer_cast<EtchNativeArray<type*> >(data); \
        return na->set(pos,subarray.get()); \
      } \
      break; \
    } \
    case EtchObjectType::SMART_POINTER: { \
      EtchNativeArray<capu::SmartPointer<type> >* na = (EtchNativeArray<capu::SmartPointer<type> >*)(this); \
      if(dims == 1) { \
        capu::SmartPointer<type> value = capu::smartpointer_cast<type> (data); \
        return na->set(pos, value); \
      } else { \
        capu::SmartPointer<EtchNativeArray<capu::SmartPointer<type> > > subarray = capu::smartpointer_cast<EtchNativeArray<capu::SmartPointer<type> > >(data); \
        return na->set(pos,subarray.get()); \
      } \
      break; \
    } \
    default: { \
      break; \
    } \
  } \
  break; \
}

typedef capu::SmartPointer<EtchNativeArray<EtchObjectPtr>    > EtchNativeArrayObjectPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchBoolPtr>      > EtchNativeArrayBoolPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchShortPtr>     > EtchNativeArrayShortPtr;
typedef capu::SmartPointer<EtchNativeArray<capu::int8_t>     > EtchNativeArrayBytePtr;
typedef capu::SmartPointer<EtchNativeArray<EtchInt32Ptr>     > EtchNativeArrayIntPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchLongPtr>      > EtchNativeArrayLongPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchFloatPtr>     > EtchNativeArrayFloatPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchDoublePtr>    > EtchNativeArrayDoublePtr;
typedef capu::SmartPointer<EtchNativeArray<EtchStringPtr>    > EtchNativeArrayStringPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchHashSetPtr>   > EtchNativeArrayHashSetPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchListPtr>      > EtchNativeArrayListPtr;
typedef capu::SmartPointer<EtchNativeArray<EtchHashTablePtr> > EtchNativeArrayHashTablePtr;


#endif //__ETCHNATIVEARRAY_H__

