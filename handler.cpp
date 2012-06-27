#include "handler.h"

void socketHandler::read(){
    std::cout<<"\n\nread\n";
    char* df;
    int dfs=10000;
    int g=s.read(df,dfs);
}
socketHandler::socketHandler(SbuSocket d)
{

    SbuSocket s=d;
    QThread* thread = new QThread;
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(read()));
//    connect(this, SIGNAL(finished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
