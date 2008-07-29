package metreos.core.io.test;

import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import metreos.core.Who;
import metreos.core.io.Connection;
import metreos.core.io.DataHandler;
import metreos.core.io.DataSource;
import metreos.core.io.PacketHandler;
import metreos.core.io.PacketSource;
import metreos.core.io.Packetizer;
import metreos.util.FlexBuffer;

import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.transport.Session;
import etch.bindings.java.transport.Transport;

/**
 * JUnit Test for Packetizer
 */
public class TestPacketizer
{
	private final MyPacketHandler mph = new MyPacketHandler();

	private final Packetizer p = new Packetizer( mph );
	
	private final Who who = new Who() { /* nothing */ };

	/** setup test */
	@Before
	public void setUp()
	{
		p.setSource( mph );
	}

	////////////////////////////////////////////////////////////////
	// these tests are for packets delivered to the packet source //
	////////////////////////////////////////////////////////////////

	/** @throws Exception */
	@Test
	public void packet1() throws Exception
	{
		// Create empty packet to send
		assert p.headerSize() == 8;
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// space for header:
			0, 0, 0, 0, 0, 0, 0, 0
		    // packet data:
		} );
		
		// this is the result we expect.
		byte[][] result = new byte[][]
		{
			{
				// header:
				-34, -83, -66, -17, 0, 0, 0, 0
			    // packet data:
			}
		};

		p.packet( who, buf );
		
		assertSame( What.DATA, mph.what );
		assertTrue( mph.check( result ) );
		assertSame( who, mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void packet2() throws Exception
	{
		// Create 1 byte packet to send
		assert p.headerSize() == 8;
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// space for header:
		    0, 0, 0, 0, 0, 0, 0, 0,
		    // packet data:
		    1
		} );
		
		// this is the result we expect.
		byte[][] result = new byte[][]
		{
			{
				// header:
				-34, -83, -66, -17, 0, 0, 0, 1,
			    // packet data:
			    1
			}
		};

		p.packet( who, buf );
		
		assertSame( What.DATA, mph.what );
		assertTrue( mph.check( result ) );
		assertSame( who, mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void packet3() throws Exception
	{
		// Create 2 byte packet to send
		assert p.headerSize() == 8;
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// space for header:
		    0, 0, 0, 0, 0, 0, 0, 0,
		    // packet data:
		    2, 3
		} );
		
		byte[][] result = new byte[][]
		{
			{
				// header:
				-34, -83, -66, -17, 0, 0, 0, 2,
			    // packet data:
			    2, 3
			}
		};

		p.packet( who, buf );
		
		assertSame( What.DATA, mph.what );
		assertTrue( mph.check( result ) );
		assertSame( who, mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void packet4() throws Exception
	{
		// Create too-small packet to send
		assert p.headerSize() > 7;
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// space for header:
			0, 0, 0, 0, 0, 0, 0 // too short!
		    // packet data:
		} );

		p.packet( who, buf );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void packet5() throws Exception
	{
		// Create too-small packet to send
		assert p.headerSize() > 0;
		FlexBuffer buf = new FlexBuffer( new byte[] {} );
		p.packet( who, buf );
	}

	/////////////////////////////////////////////
	// these tests are for various bad packets //
	/////////////////////////////////////////////

	/** @throws Exception */
	@Test( expected = IOException.class )
	public void bad1() throws Exception
	{
		// Create data to send
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			0, 0, 0, 0, 0, 0, 0, 0 // bad sig
		    // packet data:
		} );

		p.data( who, buf );
	}

	/** @throws Exception */
	@Test( expected = IOException.class )
	public void bad2() throws Exception
	{
		// Create data to send
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			1, 2, 3, 4, 0, 0, 0, 0 // bad sig
		    // packet data:
		} );

		p.data( who, buf );
	}

	/** @throws Exception */
	@Test( expected = IOException.class )
	public void bad3() throws Exception
	{
		// Create data to send
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			-34, -83, -66, -17, 0, 1, 0, 0 // size too big
		    // packet data:
		} );

		p.data( who, buf );
	}

	/** @throws Exception */
	@Test( expected = IOException.class )
	public void bad4() throws Exception
	{
		// Create data to send
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			-34, -83, -66, -17, -1, -1, -1, -1 // negative packet size
		    // packet data:
		} );

		p.data( who, buf );
	}
	
	////////////////////////////////////////////////////////////
	// these tests are for a single packet in a single buffer //
	////////////////////////////////////////////////////////////

	/** @throws Exception */
	@Test
	public void singleSingleData0() throws Exception
	{
		// Create data to send
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			-34, -83, -66, -17, 0, 0, 0, 0
		    // packet data:
		} );

		p.data( who, buf );
		
		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void singleSingleData1() throws Exception
	{
		// length = 1
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			-34, -83, -66, -17, 0, 0, 0, 1,
		    // packet data:
		    1
		} );
		byte[][] result = new byte[][]
		{
			{
			    // packet data:
				1
			}
		};

		p.data( who, buf );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result ) );
		assertNull( mph.xrecipient );
		assertSame( who, mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void singleSingleData2() throws Exception
	{
		// length = 2
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			-34, -83, -66, -17, 0, 0, 0, 2,
		    // packet data:
		    3, 4
		} );
		byte[][] result = new byte[][]
		{
			{
			    // packet data:
			    3, 4
			}
		};

		p.data( who, buf );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result ) );
		assertNull( mph.xrecipient );
		assertSame( who, mph.xsender );
		assertNull( mph.xreset );
	}

	// //////////////////////////////////////////////////////
	// these tests are for two packets in a single buffer //
	// //////////////////////////////////////////////////////

	/** @throws Exception */
	@Test
	public void data1() throws Exception
	{
		// 2x length = 0
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet header:
			-34, -83, -66, -17, 0, 0, 0, 0,
			// packet header:
			-34, -83, -66, -17, 0, 0, 0, 0
		} );

		p.data( who, buf );

		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void data2() throws Exception
	{
		// length = 1, length = 0
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet 1 header:
			-34, -83, -66, -17, 0, 0, 0, 1,
		    // packet 1 data:
		    1,
			// packet 2 header:
		    -34, -83, -66, -17, 0, 0, 0, 0,
		    // packet 2 data:
		} );
		
		byte[][] result = new byte[][]
		{
		    {
			    // packet 1 data:
			    1
		    }
		};

		p.data( who, buf );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result ) );
		assertNull( mph.xrecipient );
		assertSame( who, mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void data3() throws Exception
	{
		// length = 0, length = 1
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet 1 header:
			-34, -83, -66, -17, 0, 0, 0, 0,
		    // packet 1 data:
			// packet 2 header:
			-34, -83, -66, -17, 0, 0, 0, 1,
		    // packet 2 data:
		    2
		} );
		
		byte[][] result = new byte[][]
		{
		    {
			    // packet 2 data:
			    2
		    }
		};

		p.data( who, buf );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result ) );
		assertNull( mph.xrecipient );
		assertSame( who, mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void data4() throws Exception
	{
		// 2x length = 1
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet 1 header:
			-34, -83, -66, -17, 0, 0, 0, 1,
		    // packet 1 data:
		    1,
			// packet 2 header:
		    -34, -83, -66, -17, 0, 0, 0, 1,
		    // packet 2 data:
		    2
		} );
		
		byte[][] result = new byte[][]
		{
		    {
			    // packet 1 data:
			    1
		    },
		    {
			    // packet 2 data:
			    2
		    }
		};

		p.data( who, buf );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result ) );
		assertNull( mph.xrecipient );
		assertSame( who, mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void doubleSingleData2() throws Exception
	{
		// length = 2
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet 1 header:
			-34, -83, -66, -17, 0, 0, 0, 2,
		    // packet 1 data:
		    3, 4,
			// packet 2 header:
		    -34, -83, -66, -17, 0, 0, 0, 2,
		    // packet 2 data:
		    5, 6
		} );
		byte[][] result = new byte[][]
		{
		    {
			    // packet 1 data:
		        3, 4
		    },
		    {
			    // packet 2 data:
		        5, 6
		    }
		};

		p.data( who, buf );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result ) );
		assertNull( mph.xrecipient );
		assertSame( who, mph.xsender );
		assertNull( mph.xreset );
	}

	// ////////////////////////////////////////////////////////////////////
	// these tests are for two packets in two buffers with header split //
	// ////////////////////////////////////////////////////////////////////

	/** @throws Exception */
	@Test
	public void doubleSingle_HeaderSplit_Data0() throws Exception
	{
		// length = 0
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet 1 header (partial):
			-34, -83, -66, -17, 0, 0
		} );

		p.data( who, buf );

		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );

		FlexBuffer buf2 = new FlexBuffer( new byte[]
		{
			// packet 1 header (remainder):
		    0, 0,
			// packet 2 header
		    -34, -83, -66, -17, 0, 0, 0, 0
		} );

		p.data( null, buf2 );

		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void doubleSingle_HeaderSplit_Data1() throws Exception
	{
		// length = 1
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			// packet 1 header (partial):
			-34, -83, -66, -17, 0, 0
		} );

		p.data( who, buf );

		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );

		FlexBuffer buf2 = new FlexBuffer( new byte[]
		{
			// packet 1 header (remainder):
		    0, 1,
		    // packet 1 data:
		    1,
			// packet 2 header:
		    -34, -83, -66, -17, 0, 0, 0, 1,
		    // packet 2 data:
		    2
		} );
		
		byte[][] result2 = new byte[][]
		{
		    {
			    // packet 1 data:
			    1
		    },
		    {
			    // packet 2 data:
			    2
		    }
		};

		p.data( who, buf2 );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result2 ) );
		assertNull( mph.xrecipient );
		assertSame( who, mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void doubleSingle_HeaderSplit_Data2() throws Exception
	{
		// length = 2
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			-34, -83, -66, -17, 0, 0
		} );

		p.data( who, buf );

		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );

		FlexBuffer buf2 = new FlexBuffer( new byte[]
		{
		    0, 2, 3, 4, -34, -83, -66, -17, 0, 0, 0, 2, 5, 6
		} );
		byte[][] result2 = new byte[][]
		{
		    {
		        3, 4
		    },
		    {
		        5, 6
		    }
		};

		p.data( null, buf2 );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result2 ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	// //////////////////////////////////////////////////////////////////
	// these tests are for two packets in two buffers with body split //
	// //////////////////////////////////////////////////////////////////

	/** @throws Exception */
	@Test
	public void doubleSingle_BodySplit_Data2() throws Exception
	{
		// length = 2
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			-34, -83, -66, -17, 0, 0, 0, 2, 1
		} );

		p.data( who, buf );

		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );

		FlexBuffer buf2 = new FlexBuffer( new byte[]
		{
		    2, -34, -83, -66, -17, 0, 0, 0, 2, 3, 4
		} );
		byte[][] result2 = new byte[][]
		{
		    {
		        1, 2
		    },
		    {
		        3, 4
		    }
		};

		p.data( null, buf2 );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result2 ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void doubleSingle_BodySplit_Data3() throws Exception
	{
		// length = 3
		FlexBuffer buf = new FlexBuffer( new byte[]
		{
			-34, -83, -66, -17, 0, 0, 0, 3, 5, 6
		} );

		p.data( who, buf );

		assertNull( mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );

		FlexBuffer buf2 = new FlexBuffer( new byte[]
		{
		    7, -34, -83, -66, -17, 0, 0, 0, 3, 8, 9, 10
		} );
		byte[][] result2 = new byte[][]
		{
		    {
		        5, 6, 7
		    },
		    {
		        8, 9, 10
		    }
		};

		p.data( null, buf2 );

		assertSame( What.PACKET, mph.what );
		assertTrue( mph.check( result2 ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void up() throws Exception
	{
		p.sessionNotify( Session.UP );

		assertSame( What.UP, mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void down() throws Exception
	{
		p.sessionNotify( Session.DOWN );

		assertSame( What.DOWN, mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void localAddress() throws Exception
	{
		p.transportQuery( Connection.LOCAL_ADDRESS );

		assertSame( What.LOCALADDRESS, mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void remoteAddress() throws Exception
	{
		p.transportQuery( Connection.REMOTE_ADDRESS );

		assertSame( What.REMOTEADDRESS, mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/** @throws Exception */
	@Test
	public void stop() throws Exception
	{
		p.transportControl( Transport.STOP, null );

		assertSame( What.STOP, mph.what );
		assertTrue( mph.check( null ) );
		assertNull( mph.xrecipient );
		assertNull( mph.xsender );
		assertNull( mph.xreset );
	}

	/**
	 * testing events.
	 */
	enum What
	{
		/** blah */ DATA,
		/** blah */ PACKET,
		/** blah */ UP,
		/** blah */ DOWN,
		/** blah */ LOCALADDRESS,
		/** blah */ REMOTEADDRESS,
		/** blah */ STOP,
		/** blah */ STARTED,
		/** blah */ STOPPED,
		/** blah */ EXCEPTION
	};

	/**
	 * A packet handler test jig.
	 */
	public static class MyPacketHandler implements PacketHandler, DataSource
	{
		/**
		 * What operation was tried.
		 */
		public What what;
		
		/**
		 * The recipient of data calls.
		 */
		public Who xrecipient;

		/**
		 * The sender of packet calls.
		 */
		public Who xsender;
		
		/**
		 * The reset of close calls.
		 */
		public Boolean xreset;

		/**
		 * The bytes of data or packet calls.
		 */
		private List<byte[]> list = new ArrayList<byte[]>();

		// Auxiliary methods

		/**
		 * @param result
		 * @return true if the result matches list.
		 */
		public boolean check( byte[][] result )
		{
			if (result == null)
				return list.size() == 0;
			
			if (list.size() != result.length)
				return false;

			for (int i = 0; i < list.size(); i++)
				if (!check( list.get( i ), result[i] ))
					return false;
			
			return true;
		}

		private boolean check( byte[] a, byte[] b )
		{
			if (a.length != b.length)
				return false;

			for (int i = 0; i < a.length; i++)
			{
				if (a[i] != b[i])
					return false;
			}
			
			return true;
		}

		// PacketHandler methods

		public void packet( Who sender, FlexBuffer buf )
		    throws Exception

		{
			what = What.PACKET;
			xsender = sender;
			list.add( buf.getAvailBytes() );

		}

		// DataSource methods

		public void data( Who recipient, FlexBuffer buf ) throws Exception
		{
			what = What.DATA;
			xrecipient = recipient;
			list.add( buf.getAvailBytes() );
		}

		public DataHandler getHandler()
		{
			// ignore.
			return null;
		}

		public void setHandler( DataHandler handler )
		{
			// ignore.
		}

		public PacketSource getSource()
		{
			// ignore.
			return null;
		}

		public void setSource( PacketSource src )
		{
			// ignore.
		}

		public void sessionControl( Object control, Object value )
		{
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void sessionNotify( Object event )
		{
			if (event.equals( Session.UP))
			{
				what = What.UP;
				return;
			}
			
			if (event.equals( Session.DOWN))
			{
				what = What.DOWN;
				return;
			}
		}

		public Object sessionQuery( Object query )
		{
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void transportControl( Object control, Object value ) throws Exception
		{
			if (control.equals( Transport.STOP))
			{
				what = What.STOP;
				return;
			}
			
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void transportNotify( Object event ) throws Exception
		{
			// ignore.
		}

		public Object transportQuery( Object query ) throws Exception
		{
			if (query.equals( Connection.LOCAL_ADDRESS ))
			{
				what = What.LOCALADDRESS;
				return null;
			}
			
			if (query.equals( Connection.REMOTE_ADDRESS ))
			{
				what = What.REMOTEADDRESS;
				return null;
			}
			
			throw new UnsupportedOperationException( "unknown query "+query );
		}
	}
}
