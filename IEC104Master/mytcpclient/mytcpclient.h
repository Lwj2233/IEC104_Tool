#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include <QThread>
#include <QHostAddress>
#include "mytcpclientthread.h"

class MyTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpClient(QHostAddress addr, quint16 port, QObject *parent = nullptr);
    ~MyTcpClient();

// 对外的接口
public slots:
    void Connect();     // 连接
    void DisConnect();  // 断开
    void SendData(QByteArray ba);

// 对外的信号
signals:
    //void ReadDataSignal(const char *, quint32 len);   // 读数据
    void ReadDataSignal(QByteArray);                    // 读数据
    void ConnectStatusSignal(const bool s);             // 当前连接状态（是否连接）

// 对TcpThread的槽函数
private slots:
    //void ReadDataSlot(const char *, quint32 len);     // 读数据
    void ReadDataSlot(QByteArray);                      // 读数据
    void ConnectStatusSlot(const bool s);               // 当前连接状态（是否连接）

// 对TcpThread的信号
signals:
    void ConnectSignal();     // 连接
    void DisConnectSignal();  // 断开
    void SendDataSignal(QByteArray ba);

private:
    MyTcpClientThread *m_tcpClientThread;
    QThread *m_tcpClientThreadTh;

    QByteArray m_date;
};

#endif // MYTCPCLIENT_H
