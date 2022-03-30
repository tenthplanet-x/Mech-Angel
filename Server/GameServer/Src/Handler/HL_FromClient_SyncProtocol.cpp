#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_SyncProtocol.h"
#include <algorithm>
#include "MC_MessagePackageHelp.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "SM_StateBase.h"
#include "LC_ServerNPCManager.h"

#include "oss_define.h"

using namespace Protocol;
using namespace GameLogic;
using namespace StateMachine;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_SYNC_GAME_PLAYER_MOVE, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	if (!pkPlayer->IsInGame())
	{
		return;
	}

	float fCurrentTime = GET_PROCESS_TIME;
	static float fLastTime = 0;
	float fDelta = fCurrentTime - fLastTime;

	fLastTime = fCurrentTime;	

	//pkPlayer = pkPlayer->GetCurrentActivateBuddy();

	MG_Sync_GamePlayerMove syncMsg;
	if (!syncMsg.Decode(pMsgBody, usBodySize))
		return;

	if ( pkPlayer->GetAIControl() )
		return;
	pkPlayer->BuddyControllerOperation_Move(syncMsg,fCurrentTime);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_PLAYER_JUMP, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	if (!pkPlayer->IsInGame())
	{
		return;
	}
	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;
	//解码
	MG_Req_PlayerJump jmpMsg;
	if (!jmpMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	float fCurTime = GET_PROCESS_TIME;
	//技能阶段
	if (IS_SKILL_TYPE(pkPlayer->GetCurrentStateType()))
	{
		//return;
		pkPlayer->TryInterruptSkill( -1, fCurTime, SKI_FORCE);
		pkPlayer->TryFinishSkill( -1, fCurTime, pkPlayer->GetCurrentLocation().x, pkPlayer->GetCurrentLocation().y );
		//pCurrentPlayer->SetLogicCombatFlag(false);

	}

	//检查标志位
	if (0 != (pkPlayer->GetAllSkillFlag() & SK_JUMP_NA_FLAGS))
	{
		return;
	}

	float godTime = 0;	//无敌时间
	int32_t iSkillTypeID = 0;
	//检查资源
	int costStamina = 0;
	switch (jmpMsg.m_usJumpStage)
	{
		case ST_PLAYER_JUMP1:
			{
				if ( !pkPlayer->GetControllerSkillAsset()->IsLearnedSkill(GetGlobalSetting.iSprintSkillID, true) )
				{
					return;
				}
				// mana checking
				if ( pkPlayer->GetOwnerMP() <= 0 )
				{
					MG_Rlt_PlayerExecuteSkillTypeDir rltMsg;
					rltMsg.m_lRequesterID			= pkPlayer->GetID();
					rltMsg.m_lSkillExecuteID		= GetGlobalSetting.iSprintSkillID;
					rltMsg.m_iResultType			= RE_SKILL_NOT_ENOUGH_AMM;
					rltMsg.m_fServerTime			= GET_PROCESS_TIME;

					pkPlayer->SendMsgToClient(MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_DIR, &rltMsg);
					return;
				}
				if ( pkPlayer->GetSkillCoolDownMap()->GetSkillCurrentCDTime(GetGlobalSetting.iSprintSkillID) > 0.1f)
				{
					MG_Rlt_PlayerExecuteSkillTypeDir rltMsg;
					rltMsg.m_lRequesterID			= pkPlayer->GetID();
					rltMsg.m_lSkillExecuteID		= GetGlobalSetting.iSprintSkillID;
					rltMsg.m_iResultType			= RE_SKILL_PENDING_FAIL_CD;
					rltMsg.m_fServerTime			= GET_PROCESS_TIME;

					pkPlayer->SendMsgToClient(MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_DIR, &rltMsg);
					return;
				}
				costStamina = PLAYER_JUMP1_COST_STAMINA;
				godTime = PLAYER_JUMP1_MIN_TIME;
				iSkillTypeID = GetGlobalSetting.iSprintSkillID;
			}
			break;
		case ST_PLAYER_JUMP2:
			{
				costStamina = PLAYER_JUMP2_COST_STAMINA;
				godTime = PLAYER_JUMP2_MAX_TIME;
				iSkillTypeID = SKILL_JUMP_D2;
			}
			break;
		case ST_PLAYER_JUMP3:
			{
				costStamina = PLAYER_JUMP3_COST_STAMINA;
				godTime = PLAYER_JUMP3_TIME;
				iSkillTypeID = SKILL_JUMP_D3;
			}
			break;
		case ST_PLAYER_JUMP_GUIDE:
			{
				iSkillTypeID = ST_PLAYER_JUMP_GUIDE;
				switch ( jmpMsg.m_usJumpType)
				{
				case ST_PLAYER_JUMP1:
					godTime = JUMP1_GUID_T;
					break;
				case ST_PLAYER_JUMP2:
					godTime = JUMP1_GUID_T + JUMP2_GUID_T + JUMP2_GUID_T + JUMP2_GUID_DROP;
					break;
				case ST_PLAYER_JUMP3:
					godTime = JUMP1_GUID_T + JUMP2_GUID_T + JUMP3_GUID_T;
					break;
				}
			}
			break;
		default:
			return;
	}
	int curStamina = pkPlayer->GetStamina();
	if (curStamina < costStamina)
	{
		return;
	}

	//if (!pkPlayer->GetSkillAsset()->IsLearnedSkill(iSkillTypeID, true) && iSkillTypeID != ST_PLAYER_JUMP_GUIDE)
	//{
	//	return;
	//}
	oss_jump(pkPlayer, iSkillTypeID);

	//扣除资源
	//pkPlayer->SetStamina(curStamina - costStamina);

	//同步参数
	UT_Vec3Int kCurrentLocation;
	UT_Vec3Int kTargetLocation;
	kCurrentLocation.x = jmpMsg.m_lCurrentLocationX;
	kCurrentLocation.y = jmpMsg.m_lCurrentLocationY;
	kCurrentLocation.z = jmpMsg.m_lCurrentLocationZ;
	kTargetLocation.x  = jmpMsg.m_lTargetLocationX;
	kTargetLocation.y  = jmpMsg.m_lTargetLocationY;
	kTargetLocation.z  = jmpMsg.m_lTargetLocationZ;

	pkPlayer->SetCurrentLocation(kCurrentLocation);
	pkPlayer->SetTargetLocation(kTargetLocation);
	pkPlayer->SetVerticalOffset((float)jmpMsg.m_iVerticalOffset);

	//无敌效果
	pkPlayer->SetFlyCountDownTime(godTime);
	pkPlayer->SetSkillFlag(SK_FLY, true);

	float fDebugTime = GET_PROCESS_TIME;

	//轻功无敌cd，结束时间设置
	pkPlayer->SetJumpImmortalEndTime(jmpMsg.m_usJumpStage, godTime);

	//广播消息
	MG_Rlt_PlayerJump	rltMsg;
	rltMsg.m_lRequesterID = jmpMsg.m_lRequesterID;
	rltMsg.m_usJumpStage  = jmpMsg.m_usJumpStage;
	rltMsg.m_usJumpType	= jmpMsg.m_usJumpType;
	rltMsg.m_lCurrentLocationX = jmpMsg.m_lCurrentLocationX;
	rltMsg.m_lCurrentLocationY = jmpMsg.m_lCurrentLocationY;
	rltMsg.m_lCurrentLocationZ = jmpMsg.m_lCurrentLocationZ;
	rltMsg.m_iVerticalOffset   = jmpMsg.m_iVerticalOffset;
	rltMsg.m_lTargetLocationX  = jmpMsg.m_lTargetLocationX;
	rltMsg.m_lTargetLocationY  = jmpMsg.m_lTargetLocationY;
	rltMsg.m_lTargetLocationZ  = jmpMsg.m_lTargetLocationZ;

	pkPlayer->BroadCastToArea(MGPT_RLT_PLAYER_JUMP, &rltMsg);


	if (jmpMsg.m_usJumpStage == ST_PLAYER_JUMP1) 
	{
		int nSlot = -1;
		pkPlayer->LearnSkill(iSkillTypeID, nSlot);
		pkPlayer->GetSkillCoolDownMap()->AddSkillCDSlot(iSkillTypeID);
		pkPlayer->GetSkillCoolDownMap()->RefreshSkillCD(iSkillTypeID);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_SKILL_CD);
		pkPlayer->ModifyOwnerMP( -GetGlobalSetting.nOwnerMPResumeValue );
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_SYNC_UI_OPERATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_GC_Sync_UIOperation syncMsg;
	if (!syncMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame())
	{
		return;
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_SYNC_CLIENT_SAMPLE_RECORD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_GC_Sync_ClientSampleRecord syncMsg;
	if (!syncMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_PLAYER_IN_FOV, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_GC_REQ_SYNC_PLAYER msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	return;

	if (msg.m_kActorIDs.size() > (size_t)MAX_PLAYER_COUNT_IN_FOV)
	{
		msg.m_kActorIDs.resize((size_t)MAX_PLAYER_COUNT_IN_FOV);
	}

	{
		const CSObjectIdSet& playerIDsInFov = pkPlayer->GetFOVMap()->GetPlayerIDsInFOV();
		/*
		object_id_vector playerInFovVector(playerIDsInFov.begin(), playerIDsInFov.end());
		/*/
		/// CSObjectIdSet 不支持上述写法
		object_id_vector playerInFovVector;
		playerInFovVector.reserve((playerIDsInFov.size()));
		for(CSObjectIdSet::const_iterator it = playerIDsInFov.begin(), itEnd = playerIDsInFov.end(); 
			it != itEnd; ++it)
			playerInFovVector.push_back(*it);
		//*/
		object_id_type playerIDs[MAX_PLAYER_COUNT_IN_FOV * 2] = {0};

		size_t playerInFovCount = playerInFovVector.size();
		if (playerInFovCount > MAX_PLAYER_COUNT_IN_FOV)
		{
			playerInFovCount = MAX_PLAYER_COUNT_IN_FOV;
		}
		object_id_type* endLeave = std::set_difference(msg.m_kActorIDs.begin(), msg.m_kActorIDs.end(),
								   playerInFovVector.begin(), playerInFovVector.begin()+playerInFovCount, playerIDs);
		MG_SyncPlayerLeave leaveMsg;
		bool isLeaveMsgSend = false;
		for (object_id_type* leaveID = playerIDs; leaveID != endLeave; ++leaveID)
		{
			if (pkPlayer->GetID() == *leaveID)
			{
				continue;
			}
			MC_MessagePackageHelp::Packet(leaveMsg, *leaveID);
			isLeaveMsgSend = true;
			if (MC_MessagePackageHelp::EntryCount(leaveMsg) >= MAX_PLAYER_COUNT_IN_FOV)
			{
				break;
			}
		}

		MG_SyncPlayerEnter playerEnterMsg;
		MG_SyncPlayersPropertyInFOV playerEnterPropertyMsg;
		MG_SyncPlayersFastPropertyInFOV playerFastPropertyMsg;
		MG_SyncPlayersExtPropertyInFOV playerExtPropertyMsg;
		MG_SyncPlayersSocietyInFOV playerEnterSocietyMsg;
		MG_SyncPlayersSkillStateInFOV playerSkillStateMsg;
		MG_SyncPlayersEquipInFOV playerEquipMsg;
		MG_SyncSubordinateInfoInFOV playerSubordinateMsg;
		MG_SyncPlayerControllersPropertyInFOV playerControllerPropertyMsg;

		bool isEnterMsgSend = false;
		LC_ServerPlayerManager* playerManager = SERVER_GET_PLAYER_MANAGER();
		object_id_type* endEnter = std::set_difference(playerInFovVector.begin(), playerInFovVector.begin() + playerInFovCount,
								   msg.m_kActorIDs.begin(), msg.m_kActorIDs.end(), playerIDs);

		for (object_id_type* enterID = playerIDs; enterID != endEnter; ++enterID)
		{
			if (pkPlayer->GetID() == *enterID)
			{
				continue;
			}
			LC_ServerPlayer* FOVPlayer = (LC_ServerPlayer*)playerManager->FindPlayer(*enterID);
			if (FOVPlayer)
			{
				MC_MessagePackageHelp::Packet(playerEnterMsg, FOVPlayer);
				MC_MessagePackageHelp::Packet(playerEnterPropertyMsg, FOVPlayer);
				MC_MessagePackageHelp::Packet(playerFastPropertyMsg, FOVPlayer);
				MC_MessagePackageHelp::Packet(playerExtPropertyMsg, FOVPlayer);
				MC_MessagePackageHelp::Packet(playerEnterSocietyMsg, FOVPlayer);
				MC_MessagePackageHelp::Packet(playerSkillStateMsg, FOVPlayer, true);
				MC_MessagePackageHelp::Packet(playerEquipMsg, FOVPlayer);
				MC_MessagePackageHelp::Packet(playerSubordinateMsg, FOVPlayer);
				MC_MessagePackageHelp::Packet(playerControllerPropertyMsg, FOVPlayer);
				isEnterMsgSend = true;
				if (MC_MessagePackageHelp::EntryCount(playerEnterMsg) >= MAX_PLAYER_COUNT_IN_FOV)
				{
					break;
				}
			}
		}
		if (isEnterMsgSend)
		{
			pkPlayer->SendMsgToClient(playerEnterMsg.GetMessageID(), &playerEnterMsg);
			pkPlayer->SendMsgToClient(playerEnterPropertyMsg.GetMessageID(), &playerEnterPropertyMsg);
			pkPlayer->SendMsgToClient(playerFastPropertyMsg.GetMessageID(), &playerFastPropertyMsg);
			pkPlayer->SendMsgToClient(playerExtPropertyMsg.GetMessageID(), &playerExtPropertyMsg);
			pkPlayer->SendMsgToClient(playerEnterSocietyMsg.GetMessageID(), &playerEnterSocietyMsg);
			pkPlayer->SendMsgToClient(playerSkillStateMsg.GetMessageID(), &playerSkillStateMsg);
			pkPlayer->SendMsgToClient(playerEquipMsg.GetMessageID(), &playerEquipMsg);
			pkPlayer->SendMsgToClient(playerSubordinateMsg.GetMessageID(), &playerSubordinateMsg);
			pkPlayer->SendMsgToClient(playerControllerPropertyMsg.GetMessageID(), &playerControllerPropertyMsg);
			pkPlayer->GetMessageDirtyFlag().GetUniCastFlag().ClearPlayerEnterOrLeave();
		}
		if (isLeaveMsgSend)
		{
			pkPlayer->SendMsgToClient(leaveMsg.GetMessageID(), &leaveMsg);
			pkPlayer->GetMessageDirtyFlag().GetUniCastFlag().ClearPlayerEnterOrLeave();
		}
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_NPC_IN_FOV, this_source_type, true)
{
	MG_GC_REQ_SYNC_NPC msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	return;

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}


	if (msg.m_kActorIDs.size() > (size_t)MAX_NPC_COUNT_IN_FOV)
	{
		msg.m_kActorIDs.resize((size_t)MAX_NPC_COUNT_IN_FOV);
	}

	{
		const CSObjectIdSet& npcIDsInFov = pkPlayer->GetFOVMap()->GetNPCsIDInFOV();
		/*
		object_id_vector NPCInFovVector(npcIDsInFov.begin(), npcIDsInFov.end());
		/*/
		/// CSObjectIdSet 不支持上述写法
		object_id_vector NPCInFovVector;
		NPCInFovVector.reserve((npcIDsInFov.size()));
		for(CSObjectIdSet::const_iterator it = npcIDsInFov.begin(), itEnd = npcIDsInFov.end(); 
			it != itEnd; ++it)
			NPCInFovVector.push_back(*it);
		//*/
		
 		std::sort(NPCInFovVector.begin(), NPCInFovVector.end());
		
		object_id_type NPCIDs[MAX_NPC_COUNT_IN_FOV * 2] = {0};
		size_t NPCInFovCount = NPCInFovVector.size();
		if (NPCInFovCount > MAX_NPC_COUNT_IN_FOV)
		{
			NPCInFovCount = MAX_NPC_COUNT_IN_FOV;
		}
		object_id_type* endLeave = std::set_difference(msg.m_kActorIDs.begin(), msg.m_kActorIDs.end(),
								   NPCInFovVector.begin(), NPCInFovVector.begin() + NPCInFovCount, NPCIDs);
		MG_SyncNpcLeave leaveMsg;
		bool isLeaveMsgSend = false;
		for (object_id_type* leaveID = NPCIDs; leaveID != endLeave; ++leaveID)
		{
			MC_MessagePackageHelp::Packet(leaveMsg, *leaveID);
			isLeaveMsgSend = true;
			if (leaveMsg.m_lNpcNum >= MAX_NPC_COUNT_IN_FOV)
			{
				break;
			}
		}
		MG_SyncNpcEnter npcEnterMsg;
		Protocol::MG_SyncNPCsPropertyInFOV npcPropertyInFovMsg;
		MG_SyncNPCsSkillStateInFOV	npcSkillStateMsg;
		MG_SyncNPCsInteractionValueInFOV npcInteractionValueMsg;
		bool isEnterMsgSend = false;
		object_id_type* endEnter = std::set_difference(NPCInFovVector.begin(), NPCInFovVector.begin() + NPCInFovCount,
								   msg.m_kActorIDs.begin(), msg.m_kActorIDs.end(), NPCIDs);
		LC_ServerNPCManager* npcManager = SERVER_GET_NPC_MANAGER();
		for (object_id_type* enterID = NPCIDs; enterID != endEnter; ++enterID)
		{
			LC_ServerNPC* FOVNpc = (LC_ServerNPC*)npcManager->FindNPC(*enterID);
			if (FOVNpc)
			{
				MC_MessagePackageHelp::Packet(npcEnterMsg, FOVNpc);
				MC_MessagePackageHelp::Packet(npcPropertyInFovMsg, FOVNpc);
				MC_MessagePackageHelp::Packet(npcSkillStateMsg, FOVNpc);
				MC_MessagePackageHelp::Packet(npcInteractionValueMsg, FOVNpc);
				isEnterMsgSend = true;
				if (npcEnterMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
				{
					break;
				}
			}
		}
		if (isEnterMsgSend)
		{
			pkPlayer->SendMsgToClient(npcEnterMsg.GetMessageID(), &npcEnterMsg);
			pkPlayer->SendMsgToClient(npcPropertyInFovMsg.GetMessageID(), &npcPropertyInFovMsg);
			pkPlayer->SendMsgToClient(npcSkillStateMsg.GetMessageID(), &npcSkillStateMsg);
			pkPlayer->SendMsgToClient(npcInteractionValueMsg.GetMessageID(), &npcInteractionValueMsg);
			pkPlayer->GetMessageDirtyFlag().GetUniCastFlag().ClearNpcEnterOrLeave();
		}
		if (isLeaveMsgSend)
		{
			pkPlayer->SendMsgToClient(leaveMsg.GetMessageID(), &leaveMsg);
			pkPlayer->GetMessageDirtyFlag().GetUniCastFlag().ClearNpcEnterOrLeave();
		}
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SYNC_PING, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_GC_REQ_SYNC_PING reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	{
		MG_GC_RLT_SYNC_PING rltMsg;
		rltMsg.m_token			= reqMsg.m_token;
		rltMsg.m_uiServerTime	= GET_CURRENT_TIMESTAMP_IN_SECONDS();
		pkPlayer->SendMsgToClient(rltMsg.GetMessageID(), &rltMsg);
	}
}

}
