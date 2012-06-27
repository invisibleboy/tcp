#include "SbuServerSocket.h"
#include <QThread>
#include <QEventLoop>
SbuServerSocket::SbuServerSocket(int port)
{
    this->myPort=port;
    connectedSockets.clear();
}
Segment* SbuServerSocket::SynackCreator(Segment *rcvd_seg)
{
    Segment *segment = new Segment;
    srand(time(NULL)+1000);
    this->iSeqNum=(int)rand()%2000;//initial random sequence number
    this->seqNum=this->iSeqNum;
    segment->header.th_flags=18;//CWR,ECE,URG,ACK,PSH,RST,SYN,FIN
    segment->header.th_seq=this->seqNum;
    segment->header.th_dport=rcvd_seg->header.th_sport;
    segment->header.th_sport=this->myPort;
    segment->header.th_sum=0;
    segment->header.th_ack=rcvd_seg->header.th_seq+1;
    segment->header.th_off=6;
    segment->header.th_timestamp=rcvd_seg->header.th_timestamp;
    segment->header.th_sum=chkSum(segment);
    return segment;
}
SbuSocket SbuServerSocket::accept ()
{
    std::cout<<"wait for syn\n";
    while(1) {
        ip *iphdr;
        Segment* rcvd_segment= readFromRaw(iphdr);
        if(rcvd_segment->header.th_flags!=2)
            continue;
        if(chkSum(rcvd_segment)!=0)
        {
                std::cout<<"***********Server************checksum faild "<<std::endl;
            continue;
        }
        if(rcvd_segment->header.th_dport!=myPort)
            continue;
        std::cout<<">>>>>>>>>Server side:syn has been received from client:\n";
        Segment* synack=SynackCreator(rcvd_segment);

        send(synack,false,inet_ntoa(iphdr->ip_src),0);
        std::cout<<">>>>>>>>>Server side: Synack has been sent\n";
        //waiting for ack of synAck
        Segment* rcvd_segment2= readFromRaw(iphdr);
        if(chkSum(rcvd_segment2)!=0)
        {
            std::cout<<"***********Server************Corupted packet"<<std::endl;
            continue;
        }
        if(rcvd_segment2->header.th_dport!=myPort)
            continue;
        if(rcvd_segment2->header.th_flags!=16)
            continue;
        std::cout<<">>>>>>>>>Server side:ack of synAck has been received from client:\n";

        state=ESTABILISHED;

        SbuSocket new_socket(inet_ntoa(iphdr->ip_src),(int)rcvd_segment->header.th_sport,this->myPort);
        connectedSockets+=new_socket;
        return new_socket;
    }
    //emit this->acceptFinished();
}
