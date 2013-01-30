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

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;

/**
 * Log implements a standardized reporting mechanism for named
 * events which get reported to a variety of sinks in a variety
 * of formats.
 * 
 * NOTE: Log.report is not a substitute for System.out.printf.
 * 
 * Example:
 * 
 * Log.report( "SystemUp", "time", new Date() );
 * 
 * The event is "SystemUp", and there is one additional parameter
 * named "time" which has a value of the current date/time.
 * 
 * Under the hood, the event is used to select a Template,
 * which then controls how the event is handled. The template
 * knows the importance of the event, perhaps how the event
 * is formatted, etc. It is done this way to minimize how much
 * work is done until it is time to actually do something with
 * the log entry. For example, if the log entry is not important
 * enough, it will be ignored. Any work done to format it will be
 * wasted effort. Furthermore, a preformmated log entry cannot be
 * localized.
 */
public class Log
{
	private Log()
	{
		// cannot create an instance of this class.
	}

	/////////////////////////
	// DEFAULT DATE FORMAT //
	/////////////////////////
	
	/**
	 * The default date format used by entry formatters.
	 */
	public static DateFormat defaultDateFormat
		= new SimpleDateFormat( "HH:mm:ss.SSS" );
	
	///////////////
	// TEMPLATES //
	///////////////

	/**
	 * Registers the template, making it globally available. It isn't necessary
	 * to register a template to use it directly, but Log.report only works
	 * with registered templates.
	 * 
	 * @param template the template to register.
	 * 
	 * @return the template that was registered.
	 * 
	 * @throws IllegalArgumentException if the template name has already
	 * been registered.
	 */
	public static Template register( Template template )
	{
		synchronized (templates)
		{
			if (hasTemplate( template.getName() ))
				throw new IllegalArgumentException(
					"template '"+template.getName()+"' already registered" );
			addTemplate( template );
		}
		
		synchronized (template)
		{
			sync( template );
		}
		
		return template;
	}

	/**
	 * Creates a template for an event and registers it all in one step.
	 * 
	 * @param name the name of the event.
	 * @param category the category of the event.
	 * @param enable flag which allows reporting of the event.
	 * @param fields the list of fields of the event.
	 * 
	 * @return the template that was created and registered.
	 * 
	 * @throws IllegalArgumentException if the template name has already
	 * been registered.
	 */
	public static Template register( String name, Category category,
			boolean enable, String ... fields )
		{
			return register( new Template( name, category, enable, Template.UNLIMITED_ENTRIES, fields ) );
		}
		
	/**
	 * Register and set log entry limit
	 * @param name
	 * @param category
	 * @param enable
	 * @param limit
	 * @param fields
	 * @return the template that was created and registered.
	 */
	public static Template register( String name, Category category,
		boolean enable, int limit, String ... fields )
	{
		return register( new Template( name, category, enable, limit, fields ) );
	}
	
	/**
	 * @param name the name of the event.
	 * @return true if the named template exists.
	 */
	public static boolean hasTemplate( String name )
	{
		return templates.containsKey( name );
	}

	private static void addTemplate( Template template )
	{
		templates.put( template.getName(), template );
	}
	
	private static Template getTemplate0( String name )
	{
		return templates.get( name );
	}
	
	/**
	 * @param name the name of the event.
	 * 
	 * @param register flag determines if a newly created template
	 * will be registered or not. The template should not be registered
	 * if the "name" of the event is really an already formatted string
	 * which should not be remembered. this might happen if legacy logging
	 * or tracing code is being converted (e.g., by changing calls to
	 * System.out.println() into Log.report().
	 * 
	 * @param defCat the default category if the template does not
	 * exist.
	 * 
	 * @param defEnable the default enable if the template does not
	 * exist.
	 * 
	 * @return the template with the specified name if registed, or
	 * a new template with specified name and default category and
	 * enable. The new template may or may not be registered depending
	 * up the value of the register parameter.
	 * 
	 * @see #getTemplate(String)
	 */
	public static Template getTemplate( String name, boolean register,
		Category defCat, boolean defEnable )
	{
		Template template = getTemplate0( name );
		if (template == null)
		{
			if (!register)
				return new Template( name, defCat, defEnable, Template.UNLIMITED_ENTRIES );

			synchronized (templates)
			{
				template = getTemplate0( name );
				if (template == null)
					template = register( name, defCat, defEnable );
			}
		}
		return template;
	}
	
