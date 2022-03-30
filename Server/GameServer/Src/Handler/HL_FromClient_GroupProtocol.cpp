#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_GroupProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_PlayerGroupBase.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerWorldManager.h"
#include "UT_ServerHelper.h"
//#include "LC_ServerMapLogic_TranscriptionBase.h"
#include "LC_ServerMapLogic_Place.h"
#include "oss_define.h"
#include "CF_CrossRealmMap.h"
#include "MG_Game2Group.h"
#include "LC_Arena_Manager.h"
#include "LC_ServerGuildManager.h"
#include "gsnode.h"
#include "TS_TranManager.h"
using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;
//-------------------------------------------------------
struct HL_GroupServiceContext
{
	HL_GroupServiceContext()
		: m_pkServiceSourcePlayer(NULL)
		, m_pkServiceTargetPlayer(NULL)
		, m_pkServiceAgentPlayer(NULL)
		, m_pkServiceGroup(NULL)
		, m_pkServiceGroupEntry(NULL)
		, m_lServiceSourceCharID()
		, m_lServiceTargetCharID()
		, m_lServiceAgentCharID()
		, m_lServiceGroupID(0)
		, m_eServiceResult(RE_SUCCESS)
	{};

	LC_ServerPlayer*		m_pkServiceSourcePlayer;	//事务发起人
	LC_ServerPlayer*		m_pkServiceTargetPlayer;	//事务目标
	LC_ServerPlayer*		m_pkServiceAgentPlayer;		//事务中间人

	LC_ServerPlayerGroup*	m_pkServiceGroup;			//事务相关组队
	LC_PlayerGroupEntry*	m_pkServiceGroupEntry;		//事务相关组队数据

	unique_id_impl			m_lServiceSourceCharID;		//事务发起人ID
	unique_id_impl			m_lServiceTargetCharID;		//事务目标ID
	unique_id_impl			m_lServiceAgentCharID;		//事务中间人ID

	object_id_type			m_lServiceGroupID;			//事务相关组队ID

	StringType				m_kServiceSourcePlayerName;	//事务发起人名字
	StringType				m_kServiceTargetPlayerName; //事务目标名字
	StringType				m_kServiceAgentPlayerName;  //事务中间人名字

	ResultType				m_eServiceResult;			//事务结果

