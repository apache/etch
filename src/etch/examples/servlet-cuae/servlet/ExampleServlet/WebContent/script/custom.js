function start()
{

	
	// we bind keydown to the sendtext input element.  If it occurs, we send an XMLHTTPRequest to servlet to indicate message should be sent
	$("#sendtext").keydown(function(event) {

			 	if(event.which == 13)
			 	{		
					// fire off ajax POST with the value of the sendtext input element
					$.ajax({ 
						type: "POST",
        				url: "http://" + serverHost + ":8080/ExampleServlet/ExampleServlet",   
        				data: "message=" + $(this).attr("value"),
        				dataType: "xml",   
        				success: function(xml)
        				{
        					if($(xml).find("response").text() == "Failure")
        					{
        						$("#status").text("Failure in sending message from servlet.");
        					}
        				},
        				failure: function()
				        {
				        	// some sort of communication error...
				            $("#status").text("Unable to communicate with servlet..");
				        }
					});
			 	}
			});
	

	pollServer();                
 }
 
 function pollServer()
 {
	 $.ajax({
	        type: "GET",
	        url: "http://" + serverHost + ":8080/ExampleServlet/ExampleServlet", 
	        data: "poll=1",
	        dataType: "xml",   
	        success: function(xml)
	        {
	             // for every <message></message> element, stick the selection value under the #responses div
	             
	             // loop over every mesage
	             $(xml).find("message").each(function()
	             {
	             	var selection = $(this).text();
					
	             	$("<div></div>")
	             		.addClass("message")
	             		.html(selection)
	             		.appendTo("#responses");
	 
	             });
	             
	             setTimeout(pollServer, 5000);
	        },
	        
	        failure: function()
	        {
	        	// some sort of communication error...
	            $("#status").text("Unable to communicate with server...");
	        }
	 });
 }