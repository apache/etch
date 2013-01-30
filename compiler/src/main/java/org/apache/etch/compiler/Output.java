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
import java.io.IOException;
import java.io.OutputStream;

/**
 * Interface of an output container for the compiler.
 */
public interface Output
{
	/**
	 * Creates an OutputStream for the output file.
	 * @param fn
	 * @return a new OutputStream for the output file.
	 */
	public OutputStream newOutputStream( String fn );

	/**
	 * Creates a new Output to hold the output for the package.
	 * @param pn
	 * @return a new Output.
	 */
	public Output newPackage( String pn );
	
	/**
	 * Reports on the contents of the Output.
	 * @param what
	 */
	public void report( String what );

	/**
	 * Saves the files in this output container.
	 * @param outputDir the directory for the output files.
	 * @param noQualOutputDir if true do not qualify the file name with the
	 * module name.
	 * @param noFlattenPackages if true do not flatten the directory structure
	 * implied by the module name.
	 * @param force if true always overwrite files.
	 * @throws IOException 
	 */
	public void saveFiles( File outputDir, boolean noQualOutputDir,
		boolean noFlattenPackages, boolean force ) throws IOException;
}
