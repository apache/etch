/**
 *
 */
package etch.bindings.java.compiler.test;


import static org.junit.Assert.fail;
import etch.bindings.java.util.StrIntHashMap;
import org.junit.Before;
import org.junit.Test;


/**
 * @author rebarraz
 *
 */
public class TestMainServiceClient
{
	// Settings
	//--------------------------------------------------------------------------

	// Instance of the class.
	private MainTestClient myMainTestClient;

	// Instances of arguments types
	private Boolean			myBool;
	private Byte			myByte;
	private Short			myShort;
	private Integer 		myInt;
	private Long			myLong;
	private Float			myFloat;
	private Double  		myDouble;
	private Object			myObj;
	private String			myString;
	private StrIntHashMap	myMap;
	private etch.bindings.java.compiler.test.Test.E1 myE1;
	private etch.bindings.java.compiler.test.Test.S1 myS1;
	private etch.bindings.java.compiler.test.Test.S2 myS2;
	private etch.bindings.java.compiler.test.Test.S3 myS3;
	private etch.bindings.java.compiler.test.Test.S4 myS4;
	private boolean[]		myBools;
	private byte[]			myBytes;
	private short[]			myShorts;
	private int[]			myInts;
	private long[]			myLongs;
	private float[]			myFloats;
	private double[]		myDoubles;
	private Object[]		myObjs;
	private String[]		myStrings;
	private StrIntHashMap[]	myMaps;
	private etch.bindings.java.compiler.test.Test.E1[] myE1s;
	private etch.bindings.java.compiler.test.Test.S1[] myS1s;
	private etch.bindings.java.compiler.test.Test.S2[] myS2s;


	// Maintenance
	// -------------------------------------------------------------------------

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp()
		throws Exception
	{
		myMainTestClient = new MainTestClient();
	}


	// Tests
	//--------------------------------------------------------------------------

