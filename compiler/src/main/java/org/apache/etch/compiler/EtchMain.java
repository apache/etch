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

import java.io.File;
import java.util.StringTokenizer;

import org.apache.etch.util.cmd.CommandParser;
import org.apache.etch.util.cmd.Option;
import org.apache.etch.util.cmd.Parameter;
import org.apache.etch.util.cmd.Program;


/**
 * The main command line interface for the etch compiler.
 */
public class EtchMain extends Program
{
	/**
     * @param args
     * @throws Exception 
     */
    public static void main( String[] args ) throws Exception
    {
    	new EtchMain().doMain( args );
    }
    
    /**
     * Runs the main program, parsing command line arguments and performing
     * the action.
     * @param args
     * @throws Exception
     */
    public void doMain( String[] args ) throws Exception
    {
    	try
    	{
    		main( this, args );
    	}
        catch ( Throwable e )
        {
        	e.printStackTrace();
            exit( 3, toString(), e.toString(), true );
            return;
        }
    }

    /**
     * Constructs the etch parser / compiler driver.
     */
    public EtchMain()
    {
        // nothing to do.
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
        cp.defineFileOption( "-d|--output-dir", "outputDir", "setOutputDir",
            "output directory for compiler generated files",
            Option.SINGLETON, null, null );

        cp.defineFileOption( "-m|--mixin-output-dir", "outputDir", "setMixinOutputDir",
            "output directory for compiler generated files for mixins",
            Option.SINGLETON, null, null );

        cp.defineFileOption( "-t|--template-output-dir", "outputDir", "setTemplateOutputDir",
            "output directory for compiler generated user editable template files",
            Option.SINGLETON, null, null );
            
        cp.defineStringOption( "-b|--binding", "binding", "setBinding",
            "specifies a target language binding to generate",
            Option.REQUIRED|Option.SINGLETON, null, null );

        cp.defineStringOption( "-w|--what", "what", "addWhat",
            "specifies what files to generate (depends upon binding, try -w help for more info; separate with '"+CmdLineOptions.WHAT_DELIMETER+"')",
            Option.NONE, null, null );

        cp.defineNullOption( "-g|--ignore-global-words-list", "ignoreGlobalWordsList",
            "ignore the global reserved words list",
            Option.SINGLETON );

        cp.defineNullOption( "-l|--ignore-local-words-list", "ignoreLocalWordsList",
            "ignore the local (binding-specific) reserved words list",
            Option.SINGLETON );

        cp.defineFileOption( "-W|--user-words-list", "wordList", "setUserWordsList",
            "file name of a user-specified reserved words list",
            Option.SINGLETON, null, null );

        cp.defineStringOption( "-I|--include-path", "includePath", "addIncludePath",
            "adds search directories for included or mixed in etch files (separate elements with system path separator '"+File.pathSeparator+"')",
            Option.NONE, null, null );

        cp.defineNullOption( "-i|--ignore-include-path", "ignoreIncludePath",
            "ignore the "+CmdLineOptions.ETCH_INCLUDE_PATH+" environment variable",
            Option.SINGLETON );

        cp.defineNullOption( "-n|--no-mixin-artifacts", "noMixinArtifacts",
            "mixin artifacts should not be generated",
            Option.SINGLETON );

        cp.defineNullOption( "-q|--quiet", "quiet",
            "only report problems",
            Option.SINGLETON );

        cp.defineNullOption( "-v|--version", "doVersion",
            "show the version and exit",
            Option.SINGLETON );

        cp.defineNullOption( "-f|--no-flatten-packages", "noFlattenPackages",
            "namespace directory tree should not be flattened",
            Option.SINGLETON );
        
        cp.defineNullOption( "--testing", "setTesting",
        	"",
        	Option.HIDDEN );

        cp.defineFileParameter( "sourceFile", "setSourceFile",
            "etch source file to compile", true, false, null );
    }
    
    private final CmdLineOptions clo = new CmdLineOptions();

    /**
     * Sets the output directory of compiler generated files.
     * @param cp
     * @param option
     * @param token
     * @param value the output directory
     */
    public void setOutputDir( CommandParser cp, Option option, String token,
        File value )
    {
        clo.outputDir = value;
    }
    
    /**
     * Sets the output directory for user editable template files.
     * @param cp
     * @param option
     * @param token
     * @param value the template output directory
     */
    public void setTemplateOutputDir( CommandParser cp, Option option, String token,
        File value )
    {
    	clo.templateOutputDir = value;
    }

