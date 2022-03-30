#include "HL_PacketHandler.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_SequenceManager.h"
#include "NW_ServerSession.h"
#include "NW_Helper.h"

#include "GameServerApp.h"
#include "oss_360_define.h"
#include "CF_LevelUp.h"
#include "CF_CreateCharParam.h"
#include "LC_ServerPlayerManager.h"
#include "GameServerApp.h"
#include "UT_ServerHelper.h"
#include "LC_PackEntryFactory.h"
#include "TS_TranManager.h"
#include "TS_TranType.h"
#include "LC_RankManager.h"
#include "iostream_color.h"
#include "oss_define.h"
#include "LC_ItemFactoryBase.h"
#include "ServerCommDef.h"

#include "MG_Stream.h"
#include "MG_Common.h"
#include "MG_PB.h"
#include "MG_DatabaseStruct.h"
#include "MG_GMTool.h"
#include "MG_Tools.h"
#include "MG_CS_CurrencyTradeProtocol.h"
#include "MG_Profile.h"
#include "MG_LoginProtocol.h"
#include "GlobalSettings.h"
#include "LC_GameStory_Manager.h"
#include "LC_GameStoryBase.h"
#include "LC_GameStoryDataDefine.h"
#include "MG_Game2Login.h"
using namespace Protocol;
using namespace GameLogic;
using namespace Utility;

namespace Protocol
{
#define this_source_type SERVER_TYPE_LOGIN

ON_HANDLE(MGPT_GP_RLT_ROLE_PROFILE_INFO, this_source_type, false)
{
	MG_RLT_Char_Profile kMsg;
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

	if (kMsg.m_iResult == RE_SUCCESS)
	{
		GameLogic::LC_SocietyBasicInfo kBaseInfo;

		kBaseInfo.SetCareer(kMsg.m_byCareer);
		kBaseInfo.SetCharID(kMsg.m_uiCharID);
		kBaseInfo.SetCharType(kMsg.m_byCharType);
		kBaseInfo.SetLevel(kMsg.m_usCharLevel);
		kBaseInfo.SetWeaponType(kMsg.m_byWeapon);
		kBaseInfo.SetCharName(kMsg.m_szCharName.c_str());
		pkPlayer->UpdateSocietyAsset(kBaseInfo);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_CHAR_PROFILE, &kMsg);
}

//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_SWITCH_MAP_PREPARE_PLAYER, this_source_type, false)
{
	MG_SS_REQ_Transfer_PreparePlayer kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (kMsgBody.m_iDstServerID != UT_ServerHelper::GetServerId())
		return;

	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_ServerPlayerEnterLeaveManager::PlayerPrepareSwitchLineArg	arg(kMsgBody.m_eInOutType, kMsgBody.m_iDstMapResID, kMsgBody.m_lDstMapLogicID);
	bool bReqSuccess = enterLeaveManager->AddSwitchMapLinePlayer( rkMsgHead.m_uiUserID, arg );

	/// 回复Login，是否成功为该玩家做好切线准备
	MG_SS_RLT_Transfer_PreparePlayer	kRltBody;
	kRltBody.m_iResult = bReqSuccess ? RE_SUCCESS : RE_FAIL;
	kRltBody.m_eInOutType = kMsgBody.m_eInOutType;
	kRltBody.m_iSrcServerID = kMsgBody.m_iSrcServerID;
	kRltBody.m_iDstServerID = kMsgBody.m_iDstServerID;
	kRltBody.m_iDstMapResID = kMsgBody.m_iDstMapResID;
	kRltBody.m_lDstMapLogicID = kMsgBody.m_lDstMapLogicID;
	kRltBody.m_iPlayerGuildID = kMsgBody.m_iPlayerGuildID;

	SendMsgToLogin(MGPT_SS_SWITCH_MAP_PREPARE_PLAYER_RESULT, &kRltBody, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_SS_MAP_LINE_RESULT, this_source_type, false)
{
	const unique_id_impl& uiCharID = rkMsgHead.m_uiCharID;
	MG_SS_MAP_LINE_RESULT kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkWorldManager->SetGlobalMapLogicId_Lite( kMsgBody.nSrcNodeId, kMsgBody.nOrigLogicId, kMsgBody.nMapResId, kMsgBody.nGlobalLineIndex);
	return;
}

ON_HANDLE(MGPT_GL_REQ_UPDATE_ACCOUNT_DETAIL_INFO, this_source_type, false)
{
	const unique_id_impl& uiCharID = rkMsgHead.m_uiCharID;
	MG_GL_User_Account_Info kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(uiCharID);
	if (NULL == pkPlayer)
	{
		return;
	}
	//商城VIP经验点数
	//pkPlayer->SetBindPhone(kMsgBody.m_bBindPhone > 0);
	//pkPlayer->SetAdult(kMsgBody.m_byAdult > 0);
	//pkPlayer->SetFreezeAcc(kMsgBody.m_tmFreezeAccount);
	//pkPlayer->SetFreezeRol(kMsgBody.m_tmFreezeRole);

	//pkPlayer->setUserUUID(kMsgBody.m_userKey);

	if (kMsgBody.m_userKey > 0 && pkPlayer->getUserUUID() != kMsgBody.m_userKey)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "UPDATE_ACCOUNT_DETAIL_INFO userKey error, login:%lld--local:%lld ", kMsgBody.m_userKey, pkPlayer->getUserUUID());
	}

