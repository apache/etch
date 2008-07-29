// This file automatically generated by:
//   Etch Version 0.95/csharp backend 0.95
//   Wed Aug 15 18:39:49 CDT 2007

namespace etch.examples.ccmuser
{


using System;
using Etch.Msg;
using Etch.Transport;
using Etch.Support;


///
///<summary> Message to call translator for CcmUserServer. </summary>
///
public class StubCcmUserServer : StubCcmUser
{
	///
	/// <summary>Stub for CcmUserServer.</summary>
	/// <param name="obj"> the implementation of CcmUserServer responsive to requests.</param>
	/// <param name="pool"> thread pool used to run AsyncReceiverMode.POOL methods.</param>
	/// <param name="free"> thread pool used to run AsyncReceiverMode.FREE methods..</param>
	///
	public StubCcmUserServer( CcmUserServer obj, Pool pool, Pool free ) : base( obj, pool, free )
	{	
		this._obj = obj;
	}
	
	private CcmUserServer _obj;

	public override bool Message( MessageSource _src, Who _sender, Message _msg ) 
	{
		if (_msg == null)
		{
			base.Message( _src, _sender, _msg );
			return true;
		}

		switch (_msg.GetXType.Id)
		{
			case -267962576: // ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer_login
				do_login( _src, _sender, _msg );
				return true;
			case 1274147843: // ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer_logout
				do_logout( _src, _sender, _msg );
				return true;
			case 1580300234: // ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer_getUserConfig
				do_getUserConfig( _src, _sender, _msg );
				return true;
			case -1839976634: // ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer_setSpeeddialLabel
				do_setSpeeddialLabel( _src, _sender, _msg );
				return true;
			case -1424516809: // ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer_setSpeeddialNumber
				do_setSpeeddialNumber( _src, _sender, _msg );
				return true;
			default:
				return base.Message( _src, _sender, _msg );
		}
	}

	///
	/// <param name="_src"></param>
	/// <param name="_sender"></param>
	/// <param name="_msg"></param>
	/// Exception:
	///		throws Exception	
	///
	void do_login( MessageSource _src, Who _sender, Message _msg )
	{
		try
		{
			string serverIP =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_serverIP )
				;
			string username =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_username )
				;
			string password =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_password )
				;

			_obj.login(
				serverIP
				, username
				, password
			);

			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_login );
			_src.Message( _sender, _rmsg );
		}
		catch ( Exception e )
		{
			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_login );
			_rmsg.Add( ValueFactoryCcmUser._mf_result, e );
			_src.Message( _sender, _rmsg );
		}
	}

	///
	/// <param name="_src"></param>
	/// <param name="_sender"></param>
	/// <param name="_msg"></param>
	/// Exception:
	///		throws Exception	
	///
	void do_logout( MessageSource _src, Who _sender, Message _msg )
	{
		try
		{

			_obj.logout(
			);

			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_logout );
			_src.Message( _sender, _rmsg );
		}
		catch ( Exception e )
		{
			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_logout );
			_rmsg.Add( ValueFactoryCcmUser._mf_result, e );
			_src.Message( _sender, _rmsg );
		}
	}

	///
	/// <param name="_src"></param>
	/// <param name="_sender"></param>
	/// <param name="_msg"></param>
	/// Exception:
	///		throws Exception	
	///
	void do_getUserConfig( MessageSource _src, Who _sender, Message _msg )
	{
		try
		{
			string user =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_user )
				;

			Object _result =
			_obj.getUserConfig(
				user
			);

			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_getUserConfig );
			_rmsg.Add( ValueFactoryCcmUser._mf_result, _result );
			_src.Message( _sender, _rmsg );
		}
		catch ( Exception e )
		{
			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_getUserConfig );
			_rmsg.Add( ValueFactoryCcmUser._mf_result, e );
			_src.Message( _sender, _rmsg );
		}
	}

	///
	/// <param name="_src"></param>
	/// <param name="_sender"></param>
	/// <param name="_msg"></param>
	/// Exception:
	///		throws Exception	
	///
	void do_setSpeeddialLabel( MessageSource _src, Who _sender, Message _msg )
	{
		try
		{
			string pkid =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_pkid )
				;
			string newLabel =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_newLabel )
				;

			_obj.setSpeeddialLabel(
				pkid
				, newLabel
			);

			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_setSpeeddialLabel );
			_src.Message( _sender, _rmsg );
		}
		catch ( Exception e )
		{
			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_setSpeeddialLabel );
			_rmsg.Add( ValueFactoryCcmUser._mf_result, e );
			_src.Message( _sender, _rmsg );
		}
	}

	///
	/// <param name="_src"></param>
	/// <param name="_sender"></param>
	/// <param name="_msg"></param>
	/// Exception:
	///		throws Exception	
	///
	void do_setSpeeddialNumber( MessageSource _src, Who _sender, Message _msg )
	{
		try
		{
			string pkid =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_pkid )
				;
			string newNumber =
				(string)
					_msg.Get( ValueFactoryCcmUser._mf_newNumber )
				;

			_obj.setSpeeddialNumber(
				pkid
				, newNumber
			);

			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_setSpeeddialNumber );
			_src.Message( _sender, _rmsg );
		}
		catch ( Exception e )
		{
			Message _rmsg = _msg.Reply( ValueFactoryCcmUser._mt_etch_examples_ccmuser_CcmUserServer__result_setSpeeddialNumber );
			_rmsg.Add( ValueFactoryCcmUser._mf_result, e );
			_src.Message( _sender, _rmsg );
		}
	}
}

}