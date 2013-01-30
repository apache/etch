// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;

namespace etch.tests
{
    public class MyValueFactoryCuae : DefaultValueFactory
    {

        private readonly static TypeMap types = new TypeMap();
    
     

	    private readonly static Class2TypeMap class2type = 
                new Class2TypeMap();
	
	
	
	public MyValueFactoryCuae() : base("none:",types,class2type)
	{
			
	}
	static MyValueFactoryCuae()
	{
		DefaultValueFactory.Init( types, class2type );

        _mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit.SetAsyncMode(AsyncMode.QUEUED);
		_mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit.SetResult( _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit );
        _mt_etch_bindings_csharp_examples_cuae_CuaeClient_doit2.SetAsyncMode(AsyncMode.FREE);
		_mt_etch_bindings_csharp_examples_cuae_CuaeClient_doit2.SetResult( _mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2 );
        _mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit3.SetResult(_mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit3);



	//	class2type.Add( typeof( etch.bindings.csharp.examples.cuae.ConstsCuae.Request ), _mt_etch_bindings_csharp_examples_cuae_Cuae_Request );
	//	_mt_etch_bindings_csharp_examples_cuae_Cuae_Request.SetClass( typeof( etch.bindings.csharp.examples.cuae.ConstsCuae.Request ) );
		_mt_etch_bindings_csharp_examples_cuae_Cuae_Request.SetImportExportHelper
			( 
				new ImportExportHelper( 
					
					delegate( ValueFactory vf, Object value ) 
					{
						return (StructValue)value;
					}, 
					delegate( StructValue sv ) 
					{
                        return sv;
					} 
				)
			);


	//	class2type.Add( typeof( etch.bindings.csharp.examples.cuae.ConstsCuae.Response ), _mt_etch_bindings_csharp_examples_cuae_Cuae_Response );
	// 	_mt_etch_bindings_csharp_examples_cuae_Cuae_Response.SetClass( typeof( etch.bindings.csharp.examples.cuae.ConstsCuae.Response ) );
		_mt_etch_bindings_csharp_examples_cuae_Cuae_Response.SetImportExportHelper
			( 
				new ImportExportHelper(
                    delegate(ValueFactory vf, Object value)
                    {
                        return (StructValue)value;
                    },
                    delegate(StructValue sv)
                    {
                        return sv;
                    } 
				)
			);

        _mt_etch_bindings_csharp_examples_cuae_Cuae_ReqWithMessage.SetSuperType(_mt_etch_bindings_csharp_examples_cuae_Cuae_Request);

        _mt_etch_bindings_csharp_examples_cuae_Cuae_ReqWithMessage.SetImportExportHelper
            (
                new ImportExportHelper(
                    delegate(ValueFactory vf, Object value)
                    {
                        return (StructValue)value;
                    },
                    delegate(StructValue sv)
                    {
                        return sv;
                    }
                )
            );

        _mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode.SetSuperType(_mt_etch_bindings_csharp_examples_cuae_Cuae_Response);
        _mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode.SetImportExportHelper
    (
        new ImportExportHelper(
            delegate(ValueFactory vf, Object value)
            {
                return (StructValue)value;
            },
            delegate(StructValue sv)
            {
                return sv;
            }
        )
    );

		// initialize the extern serializers:

		// done updating types and fields, lock them.
		types.Lock();
		
		class2type.Lock();


		_mt_etch_bindings_csharp_examples_cuae_Cuae_Request.PutValidator( _mf_code, Validator_int.Get( 0 ) );

        _mt_etch_bindings_csharp_examples_cuae_Cuae_ReqWithMessage.PutValidator(_mf_code, Validator_int.Get(0));
        _mt_etch_bindings_csharp_examples_cuae_Cuae_ReqWithMessage.PutValidator(_mf_msg, Validator_string.Get(0));


		_mt_etch_bindings_csharp_examples_cuae_Cuae_Response.PutValidator( _mf_msg, Validator_string.Get( 0 ) );

        _mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode.PutValidator(_mf_msg, Validator_string.Get(0));
        _mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode.PutValidator(_mf_code, Validator_int.Get(0));



		// params for doit
        _mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit.PutValidator(_mf_req, Validator_StructValue.Get(_mt_etch_bindings_csharp_examples_cuae_Cuae_Request, 0));
		_mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );



		// params for _result_doit
        _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit.PutValidator(_mf_result, Validator_StructValue.Get(_mt_etch_bindings_csharp_examples_cuae_Cuae_Response, 0));
		_mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );
		_mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit.PutValidator( _mf_result, Validator_RuntimeException.Get() ); // thrown RuntimeException
		_mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit.PutValidator( _mf__inReplyTo, Validator_long.Get( 0 ) );



