using System;
using System.Collections.Generic;
using System.Text;
using Etch.Transport;

namespace Etch.Util
{
    /// <summary>
    /// Models a url of the form scheme://user:password@host:port/uri;parms?terms#fragment
    /// </summary>
    public class URL
    {
        #region MEMBER VARIABLES

        private String _scheme;
        private String _user;
        private String _password;
        private String _host;
        private int _port;
        private String _uri;
        private List<String> _params;
        private Dictionary<String, Object> _terms;
        private String _fragment;

        #endregion

        #region CONSTRUCTORS

        /// <summary>
        /// Constructs a url from a string.
        /// </summary>
        /// <param name="urlString"></param>
        public URL( String urlString )
        {
            Parse( urlString );
        }

        /// <summary>
        /// Constructs a URL from another URL
        /// </summary>
        /// <param name="other"></param>
        public URL( URL other )
        {
            _scheme = other.Scheme;
            _user = other.User;
            _password = other.Password;
            _host = other.Host;
            _port = other.Port;
            _uri = other.Uri;
            _params = CopyParams( other._params );
            _terms = CopyTerms( other._terms );
            _fragment = other._fragment;
        }

        /// <summary>
        /// Constructs an empty URL
        /// </summary>
        public URL()
        {
            // nothing to do.
        }

        #endregion

        private void Parse( String s )
        {
            // s is scheme:[//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]

            String[] x = StringUtil.LeftSplit( s, ':' );
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
            _uri = Unescape( s );
        }

