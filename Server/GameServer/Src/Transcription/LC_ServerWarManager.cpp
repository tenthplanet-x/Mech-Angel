#include "GameServerApp.h"
#include "time.h"
#include "UT_ServerHelper.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerGuildManager.h"
#include "LC_ServerWarManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "CF_ForcesBattle.h"
#include "LC_ServerMapLogic_WarPlay.h"
#include <boost/algorithm/string/replace.hpp>
#include "CF_WorldMapList.h"
#include "CF_GuildWarCrontab.h"
#include "LC_CrossRealmRaidManager.h"
#include "CF_GuildWarActivity.h"
#include "LC_GameStory_Manager.h"
using namespace GameLogic;
using namespace Skill;
using namespace Protocol;
using namespace CSVFile;
using namespace Utility;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LC_GuildWarCron::Init(int nType)
{
	CF_GuildWarCrontab::DataEntryMapExternIterator beg = CF_GuildWarCrontab::Instance()->GetIterator();
	while (beg.HasMoreElements())
	{
		CF_GuildWarCrontab::DataEntry* pkEntry = beg.PeekNextValuePtr();
		if (/*pkEntry != NULL &&*/ pkEntry->_iType == nType)
		{
			m_nCronIndex = pkEntry->_iID;
			break;
		}
		beg.MoveNext();
	}
}

bool _getWarBaseTime(int32_t iWarType, int64_t& iStartTime, int64_t& iConflateTime)
{
	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	switch(iWarType)
	{
	case WarType_CrossRealmGuildWar:
		{
			const msg_define::game_init_info& rkGameInit = pkServerMgr->GetGameInitInfo();
			if (rkGameInit.time_infos_size() <= 0)
			{
				return false;
			}

			uint32_t startTime = 0;
			uint32_t conflateTime = 0;
			for (int i=0; i<rkGameInit.time_infos_size(); ++i)
			{
				const msg_define::server_time_info& rkTimeInfo = rkGameInit.time_infos(i);
				if (startTime<=0 || rkTimeInfo.start_time()<startTime)
				{
					startTime = rkTimeInfo.start_time();
				}
				if (conflateTime<=0 || rkTimeInfo.conflate_time()>conflateTime)
				{
					conflateTime = rkTimeInfo.conflate_time();
				}
			}
			iStartTime = LC_Helper::GetDayTimeStamp(startTime, 0, 0, 0);
			iConflateTime = LC_Helper::GetDayTimeStamp(conflateTime, 0, 0, 0);
		}
		break;
	case WarType_GuildWar:
	case WarType_DomainWar:
		{
			iStartTime = LC_Helper::GetDayTimeStamp(pkServerMgr->GetServerStartTime(), 0, 0, 0);
			iConflateTime = LC_Helper::GetDayTimeStamp(pkServerMgr->GetServerConflateTime(), 0, 0, 0);
		}
		break;
	default:
		return false;
		break;
	}
	return true;
}
int64_t getCommonValidTime(int64_t nStartStamp, const StartTime& rStartInt, int64_t nMergeStamp, const StartTime& rMergeInt)
{
	int64_t startCoolTime = nStartStamp + rStartInt.GetTimeStramp();
	int64_t conflatetCoolTime = nMergeStamp + rMergeInt.GetTimeStramp();
	return startCoolTime>conflatetCoolTime ? startCoolTime : conflatetCoolTime;
}
int64_t getNearbyTime(int64_t nCurStamp, int64_t nStartStamp, const StartTime& rStartInt, const Utility::UT_CronData& rCronTurn, const StartTime& rInterval)
{
	if (nStartStamp > 0 && rInterval.GetTimeStramp() > 0 && nStartStamp + rInterval.GetTimeStramp() > nCurStamp)
	{
		int64_t nTimeS = rStartInt.GetTimeStramp() + nStartStamp;
		if (nTimeS >= nCurStamp)
		{
			return nTimeS;
		}
		if(rCronTurn.IsValid())
		{
			return rCronTurn.GetNextTime(nCurStamp);
		}
	}
	return 0;
}

time_t LC_GuildWarCron::getNextTime(int32_t iWarType, time_t nCurStamp)
{
	CF_GuildWarCrontab::DataEntry* pkEntry = CF_GuildWarCrontab::Instance()->GetEntryPtr(m_nCronIndex);
	if (pkEntry == NULL)
	{
		return 0;
	}

	int64_t uiStartTime = 0;
	int64_t uiConflateTime = 0;
	_getWarBaseTime(iWarType, uiStartTime, uiConflateTime);
	int64_t uiCommonValidTime = getCommonValidTime(uiStartTime, pkEntry->_iOpenInterval, uiConflateTime, pkEntry->_iMergeInterval);
	//开服时间
	time_t nTmpTime = (time_t)getNearbyTime(nCurStamp, LC_Helper::GetDayTimeStamp(uiStartTime, 0, 0, 0),
		pkEntry->_iOpenComTime, pkEntry->_iOpenTurnTime, pkEntry->_iOpenInterval);
	if (nTmpTime <= 0)
	{
		//合服时间
		nTmpTime = (time_t)getNearbyTime(nCurStamp, LC_Helper::GetDayTimeStamp(uiConflateTime, 0, 0, 0),
			pkEntry->_iMergeComTime, pkEntry->_iMergeTurnTime, pkEntry->_iMergeInterval);
	}

	if (nTmpTime <= 0)
	{
		//正常时间
		nTmpTime = pkEntry->_iComTime.GetNextTime(nCurStamp<uiCommonValidTime ? uiCommonValidTime : nCurStamp);
	}
	return nTmpTime;
}


time_t getPreviousTime(time_t nCurStamp, time_t nStartStamp, const StartTime& rStartInt, const Utility::UT_CronData& rCronTurn, const StartTime& rInterval)
{
	if (nStartStamp <= 0 || rInterval.GetTimeStramp() <= 0)
	{
		return 0;
	}
	//最后一次触发时间
	time_t nMaxPrev = nStartStamp + rInterval.GetTimeStramp();
	time_t nNextCom = nStartStamp + rStartInt.GetTimeStramp();		//第一次触发
	time_t nMorePrev = nNextCom;
	if(rCronTurn.IsValid())
	{
		time_t nTurnPrev = rCronTurn.GetPrevTime(nCurStamp);
		if (nNextCom < nTurnPrev)
		{
			nMorePrev = nTurnPrev;
		}
	}
	if(nMorePrev > nMaxPrev || nMaxPrev > nCurStamp)
	{
		return 0;
	}
	return nMorePrev;
}

time_t LC_GuildWarCron::getPrevTime(int32_t iWarType, time_t nCurStamp)
{
	CF_GuildWarCrontab::DataEntry* pkEntry = CF_GuildWarCrontab::Instance()->GetEntryPtr(m_nCronIndex);
	if (pkEntry == NULL)
	{
		return 0;
	}

	int64_t uiStartTime = 0;
	int64_t uiConflateTime = 0;
	_getWarBaseTime(iWarType, uiStartTime, uiConflateTime);
	int64_t uiCommonValidTime = getCommonValidTime(uiStartTime, pkEntry->_iOpenInterval, uiConflateTime, pkEntry->_iMergeInterval);

	time_t nPrevTime = pkEntry->_iComTime.GetPrevTime(nCurStamp);
	if (nPrevTime < uiCommonValidTime)
	{
		nPrevTime = 0;
	}

	if (nPrevTime <= 0)
	{
		//合服时间
		nPrevTime = getPreviousTime(nCurStamp, uiConflateTime,
			pkEntry->_iMergeComTime, pkEntry->_iMergeTurnTime, pkEntry->_iMergeInterval);
	}

	//开服时间
	if (nPrevTime <= 0)
	{
		nPrevTime = getPreviousTime(nCurStamp, uiStartTime,
			pkEntry->_iOpenComTime, pkEntry->_iOpenTurnTime, pkEntry->_iOpenInterval);
	}

	return nPrevTime;
}


