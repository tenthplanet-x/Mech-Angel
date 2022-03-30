#include "LM_LocaleManager.h"
#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "LC_ServerNPCInteractionManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerNPCManager.h"
#include "LC_ServerNPC.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_NPCTransferManager.h"
#include "LC_ServerActorOperationManager.h"
#include "time.h"
#include "UT_ServerHelper.h"
#include "LC_ServerWarManager.h"
#include "LC_ServerGuildManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_BatchPlay.h"
#include "LC_RankManager.h"
#include "oss_define.h"
#include "oss_360_define.h"
#include "str2val.h"
#include "TS_TranManager.h"
#include "LC_CommonStatAsset.h"
#include "LC_ServerMapLogic_WarPlay.h"
#include "LC_ServerMapLogic_SoloDulePlay.h"
#include "LC_GameStory_Manager.h"
#include <boost/algorithm/string/replace.hpp>
#include "LC_GuildRaidDataManager.h"
#include "UT_Timer.h"
#include "LC_ActivationCodeAsset.h"
#include "UT_ActivationCode.h"
#include "LC_ShotActivity.h"
#include "LC_ShortcutBar.h"
#include "LC_RumorManager.h"
#include "LC_ServerMapLogic_NewBee.h"
#include "LC_TaskManager.h"
#include "LC_ServerShopAsset.h"
#include "LC_Arena_Manager.h"

#include "SC_ScriptAction.h"
#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"
#include "SC_ServerDebugInterface.h"
#include "SC_ServerScriptInterfaceHeader.h"

#include "LC_GameEventManager.h"

#include "msg.pb.h"
#include "redis.pb.h"
#include "MG_Common.h"
#include "MG_PB.h"
#include "MG_AssistProtocol.h"
#include "MG_CS_FriendProtocol.h"
#include "MG_ShortcutKeyProtocol.h"
#include "MG_LoginProtocol.h"
#include "LC_TransferBase.h"
#include "LC_PackEntryFactory.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ServerTools.h"
#include "publish_define.h"

#include "CF_ActivationCodeSpecial.h"
#include "CF_VIP.h"
#include "CF_ForcesBattle.h"
#include "CF_GlobalPowerGuildReward.h"
#include "CF_GuildRaid.h"
#include "CF_WorldMapList.h"
#include "CF_LevelUp.h"
#include "CF_ActivationCode.h"
#include "CF_GuildRaid.h"
#include "CF_VIP.h"
#include "CF_TransferArea.h"
#include "CF_ForcesBattle.h"
#include "CF_FriendMsgBroad.h"
#include "ConfigManager.h"
#include "LC_HttpHandler.h"
#include "CF_TranscriptionRule.h"
#include "SystemDefine.h"
#include "LC_ServerTaskManager.h"
#include "CF_BossInvade.h"
#include "../Buddy/BuddyInstance.h"
#include "MG_CS_Guild.h"
#include "UT_OperateLog.h"
#include "gsnode.h"
#include "LC_ServerMapLogic_SkyPlay.h"
#include "LC_ServerMapLogic_SummitTeamPlay.h"
#include "LC_Dule_Manager.h"
#include "CF_GuildLevel.h"
using namespace OPERATELOG;
using namespace CSVFile;
using namespace Protocol;
using namespace GameLogic;
using namespace StateMachine;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_LEVEL_UP, this_source_type, false)
{
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_RESPAWN_ON_CURRENT_LOCATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}


	MG_REQ_RespawnOnCurrentLocation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if (pOwner == NULL)
		return;

	pOwner->ReliveAllBuddy(RELIVE_NOW, reqMsg.m_Auto);

}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_RESPAWN_ON_SAFE_LOCATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_RespawnOnSafeLocation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if (pOwner == NULL)
		return;

	pOwner->ReliveAllBuddy(RELIVE_SAFE);
	
}
//-------------------------------------------------------
ON_HANDLE(MGPT_RLT_SKILL_RESPAWN, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_SkillRespawn rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	object_id_type lRequesterID	= rltMsg.m_lRespawnSourcePlayerID;
	if (rltMsg.m_lConfirmResult == RE_SKILL_ACCEPT_SKILL_RESPAWN)
	{
		int  iReduceExp = 0;
		//添加惩罚状态
		if (pkPlayer->GetLevel() >= MIN_NOT_NEWBIE_LEVEL)
		{
			object_id_type lAttackerID = pkPlayer->GetAttackerID();
			int64_t lBaseEXP	= LC_Helper::GetLevelUpExp(pkPlayer->GetLevel() + 1);
			//被怪物打死
			if (IS_NPC_ID(lAttackerID))
			{
				switch (pkPlayer->GetPKState())
				{
					case PST_WHITE:
					case PST_GRAY:
						{
							iReduceExp = Float2Long(lBaseEXP * 0.015f);
						}
						break;
					case PST_YELLOW:
						{
							iReduceExp = Float2Long(lBaseEXP * 0.015f);
						}
						break;
					case PST_RED:
						break;
					default:
						break;
				}
			}
			else if (IS_PLAYER_CLASS_ID(lAttackerID))	//被玩家打死
			{
				//不处于决斗状态
				if (pkPlayer->GetDuelState()->GetDuelState() != LC_PlayerDuelState::DS_IN_DUEL)
				{
					switch (pkPlayer->GetPKState())
					{
						case PST_WHITE:
						case PST_GRAY:
							{
								//iReduceExp = Float2Long(lPlayerEXP * 0.02f);
							}
							break;
						case PST_YELLOW:
							{
								iReduceExp = Float2Long(lBaseEXP * 0.02f);
							}
							break;
						case PST_RED:
							{
								iReduceExp = Float2Long(lBaseEXP * 0.1f);
							}
							break;
						default:
							break;
					}
				}
			}
		}

		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkRespawnSourcePlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lRequesterID);
		int64_t lPlayerEXP = pkPlayer->GetEXP();
		//副本内技能复活不扣经验
		if (pkPlayer->GetMapLogicType() < MT_TRANSCRIPTION_BEGIN || pkPlayer->GetMapLogicType() > MT_TRANSCRIPTION_END)
		{
			pkPlayer->ReduceEXP(iReduceExp, SYS_FUNC_TP_RESPAWN);
		}
		else
		{
			iReduceExp = 0;
		}
		//发送网络应答
		MG_RLT_Respawn rltMsg;
		rltMsg.m_lPlayerID		= pkPlayer->GetID();
		rltMsg.m_iRespawnType	= MGPT_RLT_SKILL_RESPAWN;
		rltMsg.m_iHP			= pkPlayer->GetSkillRespawnHP();
		rltMsg.m_iMP			= pkPlayer->GetSkillRespawnMP();
		rltMsg.m_iReduceExp		= UT_MathBase::MinInt64(iReduceExp, lPlayerEXP);
		//pkPlayer->SendMsgToClient(MGPT_RLT_RESPAWN, &rltMsg);
		pkPlayer->BroadCastToArea(MGPT_RLT_RESPAWN, &rltMsg);
		pkPlayer->SetSkillRespawnSourcePlayerID(lRequesterID);
		pkPlayer->SkillRespawn();

		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		{
			LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
			if (pkGameEvent != NULL)
			{
				pkGameEvent->SetEventType(GLET_SKILL_RESPAWN);
				pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_AREA);
				pkGameEvent->SetEventParam(EP_RESPAWN_SRC_ACTOR_ID, lRequesterID);
				pkGameEvent->SetEventParam(EP_RESPAWN_DST_ACTOR_ID, pkPlayer->GetID());
				pkGameEvent->SetEventParam(EP_RESPAWN_SKILL_ID, 0);
				pkEventManager->InsertGameEvent(pkGameEvent);
			}
		}

		//在释放复活技能的玩家位置复活
		int		lTargetMapResID = 0;
		UT_Vec3Int	kTargetLocation;
		int		lTargetMapLogicID = 0;
		if (NULL != pkRespawnSourcePlayer)
		{
			lTargetMapResID		= pkRespawnSourcePlayer->GetMapResID();
			kTargetLocation		= pkRespawnSourcePlayer->GetCurrentLocation();
			lTargetMapLogicID	= pkRespawnSourcePlayer->GetMapLogicID();
		}
		else
		{
			lTargetMapResID		= pkPlayer->GetMapResID();
			kTargetLocation		= pkPlayer->GetCurrentLocation();
			lTargetMapLogicID	= pkPlayer->GetMapLogicID();
		}

		LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
		{
			pkServerWorldManager->RequestChangePlayerLocation(pkPlayer,
					LMIOT_SKILL_RESPAWN,
					lTargetMapLogicID,
					lTargetMapResID,
					kTargetLocation);
		}
	}
	else
	{
		pkPlayer->SetSkillRespawnSourcePlayerID(0);
		pkPlayer->SetSkillFlag(SK_SKILL_RESPAWN, false);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_TALKTONPC, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_TalkToNPC reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	object_id_type lNPCID    = reqMsg.m_lNPCID;

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkNPCManager->FindNPC(lNPCID);
	if (!pkNPC)
	{
		return;
	}

	bool bCheckOwnerSuccess = true;
	if (pkNPC->GetProperty_NeedCheckOwnerShip())
	{
		LC_ServerNPCOwnerList& rkOwnerList = pkNPC->GetNPCOwnerList();
		if (!rkOwnerList.HasOwnerID(pkPlayer->GetID()))
		{
			bCheckOwnerSuccess = false;
		}
	}

	if (bCheckOwnerSuccess)
	{
		//设置锁定目标,自动转换到Talk状态
		pkPlayer->SetLockedNpcID(lNPCID);
		pkPlayer->SetLockedTargetID(lNPCID);

		//拥有谈话状态的npc才锁定谈话对象
		if (SFT_DEFAULT == pkNPC->GetAIFuncValue(SF_IDLE_CAN_TALK) || SFT_DEFAULT == pkNPC->GetAIFuncValue(SF_WANDER_CAN_TALK))
		{
			pkNPC->SetLockedTargetID(pkPlayer->GetID());
		}

		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (NULL == pkGameEvent)
		{
			return;
		}
		pkGameEvent->SetEventType(GLET_TALK_TO_NPC);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_NONE);
		pkGameEvent->SetEventParam(EP_TALK_TO_NPC_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_TALK_TO_NPC_CHAR_TYPE_ID, pkNPC->GetCharType());
		pkGameEvent->SetEventParam(EP_TALK_TO_NPC_MAPID, pkPlayer->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
	}

	//1、如果不需要检查npc归属，则客户端会直接与npc交互
	//2、如果需要检查npc归属，则客户端收到服务器的验证消息才会与npc交互
	if (pkNPC->GetProperty_NeedCheckOwnerShip())
	{
		MG_RLT_TalkToNPC kRltTalkToNPC;
		kRltTalkToNPC.m_lPlayerID = pkPlayer->GetID();
		kRltTalkToNPC.m_lNPCID = lNPCID;
		kRltTalkToNPC.m_bCheckNPCOwerShipSuccess = bCheckOwnerSuccess;
		pkPlayer->SendMsgToClient(MGPT_RLT_TALKTONPC, &kRltTalkToNPC);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_NETPLAYER_ATTRIBUTE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_NetPlayerAttribute reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(reqMsg.m_value.charid());
	if (NULL == pkNetPlayer)
	{
		return;
	}

	MG_RLT_NetPlayerAttribute rltMsg;
	pkNetPlayer->PackNetPlayerAttribute(rltMsg);
	pkPlayer->SendMsgToClient(rltMsg.GetMessageID(), &rltMsg);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHORTCUT_PACKAGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ShortcutPackage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	int iPage		= reqMsg.m_btPage;
	int iSlotIndex	= reqMsg.m_btSlotIndex;
	int lTypeID	= reqMsg.m_lTypeID;
	int lExtraVal	= reqMsg.m_btExtraVal;
	
	LC_ShortcutBar* pkShortcutBar = pkPlayer->GetShortcutBar();
	if (pkShortcutBar)
	{
		pkShortcutBar->ShortcutPackage(iPage, iSlotIndex, lTypeID, lExtraVal);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHORTCUT_EQUIP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ShortcutEquip reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	int iPage		= reqMsg.m_btPage;
	int iSlotIndex	= reqMsg.m_btSlotIndex;
	int lTypeID	= reqMsg.m_lTypeID;
	
	LC_ShortcutBar* pkShortcutBar = pkPlayer->GetShortcutBar();
	if (pkShortcutBar)
	{
		pkShortcutBar->ShortcutEquip(iPage, iSlotIndex, lTypeID);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHORTCUT_SKILL, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ShortcutSkill reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	int iPage		= reqMsg.m_btPage;
	int iSlotIndex	= reqMsg.m_btSlotIndex;
	int lTypeID	= reqMsg.m_lTypeID;
	
	LC_ShortcutBar* pkShortcutBar = pkPlayer->GetShortcutBar();
	if (pkShortcutBar)
	{
		LC_SkillAssetEntry* pkEntry = pkPlayer->GetSkillAsset()->FindLearnedSkill(lTypeID);
		if (pkEntry)
		{
			pkShortcutBar->ShortcutSkill(iPage, iSlotIndex, pkEntry->GetTypeID());
		}
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHORTCUT_VEHICLE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ShortcutVehicle reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	int iPage		= reqMsg.m_btPage;
	int iSlotIndex	= reqMsg.m_btSlotIndex;
	int lTypeID	= reqMsg.m_lTypeID;
	
	LC_ShortcutBar* pkShortcutBar = pkPlayer->GetShortcutBar();
	if (pkShortcutBar)
	{
		pkShortcutBar->ShortcutVehicle(iPage, iSlotIndex, lTypeID);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHORTCUT_EXCHANGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ShortcutExchange reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	int iPage1		= reqMsg.m_btPage1;
	int iSlotIndex1	= reqMsg.m_btSlotIndex1;
	int iPage2		= reqMsg.m_btPage2;
	int iSlotIndex2	= reqMsg.m_btSlotIndex2;
	
	LC_ShortcutBar* pkShortcutBar = pkPlayer->GetShortcutBar();
	if (pkShortcutBar)
	{
		pkShortcutBar->ShortcutExchange(iPage1, iSlotIndex1, iPage2, iSlotIndex2);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHORTCUT_REMOVE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ShortcutRemove reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	int iPage		= reqMsg.m_btPage;
	int iSlotIndex	= reqMsg.m_btSlotIndex;

	LC_ShortcutBar* pkShortcutBar = pkPlayer->GetShortcutBar();
	if (pkShortcutBar)
	{
		pkShortcutBar->ShortcutRemove(iPage, iSlotIndex);
	}
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TRANSFER_DETAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	object_id_type nPlayerId = pkPlayer->GetOwnerPlayerId();

	MG_REQ_TransferDetail reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_SS_TransferDetail_REQ  reqData;
	reqData.nPlayerId = nPlayerId;
	reqData.nMapResId = reqMsg.m_iResID;

	SendMsgToLogin(MGPT_REQ_TRANSFER_DETAIL, &reqData, 0, 0  );
	//GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_SYSTEM, "fenxian, recv client MGPT_REQ_TRANSFER_DETAIL, send to login, nPlayerId=%d, mapResID=%d", nPlayerId, reqMsg.m_iResID);
}

ON_HANDLE(MGPT_REQ_GUILD_FIGHT_TOWER_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer->SyncGuildMapInfoToPlayer();
}

ON_HANDLE(MGPT_REQ_GUILD_FIGHT_GUILD_MEMBER_ID_INFO, this_source_type, false)
{
	// 获取 公会成员列表及公会战积分
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_GuildFightGuildMemberIdInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	std::map<uint32_t, uint32_t> m_score;
	UT_ServerHelper::ReqGuildFightInfoToSociety(UpdateGuildFightType::UGFT_GET_ALL_MEMBER_POINT,reqMsg.m_nGuild,pkPlayer->GetInstance(),false,0,0,m_score);
}

ON_HANDLE(MGPT_REQ_GUILD_FIGHT_PLAYER_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_GuildFightPlayerInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	if(reqMsg.m_checkTime)
	{
		if(!UT_ServerHelper::CheckStoryIsActive(GameStoryIDType_BestGuild))
			return;
	}
	UT_ServerHelper::ReqPlayerOfflineToDb(OfflinePlayerFromDBType::OPFDT_GUILD_FIGHT_PLAYER_INFO,reqMsg.m_IdVec,0,0,0,pkPlayer->GetUserID(),pkPlayer->GetInstance());
	
}

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TRANSFER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_Transfer reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	if (reqMsg.m_iIndex < 0)
		return;

	ResultType res = pkPlayer->TransferMap(reqMsg.m_iTransferType, reqMsg.m_iResId, reqMsg.m_iIndex, reqMsg.m_iServerID, reqMsg.m_bTribuMap, reqMsg.m_iLineNum);
	const unique_id_impl& instance = pkPlayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_REQ_TRANSFER res:%d, serverid:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, m_iTransferType:%d, m_iResId:%d,m_iIndex:%d, m_iServerID:%d, m_bTribuMap:%d, m_iLineNum:%d\n", 
		res, GetGameServerID(), instance.detail(), instance.catagory(), pkPlayer->GetUserID(), reqMsg.m_iTransferType, reqMsg.m_iResId, reqMsg.m_iIndex, reqMsg.m_iServerID,  reqMsg.m_bTribuMap, reqMsg.m_iLineNum);
	if(RE_SUCCESS != res)
	{
		MG_Message errorMsg;
		errorMsg.m_iMessageID = res;
		pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
	}
}

ON_HANDLE(MGPT_REQ_TRANSFERBATTLEPLAYBACKMAP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_TransferBattlePlaybackMap reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	pkPlayer->TransferBattlePlaybackMap(reqMsg.m_iMapResID);
}

//-----------------------------------------------------------------------
ON_HANDLE(MGPT_GAW_REQ_SHOP_TRANSFERITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if(pkPlayer == NULL)
	{
		return;
	}

	MG_REQ_ShopTransferItem reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerMapLogic_Place* pkMapLogic = NULL;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkMapLogic = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (pkMapLogic == NULL)
		return;

	PlaceLogicState result = pkMapLogic->GetPlaceState();
	if(result == PLACESTATE_PREEND)
	{
		CF_TranscriptionRule::DataEntry* pkRuleData = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(reqMsg.m_iMapResId);
		if (!pkRuleData)
		{
			return;
		}

		CF_ShopList::DataEntry* pkShopData = SafeGetCSVFileDataEntryPtr<CF_ShopList>(pkRuleData->_ShopID);
		if(!pkShopData)
		{
			return;
		}

		ResultType iResult = LC_Helper::PayCashMap(pkPlayer, pkShopData->_ActualPriceList, SYS_FUNC_TP_TRANSPORT_SHOP, pkShopData->_iItemID);
		if(iResult != RE_SUCCESS)
		{
			return;
		}
		iResult = LC_Helper::AddItem(pkPlayer->GetPackAsset(), pkShopData->_iItemID, 1, SYS_FUNC_TP_TRANSPORT_SHOP, false);
		if(RE_SUCCESS != iResult)//pkPlayer->AddItem(pkShopData->_iItemID, 1))
		{
			UT_ServerHelper::LC_ItemCntList kItemList;
			UT_ServerHelper::__ItemCnt rItemCnt;
			rItemCnt.m_nId = pkShopData->_iItemID;
			rItemCnt.m_nCnt =1;
			kItemList.push_back(rItemCnt);
			UT_ServerHelper::SendToMail(pkRuleData->_sMailTitle, pkRuleData->_sMailDesc, pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName(), 0, kItemList);
		}		
		pkMapLogic->SetPlaceState(PLACESTATE_END);
	}
}

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SEEKTREASURE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SeekTreasure	reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
	{
		return;
	}
	pkGameEvent->SetEventType(GLET_SEEK_TREASURE);
	pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkPlayer->GetID());
	pkEventManager->InsertGameEvent(pkGameEvent);
}
//-----------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DIGTREASURE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SeekTreasure	reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
	{
		return;
	}
	pkGameEvent->SetEventType(GLET_DIG_TREASURE);
	pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkPlayer->GetID());
	pkEventManager->InsertGameEvent(pkGameEvent);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_MIDAUTUNMDAY_GET_ITEM, this_source_type, false)
{
	
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_QUIT_TRANSCRIPTION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_QUIT_Transcription reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (ptr == NULL)
	{
		return;
	}

	const unique_id_impl& playerInstance = pkPlayer->GetInstance();
	pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_QUIT_TRANSCRIPTION, 0, 1,	UT_Vec3Int());
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MG_REQ_QUIT_Transcription-quit_result transfer c_cid:[%d], c_citizenship:[%d], c_uid:[%d], resMapId:[%d], logicMapID:[%d], time:[%d]\n", 
		playerInstance.detail(), playerInstance.catagory(), pkPlayer->GetUserID(), pkPlayer->GetMapResID(), pkPlayer->GetMapLogicID(), GET_CURRENT_TIMESTAMP_IN_SECONDS());
}
//-----------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GET_BUY_TIMES, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetBuyTimes reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	MG_RLT_GetBuyTimes respMsg;
	respMsg.m_type = MSGTYPE_GET_TIMES;
	respMsg.m_iResult = RE_SUCCESS;
	respMsg.m_iGameStoryId = reqMsg.m_iGameStoryId;
	respMsg.m_iBuyType = reqMsg.m_iBuyType;
	pkPlayer->GetBuyVipInfo(reqMsg.m_iGameStoryId, reqMsg.m_iBuyType, respMsg.m_buyTimes, respMsg.m_totalTimes, respMsg.m_costCount, respMsg.m_costType);
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_BUY_TIMES, &respMsg);
}
//-------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_BUY_TIMES, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_BuyTimes reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->BuyVIPTimes(reqMsg.m_iGameStoryId, reqMsg.m_iBuyType, reqMsg.m_iTimes);
}
//-----------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_QUICKRAID_NEW, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	
	MG_REQ_QuickRaid_New reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetQuickRaidAsset().QuickRaid_New(pkPlayer, reqMsg.m_nResMapID, reqMsg.m_nRaidNum);
}

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_NPC_UPGRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_NPC_UPGRADE reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
	{
		return;
	}
	pkGameEvent->SetEventType(GLET_GAMEPLAYER_UPGRADE_NPC);
	pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkPlayer->GetID());
	pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, reqMsg.m_lNPCID);
	pkGameEvent->SetEventParam(EVENT_BASE_PARAM_3, reqMsg.m_lNPCCharType);
	pkGameEvent->SetEventParam(EVENT_BASE_PARAM_4, reqMsg.m_lUpdateIndex);
	pkEventManager->InsertGameEvent(pkGameEvent);
}

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SET_TRANSFER_POS, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetTransferPos reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}


	ResultType iResult = RE_TRANSFER_NPC_CANT_BIND;
	LC_NPCManagerBase* const pkNPCManager = ENGINE_GET_NPC_MANAGER();
	if (pkNPCManager)
	{
		LC_NPCBase* const pkNPC = pkNPCManager->FindNPC(reqMsg.m_lNPCID);
		if (pkNPC)
		{
			int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pkNPC->GetCurrentLocation());
			if (iDistance > MAX_NPC_INTERACTION_DISTANCE)
			{
				return;
			}
			LC_NPCTransferManager* pkTranferManager = LC_NPCTransferManager::GetSingletonPtr();
			if (!pkTranferManager)
			{
				return;
			}
			LC_TransferBase* const pkNPCTransfer = pkTranferManager->GetNPCTransfer(pkNPC->GetCharType());
			if (pkNPCTransfer)
			{
				if (pkNPCTransfer->CanBindTransferPos())
				{
					int iIndex = pkNPCTransfer->GetBindTransferPosIndex();
					CF_TransferArea* pkTransferArea = CF_TransferArea::GetSingletonPtr();
					CF_TransferArea::DataEntry* pkAreaData = pkTransferArea->GetEntryPtr(iIndex);
					if (pkAreaData)
					{
						pkPlayer->SetDefaultTransferPos(pkAreaData->_lMapID, pkAreaData->_kPos);
						iResult = RE_TRANSFER_NPC_BIND_SUCCESS;
					}
				}
			}
		}
	}
	MG_RLT_SetTransferPos rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_iResult   = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_SET_TRANSFER_POS, &rltMsg);

	if (iResult == RE_TRANSFER_NPC_BIND_SUCCESS)
	{
		pkPlayer->SyncDefaultTransferPos();
	}
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SET_TITLE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SetTitle msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	ResultType iResult = RE_SUCCESS;
	LC_TitleAsset* pkTitleAsset = pkPlayer->GetTitleAsset();
	if (msg.m_iSetTitleType == TNT_SYSTEM_TITLE && !pkTitleAsset->IsHoldSystemTitle(msg.m_iSetTitleIndex))
	{
		iResult = RE_TITLE_NOT_EXIST;
	}
	if (iResult == RE_SUCCESS)
	{
		pkTitleAsset->SetCurrentTitle(msg.m_iSetTitleType, msg.m_iSetTitleIndex);
		StringType Title = pkTitleAsset->GetTitle(msg.m_iSetTitleType, msg.m_iSetTitleIndex);
		pkPlayer->SetCharTitle(Title);
		pkPlayer->SetChangeTitleFlag(true);
		if (msg.m_iSetTitleType == TNT_DIY_TITLE)
		{
			iResult = RE_TITLE_DIY_SET_SUCCESS;
		}
		else if (msg.m_iSetTitleType == TNT_SYSTEM_TITLE)
		{
			LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
			LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
			if (pkGameEvent == NULL)
			{
				return;
			}
			pkGameEvent->SetEventType(GLET_CHANGE_SYSTEM_TITLE);
			pkGameEvent->SetEventParam(EP_CHANGE_SYSTEM_TITlE_PLAYER_ID, pkPlayer->GetID());
			pkGameEvent->SetEventParam(EP_CHANGE_SYSTEM_TITlE_ID, msg.m_iSetTitleIndex);
			pkEventManager->InsertGameEvent(pkGameEvent);
			iResult = RE_TITLE_SET_SUCCESS;
		}
	}
	MG_RLT_SetTitle rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_iResult = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_SET_TITLE, &rltMsg);

	//称号系统立即同步
	pkPlayer->SyncTitleNameInfoToClient();
	pkPlayer->SetDirtyFlag(DIRIY_FLAG_TITLE_NAME, true);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_SYSTEM_TITLE, this_source_type, false)
{
}

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_REMOVE_SYSTEM_TITLE, this_source_type, false)
{
}

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_LEARNPRODUCTION, this_source_type, false)
{
	/*
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	return;
	*/
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_FORGETPRODUCTION, this_source_type, false)
{
	/*
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	return;
	*/
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_UPGRADEPRODUCTION, this_source_type, false)
{
	/*
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	return;
	*/
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_COLLECT_ITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CollectItem reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	uint32_t lOBJID = reqMsg.m_lOBJID;
	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)(pkNPCManager->FindNPC(lOBJID));
	if (pkNPC == NULL)
	{
		return;
	}
	int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pkNPC->GetCurrentLocation());
	if (iDistance > MAX_NPC_INTERACTION_DISTANCE)
	{
		return;
	}
	LC_ServerNPCInteractionManager* pkManager = (LC_ServerNPCInteractionManager*)LC_ServerNPCInteractionManager::GetSingletonPtr();
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
	{
		return;
	}
	ResultType iResult = pkManager->NPCInteraction(pkNPC, pkPlayer);
	GameLogic::ResultInfo& rResultInfo = pkManager->GetResultInfo();
	pkGameEvent->SetEventType(GLET_PLAYER_PICK_ITEM);
	pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
	pkGameEvent->SetEventParam(EP_PICK_ITEM_PLAYER_ID, pkPlayer->GetID());
	pkGameEvent->SetEventParam(EP_PICK_ITEM_OBJECT_ID, pkNPC->GetID());
	pkGameEvent->SetEventParam(EP_PICK_ITEM_OBJECT_TYPE_ID, pkNPC->GetCharType());
	pkGameEvent->SetEventParam(EP_PICK_ITEM_RESULT, iResult);
	pkGameEvent->SetEventParam(EP_PICK_ITEM_RESULT_PARAM, rResultInfo.m_lParam1);
	pkGameEvent->SetEventParam(EP_PICK_ITEM_MAP_ID, pkPlayer->GetMapResID());
	pkEventManager->InsertGameEvent(pkGameEvent);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SET_TUTORIAL_DATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SetTutorialData reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	
	int	nIndex = reqMsg.m_lIndex;
	if (nIndex < 0 || nIndex >= MG_TUTORIALDATA_LENGTH * CHAR_BIT)
	{
		return;
	}
	
	pkPlayer->GetTutorialAsset()->SetBitVec(nIndex, true);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_NPC_OWNERSHIP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_NPC_Ownership reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();
	

	LC_ServerNPC* pkNPC = (LC_ServerNPC*)(pkNPCManager->FindNPC(reqMsg.m_lNPCID));
	if (!pkNPC)
	{
		return;
	}
	bool bOwnerShip = false;
	LC_ServerNPCOwnerList& rkNPCOwnerList = pkNPC->GetNPCOwnerList();
	if (rkNPCOwnerList.GetOwnerCount() == 0)
	{
		bOwnerShip = true;
	}
	else
	{
		bOwnerShip = rkNPCOwnerList.HasOwnerID(pkPlayer->GetID());
	}
	MG_RLT_NPC_Ownership rltMsg;
	rltMsg.m_lNPCID = reqMsg.m_lNPCID;
	rltMsg.m_bOwnerShip = bOwnerShip;
	pkPlayer->SendMsgToClient(MGPT_RLT_NPC_OWNERSHIP, &rltMsg);
}

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SetOperationParameter reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ActorOperationManagerBase* pkOperationManager = LC_ServerActorOperationManager::GetSingletonPtr();
	if (NULL == pkOperationManager)
	{
		return;
	}
	int lOperationType		= reqMsg.m_lOperationType;
	int lParameterIndex	= reqMsg.m_lParameterIndex;
	int lVal				= reqMsg.m_lParameterVal;

	// 区分 英雄 和指挥官

	LC_ActorOperationEntry* pkOperationEntry = pkPlayer->GetActorOperationAsset()->GetEntryByOperationType(lOperationType);
	if (NULL == pkOperationEntry)
	{
		return;
	}
	pkOperationManager->SetOperationParameter(pkOperationEntry, pkPlayer, lParameterIndex, lVal);

	MG_RESP_SetOperationParameter respMsg;
	respMsg.m_lOperationType = lOperationType;
	respMsg.m_lParameterIndex = lParameterIndex;
	respMsg.m_lParameterVal = lVal;
	pkPlayer->SendMsgToClient(MGPT_RLT_SET_ACTOR_OPERATION_PARAMETER, &respMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_EXECUTE_ACTOR_OPERATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ExecuteOperation reqMsg;
	// 测试
	//usBodySize = 16;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ActorOperationManagerBase* pkOperationManager = LC_ServerActorOperationManager::GetSingletonPtr();
	if (NULL == pkOperationManager)
	{
		return;
	}
	// 测试
	//reqMsg.m_lOperationType = LAOT_EQUIP_SLOT_UPGRADE;
	int lOperationType		= reqMsg.m_lOperationType;

	LC_ActorOperationEntry* pkOperationEntry = pkPlayer->GetActorOperationAsset()->GetEntryByOperationType(lOperationType);
	if (NULL == pkOperationEntry)
	{
		return;
	}

	MG_RLT_ExecuteOperation rltMsg;
	rltMsg.m_lOperationType = reqMsg.m_lOperationType;

	rltMsg.m_iResult = pkOperationManager->CheckOperationExecute(pkOperationEntry, pkPlayer);

	if (RE_SUCCESS == rltMsg.m_iResult)
	{
		// 测试
		/*reqMsg.m_nBuddyID = 1;
		reqMsg.m_nIndex  = 0;
		reqMsg.m_nDescID = 11360001;
		rltMsg.m_iResult = pkOperationManager->ExecuteOperation(pkOperationEntry, pkPlayer,reqMsg.m_nBuddyID ,reqMsg.m_nIndex,reqMsg.m_nDescID);*/

		// 测试
		/*reqMsg.m_nBuddyID = 1;
		reqMsg.m_nIndex  = 1;
		reqMsg.m_nDescID = 11361001;
		rltMsg.m_iResult = pkOperationManager->ExecuteOperation(pkOperationEntry, pkPlayer,reqMsg.m_nBuddyID ,reqMsg.m_nIndex,reqMsg.m_nDescID);*/

		// 测试
		/*reqMsg.m_nBuddyID = 1;
		reqMsg.m_nIndex  = 2;
		reqMsg.m_nDescID = 11364001;
		rltMsg.m_iResult = pkOperationManager->ExecuteOperation(pkOperationEntry, pkPlayer,reqMsg.m_nBuddyID ,reqMsg.m_nIndex,reqMsg.m_nDescID);*/

		// 测试
		/*reqMsg.m_nBuddyID = 1;
		reqMsg.m_nIndex  = 4;
		reqMsg.m_nDescID = 11362001;*/
		rltMsg.m_iResult = pkOperationManager->ExecuteOperation(pkOperationEntry, pkPlayer,reqMsg.m_nBuddyID ,reqMsg.m_nIndex,reqMsg.m_nDescID);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_EXECUTE_ACTOR_OPERATION, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_RESET_ACTOR_OPERATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ResetOperation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ActorOperationManagerBase* pkOperationManager = LC_ServerActorOperationManager::GetSingletonPtr();
	if (NULL == pkOperationManager)
	{
		return;
	}
	int lOperationType		= reqMsg.m_lOperationType;
	bool bClearParameters   = reqMsg.m_bClearParameters;
	LC_ActorOperationEntry* pkOperationEntry = pkPlayer->GetActorOperationAsset()->GetEntryByOperationType(lOperationType);
	if (NULL == pkOperationEntry)
	{
		return;
	}
	pkOperationManager->ResetOperation(pkOperationEntry, pkPlayer, bClearParameters);
}
//---------------------------------------------------------------------------------------------
ON_HANDLE(MGPT_NOTIFY_SERVER_ALREADY_IN_GAME, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_NOTIFY_SERVER_ALREADY_IN_GAME msgReceived;
	if (!msgReceived.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	const unique_id_impl& playerInstance = pkPlayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_NOTIFY_SERVER_ALREADY_IN_GAME mapLogicID:%d mapResID:%d, c_id:%d, c_citizenship:%d, c_uid:%d", 
		msgReceived.m_iLogicMapID , msgReceived.m_iResMapID, playerInstance.detail(), playerInstance.catagory(), pkPlayer->GetUserID());

	int32_t mapResMapID = pkPlayer->GetMapResID();
	int mapLogicMapID = pkPlayer->GetMapLogicID();
	if(mapLogicMapID!=msgReceived.m_iLogicMapID || mapResMapID!=msgReceived.m_iResMapID || !pkPlayer->IsClientLoading())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_NOTIFY_SERVER_ALREADY_IN_GAME error! c_id:%d, c_citizenship:%d, c_uid:%d, playerMapLogicID:%d msgMapLogicID:%d, playerResID:%d, msgMapResID:%d, loading:%d", 
			playerInstance.detail(), playerInstance.catagory(), pkPlayer->GetUserID(), mapLogicMapID, msgReceived.m_iLogicMapID, mapResMapID, msgReceived.m_iResMapID, pkPlayer->IsClientLoading());
		return;
	}

	pkPlayer->ClientLoadFinished();

	pkPlayer->SetAttackFlag(true);
	
	//Insert Event, The TranscriptionMgr should handle
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent != NULL)
	{
		pkGameEvent->SetEventType(GLET_CLIENT_ALREADY_IN_GAME);
		pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, msgReceived.m_iLogicMapID);
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
	
	pkPlayer->PostEnterMap();
	//
	pkPlayer->SetActivateSummitArenaState(true);
	pkPlayer->NotifyActivateArena2Client();
}