	//错误码格式 [组队系统]<事务类型>[事务结果][事务发起人ID|事务目标ID|事务中间人ID|事务组队ID]
	void					PrintSelf(const StringType& rkPrefix)
	{
		if (m_eServiceResult != RE_SUCCESS)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[GROUP SYSTEM]"
						, rkPrefix.c_str()
						, m_eServiceResult
						, m_lServiceSourceCharID
						, m_lServiceTargetCharID
						, m_lServiceAgentCharID
						, m_lServiceGroupID);
		}
	}
};

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CREATE_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CreateGroup reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//检查前置条件
	HL_GroupServiceContext kServiceContext;

	kServiceContext.m_pkServiceSourcePlayer		=  pkPlayer;
	kServiceContext.m_lServiceSourceCharID	=  pkPlayer->GetInstance();

	do
	{

		//是否屏蔽该操作
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_CREATE_GROUP))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
			break;
		}
		// 跨服中不能组队
 		if (UT_ServerHelper::CanCreateTeamInCross(pkPlayer) == false)
		{
			kServiceContext.m_eServiceResult = RE_ON_CROSS_SERVER;
			break;
		}
		//发起人是否已有队伍
		LC_PlayerGroupBase* pGroup = kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup();
		if (pGroup != NULL && pGroup->IsInGroupMemeberList(pkPlayer->GetInstance()) == true)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CREATE_FAIL_EXIST_TEAM;
			break;
		}
		//是否可以创建队伍
		LC_ServerPlayerGroupManager* pkPlayerGroupManager = (LC_ServerPlayerGroupManager*)SERVER_GET_PLAYER_GROUP_MANAGER();
		int nGroupType = E_PlayerTeamType_Local;
		if(UT_ServerHelper::IsInCross(pkPlayer) == true)
		{
			nGroupType = E_PlayerTeamType_CLocal;
		}
		LC_ServerPlayerGroup* group = pkPlayerGroupManager->CreateGroup(pkPlayer, nGroupType);
		if (group == NULL)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CREATE_FAIL_CANNT_CREATE;
			break;
		}
		kServiceContext.m_pkServiceGroup	= group;
		kServiceContext.m_lServiceGroupID	= group->GetID();
		kServiceContext.m_eServiceResult = RE_GROUP_CREATE_SUCCESS;
		oss_role_team(pkPlayer, 1);

	}while (0);

	if (kServiceContext.m_eServiceResult != RE_GROUP_CREATE_SUCCESS)
	{
		MG_RLT_CreateGroup rltMsg;
		rltMsg.m_lGroupID		= kServiceContext.m_lServiceGroupID;
		rltMsg.m_iCreateResult	= kServiceContext.m_eServiceResult;
		kServiceContext.m_pkServiceSourcePlayer->SendMsgToClient(MGPT_RLT_CREATE_GROUP, &rltMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_EXIT_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ExitGroup reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer	=  pkPlayer;
		kServiceContext.m_lServiceSourceCharID	=  pkPlayer->GetInstance();
	}

	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_EXIT))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}
	//队伍是否存在并正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		uint32_t lGroupID	= reqMsg.m_lGroupID;
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (!pkPlayerGroup)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_EXIT_FAIL_NOT_EXIST_TEAM;
		}
		else if (pkPlayerGroup->GetID() != lGroupID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_EXIT_FAIL_NOT_RIGHT_TEAM;
		}
		kServiceContext.m_lServiceGroupID = lGroupID;
		kServiceContext.m_pkServiceGroup = pkPlayerGroup;
	}

	//完成检查，处理服务
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ServerPlayerGroupManager* pkPlayerGroupManager = (LC_ServerPlayerGroupManager*)SERVER_GET_PLAYER_GROUP_MANAGER();
		bool destory = false;
		LC_ServerPlayerGroup* group = pkPlayerGroupManager->GetPlayerGroup(pkPlayer->GetInstance());
		if(group != NULL)
		{
			ResultType ret = RE_SUCCESS;
			if(group->GetGroupType() == E_PlayerTeamType_Cross)
			{
				ret = RE_GROUP_CROSS_NOT_OPT;
			}
			else
			{
				ret = pkPlayerGroupManager->leaveGroup(pkPlayer, group, destory);
			}
			if (ret != RE_SUCCESS)
			{
				MG_RLT_ExitGroup rltMsg;
				rltMsg.m_lExitCharID		= pkPlayer->GetInstance();
				rltMsg.m_lGroupID			= kServiceContext.m_lServiceGroupID;
				TPSTR2STDSTR(kServiceContext.m_pkServiceSourcePlayer->GetOwnerCharName(), rltMsg.m_szExitPlayerName);
				rltMsg.m_iExitResult		= ret;
				pkPlayer->SendMsgToClient(MGPT_RLT_EXIT_GROUP, &rltMsg);
				return;
			}
			oss_role_team(pkPlayer, 4);
		}
	}
	else
	{
		MG_RLT_ExitGroup rltMsg;
		rltMsg.m_iExitResult = kServiceContext.m_eServiceResult;
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_EXIT_GROUP, &rltMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_JOIN_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_JoinGroup reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer	=  pkPlayer;
		kServiceContext.m_lServiceSourceCharID	=  pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName = pkPlayer->GetOwnerCharName();
	}
	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_JOIN))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}

	//申请者队伍是否存在并正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup())
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_EXIST_TEAM;
		}
	}

	//目标是否存在
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkJoinTarget = (LC_ServerPlayer*)(pkPlayerManager->FindPlayer(reqMsg.m_lJoinTargetCharID));
		if (!pkJoinTarget)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_TARGET_OFFLINE;
		}
		if (pkJoinTarget)
		{
 			if (UT_ServerHelper::CanCreateTeamInCross(pkJoinTarget) == false)
			{
				// 跨服中不能组队
				kServiceContext.m_eServiceResult = RE_ON_TARGET_CROSS_SERVER;
			}
			kServiceContext.m_pkServiceTargetPlayer  = pkJoinTarget;
			kServiceContext.m_lServiceTargetCharID = pkJoinTarget->GetInstance();
		}
	}
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
 		if (UT_ServerHelper::CanCreateTeamInCross(pkPlayer) == false)
		{
			// 跨服中不能组队
			kServiceContext.m_eServiceResult = RE_ON_CROSS_SERVER;
		}
	}
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		//if (kServiceContext.m_pkServiceTargetPlayer != NULL &&
		//	UT_ServerHelper::GetActiveId(kServiceContext.m_pkServiceTargetPlayer->GetCitizenship()) != UT_ServerHelper::GetActiveId(pkPlayer->GetCitizenship()))
		int32_t serviceTargetPlayerEntryID = GSNodeNetwork::Instance().getEntryIdByMergeId(kServiceContext.m_pkServiceTargetPlayer->GetCitizenship());
		int32_t playerEntryID = GSNodeNetwork::Instance().getEntryIdByMergeId(pkPlayer->GetCitizenship());
		if (kServiceContext.m_pkServiceTargetPlayer != NULL && serviceTargetPlayerEntryID != playerEntryID)
		{
			// 跨服中不能组队
			kServiceContext.m_eServiceResult = RE_ON_CROSS_SERVER;
		}
	}
	//目标队伍是否存在
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ServerPlayerGroup* pkPlayerGroup  = (LC_ServerPlayerGroup*)kServiceContext.m_pkServiceTargetPlayer->GetPlayerGroup();
		if (!pkPlayerGroup)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_NOT_EXIST_TEAM;
		}
		kServiceContext.m_pkServiceGroup	 = pkPlayerGroup;
		if (pkPlayerGroup)
		{
			kServiceContext.m_lServiceGroupID = pkPlayerGroup->GetID();
		}
	}
	//目标队伍是否已满
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->IsMemberListFull())
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_TEAM_IS_FULL;
		}
	}

	//目标队伍是否已开启屏蔽开关
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->GetForbidJoinFlag())
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_CHEIF_FORBID_JOIN;
		}
	}

	//队长是中间人,是否在线
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_PlayerGroupEntry* pkChiefEntry = kServiceContext.m_pkServiceGroup->GetCheif();
		if(pkChiefEntry != NULL)
		{
			kServiceContext.m_lServiceAgentCharID = pkChiefEntry->GetCharID();
		}

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkCheifPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(kServiceContext.m_lServiceAgentCharID));
		if (!pkCheifPlayer)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_CHEIF_OFFLINE;
		}
		kServiceContext.m_pkServiceAgentPlayer = pkCheifPlayer;
		if (pkCheifPlayer)
		{
			kServiceContext.m_kServiceAgentPlayerName = pkCheifPlayer->GetOwnerCharName();
		}
	}

	//是否被队长屏蔽
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceAgentPlayer->IsInForbidList(kServiceContext.m_lServiceSourceCharID))
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_IN_CHEIF_FORBIDLIST;
		}
	}

	//是否在队长黑名单中
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceAgentPlayer->IsInBlackList(kServiceContext.m_lServiceSourceCharID))
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_IN_CHEIF_BLACKLIST;
		}
	}

	//是否在申请列表中
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->IsInGroupCandidateList(kServiceContext.m_lServiceSourceCharID))
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_IN_CANDIDATE;
		}
	}

	//申请列表是否已满
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->IsCandidateListFull())
		{
			kServiceContext.m_eServiceResult = RE_GROUP_JOIN_FAIL_TOO_MANY;
		}
	}

	//是否和队长在同一服务器
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		//if (kServiceContext.m_pkServiceTargetPlayer != NULL && kServiceContext.m_pkServiceAgentPlayer != NULL && 
		//	UT_ServerHelper::GetActiveId(kServiceContext.m_pkServiceTargetPlayer->GetCitizenship()) != UT_ServerHelper::GetActiveId(kServiceContext.m_pkServiceAgentPlayer->GetCitizenship()))
		int32_t serviceTargetPlayerEntryID = GSNodeNetwork::Instance().getEntryIdByMergeId(kServiceContext.m_pkServiceTargetPlayer->GetCitizenship());
		int32_t serviceAgentPlayerEntryID = GSNodeNetwork::Instance().getEntryIdByMergeId(kServiceContext.m_pkServiceAgentPlayer->GetCitizenship());
		if (kServiceContext.m_pkServiceTargetPlayer != NULL && kServiceContext.m_pkServiceAgentPlayer != NULL &&  serviceTargetPlayerEntryID!=serviceAgentPlayerEntryID)
		{
			// 跨服中不能组队
			kServiceContext.m_eServiceResult = RE_ON_CROSS_SERVER;
		}
	}
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		bool combin = false;
		LC_ServerMapLogic_Place* currentlogicbase = NULL;
		bool ret = kServiceContext.m_pkServiceGroup->CheckGroupRaidExclude(pkPlayer, currentlogicbase, combin);
		if (!ret)
		{
			kServiceContext.m_eServiceResult = RE_RAID_EXCLUDE_CANT_COMBIN;
		}
	}

	//屏蔽跨服组队
	if(kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if(kServiceContext.m_pkServiceGroup != NULL && kServiceContext.m_pkServiceGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CROSS_NOT_OPT;
		}
	}

	//添加到申请列表中
	if(kServiceContext.m_eServiceResult == RE_SUCCESS && kServiceContext.m_pkServiceGroup != NULL)
	{
		kServiceContext.m_eServiceResult = kServiceContext.m_pkServiceGroup->AddPlayerToCandidateList(kServiceContext.m_pkServiceSourcePlayer);
	}

	//完成检查，处理服务
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		MG_REQ_ConfirmGroupJoin reqConfirmMsg;
		reqConfirmMsg.m_lGroupID		= kServiceContext.m_lServiceGroupID;
		reqConfirmMsg.m_lJoinTargetCharID	= kServiceContext.m_lServiceSourceCharID;
		reqConfirmMsg.m_lLevel			= kServiceContext.m_pkServiceSourcePlayer->GetLevel();
		reqConfirmMsg.m_lCharType		= kServiceContext.m_pkServiceSourcePlayer->GetCharType();
		reqConfirmMsg.m_lHeadAvatarID	= kServiceContext.m_pkServiceSourcePlayer->GetCurrentEquipItem(ET_HEAD, true);
		reqConfirmMsg.m_lBodyAvatarID	= kServiceContext.m_pkServiceSourcePlayer->GetCurrentEquipItem(ET_BODY, true);
		reqConfirmMsg.m_lFootAvatarID	= kServiceContext.m_pkServiceSourcePlayer->GetCurrentEquipItem(ET_FOOT, true);
		reqConfirmMsg.m_lWeaponAvatarID = kServiceContext.m_pkServiceSourcePlayer->GetCurrentEquipItem(ET_WEAPON, true);

		TPSTR2STDSTR(kServiceContext.m_kServiceSourcePlayerName, reqConfirmMsg.m_strJoinPlayerName);

		//发送确认请求给队长
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceAgentPlayer, MGPT_REQ_CONFIRM_GROUP_JOIN, &reqConfirmMsg);

		MG_RLT_JoinGroup msgJoinResult;
		msgJoinResult.m_lGroupID = kServiceContext.m_lServiceGroupID;
		msgJoinResult.m_iJoinResult = RE_GROUP_JOIN_REQ_SUCESS;
		msgJoinResult.m_lJoinCharID = kServiceContext.m_lServiceTargetCharID;

		//发送回执给自己
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_JOIN_GROUP, &msgJoinResult);
		
	}
	else
	{
		MG_RLT_JoinGroup msgJoinResult;
		msgJoinResult.m_lGroupID = kServiceContext.m_lServiceGroupID;
		msgJoinResult.m_iJoinResult = kServiceContext.m_eServiceResult;
		msgJoinResult.m_lJoinCharID = kServiceContext.m_lServiceTargetCharID;
		TPSTR2STDSTR(kServiceContext.m_kServiceAgentPlayerName, msgJoinResult.m_strCheifName);
		//发送回执给自己
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_JOIN_GROUP, &msgJoinResult);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_RLT_CONFIRM_GROUP_JOIN, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_RLT_ConfirmGroupJoin rltConfirmMsg;
	if (!rltConfirmMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer	= pkPlayer;
		kServiceContext.m_lServiceSourceCharID	= pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName	= pkPlayer->GetOwnerCharName();
	}

	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_CONFIRM_JOIN))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}

	//服务发起人是否有队伍,并且队伍正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		uint32_t lGroupID = rltConfirmMsg.m_lGroupID;
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (!pkPlayerGroup || pkPlayerGroup->GetID() != lGroupID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}

		kServiceContext.m_pkServiceGroup = pkPlayerGroup;
		kServiceContext.m_lServiceGroupID = lGroupID;
	}

	//服务器发起人是否是队长
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_PlayerGroupEntry* pkChiefEntry =  kServiceContext.m_pkServiceGroup->GetCheif();
		if (NULL==pkChiefEntry || pkChiefEntry->GetCharID() != kServiceContext.m_lServiceSourceCharID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
	}

	//申请人是否存在
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		const unique_id_impl& lJoinCharID = rltConfirmMsg.m_lJoinTargetCharID;
		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkJoinPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lJoinCharID));
		if (!pkJoinPlayer)
		{
			if (rltConfirmMsg.m_iConfirmResult == RE_GROUP_JOIN_CONFIRM_ACCEPT)
			{
				kServiceContext.m_eServiceResult = RE_GROUP_JOIN_CONFIRM_FAIL_TARGET_OFFLINE;
			}
			else
			{
				kServiceContext.m_eServiceResult = RE_FAIL;
			}

		}
		kServiceContext.m_pkServiceTargetPlayer	= pkJoinPlayer;
		kServiceContext.m_lServiceTargetCharID	= lJoinCharID;
		if (pkJoinPlayer)
		{
			kServiceContext.m_kServiceTargetPlayerName = pkJoinPlayer->GetOwnerCharName();
		}
	}

	//申请人是否在申请列表中
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (!kServiceContext.m_pkServiceGroup->IsInGroupCandidateList(kServiceContext.m_lServiceTargetCharID))
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
	}

	ResultType iConfirmResult = rltConfirmMsg.m_iConfirmResult;
	if (iConfirmResult == RE_GROUP_JOIN_CONFIRM_ACCEPT)
	{
		//同意申请的情况下，申请人是否已有队伍
		if (kServiceContext.m_eServiceResult == RE_SUCCESS)
		{
			if (kServiceContext.m_pkServiceTargetPlayer->GetPlayerGroup())
			{
				kServiceContext.m_eServiceResult = RE_GROUP_JOIN_CONFIRM_FAIL_TARGET_IN_TEAM;
			}
		}

		//同意申请的情况下，申请人是否被队伍屏蔽
		if (kServiceContext.m_eServiceResult == RE_SUCCESS)
		{
			if (kServiceContext.m_pkServiceGroup->GetForbidJoinFlag())
			{
				kServiceContext.m_eServiceResult = RE_GROUP_JOIN_CONFIRM_FAIL_CHEIF_FORBID_JOIN;
			}
		}

		//同意申请的情况下，申请人是否被队长屏蔽
		if (kServiceContext.m_eServiceResult == RE_SUCCESS)
		{
			if (kServiceContext.m_pkServiceSourcePlayer->IsInForbidList(kServiceContext.m_lServiceTargetCharID))
			{
				kServiceContext.m_eServiceResult = RE_GROUP_JOIN_CONFIRM_FAIL_IN_CHEIF_FORBIDLIST;
			}
		}

		//同意申请的情况下，申请人是否在队长黑名单中
		if (kServiceContext.m_eServiceResult == RE_SUCCESS)
		{
			if (kServiceContext.m_pkServiceSourcePlayer->IsInBlackList(kServiceContext.m_lServiceTargetCharID))
			{
				kServiceContext.m_eServiceResult = RE_GROUP_JOIN_CONFIRM_FAIL_IN_CHEIF_BLACKLIST;
			}
		}

		//同意申请的情况下，队伍人数是否已满
		if (kServiceContext.m_eServiceResult == RE_SUCCESS)
		{
			if (kServiceContext.m_pkServiceGroup->IsMemberListFull())
			{
				kServiceContext.m_eServiceResult = RE_GROUP_JOIN_CONFIRM_FAIL_TEAM_IS_FULL;
			}
		}
	}

	//屏蔽跨服组队
	if(kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if(kServiceContext.m_pkServiceGroup != NULL && kServiceContext.m_pkServiceGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CROSS_NOT_OPT;
		}
	}

	if(kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		bool bPlayerCross = UT_ServerHelper::IsInCross(kServiceContext.m_pkServiceSourcePlayer);
		if(bPlayerCross != UT_ServerHelper::IsInCross(kServiceContext.m_pkServiceTargetPlayer))
		{
			//被邀请者在跨服中或者邀请者在跨服中，状态不一致
			kServiceContext.m_eServiceResult = RE_GROUP_CROSS_NOT_OPT;
		}
		else
		{
			if(bPlayerCross == true)
			{
				//假如在跨服中，地图ID不一致
				LC_ServerMapLogic_Place* placePlayer = UT_ServerHelper::GetPlayerPlace(kServiceContext.m_pkServiceSourcePlayer);
				LC_ServerMapLogic_Place* placeInvite = UT_ServerHelper::GetPlayerPlace(kServiceContext.m_pkServiceTargetPlayer);
				if( placePlayer != NULL && placeInvite != NULL && placePlayer->GetMapLogicID() != placeInvite->GetMapLogicID())
				{
					kServiceContext.m_eServiceResult = RE_GROUP_LOCAL_CROSS_MAP_ID;
				}
			}
		}
	}

	//检查结束，开始处理
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		//从申请列表中删除申请人
		kServiceContext.m_pkServiceGroup->RemoveCandidateInfo(kServiceContext.m_lServiceTargetCharID);

		MG_RLT_JoinGroup rltMsg;
		rltMsg.m_lGroupID = kServiceContext.m_lServiceGroupID;
		rltMsg.m_lJoinCharID = kServiceContext.m_lServiceTargetCharID;
		TPSTR2STDSTR(kServiceContext.m_kServiceSourcePlayerName, rltMsg.m_strCheifName);
		TPSTR2STDSTR(kServiceContext.m_kServiceTargetPlayerName, rltMsg.m_strJoinPlayerName);

		if (rltConfirmMsg.m_iConfirmResult == RE_GROUP_JOIN_CONFIRM_ACCEPT)
		{
			ResultType ret = kServiceContext.m_pkServiceGroup->AddPlayerToMemberList(kServiceContext.m_pkServiceTargetPlayer);
			if (ret != RE_GROUP_JOIN_SUCCESS)
			{
				rltMsg.m_iJoinResult = ret;
				kServiceContext.m_pkServiceTargetPlayer->SendMsgToClient(MGPT_RLT_JOIN_GROUP, &rltMsg);
				return;
			}
			oss_role_team(pkPlayer, 3);
		}
		else
		{
			rltMsg.m_iJoinResult = RE_GROUP_JOIN_CONFIRM_REJECT;
			kServiceContext.m_pkServiceTargetPlayer->SendMsgToClient(MGPT_RLT_JOIN_GROUP, &rltMsg);
		}
	}
	else
	{
		MG_RLT_JoinGroup rltMsg;
		rltMsg.m_iJoinResult = kServiceContext.m_eServiceResult;
		rltMsg.m_lGroupID	 = kServiceContext.m_lServiceGroupID;
		rltMsg.m_lJoinCharID = kServiceContext.m_lServiceTargetCharID;
		TPSTR2STDSTR(kServiceContext.m_kServiceSourcePlayerName, rltMsg.m_strCheifName);
		TPSTR2STDSTR(kServiceContext.m_kServiceTargetPlayerName, rltMsg.m_strJoinPlayerName);
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_JOIN_GROUP, &rltMsg);

		//特殊处理，如果申请方已不在线，从申请列表中删除
		if (kServiceContext.m_eServiceResult == RE_GROUP_JOIN_CONFIRM_FAIL_TARGET_OFFLINE || kServiceContext.m_eServiceResult == RE_FAIL)
		{
			if (kServiceContext.m_pkServiceGroup)
			{
				kServiceContext.m_pkServiceGroup->RemoveCandidateInfo(kServiceContext.m_lServiceTargetCharID);
			}
		}
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_KICK_PLAYER_FROM_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_KickPlayerFromGroup reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer	=  pkPlayer;
		kServiceContext.m_lServiceSourceCharID   = pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName = pkPlayer->GetOwnerCharName();
	}
	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_KICK_PLAYER))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}
	//服务发起人是否有队伍
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (!pkPlayerGroup)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_KICK_FAIL_NOT_EXIST_TEAM;
		}
		kServiceContext.m_pkServiceGroup = pkPlayerGroup;
	}
	//服务发起人队伍是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		uint32_t lGroupID = reqMsg.m_lGroupID;
		if (kServiceContext.m_pkServiceGroup->GetID() != lGroupID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_KICK_FAIL_NOT_RIGHT_TEAM;
		}
		kServiceContext.m_lServiceGroupID = lGroupID;
	}
	//服务发起人是否是队长
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if ( kServiceContext.m_pkServiceGroup->GetGroupCheifCharID() != kServiceContext.m_lServiceSourceCharID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_KICK_FAIL_NOT_CHEIF;
		}
	}
	//服务目标是否是该队伍成员
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		const unique_id_impl& lKickCharID = reqMsg.m_lKickCharID;
		if (!kServiceContext.m_pkServiceGroup->IsInGroupMemeberList(lKickCharID))
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
		kServiceContext.m_lServiceTargetCharID = lKickCharID;
		if(kServiceContext.m_pkServiceTargetPlayer == NULL)
		{
			kServiceContext.m_pkServiceTargetPlayer = (LC_ServerPlayer*)((SERVER_GET_PLAYER_MANAGER())->FindPlayer(lKickCharID));
		}
	}
	//服务目标是否在线
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_PlayerGroupEntry* pkPlayerEntry = kServiceContext.m_pkServiceGroup->FindGroupEntryInMemberList(reqMsg.m_lKickCharID);
		if (NULL == pkPlayerEntry)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
		else
		{
			kServiceContext.m_pkServiceGroupEntry = pkPlayerEntry;
			kServiceContext.m_kServiceTargetPlayerName = pkPlayerEntry->GetName();
		}
	}
	//检查完毕，开始处理
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		ResultType ret = RE_SUCCESS;
		if(kServiceContext.m_pkServiceGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			ret = RE_GROUP_CROSS_NOT_OPT;
		}
		else
		{
			ret = kServiceContext.m_pkServiceGroup->Kick(reqMsg.m_lKickCharID);
		}
		if (ret != RE_SUCCESS)
		{
			MG_RLT_KickPlayerFromGroup rltMsg;
			rltMsg.m_iKickResult = ret;
			rltMsg.m_lGroupID = kServiceContext.m_lServiceGroupID;
			rltMsg.m_lKickCharID = kServiceContext.m_lServiceTargetCharID;
			TPSTR2STDSTR(kServiceContext.m_kServiceTargetPlayerName, rltMsg.m_szKickPlayerName);
			//通知给当前队伍
			if(kServiceContext.m_pkServiceTargetPlayer != NULL)
			{
				kServiceContext.m_pkServiceTargetPlayer->SendMsgToClient(MGPT_RLT_KICK_PLAYER_FROM_GROUP, &rltMsg);
			}
		}
		oss_role_team(pkPlayer, 4);
	}
	else
	{
		MG_RLT_KickPlayerFromGroup rltMsg;
		rltMsg.m_iKickResult = kServiceContext.m_eServiceResult;
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_KICK_PLAYER_FROM_GROUP, &rltMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_DISMISS_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_DismissGroup reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//检查前置条件
	HL_GroupServiceContext kServiceContext;

	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer	 =  pkPlayer;
		kServiceContext.m_lServiceSourceCharID   = pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName = pkPlayer->GetOwnerCharName();
	}
	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_DISMISS_GROUP))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}
	//服务发起人是否有队伍并且正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		uint32_t lGroupID = reqMsg.m_lGroupID;
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (!pkPlayerGroup || pkPlayerGroup->GetID() != lGroupID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_DISMISS_FAIL_NOT_EXIST_TEAM;
		}
		kServiceContext.m_pkServiceGroup  = pkPlayerGroup;
		kServiceContext.m_lServiceGroupID = lGroupID;
	}
	//服务发起人是否是队长
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->GetGroupCheifCharID() != kServiceContext.m_lServiceSourceCharID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_DISMISS_FAIL_NOT_CHEIF;
		}
	}
	//屏蔽跨服组队
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if(kServiceContext.m_pkServiceGroup != NULL && kServiceContext.m_pkServiceGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CROSS_NOT_OPT;
		}
	}
	
	//检查完毕，开始处理
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
		pkPlayerGroupManager->Dismiss(kServiceContext.m_pkServiceGroup);
		oss_role_team(pkPlayer, 2);
	}
	else
	{
		MG_RLT_DismissGroup rltMsg;
		rltMsg.m_iDisimissResult = kServiceContext.m_eServiceResult;
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_DISMISS_GROUP, &rltMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_INVITE_TO_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_InviteToGroup reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	const unique_id_impl& lBeInviteCharID	= reqMsg.m_lBeInviteCharID;
	LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(pkPlayer->GetPlayerGroup());
	if (pkPlayerGroup)
	{
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_SUCCESS;
		if (pkPlayerGroup->GetGroupCheifCharID() != pkPlayer->GetInstance())
		{
			msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_NOT_CHEIF;
		}
		if (msgInviteToGroup.m_iInviteResult == RE_SUCCESS && pkPlayerGroup->IsMemberListFull())
		{
			msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_TEAM_FULL;
		}
		if(msgInviteToGroup.m_iInviteResult == RE_SUCCESS && pkPlayerGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			msgInviteToGroup.m_iInviteResult = RE_GROUP_CROSS_NOT_OPT;
		}
		if(msgInviteToGroup.m_iInviteResult != RE_SUCCESS)
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
			return;
		}
	}
	if (pkPlayer->IsForbidTarget(lBeInviteCharID))
	{
		// 无法邀请屏蔽列表中的玩家?
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_TARGET_IN_FORBID;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
		return;
	}
	// 被邀请玩家不在线
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lBeInviteCharID));
	if (!pkBeInvitePlayer)
	{
		// 被邀请玩家已经下线
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_TARGET_OFFLINE;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
		return;
	}
 	if (UT_ServerHelper::CanCreateTeamInCross(pkPlayer) == false)
	{
		// 跨服中不能组队
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_ON_CROSS_SERVER;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
		return;
	}
	if (UT_ServerHelper::CanCreateTeamInCross(pkBeInvitePlayer) == false)
	{
		// 跨服中不能组队
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_ON_TARGET_CROSS_SERVER;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
		return;
	}
	if (pkBeInvitePlayer->IsForbidTarget(pkPlayer->GetInstance()))
	{
		// 无法邀请屏蔽列表中的玩家?
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_SELF_IN_FORBID;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
		return;
	}
	if (pkBeInvitePlayer->GetPlayerGroup())
	{
		// 被邀请玩家已经有队伍
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_TARGET_IN_TEAM;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);

		// 提示被邀请玩家
		MG_REQ_ConfirmBeInviteToGroup reqConfirmMsg;
		reqConfirmMsg.m_iResultType	= RE_GROUP_BE_INVITE_FAIL_IN_GROUP;
		reqConfirmMsg.m_lInviteCharID = pkPlayer->GetInstance();
		reqConfirmMsg.m_lGroupID = 0;
		reqConfirmMsg.m_lBeInviteCharID = lBeInviteCharID;
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), reqConfirmMsg.m_strInvitePlayerName);
		pkBeInvitePlayer->SendMsgToClient(MGPT_REQ_CONFIRM_BEINVITE_TO_GROUP, &reqConfirmMsg);
		return;
	}
	bool bPlayerCross = UT_ServerHelper::IsInCross(pkPlayer);
	if(bPlayerCross != UT_ServerHelper::IsInCross(pkBeInvitePlayer))
	{
		//被邀请者在跨服中或者邀请者在跨服中，状态不一致
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_GROUP_CROSS_NOT_OPT;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
		return;
	}

	if(bPlayerCross == true)
	{
		//假如在跨服中，地图ID不一致
		LC_ServerMapLogic_Place* placePlayer = UT_ServerHelper::GetPlayerPlace(pkPlayer);
		LC_ServerMapLogic_Place* placeInvite = UT_ServerHelper::GetPlayerPlace(pkBeInvitePlayer);
		if( placePlayer != NULL && placeInvite != NULL && placePlayer->GetMapLogicID() != placeInvite->GetMapLogicID())
		{
			MG_RLT_InviteToGroup msgInviteToGroup;
			msgInviteToGroup.m_iInviteResult = RE_GROUP_LOCAL_CROSS_MAP_ID;
			pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
			return;
		}
	}

	int32_t invitePlayerEntryID = GSNodeNetwork::Instance().getEntryIdByMergeId(pkPlayer->GetCitizenship());
	int32_t BeInvitePlayerEntryID = GSNodeNetwork::Instance().getEntryIdByMergeId(pkBeInvitePlayer->GetCitizenship());
	if (pkBeInvitePlayer != NULL && invitePlayerEntryID != BeInvitePlayerEntryID)
	{
		//两者服务器ID不一致
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_ON_CROSS_SERVER;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
		return;
	}

	if (pkPlayerGroup)
	{
		{
			// 玩家是否处于忙碌状态
			bool bIsBusy = true;
			int iIndex = 0;
			LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
			for (int i = 0; i < MAX_BE_INVITE_PLAYER_GROUP_COUNT; ++i)
			{
				uint32_t lGroupID = pkBeInvitePlayer->GetBeInviteGroupIDByIndex(i);
				if (lGroupID == pkPlayerGroup->GetID())
				{
					break;
				}
				if (lGroupID == INVALID_GROUP_ID)
				{
					bIsBusy = false;
					iIndex = i;
					break;
				}
				else
				{
					LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->FindPlayerGroupByGroupID(lGroupID);
					if (!pkPlayerGroup)
					{
						bIsBusy = false;
						iIndex = i;
						break;
					}
				}
			}
			if (bIsBusy)
			{
				MG_RLT_InviteToGroup msgInviteToGroup;
				msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_TARGET_IS_BUSY;
				pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
				return;
			}
			else
			{
				pkBeInvitePlayer->SetBeInviteGroupIDByIndex(iIndex, pkPlayerGroup->GetID());
			}
		}
		bool combin = false;
		LC_ServerMapLogic_Place* currentlogicbase = NULL;
		bool ret = pkPlayerGroup->CheckGroupRaidExclude(pkBeInvitePlayer, currentlogicbase, combin);
		if (!ret)
		{
			MG_RLT_InviteToGroup msgInviteToGroup;
			msgInviteToGroup.m_iInviteResult = ret;
			pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
			return;
		}
		const unique_id_impl& lCheifCharID = pkPlayerGroup->GetGroupCheifCharID();
		if (lCheifCharID != pkPlayer->GetInstance())
		{
			//转发消息
			LC_ServerPlayer* pkCheifPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lCheifCharID));
			if (!pkCheifPlayer)
			{
				return;
			}
			if (pkCheifPlayer->IsForbidTarget(lBeInviteCharID))
			{
				MG_RLT_InviteToGroup msgInviteToGroup;
				msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_TARGET_IN_CHEIF_FORBID;
				pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
				return;
			}
			if (pkBeInvitePlayer->IsForbidTarget(lCheifCharID))
			{
				MG_RLT_InviteToGroup msgInviteToGroup;
				msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_FAIL_CHEIF_IN_TARGET_FORBID;
				pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
				return;
			}
			MG_REQ_ConfirmInviteToGroup reqConfirmMsg;
			reqConfirmMsg.m_lInviteCharID = pkPlayer->GetInstance();
			reqConfirmMsg.m_lGroupID = pkPlayerGroup->GetID();
			reqConfirmMsg.m_lBeInviteCharID = lBeInviteCharID;
			TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), reqConfirmMsg.m_strInvitePlayerName);
			TPSTR2STDSTR(pkBeInvitePlayer->GetOwnerCharName(), reqConfirmMsg.m_strBeInvitePlayerName);
			pkCheifPlayer->SendMsgToClient(MGPT_REQ_CONFIRM_INVITE_TO_GROUP, &reqConfirmMsg);
			return;
		}
		else
		{
			//从申请列表中删除申请人
			pkPlayerGroup->RemoveCandidateInfo(lCheifCharID);
			oss_role_team(pkPlayer, 3);
			MG_REQ_ConfirmBeInviteToGroup reqConfirmMsg;
			reqConfirmMsg.m_iResultType	= RE_SUCCESS;
			reqConfirmMsg.m_lInviteCharID = pkPlayer->GetInstance();
			reqConfirmMsg.m_lGroupID = pkPlayerGroup->GetID();
			reqConfirmMsg.m_lBeInviteCharID = lBeInviteCharID;
			TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), reqConfirmMsg.m_strInvitePlayerName);
			if (currentlogicbase == NULL)
			{
				reqConfirmMsg.m_GroupRaidVaild = false;
			}
			else
			{
				reqConfirmMsg.m_GroupRaidVaild = true;
				reqConfirmMsg.m_GroupRaidLogicId = currentlogicbase->GetMapLogicID();
				reqConfirmMsg.m_GroupRaidResId = currentlogicbase->GetMapResID();
				reqConfirmMsg.m_GroupRaidType = currentlogicbase->GetMapLogicType();
				reqConfirmMsg.m_Combin = combin;
			}
			pkBeInvitePlayer->SendMsgToClient(MGPT_REQ_CONFIRM_BEINVITE_TO_GROUP, &reqConfirmMsg);
			MG_SyncPlayerGroupMemberList syncMsg;
			pkPlayerGroup->PackageGroupMemberList(syncMsg);
			pkBeInvitePlayer->SendMsgToClient(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &syncMsg);
			// 通知被邀请人
			MG_RLT_InviteToGroup msgInviteToGroup;
			msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_SUCCESS;
			pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
			return;
		}
	}
	else
	{
		MG_REQ_ConfirmBeInviteToGroup reqConfirmMsg;
		reqConfirmMsg.m_iResultType	= RE_SUCCESS;
		reqConfirmMsg.m_lInviteCharID = pkPlayer->GetInstance();
		reqConfirmMsg.m_lGroupID = INVALID_GROUP_ID;
		reqConfirmMsg.m_lBeInviteCharID = lBeInviteCharID;
		reqConfirmMsg.m_GroupRaidVaild = false;
		pkBeInvitePlayer->SendMsgToClient(MGPT_REQ_CONFIRM_BEINVITE_TO_GROUP, &reqConfirmMsg);

		MG_SyncPlayerGroupMemberList syncMsg;
		syncMsg.m_lGroupID = INVALID_GROUP_ID;
		syncMsg.m_iItemGrade = ITEM_GRADE_NONE;

		syncMsg.m_akMemberEntries.resize(1);
		syncMsg.m_akMemberEntries[0].m_ucIndex				= 0;
		syncMsg.m_akMemberEntries[0].m_bCheif				= false;
		syncMsg.m_akMemberEntries[0].m_lPlayerID			= pkPlayer->GetID();
		syncMsg.m_akMemberEntries[0].m_uiCharID				= pkPlayer->GetInstance();
		syncMsg.m_akMemberEntries[0].m_lPlayerLevel			= (uint16_t)pkPlayer->GetLevel();
		syncMsg.m_akMemberEntries[0].m_iPlayerCareerType	= (uint8_t)pkPlayer->GetCareerType();
		syncMsg.m_akMemberEntries[0].m_iWeaponType			= (uint8_t)pkPlayer->GetWeaponAttribute();
		syncMsg.m_akMemberEntries[0].m_iVehicleLevel		= (uint8_t)pkPlayer->GetVehicleLevel();
		syncMsg.m_akMemberEntries[0].m_bPlayerDead			= pkPlayer->GetDead();
		syncMsg.m_akMemberEntries[0].m_bPlayerOffline		= PLAYER_ON_LINE;
		syncMsg.m_akMemberEntries[0].m_lCharType			= pkPlayer->GetCharType();
		syncMsg.m_akMemberEntries[0].m_lMaxHP				= pkPlayer->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
		syncMsg.m_akMemberEntries[0].m_iBowLevel			= (uint8_t)pkPlayer->GetBowLevel();
		syncMsg.m_akMemberEntries[0].m_lHP					= pkPlayer->GetHP();
		syncMsg.m_akMemberEntries[0].m_iDevaEyeLevel		= (uint8_t)pkPlayer->GetDevaEyeLevel();
		syncMsg.m_akMemberEntries[0].m_lHeadAvatarID		= pkPlayer->GetCurrentEquipItem(ET_HEAD, true);
		syncMsg.m_akMemberEntries[0].m_lBodyAvatarID		= pkPlayer->GetCurrentEquipItem(ET_BODY, true);
		syncMsg.m_akMemberEntries[0].m_lFootAvatarID		= pkPlayer->GetCurrentEquipItem(ET_FOOT, true);
		syncMsg.m_akMemberEntries[0].m_lWeaponAvatarID		= pkPlayer->GetCurrentEquipItem(ET_WEAPON, true);
		syncMsg.m_akMemberEntries[0].m_lPosX				= pkPlayer->GetCurrentLocation().x;
		syncMsg.m_akMemberEntries[0].m_lPosY				= pkPlayer->GetCurrentLocation().y;
		syncMsg.m_akMemberEntries[0].m_lMapID				= pkPlayer->GetMapLogicID();
		syncMsg.m_akMemberEntries[0].m_lMapResID			= pkPlayer->GetMapResID();
		syncMsg.m_akMemberEntries[0].m_lLogicAreaID			= pkPlayer->GetLogicAreaId();
		syncMsg.m_akMemberEntries[0].m_nTransformID = pkPlayer->GetCurrentActivateBuddyTransformID();
		syncMsg.m_akMemberEntries[0].m_iScore = pkPlayer->GetActivateBuddyCombatScore();
		
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), syncMsg.m_akMemberEntries[0].m_strPlayerName);
		pkBeInvitePlayer->SendMsgToClient(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &syncMsg);

		// 通知被邀请人
		MG_RLT_InviteToGroup msgInviteToGroup;
		msgInviteToGroup.m_iInviteResult = RE_GROUP_INVITE_SUCCESS;
		pkPlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &msgInviteToGroup);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_RLT_CONFIRM_INVITE_TO_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_ConfirmInviteToGroup rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	const unique_id_impl& lInviteCharID		= rltMsg.m_lInviteCharID;
	uint32_t lGroupID					= rltMsg.m_lGroupID;
	const unique_id_impl&  lBeInviteCharID	= rltMsg.m_lBeInviteCharID;
	ResultType iConfirmResult			= rltMsg.m_iConfirmResult;

	LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(pkPlayer->GetPlayerGroup());
	if (!pkPlayerGroup || pkPlayerGroup->GetID() != lGroupID)
	{
		return;
	}

	object_id_type lPlayerCheifID = pkPlayerGroup->GetGroupCheifID();
	if (lPlayerCheifID != pkPlayer->GetID())
	{
		return;
	}

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lInviteCharID));
	LC_ServerPlayer* pkInvitePlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lBeInviteCharID));

	StringType kCheifPlayerName		= "";
	StringType kInvitePlayerName	= "";
	StringType kBeInvitePlayerName	= "";

	kCheifPlayerName	= pkPlayer->GetOwnerCharName();
	if (pkInvitePlayer)
	{
		kInvitePlayerName	= pkInvitePlayer->GetOwnerCharName();
	}
	if (pkBeInvitePlayer)
	{
		kBeInvitePlayerName	= pkBeInvitePlayer->GetOwnerCharName();
	}
	// 被邀请者已经离线，就通知邀请者和队长
	if (NULL == pkBeInvitePlayer)
	{
		MG_RLT_ConfirmInviteToGroup rltCIMsg;
		rltCIMsg.m_lInviteCharID = lInviteCharID;
		rltCIMsg.m_lGroupID	= rltMsg.m_lGroupID;
		rltCIMsg.m_lBeInviteCharID = lBeInviteCharID;
		rltCIMsg.m_iConfirmResult = RE_GROUP_INVITE_FAIL_TARGET_NOT_EXIST;

		if (NULL != pkInvitePlayer)
		{
			pkInvitePlayer->SendMsgToClient(MGPT_RLT_CONFIRM_INVITE_TO_GROUP, &rltCIMsg);
		}
		object_id_type lCheifID = pkPlayerGroup->GetGroupCheifID();
		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkCheifPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lCheifID));
		if (NULL != pkCheifPlayer)
		{
			pkCheifPlayer->SendMsgToClient(MGPT_RLT_CONFIRM_INVITE_TO_GROUP, &rltCIMsg);
		}
		return;
	}
	if (pkInvitePlayer)
	{
		MG_RLT_ConfirmInviteToGroup rltMsg2;
		rltMsg2.m_lInviteCharID	= lInviteCharID;
		rltMsg2.m_lGroupID = rltMsg.m_lGroupID;
		rltMsg2.m_lBeInviteCharID = lBeInviteCharID;
		rltMsg2.m_iConfirmResult = iConfirmResult;
		TPSTR2STDSTR(kInvitePlayerName, rltMsg2.m_strInvitePlayerName);
		TPSTR2STDSTR(kBeInvitePlayerName, rltMsg2.m_strBeInvitePlayerName);
		TPSTR2STDSTR(kCheifPlayerName, rltMsg2.m_strChiefPlayerName);
		pkInvitePlayer->SendMsgToClient(MGPT_RLT_CONFIRM_INVITE_TO_GROUP, &rltMsg2);
	}
	bool combin = false;
	LC_ServerMapLogic_Place* currentlogicbase = NULL;
	bool ret = pkPlayerGroup->CheckGroupRaidExclude(pkPlayer, currentlogicbase, combin);
	if (!ret)
	{
		return;
	}
	pkPlayer->RemoveBeInviteGroupByID(lGroupID);
	if (iConfirmResult == RE_GROUP_INVITE_CHEIF_ACCEPT_JOIN)
	{
		MG_REQ_ConfirmBeInviteToGroup reqConfirmMsg;
		reqConfirmMsg.m_iResultType = RE_SUCCESS;
		reqConfirmMsg.m_lInviteCharID = lInviteCharID;
		reqConfirmMsg.m_lGroupID = lGroupID;
		reqConfirmMsg.m_lBeInviteCharID = lBeInviteCharID;
		TPSTR2STDSTR(kInvitePlayerName, reqConfirmMsg.m_strInvitePlayerName);
		if (currentlogicbase == NULL)
		{
			reqConfirmMsg.m_GroupRaidVaild = false;
		}
		else
		{
			reqConfirmMsg.m_GroupRaidVaild = true;
			reqConfirmMsg.m_GroupRaidLogicId = currentlogicbase->GetMapLogicID();
			reqConfirmMsg.m_GroupRaidResId = currentlogicbase->GetMapResID();
			reqConfirmMsg.m_GroupRaidType = currentlogicbase->GetMapLogicType();
			reqConfirmMsg.m_Combin = combin;
		}

		pkBeInvitePlayer->SendMsgToClient(MGPT_REQ_CONFIRM_BEINVITE_TO_GROUP, &reqConfirmMsg);

		MG_SyncPlayerGroupMemberList syncMsg;
		pkPlayerGroup->PackageGroupMemberList(syncMsg);
		pkBeInvitePlayer->SendMsgToClient(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &syncMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_RLT_CONFIRM_BEINVITE_TO_GROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_ConfirmBeInviteToGroup reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	const unique_id_impl& lInviteCharID	= reqMsg.m_lInviteCharID;
	int lGroupID = reqMsg.m_lGroupID;
	const unique_id_impl& lBeInviteCharID = reqMsg.m_lBeInviteCharID;
	ResultType iConfirmResult = reqMsg.m_iConfirmResult;
	if (pkPlayer->GetPlayerGroup())
	{
		return;
	}
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(pkPlayerGroupManager->FindPlayerGroupByGroupID(lGroupID));
	//通知给邀请者
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkInvitePlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lInviteCharID));
	if (pkInvitePlayer)
	{
		MG_RLT_InviteToGroup rltMsg;
		rltMsg.m_lInviteCharID	= lInviteCharID;
		rltMsg.m_lGroupID			= lGroupID;
		rltMsg.m_lBeInviteCharID	= lBeInviteCharID;
		rltMsg.m_iInviteResult		= iConfirmResult;
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), rltMsg.m_strBeInvitePlayerName);
		pkInvitePlayer->SendMsgToClient(MGPT_RLT_INVITE_TO_GROUP, &rltMsg);
	}
	else
	{
		MG_RLT_JoinGroup rltMsg;
		rltMsg.m_lGroupID = lGroupID;
		rltMsg.m_iJoinResult = RE_GROUP_JOIN_FAIL_TARGET_OFFLINE;
		rltMsg.m_lJoinCharID = reqMsg.m_lBeInviteCharID;
		pkPlayer->SendMsgToClient(MGPT_RLT_JOIN_GROUP, &rltMsg);
		return;
	}
	pkPlayer->RemoveBeInviteGroupByID(lGroupID);
	if (iConfirmResult == RE_GROUP_INVITE_TARGET_ACCEPT_JOIN)
	{
		if (!pkPlayerGroup)
		{
			//判断邀请者是否存在队伍，如果不存在就创建
			LC_ServerPlayerGroup* pkInvitePlayerGroup = (LC_ServerPlayerGroup*)pkInvitePlayer->GetPlayerGroup();
			if (pkInvitePlayerGroup != NULL)
			{
				pkPlayerGroup = pkInvitePlayerGroup;
			}
			else
			{
				LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
				int nGroupType = E_PlayerTeamType_Local;
				if(UT_ServerHelper::IsInCross(pkInvitePlayer) == true)
				{
					nGroupType = E_PlayerTeamType_CLocal;
				}
				pkPlayerGroup = pkPlayerGroupManager->CreateGroup(pkInvitePlayer, nGroupType);
			}
			if (pkPlayerGroup == NULL)
			{
				return;
			}
		}
		MG_RLT_JoinGroup rltMsg;
		rltMsg.m_lGroupID = pkPlayerGroup->GetID();
		rltMsg.m_lJoinCharID = pkPlayer->GetInstance();
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), rltMsg.m_strJoinPlayerName);
		ResultType ret = pkPlayerGroup->AddPlayerToMemberList(pkPlayer);
		if (ret != RE_SUCCESS)
		{
			rltMsg.m_iJoinResult = ret;
			pkPlayer->SendMsgToClient(MGPT_RLT_JOIN_GROUP, &rltMsg);
			pkInvitePlayer->SendMsgToClient(MGPT_RLT_JOIN_GROUP, &rltMsg);
			return;
		}
		MG_SyncPlayerGroupMemberList syncMsg;
		pkPlayerGroup->PackageGroupMemberList(syncMsg);
		pkPlayer->SendMsgToClient(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &syncMsg);
		oss_role_team(pkPlayer, 3);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_APPOINTMENT_CHEIF, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AppointmentCheif reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer		   =  pkPlayer;
		kServiceContext.m_lServiceSourceCharID   = pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName = pkPlayer->GetOwnerCharName();
	}
	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_APPOINT_CHEIF))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}
	//服务发起人是否有队伍，并且队伍正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		uint32_t lGroupID	= reqMsg.m_lGroupID;
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (!pkPlayerGroup || pkPlayerGroup->GetID() != lGroupID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_APPOINTMENT_CHEIF_FAIL_NOT_EXIST_TEAM;
		}
		kServiceContext.m_pkServiceGroup  = pkPlayerGroup;
		kServiceContext.m_lServiceGroupID = lGroupID;
	}
	//服务发起人是否是队长
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->GetGroupCheifCharID() != kServiceContext.m_lServiceSourceCharID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_APPOINTMENT_CHEIF_FAIL_NOT_CHEIF;
		}
	}
	//服务目标是否在队伍中
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (!kServiceContext.m_pkServiceGroup->IsInGroupMemeberList(reqMsg.m_lBeAppointmentCharID))
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
		kServiceContext.m_lServiceTargetCharID = reqMsg.m_lBeAppointmentCharID;
	}
	//屏蔽跨服组队
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if(kServiceContext.m_pkServiceGroup != NULL && kServiceContext.m_pkServiceGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CROSS_NOT_OPT;
		}
	}
	
	//服务目标是否存在
	//检查完毕，开始处理
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		//设置队长
		kServiceContext.m_pkServiceGroup->SetGroupCheifID(kServiceContext.m_lServiceTargetCharID);
		oss_role_team(pkPlayer, 5);
		MG_RLT_AppointmentCheif rltMsg;
		rltMsg.m_iAppointmentResult	= RE_GROUP_APPOINTMENT_CHEIF_SUCCESS;
		rltMsg.m_lRequestCharID	= kServiceContext.m_lServiceSourceCharID;
		rltMsg.m_lGroupID = kServiceContext.m_lServiceGroupID;
		rltMsg.m_lBeAppointmentCharID = kServiceContext.m_lServiceTargetCharID;
		//通知给当前队伍
		kServiceContext.m_pkServiceGroup->BroadCast(MGPT_RLT_APPOINTMENT_CHEIF, &rltMsg);
		MG_SyncPlayerGroupMemberList syncMsg;
		kServiceContext.m_pkServiceGroup->PackageGroupMemberList(syncMsg);
		kServiceContext.m_pkServiceGroup->BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &syncMsg);
		// 同步申请信息给队长
		kServiceContext.m_pkServiceGroup->CheifSyncGroupCandidateListToClient(kServiceContext.m_lServiceTargetCharID);
	}
	else
	{
		MG_RLT_AppointmentCheif rltMsg;
		rltMsg.m_iAppointmentResult	= kServiceContext.m_eServiceResult;
		rltMsg.m_lRequestCharID	= kServiceContext.m_lServiceSourceCharID;
		rltMsg.m_lGroupID = kServiceContext.m_lServiceGroupID;
		rltMsg.m_lBeAppointmentCharID = kServiceContext.m_lServiceTargetCharID;
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_APPOINTMENT_CHEIF, &rltMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_MODIFY_QUALITY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ModifyQuality reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer	= pkPlayer;
		kServiceContext.m_lServiceSourceCharID = pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName = pkPlayer->GetOwnerCharName();
	}
	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_MODIFY_QUALITY))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}
	//服务发起人是否有队伍，并且队伍正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		uint32_t lGroupID = reqMsg.m_lGroupID;
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (!pkPlayerGroup || pkPlayerGroup->GetID() != lGroupID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
		kServiceContext.m_pkServiceGroup  = pkPlayerGroup;
		kServiceContext.m_lServiceGroupID = lGroupID;
	}
	//服务发起人是否是队长
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->GetGroupCheifCharID() != kServiceContext.m_lServiceSourceCharID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_MODIFY_QUALITY_NOT_CHEIF;
		}
	}
	//屏蔽跨服组队
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if(kServiceContext.m_pkServiceGroup != NULL && kServiceContext.m_pkServiceGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CROSS_NOT_OPT;
		}
	}
	
	int iItemGrade = reqMsg.m_iItemGrade;
	//检查完毕，开始处理
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->SetItemGrade(iItemGrade))
		{
			MG_RLT_ModityQuality rltMsg;
			rltMsg.m_iResult			= RE_GROUP_MODIFY_QUALITY_SUCCESS;
			rltMsg.m_lRequestCharID	= kServiceContext.m_lServiceSourceCharID;
			rltMsg.m_lGroupID			= kServiceContext.m_lServiceGroupID;
			rltMsg.m_iItemGrade			= iItemGrade;
			//通知给当前队伍
			kServiceContext.m_pkServiceGroup->BroadCast(MGPT_RLT_MODIFY_QUALITY, &rltMsg);
		}
	}
	else
	{
		MG_RLT_ModityQuality rltMsg;
		rltMsg.m_iResult = kServiceContext.m_eServiceResult;
		rltMsg.m_lRequestCharID	= kServiceContext.m_lServiceSourceCharID;
		rltMsg.m_lGroupID			= kServiceContext.m_lServiceGroupID;
		rltMsg.m_iItemGrade			= iItemGrade;
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_MODIFY_QUALITY, &rltMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CHEIF_FORBID_JOIN, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ForbidJion reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer		= pkPlayer;
		kServiceContext.m_lServiceSourceCharID   = pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName = pkPlayer->GetOwnerCharName();
	}
	//是否屏蔽该操作
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ActionForbid* pkActionForbid = kServiceContext.m_pkServiceSourcePlayer->GetActionForbid();
		if (pkActionForbid->IsActionForbid(LAFT_GROUP, LAF_GROUP_FORBID_JOIN))
		{
			kServiceContext.m_eServiceResult = RE_ACTION_FORBID_GROUP;
		}
	}
	//服务发起人是否有队伍，并且队伍正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		uint32_t lGroupID	= reqMsg.m_lGroupID;
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (!pkPlayerGroup || pkPlayerGroup->GetID() != lGroupID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
		kServiceContext.m_pkServiceGroup  = pkPlayerGroup;
		kServiceContext.m_lServiceGroupID = lGroupID;
	}
	//服务发起人是否是队长
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if (kServiceContext.m_pkServiceGroup->GetGroupCheifCharID() != kServiceContext.m_lServiceSourceCharID)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_FROBID_JOIN_FAILED_NOT_CHEIF;
		}
	}
	//屏蔽跨服组队
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		if(kServiceContext.m_pkServiceGroup != NULL && kServiceContext.m_pkServiceGroup->GetGroupType() == E_PlayerTeamType_Cross)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_CROSS_NOT_OPT;
		}
	}
	
	bool m_bForbidJoinFlag	= reqMsg.m_bForbidJoinFlag;
	//检查完毕，开始处理
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceGroup->SetForbidJoinFlag(m_bForbidJoinFlag);
		MG_RLT_ForbidJion rltMsg;
		rltMsg.m_iResult				= RE_GROUP_FROBID_JOIN_SUCCESS;
		rltMsg.m_lRequestCharID			= kServiceContext.m_lServiceSourceCharID;
		rltMsg.m_lGroupID				= kServiceContext.m_lServiceGroupID;
		rltMsg.m_bForbidJoinFlag		= m_bForbidJoinFlag;
		kServiceContext.m_pkServiceGroup->BroadCast(MGPT_RLT_CHEIF_FORBID_JION, &rltMsg);
	}
	else
	{
		MG_RLT_ForbidJion rltMsg;
		rltMsg.m_iResult				= kServiceContext.m_eServiceResult;
		rltMsg.m_lRequestCharID			= kServiceContext.m_lServiceSourceCharID;
		rltMsg.m_lGroupID				= kServiceContext.m_lServiceGroupID;
		rltMsg.m_bForbidJoinFlag		= m_bForbidJoinFlag;
		SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_RLT_CHEIF_FORBID_JION, &rltMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SYNC_INVITE_GROUP_MEMBER_LIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SyncInviteGroupMemberList reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//检查前置条件
	HL_GroupServiceContext kServiceContext;
	//服务发起人是否正确
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		kServiceContext.m_pkServiceSourcePlayer = pkPlayer;
		kServiceContext.m_lServiceSourceCharID = pkPlayer->GetInstance();
		kServiceContext.m_kServiceSourcePlayerName = pkPlayer->GetOwnerCharName();
	}
	//服务发起人是否已有队伍
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(kServiceContext.m_pkServiceSourcePlayer->GetPlayerGroup());
		if (pkPlayerGroup)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
	}
	//服务发起人是否被请求的队伍邀请过
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		bool bFind = false;
		int lGroupID = reqMsg.m_uiGroupID;
		for (int i = 0; i < MAX_BE_INVITE_PLAYER_GROUP_COUNT; ++i)
		{
			if (lGroupID == kServiceContext.m_pkServiceSourcePlayer->GetBeInviteGroupIDByIndex(i))
			{
				bFind = true;
				break;
			}
		}
		if (!bFind)
		{
			kServiceContext.m_eServiceResult = RE_GROUP_SYSTEM_ERROR;
		}
		kServiceContext.m_lServiceGroupID = lGroupID;
	}
	//检查完毕，开始处理
	if (kServiceContext.m_eServiceResult == RE_SUCCESS)
	{
		LC_PlayerGroupManagerBase* pkPlayerGroupManager = LC_PlayerGroupManagerBase::GetSingletonPtr();
		LC_ServerPlayerGroup* pkPlayerGroup = (LC_ServerPlayerGroup*)(pkPlayerGroupManager->FindPlayerGroupByGroupID(kServiceContext.m_lServiceGroupID));
		if (pkPlayerGroup)
		{
			MG_SyncPlayerGroupMemberList syncMsg;
			pkPlayerGroup->PackageGroupMemberList(syncMsg);
			SAFE_SEND_TO_CLIENT(kServiceContext.m_pkServiceSourcePlayer, MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &syncMsg)
		}
	}
}