void _getTimeList(std::vector<uint32_t>& rList, uint32_t nCurTime, uint32_t nBaseTime, const StartTime& rStartInt, const Utility::UT_CronData& rCronTurn, const StartTime& rInterval)
{
	rList.push_back((uint32_t)getPreviousTime(nCurTime, nBaseTime, rStartInt, rCronTurn, rInterval));
	uint32_t nTmpTime = nCurTime;
	while ( rList.size() < 6)
	{
		nTmpTime = (uint32_t)getNearbyTime(nTmpTime + 1, nBaseTime, rStartInt, rCronTurn, rInterval);
		if(nTmpTime == 0)
		{
			break;
		}
		rList.push_back(nTmpTime);
	}
}

void LC_GuildWarCron::fillNextTimeList(int32_t iWarType, uint32_t& nCurTime, uint32_t& nBaseTime, int nType, std::vector<uint32_t>& rList)
{
	CF_GuildWarCrontab::DataEntry* pkEntry = CF_GuildWarCrontab::Instance()->GetEntryPtr(m_nCronIndex);
	if (pkEntry == NULL)
	{
		return;
	}
	rList.clear();
	if(nCurTime == 0)
	{
		nCurTime = (uint32_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
	}
	//取的6个时间，前一个，后5个
	int64_t uiStartTime = 0;
	int64_t uiConflateTime = 0;
	_getWarBaseTime(iWarType, uiStartTime, uiConflateTime);
	int64_t uiCommonValidTime = getCommonValidTime(uiStartTime, pkEntry->_iOpenInterval, uiConflateTime, pkEntry->_iMergeInterval);
	rList.reserve(6);
	switch(nType)
	{
	case E_GuildWarCronTime_Open:
		{
			if(nBaseTime == 0)
			{
				nBaseTime = (uint32_t)uiStartTime;
			}
			_getTimeList(rList, nCurTime, nBaseTime, pkEntry->_iOpenComTime, pkEntry->_iOpenTurnTime, pkEntry->_iOpenInterval);
		}
		break;
	case E_GuildWarCronTime_Merge:
		{
			if(nBaseTime == 0)
			{
				nBaseTime = (uint32_t)uiConflateTime;
			}
			_getTimeList(rList, nCurTime, nBaseTime, pkEntry->_iMergeComTime, pkEntry->_iMergeTurnTime, pkEntry->_iMergeInterval);
		}
		break;
	case E_GuildWarCronTime_Common:
		{
			if(nBaseTime == 0)
			{
				nBaseTime = (uint32_t)uiCommonValidTime;
			}
			int64_t nTmpTime = nCurTime<uiCommonValidTime ? uiCommonValidTime : nCurTime;
			int64_t nPrevTime = pkEntry->_iComTime.GetPrevTime(nTmpTime);
			if (nPrevTime >= uiCommonValidTime)
			{
				rList.push_back((uint32_t)nPrevTime);
			}
			while ( rList.size() < 6)
			{
				nTmpTime = pkEntry->_iComTime.GetNextTime(nTmpTime + 1);
				rList.push_back((uint32_t)nTmpTime);
			}
		}
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GuildWarUnit::GuildWarUnit(): m_Config(), m_WarState(GuildWarState_None), m_NoticeIndex(0), m_TimeStamp(0), m_CurMap(), m_WarCron(), m_WarType(WarType_None) {}

GuildWarUnit::~GuildWarUnit()
{

}

void GuildWarUnit::Clear()
{
	m_WarState = GuildWarState_None;
	m_NoticeIndex = 0;
	m_TimeStamp = 0;
	m_CurMap.Clear();
}

bool GuildWarUnit::Init(int nWarType, const StringType& rPath)
{
	Clear();
	m_WarType = nWarType;
	m_WarCron.Init(nWarType);
	return m_Config.Init(rPath);
}

void GuildWarUnit::UnInit()
{
	Clear();
}

void GuildWarUnit::PrepareCurPower()
{
	if (m_WarState == GuildWarState_None)
	{
		PrepareNearWar();
	}
}

void GuildWarUnit::PrepareNearWar()
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	time_t powerstamp = 0;
	Protocol::PS_PowerGuildInfo* powerguild = pkGuildManager->GetCurLocalPowerGuildInfo();
	if (powerguild != NULL)
	{
		powerstamp = powerguild->m_TimeStamp;
	}
	time_t Curstamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
	time_t Nextstamp = m_WarCron.getNextTime(m_WarType, Curstamp);
	time_t Prevstamp = m_WarCron.getPrevTime(m_WarType, Curstamp);
	int    timeLen = m_Config.GuildWarTime.TimeLen;
	do
	{
		//开启剩余时间超过20分钟的活动
		if (Curstamp > Prevstamp && Prevstamp + timeLen - 1200 >= Curstamp)
		{
			StartWar(Prevstamp);
			break;
		}
		//准备开启下一轮活动
		if (Nextstamp >= Curstamp)
		{
			if (Nextstamp == Curstamp)
			{
				StartWar(Curstamp);
			}
			else
			{
				SetNotice(Nextstamp - Curstamp, Curstamp);
			}
		}
	}while (false);
}

void GuildWarUnit::SetNotice(time_t span, time_t curtimestamp)
{
	if (span < 0)
	{
		return;
	}
	m_NoticeIndex = 0;
	m_WarState = GuildWarState_Peace;
	NoticeSetType::iterator beg = m_Config.GuildWarTime.Notice.begin();
	for (; beg != m_Config.GuildWarTime.Notice.end(); ++beg, ++m_NoticeIndex)
	{
		if (span >= beg->TimeSpan)
		{
			break;
		}
	}
	if (beg == m_Config.GuildWarTime.Notice.end())
	{
		m_TimeStamp = curtimestamp + span;
	}
	else
	{
		m_TimeStamp = curtimestamp + (span - beg->TimeSpan);
	}
}

void GuildWarUnit::GetWarTimeStamp(time_t& beginstamp, time_t& endstamp)
{
	if (m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner)
	{
		beginstamp = m_TimeStamp - m_Config.GuildWarTime.TimeLen;
	}
	else if (m_WarState == GuildWarState_Peace)
	{
		if (m_NoticeIndex >= m_Config.GuildWarTime.Notice.size())
		{
			beginstamp = m_TimeStamp;
		}
		else
		{
			beginstamp = m_TimeStamp + m_Config.GuildWarTime.Notice[m_NoticeIndex].TimeSpan;
		}
	}
	endstamp = beginstamp + m_Config.GuildWarTime.TimeLen;
}

ResultType GuildWarUnit::EnterGuildWar(LC_ServerPlayer* player)
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return RE_GUILDWAR_NOT_START;
	}
	if (!m_CurMap.m_Vaild)
	{
		return RE_GUILDWAR_NOT_START;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	return pkWorldManager->RequestChangePlayerLocation(player, LMIOT_TRANSFER, m_CurMap.m_MapLogicId, m_CurMap.m_MapResid, UT_Vec3Int());
}

void GuildWarUnit::BroadCast(const StringType& notifystr, bool allserver, int logicid)
{
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	bool res = UT_ServerHelper::GetGameNoticeFromString(notifystr, noticeFmort);
	if(!res)
	{
		return;
	}
	noticeFmort.SetChannelType(PROMPT_CHANNEL_ACTIVITY_TIP);

	MG_GameNotice noticeMsg;
	UT_ServerHelper::FillGameNoticeMsg(noticeFmort, noticeMsg);	

	if (allserver)
	{
		BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg, false);
	}
	else
	{
		LC_ServerMapLogic_Place* thelogic = GetMapLogic(logicid);
		if (thelogic != NULL)
		{
			thelogic->BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg);
		}
	}
}

