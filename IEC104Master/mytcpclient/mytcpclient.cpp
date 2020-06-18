#include "mytcpclient.h"

MyTcpClient::MyTcpClient(const QString &hostName, quint16 port, QObject *parent) : QObject(parent)
{
    // 注册类型
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<bool>("bool");

    m_date.clear(); // 数据取清零

    m_tcpClientThreadTh = new QThread(this);
    m_tcpClientThread = new MyTcpClientThread(hostName, port);
    m_tcpClientThread->moveToThread(m_tcpClientThreadTh);

    // 连接
    connect(this, &MyTcpClient::ConnectSignal, m_tcpClientThread, &MyTcpClientThread::ConnectSlot);
    // 断开
    connect(this, &MyTcpClient::DisConnectSignal, m_tcpClientThread, &MyTcpClientThread::DisConnectSlot);
    // 发送数据
    connect(this, &MyTcpClient::SendDataSignal, m_tcpClientThread, &MyTcpClientThread::SendDataSlot);

    // 接收数据
    connect(m_tcpClientThread, &MyTcpClientThread::ReadDataSignal, this, &MyTcpClient::ReadDataSlot);
    // 连接状态（是否已连接）
    connect(m_tcpClientThread, &MyTcpClientThread::ConnectStatusSignal, this, &MyTcpClient::ConnectStatusSlot);

    m_tcpClientThreadTh->start();
}

MyTcpClient::~MyTcpClient()
{
    m_tcpClientThreadTh->quit();
    m_tcpClientThreadTh->wait();

    delete m_tcpClientThread;
    delete m_tcpClientThreadTh;
}

/***********************************************************************************************
***************************************** 对外的接口 *******************************************
***********************************************************************************************/
void MyTcpClient::Connect()     // 连接
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
    emit ConnectSignal();       // 给 TcpThread 发送连接信号
}

void MyTcpClient::DisConnect()  // 断开
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
    emit DisConnectSignal();    // 给 TcpThread 发送断开信号
}

void MyTcpClient::SendData(const char *c, const uint32_t len)
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
    emit SendDataSignal(c, len);    // 给 TcpThread 发送 发数据信号
}

/***********************************************************************************************
************************************** 对TcpThread的槽函数 ***************************************
***********************************************************************************************/
//void MyTcpClient::ReadDataSlot(const char *c, quint32 len)    // 读数据
//{
//    static char data[2048];
//    memset(data, 0x00, sizeof(data));
//    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
//    memcpy(data, c, len);
//    emit ReadDataSignal(data, len);    // 对外发送信号（收到的数据）
//}

void MyTcpClient::ReadDataSlot(QByteArray ba)                    // 读数据
{
    m_date.append(ba);

    //static uint32_t len;
    //static const char *c;
    //
    //len = ba.size();
    //c = ba.data();
    //emit ReadDataSignal(c, len);    // 对外发送信号（收到的数据）
    emit ReadDataSignal(ba);    // 对外发送信号（收到的数据）
}

void MyTcpClient::ConnectStatusSlot(const bool s)               // 当前连接状态（是否连接）
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__ << s;
    emit ConnectStatusSignal(s);    // 对外发送信号（是否连接）
}
