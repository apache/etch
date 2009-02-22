// This file automatically generated by:
//   Apache Etch 1.1.0-incubating (LOCAL-0) / java 1.1.0-incubating (LOCAL-0)
//   Sat Feb 21 22:11:51 CST 2009
// This file is automatically created and should not be edited!

package etch.services.ns;


/**
 * Call to message translator for NameServiceClient.
 */
@SuppressWarnings("unused")
public final class RemoteNameServiceClient extends RemoteNameService implements NameServiceClient
{
	/**
	 * Constructs the RemoteNameServiceClient.
	 *
	 * @param svc
	 * @param vf
	 */
	public RemoteNameServiceClient( org.apache.etch.bindings.java.support.DeliveryService svc, org.apache.etch.bindings.java.msg.ValueFactory vf )
	{
		super( svc, vf );
	}

	/**
	 * {@link _Async} class instance used to hide asynchronous message
	 * implementation. Use this to invoke the asynchronous message
	 * implementations.
	 */
	public final _Async _async = new _Async();

	/**
	 * {@link _Async} class instance used to hide asynchronous message
	 * implementation. This is here for backwards compatibility only, use
	 * {@link #_async} instead.
	 * @deprecated
	 */
	@Deprecated
	public final _Async _inner = _async;

	public final void entryChanged(
		String query
		, etch.services.ns.NameService.Entry entry
	)
	{
		_async._end_entryChanged( _async._begin_entryChanged(
			query
			, entry
		) );
	}

	/**
	 * Asynchronous implementation of service methods.
	 */
	public final class _Async extends RemoteNameService._Async
	{

		/**
		 * Begins a call to entryChanged.
		 *
		 * Notifies the client of a change in an entry. The entry might have been
		 * created, updated, or removed.
		 * 
		 * Note: while this might have normally been an event, we made it a call
		 * to slow down the processing of what might otherwise be a rather large
		 * change set.
		 * 
		 * Note: when keeping track of entries, always keep the one with the latest
		 * lastUpdate.
		 * 
		 * @param query the query which triggered the notification.
		 * 
		 * @param entry the entry which has changed.
		 * @return mailbox used to retrieve the result using _end_entryChanged.
		 * @see RemoteNameServiceClient#entryChanged
		 * @see #_end_entryChanged
		 */
		public final org.apache.etch.bindings.java.support.Mailbox _begin_entryChanged(
			String query
			, etch.services.ns.NameService.Entry entry
		)
		{
			org.apache.etch.bindings.java.msg.Message _msg = _newMessage( ValueFactoryNameService._mt_etch_services_ns_NameService_entryChanged );
			_msg.put( ValueFactoryNameService._mf_query, query );
			_msg.put( ValueFactoryNameService._mf_entry, entry );
			return _begincall( _msg );
		}
		
		/**
		 * Ends a call to entryChanged.
		 *
		 * Notifies the client of a change in an entry. The entry might have been
		 * created, updated, or removed.
		 * 
		 * Note: while this might have normally been an event, we made it a call
		 * to slow down the processing of what might otherwise be a rather large
		 * change set.
		 * 
		 * Note: when keeping track of entries, always keep the one with the latest
		 * lastUpdate.
		 * 
		 * @param mb mailbox returned by _begin_entryChanged.
		 *
		 * @see RemoteNameServiceClient#entryChanged
		 * @see #_begin_entryChanged
		 */
		public final void _end_entryChanged( org.apache.etch.bindings.java.support.Mailbox mb )
		{
			try
			{
				_endcall( mb,
					ValueFactoryNameService._mt_etch_services_ns_NameService__result_entryChanged  );
			}
			catch ( Exception e )
			{
				if (e instanceof RuntimeException) throw (RuntimeException) e;
				throw new RuntimeException( "unexpected exception from peer: "+e, e );
			}
		}

		// Mixin Methods
	}
}
