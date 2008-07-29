using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace Etch.Support
{
    /// <summary>
    /// A pool which runs runnable directly
    /// </summary>
    public class NullPool : Pool
    {
        #region Pool Members

        public void Run( RunDelegate d1, ExceptionDelegate d2 )
        {
            new Thread(
                new ThreadStart( delegate()
                                {
                                    try
                                    {
                                        d1();
                                    }
                                    catch ( Exception e )
                                    {
                                        d2( e );
                                    }
                                } ) ).Start();
        }

        #endregion
    }
}