void GuildWarUnit::BroadCastGroup(const StringType& notifystr, bool allserver, int logicid)
{
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	bool res = UT_ServerHelper::GetGameNoticeFromString(notifystr, noticeFmort);
	if(!res)
	{
		return;
	}
	noticeFmort.SetChannelType(PROMPT_CHANNEL_ACTIVITY_TIP);

	MG_GameNotice noticeMsg;
	UT_ServerHelper::FillGameNoticeMsg(noticeFmort, noticeMsg);	

	if (allserver)
	{
		SendMsgToChat(MGPT_GAME_NOTICE_MSG, &noticeMsg, 0, 0, UT_ServerHelper::GetServerId(), 0, MG_SERVER_ROUTER_FLAG_GROUP);
	}
	else
	{
		LC_ServerMapLogic_Place* thelogic = GetMapLogic(logicid);
		if (thelogic != NULL)
		{
			thelogic->BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg);
		}
	}
}

int32_t GuildWarUnit::GetWarActivityID(uint64_t nTime)
{
	if(nTime == 0)
	{
		nTime = m_TimeStamp;
	}

	uint64_t nStartTime = (uint64_t)LC_Helper::GetDayTimeStamp(GetGameServerApp()->GetServerManager()->GetServerStartTime(), 0, 0, 0);
	uint64_t nMergeTime = (uint64_t)LC_Helper::GetDayTimeStamp(GetGameServerApp()->GetServerManager()->GetServerConflateTime(), 0, 0, 0);

	CF_GuildWarActivity* pWarActivity = CF_GuildWarActivity::GetSingletonPtr();
	{
		CF_GuildWarActivity::DataEntryMapExternIterator iter = pWarActivity->GetIterator();
		while(iter.HasMoreElements())
		{
			CF_GuildWarActivity::DataEntry * entry = iter.PeekNextValuePtr();
			if(/*entry != NULL &&*/ entry->_iType == m_WarType && 
				LC_Helper::IsCommTimeViable(nTime, nStartTime, nMergeTime, entry->_beginTime.GetTimeStramp(), entry->_endTime.GetTimeStramp(), 
				entry->_coldSTime, entry->_beginSTime, entry->_endSTime, entry->_coldMTime, entry->_beginMTime, entry->_endMTime))
			{
				return entry->_iActivity;
			}
			iter.MoveNext();
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GuildWarLocal::GuildWarLocal(): GuildWarUnit() {}
GuildWarLocal::~GuildWarLocal() {}

bool GuildWarLocal::Init(int nWarType, const StringType& rPath)
{
	if (GuildWarUnit::Init(nWarType, rPath) == false)
	{
		return false;
	}
	if (m_Config.GuildMap.IsVaild() == false)
	{
		return false;
	}
	return true;
}

void GuildWarLocal::Update(float fCurrentTime, float fDeltaTime)
{
	if (m_WarState == GuildWarState_None)
	{
		return ;
	}
	else if (m_WarState == GuildWarState_Peace)
	{
		time_t curtimestamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if (curtimestamp < m_TimeStamp)
		{
			return;
		}
		size_t noticesize = m_Config.GuildWarTime.Notice.size();
		if (m_NoticeIndex >= noticesize)
		{
			StartWar(m_TimeStamp);
			return;
		}
		else
		{
			BroadCast(m_Config.GuildWarTime.Notice[m_NoticeIndex++].NoticeStr.c_str(), false, true);
			if (m_NoticeIndex == noticesize)
			{
				m_TimeStamp = m_TimeStamp + m_Config.GuildWarTime.Notice[m_NoticeIndex - 1].TimeSpan;
			}
			else
			{
				m_TimeStamp = m_TimeStamp + m_Config.GuildWarTime.Notice[m_NoticeIndex - 1].TimeSpan - m_Config.GuildWarTime.Notice[m_NoticeIndex].TimeSpan;
			}
		}
	}
	else
	{
		time_t curtimestamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if (curtimestamp >= m_TimeStamp)
		{
			EndWar();
		}
	}
}

void GuildWarLocal::StartWar(time_t startTimestamp)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	int curpow = INVALID_GUILD_ID;
	Protocol::PS_PowerGuildInfo* powerguild = pkGuildManager->GetCurLocalPowerGuildInfo();
	if (powerguild != NULL)
	{
		curpow = powerguild->m_GuildId;
	}

	m_CurMap.Clear();
	m_WarState = GuildWarState_War_Ownerless;
	m_TimeStamp = startTimestamp + m_Config.GuildWarTime.TimeLen;
	//选取地图
	GuildMapEntry& rMapEntry = m_Config.GuildMap;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, rMapEntry.MapResId, MT_GUILDWAR);
	if (logic == NULL)
	{
		return;
	}
	LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)logic->GetPlay(PlayType_War);
	if (play == NULL)
	{
		return;
	}
	play->SetWarType(WarType_GuildWar);
	logic->StartPlay();
	m_CurMap.m_Vaild = true;
	m_CurMap.m_MapLogicId = logic->GetMapLogicID();
	m_CurMap.m_MaxPlayer = rMapEntry.MaxPlayer;
	m_CurMap.m_MapResid = rMapEntry.MapResId;
	//清除原有盟主
	MG_SS_LocalGuildWar_Start startmsg;
	startmsg.m_CurrentPowerTimeStamp = startTimestamp;
	SendMsgToSociety(MGPT_GGU_LOCAL_GUILDWAR_START, &startmsg);

	LC_ServerPlayer player;
	//系统公告
	BroadCast(m_Config.GuildWarTime.StartNotice.c_str(), false, true);

	LC_ServerPlayerManager* pkServerPlayerMgr =	SERVER_GET_PLAYER_MANAGER();
	const LC_PlayerIDPtrMap& allPlayers = pkServerPlayerMgr->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator cit = allPlayers.begin();
	for (; cit != allPlayers.end(); ++cit)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)cit->second;
		if (pkServerPlayer && pkServerPlayer->GetLevel() >= GUILDWAR_MINLEVEL && !UT_ServerHelper::IsInCross((pkServerPlayer)))
		{
			pkServerPlayer->SendMsgToClient(MGPT_NOTIFY_POWERGUILDPLAYER_BEGIN);
		}
	}

	OnWarBegin();
}

