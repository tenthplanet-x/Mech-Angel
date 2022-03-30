#include "LM_LocaleManager.h"
#include "GameServerApp.h"
#include "MG_TYPES.h"
#include "HL_PacketHandler.h"
#include "TS_TranManager.h"
#include "TS_TranType.h"
#include "LC_ServerGuildManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "MG_Game2Guild.h"
#include "MG_CS_Guild.h"
#include "MG_CS_GuildHome.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerWarManager.h"
#include "NW_Helper.h"
#include "NW_ServerSession.h"
#include "CF_GuildLevel.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "UT_ServerHelper.h"
#include "LC_CrossRealmRaidManager.h"
#include "LC_GameStory_Manager.h"
#include "oss_define.h"
#include "MG_Common.h"
#include "LC_ServerLogicManager.h"
#include "LC_GameStory_Manager.h"
#include "LC_ServerTools.h"
#include "LC_ServerGuildWarAsset.h"
#include "ConfigManager.h"
#include "CF_BestGuildBossInfo.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_SoloDulePlay.h"
#include "UT_OperateLog.h"
#include "gsnode.h"
using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_SOCIETY

enum GuildHomeOP
{
	UPDATE_BUIDING,
	SHOPPING,
	DONATE,
	PUTWAREHOUSE,
	SWAPWAREHOUSE,
	GETWAREHOUSEINFO,
	GET_KILLTHIEF_PRIZE,
};

#define		TIMER_FOR_GUILD_WAR_MSG		1.2

ON_HANDLE(MGPT_GL_SERVER_INIT_RTL, SERVER_TYPE_SOCIETY, false)
{
	SERVER_GET_GAMESTORY_MANAGER->OnSocietyServerConnected();
}

ON_HANDLE(MGPT_GGU_RLT_CREATE_GUILD, this_source_type, false)
{
	MG_SS_RLT_Create_Guild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	// 如果能找到玩家，说明公会创建是本节点发起的

	if (kMsg.m_uiResult != RE_SUCCESS)
	{
		if (pkPlayer == NULL)	return;	// 非本节点的创建失败
		MG_RLT_CreateGuild kRltMsg;
		kRltMsg.m_uiCharID = pkPlayer->GetInstance();
		kRltMsg.m_uiResult = kMsg.m_uiResult;
		pkPlayer->SendMsgToClient(MGPT_RLT_CREATE_GUILD, &kRltMsg);
		return;
	}
	// 其它节点创建的公会，也要在本节点建立信息
	char_data_define::guild_info guildinfo;
	guildinfo.ParseFromString(kMsg.m_kGuildInfo);
	pkGuildManager->UpdateGuildInfo(guildinfo.guild_id(), true, guildinfo, unique_id_impl(rkMsgHead.m_uiCharID), MGPT_REQ_CREATE_GUILD);

	// 本节点创建的公会，才能把创建消息广播给其它服务器
	if (pkPlayer == NULL)	return;

	StringType rName = guildinfo.guild_name().c_str();
	oss_party_build(pkPlayer, guildinfo.guild_id(), CodeConvert::MbsToUtf8(rName), guildinfo.guild_level(), 0, 0);

	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	noticeFmort.SetMsgID(35);
	noticeFmort.AddPlayerInfo(pkPlayer->GetInstanceUniqueID(), pkPlayer->GetOwnerCharName());
	noticeFmort.AddString(rName);
	noticeFmort.AddUInt64Number(guildinfo.guild_id());
	noticeFmort.AddNumber(guildinfo.guild_level());
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort);

	pkPlayer->SaveCharPB(false);
	//新创建公会 同步个人 公会战积分
	std::vector<uint64_t> vecId;
	vecId.push_back(pkPlayer->GetInstance());
	UT_ServerHelper::ReqPlayerOfflineToDb(OfflinePlayerFromDBType::OPFDT_GUILD_FIGHT_UPDATE_SCORE,vecId,0,0,0,pkPlayer->GetUserID(),pkPlayer->GetInstance());
	// 更新redis 里的副本 后期在优化， redis 中 应该只存一份玩家所有系统模块的副本。 由各个模块变动去更新相关东西。 不应该跟其他业务有瓜葛
	UT_ServerHelper::UpdatePlayerInfoGuildName(pkPlayer->GetInstance(),pkPlayer->GetOwnerCharName().c_str(),guildinfo.guild_name(),pkPlayer->GetInstance(),guildinfo.guild_id(),pkPlayer->GetCommanderTitleID());
}

ON_HANDLE(MGPT_GGU_RLT_MODITY_GUILD_DECLARATION, this_source_type, false)
{
	MG_SS_RLT_Set_Declaration kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (NULL == guild)
	{
		return;
	}
	guild->ModifyDeclarationRlt(pkPlayer, kMsg.m_uiStatus);
}

ON_HANDLE(MGPT_GGU_RLT_GUILD_KICKMEMBER, this_source_type, false)
{
	MG_SS_RLT_Guild_KickMember kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	
	guild->KickMemberRlt(pkPlayer, kMsg.m_uiResult);
}

ON_HANDLE(MGPT_GGU_RLT_FASTJOIN_GUILD, this_source_type, false)
{
	MG_SS_RLT_FastJoin_Guild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	MG_RLT_FastApplyGuild kRltMsg;
	if(RE_SUCCESS != kMsg.m_uiStatus)
	{		
		kRltMsg.m_nResult = kMsg.m_uiStatus;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_FASTAPPLYGUILD, &kRltMsg);
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		kRltMsg.m_nResult = RE_FAIL;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_FASTAPPLYGUILD, &kRltMsg);
		return;
	}

	mem::vector<int32_t> indexVec;
	for(int i=0; i<kMsg.m_GuildBaseInfo.size(); ++i)
	{
		int level = kMsg.m_GuildBaseInfo[i].usLevel;
		int memberNum = kMsg.m_GuildBaseInfo[i].memberNums;
		int guildID = kMsg.m_GuildBaseInfo[i].uiGuildID;
		CF_GuildLevel::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_GuildLevel>(level);
		if(pkEntry==NULL || memberNum>=pkEntry->_MaxPlayer)
		{
			continue;
		}

		indexVec.push_back(i);
	}

	int size = indexVec.size();
	if(size > 0)
	{
		int index = indexVec[UT_MathBase::RandInRangeInt(0, size-1)];
		pkGuildManager->Add2Guild(pkPlayer, kMsg.m_GuildBaseInfo[index].uiGuildID, kMsg.m_GuildBaseInfo[index].usLevel);
		return;
	}

	kRltMsg.m_nResult = RE_APPLY_GUILD_FAIL_MEMBER_FULL;
	SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_FASTAPPLYGUILD, &kRltMsg);
	return;
}

