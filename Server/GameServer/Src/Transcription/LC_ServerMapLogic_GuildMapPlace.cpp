#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerMapLogic_GuildMapPlace.h"
#include "LC_ServerGuildManager.h"
#include "CF_WorldMapList.h"
#include "CF_GuildTowerRankReward.h"
#include "CF_GuildTower.h"
#include "UT_ServerHelper.h"
#include "UT_Redis.h"
#include "LC_ServerNPC.h"
#include "UT_OperateLog.h"
#include "GameServerApp.h"

using namespace OPERATELOG;
using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;
using namespace Protocol;
//------------------------------------------------------------------------------
LC_ServerMapLogic_GuildMapPlace::LC_ServerMapLogic_GuildMapPlace() : LC_ServerMapLogic_Place(), m_nGuildID(0), m_nInvaildTime(0)
{
}
//------------------------------------------------------------------------------
LC_ServerMapLogic_GuildMapPlace::~LC_ServerMapLogic_GuildMapPlace()
{

}
void LC_ServerMapLogic_GuildMapPlace::SetGuildID(uint64_t guildID)
{
	m_nGuildID = guildID;
}

uint64_t LC_ServerMapLogic_GuildMapPlace::GetGuildID()
{
	return m_nGuildID;
}

void LC_ServerMapLogic_GuildMapPlace::SetPlaceInvaildTime(int32_t time)
{
	m_nInvaildTime = time;
}

int32_t LC_ServerMapLogic_GuildMapPlace::GetPlaceInvaildTime()
{
	return m_nInvaildTime;
}
//------------------------------------------------------------------------------
void LC_ServerMapLogic_GuildMapPlace::Init()
{
	LC_ServerMapLogic_Place::Init();
	Clear();
}
void LC_ServerMapLogic_GuildMapPlace::UnInit()
{
	LC_ServerMapLogic_Place::UnInit();
	Clear();
}
void LC_ServerMapLogic_GuildMapPlace::Clear()
{
	m_nGuildID = 0;
	m_nInvaildTime = 0;
	SetMapLogicType(MT_GUILD_MAP);
}
//-----------------------------------------------------------------------------------
int LC_ServerMapLogic_GuildMapPlace::GetLeftTime()
{
	int time = 0;
	int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	int32_t endTime = GetPlaceInvaildTime();
	int32_t deltaTime = endTime-currentTime;
	if(deltaTime > 0)
	{
		time = deltaTime < m_PlayerVaildTime ? deltaTime : m_PlayerVaildTime;
	}

	if(deltaTime<0 || 0==time)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_receivedClientAlreadyInGame currentTime:%d, endTime:%d, deltaTime:%d, m_PlayerVaildTime:%d", 
			currentTime, endTime, deltaTime, m_PlayerVaildTime);
	}

	return time;
}

void LC_ServerMapLogic_GuildMapPlace::Update(float fCurrentTime, float fDeltaTime)
{
	GameServerManager* pkAppMgr = GetGameServerApp()->GetServerManager();
	if (!pkAppMgr->isLoaded())
	{
		return;
	}

	LC_ServerMapLogicBase::Update(fCurrentTime, fDeltaTime);
	PlaceLogicState result = GetPlaceState();
	if (result == PLACESTATE_UNINITED)
	{
		SetActive(false);
		return;
	}
	else if(result == PLACESTATE_PROCESS)//进行中
	{
		if (m_Start)
		{
			int resMapID = GetMapResID();
			int mapID = GetMapLogicID();
			float time = GET_PROCESS_TIME;
			float fRemainTime = fCurrentTime - m_TimeStamp - m_CountDownTime;
			if (m_TimeStamp > 1.0f && fCurrentTime >= m_TimeStamp+m_CountDownTime)
			{
				SetPlaceState(PLACESTATE_FAIL);//时间到，失败	
			}			
			else
			{
				UpdateAllPlay(fCurrentTime, fDeltaTime);
			}
		}
	}
	else if (result == PLACESTATE_PREEND) //进入倒计时
	{
		//副本提前结束后，结算界面弹出之前，清除场景中的所有怪（副本中的怪和玩家用技能召唤的怪都清掉）
		ClearAllMonster();

		//倒计时开始到副本结束的时间 时间到退出副本
		if (fCurrentTime >= m_TimeStamp)
		{
			SetPlaceState(PLACESTATE_END);
		}
		else
		{
			if (m_Start)
			{
				UpdateAllPlay(fCurrentTime, fDeltaTime);
			}
		}
	}
	else if(result == PLACESTATE_FAIL)
	{
		//副本失败后，结算界面弹出之前，清除场景中的所有怪（副本中的怪和玩家用技能召唤的怪都清掉）
		ClearAllMonster();

		if(m_TimeStamp>1.0f && fCurrentTime >= m_TimeStamp)
		{
			SetPlaceState(PLACESTATE_END);
		}
	}
	else if (result == PLACESTATE_END)
	{
		ForceUpdateAllPlayerDamageData();

		EndAllPlay();
		UnInit();
		return;
	}
}