    /**
     * Sets the binding.
     * @param cp
     * @param option
     * @param token
     * @param value the binding
     */
    public void setBinding( CommandParser cp, Option option, String token,
        String value )
    {
    	clo.binding = value;
    }

    /**
     * Adds to the set of what needs to be generated.
     * @param cp
     * @param option
     * @param token
     * @param value the list of what needs to be generated.
     */
    public void addWhat( CommandParser cp, Option option, String token,
        String value )
    {
        StringTokenizer st = new StringTokenizer( value, CmdLineOptions.WHAT_DELIMETER );
        while (st.hasMoreElements()) 
        	clo.what.add(st.nextToken().toUpperCase());
    }

    /**
     * Sets the ignore the globally reserved words list flag.
     * @param cp
     * @param option
     * @param token
     */
    public void ignoreGlobalWordsList(CommandParser cp, Option option, String token)
    {
    	clo.ignoreGlobalWordsList = true;
    }

    /**
     * Sets the ignore the locally reserved words list flag.
     * @param cp
     * @param option
     * @param token
     */
    public void ignoreLocalWordsList(CommandParser cp, Option option, String token)
    {
    	clo.ignoreLocalWordsList = true;
    }

    /**
     * Sets the file name of the user-defined reserved words list.
     * @param cp
     * @param option
     * @param token
     * @param value the file name of the user-defined reserved words list
     */
    public void setUserWordsList(CommandParser cp, Option option, String token,
        File value)
    {
    	clo.userWordsList = value;
    }

    /**
     * Adds to the include path.
     * @param cp
     * @param option
     * @param token
     * @param value path list to append to the current include path.
     */
    public void addIncludePath( CommandParser cp, Option option, String token,
        String value )
    {
        StringTokenizer includeTokens = new StringTokenizer(value,
            File.pathSeparator);

        while(includeTokens.hasMoreTokens())
        {
            File temp = new File(includeTokens.nextToken());
            clo.includePath.add(temp);
        }
    }

    /**
     * Sets the ignore the include path from the environment flag.
     * @param cp
     * @param option
     * @param token
     */
    public void ignoreIncludePath(CommandParser cp, Option option,
        String token)
    {
    	clo.ignoreIncludePath = true;
    }

    /**
     * Sets the quiet flag.
     * @param cp
     * @param option
     * @param token
     */
    public void quiet( CommandParser cp, Option option, String token )
    {
    	clo.quiet = true;
    }

    /**
     * Shows the version and exits.
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
     * Sets the suppress mixin artifacts flag.
     * @param cp
     * @param option
     * @param token
     */
    public void noMixinArtifacts( CommandParser cp, Option option, String token )
    {
    	clo.noMixinArtifacts = true;
    }

    /**
     * Sets the output directory for mixins.
     * @param cp
     * @param option
     * @param token
     * @param value the output directory for mixin
     */
    public void setMixinOutputDir( CommandParser cp, Option option, String token,
        File value )
    {
    	clo.mixinOutputDir = value;
    }

    /**
     * Sets the no flatten packages flag.
     * @param cp
     * @param option
     * @param token
     */
    public void noFlattenPackages( CommandParser cp, Option option, String token )
    {
    	clo.noFlattenPackages = true;
    }
    
    /**
     * Sets the hidden testing flag.
     * @param cp
     * @param option
     * @param token
     */
    public void setTesting( CommandParser cp, Option option, String token )
    {
    	clo.testing = true;
    	clo.lh = new ListLogHandler();
    	testingClo = clo;
    }
    
    /**
     * If --testing is on the command line, the CmdLineOptions is saved
     * here for reference by unit testing programs.
     */
    public CmdLineOptions testingClo;

    /**
     * Sets the etch source file to compile.
     * @param cp the command parser
     * @param param
     * @param value path of etch file to compile
     */
    public void setSourceFile( CommandParser cp, Parameter param, File value )
    {
    	clo.sourceFile = value;
    }

    @Override
    protected void run() throws Exception
    {
        // Instantiate a new compiler instance and run it.
    	ClassLoader cl = EtchCompiler.setupClassLoader( null );
        EtchCompiler etchCompiler = new EtchCompiler( cl );
        
        etchCompiler.run( clo );
        
        if (clo.lh.hasError())
        	exit( 3, "EtchMain", "errors during compile", false );
    }
}
