const o2s=function(o){try{return JSON.stringify(o);}catch(ex){}};
const s2o=function(s){try{return(new Function('return '+s))()}catch(ex){}};
const Q=require('q');//important..
const web=require("http");//TODO https later
var logger=console;

var StreamToString=function(stream, callback){
	var str = '';
	stream.on('data', function(chunk){
		str += chunk;
	}).on('end', function(){
		try{
			callback(str);
		}catch(ex){
			logger.log("ex=",ex);
		}
	}).on('error', function(err){
		callback(""+err);
	});
};

module.exports = function(opts){
	var logger=opts.logger || console;

	var host=opts.host||'127.0.0.1';
	var port=opts.port||5555;

	//注意，这里用到ES6的harmony之 "Proxy"特性，不是给Web参考的。
	//Web请参考 sptrader_api_server_avatar_web.js
	Q.makePromise.prototype=new Proxy(Q.makePromise.prototype,{
		//mmm is name of method()
		get: (target, mmm, receiver)=>{
			var _target0=target;
			//var _this=this;
			//return if already exists:
			var rt=_target0[mmm];
			if(rt){ return rt; }
			//return a empty default method() for not string 
			if ('string'!=typeof mmm){
				logger.log('TODO mmm=',mmm);
				return default_method;
			}
			var methods=_target0._methods_;
			if(!methods){ methods=_target0._methods_={}; }
			rt=methods[mmm];
			if(!rt){
				//if the method not buffered, build one
				rt=methods[mmm]=new Proxy(()=>{},{
					apply: function(target2, thisArg, argumentsList){
						return thisArg.then((prevResult)=>{
							//logger.log('apply(',mmm,').prevResult=',prevResult);
							var callParam;
							if(mmm=='call'){
								callParam={};
								if(argumentsList && argumentsList.length>0){
									callParam.m=argumentsList[0];
								}
								if(argumentsList && argumentsList.length>1){
									callParam.p=argumentsList[1];
								}
							}else if(mmm=='on'){
								throw new Error('on() is not support to call remotely!');
							}else{
								callParam={m:mmm};
								if(argumentsList && argumentsList.length>0) callParam.p=argumentsList[0];
							}
							var dfr=Q.defer();
							var postData=o2s(callParam);
							var reqp={
								hostname: host,
								port: port,
								method: 'POST',
								path:'/',
								headers: {
									'Content-Type': 'application/x-www-form-urlencoded',
									'Content-Length': Buffer.byteLength(postData)
								}
							};
							var req=web.request(reqp,res=>{
								StreamToString(res,s=>{
									//setTimeout(()=>{
									dfr.resolve(s2o(s));
									//},3000);//timeout for test...
								});
							}).on('error',err=>{
								logger.log(`problem with request: ${err.message}`);
								dfr.resolve({STS:"KO",errmsg:""+err});
							});
							req.write(postData);
							req.end();
							return dfr.promise;
						});
					}
				});
			}
			return rt;
		}
	});
	return Q();
};