	/**
	 * @param name the name of the event.
	 * 
	 * @return the template with the specified name if registered, or
	 * a new template with specified name and default category and
	 * default enable. The new template may or may not be registered
	 * depending upon the value of the allowAutoRegister global.
	 * 
	 * @see #getTemplate(String, boolean, Category, boolean)
	 * @see #getAllowAutoRegister()
	 * @see #getDefaultCategory()
	 * @see #getDefaultEnable()
	 * @see #getDefaultLimit()
	 */
	public static Template getTemplate( String name )
	{
		return getTemplate( name, getAllowAutoRegister(),
			getDefaultCategory(), getDefaultEnable() );
	}
	
	/**
	 * @return the default enable flag for unknown events.
	 */
	public static boolean getDefaultEnable()
	{
		return defaultEnable;
	}
	
	/**
	 * @return the default limit for unknown events.
	 */
	public static int getDefaultLimit()
	{
		return defaultLimit;
	}
	
	/**
	 * @return the default category for unknown events.
	 */
	public static Category getDefaultCategory()
	{
		return defaultCategory;
	}

	/**
	 * @return the value of the auto register flag, which controls
	 * whether Log.report(name, ...) and Log.getTemplate(name) will
	 * register a newly created template.
	 */
	public static boolean getAllowAutoRegister()
	{
		return allowAutoRegister;
	}
	
	/**
	 * Sets the value of the auto register flag.
	 * 
	 * @param value the new value of the auto register flag.
	 * 
	 * @see #getTemplate(String)
	 */
	public static void setAllowAutoRegister( boolean value )
	{
		allowAutoRegister = value;
	}
	
	private static boolean allowAutoRegister = true;
	
	private static Category defaultCategory = Category.Debug;
	
	private static boolean defaultEnable = true;
	
	private static int defaultLimit = Template.UNLIMITED_ENTRIES;
	
	private final static Map<String, Template> templates =
		Collections.synchronizedMap( new HashMap<String, Template>() );

	///////////
	// SINKS //
	///////////

	/**
	 * Adds the specified sink to the set of sinks which
	 * desire notification of events.
	 * @param sink
	 */
	public static void addSink( Sink sink )
	{
		if (sink == null)
		{
			// HACK: this is special hack that adds a default
			// log sink which prints to standard out. But we
			// only want one, so while a nullSink is active,
			// don't add another.

			if (nullSink != null)
				return;
			
			nullSink = sink = new Sink()
			{
				public synchronized void start()
				{
					checkIsStarted();
					started = true;
					sync( this );
				}

				public boolean isStarted()
				{
					return started;
				}

				public boolean hasInterest( Template template )
				{
					return started;
				}
				
				public synchronized void report( Entry entry )
				{
					System.out.println( entry );
				}

				public synchronized void stop()
				{
					started = false;
					sync( this );
				}
				
				private boolean started = true;

				public String getName()
				{
					return null;
				}

				public void setAcceptCat( String tokenList )
				{
					// ignore.
				}

				public void setDenyCat( String tokenList )
				{
					// ignore.
				}

				public void setAcceptName( String tokenList )
				{
					// ignore.
				}

				public void setDenyName( String tokenList )
				{
					// ignore.
				}

				public void checkIsStarted()
				{
					if (started)
						throw new IllegalStateException( "already started" );
				}
			};
		}
		
		synchronized (sink)
		{
			if (sinks.add( sink ))
				sync( sink );
			
			String name = sink.getName();
			if (name != null)
				sinksByName.put( name, sink );
		}
	}
	
	private static Sink nullSink;
	
	/**
	 * Removes the specified sink from the set of sinks which
	 * desire notification of events.
	 * @param sink
	 */
	public static void removeSink( Sink sink )
	{
		synchronized (sink)
		{
			if (sinks.remove( sink ))
				sync( sink );
			
			String name = sink.getName();
			if (name != null)
				sinksByName.remove( name );
			
			if (sink == nullSink)
				nullSink = null;
		}
	}
	
	/**
	 * @param name
	 * @return the named sink or null.
	 */
	public static Sink findSink( String name )
	{
		return sinksByName.get( name );
	}

	/**
	 * @param sink
	 */
	public static void sync( Sink sink )
	{
		// sink is synchronized.
		
		// the interests of this sink must be compared to all the
		// templates. the list of templates must be frozen so that
		// we are sure all templates are fairly considered. if a
		// template is added or removed, a similar process is
		// performed but freezing the sink list.
		
		boolean installed = sinks.contains( sink );
		synchronized (templates)
		{
			for (Template template : templates.values())
			{
				if (installed && sink.hasInterest( template ))
					template.addSink( sink );
				else
					template.removeSink( sink );
			}
		}
	}
	
