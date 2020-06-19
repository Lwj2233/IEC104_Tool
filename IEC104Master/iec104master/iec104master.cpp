#include "iec104master.h"
#include <QDebug>

IEC104Master::IEC104Master(QHostAddress addr, QWidget *parent)
    : QMainWindow(parent)
    , m_hostAddr(addr)      // IP地址
    , m_connectFlag(0)
    , m_sendCount(0)        // 发送计数
    , m_recvCount(0)        // 接收计数
{
    //ui->setupUi(this);
    ///*设置表格是否充满，即行末不留空*/
    //ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ///*设置tablewidget等宽*/
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //ui->tableWidget->horizontalHeader()->resizeSection(0, 100); // 时间
    //ui->tableWidget->horizontalHeader()->resizeSection(1, 60);  // 收发
    //ui->tableWidget->horizontalHeader()->resizeSection(2, 60);  // 帧类型
    //// 表格使用交替色填充
    //ui->tableWidget->setAlternatingRowColors(true);
    //// 显示最后一行
    //ui->tableWidget->scrollToBottom();
    //
    ///*设置表格是否充满，即行末不留空*/
    //ui->tableWidget_YX->horizontalHeader()->setStretchLastSection(true);
    ///*设置tablewidget等宽*/
    //ui->tableWidget_YX->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //ui->tableWidget_YX->horizontalHeader()->resizeSection(0, 60);   // 地址
    //ui->tableWidget_YX->horizontalHeader()->resizeSection(1, 60);   // 地址
    //ui->tableWidget_YX->horizontalHeader()->resizeSection(2, 60);   // 值
    //// 表格使用交替色填充
    //ui->tableWidget_YX->setAlternatingRowColors(true);
    //ui->tableWidget_YX->setRowCount(DATA_NUM_YX);
    //
    ///*设置表格是否充满，即行末不留空*/
    //ui->tableWidget_YC->horizontalHeader()->setStretchLastSection(true);
    ///*设置tablewidget等宽*/
    //ui->tableWidget_YC->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //ui->tableWidget_YC->horizontalHeader()->resizeSection(0, 60);   // 地址
    //ui->tableWidget_YC->horizontalHeader()->resizeSection(1, 60);   // 地址
    //ui->tableWidget_YC->horizontalHeader()->resizeSection(2, 60);   // 值
    //// 表格使用交替色填充
    //ui->tableWidget_YC->setAlternatingRowColors(true);
    //ui->tableWidget_YC->setRowCount(DATA_NUM_YC);
    //
    //ui->dockWidget->setWindowTitle("报文");
    //ui->dockWidget_2->setWindowTitle("遥信");
    //ui->dockWidget_3->setWindowTitle("遥测");
    //tabifyDockWidget(ui->dockWidget, ui->dockWidget_2);
    //tabifyDockWidget(ui->dockWidget, ui->dockWidget_3);

    m_timer = new QTimer();
    QObject::connect(m_timer, &QTimer::timeout,
                     this, &IEC104Master::timerOut);

    m_tcpClientTh = new QThread();
    m_tcpClient = new MyTcpClient(m_hostAddr, 2404);
    m_tcpClient->moveToThread(m_tcpClientTh);
    connect(m_tcpClient, &MyTcpClient::ConnectStatusSignal,
            this, &IEC104Master::ConnectStatusSlot);
    connect(m_tcpClient, &MyTcpClient::ReadDataSignal,
            this, &IEC104Master::IEC104Recv);

    m_tcpClientTh->start();
}

IEC104Master::~IEC104Master()
{

}

void IEC104Master::Connect()     // 连接
{
    m_tcpClient->Connect();
}

void IEC104Master::DisConnect()  // 断开
{
    m_tcpClient->DisConnect();
}

float IEC104Master::getYcData(uint32_t addr)         // 获取单个遥测数据
{
    return m_data_yc.value(addr);
}

uint8_t IEC104Master::getYxData(uint32_t addr)       // 获取单个遥信数据
{
    return m_data_yx.value(addr);
}

QMap<uint32_t, float> IEC104Master::getYcDatas()     // 获取所有遥测数据
{
    return m_data_yc;
}

QMap<uint32_t, uint8_t> IEC104Master::getYxDatas()   // 获取所有遥信数据
{
    return m_data_yx;
}

