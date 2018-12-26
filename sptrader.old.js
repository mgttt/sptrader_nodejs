const Q=require('q');
const os=require('os');
const logger=console;

module.exports=
	(()=>{
		logger.log('load default _remoteModule');
		var ver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
		//var avatar= require('../sptrader/SpTrader.'+ver+'.node');
		var avatar= require('./SpTrader.'+ver+'.node');

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
		avatar._on(o=>{setTimeout(()=>{_callback_for_on(o)},2);});
		return avatar;
	})() || (()=>{throw new Error('sptrader not found?')})() ;

