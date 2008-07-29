
- - - - - - - - 
Setting paths
- - - - - - - - 

1) Go to My Computer, Advanced, Environment Variables.
2) Add a new variable, ANT_HOME and set to C:\Program Files\apache-ant-1.7.0
3) Add the following paths to the PATH system variable: 
	C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727; (or your installed .NET path)
	C:\Program Files\apache-ant-1.7.0\bin or your installed ANT bin path);
	C:\<Etch_Installed_Path>\bin
4) Add a new variable, ETCH_HOME and set to C:\<Etch_Installed_Path>


- - - - - - - - - - - - - - - - - - - - - - - - 
Executing Distributed Hash Table for C#
- - - - - - - - - - - - - - - - - - - - - - - - 

No IDE:
-------

1) The command "ant generate_cs" will generate all the necessary C# stub files, resolve all references and compile all the files required in order for this application.

2) Run "server_cs" in order to launch the C# Distributed Hash Table server.

3) Run "DistMapClient_cs" in order to launch the C# client.


With IDE (VS 2005):
-------------------

1) Start a New project and select "New Project from existing code". Say you name this DistributedHashTable (.csproj).

2) Browse to the "distmap" folder in <Etch_Installed_Path> and select "Console Application" after specifying a name for your project. 

3) First, exclude all the .java files from the project. Select each one of them in the Solution Explorer, right click on them, and say "Exclude from Project".

4) Add a reference to Etch4CS.dll. Right click on "References" in the "Solution Explorer", and say "Add a reference". Then, go to the "browse" tab, go to the lib directory in your Etch path and select Etch4CS.dll. 

5) Add a reference to NUnit. Right click on "References" in the "Solution Explorer", and say "Add a reference". Go to the .NET tab, and select "nunit.framework"

5.1) In order to generate the auto-generated .cs files from the etch file, right click on the .etch file, and click on "Etch(Both)".

6) Since there are two Main(...) functions within this project, you'll have to start a different project which contains TestRemoteDistMapCommandLine.cs

7) Start a New project and select "New Project from existing code". Give the same path as before. Say you name this "DistMapClient"

8) Close this solution for now. ReLoad the previous solution. Right click on this solution, and click on Add -> Existing Project -> "DistMapClient.csproj".

9) In this project, exclude all the files, except for "TestRemoteDistMap.cs". Also, in DistributedHashTable.csproj, exclude ""TestRemoteDistMapCommandLine.cs"

10) Add the same references as before to "DistMapClient.csproj" as well. Also, add a reference to "DistributedHashTable.csproj".

11) Right click on "DistMapClient.csproj" and say "Unload Project". 

12) Open this same solution in another VS2005 instance. In this case, unload DistributedHashTable.csproj. 

13) Run (Ctrl + F5) the solution in the previous instance of VS2005 first i.e., the Listener. 

14) Now, run the solution in the other instance of VS2005, ( the one with the "DistMapClient.cs" file in it ) i.e., the client. 

- - - - - - - - - - - - - - - - - - - - -
Executing DistributedHashTable for Java
- - - - - - - - - - - - - - - - - - - - -

No IDE:
-------
1) The command "ant generate_java" will generate all the necessary Java stub files, resolve all references and compile all the files required in order for this application.

2) Run "server_java" in order to launch the Java Distributed Hash Table server.

3) Run "DistMapClient_java" in order to launch the java client.

NOTE: In this build, there is no Client for java. Work to do. 
