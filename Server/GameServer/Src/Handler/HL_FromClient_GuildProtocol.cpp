#include "zlib.h"
#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_CS_Guild.h"
#include "MG_Game2Guild.h"
#include "MG_ProtocolStruct.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerPlayerGroup.h"
#include "TS_TranManager.h"
#include "TS_TranType.h"
#include "CF_GuildLevel.h"
#include "LC_ServerGuildManager.h"
#include "LC_GuildLadderManager.h"
#include "UT_ServerHelper.h"
#include "MG_PB.h"
#include "msg.pb.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

ON_HANDLE(MGPT_REQ_GUILD_ASSET, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GuildAsset kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	pkGuildManager->QueryPlayerGuild(pkPlayer);
}

ON_HANDLE(MGPT_REQ_CREATE_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CreateGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	GetGameServerApp()->GetTextFilterChannel().BounedPush(kMsg, rkMsgHead.m_uiUserID);
}

ON_HANDLE(MGPT_REQ_RECRUIT_GUILD_MEMBER, this_source_type, false)
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

	MG_REQ_RecruitGuildMember reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_RecruitGuildMember resqMsg;
	resqMsg.m_uiResult = RE_FAIL;

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	uint64_t guildid = pkGuildManager->GetPlayerGuildId(pkPlayer->GetInstance());
	if(guildid == INVALID_GUILD_ID || guildid != reqMsg.m_uiGuildID)
	{		
		pkPlayer->SendMsgToClient(MGPT_RLT_RECRUIT_GUILD_MEMBER, &resqMsg);
		return;
	}

	LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(guildid);
	if(pkGuildInfo == NULL)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_RECRUIT_GUILD_MEMBER, &resqMsg);
		return;
	}

	Protocol::PS_GuildInfo guildInfo;
	pkGuildInfo->Fill(guildInfo);

	int64_t currentTime =  GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(currentTime - guildInfo.m_RecruitTime < MIN_RECRUIT_TIME)
	{
		//招募间隔时间不足3分钟
		resqMsg.m_uiResult = RE_RECURIT_TIME_TOO_SHORT;
		pkPlayer->SendMsgToClient(MGPT_RLT_RECRUIT_GUILD_MEMBER, &resqMsg);
	}
	else
	{
		//发给招募请求者
		resqMsg.m_uiResult = RE_SUCCESS;
		pkPlayer->SendMsgToClient(MGPT_RLT_RECRUIT_GUILD_MEMBER, &resqMsg);

		//系统发送招募消息
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(34);
		noticeFmort.AddString(STDSTR2TPSTR(guildInfo.szName));
		noticeFmort.AddNumber(guildInfo.level);
		noticeFmort.AddUInt64Number(guildInfo.uiGuildID);
		noticeFmort.AddNumber(guildInfo.level);
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkPlayer->GetCitizenship());

		//发送给guildserver，更新
		MG_SS_REQ_UPDATE_GuildRecruitTime kReqSGMsg;
		kReqSGMsg.m_GuildId = pkGuildInfo->GetId();
		kReqSGMsg.m_RecruitTime = currentTime;
		SendMsgToSociety(MGPT_GGU_REQ_UPDATE_GUILDRECRUITTIME, &kReqSGMsg);

		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILD_OPERATE,OPERATE_LOGTYPE10);
		RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode);
	}
}

