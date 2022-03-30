#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "NW_Helper.h"
#include "MG_CS_FriendProtocol.h"
#include "MG_CS_PromoteProtocol.h"
#include "MG_Friend.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "GameServerApp.h"
#include "LC_ServerPlayer.h"
#include "MG_Promote.h"
#include "UT_ServerHelper.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerPlayerGroup.h"

using namespace Protocol;
using namespace GameLogic;
using namespace Utility;

namespace Protocol
{
#define this_source_type SERVER_TYPE_SOCIETY

ON_HANDLE(MGPT_GF_RLT_GET_FRIEND_LIST, this_source_type, false)
{
	MG_SS_RLT_Friend_List kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_SocietyAsset* pkSocietyAsset = pkPlayer->GetSocietyAsset();
	LC_FriendInfo* pkFriendInfo = pkSocietyAsset->GetFriendInfo();

	unique_id_type playerCharID = pkPlayer->GetInstance();

	MG_RLT_GetFriendList kRltMsg;
	if (kMsg.m_iResult == RE_SUCCESS)
	{
		ST_SS_Friend_List& rFriendList = kMsg.m_rFriendList;
		ST_SS_Friend_List& rRltFriendList = kRltMsg.m_rFriendList;

		uint16_t uiFriendCount = rFriendList.m_iCount;
		if (uiFriendCount > MAX_FRIEND_NUM)
		{
			uiFriendCount = MAX_FRIEND_NUM;
		}

		rRltFriendList.m_iCount = 0;

		pkSocietyAsset->GetFriendInfo()->Reset();
		for (int i = 0; i < uiFriendCount; ++i)
		{
			const PS_FriendInfo& kDBInfo = rFriendList.m_akInfos[i];							// 数据库返回信息
			pkSocietyAsset->AddSociety(ST_FRIEND, kDBInfo.kBasicInfo.uiCharID, kDBInfo.kBasicInfo.szCharName.c_str());
			// 将数据保存到资产中
			LC_FriendDetailInfo* pkInfoTemp = pkFriendInfo->GetFriendDetailInfo(kDBInfo.kBasicInfo.uiCharID);
			if (NULL == pkInfoTemp || NULL == pkInfoTemp->GetBasic())
			{
				continue;
			}

			int groupID = 0;
			LC_ServerPlayerGroupManager* pkPlayerGroupManager = (LC_ServerPlayerGroupManager*)SERVER_GET_PLAYER_GROUP_MANAGER();
			LC_ServerPlayerGroup* pkGroup = pkPlayerGroupManager->GetPlayerGroup(kDBInfo.kBasicInfo.uiCharID);
			if(NULL != pkGroup)
			{
				groupID = pkGroup->GetID();
			}

			pkInfoTemp->GetFromProtocol(groupID, kDBInfo);

			LC_SocietyBasicInfo* pkBasic = pkInfoTemp->GetBasic();
			if (pkBasic != NULL)
			{
				pkBasic->SetDegree(pkPlayer->GetSocietyAsset()->GetDegree(pkBasic->GetCharID()));
				LC_ServerPlayer* pkFriendTemp = static_cast<LC_ServerPlayer*>(pkPlayerManager->FindPlayer(pkBasic->GetCharID()));
				if (NULL != pkFriendTemp)
				{
					pkInfoTemp->SetMapID((int16_t)pkFriendTemp->GetMapResID());
					pkInfoTemp->SetArea((int16_t)pkFriendTemp->GetCurrentLogicAreaID());
					pkBasic->SetCharType(pkFriendTemp->GetCharType());
					pkBasic->SetStatus(pkFriendTemp->GetSocietyAsset()->GetStatus());
					pkBasic->SetCareer(pkFriendTemp->GetCareerType());
					pkBasic->SetWeaponType(pkFriendTemp->GetWeaponAttribute());
					pkBasic->SetLevel(pkFriendTemp->GetLevel());
					pkBasic->SetBornCharType(pkFriendTemp->GetProfilePictureID());
					pkInfoTemp->SetCharMood(pkFriendTemp->GetCharMood());
					//pkBasic->SetScore(pkFriendTemp->GetTotalScore());
					pkBasic->SetScore(pkFriendTemp->GetControllerCombatScore());
					const platform_vip_info& pf_info = pkFriendTemp->GetPlatformVipInfo();
					for(platform_vip_info::const_iterator iter = pf_info.begin(); iter != pf_info.end(); iter++)
					{
						pkBasic->AddPlatformInfo(iter->first, iter->second.format_vip_info());
					}
				}
			}
			bool tag = pkPlayer->GetSocietyAsset()->IsInBeSociety(ST_FRIEND, kDBInfo.kBasicInfo.uiCharID);
			pkInfoTemp->SetBeFriendWithEachOther(tag);
			pkInfoTemp->SetToProtocol(rRltFriendList.m_akInfos[rRltFriendList.m_iCount++]);
		}
	}

	kRltMsg.m_iResultType = kMsg.m_iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_FRIEND_LIST, &kRltMsg);
}

