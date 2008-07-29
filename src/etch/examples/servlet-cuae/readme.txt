To view a rough video of the app, you can just double-click example.html.

To build this demo, you will need:


* Eclipse
	- Plugin for Eclipse: Installed Callisto 'Web and J2EE Development' plugins to aid in rapid tomcat development.

* Etch 0.9 		(I strongly recommend extracting the etch\build-output\binary-packages\etch-0.9.zip to c:\etch-0.9\, because that's where all the code expects it to be)
* Tomcat 5.5 		(I recommend extracting to c:\)
* CUAD 2.4 		(default isntall path)
* Visual Studio 2005 	(doesn't matter where you install this).
* nunit 2.4.1 		(http://www.nunit.org/ -- install default path


1) Building the Visual Studio project should be almost as simple as opening the project and hitting build, aside from one or two things. 
	- Most likely, the references will be broken for the ProviderExample project.   Re-add each broken dll by clicking References > Add Reference and browsing to c:\program files\cisco system\unified application designer\framework\1.0\coreassemblies
	- The PostBuild project has a file called PostBuild.bat.  The paths in this file are most likely wrong.  You'll need to fix them for your own setup, and also mcp.exe is only on the CUAE, but you'll need to copy it to your development machine (and CUAD already installed!).
	- If everything went well, then twoway.mcp is available in the provider\BridgeProvider\PostBuild directory, which can be uploaded to the CUAE via mceadmin > Providers > Upload

2) Open CUAD, and open the \mca\MCA.max file, and hit build.  You can deploy this application to a CUAE server. Once you've deployed the application, you will need to configure it to push to a specific phone.
Configure CUAE to push to one specific IP Phone by:
mceadmin > Applications > MCA > 
	- PhoneIP : IP address of a phone
	- PhoneUser : Username of a phone
	- PhonePass : Password of a phone
	- CUAEIP : IP address of the CUAE server


3) If you have the Callisto 'Web and J2EE Development plugin', you should be able to run the tomcat servlet by hitting Debug or Run in Eclipse.  You can always manually load the content into a tomcat install, as well.



To run this demo, you will need:

If everything is running, you should be able to go to http://tomcatIP:8080/ExampleServlet/ExampleServlet.  This will bring up a dialog.  It's pretty easy to do beyond there.  If you have problems, check the console out from the eclipse servlet, as well as check the Application Server log in mceadmin > Server Logs > Appserver