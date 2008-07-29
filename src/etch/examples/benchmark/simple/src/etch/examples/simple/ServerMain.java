package etch.examples.simple;

import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.DefaultDeliveryService;
import etch.bindings.java.transport.DefaultListenerHandler;
import etch.bindings.java.transport.DefaultMessageHandler;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.MessageHandler;

public class ServerMain 
{
    public static void main( String[] args ) throws Exception
    {
        ListenerHandler lh = new DefaultListenerHandler()
        {
            @Override
            protected MessageHandler<MailboxMessageSource> newMessageHandler()
            {
                return new DefaultMessageHandler<StubSimpleServer>()
                {
                    @Override
                    protected StubSimpleServer newStub( MailboxMessageSource src )
                    {
                        DefaultDeliveryService svc = new DefaultDeliveryService(src, null);
                        RemoteSimpleClient rfc = new RemoteSimpleClient(svc);
                        return new StubSimpleServer(new SimpleServerImpl(rfc), null, null);
                    }

                    @Override
                    public String toString()
                    {
                        return "MessageHandler";
                    }
                };
            }

            @Override
            protected ValueFactory newValueFactory()
            {
                return new ValueFactorySimple();
            }

            @Override
            public String toString()
            {
                return "ListenerHandler";
            }
        };
        
        Listener l = new Listener( lh, 5, "", 4001, 0 );
        l.start();
    }
}