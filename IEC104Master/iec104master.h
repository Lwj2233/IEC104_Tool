#ifndef IEC104MASTER_H
#define IEC104MASTER_H

#include <QMainWindow>
#include "IEC104.h"
#include "mytcpclient.h"
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class IEC104Master; }
QT_END_NAMESPACE

typedef enum _enumFrameRecvSend {
    frameSend,
    frameRecv
} enumFrameRecvSend;

class IEC104Master : public QMainWindow
{
    Q_OBJECT

public:
    IEC104Master(QWidget *parent = nullptr);
    ~IEC104Master();

private slots:
    void timerOut();
    //void IEC104Recv(const char *, quint32);
    void IEC104Recv(QByteArray);
    void ConnectStatusSlot(bool s);

    void on_pushButton_clicked();

private:
    void IEC104Send(const char *, quint32);
    void setTableWidget(enumFrameRecvSend frameRS, enumFrameType frameType, QString data);

private:
    void AnalysisIFrm(QByteArray);
    void AnalysisSFrm(QByteArray);
    void AnalysisUFrm(QByteArray);

    void AnalysisIFrm_SP_NA(QByteArray);// 1	单点遥信变位
    void AnalysisIFrm_SP_TA(QByteArray);// 2	单点遥信变位短时标
    void AnalysisIFrm_DP_NA(QByteArray);// 3	双点遥信变位
    void AnalysisIFrm_DP_TA(QByteArray);// 4	双点遥信变位短时标
    void AnalysisIFrm_ST_NA(QByteArray);// 5	步长位置信息
    void AnalysisIFrm_ST_TA(QByteArray);// 6	带短时标的步长位置信息
    void AnalysisIFrm_BO_NA(QByteArray);// 7	32位比特串
    void AnalysisIFrm_BO_TA(QByteArray);// 8	带短时标的32位比特串
    void AnalysisIFrm_ME_NA(QByteArray);// 9	带品质描述的规一化值
    void AnalysisIFrm_ME_TA(QByteArray);// 10   带短时标带品质描述的规一化值
    void AnalysisIFrm_ME_NB(QByteArray);// 11   带品质描述的比例系数
    void AnalysisIFrm_ME_TB(QByteArray);// 12   带短时标带品质描述的比例系数
    void AnalysisIFrm_ME_NC(QByteArray);// 13   短浮点数
    void AnalysisIFrm_ME_TC(QByteArray);// 14   带短时标的短浮点数
    void AnalysisIFrm_IT_NA(QByteArray);// 15   累计值
    void AnalysisIFrm_IT_TA(QByteArray);// 16   带短时标的累计值
    void AnalysisIFrm_EP_TA(QByteArray);// 17   带时标的继电保护或重合闸设备单个事件
    void AnalysisIFrm_EP_TB(QByteArray);// 18   带时标的继电保护装置成组启动事件
    void AnalysisIFrm_EP_TC(QByteArray);// 19   带时标的继电保护装置成组输出电路信息事件
    void AnalysisIFrm_PS_NA(QByteArray);// 20   具有状态变位检出的成组单点信息
    void AnalysisIFrm_ME_ND(QByteArray);// 21   不带品质描述的规一化值
    void AnalysisIFrm_SP_TB(QByteArray);// 30   带 CP56Time2a 时标的单点信息
    void AnalysisIFrm_DP_TB(QByteArray);// 31   带 CP56Time2a 时标的双点信息
    void AnalysisIFrm_ST_TB(QByteArray);// 32   带 CP56Time2a 时标的步位置信息
    void AnalysisIFrm_BO_TB(QByteArray);// 33   带 CP56Time2a 时标的 32 比特串
    void AnalysisIFrm_ME_TD(QByteArray);// 34   带 CP56Time2a 时标的测量值规一化值
    void AnalysisIFrm_ME_TE(QByteArray);// 35   带 CP56Time2a 时标的测量值标度化值
    void AnalysisIFrm_ME_TF(QByteArray);// 36   带 CP56Time2a 时标的测量值短浮点数
    void AnalysisIFrm_IT_TB(QByteArray);// 37   带 CP56Time2a 时标的累计量
    void AnalysisIFrm_EP_TD(QByteArray);// 38   带 CP56Time2a 时标的继电保护设备事件
    void AnalysisIFrm_EP_TE(QByteArray);// 39   带 CP56Time2a 时标的继电保护设备成组启动事件
    void AnalysisIFrm_EP_TF(QByteArray);// 40   带 CP56Time2a 时标的继电保拌装置成组输出电路信息
    void AnalysisIFrm_EI_NA(QByteArray);// 70   初始化结束

private:
    Ui::IEC104Master *ui;
    QThread *m_tcpClientTh;
    MyTcpClient *m_tcpClient;

    QTimer *m_timer;

    int m_connectFlag;      // 连接标志 0-刚连接好， 非0-连接发送过报文
};
#endif // IEC104MASTER_H
