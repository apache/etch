/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.compiler;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;

import org.apache.etch.compiler.ast.Module;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Opt;
import org.apache.etch.compiler.ast.Service;


/**
 * The main driver of the compiler.
 */
public class EtchCompiler
{
    /**
     * Instantiate a new compiler
     * @param cl the class loader to use to load the binding
     */
    public EtchCompiler( ClassLoader cl )
    {
    	this.cl = cl;
    }
    
    private final ClassLoader cl;
    
    /**
     * Runs the compiler using the specified input options.
     * @param clo
     * @throws Exception 
     */
    public void run( CmdLineOptions clo ) throws Exception
    {
    	clo.cl = cl;
    	
    	if (clo.lh == null)
    	{
	    	clo.lh = new ConsoleLogHandler( "Command" );
	    	clo.lh.setLevel(
	    		clo.quiet ? LogHandler.LEVEL_WARNING : LogHandler.LEVEL_INFO );
    	}
    	
    	if (!initBindingClass( clo ))
    		return;
    	
    	if (!checkSourceFile( clo ))
    		return;
    	
    	initOutput( clo );
        clo.lh.push( clo.sourceFile.getName(), null );
        
        try
        {
			compile( clo );
	    	
			if (!clo.lh.hasError())
			{
		        clo.lh.report( LogHandler.LEVEL_INFO, "Saving Resources..." );
		        
		        saveOutput( clo );
		        
		        if (!clo.lh.hasError())
		        	clo.lh.report( LogHandler.LEVEL_INFO, "Saving Resources Done." );
		        else
		        	clo.lh.report( LogHandler.LEVEL_INFO, "Saving Resources Failed." );
			}
			else
				clo.lh.report( LogHandler.LEVEL_INFO, "Compile Failed." );
	    }
        catch ( Exception e )
        {
        	clo.lh.report( LogHandler.LEVEL_ERROR, "caught exception: %s", e );
        	e.printStackTrace();
        }
        finally
        {
        	clo.lh.pop( clo.sourceFile.getName() );
        }
    }

	private static boolean initBindingClass( CmdLineOptions clo )
	{
		if (clo.bindingClass != null)
			return true;

		String n = clo.binding;
		
		if (n == null || n.length() == 0)
		{
			clo.lh.report( LogHandler.LEVEL_ERROR,
				"Binding not specified." );
			return false;
		}
		
		if (n.equalsIgnoreCase( "help" ))
		{
			// TODO find some way to list the bindings?
			clo.lh.report( LogHandler.LEVEL_ERROR,
				"Binding help not yet implemented." );
			return false;
		}
		
		if (n.equalsIgnoreCase( "null" ))
		{
			clo.bindingClass = NullCompiler.class;
			return true;
		}
		
		String cn = String.format( BINDING_FORMAT, n );
		
		try
		{
			clo.bindingClass = clo.cl.loadClass( cn );
			return true;
		}
		catch ( ClassNotFoundException e )
		{
			clo.lh.report( LogHandler.LEVEL_ERROR,
				"Binding '%s' could not be loaded; class '%s' not in classpath.",
				n, cn );
			return false;
		}
		catch ( RuntimeException e )
		{
			clo.lh.report( LogHandler.LEVEL_ERROR,
				"Binding '%s' could not be loaded; class '%s' threw exception %s",
				n, cn, e );
			e.printStackTrace();
			return false;
		}
	}
	
	private static boolean initBackend( CmdLineOptions clo )
	{
		try
		{
			clo.backend = (Backend) clo.bindingClass.newInstance();
			return true;
		}
		catch ( Exception e )
		{
			clo.lh.report( LogHandler.LEVEL_ERROR,
				"Binding '%s' could not be initialized; caught exception %s.",
				clo.binding, e );
			e.printStackTrace();
			return false;
		}
	}
    
    private final static String BINDING_FORMAT = "org.apache.etch.bindings.%s.compiler.Compiler";

	private static boolean checkSourceFile( CmdLineOptions clo )
	{
		File f = clo.sourceFile;
		
		if (!f.isFile())
		{
			clo.lh.report( LogHandler.LEVEL_ERROR,
				"Source file '%s' does not exist or is not a file.", f );
			return false;
		}
		
		if (!f.getName().endsWith( ".etch" ))
		{
			clo.lh.report( LogHandler.LEVEL_ERROR,
				"Source file '%s' must have .etch extension.", f );
			return false;
		}
		
		return true;
	}

