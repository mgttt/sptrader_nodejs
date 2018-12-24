#include "include/ApiProxyWrapper.h"
#include "include/ApiProxyWrapperReply.h"
#include "macromagic.h"
#include <v8.h>
#include "ApiProxyWrapperTypes.h"
#define EXPORT_DECLARE(fff) void fff(const v8::FunctionCallbackInfo<v8::Value>& args);
class NODE_MODULE_LOGIC : public ApiProxyWrapperReply
{
	public:
		NODE_MODULE_LOGIC(void);
		~NODE_MODULE_LOGIC(void);

		ITR(EXPORT_DECLARE,EXPAND(NODE_MODULE_FUNC_LIST));//declare the module methods iterally:
		//0
		void OnTest();
		//1
		void OnLoginReply(long ret_code,char *ret_msg);
		//2
		void OnPswChangeReply(long ret_code, char *ret_msg);
		//3
		void OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg);
		//4
		void OnApiOrderBeforeSendReport(const SPApiOrder *order);
		//5
		void OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg);
		//6
		void OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order);
		//7
		void OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty);
		//8
		void OnApiTradeReport(long rec_no, const SPApiTrade *trade);
		//9
		void OnApiLoadTradeReadyPush(long rec_no, const SPApiTrade *trade);
		//10
		void OnApiPriceUpdate(const SPApiPrice *price);
		//11
		void OnApiTickerUpdate(const SPApiTicker *ticker);
		//12
		void OnApiOrderReport(long rec_no, const SPApiOrder *order);
		//13
		void OnInstrumentListReply(bool is_ready, char *ret_msg);
		//14
		void OnBusinessDateReply(long business_date);
		//15
		void OnConnectedReply(long host_type, long con_status);
		//16
		void OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg);
		//17
		void OnAccountLogoutReply(long ret_code, char* ret_msg);
		//18
		void OnAccountInfoPush(const SPApiAccInfo *acc_info);
		//19
		void OnAccountPositionPush(const SPApiPos *pos);
		//20
		void OnUpdatedAccountPositionPush(const SPApiPos *pos);
		//21
		void OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal);
		//22
		void OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg);
		//23
		void OnApiAccountControlReply(long ret_code, char *ret_msg);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <uv.h>
#include "uv_callback.hpp"
uv_callback_t _uv_callback_on_ptr;
int dynamic_call_counter = 0;
#include "json.hpp" //https://github.com/nlohmann/json/releases/download/v2.1.1/json.hpp
using json = nlohmann::json;
#include <iostream> //for cout << .... << endl
#include <map>
#include <queue>
#include <mutex>
#include <climits> //for INT_MAX
using namespace std;//for string
template<typename T>
class mutex_queue
{
	private:
		std::queue<T> m_queque;
		mutable std::mutex m_mutex;
	public:
		void push( const T value )
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queque.push(value);
		}
		T pop()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			T rt;
			if(!m_queque.empty()){
				rt = m_queque.front();
				m_queque.pop();
			}
			return rt;
		}
};
static mutex_queue<json> _callback_queue;
ApiProxyWrapper apiProxyWrapper;
#include <iconv.h> //for gbk/big5/utf8
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;
	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen) == -1) return -1;
	iconv_close(cd);
	return 0;
}
std::string any2utf8(std::string in,std::string fromEncode,std::string toEncode)
{
	char* inbuf=(char*) in.c_str();
	int inlen=strlen(inbuf);
	int outlen=inlen*3;//in case unicode 3 times than ascii
	char outbuf[outlen]={0};
	int rst=code_convert((char*)fromEncode.c_str(),(char*)toEncode.c_str(),inbuf,inlen,outbuf,outlen);
	if(rst==0){ return std::string(outbuf);
	}else{ return in; }
}
std::string gbk2utf8(const char* in) { return any2utf8(std::string(in),std::string("gbk"),std::string("utf-8")); }
std::string big2utf8(const char* in) { return any2utf8(std::string(in),std::string("big5"),std::string("utf-8")); }
//map<string, v8::Persistent<v8::Function> > _callback_map;
v8::Persistent<v8::Function> _callback_for_on;
struct MyUvShareData
{
	int seq=-99;//for tracing sequence
	string api;//the api name
	string out_s;
	json in;
	json out;
	json rst;
	v8::Persistent<v8::Function> callback;
	int rc=-99;
};

//http://stackoverflow.com/questions/34356686/how-to-convert-v8string-to-const-char
//char* V8_To_c_str(const v8::String::Utf8Value& value){
//	char* rt=(char*) (*value ? *value : "<string conversion failed>");
//	return rt;
//}
//https://github.com/pmed/v8pp/blob/2e0c25ebe6f478bc4ab706d6878c6b6451ba1c7e/v8pp/json.hpp