//--------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TRANSCRIPTION_PROGRESS, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_TRANSCRIPTION_PROGRESS msgReceived;
	if (!msgReceived.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (ptr == NULL)
	{
		return;
	}
	LC_ServerMapLogic_BatchPlay* play = (LC_ServerMapLogic_BatchPlay*) ptr->GetPlay(PlayType_Batch);
	if (play == NULL)
	{
		return;
	}
	Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS msg;
	pkPlayer->SendMsgToClient(MGPT_NOTIFY_TRANSCRIPTION_PROGRESS, play->progress2proto(msg));
}

ON_HANDLE(MGPT_REQ_TRANSCRIPTION_BEGIN_DIALOGUE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_BEGIN_DIALOGUE msgReceived;
	if (!msgReceived.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (ptr == NULL)
	{
		return;
	}
	ptr->ClientBeginDialogue(pkPlayer, msgReceived.m_dialogueId);
}

ON_HANDLE(MGPT_REQ_TRANSCRIPTION_FINISH_DIALOGUE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_FINISH_DIALOGUE msgReceived;
	if (!msgReceived.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (ptr == NULL)
	{
		return;
	}
	ptr->ClientFinishDialogue(pkPlayer);
}

ON_HANDLE(MGPT_REQ_RETURNTOOUT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ReturnToOut msgReceived;
	if (!msgReceived.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//查询是否有资产
	LC_PortalEntry* entry = pkPlayer->GetPortalAsset()->GetMapPortal(pkPlayer->GetMapResID());
	if (entry == NULL)
	{
		return;
	}
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	ResultType ret = pkServerWorldManager->RequestChangePlayerLocation(pkPlayer,
					 LMIOT_RETURN_CITY,
					 0,
					 entry->m_TargetMapResId,
					 entry->m_TargetPos);
	if (ret != RE_SUCCESS)
	{
		return;
	}
	pkPlayer->GetPortalAsset()->DelPlayerPortal(pkPlayer->GetID());
}


ON_HANDLE(MGPT_REQ_BAILIAN_CONVEY1, this_source_type, false)
{
	
}

ON_HANDLE(MGPT_REQ_BAILIAN_CONVEY2, this_source_type, false)
{
	
}

ON_HANDLE(MGPT_REQ_BAILIAN_AWARDBUFF, this_source_type, false)
{
	
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_NOTIFY_SERVER_ALREADY_IN_CENTER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_NOTIFY_SERVER_ALREADY_IN_CENTER_IN_CAREERLEARN_TRANSCRIPTION msgReceived;
	if (!msgReceived.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (msgReceived.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}

	//Insert Event, The TranscriptionMgr should handle
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
	{
		return;
	}
	pkGameEvent->SetEventType(GLET_PLAYER_IN_CENTER);
	pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkPlayer->GetID());
	pkEventManager->InsertGameEvent(pkGameEvent);
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAVE_SHORTCUT_KEY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SaveShortcutKey reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ShortcutKeyAsset* pkShortcutKeyAsset = pkPlayer->GetShortcutKeyAsset();
	pkShortcutKeyAsset->GetDataFromProtocolStruct(reqMsg.m_kShortcutKeyInfo);

	//发送网络应答
	MG_RLT_SaveShortcutKey rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_iResult = RE_SUCCESS;
	pkPlayer->SendMsgToClient(MGPT_RLT_SAVE_SHORTCUT_KEY, &rltMsg);
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHOW_FASHION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ShowFashion reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	bool	bShow = reqMsg.m_bIsShowFashion;
	/*
	if (reqMsg.m_lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (NULL == pkPlayer)
	{
		return;
	}

	pkPlayer->SetIsShowFashion(bShow);
	pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
}


ON_HANDLE(MGPT_REQ_EIGHTEEN_BRONZEMAN_TEACHTASK, this_source_type, false)
{
	
}

ON_HANDLE(MGPT_REQ_EIGHTEEN_BRONZEMAN_BEGINCHALLENGE, this_source_type, false)
{
	
}

ON_HANDLE(MGPT_REQ_HIDDENWEAPON_TEACHTASK, this_source_type, false)
{
	
}

ON_HANDLE(MGPT_REQ_HIDDENWEAPON_BEGINCHALLENGE, this_source_type, false)
{
	
}

ON_HANDLE(MGPT_REQ_HOOK_INFO, this_source_type, false)
{
	return;
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_HOOKUIITEM, this_source_type, false)
{
	return;
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADDHOOKTIME_ITEM, this_source_type, false)
{
	return;
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_RECV_LUCK_AWARD, this_source_type, false)
{
	return;
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAFE_SYSTEM_SET_SECOND_PASSWORD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SetSafeSystemSecondPassword reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (pkSafeSystemAsset)
	//{
	//	pkSafeSystemAsset->SetSecondPassword(STDSTR2TPSTR(reqMsg.m_cPassword));
	//}
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAFE_SYSTEM_CHANGE_SECOND_PASSWORD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ChangeSafeSystemSecondPassword reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (pkSafeSystemAsset)
	//{
	//	pkSafeSystemAsset->ChangeSecondPassword(STDSTR2TPSTR(reqMsg.m_cOldPassword), STDSTR2TPSTR(reqMsg.m_cNewPassword));
	//}
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAFE_SYSTEM_RESET_SECOND_PASSWORD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ResetSafeSystemSecondPassword reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (pkSafeSystemAsset)
	//{
	//	pkSafeSystemAsset->ResetSecondPassword();
	//}
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAFE_SYSTEM_CANCEL_RESET_SECOND_PASSWORD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CancelResetSafeSystemSecondPassword reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (pkSafeSystemAsset)
	//{
	//	pkSafeSystemAsset->CancelResetSecondPassword(STDSTR2TPSTR(reqMsg.m_cPassword));
	//}
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAFE_SYTEM_TRY_SECOND_PASSWORD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_TrySafeSystemSecondPassword reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (pkSafeSystemAsset)
	//{
	//	pkSafeSystemAsset->TrySecondPassword(STDSTR2TPSTR(reqMsg.m_cPassword));
	//}
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAFE_SYSTEM_DELAY_SET_SECOND_PASSWORD_TIME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_DelaySafeSystemSetSecondPasswordTime reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (pkSafeSystemAsset)
	//{
	//	pkSafeSystemAsset->DelayNotifySecondPassword();
	//}
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAFE_SYSTEM_SET_SAFE_LOCK_TIME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SetSafeSystemSafeLockTime reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (pkSafeSystemAsset)
	//{
	//	pkSafeSystemAsset->SetSafeLockTime(reqMsg.m_uiSafeLockTime);
	//}
}
//---------------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_REPLY_CAPTCHA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Reply_Captcha reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//LC_ServerSafeSystemAsset* pkSafeSystemAsset = (LC_ServerSafeSystemAsset*)pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//pkSafeSystemAsset->CheckCaptcha(reqMsg.m_lCaptcha, reqMsg.m_lCaptchaRevisionID);
}

ON_HANDLE(MGPT_REQ_GUILDWAR_CATAPULT_FIRE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GuildWar_FireCatapult_Req req;
	if (!req.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_GuildWar_FireCatapult_Resp resp;
	resp.m_CatapultId = req.m_CatapultId;
	resp.m_TargetX = req.m_TargetX;
	resp.m_TargetY = req.m_TargetY;
	resp.m_CatapultLevel = req.m_CatapultLevel;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* maplogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (maplogic == NULL)
	{
		return;
	}
	LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)maplogic->GetPlay(PlayType_War);
	if (play == NULL)
	{
		return;
	}
	WarCatapult* catapult = play->GetCatapult(req.m_CatapultId);
	if (catapult == NULL)
	{
		return;
	}
	Utility::UT_Vec3Int pos(req.m_TargetX, req.m_TargetY, 0);
	resp.m_Result = catapult->Fire(pkPlayer, pos, req.m_CatapultLevel, resp.m_Cost);
	if (resp.m_Result != RE_SUCCESS)
	{
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDWAR_CATAPULT_FIRE, &resp);
	}
}

ON_HANDLE(MGPT_REQ_GUILDWAR_ENTER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return;
	}
	ResultType ret = manager->EnterLocalGuildWar(pkPlayer);
	if (ret != RE_SUCCESS)
	{
		MG_EnterGuild_Resp resp;
		resp.m_Result = ret;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDWAR_ENTER, &resp);
	}
}

ON_HANDLE(MGPT_REQ_GLOBALGUILDWAR_ENTER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return;
	}
	ResultType ret = manager->EnterCrossRealmGuildWar(pkPlayer);
	if (ret != RE_SUCCESS)
	{
		MG_EnterGlobalGuild_Resp resp;
		resp.m_Result = ret;
		pkPlayer->SendMsgToClient(MGPT_RESP_GLOBALGUILDWAR_ENTER, &resp);
	}
}

ON_HANDLE(MGPT_REQ_SIGNIN, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_SignIn_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_SignIn_Resp rltMsg;
	rltMsg.m_nType = msg.m_nType;
	rltMsg.m_Result = pkPlayer->SignInByType(msg.m_nType);
	pkPlayer->SendMsgToClient(MGPT_RESP_SIGNIN, &rltMsg);
}

ON_HANDLE(MGPT_REQ_RESIGNIN, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_ReSignIn_Req reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_ReSignIn_Resp respMsg;
	respMsg.m_nType = reqMsg.m_nType;
	respMsg.m_nDay = reqMsg.m_Day;
	respMsg.m_Result = pkPlayer->ReSignInByType(reqMsg.m_nType, reqMsg.m_Day);
	pkPlayer->SendMsgToClient(MGPT_RESP_RESIGNIN, &respMsg);
}

ON_HANDLE(MGPT_REQ_SIGNINREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_SignInReward_Req reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_SignInReward_Resp respMsg;
	respMsg.m_nType = reqMsg.m_nType;
	respMsg.m_nID = reqMsg.m_nID;
	respMsg.m_Result = pkPlayer->GetSignInRewardItem(reqMsg.m_nType, reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RESP_SIGNINREWARD, &respMsg);
}

ON_HANDLE(MGPT_REQ_DOMAINREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_DomainReward_Req reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_DomainReward_Resp resp;
	resp.m_Result = RE_SUCCESS;
	resp.m_ResId = reqMsg.m_ResId;
	if (pkPlayer->GetGuildID() == INVALID_GUILD_ID)
	{
		resp.m_Result = RE_DOMAIN_NOT_IN_POWERGUILD;
		pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
		return;
	}
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	const DomainPowerMap* thepowermap = pWorldGuildWarAsset->GetDomainPowerGuildInfo(pkPlayer->GetActiveServer());
	if (thepowermap == NULL)
	{
		resp.m_Result = RE_DOMAIN_NOT_OPEN;
		pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
		return;
	}
	if (resp.m_ResId != 0)
	{
		DomainPowerMap::const_iterator it = thepowermap->find(resp.m_ResId);
		if (it == thepowermap->end())
		{
			resp.m_Result = RE_DOMAIN_NOT_OPEN;
			pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
			return;
		}
		if (it->second.m_GuildId != pkPlayer->GetGuildID())
		{
			resp.m_Result = RE_DOMAIN_NOT_IN_POWERGUILD;
			pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
			return;
		}
		if (!pkPlayer->GetPlayerGuildAssert()->CheckDomainReward(resp.m_ResId, it->second.m_TimeStamp))
		{
			resp.m_Result = RE_DOMAIN_REWARD_GETTED;
			pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
			return;
		}
		CF_ForcesBattle* forcebattle = CF_ForcesBattle::GetSingletonPtr();
		CF_ForcesBattle::DataEntry* entry = forcebattle->GetEntryPtr(resp.m_ResId);
		if (entry == NULL)
		{
			return;
		}
		if (!entry->_Rewards.empty())
		{
			ResultType ret = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, entry->_Rewards, pkPlayer, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar, "", "", false);
			if (ret != RE_SUCCESS)
			{
				resp.m_Result = ret;
				pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
				return;
			}

			Utility::UT_SIMDataList::iterator itReward = entry->_Rewards.begin();
			int i = 0;
			for (; i<MAX_ITEM_LEN&&itReward!=entry->_Rewards.end() ; ++itReward, ++i)
			{
				resp.m_item[i] = itReward->ID();
				resp.m_itemCount[i] = itReward->ItemCount();
			}
			resp.m_itemlen = i;
		}
		//经验
		int playerlevel = pkPlayer->GetLevel();
		TStringVector kParams;
		kParams.push_back("BASE");
		kParams.push_back("PLAYERLV");

		attr_value_type iExp = 0;
		SA_RT_TRIGGER_EXPRESSION(attr_value_type, iExp, entry->_Exp, kParams, LC_Helper::BaseLevelExp(playerlevel), playerlevel);

		MoneyType iCash = 0;
		SA_RT_TRIGGER_EXPRESSION(MoneyType, iCash, entry->_Gold, kParams, LC_Helper::BaseLevelCash(playerlevel), playerlevel);

		attr_value_type iSP = 0;
		SA_RT_TRIGGER_EXPRESSION(attr_value_type, iSP, entry->_Sp, kParams, LC_Helper::BaseLevelSP(playerlevel), playerlevel);

		pkPlayer->AddCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar);
		pkPlayer->AddExpWithGamePlay(iExp, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar);
		pkPlayer->AddSkillLevelupPoints(iSP, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar);

		pkPlayer->GetPlayerGuildAssert()->DomainReward(resp.m_ResId, it->second.m_TimeStamp);

		resp.m_exp = (uint32_t)iExp;
		resp.m_money = (uint32_t)iCash;
		resp.m_sp = (uint32_t)iSP;

		resp.m_Result = RE_SUCCESS;
		pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
		return;
	}
	else
	{
		MoneyType money = 0;
		attr_value_type exp = 0;
		attr_value_type sp = 0;

		mem::vector<int> allresid;
		mem::vector<int64_t> allresidtimestamp;

		TStringVector kParams;
		kParams.push_back("BASE");
		kParams.push_back("PLAYERLV");

		Utility::UT_SIMDataList kRewards;
		DomainPowerMap::const_iterator beg = thepowermap->begin();
		for (; beg != thepowermap->end(); ++beg)
		{
			if (beg->second.m_GuildId == pkPlayer->GetGuildID() && pkPlayer->GetPlayerGuildAssert()->CheckDomainReward(beg->first, beg->second.m_TimeStamp))
			{
				allresid.push_back(beg->first);
				allresidtimestamp.push_back(beg->second.m_TimeStamp);
				CF_ForcesBattle* forcebattle = CF_ForcesBattle::GetSingletonPtr();
				CF_ForcesBattle::DataEntry* entry = forcebattle->GetEntryPtr(beg->first);
				if (entry == NULL)
				{
					continue;
				}
				kRewards.insert(kRewards.end(), entry->_Rewards.begin(), entry->_Rewards.end());

				int playerlevel = pkPlayer->GetLevel();

				attr_value_type iExp = 0;
				SA_RT_TRIGGER_EXPRESSION(attr_value_type, iExp, entry->_Exp, kParams, LC_Helper::BaseLevelExp(playerlevel), playerlevel);

				MoneyType iCash = 0;
				SA_RT_TRIGGER_EXPRESSION(MoneyType, iCash, entry->_Gold, kParams, LC_Helper::BaseLevelCash(playerlevel), playerlevel);

				attr_value_type iSP = 0;
				SA_RT_TRIGGER_EXPRESSION(attr_value_type, iSP, entry->_Sp, kParams, LC_Helper::BaseLevelSP(playerlevel), playerlevel);

				money += iExp;
				exp += iCash;
				sp += iSP;
			}
		}
		if (!kRewards.empty())
		{
			ResultType ret = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kRewards, pkPlayer, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar, "", "", false);
			if (ret != RE_SUCCESS)
			{
				resp.m_Result = ret;
				pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
				return;
			}

			Utility::UT_SIMDataList::iterator itReward = kRewards.begin();
			int i = 0;
			for (; i<MAX_ITEM_LEN&&itReward!=kRewards.end() ; ++itReward, ++i)
			{
				resp.m_item[i] = itReward->ID();
				resp.m_itemCount[i] = itReward->ItemCount();
			}
			resp.m_itemlen = i;
		}
		pkPlayer->AddCash(money, CT_UNBIND_CASH, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar);
		pkPlayer->AddExpWithGamePlay(exp, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar);
		pkPlayer->AddSkillLevelupPoints(sp, SYS_FUNC_TP_GUILD_WAR, WarType_DomainWar);
		mem::vector<int>::iterator resbeg = allresid.begin();
		for (int j = 0; resbeg != allresid.end(); ++j, ++resbeg)
		{
			pkPlayer->GetPlayerGuildAssert()->DomainReward(*resbeg, allresidtimestamp[j]);
		}
		
		resp.m_exp = (uint32_t)exp;
		resp.m_money = (uint32_t)money;
		resp.m_sp = (uint32_t)sp;
		resp.m_Result = RE_SUCCESS;
		pkPlayer->SendMsgToClient(MGPT_RESP_DOMAINREWARD, &resp);
		return;
	}
}

ON_HANDLE(MGPT_REQ_GLOBALGUILDWARREWARD, this_source_type, false)
{
}

ON_HANDLE(MGPT_REQ_GUILDRAIDREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GuildRaidReward_Req reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_GuildRaidReward_Resp resp;
	resp.m_Result = RE_SUCCESS;
	resp.m_Page = reqMsg.m_Page;
	resp.m_Stage = reqMsg.m_Stage;

	if (pkPlayer->GetGuildID() == INVALID_GUILD_ID)
	{
		resp.m_Result = RE_GUILD_POWER_GUILD_REWARD_NONE;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDREWARD, &resp);
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
	if (guildinfo == NULL)
	{
		resp.m_Result = RE_GUILD_POWER_GUILD_REWARD_NONE;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDREWARD, &resp);
		return;
	}

	resp.m_Result = guildinfo->CheckGuildStage(pkPlayer->GetInstance(), reqMsg.m_Page, reqMsg.m_Stage);
	if (resp.m_Result != RE_SUCCESS)
	{
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDREWARD, &resp);
		return;
	}

	resp.m_Result = pkPlayer->GetPlayerGuildAssert()->CheckRaidStageReward(reqMsg.m_Page, reqMsg.m_Stage);
	if (resp.m_Result != RE_SUCCESS)
	{
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDREWARD, &resp);
		return;
	}

	LC_GuildRaidDataManager* manager = (LC_GuildRaidDataManager*)LC_GuildRaidDataManager::GetSingletonPtr();
	CF_GuildRaid::DataEntry* entry = manager->GetDataEntry(reqMsg.m_Page, reqMsg.m_Stage);
	if (entry == NULL)
	{
		return;
	}
	mem::vector<int> result;
	bool ret = LC_Helper::SpliteString3Int(entry->_kRewards, result);
	if (!ret || result.empty() || (int)result.size() % 3 != 0)
	{
		result.clear();
	}
	//发放奖励
	PackOperateInfoList objOPList;
	objOPList.clear();
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	LC_BackPackEntryList kEntryList;
	int count = (int)result.size() / 3;
	for (int i = 0 ; i < count ; ++i)
	{
		kEntryList.clear();
		int32_t bindMode = (result[i * 3 + 2] == 1);
		pkPackEntryFactory->CreateItemEntry(result[i * 3], result[i * 3 + 1], CIET_NONE, kEntryList, bindMode);
		for (int j = 0 ; j < (int)kEntryList.size() ; ++j)
		{
			PackOperateInfo objData;
			objData.Reset();
			objData.m_iOPType = OPT_ADD_ITEM;
			objData.m_bOPEntry = true;
			objData.m_bMerger  = true;
			objData.m_kPackEntry.Clone(kEntryList[j]);
			objOPList.push_back(objData);
		}

	}

	ResultType iResult = LC_Helper::DoItemOPs(pkPlayer, objOPList, SYS_FUNC_TP_GUILD_RAID, reqMsg.m_Stage);
	if (iResult != RE_SUCCESS)
	{
		resp.m_Result = iResult;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDREWARD, &resp);
		return;
	}
	//经验
	int playerlevel = pkPlayer->GetLevel();

	TStringVector kParams;
	kParams.push_back("BASE");
	kParams.push_back("PLAYERLV");

	attr_value_type iExp = 0;
	SA_RT_TRIGGER_EXPRESSION(attr_value_type, iExp, entry->_Exp, kParams, LC_Helper::BaseLevelExp(playerlevel), playerlevel);

	MoneyType iCash = 0;
	SA_RT_TRIGGER_EXPRESSION(MoneyType, iCash, entry->_Gold, kParams, LC_Helper::BaseLevelCash(playerlevel), playerlevel);

	attr_value_type iSP = 0;
	SA_RT_TRIGGER_EXPRESSION(attr_value_type, iSP, entry->_Sp, kParams, LC_Helper::BaseLevelSP(playerlevel), playerlevel);

	pkPlayer->AddExpWithGamePlay(iExp, SYS_FUNC_TP_GUILD_RAID, reqMsg.m_Stage);

	pkPlayer->AddCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_GUILD_RAID, reqMsg.m_Stage);

	pkPlayer->AddSkillLevelupPoints(iSP, SYS_FUNC_TP_GUILD_RAID, reqMsg.m_Stage);

	pkPlayer->GetPlayerGuildAssert()->RaidStageReward(reqMsg.m_Page, reqMsg.m_Stage);

	pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDREWARD, &resp);
}

ON_HANDLE(MGPT_REQ_GUILDRAIDENTER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_EnterGuildRaid_Resp resp;
	resp.m_Result = RE_SUCCESS;
	if (pkPlayer->GetGuildID() == INVALID_GUILD_ID)
	{
		resp.m_Result = RE_GUILD_NONE;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDENTER, &resp);
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
	if (guildinfo == NULL)
	{
		resp.m_Result = RE_GUILD_NONE;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDENTER, &resp);
		return;
	}
	resp.m_Result = guildinfo->PlayerEnterGuildRaid(pkPlayer);
	pkPlayer->SendMsgToClient(MGPT_RESP_GUILDRAIDENTER, &resp);
	return;
}

ON_HANDLE(MGPT_QUERY_GUILD_RAID_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	if (pkPlayer->GetGuildID() == INVALID_GUILD_ID)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
	if (guildinfo == NULL)
	{
		return;
	}

	LC_ServerGuildBasicInfo& rBaseInfo = guildinfo->GetGuildBasicInfo();

	MG_Notify_GuildRaid msg;
	msg.m_GuildId = guildinfo->GetId();
	msg.m_Page = rBaseInfo.m_GuildRaidPage;
	msg.m_Stage = rBaseInfo.m_GuildRaidStage;
	msg.m_StageTimeStamp = rBaseInfo.m_GuildRaidStageTimeStamp;
	msg.m_RaidStageTimeSpan.insert(msg.m_RaidStageTimeSpan.begin(), rBaseInfo.m_GuildRaidStageSpan.begin(), rBaseInfo.m_GuildRaidStageSpan.end());
	pkPlayer->SendMsgToClient(MGPT_NOTIFY_GUILD_RAID_INFO, &msg);
}


ON_HANDLE(MGPT_REQ_SETAI, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_SetAI_Req reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (!LC_Helper::isInArena(pkPlayer))
	{
		return;
	}
	pkPlayer->SetAIControl(reqMsg.m_SetAI);
}

ON_HANDLE(MGPT_REQ_END_SOLODULE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_REQ_Quit_SoloMap reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	if (!LC_Helper::isInArena(pkPlayer))
		return;
	LC_ServerMapLogic_Place* logicbase = UT_ServerHelper::GetPlayerPlace(pkPlayer);
	if (logicbase == NULL)
		return;
	if(reqMsg.m_nPlayType == PlayType_SoloDule)
	{
		LC_ServerMapLogic_SoloDulePlay* play = (LC_ServerMapLogic_SoloDulePlay*)logicbase->GetPlay(PlayType_SoloDule);
		if (play == NULL)
			return;
		play->End();
	}
	else if(reqMsg.m_nPlayType == PlayType_Sky)
	{
		LC_ServerMapLogic_SkyPlay* play = (LC_ServerMapLogic_SkyPlay*)logicbase->GetPlay(PlayType_Sky);
		if (play == NULL)
			return;
		play->PlayerLevave(reqMsg.m_nOpType);
		if(reqMsg.m_nOpType == QSMOT_None)
			pkPlayer->SetPlayStateEnd(SKY_SOLO_DULE_RES_ID);
	}
	else if(reqMsg.m_nPlayType == PlayType_SummitTeam)
	{
		LC_ServerMapLogic_SummitTeamPlay* play = (LC_ServerMapLogic_SummitTeamPlay*)logicbase->GetPlay(PlayType_SummitTeam);
		if (play == NULL)
			return;
		play->PlayerLevave(reqMsg.m_nOpType);
		if(reqMsg.m_nOpType == QSMOT_None)
			pkPlayer->SetPlayStateEnd(SUMMIT_TEAM_SOLO_DULE_RES_ID);
	}
}

ON_HANDLE(MGPT_REQ_PLAYER_DETAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetPlayerInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetPlayerDetailInfo(reqMsg.m_CharID, reqMsg.m_SystemType);
}

ON_HANDLE(MGPT_REQ_PLAYER_PK_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_REQ_PlayerPkInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	if (0 == reqMsg.m_CharID)
		return;
	pkPlayer->GetOtherOfflineInfo( OfflinePlayerFromDBType::OPFDT_PK,reqMsg.m_CharID);
	
}


ON_HANDLE(MGPT_REQ_CONTROLLER_AND_CORE_COMBAT_SCORE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	
	MG_REQ_ControllerAndCoreCombatScore reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	uint32_t uBuddyId = reqMsg.m_nBuddyID;
	if (0 == uBuddyId)
		return;
	BuddyInstance* pBuddy = pkPlayer->GetBuddyInstanceByID(uBuddyId);
	if ( pBuddy == NULL )
		return;
	// 查找相关buddy 信息 
	MG_RLT_ControllerAndCoreCombatScore rltMsg;
	rltMsg.m_nBuddyID = reqMsg.m_nBuddyID;
	rltMsg.m_nControllerCombatScore = pkPlayer->GetControllerCombatScore();

	rltMsg.m_nCoreCombatScore = pkPlayer->GetAllBuddyCoreScoreExceptInputID(uBuddyId);
	pkPlayer->SendMsgToClient(MGPT_RLT_CONTROLLER_AND_CORE_COMBAT_SCORE, &rltMsg);
}
//
ON_HANDLE(MGPT_GREAT_KUNGFU_OPERATION_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GreatKungFuOperationREQ reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_GreatKungFuOperationRESP resp;
	resp.m_uiID = reqMsg.m_uiID;
	resp.m_uiOPCode = reqMsg.m_uiOPCode;
	resp.m_uiOPPara = reqMsg.m_uiOPPara;
	switch (reqMsg.m_uiOPCode)
	{
		case GKF_OPT_REALIZE:
			{
				resp.m_uiOPRslt = pkPlayer->TryRealizeGreatKungFu(reqMsg.m_uiID);
			}
			break;
		default:
			resp.m_uiOPRslt = RE_FAIL;
			break;
	}
	pkPlayer->SendMsgToClient(MGPT_GREAT_KUNGFU_OPERATION_RESP, &resp);
}

ON_HANDLE(MGPT_REQ_PLAYERRAIDSETTLE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	if (!LC_Helper::isInTranscript(pkPlayer))
	{
		return;
	}
	LC_ServerMapLogic_Place* logicbase = UT_ServerHelper::GetPlayerPlace(pkPlayer);
	if (logicbase != NULL)
	{
		LC_ServerMapLogic_BatchPlay* play = (LC_ServerMapLogic_BatchPlay*)logicbase->GetPlay(PlayType_Batch);
		if (play != NULL)
		{
			play->PlayerSettle(pkPlayer->GetID());
		}
	}
}

ON_HANDLE(MGPT_REQ_OPEN_ABILITY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_ReqOpenAbility reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	MG_RespOpenAbility resp;
	resp.m_iID = reqMsg.m_iID;
	resp.m_Result = pkPlayer->OpenPetAbility(reqMsg.m_iID, reqMsg.m_nParam);
	pkPlayer->SendMsgToClient(MGPT_RESP_OPEN_ABILITY, &resp);
}

ON_HANDLE(MGPT_TREASURE_CHESTS_LOGS_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_TreasureChestsLogsReq reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_TreasureChestsManager* pkTreasureChestsManager = LC_TreasureChestsManager::GetSingletonPtr();
	if (NULL == pkTreasureChestsManager)
	{
		return;
	}
	MG_TreasureChestsLogsNotify notify;
	pkTreasureChestsManager->SetDataToProtocolStruct(notify);
	pkPlayer->SendMsgToClient(MGPT_TREASURE_CHESTS_LOGSNOTIFY, &notify);
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_PAIR_SITRECOVER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Common reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	int64_t value = 0;
	int ret = reqMsg.get("play_id", value);
	if (!ret)
	{
		return;
	}
	static const int64_t k_play_id = boost::strval::str_to_val("play_id");
	static const int64_t k_other_id = boost::strval::str_to_val("other_id");
	object_id_type lPlayerID = get_value(reqMsg, k_play_id, 0);
	object_id_type lOtherID = get_value(reqMsg, k_other_id, 0);
	if (!(lPlayerID && lOtherID))
	{
		return;
	}
	if (lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	LC_ServerPlayer* pkOtherPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(lOtherID);
	ResultType iResult = RE_FAIL;
	if (NULL != pkOtherPlayer)
	{
		LC_MapBase* my_map = pkPlayer->GetMap();
		LC_MapBase* other_map = pkOtherPlayer->GetMap();
		if (my_map != NULL && my_map == other_map)
		{
			LC_ServerWorldManager* swm = SERVER_GET_WORLD_MANAGER();
			//是城镇，或者是幽林密境
			if (swm->IsTownMap(my_map->GetMapResID()) || my_map->GetMapResID() == 2101)	 //2101是幽林秘境的ID，策划要求在幽林秘境能双休,modify by lxm，
			{
				object_id_type my_psrid = pkPlayer->GetPairSitRecoverID();
				object_id_type other_psrid = pkOtherPlayer->GetPairSitRecoverID();
				if ((my_psrid == 0 && other_psrid == 0) || pkPlayer->GetID() == other_psrid)
				{
					int pdistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pkOtherPlayer->GetCurrentLocation());
					if (pdistance < 3000)
					{
						float now = GET_PROCESS_TIME;
						pkPlayer->SetPairSitRecoverID(lOtherID);
						pkPlayer->SetStartMatchPairSitRecoverTime(now);
						pkOtherPlayer->SetPairSitRecoverID(lPlayerID);
						pkOtherPlayer->SetStartMatchPairSitRecoverTime(now);
						iResult = RE_SUCCESS;
					}
					else
					{
						iResult = RE_MATCH_PAIR_SITRECOVER_TOO_FAR;
					}
				}
				else
				{
					if (my_psrid != 0)
					{
						iResult = RE_MATCH_PAIR_SITRECOVER_BUSY_ON_YOUR;
					}
					else
					{
						iResult = RE_MATCH_PAIR_SITRECOVER_BUSY_ON_OTHER;
					}
				}
			}
		}
	}

	MG_Common sucMsg;
	sucMsg.set("result", iResult);
	sucMsg.set("other_id", lOtherID);
	pkPlayer->SendMsgToClient(MGPT_RLT_PAIR_SITRECOVER, &sucMsg);
}

ON_HANDLE(MGPT_COMMON_STAT_BASIC_OPERATION_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_PB<msg_define::common_stat_basic_operation_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_CommonStatManager* pkMgr = LC_CommonStatManager::GetSingletonPtr();
	if (NULL == pkMgr)
	{
		return;
	}

	msg_define::common_stat_basic_operation_req& req_val = reqMsg.m_value;
	MG_PB<msg_define::common_stat_basic_operation_resp> respMsg;
	msg_define::common_stat_basic_operation_resp& resp_val = respMsg.m_value;
	resp_val.set_type(req_val.type());
	resp_val.set_op_code(req_val.op_code());
	resp_val.set_op_para(req_val.op_para());

	LC_CommonStatAsset& kCmnStatAsset = pkPlayer->GetCommonStatAsset();
	uint32_t rslt = RE_FAIL;

	switch (req_val.op_code())
	{
		case LC_CMN_STAT_OP_BASIC_REWARD:
			{
				int32_t iStatType = req_val.type();
				int32_t iRewardType = req_val.op_para();
				LC_CommonStatBasicRewardEntry* pkRewardEntry = pkMgr->GetCmnBasicRewardEntry(iStatType, iRewardType);
				if (pkRewardEntry == NULL || pkRewardEntry->m_iServerForce)
				{
					break;
				}
				if(pkRewardEntry->m_iPlatformType > 0 && !pkPlayer->GetPlayerPlatformInfoAsset().CheckPlatformTypeMatch(pkRewardEntry->m_iPlatformType))
				{
					break;
				}
				LC_CommonStatBasic* pkBasic = kCmnStatAsset.GetCommonStatBasic(iStatType);
				if (NULL == pkBasic)
				{
					break;
				}
				if (pkBasic->IsFlagSet(iRewardType))
				{
					rslt = RE_COMMON_STAT_REWARDED;
					break;
				}
				if (pkRewardEntry->m_bForceMail)
				{
					rslt = UT_ServerHelper::SendToMail(pkRewardEntry->m_sunOnMailTitle, pkRewardEntry->m_sunOnMailDesc, pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName(), 0, pkRewardEntry->m_kRewardItems);
				}
				else
				{
					rslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkRewardEntry->m_kRewardItems, pkPlayer, SYS_FUNC_TP_CMN_STAT_BASIC, req_val.op_code(), pkRewardEntry->m_sunOnMailTitle, pkRewardEntry->m_sunOnMailDesc, pkRewardEntry->m_bForceMail);
				}
				if (RE_SUCCESS != rslt)
				{
					break;
				}
				pkBasic->SetFlag(iRewardType, true);
				kCmnStatAsset.UpdateDirtyFlag();
				rslt = RE_SUCCESS;
			}
			break;
		case LC_CMN_STAT_OP_PROFIT_QUERY:
			{
				LC_CommonStatBasic* pkBasic = kCmnStatAsset.GetCommonStatBasic(LC_ST_TP_DAILY);
				if (NULL == pkBasic)
				{
					break;
				}
				
				MG_PB<char_data_define::common_stat_profit> notifyMsg;
				pkBasic->SetDataToPB(notifyMsg.m_value);
				pkPlayer->SendMsgToClient(MGPT_COMMON_STAT_BASIC_PROFIT_NOTIFY, &notifyMsg);

				rslt = RE_SUCCESS;
			}
			break;
		default:
			break;
	}
	resp_val.set_result(rslt);
	pkPlayer->SendMsgToClient(MGPT_COMMON_STAT_BASIC_OPERATION_RESP, &respMsg);
};

ON_HANDLE(MGPT_COMMON_STAT_OPERATION_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;

	}
	MG_PB<msg_define::common_stat_operation_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_CommonStatManager* pkMgr = LC_CommonStatManager::GetSingletonPtr();
	if (NULL == pkMgr)
	{
		return;
	}
	msg_define::common_stat_operation_req& req_val = reqMsg.m_value;
	MG_PB<msg_define::common_stat_operation_resp> respMsg;
	msg_define::common_stat_operation_resp& resp_val = respMsg.m_value;
	int nOPCode = req_val.op_code();
	int nOPType = req_val.type();
	int nOPParam = req_val.op_para();
	resp_val.set_type(nOPType);
	resp_val.set_op_code(nOPCode);
	resp_val.set_op_para(nOPParam);

	LC_CommonStatAsset& kCmnStatAsset = pkPlayer->GetCommonStatAsset();
	LC_CommonStatConfigDataEntry* pkConfig = pkMgr->GetCmnConfigDataEntry(req_val.type());
	if (NULL == pkConfig)
	{
		return;
	}
	if(pkConfig->m_iPlatformType > 0 && !pkPlayer->GetPlayerPlatformInfoAsset().CheckPlatformTypeMatch(pkConfig->m_iPlatformType))
	{
		return;
	}
	if (!pkConfig->IsServerValid(GetGameUnionID(), GetGameServerID()))
	{
		return;
	}
	LC_CommonStatEntry* pkCmnStatEntry = kCmnStatAsset.GetCommonStatEntry(req_val.type());
	LC_CommonStatRewardDataEntry* pkCSRewardEntry = NULL;
	LC_CommonStatReplevyDataEntry* pkCSReplevyEntry = NULL;
	uint32_t rslt = RE_FAIL;
	if (pkCmnStatEntry)
	{
		int32_t iStoryID = pkConfig->m_iStoryID;
		//common?
		if (pkConfig->IsFreeOpen() && pkConfig->CheckOpenPlayerLevel(pkPlayer->GetLevel()))
		{
			pkCmnStatEntry->SetEnabled(true);
		}
		switch (resp_val.op_code())
		{
			case LC_CMN_STAT_OP_OPEN:
				{
					if (pkCmnStatEntry->IsEnabled())
					{
						rslt = RE_FAIL;
						break;
					}
					if (!pkConfig->CheckOpenPlayerLevel(pkPlayer->GetLevel()))
					{
						rslt = RE_COMMON_STAT_LEVEL_LACK;
						break;
					}
					LC_ShopCostRecord kShopCost;
					if (!pkConfig->IsFreeOpen())
					{
						const LC_ShopIDMap* pkShopMap = pkConfig->GetOpenShopIDs(req_val.op_para());
						if (NULL == pkShopMap)
						{
							rslt = RE_FAIL;
							break;
						}
						rslt = LC_Helper::IsShopAvailable(pkPlayer, *pkShopMap);
						if (RE_SUCCESS != rslt)
						{
							break;
						}
						int nLogCode = 0;
						if(req_val.type() == LC_CMN_STAT_TP_MONTHLY_CARD)
						{
							nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY13_OPERATE, OPERATE_LOGTYPE1);
						}
						else
						{
							nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY14_OPERATE, OPERATE_LOGTYPE1);
						}
						LC_Helper::PayForShop(pkPlayer, *pkShopMap, SYS_FUNC_TP_CMN_STAT_OPEN, req_val.type(), &kShopCost, nLogCode);
					}
					switch (req_val.type())
					{
						case LC_CMN_STAT_TP_MONTHLY_CARD:
							{
								pkCmnStatEntry->Clear(GET_CURRENT_TIMESTAMP_IN_SECONDS());
							}
							break;
						default:
							break;
					}
					pkCmnStatEntry->SetEnabled(true);
					pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_OPEN_COST, (uint32_t)kShopCost.Cash(CT_UNBIND_YUANBAO));
					kCmnStatAsset.UpdateDirtyFlag();
					rslt = RE_SUCCESS;
				}
				break;
			default:
				{
					//detail?
					int32_t nSecHelp = 0;
					if (!pkCmnStatEntry->IsEnabled())
					{
						rslt = RE_FAIL;
						break;
					}
					switch (req_val.type())
					{
					case LC_CMN_STAT_TP_WHILT_VIP_FREE_REWARD:
						{
							if(kCmnStatAsset.IsRewardReg(LC_CMN_STAT_TP_WHILT_VIP_FREE_REWARD,req_val.op_para(),LC_ST_TP_DAILY))
							{
								rslt = RE_COMMON_STAT_REWARDED;
								break;
							}
							pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), req_val.op_para());
							if (NULL == pkCSRewardEntry)
							{
								rslt = RE_COMMON_STAT_BAD_KEY;
								break;
							}

							uint32_t res = LC_Helper::GetVipPrivilegeTimesByType(VIP_PC_FREE_REWARD_WHITE_ITEM, pkPlayer);
							if(res > 0)
							{
								//do the reward
								CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(), req_val.op_para(), pkConfig, pkCSRewardEntry, pkCSReplevyEntry, (1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
								opArg.nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY15_OPERATE, OPERATE_LOGTYPE2);
								opArg();
								rslt = opArg.m_uiRslt;								
							}
							break;
						}
					case LC_CMN_STAT_TP_GOLD_VIP_FREE_REWARD:
						{
							if(kCmnStatAsset.IsRewardReg(LC_CMN_STAT_TP_GOLD_VIP_FREE_REWARD,req_val.op_para(),LC_ST_TP_DAILY))
							{
								rslt = RE_COMMON_STAT_REWARDED;
								break;
							}
							pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), req_val.op_para());
							if (NULL == pkCSRewardEntry)
							{
								rslt = RE_COMMON_STAT_BAD_KEY;
								break;
							}

							uint32_t res = LC_Helper::GetVipPrivilegeTimesByType(VIP_PC_FREE_REWARD_GOLD_ITEM, pkPlayer);
							if(res > 0)
							{
								//do the reward
								CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(), req_val.op_para(), pkConfig, pkCSRewardEntry, pkCSReplevyEntry, (1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
								opArg.nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY16_OPERATE, OPERATE_LOGTYPE2);
								opArg();
								rslt = opArg.m_uiRslt;
							}
							break;
						}
						case LC_CMN_STAT_TP_LOGIN:
							{
								switch (req_val.op_code())
								{
									case LC_CMN_STAT_OP_DRAW:
										{
											if (req_val.op_para() > (int32_t)kCmnStatAsset.GetAccuCntAll(LC_CMN_STAT_TP_LOGIN))
											{
												rslt = RE_COMMON_STAT_LOGIN_DAYS_LACK;
												break;
											}
											if (kCmnStatAsset.IsRewardReg(LC_CMN_STAT_TP_LOGIN,req_val.op_para(),LC_ST_TP_CMN_END))
											{
												rslt = RE_COMMON_STAT_LOGIN_REWARDED;
												break;
											}

											if(req_val.type()==0 && req_val.op_para()==1)
											{
												uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
												uint32_t time = pkPlayer->GetFirstDayLoginRewardTime();
												if(currentTime < time)
												{
													rslt = RE_COMMON_STAT_LOGIN_DAYS_LACK;
													break;
												}												
											}

											pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), req_val.op_para());
											if (NULL == pkCSRewardEntry)
											{
												rslt = RE_COMMON_STAT_LOGIN_BAD_KEY;
												break;
											}
											//do the reward
											CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(), req_val.op_para(), pkConfig, pkCSRewardEntry, pkCSReplevyEntry, (1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
											opArg.nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY1_OPERATE, OPERATE_LOGTYPE1);
											opArg.nLogParams.clear();
											opArg.nLogParams.push_back(req_val.op_para());
											opArg();
											if (RE_SUCCESS != opArg.m_uiRslt)
											{
												rslt = opArg.m_uiRslt;
												break;
											}
											rslt = RE_SUCCESS;
										}
										break;
									default:
										break;
								}
							}
							break;
						case LC_CMN_STAT_TP_SHUNWANG_LOGIN_PLATFORM:
							{
								rslt = pkPlayer->GetReward_ShunWangPlatform(req_val.type(), req_val.op_para());
							}
							break;
						case LC_CMN_STAT_TP_SHUNWANG_BOXGIFT:
							{
								rslt = pkPlayer->GetReward_ShunWangLoginReward(req_val.type(), req_val.op_para());
							}
							break;
						case LC_CMN_STAT_TP_SHUNWANG_LEVEL:
							{
								rslt = pkPlayer->GetReward_ShunWangLevel(req_val.type(), req_val.op_para());
							}
							break;
						case LC_CMN_STAT_TP_SHUNWANG_ANTI_WALLOW:
							{
								rslt = pkPlayer->GetReward_AntiWallowReward(req_val.type(), req_val.op_para());
							}
							break;
						case LC_CMN_STAT_TP_DAILY_ONLINE:
							{
								LC_CommonStatBasic* pkCommonStatBasic = kCmnStatAsset.GetCommonStatBasic(LC_ST_TP_DAILY);
								uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
								switch (req_val.op_code())
								{
									case LC_CMN_STAT_OP_DRAW:
										{
											if (pkConfig->m_iStoryID)
											{
												if (!SERVER_GET_GAMESTORY_MANAGER->CheckIsActiveStory(pkConfig->m_iStoryID))
												{
													rslt = RE_GAMESTORY_UNACTIVE;
													break;
												}
											}
											uint32_t uiOnlineTime = pkCommonStatBasic->GetOnlineTime(timestamp);
											if (req_val.op_para() > (int32_t)uiOnlineTime)
											{
												rslt = RE_COMMON_STAT_ONLINE_LACK;
												break;
											}
											pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), req_val.op_para());
											if (NULL == pkCSRewardEntry)
											{
												rslt = RE_COMMON_STAT_BAD_KEY;
												break;
											}
											if (pkCmnStatEntry->IsRewardReg(LC_ST_TP_DAILY, pkCSRewardEntry->m_iSaveIndex))
											{
												rslt = RE_COMMON_STAT_REWARDED;
												break;
											}
											//do the reward
											int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ONLINE_OPERATE, OPERATE_LOGTYPE1);
											CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(), req_val.op_para(), pkConfig, pkCSRewardEntry, pkCSReplevyEntry, (1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
											opArg.nLogCode = nLogCode;
											opArg.nLogParams.clear();
											opArg.nLogParams.push_back(req_val.op_para());
											opArg();
											if (RE_SUCCESS != opArg.m_uiRslt)
											{
												rslt = opArg.m_uiRslt;
												break;
											}
											rslt = RE_SUCCESS;
										}
										break;
									default:
										break;
								}
							}
							break;
						case LC_CMN_STAT_TP_MONTHLY_CARD:
							{
								switch (req_val.op_code())
								{
									case LC_CMN_STAT_OP_DRAW:
										{
											bool isClear = false;
											int32_t iCurKey = pkCmnStatEntry->GetAccumulateCntAll();
											for(int key = 0; key<=iCurKey; ++key)
											{
												if (pkConfig->m_iMaxCnt > 0 && key + 1 >= pkConfig->m_iMaxCnt)
												{
													key = pkConfig->m_iMaxCnt - 1;
													isClear  = true;
												}
											
												pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), key);
												if (NULL == pkCSRewardEntry)
												{
													rslt = RE_COMMON_STAT_BAD_KEY;
													break;
												}

												if (pkCmnStatEntry->IsRewardReg(LC_ST_TP_FORERVER, pkCSRewardEntry->m_iSaveIndex))
												{
													continue;
												}
												int nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY13_OPERATE, OPERATE_LOGTYPE2);
												CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(), key, pkConfig, pkCSRewardEntry, pkCSReplevyEntry, (1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
												opArg.nLogCode = nLogCode;
												opArg.nLogParams.clear();
												opArg.nLogParams.push_back(iCurKey+1);
												opArg();
												if (RE_SUCCESS != opArg.m_uiRslt)
												{
													rslt = opArg.m_uiRslt;
													break;
												}
												else
												{
													rslt = RE_SUCCESS;
												}
											}
											if (isClear)
											{
												pkCmnStatEntry->SetEnabled(false);
											}
										}
										break;
									default:
										break;
								}
							}
							break;
						case LC_CMN_STAT_TP_RECHARGE_PROFIT:
							{
								if (pkConfig->m_iStoryID)
								{
									if (!SERVER_GET_GAMESTORY_MANAGER->CheckIsActiveStory(pkConfig->m_iStoryID))
									{
										rslt = RE_GAMESTORY_UNACTIVE;
										break;
									}

									if ((int32_t)pkCmnStatEntry->GetAccumulateStoryCount() < resp_val.op_para())
									{
										rslt = RE_COMMON_STAT_BAD_KEY;
										break;
									}

									pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), resp_val.op_para());
									if (NULL == pkCSRewardEntry)
									{
										rslt = RE_COMMON_STAT_BAD_KEY;
										break;
									}

									if (pkCmnStatEntry->IsStoryRewardReg(pkCSRewardEntry->m_iSaveIndex))
									{
										rslt = RE_COMMON_STAT_REWARDED;
										break;
									}
								}
								else
								{
									if ((int32_t)pkCmnStatEntry->GetAccumulateCntAll() < resp_val.op_para())
									{
										rslt = RE_COMMON_STAT_BAD_KEY;
										break;
									}
									pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), resp_val.op_para());
									if (NULL == pkCSRewardEntry)
									{
										rslt = RE_COMMON_STAT_BAD_KEY;
										break;
									}

									if (pkCmnStatEntry->IsRewardReg(LC_ST_TP_FORERVER, pkCSRewardEntry->m_iSaveIndex))
									{
										rslt = RE_COMMON_STAT_REWARDED;
										break;
									}
								}

								CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(), req_val.op_para(), pkConfig, pkCSRewardEntry, pkCSReplevyEntry, (1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
								opArg();
								if (RE_SUCCESS != opArg.m_uiRslt)
								{
									rslt = opArg.m_uiRslt;
									break;
								}
								rslt = RE_SUCCESS;
							}
							break;
						case LC_CMN_STAT_TP_ENDLESS_TOWER_1:
						case LC_CMN_STAT_TP_ENDLESS_TOWER_2:
						case LC_CMN_STAT_TP_ENDLESS_TOWER_3:
						case LC_CMN_STAT_TP_ENDLESS_TOWER_4:
						case LC_CMN_STAT_TP_ENDLESS_TOWER_5:
						case LC_CMN_STAT_TP_ENDLESS_TOWER_6:
							{
								switch (req_val.op_code())
								{
								case LC_CMN_STAT_OP_DRAW:
									{
										if (req_val.op_para() > (int32_t)pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_MAX_BATCH))
										{
											rslt = RE_COMMON_STAT_ULTIMATE_BAD_BATCH;
											break;
										}
										pkCSRewardEntry = pkMgr->GetCmnRewardDataEntry(req_val.type(), req_val.op_para());
										if (NULL == pkCSRewardEntry)
										{
											rslt = RE_COMMON_STAT_ULTIMATE_BAD_BATCH;
											break;
										}
										if (pkCmnStatEntry->IsRewardReg(LC_ST_TP_CMN_END, pkCSRewardEntry->m_iSaveIndex))
										{
											rslt = RE_COMMON_STAT_ULTIMATE_BATCH_REWARED;
											break;
										}
										//do the reward
										CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(), req_val.op_para(), pkConfig, pkCSRewardEntry, pkCSReplevyEntry, (1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
										opArg.nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN1_OPERATE, OPERATE_LOGTYPE2);
										opArg.nLogParams.clear();
										opArg.nLogParams.push_back(nOPParam);
										opArg();
										if (RE_SUCCESS != opArg.m_uiRslt)
										{
											rslt = opArg.m_uiRslt;
											break;
										}
										rslt = RE_SUCCESS;
									}
								}
							}
							break;
						default:
							break;
					}
				}
				break;
		}
	}
	resp_val.set_result(rslt);
	if (RE_SUCCESS == rslt)
	{
		//do event
	}
	pkPlayer->SendMsgToClient(MGPT_COMMON_STAT_OPERATION_RESP, &respMsg);
}

ON_HANDLE(MGPT_GAMESTORY_KEY_ECOUNTED_STAT_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_REQ reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	GameStory::LC_GameStory_Manager* pkStoryManager = SERVER_GET_GAMESTORY_MANAGER;
	if (NULL == pkStoryManager)
	{
		return;
	}
	//pkStoryManager->NotifyActorEncountedStat(pkPlayer, reqMsg.m_iStroyID, reqMsg.m_uiID);
	pkStoryManager->NotifyActorEncountedStatWithoutStoryID(pkPlayer, reqMsg.m_uiID);
}

ON_HANDLE(MGPT_GAMESTORY_ENTER_CROSS_MAP_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GAMESTORY_ENTER_CROSS_MAP_REQ reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	GameStory::LC_GameStory_Manager* pkStoryManager = SERVER_GET_GAMESTORY_MANAGER;
	if (NULL == pkStoryManager)
	{
		return;
	}
	MG_GAMESTORY_ENTER_CROSS_MAP_RESP respMsg;
	respMsg.m_iStoryID = reqMsg.m_iStoryID;
	respMsg.m_iMapResID = reqMsg.m_iMapResID;


	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (pkArenaMgr->IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "gamestory enter cross failed! in arena sign!");
		respMsg.m_uiRslt = RE_ARENA_IN_SIGN;
	}
	else
	{
		respMsg.m_uiRslt = pkStoryManager->ReqEnterCrossMap(pkPlayer, reqMsg.m_iStoryID, reqMsg.m_iMapResID);
	}
	pkPlayer->SendMsgToClient(MGPT_GAMESTORY_ENTER_CROSS_MAP_RESP, &respMsg);
}

ON_HANDLE(MGPT_GAMESTORY_OPERATION_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GAMESTORY_OPERATION_REQ reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	GameStory::LC_GameStory_Manager* pkStoryManager = SERVER_GET_GAMESTORY_MANAGER;
	if (NULL == pkStoryManager)
	{
		return;
	}
	MG_GAMESTORY_OPERATION_RESP respMsg;
	respMsg.m_nBuyItem = reqMsg.m_nBuyItem;
	respMsg.m_iStoryID = reqMsg.m_iStoryID;
	respMsg.m_iOPCode = reqMsg.m_iOPCode;
	size_t sz = sizeof(respMsg.m_kOPPara);
	::memcpy(respMsg.m_kOPPara, reqMsg.m_kOPPara, sz);
	switch (reqMsg.m_iOPCode)
	{
		case GS_OP_ENTER_CROSS_MAP:
			{
				respMsg.m_uiRslt = pkStoryManager->ReqEnterCrossMap(pkPlayer, reqMsg.m_iStoryID, reqMsg.m_kOPPara[0]);
			}
			break;
		case GS_OP_TASK_WAVER:
			{
				respMsg.m_uiRslt = pkStoryManager->ReqTaskWaver(pkPlayer, reqMsg.m_iStoryID, reqMsg.m_nBuyItem);
			}
			break;
		case GS_OP_SHOPPING:
			{
				respMsg.m_uiRslt = pkStoryManager->ReqShopping(pkPlayer, reqMsg.m_iStoryID, reqMsg.m_kOPPara[0]);
			}
			break;
		case GS_OP_TRANSFER:
			{
				respMsg.m_uiRslt = pkStoryManager->ReqTransfer(pkPlayer, reqMsg.m_iStoryID, reqMsg.m_kOPPara[0]);
			}
			break;
		case GS_OP_TASK_APPOINT:
			{
				respMsg.m_uiRslt = pkStoryManager->ReqTaskAppoint(pkPlayer, reqMsg.m_iStoryID, reqMsg.m_kOPPara[0]);
			}
			break;
		case GS_OP_SNAP_TOP:
			{
				GameStory::RankID kRankID(reqMsg.m_kOPPara[0], reqMsg.m_kOPPara[1]);
				const GameStory::GSRTRankEntry* pkRTRankEntry = pkStoryManager->QueryRankSnap(reqMsg.m_iStoryID, kRankID);
				if (NULL==pkRTRankEntry || pkRTRankEntry->kIndices.empty())
				{
					respMsg.m_uiRslt = RE_FAIL;
				}
				else
				{
					const GameStory::GSRTPlayerCommonStat* pkPlayerStat = pkRTRankEntry->kIndices.front();
					if (pkPlayerStat)
					{
						respMsg.m_strRslt = pkPlayerStat->sName.c_str();
					}
				}
			}
			break;
		default:
			break;
	}
	pkPlayer->SendMsgToClient(MGPT_GAMESTORY_OPERATION_RESP, &respMsg);
}

ON_HANDLE(MGPT_GAMESTORY_RELATE_TIME_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GAMESTORY_RELATE_TIME_REQ reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	GameStory::LC_GameStory_Manager* pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	if (NULL == pkGSMgr)
	{
		return;
	}
	MG_GAMESTORY_RELATE_TIME_RESP respMsg;
	respMsg.m_iStoryID = reqMsg.m_iStoryID;
	respMsg.m_iRealTime = pkGSMgr->GetRelateTime(reqMsg.m_iStoryID);
	pkPlayer->SendMsgToClient(MGPT_GAMESTORY_RELATE_TIME_RESP, &respMsg);
}

ON_HANDLE(MGPT_GAMESTORY_FACTION_DEFENDER_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

    MG_GAMESTORY_FACTION_DEFENDER_RESP respMsg;
    respMsg.m_iDefender = UT_ServerHelper::GetFactionDefServer();
	pkPlayer->SendMsgToClient(MGPT_GAMESTORY_FACTION_DEFENDER_RESP, &respMsg);
}

ON_HANDLE(MGPT_REQ_ACTOR_INFO, this_source_type, false)
{
	return;

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_PB<msg_define::actor_info_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(reqMsg.m_value.npc_id());
	if (NULL == pkActor)
	{
		return;
	}

	MG_PB<msg_define::actor_info_resp> respMsg;
	respMsg.m_value.set_npc_id(pkActor->GetID());
	respMsg.m_value.set_npc_name(pkActor->GetOwnerCharName().c_str());
	respMsg.m_value.set_map_logicid(pkActor->GetMapLogicID());
	respMsg.m_value.set_map_resid(pkActor->GetMapResID());

	msg_define::int_vector2* pkpos = respMsg.m_value.mutable_position();
	pkpos->set_x(pkActor->GetCurrentLocation().x);
	pkpos->set_y(pkActor->GetCurrentLocation().y);

	msg_define::actor_property* pkproperty = respMsg.m_value.mutable_property();
	pkproperty->set_max_hp(pkActor->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
	pkproperty->set_max_hp(pkActor->GetHP());
	pkPlayer->SendMsgToClient(MGPT_RESP_ACTOR_INFO, &respMsg);
}
ON_HANDLE(MGPT_REQ_USE_ACTIVATION_CODE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_PB<msg_define::use_activation_code_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActivationCodeAsset& kActivationCodeAsset = pkPlayer->GetActivationCodeAsset();
	StringType sFinalCode = reqMsg.m_value.code().c_str();
	int type = reqMsg.m_value.type();

	MG_PB<msg_define::use_activation_code_resp> respMsg;
	respMsg.m_value.set_type(type);
	respMsg.m_value.set_code(sFinalCode.c_str());
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	ResultType rslt = RE_SUCCESS;
	do
	{
		//去掉激活码cd时间
		//if (kActivationCodeAsset.IsOPCoolingDown(timestamp))
		//{
		//	rslt = RE_ACTIVATION_CODE_OP_CD;
		//	break;
		//}
		kActivationCodeAsset.RegOPTimestamp(timestamp);
		const LC_ActivationCodeEntry* pkCodeEntry = kActivationCodeAsset.GetCodeEntry(sFinalCode);
		if (pkCodeEntry && pkCodeEntry->IsValid())
		{
			rslt = RE_ACTIVATION_CODE_USED;
			break;
		}

		const std::string& sRedisPrefix = GetGameServerApp()->GetRedisGlobalPrefix();
		//const std::string& sRedisServerPrefix = GetGameServerApp()->GetRedisServerPredix();
		const std::string& sRedisServerPrefix = GetGameServerApp()->GetRedisEntryServerPredix();
		Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetGlobalRedisHandler();
		//if (NULL == pkRedisHandler)
		//{
		//	rslt = RE_FAIL;
		//	break;
		//}

		//check code valid
		redis_define::activation_code_define kcodedefine;
		Utility::ActivationCodeEntry kCode;
		CF_ActivationCodeSpecial::DataEntry* pkSpecialCode = NULL;
		bool bSpecialCode = IsSpecialCode(sFinalCode, pkSpecialCode);
		bSpecialCode = (NULL == pkSpecialCode) ? false : true;
		if (bSpecialCode)
		{
			if (pkSpecialCode->_iStartTime > 0 && (int64_t)timestamp < pkSpecialCode->_iStartTime)
			{
				rslt = RE_ACTIVATION_CODE_NOTIN_TIMEZONE;
				break;
			}
			if (pkSpecialCode->_iEndTime > 0 && (int64_t)timestamp > pkSpecialCode->_iEndTime)
			{
				rslt = RE_ACTIVATION_CODE_NOTIN_TIMEZONE;
				break;
			}
		}
		else
		{
			// input: sFinalCode.c_str();
			// output: kCode
			if (!Utility::ParseActivationCode(pkRedisHandler, sFinalCode.c_str(), kCode))
			{
				rslt = RE_ACTIVATION_CODE_UNVALID;
				break;
			}
			redispp::StringReply kCodeDefineReply;
			UT_REDIS_CMD_WITH_REPLY(rslt, pkRedisHandler, kCodeDefineReply, hget, sRedisPrefix + UT_REDIS_KEY_CODE_DEFINES, boost::lexical_cast<std::string>(kCode.iCodeID));
			if (rslt != 0 || !kCodeDefineReply.result())
			{
				rslt = RE_ACTIVATION_CODE_UNVALID;
				break;
			}

			if (!kcodedefine.ParseFromString(kCodeDefineReply))
			{
				rslt = RE_ACTIVATION_CODE_UNVALID;
				break;
			}
			if (kcodedefine.length() > 0)
			{
				if (kcodedefine.length() != (int32_t)sFinalCode.size())
				{
					rslt = RE_ACTIVATION_CODE_UNVALID;
					break;
				}
				sFinalCode = sFinalCode.substr(0, kcodedefine.length());
			}
			if (kcodedefine.max_serial() && kCode.iCodeSerial >= kcodedefine.max_serial())
			{
				rslt = RE_ACTIVATION_CODE_UNVALID;
				break;
			}
		}

		//check use limit
		int32_t iCodeType = 0, iUseLimit = 0;
		bool bGlobal = 0;
		if (bSpecialCode)
		{
			iCodeType = pkSpecialCode->_iType;
			iUseLimit = pkSpecialCode->_iUseLimit;
			bGlobal = pkSpecialCode->_bGlobal;
		}
		else
		{
			iCodeType = kcodedefine.type();
			iUseLimit = kcodedefine.use_limit();
			bGlobal = kcodedefine.has_global() ? kcodedefine.global() == 1 : false;
		}

		if(-1 != type && iCodeType !=type)
		{
			rslt = RE_ACTIVATION_CODE_ERROR_1;
			break;
		}

		CF_ActivationCode::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_ActivationCode>(iCodeType);
		rslt = kActivationCodeAsset.CheckCanUse(sFinalCode, iCodeType);
		if (RE_SUCCESS != rslt)
		{
			break;
		}
		int iCodeUsedTimes = 0;
		std::string sRedisUsedTimesKey;
		if (bGlobal)
		{
			sRedisUsedTimesKey = sRedisPrefix + UT_REDIS_KEY_CODE_USED_TIMES;
		}
		else
		{
			sRedisUsedTimesKey = sRedisServerPrefix + UT_REDIS_KEY_CODE_USED_TIMES;
		}
		UT_REDIS_CMD_WITH_REPLY(rslt, pkRedisHandler, iCodeUsedTimes, hincrBy, sRedisUsedTimesKey, sFinalCode.c_str(), 0);
		if (iUseLimit > 0 && iCodeUsedTimes >= iUseLimit)
		{
			rslt = RE_ACTIVATION_CODE_USED;
			break;
		}

		//check code use range
		LC_RewardItemsEntry kRewardItem;
		server_union_info& kunioninfo = GetGameServerApp()->GetServerUnionInfo();
		//int iServerID = GetGameServerApp()->GetServerInfo().m_iServerID;
		int iServerID = GetGameServerApp()->GetServerEntryId();
		if (bSpecialCode)
		{
			if (!CheckValidUnionID(pkSpecialCode, (int32_t)kunioninfo.union_id))
			{
				rslt = RE_ACTIVATION_CODE_BAD_SERVER;
				break;
			}
			if (!CheckValidServerID(pkSpecialCode, (int32_t)iServerID))
			{
				rslt = RE_ACTIVATION_CODE_BAD_SERVER;
				break;
			}
			//if (pkSpecialCode)
			{
				for (size_t i = 0; i < pkSpecialCode->_kRewards.size(); ++i)
				{
					kRewardItem.Append(pkSpecialCode->_kRewards.at(i));
				}
			}
		}
		else
		{
			{
				bool bUionLimit = true;
				for (int i = 0; i < kcodedefine.uions_size(); ++i)
				{
					if (kcodedefine.uions(i) == kunioninfo.union_id)
					{
						bUionLimit = false;
						break;
					}
				}

				if (kcodedefine.uions_size() > 0 && bUionLimit)
				{
					rslt = RE_ACTIVATION_CODE_BAD_SERVER;
					break;
				}
			}
			{
				bool bGroupLimit = true;
				for (int i = 0; i < kcodedefine.groups_size(); ++i)
				{
					if (kcodedefine.groups(i) == kunioninfo.group_id)
					{
						bGroupLimit = false;
						break;
					}
				}

				if (kcodedefine.groups_size() > 0 && bGroupLimit)
				{
					rslt = RE_ACTIVATION_CODE_BAD_SERVER;
					break;
				}
			}
			{
				bool bServerLimit = true;
				for (int i = 0; i < kcodedefine.servers_size(); ++i)
				{
					if (kcodedefine.servers(i) == iServerID)
					{
						bServerLimit = false;
						break;
					}
				}
				if (kcodedefine.servers_size() > 0 && bServerLimit)
				{
					rslt = RE_ACTIVATION_CODE_BAD_SERVER;
					break;
				}
			}
			for (int i = 0; i < kcodedefine.gifts_size(); ++i)
			{
				const redis_define::item_entry& kitem = kcodedefine.gifts(i);
				Utility::UT_SIMDataInfo kSim;
				kSim.SetItemInfo(kitem.id(), kitem.count(), kitem.bind());
				kRewardItem.Append(kSim);
			}
		}
		//cdk
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, CDK_OPERATE, OPERATE_LOGTYPE1);
		mem::map<int32_t, int32_t> kPackSizeMap;
		kRewardItem.PackSize(kPackSizeMap, PACK_TYPE_UNDEFINE, 0);
		mem::map<int32_t, int32_t>::iterator it = kPackSizeMap.begin();
		for (; it != kPackSizeMap.end(); ++it)
		{
			if (pkPlayer->GetPackAsset()->GetPackEmptyEntryCount(it->first) < it->second)
			{
				rslt = RE_ACTIVATION_CODE_PACK_FULL;
				break;
			}
		}
		Utility::UT_SIMDataList kItems;
		kRewardItem.Resolve(kItems, NULL);

		rslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVATION_CODE, kcodedefine.id(), pkDataEntry->_sMailTitle, pkDataEntry->_sMailDesc,true,false,false, nLogCode);

		//register code
		LC_ActivationCodeEntry kCodeEntry;
		if (bSpecialCode)
		{
			kCodeEntry.m_iID = pkSpecialCode->_iID;
			kCodeEntry.m_iSerial = 0;
		}
		else
		{
			kCodeEntry.m_iID = kCode.iCodeID;
			kCodeEntry.m_iSerial = kCode.iCodeSerial;
		}
		kCodeEntry.m_iType = pkDataEntry->_iType;
		kCodeEntry.m_sFinalCode = sFinalCode;
		kActivationCodeAsset.RegCodeEntry(kCodeEntry);

		UT_REDIS_CMD(rslt, pkRedisHandler, hincrBy, sRedisUsedTimesKey, sFinalCode.c_str(), 1);
	}while (0);
	respMsg.m_value.set_rslt(rslt);
	pkPlayer->SendMsgToClient(MGPT_RESP_USE_ACTIVATION_CODE, &respMsg);
}
ON_HANDLE(MGPT_REQ_USE_ACTIVATION_COUNT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_PB<msg_define::activation_code_use_count_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	msg_define::activation_code_use_count_req& rk_req = reqMsg.m_value;

	MG_PB<msg_define::activation_code_use_count_resp> respMsg;
	respMsg.m_value.set_type(rk_req.type());

	LC_ActivationCodeAsset& kActivationCodeAsset = pkPlayer->GetActivationCodeAsset();
	LC_ActivationCodeTypeStat* pkStat = kActivationCodeAsset.GetCodeTypeStat(rk_req.type());
	if (pkStat)
	{
		respMsg.m_value.set_count(pkStat->m_iCount);
	}
	pkPlayer->SendMsgToClient(MGPT_RESP_USE_ACTIVATION_COUNT, &respMsg);
}
ON_HANDLE(MGPT_REQ_SHOT_ACTIVITY_INFO, this_source_type, false)
{
	
}
ON_HANDLE(MGPT_REQ_SHOT_ACTIVITY_RANK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_ShotActivityManager* pkShotMgr = LC_ShotActivityManager::GetSingletonPtr();
	if (NULL == pkShotMgr)
	{
		return;
	}
	MG_PB<msg_define::shot_activity_rank_info_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	pkShotMgr->OnRankInfoReq(pkPlayer, reqMsg.m_value);
}
ON_HANDLE(MGPT_REQ_SHOT_ACTIVITY_RANK_INDEX, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_ShotActivityManager* pkShotMgr = LC_ShotActivityManager::GetSingletonPtr();
	if (NULL == pkShotMgr)
	{
		return;
	}
	MG_PB<msg_define::shot_activity_rank_index_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkShotMgr->OnRankIndexReq(pkPlayer, reqMsg.m_value);
}

ON_HANDLE(MGPT_REQ_ACHIEVEMENT_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AchievementReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_AchievementReward rltMsg;
	rltMsg.m_nAchievementID = reqMsg.m_nAchievementID;
	rltMsg.m_iResult = pkPlayer->AddAchievementReward(reqMsg.m_nAchievementID);
	if(RE_SUCCESS != rltMsg.m_iResult)
	{
		const unique_id_impl& instance = pkPlayer->GetInstance();
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MG_REQ_AchievementReward c_cid:%d, c_citizenship:%d, c_uid:%d, achievementID:%d, result:%d",
			instance.detail(), instance.catagory(), pkPlayer->GetUserID(), reqMsg.m_nAchievementID, rltMsg.m_iResult);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ACHIEVEMENT_REWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_ACHIEVEMENTPOINT_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AchievementPointReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_AchievementPointReward rltMsg;
	rltMsg.m_iRewardID = reqMsg.m_iRewardID;
	rltMsg.m_iResult = pkPlayer->AddAchievementPointReward(reqMsg.m_iRewardID);
	pkPlayer->SendMsgToClient(MGPT_RLT_ACHIEVEMENTPOINT_REWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETSEVENDAYREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetSevenDayReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetSevenDayReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->AddSevenDayPointReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETSEVENDAYREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_ROBOT_ENTER_MAP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
#ifndef ROBOT_ENABLE
	return;
#endif //ROBOT_ENABLE
	MG_ReqRobotEnterMap reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SC_ServerDebugInterface* pkDebugInterface = SC_ServerScriptInterface::GetSingletonPtr()->GetDebugInterface();
	CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(reqMsg.m_iMapResID);
	if (!pkWorldMapData)
	{
		return;
	}
	MG_RespRobotEnterMap respMsg;
	respMsg.m_iMapResID = reqMsg.m_iMapResID;
	if (pkWorldMapData->iPreTaskID && !pkPlayer->GetTaskMap()->IsInFinishedTaskMap(pkWorldMapData->iPreTaskID))
	{
		pkDebugInterface->AcceptTask(pkPlayer->GetID(), pkWorldMapData->iPreTaskID);
		pkDebugInterface->FinishTask(pkPlayer->GetID(), pkWorldMapData->iPreTaskID);
	}
	int iNeedLevel = pkWorldMapData->_iNeedLevel;
	CF_TranscriptionRule::DataEntry* pkRuleData = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(reqMsg.m_iMapResID);
	if (pkRuleData)
	{
		iNeedLevel = pkRuleData->_iPlayerMinLevel;
		pkPlayer->ActiveSkillState(39112067, 3600, 0, 0);
	}
	if (pkPlayer->GetLevel() < iNeedLevel)
	{
		pkPlayer->SetLevel(iNeedLevel);
	}
	if (pkPlayer->GetUnBindCash().GetMoney() < 10000)
	{
		pkPlayer->AddCash(10000, CT_UNBIND_CASH, SYS_FUNC_TP_GM, 0);
	}
	pkPlayer->AddVigor(100);
	

	pkPlayer->SetRobot();

	UT_Vec3Int kPos(0, 0, 0);
	UT_ServerHelper::GetRandomLocationXYZ(kPos,reqMsg.m_iMapResID,pkPlayer->GetCitizenship());

	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	int32_t iMapLogicID = 0;
	switch(pkWorldMapData->_iMapLogicType)
	{
	case MT_ARENA_SOLO:
	case MT_GUILDWAR:
		{
			LC_ServerMapLogic_Place* pkPlace = pkServerWorldManager->FindProperMapLogicByRes(reqMsg.m_iMapResID, false);
			if (pkPlace)
			{
				iMapLogicID = pkPlace->GetMapLogicID();
			}
		}
		break;
	}
	respMsg.m_iRslt = pkServerWorldManager->RequestChangePlayerLocation(pkPlayer,
					  LMIOT_TRANSFER,
					  iMapLogicID,
					  reqMsg.m_iMapResID,
					  kPos);
	pkPlayer->SendMsgToClient(MGPT_RESP_ROBOT_ENTER_MAP, &respMsg);
}

ON_HANDLE(MGPT_REQ_ROBOT_RESET_TASK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
#ifndef ROBOT_ENABLE
	return;
#endif //ROBOT_ENABLE
	MG_ReqRobotResetTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_RespRobotResetTask respMsg;
	respMsg.m_iTaskID = reqMsg.m_iTaskID;

	/*pkPlayer->AddAttributePlus(ATT_MAX_HP, 100, 100);
	pkPlayer->AddAttributePlus(ATT_MAX_ATK, 100, 100);
	pkPlayer->AddAttributePlus(ATT_HP_RECOVER, 100, 100);
	pkPlayer->AddAttributePlus(ATT_DEF, 100, 100);
	pkPlayer->AddAttributePlus(ATT_DOG_RATE, 100, 100);
	pkPlayer->AddAttributePlus(ATT_CIR_RATE, 100, 100);*/

	SC_ServerDebugInterface* pkDebugInterface = SC_ServerScriptInterface::GetSingletonPtr()->GetDebugInterface();
	pkDebugInterface->AcceptTask(pkPlayer->GetID(), reqMsg.m_iTaskID);
	pkPlayer->SendMsgToClient(MGPT_RESP_ROBOT_RESET_TASK, &respMsg);
}

ON_HANDLE(MGPT_REQ_CLEAR_PK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_PKRecorderBase* pPKRecorder = pkPlayer->GetPKRecorder();
	if (pPKRecorder == NULL)
	{
		return;
	}

	MG_ReqClearPK reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RespClearPK resp;
	int nPKValue = pPKRecorder->GetPKValue();
	if (nPKValue <= 0)
	{
		resp.m_Result = RE_NOTHAVE_PK;
		pkPlayer->SendMsgToClient(MGPT_RESP_CLEAR_PK, &resp);
		return;
	}
	int nOldPkValue = nPKValue;
	if (reqMsg.m_nValue > 0 && nPKValue > reqMsg.m_nValue)
	{
		nPKValue = reqMsg.m_nValue;
	}

	int costyuanbao = UT_MathBase::MinInt(1 * nPKValue, 1000);
	if (costyuanbao <= 0 || !pkPlayer->ReduceCash(costyuanbao, CT_UNBIND_YUANBAO, SYS_FUNC_TP_PK, 0))
	{
		resp.m_Result = RE_SHOPCITY_CASH_NOT_ENOUGH;
		pkPlayer->SendMsgToClient(MGPT_RESP_CLEAR_PK, &resp);
		return;
	}

	pPKRecorder->SetPKValue(nOldPkValue - nPKValue);
	resp.m_Result = RE_CLEAR_PKBUFF_SUCCESS;
	pkPlayer->SendMsgToClient(MGPT_RESP_CLEAR_PK, &resp);
}

ON_HANDLE(MGPT_NOTIFY_COMBO_COUNT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_NotifyComboCount msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
}

ON_HANDLE(MGPT_REQ_CHANGE_CHAR_NAME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_PB<msg_define::change_name_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	msg_define::change_name_req& reqVal = reqMsg.m_value;

	ResultType rslt = RE_SUCCESS;
	do {
		
		if (reqVal.new_name().length() <= 0 || reqVal.new_name().length() + CHAR_NAME_REVERSE > MAX_CHAR_NAME_LENGTH)
		{
			rslt = RE_LOGIN_CHARNAME_LEN_INVALID;
			break;
		}

		std::stringstream ss;
		static int32_t _union_id = GetGameUnionID();
		ss << get_name_prefix(_union_id, pkPlayer->GetCitizenship()) << pkPlayer->GetCitizenship() << "." << reqVal.new_name();
		std::string sNewName = ss.str();
		for (size_t i = 0; i < sNewName.size(); ++i)
		{
			
			if (',' == sNewName[i] || '#' == sNewName[i] || '<' == sNewName[i] ||
				'>' == sNewName[i] || '&' == sNewName[i] || '%' == sNewName[i] ||
				'\r' == sNewName[i] || '\n' == sNewName[i])
			{
				rslt = RE_LOGIN_CHARNAME_INVALID;
				break;
			}
		}

		/*
		size_t iSafeLen = SAFE_TRUNCATE_STRING(sNewName.begin(), sNewName.end(), sNewName.length());
		if (iSafeLen < sNewName.length())
		{
			sNewName.resize(iSafeLen);
		}
		*/

		if (sNewName == TPSTR2STDSTR(pkPlayer->GetOwnerCharName()))
		{
			rslt = RE_LOGIN_NAME_ALREADY_USED;
			break;
		}
		
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		if(NULL == pkPackAsset)
		{
			rslt = RE_FAIL;
			break;
		}
		LC_BackPackEntry* pkCardEntry = pkPackAsset->GetBackPackEntry(reqVal.pack_type(), reqVal.pack_index());
		if (!pkCardEntry)
		{
			rslt = RE_ITEM_INVALID;
			break;
		}
		if (pkCardEntry->GetBackPackValueByType(LPSFET_TYPE_ID) != SPEC_ITEM_TID_RENAME_CARD)
		{
			rslt = RE_ITEM_CANT_USE;
			break;
		}
		PackOperateInfoList kItemOPs;
		PackOperateInfo objData;
		objData.Reset();
		objData.m_iOPType = OPT_DELETE_ITEM_BY_INDEX;
		objData.m_bOPEntry = false;
		objData.m_iPackType = reqVal.pack_type();
		objData.m_iIndex = reqVal.pack_index();
		objData.m_lCount = 1;
		kItemOPs.push_back(objData);

		rslt = LC_Helper::CheckItemOPs(pkPackAsset, kItemOPs);
		if (RE_SUCCESS != rslt)
		{
			break;
		}
		pkCardEntry->SetBackPackValueByType(LPSFET_LOCKED, true);

		MG_PB<msg_define::change_name_try_req> tryReqMsg;
		msg_define::change_name_try_req& tryReqVal = tryReqMsg.m_value;
		tryReqVal.set_pack_type(reqVal.pack_type());
		tryReqVal.set_pack_index(reqVal.pack_index());
		tryReqVal.set_new_name(sNewName);
		SendMsgToGameDB(MGPT_GD_REQ_CHANGE_CHAR_NAME, &tryReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	} while (0);

	if (RE_SUCCESS != rslt)
	{
		MG_PB<msg_define::change_name_resp> respMsg;
		msg_define::change_name_resp& respVal = respMsg.m_value;
		respVal.set_result(rslt);
		pkPlayer->SendMsgToClient(MGPT_RLT_CHANGE_CHAR_NAME, &respMsg);
	}
}

ON_HANDLE(MGPT_REQ_RECEIVE_DAILY_ACTIVITY_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ReceiveDailyActivityReward msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_Rlt_ReceiveDailyActivityReward rltMsg;
	rltMsg.m_nRewardID = msg.m_nRewardID;
	rltMsg.m_nResult = pkPlayer->ReceiveDailyAcivityRewards(msg.m_nRewardID);

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->syncDailyActivity();
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_RECEIVE_DAILY_ACTIVITY_REWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_ACTIVATE_STARVEIN, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_Req_ActivateStarVein msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_Rlt_ActivateStarVein rltMsg;
	rltMsg.m_nStarVeinID = msg.m_nStarVeinID;
	rltMsg.m_nResult = pkPlayer->ActivateStarVein(msg.m_nStarVeinID);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncActivateStarVein();
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_ACTIVATE_STARVEIN, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GET_STARVEIN_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetStarVeinReward msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetStarVeinReward rltMsg;
	rltMsg.m_nID = msg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetStarVeinReward(msg.m_nID);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncStarVeinGoalRecord();
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_GET_STARVEIN_REWARD, &rltMsg);
}

ON_HANDLE(MG_REQ_REWARD_TIME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RewardTime msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_RewardTime rltMsg;
	rltMsg.m_nResult = pkPlayer->CanGetFirstDailyChargeReward();
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		rltMsg.m_time = pkPlayer->GetNextFDTime();
	}

	pkPlayer->SendMsgToClient(MG_RLT_REWARD_TIME, &rltMsg);
}


ON_HANDLE(MG_REQ_GET_FD_FREE_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetFDFreeReward msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetFDFreeReward rltMsg;
	rltMsg.m_nDayID = msg.m_nDayID;
	rltMsg.m_nResult = RE_SUCCESS;
	if(msg.m_nDayID != (uint32_t)pkPlayer->GetFDDay())
	{
		rltMsg.m_nResult = RE_FAIL;
	}

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		rltMsg.m_nResult = pkPlayer->GetFreeReward(msg.m_nDayID, msg.m_nFreeRewardID);
	}

	pkPlayer->SendMsgToClient(MG_RLT_GET_FD_FREE_REWARD, &rltMsg);
}

ON_HANDLE(MG_REQ_GET_FD_DISCOUNT_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetFDDisCountReward msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetFDDisCountReward rltMsg;
	rltMsg.m_nDayID = msg.m_nDayID;
	rltMsg.m_nResult = RE_SUCCESS;
	if(msg.m_nDayID != (uint32_t)pkPlayer->GetFDDay())
	{
		rltMsg.m_nResult = RE_FAIL;
	}

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		rltMsg.m_nResult = pkPlayer->GetDisCountReward(msg.m_nDayID, msg.m_DisCountRewardID);
	}

	pkPlayer->SendMsgToClient(MG_RLT_GET_FD_DISCOUNT_REWARD, &rltMsg);
}

