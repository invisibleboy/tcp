#ifndef SBUSOCKET_H
#define SBUSOCKET_H
#include<vector>
#include<QString>
#include "Socket.h"
class SbuSocket:Socket {
private:
    enum State {SYN_SENT,ESTABILISHED,FIN_WAIT_1,FIN_WAIT_2,TIME_WAIT,CLOSED}state;
    Segment* synCreator();

public:
    // this method creates a connection to server and creates // a thread to buffer and acknowledge arrived packets
    SbuSocket (char* serverHost, int serverPort);
    SbuSocket (char* serverHost, int serverPort,int port);
    // read size bytes from SOCKET BUFFER and save it to
    // readBuffer and returns the number of bytes in the
    // readBuffer
    int read (char* readBuffer, int size);
    // if write buffer was greater than MSS then divide
    // writeBuffer to packet nnwith MSS size and then send
    // there
    bool write (char* writeBuffer, int size);
    // closes the socket and makes the resources free.
    bool close ();
};



#endif // SBUSOCKET_H
