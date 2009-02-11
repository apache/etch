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
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    [TestFixture]
    public class TestStubBase
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestStubBase" );
        }

        public enum What 
        { 
            /** wanted message delivered */ UNWANTED 
        };

        [Test]
        public void Nothing()
        { }

        //[Test] 
        [ExpectedException("System.InvalidOperationException")]
        public void Unwanted3()
        {
            Object obj = new Object();

            DeliveryService src = new MyMessageSource();
            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.SessionMessage(sender, msg);
           Assert.Fail("delivery of unwanted message should fail");
        }

        //[Test]
        [ExpectedException("System.InvalidOperationException")]
        public void Unwanted4()
        {
            Object obj = new Object();

            DeliveryService src = new MyMessageSource();
            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.SessionMessage(sender, msg);
            Assert.Fail("delivery of unwanted message should fail");
        }

        public class MyMessageSource : DeliveryService
        {

           

            #region TransportMessage Members

            public void TransportMessage(Who recipient, Message msg)
            {
                Assert.Fail("not needed");
            }

            #endregion

            #region Transport<SessionMessage> Members

            public object TransportQuery(object query)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void TransportControl(object control, object value)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void TransportNotify(object eventObj)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SetSession(SessionMessage session)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region DeliveryService Members

            public Mailbox BeginCall(Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public object EndCall(Mailbox mb, XType responseType)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region SessionMessage Members

            public bool SessionMessage(Who sender, Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region Session Members

            public object SessionQuery(object query)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SessionControl(object control, object value)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SessionNotify(object eventObj)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region Transport<SessionMessage> Members


            public SessionMessage GetSession()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }
    }

    public class MyWho : Who
    {
    }
}