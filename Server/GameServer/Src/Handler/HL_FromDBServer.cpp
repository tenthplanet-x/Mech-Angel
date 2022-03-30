#include "HL_PacketHandler.h"

#include "NW_ServerSession.h"
#include "NW_Helper.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerQueueManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerGuildManager.h"
#include "GameServerApp.h"
#include "LC_ShotActivity.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_SoloDulePlay.h"
#include "oss_360_define.h"
using namespace Protocol;
using namespace GameLogic;
using namespace Utility;

namespace Protocol
{
#define this_source_type SERVER_TYPE_DB

//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_LITE, this_source_type, false)
{
	MG_GD_Load_Char_PB_Lite	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB failed!");
		return;
	}
	object_id_type playerID = Info.player_id;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	{
		pkPlayer->PreLoadCharData_Lite( Info.lite_info );
	}
}
	
ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_BASE, this_source_type, false)
{
	MG_GD_Load_Char_PB_Base	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB_Base failed!");
		return;
	}
	object_id_type playerID = Info.player_id;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	{
		pkPlayer->PreLoadCharData_Base( Info.baseinfo, Info.isEnd );
	}
}

ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_FULL, this_source_type, false)
{
	MG_GD_Load_Char_PB_Full	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB_Full failed!");
		return;
	}
	object_id_type playerID = Info.player_id;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	{
		pkPlayer->PreLoadCharData_Full( Info.fullinfo, Info.isEnd );
	}
}

ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_Pack, this_source_type, false)
{
	MG_GD_Load_Char_PB_Base	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB_Pack failed!");
		return;
	}
	object_id_type playerID = Info.player_id;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	{
		pkPlayer->PreLoadCharData_Pack( Info.baseinfo, Info.isEnd );
	}
}

ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_Development, this_source_type, false)
{
	//MG_GD_Load_Char_PB_Base	Info;
	//if (!Info.Decode(pMsgBody, usBodySize))
	//{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB_Development failed!");
		//return;
	//}
	//object_id_type playerID = Info.player_id;

	//LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	//if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	//{
		//pkPlayer->PreLoadCharData_Development( Info.baseinfo, Info.isEnd );
	//}
}

ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_Hero, this_source_type, false)
{
	MG_GD_Load_Char_PB_Base	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB_Hero failed!");
		return;
	}
	object_id_type playerID = Info.player_id;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	{
		pkPlayer->PreLoadCharData_Hero( Info.baseinfo, Info.isEnd );
	}
}

ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_PlayerBase, this_source_type, false)
{
	MG_GD_Load_Char_PB_Base	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB_PlayerBase failed!");
		return;
	}
	object_id_type playerID = Info.player_id;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	{
		pkPlayer->PreLoadCharData_PlayerBase( Info.baseinfo, Info.isEnd );
	}
}

ON_HANDLE(MGPT_GD_LOAD_CHAR_PB_Offline, this_source_type, false)
{
	MG_GD_Load_Char_PB_Base	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Load_Char_PB_Offline failed!");
		return;
	}
	object_id_type playerID = Info.player_id;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(playerID));
	if (NULL != pkPlayer && pkPlayer->GetID() == playerID)
	{
		pkPlayer->PreLoadCharData_Offline( Info.baseinfo, Info.isEnd );
	}
}

ON_HANDLE(MGPT_GD_SAVE_CHAR_PB_RESULT, this_source_type, false)
{
	MG_GD_Save_Char_PB_Result	Info;
	if (!Info.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_GD_Save_Char_PB_Result failed!");
		return;
	}

	const unique_id_impl& instance = Info.instance;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MG_GD_Save_Char_PB_Result serverid:[%d], entryid:[%d], c_cid:[%d], c_citizenship:[%d], saveflag:[%d], nReason:[%d]",
		GetGameServerID(), GetGameServerEntryID(), instance.detail(), instance.catagory(), Info.bFlag, Info.nReason);
	
	if(RE_SERVER_KICK_USER_NEW_LOGIN == Info.nReason)
	{
		SendMsgToLogin(MGPT_SS_SAVE_CHAR_RESULT, &Info, 0, 0);
	}
	else if(RE_SWITCH_MAP_LINE_SAVE == Info.nReason)
	{
		//切节点 强行保证 gs->db db->gs gs->login 玩家离线，login ->其他gs 加载
		LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		enterLeaveManager->_notifyOtherServerUserReset( Info.userId, Info.instance, 0);
	}
}

