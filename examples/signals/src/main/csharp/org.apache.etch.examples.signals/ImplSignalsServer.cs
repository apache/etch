// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
//

using System;

using org.apache.etch.examples.signals.types.Signals;
using Org.Apache.Etch.Bindings.Csharp.Util;
using System.Timers;

///<summary>Your custom implementation of BaseSignalsServer. Add methods here to provide
///implementation of messages from the client. </summary>
namespace org.apache.etch.examples.signals
{
  ///<summary>Implementation for ImplSignalsServer</summary>
  public class ImplSignalsServer : BaseSignalsServer
  {
    private static int clientCount;

    System.Timers.Timer signalTimer;
    private int signalCount;

    /// <summary>Constructs the ImplSignalsServer.</summary>
    /// <param name="client">a connection to the client session. Use this to
    /// send a message to the client.</param>
    public ImplSignalsServer( RemoteSignalsClient client )
      : base( client )
    {
    }

    public override string get_free_client_name()
    {
      string clientName = "client" + clientCount;

      clientCount++;

      Console.WriteLine( "Allocated client name: " + clientName );
      return clientName;
    }

    public override void _SubscribeExampleSignal( Who sender )
    {
      base._SubscribeExampleSignal( sender );
      Console.WriteLine( "Subscribed " + sender );

      if ( signalTimer == null )
      {
        Console.WriteLine( "Starting timer" );

        signalTimer = new System.Timers.Timer();
        signalTimer.Interval = 1000;
        signalTimer.Elapsed += TimerElapsedHandler;
        signalTimer.Enabled = true;
      }
    }

    public override void _UnsubscribeExampleSignal( Who sender )
    {
      base._UnsubscribeExampleSignal( sender );
      Console.WriteLine( "Unsubscribed " + sender );

      if ( client.ExampleSubscribers.Length < 1 )
      {
        Console.WriteLine( "No subscribers, stopping timer" );

        DisposeTimer();
      }
    }

    public override void _SessionNotify( object eventObj )
    {
      if ( eventObj.Equals( SessionConsts.DOWN ) )
      {
        Console.WriteLine( "Session DOWN, stopping timer" );
        DisposeTimer();
      }
    }

    private void DisposeTimer()
    {
      if ( signalTimer != null )
      {
        signalTimer.Enabled = false;
        signalTimer.Dispose();
        signalTimer = null;
      }
    }

    private void TimerElapsedHandler( object sender, ElapsedEventArgs e )
    {
      if ( signalTimer != null )
      {
        signalCount++;
        Console.WriteLine( "Sending event to " + client.ExampleSubscribers.Length + " subscribers (" + signalCount + ")" );
        client.example_signal( signalCount );
      }
    }
  }
}