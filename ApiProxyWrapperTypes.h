#ifndef ApiProxyWrapperTypeH
#define ApiProxyWrapperTypeH

//---------------------------------------------------------------------------
//#include "SPApiProxyDataType.h"
//#include "ApiProxyWrapperReply.h"

//#define COPY_V8_TO_STR(aaa,kkk)\
//	v8::Local<v8::String> in_##kkk = v8::Local<v8::String>::Cast(aaa);\
//	char kkk[255]={0};\
//	V8ToCharPtr(in_##kkk,kkk);
//#define COPY_TO_INT(aaa,kkk)\
//	int kkk=0;\
//	if(aaa.is_number_integer()){ kkk=aaa; }
//#define COPY_TO_LNG(aaa,kkk)\
//	long kkk=0;\
//	if(aaa.is_number_integer()){ kkk=aaa; }
//#define COPY_TO_DBL(aaa,kkk)\
//	double kkk=0;\
//	if(aaa.is_number_float()){ kkk=aaa; }
//#define COPY_TO_STR(aaa,kkk)\
//	char kkk[255]={0};\
//	if(aaa.is_string()){\
//		string str_in_##kkk=aaa;\
//		strcpy(kkk,str_in_##kkk.c_str());\
//	}
#define COPY_STF(sss,ttt,fff) try{ttt[#fff]=sss.fff;}catch(...){ cout << "COPY_STF ERRORKO "<< #fff << "=" << sss.fff << endl;}
#define COPY_TO_STF_VAL(sss,ttt,fff) if(!sss[#fff].is_null())ttt.fff=sss[#fff];
//#define COPY_TO_STF_STR(sss,ttt,fff) if(!sss[#fff].is_null())strcpy(ttt.fff,sss[#fff].get<string>().c_str());
#define COPY_TO_STF_STR(sss,ttt,fff) if(!sss[#fff].is_null())strncpy(ttt.fff,sss[#fff].get<string>().c_str(),(sizeof ttt.fff));
//#define COPY_TO_STF_CHR(sss,ttt,fff) if(!sss[#fff].is_null())try{ttt.fff,sss[#fff].get<char>();}catch(...){cout<< " ERRORKO "<< #fff << "=" << sss[#fff].get<char>() <<endl;}
#define COPY_TO_STF_CHR(sss,ttt,fff) if(!sss[#fff].is_null())try{char c_##fff[1];strncpy(c_##fff,sss[#fff].get<string>().c_str(),1);ttt.fff=c_##fff[0];}catch(...){cout<< " ERRORKO "<< #fff << "=" << sss[#fff].dump() <<endl;}
#define COPY_TO_STF_TINYINT(sss,ttt,fff) if(!sss[#fff].is_null())try{int i_##fff;i_##fff=sss[#fff];ttt.fff=(tinyint)i_##fff;}catch(...){cout<< " ERRORKO "<< #fff << "=" << sss[#fff].dump() <<endl;}

#define COPY_SPApiPos_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Qty,\
			DepQty,\
			LongQty,\
			ShortQty,\
			TotalAmt,\
			DepTotalAmt,\
			LongTotalAmt,\
			ShortTotalAmt,\
			PLBaseCcy,\
			PL,\
			ExchangeRate,\
			AccNo,\
			ProdCode,\
			LongShort,\
			DecInPrice,\
			))
#define COPY_SPApiTicker_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Price,\
			Qty,\
			TickerTime,\
			DealSrc,\
			ProdCode,\
			DecInPrice,\
			))
#define COPY_SPApiProduct_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			ProdCode,\
			ProdType,\
			ProdName,\
			Underlying,\
			InstCode,\
			ExpiryDate,\
			CallPut,\
			Strike,\
			LotSize,\
			ProdName1,\
			ProdName2,\
			OptStyle,\
			TickSize,\
			));\
			ttt["ProdNameUtf8"]=gbk2utf8(sss.ProdName2);
