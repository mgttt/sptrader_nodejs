#ifndef SPApiProxyDataTypeH
#define SPApiProxyDataTypeH

typedef char            tinyint;
typedef unsigned char   u_tinyint;
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

#include "CCTypes.h"

typedef char STR4[4];
typedef char STR16[16];
typedef char STR40[40];

#include <stdint.h>

#define ORD_BUY         'B'
#define ORD_SELL        'S'
#define STOP_LOSS       'L'
#define STOP_UP         'U'
#define STOP_DOWN       'D'

#define AO_PRC          ((int32_t)0x7fffffff)

#define ORD_LIMIT       0
#define ORD_AUCTION     2
#define ORD_MARKET      6

#define COND_NONE       0
#define COND_STOP       1
#define COND_SCHEDTIME  3
#define COND_OCOSTOP    4
#define COND_TRAILSTOP  6
#define COND_COMBO_OPEN     8
#define COND_COMBO_CLOSE    9
#define COND_STOP_PRC       11
#define COND_OCOSTOP_PRC    14
#define COND_TRAILSTOP_PRC  16

#define VLD_REST_OF_DAY   0
#define VLD_FILL_AND_KILL 1
#define VLD_FILL_OR_KILL  2
#define VLD_UNTIL_EXPIRE  3
#define VLD_SPEC_TIME     4

#define ACT_ADD_ORDER    1
#define ACT_CHANGE_ORDER 2
#define ACT_DELETE_ORDER 3

#define ORDSTAT_SENDING     0
#define ORDSTAT_WORKING     1
#define ORDSTAT_INACTIVE    2
#define ORDSTAT_PENDING     3
#define ORDSTAT_ADDING      4
#define ORDSTAT_CHANGING    5
#define ORDSTAT_DELETING    6
#define ORDSTAT_INACTING    7
#define ORDSTAT_PARTTRD_WRK 8 // partial traded & working
#define ORDSTAT_TRADED      9
#define ORDSTAT_DELETED     10
#define ORDSTAT_APPROVEWAIT 18
#define ORDSTAT_TRADEDREP   20 // traded & reported
#define ORDSTAT_DELETEDREP  21 // deleted & reported
#define ORDSTAT_RESYNC_ABN  24 // resync abnormal
#define ORDSTAT_PARTTRD_DEL 28 // partial traded & deleted
#define ORDSTAT_PARTTRD_REP 29 // partial traded & reported (deleted)

#define ORDBIT_TPLUS1           0 // value 1
#define TRDBIT_TPLUS1           0 // value 1

#define OC_DEFAULT              '\0'
#define OC_OPEN                 'O'
#define OC_CLOSE                'C'
#define OC_MANDATORY_CLOSE      'M'

#define CTRLMASK_CTRL_LEVEL     1  //户口控制:级别
#define CTRLMASK_KICKOUT        2  //户口控制:踢走

typedef struct
{
	int32_t Qty;                    //上日仓位
	int32_t DepQty;                 //存储仓位
	int32_t LongQty;                //今日长仓
	int32_t ShortQty;               //今日短仓
	double TotalAmt;             //上日成交
	double DepTotalAmt;          //上日持仓总数(数量*价格)
	double LongTotalAmt;         //今日长仓总数(数量*价格)
	double ShortTotalAmt;        //今日短仓总数(数量*价格)
	double PLBaseCcy;            //盈亏(基本货币)
	double PL;                   //盈亏
	double ExchangeRate;         //汇率
	STR16 AccNo;                 //用户
	STR16 ProdCode;              //合约代码
	char LongShort;              //上日持仓买卖方向
	tinyint DecInPrice;          //小数点
} SPApiPos;