void IEC104Master::timerOut()
{
    // 发送数据
    emit upData(m_data_yx, m_data_yc);

    if(m_connectFlag == 0)  {   // 连接标志 0-刚连接好， 非0-连接发送过报文)
        APCI_104 U1;     /* U 格式启动帧 */
        U1.bHead = 0x68;
        U1.bAPDUlen = 0x04;
        U1.bCtlArr1 = 0x07;
        U1.bCtlArr2 = 0x00;
        U1.bCtlArr3 = 0x00;
        U1.bCtlArr4 = 0x00;
        this->IEC104SendUFrm((const char*)&U1, sizeof(U1));
    }
    else if(m_connectFlag == 5){
        IEC104SendIFrm_IC_NA(); // 总召唤
    }
    else if(m_connectFlag % 300 == 0){
        IEC104SendIFrm_IC_NA(); // 总召唤
    }
    else if(m_connectFlag % 20 == 0){
        APCI_104 U2;    /* U 链路测试报文 */
        U2.bHead = 0x68;
        U2.bAPDUlen = 0x04;
        U2.bCtlArr1 = 0x43;
        U2.bCtlArr2 = 0x00;
        U2.bCtlArr3 = 0x00;
        U2.bCtlArr4 = 0x00;
        this->IEC104SendUFrm((const char*)&U2, sizeof(U2));
    }

    m_connectFlag++;
}

//void IEC104Master::IEC104Recv(const char *c, quint32 len)
//{
//    static char data[2048];
//    memset(data, 0x00, sizeof(data));
//    memcpy(data, c, len);
//
//    QByteArray ba(data, len);
//    QString ret(ba.toHex(' ').toUpper());    // QByteArray转十六进制CString
//    qDebug() << ret;
//    this->setTableWidget("Recv", ret);
//}

void IEC104Master::IEC104Recv(QByteArray ba)
{
    QString ret(ba.toHex(' ').toUpper());    // QByteArray转十六进制CString
    enumFrameType frameType = frameTypeUnknown;

    APCI_104 *p = (APCI_104 *)ba.data();
    if(p->bHead != 0x68) {          // 判断帧头是否正确
        ////
        this->setTableWidget(frameRecv, frameType, ret);
        return;
    }
    if(p->bAPDUlen > 253) {         // 判断长度是否正确
        ////
        this->setTableWidget(frameRecv, frameType, ret);
        return;
    }
    // I帧
    if(((p->bCtlArr1 & 0x01) == 0x00) && ((p->bCtlArr3 & 0x01) == 0x00)) {
        frameType = frameTypeI;
        m_recvCount = (p->bCtlArr1 + p->bCtlArr2*256) >> 1;     // 将发送计数存到本地，变为接收计数
        AnalysisIFrm(ba);
    }
    // S帧
    else if((p->bCtlArr1 == 0x01) && (p->bCtlArr2 == 0x00) && ((p->bCtlArr3 & 0x01) == 0x00)) {
        frameType = frameTypeS;
        AnalysisSFrm(ba);
    }
    // U帧
    else if(((p->bCtlArr1 & 0x03) == 0x03) && (p->bCtlArr2 == 0x00) && (p->bCtlArr3 == 0x00) && (p->bCtlArr4 == 0x00)) {
        frameType = frameTypeU;
        AnalysisUFrm(ba);
    }
    // 未知帧
    else {
        frameType = frameTypeUnknown;
    }
    this->setTableWidget(frameRecv, frameType, ret);

}

void IEC104Master::ConnectStatusSlot(bool s)
{
    qDebug() << "Connect Status " << s;
    if(s == 1) {
        m_connectFlag = 0;      // 连接标志 0-刚连接好， 非0-连接发送过报文
        m_timer->start(1000);
    }
    else {
        m_timer->stop();
    }
}
void IEC104Master::IEC104SendIFrm(const char *c, quint32 len)
{
    static char cIFrm[2048];
    APCI_104 *pI = (APCI_104 *)cIFrm;
    pI->bHead = 0x68;
    pI->bAPDUlen = len+4;
    pI->bCtlArr1 = (m_sendCount << 1) & 0xFF;
    pI->bCtlArr2 = (m_sendCount << 1 >> 8) & 0xFF;
    pI->bCtlArr3 = (m_recvCount << 1) & 0xFF;
    pI->bCtlArr4 = (m_recvCount << 1 >> 8) & 0xFF;
    m_sendCount++;

    memcpy(&(cIFrm[6]), c, len);

    m_tcpClient->SendData(cIFrm, len+6);

    QByteArray ba(cIFrm, len+6);
    QString ret(ba.toHex(' ').toUpper());    // QByteArray转十六进制CString
    this->setTableWidget(frameSend, frameTypeI, ret);
}
void IEC104Master::IEC104SendSFrm(const char *c, quint32 len)
{

}
void IEC104Master::IEC104SendUFrm(const char *c, quint32 len)
{
    m_tcpClient->SendData(c, len);

    QByteArray ba(c, len);
    QString ret(ba.toHex(' ').toUpper());    // QByteArray转十六进制CString
    this->setTableWidget(frameSend, frameTypeU, ret);
}

