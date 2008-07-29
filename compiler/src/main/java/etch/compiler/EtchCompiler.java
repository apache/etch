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
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.regex.Pattern;

import etch.compiler.ast.Module;

public class EtchCompiler
{

    private static String ETCH_INCLUDE_PATH = "ETCH_INCLUDE_PATH";
    private static Integer ETCH_OUTPUT_COUNTER = 1;

    /**
     * Instantiate a new compiler
     */
    public EtchCompiler( ClassLoader cl )
    {
    	this.cl = cl;
    	
        // Default options
        ignoreGlobal = false;
        ignoreLocal = false;
        ignoreEnvIncludePath = false;
        
        initLogHandler();
        initIncludePath();
    }
    
    private final ClassLoader cl;

    private File getTempOutputDirectory() throws Exception
    {
        // TODO check
        String suffix = String.format("%d", ETCH_OUTPUT_COUNTER++);
        File tmpOutputDir = File.createTempFile("etch-outputdir", suffix);
        if (!tmpOutputDir.delete())
        {
            String msg = String.format("Could not initialize temporary output directory (delete): %s \n", tmpOutputDir);
            throw new Exception(msg);
        }
        if (!tmpOutputDir.mkdirs())
        {
            String msg = String.format("Could not initialize temporary output directory (create): %s \n", tmpOutputDir);
            throw new Exception(msg);
        }
        tmpOutputDir.deleteOnExit();
        String msg = String.format("Created temporary output directory: %s\n", tmpOutputDir);
        logHandler.logMessage( LogHandler.LEVEL_INFO, null, msg);
        return tmpOutputDir;
    }

    // logHandler

    private LogHandler logHandler;
    
    private void initLogHandler()
    {
        logHandler = new CompilerLogHandler( "Command" );
    }

    public void setQuiet(boolean value)
    {
        logHandler.setQuiet(value);
    }
    
    // includePath
    
    private List<File> includePath;
    private List<File> envIncludePath;

    private void initIncludePath()
    {
        includePath    = new LinkedList<File>();
        envIncludePath = new LinkedList<File>();
        
        // Harvest include path from envvar 'ETCH_INCLUDE_PATH'
        
        String tempIncludePathString = System.getenv(ETCH_INCLUDE_PATH);
        if (tempIncludePathString != null)
        {
            envIncludePath = new LinkedList<File>();
            StringTokenizer tempPath = new StringTokenizer(tempIncludePathString, File.pathSeparator);
            while (tempPath.hasMoreTokens())
            {
                File temp = new File(tempPath.nextToken());
                if (temp.exists() && temp.isDirectory())
                {
                    try
                    {
                        envIncludePath.add(temp.getCanonicalFile());                        
                    }
                    catch ( IOException e )
                    {
                        // ignore
                    }
                }
            }
        }
    }

    /**
     * add a directory to the ETCH_INCLUDE_PATH
     *
     * @param value path to add
     */
    public void addIncludePath(File value)
    {
        if (value.exists() && value.isDirectory())
        {
            try
            {
                includePath.add(value.getCanonicalFile());                
            }
            catch ( IOException e )
            {
                // ignore
            }
        }
    }

    /**
     * return a list of the directories in the Etch include path
     *
     */
    public List<File> getIncludePath()
    {
        return getIncludePath(null);
    }
    
    /**
     * return a list of the directories in the Etch include path, inserting 'workingDirectory' at the beginning
     *
     * @param workingDirectory use this value as the workingDirectory path
     */
    public List<File> getIncludePath(File workingDirectory)
    {
        // Setup Etch include path from the environment variable, commandline options
        // and the working directory of the etch file itself.
        List<File> effectiveIncludePath = new LinkedList<File>();
        
        if (!ignoreEnvIncludePath)
            effectiveIncludePath.addAll(envIncludePath);
        
        effectiveIncludePath.addAll(includePath);
        if (workingDirectory != null)
        {
            try
            {
                effectiveIncludePath.add(0, workingDirectory.getCanonicalFile());                
            }
            catch ( IOException e )
            {
                String msg = String.format("Could not add working directory %s to the includePath \n", workingDirectory);
                logHandler.logMessage ( LogHandler.LEVEL_WARNING, null, msg );
            }
        }
        
        return effectiveIncludePath;
    }

    // outputDirectory
    
    private File outputDir;

    /**
     * set the output directory for generated source
     *
     * @param value directory to output source
     */
    public void setOutputDirectory(File value) throws Exception
    {
        if (value == null)
        {
            outputDir = null;
            return;
        }
            
        if (value.exists() && (!value.isDirectory() || !value.canWrite()))
        {
            // Not a directory, not writable
            // TODO better exception name
            throw new Exception("Specified output directory: '" + value.toString() + "' is not a directory or not writable \n");
        }
        outputDir = value.getCanonicalFile();
    }