//#define COPY_SPApiPrice_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
//			Bid[SP_MAX_DEPTH],\
//			BidQty[SP_MAX_DEPTH],\
//			BidTicket[SP_MAX_DEPTH],\
//			Ask[SP_MAX_DEPTH],\
//			AskQty[SP_MAX_DEPTH],\
//			AskTicket[SP_MAX_DEPTH],\
//			Last[SP_MAX_LAST],\
//			LastQty[SP_MAX_LAST],\
//			LastTime[SP_MAX_LAST],\
//			Equil,\
//			Open,\
//			High,\
//			Low,\
//			Close,\
//			CloseDate,\
//			TurnoverVol,\
//			TurnoverAmt,\
//			OpenInt,\
//			ProdCode,\
//			ProdName,\
//			DecInPrice,\
//			Timestamp,\
//			))
#define COPY_SPApiPrice_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Equil,\
			Open,\
			High,\
			Low,\
			Close,\
			CloseDate,\
			TurnoverVol,\
			TurnoverAmt,\
			OpenInt,\
			ProdCode,\
			ProdName,\
			DecInPrice,\
			Timestamp,\
			));\
			ttt["Bid"]=sss.Bid[0];\
			ttt["BidQty"]=sss.BidQty[0];\
			ttt["BidTicket"]=sss.BidTicket[0];\
			ttt["Ask"]=sss.Ask[0];\
			ttt["AskQty"]=sss.AskQty[0];\
			ttt["AskTicket"]=sss.AskTicket[0];\
			ttt["Last"]=sss.Last[0];\
			ttt["LastQty"]=sss.LastQty[0];\
			ttt["LastTime"]=sss.LastTime[0];\

//wjc:以下官方可能未同步到linux版:
//ExStateNo,\
//TradeStateNo,\
//Suspend,\
//ExpiryYMD,\
//ContractYMD,
#define COPY_SPApiTrade_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			RecNo,\
			Price,\
			TradeNo,\
			ExtOrderNo,\
			IntOrderNo,\
			Qty,\
			TradeDate,\
			TradeTime,\
			AccNo,\
			ProdCode,\
			Initiator,\
			Ref,\
			Ref2,\
			GatewayCode,\
			ClOrderId,\
			BuySell,\
			OpenClose,\
			Status,\
			DecInPrice,\
			OrderPrice,\
			TradeRef,\
			TotalQty,\
			RemainingQty,\
			TradedQty,\
			AvgTradedPrice,\
			));
//wjc:AvgPrice
#define COPY_SPApiMMOrder_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			BidExtOrderNo,\
			AskExtOrderNo,\
			BidAccOrderNo,\
			AskAccOrderNo,\
			BidPrice,\
			AskPrice,\
			BidQty,\
			AskQty,\
			SpecTime,\
			OrderOptions,\
			ProdCode,\
			AccNo,\
			ClOrderId,\
			OrderType,\
			ValidType,\
			DecInPrice,\
			));
//wjc:OrigClOrderId,//旧用户自定义参考,但是win's spapidll.h也没有哦.
#define COPY_SPApiAccInfo_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			NAV,\
			BuyingPower,\
			CashBal,\
			MarginCall,\
			CommodityPL,\
			LockupAmt,\
			CreditLimit,\
			MaxMargin,\
			MaxLoanLimit,\
			TradingLimit,\
			RawMargin,\
			IMargin,\
			MMargin,\
			TodayTrans,\
			LoanLimit,\
			TotalFee,\
			LoanToMR,\
			LoanToMV,\
			AccName,\
			BaseCcy,\
			MarginClass,\
			TradeClass,\
			ClientId,\
			AEId,\
			AccType,\
			CtrlLevel,\
			Active,\
			MarginPeriod,\
			));\
			ttt["AccNameUtf8"]=big2utf8(sss.AccName);
#define COPY_SPApiInstrument_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Margin,\
			ContractSize,\
			MarketCode,\
			InstCode,\
			InstName,\
			InstName1,\
			InstName2,\
			Ccy,\
			DecInPrice,\
			InstType\
			));\
			ttt["InstName2Utf8"]=gbk2utf8(sss.InstName2);
#define COPY_SPApiAccBal_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			CashBf,\
			TodayCash,\
			NotYetValue,\
			Unpresented,\
			TodayOut,\
			Ccy\
			))
#define COPY_SPApiOrder_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Price,\
			StopLevel,\
			UpLevel,\
			UpPrice,\
			DownLevel,\
			DownPrice,\
			ExtOrderNo,\
			IntOrderNo,\
			Qty,\
			TradedQty,\
			TotalQty,\
			ValidTime,\
			SchedTime,\
			TimeStamp,\
			OrderOptions,\
			AccNo,\
			ProdCode,\
			Initiator,\
			Ref,\
			Ref2,\
			GatewayCode,\
			ClOrderId,\
			BuySell,\
			StopType,\
			OpenClose,\
			CondType,\
			OrderType,\
			ValidType,\
			Status,\
			DecInPrice,\
			OrderAction,\
			UpdateTime,\
			UpdateSeqNo\
			))

//#define COPY_TO_JSON(structName,$src,$tgt) EVAL(CAT(CAT(COPY_,structName),_FIELDS)($src,$tgt))
#define COPY_TO_JSON(structName,$src,$tgt) MCAT(COPY_,structName,_FIELDS)($src,$tgt)

