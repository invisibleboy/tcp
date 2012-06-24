#ifndef SBUTCP_H
#define SBUTCP_H

#include <iostream>
#include <sys/types.h>
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
#include <inttypes.h>
#define MSS 536
#define PROTOCOL_NUM IPPROTO_RAW
#define BUFSIZE     1500
#define ALPHA 0.125
#define BETA 2
struct sbutcphdr {
   uint16_t th_sport;
   uint16_t th_dport;
   uint32_t th_seq;
   uint32_t th_ack;
   uint32_t   th_off:4;
   uint8_t  th_flags;
   uint32_t th_timestamp;
   uint16_t th_sum;
};
#endif // SBUTCP_H
