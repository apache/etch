// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Collections.Generic;
using System.Text;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Models a url of the form scheme://user:password@host:port/uri;parms?terms#fragment
    /// </summary>
    public class URL
    {
        #region MEMBER VARIABLES

        private string scheme;
        private string user;
        private string password;
        private string host;
        private int? port;
        private string uri;
        private List<string> parms;
        private Dictionary<string, object> terms;
        private string fragment;

        #endregion

        #region CONSTRUCTORS

        /// <summary>
        /// Constructs a url from a string.
        /// </summary>
        /// <param name="urlString"></param>
        public URL( string urlString )
        {
            Parse( urlString );
        }

        /// <summary>
        /// Constructs a URL from another URL
        /// </summary>
        /// <param name="other"></param>
        public URL( URL other )
        {
            scheme = other.scheme;
            user = other.user;
            password = other.password;
            host = other.host;
            port = other.port;
            uri = other.uri;
            parms = CopyList( other.parms );
            terms = CopyTerms( other.terms );
            fragment = other.fragment;
        }

        /// <summary>
        /// Constructs an empty URL
        /// </summary>
        public URL()
        {
            // nothing to do.
        }

        #endregion

        private void Parse( string s )
        {
            // s is scheme:[//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]

            string[] x = StringUtil.LeftSplit( s, ':' );
            if ( x == null )
                throw new ArgumentNullException( "missing scheme" );

            Scheme = Unescape( x[ 0 ] );
            s = x[ 1 ];

            // s is [//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]

            x = StringUtil.LeftSplit( s, '#' );
            if ( x!=null )
            {
                Fragment = Unescape( x[ 1 ] );
                s = x[ 0 ];
            }

            // s is [//[user[:password]@]host[:port]/]uri[;params][?terms]

            x = StringUtil.LeftSplit( s, '?' );
            if ( x != null )
            {
                ParseTerms( x[ 1 ] );
                s = x[ 0 ];
            }

            // s is [//[user[:password]@]host[:port]/]uri[;params]

            x = StringUtil.LeftSplit( s, ';' );
            if ( x != null )
            {
                ParseParams( x[ 1 ] );
                s = x[ 0 ];
            }

            // s is [//[user[:password]@]host[:port]/]uri

            if ( s.StartsWith( "//" ) )
            {
                s = s.Substring( 2 );
                // s is [user[:password]@]host[:port]/uri
                x = StringUtil.LeftSplit( s, '/' );
                if ( x != null )
                {
                    // s is [user[:password]@]host[:port]/uri
                    ParseHost( x[ 0 ] );
                    s = x[ 1 ];
                }
                else
                {
                    // s is [user[:password]@]host[:port]
                    ParseHost( s );
                    s = "";
                }
            }
            Uri = Unescape( s );
        }

        private void ParseHost( string s )
        {
            // s is [user[:password]@]host[:port]
            string[] x = StringUtil.LeftSplit( s, '@' );
            if ( x != null )
            {
                ParseUserPassword( x[ 0 ] );
                ParseHostPort( x[ 1 ] );
            }
            else
            {
                ParseHostPort( s );
            }
        }

        private void ParseUserPassword( string s )
        {
            // s is user[:password]
            string[] x = StringUtil.LeftSplit( s, ':' );
            if ( x != null )
            {
                User = Unescape( x[ 0 ] );
                Password = Unescape( x[ 1 ] );
            }
            else
            {
                User = Unescape( s );
            }
        }

        private void ParseHostPort( string s )
        {
            // s is host[:port]
            string[] x = StringUtil.LeftSplit( s, ':' );
            if ( x != null )
            {
                Host = Unescape( x[ 0 ] );
                string p = Unescape(x[1]);
                CheckNotInteger("port", p);
                Port = int.Parse(p);
            }
            else
            {
                Host = Unescape( s );
            }
        }

        private void ParseParams( string s )
        {
            // s is param[;param]*
            if ( s.Length == 0 )
                return;

            EnsureParams();

            string[] x;
            while ( ( x = StringUtil.LeftSplit( s, ';' ) ) != null )
            {
                AddParam( Unescape( x[ 0 ] ) );
                s = x[ 1 ];
            }
            AddParam( Unescape( s ) );
        }

        private void ParseTerms( string s )
        {
            // s is term[&term]*

            if ( s.Length == 0 )
                return;

            string[] x;
            while ( ( x = StringUtil.LeftSplit( s, '&' ) ) != null )
            {
                ParseTerm( x[ 0 ] );
                s = x[ 1 ];
            }
            ParseTerm( s );
        }

        private void ParseTerm( string s )
        {
            // s is name[=value]

            if ( s.Length == 0 )
                return;

            EnsureTerms();

            string[] x = StringUtil.LeftSplit( s, '=' );
            if ( x != null )
                AddTerm( Unescape( x[ 0 ] ), Unescape( x[ 1 ] ) );
            else
                AddTerm( Unescape( s ), "" );
        }

        #region SCHEME
        
        /// <summary>
        /// Gets and Sets the scheme. 
        /// Return the scheme which may be null but not blank.
        /// </summary>
        public string Scheme
        {
            get 
            {
                return scheme;
            }
            set
            {
                CheckNotBlank( "scheme", value );
                scheme = value;
            }
        }

        /// <summary>
        /// Tests the scheme for a match. The schemes are case-insensitive.
        /// </summary>
        /// <param name="testScheme">a scheme to test against.</param>
        /// <returns>true if the schemes match, false otherwise.</returns>
        public bool IsScheme( string testScheme )
        {
            return testScheme.Equals( scheme, StringComparison.CurrentCultureIgnoreCase );
        }

        #endregion

        #region USER

        public string User
        {
            get
            {
                return user;
            }
            set
            {
                CheckNotBlank( "user", value );
                user = value;
            }
        }

        #endregion

        #region PASSWORD

        public string Password
        {
            get
            {
                return password;
            }
            set
            {
                //CheckNotBlank( "password", value );
                password = value;
            }
        }

        #endregion

        #region HOST

        public string Host
        {
            get
            {
                return host;
            }
            set
            {
                CheckNotBlank( "host", value );
                host = value;
            }
        }

        #endregion

        #region PORT

        public int? Port
        {
            get
            {
                return port;
            }
            set
            {
                if ( value != null && value < 0 || value > 65535 )
                    throw new ArgumentOutOfRangeException( "port < 0 || port > 65535 " );
                port = value;
            }
        }

        public bool HasPort()
        {
            return port != null;
        }

        #endregion

        #region URI

        public string Uri
        {
            get
            {
                return uri;
            }
            set
            {
                uri = value;
            }
        }

        #endregion

        #region PARAMS

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if there is atleast one param</returns>
        public bool HasParams()
        {
            return ( ( parms!=null ) && ( parms.Count > 0 ) );
        }

        /// <summary>
        /// Fetches the first param found which starts with the given prefix. The
        /// search order is not specified, and the params are not maintained in any
        /// specific order.
        /// </summary>
        /// <param name="prefix">the prefix of the param to fetch (e.g., "transport=").</param>
        /// <returns>the param which starts with the specified prefix.</returns>
        /// 
        public string GetParam( string prefix )
        {
            CheckNotNull( prefix, "prefix == null");

            if (parms == null)
                return null;

            foreach (string p in parms)
                if (p.StartsWith(prefix))
                    return p;

            return null;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>an iterator over all the params. The params are strings, generally
        /// of the form "transport=tcp". But they can be anything you like, really.
        /// The iterator might be empty.</returns>
        /// 
        public string[] GetParams()
        {
            if ( parms == null )
                return new string[] {};

            return parms.ToArray();
        }

        /// <summary>
        /// Adds a param to the set of params for this url. Only the set of unique
        /// params is maintained. Duplicate param values are suppressed.
        /// </summary>
        /// <param name="param">a param (e.g., "transport=tcp" or "01831864574898475").</param>
        /// 
        public void AddParam( string param )
        {
            CheckNotNull(param, "param == null");
            EnsureParams();
            parms.Add( param );
        }

        /// <summary>
        /// Removes the first param found which starts with the given prefix. The
        /// search order is not specified, and the params are not maintained in any
        /// specific order.
        /// 
        /// </summary>
        /// <param name="prefix">the prefix of the param to remove (e.g., "transport=").</param>
        /// <returns>the param removed.</returns>
        /// 
        public string RemoveParam( string prefix )
        {
            CheckNotNull(prefix, "prefix == null");

            if ( parms == null )
                return null;

            foreach ( string p in GetParams() )
            {
                if ( p.StartsWith( prefix ) )
                {
                    parms.Remove( p );
                    return p;
                }
            }
            return null;
        }

        /// <summary>
        /// Clear all params
        /// </summary>
        public void ClearParams()
        {
            if ( parms != null )
                parms.Clear();
        }

        public void EnsureParams()
        {
            if ( parms == null )
                parms = new List<string>();
        }

        #endregion

        #region QUERY TERMS

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if there is at least one query term. Query terms
        /// are of the form name=value</returns>
        public bool HasTerms()
        {
            return terms != null && terms.Count > 0;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns>true if there is at least one query term with the specified
        /// name</returns>
        public bool HasTerm( string name )
        {
            CheckName(name);

            if ( terms == null )
                return false;

            return terms.ContainsKey( name );
        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>if there is a query term with the specified value.</returns>
        public bool HasTerm( string name, string value )
        {
            CheckName(name);

            if ( terms == null )
                return false;

            if (value == null)
                return HasTerm(name);

            object obj;
            if (!terms.TryGetValue(name, out obj))
                return false;

            if ( obj is List<string> )
                return ( ( List<string> ) obj ).Contains( value );

            return obj.Equals( value );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>if there is a query term with the specified value.</returns>
        public bool HasTerm( string name, int? value )
        {
            return HasTerm( name, ToString(value) );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>if there is a query term with the specified value.</returns>
        public bool HasTerm(string name, double? value)
        {
            return HasTerm(name, ToString(value));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>if there is a query term with the specified value.</returns>
        public bool HasTerm(string name, bool? value)
        {
            return HasTerm(name, ToString(value));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns>true if the query term specified by name has multiple values.</returns>
        public bool HasMultipleValues(string name)
        {
            CheckName(name);

            if ( terms == null )
                return false;

            object obj;
            if (!terms.TryGetValue(name, out obj))
                return false;

            if ( obj is List<string> )
                return ( ( List<string> ) obj ).Count > 1;

            return false;
        }

        /// <summary>
        /// Gets the value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the value of the specified term, or null if not found.</returns>
        public string GetTerm(string name)
        {
            CheckName(name);

            if (terms == null)
                return null;

            object obj;
            if (!terms.TryGetValue(name, out obj))
                return null;

            if (obj is List<string>)
            {
                IEnumerator<string> i = ((List<string>)obj).GetEnumerator();

                if (!i.MoveNext())
                    return null;

                string v = i.Current;

                if (i.MoveNext())
                    throw new Exception(string.Format("term {0} has multiple values", name));

                return v;
            }

            return (string)obj;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="defaultValue"></param>
        /// <returns>the value of the specified term, or defaultValue if not found.</returns>
        public string GetTerm( string name, string defaultValue )
        {
            string value = GetTerm( name );
            if ( value == null )
                return defaultValue;
            return value;
        }

        /// <summary>
        /// Gets the integer value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the integer value, or null if not found.</returns>
        /// 
        public int? GetIntegerTerm(string name)
        {
            string s = GetTerm(name);
            if (s == null)
                return null;
            return int.Parse(s);
        }

        /// <summary>
        /// Gets the integer value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="defaultValue">the value to return if the term is not found.</param>
        /// <returns>the integer value, or defaultValue if not found.</returns>
        /// <see cref="GetTerm( string )"/>
        /// 
        public int GetIntegerTerm(string name, int defaultValue)
        {
            int? v = GetIntegerTerm(name);
            if (v == null)
                return defaultValue;
            return v.Value;
        }

        /// <summary>
        /// Gets the double value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the double value, or null if not found.</returns>
        /// 
        public double? GetDoubleTerm(string name)
        {
            string s = GetTerm(name);
            if (s == null)
                return null;
            return double.Parse(s);
        }

        /// <summary>
        /// Gets the double value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="defaultValue">the value to return if the term is not found.</param>
        /// <returns>the double value, or defaultValue if not found.</returns>
        /// <see cref="GetTerm( string )"/>
        /// 
        public double GetDoubleTerm(string name, double defaultValue)
        {
            double? v = GetDoubleTerm(name);
            if (v == null)
                return defaultValue;
            return v.Value;
        }

        /// <summary>
        /// Gets the boolean value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the boolean value, or null if not found.</returns>
        /// <see cref="GetTerm(string)"/>
        public bool? GetBooleanTerm(string name)
        {
            string s = GetTerm(name);
            if (s == null)
                return null;
            return s.Equals("true", StringComparison.CurrentCultureIgnoreCase);
        }

        /// <summary>
        /// Gets the bool value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="defaultValue">the value to return if the term is not found.</param>
        /// <returns>the bool value, or defaultValue if not found.</returns>
        /// <see cref="GetTerm( string )"/>
        /// 
        public bool GetBooleanTerm(string name, bool defaultValue)
        {
            bool? v = GetBooleanTerm(name);
            if (v == null)
                return defaultValue;
            return v.Value;
        }

        /// <summary>
        /// Gets the values of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>an iterator over the string values of the query term. May be empty.</returns>
        public string[] GetTerms( string name )
        {
            CheckName(name);

            if ( terms == null )
                return new string[] {};

            object obj;
            if (!terms.TryGetValue(name, out obj))
                return new string[] { };

            if ( obj is List<string> )
                return ( ( List<string> ) obj ).ToArray();

            return new string[] { (string)obj };
        }

        /// <summary>
        /// Gets the names of the query terms.
        /// </summary>
        /// <returns>an iterator over the string names.</returns>
        public string[] GetTermNames()
        {
            if ( terms == null )
                return new string[] { };

            return ToArray(terms.Keys);
        }

        /// <summary>
        /// Adds the specified query term to the set of query terms. Duplicate
        /// name/value pairs are suppressed.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void AddTerm(string name, string value)
        {
            CheckName(name);

            if (value == null)
                return;

            EnsureTerms();

            object obj;

            if (!terms.TryGetValue(name, out obj))
            {
                terms.Add( name, value );
                return;
            }

            if ( obj is List<string> )
            {
                List<string> values = (List<string>)obj;
                if (!values.Contains(value))
                    values.Add(value);
                return;
            }

            // obj is not a list but we need one, so replace obj in terms with a
            // list, then add value to the list.

            List<string> nvalues = new List<string>();
            terms[name] = nvalues;
            nvalues.Add( ( string ) obj );

            nvalues.Add( value );
        }

        /// <summary>
        /// Adds the specified query term to the set of query terms. Duplicate
        /// name/value pairs are suppressed.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void AddTerm( string name, int? value )
        {
            AddTerm(name, ToString(value));
        }

        /// <summary>
        /// Adds the specified query term to the set of query terms. Duplicate
        /// name/value pairs are suppressed.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void AddTerm(string name, double? value)
        {
            AddTerm(name, ToString(value));
        }

        /// <summary>
        /// Adds the specified query term to the set of query terms. Duplicate
        /// name/value pairs are suppressed.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void AddTerm(string name, bool? value)
        {
            AddTerm(name, ToString(value));
        }

        /// <summary>
        /// Removes all the values associated with the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>true if something was removed, false otherwise.</returns>
        public bool RemoveTerm( string name )
        {
            CheckName(name);

            if (terms == null)
                return false;

            return terms.Remove(name);
        }

        /// <summary>
        /// Removes the specified name/value pair from the set of query terms.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>true if the name/value pair was found and removed.</returns>
        /// 
        public bool RemoveTerm( string name, string value )
        {
            CheckName(name);

            if ( terms == null )
                return false;

            if (value == null)
                return RemoveTerm(name);

            object obj;
            if (!terms.TryGetValue(name, out obj))
                return false;

            if ( obj is List<string> )
            {
                List<string> x = ( List<string> ) obj;
                bool ok = x.Remove( value );
                if (x.Count == 0)
                    terms.Remove(name);
                return ok;
            }

            if (obj.Equals(value))
            {
                terms.Remove(name);
                return true;
            }

            return false;
        }

        public Boolean RemoveTerm(string name, int? value)
        {
            return RemoveTerm(name, ToString(value));
        }

        public Boolean RemoveTerm(string name, double? value)
        {
            return RemoveTerm(name, ToString(value));
        }

        public Boolean RemoveTerm(string name, bool? value)
        {
            return RemoveTerm(name, ToString(value));
        }

        /// <summary>
        /// Removes all query terms
        /// </summary>
        public void ClearTerms()
        {
            if ( terms != null )
                terms.Clear();
        }

        private void EnsureTerms()
        {
            if ( terms == null )
                terms = new Dictionary<string, object>();
        }


        #endregion

        #region FRAGMENT

        public string Fragment
        {
            get
            {
                return fragment;
            }
            set
            {
                CheckNotBlank( "fragment", value );
                fragment = value;
            }
        }

        #endregion

        #region UTIL

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();

            Escape( sb, scheme );
            sb.Append( ':' );

            if ( host != null )
            {
                sb.Append( "//" );
                if ( user != null )
                {
                    Escape( sb, user );
                    if ( password != null )
                    {
                        sb.Append( ':' );
                        Escape( sb, password );
                    }
                    sb.Append( '@' );
                }
                Escape( sb, host );
                if ( port != null )
                {
                    sb.Append( ':' );
                    sb.Append( port );
                }
                sb.Append( '/' );
            }

            if ( uri != null )
                Escape( sb, uri );

            if ( HasParams() )
                ParamsToString( sb );

            if ( HasTerms() )
                TermsToString( sb );

            if ( fragment != null )
            {
                sb.Append( '#' );
                Escape( sb, fragment );
            }

            return sb.ToString();
        }

        public override int GetHashCode()
        {
            int code = 23547853;
		    code ^= hc( scheme );
		    code ^= hc( user );
		    code ^= hc( password );
		    code ^= hc( host );
		    code ^= hc( port );
		    code ^= hc( uri );
		    code ^= hc( parms );
		    code ^= hc( terms );
		    code ^= hc( fragment );
		    return code;
        }

        private int hc( Dictionary<string,object> m )
	    {
		    return m != null ? m.GetHashCode() : 793;
	    }

	    private int hc( List<string> s )
	    {
		    return s != null ? s.GetHashCode() : 161;
	    }

	    private int hc( int? i )
	    {
		    return i != null ? i.GetHashCode() : 59;
	    }

	    private int hc( string s )
	    {
		    return s != null ? s.GetHashCode() : 91;
	    }


        public override bool Equals( object obj )
        {
            if ( obj == this )
                return true;

            if (obj == null)
                return false;

            if (obj.GetType() != typeof(URL))
                return false;

            URL other = ( URL ) obj;

            if ( !StringUtil.EqIgnoreCase(scheme, other.scheme) )
                return false;

            if (!StringUtil.Eq(user, other.user))
                return false;

            if (!StringUtil.Eq(password, other.password))
                return false;

            if (!StringUtil.Eq(host, other.host))
                return false;

            if (!Eq(port, other.port))
                return false;

            if (!StringUtil.Eq(uri, other.uri))
                return false;

            if ( !Eq( parms, other.parms ) )
                return false;

            if ( !Eq( terms, other.terms ) )
                return false;

            if (!StringUtil.Eq(fragment, other.fragment))
                return false;

            return true;
        }

        private static bool Eq(object a, object b)
        {
            if (ReferenceEquals(a, b))
                return true;

            if (a == null || b == null)
                return false;

            return a.Equals(b);
        }

        private void ParamsToString( StringBuilder sb )
        {
            foreach (string param in GetParams())
            {
                sb.Append( ';' );
                Escape( sb, param );
            }
        }

        private void TermsToString( StringBuilder sb )
        {
            char sep = '?';
            foreach (string name in GetTermNames())
            {
                foreach (string value in GetTerms(name))
                {
                    sb.Append(sep);
                    Escape(sb, name);
                    sb.Append('=');
                    Escape(sb, value);
                    sep = '&';
                }
            }
        }

        /// <summary>
        /// Dumps URL contents for easy viewing
        /// </summary>
        public void Dump()
	    {
		    Console.WriteLine( "---------------" );
		    Console.WriteLine( "scheme = "+scheme );
		    Console.WriteLine( "user = "+user );
		    Console.WriteLine( "password = "+password );
		    Console.WriteLine( "host = "+host );
		    Console.WriteLine( "port = "+port );
		    Console.WriteLine( "uri = "+uri );
		    Console.WriteLine( "params = "+parms );
		    Console.WriteLine( "terms = "+terms );
		    Console.WriteLine( "fragment = "+fragment );
        }

        private static void Escape( StringBuilder sb, string s )
        {
            if (s == null)
            {
                sb.Append("null");
                return;
            }

            CharIterator i = new CharIterator( s );
            while ( i.MoveNext() )
            {
                char c = i.Current;
                if ( IsEscaped( c ) )
                {
                    sb.Append( '%' );
                    sb.Append( StringUtil.ToHex( ( c >> 4 )& 15 ) );
                    sb.Append( StringUtil.ToHex( c & 15 ) );
                }
                else if ( c == ' ' )
                {
                    sb.Append( '+' );
                }
                else
                {
                    sb.Append( c );
                }
            }
        }

        private static bool IsEscaped( char c )
        {
            if ( c >= '0' && c <= '9' )
                return false;
            if ( c >= 'A' && c <= 'Z' )
                return false;
            if ( c >= 'a' && c <= 'z' )
                return false;
            if ( c == ' ' )
                return false;
            if ( c == ',' )
                return false;
            if ( c == '.' )
                return false;
            if ( c == '/' )
                return false;
            if ( c == '!' )
                return false;
            if ( c == '$' )
                return false;
            if ( c == '^' )
                return false;
            if ( c == '*' )
                return false;
            if ( c == '(' )
                return false;
            if ( c == ')' )
                return false;
            if ( c == '_' )
                return false;
            if ( c == '-' )
                return false;
            if ( c == '{' )
                return false;
            if ( c == '}' )
                return false;
            if ( c == '[' )
                return false;
            if ( c == ']' )
                return false;
            if ( c == '|' )
                return false;
            if ( c == '\\' )
                return false;
            return true;
        }

        private static string Unescape( string s )
        {
            StringBuilder sb = new StringBuilder();
            CharIterator i = new CharIterator( s );
            while ( i.MoveNext() )
            {
                char c = i.Current;
                if ( c == '%')
                {
                    i.MoveNext();
                    char c1 = i.Current;
                    i.MoveNext();
                    char c2 = i.Current;
                    sb.Append( ( char ) ( ( StringUtil.FromHex( c1 ) << 4 ) | StringUtil.FromHex( c2 ) ) );
                }
                else if ( c == '+' )
                {
                    sb.Append( ' ' );
                }
                else
                {
                    sb.Append( c );
                }
            }
            return sb.ToString();
        }

        private static void CheckName(string name)
        {
            if (name == null || name.Length == 0)
                throw new ArgumentException("name null or empty");
        }

        private static string ToString(object value)
        {
            return value != null ? value.ToString() : null;
        }

        private static string[] ToArray(Dictionary<string, object>.KeyCollection keyCollection)
        {
            string[] a = new string[keyCollection.Count];
            keyCollection.CopyTo(a, 0);
            return a;
        }

        private void CheckNotNull(string value, string msg)
        {
            if (value == null)
                throw new NullReferenceException(msg);
        }

        private static void CheckNotBlank( string name, string value )
        {
            if ( value != null && value.Length == 0 )
                throw new ArgumentException( name + " is blank" );
        }

        private static void CheckNotInteger(string name, string value)
        {
            CheckNotBlank(name, value);
            try
            {
                int.Parse(value);
            }
            catch (FormatException)
            {
                throw new ArgumentException(name + " is not integer");
            }
        }

        private static List<string> CopyList( List<string> parms )
        {
            // just goes one level deep.

            if (parms == null)
                return null;

            return new List<string>(parms);
        }

        private static Dictionary<string, object> CopyTerms( Dictionary<string, object> terms )
        {
            if ( terms == null )
                return null;

            Dictionary<string, object> nterms = new Dictionary<string, object>();
            foreach (KeyValuePair<string, object> me in terms)
            {
                string name = me.Key;
                object value = me.Value;
                if (value is List<string>)
                    value = CopyList((List<string>) value);
                nterms.Add(name, value);
            }
            return nterms;
        }

        #endregion
    }
}