ON_HANDLE(MGPT_REQ_GUILD_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GuildReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}


	MG_RLT_GuildReward resqMsg;
	resqMsg.m_uiResult = RE_FAIL;

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	uint64_t guildid = pkGuildManager->GetPlayerGuildId(pkPlayer->GetInstance());
	if(guildid == INVALID_GUILD_ID || guildid != reqMsg.m_uiGuildID)
	{		
		pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_REWARD, &resqMsg);
		return;
	}

	LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(guildid);
	if(pkGuildInfo == NULL)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_REWARD, &resqMsg);
		return;
	}

	int guildLevel = pkGuildInfo->GetLevel();
	CF_GuildLevel* guildlevel = CF_GuildLevel::GetSingletonPtr();
	CF_GuildLevel::DataEntry* entry = guildlevel->GetEntryPtr(guildLevel);
	if(entry == NULL)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_REWARD, &resqMsg);
		return;
	}
	//工会每日--埋点日志
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILD_OPERATE, OPERATE_LOGTYPE2);
	//可以领取
	if(pkPlayer->GetGuildRewardState())
	{		
		Utility::UT_SIMDataList items;
		Utility::ParseFmtString2DataIFs(entry->_sReward, items);
		
		StringType mailTitle;
		StringType mailDesc;
		resqMsg.m_uiResult = UT_ServerHelper::HandleBonusItems(PACK_TYPE_BASIC, items, pkPlayer, SYS_FUNC_TP_GUILDREWARD, guildLevel, mailTitle, mailDesc, false, false, false, nLogCode);
		if(RE_SUCCESS == resqMsg.m_uiResult)
		{
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
			pkPlayer->SetGuildRewardFlag(false);
			pkPlayer->SetGuildRewardState(false);
		}
		else if(RE_ITEM_PACKAGE_FULL == resqMsg.m_uiResult)
		{
			//背包已满
			resqMsg.m_uiResult = RE_ITEM_CANT_PUT_PACKAGE;
			pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_REWARD, &resqMsg);
			return;	
		}
	}
	else
	{
		resqMsg.m_uiResult = RE_GUILD_REWARD_ALREADY_BUY;
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_REWARD, &resqMsg);
}

ON_HANDLE(MGPT_REQ_APPLY_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ApplyGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkServerPlayerManager == NULL)
	{
		return;
	}

	//查看是否是从人那边申请
	uint64_t guildid = 0;
	int level = 0;
	if (kMsg.m_uiTargetCharID != INVALID_UNIQUE_ID)
	{
		LC_ServerGuildInfo* guild = pkGuildManager->GetPlayerGuild(kMsg.m_uiTargetCharID);
		if (guild == NULL)
		{
			MG_RLT_ApplyGuild kRltMsg;
			kRltMsg.m_uiGuildID = 0;
			kRltMsg.m_uiResult = RE_NO_GUILD;
			SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_APPLY_GUILD, &kRltMsg);
			return;
		}
		guildid = guild->GetId();
		level = guild->GetLevel();
	}
	else
	{
		guildid = kMsg.m_uiGuildID;
		level = kMsg.m_Level;
	}
	ResultType ret = pkGuildManager->Add2Guild(pkPlayer, guildid, level);
	if(ret != RE_SUCCESS)
	{
		MG_RLT_ApplyGuild kRltMsg;
		kRltMsg.m_uiGuildID = kMsg.m_uiGuildID;
		kRltMsg.m_uiResult = ret;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_APPLY_GUILD, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_REQ_FASTAPPLYGUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_FastApplyGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}

	MG_RLT_FastApplyGuild kRltMsg;
	kRltMsg.m_nResult = pkGuildManager->FastAdd2Guild(pkPlayer);
	if(kRltMsg.m_nResult != RE_SUCCESS)
	{
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_FASTAPPLYGUILD, &kRltMsg);
		return;
	}
}


