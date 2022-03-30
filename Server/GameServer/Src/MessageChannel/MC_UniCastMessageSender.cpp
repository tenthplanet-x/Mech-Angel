
#include "MC_UniCastMessageSender.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPC.h"
#include "MC_MessagePackageHelp.h"
#include "MC_PlayerFovVector.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerNPCManager.h"

#include <algorithm>

using namespace Protocol;
using namespace GameLogic;

PlayerEnterOrLeaveUniCastMessageSender::PlayerEnterOrLeaveUniCastMessageSender()
{
}
bool PlayerEnterOrLeaveUniCastMessageSender::IsDataDirty( LC_ServerPlayer* player )
{
	return player->GetMessageDirtyFlag().GetUniCastFlag().IsPlayerEnterOrLeave();
}

void PlayerEnterOrLeaveUniCastMessageSender::ClearData( LC_ServerPlayer* player )
{
	player->GetMessageDirtyFlag().GetUniCastFlag().ClearPlayerEnterOrLeave();
}

void PlayerEnterOrLeaveUniCastMessageSender::SendMessage( LC_ServerPlayer* player ,MC_PlayerFovVector& playerFovVector,size_t sectorIndex)
{
	MC_MessagePackageHelp::Begin(m_leaveMsg);
	bool isLeaveMsgSend = false;

	object_id_type iSelfID = player->GetID();

	object_id_vector leavePlayers;
	player->GetFOVMap()->GetLeavePlayers(leavePlayers);
	for (size_t i = 0; i< leavePlayers.size();++i)
	{
		if (iSelfID == leavePlayers[i])
		{
			continue;
		}

		MC_MessagePackageHelp::Packet(m_leaveMsg, leavePlayers[i]);
		isLeaveMsgSend = true;
		if (m_leaveMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
		{
			break;
		}
	}

	object_id_vector enterPlayers;
	player->GetFOVMap()->GetEntetPlayers(enterPlayers);

	MC_MessagePackageHelp::Begin(m_playerEnterMsg);
	MC_MessagePackageHelp::Begin(m_playerEnterPropertyMsg);
	MC_MessagePackageHelp::Begin(m_playerFastPropertyMsg);
	MC_MessagePackageHelp::Begin(m_playerExtPropertyMsg);
	MC_MessagePackageHelp::Begin(m_playerEnterSocietyMsg);
	MC_MessagePackageHelp::Begin(m_playerSkillStateMsg);	
	MC_MessagePackageHelp::Begin(m_playerEquipMsg);
	MC_MessagePackageHelp::Begin(m_playerSubordinateMsg);
	MC_MessagePackageHelp::Begin(m_playerControllerPropertyMsg);

	bool isEnterMsgSend = false;
	LC_ServerPlayerManager* playerManager = SERVER_GET_PLAYER_MANAGER();
	for (size_t i = 0; i< enterPlayers.size();++i)
	{
		if (iSelfID == enterPlayers[i])
		{
			continue;
		}

		LC_ServerPlayer* FOVPlayer = (LC_ServerPlayer*)playerManager->FindPlayer(enterPlayers[i]);
		if (FOVPlayer)
		{
			MC_MessagePackageHelp::Packet(m_playerEnterMsg, FOVPlayer);
			MC_MessagePackageHelp::Packet(m_playerEnterPropertyMsg, FOVPlayer);
			MC_MessagePackageHelp::Packet(m_playerFastPropertyMsg, FOVPlayer);
			MC_MessagePackageHelp::Packet(m_playerExtPropertyMsg, FOVPlayer);
			MC_MessagePackageHelp::Packet(m_playerEnterSocietyMsg, FOVPlayer);
			MC_MessagePackageHelp::Packet(m_playerEquipMsg, FOVPlayer);
			MC_MessagePackageHelp::Packet(m_playerSkillStateMsg, FOVPlayer, true);
			MC_MessagePackageHelp::Packet(m_playerSubordinateMsg, FOVPlayer);
			MC_MessagePackageHelp::Packet(m_playerControllerPropertyMsg, FOVPlayer);

			isEnterMsgSend = true;
		}
		if (MC_MessagePackageHelp::EntryCount(m_playerEnterMsg) >= MAX_PLAYER_COUNT_IN_FOV)
		{
			break;
		}
	}

	LC_ServerPlayer * pkOwnerplayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerplayer)
		return;

	if (isEnterMsgSend)
	{
		pkOwnerplayer->SendMsgToClient(m_playerEnterMsg.GetMessageID(), &m_playerEnterMsg);
		pkOwnerplayer->SendMsgToClient(m_playerEnterPropertyMsg.GetMessageID(), &m_playerEnterPropertyMsg);
		pkOwnerplayer->SendMsgToClient(m_playerFastPropertyMsg.GetMessageID(), &m_playerFastPropertyMsg);
		pkOwnerplayer->SendMsgToClient(m_playerExtPropertyMsg.GetMessageID(), &m_playerExtPropertyMsg);
		pkOwnerplayer->SendMsgToClient(m_playerEnterSocietyMsg.GetMessageID(), &m_playerEnterSocietyMsg);
		pkOwnerplayer->SendMsgToClient(m_playerEquipMsg.GetMessageID(), &m_playerEquipMsg);
		pkOwnerplayer->SendMsgToClient(m_playerSkillStateMsg.GetMessageID(), &m_playerSkillStateMsg);
		pkOwnerplayer->SendMsgToClient(m_playerSubordinateMsg.GetMessageID(), &m_playerSubordinateMsg);
		pkOwnerplayer->SendMsgToClient(m_playerControllerPropertyMsg.GetMessageID(), &m_playerControllerPropertyMsg);
	}
	if (isLeaveMsgSend)
	{
		pkOwnerplayer->SendMsgToClient(m_leaveMsg.GetMessageID(),&m_leaveMsg);
	}
}

