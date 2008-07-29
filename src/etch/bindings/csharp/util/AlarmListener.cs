using System;
using System.Text;
using System.Collections.Generic;
using Etch.Transport;

namespace Etch.Util
{
    /// <summary>
    /// Classes wanting alarms must implement this interface to receive the
    /// alarm.
    /// </summary>
    public interface AlarmListener
    {
        /// <summary>
        /// Reports that the listener's alarm went off.
        /// </summary>
        /// 
        /// <param name="manager">the manager of the alarm.</param>
        /// 
        /// <param name="state">a bit of state for the listener.</param>
        /// 
        /// <param name="due">due the time the alarm was set to go off. This is in the
        /// time coordinates as returned by Timer.GetNanos().</param>
        /// 
        /// <returns>
        /// the positive or negative snooze time in ms for this alarm,
        /// or 0 to shutoff the alarm. If a positive number is returned, it is
        /// added to the current due time of the alarm to calculate the new
        /// due time for the alarm. Thus there should be very little drift
        /// in the frequency of the alarms (although there is some because
        /// of system clock jitter).
        /// 
        /// If the system is very busy, or if the listeners are trying to
        /// do too much, alarms may not be processed quickly enough and
        /// calculated new due times will already be in the past. This is a
        /// pathological condition which will spiral out of control if not
        /// detected.
        /// 
        /// If a negative number is returned, it is subtracted from the current
        /// value of Timer.GetNanos() to calculate the new due time for the
        /// alarm. (In this case, subtracting the negative number is the same
        /// as adding the absolute value...)
        /// </returns>
        /// 
        /// <see cref="Timer.GetNanos()"/>


        int Wakeup( AlarmManager manager, Object state, long due );
    }
}