ON_HANDLE(MG_REQ_BUY_GROWTH_PACKAGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_BuyGrowthPackage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_BuyGrowthPackage rltMsg;
	rltMsg.m_nBuddyID = reqMsg.m_nBuddyID;
	rltMsg.m_nResult = pkPlayer->BuyBuddyGrowthPackage(reqMsg.m_nBuddyID);
	
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncBuddyGrowthPackage();
	}

	pkPlayer->SendMsgToClient(MG_RLT_BUY_GROWTH_PACKAGE, &rltMsg);

}

ON_HANDLE(MG_REQ_GET_GROWTH_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetGrowthReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetGrowthReward rltMsg;
	rltMsg.m_nBuddyID = reqMsg.m_nBuddyID;
	rltMsg.m_nResult = pkPlayer->GetBuddyGrowthReward(reqMsg.m_nBuddyID);

	if(RE_SUCCESS==rltMsg.m_nResult || RE_BUDDY_GROWTH_REWARD_PACKET_HAS_FULL==rltMsg.m_nResult)
	{
		pkPlayer->SyncBuddyGrowthPackage();
	}

	pkPlayer->SendMsgToClient(MG_RLT_GET_GROWTH_REWARD, &rltMsg);
}

//收藏激活
ON_HANDLE(MG_REQ_COLLECTION_ACTIVATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	
	MG_REQ_CollectionActivation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int size = reqMsg.m_nCollectActivationIDs.size();
	MG_RLT_CollectionActivation rltMsg;
	if(size > 30)
	{
		rltMsg.m_nResult = RE_ERROR_PARAM;
	}
	
	for(int i=0; i<size; ++i)
	{
		int id = reqMsg.m_nCollectActivationIDs[i];
		rltMsg.m_nResult = pkPlayer->CollectionActivation(id);
		if(RE_SUCCESS == rltMsg.m_nResult)
		{
			//收藏激活--埋点
			vector<uint64_t> nParam;
			nParam.clear();
			nParam.push_back(id);
			int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, COLLECTION_OPERATE, OPERATE_LOGTYPE1);
			RecordOperateLog(pkPlayer, BASE_OPERATE, nLogCode,nParam);
			rltMsg.m_nCollectActivationIDs.push_back(id);
		}
		else
		{
			break;
		}
	}
	pkPlayer->GetCommanderAttrMgr().RefreshCollectionAttributeMap(pkPlayer, false);
	pkPlayer->SendMsgToClient(MG_RLT_COLLECTION_ACTIVATION, &rltMsg);
}
//收藏进度
ON_HANDLE(MG_REQ_COLLECTION_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CollectionReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_CollectionReward rltMsg;
	rltMsg.m_nCollectRewardID = reqMsg.m_nCollectRewardID;
	rltMsg.m_nResult = pkPlayer->CollectionReward(reqMsg.m_nCollectRewardID);

	pkPlayer->SendMsgToClient(MG_RLT_COLLECTION_REWARD, &rltMsg);
}
//圣痕激活
ON_HANDLE(MG_REQ_ACTIVATE_SKILLBOOK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ActivateSkillBook reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int skillBookID = reqMsg.m_nSkillBookID;

	MG_RLT_ActivateSkillBook rltMsg;
	rltMsg.m_nSkillBookID = skillBookID;
	rltMsg.m_nResult = pkPlayer->ActivateSkillBook(reqMsg.m_nSkillBookID);
	pkPlayer->SendMsgToClient(MG_RLT_ACTIVATE_SKILLBOOK, &rltMsg);

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncOpenNewSkillBook(reqMsg.m_nSkillBookID);
		pkPlayer->SyncSkillBookInfo();
	}
}