	if (pkPlayer->PropellID() < kMsgBody.m_uiCharPropellID)
	{
		pkPlayer->SetPropellID(kMsgBody.m_uiCharPropellID);
	}
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GMTOOL2_MUTE_USER, this_source_type, false)
{
	MG_SS_MuteUser kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	for ( int i = 0; i < kMsgBody.m_nUserIds.size(); i++ )
	{
		if ( kMsgBody.m_nUserIds[i] == 0 )
			continue;
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kMsgBody.m_nUserIds[i]);
		if ( pkPlayer == NULL )
			continue;
		// Try to mute
		if ( kMsgBody.m_nOPType == 1 )
			pkPlayer->CheckGagTime( kMsgBody.m_nTime );
		else if ( kMsgBody.m_nOPType == 2 )
			pkPlayer->CheckGagTime( 0 );
	}
}

ON_HANDLE(MGPT_SS_KICK_PLAYER, this_source_type, false)
{
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	MG_SS_RespKickUser respKick;
	respKick.uResult = 0;
	MG_SS_KickUser toGS_Kick;
	if (!toGS_Kick.Decode(pMsgBody, usBodySize))
	{	
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR," MGPT_SS_KICK_PLAYER Decode error uiCharID[ ",rkMsgHead.m_uiCharID,"]userid[",rkMsgHead.m_uiUserID,"]" );
		respKick.uResult = 1;
		SendMsgToLogin(MGPT_SS_RESP_KICK_PLAYER, &respKick, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, iSequence);
		return;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	if ( rkMsgHead.m_uiCharID == 0 && rkMsgHead.m_uiUserID == 0)
	{
		// KickAll
		mem::vector<LC_ServerPlayer*> vecPlayer;
		const GameLogic::LC_PlayerUIDPtrMap&	kPlayerUIDPtrMap = pkPlayerManager->GetPlayerUIDPtrMap();
		GameLogic::LC_PlayerUIDPtrMap::const_iterator it = kPlayerUIDPtrMap.begin();
		for ( ;it != kPlayerUIDPtrMap.end(); ++it )
		{
			LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)(it->second);
			if ( pPlayer == NULL )
				continue;
			vecPlayer.push_back(pPlayer);
		}

		for ( int i = 0; i < vecPlayer.size(); i++ )
		{
			if ( vecPlayer[i] == NULL )
				continue;
			enterLeaveManager->UserLeaveServer(vecPlayer[i], LMIOT_BEKICKOFFED, toGS_Kick.m_nReason);
		}
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO," MGPT_SS_KICK_PLAYER login kick all success  m_nReason[",toGS_Kick.m_nReason,"]" );
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (NULL == pkPlayer)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR," MGPT_SS_KICK_PLAYER NULL == pkPlayer uiCharID[ ",rkMsgHead.m_uiCharID,"]userid[",rkMsgHead.m_uiUserID,"]m_nReason[",toGS_Kick.m_nReason,"]" );
		respKick.uResult = 2;
		SendMsgToLogin(MGPT_SS_RESP_KICK_PLAYER, &respKick, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, iSequence);
		return;
	}
	
	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if (pOwner == NULL)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR," MGPT_SS_KICK_PLAYER NULL == pOwner uiCharID[ ",rkMsgHead.m_uiCharID,"]userid[",rkMsgHead.m_uiUserID,"]m_nReason[",toGS_Kick.m_nReason,"]" );
		respKick.uResult = 2;
		SendMsgToLogin(MGPT_SS_RESP_KICK_PLAYER, &respKick, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, iSequence);
		return;
	}
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
	pOwner->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
	for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
	{
		LC_ServerPlayer* pLeaveBuddy = vecBuddyPtrPlayer[i];
		IF_A_NA_PLAYER(pLeaveBuddy)
				continue;
		enterLeaveManager->UserLeaveServer(pLeaveBuddy,LMIOT_BEKICKOFFED, toGS_Kick.m_nReason);
	}

	SendMsgToLogin(MGPT_SS_RESP_KICK_PLAYER, &respKick, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, iSequence);
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO," MGPT_SS_KICK_PLAYER login kick success  id[",pkPlayer->GetID(),"] uiCharID[ ",rkMsgHead.m_uiCharID,"]userid[",rkMsgHead.m_uiUserID,"]m_nReason[",toGS_Kick.m_nReason,"]" );
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GL_REQ_BROADCAST_MESSAGE, this_source_type, false)
{
	MG_GL_Broadcast_Message kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_SystemAnnounce kMsg;
	if (kMsgBody.m_szMessage.size() > MAX_CHAT_MESSAGE_SIZE)
	{
		return;
	}
	kMsg.m_szSystemAnnounce = kMsgBody.m_szMessage;
	GetGameServerApp()->BroadCastMsgToClient(MGPT_SYSTEM_ANNOUNCE, &kMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_TRANSFER_DIFMAP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_SS_RLT_Transfer_DifMap reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	int currServerId = UT_ServerHelper::GetServerId();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_SS_RLT_TRANSFER_DIFMAP transfer currServerId:[%d] dstServerId:[%d] c_cid:[%d] c_citizenship:[%d] c_uid:[%d] srcMapId:[%d] dstMapId:[%d]", 
		currServerId,reqMsg.m_iDstServerID, pkPlayer->GetInstance().detail(), pkPlayer->GetInstance().catagory(), pkPlayer->GetUserID(), reqMsg.m_lSrcMapResID, reqMsg.m_lDstMapResID);
	
	if (reqMsg.m_iDstServerID == 0 || reqMsg.m_lDstMapResID == 0)
		return;

	if(INVALID_GUILD_ID != reqMsg.m_iPlayerGuildID)
	{
		pkPlayer->SetGuildID(reqMsg.m_iPlayerGuildID);
	}
	
	if(reqMsg.m_iDstServerID == currServerId)
	{
		// 要进入本服，角色原先就在本服上，或者刚被本服加载完毕
		// 注意 RequestChangePlayerLocation 最后的参数为 true；避免消息在GS和login中来回循环
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		UT_Vec3Int	kDstPos(reqMsg.m_iDstMapPosX, reqMsg.m_iDstMapPosY, reqMsg.m_iDstMapPosZ);
		pkWorldManager->RequestChangePlayerLocation(pkPlayer, (GameLogic::LC_MapInOutType)reqMsg.m_eInOutType, reqMsg.m_lDstMapLogicID, reqMsg.m_lDstMapResID, kDstPos, 0, true);
	}
	else
	{
		// 角色在本服上，要进入其他服务器，先下线
		LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
		if (pOwner == NULL)
			return;
		if(false == GetGameServerApp()->IsNodeType_Global())
			pOwner->UpdateWorldLocationAssetData();
		
		pOwner->SaveCharPB(true,RE_SWITCH_MAP_LINE_SAVE);
		pOwner->SetEnterServerMode( ENTER_SERVER_MODE::eEnterServerMode_SwitchNode );
		std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
		pOwner->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
		for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
		{
			LC_ServerPlayer* pLeaveBuddy = vecBuddyPtrPlayer[i];
			IF_A_NA_PLAYER(pLeaveBuddy)
				continue;
			enterLeaveManager->UserLeaveServer(pLeaveBuddy, LMIOT_RESET,true,false);//bSendlogin 设置为false  必须是 LMIOT_RESET，等db 回复后 RE_SWITCH_MAP_LINE_SAVE 给login发 MGPT_SS_ROLE_LEAVE_GAME
		}
	}
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_RESP_TRANSFER_DETAIL, this_source_type, false)
{
	const unique_id_impl& uiCharID = rkMsgHead.m_uiCharID;
	MG_SS_TransferDetail_RESP kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = (LC_ServerPlayerManager*)LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(kMsgBody.nPlayerId);
	if ( pPlayer == NULL )
		return;

	MG_RESP_TransferDetail respMsg;

	int nResultSize = kMsgBody.m_kDetails.size();
	//GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_SYSTEM, "fenxian, recv login MGPT_RESP_TRANSFER_DETAIL send to client, playerId=%d, nResultSize=%d", kMsgBody.nPlayerId, nResultSize);
	if ( nResultSize > 0 )
	{
		respMsg.m_kDetails.resize(nResultSize);
		for (size_t i = 0; i < nResultSize; ++i)
		{
			PS_MapDetail& kDst = respMsg.m_kDetails[i];
			PS_MapDetail& kSrc = kMsgBody.m_kDetails[i];

			kDst = kSrc;
		}
	}
	else
	{
		// TODO 需要策划给出 本地有分线 但是不上报login统一管理(切节点) 需要策划给这种类型的地图 一个标记位。当不全局管理的分线，在本地拿。
		// if ( pMap && pkWorldManager->_checkAllowGlobalMapLine(pMap,pMap->_iMapLogicType) ) 修改此处判断条件
		CF_WorldMapList::DataEntry* pMap = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(kMsgBody.nMapResId);
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		if ( pMap && pkWorldManager->_checkAllowGlobalMapLine(pMap,pMap->_iMapLogicType) )
		{
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			LC_ServerMapLogicPlaceVector* pkPlaceVec = pkWorldManager->GetMapLogicByRes(kMsgBody.nMapResId);
			if (pkPlaceVec)
			{
				int32_t nCurGSNodeId = GetGameServerApp()->GetServerInfo().m_iServerID;
				respMsg.m_kDetails.resize(pkPlaceVec->size());
				for (size_t i = 0; i < pkPlaceVec->size(); ++i)
				{
					PS_MapDetail& kData = respMsg.m_kDetails[i];
					LC_ServerMapLogic_Place* mapLogic = (*pkPlaceVec)[i];
					kData.m_iGSNodeID = nCurGSNodeId;
					kData.m_iResID = mapLogic->GetMapResID();
					kData.m_iLogicID = mapLogic->GetMapLogicID();
					kData.m_uiPlayerCount = mapLogic->GetControllerPlayerCount();
					kData.m_iLineNum = mapLogic->GetMap()->GetLocalLineNum();
				}
			}
		}
	}

	pPlayer->SendMsgToClient(MGPT_RESP_TRANSFER_DETAIL, &respMsg);

	return;
}

