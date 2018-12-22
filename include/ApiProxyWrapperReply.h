//---------------------------------------------------------------------------

#ifndef ApiProxyWrapperReplyH
#define ApiProxyWrapperReplyH
//---------------------------------------------------------------------------
#include <stdio.h>
#include "SPApiProxyDataType.h"
class ApiProxyWrapperReply {

	public:
		ApiProxyWrapperReply(void){};
		~ApiProxyWrapperReply(void){};
		virtual void OnTest() = 0;
		virtual void OnLoginReply(long ret_code,char *ret_msg) = 0;
		virtual void OnConnectedReply(long host_type,long con_status) = 0;
		virtual void OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg) = 0;
		virtual void OnApiOrderReport(long rec_no, const SPApiOrder *order) = 0;
		virtual void OnApiOrderBeforeSendReport(const SPApiOrder *order) = 0;
		virtual void OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg) = 0;
		virtual void OnAccountLogoutReply(long ret_code, char* ret_msg) = 0;
		virtual void OnAccountInfoPush(const SPApiAccInfo *acc_info) = 0;
		virtual void OnAccountPositionPush(const SPApiPos *pos) = 0;
		virtual void OnUpdatedAccountPositionPush(const SPApiPos *pos) = 0;
		virtual void OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal) = 0;
		virtual void OnApiTradeReport(long rec_no, const SPApiTrade *trade) = 0;
		virtual void OnApiPriceUpdate(const SPApiPrice *price) = 0;
		virtual void OnApiTickerUpdate(const SPApiTicker *ticker) = 0;
		virtual void OnPswChangeReply(long ret_code, char *ret_msg) = 0;
		virtual void OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg) = 0;
		virtual void OnInstrumentListReply(bool is_ready, char *ret_msg) = 0;
		virtual void OnBusinessDateReply(long business_date) = 0;
		virtual void OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order) = 0;
		virtual void OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg) = 0;
		virtual void OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty) = 0;
		virtual void OnApiAccountControlReply(long ret_code, char *ret_msg) = 0;
		virtual void OnApiLoadTradeReadyPush(long rec_no, const SPApiTrade *trade) = 0;
};

#endif
