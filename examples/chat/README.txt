To build:

> ant debug

The debug ant target will build both the java and csharp
programs.

To run java chat, open a cmd window and run these commands:

> cd target\bin
> start java -cp chat.jar etch.examples.chat.MainChatListener
> start java -cp chat.jar etch.examples.chat.MainChatClient
> start java -cp chat.jar etch.examples.chat.MainChatClient

Two chat clients will be started; there is a help command;
login as a different user in each with any password; they
can chat with each other!

To run csharp chat:

> cd target\bin
> start ChatListener.exe
> start ChatClient.exe
> start ChatClient.exe

You can mix and match the various clients and listeners.