ON_HANDLE(MGPT_GL_RLT_GET_ACCOUNT_DETAIL_INFO, this_source_type, false)
{
	int32_t uServerId = UT_ServerHelper::GetServerId();
	const unique_id_impl& uiCharID = rkMsgHead.m_uiCharID;
	uint32_t userId = rkMsgHead.m_uiUserID;
	MG_GL_User_Account_Info kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "<%u>handle MGPT_GL_RLT_GET_ACCOUNT_DETAIL_INFO, userID:%d CharID:%lld",uServerId ,userId, uiCharID);
		return;
	}
	GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "<enter:%u><c_citizenship:%u>handle MGPT_GL_RLT_GET_ACCOUNT_DETAIL_INFO,<c_cid:%d><userKey:%lld><szAccount:%s><cRoleCount:%d>", 
		uServerId,uiCharID.catagory(),userId, kMsgBody.m_userKey, kMsgBody.m_szAccount.c_str(), kMsgBody.m_cRoleCount);

	LC_ServerPlayerManager* pkPlayerManager = (LC_ServerPlayerManager*)LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(uiCharID);
	
	if (NULL == pkPlayer)
	{
		LC_UserAccountInfoCache& kCacheInfo = enterLeaveManager->GetUserInfoCache(uiCharID);
		kCacheInfo.m_cRoleCount = kMsgBody.m_cRoleCount;
		kCacheInfo.m_uiLoginIP = kMsgBody.m_uiLoginIP;
		kCacheInfo.m_uiLastLoginIP = kMsgBody.m_uiLastLoginIP;
		kCacheInfo.m_ulClientMAC = kMsgBody.m_ulClientMAC;
		STRNCPY(kCacheInfo.m_szAccount, kMsgBody.m_szAccount.c_str(), kMsgBody.m_szAccount.length() + 1);
		kCacheInfo.m_byAdult = kMsgBody.m_byAdult;
		kCacheInfo.m_offlineTime = kMsgBody.m_offlineTime;
		kCacheInfo.m_onlineTime = kMsgBody.m_onlineTime;
		kCacheInfo.m_bBindPhone = kMsgBody.m_bBindPhone;
		kCacheInfo.m_iLoginTime = kMsgBody.m_iLoginTime;
		kCacheInfo.m_uiCharPropellID = kMsgBody.m_uiCharPropellID;
		kCacheInfo.m_userKey = kMsgBody.m_userKey;
		kCacheInfo.m_fTimestamp = GET_PROCESS_TIME;
		kCacheInfo.m_nLoginPlatform = kMsgBody.m_nLoginPlatform;
		kCacheInfo.m_nLoginPlatformPage = kMsgBody.m_nLoginPlatformPage;
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " cache accout data will send db to login 1111 ");
		return;
	}
	pkPlayerManager->OnPlayerLoginIPSet(pkPlayer, pkPlayer->GetLoginIP(), kMsgBody.m_uiLoginIP);
	pkPlayer->SetLoginIP(kMsgBody.m_uiLoginIP);
	pkPlayer->SetClientIP(kMsgBody.m_uiLastLoginIP);
	pkPlayer->SetClientMAC(kMsgBody.m_ulClientMAC);
	pkPlayer->SetUserName(STDSTR2TPSTR(kMsgBody.m_szAccount));

	if (pkPlayer->PropellID() < kMsgBody.m_uiCharPropellID)
	{
		pkPlayer->SetPropellID(kMsgBody.m_uiCharPropellID);
	}
	pkPlayer->setUserUUID(kMsgBody.m_userKey);
	pkPlayer->GetPlayerPlatformInfoAsset().SetPlatformType(kMsgBody.m_nLoginPlatform);
	pkPlayer->GetPlayerPlatformInfoAsset().SetLoginPlatformPage(kMsgBody.m_nLoginPlatformPage);
	enterLeaveManager->LoadDataFromLogin(pkPlayer);

	pkPlayer->SetAccountLoginTime(kMsgBody.m_iLoginTime);
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " will send db to login 2222 ");
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GL_REQ_CREATE_CHAR, this_source_type, false)
{
	MG_GL_Req_CreateChar kMsgBody;
	if (!kMsgBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_GL_Rlt_CreateChar kRltBody;
	kRltBody.Info.Info.m_byCharType		= (uint8_t)kMsgBody.m_bySex;
	kRltBody.Info.Info.m_byCareer		= (uint8_t)kMsgBody.m_byCareer;
	kRltBody.Info.Info.m_byWeapon		= (uint8_t)kMsgBody.m_byWeaponType;
	kRltBody.Info.Info.m_usCharLevel = 1;
	STRNCPY(kRltBody.Info.Info.m_szCharName, kMsgBody.m_szCharName.c_str(), sizeof(kRltBody.Info.Info.m_szCharName));
	kRltBody.Info.Info.m_iCitizenship   = kMsgBody.m_iCitizenship;
	kRltBody.m_iResult = RE_LOGIN_CREATE_CHAR_SUCCESS;

	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	if (!pkServerMgr->IsServerOpened())
	{
		kRltBody.m_iResult = RE_LOGIN_FAIL_WAIT_SERVER_OPEN;
	}
	else
	{
		int32_t nBuddyId = kRltBody.Info.Info.m_byCharType;
		CSVFile::CF_CreateCharParam::DataEntry* entry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_CreateCharParam>(nBuddyId, false, true);
		if (NULL == entry)
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CREATE_CHAR FAILED, UNKOWN WEAPON TTPE", kMsgBody.m_byWeaponType);
			kRltBody.m_iResult = RE_LOGIN_CREATE_CHAR_FAIL;
		}
	}
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	SendMsgToLogin(MGPT_GL_RLT_CREATE_CHAR, &kRltBody, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, iSequence);
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_DEPOSIT_MONEY, this_source_type, false)
{
	MG_RLT_Deposit_Money kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (RE_SUCCESS != kRltBody.m_iResult)
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_Money& rkYuanBao = pkPlayer->GetUnBindYuanBao();
	rkYuanBao.SetMoney(kRltBody.m_llYuanBao);

	//通知客户端，转出YuanBao成功
	MG_RLT_CurrencyYuanBaoDraw kRltMsg;
	kRltMsg.m_iResult = kRltBody.m_iResult;
	kRltMsg.m_llYuanBaoCount	= kRltBody.m_llDeltaYuanBao;
	kRltMsg.m_llResultYuanBao	= kRltBody.m_llYuanBao;
	pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_YUANBAO_DRAW, &kRltMsg);
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_DRAW_MONEY, this_source_type, false)
{
	MG_RLT_Draw_Money kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (RE_SUCCESS != kRltBody.m_iResult)
	{
		return;
	}

	MG_GG_REQ_DepositYuanbao kReqMsg;
	kReqMsg.m_llTransYuanBao = -kRltBody.m_llDeltaYuanBao;
	STRNCPY(kReqMsg.m_szBillingNO, kRltBody.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO));
	SendMsgToTrade(MGPT_SS_REQ_DEPOSITYUANBAO, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_Money& rkYuanBao = pkPlayer->GetUnBindYuanBao();
	rkYuanBao.SetMoney(kRltBody.m_llYuanBao);
}