ON_HANDLE(MGPT_GGU_RLT_JOIN_GUILD, this_source_type, false)
{
	MG_SS_RLT_Join_Guild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	if(false == kMsg.m_bAutoAdd)
	{
		MG_RLT_ApplyGuild kRltApplyGuild;
		kRltApplyGuild.m_uiGuildID	= kMsg.m_uiGuildID;
		kRltApplyGuild.m_uiResult	= kMsg.m_uiStatus;
		kRltApplyGuild.m_uiCharID	= pkPlayer->GetInstance();
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), kRltApplyGuild.m_szName);
		kRltApplyGuild.m_guildname = kMsg.m_guildName;
		pkPlayer->SendMsgToClient(MGPT_RLT_APPLY_GUILD, &kRltApplyGuild); // 同申请加入
	}	

	if(RE_SUCCESS == kMsg.m_uiStatus)
	{
		pkPlayer->SaveCharPB(false);
		//新成员加入公会  同步个人 公会战积分
		std::vector<uint64_t> vecId;
		vecId.push_back(pkPlayer->GetInstance());
		UT_ServerHelper::ReqPlayerOfflineToDb(OfflinePlayerFromDBType::OPFDT_GUILD_FIGHT_UPDATE_SCORE,vecId,0,0,0,pkPlayer->GetUserID(),pkPlayer->GetInstance());
		UT_ServerHelper::UpdatePlayerInfoGuildName(pkPlayer->GetInstance(),pkPlayer->GetOwnerCharName().c_str(),kMsg.m_guildName,pkPlayer->GetInstance(),kMsg.m_uiGuildID,pkPlayer->GetCommanderTitleID());
	}
}

ON_HANDLE(MGPT_GGU_RLT_GUILD_CONFIRM_JOIN, this_source_type, false)
{
	MG_SS_RLT_Handle_Applicant kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	vector<uint64_t> nParams;
	nParams.clear();
	nParams.push_back(kMsg.m_uiApplicantCharID);
	nParams.push_back(kMsg.m_uiStatus);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILD_OPERATE,OPERATE_LOGTYPE11);
	RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode,nParams);

	guild->ApplyResultRlt(pkPlayer, kMsg.m_uiStatus, kMsg.m_uiApplicantCharID);
	pkPlayer->SaveCharPB(false);
	// 同步个人 公会战积分
	std::vector<uint64_t> vecId;
	vecId.push_back(pkPlayer->GetInstance());
	UT_ServerHelper::ReqPlayerOfflineToDb(OfflinePlayerFromDBType::OPFDT_GUILD_FIGHT_UPDATE_SCORE,vecId,0,0,0,pkPlayer->GetUserID(),pkPlayer->GetInstance());
	UT_ServerHelper::UpdatePlayerInfoGuildName(pkPlayer->GetInstance(),pkPlayer->GetOwnerCharName().c_str(), TPSTR2STDSTR(guild->GetGuildName()), pkPlayer->GetInstance(),kMsg.m_uiGuildID,pkPlayer->GetCommanderTitleID());
}

ON_HANDLE(MGPT_GGU_RLT_EXIT_GUILD, this_source_type, false)
{
	MG_SS_RLT_Quit_Guild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->ExitGuildRlt(pkPlayer, kMsg.m_uiResult);
}

ON_HANDLE(MGPT_GGU_RLT_GUILD_ASSET, this_source_type, false)
{
	MG_SS_RLT_Guild_Info kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	
	char_data_define::guild_info guildinfo;
	guildinfo.ParseFromString(kMsg.m_guildInfo);
	LC_ServerGuildInfo* pGuild = pkGuildManager->PlayerGuildInfo(rkMsgHead.m_uiCharID, kMsg.m_bNewAdd, kMsg.m_Result, guildinfo);
	if(pGuild != NULL)
	{
		pGuild->SetDirtyTime(kMsg.m_GuildDirtyTime);
	}
	
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if(NULL == player)
	{
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "get_guildinfo_from_society serverId:%d, entryID:%d, uid:%d r1", GetGameServerID(), GetGameServerEntryID(), rkMsgHead.m_uiCharID);
		return;
	}

	player->SetbLoadFromSocietyFlag(true);
	player->SyncActiviteBuddyProperty();
	player->SyncSocietyToClient();
}

ON_HANDLE(MGPT_GG_SENDGUILDMEMBERSTATE, this_source_type, false)
{
	MG_SS_SendGuildMemberState kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kMsg.m_nGuildID);
	if (guild == NULL)
	{
		return;
	}

	guild->UpdateSendGuildMemberState(kMsg.m_nSendState, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_GG_PLAYERGUILDSCORE, this_source_type, false)
{
	MG_SS_PlayerGuildScore kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kMsg.m_nGuildID);
	if (guild == NULL)
	{
		return;
	}

	guild->UpdatePlayerGuildScore(kMsg.m_nGuildScore, rkMsgHead.m_uiCharID);
}


ON_HANDLE(MGPT_GGU_RLT_GUILD_MEMBER, this_source_type, false)
{
	MG_SS_RLT_GuildMember kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kMsg.m_nGuildID);
	if (guild == NULL)
	{
		return;
	}
	guild->UpdateMembersInfo(kMsg.m_kMemberInfo, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_GGU_RLT_DISMISS_GUILD, this_source_type, false)
{
	MG_SS_RLT_Dissmiss_Guild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	vector<uint64_t> nParams;
	nParams.clear();
	nParams.push_back(pkPlayer->GetGuildID());
	nParams.push_back(kMsg.m_Result);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILD_OPERATE,OPERATE_LOGTYPE9);
	RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode,nParams);
	guild->DismissRlt(pkPlayer, kMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_RLT_GUILD_CLEAR, this_source_type, false)
{
	MG_SS_RLT_ClearGuildApply kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->ClearApplyRlt(pkPlayer, kMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_RLT_GUILD_APPOINT, this_source_type, false)
{
	MG_SS_RLT_Guild_Promote kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->AppointRlt(pkPlayer, kMsg.m_uiStatus);
}

ON_HANDLE(MGPT_GGU_RLT_POWERGUILD_APPOINT, this_source_type, false)
{
	MG_SS_Appoint_Power_RLT kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	if (kMsg.m_Result == RE_SUCCESS)
	{
		LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kMsg.m_TargetCharID);
		if (pkTargetPlayer)
		{
			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			if (kMsg.m_Position == 1 || kMsg.m_Position == 2)
			{
				noticeFmort.SetMsgID(41);
			}
			else
			{
				noticeFmort.SetMsgID(42);
			}
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
		}
	}
	guild->AppointPowerRlt(pkPlayer, kMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_RLT_DONATE, this_source_type, false)
{
	MG_SS_RLT_Donate kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->DoNateRlt(pkPlayer, kRltMsg.Result);
}

ON_HANDLE(MGPT_GGU_RLT_GUILDLIST, this_source_type, false)
{
	MG_SS_RLT_GuildList kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	//gm async req
	TS_TranBase* pkTran = pkTranManager->GetTran(rkMsgHead.m_iSequence);
	if (pkTran)
	{
		pkTran->HandlerMsg(rkMsgHead, &kRltMsg);
		pkTranManager->UnRegisteTran(pkTran->GetID());
	}
	else
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		if (!pkPlayerManager)
		{
			return;
		}

		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (!pkPlayer)
		{
			return;
		}
		pkPlayer->SendMsgToClient(MGPT_RLT_SEARCH_GUILD, &kRltMsg);
	}
}

ON_HANDLE(MGPT_GGU_RLT_GUILDRANK, this_source_type, false)
{
	MG_SS_RLT_GuildRank kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	//gm async req
	TS_TranBase* pkTran = pkTranManager->GetTran(rkMsgHead.m_iSequence);
	if (pkTran)
	{
		pkTran->HandlerMsg(rkMsgHead, &kRltMsg);
		pkTranManager->UnRegisteTran(pkTran->GetID());
	}
	else
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		if (!pkPlayerManager)
		{
			return;
		}

		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (!pkPlayer)
		{
			return;
		}

		kRltMsg.m_iResult = RE_SUCCESS;
		pkPlayer->SendMsgToClient(MGPT_RLT_SEARCH_GUILD_RANK, &kRltMsg);
	}
}