	/**
	 * @param template
	 */
	public static void sync( Template template )
	{
		// template is synchronized at this point. templates
		// are essentially immutable except for the enable
		// flag.

		if (!template.getEnable())
		{
			template.clearSinks();
			return;
		}
		
		synchronized (sinks)
		{
			for (Sink sink : sinks)
			{
				if (sink.hasInterest( template ))
					template.addSink( sink );
				else
					template.removeSink( sink );
			}
		}
	}
	
	private final static Set<Sink> sinks =
		Collections.synchronizedSet( new HashSet<Sink>() );
	
	private final static Map<String, Sink> sinksByName =
		Collections.synchronizedMap( new HashMap<String, Sink>() );
	
	/**
	 * @return a list of the sinks.
	 */
	public static Sink[] getSinks()
	{
		synchronized (sinks)
		{
			return sinks.toArray( new Sink[sinks.size()] );
		}
	}
	
	///////////////////
	// REPORT EVENTS //
	///////////////////
	
	/**
	 * Reports the named event with the specified parameters.
	 * @param name the name of the event.
	 * @param fieldValues a sequence of field/value pairs which are
	 * parameters of the event.
	 * @return the entry that was reported.
	 */
	public static Entry report( String name, Object ... fieldValues )
	{
		return newEntry( name, fieldValues ).report();
	}
	
	/**
	 * Creates a new entry for the named event with the specified
	 * parameters.
	 * @param name the name of the event.
	 * @param fieldValues a sequence of field/value pairs which are
	 * parameters of the event.
	 * @return the entry that was created.
	 */
	public static Entry newEntry( String name, Object ... fieldValues )
	{
		return getTemplate( name ).newEntry( fieldValues );
	}
	
	/**
	 * Log entry id generator.
	 */
	final static IdGenerator entryIdGen = new IdGenerator( Timer.getNanos() );

	/**
	 * Standard way of attaching an exception to a log entry.
	 */
	public static final String EXCP = "excp";

	//////////////////
	// stack traces //
	//////////////////
	
	/**
	 * @param t
	 * @return the string rep of the stack trace of the specified throwable.
	 */
	public static String getStackTrace( Throwable t )
	{
		StringWriter sw = new StringWriter();
		PrintWriter pw = new PrintWriter( sw );
		t.printStackTrace( pw );
		return sw.toString().trim();
	}

	/////////////
	// Classes //
	/////////////
	
	/**
	 * Categories of events. Category may be reported when the event is
	 * logged, and sinks may use category to select events to be logged.
	 */
	public enum Category
	{
		/**
		 * Very detailed info about a process.
		 */
		Verbose,
		/**
		 * General info about a process.
		 */
		Info,
		/**
		 * A warning about something that may not be right in a process.
		 */
		Warning,
		/**
		 * A report about an error in a process.
		 */
		Error,
		/**
		 * A debugging report about a process.
		 */
		Debug,
		/**
		 * An audit report from a process.
		 */
		Audit,
		/**
		 * A statistic report.
		 */
		Stat
	}
	
	/**
	 * A template for an event.
	 */
	final public static class Template
	{
		/**
		 * Constructs the Template.
		 *
		 * @param name the name of the event.
		 * @param category the category of the event.
		 * @param enable flag which allows reporting of the event.
		 * @param limit the limit on how many such messages per period
		 * to allow.
		 * @param fields the list of fields of the event.
		 */
		public Template( String name, Category category,
			boolean enable, int limit, String ... fields )
		{
			this.name = name;
			this.category = category;
			this.limit = limit;
			setEnable( enable );
			addFields( fields );
		}
		
		/**
		 * Flag indicating unlimited entry rate.
		 */
		public static final int UNLIMITED_ENTRIES = -1;

		private final String name;
		
		private final Category category;
		
		private int limit = UNLIMITED_ENTRIES;
		
		private int dropCount = 0;
		
		private int minuteCount = 0;
		
		private long lastNanos;
								

		/**
		 * @return the name of this template.
		 */
		public String getName()
		{
			return name;
		}
		
		/**
		 * @return the category of this template.
		 */
		public Category getCategory()
		{
			return category;
		}
		
		/**
		 * @return entries per minutes
		 */
		public int getLimit()
		{
			return limit;
		}
		
		/**
		 * set log entries per minute
		 * @param limit
		 */
		public void setLimit( int limit )
		{
			this.limit = limit;
		}

		/**
		 * @return dropped entries count
		 */
		public int getDropCount()
		{
			return dropCount;
		}
		