ON_HANDLE(MG_REQ_UNLOCK_SKILLBOOKSLOT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_Unlock_SkillBookSlot reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_Unlock_SkillBookSlot rltMsg;
	rltMsg.m_nIndex = reqMsg.m_nIndex;
	rltMsg.m_nSlotID = reqMsg.m_nSlotID;
	rltMsg.m_nResult = pkPlayer->UnlockSkillBookSlot(reqMsg.m_nIndex, reqMsg.m_nSlotID);
	rltMsg.m_nSlotState = (rltMsg.m_nResult==RE_SUCCESS) ? true : false;
	pkPlayer->SendMsgToClient(MG_RLT_UNLOCK_SKILLBOOKSLOT, &rltMsg);
	
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncSkillBookSlotInfo();
	}
}

ON_HANDLE(MG_REQ_WEAR_SKILLBOOK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_WearSkillBook reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_WearSkillBook rltMsg;
	rltMsg.m_nOperatorType = reqMsg.m_nOperatorType;
	rltMsg.m_nIndex = reqMsg.m_nIndex;
	rltMsg.m_nSlotID = reqMsg.m_nSlotID;
	rltMsg.m_nSkillBookID = reqMsg.m_nSkillBookID;
	rltMsg.m_nResult = RE_FAIL;
	switch(reqMsg.m_nOperatorType)
	{
	case 0:
		rltMsg.m_nResult = pkPlayer->UnWearSkillBook(reqMsg.m_nIndex, reqMsg.m_nSlotID, reqMsg.m_nSkillBookID);
		break;
	case 1:
		rltMsg.m_nResult = pkPlayer->WearSkillBook(reqMsg.m_nIndex, reqMsg.m_nSlotID, reqMsg.m_nSkillBookID);
		break;
	default:
		break;
	}
	
	pkPlayer->SendMsgToClient(MG_RLT_WEAR_SKILLBOOK, &rltMsg);

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		if(reqMsg.m_nOperatorType == 1)
		{
			//圣痕佩戴--埋点日志
			vector<uint64_t> nParams;
			nParams.clear();
			nParams.push_back(reqMsg.m_nIndex+1);
			nParams.push_back(reqMsg.m_nSlotID+1);
			int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, SHENGHEN_OPERATE, OPERATE_LOGTYPE1);
			RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode,nParams);
		}
		
		pkPlayer->SyncSkillBookInfo();
		pkPlayer->SyncSkillBookSlotInfo();
	}
}


