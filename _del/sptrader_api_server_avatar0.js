const o2s=function(o){try{return JSON.stringify(o);}catch(ex){}};
const s2o=function(s){try{return(new Function('return '+s))()}catch(ex){}};
const Q=require('q');//important..
const web=require("http");//TODO https later
var logger=console;

const StreamToString=(stream, callback)=>{
	var str = '';
	stream.on('data', function(chunk) {
		str += chunk;
	}).on('end', function(){
		try{
			callback(str);
		}catch(ex){
			logger.log("ex=",ex);
		}
	}).on('error', function(err){
		callback(""+err);
	})
	;
};

//TODO add 4rd "timeout"
const SimpleRequest=(hostname,port,param)=>{
	var dfr=Q.defer();
	var postData=o2s(param);
	var reqp={
		hostname: hostname,
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
			dfr.resolve(s2o(s));
		});
	}).on('error',err=>{
		logger.log(`problem with request: ${err.message}`);
		dfr.resolve({STS:"KO",errmsg:""+err});
	});
	req.write(postData);
	req.end();
	return dfr.promise;
};

module.exports = function(opts){
	var logger=opts.logger || console;

	var host=opts.host||'127.0.0.1';
	var port=opts.port||5555;

	//NOTES: using Proxy(NODEJS6+ Harmony feature that support some magic method)
	//https://developer.mozilla.org/en/docs/Web/JavaScript/Reference/Global_Objects/Proxy
	return new Proxy({},{

		//mmm is name of method()
		get: (target, mmm, receiver)=>{

			var default_method=new Proxy(()=>{},{
				apply: function(target, thisArg, argumentsList) {
					return null;
				}
			});

			//return if already exists:
			var rt=target[mmm];
			if(rt){ return rt; }

			//return a empty default method() for not string 
			if ('string'!=typeof mmm){
				logger.log('TODO2 mmm=',mmm);
				return default_method;
			}

			var methods=target._methods_;
			if(!methods){ methods=target._methods_={}; }
			rt=methods[mmm];
			if(!rt){
				//if the method not buffered, build one
				rt=methods[mmm]=new Proxy(()=>{},{
					apply: function(target, thisArg, argumentsList){
						var callParam={m:mmm};
						if(argumentsList && argumentsList.length>0) callParam.p=argumentsList[0];

						//send to remote and return a Promise:
						return SimpleRequest(host,port,callParam);
					}
				});
			}
			return rt;
		}
	});
};

