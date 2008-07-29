using System;
using System.Diagnostics;
using System.Data;
using System.Timers;

using MySql.Data.MySqlClient;

using Metreos.Core;
using Metreos.ProviderFramework;
using Metreos.Core.ConfigData;
using Metreos.Interfaces;
using Metreos.Utilities;
using Metreos.Messaging;
using Metreos.PackageGeneratorCore.Attributes;
using Metreos.PackageGeneratorCore.PackageXml;

using Etch.Transport;
using Etch.Msg;

using cisco.etch.examples;

namespace Cisco.Providers.TwoWay
{
    [ProviderDecl("TwoWay Provider")]
    [PackageDecl("Cisco.Providers.TwoWay", "Description of your provider")]
    public class TwoWayProvider : ProviderBase
    {
        protected const string ProviderNamespace = "Cisco.Providers.TwoWay";
        protected Listener listener;
        protected TwoWayServerImpl etchServer;

        public TwoWayProvider(IConfigUtility configUtility)
            : base(typeof(TwoWayProvider), "TwoWay Provider", configUtility)
        {
            
        }

        #region ProviderBase Implementation
        protected override bool Initialize(out ConfigEntry[] configItems, out Extension[] extensions)
        {
            configItems = null;  // these two lines are here only so that the provider compiles.
            extensions = null;   // you will most likely remove these when implementing your own

            ///     TODO:
            ///     1.  Defining which methods handle which actions that originate from apps.
            ///     This is done by creating a relationship between an action name and a 
            ///     HandleMessageDelegate class... and then adding that to the  messageCallbacks
            ///     collection
            ///     Example:
            this.messageCallbacks.Add(ProviderNamespace + "." + "SelectionMade",
                    new HandleMessageDelegate(this.SelectionMadeHandler));


            ///     2.  Define configuration items
            ///     A provider declares what is configurable in the management console in the 
            ///     Initialize method.  Do so by defining how many configItems you have as
            ///     an array, and then go about defining each configItem!
            ///     
            ///      
            ///		configItems = new ConfigEntry[1];
            ///		configItems[0] = new ConfigEntry("DB Name of Item", "Display Name of Item", Default_Value, 
            ///			"Description of Item", IConfig.StandardFormat.YOUR_TYPE, is_required);


            ///     3.  Define extension items
            ///     A provider declares what are extensions (actions that the *administrator*
            ///     can preform in the management console) in the Initialize method.  Do so by
            ///     defining how many extensions you have as an array, and then go about defining
            ///     each item!
            ///     
            ///     One important note about extensions.  Extensions use the the
            ///     messageCallBacks collection defined above to determine which method is initiated based
            ///     on the administrator invoking the extension.  The key is to simply ensure your extension
            ///     uses the same name as a messageCallback.  Also, there is nothing to prevent you from
            ///     using the same messageCallback for an action and an extension.
            ///     
      
            return true;
        }


        protected override void RefreshConfiguration()
        {
            ///    TODO:
            ///    1.  Retrieve configuration values, and apply to provider.
            ///    Use this.GetConfigValue("DB Name of Item") to get an object from the database
        }

        /// <summary>
        ///     If this method fires, then an event we fired was handled by no application.
        /// </summary>
        protected override void HandleNoHandler(ActionMessage noHandlerAction, EventMessage originalEvent)
        {

        }

        /// <summary>
        ///     * Runs in "your" thread – not the application manager – 
        ///           so that it doesn't slow down the startup of other providers.
        ///     * You must call RegisterNamespace() here or applications can not use the actions of this provider.
        ///     * Perform possibly time-consuming actions, e.g., initializing stack.
        ///     * Note: Your provider should not send any events 
        ///           (and will not receive any actions) until this method completes.
        /// </summary>
        protected override void OnStartup()
        {
            RegisterNamespace();
            
            EtchServer sm = new EtchServer(this);
            Listener listener = new Listener(sm.lh, 5, "10.89.31.162", 4051, 0);
            listener.Start();
            listener.WaitUp(4000);

            log.Write(TraceLevel.Info, "Listener is online!");

            base.OnStartup();
        }

        /// <summary>   
        ///     Guaranteed to be called on a graceful shutdown of the Application Server
        /// </summary>
        protected override void OnShutdown()
        {
            // This is throwing un-handled exception
            //listener.ShutdownOutput();
            listener.Stop();
            listener.Close(false);
        }