ON_HANDLE(MG_REQ_RELIVE_BEFOREENTERMAP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_Relive_BeforeEnterMap reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_Relive_BeforeEnterMap rltMsg;
	rltMsg.m_iResult = pkPlayer->ReliveAllBuddyBeforeEnterMap(true);
	if(rltMsg.m_iResult ==RE_SUCCESS)
	{
		pkPlayer->SetReliveBeforeEnterMapTag(true);
	}
	pkPlayer->SendMsgToClient(MG_RLT_RELIVE_BEFOREENTERMAP, &rltMsg);
}

ON_HANDLE(MG_REQ_BIOGRAPHYPART_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_BiographyPartReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_BiographyPartReward rltMsg;
	rltMsg.m_nBiographyPartID = reqMsg.m_nBiographyPartID;
	rltMsg.m_nResult = pkPlayer->BiographyPartReward(reqMsg.m_nBiographyPartID);
	pkPlayer->SendMsgToClient(MG_RLT_BIOGRAPHYPART_REWARD, &rltMsg);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncBiographyRewardInfo();
		//LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
		//pkTaskManager->AssignTask(pkPlayer, pkTaskManager->GetAssignType(STT_BIOGRAPHY_TASK), false, NONE_ASSIGN, NONE_ASSIGN, reqMsg.m_nBiographyPartID);
	}
}

