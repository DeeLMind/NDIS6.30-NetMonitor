#pragma once
#include<Windows.h>
#include"Networks_User.h"
//�õ��ĺ�
#define Tranverse16(X)   ((((UINT16)(X) & 0xff00) >> 8) |(((UINT16)(X) & 0x00ff) << 8))    //����USHORT���С��ת��
#define SET_INFO_TYPE(A,B) (A->Type=B)

//PacketInfo->type
#define INFO_UNKNOWN 0
#define INFO_ARP     1
#define INFO_RARP    2
#define INFO_TCP     3
#define INFO_UDP     4
#define INFO_ICMP    5
#define INFO_IGMP    6
#define INFO_HTTP    7
#define INFO_NAT     8
#define INFO_DHCP    9
#define INFO_IPv6    10
#define INFO_QICQ    11
#define INFO_NTP     12
#define INFO_SSDPv4  13 

//ARP->opcode
#define ARP_REQUEST  1
#define ARP_REPLY    2

//DHCP->messageType
#define DHCP_REQUEST 1
#define DHCP_REPLY   2  

//TCP->flagsOffset  XΪflagsOffset
#define TCP_TEST_FIN(X)       Tranverse16(X) & 0x1
#define TCP_TEST_SYN(X)       Tranverse16(X) >> 1 & 0x1
#define TCP_TEST_RST(X)		  Tranverse16(X) >> 2 & 0x1
#define TCP_TEST_PSH(X)		  Tranverse16(X) >> 3 & 0x1
#define TCP_TEST_ACK(X)		  Tranverse16(X) >> 4 & 0x1
#define TCP_TEST_URG(X)		  Tranverse16(X) >> 5 & 0x1
#define TCP_GETDATAOFFSET(X)  Tranverse16(X) >> 12

typedef struct _PacketInfo
{
	int Type;     //���ݰ����� ��INFO_XXX��
	int Size;     //���ݰ���С
	BOOLEAN IsSendPacket;
	MAC Mac;      //������·��
	union 
	{
		IPPacket Ip;
		ARPPacket Arp;
	}protocol;    //�����
	union 
	{
		TCPPacket Tcp;
		UDPPacket Udp;
		ICMPPacket Icmp;
		IGMPPacket Igmp;
	}protocol1;  //�����
	union 
	{
		QICQPacket Qicq;
		DHCPPacket Dhcp;
	}protocol2;
	UCHAR RawPacket[2000];     //ԭʼ�İ����ݣ�MTU<=1500��
}PacketInfo,*PPacketInfo;