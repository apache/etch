///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 28, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using System.Text;
using System.Collections.Generic;
using Etch.Msg;

namespace Etch.Support
{

/// <summary>Default implementation of ValueFactory which provides some dynamic type and field support, as well 
/// as standard value conversions and import and export.</summary>
public abstract class DefaultValueFactory : ValueFactory
{
	/// <summary>Constructs the DefaultValueFactory with up to 100 automatic type and field declarations.</summary>
    public DefaultValueFactory()
    {

    }
	
    ///// <summary>Constructs the DefaultValueFactory.</summary>
    ///// <param name="maxAutoCount">maxAutoCount the maximum number of automatic type and field declarations 
    ///// (counted separately).</param>
    // public DefaultValueFactory( int maxAutoCount )
    // {
    //     types = new TypeMap( maxAutoCount );

    //     fields = new FieldMap( maxAutoCount );

    //     { AddType( _mt__Etch_RuntimeException ); }
    //     { AddType( _mt__Etch_AuthException ); }
    //     { AddType( _mt__Etch_Date ); }
    //     { AddType( _mt__exception ); }

    //     { AddField( _mf_msg ); }
    //     { AddField( _mf_ticks ); }
    //     { AddField( _mf__messageId ); }
    //     { AddField( _mf__inReplyTo ); }
    //     { AddField( _mf_result );  }

    // }

    public static void Init( TypeMap types, FieldMap fields )
    {
        types.Add( _mt__Etch_RuntimeException );
        types.Add( _mt__Etch_AuthException );
        types.Add( _mt__exception );

        fields.Add( _mf_msg );
        fields.Add( _mf__messageId );
        fields.Add( _mf__inReplyTo );
        fields.Add( _mf_result );
    }

    /// <summary>The type of the standard unchecked exception.</summary>	
	public readonly static XType _mt__Etch_RuntimeException = new XType( "_Etch_RuntimeException" );

    /// <summary>
    /// The type of the standard auth exception.
    /// </summary>
    public readonly static XType _mt__Etch_AuthException = new XType( "_Etch_AuthException" );

	/// <summary>The type of the response to a oneway message which throws an exception.</summary>
    public readonly static XType _mt__exception = new XType( "_exception" );
	
	/// <summary>The msg field of the standard unchecked exception.</summary>
    public readonly static Field _mf_msg = new Field( "msg" );
	
    /// <summary>The well-known _messageId field.</summary>
    public readonly static Field _mf__messageId = new Field( "_messageId" );
	
	/// <summary>The well-known _inReplyTo field. </summary>
    public readonly static Field _mf__inReplyTo = new Field( "_inReplyTo" );

    /// <summary> The well-known result field </summary>
    public readonly static Field _mf_result = new Field( "result" );

    /// <summary>
    /// Used to initialize static members
    /// </summary>
    static DefaultValueFactory()
    {
        _mt__Etch_RuntimeException.PutValidator( _mf_msg, Validator_string.Get( 0 ) );
        
        _mt__Etch_AuthException.PutValidator( _mf_msg, Validator_string.Get( 0 ) );
    
        _mt__exception.PutValidator( _mf_result, Validator_RuntimeException.Get() );
        _mt__exception.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );
        _mt__exception.PutValidator( _mf__inReplyTo, Validator_long.Get( 0 ) );
    }
 
	//////////
	// TYPE //
	//////////
	
    //public XType AddType( XType type )
    //{
    //    return types.Add( type );
    //}
	
    //public XType GetType( int id ) 
    //{
    //    // check the well-known types
		
    //    switch (id)
    //    {
    //        case -284614264:
    //            return _mt__Etch_RuntimeException;
    //        case -1746727050:
    //            return _mt__Etch_AuthException;
    //        case -1201742561: return _mt__Etch_Date;
    //        case -1522717131: return _mt__exception;
    //    }
    //    // check the dynamic types
		
    //    return types.Get( id );
    //}

    //public XType GetType( String name )
    //{
    //    return types.Get( name );
    //}
	
    //public ICollection<XType> GetTypes()
    //{
    //    return types.Values();
    //}
	
    //private  IdNameMap<XType> types;

	///////////
	// FIELD //
	///////////
	
    //public Field AddField( Field mf )
    //{
    //    return fields.Add( mf );
    //}

    //public Field GetField( int id )
    //{
    //    // check the well-known fields
		
    //    switch (id)
    //    {
    //        case 769750364: return _mf_msg;
    //        case 543806897: return _mf_ticks;
    //        case 1661383784: return _mf__messageId;
    //        case -307705434: return _mf__inReplyTo;
    //        case -2130379326: return _mf_result;
    //    }
		
    //    // check the dynamic fields
		
    //    return fields.Get( id );
    //}

    //public Field GetField( String name )
    //{
    //    return fields.Get(name);
    //}
	
    //public ICollection<Field> GetFields()
    //{
    //    return fields.Values();
    //}
	
    //private  IdNameMap<Field> fields;

	/////////////////////
	// STRING ENCODING //
	/////////////////////
	
	public Encoding GetStringEncoding()
	{
        return Encoding.UTF8;
		
	}

	////////////////
	// MESSAGE ID //
	////////////////

	public long? GetMessageId( Message msg )
	{
		return (long?)( msg.Get( _mf__messageId ) );
	}

	public void SetMessageId( Message msg, long? msgid )
	{
		if (msgid != null)
			msg[_mf__messageId] = msgid ; 
		else
			msg.Remove( _mf__messageId ); 
	}

	/////////////////
	// IN REPLY TO //
	/////////////////