QString FrameType2Str(enumFrameType frameType)
{
    QString str;
    switch (frameType) {
    case frameTypeUnknown:
        str = "未知";
        break;
    case frameTypeI:
        str = "I";
        break;
    case frameTypeS:
        str = "S";
        break;
    case frameTypeU:
        str = "U";
        break;
    }
    return str;
}

// 设置表格显示
void IEC104Master::setTableWidget(enumFrameRecvSend frameRS, enumFrameType frameType, QString data)
{
    QTime current_time =QTime::currentTime();
    QString strTime = current_time.toString("hh:mm:ss.zzz");

    //int row = ui->tableWidget->rowCount();
    //ui->tableWidget->insertRow(row);
    //
    //ui->tableWidget->setItem(row, 0, new QTableWidgetItem(strTime));
    //ui->tableWidget->setItem(row, 1, new QTableWidgetItem((frameRS==frameRecv)?"R":"S"));
    //ui->tableWidget->setItem(row, 2, new QTableWidgetItem(FrameType2Str(frameType)));
    //ui->tableWidget->setItem(row, 3, new QTableWidgetItem(data));
    //// 显示最后一行
    //ui->tableWidget->scrollToBottom();
}

// 总召唤
void IEC104Master::IEC104SendIFrm_IC_NA()
{
    // 总召唤
    static char c[128];
    I_ASDU_INFO *p = (I_ASDU_INFO *)c;
    p->bTranType  = 0x64    ;   //类型表示
    p->bSQNum     = 0x01    ;   //可变结构限定词
    p->bTranCot   = 0x0006  ;   //传输原因
    p->bAPDUaddr  = 0x0001  ;   //APDU地址
    p->pinfo[0]   = 0x00    ;   //信息体信息
    p->pinfo[1]   = 0x00    ;   //信息体信息
    p->pinfo[2]   = 0x00    ;   //信息体信息
    p->pinfo[3]   = 0x14    ;   //信息体信息

    this->IEC104SendIFrm(c, 10);
}

