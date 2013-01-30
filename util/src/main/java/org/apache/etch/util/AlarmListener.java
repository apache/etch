/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.util;

/**
 * Classes wanting alarms must implement this interface to receive the
 * alarm.
 * 
 * @see AlarmManager
 */
public interface AlarmListener
{
	/**
	 * Reports that the listener's alarm went off.
	 * 
	 * @param manager the manager of the alarm.
	 * 
	 * @param state a bit of state for the listener.
	 * 
	 * @param due the time the alarm was set to go off. This is in the
	 * time coordinates as returned by Timer.getNanos().
	 * 
	 * @return the positive or negative snooze time in ms for this alarm,
	 * or 0 to shutoff the alarm. If a positive number is returned, it is
	 * added to the current due time of the alarm to calculate the new
	 * due time for the alarm. Thus there should be very little drift
	 * in the frequency of the alarms (although there is some because
	 * of system clock jitter).
	 * 
	 * If the system is very busy, or if the listeners are trying to
	 * do too much, alarms may not be processed quickly enough and
	 * calculated new due times will already be in the past. This is a
	 * pathological condition which will spiral out of control if not
	 * detected.
	 * 
	 * If a negative number is returned, it is subtracted from the current
	 * value of Timer.getNanos() to calculate the new due time for the
	 * alarm. (In this case, subtracting the negative number is the same
	 * as adding the absolute value...)
	 * 
	 * @see Timer#getNanos()
	 */
	public int wakeup( AlarmManager manager, Object state, long due );
}