void GuildWarLocal::Order(int delaytime)
{
	if (m_WarState == GuildWarState_War_Owner || m_WarState == GuildWarState_War_Ownerless)
	{
		return;
	}
	time_t curtimestamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if (delaytime <= 0)
	{
		StartWar(curtimestamp);
	}
	else
	{
		SetNotice(delaytime, curtimestamp);
	}
	//设置开始时间
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	time_t begtime = 0;
	time_t endtime = 0;
	GetWarTimeStamp(begtime, endtime);
	pWorldGuildWarAsset->SetTimeStamp(begtime, endtime);
}

bool GuildWarLocal::CharEnter(LC_ServerPlayer* player)
{
	if ((m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner) && player->GetLevel() >= GUILDWAR_MINLEVEL && !UT_ServerHelper::IsInCross(player))
	{
		player->SendMsgToClient(MGPT_NOTIFY_POWERGUILDPLAYER_BEGIN);
		return false;
	}
	return true;
}

ResultType GuildWarLocal::EnterGuildWar(LC_ServerPlayer* player)
{
	if (UT_ServerHelper::IsInCross(player))
	{
		return RE_ON_CROSS_SERVER;
	}
	return GuildWarUnit::EnterGuildWar(player);
}

LC_ServerMapLogic_Place* GuildWarLocal::GetMapLogic(int logicid)
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return NULL;
	}
	if (!m_CurMap.m_Vaild)
	{
		return NULL;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	return pkWorldManager->GetMapLogic(m_CurMap.m_MapLogicId);
}


void GuildWarLocal::PrepareNearWar()
{
	GuildWarUnit::PrepareNearWar();
	//设置开始时间
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	time_t begtime = 0;
	time_t endtime = 0;
	GetWarTimeStamp(begtime, endtime);
	pWorldGuildWarAsset->SetTimeStamp(begtime, endtime);
}

void GuildWarLocal::NotifyPowerEnd(uint64_t power, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId)
{
	time_t startstamp = 0;
	time_t endstamp = 0;
	GetWarTimeStamp(startstamp, endstamp);
	if (m_WarState == GuildWarState_War_Ownerless)
	{
		m_WarState = GuildWarState_War_Owner;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	pkGuildManager->ReSetLocalPower(power, startstamp, idList);
	LC_ServerGuildInfo* powerguild = pkGuildManager->GetGuildInfoByID(power);
	if (powerguild == NULL)
	{
		BroadCast(m_Config.GuildWarTime.FailNotify.c_str(), false, true);
	}
	else
	{
		StringType successnotify = m_Config.GuildWarTime.SuccessNotify.c_str();
		StringType playername, playerPosition;
		powerguild->GetMasterName(playername);
		powerguild->GetMasterPosition(playerPosition);
		successnotify += "s:" + powerguild->GetName() + ";s:" + playerPosition + ";s:" + playername + ";";
		BroadCast(successnotify.c_str(), false, true);
		//给所有的帮会成员发送奖励
		powerguild->PowerGuildReward();
	}
	OnWarEnd(powerguild, startstamp);
}

void GuildWarLocal::OnWarBegin()
{
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	pWorldGuildWarAsset->ClearLocalWarFightCapacity();
}

void GuildWarLocal::OnWarEnd(LC_ServerGuildInfo* pGuild, time_t timeStamp)
{
	if(pGuild == NULL)
	{
		return;
	}
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	Guild_Member_Ptr_List rMemberList = pGuild->GetEldestMemberList();
	if(rMemberList.empty() == true)
	{
		return;
	}
	LocalWarAsset::fightCapacityList rListFinal, rListTemp[GUILD_MEMBER_ELDER+1];
	for(Guild_Member_Ptr_List::iterator iter = rMemberList.begin(); iter != rMemberList.end(); iter ++)
	{
		GuildServerMember* pMember = *iter;
		if(pMember == NULL || pMember->m_Position <= 0 || pMember->m_Position > GUILD_MEMBER_ELDER)
		{
			continue;
		}
		rListTemp[pMember->m_Position].push_back(pMember->m_Battle);
	}

	rListFinal.resize(5);
	{
		LocalWarAsset::fightCapacityList& rList = rListTemp[GUILD_MEMBER_MASTER];
		if(rList.size() > 0)
		{
			rListFinal[0] = rList[0];
		}
	}
	{
		LocalWarAsset::fightCapacityList& rList = rListTemp[GUILD_MEMBER_GUARDIAN];
		if(rList.size() > 0)
		{
			rListFinal[1] = rList[0];
		}
		if(rList.size() > 1)
		{
			rListFinal[2] = rList[1];
		}
	}
	{
		LocalWarAsset::fightCapacityList& rList = rListTemp[GUILD_MEMBER_ELDER];
		if(rList.size() > 0)
		{
			rListFinal[3] = rList[0];
		}
		if(rList.size() > 1)
		{
			rListFinal[4] = rList[1];
		}
	}
	pWorldGuildWarAsset->SetLocalWarFightCapacity(rListFinal, timeStamp);
}

void GuildWarLocal::EndWar()
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return;
	}
	if (m_CurMap.m_Vaild)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(m_CurMap.m_MapLogicId);
		if (logic == NULL)
		{
			return;
		}
		logic->EndAllPlay();
	}
	m_CurMap.m_Vaild = false;
	if (!m_Config.GuildWarTime.EndNotify.empty())
	{
		BroadCast(m_Config.GuildWarTime.EndNotify.c_str(), false, true);
	}
	m_WarState = GuildWarState_Peace;
	PrepareNearWar();
}

void GuildWarLocal::ForceEndWar()
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return;
	}
	if (m_CurMap.m_Vaild)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(m_CurMap.m_MapLogicId);
		if (logic != NULL)
		{
			logic->EndAllPlay();
			logic->Finsh();
		}
	}
	m_CurMap.m_Vaild = false;
	m_WarState = GuildWarState_Peace;
	PrepareNearWar();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GuildWarCross::GuildWarCross(): GuildWarUnit(), m_tStartTimeStamp(0), m_tEndTimeStamp(0), m_bIgnoreResult(false) {}

GuildWarCross::~GuildWarCross(){}

void GuildWarCross::Clear()
{
	GuildWarUnit::Clear();
	m_tStartTimeStamp = m_tEndTimeStamp = 0;
	m_bIgnoreResult = false;
}

bool GuildWarCross::Init(int nWarType, const StringType& rPath)
{
	if (GuildWarUnit::Init(nWarType, rPath) == false)
	{
		return false;
	}
	if (m_Config.GuildMap.IsVaild() == false)
	{
		return false;
	}
	m_tStartTimeStamp = m_tEndTimeStamp = 0;
	m_bIgnoreResult = false;
	return true;
}

void GuildWarCross::Update(float fCurrentTime, float fDeltaTime)
{
	if (m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner)
	{
		time_t curtimestamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if (curtimestamp >= m_tEndTimeStamp + 600)
		{
			EndWar();
		}
	}
}

bool GuildWarCross::CharEnter(LC_ServerPlayer* player)
{
	if ((m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner) && player->GetLevel() >= CROSSREALMGUILDWAR_MINLEVEL && !UT_ServerHelper::IsInCross(player))
	{
		player->SendMsgToClient(MGPT_NOTIFY_GLOBALPOWERGUILDPLAYER_BEGIN);
		return false;
	}
	return true;
}