void LC_ServerMapLogic_GuildMapPlace::ForceUpdateDamageData(LC_ServerPlayer* pkPlayer, bool syncClient)
{
	if(NULL==pkPlayer || INVALID_GUILD_ID==pkPlayer->GetGuildID() || false==pkPlayer->IsMainController())
	{
		return;
	}

	LC_CombatRecorder recorder = pkPlayer->GetWriteCombatRecorderData();
	uint64_t data = recorder.GetValue();

	mem::vector<uint64_t> params;
	params.reserve(2);
	params.push_back(pkPlayer->GetGuildID());
	params.push_back(GetMapResID());

	uint64_t totalDamage = 0;
	int32_t rankIndex = pkPlayer->UpdateGuildMapBossDamage(params, data, totalDamage);
	pkPlayer->DisableCombatRecorder(rankIndex, data, totalDamage, syncClient);

	const unique_id_impl& instance = pkPlayer->GetInstance();
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ForceUpdateDamageData c_cid:%d, c_citizenship:%d, c_uid:%d, mapID:%d, guildID:%lld, totalDamage:%lld\n",
		instance.detail(), instance.catagory(), pkPlayer->GetUserID(), GetMapResID(), pkPlayer->GetGuildID(), totalDamage);

	LC_ActorPtrList pkNpcList;
	GetAllMonster(pkNpcList);
	for(LC_ActorPtrList::iterator it=pkNpcList.begin(); it!=pkNpcList.end(); ++it)
	{
		LC_ServerNPC* pkNpc = (LC_ServerNPC*)*it;
		if(NULL != pkNpc)
		{
			pkNpc->CheckAddGuildMapBossReward(true, pkNpc->GetHP());
		}
	}
}

void LC_ServerMapLogic_GuildMapPlace::ForceUpdateAllPlayerDamageData()
{
	GameLogic::LC_ActorPtrList allplayer;
	GetAllRealPlayer(allplayer);
	GameLogic::LC_ActorPtrList::iterator end = allplayer.end();
	for (GameLogic::LC_ActorPtrList::iterator beg=allplayer.begin(); beg!=end; ++beg)
	{
		ForceUpdateDamageData((LC_ServerPlayer*)(*beg), true);
	}
}

void LC_ServerMapLogic_GuildMapPlace::SettleGuildMapBoss(int bossID)
{
	int mapID = GetMapResID();
	uint64_t guildID = GetGuildID();

	//更新玩家伤害数据
	GameLogic::LC_ActorPtrList players;
	GetAllRealPlayer(players);

	GameLogic::LC_ActorPtrList::iterator actorBeg = players.begin();
	for(; actorBeg != players.end(); ++actorBeg)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*actorBeg);
		if (pkPlayer!=NULL && 0!=pkPlayer->GetGuildID())
		{
			LC_CombatRecorder recorder = pkPlayer->GetWriteCombatRecorderData();
			uint64_t data = recorder.GetValue();

			mem::vector<uint64_t> params;
			params.reserve(2);
			params.push_back(pkPlayer->GetGuildID());
			params.push_back(mapID);
			
			uint64_t totalDamage = 0;
			int32_t rankIndex = pkPlayer->UpdateGuildMapBossDamage(params, data, totalDamage);
			pkPlayer->DisableCombatRecorder(rankIndex, data, totalDamage);
		}
	}

	//更新怪物血量数据
	LC_ActorPtrList pkNpcList;
	GetAllMonster(pkNpcList);
	for(LC_ActorPtrList::iterator it=pkNpcList.begin(); it!=pkNpcList.end(); ++it)
	{
		LC_ServerNPC* pkNpc = (LC_ServerNPC*)*it;
		if(NULL != pkNpc)
		{
			pkNpc->CheckAddGuildMapBossReward(true, pkNpc->GetHP());
		}
	}

	GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_ERROR, "SettleGuildMapBoss bossID:%d, mapID:%d, guildID:%lld, playerSize:%d, monsterSize:%d\n", bossID, mapID, guildID, players.size(), pkNpcList.size());

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if(NULL == pkGuildManager)
	{
		return;
	}

	LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(guildID);
	if(NULL == pkGuildInfo)
	{
		return;
	}

	CF_GuildTower::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GuildTower>(mapID);
	if(NULL==pkData || pkData->_lCharType!=bossID)
	{
		return;
	}

	mem::vector<uint64_t> params;
	params.reserve(2);
	params.push_back(guildID);
	params.push_back(mapID);

	mem::vector<uint64_t> allPlayers;
	UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_GUILDMAPBOSS_DAMAGE_RANK, params, allPlayers);

	//发奖
	Utility::UT_SIMDataList itemlist;
	CF_GuildTowerRankReward::DataEntry* pkEntry = NULL;
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN17_OPERATE, OPERATE_LOGTYPE2);
	for(int i=0; i<allPlayers.size(); ++i)
	{
		//发奖时，不在公会，不发奖
		if(false == pkGuildInfo->IsGuildPlayer(allPlayers[i]))
		{
			continue;
		}

		if(NULL!=pkEntry && i>=pkEntry->_iRankStartIndex && i<pkEntry->_iRankEndIndex)
		{
			std::stringstream ss;
			ss<< (i+1) << ";";
			StringType mailDesc = pkEntry->_iMailDesc + STDSTR2TPSTR(ss.str());
			UT_ServerHelper::SendToMail(pkEntry->_iMailTitle, mailDesc, allPlayers[i], "", 0, itemlist, "",nLogCode,i+1);
		}
		else
		{
			CF_GuildTowerRankReward::DataEntryMapExternIterator kIt = CF_GuildTowerRankReward::GetSingletonPtr()->GetIterator();
			for(; kIt.HasMoreElements(); kIt.MoveNext())
			{
				pkEntry = kIt.PeekNextValuePtr();
				if(pkEntry->_iType==pkData->_iRankRewardType && i>=pkEntry->_iRankStartIndex && i<pkEntry->_iRankEndIndex)
				{
					itemlist.clear();
					Utility::ParseFmtString2DataIFs(pkEntry->_kReward, itemlist);

					stringstream index;
					index << (i+1) << ";";

					StringType mailDesc =  pkEntry->_iMailDesc + STDSTR2TPSTR(index.str());
					UT_ServerHelper::SendToMail(pkEntry->_iMailTitle, mailDesc, allPlayers[i], "", 0, itemlist,"",nLogCode,i+1);
				}
			}
		}
	}
}
