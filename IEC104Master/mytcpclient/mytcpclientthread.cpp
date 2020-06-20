#include "mytcpclientthread.h"

MyTcpClientThread::MyTcpClientThread(QHostAddress addr, quint16 port, QObject *parent)
    : QObject(parent)
    , m_hostAddr(addr)  // 服务器地址
    , m_port(port)          // 服务器端口
{
    // 注册类型
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<bool>("bool");

    m_recvDate.clear();
    m_frameDatas.clear();

    m_timer = new QTimer(this);
    QObject::connect(m_timer, &QTimer::timeout,
                     this, &MyTcpClientThread::timerOut);
    m_timer->start(10);

    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->abort();       // 中止当前连接并重置套接字

    // 读数据
    connect(m_tcpSocket, &QTcpSocket::readyRead,
            this, &MyTcpClientThread::ReadDataSlot);

    // 错误
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(ReadErrorSlot(QAbstractSocket::SocketError)));

    // 连接断开
    connect(m_tcpSocket, &QTcpSocket::disconnected,
            this, &MyTcpClientThread::DisConnectedSlot);
}


MyTcpClientThread::~MyTcpClientThread()
{
    delete m_tcpSocket;
}

/***********************************************************************************************
***************************************** 对内的槽函数 *******************************************
***********************************************************************************************/
void MyTcpClientThread::ReadErrorSlot(QAbstractSocket::SocketError socketError)
{
    m_tcpSocket->disconnectFromHost();
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__
             << (tr("连接出错：%1").arg(m_tcpSocket->errorString()));
    emit ConnectStatusSignal(false);
}

void MyTcpClientThread::ReadDataSlot()
{
    QByteArray ba;
    QByteArray frameData;

    // 读TCP中的数据
    ba = m_tcpSocket->readAll();
    m_recvDate.append(ba);      // 将数据存到缓冲区在

    while(m_recvDate.size() >= 6) {  //判断数据缓冲区的长度
        // 缓冲区中的第一个数是0x68（104的帧头）
        if((uint8_t)(m_recvDate.at(0)) == (uint8_t)(0x68)) {
            uint8_t ADU_Len = (uint8_t)(m_recvDate.at(1));  // 获取ADU长度
            // 判断缓冲区中的数据是否够一帧数据
            if(m_recvDate.size() < 2+ADU_Len) {             // 不够一帧，跳出循环
                break;
            }
            // 缓冲区中的数据刚好为一帧的数据
            else if(m_recvDate.size() == 2+ADU_Len) {
                frameData = m_recvDate.mid(0, 2+ADU_Len);   // 将数据保存到帧数据空间
                m_recvDate.clear();                         // 将缓冲区清空
            }
            // 缓冲区中的数据 > 一帧的数据
            else {
                frameData = m_recvDate.mid(0, 2+ADU_Len);       // 截取缓冲区的这一帧数据
                m_recvDate.remove(0, 2+ADU_Len);                // 将已经截取的数据移除
            }
            qDebug()  << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__
                      << tr("%1: %2")
                         .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                         .arg(QString(frameData.toHex(' ').toUpper()));
            m_mutex.lock();
            m_frameDatas.append(frameData);                 // 将数据保存
            m_mutex.unlock();
        }
        // 缓冲区中的第一个数不是0x68，删除第1个数
        else {
            m_recvDate.remove(0, 1);
        }
    }
}

void MyTcpClientThread::DisConnectedSlot()
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__
                << "disconnected" ;
    emit ConnectStatusSignal(false);
}

/***********************************************************************************************
***************************************** 对外的槽函数 *******************************************
***********************************************************************************************/
void MyTcpClientThread::ConnectSlot()     // 连接
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
    // 连接主机
    m_tcpSocket->connectToHost(m_hostAddr, m_port);
    if (m_tcpSocket->waitForConnected(1000)) {  // 连接超时等待时间
        qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__
                 << "连接成功";
        emit ConnectStatusSignal(true);
    }
    else {
        qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__
                 << "连接失败";
        emit ConnectStatusSignal(false);
    }
}

void MyTcpClientThread::DisConnectSlot()  // 断开
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
    // 断开主机
    m_tcpSocket->disconnectFromHost();
    emit ConnectStatusSignal(false);
}

void MyTcpClientThread::SendDataSlot(QByteArray ba)
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
    m_tcpSocket->write(ba);
}

/***********************************************************************************************
*****************************************  *******************************************
***********************************************************************************************/

void MyTcpClientThread::timerOut()
{
    static QByteArray frameData;

    m_mutex.lock();
    if(m_frameDatas.isEmpty()) {
        m_mutex.unlock();
        return;
    }
    frameData = m_frameDatas.at(0);             // 取第1个
    m_frameDatas.removeFirst();                 // 删除第1个
    m_mutex.unlock();

    emit ReadDataSignal(frameData);
}
