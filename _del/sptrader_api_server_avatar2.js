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

	var _LastPromise=Q();

	var Avatar=function(){
		this.call=function(m,p,cb){
			var _this=this;
			var callParam={m:m};
			if(p) callParam.p=p;
			var SimpleRequest=function(prevResult){
				//logger.log('at SimpleRequest prevResult=',prevResult);
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
			_LastPromise=_LastPromise.then(SimpleRequest);
			return _this;
		};
		this.delay=function(tt){
			_LastPromise=_LastPromise.delay(tt);
			return this;
		};
		this.then=function(f){
			_LastPromise=_LastPromise.then(f);
			return this;
		};
		this.fail=function(f){
			_LastPromise=_LastPromise.fail(f);
			return this;
		};
		this.done=function(f){
			return _LastPromise.done(f);
		};
	};
	return new Avatar();
};