        #endregion

        #region Provider Events

        // This method is invoked by the Etch server (TwoWayServerImpl) whenever the client (servlet) invokes the sendCUAEMessage method.
        // The result of this method is that a script instance is started on the Application Server

        [Event(ProviderNamespace + "." + "PushMessage", true, null, "PushMessage", "Pushes message to a phone.")]
        [EventParam("message", typeof(System.String), true, "The message to push.")]
        public void PushMessage(string message)
        {
            string scriptId = System.Guid.NewGuid().ToString();
            EventMessage msg = CreateEventMessage(
                ProviderNamespace + "." + "PushMessage",
                EventMessage.EventType.Triggering,
                scriptId);

            // Add the message content to EventMessage structure so that our script can pull it out and use it.
            msg.AddField("message", message);

            // This sends the message we just constructed to the Application Runtime Environment.  If a script is installed that matches the
            //'Cisco.Providers.TwoWay.PushMessage' event, then it will initiate a script instance.
            
            palWriter.PostMessage(msg);
        }

        #endregion

        #region Actions

        [Action(ProviderNamespace + "." + "SelectionMade", false, "SelectionMade", "Indicates to the servlet that a selection was made.", false)]
        [ActionParam("Selection", typeof(string), true, false, "Value1 used to do XYZ")]
         protected void SelectionMadeHandler(ActionBase action)
        {
            // Extract the info sent down from application out of the action message
            string value1 = null;
            action.InnerMessage.GetString("Selection", true, String.Empty, out value1);

            /// value1 better had be either Water, Coke, or Hotdog, because that is what we constructed.
           
            // call etch client method
            try
            {
                ConstsTwoWay.Selection selection = (ConstsTwoWay.Selection) Enum.Parse(typeof(ConstsTwoWay.Selection), value1);
                etchServer.client.sendServletMessage(selection);
            }
            catch(Exception e)
            {
                log.Write(TraceLevel.Error, "Unable to send IP Phone message to servlet!\nMessage: " + e);
            }
                  
            // tell application that we successfully processed the request
            action.SendResponse(true);
        }

        #endregion

        #region Etch Plumbing

        public class EtchServer
        {
            public MyDefaultListenerHandler lh;

            public EtchServer(TwoWayProvider server)
            {
                lh = new MyDefaultListenerHandler(this, server);
            }

            public class MyDefaultListenerHandler : DefaultListenerHandler
            {
                EtchServer sm;
                MyDeliveryService sds;
                TwoWayProvider server;

                public MyDefaultListenerHandler(EtchServer sm, TwoWayProvider server)
                {
                    this.sm = sm;
                    this.sds = new MyDeliveryService(this, server);
                    this.server = server;
                }

                protected override Etch.Transport.MessageHandler<MailboxMessageSource> NewMessageHandler()
                {
                    return new MyDeliveryService(this, server);
                }

                protected override ValueFactory NewValueFactory()
                {
                    return new ValueFactoryTwoWay();
                }

                public override string ToString()
                {
                    return "TwoWayServer";
                }

                public class MyDeliveryService : DefaultDeliveryService<StubTwoWayServer>
                {
                    MyDefaultListenerHandler sdlh;
                    TwoWayProvider server;

                    public MyDeliveryService(MyDefaultListenerHandler sdlh, TwoWayProvider server)
                    {
                        this.sdlh = sdlh;
                        this.server = server;
                    }

                    protected override StubTwoWayServer NewStub(MailboxMessageSource src)
                    {
                        this.SetMailboxMessageSource(src);
                        RemoteTwoWayClient rsc = new RemoteTwoWayClient(this);
                        TwoWayServerImpl etchServer = new TwoWayServerImpl(server, rsc);

                        // let the provider have access to this server instance.
                        // It is a little odd to do this--this assumes there will be only 
                        // one client connected (i.e., one servlet/tomcat instance talking to this provider)
                        server.etchServer = etchServer;

                        return new StubTwoWayServer(etchServer, null, null);
                    }

                    public override string ToString()
                    {
                        return "MyDeliveryService";
                    }
                }
            }
        }
        #endregion

        /// <summary>
        ///     This was added just to give access to the logger to the TwoWayServerImpl class
        /// </summary>
        public void LogWrite(TraceLevel level, string message)
        {
            log.Write(level, message);
        }
        
    }
}