ON_HANDLE(MGPT_GF_RLT_GET_ENEMY_LIST, this_source_type, false)
{
	MG_SS_RLT_Enemy_List kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_SocietyAsset* pkSocietyAsset = pkPlayer->GetSocietyAsset();
	LC_EnemyInfo* pkEnemyInfo = pkSocietyAsset->GetEnemyInfo();

	MG_RLT_GetEnemyList kRltMsg;
	if (kMsg.m_iResult == RE_SUCCESS)
	{
		pkEnemyInfo->Reset();

		ST_SS_Enemy_List& rEnemyList = kMsg.m_rEnemyList;
		ST_SS_Enemy_List& rRtlEnemyList = kRltMsg.m_rEnemyList;

		uint16_t uiCount = rEnemyList.m_iCount;
		if (uiCount > MAX_ENEMY_NUM)
		{
			uiCount = MAX_ENEMY_NUM;
		}

		rRtlEnemyList.m_iCount = 0;

		for (int i = 0; i < uiCount; ++i)
		{
			const PS_SocietyBasicInfo& kDBInfo = rEnemyList.m_akInfos[i];		// 数据库返回信息
			// 将数据保存到资产中
			pkSocietyAsset->AddSociety(ST_ENEMY, kDBInfo.uiCharID, kDBInfo.szCharName.c_str());
			LC_SocietyBasicInfo* pkInfoTemp = pkEnemyInfo->GetSocietyBasicInfo(kDBInfo.uiCharID);
			if (NULL == pkInfoTemp)
			{
				continue;
			}
			pkInfoTemp->GetFromProtocol(kDBInfo);
			pkInfoTemp->SetDegree(pkPlayer->GetSocietyAsset()->GetDegree(pkInfoTemp->GetCharID()));

			// 是否在线，在线的话，更新在线信息到对方
			LC_ServerPlayer* pkEnemyTemp = static_cast<LC_ServerPlayer*>(pkPlayerManager->FindPlayer(pkInfoTemp->GetCharID()));
			if (pkEnemyTemp != NULL)
			{
				pkInfoTemp->SetCharType(pkEnemyTemp->GetCharType());
				pkInfoTemp->SetStatus(pkEnemyTemp->GetSocietyAsset()->GetStatus());
				pkInfoTemp->SetCareer(pkEnemyTemp->GetCareerType());
				pkInfoTemp->SetLevel(pkEnemyTemp->GetLevel());
				pkInfoTemp->SetWeaponType(pkEnemyTemp->GetWeaponAttribute());
				pkInfoTemp->SetScore(pkEnemyTemp->GetControllerCombatScore());
				pkInfoTemp->SetPlatformInfo(pkInfoTemp->GetPlatformInfo());
			}
			rRtlEnemyList.m_akInfos[rRtlEnemyList.m_iCount].beFriendWithEachOther = 0;
			pkInfoTemp->SetToProtocol(rRtlEnemyList.m_akInfos[rRtlEnemyList.m_iCount++]);
		}
	}
	kRltMsg.m_iResultType = kMsg.m_iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_ENEMY_LIST, &kRltMsg);
}

