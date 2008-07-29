using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Transport;
using System.Threading;

namespace Etch.Util.Test
{
    [TestFixture]
    public class TestCircularQueue
    {
        private static CircularQueue<int?> cq;
        [TestFixtureSetUp]
        public void SetUp()
        {
            cq = new CircularQueue<int?>( 2 );
        }

        public void ResetQueue()
        {
            cq = new CircularQueue<int?>( 2 );
        }

        [Test]
        public void Start()
        {   
            Assert.IsTrue( cq.Size() == 2 );
            Assert.IsTrue( cq.Count() == 0 );
            Assert.IsTrue( cq.IsEmpty() );
            Assert.IsTrue( !cq.IsFull() );
            Assert.IsTrue( cq.Get( -1 ) == null );
        }

        [Test]
        public void AddElement()
        {
            Assert.IsTrue( cq.Put( 1 ) );
            Assert.IsTrue( cq.Size() == 2 );
            Assert.IsTrue( cq.Count() == 1 );
            Assert.IsTrue( !cq.IsEmpty() );
            Assert.IsTrue( !cq.IsFull() );
        }

        [Test]
        public void GetElement()
        {
            Assert.IsTrue( cq.Get() == 1 );
            Assert.IsTrue( cq.Size() == 2 );
            Assert.IsTrue( cq.Count() == 0 );
            Assert.IsTrue( cq.IsEmpty() );
            Assert.IsTrue( !cq.IsFull() );
            Assert.IsTrue( cq.Get( -1 ) == null );
        }

        [Test]
        public void AddAfterRemoveElement()
        {
            Assert.IsTrue( cq.Put( 2 ) );
            Assert.IsTrue( cq.Size() == 2 );
            Assert.IsTrue( cq.Count() == 1 );
            Assert.IsTrue( !cq.IsEmpty() );
            Assert.IsTrue( !cq.IsFull() );
        }

        [Test]
        public void AddElementFullQueue()
        {
            Assert.IsTrue( cq.Put( 3 ) );
            Assert.IsTrue( cq.Size() == 2 );
            Assert.IsTrue( cq.Count() == 2 );
            Assert.IsTrue( !cq.IsEmpty() );
            Assert.IsTrue( cq.IsFull() );
        }

        long t0, t;

        [Test]
        public void NoDelayAdd()
        {
            t0 = HPTimer.Now();
            Assert.IsTrue( !cq.Put( 4, -1 ) );
            t = HPTimer.NsSince( t0 );
            // Assert.IsTrue( t == 0, "time %d > 0 nanos", t );
            Assert.IsTrue( cq.Size() == 2 );
            Assert.IsTrue( cq.Count() == 2 );
            Assert.IsTrue( !cq.IsEmpty() );
            Assert.IsTrue( cq.IsFull() );
        }

        [Test]
        public void DelayAdd()
        {
            t0 = HPTimer.Now();
            Assert.IsTrue( !cq.Put( 4, 50 ) );
            t = HPTimer.NsSince( t0 );
            // Assert.IsTrue( t >= 50, "time %d < 50 millis", t );
            Assert.IsTrue ( t >= 50 );
            Assert.IsTrue( cq.Size() == 2 );
            Assert.IsTrue( cq.Count() == 2 );
            Assert.IsTrue( !cq.IsEmpty() );
            Assert.IsTrue( cq.IsFull() );
        }

        [Test]
        public void CheckConsistency()
        {

            Assert.IsTrue( cq.Get() == 2 );
		    Assert.IsTrue( cq.Size() == 2 );
		    Assert.IsTrue( cq.Count() == 1 );
		    Assert.IsTrue( !cq.IsEmpty() );
		    Assert.IsTrue( !cq.IsFull() );

		    Assert.IsTrue( cq.Get() == 3 );
		    Assert.IsTrue( cq.Size() == 2 );
		    Assert.IsTrue( cq.Count() == 0 );
		    Assert.IsTrue( cq.IsEmpty() );
		    Assert.IsTrue( !cq.IsFull() );
        }

        //[Test]
        //public void TestGetNextAvailable()
        //{
        //    t0 = HPTimer.Now();
        //    Assert.IsTrue( cq.Get( -1 ) == null );
        //    t = HPTimer.NsSince( t0 );
        //    Console.WriteLine( t );
        //    Assert.IsTrue( t == 0 );

        //    t0 = HPTimer.Now();
        //    Assert.IsTrue( cq.Get( 50 ) == null );
        //    t = HPTimer.NsSince( t0 );
        //    Assert.IsTrue( t >= 50 );
        //}

        [Test]
        public void TestRoutineOperations()
        {
            Assert.IsTrue( cq.Put( 4 ) );
    		
		    Assert.IsTrue( cq.Put( 5 ) );
    		
		    Assert.IsTrue( cq.Get() == 4 );
    		
		    Assert.IsTrue( cq.Put( 6 ) );
    		
		    Assert.IsTrue( cq.Get() == 5 );
    		
		    Assert.IsTrue( cq.Put( 7 ) );
    		
		    Assert.IsTrue( cq.Get() == 6 );
    		
		    Assert.IsTrue( cq.Put( 8 ) );
    		
		    Assert.IsTrue( cq.Get() == 7 );
    		
		    cq.Close();
    		
		    Assert.IsTrue( cq.Get() == 8 );
    		
		    Assert.IsTrue( cq.Get() == null );
        }

        [Test]
        public void TestDelay_NoAdd()
        {
            ResetQueue();

            int? i = cq.Get( 4000 );

            // no element in queue, and timeout
            Assert.AreEqual( null, i );
        }
        
        [Test]
        public void TestDelay_Add()
        {
            ResetQueue();

            // start new thread to enter stuff into queue
            ( new Thread( new ThreadStart( NewThread ) ) ).Start();

            int? i = cq.Get( 4000 );

            Assert.AreEqual( 1, i );
        }

        /// <summary>
        /// Thread that puts in stuff into the queue after 
        /// certain time
        /// </summary>
        public void NewThread()
        {
            Thread.Sleep( 2000 );
            cq.Put( 1 );
        }
    }
}
