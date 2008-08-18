/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.compiler;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;

import etch.compiler.ast.Module;
import etch.compiler.ast.Name;
import etch.compiler.ast.Opt;
import etch.compiler.ast.Service;

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
    	
    	clo.lh = new CompilerLogHandler( "Command" );
    	clo.lh.setQuiet( clo.quiet );
    	
    	if (!initBinding( clo ))
    		return;
    	
    	if (!checkSourceFile( clo ))
    		return;
    	
		compile( clo );
    }

	private boolean initBinding( CmdLineOptions clo )
	{
		String n = clo.binding;
		
		if (n == null || n.length() == 0)
		{
			clo.lh.logMessage( LogHandler.LEVEL_ERROR, null,
				"Binding not specified." );
			return false;
		}
		
		if (n.equalsIgnoreCase( "help" ))
		{
			// TODO find some way to list the bindings?
			clo.lh.logMessage( LogHandler.LEVEL_ERROR, null,
				"Binding help not implemented." );
			return false;
		}
		
		String cn = String.format( BINDING_FORMAT, n );
		
		try
		{
			bindingClass = cl.loadClass( cn );
		}
		catch ( ClassNotFoundException e )
		{
			clo.lh.logMessage( LogHandler.LEVEL_ERROR, null,
				String.format( "Binding '%s' could not be loaded; class '%s' not in classpath.", n, cn ) );
			return false;
		}
		
		try
		{
			binding = (Backend) bindingClass.newInstance();
		}
		catch ( Exception e )
		{
			clo.lh.logMessage( LogHandler.LEVEL_ERROR, null,
				String.format( "Binding '%s' could not be initialized; caught exception %s.", n, e ) );
			e.printStackTrace();
			return false;
		}
		
		return true;
	}
    
    private final static String BINDING_FORMAT = "etch.bindings.%s.compiler.Compiler";
    
    private Class<?> bindingClass;
    
    private Backend binding;

	private boolean checkSourceFile( CmdLineOptions clo )
	{
		File f = clo.sourceFile;
		
		if (!f.isFile())
		{
			clo.lh.logMessage( LogHandler.LEVEL_ERROR, null,
				String.format( "Source file '%s' does not exist or is not a file.", f ) );
			return false;
		}
		
		if (!f.getName().endsWith( ".etch" ))
		{
			clo.lh.logMessage( LogHandler.LEVEL_ERROR, null,
				String.format( "Source file '%s' must have .etch extension.", f ) );
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
                    File f = new File( u.toURI() );
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
            		"Specified $ETCH_HOME/lib is not a directory: %s", s ) );

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
                    ncl.addURL( f.toURL() );
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

    private void initIncludePath( CmdLineOptions clo )
    {
    	if (clo.effectiveIncludePath != null)
    		return;
    	
    	// includePath is composed of the source file's directory, then
    	// the paths specifies by -I command line directives, then the
    	// paths specified by the ETCH_INCLUDE_PATH environment variable.
    	
    	clo.effectiveIncludePath = new ArrayList<File>();
    	if (clo.workingDir != null)
    		clo.effectiveIncludePath.add( clo.workingDir );
    	
    	for (File f: clo.includePath)
    	{
    		if (f.isDirectory())
    			clo.effectiveIncludePath.add( f );
    		else
    			clo.lh.logMessage( LogHandler.LEVEL_WARNING, null,
            		"Include path element doesn't exist: "+f );
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
	                	clo.lh.logMessage( LogHandler.LEVEL_WARNING, null,
	                		"Environment include path element doesn't exist: "+f );
	            }
	        }
    	}
    }

	private void compile( CmdLineOptions clo ) throws Exception
	{
		InputStream is = new BufferedInputStream( new FileInputStream( clo.sourceFile ) );
		clo.workingDir = clo.sourceFile.getCanonicalFile().getParentFile();
    	
    	try
		{
			compile( clo, is );
		}
		finally
		{
			is.close();
		}
	}
    
    private void compile( CmdLineOptions options, InputStream is ) throws Exception
	{
    	initIncludePath( options );
    	
    	options.lh.logMessage( LogHandler.LEVEL_INFO, null,
    		options.sourceFile != null
    			? String.format( "Compiling %s ...\n", options.sourceFile )
    			: "Compiling ...\n" );

    	final EtchGrammar parser = new EtchGrammar( binding, is );
        
        final Module m;
        
        // Parse .etch source
        try
        {
            m = parser.module( options );
            options.lh.logMessage( LogHandler.LEVEL_INFO, null, " Parsed Ok.\n");
            m.check();
            options.lh.logMessage( LogHandler.LEVEL_INFO, null, " Checked Ok.\n");
        }
        catch ( ParseException e )
        {
            String fmt = e.getMessage();
            options.lh.logMessage( LogHandler.LEVEL_ERROR, e.currentToken, fmt);
            // TODO better exception
            throw new Exception(e.currentToken + " " + fmt);
        }
        catch ( Exception e)
        {
            String fmt = e.getMessage();
            options.lh.logMessage( LogHandler.LEVEL_ERROR, null, fmt);
            // TODO better exception
            throw e;
        }
        
        // Code generation
        
        try
        {
        	initOutput( options );
        	
            // TODO report work lists?
        	options.lh.logMessage( LogHandler.LEVEL_INFO, null, " Generating Resources... \n");
            
            // TODO integrate includePath with code generation.
            binding.generate( m, options );

            options.lh.logMessage( LogHandler.LEVEL_INFO, null, " Saving Resources... \n");
            
            saveOutput( options );
            
            // Move generated code to target location
            //   the parser will set 'isMixinPresent' if a "mixin" directive is in the .etch file
            // TODO would be nice if the backend just put things in the right place from the start .. sigh re-factor later        
            // moveGeneratedCode(options.outputDir, options.isMixinPresent, workingDirectory);
        }
        catch ( Exception e )
        {
            String fmt = e.getMessage();
            // TODO better exception
            e.printStackTrace();
            throw new Exception(fmt);
        }

        // Complete

        options.lh.logMessage( LogHandler.LEVEL_INFO, null, " Compile Done.\n");
    }

