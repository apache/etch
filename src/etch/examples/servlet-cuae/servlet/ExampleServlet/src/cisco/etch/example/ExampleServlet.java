package cisco.etch.example;



import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import metreos.core.io.TcpConnection;

import cisco.etch.examples.RemoteTwoWayServer;
import cisco.etch.examples.StubTwoWayClient;
import cisco.etch.examples.StubTwoWayServer;
import cisco.etch.examples.TwoWayClient;
import cisco.etch.examples.TwoWayServer;
import cisco.etch.examples.ValueFactoryTwoWay;
import etch.bindings.java.transport.DefaultDeliveryService2;
import etch.bindings.java.transport.MailboxMessageSource;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.*;


/**
 * Servlet implementation class for Servlet: MessagingServer
 *
 */
 public class ExampleServlet extends javax.servlet.http.HttpServlet implements javax.servlet.Servlet {

	protected ArrayList<String> list = new ArrayList<String>();
	 
	protected  StubTwoWayClient client = null;

	protected TwoWayServer server = null;
	
	public ExampleServlet() {
		super();
	}   	
	
	
	@Override
	public void init()
	{
		server = createConnection();
		
		if(server == null)
		{
			// Unable to connect to server!  
			System.out.println("Unable to connect to the CUAE provider!");
		}
	}
	
	@Override
	public void destroy()
	{
		if(server != null)
		{
			// um. How do I destroy this connection?  Not sure.
			server = null;
		}
	}
	
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		// first do sanity checks
		if(CheckUnrecoverables(response))
		{
			return;
		}
		
		// determine if this is a new session.
		
		String pollParameter = request.getParameter("poll");
		
		if(pollParameter == null)
		{
			// new session.  Feed out base HTML
			PrintBaseHTML(request.getServerName(), response);
		}
		else
		{
			// must be a poll-check from already-connected browser.  Check for outstanding changes, update 
			
			System.out.println("A poll-check was made by the browser.");
			
			ExpirePage(response);
			response.setContentType("text/xml");
			
			PrintWriter writer = response.getWriter();		
			writer.write("<response>");
			
			if(areNewMessages())
			{
				String[] messages = popMessages();
				
				if(messages != null)
				{	
					for(int i = 0; i < messages.length; i++)
					{
						writer.write("<message>" + messages[i] + "</message>");
					}		
				}
			}
			writer.write("</response>");
		}
	}  	
	
	protected String[] popMessages()
	{
		String[] messages = list.toArray(new String[0]);
		list.clear();
		return messages;
	}
	
	protected void PrintBaseHTML(String host, HttpServletResponse response)
	{
		try
		{
			PrintWriter writer = response.getWriter();
			
			// expire page so that IE does not cache XMLHTTPRequests
			ExpirePage(response);
			
			writer.write("<html>");
			writer.write("<head>");
			writer.write("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\" />");
			writer.write("<title>Panel</title>");
			writer.write("<link rel=\"stylesheet\" type=\"text/css\" href=\"/ExampleServlet/style/basic.css\" />");
			writer.write("<script type=\"text/javascript\">var serverHost = '" + host + "';</script>");
			writer.write("<script type=\"text/javascript\" src=\"/ExampleServlet/script/jquery.js\"></script>");
			writer.write("<script type=\"text/javascript\" src=\"/ExampleServlet/script/custom.js\"></script>");
			writer.write("<script type=\"text/javascript\">$(document).ready(function() {  start();  });</script>");
			
			writer.write("</head>");
			writer.write("<body>");
			writer.write("<div id=\"status\"></div>");
			writer.write("<div class=\"label\">Send text to a phone:</div>");
			writer.write("<input id=\"sendtext\" type=\"text\" value=\"\"></input>");
			writer.write("<p>Responses from phone:</p><hr />");
			writer.write("<div id=\"responses\"></div>");
			writer.write("</body>");
			writer.write("</html>");
		}
		catch(Exception e)
		{
			// Unable to acquire writer.  Fatal failure
			System.out.println("Unable to acquire the response stream.  Aborting...");
			return;
		}
	}
	
	protected void ExpirePage(HttpServletResponse response)
	{
		Calendar tenYearsAgo = Calendar.getInstance();
		tenYearsAgo.set(Calendar.YEAR, tenYearsAgo.get(Calendar.YEAR) - 10);
		long tenYrsLater = tenYearsAgo.getTime().getTime();
		response.setDateHeader("Expires", tenYrsLater);
	}
	
	protected boolean areNewMessages()
	{
		return list.size() > 0;
	}

	// Posts come whenever the user hits 'enter' in the message form
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
		boolean success = false;
		
		String messageValue = request.getParameter("message");
		
		System.out.println("debug out: " + messageValue);
		
		if(server != null)
		{
			try
			{
				server.sendCUAEMessage(messageValue);
				success = true;
			}
			catch(Exception e)
			{
				System.out.println("Exception in sendCUAEMessage: " + e.getLocalizedMessage());	
			}
		}

		PrintWriter writer = response.getWriter();
		
		ExpirePage(response);
		response.setContentType("text/xml");
		
		if(success)
		{
			writer.write("<response>Success</response>");
		}
		else
		{
			writer.write("<response>Failure</response>");
		}
	}   
	
	protected boolean CheckUnrecoverables(HttpServletResponse response)
	{
		boolean unrecoverable = false;
		
		if(server == null)
		{
			try
			{
				PrintWriter writer = response.getWriter();
				
				// expire page so that IE does not cache XMLHTTPRequests
				ExpirePage(response);
				
				writer.write("<html>");
				writer.write("<head>");
				writer.write("<title>Panel</title>");
				writer.write("<html>");
				writer.write("<head>");
				writer.write("<link rel=\"stylesheet\" type=\"text/css\" href=\"/style/basic.css\">");
				writer.write("</head>");
				writer.write("<body>");
				writer.write("<p>No connection to the CUAE server was made. Restart the Servlet.</p>");
				writer.write("</body>");
				writer.write("</html>");
			}
			catch(Exception e) { }
			
			unrecoverable =  true;
		}
		
		return unrecoverable;
	}
	
	protected TwoWayServer createConnection()
	{
		TwoWayServer server;
		
		ValueFactoryTwoWay vf = new ValueFactoryTwoWay();
        
        DefaultDeliveryService2<StubTwoWayClient> dds 
            = new DefaultDeliveryService2<StubTwoWayClient>()
        {
            @Override
            protected StubTwoWayClient newStub( MailboxMessageSource src )
            {
                return client;
            }

            @Override
            public String toString()
            {
                return "ClientMain";
            }
        };

        TcpConnection c = new TcpConnection( null, "10.89.31.162", 4051, 0 );
        
        DefaultDeliveryService2 svc = 
            DefaultDeliveryService2.setUp(dds, c, vf );
     
        server = new RemoteTwoWayServer( svc );
 
        client = new StubTwoWayClient( new TwoWayClient()
        {
        	public void sendServletMessage(
        			Selection selection)
        	{
        		// Notify active servlets... somehow.
        		System.out.println("Notify active sessions!");
        		
        		list.add(selection.toString());
        	}

            public void bogus() {}
            
        }, null, null );
        
        try
        {
        	c.start();
        	c.waitUp( 4000 );
        }
        catch(Exception e)
        {
        	System.out.println("Unable to connect to server. Exception: " + e.getLocalizedMessage());
        	return null;
        }
        
        return server;
	}
}
