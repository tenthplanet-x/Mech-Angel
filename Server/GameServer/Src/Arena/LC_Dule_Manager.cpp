#include "LM_LocaleManager.h"
#include "MG_Game2Group.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"
#include "GameServerApp.h"
#include "MG_ArenaProtocol.h"
#include "LC_ArenaBattleAssert.h"
#include "LC_ServerWorldManager.h"
#include "CF_RankReward.h"
#include "UT_ServerHelper.h"
#include "LC_Dule_Manager.h"
#include "CF_RankReward.h"
#include "LC_ServerPlayerManager.h"
#include "LC_RankManager.h"
#include "LC_ServerMapLogic_SoloDulePlay.h"
#include "LC_ServerMapLogic_SkyPlay.h"
#include "LC_ServerMapLogic_SummitTeamPlay.h"
#include "LC_ServerMapLogic_Place.h"
#include "oss_define.h"
#include "LC_GameEventManager.h"
#include "LC_PackEntryFactory.h"
#include "GlobalSettings.h"
#include "UT_OperateLog.h"
#include "LC_Helper.h"
using namespace OPERATELOG;
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

//--------------------------------------------------------------------------------------------------------------
LC_Solo_Duel_Manager::LC_Solo_Duel_Manager():m_Parent(NULL), m_SoloDuelMap(),m_eMapType(MapType::MT_NORMAL),m_nMapResId(0)
{
	
}
LC_Solo_Duel_Manager::~LC_Solo_Duel_Manager()
{
	fin();
}

bool LC_Solo_Duel_Manager::init(LC_Dule_Manager* parent, GameLogic::SOLO_DULE_TYPE eType,MapType eMapType,int nMapResId )
{
	m_Parent = parent;
	m_eType = eType;
	m_nMapResId = nMapResId;
	m_eMapType = eMapType;
	
	return true;
}

void LC_Solo_Duel_Manager::fin()
{
	m_Parent = NULL;
	Solo_Duel_Info * p_Solo = NULL;
	for (SoloDuelInfoMap::iterator solobeg = m_SoloDuelMap.begin(); solobeg != m_SoloDuelMap.end(); ++solobeg)
	{
		p_Solo = solobeg->second;
		if(p_Solo)
		{
			T_DELETE_D p_Solo;
		}
		p_Solo = NULL;
	}
	m_SoloDuelMap.clear();
}

Solo_Duel_Info* LC_Solo_Duel_Manager::GetSoloDuelInfo(const unique_id_impl& charID)
{
	SoloDuelInfoMap::iterator it=m_SoloDuelMap.find(charID);
	if (it==m_SoloDuelMap.end())
		return NULL;
	return it->second;
}

void LC_Solo_Duel_Manager::AddSoloDuelInfo(const unique_id_impl& charID,const unique_id_impl& bechallengeCharID,int challengeindex,int bechallengeindex)
{
	SoloDuelInfoMap::iterator it=m_SoloDuelMap.find(charID);
	if (it!=m_SoloDuelMap.end() && it->second)
	{
		it->second->m_beChallengeCharID=bechallengeCharID;
		it->second->m_UserIndex=challengeindex;
		it->second->m_beChallengeIndex=bechallengeindex;
	}
	else
	{
		Solo_Duel_Info* duelinfo=T_NEW_D Solo_Duel_Info;
		duelinfo->m_CharID=charID;
		duelinfo->m_UserIndex=challengeindex;
		duelinfo->m_beChallengeCharID=bechallengeCharID;
		duelinfo->m_beChallengeIndex=bechallengeindex;
		m_SoloDuelMap[charID]=duelinfo;
	}
}

void LC_Solo_Duel_Manager::DelSoloDuelInfo(const unique_id_impl& charID)
{
	SoloDuelInfoMap::iterator it=m_SoloDuelMap.find(charID);
	if (it==m_SoloDuelMap.end())
	{
		return;
	}
	T_DELETE_D it->second;
	m_SoloDuelMap.erase(it);
}

LC_ServerPlayer* LC_Solo_Duel_Manager::GetPlayer(const unique_id_impl& charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
		return NULL;
	return (LC_ServerPlayer*)playManager->FindPlayer(charID);
}

