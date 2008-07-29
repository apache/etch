///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 13, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.


using NUnit.Framework;
using System;
using System.Collections.Generic;
using Etch.Transport;

namespace Etch.Msg.Test
{

    [TestFixture]
    public class TestArrayValue 
    {

        [Test]
	    public void Constructor1()
	    {
		    ArrayValue av = new ArrayValue( new sbyte[0] );
		    Assert.AreEqual( (sbyte) 0, av.Type );
		    Assert.IsNull( av.CustomStructType );
		    Assert.AreEqual( 0, av.Dim );
	    }
    	
	    [Test]
	    public void Constructor2()
	    {
		    XType t1 = new XType( "t1" );
		    ArrayValue av = new ArrayValue( new sbyte[0], (sbyte) 1, t1, 2 );
		    Assert.AreEqual( (sbyte) 1, av.Type );
		    Assert.AreEqual( t1, av.CustomStructType );
		    Assert.AreEqual( 2, av.Dim );
	    }
    	
	    [Test]
	    public void constructor3()
	    {
		    XType t2 = new XType( "t2" );
            ArrayValue av = new ArrayValue( new sbyte[ 0 ], ( sbyte ) 3, t2, 4 );
		    Assert.AreEqual( (byte) 3, av.Type );
		    Assert.AreEqual( t2, av.CustomStructType );
		    Assert.AreEqual( 4, av.Dim );
	    }
    	
	    [Test]
	    public void AddMany1()
	    {
		    DateTime dt = new DateTime();
		    ArrayValue av = new ArrayValue( new Object[] { false, (sbyte) 1, (short) 2, 3, (long) 4, (float) 1.5, 1.6, "abc", null, dt } );
		    Assert.AreEqual( 10, av.Size() );
		    int index = 0;
		    Assert.AreEqual( false, av.Get( index++ ) );
		    Assert.AreEqual( (sbyte) 1, av.Get( index++ ) );
		    Assert.AreEqual( (short) 2, av.Get( index++ ) );
		    Assert.AreEqual( 3, av.Get( index++ ) );
		    Assert.AreEqual( (long) 4, av.Get( index++ ) );
		    Assert.AreEqual( (float) 1.5, av.Get( index++ ) );
		    Assert.AreEqual( 1.6, av.Get( index++ ) );
		    Assert.AreEqual( "abc", av.Get( index++ ) );
		    Assert.AreEqual( null, av.Get( index++ ) );
		    Assert.AreEqual( dt, av.Get( index++ ) );
	    }
    	
	    [Test]
	    public void AddMany2()
	    {
		    DateTime dt = new DateTime();
		    ArrayValue av = new ArrayValue( new Object[] { (float) 1.5, 1.6, "abc", null, dt, false, (sbyte) 1, (short) 2, 3, (long) 4 } );
		    Assert.AreEqual( 10, av.Size() );
		    int index = 0;
		    Assert.AreEqual( (float) 1.5, av.Get( index++ ) );
		    Assert.AreEqual( 1.6, av.Get( index++ ) );
		    Assert.AreEqual( "abc", av.Get( index++ ) );
		    Assert.AreEqual( null, av.Get( index++ ) );
		    Assert.AreEqual( dt, av.Get( index++ ) );
		    Assert.AreEqual( false, av.Get( index++ ) );
		    Assert.AreEqual( (sbyte) 1, av.Get( index++ ) );
		    Assert.AreEqual( (short) 2, av.Get( index++ ) );
		    Assert.AreEqual( 3, av.Get( index++ ) );
		    Assert.AreEqual( (long) 4, av.Get( index++ ) );
	    }
    }
}