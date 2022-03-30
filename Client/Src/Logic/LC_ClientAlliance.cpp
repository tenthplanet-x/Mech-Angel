
#include "ClientPCH.h"
#include "LC_ClientAlliance.h"
#include "LC_ClientLogicManager.h"
#include "NW_Helper.h"
#include "NW_Game2ClientSession.h"
#include "LC_ClientGamePlayer.h"
#include "UT_LogicInfoMessage.h"
#include "LC_ClientHelp.h"
#include "UI_UIManager.h"
#include "NW_ClientSessionManager.h"

using namespace GameLogic;
using namespace Protocol;


LC_ClientAlliance::LC_ClientAlliance()
{
	m_groupID = 0;
	m_iWQGainTime = 0;
	m_iGainServerID = 0;
	m_iFactionWinTime = 0;
	m_iFactionDefServer = 0;
	m_iFactionDefStatus = 0;
}

LC_ClientAlliance::~LC_ClientAlliance()
{

}

void LC_ClientAlliance::ReqAllianceInfo()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	MG_Req_Alliance_Server_List msg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_ALLIANCE_SERVER_LIST_REQ, &msg);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientAlliance::ReqAllianceUnitInfo()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_ALLIANCE_UNIT_INFO_REQ, NULL);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientAlliance::RltAllianceUnitInfo(Protocol::MG_Resp_Alliance_Unit_Info &msg)
{
	m_iGainServerID = msg.m_nGainServer;
	m_iWQGainTime = msg.m_nGainTime;

	m_iFactionWinTime = msg.m_iFactionWinTime;
	m_iFactionDefServer = msg.m_iFactionDefServer;
	m_iFactionDefStatus = msg.m_iFactionDefStatus;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GlET_RLT_UINIT_INFO);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
}


bool LC_ClientAlliance::IsCanDrawWQRwd()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return false;

	uint32_t nKey = LC_ClientHelp::GetDailyCleanKeyID(E_RewardGain_KingFlag, 0);
	std::map<uint32_t,uint64_t>::iterator it = m_UnitRwdMap.find(nKey);
	if(it != m_UnitRwdMap.end())
	{
		uint64_t nGainTime = it->second;
		if(nGainTime > 0 && LC_Helper::CheckIsSameDay(m_iWQGainTime, nGainTime))
		{
			return false;
		}

		if(nGainTime > 0 && LC_Helper::CheckIsSameDay(pkGamePlayer->GetServerTimeVal(), nGainTime))
		{
			return false;
		}
	}

	return pkGamePlayer->GetServerID() == m_iGainServerID && m_iWQGainTime > 0;
}

void LC_ClientAlliance::ReqUnitReward()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SEND_REWARD_GAIN_REQ, NULL);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientAlliance::RltUnitReward(uint16_t m_type, const std::map<uint32_t,uint64_t>& rwd_map)
{
	if (m_type == 0)
	{
		m_UnitRwdMap.clear();
		m_UnitRwdMap = rwd_map;
	}
	else if (m_type == 1)
	{
		for ( std::map<uint32_t,uint64_t>::const_iterator it = rwd_map.begin(); it != rwd_map.end(); ++it)
		{
			m_UnitRwdMap[it->first] = it->second;
		}
	}
}

void LC_ClientAlliance::ReqDrawUnitRwd(int iType)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;
	MG_Req_Alliance_Unit_Gain reqMsg;
	reqMsg.m_iType = iType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_ALLIANCE_UNIT_GAIN_REQ, &reqMsg);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientAlliance::RltAllianceInfo(int groupID, Protocol::UnitServerNodeList& li)
{
	m_groupID = groupID;
	m_serverList.clear();
	m_serverList.resize(li.size());
	int nIndex = 0;
	for(UnitServerNodeList::iterator ite = li.begin(); ite != li.end(); ++ite)
	{
		ServerNode& rNode = m_serverList[nIndex++];
		rNode.serverID = ite->m_nServerID;
		rNode.serverName = STDSTR2TPSTR(ite->m_sServerName);
	}
}

