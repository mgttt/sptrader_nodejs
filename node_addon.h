//#include "node_addon.h"
//@note: macro for building a nodejs addon
//@ref: https://nodejs.org/api/addons.html

#define EXPORT_NODE_METHOD(method) NODE_SET_METHOD(exports,#method,method);

#define DEFINE_NODE_METHOD($method) \
	void $method(const v8::FunctionCallbackInfo<v8::Value>& args){\
		_myLogic.$method(args);\
	}

#define MY_NODE_MODULE($mdlname,$LogicClass,...)\
	namespace $mdlname {\
		$LogicClass _myLogic;\
		ITR(DEFINE_NODE_METHOD,__VA_ARGS__);\
		void init(v8::Local<v8::Object> exports){\
			ITR(EXPORT_NODE_METHOD,__VA_ARGS__);\
		}\
		NODE_MODULE($mdlname, init)\
	}

#define QUOTEME2(aaa,bbb) QUOTEME(aaa##bbb)
#define LOGICFILE(mmm) QUOTEME2(mmm,Logic.hpp)

#ifdef NODE_MODULE_NAME

#define NODE_MODULE_LOGIC CAT(NODE_MODULE_NAME,Logic)
#define NODE_MODULE_LOGIC_CLASS() NODE_MODULE_LOGIC

//include {$NODE_MODULE_NAME}Login.hpp:
//#include LOGICFILE(NODE_MODULE_NAME)
#include LOGICFILE(Addon)

//bind method list to "exports"(nodejs module inner thing)
MY_NODE_MODULE(NODE_MODULE_NAME,NODE_MODULE_LOGIC,EXPAND(NODE_MODULE_FUNC_LIST));
#undef NODE_MODULE_LOGIC_CLASS
#undef NODE_MODULE_LOGIC

#endif
