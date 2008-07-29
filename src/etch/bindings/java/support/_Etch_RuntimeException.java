package etch.bindings.java.support;

/**
 * A RuntimeException which exposes its msg field. Returned by the stub
 * to the remote when the stub catches a RuntimeException.
 */
public class _Etch_RuntimeException
	extends RuntimeException
{
	private static final long serialVersionUID = 3051515541923877063L;

	/**
	 * Constructs the EtchRuntimeException. Don't init anything.
	 */
	public _Etch_RuntimeException()
	{
		// don't init anything.
	}
	
	/**
	 * Constructs the EtchRuntimeException.
	 * @param msg description of the exception that was caught by the stub
	 * on the remote side.
	 */
	public _Etch_RuntimeException( String msg )
	{
		this.msg = msg;
	}

	@Override
	public String getMessage()
	{
		return msg;
	}

	/**
	 * Description of the exception that was caught by the stub
	 * on the remote side.
	 */
	public String msg;
	
	@Override
	public String toString()
	{
		return "Remote side threw this runtime exception: "+getMessage();
	}
}
