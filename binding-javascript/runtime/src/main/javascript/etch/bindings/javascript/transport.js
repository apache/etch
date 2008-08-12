// Etch Proof of Concept Transport Utility Class

// Instantiate Etch transport utility namespace with minimal conflicts to environment
new function()
{
	// creates namespace

	var namespace = "etch.poc";

	namespacer();

	function namespacer()
	{
		var nsBits = namespace.split(".");

		var base = window;

		for(var i = 0; i < nsBits.length; i++)
		{	
			base = createNamespace(base, nsBits[i]);		
		}	
	}

	function createNamespace(base, nsBit)
	{	

		// If no one has yet created this name, let's go ahead and create it for ourselves as an empty object.
		if(base[nsBit] == null)
		{
			base[nsBit] = {};
		}	

		// someone must have already made it--that's actually OK, as long as the already-constructed type
		// is an object.  Otherwise there isn't much we can do.
		else if(typeof base[nsBit]!= 'object')
		{
			alert("Namespace collision on " + nsBit);
		}
	
		return base[nsBit];
	}
}

etch.poc.TypeMap = { };

/* Transport Constructor
 * @param service The service
 */
etch.poc.Transport = function Transport(service)
{
	this.service = service;	
}

// The service being communicated with
etch.poc.Transport.prototype.service = null;

// The host as indicated in the init() function
etch.poc.Transport.prototype.host = null;

// Invoked only as a session is created between client and server
etch.poc.Transport.prototype.startUri = null;

// Invoked whenever a one-way or two-way, server-directed message is sent 
etch.poc.Transport.prototype.actionUri = null;

// Invoked repeatedly in a long-poll fashion for low latency events from server
etch.poc.Transport.prototype.eventUri = null;

// Invoked to send a result for a two-way, client-directed message
etch.poc.Transport.prototype.respondUri = null;

// Used to generate transactionIDs when client sends messages to server
etch.poc.Transport.prototype.transactionFactory = 0;

// Used to indicate the version of the wire format for the messaging
etch.poc.Transport.prototype.version = 1;

// Used to keep track of outstanding transactions.  
// Key = TransactionId, Value = Metadata about the transaction
etch.poc.Transport.prototype.pendingTransactions = {};

// Used to track the ID of the session sent in the respnose to the startUri
etch.poc.Transport.prototype.sessionId = null;

// Used to indicate whether the service is running
etch.poc.Transport.prototype.running = false;

// known sessionNotify messages
etch.poc.Transport.prototype.sessionUp = "UP";
etch.poc.Transport.prototype.sessionDown = "DOWN";

etch.poc.Transport.prototype.fixupData = function(key, value)
{
	// fix up date strings into Date.  
	// This is unabashedly taken directly from the comments of JSON2.js. 
	// TODO: expand to accept milliseconds
 	var a;
	if(value != null && typeof value === 'object')
	{
		// Attempt to look this up in our type map, 
		// and construct the proper Etch type out of the JSON object
		var typeField = value["@type"];

		//console.log("typeField: " + typeField);
		if(typeField !== undefined)
		{
			var typeConstructor = etch.poc.TypeMap[typeField];
			
			if(typeConstructor != null)
			{
				//console.log(typeConstructor);
				var wrappedType = new typeConstructor(value);
				//console.log("Type Wrapped: " + wrappedType);
				return wrappedType;
			}
			else
			{
				//console.log("No constructor defined for: " + typeField);
			}
		}
		else
		{
			//console.log("No @type in JSON object. Ignoring...");
		}
	}
	else if (typeof value === 'string') 
	{
		a = /^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2}(?:\.\d*)?)Z$/.exec(value);
		if (a) 
		{
                        return new Date(Date.UTC(+a[1], +a[2] - 1, +a[3], +a[4], +a[5], +a[6]));
		}
	}
	return value;		
}

// host is expected to be of form 'http://host:port/basePath'
etch.poc.Transport.prototype.init = function(host)
{
	this.host = host;
	this.startUri = host + "/start";	
	this.actionUri = host + "/request";
	this.eventUri = host + "/event";
	this.respondUri = host + "/response";		
}

