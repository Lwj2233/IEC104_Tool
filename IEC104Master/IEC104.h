#ifndef IEC104_H
#define IEC104_H

#define TESTFR_CON    0x43
#define TESTFR_ACT    0x83
#define STOPDT_CON    0x13
#define STOPDT_ACT    0x23
#define STARTDT_CON   0x07
#define STARTDT_ACT   0x0B
#define U_FRM         0X03
#define S_FRM         0x01
#define I_FRM         0x00

#define DL_104_T0     30
#define DL_104_T1     15
#define DL_104_T2     10
#define DL_104_T3     20

#define DOT_TYPE_YX 1
#define DOT_TYPE_YC 2
#define DOT_TYPE_YM 3

//T1发送方发送一个I格式报文或U格式报文后，必须在t1的时间内得到接收方的认可,否则主动关闭连接
//T2在接收到I格式报文后，若经过t2时间未再收到新的I格式报文，则必须向发送方发送S格式帧,对已经接收到的I格式报文进行认可，显然t2必须小于t1。
//T3子站RTU端每接收一帧I帧、S帧或者U帧将重新触发计时器t3，若在t3内未接收到任何报文，将向对方发送测试链路帧TESTFR

#define DL_MAX_SEND_I_PAG 12
#define DL_MAX_REV_I_PAG  8

#ifndef U8
typedef unsigned char U8;
#endif

#ifndef U16
typedef unsigned short U16;
#endif

#ifndef U32
typedef unsigned int U32;
#endif

#ifndef U64
typedef unsigned long long U64;
#endif


#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

/***************************************************************************************/
enum STI104             //子站传输类型标志
{
    M_SP_NA_1 	= 1	,	//单点遥信变位
    M_SP_TA_1 	= 2	,	//单点遥信变位短时标
    M_DP_NA_1 	= 3	,	//双点遥信变位
    M_DP_TA_1 	= 4	,	//双点遥信变位短时标
    M_ST_NA_1 	= 5	,	//步长位置信息
    M_ST_TA_1 	= 6	,	//带短时标的步长位置信息
    M_BO_NA_1 	= 7	,	//32位比特串
    M_BO_TA_1 	= 8	,	//带短时标的32位比特串
    M_ME_NA_1 	= 9	,	//带品质描述的规一化值
    M_ME_TA_1 	= 10 ,	//带短时标带品质描述的规一化值
    M_ME_NB_1 	= 11 ,  //带品质描述的比例系数
    M_ME_TB_1 	= 12 ,  //带短时标带品质描述的比例系数
    M_ME_NC_1 	= 13 ,  //短浮点数
    M_ME_TC_1 	= 14 ,  //带短时标的短浮点数
    M_IT_NA_1 	= 15 ,  //累计值
    M_IT_TA_1 	= 16 ,  //带短时标的累计值
    M_EP_TA_1 	= 17 ,  //带时标的继电保护或重合闸设备单个事件
    M_EP_TB_1 	= 18 ,  //带时标的继电保护装置成组启动事件
    M_EP_TC_1 	= 19 ,  //带时标的继电保护装置成组输出电路信息事件
    M_PS_NA_1 	= 20 ,  //具有状态变位检出的成组单点信息
    M_ME_ND_1 	= 21 ,  //不带品质描述的规一化值
    M_SP_TB_1 	= 30 ,  //带 CP56Time2a 时标的单点信息
    M_DP_TB_1 	= 31 ,  //带 CP56Time2a 时标的双点信息
    M_ST_TB_1 	= 32 ,  //带 CP56Time2a 时标的步位置信息
    M_BO_TB_1 	= 33 ,  //带 CP56Time2a 时标的 32 比特串
    M_ME_TD_1 	= 34 ,  //带 CP56Time2a 时标的测量值规一化值
    M_ME_TE_1 	= 35 ,  //带 CP56Time2a 时标的测量值标度化值
    M_ME_TF_1	= 36 ,  //带 CP56Time2a 时标的测量值短浮点数
    M_IT_TB_1	= 37 ,  //带 CP56Time2a 时标的累计量
    M_EP_TD_1	= 38 ,  //带 CP56Time2a 时标的继电保护设备事件
    M_EP_TE_1	= 39 ,  //带 CP56Time2a 时标的继电保护设备成组启动事件
    M_EP_TF_1	= 40 ,  //带 CP56Time2a 时标的继电保拌装置成组输出电路信息

    M_EI_NA_1 	= 70   //初始化结束
};

