<?xml version="1.0" encoding="ISO-8859-1" ?>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@page import="java.io.IOException" %>
<%@page import="java.io.PrintWriter" %>
<%@page import="etch.bindings.java.transport.*" %>
<%@page import="metreos.core.io.TcpConnection" %>
<%@page import="etch.examples.ccmuser.*" %>
<%@page import="etch.examples.ccmuser.CcmUser.*" %>
<%@page import="etch.examples.ccmuser.CcmUserServer" %>
<%@page import="etch.examples.ccmuser.ValueFactoryCcmUser" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1" />
<title>Unified Communications User Configuration</title>
<link rel="stylesheet" type="text/css" href="./style/default.css" />
<script src="./script/jquery.js" type="text/javascript"></script>
<script type="text/javascript">
 
 $(document).ready(function() {
	 $(".clicker").click(function(event) { 			

		var clickerElement = $(this);

		clickerElement.parent().find("div.children").slideToggle("slow");

		if(clickerElement.attr("src") == "./image/plus.gif")
		{
		 	clickerElement.attr("src", "./image/x.gif");	
		}
		else
		{
			clickerElement.attr("src", "./image/plus.gif");	
		}
		
		return false;
	});
	
	$("div.speed span.description").click(function() { editDescription($(this)) } );

	$("div.speed span.marker").click(function() { editNumber($(this)) } );
	
	$("img.speed").click(function() {
		
		// retreive parent id attr and retrieve 2nd-sibling over text
		
		var imgElement = $(this);
		var device = imgElement.parent().parent().attr("id");
		var speeddialNumber = imgElement.next().next().text(); 
		
		$.ajax({
   					type: "GET",
   					url: "./Proxy",
   					data: "device=" + device + "&to=" + speeddialNumber + "&cuae=<%= request.getParameter("up_c") %>",
   					dataType: "xml",
   					success: function(xml)
   					{
   						// response types
   						// <response>Success</response>");
   						// <response>Error</response>");

   						if($(xml).find("response").text() == "Success")
                        {
                            imgElement.fadeTo("slow", 0.3, function() { imgElement.fadeTo("slow", 1.0) } );
                        }

                        if($(xml).find("response").text() == "Error")
                        {
                            $("#statusMessage").text("Unable to initiate call.").css({"font-style": "italic", color: "red"});
                        }
   					},

   					error: function(request, msg, exp)
                  	{
                         $("#statusMessage").text("Failure to initiate Speeddial").css({"font-style": "italic", color: "red"});
                   	}
             	});
	});

 });
 
function editDescription(thisNode)
{
	var currentValue = thisNode.text();

		 var editBox = $("<input></input>").attr( { type: "text", value: currentValue })
		 	.bind("keydown",  function(event) {

			 	if(event.which == 13)
			 	{
					var replacement = $("<span></span").text($(this).attr("value")).addClass("description").insertBefore($(this))
					.bind("click", function() { editDescription($(this)) });

					$(this).remove();

					// fire off ajax request
					updateSpeeddialLabel(replacement.text(), replacement.parent().attr("id"), replacement);		
			 	}
			 	
			})
		 .bind("mouseout", function(event) {
				$("<span></span").text(currentValue).addClass("description").insertBefore($(this))
						.bind("click", function() { editDescription($(this)) });
						
					$(this).remove();
			 });


		 editBox.insertBefore(thisNode);

		 thisNode.remove();
}

function editNumber(thisNode)
{
	var currentValue = thisNode.text();

		 var editBox = $("<input></input>").attr( { type: "text", value: currentValue })
		 	.bind("keydown",  function(event) {

			 	if(event.which == 13)
			 	{
					var replacement = $("<span></span").text($(this).attr("value")).addClass("marker").insertBefore($(this))
					.bind("click", function() { editNumber($(this)) });
					
					$(this).remove();

					// fire off ajax request
					updateSpeeddialNumber(replacement.text(), replacement.parent().attr("id"), replacement);				
			 	}
			 	
			})
		 	.bind("mouseout", function() {
				$("<span></span").text(currentValue).addClass("marker").insertBefore($(this))
						.bind("click", function() { editNumber($(this)) });
					
					$(this).remove();
			 });

		 editBox.insertBefore(thisNode);

		 thisNode.remove();
}