ON_HANDLE(MGPT_GD_RES_ALL_CHAR_INFO, this_source_type, false)
{
	MG_GD_Res_Char_All_Info res;
	if (!res.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(res.player_id));
	if(NULL == pkPlayer)
	{
		return;
	}

	const unique_id_impl& instance = pkPlayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d][LOGIN STEP 1] data from dbsrv! c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d", 
		UT_ServerHelper::GetServerId(), instance.detail(), instance.catagory(), pkPlayer->GetUserID(), res.player_id);

	if (pkPlayer->GetID() == res.player_id)
	{
		bool clonePlayer = IS_PLAYER_CLONE_ID(res.player_id);
		pkPlayer->ParseCharDataFromPB(clonePlayer);
	}

	if (pkPlayer->GetID() == res.player_id && pkPlayer->GetLoadDataSuccess())
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d][LOGIN STEP 2] get all info from db! entryid:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d",
			UT_ServerHelper::GetServerId(), GetGameServerEntryID(), instance.detail(), instance.catagory(), pkPlayer->GetUserID(), res.player_id);

		//发送角色信息（因为已经全部load完)
		pkPlayer->SetLoadDataFromDBFlag(true);
		if (LC_ServerPlayer::SERVER_STATE_LOGINING == pkPlayer->GetServerViewState())
		{
			LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
			enterLeaveManager->LoadDataFromDB(pkPlayer, true);
		}
		else
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "c_cid[%d], c_citizenship[%d], c_uid[%d] enter game state error, state[%i], char type[%i]",
				instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkPlayer->GetServerViewState(), pkPlayer->GetCharType());
		}
	}
}

ON_HANDLE(MGPT_SS_NO_ROLE_DATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(rkMsgHead.m_uiCharID));
	if (NULL != pkPlayer)
	{
		LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		enterLeaveManager->LoadDataFromDB(pkPlayer, false);
	}
}


ON_HANDLE(MGPT_GL_RESP_PLAYER_DETAIL, this_source_type, false)
{
	MG_GG_Resp_PlayerPBDetail kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer && kRltBody.m_CharID != 0)
	{
		kRltBody.m_nUserInfo.m_nBuddyRankIndex.clear();
		for(std::map<int32_t,PS_Buddy_Info_BuddyCS>::iterator it=kRltBody.m_nUserInfo.m_BuddyInfo.begin(); it!=kRltBody.m_nUserInfo.m_BuddyInfo.end(); ++it)
		{
		}
		pkPlayer->SendMsgToClient(MGPT_RESP_PLAYER_DETAIL, &kRltBody);
	}
}


ON_HANDLE(MGPT_GG_RESP_GUILD_FIGHT_PLAYER_INFO, this_source_type, false)
{
	MG_GG_Resp_GuildFightPlayerInfo kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
		return;
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer)
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_PLAYER_INFO, &kRltBody);
	
}
//离线数据 返回

ON_HANDLE(MGPT_GG_RLT_OFFLINE_PLAYER_FROM_DB, this_source_type, false)
{
	MG_GG_RLT_OfflinePlayerFromDB kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
		return;
	if(0  == kRltBody.m_nUserInfo.m_CharID)
		return;

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (NULL == pkPlayer)
		return;
	pkPlayer->NotifyPkInfo(kRltBody.m_nUserInfo);
	
}
//
ON_HANDLE(MGPT_GG_RLT_GUILD_SCORE_NOTIFY_GS, this_source_type, false)
{
	MG_GG_RLT_Guild_Score_Notify kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
		return;
	if(0  == kRltBody.m_uCharId)
		return;
	
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltBody.m_uCharId);
	if (NULL != pkPlayer)
	{
		if(kRltBody.m_bSettle)
		{
			pkPlayer->SetLastAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_GUILD,ROIT_Total);
			pkPlayer->UpdateGuildFightScore( kRltBody.m_score);
		}
	}
	//同步 society服务器
	UT_ServerHelper::ReqGuildFightInfoToSociety(UpdateGuildFightType::UGFT_UPDATE_POINT,kRltBody.m_msgUniqueKey,kRltBody.m_uCharId,kRltBody.m_bSettle,0,0,kRltBody.m_score);
}
//
ON_HANDLE(MGPT_GG_RLT_TIMER_ADD_SCORE, this_source_type, false)
{
	MG_GG_RLT_Timer_Add_Score kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
		return;
	//kRltBody.m_uCacheId
	UT_ServerHelper::SetEraseRewardSendState(kRltBody.m_uCacheId);
}