PlayerSyncAllPlayerMessageSender::PlayerSyncAllPlayerMessageSender()
{
}
bool PlayerSyncAllPlayerMessageSender::IsDataDirty( LC_ServerPlayer* player )
{
	return player->GetMessageDirtyFlag().GetUniCastFlag().IsSysnAll();
}

void PlayerSyncAllPlayerMessageSender::ClearData( LC_ServerPlayer* player )
{
	player->GetMessageDirtyFlag().GetUniCastFlag().ClearSysnAll();
}

void PlayerSyncAllPlayerMessageSender::SendMessage( LC_ServerPlayer* player ,MC_PlayerFovVector& playerFovVector,size_t sectorIndex)
{
	MC_MessagePackageHelp::Begin(m_playersInfoInFOVMsg);
	MC_MessagePackageHelp::Begin(m_playersPropertyInFOVMsg);
	MC_MessagePackageHelp::Begin(m_playerFastPropertyMsg);
	MC_MessagePackageHelp::Begin(m_playerExtPropertyMsg);
	MC_MessagePackageHelp::Begin(m_playersSocietyInFOVMsg);
	MC_MessagePackageHelp::Begin(m_playersEquipInFOVMsg);
	MC_MessagePackageHelp::Begin(m_playerSkillStateMsg);
	MC_MessagePackageHelp::Begin(m_playerSubordinateMsg);
	MC_MessagePackageHelp::Begin(m_playerControllerPropertyMsg);
	
	bool isEnterMsgSend = false;
	size_t playerInFovCount = playerFovVector.GetPlayerFovInSectorCount(sectorIndex);
	for (size_t playerIndex = 0; playerIndex<playerInFovCount; ++playerIndex)
	{
		LC_ServerPlayer* FOVPlayer = playerFovVector.GetPlayerFovInSector(sectorIndex,playerIndex);
		MC_MessagePackageHelp::Packet(m_playersInfoInFOVMsg,FOVPlayer);
		MC_MessagePackageHelp::Packet(m_playersPropertyInFOVMsg, FOVPlayer);
		MC_MessagePackageHelp::Packet(m_playerFastPropertyMsg, FOVPlayer);
		MC_MessagePackageHelp::Packet(m_playerExtPropertyMsg, FOVPlayer);
		MC_MessagePackageHelp::Packet(m_playersSocietyInFOVMsg, FOVPlayer);
		MC_MessagePackageHelp::Packet(m_playersEquipInFOVMsg,FOVPlayer);
		MC_MessagePackageHelp::Packet(m_playerSkillStateMsg,FOVPlayer, true);
		MC_MessagePackageHelp::Packet(m_playerSubordinateMsg,FOVPlayer);
		MC_MessagePackageHelp::Packet(m_playerControllerPropertyMsg,FOVPlayer);
		isEnterMsgSend = true;
		if (MC_MessagePackageHelp::EntryCount(m_playersInfoInFOVMsg) >= MAX_PLAYER_COUNT_IN_FOV)
		{
			break;
		}
	}

	LC_ServerPlayer * pkOwnerplayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerplayer)
		return;

	if (isEnterMsgSend)
	{
		pkOwnerplayer->SendMsgToClient(m_playersInfoInFOVMsg.GetMessageID(),&m_playersInfoInFOVMsg);
		pkOwnerplayer->SendMsgToClient(m_playersPropertyInFOVMsg.GetMessageID(), &m_playersPropertyInFOVMsg);
		pkOwnerplayer->SendMsgToClient(m_playerFastPropertyMsg.GetMessageID(), &m_playerFastPropertyMsg);
		pkOwnerplayer->SendMsgToClient(m_playerExtPropertyMsg.GetMessageID(), &m_playerExtPropertyMsg);
		pkOwnerplayer->SendMsgToClient(m_playersSocietyInFOVMsg.GetMessageID(), &m_playersSocietyInFOVMsg);
		pkOwnerplayer->SendMsgToClient(m_playersEquipInFOVMsg.GetMessageID(),&m_playersEquipInFOVMsg);
		pkOwnerplayer->SendMsgToClient(m_playerSkillStateMsg.GetMessageID(),&m_playerSkillStateMsg);
		pkOwnerplayer->SendMsgToClient(m_playerSubordinateMsg.GetMessageID(),&m_playerSubordinateMsg);
		pkOwnerplayer->SendMsgToClient(m_playerControllerPropertyMsg.GetMessageID(),&m_playerControllerPropertyMsg);
	}

