#ifndef SBUSERVERSOCKET_H
#define SBUSERVERSOCKET_H
#include "SbuSocket.h"
#include <vector>
#include <QList>
#include <QThread>

class SbuServerSocket:Socket,QThread {
private:
    // the list of connected sockets to this server
    QList<SbuSocket> connectedSockets;
    enum State {LISTEN,SYN_RCVD,ESTABILISHED,CLOSE_WAIT,LAST_ACK,CLOSED} state;
    Segment* SynackCreator(Segment *rcvd_seg);
public:
    void run();
    // this method creates a thread to accept connection from
    // client socket
    SbuServerSocket (int port); 
    // this method remove a socket object from first of
    // connected sockets or wait to a connection established.
    SbuSocket accept ();
    // closes the socket and makes the resources free.
    void close ();
};

#endif // SBUSERVERSOCKET_H