		/**
		 * set drop count to 0
		 */
		public void resetDropCount()
		{
			dropCount = 0;
		}
				
		////////////////
		// ENABLEMENT //
		////////////////
		
		/**
		 * @return the enable flag of this template. If true, the
		 * event will be reported to sinks that are interested. If
		 * false, the event will not be reported no matter what.
		 */
		public boolean getEnable()
		{
			return enable;
		}
		
		/**
		 * Sets the enable flag of this template.
		 * 
		 * @param enable the enable flag of this template. If true, the
		 * event will be reported to sinks that are interested. If
		 * false, the event will not be reported no matter what.
		 */
		public synchronized void setEnable( boolean enable )
		{
			if (enable != this.enable)
			{
				this.enable = enable;
				sync( this );
			}
			
			if (isLimitedLogging())
			{
				lastNanos = Timer.getNanos();
				dropCount = 0;
				minuteCount = 0;
			}
		}
		
		private boolean enable = true;

		////////////
		// FIELDS //
		////////////
		
		/**
		 * @param field the field name.
		 * @return true if this field is defined.
		 */
		public boolean hasField( String field )
		{
			return _fields.contains( field );
		}
		
		/**
		 * Adds the specified field to the template if it hasn't already
		 * been added.
		 * @param field the field name.
		 * @return this template.
		 */
		public Template addField( String field )
		{
			synchronized (_fields)
			{
				if (!hasField( field ))
				{
					int id = _fields.size();
					_fields.add( field );
					fieldIds.put( field, id );
				}
			}
			return this;
		}
		
		/**
		 * @param field the field name.
		 * @param create flag which specifies that the field should be
		 * created if it doesn't already exist.
		 * @return the id of the specified field.
		 */
		public int getFieldId( String field, boolean create )
		{
			Integer id = fieldIds.get( field );
			if (id == null)
			{
				if (!create)
					return -1;
				addField( field );
				id = fieldIds.get( field );
			}
			return id;
		}
		
		/**
		 * @return the count of fields in the template.
		 */
		public int countFields()
		{
			return _fields.size();
		}
		
		/**
		 * @param fields a list of fields to add.
		 * @return the template.
		 */
		public Template addFields( String ... fields )
		{
			for (String field: fields)
				addField( field );
			
			return this;
		}

		/**
		 * @return a list of the fields of the template.
		 */
		public String[] getFields()
		{
			synchronized (_fields)
			{
				return _fields.toArray( new String[_fields.size()] );
			}
		}
		
		private List<String> _fields =
			Collections.synchronizedList( new ArrayList<String>() );
		
		private Map<String, Integer> fieldIds =
			Collections.synchronizedMap( new HashMap<String, Integer>() );

		///////////
		// SINKS //
		///////////
		
		/**
		 * @return true if the event is enabled and there are any
		 * sinks interested in the event.
		 */
		boolean isEnabled()
		{
			return enable && hasSinks();
		}
		
		/**
		 * @return true if there are any log sinks interested in this
		 * template.
		 */
		private boolean hasSinks()
		{
			return !tsinks.isEmpty();
		}

		/**
		 * Adds sink to the set of those notified when this
		 * template generates an event.
		 * @param sink
		 */
		void addSink( Sink sink )
		{
			synchronized (tsinks)
			{
				tsinks.add( sink );
				sinksArray = null;
			}
		}

		/**
		 * Removes sink from the set of those notified when this
		 * template generates an event.
		 * @param sink
		 */
		void removeSink( Sink sink )
		{
			synchronized (tsinks)
			{
				tsinks.remove( sink );
				sinksArray = null;
			}
		}

		/**
		 * Removes all the sinks.
		 */
		void clearSinks()
		{
			synchronized (tsinks)
			{
				tsinks.clear();
				sinksArray = null;
			}
		}

		/**
		 * @return the list of sinks interested in this event.
		 */
		public Sink[] getSinks()
		{
			synchronized (tsinks)
			{
				if (sinksArray == null)
					sinksArray = tsinks.toArray( new Sink[tsinks.size()] );
				return sinksArray;
			}
		}
		
		private Set<Sink> tsinks = new HashSet<Sink>();
		
		private Sink[] sinksArray;

		/////////////
		// ENTRIES //
		/////////////
		
