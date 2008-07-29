using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Msg;

namespace Etch.Transport
{
    public abstract class AbstractDeliveryService : DeliveryService
    {

       public AbstractDeliveryService(MailboxManager transport, string uri, Resources resources )
       {
           this.transport = transport;
           transport.SetSession(this);
       }


        private SessionMessage _stub;

        private MailboxManager transport;

        

        /// <summary>
        /// Removes all the stubs in the set of stubs
        /// </summary>
        public void RemoveStub()
        {
            _stub = null;
        }

        public Object SessionQuery(Object query)
        {
            if (_stub != null)
                return _stub.SessionQuery(query);

            throw new Exception("Unknown query " + query);
        }

        public void SessionControl(Object control, Object value)
        {
            if (_stub != null)
                _stub.SessionControl(control, value);

            throw new Exception("Unknown control " + control);
        }

        public void SessionNotify(Object eventObj)
        {
            if (eventObj is string)
            {
                string stringObj = (string) eventObj;
                if (stringObj == SessionConsts.UP)
                {
                    status.Set(SessionConsts.UP);
                }
                else if (stringObj == SessionConsts.DOWN)
                {
                    status.Set(SessionConsts.DOWN);
                }
                
            }
           
                _stub.SessionNotify(eventObj);
                
  
            

        }

        private Monitor<string> status = new Monitor<string>("session status");

       
        public Object TransportQuery(Object query)
        {
            if(query.GetType() == typeof(TransportConsts.WaitUp))
            {
                waitUp(((TransportConsts.WaitUp) query)._maxDelay);
                return null;
            }
            else if (query.GetType() == typeof(TransportConsts.WaitDown))
            {
                waitDown(((TransportConsts.WaitDown)query)._maxDelay);
                return null;
            }
            else
            {
                return transport.TransportQuery(query);
            }

        }

        private void waitUp(int maxDelay)
        {
            status.WaitUntilEq(SessionConsts.UP,maxDelay);
        }

        private void waitDown(int maxDelay)
        {
            status.WaitUntilEq(SessionConsts.DOWN, maxDelay);
        }

        public void TransportControl(Object control, Object value)
        {
            if (control is string)
            {
                string stringObj = (string) control;

                if (stringObj == TransportConsts.START_AND_WAIT_UP)
                {
                    transport.TransportControl(TransportConsts.START, null);
                    waitUp(((int?)value).Value);
                }
                else if (stringObj == TransportConsts.STOP_AND_WAIT_DOWN)
                {
                    transport.TransportControl(TransportConsts.STOP,null);
                    waitDown(((int?)value).Value);
                }
                else
                {
                    transport.TransportControl(control, value);
                }       
            }
            else
            {
                transport.TransportControl(control, value); 
            }
           
        }

        public void TransportNotify(Object eventObj)
        {
            transport.TransportNotify(eventObj);
        }

       

        public Mailbox BeginCall(Message msg)
        {
            try
            {
                return transport.TransportCall(null, msg);
            }
            catch (Exception e)
            {
                throw new Exception("unexpected exception sending message", e);
            }
        }

        public Object EndCall(Mailbox mb, XType responseType)
        {
            try
            {
                Element mbe = mb.Read(responseType.Timeout);
                if (mbe == null)
                    throw new TimeoutException("timeout waiting for " + responseType);
                Message rmsg = mbe.msg;
                rmsg.CheckType(responseType);
                Object r = rmsg.Get(responseType.ResponseField);
                if (r is Exception)
                {
                    Exception e = (Exception)r;
                    throw e;
                }
                return r;
            }
            finally
            {
                mb.CloseDelivery();
            }
        }

        public override string ToString()
        {
            return transport.ToString();
        }

    

     
        #region SessionMessage Members

        public bool SessionMessage(Who sender, Message msg)
        {
           return  _stub.SessionMessage(sender, msg);
        }

        /// <summary>
        /// Adds a stub to a list of stub
        /// </summary>
        /// <param name="stb"></param>
        public void SetSession(SessionMessage stb)
        {
            if (_stub != null)
                throw new Exception("Unsupported -- only one stub for now");
            _stub = stb;
        }

        #endregion

        #region TransportMessage Members

        public void TransportMessage(Who recipient, Message msg)
        {
            transport.TransportMessage(recipient,msg);
        }

        #endregion

        #region Transport<SessionMessage> Members


        public SessionMessage GetSession()
        {
            return _stub;
        }

        #endregion
    }
}
