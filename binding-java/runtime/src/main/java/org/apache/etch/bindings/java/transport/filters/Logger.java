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

package org.apache.etch.bindings.java.transport.filters;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;


/**
 * A Message Filter which logs the messages seen to a file.
 */
@SuppressWarnings("all")
public class Logger extends AbstractMessageFilter
{

	private BufferedWriter writer;

	private static String FILE_PATH = "Logger.filePath";

	private String fileName = null;
	
	private String testRenamedFileName = null;

	private DateFormat df =  new SimpleDateFormat( "HH:mm:ss.SSS" );

	private String newLine = System.getProperty( "line.separator" );
	
	private SimpleDateFormat fileNameFormat = new SimpleDateFormat(
	"yyyy.MM.dd.HH.mm.ss" );
	
	public static boolean testFlag = false;

	enum Direction
	{
		UP, DOWN
	}

	public Logger( TransportMessage transport, URL uri, Resources resources )
	throws Exception
	{
		super( transport, uri, resources );

		initializeLogger(uri);
	}
	
	private void initializeLogger(URL uri) throws Exception {
		
		fileName = uri.getTerm( FILE_PATH );
		if (fileName == null)
		{
			// use default name
			fileName = "Log.txt";
		}
		
		if (!testFlag)
		{
			File f = new File( fileName );
			if (f.exists())
			{
				/*
				 * String name = f.getName().substring( 0, f.getName().indexOf(
				 * "." ) ); String newFileName = f.getParent() +
				 * File.separatorChar + name + fileNameFormat.format( new Date() ) +
				 * ".txt";
				 */
				String newFileName = generateNewFileName( f.getName(), f
					.getParent() );
				boolean status = f.renameTo( new File( newFileName ) );
			}
			writer = new BufferedWriter( new FileWriter( f, true ) );
		}
		else {
			testRenamedFileName = generateNewFileName(fileName,null);
		}
	
	}
	
	private String generateNewFileName(String fileName, String pathName) throws
		Exception
	{
		if (pathName == null)
			pathName="";
		String name = fileName.substring( 0,
			fileName.indexOf( "." ) );
		String newFileName = pathName + File.separatorChar + name
		+ fileNameFormat.format( new Date() ) + ".txt";
		return newFileName;
	}

	@Override
	public String toString()
	{
		return "Logger/" + transport;
	}

	@Override
	public boolean sessionMessage( Who sender, Message msg ) throws Exception
	{
		// System.out.println("Direction: UP, Message: " + msg.toString());
		writeToFile( Direction.UP, sender, msg.toString() );
		return super.sessionMessage( sender, msg );
	}

	@Override
	public void transportMessage( Who recipient, Message msg ) throws Exception
	{
		// System.out.println("Direction: DOWN, Message: " + msg.toString());
		writeToFile( Direction.DOWN, recipient, msg.toString() );
		super.transportMessage( recipient, msg );
	}

	@Override
	protected boolean sessionUp() throws Exception
	{
		writeToFile( Direction.UP, null, transport.toString() );
		return true;
	}

	@Override
	protected boolean sessionDown() throws Exception
	{
		writeToFile( Direction.DOWN, null, transport.toString() );
		// writer.close();
		return true;
	}

	public void writeToFile( Direction direction, Who msgOrigin, String msg )
	throws Exception
	{
		if (writer == null)
			return;
		
		try
		{
			synchronized (writer)
			{
				writer.write( df.format( new Date() ) );
				writer.write( ": " );
				writer.write( convertDirToString( direction ) );
				writer.write( ": " );
				if (msgOrigin != null)
				{
					writer.write( msgOrigin.toString() );
					writer.write( ": " );
				}
				writer.write( msg );
				writer.write( newLine );
				writer.flush();
			}
		}
		catch ( Exception e )
		{
			writer.close();
			throw e;
		}
	}

	private String convertDirToString( Direction dir )
	{
		if (dir.ordinal() == Direction.DOWN.ordinal())
		{
			return "Outgoing";
		}
		return "Incoming";
	}

	public String getFileName()
	{
		return fileName;
	}

	public String getTestRenamedFileName()
	{
		return testRenamedFileName;
	}
}
