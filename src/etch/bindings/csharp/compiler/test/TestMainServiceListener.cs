using System;
using System.Collections.Generic;
using System.Text;
using etch.bindings.csharp.compiler.test;

namespace Etch.Compiler.Test
{
    public class TestMainServiceListener
    {
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
                    MainTestListener.Main1( null );
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