void IEC104Master::AnalysisIFrm(QByteArray ba)
{
    uint8_t bTranType = ba.at(6);                   // 类型
    //uint8_t bSQNum = ba.at(7);                      // 可变结构限定词
    //uint16_t wTranCot = ba.at(8)+256*ba.at(9);      // 传输原因
    //uint16_t wAPDUaddr = ba.at(10)+256*ba.at(11);   // 站地址

    switch(bTranType){
    case M_SP_NA_1: // 1  ,	//单点遥信变位
        AnalysisIFrm_SP_NA(ba);
        break;
    case M_SP_TA_1: // 2  ,	//单点遥信变位短时标
        AnalysisIFrm_SP_TA(ba);
        break;
    case M_DP_NA_1: // 3  ,	//双点遥信变位
        AnalysisIFrm_DP_NA(ba);
        break;
    case M_DP_TA_1: // 4  ,	//双点遥信变位短时标
        AnalysisIFrm_DP_TA(ba);
        break;
    case M_ST_NA_1: // 5  ,	//步长位置信息
        AnalysisIFrm_ST_NA(ba);
        break;
    case M_ST_TA_1: // 6  ,	//带短时标的步长位置信息
        AnalysisIFrm_ST_TA(ba);
        break;
    case M_BO_NA_1: // 7  ,	//32位比特串
        AnalysisIFrm_BO_NA(ba);
        break;
    case M_BO_TA_1: // 8  ,	//带短时标的32位比特串
        AnalysisIFrm_BO_TA(ba);
        break;
    case M_ME_NA_1: // 9  ,	//带品质描述的规一化值
        AnalysisIFrm_ME_NA(ba);
        break;
    case M_ME_TA_1: // 10 ,	//带短时标带品质描述的规一化值
        AnalysisIFrm_ME_TA(ba);
        break;
    case M_ME_NB_1: // 11 ,  //带品质描述的比例系数
        AnalysisIFrm_ME_NB(ba);
        break;
    case M_ME_TB_1: // 12 ,  //带短时标带品质描述的比例系数
        AnalysisIFrm_ME_TB(ba);
        break;
    case M_ME_NC_1: // 13 ,  //短浮点数
        AnalysisIFrm_ME_NC(ba);
        break;
    case M_ME_TC_1: // 14 ,  //带短时标的短浮点数
        AnalysisIFrm_ME_TC(ba);
        break;
    case M_IT_NA_1: // 15 ,  //累计值
        AnalysisIFrm_IT_NA(ba);
        break;
    case M_IT_TA_1: // 16 ,  //带短时标的累计值
        AnalysisIFrm_IT_TA(ba);
        break;
    case M_EP_TA_1: // 17 ,  //带时标的继电保护或重合闸设备单个事件
        AnalysisIFrm_EP_TA(ba);
        break;
    case M_EP_TB_1: // 18 ,  //带时标的继电保护装置成组启动事件
        AnalysisIFrm_EP_TB(ba);
        break;
    case M_EP_TC_1: // 19 ,  //带时标的继电保护装置成组输出电路信息事件
        AnalysisIFrm_EP_TC(ba);
        break;
    case M_PS_NA_1: // 20 ,  //具有状态变位检出的成组单点信息
        AnalysisIFrm_PS_NA(ba);
        break;
    case M_ME_ND_1: // 21 ,  //不带品质描述的规一化值
        AnalysisIFrm_ME_ND(ba);
        break;
    case M_SP_TB_1: // 30 ,  //带 CP56Time2a 时标的单点信息
        AnalysisIFrm_SP_TB(ba);
        break;
    case M_DP_TB_1: // 31 ,  //带 CP56Time2a 时标的双点信息
        AnalysisIFrm_DP_TB(ba);
        break;
    case M_ST_TB_1: // 32 ,  //带 CP56Time2a 时标的步位置信息
        AnalysisIFrm_ST_TB(ba);
        break;
    case M_BO_TB_1: // 33 ,  //带 CP56Time2a 时标的 32 比特串
        AnalysisIFrm_BO_TB(ba);
        break;
    case M_ME_TD_1: // 34 ,  //带 CP56Time2a 时标的测量值规一化值
        AnalysisIFrm_ME_TD(ba);
        break;
    case M_ME_TE_1: // 35 ,  //带 CP56Time2a 时标的测量值标度化值
        AnalysisIFrm_ME_TE(ba);
        break;
    case M_ME_TF_1: // 36 ,  //带 CP56Time2a 时标的测量值短浮点数
        AnalysisIFrm_ME_TF(ba);
        break;
    case M_IT_TB_1: // 37 ,  //带 CP56Time2a 时标的累计量
        AnalysisIFrm_IT_TB(ba);
        break;
    case M_EP_TD_1: // 38 ,  //带 CP56Time2a 时标的继电保护设备事件
        AnalysisIFrm_EP_TD(ba);
        break;
    case M_EP_TE_1: // 39 ,  //带 CP56Time2a 时标的继电保护设备成组启动事件
        AnalysisIFrm_EP_TE(ba);
        break;
    case M_EP_TF_1: // 40 ,  //带 CP56Time2a 时标的继电保拌装置成组输出电路信息
        AnalysisIFrm_EP_TF(ba);
        break;
    case M_EI_NA_1: // 70   //初始化结束
        AnalysisIFrm_EI_NA(ba);
        break;
    }
}
void IEC104Master::AnalysisSFrm(QByteArray ba)
{

}
void IEC104Master::AnalysisUFrm(QByteArray ba)
{

}

