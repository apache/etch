package etch.bindings.java.transport;

import metreos.util.URL;
import etch.bindings.java.msg.TaggedDataInput;
import etch.bindings.java.msg.TaggedDataOutput;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedDataInput;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedDataOutput;

/**
 * Format factory for binary tagged data.
 */
public class BinaryTaggedDataFormatFactory extends FormatFactory
{
	@Override
	public TaggedDataInput newTaggedDataInput( ValueFactory vf, URL uri )
	{
		return new BinaryTaggedDataInput( vf, uri );
	}

	@Override
	public TaggedDataOutput newTaggedDataOutput( ValueFactory vf, URL uri )
	{
		return new BinaryTaggedDataOutput( vf, uri );
	}
}