    /**
     * return the name of the output directory
     *
     */
    public File getOutputDirectory()
    {
        return outputDir;
    }

    // templateOutputDirectory
    
    private File templateOutputDir;
    
    /**
     * set an alternative output directory for generated template source, i.e. IMPL and MAIN files
     *
     * @param value directory to output template source
     */
    public void setTemplateOutputDirectory(File value) throws Exception
    {
        if (value == null)
        {
            templateOutputDir = null;
            return;
        }
            
        if (!value.isDirectory() || !value.canWrite())
        {
            // Not a directory, not writable
            // TODO better exception name
            throw new Exception("Specified output directory: '" + value.toString() + "' is not a directory or not writable \n");
        }
        templateOutputDir = value.getCanonicalFile();
    }
    
    /**
     * return the name of the template output directory
     *
     */
    public File getTemplateOutputDirectory()
    {
        return templateOutputDir;
    }
    
    private boolean overwriteTemplate;

    /**
     * set the value of the OverwriteTemplate flag
     *
     * @param value
     */
    public void setOverwriteTemplate(boolean value)
    {
        overwriteTemplate = value;
    }

    /**
     * return the value of the OverwriteTemplate flag
     */
    public boolean getOverwriteTemplate()
    {
        return overwriteTemplate;
    }    

    // binding

    private String   bindingName;
	private Class<?> bindingClass;
    
    /**
     * set the name of the binding
     * The compiler will assume the binding class is 'etch.bindings.<name>.compiler.Compiler'
     *
     * @param value name of the binding (e.g., java, csharp, xml, ...)
     */
    public void setBindingName(String value)
    {
        bindingName = value;
    }
    
    /**
     * get the name of the binding
     *
     */
    public String getBindingName()
    {
        return bindingName;
    }
    
    /**
     * get the fully-qualified class name of the binding
     *
     */
    public String getBindingClassName()
    {
        if (bindingName == null)
            return null;
                        
        // TODO perhaps allow user to specifiy a fully qualified classname for the binding as an option?
        return String.format("etch.bindings.%s.compiler.Compiler", bindingName);
    }
    
    
    private boolean initBindingClass() throws Exception
    {

        if (bindingClass != null)
            return true;

        try
        {   
            bindingClass = cl.loadClass(getBindingClassName());
            // bindingClass = Class.forName(getBindingClassName());
            return true;
        }
        catch ( ClassNotFoundException e )
        {
            String msg = String.format("Binding '" + getBindingName() + "' could not be loaded; class '" + getBindingClassName() + "' not in classpath.\n");
            // TODO better exception
            e.printStackTrace();
            throw new Exception(msg);
        }
    }
    
    private Class<?> getBindingClass() throws Exception
    {
        initBindingClass();
        return bindingClass;
    }
    
    private Backend getBindingClassInstance() throws Exception
    {
        initBindingClass();
        return (Backend) bindingClass.newInstance();    
    }

    // what

    private Set<String> what = new HashSet<String>();

    /**
     * add to the list of what the compiler should produce, (e.g. 'client','server','helper')
     *
     * @param what a valid compiler product
     */
    public void addWhat(String value) throws Exception
    {
        what.add(value);
    }

    /**
     * return the set of compiler products to be produced
     *
     */
    public Set<String> getWhat()
    {
        return what;
    }

    // ignoreGlobal

    private boolean ignoreGlobal;

    /**
     * set the value of the IgnoreGlobal flag
     *
     * @param value
     */
    public void setIgnoreGlobal(boolean value)
    {
        ignoreGlobal = value;
    }

    /**
     * return the value of the IgnoreGlobal flag
     */
    public boolean getIgnoreGlobal()
    {
        return ignoreGlobal;
    }

    // ignoreLocal

    private boolean ignoreLocal;

    /**
     * set the value of the IgnoreLocal flag
     *
     * @param value
     */
    public void setIgnoreLocal(boolean value)
    {
        ignoreLocal = value;
    }

    /**
     * return the value of the IgnoreLocal flag
     */
    public boolean getIgnoreLocal()
    {
        return ignoreLocal;
    }

    // ignoreEnvironmentIncludePath

    private boolean ignoreEnvIncludePath;

    /**
     * set the value of the IgnoreEnvironmentIncludePath flag
     *
     * @param value
     */
    public void setIgnoreEnvironmentIncludePath(boolean value)
    {
        ignoreEnvIncludePath = value;
    }

    /**
     * return value of the IgnoreEnvironmentIncludePath flag
     */
    public boolean getIgnoreEnvironmentIncludePath()
    {
        return ignoreEnvIncludePath;
    }

    // userWordsList

