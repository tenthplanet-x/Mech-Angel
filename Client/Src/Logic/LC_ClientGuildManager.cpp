#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "LC_ClientGuildManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientNetPlayerManager.h"
#include "NW_Helper.h"

#include "NW_ClientSessionManager.h"
#include "MG_MessageBase.h"
#include "UT_LogicInfoMessage.h"
#include "LC_GuildRaidDataManager.h"

#include "UI_UIManager.h"
#include <functional>

//#include "CF_GuildSkill.h"
//#include "CF_GuildMaterial.h"
//#include "CF_Milestone.h"
#include "CF_ItemCategory.h"
#include "CF_ItemList.h"
#include "CF_GuildLevel.h"
#include "CF_SkillStateList.h"
#include "TT_TextFilter.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;

LC_ClientGuildManager::LC_ClientGuildManager()
: m_bReqUnionInfo(false)//m_bREQSearchGuildInfo(false)
, m_iSelectedSkillIndex(0)
, m_lDonateMaterialID(0)
, m_iPage(0)
{
	::memset(applyPlayerId, 0, sizeof(applyPlayerId));
}

LC_ClientGuildManager::~LC_ClientGuildManager()
{

}

ResultType LC_ClientGuildManager::RequestGuildAsset()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( pkGamePlayer )
	{		
		MG_REQ_GuildAsset kMsg;
		kMsg.m_uiCharID = pkGamePlayer->GetCommanderUID();

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_ASSET, &kMsg);

		if(bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			return RE_SUCCESS;
		}
	}

	return RE_FAIL;
}

ResultType LC_ClientGuildManager::RequestCreateGuild(const StringType& strGuildName,const StringType& strGuildDeclare, int iCreatType)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	// 是否有不合法字符
	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
	if (pkTextFilter)
	{
		pkTextFilter->SetText(strGuildName);
		if ( pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAR_NAME,false, true) )
		{
			//SendLogicInfoMessage(RE_CREATE_GUILD_FAIL_NAME_IS_WRONG);
			return RE_CREATE_GUILD_FAIL_NAME_IS_WRONG;
		}
	}

	ResultType iResult = pkGamePlayer->CanCreateGuild(strGuildName);

	if ( iResult == RE_SUCCESS )
	{
		MG_REQ_CreateGuild kMsg;

		kMsg.m_uiCharID = pkGamePlayer->GetCommanderUID();
		TPSTR2STDSTR(strGuildName, kMsg.m_szName);
		kMsg.m_CostType = iCreatType;
		TPSTR2STDSTR(strGuildDeclare, kMsg.m_szGuildDeclaration);

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CREATE_GUILD, &kMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}
	else
	{
		//SendLogicInfoMessage(iResult);
	}

	return iResult;
}

ResultType LC_ClientGuildManager::RequestApplyGuild(guild_id_type lGuildID, int iGuildLevel)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	ResultType iResult = pkGamePlayer->CanApplyGuild(true);
	if (iResult == RE_SUCCESS)
	{
		MG_REQ_ApplyGuild kMsg;

		kMsg.m_uiCharID  = 0;
		kMsg.m_uiGuildID = lGuildID;
		kMsg.m_Level = iGuildLevel;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_APPLY_GUILD, &kMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}

	return iResult;
}

ResultType LC_ClientGuildManager::RequestComfirmApplyGuild(int iIndex, bool bAgree)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}	

	// 请求结果
	LC_GuildApplyEntry* pkApplyEntry = GetGuildAsset()->GetGuildApplyInfo().GetApplyEntry(iIndex);
	if ( pkApplyEntry && pkApplyEntry->GetValid() )
	{
		MG_REQ_ConfirmApplyGuild  kRltMsg;

		kRltMsg.m_uiCharID			= pkGamePlayer->GetCommanderUID();
		kRltMsg.m_uiApplyCharID		= pkApplyEntry->GetCharID();
		TPSTR2STDSTR(pkApplyEntry->GetName(), kRltMsg.m_szApplyCharName);
		kRltMsg.m_bAgree			= bAgree;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CONFIRM_APPLY_GUILD, &kRltMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestComfirmApplyAllGuildEntry(bool bAgree)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}
	//备份一下申请列表，以便不重复发送请求
	for(int i=0; i<MAX_GUILD_APPLY_NUM; ++i)
	{
		applyPlayerId[i] = GetGuildAsset()->GetGuildApplyInfo().GetApplyEntry(i)->GetCharID();
	}

	return RequestComfirmApplyAllGuild(0,true);
}


