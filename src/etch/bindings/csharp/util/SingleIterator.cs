using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Util
{
    /// <summary>
    /// An iterator over a single object
    /// </summary>
    public class SingleIterator<E> : IEnumerator<E>
    {
        private E _obj;

        public SingleIterator( Object obj )
        {
            _obj = ( E ) obj;
        }

        #region IEnumerator<E> Members

        public E Current
        {
            get 
            {
                if ( _obj == null )
                    throw new Exception( "No Such element" );

                E o = _obj;
                _obj = default(E);
                return o;
            }
        }

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion

        #region IEnumerator Members

        object System.Collections.IEnumerator.Current
        {
            get
            {
                if ( _obj == null )
                    throw new Exception( "No Such element" );

                E o = _obj;
                _obj = default( E );
                return o;
            }
        }

        public bool MoveNext()
        {
            return _obj != null;
        }

        public void Reset()
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion
    }
}