ON_HANDLE(MGPT_GGU_RLT_RANDOM_GUILD_NAME, this_source_type, false)
{
	MG_SS_RLT_RandomGuildName kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	//gm async req
	TS_TranBase* pkTran = pkTranManager->GetTran(rkMsgHead.m_iSequence);
	if (pkTran)
	{
		pkTran->HandlerMsg(rkMsgHead, &kRltMsg);
		pkTranManager->UnRegisteTran(pkTran->GetID());
	}
}

ON_HANDLE(MGPT_GGU_NOTIFY_BASEINFO, this_source_type, false)
{
	MG_SS_Notify_BaseInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guild == NULL)
	{
		GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "MG_SS_Notify_BaseInfo m_GuildId:%d r1", kRltMsg.m_GuildId);
		return;
	}
	char_data_define::guild_info info;
	info.ParseFromString(kRltMsg.m_BaseInfo);
	guild->UpdateBasicInfo(false, info, kRltMsg.m_OperatorCharID, kRltMsg.m_Reason);
	guild->SetDirtyTime(kRltMsg.m_GuildDirtyTime);

	//升级
	if (kRltMsg.m_Reason == MGPT_REQ_UPGRADE_GUILD)
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		if (pkPlayerManager != NULL)
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltMsg.m_OperatorCharID);
			if (pkPlayer != NULL)
			{
				int nCount = 0;
				LC_Guild_Assert_Entry* pEntry = guild->GetGuildBasicInfo().GetGuildAssert(0);
				if (pEntry != NULL)
				{
					nCount = (int)pEntry->m_Assert_Count;
				}
				vector<uint64_t> nParams;
				nParams.clear();
				nParams.push_back(guild->GetLevel());
				int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILD_OPERATE, OPERATE_LOGTYPE4);
				RecordOperateLog(pkPlayer, BASE_OPERATE,nLogCode,nParams);
				oss_party_levelup(pkPlayer, guild->GetId(), guild->GetName(), guild->GetLevel(), nCount, 0);
			}
		}
	}
}

ON_HANDLE(MGPT_GGU_NOTIFY_ADD2GUILD, this_source_type, false)
{
	MG_SS_Notify_AddGuild kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltMsg.m_TargetCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	SendMsgToSociety(MGPT_SS_ROLE_ENTER_GAME, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance(), pkPlayer->GetCitizenship(), GetGateWayServerID(pkPlayer));
	LC_ServerGuildInfo*	pGuild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if(pGuild == NULL || pGuild->GetDirtyTime() != kRltMsg.m_GuildDirtyTime)
	{
		pkPlayer->GetGuildDataFromDB(kRltMsg.m_GuildId, true);
	}
}

ON_HANDLE(MGPT_GGU_NOTIFY_ADD_MEMBER, this_source_type, false)
{
	MG_SS_Notify_Guild_MemberAdd kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	char_data_define::guild_member_info info;
	info.ParseFromString(kRltMsg.m_MemberInfo);
	 uint64_t _uiCharID = info.userinfo().userid();
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkOperatorPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(kRltMsg.m_OperatorCharID));

	// MGPT_GGU_NOTIFY_ADD_MEMBER 这个消息所有节点都能收到，society会广播
	// 发起方在本服，调用UT_ServerHelper::BrocastNoticeMsg进行通知，该函数会发消息给society，society会通知其它节点
	// 这样的话，只有发起方的GS收到MGPT_GGU_NOTIFY_ADD_MEMBER后，然后才会通知society，否则就是本服的所有节点都会通知society，通知消息会变成NxN份
	if(pkOperatorPlayer != NULL )	// UT_ServerHelper::CheckLocal(charID.catagory()) == true)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(5);
		noticeFmort.AddString(info.userinfo().user_name().c_str());
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort, kRltMsg.m_GuildId);
	}
	
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guild == NULL)
	{
		{
			MG_Notify_Add2Guild addmsg;
			addmsg.m_uiCharID = info.userinfo().userid();
			LC_ServerPlayer* player = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(addmsg.m_uiCharID));
			if (player != NULL)
			{
				player->SendMsgToClient(MGPT_NOTIFY_ADD2GUILD, &addmsg);
				UT_ServerHelper::UpdatePlayerInfoGuildName(player->GetInstance(),player->GetOwnerCharName().c_str(), kRltMsg.m_sGuildName, player->GetInstance(),kRltMsg.m_GuildId,player->GetCommanderTitle());
			}
		}
	}
	else
	{
		guild->UpdateMemberAdd(info, kRltMsg.m_OperatorCharID, kRltMsg.m_Reason);
		LC_ServerPlayer* player = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(_uiCharID));
		if (player != NULL)
		{
			UT_ServerHelper::UpdatePlayerInfoGuildName(_uiCharID,player->GetOwnerCharName().c_str(),kRltMsg.m_sGuildName,player->GetInstance(),kRltMsg.m_GuildId,player->GetCommanderTitle());
		}
	}
	
}

ON_HANDLE(MGPT_GGU_NOTIFY_DEL_MEMBER, this_source_type, false)
{
	MG_SS_Notify_Guild_MemberLeave kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	pkGuildManager->GuildMemberLeave(kRltMsg.m_guildid, kRltMsg.m_uiTargetCharID, kRltMsg.m_Reason, kRltMsg.m_OperatorCharID);
}

ON_HANDLE(MGPT_GGU_NOTIFY_ADD_APPLY, this_source_type, false)
{
	MG_SS_Notify_Add_Apply kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_guildid);
	if (guild == NULL)
	{
		return;
	}
	char_data_define::guild_apply_info info;
	info.ParseFromString(kRltMsg.m_UserInfo);
	guild->UpdateApplyInfoAdd(info);
}

ON_HANDLE(MGPT_GGU_RLT_GUILD_APPLYINFO, this_source_type, false)
{
	MG_SS_RLT_GuildApplyInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}

	char_data_define::guild_applys info;
	info.ParseFromString(kRltMsg.m_ApplyInfo);
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(info.guild_id());
	if (guild == NULL)
	{
		return;
	}
	guild->UpdateApplyInfo(info, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_GGU_NOTIFY_DEL_APPLY, this_source_type, false)
{
	MG_SS_Notify_Del_Apply kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_guildid);
	if (guild == NULL)
	{
		return;
	}
	guild->UpdateApplyInfoDel(kRltMsg.m_TargetCharID);
}

ON_HANDLE(MGPT_GGU_NOTIFY_POSITION, this_source_type, false)
{
	MG_SS_Notify_PositionChange kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_Guildid);
	if (guild == NULL)
	{
		return;
	}
	char_data_define::guild_member_position info;
	info.ParseFromString(kRltMsg.m_PositionChange);
	guild->UpdatePositionChange(info, kRltMsg.m_OperatorCharID);
}

ON_HANDLE(MGPT_GGU_NOTIFY_UPDATE_MEMBER_INFO, this_source_type, false)
{
	MG_SS_Notify_UPDATE_MEMBER_INFO kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildID);
	if (guild == NULL)
	{
		return;
	}
	char_data_define::guild_member_info info;
	info.ParseFromString(kRltMsg.m_MemberInfo);
	guild->UpdateMemberInfo(info);
}

ON_HANDLE(MGPT_GGU_NOTIFY_CLEAR_APPLY, this_source_type, false)
{
	MG_SS_Notify_ClearApply kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guild == NULL)
	{
		return;
	}
	guild->UpdateApplyInfoClear(kRltMsg.m_OperatorCharID);
}