ON_HANDLE(MGPT_GF_RLT_GET_BLACKLIST, this_source_type, false)
{
	MG_SS_RLT_Black_List kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_SocietyAsset* pkSocietyAsset = pkPlayer->GetSocietyAsset();
	LC_BlackListInfo* pkBlackListInfo = pkSocietyAsset->GetBlackListInfo();

	MG_RLT_GetBlackList kRltMsg;
	if (kMsg.m_iResult == RE_SUCCESS)
	{
		pkBlackListInfo->Reset();

		ST_SS_Black_List& rBlackList = kMsg.m_rBlackList;
		ST_SS_Black_List& rRtlBlackList = kRltMsg.m_rBlackList;

		uint16_t uiCount = rBlackList.m_iCount;
		if (uiCount > MAX_BLACKLIST_NUM)
		{
			uiCount = MAX_BLACKLIST_NUM;
		}

		rRtlBlackList.m_iCount = 0;
		

		for (int i = 0; i < uiCount; ++i)
		{
			const PS_SocietyBasicInfo& kDBInfo = rBlackList.m_akInfos[i];		// 数据库返回信息

			pkSocietyAsset->AddSociety(ST_BLACKLIST, kDBInfo.uiCharID, kDBInfo.szCharName.c_str());
			LC_SocietyBasicInfo* pkInfoTemp = pkBlackListInfo->GetSocietyBasicInfo(kDBInfo.uiCharID);
			if (NULL == pkInfoTemp)
			{
				continue;
			}
			pkInfoTemp->GetFromProtocol(kDBInfo);
			pkInfoTemp->SetDegree(pkPlayer->GetSocietyAsset()->GetDegree(pkInfoTemp->GetCharID()));

			LC_ServerPlayer* pkBlackTemp = static_cast<LC_ServerPlayer*>(pkPlayerManager->FindPlayer(pkInfoTemp->GetCharID()));
			if (pkBlackTemp != NULL)
			{
				pkInfoTemp->SetCharType(pkBlackTemp->GetCharType());
				pkInfoTemp->SetStatus(pkBlackTemp->GetSocietyAsset()->GetStatus());
				pkInfoTemp->SetCareer(pkBlackTemp->GetCareerType());
				pkInfoTemp->SetLevel(pkBlackTemp->GetLevel());
				pkInfoTemp->SetWeaponType(pkBlackTemp->GetWeaponAttribute());
				pkInfoTemp->SetScore(pkBlackTemp->GetControllerCombatScore());
				const platform_vip_info& pf_info = pkBlackTemp->GetPlatformVipInfo();
				for(platform_vip_info::const_iterator iter = pf_info.begin(); iter != pf_info.end(); iter++)
				{
					pkInfoTemp->AddPlatformInfo(iter->first, iter->second.format_vip_info());
				}
			}
			pkInfoTemp->SetFriendEachOther(0);
			pkInfoTemp->SetToProtocol(rRtlBlackList.m_akInfos[rRtlBlackList.m_iCount++]);
		}
	}
	kRltMsg.m_iResultType = kMsg.m_iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_BLACKLIST, &kRltMsg);
}

ON_HANDLE(MGPT_GF_RLT_SET_RELATION, this_source_type, false)
{
	MG_SS_RLT_SetRelation kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	char_data_define::friend_relation_info& relation = kMsg.m_kRelation;
	char_data_define::friend_char_info& char_info = kMsg.m_kChar;
	pkPlayer->ChangeCharFriendRelation(kMsg.m_iRslt, relation, char_info, kMsg.m_szMsg);
}

