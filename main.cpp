#include <QtCore/QCoreApplication>
#include<iostream>
#include <QThread>
#include"SbuSocket.h"
#include "SbuServerSocket.h"
using namespace std;
class socketHandler:public QObject{
    Q_OBJECT
    SbuSocket s;
    socketHandler(SbuSocket d){
        s=d;
        QThread* thread = new QThread;
        this->moveToThread(thread);
        connect(thread, SIGNAL(started()), this, SLOT(read()));
        connect(this, SIGNAL(finished()), thread, SLOT(quit()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
    }
public slots:
    void read(){
        char* df;
        int dfs=10000;
        s.read(df.dfs);
    }
signals:
    void finished();
};

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
//    std::cout<<"salam\n";
    SbuSocket *client;
    SbuServerSocket *server;
    server= new SbuServerSocket(100);
    sleep(5);
//    std::cout<<"salam1\n";
    while(1){
        SbuSocket reciver=server->accept();
        socketHandler d = new socketHandler(reciver);
    }

//    client= new SbuSocket("127.0.0.1",100);
//    client->write("asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;",700);
//    std::cout<<"salam1\n";
//    sleep(1);
//    client=new SbuSocket("192.168.100.4",100);
    return a.exec();
}