ON_HANDLE(MGPT_GGU_NOTIFY_DISMISS, this_source_type, false)
{
	MG_SS_Notify_Dissmiss_Guild kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_uiGuildID);
	if (guild == NULL)
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager != NULL)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltMsg.m_OperatorCharID);
		if (pkPlayer != NULL)
		{
			oss_party_dismiss(pkPlayer, kRltMsg.m_uiGuildID, guild);
		}
	}
	pkGuildManager->DissmissGuild(kRltMsg.m_uiGuildID, kRltMsg.m_OperatorCharID);
}

ON_HANDLE(MGPT_GGU_RLT_PRECREATE_GUILD, this_source_type, false)
{
	MG_SS_RLT_PreCreate_Guild kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return;
	}

	CF_GuildCreateLimit::DataEntry* pCreateCfg = pCreateCfgMgr->getLimit();
	if(pCreateCfg == NULL)
	{
		return;
	}
	//创建工会--埋点日志
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILD_OPERATE, OPERATE_LOGTYPE1);
	if (RE_SUCCESS == kRltMsg.m_uiResult)
	{
		if (kRltMsg.m_CostType == 0)
		{
			if (!pkPlayer->ReduceCash(pCreateCfg->_iCash, CT_UNBIND_CASH, SYS_FUNC_TP_GUILD_CREATE, 0,nLogCode))
			{
				return;
			}
		}
		else
		{
			if (!pkPlayer->ReduceCash(pCreateCfg->_iYuanBao, CT_UNBIND_YUANBAO, SYS_FUNC_TP_GUILD_CREATE, 0, nLogCode))
			{
				return;
			}
		}

		MG_SS_REQ_Create_Guild kSGMsg;
		CF_GuildLevel* guildlevel = CF_GuildLevel::GetSingletonPtr();
		CF_GuildLevel::DataEntry* entry = guildlevel->GetEntryPtr(1);
		if (entry == NULL)
		{
			return;
		}
		kSGMsg.m_MaxPlayer = entry->_MaxPlayer;
		char_data_define::guild_user_info userinfo;
		LC_ServerGuildManager::GetSingletonPtr()->FillPlayInfo(pkPlayer, userinfo);
		userinfo.SerializeToString(&kSGMsg.m_userInfo);
		//STRNCPY( kSGMsg.m_szCharName, pkPlayer->GetCharName().c_str(), sizeof(kSGMsg.m_szGuildName));
		kSGMsg.m_szGuildName = kRltMsg.m_szGuildName;
		kSGMsg.m_GuildDeclaration = kRltMsg.m_szGuildDeclaration;
		SendMsgToSociety(MGPT_GGU_REQ_CREATE_GUILD, &kSGMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		const unique_id_impl& instance = pkPlayer->GetInstance();
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "player try to create a guild! c_cid:%d, c_citizenshipID:%d, c_uid:%d", instance.detail(), instance.catagory(), pkPlayer->GetUserID());
	}
	else
	{
		MG_RLT_CreateGuild kCSmsg;
		kCSmsg.m_uiCharID = pkPlayer->GetInstance();
		kCSmsg.m_uiResult = kRltMsg.m_uiResult;
		pkPlayer->SendMsgToClient(MGPT_RLT_CREATE_GUILD, &kCSmsg);
	}
}

ON_HANDLE(MGPT_GGU_NOTIFY_ONLINE, this_source_type, false)
{
	MG_SS_Notify_Online kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guild == NULL)
	{
		return;
	}
	//int serverid = GetGameServerEntryID();
	//guild->UpdateMemberOnline(kRltMsg.m_CharID, serverid, kRltMsg.m_ServerType, kRltMsg.m_LastTime, kRltMsg.m_Online);
	guild->UpdateMemberOnline(kRltMsg.m_CharID, kRltMsg.m_ServerId, kRltMsg.m_ServerType, kRltMsg.m_LastTime, kRltMsg.m_Online);
}

ON_HANDLE(MGPT_GGU_RLT_HANDLE_ASSERT, this_source_type, false)
{
	MG_SS_GuildHandleAssert_RESP kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	pkGuildManager->GuildAssertTransactionRLT(kRltMsg);
}

