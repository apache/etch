using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary>
    /// Class to help construct transport stacks.
    /// </summary>
    public class TransportHelper
    {
        #region RESOURCES

        /// <summary>
        /// The Pool to use to execute queued async receiver messages. */
	    /// </summary>
        public static readonly String QUEUED_POOL = "QUEUED_POOL";
    	
	    /// <summary>
	    /// The Pool to use to execute free async receiver messages.
	    /// </summary>
 	    public static readonly String FREE_POOL = "FREE_POOL";
    	
	    /// <summary>
	    /// Binary transport format
	    /// </summary>
	    public static readonly String BINARY = "binary";
    	
	    /// <summary>
        /// Xml transport format
	    /// </summary>
	    public static readonly String XML = "xml";

        #endregion

        #region UTILITIES

        /// <summary>
        /// Initializes standard resources.
        /// </summary>
        /// <param name="resources">initialized with default values for standard items.</param>
        /// <returns></returns>
        public static Resources InitResources( Resources resources )
        {
            if ( resources == null )
                resources = new Resources();

            if ( !resources.ContainsKey( QUEUED_POOL ) )
                resources.Add( QUEUED_POOL, new QueuedPool() );

            if ( !resources.ContainsKey( FREE_POOL ) )
                resources.Add( FREE_POOL, new FreePool() );

            if ( !resources.ContainsKey( Messagizer.FORMAT ) )
                resources.Add( Messagizer.FORMAT, BINARY );

            return resources;
        }

        #endregion
    }
}