		/**
		 * Constructs a new entry for this event with the specified
		 * parameter values. If the event is enabled and some sink
		 * wants it, then a functional entry object is returned which
		 * can be used to report the event. If the event is disabled
		 * or no sink wants it, a dummy entry object is returned which
		 * doesn't report anything.
		 * 
		 * @param fieldValues a sequence of field/value pairs which are
		 * parameters of the event.
		 * 
		 * @return constructed entry object which is used to report
		 * the event.
		 * 
		 * @see NormalEntry
		 * @see DummyEntry
		 */
		public Entry newEntry( Object ... fieldValues )
		{
			synchronized (countSync)
			{
				count++;
				
				int k = fieldValues.length;
				fieldValuesCount += k;
				if (k == 0)
					zeroFieldValuesCount++;
			}
			
			if (isEnabled())
			{
				if (isLimitedLogging())
				{
					if (isLoggingTooFast())
					{
						dropCount++;
						synchronized(countSync)
						{
							dummyCount++;
						}
						return new DummyEntry();
					}
					minuteCount++;
				}
				return new NormalEntry( this, fieldValues );
			}
			
			synchronized (countSync)
			{
				dummyCount++;
			}
			
			return new DummyEntry();
		}

		private boolean isLoggingTooFast()
		{
			if (!isLimitedLogging())
				return false;	// logging is not limited.
			
			if (Timer.getSecondsSince(lastNanos) > 60)
			{
				// over a minute now, reset counters.
				minuteCount = 0;
				//dropCount = 0;
				// reset time stamp.
				lastNanos = Timer.getNanos();
				
				return false;
			}
			if (minuteCount < getLimit())
			{
				// still within limit.
				return false;
			}
			
			return true;
		}

		private boolean isLimitedLogging()
		{
			return limit != UNLIMITED_ENTRIES;
		}
	}
	
	/**
	 * Count of log entries created.
	 */
	static int count;
	
	/**
	 * Count of dummy log entries created.
	 */
	static int dummyCount;
		
	/**
	 * Count of field value pairs reported.
	 */
	static int fieldValuesCount;
	
	/**
	 * Description of zeroFieldValuesCount.
	 */
	static int zeroFieldValuesCount;
	
	/**
	 * Synchronize access to counters.
	 */
	final static Object countSync = new Object();

	private static final String LOG_STATS = "LogStats";

	private static final String COUNT = "count";

	private static final String DUMMY_COUNT = "dummyCount";
	
	private static final String FIELD_VALUES_COUNT = "fieldValuesCount";

	private static final String ZERO_FIELD_VALUES_COUNT = "zeroFieldValuesCount";
	
	/**
	 * Reports log stats.
	 */
	public static void reportLogStats()
	{
		synchronized (countSync)
		{
			Log.report( LOG_STATS,
				COUNT, count,
				DUMMY_COUNT, dummyCount,
				FIELD_VALUES_COUNT, fieldValuesCount,
				ZERO_FIELD_VALUES_COUNT, zeroFieldValuesCount);
		}
	}
	
	static
	{
		Log.register( LOG_STATS, Category.Stat, true,
			COUNT, DUMMY_COUNT, FIELD_VALUES_COUNT, ZERO_FIELD_VALUES_COUNT );
	}
	
	/**
	 * Interface for log entries.
	 */
	public interface Entry
	{
		/**
		 * @return the template for this entry.
		 */
		public Template getTemplate();
		
		/**
		 * @return the timestamp for this entry. This is the
		 * value returned by System.currentTimeMillis() when
		 * the entry was created.
		 */
		public long getTimestamp();
		
		/**
		 * @param field the field name.
		 * @return true if the field is defined and has a value.
		 */
		public boolean hasField( String field );
		
		/**
		 * @param field the field name.
		 * @return the field's value (possibly null).
		 */
		public Object getField( String field );
		
		/**
		 * Sets the value of the field.
		 * @param field the field name.
		 * @param value the field value, may be null.
		 * @return this entry.
		 */
		public Entry setField( String field, Object value );
		
		/**
		 * @return reports this entry to interested log sinks. An
		 * entry id is assigned if the entry is actually going to
		 * be reported to a sink.
		 */
		public Entry report();
		
		/**
		 * Calls the FieldValueFormatter.format() method for each field
		 * defined in the template (even those with null values).
		 * 
		 * @param formatter
		 * @throws IOException
		 */
		public void formatFieldValues( FieldValueFormatter formatter )
			throws IOException;
		
		/**
		 * Converts entry to string value using standard date format.
		 * @return string value of entry
		 */
		public String toString();
		
		/**
		 * Converts entry to string value using standard date format.
		 * @param brief
		 * @return string value of entry
		 */
		public String toString( boolean brief );

