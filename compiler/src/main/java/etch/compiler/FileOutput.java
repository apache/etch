package etch.compiler;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

/**
 * File/directory based implementation of Output container.
 */
public class FileOutput implements Output
{
	/**
	 * Constructs top-level Output, no package.
	 */
	public FileOutput()
	{
		this( null );
	}
	
	/**
	 * Constructs mid-level Output with package name.
	 * @param pn
	 */
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

	public void saveFiles( File outputDir, boolean noQualOutputDir,
		boolean noFlattenPackages, boolean force ) throws IOException
	{
		saveFiles( outputDir, noQualOutputDir, noFlattenPackages, force, null );
	}

	private void saveFiles( File outputDir, boolean noQualOutputDir,
		boolean noFlattenPackages, boolean force, String pkg ) throws IOException
	{
		if (pn != null)
		{
			if (pkg != null)
				pkg += '.' + pn;
			else
				pkg = pn;
		}

//		System.out.printf(
//			"saveFiles: outputDir %s, noQualOutputDir %s, noFlattenPackages %s, force %s, pkg %s\n",
//			outputDir, noQualOutputDir, noFlattenPackages, force, pkg );
		
		for (Map.Entry<String, ByteArrayOutputStream> me: files.entrySet())
			saveFile( outputDir, noQualOutputDir, noFlattenPackages, force, pkg, me.getKey(), me.getValue() );
		
		for (FileOutput p: packages.values())
			p.saveFiles( outputDir, noQualOutputDir, noFlattenPackages, force, pkg );
	}

	private void saveFile( File outputDir, boolean noQualOutputDir,
		boolean noFlattenPackages, boolean force, String pkg, String fn,
		ByteArrayOutputStream value ) throws IOException
	{
//		System.out.printf(
//			"saveFile: outputDir %s, noQualOutputDir %s, noFlattenPackages %s, force %s, pkg %s, fn %s, size %d\n",
//			outputDir, noQualOutputDir, noFlattenPackages, force, pkg, fn, value.size() );
		
		if (pkg != null && !noQualOutputDir)
		{
			if (noFlattenPackages)
				pkg = pkg.replace( '.', File.separatorChar );
			outputDir = new File( outputDir, pkg );
		}
		
		File f = new File( outputDir, fn ).getCanonicalFile();
		
		if (f.isFile() && !force)
		{
			System.out.printf( "*** existing file not overwritten: %s\n", f );
			return;
		}
		
		f.getParentFile().mkdirs();
		
		FileOutputStream fos = new FileOutputStream( f );
		try
		{
			value.writeTo( fos );
		}
		catch ( IOException e )
		{
			fos.close();
			f.delete();
			IOException x = new IOException( "error writing "+f );
			x.initCause( e );
			throw x;
		}
		finally
		{
			fos.close();
		}
		
		System.out.printf( "wrote file %s, size %d\n", f, value.size() );
	}
}
