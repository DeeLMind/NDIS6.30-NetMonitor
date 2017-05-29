#pragma once
/********************************************************************************
*                                                                               *
* packetcpy.h --  RawPacket Manager                                             *
*                                                                               *
* Copyright Arch-Vile. All rights reserved.                     *
*                                                                               *
********************************************************************************/
#include "Struct.h"
int ZlzCalcBufSizeOrCopy(PS_PACKET Packet, PNET_BUFFER_LIST Nbl, int num, int MdlNum)   //�������θú�������һ�λ�ȡMDL��Ŀ�������ڴ棬�ڶ��ο�����ַ��
{
	PNET_BUFFER_LIST nbltemp = Nbl;
	PNET_BUFFER nb = NULL;
	int TempNum = 0;
	if (num)
	{
		Packet->mdllist = (PMDL*)ExAllocatePool(NonPagedPool, sizeof(PMDL)*MdlNum);
		if (Packet->mdllist == NULL)
		{
			KeBugCheckEx(IRQL_NOT_LESS_OR_EQUAL, 0, 0, 0, 0);
		}
	}
	nb = NET_BUFFER_LIST_FIRST_NB(nbltemp);
	do
	{
		if (nb != NULL)
		{
			for (PMDL mdl = NET_BUFFER_FIRST_MDL(nb); mdl != NULL; mdl = mdl->Next)
			{
				if (num == 0)
				{
					TempNum++;
				}
				else
				{
					Packet->mdllist[TempNum] = mdl;
					TempNum++;
				}
			}
		}
		nb = NET_BUFFER_NEXT_NB(nb);
	} while (nb != NULL);
	return TempNum;
}
VOID analysis(PS_PACKET Packet)
{
	PNET_BUFFER nb = NET_BUFFER_LIST_FIRST_NB(Packet->buffer);
	PVOID address = MmGetSystemAddressForMdlSafe(nb->MdlChain, IoPriorityNormal);
	MAC macpacket;
	RtlCopyMemory(&macpacket, address, sizeof(MAC));
	DbgPrint("start*********************************\n");
	DbgPrint("Packet num:%d\n", Packet->MdlNumber);
	DbgPrint("dest mac:%02x-%02x-%02x-%02x-%02x-%02x\n", macpacket.dst[0], macpacket.dst[1], macpacket.dst[2], macpacket.dst[3], macpacket.dst[4], macpacket.dst[5]);
	DbgPrint("source mac:%02x-%02x-%02x-%02x-%02x-%02x\n", macpacket.sou[0], macpacket.sou[1], macpacket.sou[2], macpacket.sou[3], macpacket.sou[4], macpacket.sou[5]);
	DbgPrint("type:%04x\n", Tranverse16(macpacket.type));
	DbgPrint("%s\n", Packet->IsSendPacket ? "Send" : "Receive");
	DbgPrint("end*********************************\n");
}
/*NTSTATUS ZlzCleanPool(PFILTER_CONTEXT Context)
{
	for (int num = 0; num < Context->CurrentRecvNum; num++)
	{
		if (Context->PacketRecvPool[num] != NULL)
		{
			if (Context->PacketRecvPool[num]->buffer != NULL)
			{
				NdisFreeCloneNetBufferList(Context->PacketRecvPool[num]->buffer, 0);
				if (Context->PacketRecvPool[num]->mdllist != NULL)
				{
					ExFreePool(Context->PacketRecvPool[num]->mdllist);
				}
				ExFreePool(Context->PacketRecvPool[num]);
			}
		}
	}
	NdisZeroMemory(Context->PacketRecvPool, sizeof(Context->PacketRecvPool)*Global.RecvPoolMax);
	Context->CurrentRecvNum = 0;
	return STATUS_SUCCESS;
}*/
NTSTATUS ZlzCleanList(PFILTER_CONTEXT Context)
{
	while (1)
	{
		if (Context->PacketRecvList.Blink != NULL)
		{
			PS_PACKET Packet = (PS_PACKET)Context->PacketRecvList.Blink;
			//RemoveTailList returns a pointer to the entry that was at the tail of the list. 
			//If the list is empty, RemoveTailList returns ListHead.
			if (RemoveTailList(&Context->PacketRecvList) == &Context->PacketRecvList)
			{
				break;
			}
			if (Packet->buffer != NULL)
			{
				NdisFreeCloneNetBufferList(Packet->buffer, 0);
				if (Packet->mdllist != NULL)
				{
					ExFreePool(Packet->mdllist);
				}
				ExFreePool(Packet);
			}
		}
	}
	Context->CurrentRecvNum = 0;
	return STATUS_SUCCESS;
}
NTSTATUS ZlzRemoveListHead(PFILTER_CONTEXT Context)
{
	PS_PACKET Packet = (PS_PACKET)Context->PacketRecvList.Flink;
	RemoveHeadList(&Context->PacketRecvList);
	if (Packet->buffer != NULL)
	{
		NdisFreeCloneNetBufferList(Packet->buffer, 0);
		if (Packet->mdllist != NULL)
		{
			ExFreePool(Packet->mdllist);
		}
		ExFreePool(Packet);
	}
	Context->CurrentRecvNum--;
	return STATUS_SUCCESS;
}
NTSTATUS ZlzInsertIntoList(PS_PACKET Packet, PFILTER_CONTEXT Context)
{
	KIRQL irql;
	KeAcquireSpinLock(&Context->NetBufferListLock, &irql);
	if (Context->CurrentRecvNum >= Global.RecvPoolMax)     //������
	{
		ZlzCleanList(Context);
	}
	InsertTailList(&Context->PacketRecvList, &Packet->PacketList);
	Context->CurrentRecvNum++;
	KeReleaseSpinLock(&Context->NetBufferListLock, irql);
#ifdef DBG
	analysis(Packet);//��ӡһЩ����
#endif // DEBUG
	return STATUS_SUCCESS;
}
NTSTATUS ZlzCopyNdlToBufferAndInsert(PFILTER_CONTEXT Context, PNET_BUFFER_LIST Nbl,BOOLEAN IsSendPacket) //��NDL�����ֱ����
{
	PNET_BUFFER_LIST nbltemp = Nbl;
	do
	{
		PNET_BUFFER_LIST CloneNbl = NdisAllocateCloneNetBufferList(nbltemp, Context->NetBufferPool, NULL, 0); //FLAG=0ʱ��ʼ������������MDL��ע��clone������������ϵ�����NBL����¡��ֻ���¡һ������clone����Next����ΪNULL
		if (CloneNbl == NULL)
		{
			return STATUS_UNSUCCESSFUL;
		}
		PS_PACKET Packet = (PS_PACKET)ExAllocatePool(NonPagedPool, sizeof(S_PACKET));
		int MdlNum = ZlzCalcBufSizeOrCopy(Packet, CloneNbl, 0, 0);
		ZlzCalcBufSizeOrCopy(Packet, CloneNbl, 1, MdlNum);
		Packet->MdlNumber = MdlNum;
		Packet->buffer = CloneNbl;
		Packet->size = 0;
		Packet->IsSendPacket = IsSendPacket;
		ZlzInsertIntoList(Packet, Context);
		nbltemp = NET_BUFFER_LIST_NEXT_NBL(nbltemp);
	} while (nbltemp != NULL);
	return STATUS_SUCCESS;
}
NTSTATUS ZlzGetNetworkAdapterInformation(PFILTER_CONTEXT Context)
{
	WCHAR* TempPath = NULL;
	NDIS_STRING DevPathName = Context->DevInfo.DevPathName;
	NDIS_STRING RegPath = RTL_CONSTANT_STRING(REG_NETWORKTCPIPCONFIG_PATH);
	TempPath = ExAllocatePoolWithTag(NonPagedPool, DevPathName.Length + RegPath.Length + sizeof(WCHAR), 'asd');
	if (TempPath == NULL)
	{
		return STATUS_UNSUCCESSFUL;
	}
	RtlStringCbCatW(TempPath, DevPathName.Length + RegPath.Length + sizeof(WCHAR), RegPath.Buffer);
	RtlStringCbCatW(TempPath, DevPathName.Length + RegPath.Length + sizeof(WCHAR), DevPathName.Buffer);
	DbgPrint("%wZ\n", TempPath);
	ExFreePoolWithTag(TempPath, 'asd');
	return STATUS_SUCCESS;
}
/*int ZlzCalcBufSizeOrCopy(PVOID DestBuf, PNET_BUFFER_LIST Nbl, int num)   //�������θú�������һ�λ�ȡ��С�������ڴ棬�ڶ��ο�����
{
	PNET_BUFFER_LIST nbltemp = Nbl;
	PNET_BUFFER nb = NULL;
	int size = 0;
	char *buf = (char*)DestBuf;
	do
	{
		nb = NET_BUFFER_LIST_FIRST_NB(nbltemp);
		do
		{
			if (nb != NULL)
			{
				for (PMDL mdl = NET_BUFFER_FIRST_MDL(nb); mdl != NULL; mdl = mdl->Next)
				{

					int mdlsize = MmGetMdlByteCount(mdl);
					size += mdlsize;
					if (num)
					{
						PVOID mdladdress = MmGetSystemAddressForMdlSafe(mdl, IoPriorityNormal);
						memcpy(buf, mdladdress, mdlsize);
						buf += mdlsize;
					}
				}
			}
			nb = NET_BUFFER_NEXT_NB(nb);
		} while (nb != NULL);
		nbltemp = NET_BUFFER_LIST_NEXT_NBL(nbltemp);
	} while (nbltemp != NULL);
	return size;
}*/