/***************************************************************************************/
enum MTI104                 //主站传输类型标志
{
    C_SC_NA_1	= 45	,	//	单点遥控
    C_DC_NA_1	= 46	,	//	双点遥控
    C_RC_NA_1	= 47	,	//	升降命令
    C_SE_NA_1 	= 48	,	//	设定规一化值命令
    C_SE_NB_1 	= 49	,	//	设定比例系数值命令
    C_SE_NC_1 	= 50	,	//	设定浮点数值命令
    C_BO_NA_1	= 51	,	//	32位比特串
    C_SC_TA_1 	= 58	,	//	带长时标单点遥控(CP56)
    C_DC_TA_1 	= 59	,	//	带长时标双点遥控(CP56)
    C_RC_TA_1	= 60	,	//	带长时标升降命令(CP56)
    C_SE_TA_1	= 61	,	//	带长时标设定规一化值命令(CP56)
    C_SE_TB_1 	= 62	,	//	带长时标设定比例系数值命令(CP56)
    C_SE_TC_1 	= 63	,	//	带长时标设定浮点数值命令(CP56)
    C_BO_TA_1 	= 64	,	//	带长时标32位比特串(CP56)
    C_IC_NA_1 	= 100	,	//	总召唤
    C_CI_NA_1 	= 101	,	//	电能脉冲召唤命令
    C_RD_NA_1 	= 102	,	//	读数据命令
    C_CS_NA_1 	= 103	,	//	时钟同步命令
    C_TS_NA_1 	= 104	,	//	测试命令
    C_RP_NA_1 	= 105	,	//	复位进程命令
    C_CD_NA_1 	= 106	,	//	延时获得命令
    C_TS_TA_1 	= 107	,	//	带长时标测试命令
    P_ME_NA_1 	= 110	,	//	规一化值的参数
    P_ME_NB_1 	= 111	,	//	比例系数的参数
    P_ME_NC_1 	= 112	,	//	浮点数值的参数
    P_AC_NA_1 	= 113		//	参数的激活
};

/***************************************************************************************/
enum COT104               //传输原因
{
    PER_CRC_104     = 1	,   //周期循环per cyc
    BACK_104        = 2	,   //背景扫描back
    SPONT_104       = 3	,   //突发spont
    INIT_104        = 4	,   //初始化init
    REQ_104         = 5	,   //请求或被请求 req
    ACT_104         = 6	,   //激活act
    ACTCON_104      = 7	,   //激活确认actcon
    DEACT_104       = 8	,   //停止激活deact
    DEACTCON_104    = 9	,   //停止激活确认deactcon
    ACTTERM_104     = 10,   //激活结束actterm
    RETREM_104      = 11,   //远程命令引起的返送信息retrem
    RETLOC_104      = 12,   //当地命令引起的返送信息retloc
    FILE_104        = 13,   //文件传送file
    INTROGEN_104    = 20,   //响应总召唤introgen
    REQCOGCN        = 37,   //响应计数量总召唤reqcogcn
    REQCON1         = 38,   //响应第 1组计数量召唤reqco1
    REQCON2         = 39,   //响应第 2组计数量召唤reqco2
    REQCON3         = 40,   //响应第 3组计数量召唤reqco3
    REQCON4         = 41,   //响应第 4组计数量召唤reqco4

    UNDEFINE_TI = 44,       //未定义传输类型
    UNDEFINE_COT = 45,      //未定义传输原因
    UNDEFINE_COMM_ADDR = 46,//未知的公共地址
    UNDEFINE_INFO_ADDR = 47 //未知的信息地址
};

typedef struct _EXTRACTFRM_INFO
{
    U8  bFrmType    ;   //帧类型 三种 U /I /S
    U8  bTranType   ;   //传输类型  对应STI104 MTI104 和U格式的6种类型
    U16 bTranCot    ;   //传输原因  对应COT104
    U16 bRevS       ;   //接收序号
    U16 bSendS      ;   //发送序号
    U8  bVar        ;   //保留，做结构对齐用的
    U8  bSQNum      ;   //可变结构限定词
    U16 bAPDUaddr   ;   //APDU地址
    U8  *pinfo      ;   //信息体信息

}EXTRACTFRM_INFO;

/***************************************************************************************/
//从站信息体结构 定义开始
typedef struct _I_M_SP_NA_INFO // 单点遥信变位 信息体结构
{
    U8  InfoAddr1 ;   //信息体地址1
    U8  InfoAddr2 ;   //信息体地址2
    U8  InfoAddr3 ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;   //信息体元素
}I_M_SP_NA_INFO;

