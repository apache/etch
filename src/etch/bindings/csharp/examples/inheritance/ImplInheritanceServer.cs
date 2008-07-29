// This file automatically generated by:
//   @EtchVersion@ (@EtchBuildTag@) / csharp @EtchVersion@ (@EtchBuildTag@)
//   Tue Apr 15 15:02:11 CDT 2008


using System;
using etch.bindings.csharp.examples.inheritance.types.Inheritance;
/**
 * Call to message translator for InheritanceServer.
 */
namespace etch.bindings.csharp.examples.inheritance
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