ON_HANDLE(MGPT_REQ_CONFIRM_APPLY_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ConfirmApplyGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_RLT_ConfirmApplyGuild kRltMsg;
	kRltMsg.m_uiCharID = pkPlayer->GetInstance();
	kRltMsg.m_uiApplyCharID = kMsg.m_uiApplyCharID;
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{

		kRltMsg.m_Result = RE_ACTION_FORBID_GUILD;
		pkPlayer->SendMsgToClient(MGPT_RLT_CONFIRM_APPLY_GUILD, &kRltMsg);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	ResultType ret = guild->ApplyResultReq(pkPlayer, kMsg.m_bAgree, kMsg.m_uiApplyCharID);
	if (ret != RE_SUCCESS)
	{
		kRltMsg.m_Result = ret;
		pkPlayer->SendMsgToClient(MGPT_RLT_CONFIRM_APPLY_GUILD, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_REQ_FORBID_GUILD_APPLY, this_source_type, false)
{
	
}

ON_HANDLE(MGPT_REQ_CLEAR_GUILD_APPLYLIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ClearGuildApplyList kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_RLT_ClearGuildApplyList resp;
		resp.m_uiResult = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_CLEAR_GUILD_APPLYLIST, &resp);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_RLT_ClearGuildApplyList resp;
		resp.m_uiResult = RE_GUILD_NOT_EXIST;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_CLEAR_GUILD_APPLYLIST, &resp);
		return;
	}
	ResultType ret32 = guild->ClearApplyReq(pkPlayer);
	if (ret32 != RE_SUCCESS)
	{
		MG_RLT_ClearGuildApplyList resp;
		resp.m_uiResult = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_CLEAR_GUILD_APPLYLIST, &resp);
		return;
	}
}

ON_HANDLE(MGPT_REQ_EXIT_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ExitGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_RLT_ExitGuild resp;
		resp.m_uiCharID = pkPlayer->GetInstance();
		resp.m_uiResult = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_EXIT_GUILD, &resp);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_RLT_ExitGuild resp;
		resp.m_uiCharID = pkPlayer->GetInstance();
		resp.m_uiResult = RE_GUILD_NOT_EXIST;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_EXIT_GUILD, &resp);
		return;
	}
	ResultType ret32 = guild->ExitGuildReq(pkPlayer);
	if (ret32 != RE_SUCCESS)
	{
		MG_RLT_ExitGuild resp;
		resp.m_uiCharID = pkPlayer->GetInstance();
		resp.m_uiResult = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_EXIT_GUILD, &resp);
		return;
	}
	else
	{
		UT_ServerHelper::UpdatePlayerInfoGuildName(pkPlayer->GetInstance(),pkPlayer->GetOwnerCharName().c_str(),"",0,0,pkPlayer->GetCommanderTitleID());
	}
}

ON_HANDLE(MGPT_REQ_MODIFY_GUILD_DECLARATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ModifyGuildDeclaration kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_RLT_ModifyGuildDeclaration kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_MODIFY_GUILD_DECLARATION, &kRltMsg);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_RLT_ModifyGuildDeclaration kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_GUILD_NOT_EXIST;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_MODIFY_GUILD_DECLARATION, &kRltMsg);
		return;
	}
	if(guild->GetLevel() < 3)
	{
		MG_RLT_ModifyGuildDeclaration kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_GUILD_DECLARATION_LEVLE_LESS;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_MODIFY_GUILD_DECLARATION, &kRltMsg);
		return;
	}
	StringType szDeclaration = STDSTR2TPSTR(kMsg.m_szDeclaration);
	ResultType ret32 = guild->ModifyDeclarationReq(pkPlayer, szDeclaration);
	if (ret32 != RE_SUCCESS)
	{
		MG_RLT_ModifyGuildDeclaration kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_MODIFY_GUILD_DECLARATION, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_REQ_GUILD_KICKMEMBER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_KickMemberFromGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_RLT_KickMemberFromGuild kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_GUILD_KICKMEMBER, &kRltMsg);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_RLT_KickMemberFromGuild kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_GUILD_NOT_EXIST;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_GUILD_KICKMEMBER, &kRltMsg);
		return;
	}
	/*if(UT_ServerHelper::CheckStoryIsActive(GameStoryIDType_GUILDBONFIRPARTY))
	{
		MG_RLT_KickMemberFromGuild kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_EXIST_GUILD_IN_GUILDPARTY;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_GUILD_KICKMEMBER, &kRltMsg);
		return;
	}*/
	ResultType ret32 = guild->KickMemberReq(pkPlayer, kMsg.m_uiTargetCharID);
	if (ret32 != RE_SUCCESS)
	{
		MG_RLT_KickMemberFromGuild kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_GUILD_KICKMEMBER, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_REQ_DISMISS_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_DismissGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_RLT_DismissGuild kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_DISMISS_GUILD, &kRltMsg);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_RLT_DismissGuild kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = RE_GUILD_NOT_EXIST;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_DISMISS_GUILD, &kRltMsg);
		return;
	}
	ResultType ret32 = guild->DismissReq(pkPlayer);
	if (ret32 != RE_SUCCESS)
	{
		MG_RLT_DismissGuild kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_DISMISS_GUILD, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_REQ_INVITE_TO_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_InviteToGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kMsg.m_uiBeInviteCharID);
	if (pkBeInvitePlayer == NULL)
	{
		MG_RLT_InviteToGuild kRltMsg;
		kRltMsg.m_uiInviteCharID = kMsg.m_uiCharID;
		kRltMsg.m_uiResult = RE_GUILD_INVITE_FAILED_TARGET_IS_OFFLINE;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_INVITE_TO_GUILD, &kRltMsg);
		return;
	}

	if(pkBeInvitePlayer != pkBeInvitePlayer->GetOwnerPlayer())
	{
		return;
	}

	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_RLT_InviteToGuild kRltMsg;
		kRltMsg.m_uiInviteCharID = kMsg.m_uiCharID;
		kRltMsg.m_uiResult = RE_GUILD_NOT_EXIST;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_INVITE_TO_GUILD, &kRltMsg);
		return;
	}
	ResultType ret32 = guild->Invite(pkPlayer, pkBeInvitePlayer);
	if (ret32 != RE_SUCCESS)
	{
		MG_RLT_InviteToGuild kRltMsg;
		kRltMsg.m_uiInviteCharID = kMsg.m_uiCharID;
		kRltMsg.m_uiResult = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_INVITE_TO_GUILD, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_RLT_CONFIRM_INVITE_TO_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_ConfirmInviteToGuild kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(kMsg.m_uiGuildID);
	if (guild == NULL)
	{
		return;
	}
	guild->InviteRlt(pkPlayer, kMsg.m_bAgree);
}


