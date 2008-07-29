using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using Etch.Util;

namespace Etch.Compiler.Test
{
    class FakeTest : etch.bindings.csharp.compiler.test.Test
    {
        public int? beets( etch.bindings.csharp.compiler.test.ConstsTest.E1? e ) 
	    {
		    
    		
		    switch (e)
		    {
                case etch.bindings.csharp.compiler.test.ConstsTest.E1.A:
                    return 5;
                case etch.bindings.csharp.compiler.test.ConstsTest.E1.B:
                    throw new etch.bindings.csharp.compiler.test.ConstsTest.Excp3();
                case etch.bindings.csharp.compiler.test.ConstsTest.E1.C:
                    throw new etch.bindings.csharp.compiler.test.ConstsTest.Excp4();
			    default: return null;
		    }
	    }

	    public void blow( String msg, int? code )  
	    {
		    throw new etch.bindings.csharp.compiler.test.ConstsTest.Excp1( msg, code );
	    }

	    public double? dist( etch.bindings.csharp.compiler.test.ConstsTest.S1 a, etch.bindings.csharp.compiler.test.ConstsTest.S1 b )
	    {
		    etch.bindings.csharp.compiler.test.ConstsTest.S1 c = new etch.bindings.csharp.compiler.test.ConstsTest.S1( a.x-b.x, a.y-b.y, a.z-b.z );
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

	    public int? trans( etch.bindings.csharp.compiler.test.ConstsTest.E1? e, int? x )
	    {
		    switch (e)
		    {
                case etch.bindings.csharp.compiler.test.ConstsTest.E1.A:
                    return x / 2;
                case etch.bindings.csharp.compiler.test.ConstsTest.E1.B:
                    return x * 2;
                case etch.bindings.csharp.compiler.test.ConstsTest.E1.C:
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

        public etch.bindings.csharp.compiler.test.ConstsTest.E1? p_E1( etch.bindings.csharp.compiler.test.ConstsTest.E1? a )
	    {
		    return a;
	    }

        public etch.bindings.csharp.compiler.test.ConstsTest.E1[] p_E1_array( etch.bindings.csharp.compiler.test.ConstsTest.E1[] a )
	    {
		    return a;
	    }

        public etch.bindings.csharp.compiler.test.ConstsTest.S1 p_S1( etch.bindings.csharp.compiler.test.ConstsTest.S1 a )
	    {
		    return a;
	    }

        public etch.bindings.csharp.compiler.test.ConstsTest.S1[] p_S1_array( etch.bindings.csharp.compiler.test.ConstsTest.S1[] a )
	    {
		    return a;
	    }

        public etch.bindings.csharp.compiler.test.ConstsTest.S2 p_S2( etch.bindings.csharp.compiler.test.ConstsTest.S2 a )
	    {
		    return a;
	    }

        public etch.bindings.csharp.compiler.test.ConstsTest.S2[] p_S2_array( etch.bindings.csharp.compiler.test.ConstsTest.S2[] a )
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

	    public bool? canM7( int? x )
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

        public bool? canAdd( bool? flag, int? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m1()
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m10( double? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m2( bool? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m3( bool? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m4( bool? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        public bool? can_m7( int? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#canM11(java.lang.String)
         */
        public bool? canM11( string x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#canM5(java.lang.Byte)
         */
        public bool? canM5( sbyte? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#canM6(java.lang.Short)
         */
        public bool? canM6( short? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#canM8(java.lang.Long)
         */
        public bool? canM8( long? x )
        {
            // TODO Auto-generated method stub
            return null;
        }

        /* (non-Javadoc)
         * @see etch.bindings.java.compiler.test.Test#canM9(java.lang.Float)
         */
        public bool? canM9( float? x )
        {
            // TODO Auto-generated method stub
            return null;
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
        public bool? can_m15( etch.bindings.csharp.compiler.test.ConstsTest.E1? x )
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
        public void m14( etch.bindings.csharp.compiler.test.ConstsTest.S1 k )
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

	    public etch.bindings.csharp.compiler.test.ConstsTest.S3 p_object_struct( etch.bindings.csharp.compiler.test.ConstsTest.S3 a ) 
        {
		    return a;
	    }

	    public etch.bindings.csharp.compiler.test.ConstsTest.S4 p_object_struct_array( etch.bindings.csharp.compiler.test.ConstsTest.S4 a ) 
        {
		    return a;
	    }

	    public void throwExcp5( String msg, int? code, object value ) 
        {
		    throw new etch.bindings.csharp.compiler.test.ConstsTest.Excp5( msg, code, value );
	    }

	    public void throwExcp6(String msg, int? code, object[] value ) 
        {
		   throw new etch.bindings.csharp.compiler.test.ConstsTest.Excp6( msg, code, value );		
	    }
    }
}