    private String userWordsList;

    /**
     * specify a Users Words list
     *
     * @param value path to Users Words list file
     */
    public void setUserWordsList(File value) throws Exception
    {
        if (!value.isFile() || !value.canRead())
        {
            // Not a file or not readable
            // TODO better Exception
            throw new Exception("The user words list specified: '" + value.toString() + "' is not a file or is not readable");
        }
        userWordsList = value.getCanonicalFile().getPath();
    }

    /**
     * return the path to the User Words list file
     */
    public String getUserWordsList()
    {
        return userWordsList;
    }

    // isMixinSuppressed
    
    private boolean isMixinSuppressed = false;

    /**
     * set the value of the MixinSuppressed flag
     *
     * @param value
     */
    public void setMixinSuppressed(boolean value)
    {
        isMixinSuppressed = value;
    }

    /**
     * return the value of the MixinSuppressed flag
     */
    public boolean getNoMixinArtifacts()
    {
        return isMixinSuppressed;
    }

    // mixinOutputDir

    private File mixinOutputDir;

    /**
     * set the path for mixin outputs
     *
     * @param value path
     */
    public void setMixinOutputDirectory(File value) throws Exception
    {
        if (!value.isDirectory() || !value.canWrite())
        {
            // Not a directory or cannot write
            throw new Exception(String.format("Mixin output directory %s is not found or is not writable", value));
        }
        mixinOutputDir = value.getCanonicalFile();
    }

    /**
     * get the value of the mixin output directory
     *
     */
    public File getMixinOutputDirectory()
    {
        return mixinOutputDir;
    }

    // flattenPackages

    private boolean flattenPackages = true;

    /**
     * set the value of the FlattenPackages flag
     *
     * @param value
     */
    public void setFlattenPackages(boolean value)
    {
        flattenPackages = value;
    }

    /**
     * get the value of the FlattenPackages attribute
     */
    public boolean getFlattenPackages()
    {
        return flattenPackages;
    }
    
    // sourceFiles
    
    /**
     * validate that the path specified is a .etch file
     *
     * @param value filename
     */
    public void validateEtchFile(File value) throws Exception
    {
        
        if (!value.isFile() || !value.canRead())
        {
            String msg = String.format("File '%s' does not exist or cannot be read.", value);
            // TODO better exception
            throw new Exception(msg);
        }
        
        String s = value.getName();
        if (!s.endsWith(".etch"))
        {
            String msg = String.format("File '%s' must have .etch extension", value);
            // TODO better exception
            throw new Exception(msg);
        }
    }
    
    /****************************/
    
    /**
     * Compile an .etch file, assume workingDirectory is the same as the file's parent directory
     *
     * @param name filename
     */
    public void compile(File name) throws Exception
    {
        name = name.getCanonicalFile();
        validateEtchFile(name);
        
        FileInputStream is = new java.io.FileInputStream(name);
        
        String msg = String.format("Compiling %s ...\n", name);
        logHandler.logMessage( LogHandler.LEVEL_INFO, null, msg);
        if (name.getParentFile() != null)
            compile((InputStream)is, name.getParentFile());
        else
            compile((InputStream)is, name.getAbsoluteFile().getParentFile());
    }

    /**
     * Compile an .etch input stream
     *
     * @param is stream
     */
    public void compile(InputStream is) throws Exception
    {
        // TODO null is not valid, pass a legitimate dir or change the other method
        compile(is, null);
    }