void IEC104Master::AnalysisIFrm_SP_NA(QByteArray ba)// 1	单点遥信变位
{
    uint32_t addr;
    uint32_t value;
    uint8_t bSQNum = ba.at(7);                      // 可变结构限定词
    I_M_SP_NA_INFO *p = (I_M_SP_NA_INFO *)(ba.data() + 12);
    for(int i = 0; i < bSQNum; i++) {
        addr = (uint8_t)p->InfoAddr1 + (uint8_t)p->InfoAddr2*256 + (uint8_t)p->InfoAddr3*256*256;
        value = p->InfoData;
        //qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << QString("0x%1 : %2").arg(addr, 6, 16, QLatin1Char('0')).arg(value);
        m_data_yx.insert(addr, value);
        p++;
        int row = addr - ADDR_START_YX;
        //ui->tableWidget_YX->setItem(row, 0, new QTableWidgetItem(QString::number(addr)));
        //ui->tableWidget_YX->setItem(row, 1, new QTableWidgetItem(QString::number(addr, 16)));
        //ui->tableWidget_YX->setItem(row, 2, new QTableWidgetItem(QString::number(value)));
        //ui->tableWidget_YX->setItem(row, 3, new QTableWidgetItem(App::YX_Names.at(row)));
    }
    //qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << m_data_yx;
}
void IEC104Master::AnalysisIFrm_SP_TA(QByteArray ba)// 2	单点遥信变位短时标
{

}
void IEC104Master::AnalysisIFrm_DP_NA(QByteArray ba)// 3	双点遥信变位
{

}
void IEC104Master::AnalysisIFrm_DP_TA(QByteArray ba)// 4	双点遥信变位短时标
{

}
void IEC104Master::AnalysisIFrm_ST_NA(QByteArray ba)// 5	步长位置信息
{

}
void IEC104Master::AnalysisIFrm_ST_TA(QByteArray ba)// 6	带短时标的步长位置信息
{

}
void IEC104Master::AnalysisIFrm_BO_NA(QByteArray ba)// 7	32位比特串
{

}
void IEC104Master::AnalysisIFrm_BO_TA(QByteArray ba)// 8	带短时标的32位比特串
{

}
void IEC104Master::AnalysisIFrm_ME_NA(QByteArray ba)// 9	带品质描述的规一化值
{

}
void IEC104Master::AnalysisIFrm_ME_TA(QByteArray ba)// 10   带短时标带品质描述的规一化值
{

}
void IEC104Master::AnalysisIFrm_ME_NB(QByteArray ba)// 11   带品质描述的比例系数
{

}
void IEC104Master::AnalysisIFrm_ME_TB(QByteArray ba)// 12   带短时标带品质描述的比例系数
{

}
void IEC104Master::AnalysisIFrm_ME_NC(QByteArray ba)// 13   短浮点数
{
    //uint8_t bTranType = ba.at(6);                         // 类型
    //uint8_t bSQNum = (uint8_t)ba.at(7) - (uint8_t)0x80;   // 可变结构限定词
    uint16_t wTranCot = ba.at(8)+256*ba.at(9);              // 传输原因
    //uint16_t wAPDUaddr = ba.at(10)+256*ba.at(11);         // 站地址
    switch(wTranCot) {
    case INTROGEN_104:      // 响应总召唤
        AnalysisIFrm_ME_NC_INTROGEN(ba);
        break;
    case SPONT_104:         // 变换上传
        AnalysisIFrm_ME_NC_SPONT(ba);
        break;
    }


}

