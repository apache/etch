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
using Org.Apache.Etch.Bindings.Csharp.Util;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Test1;

namespace etch.tests
{
    class FakeTest1 : Test1
    {
        public int? beets( E1? e ) 
	    {
		    
    		
		    switch (e)
		    {
                case E1.A:
                    return 5;
                case E1.B:
                    throw new Excp3();
                case E1.C:
                    throw new Excp4();
			    default: return null;
		    }
	    }

	    public void blow( String msg, int? code )  
	    {
		    throw new Excp1( msg, code );
	    }

	    public double? dist( S1 a, S1 b )
	    {
		    S1 c = new S1( a.x-b.x, a.y-b.y, a.z-b.z );
		    return Math.Sqrt( (double)(c.x*c.x + c.y*c.y + c.z*c.z) );
	    }

	    public int? incr( int? x )
	    {
		    return x+1;
	    }

	    public void nothing()
	    {
		    // nothing.
	    }

	    public int? sub( int? x, int? y )
	    {
		    return x-y;
	    }

	    public int? sum( int[] x )
	    {
		    int sum = 0;
		    foreach (int i in x)
			    sum += i;
		    return sum;
	    }

	    public int? trans( E1? e, int? x )
	    {
		    switch (e)
		    {
                case E1.A:
                    return x / 2;
                case E1.B:
                    return x * 2;
                case E1.C:
                    return x + 7;
			    default: return null;
		    }
	    }

	    public int[] fill( int? n, int? x )
	    {
		    int[] y = new int[(int)n];
		    for (int i = 0; i < n; i++)
			    y[i] = (int)x;
		    return y;
	    }

        public E1? p_E1( E1? a )
	    {
		    return a;
	    }

        public E1[] p_E1_array( E1[] a )
	    {
		    return a;
	    }

        public S1 p_S1( S1 a )
	    {
		    return a;
	    }

        public S1[] p_S1_array( S1[] a )
	    {
		    return a;
	    }

        public S2 p_S2( S2 a )
	    {
		    return a;
	    }

        public S2[] p_S2_array( S2[] a )
	    {
		    return a;
	    }

	    public bool? p_boolean( bool? a )
	    {
		    return a;
	    }

	    public bool[] p_boolean_array( bool[] a )
	    {
		    return a;
	    }

	    public sbyte? p_byte( sbyte? a )
	    {
		    return a;
	    }

	    public sbyte[] p_byte_array( sbyte[] a )
	    {
		    return a;
	    }

        public double? p_double( double? a )
	    {
		    return a;
	    }

	    public double[] p_double_array( double[] a )
	    {
		    return a;
	    }

	    public float? p_float( float? a )
	    {
		    return a;
	    }

	    public float[] p_float_array( float[] a )
	    {
		    return a;
	    }

	    public int? p_int( int? a )
	    {
		    return a;
	    }

	    public int[] p_int_array( int[] a )
	    {
		    return a;
	    }

	    public long? p_long( long? a )
	    {
		    return a;
	    }

	    public long[] p_long_array( long[] a )
	    {
		    return a;
	    }

	    public short? p_short( short? a )
	    {
		    return a;
	    }

	    public short[] p_short_array( short[] a )
	    {
		    return a;
	    }

	    public string p_string( string a )
	    {
		    return a;
	    }

	    public string[] p_string_array( string[] a )
	    {
		    return a;
	    }

	    public StrIntHashMap p_Blob( StrIntHashMap a )
	    {
		    return a;
	    }

	    public StrIntHashMap[] p_Blob_array( StrIntHashMap[] a )
	    {
		    return a;
	    }

	    public void alwaysFails()
	    {
		    // i'm never called.
		    throw new Exception( "alwaysFails" );
	    }

	    public int? alwaysWorks()
	    {
		    // i'm always called.
		    return 23;
	    }

	    public bool? isFalse()
	    {
		    return false;
	    }

	    public bool? isTrue()
	    {
		    return true;
	    }