ON_HANDLE(MG_REQ_RESET_SHOPTIMES, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ResetShopTimes reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_ResetShopTimes rltMsg;
	rltMsg.m_nShopType = reqMsg.m_nShopType;
	rltMsg.m_nResult = pkPlayer->ResetShopTimes(reqMsg.m_nShopType, rltMsg.m_nResetTimes);
	pkPlayer->SendMsgToClient(MG_RLT_RESET_SHOPTIMES, &rltMsg);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		MG_RLT_GetShopItem rltMsg;
		rltMsg.m_nShopType = reqMsg.m_nShopType;
		pkPlayer->GetShopItem(reqMsg.m_nShopType, rltMsg.m_nItemInfo);
		pkPlayer->SendMsgToClient(MGPT_RLT_GET_SHOPITEM, &rltMsg);
	}
}

ON_HANDLE(MQ_REQ_GET_SHOPRESETTIMES, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetShopResetTimes reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetShopResetTimes rltMsg;
	rltMsg.m_nShopType = reqMsg.m_nShopType;
	rltMsg.m_nResetTimes = pkPlayer->GetShopResetTimes(reqMsg.m_nShopType);
	pkPlayer->SendMsgToClient(MQ_RLT_GET_SHOPRESETTIMES, &rltMsg);
}

