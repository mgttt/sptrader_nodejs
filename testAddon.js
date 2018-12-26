//var q_sptrader = require('./q_sptrader')();

// fwd msg from q_sptrader to parent...
const logger=console;

send_parent = (m,p)=>{
	process.send({m,p});
};

tmp_loop = ()=>{
	var pid = process.pid;
	logger.log("child.pid=",pid);

	//TMP
	send_parent('checkin',new Date());

	setTimeout(()=>{
		tmp_loop()
	},7777);
};

tmp_loop();
