using System;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Etch.Transport;

namespace Etch.Util
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
    		
		  
    		
            //long now = (long) DateTime.Now.Millisecond * NANOS_PER_MILLI;
            //long end = maxDelay != 0 ? now + maxDelay * NANOS_PER_MILLI : long.MaxValue;

            //DateTime nowInit = DateTime.Now;
            //DateTime nowNext;
            //TimeSpan span;

            // all time in millis
            long hptimer_init =HPTimer.Now();
            long hptimer_end = GetEndTime(hptimer_init, maxDelay);

            Debug.Assert(hptimer_end > hptimer_init, "hptimer_end > hptimer_init");

    //        long hptimer_end = maxDelay > 0 ? maxDelay : long.MaxValue;
            long hptimer_diff = 0;

            //long now = 0;
            //long end = maxDelay > 0 ? now + maxDelay : long.MaxValue;
            
            //Debug.Assert( now < end );
    		
		    //while (now < end)
     //       while ( hptimer_diff < hptimer_end )  
            while ((hptimer_diff = (hptimer_end - hptimer_init)/NANOS_PER_MILLI) > 0)
		    {
			    // the queue is empty, not closed, and delay has not run out...
                Debug.Assert(IsEmpty(), "IsEmpty()");
                Debug.Assert(!IsClosed(),"!IsClosed()");
                Debug.Assert(hptimer_diff > 0, "hptimer_diff > 0");
			  

                //System.Threading.Monitor.Wait( this, (int)( end - now ) );
                System.Threading.Monitor.Wait(this,(int)(hptimer_diff) );

                //nowNext = DateTime.Now;
                //span = ( nowNext - nowInit );
                //nowInit = nowNext;
                //now = ( long ) span.TotalMilliseconds;

           //     hptimer_diff = HPTimer.MillisSince( hptimer_init );


			    if (!IsEmpty())
				    return GetAndNotify();

                if ( IsClosed() )
                    return default( T ); //null;

                hptimer_init = HPTimer.Now();

                //now = ( long ) DateTime.Now.Millisecond * NANOS_PER_MILLI;
		    }

            return default( T ); // null;
	    }
        private long NANOS_PER_MILLI = 1000000;

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
            
            // long NANOS_PER_MILLI = 1000000;

		    // the queue is not closed, the queue is full, and the caller has requested a delay...
    		
            //long now = HPTimer.Now();
            //long end = maxDelay != 0 ? now + maxDelay * NANOS_PER_MILLI : long.MaxValue;
            //Debug.Assert( now < end );

            // all time in millis
            long hptimer_init = HPTimer.Now();
   //         long hptimer_end = maxDelay > 0 ? maxDelay : long.MaxValue;
            long hptimer_end = GetEndTime(hptimer_init, maxDelay);
            Debug.Assert(hptimer_end > hptimer_init, "hptimer_end > hptimer_init");
            long hptimer_diff = 0;


            while ((hptimer_diff = (hptimer_end - hptimer_init) / Timer.NANOS_PER_MILLI) > 0)    
		    {
			    // the queue is not closed, the queue is full, and delay has not run out...
			    Debug.Assert( !IsClosed() );
                Debug.Assert( IsFull() );
         //       System.Threading.Monitor.Wait( this, ( int ) ( hptimer_end - hptimer_diff ) );
		        int diff = (int) hptimer_diff;
                if (diff < 0)
                {
                    Console.WriteLine(" Time to wait is " + diff);
                    break;
                }
                else if (diff > 0)
                {
                    System.Threading.Monitor.Wait(this, (int)hptimer_diff);
                }
		       
                
			    if (IsClosed())
				    return false;
    			
			    // the queue is not closed.
    			
			    if (!IsFull())
			    {
			        PutAndNotify(obj);
				    return true;
			    }

                hptimer_init = HPTimer.Now();
    			
			    // the queue is not closed, the queue is full, and delay might have run out...
            //    hptimer_diff = HPTimer.MillisSince( hptimer_init );
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

        private long GetEndTime(long now,int maxDelay)
        {
            return maxDelay > 0 ? now + (maxDelay * Timer.NANOS_PER_MILLI) : long.MaxValue;
        }
    }
}
