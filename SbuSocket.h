#ifndef SBUSOCKET_H
#define SBUSOCKET_H
#include<vector>
#include<QString>
#include<QObject>
#include "Socket.h"
class SbuSocket:public QObject,Socket{
    Q_OBJECT
private:
    int congWin;
    int* ackArray;
    int lstByteAcked;
    int lstByteSent;
    int nextByteEx;
    float estimatedRTT;
    float TOI;
    enum State {SYN_SENT,ESTABILISHED,FIN_WAIT_1,FIN_WAIT_2,TIME_WAIT,CLOSED}state;
    Segment* synCreator();
    void TOCalculator(Segment* rcvd_segment);
private slots:
        void ackListener();
signals:
        void finished();
public:
    // this method creates a connection to server and creates // a thread to buffer and acknowledge arrived packets
    SbuSocket (char* serverHost, int serverPort);
    SbuSocket (char* serverHost, int serverPort,int port);
    SbuSocket (const SbuSocket & sbuSocket);
    SbuSocket();
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

    SbuSocket& operator =(const SbuSocket& sbuSocket);
};
#endif // SBUSOCKET_H
