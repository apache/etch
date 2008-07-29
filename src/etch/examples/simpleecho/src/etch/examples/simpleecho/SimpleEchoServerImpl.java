package etch.examples.simpleecho;

public class SimpleEchoServerImpl implements SimpleEchoServer
{
    private final SimpleEchoClient client;

    public SimpleEchoServerImpl (SimpleEchoClient client)
    {
        this.client = client;
    }

    public void write (String msg)
    {
        client.onNewMessage("You said: " + msg);
    }
}
