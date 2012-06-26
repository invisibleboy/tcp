#include "SbuSocket.h"
#include <QFuture>
#include<QtConcurrentRun>
using namespace std;
SbuSocket::SbuSocket()
{

}

SbuSocket::SbuSocket (char* serverHost, int serverPort)
{

    congWin=2;
    estimatedRTT=0;
    lstByteAcked=lstByteSent=0;
    nextByteEx=0;
    srand(time(NULL));
    int t=(int)rand()%63999+1000;

    this->myPort=t;

    //set class members
    this->serverHost=serverHost;
    this->hisPort=serverPort;

    state=CLOSED;
    //sending SYN
    Segment *segment= synCreator();
    send(segment,false,serverHost,0);        //send only header of sbutcp
    std::cout<<">>>>>>>>>client side: syn has been sent\n";
    //    printSegment(segment);
    state=SYN_SENT;     //change state of connection
    //waiting for SYN_ACK from server
    Segment* rcvd_segment;
    while(1) {
        ip *iphdr;
        rcvd_segment= readFromRaw(iphdr);
        if(chkSum(&SegmentWithSize(rcvd_segment,0))!=0)
        {
            cout<<"client side chksum checked!\n";
            continue;
        }
        if(rcvd_segment->header.th_dport!=myPort)
        {
            cout<<"client side des port checked!\n";
            continue;
        }
        if(rcvd_segment->header.th_flags!=18)
        {
            cout<<"client side flags checked!\n";
            continue;
        }
        std::cout<<">>>>>>>>>Client side : Synack receive from server:\n";
        this->nextByteEx=rcvd_segment->header.th_seq+1;
        break;
    }
    Segment* synAck_Ack=ackCreator(rcvd_segment);
    send(synAck_Ack,false,serverHost,0);
    std::cout<<">>>>>>>>>Client side : Synack_ack has been sent\n";
    state = ESTABILISHED;
}
SbuSocket::SbuSocket (char* serverHost, int serverPort,int port)
{
    //set class members
    congWin=2;
    estimatedRTT=0;
    lstByteAcked=lstByteSent=0;
    nextByteEx=0;
    this->serverHost=serverHost;
    this->hisPort=serverPort;
    this->myPort=port;
    this->state=CLOSED;

}
SbuSocket::SbuSocket (const SbuSocket & sbuSocket)
{

}
Segment* SbuSocket::synCreator()
{
    Segment *segment = new Segment;
    srand(time(NULL));
    this->iSeqNum=(int)rand()%2000;//initial random sequence number
    this->seqNum=this->iSeqNum;
    segment->header.th_flags=2;//CWR,ECE,URG,ACK,PSH,RST,SYN,FIN
    segment->header.th_seq=this->seqNum;
    segment->header.th_ack=0;
    segment->header.th_dport=this->hisPort;
    segment->header.th_sport=this->myPort;
    segment->header.th_sum=0;
    segment->header.th_off=6;
    segment->header.th_timestamp=(unsigned long)time(NULL);
    segment->header.th_sum=chkSum(segment);
    return segment;
}
SbuSocket& SbuSocket::operator =(const SbuSocket& sbuSocket)
{

}

bool SbuSocket::write (char* writeBuffer, int size)
{
    QThread* thread = new QThread;
    ackArray = new int[size];
    memset(ackArray,0,size);
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(ackListener()));
    connect(this, SIGNAL(finished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

    Segment* segment = new Segment;
    segment->header.th_dport=this->hisPort;
    segment->header.th_off=6;
    segment->header.th_sport=this->myPort;
    segment->header.th_flags=0;
    segment->header.th_ack=0;
    int t_size=size;
    while(t_size>0){
        if(lstByteAcked-lstByteSent<=congWin)
        {
            segment->header.th_sum=0;
            segment->header.th_timestamp=(uint32_t)time(NULL);
            segment->header.th_seq=this->seqNum;
            int dataSize=((t_size>MSS) ? MSS:t_size);
            segment->data=writeBuffer+(this->seqNum-this->iSeqNum);
            segment->header.th_flags=((t_size>MSS) ? 0:8);
            this->seqNum+=dataSize;
            SegmentWithSize t;
            t.segment=segment;
            t.sizeOfdata=dataSize;
            segment->header.th_sum=chkSum(&t);
            send(segment,true,serverHost,dataSize);
            t_size-= dataSize;
        }
    }
    emit finished();
    delete ackArray;
}
void SbuSocket::ackListener()
{
    while(1)
    {
        ip *iphdr= new ip;
        Segment* rcvd_segment= readFromRaw(iphdr);
        if(chkSum(rcvd_segment)!=0)
        {
            //TODO message
            std::cout<<"Corrupt ack packet";
            continue;
        }
        if(rcvd_segment->header.th_dport!=myPort)
            continue;
        if(rcvd_segment->header.th_flags!=2)
            continue;
        TOCalculator(rcvd_segment);
        if(rcvd_segment->header.th_ack>lstByteAcked)
        {
            //@TODO TIMER & congwin
            lstByteAcked=rcvd_segment->header.th_ack;
            ackArray[rcvd_segment->header.th_ack-iSeqNum]++;
        }
        else
        {
            //@TODO TIMER & congwin
            if(ackArray[rcvd_segment->header.th_ack]==3)
            {
                SegmentWithSize d;
                d.segment->header.th_seq=rcvd_segment->header.th_ack;
                Segment retransmitSegment = *sendBuff[sendBuff.indexOf(d)].segment;
                send(&retransmitSegment,true,serverHost,sendBuff[sendBuff.indexOf(d)].sizeOfdata);
                ackArray[rcvd_segment->header.th_ack-iSeqNum]=0;
            }
        }
    }
}

void SbuSocket::TOCalculator(Segment* rcvd_segment)//time out calculator
{
    float sampleRTT=((uint32_t)time(NULL) - rcvd_segment->header.th_timestamp);
    estimatedRTT = ALPHA*estimatedRTT + (1-ALPHA)*sampleRTT;
    TOI=BETA*estimatedRTT;
    return;
}
int SbuSocket::read (char* readBuffer, int size)
{
    int rcvdBytes=0;
    while(1)
    {
        int segmentDataSize;
        ip *iphdr= new ip;
        Segment* rcvd_segment= readFromRaw(iphdr,segmentDataSize);

        if(chkSum(rcvd_segment)!=0)
        {
            //TODO message
            std::cout<<"Corrupt packet";
            continue;
        }
        if(rcvd_segment->header.th_dport!=myPort)
            continue;

        if(rcvdBytes+segmentDataSize<size)
        {
            rcvdBytes+=size;
        }
        else
        {
            cout<<"Error#32: buffer overflow";
        }
        Segment * ackSegment=ackCreator(rcvd_segment);
        send(ackSegment,false,serverHost,0);
        if(rcvd_segment->header.th_flags==8 )
        {

        }
    }
}
