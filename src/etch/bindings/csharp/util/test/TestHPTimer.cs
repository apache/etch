using System.Threading;
using NUnit.Framework;

namespace Etch.Util.Test
{
    [TestFixture]
    public class TestHPTimer
    {
        [Test]
        public void one()
        {
            System.Console.WriteLine("one starting");

            long x = HPTimer.Now();
            for (int i = 0; i < 10000; i++)
            {
                // assert that values from HPTimer.Now() are always monotonically increasing
                long y = HPTimer.Now();
                Assert.GreaterOrEqual(y, x);
                long diff = y - x;
                Assert.GreaterOrEqual(y, 0);
                int idiff = (int) diff;
                Assert.GreaterOrEqual(idiff, 0);
                x = y;
                Thread.Sleep(1);
            }

            System.Console.WriteLine("one done");
        }

        [Test]
        public void two()
        {
            System.Console.WriteLine("two starting");

            Thread t1 = new Thread(foo);
            Thread t2 = new Thread(foo);

            t1.Start();
            t2.Start();

            t1.Join();
            t2.Join();

            System.Console.WriteLine("two done");
        }

        public void foo()
        {
            long x = HPTimer.Now();
            for (int i = 0; i < 10000; i++)
            {
                // assert that values from HPTimer.Now() are always monotonically increasing
                long y = HPTimer.Now();
                Assert.GreaterOrEqual(y, x);
                long diff = y - x;
                Assert.GreaterOrEqual(y, 0);
                int idiff = (int)diff;
                Assert.GreaterOrEqual(idiff, 0);
                x = y;
                Thread.Sleep(1);
            }
        }
    }
}