function updateSpeeddialLabel(newLabel, speeddialPkid, replacement)
 {
 	var updateLabel;
 	
 	$.ajax({
   					type: "GET",
   					url: "./UpdateSpeeddialLabel.jsp",
   					data: "newLabel=" + newLabel + "&pkid=" + speeddialPkid + "&up_e=<%= request.getParameter("up_e") %>&up_s=<%= request.getParameter("up_s") %>&up_a=<%= request.getParameter("up_a") %>&up_p=<%= request.getParameter("up_p") %>",
   					dataType: "xml",
   					beforeSend: function(request)
   					{
	   					// we want to add an 'updating' out to side of last span on this row
						updateLabel = $("<span></span>").text("Updating label...").addClass("updating")
						
						if($.browser.msie)
						{
							updateLabel.insertAfter(replacement.next().next());
						}
						else
						{
							updateLabel.insertAfter(replacement.next());
						}
   					},
   					success: function(xml)
   					{
   						// response types
   						// <response>Success</response>");
   						// <response>Error</response>");

   						if($(xml).find("response").text() == "Success")
                        {
                        	updateLabel.fadeTo("slow", 0.1, function() { updateLabel.remove(); } );
                        }

                        if($(xml).find("response").text() == "Error")
                        {
                        	updateLabel.fadeTo("slow", 0.0, function() { updateLabel.remove() } );
                            $("#statusMessage").text("Unable to update speeddial label.").css({"font-style": "italic", color: "red"});
                        }
   					},

   					error: function(request, msg, exp)
                  	{
						updateLabel.fadeTo("slow", 0.0, function() { updateLabel.remove() } );
                         $("#statusMessage").text("Failure to update speeddial label").css({"font-style": "italic", color: "red"});
                   	}
             	});
 }
 
 function updateSpeeddialNumber(newNumber, speeddialPkid, replacement)
 {
 	var updateLabel;
 	
 	$.ajax({
   					type: "GET",
   					url: "./UpdateSpeeddialNumber.jsp",
   					data: "newNumber=" + newNumber + "&pkid=" + speeddialPkid + "&up_e=<%= request.getParameter("up_e") %>&up_s=<%= request.getParameter("up_s") %>&up_a=<%= request.getParameter("up_a") %>&up_p=<%= request.getParameter("up_p") %>",
   					dataType: "xml",
   					beforeSend: function(request)
   					{
	   					// we want to add an 'updating' out to side of last span on this row
						updateLabel = $("<span></span>").text("Updating number...").addClass("updating")
						
						if($.browser.msie)
						{
							updateLabel.insertAfter(replacement.next());
						}
						else
						{
							updateLabel.insertAfter(replacement);
						}
   					},
   					success: function(xml)
   					{
   						// response types
   						// <response>Success</response>");
   						// <response>Error</response>");

   						if($(xml).find("response").text() == "Success")
                        {
                        	updateLabel.fadeTo("slow", 0.1, function() { updateLabel.remove() } );
                        }

                        if($(xml).find("response").text() == "Error")
                        {
                        	updateLabel.fadeTo("slow", 0.1, function() { updateLabel.remove() } );
                            $("#statusMessage").text("Unable to update speeddial label.").css({"font-style": "italic", color: "red"});
                        }
   					},

   					error: function(request, msg, exp)
                  	{
						updateLabel.fadeTo("slow", 0.0, function() { updateLabel.remove() } );
                         $("#statusMessage").text("Failure to update speeddial label").css({"font-style": "italic", color: "red"});
                   	}
             	});
 }

 </script>