LC_ServerMapLogic_Place* GuildWarCross::GetMapLogic(int logicid)
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return NULL;
	}
	if (!m_CurMap.m_Vaild)
	{
		return NULL;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	return pkWorldManager->GetMapLogic(m_CurMap.m_MapLogicId);
}

void GuildWarCross::NotifyPowerEnd(uint64_t power, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId)
{
	if (m_WarState == GuildWarState_War_Ownerless)
	{
		m_WarState = GuildWarState_War_Owner;
	}
	if (m_bIgnoreResult)
	{
		return;
	}
	StringType playername, playerPosition;
	LC_ServerGuildInfo* powerguild = LC_ServerGuildManager::GetSingletonPtr()->GetGuildInfoByID(power);
	if (powerguild != NULL)
	{
		powerguild->GetMasterName(playername);
		powerguild->GetMasterPosition(playerPosition);
		//给所有的帮会成员发送奖励
		powerguild->GlobalPowerGuildReward();
	}
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}

	MG_SS_CrossRealmGuildWar_SetPower msg;
	msg.m_GroupId = pWorldGuildWarAsset->GetGroupId();
	msg.m_PowerGuildId = power;
	msg.m_WarTimestamp = m_tStartTimeStamp;
	TPSTR2STDSTR(playername, msg.m_PlayerName);
	TPSTR2STDSTR(playerPosition, msg.m_PlayerPosition);
	LC_Helper::CopyStructProtocolMem_List(idList, msg.m_JoinCharIDList);
	msg.m_nActivity = GetWarActivityID(m_tStartTimeStamp);
	SendMsgToSociety(MGPT_GGU_CROSSREALM_GUILDWAR_SET_POWER, &msg);

	GfxWriteFmtLog(LOG_GUILD_FILELINE, LOG_SWITCH_GUILD, "require setCrossRealmPower", power, msg.m_GroupId, m_tStartTimeStamp);
}

void GuildWarCross::EndWar()
{
	if (m_WarState == GuildWarState_None)
	{
		return;
	}
	if (m_CurMap.m_Vaild)
	{
		WarPlaceEnd(m_tStartTimeStamp, true, false);
	}
	else
	{
		EndWar(m_tStartTimeStamp);
	}
}

void GuildWarCross::Order(int delaytime)
{
	if (m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner)
	{
		return;
	}
	MG_SS_CrossRealmGuildWar_Order order;
	order.m_GroupId = GetGameServerApp()->GetServerManager()->GetCurGuildGroupId();
	order.m_TimeDelay = delaytime;
	SendMsgToSociety(MGPT_GGU_CROSSREALM_GUILDWAR_ORDER_REQ, &order);
}

void GuildWarCross::ForceEndWar()
{
	// 	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	// 	{
	// 		return;
	// 	}
	MG_SS_CrossRealmGuildWar_ForceEnd endmsg;
	endmsg.m_GroupId = GetGameServerApp()->GetServerManager()->GetCurGuildGroupId();
	SendMsgToSociety(MGPT_GGU_CROSSREALM_GUILDWAR_FORCEEND_REQ, &endmsg);
}

void GuildWarCross::GetWarTimeStamp(time_t& beginstamp, time_t& endstamp)
{
	beginstamp = m_tStartTimeStamp;
	endstamp = m_tEndTimeStamp;
}

void GuildWarCross::StartWar(time_t starttime, time_t endtime)
{
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	pWorldGuildWarAsset->SetGlobalTimeStamp(starttime, endtime);
	m_WarState = GuildWarState_War_Ownerless;
	m_tStartTimeStamp = starttime;
	m_tEndTimeStamp = endtime;
	BroadCast(m_Config.GuildWarTime.StartNotice.c_str(), false, true);

	LC_ServerPlayerManager* pkServerPlayerMgr = SERVER_GET_PLAYER_MANAGER();
	const LC_PlayerIDPtrMap& allPlayers = pkServerPlayerMgr->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator cit = allPlayers.begin();
	for (; cit != allPlayers.end(); ++cit)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)cit->second;
		if (pkServerPlayer && pkServerPlayer->GetLevel() >= CROSSREALMGUILDWAR_MINLEVEL && !UT_ServerHelper::IsInCross((pkServerPlayer)))
		{
			pkServerPlayer->SendMsgToClient(MGPT_NOTIFY_GLOBALPOWERGUILDPLAYER_BEGIN);
		}
	}
	SERVER_GET_GAMESTORY_MANAGER->OnCrossWarTimeChange(starttime, 0);
}

void GuildWarCross::EndWar(time_t starttime)
{
	m_WarState = GuildWarState_None;
}

void GuildWarCross::Notice(int noticeindex)
{
	int noticesize = (int)m_Config.GuildWarTime.Notice.size();
	if (noticeindex >= noticesize)
	{
		return;
	}
	BroadCast(m_Config.GuildWarTime.Notice[noticeindex].NoticeStr.c_str(), false, true);
}

void GuildWarCross::SetWarTime(time_t starttime, time_t endtime, int32_t nState)
{
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	pWorldGuildWarAsset->SetGlobalTimeStamp(starttime, endtime);
	if(m_WarState != nState && (m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner))
	{
		EndWar();
	}
	m_tStartTimeStamp = starttime;
	m_tEndTimeStamp = endtime;
	m_WarState = (GuildWarState)nState;
	SERVER_GET_GAMESTORY_MANAGER->OnCrossWarTimeChange(0, starttime);
}

void GuildWarCross::SocietyShutDown()
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return;
	}
	if (m_CurMap.m_Vaild)
	{
		WarPlaceEnd(m_tStartTimeStamp, true, true);
	}
	m_WarState = GuildWarState_None;
}

