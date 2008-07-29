/* $Id: AlarmManager.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 23, 2005.
 * 
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.util;

import java.util.HashMap;
import java.util.Map;
import java.util.TreeSet;

//import metreos.util.Log;

/**
 * The alarm manager is used to implement alarms.
 * 
 * A listener desiring a wakeup call should add itself to the manager
 * with the specified delay in milliseconds. The listener may be removed
 * at any time. When the alarm goes off, the listener may reset or remove
 * the alarm.
 * 
 * @see #add(AlarmListener, Object, int)
 * @see #remove(AlarmListener)
 */
public class AlarmManager extends AbstractStartable implements Runnable
{
	/**
	 * Adds the listener to the set of those getting wakeup calls.
	 * 
	 * Note: uses the statically configured Alarm.
	 * 
	 * @param listener the target of the wakeup call.
	 * 
	 * @param state a bit of state for the listener.
	 * 
	 * @param delay the positive delay in milliseconds before the wakeup call.
	 * 
	 * @see #add(AlarmListener, Object, int)
	 */
	public static void staticAdd( AlarmListener listener, Object state, int delay )
	{
		getAlarmManager( true ).add( listener, state, delay );
	}
	
	/**
	 * Removes the listener from the set of those getting wakeup calls.
	 * 
	 * Note: uses the statically configured Alarm.
	 * 
	 * @param listener the target of the wakeup call.
	 * 
	 * @see #remove(AlarmListener)
	 */
	public static void staticRemove( AlarmListener listener )
	{
		AlarmManager am = getAlarmManager( false );
		if (am != null)
			am.remove( listener );
	}
	
	/**
	 * @param startIt if true, we really need one and if there isn't
	 * one then one should be started, otherwise one should not be
	 * started.
	 * @return the statically available alarm manager, or a newly
	 * allocated one if there is none.
	 */
	public static AlarmManager getAlarmManager( boolean startIt )
	{
		if (alarmManager == null)
		{
			synchronized (AlarmManager.class)
			{
				if (alarmManager == null)
				{
					if (!startIt)
						return null;
					
					try
					{
						AlarmManager am = new AlarmManager();
						am.start();
						alarmManager = am;
					}
					catch ( Exception e )
					{
						e.printStackTrace();
					}
				}
			}
		}
		return alarmManager;
	}
	
	/**
	 * @param newAlarmManager a new AlarmManager to be statically available.
	 * @return the old statically available AlarmManager.
	 */
	public static synchronized AlarmManager setAlarmManager(
		AlarmManager newAlarmManager )
	{
		AlarmManager oldAlarmManager = alarmManager;
		alarmManager = newAlarmManager;
		return oldAlarmManager;
	}
	
	private static AlarmManager alarmManager;
	
	/**
	 * Constructs the AlarmManager.
	 * @param nWorkers the number of worker threads to create to process
	 * wakeups.
	 */
	public AlarmManager( int nWorkers )
	{
		if (nWorkers <= 0)
			throw new IllegalArgumentException( "nWorkers <= 0" );
		
		this.nWorkers = nWorkers;
	}
	
	/**
	 * Constructs the AlarmManager with the default number of
	 * workers (4).
	 */
	public AlarmManager()
	{
		this( DEFAULT_NWORKERS );
	}
	
	private final int nWorkers;
	
	private final static int DEFAULT_NWORKERS = 1;

	@Override
	protected void start0() throws Exception
	{
		clearAlarms();
		clearQueue();
		
		worker = new Thread[nWorkers];
		for (int i = 0; i < nWorkers; i++)
		{
			worker[i] = new Thread( this, "AlarmManager worker thread "+i );
			worker[i].start();
		}
	}
	
	private Thread[] worker;

	@Override
	protected void stop0() throws InterruptedException
	{
		clearAlarms();
		clearQueue();
		notifyAll();
		
		for (int i = 0; i < nWorkers; i++)
		{
			worker[i].join();
			worker[i] = null;
		}
	}
	