</head>
<body>
<div id="statusMessage"><i></i></div>
<%
	String configServerIP 	= request.getParameter("up_e");
	String serverIP 		= request.getParameter("up_s");
	String username 		= request.getParameter("up_a");
	String password 		= request.getParameter("up_p");
	String theUser   		= request.getParameter("up_u");
	
	if(
		configServerIP != null && configServerIP != "" && 
		serverIP != null && serverIP != "" && 
		username != null && username != "" && 
		password != null && password != "" && 
		theUser != null && theUser != "")
	{
		boolean test = false;
		
		if(!test)
		{
			CcmUserServer configServer = CreateServer(configServerIP);
			
			if(configServer != null)
			{
				configServer.login(serverIP, username, password);
				
				UserConfiguration userConfig = configServer.getUserConfig(theUser);
				
				configServer.logout();
				
				if(userConfig != null)
				{
					out.println("<div>");
					if(userConfig.devices != null)
					{
						for(int i = 0; i < userConfig.devices.length; i++)
						{
							Device device = userConfig.devices[i];
							out.println("<div class=\"phone\" id=\"" + device.name + "\"><img class=\"clicker\" src=\"./image/plus.gif\" /><span class=\"description\">" + device.description + "</span><span class=\"marker\">" + device.type + "</span>");
							
							if(device.speeddials != null)
							{
								for(int j = 0; j < device.speeddials.length; j++)
								{
									Speeddial speeddial = device.speeddials[j];
									
									out.println("<div class=\"speed children\" id=\"" + speeddial.pkid + "\"><img class=\"speed\" src=\"./image/phone.gif\" /><span class=\"description\">" + speeddial.label + "</span><span class=\"marker\">" + speeddial.number + "</span></div>");
								}
							}
							
							out.println("</div>");
						}
					}
					else
					{
						out.println("<p>No devices defined for this user.</p>");
					}
					out.println("</div>");
				}
				else
				{
					out.println("Unable to connect to User Configuration server");
				}
			}
			else
			{
				out.println("Unable to connect to User Configuration server");
			}	
			
			
		}
		else
		{
			UserConfiguration userConfig = new UserConfiguration();
			userConfig.devices = new Device[2];
			userConfig.devices[0] = new Device();
			userConfig.devices[0].description = "My Desk Phone";
			userConfig.devices[0].name = "SEP000011112222";
			userConfig.devices[0].type = "7960";
			userConfig.devices[1] = new Device();
			userConfig.devices[1].description = "My Other Phone";
			userConfig.devices[1].name = "SEP000022221111";
			userConfig.devices[1].type = "IP Communicator";
			userConfig.devices[1].speeddials = new Speeddial[2];
			userConfig.devices[1].speeddials[0] = new Speeddial();
			userConfig.devices[1].speeddials[0].label = "Wife";
			userConfig.devices[1].speeddials[0].number = "50001";
			userConfig.devices[1].speeddials[0].pkid = "124324";
			userConfig.devices[1].speeddials[1] = new Speeddial();
			userConfig.devices[1].speeddials[1].label = "Boss";
			userConfig.devices[1].speeddials[1].number = "95126892222222";
			userConfig.devices[1].speeddials[1].pkid = "932487983";
			out.println("<div>");
			if(userConfig.devices != null)
			{
				for(int i = 0; i < userConfig.devices.length; i++)
				{
					Device device = userConfig.devices[i];
					out.println("<div class=\"phone\" id=\"" + device.name + "\"><img class=\"clicker\" src=\"./image/plus.gif\" /><span class=\"description\">" + device.description + "</span><span class=\"marker\">" + device.type + "</span>");
					
					if(device.speeddials != null)
					{
						for(int j = 0; j < device.speeddials.length; j++)
						{
							Speeddial speeddial = device.speeddials[j];
							
							out.println("<div class=\"speed children\" id=\"" + speeddial.pkid + "\"><img class=\"speed\" src=\"./image/phone.gif\" /><span class=\"description\">" + speeddial.label + "</span><span class=\"marker\">" + speeddial.number + "</span></div>");
						}
					}
					
					out.println("</div>");
				}
			}
		}
	}
	else
	{
		out.println("The query string must have up_e(Etch Server IP), up_s (AXL server IP), up_a (AXL username), and up_p (AXL password), and up_u (username of interest) defined"); 
	}
%>
</body>
</html><%!

static StubCcmUserClient smc;

protected CcmUserServer CreateServer(String configServerIP)
{
	ValueFactoryCcmUser vf = new ValueFactoryCcmUser();
    
    DefaultDeliveryService2<StubCcmUserClient> dds 
        = new DefaultDeliveryService2<StubCcmUserClient>()
    {
        @Override
        protected StubCcmUserClient newStub( MailboxMessageSource src )
        {
            return smc;
        }

        @Override
        public String toString()
        {
            return "ClientMain";
        }
    };

    TcpConnection c = new TcpConnection( null, configServerIP, 4021, 0 );
    
    DefaultDeliveryService2 svc = 
        DefaultDeliveryService2.setUp(dds, c, vf );
 
    CcmUserServer rfs = new RemoteCcmUserServer( svc );
    
    smc = new StubCcmUserClient( new CcmUserClient()
    {
    	public void DoINeedThis()
    	{
    		
    	}

        public void onNewMessage(String msg) {}
        
    }, null, null );
    
    try
    {
    c.start();
    c.waitUp( 4000 );
    }
    catch(Exception e)
    {
    	System.out.println("Unable to connect to etch server. " + e.getLocalizedMessage());
    	return null;
    }

    	return rfs;
}
%>