		/**
		 * Converts entry to string using specified date format.
		 * @param df
		 * @return string value of entry
		 */
		public String toString( DateFormat df );

		/**
		 * Converts entry to string using specified date format.
		 * @param brief
		 * @param df
		 * @return string value of entry
		 */
		public String toString( boolean brief, DateFormat df );
		
		/**
		 * @param wtr
		 * @throws IOException
		 */
		public void write( Writer wtr ) throws IOException;
		
		/**
		 * @param wtr
		 * @param brief
		 * @throws IOException
		 */
		public void write( Writer wtr, boolean brief ) throws IOException;
		
		/**
		 * @param wtr
		 * @param df
		 * @throws IOException
		 */
		public void write( Writer wtr, DateFormat df ) throws IOException;
		
		/**
		 * @param wtr
		 * @param brief
		 * @param df
		 * @throws IOException
		 */
		public void write( Writer wtr, boolean brief, DateFormat df ) throws IOException;
		
		/**
		 * @return the id assigned to the entry when it was logged.
		 * If the entry was not logged, returns 0.
		 */
		public long getEntryId();
	}
	
	/**
	 * Basic implementation of entry which includes support
	 * for template and timestamp.
	 */
	abstract public static class AbstractEntry implements Entry
	{
		/**
		 * Constructs the AbstractEntry with the specified template
		 * and timestamp.
		 *
		 * @param template the template which created this entry.
		 * @param timestamp the current value of System.currentTimeMillis();
		 */
		public AbstractEntry( Template template, long timestamp )
		{
			this.template = template;
			this.timestamp = timestamp;
		}
		
		private final Template template;
		
		private final long timestamp;

		final public Template getTemplate()
		{
			return template;
		}

		final public long getTimestamp()
		{
			return timestamp;
		}

		@Override
		final public String toString()
		{
			return toString( false, defaultDateFormat );
		}

		final public String toString( boolean brief )
		{
			return toString( brief, defaultDateFormat );
		}

		final public String toString( DateFormat df )
		{
			return toString( false, df );
		}

		final public String toString( boolean brief, DateFormat df )
		{
			try
			{
				StringWriter sw = new StringWriter();
				write( sw, brief, df );
				return sw.toString();
			}
			catch ( IOException e )
			{
				return "format failed, caught exception "+e;
			}
		}

		final public void write( Writer wtr ) throws IOException
		{
			write( wtr, false, defaultDateFormat );
		}

		final public void write( Writer wtr, boolean brief ) throws IOException
		{
			write( wtr, brief, defaultDateFormat );
		}

		final public void write( final Writer wtr, DateFormat df ) throws IOException
		{
			write( wtr, false, df );
		}
	}
	
	/**
	 * Dummy implementation of entry which doesn't do jack.
	 * Returned by Template.newEntry when the event is disabled
	 * or there is no interest in it.
	 */
	final public static class DummyEntry extends AbstractEntry
	{
		/**
		 * Constructs the DummyEntry.
		 */
		public DummyEntry()
		{
			super( null, 0 );
		}
		
		public boolean hasField( String field )
		{
			// ignore.
			return false;
		}
		
		public Object getField( String field )
		{
			// ignore.
			return null;
		}

		public Entry setField( String field, Object value )
		{
			// ignore.
			return this;
		}

		public Entry report()
		{
			// ignore.
			return this;
		}
		
		public long getEntryId()
		{
			// ignore.
			return 0;
		}

		public void formatFieldValues( FieldValueFormatter formatter )
		{
			// ignore.
		}

		public void write( Writer wtr, boolean brief, DateFormat df ) throws IOException
		{
			wtr.write( "DummyEntry(" );
			wtr.write( getTemplate().getName() );
			wtr.write( ")" );
		}
	}
	
	/**
	 * A normal implementation of entry which saves field values
	 * and reports to sinks. Returned by Template.newEntry when
	 * the event is enabled and there is interest in it.
	 */
	final public static class NormalEntry extends AbstractEntry
	{
		/**
		 * Constructs the PlainEntry.
		 *
		 * @param template the template which created this entry.
		 * @param fieldValues a sequence of field/value pairs which are
		 * parameters of the event.
		 */
		public NormalEntry( Template template, Object ... fieldValues )
		{
			super( template, System.currentTimeMillis() );
			fields = new Object[template.countFields()];
			setFields( fieldValues );
		}

		////////////
		// FIELDS //
		////////////
		
		public boolean hasField( String field )
		{
			return hasField( getTemplate().getFieldId( field, false ) );
		}
		
		public Object getField( String field )
		{
			return getField( getTemplate().getFieldId( field, false ) );
		}

