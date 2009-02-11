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
namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    //[TestFixture]
    //public class TestDefaultDeliveryService
    //{
    //    private DefaultDeliveryService<MyStub> svc;

    //    private MyStub ms;

    //    private readonly ValueFactory vf = new MyValueFactory();

    //    private Message msg;

    //    private MyDataSource ds = new MyDataSource();

    //    XType mt_add = new XType( "add" );
    //    XType mt_add_result = new XType( "add_result" );
    //    Field mf_x = new Field( "x" );
    //    Field mf_y = new Field( "y" );
    //    Field mf_result = new Field( "result" );

    //    public Message ConstructSimpleMessage()
    //    {   
    //        return new Message( MyValueFactory.mt_function, vf );
            
    //    }

    //    public Message ConstructAddMessage() 
    //    {

    //        msg = new Message( MyValueFactory.mt_add, vf );
    //        msg.Add( MyValueFactory.mf_x, 4 );
    //        msg.Add( MyValueFactory.mf_y, 5 );
    		
    //        return msg;
    //    }

    //    /// <summary>
    //    /// Setup method to be called before every test.
    //    /// </summary>
    //    [SetUp]
    //    public void Setup()
    //    {
    //        ms = new MyStub( null, null );

    //        DefaultDeliveryService<MyStub> dds = new MyDefaultDeliveryService( ms );

    //        svc = DefaultDeliveryService<MyStub>.SetUp( dds, ds, vf );
    //    }

    //    //[Test]
    //    //public void Up()
    //    //{
    //    //    Setup();
    //    //    svc.Up( new MailboxManager( svc, 0 ) );
    //    //    Assert.AreEqual( ms.what, What.MYSTUBCREATED );
    //    //}

    //    //[Test]
    //    //public void Down()
    //    //{
    //    //    Setup();
    //    //    svc.Up( new MailboxManager( svc, 0 ) );
    //    //    svc.Down( new MailboxManager( svc, 0 ) );
    //    //    Assert.AreEqual( ms.what, What.MYSTUBMESSAGE );
    //    //}

    //    [Test]
    //    public void ShutdownOutput()
    //    {
    //        svc.TransportControl( TransportConsts.STOP, null );
    //        Assert.AreEqual( ds.what, What.MYDATASOURCESHUTDOWNOUTPUT );
    //    }

    //    [Test]
    //    public void Message()
    //    {
    //        svc.SessionNotify( SessionConsts.UP );
    //        svc.Message( null, ConstructSimpleMessage() );
    //        Assert.AreEqual( ms.what, What.MYSTUBMESSAGE );
    //    }

    //    [Test]
    //    public void Send()
    //    {
    //        svc.Send( ConstructSimpleMessage() );
    //        Assert.AreEqual( ds.what, What.MYDATASOURCEDATA );
    //    }

    //    [Test]
    //    public void Call()
    //    {
    //        try
    //        {
    //            svc.EndCall( svc.BeginCall( ConstructAddMessage() ), mt_add_result,
    //                mf_result, 1 );
    //        }
    //        catch ( Exception )
    //        {
    //            Assert.AreEqual( ds.what, What.MYDATASOURCEDATA );
    //        }
    //    }

    //    public class MyDefaultDeliveryService : DefaultDeliveryService<MyStub>
    //    {
    //        private MyStub ms;

    //        public MyDefaultDeliveryService( MyStub ms )
    //        {
    //            this.ms = ms;
    //        }

    //        protected override MyStub NewStub( MailboxMessageSource src )
    //        {
    //            return ms;
    //        }

    //        public override string ToString()
    //        {
    //            return "MyDefaultDeliveryService testing";
    //        }
    //    }

    //    public enum What
    //    {
    //        MYSTUBCREATED,
    //        MYSTUBMESSAGE,
    //        MYDATASOURCESHUTDOWNOUTPUT,
    //        MYDATASOURCEDATA
    //    };

    //    public class MyDataSource : DataSource
    //    {
    //        public Enum what;

    //        #region DataSource Members

    //        public void Data( Who recipient, FlexBuffer buf )
    //        {
    //            what = What.MYDATASOURCEDATA;
    //        }

    //        #endregion


    //        #region Source Members

    //        public object GetHandler()
    //        {
    //            return null;
    //        }

    //        public void SetHandler( object handler )
    //        {
    //            // nothing to do. 
    //        }

    //        #endregion

    //        #region Transport Members

    //        public object TransportQuery( object query )
    //        {
    //            return null;
    //        }

    //        public void TransportControl( object control, object value )
    //        {
    //            if ( control.Equals( TransportConsts.STOP ) )
    //                what = What.MYDATASOURCESHUTDOWNOUTPUT;
    //        }

    //        public void TransportNotify( object eventObj )
    //        {
    //            // ignore
    //        }

    //        #endregion
    //    }

    //    public class MyStub : StubBase
    //    {
    //        public Enum what;

    //        public MyStub( MessageSource src, Object obj ) : base ( src, obj, null, null )
    //        {
    //            what = What.MYSTUBCREATED;
    //        }

    //        public override bool Message( Who sender, Message msg )
    //        {
    //            what = What.MYSTUBMESSAGE;
    //            return true;
    //        }
    //    }

    //    public class MyValueFactory : DefaultValueFactory
    //    {
    //        private readonly static TypeMap types = new TypeMap();

    //        private readonly static FieldMap fields = new FieldMap();

    //        private readonly static Class2TypeMap class2type = 
    //            new Class2TypeMap();

    //        public readonly static XType mt_function = types.Get( "function" );

    //        public readonly static XType mt_add = types.Get( "add" );

    //        public readonly static XType mt_add_result = types.Get( "add_result" );

    //        public readonly static Field mf_x = fields.Get( "x" );

    //        public readonly static Field mf_y = fields.Get( "y" );

    //        public readonly static Field mf_result = fields.Get( "xresult" );

    //        static MyValueFactory()
    //        {
    //            DefaultValueFactory.Init( types, fields, class2type );

    //            mt_function.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

    //            mt_add.PutValidator( mf_x, Validator_int.Get( 0 ) );
    //            mt_add.PutValidator( mf_y, Validator_int.Get( 0 ) );
    //            mt_add.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

    //            mt_add_result.PutValidator( mf_result, Validator_int.Get( 0 ) );
    //            mt_add_result.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
    //            mt_add_result.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );
    //        }

    //        public override XType GetType( int? id )
    //        {
    //            return types.Get( id );
    //        }

    //        public override XType GetType( string name )
    //        {
    //            return types.Get( name );
    //        }

    //        public override ICollection<XType> GetTypes()
    //        {
    //            return types.Values();
    //        }

    //        public override Field GetField( int? id )
    //        {
    //            return fields.Get( id );
    //        }

    //        public override Field GetField( string name )
    //        {
    //            return fields.Get( name );
    //        }

    //        public override ICollection<Field> GetFields()
    //        {
    //            return fields.Values();
    //        }

    //        public override StructValue ExportCustomValue( Object value )
    //        {
    //            return ExportCustomValue( this, class2type, value );
    //        }

    //        public override XType GetCustomStructType( Type c )
    //        {
    //            return class2type.Get( c );
    //        }
    //    }
    //}
}
