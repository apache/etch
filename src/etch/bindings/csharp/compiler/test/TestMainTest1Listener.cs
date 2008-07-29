using System;
using System.Collections.Generic;
using System.Text;
using etch.bindings.csharp.compiler.test;
using NUnit.Framework;

namespace Etch.Compiler.Test
{
    public class TestMainTest1Listener
    {
        [Test]
        public void NewTestServerMethod()
        {
            MainTest1Listener m = new MainTest1Listener();
            Object o = m.NewTest1Server( null );
            Assert.IsNotNull( o );
            Assert.IsTrue( o is ImplTest1Server );
        }

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine( "TestListener" );
        }

        private class ListenerThread
        {
            private Boolean myHasErrors;

            public ListenerThread()
            {
                myHasErrors = false;
            }

            public void Run()
            {
                try
                {
    //                MainTest1Listener.Main( null );
                }
                catch ( Exception e )
                {
                    Console.WriteLine( e.StackTrace );
                    myHasErrors = false;
                }
            }

            public Boolean HasErrors()
            {
                return myHasErrors;
            }
        }
    }
}
