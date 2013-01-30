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

package org.apache.etch.tests;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.bindings.java.support.ObjSession;
import org.apache.etch.bindings.java.support.Pool;
import org.apache.etch.bindings.java.support.StubBase;
import org.apache.etch.bindings.java.support.StubHelper;
import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;
import org.apache.etch.util.core.Who;


/**
 * Hand-built CuaeServerStub which dispatches to our raw impl.
 */
public class MyCuaeServerStub extends StubBase<Object>
{
	static
	{
		StubHelper<MyCuaeServerImpl> syncHelper = new StubHelper<MyCuaeServerImpl>()
		{
			@Override
			public void run( DeliveryService svc, MyCuaeServerImpl obj, Who sender, Message msg ) throws Exception
			{
				obj.dispatch( sender, msg );
			}
		};
		
		MyValueFactoryCuae._mt_etch_examples_cuae_CuaeServer_doit1
			.setStubHelper( syncHelper );
		
		MyValueFactoryCuae._mt_etch_examples_cuae_CuaeServer_test1
			.setStubHelper( syncHelper );
		
		MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_doit3
			.setStubHelper( syncHelper );
		
		MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_doit4
			.setStubHelper( syncHelper );
		
		MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_test3
			.setStubHelper( syncHelper );
	}
	
	/**
	 * Constructs MyCuaeServerStub.
	 * @param svc
	 * @param queued
	 * @param free
	 * @param vf 
	 */
	public MyCuaeServerStub( DeliveryService svc, Pool queued, Pool free,
		MyValueFactoryCuae vf )
	{
		super( svc, new MyCuaeServerImpl( svc, vf ), queued, free );
	}

	/**
	 * Hand-build CuaeServerImpl which is sent messages from CuaeServerStub.
	 */
	public static class MyCuaeServerImpl implements ObjSession
	{
		/**
		 * @param svc
		 * @param vf 
		 */
		public MyCuaeServerImpl( DeliveryService svc, MyValueFactoryCuae vf )
		{
			this.svc = svc;
			this.vf = vf;
		}
		
		/** */
		final DeliveryService svc;
		
		/** */
		final MyValueFactoryCuae vf;
		
		/**
		 * Handle messages from peer.
		 * @param sender
		 * @param msg
		 * @throws Exception
		 */
		public void dispatch( Who sender, Message msg ) throws Exception
		{
			System.out.println( "msg = "+msg );
			if (msg.isType( MyValueFactoryCuae._mt_etch_examples_cuae_CuaeServer_doit1 ))
			{
				StructValue req = (StructValue) msg.get( MyValueFactoryCuae._mf_req );
				MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Request.checkIsAssignableFrom( req.type() );
				int code = (Integer) req.get( MyValueFactoryCuae._mf_code );
				
				StructValue resp;
				switch (code)
				{
					case 23:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Response, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "foo" );
						delayDoit2a( 7 );
						break;
					
					case 19:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_RespWithCode, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "bar" );
						resp.put( MyValueFactoryCuae._mf_code, 54 );
						delayDoit2b( 11, "heaven" );
						break;
					
					case 13:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Response, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "baz" );
						delayDoit2a( 99 );
						break;
					