inline std::string json_stringify(v8::Isolate* isolate, v8::Handle<v8::Value> value)
{
	if (value.IsEmpty()) { return std::string("null"); }//patch wjc
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::Object> theJSON = isolate->GetCurrentContext()->
		Global()->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
	v8::Local<v8::Function> stringify = theJSON->Get(
			v8::String::NewFromUtf8(isolate, "stringify")).As<v8::Function>();
	v8::Local<v8::Value> result = stringify->Call(theJSON, 1, &value);
	v8::String::Utf8Value const str(result);
	std::string rt=std::string(*str, str.length());
	if(rt=="undefined") return std::string("null");//patch by wjc
	return rt;
}

inline v8::Handle<v8::Value> json_parse(v8::Isolate* isolate, std::string const& str)
{
	if (str.empty()) { return v8::Handle<v8::Value>(); }
	v8::EscapableHandleScope scope(isolate);
	v8::Local<v8::Object> theJSON = isolate->GetCurrentContext()->
		Global()->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
	v8::Local<v8::Function> parse = theJSON->Get(
			v8::String::NewFromUtf8(isolate, "parse")).As<v8::Function>();
	v8::Local<v8::Value> value = v8::String::NewFromUtf8(isolate, str.data(),
			v8::String::kNormalString, static_cast<int>(str.size()));

	//NOTES: TODO try-catch replace in new nodejs....
//TODO if NODE<10...
	v8::TryCatch try_catch;
	v8::Local<v8::Value> result = parse->Call(theJSON, 1, &value);
//TODO if NODE<10...
	if (try_catch.HasCaught()) { result = try_catch.Exception(); }

	return scope.Escape(result);
}

#include <atomic>
std::atomic<int> seq_count(0);
void * after_worker_for_on_q(uv_callback_t *callback, void *data)
{
	cout << " ";
	bool f_continue=false;
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate);
	do{
		json qi=_callback_queue.pop();
		f_continue=qi.is_null() ? false: true;
		if(f_continue){
			if(!_callback_for_on.IsEmpty()){
				v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, _callback_for_on);
				const unsigned argc = 1;

				//prev:
				//v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(v8::String::NewFromUtf8(isolate,qi.dump().c_str()))};

				//v8::Local<v8::Context> contextA = v8::Context::New(isolate);
				//v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(contextA,v8::String::NewFromUtf8(isolate,qi.dump().c_str())).ToLocalChecked()};

				v8::Local<v8::Value> argv[argc]={json_parse(isolate,qi.dump().c_str())};

				callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
			}
			qi=NULL;
			//}
		}else{
			qi=NULL;
		}
	}while(f_continue);
}
//conert v8 string to char* (for sptrader api)
inline void V8ToCharPtr(const v8::Local<v8::Value>& v8v, char* rt){
	const v8::String::Utf8Value value(v8v);
	const char* rt0=(*value ? *value : "<string conversion failed>");
	strcpy(rt,rt0);
}

