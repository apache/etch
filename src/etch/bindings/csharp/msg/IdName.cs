/// $Id$
/// <Author> Created by Champakesan, Badri Narayanan on Jun 8, 2007. </Author>
/// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;

namespace Etch.Msg
{
    /// <summary>An IdName is a base class for Field or Type. It is used
    /// to bind together a type or field name with the associated id. The id
    /// is used for certain operations, such as the key in a Map, comparisons,
    /// and binary encoding on the wire, while the name is used for display.
    /// <see cref=""/> Field
    /// <see cref=""/> Type</summary>
    public class IdName
    {
        /// <summary>Constructs the IdName.
        /// <param name="id">id for the name (normally computed using</param> 
        /// <param name="name"></param>
        /// {<link>#hash(String)})</link></summary>
        public IdName(int? id, String name)
        {
            this.id = id;
            this.name = name;
        }

        /// <summary>Constructs the IdName by computing the appropriate id given the name.
        /// <param name="name"> name the name of the item.</param>
        /// <see cref=""/> #hash(String)
        /// </summary>
        public IdName(string name)
            : this(Hash(name), name)
        { }

        private readonly int? id;

        private readonly string name;

        /// <returns>the id of the item. </returns> 
        public int? Id
        {
            get
            {
                return id;
            }
        }

        /// <returns> the name of the item.</returns>
        public string Name
        {
            get
            {
                return name;
            }
        }

        /// Override 
        public override string ToString() 
        {
            return String.Format( "{0}({1})", name, id );
        }

        /// Override 
        public override bool Equals(object obj) 
        {
            if(obj == this)
                return true;

            if(obj == null)
                return false;

            if(this.GetType() != obj.GetType())
                return false;

            IdName other = (IdName) obj;

            return id.Equals( other.Id ) && name.Equals( other.Name );
        }

        /// Override
        public override int GetHashCode() 
        {
            return id.GetHashCode() ^ name.GetHashCode();
        }

        /// <summary>Computes the hash value of the name to be used as the id when
        /// constructing an IdName.</summary>
        /// <param name="name"> name the name of the type or field. </param>
        /// <returns>a hash of name in the unicode character encoding which is
        /// very likely to be unique over reasonable name spaces. Collisions
        /// should be very unlikely as they will force the user to change the
        /// name.</returns>
        public static int Hash(String name)
        {
            // combination of djb2 and sdbm hash algorithm:
            int hash = 5381;
            int n = name.Length;
            for(int i = 0; i < n; i++)
            {
                char c = name[i]; 
                int h6 = hash << 6;
                hash = (h6 << 10) + h6 - hash + c;
            }
            return hash;
        }
    }
}
