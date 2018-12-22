//e.g.:
//LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node test_sptrader.js
//or
//docker run -v"$PWD:/sptrader" -ti -w/sptrader cmptech/auto_ubuntu1610_nodejs_sharessl sh -c "LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so /node-v7.7.4/bin/node test_sptrader.js

var logger=console;

function getSptraderModule(){
	var os=require('os');
	var plugver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
	return require('./SpTrader.'+plugver+'.node');
}

const sptrader=getSptraderModule();

console.log(sptrader.SPAPI_Initialize());

console.log(sptrader.SPAPI_GetDllVersion());

