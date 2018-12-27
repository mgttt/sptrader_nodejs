//wrapper child proc for the .node addon

const os=require('os');
var ver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
var avatar= require('./SpTrader.'+ver+'.node');
send_parent=(responseId,responseData)=>process.send({responseId,responseData});
avatar._on( ({data,on,seq}={}) => send_parent(on,data) );
process.on('message',({callbackId,m,p}={})=>avatar._call(m,p||{},
	responseData=>setTimeout(()=>send_parent(callbackId,responseData),2)
));


//const logger=console;
//var ver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
//logger.log('ver=',ver);
//avatar._on((o={})=>{
//	logger.log("DEBUG _on",o);
//	var {data,on,seq} = o||{};
//	if(on)send_parent(on,data);
//	else logger.log('unhandle _on()',o);
//});
//avatar._on(o=>{setTimeout(()=>{
//	var {data,on,seq} = o||{};
//	if(on){
//		send_parent(on,data);
//		logger.log("DEBUG send_parent",on,data);
//	}
//	else logger.log('unhandle _on()',o);
//},2);});
//process.on('message',msg=>{
//	var {callbackId,m,p}=(msg||{});
//	if(avatar._call && m && callbackId){
//		return avatar._call(m,p||{},
//			responseData=>setTimeout(()=>send_parent(callbackId,responseData),2)
//		);			
//	}
//	logger.log("unhandle .msg=",msg);
//});
