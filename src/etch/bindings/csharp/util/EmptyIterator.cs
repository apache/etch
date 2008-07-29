using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Util
{
    public class EmptyIterator<E> : IEnumerator<E>
    {

        #region IEnumerator<E> Members

        public E Current
        {
            get { throw new Exception( "No such element" ); }
        }

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            throw new Exception( "Invalid operation" );
        }

        #endregion

        #region IEnumerator Members

        object System.Collections.IEnumerator.Current
        {
            get { throw new Exception( "No such element" ); }
        }

        public bool MoveNext()
        {
            return false;
        }

        public void Reset()
        {
            throw new Exception( "Invalid operation" );
        }

        #endregion
    }
}