#define ASYNC_CALLBACK_FOR_ON_Q($callbackName,$jsonData)\
	int seq=(++seq_count);\
	if(seq > INT_MAX - 1000) seq_count=0;\
	json qi;\
	qi["on"]=#$callbackName;\
	qi["data"]=$jsonData;\
	qi["seq"]=seq;\
	_callback_queue.push(qi);\
	qi=NULL;\
	$jsonData=NULL;\
	uv_callback_fire(&_uv_callback_on_ptr, NULL, NULL);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NODE_MODULE_LOGIC::NODE_MODULE_LOGIC(void){
	uv_callback_init(uv_default_loop(), &_uv_callback_on_ptr, after_worker_for_on_q, UV_DEFAULT);
	apiProxyWrapper.SPAPI_Initialize();//1.1
	apiProxyWrapper.SPAPI_RegisterApiProxyWrapperReply(this);
}
NODE_MODULE_LOGIC::~NODE_MODULE_LOGIC(void){}
//0
void NODE_MODULE_LOGIC::OnTest()
{
	json j;
	ASYNC_CALLBACK_FOR_ON_Q(Test,j);
}
//1
void NODE_MODULE_LOGIC::OnLoginReply(long ret_code,char *ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;//strange...
	j["ret_msg"]=string(ret_msg);
	//j["ret_msg"]=big2utf8((const char*)ret_msg);//TEST FAILED...
	ASYNC_CALLBACK_FOR_ON_Q(LoginReply,j);
}
//2
void NODE_MODULE_LOGIC::OnPswChangeReply(long ret_code, char *ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALLBACK_FOR_ON_Q(ApiTickerUpdate,j);
}
//3
void NODE_MODULE_LOGIC::OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg)
{
	json j;
	j["action"]=action;
	j["err_code"]=err_code;
	//j["err_msg"]=err_msg;
	j["err_msg"]=string(err_msg);
	if(NULL!=order) COPY_TO_JSON(SPApiOrder,(*order),j["order"]);
	ASYNC_CALLBACK_FOR_ON_Q(OrderRequestFailed,j);
}
//4
void NODE_MODULE_LOGIC::OnApiOrderBeforeSendReport(const SPApiOrder *order)
{
	json j;
	j["rec_no"]=0;
	if(NULL!=order) COPY_TO_JSON(SPApiOrder,(*order),j["order"]);
	ASYNC_CALLBACK_FOR_ON_Q(OrderBeforeSendReport,j);
}
//5 SPAPI_RegisterMMOrderRequestFailed
void NODE_MODULE_LOGIC::OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg)
{
	json j;
	j["err_code"]=err_code;
	//j["err_msg"]=err_msg;
	j["err_msg"]=string(err_msg);
	if(NULL!=mm_order) COPY_TO_JSON(SPApiMMOrder,(*mm_order),j["mm_order"]);
	ASYNC_CALLBACK_FOR_ON_Q(MMOrderRequestFailed,j);
}
//6
void NODE_MODULE_LOGIC::OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order)
{
	json j;
	if(NULL!=mm_order) COPY_TO_JSON(SPApiMMOrder,(*mm_order),j["mm_order"]);
	ASYNC_CALLBACK_FOR_ON_Q(MMOrderBeforeSendReport,j);
}
//7.SPAPI_RegisterQuoteRequestReceivedReport
void NODE_MODULE_LOGIC::OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty)
{
	json j;
	j["product_code"]=product_code;
	//j["product_code"]=string(product_code);
	j["buy_sell"]=buy_sell;
	j["qty"]=qty;
	ASYNC_CALLBACK_FOR_ON_Q(QuoteRequestReceived,j);
}
//8
void NODE_MODULE_LOGIC::OnApiTradeReport(long rec_no, const SPApiTrade *trade)
{
	json j;
	if(NULL!=trade) COPY_TO_JSON(SPApiTrade,(*trade),j["trade"]);
	ASYNC_CALLBACK_FOR_ON_Q(TradeReport,j);
}
//9
void NODE_MODULE_LOGIC::OnApiLoadTradeReadyPush(long rec_no, const SPApiTrade *trade)
{
	json j;
	if(NULL!=trade) COPY_TO_JSON(SPApiTrade,(*trade),j["trade"]);
	ASYNC_CALLBACK_FOR_ON_Q(LoadTradeReadyPush,j);
}
//10
void NODE_MODULE_LOGIC::OnApiPriceUpdate(const SPApiPrice *price)
{
	json j;
	if(NULL!=price) COPY_TO_JSON(SPApiPrice,(*price),j["price"]);
	ASYNC_CALLBACK_FOR_ON_Q(PriceReport,j);
}
//11
void NODE_MODULE_LOGIC::OnApiTickerUpdate(const SPApiTicker *ticker)
{
	json j;
	if(NULL!=ticker) COPY_TO_JSON(SPApiTicker,(*ticker),j["ticker"]);
	ASYNC_CALLBACK_FOR_ON_Q(TickerUpdate,j);
}
//12
void NODE_MODULE_LOGIC::OnApiOrderReport(long rec_no, const SPApiOrder *order)
{
	json j;
	j["rec_no"]=rec_no;
	if(NULL!=order) COPY_TO_JSON(SPApiOrder,(*order),j["order"]);
	ASYNC_CALLBACK_FOR_ON_Q(OrderReport,j);
}
//13
void NODE_MODULE_LOGIC::OnInstrumentListReply(bool is_ready, char *ret_msg)
{
	printf("\nInstrument Ready:%s. Ret Msg:%s\n",is_ready?"Ok":"No", ret_msg);
	json j;
	j["is_ready"]=is_ready;
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALLBACK_FOR_ON_Q(InstrumentListReply,j);
}
//14
void NODE_MODULE_LOGIC::OnBusinessDateReply(long business_date)
{
	json j;
	j["business_date"]=business_date;
	ASYNC_CALLBACK_FOR_ON_Q(BusinessDateReply,j);
}
//15
void NODE_MODULE_LOGIC::OnConnectedReply(long host_type, long conn_status)
{
	json j;
	j["host_type"]=host_type;
	j["conn_status"]=conn_status;
	ASYNC_CALLBACK_FOR_ON_Q(ConnectedReply,j);
}
//16
void NODE_MODULE_LOGIC::OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg)
{
	json j;
	j["accNo"]=accNo;
	//j["accNo"]=string(accNo);
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON_Q(AccountLoginReply,j);
}
//17
void NODE_MODULE_LOGIC::OnAccountLogoutReply(long ret_code, char* ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON_Q(AccountLogoutReply,j);
}
//18
void NODE_MODULE_LOGIC::OnAccountInfoPush(const SPApiAccInfo *acc_info)
{
	json j;
	if(NULL!=acc_info) COPY_TO_JSON(SPApiAccInfo,(*acc_info),j["acc_info"]);
	ASYNC_CALLBACK_FOR_ON_Q(AccountInfoPush,j);
}
//19
void NODE_MODULE_LOGIC::OnAccountPositionPush(const SPApiPos *pos)
{
	json j;
	if(NULL!=pos) COPY_TO_JSON(SPApiPos,(*pos),j["pos"]);
	ASYNC_CALLBACK_FOR_ON_Q(AccountPositionPush,j);
}
//20
void NODE_MODULE_LOGIC::OnUpdatedAccountPositionPush(const SPApiPos *pos)
{
	json j;
	if(NULL!=pos) COPY_TO_JSON(SPApiPos,(*pos),j["pos"]);
	ASYNC_CALLBACK_FOR_ON_Q(UpdatedAccountPositionPush,j);
}
//21
void NODE_MODULE_LOGIC::OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal)
{
	json j;
	if(NULL!=acc_bal) COPY_TO_JSON(SPApiAccBal,(*acc_bal),j["acc_bal"]);
	ASYNC_CALLBACK_FOR_ON_Q(UpdatedAccountBalancePush,j);
}
//22
void NODE_MODULE_LOGIC::OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg)
{
	json j;
	j["inst_code"]=inst_code;
	//j["inst_code"]=string(inst_code);
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
	j["is_ready"]=is_ready;
	ASYNC_CALLBACK_FOR_ON_Q(ProductListByCodeReply,j);
}
//23
void NODE_MODULE_LOGIC::OnApiAccountControlReply(long ret_code, char *ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALLBACK_FOR_ON_Q(AccountControlReply,j);
}
//1.3
inline void SPAPI_SetLanguageId(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_INT(in["langid"],langid);
	apiProxyWrapper.SPAPI_SetLanguageId(langid);
	my_data->rc =0;
}
//1.4
inline void SPAPI_SetLoginInfo(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["host"],host);
	COPY_TO_INT(in["port"],port);
	COPY_TO_STR(in["license"],license);
	COPY_TO_STR(in["app_id"],app_id);
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["password"],password);
	apiProxyWrapper.SPAPI_SetLoginInfo(host, port, license, app_id, user_id, password);
	my_data->rc =0;
}
//1.5
inline void SPAPI_Login(MyUvShareData * my_data){
	my_data->rc = apiProxyWrapper.SPAPI_Login();
}
//1.6
inline void SPAPI_Logout(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	my_data->rc = apiProxyWrapper.SPAPI_Logout(user_id);
}
//1.7
inline void SPAPI_ChangePassword(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["old_psw"],old_psw);
	COPY_TO_STR(in["new_psw"],new_psw);
	my_data->rc = apiProxyWrapper.SPAPI_ChangePassword(user_id,old_psw,new_psw);
}
//1.8
inline void SPAPI_GetLoginStatus(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_INT(in["host_id"],host_id);
	my_data->rc = apiProxyWrapper.SPAPI_GetLoginStatus(user_id,host_id);
}
//1.9
inline void SPAPI_AddOrder(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STRUCT(SPApiOrder,in["order"],order);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_AddOrder(&order);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiOrder,order,out);
		my_data->out=out;
	}
}
//1.10
inline void SPAPI_AddInactiveOrder(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STRUCT(SPApiOrder,in["order"],order);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_AddInactiveOrder(&order);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiOrder,order,out);
		my_data->out=out;
	}
}
//1.11
inline void SPAPI_ChangeOrder(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_DBL(in["org_price"],org_price);
	COPY_TO_LNG(in["org_qty"],org_qty);
	COPY_TO_STRUCT(SPApiOrder,in["order"],order);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_ChangeOrder(user_id,&order,org_price,org_qty);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiOrder,order,out);
		my_data->out=out;
	}
}
//1.12
inline void SPAPI_ChangeOrderBy(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	COPY_TO_DBL(in["org_price"],org_price);
	COPY_TO_LNG(in["org_qty"],org_qty);
	COPY_TO_DBL(in["newPrice"],newPrice);
	COPY_TO_LNG(in["newQty"],newQty);
	my_data->rc = apiProxyWrapper.SPAPI_ChangeOrderBy(user_id,acc_no,accOrderNo,org_price,org_qty,newPrice,newQty);
}
//1.13
inline void SPAPI_GetOrderByOrderNo(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["int_order_no"],int_order_no);
	//COPY_TO_STRUCT(SPApiOrder,in["order"],order);
	SPApiOrder order={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetOrderByOrderNo(user_id,acc_no,int_order_no,&order);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiOrder,order,out);
		my_data->out=out;
	}
}
//1.14
inline void SPAPI_GetOrderCount(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_GetOrderCount(user_id,acc_no);
}
//1.15
inline void SPAPI_GetActiveOrders(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	vector<SPApiOrder> apiOrderList;
	//int SPAPI_GetActiveOrders(char *user_id, char *acc_no, vector<SPApiOrder>& apiOrderList);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetActiveOrders(user_id,acc_no,apiOrderList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiOrder,apiOrderList,out);
		if(apiOrderList_size>0){
			my_data->out=out;
		}
	}
}
//1.16 Just Example which don't use! 
//inline void SPAPI_GetOrdersByArray(MyUvShareData * my_data){
//	json in=my_data->in;
//	COPY_TO_STR(in["user_id"],user_id);
//	COPY_TO_STR(in["acc_no"],acc_no);
//	int count =  apiProxyWrapper.SPAPI_GetOrderCount(user_id,acc_no);
//	SPApiOrder * apiOrderList = (SPApiOrder *)malloc(count * sizeof(apiOrderList));
//	//int SPAPI_GetOrdersByArray(char *user_id, char *acc_no,SPApiOrder* apiOrderList)
//	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetOrdersByArray(user_id,acc_no,apiOrderList);
//	if(rc==0){
//		json out;
//		for (int i = 0; i < count; i++) {
//			SPApiOrder& order = apiOrderList[i];
//			COPY_TO_JSON(SPApiOrder,order,out[i]);
//		}
//		free(apiOrderList);
//		my_data->out=out;
//	}
//}
//1.17
inline void SPAPI_DeleteOrderBy(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	COPY_TO_STR(in["productCode"],productCode);
	COPY_TO_STR(in["clOrderId"],clOrderId);
	my_data->rc = apiProxyWrapper.SPAPI_DeleteOrderBy(user_id,acc_no,accOrderNo,productCode,clOrderId);
}
//1.18
inline void SPAPI_DeleteAllOrders(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_DeleteAllOrders(user_id,acc_no);
}
//1.19
inline void SPAPI_ActivateOrderBy(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	my_data->rc = apiProxyWrapper.SPAPI_ActivateOrderBy(user_id,acc_no,accOrderNo);
}
//1.20
inline void SPAPI_ActivateAllOrders(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_ActivateAllOrders(user_id,acc_no);
}
//1.21
inline void SPAPI_InactivateOrderBy(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	my_data->rc = apiProxyWrapper.SPAPI_InactivateOrderBy(user_id,acc_no,accOrderNo);
}
//1.22
inline void SPAPI_InactivateAllOrders(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_InactivateAllOrders(user_id,acc_no);
}
//1.23
inline void SPAPI_SendMarketMakingOrder(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STRUCT(SPApiMMOrder,in["mmorder"],mmorder);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_SendMarketMakingOrder(user_id,&mmorder);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiMMOrder,mmorder,out);
		my_data->out=out;
	}
}
//1.24
inline void SPAPI_GetPosCount(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	my_data->rc = apiProxyWrapper.SPAPI_GetPosCount(user_id);
}
//1.25
inline void SPAPI_GetAllPos(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	vector<SPApiPos> apiPosList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAllPos(user_id,apiPosList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiPos,apiPosList,out);
		if(apiPosList_size>0){
			my_data->out=out;
		}
	}
}
//1.27
inline void SPAPI_GetPosByProduct(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	SPApiPos pos={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetPosByProduct(user_id,prod_code,&pos);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiPos,pos,out);
		my_data->out=out;
	}
}
//1.28
inline void SPAPI_GetTradeCount(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_GetTradeCount(user_id,acc_no);
}
//1.29
inline void SPAPI_GetAllTrades(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	vector<SPApiTrade> apiTradeList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAllTrades(user_id,acc_no,apiTradeList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiTrade,apiTradeList,out);
		if(apiTradeList_size>0){
			my_data->out=out;
		}
	}
}
//1.31
inline void SPAPI_SubscribePrice(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	COPY_TO_INT(in["mode"],mode);
	my_data->rc = apiProxyWrapper.SPAPI_SubscribePrice(user_id,prod_code,mode);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
}
//1.32
inline void SPAPI_GetPriceByCode(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	SPApiPrice price={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetPriceByCode(user_id,prod_code,&price);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiPrice,price,out);
		my_data->out=out;
	}
}
//1.33
inline void SPAPI_LoadInstrumentList(MyUvShareData * my_data){
	my_data->rc = apiProxyWrapper.SPAPI_LoadInstrumentList();
}
//1.34
inline void SPAPI_GetInstrumentCount(MyUvShareData * my_data){
	my_data->rc = apiProxyWrapper.SPAPI_GetInstrumentCount();
}
//1.35
inline void SPAPI_GetInstrument(MyUvShareData * my_data){
	json in=my_data->in;
	vector<SPApiInstrument> apiInstList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetInstrument(apiInstList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiInstrument,apiInstList,out);
		if(apiInstList_size>0){
			my_data->out=out;
		}
	}
}
//1.37
inline void SPAPI_GetInstrumentByCode(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["inst_code"],inst_code);
	SPApiInstrument inst={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetInstrumentByCode(inst_code, &inst);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiInstrument,inst,out);
		my_data->out=out;
	}
}
//1.38
inline void SPAPI_GetProductCount(MyUvShareData * my_data){
	json in=my_data->in;
	my_data->rc = apiProxyWrapper.SPAPI_GetProductCount();
}
//1.39
inline void SPAPI_GetProduct(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["inst_code"],inst_code);
	vector<SPApiProduct> apiProdList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetProduct(apiProdList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiProduct,apiProdList,out);
		if(apiProdList_size>0){
			my_data->out=out;
		}
	}
}
//1.41
inline void SPAPI_GetProductByCode(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["prod_code"],prod_code);
	SPApiProduct prod={0};
	int rc=my_data->rc = apiProxyWrapper.SPAPI_GetProductByCode(prod_code,&prod);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
	if (rc == 0){
		json out;
		COPY_TO_JSON(SPApiProduct,prod,out);
		my_data->out=out;
	}
}
//1.42
inline void SPAPI_GetAccBalCount(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	my_data->rc = apiProxyWrapper.SPAPI_GetAccBalCount(user_id);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
}
//1.43
inline void SPAPI_GetAllAccBal(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	vector<SPApiAccBal> apiAccBalList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAllAccBal(user_id,apiAccBalList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiAccBal,apiAccBalList,out);
		if(apiAccBalList_size>0){
			my_data->out=out;
		}
	}
}
//1.45
inline void SPAPI_GetAccBalByCurrency(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["ccy"],ccy);
	SPApiAccBal acc_bal={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAccBalByCurrency(user_id,ccy,&acc_bal);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiAccBal,acc_bal,out);
		my_data->out=out;
	}
}
//1.46
inline void SPAPI_SubscribeTicker(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	COPY_TO_INT(in["mode"],mode);
	int rc= my_data->rc = apiProxyWrapper.SPAPI_SubscribeTicker(user_id, prod_code, mode);
}
//1.47
inline void SPAPI_SubscribeQuoteRequest(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	COPY_TO_INT(in["mode"],mode);
	my_data->rc = apiProxyWrapper.SPAPI_SubscribeQuoteRequest(user_id, prod_code, mode);
}
//1.48
inline void SPAPI_SubscribeAllQuoteRequest(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_INT(in["mode"],mode);
	my_data->rc = apiProxyWrapper.SPAPI_SubscribeAllQuoteRequest(user_id, mode);
}
//1.49
inline void SPAPI_GetAccInfo(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	SPApiAccInfo acc_info={0};
	int rc= my_data->rc = apiProxyWrapper.SPAPI_GetAccInfo(user_id, &acc_info);
	if (rc == 0){
		json out;
		COPY_TO_JSON(SPApiAccInfo,acc_info,out);
		my_data->out=out;
	}
}
//1.50
inline void SPAPI_GetDllVersion(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["ver_no"],ver_no);
	COPY_TO_STR(in["rel_no"],rel_no);
	COPY_TO_STR(in["suffix"],suffix);
	my_data->rc = apiProxyWrapper.SPAPI_GetDllVersion(ver_no, rel_no, suffix);
	json out;
	out["ver_no"]=ver_no;
	out["rel_no"]=rel_no;
	out["suffix"]=suffix;
	my_data->out=out;
}
//1.51
inline void SPAPI_LoadProductInfoListByCode(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["inst_code"],inst_code);
	my_data->rc = apiProxyWrapper.SPAPI_LoadProductInfoListByCode(inst_code);
}
//1.52
inline void SPAPI_SetApiLogPath(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["path"],path);
	my_data->rc = apiProxyWrapper.SPAPI_SetApiLogPath(path);
}
//1.53
inline void SPAPI_GetCcyRateByCcy(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["ccy"],ccy);
	double rate;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetCcyRateByCcy(user_id,ccy, rate);
	if(rc==0){
		json out;
		out["rate"]=rate;
		my_data->out=out;
	}
}
//1.54
inline void SPAPI_AccountLogin(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_AccountLogin(user_id,acc_no);
}
//1.55
inline void SPAPI_AccountLogout(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_AccountLogout(user_id,acc_no);
}
//1.56
inline void SPAPI_SendAccControl(MyUvShareData * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	char ctrl_mask=in["ctrl_mask"].get<char>();
	char ctrl_level=in["ctrl_level"].get<char>();
	my_data->rc = apiProxyWrapper.SPAPI_SendAccControl(user_id,acc_no,ctrl_mask,ctrl_level);
}
#define DFN_FNC_PTR(aaa) BRACKET_WRAP(#aaa,aaa),
std::map<std::string,void(*)(MyUvShareData*my_data)> _apiDict{
	ITR(DFN_FNC_PTR,EXPAND( //API 20161216:
				//SPAPI_Initialize,//1.1 ignore
				//SPAPI_Uninitialize,//1.2 ignore
				SPAPI_SetLanguageId,//1.3
				SPAPI_SetLoginInfo,//1.4
				SPAPI_Login,//1.5
				SPAPI_Logout,//1.6
				SPAPI_ChangePassword,//1.7
				SPAPI_GetLoginStatus,//1.8
				//下单相关：{
				SPAPI_AddOrder,//1.9
				SPAPI_AddInactiveOrder,//1.10
				SPAPI_ChangeOrder,//1.11
				SPAPI_ChangeOrderBy,//1.12
				SPAPI_GetOrderByOrderNo,//1.13
				SPAPI_GetOrderCount,//1.14 use with SPAPI_GetOrdersByArray
				SPAPI_GetActiveOrders,//1.15
				//SPAPI_GetOrdersByArray,//1.16 作用不大，仅做为ByArray类的用法的e.g.
				SPAPI_DeleteOrderBy,//1.17
				SPAPI_DeleteAllOrders,//1.18 
				SPAPI_ActivateOrderBy,//1.19
				SPAPI_ActivateAllOrders,//1.20 
				SPAPI_InactivateOrderBy,//1.21 
				SPAPI_InactivateAllOrders,//1.22
				SPAPI_SendMarketMakingOrder,//1.23
				//下单相关：}
				//持仓相关：{
				SPAPI_GetPosCount,//1.24 use with SPAPI_GetAllPosByArray
				SPAPI_GetAllPos,//1.25
				//SPAPI_GetAllPosByArray,//1.26 暂时没用
				SPAPI_GetPosByProduct,//1.27
				//持仓相关：}
				//成交相关：{
				SPAPI_GetTradeCount,//1.28
				SPAPI_GetAllTrades,//1.29
				//SPAPI_GetAllTradeByArray,//1.30 暂时没用
				//成交相关：}
				//行情相关：{
				SPAPI_SubscribePrice,//1.31
				SPAPI_GetPriceByCode,//1.32 
				//行情相关：}
				//市场及产品相关{
				SPAPI_LoadInstrumentList,//1.33
				SPAPI_GetInstrumentCount,//1.34 作用不大 use with SPAPI_GetInstrumentByArray
				SPAPI_GetInstrument,//1.35 获得市场信息,Instrument其实跟市场差不多，估计用市场歧义多，所以他们用这个TERMS(Instrument)
				//SPAPI_GetInstrumentByArray,//1.36 暂时不需要
				SPAPI_GetInstrumentByCode,//1.37 暂时没用,后面可能需要更新单个市场信息设定时可能需要.
				SPAPI_GetProductCount,//1.38
				SPAPI_GetProduct,//1.39
				//SPAPI_GetProductByArray,//1.40 暂时没用
				SPAPI_GetProductByCode,//1.41
				//产品相关：}
				//以下为帐号及其它：
				SPAPI_GetAccBalCount,//1.42 //获取现金结余的数量
				SPAPI_GetAllAccBal,//1.43,注：此方法如果是AE登入需要AccountLogin一个客户才能取客户数据.
				//SPAPI_GetAllAccBalByArray,//1.44 暂时没用
				SPAPI_GetAccBalByCurrency,//1.45
				SPAPI_SubscribeTicker,//1.46
				SPAPI_SubscribeQuoteRequest,//1.47
				SPAPI_SubscribeAllQuoteRequest,//1.48
				SPAPI_GetAccInfo,//1.49
				SPAPI_GetDllVersion,//1.50
				SPAPI_LoadProductInfoListByCode,//1.51
				SPAPI_SetApiLogPath,//1.52
				SPAPI_GetCcyRateByCcy,//1.53
				SPAPI_AccountLogin,//1.54
				SPAPI_AccountLogout,//1.55
				SPAPI_SendAccControl,//1.56
				))
};
#include <exception>
#include <typeinfo>
#include <stdexcept>
// NOTES: In this worker thread, you cannot access any V8/node js variables
void worker_for_call(uv_work_t * req){
	MyUvShareData * my_data = (MyUvShareData *)req->data;//no
	//MyUvShareData * my_data = static_cast<MyUvShareData *>(req->data);
	json in=my_data->in;
	string api=my_data->api;
	json rst;
	rst["api"]=api;
	rst["in"]=in;
	void (*fcnPtr)(MyUvShareData * my_data) = _apiDict[api];
	if(NULL!=fcnPtr){
		try{
			fcnPtr(my_data);
		} catch (const std::exception& e) {
			// this executes if f() throws std::logic_error (base rule)
			rst["STS"]="KO";
			rst["errmsg"]=e.what();
		} catch (...) {
			// this executes if f() throws std::string or int or any other unrelated type
			rst["STS"]="KO";
			std::exception_ptr p = std::current_exception();
			rst["errmsg"]=(p ? p.__cxa_exception_type()->name() : "null");
		}
	}else{
		rst["STS"]="KO";
		rst["errmsg"]="not found api:"+api;
	}
	rst["out"]=my_data->out;
	my_data->out_s=my_data->out.dump();
	my_data->out=NULL;
	my_data->rst=rst;
	rst=NULL;
}
void after_worker_for_call(uv_work_t * req,int status){
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate);
	MyUvShareData * my_data = (MyUvShareData *)req->data;
	const unsigned argc = 1;

	json rst=my_data->rst;
	rst["rc"]=my_data->rc;
	if (my_data->rc<0) rst["STS"]="KO";
	else rst["STS"]="OK";

	//prev
	//v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(v8::String::NewFromUtf8(isolate,rst.dump().c_str()))};

	//seg fault?
	//v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(v8::Context::New(isolate),
	//		v8::String::NewFromUtf8(isolate,rst.dump().c_str())).ToLocalChecked()};

	v8::Local<v8::Value> argv[argc]={json_parse(isolate,rst.dump().c_str())};

	rst=NULL;

	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, my_data->callback);
	req->data=NULL;//unhook before delete my_data
	my_data->callback.Reset();
	my_data->rst=NULL;
	delete my_data;
	delete req;
	callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
}
#define METHOD_START_ONCALL($methodname)\
	void NODE_MODULE_LOGIC::$methodname(const v8::FunctionCallbackInfo<v8::Value>& args) {\
		int args_len=args.Length();\
		v8::Isolate* isolate = args.GetIsolate();\
		v8::Local<v8::Object> rt=  v8::Object::New(isolate);\
		v8::Local<v8::Object> out= v8::Object::New(isolate);\
		v8::Local<v8::Object> in = v8::Object::New(isolate);\
		v8::Local<v8::Function> callback;\
		if (args_len>0){\
			if(args[args_len-1]->IsFunction()){\
				callback = v8::Local<v8::Function>::Cast(args[args_len-1]);\
				if(args_len>2){\
					in = v8::Local<v8::Object>::Cast(args[args_len-2]);\
				}\
			}else{\
				if(args_len>1){\
					in = v8::Local<v8::Object>::Cast(args[args_len-1]);\
				}\
			}\
		}\
		int rc=0;
#define METHOD_END_ONCALL($methodname)\
		rt->Set(v8::String::NewFromUtf8(isolate,"in"), in);\
		rt->Set(v8::String::NewFromUtf8(isolate,"out"), out);\
		rt->Set(v8::String::NewFromUtf8(isolate,"rc"), v8::Integer::New(isolate,rc));\
		args.GetReturnValue().Set(rt);\
	}