ResultType LC_ClientGuildManager::RequestComfirmApplyAllGuild(int Index, bool bAgree)
{
	LC_GuildApplyEntry* pkApplyEntry = NULL;
	for(int i=0;i<MAX_GUILD_APPLY_NUM;++i)
	{
		if(GetGuildAsset()->GetGuildApplyInfo().GetApplyEntry(i)->GetCharID() == applyPlayerId[Index])
		{
			pkApplyEntry = GetGuildAsset()->GetGuildApplyInfo().GetApplyEntry(i);
			applyPlayerId[Index] = INVALID_ACTOR_ID;
			break;
		}
	}
	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	if ( pkApplyEntry && pkApplyEntry->GetValid() )
	{

		MG_REQ_ConfirmApplyGuild  kRltMsg;

		kRltMsg.m_uiCharID			= pkGamePlayer->GetCommanderUID();
		kRltMsg.m_uiApplyCharID		= pkApplyEntry->GetCharID();
		TPSTR2STDSTR(pkApplyEntry->GetName(), kRltMsg.m_szApplyCharName);
		kRltMsg.m_bAgree			= bAgree;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CONFIRM_APPLY_GUILD, &kRltMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestForbidApply()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	bool bForbid = !( GetGuildAsset()->GetGuildInfo().GetForbidApply() );

	{
		MG_REQ_ForbidGuildApply kReqMsg;

		kReqMsg.m_uiCharID	= pkGamePlayer->GetCommanderUID();
		kReqMsg.m_bIsForbidApply = bForbid;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_FORBID_GUILD_APPLY, &kReqMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}
	
	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestClearApplyList()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	{
		MG_REQ_ClearGuildApplyList kReqMsg;

		kReqMsg.m_uiCharID	= pkGamePlayer->GetCommanderUID();

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CLEAR_GUILD_APPLYLIST, &kReqMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestAutoAddApplyList(bool bCheck, guild_id_type guildId)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	MG_Notify_GuildApply_AutoAdd kReqMsg;

	kReqMsg.m_AutoAdd = bCheck;
	kReqMsg.m_GuildId = guildId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_AUTOADD_CHANGE, &kReqMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}

	return RE_SUCCESS;
}

void LC_ClientGuildManager::RequestExitGuild()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return;
	}

	MG_REQ_ExitGuild kMsg;

	kMsg.m_uiCharID  = pkGamePlayer->GetCommanderUID();
	kMsg.m_uiGuildID = pkGamePlayer->GetGuildID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXIT_GUILD, &kMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

ResultType LC_ClientGuildManager::RequestModifyDeclaration(StringType& strGuildDeclaration)
{
	StringType::iterator kStrIter = strGuildDeclaration.begin();
	for (;kStrIter != strGuildDeclaration.end();)
	{
		if (*kStrIter == char(10))
		{
			kStrIter = strGuildDeclaration.erase(kStrIter);
		}
		else
		{
			++kStrIter;
		}
	}
	// 是否有不合法字符
	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
	if (pkTextFilter)
	{
		pkTextFilter->SetText(strGuildDeclaration);
		if ( pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAT,false, true) )
		{
			SendLogicInfoMessage(RE_MODIFY_GUILD_FAILED_WRONG_CHAR);
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_GUILD_DECLARATION_MODIFY_FAILED);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
			return RE_MODIFY_GUILD_FAILED_WRONG_CHAR;
		}
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	ResultType iResult = pkGamePlayer->CanModifyGuildDeclaration(&m_kGuildAsset);
	if (iResult == RE_SUCCESS)
	{
		MG_REQ_ModifyGuildDeclaration kMsg;

		kMsg.m_uiCharID  = pkGamePlayer->GetCommanderUID();
		//kMsg.m_uiGuildID = pkGamePlayer->GetGuildID();
		TPSTR2STDSTR(strGuildDeclaration, kMsg.m_szDeclaration);

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MODIFY_GUILD_DECLARATION, &kMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}

	return iResult;
}

