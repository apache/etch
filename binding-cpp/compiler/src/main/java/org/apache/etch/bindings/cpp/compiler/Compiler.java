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

package org.apache.etch.bindings.cpp.compiler;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.CmdLineOptions;
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.LogHandler;
import org.apache.etch.compiler.Output;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;
import org.apache.etch.compiler.Version;
import org.apache.etch.compiler.ast.Builtin;
import org.apache.etch.compiler.ast.Enumx;
import org.apache.etch.compiler.ast.Except;
import org.apache.etch.compiler.ast.Item;
import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.MessageDirection;
import org.apache.etch.compiler.ast.Module;
import org.apache.etch.compiler.ast.MsgDirHelper;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Named;
import org.apache.etch.compiler.ast.ParamList;
import org.apache.etch.compiler.ast.Parameter;
import org.apache.etch.compiler.ast.ReservedWordChecker;
import org.apache.etch.compiler.ast.Service;
import org.apache.etch.compiler.ast.Struct;
import org.apache.etch.compiler.ast.Thrown;
import org.apache.etch.compiler.ast.TypeRef;
import org.apache.etch.compiler.opt.ToString;
import org.apache.etch.compiler.opt.ToString.FieldItem;
import org.apache.etch.compiler.opt.ToString.FmtItem;
import org.apache.etch.compiler.opt.ToString.StringItem;
import org.apache.etch.util.Assertion;
import org.apache.etch.util.Hash;
import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.ResourceNotFoundException;
import org.apache.velocity.runtime.RuntimeServices;
import org.apache.velocity.runtime.log.LogChute;

/**
 * Compiler is a helper class not only for Backend, but also for the templates.
 * They call methods here to perform "hard" tasks.
 */
public class Compiler extends Backend {

  private final static String tmplPath1 = "org/apache/etch/bindings/cpp/compiler/";

  private final static String tmplPath2 = "resources/org/apache/etch/bindings/cpp/compiler/";

  /**
   * Filename suffix for Header and Implementation files
   */
  private final static String fnSuffixH = ".h";
  private final static String fnSuffixI = ".cpp";

  /**
   * Folder Name - Include and Source
   */
  private final static String fnInclude = "include/";
  private final static String fnSource = "src/";

  private final static String VERSION = Version.VERSION + " / "
      + CompilerVersion.VERSION;

  /**
   * Constructs the Compiler. This is a helper class not only for Backend, but
   * also for the templates. They call methods here to perform "hard" tasks. * @throws
   * Exception
   */
  public Compiler() throws Exception {
    this.initVelocity();

    String[] path = { tmplPath1, tmplPath2 };

    // value factory templates
    this.vf_h_vm = this.getTemplate(path, "vf_h.vm");
    this.vf_cpp_vm = this.getTemplate(path, "vf_cpp.vm");

    // interface templates
    this.intf_h_vm = this.getTemplate(path, "intf_h.vm");
    this.intf_cpp_vm = this.getTemplate(path, "intf_cpp.vm");

    // interface remote templates
    this.remote_h_vm = this.getTemplate(path, "remote_h.vm");
    this.remote_cpp_vm = this.getTemplate(path, "remote_cpp.vm");

    // interface stub templates
    this.stub_h_vm = this.getTemplate(path, "stub_h.vm");
    this.stub_cpp_vm = this.getTemplate(path, "stub_cpp.vm");

    // helper templates
    this.helper_h_vm = this.getTemplate(path, "helper_h.vm");
    this.helper_cpp_vm = this.getTemplate(path, "helper_cpp.vm");

    this.readme_vm = this.getTemplate(path, "readme.vm");

    // Main template
    this.main_h_vm = this.getTemplate(path, "main_h.vm");
    this.main_cpp_vm = this.getTemplate(path, "main_cpp.vm");

    // Base template
    this.base_h_vm = this.getTemplate(path, "base_h.vm");
    this.base_cpp_vm = this.getTemplate(path, "base_cpp.vm");

    // Implementation template
    this.impl_h_vm = this.getTemplate(path, "impl_h.vm");
    this.impl_cpp_vm = this.getTemplate(path, "impl_cpp.vm");

    // Wireshark
    this.etch_wireshark_vm = this.getTemplate(path, "etch_wireshark.vm");

    // Keyword list
    this.local_kwd = this.getPath(path, "cppKeywords.kwd");
  }

  // interface templates
  private final Template intf_h_vm;
  private final Template intf_cpp_vm;

  // Base template
  private final Template base_h_vm;
  private final Template base_cpp_vm;

  // Value Factory
  private final Template vf_h_vm;
  private final Template vf_cpp_vm;

  // remote interface templates
  private final Template remote_h_vm;
  private final Template remote_cpp_vm;

  // interface stub templates
  private final Template stub_h_vm;
  private final Template stub_cpp_vm;

  // helper templates
  private final Template helper_h_vm;
  private final Template helper_cpp_vm;

  private final Template readme_vm;

  // Main template
  private final Template main_h_vm;
  private final Template main_cpp_vm;

  // Implementation template
  private final Template impl_h_vm;
  private final Template impl_cpp_vm;

  private final Template etch_wireshark_vm;

  private final String local_kwd;

  private LogHandler lh;

  /**
   * Initializes use of velocity engine and sets up resource loaders. * @throws
   * Exception
   */
  private void initVelocity() throws Exception {
    Velocity.setProperty(Velocity.RUNTIME_LOG_LOGSYSTEM, new MyLogger());
    Velocity.setProperty(Velocity.RESOURCE_LOADER, "file, class");

    Velocity.setProperty("file.resource.loader.description",
        "Velocity File Resource Loader");
    Velocity.setProperty("file.resource.loader.class",
        "org.apache.velocity.runtime.resource.loader.FileResourceLoader");
    Velocity.setProperty("file.resource.loader.path", ".");

    Velocity.setProperty("class.resource.loader.description",
        "Velocity Classpath Resource Loader");
    Velocity.setProperty("class.resource.loader.class",
        "org.apache.velocity.runtime.resource.loader.ClasspathResourceLoader");
    Velocity.init();
  }