//._on = callback
METHOD_START_ONCALL(_on){
	_callback_for_on.Reset(isolate,callback);
}METHOD_END_ONCALL(_on)

/* async mode if has(callback),pls use as much as possible, 'coz sync mode might block the nodejs */
METHOD_START_ONCALL(_call){
	if(args_len>0){
		COPY_V8_TO_STR(args[0],_call);
		MyUvShareData * req_data = new MyUvShareData;
		req_data->api=string(_call);
		req_data->in=json::parse(json_stringify(isolate,in));
		if(!callback.IsEmpty()){//ASYNC
			rt->Set(v8::String::NewFromUtf8(isolate,"mode"), v8::String::NewFromUtf8(isolate,"ASYNC"));
			req_data->callback.Reset(isolate, callback);//important
			req_data->seq=(++seq_count);
			uv_work_t *req= new uv_work_t();
			req->data=(void*)req_data;
			uv_queue_work(uv_default_loop(),req,worker_for_call,after_worker_for_call);
		}else{//SYNC
			rt->Set(v8::String::NewFromUtf8(isolate,"mode"), v8::String::NewFromUtf8(isolate,"SYNC"));
			uv_work_t *req= new uv_work_t();
			req->data=(void*)req_data;
			worker_for_call(req);

			//prev
			//out=v8::Local<v8::Object>::Cast(v8::JSON::Parse(
			//			v8::String::NewFromUtf8(isolate,req_data->out_s.c_str())
			//			));

			//out=v8::Local<v8::Object>::Cast(v8::JSON::Parse(
			//			v8::Context::New(isolate),
			//			v8::String::NewFromUtf8(isolate,req_data->out_s.c_str())
			//			).ToLocalChecked());
			
			out=v8::Local<v8::Object>::Cast(json_parse( isolate, req_data->out_s.c_str() ));
			
			rc=req_data->rc;
			if(rc<0){
				rt->Set(v8::String::NewFromUtf8(isolate,"STS"), v8::String::NewFromUtf8(isolate,"KO"));
			}else{
				rt->Set(v8::String::NewFromUtf8(isolate,"STS"), v8::String::NewFromUtf8(isolate,"OK"));
			}
			req->data=NULL;
			delete req_data;
			delete req;
		}
		rt->Set(v8::String::NewFromUtf8(isolate,"api"), v8::String::NewFromUtf8(isolate,_call));
	}
}METHOD_END_ONCALL(_call)