ResultType LC_Solo_Duel_Manager::ChallengeCheck(LC_ServerPlayer* pkPlayer,const unique_id_impl& bechallengeCharID, int bechallengeindex)
{
	if (pkPlayer == NULL)
		return RE_FAIL;
	if (pkPlayer->GetInstance() == bechallengeCharID)
		return RE_FAIL;
	ResultType ret32 = __CheckChallenge(pkPlayer);
	if (ret32 != RE_SUCCESS)
	{
		pkPlayer->SendChallengeResToClient(ret32,bechallengeCharID,m_eType );
		return RE_FAIL;
	}
	if (LC_Helper::isInArena(pkPlayer) || LC_Helper::isInTranscript(pkPlayer))//正在副本中不允许
	{
		pkPlayer->SendChallengeResToClient(RE_SOLO_DULE_IN_TRANSCRIPTION,bechallengeCharID,m_eType );
		return RE_FAIL;
	}

	AddSoloDuelInfo(pkPlayer->GetInstance(), bechallengeCharID, pkPlayer->GetArenaBattleAssert()->GetLocalIndex(), bechallengeindex);
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	switch ( m_eType )
	{
	case GameLogic::eSoloDuleType_Arena:
		{
			ret32 = rank_Manager->SendGroupChallengeReq(pkPlayer, bechallengeCharID, bechallengeindex);
			if (ret32 != RE_SUCCESS)
			{
				pkPlayer->SendChallengeResToClient(ret32,bechallengeCharID,m_eType);
				return ret32;
			}
		}
		break;
	case GameLogic::eSoloDuleType_GuildOffline:
		{
			uint64_t nInstanceID = pkPlayer->GetInstance();
			return rank_Manager->ChallengeResponse_GuildOffline(nInstanceID, bechallengeCharID, RE_SUCCESS );
		}
		break;
	case GameLogic::eSoloDuleType_Sky:
		{
			//道具检测
			if(pkPlayer->GetSkyArenaChallengeTimes() >= GetGlobalSetting.iSkyArenaDailyFreeChallengeTimes)
			{
				if (pkPlayer->CheckConsumeItemByID(GetGlobalSetting.iSkyArenaChallengeItemId, 1) == RE_SUCCESS)
				{
					pkPlayer->ConsumeItemsInPackageByID(GetGlobalSetting.iSkyArenaChallengeItemId, 1);
				}
				else
				{
					return RE_SKYARENA_SOLO_ITEM_LIMIT;
				}
			}
			RankTimeData timeData;
			rank_Manager->GetRankOpenTime(timeData);
			int nTime = time(NULL);
			if( nTime >= timeData.nCloseTime || nTime < timeData.nStartTime)
			{
				return RE_SKYARENA_SOLO_ITEM_ISCLOSE;
			}
			ret32 = rank_Manager->SendGroupChallengeSummitTeamReq(pkPlayer, bechallengeCharID,Arena_Type_Firmament);
			if (ret32 != RE_SUCCESS)
			{
				pkPlayer->SendChallengeResToClient(ret32,bechallengeCharID,m_eType);
				return ret32;
			}
		}
		break;
	case GameLogic::eSoloDuleType_Peak_Team:
		{
			ret32 = rank_Manager->SendGroupChallengeSummitTeamReq(pkPlayer, bechallengeCharID,Arena_Type_Peak_Group);
			if (ret32 != RE_SUCCESS)
			{
				pkPlayer->SendChallengeResToClient(ret32,bechallengeCharID,m_eType);
				return ret32;
			}
			
		}
		break;

	}
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::ChallengeCheckResult(const unique_id_impl& charID, uint64_t nBeChallengeCharID, ResultType iResult,uint64_t solofightuniqueId,int nScore)
{
	LC_ServerPlayer* pkPlayer = GetPlayer(charID);
	if (pkPlayer == NULL)
		return RE_FAIL;
	const unique_id_impl& bechallengeCharID = nBeChallengeCharID;
	if (LC_Helper::isInArena(pkPlayer) || LC_Helper::isInTranscript(pkPlayer))//正在副本中不允许
	{
		pkPlayer->SendChallengeResToClient(-1,bechallengeCharID,m_eType );
		DelSoloDuelInfo(pkPlayer->GetInstance());
		return RE_FAIL;
	}

	Solo_Duel_Info* duelinfo = GetSoloDuelInfo(pkPlayer->GetInstance());
	if (duelinfo == NULL)
		return RE_FAIL;
	
	
	if (duelinfo->m_beChallengeCharID != bechallengeCharID)
	{
		pkPlayer->SendChallengeResToClient(-2,bechallengeCharID,m_eType );
		DelSoloDuelInfo(pkPlayer->GetInstance());
		return RE_FAIL;
	}
	duelinfo->m_solofightuniqueId = solofightuniqueId;
	duelinfo->m_uScore = nScore;
	if(solofightuniqueId == 0)
	{
		duelinfo->m_solofightuniqueId = GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
	}
	if (iResult != RE_SUCCESS)
	{
		pkPlayer->SendChallengeResToClient(iResult,bechallengeCharID,m_eType );
		DelSoloDuelInfo(pkPlayer->GetInstance());
		return RE_FAIL;
	}
	//创建副本
	//拉人进副本
	ResultType ret = Challenge(pkPlayer, bechallengeCharID, duelinfo->m_beChallengeIndex,duelinfo->m_solofightuniqueId);
	if (ret != RE_SUCCESS)
	{
		pkPlayer->SendChallengeResToClient(ret,bechallengeCharID,m_eType );
		DelSoloDuelInfo(pkPlayer->GetInstance());
		return ret;
	}
	
	if ( m_eType == GameLogic::eSoloDuleType_Arena )
	{
		LC_ArenaBattleAsset* pAsset = pkPlayer->GetArenaBattleAssert(m_eType);
		pAsset->SetLocalChallengeCD();
		pAsset->DelLocalTotalChallengeTime();
		pAsset->AddLocalChallengedTime();
		pkPlayer->GetCommonStatAsset().AddArenaChallengeCnt();
	}

	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
	if (pkGameEvent)
	{
		GameLogic::GameEventType nEventType = GLET_NONE;
		switch ( m_eType )
		{
		case GameLogic::eSoloDuleType_Arena:
			nEventType = GLET_ARENA_CHALLENGE;
			break;
		case GameLogic::eSoloDuleType_GuildOffline:
			nEventType = GLET_ARENA_CHALLENGE_GUILD_OFFLINE;
			break;
		case GameLogic::eSoloDuleType_Sky:
			nEventType = GLET_CROSS_ARENA_SKY;
			break;
		case GameLogic::eSoloDuleType_Peak_Team:
			nEventType = GLET_CROSS_ARENA_CHALLENGE	;
			break;
		}
		pkGameEvent->SetEventType(nEventType);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
		pkGameEvent->SetEventParam(EP_CHALLENGE_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_CHALLENGE_CITY, charID.catagory());
		pkGameEvent->SetEventParam(EP_CHALLENGE_CHAR, charID.detail());
		pkGameEvent->SetEventParam(EP_CHALLENGE_TARGET_CITY, bechallengeCharID.catagory());
		pkGameEvent->SetEventParam(EP_CHALLENGE_TARGET_CHAR, bechallengeCharID.detail());
		pkGameEvent->SetEventParam(EP_CHALLENGE_MAP_ID, pkPlayer->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::Challenge(LC_ServerPlayer* pkPlayer, const unique_id_impl& bechallengeCharID, int beChallengeindex,uint64_t solofightuniqueId)
{
	if (pkPlayer == NULL)
		return RE_FAIL;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, m_nMapResId, m_eMapType);
	if (logic == NULL)
		return RE_FAIL;
	int64_t isolofightuniqueId = solofightuniqueId;
	if( GameLogic::eSoloDuleType_Arena == m_eType || GameLogic::eSoloDuleType_GuildOffline == m_eType)
	{
		LC_ServerMapLogic_SoloDulePlay* play = (LC_ServerMapLogic_SoloDulePlay*)logic->GetPlay(PlayType_SoloDule);
		if (play == NULL)
			return RE_FAIL;
		play->ReservaDuleInfo(pkPlayer->GetID(), bechallengeCharID, beChallengeindex,isolofightuniqueId);
		if (GameLogic::eSoloDuleType_Arena == m_eType)
		{
			play->GetPlace()->NotifyGameEventToClient(pkPlayer,EVENT_TYPE_FIGHT_RECORD_START,isolofightuniqueId,pkPlayer->GetUserID(),1 ,play->GetType());
		}
	}
	else if ( GameLogic::eSoloDuleType_Sky == m_eType )
	{
		LC_ServerMapLogic_SkyPlay* play = (LC_ServerMapLogic_SkyPlay*)logic->GetPlay(PlayType_Sky);
		if (play == NULL)
			return RE_FAIL;
		play->ReservaDuleInfo(pkPlayer->GetID(), bechallengeCharID);
	}
	else if ( GameLogic::eSoloDuleType_Peak_Team == m_eType )
	{
		LC_ServerMapLogic_SummitTeamPlay* play = (LC_ServerMapLogic_SummitTeamPlay*)logic->GetPlay(PlayType_SummitTeam);
		if (play == NULL)
			return RE_FAIL;
		play->ReservaDuleInfo(pkPlayer->GetID(), bechallengeCharID);
		//play->InitTeamFightId(1,solofightuniqueId);
	}

	pkPlayer->SetSoloDulePlay(true);
	ResultType eResult = pkWorldManager->RequestChangePlayerLocation(pkPlayer,
						 LMIOT_TRANSFER,
						 logic->GetMapLogicID(),
						 m_nMapResId,
						 UT_Vec3Int());
	if (eResult != RE_SUCCESS)
	{
		return eResult;
	}
	pkPlayer->SaveSnapshotData();
	pkPlayer->ReliveAllBuddyBeforeEnterMap(true,true);
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::SetChallengeResult_GuildOffline(LC_ServerPlayer* pkPlayer, uint64_t beChallengeCharID, uint64_t beChallengeGuildID, const StringType& beChallengeCharName, bool sucess)
{
	if (pkPlayer == NULL)
	{
		return RE_FAIL;
	}
	Solo_Duel_Info* duelinfo = GetSoloDuelInfo(pkPlayer->GetInstance());
	if (duelinfo == NULL)
	{
		return RE_FAIL;
	}
	if (duelinfo->m_beChallengeCharID != beChallengeCharID)
	{
		return RE_FAIL;
	}
	//本地处理
	LC_ArenaBattleAsset* pAsset = pkPlayer->GetArenaBattleAssert(GameLogic::eSoloDuleType_GuildOffline);
	if ( pAsset == NULL )
		return RE_FAIL;

	LC_ServerPlayerGuild* pPlayerGuild = pkPlayer->GetPlayerGuildRobInfo();

	MG_NOTIFY_GuildOfflineChallenge rltMsg;
	rltMsg.m_nSuccess = sucess;
	rltMsg.m_nAccountPoint = pkPlayer->GetBestGuildSettleAccountPoint();
	rltMsg.m_nLastRankIndex = pkPlayer->GetLastAccountPointRankIndexByTypeAndStoryId(GFRT_PERSONAL, ROIT_Total);
	rltMsg.m_nCurrentRankIndex = pkPlayer->GetCurrentAccountPointRankIndexByTypeAndStoryId(GFRT_PERSONAL, ROIT_Total);
	rltMsg.m_nLastGuildRankIndex =  pkPlayer->GetLastAccountPointRankIndexByTypeAndStoryId(GFRT_GUILD, ROIT_Total);
	rltMsg.m_nCurrentGuildRankIndex = pkPlayer->GetCurrentAccountPointRankIndexByTypeAndStoryId(GFRT_GUILD, ROIT_Total);	
	pkPlayer->SendMsgToClient(MGPT_NOTIFY_RANK_GUILDOFFLINE_CHALLENGE, &rltMsg);

	//挑战失败或者抢到了分，扣次数
	if(rltMsg.m_nSuccess==false || rltMsg.m_nAccountPoint>0)
	{
		pPlayerGuild->ReduceRobScoreChallengeNumAndItem(pkPlayer);
	}

	const unique_id_impl& instance = pkPlayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SetChallengeResult_GuildOffline success:%d, serverid:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, challengePlayerGuildID:%lld, beChallengeGuildID:%lld, account:%d", 
		rltMsg.m_nSuccess, GetGameServerID(), instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkPlayer->GetGuildID(), beChallengeGuildID, rltMsg.m_nAccountPoint);

	//抢分成功
	if(rltMsg.m_nSuccess==true && rltMsg.m_nAccountPoint>0)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(25);
		noticeFmort.AddString(pkPlayer->GetGuildName());//公会名
		noticeFmort.AddString(pkPlayer->GetOwnerCharName());//玩家名
		noticeFmort.AddString(beChallengeCharName);//被挑战玩家名
		noticeFmort.AddNumber(rltMsg.m_nAccountPoint);
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort, beChallengeGuildID, pkPlayer);
	}

	DelSoloDuelInfo(pkPlayer->GetInstance());
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::SetChallengeResult_Arena(LC_ServerPlayer* pkPlayer, uint64_t beChallengeCharID, PS_Rank_User_Info& beChallenge, bool sucess)
{
	if (pkPlayer == NULL)
	{
		return RE_FAIL;
	}
	Solo_Duel_Info* duelinfo = GetSoloDuelInfo(pkPlayer->GetInstance());
	if (duelinfo == NULL)
	{
		return RE_FAIL;
	}
	if (duelinfo->m_beChallengeCharID != beChallengeCharID)
	{
		return RE_FAIL;
	}
	
	pkPlayer->GetArenaBattleAssert()->NotifyChallengeResult(sucess);

	AchievementEvent kJoinEvent;
	kJoinEvent.SetNum(1);
	pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_SOLODULE_JOIN, kJoinEvent);

	//比武场胜利次数
	if (sucess)
	{
		int winStreak = pkPlayer->GetArenaBattleAssert()->GetWinStreak();
		AchievementEvent kEvent;
		kEvent.SetNum(winStreak);
		pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_SOLODULE_WIN, kEvent);

		LC_GameEventManager* pkGameEventManager = LC_GameEventManager::GetSingletonPtr();
		{
			LC_GameEvent* pkGameEvent = pkGameEventManager->AllocGameEvent();
			if (NULL != pkGameEvent)
			{
				pkGameEvent->SetEventType(GLET_CHALLENGE_WIN);
				pkGameEvent->SetEventParam(EP_CHALLENGE_WIN_PLAYER_ID, pkPlayer->GetID());
				pkGameEvent->SetEventParam(EP_CHALLENGE_WIN_MAPID, pkPlayer->GetMapResID());
				pkGameEventManager->InsertGameEvent(pkGameEvent);
			}
		}		
	}

	DelSoloDuelInfo(pkPlayer->GetInstance());
	return RE_SUCCESS;
}
ResultType LC_Solo_Duel_Manager::SetChallengeResultResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Challenge_Result& resp)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetChallengeResultResponse result:%d, c_cid:%d, c_citizenship:%d r1", resp.m_iResult, charID.detail(), charID.catagory());
		return RE_FAIL;
	}

	LC_ServerPlayer* pOwnerPlayer = player->GetOwnerPlayer();
	if(NULL == pOwnerPlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetChallengeResultResponse result:%d, c_cid:%d, c_citizenship:%d r2", resp.m_iResult, charID.detail(), charID.catagory());
		return RE_FAIL;
	}

	if (resp.m_iResult != RE_SUCCESS)
	{
		pOwnerPlayer->SendChallengeResToClient(resp.m_iResult,resp.m_bChallengeId,m_eType );
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetChallengeResultResponse result:%d, c_cid:%d, c_citizenship:%d, m_iResult:%d r3", 
			resp.m_iResult, charID.detail(), charID.catagory(), resp.m_iResult);
		return RE_FAIL;
	}
	int topfrom = NONE_INDEX;
	int topcur = NONE_INDEX;
	LC_ArenaBattleAsset* arenaAssert = pOwnerPlayer->GetArenaBattleAssert();
	attr_value_type exp = 0;
	MoneyType money = 0;
	attr_value_type sp = 0;
	attr_value_type courage = 0;
	MoneyType lijin = 0;

	topfrom = arenaAssert->GetLocalTopIndex();
	arenaAssert->SetLocalIndex(resp.m_CurIndex);
	pOwnerPlayer->GetCommonStatAsset().SetArenaIndex(resp.m_CurIndex);
	topcur = arenaAssert->GetLocalTopIndex();
	
	if (topfrom != topcur)
	{
		//判断名次是否下降
		if (topcur == NONE_INDEX || (topfrom != NONE_INDEX && topcur > topfrom))
		{
			exp = 0;
			money = 0;
			sp = 0;
			courage = 0;
		}
		else
		{
			if (topfrom == NONE_INDEX)
			{
				exp = 1;
				money = 1;
				sp = 1;
				courage = 1;
			}
			else if (topfrom > topcur)
			{
				CF_RankReward* pkCSVRankList = CF_RankReward::GetSingletonPtr();
				int allrankcount = (int)pkCSVRankList->GetEntryCount();
				if (allrankcount >= 1)
				{
					float lastexpone = 0.0f;
					float lastmoneyone = 0.0f;
					float lastspone = 0.0f;
					float lastcourageone = 0.0f;
					float lastlijin = 0.0f;
					CF_RankReward::DataEntry* pkEntry = pkCSVRankList->GetEntryPtr(allrankcount);
					if (pkEntry != NULL)
					{
						lastexpone = pkEntry->_ExpUpgrade;
						lastmoneyone = pkEntry->_MoneyUpgrade;
						lastspone = pkEntry->_SpUpgrade;
						lastcourageone = pkEntry->_CourageUpgrade;
						lastlijin = pkEntry->_LiJinUpgrade;
					}
					int everycount = allrankcount - 1;
					int beg = topfrom - 1;
					int end = topcur;
					double totalexp = 0.0f;
					double totalmoney = 0.0f;
					double totalsp = 0.0f;
					double totalcourage = 0.0f;
					double totallijin = 0.0f;
					while (beg >= end)
					{
						if (beg >= everycount)
						{
							totalexp += lastexpone;
							totalmoney += lastmoneyone;
							totalsp += lastspone;
							totalcourage += lastcourageone;
							totallijin += lastlijin;
						}
						else
						{
							CF_RankReward::DataEntry* pkEntry = pkCSVRankList->GetEntryPtr(beg + 1);
							if (pkEntry != NULL)
							{
								totalexp += pkEntry->_ExpUpgrade;
								totalmoney += pkEntry->_MoneyUpgrade;
								totalsp += pkEntry->_SpUpgrade;
								totalcourage += pkEntry->_CourageUpgrade;
								totallijin += pkEntry->_LiJinUpgrade;
							}
						}
						--beg;
					}
					exp = (int)(((double)(player->GetNoPeakLevel() * 5 + 20)) * totalexp);
					money = (MoneyType)(((double)(player->GetNoPeakLevel() * 5 + 20)) * totalmoney);;
					sp = (attr_value_type)totalsp;
					courage = (attr_value_type)totalcourage;
					lijin = (MoneyType)totallijin;
				}
			}
		}
	}

	pOwnerPlayer->AddCash(money, CT_UNBIND_CASH, SYS_FUNC_TP_RANK_SOLODULE, 0);
	pOwnerPlayer->AddExpWithGamePlay(exp, SYS_FUNC_TP_RANK_SOLODULE, 0);
	pOwnerPlayer->AddSkillLevelupPoints(sp, SYS_FUNC_TP_RANK_SOLODULE, 0);
	pOwnerPlayer->AddCouragePoint(courage, SYS_FUNC_TP_RANK_SOLODULE, 0);

	//挑战成功前3名，发送系统公告
	int32_t PreIndex = resp.m_PreIndex;
	int32_t CurIndex = resp.m_CurIndex;
	const unique_id_impl& instance = player->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "soloduleResult serverid:%d, result:%d, CurIndex:%d, PreIndex:%d c_cid:%d, c_citizenship:%d, c_uid:%d", 
		GetGameServerID(), resp.m_sucess, CurIndex, PreIndex, instance.detail(), instance.catagory(), player->GetUserID());

	if(resp.m_sucess && CurIndex<3 && CurIndex<PreIndex)
	{
		StringType challengeName = player->GetOwnerCharName();
		StringType beChallengeName = STDSTR2TPSTR(resp.m_bChallengeName);

		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(0);

		//第一名
		if(CurIndex == 0)
		{
			noticeFmort.SetMsgID(32);
		}
		else if(CurIndex==1 || CurIndex==2)
		{
			//第二名 第三名
			noticeFmort.SetMsgID(33);
		}
		
		if(noticeFmort.m_nMsgID != 0)
		{
			noticeFmort.AddPlayerInfo(pOwnerPlayer->GetInstanceUniqueID(), challengeName);
			if(resp.m_nRobot)
			{
				noticeFmort.AddString(beChallengeName);
			}
			else
			{
				noticeFmort.AddPlayerInfo(resp.m_bChallengeId, beChallengeName);
			}
			noticeFmort.AddNumber(CurIndex+1);
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
		}
	}

	MG_NOTIFY_Challenge sendmsg;
	sendmsg.m_CurIndex = resp.m_CurIndex;
	sendmsg.m_PreIndex = resp.m_PreIndex;
	sendmsg.m_sucess = resp.m_sucess;
	sendmsg.m_bChallengeName = resp.m_bChallengeName;
	sendmsg.m_TopPre = topfrom;
	sendmsg.m_TopCur = topcur;
	sendmsg.m_Exp = (int)exp;
	sendmsg.m_Money = (int)money;
	sendmsg.m_SP = (int)sp;
	sendmsg.m_Courage = (int)courage;
	sendmsg.m_Lijin = (int)lijin;
	sendmsg.m_nSoloType  = m_eType;
	player->SendMsgToClient(MGPT_NOTIFY_RANK_CHALLENGE, &sendmsg);

	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::BeChallengeNotify(const unique_id_impl& charID, Protocol::MG_GG_Rank_Notify_beChallenge& resp)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
	{
		return RE_FAIL;
	}

	LC_ArenaBattleAsset* arenaAssert = player->GetArenaBattleAssert();
	arenaAssert->SetLocalIndex(resp.m_CurIndex);
	player->GetCommonStatAsset().SetArenaIndex(resp.m_CurIndex);

	MG_NOTIFY_beChallenge sendmsg;
	sendmsg.nType = resp.m_Type;
	sendmsg.m_Score = resp.m_Score;
	if(Arena_Type_Firmament == sendmsg.nType)
	{
		player->SetSkyArenaScoreValue(sendmsg.m_Score);
	}
	player->SendMsgToClient(MGPT_NOTIFY_RANK_BECHALLENGE, &sendmsg);
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::AddChallengeTime(LC_ServerPlayer* pkPlayer,int nLogCode, const vector<int>& nParams)
{
	if (pkPlayer == NULL)
	{
		return RE_FAIL;
	}

	int localDailyChallengeBuyCount = pkPlayer->GetArenaBattleAssert()->GetLocalDailyChallengeBuyCount();
	if(localDailyChallengeBuyCount >= GetGlobalSetting.ChallengeCostTimes)
	{
		return RE_CHALLENGE_COUNT_LIMITED;
	}
	
	int money = GetGlobalSetting.ChallengeCostNumInit + (localDailyChallengeBuyCount * GetGlobalSetting.ChallengeCostNumPlus);
	if (!pkPlayer->ReduceCash(money, ((CashType)(GetGlobalSetting.ChallengeCostType)), SYS_FUNC_TP_RANK_SOLODULE, 0,nLogCode, nParams))
	{
		return RE_UNBIND_YUANBAO_NOT_ENOUGH;
	}

	pkPlayer->GetArenaBattleAssert()->AddLocalBuyChallengeTime();
	pkPlayer->GetArenaBattleAssert()->AddLocalDailyChallengeBuyCount();

	MG_RESP_AddChallengeTimes sendmsg;
	sendmsg.m_iResult = RE_SUCCESS;
	pkPlayer->SendMsgToClient(MGPT_RESP_ADD_CHALLENGE_TIME, &sendmsg);
	return RE_SUCCESS;

}

