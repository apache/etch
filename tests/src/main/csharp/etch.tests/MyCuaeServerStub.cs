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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace etch.tests
{
    public class MyCuaeServerStub : StubBase
    {

        static MyCuaeServerStub()
        {
            StubHelperRun helper1 = delegate(DeliveryService _src, object obj, Who _sender, Message _msg)
                                        {
                                            MyCuaeServerImpl impl = (MyCuaeServerImpl)obj;
                                            impl.dispatch(_sender,_msg);
                                        };
    //        MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit.SetStubHelper(new StubHelper(helper1, StubHelper.AsyncMode.NONE));
    //        MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit3.SetStubHelper(new StubHelper(helper1, StubHelper.AsyncMode.NONE));

            MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit.SetStubHelper(helper1);
                   MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit3.SetStubHelper(helper1);
        }


        public MyCuaeServerStub(DeliveryService svc, Pool queued, Pool free, MyValueFactoryCuae vf)
            : base(svc, new MyCuaeServerImpl(svc, vf), queued, free)
        {

        }

        public class MyCuaeServerImpl
        {
            /**
             * @param svc
             * @param vf 
             */
            public MyCuaeServerImpl(DeliveryService svc, MyValueFactoryCuae vf)
            {
            
                this.svc = svc;
                this.vf = vf;
            }

       

            private DeliveryService svc;

            private MyValueFactoryCuae vf;

            /**
             * Handle messages from peer.
             * @param sender
             * @param msg
             * @throws Exception
             */
            public void dispatch(Who sender, Message msg)
            {
                Console.WriteLine("msg = " + msg);
                if (msg.IsType(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit))
                {
                    StructValue req = (StructValue)msg.Get(MyValueFactoryCuae._mf_req);
                    MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Request.CheckIsAssignableFrom(req.GetXType);
                    int? code = (int?)req.Get(MyValueFactoryCuae._mf_code);

                   // String m;
                    StructValue resp;
                    switch (code.GetValueOrDefault())
                    {
                        case 23:
                            resp = new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response, vf);
                            resp.Add(MyValueFactoryCuae._mf_msg,"foo");
                            delayDoit2a(7);
                            break;

                        case 19:
                            resp =
                                new StructValue(
                                    MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode, vf);
                            resp.Add(MyValueFactoryCuae._mf_msg, "bar");
                            resp.Add(MyValueFactoryCuae._mf_code, 54);
                            delayDoit2b(11,"heaven");
                            break;

                        case 13:
                            resp = new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response, vf);
                            resp.Add(MyValueFactoryCuae._mf_msg, "baz");
                            delayDoit2a(99);
                            break;

                        default:
                            resp =
                               new StructValue(
                                   MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode, vf);
                            resp.Add(MyValueFactoryCuae._mf_msg, " unknown code " + code);
                            resp.Add(MyValueFactoryCuae._mf_code, 63);
                            delayDoit2b(23, "killer bee");
                            break;
                    }

                    

                    Message rmsg = msg.Reply();
                    Console.WriteLine("rmsg = " + rmsg);
                    Console.WriteLine("resp = " + resp);
                    rmsg.Add(MyValueFactoryCuae._mf_result, resp);
                    svc.TransportMessage(sender,rmsg);
                    return;
                }

                if (msg.IsType(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit3))
                {
                    StructValue[] reqs = (StructValue[]) msg.Get(MyValueFactoryCuae._mf_req);
                    StructValue req = reqs[0];
                    MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Request.CheckIsAssignableFrom(
                        req.GetXType);
                    int? code = (int?) req.Get(MyValueFactoryCuae._mf_code);

                    StructValue resp;
                    switch (code.GetValueOrDefault())
                    {
                        case 23:
                            resp =
                                new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response, vf);
                            resp.Add(MyValueFactoryCuae._mf_msg, "foo");
                            delayDoit2a(7);
                            break;

                        case 19:
                            resp = new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode, vf);
                                resp.Add( MyValueFactoryCuae._mf_msg, "bar" );
                                resp.Add( MyValueFactoryCuae._mf_code, 54 );
                                delayDoit2b( 11, "heaven" );
                                break; 

                        case 13:
                            resp =
                                new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response, vf);
                            resp.Add(MyValueFactoryCuae._mf_msg, "baz");
                            delayDoit2a(99);
                            break;

                          	default:
                                resp = new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode, vf);
                                    resp.Add( MyValueFactoryCuae._mf_msg, "unknown code " + code );
                                    resp.Add( MyValueFactoryCuae._mf_code, 63 );
                                    delayDoit2b( 23, "killer bee" );
                                    break; 
                    }

                    Message rmsg = msg.Reply();
                    Console.WriteLine("rmsg = " + rmsg);
                    Console.WriteLine("resp = " + resp);
                    rmsg.Add(MyValueFactoryCuae._mf_result, new StructValue[] { resp });
                    svc.TransportMessage(sender,rmsg);
                    return;


                }


                throw new Exception("unknown msg type " + msg);
            }

            private void delayDoit2a(int code)
            {
                try
                {
                    TodoManager.AddTodo(new ToDoObj(svc,vf,code));

                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }

            private void delayDoit2b(int code, string msg)
            {
                try
                {
                    TodoManager.AddTodo(new ToDoObj1(svc, vf, code,msg));

                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
        }
        
    }

    public class ToDoObj : Todo
    {
        private DeliveryService svc;

        private MyValueFactoryCuae vf;

        private int code;

        public ToDoObj(DeliveryService svc, MyValueFactoryCuae vf, int code)
        {
            this.svc = svc;
            this.vf = vf;
            this.code = code;
        }
        
        
        public void Doit( TodoManager mgr ) 
		{
            StructValue req = new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Request, vf);
						req.Add( MyValueFactoryCuae._mf_code, code );
						
						Message msg = new Message( MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeClient_doit2, vf );
						msg.Add( MyValueFactoryCuae._mf_req, req );
						
						Mailbox mb = svc.BeginCall( msg );
						Object value = svc.EndCall( mb, MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2 );
						
						if (value is Exception)
						{
							Exception e = (Exception) value;
							throw e;
						}
						
						StructValue resp = (StructValue) value;
				//		resp.CheckType( MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response );
                        MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response.CheckIsAssignableFrom(resp.GetXType);
                        Console.WriteLine( "**** delayDoit2a: req {0} response {1}\n", code, resp );
			//			String m = (String) resp.Get( MyValueFactoryCuae._mf_msg );
						
					}

					public void Exception( TodoManager mgr, Exception e )
					{
						Console.WriteLine(e);
					}
        
    }

    public class ToDoObj1 : Todo
    {
        private DeliveryService svc;

        private MyValueFactoryCuae vf;

        private int code;

        private string message;

        public ToDoObj1(DeliveryService svc, MyValueFactoryCuae vf, int code, string msg)
        {
            this.svc = svc;
            this.vf = vf;
            this.code = code;
            this.message = msg;
        }


        public void Doit(TodoManager mgr)
        {
            StructValue req = new StructValue(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_ReqWithMessage, vf);
            req.Add(MyValueFactoryCuae._mf_code, code);
            req.Add(MyValueFactoryCuae._mf_msg, message);

            Message msg = new Message(MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeClient_doit2, vf);
            msg.Add(MyValueFactoryCuae._mf_req, req);

            Mailbox mb = svc.BeginCall(msg);
            Object value = svc.EndCall(mb, MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2 );

            if (value is Exception)
            {
                Exception e = (Exception)value;
                throw e;
            }

            StructValue resp = (StructValue)value;
            //		resp.CheckType( MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response );
            MyValueFactoryCuae._mt_etch_bindings_csharp_examples_cuae_Cuae_Response.CheckIsAssignableFrom(resp.GetXType);
            Console.WriteLine("**** delayDoit2b: req {0} response {1}\n", code, resp);
            //			String m = (String) resp.Get( MyValueFactoryCuae._mf_msg );

        }

        public void Exception(TodoManager mgr, Exception e)
        {
            Console.WriteLine(e);
        }

    }
}