ON_HANDLE(MGPT_GF_REQUEST_NOTIFY, this_source_type, false)
{
	MG_SS_RequestNotify kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_SocietyAsset* pkSocietyAsset = pkPlayer->GetSocietyAsset();
	for (uint32_t i = 0; i < kMsg.m_uiRequestCnt; i++)
	{
		LC_FriendRequestInfo reqIF;
		reqIF.m_uiUserID = kMsg.m_kRequests[i].m_uiUserID;
		reqIF.m_uiCharID = kMsg.m_kRequests[i].m_uiCharID;
		STRNCPY(reqIF.m_szCharName, kMsg.m_kRequests[i].m_szCharName.c_str(), sizeof(reqIF.m_szCharName));
		STRNCPY(reqIF.m_szMsg, kMsg.m_kRequests[i].m_szMsg.c_str(), sizeof(reqIF.m_szMsg));
		reqIF.m_uiCareer = kMsg.m_kRequests[i].m_uiCareer;
		reqIF.m_uiWeaponType = kMsg.m_kRequests[i].m_uiWeaponType;
		reqIF.m_uiLevel = kMsg.m_kRequests[i].m_uiLevel;
		pkSocietyAsset->AppendRequest(reqIF);
	}

	//handle request list
	LC_FriendRequestInfo reqIF;
	if (pkPlayer->GetSocietyAsset()->PopRequest(reqIF))
	{
		MG_REQ_Confirm_BeAddFriend xConfirmMsg;
		xConfirmMsg.m_lTargetCharID = reqIF.m_uiCharID;
		xConfirmMsg.m_szCharName = reqIF.m_szCharName;
		xConfirmMsg.m_szMsg = reqIF.m_szMsg;
		xConfirmMsg.m_uiCareer = reqIF.m_uiCareer;
		xConfirmMsg.m_uiWeaponType = reqIF.m_uiWeaponType;
		xConfirmMsg.m_uiLevel = reqIF.m_uiLevel;
		pkPlayer->SendMsgToClient(MGPT_REQ_CONFIRM_BEADDFRIEND, &xConfirmMsg);
	}
}
//////////////////////////////////////////////////////////////////////////
//promote related
ON_HANDLE(MGPT_SS_PROMOTE_PROMOTE_INFO_SYNC, this_source_type, false)
{
	MG_SS_PromoteInfoSync kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_PromoterAsset& kPromoterAsset = pkPlayer->GetPromoterAsset();
	kPromoterAsset.SetID(kMsg.m_uiID);
	kPromoterAsset.GetAsPromoteeInfo().GetDataFromProtocol(kMsg.m_kAsPromotee);
	kPromoterAsset.GetMyPromoteesInfo().GetDataFromProtocol(kMsg.m_kMyPromotees);
	kPromoterAsset.UpdateDirtyFlag();
}

ON_HANDLE(MGPT_SS_PROMOTE_SET_AS_PROMOTEE_INFO_RESP, this_source_type, false)
{
	MG_SS_SetAsPromoteeInfoResp kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	MG_AsPromoteeOperationRESP resp;
	resp.m_uiOpCode = kMsg.m_uiOpCode;
	resp.m_uiParamA = kMsg.m_uiParamA;
	resp.m_uiParamB = kMsg.m_uiParamB;
	resp.m_uiRslt = kMsg.m_uiRslt;
	resp.m_kAsPromotee = kMsg.m_kAsPromotee;

	if (kMsg.m_uiRslt == RE_SUCCESS)
	{
		LC_PromoterAsset& kPromoterAsset = pkPlayer->GetPromoterAsset();
		kPromoterAsset.GetAsPromoteeInfo().GetDataFromProtocol(kMsg.m_kAsPromotee);
		switch (resp.m_uiOpCode)
		{
			case PS_AS_PRO_OP_SET:
				{
					//互相加好友
					//这样做是建立在好友关系 没有附加信息的基础上的
					//比如没有好友度 和分组  否则要修改协议了
					PS_CharBasicDescip& kCharDescrip = resp.m_kAsPromotee.m_kCharBasic;

					//被加
					MG_SS_REQ_SetRelation reqMsg;
					char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
					relation.set_char_id(rkMsgHead.m_uiCharID);
					reqMsg.m_nServerID = GetGameServerEntryID();

					uint32_t nf = relation.flag() | (1 << char_data_define::friend_relation_info_relation_type_FRIEND);

					relation.set_flag(nf);
					SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, kCharDescrip.m_uiUserID, kCharDescrip.m_uiCharID);

					//加对方
					relation.set_char_id(kCharDescrip.m_uiCharID);
					pkPlayer->GetSocietyAsset()->FillDataToPB(relation);
					SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
				}
				break;
			case PS_AS_PRO_OP_REWARD:
				{
					pkPlayer->HandlePromoteReward(CPRT_AS_PROMOTEE);
				}
				break;
			default:
				break;
		}
		kPromoterAsset.UpdateDirtyFlag();
	}
	pkPlayer->SendMsgToClient(MGPT_AS_PROMOTEE_OPERATION_RESP, &resp);
}