		// params for doit2
        _mt_etch_bindings_csharp_examples_cuae_CuaeClient_doit2.PutValidator(_mf_req, Validator_StructValue.Get(_mt_etch_bindings_csharp_examples_cuae_Cuae_Request, 0));
		_mt_etch_bindings_csharp_examples_cuae_CuaeClient_doit2.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );



		// params for _result_doit2
        _mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2.PutValidator(_mf_result, Validator_StructValue.Get(_mt_etch_bindings_csharp_examples_cuae_Cuae_Response, 0));
		_mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );
		_mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2.PutValidator( _mf_result, Validator_RuntimeException.Get() ); // thrown RuntimeException
		_mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2.PutValidator( _mf__inReplyTo, Validator_long.Get( 0 ) );

        // params for doit3
        _mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit3.PutValidator(_mf_req, Validator_StructValue.Get(_mt_etch_bindings_csharp_examples_cuae_Cuae_Request, 1));
        _mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit3.PutValidator(_mf__messageId, Validator_long.Get(0));



        // params for _result_doit3
        _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit3.PutValidator(_mf_result,Validator_StructValue.Get(_mt_etch_bindings_csharp_examples_cuae_Cuae_Response, 1) );
        _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit3.PutValidator(_mf__messageId, Validator_long.Get(0));
        _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit3.PutValidator(_mf_result, Validator_RuntimeException.Get()); // thrown RuntimeException
        _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit3.PutValidator(_mf__inReplyTo, Validator_long.Get(0));

        _mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2.Timeout = 500;
        _mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2.ResponseField = _mf_result;


	
	} // end of static constructor

	
	
	/// Type for etch.bindings.csharp.examples.cuae.Cuae.Request 
	public static XType _mt_etch_bindings_csharp_examples_cuae_Cuae_Request =  types.Get( "etch.bindings.csharp.examples.cuae.Cuae.Request" );

    /// Type for etch.bindings.csharp.examples.cuae.Cuae.ReqWithMessage 
    public static XType _mt_etch_bindings_csharp_examples_cuae_Cuae_ReqWithMessage = types.Get("etch.bindings.csharp.examples.cuae.Cuae.ReqWithMessage");

	/// Type for etch.bindings.csharp.examples.cuae.Cuae.Response 
	public static XType _mt_etch_bindings_csharp_examples_cuae_Cuae_Response =  types.Get( "etch.bindings.csharp.examples.cuae.Cuae.Response" );

    /// Type for etch.bindings.csharp.examples.cuae.Cuae.RespWithCode 
    public static XType _mt_etch_bindings_csharp_examples_cuae_Cuae_RespWithCode = types.Get("etch.bindings.csharp.examples.cuae.Cuae.RespWithCode");

	/// Type for etch.bindings.csharp.examples.cuae.CuaeServer.doit 
	public static XType _mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit =  types.Get( "etch.bindings.csharp.examples.cuae.CuaeServer.doit" );

	/// Type for etch.bindings.csharp.examples.cuae.CuaeClient._result_doit 
	public static XType _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit =  types.Get( "etch.bindings.csharp.examples.cuae.CuaeClient._result_doit" );

	/// Type for etch.bindings.csharp.examples.cuae.CuaeClient.doit2 
	public static XType _mt_etch_bindings_csharp_examples_cuae_CuaeClient_doit2 =  types.Get( "etch.bindings.csharp.examples.cuae.CuaeClient.doit2" );

	/// Type for etch.bindings.csharp.examples.cuae.CuaeServer._result_doit2 
	public static XType _mt_etch_bindings_csharp_examples_cuae_CuaeServer__result_doit2 =  types.Get( "etch.bindings.csharp.examples.cuae.CuaeServer._result_doit2" );

    /// Type for etch.bindings.csharp.examples.cuae.CuaeServer.doit3 
    public static XType _mt_etch_bindings_csharp_examples_cuae_CuaeServer_doit3 = types.Get("etch.bindings.csharp.examples.cuae.CuaeServer.doit3");

    /// Type for etch.bindings.csharp.examples.cuae.CuaeClient._result_doit3 
    public static XType _mt_etch_bindings_csharp_examples_cuae_CuaeClient__result_doit3 = types.Get("etch.bindings.csharp.examples.cuae.CuaeClient._result_doit3");

	
	

	
	
				public static Field _mf_code = new Field( "code" );
		
				public static new Field _mf_msg = new Field( "msg" );
		
				public static Field _mf_req = new Field( "req" );
		
				public static new Field _mf_result = new Field( "result" );
    }
}