ON_HANDLE(MGPT_GL_RLT_CHAR_PROPELL_INFO, this_source_type, false)
{
	MG_GL_RLT_CharPropellInfo kRltBody;
	if (!kRltBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	//do charactor propel info handle
	PropellInfoList propells;
	std::vector<PS_PropellInfo>::iterator it = kRltBody.m_kPropellInfos.begin();
	for (; it != kRltBody.m_kPropellInfos.end(); ++it)
	{
		PropellInfo propellIF;
		propellIF.m_uiID = it->m_uiID;
		propellIF.m_uiFlag = it->m_uiFlag;
		propellIF.m_sInfo.append(it->m_sInfo.c_str(), it->m_sInfo.size());

		propells.push_back(propellIF);
	}
	pkPlayer->SyncPropellInfo(propells);
}

ON_HANDLE(MGPT_GL_SERVER_INIT_RTL, this_source_type, false)
{
	MG_PB<msg_define::game_init_info> kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	bool res = GetGameServerApp()->GetServerManager()->SetGameInitInfo(kMsg.m_value);
	if(true == res)
	{
		UT_ServerHelper::LoadRankOpenInfoByExcelToRedis();
	}	
}

ON_HANDLE(MGPT_GL_SERVER_INIT_RTL_GlOBAL_SERVER,  this_source_type, false)
{
	if(GetGameServerID() == GetGameServerEntryID())
	{
		return;
	}

	MG_PB<msg_define::game_init_info> kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	bool res = GetGameServerApp()->GetServerManager()->ResetGameInitInfo(kMsg.m_value);
	if(true == res)
	{
		 GetGameServerApp()->GetServerManager()->GameServerLoadSuccess();
		UT_ServerHelper::LoadRankOpenInfoByExcelToRedis();
	}
}

ON_HANDLE(MGPT_GL_SERVER_INIT_RTL_SET_ENTRY_ID, this_source_type, false)
{
	MG_RLT_InitSetEntryID kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GL_SERVER_INIT_RTL_SET_ENTRY_ID_Decode error! gsid:%d", GetGameServerID());
		return;
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SetRedisEntryServerPredix! gsid:%d, serverid:%d, entryID:%d", GetGameServerID(), kMsg.m_iServerID, kMsg.m_iEntryID);

	if (kMsg.m_iServerID != GetGameServerID() || kMsg.m_iEntryID == 0)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GL_SERVER_INIT_RTL_SET_ENTRY_ID_Set error! gsid:%d, serverid:%d, entryID:%d", GetGameServerID(), kMsg.m_iServerID, kMsg.m_iEntryID);
		return;
	}

	GetGameServerApp()->SetServerEntryId(kMsg.m_iEntryID);
	GetGameServerApp()->SetRedisEntryServerPredix(kMsg.m_iEntryID);	
}

