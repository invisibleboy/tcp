#include "SbuSocket.h"
#include <QThread>
#include<QTimer>
#include<QSignalMapper>
#include<QMap>
using namespace std;

SbuSocket::SbuSocket()
{

}
SbuSocket::SbuSocket (char* serverHost, int serverPort)
{

    congWin=2*MSS;
    estimatedRTT=2;
    lstByteAcked=-1;
    lstByteSent=0;
    nextByteEx=0;
    TOI=2000;
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
        if(chkSum(rcvd_segment)!=0)
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
//        printSegment(rcvd_segment);
        TOCalculator(rcvd_segment);
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
    this->myPort=sbuSocket.myPort;
    this->congWin=2;
    this->estimatedRTT=0;
    this->lstByteAcked=this->lstByteSent=0;
    this->nextByteEx=0;
    this->serverHost=sbuSocket.serverHost;
    this->hisPort=sbuSocket.hisPort;
    this->state=sbuSocket.state;
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
    segment->header.th_timestamp=getCurrentTime();
    segment->header.th_sum=chkSum(segment);
    return segment;
}
SbuSocket& SbuSocket::operator =(const SbuSocket& sbuSocket)
{
    this->myPort=sbuSocket.myPort;
    this->congWin=2;
    this->estimatedRTT=0;
    this->lstByteAcked=this->lstByteSent=0;
    this->nextByteEx=0;
    this->serverHost=sbuSocket.serverHost;
    this->hisPort=sbuSocket.hisPort;
    this->state=sbuSocket.state;
    return *this;
}
bool SbuSocket::write (char* writeBuffer, int size)
{
    //    QThread* thread = new QThread;
    ackArray = new int[size];
    timers = new QTimer*[size];
    memset(ackArray,0,size);
    //    this->moveToThread(thread);
    //    connect(thread, SIGNAL(started()), this, SLOT(ackListener()));
    //    connect(this, SIGNAL(finished()), thread, SLOT(quit()));
    //    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    this->start();

    Segment* segment = new Segment;
    segment->header.th_dport=this->hisPort;
    segment->header.th_off=6;
    segment->header.th_sport=this->myPort;
    segment->header.th_flags=0;
    segment->header.th_ack=0;
    int t_size=size;
    while(t_size>0){
        if(lstByteAcked-lstByteSent-congWin<=MSS)
        {
            //            cout<<"sending";
            cout.flush();
            segment->header.th_sum=0;
            segment->header.th_timestamp=getCurrentTime();
            segment->header.th_seq=this->seqNum;
            int dataSize=((t_size>MSS) ? MSS:t_size);
            for(int i=0;i<dataSize;i++)
                segment->data[i]=writeBuffer[this->seqNum-this->iSeqNum-1+i];
            segment->header.th_flags=((t_size>MSS) ? 0:8);
            this->seqNum+=dataSize;
            SegmentWithSize *t=new SegmentWithSize(segment,dataSize);
            segment->header.th_sum=chkSum(t);
            QTimer *t1=new QTimer();
            QSignalMapper *signalMapper=new QSignalMapper;
            connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(retransmitTimeout(int)));
            signalMapper->setMapping(t1,(int)segment->header.th_seq);
            connect(t1,SIGNAL(timeout()),signalMapper,SLOT(map()));
            //            connect(t1,SIGNAL(timeout()),this,SLOT(test()));
            timers[segment->header.th_seq-iSeqNum]=t1;
            send(segment,true,serverHost,dataSize);
            t1->start(this->TOI);
            //            cout<<"this->TOI"<<this->TOI;;
            t_size-= dataSize;
        }
    }
    //    emit finished();
    delete ackArray;
}
void SbuSocket::test()
{
    cout<<"test\n";
    cout.flush();
    //    exit(1);
}

void SbuSocket::retransmitTimeout(int sequence)
{
    cout<<"retransmition segment:\n";
    //    cout.flush();
    SegmentWithSize d;
    Segment *s=new Segment;
    s->header.th_seq=sequence;
    d.segment=s;
    if(sendBuff.contains(d))
    {
        int index=sendBuff.indexOf(d);
        printSegment(&sendBuff[index]);
        send(sendBuff[index].segment,true,serverHost,sendBuff[index].sizeOfdata);
        QTimer *t=(QTimer *)sender();
        t->start(TOI);
        //    cout<<"this->TOI"<<this->TOI;;
    }
}