//    private static class GeneratedFiles
//    {
//        private Pattern rest         = Pattern.compile(".*");
//        private String IMPL_PATTERN  = ".*Impl.+(Server|Client)..+$";
//        private String MAIN_PATTERN  = ".*Main.+(Client|Listener)..+$";
//
//        private File basedir;
//
//        public GeneratedFiles(File basedir)
//        {
//            this.basedir = basedir;
//        }
//            
//        public List<String> relativeFiles()
//        {
//            return strip(find(null));
//        }
//        
//        public List<String> templateFiles()
//        {
//            List<String> r = new LinkedList<String>();
//            
//            for (String f: relativeFiles())
//            {
//                if (f.matches(IMPL_PATTERN) || f.matches(MAIN_PATTERN))
//                    r.add(f);
//            }
//            return r;
//        }
//        
//        public List<String> nonTemplateFiles()
//        {
//            List<String> r = new LinkedList<String>();
//            
//            for (String f: relativeFiles())
//            {
//                if (!f.matches(IMPL_PATTERN) && !f.matches(MAIN_PATTERN))
//                    r.add(f);
//            }
//            return r;
//        }
//        
//        private List<String> strip(List<File> files)
//        {
//            List<String> s = new LinkedList<String>();
//
//            for (File n: files)
//            {
//                Scanner t = new Scanner(n.getPath());
//                t.skip(Pattern.quote(basedir.getPath()));
//                s.add(t.next(rest));
//            }
//            return s;
//        }
//
//        private List<File> find(File root)
//        {
//            List<File> lst = new LinkedList<File>();
//
//            if (root == null)
//                root = basedir;
//
//            for (File n: root.listFiles())
//            {
//                if (n.isDirectory())
//                {
//                    lst.addAll(find(n));
//                }
//                else
//                {
//                    lst.add(n);
//                }
//            }
//            return lst;
//        }
//    }

//    private void moveFiles(File srcDir, File destDir, List<String> files, boolean overwrite) throws Exception
//    {
//        //
//        for (String f: files)
//        {
//            File srcFile  = new File(srcDir, f);
//            File destFile = new File(destDir, f);
//            if (destFile.exists() && overwrite)
//                destFile.delete();
//                
//            if (!destFile.exists())
//            {
//                File parent = new File(destFile.getParent());
//                parent.mkdirs();
//                srcFile.renameTo(destFile);
//                clo.lh.logMessage( LogHandler.LEVEL_INFO, null, "  Writing " + f + "\n");
//            }
//            else
//            {
//                logHandler.logMessage( LogHandler.LEVEL_INFO, null, "  No overwrite, skipping " + f + "\n");
//                srcFile.delete();
//            }
//        }
//    }

