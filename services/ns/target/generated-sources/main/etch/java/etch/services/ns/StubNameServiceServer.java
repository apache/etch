// This file automatically generated by:
//   Apache Etch 1.1.0-incubating (LOCAL-0) / java 1.1.0-incubating (LOCAL-0)
//   Sat Feb 21 22:11:51 CST 2009
// This file is automatically created and should not be edited!

package etch.services.ns;

import org.apache.etch.util.core.Who;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Pool;
import org.apache.etch.bindings.java.support.StubHelper;
import org.apache.etch.bindings.java.support._Etch_AuthException;
import org.apache.etch.bindings.java.support.Pool.PoolRunnable;

/**
 * Message to call translator for NameServiceServer.
 */
@SuppressWarnings("unused")
public class StubNameServiceServer extends StubNameService<NameServiceServer>
{
	/**
	 * Stub for NameServiceServer.
	 * @param svc the delivery service.
	 * @param obj the implementation of NameServiceServer responsive to requests.
	 * @param queued thread pool used to run AsyncMode.QUEUED methods.
	 * @param free thread pool used to run AsyncMode.FREE methods.
	 */
	public StubNameServiceServer( DeliveryService svc, NameServiceServer obj, Pool queued, Pool free )
	{
		super( svc, obj, queued, free );
	}
	
	static
	{
	}

	/**
	 * Method used to force static initialization.
	 */
	public static void init()
	{
		// nothing to do.
	}
	
	static
	{
		ValueFactoryNameService._mt_etch_services_ns_NameService_lookup.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					String source = (String) _msg.get( ValueFactoryNameService._mf_source );
					if (!_obj.canLookup(
						source
					))
					throw new _Etch_AuthException( "stub could not authorize lookup using canLookup" );
					_rmsg.put( ValueFactoryNameService._mf_result,
					_obj.lookup(
						source
					)
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_find.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					String query = (String) _msg.get( ValueFactoryNameService._mf_query );
					Integer offset = (Integer) _msg.get( ValueFactoryNameService._mf_offset );
					Integer count = (Integer) _msg.get( ValueFactoryNameService._mf_count );
					if (!_obj.canFind(
						query
					))
					throw new _Etch_AuthException( "stub could not authorize find using canFind" );
					_rmsg.put( ValueFactoryNameService._mf_result,
					_obj.find(
						query
						, offset
						, count
					)
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_register.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					String sourceUri = (String) _msg.get( ValueFactoryNameService._mf_sourceUri );
					java.util.Map<?, ?> qualities = (java.util.Map<?, ?>) _msg.get( ValueFactoryNameService._mf_qualities );
					String targetUri = (String) _msg.get( ValueFactoryNameService._mf_targetUri );
					Integer ttl = (Integer) _msg.get( ValueFactoryNameService._mf_ttl );
					if (!_obj.canRegister(
						sourceUri
						, qualities
						, targetUri
					))
					throw new _Etch_AuthException( "stub could not authorize register using canRegister" );
					_obj.register(
						sourceUri
						, qualities
						, targetUri
						, ttl
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_registerBulk.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					etch.services.ns.NameService.Entry[] entries = (etch.services.ns.NameService.Entry[]) _msg.get( ValueFactoryNameService._mf_entries );
					if (!_obj.canRegisterBulk(
						entries
					))
					throw new _Etch_AuthException( "stub could not authorize registerBulk using canRegisterBulk" );
					_obj.registerBulk(
						entries
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_unregister.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					String sourceUri = (String) _msg.get( ValueFactoryNameService._mf_sourceUri );
					Boolean deleteEntry = (Boolean) _msg.get( ValueFactoryNameService._mf_deleteEntry );
					if (!_obj.canUnregister(
						sourceUri
						, deleteEntry
					))
					throw new _Etch_AuthException( "stub could not authorize unregister using canUnregister" );
					_obj.unregister(
						sourceUri
						, deleteEntry
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_subscribe.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					String query = (String) _msg.get( ValueFactoryNameService._mf_query );
					if (!_obj.canFind(
						query
					))
					throw new _Etch_AuthException( "stub could not authorize subscribe using canFind" );
					_obj.subscribe(
						query
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_unsubscribe.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					_obj.unsubscribe(
						(String) _msg.get( ValueFactoryNameService._mf_query )
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_unsubscribeAll.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					_obj.unsubscribeAll(
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_canLookup.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					_rmsg.put( ValueFactoryNameService._mf_result,
					_obj.canLookup(
						(String) _msg.get( ValueFactoryNameService._mf_source )
					)
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_canFind.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					_rmsg.put( ValueFactoryNameService._mf_result,
					_obj.canFind(
						(String) _msg.get( ValueFactoryNameService._mf_query )
					)
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_canRegister.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					_rmsg.put( ValueFactoryNameService._mf_result,
					_obj.canRegister(
						(String) _msg.get( ValueFactoryNameService._mf_sourceUri )
						, (java.util.Map<?, ?>) _msg.get( ValueFactoryNameService._mf_qualities )
						, (String) _msg.get( ValueFactoryNameService._mf_targetUri )
					)
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_canUnregister.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					_rmsg.put( ValueFactoryNameService._mf_result,
					_obj.canUnregister(
						(String) _msg.get( ValueFactoryNameService._mf_sourceUri )
						, (Boolean) _msg.get( ValueFactoryNameService._mf_deleteEntry )
					)
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
		ValueFactoryNameService._mt_etch_services_ns_NameService_canRegisterBulk.setStubHelper( new StubHelper<NameServiceServer>()
		{
			public final void run( DeliveryService _svc, NameServiceServer _obj, Who _sender, Message _msg ) throws Exception
			{
				final Message _rmsg = _msg.reply();
				try
				{
					_rmsg.put( ValueFactoryNameService._mf_result,
					_obj.canRegisterBulk(
						(etch.services.ns.NameService.Entry[]) _msg.get( ValueFactoryNameService._mf_entries )
					)
					);
				}
				catch ( Exception e )
				{
					sessionNotify( _obj, e );
					_rmsg.put( ValueFactoryNameService._mf_result, e );
				}
				_svc.transportMessage( _sender, _rmsg );
			}
		} );
	}
}
