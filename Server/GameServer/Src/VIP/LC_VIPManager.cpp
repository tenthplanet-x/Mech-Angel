
#include "LC_VIPManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerLogicManager.h"
#include "NW_Helper.h"

using namespace Utility;
using namespace Protocol;
using namespace GameLogic;
//---------------------------------------------------------------------
LC_VIPManager::LC_VIPManager()
{

}
//---------------------------------------------------------------------
LC_VIPManager::~LC_VIPManager()
{

}
//---------------------------------------------------------------------
void LC_VIPManager::Init()
{
	/*UT_LineFile kLineFile;
	kLineFile.Load("Data\\Config\\all.txt");

	m_kIPList.clear();

	int iIPCount = kLineFile.GetLineCount();

	for(int i = 0; i < iIPCount; i++)
	{
		uint32_t uiIP = inet_addr(kLineFile.GetLineString(i).c_str());
		m_kIPList.push_back(ntohl(uiIP));
	}

	m_fLastUpdateTime = 0.0f;*/
}
//---------------------------------------------------------------------
void LC_VIPManager::UnInit()
{
	/*m_kIPList.clear();*/
}
//---------------------------------------------------------------------
void LC_VIPManager::Update(float fCurrentTime)
{
}
//---------------------------------------------------------------------
void LC_VIPManager::PlayerEnterGame(LC_ServerPlayer* pkPlayer,uint32_t uiIP)
{
	/*uint32_t uiIPCount = m_kIPList.size();

	pkPlayer->SetVIPLevel(0);

	for(uint32_t uiIPIndex = 0; uiIPIndex < uiIPCount; uiIPIndex++)
	{
		if(uiIP == m_kIPList[uiIPIndex])
		{
			pkPlayer->SetVIPLevel(1);
			pkPlayer->ActiveSkillState(38200207,0,0);
		}
	}*/
}
//---------------------------------------------------------------------
void LC_VIPManager::PlayerKillNPC(LC_ServerPlayer* pkPlayer,int lNPCTypeID)
{
	/*if(pkPlayer->GetVIPLevel() <= 0)
		return;*/
}
//---------------------------------------------------------------------
void LC_VIPManager::PlayerSendGMCommand(LC_ServerPlayer* pkPlayer,const StringType& rkCommand )
{
	if(pkPlayer->GetVIPLevel() != GM_ACCOUNT_VIP_LEVEL)
		return;

	TStringVector kCommandVector = UT_BaseStringUtility::SplitString(rkCommand,' ');
	if(kCommandVector.size() <= 0)
		return;

	const StringType & kCommandHeader = kCommandVector[0];

	if(UT_BaseStringUtility::StrCompare(kCommandHeader,"setspeed") == 0)
	{
		if(kCommandVector.size() < 2)
			return;

		const StringType & kParameter1	= kCommandVector[1];
		int	   lTargetSpeed = atoi(kParameter1.c_str());

		lTargetSpeed = UT_MathBase::MinInt(lTargetSpeed,1200);
		
		pkPlayer->GetAttributeMap()->SetAttributeBase(ATT_MOVE_SPEED,lTargetSpeed);
		pkPlayer->GetAttributeMap()->RecomputeAttribute(ATT_MOVE_SPEED);
	}

	if(UT_BaseStringUtility::StrCompare(kCommandHeader,"movetoid") == 0)
	{
		if(kCommandVector.size() < 2)
			return;

		const StringType & kParameter1 = kCommandVector[1];
		object_id_type	   lTargetID = atoi(kParameter1.c_str());

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();

		LC_PlayerBase* pkTargetPlayer = pkServerPlayerManager->FindPlayer(lTargetID);
		if(NULL == pkTargetPlayer)
			return;
		
		pkPlayer->ChangeMap(pkTargetPlayer->GetMapLogicID(),pkTargetPlayer->GetMapResID(),pkTargetPlayer->GetMapLogicType(),pkTargetPlayer->GetCurrentLocation());
	}

	if(UT_BaseStringUtility::StrCompare(kCommandHeader,"movetoname") == 0)
	{
		if(kCommandVector.size() < 2)
			return;

		const StringType & kName = kCommandVector[1];

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();

		LC_PlayerBase* pkTargetPlayer = pkServerPlayerManager->FindPlayerByName(kName);
		if(NULL == pkTargetPlayer)
			return;

		pkPlayer->ChangeMap(pkTargetPlayer->GetMapLogicID(),pkTargetPlayer->GetMapResID(),pkTargetPlayer->GetMapLogicType(),pkTargetPlayer->GetCurrentLocation());
	}

	if(UT_BaseStringUtility::StrCompare(kCommandHeader,"movetopos") == 0)
	{
		if(kCommandVector.size() < 2)
			return;

		const StringType & kParameter1 = kCommandVector[1];
		const StringType & kParameter2 = kCommandVector[2];
		const StringType & kParameter3 = kCommandVector[3];
		int	   lMapResID = atoi(kParameter1.c_str());
		int	   lMapPosX  = atoi(kParameter2.c_str());
		int	   lMapPosY  = atoi(kParameter3.c_str());

		MG_RLT_GM_Transfer rltMsg;
		rltMsg.m_iMapResID = lMapResID;
		rltMsg.m_uiPosX = lMapPosX;
		rltMsg.m_uiPosY = lMapPosY;

		pkPlayer->SendMsgToClient(MGPT_RLT_GM_TRANSFER,&rltMsg);
	}

	if(UT_BaseStringUtility::StrCompare(kCommandHeader,"followtoid") == 0)
	{
		if(kCommandVector.size() < 2)
			return;

		const StringType & kParameter1 = kCommandVector[1];
		object_id_type	   lTargetID = atoi(kParameter1.c_str());

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();

		LC_PlayerBase* pkTargetPlayer = pkServerPlayerManager->FindPlayer(lTargetID);

		ResultType iFollowResult;
		object_id_type       lFollowTargetID;
		if(NULL == pkTargetPlayer)
		{
			iFollowResult = RE_FAIL;
			lFollowTargetID = 0;
		}
		else
		{
			iFollowResult = RE_SUCCESS;
			lFollowTargetID = lTargetID;
		}

		MG_RLT_FollowPlayer rltMsg;
		rltMsg.m_iFollowResult = iFollowResult;
		rltMsg.m_lTargetPlayerID = lFollowTargetID;

		pkPlayer->SendMsgToClient(MGPT_RLT_FOLLOW_PLAYER,&rltMsg);
	}

	if(UT_BaseStringUtility::StrCompare(kCommandHeader,"followtoname") == 0)
	{
		if(kCommandVector.size() < 2)
			return;

		const StringType & kName = kCommandVector[1];

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();

		LC_PlayerBase* pkTargetPlayer = pkServerPlayerManager->FindPlayerByName(kName);

		ResultType iFollowResult;
		object_id_type       lFollowTargetID;
		if(NULL == pkTargetPlayer)
		{
			iFollowResult = RE_FAIL;
			lFollowTargetID = 0;
		}
		else
		{
			iFollowResult = RE_SUCCESS;
			lFollowTargetID = pkTargetPlayer->GetID();
		}

		MG_RLT_FollowPlayer rltMsg;
		rltMsg.m_iFollowResult = iFollowResult;
		rltMsg.m_lTargetPlayerID = lFollowTargetID;

		pkPlayer->SendMsgToClient(MGPT_RLT_FOLLOW_PLAYER,&rltMsg);
	}
}
