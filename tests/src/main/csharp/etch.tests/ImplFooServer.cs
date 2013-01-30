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
using org.apache.etch.tests.types.Bar;
using org.apache.etch.tests.types.Baz;
using org.apache.etch.tests.types.Foo;

/**
 * Call to message translator for FooServer.
 */
namespace etch.tests
{

	public class ImplFooServer : BaseFooServer
	{

		/// 
		/// <summary>Constructs the ImplFooServer.</summary>
		///
 		/// <param name=client></param>
 		///
		public ImplFooServer(RemoteFooClient client )
		{
		    this.client = client;
            _delegate_foo1 = Myfoo1;
            _delegate_foo2 = Myfoo2;
            _delegate_bar1 = Mybar1;
            _delegate_bar2 = Mybar2;
            _delegate_baz1 = Mybaz1;
            _delegate_baz2 = Mybaz2;
		}

	    private readonly RemoteFooClient client;

        ///
        /// <summary>Begins the call to foo1.</summary>
        ///
        public bool? Myfoo1(
                FooData fooData
            )
        {
            
            Console.WriteLine("foo1 called with " + fooData);
            return true;
        }
        ///
        /// <summary>Begins the call to foo2.</summary>
        ///
        public bool? Myfoo2(
                FooData fooData
            )
        {

            Console.WriteLine("foo2 called with " + fooData);
            return false;
        }



        public bool? Mybaz1(
           BazData bazData
        )
        {
            Console.WriteLine("baz1 called with " + bazData);
            return true;
        }

        public bool? Mybaz2(
           BazData bazData
        )
        {
            Console.WriteLine("baz2 called with " + bazData);
            return false;
        }


        public bool? Mybar1(
            BarData barData
        )
        {
            Console.WriteLine("bar1 called with " + barData);
            return true;
        }

        public bool? Mybar2(
            BarData barData
        )
        {
            Console.WriteLine("bar2 called with " + barData);
            return false;
        }
		
	}
}