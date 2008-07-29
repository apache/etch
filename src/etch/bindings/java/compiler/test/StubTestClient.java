// This file automatically generated by:
//   @EtchVersion@ (@EtchBuildTag@) / java @EtchVersion@ (@EtchBuildTag@)
//   Mon Nov 12 14:29:21 CST 2007

package etch.bindings.java.compiler.test;

import etch.bindings.java.msg.Message;
import etch.bindings.java.support.MessageSource;
import etch.bindings.java.support.Pool;
import etch.bindings.java.support._Etch_AuthException;
import metreos.core.Who;
import etch.bindings.java.support.Pool.PoolRunnable;
import etch.bindings.java.util.StrIntHashMap;

/**
 * Message to call translator for TestClient.
 */
@SuppressWarnings("unused")
public class StubTestClient extends StubTest
{
	/**
	 * Stub for TestClient.
	 * @param src the source of messages.
	 * @param obj the implementation of TestClient responsive to requests.
	 * @param queued thread pool used to run AsyncReceiverMode.QUEUED methods.
	 * @param free thread pool used to run AsyncReceiverMode.FREE methods.
	 */
	public StubTestClient( MessageSource src, TestClient obj, Pool queued, Pool free )
	{
		super( src, obj, queued, free );
		this._obj = obj;
	}
	
	@SuppressWarnings("unused")
	private final TestClient _obj;

	@Override
	public boolean message( final Who _sender, final Message _msg )
		throws Exception
	{
		if (_msg == null)
		{
			super.message( _sender, _msg );
			return true;
		}

		switch (_msg.type().getId())
		{
			default:
				return super.message( _sender, _msg );
		}
	}
}