ON_HANDLE(MGPT_GGU_RLT_POWERGUILD_REWARD_CHECK, this_source_type, false)
{
	MG_SS_Power_Reward_Check_RLT kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
	if (guildinfo == NULL)
	{
		return;
	}
	guildinfo->PowerGuildRewardCheckRlt(pkPlayer, kRltMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_POWERGUILD_REWARD_RESP, this_source_type, false)
{
	MG_SS_Power_Reward_Check_RLT kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
	if (guildinfo == NULL)
	{
		return;
	}
	guildinfo->PowerGuildRewardGain(pkPlayer, kRltMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_RLT_POWERGUILD_REWARD_TIME, this_source_type, false)
{
	MG_SS_Power_Reward_Time_RLT kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
	if (guildinfo == NULL)
	{
		return;
	}
	MG_Power_Reward_Time_RLT rlt;
	rlt.m_Type = kRltMsg.m_Type;
	rlt.m_TimeLocal = kRltMsg.m_TimeLocal;
	rlt.m_TimeLocalStamp = kRltMsg.m_TimeLocalStamp;

	ResultType ret = guildinfo->PowerGuildRewardTimeGain(pkPlayer, kRltMsg.m_Type, rlt, false);
	if (ret == RE_SUCCESS)
	{
		pkPlayer->SendMsgToClient(MGPT_GGU_RLT_POWERGUILD_REWARD_TIME, &rlt);
	}
}

ON_HANDLE(MGPT_GGU_NOTIFY_DOMAININFO, this_source_type, false)
{
	MG_SS_Domain_PowerNotify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	pkGuildManager->UpdateAllDomainPower(kRltMsg);
}

ON_HANDLE(MGPT_GGU_NOTIFY_POWERGUILD, this_source_type, false)
{
	MG_SS_PowerGuild_Notify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	pkGuildManager->UpdateAllLocalPower(kRltMsg);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILDWAR_NOTICE, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_Notice kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (GetGameServerApp()->GetServerManager()->GetCurGuildGroupId() != kRltMsg.m_GroupId)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->CrossRealmGuildWarNotice(kRltMsg.m_NoticeIndex);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILDWAR_START, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_Start kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (GetGameServerApp()->GetServerManager()->GetCurGuildGroupId() != kRltMsg.m_GroupId)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->CrossRealmGuildWarGuildStart(kRltMsg.m_StartTimestamp, kRltMsg.m_EndTimestamp);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILDWAR_END, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_End kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (GetGameServerApp()->GetServerManager()->GetCurGuildGroupId() != kRltMsg.m_GroupId)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->CrossRealmGuildWarGuildEnd(kRltMsg.m_StartTimestamp);
}

ON_HANDLE(MGPT_REQ_GUILD_WAR_REFRESH_TIME, this_source_type, false)
{
	static float fCurrent = 0;
	float fNow = GET_PROCESS_TIME;
	if (fNow - fCurrent < TIMER_FOR_GUILD_WAR_MSG)
		return;
	fCurrent = fNow;
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->ReportCrossRealmGuildWarInfo();
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILDWAR_POWER_NOTIFY, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_PowerNotify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (GetGameServerApp()->GetServerManager()->GetCurGuildGroupId() != kRltMsg.m_GroupId)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->CrossRealmGuildWarPowerNotify(kRltMsg);
}

ON_HANDLE(MGPT_NOTIFY_MSG, this_source_type, false)
{
	MG_NotifyMsg kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (GetGameServerApp()->GetServerManager()->GetCurGuildGroupId() != kRltMsg.m_GroupID)
	{
		return;
	}

	BroadcastMsgToClient(MGPT_NOTIFY_MSG, &kRltMsg);
}

ON_HANDLE(MGPT_GAME_NOTICE_MSG, this_source_type, false)
{
	MG_GameNotice kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (GetGameServerApp()->GetServerManager()->GetCurGuildGroupId() != kRltMsg.m_nGuildGroupID)
	{
		return;
	}

	BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &kRltMsg);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_POWERGUILD_NOTIFY, this_source_type, false)
{
	MG_SS_CrossRealm_PowerGuild_Notify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	static float fCurrent = 0;
	float fNow = GET_PROCESS_TIME;
	if (fNow - fCurrent < TIMER_FOR_GUILD_WAR_MSG)
		return;
	fCurrent = fNow;
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	pkGuildManager->UpdateAllCrossRealmPower(kRltMsg);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_WARTIME_NOTIFY, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_TimeNotify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	static float fCurrent = 0;
	float fNow = GET_PROCESS_TIME;
	if (fNow - fCurrent < TIMER_FOR_GUILD_WAR_MSG)
		return;
	fCurrent = fNow;
	if (GetGameServerApp()->GetServerManager()->GetCurGuildGroupId() != kRltMsg.m_GroupId)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->SetCrossRealmGuildWarTime(kRltMsg.m_BeginTime, kRltMsg.m_EndTime, kRltMsg.m_WarState);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILD_WARPLACE_START, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_WarPlaceStart kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->CrossRealmGuildWarPlaceStart(kRltMsg.m_StartTimestamp, kRltMsg.m_EndTimestamp, kRltMsg.m_GroupId, kRltMsg.m_PowerGuildId, kRltMsg.m_RaidId);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILD_WARPLACE_END, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_WarPlaceEnd kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->CrossRealmGuildWarPlaceEnd(kRltMsg.m_StartTimestamp, kRltMsg.m_Force, kRltMsg.m_Ignore);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILDWAR_GUILDENTER_RESP, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_GuildEnter_Resp kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltMsg.m_CharID);
	if (pkPlayer == NULL)
	{
		return;
	}
	if (kRltMsg.m_Result != RE_SUCCESS)
	{
		MG_EnterGlobalGuild_Resp resp;
		resp.m_Result = kRltMsg.m_Result;
		pkPlayer->SendMsgToClient(MGPT_RESP_GLOBALGUILDWAR_ENTER, &resp);
		return;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	enterLeaveManager->PlayerTryEnterCrossRealm(pkPlayer, kRltMsg.m_TargetServer);
}

ON_HANDLE(MGPT_GGU_CROSSREALM_GUILDWAR_PLACEENTER_REQ, this_source_type, false)
{
	MG_SS_CrossRealmGuildWar_PlaceEnter_Req kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_SS_CrossRealmGuildWar_PlaceEnter_Resp resp;
	resp.m_CharID = kRltMsg.m_CharID;
	resp.m_Userid = kRltMsg.m_Userid;
	resp.m_serverId = kRltMsg.m_serverId;
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();

	resp.m_Result = manager->AppointEnterCrossRealmGuildWar(GET_PLAYERID(kRltMsg.m_Userid), kRltMsg.m_Userid);
	if (resp.m_Result != RE_SUCCESS)
	{
		SendMsgToSociety(MGPT_GGU_CROSSREALM_GUILDWAR_PLACEENTER_RESP, &resp);
		return;
	}
	resp.m_TargetServerId = UT_ServerHelper::GetServerId();
	SendMsgToSociety(MGPT_GGU_CROSSREALM_GUILDWAR_PLACEENTER_RESP, &resp);
}

ON_HANDLE(MGPT_GGU_NOTIFY_UPDATE_GUILDRAID_INFO, this_source_type, false)
{
	MG_SS_Notify_UPDATE_GuildRaid_INFO kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guildinfo == NULL)
	{
		return;
	}
	guildinfo->NotifyGuildRaidInfo(kRltMsg);
}

ON_HANDLE(MGPT_GGU_RLT_UPGRADE, this_source_type, false)
{
	MG_SS_Upgrade_RLT kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	if (pkPlayer->GetGuildID() == INVALID_GUILD_ID)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->UpgradeRlt(pkPlayer, kRltMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_RLT_AUTOADD_CHANGE, this_source_type, false)
{
	MG_SS_RLT_GuildApply_AutoAdd kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	if (pkPlayer->GetGuildID() == INVALID_GUILD_ID)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->AutoaddRlt(pkPlayer, kRltMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_NOTIFY_AUTOADD_CHANGE, this_source_type, false)
{
	MG_SS_Notify_GuildApply_AutoAdd kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guild == NULL)
	{
		return;
	}
	guild->UpdateAutoAdd(kRltMsg.m_AutoAdd);
}

ON_HANDLE(MGPT_GGU_CREATE_RAID_RESP, this_source_type, false)
{
	MG_SS_CreateRaid_RESP kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_CrossRealmRaidManager* pkManager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
	if (pkManager == NULL)
	{
		return;
	}
	pkManager->CreateCrossRealmRaidResp(kRltMsg.m_Result, kRltMsg.m_RaidId, kRltMsg.m_ServerId, kRltMsg.m_MapResId);
}

ON_HANDLE(MGPT_GGU_DESTROY_RAID_GAME_NOTIFY, this_source_type, false)
{
	MG_SS_RaidDestory_Game_Notify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_CrossRealmRaidManager* pkManager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
	if (pkManager == NULL)
	{
		return;
	}
	pkManager->DestoryCrossRealmRaidGameNotify(kRltMsg.m_RaidId ,kRltMsg.m_MapResId, kRltMsg.m_ServerId);
}

ON_HANDLE(MGPT_GGU_ENTER_RAID_RESP, this_source_type, false)
{
	MG_SS_EnterRaid_RESP kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltMsg.m_CharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_CrossRealmRaidManager* pkManager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
	if (pkManager == NULL)
	{
		return;
	}
	pkManager->EnterCrossRealmRaidResp(pkPlayer, kRltMsg.m_Result, kRltMsg.m_TargetServerId);
}

ON_HANDLE(MGPT_GGU_APPOINTENTER_RAID_REQ, this_source_type, false)
{
	MG_SS_EnterRaid_REQ kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_CrossRealmRaidManager* pkManager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();

	ResultType ret = pkManager->PreEnterCrossRealmRaid_Place(kRltMsg.m_RaidId, rkMsgHead.m_uiCharID, kRltMsg.m_UserId);
	MG_SS_EnterRaid_RESP resp;
	resp.m_RaidId = kRltMsg.m_RaidId;
	resp.m_CharID = kRltMsg.m_CharID;
	resp.m_UserId = kRltMsg.m_UserId;
	resp.m_Level = kRltMsg.m_Level;
	resp.m_ServerId =  kRltMsg.m_ServerId;
	resp.m_Result = ret;
	SendMsgToSociety(MGPT_GGU_APPOINTENTER_RAID_RESP, &resp);
}

ON_HANDLE(MGPT_GGU_PREPARE_RAID_REQ, this_source_type, false)
{
	MG_SS_PrepareRaid_REQ kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int32_t iLevel = GetGameServerApp()->GetServerManager()->GetCrossAverageLevel();
	LC_CrossRealmRaidManager* pkManager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
	ResultType ret = pkManager->PrepareCrossRealmRaid(kRltMsg.m_RaidId, kRltMsg.m_ResId, (int32_t)iLevel);
	MG_SS_PrepareRaid_RESP resp;
	resp.m_RaidId = kRltMsg.m_RaidId;
	resp.m_ResId = kRltMsg.m_ResId;
	resp.m_Result = ret;
	CF_TranscriptionRule::DataEntry* pkTransEntry = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(kRltMsg.m_ResId);
	if (pkTransEntry)
	{
		resp.m_nTotalLen = pkTransEntry->_iExistTime;
	}
	SendMsgToSociety(MGPT_GGU_PREPARE_RAID_RESP, &resp);
}

ON_HANDLE(MGPT_GGU_CREATE_TRANSPORT_RESP, this_source_type, false)
{
	MG_SS_Create_Guild_Transport_Resp kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->CreateTransportResp(pkPlayer, kRltMsg.m_Transportid, kRltMsg.m_Result, kRltMsg.m_Index, kRltMsg.m_LogicId);
}

ON_HANDLE(MGPT_GGU_GET_RAIDSERVERLIST_RESP, this_source_type, false)
{
	MG_SS_CommonStrInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	pkPlayer->SendNotifyStrToClient(kRltMsg.m_strInfo.c_str(), PROMPT_CHANNEL_SYSTEM_TIP, PROMPT_LEVEL_NORMAL);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "raid server list: %s", kRltMsg.m_strInfo.c_str());
}