ON_HANDLE(MGPT_CROSSREALM_MAPLIST_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_MAPLIST_REQ, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_TEAMLIST_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CrossRealm_TeamList reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_TEAMLIST_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_TEAMDETAIL_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CrossRealm_TeamDetail reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_TEAMDETAIL_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

void CrossRealmFillPlayInfo(LC_ServerPlayer* pkPlayer, PS_TeamPlayerAbstract& info)
{
	if (pkPlayer == NULL)
	{
		return;
	}
	info.m_CharID = pkPlayer->GetInstance();
	info.m_UserId = pkPlayer->GetUserID();
	TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), info.m_PlayerName);
	info.m_Level = pkPlayer->GetLevel();
	//info.m_ServerId = UT_ServerHelper::GetActiveId(pkPlayer);
	info.m_ServerId = GSNodeNetwork::Instance().getEntryIdByMergeId(pkPlayer->GetCitizenship());
	//info.m_TotalScore = pkPlayer->GetTotalScore();
	info.m_TotalScore = pkPlayer->GetControllerCombatScore();
	info.m_CharType = pkPlayer->GetCharType();
	info.m_HeadEquip = pkPlayer->GetCurrentEquipItem(ET_HEAD);
	info.m_BodyEquip = pkPlayer->GetCurrentEquipItem(ET_BODY);
	info.m_WeaponEquip = pkPlayer->GetWeaponAttribute();
}