					default:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_RespWithCode, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "unknown code " + code );
						resp.put( MyValueFactoryCuae._mf_code, 63 );
						delayDoit2b( 23, "killer bee" );
						break;
				}
				
				
				Message rmsg = msg.reply();
				System.out.println( "rmsg = "+rmsg );
				System.out.println( "resp = "+resp );
				rmsg.put( MyValueFactoryCuae._mf_result, resp );
				svc.transportMessage( sender, rmsg );
				return;
			}
			
			if (msg.isType( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_doit3 ))
			{
				StructValue[] reqs = (StructValue[]) msg.get( MyValueFactoryCuae._mf_req );
				StructValue req = reqs[0];
				MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Request.checkIsAssignableFrom( req.type() );
				int code = (Integer) req.get( MyValueFactoryCuae._mf_code );
				
				StructValue resp;
				switch (code)
				{
					case 23:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Response, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "foo" );
						delayDoit2a( 7 );
						break;
					
					case 19:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_RespWithCode, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "bar" );
						resp.put( MyValueFactoryCuae._mf_code, 54 );
						delayDoit2b( 11, "heaven" );
						break;
					
					case 13:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Response, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "baz" );
						delayDoit2a( 99 );
						break;
					
					default:
						resp = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_RespWithCode, vf );
						resp.put( MyValueFactoryCuae._mf_msg, "unknown code " + code );
						resp.put( MyValueFactoryCuae._mf_code, 63 );
						delayDoit2b( 23, "killer bee" );
						break;
				}
				
				
				Message rmsg = msg.reply();
				System.out.println( "rmsg = "+rmsg );
				System.out.println( "resp = "+resp );
				rmsg.put( MyValueFactoryCuae._mf_result, new StructValue[] { resp } );
				svc.transportMessage( sender, rmsg );
				return;
			}
			
			throw new IllegalArgumentException( "unknown msg type "+msg );
		}

		private void delayDoit2a( final int code )
		{
			try
			{
				TodoManager.addTodo( new Todo()
				{
					public void doit( TodoManager mgr ) throws Exception
					{
						StructValue req = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Request, vf );
						req.put( MyValueFactoryCuae._mf_code, code );
						
						Message msg = new Message( MyValueFactoryCuae._mt_etch_examples_cuae_CuaeClient_doit2, vf );
						msg.put( MyValueFactoryCuae._mf_req, req );
						
						Mailbox mb = svc.begincall( msg );
						Object value = svc.endcall( mb, MyValueFactoryCuae._mt_etch_examples_cuae_CuaeServer__result_doit2 );
						
						if (value instanceof Exception)
						{
							Exception e = (Exception) value;
							e.fillInStackTrace();
							throw e;
						}
						
						StructValue resp = (StructValue) value;
						MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Response.checkIsAssignableFrom( resp.type() );
						System.out.printf( "**** delayDoit2a: req %d response %s\n", code, resp );
					}

					public void exception( TodoManager mgr, Exception e )
					{
						e.printStackTrace();
					}
				} );
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
		}

		private void delayDoit2b( final int code, final String message )
		{
			try
			{
				TodoManager.addTodo( new Todo()
				{
					public void doit( TodoManager mgr ) throws Exception
					{
						StructValue req = new StructValue( MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_ReqWithMessage, vf );
						req.put( MyValueFactoryCuae._mf_code, code );
						req.put( MyValueFactoryCuae._mf_msg, message );
						
						Message msg = new Message( MyValueFactoryCuae._mt_etch_examples_cuae_CuaeClient_doit2, vf );
						msg.put( MyValueFactoryCuae._mf_req, req );
						
						Mailbox mb = svc.begincall( msg );
						Object value = svc.endcall( mb, MyValueFactoryCuae._mt_etch_examples_cuae_CuaeServer__result_doit2 );
						
						if (value instanceof Exception)
						{
							Exception e = (Exception) value;
							e.fillInStackTrace();
							throw e;
						}
						
						StructValue resp = (StructValue) value;
						MyValueFactoryCuae._mt_etch_examples_cuae_Cuae_Response.checkIsAssignableFrom( resp.type() );
						System.out.printf( "**** delayDoit2b: req %d response %s\n", code, resp );
					}

					public void exception( TodoManager mgr, Exception e )
					{
						e.printStackTrace();
					}
				} );
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
		}

		public Object _sessionQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void _sessionControl( Object control, Object value )
			throws Exception
		{
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void _sessionNotify( Object event ) throws Exception
		{
			if (event instanceof Throwable)
				((Throwable) event).printStackTrace();
		}
	}
}
