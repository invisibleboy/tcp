#ifndef SBUSERVERSOCKET_H
#define SBUSERVERSOCKET_H
#include "SbuSocket.h"
#include <vector>
#include <QList>
#include <QThread>

class SbuServerSocket:public QObject,Socket {
    Q_OBJECT
private:
    // the list of connected sockets to this server
    QList<SbuSocket> connectedSockets;
    enum State {LISTEN,SYN_RCVD,ESTABILISHED,CLOSE_WAIT,LAST_ACK,CLOSED} state;
    Segment* SynackCreator(Segment *rcvd_seg);
public:
    // this method creates a thread to accept connection from
    // client socket
    SbuServerSocket (int port); 
    // this method remove a socket object from first of
    // connected sockets or wait to a connection established.
    void close ();
public slots:
    SbuSocket accept ();
    // closes the socket and makes the resources free.
};

#endif // SBUSERVERSOCKET_H
