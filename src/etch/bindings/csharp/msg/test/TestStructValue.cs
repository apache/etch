///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 13, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.


using System;
using System.Collections.Generic;
using NUnit.Framework;

namespace Etch.Msg.Test
{
    [TestFixture]
    public class TestStructValue
    {
        private XType mt1 = new XType("one");
        private XType mt2 = new XType("two");
        private Field mf1 = new Field( "f1" );
        private Field mf2 = new Field( "f2" );
        private Field mf3 = new Field( "f3" );
        private Field mf4 = new Field( "f4" );
        private Field mf5 = new Field( "f5" );
        private Field mf6 = new Field( "f6" );

        public TestStructValue()
        {
            mt1.PutValidator( mf1, Validator_boolean.Get( 0 ) );
            mt1.PutValidator( mf2, Validator_boolean.Get( 1 ) );
            mt1.PutValidator( mf3, Validator_int.Get( 0 ) );
            mt1.PutValidator( mf4, Validator_int.Get( 1 ) );
            mt1.PutValidator( mf5, Validator_string.Get( 0 ) );
            mt1.PutValidator( mf6, Validator_string.Get( 1 ) );
        }

        [Test]
        public void Test2str() 
	    {
            StructValue sv = new StructValue( mt1 );
            Assert.AreEqual( "one(785945377): {}", sv.ToString() );

            sv = new StructValue( mt2 );
            Assert.AreEqual( "two(827843303): {}", sv.ToString() );

            sv = new StructValue( mt1 );
            sv.Add( mf1, true );
            Assert.AreEqual( "one(785945377): {f1(1512176592)=True}", sv.ToString() );

            sv = new StructValue( mt1 );
            sv.Add( mf3, 23 );
            Assert.AreEqual( "one(785945377): {f3(1512176594)=23}", sv.ToString() );

            sv = new StructValue( mt1 );
            sv.Add( mf1, false );
            sv.Add( mf3, 74 );
            Assert.AreEqual( "one(785945377): {f1(1512176592)=False, f3(1512176594)=74}", sv.ToString() );
	    }

        [Test]
        public void GetXType()
        {
            StructValue sv = new StructValue(mt1);
            Assert.AreSame(mt1, sv.GetXType);

            sv = new StructValue(mt2);
            Assert.AreSame(mt2, sv.GetXType);
        }

        [Test]
        public void IsType()
        {
            StructValue sv = new StructValue(mt1);
            Assert.IsTrue(sv.IsType(mt1));
            Assert.IsFalse(sv.IsType(mt2));

            sv = new StructValue(mt2);
            Assert.IsTrue(sv.IsType(mt2));
            Assert.IsFalse(sv.IsType(mt1));
        }

        [Test]
        public void CheckType_()
        {
            StructValue sv = new StructValue(mt1);
            sv.CheckType(mt1);
            try { sv.CheckType(mt2); Assert.IsTrue(false); }
            catch(ArgumentException ) { 
                Assert.IsTrue(true);
            }

            sv = new StructValue(mt2);
            sv.CheckType(mt2);
            try { sv.CheckType(mt1); Assert.IsTrue(false); }
            catch(ArgumentException ) { Assert.IsTrue(true);
            }
        }

        [Test]
        public void Iterator()
        {
            StructValue sv = new StructValue( mt1 );
            Assert.IsFalse( sv.Iterator().MoveNext() );

            sv.Add( mf1, true );

            IEnumerator<KeyValuePair<Field, Object>> it = sv.Iterator();
            Assert.IsTrue( it.MoveNext() );
            KeyValuePair<Field, Object> me = it.Current;
            Assert.AreEqual( mf1, me.Key );
            Assert.AreEqual( true, me.Value );
            Assert.IsFalse( it.MoveNext() );

            Dictionary<Field, Object> vals = new Dictionary<Field, object>();
            vals.Add( mf1, true );
            vals.Add( mf3, 2 );
            vals.Add( mf5, "a" );

            sv.Add( mf3, 2 );
            sv.Add( mf5, "a" );

            it = sv.Iterator();
            while ( it.MoveNext() )
            {
                me = it.Current;
                Field key = me.Key;
                Assert.IsTrue( vals.ContainsKey( key ) );
                Assert.AreEqual( vals[key], me.Value );
                vals.Remove( key );
            }
            Assert.IsTrue( vals.Count == 0 );
        }

        public void CheckType1()
	    {
		    new StructValue( mt1 ).CheckType( mt1 );
		    new StructValue( mt2 ).CheckType( mt2 );
	    }

	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void CheckType2()
	    {
		    new StructValue( mt1 ).CheckType( mt2 );
	    }

	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void CheckType()
	    {
		    new StructValue( mt2 ).CheckType( mt1 );
	    }
    	
	     
	    [Test]
	    public void Get() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    Assert.IsTrue( sv.Count == 0 );
		    Assert.AreEqual( 0, sv.Count );
            Assert.IsNull( sv.Get( mf1 ) );
    		
		    sv.Add( mf1, true );
		    Assert.IsFalse( sv.Count == 0 );
		    Assert.AreEqual( 1, sv.Count );
		    Assert.AreEqual( true, sv.Get( mf1 ) );
    		
		    sv.Add( mf1, false );
		    Assert.IsFalse( sv.Count == 0 );
		    Assert.AreEqual( 1, sv.Count );
		    Assert.AreEqual( false, sv.Get( mf1 ) );
    		