	/**
     * @param etchHome command line specified location for the etch.
     * @return sets up the class loader based on information from
     * our environment.
     * @throws Exception
     */
    public static ClassLoader setupClassLoader( File etchHome )
        throws Exception
    {
        // get the current class path...
        
        Set<String> mainClassPath = new HashSet<String>();
        
        ClassLoader cl = EtchMain.class.getClassLoader();
        if (cl instanceof URLClassLoader)
        {
            for (URL u: ((URLClassLoader) cl).getURLs())
            {
                String s = u.toString();
                if (s.startsWith( "file:" ) && s.endsWith( ".jar" ))
                {
                    final String scheme = "file";
					final String path = s.substring( scheme.length() + 1 );
					// multi-arg constructor properly escapes paths
					File f = new File( new URI( scheme, null, path, null ) );
                    if (f.isFile() && f.canRead())
                    {
                        s = f.getCanonicalPath();
                        mainClassPath.add( s );
//                      System.out.println( "mainClassPath.add: "+s );
                    }
                }
            }
        }
        
        // search etch.home (if specified) for more jar files to add
        // to a secondary class loader. exclude jar files on the main
        // class path.
        
        String s = etchHome != null ? etchHome.getCanonicalPath() : null;
        
        if (s == null || s.length() == 0)
        {
        	// from java -Detch.home=...
            s = System.getProperty( "etch.home" );
        }

        if (s == null || s.length() == 0)
        {
            s = System.getenv( "ETCH_HOME" );
        }
        
        if (s != null && s.length() > 0)
            s = s + File.separator + "lib";

        //      System.out.println( "etch.home.lib = "+s );
        if (s != null && s.length() > 0)
        {
            File d = new File( s );
            
            if (!d.isDirectory() || !d.canRead())
            	throw new IOException( String.format(
            		"Specified $ETCH_HOME/lib is not a directory: %s\n", s ) );

            d = d.getCanonicalFile();
            
            MyURLClassLoader ncl = new MyURLClassLoader( new URL[] {}, cl );
            for (File f: d.listFiles())
            {
                if (!f.isFile())
                    continue;
                String x = f.getCanonicalPath();
                if (!x.endsWith( ".jar" ))
                    continue;
                if (!mainClassPath.contains( x ))
                    ncl.addURL( f.toURI().toURL() );
            }
            cl = ncl;
        }
        return cl;
    }
    
    private static class MyURLClassLoader extends URLClassLoader
    {
        /**
         * This is just a class loader where we can hang more urls to
         * search.
         * @param urls
         * @param parent
         */
        public MyURLClassLoader( URL[] urls, ClassLoader parent )
        {
            super( urls, parent );
        }
        
        @Override
        public void addURL( URL url )
        {
            super.addURL( url );
        }
    }
    
    // includePath

    /**
     * Initialize {@link CmdLineOptions#effectiveIncludePath} given the
     * compilation we are about to perform.
     * @param clo
     */
    private static void initIncludePath( CmdLineOptions clo ) throws IOException
    {
//    	System.out.println( "initIncludePath: sourceFile = "+clo.sourceFile );
//    	System.out.println( "initIncludePath: includePath = "+clo.includePath );
    	
    	// includePath is composed of the source file's directory, then
    	// the paths specifies by -I command line directives, then the
    	// paths specified by the ETCH_INCLUDE_PATH environment variable.
    	
    	clo.effectiveIncludePath = new ArrayList<File>( );
    	
    	// add directory of source file.
    	if (clo.sourceFile != null)
    		clo.effectiveIncludePath.add(
    			clo.sourceFile.getCanonicalFile().getParentFile() );
    	
    	for (File f: clo.includePath)
    	{
    		if (f.isDirectory())
    			clo.effectiveIncludePath.add( f );
    		else
    			clo.lh.report( LogHandler.LEVEL_WARNING,
            		"Include path element doesn't exist: %s", f );
    	}
    	
    	if (!clo.ignoreIncludePath)
    	{
	        String s = System.getenv( CmdLineOptions.ETCH_INCLUDE_PATH );
	        if (s != null)
	        {
	            StringTokenizer st = new StringTokenizer( s, File.pathSeparator );
	            while (st.hasMoreTokens())
	            {
	                File f = new File( st.nextToken() );
	                if (f.isDirectory())
	                	clo.effectiveIncludePath.add( f );
	                else
	                	clo.lh.report( LogHandler.LEVEL_WARNING,
	                		"Environment include path element doesn't exist: %s", f );
	            }
	        }
    	}
//    	System.out.println( "initIncludePath: effectiveIncludePath = "+clo.effectiveIncludePath );
    }