ON_HANDLE(MGPT_GGU_TRANSPORT_NOTIFY, this_source_type, false)
{
	MG_SS_Guild_Transport_Notify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guild == NULL)
	{
		return;
	}
	char_data_define::guild_transport transport;
	transport.ParseFromString(kRltMsg.m_TransportInfo);
	guild->TransportNotify(transport);
}

ON_HANDLE(MGPT_GGU_TRANSPORT_DESTORYNOTIFY, this_source_type, false)
{
	MG_SS_Guild_Transport_DestoryNotify kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	pkGuildManager->TransportDestoryNotify(kRltMsg.m_GuildId, kRltMsg.m_TransportId);
}

ON_HANDLE(MGPT_GGU_ENTER_TRANSPORT_RESP, this_source_type, false)
{
	MG_SS_Guild_Transport_Enter_Resp kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->EnterTransportResp(pkPlayer, kRltMsg.m_TransportId, kRltMsg.m_Result);
}

ON_HANDLE(MGPT_GGU_ENTERAPPOINT_TRANSPORT_REQ, this_source_type, false)
{
	MG_SS_Guild_Transport_Enter_Appoint_Req kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_GuildId);
	if (guild == NULL)
	{
		return;
	}

	ResultType ret = guild->AppointEnterTransport_Place(kRltMsg.m_CharID, kRltMsg.m_UserId, kRltMsg.m_Level, kRltMsg.m_TransportId);
	MG_SS_Guild_Transport_Enter_Appoint_Resp resp;
	resp.m_GuildId = kRltMsg.m_GuildId;
	resp.m_TransportId = kRltMsg.m_TransportId;
	resp.m_CharID = kRltMsg.m_CharID;
	resp.m_UserId = kRltMsg.m_UserId;
	resp.m_Level = kRltMsg.m_Level;
	resp.m_Result = ret;
	SendMsgToSociety(MGPT_GGU_ENTERAPPOINT_TRANSPORT_RESP, &resp);
}

ON_HANDLE(MGPT_RLT_GUILD_TRANS_DATA, this_source_type, false)
{
	MG_SS_GUILD_TRANS_DATA kReqBody;
	if (!kReqBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_RLT_GUILD_TRANS_DATA msgType:%d, serverid:%d, guild:%d\n", kReqBody.m_msgType, GetGameServerID(), kReqBody.m_guildId);

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(kReqBody.m_guildId);
	if (guild == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_RLT_GUILD_TRANS_DATA msgType:%d, serverid:%d, not find guild\n", kReqBody.m_msgType, GetGameServerID());
		return;
	}
	guild->BroadCastMsg(kReqBody.m_msgType, kReqBody.m_msgInfo.c_str(), (uint16_t)kReqBody.m_msgInfo.length());
}

void  __funcAddPlayerIdPtr(LC_ServerPlayerManager* pkPlayerManager, const unique_id_type& id, int32_t nForceMail, mem::list<LC_ServerPlayer*>& rPlayerPtrList, unique_id_list& rPlayerIdList)
{
	do 
	{
		if(nForceMail <= 0)
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(id);
			if (pkPlayer != NULL)
			{
				rPlayerPtrList.push_back(pkPlayer);
				break;
			}
		}
		rPlayerIdList.push_back(id);
	} while (false);
}

void __funcGainGift(LC_CommonStatRewardDataEntry* pRewardConfig, int32_t nWinGainType, mem::list<LC_ServerPlayer*>& rPlayerPtrList, unique_id_list& rPlayerIdList)
{
	if(!rPlayerIdList.empty())
	{
		gain_CommonStatDraw(rPlayerIdList, NULL, pRewardConfig, nWinGainType);
	}
	if(!rPlayerPtrList.empty())
	{
		gain_CommonStatDraw(rPlayerPtrList, NULL, pRewardConfig, nWinGainType);
	}
	rPlayerIdList.clear();
	rPlayerPtrList.clear();
}

ON_HANDLE(MGPT_RLT_GUILD_WAR_GIFT, this_source_type, false)
{

}

ON_HANDLE(MGPT_REQ_GUILD_LEADER_AUTOCHG, this_source_type, false)
{
	MG_SS_Notify_LeaderChange kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	pkGuildManager->ClearGuildAllPower(kRltMsg.m_ServerId, kRltMsg.m_GuildId);
}

ON_HANDLE(MGPT_GUILD_WAR_MARKCHG, this_source_type, false)
{
	MG_SS_GUILD_WAR_MARK_INFO kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	static float fCurrent = 0;
	float fNow = GET_PROCESS_TIME;
	if (fNow - fCurrent < TIMER_FOR_GUILD_WAR_MSG)
		return;
	fCurrent = fNow;

	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset != NULL)
	{
		LC_ServerGuildWarAsset* pServerWarAsset = dynamic_cast<LC_ServerGuildWarAsset*>(pWorldGuildWarAsset);
		if(pServerWarAsset != NULL)
		{
			pServerWarAsset->GetServerMarkFromSociety(kRltMsg);
		}
	}
}

ON_HANDLE(MGPT_GGU_RLT_ACTACTIVEREWARDINFO, this_source_type, false)
{
	MG_RLT_ACTActiveRewardInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::UpdateLimitRewardRecord(kRltMsg.m_nKey, kRltMsg.m_nValue);
	UT_ServerHelper::RemoveLimitActivity(kRltMsg.m_nKey);
}

ON_HANDLE(MGPT_GGU_RLT_ACTACTIVEREWARDINFO_B, this_source_type, false)
{
	MG_RLT_ACTActiveRewardInfo_B kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GGU_RLT_ACTACTIVEREWARDINFO_B serverid:%d r1\n", GetGameServerID());
		return;
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_GGU_RLT_ACTACTIVEREWARDINFO_B kRltMsg.m_nKey:%d, kRltMsg.m_nValue:%d\n", kRltMsg.m_nKey, kRltMsg.m_nValue);

	UT_ServerHelper::UpdateLimitRewardRecordB(kRltMsg.m_nKey, kRltMsg.m_nValue);
	UT_ServerHelper::RemoveLimitActivityB(kRltMsg.m_nKey);
}