ResultType LC_ClientGuildManager::RequestAppoint(unique_id_type lTargetID, const StringType& strName, long lOfficialPostion)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	if (pkGamePlayer->GetGuildID() == INVALID_GUILD_ID)
	{
		SendLogicInfoMessage(RE_GUILD_APPOINT_FAILED_NOT_EXIST_GUILD);
		return RE_GUILD_APPOINT_FAILED_NOT_EXIST_GUILD;
	}

	// 非掌门不能 任命掌门
	if (lOfficialPostion == GUILD_MEMBER_MASTER)
	{
		if ( GetPersonalInfo() && GetPersonalInfo()->GetOfficialPosition() != GUILD_MEMBER_MASTER) 
		{			
			SendLogicInfoMessage(RE_GUILD_APPOINT_FAILED_NOT_MASTER);
			return RE_GUILD_APPOINT_FAILED_NOT_MASTER;
		}
	}

	{
		MG_REQ_GuildAppoint kMsg;

		kMsg.m_uiCharID = pkGamePlayer->GetCommanderUID();
		kMsg.m_uiTargetCharID = lTargetID;
		kMsg.m_byOfficialPostion = static_cast <uint8_t> (lOfficialPostion);
		TPSTR2STDSTR(strName, kMsg.m_szTargetCharName);

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_APPOINT, &kMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestKickMember(unique_id_type lTargetID)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	ResultType iResult = pkGamePlayer->CanKickMemberFromGuild(&m_kGuildAsset);
	if (iResult == RE_SUCCESS)
	{
		/*
		// 获取自己的职位
		if (NULL == GetPersonalInfo())
		{
			return RE_FAIL;
		}
		long lOfficialSelfPosition = GetPersonalInfo()->GetOfficialPosition();

		// 获取目标玩家的职位
		long lOfficialTargetPosition = GUILD_MEMBER_INVALID;
		LC_GuildMemberInfo& kAllMemberInfo = GetGuildAsset()->GetGuildMemberInfo();
		for (int i = 0; i < MAX_GUILD_MEMBER_NUM; ++i)
		{
			LC_GuildMemberEntry* pkMember = kAllMemberInfo.GetMemberEntry(i);
			if (pkMember && pkMember->GetTypeID() == lTargetID)
			{
				lOfficialTargetPosition = pkMember->GetOfficialPosition();
				break;
			}
		}
	
		if (GUILD_POSITION_LEVEL[lOfficialSelfPosition] <= GUILD_POSITION_LEVEL[lOfficialTargetPosition])
		{
			SendLogicInfoMessage(RE_GUILD_KICKMEMBER_FAILED_POWER);
			return RE_GUILD_KICKMEMBER_FAILED_POWER;
		}
		*/

		MG_REQ_KickMemberFromGuild kMsg;

		kMsg.m_uiCharID = pkGamePlayer->GetCommanderUID();
		kMsg.m_uiGuildID = pkGamePlayer->GetGuildID();
		kMsg.m_uiTargetCharID = lTargetID;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_KICKMEMBER, &kMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestDismissGuild()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	ResultType iResult = pkGamePlayer->CanDismissGuild(&m_kGuildAsset);
	if (iResult == RE_SUCCESS)
	{
		MG_REQ_DismissGuild kMsg;

		kMsg.m_uiCharID = pkGamePlayer->GetCommanderUID();

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DISMISS_GUILD, &kMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}

	return RE_SUCCESS;
}

//ResultType LC_ClientGuildManager::RequestGuildInviteList()
//{
//	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
//	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
//
//	if ( !pkGamePlayer )
//	{
//		return RE_FAIL;
//	}
//
//	ResultType iResult = pkGamePlayer->CanGetInviteList();
//	if (iResult == RE_SUCCESS)
//	{
//		MG_REQ_GuildInviteList kMsg;
//		kMsg.m_uiCharID = pkGamePlayer->GetInstance();
//
//		MG_PACKET_ENCODE_HANDLER hMsgHandler;
//		bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_GUILD_INVITE_LIST, &kMsg);
//		if (bCreateMsg)
//		{
//			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
//		}
//	}
//	else
//	{
//		SendLogicInfoMessage(iResult);
//	}
//
//	return RE_SUCCESS;	
//}

ResultType LC_ClientGuildManager::RequestInviteToGuild(unique_id_type lTargetID)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	ResultType iResult = pkGamePlayer->CanInvite();
	if (iResult == RE_SUCCESS && lTargetID > 0 )
	{
		MG_REQ_InviteToGuild kMsg;

		kMsg.m_uiCharID			= pkGamePlayer->GetCommanderUID();
		kMsg.m_uiBeInviteCharID = lTargetID;
		kMsg.m_uiGuildID		= pkGamePlayer->GetGuildID();

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_INVITE_TO_GUILD, &kMsg );
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}

		SendLogicInfoMessage(RE_GROUP_INVITE_SEND_SUCCESS);
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestConfirmInviteToGuild(guild_id_type lGuildID, unique_id_type lInvitePlayer, bool bAgree)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}
	
	{
		MG_RLT_ConfirmInviteToGuild kMsg;

		kMsg.m_uiInviteCharID		= lInvitePlayer;
		kMsg.m_uiGuildID			= lGuildID;
		kMsg.m_uiBeInviteCharID		= pkGamePlayer->GetCommanderUID();
		kMsg.m_bAgree				= bAgree;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_RLT_CONFIRM_INVITE_TO_GUILD, &kMsg );
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}
	}
	return RE_SUCCESS;
}

ResultType LC_ClientGuildManager::RequestUpgradeGuild(void)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_UPGRADE_GUILD, NULL);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

void LC_ClientGuildManager::RequestGuildRankNumber()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_SearchGuildRank msg;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SEARCH_GUILD_RANK, &msg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

LC_ClientGuildMilestoneStruct* LC_ClientGuildManager::GetGuildMilestone()
{
	return &m_kGuildMilestone;
}

LC_ClientGuildSkillInfoStruct* LC_ClientGuildManager::GetGuildSkillInfo()
{
	return &m_kGuildSkillInfo;
}

//ResultType LC_ClientGuildManager::RequestGuildSkill()
//{
//	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
//	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
//
//	if ( !pkGamePlayer )
//	{
//		return RE_FAIL;
//	}
//
//	if (pkGamePlayer->GetGuildID() == INVALID_GUILD_ID)
//	{
//		SendLogicInfoMessage(RE_GUILD_GET_SKILL_FAILED_NOT_EXIST_GUILD);
//		return RE_GUILD_GET_SKILL_FAILED_NOT_EXIST_GUILD;
//	}
//
//	// 发送请求信息
//	MG_REQ_GuildSkillInfo kMsg;
//	kMsg.m_uiCharID = pkGamePlayer->GetInstance();
//	
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_GUILD_SKILL, &kMsg );
//	if (bCreateMsg)
//	{
//		pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
//	}
//
//	return RE_SUCCESS;
//}

ResultType LC_ClientGuildManager::RequestDonateMaterial(int64_t lMaterialNum, bool bMoney)
{	
	if (lMaterialNum == 0)
	{
		SendLogicInfoMessage(RE_GUILD_DONATE_MATERIALS_FAILED_IS_ZERO);
		return RE_GUILD_DONATE_MATERIALS_FAILED_IS_ZERO;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	ResultType iResult = pkGamePlayer->CanDonateGuildMaterials(m_lDonateMaterialID, lMaterialNum);
	if (iResult == RE_SUCCESS || bMoney)
	{
		// 需要判断能否捐献材料	
		//LC_ClientGuildSingleSkill* pkGuildSkill = GetGuildSkillInfo()->GetSingleSkillInfo(m_iSelectedSkillIndex);
		//if ( pkGuildSkill )
		//{
		//	int iCurSkillIndex = GetGuildSkillInfo()->GetSingleSkillIndex( pkGuildSkill->GetSkillID() );
		//	if (iCurSkillIndex < 0)
		//	{
		//		return RE_FAIL;
		//	}
		//	// 查看能否学习技能

		MG_REQ_DonateGuildMaterials kMsg;

		kMsg.m_uiCharID = pkGamePlayer->GetCommanderUID();
			//kMsg.m_uiSkillID = pkGuildSkill->GetSkillID();
			//kMsg.m_usSkillIndex	= iCurSkillIndex;
		kMsg.m_uiMaterailTypeID	= !bMoney ? m_lDonateMaterialID : GUILD_ASSET_TYPE_MONEY;
		kMsg.m_uiMaterailNumber	= lMaterialNum;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_DONATE_GUILD_MATERIALS, &kMsg );
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
		}

		return RE_SUCCESS;		
	}
	else
		SendLogicInfoMessage(iResult);

	return RE_FAIL;
}

