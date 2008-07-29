using System;
using System.Text;
using System.Threading;
using System.Collections.Generic;

namespace Etch.Transport
{
    /// <summary>
    /// Implementation of startable which creates a thread and runs.
    /// </summary>
    abstract public class Runner : AbstractStartable 
    {
        /// <summary>
        /// Constructs the Runner.
        /// </summary>
        public Runner()
        { 
            // nothing to do.
        }

        /// <summary>
        /// The handler is used to report started and stopped events.
        /// </summary>
        private RunnerHandler handler;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="handler">handler the handler to notify of started and stopped events.</param>
        public void SetHandler(RunnerHandler handler)
        {
            this.handler = handler;
        }

        protected override void Start0()
        {
            thisThread = new Thread(new ThreadStart(Run));
            thisThread.Start();
        }

        Thread thisThread;

        protected override void Stop0()
        {
            Thread t = thisThread;
            if (t != null)
            {
                thisThread = null;
      //          t.Join( 30 );
            }
        }

        public void Run()
        {
            FireStarted();

            try
            {
                bool first = true;
                while (IsStarted())
                {
                    if (!Run0(first))
                        break;
                    first = false;
                }
            }
            catch (Exception e)
            {
                FireException("run", e);
            }
            finally
            {
                SetStopped();
                FireStopped();
            }
        }

        private void FireStarted()
        {
            handler.Started();
        }

        /// <summary>
        /// Reports an exception caught by the thread managing the runnable.
        /// </summary>
        /// <param name="what"></param>
        /// <param name="e"></param>
        protected void FireException(String what, Exception e)
        {
            // Console.WriteLine( e );
            try
            {
                handler.Exception( what, e );
            }
            catch ( Exception x )
            {
                Console.WriteLine( x.Message );
                Console.WriteLine( x.StackTrace );
            }
        }

        private void FireStopped()
        {
            handler.Stopped();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="first">true the first time we are called</param>
        /// <returns>true if we should keep running.</returns>
        /// Exception:
        ///     throws Exception
        abstract protected bool Run0(bool first);

    }
}
