/// $Id$
/// <Author> Created by Champakesan, Badri Narayanan on Jun 8, 2007. </Author>
/// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using System.Collections.Generic;

namespace Etch.Msg
{
    /// <summary>
    /// Type denotes the type of a struct or message. When used with a
    /// message it typically denotes an action or event.
    /// </summary>
    public class XType : IdName
    {
        /// <summary>Constructs the Type.</summary>
        /// <param name="id">id of the type.</param>
        /// <param name="name">name of the type.</param>
	    public XType( int id, String name ) : base( id, name )
	    {
        }

	    /// <summary>Constructs the Type, computing the appropriate value for the id.</summary>
        /// <param name="name">name the name of the type.</param>
	    public XType( String name ) : base( name )
	    {
        }

        public Validator GetValidator( Field key )
        {
            if ( !runValidators )
                return null;
            try
            {
                return validators[ key ];
            }
            catch ( KeyNotFoundException )
            {
                return null;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the current setting of runvalidators</returns>
        public static Boolean GetRunValidators()
        {
            return runValidators;
        }

        /// <summary>
        /// Enables or disables type validation for messages.
        /// </summary>
        /// <param name="value"></param>
        public static void SetRunValidators( bool value )
        {
            runValidators = value;
        }

        private static bool runValidators = true;

        /// <summary>
        /// Adds the validator to the chain for this key.
        /// </summary>
        /// <param name="key"></param>
        /// <param name="vldtr"></param>
        public void PutValidator( Field key, Validator vldtr )
        {
            if ( vldtr == null )
                return;

            lock ( validators )
            {
                Validator v;
                if ( validators.ContainsKey( key ) )
                    v = validators[ key ];
                else
                    v = null;

                if ( v != null )
                {
                    // Required because Add in C# doesn't allow replace the existing 
                    // value if a key is already present. It just throws an exception.
                    ClearValidator( key );

                    validators.Add( key, new ComboValidator( v, vldtr ) );
                }
                else
                    validators.Add( key, vldtr );
            }
        }

        /// <summary>
        /// Removes the validator chain for this key
        /// </summary>
        /// <param name="key"></param>
        public void ClearValidator( Field key )
        {
            validators.Remove( key );
        }

        public Dictionary<Field, Validator> validators = new Dictionary<Field, Validator>();

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the set of fields for this type</returns>
        public List<Field> GetFields()
        {
            List<Field> keyList = new List<Field>();

            foreach ( Field f in validators.Keys )
                keyList.Add( f );

            return keyList;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the result type of this message type</returns>
        public XType GetResult()
        {
            return rType;
        }


        public void SetResult( XType rType )
        {
            this.rType = rType;
        }

        private XType rType;
  }
}