	private static Module compile( CmdLineOptions clo ) throws Exception
	{
		InputStream is = new BufferedInputStream( new FileInputStream( clo.sourceFile ) );
    	try
		{
			return compile( clo, is );
		}
		finally
		{
			is.close();
		}
	}
    
    private static Module compile( CmdLineOptions clo, InputStream is ) throws Exception
	{
    	initIncludePath( clo );
    	
    	if (!initBackend( clo ))
    		return null;
    	
    	if (clo.sourceFile != null)
    		clo.lh.report( LogHandler.LEVEL_INFO, "Compiling %s...", clo.sourceFile );
    	else
    		clo.lh.report( LogHandler.LEVEL_INFO, "Compiling..." );
    	
    	final EtchGrammar parser = new EtchGrammar( clo.backend, is );
        
        final Module m;
        
        // Parse .etch source
        try
        {
            m = parser.module( clo );
            clo.lh.report( LogHandler.LEVEL_INFO, "Parsed Ok." );
            
            m.check();
            clo.lh.report( LogHandler.LEVEL_INFO, "Checked Ok." );
        }
        catch ( ParseException e )
        {
            String fmt = e.getMessage();
            clo.lh.report( LogHandler.LEVEL_ERROR,
            	e.currentToken != null ? e.currentToken.beginLine : null,
            		e.getMessage() );
            // TODO better report of a ParseException
            throw new Exception(e.currentToken + " " + fmt);
        }
        catch ( Exception e )
        {
            clo.lh.report( LogHandler.LEVEL_ERROR, "caught exception: %s", e );
            // TODO better report of an Exception
            throw e;
        }
        
        // Code generation
        
        try
        {
            // TODO report work lists?
        	clo.lh.report( LogHandler.LEVEL_INFO, "Generating Resources..." );
            
            // TODO integrate includePath with code generation.
            clo.backend.generate( m, clo );

            // Move generated code to target location
            //   the parser will set 'isMixinPresent' if a "mixin" directive is in the .etch file
            // TODO would be nice if the backend just put things in the right place from the start .. sigh re-factor later        
            // moveGeneratedCode(options.outputDir, options.isMixinPresent, workingDirectory);
        }
        catch ( Exception e )
        {
            String fmt = e.getMessage();
            // TODO better report of an Exception
            e.printStackTrace();
            throw new Exception(fmt);
        }

        // Complete

        clo.lh.report( LogHandler.LEVEL_INFO, "Compile Done." );
        return m;
    }

	private static void initOutput( CmdLineOptions clo )
	{
		clo.output = new FileOutput();
		clo.templateOutput = new FileOutput();
		clo.mixinOutput = clo.noMixinArtifacts ? new NullOutput() : new FileOutput();
	}

	private static void saveOutput( CmdLineOptions clo ) throws IOException
	{
		// TODO implement saveOutput
//		clo.output.report( "output" );
//		clo.templateOutput.report( "templateOutput" );
//		clo.mixinOutput.report( "mixinOutput" );
		
		if (clo.testing)
			return;
		
		// Destination directory of generated artifacts. Use source file's
		// parent directory if not specified.
		
		File outputDir = clo.outputDir;
		boolean noQualOutputDir = false;
		if (outputDir == null)
		{
			outputDir = clo.sourceFile.getCanonicalFile().getParentFile();
			noQualOutputDir = true;
		}

		// Destination directory of generated user-editable artifacts. Use
		// outputDir if not specified.
		
		File templateOutputDir = clo.templateOutputDir;
		boolean noQualTemplateOutputDir = false;
		if (templateOutputDir == null)
		{
			templateOutputDir = outputDir;
			noQualTemplateOutputDir = noQualOutputDir;
		}
		
		// Destination directory of mixin generated artifacts. Use outputDir
		// if not specified.
		
		File mixinOutputDir = clo.mixinOutputDir;
		boolean noQualMixinOutputDir = false;
		if (mixinOutputDir == null)
		{
			mixinOutputDir = outputDir;
			noQualMixinOutputDir = noQualOutputDir;
		}
		
		boolean force = clo.what.contains( Backend.WHAT_FORCE );

		saveFiles( clo.output, outputDir, noQualOutputDir, clo.noFlattenPackages, true );
		saveFiles( clo.templateOutput, templateOutputDir, noQualTemplateOutputDir, clo.noFlattenPackages, force );
		saveFiles( clo.mixinOutput, mixinOutputDir, noQualMixinOutputDir, clo.noFlattenPackages, true );
	}