        private void ParseHost( String s )
        {
            // s is [user[:password]@]host[:port]
            String[] x = StringUtil.LeftSplit( s, '@' );
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

        private void ParseUserPassword( String s )
        {
            // s is user[:password]
            String[] x = StringUtil.LeftSplit( s, ':' );
            if ( x != null )
            {
                _user = Unescape( x[ 0 ] );
                _password = Unescape( x[ 1 ] );
            }
            else
            {
                _user = Unescape( s );
            }
        }

        private void ParseHostPort( String s )
        {
            // s is host[:port]
            String[] x = StringUtil.LeftSplit( s, ':' );
            if ( x != null )
            {
                _host = Unescape( x[ 0 ] );
                _port = Convert.ToInt32( Unescape( x[ 1 ] ) );
            }
            else
            {
                _host = Unescape( s );
            }
        }

        private void ParseParams( String s )
        {
            // s is param[;param]*
            if ( s.Length == 0 )
                return;

            EnsureParams();

            String[] x;
            while ( ( x = StringUtil.LeftSplit( s, ';' ) ) != null )
            {
                AddParam( Unescape( x[ 0 ] ) );
                s = x[ 1 ];
            }
            AddParam( Unescape( s ) );
        }

        private void ParseTerms( String s )
        {
            // s is term[&term]*

            if ( s.Length == 0 )
                return;

            String[] x;
            while ( ( x = StringUtil.LeftSplit( s, '&' ) ) != null )
            {
                ParseTerm( x[ 0 ] );
                s = x[ 1 ];
            }
            ParseTerm( s );
        }

        private void ParseTerm( String s )
        {
            // s is name[=value]

            if ( s.Length == 0 )
                return;

            EnsureTerms();

            String[] x = StringUtil.LeftSplit( s, '=' );
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
        public String Scheme
        {
            get 
            {
                return _scheme;
            }
            set
            {
                CheckNotBlank( "scheme", value );
                _scheme = value;
            }
        }

        /// <summary>
        /// Tests the scheme for a match. The schemes are case-insensitive.
        /// </summary>
        /// <param name="testScheme">a scheme to test against.</param>
        /// <returns>true if the schemes match, false otherwise.</returns>
        public bool IsScheme( String testScheme )
        {
            return testScheme.Equals( _scheme, StringComparison.CurrentCultureIgnoreCase );
        }

        #endregion

        #region USER

        public String User
        {
            get
            {
                return _user;
            }
            set
            {
                CheckNotBlank( "user", value );
                _user = value;
            }
        }

        #endregion

        #region PASSWORD

        public String Password
        {
            get
            {
                return _password;
            }
            set
            {
                CheckNotBlank( "password", value );
                _password = value;
            }
        }

        #endregion

        #region HOST

        public String Host
        {
            get
            {
                return _host;
            }
            set
            {
                CheckNotBlank( "host", value );
                _host = value;
            }
        }

        #endregion

        #region PORT

        public int Port
        {
            get
            {
                return _port;
            }
            set
            {
                if ( value < 0 || value > 65535 )
                    throw new ArgumentOutOfRangeException( "port < 0 || port > 65535 " );
                _port = value;
            }
        }

        #endregion

        #region URI

        public String Uri
        {
            get
            {
                return _uri;
            }
            set
            {
                _uri = value;
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
            return ( ( _params!=null ) && ( _params.Count > 0 ) );
        }

        /// <summary>
        /// Fetches the first param found which starts with the given prefix. The
        /// search order is not specified, and the params are not maintained in any
        /// specific order.
        /// </summary>
        /// <param name="prefix">the prefix of the param to fetch (e.g., "transport=").</param>
        /// <returns>the param which starts with the specified prefix.</returns>
        /// 
        public String GetParam( String prefix )
        {
            if ( _params == null )
                return null;

            foreach ( String s in _params )
            {
                if ( s.StartsWith( prefix ) )
                    return s;
            }

            return null;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>an iterator over all the params. The params are strings, generally
        /// of the form "transport=tcp". But they can be anything you like, really.
        /// The iterator might be empty.</returns>
        /// 
        public IEnumerator<String> GetParams()
        {
            if ( _params == null )
                return new EmptyIterator<String>();

            return _params.GetEnumerator();
        }

        /// <summary>
        /// Adds a param to the set of params for this url. Only the set of unique
        /// params is maintained. Duplicate param values are suppressed.
        /// </summary>
        /// <param name="param">a param (e.g., "transport=tcp" or "01831864574898475").</param>
        /// 
        public void AddParam( String param )
        {
            EnsureParams();
            _params.Add( param );
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
        public String RemoveParam( String prefix )
        {
            if ( _params == null )
                return null;

            foreach ( String s in _params )
            {
                if ( s.StartsWith( prefix ) )
                {
                    _params.Remove( s );
                    return s;
                }
            }
            return null;
        }

        /// <summary>
        /// Clear all params
        /// </summary>
        public void ClearParams()
        {
            if ( _params != null )
                _params.Clear();
        }

        public void EnsureParams()
        {
            if ( _params == null )
                _params = new List<string>();
        }

        #endregion

        #region QUERY TERMS

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if there is at least one query term. Query terms
        /// are of the form name=value</returns>
        public Boolean HasTerms()
        {
            return _terms!=null && _terms.Count>0;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns>true if there is at least one query term with the specified
        /// name</returns>
        public Boolean HasTerm( String name )
        {
            if ( _terms == null )
                return false;
            return _terms.ContainsKey( name );
        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>if there is a query term with the specified value.</returns>
        public Boolean HasTerm( String name, String value )
        {
            if ( _terms == null )
                return false;

            Object obj = _terms[ name ];
            if ( obj == null )
                return false;

            if ( obj is List<String> )
                return ( ( List<String> ) obj ).Contains( value );

            return obj.Equals( value );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>if there is a query term with the specified value.</returns>
        public Boolean HasTerm( String name, int? value )
        {
            return HasTerm( name, value.ToString() );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>if there is a query term with the specified value.</returns>
        public Boolean HasTerm( String name, double? value )
        {
            return HasTerm( name, value.ToString() );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns>true if the query term specified by name has multiple values.</returns>
        public Boolean HasMultipleValues(String name)
        {
            if ( _terms == null )
                return false;

            Object obj = _terms[ name ];
            if ( obj == null )
                return false;

            if ( obj is List<String> )
                return ( ( List<String> ) obj ).Count > 1;

            return false;
        }

        /// <summary>
        /// Gets the value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the value of the specified term, or null if not found.</returns>
        public String GetTerm( String name )
        {
            Object obj = null;
            if ( _terms == null )
                return null;
            try
            {
                obj = _terms[name];
            }
            catch (Exception)
            {
                
            }
            if ( obj == null )
                return null;

            if ( obj is List<String> )
            {
                IEnumerator<String> i = ( ( List<String> ) obj ).GetEnumerator();
                i.MoveNext();
                String s = i.Current;

                if ( i.MoveNext() )
                    throw new Exception( "term has multiple values" );
                return s;
            }
            return ( String ) obj;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="defaultValue"></param>
        /// <returns>the value of the specified term, or defaultValue if not found.</returns>
        public String GetTerm( String name, String defaultValue )
        {
            String value = GetTerm( name );
            if ( value == null )
                return defaultValue;
            return value;
        }

        /// <summary>
        /// Gets the boolean value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the boolean value, or null if not found.</returns>
        /// <see cref="GetTerm(String)"/>
        public bool? GetBooleanTerm( String name )
        {
            String s = GetTerm( name );
            if ( s == null )
                return null;

            // modeling java behavior
            return s.Equals( "true", StringComparison.CurrentCultureIgnoreCase )? true : false;
        }

        public Boolean GetBooleanTerm( String name, Boolean defaultValue )
        {
            String s = GetTerm( name );
            if ( s == null )
                return defaultValue;

            return ( s.Equals( "true", StringComparison.CurrentCultureIgnoreCase ) )? true : false;
        }

        /// <summary>
        /// Gets the integer value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the integer value, or null if not found.</returns>
        /// 
        public int? GetIntegerTerm( String name )
        {
            String s = GetTerm( name );
            if ( s == null )
                return null;
            return Convert.ToInt32( s );
        }

        /// <summary>
        /// Gets the integer value of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="defaultValue">the value to return if the term is not found.</param>
        /// <returns>the integer value, or defaultValue if not found.</returns>
        /// <see cref="GetTerm( String )"/>
        /// 
        public int? GetIntegerTerm( String name, int defaultValue )
        {
            int? i = GetIntegerTerm( name );
            if ( i == null )
                return defaultValue;
            return i;
        }

        public double? GetDoubleTerm( String name )
        {
            String s = GetTerm( name );
            if ( s == null )
                return null;

            return Convert.ToDouble( s );
        }

        /// <summary>
        /// Gets the values of the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>an iterator over the string values of the query term. May be empty.</returns>
        public IEnumerator<String> GetTerms( String name )
        {
            if ( _terms == null )
                return null;
            Object obj = _terms[ name ];
            if ( obj == null )
                return new EmptyIterator<String>();

            if ( obj is List<String> )
                return ( ( List<String> ) obj ).GetEnumerator();

            return new SingleIterator<String>( ( String ) obj );
        }

        /// <summary>
        /// Gets the names of the query terms.
        /// </summary>
        /// <returns>an iterator over the string names.</returns>
        public IEnumerator<String> GetTermNames()
        {
            if ( _terms == null )
                return new EmptyIterator<String>();

            return _terms.Keys.GetEnumerator();
        }

        /// <summary>
        /// Adds the specified query term to the set of query terms. Duplicate
        /// name/value pairs are suppressed.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void AddTerm( String name, String value )
        {
            EnsureTerms();
            Object obj;

            try
            {
                obj = _terms[ name ];
            }
            catch ( KeyNotFoundException )
            {
                obj = null;
            }

            if ( obj == null )
            {
                _terms.Add( name, value );
                return;
            }

            if ( obj is List<String> )
            {
                ( ( List<String> ) obj ).Add( value );
                return;
            }

            List<String> values = new List<string>();
            values.Add( ( String ) obj );
            values.Add( value );
            _terms[ name ] = values;
        }

        /// <summary>
        /// Adds the specified query term to the set of query terms. Duplicate
        /// name/value pairs are suppressed.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void AddTerm( String name, int? value )
        {
            AddTerm( name, value.ToString() );
        }

        /// <summary>
        /// Adds the specified query term to the set of query terms. Duplicate
        /// name/value pairs are suppressed.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void AddTerm( String name, double? value )
        {
            AddTerm( name, value.ToString() );
        }

        /// <summary>
        /// Removes all the values associated with the specified query term.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>true if something was removed, false otherwise.</returns>
        public Boolean RemoveTerm( String name )
        {
            return _terms.Remove( name );
        }

        /// <summary>
        /// Removes the specified name/value pair from the set of query terms.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns>true if the name/value pair was found and removed.</returns>
        /// 
        public Boolean RemoveTerm( String name, String value )
        {
            if ( _terms == null )
                return false;

            Object obj = _terms[ name ];
            if ( obj == null )
                return false;

            if ( obj is List<String> )
            {
                List<String> x = ( List<String> ) obj;
                Boolean ok = x.Remove( value );
                if ( x.Count == 1 )
                {
                    IEnumerator<String> it = x.GetEnumerator();
                    it.MoveNext();
                    _terms[ name ] = it.Current;
                }

                return ok;
            }
            _terms.Remove( name );
            return true;
        }

        /// <summary>
        /// Removes all query terms
        /// </summary>
        public void ClearTerms()
        {
            if ( _terms != null )
                _terms.Clear();
        }

        private void EnsureTerms()
        {
            if ( _terms == null )
                _terms = new Dictionary<String, Object>();
        }


        #endregion

        #region FRAGMENT

        public String Fragment
        {
            get
            {
                return _fragment;
            }
            set
            {
                CheckNotBlank( "fragment", value );
                _fragment = value;
            }
        }

        #endregion

        #region UTIL

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();

            Escape( sb, _scheme );
            sb.Append( ':' );

            if ( _host != null )
            {
                sb.Append( "//" );
                if ( _user != null )
                {
                    Escape( sb, _user );
                    if ( _password != null )
                    {
                        sb.Append( ':' );
                        Escape( sb, _password );
                    }
                    sb.Append( '@' );
                }
                Escape( sb, _host );
                if ( _port != 0 )
                {
                    sb.Append( ':' );
                    sb.Append( _port );
                }
                sb.Append( '/' );
            }

            if ( _uri != null )
                Escape( sb, _uri );

            if ( HasParams() )
                ParamsToString( sb );

            if ( HasTerms() )
                TermsToString( sb );

            if ( _fragment != null )
            {
                sb.Append( '#' );
                Escape( sb, _fragment );
            }
            return sb.ToString();
        }

        public override int GetHashCode()
        {
            int code = 23547853;
		    code ^= hc( _scheme );
		    code ^= hc( _user );
		    code ^= hc( _password );
		    code ^= hc( _host );
		    code ^= hc( _port );
		    code ^= hc( _uri );
		    code ^= hc( _params );
		    code ^= hc( _terms );
		    code ^= hc( _fragment );
		    return code;
        }

        private int hc( Dictionary<String,Object> m )
	    {
		    return m != null ? m.GetHashCode() : 793;
	    }

	    private int hc( List<String> s )
	    {
		    return s != null ? s.GetHashCode() : 161;
	    }

	    private int hc( int? i )
	    {
		    return i != null ? i.GetHashCode() : 59;
	    }

	    private int hc( String s )
	    {
		    return s != null ? s.GetHashCode() : 91;
	    }


        public override bool Equals( object obj )
        {
            if ( obj == this )
                return true;

            if ( !( obj is URL ) )
                return false;

            URL other = ( URL ) obj;

            if ( !_scheme.Equals( other.Scheme, StringComparison.CurrentCultureIgnoreCase ) )
                return false;

            if ( !_user.Equals( other.User ) )
                return false;

            if ( !_password.Equals( other.Password ) )
                return false;

            if ( !_host.Equals( other.Host ) )
                return false;

            if ( _port != other.Port )
                return false;

            if (!_uri.Equals(other._uri))
                return false;

            if ( !CompareParams( _params, other._params ) )
                return false;

            if ( !CompareTerms( _terms, other._terms ) )
                return false;

            if ( !_fragment.Equals( other.Fragment ) )
                return false;

            return true;
        }

        private bool CompareParams( List<String> a, List<String> b )
        {
            if ( a == b )
                return true;

            int na = a != null ? a.Count : 0;
            int nb = b != null ? b.Count : 0;

            

            if (na == 0 || nb == 0)
                return na == 0 && nb == 0;

            return a.Equals( b );
        }

        private bool CompareTerms( Dictionary<String, Object> a, Dictionary<String, Object> b )
        {
            if ( a == b )
                return true;

            int na = a != null ? a.Count : 0;
            int nb = b != null ? b.Count : 0;

            if ( na == 0 || nb == 0 )
                return na == 0 && nb == 0;

            return a.Equals( b );
        }

        private void ParamsToString( StringBuilder sb )
        {
            IEnumerator<String> it = GetParams();
            while ( it.MoveNext() )
            {
                String param = it.Current;
                sb.Append( ';' );
                Escape( sb, param );
            }
        }

        private void TermsToString( StringBuilder sb )
        {
            char sep = '?';
            IEnumerator<String> it = GetTermNames();
            while ( it.MoveNext() )
            {
                String name = it.Current;
                IEnumerator<String> jt = GetTerms( name );
                while ( jt.MoveNext() )
                {
                    String value = jt.Current;
                    sb.Append( sep );
                    Escape( sb, name );
                    sb.Append( '=' );
                    Escape( sb, value );
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
		    Console.WriteLine( "scheme = "+_scheme );
		    Console.WriteLine( "user = "+_user );
		    Console.WriteLine( "password = "+_password );
		    Console.WriteLine( "host = "+_host );
		    Console.WriteLine( "port = "+_port );
		    Console.WriteLine( "uri = "+_uri );
		    Console.WriteLine( "params = "+_params );
		    Console.WriteLine( "terms = "+_terms );
		    Console.WriteLine( "fragment = "+_fragment );
	    }

        private void Escape( StringBuilder sb, String s )
        {
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

        private static Boolean IsEscaped( char c )
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

        private String Unescape( String s )
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

        private static void CheckNotBlank( String name, String value )
        {
            if ( value != null && value.Length == 0 )
                throw new ArgumentException( name + " is blank" );
        }

        private static String ValueToString( double value )
        {
            return Convert.ToString( value );
        }

        private static String ValueToString( int value )
        {
            return Convert.ToString( value );
        }

        private static List<String> CopyParams( List<String> paramss )
        {
            if ( paramss == null )
                return null;
            return new List<String>( paramss );
        }

        private static Dictionary<String, Object> CopyTerms( Dictionary<String, Object> terms )
        {
            if ( terms == null )
                return null;

            Dictionary<String, Object> map = new Dictionary<string, object>( terms );
            IEnumerator<KeyValuePair<String, Object>> i = map.GetEnumerator();
            while ( i.MoveNext() )
            {
                KeyValuePair<String, Object> me = i.Current;
                Object obj = me.Value;
                if ( obj is List<String> )
                    map[ me.Key ] = new List<String>( ( List<String> ) obj );
            }
            return map;
        }

        #endregion
    }
}
