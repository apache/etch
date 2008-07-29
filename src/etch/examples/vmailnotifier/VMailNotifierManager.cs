// CallNotifierManager.cs
// Implementation of call notifier manager

using System;

namespace vmailnotifier
{
    public class VMailNotifierManager
	{
        private NotifierForm lastNotifier = null;           // last notifier
        static VMailNotifierManager instance = null;        // one and only callmanager instance
        static readonly object cnmlock = new object();      // sigleton lock

        /// <summary>
        /// default constructor
        /// </summary>
        private VMailNotifierManager()
        {
        }

        /// <summary>
        /// CallNotifierManager instance accessor
        /// </summary>
        public static VMailNotifierManager Instance
        {
            get
            {
                lock (cnmlock)
                {
                    if (instance == null)
                    {
                        instance = new VMailNotifierManager();
                    }
                    return instance;
                }
            }
        }

        #region public interface
        /// <summary>
        /// Add notifier
        /// </summary>
        /// <param name="cid">caller id</param>
        /// <param name="timeStamp">time stamp</param>
        /// <param name="x0">x coordinate</param>
        /// <param name="y0">y coordinate</param>
        public void AddNotifier(string cid, string timeStamp, int x0, int y0)
        {
            int xo = 0;
            int yo = 0;
            if (lastNotifier != null && lastNotifier.IsDisposed == false)
            {
                // offset it
                xo = lastNotifier.Right;
                yo = lastNotifier.Top;
            }

            lastNotifier = new NotifierForm();
            lastNotifier.ShowOptions(false);
            lastNotifier.SetDimensions(x0, y0);

            lastNotifier.Notify(cid, timeStamp, xo, yo);
        }
        #endregion
    }
}
