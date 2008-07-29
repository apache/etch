using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Text;

namespace BenchmarkService
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            Uri mexAddress = new Uri("http://localhost:8088/benchmark");
            ServiceHost serviceHost = new
                ServiceHost(typeof(BenchmarkService), mexAddress);
            serviceHost.AddServiceEndpoint(typeof(BenchmarkService),
                                        new BasicHttpBinding(),
                                        mexAddress);

            ServiceMetadataBehavior smb = serviceHost.Description.Behaviors.Find<ServiceMetadataBehavior>();
            if(smb != null)
            {
                smb.HttpGetEnabled = true;
                smb.HttpGetUrl = mexAddress;
            }
            else
            {
                smb = new ServiceMetadataBehavior();
                smb.HttpGetEnabled = true;
                smb.HttpGetUrl = mexAddress;
                serviceHost.Description.Behaviors.Add(smb);
            }
            
            serviceHost.Open();
            Console.WriteLine("Service at your service.");
            Console.ReadKey();
            serviceHost.Close();
        }
    }

    [ServiceContract]
    internal class BenchmarkService
    {
        [OperationContract]
        private int? add(int? x, int? y)
        {
            return x + y;
        }

        [OperationContract]
        private int? sum(int[] values)
        {
            int sum = 0;
            foreach(int value in values)
                sum += value;
            return sum;
        }

        [OperationContract]
        private void report(int? code, string msg)
        {
            // do nothing
        }
    }
}
