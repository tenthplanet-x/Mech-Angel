#pragma once
#include "MC_IMessageSender.h"
#include "LC_ServerPlayer.h"
#include "MG_SyncProtocol.h"

class PlayerEnterOrLeaveUniCastMessageSender : public MC_IUniCastMessageSender
{
public:
	PlayerEnterOrLeaveUniCastMessageSender();
	virtual bool IsDataDirty(LC_ServerPlayer* player);
	virtual void ClearData(LC_ServerPlayer* player);
	virtual void SendMessage(LC_ServerPlayer* player,MC_PlayerFovVector& playerFovVector,size_t sectorIndex);
private:
	Protocol::MG_SyncPlayerLeave				m_leaveMsg;
	Protocol::MG_SyncPlayerEnter				m_playerEnterMsg;
	Protocol::MG_SyncPlayersPropertyInFOV		m_playerEnterPropertyMsg;
	Protocol::MG_SyncPlayersFastPropertyInFOV	m_playerFastPropertyMsg;
	Protocol::MG_SyncPlayersExtPropertyInFOV	m_playerExtPropertyMsg;
	Protocol::MG_SyncPlayersSocietyInFOV		m_playerEnterSocietyMsg;
	Protocol::MG_SyncPlayersEquipInFOV			m_playerEquipMsg;
	Protocol::MG_SyncPlayersSkillStateInFOV		m_playerSkillStateMsg;	
	Protocol::MG_SyncSubordinateInfoInFOV		m_playerSubordinateMsg;
	Protocol::MG_SyncPlayerControllersPropertyInFOV		m_playerControllerPropertyMsg;
};

class PlayerSyncAllPlayerMessageSender : public MC_IUniCastMessageSender
{
public:
	PlayerSyncAllPlayerMessageSender();
	virtual bool IsDataDirty(LC_ServerPlayer* player);
	virtual void ClearData(LC_ServerPlayer* player);
	virtual void SendMessage(LC_ServerPlayer* player,MC_PlayerFovVector& playerFovVector,size_t sectorIndex);
private:
	Protocol::MG_SyncPlayerEnter				m_playersInfoInFOVMsg;
	Protocol::MG_SyncPlayersPropertyInFOV		m_playersPropertyInFOVMsg;
	Protocol::MG_SyncPlayersFastPropertyInFOV	m_playerFastPropertyMsg;
	Protocol::MG_SyncPlayersExtPropertyInFOV	m_playerExtPropertyMsg;
	Protocol::MG_SyncPlayersSocietyInFOV		m_playersSocietyInFOVMsg;
	Protocol::MG_SyncPlayersEquipInFOV			m_playersEquipInFOVMsg;
	Protocol::MG_SyncPlayersSkillStateInFOV		m_playerSkillStateMsg;
	Protocol::MG_SyncSubordinateInfoInFOV		m_playerSubordinateMsg;
	Protocol::MG_SyncPlayerControllersPropertyInFOV		m_playerControllerPropertyMsg;

	Protocol::MG_SyncNPCsPropertyInFOV				m_npcPropertyInFovMsg;
	Protocol::MG_SyncNPCsMoveInFOV					m_npcLocationInFovMsg;
	Protocol::MG_SyncNPCsSkillStateInFOV			m_npcSkillStateMsg;
	Protocol::MG_SyncNPCsInteractionValueInFOV		m_npcInteractionMsg;
};

class NpcEnterOrLeaveUniCastMessageSender : public MC_IUniCastMessageSender
{
public:
	NpcEnterOrLeaveUniCastMessageSender();
	virtual bool IsDataDirty(LC_ServerPlayer* player);
	virtual void ClearData(LC_ServerPlayer* player);
	virtual void SendMessage(LC_ServerPlayer* player,MC_PlayerFovVector& playerFovVector,size_t sectorIndex);
private:
	Protocol::MG_SyncNpcLeave							m_leaveMsg;
	Protocol::MG_SyncNpcEnter							m_npcEnterMsg;
	Protocol::MG_SyncNPCsPropertyInFOV					m_npcPropertyInFovMsg;
	Protocol::MG_SyncNPCsSkillStateInFOV				m_npcSkillStateMsg;
	Protocol::MG_SyncNPCsInteractionValueInFOV			m_npcInteractionMsg;

};

class PlayerSyncAllNPCMessageSender : public MC_IUniCastMessageSender
{
public:
	PlayerSyncAllNPCMessageSender();
	virtual bool IsDataDirty(LC_ServerPlayer* player);
	virtual void ClearData(LC_ServerPlayer* player);
	virtual void SendMessage(LC_ServerPlayer* player,MC_PlayerFovVector& playerFovVector,size_t sectorIndex);
private:
	Protocol::MG_SyncNPCsPropertyInFOV		m_npcPropertyInFovMsg;
	Protocol::MG_SyncNPCsMoveInFOV			m_npcLocationInFovMsg;
};
