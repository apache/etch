package metreos.core.io.test;

import static org.junit.Assert.*;

import java.lang.reflect.Array;
import java.net.ConnectException;
import java.net.Socket;
import java.net.SocketException;
import java.util.Collection;
import java.util.Set;

import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import metreos.core.Who;
import metreos.core.io.DataSource;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.core.io.Packetizer;
import metreos.core.io.TcpConnection;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_boolean;
import etch.bindings.java.msg.Validator_byte;
import etch.bindings.java.msg.Validator_double;
import etch.bindings.java.msg.Validator_float;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.msg.Validator_object;
import etch.bindings.java.msg.Validator_short;
import etch.bindings.java.msg.Validator_string;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.support.FieldMap;
import etch.bindings.java.support.Mailbox;
import etch.bindings.java.support.TypeMap;
import etch.bindings.java.transport.MailboxManager;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.MessageHandler;
import etch.bindings.java.transport.Messagizer;
import java.util.Map;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;

/**
 * @author manogane
 *
 * This file performs interop tests between data types 
 * in C# and Java ( including multidimensional arrays ) 
 *
 * Instructions: 
 * --------------
 * 
 * For Java -> C# test, first start the C# listener by 
 * running the main method of the test file with the same name. 
 * Thereafter, run unit tests on the Java side. 
 * 
 * For the C# -> Java test, first start the Java listener and 
 * then run the tests on the C# side.
 *
 */
public class InteropTest
{

	private MailboxManager mmgr;
    private MyMessageHandler mh;


    private MyValueFactory vf = new MyValueFactory();

    public static int testCount = 0;
    public static boolean DEBUG = false;
    
    private MyListenerHandler lh;
    private Listener l;

    private String host = "127.0.0.1";
    private int port = 4001;
    private int listenerPort = 4001;
    private int delay = 0;

    private boolean CSHARPTOJAVA = false;

    private static TcpConnection c;

    //private Converter converter = null;

    private static int MAXFIELDS = 100;
    private static Object[] verifier = new Object[MAXFIELDS];

    public void startListener() throws Exception
    {
        lh = new MyListenerHandler( );
        l =  new Listener( lh, 5, host, listenerPort, 0 );
        l.start();
        l.waitUp( 4000 );
    }
	
	@Before
	public void Before() throws Exception
	{
		try
		{
			setUp( null );
		}
		catch ( Exception e )
		{
    		String errorMessage = "The C# side listener is not UP ";
            assertFalse( errorMessage, true );
		}
	}
	
	/**
	 * @param s 
	 * @throws java.lang.Exception
	 */
	//@Before
	public  void setUp( Socket s ) throws Exception
	{   
		mh = new MyMessageHandler( this );
        mmgr = new MailboxManager( mh, 0 );

        Messagizer m = new Messagizer( mmgr, vf );
        mmgr.setSource( m );

        Packetizer p = new Packetizer( m );
        m.setSource( p );

        if ( !CSHARPTOJAVA )
            c = new TcpConnection( p, host, port, delay );
        else
            c = new TcpConnection( p, s );
        
        p.setSource( c );

        c.start();
        c.waitUp( 4000 );
        
	}
	
    @org.junit.Test
    public void TestMessageBool() throws Exception
    {
    	
        Message message = new Message( MyValueFactory.mtcmd[0], vf );
        boolean no1 = true;
        boolean no2 = false;
        MyValueFactory.mtcmd[ 0 ].putValidator( MyValueFactory.mk[0], Validator_boolean.get(  0  ) );
        MyValueFactory.mtcmd[ 0 ].putValidator( MyValueFactory.mk[1], Validator_boolean.get(  0  ) );
        message.put( MyValueFactory.mk[ 0 ], no1 );
        message.put( MyValueFactory.mk[ 1 ], no2 );

        //System.out.println( mtcmd[0].validators.size() );
        // Set the object to be verified against.
        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;
        
        @SuppressWarnings("unused")
		Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );
        