ON_HANDLE(MGPT_GGU_RLT_GETLIMITREWARD, this_source_type, false)
{
	MG_RLT_GetLimitReward kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	GameLogic::LC_RewardItemsExtend rkExtend;
	rkExtend.m_iChoice = kRltMsg.m_iChoice;

	pkPlayer->DoLimitReward(kRltMsg.m_iActivetyType, kRltMsg.m_iType, kRltMsg.m_iSchedule, kRltMsg.m_iKey, rkExtend, kRltMsg.m_nValue);
}

ON_HANDLE(MGPT_GGU_RLT_GETLIMITREWARD_B, this_source_type, false)
{
	MG_RLT_GetLimitReward_B kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GGU_RLT_GETLIMITREWARD_B serverID:%d, playerID:%d r1", GetGameServerID(), rkMsgHead.m_uiCharID);
		return; 
	}

	GameLogic::LC_RewardItemsExtend rkExtend;
	rkExtend.m_iChoice = kRltMsg.m_iChoice;
	pkPlayer->DoLimitReward(kRltMsg.m_iActivetyType, kRltMsg.m_iType, kRltMsg.m_iSchedule, kRltMsg.m_iKey, rkExtend, kRltMsg.m_nValue);

	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GGU_RLT_GETLIMITREWARD_B serverID:%d, playerID:%d DoLimitReward", GetGameServerID(), rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_GGU_NOTIFY_MODIFY_BUFFID, this_source_type, false)
{
	MG_SS_Notify_ModifyBuffId kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
		return;
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (NULL == pkGuildManager)
		return;

	const unique_id_impl& instance = kRltMsg.m_uId;
	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "MGPT_GGU_NOTIFY_MODIFY_BUFFID eType:%d, c_cid:%d, c_citizenship:%d, m_bAdd:%d, m_uTowerId:%d",
		kRltMsg.m_type, instance.detail(), instance.catagory(), kRltMsg.m_bAdd, kRltMsg.m_uTowerId);

	if(UpdateGuildFightType::UGFT_BUFF == kRltMsg.m_type)
	{
		if(kRltMsg.m_uCacheId)
			UT_ServerHelper::SetEraseRewardSendState(kRltMsg.m_uCacheId);

		CF_BestGuildBossInfo* pkGuildBossInfo = CF_BestGuildBossInfo::GetSingletonPtr();
		CF_BestGuildBossInfo::DataEntry* pkData = pkGuildBossInfo->GetEntryPtr(kRltMsg.m_uTowerId);
		if(pkData)
			UT_ServerHelper::UpdateSingleBuffIdToGuildAllMember(kRltMsg.m_uId,pkData->_iBuff,kRltMsg.m_bAdd);	
	}
	if(UpdateGuildFightType::UGFT_UPDATE_POINT == kRltMsg.m_type )
	{
		if(kRltMsg.m_bAdd)
		{
			LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
			if(!pkPlayerManager)
				return;
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltMsg.m_uId);
			if (!pkPlayer)
				return;
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			LC_ServerMapLogic_Place* place=(LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
			if (place==NULL)
				return;
			LC_ServerMapLogic_SoloDulePlay* play=(LC_ServerMapLogic_SoloDulePlay*)place->GetPlay(PlayType_SoloDule);
			if (play==NULL)
				return;

			//
			pkPlayer->SetCurrentAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_GUILD,ROIT_Total);
			pkPlayer->SetCurrentAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_PERSONAL,ROIT_Total);
			
			/*UT_ServerHelper::gCacheGuildDefendMap[pkPlayer->GetInstance()] = singleFightRecordInfo;*/
			mem::map<uint64_t,SingleFightRecord>::iterator itc = UT_ServerHelper::gCacheGuildDefendMap.find(pkPlayer->GetInstance());
			if(itc != UT_ServerHelper::gCacheGuildDefendMap.end())
			{
				itc->second.m_nScore = -(pkPlayer->GetBestGuildSettleAccountPoint());

				MG_SS_AddGuildDefendLog msg;
				msg.m_serverID = GSNodeNetwork::Instance().getEntryIdByMergeId(GetGameServerID());
				msg.m_charID = itc->second.m_DefenceInfo.m_CommanderBaseInfo.m_uCharId;
				msg.m_logRecord = itc->second;
				SendMsgToSociety(MGPT_SS_ADDGUILDDEFENDLOG, &msg);
				UT_ServerHelper::BroadcastBeFightedToPlayer(itc->second.m_DefenceInfo.m_CommanderBaseInfo.m_uCharId);
				UT_ServerHelper::gCacheGuildDefendMap.erase(itc);
			}
			play->Settle_GuildOffline();
			LC_ServerPlayerGuild* pPlayerGuild = pkPlayer->GetPlayerGuildRobInfo();
			pPlayerGuild->UpdateRobNum();
		}
	}
	else if(UpdateGuildFightType::UGFT_GET_ALL_MEMBER_POINT == kRltMsg.m_type)
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		if(!pkPlayerManager)
			return;
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kRltMsg.m_uId);
		if (!pkPlayer)
			return;
		MG_Resp_GuildFightGuildMemberIdInfo respMsg;
		respMsg.m_nGuild = kRltMsg.m_msgUniqueKey;
		respMsg.m_IdVec.assign(kRltMsg.m_member.begin(),kRltMsg.m_member.end());
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_GUILD_MEMBER_ID_INFO, &respMsg);
	}
	else if(UpdateGuildFightType::UGFT_GIVE_ALL_MEMBER_POINT == kRltMsg.m_type)
	{
		//kRltMsg.m_uCacheId
		UT_ServerHelper::ReqPlayerOfflineToDb(OfflinePlayerFromDBType::OPFDT_GUILD_FIGHT_OP_SCORE,kRltMsg.m_member,kRltMsg.m_msgUniqueKey,kRltMsg.m_uTowerId,kRltMsg.m_uCacheId,0,0);
		//SetEraseRewardSendState
	}
	else if(UpdateGuildFightType::UGFT_GET_ALL_MEMBER_LEVEL == kRltMsg.m_type)
	{
		//发送公会奖励
		// 公会id kRltMsg.m_msgUniqueKey;
		// 名次  kRltMsg.m_uId;
		// rankId kRltMsg.m_uTowerId
		UT_ServerHelper::SendGuildFightGuildRewardByStoryId(kRltMsg.m_msgUniqueKey,kRltMsg.m_uTowerId,kRltMsg.m_uId,kRltMsg.m_memberAndlevel);
	}
}

ON_HANDLE(MGPT_GG_ADDGUILDPRESENTINFO, this_source_type, false)
{
	MG_GG_AddGuildPresentInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (guild != NULL)
	{
		guild->AddGuildPresentInfo(kRltMsg.m_nID, kRltMsg.m_nData);
	}
}

ON_HANDLE(MGPT_GG_NOTICEMODIFYPRESENTID, this_source_type, false)
{
	MG_GG_NoticeModifyPresentID kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (guild != NULL)
	{
		if(kRltMsg.m_nOPType == OPType_Add)
		{
			guild->AddPresentID(kRltMsg.m_nPresentID);
		}
		else if(kRltMsg.m_nOPType == OPType_Del)
		{
			guild->DelPresentID(kRltMsg.m_nPresentID);
		}
	}
}