ON_HANDLE(MGPT_CS_FIRE_TIMER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_CS_FireTimer reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	pkTimerMgr->FireTriTimestamp(reqMsg.m_nTimerId);
}

ON_HANDLE(MGPT_REQ_RUNE_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RuneReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->RuneReward(reqMsg.m_nRewardID);
}

ON_HANDLE(MGPT_REQ_GET_SHOPITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetShopItem reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetShopItem rltMsg;
	rltMsg.m_nShopType = reqMsg.m_nShopType;
	pkPlayer->GetShopItem(reqMsg.m_nShopType, rltMsg.m_nItemInfo);
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_SHOPITEM, &rltMsg);
}

ON_HANDLE(MGPT_REQ_BUYINVESMENTCARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_BuyInvesmentCard reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_BuyInvesmentCard rltMsg;
	rltMsg.m_nInvesmentType = reqMsg.m_nInvesmentType;
	rltMsg.m_nResult = pkPlayer->BuyInvestmentCard(reqMsg.m_nInvesmentType);
	pkPlayer->SendMsgToClient(MGPT_RLT_BUYINVESMENTCARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_ADDINVESTMENTCARDREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AddInvestmentCardReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_AddInvestmentCardReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->AddInvestmentCardReward(reqMsg.m_nID, rltMsg.m_nRewardType);
	pkPlayer->SendMsgToClient(MGPT_RLT_ADDINVESTMENTCARDREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_REFRESHREWARDTASK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RefershRewardTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_RefershRewardTask rltMsg;
	rltMsg.m_nResult = pkPlayer->RefreshRewardTask();
	pkPlayer->SendMsgToClient(MGPT_RLT_REFRESHREWARDTASK, &rltMsg);
}

ON_HANDLE(MGPT_REQ_CLOSESETTLEWINDOW, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CloseSettleWindow reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_CloseSettleWindow rltMsg;
	pkPlayer->SendMsgToClient(MGPT_RLT_CLOSESETTLEWINDOW, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SETRANKTRANSFERSTATE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetRankTransferState reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetRankTransferState rltMsg;
	rltMsg.m_nMapResID = reqMsg.m_nMapResID;
	rltMsg.m_nResult = pkPlayer->SetRankTransferFailed(reqMsg.m_nMapResID);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETRANKTRANSFERSTATE, &rltMsg);
}

//
ON_HANDLE(MGPT_REQ_GETRANKYIZHONGQINXI, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetRankYiZhongQinXi reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::GetPlayerDamageRankFromRedis(pkPlayer, reqMsg.m_nStoryID, reqMsg.m_nMinIndex, reqMsg.m_nMaxIndex);
}

ON_HANDLE(MGPT_REQ_GETGUILDDAMAGERANK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetGuildDamageRank reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::GetGuildDamageRankFromRedis(pkPlayer, reqMsg.m_nStoryID, reqMsg.m_nMinIndex, reqMsg.m_nMaxIndex);
}


ON_HANDLE(MGPT_REQ_ADDREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AddReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_AddReward rltMsg;
	rltMsg.m_nVIPLevel = reqMsg.m_nVIPLevel;
	rltMsg.m_nResult = pkPlayer->AddNewVipReward(reqMsg.m_nVIPLevel);
	pkPlayer->SendMsgToClient(MGPT_RLT_ADDREWARD, &rltMsg);
}

ON_HANDLE(MGPT_CS_NEWVIP_REWARD_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_CS_NewVIPRewardReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	MG_SC_NewVIPRewardResult rltMsg;
	rltMsg.m_nError = pkPlayer->GetVIPDropReward();
	if(RE_SUCCESS == rltMsg.m_nError)
	{
		rltMsg.m_nError = pkPlayer->TryFetchNewVIPBonus();
	}
	pkPlayer->SendMsgToClient(MGPT_SC_NEWVIP_REWARD_RESULT, &rltMsg);
}

ON_HANDLE(MGPT_REQ_COMMITANSWER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CommitAnswer reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_CommitAnswer rltMsg;
	rltMsg.m_nTopicIndex = reqMsg.m_nTopicIndex;
	rltMsg.m_bResult = pkPlayer->CommitAnswer(reqMsg.m_nTopicIndex, reqMsg.m_nChoice);
	pkPlayer->SendMsgToClient(MGPT_RLT_COMMITANSWER, &rltMsg);
}

ON_HANDLE(MGPT_REQ_ANSWERRANKINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AnswerRankInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_AnswerRankInfo rltMsg;
	rltMsg.m_nRankType =reqMsg.m_nRankType; 
	rltMsg.m_nResult = pkPlayer->GetAnswerRankInfo(reqMsg.m_nMinIndex, reqMsg.m_nMaxIndex, rltMsg);
	
	pkPlayer->SendMsgToClient(MGPT_RLT_ANSWERRANKINFO, &rltMsg);
}


ON_HANDLE(MGPT_REQ_GUILDFIGHTRANKINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_GuildFightRankInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	MG_RLT_GuildFightRankInfo rltMsg;
	rltMsg.m_nResult = pkPlayer->GetGuildFightRankInfo(reqMsg, rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_GUILDFIGHTRANKINFO, &rltMsg);
}


ON_HANDLE(MGPT_REQ_GETNEARBYPLAYERINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetNearbyPlayer reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetNearbyPlayer rltMsg;
	pkPlayer->GetNearbyPlayer(rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETNEARBYPLAYERINFO, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETRANKSUBBINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetRankSubBInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetRankSubBInfo rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nType = reqMsg.m_nType;
	rltMsg.m_nResult = pkPlayer->GetActivityRankInfo(reqMsg.m_nType, reqMsg.m_nID, reqMsg.m_nMinIndex, reqMsg.m_nMaxIndex, rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETRANKSUBBINFO, &rltMsg);
}

ON_HANDLE(MGPT_REQ_YIZHONGQINGXI_DAMAGEDATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_YiZhongQinXiDamageData reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	UT_ServerHelper::GetPlayerDamageData(pkPlayer, reqMsg.m_nStoryID, reqMsg.m_nCharID);
}

ON_HANDLE(MGPT_REQ_YIZHONGQINGXI_DETAILDAMAGEDATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_YiZhongQinXiDetailDamageData reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::GetDetailDamageData(pkPlayer, reqMsg.m_nStoryID, reqMsg.m_nTime, reqMsg.m_nCharID);
}

ON_HANDLE(MGPT_REQ_FIGHTRECORD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_SS_GetGuildDefendLog msg;
	msg.m_serverID = GSNodeNetwork::Instance().getEntryIdByMergeId(GetGameServerID());
	msg.m_charID = pkPlayer->GetInstance();
	msg.m_nodeID = GetGameServerID();
	SendMsgToSociety(MGPT_SS_GETGUILDDEFENDLOG, &msg);
}

ON_HANDLE(MGPT_REQ_GETBEDEFIGHTEDFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_SS_REQ_GetBeDefightedFlag msg;
	msg.m_serverID = GSNodeNetwork::Instance().getEntryIdByMergeId(GetGameServerID());
	msg.m_charID = pkPlayer->GetInstance();
	msg.m_nodeID = GetGameServerID();
	SendMsgToSociety(MGPT_SS_REQ_GETBEDEFIGHTEDFLAG, &msg);
}


ON_HANDLE(MGPT_REQ_UPGRADECOMMANDERTITLE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_UpgradeCommanderTitle reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_UpgradeCommanderTitle rltMsg;
	rltMsg.m_nResult = pkPlayer->UpgradeCommanderTitle();
	rltMsg.m_nCurrentCommanderTitle = pkPlayer->GetCommanderTitle();
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->GetCommanderAttrMgr().RefreshCommanderTitleAttributeMap(pkPlayer, false);
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_UPGRADECOMMANDERTITLE, &rltMsg);
}

ON_HANDLE(MGPT_REQ_KILLBOSSREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_KillBossReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_KillBossReward rltMsg;
	rltMsg.m_nNpcCharID = reqMsg.m_nNpcCharID;
	rltMsg.m_bServerReward = reqMsg.m_bServerReward;
	if(rltMsg.m_bServerReward)
	{
		rltMsg.m_nResult = pkPlayer->GetServerFirstKillBossReward(reqMsg.m_nNpcCharID);
	}
	else
	{
		rltMsg.m_nResult = pkPlayer->GetKillBossReward(reqMsg.m_nNpcCharID);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_KILLBOSSREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETSPECIALTRAININGREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetSpecialTrainingReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetSpecialTrainingReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetSpecialTrainingReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETSPECIALTRAININGREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SETAUTODECOMPOSITIONITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetAutoDecompositionItem reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetAutoDecompositionItem rltMsg;
	rltMsg.m_nAutoDecomposition = pkPlayer->SetAutoDecomposition(reqMsg.m_nAutoDecomposition);
	rltMsg.m_nResult = RE_SUCCESS;
	pkPlayer->SendMsgToClient(MGPT_RLT_SETAUTODECOMPOSITIONITEM, &rltMsg);
	pkPlayer->SyncDecompositionSetting();
}

ON_HANDLE(MGPT_REQ_SETPROFILEPICTURE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetProfilePicture reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetProfilePicture rltMsg;
	rltMsg.m_nProfilePictureID = reqMsg.m_nProfilePictureID;
	rltMsg.m_nResult = pkPlayer->SetProfilePictureID(reqMsg.m_nProfilePictureID);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETPROFILEPICTURE, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SETCURRENTCOMMANDERTITLEID, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetCurrentCommanderTitleID reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetCurrentCommanderTitleID rltMsg;
	rltMsg.m_nCurrentCommanderTitleID = reqMsg.m_nCurrentCommanderTitleID;
	rltMsg.m_nResult = pkPlayer->SetCurrentCommanderTitleFrameID(reqMsg.m_nCurrentCommanderTitleID);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETCURRENTCOMMANDERTITLEID, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SETCOMMANDERTITLEFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetCommanderTitleFlag reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetCommanderTitleFlag rltMsg;
	rltMsg.m_nTitelID = reqMsg.m_nTitelID;
	rltMsg.m_nResult = pkPlayer->SetCommanderTitleFlag(reqMsg.m_nTitelID);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETCOMMANDERTITLEFLAG, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SETPROFILEPICTUREFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetProfilePictureFlag reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetProfilePictureFlag rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->SetProfilePictureFlag(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETPROFILEPICTUREFLAG, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETCHAPTERRAIDSTARREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetChapterRaidStarReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetChapterRaidStarReward rltMsg;
	rltMsg.m_nChapterID = reqMsg.m_nChapterID;
	rltMsg.m_nRewardIndex = reqMsg.m_nRewardIndex;
	rltMsg.m_nResult = pkPlayer->GetChapterRaidStarReward(reqMsg.m_nChapterID, reqMsg.m_nRewardIndex);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETCHAPTERRAIDSTARREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETYIJITUXISTAGEREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetYiJiTuXiStageReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetYiJiTuXiStageReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetYiJiTuXiStageReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETYIJITUXISTAGEREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETPETISLANDSSTAGEREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetPetIslandsStageReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetPetIslandsStageReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetPetIslandsStageReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETPETISLANDSSTAGEREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETYIJIXUNBAOSTAGEREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetYiJiXunBaoStageReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetYiJiXunBaoStageReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetYiJiXunBaoStageReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETYIJIXUNBAOSTAGEREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_BUYRARESUBDEVAEYETRANSFORM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_BuyRareSubDevaEyeTransform reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_BuyRareSubDevaEyeTransform rltMsg;
	rltMsg.m_nShopID = reqMsg.m_nShopID;
	rltMsg.m_nResult = pkPlayer->BuyRareSubDevaEyeTransform(reqMsg.m_nShopID);
	pkPlayer->SendMsgToClient(MGPT_RLT_BUYRARESUBDEVAEYETRANSFORM, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETROTARYTABLEREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetRotaryTableReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetRotaryTableAsset().GetRotaryTableReward(pkPlayer);
}

ON_HANDLE(MGPT_REQ_SETROTARYTABLESKIPANIMATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetRotaryTableSkipAnimation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetRotaryTableAsset().SetRotaryTableSkipAnimation(pkPlayer, reqMsg.m_bSkipAnimation);
}

ON_HANDLE(MGPT_REQ_BUYNONPAREILITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_BuyNonpareilItem reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetNonpareilAsset().BuyNonpareilItem(pkPlayer, reqMsg.m_nID);
}

ON_HANDLE(MGPT_REQ_GETNONPAREILRETURN, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetNonpareilReturn reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetNonpareilAsset().GetNonpareilReturn(pkPlayer, reqMsg.m_nID);
}

ON_HANDLE(MGPT_GAMESTORY_KEY_ECOUNTED_STAT_REQ_EX, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_REQ_EX reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	GameStory::LC_GameStory_Manager* pkStoryManager = SERVER_GET_GAMESTORY_MANAGER;
	if (NULL == pkStoryManager)
	{
		return;
	}
	pkStoryManager->NotifyActorEncountedStatByDamageType(pkPlayer, reqMsg.m_iStroyID, reqMsg.m_uiID, reqMsg.m_nDamageType);
}

ON_HANDLE(MGPT_REQ_GUILD_FIGHT_ROB_SCORE_RECEIVE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_GuildFightRobScoreReceive reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	MG_Resp_GuildFightRobScoreReceive msg;
	msg.m_uiResult = pkPlayer->GetPlayerGuildRobInfo()->ReceiveRewardByWonNum(pkPlayer,reqMsg.m_ReceiveNum);
	pkPlayer->GetPlayerGuildRobInfo()->PackGuildRobScoreReceivedInfo(msg.m_ReceivedInfo);
	pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_ROB_SCORE_RECEIVE, &msg);
}

ON_HANDLE(MGPT_REQ_GETREGULARBOSSKILLERINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GetRegularBossKillerInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	} 

	MG_RLT_GetRegularBossKillerInfo rltMsg;
	rltMsg.m_nCharTypeID = reqMsg.m_nCharTypeID;
	rltMsg.m_nMapID = reqMsg.m_nMapID;
	rltMsg.m_nResult = UT_ServerHelper::GetRegularBossKillerInfo(reqMsg.m_nCharTypeID, reqMsg.m_nMapID, rltMsg.m_nGuildName, rltMsg.m_nPlayerName);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETREGULARBOSSKILLERINFO, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SETCHAINFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SetChainFlag reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetChainFlag rltMsg;
	rltMsg.m_nIndex = reqMsg.m_nIndex;
	rltMsg.m_nResult = pkPlayer->SetChainFlag(reqMsg.m_nIndex);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETCHAINFLAG, &rltMsg);
}

ON_HANDLE(MGPT_REQ_ADDESCORTREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddEscortReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_AddEscortReward rltMsg;
	rltMsg.m_nEscortType = reqMsg.m_nEscortType;
	rltMsg.m_nNpcID = reqMsg.m_nNpcID;
	rltMsg.m_nResult = pkPlayer->AddEscortReward(reqMsg.m_nEscortType, reqMsg.m_nNpcID);
	pkPlayer->SendMsgToClient(MGPT_RLT_ADDESCORTREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETBESTGUILDREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GetBestGuildReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetBestGuildReward rltMsg;
	rltMsg.m_nResult = pkPlayer->GetBestGuildReward();
	pkPlayer->SendMsgToClient(MGPT_RLT_GETBESTGUILDREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SETEQUIPSUITLEVEL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SetEquipSuitLevel reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetEquipSuitLevel rltMsg;
	rltMsg.m_nEquipSuitLevel = reqMsg.m_nEquipSuitLevel;
	rltMsg.m_nResult = pkPlayer->SetEquipSuitLevel(reqMsg.m_nEquipSuitLevel);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETEQUIPSUITLEVEL, &rltMsg);
}


ON_HANDLE(MGPT_REQ_GETRESOURCERETRIEVERREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GetResourceRetrieverReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetResourceRetrieverReward rltMsg;
	rltMsg.m_nSpecialFind = reqMsg.m_nSpecialFind;
	for(int i=0; i<reqMsg.m_nIDVec.size(); ++i)
	{
		rltMsg.m_nResult = pkPlayer->GetResourceRetrieverReward(reqMsg.m_nIDVec[i], reqMsg.m_nSpecialFind);
		if(RE_SUCCESS == rltMsg.m_nResult)
		{
			rltMsg.m_nID.push_back(reqMsg.m_nIDVec[i]);
		}
		else
		{
			break;
		}
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_GETRESOURCERETRIEVERREWARD, &rltMsg);
	pkPlayer->SyncResourceRetrieverRecord();

}


ON_HANDLE(MGPT_REQ_GETFIRSTCHARGEREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetFirstChargeReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetFirstChargeReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetFirstChargeReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETFIRSTCHARGEREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETVIPEXTRAREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetVIPExtraReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetVIPExtraReward rltMsg;
	rltMsg.m_nResult = pkPlayer->GetVIPExtraReward(rltMsg.m_nItemInfo);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETVIPEXTRAREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETNEWACHIEVEMENTREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetNewAchievementReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetNewAchievementReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetNewAchievementReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETNEWACHIEVEMENTREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SUBEQUIPSLOTMOLDINGSPIRIT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SubEquipSlotMoldingSpirit reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->UpdateSubEquipSlotMoldingSpiritLevel(reqMsg.m_nSubType,reqMsg.m_nSlotIndex);
}

ON_HANDLE(MGPT_REQ_BUDDYTRANSFORMOP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_BuddyTransformOP reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_BuddyTransformOP rltMsg;
	rltMsg.m_nOpType = reqMsg.m_nOpType;
	rltMsg.m_nBuddyID = reqMsg.m_nBuddyID;
	rltMsg.m_nTransformID = reqMsg.m_nTransformID;
	switch(reqMsg.m_nOpType)
	{
	case BuddyTransform_Set:
		{
			rltMsg.m_nResult = pkPlayer->SetBuddyCurrentTransformID(reqMsg.m_nBuddyID, reqMsg.m_nTransformID);
		}
		break;
	case BuddyTransform_Active:
		{
			rltMsg.m_nResult = pkPlayer->ActiveBuddyTransform(reqMsg.m_nBuddyID, reqMsg.m_nTransformID, false);
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		}
		break;
	default:
		rltMsg.m_nResult = RE_FAIL;
	}
	
	pkPlayer->SendMsgToClient(MGPT_RLT_BUDDYTRANSFORMOP, &rltMsg);
}

ON_HANDLE(MGPT_REQ_YIJITUXIRANKINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_YiJiTuXiRankInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_YiJiTuXiRankInfo rltMsg;
	rltMsg.m_bFlag = reqMsg.m_bFlag;
	rltMsg.m_nResult = pkPlayer->GetYiJiTuXiRankInfo(reqMsg.m_nMinIndex, reqMsg.m_nMaxIndex, rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_YIJITUXIRANKINFO, &rltMsg);
}

ON_HANDLE(MGPT_SETGUILDPRESENTFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_SetGuildPresentFlag reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SetGuildPresentFlag(reqMsg.m_nShowGuildPresentName);
}

ON_HANDLE(MGPT_REQ_GETGUILDPRESENTREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetGuildPresentReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetGuildPresentReward rltMsg;
	rltMsg.m_nResult = pkPlayer->GetGuildPresentReward(reqMsg.m_nIDSet, rltMsg.m_nIDRewardInfo);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETGUILDPRESENTREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETGUILDCRYSTALREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetGuildCrystalReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetGuildCrystalReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetGuildCrystalReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETGUILDCRYSTALREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETTREASUREPOS, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetTreasurePos reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetTreasurePos rltMsg;
	rltMsg.m_nResult = pkPlayer->GetTreasurePos(rltMsg.m_nMapID, rltMsg.m_nX, rltMsg.m_nY, rltMsg.m_nZ);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETTREASUREPOS, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETGUILDMAPBOSSRANK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetGuildMapBossRank reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetGuildMapBossRank rltMsg;
	rltMsg.m_nMapID = reqMsg.m_nMapID;
	rltMsg.m_nMinIndex = reqMsg.m_nMinIndex;
	rltMsg.m_nMaxIndex = reqMsg.m_nMaxIndex;	
	rltMsg.m_nResult = pkPlayer->GetGuildMapBossRank(rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETGUILDMAPBOSSRANK, &rltMsg);
}

//接收委派任务
ON_HANDLE(MGPT_CS_AccpetEntrustTask_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_AccpetEntrustTask_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	player->Player_AcceptEntrustTask( msg.m_Id, msg.m_Vec );
}

//加速委派任务
ON_HANDLE(MGPT_CS_SpeedUpEntrustTask_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_SpeedUpEntrustTask_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	player->Player_SpeedUpEntrustTask( msg.m_Index );
}

//得到委派奖励
ON_HANDLE(MGPT_CS_GetAwardEntrustTask_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_GetAwardEntrustTask_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	player->Player_GetAwardEntrustTask( msg.m_Indexs );
}

//刷新委派任务
ON_HANDLE(MGPT_CS_UpdateEntrustTask_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_UpdateEntrustTask_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	player->Player_UpdateEntrustTask();
}

ON_HANDLE(MGPT_REQ_GUILDMAPBOSSEND, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GuildMapBossEnd reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(GetGameServerApp()->IsNodeType_Global())
	{
		//公会秘境，玩家主动退出，不需要弹结算界面
		bool sync = MT_GUILD_MAP==pkPlayer->GetMapLogicType() ? false : true;
		pkPlayer->ForceUpdateDamageData(sync);

		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
		if (ptr != NULL)
		{
			const unique_id_impl& playerInstance = pkPlayer->GetInstance();
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MG_REQ_GuildMapBossEnd-quit transfer c_cid:[%d], c_citizenship:[%d], c_uid:[%d], resMapId:[%d], logicMapID:[%d], time:[%d]\n", 
				playerInstance.detail(), playerInstance.catagory(), pkPlayer->GetUserID(), pkPlayer->GetMapResID(), pkPlayer->GetMapLogicID(), GET_CURRENT_TIMESTAMP_IN_SECONDS());
			
			pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_QUIT_TRANSCRIPTION, 0, 1,	UT_Vec3Int());
		}
	}
}

ON_HANDLE(MGPT_REQ_GETGAMERESTARTREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetGameRestartReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetGameRestartReward rltMsg;
	rltMsg.m_nID = reqMsg.m_nID;
	rltMsg.m_nResult = pkPlayer->GetGameRestartReward(reqMsg.m_nID);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETGAMERESTARTREWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_GETSINGLEDAMAGEREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetSingleDamageReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_GetSingleDamageReward rltMsg;
	rltMsg.m_nMapID = reqMsg.m_nMapID;
	rltMsg.m_nRewardIndex = reqMsg.m_nRewardIndex;
	rltMsg.m_nResult = pkPlayer->GetSingleDamageReward(reqMsg.m_nMapID, reqMsg.m_nRewardIndex);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETSINGLEDAMAGEREWARD, &rltMsg);
}

