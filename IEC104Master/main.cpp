#include "iec104master.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IEC104Master w;
    w.show();
    return a.exec();
}
