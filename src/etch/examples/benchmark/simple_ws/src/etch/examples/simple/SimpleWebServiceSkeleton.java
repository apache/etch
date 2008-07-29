
/**
 * SimpleWebServiceSkeleton.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
 */
package etch.examples.simple;

/**
 *  SimpleWebServiceSkeleton java skeleton for the axisService
 */
public class SimpleWebServiceSkeleton implements SimpleWebServiceSkeletonInterface
{
	/**
	 * Auto generated method signature
     * @param add0
     */
	public etch.examples.simple.xsd.AddResponse add(etch.examples.simple.xsd.Add add0)
    {
		etch.examples.simple.xsd.AddResponse ar = new etch.examples.simple.xsd.AddResponse();
		ar.set_return(add0.getX() + add0.getY());
		return ar;
    }
              
	/**
	 * Auto generated method signature
	 * @param sum2
     */
    public etch.examples.simple.xsd.SumResponse sum(etch.examples.simple.xsd.Sum sum2)
    {
    	etch.examples.simple.xsd.SumResponse sr = new etch.examples.simple.xsd.SumResponse();
    	
    	int sum = 0;
    	int[] vals = sum2.getVals();
    	int len = vals.length;
    	for (int i=0; i<len; i++)
    		sum += vals[i];
    	
    	return sr;
    }
              
	/**
	 * Auto generated method signature
	 * @param echoVoid4
	 */
    public void echoVoid(etch.examples.simple.xsd.EchoVoid echoVoid4)
    {
    	// no-op
    }     
}
    