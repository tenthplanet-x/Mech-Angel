#include "MG_CS_GuildHome.h"
#include "LC_ClientGuildHomeMgr.h"

using namespace GameLogic;
using namespace Protocol;

void HL_FromGameServer::_handlerRltEnterGuildHome( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_EnterGuildHome rltEnterMsg;
	if (!rltEnterMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_EnterGuildHome��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_EnterGuildHome���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}
	ResultType lResult = rltEnterMsg.m_uiResult;
	SendLogicInfoMessage(lResult);

}

void HL_FromGameServer::_handlerRltLeaveGuildHome( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{

	MG_RLT_LeaveGuildHome rltLeaveMsg;
	if (!rltLeaveMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_LeaveGuildHome��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_LeaveGuildHome���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}
	ResultType lResult = rltLeaveMsg.m_uiResult;
	SendLogicInfoMessage(lResult);
}

void HL_FromGameServer::_handlerRltUpgradeGuildNPC(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_UpgradeNPC rltUpgradeMsg;
	if (!rltUpgradeMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_UpgradeNPC��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_UpgradeNPC���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}

	ResultType lResult = rltUpgradeMsg.m_uiResult;
	SendLogicInfoMessage(lResult);
	
}

void HL_FromGameServer::_handlerRltBuyFromGuildNPC(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BUY_FROM_GUILDNPC rltUpgradeMsg;
	if (!rltUpgradeMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_BUY_FROM_GUILDNPC��������");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_BUY_FROM_GUILDNPC���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}
	ResultType lResult = rltUpgradeMsg.m_uiResult;
	SendLogicInfoMessage(lResult);
}


void HL_FromGameServer::_handlerRltDonate(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DONATE kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_DONATE��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_DONATE���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}

	ResultType lResult = kRltMsg.m_uiResult;
	SendLogicInfoMessage(lResult);
}

void HL_FromGameServer::_handlerRltPutWareHouse(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_PUTWAREHOUSE kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_PUTWAREHOUSE��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_PUTWAREHOUSE���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}

	ResultType lResult = kRltMsg.m_uiResult;
	if (lResult == RE_SUCCESS_PUTGUILDWAREHOUSE)
	{
		//ˢ��DirtyFlag
		LC_ClientGuildHomeMgr* pkClientGuildHomeMgr = LC_ClientGuildHomeMgr::GetSingletonPtr();
		if (pkClientGuildHomeMgr)
		{
			pkClientGuildHomeMgr->AddDirtyFlagRevisionID(LC_ClientGuildHomeMgr::GUILDHOME_WAREHOUSE);
		}
	}

	SendLogicInfoMessage(lResult);
}

void HL_FromGameServer::_handlerRltSwapWareHouse(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GETWAREHOUSE kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GETWAREHOUSE��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GETWAREHOUSE���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}

	
	ResultType lResult = kRltMsg.m_uiResult;
	if (lResult == RE_SUCCESS_GETGUILDWAREHOUSE)
	{
		//ˢ��DirtyFlag
		LC_ClientGuildHomeMgr* pkClientGuildHomeMgr = LC_ClientGuildHomeMgr::GetSingletonPtr();
		if (pkClientGuildHomeMgr)
		{
			pkClientGuildHomeMgr->AddDirtyFlagRevisionID(LC_ClientGuildHomeMgr::GUILDHOME_WAREHOUSE);
		}
	}
	SendLogicInfoMessage(lResult);

}


void HL_FromGameServer::_handlerRltGetGuildWareHouse( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_GUILDWAREHOUSE kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GETWAREHOUSE��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GETWAREHOUSE���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}

	ResultType lResult = kRltMsg.m_uiResult;
	
	if (lResult == RE_SUCCESS)
	{
		LC_ClientGuildHomeMgr* pkClientGuildHomeMgr = LC_ClientGuildHomeMgr::GetSingletonPtr();
		if (pkClientGuildHomeMgr)
		{
			//���±��زֿ�
			pkClientGuildHomeMgr->GetGuildWareHouseFromProtocol(kRltMsg);
			pkClientGuildHomeMgr->AddDirtyFlagRevisionID(LC_ClientGuildHomeMgr::GUILDHOME_WAREHOUSE);
		}
	}
	SendLogicInfoMessage(lResult);
}

void HL_FromGameServer::_handlerRltThiefCount( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_CHANGE_THIEF_RECORD kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GETWAREHOUSE��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GETWAREHOUSE���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}
	LC_ClientGuildHomeMgr* pkClientGuildHomeMgr = LC_ClientGuildHomeMgr::GetSingletonPtr();
	if (pkClientGuildHomeMgr)
	{
		pkClientGuildHomeMgr->GetKillThiefStoryCount(kRltMsg);
		pkClientGuildHomeMgr->AddDirtyFlagRevisionID(LC_ClientGuildHomeMgr::GUILDHOME_KILLTHIEF_COUNT);
	}
}

void HL_FromGameServer::_handlerRltGetPrizeOfKillThief( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_GetPrizeOfKillThief kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GetPrizeOfKillThief��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GetPrizeOfKillThief���󷵻���Ϣ�߼����� No PkGamePlayer");
		return;
	}
	SendLogicInfoMessage(kRltMsg.m_uiResult);
}

void HL_FromGameServer::_handlerRltGuildHomeInStory( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_GUILDHOME_STORY kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_RLT_GUILDHOME_STORY��������");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"���ɼ�԰����KILLTHIEF� No PkGamePlayer");
		return;
	}
	LC_ClientGuildHomeMgr* pkClientGuildHomeMgr = LC_ClientGuildHomeMgr::GetSingletonPtr();
	if (!pkClientGuildHomeMgr)
	{
		return;
	}
	if (GameLogic::KILLTHIEF == kRltMsg.m_iStory)
	{
		pkClientGuildHomeMgr->AddDirtyFlagRevisionID(LC_ClientGuildHomeMgr::GUILDHOME_KILLTHIEF_COUNT);
		pkClientGuildHomeMgr->SetIsInThief(true);
	}
	else
	{
		pkClientGuildHomeMgr->AddDirtyFlagRevisionID(LC_ClientGuildHomeMgr::GUILDHOME_KILLTHIEF_COUNT);
		pkClientGuildHomeMgr->SetIsInThief(false);
	}
	
}
	
void HL_FromGameServer::_handlerRltOpenKillThief( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_OpenKillThief kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		return;
	}
	if (kRltMsg.m_uiResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(RE_FAILED_OPEM_KILL_THIEF_HAS_PALYED);
	}
}

void HL_FromGameServer::_handlerRltKillThiefRecordTime( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_KillThiefRecordTime kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGuildHomeMgr* pkClientGuildHomeMgr = LC_ClientGuildHomeMgr::GetSingletonPtr();
	if (pkClientGuildHomeMgr)
	{
		pkClientGuildHomeMgr->SetRecordTime(kRltMsg.m_fRecordTime);
		pkClientGuildHomeMgr->AddDirtyFlagRevisionID(LC_ClientGuildHomeMgr::GUILDHOME_KILLTHIEF_RECORDTIME);
	}
}
void HL_FromGameServer::_handlerRltKillThiefInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_KILLTHIEF_INFO kRltMsg;
	if (!kRltMsg.Decode(pMsg,wSize))
	{
		return;
	}
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_NPCFUNCTION_SHOW);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,kRltMsg.m_uiScore);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,kRltMsg.m_uiFame);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);


}
