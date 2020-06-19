#include "iec104master.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IEC104Master m(QHostAddress("192.168.1.101"));
    m.Connect();

    QObject::connect(&m, &IEC104Master::upData,
                     [](QMap<uint32_t, uint8_t> m_data_yx,
                     QMap<uint32_t, float> m_data_yc) {
        qDebug() << "_________________________________________________________";
        qDebug() << m_data_yx;
        qDebug() << m_data_yc;
        qDebug() << "---------------------------------------------------------";

    });

    return a.exec();
}
