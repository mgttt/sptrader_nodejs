var debug=1;
var logger=console;
var wc=require('q_webclient');//我们自己写的 基于 q.js 的 webclient，暂时还好用.
//wc.setDebugLevel(2);

const o2s=function(o){try{return JSON.stringify(o);}catch(ex){logger.log('JSON.stringify.err=',ex)}};
const s2o=function(s){try{return(new Function('return '+s))()}catch(ex){}};

var SpWrapper=function(opts){
	this.remote=(m,p)=>{
		return wc.web_Promise(opts,{m,p})
			.then(rst=>{
				var rt={STS:'KO'};
				var body=rst.body;
				if(rst){
					if(body){
						var o=s2o(body);
						if(o){
							if(o.STS){
								rt=o;
								return Q(rt);
							}else{
								if(o.errmsg){
									rt.errmsg=o.errmsg;
									rt.s=body;
								}else{
									rt.errmsg=body;
									rt.s=body;
								}
							}
						}else{
							rt.errmsg=body;
						}
					}else{
						if(rst.errmsg)rt.errmsg=rst.errmsg;
						else rt.errmsg=o2s(rst);
					}
				}else{
					rt.errmsg='no reply?'
				}
				//logger.log('reject with ',rst);
				return Q.reject(rt);
			});
	}
};

//现在是hardcode，应该要从命令行获取{
var user_id='DEMO201705235';
var host='demo.spsystem.info';
var host_id=80;
var port=8080;
var password='123456';
var license='592E64FEBCDFD';
var app_id="SPDEMO";
//现在是hardcode，应该要从命令行获取}

const Q=require('q');
var QSP=new SpWrapper({
	//url:'http://sptrader.cc:6601',//old tmp
	url:'http://localhost:6601',
	//cookie_pack:'mytest6601'//id for cookie pack
});

//如果在登陆状态就返回，否则尝试 try_time 次
function TryLogin(try_time/*TODO*/){
	return Q(QSP)
		.invoke('remote','SPAPI_GetLoginStatus',{user_id,host_id})
		.then(rst=>{
			if(debug>1) logger.log('SPAPI_GetLoginStatus.then.rst=',rst);
			if(rst&&rst.rc==3){
				return Q({STS:'OK'});//是登陆状态.
			}else{
				logger.log('try login again....');
				Q(QSP)	
					.invoke('remote','SPAPI_SetLoginInfo',{user_id,host_id,host,port,password,license,app_id})
					.then(rst=>{
						if(rst&&rst.rc==0){
							logger.log('SPAPI_SetLoginInfo.rst=',rst);
							return QSP
						}else{
							throw rst;
							//return Q.reject({STS:'KO',errmsg:'SPAPI_SetLoginInfo failed.'});
						}
					})
					.invoke('remote','SPAPI_Login')
					.then(rst=>{
						if(debug>1) logger.log('SPAPI_Login.rst=',rst);
						if(rst&&rst.rc==0){
							return QSP
						}else{
							throw rst;
						}
					})
					.delay(3333)//let login completed
					.invoke('remote','SPAPI_GetLoginStatus',{user_id,host_id})
					.then(rst=>{
						if(debug>0) logger.log('SPAPI_GetLoginStatus.rst=',rst);
						if(rst&&rst.rc==3){
							return Q({STS:'OK'});//是登陆状态.
						}else{
							return Q.reject({STS:'KO',errmsg:'try login again failed?'});
						}
					})
				;
			}
		})
}
function GetBalance(){
	//TODO 可以再根据实际需求调整结果形式:
	return Q.all([
		Q(QSP).invoke('remote','SPAPI_GetAllAccBal',{user_id}),
		Q(QSP).invoke('remote','SPAPI_GetAccInfo',{user_id}),
	])
		.then(rst_a=>{
			return Q({STS:'OK',bal:rst_a[0],acc:rst_a[1]});
		});
}
function GetProducts(rst_prev){
	var inst_code='HSI';//TMP
	return Q(QSP).invoke('remote','SPAPI_LoadInstrumentList')
		.then(rst=>{
			if(rst&&(rst.rc==0||rst.rc==1)){
				if(rst.rc==0) return Q.delay(6666).then(()=>{return QSP});//let it complete
				/* TODO do later if for other products
					for (var i in rst.out){
						var inst_code = rst.out[i].InstCode;
						sptrader.callSpTrader('SPAPI_LoadProductInfoListByCode',{inst_code})
						.then(rst=>{
							logger.log('load successed');
						});
					}
					*/
				return QSP;
			}else{
				logger.log(rst);
				return Q.reject({STS:'KO',errmsg:'fail SPAPI_LoadInstrumentList'});
			}
		})
		.invoke('remote','SPAPI_LoadProductInfoListByCode',{inst_code})
		.then(rst=>{
			if(rst&&(rst.rc==0||rst.rc==1)){
				if(rst.rc==0) return Q.delay(6666).then(()=>{return QSP});//let it complete
				return QSP;
			}else{
				logger.log(rst)
				return Q.reject({STS:'KO',errmsg:'fail SPAPI_LoadProductInfoListByCode'});
			}
		})
		.invoke('remote','SPAPI_GetProduct')
		.then(rst=>{
			logger.log(rst);
			return rst;
		});
}

TryLogin(1)
	.then(GetBalance)
	.delay(1111)
	.then(GetProducts)
	.catch(err=>{
		logger.log('catch.err=',err);
		return err;//to done()
	})
	.done(rst=>{
		logger.log('done.rst=',rst);
	});
