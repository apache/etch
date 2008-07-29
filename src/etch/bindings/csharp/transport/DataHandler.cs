using System;
using System.Text;
using System.Collections.Generic;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary>
    /// Interface used to deliver data from a stream source.
    /// </summary>
    public interface DataHandler : SourceHandler<Source> 
    {

        /// <summary>
        /// Delivers data from a stream source.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="buf"></param>
        void Data( Who sender, FlexBuffer buf );
    }
}
