// wrapper to call child proc:
//.call(m,p,cb)
//.on(m,cb)

//NOTES: npm install q4web
const q4web = require('q4web')({cookie_readonly:true});

var {options,getTimeStr,Q,QOK,o2s,o2o,s2o,os,base64_encode,base64_decode}=q4web;

//const logger=console;
var server_id = options.server_id;
var logger = {
	log: function(){
		return q4web.logger.log.apply(q4web.logger,[server_id,getTimeStr()].concat(Array.from(arguments)));
	}
};

//
if(options.use_east_monitor){
	const easyMonitor = require('easy-monitor');
	easyMonitor('test_tinyfsm_mem');//browser http://127.0.0.1:12333
	var GetMemoryUsage = ()=>{
		var rt = process.memoryUsage()||{};
		rt.freemem=os.freemem();
		rt.totalmem=os.totalmem();
		return rt;
	};
	var gMemoryUsage = GetMemoryUsage();
	var Max=(n1,n2)=>(n1>n2)?n1:n2;
	var GetMemInfo = ()=>{
		var _MemoryUsage = GetMemoryUsage();
		for(var k in _MemoryUsage){
			gMemoryUsage[k+'_max'] = Max(_MemoryUsage[k],gMemoryUsage[k]);
			gMemoryUsage[k] = _MemoryUsage[k];
		}
		return gMemoryUsage;
	}
}

var proc=null;
var proc_pid=0;
var responseCallbacksPool = {};
var msgId = (new Date()).getTime();
const {fork} = require('child_process');
const proc_file = "./sptrader_addon_proc";

var avatar={};

module.exports= (()=>{

	opts = {env:{
		LD_PRELOAD:"libapiwrapper.so",
		LD_LIBRARY_PATH:__dirname
	}};

	//proc = fork(proc_file, opts);
	proc = fork(proc_file, [], opts);

	proc_pid = proc.pid;

	proc
		.on('message', (msg={})=>{
			var {responseId,responseData}=msg;

			if(responseId){
				var _cb = responseCallbacksPool[responseId];
				if(_cb){
					//logger.log("DEBUG cb.responseData=",responseData);
					_cb(responseData);
					if(_cb.time){
						logger.log("DEBUG DELETE cb.time=",_cb.time);
						responseCallbacksPool[responseId]=null;
						delete responseCallbacksPool[responseId];
					}
					return;
				}
			}
			//logger.log("TODO DEBUG .message.msg=",responseId,responseData);
		})
		.on('error',err =>{ logger.log('[',proc_pid,']error.err=',err); })
		.on('exit', errcode => {
			logger.log('[',proc_pid,']fork again...');
			proc = fork(proc_file)
		})
	;

	avatar.call_q = avatar.call = (m,p) => {

		var dfr = Q.defer();

		var msg = {m,p};

		var cb = responseData => dfr.resolve(responseData);

		msgId=(msgId + 1) % 1000000;

		var callTime = cb.time = //for own gc()
			(new Date()).getTime();

		var callbackId = msg.callbackId = 'cb_' + msgId + '_' + callTime;

		msg.callbackId = callbackId;//{callbackId,m,p}
		responseCallbacksPool[callbackId] = cb;

		setTimeout(()=>dfr.reject({errmsg:"TIMEOUT"}),30000);

		proc.send(msg);
		return dfr.promise;
	};

	avatar.on = (m,cb)=>{
		logger.log("DEBUG buffer .on=",m);
		responseCallbacksPool[m]=cb;
		return avatar;
	};

	return avatar;
})();

var gc=()=>{
	var nowTime = new Date().getTime();
	for (var k in responseCallbacksPool) {
		var cb = responseCallbacksPool[k];
		//NOTES: 只清呼叫类的，所以判断 .time刚好，先测试下60秒的清...
		if (cb.time && (nowTime - cb.time) > 60000) {
			logger.log('gc k=', k);
			responseCallbacksPool[k] = null;
			delete responseCallbacksPool[k];
		}
	}
}

var gc_loop = ()=>{
	setTimeout(gc_loop,66666);
	try{ gc(); }catch(ex){console.log("gc.ex=",ex);}
};

gc_loop();
