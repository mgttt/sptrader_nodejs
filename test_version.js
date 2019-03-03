const logger=console;

//logger.log("before load SegfaultHandler");
//var SegfaultHandler = require('segfault-handler');
//logger.log("after load SegfaultHandler");
//
//SegfaultHandler.registerHandler("crash1.log"); // With no argument, SegfaultHandler will generate a generic log file name
//
//// Optionally specify a callback function for custom logging. This feature is currently only supported for Node.js >= v0.12 running on Linux.
//SegfaultHandler.registerHandler("crash2.log", function(signal, address, stack) {
//	// Do what you want with the signal, address, or stack (array)
//	// This callback will execute before the signal is forwarded on.
//});

//SegfaultHandler.causeSegfault(); 

function copy_o2o(o1,o2){for(var k in o2){o1[k]=o2[k]}return o1}
const argv2o=o=>o.reduce((r,e)=>(m=e.match(/^(\/|--?)([a-zA-Z0-9-_]*)="?(.*)"?$/))&&(r[m[2]]=m[3])&&r||r,{});

const os=require('os');

var argo={};
copy_o2o(argo,argv2o(process.argv));
//logger.log('argo=',argo);

var q_sptrader = require('./q_sptrader')();

//test if any segfault:
//const crypto=require('crypto');

var { host, port, license, app_id, user_id, password, host_id } = argo;

if(!host_id) host_id=80;//Default 80

port*=1;
host_id*=1;

q_sptrader.invoke('on','Test',rt=>{
	logger.log(".Test.rt=",rt);
});

q_sptrader.invoke('on','LoginReply',rt=>{
	logger.log(".LoginReply.rt=",rt);
});

q_sptrader.invoke('on','ConnectedReply',function(rt){
	logger.log("1:连接中，2:已连接，3:连接错误， 4:连接失败");
	logger.log(".ConnectedReply.rt=",rt);
});

if(!password) throw new Error('need /password=');

var login_info = { host, port, license, app_id, user_id, password };

var is_prod_init_done=false;

q_sptrader
	.invoke('call','SPAPI_GetDllVersion',{test:new Date()})
	.then( rst =>{
		logger.log('SPAPI_GetDllVersion.rst=',rst);
		return q_sptrader })
	.invoke('call','SPAPI_SetLoginInfo',login_info)
	.then((rst)=>{
		logger.log('SPAPI_SetLoginInfo.rst=',rst);

		if(rst && rst.rc==0) return q_sptrader;
		throw new Error("SPAPI_SetLoginInfo.rst="+rst);
	})
	.invoke('call','SPAPI_Login',{})
	.delay(3333)
	.then((rst)=>{
		logger.log('SPAPI_Login.rst=',rst);
		if(rst && rst.rc==0) return q_sptrader;
		throw new Error("SPAPI_Login.rst="+rst);
	})
	.then(()=>{ return q_sptrader; })
	.delay(6666)
	.invoke('call','SPAPI_GetLoginStatus',{user_id,host_id})
	.fail(err=>{
		logger.log('fail.rst=',err);
	})
	.done(rst=>{
		logger.log(process.versions);
		logger.log('done.rst=',rst);

		if(!is_prod_init_done){
			return q_sptrader
				.invoke('call','SPAPI_LoadInstrumentList')
				.delay(4000).then(()=>q_sptrader)
			//.invoke('call','SPAPI_GetInstrument')
			//.then(rst=>{
			//	if(rst)_tmp_log(o2s(rst.out));
			//})
				.invoke('call','SPAPI_LoadProductInfoListByCode',{inst_code:"HSI"})
				.delay(4000).then(()=>q_sptrader)
				.invoke('call','SPAPI_GetProduct')
				.then(rst=>{
					is_prod_init_done=true;
					logger.log(rst);
					//if(rst)_tmp_log(o2s(rst.out));
					//return Q({STS:"OK",errmsg:"",type:"I",sleepTime:4444});
				});
		}

		//if(rst&&rst.rc==3){
		//	dfr.resolve({STS:"OK"});
		//}else{
		//	logger.log("DEBUG SPAPI_GetLoginStatus",rst);
		//	rst.STS='KO';
		//	if(!rst.errmsg) rst.errmsg="LOGIN seems failed..";
		//	//rst.LatestLoginStatus=LatestLoginStatus;
		//	dfr.resolve(rst);
		//}
	})
;

