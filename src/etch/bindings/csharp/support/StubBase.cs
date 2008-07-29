///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 25, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved. 


using System;
using Etch.Msg;
using Etch.Transport;

namespace Etch.Support
{

    /// <summary>
    /// Base class of stub implementations
    /// </summary>
    public class StubBase : SessionMessage
    {
        /// <summary>Constructs the StubBase.</summary>
        /// <param name="obj">obj</param>
        /// <param name="queued">thread pool used to run AsyncReceiverMode.QUEUED methods.</param>
        /// <param name="free">thread pool used to run AsyncReceiverMode.FREE methods.</param>
        public StubBase(DeliveryService src, object obj, Pool queued, Pool free)
        {
            _src = src;
            _obj = obj;
            _queued = queued;
            _free = free;
            src.SetSession(this);
        }

        /// <summary>
        /// The object used to implement stub
        /// </summary>
        private readonly object _obj;

        /// <summary>
        /// The thread pool used to run AsyncReceiverMode.QUEUED methods.
        /// </summary>
        public readonly Pool _queued;

        /// <summary>
        /// The thread pool used to run AsyncReceiverMode.FREE methods.
        /// </summary>
        public readonly Pool _free;

        protected readonly DeliveryService _src;

        

        public Object SessionQuery(Object query)
        {
            if (_obj is ObjSession)
                return ((ObjSession)_obj)._SessionQuery(query);
            throw new Exception("unknown query " + query);
        }

        public void SessionControl(Object control, Object value)
        {
            if (_obj is ObjSession)
                ((ObjSession)_obj)._SessionControl(control, value);
            else
                throw new Exception("unknown control " + control);
        }

        public void SessionNotify(Object eventObj)
        {
            if (_obj is ObjSession)
                ((ObjSession)_obj)._SessionNotify(eventObj);
        }

        /// <summary>
        /// Delegate that helps the stub dispatch the received message
        /// </summary>
        /// <param name="_src"></param>
        /// <param name="_obj"></param>
        /// <param name="sender"></param>
        /// <param name="msg"></param> 
        public delegate void StubHelperRun(DeliveryService _src, Object _obj, Who sender, Message msg);


        #region SessionMessage Members

        public bool SessionMessage(Who sender, Message msg)
        {
            XType type = msg.GetXType;

            //StubHelper<StubBase<Object>> helper = ( StubHelper<StubBase<Object>> ) type.GetStubHelper();
            //if ( helper == null )
            //    return false;
            StubHelperRun helper = (StubHelperRun)type.GetStubHelper();
            /*      StubHelper stubHelper = (StubHelper)type.GetStubHelper();
                  StubHelperRun helper = (StubHelperRun) stubHelper.GetDelegate(); */
            if (helper == null)
                return false;
            if (type.GetAsyncMode() == AsyncMode.QUEUED)
                _queued.Run(delegate() { helper(_src, _obj, sender, msg); }, delegate(Exception e)
                {
                    try { SessionNotify(e); }
                    catch (Exception e1)
                    {
                        Console.WriteLine(e1.StackTrace);
                    }
                });
            else if (type.GetAsyncMode() == AsyncMode.FREE)
                _free.Run(delegate()
                              {
                                  helper(_src, _obj, sender, msg);
                              },
                              delegate(Exception e)
                              {
                                  try
                                  {
                                      SessionNotify(e);
                                  }
                                  catch (Exception e1)
                                  {
                                      Console.WriteLine(e1.StackTrace);
                                  }
                              });
            else
                helper(_src, _obj, sender, msg);

            return true;
        }

        #endregion
    }
}