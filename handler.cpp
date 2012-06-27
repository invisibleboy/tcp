#include "handler.h"

void socketHandler::read(){
    std::cout<<"\n\nread\n";
    int dfs=10000;
    char* df= new char[dfs];
    std::cout<<"rcvd data:\n";
    int g=s.read(df,dfs);
    std::cout<<g<<std::endl;
    for(int i=0;i<g;i++)
        std::cout<<(char)df[i];
    std::cout<<"\ntransmission finished!"<<std::endl;
}
socketHandler::socketHandler(SbuSocket d)
{

    s=d;
    QThread* thread = new QThread;
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(read()));
//    connect(this, SIGNAL(finished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