ON_HANDLE(MGPT_GG_DELETEGUILDPRESENTINFO, this_source_type, false)
{
	MG_GG_DeleteGuildPresentInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*  guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (guild != NULL)
	{
		guild->DeleteGuildPresentInfo(kRltMsg.m_nID);
	}
}

ON_HANDLE(MGPT_GG_SENDGUILDPRESENTSTATE, this_source_type, false)
{
	MG_GG_SendGuildPresentState kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (guild != NULL)
	{
		guild->SendGuildPresentState(kRltMsg.m_nState, rkMsgHead.m_uiCharID);
	}
}

ON_HANDLE(MGPT_GG_NOTIFY_GUILDPRESENTINFO, this_source_type, false)
{
	MG_GG_Notify_GuildPresentInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (guild != NULL)
	{
		guild->SetGuildPresentInfo(kRltMsg.m_nGuildPresentInfo);
	}
}

ON_HANDLE(MGPT_GG_UPDATEGUILDCRYSTALINFO, this_source_type, false)
{
	MG_GG_UpdateGuildCrystalInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (guild != NULL)
	{
		guild->UpdateGuildCrystalInfo(kRltMsg);
	}
}

ON_HANDLE(MGPT_GG_RLT_ADDGUILDBOSSINFO, this_source_type, false)
{
	MG_GG_RLT_AddGuildBossInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//开启成功后，清除榜单
	mem::vector<uint64_t> params;
	params.reserve(2);
	params.push_back(kRltMsg.m_nGuildID);
	params.push_back(kRltMsg.m_nMapID);
	UT_ServerHelper::RedisRank_ClearDamageRecord(UT_REDIS_KEY_GUILDMAPBOSS_DAMAGE_RANK, params);

	//开启成功后，清除redis存储的boss状态数据
	UT_ServerHelper::DeleteGuildMapBossHP(kRltMsg.m_nMapID, kRltMsg.m_nGuildID);

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(kRltMsg.m_nCharID);
	if(NULL != pkPlayer)
	{
		MG_RLT_ActivityGuildBossMap rltMsg;
		rltMsg.m_nMapID = kRltMsg.m_nMapID;
		rltMsg.m_nResult = kRltMsg.m_nResult;
		pkPlayer->SendMsgToClient(MGPT_RLT_ACTIVITYGUILDBOSSMAP, &rltMsg);
	}
}

ON_HANDLE(MGPT_GG_SYNCNEWOPENGUILDMAPBOSS, this_source_type, false)
{
	MG_GG_SyncNewOpenGuildMapBoss kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (pkGuild != NULL)
	{
		pkGuild->BroadcastNewOpenGuildMapBoss();
	}

}

ON_HANDLE(MGPT_GG_UPDATEGUILDMAPBOSSINFO, this_source_type, false)
{
	MG_GG_UpdateGuildMapBossInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::UpdateGuildMapBossState(kRltMsg.m_nMapID, kRltMsg.m_nGuildID, kRltMsg.m_nBossInfo, kRltMsg.m_nOldState);

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (pkGuild != NULL)
	{
		pkGuild->UpdateGuildMapBossInfo(kRltMsg.m_nMapID, kRltMsg.m_nBossInfo);
	}
}

ON_HANDLE(MGPT_GG_NOTIFYGUILDMAPBOSSINFO, this_source_type, false)
{
	MG_GG_NotifyGuildMapBossInfo kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (pkGuild != NULL)
	{
		pkGuild->SetGuildMapBossInfo(kRltMsg.m_nGuildBossInfo, kRltMsg.m_nFinishMapIDInfo,rkMsgHead.m_uiCharID);
	}
}

ON_HANDLE(MGPT_GG_UPDATEGUILDMAPFINISHMAP, this_source_type, false)
{
	MG_GG_UpdateGuildMapFinishMap kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(kRltMsg.m_nGuildID);
	if (pkGuild != NULL)
	{
		pkGuild->UpdateGuildMapFinishMap(kRltMsg.m_nFinishMapID);
	}
}

ON_HANDLE(MGPT_S2G_CREATEGUILDBOSSMAP, this_source_type, false)
{
	MG_S2G_CreateGuildBossMap msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_G2S_CreateGuildBossMap rltMsg;
	rltMsg.m_nBossInfo = msg.m_nBossInfo;
	rltMsg.m_nConsumeItem = msg.m_nConsumeItem;
	rltMsg.m_nMapType = msg.m_nMapType;
	rltMsg.m_nMapID = msg.m_nMapID;
	rltMsg.m_nGuildID = msg.m_nGuildID;
	rltMsg.m_nCharID = msg.m_nCharID;
	rltMsg.m_nServerID = msg.m_nServerID;
	if(true != GetGameServerApp()->IsNodeType_Global())
	{
		rltMsg.m_nResult = RE_GUILDMAP_ERROR_19;
		SendMsgToSociety(MGPT_G2S_CREATEGUILDBOSSMAP, &rltMsg);
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_S2G_CREATEGUILDBOSSMAP not is globalNode! serverid:%d", GetGameServerID());
		return;
	}	

	const unique_id_impl& instance = msg.m_nCharID;
	static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldMgr->CreateServerMapLogic(0, msg.m_nMapID, msg.m_nMapType, 0);
	if(NULL == pkPlace)
	{
		rltMsg.m_nResult = RE_GUILDMAP_ERROR_20;
		SendMsgToSociety(MGPT_G2S_CREATEGUILDBOSSMAP, &rltMsg);

		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_S2G_CreateGuildBossMap error! mapID:%d, mapType:%d, c_cid:%d, c_citizenship:%d", 
			msg.m_nMapID, msg.m_nMapType, instance.detail(), instance.catagory());
		return;
	}

	int mapLogicID = pkPlace->GetMapLogicID();
	pkPlace->SetGuildID(msg.m_nGuildID);
	pkPlace->SetPlaceInvaildTime(msg.m_nBossInfo.m_nInvaildTime);

	rltMsg.m_nResult = RE_SUCCESS;
	SendMsgToSociety(MGPT_G2S_CREATEGUILDBOSSMAP, &rltMsg);

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_S2G_CREATEGUILDBOSSMAP create newVaild map! guildID:%d, mapID:%d, mapLogicID:%d, c_cid:%d, c_citizenship:%d", 
		msg.m_nGuildID, msg.m_nMapID, pkPlace->GetMapLogicID(), instance.detail(), instance.catagory());
}

ON_HANDLE(MGPT_SS_RLTGUILDDEFENDLOG, this_source_type, false)
{
	MG_RESP_FightRecord msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if(NULL != pkPlayer)
	{
		pkPlayer->SendMsgToClient(MGPT_RESP_FIGHTRECORD, &msg);
	}
}

ON_HANDLE(MGPT_SS_RESP_GETBEDEFIGHTEDFLAG, this_source_type, false)
{
	MG_SS_RESP_GetBeDefightedFlag msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(msg.m_charID);
	if(NULL != pkPlayer)
	{
		MG_NoticeBeDefighted rltMsg;
		rltMsg.m_nBeDefighted = msg.m_nBeFighted;
		pkPlayer->SendMsgToClient(MGPT_NOTICEBEDEFIGHTED, &rltMsg);
	}
}

ON_HANDLE(MGPT_SS_RESP_SENDMAILTOGUILD, this_source_type, false)
{
	MG_RESP_SendMailToGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::SendGuildMail(kMsg);
}
// end namespace Protocol
}


