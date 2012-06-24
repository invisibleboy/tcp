#include "SbuSocket.h"

using namespace std;

SbuSocket::SbuSocket (char* serverHost, int serverPort)
{

    srand(time(NULL));
    int t=(int)rand()%63999+1000;

    this->myPort=t;

    //set class members
    this->serverHost=serverHost;
    this->hisPort=serverPort;

    state=CLOSED;
    //sending SYN
    Segment *segment= synCreator();
    send(segment,false,serverHost);        //send only header of sbutcp
    std::cout<<">>>>>>>>>client side: syn has been sent\n";
//    printSegment(segment);
    state=SYN_SENT;     //change state of connection
    //waiting for SYN_ACK from server
    Segment* rcvd_segment;
    while(1) {
        ip *iphdr;
        rcvd_segment= readFromRaw(iphdr);
        if(chkSum(rcvd_segment,false)!=0)
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
        break;
    }
    Segment* synAck_Ack=ackCreator(rcvd_segment);
    send(synAck_Ack,false,serverHost);
    std::cout<<">>>>>>>>>Client side : Synack_ack has been sent\n";
    state = ESTABILISHED;
}
SbuSocket::SbuSocket (char* serverHost, int serverPort,int port)
{
    //set class members
    this->serverHost=serverHost;
    this->hisPort=serverPort;
    this->myPort=port;
    this->state=CLOSED;

}
Segment* SbuSocket::synCreator()
{
    Segment *segment = new Segment;
    srand(time(NULL));
    this->seqNum=(int)rand()%2000;//initial random sequence number
    segment->header.th_flags=2;//CWR,ECE,URG,ACK,PSH,RST,SYN,FIN
    segment->header.th_seq=this->seqNum;
    segment->header.th_ack=0;
    segment->header.th_dport=this->hisPort;
    segment->header.th_sport=this->myPort;
    segment->header.th_sum=0;
    segment->header.th_off=6;
    segment->header.th_timestamp=(unsigned long)time(NULL);
    segment->header.th_sum=chkSum(segment,false);
    return segment;
}
