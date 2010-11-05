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

package org.apache.etch.bindings.c.compiler;

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
import org.apache.etch.compiler.EtchGrammar;
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.EtchHelper;
import org.apache.etch.compiler.LogHandler;
import org.apache.etch.compiler.Output;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;
import org.apache.etch.compiler.Version;
import org.apache.etch.compiler.ast.Builtin;
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

	private final static String tmplPath1 = "org/apache/etch/bindings/c/compiler/";

	private final static String tmplPath2 = "resources/org/apache/etch/bindings/c/compiler/";

	private final static String fnSuffixH = ".h";
	private final static String fnSuffixI = ".c";

	private final static String VERSION = Version.VERSION + " / "
			+ CompilerVersion.VERSION;

	/**
	 * Constructs the Compiler. This is a helper class not only for Backend, but
	 * also for the templates. They call methods here to perform "hard" tasks.
	 * 
	 * @throws Exception
	 */
	public Compiler() throws Exception {
		initVelocity();

		String[] path = { tmplPath1, tmplPath2 };

		// value factory templates
		vf_h_vm = getTemplate(path, "vf_h.vm");
		vf_c_vm = getTemplate(path, "vf_c.vm");

		// interface templates
		intf_h_vm = getTemplate(path, "intf_h.vm");
		intf_c_vm = getTemplate(path, "intf_c.vm");
		implx_c_vm = getTemplate(path, "implx_c.vm");

		// remote interface templates
		remote_h_vm = getTemplate(path, "remote_h.vm");
		remote_c_vm = getTemplate(path, "remote_c.vm");
		remote_server_h_vm = getTemplate(path, "remote_server_h.vm");
		remote_server_c_vm = getTemplate(path, "remote_server_c.vm");
		remote_client_h_vm = getTemplate(path, "remote_client_h.vm");
		remote_client_c_vm = getTemplate(path, "remote_client_c.vm");

		// interface stub templates
		stub_h_vm = getTemplate(path, "stub_h.vm");
		stub_c_vm = getTemplate(path, "stub_c.vm");

		// helper templates
		helper_h_vm = getTemplate(path, "helper_h.vm");
		helper_c_vm = getTemplate(path, "helper_c.vm");

		// readme template
		readme_vm = getTemplate(path, "readme.vm");

		// Main template
		main_h_vm = getTemplate(path, "main_h.vm");
		main_c_vm = getTemplate(path, "main_c.vm");

		// Base template
		base_h_vm = getTemplate(path, "base_h.vm");
		base_c_vm = getTemplate(path, "base_c.vm");

		// Implementation template
		impl_h_vm = getTemplate(path, "impl_h.vm");
		impl_c_vm = getTemplate(path, "impl_c.vm");

		etch_keywords_vm = getTemplate(path, "etch_wireshark.vm");
		
		// Keyword list
		local_kwd = getPath(path, "cKeywords.kwd");
	}

	// Value Factory
	private final Template vf_h_vm;
	private final Template vf_c_vm;

	// interface templates
	private final Template intf_h_vm;
	private final Template intf_c_vm;

	// remote interface templates
	private final Template remote_h_vm;
	private final Template remote_c_vm;
	private final Template remote_server_h_vm;
	private final Template remote_server_c_vm;
	private final Template remote_client_h_vm;
	private final Template remote_client_c_vm;

	// interface stub templates
	private final Template stub_h_vm;
	private final Template stub_c_vm;

	// helper templates
	private final Template helper_h_vm;
	private final Template helper_c_vm;

	// readme template
	private final Template readme_vm;

	// Main template
	private final Template main_h_vm;
	private final Template main_c_vm;

	// Base template
	private final Template base_h_vm;
	private final Template base_c_vm;

	// Implementation template
	private final Template impl_h_vm;
	private final Template impl_c_vm;
	private final Template implx_c_vm;

	private final Template etch_keywords_vm;
	
	private final String local_kwd;

	private LogHandler lh;

	/**
	 * Initializes use of velocity engine and sets up resource loaders.
	 * 
	 * @throws Exception
	 */
	private void initVelocity() throws Exception {
		Velocity.setProperty(Velocity.RUNTIME_LOG_LOGSYSTEM, new MyLogger());

		Velocity.setProperty(Velocity.RESOURCE_LOADER, "file, class");

		Velocity.setProperty("file.resource.loader.description",
				"Velocity File Resource Loader");
		Velocity
				.setProperty("file.resource.loader.class",
						"org.apache.velocity.runtime.resource.loader.FileResourceLoader");
		Velocity.setProperty("file.resource.loader.path", ".");
		Velocity.setProperty("class.resource.loader.description",
				"Velocity Classpath Resource Loader");
		Velocity
				.setProperty("class.resource.loader.class",
						"org.apache.velocity.runtime.resource.loader.ClasspathResourceLoader");
		Velocity.init();
	}

	private static class MyLogger implements LogChute {
		private final LogHandler lh = null;

		public void init(RuntimeServices rts) throws Exception {
			// ignore.
		}

		public boolean isLevelEnabled(int level) {
			return level >= 2;
		}

		public void log(int level, String msg) {
			if (level < 2)
				return;

			if (lh != null)
				lh.report(level == 2 ? LogHandler.LEVEL_WARNING
						: LogHandler.LEVEL_ERROR, null, msg);
			else
				System.out.printf("Velocity msg (%d): %s\n", level, msg);
		}

		public void log(int level, String msg, Throwable e) {
			if (level < 2)
				return;

			if (lh != null)
				lh.report(level == 2 ? LogHandler.LEVEL_WARNING
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
	public void generate(Module module, CmdLineOptions options)
			throws Exception {
		// java always wants to not flatten packages:
		options.noFlattenPackages = true;

		lh = options.lh;

		boolean ignoreGlobal = options.ignoreGlobalWordsList;
		boolean ignoreLocal = options.ignoreLocalWordsList;
		String userWords = options.userWordsList != null ? options.userWordsList
				.getPath()
				: null;
		Set<String> what = options.what;

		// Load the reserved words lists if any have been provided.
		Map<String, String> words = new HashMap<String, String>();
		if (!ignoreGlobal)
			mapWords(global_kwd, words);
		if (!ignoreLocal)
			mapWords(local_kwd, words);
		if (userWords != null)
			mapWords(userWords, words);

		// check for collisions with the reserved word list.
		ReservedWordChecker checker = new ReservedWordChecker(words, false, lh);
		module.treewalk(checker);
		if (!checker.ok()) {
			lh.report(LogHandler.LEVEL_ERROR, null,
					"Encountered errors during c generation.");
			return;
		}

		// ok, we're ready to generate code. make sure the
		// output directories exist.

		Output dir = options.output;
		Output templateDir = options.templateOutput;

		// generate code for each service.

		for (Service intf : module) {
			generate(intf, what, dir, templateDir);
		}
	}

	public boolean isGenerateImpl() {
		return isGenerateImpl;
	}

	public boolean isGenerateMain() {
		return isGenerateMain;
	}

	boolean isGenerateImpl;
	boolean isGenerateMain;

	private void generate(final Service intf, Set<String> what, Output dir,
			Output templateDir) throws Exception {
		what = populateWhat(what);

		if (what.isEmpty()) {
			return;
		}

		final MessageDirection msgDir = getMessageDirection(what);

		if (what.contains(WHAT_INTF)) {
			// Generate the value factory file.

			generateVf(intf, dir);

			// Generate the interface, remote, and stub files.
			generateIntfRemoteStub(intf, dir, msgDir, MessageDirection.BOTH,
					false);
			generateIntfRemoteStub(intf, dir, msgDir, MessageDirection.SERVER,
					true);
			generateIntfRemoteStub(intf, dir, msgDir, MessageDirection.CLIENT,
					true);

			// Generate helper file.
			generateHelper(intf, dir, msgDir);

			// Generate base file.
			generateBase(intf, dir, msgDir, MessageDirection.SERVER, true);
			generateBase(intf, dir, msgDir, MessageDirection.CLIENT, true);

			// Generate readme file.
			generateReadme(intf, dir, msgDir);
			
			generateKeywordList(intf, dir, msgDir, MessageDirection.BOTH, true);
		}

		// Generate main template file.
		if (what.contains(WHAT_MAIN)) {
			isGenerateMain = true;
			generateMain(intf, templateDir, msgDir);
		}

		// Generate impl template file.
		if (what.contains(WHAT_IMPL)) {
			isGenerateImpl = true;
			generateImpl(intf, templateDir, msgDir);
		}
	}

	private void generateReadme(final Service intf, Output dir,
			final MessageDirection msgDir) throws Exception {
		doFile(dir, "readme-etch-c-files.txt", lh, new Gen() {
			public void run(PrintWriter pw) throws Exception {
				generateReadme(pw, intf, msgDir);
			}
		});
	}

	private void generateVf(final Service intf, Output dir) throws Exception {

		doFile(dir, getVfName(intf) + fnSuffixH, lh, new Gen() {
			public void run(PrintWriter pw) throws Exception {
				generateVfH(pw, intf);
			}
		});

		doFile(dir, getVfName(intf) + fnSuffixI, lh, new Gen() {
			public void run(PrintWriter pw) throws Exception {
				generateVfI(pw, intf);
			}
		});

	}

	private void generateHelper(final Service intf, Output dir,
			final MessageDirection msgDir) throws Exception {
		doFile(dir, getHelperName(intf) + fnSuffixH, lh, new Gen() {
			public void run(PrintWriter pw) throws Exception {
				generateHelperH(pw, intf, msgDir);
			}
		});

		doFile(dir, getHelperName(intf) + fnSuffixI, lh, new Gen() {
			public void run(PrintWriter pw) throws Exception {
				generateHelperI(pw, intf, msgDir);
			}
		});

	}

	private void generateMain(final Service intf, Output dir,
			MessageDirection msgDir) throws Exception {
		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.CLIENT) {
			doFile(dir, getMainName(intf, MessageDirection.CLIENT) + fnSuffixH,
					lh, new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateMainH(pw, intf, MessageDirection.CLIENT,
									false);
						}
					});

			doFile(dir, getMainName(intf, MessageDirection.CLIENT) + fnSuffixI,
					lh, new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateMainI(pw, intf, MessageDirection.CLIENT,
									false);
						}
					});

		}

		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.SERVER) {
			doFile(dir, getMainName(intf, MessageDirection.SERVER) + fnSuffixH,
					lh, new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateMainH(pw, intf, MessageDirection.SERVER,
									false);
						}
					});

			doFile(dir, getMainName(intf, MessageDirection.SERVER) + fnSuffixI,
					lh, new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateMainI(pw, intf, MessageDirection.SERVER,
									false);
						}
					});
		}
	}

	private void generateKeywordList(final Service intf, Output dir,
			final MessageDirection what, final MessageDirection mc,
			final boolean hasBaseClass) throws Exception {
		doFile(dir, getKeywordFilename(intf), lh,
				new Gen() {
					public void run(PrintWriter pw) throws Exception {
						generateKeywords(pw, intf, mc, hasBaseClass);
					}
				});
	}
	
	private void generateBase(final Service intf, Output dir,
			final MessageDirection what, final MessageDirection mc,
			final boolean hasBaseClass) throws Exception {
		doFile(dir, getBaseFileNameH(intf, getDirectionName(mc)), lh,
				new Gen() {
					public void run(PrintWriter pw) throws Exception {
						generateBaseH(pw, intf, mc, hasBaseClass);
					}
				});

		doFile(dir, getBaseFileNameI(intf, getDirectionName(mc)), lh,
				new Gen() {
					public void run(PrintWriter pw) throws Exception {
						generateBaseI(pw, intf, mc, hasBaseClass);
					}
				});
	}

	private void generateImpl(final Service intf, Output dir,
			MessageDirection msgDir) throws Exception {
		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.CLIENT) {
			doFile(dir, getImplFileNameH(intf,
					getDirectionName(MessageDirection.CLIENT)), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateImplH(pw, intf, MessageDirection.CLIENT, false);
				}
			});

			doFile(dir, getImplFileNameI(intf,
					getDirectionName(MessageDirection.CLIENT)), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateImplI(pw, intf, MessageDirection.CLIENT, false);
				}
			});

			doFile(dir, getImplXFileNameI(intf,
					getDirectionName(MessageDirection.CLIENT)), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateImplXI(pw, intf, MessageDirection.CLIENT, false);
				}
			});
		}

		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.SERVER) {
			doFile(dir, getImplFileNameH(intf,
					getDirectionName(MessageDirection.SERVER)), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateImplH(pw, intf, MessageDirection.SERVER, false);
				}
			});

			doFile(dir, getImplFileNameI(intf,
					getDirectionName(MessageDirection.SERVER)), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateImplI(pw, intf, MessageDirection.SERVER, false);
				}
			});

			doFile(dir, getImplXFileNameI(intf,
					getDirectionName(MessageDirection.SERVER)), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateImplXI(pw, intf, MessageDirection.SERVER, false);
				}
			});
		}

	}

	private void generateIntfRemoteStub(final Service intf, Output dir,
			final MessageDirection what, final MessageDirection mc,
			final boolean hasBaseClass) throws Exception {
		// Generate interface file

		if (mc == MessageDirection.BOTH) {
			doFile(dir, getIntfFileNameH(intf), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateIntfH(pw, intf, mc, hasBaseClass);
				}
			});

			doFile(dir, getIntfFileNameI(intf), lh, new Gen() {
				public void run(PrintWriter pw) throws Exception {
					generateIntfI(pw, intf, mc, hasBaseClass);
				}
			});
		}

		// Generate remote file

		if (mc == MessageDirection.BOTH || what == MessageDirection.BOTH
				|| mc != what) {
			doFile(dir, getRemoteFileNameH(intf, getDirectionName(mc)), lh,
					new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateRemoteH(pw, intf, mc, hasBaseClass);
						}
					});

			doFile(dir, getRemoteFileNameI(intf, getDirectionName(mc)), lh,
					new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateRemoteI(pw, intf, mc, hasBaseClass);
						}
					});
		}

		// Generate stub file

		if (mc != what) {
			doFile(dir, getStubFileNameH(intf, getDirectionName(mc)), lh,
					new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateStubH(pw, intf, mc, hasBaseClass);
						}
					});

			doFile(dir, getStubFileNameI(intf, getDirectionName(mc)), lh,
					new Gen() {
						public void run(PrintWriter pw) throws Exception {
							generateStubI(pw, intf, mc, hasBaseClass);
						}
					});
		}
	}

	/**
	 * Generate the value factory header for the service.
	 * 
	 * @param pw
	 * @param intf
	 * @throws Exception
	 */
	void generateVfH(PrintWriter pw, Service intf) throws Exception {
		// params keeps track of the total set of parameters
		// named (for enums, structs, exceptions, and messages).
		Set<String> params = new HashSet<String>();

		VelocityContext context = new VelocityContext();
		context.put("now", new Date());
		context.put("version", VERSION);
		context.put("helper", this);
		context.put("intf", intf);
		context.put("params", params);
		vf_h_vm.merge(context, pw);
	}

	/**
	 * Generate the value factory implementation for the service.
	 * 
	 * @param pw
	 * @param intf
	 * @throws Exception
	 */
	void generateVfI(PrintWriter pw, Service intf) throws Exception {
		// params keeps track of the total set of parameters
		// named (for enums, structs, exceptions, and messages).
		Set<String> params = new HashSet<String>();

		VelocityContext context = new VelocityContext();
		context.put("now", new Date());
		context.put("version", VERSION);
		context.put("helper", this);
		context.put("intf", intf);
		context.put("params", params);
		vf_c_vm.merge(context, pw);
	}

	/**
	 * Generate the interface header for the service.
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		intf_h_vm.merge(context, pw);

	}

	/**
	 * Generate the interface implementation for the service.
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		intf_c_vm.merge(context, pw);
	}

	/**
	 * Generate the call to message implementation of the interface. This class
	 * turns calls on its methods into messages which are sent to the remote
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		context.put("methodList", new ArrayList<String>());
		switch (mc) {
		case BOTH:
			remote_h_vm.merge(context, pw);
			break;
		case SERVER:
			remote_server_h_vm.merge(context, pw);
			break;
		case CLIENT:
			remote_client_h_vm.merge(context, pw);
			break;
		default:
			break;
		}
	}

	/**
	 * Generate the call to message implementation of the interface. This class
	 * turns calls on its methods into messages which are sent to the remote
	 * stub. For two-way calls, it then waits for a response message, returning
	 * the result therein to the caller.
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		context.put("methodList", new ArrayList<String>());
		switch (mc) {
		case BOTH:
			remote_c_vm.merge(context, pw);
			break;
		case SERVER:
			remote_server_c_vm.merge(context, pw);
			break;
		case CLIENT:
			remote_client_c_vm.merge(context, pw);
			break;
		default:
			break;
		}
	}

	/**
	 * Generate the message to call implementation. This class accepts a message
	 * and turns it back into a call on the user's implementation. For two-way
	 * messages, the return value from the user's implementation method is
	 * turned into the appropriate response message and sent.
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("suffix_remote", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		stub_h_vm.merge(context, pw);
	}

	/**
	 * Generate the message to call implementation. This class accepts a message
	 * and turns it back into a call on the user's implementation. For two-way
	 * messages, the return value from the user's implementation method is
	 * turned into the appropriate response message and sent.
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("suffix_remote", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		stub_c_vm.merge(context, pw);
	}

	/**
	 * Generate the transport plumbing helper.
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
		context.put("mc", mc);
		helper_h_vm.merge(context, pw);
	}

	/**
	 * Generate the transport plumbing helper.
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
		context.put("mc", mc);
		helper_c_vm.merge(context, pw);

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

		readme_vm.merge(context, pw);
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
	void generateMainH(PrintWriter pw, Service intf, MessageDirection mc,
			boolean hasBaseClass) throws Exception {
		VelocityContext context = new VelocityContext();
		context.put("now", new Date());
		context.put("version", VERSION);
		context.put("helper", this);
		context.put("intf", intf);
		context.put("mc", mc);
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		main_h_vm.merge(context, pw);
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		main_c_vm.merge(context, pw);
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		context.put("methodList", new ArrayList<String>());
		base_h_vm.merge(context, pw);
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		context.put("methodList", new ArrayList<String>());
		base_c_vm.merge(context, pw);
	}

	/**
	 * Generate the template user implemention class which extends the base
	 * implementation generated above. This class will only have the appropriate
	 * constructor and reference to the appropriate remote, and a comment
	 * inviting the user to override methods.
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		impl_h_vm.merge(context, pw);
	}

	/**
	 * Generate the template user implemention class which extends the base
	 * implementation generated above. This class will only have the appropriate
	 * constructor and reference to the appropriate remote, and a comment
	 * inviting the user to override methods.
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
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		impl_c_vm.merge(context, pw);
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
		etch_keywords_vm.merge(context, pw);
	}
	
	/**
	 * Generate the template user implemention class which extends the base
	 * implementation generated above. This class will only have the appropriate
	 * constructor and reference to the appropriate remote, and a comment
	 * inviting the user to override methods.
	 * 
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	void generateImplXI(PrintWriter pw, Service intf, MessageDirection mc,
			boolean hasBaseClass) throws Exception {
		VelocityContext context = new VelocityContext();
		context.put("now", new Date());
		context.put("version", VERSION);
		context.put("helper", this);
		context.put("intf", intf);
		context.put("mc", mc);
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		implx_c_vm.merge(context, pw);
	}

	public String getKeywordForWireshark(String fieldname) {
		int hash = Hash.hash(fieldname);
		return String.format("0x%08x", hash) + "," + fieldname;
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

		return getDirectionName(getRemoteDirection(mc));
	}

	public boolean hasMessageDirectionBoth(Service intf) {
		return intf.hasMessageDirection(MessageDirection.BOTH);
	}

	public boolean isClient(MessageDirection mc) {
		return mc == MessageDirection.CLIENT || mc == MessageDirection.BOTH;
	}

	public boolean isServer(MessageDirection mc) {
		return mc == MessageDirection.SERVER || mc == MessageDirection.BOTH;
	}

	public String getDirectionName(MessageDirection mc) {
		return MsgDirHelper.getSuffix(mc).toLowerCase();
	}

	public MessageDirection getMsgDirServer(){
		return MessageDirection.SERVER;
	}
	
	public MessageDirection getMsgDirClient(){
		return MessageDirection.CLIENT;
	}
	
	public String getServiceName(Service intf) {
		return intf.name().name().toLowerCase();
	}

	public String getIntfName(Service intf) {
		return getServiceName(intf);
	}

	public String getIntfFileNameH(Service intf) {
		return getIntfName(intf) + "_interface" + fnSuffixH;
	}

	public String getIntfFileNameI(Service intf) {
		return getIntfName(intf) + "_interface" + fnSuffixI;
	}

	public String getBaseName(Service intf, String suffix) {
		return getServiceName(intf)
				+ (suffix != null && !suffix.isEmpty() ? "_" + suffix : "");
	}

	public String getBaseFileNameH(Service intf, String suffix) {
		return getBaseName(intf, suffix) + fnSuffixH;
	}

	public String getKeywordFilename(Service intf) {
		return getServiceName(intf) + "_wireshark.ewh";
	}
	
	public String getBaseFileNameI(Service intf, String suffix) {
		return getBaseName(intf, suffix) + fnSuffixI;
	}

	public String getRemoteName(Service intf, String suffix) {
		return getServiceName(intf) + "_remote"
				+ (suffix != null && !suffix.isEmpty() ? "_" + suffix : "");
	}

	public String getRemoteFileNameH(Service intf, String suffix) {
		return getRemoteName(intf, suffix) + fnSuffixH;
	}

	public String getRemoteFileNameI(Service intf, String suffix) {
		return getRemoteName(intf, suffix) + fnSuffixI;
	}

	public String getStubName(Service intf, String suffix) {
		return getServiceName(intf)
				+ (suffix != null && !suffix.isEmpty() ? "_" + suffix : "")
				+ "_stub";
	}

	public String getStubFileNameH(Service intf, String suffix) {
		return getStubName(intf, suffix) + fnSuffixH;
	}

	public String getStubFileNameI(Service intf, String suffix) {
		return getStubName(intf, suffix) + fnSuffixI;
	}

	public String getImplName(Service intf, String suffix) {
		return getServiceName(intf)
				+ (suffix != null && !suffix.isEmpty() ? "_" + suffix : "")
				+ "_impl";
	}

	public String getImplFileNameH(Service intf, String suffix) {
		return getImplName(intf, suffix) + fnSuffixH;
	}

	public String getImplFileNameI(Service intf, String suffix) {
		return getImplName(intf, suffix) + fnSuffixI;
	}

	public String getImplXName(Service intf, String suffix) {
		return getServiceName(intf)
				+ (suffix != null && !suffix.isEmpty() ? "_" + suffix : "")
				+ "_implx";
	}

	public String getImplXFileNameH(Service intf, String suffix) {
		return getImplXName(intf, suffix) + fnSuffixH;
	}

	public String getImplXFileNameI(Service intf, String suffix) {
		return getImplXName(intf, suffix) + fnSuffixI;
	}

	public String getVfName(Service intf) {
		return getServiceName(intf) + "_valufact";
	}

	public String getVfFileNameH(Service intf) {
		return getVfName(intf) + fnSuffixH;
	}

	public String getVfFileNameI(Service intf) {
		return getVfName(intf) + fnSuffixI;
	}

	public String getMainName(Service intf, MessageDirection mc) {
		if (mc == MessageDirection.SERVER)
			return getServiceName(intf) + "_listener_main";
		return getServiceName(intf) + "_client_main";
	}

	public String getMainFileNameH(Service intf, MessageDirection mc) {
		return getMainName(intf, mc) + fnSuffixH;
	}

	public String getMainFileNameI(Service intf, MessageDirection mc) {
		return getMainName(intf, mc) + fnSuffixI;
	}

	public String getHelperName(Service intf) {
		return getServiceName(intf) + "_helper";
	}

	public String getHelperFileNameH(Service intf) {
		return getHelperName(intf) + fnSuffixH;
	}

	public String getHelperFileNameI(Service intf) {
		return getHelperName(intf) + fnSuffixI;
	}

	public void test(TypeRef type) {
		int i;
		System.out.println(type);
		System.out.println("builtin " + type.isBuiltin());
		type.getNamed(type.intf()).efqname(this);

	}

	public void testMsg(Message msg) {
		int i;
		// msg.getResultParam().efqname(this);
		// msg.type().getNamed(msg.service()).efqname(this)
		msg.getResultParam().efqname(this);
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
			return value.image + (value.image.indexOf('.')>-1 ? "f" : "");
		case EtchGrammarConstants.STRING:
			return "L" + protectString(value.image);
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

	@Override
	public String getTypeName(TypeRef type) {
		if (type.dim() > 0)
			return "etch_arraytype*";
		return getPointerTypeName(type);
	}

	public String getNativeTypeNameForConstants(TypeRef type) {
		if (type.type().kind == EtchGrammarConstants.STRING)
			return "wchar_t*";
		return getNativeTypeName(type, false);
	}

	/**
	 * @param type
	 *            the etch type
	 * @return the fundamental native type for c so etch int -> c int, while
	 *         etch string -> wchar* string.
	 */
	@Override
	public String getNativeTypeName(TypeRef type) {
		return getNativeTypeName(type, false);
	}

	public String getNativeTypeName(TypeRef type, boolean addStruct) {
		return getNativeTypeName(type, false, false);
	}
	
	public String getEtchTypeName(TypeRef type) {
		return getNativeTypeName(type, false, true);
	}
	
	public String getNativeTypeName(TypeRef type, boolean addStruct, boolean etch_type) {
		if (type.isArray()) {
			return (etch_type ? "etch_arraytype" : "etch_arraytype*");
		}
		
		Token t = type.type();
		switch (t.kind) {
		case EtchGrammarConstants.VOID:
			return "void";
		case EtchGrammarConstants.BOOLEAN:
			return (etch_type ? "etch_boolean" : "boolean");
		case EtchGrammarConstants.BYTE:
			return (etch_type ? "etch_int8" : "byte");
		case EtchGrammarConstants.SHORT:
			return (etch_type ? "etch_int16" : "short");
		case EtchGrammarConstants.INT:
			return (etch_type ? "etch_int32" : "int");
		case EtchGrammarConstants.LONG:
			return (etch_type ? "etch_int64" : "int64");
		case EtchGrammarConstants.FLOAT:
			return (etch_type ? "etch_float" : "float");
		case EtchGrammarConstants.DOUBLE:
			return (etch_type ? "etch_double" : "double");
		case EtchGrammarConstants.STRING:
			return (etch_type ? "etch_string" : "etch_string*");
		case EtchGrammarConstants.OBJECT:
			return (etch_type ? "etch_object" : "etch_object*");
		default: {
			// we have to use a fully qualified name here.
			// find the actual type...
			Named<?> n = type.intf().get(t.image);
			System.out.println("Token:" + t.image);
			if (n == null)
				throw new IllegalArgumentException(String.format(
						"undefined or ambiguous name at line %d: %s",
						t.beginLine, t.image));

			
			if (n.isEnumx()) 
			{
				return n.efqname(this) + (etch_type ? "" : "_enum");
			}
			else
			{
				return (addStruct ? "struct " : "") + n.efqname(this) + (etch_type ? "" : "*");
			}
		}
		}
	}

	/**
	 * @param type
	 *            the etch type
	 * @return the fundamental native reference type for c. so etch int -> java
	 *         etch_int32*, while etch string -> etch_string*.
	 */
	public String getPointerTypeName(TypeRef type) {
		if (type.isArray())
			return "etch_arraytype*";
		Token t = type.type();

		switch (t.kind) {
		case EtchGrammarConstants.VOID:
			return "void*";
		case EtchGrammarConstants.BOOLEAN:
			return "etch_boolean*";
		case EtchGrammarConstants.BYTE:
			return "etch_byte*";
		case EtchGrammarConstants.SHORT:
			return "etch_int16*";
		case EtchGrammarConstants.INT:
			return "etch_int32*";
		case EtchGrammarConstants.LONG:
			return "etch_int64*";
		case EtchGrammarConstants.FLOAT:
			return "etch_float*";
		case EtchGrammarConstants.DOUBLE:
			return "etch_double*";
		case EtchGrammarConstants.STRING:
			return "etch_string*";
		case EtchGrammarConstants.OBJECT:
			return "etch_object*";
		default: {
			// we have to use a fully qualified name here.
			// find the actual type...
			Named<?> n = type.intf().get(t.image);
			if (n == null)
				throw new IllegalArgumentException(String.format(
						"undefined or ambiguous name at line %d: %s",
						t.beginLine, t.image));
			return n.efqname(this) + "*";
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
		return "c";
	}

	@Override
	public String enum_efqname(String fqname, String moduleName,
			String serviceName, String enumName) {
		// return serviceName.toLowerCase() + "_" + enumName.toLowerCase();
		return serviceName.toLowerCase() + "_" + enumName;
	}

	@Override
	public String except_efqname(String fqname, String moduleName,
			String serviceName, String exceptName) {
		// return serviceName.toLowerCase() + "_" + exceptName.toLowerCase();
		return serviceName.toLowerCase() + "_" + exceptName;
	}

	@Override
	public String struct_efqname(String fqname, String moduleName,
			String serviceName, String structName) {
		// return serviceName.toLowerCase() + "_" + structName.toLowerCase();
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
		history = new HashSet<String>();
		return true;
	}

	public boolean addStringToHistory(String s) {
		history.add(s);
		return true;
	}

	public boolean historyContains(String s) {
		return history.contains(s);
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
		sb.append("String.format( ");
		sb.append("\"");
		for (FmtItem i : list) {
			if (i instanceof FieldItem) {
				sb.append("%s");
			} else {
				escape(sb, ((StringItem) i).value());
			}
		}
		sb.append("\"");
		for (FmtItem i : list) {
			if (i instanceof FieldItem) {
				sb.append(", ");
				sb.append(((FieldItem) i).value());
			}
		}
		sb.append(" )");
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

	public String getValidator(Service service, Named<?> named) {
		if (named instanceof Parameter) {
			Parameter param = (Parameter) named;
			TypeRef type = param.type();

			if (type.isBuiltin())
				return String.format("(etch_object*)etchvtor_%s_get( %d )", getValidatorStringForParam(param),
						type.dim());

			Named<?> n = type.getNamed(type.intf());

			if (n.isBuiltin()) {
				Builtin b = (Builtin) n;
				String cn = b.className();

				int i = cn.indexOf('<');
				if (i >= 0)
					cn = cn.substring(0, i);
				
				String classid = getClassIDBuiltin(b);
				String messagetype = getMessageTypeBuiltin(b);
				return String.format(
						"(etch_object*)etchvtor_custom_get(%s, CLASSID_%s, builtins._mt_%s,%d)",
							getEtchTypeForBuiltin(b),classid, messagetype, getDimensionForBuiltin(b));
			}


			if (n.isStruct() || n.isExcept() || n.isEnumx())
				return String.format(
						"(etch_object*)etchvtor_custom_get(ETCHTYPEB_USER, get_dynamic_classid_unique(&CLASSID_%s), %s_valufact_get_static()->_mt_%s, %d)", 
								 n.efqname(this).toUpperCase(), getDefiningServiceNameOf(type), n.efqname(this), type.dim());

			if (n.isExtern())
			{
				Assertion.check(false,"extern type not supported for " + n );
			}
			else
			{
				Assertion.check(false,"unknown type " + n);
			}

			return "";
		}

		if (named instanceof Thrown) {
			Thrown thrown = (Thrown) named;
			Except e = (Except) thrown.getNamed();
			return String.format("(etch_object*)etchvtor_custom_get( ETCHTYPEB_USER,get_dynamic_classid_unique(&%s), %s_valufact_get_static()->_mt_%s_%s,0)", 
						getClassIdVarName(thrown),getExcept(thrown).service().name().toString().toLowerCase(),getExcept(thrown).service().name().toString().toLowerCase(),thrown);
		}

		if (named instanceof Item)
			return "(etch_object*)etchvtor_boolean_get( 0 )";

		return "null";
	}

	private int getDimensionForBuiltin(Builtin b) {
		if(b.bindingName().equals("etch_date"))
			return 0;
		if(b.bindingName().equals("etch_arraylist"))
			return 1;
		if(b.bindingName().equals("etch_hashtable"))
			return 0;
		if(b.bindingName().equals("etch_set"))
			return 0;

		return 0;
	}

	private String getEtchTypeForBuiltin(Builtin b) {
		if(b.bindingName().equals("etch_date"))
			return "ETCHTYPEB_PRIMITIVE";
		if(b.bindingName().equals("etch_arraylist"))
			return "ETCHTYPEB_ETCHLIST";		
		if(b.bindingName().equals("etch_hashtable"))
			return "ETCHTYPEB_ETCHMAP";
		if(b.bindingName().equals("etch_set"))
			return "ETCHTYPEB_ETCHSET";

		return "ETCHTYPEB_USER";
		
	}

	private String getMessageTypeBuiltin(Builtin b) {
		if(b.bindingName().equals("etch_date"))
			return "_etch_datetime";
		if(b.bindingName().equals("etch_arraylist"))
			return "_etch_list";		
		if(b.bindingName().equals("etch_hashtable"))
			return "_etch_map";
		if(b.bindingName().equals("etch_set"))
			return "_etch_set";
		return "NULL";
	}

	private String getClassIDBuiltin(Builtin b) {
		if(b.bindingName().equals("etch_date"))
			return "DATE";
		if(b.bindingName().equals("etch_arraylist"))
			return "ETCH_LIST";		
		if(b.bindingName().equals("etch_hashtable"))
			return "ETCH_MAP";
		if(b.bindingName().equals("etch_set"))
			return "ETCH_SET";

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
		addBuiltin( service, newName( "List" ), "etch_arraylist", true );
		addBuiltin( service, newName( "Map" ), "etch_hashtable", true );
		addBuiltin( service, newName( "Set" ), "etch_set", true );
		addBuiltin(service, newName("Datetime"), "etch_date", false);
	}

	public String getValidatorStringForParam(Parameter param) {
		TypeRef type = param.type();
		return getValidatorStringForParam(type);
	}

	public String getValidatorStringForParam(TypeRef type) {
		switch (type.type().kind) {
		case EtchGrammarConstants.BOOLEAN:
			return "boolean";
		case EtchGrammarConstants.BYTE:
			return "byte";
		case EtchGrammarConstants.INT:
			return "int32";
		case EtchGrammarConstants.SHORT:
			return "int16";
		case EtchGrammarConstants.DOUBLE:
			return "double";
		case EtchGrammarConstants.FLOAT:
			return "float";
		case EtchGrammarConstants.LONG:
			return "int64";
		case EtchGrammarConstants.STRING:
			return "string";
		}
		// what should be the default here?
		return "object";
	}

	public String getValidatorStringForParam(Message param) {
		return getValidatorStringForParam(param.type());
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
		return isCustomType(param.type());
	}

	public boolean isRefType(TypeRef param) {
		if(param == null)
			return true;
		switch(param.type().kind) {
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
		return "CLASSID_" + n.efqname(this).toUpperCase();
	}

	public String getClassIdVarName(Thrown t) {
		Except p = (Except) t.getNamed();
		return "CLASSID_" + p.service().name().toString().toUpperCase() + "_"
				+ p.name().toString().toUpperCase();
	}

	public Except getExcept(Thrown t) {
		Except e = (Except) t.getNamed();
		return e;
	}

	public String getClassIdVarName(Message n) {
		Named<?> na = n.type().getNamed(n.type().intf());
		return "CLASSID_" + na.efqname(this).toUpperCase();
	}

	public String getDefiningServiceNameOf(TypeRef type) {
		Named<?> na = type.getNamed(type.intf());
		return na.parent().name().name.toLowerCase();
	}

	@Override
	public String getValidator(Named<?> named) {
		return getValidator(null, named);
	}

}
