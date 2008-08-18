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
import java.util.StringTokenizer;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.DirSet;

import etch.compiler.CmdLineOptions;
import etch.compiler.EtchCompiler;

/**
 * Ant task for compiling Etch sources files.
 * 
 */
public class EtchCompileTask extends Task
{
    /**
     * Creates a new ant task for running the Etch compiler.
     */
    public EtchCompileTask()
    {
        // nothing to do.
    }
    
    private CmdLineOptions clo = new CmdLineOptions();

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
            clo.includePath.add(new File(ds.getBasedir(), dir));
    }

    /**
     * Specifies a source file to compile.
     *
     * @param file
     */
    public void setFile( File file )
    {
    	if (clo.sourceFile != null)
    		throw new BuildException( "only one file at a time, please" );
        clo.sourceFile = file;
    }

    /**
     * Sets the output directory for compiler generated files.
     *
     * @param output
     */
    public void setOutput( File output )
    {
        clo.outputDir = output;
    }

    /**
     * Sets the binding name
     *
     * @param binding
     */
    public void setBinding( String binding )
    {
    	clo.binding = binding;
    }

    /**
     * Sets the set of pieces the compiler should generate
     *
     * @param what
     */
    public void setWhat( String what )
    {
        StringTokenizer st = new StringTokenizer(what, CmdLineOptions.WHAT_DELIMETER );
        while (st.hasMoreElements())
            clo.what.add(st.nextToken().toUpperCase());            
    }
    
    /**
     * Set to ignore the globally reserved words list
     *
     * @param ignoreglobal
     */
    public void setIgnoreglobal( boolean ignoreglobal )
    {
    	clo.ignoreGlobalWordsList = ignoreglobal;
    }
    
    /**
     * Set to ignore the locally reserved words list
     *
     * @param ignorelocal
     */
    public void setIgnorelocal( boolean ignorelocal )
    {
    	clo.ignoreLocalWordsList = ignorelocal;
    }
    
    /**
     * Specify a user-defined reserved words list
     *
     * @param wordlist
     */
    public void setWordlist(File wordlist)
    {
    	clo.userWordsList = wordlist;
    }

    /**
     * Ignore the ETCH_INCLUDE_PATH envvar
     *
     * @param ignoreincludepath
     */
    public void setIgnoreincludepath( boolean ignoreincludepath )
    {
    	clo.ignoreIncludePath = ignoreincludepath;
    }
    
    /**
     * Do not generate mixin artifacts
     *
     * @param nomixinartifacts
     */
    public void setNomixinartifacts( boolean nomixinartifacts )
    {
    	clo.noMixinArtifacts = nomixinartifacts;
    }
    
    /**
     * Set mixin output directory
     *
     * @param mixinoutput
     */
    public void setMixinoutput( File mixinoutput )
    {
    	clo.mixinOutputDir = mixinoutput;
    }

    /**
     * Set to not flatten packages
     *
     * @param noflattenpackages
     */
    public void setNoflattenpackages( boolean noflattenpackages )
    {
    	clo.noFlattenPackages = noflattenpackages;
    }
    
    /**
     * Set output directory for compiler generated user editable files.
     *
     * @param tmploutputdir
     */
    public void setTemplateoutput(File tmploutputdir)
    {
        clo.templateOutputDir = tmploutputdir;
    }
    
    /**
     * Sets quiet flag.
     * @param quiet
     */
    public void setQuiet(boolean quiet)
    {
    	clo.quiet = quiet;
    }

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
			cl = EtchCompiler.setupClassLoader( etchHome );
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			throw new BuildException( "problem setting up etch compiler class loader", e );
		}
    	
        EtchCompiler etchCompiler;
        
        try
        {
            // Instantiate a new compiler instance
            etchCompiler = new EtchCompiler( cl );
        }
        catch (Exception e)
        {
        	e.printStackTrace();
            throw new BuildException( "problem setting up etch compiler", e );
        }
        
        try
        {
        	etchCompiler.run( clo );
        }
        catch ( Exception e )
        {
        	e.printStackTrace();
        	throw new BuildException( "problem running etch compiler", e );
        }

        // VALIDATE

//        if (bindingName == null)
//            throw new BuildException ("No binding specified.");
//            
//        try
//        {
//            etchCompiler.setBindingName(bindingName);
//            etchCompiler.setIgnoreEnvironmentIncludePath(ignoreEnvironmentIncludePath);        
//            etchCompiler.setMixinSuppressed(suppressMixin);
//            etchCompiler.setFlattenPackages(!noFlattenPackages);        
//            etchCompiler.setOverwriteTemplate(overwriteTemplate);
//            etchCompiler.setIgnoreLocal(ignoreLocal);
//            etchCompiler.setIgnoreGlobal(ignoreGlobal);
//        
//            for (File f: includePath)
//                etchCompiler.addIncludePath(f);
//            
//            for (String w: what)
//                etchCompiler.addWhat(w);
//
//            if (outputDirectory != null)
//                etchCompiler.setOutputDirectory(outputDirectory);
//        
//            if (userWordList != null)
//                etchCompiler.setUserWordsList(userWordList);
//        
//            if (mixinOutputDirectory != null)
//                etchCompiler.setMixinOutputDirectory(mixinOutputDirectory);
//        
//            if (templateOutputDirectory != null)
//                etchCompiler.setTemplateOutputDirectory(templateOutputDirectory);
//        }
//        catch (Exception e)
//        {
//            throw new BuildException("Invalid parameter passed to the compiler:\n" + e.getMessage());
//        }
//        
//        // EXECUTE 
//        
//        boolean status = true;
//    	System.out.println( "compiling "+sourceFile );
//        try
//        {
//            etchCompiler.validateEtchFile(sourceFile);
//            etchCompiler.compile(sourceFile);
//        }
//        catch ( Exception e )
//        { 
//            String msg = String.format("Error Compiling %s: %s", sourceFile, e.getMessage());
//            // TODO log appropriately
//            System.out.println(msg);
//            e.printStackTrace();
//            status = false;
//        }
//        if (!status)
//            throw new BuildException("One or more .etch files failed to compile.");
    }
}
