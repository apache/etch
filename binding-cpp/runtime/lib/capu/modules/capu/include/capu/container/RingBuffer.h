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
#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include "capu/Config.h"
#include "capu/Error.h"

namespace capu
{
    /*
    * RingBuffer to store data.
    * The buffer is not thread-safe!
    */
    template<typename T>
    class RingBuffer
    {
        /*
        * Iterator class.
        */
        class RingBufferIterator
        {
        public:
            friend class RingBuffer<T>;
            RingBufferIterator(RingBuffer<T>* buffer);
            ~RingBufferIterator();
            bool_t hasNext();
            status_t next(T* element);
        private:
            uint32_t mCurrentIndex;
            bool_t mMoved;
            RingBuffer<T>* mBuffer;
        };

    public:   
        /*
        * Used for iterations over the buffer.
        */
        typedef typename RingBuffer<T>::RingBufferIterator Iterator;

        /*
        * Destructor.
        */
        virtual ~RingBuffer();

        /*
        * Gets an iterator to iterate over the buffer.
        * @return Iterator for iteration over the buffer.
        */
        Iterator begin();

        /*
        * Inserts new data in the buffer.
        * @param element The element that should get added to the buffer.
        * @return The status to indicate if the add operation did succeed.
        */
        status_t add(const T& element);

        /*
        * Clears the buffer.
        * @return The status to indicate if the clear operation did succeed.
        */
        status_t clear();

        /*
        * Gets the size of the buffer. This is not the current fill status.
        * @return The size of the buffer.
        */
        uint32_t size();

        /*
        * Creates a new ring-base buffer with the specified size.
        *
        * @param bufferSize The size of the buffer.
        */
        RingBuffer(uint32_t bufferSize);

    private:      
        uint32_t mStart;
        uint32_t mEnd;
        uint32_t mBufferSize;
        bool_t mEmpty;
        T* mData; // we use a backing array for the ring buffer
        
        /*
        * Helper function to increase an index so that the new index is always inside the array bounds.
        * @param index The index that should get increased.
        */
        void increase(uint32_t& index);

        /*
        * Helper function to determine if the buffer is full.
        * @return True if the buffer is full, false otherwise.
        */
        bool_t isFull();
    };

    template<typename T>
    inline RingBuffer<T>::RingBuffer(uint32_t bufferSize)
        : mStart(0)
        , mEnd(0)
        , mBufferSize(bufferSize)
        , mEmpty(true)
        , mData(new T[bufferSize])
    {
    }

    template<typename T>
    inline RingBuffer<T>::~RingBuffer()
    {
        delete[] mData;
    }

    template<typename T>
    inline bool_t RingBuffer<T>::isFull()
    {
        return mStart == mEnd && !mEmpty;
    }

    template<typename T>
    inline void RingBuffer<T>::increase(uint32_t& index)
    {
        index = (index + 1) % mBufferSize;
    }

    template<typename T>
    inline uint32_t RingBuffer<T>::size()
    {
        return mBufferSize;
    }

    template<typename T>
    inline status_t RingBuffer<T>::add(const T& element)
    {
        if (mBufferSize == 0)
        {
            // special case of a zero buffer size
            return CAPU_ERANGE;
        }

        mData[mEnd] = element; // write operation into the buffer

        if(isFull())
        {
            // only increase 'start pointer' if the buffer is full
            increase(mStart);
        }
        increase(mEnd);
        mEmpty = false;

        return CAPU_OK;
    }

    template<typename T>
    inline status_t RingBuffer<T>::clear()
    {
        mStart = 0;
        mEnd = 0;
        mEmpty = true;
        return CAPU_OK;
    }

    template<typename T>
    inline typename RingBuffer<T>::Iterator RingBuffer<T>::begin()
    {
        return RingBufferIterator(this);
    }

    template<typename T>
    inline RingBuffer<T>::RingBufferIterator::RingBufferIterator(RingBuffer<T>* buffer)
        : mCurrentIndex(buffer->mStart),
        mMoved(false),
        mBuffer(buffer)
        
    {
        // note: we could use a modification-counter on the RingBuffer to detect concurrent modifications
    }

    template<typename T>
    inline RingBuffer<T>::RingBufferIterator::~RingBufferIterator()
    {
    }

    template<typename T>
    inline bool_t RingBuffer<T>::RingBufferIterator::hasNext()
    {
        if(mBuffer->mEmpty)
        {
            // empty buffer has no next element
            return false;
        }
        if(mCurrentIndex == mBuffer->mEnd)
        {
            // pointing to end in an non-empty buffer: we are finished if we did move once!
            return !mMoved;
        }

        // buffer is not empty and we're not pointing to the end: iteration can continue
        return true;
    }

    template<typename T>
    inline status_t RingBuffer<T>::RingBufferIterator::next(T* element)
    {
        if (!element)
        {
            // user did not provide any memory space for the next element
            return CAPU_ENO_MEMORY;
        }
        if (!hasNext())
        {
            // note: 'hasNext' is probably called twice:
            // while(it.hasNext()) <- call in client code
            //     it.next(&curr); <- call to 'hasNext' in 'next' method (this method)
            // can we ignore that?
            // but if we do not check here, we cannot know if there is a next element.
            return CAPU_ENOT_EXIST;   
        }

        mMoved = true;
        *element = mBuffer->mData[mCurrentIndex]; // assignment operation from the buffer
        mBuffer->increase(mCurrentIndex); // increase iteration index

        return CAPU_OK;
    }

}

#endif //__RINGBUFFER_H__