/*
	MC_MessagePackageHelp::Begin(m_npcPropertyInFovMsg);
	MC_MessagePackageHelp::Begin(m_npcLocationInFovMsg);
	MC_MessagePackageHelp::Begin(m_npcSkillStateMsg);
	MC_MessagePackageHelp::Begin(m_npcInteractionMsg);

	bool haveNpcmessage = false;

	size_t npcInFovCount = playerFovVector.GetNPCFovInSectorCount(sectorIndex);
	for (size_t npcIndex = 0 ; npcIndex < npcInFovCount; ++npcIndex)
	{
		LC_ServerNPC* npc = playerFovVector.GetNPCFovInSector(sectorIndex,npcIndex);
		MC_MessagePackageHelp::Packet(m_npcPropertyInFovMsg,npc);
		MC_MessagePackageHelp::Packet(m_npcLocationInFovMsg,npc);
		MC_MessagePackageHelp::Packet(m_npcSkillStateMsg,npc);
		MC_MessagePackageHelp::Packet(m_npcInteractionMsg,npc);
		haveNpcmessage = true;
		if (m_npcPropertyInFovMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
		{
			break;
		}
	}

	if (haveNpcmessage)
	{
 		player->SendMsgToClient(m_npcPropertyInFovMsg.GetMessageID(),&m_npcPropertyInFovMsg);
 		player->SendMsgToClient(m_npcLocationInFovMsg.GetMessageID(),&m_npcLocationInFovMsg);
 		player->SendMsgToClient(m_npcSkillStateMsg.GetMessageID(),&m_npcSkillStateMsg);
 		player->SendMsgToClient(m_npcInteractionMsg.GetMessageID(),&m_npcInteractionMsg);
	}
*/
}