typedef struct
{
	double Price;              //价格
	double StopLevel;          //止损价格
	double UpLevel;            //上限水平
	double UpPrice;            //上限价格
	double DownLevel;          //下限水平
	double DownPrice;          //下限价格
	bigint ExtOrderNo;         //外部指示
	int32_t IntOrderNo;           //用户订单编号
	int32_t Qty;                  //剩余数量
	int32_t TradedQty;            //已成交数量
	int32_t TotalQty;             //全部数量
	int32_t ValidTime;            //有效时间
	int32_t SchedTime;            //预订发送时间
	int32_t TimeStamp;            //服务器接收订单时间
	uint32_t OrderOptions;
	STR16 AccNo;               //用户帐号
	STR16 ProdCode;            //合约代号
	STR16 Initiator;           //下单用户
	STR16 Ref;                 //参考
	STR16 Ref2;                //参考2
	STR16 GatewayCode;         //网关
	STR40 ClOrderId;           //用户自定义指令代号
	char BuySell;              //买卖方向
	char StopType;             //止损类型
	char OpenClose;            //开平仓
	tinyint CondType;          //订单条件类型
	tinyint OrderType;         //订单类型
	tinyint ValidType;         //订单有效类型
	tinyint Status;            //状态
	tinyint DecInPrice;        //合约小数位
	tinyint OrderAction;
	int32_t UpdateTime;
	int32_t UpdateSeqNo;
} SPApiOrder;

typedef struct
{
	int32_t RecNo;
	double Price;              //成交价格
	bigint TradeNo;            //成交编号
	bigint ExtOrderNo;         //外部指示
	int32_t IntOrderNo;           //用户订单编号
	int32_t Qty;                  //成交数量
	int32_t TradeDate;            //成交日期
	int32_t TradeTime;            //成交时间
	STR16 AccNo;               //用户
	STR16 ProdCode;            //合约代码
	STR16 Initiator;           //下单用户
	STR16 Ref;                 //参考
	STR16 Ref2;                //参考2
	STR16 GatewayCode;         //网关
	STR40 ClOrderId;           //用户自定义指令代号
	char BuySell;              //买卖方向
	char OpenClose;            //开平仓
	tinyint Status;            //状态
	tinyint DecInPrice;        //小数位
	double OrderPrice;
	STR40 TradeRef;
	int32_t TotalQty;
	int32_t RemainingQty;
	int32_t TradedQty;
	double AvgTradedPrice;
} SPApiTrade;


#define REQMODE_UNSUBSCRIBE     0
#define REQMODE_SUBSCRIBE       1
#define REQMODE_SNAPSHOT        2

typedef struct
{
	STR16 MarketCode;
	STR40 MarketName;
} SPApiMarket;

typedef struct
{
	double Margin;
	double ContractSize;
	STR16 MarketCode;
	STR16 InstCode;
	STR40 InstName;
	STR40 InstName1; //new added
	STR40 InstName2; //new added
	STR4 Ccy;
	char DecInPrice;
	char InstType;
} SPApiInstrument;

typedef struct
{
	STR16 ProdCode;
	char ProdType;
	STR40 ProdName;
	STR16 Underlying;
	STR16 InstCode;
	int32_t ExpiryDate;
	char CallPut;
	int32_t Strike;
	int32_t LotSize;
	STR40 ProdName1;
	STR40 ProdName2;
	char OptStyle;
	int32_t TickSize;
}SPApiProduct;

#define SP_MAX_DEPTH    20
#define SP_MAX_LAST     20
typedef struct
{
	double Bid[SP_MAX_DEPTH];     //买入价
	int32_t BidQty[SP_MAX_DEPTH];    //买入量
	int32_t BidTicket[SP_MAX_DEPTH]; //买指令数量
	double Ask[SP_MAX_DEPTH];     //卖出价
	int32_t AskQty[SP_MAX_DEPTH];    //卖出量
	int32_t AskTicket[SP_MAX_DEPTH]; //卖指令数量
	double Last[SP_MAX_LAST];     //成交价
	int32_t LastQty[SP_MAX_LAST];    //成交数量
	int32_t LastTime[SP_MAX_LAST];   //成交时间
	double Equil;                 //平衡价
	double Open;                  //开盘价
	double High;                  //最高价
	double Low;                   //最低价
	double Close;                 //收盘价
	int32_t CloseDate;               //收市日期
	double TurnoverVol;           //总成交量
	double TurnoverAmt;           //总成交额
	int32_t OpenInt;                 //未平仓
	STR16 ProdCode;               //合约代码
	STR40 ProdName;               //合约名称
	char DecInPrice;              //合约小数位
	//COPY FROM WIN64's spapidll.h {
	//int32_t ExStateNo;            //港期市场状态
	//int32_t TradeStateNo;         //市场状况
	//bool Suspend;                 //股票-是否已停牌
	//int32_t ExpiryYMD;            //产品到期日期
	//int32_t ContractYMD;          //合约到期日期
	//COPY FROM WIN64's spapidll.h }
	int32_t Timestamp;
} SPApiPrice;

