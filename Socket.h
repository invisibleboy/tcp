#ifndef SOCKET_H
#define SOCKET_H
#include"SbuTcp.h"
#include<QObject>
struct Segment
{
    struct sbutcphdr header;
    char* data;
};
struct SegmentWithSize{
    Segment *segment;
    int sizeOfdata;
    SegmentWithSize(Segment* segment,int sizeOfdata)
    {
        this->segment=segment;
        this->sizeOfdata=sizeOfdata;
    }
    SegmentWithSize()
    {
        segment=0;
        sizeOfdata=0;
    }

    bool operator ==(const SegmentWithSize s)const
    {
        return this->segment->header.th_seq==s.segment->header.th_seq;
    }
    bool operator<(const SegmentWithSize s)const
    {
        return this->segment->header.th_seq<s.segment->header.th_seq;
    }
};

class Socket{


private:

protected:
    char* serverHost;//Server IP Address
    int myPort;
    int hisPort;
    QList<SegmentWithSize> sendBuff;
    QList<SegmentWithSize> rcvBuff;
    unsigned long  seqNum;
    unsigned long iSeqNum;
    unsigned long  ackNum;
    void send(Segment *segment,bool hasdata,char *to,int sizeOfdata);
    uint16_t chkSum(SegmentWithSize* s);
    uint16_t chkSum(Segment* s);
    uint16_t in_chkSum(uint16_t * addr, int len);
    Segment* readFromRaw(ip *&iphdr);
    Segment* readFromRaw(ip*&iphdr,int &size);
//    static bool busyPort[64000];
    void printSegment(Segment *s);
    void printSegment(SegmentWithSize *s);
    Segment* ackCreator(Segment*);
public:


    Socket();
    ~Socket();
};


#endif // SOCKET_H
