#ifndef HANDLER_H
#define HANDLER_H
#include <QtCore/QCoreApplication>
#include <iostream>
#include <QThread>
#include "SbuSocket.h"
#include "SbuServerSocket.h"
class socketHandler:public QObject{
    Q_OBJECT
public:
    SbuSocket s;
    socketHandler(SbuSocket s);



public slots:
    void read();
signals:
    void finished();
};

#endif // HANDLER_H
