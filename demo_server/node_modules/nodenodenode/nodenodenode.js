/*
 * .daemon() fwd req to appModule
 *        server_host/server_port => http server
 *    or  http_host/http_port     => http server
 *         https_host/https_port  => https server
 *         ws_host/ws_port        => web-socket server
 *                   + ws_secure  => wss
 */
const Q = require('q');
const util = require('util');
var logger=console;//default logger
function isEmpty(o,i){for(i in o){return!1}return!0}
function copy_o2o(o1,o2){ for(var k in o2){ o1[k]=o2[k]; } return o1; }//copy from o2 to o1, but notes that didn't check o1 here...
function argv2o(argv,m,mm){
	var rt={};for(k in argv)(m=(rt[""+k]=argv[k]).match(/^(\/|--?)([a-zA-Z0-9-_]*)=(.*)/))&&(rt[m[2]]=(mm=m[3].match(/^".*"$/))?mm[1]:m[3]);return rt;
}
var flag_init_ok=false;
var argo={};
var nodenodenode=module.exports={
	argv2o:argv2o,//expose to caller if they need
	daemon:this_argo=>{
		if(!isEmpty(this_argo)) copy_o2o(argo,this_argo);
		var rt={STS:'OK'};
		if(flag_init_ok){
			//console.log('duplicate init');
			copy_o2o(argo,this_argo);
			rt.errmsg='duplicate init';
		}else{
			copy_o2o(argo,argv2o(process.argv));
			if(typeof(nw)!='undefined'){
				copy_o2o(argo,argv2o(nw.App.argv));
				argo.is_nwjs=true;
				//nwjs special...
				logger={log:function(){
					try{console.log(util.format.apply(null, arguments))}catch(ex){
						console.log.apply(console,arguments);}
				}};
			}
			if(typeof(global)!='undefined'){
				argo.has_global=true;
			}
			//logger.log(argo);
			//for(var k in argo){
			//	logger.log('argo.'+k+' is '+typeof(argo[k]));
			//}

			process.env.UV_THREADPOOL_SIZE = argo.UV_THREADPOOL_SIZE || 126;//thread pool for uv_queue_work()

			logger.log(process.env);

			logger.log("process.versions=",process.versions);

			if(!argo.app){
				if(!argo.approot){//must specify the approot for default egapp
					//throw new Error('-approot is needed if -app is absent');
					argo.approot=process.cwd();
				}
				argo.app=__dirname + '/egapp.js';//load the default egapp ...
			}

			var appModule=require(argo.app)({argo});

			////////////////////////////////////////////////////////// HTTP => APP
			var http_host=argo.server_host||argo.http_host||argo.h||'0.0.0.0',http_port=argo.server_port||argo.http_port||argo.p;
			//var http_server=require('http').createServer(appModule({argo}));
			if(http_port){
				if(!appModule.handleHttp) throw new Exception('appModule.handleHttp is not defined.');
				argo.http_server=require('http').createServer(appModule.handleHttp);//let the internal logic can access
				try{
					argo.http_server.listen(http_port,http_host,()=>{logger.log('http listen on ',http_host,':',http_port)});
					rt.flag_http=true;
					flag_init_ok=true;
				}catch(ex){
					logger.log('failed to start http_server on '+http_host+':'+http_port);
					logger.log(''+ex);
					//flag_init_ok=false;
				}
			}

			////////////////////////////////////////////////////////// HTTPS
			var https_host=argo.https_host||'0.0.0.0',https_port=argo.https_port;
			if(https_port){
				if(!appModule.handleHttps) throw new Exception('appModule.handleHttps is not defined.');
				var https_key=argo.https_key;
				var https_cert=argo.https_cert;
				const options = {
					key: fs.readFileSync(https_key),
					cert: fs.readFileSync(https_cert)
				};
				argo.https_server=require('https').createServer(appModule.handleHttps);//let the internal logic can access
				try{
					argo.https_server.listen(https_host,https_host,()=>{logger.log('https listen on ',https_host,':',https_port)});
					flag_init_ok=true;
					rt.flag_https=true;
				}catch(ex){
					logger.log('failed to start https_server on '+https_host+':'+https_port);
					logger.log(''+ex);
					//flag_init_ok=false;
				}
			}

			////////////////////////////////////////////////////////// WEBSOCKET
			var ws_port=argo.ws_port,ws_host=argo.ws_host||'0.0.0.0';
			if(ws_port){
				var _client_conn_a={};//buffer of conn-s
				try{
					var ws = require("nodejs-websocket");
					if(!ws){
						logger.log("nodejs-websocket module needed");
						process.exit(2);
					}
					if(ws_port>1024){
					}else{
						logger.log("port < 1024?");
						//process.exit(3);//NOTES: outside sh caller will not handle for case 3
					}
					logger.log("pid=",process.pid);
					ws.setMaxBufferLength(20971520);
					var ws_opts={};
					if (argo.ws_secure) ws_opts.secure=true;
					var ws_server = argo.ws_server = ws.createServer(ws_opts);
					ws_server.on('connection',function(conn){
						var _addr=(conn.socket.remoteAddress);
						var _port=(conn.socket.remotePort);
						var _key=""+_addr+":"+_port;
						logger.log("on conn "+_key);
						conn.key=_key;//用IP加PORT的方法来识别每个conn...
						_client_conn_a[_key]=conn;
						conn.on("error", function (e){
							logger.log("ws_server.conn.error",e);
						});
						conn.on("text", function (data_s){
							logger.log("on text",data_s);
							if(!appModule.handleWebSocket) throw new Exception('appModule.handleWebSocket is not defined.');
							appModule.handleWebSocket(data_s,conn);
						});
						conn.on("close", function (code, reason){
							logger.log("ws_server.close="+code+","+reason,"key="+ws_server.key);
							_client_conn_a[_key]=null;
							delete _client_conn_a[_key];
						});
					});
					ws_server.on('error', function(e){
						logger.log("ws_server.error",e);
						if (e.code == 'EADDRINUSE'){
							logger.log('Address in use');
							process.exit(3);
						}
					});
					logger.log("ws listen on "+ws_port);
					try{
						ws_server.listen(ws_port);
						flag_init_ok=true;
						rt.flag_ws=true;
					}catch(ex){
						logger.log('failed to start ws_server on '+ws_host+':'+ws_port);
						logger.log(''+ex);
						//flag_init_ok=false;
					}
				}catch(ex){
					logger.log("ws.ex=",ex);
				}
			}
			process.on('uncaughtException', err=>{
				appModule.handleUncaughtException(err);
			});
			process.on("exit",function(i){
				if(appModule.handleExit){
					appModule.handleExit();
				}else{
					logger.log('process.on.exit',i);
				}
			});
			process.on('SIGINT', function(){
				logger.log('!!!! process.on("SIGINT")');
				if(appModule.handleSIGINT){
					appModule.handleSIGINT();
				}else{
					logger.log('SIGINT');
				}
			});
			process.on('SIGTERM', function(){
				logger.log('!!!! process.on("SIGTERM")');
				if(appModule.handleSIGTERM){
					appModule.handleSIGTERM();
				}else{
					logger.log('SIGTERM');
				}
			});
			if(flag_init_ok){
			}else{
				//logger.log("init failed.  missing -ws_port or -server_port or -https_port ?");
				//process.exit(4);
				rt.STS='KO';
				rt.errmsg='init failed,missing -ws_port or -server_port or -https_port ?';
			}
		}//!flag_init_ok
		return Q(rt);
	}//daemon()
};
