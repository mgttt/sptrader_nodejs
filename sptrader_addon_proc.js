//wrapper child proc for the .node

//@<os>
const os=require('os');

//logger=@<console>;
const logger=console;

//@.ver=
var ver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;

logger.log('ver=',ver);

send_parent = (m,p)=>{
	process.send({m,p});
};

//module.exports=
sptrader_addon = (()=>{
	var avatar= require('./SpTrader.'+ver+'.node');

	//TODO wrap the callback to the caller...
	avatar.call=function(m,p){
		var dfr=Q.defer();
		if(!p)p={};
		var argumentsList=[m,p,function(rst){
			setTimeout(()=>{//实践经验:加setTimeout后再返回能充分利用异步来SPAPI的阻塞...
				dfr.resolve(rst);
			},2);
		}];
		if(avatar._call){
			if(!argumentsList) argumentsList={};//TMP FIX, need to fix c/c++ later...
			//TODO 这里需要做一个 api_call_audit_log
			avatar._call.apply(avatar,argumentsList);
		}else{
			throw new Error("not found _call() ???");
		}
		return dfr.promise;
	};

	var _callback_hooks={};
	avatar.on=(m,cb)=>{ _callback_hooks[m]=cb; };
	var _callback_for_on=function(o,d){
		if(o){
			var _data=o.data,_on=o.on,seq=o.seq;
			if(_on){
				var _cb=_callback_hooks[_on];
				if(_cb){
					_cb(_data);
				}else{
					logger.log("TODO on(",_on,")",_data);
				}
			}
		}
	};
	avatar._on(o=>{setTimeout(()=>{_callback_for_on(o)},2)});
	return avatar;
})() || (()=>{throw new Error('sptrader addon not found '+ver)})() ;

process.on('message',msg=>{
	logger.log("TODO sptrader_addon_proc.on(Message).msg=",msg);
});

//var responseCallbacks = {};
//var msgHandlerSet={};
//var msgId = 1;
//function _gc(){
//	if(responseCallbacks){
//		var nowTime=new Date().getTime();
//		for(k in responseCallbacks){
//			var cb=responseCallbacks[k];
//			if( (nowTime-cb.time)>1800000 ){
//				responseCallbacks[msg.responseId]=null;
//				delete responseCallbacks[msg.responseId];
//			}
//		}
//	}
//}
//if (cb) {
//	msgId=(msgId + 1) % 1000000;
//	var callTime=cb.time=(new Date()).getTime();
//	var callbackId = msg.callbackId = 'cb_' + msgId + '_' + callTime;
//	responseCallbacks[callbackId] = cb;
//}

/*
_app2js: function (msg, callback){
	var _this=this;
	setTimeout(function(){
		if (msg.responseId) {
			//this msg is a "Reply", so find the original callback
			callback = responseCallbacks[msg.responseId];
			if (!callback) { return; }
			callback(msg.responseData);
			try{
				responseCallbacks[msg.responseId]=null;
				delete responseCallbacks[msg.responseId];
				_gc();
			}catch(ex){console.log(ex);}
		} else {
			var handler = null;
			if (msg.handlerName) {
				if(handler = msgHandlerSet[msg.handlerName]){
					try {
						if (msg.callbackId) {
							var callbackResponseId = msg.callbackId;
							callback = function(responseData) {
								win.WebViewJavascriptBridge._js2app({
									responseId: callbackResponseId,
									responseData: responseData
								});
							};
						}
						handler(msg.data, callback);
					} catch (exception) {
						console.log("WebViewJavascriptBridge: WARNING: javascript handler threw.", message, exception);
					}

				}else{
					console.log("WebViewJavascriptBridge: not found handler name="+msg.handlerName);
				}
			}else{
				console.log("WebViewJavascriptBridge ERROR: unsupported msg from app!!!",msg);
			}
		}
	},1);
	return {STS:"_app2js"};
},
*/	

//TODO do gc()
tmp_loop = ()=>{

	//try{
	//	responseCallbacks[msg.responseId]=null;
	//	delete responseCallbacks[msg.responseId];
	//	_gc();
	//}catch(ex){console.log(ex);}
	
	var pid = process.pid;
	logger.log("child.pid=",pid);

	//TMP
	send_parent('checkin',new Date());

	setTimeout(()=>{
		tmp_loop()
	},7777);
};

tmp_loop();

