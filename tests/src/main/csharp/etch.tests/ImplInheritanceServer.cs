// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Inheritance;

/**
 * Call to message translator for InheritanceServer.
 */
namespace etch.tests
{

	public class ImplInheritanceServer : BaseInheritanceServer
	{

		/// 
		/// <summary>Constructs the ImplInheritanceServer.</summary>
		///
 		/// <param name=client></param>
 		///
		public ImplInheritanceServer(RemoteInheritanceClient client )
		{
		    this.client = client;
            _delegate_f1 = Myf1;
            _delegate_f2 = Myf2;
            _delegate_f3 = Myf3;
            _delegate_f4 = Myf4;
            _delegate_f5 = Myf5;
            _delegate_f6 = Myf6;
            _delegate_f7 = Myf7;
            _delegate_f8 = Myf8;
		}

	    private readonly RemoteInheritanceClient client;
	
		// Implement delegates or provide implementation of client
        ///
        /// <summary>Begins the call to f1.</summary>
        ///
        public Object Myf1(
                Object v
            )
        {
            return v;
        }
        ///
        /// <summary>Begins the call to f2.</summary>
        ///
        public S1 Myf2(
                S1 v
            )
        {
            return v;
        }
        ///
        /// <summary>Begins the call to f3.</summary>
        ///
        public S2 Myf3(
                S2 v
            )
        {
            return v;
        }
        ///
        /// <summary>Begins the call to f4.</summary>
        ///
        public S3 Myf4(
                S3 v
            )
        {
            return v;
        }
        ///
        /// <summary>Begins the call to f5.</summary>
        ///
        public Object[] Myf5(
                Object[] v
            )
        {
            return v;
        }
        ///
        /// <summary>Begins the call to f6.</summary>
        ///
        public S1[] Myf6(
                S1[] v
            )
        {
            return v;
        }
        ///
        /// <summary>Begins the call to f7.</summary>
        ///
        public S2[] Myf7(
                S2[] v
            )
        {
            return v;
        }
        ///
        /// <summary>Begins the call to f8.</summary>
        ///
        public S3[] Myf8(
                S3[] v
            )
        {
            return v;
        }
		
	}
}