ON_HANDLE(MGPT_REQ_GUILD_APPOINT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GuildAppoint kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(kMsg.m_uiCharID != pkPlayer->GetInstance())
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_RLT_GuildAppoint kRltMsg;
		kRltMsg.m_uiCharID = kMsg.m_uiCharID;
		kRltMsg.m_uiTargetCharID = kMsg.m_uiTargetCharID;
		kRltMsg.m_uiResult = RE_ACTION_FORBID_GUILD;
		kRltMsg.m_szTargetCharName = kMsg.m_szTargetCharName;
		kRltMsg.m_byOfficialPostion = kMsg.m_byOfficialPostion;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_GUILD_APPOINT, &kRltMsg);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_RLT_GuildAppoint kRltMsg;
		kRltMsg.m_uiCharID = kMsg.m_uiCharID;
		kRltMsg.m_uiTargetCharID = kMsg.m_uiTargetCharID;
		kRltMsg.m_uiResult = RE_GUILD_NOT_EXIST;
		kRltMsg.m_szTargetCharName = kMsg.m_szTargetCharName;
		kRltMsg.m_byOfficialPostion = kMsg.m_byOfficialPostion;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_GUILD_APPOINT, &kRltMsg);
		return;
	}
	ResultType ret32 = guild->AppointReq(pkPlayer, kMsg.m_uiTargetCharID, kMsg.m_byOfficialPostion);
	if (ret32 != RE_SUCCESS)
	{
		MG_RLT_GuildAppoint kRltMsg;
		kRltMsg.m_uiCharID  = kMsg.m_uiCharID;
		kRltMsg.m_uiResult  = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_GUILD_APPOINT, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_REQ_POWERGUILD_APPOINT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Appoint_Power_REQ kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_Appoint_Power_RLT kRltMsg;
		kRltMsg.m_Result = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_POWERGUILD_APPOINT, &kRltMsg);
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		MG_Appoint_Power_RLT kRltMsg;
		kRltMsg.m_Result = RE_GUILD_NOT_EXIST;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_POWERGUILD_APPOINT, &kRltMsg);
		return;
	}
	ResultType ret32 = guild->AppointPowerReq(pkPlayer, kMsg.m_TargetCharID, kMsg.m_Position);
	if (ret32 != RE_SUCCESS)
	{
		MG_Appoint_Power_RLT kRltMsg;
		kRltMsg.m_Result = ret32;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_POWERGUILD_APPOINT, &kRltMsg);
		return;
	}
}