typedef struct _I_M_SP_TA_INFO // 单点遥信变位（带短时标） 信息体结构
{
    U8  InfoAddr1 ;   //信息体地址1
    U8  InfoAddr2 ;   //信息体地址2
    U8  InfoAddr3 ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;   //信息体元素
    U8  bMin      ;   //分
    U8  bSec      ;   //秒
    U8  bMilliSec ;   //毫秒
}I_M_SP_TA_INFO;

typedef struct _I_M_DP_NA_INFO // 双点遥信变位 信息体结构
{
    U8  InfoAddr1 ;   //信息体地址1
    U8  InfoAddr2 ;   //信息体地址2
    U8  InfoAddr3 ;   //信息体地址3  信息体地低位在前，高位在后
    U8  InfoData  ;   //信息体元素
}I_M_DP_NA_INFO;

typedef struct _I_M_DP_TA_INFO  //双点遥信变位(带短时标)信息体结构
{
    U8  InfoAddr1 ;   //信息体地址1
    U8  InfoAddr2 ;   //信息体地址2
    U8  InfoAddr3 ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;   //信息体元素
    U8  bMin      ;   //分
    U8  bSec      ;   //秒
    U8  bMilliSec ;   //毫秒
}I_M_DP_TA_INFO;

typedef struct _I_M_ST_NA_INFO // 步长位置信息 信息体结构
{
    U8  InfoAddr1     ;   //信息体地址1
    U8  InfoAddr2     ;   //信息体地址2
    U8  InfoAddr3     ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData      ;   //信息体元素
    U8  InfoBodyDec   ;   //信息体描述

}I_M_ST_NA_INFO;


typedef struct _I_M_ME_NA_INFO // 带品质描述的规一化值 信息体结构
{
    U8  InfoAddr1     ;   //信息体地址1
    U8  InfoAddr2     ;   //信息体地址2
    U8  InfoAddr3     ;   //信息体地址3   信息体地低位在前，高位在后
    U16 InfoData      ;   //信息体元素
    U8  InfoBodyDec   ;   //信息体描述
}I_M_ME_NA_INFO;

typedef struct _I_M_ME_NC_INFO // 短浮点数 信息体结构
{
    float InfoData    ;   //信息体元素
    U8  InfoBodyDec   ;   //信息体描述
}I_M_ME_NC_INFO;

typedef struct _I_M_IT_NA_INFO // 累计值 信息体结构
{
    U8  InfoAddr1   ;   //信息体地址1
    U8  InfoAddr2   ;   //信息体地址2
    U8  InfoAddr3   ;   //信息体地址3   信息体地低位在前，高位在后
    U32 InfoData    ;   //信息体元素
    U8  InfoBodyDec ;   //信息体描述
}I_M_IT_NA_INFO;

typedef struct _I_M_SP_TB_INFO // 带 CP56Time2a 时标的单点信息 信息体结构
{
    U8  InfoAddr1 ;       //信息体地址1
    U8  InfoAddr2 ;       //信息体地址2
    U8  InfoAddr3 ;       //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;       //信息体元素
    U8  bYear     ;       //年
    U8  bMonth    ;       //月
    U8  bDay      ;       //日
    U8  bHour     ;       //时
    U8  bMin      ;       //分
    U8  bSec      ;       //秒
    U8  bMilliSec ;       //毫秒
}I_M_SP_TB_INFO;

typedef struct _I_M_ST_TB_INFO // 带 CP56Time2a 时标的步位置信息 信息体结构
{
    U8  InfoAddr1     ;   //信息体地址1
    U8  InfoAddr2     ;   //信息体地址2
    U8  InfoAddr3     ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData      ;   //信息体元素
    U8  InfoBodyDec   ;   //信息体描述
    U8  bYear         ;   //年
    U8  bMonth        ;   //月
    U8  bDay          ;   //日
    U8  bHour         ;   //时
    U8  bMin          ;   //分
    U8  bSec          ;   //秒
    U8  bMilliSec     ;   //毫秒
}I_M_ST_TB_INFO;

typedef struct _I_M_EP_TD_INFO // 带 CP56Time2a 时标的继电保护设备事件 信息体结构
{
    U8  InfoAddr1     ;   //信息体地址1
    U8  InfoAddr2     ;   //信息体地址2
    U8  InfoAddr3     ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData      ;   //信息体元素
    U16 InfoBodyDec   ;   //信息体描述
    U8  bYear         ;   //年
    U8  bMonth        ;   //月
    U8  bDay          ;   //日
    U8  bHour         ;   //时
    U8  bMin          ;   //分
    U8  bSec          ;   //秒
    U8  bMilliSec     ;   //毫秒
}I_M_EP_TD_INFO;

