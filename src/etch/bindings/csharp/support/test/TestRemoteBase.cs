///$Id$
///
///Created by Champakesan, Badri Narayanan on Jul 02, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using NUnit.Framework;
using Etch.Msg;
using Etch.Support;
using System.Collections.Generic;
//using Etch.Support.Test;

namespace Etch.Support.Test
{

    [TestFixture]
    public class TestRemoteBase
    {
        public void Init()
        {
            svc.xmsg = null;
            svc.xresponseType = null;
            svc.xresponseField = null;
            svc.xtimeout = 0;
        }

        private static MyDeliveryService svc = new MyDeliveryService(); //declared static
        //private static DefaultValueFactory vf = new DefaultValueFactory();//declared static
        private static MyDefaultValueFactory vf = new MyDefaultValueFactory();
        private RemoteBase<DefaultValueFactory> rb = new RemoteBase<DefaultValueFactory>(svc, vf);
    	private XType mt = new XType( "foo" );
    	private XType rmt = new XType( "bar" );
    	private Field rmf = new Field( "baz" );
	
	    [Test]
	    public void NewMessage()
	    {
		    Message msg = rb._NewMessage( mt );
		    msg.CheckType( mt );
		    Assert.AreSame( vf, msg._vf );
		    Assert.IsNull( svc.what );
		    Assert.IsNull( svc.xmsg );
		    Assert.IsNull( svc.xresponseType );
		    Assert.IsNull( svc.xresponseField );
		    Assert.AreEqual( 0, svc.xtimeout );
	    }
    	
	    [Test]
	    public void Send() 
	    {
		    Message msg = rb._NewMessage( mt );
    		
		    rb._svc.Send( msg );
		    Assert.IsTrue( svc.what.Equals(What.SEND));
		    Assert.AreSame( svc.xmsg, msg );
		    Assert.IsNull( svc.xresponseType );
		    Assert.IsNull( svc.xresponseField );
		    Assert.AreEqual( 0, svc.xtimeout );
	    }
    	
        [Test]
	    public void Call() 
	    {
		    Message msg = rb._NewMessage( mt );

            Object result = rb._svc.EndCall( rb._svc.BeginCall( msg ), rmt, rmf, 98 );

		    Assert.IsTrue( svc.what.Equals(What.CALL));
		    Assert.AreSame( svc.xmsg, msg );
		    Assert.AreSame( svc.xresponseType, rmt );
		    Assert.AreSame( svc.xresponseField, rmf );
		    Assert.AreEqual( svc.xtimeout, 98 );
		    Assert.AreEqual( result, 23 );
	    }
    	
	    public enum What { SEND, CALL, SHUTDOWN, BEGINCALL };
    	
	    public class MyDeliveryService : DeliveryService
	    {

            public Enum what;
            
            public void Init()
            {
                what = null;
            }

		    public Message xmsg;
		    public XType xresponseType;
		    public Field xresponseField;
		    public int xtimeout;
            public Mailbox xmb;
    		
		    public void Send( Message msg ) 
		    {
			    Assert.IsNull( what );
			    what = What.SEND;
			    this.xmsg = msg;
		    }

            public Mailbox BeginCall( Message msg )
            {
                Init();
                Assert.IsNull( what );
                what = What.BEGINCALL;
                this.xmsg = msg;
                this.xmb = new Etch.Transport.PlainMailbox( null, 0L, 0, 0, 1 );
                return this.xmb;
            }

            public Object EndCall( Mailbox mb, XType responseType,
			                    Field responseField, int timeout ) 
		    {
                Assert.AreEqual( What.BEGINCALL, what );
                Assert.AreEqual( xmb, mb );
			    what = What.CALL;
                xmb = null;
			    this.xresponseType = responseType;
			    this.xresponseField = responseField;
			    this.xtimeout = timeout;
			    return 23;
		    }

            public void TransportNotify( Object eventObj )
            {
                // ignore
            }

            public Object TransportQuery( Object query )
            {
                // ignore
                return null;
            }

            public void TransportControl( Object control, Object value )
            {
                // ignore
            }
	    }

        /// <summary>
        /// A fake implementation of DefaultValueFactory for testing
        /// </summary>
        /// 
	    public class MyDefaultValueFactory : DefaultValueFactory
	    {
		    private readonly static TypeMap types = new TypeMap();

		    private readonly static FieldMap fields = new FieldMap();

		    static MyDefaultValueFactory()
		    {
			    DefaultValueFactory.Init( types, fields );
		    }

		    /**
		     * Constructs the ValueFactoryFake.
		     */
		    public MyDefaultValueFactory()
		    {
			    // nothing to do.
		    }

		    public override Field GetField( int? id )
		    {
			    return fields.Get( id );

		    }

            public override Field GetField( String name )
		    {
			    return fields.Get( name );
		    }

            public override ICollection<Field> GetFields()
		    {
			    return fields.Values();
		    }

            public override XType GetType( String name )
		    {
			    return types.Get( name );
		    }

            public override ICollection<XType> GetTypes()
		    {
			    return types.Values();
		    }

            public override XType GetType( int? id )
		    {
			    return types.Get( id );
		    }
	    }
  }
}