NpcEnterOrLeaveUniCastMessageSender::NpcEnterOrLeaveUniCastMessageSender()
{
}

bool NpcEnterOrLeaveUniCastMessageSender::IsDataDirty( LC_ServerPlayer* player )
{
	return player->GetMessageDirtyFlag().GetUniCastFlag().IsNpcEnterOrLeave();
}

void NpcEnterOrLeaveUniCastMessageSender::ClearData( LC_ServerPlayer* player )
{
	player->GetMessageDirtyFlag().GetUniCastFlag().ClearNpcEnterOrLeave();
}

void NpcEnterOrLeaveUniCastMessageSender::SendMessage( LC_ServerPlayer* player ,MC_PlayerFovVector& playerFovVector,size_t sectorIndex)
{
	LC_ServerPlayer * pkOwnerplayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerplayer)
		return;

	m_leaveMsg.m_lNpcNum = 0;
	bool isLeaveMsgSend = false;

	size_t  nSendSize = 0;
	size_t  nTotalSize = 0;
	int32_t nLoopCount = 0;

	mem::vector<object_id_type> leaveNPCs;
	leaveNPCs.reserve(MAX_NPC_COUNT_IN_FOV);
	player->GetFOVMap()->GetLeaveNPCs(leaveNPCs);
	nTotalSize = leaveNPCs.size();
	for (size_t i = 0; i< nTotalSize;++i)
	{
		MC_MessagePackageHelp::Packet(m_leaveMsg,leaveNPCs[nSendSize]);
		isLeaveMsgSend = true;
		nSendSize++;
		nLoopCount++;
		//if (m_leaveMsg.m_lNpcNum >= MAX_NPC_COUNT_IN_FOV || m_leaveMsg.m_lNpcNum >= nTotalSize)
		if (nLoopCount >= MAX_NPC_COUNT_IN_FOV || nSendSize >= nTotalSize)
		{
			//break;
			//do send
			pkOwnerplayer->SendMsgToClient(m_leaveMsg.GetMessageID(),&m_leaveMsg);
			m_leaveMsg.m_lNpcNum = 0;
			nLoopCount = 0;
		}
	}

	mem::vector<object_id_type> enterNPCs;
	enterNPCs.reserve(MAX_NPC_COUNT_IN_FOV);
	player->GetFOVMap()->GetEntetNPCs(enterNPCs);

	MC_MessagePackageHelp::Begin(m_npcEnterMsg);
	MC_MessagePackageHelp::Begin(m_npcPropertyInFovMsg);
	MC_MessagePackageHelp::Begin(m_npcSkillStateMsg);
	MC_MessagePackageHelp::Begin(m_npcInteractionMsg);
	bool isEnterMsgSend = false;
	LC_ServerNPCManager* npcManager = SERVER_GET_NPC_MANAGER();
	nSendSize = 0;
	nTotalSize = enterNPCs.size();
	nLoopCount = 0;
	for (size_t i = 0; i< nTotalSize;++i)
	{
		LC_ServerNPC* FOVNpc = (LC_ServerNPC*)npcManager->FindNPC(enterNPCs[nSendSize]);
		if (FOVNpc)
		{
			MC_MessagePackageHelp::Packet(m_npcEnterMsg,FOVNpc);
			MC_MessagePackageHelp::Packet(m_npcPropertyInFovMsg,FOVNpc);
			MC_MessagePackageHelp::Packet(m_npcSkillStateMsg,FOVNpc, true);
			MC_MessagePackageHelp::Packet(m_npcInteractionMsg,FOVNpc);
			isEnterMsgSend = true;
			nLoopCount++;
			nSendSize++;
		}
		//if (m_npcEnterMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
		if (nLoopCount >= MAX_NPC_COUNT_IN_FOV || nSendSize >= nTotalSize)
		{
			//break;
			//do send
			pkOwnerplayer->SendMsgToClient(m_npcEnterMsg.GetMessageID(),&m_npcEnterMsg);
			pkOwnerplayer->SendMsgToClient(m_npcPropertyInFovMsg.GetMessageID(),&m_npcPropertyInFovMsg);
			pkOwnerplayer->SendMsgToClient(m_npcSkillStateMsg.GetMessageID(),&m_npcSkillStateMsg);
			pkOwnerplayer->SendMsgToClient(m_npcInteractionMsg.GetMessageID(),&m_npcInteractionMsg);
			MC_MessagePackageHelp::Begin(m_npcEnterMsg);
			MC_MessagePackageHelp::Begin(m_npcPropertyInFovMsg);
			MC_MessagePackageHelp::Begin(m_npcSkillStateMsg);
			MC_MessagePackageHelp::Begin(m_npcInteractionMsg);
			nLoopCount = 0;
		}
	}

	return;

	if (isEnterMsgSend)
	{
		pkOwnerplayer->SendMsgToClient(m_npcEnterMsg.GetMessageID(),&m_npcEnterMsg);
		pkOwnerplayer->SendMsgToClient(m_npcPropertyInFovMsg.GetMessageID(),&m_npcPropertyInFovMsg);
		pkOwnerplayer->SendMsgToClient(m_npcSkillStateMsg.GetMessageID(),&m_npcSkillStateMsg);
		pkOwnerplayer->SendMsgToClient(m_npcInteractionMsg.GetMessageID(),&m_npcInteractionMsg);
	}
	if (isLeaveMsgSend)
	{
		pkOwnerplayer->SendMsgToClient(m_leaveMsg.GetMessageID(),&m_leaveMsg);
	}
}