etch.poc.Transport.prototype.connect = function(args) {
  
	var that = this;

	//console.log("Opening against host: " + this.startUri	);

	this.running = true;

	var transactionId = this.generateTransactionId();

	var transaction = { version: this.version, transactionId: transactionId };

	this.pendingTransactions[transactionId] = transaction;

 	transaction.xhr = jQuery.ajax({
		type: "GET",
		url: this.startUri,
		cache: false,
		dataType: "json",
		timeout: args.timeoutValue,
		success: function(data, textStatus) {
        	
			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}

			that.sessionId = data.sessionId;

			//console.log("Connection success. code: " + textStatus + ", sessionId: " + that.sessionId);

			//console.log("Opening event channel");
	
			that.startEventChannel();
			
			if(args.onSuccess != null)
			{
				//console.log("Calling user-defined connect success callback");

				if(that.service.callbacks._sessionNotify != null)
				{
					that.service.callbacks._sessionNotify(that.sessionUp);
				}
				args.onSuccess();

				//console.log("Return from user-defined connect success callback");
			}	
		},
		error: function(xhr, textStatus, exceptionObject) {
		
			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}

			//console.log("Connection failure. code: " + textStatus);

			if(args.onError != null)
			{
				//console.log("Calling user-defined connect error callback");

				args.onError();

				//console.log("Return from user-defined connect error callback");
			}
		}
		});
}

etch.poc.Transport.prototype.stop = function()
{
	this.running = false;

	for(var pendingTransaction in this.pendingTransactions)
	{
		if(pendingTransaction.xhr != null)
		{
			pendingTransaction.xhr.abort();
		}

		delete this.pendingTransactions[pendingTransaction.transactionId];		
	}
}

etch.poc.Transport.prototype.startEventChannel = function()
{
	this.openEventChannel();
}



etch.poc.Transport.prototype.openEventChannel = function()
{
	if(!this.running)
	{
		////console.log("Exiting event channel");
		return;
	}

	//	//console.log("Opening against host: " + this.eventUri	);

	var that = this;
	
	var transactionId = this.generateTransactionId();

	var transaction = { transactionId : transactionId };

	this.pendingTransactions[transactionId] = transaction;

 	transaction.xhr = jQuery.ajax({
		type: "POST",
		data: JSON.stringify({ version: this.version, sessionId: this.sessionId }),
		url: this.eventUri,
		cache: false,
	//	dataType: "json", // so we can run it through JSON.parse instead
		timeout: 35000,
		success: function(data, textStatus) {
        	
			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}

			//console.log("Eventing success. code: " + textStatus);
	
			//setTimeout(theService.openEventChannel, 1); // re-initiate eventing

			that.openEventChannel();

			data = JSON.parse(data, that.fixupData);

			that.handleMessage(data);
		},
		error: function(xhr, textStatus, exceptionObject) {
		
			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}

			//console.log("Eventing failure. code: " + textStatus);

			if(that.service.callbacks._sessionNotify != null)
			{
				that.service.callbacks._sessionNotify(that.sessionDown);
			}
		}
		});	
}

/* Models a Oneway message sent from client to server
 * @param methodName is the fully qualified name of the method being invoked, i.e., com.foo.bar.Method
 * @param args are an associative array 
 * @param methodTimeout integer (in milliseconds) governing how long to wait on transport layer for a response
 * @param successCallback called on successful send of the method
 * @param errorCallback called on failed send of the method
 */
etch.poc.Transport.prototype.onewayToServer = function(methodName, args, methodTimeout) 
{	
	var that = this;

	// rip out non-data from arguments
	// rip out non-data from arguments
	var onSuccess = null;
	var onError = null;

	if(args != null)
	{
		onSuccess = args.onSuccess;
		onError = args.onError;

		delete args.onSuccess;
		delete args.onError;
	};

	// no need to keep up with transaction
	var transactionId = this.generateTransactionId();

	var transaction = { transactionId : transactionId };

	this.pendingTransactions[transactionId] = transaction;

	transaction.xhr = jQuery.ajax({
		type: "POST",
		data: JSON.stringify({ version: this.version, sessionId: this.sessionId, transactionId: transactionId, method: methodName, args: args}),
		url: this.actionUri,
		cache: false,
		processData: false,
		contentType: "application/json",
		dataType: "json",
		timeout: 30000, /** methodTimeout removed... but how long should we wait on the transport side, just to send a timeout? */ 
		success: function(data, textStatus) {
        	
			//console.log(methodName + " success. code: " + textStatus);

			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}

			if(onSuccess != null)
			{
				//console.log("Calling user-defined " + methodName + " success callback");

				onSuccess();

				//console.log("Return from user-defined " + methodName + " success callback");
			}	
		},
		error: function(xhr, textStatus, exceptionObject) {
		
			//console.log(methodName + " failure. code: " + textStatus);

			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}

			if(onError != null)
			{
				//console.log("Calling user-defined " + methodName + " error callback");

				onError();

				//console.log("Return from user-defined " + methodName + " error callback");
			}
		}
		});
}