ON_HANDLE(MGPT_REQ_POWERGUILD_REWARD, this_source_type, false)
{
	//LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	//if (NULL == pkPlayer)
	//{
	//	return;
	//}
	//LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	//LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
	//if (guildinfo == NULL)
	//{
	//	return;
	//}
	//ResultType ret = guildinfo->PowerGuildRewardReq(pkPlayer);
	//if (ret != RE_SUCCESS)
	//{
	//	MG_PowerGuildReward_RLT rlt;
	//	rlt.m_Result = ret;
	//	pkPlayer->SendMsgToClient(MGPT_RESP_POWERGUILD_REWARD, &rlt);
	//}
}

ON_HANDLE(MGPT_GGU_REQ_POWERGUILD_REWARD_TIME, this_source_type, false)
{
	MG_Power_Reward_Time_REQ kReqBody;
	if (!kReqBody.Decode(pMsgBody, usBodySize))
	{
		return ;
	}
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
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
	rlt.m_Type = kReqBody.m_Type;
	ResultType ret = guildinfo->PowerGuildRewardTimeGain(pkPlayer, kReqBody.m_Type, rlt);
	if (ret == RE_SUCCESS)
	{
		pkPlayer->SendMsgToClient(MGPT_GGU_RLT_POWERGUILD_REWARD_TIME, &rlt);
	}
}

ON_HANDLE(MGPT_REQ_DONATE_GUILD_MATERIALS, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_DonateGuildMaterials kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_DonateGuildMaterials kRltMsg;
	kRltMsg.m_uiCharID = kMsg.m_uiCharID;

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		kRltMsg.m_uiResult = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_DONATE_GUILD_MATERIALS, &kRltMsg);
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}

	ResultType iResult = guild->DoNateReq(pkPlayer, kMsg.m_uiMaterailTypeID, kMsg.m_uiMaterailNumber);
	if (iResult != RE_SUCCESS)
	{
		kRltMsg.m_uiResult = iResult;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_DONATE_GUILD_MATERIALS, &kRltMsg);
		return;
	}
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}

ON_HANDLE(MGPT_REQ_SEARCH_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GuildSearch kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		return;
	}

	MG_SS_REQ_GuildList kSGMsg;
	kSGMsg.m_ServerId = pkPlayer->GetCitizenship();
	kSGMsg.m_uiIndex = kMsg.m_uiIndex;
	kSGMsg.m_uiCount = kMsg.m_SearchNum;
	kSGMsg.m_SortType = kMsg.m_SortType;
	kSGMsg.m_SortCol = kMsg.m_SortCol;
	kSGMsg.m_AutoAdd = kMsg.m_AutoAdd;
	SendMsgToSociety(MGPT_GGU_REQ_GUILDLIST, &kSGMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}


ON_HANDLE(MGPT_REQ_SEARCH_GUILD_RANK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SearchGuildRank kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		return;
	}	

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	uint64_t guildid = pkGuildManager->GetPlayerGuildId(pkPlayer->GetInstance());
	if(guildid == INVALID_GUILD_ID)
	{
		MG_SS_RLT_GuildRank resqMsg;
		resqMsg.m_iResult = RE_FAIL;
		pkPlayer->SendMsgToClient(MGPT_RLT_SEARCH_GUILD_RANK, &resqMsg);
		return;
	}

	MG_SS_REQ_GuildRank kSGMsg;
	kSGMsg.m_ServerId =  pkPlayer->GetCitizenship();
	kSGMsg.m_GuildID = guildid;
	SendMsgToSociety(MGPT_GGU_REQ_GUILDRANK, &kSGMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_REQ_UPGRADE_GUILD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_GuildUpgrade result;
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		result.m_Result = RE_ACTION_FORBID_GUILD;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_UPGRADE_GUILD, &result);
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	ResultType iResult = guild->UpgradeReq(pkPlayer);
	if (iResult != RE_SUCCESS)
	{
		result.m_Result = iResult;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_UPGRADE_GUILD, &result);
		return;
	}
}

