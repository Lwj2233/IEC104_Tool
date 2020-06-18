#ifndef APP_H
#define APP_H

#include <QObject>

class App : public QObject
{
    Q_OBJECT
public:
    explicit App(QObject *parent = nullptr);

    static QStringList YX_Names;
    static QStringList YC_Names;

signals:

};

#endif // APP_H
