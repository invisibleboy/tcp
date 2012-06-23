#ifndef SOCKET_H
#define SOCKET_H
#include"SbuTcp.h"
struct Segment
{
    struct sbutcphdr header;
    char data[MSS];
};
class Socket{
private:

protected:
    char* serverHost;//Server IP Address
    int myPort;
    int hisPort;
    unsigned long  seqNum;
    unsigned long  ackNum;
    void send(Segment*,bool,char *);
    uint16_t chkSum(Segment* s,bool hasData );
    uint16_t in_chkSum(uint16_t * addr, int len);
    Segment* readFromRaw(ip *&iphdr);
//    static bool busyPort[64000];
    void printSegment(Segment *s);
    Segment* ackCreator(Segment*);
public:
};

#endif // SOCKET_H