ON_HANDLE(MGPT_REQ_AUTOADD_CHANGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GuildApply_AutoAdd kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	ResultType ret = guild->AutoaddReq(pkPlayer, kMsg.m_AutoAdd);
	if (ret != RE_SUCCESS)
	{
		MG_RESP_GuildApply_AutoAdd result;
		result.m_Result = ret;
		SAFE_SEND_TO_CLIENT(pkPlayer, MGPT_RLT_AUTOADD_CHANGE, &result);
		return;
	}
}

ON_HANDLE(MGPT_REQ_MEMBERAUTOADD_CHANGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Member_AutoAdd kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->SetGuildAutoAdd(kMsg.m_AutoAdd);
	MG_RLT_Member_AutoAdd result;
	result.m_Result = RE_SUCCESS;
	result.m_AutoAdd = kMsg.m_AutoAdd;

}

ON_HANDLE(MGPT_REQ_GUILDTRANSPORT_CREATE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_GuildTransport_Create kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	ResultType ret = guild->CreateTransport(pkPlayer, kMsg.m_Index);
	if (ret != RE_SUCCESS)
	{
		MG_Resp_GuildTransport_Create resp;
		resp.m_Index = kMsg.m_Index;
		resp.m_Result = ret;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDTRANSPORT_CREATE, &resp);
	}
}

ON_HANDLE(MGPT_REQ_GUILDTRANSPORT_ENTER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_Enter_GuildTransport kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	ResultType result = guild->EnterTransport(pkPlayer, kMsg.m_TransportId);
	if (result != RE_SUCCESS)
	{
		MG_Resp_Enter_GuildTransport resp;
		resp.m_TransportId = kMsg.m_TransportId;
		resp.m_Result = result;
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILDTRANSPORT_ENTER, &resp);
	}
}

ON_HANDLE(MGPT_REQ_ADD_GUILDGOOD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_Add_GuildGood kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	ResultType ret = guild->AddGood(pkPlayer, kMsg.m_TargetCharID, kMsg.m_GoodType);
	MG_Resp_Add_GuildGood resp;
	resp.m_TargetCharID = kMsg.m_TargetCharID;
	resp.m_GoodType = kMsg.m_GoodType;
	resp.m_Result = ret;
	pkPlayer->SendMsgToClient(MGPT_RESP_ADD_GUILDGOOD, &resp);

}

ON_HANDLE(MGPT_REQ_GUILD_ITEM_PACK_OPT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	IF_A_NA_PLAYER(pkPlayer)
	{
		return;
	}

	MG_Req_GuildItemPack_Opt kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame())
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	ResultType nRel = RE_FAIL;
	switch (kMsg.m_OptType)
	{
		case 1:			//出售物品到仓库
			{
				nRel = guild->SellBagItemToGuildItemPack(kMsg.m_iPacketCount, pkPlayer, kMsg.m_iPacketType, kMsg.m_iPacketIndex);
				if (nRel ==RE_ITEM_PACKAGE_FULL)
				{
					nRel = RE_GUILD_WAREHOUSE_FULL;
				}
			}
			break;
		case 2:			//购买仓库物品
			{
				nRel = guild->BuyGuildPackItem(pkPlayer, kMsg.m_iPacketIndex, kMsg.m_iPacketCount, kMsg.m_sItemGuid.c_str());
			}
			break;
		case 3:			//帮主销毁物品
			{
				nRel = guild->DestroyGuildItemPackItem(pkPlayer, kMsg.m_iPacketIndex, kMsg.m_iPacketCount);
			}
			break;
		default:
			break;
	}
	MG_Resp_GuildItemPack_Opt resp;
	resp.m_OptType = kMsg.m_OptType;
	resp.m_Result = nRel;
	pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_ITEM_PACK_OPT, &resp);
}