	public long? GetInReplyTo( Message msg )
	{
        
		return (long?)( msg.Get( _mf__inReplyTo ) ); 
	}

	public void SetInReplyTo( Message msg, long? msgid )
	{
		if (msgid != null)
			msg[_mf__inReplyTo] = msgid ;
		else
			msg.Remove( _mf__inReplyTo );
	}

	//////////////////////
	// VALUE CONVERSION //
	//////////////////////

	public virtual StructValue ExportCustomValue( Object value )
	{

        Type t = value.GetType();

        if ( t == typeof( _Etch_AuthException ) )
        {
            StructValue sv = new StructValue( _mt__Etch_AuthException );
            _Etch_AuthException v = ( _Etch_AuthException ) value;
            sv[ _mf_msg ] = v.ToString();
            return sv;
        }

		// catch any exception which wasn't otherwise
		// handled and pass it through.
		if (value is Exception)
		{
			StructValue sv = new StructValue( _mt__Etch_RuntimeException );
			Exception v = (Exception) value;
			sv[_mf_msg] = v.ToString() ; 
			return sv;
		}



        return null;
	}

	public virtual Object ImportCustomValue( StructValue sv )
	{
		XType type = sv.GetXType;

        if ( type == _mt__Etch_RuntimeException )
            return new _Etch_RuntimeException( ( String ) sv.Get( _mf_msg ) );

        if ( type == _mt__Etch_AuthException )
            return new _Etch_AuthException( ( String ) sv.Get( _mf_msg ) );

        return null;
	}


    public virtual XType GetCustomStructType( Type c )
    {

        if ( c == typeof( _Etch_RuntimeException ) )
            return _mt__Etch_RuntimeException;
        if ( c == typeof( _Etch_AuthException ) )
            return _mt__Etch_AuthException;

        return null;
    }

    public virtual Type GetCustomType( XType type )
    {

        if ( type == _mt__Etch_RuntimeException )
            return typeof( _Etch_RuntimeException );
        if ( type == _mt__Etch_AuthException )
            return typeof( _Etch_AuthException );

        return null;
    }

    //public static Object ArrayValue2Native( Object obj, Type clss )
    //{
    //    if ( obj == null )
    //        return null;

    //    if ( ! clss.IsArray )
    //        return obj; //cnvrtr.Convert( obj );

    //    if ( !( obj is ArrayValue ) )
    //        return obj;


    //    ArrayValue av = ( ArrayValue ) obj;
    //    int k = av.Count;

    //    clss = clss.GetElementType();
        
    //    Array v = Array.CreateInstance( clss, k );

    //    for ( int i = 0; i < k; i++ )
    //    {
    //        ( ( Array ) v ).SetValue( ArrayValue2Native( av[ i ], clss ), i );
    //    }
    //    return v;
    //}

	
    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional Boolean array.</param>
    ///// <returns>n-dimensional Boolean array.</returns>
    //public static Object to_boolean( Type c, Object obj ) //params Object[] obj
    //{
    //    return ArrayValue2Native(obj, c);
    //}


    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional byte array.</param>
    ///// <returns>n-dimensional byte array.</returns>
    //public static Object to_byte( Type c, Object obj )
    //{
    //    return ArrayValue2Native( obj, c ); 
    //}

	
    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional short array.</param>
    ///// <returns>n-dimensional short array.</returns>
    //public static Object to_short( Type c, Object obj )
    //{
    //    return ArrayValue2Native( obj, c ); 
    //}

    
    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional int array.</param>
    ///// <returns>n-dimensional int array.</returns>
    //public static Object to_int( Type c, Object obj )
    //{
    //    return ArrayValue2Native( obj, c ); 
    //}

    
    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional long array.</param>
    ///// <returns>n-dimensional long array.</returns>
    //public static Object to_long( Type c, Object obj )
    //{
    //    return ArrayValue2Native( obj, c ); 
    //}

	  
    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional float array.</param>
    ///// <returns>n-dimensional float array.</returns>
    //public static Object to_float( Type c, Object obj )
    //{
    //    return ArrayValue2Native( obj, c );
    //}

    
    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional double array.</param>
    ///// <returns>n-dimensional double array.</returns>
    //public static Object to_double( Type c, Object obj )
    //{
    //    return ArrayValue2Native( obj, c ); 
    //}

	
    ///// <summary></summary>
    ///// <param name="n">n number of dimensions.</param>
    ///// <param name="obj">obj ArrayValue conformant to n-dimensional String array.</param>
    ///// <returns>n-dimensional String array.</returns>
    //public static Object to_string( Type c, Object obj )
    //{
    //    return ArrayValue2Native( obj, c ); 
    //}

    #region ValueFactory Members

    public abstract XType GetType( int? id );

    public abstract XType GetType( string name );

    public abstract ICollection<XType> GetTypes();

    public abstract Field GetField( int? id );

    public abstract Field GetField( string name );

    public abstract ICollection<Field> GetFields();

    #endregion
}


    
//public class TypeMap : IdNameMap<XType>
//{
//    public TypeMap( int myMaxAutoCount )
//           : base( myMaxAutoCount )
//       {// nothing
//       }
//    public override XType MakeNew(int? id, String name)
//    {
//       return new XType( id, name );
//    }
//}

//public class FieldMap : IdNameMap<Field>{
//    public FieldMap(int myMaxAutoCount)
//        : base(myMaxAutoCount)
//    {// nothing
//    }
//       public override Field MakeNew(int? id, String name)
//       {
//           return new Field(id, name);
//       }
// }
                 
}