ON_HANDLE(MGPT_CROSSREALM_ADD2TEAM_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CrossRealm_Add2Team reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	CF_CrossRealmMap* thecrossmap = CF_CrossRealmMap::GetSingletonPtr();
	CF_CrossRealmMap::DataEntry* pkEntry = thecrossmap->GetEntryPtr(reqMsg.m_MapTypeId);
	if (pkEntry == NULL)
	{
		return;
	}

	
	if (pkEntry->_Limit > 0 && pkPlayer->GetCrossRealmTeamAssert()->GetDailyMapTime(reqMsg.m_MapTypeId) >= pkEntry->_Limit)
	{
		MG_RESP_CrossRealm_Add2Team resp;
		resp.m_MapTypeId = reqMsg.m_MapTypeId;
		resp.m_TeamId = reqMsg.m_TeamId;
		resp.m_Result = RE_CROSSREALMMAP_TIME_OUT;
		pkPlayer->SendMsgToClient(MGPT_CROSSREALM_ADD2TEAM_RESP, &resp);
		return;
	}

	if(pkPlayer->GetLevel() < pkEntry->_kLevel)
	{
		MG_RESP_CrossRealm_Add2Team respMsg;
		respMsg.m_MapTypeId = reqMsg.m_MapTypeId;
		respMsg.m_TeamId = reqMsg.m_TeamId;
		respMsg.m_Result = RE_TRANS_LEVEL_LOW;
		pkPlayer->SendMsgToClient(MGPT_CROSSREALM_ADD2TEAM_RESP, &respMsg);
		return;
	}

	MG_GG_REQ_Add2Team msg;
	msg.m_MapTypeId = reqMsg.m_MapTypeId;
	msg.m_TeamId = reqMsg.m_TeamId;
	msg.m_Size = 1;
	CrossRealmFillPlayInfo(pkPlayer, msg.m_PlayerList[0]);
	SendMsgToSociety(MGPT_SS_CROSSREALM_ADD2TEAM_REQ, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_CREATETEAM_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}


	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (pkArenaMgr->IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter cross releam failed! in arena sign!");
		return;
	}

	MG_REQ_CrossRealm_CreateTeam reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	CF_CrossRealmMap* thecrossmap = CF_CrossRealmMap::GetSingletonPtr();
	CF_CrossRealmMap::DataEntry* pkEntry = thecrossmap->GetEntryPtr(reqMsg.m_MapTypeId);
	if (pkEntry == NULL)
	{
		return;
	}
	if (pkEntry->_Limit > 0 && pkPlayer->GetCrossRealmTeamAssert()->GetDailyMapTime(reqMsg.m_MapTypeId) >= pkEntry->_Limit)
	{
		MG_RLT_CrossRealm_CreateTeam resp;
		resp.m_MapTypeId = reqMsg.m_MapTypeId;
		resp.m_Result = RE_CROSSREALMMAP_TIME_OUT;
		pkPlayer->SendMsgToClient(MGPT_CROSSREALM_CREATETEAM_RESP, &resp);
		return;
	}
	MG_GG_REQ_CreateTeam msg;
	msg.m_MapTypeId = reqMsg.m_MapTypeId;
	msg.m_TotalScore = reqMsg.m_TotalScore;
	msg.m_AutoStart = reqMsg.m_AutoStart;
	CrossRealmFillPlayInfo(pkPlayer, msg.m_PlayerInfo);
	SendMsgToSociety(MGPT_SS_CROSSREALM_CREATETEAM_REQ, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_LEAVETEAM_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GG_REQ_LeaveTeam msg;
	SendMsgToSociety(MGPT_SS_CROSSREALM_LEAVETEAM_REQ, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_TEAMINFOEDIT_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CrossRealm_TeamInfoEdit reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_TEAMINFOEDIT_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_KICKMEMBER_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CrossRealm_Kick_Member reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_KICKMEMBER_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_READYMEMBER_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (pkArenaMgr->IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter cross releam failed! in arena sign!");
		return;
	}

	MG_REQ_CrossRealm_Ready_Member reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_READYMEMBER_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_INVITEPLAYER_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer) return;

	MG_REQ_InvitePlayer_CrossRealmTeam reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize)) return;

	CF_CrossRealmMap::DataEntry* pkEntry = CF_CrossRealmMap::GetSingletonPtr()->GetEntryPtr(reqMsg.m_iMapTypeID);
	if (pkEntry == NULL) return;

	MG_RESP_InvitePlayer_CrossRealmTeam respMsg;
	respMsg.m_iBeInviteCharID = reqMsg.m_iBeInviteCharID;
	respMsg.m_iplayerName = TPSTR2STDSTR(pkPlayer->GetOwnerCharName());
	respMsg.m_Result = RE_SUCCESS;

	if (pkPlayer->IsForbidTarget(reqMsg.m_iBeInviteCharID))
	{
		// 无法邀请屏蔽列表中的玩家?
		respMsg.m_Result = RE_GROUP_INVITE_FAIL_TARGET_IN_FORBID;
		pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
		return;
	}

	uint64_t beInviteCharID= reqMsg.m_iBeInviteCharID;
	InviteType type = (InviteType)reqMsg.m_inviteType;

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(beInviteCharID));
	if(pkBeInvitePlayer == NULL)
	{
		// 被邀请玩家不在线
		respMsg.m_Result = RE_INVITE_FAILED_TARGET_IS_OFFLINE;
		pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
		return;
	}

	if(type == INTITE_FRIENDS)
	{
		if (!pkPlayer->IsFriend(beInviteCharID))	
		{
			//被邀请的人不是好友
			respMsg.m_Result = RE_INVITE_FAILED_TARGET_NOT_FRIENT;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}

		//cd时间判断
		uint64_t beInviteTime= pkPlayer->GetSocietyAsset()->GetInviteTime(beInviteCharID);
		int64_t currentTime =  GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if(beInviteTime != 0 && beInviteTime+MIN_INVITE_TIME > currentTime)
		{
			respMsg.m_Result = RE_INVITE_TIME_TOO_SHORT;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}

		//进入等级判断
		if(pkBeInvitePlayer->GetLevel()  < pkEntry->_kLevel)
		{
			respMsg.m_Result = RE_INVITE_FAILED_TARGET_lEVEL_LOW;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}

		//战力判断
		if(pkBeInvitePlayer->GetControllerCombatScore() < reqMsg.m_iScore)
		{
			respMsg.m_Result = RE_INVITE_FAILED_TARGET_SCORE_LOW;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}

		pkPlayer->GetSocietyAsset()->SetInviteTime(beInviteCharID, currentTime);

	}
	else if(type == INVITE_GUILDFRIENDS)
	{
		//公会好友
		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		if(NULL == pkGuildManager) return;

		//不能邀请自己
		if(pkPlayer->GetInstance() == beInviteCharID) return;

		//邀请者所在的公会信息
		LC_ServerGuildInfo* guild = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
		if(NULL == guild) return;

		//被邀请的玩家的信息
		GuildServerMember* beInvitePlayerInfo = guild->GetMember(beInviteCharID);
		if(NULL == beInvitePlayerInfo) 
		{
			//被邀请的人不是公会好友
			respMsg.m_Result = RE_INVITE_FAILED_TARGET_NOT_FRIENT;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}

		//cd时间
		uint64_t beInviteTime= pkPlayer->GetInviteTime(beInviteCharID);
		int64_t currentTime =  GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if(beInviteTime != 0 && beInviteTime+MIN_INVITE_TIME > currentTime)
		{
			respMsg.m_Result = RE_INVITE_TIME_TOO_SHORT;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}
		
		//等级
		if(pkBeInvitePlayer->GetLevel()  < pkEntry->_kLevel)
		{
			// 被邀请玩家等级不满足进入副本等级
			respMsg.m_Result = RE_INVITE_FAILED_TARGET_lEVEL_LOW;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}

		//战力判断
		if(pkBeInvitePlayer->GetControllerCombatScore() < reqMsg.m_iScore)
		{
			respMsg.m_Result = RE_INVITE_FAILED_TARGET_SCORE_LOW;
			pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
			return;
		}

		pkPlayer->SetInviteTime(beInviteCharID, currentTime);
	}	

	MG_Notify_Invite_CrossRealmTeam NotifyMsg;
	NotifyMsg.m_iInviteCharID = pkPlayer->GetInstance();
	NotifyMsg.m_playerName = TPSTR2STDSTR(pkPlayer->GetOwnerCharName());
	NotifyMsg.m_iMapTypeID = reqMsg.m_iMapTypeID;
	NotifyMsg.m_iTeamID = reqMsg.m_iTeamID;
	pkBeInvitePlayer->SendMsgToClient(MGPT_NOTIFY_CROSSREALM_INVITE, &NotifyMsg);

	pkPlayer->SendMsgToClient(MGPT_CROSSREALM_INVITEPLAYER_RESP, &respMsg);
	return;
}