ON_HANDLE(MGPT_REQ_GUILD_LOG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GuildLog_Info kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return ;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->Fill(kMsg.m_nIndex, kMsg.m_GuildHistory);
	pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_LOG, &kMsg);
}

ON_HANDLE(MGPT_REQ_GUILD_WAR_MARK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset != NULL)
	{
		MG_SyncGuildWarServerMark kMsg;
		pWorldGuildWarAsset->SetServerMarkFromProtocol(kMsg);
		pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_WAR_MARK, &kMsg);
	}
}

ON_HANDLE(MGPT_REQ_GUILD_LADDER_SIGNUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	LC_GuildLadderManager* pkLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
	if (NULL == pkLadderMgr)
	{
		return;
	}
	ResultType ret = RE_SUCCESS;
	do {
		if(UT_ServerHelper::IsInCross(pkPlayer))
		{
			ret = RE_ON_CROSS_SERVER;
			break;
		}
		if (guild->GetLevel() < pkLadderMgr->GetSignUpLevelLimit())
		{
			ret = RE_GUILD_LEVEL_NOTENOUGH;
			break;
		}
		if (!pkLadderMgr->IsInSignUpTime())
		{
			ret = RE_GUILD_LADDER_NOT_IN_SIGNUP_TIME;
			break;
		}
		if (!guild->IsElder(pkPlayer->GetInstance()) && !guild->IsViceMaster(pkPlayer->GetInstance()) &&
			!guild->IsMaster(pkPlayer->GetInstance()))
		{
			ret = RE_GUILD_LADDER_AUTH_NOENOUGH;
			break;
		}
		mem::vector<PS_CostEntry> entry;
		PS_CostEntry temp;
		temp.m_ItemType = 0;
		temp.m_ItemCount = pkLadderMgr->GetSignUpNeedMoney();
		entry.push_back(temp);
		ret = guild->CheckCost(entry);
		if (ret != RE_SUCCESS)
		{
			break;
		}
	} while(0);
	if (RE_SUCCESS != ret)
	{
		MG_PB<msg_define::guild_ladder_signup_resp> respMsg;
		msg_define::guild_ladder_signup_resp& resp = respMsg.m_value;
		resp.set_result(ret);
		pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_LADDER_SIGNUP, &respMsg);
		return;
	}
	MG_PB<msg_define::guild_ladder_signup_req> reqMsg;
	msg_define::guild_ladder_signup_req& req = reqMsg.m_value;
	req.set_uid(guild->GetUid());
	req.set_name(TPSTR2STDSTR(guild->GetName()));
	req.set_level(guild->GetLevel());
	req.set_battle(guild->GetActiveBattle());
	SendMsgToGlobal(MGPT_REQ_GUILD_LADDER_SIGNUP, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GUILD_LADDER_BASE_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	LC_GuildLadderManager* pkLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
	if (NULL == pkLadderMgr)
	{
		return;
	}
	MG_PB<msg_define::guild_ladder_base_info_resp> respMsg;
	msg_define::guild_ladder_base_info_resp& resp = respMsg.m_value;
	if (guild->IsLadderInfoValid(pkLadderMgr->GetConfig()))
	{
		guild->FillLadderInfo(resp);
		resp.set_next_time(pkLadderMgr->GetConfig().GetNextTime(time(NULL)));
		pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_LADDER_BASE_INFO,  &respMsg);
	}
	else
	{
		MG_PB<msg_define::guild_ladder_base_info_req> reqMsg;
		msg_define::guild_ladder_base_info_req& req = reqMsg.m_value;
		req.set_uid(guild->GetUid());
		SendMsgToGlobal(MGPT_REQ_GUILD_LADDER_BASE_INFO, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
}

ON_HANDLE(MGPT_REQ_GUILD_LADDER_RECORDS, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	if (guild->GetUid() == INVALID_UNIQUE_ID)
	{
		return;
	}
	MG_PB<msg_define::guild_ladder_record_req> reqMsg;
	reqMsg.m_value.set_guild_uid(guild->GetUid());
	SendMsgToGlobal(MGPT_REQ_GUILD_LADDER_RECORDS, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GUILD_LADDER_RANK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_PB<msg_define::guild_ladder_rank_req> reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}

	msg_define::guild_ladder_rank_req& req = reqMsg.m_value;
	req.set_uid(guild->GetUid());
	SendMsgToGlobal(MGPT_REQ_GUILD_LADDER_RANK, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GUILD_LADDER_ENTER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	unique_id_impl kGuildUID = 1;
	static LC_GuildLadderManager* pkLadderGuildMgr = LC_GuildLadderManager::GetSingletonPtr();
	if (NULL == pkLadderGuildMgr)
	{
		return;
	}
	int32_t iTargetServer = pkLadderGuildMgr->GetTargetServerByGuildUID(kGuildUID);
	if (iTargetServer <= 0)
	{
		return;
	}
	MG_PB<msg_define::guild_ladder_ss_enter_req> kReqMsg;
	msg_define::guild_ladder_ss_enter_req& msg = kReqMsg.m_value;
	Protocol::PS_CharBaseInfoLite kLiteInfo; 
	char_data_define::char_base_info kBaseInfo;
	char_data_define::char_full_info kFullInfo;
	pkPlayer->SaveCharPB(kLiteInfo, kBaseInfo, kFullInfo);
	GameServerApp* pkGameServerApp = GetGameServerApp();
	if (NULL == pkGameServerApp)
	{
		return;
	}
	Protocol::MG_PACKET_ENCODE_HANDLER& kPakcetHandler = pkGameServerApp->GetPacketHandler();
	kPakcetHandler.Clear();
	MG_OUTPUT_CDR kLiteInfoStream(kPakcetHandler.GetBuff(), SIZE_MSG_BUFF);
	if (!kLiteInfo.Compress(&kLiteInfoStream))
	{
		return;
	}
	if (!kLiteInfoStream.GoodBit())
	{
		return;
	}

	msg.set_lite_info(kPakcetHandler.GetBuff(), kLiteInfoStream.GetMsgLength());
	if (!kBaseInfo.SerializeToString(msg.mutable_full_info()))
	{
		return;
	}
	if (msg.full_info().size() >= 0xC000)
	{
		static std::string _zipbuf;
		uLongf length = ::compressBound((uLong)msg.full_info().size());
		_zipbuf.resize((size_t)length);

		if (Z_OK == compress((Bytef*)(Utility::get_buff_ptr(_zipbuf)), (uLongf*)&length, (Bytef*)msg.full_info().c_str(), (uLong)msg.full_info().size()))
		{
			_zipbuf.resize(length);
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Guild Ladder SaveCharPB ZIP", pkPlayer->GetID(), pkPlayer->GetInstance(), msg.full_info().size(), length);
			kBaseInfo.set_data_flag(1);
			msg.set_full_info(_zipbuf);
		}
	}
	if (!kBaseInfo.SerializeToString(msg.mutable_base_info()))
	{
		return;
	}
	msg.set_object_id(pkPlayer->GetID());
	SendMsgToGame(iTargetServer, MGPT_G2G_GUILD_LADDER_ENTER_REQ, &kReqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstanceUniqueID(), rkMsgHead.m_iSequence);
}

ON_HANDLE(MGPT_REQ_GUILD_LADDER_LEAVE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	
}

ON_HANDLE(MGPT_REQ_ACTIVITYGUILDBOSSMAP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ActivityGuildBossMap kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	MG_RLT_ActivityGuildBossMap rltMsg;
	rltMsg.m_nMapID = kMsg.m_nMapID;
	rltMsg.m_nResult = pkPlayer->ActivityGuildBossMap(kMsg.m_nMapID);
	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_ACTIVITYGUILDBOSSMAP, &rltMsg);
	}
}

}
