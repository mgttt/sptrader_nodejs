#include <node.h>
#include "macromagic.h"

/* due to some pratise exp. a little hack is needed at the caller site to release the function hook, so that the multithreading is more fluent:

	(()=>{
		var os=require('os');
		var plugver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
		var rt= require('MyAddon.'+plugver+'.node');

		//a hack for the faint/uv/threading issues:
		rt.call=function(){
			setTimeout(()=>{
				this._call.apply(arguments);
			},1);
		};
		rt.on=function(evtName,cb){
			rt._on(evtName,function(rst){
				setTimeout(()=>{
					if(cb) cb(rst);
					else{
						logger.log("ERROR !!! handle evtName=",evtName,rst);
					}
				},1);
			});
		};
		return rt;
	})();

	*/
//define NODE_MODULE_NAME & NODE_MODULE_FUNC_LIST for node_addon.h:
#define NODE_MODULE_NAME SpTrader
#define NODE_MODULE_FUNC_LIST _on,_call

#include "node_addon.h"

//cleanup:
#undef NODE_MODULE_FUNC_LIST
#undef NODE_MODULE_NAME