bool LC_ClientGuildManager::CheckLearnGuildSkill(int iSkillIndex)
{
  return false;
  /*
	CF_GuildSkill* pkGuildSkill = CF_GuildSkill::GetSingletonPtr();
	if (!pkGuildSkill)
		return false;

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
		return false;

	// 1. 获取到当前技能ID
	LC_ClientGuildSingleSkill* pkSkill = GetGuildSkillInfo()->GetSingleSkillInfo( iSkillIndex );
	if ( NULL != pkSkill )
	{
		bool bIsLearned = false;

		// 查看当前研发技能是否已经研发成功
		long lCurrentSkillID = pkSkill->GetSkillID();

		// 获取当前技能的编号
		int iCurSkillIndex = GetGuildSkillInfo()->GetSingleSkillIndex(lCurrentSkillID);
		if (iCurSkillIndex < 0)
		{
			return false;
		}

		std::vector<long> vecCanLearnSkillList;
		if ( pkSkill->GetSkillExp() >= pkSkill->GetMaxSkillExp() )
		{
			// 如果已经研发成功加入到技能研发列表
			vecCanLearnSkillList.push_back( lCurrentSkillID );
		}

		// 遍历出该技能已经研发的低级技能
		long lPrevSkillID = 0;

		do 
		{			
			const CF_GuildSkill::DataEntry* pkDataEntryTemp = pkGuildSkill->GetEntryPtr( lCurrentSkillID );
			if (pkDataEntryTemp)
			{
				// 找到该技能
				lPrevSkillID = pkDataEntryTemp->_lPrevSkillID;
				if (lPrevSkillID != 0)
				{
					vecCanLearnSkillList.push_back(lPrevSkillID);
					lCurrentSkillID = lPrevSkillID;
				}
			}
		} while (lPrevSkillID != 0);

		// 如果可学技能数目为0，则表示还不能学习门派技能
		if (vecCanLearnSkillList.size() == 0)
		{
			return false;
		}

		// 是否引进学习过该技能
		bool bTargetIsLearnedSkill = pkGamePlayer->GetSkillAsset()->IsLearnedSkill(lCurrentSkillID);

		// 查看玩家已经学习了那一级技能
		bool bFind = false;
		size_t  iPos = 0;	// 已学习技能在可学技能列表中的位置		
		for ( size_t i = 0; i < vecCanLearnSkillList.size(); ++i )
		{
			long lTempSkillID = vecCanLearnSkillList.at(i);

			bool bIsLearned = pkGamePlayer->GetSkillAsset()->IsLearnedSkill(lTempSkillID, true);
			if ( bIsLearned )
			{
				iPos = i;
				bFind = true;
				break;
			}
		}

		// 下一级技能位置
		size_t iNextSkillPos = 0;
		if ( !bFind )
		{
			if ( bTargetIsLearnedSkill )
			{
				return false;
			}
			else
			{
				// 学习第一个技能
				iNextSkillPos = vecCanLearnSkillList.size() - 1;
			}
		}
		else
		{
			if (iPos == 0)
			{
				return false;
			}
			else
			{
				iNextSkillPos = iPos - 1;
			}			
		}

		long lNextSkillID = 0;
		if ( iNextSkillPos >= 0 && iNextSkillPos < vecCanLearnSkillList.size() )
		{
			lNextSkillID = vecCanLearnSkillList.at(iNextSkillPos);
		}

		//// 获取该技能的贡献度
		//long lNeedContribute = 0;
		//{
		//	const CF_GuildSkill::DataEntry* pkDataEntry = pkGuildSkill->GetEntryPtr( lNextSkillID );
		//	if ( pkDataEntry )
		//	{
		//		lNeedContribute = pkDataEntry->_lLeanNeedContribute;
		//	}
		//}

		//// 参看当前自己的贡献度，能否够学习该技能				
		//if (GetPersonalInfo()->GetCurrentContribute() < lNeedContribute )
		//{
		//	// 贡献度不足无法学习该技能
		//	return false;
		//}
	}
	return true;
  */
}