//    private void moveGeneratedCode(File srcDir, boolean mixinPresent, File workingDirectory) throws Exception
//    {
//
//        File target = outputDir;
//        File templateTarget = templateOutputDir;
//        
//        //  override output directory if mixin detected
//        if (mixinPresent && mixinOutputDir != null)
//            target = mixinOutputDir;
//                
//        if (target == null)
//            target = workingDirectory;
//        
//        if (templateOutputDir == null)
//            templateTarget = target;
//        
//        // move files
//        GeneratedFiles gf = new GeneratedFiles(srcDir);
//        
//        logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Moving generated code to " + target.toString() + "\n");
//        moveFiles(srcDir, target, gf.nonTemplateFiles(), true);
//        
//        logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Moving starter template code to " + templateTarget.toString() + "\n");
//        moveFiles(srcDir, templateTarget, gf.templateFiles(), overwriteTemplate);
//        
//    }

	private void initOutput( CmdLineOptions options )
	{
		options.output = new FileOutput();
		options.templateOutput = new FileOutput();
		options.mixinOutput = options.noMixinArtifacts ? new NullOutput() : new FileOutput();
	}

	private void saveOutput( CmdLineOptions options )
	{
		// TODO Auto-generated method stub
		options.output.report();
	}

	/**
	 * Parses a mixin which is within the specified service.
	 * @param intf
	 * @param n
	 * @param opts
	 * @return the constructed Mixin object, or null.
	 */
	public static Module parseModule( Service intf, Name n, Map<String, Opt> opts )
	{
		CmdLineOptions clo = intf.getCmdLineOptions();
		try
		{
			clo.lh.logMessage( LogHandler.LEVEL_INFO, n.token,
			" Checking Mixin file: " + n.name + " at line : "
				+ n.token.beginLine + "\n" );
			
			String filename = n.name;
			filename = filename.replace( '.', '/' );
			filename = filename + ".etch";
			List<File> list = clo.includePath;
			String searchPath = null;
			File etchFile = null;
			boolean mixinFileExist = false;
			
			if (list != null)
			{
				for (int i = 0; i < list.size(); i++)
				{
					File f = list.get( i );
					if (f != null)
					{
						searchPath = f.getAbsolutePath() + "/" + filename;
						etchFile = new File( searchPath );
						if (etchFile.exists())
						{
							mixinFileExist = true;
							break;
						}
					}
				}
				if (mixinFileExist)
				{
	
					clo.lh.logMessage( LogHandler.LEVEL_INFO, n.token,
						" Found mixin file " + etchFile.getAbsolutePath()
							+ " in Include Path \n" );
					// System.out.println(" We found the file " + etchFile.getName()
					// + "
					// Now lets try to generate the parsed tree");
					
					// Does -d or -dm option exist. If not return
					if (!clo.noMixinArtifacts)
					{
						if (clo.outputDir == null
							&& clo.mixinOutputDir == null)
						{
							clo.lh
							.logMessage(
								LogHandler.LEVEL_ERROR,
								n.token,
								"-d or -m option is not specified. Please specify one of these"
								+ " options. Mixin artifacts will only be generated when atleast one of these options"
								+ " is present \n" );
	
							return null;
						}
					}
					
					InputStream is = null;
					try
					{
						is = new java.io.FileInputStream( etchFile );
					}
					catch ( FileNotFoundException e1 )
					{
						e1.printStackTrace();
					}
	
					clo.lh.logMessage( LogHandler.LEVEL_INFO, null,
						"Parsing file " + etchFile.getAbsolutePath() + "\n" );
					
					clo.lh.push( etchFile.getName(), new Integer(
						n.token.beginLine ) );
					
					try
					{
						CmdLineOptions cmdLineObject = new CmdLineOptions(clo);
						HashSet<String> mixinWhat = new HashSet<String>();
						if (cmdLineObject.noMixinArtifacts)
						{
							cmdLineObject.isMixinPresent = false;
							mixinWhat.add(Backend.WHAT_NONE);
						}
						else
						{
							cmdLineObject.isMixinPresent = true;
							mixinWhat = whatForMixin(cmdLineObject.what);
						}
						cmdLineObject.what = mixinWhat;
						
//						EtchCompiler.compile( intf.getCmdLineOptions(), is );
					}
					finally
					{
						clo.lh.pop();
					}
				}
				clo.lh.logMessage( LogHandler.LEVEL_ERROR, n.token,
					" Mixin file does not exist in Include Path. \n" );
			}
			else
			{
				clo.lh.logMessage( LogHandler.LEVEL_ERROR, n.token,
					" No Include Path defined for Mixin File \n" );
			}
		}
		catch (Exception e) {
			clo.lh.logMessage( LogHandler.LEVEL_ERROR, n.token,
			" Unexpected Error occured during parsing mixin \n" );
			e.printStackTrace();
		}

		return null;
	}
	
	private static HashSet<String> whatForMixin( Set<String> what )
	{
		HashSet<String> mixinWhat = new HashSet<String>();

		if (what.contains( Backend.WHAT_BOTH ))
		{
			mixinWhat.add( Backend.WHAT_BOTH );
		}

		if (what.contains( Backend.WHAT_CLIENT ))
		{
			mixinWhat.add( Backend.WHAT_CLIENT );
		}

		if (what.contains( Backend.WHAT_SERVER ))
		{
			mixinWhat.add( Backend.WHAT_SERVER );
		}

		if (what.contains( Backend.WHAT_NONE ))
		{
			mixinWhat.clear();
			mixinWhat.add( Backend.WHAT_NONE );
		}

		return mixinWhat;
	}
}
