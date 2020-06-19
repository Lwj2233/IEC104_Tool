#ifndef MYTCPCLIENTTHREAD_H
#define MYTCPCLIENTTHREAD_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include <QMutex>
#include <QHostAddress>

class MyTcpClientThread : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpClientThread(QHostAddress addr, quint16 port, QObject *parent = nullptr);
    ~MyTcpClientThread();

public:

// 对外的槽函数
public slots:
    void ConnectSlot();     // 连接
    void DisConnectSlot();  // 断开
    void SendDataSlot(const char *, const quint32 len);

// 对内的槽函数
private slots:
    void ReadDataSlot();                                // 读数据
    void ReadErrorSlot(QAbstractSocket::SocketError);   // 接收错误信息
    void DisConnectedSlot();                            // 接收连接断开信号

// 对外的信号
signals:
    //void ReadDataSignal(const char *, quint32 len);     // 读数据
    void ReadDataSignal(QByteArray);                  // 读数据
    void ConnectStatusSignal(const bool s);             // 当前连接状态（是否连接）


private slots:
    void timerOut();
private:
    QTcpSocket *m_tcpSocket;

    QHostAddress m_hostAddr;            // 服务器IP
    quint16 m_port;                     // 服务器端口


    QByteArray m_recvDate;              // 接收数据缓冲区
    QMutex m_mutex;                     // 访问帧数据是加锁
    QList<QByteArray> m_frameDatas;     // 存放按帧解析开的数据

    QTimer *m_timer;
};

#endif // MYTCPCLIENTTHREAD_H