ON_HANDLE(MGPT_GL_RESP_CLOSESERVER, this_source_type, false)
{
	freopen("CON", "w", stdout);
	GameServerManager* servermanager = GetGameServerApp()->GetServerManager();
	servermanager->CloseAllServer();
	std::cout << std::io_color::red << "You are closed by GM, press any key to exit!" << std::endl;
	std::cin.get();
	exit(1);
}

ON_HANDLE(MGPT_GMTOOL2_SEND_MAIL_ALL_CHAR, this_source_type, false)
{
	MG_SS_SendMail kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return;
	}
	Utility::UT_SIMDataList kItems;
	StringType strItems = kMsg.m_strItemData.c_str();
	StringType strTitle = kMsg.m_strTitle.c_str();
	StringType strContent = kMsg.m_strContent.c_str();
	Utility::ParseFmtString2DataIFs(strItems, kItems);

	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to all:", cit->ToString().c_str());
		}
	}

	UT_ServerHelper::GM_SendToMailAll( strTitle, strContent, 0, kEntryList, GetGameServerID(), "system");
}

ON_HANDLE(MGPT_GMTOOL2_SEND_MAIL_ONLINE, this_source_type, false)
{
}

ON_HANDLE(MGPT_GMTOOL2_PAY, this_source_type, false)
{
	// pay or fetch
	MG_SS_Pay_Notify kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GMTOOL2_PAY Decode error userid %d m_uiCharID：%ld :", rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GMTOOL2_PAY FindPlayer error m_uiUserID %d  charId %d  :",rkMsgHead.m_uiUserID,rkMsgHead.m_uiCharID);
		return;
	}
	pkPlayer->FetchSinglePay(kMsg.m_strOrderId);
	return;
}

