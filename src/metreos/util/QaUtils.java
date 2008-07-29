/* QaUtils.java
 *
 * Created by wayetman on Oct 11, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */
package metreos.util;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Properties;

/**
 * @author wayetman
 * This class contains methods to be accessed for QA Junit tests.
 */
public class QaUtils
{	
	//----------- ReportUtils -------------------
	
	public static enum State {INFO, WARNING, ERROR, VERBOSE};
	
	/**
	 * This method will return a State map representing a report log
	 * level.
	 * @param level - property file value for the key loglevel
	 * @return - returns a state, defaults to INFO.
	 */
	public static State convertState(String level)
	{
	    QaUtils.State logLevel = null;
		if (level.equals("INFO"))
	    {
	    	logLevel=QaUtils.State.INFO; 
	    } else if (level.equals("WARNING")) 
	    {
	    	logLevel=QaUtils.State.WARNING; 
	    } else if (level.equals("ERROR"))
	    {
	    	logLevel=QaUtils.State.ERROR; 
	    } else if (level.equals("VERBOSE"))
	    {
	    	logLevel=QaUtils.State.VERBOSE; 
	    }else {
	    	logLevel=QaUtils.State.INFO;
	    }	    		    		  
		return logLevel;
	}
		
	/**
	 * This method appends string to a file.
	 * @param logLevel - logging leve identifier
	 * @param fileSpec - specification for the file to append to
	 * @param msg - string to append to the file.
	 */
	public static void log (State logLevel, String fileSpec, String msg)
	{
		try
		{
			appendToFile (fileSpec,"\r\n"+logLevel + ": " +msg);
		}
		catch ( IOException e )
		{
			System.out.println("I/O Exception.");
			e.printStackTrace();
		}
	}
	
	/**
	 * This method will return a string formated to represent a date and time stamp
	 * to be as a prefix to a file name.
	 * @return dateStamp
	 */
	public static String dateStamp() 
	{
		final Calendar cal = new GregorianCalendar();

		//	 Get the components of the time
		int hour12 = cal.get(Calendar.HOUR);            // 0..11
		int hour24 = cal.get(Calendar.HOUR_OF_DAY);     // 0..23
		int min = cal.get(Calendar.MINUTE);             // 0..59
		int sec = cal.get(Calendar.SECOND);             // 0..59
		int intMonth = cal.get( Calendar.MONTH) + 1;
		int intDay = cal.get( Calendar.DAY_OF_MONTH);               	                         
		String strYear = Integer.toString(cal.get( Calendar.YEAR));
		String strMonth = null;
		String strDay = null;

		if ( (intMonth > 0) && (intMonth < 10) ) 
		{  
			strMonth = "0" + intMonth;
		} 
		else 
		{
			strMonth=Integer.toString(intMonth);
		}

		if (intDay < 10) 
		{
			strDay = "0" + intDay;
		} else 
		{
			strDay=Integer.toString(intDay);
		}

		String dateStamp=strYear+strMonth+strDay+Integer.toString(hour24)+Integer.toString(min)+Integer.toString(sec);
		return dateStamp;
    }
	
	//----------------------  FileUtils ---------------
	
    /**
     * This method will open a properties file and return a properties object.
     * @param fileName - file specification for the file to be opened.
     * @return tempProp - a properties object.
     */
    public static Properties loadProperties(String fileName) 
    {
        InputStream propsFile;
        Properties tempProp = new Properties();

        try {
            propsFile = new FileInputStream(fileName);
            tempProp.load(propsFile);
            propsFile.close();
        } catch (IOException ioe) {
            System.out.println("I/O Exception.");
            ioe.printStackTrace();
            System.exit(0);
        }
        return tempProp;
    }
    
    /**
     * This method will search for a file spec and return a boolean
     * indicating if the file was found or not.
     * @param filename
     * @return exists
     */
    public static boolean fileExists(String filename)
    {
    	boolean exists = (new File(filename)).exists();
        return exists;
    }
    
    /**
     * This method create a directory; all non-existent ancestor directories are 
     * automatically created
     * @param directoryName
     * @return success
     */
    public static boolean dirCreated(String directoryName)
    {
      boolean success = (new File(directoryName)).mkdirs();
      if (!success) 
      {
        // Directory creation failed
    	  System.out.println("Error: Directory creation failed.");  
      }
      return success;
    }
    
    
    /**
     * This method writes a sting buffer to a file. 
     * @param fileSpec  - Output File spec
     * @param contents - String buffer
     * @throws IOException
     */
    public static void writeToFile(String fileSpec, String contents ) throws IOException 
    {   
      FileOutputStream ostr = new FileOutputStream(fileSpec); 
      // or, ostr = new FileOutputStream(filename);
      
      OutputStreamWriter owtr = new OutputStreamWriter( ostr ); // promote
      
      owtr.write( contents, 0, contents.length() );
      owtr.close();                          
    }	// end of WriteToFile  
    
    /**
     * This method appends a string to a File.
     * @param fileSpec - Output File spec
     * @param contents - String buffer
     * @throws IOException
     */
    public static void appendToFile(String fileSpec, String contents ) throws IOException 
    {  
      BufferedWriter out = new BufferedWriter(new FileWriter(fileSpec, true));
      out.write(contents);
      out.close();  
    }

    public static void appendToFile(File inFile, String contents ) throws IOException 
    {  
      //System.out.println(inFile.getPath() + " " +  inFile.exists());
      BufferedWriter out = new BufferedWriter(new FileWriter(inFile, true));
      out.write(contents);
      out.close();  
    }   
    
    //------------ string utils --------------
    
    /**
     * This method is used to perform a string replace.
     * @param str Input string
     * @param pattern Pattern to find
     * @param replace Replacement string
     * @return Input string after string replace.
     */
    public static String replace(String str, String pattern, String replace) {
      int s = 0;
      int e = 0;
      StringBuffer result = new StringBuffer();

      while ((e = str.indexOf(pattern, s)) >= 0) 
      {
          result.append(str.substring(s, e));
          result.append(replace);
          s = e+pattern.length();
      }
      result.append(str.substring(s));
      return result.toString();
    }    
    
}
