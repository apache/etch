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
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Threading;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    
    /// <summary>
    /// The alarm manager is used to implement alarms.
    /// A listener desiring a wakeup call should add itself to the manager
    /// with the specified delay in milliseconds. The listener may be removed
    /// at any time. When the alarm goes off, the listener may reset or remove
    /// the alarm.
    /// </summary>
    public class AlarmManager : AbstractStartable
    {
        /// <summary>
        /// Adds the listener to the set of those getting wakeup calls.
        /// </summary>
        /// <param name="listener">the target of the wakeup call.</param>
        /// <param name="state">a bit of state for the listener.</param>
        /// <param name="delay">the positive delay in milliseconds before the wakeup call.</param>
        public static void staticAdd(AlarmListener listener, Object state, int delay)
        {
            GetAlarmManager(true).Add(listener, state, delay);
        }

        /// <summary>
        /// Removes the listener from the set of those getting wakeup calls.
        /// </summary>
        /// <param name="listener">the target of the wakeup call.</param>
        public static void staticRemove(AlarmListener listener)
        {
            AlarmManager am = GetAlarmManager(false);
            if (am != null)
                am.Remove(listener);
        }

        /// <summary>
        /// Gets the statically configured alarm manager, creating one if
        /// specified and none exists.
        /// </summary>
        /// <param name="startIt">if true, we really need one and if there isn't
        /// one then one should be started, otherwise one should not be started</param>
        /// <returns>he statically configured alarm manager, or a newly
        /// created one if there is none.</returns>
        public static AlarmManager GetAlarmManager(bool startIt)
        {
            if (alarmManager == null)
            {
                lock (alarmManagerSync)
                {
                    if (alarmManager == null)
                    {
                        if (!startIt)
                            return null;

                        AlarmManager am = new AlarmManager();
                        am.Start();
                        alarmManager = am;
                    }
                }
            }

            return alarmManager;
        }

        /// <summary>
        /// Sets the statically configured alarm manager, returning the old one
        /// if any. Don't forget to stop the old one.
        /// </summary>
        /// <param name="newAlarmManager">a new AlarmManager to be statically available</param>
        /// <returns>the old statically available AlarmManager.</returns>
        public static AlarmManager SetAlarmManager(AlarmManager newAlarmManager)
        {
            lock (alarmManagerSync)
            {
                AlarmManager oldAlarmManager = alarmManager;
                alarmManager = newAlarmManager;
                return oldAlarmManager;
            }
        }

        private static AlarmManager alarmManager;

        private readonly static Object alarmManagerSync = new Object();

        /// <summary>
        /// Constructs the AlarmManager.
        /// </summary>
        /// <param name="nWorkers">the number of worker threads to create to process
        /// wakeups. Must be > 0 and < 100.</param>
        public AlarmManager(int nWorkers)
        {
            if (nWorkers <= 0 || nWorkers >= 100)
                throw new ArgumentException("nWorkers <= 0 || nWorkers >= 100");

            this.nWorkers = nWorkers;
        }

        /// <summary>
        /// onstructs the AlarmManager with the default number of workers.
        /// </summary>
        public AlarmManager()
            : this(DEFAULT_NWORKERS)
        {

        }

        private readonly int nWorkers;

        /// <summary>
        /// The default number of worker threads (1).
        /// </summary>
        public const int DEFAULT_NWORKERS = 1;


        protected override void Start0()
        {
            ClearAlarms();
            ClearQueue();

            worker = new Thread[nWorkers];
            for (int i = 0; i < nWorkers; i++)
            {
                worker[i] = new Thread(run);
                worker[i].IsBackground = true;
                worker[i].Start();
            }
        }
        private Thread[] worker;

        protected override void Stop0()
        {
            ClearAlarms();
            ClearQueue();

            lock (this)
            {
                Monitor.PulseAll(this);
            }

            for (int i = 0; i < nWorkers; i++)
            {
                if (worker[i] != null)
                {
                    worker[i].Join();
                    worker[i] = null;
                }
            }
        }

        /// <summary>
        /// Adds the listener to the set of those getting wakeup calls. If the
        /// listener is already scheduled for a wakeup call, the schedule is
        /// adjusted. There can only be one outstanding wakeup call per listener.
        /// This method is thread safe.
        /// </summary>
        /// <param name="listener">the target of the wakeup call.</param>
        /// <param name="state">a bit of state for the listener.</param>
        /// <param name="delay">the positive delay in milliseconds before the wakeup call.</param>
        /// <exception cref="ArgumentException">throws ArgumentException if the delay is less
        /// than or equal to 0</exception>
        [MethodImpl(MethodImplOptions.Synchronized)]
        public void Add(AlarmListener listener, Object state, int delay)
        {
            if (listener == null)
                throw new ArgumentNullException("listener");

            if (delay <= 0)
                throw new ArgumentException("delay <= 0");

            CheckIsStarted();

            long due = HPTimer.Now() + delay * HPTimer.NS_PER_MILLISECOND;

            Alarm alarm = GetAlarm(listener);
            if (alarm != null)
            {
                // schedule is being adjusted
                Dequeue(alarm);
                alarm.setDue(due);
                alarm.setState(state);
                Enqueue(alarm);
            }
            else
            {
                alarm = new Alarm(listener, state, due);
                AddAlarm(listener, alarm);
                Enqueue(alarm);
            }

            NotifyWorker("add");
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void Update(Alarm alarm, int delay)
        {
            long due = delay > 0
                ? alarm.getDue() + delay * HPTimer.NS_PER_MILLISECOND
                : HPTimer.Now() - delay * HPTimer.NS_PER_MILLISECOND;

            alarm.setDue(due);
            Enqueue(alarm);
            NotifyWorker("update");
        }

        /// <summary>
        /// Removes any scheduled wakeup call for this listener.
        /// This method is thread safe.
        /// </summary>
        /// <param name="listener">the target of the wakeup call.</param>
        [MethodImpl(MethodImplOptions.Synchronized)]
        public void Remove(AlarmListener listener)
        {
            CheckIsStarted();

            Alarm alarm = RemoveAlarm(listener);
            if (alarm != null)
            {
                Dequeue(alarm);
                NotifyWorker("remove");
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void Remove(Alarm alarm)
        {
            RemoveAlarm(alarm.listener);
        }

        private void Wakeup(Alarm alarm)
        {
            try
            {
                int delay = alarm.listener.Wakeup(this, alarm.getState(), alarm.getDue());
                if (delay != 0)
                    Update(alarm, delay);
                else
                    Remove(alarm);
            }
            catch (Exception e)
            {
                Remove(alarm);
                Report(e);
            }
        }

        private Alarm getNextDueAlarm()
        {
            // ok, the worker needs to get the next due alarm and
            // then wait until its due time, then return it. if alerted
            // by notifyWorker, it should refresh the next due alarm.
            // one trick will be in excluding multiple workers from
            // coming in here at the same time.
            lock (getNextDueAlarmSync)
            {
                lock (this)
                {
                    while (true)
                    {
                        //					
                        if (!IsStarted())
                            return null;

                        Alarm alarm = getFirst();

                        if (alarm == null)
                        {
                            try
                            {
                                //Console.WriteLine(" Waiting in getNextDueAlarm ");

                                Monitor.Wait(this, Int32.MaxValue);

                                //Console.WriteLine(" Done Waiting in getNextDueAlarm ");

                                continue;
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine(e);
                                return null;
                            }
                        }

                        long delayNs = alarm.getDue() - HPTimer.Now();
                        if (delayNs > 0)
                        {
                            try
                            {
                                long delay = delayNs / HPTimer.NS_PER_MILLISECOND;
                                if (delay > 0)
                                {
                                    int d = (int)delay;
                                    Monitor.Wait(this, d);
                                }
                                continue;
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine(e);
                                return null;
                            }
                        }

                        // the alarm being returned has not been removed
                        // from alarmsByListener. it is presumed that the
                        // alarm will be set again. if not, it should be
                        // removed.

                        Dequeue(alarm);
                        return alarm;
                    }
                }
            }
        }

        private readonly Object getNextDueAlarmSync = new Object();

        private void NotifyWorker(String reason)
        {
            //		Log.report( "notify", "who", this, "reason", reason, "where", new Throwable() );
            // the set of alarms has changed.
            Monitor.Pulse(this);
        }

        public void run()
        {
            try
            {
                Alarm alarm;
                while ((alarm = getNextDueAlarm()) != null)
                {
                    Wakeup(alarm);
                }
            }
            catch (Exception e)
            {
                Report(e);
            }
        }

        private void Report(Exception e)
        {
            Console.WriteLine(e);
        }

        private Alarm GetAlarm(AlarmListener listener)
        {
            Alarm alarm;
            return alarmsByListener.TryGetValue(listener, out alarm) ? alarm : null;
        }

        private void AddAlarm(AlarmListener listener, Alarm alarm)
        {
            alarmsByListener.Add(listener, alarm);
        }

        private Alarm RemoveAlarm(AlarmListener listener)
        {
            Alarm alarm = GetAlarm(listener);
            if (alarm != null)
                alarmsByListener.Remove(listener);
            return alarm;
        }

        private void ClearAlarms()
        {
            alarmsByListener.Clear();
        }

        private readonly Dictionary<AlarmListener, Alarm> alarmsByListener = new Dictionary<AlarmListener, Alarm>();

        ////////////////////////
        // ALARMS BY DUE TIME //
        ////////////////////////

        private Alarm getFirst()
        {
            if (alarms.Count == 0)
                return null;

            return alarms[0];
        }

        private void Enqueue(Alarm alarm)
        {
            alarms.Add(alarm);
            alarms.Sort();
        }

        private void Dequeue(Alarm alarm)
        {
            alarms.Remove(alarm);

        }

        private void ClearQueue()
        {
            alarms.Clear();
        }

        private readonly List<Alarm> alarms = new List<Alarm>();


        private class Alarm : IComparable<Alarm>
        {
            /// <summary>
            /// 
            /// </summary>
            /// <param name="listener">the target of the wakeup call.</param>
            /// <param name="state">a bit of state for the listener.</param>
            /// <param name="due">he absolute due time for the alarm.</param>
            public Alarm(AlarmListener listener, Object state, long due)
            {
                this.listener = listener;
                this.state = state;
                this.due = due;
            }

            /// <summary>
            /// 
            /// </summary>
            /// <returns>the state for the listener.</returns>
            public Object getState()
            {
                return state;
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="state">a new bit of state for the listener./param>
            public void setState(Object state)
            {
                this.state = state;
            }

            /// <summary>
            /// 
            /// </summary>
            /// <returns>the time the alarm is due.</returns>
            public long getDue()
            {
                return due;
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="due"></param>
            public void setDue(long due)
            {
                this.due = due;
            }


            public override int GetHashCode()
            {
                return (int)((due ^ (due >> 32)) ^ (seq ^ (seq >> 32)));
            }


            public override bool Equals(Object obj)
            {
                if (obj == this)
                    return true;

                if (obj == null)
                    return false;

                if (obj.GetType() != typeof(Alarm))
                    return false;

                Alarm other = (Alarm)obj;

                return due == other.due && seq == other.seq;
            }

            public int CompareTo(Alarm other)
            {
                if (due < other.due)
                    return -1;

                if (due > other.due)
                    return 1;

                // due time is the same for both, now we need to
                // compare the seq.

                if (seq < other.seq)
                    return -1;

                if (seq > other.seq)
                    return 1;

                return 0;
            }

            /// <summary>
            /// The listener for wakeup events.
            /// </summary>
            public readonly AlarmListener listener;

            /// <summary>
            /// Just a bit of state for the listener.
            /// </summary>
            private Object state;

            /// <summary>
            /// The time the alarm is due.
            /// </summary>
            private long due;

            /// <summary>
            /// A unique for all reasonable time sequence number.
            /// </summary>
            private readonly long seq = idGen.Next();

            private readonly static IdGenerator idGen = new IdGenerator();
        }

        /// <summary>
        /// Shuts down the default alarm manager if there is one.
        /// </summary>
        public static void shutdown()
        {
            AlarmManager am = SetAlarmManager(null);
            if (am != null)
                am.Stop();
        }



    }
}
