using System;
using System.Text;
using System.Collections.Generic;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary>
    /// A combination of MessageSource and Source.
    /// </summary>
    public interface MessagizerMessageSource : Source, MessageSource
    {
        // nothing
    }
}
