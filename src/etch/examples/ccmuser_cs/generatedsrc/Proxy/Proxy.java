package Proxy;

import javax.servlet.Servlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLDecoder;
import java.io.IOException;
import java.io.InputStream;
import java.util.*;

public class Proxy extends HttpServlet implements Servlet {

	public Proxy () 
	{
		super();
	}
	
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		try
		{
			response.setContentType("text/xml");
			
			Calendar tenYearsAgo = Calendar.getInstance();
			tenYearsAgo.set(Calendar.YEAR, tenYearsAgo.get(Calendar.YEAR) - 10);
			long tenYrsLater = tenYearsAgo.getTime().getTime();
			response.setDateHeader("Expires", tenYrsLater);
			
			String uri = "http://" + request.getParameter("cuae") + ":8000/UserConfig/Speeddial?device=" + request.getParameter("device") + "&to=" + request.getParameter("to");
			URL url = new URL(URLDecoder.decode(uri, "utf-8"));
			HttpURLConnection connection = (HttpURLConnection) url.openConnection();
			response.setStatus(connection.getResponseCode());
			
			InputStream is = connection.getInputStream();
	        
	        while(true) 
	        {
	            byte[] bytes = new byte[128];
	            int read = is.read(bytes);
	            if(read <= 0) break; 
	            response.getOutputStream().write(bytes, 0, read);
	        }
		}
		catch(Exception e)
		{
			System.out.println(e.getLocalizedMessage());
		}
	}
}
