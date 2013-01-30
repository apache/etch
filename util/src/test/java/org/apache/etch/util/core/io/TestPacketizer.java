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

package org.apache.etch.util.core.io;

import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.core.Who;
import org.junit.Before;
import org.junit.Test;


/**
 * JUnit Test for Packetizer
 */
public class TestPacketizer
{
	private final MyTransportData transport = new MyTransportData();
	private final MySessionPacket session = new MySessionPacket();
	private final Packetizer p = new Packetizer( transport, "tcp:", null );
	private final Who who = new Who() { /* nothing */ };

	/** setup test */
	@Before
	public void setUp()
	{
		p.setSession( session );
	}
	
	/** @throws Exception */
	@Test
	public void connections() throws Exception
	{
		assertSame( transport, p.getTransport() );
		assertSame( session, p.getSession() );
		assertSame( p, transport.getSession() );
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

		p.transportPacket( who, buf );
		
		assertSame( What.DATA, transport.what );
		assertTrue( transport.check( result ) );
		assertSame( who, transport.recipient );
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

		p.transportPacket( who, buf );
		
		assertSame( What.DATA, transport.what );
		assertTrue( transport.check( result ) );
		assertSame( who, transport.recipient );
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

		p.transportPacket( who, buf );
		
		assertSame( What.DATA, transport.what );
		assertTrue( transport.check( result ) );
		assertSame( who, transport.recipient );
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

		p.transportPacket( who, buf );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void packet5() throws Exception
	{
		// Create too-small packet to send
		assert p.headerSize() > 0;
		FlexBuffer buf = new FlexBuffer( new byte[] {} );
		p.transportPacket( who, buf );
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

		p.sessionData( who, buf );
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

		p.sessionData( who, buf );
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

		p.sessionData( who, buf );
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

		p.sessionData( who, buf );
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

		p.sessionData( who, buf );
		
		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );
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

		p.sessionData( who, buf );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result ) );
		assertSame( who, session.sender );
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

		p.sessionData( who, buf );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result ) );
		assertSame( who, session.sender );
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

		p.sessionData( who, buf );

		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );
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

		p.sessionData( who, buf );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result ) );
		assertSame( who, session.sender );
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

		p.sessionData( who, buf );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result ) );
		assertSame( who, session.sender );
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

		p.sessionData( who, buf );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result ) );
		assertSame( who, session.sender );
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

		p.sessionData( who, buf );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result ) );
		assertSame( who, session.sender );
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

		p.sessionData( who, buf );

		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );

		FlexBuffer buf2 = new FlexBuffer( new byte[]
		{
			// packet 1 header (remainder):
		    0, 0,
			// packet 2 header
		    -34, -83, -66, -17, 0, 0, 0, 0
		} );

		p.sessionData( null, buf2 );

		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );
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

		p.sessionData( who, buf );

		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );

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

		p.sessionData( who, buf2 );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result2 ) );
		assertSame( who, session.sender );
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

		p.sessionData( who, buf );

		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );

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

		p.sessionData( null, buf2 );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result2 ) );
		assertNull( session.sender );
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

		p.sessionData( who, buf );

		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );

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

		p.sessionData( null, buf2 );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result2 ) );
		assertNull( session.sender );
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

		p.sessionData( who, buf );

		assertNull( session.what );
		assertTrue( session.check( null ) );
		assertNull( session.sender );

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

		p.sessionData( null, buf2 );

		assertSame( What.PACKET, session.what );
		assertTrue( session.check( result2 ) );
		assertNull( session.sender );
	}

	/** @throws Exception */
	@Test
	public void sessionQuery() throws Exception
	{
		Object QUERY = "foo";
		Object RESULT = "bar";
		
		assertNull( session.what );
		assertNull( session.query );
		session.query_result = RESULT;
		
		Object result = p.sessionQuery( QUERY );

		assertSame( What.SESSION_QUERY, session.what );
		assertSame( QUERY, session.query );
		assertSame( RESULT, result );
	}

	/** @throws Exception */
	@Test
	public void sessionControl() throws Exception
	{
		Object CONTROL = "foo";
		Object VALUE = "bar";
		
		assertNull( session.what );
		assertNull( session.control );
		assertNull( session.value );
		
		p.sessionControl( CONTROL, VALUE );

		assertSame( What.SESSION_CONTROL, session.what );
		assertSame( CONTROL, session.control );
		assertSame( VALUE, session.value );
	}

	/** @throws Exception */
	@Test
	public void sessionNotify() throws Exception
	{
		Object EVENT = "foo";
		
		assertNull( session.what );
		assertNull( session.event );
		
		p.sessionNotify( EVENT );

		assertSame( What.SESSION_NOTIFY, session.what );
		assertSame( EVENT, session.event );
	}

	/** @throws Exception */
	@Test
	public void transportQuery() throws Exception
	{
		Object QUERY = "foo";
		Object RESULT = "bar";
		
		assertNull( transport.what );
		assertNull( transport.query );
		transport.query_result = RESULT;
		
		Object result = p.transportQuery( QUERY );

		assertSame( What.TRANSPORT_QUERY, transport.what );
		assertSame( QUERY, transport.query );
		assertSame( RESULT, result );
	}

	/** @throws Exception */
	@Test
	public void transportControl() throws Exception
	{
		Object CONTROL = "foo";
		Object VALUE = "bar";
		
		assertNull( transport.what );
		assertNull( transport.control );
		assertNull( transport.value );
		
		p.transportControl( CONTROL, VALUE );

		assertSame( What.TRANSPORT_CONTROL, transport.what );
		assertSame( CONTROL, transport.control );
		assertSame( VALUE, transport.value );
	}

	/** @throws Exception */
	@Test
	public void transportNotify() throws Exception
	{
		Object EVENT = "foo";
		
		assertNull( transport.what );
		assertNull( transport.event );
		
		p.transportNotify( EVENT );

		assertSame( What.TRANSPORT_NOTIFY, transport.what );
		assertSame( EVENT, transport.event );
	}

	/**
	 * testing events.
	 */
	enum What
	{
		/** */ DATA,
		/** */ PACKET,
		/** */ SESSION_QUERY,
		/** */ SESSION_CONTROL,
		/** */ SESSION_NOTIFY,
		/** */ TRANSPORT_QUERY,
		/** */ TRANSPORT_CONTROL,
		/** */ TRANSPORT_NOTIFY
	};
	
	/**
	 * Test jig
	 */
	public static class MyTransportData implements TransportData
	{
		/** */ public What what;
		/** */ public Object query;
		/** */ public Object query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;
		/** */ public Who recipient;
		/** */ public List<byte[]> list = new ArrayList<byte[]>();
		
		public Object transportQuery( Object query ) throws Exception
		{
			what = What.TRANSPORT_QUERY;
			this.query = query;
			return query_result;
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			what = What.TRANSPORT_CONTROL;
			this.control = control;
			this.value = value;
		}

		public void transportNotify( Object event ) throws Exception
		{
			what = What.TRANSPORT_NOTIFY;
			this.event = event;
		}
		
		public void transportData( Who recipient, FlexBuffer buf )
			throws Exception
		{
			what = What.DATA;
			this.recipient = recipient;
			list.add( buf.getAvailBytes() );
		}

		public SessionData getSession()
		{
			return session;
		}

		public void setSession( SessionData session )
		{
			this.session = session;
		}
		
		private SessionData session;

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
	}

	/**
	 * A packet handler test jig.
	 */
	public static class MySessionPacket implements SessionPacket
	{
		/** */ public What what;
		/** */ public Who sender;
		/** */ public List<byte[]> list = new ArrayList<byte[]>();
		/** */ public Object query;
		/** */ public Object query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;

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

		public void sessionPacket( Who sender, FlexBuffer buf )
		    throws Exception
		{
			what = What.PACKET;
			this.sender = sender;
			list.add( buf.getAvailBytes() );

		}

		public Object sessionQuery( Object query )
		{
			what = What.SESSION_QUERY;
			this.query = query;
			return query_result;
		}

		public void sessionControl( Object control, Object value )
		{
			what = What.SESSION_CONTROL;
			this.control = control;
			this.value = value;
		}

		public void sessionNotify( Object event )
		{
			what = What.SESSION_NOTIFY;
			this.event = event;
		}
	}
}
