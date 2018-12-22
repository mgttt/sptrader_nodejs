////NOTES:
//npm install -save nodenodenode moment-timezone q node-persist
//TODO make argo with http_port
require('./nodenodenode')
	.daemon({
		//app:__dirname+'/egapp.js',
		approot:__dirname,//need for using default app(egapp), then auto find the "app.js" in folder $approot ...
		http_port:4321,//先不用argo直接这里塞也可以的.
		ws_port:4333//npm install nodejs-websocket
	})
	.delay(444)
	.done(rst=>{
		console.log(rst);

		//https://github.com/sitegui/nodejs-websocket
		const ws= require('nodejs-websocket');
		/*
		 options is an object that will be passed to net.connect() (or tls.connect() if the protocol is "wss:"). The properties "host" and "port" will be read from the URL. The optional property extraHeaders will be used to add more headers to the HTTP handshake request. If present, it must be an object, like {'X-My-Header': 'value'}. The optional property protocols will be used in the handshake (as "Sec-WebSocket-Protocol" header) to allow the server to choose one of those values. If present, it must be an array of strings.
		 */
		var conn=ws.connect('ws://127.0.0.1:4333/', {}, function(){
			console.log("ws.callback")
			conn.sendText(JSON.stringify({m:'GetVersion'}));
		});
		conn.on("text", function (str) {
			console.log("Received "+str)
		});
		conn.on("close", function (code, reason) {
			console.log("Connection closed")
		});
		
		/*
		var {exec,execSync} = require('child_process');
		var cmd_s="curl http://127.0.0.1:4321/ -d \"{'m':'GetVersion'}\"";
		console.log(cmd_s);
		console.log(execSync(cmd_s).toString());
*/

		//var exec_o=exec(cmd_s);//,function(err,stdout,stderr){ if(err) { console.log('error:'+err); } console.log('stderr:'+stderr); console.log('stdout:'+stdout); }
		//exec_o.stdout.pipe(process.stdout);
		//exec_o.stderr.pipe(process.stderr);

		//https://github.com/websockets/ws
		//const WebSocket = require('ws');
		//const ws = new WebSocket('ws://www.host.com/path');
		//ws.on('open', function open() {
		//	ws.send('something');
		//});
		//ws.on('message', function incoming(data) {
		//	console.log(data);
		//});

	});
