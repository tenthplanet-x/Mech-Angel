#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_CS_SearchPlayerProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_Common.h"
using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

ON_HANDLE(MGPT_REQ_SEARCH_PLAYER_BASIC, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SearchPlayer_Basic kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	uint32_t uiLastSearchPlayerID	= kMsg.m_lLastSearchPlayerID;
	//bool	 bSearchForward			= kMsg.m_bSearchForward;
	MG_RLT_SearchPlayer kRltMsg;

	int iSearchCount = 0;
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	const LC_PlayerIDPtrMap& kPlayerIDMap = pkPlayerManager->GetPlayerIDMap();

	LC_PlayerIDPtrMap::const_iterator IterStart;
	LC_PlayerIDPtrMap::const_iterator IterEnd;

	IterStart = kPlayerIDMap.find(uiLastSearchPlayerID);
	IterEnd = kPlayerIDMap.end();
	if (IterStart == kPlayerIDMap.end())
	{
		IterStart = kPlayerIDMap.begin();
	}


	for (LC_PlayerIDPtrMap::const_iterator it = IterStart; it != IterEnd && iSearchCount < MAX_SEARCH_PLAYER_NUM; ++it)
	{
		LC_PlayerBase* searchPlayer = it->second;
		if (NULL != searchPlayer && searchPlayer->GetID() != pkPlayer->GetID())
		{
			if (UT_BaseStringUtility::FindSubString(pkPlayer->GetOwnerCharName(), STDSTR2TPSTR(kMsg.m_szName)) != -1)
			{
				PS_SocietyBasicInfo& rkInfo = kRltMsg.kPlayerList[iSearchCount];
				TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), rkInfo.szCharName);
				rkInfo.iCharType	= (int8_t)pkPlayer->GetCharType();
				rkInfo.uiCharID		= pkPlayer->GetInstance();
				rkInfo.iCareer		= (int8_t)pkPlayer->GetCareerType();
				rkInfo.iLevel		= (int16_t)pkPlayer->GetLevel();
				rkInfo.iWeaponType	= (int8_t)pkPlayer->GetWeaponAttribute();
				rkInfo.iBornCharType = (int8_t)pkPlayer->GetProfilePictureID();
				rkInfo.beFriendWithEachOther = searchPlayer->GetSocietyAsset()->IsInBeSociety(ST_FRIEND, pkPlayer->GetInstance());

				++iSearchCount;
			}
		}
	}

	kRltMsg.m_currentArraySize = iSearchCount;
	pkPlayer->SendMsgToClient(MGPT_RLT_SEARCH_PLAYER, &kRltMsg);
}

ON_HANDLE(MGPT_REQ_SEARCH_PLAYER_ADVANCED, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SearchPlayer_Advanced kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SearchPlayer kRltMsg;
	uint32_t uiLastSearchPlayerID = kMsg.m_lLastSearchPlayerID;
	//bool bSearchForward	= kMsg.m_bSearchForward;
	int iSearchCount = 0;

	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	const LC_PlayerIDPtrMap& kPlayerIDMap = pkPlayerManager->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator IterStart;
	LC_PlayerIDPtrMap::const_iterator IterEnd;

	IterStart = kPlayerIDMap.find(uiLastSearchPlayerID);
	IterEnd = kPlayerIDMap.end();
	if (IterStart == kPlayerIDMap.end())
	{
		IterStart = kPlayerIDMap.begin();
	}

	for (LC_PlayerIDPtrMap::const_iterator it = IterStart; it != IterEnd && iSearchCount < MAX_SEARCH_PLAYER_NUM; ++it)
	{
		LC_PlayerBase* searchPlayer = it->second;
		if (NULL != searchPlayer && searchPlayer->GetID() != pkPlayer->GetID())
		{
			if ((kMsg.m_iCharType == 0 || kMsg.m_iCharType == pkPlayer->GetCharType())
					&& (kMsg.m_iWeaponType == 0 || kMsg.m_iWeaponType == pkPlayer->GetWeaponAttribute())
					&& (kMsg.m_sCareerType == -1 || kMsg.m_sCareerType == pkPlayer->GetCareerType())
					&& (kMsg.m_usBeginLevel == 0 || (kMsg.m_usBeginLevel <= pkPlayer->GetLevel() && pkPlayer->GetLevel() <= kMsg.m_usEndLevel)))
			{
				PS_SocietyBasicInfo& rkInfo = kRltMsg.kPlayerList[iSearchCount];
				TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), rkInfo.szCharName);
				rkInfo.iCharType	= (int8_t)pkPlayer->GetCharType();
				rkInfo.uiCharID		= pkPlayer->GetInstance();
				rkInfo.iCareer		= (int8_t)pkPlayer->GetCareerType();
				rkInfo.iWeaponType	= (int8_t)pkPlayer->GetWeaponAttribute();
				rkInfo.iLevel		= (uint16_t)pkPlayer->GetLevel();
				rkInfo.iBornCharType = (int8_t)pkPlayer->GetProfilePictureID();
				rkInfo.beFriendWithEachOther = searchPlayer->GetSocietyAsset()->IsInBeSociety(ST_FRIEND, pkPlayer->GetInstance());

				++iSearchCount;
			}
		}
	}

	kRltMsg.m_currentArraySize = iSearchCount;
	pkPlayer->SendMsgToClient(MGPT_RLT_SEARCH_PLAYER, &kRltMsg);
}

ON_HANDLE(MGPT_REQ_RECORD_TRANSPORT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_Record_Transport kmsg;
	if (!kmsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->GetTransportAsset()->addTransport(kmsg.m_transportId);
}

}
