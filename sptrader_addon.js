// wrapper to call child proc
//@{call,on}
//.call(m,p,cb)
//.on(m,cb)

const Q=require('q');
const os=require('os');
const logger=console;

proc=null;
proc_pid=0;

var responseCallbacksPool = {};

var msgId = (new Date()).getTime();

const {fork} = require('child_process');

const proc_file = "./sptrader_addon_proc";

//TODO move inside func body later...
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
			logger.log("TODO DEBUG .message.msg=",responseId,responseData);
		})
		.on('error',err =>{ logger.log('[',proc_pid,']error.err=',err); })
		.on('exit', errcode => {
			logger.log('[',proc_pid,']fork');
			proc = fork(proc_file)
		})
	;

	//proc.on('error',err =>{ logger.log('[',proc_pid,']error.err=',err); });
	//proc.on('exit', errcode => {
	//	logger.log('[',proc_pid,']fork');
	//	proc = fork(proc_file)
	//});

	avatar.call_q = avatar.call = (m,p) => {

		var dfr = Q.defer();

		var msg = {m,p};

		//@see on(message)
		var cb = responseData => {
			dfr.resolve(responseData);
		};
		msgId=(msgId + 1) % 1000000;

		var callTime =
			cb.time = //for own gc()
			(new Date()).getTime();

		//
		var callbackId = msg.callbackId = 'cb_' + msgId + '_' + callTime;

		msg.callbackId = callbackId;//{callbackId,m,p}

		//TODO pack time.
		responseCallbacksPool[callbackId] = cb;

		//TODO...
		setTimeout(()=>{
			dfr.reject({errmsg:"TIMEOUT"});
		},30000);

		//logger.log('sptrader.call(',m,p,')',msg);
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

//TODO
gc=()=>{
	//loop responseCallbacksPool check callTime and clear timeout 31
	//	responseCallbacksPool[msg.responseId]=null;
	//	delete responseCallbacksPool[msg.responseId];
	//send_parent('checkin',new Date());
	logger.log("TODO sptrader_addon.gc()");
}

gc_loop = ()=>{
	try{ gc(); }catch(ex){console.log("gc.ex=",ex);}
	setTimeout(()=>gc_loop(),7777);
};
gc_loop();
