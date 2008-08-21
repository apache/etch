package etch.compiler;

import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

public class FileOutput implements Output
{
	public FileOutput()
	{
		this( null );
	}
	
	public FileOutput( String pn )
	{
		this.pn = pn;
	}
	
	private final String pn;
	
	public void report( String what )
	{
		report( what, "" );
	}
	
	private void report( String what, String indent )
	{
		if (pn != null)
			System.out.printf( "%spackage %s", indent, pn );
		else
			System.out.printf( "%s%s", indent, what );
		System.out.println();
		
		String sindent = indent+"  ";
		
		for (Map.Entry<String, ByteArrayOutputStream> me: files.entrySet())
			report( sindent, me.getKey(), me.getValue() );
		
		for (FileOutput p: packages.values())
			p.report( what, sindent );
	}

	private void report( String indent, String fn, ByteArrayOutputStream value )
	{
		System.out.printf( "%sfile %s %d", indent, fn, value.size() );
		System.out.println();
	}

	public OutputStream newOutputStream( String fn )
	{
		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		files.put( fn, baos );
		return baos;
	}
	
	private final Map<String, ByteArrayOutputStream> files =
		new HashMap<String, ByteArrayOutputStream>();

	public Output newPackage( String pn )
	{
		FileOutput p = packages.get( pn );
		if (p == null)
		{
			p = new FileOutput( pn );
			packages.put( pn, p );
		}
		return p;
	}
	
	private final Map<String, FileOutput> packages = new HashMap<String, FileOutput>();
}