ON_HANDLE(MGPT_CROSSREALM_REFUSE_INVITE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer) return;

	MG_CrossRealm_Refush_Invate reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize)) return;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL)  return;

	LC_ServerPlayer* pkInvitePlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(reqMsg.m_iInviteCharID);
	if (pkInvitePlayer == NULL)  return;

	MG_Notify_CrossRealm_Refush_Invate respMsg;
	respMsg.m_iBeInviteCharID = pkPlayer->GetUserID();	
	respMsg.m_iplayerName = TPSTR2STDSTR(pkPlayer->GetOwnerCharName());

	pkInvitePlayer->SendMsgToClient(MGPT_NOTIFY_CROSSREALM_REFUSE_INVITE, &respMsg);
}

ON_HANDLE(MGPT_INVITE_ALLPLAYER_CROSSREALMTEAM_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer) return;

	MG_REQ_InviteAllPlayer_CrossRealmTeam reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize)) return;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager == NULL) return;

	CF_CrossRealmMap::DataEntry* pkEntry = CF_CrossRealmMap::GetSingletonPtr()->GetEntryPtr(reqMsg.m_iMapTypeID);
	if (pkEntry == NULL) return;

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	int count = 0;
	if(reqMsg.m_inviteType == INTITE_FRIENDS)
	{
		//普通好友
		LC_SocietyAsset* pkSocietyAsset = pkPlayer->GetSocietyAsset();

		LC_FriendInfo* pkFriendInfo = pkSocietyAsset->GetFriendInfo();

		LC_SocietyBasicInfoMap* AllFriendInfo = pkFriendInfo->GetAllSocietyBasicInfo();
		if(AllFriendInfo == NULL) return;

		for(LC_SocietyBasicInfoMap::iterator it = AllFriendInfo->begin(); it!=AllFriendInfo->end(); ++it)
		{
			uint64_t beInviteCharID = it->first;
			LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(beInviteCharID));
			if(pkBeInvitePlayer != NULL && pkBeInvitePlayer->GetLevel() >= pkEntry->_kLevel)
			{
				//cd时间判断
				uint64_t beInviteTime= pkPlayer->GetSocietyAsset()->GetInviteTime(beInviteCharID);
				int64_t currentTime =  GET_CURRENT_TIMESTAMP_IN_SECONDS();
				if(beInviteTime != 0 && beInviteTime+MIN_INVITE_TIME > currentTime)
				{
					continue;
				}

				//更新邀请时间
				pkPlayer->GetSocietyAsset()->SetInviteTime(beInviteCharID, currentTime);

				MG_Notify_Invite_CrossRealmTeam NotifyMsg;
				NotifyMsg.m_iInviteCharID = pkPlayer->GetInstance();
				NotifyMsg.m_playerName = TPSTR2STDSTR(pkPlayer->GetOwnerCharName());
				NotifyMsg.m_iMapTypeID = reqMsg.m_iMapTypeID;
				NotifyMsg.m_iTeamID = reqMsg.m_iTeamID;
				pkBeInvitePlayer->SendMsgToClient(MGPT_NOTIFY_CROSSREALM_INVITE, &NotifyMsg);

				++count;
			}
		}
	}
	else if(reqMsg.m_inviteType == INVITE_GUILDFRIENDS)
	{
		//公会好友
		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		if(NULL == pkGuildManager) return;

		//玩家所在的公会信息
		LC_ServerGuildInfo* guild = pkGuildManager->GetPlayerGuild(pkPlayer->GetInstance());
		if(NULL == guild) return;
		
		//公会成员信息
		Guild_Member_Map* guildMemberInfos = guild->GetAllMemberInfo();

		for(Guild_Member_Map::iterator it = guildMemberInfos->begin(); it!=guildMemberInfos->end(); ++it)
		{
			uint64_t beInviteCharID = it->first;
			if(beInviteCharID == pkPlayer->GetInstance())
			{//不邀请自己
				continue;
			}

			//cd时间
			uint64_t beInviteTime= pkPlayer->GetInviteTime(beInviteCharID);
			int64_t currentTime =  GET_CURRENT_TIMESTAMP_IN_SECONDS();
			if(beInviteTime != 0 && beInviteTime+MIN_INVITE_TIME > currentTime)
			{
				continue;
			}

			LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(beInviteCharID));
			if(pkBeInvitePlayer != NULL && pkBeInvitePlayer->GetLevel() >= pkEntry->_kLevel)
			{
				//更新邀请时间
				pkPlayer->SetInviteTime(beInviteCharID, currentTime);

				MG_Notify_Invite_CrossRealmTeam NotifyMsg;
				NotifyMsg.m_iInviteCharID = pkPlayer->GetInstance();
				NotifyMsg.m_playerName = TPSTR2STDSTR(pkPlayer->GetOwnerCharName());
				NotifyMsg.m_iMapTypeID = reqMsg.m_iMapTypeID;
				NotifyMsg.m_iTeamID = reqMsg.m_iTeamID;
				pkBeInvitePlayer->SendMsgToClient(MGPT_NOTIFY_CROSSREALM_INVITE, &NotifyMsg);

				++count;
			}
		}
	}

	MG_RLT_InviteAllPlayer_CrossRealmTeam resqMsg;
	resqMsg.m_Result = count > 0 ? RE_SUCCESS : RE_FAIL;
	pkPlayer->SendMsgToClient(MGPT_INVITE_ALLPLAYER_CROSSREALMTEAM_RESP, &resqMsg);
}

