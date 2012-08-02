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

#ifndef __SMARTPOINTER_H__
#define __SMARTPOINTER_H__

#include "capu/Config.h"
#include "capu/os/AtomicOperation.h"

namespace capu {

  template<class T>
  class SmartPointer {
  public:
    /**
     * Default constructor
     */
    SmartPointer();

    /**
     * Constructor
     * @param pointer to object
     */
    SmartPointer(T* ptr);

    /**
    * Copy constructor for different, but castable type
    * @param reference to smartPointer
    */
    template<class X> friend class SmartPointer;
    template<class X>
    SmartPointer(const SmartPointer<X>& smartPointer);

    /**
    * Copy constructor
    * @param reference to smartPointer
    */
    SmartPointer(const SmartPointer& smartPointer);

    /**
     * Deconstructor
     */
    ~SmartPointer();

    /**
    * Overload assignment operator to be able to decrement the
    * reference count if another object gets assigned to this smart pointer
    * @param reference to smartPointer
    */
    SmartPointer& operator= (const SmartPointer& smartPointer);

    /**
    * Overload assignment operator for castable, but different type
    * @param reference to smartPointer
    */
    template <class X>
    SmartPointer<T>& operator= (const SmartPointer<X>& smartPointer);

    /**
    * Overload assignment operator to be able to decrement the
    * reference count if another object gets assigned to this smart pointer
    */
    SmartPointer& operator= (T* ptr);

    /**
    * Overload file operator to be able to access the object
    * referenced by the pointer
    */
    T* operator->() const;

    /**
     * Overload dereference operator to be able to get the object
     * referenced by the pointer. Use with care!
     */
    T& operator*() const;

    /**
     * Returns true if two smart pointer are equal to each other
     *         false otherwise
     */
   capu::bool_t operator==(const SmartPointer<T>& x) const;

   /**
     * Returns true if two smart pointer aren't equal to each other
     *         false otherwise
     */
   capu::bool_t operator!=(const SmartPointer<T>& x) const;

    /**
     * Returns the object stored by the smartPointer
     */
    T* get() const;

    /**
     * Check if object exists
     * @return true if object exists
     */
    operator bool();

    /**
    * Returns the reference counter value.
    * If the object does not exist, 0 ist returned
    * @return reference count
    */
    capu::uint32_t getRefCount();

    /**
    * cast a SmartPointer of type X to type T
    * this is a simple C-cast, so there are no type checks
    * @param reference to smartPointer
    */
    template<class X>
    SmartPointer<X> unchecked_cast();

  private:
    T* mData;
    capu::uint32_t* mReferenceCount;

    void incRefCount();
    void decRefCount();

    void freeData();
  };

  template<class T>
  inline
  SmartPointer<T>::SmartPointer()
  : mData(0)
  , mReferenceCount(0) {
  }

  template<class T>
  inline
  SmartPointer<T>::SmartPointer(T* ptr)
  : mData(0)
  , mReferenceCount(0) {
    if (mData != ptr) {
      mData = ptr;
      mReferenceCount = new capu::uint32_t(0);
      incRefCount();
    }
  }

  template<class T>
  template<class X>
  inline
  SmartPointer<T>::SmartPointer(const SmartPointer<X>& smartPointer)
    : mData(static_cast<T*> (smartPointer.mData))
    , mReferenceCount(smartPointer.mReferenceCount)
  {
    incRefCount();
  }

  template<class T>
  inline
  SmartPointer<T>::SmartPointer(const SmartPointer<T>& smartPointer)
  : mData(smartPointer.mData)
  , mReferenceCount(smartPointer.mReferenceCount) {
    incRefCount();
  }

  template<class T>
  inline
  SmartPointer<T>::~SmartPointer() {
    decRefCount();
  }

  template<class T>
  inline
  SmartPointer<T>& SmartPointer<T>::operator=(const SmartPointer<T>& smartPointer) {
    if (this != &smartPointer) {
      decRefCount();

      mData = smartPointer.mData;
      mReferenceCount = smartPointer.mReferenceCount;

      incRefCount();
    }

    return *this;
  }

  template<class T>
  inline
  SmartPointer<T>& SmartPointer<T>::operator=(T* ptr) {
    if (mData != ptr) {
      decRefCount();

      mData = ptr;
      mReferenceCount = new capu::uint32_t(0);

      incRefCount();
    }
    return *this;
  }

  template <class T>
  template <class X>
  inline
  SmartPointer<T>& SmartPointer<T>::operator= (const SmartPointer<X>& smartPointer) {
    if (*this != smartPointer) {
      decRefCount();
      mData = static_cast<T*> (smartPointer.mData);
      mReferenceCount = smartPointer.mReferenceCount;
      incRefCount();
    }
    return *this;
  }

  template<class T>
  inline
  T* SmartPointer<T>::operator->() const {
    return mData;
  }

  template<class T>
  inline
  T& SmartPointer<T>::operator*() const {
  return *mData;
  }

  template<class T>
  capu::bool_t SmartPointer<T>::operator==(const SmartPointer<T>& x) const {
    return ((x.mData == this->mData) && (x.mReferenceCount == this->mReferenceCount));
  }

  template<class T>
  capu::bool_t SmartPointer<T>::operator!=(const SmartPointer<T>& x) const {
    return ((x.mData != this->mData) || (x.mReferenceCount != this->mReferenceCount));
  }

  template<class T>
  inline
  void SmartPointer<T>::incRefCount() {
    if (mReferenceCount) {
      capu::AtomicOperation::AtomicInc32(*mReferenceCount);
    }
  }

  template<class T>
  inline
  void SmartPointer<T>::decRefCount() {
    if (mReferenceCount) {
      capu::AtomicOperation::AtomicDec32(*mReferenceCount);
      if (!(*mReferenceCount)) {
        freeData();
      }
    }
  }

  template<class T>
  inline
  void SmartPointer<T>::freeData() {
    delete mData;
    delete mReferenceCount;
    mData = 0;
    mReferenceCount = 0;
  }

  template<class T>
  inline
  T* SmartPointer<T>::get() const {
    return mData;
  }

  template<class T>
  inline
  SmartPointer<T>::operator bool() {
    return mData != 0;
  }

  template<class T>
  inline
  capu::uint32_t SmartPointer<T>::getRefCount() {
    if (mReferenceCount != 0)
      return *mReferenceCount;
    else
      return 0;
  }

  template<class T>
  template<class X>
  inline
  SmartPointer<X> SmartPointer<T>::unchecked_cast()
  {
    SmartPointer<X> p;
    p.mData = (X*)mData;
    p.mReferenceCount = mReferenceCount;
    incRefCount();
    return p;
  }

  template<class X, class T>
  SmartPointer<X> smartpointer_cast(SmartPointer<T>& ptr)
  {
      return ptr.template unchecked_cast<X>();
  }
}

#endif // __SMARTPOINTER_H__