ResultType LC_Solo_Duel_Manager::GetChallengeCD(LC_ServerPlayer* pkPlayer)
{
	if (pkPlayer == NULL)
	{
		return RE_FAIL;
	}

	MG_RESP_GetChallengeCD sendmsg;
	sendmsg.m_CD = pkPlayer->GetArenaBattleAssert()->GetLocalChallengeCD();
	pkPlayer->SendMsgToClient(MGPT_RESP_GET_CHALLENGE_CD, &sendmsg);
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::ClearChallengeCD(LC_ServerPlayer* pkPlayer, int nLogCode, const vector<int>& nParams)
{
	if (pkPlayer == NULL)
	{
		return RE_FAIL;
	}
	int cost = pkPlayer->GetArenaBattleAssert()->GetLocalClearChallengeCDCost();
	if (cost > 0)
	{
		if (!pkPlayer->ReduceCash(cost, CT_UNBIND_YUANBAO, SYS_FUNC_TP_RANK_SOLODULE, 0,nLogCode, nParams))
		{
			return RE_SHOPCITY_CASH_NOT_ENOUGH;
		}
		pkPlayer->GetArenaBattleAssert()->ClearLocalChallengeCD();
	}

	MG_RESP_ClearChallengeCD sendmsg;
	sendmsg.m_iResult = RE_SUCCESS;
	pkPlayer->SendMsgToClient(MGPT_RESP_CLEAR_CHALLENGE_CD, &sendmsg);
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::GetDailyReward(LC_ServerPlayer* pkPlayer, int rankID, bool bGet, Protocol::MG_GG_RTL_Gain_Rank_Gift* pGainRtl,int nLogCode, const vector<int>& nParams)
{
	if(pkPlayer == NULL)
	{
		return RE_FAIL;
	}
	if(bGet == false)
	{
		//请求
		MG_GG_REQ_Gain_Rank_Gift reqMsg;
		reqMsg.m_RankID = rankID;
		SendMsgToGroup(MGPT_SS_RANK_QUERY_GAINGIFT_REQ, &reqMsg, pkPlayer->GetUserID(),pkPlayer->GetInstance());
		return RE_SUCCESS;
	}

	const unique_id_impl& instance = pkPlayer->GetInstance();
	//领取
	MG_RESP_Daily_Award sendmsg;
	if(pGainRtl == NULL || pGainRtl->m_RankGain == false)
	{
		sendmsg.m_iResult = RE_DAILY_AWARD_FAILED_HAS_AWARD;
		pkPlayer->SendMsgToClient(MGPT_RLT_DAILY_AWARD, &sendmsg);
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDailyReward c_cid:%d, c_citizenship:%d, c_uid:%d r1", instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return sendmsg.m_iResult;
	}
	
	int dailyindex = pGainRtl->m_RankIndex;
	CF_RankReward* pkCSVRankList = CF_RankReward::GetSingletonPtr();
	int nCsvIndex = (int)pkCSVRankList->GetEntryCount();
	if (dailyindex == NONE_INDEX || dailyindex >= nCsvIndex)
	{
		dailyindex = nCsvIndex - 1;
	}
	CF_RankReward::DataEntry* pkEntry = pkCSVRankList->GetEntryPtr(dailyindex + 1);
	if (pkEntry == NULL)
	{
		sendmsg.m_iResult = RE_DAILY_AWARD_FAILED_INDEX;
		pkPlayer->SendMsgToClient(MGPT_RLT_DAILY_AWARD, &sendmsg);
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDailyReward c_cid:%d, c_citizenship:%d, c_uid:%d dailyindex:%d r2", instance.detail(), instance.catagory(), pkPlayer->GetUserID(), dailyindex);
		return sendmsg.m_iResult;
	}

	int32_t level = pkPlayer->GetLevel();
	CF_EXPList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(level);
	if(NULL == pkData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDailyReward c_cid:%d, c_citizenship:%d, c_uid:%d level:%d r3", 
			instance.detail(), instance.catagory(), pkPlayer->GetUserID(), level);
		return RE_FAIL;
	}

	attr_value_type exp = pkData->_lBaseBuddyXP * pkEntry->_DailyExpParm;
	MoneyType money = pkData->_lBaseCash * pkEntry->_DailyMoneyParm;
	attr_value_type sp = (attr_value_type)pkEntry->_DailySP;
	attr_value_type courage = (attr_value_type)pkEntry->_DailyCourage;
	UT_SIMDataList materials = pkEntry->_DailyMaterial;
	//发奖
	pkPlayer->AddCash(money, CT_UNBIND_CASH, SYS_FUNC_TP_RANK_SOLODULE, 0,0,true, NULL,nLogCode, nParams);
	pkPlayer->AddExpWithGamePlay(exp, SYS_FUNC_TP_RANK_SOLODULE, 0);
	RecordCashOperateLog(pkPlayer, nLogCode, GET_OPERATE, CT_USER_EXP, exp, -1, nParams);
	pkPlayer->AddSkillLevelupPoints(sp, SYS_FUNC_TP_RANK_SOLODULE, 0);
	pkPlayer->AddCouragePoint(courage, SYS_FUNC_TP_RANK_SOLODULE, 0, nLogCode, nParams);
	if (materials.size() > 0)
	{
		UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, materials, pkPlayer, SYS_FUNC_TP_RANK_SOLODULE, 0, "188;", "189;", true,false,false,nLogCode,nParams);
	}
	sendmsg.m_iResult = RE_SUCCESS;
	pkPlayer->SendMsgToClient(MGPT_RLT_DAILY_AWARD, &sendmsg);

	oss_role_arena_reward(pkPlayer, dailyindex, money, exp);
	return RE_SUCCESS;
}