PlayerSyncAllNPCMessageSender::PlayerSyncAllNPCMessageSender()
{
}

bool PlayerSyncAllNPCMessageSender::IsDataDirty( LC_ServerPlayer* player )
{
	return true;
}

void PlayerSyncAllNPCMessageSender::ClearData( LC_ServerPlayer* player )
{

}

void PlayerSyncAllNPCMessageSender::SendMessage( LC_ServerPlayer* player ,MC_PlayerFovVector& playerFovVector,size_t sectorIndex)
{
	MC_MessagePackageHelp::Begin(m_npcPropertyInFovMsg);
	MC_MessagePackageHelp::Begin(m_npcLocationInFovMsg);

	bool haveNpcmessage = false;

	size_t npcInFovCount = playerFovVector.GetNPCFovInSectorCount(sectorIndex);
	for (size_t npcIndex = 0 ; npcIndex < npcInFovCount; ++npcIndex)
	{
		LC_ServerNPC* npc = playerFovVector.GetNPCFovInSector(sectorIndex,npcIndex);
		MC_MessagePackageHelp::Packet(m_npcPropertyInFovMsg,npc);
		MC_MessagePackageHelp::Packet(m_npcLocationInFovMsg,npc);
		haveNpcmessage = true;
		if (MC_MessagePackageHelp::EntryCount(m_npcPropertyInFovMsg) >= MAX_NPC_COUNT_IN_FOV)
		{
			break;
		}
	}

	LC_ServerPlayer * pkOwnerplayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerplayer)
		return;

	if (haveNpcmessage)
	{
		pkOwnerplayer->SendMsgToClient(m_npcPropertyInFovMsg.GetMessageID(),&m_npcPropertyInFovMsg);
		pkOwnerplayer->SendMsgToClient(m_npcLocationInFovMsg.GetMessageID(),&m_npcLocationInFovMsg);
	}
}