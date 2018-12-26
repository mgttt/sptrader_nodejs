const Q=require('q');
const os=require('os');
const logger=console;

var _remoteModule;

module.exports = function(Application){
	if(!Application)Application={};
	if(!Application.argo)Application.argo={};
	var Q_remoteModule;
	if(!_remoteModule){
		_remoteModule=Application.argo.sptrader || (()=>{
			return require('./sptrader_addon')
		})() || (()=>{throw new Error('sptrader_addon not found?')})() ;

		_remoteModule.on('Test', rst=>{
			logger.log("_remoteModule.on(Test).rst=>",rst);
		});

		Q_remoteModule=Q(_remoteModule);

		Q_remoteModule
			.invoke('call','SPAPI_GetDllVersion',{test:new Date()})
			.then(rst=>{
				logger.log("Q(_remoteModule).call(1) =>",rst);
				return Q_remoteModule;
			})
			.invoke('call','SPAPI_GetDllVersion',{test:new Date()})
			.then(rst=>{
				logger.log("Q(_remoteModule).call(2) =>",rst);
				return Q_remoteModule;
			})
		;
		Application.Q_remoteModule=Q_remoteModule;
	}
	return Q_remoteModule;
};
