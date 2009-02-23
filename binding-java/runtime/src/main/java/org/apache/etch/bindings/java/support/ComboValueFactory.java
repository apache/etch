package org.apache.etch.bindings.java.support;

import java.util.HashSet;
import java.util.Set;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.msg.Validator.Level;

public class ComboValueFactory implements ValueFactory
{
	public ComboValueFactory( ValueFactory vf )
	{
		dvf = vf;
	}
	
	private final ValueFactory dvf;

	public void addType( Type type )
	{
		dvf.addType( type );
	}

	public Type getType( Integer id )
	{
		Type t = dvf.getType( id );
		if (t != null)
			return t;
		
		for (ValueFactory vf: vfs)
		{
			t = vf.getType( id );
			if (t != null)
				return t;
		}
		
		return null;
	}

	public Type getType( String name )
	{
		Type t = dvf.getType( name );
		if (t != null)
			return t;
		
		for (ValueFactory vf: vfs)
		{
			t = vf.getType( name );
			if (t != null)
				return t;
		}
		
		return null;
	}

	public Set<Type> getTypes()
	{
		Set<Type> types = new HashSet<Type>( dvf.getTypes() );
		for (ValueFactory vf: vfs)
			types.addAll( vf.getTypes() );
		return types;
	}

	public StructValue exportCustomValue( Object value )
		throws UnsupportedOperationException
	{
		try
		{
			return dvf.exportCustomValue( value );
		}
		catch ( UnsupportedOperationException e )
		{
			for (ValueFactory vf: vfs)
			{
				try
				{
					return vf.exportCustomValue( value );
				}
				catch ( UnsupportedOperationException e1 )
				{
					// ignore.
				}
			}
			throw new UnsupportedOperationException();
		}
	}

	public Object importCustomValue( StructValue struct )
		throws UnsupportedOperationException
	{
		try
		{
			return dvf.importCustomValue( struct );
		}
		catch ( UnsupportedOperationException e )
		{
			for (ValueFactory vf: vfs)
			{
				try
				{
					return vf.importCustomValue( struct );
				}
				catch ( UnsupportedOperationException e1 )
				{
					// ignore.
				}
			}
			throw new UnsupportedOperationException();
		}
	}

	public Type getCustomStructType( Class<?> c )
		throws UnsupportedOperationException
	{
		try
		{
			return dvf.getCustomStructType( c );
		}
		catch ( UnsupportedOperationException e )
		{
			for (ValueFactory vf: vfs)
			{
				try
				{
					return vf.getCustomStructType( c );
				}
				catch ( UnsupportedOperationException e1 )
				{
					// ignore.
				}
			}
			throw new UnsupportedOperationException();
		}
	}

	public Long getInReplyTo( Message msg )
	{
		return dvf.getInReplyTo( msg );
	}

	public void setInReplyTo( Message msg, Long msgid )
	{
		dvf.setInReplyTo( msg, msgid );
	}

	public Level getLevel()
	{
		return dvf.getLevel();
	}

	public Long getMessageId( Message msg )
	{
		return dvf.getMessageId( msg );
	}

	public void setMessageId( Message msg, Long msgid )
	{
		dvf.setMessageId( msg, msgid );
	}

	public String getStringEncoding()
	{
		return dvf.getStringEncoding();
	}

	public Field get_mf__inReplyTo()
	{
		return dvf.get_mf__inReplyTo();
	}

	public Field get_mf__messageId()
	{
		return dvf.get_mf__messageId();
	}

	public Type get_mt__exception()
	{
		return dvf.get_mt__exception();
	}

	public Level setLevel( Level level )
	{
		return dvf.setLevel( level );
	}

	public void lockDynamicTypes()
	{
		dvf.lockDynamicTypes();
		for (ValueFactory vf: vfs)
			vf.lockDynamicTypes();
	}

	public void unlockDynamicTypes()
	{
		dvf.unlockDynamicTypes();
		for (ValueFactory vf: vfs)
			vf.unlockDynamicTypes();
	}

	public void addVf( ValueFactory vf )
	{
		Set<ValueFactory> temp = new HashSet<ValueFactory>( vfs );
		temp.add( vf );
		vfs = temp;
	}
	
	private Set<ValueFactory> vfs = new HashSet<ValueFactory>();

	public ValueFactory getDefaultVf()
	{
		return dvf;
	}
}
