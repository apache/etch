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

import java.io.File;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import etch.util.cmd.CommandParser;
import etch.util.cmd.Option;
import etch.util.cmd.Parameter;
import etch.util.cmd.Program;

/**
 * The main command line interface for the etch compiler.
 */
public class EtchMain extends Program
{
    private static String WHO_DELIMETER = ",";
    
    /**
     * @param args
     * @throws Exception 
     */
    public static void main( String[] args ) throws Exception
    {
        try
        {
            main( new EtchMain(), args );
        }
        catch ( Throwable e )
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
            System.exit( 3 );
            return;
        }
    }
    
    /**
     * @param etchHome command line specified location for the etch.
     * @return sets up the class loader based on information from
     * our environment.
     * @throws Exception
     */
    public static ClassLoader setupClassLoader( String etchHome )
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
	    		if (s.startsWith( "file:/" ) && s.endsWith( ".jar" ))
	    		{
	    			s = s.substring( 6 );
	    			File f = new File( s );
	    			if (f.isFile() && f.canRead())
	    			{
	    				s = f.getCanonicalPath();
	    				mainClassPath.add( s );
//    					System.out.println( "mainClassPath.add: "+s );
	    			}
	    		}
	    	}
    	}
    	
    	// search etch.home.lib (if specified) for more jar files to add
    	// to a secondary class loader. exclude jar files on the main
    	// class path.
    	
    	String s = etchHome;
    	
    	if (s == null || s.length() == 0)
    		s = System.getProperty( "etch.home.lib" );
    	
    	if (s == null || s.length() == 0)
    	{
    		s = System.getProperty( "etch.home" );
    		if (s != null && s.length() > 0)
    			s = s + File.separator + "lib";
    	}

    	if (s == null || s.length() == 0)
    	{
    		s = System.getenv( "ETCH_HOME" );
    		if (s != null && s.length() > 0)
    			s = s + File.separator + "lib";
    	}
    	