/* Models a Twoway message sent from client to server
 * @param methodName is the fully qualified name of the method being invoked, i.e., com.foo.bar.Method
 * @param args are an associative array 
 * @param methodTimeout integer (in milliseconds) governing how long to wait on transport layer for a response from the server
 * @param successCallback called on successful send, and receive of response, for the method
 * @param errorCallback called on failed send, and receive of response, for the method
 */
etch.poc.Transport.prototype.twowayToServer = function(methodName, args, methodTimeout) {
	
	
	var that = this;

	// rip out non-data from arguments
	var onSuccess = null;
	var onError = null;

	if(args != null)
	{
		onSuccess = args.onSuccess;
		onError = args.onError;

		delete args.onSuccess;
		delete args.onError;
	}

	// keep up-to-date our pending transaction bookkeeping 
	var transactionId = this.generateTransactionId();
	
	var transaction = {
		transactionId: transactionId,
		onSuccess: onSuccess,
		onError: onError
		};

	this.pendingTransactions[transactionId] = transaction;

	transaction.xhr = jQuery.ajax({
		type: "POST",
		data: JSON.stringify({ version: this.version, sessionId: this.sessionId, transactionId: transactionId, method: methodName, args: args}),
		url: this.actionUri,
		cache: false,
		processData: false,
		contentType: "application/json",
		dataType: "json",
		timeout: methodTimeout === 0 ? undefined : methodTimeout,
		beforeSend: function(xhr)
		{
			if(methodTimeout > 0)
			{
				transaction.timerId = setTimeout(function(){
				
					//console.log("Timeout fired for method: " + methodName + ", transaction: " + transactionId);

					if(that.pendingTransactions[transactionId] != null)
					{
						delete that.pendingTransactions[transactionId];

						if(onError != null)
						{
							//console.log("Calling user-defined " + methodName + " error callback due to timeout");
		
							onError(new etch.poc.TimeoutException());

							//console.log("Return from user-defined " + methodName + " error callback due to timeout");
						}
					}

				}, methodTimeout /* This would come from Etch IDL */);
			}
		},
		success: function(data, textStatus) {
        	
			//console.log(methodName + " send success. code: " + textStatus);		
		},
		error: function(xhr, textStatus, exceptionObject) {
		
			//console.log(methodName + " failure. code: " + textStatus);

			//console.log("clearing message timer");

			clearTimeout(transaction.timerId);

			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];

				if(onError != null)
				{
					//console.log("Calling user-defined " + methodName + " error callback");

					onError();

					//console.log("Return from user-defined " + methodName + " error callback");
				}	
			}
		}
		});
}

/* Dispatches to the correct callback, based on contents of data 
 * @param data should contain a field named 'eventMessages', which should be an array
	       Each array element should contain a field called 'type', which can be of type 'messageResult' or 'clientMessage'
 */
