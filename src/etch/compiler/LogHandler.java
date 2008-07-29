package etch.compiler;


/**
 * This Interface is used for Logging messages. It helps provide a consistent
 * way of printing and reporting Error, Info and Warning messages.
 * @author gsandhir
 *
 */
public interface LogHandler
{
	/**
	 * Constant to specify an Error Message
	 */
	public static final int LEVEL_ERROR = 1;
	/**
	 * Constant to specify an Warning Message
	 */
	public static final int LEVEL_WARNING = 2;
	/**
	 * Constant to specify an Info Message
	 */
	public static final int LEVEL_INFO = 3;
	/**
	 * Constant to specify an unformatted message to be printed always.
	 */
	public static final int LEVEL_MESSAGE = 4;
	
	/**
	 * This method is used to either store or print messages
	 * @param level indicates ERROR, WARNING or INFO
	 * @param token indicates the token at which the error occured
	 * @param msg String message
	 */
	public void logMessage(int level, Token token, String msg );
	
	/**
	 * This method is used to specify the source of the Message
	 * @param str  File name where the Message occured
	 * @param num  Line Number where the Message occured
	 */
	
	public void push(String str, Integer num);
	
	/**
	 * This method removes the source that was added by the push method.
	 * It behaves exactly like the pop method in Stacks
	 *
	 */
	
	public void pop();
	
	/**
	 * Indicates whether an error was encountered
	 * @return flag to indicate whether an error occured
	 */
	
	public boolean hasError();
	
	/**
	 * Sets the quiet flag.
	 * @param quiet
	 */
	public void setQuiet( boolean quiet );

	/**
	 * This method gets the isQuiet Variable
	 * @return whether Compiler is printing info and warning messages
	 */
	public boolean getQuiet();
	
	/**
	 * Sets the current phase of processing.
	 * @param phase
	 */
	public void setPhase( String phase );
	
	/**
	 * @return the current phase of processing.
	 */
	public String getPhase();
}
