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
  class SmartPointer
  {
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

  private:
    T* m_data;
    capu::uint32_t* m_referenceCount;

    void incRefCount();
    void decRefCount();

    void freeData();
  };

  template<class T> 
  inline
    SmartPointer<T>::SmartPointer()
    : m_data(0)
    , m_referenceCount(0)
  {
  }

  template<class T>
  inline
    SmartPointer<T>::SmartPointer(T* ptr)
    : m_data(0)
    , m_referenceCount(0)
  {
    if( m_data != ptr)
    {
      m_data = ptr;
      m_referenceCount = new capu::uint32_t(0);
      incRefCount();
    }
  }

  template<class T>
  template<class X>
  inline
  SmartPointer<T>::SmartPointer(const SmartPointer<X>& smartPointer)
    : m_data(static_cast<X*> (smartPointer.m_data))
    , m_referenceCount(smartPointer.m_referenceCount)
  {
    incRefCount();
  }

  template<class T>
  inline
    SmartPointer<T>::SmartPointer(const SmartPointer<T>& smartPointer)
    : m_data(smartPointer.m_data)
    , m_referenceCount(smartPointer.m_referenceCount)
  {
    incRefCount();
  }

  template<class T>
  inline
    SmartPointer<T>::~SmartPointer()
  {
    decRefCount();
  }

  template<class T>
  inline
  SmartPointer<T>& SmartPointer<T>::operator= (const SmartPointer<T>& smartPointer)
  {
    if(this != &smartPointer)
    {
      decRefCount();

      m_data = smartPointer.m_data;
      m_referenceCount = smartPointer.m_referenceCount;

      incRefCount();
    }

    return *this;
  }

  template<class T>
  inline
  SmartPointer<T>& SmartPointer<T>::operator= (T* ptr)
  {
    if( m_data != ptr)
    {
      decRefCount();

      m_data = ptr;
      m_referenceCount = new capu::uint32_t(0);

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
      m_data = static_cast<X*> (smartPointer.m_data);
      m_referenceCount = smartPointer.m_referenceCount;
      incRefCount();
    }
    return *this;
  }

  template<class T>
  inline
  T* SmartPointer<T>::operator->() const
  {
    return m_data;
  }

  template<class T>
  inline
  T& SmartPointer<T>::operator*() const
  {
  return *m_data;
  }

  template<class T>
  capu::bool_t SmartPointer<T>::operator==(const SmartPointer<T>& x) const {
    return ((x.m_data == this->m_data) && (x.m_referenceCount == this->m_referenceCount));
  }

  template<class T>
  capu::bool_t SmartPointer<T>::operator!=(const SmartPointer<T>& x) const {
    return ((x.m_data != this->m_data) || (x.m_referenceCount != this->m_referenceCount));
  }

  template<class T>
  inline
  void SmartPointer<T>::incRefCount()
  {
    if(m_referenceCount)
    {
      capu::AtomicOperation::AtomicInc32(*m_referenceCount);
    }
  }

  template<class T>
  inline
  void SmartPointer<T>::decRefCount()
  {
    if(m_referenceCount) {
      capu::AtomicOperation::AtomicDec32(*m_referenceCount);
      if (!(*m_referenceCount)) {
        freeData();
      }
    }
  }

  template<class T>
  inline
  void SmartPointer<T>::freeData()
  {
    delete m_data;
    delete m_referenceCount;
    m_data = 0;
    m_referenceCount = 0;
  }

  template<class T>
  inline
  T* SmartPointer<T>::get() const
  {
    return m_data;
  }

  template<class T>
  inline
  SmartPointer<T>::operator bool()
  {
    return m_data != 0;
  }

  template<class T>
  inline
  capu::uint32_t SmartPointer<T>::getRefCount()
  {
    if (m_referenceCount != 0)
      return *m_referenceCount;
    else 
      return 0;
  }
}

#endif // __SMARTPOINTER_H__
