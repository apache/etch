/**
 * 
 */
package etch.bindings.java.transport;

import metreos.core.io.Source;
import etch.bindings.java.support.MessageSource;

/**
 * A combination of MessageSource and Source.
 */
public interface MessagizerMessageSource
	extends MessageSource,
		Source<MessageHandler<MessagizerMessageSource>>
{
	// nothing
}