//    	System.out.println( "etch.home.lib = "+s );
    	if (s != null && s.length() > 0)
    	{
    		File d = new File( s );
    		if (d.isDirectory() && d.canRead())
    		{
    			MyURLClassLoader ncl = new MyURLClassLoader( new URL[] {}, cl );
    			for (File f: d.listFiles())
    			{
    				if (!f.isFile())
    					continue;
    				if (!f.getName().endsWith( ".jar" ))
    					continue;
    				s = f.getCanonicalPath();
    				if (!mainClassPath.contains( s ))
    				{
        				s = s.replace( '\\', '/' );
        				s = String.format( "file:/%s", s );
//    					System.out.println( "+++ "+s );
    					ncl.addURL( new URL( s ) );
    				}
    				else
    				{
//    					System.out.println( "--- "+s );
    				}
    			}
    			cl = ncl;
    		}
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

    /**
     * Constructs the etch parser / compiler driver.
     */
    public EtchMain()
    {
        sourceFiles = new LinkedList<File>();   
        what        = new LinkedList<String>();
        includePath = new LinkedList<File>();
    }

    @Override
    public String toString()
    {
        return "etch";
    }

    @Override
    protected void defineOptionsAndParameters( CommandParser cp )
        throws Exception
    {
        cp.defineFileOption( "-t|--template-out", "templateDir", "setTemplateDir",
            "specifies the output directory for Impl* and Main* files",
            Option.SINGLETON, null, null );
            
        cp.defineFileOption( "-d|--output-dir", "outputDir", "setOutputDir",
            "specifies the output directory",
            Option.SINGLETON, null, null );

        cp.defineStringOption( "-b|--binding", "binding", "setBinding",
            "specifies a target language binding to generate",
            Option.REQUIRED|Option.SINGLETON, null, null );

        cp.defineStringOption( "-w|--what|--who", "what", "setWhat",
            "specifies what files to generate: all,server,client,impl,main,helper (--who is deprecated)",
            Option.SINGLETON, "BOTH", null );

        cp.defineNullOption( "-g|--ignore-global", "ignoreGlobalWordsList",
            "ignore the global reserved words list",
            Option.SINGLETON );

        cp.defineNullOption( "-l|--ignore-local", "ignoreLocalWordsList",
            "ignore the local reserved words list",
            Option.SINGLETON );

        cp.defineFileOption( "-W|--word-list", "wordList", "setUserWordsList",
            "specifies a reserved words list",
            Option.SINGLETON, null, null );

        cp.defineStringOption( "-I|--include-path", "includePath", "setIncludePath",
            "a search directory for included etch files",
            Option.NONE, null, null );

        /*
        cp.defineStringOption( "-L|--etch-lib", "etchLib", "setEtchLib",
            "the fully qualified path of ETCH_HOME/lib",
            Option.SINGLETON, null, null );
        */

        cp.defineNullOption( "-i|--ignore-include-path", "ignoreEnvIncludePath",
            "ignore the include path environment variable",
            Option.SINGLETON );

        cp.defineFileOption( "-m|--dir-mixin", "mixinOutputDir", "setMixinOutputDir",
            "the output dir for mixin artifacts",
            Option.SINGLETON, null, null );

        cp.defineNullOption( "-n|--no-mixin-artifacts", "setNoMixinArtifacts",
            "mixin artifacts should not be generated",
            Option.SINGLETON );

        cp.defineNullOption( "-q|--quiet", "setQuiet",
            "only report problems",
            Option.SINGLETON );

        cp.defineNullOption( "-v|--version", "doVersion",
            "show the version and exit",
            Option.SINGLETON );

        cp.defineNullOption( "-f|--no-flatten", "setNoFlattenPackages",
            "the namespace directory tree should not be flattened",
            Option.SINGLETON );

        cp.defineFileParameter( "file", "setFile",
            "Etch source to compile", true, true, null );
    }

    /**
     * Command parser method to set the output directory.
     * @param cp
     * @param option
     * @param token
     * @param value the output directory
     * @return true if it worked, false otherwise.
     */
    public boolean setOutputDir( CommandParser cp, Option option, String token,
        File value )
    {
        outputDirectory = value;
        return true;
    }
    private File outputDirectory = null;
    
    /**
     * Command parser method to set the output directory for template files Impl* and Main*.
     * @param cp
     * @param option
     * @param token
     * @param value the template output directory
     * @return true if it worked, false otherwise.
     */
    public boolean setTemplateDir( CommandParser cp, Option option, String token,
        File value )
    {
        templateOutputDirectory = value;
        return true;
    }
    private File templateOutputDirectory = null;    

    /**
     * Command parser method to set the binding.
     * @param cp
     * @param option
     * @param token
     * @param value the binding
     * @return true if it worked, false otherwise.
     */
    public boolean setBinding( CommandParser cp, Option option, String token,
        String value )
    {
        bindingName = value;
        return true;
    }
    private String bindingName = null;

    /**
     * Command parser method to set for whom/what we are generating code.
     * @param cp
     * @param option
     * @param token
     * @param value the list of what needs to be generated.
     * @return true if it worked, false otherwise.
     */
    public boolean setWhat( CommandParser cp, Option option, String token,
        String value )
    {
        StringTokenizer st = new StringTokenizer(value,WHO_DELIMETER);

        while (st.hasMoreElements()) 
            what.add(st.nextToken().toUpperCase());

        return true;
    }
    private List<String> what;

    /**
     * Command parser method to ignore the globally reserved words list.
     * @param cp
     * @param option
     * @param token
     */
    public void ignoreGlobalWordsList(CommandParser cp, Option option, String token)
    {
        ignoreGlobal = true;
    }
    private boolean ignoreGlobal = true;

    /**
     * Command parser method to ignore the locally reserved words list.
     * @param cp
     * @param option
     * @param token
     */
    public void ignoreLocalWordsList(CommandParser cp, Option option, String token)
    {
        ignoreLocal = true;
    }
    private boolean ignoreLocal = false;

    /**
     * Command parser method to set the user-defined reserved words list.
     * @param cp
     * @param option
     * @param token
     * @param value the reserved words list
     * @return true if it worked, false otherwise.
     */
    public boolean setUserWordsList(CommandParser cp, Option option, String token,
        File value)
    {
        userWordList = value;
        return true;
    }
    private File userWordList = null;

    /**
     * Command parser method to set the include path.
     * @param cp
     * @param option
     * @param token
     * @param value path to append to the current include path
     */
    public void setIncludePath( CommandParser cp, Option option, String token,
        String value )
    {
        StringTokenizer includeTokens = new StringTokenizer(value,
            File.pathSeparator);

        while(includeTokens.hasMoreTokens())
        {
            File temp = new File(includeTokens.nextToken());
            includePath.add(temp);
        }
    }
    private List<File> includePath;

    /**
     * Command parser method to ignore the include path from the environment.
     * @param cp
     * @param option
     * @param token
     */
    public void ignoreEnvIncludePath(CommandParser cp, Option option,
        String token)
    {
        ignoreEnvironmentIncludePath = true;
    }
    private boolean ignoreEnvironmentIncludePath = false;

    /**
     * Command parser method to set the quiet flag.
     * @param cp
     * @param option
     * @param token
     */
    public void setQuiet( CommandParser cp, Option option, String token )
    {
        quiet = true;
    }
    private boolean quiet = false;

    /**
     * Command parser method to show the version and exit.
     * @param cp
     * @param option
     * @param token
     * @return false so that we stop.
     */
    public boolean doVersion( CommandParser cp, Option option, String token )
    {
        System.out.println(String.format("Etch version is %s", Version.VERSION));
        return false;
    }

    /**
     * Command parser method to suppress mixin artifacts.
     * @param cp
     * @param option
     * @param token
     */
    public void setNoMixinArtifacts( CommandParser cp, Option option, String token )
    {
        mixinSuppressed = true;
    }
    private boolean mixinSuppressed = false;

    /**
     * Command parser method to set the output directory for mixins.
     * @param cp
     * @param option
     * @param token
     * @param value the output directory for mixin
     * @return true if it worked, false otherwise.
     */
    public boolean setMixinOutputDir( CommandParser cp, Option option, String token,
        File value )
    {
        mixinOutputDirectory = value;
        return true;
    }
    
    private File mixinOutputDirectory = null;

    /**
     * @param cp
     * @param option
     * @param token
     */
    public void setNoFlattenPackages( CommandParser cp, Option option, String token)
    {
    	noFlattenPackages = true;
    }
    
    private boolean noFlattenPackages;

    /**
     * Command parser method to set the etch file to compile.
     * @param cp
     * @param param
     * @param value path of etch file to compile
     * @return true if file added to list, false otherwise.
     */
    public boolean setFile( CommandParser cp, Parameter param, File value )
    {
        sourceFiles.add(value);
        return true;
    }
    private List<File> sourceFiles;

    @Override
    protected void run() throws Exception
    {
        if (bindingName == null)
            throw new Exception ("No binding specified.");
        
        // Instantiate a new compiler instance
        EtchCompiler etchCompiler = new EtchCompiler( setupClassLoader( null ) );
        etchCompiler.setQuiet( quiet );

        // VALIDATE

        try
        {
            etchCompiler.setBindingName(bindingName);
            etchCompiler.setIgnoreEnvironmentIncludePath(ignoreEnvironmentIncludePath);        
            etchCompiler.setMixinSuppressed(mixinSuppressed);
            etchCompiler.setFlattenPackages(!noFlattenPackages);        
            etchCompiler.setOverwriteTemplate(false);
            etchCompiler.setIgnoreLocal(ignoreLocal);
            etchCompiler.setIgnoreGlobal(ignoreGlobal);
        
            for (File f: includePath)
                etchCompiler.addIncludePath(f);
            
            for (String w: what)
                etchCompiler.addWhat(w);

            if (outputDirectory != null)
                etchCompiler.setOutputDirectory(outputDirectory);
        
            if (userWordList != null)
                etchCompiler.setUserWordsList(userWordList);
        
            if (mixinOutputDirectory != null)
                etchCompiler.setMixinOutputDirectory(mixinOutputDirectory);
        
            if (templateOutputDirectory != null)
                etchCompiler.setTemplateOutputDirectory(templateOutputDirectory);
        }
        catch (Exception e)
        {
            throw new Exception("Invalid parameter passed to the compiler:\n" + e.getMessage());
        }
        
        // EXECUTE 
        
        boolean status = true;
        for (File name: sourceFiles)
        {
            try
            {
                etchCompiler.validateEtchFile(name);
                etchCompiler.compile(name);
            }
            catch ( Exception e )
            {
                System.out.println(String.format("Error Compiling %s: %s", name.getName(), e.getMessage()));
                e.printStackTrace();
                status = false;
            }
        }
        if (!status)
        {
            throw new Exception("One or more .etch files failed to compile.");
        }
    }
}