	/**
	 * Adds the listener to the set of those getting wakeup calls. If the
	 * listener is already scheduled for a wakeup call, the schedule is
	 * adjusted. There can only be one outstanding wakeup call per listener.
	 * 
	 * This method is thread safe.
	 * 
	 * @param listener the target of the wakeup call.
	 * 
	 * @param state a bit of state for the listener.
	 * 
	 * @param delay the positive delay in milliseconds before the wakeup call.
	 * 
	 * @throws IllegalArgumentException if the delay is less than or equal to
	 * 0.
	 */
	public synchronized void add( AlarmListener listener, Object state, int delay )
	{
		if (delay <= 0)
			throw new IllegalArgumentException( "delay <= 0" );
		
		checkStarted();
		
		long due = Timer.getNanos() + delay * Timer.NANOS_PER_MILLI;
		
		Alarm alarm = getAlarm( listener );
		if (alarm != null)
		{
			// schedule is being adjusted
			dequeue( alarm );
			alarm.setDue( due );
			alarm.setState( state );
			enqueue( alarm );
		}
		else
		{
			alarm = new Alarm( listener, state, due );
			addAlarm( listener, alarm );
			enqueue( alarm );
		}
		
		notifyWorker( "add" );
	}
	
	private void checkStarted()
	{
		if (!isStarted())
			throw new RuntimeException( "not started" );
	}

	private synchronized void update( Alarm alarm, int delay )
	{
		long due = delay > 0
			? alarm.getDue() + delay * Timer.NANOS_PER_MILLI
			: Timer.getNanos() - delay * Timer.NANOS_PER_MILLI;
		
		alarm.setDue( due );
		enqueue( alarm );
		
		notifyWorker( "update" );
	}
	
	/**
	 * Removes any scheduled wakeup call for this listener.
	 * 
	 * This method is thread safe.
	 * 
	 * @param listener the target of the wakeup call.
	 */
	public synchronized void remove( AlarmListener listener )
	{
		checkStarted();
		
		Alarm alarm = removeAlarm( listener );
		if (alarm != null)
			dequeue( alarm );
		
		notifyWorker( "remove" );
	}
	
	private synchronized void remove( Alarm alarm )
	{
		removeAlarm( alarm.listener );
	}
	
	private void wakeup( Alarm alarm )
	{
		try
		{
			int delay = alarm.listener.wakeup( this, alarm.getState(), alarm.getDue() );
			if (delay != 0)
				update( alarm, delay );
			else
				remove( alarm );
		}
		catch ( Exception e )
		{
//			Log.report( "wakeup", "who", alarm.listener, Log.EXCP, e );
			remove( alarm );
		}
	}
	
	private Alarm getNextDueAlarm()
	{
		// ok, the worker needs to get the next due alarm and
		// then wait until its due time, then return it. if alerted
		// by notifyWorker, it should refresh the next due alarm.
		// one trick will be in excluding multiple workers from
		// coming in here at the same time.
		synchronized (getNextDueAlarmSync)
		{
			synchronized (this)
			{
				while (true)
				{
//					Log.report( "AlarmManager.getNextDueAlarm", "who", this, "isStarted", isStarted() );
					if (!isStarted())
						return null;
					
					Alarm alarm = getFirst();
					
					if (alarm == null)
					{
						try
						{
//							Log.report( "AlarmManager.wait", "who", this );
							wait( Long.MAX_VALUE );
							continue;
						}
						catch ( InterruptedException e )
						{
							return null;
						}
					}
					
					long delayNs = alarm.getDue() - Timer.getNanos();
					if (delayNs > 0)
					{
						try
						{
							long delay = delayNs / Timer.NANOS_PER_MILLI;
							if (delay > 0)
								wait( delay );
							continue;
						}
						catch ( InterruptedException e )
						{
							return null;
						}
					}
					
					// the alarm being returned has not been removed
					// from alarmsByListener. it is presumed that the
					// alarm will be set again. if not, it should be
					// removed.
					
					dequeue( alarm );
					return alarm;
				}
			}
		}
	}