ON_HANDLE(MGPT_GMTOOL2_SET_GMLEVEL, this_source_type, false)
{
	MG_SS_SetGMLevel kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GMTOOL2_SET_GMLEVEL Decode error userid %d m_uiCharID：%ld :", rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GMTOOL2_SET_GMLEVEL FindPlayer error m_uiUserID %d  charId %d  :",rkMsgHead.m_uiUserID,rkMsgHead.m_uiCharID);
		return;
	}
	pkPlayer->SetGMLevel(kMsg.m_nGMLevel);
	pkPlayer->SyncControllerPropertyToClient();
	return;
}

ON_HANDLE(MGPT_GMTOOL2_REPORT_ONLINE_COUNT, this_source_type, false)
{
	MG_SS_OnlineCount kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int32_t nServerEntryId = GetGameServerApp()->GetServerEntryId();
	int32_t nServerId = GetGameServerApp()->GetServerManager()->GetCurServerId();
	if ( nServerEntryId != nServerId )
		return;

	time_t nTimeStamp = time(NULL);
	oss_360_log_online_count(kMsg.m_nCount);
}


ON_HANDLE(MGPT_GMTOOL2_SEND_MAIL_USER, this_source_type, false)
{
	MG_SS_SendMail kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int32_t nServerEntryId = GetGameServerApp()->GetServerEntryId();
	int32_t nServerId = GetGameServerApp()->GetServerManager()->GetCurServerId();
	// Only Entry node can send multi-user mail.
	if ( nServerEntryId != nServerId )
		return;

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return;
	}
	Utility::UT_SIMDataList kItems;
	StringType strItems = kMsg.m_strItemData.c_str();
	StringType strTitle = kMsg.m_strTitle.c_str();
	StringType strContent = kMsg.m_strContent.c_str();
	Utility::ParseFmtString2DataIFs(strItems, kItems);

	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to all:", cit->ToString().c_str());
		}
	}

	for ( int i = 0; i < kMsg.m_nUserIds.size(); i++ )
	{
		unique_id_impl charId = kMsg.m_nUserIds[i];
		if ( charId == 0 )	
			continue;
		if ( charId.catagory() == 0 )
			charId = GEN_UNIQUE_ID(0, nServerEntryId, charId);
		UT_ServerHelper::SendToMail(strTitle, strContent, charId, "", 0, kEntryList, "system");
	}
}

ON_HANDLE(MGPT_GL_RESP_ALLSERVER_CONNECTTIME, this_source_type, false)
{
	MG_GL_RESP_AllServerConnectTime kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	StringStreamType sStream("");
	std::map<int32_t, int32_t>::iterator beg = kMsg.m_AllInfo.begin();
	struct tm* result;
	int32_t tm_year = 0;
	int32_t tm_mon = 0;
	int num = 0;
	for (int32_t i = 0; i < (int32_t)kMsg.m_AllInfo.size();)
	{
		std::map<int32_t, int32_t>::iterator beg = kMsg.m_AllInfo.begin();
		int32_t i_serverid = 0;
		int32_t i_servertime = 0;
		for (; beg != kMsg.m_AllInfo.end(); ++beg)
		{
			if (i_servertime < beg->second)
			{
				i_serverid = beg->first;
				i_servertime = beg->second;
			}
		}
		time_t timep = (time_t)i_servertime;
		result = localtime(&timep);
		sStream << "[";
		sStream << i_serverid;
		sStream << " ";
		tm_year = result->tm_year + 1900;
		tm_mon = result->tm_mon + 1;
		sStream << tm_year;
		sStream << "/";
		sStream << tm_mon;
		sStream << "/";
		sStream << result->tm_mday;
		sStream << " ";
		sStream << result->tm_hour;
		sStream << ":";
		sStream << result->tm_min;
		sStream << ":";
		sStream << result->tm_sec;
		sStream << "]\n";
		kMsg.m_AllInfo.erase(i_serverid);
		num++;
		if (num == 5)
		{
			pkPlayer->SendNotifyStrToClient(sStream.str(), PROMPT_CHANNEL_SYSTEM_TIP, PROMPT_LEVEL_NORMAL);
			sStream.str("");
			num = 0;
		}

	}
	pkPlayer->SendNotifyStrToClient(sStream.str(), PROMPT_CHANNEL_SYSTEM_TIP, PROMPT_LEVEL_NORMAL);
}

ON_HANDLE(MGPT_GL_RESP_EXPORTROBOT, this_source_type, false)
{
	MG_GL_RESP_ExportRobot kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	ExportRobot* exportrobot = (ExportRobot*)ExportRobot::GetSingletonPtr();
	exportrobot->Export2LoginSvrdResp(kMsg.m_Result, kMsg.m_Index, kMsg.m_AllName);
}

