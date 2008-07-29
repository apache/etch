///$Id$
/// 
///Created by Champakesan, Badri Narayanan on Jun 8, 2007.
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
   
using System;

namespace Etch.Msg
{

   /// <summary>Field is an IdName which denotes a field of a struct or message (i.e., a key for a value).</summary>
    public class Field : IdName
    {
        ///<summary>Constructs the Field.</summary>
        /// <param name="id">id the id of the field.</param>
        /// <param name="name">name the name of the field.</param>
        public Field(int id, string name)
            : base(id, name)
        { }

        /// <param name="name">name the name of the field.</param>
        public Field(string name)
            : base(name)
        { }
    }
}