//ResultType LC_ClientGuildManager::RequestLearnGuildSkill()
//{
//	CF_GuildSkill* pkGuildSkill = CF_GuildSkill::GetSingletonPtr();
//	if (!pkGuildSkill)
//	{
//		return RE_FAIL;
//	}
//
//	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
//	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
//
//	if ( !pkGamePlayer )
//	{
//		return RE_FAIL;
//	}
//	
//	// 1. 获取到当前技能ID
//	LC_ClientGuildSingleSkill* pkSkill = GetGuildSkillInfo()->GetSingleSkillInfo( m_iSelectedSkillIndex );
//	if ( NULL != pkSkill )
//	{
//		bool bIsLearned = false;
//		
//		// 查看当前研发技能是否已经研发成功
//		long lCurrentSkillID = pkSkill->GetSkillID();
//
//		// 获取当前技能的编号
//		int iCurSkillIndex = GetGuildSkillInfo()->GetSingleSkillIndex(lCurrentSkillID);
//		if (iCurSkillIndex < 0)
//		{
//			return RE_FAIL;
//		}
//
//		std::vector<long> vecCanLearnSkillList;
//		if ( pkSkill->GetSkillExp() >= pkSkill->GetMaxSkillExp() )
//		{
//			// 如果已经研发成功加入到技能研发列表
//			vecCanLearnSkillList.push_back( lCurrentSkillID );
//		}
//
//		// 遍历出该技能已经研发的低级技能
//		long lPrevSkillID = 0;
//		
//		do 
//		{			
//			const CF_GuildSkill::DataEntry* pkDataEntryTemp = pkGuildSkill->GetEntryPtr( lCurrentSkillID );
//			if (pkDataEntryTemp)
//			{
//				// 找到该技能
//				lPrevSkillID = pkDataEntryTemp->_lPrevSkillID;
//				if (lPrevSkillID != 0)
//				{
//					vecCanLearnSkillList.push_back(lPrevSkillID);
//					lCurrentSkillID = lPrevSkillID;
//				}
//			}
//		} while (lPrevSkillID != 0);
//
//		// 如果可学技能数目为0，则表示还不能学习门派技能
//		if (vecCanLearnSkillList.size() == 0)
//		{
//			SendLogicInfoMessage(RE_GUILD_LEARN_SKILL_FAILED_GUILD_SKILL_NOT_RESEARCH);
//			return RE_GUILD_LEARN_SKILL_FAILED_GUILD_SKILL_NOT_RESEARCH;
//		}
//		
//		// 是否引进学习过该技能
//		bool bTargetIsLearnedSkill = pkGamePlayer->GetSkillAsset()->IsLearnedSkill(lCurrentSkillID);
//
//		// 查看玩家已经学习了那一级技能
//		bool bFind = false;
//		size_t  iPos = 0;	// 已学习技能在可学技能列表中的位置		
//		for ( size_t i = 0; i < vecCanLearnSkillList.size(); ++i )
//		{
//			long lTempSkillID = vecCanLearnSkillList.at(i);
//
//			bool bIsLearned = pkGamePlayer->GetSkillAsset()->IsLearnedSkill(lTempSkillID, true);
//			if ( bIsLearned )
//			{
//				iPos = i;
//				bFind = true;
//				break;
//			}
//		}
//		
//		// 下一级技能位置
//		size_t iNextSkillPos = 0;
//		if ( !bFind )
//		{
//			if ( bTargetIsLearnedSkill )
//			{
//				SendLogicInfoMessage(RE_GUILD_LEARN_SKILL_FAILED_SKILL_IS_HIGH);
//				return RE_GUILD_LEARN_SKILL_FAILED_SKILL_IS_HIGH;
//			}
//			else
//			{
//				// 学习第一个技能
//				iNextSkillPos = vecCanLearnSkillList.size() - 1;
//			}
//		}
//		else
//		{
//			if (iPos == 0)
//			{
//				// 技能已经学习到最高级，无需继续学习
//				SendLogicInfoMessage(RE_GUILD_LEARN_SKILL_FAILED_GUILD_SKILL_HAS_LEARNED);
//				return RE_GUILD_LEARN_SKILL_FAILED_GUILD_SKILL_HAS_LEARNED;
//			}
//			else
//			{
//				iNextSkillPos = iPos - 1;
//			}			
//		}
//		
//		long lNextSkillID = 0;
//		if ( iNextSkillPos >= 0 && iNextSkillPos < vecCanLearnSkillList.size() )
//		{
//			lNextSkillID = vecCanLearnSkillList.at(iNextSkillPos);
//		}
//		
//		// 获取该技能的贡献度
//		long lNeedContribute = 0;
//		{
//			const CF_GuildSkill::DataEntry* pkDataEntry = pkGuildSkill->GetEntryPtr( lNextSkillID );
//			if ( pkDataEntry )
//			{
//				lNeedContribute = pkDataEntry->_lLeanNeedContribute;
//			}
//		}
//		
//		// 参看当前自己的贡献度，能否够学习该技能				
//		if (GetPersonalInfo() && GetPersonalInfo()->GetCurrentContribute() < lNeedContribute )
//		{
//			// 贡献度不足无法学习该技能
//			SendLogicInfoMessage(RE_GUILD_LEARN_SKILL_FAILED_CONTRIBUTE_NOT_ENOUGH);
//			return RE_GUILD_LEARN_SKILL_FAILED_CONTRIBUTE_NOT_ENOUGH;
//		}
//
//		// 请求学习相关门派技能
//		{
//			MG_REQ_LearnGuildSkill kMsg;
//
//			kMsg.m_uiCharID		= pkGamePlayer->GetInstance();
//			kMsg.m_uiSkillID	= lNextSkillID;
//			kMsg.m_uiContribute	= lNeedContribute;
//			kMsg.m_usSkillIndex = iCurSkillIndex;
//
//			MG_PACKET_ENCODE_HANDLER hMsgHandler;
//			bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_LEARN_GUILD_SKILL, &kMsg );
//			if (bCreateMsg)
//			{
//				pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
//			}
//		}
//	}
//
//	return RE_SUCCESS;
//}