	/**
	 */
	@Test
	public void nothingMethod()
	{
		myMainTestClient.nothing();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void incrMethod()
	{
		myInt = myMainTestClient.incr(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void addMethod()
	{
		myInt = myMainTestClient.add(myInt, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void subMethod()
	{
		myInt = myMainTestClient.sub(myInt, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void sumMethod()
	{
		myInt = myMainTestClient.sum(myInts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void transMethod()
	{
		myInt = myMainTestClient.trans(myE1, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void distMethod()
	{
		myDouble = myMainTestClient.dist(myS1, myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void fillMethod()
	{
		myInts = myMainTestClient.fill(myInt, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void fillObjectMethod()
	{
		myObjs = myMainTestClient.fillObject(myInt, myObj);
	}

	/**
	 * @throws Exception
	 */
	@Test
	public void blowMethod()
		throws Exception
	{
		myMainTestClient.blow(myString, myInt);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void beetsMethod()
		throws Exception
	{
		myInt = myMainTestClient.beets(myE1);
	}

	/**
	 * @throws Exception
	 */
	@Test
	public void throwExcp5Method()
		throws Exception
	{
		myMainTestClient.throwExcp5(myString, myInt, myObj);
	}

	/**
	 * @throws Exception
	 */
	@Test
	public void throwExcp6Method()
		throws Exception
	{
		myMainTestClient.throwExcp6(myString, myInt, myObjs);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_booleanMethod()
	{
		myBool = myMainTestClient.p_boolean(myBool);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_boolean_arrayMethod()
	{
		myBools = myMainTestClient.p_boolean_array(myBools);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_byteMethod()
	{
		myByte = myMainTestClient.p_byte(myByte);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_byte_arrayMethod()
	{
		myBytes = myMainTestClient.p_byte_array(myBytes);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_shortMethod()
	{
		myShort = myMainTestClient.p_short(myShort);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_short_arrayMethod()
	{
		myShorts = myMainTestClient.p_short_array(myShorts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_intMethod()
	{
		myInt = myMainTestClient.p_int(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_int_arrayMethod()
	{
		myInts = myMainTestClient.p_int_array(myInts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_longMethod()
	{
		myLong = myMainTestClient.p_long(myLong);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_long_arrayMethod()
	{
		myLongs = myMainTestClient.p_long_array(myLongs);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_floatMethod()
	{
		myFloat = myMainTestClient.p_float(myFloat);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_float_arrayMethod()
	{
		myFloats = myMainTestClient.p_float_array(myFloats);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_doubleMethod()
	{
		myDouble = myMainTestClient.p_double(myDouble);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_double_arrayMethod()
	{
		myDoubles = myMainTestClient.p_double_array(myDoubles);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_stringMethod()
	{
		myString = myMainTestClient.p_string(myString);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_string_arrayMethod()
	{
		myStrings = myMainTestClient.p_string_array(myStrings);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_E1Method()
	{
		myE1 = myMainTestClient.p_E1(myE1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_E1_arrayMethod()
	{
		myE1s = myMainTestClient.p_E1_array(myE1s);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S1Method()
	{
		 myS1 = myMainTestClient.p_S1(myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S1_arrayMethod()
	{
		 myS1s = myMainTestClient.p_S1_array(myS1s);

	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S2Method()
	{
		myS2 = myMainTestClient.p_S2(myS2);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S2_arrayMethod()
	{
		myS2s = myMainTestClient.p_S2_array(myS2s);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_BlobMethod()
	{
		myMap = myMainTestClient.p_Blob(myMap);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_Blob_ArrayMethod()
	{
		myMaps = myMainTestClient.p_Blob_array(myMaps);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_objectMethod()
	{
		myObj = myMainTestClient.p_object(myObj);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_arrayMethod()
	{
		myMainTestClient.p_object_array(null);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_structMethod()
	{
		myS3 = myMainTestClient.p_object_struct(myS3);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_struct_arrayMethod()
	{
		myS4 = myMainTestClient.p_object_struct_array(myS4);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void alwaysWordsMethod()
	{
		myInt = myMainTestClient.alwaysWorks();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void isTrueMethod()
	{
		myBool = myMainTestClient.isTrue();
	}

	/**
	 */
	@Test
	public void alwaysFailsMethod()
	{
		myMainTestClient.alwaysFails();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void isFalseMethod()
	{
		myBool = myMainTestClient.isFalse();
	}

	/**
	 */
	@Test
	public void m1Method()
	{
		myMainTestClient.m1();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m1Method()
	{
		myBool = myMainTestClient.can_m1();
	}

	/**
	 */
	@Test
	public void m2Method()
	{
		myMainTestClient.m2();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m2Method()
	{
		myBool = myMainTestClient.can_m2(null);
	}

	/**
	 */
	@Test
	public void m3Method()
	{
		myMainTestClient.m3();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m3Method()
	{
		myBool = myMainTestClient.can_m3(null);
	}

	/**
	 */
	@Test
	public void m4Method()
	{
		myMainTestClient.m4();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m4Method()
	{
		myBool = myMainTestClient.can_m4(myBool);
	}

	/**
	 */
	@Test
	public void m5Method()
	{
		myMainTestClient.m5();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m5Method()
	{
		myBool = myMainTestClient.can_m5(myByte);
	}

	/**
	 */
	@Test
	public void m7Method()
	{
		myMainTestClient.m7();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m7Method()
	{
		myBool = myMainTestClient.can_m7(myInt);
	}

	/**
	 */
	@Test
	public void m8Method()
	{
		myMainTestClient.m8();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m8Method()
	{
		myBool = myMainTestClient.can_m8(myLong);
	}

	/**
	 */
	@Test
	public void m9Method()
	{
		myMainTestClient.m9();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m9Method()
	{
		myBool = myMainTestClient.can_m9(myFloat);
	}

	/**
	 */
	@Test
	public void m10Method()
	{
		myMainTestClient.m10();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m10Method()
	{
		myBool = myMainTestClient.can_m10(myDouble);
	}

	/**
	 */
	@Test
	public void m11Method()
	{
		myMainTestClient.m11();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m11Method()
	{
		myBool = myMainTestClient.can_m11(myString);
	}

	/**
	 */
	@Test
	public void m12Method()
	{
		myMainTestClient.m12();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m12Method()
	{
		myBool = myMainTestClient.can_m12(myInt);
	}

	/**
	 */
	@Test
	public void m13Method()
	{
		myMainTestClient.m13(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m13Method()
	{
		myBool = myMainTestClient.can_m13(myInt);
	}

	/**
	 */
	@Test
	public void m14Method()
	{
		myMainTestClient.m14(myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m14Method()
	{
		myBool = myMainTestClient.can_m14(myInt);
	}

	/**
	 */
	@Test
	public void m15Method()
	{
		myMainTestClient.m15();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m15Method()
	{
		myBool = myMainTestClient.can_m15(myE1);
	}

	/**
	 *
	 *
	 */
	@Test
	public void mainMethod()
	{
		ClientThread m = new ClientThread();
		m.run();

		try
		{
			m.join(1000);
		}
		catch (Exception e)
		{
			fail();
		}
	}

	// Helpers
	//--------------------------------------------------------------------------

	/**
	 * Simple thread class for calling main and having all of Thread's methods.
	 */
	private static class ClientThread extends Thread
	{
		/**
		 * Go at it.
		 */
		public void run()
		{
			try
			{
				MainTestClient.main(null);
			}
			catch (Exception e)
			{
			}
		}
	}
}