void GuildWarCross::ReportWarParm()
{
	Protocol::MG_SS_CrossRealmGuildWar_Register info;
	info.m_groupId = GetGameServerApp()->GetServerManager()->GetCurGuildGroupId();
	ST_GuildWarConfigInfo& rWarConfig = info.m_warConfig;
	rWarConfig.m_DuringTime = m_Config.GuildWarTime.TimeLen;

	uint32_t nCurTime = (uint32_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
	m_WarCron.fillNextTimeList(m_WarType, nCurTime, rWarConfig.m_nOpenTime, LC_GuildWarCron::E_GuildWarCronTime_Open, rWarConfig.m_openTimeList);
	m_WarCron.fillNextTimeList(m_WarType, nCurTime, rWarConfig.m_nMergeTime, LC_GuildWarCron::E_GuildWarCronTime_Merge, rWarConfig.m_mergeTimeList);
	m_WarCron.fillNextTimeList(m_WarType, nCurTime, rWarConfig.m_nCommonTime, LC_GuildWarCron::E_GuildWarCronTime_Common, rWarConfig.m_commTimeList);

	NoticeSetType& rNoticeSet = m_Config.GuildWarTime.Notice;
	rWarConfig.m_noticeList.reserve(rNoticeSet.size());
	for (NoticeSetType::iterator beg = rNoticeSet.begin(); beg != rNoticeSet.end(); ++beg)
	{
		rWarConfig.m_noticeList.push_back(beg->TimeSpan);
	}

	ST_GuildWarMapConfig& rMapCfg = rWarConfig.m_mapCfgInfo;
	GuildMapEntry& rNode = m_Config.GuildMap;
	rMapCfg.clear();
	rMapCfg.m_mapResId = rNode.MapResId;
	rMapCfg.m_rateId = rNode.Rate;
	rMapCfg.m_maxPlayer = rNode.MaxPlayer;
	SendMsgToSociety(MGPT_GGU_CROSSREALM_GUILD_REGISTER, &info);
}

void GuildWarCross::WarPowerNotify(Protocol::MG_SS_CrossRealmGuildWar_PowerNotify& info)
{
	if (info.m_GuildId == INVALID_GUILD_ID)
	{
		BroadCast(m_Config.GuildWarTime.FailNotify.c_str(), false, true);
	}
	else
	{
		StringType successnotify = m_Config.GuildWarTime.SuccessNotify.c_str();
		StringType playername = STDSTR2TPSTR(info.m_PlayerName);
		StringType playerPosition = STDSTR2TPSTR(info.m_PlayerPosition);
		StringType guildname = STDSTR2TPSTR(info.m_GuildName);
		StringType servername;
		GetGameServerApp()->GetServerManager()->GetServerInfo(info.m_ServerId, servername);
		successnotify = boost::replace_all_copy(successnotify, "[SERVERNAME]", servername);
		successnotify = boost::replace_all_copy(successnotify, "[GUILDNAME]", guildname);
		successnotify = boost::replace_all_copy(successnotify, "[PLAYERNAME]", playername);
		successnotify = boost::replace_all_copy(successnotify, "[POSITION]", playerPosition);
		BroadCast(successnotify.c_str(), false, true);
	}
}

void GuildWarCross::WarPlaceStart(time_t starttime, time_t endtime, int groupid, int powerguildid, int64_t nRaidId)
{
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	if (GetGameServerApp()->GetServerManager()->GetCurServerType() != SERVER_TYPE_GAME)
	{
		pWorldGuildWarAsset->SetGroupId(groupid);
		pWorldGuildWarAsset->SetGlobalTimeStamp(starttime, endtime);
	}
	m_WarState = GuildWarState_War_Ownerless;
	m_tStartTimeStamp = starttime;
	m_tEndTimeStamp = endtime;
	m_bIgnoreResult = false;
	LC_ServerMapLogic_Place* logic = NULL;
	if(nRaidId > 0)
	{
		int nLogicId = 0;
		LC_CrossRealmRaidManager* pkManager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
		if(pkManager != NULL)
		{
			LC_CrossRealmPlaceRaidInfo* pRaidInfo = pkManager->GetCrossRealmPlaceRaid(nRaidId);
			if(pRaidInfo != NULL)
			{
				nLogicId = pRaidInfo->m_LogicId;
			}
		}
		logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(nLogicId);
	}
	if (logic == NULL)
	{
		logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, m_Config.GuildMap.MapResId, MT_GUILDWAR);
		if (logic == NULL)
			return;
		logic->SetCreateCross(true);
	}
	if(logic == NULL)
	{
		return;
	}
	logic->SetCrossRealm(true, CrossRealmType_GuildWar);
	LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)logic->GetPlay(PlayType_War);
	if (play == NULL)
	{
		return;
	}
	play->SetWarType(WarType_CrossRealmGuildWar);
	logic->StartPlay();
	m_CurMap.m_Vaild = true;
	m_CurMap.m_MapLogicId = logic->GetMapLogicID();
	m_CurMap.m_MaxPlayer = m_Config.GuildMap.MaxPlayer;
	m_CurMap.m_MapResid = m_Config.GuildMap.MapResId;
}

void GuildWarCross::WarPlaceEnd(time_t starttime, bool force, bool ignore)
{
	m_WarState = GuildWarState_None;
	m_bIgnoreResult = ignore;
	//重置GroupId
	if (GetGameServerApp()->GetServerManager()->GetCurServerType() != SERVER_TYPE_GAME)
	{
		LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
		{
			LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
			if(pWorldGuildWarAsset != NULL)
			{
				pWorldGuildWarAsset->SetGroupId(0);
			}
		}
	}
	if (m_CurMap.m_Vaild)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(m_CurMap.m_MapLogicId);
		if (logic != NULL)
		{
			if(logic->GetCreateCross() == false)
			{
				logic->EndPlay(PlayType_War);
			}
			else
			{
				if (force == true)
				{
					logic->Finsh();
				}
				else
				{
					logic->EndAllPlay();
				}
			}
		}
	}
	m_CurMap.Clear();
	m_CurMap.m_Vaild = false;
}

ResultType GuildWarCross::EnterGuildWar(LC_ServerPlayer* player)
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return RE_GUILDWAR_NOT_START;
	}
	if (!m_CurMap.m_Vaild)
	{
		MG_SS_CrossRealmGuildWar_GuildEnter_Req req;
		req.m_CharID = player->GetInstance();
		req.m_Userid = player->GetUserID();
		req.m_ActiveServerId = UT_ServerHelper::GetServerId();
		SendMsgToSociety(MGPT_GGU_CROSSREALM_GUILDWAR_GUILDENTER_REQ, &req);
		return RE_SUCCESS;
	}
	else
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		return pkWorldManager->RequestChangePlayerLocation(player, LMIOT_TRANSFER, m_CurMap.m_MapLogicId, m_CurMap.m_MapResid, UT_Vec3Int());
	}
}

ResultType GuildWarCross::AppointEnterGuildWar(object_id_type playerID, user_id_type userid)
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return RE_GUILDWAR_NOT_START;
	}
	if (!m_CurMap.m_Vaild)
	{
		return RE_GUILDWAR_NOT_START;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* thelogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(m_CurMap.m_MapLogicId);
	if (thelogic == NULL)
	{
		return RE_FAIL;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (m_CurMap.m_MaxPlayer == 0)
	{
		enterLeaveManager->AddCrossRealmGuildWarPlayerInfo(playerID, userid, m_CurMap.m_MapLogicId);
	}
	else
	{
		int playercount = thelogic->GetAllRealPlayerSize();
		int holdcount = enterLeaveManager->GetHoldCount(HoldDataType_CrossRealmGuildWar);
		int totalcount = playercount + holdcount;
		if (totalcount >= m_CurMap.m_MaxPlayer)
		{
			return RE_CROSSREALM_PLAYER_FULL;
		}
		enterLeaveManager->AddCrossRealmGuildWarPlayerInfo(playerID, userid, m_CurMap.m_MapLogicId);
	}
	return RE_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GuildWarDomain::GuildWarDomain():GuildWarUnit(), m_DomainMap() {}
GuildWarDomain::~GuildWarDomain() {}

void GuildWarDomain::Clear()
{
	GuildWarUnit::Clear();
	m_DomainMap.clear();
}

bool GuildWarDomain::Init(int nWarType, const StringType& rPath)
{
	if (GuildWarUnit::Init(nWarType, rPath) == false)
	{
		return false;
	}
	//加载所有势力地图
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	CF_ForcesBattle* obj = CF_ForcesBattle::GetSingletonPtr();
	CF_ForcesBattle::DataEntryMapExternIterator kIter = obj->GetIterator();
	bool find = false;
	while (kIter.HasMoreElements())
	{
		CF_ForcesBattle::DataEntry* pkData = kIter.PeekNextValuePtr();
		const LC_ServerMapLogicPlaceVector* pkAllMaps = pkWorldManager->GetMapLogicByRes(pkData->_MapResID);
		if (pkAllMaps && !pkAllMaps->empty())
		{
			LC_ServerMapLogicPlaceVector::const_iterator mapbeg = pkAllMaps->begin();
			for (; mapbeg != pkAllMaps->end(); ++mapbeg)
			{
				LC_ServerMapLogic_Place* pLogicPlace = *mapbeg;
				if (pLogicPlace != NULL && pLogicPlace->GetMapLogicType() == MT_NORMAL && pLogicPlace->getTributaryIndex() == pkData->_iLineID)
				{
					m_DomainMap.push_back(pLogicPlace);
					find = true;
					break;
				}
			}
		}
		kIter.MoveNext();
	}
	if (!find)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "LC_WarManager Init Domain War Failed!");
		return false;
	}
	return true;
}

