#
QT       += core network

INCLUDEPATH += $$PWD

HEADERS  += \
        $$PWD/IEC104.h  \
        $$PWD/iec104master.h

SOURCES += \
        $$PWD/iec104master.cpp

#FORMS += \
#        $$PWD/mytcpclient.ui 

#RESOURCES += \
#   $$PWD/mytcpclient.qrc 

#include(./mytcpclient/mytcpclient.pri)
