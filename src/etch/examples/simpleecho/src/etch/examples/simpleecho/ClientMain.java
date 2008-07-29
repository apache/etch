package etch.examples.simpleecho;

import java.lang.Thread;

import etch.bindings.java.transport.DefaultDeliveryService2;
import etch.bindings.java.transport.MailboxMessageSource;
import metreos.core.io.TcpConnection;

/**
 * Description of testRemoteFooServer.
 */
public class ClientMain 
{
    static StubSimpleEchoClient sfc;

    public static void main( String[] args ) throws Exception
    {
        ValueFactorySimpleEcho vf = new ValueFactorySimpleEcho();
        
        DefaultDeliveryService2<StubSimpleEchoClient> dds 
            = new DefaultDeliveryService2<StubSimpleEchoClient>()
        {
            @Override
            protected StubSimpleEchoClient newStub( MailboxMessageSource src )
            {
                return sfc;
            }

            @Override
            public String toString()
            {
                return "ClientMain";
            }
        };

        TcpConnection c = new TcpConnection( null, "", 4001, 0 );
        
        DefaultDeliveryService2 svc = 
            DefaultDeliveryService2.setUp(dds, c, vf );
        
        final SimpleEchoServer rfs = new RemoteSimpleEchoServer( svc );
        
        sfc = new StubSimpleEchoClient( new SimpleEchoClient()
        {
            public void onNewMessage(String message)
            {
                System.out.println(message); 
            }

            public void bogus() {}
        }, null, null );
        
        c.start();
        c.waitUp( 4000 );


        Thread t = new Thread() 
        {
            public void run() 
            {
                try 
                {
                    while (true)
                    {
                        rfs.write("Hi");
                        sleep(1000); 
                    }
                }
                catch(InterruptedException ex) {}
            }
        };

        System.out.println("Press enter to quit.");
        t.start();
        System.in.read();
        t.interrupt();
        t.join();
        System.out.println("Exiting.");

        svc.shutdownOutput();
        c.stop();
        c.waitDown(4000);
    }
}

