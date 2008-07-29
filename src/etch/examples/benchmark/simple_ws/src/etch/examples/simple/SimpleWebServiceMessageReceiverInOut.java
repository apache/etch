
        /**
        * SimpleWebServiceMessageReceiverInOut.java
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
        */
        package etch.examples.simple;

        /**
        *  SimpleWebServiceMessageReceiverInOut message receiver
        */

        public class SimpleWebServiceMessageReceiverInOut extends org.apache.axis2.receivers.AbstractInOutSyncMessageReceiver{


        public void invokeBusinessLogic(org.apache.axis2.context.MessageContext msgContext, org.apache.axis2.context.MessageContext newMsgContext)
        throws org.apache.axis2.AxisFault{

        try {

        // get the implementation class for the Web Service
        Object obj = getTheImplementationObject(msgContext);

        SimpleWebServiceSkeletonInterface skel = (SimpleWebServiceSkeletonInterface)obj;
        //Out Envelop
        org.apache.axiom.soap.SOAPEnvelope envelope = null;
        //Find the axisOperation that has been set by the Dispatch phase.
        org.apache.axis2.description.AxisOperation op = msgContext.getOperationContext().getAxisOperation();
        if (op == null) {
        throw new org.apache.axis2.AxisFault("Operation is not located, if this is doclit style the SOAP-ACTION should specified via the SOAP Action to use the RawXMLProvider");
        }

        java.lang.String methodName;
        if(op.getName() != null & (methodName = org.apache.axis2.util.JavaUtils.xmlNameToJava(op.getName().getLocalPart())) != null){

        

            if("add".equals(methodName)){
                
                etch.examples.simple.xsd.AddResponse addResponse6 = null;
                        etch.examples.simple.xsd.Add wrappedParam =
                                                             (etch.examples.simple.xsd.Add)fromOM(
                                    msgContext.getEnvelope().getBody().getFirstElement(),
                                    etch.examples.simple.xsd.Add.class,
                                    getEnvelopeNamespaces(msgContext.getEnvelope()));
                                                
                                               addResponse6 =
                                                   
                                                   
                                                         skel.add(wrappedParam)
                                                    ;
                                            
                                        envelope = toEnvelope(getSOAPFactory(msgContext), addResponse6, false);
                                    } else 

            if("sum".equals(methodName)){
                
                etch.examples.simple.xsd.SumResponse sumResponse8 = null;
                        etch.examples.simple.xsd.Sum wrappedParam =
                                                             (etch.examples.simple.xsd.Sum)fromOM(
                                    msgContext.getEnvelope().getBody().getFirstElement(),
                                    etch.examples.simple.xsd.Sum.class,
                                    getEnvelopeNamespaces(msgContext.getEnvelope()));
                                                
                                               sumResponse8 =
                                                   
                                                   
                                                         skel.sum(wrappedParam)
                                                    ;
                                            
                                        envelope = toEnvelope(getSOAPFactory(msgContext), sumResponse8, false);
                                    
            } else {
              throw new RuntimeException("method not found");
            }
        

        newMsgContext.setEnvelope(envelope);
        }
        }
        catch (Exception e) {
        throw org.apache.axis2.AxisFault.makeFault(e);
        }
        }
        
        //
            private  org.apache.axiom.om.OMElement  toOM(etch.examples.simple.xsd.EchoVoid param, boolean optimizeContent){
            
                     return param.getOMElement(etch.examples.simple.xsd.EchoVoid.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(etch.examples.simple.xsd.Sum param, boolean optimizeContent){
            
                     return param.getOMElement(etch.examples.simple.xsd.Sum.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(etch.examples.simple.xsd.SumResponse param, boolean optimizeContent){
            
                     return param.getOMElement(etch.examples.simple.xsd.SumResponse.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(etch.examples.simple.xsd.Add param, boolean optimizeContent){
            
                     return param.getOMElement(etch.examples.simple.xsd.Add.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(etch.examples.simple.xsd.AddResponse param, boolean optimizeContent){
            
                     return param.getOMElement(etch.examples.simple.xsd.AddResponse.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
                    private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, etch.examples.simple.xsd.SumResponse param, boolean optimizeContent){
                      org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                       
                                emptyEnvelope.getBody().addChild(param.getOMElement(etch.examples.simple.xsd.SumResponse.MY_QNAME,factory));
                            

                     return emptyEnvelope;
                    }
                    
                    private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, etch.examples.simple.xsd.AddResponse param, boolean optimizeContent){
                      org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                       
                                emptyEnvelope.getBody().addChild(param.getOMElement(etch.examples.simple.xsd.AddResponse.MY_QNAME,factory));
                            

                     return emptyEnvelope;
                    }
                    


        /**
        *  get the default envelope
        */
        private org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory){
        return factory.getDefaultEnvelope();
        }


        private  java.lang.Object fromOM(
        org.apache.axiom.om.OMElement param,
        java.lang.Class type,
        java.util.Map extraNamespaces){

        try {
        
                if (etch.examples.simple.xsd.EchoVoid.class.equals(type)){
                
                           return etch.examples.simple.xsd.EchoVoid.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (etch.examples.simple.xsd.Sum.class.equals(type)){
                
                           return etch.examples.simple.xsd.Sum.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (etch.examples.simple.xsd.SumResponse.class.equals(type)){
                
                           return etch.examples.simple.xsd.SumResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (etch.examples.simple.xsd.Add.class.equals(type)){
                
                           return etch.examples.simple.xsd.Add.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (etch.examples.simple.xsd.AddResponse.class.equals(type)){
                
                           return etch.examples.simple.xsd.AddResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
        } catch (Exception e) {
        throw new RuntimeException(e);
        }
           return null;
        }



    

        /**
        *  A utility method that copies the namepaces from the SOAPEnvelope
        */
        private java.util.Map getEnvelopeNamespaces(org.apache.axiom.soap.SOAPEnvelope env){
        java.util.Map returnMap = new java.util.HashMap();
        java.util.Iterator namespaceIterator = env.getAllDeclaredNamespaces();
        while (namespaceIterator.hasNext()) {
        org.apache.axiom.om.OMNamespace ns = (org.apache.axiom.om.OMNamespace) namespaceIterator.next();
        returnMap.put(ns.getPrefix(),ns.getNamespaceURI());
        }
        return returnMap;
        }

        private org.apache.axis2.AxisFault createAxisFault(java.lang.Exception e) {
        org.apache.axis2.AxisFault f;
        Throwable cause = e.getCause();
        if (cause != null) {
            f = new org.apache.axis2.AxisFault(e.getMessage(), cause);
        } else {
            f = new org.apache.axis2.AxisFault(e.getMessage());
        }

        return f;
    }

        }//end of class
    