//寻宝抽奖
ON_HANDLE(MGPT_CS_Lottery_Draw_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_Lottery_Draw_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	player->LotteryDraw( msg.m_Id, msg.m_Count );
}

//寻宝领取额外奖励
ON_HANDLE(MGPT_CS_Lottery_ExtraReward_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_Lottery_ExtraReward_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	player->LotteryExtraReward( msg.m_Id, msg.m_Param );
}

//领取充值奖励
ON_HANDLE(MGPT_SC_ChargeProfit_Reward_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_ChargeProfit_Reward_Req  msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	player->ChargeProfitReward( msg.m_Id, msg.m_Param );
}

//主题活动结束，类型6的活动，剩余材料自动兑换 MGPT_CS_ThemeAutoExchange_Req 
ON_HANDLE(MGPT_CS_ThemeAutoExchange_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_ThemeAutoExchange_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	player->ThemeAutoExchage( msg.m_Id );
}

//主题活动，玩家登陆，第一次活动开启时候的同步标记
ON_HANDLE(MGPT_CS_ThemeFlagChange_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_ThemeFlagChange_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	player->ThemeFlagChange();
}

ON_HANDLE(MGPT_REQ_SETHUANJINGINVAILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_SetHuanJingInVaild msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SetHuanJingInvaild(msg.m_nID);
}

ON_HANDLE(MGPT_REQ_SETREDPOINTFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_SetRedPointFlag msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetRedPointAsset().SetRedPointFlag(pkPlayer, msg.m_nParam, msg.m_bFlag);
}

ON_HANDLE(MGPT_REQ_GETSUBBVALUE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetSubBValue msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetSubBDataByType(msg.m_nTypeVec);
}

ON_HANDLE(MGPT_REQ_GETTASKEXTRAREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetTaskExtraReward msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetTaskExtraRewardAsset().GetTaskExtraReward(pkPlayer);
}

ON_HANDLE(MGPT_REQ_ADDNOVICETUTORIAL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_AddNoviceTutorial msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetNoviceTutorialAsset().AddNoviceTutorial(pkPlayer, msg.m_nNoviceTutorialID);
}

ON_HANDLE(MGPT_REQ_GETNOVICETUTORIALREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetNoviceTutorialReward msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetNoviceTutorialAsset().GetNoviceTutorialReward(pkPlayer, msg.m_nNoviceTutorialID);
}


ON_HANDLE(MGPT_REQ_SETMAPBOSSFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_SetMapBossFlag msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetPlayerMapInfoAsset().SetMapBossFlag(pkPlayer, msg.m_serverPlayerBossFlagData);
}

ON_HANDLE(MGPT_REQ_ADDID, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_AddID msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetIDAsset().AddIDByType(pkPlayer, msg.m_nType, msg.m_nValue);
}

ON_HANDLE(MGPT_GET_GAMESTORYBATCH, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_GET_GameStoryBatch msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::GetGameStoryBatch(pkPlayer, msg.m_nResMapID);
}
ON_HANDLE(MGPT_GET_MAPNPCINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_GET_MapNpcInfo msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::NoticeNpcInfoToPlayer(pkPlayer, msg.m_nGameStoryID, msg.m_nResMapID);
}
ON_HANDLE(MGPT_REQ_GETPLAYERSCHEMEDATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetPlayerSchemeData msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	CF_WorldMapList::DataEntry* pkDate = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(msg.m_nMapID);
	if(NULL==pkDate || pkDate->_iSchemeID!=msg.m_nSchemeID || IS_SOLODULE_ROBOT_PLAYERID(msg.m_nCharID) || msg.m_nCharID==pkPlayer->GetInstance())
	{
		return;
	}

	MG_REQ_G2G_GetPlayerSchemeData reqMsg;
	reqMsg.m_nCharID = msg.m_nCharID;
	reqMsg.m_nSystemType = msg.m_nSystemType;
	reqMsg.m_nMapID = msg.m_nMapID;
	reqMsg.m_nSchemeType = msg.m_nSchemeType;
	UT_ServerHelper::GetSchemeLinkID(msg.m_nMapID, reqMsg.m_nSchemeLinkID);
	SendMsgToGroup(MGPT_REQ_G2G_GETPLAYERSCHEMEDATA, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GETSUMMITSINGLEDATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetSummitSingleData rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetSummitGuessAsset().GetSummitSingleData(pkPlayer, rltMsg);
}

ON_HANDLE(MGPT_REQ_GETSUMMITSUPPORTINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetSummitSupportInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetSummitGuessAsset().GetSummitSupportInfo(pkPlayer, rltMsg);
}

ON_HANDLE(MGPT_REQ_SETSUMMITGUESSINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_SetSummitGuessInfo msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetSummitGuessAsset().SetSummitGuessInfo(pkPlayer, msg);
}

ON_HANDLE(MGPT_REQ_GETSUMMITWINER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetSummitWiner msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetSummitGuessAsset().GetSummitWiner(pkPlayer, msg.m_nSummitType, msg.m_nAgainstID);
}


ON_HANDLE(MGPT_REQ_SET_ATVCHARGE_REDPOINT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_Req_SetAtvCharge_RedPoint msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_Resp_SetAtvCharge_RedPoint respMsg;
	pkPlayer->SetAtvChargeRedPointByScheduleId(msg.m_nScheduleId);
	respMsg.m_nResult = success;//
	respMsg.m_nScheduleId = msg.m_nScheduleId;
	pkPlayer->SendMsgToClient(MGPT_RESP_SET_ATVCHARGE_REDPOINT, &respMsg);
}

ON_HANDLE(MGPT_REQ_REALTIMECOMBATRECORDER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_RealtimeCombatRecorder msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::GetRealtimeCombatRecorder(pkPlayer);
}
ON_HANDLE(MGPT_REQ_BONFIREPARTY_BUYLEFTITEMTIMES, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_Resp_BonFireParty_LeftBuyItemTimes rltMsg;

	rltMsg.m_nResult = success;
	do 
	{
		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		if (pkGuildManager == NULL)
		{
			rltMsg.m_nResult = 1;
			break;
		}
		uint64_t nGuildId = pkPlayer->GetGuildID();
		LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(nGuildId);
		if (guild == NULL)
		{
			rltMsg.m_nResult = 2;
			break;
		}
		CF_GuildLevel* guildlevel = CF_GuildLevel::GetSingletonPtr();
		CF_GuildLevel::DataEntry* entry = guildlevel->GetEntryPtr(guild->GetLevel());
		if(entry == NULL)
		{
			rltMsg.m_nResult = 3;
			return;
		}
		
		int nNum = UT_ServerHelper::GetGuildBuyItemTimes(nGuildId);
		rltMsg.m_nLeftTimes = entry->_BonfirSecond - nNum;
		if(rltMsg.m_nLeftTimes < 0)
		{
			rltMsg.m_nLeftTimes = 0;
		}
		rltMsg.m_nRate = (int32_t)UT_ServerHelper::GetGuildMapStateMutableParameter(nGuildId);

	} while (false);

	pkPlayer->SendMsgToClient(MGPT_RESP_BONFIREPARTY_BUYLEFTITEMTIMES, &rltMsg);
}



ON_HANDLE(MGPT_REQ_BONFIREPARTY_BUYLEFTITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_BonFireParty_BuyItem msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if(msg.m_nBuyCount == 0)
	{
		return;
	}
	MG_Resp_BonFireParty_BuyItem rltMsg;
	
	rltMsg.m_nResult = success;
	do 
	{
		if(!UT_ServerHelper::CheckStoryIsActive(GameStoryIDType_GUILDBONFIRPARTY))
		{
			rltMsg.m_nResult = RE_GAMESTORY_UNACTIVE;
			break;
		}
		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		uint64_t nGuildId = pkPlayer->GetGuildID();
		LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(nGuildId);
		if (guild == NULL)
		{
			rltMsg.m_nResult = RE_BUY_BONFIREITEM_GUILDLV_ERROR;
			break;
		}
		CF_GuildLevel* guildlevel = CF_GuildLevel::GetSingletonPtr();
		CF_GuildLevel::DataEntry* entry = guildlevel->GetEntryPtr(guild->GetLevel());
		if(entry == NULL)
		{
			rltMsg.m_nResult = RE_BUY_BONFIREITEM_NO_GUILD;
			break;
		}
		
		int nNum = UT_ServerHelper::GetGuildBuyItemTimes(nGuildId);
		rltMsg.m_nRate = (int32_t)UT_ServerHelper::GetGuildMapStateMutableParameter(nGuildId);
		rltMsg.m_nBuyCount =  entry->_BonfirSecond - nNum;

		if(nNum + msg.m_nBuyCount > entry->_BonfirSecond)
		{
			rltMsg.m_nResult = RE_BUY_BONFIREITEM_NO_TIMES;
			break;
		}
		if (!pkPlayer->IsCashEnough( (MoneyType)(GetGlobalSetting.GuildBonFirePartyCostCount * msg.m_nBuyCount), (CashType)GetGlobalSetting.GuildBonFirePartyCostType) )
		{
			rltMsg.m_nResult = RE_BUY_BONFIREITEM_ITEM_ERROR;
			break;
		}
		//扣货币
		bool flg = pkPlayer->ReduceCash((MoneyType)(GetGlobalSetting.GuildBonFirePartyCostCount * msg.m_nBuyCount), (CashType)(GetGlobalSetting.GuildBonFirePartyCostType), SYS_FUNC_TP_BuyBonFireItemResult, 0);
		if(!flg)
		{
			rltMsg.m_nResult = RE_ACTIVE_STARVEIN_CASH_NOT_ENOUGH;
			break;
		}
		
		UT_ServerHelper::AddGuildBuyItemTimes(nGuildId, msg.m_nBuyCount);
		int nRate = GetGlobalSetting.GuildBonFirePartyExpRate*msg.m_nBuyCount;
		uint32_t nValue = UT_ServerHelper::AddGuildMapStateMutableParameter(nGuildId, nRate);
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(GetGlobalSetting.GuildBonFirePartyBroadcastId);
		noticeFmort.AddString( pkPlayer->GetOwnerCharName() );
		noticeFmort.AddNumber(pkPlayer->GetCommanderTitle());//
		noticeFmort.AddNumber(pkPlayer->GetCurrentCommanderTitleFrameID());//
		noticeFmort.AddNumber(pkPlayer->GetProfilePictureID());//
		noticeFmort.AddNumber(pkPlayer->GetControllerLevel());//
		noticeFmort.AddNumber(nRate/100);//
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort, nGuildId);
		rltMsg.m_nRate = nValue;
		rltMsg.m_nBuyCount =  entry->_BonfirSecond - nNum - msg.m_nBuyCount;
	} while (false);

	pkPlayer->SendMsgToClient(MGPT_RESP_BONFIREPARTY_BUYLEFTITEM, &rltMsg);
}

ON_HANDLE(MGPT_REQ_DAMAGERANKINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetDamageRankInfo msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetDamageRankInfo(msg.m_nMapResID);
}


ON_HANDLE(MGPT_REQ_GETGUILDDAMAGERANKINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetGuildDamageRankInfo msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetGuildDamageRankInfo(msg.m_nMapResID);
}

}