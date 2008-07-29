package etch.bindings.java.transport.fmt.binary;

import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.FormatFactory;
import etch.bindings.java.transport.TaggedDataInput;
import etch.bindings.java.transport.TaggedDataOutput;

/**
 * Format factory for binary tagged data.
 */
public class BinaryTaggedDataFormatFactory extends FormatFactory
{
	@Override
	public TaggedDataInput newTaggedDataInput( ValueFactory vf, String uri )
	{
		return new BinaryTaggedDataInput( vf, uri );
	}

	@Override
	public TaggedDataOutput newTaggedDataOutput( ValueFactory vf, String uri )
	{
		return new BinaryTaggedDataOutput( vf, uri );
	}
}