ON_HANDLE(MGPT_CROSSREALM_AUTOADD_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (pkArenaMgr->IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter cross releam failed! in arena sign!");
		return;
	}

	MG_REQ_CrossRealm_AutoAdd_Team reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	CF_CrossRealmMap* thecrossmap = CF_CrossRealmMap::GetSingletonPtr();
	CF_CrossRealmMap::DataEntry* pkEntry = thecrossmap->GetEntryPtr(reqMsg.m_MapTypeId);
	if (pkEntry == NULL)
	{
		return;
	}
	if (pkEntry->_Limit > 0 && pkPlayer->GetCrossRealmTeamAssert()->GetDailyMapTime(reqMsg.m_MapTypeId) >= pkEntry->_Limit)
	{
		MG_RESP_CrossRealm_AutoAdd_Team resp;
		resp.m_MapTypeId = reqMsg.m_MapTypeId;
		resp.m_Result = RE_CROSSREALMMAP_TIME_OUT;
		pkPlayer->SendMsgToClient(MGPT_CROSSREALM_AUTOADD_RESP, &resp);
		return;
	}
	MG_GG_REQ_AutoAdd_Team msg;
	msg.m_MapTypeId = reqMsg.m_MapTypeId;
	msg.m_Size = 1;
	CrossRealmFillPlayInfo(pkPlayer, msg.m_PlayerInfo[0]);
	SendMsgToSociety(MGPT_SS_CROSSREALM_AUTOADD_REQ, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_ENTER_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (pkArenaMgr->IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter cross releam failed! in arena sign!");
		return;
	}

	MG_REQ_CrossRealm_Enter reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_ENTER_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_PLAYER_TEAMINFO_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	SendMsgToSociety(MGPT_SS_CROSSREALM_PLAYER_TEAMINFO_REQ, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CROSSREALM_TEAM_OPERATION_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CrossRealmTeamOperation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	CF_CrossRealmMap::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(reqMsg.m_iMapTypeID);
	if (NULL == pkEntry)
	{
		return;
	}
	SendMsgToSociety(MGPT_CROSSREALM_TEAM_OPERATION_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CS_GetSkyArena_BattleInfo , this_source_type, false)
{
	MG_Req_SkyArenaBattlePlays kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	
	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->RegisteTran(LC_SequenceManager::Instance().GetSequenceID(), TRAN_TYPE_SKY_ARENA, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	if (pkTran)
	{
		if (pkTran->GetStep() != TS_Server_SkyArenaBattleInfo::STEP_REQ_GG_SkyArenaBattleInfo)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer TS_Server_SkyArenaBattleInfo Error!");
			pkTranManager->UnRegisteTran(pkTran->GetID());
			return;
		}

		SS_MG_HEAD kSSMsgHead;
		kSSMsgHead.m_usMsgType = MGPT_CS_GetSkyArena_BattleInfo;
		kSSMsgHead.m_uiUserID = rkMsgHead.m_uiUserID;
		kSSMsgHead.m_uiCharID = rkMsgHead.m_uiCharID;


		ResultType iResultType = pkTran->HandlerMsg(kSSMsgHead, &kRltMsg);
		if (RE_FAIL == iResultType)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());
		}
	}
}

ON_HANDLE(MGPT_CS_GetSkyArena_GradeAwardInfo, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SkyArenaMyScore reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_GG_ReqSkyArena_GradeAwardInfo, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}


ON_HANDLE(MGPT_CS_SUMMIT_ARENA_INFO_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SummitArenaInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_SUMMIT_ARENA_INFO_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}

ON_HANDLE(MGPT_CS_SUMMIT_ARENA_MY_INFO_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SummitArenaMyGroupStage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	UT_ServerHelper::GetSchemeLinkID(reqMsg.m_nMapID, reqMsg.m_nSchemeLinkID);
	SendMsgToGroup(MGPT_SS_SUMMIT_ARENA_MY_INFO_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}


ON_HANDLE(MGPT_CS_SUMMIT_ARENA_AGAINSTLIST_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SummitArenaAgainstList reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_SUMMIT_ARENA_AGAINSTLIST_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}

ON_HANDLE(MGPT_CS_SUMMIT_ARENA_ENTRYLIST_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SummitArenaGroupStageEntryList reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_SUMMIT_ARENA_ENTRYLIST_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}


ON_HANDLE(MGPT_CS_SUMMIT_ARENA_GROUPSTAGE_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SummitArenaGroupStage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_SUMMIT_ARENA_GROUPSTAGE_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}

ON_HANDLE(MGPT_CS_SUMMIT_ARENA_HONOR_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SummitArenaHonorMsg reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_SUMMIT_ARENA_HONOR_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}

ON_HANDLE(MGPT_CS_SUMMITARENA_MY_HONORTOPID_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	

	SendMsgToGroup(MGPT_SS_SUMMITARENA_MY_HONORTOPID_REQ, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}

ON_HANDLE(MGPT_CS_SKYARENA_CHECKBATTLESTAGE_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	
	MG_Req_SkyArenaCheckBattleStage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	UT_ServerHelper::GetSchemeLinkID(reqMsg.m_nMapID, reqMsg.m_nSchemeLinkID);
	SendMsgToGroup(MGPT_SS_SKYARENA_CHECKBATTLESTAGE_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

}


ON_HANDLE(MGPT_REQ_GETRANKTYPEINFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( pkPlayer == NULL )
	{
		return;
	}

	MG_REQ_GetRankTypeInfo msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	SendMsgToGroup(MGPT_REQ_G2G_GETRANKTYPEINFO, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}
}