ResultType GameLogic::LC_ClientGuildManager::RequestSearchGuild( int iIndex /*= 0*/ ,int eSortType, int eGuildSortCol, bool bAutoAdd, int iNumPerPage)
{

	/*if (m_bREQSearchGuildInfo)
		return RE_FAIL;*/

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
		return RE_FAIL;

	MG_REQ_GuildSearch kMsg;
	kMsg.m_uiIndex = iIndex;
	kMsg.m_SortType = eSortType;
	kMsg.m_SortCol = eGuildSortCol;
	kMsg.m_AutoAdd = bAutoAdd;
	kMsg.m_SearchNum = iNumPerPage;
	//switch (iIndex)
	//{
	//case FIRST_PAGE:
	//	kMsg.m_uiIndex = m_iPage<1?0:(m_iPage-1)*10+1;
	//	break;
	//case UP_PAGE:
	//	kMsg.m_uiIndex = 0;
	//	break;
	//case DOWN_PAGE:
	//	kMsg.m_uiIndex = (m_iPage)*10+1;
	//	break;
	//}
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_SEARCH_GUILD, &kMsg );
	if (bCreateMsg)
		pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );

	//m_bREQSearchGuildInfo = true;

	return RE_SUCCESS;
}

void LC_ClientGuildManager::SetSelectedSkillIndex(int iSkillIndex)
{
	m_iSelectedSkillIndex = iSkillIndex;
}

ResultType LC_ClientGuildManager::AddDonateMaterial(int iPackType, int iIndex)
{	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(iPackType, iIndex);
	if (!pkEntry)
	{
		return RE_FAIL;
	}

	if( pkEntry->GetValid() )
	{
		long lMaterialID = pkEntry->GetTypeID();
		m_lDonateMaterialID = lMaterialID;
		return RE_SUCCESS;

		//CF_GuildMaterial* pkGuildMaterial = CF_GuildMaterial::GetSingletonPtr();
		//if ( pkGuildMaterial )
		//{
		//	CF_GuildMaterial::DataEntry* pkEntry = pkGuildMaterial->GetEntryPtr(lMaterialID);
		//	if (pkEntry)
		//	{
		//		m_lDonateMaterialID = lMaterialID;
		//		return RE_SUCCESS;
		//	}
		//	else
		//	{
		//		SendLogicInfoMessage(RE_GUILD_DONATE_MATERIALS_FAILED_NOT_RIGHT);
		//		return RE_GUILD_DONATE_MATERIALS_FAILED_NOT_RIGHT;
		//	}
		//}
	}

	return RE_FAIL;
}

long LC_ClientGuildManager::GetDonateMaterialID()
{
	return m_lDonateMaterialID;
}

void LC_ClientGuildManager::ClearDonateMaterial()
{
	m_lDonateMaterialID = INVALID_ITEM_ID;
}

void LC_ClientGuildManager::UpdateGuildInfo(const Protocol::PS_GuildInfo& kGuildInfo)
{
	//// 更新技能信息
	//{		
	//	m_kGuildSkillInfo.UpdateResearchInfo( 
	//		m_kGuildAsset.GetGuildInfo().GetBuildingLevel(GUILD_BUILDING_GATHERROOM) ,
	//		m_kGuildAsset.GetGuildInfo().GetBuildingLevel(GUILD_BUILDING_CLASSICSBOOKROOM));
	//}

	// 更新门派信息
	//上一次的GuildID
	LC_GuildInfo& GuildInfo = m_kGuildAsset.GetGuildInfo();
	guild_id_type iGuildID = GuildInfo.GetGuildID();

	GuildInfo.GetFromProtocol(kGuildInfo);

	RequestGuildRankNumber();  // 公会信息初始化时 增加申请查询排名信息
	
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	//只有第一次进入游戏才在聊天框显示公会宣言, 此时iGuildID为0
	if (pkGamePlayer != NULL && iGuildID != GuildInfo.GetGuildID() && GuildInfo.GetDeclaration().length())
	{
		//显示门派宣言
		TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
		if (pkTextFilter == NULL)
			return;

		pkTextFilter->SetText(GuildInfo.GetDeclaration());
		pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAT);
		SendChatMsg(RE_CHAT_GUILD,pkTextFilter->GetText(),"",LC_Helper::GetSystemTimeString(),GUILD_DECLARATION,0);

	}
	// 更新大事记	
	/*{
		CF_Milestone* milestonePtr = CF_Milestone::GetSingletonPtr();
		if (milestonePtr)
		{
			CF_Milestone::DataEntry* pkData = milestonePtr->GetEntryPtr(kGuildInfo.m_iMileStone);
			if (pkData && m_kGuildAsset.GetGuildInfo().GetBuildingLevel(GUILD_BUILDING_GATHERROOM) >= (uint32_t)(pkData->_lLevelLimit))
			{
				_updateGuildMilestone(kGuildInfo.m_iMileStone);
			}
		}
	}*/

	if ( pkGamePlayer )
	{
		pkGamePlayer->SetGuildID(kGuildInfo.uiGuildID);	
		pkGamePlayer->SetGuildName(STDSTR2TPSTR(kGuildInfo.szName));
	}

	LC_GuildRaidDataManager::GetSingleton().UpdateGuildInfo(kGuildInfo);
}

