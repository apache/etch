package etch.examples.simpleecho;

import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.util.Log;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.DefaultDeliveryService;
import etch.bindings.java.transport.DefaultListenerHandler;
import etch.bindings.java.transport.DefaultMessageHandler;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.MessageHandler;

public class ServerMain 
{
    /**
     * @param args
     * @throws Exception
     */
    public static void main( String[] args ) throws Exception
    {
        Log.addSink( null );
        
        ListenerHandler lh = new DefaultListenerHandler()
        {
            @Override
            protected MessageHandler<MailboxMessageSource> newMessageHandler()
            {
                return new DefaultMessageHandler<StubSimpleEchoServer>()
                {
                    @Override
                    protected StubSimpleEchoServer newStub( MailboxMessageSource src )
                    {
                        DefaultDeliveryService svc = new DefaultDeliveryService( src, null );
                        RemoteSimpleEchoClient rfc = new RemoteSimpleEchoClient( svc );
                        return new StubSimpleEchoServer( new SimpleEchoServerImpl( rfc ), null, null );
                    }

                    @Override
                    public String toString()
                    {
                        return "SimpleEchoServerMessageHandler";
                    }
                };
            }

            @Override
            protected ValueFactory newValueFactory()
            {
                return new ValueFactorySimpleEcho();
            }

            @Override
            public String toString()
            {
                return "SimpleEchoServerListener";
            }
        };
        
        Listener l = new Listener( lh, 5, null, 4001, 0 );
        l.start();
    }
}

