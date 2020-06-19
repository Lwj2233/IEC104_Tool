#include "iec104master.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IEC104Master w;

    return a.exec();
}
