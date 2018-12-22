module.exports=function(Application){
	const {Q,o2s,s2o,fs,os,logger,devlog,Session,server_id,isEmpty}=Application;

	var JobMgr=Application.getJobMgr();

	var remoteModule;
	var Q_remoteModule;
	function initRemoteModule(){
		if(!remoteModule){
			//if(Application.remoteModule){
			//	remoteModule=Application.remoteModule;
			//	return remoteModule;
			//}
			////////////////////////////////////////////////////////////////////////////////
			remoteModule=Application.argo.sptrader || (()=>{
				logger.log('load default remoteModule');
				var ver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
				var avatar= require('../sptrader/SpTrader.'+ver+'.node');

				//avatar.call=function(m,p,cb){
				//	if(cb){
				//		var argumentsList=[m,p,function(rst){
				//			setTimeout(()=>{
				//				cb(rst);
				//			},2);
				//		}];
				//		avatar._call.apply(avatar,argumentsList);
				//	}else{
				//		var argumentsList=[m,p];
				//		return avatar._call.apply(avatar,argumentsList);
				//	}
				//};
				avatar.call=function(m,p){
					var dfr=Q.defer();
					var argumentsList=[m,p,function(rst){
						setTimeout(()=>{
							//cb(rst);
							dfr.resolve(rst);
						},2);
					}];
					if(avatar._call){
						avatar._call.apply(avatar,argumentsList);
					}else{
						throw new Error("not found _call() ???");
					}
					return dfr.promise;
				};
				var _callback_hooks={};
				avatar.on=(m,cb)=>{
					_callback_hooks[m]=cb;
				};
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

			remoteModule.on('Test',function(rt){
				logger.log("remoteModule.callback(Test)=>",rt);
			});

			//for minmin to use
			remoteModule.on('ApiTickerUpdate',function(rt){
				logger.log("remoteModule.ApiTickerUpdate(Test)=>",rt);
			});

			Q_remoteModule=Q(remoteModule);
			Q_remoteModule
				.invoke('call','SPAPI_GetDllVersion',{test:new Date()})
				.then(rst=>{
					logger.log("Q(remoteModule).call(1) =>",rst);
					return Q_remoteModule;
				})
				.invoke('call','SPAPI_GetDllVersion',{test:new Date()})
				.then(rst=>{
					logger.log("Q(remoteModule).call(2) =>",rst);
					return Q_remoteModule;
				})
			;
			Application.remoteModule=remoteModule;
			Application.Q_remoteModule=Q_remoteModule;
		}
		return remoteModule;
	}

	function Quit_Promise(param){
		var rt={STS:"OK",errmsg:"Will Quit Server("+server_id+") after 1 sec, please try reconnect later about 10 seconds for init."};
		setTimeout(()=>{
			Application.quit();
		},333);
		return Q(rt);
	}

	function SuperQuit_Promise(param){
		Application.persist('auto_login_flag',false);
		return Quit_Promise(param);
	}

	function Ping_Promise(p){//throw new Error("testing");
		var ping=null;
		if(p && p.ping) ping=p.ping;
		var rt={STS:"OK"};
		rt.pong = new Date();
		if(ping){
			rt.ping=ping;
			rt.diff = (rt.pong.getTime() - (new Date(ping)).getTime())/1000;
		}
		return Q(rt);
	}
	//SIGINT
	function handleSIGINT(){
		Quit_Promise().done(()=>{
			logger.log('Quit for SIGINT/ctrl-c');
		});
		/*
			if(Application){
				var lgc=Application.getLogic();
				if(lgc){
					lgc.Quit_Promise().done(()=>{
						logger.log('Quit for SIGINT/ctrl-c');
					});
				}else{
					logger.log('Quit for SIGINT/ctrl-c');
				}
			}
			*/
	}
	function handleUncaughtException(err){
		Application.persist('LastUncaughtException',(err&&err.message)?err.message:(""+err));
		Quit_Promise().done(()=>{
			logger.log('Quit for uncaughtException',err);
		});
	}

	///////////////////////////////////////////////////////////////////////////
	initRemoteModule();

	var call=function(m,p){
		return remoteModule.call(m,p);
	};
	var rt={
		Quit_Promise,
		SuperQuit_Promise,
		//Health_Promise,
		Ping_Promise,

		handleSIGINT,
		handleUncaughtException,
		//handleExit,
		call,
	};
	rt.version=Application.getTimeStr(fs.statSync(__filename).mtime);
	rt.startTime=Application.getTimeStr();
	return rt;
};
