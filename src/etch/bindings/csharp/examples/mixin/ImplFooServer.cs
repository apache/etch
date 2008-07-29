// This file automatically generated by:
//   @EtchVersion@ (@EtchBuildTag@) / csharp @EtchVersion@ (@EtchBuildTag@)
//   Tue Apr 15 15:02:12 CDT 2008

using etch.bindings.csharp.examples.mixin.bar.types.Bar;
using etch.bindings.csharp.examples.mixin.baz.types.Baz;
using Etch.Util;

using System;
using etch.bindings.csharp.examples.mixin.types.Foo;
/**
 * Call to message translator for FooServer.
 */
namespace etch.bindings.csharp.examples.mixin
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