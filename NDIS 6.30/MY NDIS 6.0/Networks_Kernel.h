#pragma once
#pragma warning(disable:4214)
#include"Struct.h"

//�����жϰ�����
#define PACKET_IP   0x0800
#define PACKET_IPv6 0X86DD
#define PACKET_ARP  0X0806
#define PACKET_RARP 0X0835
#define PACKET_TCP  0X6
#define PACKET_IGMP 0X02
#define PACKET_ICMP 0x01
#define PACKET_UDP  0X11

//���а������ݽṹ
typedef struct _MAC
{
	UCHAR dst[6];
	UCHAR sou[6];
	USHORT type;
}MAC, *PMAC;
typedef struct _IPPacket
{
	UCHAR iphVerLen; //�汾�ź�ͷ����(��ռ4λ)
	UCHAR ipTOS; //��������
	USHORT ipLength; //����ܳ��ȣ�������IP���ĳ���
	USHORT ipID; //�����ʶ��Ωһ��ʶ���͵�ÿһ�����ݱ�
	USHORT ipFlags; //��־
	UCHAR ipTTL; //����ʱ�䣬����TTL
	UCHAR ipProtocol; //Э�飬������TCP��UDP��ICMP��
	USHORT ipChecksum; //У���
	UCHAR ipSource[4]; //ԴIP��ַ
	UCHAR ipDestination[4]; //Ŀ��IP��ַ
}IPPacket, *pIPPacket;
#pragma pack(push,1)
typedef struct _TCPPacket //20���ֽ�
{
	USHORT sourcePort; //16λԴ�˿ں�
	USHORT destinationPort;//16λĿ�Ķ˿ں�
	ULONG sequenceNumber; //32λ���к�
	ULONG acknowledgeNumber;//32λȷ�Ϻ�
	USHORT flagsOffset;//ƫ�ƺͱ�־
	USHORT windows; //16λ���ڴ�С
	USHORT checksum; //16λУ���
	USHORT urgentPointer; //16λ��������ƫ����
}TCPPacket, *PTCPPacket;
#pragma pack(pop)
typedef struct _UDPPacket
{
	USHORT sourcePort; //Դ�˿ں�
	USHORT destinationPort;//Ŀ�Ķ˿ں�
	USHORT len; //�������
	USHORT checksum; //У���
}UDPPacket, *PUDPPacket;
typedef struct _ICMPPacket
{
	UCHAR icmp_type; //��Ϣ����
	UCHAR icmp_code; //����
	USHORT icmp_checksum; //У���
						  //�����ǻ���ͷ
	USHORT icmp_id; //����Ωһ��ʶ�������ID�ţ�ͨ������Ϊ����ID
	USHORT icmp_sequence; //���к�
	ULONG icmp_timestamp; //ʱ���
}ICMPPacket, *PICMPPacket;
typedef struct _IGMPPacket //8�ֽ�
{
	UCHAR hVerType; //�汾�ź�����(��4λ)
	UCHAR uReserved; //δ��
	USHORT uCheckSum; //У���
	ULONG dwGroupAddress;//32Ϊ���ַ(D��IP��ַ)
}IGMPPacket, *PIGMPPacket;
typedef struct _ARPPacket //28�ֽڵ�ARPͷ
{
	USHORT hrd; //Ӳ����ַ�ռ䣬��̫����ΪARPHRD_EHER
	USHORT eth_type; //��̫�����ͣ�ETHERTYPE_IP
	UCHAR maclen; //MAC��ַ�ĳ��ȣ�Ϊ6
	UCHAR iplen; //IP��ַ�ĳ��ȣ�Ϊ4
	USHORT opcode; //��������,ARPOP_REWUESTΪ����ARPOP_REPLYΪ��Ӧ
	UCHAR smac[6]; //ԴMAC��ַ
	UCHAR saddr[4]; //ԴIP��ַ
	UCHAR dmac[6]; //Ŀ��MAC��ַ
	UCHAR daddr[4]; //Ŀ��IP��ַ
}ARPPacket, *PARPPacket;
typedef struct _DNSPacket 
{
	USHORT id; //��ʶ��ͨ�����ͻ��˿��Խ�DNS��������Ӧ����ƥ�䣻
	USHORT flags; //��־��(��ѯ)0x0100 (Ӧ��)0x8180 ��Щ���ֶ���������
	USHORT questions; //������Ŀ
	USHORT answers; //��Դ��¼��Ŀ
	USHORT author; //��Ȩ��Դ��¼��Ŀ
	USHORT addition; //������Դ��¼��Ŀ
}DNSPacket,*PDNSPacket;
typedef struct _QICQPacket
{
	UCHAR flag;       //0x02
	USHORT version;  //�汾
	USHORT command;  //0x2������ seqence����һ  0x27:GetFriendOnline 0x1D:RequestKey 0x17:����Ϣ   0x13:GetStatus 0x58:DwonloadFriend 0x81:GetStatusOfFriend 0x5c:GetLevel
	USHORT seqence;  //���
	int qqNumber;    //qq��
	UCHAR data;      //���ܵ�data����С������
}QICQPacket,*PQICQPacket;
typedef struct _DHCPPacket
{
	UCHAR messageType;                  //1 Request 2 Reply
	UCHAR harewareType;                 //1ΪEthernet
	UCHAR hardwareAddressLength;        //EthernetʱΪmac��ַ���ȣ�6��
	UCHAR hops;                         //������·������û��Ϊ0��
	int translateId;                    //���ID
	USHORT seconds;
	struct 
	{
		USHORT boardcastFlag : 1;       //�Ƿ��ù㲥��ʽ����
		USHORT Reserved : 15;
	}flags;
	UCHAR clientIpAddress[4];           //�Լ��ĵ�ַ
	UCHAR serverIpAddress[4];           //��������ַ
	UCHAR networkGatewayIpAddress[4];   //���ص�ַ
	UCHAR clientHardwareAddress[16];    //Ӳ����ַ
	UCHAR serverName[64];
	UCHAR fileName[128];
	UCHAR options;                      //����Ŀ�ѡ�ֶΣ���С������
}DHCPPacket,*PDHCPPacket;