package etch.examples.simple;

import java.io.IOException;
import java.rmi.RemoteException;

import etch.examples.simple.SimpleWebServiceSimpleWebServiceSOAP12PortStub.*;


public class ClientMain 
{
	private static void testVoid(SimpleWebServiceSimpleWebServiceSOAP12PortStub stub, int n, int iteration) throws RemoteException
	{
		long t0 = System.nanoTime();

		EchoVoid e = new EchoVoid();
		for (int i = 0; i < n; i++)
		{
			e.setStr("etch");
			stub.echoVoid(e);
		}
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		
		System.out.println("testVoid - iteration - " + iteration + " - time - " +  t + " - count - " +  n + " - rate - " +  r);
	}
	
	private static void testAdd(SimpleWebServiceSimpleWebServiceSOAP12PortStub stub, int n, int iteration) throws RemoteException
	{
		long t0 = System.nanoTime();

		Add a = new Add();
		a.setX(1);
		a.setY(1);                		
		for (int i = 0; i < n; i++)
		{
			stub.add(a);
		}
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		
		System.out.println("testAdd - iteration - " + iteration + " - time - " +  t + " - count - " +  n + " - rate - " +  r);
	}

	private static void testSum(SimpleWebServiceSimpleWebServiceSOAP12PortStub stub, int n, int iteration, int size) throws RemoteException
	{
		long t0 = System.nanoTime();

		Sum a = new Sum();
		int[] vals = new int[size];
		for (int i=0; i<size; i++)
			vals[i] = i;
		a.setVals(vals);
		for (int i = 0; i < n; i++)
		{
			stub.sum(a);
		}
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		
		System.out.println("testSum - size - " + size + " - iteration - " + iteration + " - time - " +  t + " - count - " +  n + " - rate - " +  r);
	}
	
	/**
	 * @param args
	 * @throws IOException 
	 * @throws InterruptedException 
	 */
	public static void main(String[] args) throws IOException, InterruptedException 
	{		
		String endPoint = "http://127.0.0.1:8080/axis2/services/SimpleWebService";
		SimpleWebServiceSimpleWebServiceSOAP12PortStub stub = new SimpleWebServiceSimpleWebServiceSOAP12PortStub(endPoint);

		int n = 6460; 
		for (int i = 0; i < 5; i++)		
		{
			testVoid( stub, n, i );
			Thread.sleep(60000);
		}
		
		n = 5420; 
		for (int i = 0; i < 5; i++)
		{
			testAdd( stub, n, i );
			Thread.sleep(60000);			
		}

		n = 1020; 
		for (int i = 0; i < 5; i++)
			testSum( stub, n, i, 4000);
		
        System.out.println("Press enter to quit.");
        System.in.read();
        System.out.println("Exiting.");		
	}
}
