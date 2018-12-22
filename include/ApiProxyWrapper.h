//---------------------------------------------------------------------------
#ifndef ApiProxyWrapperH
#define ApiProxyWrapperH
//---------------------------------------------------------------------------

#include "SPApiProxyDataType.h"
#include "ApiProxyWrapperReply.h"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
using namespace std;

class ApiProxyWrapper {
	public:
		ApiProxyWrapper(void);
		~ApiProxyWrapper(void);

		void SPAPI_RegisterApiProxyWrapperReply(ApiProxyWrapperReply* apiProxyWrapperReply);
		int SPAPI_Initialize();
		void SPAPI_SetLoginInfo(char *host, int port, char *license, char *app_id, char *user_id, char *password);
		int SPAPI_Login();
		int SPAPI_GetLoginStatus(char *user_id, short host_id);
		int SPAPI_AddOrder(SPApiOrder *order);
		int SPAPI_AddInactiveOrder(SPApiOrder* order);
		int SPAPI_ChangeOrder(char *user_id, SPApiOrder* order, double org_price, long org_qty);
		int SPAPI_ChangeOrderBy(char *user_id, char *acc_no, long accOrderNo, double org_price, long org_qty, double newPrice, long newQty);
		int SPAPI_DeleteOrderBy(char *user_id, char *acc_no, long accOrderNo, char* productCode, char* clOrderId);
		int SPAPI_DeleteAllOrders(char *user_id, char *acc_no);
		int SPAPI_ActivateAllOrders(char *user_id, char *acc_no);
		int SPAPI_InactivateAllOrders(char *user_id, char *acc_no);
		int SPAPI_ActivateOrderBy(char *user_id, char *acc_no, long accOrderNo);
		int SPAPI_InactivateOrderBy(char *user_id, char *acc_no, long accOrderNo);
		int SPAPI_GetOrderCount(char *user_id, char* acc_no);
		int SPAPI_GetOrderByOrderNo(char *user_id, char *acc_no, long int_order_no, SPApiOrder *order);
		int SPAPI_GetActiveOrders(char *user_id, char *acc_no, vector<SPApiOrder>& apiOrderList);
		int SPAPI_GetPosCount(char *user_id);
		int SPAPI_GetPosByProduct(char *user_id, char *prod_code, SPApiPos *pos);
		void SPAPI_Uninitialize();
		int SPAPI_Logout(char *user_id);
		int SPAPI_AccountLogin(char *user_id, char *acc_no);
		int SPAPI_AccountLogout(char *user_id, char *acc_no);
		int SPAPI_GetTradeCount(char *user_id, char *acc_no);
		int SPAPI_GetAllTrades(char *user_id, char *acc_no, vector<SPApiTrade>& apiTradeList);
		int SPAPI_SubscribePrice(char *user_id, char *prod_code, int mode);
		int SPAPI_SubscribeTicker(char *user_id, char *prod_code, int mode);
		int SPAPI_ChangePassword(char *user_id, char *old_password, char *new_password);
		int SPAPI_GetDllVersion(char *dll_ver_no, char *dll_rel_no, char *dll_suffix);
		int SPAPI_GetAccBalCount(char* user_id);
		int SPAPI_GetAccBalByCurrency(char *user_id, char *ccy, SPApiAccBal *acc_bal);
		int SPAPI_GetAllAccBal(char *user_id, vector<SPApiAccBal>& apiAccBalList);
		int SPAPI_GetCcyRateByCcy(char *user_id, char *ccy, double &rate);
		int SPAPI_GetAccInfo(char *user_id, SPApiAccInfo *acc_info);
		int SPAPI_GetPriceByCode(char *user_id, char *prod_code, SPApiPrice *price);
		int SPAPI_SetApiLogPath(char *path);
		int SPAPI_LoadProductInfoListByCode(char *inst_code);
		int SPAPI_GetProductCount();
		int SPAPI_GetProduct(vector<SPApiProduct>& apiProdList);
		int SPAPI_GetProductByCode(char *prod_code, SPApiProduct *prod);
		int SPAPI_LoadInstrumentList();
		int SPAPI_GetInstrumentCount();
		int SPAPI_GetInstrument(vector<SPApiInstrument>& apiInstList);
		int SPAPI_GetInstrumentByCode(char *inst_code, SPApiInstrument *inst);
		void SPAPI_SetLanguageId(int langid);
		int SPAPI_SendMarketMakingOrder(char *user_id, SPApiMMOrder *mm_order);
		int SPAPI_SubscribeQuoteRequest(char *user_id, char *prod_code, int mode);
		int SPAPI_SubscribeAllQuoteRequest(char *user_id, int mode);
		int SPAPI_GetAllAccBalByArray(char *acc_no, SPApiAccBal* apiAccBalList);
		int SPAPI_GetOrdersByArray(char* userId, char* acc_no, SPApiOrder* apiOrderList);
		int SPAPI_GetAllTradesByArray(char* userId, char* acc_no, SPApiTrade* apiTradeList);
		int SPAPI_GetInstrumentByArray(SPApiInstrument* apiInstList);
		int SPAPI_GetProductByArray(SPApiProduct* apiProdList);
		int SPAPI_GetAllPos(char *user_id, vector<SPApiPos>& apiPosList);
		int SPAPI_GetAllPosByArray(char *user_id, SPApiPos* apiPosList);
		int SPAPI_SendAccControl(char *user_id, char *acc_no, char ctrl_mask, char ctrl_level);

	private:

};

#endif

