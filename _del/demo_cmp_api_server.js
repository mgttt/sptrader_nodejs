//NOTES
//the universal api server design by cmptech
//USAGE:require('./cmp_api_server.js').daemon();

var logger=console;//default logger
var cmp_api_server;
module.exports=cmp_api_server={
	argv2o:argv=>{
		var m,mm,rt={};
		for(k in argv)(m=(rt[""+k]=argv[k]).match(/^--?([a-zA-Z0-9-_]*)=(.*)/))&&(rt[m[1]]=(mm=m[2].match(/^".*"$/))?mm[1]:m[2]);
		return rt;
	}
	,daemon:argo=>{
		if(!argo) argo=cmp_api_server.argv2o(process.argv);

		process.env.UV_THREADPOOL_SIZE = argo.UV_THREADPOOL_SIZE || 126;//MAX=255, increase the thread pool for uv_queue_work()

		logger.log(process.env);
		logger.log("process.versions=",process.versions);

		if(!argo.app) throw new Error('-app is needed');
		var appModule=require(argo.app);

		////////////////////////////////////////////////////////// WebServer Version
		//NOTES:
		//server_host/h && server_port/p for web,
		//host/port is preserved for sptrader...
		var hhh=argo.server_host||argo.h||'0.0.0.0',ppp=argo.server_port||argo.p||(()=>{
			throw new Error("-server_port is mandatory")
		})();
		var http_server=require('http').createServer(appModule({argo}));
		argo.http_server=http_server;//let the internal logic can access
		http_server.listen(ppp,hhh,()=>{logger.log('web listen on ',hhh,':',ppp)});

		////////////////////////////////////////////////////////// WebSocketServer Version (TODO fwd basic call to webserver above...)
		var ws_port=argo.ws_port,ws_host=argo.ws_host||'0.0.0.0';
		if(ws_port){
			var _d_=logger.log;
			var _client_conn_a={};//buffer of conn
			try{
				var ws = require("nodejs-websocket");
				if(!ws){
					_d_("nodejs-websocket module needed");
					process.exit(2);
				}
				if(ws_port>1024){
				}else{
					_d_("port incorrect:"+ws_port);
					process.exit(3);//NOTES: outside sh caller will not handle for case 3
				}
				_d_("pid=",process.pid);
				//process.on("exit",function(){
				//	process.nextTick(function(){
				//		_d_('This should not run');
				//	});
				//	_d_('About to exit.');
				//});
				ws.setMaxBufferLength(20971520);
				var ws_server = ws.createServer(
					//{"secure":true},
				);
				ws_server.on('connection',function(conn){
					var _addr=(conn.socket.remoteAddress);
					var _port=(conn.socket.remotePort);
					var _key=""+_addr+":"+_port;
					_d_("on conn "+_key);
					conn.key=_key;//用IP加PORT的方法来识别每个conn
					_client_conn_a[_key]=conn;
					conn.on("error", function (e){
						_d_("ws_server.conn.error",e);
					});
					conn.on("text", function (data_s) {
						//TODO fwd logic to http server above in future !!!!
						//if cmp fwd to http server
						//else maybe web socket push...
						_d_("on text",data_s);
					});
					conn.on("close", function (code, reason){
						//clean up
						_client_conn_a[_key]=null;
						delete _client_conn_a[_key];
						_d_("ws_server.close="+code+","+reason,"key="+cmp_api_server.key);
					});
				});
				ws_server.on('error', function(e){
					_d_("ws_server.error",e);
					if (e.code == 'EADDRINUSE'){
						_d_('Address in use');
						process.exit(3);
					}
				});
				_d_(" listen on "+ws_port);

				argo.ws_server=ws_server;//websocket server hook

				ws_server.listen(ws_port);
			}catch(ex){
				_d_("ws.ex=",ex);
			}
		}
		//process.on('SIGINT', function() {
		//   //db.stop(function(err) {
		//   //  process.exit(err ? 1 : 0);
		//   //});
		//	process.exit(0);
		//});
	}
};