ResultType LC_Solo_Duel_Manager::GuildOfflineChallengeTimeOP(LC_ServerPlayer* pkPlayer, int op,int nBuyNum, Protocol::MG_Resp_GuildFightRobScore_Opt& msg)
{
	ResultType res = RE_FAIL;
	if (pkPlayer == NULL)
	{
		return res;
	}

	LC_ServerPlayerGuild* pPlayerGuild = pkPlayer->GetPlayerGuildRobInfo();

	if(op == ROT_Buy)
	{
		res = pPlayerGuild->BuyRobNum(pkPlayer,nBuyNum);
		pPlayerGuild->PackGuildResMsg(msg);
	}
	else if(op == ROT_Query)
	{
		res = RE_SUCCESS;
		pPlayerGuild->PackGuildResMsg(msg);
	}
	
	return res;
}

ResultType LC_Solo_Duel_Manager::__CheckChallenge(LC_ServerPlayer* pkPlayer)
{
	if (pkPlayer == NULL)
		return RE_FAIL;

	if ( m_eType == GameLogic::eSoloDuleType_Arena )//竞技场
	{
		if (!pkPlayer->GetArenaBattleAssert()->CanLocalChallenge())
			return RE_SOLO_DULE_IN_CD;
		int lefttime = pkPlayer->GetArenaBattleAssert()->GetLocalTotalChallengeTime();
		if (lefttime <= 0)// comment next line if unlimited enter solo is necessary
			return RE_SOLO_DULE_CHALLENGE_TIME_OUT;
	}
	else if(m_eType==GameLogic::eSoloDuleType_GuildOffline)//公会抢分
	{
		LC_ServerPlayerGuild* pPlayerGuild = pkPlayer->GetPlayerGuildRobInfo();
		return pPlayerGuild->PreIsAllowChallenge(pkPlayer);
	}
	else if(m_eType==GameLogic::eSoloDuleType_Sky)//苍穹战场
	{
		
	}
	else if(m_eType==GameLogic::eSoloDuleType_Peak_Team)//巅峰竞技场 小组赛
	{
		
	}
	return RE_SUCCESS;
}