typedef struct
{
	double Price;              //价格
	int32_t Qty;                  //成交量
	int32_t TickerTime;           //时间
	int32_t DealSrc;              //来源
	STR16 ProdCode;            //合约代码
	char DecInPrice;           //小数位
} SPApiTicker;

typedef struct
{
	double NAV;              // 资产净值
	double BuyingPower;      // 购买力
	double CashBal;          // 现金结余
	double MarginCall;       //追收金额
	double CommodityPL;      //商品盈亏
	double LockupAmt;        //冻结金额
	double CreditLimit;      // 信貸限額
	double MaxMargin;        // 最高保証金
	double MaxLoanLimit;     // 最高借貸上限
	double TradingLimit;     // 信用交易額
	double RawMargin;        // 原始保證金
	double IMargin;          //  基本保証金
	double MMargin;           // 維持保証金
	double TodayTrans;        // 交易金額
	double LoanLimit;         // 證券可按總值
	double TotalFee;          //  費用總額
	double LoanToMR;
	double LoanToMV;
	STR16 AccName;            //  戶口名稱
	STR4 BaseCcy;             // 基本貨幣
	STR16 MarginClass;        // 保証金類別
	STR16 TradeClass;         // 交易類別
	STR16 ClientId;           // 客戶
	STR16 AEId;               //  經紀
	char AccType;             // 戶口類別
	char CtrlLevel;           //  控制級數
	char Active;              //  生效
	char MarginPeriod;         // 時段
} SPApiAccInfo;

typedef struct
{
	double CashBf;          //上日结余
	double TodayCash;       //今日存取
	double NotYetValue;     //未交收
	double Unpresented;     //未兑现
	double TodayOut;        //提取要求
	STR4 Ccy;               //货币
} SPApiAccBal;

typedef struct
{
	u_short HostType;
	char Host[100];
	u_short Port;
	bool SecureConnection;
	bool IsConnected;
	u_short LoginStatus;
} SPConnectionInfo;

typedef struct
{
	bigint BidExtOrderNo;   //Bid(买)单外部指示
	bigint AskExtOrderNo;   //Ask(沽)单外部指示
	long BidAccOrderNo;     //Bid(买)单编号
	long AskAccOrderNo;     //Ask(沽)单编号
	double BidPrice;          //Bid(买)单价格
	double AskPrice;          //Ask(沽)单价格
	long BidQty;            //Bid(买)单数量
	long AskQty;            //Ask(沽)单数量
	long SpecTime;          //预订发送时间 //箇璹祇癳丁 
	u_long OrderOptions;    //0=默认,1=T+1
	STR16 ProdCode;         //合约代号 //腹 
	STR16 AccNo;            //用户帐号 //ノめ眀腹 
	STR40 ClOrderId;
	STR40 OrigClOrdId;
	tinyint OrderType;      //订单类型 //璹虫摸  
	tinyint ValidType;      //订单有效类型 //璹虫Τ摸 
	tinyint DecInPrice;     //合约小数位 //计 
} SPApiMMOrder;


enum LangNoEnum { ENG, TCHI, SCHI, TCHI_UNICODE, SCHI_UNICODE};

#endif