void SbuSocket::run()
{

    while(1)
    {

        ip *iphdr= new ip;
        Segment* rcvd_segment= readFromRaw(iphdr);
        //        printSegment(rcvd_segment);
        if(chkSum(rcvd_segment)!=0)
        {
            //TODO message
            std::cout<<"Corrupt ack packet";
            continue;
        }
        if(rcvd_segment->header.th_dport!=myPort)
            continue;
        if(rcvd_segment->header.th_sport!=hisPort)
        {
            cout<<"port check failed\n";
            continue;
        }
        if(strcmp(inet_ntoa(iphdr->ip_src),serverHost)!=0)
        {
            cout<<"ip check failed\n";
            continue;
        }
        if(rcvd_segment->header.th_flags!=16)
            continue;
        cout<<"************************************************ack recieved:"<<rcvd_segment->header.th_ack<<endl;
        //if(timers[rcvd_segment->header.th_ack-iSeqNum]!=NULL)
        timers[rcvd_segment->header.th_ack-1-iSeqNum]->stop();
        TOCalculator(rcvd_segment);
        if(rcvd_segment->header.th_ack>lstByteAcked)
        {
            //@TODO TIMER & congwin
            congWin+=min((MSS^2/congWin),1);
            lstByteAcked=rcvd_segment->header.th_ack;
            ackArray[rcvd_segment->header.th_ack-iSeqNum]++;
        }
        else
        {
            //@TODO TIMER & congwin
            if(ackArray[rcvd_segment->header.th_ack]==3)
            {
                SegmentWithSize d;
                Segment *s=new Segment;
                s->header.th_seq=rcvd_segment->header.th_ack;
                d.segment=s;
                Segment retransmitSegment = *sendBuff[sendBuff.indexOf(d)].segment;
                send(&retransmitSegment,true,serverHost,sendBuff[sendBuff.indexOf(d)].sizeOfdata);
                ackArray[rcvd_segment->header.th_ack-iSeqNum]=0;
                congWin=2*MSS;
            }
        }
    }
    exec();

}
void SbuSocket::TOCalculator(Segment* rcvd_segment)//time out calculator
{

    uint32_t curr=getCurrentTime();
    float sampleRTT=(curr- rcvd_segment->header.th_timestamp);
    if(sampleRTT<0)
    {
        sampleRTT+=10000000000;
    }
    sampleRTT *= 0.001;//converting to milisecond
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
        SegmentWithSize *rcvd = new SegmentWithSize(rcvd_segment,segmentDataSize);
        //        printSegment(rcvd);
        cout<<"asasasas\n";
        if( chkSum(rcvd)!=0)
        {
            //TODO message
            std::cout<<"Corrupt packet\n";
            continue;
        }
        if(rcvd_segment->header.th_dport!=myPort)
            continue;
        if(rcvd_segment->header.th_sport!=hisPort)
        {
            cout<<"port check failed\n";
            continue;
        }
        if(strcmp(inet_ntoa(iphdr->ip_src),serverHost)!=0)
        {
            cout<<"ip check failed\n";
            continue;
        }
        if(rcvdBytes+segmentDataSize<=size)
        {
            rcvdBytes+=segmentDataSize;
        }
        else
        {
            cout<<"Error#32: buffer overflow";
        }
        Segment * ackSegment=ackCreator(rcvd_segment);
        send(ackSegment,false,serverHost,0);
        if(rcvd_segment->header.th_flags==8 )
        {
            qSort(rcvBuff.begin(),rcvBuff.end());
            int k=0;
            for(int i=0;i<rcvBuff.size();i++)
            {
                for(int j=0;j<rcvBuff.at(i).sizeOfdata;j++,k++)
                {
                    readBuffer[k]=rcvBuff.at(i).segment->data[j];
                }
            }
            return rcvdBytes;
        }
    }
}
