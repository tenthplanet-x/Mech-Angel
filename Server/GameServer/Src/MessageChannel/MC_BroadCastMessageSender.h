#pragma once

#include "MC_IMessageSender.h"
#include "MG_SyncProtocol.h"
#include "MM_MemoryAllocatorConfig.h"
#include "LC_PlayerBase.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPC.h"
#include "MC_MessagePackageHelp.h"
#include "MC_BroadCastDataDirtyFlag.h"
#include "MC_BaseDataDirtyFlag.h"

template<typename Message, int nDirtyFlag>
class MC_GridPacketDataMessageSender : public MC_IGridPacketPlayerDataMessageSender
{
public:
	MC_GridPacketDataMessageSender(): m_nDirtyFlag(nDirtyFlag)
	{
		for(int i=0;i<MAX_USER_IN_GAME_SERVER;i++)
		{
			m_packetMessageInSector[i].clear();
		}
			m_packetMessage.clear();
	}

	virtual void ClearPacketSectorMessage(size_t sectorIndex)
	{
		if(sectorIndex < 0 || sectorIndex >= MAX_USER_IN_GAME_SERVER)
		{
			return;
		}
		MC_MessagePackageHelp::Begin(m_packetMessageInSector[sectorIndex]);
	}

	virtual void BeginBroadCastMessage()
	{
		MC_MessagePackageHelp::Begin(m_packetMessage);
	}

	virtual bool CanBroadCastMessage() { return MC_MessagePackageHelp::EntryCount(m_packetMessage) > 0; }
	virtual Message* GetMessageBody() { return &m_packetMessage; }

	int GetSectorCount(size_t sectorIndex) const
	{
		if(sectorIndex < 0 || sectorIndex >= MAX_USER_IN_GAME_SERVER)
		{
			return 0;
		}
		return MC_MessagePackageHelp::EntryCount(m_packetMessageInSector[sectorIndex]);
	}

	void PacketSectorPlayerMessage(size_t sectorIndex, LC_ServerPlayer* playerInFov)
	{
		if (sectorIndex < 0 || sectorIndex >= MAX_USER_IN_GAME_SERVER || playerInFov == NULL)
		{
			return;
		}
		MC_MessagePackageHelp::Packet(m_packetMessageInSector[sectorIndex], playerInFov);
	}
	
	bool MergeSectorBroadData(MC_PlayerFovVector& playerFovVector,size_t sectorIndex)
	{
		if (sectorIndex < 0 || sectorIndex >= MAX_USER_IN_GAME_SERVER)
		{
			return false;
		}

		Message& mergeMsg = m_packetMessageInSector[sectorIndex];

		MC_MessagePackageHelp::Copy(m_packetMessage, mergeMsg, MAX_PLAYER_COUNT_IN_FOV);
		return MC_MessagePackageHelp::EntryCount(m_packetMessage) < MAX_PLAYER_COUNT_IN_FOV;
	}

	virtual bool IsDataChange(LC_ServerPlayer* playerInFov)
	{
		if (playerInFov == NULL)
		{
			return false;
		}
		MC_BroadCastDataDirtyFlag& rDirtyFlag = playerInFov->GetMessageDirtyFlag().GetBroadCastFlag();
		return rDirtyFlag.IsBroadCastFlag(nDirtyFlag);
	}