void IEC104Master::AnalysisIFrm_ME_NC_SPONT(QByteArray ba)      // 短浮点数_突发上传
{
    uint32_t addr;
    float value;
    //uint8_t bTranType = ba.at(6);                         // 类型
    uint8_t bSQNum = (uint8_t)ba.at(7);//- (uint8_t)0x80;     // 可变结构限定词
    //uint16_t wTranCot = ba.at(8)+256*ba.at(9);            // 传输原因
    //uint16_t wAPDUaddr = ba.at(10)+256*ba.at(11);         // 站地址

    I_M_ME_NC_ADDR_INFO *p = (I_M_ME_NC_ADDR_INFO *)(ba.data() + 12);
    for(int i = 0; i < bSQNum; i++) {
        addr = (uint8_t)p->InfoAddr1 + (uint8_t)p->InfoAddr2*256 + (uint8_t)p->InfoAddr3*256*256;
        value = p->InfoData;
        m_data_yc.insert(addr, value);
        p++;
        //qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << QString("0x%1 : %2").arg(addr, 6, 16, QLatin1Char('0')).arg(QString::number(value, 'f'));
        //int row = addr - ADDR_START_YC;
        //ui->tableWidget_YC->setItem(row, 0, new QTableWidgetItem(QString::number(addr)));
        //ui->tableWidget_YC->setItem(row, 1, new QTableWidgetItem(QString::number(addr, 16)));
        //ui->tableWidget_YC->setItem(row, 2, new QTableWidgetItem(QString::number(value)));
        //ui->tableWidget_YC->setItem(row, 3, new QTableWidgetItem(App::YC_Names.at(row)));
    }
    //qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << m_data_yc;
}
void IEC104Master::AnalysisIFrm_ME_NC_INTROGEN(QByteArray ba)   // 短浮点数_响应总召唤
{
    uint32_t addr;
    float value;
    //uint8_t bTranType = ba.at(6);                         // 类型
    uint8_t bSQNum = (uint8_t)ba.at(7) - (uint8_t)0x80;   // 可变结构限定词
    //uint16_t wTranCot = ba.at(8)+256*ba.at(9);              // 传输原因
    //uint16_t wAPDUaddr = ba.at(10)+256*ba.at(11);         // 站地址

    uint32_t startAddr = (uint8_t)ba.at(12) + (uint8_t)ba.at(13)*256 + (uint8_t)ba.at(14)*256*256;
    I_M_ME_NC_INFO *p = (I_M_ME_NC_INFO *)(ba.data() + 15);
    for(int i = 0; i < bSQNum; i++) {
        addr = startAddr + i;
        value = p->InfoData;
        m_data_yc.insert(addr, value);
        p++;
        //qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << QString("0x%1 : %2").arg(addr, 6, 16, QLatin1Char('0')).arg(QString::number(value, 'f'));
        //int row = addr - ADDR_START_YC;
        //ui->tableWidget_YC->setItem(row, 0, new QTableWidgetItem(QString::number(addr)));
        //ui->tableWidget_YC->setItem(row, 1, new QTableWidgetItem(QString::number(addr, 16)));
        //ui->tableWidget_YC->setItem(row, 2, new QTableWidgetItem(QString::number(value)));
        //ui->tableWidget_YC->setItem(row, 3, new QTableWidgetItem(App::YC_Names.at(row)));
    }
    //qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << m_data_yc;
}
void IEC104Master::AnalysisIFrm_ME_TC(QByteArray ba)// 14   带短时标的短浮点数
{

}
void IEC104Master::AnalysisIFrm_IT_NA(QByteArray ba)// 15   累计值
{

}
void IEC104Master::AnalysisIFrm_IT_TA(QByteArray ba)// 16   带短时标的累计值
{

}
void IEC104Master::AnalysisIFrm_EP_TA(QByteArray ba)// 17   带时标的继电保护或重合闸设备单个事件
{

}
void IEC104Master::AnalysisIFrm_EP_TB(QByteArray ba)// 18   带时标的继电保护装置成组启动事件
{

}
void IEC104Master::AnalysisIFrm_EP_TC(QByteArray ba)// 19   带时标的继电保护装置成组输出电路信息事件
{

}
void IEC104Master::AnalysisIFrm_PS_NA(QByteArray ba)// 20   具有状态变位检出的成组单点信息
{

}
void IEC104Master::AnalysisIFrm_ME_ND(QByteArray ba)// 21   不带品质描述的规一化值
{

}
void IEC104Master::AnalysisIFrm_SP_TB(QByteArray ba)// 30   带 CP56Time2a 时标的单点信息
{

}
void IEC104Master::AnalysisIFrm_DP_TB(QByteArray ba)// 31   带 CP56Time2a 时标的双点信息
{

}
void IEC104Master::AnalysisIFrm_ST_TB(QByteArray ba)// 32   带 CP56Time2a 时标的步位置信息
{

}
void IEC104Master::AnalysisIFrm_BO_TB(QByteArray ba)// 33   带 CP56Time2a 时标的 32 比特串
{

}
void IEC104Master::AnalysisIFrm_ME_TD(QByteArray ba)// 34   带 CP56Time2a 时标的测量值规一化值
{

}
void IEC104Master::AnalysisIFrm_ME_TE(QByteArray ba)// 35   带 CP56Time2a 时标的测量值标度化值
{

}
void IEC104Master::AnalysisIFrm_ME_TF(QByteArray ba)// 36   带 CP56Time2a 时标的测量值短浮点数
{

}
void IEC104Master::AnalysisIFrm_IT_TB(QByteArray)// 37   带 CP56Time2a 时标的累计量
{

}
void IEC104Master::AnalysisIFrm_EP_TD(QByteArray ba)// 38   带 CP56Time2a 时标的继电保护设备事件
{

}
void IEC104Master::AnalysisIFrm_EP_TE(QByteArray ba)// 39   带 CP56Time2a 时标的继电保护设备成组启动事件
{

}
void IEC104Master::AnalysisIFrm_EP_TF(QByteArray ba)// 40   带 CP56Time2a 时标的继电保拌装置成组输出电路信息
{

}
void IEC104Master::AnalysisIFrm_EI_NA(QByteArray ba)// 70   初始化结束
{

}
