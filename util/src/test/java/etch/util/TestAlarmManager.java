/*
 * $Id$
 * 
 * Copyright 2007-2008 Cisco Systems Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package etch.util;

import org.junit.Test;
import static org.junit.Assert.*;

/** Test of AlarmManager */
public class TestAlarmManager
{
	private final static int Q1 = 30;
	private final static int Q2 = 60;
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void start1() throws Exception
	{
		// is not started
		AlarmManager am = new AlarmManager();
		
		MyAlarmListener listener = new MyAlarmListener( 0 );
		assertEquals( 0, listener.delay );
		assertFalse( listener.wake );
		assertNull( listener.state );
		
		am.add( listener, null, Q1 );
	}
	
	/** @throws Exception */
	@Test
	public void start2() throws Exception
	{
		// is not started
		AlarmManager am = new AlarmManager();
		
		MyAlarmListener listener = new MyAlarmListener( 0 );
		assertEquals( 0, listener.delay );
		assertFalse( listener.wake );
		assertNull( listener.state );
		
		am.start();
		
		am.add( listener, null, Q1 );
		assertFalse( listener.wake );
		assertNull( listener.state );
		
		Thread.sleep( Q2 );
		assertTrue( listener.wake );
		assertNull( listener.state );
	}
	
	/** @throws Exception */
	@Test
	public void start3() throws Exception
	{
		// is not started
		AlarmManager am = new AlarmManager();
		
		MyAlarmListener listener = new MyAlarmListener( 0 );
		assertEquals( 0, listener.delay );
		assertFalse( listener.wake );
		assertNull( listener.state );
		
		am.start();
		
		am.add( listener, null, Q1 );
		assertFalse( listener.wake );
		assertNull( listener.state );
		
		Thread.sleep( Q2 );
		assertTrue( listener.wake );
		assertNull( listener.state );
		
		am.stop();
	}

	private static class MyAlarmListener implements AlarmListener
	{
		public MyAlarmListener( int delay )
		{
			this.delay = delay;
		}

		private int delay;
		
		private boolean wake;
		
		private Object state;
		
		public int wakeup( AlarmManager manager, Object state, long due )
		{
			wake = true;
			this.state = state;
			return delay;
		}
	}
}