ON_HANDLE(MGPT_SS_PROMOTE_PROMOTEE_LIST_RESP, this_source_type, false)
{
	MG_SS_PromoteeListResp kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	MG_MyPromoteesInfoRESP resp;
	resp.m_uiPageIndex = kMsg.m_uiPageIndex;
	resp.m_uiPageGranu = kMsg.m_uiPageGranu;
	resp.m_uiPageCount = kMsg.m_uiPageCount;
	resp.m_kPromotees.insert(resp.m_kPromotees.end(), kMsg.m_kPromotees.begin(), kMsg.m_kPromotees.end());
	pkPlayer->SendMsgToClient(MGPT_MY_PROMOTEES_INFO_RESP, &resp);
}

ON_HANDLE(MGPT_MY_PROMOTEE_OPERATION_RESP, this_source_type, false)
{
	MG_SS_SetCorrePromoterInfoResp kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	MG_MyPromoteeOperationRESP resp;
	resp.m_uiCharID = kMsg.m_uiCharID;
	resp.m_uiOpCode = kMsg.m_uiOpCode;
	resp.m_uiParamA = kMsg.m_uiParamA;
	resp.m_uiParamB = kMsg.m_uiParamB;
	resp.m_uiRslt = kMsg.m_uiRslt;
	resp.m_kCorrePromoter = kMsg.m_kCorrePromoter;
	switch (resp.m_uiOpCode)
	{
		case PS_MY_PRO_OP_REWARD:
			{
				pkPlayer->HandlePromoteReward(CPRT_CORRE_PROMOTER);
			}
			break;
		default:
			break;
	}
	pkPlayer->SendMsgToClient(MGPT_MY_PROMOTEE_OPERATION_RESP, &resp);
}

ON_HANDLE(MGPT_GF_RESP_FRIENDDETAIL, this_source_type, false)
{
	MG_SS_RESP_Friend_Detail kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	MG_RLT_GetFriendDetail kRltMsg;
	kRltMsg.m_uiCharID = kMsg.m_uiCharID;
	kRltMsg.m_iResult = RE_SUCCESS;
	kRltMsg.m_kFriendInfo = kMsg.m_kFriendInfo;
	kRltMsg.m_kFriendInfo.kBasicInfo.beFriendWithEachOther = false;
	pkPlayer->SendMsgToClient(MGPT_RLT_GETFRIEND_DETAIL, &kRltMsg);
}

ON_HANDLE(MGPT_GF_RLT_USER_ONLINE_INFO, this_source_type, false)
{
	MG_RESP_UserOnlineInfo kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_USER_ONLINE_INFO, &kMsg);
}

ON_HANDLE(MGPT_SS_ONLINE_USER_SYNC, this_source_type, false)
{
	MG_RESP_UserOnlineInfo kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
}

ON_HANDLE(MGPT_SS_ONLINE_USER_ADD, this_source_type, false)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
}

ON_HANDLE(MGPT_SS_ONLINE_USER_DEL, this_source_type, false)
{
	MG_RESP_UserOnlineInfo kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

}

}
