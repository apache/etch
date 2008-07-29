package etch.examples.log;

/**
 * @author bchampak
 *
 */
public interface LogSink {
	
	/**
	 * @param level
	 * @param msg
	 */
	void write( int level, String msg );
}
