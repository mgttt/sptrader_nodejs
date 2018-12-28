//wrap child proc for the .node addon
const os=require('os');
var ver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
var avatar= require('./SpTrader.'+ver+'.node');
send_parent=(responseId,responseData)=>process.send({responseId,responseData});
avatar._on( ({data,on,seq}={}) => send_parent(on,data) );
process.on('message',({callbackId,m,p}={})=>avatar._call(m,p||{},
	responseData=>setTimeout(()=>send_parent(callbackId,responseData),2)
));
