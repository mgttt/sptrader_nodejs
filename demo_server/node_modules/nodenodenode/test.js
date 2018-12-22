////NOTES:
//npm install -save nodenodenode moment-timezone q node-persist
//TODO make argo with http_port
require('nodenodenode')
	.daemon({
		//app:__dirname+'/egapp.js',
		approot:__dirname,//need for using default app(egapp), then auto find the "app.js" in folder $approot ...
		http_port:4321,//先不用argo直接这里塞也可以的.
	})
	.delay(444)
	.done(rst=>{
		console.log(rst);
		var {exec,execSync} = require('child_process');
		var cmd_s="curl http://127.0.0.1:4321/ -d \"{'m':'GetVersion'}\"";
		console.log(cmd_s);
		console.log(execSync(cmd_s).toString());
		//var exec_o=exec(cmd_s);//,function(err,stdout,stderr){ if(err) { console.log('error:'+err); } console.log('stderr:'+stderr); console.log('stdout:'+stdout); }
		//exec_o.stdout.pipe(process.stdout);
		//exec_o.stderr.pipe(process.stderr);
	});