#define COPY_TO_SPApiOrder_FIELDS(sss,ttt) \
	COPY_TO_STF_VAL(sss,ttt,Price);\
	COPY_TO_STF_VAL(sss,ttt,StopLevel);\
	COPY_TO_STF_VAL(sss,ttt,UpLevel);\
	COPY_TO_STF_VAL(sss,ttt,UpPrice);\
	COPY_TO_STF_VAL(sss,ttt,DownLevel);\
	COPY_TO_STF_VAL(sss,ttt,DownPrice);\
	COPY_TO_STF_VAL(sss,ttt,ExtOrderNo);\
	COPY_TO_STF_VAL(sss,ttt,IntOrderNo);\
	COPY_TO_STF_VAL(sss,ttt,Qty);\
	COPY_TO_STF_VAL(sss,ttt,TradedQty);\
	COPY_TO_STF_VAL(sss,ttt,TotalQty);\
	COPY_TO_STF_VAL(sss,ttt,ValidTime);\
	COPY_TO_STF_VAL(sss,ttt,SchedTime);\
	COPY_TO_STF_VAL(sss,ttt,TimeStamp);\
	COPY_TO_STF_VAL(sss,ttt,OrderOptions);\
	COPY_TO_STF_STR(sss,ttt,AccNo);\
	COPY_TO_STF_STR(sss,ttt,ProdCode);\
	COPY_TO_STF_STR(sss,ttt,Initiator);\
	COPY_TO_STF_STR(sss,ttt,Ref);\
	COPY_TO_STF_STR(sss,ttt,Ref2);\
	COPY_TO_STF_STR(sss,ttt,GatewayCode);\
	COPY_TO_STF_STR(sss,ttt,ClOrderId);\
	COPY_TO_STF_CHR(sss,ttt,BuySell);\
	COPY_TO_STF_CHR(sss,ttt,StopType);\
	COPY_TO_STF_CHR(sss,ttt,OpenClose);\
	COPY_TO_STF_TINYINT(sss,ttt,CondType);\
	COPY_TO_STF_TINYINT(sss,ttt,OrderType);\
	COPY_TO_STF_TINYINT(sss,ttt,ValidType);\
	COPY_TO_STF_TINYINT(sss,ttt,Status);\
	COPY_TO_STF_TINYINT(sss,ttt,DecInPrice);\
	COPY_TO_STF_TINYINT(sss,ttt,OrderAction);\
	COPY_TO_STF_VAL(sss,ttt,UpdateTime);\
	COPY_TO_STF_VAL(sss,ttt,UpdateSeqNo);

#define COPY_TO_SPApiMMOrder_FIELDS(sss,ttt) \
	COPY_TO_STF_VAL(sss,ttt,BidExtOrderNo);\
	COPY_TO_STF_VAL(sss,ttt,AskExtOrderNo);\
	COPY_TO_STF_VAL(sss,ttt,BidAccOrderNo);\
	COPY_TO_STF_VAL(sss,ttt,AskAccOrderNo);\
	COPY_TO_STF_VAL(sss,ttt,BidPrice);\
	COPY_TO_STF_VAL(sss,ttt,AskPrice);\
	COPY_TO_STF_VAL(sss,ttt,BidQty);\
	COPY_TO_STF_VAL(sss,ttt,AskQty);\
	COPY_TO_STF_VAL(sss,ttt,SpecTime);\
	COPY_TO_STF_VAL(sss,ttt,OrderOptions);\
	COPY_TO_STF_STR(sss,ttt,ProdCode);\
	COPY_TO_STF_STR(sss,ttt,AccNo);\
	COPY_TO_STF_STR(sss,ttt,ClOrderId);\
	COPY_TO_STF_TINYINT(sss,ttt,OrderType);\
	COPY_TO_STF_TINYINT(sss,ttt,ValidType);\
	COPY_TO_STF_TINYINT(sss,ttt,DecInPrice);

#define COPY_TO_STRUCT($structName,$src,$tgt) \
	$structName $tgt={0};\
	MCAT(COPY_TO_,$structName,_FIELDS)($src,$tgt)

#define COPY_VECTOR_TO_JSON($structName,$src,$tgt)\
	int $src##_size=$src.size();\
	if($src##_size>0){\
		for (int i = 0; i < $src##_size; i++) {\
			$structName& tmp##$structName = $src[i];\
			COPY_TO_JSON($structName,tmp##$structName,$tgt[i]);\
		}\
	}\

//---------------------------------------------------------------------------
#endif
