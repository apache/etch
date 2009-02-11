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

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Filter
{
    public class KeepAlive : AbstractMessageFilter, AlarmListener
    {


        public const String DELAY = "KeepAlive.delay";


        public const String COUNT = "KeepAlive.count";

        public KeepAlive(TransportMessage transport, URL uri, Resources resources)
            : base(transport, uri, resources)
        {

            delay = uri.GetIntegerTerm(DELAY, 15);
            if (delay <= 0)
                throw new ArgumentException("delay <= 0");

            count = uri.GetIntegerTerm(COUNT, 4);
            if (count <= 0)
                throw new ArgumentException("count <= 0");

            server = (Boolean) transport.TransportQuery(TransportConsts.IS_SERVER);

            //		Log.report( "KeepAliveInstalled",
            //			"delay", delay, "count", count, "server", server );

            vf = (ValueFactory)resources.Get(TransportConsts.VALUE_FACTORY);

            mf_delay = new Field("delay");
            mf_count = new Field("count");

            mt_request = new XType("_Etch_KeepAliveReq");
            mt_request.PutValidator(mf_delay, Validator_int.Get(0));
            mt_request.PutValidator(mf_count, Validator_int.Get(0));
            vf.AddType(mt_request);

            mt_response = new XType("_Etch_KeepAliveResp");
            vf.AddType(mt_response);

            mt_request.SetResult(mt_response);
        }

        private int delay;

        private int count;

        private readonly bool server;

        private readonly ValueFactory vf;

        private readonly Field mf_delay;

        private readonly Field mf_count;

        private readonly XType mt_request;

        private readonly XType mt_response;

        public int GetDelay()
        {
            return delay;
        }

        public int GetCount()
        {
            return count;
        }

       
        public bool GetServer()
        {
            return server;
        }


        public override bool SessionMessage(Who sender, Message msg)
        {
            if (msg.IsType(mt_request))
            {
                handleRequest(msg);
                return true;
            }

            if (msg.IsType(mt_response))
            {
                handleResponse(msg);
                return true;
            }

            return base.SessionMessage(sender, msg);
        }



        protected override bool SessionUp()
        {
            //		Log.report( "KeepAliveSessionUp", "server", server );
            up = true;
            AlarmManager.staticAdd(this, null, delay * 1000);
            tickle();
            return true;
        }

        protected override bool SessionDown()
        {
            //		Log.report( "KeepAliveSessionDown", "server", server );
            up = false;
            AlarmManager.staticRemove(this);
            return true;
        }

        private bool up;

        private void handleRequest(Message msg)
        {
            if (!server)
            {
                // we're a client that got a request, likely we're talking to a
                // server that is confused.
                return;
            }

            //Log.report( "GotKeepAliveReq", "msg", msg );
           // Console.WriteLine("GotKeepAliveReq:Msg " + msg);

            int? d = (int?)msg.Get(mf_delay);
            if (d != null && d.Value > 0)
            {
               //Console.WriteLine("KeepAliveResetDelay delay " +  d );
                delay = d.Value;
            }

            int? c = (int?)msg.Get(mf_count);
            if (c != null && c.Value > 0)
            {
               // Console.WriteLine( "KeepAliveResetCount count " + c );
                count = c.Value;
            }

            tickle();

            sendKeepAliveResp(msg);
        }

        private void handleResponse(Message msg)
        {
            if (server)
            {
                // we're a server that got a response, which means either we sent
                // a request (but we shouldn't do that if we're a server) or the
                // client is confused.
                return;
            }

            //Log.report( "GotKeepAliveResp", "msg", msg );
            //Console.WriteLine(" GotKeepAliveResp: msg " + msg);

            tickle();
        }

        private void tickle()
        {
            //	lastTickle = Timer.GetNanos();
      //      lastTickle = HPTimer.Now();
            lastTickle = HPTimer.Now();
        }

        private long lastTickle;

        private void checkTickle()
        {
           

            long ms = HPTimer.MillisSince(lastTickle);
            //		Log.report( "KeepAliveIdle", "ms", ms, "server", server );
            if (ms >= count * delay * 1000)
            {
                try
                {
                    //				Log.report( "KeepAliveReset", "server", server );
                    session.SessionNotify("KeepAlive resetting dead connection");
                    transport.TransportControl(TransportConsts.RESET, 0);
                }
                catch (Exception e)
                {
                    reportError(e);
                }
            }
        }

        private void sendKeepAliveReq()
        {
            Message msg = new Message(mt_request, vf);
            msg.Add(mf_delay, delay);
            msg.Add(mf_count, count);
            try
            {
                //		Log.report( "SendKeepAliveReq", "msg", msg );
                transport.TransportMessage(null, msg);
            }
            catch (Exception e)
            {
                reportError(e);
            }
        }

        private void sendKeepAliveResp(Message msg)
        {
            Message rmsg = msg.Reply();
            try
            {
                //Log.report( "SendKeepAliveResp", "rmsg", rmsg );
                transport.TransportMessage(null, rmsg);
            }
            catch (Exception e)
            {
                reportError(e);
            }
        }

        public int Wakeup(AlarmManager manager, Object state, long due)
        {
            //		Log.report( "KeepAliveWakeup", "server", server );

            if (!up)
                return 0;

            if (!server)
            {
                TodoManager.AddTodo(new MyTodo(sendKeepAliveReq, reportError));
            }

            checkTickle();

            return delay * 1000;
        }

        private void reportError(Exception e)
        {
            try
            {
                session.SessionNotify(e);
            }
            catch (Exception e1)
            {
                Console.WriteLine(e1);
            }
        }

        public override string ToString()
        {
            return "KeepAlive/" + transport;
        }
    }

    public class MyTodo : Todo
    {
        public delegate void doit();

        public delegate void report(Exception e);

        public MyTodo(doit d, report r)
        {
            this.d = d;
            this.r = r;
        }

        private readonly doit d;

        private readonly report r;

        public void Doit(TodoManager mgr)
        {
            d();
        }

        public void Exception(TodoManager mgr, Exception e)
        {
            r(e);
        }
    }
}
