#include <QtCore/QCoreApplication>
#include<iostream>
#include"SbuSocket.h"
#include "SbuServerSocket.h"
using namespace std;
int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
//    std::cout<<"salam\n";
    SbuSocket *client;
//    SbuServerSocket *server;
//    server= new SbuServerSocket(100);
//    std::cout<<"salam1\n";
//    server->accept();
    std::cout<<"salam1\n";
//    sleep(1);
    client=new SbuSocket("192.168.100.4",100);
    return a.exec();
}