		public Entry setField( String field, Object value )
		{
			return setField( getTemplate().getFieldId( field, true ), value );
		}
		
		/**
		 * @param fieldValues a sequence of field/value pairs which are
		 * parameters of the event.
		 * @return the entry.
		 */
		public Entry setFields( Object ... fieldValues )
		{
			if (fieldValues.length == 0)
				return this;
			
			if (fieldValues.length % 2 != 0)
				throw new IllegalArgumentException(
					"fieldValues must be sequence of field/value pairs" );
			
			Iterator<Object> i = new ArrayIterator<Object>( fieldValues );
			while (i.hasNext())
			{
				Object fieldObj = i.next();
				Object valueObj = i.next();
				
				if (!(fieldObj instanceof String))
					throw new IllegalArgumentException(
						"field not a string: "+fieldObj );
				
				setField( (String) fieldObj, valueObj );
			}
			
			return this;
		}

		////////////////
		// FIELD IMPL //
		////////////////
		
		private boolean hasField( int fid )
		{
			return getField( fid ) != null;
		}
		
		private Object getField( int fid )
		{
			return fid >= 0 && fid < fields.length ? fields[fid] : null;
		}
		
		private Entry setField( int fid, Object value )
		{
			if (fid >= fields.length)
			{
				Object[] newFields = new Object[getTemplate().countFields()];
				System.arraycopy( fields, 0, newFields, 0, fields.length );
				fields = newFields;
			}
			
			fields[fid] = value;
			
			return this;
		}
		
		private Object[] fields;

		////////////
		// REPORT //
		////////////

		public Entry report()
		{
			if (entryId != 0)
				throw new IllegalStateException( "this entry already reported" );
			
			entryId = entryIdGen.next();
			
			for (Sink sink : getTemplate().getSinks())
				sink.report( this );
			
			return this;
		}

		public void formatFieldValues( FieldValueFormatter formatter )
			throws IOException
		{
			int i = 0;
			for (String field : getTemplate().getFields())
				formatter.format( field, getField( i++ ) );
		}
		
		public long getEntryId()
		{
			return entryId;
		}
		
		private long entryId;

		////////////////
		// FORMATTING //
		////////////////
		
		public void write( final Writer wtr, boolean brief, DateFormat df ) throws IOException
		{
			if (!brief)
			{
				String ds;
				synchronized (df)
				{
					ds = df.format( getTimestamp() );
				}
				wtr.write( ds );
				wtr.write( " : " );
				wtr.write( Long.toString( getEntryId() ) );
				wtr.write( " : " );
				wtr.write( getTemplate().getCategory().toString() );
				wtr.write( " : " );
				
				// print out drop count
				if ( getTemplate().getLimit() > Template.UNLIMITED_ENTRIES &&  getTemplate().getDropCount() > 0 )
				{
					wtr.write( "(" );
					wtr.write( Integer.toString( getTemplate().getDropCount() ) );
					wtr.write( ")" );
					wtr.write( " : " );
					
					getTemplate().resetDropCount();
				}
			}
						
			wtr.write( getTemplate().getName() );
			
			formatFieldValues(
				new FieldValueFormatter()
				{
					private boolean first = true;
					public void format( String field, Object value )
						throws IOException
					{
						if (value == null)
							return;
						
						if (first)
						{
							wtr.write( " : " );
							first = false;
						}
						else
						{
							wtr.write( "; " );
						}
						
						wtr.write( field );
						wtr.write( '=' );
						if (value instanceof Throwable)
							wtr.write( getStackTrace( (Throwable) value ) );
						else
							wtr.write( value.toString() );
					}
				} );
			wtr.write( ';' );
		}
	}
	
	/**
	 * Array iterator may be used to create an iterator over
	 * an arbitrary array of a specified type.
	 * @param <T> the type of the elements of the array.
	 */
	final public static class ArrayIterator<T> implements Iterator<T>
	{
		/**
		 * Constructs the ArrayIterator.
		 *
		 * @param values the array of values to iterate over.
		 */
		public ArrayIterator( T[] values )
		{
			this.values = values;
		}
		
		private T[] values;
		
		private int index;

		public boolean hasNext()
		{
			return index < values.length;
		}

		public T next()
		{
			if (index >= values.length)
				throw new NoSuchElementException();
			
			return values[index++];
		}

		public void remove()
		{
			throw new UnsupportedOperationException();
		}
	}
	
