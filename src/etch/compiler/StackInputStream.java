package etch.compiler;

import java.io.IOException;
import java.io.InputStream;

/**
 * Input stream to support include in the etch compiler.
 */
public class StackInputStream extends InputStream
{
	static int nOnce = 0;
	
	private InputStream mainStream = null;
	private InputStream includeStream = null;
	private InputStream currentStream = null;

	/**
	 * Constructs the StackInputStream.
	 * @param in1
	 */
	public StackInputStream(InputStream in1)
	{
		this.mainStream = in1;
		currentStream = mainStream;
	}

	/**
	 * @param in2
	 */
	public void setStreamTwo(InputStream in2)
	{
		this.includeStream = in2;
	}	

	/**
	 * @param val
	 */
	public void ActivateStream(int val)
	{
		if(val == 0)
		{
			currentStream = mainStream;
		}
		else if(val == 1)
		{
			currentStream = includeStream;
		}
	}
	
	@Override
	public int read()
	{
		int val = 0;
		try
		{
			val = currentStream.read();
		}
		catch ( IOException e )
		{
			e.printStackTrace();
		}
		
		if(val == -1 && nOnce == 0 && includeStream != null)
		{
			System.out.println("------Stack Reset To Stream 0 >>>> " + val);
			ActivateStream(1);
			try
			{
				val = currentStream.read();
			}
			catch ( IOException e )
			{
				e.printStackTrace();
			}
			nOnce = 1;
		}
		
	
		System.out.println("------Stack Read >>>> " + val);
		return val;
	}
}
