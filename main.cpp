#include <QtCore/QCoreApplication>
#include<iostream>
#include <QThread>
#include"SbuSocket.h"
#include "SbuServerSocket.h"
#include "handler.h"
using namespace std;
//class socketHandler:public QObject{
//    Q_OBJECT
//    SbuSocket s;
//public:
//    socketHandler(SbuSocket d){
//        s=d;
//        QThread* thread = new QThread;
//        this->moveToThread(thread);
//        connect(thread, SIGNAL(started()), this, SLOT(read()));
//        connect(this, SIGNAL(finished()), thread, SLOT(quit()));
//        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//        thread->start();
//    }
//public slots:
//    void read(){
//        char* df;
//        int dfs=10000;
//        s.read(df,dfs);
//    }
//signals:
//    void finished();
//};

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    SbuServerSocket *server;
    server= new SbuServerSocket(100);
    while(1){
        SbuSocket reciver=server->accept();
        std::cout<<"one connected!\n";
        socketHandler *d = new socketHandler(reciver);
    }
    return a.exec();
}

