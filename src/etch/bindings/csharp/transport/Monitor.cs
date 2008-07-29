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
        
        public Monitor( String description, T initialValue )
        {
            this.description = description;
            this.value = initialValue;
        }

        public Monitor( String description ) : this( description, default( T ) )
        {   
        }

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
        /// <param name="newValue"></param>
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
        /// Waits until the monitor value is set.
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
        /// 
        /// </summary>
        /// <param name="desiredValue">Desired value</param>
        /// <param name="newValue">New value</param>
        /// <returns>The old value</returns>
        /// Exception: 
        ///     throws Exception
        public T WaitUntilEqAndSet( T desiredValue, T newValue )
        {
            return WaitUntilEqAndSet( desiredValue, 0, newValue );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="desiredValue">desired value</param>
        /// <param name="maxDelay">The maximum delay</param>
        /// <param name="newValue">New value</param>
        /// <returns>Old Value</returns>
        /// Exception:  
        ///     throws ThreadInterruptedException
        [MethodImpl( MethodImplOptions.Synchronized )]
        public T WaitUntilEqAndSet( T desiredValue, long maxDelay, T newValue )
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
        /// Waits until the value is set to the specified value.
        /// </summary>
        /// <param name="desiredValue">The value we are waiting for.</param>
        /// <param name="maxDelay">the max amount of time in ms to wait.
        /// If 0 is specified, we will wait forever.</param>
        /// Exception:
        ///     throws ThreadInterruptedException if we waited too long.
        [MethodImpl( MethodImplOptions.Synchronized )]
        public void WaitUntilEq( T desiredValue, long maxDelay )
        {
            DateTime nowInit = DateTime.Now;
            DateTime nowNext;
            TimeSpan span;

            long now = 0;

            long endTime = maxDelay > 0 ? now + maxDelay : int.MaxValue;

            while ( !Eq( value, desiredValue ) && now < endTime )
            {
                WaitUntilSet( endTime - now );
                //now = DateTime.Now.Millisecond;
                nowNext = DateTime.Now;
                span = ( nowNext - nowInit );
                now = (long) span.TotalMilliseconds;
            }

            if ( !Eq( value, desiredValue ) )
                throw new ThreadInterruptedException( "timeout" );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="undesiredValue">Undesired Value</param>
        /// <param name="newValue">New value</param>
        /// <returns>Old value</returns>
        /// Exception:
        ///     throws ThreadInterruptedException
        public T WaitUntilNotEqAndSet( T undesiredValue, T newValue )
        {
            return WaitUntilNotEqAndSet( undesiredValue, 0, newValue );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="undesiredValue">Undesired value</param>
        /// <param name="maxDelay">Max delay</param>
        /// <param name="newValue">New value</param>
        /// <returns>The old value</returns>
        /// Exception:
        ///     throws ThreadInterrupedException
        [MethodImpl( MethodImplOptions.Synchronized )]
        public T WaitUntilNotEqAndSet( T undesiredValue,
            long maxDelay, T newValue )
        {
            WaitUntilNotEq( undesiredValue );
            return Set( newValue );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="undesiredValue">Undesired value</param>
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
        public T WaitUntilNotEq( T undesiredValue, long maxDelay )
        {
            DateTime nowInit = DateTime.Now;
            DateTime nowNext;
            TimeSpan span;

            long now = 0;

            long endTime = maxDelay > 0 ? now + maxDelay : int.MaxValue;

            while ( Eq( value, undesiredValue ) && now < endTime )
            {
                WaitUntilSet( endTime - now );

                nowNext = DateTime.Now;
                span = ( nowNext - nowInit );
                nowInit = nowNext;
                now = ( long ) span.TotalMilliseconds;
            }

            if ( Eq( value, undesiredValue ) )
                throw new ThreadInterruptedException("timeout");

            return value;
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