	private final Object getNextDueAlarmSync = new Object();
	
	private void notifyWorker( String reason )
	{
//		Log.report( "notify", "who", this, "reason", reason, "where", new Throwable() );
		// the set of alarms has changed.
		notify();
	}
	
	public void run()
	{
//		Log.report( "AlarmManager.start", "who", this );
		try
		{
			Alarm alarm;
			while ((alarm = getNextDueAlarm()) != null)
			{
//				Log.report( "AlarmManager.wakeup", "who", this, "what", alarm );
				wakeup( alarm );
			}
		}
		catch ( RuntimeException e )
		{
//			Log.report( "AlarmManager.wakeup", "who", this, Log.EXCP, e );
		}
		finally
		{
//			Log.report( "AlarmManager.done", "who", this );
		}
	}

	////////////////////////
	// ALARMS BY LISTENER //
	////////////////////////
	
	private Alarm getAlarm( AlarmListener listener )
	{
		return alarmsByListener.get( listener );
	}
	
	private void addAlarm( AlarmListener listener, Alarm alarm )
	{
		alarmsByListener.put( listener, alarm );
	}
	
	private Alarm removeAlarm( AlarmListener listener )
	{
		return alarmsByListener.remove( listener );
	}
	
	private void clearAlarms()
	{
		alarmsByListener.clear();
	}
	
	private final Map<AlarmListener,Alarm> alarmsByListener = new HashMap<AlarmListener,Alarm>();

	////////////////////////
	// ALARMS BY DUE TIME //
	////////////////////////
	
	private Alarm getFirst()
	{
		if (alarms.isEmpty())
			return null;
		
		return alarms.first();
	}
	
	private void enqueue( Alarm alarm )
	{
		alarms.add( alarm );
	}

	private void dequeue( Alarm alarm )
	{
		alarms.remove( alarm );
	}
	
	private void clearQueue()
	{
		alarms.clear();
	}
	
	private final TreeSet<Alarm> alarms = new TreeSet<Alarm>();
	
	/////////////
	// CLASSES //
	/////////////

	private final static class Alarm implements Comparable<Alarm>
	{
		/**
		 * @param listener the target of the wakeup call.
		 * @param state a bit of state for the listener.
		 * @param due the absolute due time for the alarm.
		 */
		Alarm( AlarmListener listener, Object state, long due )
		{
			this.listener = listener;
			this.state = state;
			this.due = due;
		}
		
		/**
		 * @return the state for the listener.
		 */
		Object getState()
		{
			return state;
		}
		
		/**
		 * @param state a new bit of state for the listener.
		 */
		void setState( Object state )
		{
			this.state = state;
		}

		/**
		 * @return the time the alarm is due.
		 */
		long getDue()
		{
			return due;
		}

		/**
		 * @param due
		 */
		void setDue( long due )
		{
			this.due = due;
		}

		@Override
		public int hashCode()
		{
			return (int) ((due ^ (due >>> 32)) ^ (seq ^ (seq >>> 32)));
		}

		@Override
		public boolean equals( Object obj )
		{
			if (obj == this)
				return true;
			
			if (obj == null)
				return false;
			
			if (obj.getClass() != Alarm.class)
				return false;
			
			Alarm other = (Alarm) obj;
			
			return due == other.due && seq == other.seq;
		}

		public int compareTo( Alarm other )
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
		
		/**
		 * The listener for wakeup events.
		 */
		final AlarmListener listener;
		
		/**
		 * Just a bit of state for the listener.
		 */
		private Object state;
		
		/**
		 * The time the alarm is due.
		 */
		private long due;
		
		/**
		 * A unique for all reasonable time sequence number.
		 */
		private final long seq = idGen.next();
		
		private final static IdGenerator idGen = new IdGenerator();
	}

	/**
	 * Shuts down the default alarm manager if there is one.
	 * @throws Exception
	 */
	public static void shutdown() throws Exception
	{
		AlarmManager am = getAlarmManager( false );
		if (am != null)
			am.stop();
	}
}
