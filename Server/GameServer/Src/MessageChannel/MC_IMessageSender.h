#pragma once
#include "MM_MemoryAllocatorConfig.h"
#include "LC_ServerPlayer.h"

class LC_ServerPlayer;
class LC_ServerNPC;
class MC_PlayerFovVector;

class MC_IUniCastMessageSender
{
public:
	virtual bool IsDataDirty(LC_ServerPlayer* player)=0;
	virtual void ClearData(LC_ServerPlayer* player)=0;
	virtual void SendMessage(LC_ServerPlayer* player,MC_PlayerFovVector& playerFovVector,size_t sectorIndex)=0;
};

class MC_IGridPacketPlayerDataMessageSender
{
public:
	virtual void ClearPacketSectorMessage(size_t sectorIndex)=0;
	virtual bool MergeSectorBroadData(MC_PlayerFovVector& playerFovVector,size_t sectorIndex)=0;
	virtual void BeginBroadCastMessage()=0;
	virtual bool CanBroadCastMessage()=0;
	virtual Protocol::MG_BODY* GetMessageBody()=0;

	virtual int GetSectorCount(size_t sectorIndex) const = 0;

	virtual bool IsDataChange(LC_ServerPlayer* playerInFov)=0;
	virtual void PacketSectorPlayerMessage(size_t sectorIndex,LC_ServerPlayer* playerInFov)=0;
	virtual void ClearDataChange(LC_ServerPlayer* playerInFov)=0;
};

class MC_IAllPacketPlayerDataMessageSender
{
public:
	virtual void BeginBroadCastMessage()=0;
	virtual bool CanBroadCastMessage()=0;
	virtual Protocol::MG_BODY* GetMessageBody()=0;

	virtual bool IsDataChange(LC_ServerPlayer* playerInFov)=0;
	virtual void PacketPlayerMessage(LC_ServerPlayer* playerInFov)=0;
	virtual void ClearDataChange(LC_ServerPlayer* playerInFov)=0;
};

class MC_IAllPacketNPCDataMessageSender
{
public:
	virtual void BeginBroadCastMessage()=0;
	virtual bool CanBroadCastMessage()=0;
	virtual Protocol::MG_BODY* GetMessageBody()=0;

	virtual bool IsDataChange(LC_ServerNPC* npcInFov)=0;
	virtual void PacketNPCMessage(LC_ServerNPC* npcInFov)=0;
	virtual void ClearDataChange(LC_ServerNPC* npcInFov)=0;
};