  private static class MyLogger implements LogChute {
    private final LogHandler lh = null;

    @Override
    public void init(RuntimeServices rts) throws Exception {
      // ignore.
    }

    @Override
    public boolean isLevelEnabled(int level) {
      return level >= 2;
    }

    @Override
    public void log(int level, String msg) {
      if (level < 2)
        return;
      if (this.lh != null)
        this.lh.report(level == 2 ? LogHandler.LEVEL_WARNING
            : LogHandler.LEVEL_ERROR, null, msg);
      else
        System.out.printf("Velocity msg (%d): %s\n", level, msg);
    }

    @Override
    public void log(int level, String msg, Throwable e) {
      if (level < 2)
        return;
      if (this.lh != null)
        this.lh.report(level == 2 ? LogHandler.LEVEL_WARNING
            : LogHandler.LEVEL_ERROR, null, msg);
      else
        System.out.printf("Velocity msg (%d): %s: %s\n", level, msg, e);
    }
  }

  /**
   * @param path
   * @param fn
   * @return the velocity template
   * @throws Exception
   */
  private Template getTemplate(String[] path, String fn) throws Exception {
    ResourceNotFoundException rnfe = null;

    for (String p : path) {
      if (p == null)
        continue;

      // System.out.println( "trying to load template "+(p+fn) );
      try {
        if (Velocity.resourceExists(p + fn))
          return Velocity.getTemplate(p + fn);
      } catch (ResourceNotFoundException e) {
        rnfe = e;
      } catch (Exception e) {
        System.out.println("ignoring " + e);
      }
    }

    if (rnfe != null)
      throw rnfe;

    throw new ResourceNotFoundException("could not find resource: " + fn);
  }

  @Override
  public void generate(Module module, CmdLineOptions options) throws Exception {
    // java always wants to not flatten packages:
    options.noFlattenPackages = true;

    this.lh = options.lh;

    boolean ignoreGlobal = options.ignoreGlobalWordsList;
    boolean ignoreLocal = options.ignoreLocalWordsList;
    String userWords = options.userWordsList != null ? options.userWordsList
        .getPath() : null;
        Set<String> what = options.what;

        // Load the reserved words lists if any have been provided.
        Map<String, String> words = new HashMap<String, String>();
        if (!ignoreGlobal)
          this.mapWords(this.global_kwd, words);
        if (!ignoreLocal)
          this.mapWords(this.local_kwd, words);
        if (userWords != null)
          this.mapWords(userWords, words);

        // check for collisions with the reserved word list.
        ReservedWordChecker checker = new ReservedWordChecker(words, false, this.lh);
        module.treewalk(checker);
        if (!checker.ok()) {
          this.lh.report(LogHandler.LEVEL_ERROR, null,
              "Encountered errors during c generation.");
          return;
        }

        // ok, we're ready to generate code. make sure the
        // output directories exist.

        Output dir = options.output;
        Output templateDir = options.templateOutput;

        // generate code for each service.

        for (Service intf : module) {
          this.generate(intf, what, dir, templateDir);
        }
  }

  public boolean isGenerateImpl() {
    return this.isGenerateImpl;
  }

  public boolean isGenerateMain() {
    return this.isGenerateMain;
  }

  boolean isGenerateImpl;
  boolean isGenerateMain;

  private void generate(final Service intf, Set<String> what, Output dir,
      Output templateDir) throws Exception {
    what = this.populateWhat(what);

    if (what.isEmpty()) {
      // lh.logMessage( lh.LEVEL_ERROR, null, "User has selected NONE\n"
      // );
      return;
    }

    final MessageDirection msgDir = this.getMessageDirection(what);

    if (what.contains(WHAT_INTF)) {
      // Generate the value factory file.
      this.generateVf(intf, dir, msgDir);
      // Generate the interface, remote, and stub files.
      this.generateIntfRemoteStub(intf, dir, msgDir, MessageDirection.BOTH,
          false);
      this.generateIntfRemoteStub(intf, dir, msgDir, MessageDirection.SERVER,
          true);
      this.generateIntfRemoteStub(intf, dir, msgDir, MessageDirection.CLIENT,
          true);
      // Generate helper file.
      this.generateHelper(intf, dir, msgDir);
      // Generate base file.
      this.generateBase(intf, dir, msgDir);
      // Generate readme file.
      this.generateReadme(intf, dir, msgDir);
      this.generateKeywordList(intf, dir, msgDir, MessageDirection.BOTH, true);
    }

    // Generate main template file.
    if (what.contains(WHAT_MAIN))
      this.generateMain(intf, templateDir, msgDir);

    // Generate impl template file.
    if (what.contains(WHAT_IMPL))
      this.generateImpl(intf, templateDir, msgDir);
  }

