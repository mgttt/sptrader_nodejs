const Q=require('q');
const os=require('os');
const logger=console;

// wrapper to call to child proc

//OUT
//.call(m,p,cb)
//.on(m,cb)

proc=null;

fork_q = (n,opts,onMessage) =>{

	if(!onMessage && typeof(opts)=="function"){
		onMessage = opts;
	}

	const {fork} = require('child_process');
	dfr=Q.defer();

	opts = {env:{
		//LD_PRELOAD:"libapiwrapper.so",
		LD_LIBRARY_PATH:__dirname}
	};
	proc = fork(n, opts);

	if(onMessage)
		proc.on('message', onMessage);

	//TODO onMessage('onError') onMessage('onExit')
	proc.on('error',err => dfr.reject(err) );

	proc.on('exit', errcode => {
		dfr.reject(errcode)
		if(onMessage){
			onMessage('onExit',{errcode})
		}

		//fork again if child exit... to keep it alive
		proc = fork(n)
		//TODO write log
	});

	//_child_pool[proc.pid]=proc;
	return dfr.promise;
};

var responseCallbacksPool = {};

var msgId = (new Date()).getTime();

module.exports= (()=>{

	fork_q('./sptrader_addon_proc', msg=>{
		//process.send(m);
		logger.log('child proc .msg=',msg);
	})

	//@avatar
	var avatar={};

	avatar.call_q = avatar.call = (m,p) => {

		msg = {m,p};

		logger.log('TODO sptrader.call(',m,p,')');

		var cb = responseData => {
		};
		msgId=(msgId + 1) % 1000000;

		var callTime =
			cb.time = //for own gc()
			(new Date()).getTime();

		//
		var callbackId = msg.callbackId = 'cb_' + msgId + '_' + callTime;

		msg.callbackId = callbackId;//{callbackId,m,p}

		responseCallbacksPool[callbackId] = cb;

		proc.send(msg);

		//@~Q()
		return Q();
	};

	avatar.on = (m,cb)=>{
		logger.log('TODO sptrader.call(',m,')');
	};

	return avatar;

	//		logger.log('load default _remoteModule');
	//		var ver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
	//		//var avatar= require('../sptrader/SpTrader.'+ver+'.node');
	//		var avatar= require('./SpTrader.'+ver+'.node');
	//
	//		avatar.call=function(m,p){
	//			var dfr=Q.defer();
	//			if(!p)p={};
	//			var argumentsList=[m,p,function(rst){
	//				setTimeout(()=>{//实践经验:加setTimeout后再返回能充分利用异步来SPAPI的阻塞...
	//					dfr.resolve(rst);
	//				},2);
	//			}];
	//			if(avatar._call){
	//				if(!argumentsList) argumentsList={};//TMP FIX, need to fix c/c++ later...
	//				//TODO 这里需要做一个 api_call_audit_log
	//				avatar._call.apply(avatar,argumentsList);
	//			}else{
	//				throw new Error("not found _call() ???");
	//			}
	//			return dfr.promise;
	//		};

	//	var _callback_pool={};
	//	//save the cb
	//	avatar.on = (m,cb)=>( _callback_pool[m]=cb );
	//	var _callback_for_on=function(o,d){
	//		if(o){
	//			var {data,on,seq} = o;
	//			if(on){
	//				if(_cb=_callback_pool[on]){
	//					_cb(data);
	//				}else{
	//					logger.log("TODO on(",on,")",data);
	//				}
	//			}
	//		}
	//	};

	//old detach ._on({data,on,seq});
	//		avatar._on(o=>{setTimeout(()=>{_callback_for_on(o)},2);});
	//		return avatar;
})();

tmp_loop = ()=>{

	//gc();

	//try{
	//	responseCallbacks[msg.responseId]=null;
	//	delete responseCallbacks[msg.responseId];
	//	_gc();
	//}catch(ex){console.log(ex);}

	//var pid = process.pid;
	//logger.log("child.pid=",pid);

	////TMP
	//send_parent('checkin',new Date());

	setTimeout(()=>{
		tmp_loop()
	},7777);
};

tmp_loop();


