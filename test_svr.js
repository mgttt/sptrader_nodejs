Q=require('q');
logger=console;

//_child_pool={};
//_debug_pool=()=>{
//	for(var pid in _child_pool){
//		logger.log("_child_pool.pid=",pid);
//	}
//};

proc=null;

fork_q = (n,onMessage) =>{
	const {fork} = require('child_process');
	dfr=Q.defer();
	proc = fork(n);

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

fork_q('./testAddon', msg=>{
	//process.send(m);
	logger.log('child proc .msg=',msg);
})

tmp_loop = ()=>{
	pid = process.pid;//TODO
	logger.log("parent.pid=",pid);
	//_debug_pool();
	setTimeout(()=>{
		tmp_loop()
	},11111);
};

tmp_loop();