  private void generateReadme(final Service intf, Output dir,
      final MessageDirection msgDir) throws Exception {
    this.doFile(dir, "readme-etch-cpp-files.txt", this.lh, new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateReadme(pw, intf, msgDir);
      }
    });
  }

  private void generateKeywordList(final Service intf, Output dir,
      final MessageDirection what, final MessageDirection mc,
      final boolean hasBaseClass) throws Exception {
    this.doFile(dir, this.getKeywordFilename(intf), this.lh, new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateKeywords(pw, intf, mc, hasBaseClass);
      }
    });
  }

  void generateKeywords(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
    context.put("hasBaseClass", hasBaseClass);
    this.etch_wireshark_vm.merge(context, pw);
  }

  public String getKeywordForWireshark(String fieldname) {
    int hash = Hash.hash(fieldname);
    return String.format("0x%08x", hash) + "," + fieldname;
  }

  private void generateVf(final Service intf, Output dir,
      final MessageDirection mc) throws Exception {
    this.doFile(dir, fnSource + this.getVfName(intf) + fnSuffixI, this.lh,
        new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateVfI(pw, intf, mc);
      }
    });
    this.doFile(dir, fnInclude + this.getVfName(intf) + fnSuffixH, this.lh,
        new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateVfH(pw, intf, mc);
      }
    });
  }

  private void generateHelper(final Service intf, Output dir,
      final MessageDirection msgDir) throws Exception {
    this.doFile(dir, fnSource + this.getHelperName(intf) + fnSuffixI, this.lh,
        new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateHelperI(pw, intf, msgDir);
      }
    });
    this.doFile(dir, fnInclude + this.getHelperName(intf) + fnSuffixH, this.lh,
        new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateHelperH(pw, intf, msgDir);
      }
    });
  }

  private void generateMain(final Service intf, Output dir,
      MessageDirection msgDir) throws Exception {
    if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.CLIENT) {
      this.doFile(dir, this.getMainName(intf, MessageDirection.CLIENT)
          + fnSuffixI, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateMainI(pw, intf, MessageDirection.CLIENT, false);
        }
      });
      this.doFile(dir, this.getMainName(intf, MessageDirection.CLIENT)
          + fnSuffixH, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateMainH(pw, intf, MessageDirection.CLIENT, false);
        }
      });
    }

    if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.SERVER) {
      this.doFile(dir, this.getMainName(intf, MessageDirection.SERVER)
          + fnSuffixI, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateMainI(pw, intf, MessageDirection.SERVER, false);
        }
      });
      this.doFile(dir, this.getMainName(intf, MessageDirection.SERVER)
          + fnSuffixH, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateMainH(pw, intf, MessageDirection.SERVER, false);
        }
      });
    }
  }

  private void generateBase(final Service intf, Output dir,
      MessageDirection msgDir) throws Exception {
    if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.CLIENT) {
      this.doFile(dir,
          fnSource + this.getBaseName(intf, MessageDirection.CLIENT)
          + fnSuffixI, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateBaseI(pw, intf, MessageDirection.CLIENT,
              false);
        }
      });
      this.doFile(dir,
          fnInclude + this.getBaseName(intf, MessageDirection.CLIENT)
          + fnSuffixH, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateBaseH(pw, intf, MessageDirection.CLIENT,
              false);
        }
      });
    }
    if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.SERVER) {
      this.doFile(dir,
          fnSource + this.getBaseName(intf, MessageDirection.SERVER)
          + fnSuffixI, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateBaseI(pw, intf, MessageDirection.SERVER,
              false);
        }
      });
      this.doFile(dir,
          fnInclude + this.getBaseName(intf, MessageDirection.SERVER)
          + fnSuffixH, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateBaseH(pw, intf, MessageDirection.SERVER,
              false);
        }
      });
    }
  }

  private void generateImpl(final Service intf, Output dir,
      MessageDirection msgDir) throws Exception {
    if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.CLIENT) {
      this.doFile(dir, this.getImplName(intf, MessageDirection.CLIENT)
          + fnSuffixI, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateImplI(pw, intf, MessageDirection.CLIENT, false);
        }
      });
      this.doFile(dir, this.getImplName(intf, MessageDirection.CLIENT)
          + fnSuffixH, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateImplH(pw, intf, MessageDirection.CLIENT, false);
        }
      });
    }
    if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.SERVER) {
      this.doFile(dir, this.getImplName(intf, MessageDirection.SERVER)
          + fnSuffixI, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateImplI(pw, intf, MessageDirection.SERVER, false);
        }
      });
      this.doFile(dir, this.getImplName(intf, MessageDirection.SERVER)
          + fnSuffixH, this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateImplH(pw, intf, MessageDirection.SERVER, false);
        }
      });
    }
  }

  private void generateIntfRemoteStub(final Service intf, Output dir,
      final MessageDirection what, final MessageDirection mc,
      final boolean hasBaseClass) throws Exception {
    // Generate interface file
    this.doFile(dir, fnSource + this.getIntfName(intf, mc) + fnSuffixI,
        this.lh, new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateIntfI(pw, intf, mc, hasBaseClass);
      }
    });
    this.doFile(dir, fnInclude + this.getIntfName(intf, mc) + fnSuffixH,
        this.lh, new Gen() {
      @Override
      public void run(PrintWriter pw) throws Exception {
        Compiler.this.generateIntfH(pw, intf, mc, hasBaseClass);
      }
    });

    // Generate remote file

    if (mc == MessageDirection.BOTH || what == MessageDirection.BOTH
        || mc != what) {
      this.doFile(dir, fnSource + this.getRemoteName(intf, mc) + fnSuffixI,
          this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateRemoteI(pw, intf, mc, hasBaseClass);
        }
      });
      this.doFile(dir, fnInclude + this.getRemoteName(intf, mc) + fnSuffixH,
          this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateRemoteH(pw, intf, mc, hasBaseClass);
        }
      });
    }
    // Generate stub file

    if (mc == MessageDirection.BOTH || what == MessageDirection.BOTH
        || mc == what) {
      this.doFile(dir, fnSource + this.getStubName(intf, mc) + fnSuffixI,
          this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateStubI(pw, intf, mc, hasBaseClass);
        }
      });
      this.doFile(dir, fnInclude + this.getStubName(intf, mc) + fnSuffixH,
          this.lh, new Gen() {
        @Override
        public void run(PrintWriter pw) throws Exception {
          Compiler.this.generateStubH(pw, intf, mc, hasBaseClass);
        }
      });
    }
  }

  /**
   * Generate the value factory Implementation for the service.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @throws Exception
   */
  void generateVfI(PrintWriter pw, Service intf, MessageDirection mc)
      throws Exception {
    // params keeps track of the total set of parameters
    // named (for enums, structs, exceptions, and messages).
    Set<String> params = new HashSet<String>();

    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("params", params);
    context.put("inc", this.getVfName(intf) + fnSuffixH);
    context.put("mc", mc);
    context.put("namespace", this.cppNamespace(intf));
    this.vf_cpp_vm.merge(context, pw);
  }

  /**
   * Generate the value factory Header for the service.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @throws Exception
   */
  void generateVfH(PrintWriter pw, Service intf, MessageDirection mc)
      throws Exception {
    // params keeps track of the total set of parameters
    // named (for enums, structs, exceptions, and messages).
    Set<String> params = new HashSet<String>();

    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("params", params);
    context.put("mc", mc);
    context.put("namespace", this.cppNamespace(intf));
    this.vf_h_vm.merge(context, pw);
  }

  /**
   * Generate the interface Implementation for the service.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateIntfI(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("inc", this.getIntfName(intf, mc) + fnSuffixH);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("namespace", this.cppNamespace(intf));
    this.intf_cpp_vm.merge(context, pw);
  }

  /**
   * Generate the interface Header for the service.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateIntfH(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("inc", intf.name() + fnSuffixH);
    context.put("namespace", this.cppNamespace(intf));
    this.intf_h_vm.merge(context, pw);
  }

  /**
   * Generate the call to message implementation of the interface. This class
   * turns calls on its methods into messages which are sent to the remote stub.
   * For two-way calls, it then waits for a response message, returning the
   * result therein to the caller.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateRemoteI(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {

    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("inc", this.getRemoteName(intf, mc) + fnSuffixH);
    context.put("methodList", new ArrayList<String>());
    context.put("namespace", this.cppNamespace(intf));
    this.remote_cpp_vm.merge(context, pw);

  }

  /**
   * Generate the call to message implementation Header of the interface. This
   * class turns calls on its methods into messages which are sent to the remote
   * stub. For two-way calls, it then waits for a response message, returning
   * the result therein to the caller.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateRemoteH(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {

    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("inc", this.getRemoteName(intf, mc) + fnSuffixH);
    context.put("hasBaseClass", hasBaseClass);
    context.put("methodList", new ArrayList<String>());
    context.put("namespace", this.cppNamespace(intf));
    this.remote_h_vm.merge(context, pw);

  }

  /**
   * Generate the message to call implementation. This class accepts a message
   * and turns it back into a call on the user's implementation. For two-way
   * messages, the return value from the user's implementation method is turned
   * into the appropriate response message and sent.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateStubI(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("inc", this.getStubName(intf, mc) + fnSuffixH);
    context.put("hasBaseClass", hasBaseClass);
    context.put("namespace", this.cppNamespace(intf));
    this.stub_cpp_vm.merge(context, pw);
  }

  /**
   * Generate the message to call implementation. This class accepts a message
   * and turns it back into a call on the user's implementation. For two-way
   * messages, the return value from the user's implementation method is turned
   * into the appropriate response message and sent.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateStubH(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("inc", this.getIntfName(intf, mc) + fnSuffixH);
    context.put("hasBaseClass", hasBaseClass);
    context.put("namespace", this.cppNamespace(intf));
    this.stub_h_vm.merge(context, pw);
  }

  /**
   * Generate the transport plumbing helper Implementation.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @throws Exception
   */
  void generateHelperI(PrintWriter pw, Service intf, MessageDirection mc)
      throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("inc", this.getHelperName(intf) + fnSuffixH);
    context.put("mc", mc);
    context.put("namespace", this.cppNamespace(intf));
    this.helper_cpp_vm.merge(context, pw);
  }

  /**
   * Generate the transport plumbing helper Header.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @throws Exception
   */
  void generateHelperH(PrintWriter pw, Service intf, MessageDirection mc)
      throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("inc", this.getHelperName(intf) + fnSuffixH);
    context.put("mc", mc);
    context.put("namespace", this.cppNamespace(intf));
    this.helper_h_vm.merge(context, pw);
  }

  /**
   * Generate the readme.txt.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @throws Exception
   */
  void generateReadme(PrintWriter pw, Service intf, MessageDirection mc)
      throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("namespace", this.cppNamespace(intf));
    this.readme_vm.merge(context, pw);
  }

  /**
   * Generate the template main program.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateMainI(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("namespace", this.cppNamespace(intf));
    this.main_cpp_vm.merge(context, pw);
  }

  void generateMainH(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("namespace", this.cppNamespace(intf));
    this.main_h_vm.merge(context, pw);
  }

  /**
   * Generates the base implementation of the interfaces, with each method
   * throwing an exception to the tune that it isn't implemented. User's impl
   * will extend this base implementation.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateBaseI(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("inc", this.getBaseName(intf, mc) + fnSuffixH);
    context.put("hasBaseClass", hasBaseClass);
    context.put("methodList", new ArrayList<String>());
    context.put("namespace", this.cppNamespace(intf));
    this.base_cpp_vm.merge(context, pw);
  }

  /**
   * Generates the base implementation of the interfaces, with each method
   * throwing an exception to the tune that it isn't implemented. User's impl
   * will extend this base implementation.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateBaseH(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("inc", this.getIntfName(intf, mc) + fnSuffixH);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("methodList", new ArrayList<String>());
    context.put("namespace", this.cppNamespace(intf));
    this.base_h_vm.merge(context, pw);
  }

  /**
   * Generate the template user implemention class which extends the base
   * implementation generated above. This class will only have the appropriate
   * constructor and reference to the appropriate remote, and a comment inviting
   * the user to override methods.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateImplI(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("namespace", this.cppNamespace(intf));
    this.impl_cpp_vm.merge(context, pw);
  }

  /**
   * Generate the template user implemention class which extends the base
   * implementation generated above. This class will only have the appropriate
   * constructor and reference to the appropriate remote, and a comment inviting
   * the user to override methods.
   * 
   * @param pw
   * @param intf
   * @param mc
   * @param hasBaseClass
   * @throws Exception
   */
  void generateImplH(PrintWriter pw, Service intf, MessageDirection mc,
      boolean hasBaseClass) throws Exception {
    VelocityContext context = new VelocityContext();
    context.put("now", new Date());
    context.put("version", VERSION);
    context.put("helper", this);
    context.put("intf", intf);
    context.put("mc", mc);
    context.put("suffix", MsgDirHelper.getSuffix(mc));
    context.put("hasBaseClass", hasBaseClass);
    context.put("namespace", this.cppNamespace(intf));
    this.impl_h_vm.merge(context, pw);
  }

  public MessageDirection getRemoteDirection(MessageDirection mc) {
    if (mc == MessageDirection.SERVER) {
      return MessageDirection.CLIENT;
    }
    if (mc == MessageDirection.BOTH) {
      return MessageDirection.BOTH;
    }
    return MessageDirection.SERVER;
  }

  public String getRemoteDirectionName(MessageDirection mc) {

    return this.getDirectionName(this.getRemoteDirection(mc));
  }

  public boolean hasMessageDirectionBoth(Service intf) {
    return intf.hasMessageDirection(MessageDirection.BOTH);
  }

  @Override
  public boolean isClient(MessageDirection mc) {
    return mc == MessageDirection.CLIENT || mc == MessageDirection.BOTH;
  }

  @Override
  public boolean isServer(MessageDirection mc) {
    return mc == MessageDirection.SERVER || mc == MessageDirection.BOTH;
  }

  public String getDirectionName(MessageDirection mc) {
    return MsgDirHelper.getSuffix(mc).toLowerCase();
  }

  public MessageDirection getMsgDirServer() {
    return MessageDirection.SERVER;
  }

  public MessageDirection getMsgDirClient() {
    return MessageDirection.CLIENT;
  }

  public String getServiceName(Service intf) {
    return intf.name().name().toLowerCase();
  }

  public String getIntfName(Service intf) {
    return this.getServiceName(intf);
  }

  public String getVfName(Service intf) {
    return "ValueFactory" + this.getIntfName(intf, MessageDirection.BOTH);
  }

  public String getIntfName(Service intf, MessageDirection mc) {
    String suffix = MsgDirHelper.getSuffix(mc);
    return intf.name() + suffix;
  }

  public String getMainName(Service intf, MessageDirection mc) {
    if (mc == MessageDirection.SERVER)
      return "Main" + intf.name() + "Listener";
    return "Main" + this.getIntfName(intf, mc);
  }

  public String getImplName(Service intf, MessageDirection mc) {
    return "Impl" + this.getIntfName(intf, mc);
  }

  public String getRemoteName(Service intf, MessageDirection mc) {
    return "Remote" + this.getIntfName(intf, mc);
  }

  public String getStubName(Service intf, MessageDirection mc) {
    return "Stub" + this.getIntfName(intf, mc);
  }

  public String getHelperName(Service intf) {
    return intf.name() + "Helper";
  }

  public String getBaseName(Service intf, MessageDirection mc) {
    return "Base" + this.getIntfName(intf, mc);
  }

  public String getKeywordFilename(Service intf) {
    return intf.name().name().toLowerCase() + "_wireshark.ewh";
  }

  @Override
  public String asyncReceiverPoolName(Message msg) {
    return msg.getAsyncReceiver().toString().toLowerCase();
  }

  @Override
  public String getTypeValue(TypeRef type, Token value) {
    Token t = type.type();
    switch (t.kind) {
    case EtchGrammarConstants.LONG:
      return value.image + "LL";
    case EtchGrammarConstants.FLOAT:
    case EtchGrammarConstants.DOUBLE:
      return value.image + (value.image.indexOf('.') > -1 ? "f" : "");
    case EtchGrammarConstants.STRING:
      return "L" + this.protectString(value.image);
    case EtchGrammarConstants.BOOLEAN:
      return value.image.toUpperCase();
    default:
      return value.image;
    }
  }

  private String protectString(String s) {
    StringBuffer sb = new StringBuffer();
    sb.append("\"");
    for (char c : s.toCharArray()) {
      if (c == '\t') {
        sb.append("\\t");
        continue;
      }
      if (c == '\r') {
        sb.append("\\r");
        continue;
      }
      if (c == '\n') {
        sb.append("\\n");
        continue;
      }
      if (c == '\"') {
        sb.append("\\\"");
        continue;
      }
      if (c == '\\') {
        sb.append("\\\\");
        continue;
      }
      if (c >= 32 && c < 127) {
        sb.append(c);
        continue;
      }
      sb.append(String.format("\\u%04x", (int) c));
    }
    sb.append("\"");
    return sb.toString();
  }

  private String getNativeArrayTypeName(TypeRef type) {
    Token t = type.type();
    String nativeArrayName = "EtchNativeArray";
    switch (t.kind) {
    case EtchGrammarConstants.BOOLEAN:
      return nativeArrayName + "Bool";
    case EtchGrammarConstants.BYTE:
      return nativeArrayName + "Byte";
    case EtchGrammarConstants.SHORT:
      return nativeArrayName + "Short";
    case EtchGrammarConstants.INT:
      return nativeArrayName + "Int";
    case EtchGrammarConstants.LONG:
      return nativeArrayName + "Long";
    case EtchGrammarConstants.FLOAT:
      return nativeArrayName + "Float";
    case EtchGrammarConstants.DOUBLE:
      return nativeArrayName + "Double";
    case EtchGrammarConstants.STRING:
      return nativeArrayName + "String";
    case EtchGrammarConstants.OBJECT:
      return nativeArrayName + "Object";
    default: {
      // we have to use a fully qualified name here.
      // find the actual type...
      Named<?> n = type.intf().get(t.image);
      // System.out.println("Token: " + t.image);
      if (n == null)
        throw new IllegalArgumentException(String.format(
            "undefined or ambiguous name at line %d: %s", t.beginLine, t.image));
      if (n.isBuiltin()) {
        Builtin b = (Builtin) n;
        return nativeArrayName + b.className().substring(4);
      }
      if (n.isEnumx()) {
        return nativeArrayName + n.efqname(this);
      } else {
        return type.intf().name() + "::" + nativeArrayName + n.efqname(this);
      }
    }
    }
  }


  @Override
  public String getTypeName(TypeRef type) {
    return " I A M NOT IMPLEMENTED";
    //if (type.dim() > 0)
    //  return this.getPointerTypeName(type);
  }

  public String getNativeTypeNameForConstants(TypeRef type) {
    if (type.type().kind == EtchGrammarConstants.STRING)
      return "wchar_t*";
    return this.getNativeTypeName(type, false);
  }

  /**
   * @param type
   *          the etch type
   * @return the fundamental native type for c so etch int -> c int, while etch
   *         string -> wchar* string.
   */
  @Override
  public String getNativeTypeName(TypeRef type) {
    return this.getNativeTypeName(type, false);
  }

  public String getEtchTypeName(TypeRef type, boolean Pointer) {
    if (type.dim() > 0) return this.getArrayTypeName(type, Pointer);
    return Pointer ? this.getPointerTypeName(type) : this.getNativeTypeName(type, true);
  }

  private String getArrayTypeName(TypeRef type, boolean pointer) {
    // TODO Auto-generated method stub
    if(pointer)
      return this.getNativeArrayTypeName( type ) + "Ptr";
    if (type.type().kind == EtchGrammarConstants.BYTE) {
      return "EtchNativeArray<" + this.getNativeTypeName( type, false ) + "> ";
    } else {
      return "EtchNativeArray<" + this.getNativeTypeName( type, true ) + "Ptr> ";
    }
  }

  public String getNativeTypeName(TypeRef type, boolean etch_type) {
    Token t = type.type();
    switch (t.kind) {
    case EtchGrammarConstants.VOID:
      return "void";
    case EtchGrammarConstants.BOOLEAN:
      return (etch_type ? "EtchBool" : "capu::bool_t");
    case EtchGrammarConstants.BYTE:
      return (etch_type ? "EtchByte" : "capu::int8_t");
    case EtchGrammarConstants.SHORT:
      return (etch_type ? "EtchShort" : "capu::int16_t");
    case EtchGrammarConstants.INT:
      return (etch_type ? "EtchInt32" : "capu::int32_t");
    case EtchGrammarConstants.LONG:
      return (etch_type ? "EtchLong" : "capu::int64_t");
    case EtchGrammarConstants.FLOAT:
      return (etch_type ? "EtchFloat" : "capu::float_t");
    case EtchGrammarConstants.DOUBLE:
      return (etch_type ? "EtchDouble" : "capu::double_t");
    case EtchGrammarConstants.STRING:
      return (etch_type ? "EtchString" : "EtchString");
    case EtchGrammarConstants.OBJECT:
      return (etch_type ? "EtchObject" : "EtchObject");
    default: {
      // we have to use a fully qualified name here.
      // find the actual type...
      Named<?> n = type.intf().get(t.image);
      // System.out.println("Token: " + t.image);
      if (n == null)
        throw new IllegalArgumentException(String.format(
            "undefined or ambiguous name at line %d: %s", t.beginLine, t.image));
      if (n.isBuiltin()) {
        Builtin b = (Builtin) n;
        if (n.efqname(this).equals("EtchDate")) return b.className();
        if (n.efqname(this).equals("EtchList")) return b.className()+"<EtchObjectPtr> ";
        if (n.efqname(this).equals("EtchHashTable")) return b.className()+"<EtchObjectPtr, EtchObjectPtr> ";
        if (n.efqname(this).equals("EtchHashSet")) return b.className()+"<EtchObjectPtr> ";
        throw new IllegalArgumentException(String.format(
            "unable to find correct Etch data type for type at line %d: %s", t.beginLine, n.efqname(this)));
      }
      if (n.isEnumx()) {
        return n.efqname(this);
      } else {
        return type.intf().name() + "::" + n.efqname(this);
      }
    }
    }
  }

  /**
   * @param type
   *          the etch type
   * @return the fundamental native reference type for cpp.
   */
  public String getPointerTypeName(TypeRef type) {
    Token t = type.type();
    switch (t.kind) {
    case EtchGrammarConstants.VOID:
      return "void";
    case EtchGrammarConstants.BOOLEAN:
      return "EtchBoolPtr";
    case EtchGrammarConstants.BYTE:
      return "EtchBytePtr";
    case EtchGrammarConstants.SHORT:
      return "EtchShortPtr";
    case EtchGrammarConstants.INT:
      return "EtchInt32Ptr";
    case EtchGrammarConstants.LONG:
      return "EtchLongPtr";
    case EtchGrammarConstants.FLOAT:
      return "EtchFloatPtr";
    case EtchGrammarConstants.DOUBLE:
      return "EtchDoublePtr";
    case EtchGrammarConstants.STRING:
      return "EtchStringPtr";
    case EtchGrammarConstants.OBJECT:
      return "EtchObjectPtr";
    default: {
      // we have to use a fully qualified name here.
      // find the actual type...
      Named<?> n = type.intf().get(t.image);
      if (n == null)
        throw new IllegalArgumentException(String.format(
            "undefined or ambiguous name at line %d: %s", t.beginLine, t.image));
      if (n.efqname(this).equals("EtchDate"))
        return "EtchDatePtr";
      if (n.isBuiltin()) {
        return n.efqname(this) + "Ptr";
      }
      return type.intf().name() + "::" + n.efqname(this) + "Ptr";
    }
    }
  }

  @Override
  public String mfvname(String vname) {
    return "_mf_" + vname;
  }

  @Override
  public String mtvname(String vname) {
    return "_mt_" + vname;
  }

  @Override
  public String getLang() {
    return "cpp";
  }

  @Override
  public String enum_efqname(String fqname, String moduleName,
      String serviceName, String enumName) {
    return enumName;
  }

  @Override
  public String except_efqname(String fqname, String moduleName,
      String serviceName, String exceptName) {
    return serviceName.toLowerCase() + "_" + exceptName;
  }

  @Override
  public String struct_efqname(String fqname, String moduleName,
      String serviceName, String structName) {
    return serviceName.toLowerCase() + "_" + structName;
  }

  @Override
  public String qualifyParameterName(Token name) {
    return name.image;
  }

  @Override
  public String qualifyConstantName(Service intf, Token name) {
    return intf.fqname() + '.' + name.image;
  }

  @Override
  public String qualifyEnumName(Service intf, Token name) {
    return intf.fqname() + '.' + name.image;
  }

  Set<String> history = new HashSet<String>();

  public boolean resetHistory() {
    this.history = new HashSet<String>();
    return true;
  }

  public boolean addStringToHistory(String s) {
    this.history.add(s);
    return true;
  }

  public boolean historyContains(String s) {
    return this.history.contains(s);
  }

  public String cppNamespace(Service svc) {
    return (svc.fqname()).replace('.', '_');
  }

  // ///////////////////////////////////////////////
  // /////////////////// WORKSPACE /////////////////
  // ///////////////////////////////////////////////

  @Override
  public String formatString(ParamList<Service> n, boolean isExcept)
      throws ParseException, IOException {
    ToString ts = (ToString) n.getOpt("ToString");
    List<FmtItem> list;
    if (ts != null) {
      list = ts.getFormat();
      n.checkFormatList(ts.lineno(), list);
    } else if (isExcept)
      list = n.mkFormatList(true, ((Except) n).hasExtends());
    else
      list = n.mkFormatList(false, ((Struct) n).hasExtends());

    if (list.size() == 1) {
      return list.get(0).value();
    }

    StringBuffer sb = new StringBuffer();
    sb.append("\"");
    for (FmtItem i : list) {
      if (i instanceof FieldItem) {
        sb.append("%s");
      } else {
        this.escape(sb, ((StringItem) i).value());
      }
    }
    sb.append("\"");
    for (FmtItem i : list) {
      if (i instanceof FieldItem) {
        sb.append(", ");
        sb.append(((FieldItem) i).value() + "->c_str()");
      }
    }
    return sb.toString();
  }

  private void escape(StringBuffer sb, String s) throws IOException {
    StringReader rdr = new StringReader(s);
    int c;
    while ((c = rdr.read()) >= 0) {
      if (c == '"')
        sb.append("\\\"");
      else if (c == '\\')
        sb.append("\\\\");
      else if (c == '\t')
        sb.append("\\t");
      else if (c == '\r')
        sb.append("\\r");
      else if (c == '\n')
        sb.append("\\n");
      else
        sb.append((char) c);
    }
  }

  @Override
  public String getValidator(Named<?> named) {
    if (named instanceof Parameter) {
      Parameter param = (Parameter) named;
      TypeRef type = param.type();

      if (type.isBuiltin())
        return String.format("EtchValidator%s::Get(runtime, %d, tmpValue)",
            this.getValidatorStringForParam(param), type.dim());

      Named<?> n = type.getNamed(type.intf());

      if (n.isBuiltin()) {
        Builtin b = (Builtin) n;

        String cn = b.className();
        if (n.efqname(this).equals("EtchHashTable")) {
          cn += "<EtchObjectPtr, EtchObjectPtr>";
        }
        if (n.efqname(this).equals("EtchHashSet")) {
          cn += "<EtchObjectPtr>";
        }
        if (n.efqname(this).equals("EtchList")) {
          cn += "<EtchObjectPtr>";
        }

        /*
         * int i = cn.indexOf( '<' ); if (i >= 0) cn = cn.substring( 0, i );
         */
        return String.format(
            "EtchValidatorCustom::Get(runtime, %d, %s::TYPE(), %s, tmpValue);",
            type.dim(), cn, b.allowSubclass());
      }

      // Allow subclassing for etch defined structs and externs.

      if (n.isStruct() || n.isExcept()) {
        if (n.isStruct()) {
          Struct s = (Struct) n;
          return String.format(
              "EtchValidatorCustom::Get(runtime, %d, %s::TYPE(), true, tmpValue);",
              type.dim(), s
              .service().name() + "::" + n.efqname(this));
        }
        Except e = (Except) n;
        return String.format(
            "EtchValidatorCustom::Get(runtime, %d, %s::TYPE(), true, tmpValue);", type.dim(), e
            .service().name() + "::" + n.efqname(this));
      }
      // Don't allow subclassing for externs or etch defined enums.
      if (!(n.isExtern() || n.isEnumx()))
        Assertion.check(n.isExtern() || n.isEnumx(),
            "n.isExtern() || n.isEnumx(): " + n);
      Enumx e = (Enumx) n;
      return String.format(
          "EtchValidatorCustom::Get(runtime, %d, %s::TYPE(), false, tmpValue);",
          type.dim(), n.efqname(this));
    }

    if (named instanceof Thrown) {
      Thrown thrown = (Thrown) named;
      Except e = (Except) thrown.getNamed();
      String efgName = this.getExcept(thrown).service().name() + "::"
          + e.efqname(this);
      return String.format(
          "EtchValidatorCustom::Get(runtime, 0, %s::TYPE(), true, tmpValue);"
          , efgName);
    }
    if (named instanceof Item)
      return "EtchValidatorBoolean::Get(runtime, 0, tmpValue)";
    return "NULL";
  }

  /**
   * @param name
   * @return the appropriate name for a getter method.
   */
  public String getGetterName(Name name) {
    String s = name.name;
    return "get" + s.substring(0, 1).toUpperCase() + s.substring(1);
  }

  /**
   * @param name
   * @return the appropriate name for a setter method.
   */
  public String getSetterName(Name name) {
    String s = name.name;
    return "set" + s.substring(0, 1).toUpperCase() + s.substring(1);
  }

  @Override
  public void addDefaults(Service service) throws ParseException {
    this.addBuiltin(service, this.newName("List"), "EtchList", true);
    this.addBuiltin(service, this.newName("Map"), "EtchHashTable", true);
    this.addBuiltin(service, this.newName("Set"), "EtchHashSet", true);
    this.addBuiltin(service, this.newName("Datetime"), "EtchDate", false);
  }

  public String getValidatorStringForParam(Parameter param) {
    TypeRef type = param.type();
    return this.getValidatorStringForParam(type);
  }

  public String getValidatorStringForParam(TypeRef type) {
    switch (type.type().kind) {
    case EtchGrammarConstants.BOOLEAN:
      return "Boolean";
    case EtchGrammarConstants.BYTE:
      return "Byte";
    case EtchGrammarConstants.INT:
      return "Int";
    case EtchGrammarConstants.SHORT:
      return "Short";
    case EtchGrammarConstants.DOUBLE:
      return "Double";
    case EtchGrammarConstants.FLOAT:
      return "Float";
    case EtchGrammarConstants.LONG:
      return "Long";
    case EtchGrammarConstants.STRING:
      return "String";
    }
    // what should be the default here?
    return "Object";
  }

  public String getValidatorStringForParam(Message param) {
    return this.getValidatorStringForParam(param.type());
  }

  public boolean isEnumParam(Parameter p) {
    if (p.type() == null)
      return false;
    if (p.type().intf() == null)
      return false;
    if (p.type().getNamed(p.type().intf()) == null)
      return false;
    return p.type().getNamed(p.type().intf()).isEnumx();
  }

  public boolean isCustomType(Parameter param) {
    return this.isCustomType(param.type());
  }

  public boolean isRefType(TypeRef param) {
    if (param == null)
      return true;
    switch (param.type().kind) {
    case EtchGrammarConstants.BOOLEAN:
    case EtchGrammarConstants.DOUBLE:
    case EtchGrammarConstants.FLOAT:
    case EtchGrammarConstants.INT:
    case EtchGrammarConstants.LONG:
    case EtchGrammarConstants.ENUM:
    case EtchGrammarConstants.DECIMAL:
    case EtchGrammarConstants.SHORT:
      return false;
    default:
      return true;
    }
  }

  public boolean isCustomType(TypeRef param) {
    switch (param.type().kind) {
    case EtchGrammarConstants.BOOLEAN:
    case EtchGrammarConstants.DOUBLE:
    case EtchGrammarConstants.FLOAT:
    case EtchGrammarConstants.INTEGER:
    case EtchGrammarConstants.STRING:
    case EtchGrammarConstants.LONG:
      return false;
    default:
      return true;
    }
  }

  public String getTypeName(Parameter pa) {
    return pa.type().type().image;
  }

  public String getTypeName(Message pa) {
    return pa.type().type().image;
  }

  public List<String> getUsedServiceNames(Service s) {
    final List<String> ret = new LinkedList<String>();
    IncludeTreeWalker walker = new IncludeTreeWalker(ret, s);
    try {
      s.treewalk(walker);
    } catch (ParseException e) {
      e.printStackTrace();
    }
    return walker.getList();
  }

  public boolean containsStructsOrMessages(Service s) {
    return s.messages(false).hasNext() || s.structs(false).hasNext();
  }

  public String getClassIdVarName(Parameter p) {
    Named<?> n = p.type().getNamed(p.type().intf());
    return n.efqname(this);
  }

  public String getClassIdVarName(Thrown t) {
    Except p = (Except) t.getNamed();
    return p.service().name() + "::" + p.name().toString();
  }

  public Except getExcept(Thrown t) {
    Except e = (Except) t.getNamed();
    return e;
  }

  public String getClassIdVarName(Message n) {
    Named<?> na = n.type().getNamed(n.type().intf());
    return na.efqname(this);
  }

  public String getDefiningServiceNameOf(TypeRef type) {
    Named<?> na = type.getNamed(type.intf());
    return na.parent().name().name.toLowerCase();
  }

}