	/**
	 * Interface for those interested in events.
	 */
	public interface Sink extends Startable
	{
		/**
		 * Reports the specified event to the sink.
		 * @param entry
		 */
		public void report( Entry entry );
		
		/**
		 * @return the name of this sink (if it has one).
		 */
		public String getName();

		/**
		 * @param template
		 * @return true if the sink has any interest in the template.
		 */
		public boolean hasInterest( Template template );
		
		/**
		 * @param tokenList
		 */
		public void setAcceptCat( String tokenList );
		
		/**
		 * @param tokenList
		 */
		public void setDenyCat( String tokenList );
		
		/**
		 * @param tokenList
		 */
		public void setAcceptName( String tokenList );
		
		/**
		 * @param tokenList
		 */
		public void setDenyName( String tokenList );
	}
	
	/**
	 * Interface for reporting field/value pairs in response to a
	 * call to Entry.formatFieldValues().
	 * 
	 * @see Entry#formatFieldValues(FieldValueFormatter)
	 */
	public interface FieldValueFormatter
	{
		/**
		 * @param field the field name.
		 * @param value the field value, may be null.
		 * @throws IOException
		 */
		public void format( String field, Object value ) throws IOException;
	}
	
	/**
	 * A combination interface of Sink and FieldValueFormatter.
	 */
	public interface SinkFieldValueFormatter extends Sink, FieldValueFormatter
	{
		// nothing else to add.
	}

	/**
	 * Removes and stops all the sinks.
	 */
	public static void shutdown()
	{
		for (Sink sink : Log.getSinks())
		{
			Log.removeSink( sink );
			try
			{
				sink.stop();
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
		}
	}

	/**
	 * @param name
	 * @param fieldValues
	 * @return an exception report using the log entry mechanism
	 */
	public static LogEntryException newException( String name, Object ... fieldValues )
	{
		return new LogEntryException( name, fieldValues );
	}
	
	/**
	 *
	 */
	public static class LogEntryException extends Exception implements Entry
	{
		/**
		 * @param name
		 * @param fieldValues
		 */
		public LogEntryException( String name, Object ... fieldValues )
		{
			Template t = Log.getTemplate( name, getAllowAutoRegister(), Category.Error, true );
			entry = new NormalEntry( t, fieldValues );
		}
		
		/**
		 * @return the entry as reported.
		 */
		public LogEntryException report()
		{
			if (okToLog && entry.getTemplate().isEnabled())
			{
				okToLog = false;
				entry.report();
			}
			return this;
		}

		/**
		 * @param okToLog
		 * @return this log entry exception.
		 */
		public LogEntryException setOkToLog( boolean okToLog )
		{
			this.okToLog = okToLog;
			return this;
		}
		
		private boolean okToLog = true;

		@Override
		public String getMessage()
		{
			return getMessage( false );
		}
		
		/**
		 * @param brief
		 * @return a text form of the message.
		 * @see #toString(boolean)
		 */
		public String getMessage( boolean brief )
		{
			return toString( brief );
		}
		
		private final Entry entry;
		
		private static final long serialVersionUID = 1L;

		///////////////////
		// ENTRY METHODS //
		///////////////////

		public Template getTemplate()
		{
			return entry.getTemplate();
		}

		public long getTimestamp()
		{
			return entry.getTimestamp();
		}

		public boolean hasField( String field )
		{
			return entry.hasField( field );
		}

		public Object getField( String field )
		{
			return entry.getField( field );
		}

		public LogEntryException setField( String field, Object value )
		{
			entry.setField( field, value );
			return this;
		}

		public void formatFieldValues( FieldValueFormatter formatter )
			throws IOException
		{
			entry.formatFieldValues( formatter );
		}

		@Override
		public String toString()
		{
			return toString( false );
		}
		
		/**
		 * @param brief
		 * @return a text form of the message.
		 */
		public String toString( boolean brief )
		{
			return entry.toString( brief );
		}
		
		public String toString( DateFormat df )
		{
			return entry.toString( df );
		}
		
		public String toString( boolean brief, DateFormat df )
		{
			return entry.toString( brief, df );
		}

		public void write( Writer wtr ) throws IOException
		{
			entry.write( wtr );
		}

		public void write( Writer wtr, boolean brief ) throws IOException
		{
			entry.write( wtr, brief );
		}

		public void write( Writer wtr, DateFormat df ) throws IOException
		{
			entry.write( wtr, df );
		}

		public void write( Writer wtr, boolean brief, DateFormat df ) throws IOException
		{
			entry.write( wtr, brief, df );
		}

		public long getEntryId()
		{
			return entry.getEntryId();
		}
	}
}