	virtual void ClearDataChange(LC_ServerPlayer* playerInFov)
	{
		if (playerInFov == NULL)
		{
			return;
		}
		MC_BroadCastDataDirtyFlag& rDirtyFlag = playerInFov->GetMessageDirtyFlag().GetBroadCastFlag();
		rDirtyFlag.ClearBroadCastFlag(nDirtyFlag);
	}
protected:
	Message m_packetMessageInSector[MAX_USER_IN_GAME_SERVER];
	Message m_packetMessage;
	int		m_nDirtyFlag;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayerHPMPChange, LC_PlayerBase::E_DirtyFlag_HPMPChg> PlayerHPMPChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayerPVEStateChange, LC_PlayerBase::E_DirtyFlag_PVEStateType> PlayerPVEStateChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayerStateChange, LC_PlayerBase::E_DirtyFlag_StateType> PlayerStateChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayerLocation, LC_PlayerBase::E_DirtyFlag_Location> PlayerLocationChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayerAngle, LC_PlayerBase::E_DirtyFlag_Angle> PlayerAngleChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayersPropertyInFOV, LC_PlayerBase::E_DirtyFlag_Property> PlayerPropertyChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayerControllersPropertyInFOV, LC_PlayerBase::E_DirtyFlag_ControllerProperty> PlayerControllerPropertyChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayersExtPropertyInFOV, LC_PlayerBase::E_DirtyFlag_PropertyExt> PlayerExtPropertyChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayersSocietyInFOV, LC_PlayerBase::E_DirtyFlag_Socity> PlayerSocityChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayersEquipInFOV, LC_PlayerBase::E_DirtyFlag_Equip> PlayerEquipChangeBroadCastMessageSender;
typedef MC_GridPacketDataMessageSender<Protocol::MG_SyncSubordinateInfoInFOV, LC_PlayerBase::E_DirtyFlag_Subordinate> PlayerSubordinateChangeBroadCastMessageSender;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PlayerFastPropertyChangeBroadCastMessageSender: public MC_GridPacketDataMessageSender<Protocol::MG_SyncPlayersFastPropertyInFOV, LC_PlayerBase::E_DirtyFlag_PropertyFast>
{
	virtual bool IsDataChange(LC_ServerPlayer* playerInFov);
	virtual void ClearDataChange(LC_ServerPlayer* playerInFov);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Message, int nDirtyFlag>
class MC_AllPacketPlayerDataMessageSender :public MC_IAllPacketPlayerDataMessageSender
{
public:
	virtual void BeginBroadCastMessage() { MC_MessagePackageHelp::Begin(m_packetMessage); }
	virtual bool CanBroadCastMessage() { return MC_MessagePackageHelp::EntryCount(m_packetMessage) > 0; }
	virtual Message* GetMessageBody() { return &m_packetMessage; }
	virtual int GetCount(void) const { return MC_MessagePackageHelp::EntryCount(m_packetMessage);}

	void PacketPlayerMessage(LC_ServerPlayer* playerInFov)
	{
		if(playerInFov != NULL)
		{
			MC_MessagePackageHelp::Packet(m_packetMessage,playerInFov);
		}
	}

	virtual bool IsDataChange(LC_ServerPlayer* playerInFov)
	{
		if (playerInFov == NULL)
		{
			return false;
		}
		MC_BroadCastDataDirtyFlag& rDirtyFlag = playerInFov->GetMessageDirtyFlag().GetBroadCastFlag();
		return rDirtyFlag.IsBroadCastFlag(nDirtyFlag);
	}

	virtual void ClearDataChange(LC_ServerPlayer* playerInFov)
	{
		if (playerInFov == NULL)
		{
			return;
		}
		MC_BroadCastDataDirtyFlag& rDirtyFlag = playerInFov->GetMessageDirtyFlag().GetBroadCastFlag();
		rDirtyFlag.ClearBroadCastFlag(nDirtyFlag);
	}
protected:
	Message m_packetMessage;
};

typedef MC_AllPacketPlayerDataMessageSender<Protocol::MG_SyncPlayersSkillStateInFOV, LC_PlayerBase::E_DirtyFlag_SkillState> PlayerSkillStateChangeBroadCastMessageSender;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Message, int nDirtyFlag>
class MC_AllPacketNPCMessageSender :public MC_IAllPacketNPCDataMessageSender
{
public:
	virtual void BeginBroadCastMessage() { MC_MessagePackageHelp::Begin(m_packetMessage); }
	virtual bool CanBroadCastMessage() { return MC_MessagePackageHelp::EntryCount(m_packetMessage)>0; }
	virtual Message* GetMessageBody() { return &m_packetMessage; }

	void PacketNPCMessage(LC_ServerNPC* npcInFov)
	{
		if(npcInFov != NULL)
		{
			MC_MessagePackageHelp::Packet(m_packetMessage, npcInFov);
		}
	}

	virtual bool IsDataChange(LC_ServerNPC* npcInFov)
	{
		if (npcInFov == NULL)
		{
			return false;
		}
		MC_BroadCastDataDirtyFlag& rDirtyFlag = npcInFov->GetMessageDirtyFlag().GetBroadCastFlag();
		return rDirtyFlag.IsBroadCastFlag(nDirtyFlag);
	}

	virtual void ClearDataChange(LC_ServerNPC* npcInFov)
	{
		if (npcInFov == NULL)
		{
			return;
		}
		MC_BroadCastDataDirtyFlag& rDirtyFlag = npcInFov->GetMessageDirtyFlag().GetBroadCastFlag();
		rDirtyFlag.ClearBroadCastFlag(nDirtyFlag);
	}
protected:
	Message m_packetMessage;
};

typedef MC_AllPacketNPCMessageSender<Protocol::MG_SyncNPCsPropertyInFOV, LC_PlayerBase::E_DirtyFlag_HPMPChg> NPCPropertyChangeBroadCastMessageSender;
typedef MC_AllPacketNPCMessageSender<Protocol::MG_SyncNPCsMoveInFOV, LC_PlayerBase::E_DirtyFlag_Location> NPCLocationChangeBroadCastMessageSender;
typedef MC_AllPacketNPCMessageSender<Protocol::MG_SyncNPCsSkillStateInFOV, LC_PlayerBase::E_DirtyFlag_SkillState> NPCSkillStateChangeBroadCastMessageSender;
typedef MC_AllPacketNPCMessageSender<Protocol::MG_SyncNPCsInteractionValueInFOV, LC_PlayerBase::E_DirtyFlag_InterAction> NPCInteractionChangeBroadCastMessageSender;
typedef MC_AllPacketNPCMessageSender<Protocol::MG_SyncNPCsStateInFOV, LC_PlayerBase::E_DirtyFlag_StateType> NPCStateChangeBroadCastMessageSender;
typedef MC_AllPacketNPCMessageSender<Protocol::MG_SyncNPCsAngleInFOV, LC_PlayerBase::E_DirtyFlag_Angle> NPCAngleChangeBroadCastMessageSender;