        Thread.sleep( 500 );

    }

    @Test
    public void TestMessageBool3D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[1], vf );

        boolean[][][] no = new boolean[][][] {
                { { true, false, false }, { true, false, true } },
                { { false, true, false }, { false, true, false } },
                { { false, false, false }, { true, true, true } }
            };

        // Set the object to be verified against.
        verifier[ 0 ] = no;

        
        message.put( MyValueFactory.mk[2], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }


    @Test
    public void TestMessageBool2D() throws Exception
    {
    	
        Message message = new Message( MyValueFactory.mtcmd[2], vf );
        
        boolean[][] no = new boolean[][] { { false, true }, { true, false } };

        // Set the object to be verified against.
        verifier[ 0 ] = no;

        
        message.put( MyValueFactory.mk[3], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }
    
    @Test
    public void TestMessageString() throws Exception
    {
    	

        Message message = new Message( MyValueFactory.mtcmd[3], vf );
        String no1 = "hello java";
        String no2 = "hello world";
        message.put( MyValueFactory.mk[4], no1 );
        message.put( MyValueFactory.mk[5], no2 );

        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;
        
        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );

    }

    @Test
    public void TestMessageStringArray2D() throws Exception
    {
        

        Message message = new Message( MyValueFactory.mtcmd[4], vf );

        String[][] no = new String[][] { { "", "a", "ab", "abc" }, { "23" } };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[6], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageStringArray3D() throws Exception
    {
    	    	
        Message message = new Message( MyValueFactory.mtcmd[5], vf );

        String[][][] no = 
            new String[][][]
            {
                { { "", "a", "ab", "abc" }, { "23" } },
                { { "abc", "ab", "a", "" }, { "23" } },
                { { "ab", "abc", "", "a" }, { "23" } }
            };

        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[7], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();

    }

    @Test
    public void TestMessageByte() throws Exception
    {
        
        Message message = new Message( MyValueFactory.mtcmd[6], vf );
        byte no1 = Byte.MAX_VALUE;
        byte no2 = Byte.MIN_VALUE;
        message.put( MyValueFactory.mk[8], no1 );
        message.put( MyValueFactory.mk[9], no2 );

        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;
        
        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );

    }

    @Test
    public void TestMessageByteArray2D() throws Exception
    {
    	
        Message message = new Message( MyValueFactory.mtcmd[7], vf );

        byte[][] no = 
            new byte[][] { { Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE },  { 23 } };

        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[10], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();

    }

    @Test
    public void TestMessageByteArray3D() throws Exception
    {
    	
        Message message = new Message( MyValueFactory.mtcmd[8], vf );

        byte[][][] no = 
            new byte[][][] 
            {
                { { Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE }, { 23 } },
                { { Byte.MIN_VALUE, 1, 0, -1, Byte.MAX_VALUE }, { 23 } },
                { { Byte.MAX_VALUE, -1, 0, 1, Byte.MIN_VALUE }, { 23 } }
            };

        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[11], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();

    }

    @Test
    public void TestMessageShort() throws Exception
    {
        
        Message message = new Message( MyValueFactory.mtcmd[9], vf );
        short no1 = Short.MAX_VALUE;
        short no2 = Short.MIN_VALUE;
        message.put( MyValueFactory.mk[12], no1 );
        message.put( MyValueFactory.mk[13], no2 );

        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;
        
        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );

    }

    @Test
    public void TestMessageShortArray2D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[10], vf );
        
        short[][] no = new short[][] { { Short.MIN_VALUE, Byte.MIN_VALUE, -1, 0, 1,
		    Byte.MAX_VALUE, Short.MAX_VALUE }, { 23 } };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[14], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
    }

    @Test
    public void TestMessageShortArray3D() throws Exception
    {
        Message message = new Message( MyValueFactory.mtcmd[11], vf );

        short[][][] no = new short[][][] 
                        {
                            { { Short.MIN_VALUE, Byte.MIN_VALUE, -1, 0, 1,
		                    Byte.MAX_VALUE, Short.MAX_VALUE }, { 23 } },
                            { { Short.MAX_VALUE, Byte.MIN_VALUE, -1, 0, 1,
		                    Byte.MAX_VALUE, Short.MIN_VALUE}, { 23 } },
                            { { Short.MAX_VALUE, Byte.MAX_VALUE, 1, 0, -1,
		                    Byte.MIN_VALUE, Short.MIN_VALUE }, { 23 } }
                        };


        // Set array to be verified against
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[15], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );
    }

    @Test
    public void TestMessageLong() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[12], vf );
        long no1 = Long.MAX_VALUE;
        long no2 = Long.MIN_VALUE;
        message.put( MyValueFactory.mk[16], no1 );
        message.put( MyValueFactory.mk[17], no2 );

        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;
        
        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );

    }

    @Test
    public void TestMessageLongArray2D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[13], vf );

        long[][] no = new long[][] { { Long.MIN_VALUE, Integer.MIN_VALUE, Short.MIN_VALUE,
		    Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE,
		    Integer.MAX_VALUE, Long.MAX_VALUE}, { 23 } };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[18], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageLongArray3D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[14], vf );

        long[][][] no = new long[][][]
                        {
                            { { Long.MIN_VALUE, Integer.MIN_VALUE, Short.MIN_VALUE,
		                    Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE,
		                    Integer.MAX_VALUE, Long.MAX_VALUE}, { 23 } },
                            { { Long.MIN_VALUE, Integer.MAX_VALUE, Short.MAX_VALUE,
		                    Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE, Short.MIN_VALUE,
		                    Integer.MIN_VALUE, Long.MAX_VALUE}, { 23 } },
                            { { Long.MAX_VALUE, Integer.MAX_VALUE, Short.MAX_VALUE,
		                    Byte.MAX_VALUE, -1, 0, 1, Byte.MIN_VALUE, Short.MIN_VALUE,
		                    Integer.MIN_VALUE, Long.MIN_VALUE}, { 23 } }
                        };


        // Set array to be verified against
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[19], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );
    }

    

    @Test
    public void TestMessageDouble() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[15], vf );
        double no1 = 2.4;
        double no2 = -2.4;
        message.put( MyValueFactory.mk[20], no1 );
        message.put( MyValueFactory.mk[21], no2 );

        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageDoubleArray2D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[16], vf );
        
        double[][] no = new double[][] { { -1, 0, 1, Double.MIN_VALUE, Double.MIN_NORMAL,
		    Double.MAX_VALUE, Double.NaN, Double.NEGATIVE_INFINITY,
		    Double.POSITIVE_INFINITY, -0.0f, 1.1f,
		    3.141592653589793238462643383279 }, { 23 } };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[22], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageDoubleArray3D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[17], vf );
        
        double[][][] no = new double[][][]
            {
                new double[][] { { -1, 0, 1, Double.MIN_VALUE, Double.MIN_NORMAL,
		        Double.MAX_VALUE, Double.NaN, Double.NEGATIVE_INFINITY,
		        Double.POSITIVE_INFINITY, -0.0f, 1.1f,
		        3.141592653589793238462643383279 }, { 23 } },

                { { -1, 0, 1, Double.MAX_VALUE, Double.MIN_NORMAL,
		        Double.MIN_VALUE, Double.NaN, Double.NEGATIVE_INFINITY,
		        Double.POSITIVE_INFINITY, 0.0f, -1.1f,
		        3.141592653589793238462643383279 }, { 23 } },

                { { -1, 0, 1, Double.MAX_VALUE, Double.MIN_NORMAL,
		        Double.MIN_VALUE, Double.NaN, Double.POSITIVE_INFINITY,
		        Double.NEGATIVE_INFINITY, 0.0f, -1.1f,
		        -3.141592653589793238462643383279 }, { 23 } }
            };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[23], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageFloat() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[18], vf );
        float no1 = 1.2f;
        float no2 = -1.2f;
        message.put( MyValueFactory.mk[24], no1 );
        message.put( MyValueFactory.mk[25], no2 );

        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 1000 );
        //c.Close();
    }

    @Test
    public void TestMessageFloatArray2D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[19], vf );

        float[][] no = new float[][] { { -1, 0, 1, Float.MIN_VALUE, Float.MIN_NORMAL,
		    Float.MAX_VALUE, Float.NaN, Float.NEGATIVE_INFINITY,
		    Float.POSITIVE_INFINITY, -0.0f, 1.1f,
		    3.141592653589793238462643383279f },  { 23 } };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[26], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageFloatArray3D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[20], vf );
        //int[] no = new int[] { int.MinValue, short.MinValue, SByte.MinValue,
        //    -1, 0, 1, sbyte.MaxValue, short.MaxValue, int.MaxValue};

        float[][][] no = new float[][][]
            {
                { { -1, 0, 1, Float.MIN_VALUE, Float.MIN_VALUE,
		        Float.MAX_VALUE, Float.NaN, Float.NEGATIVE_INFINITY,
		        Float.POSITIVE_INFINITY, -0.0f, 1.1f,
		        3.141592653589793238462643383279f }, { 23 } },

                {{ -1, 0, 1, Float.MAX_VALUE, Float.MIN_VALUE,
		        Float.MIN_VALUE, Float.NaN, Float.NEGATIVE_INFINITY,
		        Float.POSITIVE_INFINITY, 0.0f, -1.1f,
		        3.141592653589793238462643383279f }, { 23 } },

                { { -1, 0, 1, Float.MAX_VALUE, Float.MIN_NORMAL,
		        Float.MAX_VALUE, Float.NaN, Float.POSITIVE_INFINITY,
		        Float.NEGATIVE_INFINITY, -0.0f, 1.1f,
		        -3.141592653589793238462643383279f }, { 23 } }
            };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[27], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageInt() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[21], vf );
        int no1 = 12345;
        int no2 = -no1;
        message.put( MyValueFactory.mk[28], no1 );
        message.put( MyValueFactory.mk[29], no2 );

        verifier[ 0 ] = no1;
        verifier[ 1 ] = no2;

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageIntArray2D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[22], vf );
        
        int[][] no = new int[][] { { Integer.MIN_VALUE, Short.MIN_VALUE, Byte.MIN_VALUE,
            -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE, Integer.MAX_VALUE}, { 23 } };


        // Set the object to be verified against.
        verifier[ 0 ] = no;

        message.put( MyValueFactory.mk[30], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );


        // Sleep required so that next "test" method doesn't interfere
        Thread.sleep( 500 );
        //c.Close();
    }

    @Test
    public void TestMessageIntArray3D() throws Exception
    {
    	Message message = new Message( MyValueFactory.mtcmd[23], vf );
        
        int[][][] no = new int[][][]
                        {
                            { { Integer.MIN_VALUE, Short.MIN_VALUE, Byte.MIN_VALUE,
	                        -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE, Integer.MAX_VALUE}, { 23 } },
                            { { Integer.MAX_VALUE, Short.MIN_VALUE, Byte.MIN_VALUE,
	                        -1, 0, 1, Byte.MAX_VALUE, Short.MIN_VALUE, Integer.MAX_VALUE}, { 23 } },
                            { { Integer.MAX_VALUE, Short.MAX_VALUE, Byte.MAX_VALUE,
	                        -1, 0, 1, Byte.MIN_VALUE, Short.MIN_VALUE, Integer.MIN_VALUE}, { 23 } }
                        };


        // Set array to be verified against
        verifier[0] = no;

        message.put( MyValueFactory.mk[31], no );

        Mailbox mb = 
            ( Mailbox ) mmgr.message( null, message, 1, 0 );

        Thread.sleep( 500 );
        //c.Close();
    }
    
    
    /**
	 * @throws java.lang.Exception
	 */
	@AfterClass
	public static void tearDownAfterClass() throws Exception
	{
		c.close();
        c.waitDown( 4000 );
	}
	
	
    /**
     * @author manogane
     *
     */
    public enum What
    {
        MMGRMESSAGEHANDLERSTARTED,
        MMGRMESSAGEHANDLERUP,
        MMGRMESSAGEHANDLERDown,
        MMGRMESSAGEHANDLERSTOPPED,
        MYMESSAGIZERClose,
        MYMESSAGIZERLocalAddress,
        MYMESSAGIZERRemoteAddress,
        MYMESSAGIZERShutdownInput,
        MYMESSAGIZERShutdownOutput,
        MYMESSAGIZERSTOP,
        MYMESSAGIZERMESSAGE,
        MMGRMESSAGEHANDLERMESSAGE
    };

    /// <summary>
    /// Dummy message handler
    /// </summary>
    /**
     * @author manogane
     *
     */
    public static class MyMessageHandler implements MessageHandler<MailboxMessageSource>
    {
        /**
         * 
         */
        public Enum what;

        InteropTest _tds;
        
        /**
         * @param tds
         */
        public MyMessageHandler( InteropTest tds )
        {
            _tds = tds;
        }

        /**
         * @param src
         */
        public void started( MailboxMessageSource src )
        {
            what = What.MMGRMESSAGEHANDLERSTARTED;
        }

        public void up( MailboxMessageSource src )
        {
            what = What.MMGRMESSAGEHANDLERUP;
        }

        /*public boolean message( MailboxMessageSource src, Who sender, Message msg ) throws Exception
        {
            if ( DEBUG ) 
            	System.out.println( "Message received" + msg.toString() );
        	
            _tds.Verify( msg );
            return true;
        }*/

        public void down( MailboxMessageSource src )
        {
            what = What.MMGRMESSAGEHANDLERDown;
        }

        /**
         * @param src
         */
        public void stopped( MailboxMessageSource src )
        {
            what = What.MMGRMESSAGEHANDLERSTOPPED;
        }

        /**
         * @param src
         * @param what
         * @param e
         */
        public void exception( MailboxMessageSource src, String what, Exception e )
        {
            System.out.println( "myMessageHandler.exception() called ... " );
        }

		public MailboxMessageSource getSource()
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void setSource( MailboxMessageSource src )
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionControl( Object control, Object value ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionNotify( Object event ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			// TODO Auto-generated method stub
			return null;
		}

		public boolean message( Who sender, Message msg ) throws Exception
		{
			if ( DEBUG ) 
            	System.out.println( "Message received" + msg.toString() );
        	
            _tds.Verify( msg );
            return true;
		}
    }

    
    /**
     * @author manogane
     *
     */
    public class MyListenerHandler implements ListenerHandler
    {

        public MyListenerHandler( )
        {
            // do nothing
        }

        public void down( Listener src )
        {
            System.out.println("Connection down ... ");
        }

        public void up( Listener src )
        {
            System.out.println("Connection up ... ");
        }

		public void accepted( Socket s ) throws Exception
		{
			System.out.println("Connection accepted ....");
            
            // only if sending data from csharp to java, 
            // receive...
            if ( CSHARPTOJAVA )
            {
                setUp( s );
            }	
		}

		public Listener getSource()
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void setSource( Listener src )
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionControl( Object control, Object value ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionNotify( Object event ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			// TODO Auto-generated method stub
			return null;
		}

    }

    /// <summary>
    /// Run the main function if transferring data from Java to C#.
    /// </summary>
    /// <param name="args"></param>
    /**
     * @param args
     */
    public static void main( String[] args ) throws Exception
    {
        // Just start the listener
        InteropTest inst = new InteropTest();
        inst.CSHARPTOJAVA = true;
        inst.startListener();
    	//setUpBeforeClass();
    	//inst.TestMessageBool();
        
    }

    private static void assertEquals( Object a, Object b )
    {
        
    	if (a == null || b == null)
		{
			Assert.assertEquals( a, b );
			return;
		}
		
		Class<?> aClass = a.getClass();
		Class<?> bClass = b.getClass();
		
		if (aClass == bClass)
		{
			if (aClass.isArray() && bClass.isArray())
				assertArrayEquals( a, b );
			else
				Assert.assertEquals( a, b );
		}
		//else if (a instanceof Number && b instanceof Number)
		else
		{
//			System.out.printf( "a %s, b %s\n", aClass, bClass );
			Assert.assertEquals( a, b );
		}
    }

    private static void assertArrayEquals( Object a, Object b )
    {
    	int an = Array.getLength( a );
		int bn = Array.getLength( b );
		
		assertEquals( an, bn );
		
		for (int i = 0; i < an; i++)
			assertEquals( Array.get( a, i ), Array.get( b, i ) );

        
    }

    
    /**
     * @param msg
     */
    public synchronized void Verify( Message msg ) throws Exception
    {
        int i = 0;
        boolean nullMessage = true;
      
        Set<Field> keySet = msg.keySet();
        
        for ( Field f : keySet ) 
        {
        	if ( DEBUG ) 
            {
        		System.out.println( "---------------------------------------------------" );
	            System.out.println( "Field = " + f );
	            System.out.print( "\t Value = " + msg.get( f ) );
	            System.out.println( "\t Type = " + ( msg.get( f ).getClass() ) );
	            //System.out.println( "Validator.size() = " + msg.type().validators.size());
            }
        	
            if ( !f.equals( new Field( "_messageId" ) ) )
            {
                //AssertEquals(no, msg[f]);

                if ( ! CSHARPTOJAVA) 
                    assertEquals( verifier[i], msg.get(  f ) );
                
                nullMessage = false;
                i++;
            }

            if ( nullMessage == true )
                System.out.println( "Null Message !" );
        }

        if ( !CSHARPTOJAVA )
        {
            if ( DEBUG )
            {
            	System.out.println();
	            System.out.println( "----------------------" );
	            System.out.println( "Verified test ... " );
	            System.out.println( "----------------------" );
        	}
        	testCount++;
        	// System.out.println( "Test " + testCount + " passed");
        	if ( testCount == 24 )
        		System.out.println( "All interop tests pass");
        }
        else
        {
            @SuppressWarnings("unused")
			Mailbox mb = ( Mailbox ) mmgr.message( null, msg, 1, 0 );
        }
            
    }
    
    public static class MyValueFactory extends DefaultValueFactory
    {
    	private final static TypeMap types = new TypeMap();
    	private final static FieldMap fields = new FieldMap();
    	
    	// initializers

        /*
         * Set of types
         */
        private final static int NUMCMDS = 30;
        
        private final static Type[] mtcmd = new Type[ NUMCMDS ];
        private final static int[] cmd = new int[ NUMCMDS ];
        private final static Field[] mk = new Field[ NUMCMDS * 2 ];
        private final static int k1 = 1, k2 = 2;
        
        static
        {
        	DefaultValueFactory.init( types, fields );
        	
        	// Boolean
    		mtcmd[ 0 ] = types.get( "cmd0" );
            mk[ 0 ] = fields.get( "k1" );
            mk[ 1 ] = fields.get( "k2" );
            
            mtcmd[ 0 ].putValidator( mk[ 0 ], Validator_boolean.get( 0 ) );
            mtcmd[ 0 ].putValidator( mk[ 1 ], Validator_boolean.get( 0 ) );
            mtcmd[ 0 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // Boolean 3D
            mtcmd[ 1 ] = types.get( "cmd1" );
            mk[ 2 ] = fields.get( "k1" );
            
            mtcmd[ 1 ].putValidator( mk[ 2 ], Validator_boolean.get( 3 ) );
            mtcmd[ 1 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // Boolean 2D
            mtcmd[ 2 ] = types.get( "cmd2" );
            mk[ 3 ] = fields.get( "k1" );
            
            mtcmd[ 2 ].putValidator( mk[ 3 ], Validator_boolean.get( 2 ) );
            mtcmd[ 2 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // String
            mtcmd[ 3 ] = types.get( "cmd3" );
            mk[ 4 ] = fields.get( "k1" );
            mk[ 5 ] = fields.get( "k2" );

            mtcmd[ 3 ].putValidator( mk[ 4 ], Validator_string.get( 0 ) );
            mtcmd[ 3 ].putValidator( mk[ 5 ], Validator_string.get( 0 ) );
            mtcmd[ 3 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // String 2D
            mtcmd[ 4 ] = types.get( "cmd4" );
            mk[ 6 ] = fields.get( "k1" );

            mtcmd[ 4 ].putValidator( mk[ 6 ], Validator_string.get( 2 ) );
            mtcmd[ 4 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // String 3D
            mtcmd[ 5 ] = types.get( "cmd5" );
            mk[ 7 ] = fields.get( "k1" );

            mtcmd[ 5 ].putValidator( mk[ 7 ], Validator_string.get( 3 ) );
            mtcmd[ 5 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // Byte
            mtcmd[ 6 ] = types.get( "cmd6" );
            mk[ 8 ] = fields.get( "k1" );
            mk[ 9 ] = fields.get( "k2" );

            mtcmd[ 6 ].putValidator( mk[ 8 ], Validator_byte.get( 0 ) );
            mtcmd[ 6 ].putValidator( mk[ 9 ], Validator_byte.get( 0 ) );
            mtcmd[ 6 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Byte 2D
            mtcmd[ 7 ] = types.get( "cmd7" );
            mk[ 10 ] = fields.get( "k1" );

            mtcmd[ 7 ].putValidator( mk[ 10 ], Validator_byte.get( 2 ) );
            mtcmd[ 7 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Byte 3D
            mtcmd[ 8 ] = types.get( "cmd8" );
            mk[ 11 ] = fields.get( "k1" );

            mtcmd[ 8 ].putValidator( mk[ 11 ], Validator_byte.get( 3 ) );
            mtcmd[ 8 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // Short
            mtcmd[ 9 ] = types.get( "cmd9" );
            mk[ 12 ] = fields.get( "k1" );
            mk[ 13 ] = fields.get( "k2" );

            mtcmd[ 9 ].putValidator( mk[ 12 ], Validator_short.get( 0 ) );
            mtcmd[ 9 ].putValidator( mk[ 13 ], Validator_short.get( 0 ) );
            mtcmd[ 9 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Short 2D
            mtcmd[ 10 ] = types.get( "cmd10" );
            mk[ 14 ] = fields.get( "k1" );

            mtcmd[ 10 ].putValidator( mk[ 14 ], Validator_short.get( 2 ) );
            mtcmd[ 10 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Short 3D
            mtcmd[ 11 ] = types.get( "cmd11" );
            mk[ 15 ] = fields.get( "k1" );

            mtcmd[ 11 ].putValidator( mk[ 15 ], Validator_short.get( 3 ) );
            mtcmd[ 11 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // Long
            mtcmd[ 12 ] = types.get( "cmd12" );
            mk[ 16 ] = fields.get( "k1" );
            mk[ 17 ] = fields.get( "k2" );

            mtcmd[ 12 ].putValidator( mk[ 16 ], Validator_long.get( 0 ) );
            mtcmd[ 12 ].putValidator( mk[ 17 ], Validator_long.get( 0 ) );
            mtcmd[ 12 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Long 2D
            mtcmd[ 13 ] = types.get( "cmd13" );
            mk[ 18 ] = fields.get( "k1" );

            mtcmd[ 13 ].putValidator( mk[ 18 ], Validator_long.get( 2 ) );
            mtcmd[ 13 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Long 3D
            mtcmd[ 14 ] = types.get( "cmd14" );
            mk[ 19 ] = fields.get( "k1" );

            mtcmd[ 14 ].putValidator( mk[ 19 ], Validator_long.get( 3 ) );
            mtcmd[ 14 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // Double
            mtcmd[ 15 ] = types.get( "cmd15" );
            mk[ 20 ] = fields.get( "k1" );
            mk[ 21 ] = fields.get( "k2" );

            mtcmd[ 15 ].putValidator( mk[ 20 ], Validator_double.get( 0 ) );
            mtcmd[ 15 ].putValidator( mk[ 21 ], Validator_double.get( 0 ) );
            mtcmd[ 15 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Double 2D
            mtcmd[ 16 ] = types.get( "cmd16" );
            mk[ 22 ] = fields.get( "k1" );

            mtcmd[ 16 ].putValidator( mk[ 22 ], Validator_double.get( 2 ) );
            mtcmd[ 16 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Double 3D
            mtcmd[ 17 ] = types.get( "cmd17" );
            mk[ 23 ] = fields.get( "k1" );

            mtcmd[ 17 ].putValidator( mk[ 23 ], Validator_double.get( 3 ) );
            mtcmd[ 17 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Float
            mtcmd[ 18 ] = types.get( "cmd18" );
            mk[ 24 ] = fields.get( "k1" );
            mk[ 25 ] = fields.get( "k2" );

            mtcmd[ 18 ].putValidator( mk[ 24 ], Validator_float.get( 0 ) );
            mtcmd[ 18 ].putValidator( mk[ 25 ], Validator_float.get( 0 ) );
            mtcmd[ 18 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Float 2D
            mtcmd[ 19 ] = types.get( "cmd19" );
            mk[ 26 ] = fields.get( "k1" );

            mtcmd[ 19 ].putValidator( mk[ 26 ], Validator_float.get( 2 ) );
            mtcmd[ 19 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Float 3D
            mtcmd[ 20 ] = types.get( "cmd20" );
            mk[ 27 ] = fields.get( "k1" );

            mtcmd[ 20 ].putValidator( mk[ 27 ], Validator_float.get( 3 ) );
            mtcmd[ 20 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Int
            mtcmd[ 21 ] = types.get( "cmd21" );
            mk[ 28 ] = fields.get( "k1" );
            mk[ 29 ] = fields.get( "k2" );

            mtcmd[ 21 ].putValidator( mk[ 28 ], Validator_int.get( 0 ) );
            mtcmd[ 21 ].putValidator( mk[ 29 ], Validator_int.get( 0 ) );
            mtcmd[ 21 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Int 2D
            mtcmd[ 22 ] = types.get( "cmd22" );
            mk[ 30 ] = fields.get( "k1" );

            mtcmd[ 22 ].putValidator( mk[ 30 ], Validator_int.get( 2 ) );
            mtcmd[ 22 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );

            // Int 3D
            mtcmd[ 23 ] = types.get( "cmd23" );
            mk[ 31 ] = fields.get( "k1" );

            mtcmd[ 23 ].putValidator( mk[ 31 ], Validator_int.get( 3 ) );
            mtcmd[ 23 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            
            // Object
            mtcmd[ 24 ] = types.get( "cmd24" );
            mk[ 32 ] = fields.get( "k1" );

            mtcmd[ 24 ].putValidator( mk[ 32 ], Validator_object.get( 0 ) );
            mtcmd[ 24 ].putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
            	
        	
        }

		public Field getField( Integer id )
		{
			return fields.get( id );
		}

		public Field getField( String name )
		{
			return fields.get( name );
		}

		public Collection<Field> getFields()
		{
			return fields.values();
		}

		public Type getType( Integer id )
		{
			return types.get( id );
		}

		public Type getType( String name )
		{
			return types.get( name );
		}

		public Collection<Type> getTypes()
		{
			return types.values();
		}
    }

}