	private static void saveFiles( Output output, File outputDir,
		boolean noQualOutputDir, boolean noFlattenPackages, boolean force )
		throws IOException
	{
		output.saveFiles( outputDir, noQualOutputDir, noFlattenPackages, force );
	}

	/**
	 * Parses a mixin which is within the specified service.
	 * @param intf
	 * @param n
	 * @param opts
	 * @return the constructed Mixin object, or null.
	 * @throws Exception 
	 */
	public static Module parseModule( Service intf, Name n, Map<String,
		Opt> opts ) throws Exception
	{
		CmdLineOptions clo = new CmdLineOptions( intf.getCmdLineOptions() );
		
		// find the mixin file
		
		String fn = n.name;
		
		// fn is something like a.b.c.d or perhaps just plain d.
		//
		// we will look for d.etch, a/b/c/d.etch, and a.b.c/d.etch.
		//
		// we can look in the same directory as the original source we are
		// compiling (the working dir), or any of the include path directories.
		
		String module;
		String file;
		int i = fn.lastIndexOf( '.' );
		if (i >= 0)
		{
			module = fn.substring( 0, i );
			file = fn.substring( i+1 );
		}
		else
		{
			module = null;
			file = fn;
		}
		
		file = file + ".etch";
		
//		System.out.printf( "searching for mixin '%s' module '%s'\n", file, module );
//		System.out.println( "includePath = "+clo.includePath );
//		System.out.println( "effectiveIncludePath = "+clo.effectiveIncludePath );
		
		List<File> list = new ArrayList<File>();
		
		// look for the unqualified name
		find( list, clo, null, file );
		
		if (module != null)
		{
			// look for the qualified name
			find( list, clo, module, file );
			find( list, clo, module.replace( '.', '/' ), file );
		}
		
		if (list.isEmpty())
			return null;
		
		File f = list.get( 0 );
//		System.out.println( "mixing in = "+f );
		
		return parseModule0( clo, n, f );
	}
	
	private static void find( List<File> list, CmdLineOptions clo, String path,
		String file ) throws IOException
	{
		for (File d: clo.effectiveIncludePath)
		{
			File f = d;
			
			if (path != null)
				f = new File( f, path );
			
			f = new File( f, file ).getCanonicalFile();
			
			if (f.isFile() && !list.contains( f ))
				list.add( f );
		}
	}

	/**
	 * @param clo
	 * @param n
	 * @param f
	 * @return the parsed module of the mixin.
	 * @throws Exception
	 */
	public static Module parseModule0( CmdLineOptions clo, Name n, File f )
		throws Exception
	{
		clo.sourceFile = f;
		clo.output = clo.mixinOutput;
		
		clo.templateOutput = new NullOutput();
		clo.what.remove( Backend.WHAT_MAIN );
		clo.what.remove( Backend.WHAT_IMPL );
		
		if (clo.noMixinArtifacts)
		{
			clo.what.clear();
			clo.what.add( Backend.WHAT_NONE );
		}
		
		clo.lh.report( LogHandler.LEVEL_INFO, n.token.beginLine,
			"Compiling mixin file: %s", n.name );
		clo.lh.push( f.getName(), n.token.beginLine );
		try
		{
			return compile( clo );
		}
		finally
		{
			clo.lh.pop( f.getName() );
			clo.lh.report( LogHandler.LEVEL_INFO, n.token.beginLine,
				"Done compiling mixin file: %s", n.name );
		}
	}
}