	    public int? add( int? a, int? b )
	    {
		    return a + b;
	    }

	    public bool? canAdd( bool flag, int? x )
	    {
		    // TODO Auto-generated method stub
		    return null;
	    }

	    public bool? canM1()
	    {
		    // TODO Auto-generated method stub
		    return null;
	    }

	    public bool? canM10( double? x )
	    {
		    // TODO Auto-generated method stub
		    return null;
	    }

	    public bool? canM2( bool? x )
	    {
		    // TODO Auto-generated method stub
		    return null;
	    }

	    public bool? canM3( bool? x )
	    {
		    // TODO Auto-generated method stub
		    return null;
	    }

	    public bool? canM4( bool? x )
	    {
		    // TODO Auto-generated method stub
		    return null;
	    }

	    public bool? can_m7( int? x )
	    {
		    // TODO Auto-generated method stub
		    return null;
	    }
       

	    public void m1()
	    {
		    // TODO Auto-generated method stub
    		
	    }

	    public void m10()
	    {
		    // TODO Auto-generated method stub
    		
	    }

	    public void m2()
	    {
		    // TODO Auto-generated method stub
    		
	    }

	    public void m3()
	    {
		    // TODO Auto-generated method stub
    		
	    }

	    public void m4()
	    {
		    // TODO Auto-generated method stub
    		
	    }

	    public void m7()
	    {
		    // TODO Auto-generated method stub
    		
	    }

       

       

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m11()
         */
        public void m11()
        {
            // TODO Auto-generated method stub

        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m5()
         */
        public void m5()
        {
            // TODO Auto-generated method stub

        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m6()
         */
        public void m6()
        {
            // TODO Auto-generated method stub

        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m8()
         */
        public void m8()
        {
            // TODO Auto-generated method stub

        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m9()
         */
        public void m9()
        {
            // TODO Auto-generated method stub

        }

        public bool? can_m1()
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m2(bool? x)
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m3(bool? x)
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m4(bool? x)
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m11(java.lang.String)
         */
        public bool? can_m11( string x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m5(java.lang.Byte)
         */
        public bool? can_m5( sbyte? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m6(java.lang.Short)
         */
        public bool? can_m6( short? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m8(java.lang.Long)
         */
        public bool? can_m8( long? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m9(java.lang.Float)
         */
        public bool? can_m9( float? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m10( double? x)
        {
            return null;
        }


        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m12(java.lang.Integer)
         */
        public bool? can_m12( int? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m13(java.lang.Integer)
         */
        public bool? can_m13( int? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m14(java.lang.Integer)
         */
        public bool? can_m14( int? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#can_m15(etch.bindings.java.compiler.test.Test.E1)
         */
        public bool? can_m15( E1? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m12()
         */
        public void m12()
        {
            // TODO Auto-generated method stub

        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m13(java.lang.Integer)
         */
        public void m13( int? k )
        {
            // TODO Auto-generated method stub

        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m14(etch.bindings.java.compiler.test.Test.S1)
         */
        public void m14( S1 k )
        {
            // TODO Auto-generated method stub

        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#m15()
         */
        public void m15()
        {
            // TODO Auto-generated method stub

        }

        // fill numElements of array with value o
        public object[] fillObject( int? numElements, object o )
        {
            object[] y = new object[ (int) numElements ];
            for ( int i = 0; i < numElements; i++ )
                y[ i ] = o;
            return y;
        }

        public object p_object( object a ) 
        {
		    return a;
	    }

	    public object[] p_object_array( object[] a ) 
        {
		    return a;
	    }

	    public S3 p_object_struct( S3 a ) 
        {
		    return a;
	    }

	    public S4 p_object_struct_array( S4 a ) 
        {
		    return a;
	    }

	    public void throwExcp5( String msg, int? code, object value ) 
        {
		    throw new Excp5( msg, code, value );
	    }

	    public void throwExcp6(String msg, int? code, object[] value ) 
        {
		   throw new Excp6( msg, code, value );		
	    }
    }
}
