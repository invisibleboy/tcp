#include "Socket.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<netinet/tcp.h>
#include<iostream>
#include<bitset>
#include <sys/ioctl.h>
#include <sys/time.h>
using namespace std;


void Socket::send(Segment *segment,bool hasData,char * to,int sizeOfData)
{
//    cout<<"Socket::send()\n";
    cout.flush();
    int dontblock = 0;
    int rc = ioctl(rawSocket, FIONBIO, (char *) &dontblock);
    //destination Address
    struct sockaddr_in dst_addr;
    //destination address type = ipv4
    dst_addr.sin_family = AF_INET;
    // not needed in SOCK_RAW
    dst_addr.sin_port = 0;
    // convert ip from char* to binary
    inet_pton(AF_INET, to, (struct in_addr *)&dst_addr.sin_addr.s_addr);
    // some byte that needed to be zero in address structure:
    memset(dst_addr.sin_zero, 0, sizeof(dst_addr.sin_zero));
    // cast segment to char * for send
    char *packet=(char *)segment;
    // size of segment that will send
    int sendSize=sizeof(sbutcphdr)+((hasData)?sizeOfData:0);
    // now we  can send our segment
    if (sendto(rawSocket, packet,sendSize ,0,(struct sockaddr *)&dst_addr, (socklen_t)sizeof(dst_addr)) < 0)
        cout<<"cann't send datam \n"<<"rawSocket, sendSize"<<rawSocket<<sendSize;
    SegmentWithSize t;
    t.segment=segment;
    t.sizeOfdata=sizeOfData;
    sendBuff+=t;
}
uint16_t Socket::chkSum(Segment* s)
{
    return (in_chkSum((uint16_t *)s,sizeof(sbutcphdr) ));
}
uint16_t Socket::chkSum(SegmentWithSize* s)
{
    return (in_chkSum((uint16_t *)s->segment, sizeof(s->sizeOfdata) + sizeof(sbutcphdr)) );
}
uint16_t Socket::in_chkSum (uint16_t * addr, int len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    /*
* Our algorithm is simple, using a 32 bit accumulator (sum), we add
* sequential 16 bit words to it, and at the end, fold back all the
* carry bits from the top 16 bits into the lower 16 bits.
*/
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        * (unsigned char *) (&answer) = * (unsigned char *) w;
        sum += answer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);     /* add carry */
    answer = ~sum;     /* truncate to 16 bits */
    return (answer);
}
Segment* Socket::readFromRaw(ip* &iphdr)
{
    int recv_length;
    char *packet=new char[800];
    memset(packet, 0, sizeof(packet));
    recv_length = recv(rawSocket, packet, 800, 0);
    struct ip *ipv4;
    ipv4 = (struct ip*) packet;
    iphdr=ipv4;
    char *t=(char*)packet+20;
    Segment * rcvd_segment=(struct Segment *)(t);
    SegmentWithSize sws;
    sws.segment=rcvd_segment;
    sws.sizeOfdata=recv_length-rcvd_segment->header.th_off*4;;
    return rcvd_segment;
}
Segment* Socket::readFromRaw(ip* &iphdr,int &size)
{

    int recv_length;
    char *packet=new char[800];
    memset(packet, 0, sizeof(packet));
    recv_length = recv(rawSocket, packet, 800, 0);
    struct ip *ipv4;
    ipv4 = (struct ip*) packet;
    iphdr=ipv4;
    char *t=(char*)packet+20;
    Segment * rcvd_segment=(struct Segment *)(t);
    size=recv_length-rcvd_segment->header.th_off*4-20;
    SegmentWithSize sws;
    sws.segment=rcvd_segment;
    sws.sizeOfdata=size;
    rcvBuff<<sws;
    return rcvd_segment;
}

void Socket::printSegment(Segment *s)
{
    std::cout<<"Sport: "<<s->header.th_sport<<" Dport: "<<s->header.th_dport<<" Seq num: "<< s->header.th_seq<<" Ack num:"<<s->header.th_ack<<" flag: "<<(int)s->header.th_flags<<" offset: "<<s->header.th_off<<" SUM: "<<s->header.th_sum<<endl;
}
void Socket::printSegment(SegmentWithSize *s)
{
    std::cout<<"segment with size"<<s->sizeOfdata<<std::endl;
    std::cout<<"Sport: "<<s->segment->header.th_sport<<" Dport: "<<s->segment->header.th_dport<<" Seq num: "<< s->segment->header.th_seq<<" Ack num:"<<s->segment->header.th_ack<<" flag: "<<(int)s->segment->header.th_flags<<" offset: "<<s->segment->header.th_off<<" SUM: "<<s->segment->header.th_sum<<endl;
    for(int i=0;i<s->sizeOfdata;i++)
        std::cout<<(uint8_t)s->segment->data[i];
    std::cout<<std::endl;
}
Segment* Socket::ackCreator(Segment* synack)
{
    Segment* segment= new Segment;
    this->seqNum=this->seqNum+1;//plus plus sequence number
    segment->header.th_flags=16;//CWR,ECE,URG,ACK,PSH,RST,SYN,FIN
    segment->header.th_seq=this->seqNum;
    segment->header.th_ack=synack->header.th_seq+1;
    segment->header.th_dport=this->hisPort;
    segment->header.th_sport=this->myPort;
    segment->header.th_sum=0;
    segment->header.th_off=6;
    segment->header.th_timestamp=getCurrentTime();
    segment->header.th_sum=chkSum(segment);
    return segment;
}

uint32_t Socket::getCurrentTime()
{
    timeval d;
    gettimeofday(&d, NULL);
    return (uint32_t)(d.tv_usec+1000000*(d.tv_sec%10000));
}

Socket::Socket()
{
    if ((rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        std::cout<<"error";
    }
    int dontblock = 0;
    int rc = ioctl(rawSocket, FIONBIO, (char *) &dontblock);
}

