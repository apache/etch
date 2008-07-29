
    /**
     * SimpleWebServiceSimpleWebServiceSOAP11PortCallbackHandler.java
     *
     * This file was auto-generated from WSDL
     * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
     */
    package etch.examples.simple;

    /**
     *  SimpleWebServiceSimpleWebServiceSOAP11PortCallbackHandler Callback class, Users can extend this class and implement
     *  their own receiveResult and receiveError methods.
     */
    public abstract class SimpleWebServiceSimpleWebServiceSOAP11PortCallbackHandler{



    protected Object clientData;

    /**
    * User can pass in any object that needs to be accessed once the NonBlocking
    * Web service call is finished and appropriate method of this CallBack is called.
    * @param clientData Object mechanism by which the user can pass in user data
    * that will be avilable at the time this callback is called.
    */
    public SimpleWebServiceSimpleWebServiceSOAP11PortCallbackHandler(Object clientData){
        this.clientData = clientData;
    }

    /**
    * Please use this constructor if you don't want to set any clientData
    */
    public SimpleWebServiceSimpleWebServiceSOAP11PortCallbackHandler(){
        this.clientData = null;
    }

    /**
     * Get the client data
     */

     public Object getClientData() {
        return clientData;
     }

        
           /**
            * auto generated Axis2 call back method for add method
            * override this method for handling normal response from add operation
            */
           public void receiveResultadd(
                    etch.examples.simple.SimpleWebServiceSimpleWebServiceSOAP11PortStub.AddResponse result
                        ) {
           }

          /**
           * auto generated Axis2 Error handler
           * override this method for handling error response from add operation
           */
            public void receiveErroradd(java.lang.Exception e) {
            }
                
           /**
            * auto generated Axis2 call back method for sum method
            * override this method for handling normal response from sum operation
            */
           public void receiveResultsum(
                    etch.examples.simple.SimpleWebServiceSimpleWebServiceSOAP11PortStub.SumResponse result
                        ) {
           }

          /**
           * auto generated Axis2 Error handler
           * override this method for handling error response from sum operation
           */
            public void receiveErrorsum(java.lang.Exception e) {
            }
                
               // No methods generated for meps other than in-out
                


    }
    