ON_HANDLE(MGPT_GL_SYNC_CASH_OP_RESP, this_source_type, false)
{
	MG_PB<char_data_define::user_cash_info> respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_CommonShopAsset& kCommonShopAsset = pkPlayer->GetCommonShopAsset();
	const char_data_define::user_cash_info& kuser = respMsg.m_value;
	for (int i = 0; i < kuser.char_ops_size(); ++i)
	{
		const char_data_define::char_cash_op_info& kchar = kuser.char_ops(i);
		if (kchar.char_id() == 0 || kchar.char_id() == rkMsgHead.m_uiCharID)
		{
			for (int ii = 0; ii < kchar.op_clusters_size(); ++ii)
			{
				const char_data_define::cash_op_cluster& kcluster = kchar.op_clusters(ii);
				LC_CashOPCluster* pkCluster = kCommonShopAsset.GetCashOPCluster(kcluster.cash_type());
				if (pkCluster)
				{
					for (int j = 0; j < kcluster.records_size(); ++j)
					{
						const char_data_define::cash_op_record& krecord = kcluster.records(j);
						LC_CashOPRecord* pkRecord = pkCluster->GetOPRecord(krecord.type());
						if (pkRecord)
						{
							pkRecord->SyncOPs(krecord.id());
						}
					}
				}
			}
		}
	}

	if (kuser.cashs_size())
	{
		for (int i = 0; i < kuser.cashs_size(); ++i)
		{
			const char_data_define::cash_entry& kcash = kuser.cashs(i);
			LC_CashOPCluster* pkCluster = kCommonShopAsset.GetCashOPCluster(kcash.type());
			if(pkCluster == NULL)
			{
				continue;
			}
			pkPlayer->SetCash(kcash.value() + pkCluster->CaculateDelta(), (CashType)kcash.type());
			//vip信息存储了credit，不依赖linux更新
			//pkPlayer->SetCredit(kcash.credit() + pkCluster->CaculateCredit(CS_OP_TP_DEPOSIT), (CashType)kcash.type());
		}
	}
	pkPlayer->RegisterCharInfo();
}

ON_HANDLE(MGPT_GL_SYNC_FREEZE_PLAYER_RESP, this_source_type, false)
{
	MG_GL_Freeze_Player kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (kMsg.m_iResult != 0)
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	switch (kMsg.m_iType)
	{
		case 0:			//封账号
			{
				pkPlayer->SetFreezeAcc(kMsg.m_iTime);
				pkPlayer->RegisterCharInfo();
			}
			break;
		case 1:			//封角色
			{
				pkPlayer->SetFreezeRol(kMsg.m_iTime);
				pkPlayer->RegisterCharInfo();
			}
			break;
		default:
			break;
	}
}

