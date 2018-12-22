//WARNING: this logic for demo only

const o2s=function(o){try{return JSON.stringify(o);}catch(ex){}};
//const s2o=function(s){try{return JSON.parse(s);}catch(ex){}};//fail for parsing like {m:"XXX"}
const s2o=function(s){try{return(new Function('return '+s))()}catch(ex){}};

module.exports = function(opts){
	var logger=opts.logger || console;

	//var sptrader=opts.sptrader || (()=>{throw new Error('need sptrader')})();
	var sptrader=opts.sptrader || (()=>{
		var os=require('os');
		var plugver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
		return require('../sptrader/SpTrader.'+plugver+'.node');
	})();

	sptrader._on('Test',function(rt){
		logger.log("callback(Test)=>",rt);
	});
	logger.log(sptrader._call('SPAPI_GetDllVersion',{test:new Date()}));

	var streamToString = function(stream, callback){
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

	//return the function that .createServer() needs:
	return function(req,res){
		var tm0=new Date();

		streamToString(req,function(s){
			logger.log('s=',s);
			var o=s2o(s)||{};
			logger.log('o=',o);
			var m=o.m||"VOID";
			var rt={};
			if(m){
				try{
					//this is 'SYNC' mode
					rt=sptrader._call(o.m,o.p)||{};
					/*
					if('function'==typeof(sptrader[m])){
						rt=sptrader[m].apply(sptrader,o.p)||{};
					}else{
						//TODO other api ;)
						rt.errcode=999;
						rt.errmsg='not found '+m;
						rt.o=o;
						rt.s=s;
					}
					*/
				}catch(ex){
					rt.exs=""+ex;
					rt.errcode=999;
					rt.errmsg='ex when call('+m+')';
					rt.o=o;
					rt.s=s;
				}
			}else{
				rt.errcode=998;
				rt.errmsg='need m';
				rt.o=o;
				rt.s=s;
			}
			rt.tm0=tm0;
			rt.tmX=new Date();
			res.write(o2s(rt));
			res.end();
		});
	};//return function
};
