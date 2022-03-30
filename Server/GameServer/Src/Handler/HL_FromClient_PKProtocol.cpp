#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_GameProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerMysticalShop.h"
#include "CF_WorldMapList.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_MODIFY_PK_SETTING, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ModifyPKMode reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	object_id_type lPlayerID = pkPlayer->GetID();
	LC_PKRecorderBase* pkPKRecorder = pkPlayer->GetPKRecorder();
	if (!pkPKRecorder)
	{
		return;
	}

	if (pkPKRecorder->IsJuniorProtected())
	{
		return;
	}

	CSVFile::CF_WorldMapList::DataEntry* ety = SafeGetCSVFileDataEntryPtr<CSVFile::CF_WorldMapList>(pkPlayer->GetMapResID());
	if (ety && 
		(std::find(ety->_FixPKMode.begin(), ety->_FixPKMode.end(), reqMsg.m_lpkmode) == ety->_FixPKMode.end()
		&& std::find(ety->_FixPKMode.begin(), ety->_FixPKMode.end(), PKM_MIN) == ety->_FixPKMode.end() ) )
	{
		return ;
	}
	pkPKRecorder->SetPKMode(reqMsg.m_lpkmode);
}

ON_HANDLE(MGPT_REQ_GET_PK_VALUE, this_source_type, false)
{
	MG_REQ_GetPKValue reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	static LC_ServerPlayerManager* pkPlayerMgr = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkPlayerMgr)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	if (reqMsg.m_lPlayerID != rkMsgHead.m_uiCharID)
	{
		pkPlayer = (LC_ServerPlayer*)pkPlayerMgr->FindPlayer(reqMsg.m_lPlayerID);
	}

	if (pkPlayer == NULL)
	{
		return;
	}

	LC_PKRecorderBase* pkPKRecorder = pkPlayer->GetPKRecorder();
	if (pkPKRecorder == NULL)
	{
		return;
	}

	MG_RTL_GetPKValue rtlMsg;
	rtlMsg.m_lPlayerID = reqMsg.m_lPlayerID;
	rtlMsg.m_lPkValue = pkPKRecorder->GetPKValue();

	pkPlayer->SendMsgToClient(MGPT_RLT_GET_PK_VALUE, &rtlMsg);
}

ON_HANDLE(MGPT_REQ_GAIN_OFFLINE_EXP, this_source_type, false)
{
	MG_REQ_GainOfflineExp reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_RTL_GainOfflineExp rtlMsg;
	rtlMsg.m_gainType = reqMsg.m_gainType;
	//rtlMsg.m_clientExp = reqMsg.m_clientExp;
	//rtlMsg.m_costMoney = 0;
	//rtlMsg.m_result = pkPlayer->GainOffLineExp(rtlMsg.m_gainType, rtlMsg.m_clientExp, rtlMsg.m_costMoney);

	rtlMsg.m_result = pkPlayer->GainOffLineExp(rtlMsg.m_gainType);
	pkPlayer->SendMsgToClient(MGPT_RLT_GAIN_OFFLINE_EXP, &rtlMsg);

	if (rtlMsg.m_result == RE_SUCCESS)
	{
		MG_RTL_GetOfflineExp rtlMsg;
		rtlMsg.m_nExp = pkPlayer->GetOffLineExp();
		rtlMsg.m_nOffTime = pkPlayer->GetOfflineExpTime();
		pkPlayer->SendMsgToClient(MGPT_RLT_GET_OFFLINE_EXP, &rtlMsg);
	}
}

ON_HANDLE(MGPT_REQ_GET_OFFLINE_EXP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_RTL_GetOfflineExp rtlMsg;
	rtlMsg.m_nExp = pkPlayer->GetOffLineExp();
	rtlMsg.m_nOffTime = pkPlayer->GetOfflineExpTime();
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_OFFLINE_EXP, &rtlMsg);
}

}
