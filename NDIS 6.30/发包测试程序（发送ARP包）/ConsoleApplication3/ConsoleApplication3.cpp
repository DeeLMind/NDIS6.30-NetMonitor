// ConsoleApplication3.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<Windows.h>
#include"..\..\NdisCoreApi\NdisCoreApi.h"
#pragma comment(lib,"..\\..\\lib\\NdisCoreApi.lib")
//ע��ARP_REPLY������Consoletest��Wireshark�нػ�
int main()
{
	HANDLE hF = Net_OpenFilter();
	RawPacket Packet = { 0 };
	PIO_Packet Output = (PIO_Packet)malloc(sizeof(IO_Packet));
	Net_ShowAdapter(hF, Output);
	RtlCopyMemory(Packet.Osi.Mac.sou, Output->Packet.ShowAdapter.AdapterInfo[0].MacAddress, sizeof(Packet.Osi.Mac.sou));
	UCHAR dstmac[6] = { 0xff,0xff,0xff,0xff,0xff,0xff };
	RtlCopyMemory(Packet.Osi.Mac.dst, dstmac, sizeof(dstmac));
	Packet.Osi.Mac.type = Tranverse16(PACKET_ARP);
	Packet.Osi.protocol.Arp.eth_type = Tranverse16(PACKET_IP);
	Packet.Osi.protocol.Arp.hrd = Tranverse16(1);
	Packet.Osi.protocol.Arp.maclen = 6;
	Packet.Osi.protocol.Arp.iplen = 4;
	Packet.Osi.protocol.Arp.opcode = Tranverse16(ARP_REQUEST);
	UCHAR saddr[4] = { 192,168,1,105 };      //�ƶ�ԴIP
	UCHAR daddr[4] = { 192,168,1,1 };        //��Ҫ��ȡMAC��ַ��IP
	RtlCopyMemory(Packet.Osi.protocol.Arp.daddr, daddr, sizeof(daddr));
	RtlCopyMemory(Packet.Osi.protocol.Arp.saddr, saddr, sizeof(saddr));
	RtlCopyMemory(Packet.Osi.protocol.Arp.smac, Output->Packet.ShowAdapter.AdapterInfo[0].MacAddress, sizeof(Packet.Osi.Mac.sou));
	for (int i = 0; i < 60; i++) //��ӡ���ݰ�
	{
		printf("%02x\t",Packet.RawPacket[i]);
		if (i!=0&&i % 15==0)
		{
			printf("\n");
		}
	}
	while (true)
	{
		Net_SendRawPacket(hF, &Packet,60, 0);
		Sleep(200);
	}
    return 0;
}

