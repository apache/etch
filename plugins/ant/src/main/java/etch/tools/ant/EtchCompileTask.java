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

package etch.tools.ant;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.DirSet;
import org.apache.tools.ant.types.FileSet;
import org.apache.tools.ant.types.Path;

import etch.compiler.EtchCompiler;
import etch.compiler.EtchMain;

/**
 * Ant task for compiling Etch sources files.
 * 
 */
public class EtchCompileTask extends Task
{
    private static String WHO_DELIMITER = ", ";
    
    /**
     * Creates a new ant task for running the Etch compiler.
     */
    public EtchCompileTask()
    {
        includePath  = new LinkedList<File>();
        what         = new LinkedList<String>();
    }

    /////////

    /**
     * Adds a set of directories to scan for includes
     *
     * @param dirset
     */
    public void addIncludes( DirSet dirset )
    {
        DirectoryScanner ds = dirset.getDirectoryScanner( getProject() );
        for (String dir : ds.getIncludedDirectories())
        {
            includePath.add(new File(ds.getBasedir(), dir));
        }
    }
    private List<File> includePath;

    /**
     * Specifies a source file to compile.
     *
     * @param file
     */
    public void setFile( File file )
    {
    	if (sourceFile != null)
    		throw new BuildException( "only one file at a time, please" );
    	System.out.println( "setFile: "+file );
        sourceFile = file;
    }
    
    private File sourceFile;

    /**
     * Sets the output directory for compiler generated files.
     *
     * @param output
     */
    public void setOutput( File output )
    {
        outputDirectory = output;
    }
    private File outputDirectory = null;

    /**
     * Sets the binding name
     *
     * @param binding
     */
    public void setBinding( String binding )
    {
        bindingName = binding;
    }
    private String bindingName = null;

    /**
     * Sets the set of pieces the compiler should generate
     *
     * @param what
     */
    public void setWhat( String value )
    {
        StringTokenizer st = new StringTokenizer(value, WHO_DELIMITER );
        while (st.hasMoreElements())
            what.add(st.nextToken().toUpperCase());            
    }
    private List<String> what;
    
    /**
     * Set to ignore the globally reserved words list
     *
     * @param ignoreglobal
     */
    public void setIgnoreglobal( boolean ignoreglobal )
    {
        ignoreGlobal = ignoreglobal;
    }
    private boolean ignoreGlobal = false;
    
    /**
     * Set to ignore the locally reserved words list
     *
     * @param ignorelocal
     */
    public void setIgnorelocal( boolean ignorelocal )
    {
        ignoreLocal = ignorelocal;
    }
    private boolean ignoreLocal = false;
    
    /**
     * Specify a user-defined reserved words list
     *
     * @param wordlist
     */
    public void setWordlist(File wordlist)
    {
        userWordList = wordlist;
    }
    private File userWordList = null;

    /**
     * Ignore the ETCH_INCLUDE_PATH envvar
     *
     * @param ignoreenv
     */
    public void setIgnoreenv( boolean ignoreenv )
    {
        ignoreEnvironmentIncludePath = ignoreenv;
    }
    private boolean ignoreEnvironmentIncludePath = false;
    
    /**
     * Suppress mixin artifacts
     *
     * @param suppressmixin
     */
    public void setSuppressmixin( boolean suppressmixin )
    {
        suppressMixin = suppressmixin;
    }
    private boolean suppressMixin = false;
    
    /**
     * Set mixin output directory
     *
     * @param mixinoutput
     */
    public void setMixinoutput( File mixinoutput )
    {
        mixinOutputDirectory = mixinoutput;
    }
    private File mixinOutputDirectory = null;

    /**
     * Set to flatten packages
     *
     * @param flatten
     */
    public void setFlatten( boolean flatten )
    {
        flattenPackages = flatten;
    }
    private boolean flattenPackages = false;
    
    /**
     * set to force overwrite of user-modifiable templates
     *  Impl*Client, Impl*Server, Main*Client, and Main*Listener
     *
     * @param overwrite
     */
    public void setOverwritetemplate( boolean overwrite )
    {
        overwriteTemplate = overwrite;
    }
    private boolean overwriteTemplate = false;
    
    /**
     * set to path to output user-modifiable templates
     *  Impl*Client, Impl*Server, Main*Client, Main*Listener
     *
     * @param value
     */
    public void setTemplateoutput(File value)
    {
        templateOutputDirectory = value;
    }
    private File templateOutputDirectory = null;
    
    /**
     * Sets the path of the directory of jar files for the compiler bindings.
     * @param value
     */
    //public void setEtchLib(String value)
    //{
//    	System.out.println("etchLib = "+value);
    //	this.etchLib = value;
    //}
    
    //private String etchLib;

    /** 
     * sets etch.home
     *
     * @param value
     */
    public void setHome(File value)
    {
        this.etchHome = value;
    }
    private File etchHome;

    /** EXECUTION **/
    
    @Override
    /**
     * execute
     */
    public void execute() throws BuildException
    {
    	ClassLoader cl;
    	
    	try
		{
			cl = EtchMain.setupClassLoader(etchHome + File.separator + "lib");
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			throw new BuildException( "problem setting up etch compiler", e );
		}
    	
        EtchCompiler etchCompiler;
        
        try
        {
            // Instantiate a new compiler instance
            etchCompiler = new EtchCompiler(cl);
            etchCompiler.setQuiet( false );
        }
        catch (Exception e)
        {
            throw new BuildException(e.getMessage());
        }

        // VALIDATE

        if (bindingName == null)
            throw new BuildException ("No binding specified.");
            
        try
        {
            etchCompiler.setBindingName(bindingName);
            etchCompiler.setIgnoreEnvironmentIncludePath(ignoreEnvironmentIncludePath);        
            etchCompiler.setMixinSuppressed(suppressMixin);
            etchCompiler.setFlattenPackages(flattenPackages);        
            etchCompiler.setOverwriteTemplate(overwriteTemplate);
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
            throw new BuildException("Invalid parameter passed to the compiler:\n" + e.getMessage());
        }
        
        // EXECUTE 
        
        boolean status = true;
    	System.out.println( "compiling "+sourceFile );
        try
        {
            etchCompiler.validateEtchFile(sourceFile);
            etchCompiler.compile(sourceFile);
        }
        catch ( Exception e )
        { 
            String msg = String.format("Error Compiling %s: %s", sourceFile, e.getMessage());
            // TODO log appropriately
            System.out.println(msg);
            e.printStackTrace();
            status = false;
        }
        if (!status)
            throw new BuildException("One or more .etch files failed to compile.");
    }
}