etch.poc.Transport.prototype.handleMessage = function(data)
{
	// does this contain any real data form the server?
	if(data.eventMessages != null)
	{
		//console.log("messages.length: " + data.eventMessages.length);

		for (var i = 0; i < data.eventMessages.length; i++)
		{
			var message = data.eventMessages[i];

			//console.log("message: " + JSON.stringify(message));

			// In the case that this message is a response to a previously invoked two-way, server message...
			if(message.type == "messageResult")
			{
				var transactionId = message.transactionId;
				// transactionId must be defined
				if(transactionId != null)
				{
					var pendingTransaction = this.pendingTransactions[transactionId];
	
					if(pendingTransaction != null)
					{
						delete this.pendingTransactions[transactionId];
						clearTimeout(pendingTransaction.timerId);

						if(message.data !== undefined) // presence indicates non-fault; not a value of null
						{
							if(pendingTransaction.onSuccess != null)
							{
								pendingTransaction.onSuccess(message.data);
							}
							else
							{
								//console.log("Unable to fire successCallback for transaction: " + transactionId);
							}
						}
						else if(message.errorData !== undefined)
						{
							if(pendingTransaction.onError != null)
							{
								pendingTransaction.onError(message.errorData);
							}
							else
							{
								//console.log("Unable to fire errorCallback for transaction: " + transactionId);
							}
						}
						else
						{
							//console.log("Unable to find 'data' or 'errorData' in response");
						}
					}
					else
					{
						//console.log("Unable to locate a pending transaction for: " + transactionId);
					}
				}
				else
				{	
					//console.log("Validity error: received a null transactionId");
				}
			}
			// Or in the case that this message is a new message from the server or client
			else if(message.type == "clientMessage")
			{
				// this is a client directed message.  
	
				// pull out method name
				var methodName = message.method;
	
				if(methodName != null)
				{
					try
					{
						var result = this.service.parametizer[methodName](this.service, message.data);
					}
					catch(err)
					{
						//console.log("Unable to initiate callback: " + err);
						// TODO: try/catch and fault back to server if failed
						continue;
					}
					// need to determine how this sort of metadata would be defined
					if(this.service.definitions[methodName].isOneway)
					{
						// one way messages, so no response necessary
						//console.log("Done with oneway message received from server.");
					}
					else
					{
						// twowayToClient	
						this.respondToServer(message.transactionId, message.method, result);
					}
				}
				else
				{
					//console.log("Validity error: received a null method name in client directed message");
				}
			}
		}
	}

	else
	{
		// need to figure out what other types of messages there would be
		// maybe a graceful server going down message?
		//console.log("Received a result message not tied to a pending message");
	}
	
}

etch.poc.Transport.prototype.respondToServer = function(transactionId, methodName, data)
{
	var that = this;

	//console.log("Responding to client-directed, twoway message");

	var transaction = { transactionId : transactionId };

	this.pendingTransactions[transactionId] = transaction;

	transaction.xhr = jQuery.ajax({
		type: "POST",
		data: JSON.stringify({ version: this.version, sessionId: this.sessionId, transactionId: transactionId, method: methodName, data: data}),	
		url: this.respondUri,
		cache: false,
		processData: false,
		contentType: "application/json",
		dataType: "json",
		timeout: 5000, /*This would come from Etch IDL */
		success: function(data, textStatus) {
        	
			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}
			//console.log("twowayToClient response send success. code: " + textStatus);		
		},
		error: function(xhr, textStatus, exceptionObject) {
		
			if(that.pendingTransactions[transactionId] != null)
			{
				delete that.pendingTransactions[transactionId];
			}
			//console.log("twowayToClient response failure. code: " + textStatus);
	
			// where to call this out?
		}
		});
}

etch.poc.Transport.prototype.generateTransactionId = function()
{
	return (++this.transactionFactory).toString();
}

etch.poc.BasePrototype = function BasePrototype()
{

}

etch.poc.BasePrototype.prototype["@type"] = null;

etch.poc.BasePrototype.prototype.setProperties = function(args)
{
	if(args != null && args.length > 0)
	{
		var properties = args[0];
		for(var field in properties)
		{
			this[field] = properties[field];	
		}
	}	
}

etch.poc.RuntimeException = function RuntimeException() 
{	
	this["@type"] = "_Etch_RuntimeException";
	
	this.setProperties(arguments);
	
}

etch.poc.RuntimeException.prototype.msg = null;

etch.poc.RuntimeException.prototype = new etch.poc.BasePrototype;
etch.poc.RuntimeException.prototype.constructor = etch.poc.RuntimeException;
etch.poc.TypeMap["_Etch_RuntimeException"] = etch.poc.RuntimeException.prototype.constructor;

etch.poc.AuthorizationException = function() 
{
	this["@type"] = "_Etch_AuthException";

	this.setProperties(arguments);
}

etch.poc.AuthorizationException.prototype = new etch.poc.BasePrototype;
etch.poc.AuthorizationException.prototype.constructor = etch.poc.AuthorizationException;
etch.poc.TypeMap["_Etch_AuthException"] = etch.poc.AuthorizationException.prototype.constructor;

etch.poc.TimeoutException = function()  	
{
	//console.log("constructing timeoutException");	
}