void LC_ClientAlliance::ReUse()
{
	m_groupID = 0;
	m_serverList.clear();
	m_iWQGainTime = 0;
	m_iGainServerID = 0;
	m_UnitRwdMap.clear();

	m_iFactionWinTime = 0;
	m_iFactionDefServer = 0;
	m_iFactionDefStatus = 0;
}

int LC_ClientAlliance::GetCount()
{
	return m_serverList.size();
}

StringType LC_ClientAlliance::GetServerName( int iIndex )
{
	if ( iIndex >= 0 && iIndex < m_serverList.size()) 
	{
		return m_serverList[iIndex].serverName;
	}
	else
	{
		return "";
	}
}

StringType LC_ClientAlliance::GetGuildName(int32_t serverId)
{
	GuildWinfoList::iterator fd = m_GuildInfoList.find(serverId);
	if (fd != m_GuildInfoList.end())
	{
		return (fd->second).m_GuildName.c_str();
	}
	else
	{
		return "";
	}
}

StringType GameLogic::LC_ClientAlliance::GetGuildLeader( int32_t serverId )
{
	GuildWinfoList::iterator fd = m_GuildInfoList.find(serverId);
	if (fd != m_GuildInfoList.end())
	{
		return (fd->second).m_GuildOwnerName.c_str();
	}
	else
	{
		return "";
	}
}

StringType GameLogic::LC_ClientAlliance::GetWinGuildName()
{
	return m_winInfo.m_GuildName.c_str();
}

int GameLogic::LC_ClientAlliance::GetServerID( int iIndex )
{
	if ( iIndex >= 0 && iIndex < m_serverList.size()) 
	{
		return m_serverList[iIndex].serverID;
	}
	else
	{
		return -1;
	}
}

void GameLogic::LC_ClientAlliance::ReqEnterAlliance( int serverID , int transferAreaID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	if(!pkGamePlayer->CheckCanTransferMap(transferAreaID))
		return;

	MG_Req_Alliance_Enter_CrossMap msg;
	msg.m_nServerID = serverID;
	msg.m_nAreaID = transferAreaID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_ALLIANCE_ENTER_CROSS_MAP_REQ, &msg);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

}

void GameLogic::LC_ClientAlliance::RltEnterAlliance( int retType )
{
}

void GameLogic::LC_ClientAlliance::ReqAllianceWinInfo()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_FALLIANCE_GUILD_WIN_INFO_REQ, NULL);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void GameLogic::LC_ClientAlliance::RltAllianceWinInfo(Protocol::MG_SS_GUILD_WAR_WIN_INFO& msg)
{
	 m_winInfo = msg.m_crossWin;
	 for (std::vector<ST_GUILD_WIN_Node>::iterator ite = msg.m_winList.begin(); ite != msg.m_winList.end(); ++ite)
	 {
		 m_GuildInfoList.insert(pair<int32_t, ST_GUILD_WIN_Node>(ite->m_ServerId, (*ite)));
	 }
}

bool GameLogic::LC_ClientAlliance::HasFactionReward()
{
	if (m_iFactionWinTime <=0)
	{
		return false;
	}
	std::map<uint32_t,uint64_t>::iterator it = m_UnitRwdMap.find(E_RewardGain_FactionWar);
	if(it != m_UnitRwdMap.end())
	{
		uint64_t nGainTime = it->second;
		if(nGainTime == m_iFactionWinTime)
		{
			return false;
		}
	}

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	if (!pkLogicManger)
	{
		return false;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return false;

	int myServerID = pkGamePlayer->GetServerID();
	if(m_iFactionDefServer == myServerID && m_iFactionDefStatus == FactionWar_DefWin)
	{
		return true;
	}
		
	if (m_iFactionDefServer != myServerID && m_iFactionDefStatus == FactionWar_DefLose)
	{
		return true;
	}
	
	return false;
}