void GuildWarDomain::Update(float fCurrentTime, float fDeltaTime)
{
	if (m_WarState == GuildWarState_None)
	{
		return;
	}
	else if (m_WarState == GuildWarState_Peace)
	{
		time_t curtimestamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if (curtimestamp < m_TimeStamp)
		{
			return;
		}
		size_t noticesize = m_Config.GuildWarTime.Notice.size();
		if (m_NoticeIndex >= noticesize)
		{
			StartWar(m_TimeStamp);
			return;
		}
		else
		{
			BroadCast(m_Config.GuildWarTime.Notice[m_NoticeIndex++].NoticeStr.c_str(), false);
			if (m_NoticeIndex == noticesize)
			{
				m_TimeStamp = m_TimeStamp + m_Config.GuildWarTime.Notice[m_NoticeIndex - 1].TimeSpan;
			}
			else
			{
				m_TimeStamp = m_TimeStamp + m_Config.GuildWarTime.Notice[m_NoticeIndex - 1].TimeSpan - m_Config.GuildWarTime.Notice[m_NoticeIndex].TimeSpan;
			}
		}
	}
	else
	{
		time_t curtimestamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if (curtimestamp >= m_TimeStamp)
		{
			EndWar();
		}
	}
}

void GuildWarDomain::StartWar(time_t startTimestamp)
{
	if (m_WarState == GuildWarState_War_Owner || m_WarState == GuildWarState_War_Ownerless)
	{
		return;
	}
	m_WarState = GuildWarState_War_Ownerless;
	m_TimeStamp = startTimestamp + m_Config.GuildWarTime.TimeLen;
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	const DomainPowerMap* thedomain = pWorldGuildWarAsset->GetCurDomainPowerGuildInfo();
	LC_ServerMapLogicPlaceVector::iterator beg = m_DomainMap.begin();
	for (; beg != m_DomainMap.end(); ++beg)
	{
		LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)(*beg)->GetPlay(PlayType_War);
		if (play == NULL)
		{
			continue;
		}
		if (thedomain == NULL)
		{
			play->Start();
		}
		else
		{
			DomainPowerMap::const_iterator it = thedomain->find((*beg)->GetMapResID());
			if (it == thedomain->end())
			{
				play->Start();
			}
			else if (it->second.m_TimeStamp < startTimestamp)
			{
				play->Start();
			}
		}
	}
	//清除原有盟主
	MG_SS_Domain_WarStartReq startmsg;
	startmsg.m_TimeStamp = startTimestamp;
	SendMsgToSociety(MGPT_GGU_REQ_START_DOMAINWAR, &startmsg);

	//系统公告
	BroadCast(m_Config.GuildWarTime.StartNotice.c_str(), false);

	LC_ServerPlayerManager* pkServerPlayerMgr = SERVER_GET_PLAYER_MANAGER();
	const LC_PlayerIDPtrMap& allPlayers = pkServerPlayerMgr->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator cit = allPlayers.begin();
	for (; cit != allPlayers.end(); ++cit)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)cit->second;
		if (pkServerPlayer && pkServerPlayer->GetLevel() >= GUILDWAR_MINLEVEL && !UT_ServerHelper::IsInCross(pkServerPlayer))
		{
			pkServerPlayer->SendMsgToClient(MGPT_NOTIFY_DOMAINWARPLAYER_BEGIN);
		}
	}
}

void GuildWarDomain::EndWar()
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return;
	}
	LC_ServerMapLogicPlaceVector::iterator beg = m_DomainMap.begin();
	for (; beg != m_DomainMap.end(); ++beg)
	{
		LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)(*beg)->GetPlay(PlayType_War);
		if (play == NULL)
		{
			continue;
		}
		play->End();
	}
	if (!m_Config.GuildWarTime.EndNotify.empty())
	{
		BroadCast(m_Config.GuildWarTime.EndNotify.c_str(), false);
	}
	m_WarState = GuildWarState_Peace;
	PrepareNearWar();
}

void GuildWarDomain::Order(int delaytime)
{
	if (m_WarState == GuildWarState_War_Owner || m_WarState == GuildWarState_War_Ownerless)
	{
		return;
	}
	time_t curtimestamp = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if (delaytime <= 0)
	{
		StartWar(curtimestamp);
	}
	else
	{
		SetNotice(delaytime, curtimestamp);
	}
	//设置开始时间
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	time_t begtime = 0;
	time_t endtime = 0;
	GetWarTimeStamp(begtime, endtime);
	pWorldGuildWarAsset->SetDomainTimeStamp(begtime, endtime);
}

bool GuildWarDomain::CharEnter(LC_ServerPlayer* player)
{
	if ((m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner) && player->GetLevel() >= GUILDWAR_MINLEVEL && !UT_ServerHelper::IsInCross(player))
	{
		player->SendMsgToClient(MGPT_NOTIFY_DOMAINWARPLAYER_BEGIN);
		return false;
	}
	return true;
}

LC_ServerMapLogic_Place* GuildWarDomain::GetMapLogic(int logicid)
{
	if (m_WarState != GuildWarState_War_Ownerless && m_WarState != GuildWarState_War_Owner)
	{
		return NULL;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	return pkWorldManager->GetMapLogic(logicid);
}

void GuildWarDomain::PrepareNearWar()
{
	GuildWarUnit::PrepareNearWar();
	//设置开始时间
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return;
	}
	time_t begtime = 0;
	time_t endtime = 0;
	GetWarTimeStamp(begtime, endtime);
	pWorldGuildWarAsset->SetDomainTimeStamp(begtime, endtime);
}