ON_HANDLE(MGPT_GL_SYNC_JSON_SAVE2REDIS, this_source_type, false)
{
	MG_SS_LOGIN_JSON_SAVE kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	switch (kMsg.m_jsonType)
	{
	case E_JsonSave2RedisType_ServerList:			//服务器列表
		{
			GetGameServerApp()->GetServerManager()->LoadServerListJson(kMsg.m_loadTime);
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void dealGmTransCallBack(Protocol::SS_MG_HEAD& rkMsgHead, NW_ServerSession * pkSession,char* pMsgBody, uint16_t usBodySize)
{
	MG_GMTool_CommRtl rtnMsg;
	if (!rtnMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (pkTranManager == NULL)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->GetTran(rtnMsg.m_iID);
	if (pkTran != NULL)
	{
		pkTran->HandlerMsg(rkMsgHead, &rtnMsg);
		pkTranManager->UnRegisteTran(pkTran->GetID());
	}
}


ON_HANDLE(MGPT_GAME_COM_GM_TRANS_MSG, SERVER_TYPE_LOGIN, false)
{
	dealGmTransCallBack(rkMsgHead, pkSession, pMsgBody, usBodySize);
}

ON_HANDLE(MGPT_GAME_COM_GM_TRANS_MSG, SERVER_TYPE_DB, false)
{
	dealGmTransCallBack(rkMsgHead, pkSession, pMsgBody, usBodySize);
}

ON_HANDLE(MGPT_GAME_COM_GM_TRANS_MSG, SERVER_TYPE_GROUP, false)
{
	dealGmTransCallBack(rkMsgHead, pkSession, pMsgBody, usBodySize);
}

ON_HANDLE(MGPT_GAME_COM_GM_TRANS_MSG, SERVER_TYPE_MAIL_DB, false)
{
	dealGmTransCallBack(rkMsgHead, pkSession, pMsgBody, usBodySize);
}

ON_HANDLE(MGPT_GAME_COM_GM_TRANS_MSG, SERVER_TYPE_TRADE, false)
{
	dealGmTransCallBack(rkMsgHead, pkSession, pMsgBody, usBodySize);
}

ON_HANDLE(MGPT_GAME_COM_GM_TRANS_MSG, SERVER_TYPE_SOCIETY, false)
{
	dealGmTransCallBack(rkMsgHead, pkSession, pMsgBody, usBodySize);
}
////////////////////////////////////////////////////////////////////////////////////////////////
ON_HANDLE(MGPT_SS_ONLINE_USER_ADD, this_source_type, false)
{
	LC_ServerPlayerEnterLeaveManager* pIOMgr = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if ( pIOMgr == NULL )
		return;

	pIOMgr->AddOnlinePlayer( rkMsgHead.m_uiCharID );
}

ON_HANDLE(MGPT_SS_ONLINE_USER_DEL, this_source_type, false)
{
	LC_ServerPlayerEnterLeaveManager* pIOMgr = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if ( pIOMgr == NULL )
		return;

	pIOMgr->DelOnlinePlayer( rkMsgHead.m_uiCharID );

}

ON_HANDLE(MGPT_SS_GAME_NODE_TYPE, this_source_type, false)
{
	GetGameServerApp()->SetNodeType(rkMsgHead.m_iReserved);
}

ON_HANDLE(MGPT_SS_EXCLUDE_MAP_LIST, this_source_type, false)
{
	/*
	MG_SS_Exclude_Map_List msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	//if ( pkWorldMgr )
	{
		int nSize = msg.vecMapList.size();
		for ( int i = 0; i < nSize; i++ )
		{
			pkWorldMgr->InsertExcludedMapResId(msg.vecMapList[i]);
		}
	}
	*/
}

ON_HANDLE(MGPT_SS_NOTICE_UPDATE_WATCHTOWER_INFO_GS, this_source_type, false)
{
	if(true == GetGameServerApp()->IsNodeType_Global())
	{
		return;
	}

	bool tag = UT_ServerHelper::UpdateWatchTowerFromRedis(true);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "otherServer update tower state from redis! serverid:%d, tag:%d\n", GetGameServerID(), tag);
}

ON_HANDLE(MGPT_SS_NOTICEBOSSFIRSTKILLINFO_GS, this_source_type, false)
{
	MG_SS_NoticeBossFirstKillInfo msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_SS_NOTICEBOSSFIRSTKILLINFO_GS serverid:%d, bossID:%d\n", GetGameServerID(), msg.m_nBossID);
	UT_ServerHelper::UpdateServerFirstKillBossPlayerInfo(msg.m_nBossID, msg.m_nKillerInfo.m_nPlayerName, msg.m_nKillerInfo.m_nPlayerLevel, msg.m_nKillerInfo.m_nCommanderTitleID, msg.m_nKillerInfo.m_nBornCharType, msg.m_nKillerInfo.m_nCommanderTitleFrameID);

	//broadcast
	MG_SyncBossFirstKillInfo syncMsg;
	syncMsg.m_nBossID = msg.m_nBossID;
	syncMsg.m_bKillInfo = true;
	syncMsg.m_nKillerInfo.m_nPlayerName = msg.m_nKillerInfo.m_nPlayerName;
	syncMsg.m_nKillerInfo.m_nBornCharType = msg.m_nKillerInfo.m_nBornCharType;
	syncMsg.m_nKillerInfo.m_nCommanderTitleID = msg.m_nKillerInfo.m_nCommanderTitleID;
	syncMsg.m_nKillerInfo.m_nPlayerLevel = msg.m_nKillerInfo.m_nPlayerLevel;
	syncMsg.m_nKillerInfo.m_nCommanderTitleFrameID = msg.m_nKillerInfo.m_nCommanderTitleFrameID;
	GetGameServerApp()->BroadCastMsgToClient(MGPT_SYNC_BOSS_FIRST_KILL_INFO, &syncMsg);
}

ON_HANDLE(MGPT_SS_NOTICEBOSSFIRSTKILLREWARDINFO_GS, this_source_type, false)
{
	MG_SS_NoticeBossFirstKillRewardInfo msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_SS_NOTICEBOSSFIRSTKILLREWARDINFO_GS serverid:%d, bossID:%d\n", GetGameServerID(), msg.m_nBossID);
	UT_ServerHelper::UpdateServerFirstKillBossRewardPlayerInfo(msg.m_nBossID, msg.m_nRewardInfo);
}

ON_HANDLE(MGPT_SS_UPDATETESTDATA_GS, this_source_type, false)
{
	MG_SS_UpdateTestData msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::UpdateTestDatabase(msg.m_TopIDVec);
	UT_ServerHelper::SetCheckAwardTime();

	OnSyncTestDatabase info;
	static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	pkWorldMgr->ForAllPlayer(&info);
}


ON_HANDLE(MGPT_SS_UPDATE_ALLIANCE_BOSS_INFO_REQ, this_source_type, false)
{
	MG_SS_Update_Alliance_Boss_Info msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(true != GetGameServerApp()->IsNodeType_Global())
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_SS_UPDATE_ALLIANCE_BOSS_INFO_REQ not is GlobalNode");
		return;
	}

	GameStory::LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	if(NULL == pkGSMgr)
	{
	 	return;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_SS_UPDATE_ALLIANCE_BOSS_INFO_REQ");
	pkGSMgr->Update_Alliance_Boss_Info(msg.m_nStoryID);
}

ON_HANDLE(MGPT_SS_REPETITION_GS_NODE, this_source_type, false)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "REPETITION GS Node.");
	return;
	std::cout << std::io_color::green << Utility::Utf8Show("GameServer Startup Successfully.") << std::endl;
	std::cout << std::io_color::white;

	//TestLeak2();
	OpenSTDStream();
	std::cout << std::io_color::red << Utility::Utf8Show("Repeatedly opened nodes") << std::endl;
	getchar();
	exit(0);
	//printf("MGPT_SS_REPETITION_GS_NODE\n");
}

}
