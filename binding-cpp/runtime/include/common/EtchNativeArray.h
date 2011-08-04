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

template<class T, int32_t content_type_id>
class EtchNativeArray :
    public EtchObject
{
public:

    /**
     * TypeId for EtchNativeArray.
     */
    static const int32_t TYPE_ID = EOTID_NATIVE_ARRAY;

    /**
     * Constructs a EtchNativeArray object.
     * T must be a pointer to a subclass of EtchObject (e.g. EtchInt32*) for this
     * class to work correctly.
     */
    EtchNativeArray(int32_t length);

    /**
     * Destructor for Etch Nativearray.
     * Frees array if content_owned was set.
     */
    virtual ~EtchNativeArray();

    /**
     * returns the TypeID of the content of this native array.
     */
    int32_t getContentTypeId();

    /**
     * gets the EtchObject at index i in result
     * returns ETCH_ERANGE, if out of bounds.
     * returns ETCH_OK otherwise
     */
    status_t get(int32_t index, T* result);

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
    status_t get(int32_t index, T* buffer, int32_t buffer_size, int32_t offset, int32_t count);
    
    
    /**
     * sets the EtchObject at index i in result
     * if content_owned, old value will be freed.
     * returns ETCH_ERANGE, if out of bounds.
     * returns ETCH_OK otherwise
     */
    status_t set(int32_t index, T value);
    
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
    status_t set(int32_t index, T* buffer, int32_t buffer_size, int32_t offset, int32_t count);

    
    
private:
    T* m_array;
    int32_t m_length;
    int32_t m_content_type_id;
};

template<class T, int32_t content_type_id>
EtchNativeArray<T, content_type_id>::EtchNativeArray(int32_t length)
      : EtchObject(EtchNativeArray::TYPE_ID),
      m_length(length) 
{
  m_array = new T[length];
  m_content_type_id = content_type_id;
}

template<class T, int32_t content_type_id>
EtchNativeArray<T, content_type_id>::~EtchNativeArray()
{
	delete[] m_array;
}

template<class T, int32_t content_type_id>
status_t EtchNativeArray<T, content_type_id>::set(int32_t index, T value)
{
  if(0 <= index && index < m_length) {
    m_array[index] = value;
    return ETCH_OK;
  }
  return ETCH_ERANGE;
}

template<class T, int32_t content_type_id>
int32_t EtchNativeArray<T, content_type_id>::getContentTypeId() {
	return m_content_type_id;
}

template<class T, int32_t content_type_id>
status_t EtchNativeArray<T, content_type_id>::get(int32_t index, T* result)
{
  if (result == NULL)
  {
    return ETCH_EINVAL;
  }
  else if (0 <= index && index < m_length) {
    *result = m_array[index];
    return ETCH_OK;
  }
  return ETCH_ERANGE;
}

template<class T, int32_t content_type_id>
status_t EtchNativeArray<T, content_type_id>::set(int32_t index, T* buffer, int32_t buffer_size, int32_t offset, int32_t count )
{
  if (buffer == NULL)
  {
      return ETCH_EINVAL;
  }
  if (0 <= index && index < m_length && (m_length - index) >= count && buffer_size <= (m_length - index) && buffer_size >= count && (offset+count) <= buffer_size) 
  {
      for (int i = 0;i < count - offset;i++,index++)
      {
          m_array[index] = buffer[offset+i];
      }
      
       return ETCH_OK;
  }
  return ETCH_ERANGE;
}


template<class T, int32_t content_type_id>
status_t EtchNativeArray<T, content_type_id>::get(int32_t index, T* buffer, int32_t buffer_size, int32_t offset, int32_t count )
{
  if (buffer == NULL)
  {
    return ETCH_EINVAL;
  }
  if (0 <= index && index < m_length && (m_length - index) >= count && buffer_size >= (m_length - index) && buffer_size >= count && (offset+count) <= buffer_size)
  {
      
      for(int i = 0;i < count - offset;i++,index++)
      {
          buffer[offset+i] = m_array[index];
      }
      
      return ETCH_OK;
  }
  return ETCH_ERANGE;
}



#endif //__ETCHNATIVEARRAY_H__
