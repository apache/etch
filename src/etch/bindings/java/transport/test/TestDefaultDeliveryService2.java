package etch.bindings.java.transport.test;

import static org.junit.Assert.assertSame;

import java.util.concurrent.TimeoutException;

import metreos.core.Who;
import metreos.core.io.DataHandler;
import metreos.core.io.DataSource;
import metreos.util.FlexBuffer;

import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.support.MessageSource;
import etch.bindings.java.support.StubBase;
import etch.bindings.java.transport.DefaultDeliveryService2;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.Session;
import etch.bindings.java.transport.Transport;

/**
 * Test DefaultDeliveryService2
 */
public class TestDefaultDeliveryService2
{
	private DefaultDeliveryService2<MyStub> svc;

	/**
	 * testing stub.
	 */
	final MyStub ms = new MyStub( null, null );;
	
	private final DefaultValueFactory vf = new DefaultValueFactory();

	private final MyDataSource ds = new MyDataSource();

	private final Type mt_add = new Type( "add" );

	private final Type mt_add_result = new Type( "add_result" );

	private final Field mf_x = new Field( "x" );

	private final Field mf_y = new Field( "y" );

	private final Field mf_result = new Field( "result" );

	private Message constructSimpleMessage() throws Exception
	{
		Type mt_function = new Type( "function" );
		
		vf.addType( mt_function );

		mt_function.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
		
		return new Message( mt_function, vf );
	}

	private Message constructAddMessage() throws Exception
	{
		vf.addType( mt_add );
		vf.addType( mt_add_result );
		vf.addField( mf_x );
		vf.addField( mf_y );
		vf.addField( mf_result );
		
		mt_add.putValidator( mf_x, Validator_int.get( 0 ) );
		mt_add.putValidator( mf_y, Validator_int.get( 0 ) );
		mt_add.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
		
		mt_add_result.putValidator( mf_result, Validator_int.get( 0 ) );
		mt_add_result.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
		mt_add_result.putValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.get( 0 ) );
		
		Message msg = new Message( mt_add, vf );
		msg.put( mf_x, 4 );
		msg.put( mf_y, 5 );
		return msg;
	}

	/** @throws Exception */
	@Before
	public void setUp() throws Exception
	{
		DefaultDeliveryService2<MyStub> dds = new DefaultDeliveryService2<MyStub>()
		{
			@Override
			protected MyStub newStub( MailboxMessageSource src )
			{
				return ms;

			}

			@Override
			public String toString()
			{
				return "DefaultDeliveryService2 testing.";
			}
		};

		svc = DefaultDeliveryService2.setUp( dds, ds, vf );

	}

//	@Test
//	public void up() throws Exception
//	{
//
//		svc.up( new MailboxManager( svc, 0 ) );
//		assertSame( ms.what, What.MYSTUBCREATED );
//	}

//	@Test
//	public void down() throws Exception
//	{
//		svc.up( new MailboxManager( svc, 0 ) );
//		svc.down( new MailboxManager( svc, 0 ) );
//		assertSame( ms.what, What.MYSTUBMESSAGE );
//	}

	/** @throws Exception */
	@Test
	public void shutdownOutput() throws Exception
	{
		svc.transportControl( Transport.STOP, null );
		assertSame( ds.what, What.MYDATASOURCESHUTDOWNOUTPUT );
	}

	/** @throws Exception */
	@Test
	public void message() throws Exception
	{
		svc.sessionNotify( Session.UP );
		svc.message( null, constructSimpleMessage() );
		assertSame( ms.what, What.MYSTUBMESSAGE );
	}

	/** @throws Exception */
	@Test
	public void send() throws Exception
	{
		svc.send( constructSimpleMessage() );
		assertSame( ds.what, What.MYDATASOURCEDATA );
	}

	/** @throws Exception */
	@Test
	public void voidcall() throws Exception
	{
		try
		{
			svc.endvoidcall( svc.begincall( constructAddMessage() ),
			    mt_add_result, mf_result, 1 );
		}
		catch ( TimeoutException e )
		{
			assertSame( ds.what, What.MYDATASOURCEDATA );
		}
	}

	/** @throws Exception */
	@Test
	public void call() throws Exception
	{
		try
		{
			svc.endcall( svc.begincall( constructAddMessage() ), mt_add_result,
			    mf_result, 1 );
		}
		catch ( TimeoutException e )
		{
			assertSame( ds.what, What.MYDATASOURCEDATA );
		}
	}

	/**
	 * Test events.
	 */
	enum What
	{
		/** */ MYSTUBCREATED,
		/** */ MYSTUBMESSAGE,
		/** */ MYDATASOURCESHUTDOWNOUTPUT,
		/** */ MYDATASOURCEDATA
	};

	/**
	 * DataSource for testing.
	 */
	class MyDataSource implements DataSource
	{
		/** */ 
		public What what;

		public void data( Who recipient, FlexBuffer buf ) throws Exception
		{

			what = What.MYDATASOURCEDATA;
		}
		
		public DataHandler getHandler()
		{
			return null;
		}

		public void setHandler( DataHandler handler )
		{

			// nothing to do.
		}

		public void transportControl( Object control, Object value ) throws Exception
		{
			if (control.equals( Transport.STOP ))
			{
				what = What.MYDATASOURCESHUTDOWNOUTPUT;
			}
		}

		public void transportNotify( Object event ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public Object transportQuery( Object query ) throws Exception
		{
			// TODO Auto-generated method stub
			return null;
		}
	}

	/**
	 * StubBase for testing.
	 */
	public class MyStub extends StubBase
	{
		/** */
		public What what;

		/**
		 * Construct the object.
		 * @param src 
		 * @param obj 
		 */
		public MyStub( MessageSource src, Object obj )
		{

			super( src, obj, null, null );
			what = What.MYSTUBCREATED;
		}

		@SuppressWarnings( "unused" )
		// private final Foo obj;
		
		@Override
		public boolean message( Who sender, Message msg )
		    throws Exception
		{
			what = What.MYSTUBMESSAGE;
			return true;
		}
	}
}
