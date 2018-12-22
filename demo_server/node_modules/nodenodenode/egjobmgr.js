//暂时霸位.
module.exports=function(Application){
	function _getJob(id){
		var Job=Application.getSessionVar('Jobs.'+id);
		if(Job && Job.logic && Job.logic.Preempt_Promise && "function"==typeof(Job.logic.Preempt_Promise)){
			//skip, no need to require again
		}else{
			var mdl=Application.tryRequire(argo.approot + '/job_'+id,true);
			if(mdl){
				Job.logic=mdl(Application);
				Job.sts=Application.persist('job_status_'+id);
			}else{
				logger.log('typeof mdl',typeof(mdl),id);
				throw new Error("failed to load ./job_"+id+"???");
			}
		}
		return Job;
	}

	//@seeAlso logic.js
	function changeJobStatus(job_id,new_status){
		Application.persist('job_status_'+id,new_status);
	}
	//@see sub job js
	function getReloadFlag(){
		return _reload_flag;
	}
	//@see app.js
	function setReloadFlag(flag){
		if(flag){
			//clear the cached Jobs.
			Session.Jobs={};//@see _getJob()
		}
		_reload_flag=flag;
		return _reload_flag;
	}

	//@see called by Application _ReloadJobMgr()
	function _EntryPromise(){
		var dfr=Q.defer();
		if(getReloadFlag()){
			dfr.resolve({STS:"OK",errmsg:"_EntryPromise skip for 'to reload'",toReload:true});
		}
		for(var i in JobsArr){
			if(getReloadFlag()){
				dfr.resolve({STS:"OK",errmsg:"_EntryPromise skip for() for 'to reload'",toReload:true});
			}
			//针对每个job_id构建一个循环函数_job_preempt()并基于每次执行后的.sleepTime(默认1111)来调整下一轮时间。
			(job_id=>{
				var _job_preempt=function(){
					if(getReloadFlag()){
						dfr.resolve({STS:"OK",errmsg:"_EntryPromise skip _job_preempt for 'to reload'",toReload:true});
					}else{
						var job=_getJob(job_id);
						var _sleepTime=1111;//default
						if(job.sts!='disabled'){
							if(job.logic){
								if(job.logic.Preempt_Promise && "function"==typeof(job.logic.Preempt_Promise)){
									job.logic.Preempt_Promise().done(rst=>{
										if(rst){
											if(rst.type){
												process.stdout.write(rst.type);
											}
											if(rst.sleepTime){
												_sleepTime=rst.sleepTime;
											}
											if(rst.STS!="OK"){
												logger.log("WARNING _job_preempt ",job_id,".done(KO!!)",rst);
											}
										}else{
											logger.log("DEBUG job("+job_id+").preempt empty rst?",rst);
										}
										setTimeout(()=>{
											_job_preempt();
										},_sleepTime);
									});
								}else{
									logger.log('ERROR no job.logic.Preempt_Promise for ',job_id,job);
									setTimeout(()=>{
										_job_preempt();
									},_sleepTime);
								}
							}else{
								logger.log('ERROR no job.logic???');
								setTimeout(()=>{
									_job_preempt();
								},_sleepTime);
							}
						}else{//disabled
							setTimeout(()=>{
								_job_preempt();
							},_sleepTime);
						}
					}
				};
				logger.log("kickstart _job_preempt",job_id);
				_job_preempt();
			})(JobsArr[i]);
		}
		return dfr.promise;
	}

	///////////////////////////////////////////////////////////////////////////
	const {Q,o2s,s2o,fs,os,logger,Session,server_id}=Application;
	//这里暂时先hard code
	var JobsArr=[
		//"Health",//简单的健康检查器
		//"HSI",// 恒指相关价格计算、登录维持 等
		//"Strategy",// 策略运行器
	];
	//maybe jobs can be overrided by call-params
	var argo=Application.argo || {};
	if(argo.jobs){
		JobsArr=s2o(argo.jobs);
	}
	logger.log('JobsArr=',JobsArr);

	var _reload_flag=false;

	var stats = fs.statSync(__filename);//module.filename;
	var version=Application.getTimeStr(stats.mtime);
	var startTime=Application.getTimeStr();
	return {_EntryPromise,version,startTime,changeJobStatus,getReloadFlag,setReloadFlag};
}
