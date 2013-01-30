// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{

    /// <summary>
    /// A circular queue of a fixed size. Elements are added to one
    /// end and removed from the other, with the queue size ranging
    /// from empty to full. Operations can optionally wait until
    /// finished or return status indicating success or failure.
    /// For instance, adding to a full queue can wait until an item
    /// is removed before adding the new item or it can wait only
    /// a specified amount of time before completing successfully
    /// or giving up.
    /// </summary>
    /// <typeparam name="T">the type of items in the queue.</typeparam>
    public sealed class CircularQueue<T>
    {
        /// <summary>
        /// Constructs the Circular Queue
        /// </summary>
        /// <param name="size">the maximum number of items allowed in the queue.</param>
        public CircularQueue( int size )
        {
            if ( size < 1)
                throw new ArgumentOutOfRangeException("size < 1");
            this.size = size;
       //     items = new T[ size ];
            items = new Object[size];
        }

        /// <summary>
        /// Constructs the CircularQueue with the maximum number of items
        /// defaulted to 10.
        /// </summary>
        public CircularQueue() : this (10)
        {
        }

        private readonly int size;

//        private readonly T[] items;
        private readonly Object[] items;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the maximum number of items that may be put in the queue.</returns>
        public int Size()
        {
            return size;
        }

        private int count;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the current number of items in the queue.</returns>
        public int Count()
        {
            return count;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if the queue is empty.</returns>
        public bool IsEmpty()
        {
            return count == 0;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if the queue is full.</returns>
        public bool IsFull()
        {
            return count == size;
        }

        private int head;

        private Object Get0()
        {
            Debug.Assert(count>0,"count > 0");

            Object obj = items[head];

            Debug.Assert(obj != null,"obj != null");

            items[head] = null;

            count--;
            head++;

            if (head == size)
                head = 0;

            return obj;
            
        }

       /* private T IntGet()
	    {
		    Debug.Assert( !IsEmpty() );
    		
		    Debug.Assert( items[head] != null );
		    T t = items[head];
            items[ head ] = default( T ); //null;
    		
		    // wake up someone waiting for space.
            if ( IsFull() )
                System.Threading.Monitor.Pulse( this );
    		
		    count--;
    		
		    head++;
		    if (head == size)
			    head = 0;
    		
		    return t;
	    } */

        private int tail;

        private void Put0(Object obj)
        {

            Debug.Assert(obj != null, "obj != null");
            Debug.Assert(count < size, "count < size");
            Debug.Assert(items[tail] == null, "items[tail] == null");

            items[tail] = obj;

            count++;

            tail++;

            if (tail == size)
                tail = 0;
            
        }

   /*     private void IntPut( T t )
	    {
		    Debug.Assert( t != null );
		    Debug.Assert( !IsFull() );
    		
		    Debug.Assert( items[tail] == null );
		    items[tail] = t;
    		
		    // wake up someone waiting for an item.
            if ( IsEmpty() )
                System.Threading.Monitor.Pulse(this);
    		
		    count++;
    		
		    tail++;
		    if (tail == size)
			    tail = 0;
	    } */



        /// <summary>
        /// Gets the next available item from the queue, waiting
        /// until an item is available or the queue is closed.
        /// </summary>
        /// <returns>the item from the queue, or null if the queue
        /// is closed.</returns>
        /// Exception:
        ///     throws ThreadInterruptedException if thread is interrupted
        public T Get() 
	    {
           
            return Get( 0 );
	    }

        /// <summary>
        /// Gets the next available item from the queue, waiting
        /// until an item is available or the queue is closed.
        /// </summary>
        /// <param name="maxDelay">the maximum time in ms to wait for
        /// something to be put in the queue; 0 means wait forever,
        /// less than 0 means don't wait.</param>
        /// <returns>the item from the queue, or null if maxDelay
        /// has been exceeded or the queue is closed.</returns>
        /// Exception:
        ///     throws ThreadInterruptedException

        [MethodImpl(MethodImplOptions.Synchronized)]
        public T Get( int maxDelay ) 
	    {
		    if (!IsEmpty())
			    return GetAndNotify();

            if (IsClosed() || maxDelay < 0)
               return default( T ); //null;
    		
		    // the queue is empty, not closed, and caller has requested a delay

            long now = HPTimer.Now();
            long end = EndTime(now, maxDelay);

            Debug.Assert(end > now, "end > now");

            int d;
            while ((d = RemTime(end, now)) > 0)
		    {
			    // the queue is empty, not closed, and delay has not run out...
                Debug.Assert(IsEmpty(), "IsEmpty()");
                Debug.Assert(!IsClosed(),"!IsClosed()");

                System.Threading.Monitor.Wait(this, d);

			    if (!IsEmpty())
				    return GetAndNotify();

                if ( IsClosed() )
                    return default( T ); //null;

                now = HPTimer.Now();
		    }

            return default( T ); // null;
	    }

        /// <summary>
        /// Puts an item in the queue, waiting until space is available
        /// or the queue is closed.
        /// </summary>
        /// <param name="t">a non-null item to put in the queue.</param>
        /// <returns>true if the item was placed in the queue,
        /// or false if the queue is closed.</returns>
        /// Exception:
        ///     throws ThreadInterruptedException if the thread is interrupted

        public bool Put( T t ) 
	    {
		    return Put( t, 0 );
	    }

        
        /// <summary>
        /// Puts an item in the queue, waiting until space is available
        /// or the queue is closed.
        /// </summary>
        /// <param name="obj">a non-null item to put in the queue.</param>
        /// <param name="maxDelay">the maximum time in ms to wait for
        /// available space the queue; 0 means wait forever,
        /// less than 0 means don't wait.</param>
        /// <returns>true if the item was placed in the queue,
        /// or false if maxDelay has been exceeded or the queue is closed.</returns>
        /// Exception:
        ///     throws ThreadInterruptedException if the thread is interrupted

        [MethodImpl(MethodImplOptions.Synchronized)]
        public bool Put( T obj, int maxDelay ) 
	    {
		    if (obj == null)
			    throw new ArgumentNullException( "t == null" );
    		
		    if (IsClosed())
			    return false;
    		
		    // the queue is not closed.
    		
		    if (!IsFull())
		    {
			    PutAndNotify( obj );
			    return true;
		    }
    		
		    // the queue is not closed, the queue is full.
    		
		    if (maxDelay < 0)
			    return false;
			
            long now = HPTimer.Now();
            long end = EndTime(now, maxDelay);
            
            int d;
            while ((d = RemTime(end, now)) > 0)    
		    {
			    // the queue is not closed, the queue is full, and delay has not run out...
			    Debug.Assert( !IsClosed() );
                Debug.Assert( IsFull() );
                
                System.Threading.Monitor.Wait(this, d);
                
			    if (IsClosed())
				    return false;
    			
			    // the queue is not closed.
    			
			    if (!IsFull())
			    {
			        PutAndNotify(obj);
				    return true;
			    }

                now = HPTimer.Now();
		    }
    		
		    return false;
	    }

        /// <summary>
        /// Closes the queue so that no more items may be put into it.
        /// Get will return null when there are no more items to return.
        /// </summary>
        [MethodImpl(MethodImplOptions.Synchronized)]
        public void Close()
	    {
		    if (!closed)
		    {
                closed = true;
                System.Threading.Monitor.PulseAll(this);
		    }
            
	    }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if the queue is closed.</returns>
        public bool IsClosed()
        {
            return closed;
        }

        private bool closed;

        private T GetAndNotify()
        {
            bool notify = IsFull();

            Object obj = Get0();

            notify = notify || !IsEmpty();

            if (notify)
                System.Threading.Monitor.Pulse(this);

            return (T) obj;
        }

        private void PutAndNotify(T obj)
        {
            bool notify = IsEmpty();

            Put0(obj);

            notify = notify || !IsFull();

            if (notify)
                System.Threading.Monitor.Pulse(this);
           
        }

        private long EndTime(long now, int maxDelay)
        {
            if (maxDelay == 0 || maxDelay == int.MaxValue)
                return long.MaxValue;

            return now + maxDelay * HPTimer.NS_PER_MILLISECOND;
        }

        private int RemTime(long end, long now)
        {
            if (end == long.MaxValue)
                return int.MaxValue;

            long ms = (end - now) / HPTimer.NS_PER_MILLISECOND;
            if (ms > int.MaxValue)
                return int.MaxValue;

            return (int)ms;
        }
    }
}