//
ON_HANDLE(MGPT_GG_RLT_GUILD_FIGHT_SETTLE, this_source_type, false)
{
	MG_GG_RLT_Guild_Fight_Settle kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
		return;
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltBody.m_uWinnerId);
	if (NULL == pkPlayer)
		return;
	
	pkPlayer->SetBestGuildSettleAccountPoint(kRltBody.m_uScore);

	pkPlayer->AddGoalAccountByType(LC_GOAL_TYPE_SNATCH_ACCOUNT_POINT, kRltBody.m_uScore, pkPlayer->GetMapResID());
	// Settle_GuildOffline  异步到 society 更新完 公会排行榜 在进行结算
}



//
ON_HANDLE(MGPT_GD_RLT_CHANGE_CHAR_NAME, this_source_type, false)
{
	MG_PB<msg_define::change_name_try_resp> tryRespMsg;
	if (!tryRespMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	msg_define::change_name_try_resp& tryRespVal = tryRespMsg.m_value;

	MG_PB<msg_define::change_name_resp> respMsg;
	msg_define::change_name_resp& respVal = respMsg.m_value;

	int32_t result = tryRespVal.result();
	do {
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		if (NULL == pkPackAsset)
		{
			result = RE_FAIL;
			break;
		}
		LC_BackPackEntry* pkCardEntry = pkPackAsset->GetBackPackEntry(tryRespVal.pack_type(), tryRespVal.pack_index());
		if (NULL == pkCardEntry)
		{
			result = RE_FAIL;
			break;
		}
		pkCardEntry->SetBackPackValueByType(LPSFET_LOCKED, false);
		if (pkCardEntry->GetBackPackValueByType(LPSFET_TYPE_ID) != SPEC_ITEM_TID_RENAME_CARD)
		{
			result = RE_FAIL;
			break;
		}

		if (RE_SUCCESS != result)
		{
			break;
		}

		PackOperateInfoList kItemOPs;
		PackOperateInfo objData;
		objData.Reset();
		objData.m_iOPType = OPT_DELETE_ITEM_BY_INDEX;
		objData.m_bOPEntry = false;
		objData.m_iPackType = tryRespVal.pack_type();
		objData.m_iIndex = tryRespVal.pack_index();
		objData.m_lCount = 1;
		kItemOPs.push_back(objData);
		result = LC_Helper::CheckItemOPs(pkPackAsset, kItemOPs);
		if (RE_SUCCESS != result)
		{
			break;
		}

		//Sync char name
		result = LC_Helper::DoItemOPs(pkPlayer, kItemOPs, SYS_FUNC_TP_RENAME, 0);
		if (RE_SUCCESS != result)
		{
			break;
		}

		StringType sNewName = STDSTR2TPSTR(tryRespVal.new_name());
		pkPlayer->SetCharName(sNewName);
		respVal.set_char_name(TPSTR2STDSTR(pkPlayer->GetOwnerCharName()));

		if (pkPlayer->GetLoadDataFromDBFlag())
		{
			pkPlayer->SetCharInfoDirty();
		}
		oss_360_log_change_name(pkPlayer);

		//通知好友
		pkPlayer->BroadcastChangeName();

		pkPlayer->UpdateGuildMemberInfo(true, true);
		pkPlayer->UpdatePlayerBaseDataToGroup();
		
		LC_ServerLogicManager* logicMgr = SERVER_GET_LOGIC_MANAGER();
		LC_WorldGuildWarAsset* pWorldGuildWarAsset = logicMgr->GetGuildWarAssert();
		if(pWorldGuildWarAsset == NULL)
		{
			break;
		}
		pWorldGuildWarAsset->ChangePowerGuildPlayerName(pkPlayer->GetActiveServer(), pkPlayer->GetInstance(), tryRespVal.new_name());
		pWorldGuildWarAsset->ChangeGlobalPowerGuildPlayerName(pkPlayer->GetInstance(), tryRespVal.new_name());
		pkPlayer->SyncWorldPowerGuild();
		pkPlayer->SaveCharInfoToFriendDB(true);
	} while (0);
	if (RE_SUCCESS == tryRespVal.result() && RE_SUCCESS != result)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "change name success, but consume rename card failed ! ! !", pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}

	respVal.set_result(result);
	pkPlayer->SendMsgToClient(MGPT_RLT_CHANGE_CHAR_NAME, &respMsg);
	UT_ServerHelper::UpdatePlayerInfoGuildName(pkPlayer->GetInstance(),pkPlayer->GetOwnerCharName().c_str(),pkPlayer->GetGuildName().c_str(),0,0,pkPlayer->GetCommanderTitleID());
}

}