//-----------------------------------------------------------------

LC_Dule_Manager::LC_Dule_Manager():
	m_soloDuleManager()
	, m_soloDuleManager_GuildOffline()
{
}

LC_Dule_Manager::~LC_Dule_Manager()
{
}

bool LC_Dule_Manager::init()
{
	m_soloDuleManager.init(this, GameLogic::eSoloDuleType_Arena,MT_ARENA_SOLO,SOLO_DULE_RES_ID);
	m_soloDuleManager_Sky.init(this, GameLogic::eSoloDuleType_Sky,MT_ARENA_SKY,SKY_SOLO_DULE_RES_ID);
	m_soloDuleManager_SummitTeam.init(this, GameLogic::eSoloDuleType_Peak_Team,MT_ARENA_SUMMIT_TEAM,SUMMIT_TEAM_SOLO_DULE_RES_ID);
	m_soloDuleManager_SummitWheel.init(this, GameLogic::eSoloDuleType_Peak,MT_ARENA_SUMMIT,SUMMIT_ARENA_SOLO_DULE_RES_ID);
	m_soloDuleManager_GuildOffline.init(this, GameLogic::eSoloDuleType_GuildOffline,MT_ARENA_GUILD_OFFLINE_SOLO,GUILD_OFFLINE_SOLO_DULE_RES_ID);
	
	return true;
}

void LC_Dule_Manager::fin()
{
	m_soloDuleManager.fin();
	m_soloDuleManager_Sky.fin();
	m_soloDuleManager_SummitTeam.fin();
	m_soloDuleManager_GuildOffline.fin();
}

LC_Solo_Duel_Manager* LC_Dule_Manager::GetSoloDuleManager(int nType)
{
	LC_Solo_Duel_Manager* pResult = NULL;
	switch ( nType )
	{
	case GameLogic::eSoloDuleType_Arena:
		pResult = &m_soloDuleManager;
		break;
	case GameLogic::eSoloDuleType_GuildOffline:
		pResult = &m_soloDuleManager_GuildOffline;
		break;
	case GameLogic::eSoloDuleType_Sky:
		pResult = &m_soloDuleManager_Sky;
		break;
	case GameLogic::eSoloDuleType_Peak_Team:
		pResult = &m_soloDuleManager_SummitTeam;
		break;
	default:
		break;
	}
	return pResult;
}
