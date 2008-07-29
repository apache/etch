using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Transport;

namespace etch.bindings.csharp.examples.cuae
{
    abstract public class MyCuaeHelper : CuaeHelper
    {

        public static Transport<SessionListener> NewListener( string uri,		
			Resources resources, MyCuaeServerFactory implFactory )
		{
			Resources res = InitResources( resources );
			if (!res.ContainsKey( Messagizer.VALUE_FACTORY ))
			res.Add( Messagizer.VALUE_FACTORY, new MyValueFactoryCuae() );					
			return TransportFactory.GetListener( uri, res, new MyServerFactory( uri, res, implFactory ) );
		}
		
		public new class MyServerFactory : ServerFactory
		{
		
			private string _uri;
			private Resources _resources;
			private MyCuaeServerFactory _implFactory;
			
			public MyServerFactory( string uri, Resources resources, MyCuaeServerFactory implFactory ) 
			{
				_uri = uri;
				_resources = resources;
				_implFactory = implFactory;
			}
		
			public void NewServer( DeliveryService d )
			{
				
				Pool qp = (Pool) _resources[ QUEUED_POOL ];
				Pool fp = (Pool) _resources[ FREE_POOL ];
                MyValueFactoryCuae vf = (MyValueFactoryCuae) _resources
					.Get( Messagizer.VALUE_FACTORY );
			    _implFactory.NewMyCuaeServer(d, qp, fp, new MyValueFactoryCuae());
			}
			
		}

       

       public interface MyCuaeServerFactory
	    {
		/**
		 * @param svc
		 * @param qp
		 * @param fp
		 * @param vf
		 * @return a StubIntf that accepts and processes server messages.
		 */
		    SessionMessage NewMyCuaeServer( DeliveryService svc, Pool qp, Pool fp,
			MyValueFactoryCuae vf );
	    }
    }
}