void GuildWarDomain::NotifyPowerEnd(uint64_t power, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId)
{
	time_t startstamp = 0;
	time_t endstamp = 0;
	GetWarTimeStamp(startstamp, endstamp);
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkGuildManager->ReSetDomainPower(pkWorldManager->GetMapLogic(logicid)->GetMapResID(), power, startstamp);
	LC_ServerGuildInfo* powerguild = pkGuildManager->GetGuildInfoByID(power);
	if (powerguild == NULL)
	{
		BroadCast(m_Config.GuildWarTime.FailNotify.c_str(), false, logicid);
	}
	else
	{
		StringType successnotify = m_Config.GuildWarTime.SuccessNotify.c_str();
		StringType playername, playerPosition;
		powerguild->GetMasterName(playername);
		powerguild->GetMasterPosition(playerPosition);

		successnotify += "s:" + powerguild->GetName() + ";s:" + playerPosition + ";s:" + playername + ";";
		BroadCast(successnotify.c_str(), false, logicid);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LC_WarManager::LC_WarManager(): m_LocalWarUnit(), m_CrossWarUnit(), m_DomainUnit()
{

}

LC_WarManager::~LC_WarManager()
{
	Clear();
}

void LC_WarManager::Clear()
{
	m_LocalWarUnit.Clear();
	m_CrossWarUnit.Clear();
	m_DomainUnit.Clear();
}

bool LC_WarManager::Init()
{
	if (m_LocalWarUnit.Init(WarType_GuildWar, "GuildWar_Config.json") == false)
	{
		return false;
	}

	if (m_CrossWarUnit.Init(WarType_CrossRealmGuildWar, "GlobalGuildWar_Config.json") == false)
	{
		return false;
	}

	if (m_DomainUnit.Init(WarType_DomainWar, "DomainWar_Config.json") == false)
	{
		return false;
	}
	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_MEDIUM));
	return true;
}

void LC_WarManager::UnInit()
{
	Clear();
}

void LC_WarManager::Update(float fCurrentTime, float fDeltaTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}
	m_LocalWarUnit.Update(fCurrentTime, fDeltaTime);
	m_CrossWarUnit.Update(fCurrentTime, fDeltaTime);
	m_DomainUnit.Update(fCurrentTime, fDeltaTime);
}

LC_ServerMapLogic_Place* LC_WarManager::GetMapLogic(WarType wartype, int logicid)
{
	if (wartype == WarType_GuildWar)
	{
		return m_LocalWarUnit.GetMapLogic(logicid);
	}
	else if (wartype == WarType_CrossRealmGuildWar)
	{
		return m_CrossWarUnit.GetMapLogic(logicid);
	}
	else if (wartype == WarType_DomainWar)
	{
		return m_DomainUnit.GetMapLogic(logicid);
	}
	return NULL;
}

void LC_WarManager::CharEnter(LC_ServerPlayer* player)
{
	if (player == NULL)
	{
		return;
	}
	if (m_LocalWarUnit.CharEnter(player) == false)
	{
		return;
	}
	if (m_CrossWarUnit.CharEnter(player) == false)
	{
		return;
	}
	if (m_DomainUnit.CharEnter(player) == false)
	{
		return;
	}
}

void LC_WarManager::GetWarTimeStamp(WarType wartype, time_t& beginstamp, time_t& endstamp)
{
	if (wartype == WarType_GuildWar)
	{
		m_LocalWarUnit.GetWarTimeStamp(beginstamp, endstamp);
	}
	else if (wartype == WarType_CrossRealmGuildWar)
	{
		m_CrossWarUnit.GetWarTimeStamp(beginstamp, endstamp);
	}
	else if (wartype == WarType_DomainWar)
	{
		m_DomainUnit.GetWarTimeStamp(beginstamp, endstamp);
	}
}

void LC_WarManager::NotifyPowerEnd(uint64_t power, WarType wartype, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId)
{
	switch(wartype)
	{
	case WarType_GuildWar:
		{
			m_LocalWarUnit.NotifyPowerEnd(power, logicid, idList, uiPowerId);
		}
		break;
	case WarType_CrossRealmGuildWar:
		{
			m_CrossWarUnit.NotifyPowerEnd(power, logicid, idList, uiPowerId);
		}
		break;
	case WarType_DomainWar:
		{
			m_DomainUnit.NotifyPowerEnd(power, logicid, idList, uiPowerId);
		}
		break;
	default:
		break;
	}
	GfxWriteFmtLog(LOG_GUILD_FILELINE, LOG_SWITCH_GUILD, "LC_WarManager::NotifyPowerEnd", power, wartype, logicid, uiPowerId);
}

void LC_WarManager::NotifyGivenPower(uint64_t power, WarType wartype, int logicid, const unique_id_impl& uiPowerId)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	StringType successstr;
	StringType failstr;
	if (wartype == WarType_CrossRealmGuildWar)
	{
		successstr = m_CrossWarUnit.getConfig().GuildWarTime.GivePowerSuccessNotify.c_str();
		failstr = m_CrossWarUnit.getConfig().GuildWarTime.GivePowerFailNotify.c_str();
	}
	else if (wartype == WarType_GuildWar)
	{
		successstr = m_LocalWarUnit.getConfig().GuildWarTime.GivePowerSuccessNotify.c_str();
		failstr = m_LocalWarUnit.getConfig().GuildWarTime.GivePowerFailNotify.c_str();
	}
	else
	{
		successstr = m_DomainUnit.getConfig().GuildWarTime.GivePowerSuccessNotify.c_str();
		failstr = m_DomainUnit.getConfig().GuildWarTime.GivePowerFailNotify.c_str();
	}
	if (power == INVALID_GUILD_ID)
	{
		if (wartype == WarType_CrossRealmGuildWar)
		{
			m_CrossWarUnit.BroadCastGroup(failstr.c_str(), true, logicid);
		}
		else if (wartype == WarType_DomainWar)
		{
			m_DomainUnit.BroadCast(failstr.c_str(), false, logicid);
		}
		else
		{
			m_LocalWarUnit.BroadCast(failstr.c_str(), true, logicid);
		}
	}
	else
	{
		StringType successnotify = successstr.c_str();
		LC_ServerGuildInfo* powerguild = pkGuildManager->GetGuildInfoByID(power);
		if (powerguild == NULL)
		{
			return;
		}
		StringType servername;
		GetGameServerApp()->GetServerManager()->GetServerInfo(powerguild->GetServerId(), servername);
		StringType playername, playerPosition;
		if(uiPowerId == 0)
		{
			powerguild->GetMasterName(playername);
			powerguild->GetMasterPosition(playerPosition);
		}
		else
		{
			powerguild->GetMemberNamePosition(uiPowerId, playername, playerPosition);
		}
		successnotify += "s:" + powerguild->GetName() + ";s:" + playerPosition + ";s:" + playername;

		if(wartype == WarType_DomainWar)
		{
			LC_ServerMapLogic_Place* thelogic = GetMapLogic(wartype, logicid);
			if (thelogic != NULL)
			{
				CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
				CF_WorldMapList::DataEntry* data = pkCSVWorldMapList->GetEntryPtr(thelogic->GetMapResID());
				if (data != NULL)
				{
					successnotify += ";int32:" + thelogic->GetMapResID();
				}
			}
		}

		successnotify += ";";

		if (wartype == WarType_CrossRealmGuildWar)
		{
			m_CrossWarUnit.BroadCastGroup(successnotify.c_str(), true, logicid);
		}
		else if (wartype == WarType_DomainWar)
		{
			m_DomainUnit.BroadCast(successnotify.c_str(), false, logicid);
		}
		else
		{
			m_LocalWarUnit.BroadCast(successnotify.c_str(), true, logicid);
		}
	}
}

bool LC_WarManager::IsWarFighting()
{
	return m_LocalWarUnit.IsWarFighting()|| m_CrossWarUnit.IsWarFighting() || m_DomainUnit.IsWarFighting();
}
