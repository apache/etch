using System;
using System.Text;
using System.Threading;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// A class which we can use to monitor conditions.
    /// </summary>
    /// <typeparam name="T">Type of object that can be stored in the monitor</typeparam>
    public class Monitor<T>
    {
        /// <summary>
        /// Constructs the Monitor.
        /// </summary>
        /// <param name="description">a description of this monitor</param>
        /// <param name="initialValue">the initial value of this monitor</param>
        public Monitor( String description, T initialValue )
        {
            this.description = description;
            this.value = initialValue;
        }
        /// <summary>
        /// Constructs the Monitor with null initial value
        /// </summary>
        /// <param name="description">a description of this monitor</param>
        
        public Monitor( String description ) : this( description, default( T ) )
        {   
        }

        /// <summary>
        /// Get the description of the Monitor
        /// </summary>
        /// <returns> return the description of this monitor</returns>

        public String GetDescription()
        {
            return description;
        }

        private String description;
	    private T value;

        public override string ToString()
        {
            return "Monitor "+description+": "+value;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>The current value of the monitor</returns>
        public T Get()
        {
            return value;
        }

        /// <summary>
        /// Sets the current value.
        /// </summary>
        /// <param name="newValue"> the value to be set</param>
        /// <returns>the old value.</returns>
        [MethodImpl(MethodImplOptions.Synchronized)]
        public T Set( T newValue )
	    {
		    T oldValue = value;
		    value = newValue;
		    System.Threading.Monitor.PulseAll(this);
		    return oldValue;
	    }

        /// <summary>
        /// Waits until monitor value is set
        ///  the value. Will wait forever
        /// </summary>
        /// <param name="maxDelay">the max amount of time in ms to wait.
        /// If 0 is specified, we will wait forever.</param>
        /// <returns>the current value of the monitor.</returns>
        /// Exception:
        ///     throws ThreadInterruptedException if we waited too long.
        [MethodImpl( MethodImplOptions.Synchronized )]
        private T WaitUntilSet( long maxDelay )
		{
            System.Threading.Monitor.Wait( this, (int) maxDelay );
            return value;
	    }


        /// <summary>
        /// Waits until value equals the desired value and
        /// then sets the value. Will wait forever.
        /// </summary>
        /// <param name="desiredValue">the value we want</param>
        /// <param name="newValue">the value to be set</param>
        /// <returns>The old value</returns>
        /// Exception: 
        ///     throws Exception
        public T WaitUntilEqAndSet( T desiredValue, T newValue )
        {
            return WaitUntilEqAndSet( desiredValue, 0, newValue );
        }

        /// <summary>
        /// Waits until value equals the desired value and
        /// then sets the value
        /// </summary>
        /// <param name="desiredValue">the value we want.</param>
        /// <param name="maxDelay">the max amount of time in ms to wait
        ///  If 0 is specified, we will wait forever.
        /// </param>
        /// <param name="newValue">the value to be set</param>
        /// <returns>Old Value</returns>
        /// Exception:  
        ///     throws ThreadInterruptedException
        [MethodImpl( MethodImplOptions.Synchronized )]
        public T WaitUntilEqAndSet( T desiredValue, int maxDelay, T newValue )
        {
            WaitUntilEq(desiredValue, maxDelay);
            return Set(newValue);
        }

        /// <summary>
        /// Waits forever until the value is set to the specified value.
        /// </summary>
        /// <param name="desiredValue">The value we are waiting for.</param>
        /// Exception:  
        ///     throws ThreadInterruptedException
        public void WaitUntilEq( T desiredValue )
        { 
            WaitUntilEq( desiredValue, 0 );
        }

        /// <summary>
        /// Waits until the value equals the desired value
        /// </summary>
        /// <param name="desiredValue">The value we want</param>
        /// <param name="maxDelay">the max amount of time in ms to wait.
        /// If 0 is specified, we will wait forever.</param>
        /// Exception:
        ///     throws ThreadInterruptedException if we waited too long.
        [MethodImpl( MethodImplOptions.Synchronized )]
        public void WaitUntilEq( T desiredValue, int maxDelay )
        {
            CheckDelay(maxDelay);
            
            long hptimer_init = HPTimer.Now(); 
            long hptimer_end = EndTime(hptimer_init, maxDelay);
 
            long d = 0;

            while (!Eq(value, desiredValue) && (d = RemTime(hptimer_end, hptimer_init)) > 0)
            {
                int x = (int) d;
                if (x < 0)
                {
                    Console.WriteLine(" Time to wait is " + d);
                    break;
                }
                else if (x > 0)
                {
                    System.Threading.Monitor.Wait(this, x);
                }
               

                hptimer_init = HPTimer.Now(); 
            }

            if ( !Eq( value, desiredValue ) )
                throw new ThreadInterruptedException( "timeout" );
        }
        
        private long NANOS_PER_MILLI = 1000000;
        
        
        /// <summary>
        /// Waits until value does not equal the undesired value and then
        /// sets the value. Will wait forever
        /// </summary>
        /// <param name="undesiredValue">the value we do not want</param>
        /// <param name="newValue">the value to be set</param>
        /// <returns>Old value</returns>
        /// Exception:
        ///     throws ThreadInterruptedException
        public T WaitUntilNotEqAndSet( T undesiredValue, T newValue )
        {
            return WaitUntilNotEqAndSet( undesiredValue, 0, newValue );
        }

        /// <summary>
        /// Waits until value does not equal the undesired value and then
        /// sets the value. 
        /// </summary>
        /// <param name="undesiredValue">the value we do not want</param>
        /// <param name="maxDelay">the max amount of time in ms to wait.
        /// If 0 is specified, we will wait forever.</param>
        /// <param name="newValue">New value</param>
        /// <returns>The old value</returns>
        /// Exception:
        ///     throws ThreadInterrupedException
        [MethodImpl( MethodImplOptions.Synchronized )]
        public T WaitUntilNotEqAndSet( T undesiredValue,
            int maxDelay, T newValue )
        {
            WaitUntilNotEq( undesiredValue, maxDelay );
            return Set( newValue );
        }

        /// <summary>
        /// Waits until value does not equal the undesired value. Will
        /// wait forever
        /// </summary>
        /// <param name="undesiredValue">The value we do not want</param>
        /// <returns>Curretn value of the monitor</returns>
        /// Exception:
        ///     throws ThreadInterruptedException if we waited too long.
        public T WaitUntilNotEq( T undesiredValue )
        {
            return WaitUntilNotEq( undesiredValue, 0 );
        }

        /// <summary>
        /// Waits until the value is not the specified value.
        /// </summary>
        /// <param name="undesiredValue">the value we do not want.</param>
        /// <param name="maxDelay">the max amount of time in ms to wait.
        /// If 0 is specified, we will wait forever.</param>
        /// <returns>the current value of the monitor</returns>
        /// Exception:
        ///     throws ThreadInterruptedException if we waited too long.
        [MethodImpl(MethodImplOptions.Synchronized)]
        public T WaitUntilNotEq( T undesiredValue, int maxDelay )
        {
          
            CheckDelay(maxDelay);

            // all time in millis
            long hptimer_init = HPTimer.Now();
            long hptimer_end = EndTime(hptimer_init, maxDelay);
            long d = 0;

            while (Eq(value, undesiredValue) && (d = RemTime(hptimer_end, hptimer_init)) > 0)
            { 
                System.Threading.Monitor.Wait(this, (int)d);
                hptimer_init = HPTimer.Now();              
            }

            if ( Eq( value, undesiredValue ) )
                throw new ThreadInterruptedException("timeout");

            return value;
        }

        private void CheckDelay(int maxDelay)
        {
            if (maxDelay < 0)
                throw new ArgumentException("maxDelay < 0");
        }

        private long EndTime(long now, int maxDelay)
        {
            return maxDelay > 0 ? now + maxDelay * NANOS_PER_MILLI : long.MaxValue;
        }

        private long RemTime(long end, long now)
        {
            return (end - now) / NANOS_PER_MILLI;
        }

        /// <summary>
        /// Compares the specified values.
        /// </summary>
        /// <param name="v1">a value to compare, which may be null.</param>
        /// <param name="v2">another value to compare, which may be null.</param>
        /// <returns>true if the values are equal, false otherwise. If both
        /// values are null, they are considered equal.</returns>
        private bool Eq( T v1, T v2 )
        {
            if ( v1 != null && v2 != null )
                return v1.Equals(v2);

            if ( v1 == null && v2 == null ) 
                return true;

            return false;
        }
    }
}
