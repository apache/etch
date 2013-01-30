// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.IO;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    [TestFixture]
    public class TestFlexBuffer
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write("TestFlexBuffer");
        }


        private FlexBuffer buf = new FlexBuffer();

        // General test function
        private void CheckBuf( int length, int index, int avail )
        {
            Assert.AreEqual( length, buf.Length() );
            Assert.AreEqual( index, buf.Index() );
            Assert.AreEqual( avail, buf.Avail() );
        }

        #region Basic Tests

        [Test]
        public void Initial1()
        {
            buf = new FlexBuffer();
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        public void Initial2()
        {
            buf = new FlexBuffer( new byte[ 5 ] );
            CheckBuf( 5, 0, 5 );
        }

        [Test]
        public void Initial3()
        {
            buf = new FlexBuffer( new byte[ 5 ], 2 );
            CheckBuf( 2, 0, 2 );
        }

        [Test]
        public void Initial4()
        {
            buf = new FlexBuffer( new byte[ 5 ], 1, 3 );
            CheckBuf( 4, 1, 3 );
        }

        [Test]
        public void SetLength0()
        {
            buf.SetLength( 0 );
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        public void SetLength1()
        {
            buf.SetLength( 1 );
            CheckBuf( 1, 0, 1 );
        }

        [Test]
        public void SetLength2()
        {
            buf.SetLength( 2 );
            CheckBuf( 2, 0, 2 );
        }

        [Test]
        public void SetLength3()
        {
            buf.SetLength( 2 );
            buf.SetLength( 0 );
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        [ExpectedException ( typeof ( ArgumentOutOfRangeException ) )]
        public void SetLength4()
        {
            buf.SetLength( -1 );
        }

        [Test]
        public void SetIndex0()
        {
            buf.Reset();
            buf.SetIndex( 0 );
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        [ExpectedException ( typeof ( ArgumentOutOfRangeException ) )]
        public void SetIndex1()
        {
            buf.Reset();
            buf.SetIndex( 1 );
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void SetIndex2()
        {
            buf.SetIndex( -1 );
        }
        
        [Test]
        public void SetIndex3()
        {
            buf.SetLength( 5 );
            buf.SetIndex( 1 );
            CheckBuf( 5, 1, 4 );
        }

        [Test]
        public void SetIndex4()
        {
            buf.SetLength( 5 );
            buf.SetIndex( 4 );
            CheckBuf( 5, 4, 1 );
        }

        [Test]
        public void SetIndex5()
        {
            buf.SetLength( 5 );
            buf.SetIndex( 5 );
            CheckBuf( 5, 5, 0 );
        }

        [Test]
        public void SetIndex6()
        {
            buf.SetLength( 5 );
            buf.SetIndex( 5 );
            buf.SetLength( 0 );
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        public void SetIndex7()
        {
            buf.SetLength( 5 );
            buf.SetIndex( 5 );
            buf.SetLength( 2 );
            CheckBuf( 2, 2, 0 );
        }

        [Test]
        public void Reset()
        {
            buf.SetLength( 2 );
            buf.SetIndex( 1 );
            buf.Reset();
            CheckBuf( 0, 0, 0 );
        }
        #endregion Basic Tests

        #region Cases for put ( int )
        // put once or twice (the put 0 case has already been tested).

        [Test]
        public void Put1a()
        {
            buf.Reset();
            buf.Put( 1 );
            CheckBuf( 1, 1, 0 );
        }
        
        [Test]
        public void Put1b()
        {
            buf.Reset();
            buf.Put( 1 );
            buf.Put( 2 );
            CheckBuf( 2, 2, 0 );
        }

        #endregion Cases for put ( int )

        #region Cases for put ( byte[] buf )
        // buf.length = null, 0, 1, 2

        private void TestPutBytes( int bufLen )
        {
            buf.Reset();
            buf.Put( 1 );
            buf.Put( bufLen >= 0 ? new byte[ bufLen ] : null );
            CheckBuf( bufLen + 1, bufLen + 1, 0 );
        }

        [Test]
        [ExpectedException( typeof ( NullReferenceException ) )]
        public void Put2a()
        {
            TestPutBytes( -1 );
        }

        [Test]
        public void Put2b()
        {
            TestPutBytes( 0 );
        }

        [Test]
        public void Put2c()
        {
            TestPutBytes( 1 );
        }

        [Test]
        public void Put2d()
        {
            TestPutBytes( 2 );
        }

        #endregion Cases for put ( byte[] buf )

        #region Cases for put( byte[] buf, int off, int len )
        // buf.length, off, len // failure reason
        // --- group 3:
        // null, 0, 0 // fail (buf is null)
        // null, 0, 1 // fail (buf is null)
        // null, 1, 0 // fail (buf is null)
        // --- group 4:
        // 0, 0, 0
        // 0, 0, 1 // fail (off+len > buf.length)
        // 0, 1, 0 // fail (off > buf.length)
        // --- group 5:
        // 1, 0, 0
        // 1, 0, 1
        // 1, 0, 2 // fail (off+len > buf.length)
        // 1, 1, 0
        // 1, 1, 1 // fail (off+len > buf.length)
        // 1, 1, 2 // fail (off+len > buf.length)
        // 1, 2, 0 // fail (off > buf.length)
        // 1, 2, 1 // fail (off > buf.length)
        // 1, 2, 2 // fail (off > buf.length)
        // --- group 6:
        // 2, 0, 0
        // 2, 0, 1
        // 2, 0, 2
        // 2, 1, 0
        // 2, 1, 1
        // 2, 1, 2 // fail (off+len > buf.length)
        // 2, 2, 0	
        // 2, 2, 1 // fail (off+len > buf.length)
        // 2, 2, 2 // fail (off+len > buf.length)

        private void TestPutBytesOffLen( int bufLen, int offset, int length )
        {
            buf.Reset();
            buf.Put( 1 );
            buf.Put( bufLen >= 0 ? new byte[ bufLen ] : null, offset, length );
            CheckBuf( length + 1, length + 1, 0 );
        }

        [Test]
        [ExpectedException( typeof( NullReferenceException ) )]
        public void Put3a()
        {
            TestPutBytesOffLen( -1, 0, 0 ); // fail (buf is null)
        }
        
        [Test]
        [ExpectedException( typeof( NullReferenceException ) )]
        public void Put3b()
        {
            TestPutBytesOffLen( -1, 0, 1 ); // fail (buf is null)
        }

        [Test]
        [ExpectedException( typeof( NullReferenceException ) )]
        public void Put3c()
        {
            TestPutBytesOffLen( -1, 1, 0 ); // fail (buf is null)
        }

        [Test]
        public void Put4a()
        {
            TestPutBytesOffLen( 0, 0, 0 ); 
        }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void Put4b()
        {
            TestPutBytesOffLen( 0, 0, 1 ); // fail (off+len > buf.length)
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put4c()
        {
            TestPutBytesOffLen( 0, 1, 0 ); // fail (off > buf.length)
        }

        [Test]
        public void Put5a()
        {
            TestPutBytesOffLen( 1, 0, 0 );
        }
        
        [Test]
        public void Put5b()
        {
            TestPutBytesOffLen( 1, 0, 1 );
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put5c()
        {
            TestPutBytesOffLen( 1, 0, 2 ); // fail (off+len > buf.length)
        }

        [Test]
        public void Put5d()
        {
            TestPutBytesOffLen( 1, 1, 0 ); 
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put5e()
        {
            TestPutBytesOffLen( 1, 1, 1 ); // fail (off+len > buf.length)
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put5f()
        {
            TestPutBytesOffLen( 1, 1, 2 ); // fail (off+len > buf.length)
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put5g()
        {
            TestPutBytesOffLen( 1, 2, 0 ); // fail (off > buf.length)
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put5h()
        {
            TestPutBytesOffLen( 1, 2, 1 ); // fail (off > buf.length)
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put5i()
        {
            TestPutBytesOffLen( 1, 2, 2 ); // fail (off > buf.length)
        }

        [Test]
        public void Put6a()
        {
            TestPutBytesOffLen( 2, 2, 0 ); 
        }

        [Test]
        public void Put6b()
        {
            TestPutBytesOffLen( 2, 0, 1 ); 
        }

        [Test]
        public void Put6c()
        {
            TestPutBytesOffLen( 2, 0, 2 );
        }

        [Test]
        public void Put6d()
        {
            TestPutBytesOffLen( 2, 1, 0 );
        }

        [Test]
        public void Put6e()
        {
            TestPutBytesOffLen( 2, 1, 1 );
        }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void Put6f()
        {
            TestPutBytesOffLen( 2, 1, 2 ); // fail (off+len > buf.length)
        }

        [Test]
        public void Put6g()
        {
            TestPutBytesOffLen( 2, 2, 0 ); 
        }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void Put6h()
        {
            TestPutBytesOffLen( 2, 2, 1 ); // fail (off+len > buf.length)
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void Put6i()
        {
            TestPutBytesOffLen( 2, 2, 2 ); // fail (off+len > buf.length)
        }

        #endregion Cases for put( byte[] buf, int off, int len )

        #region Cases for Put( int ), Get() 
        // put 0, get 0. // already tested above
        // put 0, get 1. // fails with EOFException
        // put 1, get 0.
        // put 1, get 1.
        // put 1, get 2. // fails with EOFException
        // put 2, get 0.
        // put 2, get 1.
        // put 2, get 2.
        // put 2, get 3. // fails with EOFException
        // put 10000, get 10000.

        // just like above, putting byte array instead
        // just like above, getting byte array instead.
        // just like above, putting and getting byte array.

        private void TestPutGet( int nPuts, int nGets )
        {
            buf.Reset();

            for ( int i = 0; i < nPuts; i++ )
                buf.Put( i );

            buf.SetIndex( 0 );

            for ( int i =0; i < nGets; i++ )
                Assert.AreEqual( ( byte ) i, ( byte ) buf.Get() );

            CheckBuf( nPuts, nGets, nPuts - nGets );
        }

        private void TestPutBytesGet( int nPuts, int nGets )
        {
            buf.Reset();

            byte[] buffer = new byte[ nPuts ];
            for ( int i = 0; i < nPuts; i++ )
                buffer[ i ] = ( byte ) i;

            buf.Put( buffer );
            buf.SetIndex( 0 );

            for ( int i =0; i<nGets; i++ )
                Assert.AreEqual( ( byte ) i, ( byte ) buf.Get() );

            CheckBuf( nPuts, nGets, nPuts-nGets );
        }

        private void TestPutGetBytes( int nPuts, int nGets )
        {
            buf.Reset();

            for ( int i = 0; i < nPuts; i++ )
                buf.Put( i );

            buf.SetIndex( 0 );

            byte[] buffer = new byte[ nGets ];
            int n = buf.Get( buffer );
            Assert.AreEqual( Math.Min( nPuts, nGets ), n );

            for ( int i =0; i < n; i++ )
                Assert.AreEqual( ( byte ) i, buffer[ i ] );

            CheckBuf( nPuts, n, nPuts-n );
        }

        private void TestPutBytesGetBytes( int nPuts, int nGets )
        {
            buf.Reset();

            byte[] buffer = new byte[ nPuts ];
            for ( int i = 0; i < nPuts; i++ )
                buffer[ i ] = ( byte ) i;

            buf.Put( buffer );
            buf.SetIndex( 0 );

            buffer = new byte[ nGets ];
            int n = buf.Get( buffer );
            Assert.AreEqual( Math.Min( nPuts, nGets ), n );

            for ( int i =0; i < n; i++ )
                Assert.AreEqual( ( byte ) i, buffer[ i ] );

            CheckBuf( nPuts, n, nPuts-n );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get1a()
        {
            TestPutGet( 0, 1 );
        }

        [Test]
        public void Get1b()
        {
            TestPutGet( 1, 0 );
        }

        [Test]
        public void Get1c()
        {
            TestPutGet( 1, 1 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get1d()
        {
            TestPutGet( 1, 2 );
        }

        [Test]
        public void Get1e()
        {
            TestPutGet( 2, 0 );
        }

        [Test]
        public void Get1f()
        {
            TestPutGet( 2, 1 );
        }

        [Test]
        public void Get1g()
        {
            TestPutGet( 2, 2 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get1h()
        {
            TestPutGet( 2, 3 );
        }

        [Test]
        public void Get1i()
        {
            TestPutGet( 10000, 9995 );
        }

        [Test]
        public void Get1j()
        {
            TestPutGet( 10000, 10000 );
        }

        [Test]
        public void Get1k()
        {
            TestPutGet( 100000, 100000 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get2a()
        {
            TestPutBytesGet( 0, 1 );
        }

        [Test]
        public void Get2b()
        {
            TestPutBytesGet( 1, 0 );
        }

        [Test]
        public void Get2c()
        {
            TestPutBytesGet( 1, 1 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get2d()
        {
            TestPutBytesGet( 1, 2 );
        }

        [Test]
        public void Get2e()
        {
            TestPutBytesGet( 2, 0 );
        }

        [Test]
        public void Get2f()
        {
            TestPutBytesGet( 2, 1 );
        }

        [Test]
        public void Get2g()
        {
            TestPutBytesGet( 2, 2 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get2h()
        {
            TestPutBytesGet( 2, 3 );
        }

        [Test]
        public void Get2i()
        {
            TestPutBytesGet( 10000, 9995 );
        }

        [Test]
        public void Get2j()
        {
            TestPutBytesGet( 10000, 10000 );
        }

        [Test]
        public void Get2k()
        {
            TestPutBytesGet( 100000, 100000 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get3a()
        {
            TestPutGetBytes( 0, 1 );
        }

        [Test]
        public void Get3b()
        {
            TestPutGetBytes( 1, 0 );
        }

        [Test]
        public void Get3c()
        {
            TestPutGetBytes( 1, 1 );
        }

        [Test]
        public void Get3d()
        {
            TestPutGetBytes( 1, 2 );
        }

        [Test]
        public void Get3e()
        {
            TestPutGetBytes( 2, 0 );
        }

        [Test]
        public void Get3f()
        {
            TestPutGetBytes( 2, 1 );
        }

        [Test]
        public void Get3g()
        {
            TestPutGetBytes( 2, 2 );
        }

        [Test]
        public void Get3h()
        {
            TestPutGetBytes( 2, 3 );
        }

        [Test]
        public void Get3i()
        {
            TestPutGetBytes( 10000, 9995 );
        }

        [Test]
        public void Get3j()
        {
            TestPutGetBytes( 10000, 10000 );
        }

        [Test]
        public void Get3k()
        {
            TestPutGetBytes( 100000, 100000 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Get4a()
        {
            TestPutBytesGetBytes( 0, 1 );
        }

        [Test]
        public void Get4b()
        {
            TestPutBytesGetBytes( 1, 0 );
        }

        [Test]
        public void Get4c()
        {
            TestPutBytesGetBytes( 1, 1 );
        }

        [Test]
        public void Get4d()
        {
            TestPutBytesGetBytes( 1, 2 );
        }

        [Test]
        public void Get4e()
        {
            TestPutBytesGetBytes( 2, 0 );
        }

        [Test]
        public void Get4f()
        {
            TestPutBytesGetBytes( 2, 1 );
        }

        [Test]
        public void Get4g()
        {
            TestPutBytesGetBytes( 2, 2 );
        }

        [Test]
        public void Get4h()
        {
            TestPutBytesGetBytes( 2, 3 );
        }

        [Test]
        public void Get4i()
        {
            TestPutBytesGetBytes( 10000, 9995 );
        }

        [Test]
        public void Get4j()
        {
            TestPutBytesGetBytes( 10000, 10000 );
        }

        [Test]
        public void Get4k()
        {
            TestPutBytesGetBytes( 100000, 100000 );
        }

        [Test]
        public void Get5a()
        {
            buf = new FlexBuffer( new byte[] { 1, 2, 3 } );
            Assert.AreEqual( 1, buf.Get() );
            Assert.AreEqual( 2, buf.Get() );
            Assert.AreEqual( 3, buf.Get() );
            CheckBuf( 3, 3, 0 );
        }

        [Test]
        public void Get5b()
        {
            buf = new FlexBuffer( new byte[] { 1, 2, 3 }, 2 );
            Assert.AreEqual( 1, buf.Get() );
            Assert.AreEqual( 2, buf.Get() );
            CheckBuf( 2, 2, 0 );
        }

        [Test]
        public void Get5c()
        {
            buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5 }, 1, 2 );
            Assert.AreEqual( 2, buf.Get() );
            Assert.AreEqual( 3, buf.Get() );
            CheckBuf( 3, 3, 0 );
        }

        #endregion

        #region Other tests

        [Test]
        public void Get6()
        {
            buf = new FlexBuffer( new byte[] { 5, 6, 7, 8 } );
            byte[] buffer = new byte[] { 11, 12, 13, 14, 15 };
            buf.Get( buffer, 1, 3 );
            Assert.AreEqual( ( byte ) 11, buffer[ 0 ] );
            Assert.AreEqual( ( byte ) 5, buffer[ 1 ] );
            Assert.AreEqual( ( byte ) 6, buffer[ 2 ] );
            Assert.AreEqual( ( byte ) 7, buffer[ 3 ] );
            Assert.AreEqual( ( byte ) 15, buffer[ 4 ] );
        }

        [Test]
        public void Compact()
        {
            buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5 }, 1, 3 );
            buf.Compact();
            CheckBuf( 3, 0, 3 );
            Assert.AreEqual( 2, buf.Get() );
            Assert.AreEqual( 3, buf.Get() );
            Assert.AreEqual( 4, buf.Get() );
        }

        #endregion

        #region Test data gets ( int, byte, short, etc )

        [Test]
        public void GetByte1()
        {
            buf = new FlexBuffer( new byte[] { 127, 1, 0, unchecked( (byte)-1), unchecked( (byte)-128) } );
            Assert.AreEqual( ( byte ) 127, buf.GetByte() );
            Assert.AreEqual( ( byte ) 1, buf.GetByte() );
            Assert.AreEqual( ( byte ) 0, buf.GetByte() );
            Assert.AreEqual( -1, buf.GetByte() );
            Assert.AreEqual( -128 , buf.GetByte() );
            CheckBuf( 5, 5, 0 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetByte2()
        {
            buf.GetByte();
        }

        [Test]
        public void GetShort1()
        {
            buf = new FlexBuffer( new byte[] { 2, 1, unchecked( ( byte ) -2 ), unchecked( ( byte ) -1 ) } );
            if ( FlexBuffer.littleEndian )
            {
                Assert.AreEqual( ( short ) 0x0102, buf.GetShort() );
                Assert.AreEqual( unchecked( ( short ) 0xfffe ), buf.GetShort() );
            }
            else
            {
                Assert.AreEqual( ( short ) 0x0201, buf.GetShort() );
                Assert.AreEqual( unchecked( ( short ) 0xfeff ), buf.GetShort() );
            }
            CheckBuf( 4, 4, 0 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetShort2()
        {
            buf.GetShort();
        }

        [Test]
        public void GetInt1()
        {
            buf = new FlexBuffer( new byte[] { 4, 3, 2, 1, unchecked( ( byte ) -4 ), unchecked( ( byte ) -3 ), 
                unchecked( ( byte ) -2 ), unchecked( ( byte ) -1 ) } );
            if ( FlexBuffer.littleEndian )
            {
                Assert.AreEqual( 0x01020304, buf.GetInt() );
                Assert.AreEqual( unchecked( ( int ) 0xfffefdfc ), buf.GetInt() );
            }
            else
            {
                Assert.AreEqual( 0x04030201, buf.GetInt() );
                Assert.AreEqual( unchecked( ( int ) 0xfcfdfeff ), buf.GetInt() );
            }
            CheckBuf( 8, 8, 0 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetInt2()
        {
            buf.GetInt();
        }

        [Test]
        public void GetLong1()
        {
            buf = new FlexBuffer( new byte[] { 8, 7, 6, 5, 4, 3, 2, 1, unchecked( ( byte ) -8 ), unchecked( ( byte ) -7 ), 
                unchecked( ( byte ) -6 ), unchecked( ( byte ) -5 ), unchecked( ( byte ) -4 ), unchecked( ( byte ) -3 ), 
                unchecked( ( byte ) -2 ), unchecked( ( byte ) -1 ) } );
            if ( FlexBuffer.littleEndian )
            {
                Assert.AreEqual( 0x0102030405060708L, buf.GetLong() );
                Assert.AreEqual( unchecked( ( long ) 0xfffefdfcfbfaf9f8L ), buf.GetLong() );
            }
            else
            {
                Assert.AreEqual( 0x0807060504030201L, buf.GetLong() );
                Assert.AreEqual( unchecked( ( long ) 0xf8f9fafbfcfdfeffL ), buf.GetLong() );
            }

            CheckBuf( 16, 16, 0 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetLong2()
        {
            buf.GetLong();
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetFloat1()
        {
            buf = new FlexBuffer( new byte[] { } );
            Assert.AreEqual( 123456e-10f, buf.GetFloat() );
            Assert.AreEqual( 123456e10f, buf.GetFloat() );
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetFloat2()
        {
            buf.GetFloat();
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetDouble1()
        {
            buf = new FlexBuffer( new byte[] { } );
            Assert.AreEqual( 123456789123456789e-110, buf.GetDouble() );
            Assert.AreEqual( 123456789123456789e110, buf.GetDouble() );
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetDouble2()
        {
            buf.GetDouble();
        }

        [Test]
        public void GetFully1()
        {
            buf = new FlexBuffer( new byte[] { 1, 2, 3, 4, 5, 6 } );
            byte[] b = new byte[ 3 ];
            buf.GetFully( b );
            Assert.AreEqual( ( byte ) 1, b[ 0 ] );
            Assert.AreEqual( ( byte ) 2, b[ 1 ] );
            Assert.AreEqual( ( byte ) 3, b[ 2 ] );
            buf.GetFully( b );
            Assert.AreEqual( ( byte ) 4, b[ 0 ] );
            Assert.AreEqual( ( byte ) 5, b[ 1 ] );
            Assert.AreEqual( ( byte ) 6, b[ 2 ] );
            CheckBuf( 6, 6, 0 );
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void GetFully2()
        {
            buf = new FlexBuffer( new byte[] { 1, 2 } );
            byte[] b = new byte[ 3 ];
            buf.GetFully( b ); 
        }

        #endregion

        #region Cases for Flexbuffer Put( FlexBuffer, ... )

        // put, put FlexBuffer with valid buffer
        // put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        public void PutFlexBuffer1()
        {
            FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
            buf = new FlexBuffer( new byte[] { } );
            buf.Put( buf0 );
            CheckBuf( buf0.Length(), buf0.Length(), 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutFlexBuffer2()
        {
            FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
            int max = 4*1024*1024;
            buf = new FlexBuffer( new byte[ max ] );
            buf.SetIndex( max );
            buf.Put( buf0 );
            CheckBuf( max+buf0.Length(), max+buf0.Length(), 0 );
        }

        // put, put FlexBuffer with length larger than available bytes, fails with IllegalArgumentException
        // put, put FlexBuffer with valid length value
        // put, put Flexbuffer to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void PutFlexBuffer3()
        {
            FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
            buf = new FlexBuffer( new byte[] { } );
            buf.SetIndex( 0 );
            buf.Put( buf0, 3 );
            CheckBuf( 3, 3, 0 );
        }

        [Test]
        public void PutFlexBuffer4()
        {
            FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
            buf = new FlexBuffer( new byte[] { } );
            buf.Put( buf0, 1 );
            CheckBuf( 1, 1, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutFlexBuffer5() 
	    {
		    FlexBuffer buf0 = new FlexBuffer( new byte[] { 1, 2 } );
		    int max = 4*1024*1024;
		    buf = new FlexBuffer( new byte[max] );
            buf.SetIndex( max );
            buf.Put( buf0, 1 );
		    CheckBuf( max+1, max+1, 0 );
	    }

        #endregion

        #region Cases for void putByte( byte )
        // putByte, put byte value and expect getByte to return matched value.
        // putByte to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        public void PutByte1()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.PutByte( ( byte ) 0x01 );
            CheckBuf( 1, 1, 0 );
            buf.SetIndex( 0 );
            Assert.AreEqual( ( byte ) 0x01, buf.GetByte() );
            CheckBuf( 1, 1, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutByte2()
        {
            int max = 4*1024*1024;
            buf = new FlexBuffer( new byte[ max ] );
            buf.SetIndex( max );
            buf.PutByte( ( byte ) 0x01 );
            CheckBuf( max+1, max+1, 0 );
        }

        #endregion

        #region Cases for void PutDouble( Double )
        // putDouble, put Double and expect getDouble to return matched value.
        // putDouble to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        public void PutDouble1()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.PutDouble( Double.MaxValue );
            CheckBuf( 8, 8, 0 );
            buf.SetIndex( 0 );
            Assert.AreEqual( Double.MaxValue, buf.GetDouble() );
            CheckBuf( 8, 8, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutDouble2()
        {
            int max = 4*1024*1024;
            buf = new FlexBuffer( new byte[ max ] );
            buf.SetIndex( max );
            buf.PutDouble( Double.MinValue );
            CheckBuf( max+8, max+8, 0 );
        }

        #endregion

        #region Cases for void putFloat( float )
        // putFloat, put Float and expect getFloat to return matched value.
        // putFloat to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        public void PutFloat1()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.PutFloat( float.MaxValue );
            CheckBuf( 4, 4, 0 );
            buf.SetIndex( 0 );
            Assert.AreEqual( float.MaxValue, buf.GetFloat() );
            CheckBuf( 4, 4, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutFloat2()
        {
            int max = 4*1024*1024;
            buf = new FlexBuffer( new byte[ max ] );
            buf.SetIndex( max );
            buf.PutFloat( float.MinValue );
            CheckBuf( max+4, max+4, 0 );
        }

        #endregion

        #region Cases for void PutInt( int )
        // putInt, put Int and expect getInt to return matched value.
        // putInt to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        public void PutInt1()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.PutInt( int.MaxValue );
            CheckBuf( 4, 4, 0 );
            buf.SetIndex( 0 );
            Assert.AreEqual( int.MaxValue, buf.GetInt() );
            CheckBuf( 4, 4, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutInt2()
        {
            int max = 4*1024*1024;
            buf = new FlexBuffer( new byte[ max ] );
            buf.SetIndex( max );
            buf.PutInt( int.MinValue );
            CheckBuf( max+4, max+4, 0 );
        }

        #endregion

        #region Cases for void putLong( long )
        // putLong, put Long and expect getLong to return matched value.
        // putShort to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        public void PutLong0()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.PutLong( long.MaxValue );
            CheckBuf( 8, 8, 0 );
            buf.SetIndex( 0 );
            Assert.AreEqual( long.MaxValue, buf.GetLong() );
            CheckBuf( 8, 8, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutLong1()
        {
            int max = 4*1024*1024;
            buf = new FlexBuffer( new byte[ max ] );
            buf.SetIndex( max );
            buf.PutLong( long.MinValue );
            CheckBuf( max+8, max+8, 0 );
        }

        #endregion

        #region Cases for void PutShort( short )
        // putShort, put short and expect getShort to return matched value.
        // putShort to a Flexbuffer approaching max length, fails with IOException.

        [Test]
        public void PutShort1()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.PutShort( short.MaxValue );
            CheckBuf( 2, 2, 0 );
            buf.SetIndex( 0 );
            Assert.AreEqual( short.MaxValue, buf.GetShort() );
            CheckBuf( 2, 2, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void PutShort2()
        {
            int max = 4*1024*1024;
            buf = new FlexBuffer( new byte[ max ] );
            buf.SetIndex( max );
            buf.PutShort( ( short ) 1 );
            CheckBuf( max+2, max+2, 0 );
        }

        #endregion

        #region Cases for FlexBuffer Skip( int, bool )
        // skip length < 0, put = false.  				// fails with illegal argument
        // skip length > buffer length, put = false.  	// fails with EOFException
        // skip length = 0, put = false.
        // skip length = buffer length, put = false.
        // skip length < buffer length, put = false.
        // skip length > buffer length, put = true. 
        // skip length < buffer length, put = true.	
        // skip length = max buffer length, put = true.	// fails with IOException


        [Test]
        [ExpectedException( typeof( ArgumentException ) )]
        public void Skip1()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.Skip( -1, false );		
        }

        [Test]
        [ExpectedException( typeof( EndOfStreamException ) )]
        public void Skip2()
        {
            buf = new FlexBuffer( new byte[] { 1, 2 } );
            buf.Skip( 3, false );
        }

        [Test]
        public void Skip3()
        {
            buf = new FlexBuffer( new byte[] { } );
            buf.Skip( 0, false );
            CheckBuf( 0, 0, 0 );
        }

        [Test]
        public void Skip4()
        {
            buf = new FlexBuffer( new byte[] { 1, 2 } );
            buf.Skip( 2, false );
            CheckBuf( 2, 2, 0 );
        }

        [Test]
        public void Skip5()
        {
            buf = new FlexBuffer( new byte[] { 1, 2 } );
            buf.Skip( 1, false );
            CheckBuf( 2, 1, 1 );
        }

        [Test]
        public void Skip6()
        {
            buf = new FlexBuffer( new byte[] { 1, 2 } );
            buf.Skip( 1, true );
            CheckBuf( 2, 1, 1 );
        }

        [Test]
        public void Skip7()
        {
            buf = new FlexBuffer( new byte[] { 1, 2 } );
            buf.Skip( 5, true );
            CheckBuf( 5, 5, 0 );
        }

        [Test]
        [ExpectedException( typeof( IOException ) )]
        public void Skip8()
        {
            buf = new FlexBuffer( new byte[] { 1, 2 } );

            int max = 4*1024*1024;
            buf.Skip( max+1, true );
            CheckBuf( max+1, max+1, 0 );
        }

        #endregion

    }
}
