#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "MG_CS_FriendProtocol.h"
#include "MG_CS_PromoteProtocol.h"
#include "MG_Friend.h"
#include "MG_Promote.h"
#include "MG_GM2Login.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_FRIEND, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddFriend msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	ResultType iResultAddFriend = RE_SUCCESS;
	do
	{
		if (pkPlayer->GetInstance() == msg.m_lTargetCharID)
		{
			iResultAddFriend = RE_ADD_FRIEND_FAILED_CANT_SELF_ADD;
			break;
		}
		LC_FriendInfo* pkFriendInfo = pkPlayer->GetSocietyAsset()->GetFriendInfo();
		if (/*NULL == pkFriendInfo || */pkFriendInfo->IsExist(msg.m_lTargetCharID))
		{
			iResultAddFriend = RE_ADD_FRIEND_FAILED_IS_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(msg.m_lTargetCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);
			relation.set_flag(relation.flag() | (1<<char_data_define::friend_relation_info_relation_type_FRIEND));
			reqMsg.m_szMsg = msg.m_szMsg;
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		//添加好友--埋点日志
		vector<uint64_t> nParams;
		nParams.clear();
		nParams.push_back(msg.m_lTargetCharID);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, FRIENDS_OPERATE, OPERATE_LOGTYPE1);
		RecordOperateLog(pkPlayer,BASE_OPERATE, nLogCode,nParams);
		return;
	}while (0);

	MG_RLT_AddFriend rltMsg;
	rltMsg.m_lTargetCharID = msg.m_lTargetCharID;
	rltMsg.m_iResultType = iResultAddFriend;
	rltMsg.m_szCharName = msg.szCharName;
	pkPlayer->SendMsgToClient(MGPT_RLT_ADD_FRIEND, &rltMsg);
}
//-----------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_RECOMMEND_FRIEND, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddRecommendFriend msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	ResultType iResultAddFriend = RE_SUCCESS;

	std::vector<unique_id_type>& m_lTargetCharIDList = msg.m_lTargetCharIDList;
	for(int i=0;i<m_lTargetCharIDList.size();++i)
	{
		unique_id_type targetID = m_lTargetCharIDList[i];
		if (pkPlayer->GetInstance() == targetID)
		{
			iResultAddFriend = RE_ADD_FRIEND_FAILED_CANT_SELF_ADD;
			break;
		}
		LC_FriendInfo* pkFriendInfo = pkPlayer->GetSocietyAsset()->GetFriendInfo();
		if (/*NULL == pkFriendInfo || */pkFriendInfo->IsExist(targetID))
		{
			iResultAddFriend = RE_ADD_FRIEND_FAILED_IS_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(targetID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);
			relation.set_flag(relation.flag() | (1<<char_data_define::friend_relation_info_relation_type_FRIEND));
			reqMsg.m_szMsg = msg.m_szMsg;
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
	}

	MG_RLT_AddRecommendFriend rltMsg;
	rltMsg.m_iResultType = iResultAddFriend;
	pkPlayer->SendMsgToClient(MGPT_RLT_ADD_RECOMMEND_FRIEND, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DEL_FRIEND, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_DelFriend msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_FriendInfo* pkFriendInfo = pkPlayer->GetSocietyAsset()->GetFriendInfo();
	ResultType iResultDelFriend = RE_SUCCESS;
	do
	{
		if (!pkFriendInfo->IsExist(msg.m_lTargetCharID))
		{
			iResultDelFriend = RE_DEL_FRIEND_FAILED_IS_NOT_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(msg.m_lTargetCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);
			uint32_t nf = relation.flag() & (~(1 << char_data_define::friend_relation_info_relation_type_FRIEND));
			relation.set_flag(nf);
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		//删除好友--埋点日志
		vector<uint64_t> nParams;
		nParams.clear();
		nParams.push_back(msg.m_lTargetCharID);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, FRIENDS_OPERATE, OPERATE_LOGTYPE2);
		RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode,nParams);
		return;
	}while (0);

	MG_RLT_DelFriend rltMsg;
	rltMsg.m_iResultType = iResultDelFriend;
	rltMsg.m_lTargetCharID = msg.m_lTargetCharID;
	pkPlayer->SendMsgToClient(MGPT_RLT_DEL_FRIEND, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_FRIEND_RECOMMEND_FRIEND, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer) return;

	MG_REQ_Recommend_Friend reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize)) return;

	pkPlayer->RecommendFriend();
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_ENEMY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddEnemy msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	ResultType iResult = RE_SUCCESS;
	LC_EnemyInfo* pkEnemyInfo = pkPlayer->GetSocietyAsset()->GetEnemyInfo();
	do
	{
		if (pkPlayer->GetInstance() == msg.m_lTargetCharID)
		{
			iResult = RE_ADD_ENEMY_FAILED_CANT_SELF_ADD;
			break;
		}
		if (pkEnemyInfo->IsExist(msg.m_lTargetCharID))
		{
			iResult = RE_ADD_ENEMY_FAILED_IS_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(msg.m_lTargetCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);

			uint32_t nf = relation.flag() | (1 << char_data_define::friend_relation_info_relation_type_ENEMY);
			relation.set_flag(nf);
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		//添加仇人--埋点日志
		vector<uint64_t> nParams;
		nParams.clear();
		nParams.push_back(msg.m_lTargetCharID);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, FRIENDS_OPERATE, OPERATE_LOGTYPE3);
		RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode,nParams);
		return;
	}while (0);

	MG_RLT_AddEnemy rltMsg;
	rltMsg.m_lTargetCharID = msg.m_lTargetCharID;
	rltMsg.m_iResultType = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_ADD_ENEMY, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DEL_ENEMY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_DelEnemy msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	ResultType iResult = RE_SUCCESS;
	LC_EnemyInfo* pkEnemyInfo = pkPlayer->GetSocietyAsset()->GetEnemyInfo();
	do
	{
		if (!pkEnemyInfo->IsExist(msg.m_lTargetCharID))
		{
			iResult = RE_DEL_ENEMY_FAILED_IS_NOT_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(msg.m_lTargetCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);

			uint32_t nf = relation.flag() & (~(1 << char_data_define::friend_relation_info_relation_type_ENEMY));
			relation.set_flag(nf);
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		return;
	}while (0);

	MG_RLT_DelEnemy rltMsg;
	rltMsg.m_lTargetCharID = msg.m_lTargetCharID;
	rltMsg.m_iResultType = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_DEL_ENEMY, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_BLACKLIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddBlackList msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	ResultType iResult = RE_SUCCESS;
	LC_BlackListInfo* pkBlacklistInfoInfo = pkPlayer->GetSocietyAsset()->GetBlackListInfo();
	do
	{
		if (pkPlayer->GetInstance() == msg.m_lTargetCharID)
		{
			iResult = RE_ADD_BLACKLIST_FAILED_CANT_SELF_ADD;
			break;
		}
		if (pkBlacklistInfoInfo->IsExist(msg.m_lTargetCharID))
		{
			iResult = RE_ADD_BLACKLIST_FAILED_IS_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(msg.m_lTargetCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);

			uint32_t nf = ((relation.flag() & (~(1 << char_data_define::friend_relation_info_relation_type_FRIEND))) | (1 << char_data_define::friend_relation_info_relation_type_BLACK));
			relation.set_flag(nf);
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		//添加黑名单--埋点日志
		vector<uint64_t> nParams;
		nParams.clear();
		nParams.push_back(msg.m_lTargetCharID);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, FRIENDS_OPERATE, OPERATE_LOGTYPE4);
		RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode,nParams);
		return;
	}while (0);

	MG_RLT_AddBlackList rltMsg;
	rltMsg.m_iResultType = iResult;
	rltMsg.m_lTargetCharID = msg.m_lTargetCharID;
	pkPlayer->SendMsgToClient(MGPT_RLT_ADD_BLACKLIST, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DEL_BLACKLIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_DelBlackList msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	ResultType iResult = RE_SUCCESS;
	LC_BlackListInfo* pkBlacklistInfoInfo = pkPlayer->GetSocietyAsset()->GetBlackListInfo();
	do
	{
		if (!pkBlacklistInfoInfo->IsExist(msg.m_lTargetCharID))
		{
			iResult = RE_DEL_BLACKLIST_FAILED_IS_NOT_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(msg.m_lTargetCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);

			uint32_t nf = relation.flag() & (~(1 << char_data_define::friend_relation_info_relation_type_BLACK));
			relation.set_flag(nf);
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		return;
	}while (0);

	MG_RLT_DelBlackList rltMsg;
	rltMsg.m_lTargetCharID = msg.m_lTargetCharID;
	rltMsg.m_iResultType = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_DEL_BLACKLIST, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_CHANGE_FRIENDGROUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ChangeFriendGroup msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	ResultType iResult = RE_SUCCESS;
	do
	{
		int32_t lGroupID = msg.m_iTargetGroupID;
		if (lGroupID < LC_FriendDetailInfo::MIN_GROUP_ID || lGroupID > LC_FriendDetailInfo::MAX_GROUP_ID)
		{
			iResult = RE_CHANGE_FRIEND_GROUP_FAILED_GROUP_WRONG;
			break;
		}
		LC_FriendInfo* pkFriendInfo = pkPlayer->GetSocietyAsset()->GetFriendInfo();
		LC_FriendDetailInfo* pkFriendDatailInfo = pkFriendInfo->GetFriendDetailInfo(msg.m_lFriendCharID);
		if (NULL == pkFriendDatailInfo)
		{
			iResult = RE_CHANGE_FRIEND_GROUP_FAILED_IS_NOT_EXIST;
			break;
		}
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(msg.m_lFriendCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		return;
	}while (0);

	MG_RLT_ChangeFriendGroup rltMsg;
	rltMsg.m_lFriendCharID = msg.m_lFriendCharID;
	rltMsg.m_iTargetGroupID = msg.m_iTargetGroupID;
	rltMsg.m_iResultType = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_CHANGE_FRIENDGROUP, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GET_FRIEND_STATUS, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetFriendStatus msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_SocietyAsset* pSocietyAsset = pkPlayer->GetSocietyAsset();

	MG_RLT_GetFriendStatus rltMsg;
	MG_REQ_UpdatePlayerState regMsg;
	int iOfflineCount = 0;
	int iCount = 0;
	int32_t nServerID = GetGameServerID();

	if (ST_FRIEND == msg.m_uiSocietyType)
	{
		LC_FriendInfo* pkFriendInfo = pSocietyAsset->GetFriendInfo();
		FriendDetailInfoMap& rDetailInfo = pkFriendInfo->GetDetails();
		for (FriendDetailInfoMap::iterator it = rDetailInfo.begin(); it != rDetailInfo.end(); ++it)
		{
			if(iCount >= MAX_FRIEND_NUM || iOfflineCount >= MAX_PLAYER_STATE_COUNT)
			{
				break;
			}
			LC_FriendDetailInfo& kInfo = it->second;
			LC_ServerPlayerEnterLeaveManager* pIOMgr = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
			if ( pIOMgr == NULL )
				break;
			bool bFound = pIOMgr->CheckOnlinePlayer( kInfo.GetCharID() );
			LC_ServerPlayer* pkPlayerTemp = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kInfo.GetCharID());
			if (pkPlayerTemp || bFound )
			{
				rltMsg.m_lOnlinePlayerCharID[iCount] = kInfo.GetCharID();
				rltMsg.m_lOnlinePlayerDegree[iCount] = kInfo.GetDegree();
				rltMsg.m_lOnlinePlayerServerID[iCount] = nServerID;
				++iCount;
			}
			else
			{
				regMsg.m_aTargetCharIDInfo[iOfflineCount++] = kInfo.GetCharID();
			}
		}
	}
	else if (ST_ENEMY == msg.m_uiSocietyType)
	{
		LC_EnemyInfo* pkEnemyInfo = pSocietyAsset->GetEnemyInfo();
		LC_SocietyBasicInfoIndices& rSocietyInfo = pkEnemyInfo->GetObjects();
		for (LC_SocietyBasicInfoIndices::iterator it = rSocietyInfo.begin(); it != rSocietyInfo.end(); ++it)
		{
			if(iCount >= MAX_FRIEND_NUM)
			{
				break;
			}
			LC_SocietyBasicInfo* pkBasic = pkEnemyInfo->GetSocietyBasicInfo(*it);
			if (pkBasic)
			{
				LC_ServerPlayer* pkPlayerTemp = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(pkBasic->GetCharID());
				if (pkPlayerTemp)
				{
					rltMsg.m_lOnlinePlayerCharID[iCount++] = pkBasic->GetCharID();
				}
			}
		}
	}

	rltMsg.m_uiSocietyType = msg.m_uiSocietyType;
	rltMsg.m_currentArraySize = iCount;
	rltMsg.m_bClearStatus = true;

	pkPlayer->SendMsgToClient(MGPT_RLT_GET_FRIEND_STATUS, &rltMsg);
	if (iOfflineCount > 0)
	{
		regMsg.m_usTargetSize = iOfflineCount;
		SendMsgToChat(MGPT_REQ_UPDATE_PLAYERSTATE, &regMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GETFRIEND_DETAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GetFriendDetail kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_FriendDetailInfo* pkFriend = pkPlayer->GetSocietyAsset()->GetFriendInfo()->GetFriendDetailInfo(kMsg.m_uiCharID);
	if (NULL == pkFriend)
	{
		return;
	}

	MG_RLT_GetFriendDetail kRltMsg;
	kRltMsg.m_uiCharID = kMsg.m_uiCharID;

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(kMsg.m_uiCharID));
	if (pkTargetPlayer != NULL)
	{
		MG_RLT_GetFriendDetail kRltMsg;
		kRltMsg.m_uiCharID = kMsg.m_uiCharID;
		kRltMsg.m_iResult = RE_SUCCESS;
		pkTargetPlayer->fillCharFriendInfo(kRltMsg.m_kFriendInfo);
		kRltMsg.m_kFriendInfo.kBasicInfo.beFriendWithEachOther = pkTargetPlayer->GetSocietyAsset()->IsInBeSociety(ST_FRIEND, pkPlayer->GetInstance());
		pkPlayer->SendMsgToClient(MGPT_RLT_GETFRIEND_DETAIL, &kRltMsg);
	}
	else
	{
		//转发到society
		MG_SS_REQ_Friend_Detail reqMsg;
		reqMsg.m_uiCharID = kMsg.m_uiCharID;
		SendMsgToSociety(MGPT_GF_REQ_FRIENDDETAIL, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_PLAYER_DELTADATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_PlayerDeltaData kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_PlayerDeltaData kRltMsg;
	kRltMsg.m_lTargetCharID = kMsg.m_lTargetCharID;

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(kRltMsg.m_lTargetCharID));

	if (pkTargetPlayer)
	{
		kRltMsg.m_iResult = RE_SUCCESS;

		PS_SocietyDeltaData& rkSocietyDeltaData = kRltMsg.m_kData;
		rkSocietyDeltaData.byCharType = static_cast < int8_t >(pkTargetPlayer->GetCharType()) ;
		rkSocietyDeltaData.byStatus = static_cast < int8_t >(pkTargetPlayer->GetSocietyAsset()->GetStatus());
		rkSocietyDeltaData.byCareer = static_cast < int8_t >(pkTargetPlayer->GetCareerType());
		rkSocietyDeltaData.byWeaponType = static_cast <int8_t>(pkTargetPlayer->GetWeaponAttribute());
		rkSocietyDeltaData.sLevel = static_cast < int16_t >(pkTargetPlayer->GetLevel());
		rkSocietyDeltaData.sMapID = static_cast < int16_t >(pkTargetPlayer->GetMapResID());
		rkSocietyDeltaData.sArea = static_cast < int16_t >(pkTargetPlayer->GetCurrentLogicAreaID());
		rkSocietyDeltaData.iGroupID = pkTargetPlayer->GetPlayerGroupID();
		TPSTR2STDSTR(pkTargetPlayer->GetCharMood(), rkSocietyDeltaData.szMood);
	}
	else
	{
		kRltMsg.m_iResult = RE_FAIL;
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_PLAYER_DELTADATA, &kRltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SOCIETY_DELTADATA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SocietyDeltaData kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SocietyDeltaData kRltMsg;
	kRltMsg.m_uiSocietyType = kMsg.m_uiSocietyType;

	int iCount = 0;
	if (ST_FRIEND == kMsg.m_uiSocietyType)
	{
		LC_FriendInfo* pkFriendInfo = pkPlayer->GetSocietyAsset()->GetFriendInfo();
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		FriendDetailInfoMap::iterator it = pkFriendInfo->GetDetails().begin();
		for (int i = 0; it != pkFriendInfo->GetDetails().end() && i < MAX_FRIEND_NUM; ++i, ++it)
		{
			LC_FriendDetailInfo& kInfo = it->second;
			{
				LC_ServerPlayer* pkPlayerTemp = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kInfo.GetCharID());
				if (pkPlayerTemp)
				{
					if (iCount < 0 || iCount >= MAX_FRIEND_NUM)
					{
						GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "arr too big-5! count:%d", iCount);
						return;
					}
					PS_SocietyDeltaData& rkSocietyDeltaData = kRltMsg.m_kData[iCount];
					kRltMsg.m_alTargetCharID[iCount] = kInfo.GetCharID();

					rkSocietyDeltaData.byCharType = static_cast < int8_t >(pkPlayerTemp->GetCharType()) ;
					rkSocietyDeltaData.byStatus = static_cast < int8_t >(pkPlayerTemp->GetSocietyAsset()->GetStatus());
					rkSocietyDeltaData.byCareer = static_cast < int8_t >(pkPlayerTemp->GetCareerType());
					rkSocietyDeltaData.byWeaponType = static_cast <int8_t>(pkPlayerTemp->GetWeaponAttribute());
					rkSocietyDeltaData.sLevel = static_cast < int16_t >(pkPlayerTemp->GetLevel());
					rkSocietyDeltaData.sMapID = static_cast < int16_t >(pkPlayerTemp->GetMapResID());
					rkSocietyDeltaData.sArea = static_cast < int16_t >(pkPlayerTemp->GetCurrentLogicAreaID());
					TPSTR2STDSTR(pkPlayerTemp->GetCharMood(), rkSocietyDeltaData.szMood);
					rkSocietyDeltaData.iGroupStatus = (pkPlayerTemp->GetPlayerGroup() != NULL);
					rkSocietyDeltaData.uiDegree = pkPlayer->GetSocietyAsset()->GetDegree(kInfo.GetCharID());
					rkSocietyDeltaData.iGroupID = pkPlayerTemp->GetPlayerGroupID();
					//-----------更新资产-----------------------------
					LC_SocietyBasicInfo* pkBasic = kInfo.GetBasic();
					if (pkBasic)
					{
						pkBasic->SetCharType(rkSocietyDeltaData.byCharType);
						pkBasic->SetCareer(rkSocietyDeltaData.byCareer);
						pkBasic->SetWeaponType(rkSocietyDeltaData.byWeaponType);
						pkBasic->SetLevel(rkSocietyDeltaData.sLevel);
					}
					++iCount;
				}
			}
		}
	}
	else if (ST_ENEMY == kMsg.m_uiSocietyType)
	{
		LC_EnemyInfo* pkEnemyInfo = pkPlayer->GetSocietyAsset()->GetEnemyInfo();
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_SocietyBasicInfoIndices::iterator it = pkEnemyInfo->GetObjects().begin();
		for (int i = 0; it != pkEnemyInfo->GetObjects().end() && i < MAX_ENEMY_NUM; ++i, ++it)
		{
			LC_SocietyBasicInfo* pkBasic = pkEnemyInfo->GetSocietyBasicInfo(*it);
			if (pkBasic)
			{
				LC_ServerPlayer* pkPlayerTemp = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(pkBasic->GetCharID());

				if (pkPlayerTemp)
				{
					if (iCount < 0 || iCount >= MAX_FRIEND_NUM)
					{
						GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "arr too big-6! count:%d", iCount);
						return;
					}
					PS_SocietyDeltaData& rkSocietyDeltaData = kRltMsg.m_kData[iCount];
					kRltMsg.m_alTargetCharID[iCount] = pkBasic->GetCharID();

					rkSocietyDeltaData.byCharType = static_cast < int8_t >(pkPlayerTemp->GetCharType()) ;
					rkSocietyDeltaData.byStatus = 1;
					rkSocietyDeltaData.byCareer = static_cast < int8_t >(pkPlayerTemp->GetCareerType());
					rkSocietyDeltaData.byWeaponType = static_cast <int8_t>(pkPlayerTemp->GetWeaponAttribute());
					rkSocietyDeltaData.sLevel = static_cast < int16_t >(pkPlayerTemp->GetLevel());
					rkSocietyDeltaData.sMapID = static_cast < int16_t >(pkPlayerTemp->GetMapResID());
					rkSocietyDeltaData.sArea = static_cast < int16_t >(pkPlayerTemp->GetCurrentLogicAreaID());
					rkSocietyDeltaData.iGroupStatus = (pkPlayerTemp->GetPlayerGroup() != NULL);
					rkSocietyDeltaData.uiDegree = pkPlayer->GetSocietyAsset()->GetDegree(pkBasic->GetCharID());

					//更新资产-----------------------
					pkBasic->SetCharType(rkSocietyDeltaData.byCharType);
					pkBasic->SetCareer(rkSocietyDeltaData.byCareer);
					pkBasic->SetWeaponType(rkSocietyDeltaData.byWeaponType);
					pkBasic->SetLevel(rkSocietyDeltaData.sLevel);
					//kInfo.SetSupremacyLevel(rkSocietyDeltaData.sSupremacyLevel);
					++iCount;
				}
			}
		}
	}
	kRltMsg.m_currentArraySize = iCount;
	pkPlayer->SendMsgToClient(MGPT_RLT_SOCIETY_DELTADATA, &kRltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_FRIENDNICK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddFriendNick msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_PlayerBase* pkTargetPlayer = pkPlayerManager->FindPlayer(msg.m_lTargetCharID);
	if (NULL == pkTargetPlayer)
	{
		// 离线好友能否评价，后续再考虑这个设计
		return;
	}

	ResultType iResult = pkTargetPlayer->GetFriendNick()->AddNick(msg.m_lSrcCharID, STDSTR2TPSTR(msg.m_szNick));
	MG_RLT_AddFriendNick rltMsg;
	rltMsg.m_iResult = iResult;
	if (RE_FRIEND_ADD_NICK_SUCCESS == iResult)
	{
		rltMsg.m_szNick = msg.m_szNick;
		rltMsg.m_lSrcCharID		= msg.m_lSrcCharID;
		rltMsg.m_lTargetCharID	= msg.m_lTargetCharID;
	}
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_ADD_FRIENDNICK, &rltMsg);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SUPPORT_FRIENDNICK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SupportFriendNick msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_PlayerBase* pkTargetPlayer = pkPlayerManager->FindPlayer(msg.m_lTargetFriendID);
	if (NULL == pkTargetPlayer)
	{
		return;
	}

	ResultType iResult = pkTargetPlayer->GetFriendNick()->Support(pkPlayer->GetInstance(), msg.m_lIndex);
	MG_RLT_SupportFriendNick rltMsg;
	rltMsg.m_iResult = iResult;
	if (RE_FRIEND_SUPPORT_NICK_SUCCESS == iResult)
	{
		rltMsg.m_lTargetFriendID = msg.m_lTargetFriendID;
		rltMsg.m_lIndex	= msg.m_lIndex;
	}
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_SUPPORT_FRIENDNICK, &rltMsg);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GETFRIENDNICK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GetFriendNick msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_PlayerBase* pkTargetPlayer = pkPlayerManager->FindPlayer(msg.m_lTargetFriendID);
	if (NULL == pkTargetPlayer)
	{
		// 离线好友能否评价，后续再考虑这个设计
		return;
	}
	MG_RLT_GetFriendNick rltMsg;
	rltMsg.m_lTargetFriendID = msg.m_lTargetFriendID;
	{
		LC_FriendNick* pkFriendNick = pkTargetPlayer->GetFriendNick();
		for (int i = 0; i < MAX_FRIEND_NICK_NUM; ++i)
		{
			LC_FriendNick::stNickInfo* pkInfo = pkFriendNick->GetNickInfo(i);
			if (pkInfo != NULL && pkInfo->szNick[0] != '\0')
			{
				rltMsg.m_kNickData[i].szNick = pkInfo->szNick;
				rltMsg.m_kNickData[i].iCount = pkInfo->iCount;
			}
		}
		pkPlayer->SendMsgToClient(MGPT_RLT_GETFRIENDNICK, &rltMsg);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_BEADDFRIEND, this_source_type, false)
{

}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_RLT_CONFIRM_BEADDFRIEND, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_Confirm_BeAddFriend kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (!kMsg.m_bAgree)
	{
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
		return;
	}
	ResultType iResultAddFriend = RE_SUCCESS;
	do
	{
		if (pkPlayer->GetInstance() == kMsg.m_lTargetCharID)
		{
			iResultAddFriend = RE_ADD_FRIEND_FAILED_CANT_SELF_ADD;
			break;
		}
		LC_FriendInfo* pkFriendInfo = pkPlayer->GetSocietyAsset()->GetFriendInfo();
		if (/*NULL == pkFriendInfo || */pkFriendInfo->IsExist(kMsg.m_lTargetCharID))
		{
			iResultAddFriend = RE_ADD_FRIEND_FAILED_IS_EXIST;
			break;
		}
		//LC_PlayerBase* pkTarPlayer = (SERVER_GET_PLAYER_MANAGER())->FindPlayer(kMsg.m_lPlayerID);
		{
			MG_SS_REQ_SetRelation reqMsg;
			char_data_define::friend_relation_info& relation = reqMsg.m_kRelation;
			relation.set_char_id(kMsg.m_lTargetCharID);
			pkPlayer->GetSocietyAsset()->FillDataToPB(relation);
			uint32_t nf = relation.flag() |  (1<<char_data_define::friend_relation_info_relation_type_FRIEND);
			relation.set_flag(nf);
			reqMsg.m_nServerID = GetGameServerEntryID();
			SendMsgToSociety(MGPT_GF_REQ_SET_RELATION, &reqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		}
		return;
	}while (0);

	MG_RLT_AddFriend rltMsg;
	rltMsg.m_iResultType = iResultAddFriend;
	rltMsg.m_szCharName = kMsg.m_szCharName;
	pkPlayer->SendMsgToClient(MGPT_RLT_ADD_FRIEND, &rltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_CHECK_PKEVENT, this_source_type, false)
{
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_CHANGE_SOCIETY_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Change_SocietyInfo xMsg;
	if (!xMsg.Decode(pMsgBody, usBodySize))
	{ 
		return; 
	}
	LC_ServerPlayerManager* pServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	switch (xMsg.m_cChangeType)
	{
		case SICT_Mood:
			{
				pkPlayer->SetCharMood(STDSTR2TPSTR(xMsg.m_szValue));
			}
			break;
		case SICT_Status:
			{
				pkPlayer->GetSocietyAsset()->SetStatus(uint32_t(xMsg.m_Value));
			}
			break;
		default:
			return;
	}
	MG_RLT_Change_SocietyInfo xChangeMsg;
	xChangeMsg.m_cChangeType = xMsg.m_cChangeType;
	xChangeMsg.m_lCharID = pkPlayer->GetInstance();
	xChangeMsg.m_Value = xMsg.m_Value;
	xChangeMsg.m_szValue = xMsg.m_szValue;
	// 通知所有关系
	LC_SocietyAsset* pPlayerSocietyAsset = pkPlayer->GetSocietyAsset();
	// 更新好友列表
	LC_FriendInfo* pPlayerFriendInfo = pPlayerSocietyAsset->GetFriendInfo();
	{
		FriendDetailInfoMap::iterator it = pPlayerFriendInfo->GetDetails().begin();
		for (int i = 0; it != pPlayerFriendInfo->GetDetails().end() && i < MAX_FRIEND_NUM; ++i, ++it)
		{
			LC_FriendDetailInfo& friendInfo = it->second;
			if (friendInfo.GetCharID() == pkPlayer->GetInstance())
			{ 
				continue; 
			}
			LC_ServerPlayer* pTargetPlayer = static_cast<LC_ServerPlayer*>(pServerPlayerManager->FindPlayer(friendInfo.GetCharID()));
			if (NULL == pTargetPlayer)
			{ 
				continue; 
			}
			LC_SocietyBasicInfo* pkBasic = friendInfo.GetBasic();
			if (NULL == pkBasic)
			{
				continue;
			}
			LC_SocietyBasicInfo kBaseInfo;
			kBaseInfo.SetCareer(pkBasic->GetCareer());
			kBaseInfo.SetCharType(pkBasic->GetCharType());
			kBaseInfo.SetStatus((int)xMsg.m_Value);
			kBaseInfo.SetLevel(pkBasic->GetLevel());
			kBaseInfo.SetWeaponType(pkBasic->GetWeaponType());
			kBaseInfo.SetCharMood(pkBasic->GetCharMood());
			kBaseInfo.SetPlatformInfo(pkBasic->GetPlatformInfo());
			kBaseInfo.SetBornCharType(pkBasic->GetBornCharType());
			pTargetPlayer->GetSocietyAsset()->UpdateSocietyBaseInfo(ST_FRIEND, kBaseInfo);
			pTargetPlayer->SendMsgToClient(MGPT_RLT_CHANGE_SOCIETY_INFO, &xChangeMsg);
		}
	}

	// 更新仇人列表
	LC_EnemyInfo* pPlayerEnemyInfo = pPlayerSocietyAsset->GetEnemyInfo();
	{
		LC_SocietyBasicInfoIndices::iterator it = pPlayerEnemyInfo->GetObjects().begin();
		for (int i = 0; it != pPlayerEnemyInfo->GetObjects().end() && i < MAX_ENEMY_NUM; ++i, ++it)
		{
			LC_SocietyBasicInfo* pkEnemyInfo = pPlayerEnemyInfo->GetSocietyBasicInfo(*it);
			if (NULL == pkEnemyInfo || pkEnemyInfo->GetCharID() == pkPlayer->GetInstance())
			{ 
				continue; 
			}
			LC_ServerPlayer* pTargetPlayer = static_cast<LC_ServerPlayer*>(pServerPlayerManager->FindPlayer(pkEnemyInfo->GetCharID()));
			if (NULL == pTargetPlayer)
			{ 
				continue; 
			}
			LC_SocietyBasicInfo kBaseInfo;
			kBaseInfo.SetCareer(pkEnemyInfo->GetCareer());
			kBaseInfo.SetCharType(pkEnemyInfo->GetCharType());
			kBaseInfo.SetStatus((int)xMsg.m_Value);
			kBaseInfo.SetLevel(pkEnemyInfo->GetLevel());
			kBaseInfo.SetWeaponType(pkEnemyInfo->GetWeaponType());
			kBaseInfo.SetCharMood(pkPlayer->GetCharMood());
			kBaseInfo.SetPlatformInfo(pkEnemyInfo->GetPlatformInfo());
			kBaseInfo.SetBornCharType(pkEnemyInfo->GetBornCharType());
			pTargetPlayer->GetSocietyAsset()->UpdateSocietyBaseInfo(ST_ENEMY, kBaseInfo);
			pTargetPlayer->SendMsgToClient(MGPT_RLT_CHANGE_SOCIETY_INFO, &xChangeMsg);
		}
	}

	// 更新黑名单列表
	LC_BlackListInfo* pPlayerBlacklistInfo = pPlayerSocietyAsset->GetBlackListInfo();
	{
		LC_SocietyBasicInfoIndices::iterator it = pPlayerBlacklistInfo->GetObjects().begin();
		for (int i = 0; it != pPlayerBlacklistInfo->GetObjects().end() && i < MAX_BLACKLIST_NUM; ++i, ++it)
		{
			LC_SocietyBasicInfo* pkBlackInfo = pPlayerBlacklistInfo->GetSocietyBasicInfo(*it);
			if (NULL == pkBlackInfo || pkBlackInfo->GetCharID() == pkPlayer->GetInstance())
			{ 
				continue; 
			}
			LC_ServerPlayer* pTargetPlayer = static_cast<LC_ServerPlayer*>(pServerPlayerManager->FindPlayer(pkBlackInfo->GetCharID()));
			if (NULL == pTargetPlayer)
			{ 
				continue; 
			}
			LC_SocietyBasicInfo kBaseInfo;
			kBaseInfo.SetCareer(pkBlackInfo->GetCareer());
			kBaseInfo.SetCharType(pkBlackInfo->GetCharType());
			kBaseInfo.SetStatus((int)xMsg.m_Value);
			kBaseInfo.SetLevel(pkBlackInfo->GetLevel());
			kBaseInfo.SetWeaponType(pkBlackInfo->GetWeaponType());
			kBaseInfo.SetCharMood(pkPlayer->GetCharMood());
			kBaseInfo.SetPlatformInfo(pkBlackInfo->GetPlatformInfo());
			kBaseInfo.SetBornCharType(pkBlackInfo->GetBornCharType());
			pTargetPlayer->GetSocietyAsset()->UpdateSocietyBaseInfo(ST_BLACKLIST, kBaseInfo);
			pTargetPlayer->SendMsgToClient(MGPT_RLT_CHANGE_SOCIETY_INFO, &xChangeMsg);
		}
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GET_PLAYER_NEARBY, this_source_type, false)
{
	MG_REQ_GetPlayerNearby kMsg;
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

	MG_RLT_GetPlayerNearby kRltMsg;
	uint32_t ip = pkPlayer->GetLoginIP();
	if (ip)
	{
		const object_id_set& idset = pkPlayerManager->GetIDSetAcrdLoginIP(ip);
		object_id_vector idlist;
		idlist.insert(idlist.end(), idset.begin(), idset.end());
		std::random_shuffle(idlist.begin(), idlist.end());

		object_id_vector::iterator it = idlist.begin();
		for (int i = 0; it != idlist.end() && i < MAX_NEARBY_NUM; ++it)
		{
			LC_ServerPlayer* pTargetPlayer = static_cast<LC_ServerPlayer*>(pkPlayerManager->FindPlayer(*it));
			if (NULL == pTargetPlayer)
			{ 
				continue; 
			}

			if (pTargetPlayer->GetInstance() == rkMsgHead.m_uiCharID)
			{
				continue;
			}

			PS_SocietyBasicInfo kBaseInfo;
			kBaseInfo.iCareer = pTargetPlayer->GetCareerType();
			kBaseInfo.iCharType = pTargetPlayer->GetCharType();
			kBaseInfo.uiCharID = pTargetPlayer->GetInstance();
			kBaseInfo.iBornCharType = pTargetPlayer->GetProfilePictureID();
			//kBaseInfo.iStatus = 1;
			kBaseInfo.iStatus = pTargetPlayer->GetSocietyAsset()->GetStatus();
			TPSTR2STDSTR(pTargetPlayer->GetOwnerCharName(), kBaseInfo.szCharName);
			kBaseInfo.iLevel = pTargetPlayer->GetLevel();
			kBaseInfo.iWeaponType = pTargetPlayer->GetWeaponAttribute();
			kBaseInfo.iScore = pTargetPlayer->GetControllerCombatScore();
			kBaseInfo.uiDegree = pkPlayer->GetSocietyAsset()->GetDegree(pTargetPlayer->GetInstance());
			kBaseInfo.beFriendWithEachOther = 0;//局域网这个字段不用，直接写成0
			const platform_vip_info& pf_info = pTargetPlayer->GetPlatformVipInfo();
			for(platform_vip_info::const_iterator iter = pf_info.begin(); iter != pf_info.end(); iter++)
			{
				kBaseInfo.kPlatform[iter->first.c_str()] =  iter->second.format_vip_info();
			}
			kRltMsg.m_kPlayerList.push_back(kBaseInfo);
			++i;
		}
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_PLAYER_NEARBY, &kRltMsg);
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SYNC_SUMMOR_POOL, this_source_type, false)
{
	
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_AS_PROMOTEE_OPERATION_REQ, this_source_type, false)
{
	MG_AsPromoteeOperationREQ kMsg;
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

	uint32_t rslt = RE_SUCCESS;
	LC_PromoterAsset& kPromoteAsset = pkPlayer->GetPromoterAsset();
	AsPromoteeInfo& kAsPromotee = kPromoteAsset.GetAsPromoteeInfo();

	switch (kMsg.m_uiOpCode)
	{
		case PS_AS_PRO_OP_SET:
			{
				if (kAsPromotee.m_uiID != 0)
				{
					rslt = RE_PROMOTE_PROMOTER_ALREADY_SET;
					break;
				}
			}
			break;
		case PS_AS_PRO_OP_CONSUME:
			{
				rslt = RE_FAIL;
			}
			break;
		case PS_AS_PRO_OP_REWARD:
			{
				if (kAsPromotee.m_uiID == 0)
				{
					rslt = RE_PROMOTE_PROMOTER_NOT_SET;
					break;
				}
				if (kAsPromotee.IsLevelRewarded(kMsg.m_uiParamA))
				{
					rslt = RE_PROMOTE_AS_PROMOTEE_ALREADY_REWARD;
					break;
				}
			}
			break;
	}
	if (RE_SUCCESS != rslt)
	{
		MG_AsPromoteeOperationRESP kResp;
		kResp.m_uiOpCode = kMsg.m_uiOpCode;
		kResp.m_uiParamA = kMsg.m_uiParamA;
		kResp.m_uiParamB = kMsg.m_uiParamB;
		kResp.m_uiRslt = rslt;
		pkPlayer->SendMsgToClient(MGPT_AS_PROMOTEE_OPERATION_RESP, &kResp);
	}
	else
	{
		MG_SS_SetAsPromoteeInfoReq kReqMsg;
		kReqMsg.m_uiOpCode = kMsg.m_uiOpCode;
		kReqMsg.m_uiParamA = kMsg.m_uiParamA;
		kReqMsg.m_uiParamB = kMsg.m_uiParamB;
		SendMsgToSociety(MGPT_SS_PROMOTE_SET_AS_PROMOTEE_INFO_REQ, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_MY_PROMOTEE_OPERATION_REQ, this_source_type, false)
{
	MG_MyPromoteeOperationREQ kMsg;
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
	uint32_t rslt = RE_SUCCESS;
	LC_PromoterAsset& kPromoteAsset = pkPlayer->GetPromoterAsset();
	MyPromotees& kMyPromotees = kPromoteAsset.GetMyPromoteesInfo();
	if (!kMyPromotees.IsExists(kMsg.m_uiCharID))
	{
		rslt = RE_PROMOTE_NOT_MY_PROMOTEE;
	}

	if (RE_SUCCESS != rslt)
	{
		MG_MyPromoteeOperationRESP kResp;
		kResp.m_uiCharID = kMsg.m_uiCharID;
		kResp.m_uiOpCode = kMsg.m_uiOpCode;
		kResp.m_uiParamA = kMsg.m_uiParamA;
		kResp.m_uiParamB = kMsg.m_uiParamB;
		kResp.m_uiRslt = rslt;
		pkPlayer->SendMsgToClient(MGPT_MY_PROMOTEE_OPERATION_RESP, &kResp);
	}
	else
	{
		MG_SS_SetCorrePromoterInfoReq kReqMsg;
		kReqMsg.m_uiCharID = kMsg.m_uiCharID;
		kReqMsg.m_uiOpCode = kMsg.m_uiOpCode;
		kReqMsg.m_uiParamA = kMsg.m_uiParamA;
		kReqMsg.m_uiParamB = kMsg.m_uiParamB;
		SendMsgToSociety(MGPT_SS_PROMOTE_SET_CORRE_PROMOTER_INFO_REQ, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_MY_PROMOTEES_INFO_REQ, this_source_type, false)
{
	MG_MyPromoteesInfoREQ kMsg;
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

	LC_PromoterAsset& kPromoteAsset = pkPlayer->GetPromoterAsset();
	MyPromotees& kMyPromotees = kPromoteAsset.GetMyPromoteesInfo();
	uint32_t idx = kMsg.m_uiPageIndex;
	uint32_t granu = kMsg.m_uiPageGranu;
	if (granu > MAX_MY_PROMOTEES_PAGE_GRANU)
	{
		granu = MAX_MY_PROMOTEES_PAGE_GRANU;
	}
	granu = UT_MathBase::MaxInt(granu, 1);
	uint32_t count = (kMyPromotees.m_uiCount + granu - 1) / granu;
	if (count <= 0)
	{
		idx = 0;
		MG_MyPromoteesInfoRESP kResp;
		kResp.m_uiPageIndex = idx;
		kResp.m_uiPageGranu = granu;
		kResp.m_uiPageCount = count;
		pkPlayer->SendMsgToClient(MGPT_MY_PROMOTEES_INFO_RESP, &kResp);
	}
	else
	{
		idx = UT_MathBase::MinInt(count - 1, idx);
		MG_SS_PromoteeListReq kReqMsg;
		kReqMsg.m_uiPageIndex = idx;
		kReqMsg.m_uiPageGranu = granu;
		kReqMsg.m_uiPageCount = count;
		uint32_t off = UT_MathBase::MinInt(idx * granu, (int)kMyPromotees.m_kPromotees.size());
		PromoteesList::iterator it = kMyPromotees.m_kPromotees.begin() + off;
		for (uint32_t i = 0; it != kMyPromotees.m_kPromotees.end() && i < granu; ++it, ++i)
		{
			kReqMsg.m_kPromoteeIDs.push_back(*it);
		}
		SendMsgToSociety(MGPT_SS_PROMOTE_PROMOTEE_LIST_REQ, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_PROMOTER_OPERATION_REQ, this_source_type, false)
{
	MG_PromoteOperationREQ kMsg;
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

	MG_PromoteOperationRESP kResp;
	kResp.m_uiOpCode = kMsg.m_uiOpCode;
	kResp.m_uiParamA = kMsg.m_uiParamA;
	kResp.m_uiParamB = kMsg.m_uiParamB;
	LC_PromoterAsset& kPromoteAsset = pkPlayer->GetPromoterAsset();

	switch (kMsg.m_uiOpCode)
	{
		case PS_PRO_OP_DAILY_SHARE:
			{
				kPromoteAsset.SetFlag(1 << CPFA_DAILY_SHARE, 1 << CPFA_DAILY_SHARE);
				kPromoteAsset.UpdateDirtyFlag();
			}
			break;
		case PS_PRO_OP_DAILY_SHARE_REWARD:
			{
				if (!kPromoteAsset.IsDailyShared())
				{
					kResp.m_uiRslt = RE_PROMOTE_DAILY_SHARE_ERR;
					break;
				}
				if (kPromoteAsset.IsDailySharedReward())
				{
					kResp.m_uiRslt = RE_PROMOTE_DAILY_SHARE_REWARD;
					break;
				}
				pkPlayer->HandlePromoteReward(CPRT_PROMOTER_DAILY);
				//do reward
				kPromoteAsset.SetFlag(1 << CPFA_DAILY_SHARE_REWARD, 1 << CPFA_DAILY_SHARE_REWARD);
				kPromoteAsset.UpdateDirtyFlag();
			}
			break;
		case PS_PRO_OP_DRAW_RAKE_OFF_A:
			{
				MyPromotees& kMyPromotees = kPromoteAsset.GetMyPromoteesInfo();
				if (kPromoteAsset.RakeOffA() >= kMyPromotees.m_uiRakeOffA)
				{
					kResp.m_uiRslt = RE_PROMOTE_RAKE_OFF_A_REWARD;
					break;
				}
				MoneyType iDelta = (MoneyType)(kMyPromotees.m_uiRakeOffA - kPromoteAsset.RakeOffA());
				kPromoteAsset.SetRakeOffA(kPromoteAsset.RakeOffA()  + iDelta);
				kPromoteAsset.UpdateDirtyFlag();
				pkPlayer->AddCash(iDelta, CT_UNBIND_YUANBAO, SYS_FUNC_TP_PROMOTE, CPRT_CORRE_PROMOTER);
			}
			break;
		default:
			break;
	}
	pkPlayer->SendMsgToClient(MGPT_PROMOTER_OPERATION_RESP, &kResp);
}

ON_HANDLE(MGPT_REQ_GET_USER_ONLINE_INFO, this_source_type, false)
{
	MG_REQ_UserOnlineInfo kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToSociety(MGPT_GF_REQ_USER_ONLINE_INFO, &kMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

}
