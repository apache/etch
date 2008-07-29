package etch.compiler.test;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

import org.junit.BeforeClass;
import org.junit.Test;

import etch.compiler.Backend;
import etch.compiler.CmdLineOptions;
import etch.compiler.CompilerLogHandler;
import etch.compiler.Etch2;
import etch.compiler.LogHandler;
import etch.compiler.Token;
/**
 * JUnit Test to test Compiler API
 * @author gsandhir
 *
 */
public class TestEtch
{
	
	private static CmdLineOptions cmdLineOption;
	private String correctEtchFormat = "module foo service bar {  void hello( int x, int y ) }";
	private String wrongEtchFormat1 = "module foo serviced bar {  void hello( int x, int y ) }";
	private String wrongEtchFormat2 = "module foo service bar   void hello( int x, int y ) }";
	private String wrongEtchFormat3 = "module foo service bar {  void hello( int x, in y ) }";
	
	/**
	 * This method sets up the CmdLineOptions
	 *
	 */
	
	@BeforeClass
	public static void setup() {	
		cmdLineOption = new CmdLineOptions();
		cmdLineOption.binding = Backend.BINDING_JAVA;
		HashSet<String> set = new HashSet<String>();
		set.add( Backend.WHAT_ALL );
		cmdLineOption.what = set;
		
	}
	/**
	 * This test case checks the successful generation of code from
	 * a given Etch String
	 *
	 */
	
	@Test
	public void generateFromString() {
		TestEtchLogHandler logHandler = new TestEtch.TestEtchLogHandler();
		String compileString = correctEtchFormat;
		logHandler.push( compileString, null );
		InputStream is1 = new ByteArrayInputStream(compileString.getBytes());
		Etch2.doCompile( cmdLineOption, is1, logHandler );	
		logHandler.pop();
		assertTrue(logHandler.errorList.isEmpty());
	} 
	
	/**
	 * This test case checks the successful generation of code from
	 * a given Etch String
	 *
	 */
	@Test
	public void generateErrorFromString1() {
		TestEtchLogHandler logHandler = new TestEtch.TestEtchLogHandler();
		String compileString = wrongEtchFormat1;
		logHandler.push( compileString, null );
		InputStream is1 = new ByteArrayInputStream(compileString.getBytes());
		Etch2.doCompile( cmdLineOption, is1, logHandler );
		logHandler.pop();
		List<String> errorList = logHandler.getErrorList();
		assertFalse(errorList.isEmpty());
		String errorMsg = (String)errorList.get( 0 );
		assertTrue(errorMsg.contains( "Encountered \"serviced\"" ));
	}
	/**
	 * This test case checks the successful generation of code from
	 * a given Etch String
	 *
	 */
	@Test
	public void generateErrorFromString2() {
		TestEtchLogHandler logHandler = new TestEtch.TestEtchLogHandler();
		String compileString = wrongEtchFormat2;
		logHandler.push( compileString, null );
		InputStream is1 = new ByteArrayInputStream(compileString.getBytes());
		Etch2.doCompile( cmdLineOption, is1, logHandler );
		logHandler.pop();
		List<String> errorList = logHandler.getErrorList();
		assertFalse(errorList.isEmpty());
		String errorMsg = (String)errorList.get( 0 );
		assertTrue(errorMsg.contains( "Encountered \"void\"" ));
	}
	/**
	 * This test case checks the successful generation of code from
	 * a given Etch String
	 *
	 */
	
	@Test
	public void generateErrorFromString3() {
		TestEtchLogHandler logHandler = new TestEtch.TestEtchLogHandler();
		String compileString = wrongEtchFormat3;
		logHandler.push( compileString, null );
		InputStream is1 = new ByteArrayInputStream(compileString.getBytes());
		Etch2.doCompile( cmdLineOption, is1, logHandler );
		logHandler.pop();
		List<String> errorList = logHandler.getErrorList();
		assertFalse(errorList.isEmpty());
		String errorMsg = (String)errorList.get( 0 );
		assertTrue(errorMsg.contains( "undefined type" ));
	}
	
	
	private static class TestEtchLogHandler extends CompilerLogHandler  {
		
		List<String> errorList = new ArrayList<String>();
		
		/**
		 * 
		 *
		 */		
		public TestEtchLogHandler() { }
		
		public void logMessage(int level, Token token, String msg) {
			if (level == LogHandler.LEVEL_ERROR) {
				String fmt = "Parsing " + printStack() + ": " + msg;				
				errorList.add( String.format( errHeader, fmt ) );
				//System.err.printf(errHeader,fmt);
			}
		}
		/**
		 * Returns the error List
		 * @return ArrayList
		 */
		
		public List<String> getErrorList() {
			return errorList;
		}
	}
}



