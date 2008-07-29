using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;

namespace Etch.Msg
{
    /// <summary>
    /// Generated code to dispatch method from message
    /// </summary>
    /// <typeparam name="S">the subclass of StubBase which needs help</typeparam>
    public interface StubHelper<S> where S : StubBase
    {
        void Run( S stub, Who sender, Message msg );
    }
}