    /**
     * Compile an .etch input stream, specify the workingDirectory
     *
     * @param is stream
     * @param workingDirectory working directory
     */
    public void compile(InputStream is, File workingDirectory) throws Exception
    {   
             
        CmdLineOptions options = new CmdLineOptions();
        workingDirectory = workingDirectory.getCanonicalFile();
        
        // Configure EtchHelper
        // TODO really need to re-factor how compiler-directive stuff in CmdLineOptions is passed around
//        EtchHelper.cl = options;
        options.lh = logHandler;

        // Setup options
        //  if there are any problems, inconsistencies in the option set, Exceptions will be thrown
        options.includePath = getIncludePath(workingDirectory);
        options.bindingClass = getBindingClass();
        options.what = getWhat();
        options.outputDir = getTempOutputDirectory();
        // options.outputDir = getOutputDirectory();
        options.mixinOutputDir = getMixinOutputDirectory();
        options.ignoreGlobal = getIgnoreGlobal();
        options.ignoreLocal = getIgnoreLocal();
        options.userWordsList = getUserWordsList();
        options.noMixinArtifacts = getNoMixinArtifacts();
        options.noFlattenPackages = !getFlattenPackages();

        // Instantiate a new backend and parser
        final Backend b = getBindingClassInstance();
            
        final EtchGrammar parser = new EtchGrammar(b, is);
        
        final Module m;
        
        // Parse .etch source
        try
        {
            m = parser.module( options );
            logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Parsed Ok.\n");
            m.check();
            logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Checked Ok.\n");
        }
        catch ( ParseException e )
        {
            String fmt = e.getMessage();
            logHandler.logMessage( LogHandler.LEVEL_ERROR, e.currentToken, fmt);
            // TODO better exception
            throw new Exception(e.currentToken + " " + fmt);
        }
        catch ( Exception e)
        {
            String fmt = e.getMessage();
            logHandler.logMessage( LogHandler.LEVEL_ERROR, null, fmt);
            // TODO better exception
            throw e;
        }
        
        // Code generation
        
        try
        {
            // TODO report work lists?
            logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Generating Resources... \n");
            
            // TODO integrate includePath with code generation.
            b.generate(m, options, logHandler);
        }
        catch ( Exception e )
        {
            String fmt = e.getMessage();
            // TODO better exception
            e.printStackTrace();
            throw new Exception(fmt);
        }
        logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Generating Complete. \n");
        
        // Move generated code to target location
        //   the parser will set 'isMixinPresent' if a "mixin" directive is in the .etch file
        // TODO would be nice if the backend just put things in the right place from the start .. sigh re-factor later        
        moveGeneratedCode(options.outputDir, options.isMixinPresent, workingDirectory);

        // Complete

        logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Compile Done.\n");
        
        // return m;
    }

    private static class GeneratedFiles
    {
        private Pattern rest         = Pattern.compile(".*");
        private String IMPL_PATTERN  = ".*Impl.+(Server|Client)..+$";
        private String MAIN_PATTERN  = ".*Main.+(Client|Listener)..+$";

        private File basedir;

        public GeneratedFiles(File basedir)
        {
            this.basedir = basedir;
        }
            
        public List<String> relativeFiles()
        {
            return strip(find(null));
        }
        
        public List<String> templateFiles()
        {
            List<String> r = new LinkedList<String>();
            
            for (String f: relativeFiles())
            {
                if (f.matches(IMPL_PATTERN) || f.matches(MAIN_PATTERN))
                    r.add(f);
            }
            return r;
        }
        
        public List<String> nonTemplateFiles()
        {
            List<String> r = new LinkedList<String>();
            
            for (String f: relativeFiles())
            {
                if (!f.matches(IMPL_PATTERN) && !f.matches(MAIN_PATTERN))
                    r.add(f);
            }
            return r;
        }
        
        private List<String> strip(List<File> files)
        {
            List<String> s = new LinkedList<String>();

            for (File n: files)
            {
                Scanner t = new Scanner(n.getPath());
                t.skip(Pattern.quote(basedir.getPath()));
                s.add(t.next(rest));
            }
            return s;
        }

        private List<File> find(File root)
        {
            List<File> lst = new LinkedList<File>();

            if (root == null)
                root = basedir;

            for (File n: root.listFiles())
            {
                if (n.isDirectory())
                {
                    lst.addAll(find(n));
                }
                else
                {
                    lst.add(n);
                }
            }
            return lst;
        }
    }

    private void moveFiles(File srcDir, File destDir, List<String> files, boolean overwrite) throws Exception
    {
        //
        for (String f: files)
        {
            File srcFile  = new File(srcDir, f);
            File destFile = new File(destDir, f);
            if (destFile.exists() && overwrite)
                destFile.delete();
                
            if (!destFile.exists())
            {
                File parent = new File(destFile.getParent());
                parent.mkdirs();
                srcFile.renameTo(destFile);
                logHandler.logMessage( LogHandler.LEVEL_INFO, null, "  Writing " + f + "\n");
            }
            else
            {
                logHandler.logMessage( LogHandler.LEVEL_INFO, null, "  No overwrite, skipping " + f + "\n");
                srcFile.delete();
            }
        }
    }

    private void moveGeneratedCode(File srcDir, boolean mixinPresent, File workingDirectory) throws Exception
    {

        File target = outputDir;
        File templateTarget = templateOutputDir;
        
        //  override output directory if mixin detected
        if (mixinPresent && mixinOutputDir != null)
            target = mixinOutputDir;
                
        if (target == null)
            target = workingDirectory;
        
        if (templateOutputDir == null)
            templateTarget = target;
        
        // move files
        GeneratedFiles gf = new GeneratedFiles(srcDir);
        
        logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Moving generated code to " + target.toString() + "\n");
        moveFiles(srcDir, target, gf.nonTemplateFiles(), true);
        
        logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Moving starter template code to " + templateTarget.toString() + "\n");
        moveFiles(srcDir, templateTarget, gf.templateFiles(), overwriteTemplate);
        
    }
}