		    sv.Add( mf1, null );
		    Assert.IsTrue( sv.Count == 0 );
		    Assert.AreEqual( 0, sv.Count );
		    Assert.IsNull( sv.Get( mf1 ) );
	    }
    	
	     
	    [Test]
	    public void Remove() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    Assert.IsTrue( sv.Count == 0 );
		    Assert.AreEqual( 0, sv.Count );
		    Assert.IsNull( sv.Get( mf1 ) );
    		
		    sv.Remove( mf1 );
		    Assert.IsTrue( sv.Count == 0 );
		    Assert.AreEqual( 0, sv.Count );
		    Assert.IsNull( sv.Get( mf1 ) );
    		
		    sv.Add( mf1, true );
		    Assert.IsFalse( sv.Count == 0 );
		    Assert.AreEqual( 1, sv.Count );
		    Assert.AreEqual( true, sv.Get( mf1 ) );
    		
		    sv.Remove( mf1 );
		    Assert.IsTrue( sv.Count == 0 );
		    Assert.IsNull( sv.Get( mf1 ) );
    		
		    sv.Remove( mf1 );
		    Assert.AreEqual( 0, sv.Count );
		    Assert.IsNull( sv.Get( mf1 ) );
	    }
    	
	     
	    [Test]
	    public void Put1() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, false );
		    sv.Add( mf2, new Boolean[] { true, false } );
		    sv.Add( mf3, 1 );
		    sv.Add( mf4, new int[] { 1, 2 } );
		    sv.Add( mf5, "a" );
		    sv.Add( mf6, new String[] { "a", "b" } );
		    Assert.AreEqual( 6, sv.Count );
	    }
    	
	     
	    [Test]
	    public void Put2() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, null );
		    sv.Add( mf2, null );
		    sv.Add( mf3, null );
		    sv.Add( mf4, null );
		    sv.Add( mf5, null );
		    sv.Add( mf6, null );
		    Assert.AreEqual( 0, sv.Count );
	    }
    	
	     
	    [Test]
	    public void Put3() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, false );
		    sv.Add( mf2, new Boolean[] { true, false } );
		    sv.Add( mf3, 1 );
		    sv.Add( mf4, new int[] { 1, 2 } );
		    sv.Add( mf5, "a" );
		    sv.Add( mf6, new String[] { "a", "b" } );
		    Assert.AreEqual( 6, sv.Count );
    		
		    // now "remove" them all
    		
		    sv.Add( mf1, null );
		    sv.Add( mf2, null );
		    sv.Add( mf3, null );
		    sv.Add( mf4, null );
		    sv.Add( mf5, null );
		    sv.Add( mf6, null );
		    Assert.AreEqual( 0, sv.Count );
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put5() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, (sbyte) 1 ); // wrong type
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put6() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, (short) 1 ); // wrong type
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put7() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, 1 ); // wrong type
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put8() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, (long) 1 ); // wrong type
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put9() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, (float) 1.2 ); // wrong type
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put10() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, 1.2 ); // wrong type
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put11() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf1, "a" ); // wrong type
	    }
    	
	     
	    [Test]
	    public void put12() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf2, null );
	    }
    	
	     
	    [Test]
	    public void put13() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf2, new Boolean[] {} );
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put14() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf2, true );
	    }
    	
	     
	    [Test][ExpectedException(typeof(ArgumentException))]
	    public void put15() 
	    {
		    StructValue sv = new StructValue( mt1 );
		    sv.Add( mf2, new Boolean[][] {} );
	    }

        
        //public class FakeTdo : TaggedDataOutput 
        //{
        //    public FakeTdo(StructValue struc)
        //    {
        //        this.xstruct = struc;
        //    }

        //    private StructValue xstruct;

        //    public void StartStruct(StructValue struc)
        //    {
        //        Assert.AreSame(xstruct, struc);
        //        Assert.IsFalse(started);
        //        Assert.IsFalse(ended);
        //        Assert.IsFalse(closed);

        //        started = true;
        //    }

        //    private bool started;

        //    private bool ended;

        //    private bool closed;

        //    public void WriteStructElement(Field key, Object value)
        //    {
        //        Assert.IsTrue(started);
        //        Assert.IsFalse(ended);
        //        Assert.IsFalse(closed);
        //        items.Add(key, value);
        //    }

        //    private Dictionary<Field, Object> items = new Dictionary<Field, Object>();

        //    public void EndStruct(StructValue struc)
        //    {
        //        Assert.AreSame(xstruct, struc);
        //        Assert.IsTrue(started);
        //        Assert.IsFalse(ended);
        //        Assert.IsFalse(closed);

        //        ended = true;
        //    }

        //    public void Close()
        //    {
        //        Assert.IsTrue(started);
        //        Assert.IsTrue(ended);
        //        Assert.IsFalse(closed);

        //        closed = true;

        //        Assert.AreEqual(xstruct.Count, items.Count);
        //        Assert.AreEqual(xstruct, items);
        //    }

        //    public void EndArray(ArrayValue array)
        //    {
        //        throw new NotSupportedException("not implemented");
        //    }

        //    public void EndMessage(Message msg)
        //    {
        //        throw new NotSupportedException("not implemented");
        //    }

        //    public void StartArray(ArrayValue array)
        //    {
        //        throw new NotSupportedException("not implemented");
        //    }

        //    public void StartMessage(Message msg)
        //    {
        //        throw new NotSupportedException("not implemented");
        //    }

        //    public void WriteArrayElement(Object value)
        //    {
        //        throw new NotSupportedException("not implemented");
        //    }

        //    #region TaggedDataOutput Members


        //    public void SetBuffer( Etch.Transport.FlexBuffer msgBuf )
        //    {
        //        // ignore
        //    }

        //    #endregion
        //}
    }
}