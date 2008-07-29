// This file automatically generated by:
//   @EtchVersion@ (@EtchBuildTag@) / csharp @EtchVersion@ (@EtchBuildTag@)
//   Tue Apr 15 15:02:10 CDT 2008


using System;
using Etch.Support;
using Etch.Transport;

/**
 * Call to message translator for ClosingServer.
 */
namespace etch.bindings.csharp.examples.closing
{

	public class ImplClosingServer : BaseClosingServer,ObjSession
	{

		/// 
		/// <summary>Constructs the ImplClosingServer.</summary>
		///
 		/// <param name=client></param>
 		///
		public ImplClosingServer(RemoteClosingClient client )
		{
		    this.client = client;
            _delegate_close = Myclose;

		}

	    private readonly RemoteClosingClient client;

        public void Myclose()
        {
            Console.WriteLine("ImplClosingServer.sayingStopToClient");
            client._TransportControl(TransportConsts.STOP, null);

            Console.WriteLine("ImplClosingServer.saidStopToClient");
        }

       

        public new void _SessionNotify(Object eventObj)
        {
            Console.WriteLine("ImplClosingServer _sessionNotify" + eventObj);
        }

		
	}
}