typedef struct _I_M_EP_TE_INFO // 带 CP56Time2a 时标的继电保护设备成组启动事件 信息体结构
{
    U8  InfoAddr1     ;   //信息体地址1
    U8  InfoAddr2     ;   //信息体地址2
    U8  InfoAddr3     ;   //信息体地址3   信息体地低位在前，高位在后
    U16 InfoData      ;   //信息体元素
    U16 InfoBodyDec   ;   //信息体描述
    U8  bYear         ;   //年
    U8  bMonth        ;   //月
    U8  bDay          ;   //日
    U8  bHour         ;   //时
    U8  bMin          ;   //分
    U8  bSec          ;   //秒
    U8  bMilliSec     ;   //毫秒
}I_M_EP_TE_INFO;

typedef struct _I_M_EP_TF_INFO // 带 CP56Time2a 时标的继电保拌装置成组输出电路信息 信息体结构
{
    U8  InfoAddr1     ;   //信息体地址1
    U8  InfoAddr2     ;   //信息体地址2
    U8  InfoAddr3     ;   //信息体地址3   信息体地低位在前，高位在后
    U16 InfoData      ;   //信息体元素
    U16 InfoBodyDec   ;   //信息体描述
    U8  bYear         ;   //年
    U8  bMonth        ;   //月
    U8  bDay          ;   //日
    U8  bHour         ;   //时
    U8  bMin          ;   //分
    U8  bSec          ;   //秒
    U8  bMilliSec     ;   //毫秒
}I_M_EP_TF_INFO;
//从站信息体结构 定义结束
/***************************************************************************************/


/***************************************************************************************/
//主站信息体结构 定义开始
typedef struct _M_C_IC_NA_INFO // 总召唤信息体结构
{
    U8  InfoAddr1 ;   //信息体地址1
    U8  InfoAddr2 ;   //信息体地址2
    U8  InfoAddr3 ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;   //信息体元素   在总召唤中，本字节是固定值为： 0X14

}M_C_IC_NA_INFO;

typedef struct _M_C_SC_NA_INFO  //单点遥控信息体结构
{
    U8  InfoAddr1 ;   //信息体地址1
    U8  InfoAddr2 ;   //信息体地址2
    U8  InfoAddr3 ;   //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;   //信息体元素

}M_C_SC_NA_INFO;

typedef struct _M_C_DC_NA_INFO  //双点遥控信息体结构
{
    U8  InfoAddr1 ;    //信息体地址1
    U8  InfoAddr2 ;    //信息体地址2
    U8  InfoAddr3 ;    //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;    //信息体元素

}M_C_DC_NA_INFO;

typedef struct _M_C_RC_NA_INFO  //升降命令信息体结构
{
    U8  InfoAddr1 ;    //信息体地址1
    U8  InfoAddr2 ;    //信息体地址2
    U8  InfoAddr3 ;    //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;    //信息体元素

}M_C_RC_NA_INFO;

typedef struct _M_C_CI_NA_INFO  //电能脉冲召唤命令信息体结构
{
    U8  InfoAddr1 ;    //信息体地址1
    U8  InfoAddr2 ;    //信息体地址2
    U8  InfoAddr3 ;    //信息体地址3   信息体地低位在前，高位在后
    U8  InfoData  ;    //信息体元素

}M_C_CI_NA_INFO;

typedef struct _M_C_CS_NA_INFO  //时钟同步命令信息体结构
{
    U8  InfoAddr1 ;   //信息体地址1
    U8  InfoAddr2 ;   //信息体地址2
    U8  InfoAddr3 ;   //信息体地址3   信息体地低位在前，高位在后
    U8  bMilliSec ;   //毫秒
    U8  bSec      ;   //秒
    U8  bMin      ;   //分
    U8  bHour     ;   //时
    U8  bDay      ;   //日
    U8  bMonth    ;   //月
    U8  bYear     ;   //年

}M_C_CS_NA_INFO;
//主站信息体结构 定义结束
/***************************************************************************************/


/***************************************************************************************/
typedef struct _APCI_104
{
    U8 bHead      ;   //0x68
    U8 bAPDUlen   ;   //APDU的长度 最大长度为253
    U8 bCtlArr1   ;   //控制域8位位组1
    U8 bCtlArr2   ;   //控制域8位位组2
    U8 bCtlArr3   ;   //控制域8位位组3
    U8 bCtlArr4   ;   //控制域8位位组4

}APCI_104;

typedef enum _enumFrameType {
    frameTypeUnknown,       // 未知帧类型
    frameTypeI,             // I帧
    frameTypeS,             // S帧
    frameTypeU              // U帧
}enumFrameType;

#endif // IEC104_H