void LC_ClientGuildManager::UpdateGuildRankInfo(uint32_t guildRank)
{
	LC_GuildInfo& GuildInfo = GetGuildAsset()->GetGuildInfo();
	uint32_t iGuildRank = GuildInfo.GetGuildRank();

	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();

	if (iGuildRank != guildRank)
	{
		GuildInfo.SetGuildRank(guildRank);
		GuildInfo.UpdateDirtyFlag();
		if ( pkGamePlayer )
		{
			pkGamePlayer->SetGuildRank(guildRank);	
		}

	}
}

void LC_ClientGuildManager::UpdateAllGuildMemberInfo(const Protocol::PS_GuildAllMemberInfo& kGuildAllMemberInfo)
{
	m_kGuildAsset.GetGuildMemberInfo().GetFromProtocol(kGuildAllMemberInfo);
}

void LC_ClientGuildManager::AddGuildMemberInfo(const Protocol::PS_GuildMemberInfo &kGuildMemberInfo)
{
	m_kGuildAsset.GetGuildMemberInfo().AddGuildMember(kGuildMemberInfo);
}

void LC_ClientGuildManager::DelGuildMemberInfo(unique_id_type iUserID)
{
	StringType sName = m_kGuildAsset.GetGuildMemberInfo().DelGuildMember(iUserID);
	if("" != sName)
		SendLogicInfoMessage(RE_GUILD_MEMBER_EXIT_SUCCESS, sName);
}

void LC_ClientGuildManager::UpdateGuildMemberInfo(const Protocol::PS_GuildMemberInfo &kGuildMemberInfo)
{
	m_kGuildAsset.GetGuildMemberInfo().UpdateGuildMember(kGuildMemberInfo);
}

void LC_ClientGuildManager::UpdateDeclaration(const StringType& strDeclaration)
{
	m_kGuildAsset.GetGuildInfo().SetDeclaration(strDeclaration);

	m_kGuildAsset.GetGuildInfo().SetDirtyFlag(true);
	m_kGuildAsset.GetGuildInfo().AddDirtyFlagRevisionID();
}

void LC_ClientGuildManager::UpdatePosition(const Protocol::PS_GuildPosition &kGuildPosition)
{
	 GuildPositionChangeInfo kInfo = m_kGuildAsset.GetGuildMemberInfo().UpdatePosition(kGuildPosition);
	 if(kInfo.m_bPromote)
	 {
		 SendLogicInfoMessage(RE_GUILD_MEMBER_POSITION_POSITION_PROMOTE, kInfo.m_sMemberName, GUILD_POSITION_2_NAME(kGuildPosition.m_Position));
	 }
	 else
	 {
		 SendLogicInfoMessage(RE_GUILD_MEMBER_POSITION_POSITION_RECEDE, kInfo.m_sMemberName, GUILD_POSITION_2_NAME(kGuildPosition.m_Position));
	 }
}

//void LC_ClientGuildManager::UpdateGuildSkillInfo(const Protocol::PS_GuildSkillInfo& kGuildSkillInfo)
//{
//	m_kGuildSkillInfo.GetFromProtocol(kGuildSkillInfo);
//
//	//更新是否能够研发
//	m_kGuildSkillInfo.UpdateResearchInfo( 
//		m_kGuildAsset.GetGuildInfo().GetBuildingLevel(GUILD_BUILDING_GATHERROOM) ,
//		m_kGuildAsset.GetGuildInfo().GetBuildingLevel(GUILD_BUILDING_CLASSICSBOOKROOM));
//}

void LC_ClientGuildManager::UpdateGuildApplyList(const Protocol::PS_GuildAllApplyInfo& kAllApplyInfo)
{
	m_kGuildAsset.GetGuildApplyInfo().GetFromProtocol(kAllApplyInfo);
}

void LC_ClientGuildManager::AddGuildApply(const Protocol::PS_GuildApplyInfo &kApplyInfo)
{
	m_kGuildAsset.GetGuildApplyInfo().AddGuildApplyInfo(kApplyInfo);

	if ( m_kGuildAsset.GetGuildApplyInfo().GetApplyCount() > 0 )
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_GUILD_APPLY_EVENT);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void LC_ClientGuildManager::DelGuildApply(unique_id_type iUserID)
{
	m_kGuildAsset.GetGuildApplyInfo().DelGuildApplyInfo(iUserID);
}

LC_GuildAsset* LC_ClientGuildManager::GetGuildAsset()
{
	return &m_kGuildAsset;
}

LC_GuildMemberEntry* LC_ClientGuildManager::GetPersonalInfo()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return NULL;
	}

	LC_GuildMemberInfo* pkGuildMemberInfo = m_kGuildAsset.GetGuildMemberInfoPtr();
	if (pkGuildMemberInfo)
	{
		for (int i = 0; i < MAX_GUILD_MEMBER_NUM; ++i)
		{
			LC_GuildMemberEntry* pkMemberEntry = pkGuildMemberInfo->GetMemberEntry(i);
			if ( pkMemberEntry && pkMemberEntry->GetValid() )
			{
				if (pkMemberEntry->GetCharID() == pkGamePlayer->GetCommanderUID())
				{
					return pkMemberEntry;
				}
			}
		}
	}

	return NULL;
}

void GameLogic::LC_ClientGuildManager::UpdateSearchGuildInfo( const Protocol::PS_GuildSearchInfo& kGuildSearchInfo,uint32_t uiIndex )
{
	m_kGuildSearchInfo.UpdateSearchGuildInfo(kGuildSearchInfo);
	m_iPage = uiIndex%10+1;
}

StringType LC_ClientGuildManager::GetGuildLevelTooltip(int iLevel)
{
	CF_GuildLevel::DataEntry *pkGuildLevelEntry = SafeGetCSVFileDataEntryPtr<CF_GuildLevel>(iLevel);
	if(NULL == pkGuildLevelEntry)
		return "";

	LC_GuildInfo *pkGuildInfo = GetGuildAsset()->GetGuildInfoPtr();
	LC_GuildMemberInfo *pkMemberInfo = GetGuildAsset()->GetGuildMemberInfoPtr();

	StringStreamType sStream("");
	sStream << GET_MSG(LC_CLIENTGUILDMANAGER_CPP_001);
	
	//sStream << "<color val=0xffffa300>帮会名称:</color> " << pkGuildInfo->GetName() << "\n";
	//sStream << "<color val=0xffffa300>现任帮主:</color> " << pkMemberInfo->GetMasterName() << "\n";
	//sStream << "<color val=0xffffa300>成立时间:</color> " << pkGuildInfo->GetCreateTimeString() << "\n";

	int iCurLevel = pkGuildInfo->GetLevel();
	//sStream << "<color val=0xffffa300>帮会等级:</color> " << iLevel << "级";
	//if(iCurLevel >= iLevel)
	//	sStream << "<color val=0xff00ee00>(达成)</color>";
	//else
	//	sStream << "<color val=0xffff0000>(未达成)</color>";

	TStringVector kVect = UT_BaseStringUtility::SplitString(pkGuildLevelEntry->_kTooltip, '|');
	//if(kVect.size() == 2)
	{
		//sStream << "<color val=0xffffa300>\n帮会等级加成人物基础属性:</color>\n";

		TStringVector kVect2 = UT_BaseStringUtility::SplitString(kVect[0], '_');
		for (size_t i = 0 ; i < kVect2.size(); ++i)
		{
			sStream << kVect2[i] << "\n";
		}
		
		//sStream << "<color val=0xffffa300>帮会等级收益加成：</color>\n";
		//sStream << "                  " << kVect[1] << "\n";
	}
	
	return sStream.str();
}

ResultType GameLogic::LC_ClientGuildManager::RequestApplyPlayerGuild(unique_id_type lPlayerID)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}

	ResultType iResult = pkGamePlayer->CanApplyGuild(true);
	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
		return iResult;
	}

	if (lPlayerID > 0)
	{
		MG_REQ_ApplyGuild kMsg;

		kMsg.m_uiCharID  = pkGamePlayer->GetCommanderUID();
		kMsg.m_uiTargetCharID = lPlayerID;
		kMsg.m_uiGuildID = 0;
		kMsg.m_Level = 0;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_APPLY_GUILD, &kMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			return RE_SUCCESS;
		}
	}

	return RE_FAIL;
}

ResultType GameLogic::LC_ClientGuildManager::RequestQuickApplyGuild()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	if ( !pkGamePlayer )
	{
		return RE_FAIL;
	}
	MG_REQ_FastApplyGuild kMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_FASTAPPLYGUILD, &kMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		return RE_SUCCESS;
	}
	return RE_FAIL;
}

void GameLogic::LC_ClientGuildManager::Reset()
{
	m_kGuildAsset.ReUse();
	m_kGuildMilestone.Reset();
	m_kGuildSkillInfo.Reset();
	m_kGuildSearchInfo.Reset();

	m_iSelectedSkillIndex = 0;		// 选中技能ID
	m_lDonateMaterialID = 0;		// 捐献材料ID

	//m_bREQSearchGuildInfo = false;
	m_bReqUnionInfo = false;

	m_iPage = 0;
}

void GameLogic::LC_ClientGuildManager::SetDonateMaterailID( int iID )
{
	m_lDonateMaterialID = iID;
}

void GameLogic::LC_ClientGuildManager::UpdateGuildMemberArenaRankInfo( std::vector<int32_t> &viRank )
{
	  m_kGuildAsset.GetGuildMemberInfo().SetGuildMemberArenaRankInfo(viRank);
}

int GameLogic::LC_ClientGuildManager::GetGuildMaxMember(int level)
{
	CF_GuildLevel::DataEntry *pkItem = SafeGetCSVFileDataEntryPtr<CF_GuildLevel>(level);	
	if(!pkItem)
		return 0;
	return pkItem->_MaxPlayer;
}

void LC_ClientGuildManager::_updateGuildMilestone(int32_t iMilestone)
{
	m_kGuildMilestone.Load(iMilestone);
}

int LC_ClientGuildManager::GetFirstValidApplyPlayerIndex()
{
	for(int i=0;i<MAX_GUILD_APPLY_NUM;++i)
	{
		if(applyPlayerId[i]!=INVALID_UNIQUE_ID)
			return i;
	}
	return -1;
}

void LC_ClientGuildManager::RequestRecruitGuildMember()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if ( !pkGamePlayer )
		return ;

	MG_REQ_RecruitGuildMember reqMsg;
	reqMsg.m_uiGuildID = pkGamePlayer->GetGuildID();
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RECRUIT_GUILD_MEMBER, &reqMsg);
	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGuildManager::RequestGetGuildReward()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if ( !pkGamePlayer )
		return ;

	MG_REQ_GuildReward reqMsg;
	reqMsg.m_uiGuildID = pkGamePlayer->GetGuildID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_REWARD, &reqMsg);
	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}