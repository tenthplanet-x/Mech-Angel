#include "LM_LocaleManager.h"
#include "MG_Common.h"
#include "PF_ServerPathManager.h"
#include "LC_PackAsset.h"
#include "MG_Game2Mail.h"
#include "NW_Helper.h"
#include "UT_ServerHelper.h"
#include "LC_ServerPlaceHolder.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerPlaceHolderManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "oss_define.h"
#include "oss_360_define.h"
#include "LC_LogicIDManager.h"
#include "LC_PackEntryFactory.h"
#include "LC_ItemFactoryBase.h"
#include "OSSDefine.h"
#include "LC_GameEventManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerGuildManager.h"
#include "LC_ServerRewardManager.h"
#include "LC_ActSubordinateRewardManager.h"
#include "ConfigManager.h"
#include "CF_ShopList.h"
#include "CF_WorldMapList.h"
#include "CF_GameNotice.h"
#include "CF_ItemGen.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"
#include "CF_ACTSubordinateRewardA.h"
#include "CF_TransferArea.h"
#include "CF_TaskRankRewards.h"
#include "CF_SpecialTask.h"
#include "ACT_Lottery.h"
#include "LC_Dule_Manager.h"
#include "LC_ServerPlayerShapshot.h"
#include "redispp.h"
#include "CF_BossInvadeRankReward.h"
#include "CF_BossInvade.h"
#include "CF_TestDatabase.h"
#include "GlobalSettings.h"
#include "LC_GameStory_Manager.h"
#include "CF_TestRankReward.h"
#include "CF_ShotActivityLimitRankRewards.h"
#include "CF_ACTSubordinateRewardB.h"
#include "CF_TaskList.h"
#include "CF_WorldObjectDesc.h"
#include "CF_BossFirstKill.h"
#include "MG_ProtocolStruct.h"
#include "MG_GameProtocol.h"
#include "CF_BestGuild.h"
#include "CF_BestGuildPersonalRankReward.h"
#include "CF_BestGuildGuildRankReward.h"
#include "CF_RegularBoss.h"
#include "CF_BestGuildBossInfo.h"
#include "CF_EscortReward.h"
#include "CF_BestGuildChapterID.h"
#include "redis.pb.h"
#include "LC_Helper.h"
#include "ACT_ActivitySchedule.h"
#include "CSVStructDefine.h"
#include "GameServerApp.h"
#include "MG_Game2Group.h"
#include "CF_RuneStar.h"
#include "CF_RuneLevelUp.h"
#include "MG_Game2Login.h"
#include "CF_YiJiTuXiRankRewards.h"
#include "CF_ItemDisassembleUseFunc.h"
#include "CF_GuildCrystal.h"
#include "CF_GuildGift.h"
#include "CF_TreasurePos.h"
#include "LC_NPCManagerBase.h"
#include "CF_GuildTower.h"
#include "CF_GameRestartNotice.h"
#include "MG_Game2DB.h"
#include "CF_BuddyThemeBossRankReward.h"
#include "CF_BuddyThemeBossType.h"
#include "md5plusplus.h"
#include "MG_ChatProtocol.h"
#include "UT_OperateLog.h"
#include "LC_ServerAllianceManager.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_GameStoryBase.h"
#include "MG_GroupProtocol.h"
#include "MG_SyncProtocol.h"
#include "LC_GameStory_Manager.h"
#include "CF_SkyArenaRankReward.h"
#include "CF_SkyArenaRank.h"
#include "CF_SummitArenaMail.h"
#include "CF_SummitArenaMatchName.h"
#include "CF_SummitArenaRankRewards.h"
#include "CF_SummitArenaGloryRankRewards.h"
#include "CF_Guess.h"
#include "CF_Scheme.h"
#include "LC_ServerMapLogic_SummitWheelPlay.h"
#include "CF_YiZhongJiangLinRankReward.h"
#include "LC_ServerNPCManager.h"
#include "LC_ActivityRankRewardMgr.h"
#include "CF_TestRankGuildReward.h"
#include "CF_ActivityRankReward.h"
#include "../Buddy/BuddyInstance.h"

using namespace OPERATELOG;
using namespace GameLogic;
using namespace Protocol;
using namespace CSVFile;
using namespace Activity;
using namespace redis_define;
using namespace Utility;

#define			CHAR_ID_STRING_LEN		128

#define			SNAPSHOT_POSTFIX			"_SnapShot"
//#define			SNAPSHOT_POSTFIX			""

//static struct BossFirstKillPlayerInfo;


int32_t UT_ServerHelper::m_nBuddyCount = 0;
bool UT_ServerHelper::m_nSettleYiZhongQinXiGameStory = false;
mem::map<uint64_t, int32_t> UT_ServerHelper::battlePassStartTime;
mem::map<uint64_t, int32_t> UT_ServerHelper::battlePassEndTime;
int32_t UT_ServerHelper::subPlusSaveTime = 0;
int32_t UT_ServerHelper::subSaveTime = 0;
int32_t UT_ServerHelper::sevenDayTrailEndTime = 0;
mem::map<int32_t, int32_t> UT_ServerHelper::investmentEndTime;
mem::map<int32_t, int32_t> UT_ServerHelper::YiZhongQinXiGameStoryEndTime;
mem::map<int32_t, mem::vector<uint64_t> > UT_ServerHelper::YiZhongQinXiRankPlayers;
int32_t UT_ServerHelper::YiZhongQinXiGameEndTime = 0;
mem::map<int32_t, int32_t> UT_ServerHelper::HuanJingEndTime;
int32_t UT_ServerHelper::AnswerActivityEndTime = 0;
TestDataBase UT_ServerHelper::TestData;
mem::vector<uint64_t> UT_ServerHelper::AnswerPlayers;
bool UT_ServerHelper::initAnswer = false;
uint32_t UT_ServerHelper::LoadAnswerInfoTime = 0;
bool UT_ServerHelper::m_nSettleAnswer = true;
AnswerAwardCheckTime UT_ServerHelper::CheckAwardTime;

mem::map<int, int> UT_ServerHelper::limitRewardRecord;
mem::set<int> UT_ServerHelper::limitActivityID;
mem::map<int, uint32_t> UT_ServerHelper::subBEndTime;
bool UT_ServerHelper::initSubB = false;
mem::map<int, uint32_t> UT_ServerHelper::subBClearTime;
mem::set<int32_t> UT_ServerHelper::rankGroupID;
mem::map<int, int> UT_ServerHelper::limitRewardRecordB;
mem::set<int> UT_ServerHelper::limitActivityIDB;
mem::map<int, mem::set<uint32_t> > UT_ServerHelper::subRewardPlayerIDs;
mem::map<int, mem::set<uint32_t> > UT_ServerHelper::subBRewardPlayerIDs;
mem::map<int, int> UT_ServerHelper::mapIDAndMirrorTaskIDRecords;
bool UT_ServerHelper::loadServerFirstKillBossInfo = false;
mem::map<int, BossFirstKillPlayerInfo> UT_ServerHelper::serverFirstKillBossRecord;
int32_t UT_ServerHelper::bestGuildEndTime = 0;
bool UT_ServerHelper::initGuildStoryID = false;
bool UT_ServerHelper::gsAppSendMail1 = true;
mem::map<int, int32_t> UT_ServerHelper::bestGuildActivityEndTime;
bool UT_ServerHelper::gsAppSendMail2 = true;
mem::map<uint64_t, RegularBossKillerInfo> UT_ServerHelper::regularBossKillerInfo;
bool UT_ServerHelper::initRegularBossKillerInfo = false;
mem::map<int32_t, int32_t> UT_ServerHelper::regularBossMapGroupInfo;
mem::map<uint64_t, std::string> UT_ServerHelper::bossKillerInfo;
mem::vector<int> UT_ServerHelper::escortNPCVec;
bool UT_ServerHelper::guildPresentExpInfoFlag = false;
bool UT_ServerHelper::initCSV=false;
mem::vector<int> UT_ServerHelper::escortTaskVec;
mem::map<int, mem::vector<int> > UT_ServerHelper::goalTypeAndID;
mem::map<int,int> UT_ServerHelper::rankIdCache;
mem::map<uint64_t, std::string> UT_ServerHelper::watchTowerStateResult;
uint32_t UT_ServerHelper::autoRewardNum = 0;
mem::map<uint32_t, RewardCache> UT_ServerHelper::RewardCacheMap;
mem::map<uint64_t, std::string> UT_ServerHelper::bossDamageInfo;
mem::vector<MailInfo> UT_ServerHelper::MainInfo;
mem::map<uint64_t,SingleFightRecord>  UT_ServerHelper::gCacheGuildDefendMap;
mem::map<uint32_t,RankOpenInfo> UT_ServerHelper::gRankOpenInfoMap;
mem::map<int32_t, int32_t> UT_ServerHelper::exportExportFlag;
mem::map<int32_t, mem::vector<uint32_t> > UT_ServerHelper::exportRobotInfo;
bool UT_ServerHelper::gbOpenFlag = false;
int32_t UT_ServerHelper::rewardTaskEndTime = 0;
mem::map<uint64_t, std::string> UT_ServerHelper::watchTowerInfoCache;
//mem::vector<int> UT_ServerHelper::bestGuildTaskScheduleID;
mem::vector<int> UT_ServerHelper::bestGuildStoryID;
bool UT_ServerHelper::watchTowerInfoFlag = false;
KillBossInfo UT_ServerHelper::killBossInfo;
mem::map<int, mem::map<int, RuneStarInfo> > UT_ServerHelper::runeStarInfo;
mem::map<int, mem::map<int, mem::vector<UT_Vec3Int> > > UT_ServerHelper::playerLevel2TreasuerPos;
mem::map<int, UT_ServerHelper::GuildMapBossHpInfo> UT_ServerHelper::guildMapBossHpInfo;
mem::vector<int> UT_ServerHelper::guildMapBossHpReward;
bool UT_ServerHelper::loadYiJiTuXi = false;
bool UT_ServerHelper::loadGuildMapBossInfo = false;
mem::map<int, mem::map<uint64_t, GuildBossInfo> > UT_ServerHelper::guildMapBossInfo;
int32_t UT_ServerHelper::gameStartTime = 0;
mem::set<uint64_t> UT_ServerHelper::restartRewardPlayerID;
mem::map<uint64_t, AnswerResult> UT_ServerHelper::offlinePlayerAnswerResult;
std::map<int, mem::map<uint64_t, int> > UT_ServerHelper::rankList;

mem::map<int32_t, mem::vector<int> > UT_ServerHelper::specialTaskInfo;
std::vector<MG_REQ_SendMailToGuild> UT_ServerHelper::sendMailSocietyMsg;
mem::map<uint64_t, uint64_t> UT_ServerHelper::m_nGuildMapStateMutableParams;
mem::map<uint64_t, uint64_t> UT_ServerHelper::m_nGuildBuyItemTimes;


static void test_reply(int32_t err, bool& ret)
{
	if (err)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RegisterCharInfo Err:%d Ret:%d", err, int(ret));
	}
}

/////////////////////////////////////////////
bool RankOpenInfo::InitRankOpenTimeByExecl(const CF_RankOpen::DataEntry* pkData)
{
	int64_t inputTimeStamp = GetGameServerApp()->GetServerManager()->GetEntryServerStartTime();
	if(pkData->_sStartTime.empty())
	{
		CSVFile::StartTime  tempStart = pkData->_StartTime1;
		uStartTime =	tempStart.GetTimeStramp(inputTimeStamp);
		int64_t timestamp = uStartTime;
		sStartTime = LC_Helper::TimeStampToString(&timestamp);
	}
	else
	{
		uStartTime = LC_Helper::DateTimeToTimeStamp(pkData->_sStartTime.c_str());
		sStartTime = pkData->_sStartTime.c_str();
	}
	if(pkData->_sEndTime.empty())
	{
		CSVFile::StartTime  tempEndTime = pkData->_EndTime1;
		uEndTime = tempEndTime.GetTimeStramp(inputTimeStamp);
		int64_t timestamp = uEndTime;
		sEndTime = LC_Helper::TimeStampToString(&timestamp);
	}
	else
	{
		uEndTime = LC_Helper::DateTimeToTimeStamp(pkData->_sEndTime.c_str());
		sEndTime = pkData->_sEndTime.c_str();
	}
	if(pkData->_sClearTime.empty())
	{

		CSVFile::StartTime  tempClearTime = pkData->_ClearTime1;
		uClearTime = tempClearTime.GetTimeStramp(inputTimeStamp);
		int64_t timestamp = uClearTime;
		sClearTime = LC_Helper::TimeStampToString(&timestamp);
	}
	else
	{
		uClearTime = LC_Helper::DateTimeToTimeStamp(pkData->_sClearTime.c_str());
		sClearTime = pkData->_sClearTime.c_str();
	}
	// 只要没过清档时间， 
	if(uClearTime > time(NULL))
	{
		bClearDbAndSocitey = true;
	}
	else
	{
		return false;
	}
	return true;
}
void RankOpenInfo::InitRankOpenIdListByExecl(const CF_RankOpen::DataEntry* pkData)
{
	mem::vector<int64_t> vecId;
	std::string sAllow(pkData->_sAllowId.c_str());
	LC_Helper::SpliteString2VecInt(sAllow,vecId,",");
	vecInId.clear();
	for(uint32_t i = 0; i < vecId.size();++i)
		vecInId.push_back(vecId[i]);
}
void RankOpenInfo::InitRankOpenInfoByExecl(const CF_RankOpen::DataEntry* pkData)
{
	InitRankOpenIdListByExecl(pkData);
	iRankId = pkData->_iId;
	bGuildReward = false;
	bPersonalReward = false;
	nConfigReward = pkData->_iReward;
}

///////////////////////////////////////////////////////////////////////////////////////
UT_ServerHelper::_GameNoticeFmort::_GameNoticeFmort(): m_nChannelType(0), m_nMsgID(0), m_strArgs(), m_numArgs(), m_i64Args(), m_argDesc()
{

}

UT_ServerHelper::_GameNoticeFmort::~_GameNoticeFmort()
{

}

void	UT_ServerHelper::_GameNoticeFmort::AddString(const StringType& str)
{
	m_strArgs.push_back(str);
	m_argDesc << ",s:" << m_strArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddNumber(int32_t num)
{
	m_numArgs.push_back(num);
	m_argDesc << ",n:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddSkyGrade(int32_t num)
{
	m_numArgs.push_back(num);
	m_argDesc << ",skygrade:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddBuddyStarLevel(int32_t starLevel)
{
	m_numArgs.push_back(starLevel);
	m_argDesc << ",bsl:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddUInt64Number(uint64_t num)
{
	m_i64Args.push_back(num);
	m_argDesc << ",un:" << m_i64Args.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddBuddyCID(int32_t num)
{
	m_numArgs.push_back(num);
	m_argDesc << ",c:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddCharType(int32_t num)
{
	m_numArgs.push_back(num);
	m_argDesc << ",npc:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddItemQuality(int32_t nID)
{
	m_numArgs.push_back(nID);
	m_argDesc << ",itquality:" << m_numArgs.size() - 1;
}

void UT_ServerHelper::_GameNoticeFmort::AddCollectionRewardName(int32_t nID)
{
	//CF_CollectionReward
	m_numArgs.push_back(nID);
	m_argDesc << ",coll:" << m_numArgs.size() - 1;
}

void UT_ServerHelper::_GameNoticeFmort::AddTitleName(int32_t nID)
{
	m_numArgs.push_back(nID);
	m_argDesc << ",title:" << m_numArgs.size() - 1;
}

void UT_ServerHelper::_GameNoticeFmort::AddCommanderTitleName(int32_t nID)
{
	//CF_CommanderTitle
	m_numArgs.push_back(nID);
	m_argDesc << ",commandertitle:" << m_numArgs.size() - 1;
}

void UT_ServerHelper::_GameNoticeFmort::AddMapID(int32_t nID)
{
	//CF_WorldMapList
	m_numArgs.push_back(nID);
	m_argDesc << ",map:" << m_numArgs.size() - 1;
}

void UT_ServerHelper::_GameNoticeFmort::AddAchievementName(int32_t nID)
{
	//CF_Achievement
	m_numArgs.push_back(nID);
	m_argDesc << ",achi:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddItemName(int32_t nID)
{
	m_numArgs.push_back(nID);
	m_argDesc << ",itname:" << m_numArgs.size() - 1;
}

void UT_ServerHelper::_GameNoticeFmort::AddItemNames(mem::set<int32_t>& itemID)
{
	if(itemID.empty())
	{
		return;
	}

	StringStreamType sStream;
	for(mem::set<int>::iterator it=itemID.begin(); it!=itemID.end(); ++it)
	{
		sStream << *it << ";";
	}
	m_strArgs.push_back(sStream.str());
	m_argDesc << ",itnames:" << m_strArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddSubordinateName(int32_t nID)
{
	m_numArgs.push_back(nID);
	m_argDesc << ",subname:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddFlyKicks(int32_t nServerID, int32_t nMapLogicID, int32_t nMapResID, const Utility::UT_Vec3Int& rPos)
{
	StringStreamType sFlyLicksData;
	sFlyLicksData << nServerID << "," << nMapLogicID << "," << nMapResID << "," << rPos.x << "," << rPos.y << "," << rPos.z;

	m_strArgs.push_back(sFlyLicksData.str());
	m_argDesc << ",flykicks:" << m_strArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddServerID(int32_t nID)
{
	m_numArgs.push_back(nID);
	m_argDesc << ",serverid:" << m_numArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddPathFound(int32_t nServerID, int32_t nMapLogicID, int32_t nMapResID, const Utility::UT_Vec3Int& rPos)
{
	StringStreamType sFlyLicksData;
	sFlyLicksData << nServerID << "," << nMapLogicID << "," << nMapResID << "," << rPos.x << "," << rPos.y << "," << rPos.z;

	m_strArgs.push_back(sFlyLicksData.str());
	m_argDesc << ",pathfind:" << m_strArgs.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddSendFlower(const unique_id_impl& userid)
{
	m_i64Args.push_back(userid.data());
	m_argDesc << ",sendflower:" << m_i64Args.size() - 1;
}

void	UT_ServerHelper::_GameNoticeFmort::AddPlayerInfo(const unique_id_impl& userid, const StringType& name)
{
	m_i64Args.push_back(userid.data());
	m_strArgs.push_back(name);
	m_argDesc << ",playerinfo:" << m_i64Args.size() - 1 << "_" << m_strArgs.size() - 1;
}

void UT_ServerHelper::_GameNoticeFmort::AddActivityId( int32_t nActivityId)
{
	m_numArgs.push_back(nActivityId);
	m_argDesc << ",ActivityId:" << m_numArgs.size() - 1;
}
///////////////////////////////////////////////////////////////////////////////////////

void initMailInfo(MG_GM_REQ_POST_MAIL& reqGMPostMail, const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, const unique_id_impl& senderid, int money, const StringType& send_name, int nMailType)
{
	reqGMPostMail.m_uiTransactionID = 0;
	reqGMPostMail.m_stMailInfo.lMailID = INVALID_MAIL_ID;					// 邮件编号
	reqGMPostMail.m_stMailInfo.bMailType = nMailType;						//邮件类型
	reqGMPostMail.m_stMailInfo.bPayFlag = false;							// 是否需要付费
	reqGMPostMail.m_stMailInfo.bReadFlag = false;							// 邮件是否已读
	reqGMPostMail.m_stMailInfo.bUntreadFlag = false;						// 退信标志
	reqGMPostMail.m_stMailInfo.lSenderCharID = senderid;						// 发件人ID
	if (send_name.length())
	{
		TPSTR2STDSTR(send_name, reqGMPostMail.m_stMailInfo.szSenderCharName);
	}
	reqGMPostMail.m_stMailInfo.lReceiverCharID = receiveid;					// 接收者ID
	reqGMPostMail.m_stMailInfo.m_iTimestamp = 0;							// 发送时间
	reqGMPostMail.m_stMailInfo.m_llMoney = money;							// 金钱附件

	// 主题
	TPSTR2STDSTR(mailname, reqGMPostMail.m_stMailInfo.szSubject);
	//邮件内容
	TPSTR2STDSTR(mailcontent, reqGMPostMail.m_stMailInfo.szContent);
	TPSTR2STDSTR(receivename, reqGMPostMail.m_stMailInfo.szReceiverCharName);
}

ResultType UT_ServerHelper::SendMailToPlayer(unique_id_type charID, const StringType& subject, const StringType& content, const Utility::UT_SIMDataList& itemlist, bool bGb2312, int nLogCode, int nExtraParams)
{
	StringType gb2312subject = subject;
	StringType gb2312content = content;

	if (bGb2312 == true)
	{
		gb2312subject = CodeConvert::Utf8ToMbs(subject);
		gb2312content = CodeConvert::Utf8ToMbs(content);
	}

	bool bContentReSet = false;
	if (gb2312content.empty())
	{
		bContentReSet = true;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return RE_FAIL;
	}

	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to", charID, cit->ToString().c_str());
		}
	}

	return SendToMail(gb2312subject, gb2312content, charID, "", 0, kEntryList, "system", nLogCode, nExtraParams);
}

ResultType UT_ServerHelper::SendMailToPlayer(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& rewardMsg, bool bGb2312,int nLogCode,int nExtraParams)
{
	StringType gb2312subject = subject;
	StringType gb2312content = content;

	if (bGb2312 == true)
	{
		gb2312subject = CodeConvert::Utf8ToMbs(subject);
		gb2312content = CodeConvert::Utf8ToMbs(content);
	}

	bool bContentReSet = false;
	if (gb2312content.empty())
	{
		bContentReSet = true;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return RE_FAIL;
	}

	Utility::UT_SIMDataList kItems;
	Utility::ParseFmtString2DataIFs(rewardMsg, kItems);

	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to", charID, cit->ToString().c_str());
		}
	}

	return SendToMail(gb2312subject, gb2312content, charID, "", 0, kEntryList, "system", nLogCode, nExtraParams);
}

ResultType UT_ServerHelper::SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, GameLogic::LC_BackPackEntryList& itemlist , const StringType& send_name,int nLogCode ,int nExtraParams)
{
	if (itemlist.empty())
	{
		return RE_SUCCESS;
	}

	MG_GM_REQ_POST_MAIL reqGMPostMail;
	static unique_id_impl senderid = GET_SYSTEM_ID();
	initMailInfo(reqGMPostMail, mailname, mailcontent, receiveid, receivename, senderid, money, send_name, MAIL_TYPE_ARENA);
	
	//埋点
	reqGMPostMail.m_stMailInfo.m_uiLogCode = nLogCode;
	reqGMPostMail.m_stMailInfo.m_iExtraParam = nExtraParams;

	//物品
	bool ret = false;
	uint32_t j = 0;
	bool sendMail = false;//是否发邮件
	for (uint32_t i = 0; i < itemlist.size(); ++i)
	{
		sendMail = true;
		itemlist[i].SetToProtocol(reqGMPostMail.m_stMailInfo.aItemAffix[j]);

		if(MAIL_ITEM_AFFIX_NUM-1 == j)
		{
			ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
			GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send mail req", mailname, mailcontent, receiveid, senderid, ret);

			j = 0;	
			sendMail = false;
			for(uint32_t k=0; k<sizeof(reqGMPostMail.m_stMailInfo.aItemAffix)/sizeof(reqGMPostMail.m_stMailInfo.aItemAffix[0]); ++k)
			{
				reqGMPostMail.m_stMailInfo.aItemAffix[k].clear();
			}
		}
		else
		{
			++j;
		}
	}

	if(sendMail)
	{
		ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
		GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send mail req", mailname, mailcontent, receiveid, senderid, ret);

		sendMail = false;
	}

	oss_role_send_mail(receiveid,reqGMPostMail.m_stMailInfo.szSubject, reqGMPostMail.m_stMailInfo.szContent, 0, (int)reqGMPostMail.m_stMailInfo.m_llMoney, 0, reqGMPostMail.m_stMailInfo.aItemAffix, MAIL_ITEM_AFFIX_NUM);
	return ret? RE_SUCCESS : RE_FAIL;
}

ResultType UT_ServerHelper::SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, GameLogic::LC_BackPackEntryPtrList& itemlist , const StringType& send_name,int nLogCode  ,int nExtraParams)
{
	if (itemlist.empty())
	{
		return RE_SUCCESS;
	}
	MG_GM_REQ_POST_MAIL reqGMPostMail;
	static unique_id_impl senderid = GET_SYSTEM_ID();
	initMailInfo(reqGMPostMail, mailname, mailcontent, receiveid, receivename, senderid, money, send_name, MAIL_TYPE_ARENA);
	//埋点
	reqGMPostMail.m_stMailInfo.m_uiLogCode = nLogCode;
	reqGMPostMail.m_stMailInfo.m_iExtraParam = nExtraParams;

	//物品
	bool ret = false;
	uint32_t j = 0;
	bool sendMail = false;//是否发邮件
	for (uint32_t i = 0; i < itemlist.size(); ++i)
	{
		sendMail = true;
		itemlist[i]->SetToProtocol(reqGMPostMail.m_stMailInfo.aItemAffix[j]);
		if(MAIL_ITEM_AFFIX_NUM-1 == j)
		{
			ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
			GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send mail req", mailname, mailcontent, receiveid, senderid, ret);

			j = 0;	
			sendMail = false;
			for(uint32_t k=0; k<sizeof(reqGMPostMail.m_stMailInfo.aItemAffix)/sizeof(reqGMPostMail.m_stMailInfo.aItemAffix[0]); ++k)
			{
				reqGMPostMail.m_stMailInfo.aItemAffix[k].clear();
			}
		}
		else
		{
			++j;
		}
	}
	
	if(sendMail)
	{
		ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
		GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send mail req", mailname, mailcontent, receiveid, senderid, ret);

		sendMail = false;
	}

	oss_role_send_mail(receiveid, reqGMPostMail.m_stMailInfo.szSubject, reqGMPostMail.m_stMailInfo.szContent, 0, (int)reqGMPostMail.m_stMailInfo.m_llMoney, 0, reqGMPostMail.m_stMailInfo.aItemAffix, MAIL_ITEM_AFFIX_NUM);
	return ret ? RE_SUCCESS : RE_FAIL;
}

ResultType UT_ServerHelper::SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, LC_ItemCntList& itemlist , const StringType& send_name,int nLogCode ,int nExtraParams)
{
	if (itemlist.empty())
	{
		return RE_SUCCESS;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	LC_BackPackEntryList kEntryList;
	for (size_t i = 0; i < itemlist.size(); ++i)
	{
		__ItemCnt& rItemCnt = itemlist[i];
		pkPackEntryFactory->CreateItemEntry(rItemCnt.m_nId, rItemCnt.m_nCnt, CIET_NONE, kEntryList, -1, 0, 0, true);
	}

	return SendToMail(mailname, mailcontent, receiveid, receivename, money, kEntryList, send_name, nLogCode, nExtraParams);
}

ResultType UT_ServerHelper::SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, const Utility::UT_SIMDataList& itemlist , const StringType& send_name,int nLogCode ,int nExtraParams)
{
	if(itemlist.empty())
	{
		return RE_SUCCESS;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit)
	{
		pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList);
	}
	return SendToMail(mailname, mailcontent, receiveid, receivename, money, kEntryList, send_name, nLogCode, nExtraParams);
}

ResultType UT_ServerHelper::SendToMailGuild(const StringType& mailname, const StringType& mailcontent, uint64_t guildId, int money, LC_ItemCntList& itemlist , const StringType& send_name,int nLogCode ,int nExtraParams)
{
	if(itemlist.empty())
	{
		return RE_SUCCESS;
	}

	//查找guild
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return RE_FAIL;
	}

	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(guildId);
	if (guild == NULL)
	{
		return RE_FAIL;
	}

	MG_GM_REQ_POST_MAIL reqGMPostMail;
	static unique_id_impl senderid = GET_SYSTEM_ID();
	initMailInfo(reqGMPostMail, mailname, mailcontent, 0, "", senderid, money, send_name, MAIL_TYPE_ARENA);
	reqGMPostMail.m_stMailInfo.m_uiLogCode = nLogCode;
	reqGMPostMail.m_stMailInfo.m_iExtraParam = nExtraParams;

	//物品
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	LC_BackPackEntryList kEntryList;
	for (size_t i = 0; i < itemlist.size(); ++i)
	{
		__ItemCnt& rItemCnt = itemlist[i];
		pkPackEntryFactory->CreateItemEntry(rItemCnt.m_nId, rItemCnt.m_nCnt, CIET_NONE, kEntryList, rItemCnt.m_nBindMode, 0, 0, true);
	}

	uint32_t j = 0;
	bool sendMail = false;//是否发邮件
	for (uint32_t i = 0; i < kEntryList.size(); ++i)
	{
		sendMail = true;
		kEntryList[i].SetToProtocol(reqGMPostMail.m_stMailInfo.aItemAffix[j]);
		if(MAIL_ITEM_AFFIX_NUM-1 == j)
		{
			guild->SendToMailMember(reqGMPostMail, senderid);

			j = 0;	
			sendMail = false;
			for(uint32_t k=0; k<sizeof(reqGMPostMail.m_stMailInfo.aItemAffix)/sizeof(reqGMPostMail.m_stMailInfo.aItemAffix[0]); ++k)
			{
				reqGMPostMail.m_stMailInfo.aItemAffix[k].clear();
			}
		}
		else
		{
			++j;
		}
	}
	
	if(sendMail)
	{
		guild->SendToMailMember(reqGMPostMail, senderid);
		sendMail = false;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send guild mail req", mailname, mailcontent, guildId, senderid);
	return RE_SUCCESS;
}

ResultType UT_ServerHelper::SendToMailAll(const StringType& mailname, const StringType& mailcontent, int money, GameLogic::LC_BackPackEntryList& itemlist, int32_t iServerID, const StringType& send_name,int nLogCode ,int nExtraParams)
{
	if(itemlist.empty())
	{
		return RE_SUCCESS;
	}

	MG_GM_REQ_POST_MAIL reqGMPostMail;
	unique_id_impl recieverid = GEN_UNIQUE_ID(GET_PLATFORM_ID(), (catagory_id_type)iServerID, 0);
	static unique_id_impl senderid = GET_SYSTEM_ID();
	initMailInfo(reqGMPostMail, mailname, mailcontent, recieverid, "", senderid, money, send_name, MAIL_TYPE_ALLCHAR);
	reqGMPostMail.m_stMailInfo.m_uiLogCode = nLogCode;
	reqGMPostMail.m_stMailInfo.m_iExtraParam = nExtraParams;

	//物品
	bool ret = false;
	uint32_t j = 0;
	bool sendMail = false;//是否发邮件
	for (uint32_t i = 0; i < itemlist.size(); ++i)
	{
		sendMail = true;
		itemlist[i].SetToProtocol(reqGMPostMail.m_stMailInfo.aItemAffix[j]);
		if(MAIL_ITEM_AFFIX_NUM-1 == j)
		{
			ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
			GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send global mail", mailname, mailcontent, recieverid, senderid, ret);

			j = 0;	
			sendMail = false;
			for(uint32_t k=0; k<sizeof(reqGMPostMail.m_stMailInfo.aItemAffix)/sizeof(reqGMPostMail.m_stMailInfo.aItemAffix[0]); ++k)
			{
				reqGMPostMail.m_stMailInfo.aItemAffix[k].clear();
			}
		}
		else
		{
			++j;
		}
	}

	if(sendMail)
	{
		ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
		GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send global mail", mailname, mailcontent, recieverid, senderid, ret);

		sendMail = false;
	}

	oss_role_send_mail(senderid, reqGMPostMail.m_stMailInfo.szSubject, reqGMPostMail.m_stMailInfo.szContent, 0, (int)reqGMPostMail.m_stMailInfo.m_llMoney, 0, reqGMPostMail.m_stMailInfo.aItemAffix, MAIL_ITEM_AFFIX_NUM);
	
	//全服邮件公告
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	noticeFmort.SetMsgID(48);
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort, iServerID);
	
	return ret ? RE_SUCCESS : RE_FAIL;
}

ResultType UT_ServerHelper::GM_SendToMailAll(const StringType& mailname, const StringType& mailcontent, int money, GameLogic::LC_BackPackEntryList& itemlist, int32_t iServerID, const StringType& send_name)
{
	if (itemlist.empty())
	{
		return RE_SUCCESS;
	}

	int32_t nServerEntryId = GetGameServerApp()->GetServerEntryId();
	int32_t nServerId = GetGameServerApp()->GetServerManager()->GetCurServerId();
	if (nServerEntryId == nServerId)
	{
		return RE_SUCCESS;
	}

	BOOL ret = true;
	unique_id_impl recieverid = GEN_UNIQUE_ID(GET_PLATFORM_ID(), (catagory_id_type)iServerID, 0);
	static unique_id_impl senderid = GET_SYSTEM_ID();
	MG_GM_REQ_POST_MAIL reqGMPostMail;
	initMailInfo(reqGMPostMail, mailname, mailcontent, recieverid, "", senderid, money, send_name, MAIL_TYPE_ALLCHAR);
	
	//物品
	uint32_t j = 0;
	bool sendMail = false;//是否发邮件
	for (uint32_t i = 0; i < itemlist.size(); ++i)
	{
		sendMail = true;
		itemlist[i].SetToProtocol(reqGMPostMail.m_stMailInfo.aItemAffix[j]);
		if(MAIL_ITEM_AFFIX_NUM-1 == j)
		{
			ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
			GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send global mail", mailname, mailcontent, recieverid, senderid, ret);

			j = 0;	
			sendMail = false;
			for(uint32_t k=0; k<sizeof(reqGMPostMail.m_stMailInfo.aItemAffix)/sizeof(reqGMPostMail.m_stMailInfo.aItemAffix[0]); ++k)
			{
				reqGMPostMail.m_stMailInfo.aItemAffix[k].clear();
			}
		}
		else
		{
			++j;
		}
	}

	if(sendMail)
	{
		ret = SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, SYSTEM_DETAIL_ID, senderid);
		GfxWriteFmtLog(LOG_INFO_FILELINE,  LOG_SWITCH_INFO, "send global mail", mailname, mailcontent, recieverid, senderid, ret);

		sendMail = false;
	}

	//全服邮件公告
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	noticeFmort.SetMsgID(48);
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort, iServerID);

	oss_role_send_mail(senderid, reqGMPostMail.m_stMailInfo.szSubject, reqGMPostMail.m_stMailInfo.szContent, 0, (int)reqGMPostMail.m_stMailInfo.m_llMoney, 0, reqGMPostMail.m_stMailInfo.aItemAffix, MAIL_ITEM_AFFIX_NUM);
	return ret ? RE_SUCCESS : RE_FAIL;
}

ResultType UT_ServerHelper::AddPlaceHolderEffect(int map_logic_id , int effect_id , int second , const Utility::UT_Vec3Int& pos)
{
	LC_ServerPlaceHolderManager* inst = SERVER_GET_PLACEHOLDER_MANAGER();
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();

	object_id_type lPlaceHolderID = LC_LogicIDManager::GetSingletonPtr()->RequestLogicID(LOT_PLACEHOLDER);
	LC_ServerPlaceHolder* the_ph = (LC_ServerPlaceHolder*)inst->CreatePlaceHolder(lPlaceHolderID);
	if (NULL == the_ph)
	{
		return RE_FAIL;
	}
	LC_Reward reward;
	reward.SetEffectID(effect_id);
	reward.SetLifeTime((float)second);

	the_ph->SetReward(reward);
	the_ph->SetMapID(map_logic_id);
	the_ph->SetPrivate(false);
	float fCurrentTime = GET_PROCESS_TIME;
	the_ph->SetLifeStartTime(fCurrentTime);
	the_ph->SetLifeEndTime(fCurrentTime + the_ph->GetLifeTime());
	the_ph->SetFromLocationPos(pos);
	the_ph->SetCurrentLocation(pos);
	pkWorldManager->AddObjectToMap(the_ph->GetMapID(), the_ph);
	return RE_SUCCESS;
}

LC_ServerMapLogic_Place* UT_ServerHelper::GetPlayerPlace(LC_ServerPlayer* player)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(player->GetMapLogicID());
	return ptr;
}

ResultType UT_ServerHelper::BrocastMsgStrServer(const StringType& msg, PromptChannel msgchannel, PromptLevel msglevel, int32_t nServerID)
{
	MG_NotifyMsg notifymsg;
	notifymsg.m_Type = msgchannel;
	notifymsg.m_MsgLevel = msglevel;
	TPSTR2STDSTR(msg, notifymsg.m_szSystemAnnounce);
	if(nServerID == 0)
	{
		nServerID = UT_ServerHelper::GetServerId();
	}
	SendMsgToChat(MGPT_NOTIFY_MSG, &notifymsg, 0, 0, nServerID, 0, MG_SERVER_ROUTER_FLAG_SINGLE);
	return RE_SUCCESS;
}

int UT_ServerHelper::GetServerId()
{
	return GetGameServerApp()->GetServerManager()->GetCurServerId();
}

int UT_ServerHelper::GetSrcServerId(LC_ServerPlayer* player)
{
	return GetGameServerApp()->GetServerManager()->GetActiveId(player->GetCitizenship());
}

bool UT_ServerHelper::CheckLocal(int serverid)
{
	return GetGameServerApp()->CheckLocalServer(serverid) == E_LocalServerState_Local;
}

bool UT_ServerHelper::CheckLocal(LC_ServerPlayer* player, int32_t nServerId)
{
	GameServerManager* pGameServerMgr = GetGameServerApp()->GetServerManager();
	return pGameServerMgr->GetActiveId(player->GetCitizenship()) == pGameServerMgr->GetActiveId(nServerId);
}

bool UT_ServerHelper::CheckLocal(LC_ServerPlayer* player)
{
	return GetGameServerApp()->CheckLocalServer(player ? player->GetCitizenship() : 0) == E_LocalServerState_Local;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool UT_ServerHelper::IsInCross(LC_ServerPlayer* player)
{
	if (NULL == player)
	{
		return false;
	}
	return IsInCross(player->GetInstance());
}

bool UT_ServerHelper::IsInCross(const unique_id_impl& id)
{
	if( 0 == id.catagory())
		return false;
	E_LocalServerState nState = GetGameServerApp()->CheckLocalServer(id.catagory());
	if(nState == E_LocalServerState_Local || nState == E_LocalServerState_Error)
	{
		return false;
	}
	return true;
}
bool UT_ServerHelper::GetFrontierLocation(LC_ServerPlayer* pkPlayer,UT_Vec3Int &Location,int32_t& MapResID)
{
	const unique_id_impl& instance = pkPlayer->GetInstance();
	LC_WorldLocationAssetEntry* pkFrontierLocation = pkPlayer->GetWorldLocationAsset()->GetWorldLocationAssetEntryByIndex(WLT_FRONTIER);
	if (NULL == pkFrontierLocation)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "GetFrontierLocation NULL == pkFrontierLocation cid:%d,citizenship:%d,uid:%d,id:%d ",instance.detail(),instance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID());
		return false;
	}
	if(pkFrontierLocation->GetValid() == true)
	{
		MapResID = pkFrontierLocation->GetMapResID();
		Location = pkFrontierLocation->GetLocation();
	}
	else
	{
		LC_ServerAllianceManager* pAllianceManager = LC_ServerAllianceManager::GetSingletonPtr();
		const AlliancePosInfo* pPosInfo = pAllianceManager->GetAlliancePosInfo(E_AllianceMarkType_Field);
		if(pPosInfo != NULL)
		{
			MapResID = pPosInfo->m_nMapRes;
			Location = pPosInfo->m_CrossPos;
		}
	}
	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_INFO, "GetFrontierLocation  cid:%d,citizenship:%d,uid:%d,id:%d MapResID:%d,(%d,%d,%d)",instance.detail(),instance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID(),MapResID,Location.x,Location.y,Location.z);
	return true;
}
bool UT_ServerHelper::GetRandomLocationXYZ(UT_Vec3Int &outPos,int mapId,int Citizenship,bool CrossFlag)
	{
		MapRoadmapManager* inst = SafeGetSingleton<MapRoadmapManager>();
		CF_MapCategory::DataEntry* roadmap = inst->GetRoadmap(mapId);
		if (NULL == roadmap)
			return false;
		CF_TransferArea* tr = CF_TransferArea::GetSingletonPtr();
		CF_TransferArea::DataEntry* dt = NULL;
		if(CrossFlag )
		{
			if(roadmap->_iCrossTransferAreaID <= 0)
				return false;
			dt = tr->GetEntryPtr(roadmap->_iCrossTransferAreaID);
		}
		else
		{
			dt = tr->GetEntryPtr(UT_ServerHelper::GetCroupServerIndex(roadmap->_iTransferAreaID, roadmap->_iPickTransferAreaType, Citizenship));
		}
		if(NULL == dt)
			return false;
		
		size_t sz = dt->_kRandomPos.size();
		if (sz <= 0)
		{
			outPos = dt->_kPos;
			return true;
		}
		const Utility::UT_SIMDataInfo kPos = dt->_kRandomPos[UT_MathBase::RandInRangeInt(0, (int)sz - 1)];
		outPos = UT_Vec3Int(kPos.X(), kPos.Y(), kPos.Z());
		return true;	
	}
///////////////////////////////////////////////////////////////////////////////////////////

object_id_type UT_ServerHelper::QueryPlayerID(const unique_id_impl& charID)
{
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	return enterLeaveManager->QueryPlayerID(charID);
}

bool UT_ServerHelper::CanCreateTeamInCross(LC_ServerPlayer* player)
{
	if (IsInCross(player) == false)
	{
		return true;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_ServerPlayerEnterLeaveManager::CrossRealmData data;
	if (enterLeaveManager->CheckCrossRealmIndex(player->GetID(), data) == false)
	{
		return false;
	}
	//不在目标服务器，不能创建队伍
	if (CheckLocal(data.m_TargetServerID) == false)
	{
		return false;
	}
	return true;
}

int UT_ServerHelper::GetActiveId(LC_ServerPlayer* player)
{
	return GetGameServerApp()->GetServerManager()->GetActiveId(player->GetCitizenship());
}

int UT_ServerHelper::GetServerCroup(int nServerID)
{
	GameServerManager* pServerMgr = GetGameServerApp()->GetServerManager();
	
	if(nServerID == 0)
	{
		nServerID = pServerMgr->GetCurServerId();
	}
	return pServerMgr->GetActiveGroupId(nServerID);
}

int UT_ServerHelper::GetCroupServerIndex(mem::vector<int>& idVector, int iPickType, int nServerID)
{
	if(idVector.size() <= 0)
	{
		return -1;
	}
	if(idVector.size() == 1)
	{
		return idVector[0];
	}
	GameServerManager* pServerMgr = GetGameServerApp()->GetServerManager();
	
	int nIndex = -1;
	if(pServerMgr->GetCurServerId() == nServerID)
	{
		nIndex = pServerMgr->GetCurGroupServerIDIndex();
	}
	else
	{
		nIndex = pServerMgr->GetGroupServerIDIndex(nServerID);
	}
	enum {
		PICK_SERVER_ORDER = 0,
		PICK_FACTION_WAR = 1,
	};
	if(nIndex < 0 || nIndex >= (int)idVector.size())
	{
		return -1;
	}
	//按服务器顺序
	if (PICK_SERVER_ORDER == iPickType)
	{
		return idVector[nIndex];
	}
	//按通天战场顺序
	else if (PICK_FACTION_WAR == iPickType)
	{
		int32_t iFirstServerID = GetFactionDefServer();
		if (iFirstServerID == 0)
		{
			return -1;
		}
		if (GetGameServerApp()->IsSameServer(iFirstServerID, nServerID))
		{
			return idVector[0];
		}
		else
		{
			int32_t iPowerServerIndex = pServerMgr->GetGroupServerIDIndex(iFirstServerID);
			if (iPowerServerIndex > nIndex)
			{
				nIndex += 1;
			}
			if(nIndex < 0 || nIndex >= (int)idVector.size())
			{
				return -1;
			}
			return idVector[nIndex];
		}
	}
	return -1;
}

bool UT_ServerHelper::GetGameNoticeFromString(const StringType& strContent, UT_ServerHelper::_GameNoticeFmort& noticeFmort)
{
	if (strContent.empty())
	{
		return false;
	}

	mem::vector<StringType> ItemInfo;
	char str1 = ';';
	LC_Helper::SpliteString2MultiString(strContent, ItemInfo, str1);
	if(ItemInfo.empty())
	{
		return false;
	}

	//id:60; s:aaa; npc:123456; int32:10
	for(mem::vector<StringType>::iterator it=ItemInfo.begin(); it!=ItemInfo.end(); ++it)
	{
		mem::vector<StringType> info;
		char str2 = ':';
		LC_Helper::SpliteString2MultiString(*it, info, str2);

		if(info.size() != 2)
		{
			return false;
		}

		if(0 == strcmp(TPSTR2STDSTR(info[0]).c_str(), "id"))
		{
			noticeFmort.SetMsgID(atoi(TPSTR2STDSTR(info[1]).c_str()));
		}			
		else if(0 == strcmp(TPSTR2STDSTR(info[0]).c_str(), "s"))
		{
			noticeFmort.AddString(info[1]);
		}
		else if(0 == strcmp(TPSTR2STDSTR(info[0]).c_str(), "npc"))
		{
			noticeFmort.AddCharType(atoi(TPSTR2STDSTR(info[1]).c_str()));
		}
		else if(0 == strcmp(TPSTR2STDSTR(info[0]).c_str(), "int32"))
		{
			noticeFmort.AddNumber(atoi(TPSTR2STDSTR(info[1]).c_str()));
		}
	}

	return true;
}

void UT_ServerHelper::NoticeNpcInfoToPlayer(LC_ServerPlayer* pkOwnerPlayer, int32_t storyID, int32_t mapResID)
{
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	GameStory::LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	GameStory::LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	if(NULL==pkGSMgr || NULL==pkLogicMgr)
	{
		return;
	}

	GameStory::LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
	if(NULL==pkStory || pkStory->GetConfig()->BroadCastMapID()!=mapResID)
	{
		return;
	}

	mem::map<int32_t, int32_t> monsterInfo;
	LC_PlayerIDPtrMap* playerIDPtrMap = NULL;
	pkStory->GetNpcAndPlayerInfo(mapResID, monsterInfo, &playerIDPtrMap);

	MG_SyncMapNpcInfo msg;
	msg.m_nResMapID = mapResID;
	msg.m_nNpcInfo.reserve(monsterInfo.size());
	for(mem::map<int32_t, int32_t>::iterator it=monsterInfo.begin(); it!=monsterInfo.end(); ++it)
	{
		KeyValue_Base data;
		data.key = it->first;
		data.value = it->second;
		msg.m_nNpcInfo.push_back(data);
	}
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCMAPNPCINFO, &msg);
}

void UT_ServerHelper::BroadcastNpcInfoToPlayer(int32_t storyID)
{
	GameStory::LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	GameStory::LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	if(NULL==pkGSMgr || NULL==pkLogicMgr)
	{
		return;
	}

	
	GameStory::LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
	if(NULL==pkStory || pkStory->GetConfig()->BroadCastMapID()==0)
	{
		return;
	}

	mem::map<int32_t, int32_t> monsterInfo;
	LC_PlayerIDPtrMap* playerIDPtrMap = NULL;
	pkStory->GetNpcAndPlayerInfo(pkStory->GetConfig()->BroadCastMapID(), monsterInfo, &playerIDPtrMap);
	if(NULL == playerIDPtrMap || playerIDPtrMap->empty())
	{
		return;
	}

	MG_SyncMapNpcInfo msg;
	msg.m_nResMapID = pkStory->GetConfig()->BroadCastMapID();
	msg.m_nNpcInfo.reserve(monsterInfo.size());
	for(mem::map<int32_t, int32_t>::iterator it=monsterInfo.begin(); it!=monsterInfo.end(); ++it)
	{
		KeyValue_Base data;
		data.key = it->first;
		data.value = it->second;
		msg.m_nNpcInfo.push_back(data);
	}

	for(LC_PlayerIDPtrMap::iterator it=playerIDPtrMap->begin(); it!=playerIDPtrMap->end(); ++it)
	{
		LC_ServerPlayer* pkOwnerPlayer = (LC_ServerPlayer*)it->second;
		if(NULL!=pkOwnerPlayer && pkOwnerPlayer->IsMainController())
		{
			pkOwnerPlayer->SendMsgToClient(MGPT_SYNCMAPNPCINFO, &msg);
		}
	}	
}

bool UT_ServerHelper::CheckStoryIsActive(int32_t storyID)
{
	GameStory::LC_GameStory_Manager* pkMgr =  SERVER_GET_GAMESTORY_MANAGER;
	if(NULL == pkMgr)
	{
		return false;
	}

	return pkMgr->StoryIsActive(storyID);
}

uint32_t UT_ServerHelper::GetStoryStartTime(int storyID)
{
	GameStory::LC_GameStory_Manager* pkMgr =  SERVER_GET_GAMESTORY_MANAGER;
	if(NULL == pkMgr)
	{
		return 0;
	}

	return pkMgr->GetStoryStartTime(storyID);
}

uint32_t UT_ServerHelper::GetStoryEndTime(int32_t storyID)
{
	GameStory::LC_GameStory_Manager* pkMgr =  SERVER_GET_GAMESTORY_MANAGER;
	if(NULL == pkMgr)
	{
		return 0;
	}

	return pkMgr->GetStoryEndTime(storyID);
}

void UT_ServerHelper::GetTopicVaildTime(int topicIndex, uint32_t& minTime, uint32_t& maxTime)
{
	uint32_t startTime = UT_ServerHelper::GetStoryStartTime(GameStoryIDType_Answer);

	minTime = startTime + GetGlobalSetting.answerReadyTime + (GetGlobalSetting.answerVaildTime+GetGlobalSetting.answerCountDownTime)*topicIndex;
	maxTime = startTime + GetGlobalSetting.answerReadyTime + GetGlobalSetting.answerVaildTime*(topicIndex+1) + GetGlobalSetting.answerCountDownTime*topicIndex;
}

uint32_t UT_ServerHelper::GetCheckAwardTime(int topicIndex)
{
	uint32_t startTime = UT_ServerHelper::GetStoryStartTime(GameStoryIDType_Answer);
	
	uint32_t checkTime = (startTime+GetGlobalSetting.answerReadyTime+GetGlobalSetting.answerVaildTime*(topicIndex+1)+GetGlobalSetting.answerCountDownTime*topicIndex)-1;

	return checkTime;
}

int UT_ServerHelper::GetActiveId(int nServerId)
{
	if(nServerId == 0)
	{
		return GetServerId();
	}
	return GetGameServerApp()->GetServerManager()->GetActiveId(nServerId);
}

bool UT_ServerHelper::SyncBasicPackToPlayer(int32_t reasonType)
{
	bool tag = true;
	switch(reasonType)
	{
	case SYS_FUNC_TP_GUILDPRESENT_REWARD:
		{
			tag = false;
			break;
		}
	default:
		break;
	}

	return tag;
}

ResultType UT_ServerHelper::HandleBonusItems(int32_t iPackType, const Utility::UT_SIMDataList& items, GameLogic::LC_PlayerBase* pkPlayer, int32_t iReasonType, uint64_t iReasonDetail, const StringType& title, const StringType& desc, bool toMail, bool bAutoUse, bool bLotteryLogic,int nLogCode,const vector<int> &Params)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (NULL == pkItemFactory)
	{
		return RE_FAIL;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return RE_FAIL;
	}

	if (items.empty())
	{
		return RE_SUCCESS;
	}

	//LC_BackPackEntryList kEntryList;
	//策划：如果不能把所有奖励都领进背包，则所有的奖励走邮件
	ResultType res = LC_Helper::CanAddItemEx(pkPlayer, items, CIET_NONE);
	if(RE_SUCCESS == res)
	{
		PackOperateInfoList kItemOPs;
		LC_Helper::BuildAddItemOPs(items, kItemOPs, iReasonType, iReasonDetail);

		bool sync = UT_ServerHelper::SyncBasicPackToPlayer(iReasonType);
		res = LC_Helper::DoItemOPs(pkPlayer, kItemOPs, iReasonType, iReasonDetail, sync, sync, nLogCode, Params);
		if(RE_SUCCESS==res && bLotteryLogic)
		{
			for (Utility::UT_SIMDataList::const_iterator cit = items.begin(); cit != items.end(); ++cit)
			{
				UT_SIMDataInfo info = *cit;
				int32_t nItemId = info.X();
				int32_t nCount = info.Y();
				int32_t nGoalID = info.ParamC();
				int32_t StoryID = info.ParamD();

				Activity::ACT_LotteryManager::AddSingleRewardItem(nItemId, nCount);			
			}
		}		
	}
	else if(RE_ITEM_PACKAGE_FULL==res && toMail)
	{
		LC_BackPackEntryList kEntryList;
		for (Utility::UT_SIMDataList::const_iterator cit = items.begin(); cit != items.end(); ++cit)
		{
			pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList);
		}
		res = SendToMail(title, desc, pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName(), 0, kEntryList,"",nLogCode);
	}
	return res;
}

void UT_ServerHelper::TryAutoUseItem(LC_ServerPlayer* player, uint32_t nBuddyId, int32_t nItemId, int nPackType, int nSlot, int nCount)
{
	ResultType iUseResult = player->UseItemInPackByIndex(nPackType, nSlot, nCount, -1, "", nBuddyId);
	if (iUseResult == RE_ITEM_USE_SUCCESS)
	{
		MG_RLT_UseItemInPackage sucMsg;
		sucMsg.m_lItemID = nItemId;
		sucMsg.m_iParam = nCount;
		player->SendMsgToClient(MGPT_RLT_USE_ITEM_IN_PACKAGE, &sucMsg);
		oss_360_log_item_log(nItemId, SYS_FUNC_TP_ITEM_USE, -nCount, player);
	}

}

void UT_ServerHelper::HandlePackOPRecord(LC_ServerPlayer* pkPlayer, PackOPRecordSet* pkPackOpRecords, int32_t iReasonType, uint64_t iReasonDetail)
{
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	if (NULL == pkPackOpRecords || NULL == pkPlayer)
	{
		return;
	}
	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();

	bool bProfitStat = false;
	if (IS_PROFIT_STAT_RELATED(iReasonType))
	{
		CF_WorldMapList::DataEntry* pkMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(pkPlayer->GetMapResID());
		bProfitStat = pkMapEntry && pkMapEntry->_ProfitStat > 0;
	}

	int32_t packType = PACK_TYPE_BASIC;
	mem::map<int32_t, int32_t> ItemInfo;
	for (PackOPRecordSet::PackOPRecords::iterator it = pkPackOpRecords->m_Add_Items.begin(); it != pkPackOpRecords->m_Add_Items.end(); ++it)
	{
		PackEntryOPRecord& rd = *it;
		packType = rd.m_lPackType;
		LC_BackPackEntry* pack = rd.m_Entry;
		if (pack)
		{
			if(rd.m_lNum > 0)
			{
				mem::map<int32_t, int32_t>::iterator findIt = ItemInfo.find(pack->GetBackPackValueByType(LPSFET_TYPE_ID));
				if(findIt == ItemInfo.end())
				{
					ItemInfo[pack->GetBackPackValueByType(LPSFET_TYPE_ID)] = rd.m_lNum;
				}
				else
				{
					findIt->second += rd.m_lNum;
				}
			}

			int itemID = pack->GetBackPackValueByType(LPSFET_TYPE_ID);
			if (bProfitStat)
			{
				pkPlayer->GetCommonStatAsset().OnGotItem(itemID, rd.m_lNum, pack->GetBackPackValueByType(LPSFET_BINDMODE), iReasonType);
			}
			CF_ItemList::DataEntry* pkData = pkCSVItemList->GetEntryPtr(itemID);
			if (NULL != pkData && !pkData->_bSkipOssLog)
			{
				StringType _log = oss_role_produceitem(pkPlayer, pack, iReasonType, iReasonDetail, pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) - rd.m_lNum, pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT),0);
				GfxWriteFmtLog(LOG_ITEM_FILELINE, LOG_SWITCH_ITEM, _log);
			}
			pkPlayer->SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
			pkPlayer->AddResource(itemID, iReasonType, rd.m_lNum);
		}
	}

	pkPlayer->NoticeAddItemGameEvent(packType, ItemInfo, iReasonType);

	for (PackOPRecordSet::PackOPRecords::iterator it = pkPackOpRecords->m_Delete_Items.begin(); it != pkPackOpRecords->m_Delete_Items.end(); ++it)
	{
		PackEntryOPRecord& rd = *it;
		LC_BackPackEntry* pack = &rd.m_CloneEntry;

		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_USE_ITEM);
			pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
			pkGameEvent->SetEventParam(EP_USE_ITEM_PLAYER_ID, pkPlayer->GetID());
			pkGameEvent->SetEventParam(EP_USE_ITEM_TYPE_ID, pack->GetBackPackValueByType(LPSFET_TYPE_ID));
			pkGameEvent->SetEventParam(EP_USE_ITEM_COUNT, rd.m_lNum);
			pkGameEvent->SetEventParam(EP_USE_ITEM_REASON, iReasonType);
			pkGameEvent->SetEventParam(EP_USE_ITEM_MAP_ID, pkPlayer->GetMapResID());
			pkEventManager->InsertGameEvent(pkGameEvent);

			pkPlayer->SendGameEvent(pkGameEvent);
		}
		CF_ItemList::DataEntry* pkData = pkCSVItemList->GetEntryPtr(pack->GetBackPackValueByType(LPSFET_TYPE_ID));
		if (NULL != pkData && !pkData->_bSkipOssLog)
		{
			StringType _log = oss_role_consumeitem(pkPlayer, pack, iReasonType, iReasonDetail, pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT), pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) - rd.m_lNum, 0);
			GfxWriteFmtLog(LOG_ITEM_FILELINE, LOG_SWITCH_ITEM, _log);
		}
		pkPlayer->SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
	}

	for (PackOPRecordSet::PackOPRecords::iterator it = pkPackOpRecords->m_Spec_Items.begin(); it != pkPackOpRecords->m_Spec_Items.end(); ++it)
	{
		PackEntryOPRecord& rd = *it;
		LC_BackPackEntry* pack = &rd.m_CloneEntry;
		CF_ItemList::DataEntry* pkData = pkCSVItemList->GetEntryPtr(pack->GetBackPackValueByType(LPSFET_TYPE_ID));
		if (NULL != pkData && !pkData->_bSkipOssLog)
		{
			StringType _log = oss_role_produceitem(pkPlayer, pack, iReasonType, iReasonDetail, pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) - rd.m_lNum, pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT), 0);
			GfxWriteFmtLog(LOG_ITEM_FILELINE, LOG_SWITCH_ITEM, _log);
		}
		pkPlayer->SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
	}
}

void UT_ServerHelper::FillGameNoticeMsg(const _GameNoticeFmort& msgFmort, MG_GameNotice& notifymsg)
{
	notifymsg.m_nID = msgFmort.m_nMsgID;
	notifymsg.m_nChannelType = msgFmort.m_nChannelType;
	for (size_t i = 0; i < msgFmort.m_strArgs.size(); i++)
	{
		notifymsg.m_strArgs.push_back(TPSTR2STDSTR(msgFmort.m_strArgs[i]));
	}
	for (size_t i = 0; i < msgFmort.m_numArgs.size(); i++)
	{
		notifymsg.m_numArgs.push_back(msgFmort.m_numArgs[i]);
	}
	for (size_t i = 0; i < msgFmort.m_i64Args.size(); i++)
	{
		notifymsg.m_i64Args.push_back(msgFmort.m_i64Args[i]);
	}
	TPSTR2STDSTR(msgFmort.m_argDesc.str(), notifymsg.m_sArgDesc);
}

ResultType UT_ServerHelper::BrocastNoticeMsg(const _GameNoticeFmort& msgFmort, uint64_t nServerID, LC_ServerPlayer* player, LC_ServerMapLogic_Place* pLogicPlace)
{
	CF_GameNotice::DataEntry* pGameNotice = CF_GameNotice::Instance()->GetEntryPtr(msgFmort.m_nMsgID);
	if(pGameNotice == NULL)
	{
		return RE_FAIL;
	}

	MG_GameNotice notifymsg;
	FillGameNoticeMsg(msgFmort, notifymsg);
	
	switch(pGameNotice->_CastRange)
	{
	case 0:					//0自己
		{
			if(player != NULL)
			{
				player->SendMsgToClient(MGPT_GAME_NOTICE_MSG, &notifymsg);
			}
		}
		break;
	case 1:					//1本地图本服
		{
			if(pLogicPlace != NULL && player != NULL)
			{
				pLogicPlace->BroadcastMsgToServerClient(player->GetCitizenship(), MGPT_GAME_NOTICE_MSG, &notifymsg);
			}
		}
		break;
	case 2:					//2本地图全服
		{
			if(pLogicPlace != NULL)
			{
				pLogicPlace->BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &notifymsg);
			}
		}
		break;
	case 3:					//3本服
		{
			BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &notifymsg, false, nServerID);
		}
		break;
	case 4:					//4本区
		{
			BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &notifymsg, true, nServerID);
		}
		break;
	case 5:					//5帮会
		{
			MG_SS_GUILD_TRANS_DATA reqMsg;
			reqMsg.m_msgType = MGPT_GAME_NOTICE_MSG;
			reqMsg.m_guildId = nServerID;
			reqMsg.m_msgInfo.resize(MAX_MULTICAST_MESSAGE_SIZE);
			notifymsg.Encode(reqMsg.m_msgInfo);
			SendMsgToSociety(MGPT_REQ_GUILD_TRANS_DATA, &reqMsg);
		}
		break;
	case 6: //本节点
		{
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			pkWorldManager->BroadCast(MGPT_GAME_NOTICE_MSG, &notifymsg);
		}break;
	}
	return RE_SUCCESS;
}

std::string UT_ServerHelper::GetRedisStr(const char* szNameSpace, mem::vector<uint64_t>& params, bool bGlobalServer)
{
	GameServerApp* pkApp = GetGameServerApp();

	std::string sKey = "";
	if (bGlobalServer == false)
	{
		sKey += pkApp->GetRedisEntryServerPredix();
	}

	std::string str = GenDetailKeyWithoutData(params);	

	sKey += szNameSpace;
	sKey += ":";
	sKey += str;

	return sKey;
}

std::string UT_ServerHelper::GetRedisStr(const char* szNameSpace, bool bGlobalServer, bool snapShot)
{
	std::string sKey = "";
	GameServerApp* pkApp = GetGameServerApp();
	if (bGlobalServer == false)
	{
		sKey += pkApp->GetRedisEntryServerPredix();
	}
	sKey += szNameSpace;

	if(snapShot)
	{
		sKey += SNAPSHOT_POSTFIX;
	}

	return sKey;
}

std::string UT_ServerHelper::GetRedisStr_EntryID(const char* szNameSpace)
{
	int entryServerID = GetGameServerEntryID();
	std::stringstream tempStream;
	tempStream << entryServerID;

	std::string sKey = "";
	sKey += szNameSpace;
	sKey += tempStream.str();

	return sKey;
}

void UT_ServerHelper::SettleYiZhongJiangLinReward()
{
	if(!GetGameServerApp()->IsNodeType_Global())
	{
		return;
	}

	//从redis中取应该发奖的活动storyID，如果不是0，则发奖励
	char* redisNamespace = UT_REDIS_KEY_YIZHONGJIANGLIN_STORY;
	int32_t settleStoryID = 0;
	mem::vector<uint64_t> key;
	key.push_back(GameStoryIDType_YiZhongJiangLin);
	bool flag = UT_ServerHelper::RedisGetData_Int(redisNamespace, key, settleStoryID);
	if(!flag)
	{
		return;
	}

	mem::vector<uint64_t> params;
	params.push_back(settleStoryID);

	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN19_OPERATE, OPERATE_LOGTYPE1);
	mem::vector<uint64_t> allPlayers;
	UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_DAMAGE_RANK, params, allPlayers);
	uint32_t maxPlayerRankIndex = allPlayers.size();
	if(0 == maxPlayerRankIndex)
	{
		return;
	}

	CF_YiZhongJiangLinRankReward::DataEntryMapExternIterator kIter1 = CF_YiZhongJiangLinRankReward::GetSingletonPtr()->GetIterator();
	for(; kIter1.HasMoreElements(); kIter1.MoveNext())
	{
		CF_YiZhongJiangLinRankReward::DataEntry* pkMailData = kIter1.PeekNextValuePtr();
		if(1 == pkMailData->_iType)
		{
			uint32_t i = pkMailData->_iRankStartIndex;
			for( ; i<pkMailData->_iRankEndIndex&&i<maxPlayerRankIndex; ++i)
			{
				std::stringstream ss;
				ss << (i+1) << ";";
				StringType mailDesc = pkMailData->_iMailDesc +  STDSTR2TPSTR(ss.str()); 
				UT_ServerHelper::SendMailToPlayer(allPlayers[i], pkMailData->_iMailTitle, mailDesc, pkMailData->_kReward, false, nLogCode, i+1);
			}

			if(i >= maxPlayerRankIndex)
			{
				break;
			}
		}
	}

	MG_REQ_SendMailToGuild reqMsg;
	reqMsg.m_nType = GuildMailType_YiZhongJiangLin;
	reqMsg.m_nIntParam.push_back(settleStoryID);

	mem::vector<uint64_t> allGuildID;
	UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_GUILD_DAMAGE_RANK, params, allGuildID);
	uint32_t maxGuildRankIndex = allGuildID.size();
	CF_YiZhongJiangLinRankReward::DataEntryMapExternIterator kIter2 = CF_YiZhongJiangLinRankReward::GetSingletonPtr()->GetIterator();
	for(; kIter2.HasMoreElements(); kIter2.MoveNext())
	{
		CF_YiZhongJiangLinRankReward::DataEntry* pkMailData = kIter2.PeekNextValuePtr();
		if(2 == pkMailData->_iType)
		{
			for(uint32_t i=pkMailData->_iRankStartIndex; i<pkMailData->_iRankEndIndex&&i<maxGuildRankIndex; ++i)
			{
				std::stringstream ss;
				ss << (i+1) << ";";
				StringType mailDesc = pkMailData->_iMailDesc +  STDSTR2TPSTR(ss.str());

				reqMsg.m_nGuildId = allGuildID[i];
				reqMsg.m_nStringParam.clear();
				reqMsg.m_nStringParam.push_back(TPSTR2STDSTR(pkMailData->_iMailTitle));
				reqMsg.m_nStringParam.push_back(TPSTR2STDSTR(mailDesc));
				reqMsg.m_nStringParam.push_back(TPSTR2STDSTR(pkMailData->_kReward));

				UT_ServerHelper::UpdateSocietySendMailMsg(reqMsg);
			}
		}		
	}
}

void UT_ServerHelper::SendGuildMail(const MG_RESP_SendMailToGuild& msg)
{
	std::stringstream logStr;
	logStr << "SendGuildMail type:" << msg.m_nType << " guildID:" << msg.m_nGuildId;
	switch(msg.m_nType)
	{
	case GuildMailType_YiZhongJiangLin:
		{
			if(msg.m_nStringParam.size() != 3 || msg.m_nIntParam.size()!=1)
			{
				logStr << " param error! " << msg.m_nStringParam.size() << " " << msg.m_nIntParam.size();
				break;
			}

			mem::vector<uint64_t> params;
			params.push_back(msg.m_nIntParam[0]);

			uint64_t damageData = 0;
			int32_t index = 0;

			int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN20_OPERATE, OPERATE_LOGTYPE1);
			for(uint32_t i=0; i<msg.m_AllGuildNumberID.size(); ++i)
			{				
				bool tag = UT_ServerHelper::RedisRank_ReadDamage(UT_REDIS_KEY_DAMAGE_RANK, params, msg.m_AllGuildNumberID[i], damageData, index);
				if(!tag)
				{
					logStr << " NDPID:" << msg.m_AllGuildNumberID[i];
					continue;
				}

				logStr << " DPID:" << msg.m_AllGuildNumberID[i];
				UT_ServerHelper::SendMailToPlayer(msg.m_AllGuildNumberID[i], STDSTR2TPSTR(msg.m_nStringParam[0]), STDSTR2TPSTR(msg.m_nStringParam[1]), STDSTR2TPSTR(msg.m_nStringParam[2]), false, nLogCode, i+1);
			}
		}
		break;
	default:
		break;
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", logStr.str().c_str());
}

void UT_ServerHelper::UpdateSocietySendMailMsg(const MG_REQ_SendMailToGuild& msg)
{
	if(sendMailSocietyMsg.empty())
	{
		sendMailSocietyMsg.reserve(20);
	}

	sendMailSocietyMsg.push_back(msg);
}

void UT_ServerHelper::SendMailMsgToSociety()
{
	if(sendMailSocietyMsg.empty())
	{
		return;
	}

	for(uint32_t i=0; i<sendMailSocietyMsg.size(); ++i)
	{
		SendMsgToSociety(MGPT_SS_REQ_SENDMAILTOGUILD, &sendMailSocietyMsg[i]);
	}
	sendMailSocietyMsg.clear();
}

void UT_ServerHelper::SettleYiZhongQinXiGameStory()
{
	//entryServer负责发奖
	if(GetGameServerID() != GetGameServerEntryID())
	{
		return;
	}

	if(!m_nSettleYiZhongQinXiGameStory)
	{
		uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		CF_BossInvade::DataEntryMapExternIterator kIter = CF_BossInvade::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_BossInvade::DataEntry* pkData = kIter.PeekNextValuePtr();
			int storyID = pkData->_iStoryID;

			int32_t endTime = UT_ServerHelper::GetYiZhongQinXiGameStoryEndTime(storyID);
			if(endTime == -1)
			{
				mem::vector<uint64_t> input;
				input.push_back(storyID);
				int endTime = 0;
				UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_STORT_END_TIME, input, endTime);
				UT_ServerHelper::SetYiZhongQinXiGameStoryEndTime(storyID, endTime);
			}

			if(endTime!=-1 && endTime!=0 && currentTime>=endTime)
			{
				UT_ServerHelper::SettleYiZhongQinXi(storyID, true);
			}
		}
	}
}

void UT_ServerHelper::SettleYiZhongQinXi(int storyID, bool init)
{
	//entryServer负责发奖
	if(GetGameServerID() != GetGameServerEntryID())
	{
		return;
	}

	mem::vector<uint64_t> params;
	params.push_back(storyID);

	mem::vector<uint64_t> allPlayers;
	if(init)
	{
		bool tag = UT_ServerHelper::GetYiZhongQinXiPlayer(storyID, allPlayers);
		if(!tag)
		{
			UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_DAMAGE_RANK, params, allPlayers);
			UT_ServerHelper::SetYiZhongQinXiPlayer(storyID, allPlayers);
		}
	}
	else
	{
		UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_DAMAGE_RANK, params, allPlayers);
	}

	ResultType res = RE_FAIL;
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN15_OPERATE, OPERATE_LOGTYPE1);
	for(int i=0; i<allPlayers.size(); ++i)
	{
		int rankIndex = i;
		unique_id_type charID = allPlayers[i];
		CF_BossInvadeRankReward::DataEntryMapExternIterator kIter = CF_BossInvadeRankReward::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_BossInvadeRankReward::DataEntry* pkData = kIter.PeekNextValuePtr();
			if(pkData->_iStoryID==storyID && rankIndex>=pkData->_iRankStartIndex && rankIndex<pkData->_iRankEndIndex)
			{
				CF_BossInvade::DataEntry* pkMailData = SafeGetCSVFileDataEntryPtr<CF_BossInvade>(storyID);
				std::stringstream ss;
				ss << (rankIndex+1) << ";";
				StringType mailDesc = pkMailData->_emailDesc +  STDSTR2TPSTR(ss.str());
				res = UT_ServerHelper::SendMailToPlayer(charID, pkMailData->_emailTitle, mailDesc, pkData->_kReward, false, nLogCode, rankIndex+1);
				if(RE_SUCCESS == res)
				{
					mem::vector<uint64_t> nameKey;
					nameKey.push_back(storyID);
					nameKey.push_back(charID);
					UT_ServerHelper::RedisClearString(UT_REDIS_KEY_COMBAT_INFO, nameKey);
				}
			}
		}
	}

	if(RE_SUCCESS == res)
	{
		mem::vector<uint64_t> params;
		params.push_back(storyID);

		UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_BATTLE_INFO, params);
		UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_DAMAGE_RANK, params);
		m_nSettleYiZhongQinXiGameStory = true;
	}
}

void UT_ServerHelper::GetGuildDamageRankFromRedis(LC_ServerPlayer* pkOwnerPlayer, int storyID, int minIndex, int maxIndex)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_GetGuildDamageRank rltMsg;
	rltMsg.m_nStoryID = storyID;
	rltMsg.m_nMinIndex = minIndex;
	rltMsg.m_nMaxIndex = maxIndex;
	rltMsg.m_nMyselfGuildInfo.m_nGuildID = pkOwnerPlayer->GetGuildID();
	rltMsg.m_nMyselfGuildInfo.m_nGuildName = pkOwnerPlayer->GetGuildName().c_str();
	rltMsg.m_nMyselfGuildInfo.m_nRankIndex = SPECIES_RANK_NONE;
	rltMsg.m_nResult = RE_FAIL;

	CF_GameStory::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GameStory>(storyID);
	if(minIndex > maxIndex || NULL==pkData)
	{
		rltMsg.m_nResult = RE_ERROR_PARAM;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETGUILDDAMAGERANK, &rltMsg);
		return;
	}
	
	mem::vector<uint64_t> params;
	params.push_back(storyID);

	mem::vector<uint64_t> allGuildID;
	UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_GUILD_DAMAGE_RANK, params, allGuildID);

	if(0 == allGuildID.size())
	{
		rltMsg.m_nResult = RE_SUCCESS;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETGUILDDAMAGERANK, &rltMsg);
		return;
	}

	uint64_t myselfGuildID = pkOwnerPlayer->GetGuildID();
	for(uint32_t i=0; i<allGuildID.size(); ++i)
	{
		if(allGuildID[i] == myselfGuildID)
		{
			UT_ServerHelper::GetGuildTotalDamageInfo(storyID, allGuildID[i], rltMsg.m_nMyselfGuildInfo);
			rltMsg.m_nMyselfGuildInfo.m_nRankIndex = i;
			break;
		}
	}

	maxIndex = maxIndex > allGuildID.size()-1 ? allGuildID.size()-1 : maxIndex;
	rltMsg.m_nGuildInfo.reserve(maxIndex-minIndex);
	for(int i=minIndex; i<=maxIndex; ++i)
	{
		PS_GuildDamageInfo data;
		UT_ServerHelper::GetGuildTotalDamageInfo(storyID, allGuildID[i], data);
		data.m_nRankIndex = i;

		rltMsg.m_nGuildInfo.push_back(data);
	}
	
	rltMsg.m_nTotalCount = allGuildID.size();
	rltMsg.m_nResult = RE_SUCCESS;
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETGUILDDAMAGERANK, &rltMsg);
}

void UT_ServerHelper::GetPlayerDamageRankFromRedis(LC_ServerPlayer* pkOwnerPlayer, int storyID, int minIndex, int maxIndex)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_GetRankYiZhongQinXi msg;
	msg.m_nStoryID = storyID;
	msg.m_nResult = RE_FAIL;
	msg.m_nTotalCount = 0;
	msg.m_nMyselfInfo.m_nCharID = pkOwnerPlayer->GetInstance();
	msg.m_nMyselfInfo.m_nRankIndex = SPECIES_RANK_NONE;

	CF_GameStory::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GameStory>(storyID);
	if(minIndex > maxIndex || NULL==pkData)
	{
		msg.m_nResult = RE_ERROR_PARAM;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETRANKYIZHONGQINXI, &msg);
		return;
	}

	mem::vector<uint64_t> params;
	params.push_back(storyID);

	mem::vector<uint64_t> allPlayers;
	UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_DAMAGE_RANK, params, allPlayers);
	if(allPlayers.size() <= 0)
	{
		msg.m_nResult = RE_SUCCESS;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETRANKYIZHONGQINXI, &msg);
		return;
	}

	unique_id_type mySelfID = pkOwnerPlayer->GetInstance();

	msg.m_nTotalCount = allPlayers.size();
	for(int i=0; i<allPlayers.size(); ++i)
	{
		if(allPlayers[i] == mySelfID)
		{
			msg.m_nMyselfInfo.m_nRankIndex = i;
			UT_ServerHelper::GetPlayerTotalDamageInfo(storyID, allPlayers[i], msg.m_nMyselfInfo);
			break;
		}
	}

	maxIndex = maxIndex > allPlayers.size()-1 ? allPlayers.size()-1 : maxIndex;
	msg.m_nPlayerInfo.reserve(maxIndex-minIndex);
	for(int i=minIndex; i<=maxIndex; ++i)
	{
		PS_SettleInfo playerInfo;
		playerInfo.m_nCharID = allPlayers[i];
		playerInfo.m_nRankIndex = i;
		UT_ServerHelper::GetPlayerTotalDamageInfo(storyID, allPlayers[i], playerInfo);
		msg.m_nPlayerInfo.push_back(playerInfo);
	}

	msg.m_nResult = RE_SUCCESS;
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETRANKYIZHONGQINXI, &msg);
}

void UT_ServerHelper::GetGuildTotalDamageInfo(uint64_t storyID, uint64_t guildID, PS_GuildDamageInfo& data)
{
	mem::vector<uint64_t> params;
	params.push_back(storyID);

	mem::vector<uint64_t> key;
	key.push_back(guildID);

	std::string result;
	UT_ServerHelper::RedisGetData_DamageString(UT_REDIS_KEY_GUILD_DAMAGE_INFO, params, key, result);

	GuildInfo info;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
	{
		return;
	}

	data.m_nGuildID = info.guildID;
	data.m_nDamage = info.totalDamage;
	data.m_nGuildName = info.guildName;
}

void UT_ServerHelper::GetPlayerTotalDamageInfo(uint64_t storyID, uint64_t charID, PS_SettleInfo& data)
{
	mem::vector<uint64_t> params;
	params.push_back(storyID);

	int32_t index = 0;
	uint64_t nScore = 0;
	bool tag = UT_ServerHelper::RedisRank_ReadDamage(UT_REDIS_KEY_DAMAGE_RANK, params, charID, nScore, index);
	if(!tag)
	{
		return;
	}

	mem::vector<uint64_t> key;
	key.push_back(charID);

	std::string result;
	UT_ServerHelper::RedisGetData_DamageString(UT_REDIS_KEY_BATTLE_INFO, params, key, result);

	SettleInfo info;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
	{
		return;
	}

	if(info.buddyIDs.size()!=info.buddyStarLevels.size() || info.equipSlotLevelCount.size()!=info.buddyIDs.size())
	{
		return;
	}

	data.m_nCharName = info.charName;
	data.m_nControllerLevel = info.controllerLevel;
	data.m_nDamageData = nScore;
	data.m_nBornType = info.bornType;
	data.m_nCommanderTitleFrameID = info.commanderTitleFrameID;
	data.m_nCommanderTitle = info.commanderTitle;

	for(int i=0; i<info.buddyIDs.size(); ++i)
	{
		PS_Buddy buddy;
		buddy.m_nBuddyData.m_nBuddyRuneInfo.clear();
		buddy.m_nBuddyData.m_nEquipSlotInfo.clear();

		buddy.m_nBuddyIndex = i<info.buddyIndexs.size() ? info.buddyIndexs[i] : i;
		buddy.m_nDamageData = i<info.buddyDamageDatas.size() ? info.buddyDamageDatas[i] : 0;
		buddy.m_nHurtData = i<info.buddyHurtDatas.size() ? info.buddyHurtDatas[i] : 0;
		buddy.m_nHealData = i<info.buddyHealDatas.size() ? info.buddyHealDatas[i] : 0;

		buddy.m_nBuddyData.m_nBuddyID = info.buddyIDs[i];
		buddy.m_nBuddyData.m_nBuddyLevel = info.buddyLevels[i];
		buddy.m_nBuddyData.m_nBuddyStarLevel = info.buddyStarLevels[i];
		buddy.m_nBuddyData.m_bActivateAllSkill = info.activeAllSkills[i];
		buddy.m_nBuddyData.m_nAllEquipSlotReinforceLevel = info.equipSlotLevelCount[i];

		int buddyID = buddy.m_nBuddyData.m_nBuddyID;
		mem::map<int32_t, mem::map<int32_t, int32_t> >::iterator findIt = info.buddyRuneStarLevelInfo.find(buddyID);
		if(findIt != info.buddyRuneStarLevelInfo.end())
		{
			for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
			{
				PS_RuneItemInfo_BuddyCS data;
				data.m_nRuneStarLevel = it1->second;
				buddy.m_nBuddyData.m_nBuddyRuneInfo.insert(std::make_pair(it1->first, data));
			}
		}

		findIt = info.buddyRuneItemInfo.find(buddyID);
		if(findIt != info.buddyRuneItemInfo.end())
		{
			for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
			{
				std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator kIt = buddy.m_nBuddyData.m_nBuddyRuneInfo.find(it1->first);
				if(kIt == buddy.m_nBuddyData.m_nBuddyRuneInfo.end())
				{
					PS_RuneItemInfo_BuddyCS data;
					data.m_nRuneItemID = it1->second;
					buddy.m_nBuddyData.m_nBuddyRuneInfo.insert(std::make_pair(it1->first, data));
				}
				else
				{
					kIt->second.m_nRuneItemID = it1->second;
				}
			}
		}

		findIt = info.equipSlotStarLevel.find(buddyID);
		if(findIt != info.equipSlotStarLevel.end())
		{
			for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
			{
				PS_EquipItemInfo_BuddyCS data;
				data.m_nEquipSlotStarLevel = it1->second;
				buddy.m_nBuddyData.m_nEquipSlotInfo.insert(std::make_pair(it1->first, data));
			}
		}

		findIt = info.equipItemID.find(buddyID);
		if(findIt != info.equipItemID.end())
		{
			for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
			{
				std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator kIt = buddy.m_nBuddyData.m_nEquipSlotInfo.find(it1->first);
				if(kIt == buddy.m_nBuddyData.m_nEquipSlotInfo.end())
				{
					PS_EquipItemInfo_BuddyCS data;
					data.m_nEquipItemID = it1->second;
					buddy.m_nBuddyData.m_nEquipSlotInfo.insert(std::make_pair(it1->first, data));
				}
				else
				{
					kIt->second.m_nEquipItemID = it1->second;
				}
			}
		}

		data.m_nVecBuddyInfo.push_back(buddy);
	}

	for(int i=0; i<info.bossCharTypeID.size(); ++i)
	{
		PS_Boss bossInfo;
		bossInfo.m_nCharTypeID = info.bossCharTypeID[i];
		bossInfo.m_nDamageData = info.bossDamageDatas[i];
		bossInfo.m_nHealData = info.bossHealDatas[i];
		bossInfo.m_nHurtData = info.bossHurtDatas[i];
		data.m_nVecBossInfo.push_back(bossInfo);
	}
}

int32_t UT_ServerHelper::RandomTestDatabase()
{
	CF_TestDatabase* pkCSVTestDatabase = CF_TestDatabase::GetSingletonPtr();
	int maxCount = pkCSVTestDatabase->GetEntryCount();
	
	if(maxCount < GetGlobalSetting.topicCount)
	{
		GetGlobalSetting.topicCount = maxCount;
	}

	MG_SS_UpdateTestData msg;
	msg.m_TopIDVec.reserve(GetGlobalSetting.topicCount);

	TestData.topic.clear();
	TestData.topic.reserve(GetGlobalSetting.topicCount);
	mem::set<int32_t> topicID;
	while(topicID.size() < GetGlobalSetting.topicCount)
	{
		int id = UT_MathBase::RandInRangeInt(1, maxCount);
		bool tag = topicID.insert(id).second;
		if(tag)
		{
			TestData.topic.push_back(id);
			msg.m_TopIDVec.push_back(id);
		}
	}

	std::stringstream ss;
	boost::ajson::save_to_buff_(TestData, ss);
	std::string data = ss.str();

	mem::vector<uint64_t> key;
	key.push_back(GameStoryIDType_Answer);

	RedisSetData_String(UT_REDIS_KEY_ANSWER_STORY_TOPIC, key, data, false, true);

	//将题目转发给其他节点服务器
	SendMsgToLogin(MGPT_SS_UPDATETESTDATA_LOGIN, &msg);

	return TestData.topic.size();
}

void UT_ServerHelper::GetTestDatabase(std::vector<int32_t>& topicID)
{
	topicID.clear();
	topicID.reserve(TestData.topic.size());
	for(int i=0; i<TestData.topic.size(); ++i)
	{
		topicID.push_back(TestData.topic[i]);
	}
}

void UT_ServerHelper::SettleAnswer(bool init)
{
	//entryServer负责发奖
	if(GetGameServerID() != GetGameServerEntryID())
	{
		return;
	}

	if(init && !m_nSettleAnswer)
	{
		return;
	}

	mem::vector<uint64_t> allPlayers;
	int32_t rankType = RankType_Fail;
	if(init && !initAnswer)
	{
		//服务器初始化时候，加载redis数据到gs
		int32_t endTime = 0;
		mem::vector<uint64_t> input;
		input.push_back(GameStoryIDType_Answer);
		bool tag = UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_ANSWER_STORY_END_TIME, input, endTime);
		if(tag)
		{
			UT_ServerHelper::SetAnswerActivityEndTime(endTime);
			UT_ServerHelper::RedisRank_ReadSimpleRange(UT_REDIS_KEY_ANSWER_ACCOUNT_POINT, allPlayers);
			UT_ServerHelper::SetAnswerPlayer(allPlayers);
			UT_ServerHelper::SetTestDatabase();
			
			UT_ServerHelper::SetCheckAwardTime();

			uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
			UT_ServerHelper::SetLoadTime(currentTime);
		}
		initAnswer = true;
	}
	else if(init && initAnswer)
	{
		//开服的时候，活动已经结束了，邮件发奖
		int32_t loadTime = UT_ServerHelper::GetLoadTime();
		int32_t endTime = UT_ServerHelper::GetAnswerActivityEndTime();
		if(loadTime > endTime)
		{
			UT_ServerHelper::GetAnswerPlayer(allPlayers);
			rankType = RankType_StartGs;//开服的时候，活动已经结束了
		}
		else
		{
			m_nSettleAnswer = false;
		}
	}
	else if(!init)
	{
		UT_ServerHelper::RedisRank_ReadSimpleRange(UT_REDIS_KEY_ANSWER_ACCOUNT_POINT, allPlayers);
		rankType = RankType_End;//正常活动结束时的结算
	}

	ResultType res = RE_FAIL;
	if(rankType > RankType_Fail)
	{
		if(UT_ServerHelper::GetAnswerCloseState())
		{
			m_nSettleAnswer = false;
		}

		if(allPlayers.empty())
		{
			res = RE_SUCCESS;
		}
		int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN7_OPERATE, OPERATE_LOGTYPE2);
		LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
		for(int i=0; i<allPlayers.size(); ++i)
		{
			int rankIndex = i;
			unique_id_type charID = allPlayers[i];
			CF_TestRankReward::DataEntryMapExternIterator kIter = CF_TestRankReward::GetSingletonPtr()->GetIterator();
			for(; kIter.HasMoreElements(); kIter.MoveNext())
			{
				CF_TestRankReward::DataEntry* pkData = kIter.PeekNextValuePtr();
				if(rankIndex>=pkData->_iRankStartIndex && rankIndex<pkData->_iRankEndIndex)
				{
					mem::vector<uint64_t> key;
					key.push_back(charID);

					std::string data;
					UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_ANSWER_PLAYER_INFO, key, data, false);
					AnswerInfo info;
					std::string errorMsg;
					int nValue = 0;
					if(boost::ajson::load_from_buff(info, data.c_str(), errorMsg))
					{
						nValue = info.accountPoint;	

					}
					stringstream ss;
					ss<< nValue << ";";
					ss<< rankIndex+1 << ";";
					StringType mailDec = pkData->_iMailDesc + STDSTR2TPSTR(ss.str());
					res = UT_ServerHelper::SendMailToPlayer(charID, pkData->_iMailTitle, mailDec, pkData->_kReward, false,nLogCode, rankIndex+1);

					if(rankType == RankType_End)
					{
						/*mem::vector<uint64_t> key;
						key.push_back(charID);

						std::string data;
						UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_ANSWER_PLAYER_INFO, key, data, false);

						AnswerInfo info;
						std::string errorMsg;
						if(boost::ajson::load_from_buff(info, data.c_str(), errorMsg))*/
						{
							//通知客户端活动结束                        
							MG_SyncActivityFinish msg;
							msg.m_nStoryID = GameStoryIDType_Answer;
							msg.m_nCharID = charID;
							msg.m_nRankIndex = rankIndex;
							msg.m_nNum = info.accountPoint;	
							SendMsgToChat(MGPT_SYNCACTIVITYFINISH, &msg, 0, charID);
						}
					}

					LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(charID);
					if(NULL != pkPlayer)
					{
						AchievementEvent kEvent;
						kEvent.SetConditionInfo(rankIndex+1);
						kEvent.SetNum(1);
						pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ANSWER_RESULT, kEvent);
					}
					else
					{
						UT_ServerHelper::UpdateOfflineAnswerResult(charID, rankIndex+1);
					}
					break;
				}
			}
		}
		UT_ServerHelper::SendAnswerGuildRankAward();
		UT_ServerHelper::SetAnswerCloseState(1);
	}

	if(RE_SUCCESS == res)
	{
		//发完排名奖励后，清除答题所有数据
		UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_ANSWER_STORY_TOPIC);
		UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_ANSWER_CHECKAWARD_TIME);
		UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_ANSWER_STORY_END_TIME);
		UT_ServerHelper::ClearAnswerPlayer();
		m_nSettleAnswer = false;
	}
}
void UT_ServerHelper::ClearAnswerRedisRank()
{
	UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_ANSWER_ACCOUNT_POINT);
	UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_ANSWER_GUILD_POINT);
	UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_ANSWER_PLAYER_INFO);
	UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_ANSWER_GUILD_INFO);
	UT_ServerHelper::ClearAnswerPlayer();
}
void UT_ServerHelper::SetAnswerCloseState(int nState)
{
	mem::vector<uint64_t> input;
	input.push_back(GameStoryIDType_Answer);
	UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_ANSWER_CLOSE, input, nState);
}

int UT_ServerHelper::GetAnswerCloseState()
{
	mem::vector<uint64_t> input;
	input.push_back(GameStoryIDType_Answer);
	int res = 0;
	UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_ANSWER_CLOSE, input, res);
	return res;
}

void UT_ServerHelper::SetAnswerPlayer(mem::vector<uint64_t>& playerIDs)
{
	for(int i=0; i<playerIDs.size(); ++i)
	{
		AnswerPlayers.push_back(playerIDs[i]);
	}
}

void UT_ServerHelper::GetAnswerPlayer(mem::vector<uint64_t>& playerIDs)
{
	for(int i=0; i<AnswerPlayers.size(); ++i)
	{
		playerIDs.push_back(AnswerPlayers[i]);
	}
}

void UT_ServerHelper::ClearAnswerPlayer()
{
	AnswerPlayers.clear();
}

void UT_ServerHelper::SettleYiJiTuXi(bool init)
{
	//entryServer负责发奖
	if(GetGameServerID() != GetGameServerEntryID())
	{
		return;
	}

	int rankType = RankType_Fail;
	if(false == init)
	{
		//结算
		rankType = RankType_End;
	}
	else if(false == loadYiJiTuXi)
	{
		//服务器启动时，判断是否结算
		int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		int32_t endTime = 0;
		mem::vector<uint64_t> input;
		input.push_back(GameStoryIDType_Monkey);
		bool tag = UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_YIJITUXI_STORY_END_TIME, input, endTime);
		if(0 != endTime && currentTime >= endTime)
		{
			rankType = RankType_StartGs;
		}

		loadYiJiTuXi = true;
	}

	if(rankType == RankType_Fail)
	{
		return;
	}

	//结算
	mem::vector<uint64_t> allPlayers;
	UT_ServerHelper::RedisRank_ReadSimpleRange(UT_REDIS_KEY_MONEYPOINT_YIJITUXI, allPlayers);

	if(allPlayers.empty())
	{
		return;
	}
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN3_OPERATE, OPERATE_LOGTYPE3);
	for(int i=0; i<allPlayers.size(); ++i)
	{
		int rankIndex = i;
		unique_id_type charID = allPlayers[i];
		CF_YiJiTuXiRankRewards::DataEntryMapExternIterator kIter = CF_YiJiTuXiRankRewards::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_YiJiTuXiRankRewards::DataEntry* pkData = kIter.PeekNextValuePtr();
			if(rankIndex>=pkData->_iRankStartIndex && rankIndex<pkData->_iRankEndIndex)
			{
				UT_ServerHelper::SendMailToPlayer(charID, "172;", "173;", pkData->_kReward, false,nLogCode, rankIndex+1);

				if(rankType == RankType_End)
				{
					mem::vector<uint64_t> key;
					key.push_back(charID);

					std::string data;
					UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_MONEYPOINT_YIJITUXI_PLAYER_INFO, key, data, false);

					AnswerInfo info;
					std::string errorMsg;
					if(boost::ajson::load_from_buff(info, data.c_str(), errorMsg))
					{
						//通知客户端活动结束
						MG_SyncActivityFinish msg;
						msg.m_nCharID = charID;
						msg.m_nStoryID = GameStoryIDType_Monkey;
						msg.m_nRankIndex = rankIndex;
						msg.m_nNum = info.accountPoint;
						SendMsgToChat(MGPT_SYNCACTIVITYFINISH, &msg, 0, charID);
					}
				}
				break;
			}
		}
	}

	//发奖后，清排行榜
	UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_MONEYPOINT_YIJITUXI);
	UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_MONEYPOINT_YIJITUXI_PLAYER_INFO);
	UT_ServerHelper::RedisRank_Clear(UT_REDIS_KEY_YIJITUXI_STORY_END_TIME);
}

void UT_ServerHelper::UpdateBattlePassStartTime(uint64_t key, int32_t startTime)
{
	battlePassStartTime[key] = startTime;
}

int32_t UT_ServerHelper::GetBattlePassStartTime(uint64_t key)
{
	int32_t startTime = 0;
	mem::map<uint64_t, int32_t>::iterator it = battlePassStartTime.find(key);
	if(it != battlePassStartTime.end())
	{
		return it->second;
	}
	return startTime;
}

void UT_ServerHelper::UpdateBattlePassEndTime(uint64_t key, int32_t endTime)
{
	battlePassEndTime[key] = endTime;
}

int32_t UT_ServerHelper::GetBattlePassEndTime(uint64_t key)
{
	int32_t endTime = 0;
	mem::map<uint64_t, int32_t>::iterator it = battlePassEndTime.find(key);
	if(it != battlePassEndTime.end())
	{
		return it->second;
	}
	return endTime;
}

int32_t UT_ServerHelper::GetSubSaveTime()
{
	return subSaveTime;
}

void UT_ServerHelper::SetSubSaveTime(int32_t time)
{
	subSaveTime = time;
}

int32_t UT_ServerHelper::GetSubPlusSaveTime()
{
	return subPlusSaveTime;
}

void UT_ServerHelper::SetSubPlusSaveTime(int32_t time)
{
	subPlusSaveTime = time;
}

int32_t UT_ServerHelper::GetSevenDayTrailEndTime()
{
	return sevenDayTrailEndTime;
}

void UT_ServerHelper::SetSevenDayTrailEndTime(int32_t time)
{
	sevenDayTrailEndTime = time;
}

int32_t UT_ServerHelper::GetInvestmentEndTime(int scheduleID)
{
	int32_t endTime = 0;
	mem::map<int32_t, int32_t>::iterator it = investmentEndTime.find(scheduleID);
	if(it != investmentEndTime.end())
	{
		return it->second;
	}
	return endTime;	
}

void UT_ServerHelper::SetInvestmentEndTime(int scheduleID, int time)
{
	investmentEndTime[scheduleID] = time;
}

int32_t UT_ServerHelper::GetYiZhongQinXiGameStoryEndTime(int storyID)
{
	int32_t endTime = -1;
	mem::map<int32_t, int32_t>::iterator it = YiZhongQinXiGameStoryEndTime.find(storyID);
	if(it != YiZhongQinXiGameStoryEndTime.end())
	{
		return it->second;
	}
	return endTime;
}

void UT_ServerHelper::SetYiZhongQinXiGameStoryEndTime(int storyID, int32_t time)
{
	YiZhongQinXiGameStoryEndTime[storyID] = time;
}

bool UT_ServerHelper::GetYiZhongQinXiPlayer(int storyID, mem::vector<uint64_t>& playerIDs)
{
	mem::map<int32_t, mem::vector<uint64_t> >::iterator it = YiZhongQinXiRankPlayers.find(storyID);
	if(it == YiZhongQinXiRankPlayers.end())
	{
		return false;
	}
	
	for(mem::vector<uint64_t>::iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
	{
		playerIDs.push_back(*kIt);
	}
	return true;
}

void UT_ServerHelper::SetYiZhongQinXiPlayer(int storyID, mem::vector<uint64_t>& playerIDs)
{
	mem::vector<uint64_t> players;
	players.reserve(playerIDs.size());
	for(mem::vector<uint64_t>::iterator kIt=playerIDs.begin(); kIt!=playerIDs.end(); ++kIt)
	{
		players.push_back(*kIt);
	}
	YiZhongQinXiRankPlayers[storyID] = players;
}

void UT_ServerHelper::SetYiZhongQinXiGameEndTime(int32_t time)
{
	YiZhongQinXiGameEndTime = time;
}

int32_t UT_ServerHelper::GetYiZhongQinXiGameEndTime()
{
	return YiZhongQinXiGameEndTime;
}

bool UT_ServerHelper::CheckRecordDestActorDamage(LC_ActorBase* pkSrcActor, LC_ActorBase* pkDestActor)
{
	if(NULL==pkSrcActor || NULL==pkDestActor)
	{
		return false;
	}

	int srcType = pkSrcActor->GetMapLogicType();
	int destType = pkDestActor->GetMapLogicType();

	if( destType==srcType && (MT_REANSCRIPTION_RANK==destType || MT_GUILD_MAP2==destType) )
	{
		return true;
	}

	return false;
}

bool UT_ServerHelper::CheckHuanJingVaild()
{
	int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	for(mem::map<int32_t, int32_t>::iterator it=HuanJingEndTime.begin(); it!=HuanJingEndTime.end(); ++it)
	{
		if(it->second >= currentTime)
		{
			return true;
		}
	}

	return false;
}

void UT_ServerHelper::SetHuanJingEndTime(int32_t storyID, int32_t endTime)
{
	mem::map<int32_t, int32_t>::iterator findIt = HuanJingEndTime.find(storyID);
	if(findIt == HuanJingEndTime.end())
	{
		HuanJingEndTime.insert(std::make_pair(storyID, endTime));
	}
	else
	{
		findIt->second = endTime;
	}
}

void UT_ServerHelper::CheckSettleHuanJingDamage()
{
	//entryServer负责发奖
	if(GetGameServerID() != GetGameServerApp()->GetServerEntryId())
	{
		return;
	}

	int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	mem::map<int32_t, int32_t>::iterator it=HuanJingEndTime.begin();
	while(it != HuanJingEndTime.end())
	{
		if(currentTime >= it->second)
		{
			SettleHuanJingDamage(it->first);
			it = HuanJingEndTime.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void UT_ServerHelper::SettleHuanJingDamage(int32_t storyID)
{
	int mapID = 0;
	CF_BuddyThemeBossType::DataEntryMapExternIterator it = CF_BuddyThemeBossType::GetSingletonPtr()->GetIterator();
	for(; it.HasMoreElements(); it.MoveNext())
	{
		CF_BuddyThemeBossType::DataEntry* pkData = it.PeekNextValuePtr();
		if(pkData->_iStoryID == storyID)
		{
			mapID = pkData->_iMapID;
			break;
		}
	}

	if(0 == mapID)
	{
		return;
	}

	mem::vector<uint64_t> params;
	params.push_back(storyID);

	mem::vector<uint64_t> allPlayers;
	UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_DAMAGE_RANK, params, allPlayers);

	ResultType res = RE_FAIL;
	CF_BuddyThemeBossRankReward::DataEntry* pkEntry = NULL;
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN16_OPERATE, OPERATE_LOGTYPE2);
	for(int i=0; i<allPlayers.size(); ++i)
	{
		int rankIndex = i;
		unique_id_type charID = allPlayers[i];
		if(NULL!=pkEntry && i>=pkEntry->_iRankStartIndex && i<pkEntry->_iRankEndIndex)
		{
			std::stringstream ss;
			ss << (i+1) << ";";

			StringType mailDesc = pkEntry->_mailDesc + STDSTR2TPSTR(ss.str());
			res = SendMailToPlayer(charID, pkEntry->_mailTitle, mailDesc, pkEntry->_kReward);
			if(RE_SUCCESS == res)
			{
				mem::vector<uint64_t> nameKey;
				nameKey.push_back(storyID);
				nameKey.push_back(charID);
				RedisClearString(UT_REDIS_KEY_COMBAT_INFO, nameKey);
			}
		}
		else
		{
			CF_BuddyThemeBossRankReward::DataEntryMapExternIterator kIt = CF_BuddyThemeBossRankReward::GetSingletonPtr()->GetIterator();
			for(; kIt.HasMoreElements(); kIt.MoveNext())
			{
				pkEntry = kIt.PeekNextValuePtr();
				if(pkEntry->_iMapID==mapID && i>=pkEntry->_iRankStartIndex && i<pkEntry->_iRankEndIndex)
				{
					std::stringstream ss;
					ss << (i+1) << ";";
					StringType mailDesc = pkEntry->_mailDesc + STDSTR2TPSTR(ss.str());
					res = SendMailToPlayer(charID, pkEntry->_mailTitle, mailDesc, pkEntry->_kReward, false, nLogCode, rankIndex+1);
					if(RE_SUCCESS == res)
					{
						mem::vector<uint64_t> nameKey;
						nameKey.push_back(storyID);
						nameKey.push_back(charID);
						RedisClearString(UT_REDIS_KEY_COMBAT_INFO, nameKey);
					}
				}
			}
		}
	}
}

void UT_ServerHelper::SetAnswerActivityEndTime(int32_t time)
{
	AnswerActivityEndTime = time;
}

int32_t UT_ServerHelper::GetAnswerActivityEndTime()
{
	return AnswerActivityEndTime;
}


void UT_ServerHelper::SetLoadTime(uint32_t loadTime)
{
	LoadAnswerInfoTime = loadTime;
}

uint32_t UT_ServerHelper::GetLoadTime()
{
	return LoadAnswerInfoTime;
}

ResultType UT_ServerHelper::CheckOperGuild()
{
	CF_GameStory::DataEntryMapExternIterator kIter = CF_GameStory::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_GameStory::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(pkData->_DismissGuild)
		{
			bool tag = UT_ServerHelper::CheckStoryIsActive(pkData->_iID);
			if(tag)
			{
				return RE_DISMISS_GUILD_ERROR_2; 
			}
		}
	}

	return RE_SUCCESS;
}

void UT_ServerHelper::UpdateTestDatabase(const std::vector<int32_t>& data)
{
	TestData.topic.clear();
	TestData.topic.reserve(data.size());
	for(int i=0; i<data.size(); ++i)
	{
		TestData.topic.push_back(data[i]);
	}
}

int32_t UT_ServerHelper::SetTestDatabase()
{
	mem::vector<uint64_t> key;
	key.push_back(GameStoryIDType_Answer);

	TestData.topic.clear();
	std::string ss;
	bool tag = RedisGetData_String(UT_REDIS_KEY_ANSWER_STORY_TOPIC, key, ss);
	if(tag)
	{
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(TestData, ss.c_str(), errorMsg))
		{
			return 0;
		}
	}

	return TestData.topic.size();
}

int32_t UT_ServerHelper::GetTopicIDFromTopicIndex(int32_t index)
{
	int size = TestData.topic.size();
	if(index >= size)
	{
		return 0;
	}

	return TestData.topic[index];
}

void UT_ServerHelper::SetCheckAwardTimeToRedis(mem::vector<uint32_t>& time)
{
	CheckAwardTime.time.clear();
	CheckAwardTime.time.reserve(time.size());
	for(int i=0; i<time.size(); ++i)
	{
		CheckAwardTime.time.push_back(time[i]);
	}

	std::stringstream ss;
	boost::ajson::save_to_buff_(CheckAwardTime, ss);
	std::string data = ss.str();

	mem::vector<uint64_t> key;
	key.push_back(GameStoryIDType_Answer);

	RedisSetData_String(UT_REDIS_KEY_ANSWER_CHECKAWARD_TIME, key, data, false, true);
}

bool UT_ServerHelper::SetCheckAwardTime()
{
	mem::vector<uint64_t> key;
	key.push_back(GameStoryIDType_Answer);

	CheckAwardTime.time.clear();
	std::string ss;
	bool tag = RedisGetData_String(UT_REDIS_KEY_ANSWER_CHECKAWARD_TIME, key, ss);
	if(tag)
	{
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(CheckAwardTime, ss.c_str(), errorMsg))
		{
			return false;
		}
	}

	return true;
}

const mem::vector<uint32_t>& UT_ServerHelper::GetCheckTime()
{
	return CheckAwardTime.time;
}

void UT_ServerHelper::GetPlayerDamageData(LC_ServerPlayer* pkOwnerPlayer, int storyID, uint64_t charID)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_YiZhongQinXiDamageData msg;
	msg.m_nStoryID = storyID;
	msg.m_nCharID = charID;
	msg.m_nResult = RE_FAIL;

	do 
	{
		CF_GameStory::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GameStory>(storyID);
		if(NULL==pkData)
		{
			msg.m_nResult = RE_ERROR_PARAM;
			break;
		}

		mem::vector<uint64_t> nameKey;
		nameKey.push_back(storyID);
		nameKey.push_back(charID);

		mem::map<uint64_t, std::string> vecRangeData;
		UT_ServerHelper::RedisReadStringRange(UT_REDIS_KEY_COMBAT_INFO, nameKey, vecRangeData);

		//最多发10个
		int count = 0;
		for(mem::map<uint64_t, std::string>::reverse_iterator it=vecRangeData.rbegin(); it!=vecRangeData.rend()&&count<10; ++it)
		{
			SettleInfo info;
			std::string errorMsg;
			if(!boost::ajson::load_from_buff(info, it->second.c_str(), errorMsg))
			{
				continue;
			}

			PS_YiZhongQinXiDataInfo data;
			data.m_nTime = it->first;
			data.m_nCharName = info.charName;
			data.m_nControllerLevel = info.controllerLevel;
			data.m_nDamageData = info.damageData;
			data.m_nBornType = info.bornType;
			data.m_nCommanderTitleFrameID = info.commanderTitleFrameID;

			msg.m_nDataInfo.push_back(data);
			++count;
		}

		msg.m_nResult = RE_SUCCESS;
	} while (0);

	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_YIZHONGQINGXI_DAMAGEDATA, &msg);
}

void UT_ServerHelper::GetDetailDamageData(LC_ServerPlayer* pkOwnerPlayer, int storyID, uint32_t time, uint64_t charID)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_YiZhongQinXiDetailDamageData msg;
	msg.m_nCharID = charID;
	msg.m_nStoryID = storyID;
	msg.m_nTime = time;
	msg.m_nResult = RE_FAIL;

	do 
	{
		CF_GameStory::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GameStory>(storyID);
		if(NULL==pkData)
		{
			msg.m_nResult = RE_ERROR_PARAM;
			break;
		}

		mem::vector<uint64_t> nameKey;
		nameKey.push_back(storyID);
		nameKey.push_back(charID);

		mem::vector<uint64_t> key;
		key.push_back(time);

		std::string result;
		bool tag = UT_ServerHelper::RedisGetStringData(UT_REDIS_KEY_COMBAT_INFO, nameKey, key, result);
		if(!tag)
		{
			break;
		}

		SettleInfo info;
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
		{
			break;
		}

		msg.m_nBaseInfo.m_nTime = time;
		msg.m_nBaseInfo.m_nCharName = info.charName;
		msg.m_nBaseInfo.m_nControllerLevel = info.controllerLevel;
		msg.m_nBaseInfo.m_nDamageData = info.damageData;
		msg.m_nBaseInfo.m_nBornType = info.bornType;
		msg.m_nBaseInfo.m_nCommanderTitleFrameID = info.commanderTitleFrameID;

		for(int i=0; i<info.buddyIDs.size(); ++i)
		{
			PS_Buddy buddy;
			buddy.m_nBuddyData.m_nBuddyRuneInfo.clear();
			buddy.m_nBuddyData.m_nEquipSlotInfo.clear();

			buddy.m_nBuddyIndex = i<info.buddyIndexs.size() ? info.buddyIndexs[i] : i;
			buddy.m_nDamageData = i<info.buddyDamageDatas.size() ? info.buddyDamageDatas[i] : 0;
			buddy.m_nHurtData = i<info.buddyHurtDatas.size() ? info.buddyHurtDatas[i] : 0;
			buddy.m_nHealData = i<info.buddyHealDatas.size() ? info.buddyHealDatas[i] : 0;

			buddy.m_nBuddyData.m_nBuddyID = info.buddyIDs[i];
			buddy.m_nBuddyData.m_nBuddyLevel = info.buddyLevels[i];
			buddy.m_nBuddyData.m_nBuddyStarLevel = info.buddyStarLevels[i];
			buddy.m_nBuddyData.m_bActivateAllSkill = info.activeAllSkills[i];
			buddy.m_nBuddyData.m_nAllEquipSlotReinforceLevel = info.equipSlotLevelCount[i];

			int buddyID = buddy.m_nBuddyData.m_nBuddyID;
			mem::map<int32_t, mem::map<int32_t, int32_t> >::iterator findIt = info.buddyRuneStarLevelInfo.find(buddyID);
			if(findIt != info.buddyRuneStarLevelInfo.end())
			{
				for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
				{
					PS_RuneItemInfo_BuddyCS data;
					data.m_nRuneStarLevel = it1->second;
					buddy.m_nBuddyData.m_nBuddyRuneInfo.insert(std::make_pair(it1->first, data));
				}
			}

			findIt = info.buddyRuneItemInfo.find(buddyID);
			if(findIt != info.buddyRuneItemInfo.end())
			{
				for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
				{
					std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator kIt = buddy.m_nBuddyData.m_nBuddyRuneInfo.find(it1->first);
					if(kIt == buddy.m_nBuddyData.m_nBuddyRuneInfo.end())
					{
						PS_RuneItemInfo_BuddyCS data;
						data.m_nRuneItemID = it1->second;
						buddy.m_nBuddyData.m_nBuddyRuneInfo.insert(std::make_pair(it1->first, data));
					}
					else
					{
						kIt->second.m_nRuneItemID = it1->second;
					}
				}
			}

			findIt = info.equipSlotStarLevel.find(buddyID);
			if(findIt != info.equipSlotStarLevel.end())
			{
				for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
				{
					PS_EquipItemInfo_BuddyCS data;
					data.m_nEquipSlotStarLevel = it1->second;
					buddy.m_nBuddyData.m_nEquipSlotInfo.insert(std::make_pair(it1->first, data));
				}
			}

			findIt = info.equipItemID.find(buddyID);
			if(findIt != info.equipItemID.end())
			{
				for(mem::map<int32_t, int32_t>::iterator it1=findIt->second.begin(); it1!=findIt->second.end(); ++it1)
				{
					std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator kIt = buddy.m_nBuddyData.m_nEquipSlotInfo.find(it1->first);
					if(kIt == buddy.m_nBuddyData.m_nEquipSlotInfo.end())
					{
						PS_EquipItemInfo_BuddyCS data;
						data.m_nEquipItemID = it1->second;
						buddy.m_nBuddyData.m_nEquipSlotInfo.insert(std::make_pair(it1->first, data));
					}
					else
					{
						kIt->second.m_nEquipItemID = it1->second;
					}
				}
			}
			msg.m_nVecBuddyInfo.push_back(buddy);
		}

		for(int i=0; i<info.bossCharTypeID.size(); ++i)
		{
			PS_Boss bossInfo;
			bossInfo.m_nCharTypeID = info.bossCharTypeID[i];
			bossInfo.m_nDamageData = info.bossDamageDatas[i];
			bossInfo.m_nHealData = info.bossHealDatas[i];
			bossInfo.m_nHurtData = info.bossHurtDatas[i];
			msg.m_nVecBossInfo.push_back(bossInfo);
		}
		msg.m_nResult = RE_SUCCESS;
	} while (0);

	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_YIZHONGQINGXI_DETAILDAMAGEDATA, &msg);
}

bool UT_ServerHelper::CheckChangeServer(GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicType, bool bCertainSameServer)
{
	if(eInOutType != LMIOT_TRANSFER || bCertainSameServer || GetGameServerApp()->IsNodeType_Global())
	{
		return false;
	}

	switch(lTargetMapLogicType)
	{
	case MT_GUILD_MAP:
	case MT_GUILD_MAP2:
		return true;
	}

	return false;
}

void UT_ServerHelper::UpdateLimitRewardRecord(int key, int value)
{
	limitRewardRecord[key] = value;

	MG_SyncACTActiveRewardInfo syncMsg;
	syncMsg.m_nID = key;
	syncMsg.m_nCount = value;
	GetGameServerApp()->BroadCastMsgToClient(MGPT_SYNC_ACT_ACTIVE_REWARD_INFO, &syncMsg);
}

void UT_ServerHelper::GetAllLimitRewardRecord(mem::map<int, int>& record)
{
	for(mem::map<int, int>::iterator it=limitRewardRecord.begin(); it!=limitRewardRecord.end(); ++it)
	{
		record[it->first] = it->second;
	}
}

int UT_ServerHelper::GetLimitRewardRecordByID(int id)
{
	mem::map<int, int>::iterator findIt = limitRewardRecord.find(id);
	if(findIt != limitRewardRecord.end())
	{
		return findIt->second;
	}
	return 0;
}

void UT_ServerHelper::UpdateLimitRewardRecordB(int key, int value)
{
	limitRewardRecordB[key] = value;

	MG_SyncACTActiveRewardInfoB syncMsg;
	syncMsg.m_nID = key;
	syncMsg.m_nCount = value;
	GetGameServerApp()->BroadCastMsgToClient(MGPT_SYNC_ACT_ACTIVE_REWARD_INFO_B, &syncMsg);
}

int UT_ServerHelper::GetLimitRewardRecordBByID(int id)
{
	mem::map<int, int>::iterator findIt = limitRewardRecordB.find(id);
	if(findIt != limitRewardRecordB.end())
	{
		return findIt->second;
	}
	return 0;
}

void UT_ServerHelper::RemoveLimitActivityB(int id)
{
	limitActivityIDB.erase(id);
}

void UT_ServerHelper::AddLimitActivityIDB(int id)
{
	limitActivityIDB.insert(id);
}

void UT_ServerHelper::CreateLimitActivityB()
{
	for(mem::set<int>::iterator it=limitActivityIDB.begin(); it!=limitActivityIDB.end(); ++it)
	{
		MG_REQ_CreateLimitReward_B reqMsg;
		reqMsg.m_nEntryServerID = GetGameServerEntryID();
		reqMsg.m_nKey = *it;
		SendMsgToSociety(MGPT_GGU_REQ_CREATELIMITREWARD_B, &reqMsg);
	}
}

void UT_ServerHelper::RemoveLimitActivity(int id)
{
	limitActivityID.erase(id);
}

void UT_ServerHelper::CreateLimitActivity()
{
	for(mem::set<int>::iterator it=limitActivityID.begin(); it!=limitActivityID.end(); ++it)
	{
		MG_REQ_CreateLimitReward reqMsg;
		reqMsg.m_nEntryServerID = GetGameServerEntryID();
		reqMsg.m_nKey = *it;
		SendMsgToSociety(MGPT_GGU_REQ_CREATELIMITREWARD, &reqMsg);
	}
}

void UT_ServerHelper::AddLimitActivityID(int id)
{
	limitActivityID.insert(id);
}

void UT_ServerHelper::SetSubBEndTime(int groupID, uint32_t endTime)
{
	subBEndTime[groupID] = endTime;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SetSubBEndTime groupID:%d, endTime:%d\n", groupID, endTime);
}

void UT_ServerHelper::GetSubBEndTime(mem::map<int, uint32_t>& time)
{
	for(mem::map<int, uint32_t>::const_iterator it=subBEndTime.begin(); it!=subBEndTime.end(); ++it)
	{
		time[it->first] = it->second;
	}
}

const mem::map<int, uint32_t> & UT_ServerHelper::GetSubBEndTimeRef()
{
	return subBEndTime;
}

void UT_ServerHelper::ClearSubBEndTime(int groupID)
{
	mem::map<int, uint32_t>::iterator findIt=subBEndTime.find(groupID);
	if(findIt != subBEndTime.end())
	{
		subBEndTime.erase(groupID);
	}

	char str1[100] = ":sub_b_info_endtime:";
	const char* szNameSpace1 = UT_ServerHelper::GetRedisNameSpace(str1, groupID);
	UT_ServerHelper::RedisRank_Clear(szNameSpace1);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ClearSubBEndTime groupID:%d, serverid:%d\n", groupID, GetGameServerID());
}

bool UT_ServerHelper::GetSubBData(LC_ServerPlayer* pkPlayer, int type, const Utility::UT_SIMDataList& paramList, int param, uint32_t& value, uint32_t& result)
{
	if(NULL == pkPlayer)
	{
		return false;
	}

	CF_RankCondition::DataEntry* pkData = UT_ServerHelper::GetRankConditionPtrByType(type);
	if(NULL == pkData)
	{
		return false;
	}

	switch(type)
	{
	case Sub_Reward_Type_Vehicle:
		{
			result = (uint32_t)(pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_VEHICLE));//坐骑战斗力
		}
		break;
	case Sub_Reward_Type_Wing:
		{
			result = (uint32_t)(pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_MUHUN));//背饰战斗力
		}
		break;
	case Sub_Reward_Type_Devaeye:
		{
			result = (uint32_t)(pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_DEVA_EYE));//萌宠战斗力
		}
		break;
	case Sub_Reward_Type_BuddyEquipSlotCombatScore:
		{
			result = (uint32_t)(pkPlayer->GetCombatEquipSlotCombatScoreByBuddys(paramList, param));
		}
		break;
	case Sub_Reward_Type_BuddyStarLevel:
		{
			result = pkPlayer->GetStarLevelByBuddys(paramList, param);
		}
		break;
	case Sub_Reward_Type_RuneItemCombatScore:
		{
			result = (uint32_t)(pkPlayer->GetRuneItemCombatScoreByItemID(paramList, param));
		}
		break;
	case Sub_Reward_Type_AllRuneItemCombatScore:
		{
			result = (uint32_t)(pkPlayer->GetAllRuneItemCombatScore());
		}
		break;
	case Sub_Reward_Type_CommanderLevel:
		{
			result = pkPlayer->GetLevel();
		}
		break;
	case Sub_Reward_Type_CommanderTitle:
		{
			result = pkPlayer->GetCommanderTitle();
		}
		break;
	case Sub_Reward_Type_ControllerEquipCombatScore:
		{
			result = (uint32_t)(pkPlayer->GetCommanderAttrMgr().GetCommanderCombatScoreByType(CST_ControllerEquip));
		}
		break;
	default:
		return false;
	}

	value = result>=pkData->_iMinCondition ? result : 0;

	return true;
}

void UT_ServerHelper::DoUpdateRankInfoToRedis(LC_ServerPlayer* pkOwnerPlayer, int type, int param, uint32_t newValue)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController() || newValue==0)
	{ 
		return;
	}

	std::string valueSpace;
	UT_ServerHelper::GetRankValueNameSpace(type, param, valueSpace);
	const char* szNameSpace1 = valueSpace.c_str();

	std::string playerInfoSpace;
	UT_ServerHelper::GetRankPlayerInfoNameSpace(type, param, playerInfoSpace);
	const char* szNameSpace2 = playerInfoSpace.c_str();

	bool tag = false;
	
	uint64_t nScore = 0;
	int32_t index = 0;
	bool res = UT_ServerHelper::RedisRank_ReadElement(szNameSpace1, pkOwnerPlayer->GetInstance(), nScore, index);
	if(!res)
	{
		tag = true;
	}
	else
	{
		uint32_t oldValue = 0;
		uint32_t time = 0;
		LC_Helper::DecodeScore(nScore, oldValue, time);
		if(newValue > oldValue)
		{
			tag = true;
		}
	}

	if(tag)
	{
		//更新redis中的数据
		uint32_t currentTime  = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		uint32_t inputTime = LC_Helper::TimeStampReverse(currentTime);
		uint64_t result = LC_Helper::EncodeScore(newValue, inputTime);
		UT_ServerHelper::RedisRank_WriteElement(szNameSpace1, pkOwnerPlayer->GetInstance(), result);

		SubBInfo info;
		info.charName = pkOwnerPlayer->GetOwnerCharName().c_str();
		info.value = newValue;
		info.activeBuddyTransformID = pkOwnerPlayer->GetCurrentActivateBuddyTransformID();
		info.charID = pkOwnerPlayer->GetInstance();

		LC_ServerPlayer* pkOperPlayer = pkOwnerPlayer->GetActivateBuddyByIndex(pkOwnerPlayer->GetCurrentActivateBuddyIndex());
		IF_A_NA_PLAYER(pkOperPlayer) 
		{
			BuddyInstance* pkBuddy = pkOwnerPlayer->GetAssignedBuddyInstance();
			if(NULL != pkBuddy)
			{
				info.cid = pkBuddy->GetTemplateID();
			}
		}
		else
		{
			BuddyInstance* pkBuddy = pkOperPlayer->GetAssignedBuddyInstance();
			if(NULL != pkBuddy)
			{
				info.cid = pkOperPlayer->GetAssignedBuddyInstance()->GetTemplateID();
			}
		}

		std::stringstream ss;
		boost::ajson::save_to_buff(info, ss);
		std::string data = ss.str().c_str();

		mem::vector<uint64_t> key;
		key.push_back(pkOwnerPlayer->GetInstance());

		UT_ServerHelper::RedisSetData_String(szNameSpace2, key, data, false, true);
	}
}

void UT_ServerHelper::GetRankValueNameSpace(int type, int key, std::string& space)
{
	mem::vector<uint64_t> param;
	param.reserve(2);
	param.push_back(type);
	param.push_back(key);

	space = ":rank_value_info:" + GenDetailKeyWithoutData(param);
}

void UT_ServerHelper::GetRankPlayerInfoNameSpace(int type, int key, std::string& space)
{
	mem::vector<uint64_t> param;
	param.reserve(2);
	param.push_back(type);
	param.push_back(key);

	space = ":rank_player_info:" + GenDetailKeyWithoutData(param);
}

void UT_ServerHelper::GetRankClearNameSpace(int type, int key, std::string& space)
{
	mem::vector<uint64_t> param;
	param.reserve(2);
	param.push_back(type);
	param.push_back(key);

	space = ":rank_clear_info:" + GenDetailKeyWithoutData(param);
}

bool UT_ServerHelper::GetPlayerRankRangeFromRedis(int type, int key, mem::vector<uint64_t>& playerIDVec)
{
	std::string valueSpace;
	UT_ServerHelper::GetRankValueNameSpace(type, key, valueSpace);
	const char* szNameSpace1 = valueSpace.c_str();

	UT_ServerHelper::RedisRank_ReadSimpleRange(szNameSpace1, playerIDVec);

	mem::vector<uint64_t> player;
	UT_ServerHelper::RedisRank_ReadSimpleRange(szNameSpace1, player, true);
	if(player.empty())
	{
		//更新快照
		UT_ServerHelper::RedisRank_CreateSnapshot(szNameSpace1);
		return false;
	}

	return true;
}

void UT_ServerHelper::DoSendRankRewardToPlayer(int type, int key, const mem::vector<uint64_t>& playerIDVec, int xlsxType)
{
	if(playerIDVec.empty())
	{
		return;
	}

	CF_RankCondition::DataEntry* pkData = UT_ServerHelper::GetRankConditionPtrByType(type);
	if(NULL == pkData)
	{
		return;
	}

	std::string playerInfoSpace;
	UT_ServerHelper::GetRankPlayerInfoNameSpace(type, key, playerInfoSpace);
	const char* szNameSpace2 = playerInfoSpace.c_str();

	//topN的玩家的上榜成绩
	struct playerInfo
	{
		uint64_t charid;
		int value;
	};

	mem::vector<playerInfo> topNPlayerInfo;
	int topN = pkData->_ConditionInfo.size();		
	for(int i=0; i<topN && i<playerIDVec.size() ; ++i)
	{
		uint64_t charID = playerIDVec[i];
		mem::vector<uint64_t> key;
		key.push_back(charID);

		std::string result;
		UT_ServerHelper::RedisGetData_String(szNameSpace2, key, result);
		SubBInfo info;
		std::string errorMsg;
		if(boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
		{
			playerInfo data;
			data.charid = charID;
			data.value = info.value;
			topNPlayerInfo.push_back(data);
		}
	}

	//topN的玩家的榜上排名
	mem::map<uint64_t, int32_t> rewardPlayer;
	for(int i=0; i< pkData->_ConditionInfo.size(); ++i)
	{
		int value =  pkData->_ConditionInfo[i];
		for(mem::vector<playerInfo>::iterator it=topNPlayerInfo.begin(); it!=topNPlayerInfo.end(); ++it)
		{
			if(it->value >= value)
			{
				rewardPlayer.insert(std::make_pair(it->charid, i));
				topNPlayerInfo.erase(it);
				break;
			}
		}
	}

	int startIndex = rewardPlayer.size();
	for(int i=startIndex; i<playerIDVec.size(); ++i, ++topN)
	{
		uint64_t charID = playerIDVec[i];
		rewardPlayer.insert(std::make_pair(charID, topN));
	}

	int nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE2);
	for(mem::map<uint64_t, int32_t>::iterator it=rewardPlayer.begin(); it!=rewardPlayer.end(); ++it)
	{
		int rankIndex = it->second;
		unique_id_type charID = it->first;

		switch(xlsxType)
		{
		case RewardXlsx_Type_ShotActivityLimitRankRewards:
			{
				CF_ShotActivityLimitRankRewards::DataEntryMapExternIterator kIter = CF_ShotActivityLimitRankRewards::GetSingletonPtr()->GetIterator();
				for(; kIter.HasMoreElements(); kIter.MoveNext())
				{
					CF_ShotActivityLimitRankRewards::DataEntry* pkData = kIter.PeekNextValuePtr();
					if(pkData->_GroupID==key && rankIndex>=pkData->_iRankStartIndex && rankIndex<pkData->_iRankEndIndex)
					{
						stringstream index;
						index << (rankIndex+1) << ";";
						StringType mailDesc = pkData->_sMailDesc + STDSTR2TPSTR(index.str());

						int nExtraParam = key * 10000 + rankIndex+1;
						UT_ServerHelper::SendMailToPlayer(charID, pkData->_sMailTitle, mailDesc, pkData->_kReward, false, nLogCode, nExtraParam);

						break;
					}
				}
			}
			break;
		case RewardXlsx_Type_2:
			{
				CActivityRankRewardMgr* pkMgr = CActivityRankRewardMgr::GetSingletonPtr();

				StringType mailTitle;
				StringType mailDesc;
				StringType reward = pkMgr->GetRankRewardItem(type, key, rankIndex, mailTitle, mailDesc);

				stringstream index;
				index << (rankIndex+1) << ";";
				int nExtraParam = key * 10000 + rankIndex + 1;
				UT_ServerHelper::SendMailToPlayer(charID, mailTitle, mailDesc + STDSTR2TPSTR(index.str()), reward, false, nLogCode, nExtraParam);
			}
			break;
		default:
			break;
		}
	}
}

void UT_ServerHelper::SettleSubB()
{
	if(!initSubB)
	{
		mem::vector<uint64_t> vecRangeData;
		RedisList_GetRangeInt(UT_REDIS_KEY_SUBB_RANK_RECORD, vecRangeData);
		for(int i=0; i<vecRangeData.size(); ++i)
		{
			rankGroupID.insert(vecRangeData[i]);
		}

		const mem::map<int, CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs>& SubBGroupDataMap = CActSubordinateRewardManager::GetSingletonPtr()->GetActSubordinateRewardEntrysGroupDataMap();
		for(mem::map<int, CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs>::const_iterator it=SubBGroupDataMap.begin(); it!=SubBGroupDataMap.end(); ++it)
		{
			int groupID = it->first;

			const CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs& dateEntryPtr = it->second;
			for(CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs::const_iterator it=dateEntryPtr.begin(); it!=dateEntryPtr.end(); ++it)
			{
				const CF_ACTSubordinateRewardB::DataEntry* pkEntry = *it;
				if(NULL != pkEntry)
				{
					mem::map<int, uint32_t>::iterator findIt = subBEndTime.find(groupID);
					if(findIt != subBEndTime.end())
					{
						continue;
					}

					mem::vector<uint64_t> input;
					input.push_back(pkEntry->_iSchedule);
					input.push_back(pkEntry->_iType);

					int32_t endTime = 0;
					char str1[100] = ":sub_b_info_endtime:";
					const char* szNameSpace1 = UT_ServerHelper::GetRedisNameSpace(str1, groupID);
					bool tag = UT_ServerHelper::RedisGetData_Int(szNameSpace1, input, endTime);
					if(tag)
					{
						SetSubBEndTime(groupID, endTime);
					}

					input.clear();
					input.push_back(pkEntry->_iFinishSchedule);
					input.push_back(pkEntry->_iType);

					int32_t clearTime = 0;
					char str2[100] = ":sub_b_info_cleartime:";
					const char* szNameSpace2 = UT_ServerHelper::GetRedisNameSpace(str2, pkEntry->_GroupID);
					tag = UT_ServerHelper::RedisGetData_Int(szNameSpace2, input, clearTime);
					if(tag)
					{
						SetSubBClearTime(groupID, clearTime);
					}

					break;
				}
			}
		}
		initSubB = true;
	}
	
	CheckRankSubB();
	CheckClearSubB();
}

void UT_ServerHelper::SetSubBClearTime(int groupID, uint32_t time)
{
	subBClearTime[groupID] = time;
}

void UT_ServerHelper::CheckClearSubB()
{
	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	mem::map<int, uint32_t>::iterator it=subBClearTime.begin();
	while(it!=subBClearTime.end())
	{
		int groupID = it->first;
		uint32_t time = it->second;
		if(currentTime >= time && 0!=time)
		{
			const CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs* entryPtrs = CActSubordinateRewardManager::GetSingletonPtr()->GetActSubordinateRewardEntrysByGroupID(groupID);
			if(NULL == entryPtrs)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CheckClearSub_1 not find group! group:%d\n", groupID);
				++it;
				continue;
			}

			CF_ACTSubordinateRewardB::DataEntry* pkEntry = *entryPtrs->begin();
			if(NULL == pkEntry)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CheckClearSub_2 pkEntry is nullptr! group:%d\n", groupID);
				++it;
				continue;
			}

			it = subBClearTime.erase(it);

			std::string valueSpace;
			UT_ServerHelper::GetRankValueNameSpace(pkEntry->_iType, pkEntry->_GroupID, valueSpace);
			const char* szNameSpace1 = valueSpace.c_str();

			std::string clearSpace;
			UT_ServerHelper::GetRankClearNameSpace(pkEntry->_iType, pkEntry->_GroupID, clearSpace);
			const char* szNameSpace3 = clearSpace.c_str();

			UT_ServerHelper::RedisRank_Clear(szNameSpace1);
			UT_ServerHelper::RedisRank_Clear(szNameSpace3);
		}
		else
		{
			++it;
		}
	}
}

void UT_ServerHelper::CheckRankSubB()
{
	//entryServer负责结算
	if(GetGameServerID() != GetGameServerEntryID())
	{
		return;
	}

	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	for(mem::map<int, uint32_t>::iterator it=subBEndTime.begin(); it!=subBEndTime.end(); ++it)
	{
		int groupID = it->first;
		if(0!=it->second && currentTime >= it->second && CanRank(groupID))
		{
			//结算排行榜
			UT_ServerHelper::RankSubB(groupID);
			UpdateRankRecord(groupID);
		}
	}
}

bool UT_ServerHelper::CanRank(int groupID)
{
	return rankGroupID.find(groupID) == rankGroupID.end();
}

void UT_ServerHelper::UpdateRankRecord(int groupID)
{
	rankGroupID.insert(groupID);

	UT_ServerHelper::RedisList_AddInt(UT_REDIS_KEY_SUBB_RANK_RECORD, groupID);
}

CF_RankCondition::DataEntry* UT_ServerHelper::GetRankConditionPtrByType(int32_t type)
{
	CF_RankCondition::DataEntryMapExternIterator kIter = CF_RankCondition::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_RankCondition::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(type == pkData->_iType)
		{
			return pkData;
		}
	}
	return NULL;
}

ResultType UT_ServerHelper::GetSubBRankInfo(LC_ServerPlayer* pkOwnerPlayer, bool active, int type, const char* szNameSpace1, const char* szNameSpace2, int minIndex, int maxIndex, MG_RLT_GetRankSubBInfo& msg)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController() || NULL==szNameSpace1 || NULL==szNameSpace1 || minIndex>maxIndex)
	{
		return RE_FAIL;
	}

	unique_id_type mySelfID = pkOwnerPlayer->GetInstance();
	mem::vector<uint64_t> allPlayers;
	if(active)
	{
		UT_ServerHelper::RedisRank_ReadSimpleRange(szNameSpace1, allPlayers);
	}
	else
	{
		UT_ServerHelper::RedisRank_ReadSimpleRange(szNameSpace1, allPlayers, true);
	}

	if(allPlayers.size() <= 0)
	{
		msg.m_nTotalCount = 0;
		msg.m_nMyselfInfo.m_nRankIndex = SPECIES_RANK_NONE;
		return RE_SUCCESS;
	}

	CF_RankCondition::DataEntry* pkData = UT_ServerHelper::GetRankConditionPtrByType(type);
	if(NULL == pkData)
	{
		return RE_FAIL;
	}

	//topN的玩家的上榜成绩
	struct playerInfo
	{
		uint64_t charid;
		uint64_t value;
	};

	mem::vector<playerInfo> topNPlayerInfo;
	int topN = pkData->_ConditionInfo.size();		
	for(int i=0; i<topN && i<allPlayers.size() ; ++i)
	{
		uint64_t charID = allPlayers[i];
		mem::vector<uint64_t> key;
		key.push_back(charID);

		std::string result;
		UT_ServerHelper::RedisGetData_String(szNameSpace2, key, result);
		SubBInfo info;
		std::string errorMsg;
		if(boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
		{
			playerInfo data;
			data.charid = charID;
			data.value = info.value;
			topNPlayerInfo.push_back(data);
		}
	}

	//topN的玩家的榜上排名
	mem::vector<playerInfo> topNPlayerIndex;
	for(int i=0; i<pkData->_ConditionInfo.size(); ++i)
	{
		int value = pkData->_ConditionInfo[i];
		for(mem::vector<playerInfo>::iterator it=topNPlayerInfo.begin(); it!=topNPlayerInfo.end(); ++it)
		{
			if(it->value >= value)
			{
				playerInfo data;
				data.charid = it->charid;
				data.value = i;

				topNPlayerIndex.push_back(data);
				topNPlayerInfo.erase(it);
				break;
			}
		}
	}

	//topNPlayerIndex中的second是名次，first是playerid
	for(mem::vector<playerInfo>::iterator it=topNPlayerIndex.begin(); it!=topNPlayerIndex.end(); ++it)
	{
		uint64_t charID = it->charid;
		mem::vector<uint64_t> key;
		key.push_back(charID);

		std::string result;
		UT_ServerHelper::RedisGetData_String(szNameSpace2, key, result);

		SubBInfo info;
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
		{
			continue;
		}

		//第一名
		if(it->value == 0)
		{
			msg.m_nFirstCID = info.activeBuddyTransformID;
			msg.m_nFirstCharID = info.charID;
		}

		//topN玩家的数据
		PS_SubBInfo playerInfo;
		playerInfo.m_nCharID = charID;
		playerInfo.m_nRankIndex = it->value;
		playerInfo.m_nCharName = info.charName;
		playerInfo.m_nValue = info.value;

		msg.m_nPlayerInfo.push_back(playerInfo);
	}

	//自己排名和数据
	bool flag = false;
	bool tag = false;
	msg.m_nTotalCount = allPlayers.size();
	for(int i=0; i<allPlayers.size(); ++i)
	{
		if(allPlayers[i] == mySelfID)
		{
			flag = true;

			mem::vector<uint64_t> key;
			key.push_back(mySelfID);

			std::string result;
			UT_ServerHelper::RedisGetData_String(szNameSpace2, key, result);

			SubBInfo info;
			std::string errorMsg;
			if(!boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
			{
				break;
			}

			msg.m_nMyselfInfo.m_nCharID = mySelfID;
			msg.m_nMyselfInfo.m_nCharName = info.charName;				
			msg.m_nMyselfInfo.m_nValue = info.value;
			msg.m_nMyselfInfo.m_nRankIndex = i;

			for(mem::vector<playerInfo>::iterator it=topNPlayerIndex.begin(); it!=topNPlayerIndex.end(); ++it)
			{
				if(mySelfID == it->charid)
				{
					tag = true;
					msg.m_nMyselfInfo.m_nRankIndex = it->value;
				}
			}
			break;
		}
	}

	if(!flag)
	{
		msg.m_nMyselfInfo.m_nRankIndex = SPECIES_RANK_NONE;
	}

	minIndex = topNPlayerIndex.size(); 
	if(minIndex < allPlayers.size())
	{
		maxIndex = maxIndex > allPlayers.size()-1 ? allPlayers.size()-1 : maxIndex;
		for(int i=minIndex; i<=maxIndex; ++i, ++topN)
		{
			uint64_t charID = allPlayers[i];
			mem::vector<uint64_t> key;
			key.push_back(charID);

			std::string result;
			UT_ServerHelper::RedisGetData_String(szNameSpace2, key, result);

			SubBInfo info;
			std::string errorMsg;
			if(!boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
			{
				continue;
			}

			PS_SubBInfo playerInfo;
			playerInfo.m_nCharID = charID;
			playerInfo.m_nRankIndex = topN;
			playerInfo.m_nCharName = info.charName;
			playerInfo.m_nValue = info.value;

			if(false==tag && charID==mySelfID)
			{
				msg.m_nMyselfInfo.m_nRankIndex = playerInfo.m_nRankIndex;
				tag = true;
			}

			msg.m_nPlayerInfo.push_back(playerInfo);
		}
	}

	return RE_SUCCESS;
}

void UT_ServerHelper::InitBestGuildInfo()
{
	if(!initGuildStoryID)
	{
		int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		mem::vector<uint64_t> input;
		input.push_back(GetGlobalSetting.BestGuildScheduleID);
		int res = -1;
		UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_BESTGUILD_ENDTIME, input, res);
		if(res<0 || currentTime<res)
		{
			res = 0;
		}
		UT_ServerHelper::SetBestGuildEndTime(res);

		for(int i=0; i<bestGuildStoryID.size(); ++i)
		{
			mem::vector<uint64_t> input;
			input.push_back(bestGuildStoryID[i]);
			int32_t time = -1;
			UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_BESTGUILD_ACTIVITY_ENDTIME, input, time);
			if(time<0 || currentTime<time)
			{
				time = 0;
			}
			UT_ServerHelper::UpdateBestGuildActivityEndTime(bestGuildStoryID[i], time);
		}
	}

	initGuildStoryID = true;
}

bool UT_ServerHelper::IsBestGuildStoryID(int storyID)
{
	for(int i=0; i<bestGuildStoryID.size(); ++i)
	{
		if(bestGuildStoryID[i] == storyID)
		{
			return true;
		}
	}
	return false;
}

/*
bool UT_ServerHelper::IsBestGuildTaskScheduleID(int scheduleID)
{
	for(int i=0; i<bestGuildTaskScheduleID.size(); ++i)
	{
		if(scheduleID == bestGuildTaskScheduleID[i])
		{
			return true;
		}
	}

	return false;
}
*/

void UT_ServerHelper::SetBestGuildEndTime(int32_t endTime)
{
	bestGuildEndTime = endTime;
}

int32_t UT_ServerHelper::GetBestGuildEndTime()
{
	return bestGuildEndTime;
}

void UT_ServerHelper::SettleBestGuildReward()
{
	int endTime = GetBestGuildEndTime();
	if(0 == endTime)
	{
		return;
	}

	bool send = gsAppSendMail1;
	bool tag = false;
	bool flag = false;
	int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(send && currentTime >= endTime)
	{
		
	}

	if(tag && flag)
	{
		SetBestGuildEndTime(0);
		//UT_ServerHelper::ClearGuildFightRedisInfo();
	}
}

void UT_ServerHelper::GameServerAppToSendBestGuildReward1(bool tag)
{
	gsAppSendMail1 = tag;
}

void UT_ServerHelper::GameServerAppToSendBestGuildReward2(bool tag)
{
	gsAppSendMail2 = tag;
}

void UT_ServerHelper::UpdateBestGuildActivityEndTime(int storyID, int32_t endTime)
{
	bestGuildActivityEndTime[storyID] = endTime;
}

void UT_ServerHelper::SettleBestGuildActivityReward()
{
	bool send = gsAppSendMail2;
	if(!send)
	{
		return;
	}

	int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	for(mem::map<int, int32_t>::iterator it=bestGuildActivityEndTime.begin(); it!=bestGuildActivityEndTime.end(); ++it)
	{
		if(0!=it->second && currentTime >= it->second)
		{			
			mem::vector<uint64_t> input;
			input.push_back(it->first);
			UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_BESTGUILD_ACTIVITY_ENDTIME, input, 0);
			it->second = 0;
		}
	}
}

bool UT_ServerHelper::IsShengYuDiGongStoryID(int storyID)
{
	switch(storyID)
	{
	case GameStoryIDType_digong1:
	case GameStoryIDType_digong2:
	case GameStoryIDType_digong3:
	case GameStoryIDType_digong4:
	case GameStoryIDType_digong5:
	case GameStoryIDType_digong6:
	case GameStoryIDType_digong7:
	case GameStoryIDType_digong8:
	case GameStoryIDType_digong9:
		return true;
	}
	return false;
}

void UT_ServerHelper::SendGuildPresentExpInfoToSociety()
{
	if(false == guildPresentExpInfoFlag && GetGameServerEntryID()==GetGameServerID())
	{
		//std::stringstream logStr;
		//logStr << "SendGuildPresentExpInfoToSociety entryID:[" << GetGameServerEntryID() << "]";
		MG_GG_LoadGuildCrystal msg;
		msg.m_nEntryNodeID = GetGameServerEntryID();
		msg.m_nMaxLevel1 = -1;

		CF_GuildGift::DataEntryMapExternIterator kIt = CF_GuildGift::GetSingletonPtr()->GetIterator();
		for(; kIt.HasMoreElements(); kIt.MoveNext())
		{
			CF_GuildGift::DataEntry* pkData = kIt.PeekNextValuePtr();
			CF_GuildCrystal::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_GuildCrystal>(pkData->_iCrystalQuality);
			if(NULL == pkEntry)
			{
				continue;
			}

			if(pkData->_iLevel > msg.m_nMaxLevel1)
			{
				msg.m_nMaxLevel1 = pkData->_iLevel;
			}

			//logStr << " level1:["<< pkData->_iLevel << "]";
			PS_GuildCrystalData data;
			data.m_nExpLevel1 = pkData->_iLevel;
			data.m_nNeedExp1Num = pkData->_iExp;
			data.m_nExpLevel2 = pkData->_iCrystalQuality;		
			data.m_nNeedExp2Num = pkEntry->_iExp;
			data.m_nRewardInfo = TPSTR2STDSTR(pkEntry->_kReward);
			data.m_nVaildTime = pkEntry->_VaildTime;

			msg.m_nGuildCrystalInfo.push_back(data);
		}

		guildPresentExpInfoFlag = true;

		if(!msg.m_nGuildCrystalInfo.empty())
		{
			SendMsgToSociety(MGPT_GG_LOADGUILDCRYSTAL, &msg);
		}
	}
}


void UT_ServerHelper::TryExportRobotToGroup()
{
	if(exportExportFlag.size() != exportRobotInfo.size())
	{
		MG_GG_REQ_GetExportRobotFlag msg;
		for(mem::map<int32_t, mem::vector<uint32_t> >::iterator it=exportRobotInfo.begin(); it!=exportRobotInfo.end(); ++it)
		{
			int robotType = it->first;
			mem::map<int32_t, int32_t>::iterator findIt = exportExportFlag.find(robotType);
			if(findIt == exportExportFlag.end())
			{
				msg.m_nRobotType = robotType;
				SendMsgToGroup(MGPT_SS_REQ_GET_EXPORTROBOTFLAG, &msg);

				exportExportFlag[robotType] = false;
			}
		}
	}
}

void UT_ServerHelper::SetExportRobotFlag(int32_t robotType, int32_t flag)
{
	exportExportFlag[robotType] = flag;
	if(flag == true)
	{
		ExportRobot* pkExportRobot = (ExportRobot*)ExportRobot::GetSingletonPtr();
		pkExportRobot->InitExportRobotToGroup(ERT_INIT, robotType);
	}
}

void UT_ServerHelper::InitExportRobot()
{
	mem::map<int32_t, mem::vector<uint32_t> >::iterator findIt = exportRobotInfo.begin();
	CF_ExportSoloDuleRobotInfo::DataEntryMapExternIterator kIter= CF_ExportSoloDuleRobotInfo::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_ExportSoloDuleRobotInfo::DataEntry* pkData = kIter.PeekNextValuePtr();
		findIt = exportRobotInfo.find(pkData->_robotType);
		if(findIt == exportRobotInfo.end())
		{
			mem::vector<uint32_t> robotIDVec;
			robotIDVec.reserve(2000);
			robotIDVec.push_back(pkData->_id);
			exportRobotInfo.insert(std::make_pair(pkData->_robotType, robotIDVec));
		}
		else
		{
			findIt->second.push_back(pkData->_id);
		}
	}
}

void UT_ServerHelper::GetRobotIDVecByRobotType(int32_t robotType, mem::vector<uint32_t>& idVec)
{
	idVec.clear();
	mem::map<int32_t, mem::vector<uint32_t> >::iterator findIt = exportRobotInfo.find(robotType);
	if(findIt != exportRobotInfo.end())
	{
		idVec.reserve(findIt->second.size());
		idVec = findIt->second;
	}
}

void UT_ServerHelper::InitCSV()
{
	if(!initCSV)
	{
		gameStartTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

		InitBestGuild();
		InitRegularBoss();
		InitEscortReward();
		InitBestGuildGoal();
		InitRuneStar();
		InitTreasurePos();
		InitBuddy();
		InitGuildMapBossHP();
		InitExportRobot();

		SC_ServerScriptInterface::GetSingletonPtr()->GetYaZhiManager()->Load_CombatYaZhi();
		InitSpecialTask();

		LC_Helper::InitRecordDamageCharType();
	}
	
	initCSV = true;
}

bool UT_ServerHelper::IsEscortNPC(int charType)
{
	for(int i=0; i<escortNPCVec.size(); ++i)
	{
		if(escortNPCVec[i] == charType)
		{
			return true;
		}
	}

	return false;
}

bool UT_ServerHelper::IsEscortTask(int taskID)
{
	for(int i=0; i<escortTaskVec.size(); ++i)
	{
		if(escortTaskVec[i] == taskID)
		{
			return true;
		}
	}

	return false;
}

bool UT_ServerHelper::GetGoalVec(int type, mem::vector<int>& goalIDVec)
{
	mem::map<int, mem::vector<int> >::iterator findIt = goalTypeAndID.find(type);
	if(findIt != goalTypeAndID.end())
	{
		for(int i=0; i<findIt->second.size(); ++i)
		{
			goalIDVec.push_back(findIt->second[i]);
		}
		return true;
	}
	
	return false;
}

void UT_ServerHelper::LoadServerFirstKillBossInfo()
{
	if(!loadServerFirstKillBossInfo)
	{
		CF_BossFirstKill::DataEntryMapExternIterator it = CF_BossFirstKill::GetSingletonPtr()->GetIterator();
		for(; it.HasMoreElements(); it.MoveNext())
		{
			CF_BossFirstKill::DataEntry* pkData = it.PeekNextValuePtr();

			std::string result;
			mem::vector<uint64_t> input;
			input.push_back(GetGlobalSetting.BossFirstKillScheduleID);
			input.push_back(pkData->_CharTypeID);
			bool tag = UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_BOSS_FIRST_KILL, input, result);
			if(tag)
			{
				BossFirstKillPlayerInfo info;
				if(result == "0")
				{
					serverFirstKillBossRecord.insert(std::make_pair(pkData->_CharTypeID, info));
				}
				else
				{
					std::string errorMsg;
					if(boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
					{
						serverFirstKillBossRecord.insert(std::make_pair(pkData->_CharTypeID, info));
					}
				}				
			}
		}

		CF_BuddyThemeBossType::DataEntryMapExternIterator it1 = CF_BuddyThemeBossType::GetSingletonPtr()->GetIterator();
		for(; it.HasMoreElements(); it.MoveNext())
		{
			CF_BuddyThemeBossType::DataEntry* pkData = it1.PeekNextValuePtr();

			mem::vector<uint64_t> input;
			input.push_back(pkData->_iStoryID);
			int32_t lastEndTime = 0;
			UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_STORT_END_TIME, input, lastEndTime);
			if(0 != lastEndTime)
			{
				SetHuanJingEndTime(pkData->_iStoryID, lastEndTime);
			}
		}

		LoadOfflineAnswerResultFromRedis();

		loadServerFirstKillBossInfo = true;
	}
}

bool UT_ServerHelper::CheckBossBeKilled(int npcID)
{
	mem::map<int, BossFirstKillPlayerInfo>::iterator findIt = serverFirstKillBossRecord.find(npcID);
	if(findIt == serverFirstKillBossRecord.end())
	{
		return false;
	}

	return findIt->second.killerLevel==0 ? false : true;
}

void UT_ServerHelper::UpdateServerFirstKillBossPlayerInfo(int npcID, std::string& playerName, int playerLevel, int commanderTitle, int profilePictureID, int commanderTitleFrameID)
{
	mem::map<int, BossFirstKillPlayerInfo>::iterator findIt = serverFirstKillBossRecord.find(npcID);
	if(findIt == serverFirstKillBossRecord.end())
	{
		BossFirstKillPlayerInfo info;
		info.killerCommanderTitleID = commanderTitle;
		info.killerbornCharType = profilePictureID;
		info.killerLevel =playerLevel;
		info.killerName = playerName;
		info.KillerCommanderTitleFrameID = commanderTitleFrameID;

		serverFirstKillBossRecord.insert(std::make_pair(npcID, info));
	}
	else
	{
		findIt->second.killerCommanderTitleID = commanderTitle;
		findIt->second.killerbornCharType = profilePictureID;
		findIt->second.killerLevel =playerLevel;
		findIt->second.killerName = playerName;
		findIt->second.KillerCommanderTitleFrameID = commanderTitleFrameID;
	}
}

void UT_ServerHelper::UpdateServerFirstKillBossRewardPlayerInfo(LC_ServerPlayer* pkPlayer, int npcID, std::string& playerName, int playerLevel, int commanderTitle, int profilePictureID, int commanderTitleFrameID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	mem::map<int, BossFirstKillPlayerInfo>::iterator findIt = serverFirstKillBossRecord.find(npcID);
	if(findIt == serverFirstKillBossRecord.end())
	{
		return;
	}
	
	BossFirstKillPlayerInfo bossInfo;
	bossInfo.killerLevel = findIt->second.killerLevel;
	bossInfo.killerName = findIt->second.killerName;
	bossInfo.killerbornCharType = findIt->second.killerbornCharType;
	bossInfo.killerCommanderTitleID = findIt->second.killerCommanderTitleID;
	bossInfo.KillerCommanderTitleFrameID = findIt->second.KillerCommanderTitleFrameID;

	for(int i=0; i<findIt->second.rewardPlayerName.size(); ++i)
	{
		bossInfo.rewardPlayerName.push_back(findIt->second.rewardPlayerName[i]);
		bossInfo.rewardPlayerLevel.push_back(findIt->second.rewardPlayerLevel[i]);
		bossInfo.rewardPlayerBornCharType.push_back(findIt->second.rewardPlayerBornCharType[i]);
		bossInfo.rewardPlayerCommanderTitleID.push_back(findIt->second.rewardPlayerCommanderTitleID[i]);
		bossInfo.rewardPlayerCommanderTitleFrameID.push_back(findIt->second.rewardPlayerCommanderTitleFrameID[i]);
	}

	bossInfo.rewardPlayerName.push_back(playerName);
	bossInfo.rewardPlayerLevel.push_back(playerLevel);
	bossInfo.rewardPlayerCommanderTitleID.push_back(commanderTitle);
	bossInfo.rewardPlayerBornCharType.push_back(profilePictureID);
	bossInfo.rewardPlayerCommanderTitleFrameID.push_back(commanderTitleFrameID);

	pkPlayer->SyncBossFirstKillInfo(npcID, bossInfo);

	int size = findIt->second.rewardPlayerLevel.size();
	if(size >= 10)
	{
		return;
	}

	mem::vector<uint64_t> input;
	input.push_back(GetGlobalSetting.BossFirstKillScheduleID);
	input.push_back(npcID);

	std::stringstream ss;
	boost::ajson::save_to_buff(bossInfo, ss);
	std::string data = ss.str();

	UT_ServerHelper::RedisSetData_String(UT_REDIS_KEY_BOSS_FIRST_KILL, input, data);

	MG_SS_NoticeBossFirstKillRewardInfo msg;
	msg.m_nBossID = npcID;
	msg.m_nRewardInfo = data;
	SendMsgToLogin(MGPT_SS_NOTICEBOSSFIRSTKILLREWARDINFO_LOGIN, &msg);
}

void UT_ServerHelper::UpdateServerFirstKillBossRewardPlayerInfo(int npcID, const std::string& info)
{
	mem::map<int, BossFirstKillPlayerInfo>::iterator findIt = serverFirstKillBossRecord.find(npcID);
	if(findIt == serverFirstKillBossRecord.end())
	{
		return;
	}

	BossFirstKillPlayerInfo data;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(data, info.c_str(), errorMsg))
	{
		return;
	}

	findIt->second = data;

	MG_SyncBossFirstKillInfo syncMsg;
	syncMsg.m_nBossID = npcID;
	syncMsg.m_bKillInfo = false;
	syncMsg.m_nKillerInfo.m_nPlayerName = findIt->second.killerName;
	syncMsg.m_nKillerInfo.m_nBornCharType = findIt->second.killerbornCharType;
	syncMsg.m_nKillerInfo.m_nCommanderTitleID = findIt->second.killerCommanderTitleID;
	syncMsg.m_nKillerInfo.m_nPlayerLevel = findIt->second.killerLevel;
	syncMsg.m_nKillerInfo.m_nCommanderTitleFrameID = findIt->second.KillerCommanderTitleFrameID;

	int num = findIt->second.rewardPlayerBornCharType.size();
	syncMsg.m_nRewardPlayerInfo.reserve(num);
	for(int i=0; i<num; ++i)
	{
		PS_BossFirstKillPlayerInfo rewardPlayerInfo;
		rewardPlayerInfo.m_nPlayerName = findIt->second.rewardPlayerName[i];
		rewardPlayerInfo.m_nPlayerLevel = findIt->second.rewardPlayerLevel[i];
		rewardPlayerInfo.m_nCommanderTitleID = findIt->second.rewardPlayerCommanderTitleID[i];
		rewardPlayerInfo.m_nBornCharType = findIt->second.rewardPlayerBornCharType[i];
		rewardPlayerInfo.m_nCommanderTitleFrameID = findIt->second.rewardPlayerCommanderTitleFrameID[i];
		syncMsg.m_nRewardPlayerInfo.push_back(rewardPlayerInfo);
	}

	GetGameServerApp()->BroadCastMsgToClient(MGPT_SYNC_BOSS_FIRST_KILL_INFO, &syncMsg);
}

bool UT_ServerHelper::GetServeFirstKillBossInfo(int npcID, struct BossFirstKillPlayerInfo& info)
{
	mem::map<int, BossFirstKillPlayerInfo>::iterator findIt = serverFirstKillBossRecord.find(npcID);
	if(findIt == serverFirstKillBossRecord.end())
	{
		return false;
	}

	info.killerbornCharType = findIt->second.killerbornCharType;
	info.killerCommanderTitleID = findIt->second.killerCommanderTitleID;
	info.killerLevel = findIt->second.killerLevel;
	info.killerName = findIt->second.killerName;
	info.KillerCommanderTitleFrameID = findIt->second.KillerCommanderTitleFrameID;

	int num = findIt->second.rewardPlayerBornCharType.size();
	for(int i=0; i<num; ++i)
	{
		info.rewardPlayerName.push_back(findIt->second.rewardPlayerName[i]);
		info.rewardPlayerLevel.push_back(findIt->second.rewardPlayerLevel[i]);
		info.rewardPlayerCommanderTitleID.push_back(findIt->second.rewardPlayerCommanderTitleID[i]);
		info.rewardPlayerBornCharType.push_back(findIt->second.rewardPlayerBornCharType[i]);
		info.rewardPlayerCommanderTitleFrameID.push_back(findIt->second.rewardPlayerCommanderTitleFrameID[i]);
	}

	return true;
}

void UT_ServerHelper::RankSubB(int groupID)
{
	const CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs* entryPtrs = CActSubordinateRewardManager::GetSingletonPtr()->GetActSubordinateRewardEntrysByGroupID(groupID);
	if(NULL == entryPtrs)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RankSubB_1 not find group! groupID:%d\n", groupID);
		return;
	}

	CF_ACTSubordinateRewardB::DataEntry* pkEntry = *entryPtrs->begin();
	if(NULL == pkEntry)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RankSubB_2 entry is nullptr! groupID:%d\n", groupID);
		return;
	}
 
	//tag：是否有快照数据
	//如果快照数据已经有，则证明已经发过奖了，不再发奖，并存储发奖记录
	mem::vector<uint64_t> allPlayers;
	bool tag = GetPlayerRankRangeFromRedis(pkEntry->_iType, pkEntry->_GroupID, allPlayers);
	if(tag)
	{
		UpdateRankRecord(groupID);
		return;
	}

	if(allPlayers.empty())
	{
		return;
	}

	DoSendRankRewardToPlayer(pkEntry->_iType, pkEntry->_GroupID, allPlayers, RewardXlsx_Type_ShotActivityLimitRankRewards);
}

void UT_ServerHelper::UpdateWatchTowerInfoCache(uint64_t id, const std::string& str)
{
	mem::map<uint64_t, std::string>::iterator findIt=watchTowerInfoCache.find(id);
	if(findIt != watchTowerInfoCache.end())
	{
		findIt->second = str;
	}
	else
	{
		watchTowerInfoCache.insert(std::make_pair(id, str));
	}
}

void UT_ServerHelper::UpdateWatchTowerInfoCacheToRedis()
{
	if(false == GetGameServerApp()->IsNodeType_Global())
	{
		return;
	}

	bool notifyNode = false;
	mem::vector<uint64_t> idVec;
	for(mem::map<uint64_t, std::string>::iterator it=watchTowerInfoCache.begin(); it!=watchTowerInfoCache.end(); ++it)
	{
		mem::vector<uint64_t> key;
		key.push_back(it->first);

		bool tag = UT_ServerHelper::RedisSetData_String(UT_REDIS_KEY_WATCH_TOWER_INFO, key, it->second, false, true);
		if(tag)
		{
			//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerInfoCacheToRedis_Success key:%d value:%s\n", it->first, it->second.c_str());
			idVec.push_back(it->first);
		}
		else
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerInfoCacheToRedis_Fail key:%d\n", it->first);
		}
	}

	for(int i=0; i<idVec.size(); ++i)
	{
		mem::map<uint64_t, std::string>::iterator findIt = watchTowerInfoCache.find(idVec[i]);
		watchTowerInfoCache.erase(findIt);
		notifyNode = true;
	}

	if(notifyNode)
	{
		MG_SS_Notice_UpdateWatchTowerInfo msg;
		SendMsgToLogin(MGPT_SS_NOTICE_UPDATE_WATCHTOWER_INFO_LOGIN, &msg);
	}
}

void UT_ServerHelper::UpdateWatchTowerTime(bool vaild, int64_t timeLen, int64_t endTime)
{
	if(true == vaild)
	{
		return;
	}

	mem::vector<uint64_t> key;
	key.push_back(GameStoryIDType_BestGuild);

	WatchTowerVaildTime info;
	info.m_nTimeLen = timeLen;
	info.m_nEndTime = endTime;

	std::stringstream ss;
	boost::ajson::save_to_buff(info, ss);
	std::string data = ss.str();

	RedisSetData_String(UT_REDIS_KEY_WATCHTOWER_TIME_INFO, key, data, false, true);
}

bool UT_ServerHelper::CheckWatchTowerInfoValid(int64_t timeLen, int64_t endTime)
{
	mem::vector<uint64_t> key;
	key.push_back(GameStoryIDType_BestGuild);

	WatchTowerVaildTime info;
	std::string str;
	bool tag= RedisGetData_String(UT_REDIS_KEY_WATCHTOWER_TIME_INFO, key, str, false);
	if(tag)
	{
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(info, str.c_str(), errorMsg))
		{
			watchTowerInfoFlag = false;
			return watchTowerInfoFlag;
		}
	}
	else
	{
		watchTowerInfoFlag = false;
		return watchTowerInfoFlag;
	}

	if(timeLen!= info.m_nTimeLen || endTime!=info.m_nEndTime)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "WatchTowerInfoValid! serverid:%d, oldTimeLen:%d, oleEndTime:%d, timeLen:%d, endTime:%d\n",
			GetGameServerID(), info.m_nTimeLen, info.m_nEndTime/1000, timeLen, endTime/1000);

		watchTowerInfoFlag = false;
		return watchTowerInfoFlag;
	}

	watchTowerInfoFlag = true;
	return watchTowerInfoFlag;
}

bool UT_ServerHelper::GetWatchTowerInfoValidFlag()
{
	return watchTowerInfoFlag;
}

void UT_ServerHelper::LoadWatchTowerFromRedis(bool vaild)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadWatchTowerFromRedis serverid:%d, vaild:%d\n", GetGameServerID(), vaild);
	if(false == vaild)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadWatchTowerFromRedis invaild! serverid:%d\n", GetGameServerID());
		RedisRank_Clear(UT_REDIS_KEY_WATCH_TOWER_INFO);
	}
	else
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadWatchTowerFromRedis vaild! reload... serverid:%d\n", GetGameServerID());
		RedisGetData_StringRange(UT_REDIS_KEY_WATCH_TOWER_INFO, watchTowerStateResult);
		RedisRank_Clear(UT_REDIS_KEY_WATCH_TOWER_INFO);
		if(watchTowerStateResult.empty())
		{
			return;
		}

		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogicPlaceVector* pkPlaceVec = pkWorldManager->GetMapLogicByRes(GetGlobalSetting.BestGuildMapID);
		if(pkPlaceVec==NULL)
		{
			//watchTowerStateResult.clear();
			return;
		}
	
		LC_ServerMapLogicPlaceVector::iterator it = pkPlaceVec->begin();
		LC_ServerMapLogic_Place* pkPlace = *it;
		if(pkPlace == NULL)
		{
			//watchTowerStateResult.clear();
			return;
		}
		pkPlace->SetLoadFlag();
	}
}

void UT_ServerHelper::LoadWatchTowerFromRedis_finish()
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadWatchTowerFromRedis_finish serverid:%d\n", GetGameServerID());
	RedisGetData_StringRange(UT_REDIS_KEY_WATCH_TOWER_INFO, watchTowerStateResult);
}

void UT_ServerHelper::UpdateWatchTowerFromSaveState()
{
	if(watchTowerStateResult.empty())
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromSaveState r1");
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogicPlaceVector* pkPlaceVec = pkWorldManager->GetMapLogicByRes(GetGlobalSetting.BestGuildMapID);
	if(pkPlaceVec==NULL)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromSaveState r2");
		return;
	}

	LC_ServerMapLogicPlaceVector::iterator it = pkPlaceVec->begin();
	LC_ServerMapLogic_Place* pkPlace = *it;
	if(pkPlace == NULL)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromSaveState r3");
		return;
	}

	for(mem::map<uint64_t, std::string>::iterator it=watchTowerStateResult.begin(); it!=watchTowerStateResult.end(); ++it)
	{
		pkPlace->UpdateWatchTowerFromSaveState(it->first, it->second);
	}
}

bool UT_ServerHelper::UpdateWatchTowerFromRedis(bool broadcast)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogicPlaceVector* pkLogicMaps = pkWorldManager->GetMapLogicByRes(GetGlobalSetting.BestGuildMapID);
	if(NULL==pkLogicMaps)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "UpdateWatchTowerFromRedis NULL==pkLogicMaps serverid:%d, broadcast:%d BestGuildMapID %d \n", GetGameServerID(), broadcast,GetGlobalSetting.BestGuildMapID);
		return false;
	}

	LC_ServerMapLogicPlaceVector::iterator it=pkLogicMaps->begin();
	LC_ServerMapLogic_Place* pkBestGuildPlace = *it;
	if(NULL == pkBestGuildPlace)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "UpdateWatchTowerFromRedis NULL==pkBestGuildPlace serverid:%d, broadcast:%d BestGuildMapID %d \n", GetGameServerID(), broadcast,GetGlobalSetting.BestGuildMapID);
		return false;
	}

	pkBestGuildPlace->UpdateWatchTowerFromRedis();

	if(broadcast)
	{
		pkBestGuildPlace->BroadcastWatchTowerInfo();
	}

	return true;
}

void UT_ServerHelper::LoadBossDamageInfoFromRedis(bool vaild)
{
	if(true == vaild)
	{
		RedisGetData_StringRange(UT_REDIS_KEY_BOSS_DAMAGE_INFO, bossDamageInfo);
	}

	RedisRank_Clear(UT_REDIS_KEY_BOSS_DAMAGE_INFO);
}

void UT_ServerHelper::UpdateBossDamageInfo(LC_NPCBase* pkNpc)
{
	if(NULL==pkNpc || bossDamageInfo.empty())
	{
		return;
	}

	uint64_t key = LC_Helper::EncodeScore(pkNpc->GetCharType(), pkNpc->GetMapResID());
	mem::map<uint64_t, std::string>::iterator findIt=bossDamageInfo.find(key);
	if(findIt !=bossDamageInfo.end())
	{
		pkNpc->UpdateDamageInfoFromRedis(findIt->second);
		bossDamageInfo.erase(findIt);
	}
}

uint32_t UT_ServerHelper::GetBestGuildRankClearTime()
{
	mem::map<uint32_t,RankOpenInfo>::iterator findIt = gRankOpenInfoMap.find(ROIT_Total);
	if(findIt == gRankOpenInfoMap.end())
	{
		return 0;
	}
	return findIt->second.uClearTime;
}

void UT_ServerHelper::SetRewardTaskEndTime(int32_t time)
{
	rewardTaskEndTime = time;
}

int32_t UT_ServerHelper::GetRewardTaskEndTime()
{
	return rewardTaskEndTime;
}

void UT_ServerHelper::GetRealtimeCombatRecorder(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(pkOwnerPlayer->GetMapLogicID());
	if(NULL==pkPlace)
	{
		return;
	}

	MG_SC_RealtimeCombatRecorder data;
	data.m_nGuildDamageValue = pkPlace->GetTotalDamage();

	int32_t mapType = pkPlace->GetMapLogicType();
	switch(mapType)
	{
	case MT_REANSCRIPTION_RANK:
	case MT_GUILD_MAP:
		data.m_nDamageValue = pkOwnerPlayer->GetWriteCombatRecorderData().GetValue();		
		break;
	case MT_GUILD_MAP2:
		data.m_nDamageValue = pkPlace->GetPlayerDamage(pkOwnerPlayer->GetInstance());
		break;
	}

	pkOwnerPlayer->SendMsgToClient(MGPT_SYNC_REALTIME_COMBAT_RECORDER, &data);
}

uint64_t UT_ServerHelper::GetActorDamageData(LC_ServerPlayer* pkOwnerPlayer, object_id_type id, int32_t type)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return 0;
	}

	if(0 == pkOwnerPlayer->GetGuildID())
	{
		return 0;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(pkOwnerPlayer->GetMapLogicID());
	if (NULL == pkPlace)
	{
		return 0;
	}

	int32_t storyID = pkPlace->GetStoryID();
	GameStory::LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	GameStory::LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
	if(NULL == pkStory)
	{
		return 0;
	}

	return pkStory->GetActorDamageData(pkOwnerPlayer, id, type);
}

void UT_ServerHelper::AddMail(MailInfo& info)
{
	MainInfo.push_back(info);
}

void UT_ServerHelper::UpdateMail()
{
	mem::vector<MailInfo>::iterator it=MainInfo.begin();
	while(it != MainInfo.end())
	{
		ResultType res = UT_ServerHelper::SendToMail(it->m_nMailTitle, it->m_nMailDesc, it->m_nPlayerID, "", 0, it->m_nReward, "", it->nLogCode, it->nExtraParam);
		if(RE_SUCCESS == res)
		{
			it = MainInfo.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void UT_ServerHelper::GetSuffixByType(StringType &outStr, int32_t nType,int32_t nStoryId)
{
	std::ostringstream streamStoryId;
	//ostringstream streamType;
	//streamType<<nType;  //n为int类型
	streamStoryId << nStoryId;
	//std::string  strType = streamType.str();
	StringType  strStageType = streamStoryId.str().c_str();
	//outStr += "_";
	//outStr += strType;
	outStr += "_";
	outStr += strStageType;
}

int UT_ServerHelper::GetAccountPointRankIndexFromRedisByTypeAndStoryId(uint64_t guildId, uint64_t playerID, int32_t type, int storyID)
{
	mem::vector<uint64_t> allIdsTemp;
	UT_ServerHelper:: GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(allIdsTemp, type,storyID);
	if(allIdsTemp.size() <= 0)
	{
		return -1;
	}
	uint64_t tempId = guildId;
	if(GuildFightRankType::GFRT_PERSONAL ==type)
		tempId = playerID;
	for(int32_t i=0; i<allIdsTemp.size(); ++i)
	{
		if(tempId == allIdsTemp[i])
		{
			return i;
		}
	}
	return -1;
}

void  UT_ServerHelper::GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(mem::vector<uint64_t>& allIds,int32_t nType,int32_t nStoryId)
{
	StringType redisPointStr ="";
	UT_ServerHelper:: GetRedisRankPonitNameByType(redisPointStr, nType,nStoryId);
	UT_ServerHelper::RedisRank_ReadSimpleRange(redisPointStr.c_str(), allIds);
}

void  UT_ServerHelper::GetRedisRankPonitNameByType(StringType &outStr, int32_t nType,int32_t nStoryId)
{
	StringType Suffix = "";
	GetSuffixByType(Suffix,nType,nStoryId);
	if(nType == GuildFightRankType::GFRT_PERSONAL)
		outStr += UT_REDIS_KEY_PERSONAL_RANK_POINT_BY_TYPE;
	else if(nType == GuildFightRankType::GFRT_GUILD)
		outStr += UT_REDIS_KEY_GUILD_RANK_POINT_BY_TYPE;
	outStr += Suffix;
}

void  UT_ServerHelper::GetRedisRankPlayerInfoNameByType(StringType &outStr, int32_t nType,int32_t nStoryId)
{
	StringType Suffix = "";
	GetSuffixByType(Suffix,nType,nStoryId);
	if(nType == GuildFightRankType::GFRT_PERSONAL)
		outStr += UT_REDIS_KEY_PERSONAL_RANK_PLAYER_INFO_BY_TYPE;
	else if(nType == GuildFightRankType::GFRT_GUILD)
		outStr += UT_REDIS_KEY_GUILD_RANK_PLAYER_INFO_BY_TYPE;
	outStr += Suffix;
}

uint64_t UT_ServerHelper::MakeValueByTimeAndPoint(uint32_t uPoint)
{
	uint32_t currentTime  = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	uint32_t inputTime = LC_Helper::TimeStampReverse(currentTime);
	return LC_Helper::EncodeScore(uPoint, inputTime);
}

void UT_ServerHelper::MakeGuildOPersonalInfoToJson(std::string &outStr,uint64_t uPoint,std::string &sGuildName,std::string &sPlayerName, int32_t nTitleId, uint64_t nGuild)
{
	GuildOPersonalInfo info;
	info.sPlayerName = sPlayerName;
	info.sGuildName = sGuildName;
	info.uPoint = uPoint;
	uint64_t uHi32Bit = 0;
	uint64_t nLow32Bit = 0;
	LC_Helper::DecodeKey(uPoint, uHi32Bit, nLow32Bit);
	int64_t nHi32Bit = uHi32Bit ;
	info.sLastTime = LC_Helper::TimeStampToString(&nHi32Bit);
	info.nGuild = nGuild;
	info.nTitleId = nTitleId;
	std::stringstream ss;
	boost::ajson::save_to_buff(info, ss);
	outStr = ss.str();
}
void UT_ServerHelper::UpdatePlayerInfoGuildName(uint64_t uId,std::string sPlayerName, std::string sNewGuildName,uint64_t uGuildMasterId,uint64_t nGuild,int nTitleId)
{
	mem::vector<uint64_t> key;
	key.push_back(uId);
	uint32_t uNowTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	// 检查还在进行中的 排行榜
	for (mem::map<uint32_t,RankOpenInfo>::iterator itc = gRankOpenInfoMap.begin(); itc != gRankOpenInfoMap.end(); ++itc)
	{
		RankOpenInfo &info = itc->second;
		if(info.uStartTime != 0 &&info.uEndTime != 0 &&  info.uStartTime <= uNowTime&&  info.uEndTime >= uNowTime && info.iRankId != ROIT_InTotal)
		{
			StringType redisPlayerInfoStr ="";
			UT_ServerHelper:: GetRedisRankPlayerInfoNameByType(redisPlayerInfoStr, GuildFightRankType::GFRT_PERSONAL,info.iRankId);
			std::string data;
			UT_ServerHelper::RedisGetData_String(redisPlayerInfoStr.c_str(), key, data, false);
			GuildOPersonalInfo info;
			std::string errorMsg;
			if(!boost::ajson::load_from_buff(info, data.c_str(), errorMsg))
				continue ;
			info.sPlayerName = sPlayerName;
			info.sGuildName = sNewGuildName;
			info.uGuildMasterId = uGuildMasterId;
			info.nGuild = nGuild;
			info.nTitleId = nTitleId;
			std::string sEndStr;
			std::stringstream ss;
			boost::ajson::save_to_buff(info, ss);
			sEndStr = ss.str();
			UT_ServerHelper::RedisSetData_String(redisPlayerInfoStr.c_str(), key, sEndStr, false, true);
		}
	}
}
void  UT_ServerHelper::CommitPersonalPointByStageType(int32_t nStoryId,uint64_t uCharID ,uint32_t uPoint,std::string &sGuildName,std::string &sPlayerName, int32_t nTitleId )
{
	if(uCharID == 0)
		return;
	//更新redis中的积分
	uint64_t value = UT_ServerHelper::MakeValueByTimeAndPoint(uPoint);

	StringType redisPointStr ="";
	UT_ServerHelper:: GetRedisRankPonitNameByType(redisPointStr, GuildFightRankType::GFRT_PERSONAL,nStoryId);
	UT_ServerHelper::RedisRank_WriteElement(redisPointStr.c_str(), uCharID, value);
	
	std::string dataStr = "";
	MakeGuildOPersonalInfoToJson(dataStr,value,sGuildName,sPlayerName,nTitleId,0);

	mem::vector<uint64_t> key;
	key.push_back(uCharID);

	StringType redisPlayerInfoStr ="";
	UT_ServerHelper:: GetRedisRankPlayerInfoNameByType(redisPlayerInfoStr, GuildFightRankType::GFRT_PERSONAL,nStoryId);
	UT_ServerHelper::RedisSetData_String(redisPlayerInfoStr.c_str(), key, dataStr, false, true);
}

void  UT_ServerHelper::CommitGuildPointByStageType(int32_t nStoryId,uint64_t uGuildID ,uint32_t uPoint,std::string &sGuildName)
{
	if(uGuildID == 0)
		return;
	//更新redis中的积分
	uint64_t value = UT_ServerHelper::MakeValueByTimeAndPoint(uPoint);

	StringType redisPointStr ="";
	UT_ServerHelper:: GetRedisRankPonitNameByType(redisPointStr, GuildFightRankType::GFRT_GUILD,nStoryId);
	UT_ServerHelper::RedisRank_WriteElement(redisPointStr.c_str(), uGuildID, value);

	std::string dataStr = "";
	std::string playerStr = "";
	MakeGuildOPersonalInfoToJson(dataStr,value,sGuildName,playerStr,0,uGuildID);

	mem::vector<uint64_t> key;
	key.push_back(uGuildID);

	StringType redisPlayerInfoStr ="";
	UT_ServerHelper:: GetRedisRankPlayerInfoNameByType(redisPlayerInfoStr, GuildFightRankType::GFRT_GUILD,nStoryId);
	UT_ServerHelper::RedisSetData_String(redisPlayerInfoStr.c_str(), key, dataStr, false, true);
}

bool UT_ServerHelper::Pack_Single_info(StringType &redisPlayerInfoStr,int32_t nIndex,uint64_t uId,PS_RANK_STRUCT& pushInfo)
{
	mem::vector<uint64_t> key;
	key.push_back(uId);

	std::string data;
	UT_ServerHelper::RedisGetData_String(redisPlayerInfoStr.c_str(), key, data, false);

	GuildOPersonalInfo info;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, data.c_str(), errorMsg))
		return false;
	pushInfo.m_nRankIndex	=	nIndex;
	pushInfo.m_nGuildName	=	info.sGuildName;
	pushInfo.m_nPlayerName	=	info.sPlayerName;
	pushInfo.m_nGuild		=	info.nGuild;
	pushInfo.m_nTitleId		=	info.nTitleId;

	uint64_t nHi32Bit = 0;
	uint64_t nLow32Bit = 0;
	LC_Helper::DecodeKey(info.uPoint, nHi32Bit, nLow32Bit);
	//uint32_t uReverse = common_helper::TimeStampReverse(nLow32Bit);
	pushInfo.m_nPoint		=	nHi32Bit;
	pushInfo.m_uGuildMasterId = info.uGuildMasterId;
	//GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_ERROR, "Pack_Single_info other. playername:%s, guildname:%s, rank:%d, point:%d, ntitle:%d\n", pushInfo.m_nPlayerName.c_str(),pushInfo.m_nGuildName.c_str(), pushInfo.m_nRankIndex,pushInfo.m_nPoint, pushInfo.m_nTitleId);
	return true;
}

bool UT_ServerHelper::SendGuildFightPersonalRewardByStoryId(int32_t nStoryId)
{
	std::stringstream sLog;
	sLog<<"公会战 个人榜发奖 id = " <<nStoryId;
	mem::vector<uint64_t> allIds;
	UT_ServerHelper:: GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(allIds, GuildFightRankType::GFRT_PERSONAL,nStoryId);
	if(allIds.size() <= 0)
		return false;
	sLog <<" num = "<<allIds.size();
	sLog<<" [";
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, 0);
	for(int i=0; i<allIds.size(); ++i)
	{
		CF_BestGuildPersonalRankReward::DataEntryMapExternIterator kIter = CF_BestGuildPersonalRankReward::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_BestGuildPersonalRankReward::DataEntry* pkData = kIter.PeekNextValuePtr();	
			if(pkData->_iType == nStoryId && pkData->_iRankStartIndex <= i && i < pkData->_iRankEndIndex )
			{
				int nExtraParam = pkData->_iID * 10000 + i+1;
				sLog<<" ( charID = " << allIds[i]<<" index = " << i+1;
				if(RE_SUCCESS != UT_ServerHelper::SendMailToPlayer(allIds[i],pkData->_iMailTitle, pkData->_iMailDesc, pkData->_kReward, false, nLogCode, nExtraParam))
				{
					sLog << " false) ";
					break;
				}
				sLog << " true)";
				break;
			}
		}
	}
	sLog<<" ]";
	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s",sLog.str().c_str());
	return true;
}

void UT_ServerHelper::SendSocietyGetMemberToGuildRankReward(int32_t id)
{
	std::stringstream sLog;
	sLog<<"公会战 公会榜发奖 send society id = " <<id ;
	mem::vector<uint64_t> allIds;
	UT_ServerHelper:: GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(allIds, GuildFightRankType::GFRT_GUILD,id);
	if(allIds.size() <= 0)
		return;
	sLog <<" num = "<<allIds.size() <<" [";
	std::map<uint32_t, uint32_t> m_score;
	for(int i=0; i<allIds.size(); ++i)
	{
		uint64_t _nGuild = allIds[i];
		sLog <<"(_nGuild =  " <<_nGuild <<" index = " << i+1<<")";
		UT_ServerHelper::ReqGuildFightInfoToSociety(UpdateGuildFightType::UGFT_GET_ALL_MEMBER_LEVEL,_nGuild,i,false,id,0,m_score);
	}
	sLog<<" ]";
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d]SendSocietyGetMemberToGuildRankReward:%s", UT_ServerHelper::GetServerId(),sLog.str().c_str());
	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s",sLog.str().c_str());
}


bool UT_ServerHelper::SendGuildFightGuildRewardByStoryId(uint64_t nGuild, int32_t rankId,int32_t index ,std::map<uint64_t,uint32_t> & memberLevel)
{
	std::stringstream sLog;
	sLog<<"received society 公会战 公会榜发奖 nGuild = " <<nGuild<<" rankId =  " << rankId <<" index = "<<index;
	sLog <<" num = "<<memberLevel.size();
	sLog<<" [";
	CF_BestGuildGuildRankReward::DataEntryMapExternIterator kIter = CF_BestGuildGuildRankReward::GetSingletonPtr()->GetIterator();
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE1);
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BestGuildGuildRankReward::DataEntry* pkData = kIter.PeekNextValuePtr();	
		if(pkData->_iType == rankId && pkData->_iRankStartIndex <= index && index < pkData->_iRankEndIndex )
		{
			for(std::map<uint64_t,uint32_t>::iterator itc = memberLevel.begin(); itc != memberLevel.end(); ++itc)
			{
				uint64_t uCharID = itc->first;
				uint32_t position = itc->second;
				sLog<<" ( uCharID = " << uCharID <<" position = "<<position;
				StringType sReward = pkData->_kReward4;
				if(position == GUILD_MEMBER_MASTER)
					sReward = pkData->_kReward1;
				else if(position == GUILD_MEMBER_GUARDIAN)
					sReward = pkData->_kReward2;
				else if(position == GUILD_MEMBER_ELDER)
					sReward = pkData->_kReward3;
				int nExtraParam = pkData->_iType * 10000 + index+1;
				if(RE_SUCCESS != UT_ServerHelper::SendMailToPlayer(uCharID,pkData->_iMailTitle, pkData->_iMailDesc, sReward, false, nLogCode, nExtraParam))
				{
					sLog << " false) ";
					continue;
				}
				sLog << " true)";
			}
			break;
		}
	}		
	
	sLog<<" ]";
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d]SendGuildFightGuildRewardByStoryId:%s", UT_ServerHelper::GetServerId(),sLog.str().c_str());
	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s",sLog.str().c_str());
	return true;
}
void UT_ServerHelper::ClearGuildFightRedisInfo(int32_t nId)
{
	ClearGuildFightRedisInfoByStoryIdAndType(nId,GuildFightRankType::GFRT_GUILD);
	ClearGuildFightRedisInfoByStoryIdAndType(nId,GuildFightRankType::GFRT_PERSONAL);
}
void UT_ServerHelper::ClearGuildFightRedisInfoByStoryIdAndType(int32_t nStoryId,int8_t nType)
{
	StringType redisPointStr ="";
	StringType redisPlayerInfoStr ="";
	UT_ServerHelper:: GetRedisRankPonitNameByType(redisPointStr, nType,nStoryId);
	UT_ServerHelper:: GetRedisRankPlayerInfoNameByType(redisPlayerInfoStr,nType,nStoryId);
	UT_ServerHelper::RedisRank_Clear(redisPointStr.c_str());
	UT_ServerHelper::RedisRank_Clear(redisPlayerInfoStr.c_str());
}

ResultType UT_ServerHelper::CheckBuyTypeVaild(uint32_t buyType)
{
	return (buyType>Buy_Raid_Type_Begin&&buyType<Buy_Raid_Type_End) ? RE_SUCCESS : RE_FAIL;
}

int32_t UT_ServerHelper::GetSchemeGroupNum(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID)
{
	int32_t groupNumCondition = -1;
	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == schemeID)
	{
		return groupNumCondition;
	}

	CF_Scheme::DataEntry* pkSchemeData = SafeGetCSVFileDataEntryPtr<CF_Scheme>(schemeID);
	if(NULL == pkSchemeData)
	{
		return groupNumCondition;
	}

	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return groupNumCondition;
	}

	groupNumCondition = pkSchemeData->_schemeGroupNum;
	if(-1 == groupNumCondition)
	{
		switch(mapID)
		{
		case SKY_SOLO_DULE_RES_ID:
			groupNumCondition = pkOwnerPlayer->GetSkyArenaGroupNum();
			break;
		}
	}

	return groupNumCondition;
}

void UT_ServerHelper::GetSchemeLinkID(uint32_t mapID, std::vector<uint32_t>& schemeLinkID)
{
	schemeLinkID.clear();
	schemeLinkID.reserve(5);

	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == schemeID)
	{
		return;
	}

	schemeLinkID.push_back(schemeID);

	CF_Scheme::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_Scheme>(schemeID);
	while(NULL!=pkData && pkData->_schemeLinkID != -1)
	{
		schemeLinkID.push_back(pkData->_schemeLinkID);
		pkData = SafeGetCSVFileDataEntryPtr<CF_Scheme>(pkData->_schemeLinkID);
	}
}

bool UT_ServerHelper::IsSummitArenaMap(uint32_t mapID)
{
	return mapID==SUMMIT_TEAM_SOLO_DULE_RES_ID;
}

void UT_ServerHelper::SendGuessMailToPlayer(uint32_t guessType, bool success, std::string name, uint32_t summitType, uint32_t againstID, std::vector<uint64_t>& playerIDVec)
{
	std::stringstream sLog;
	sLog << "SendGuessMailToPlayer guessType:[" << guessType << "], summitType:[" <<summitType <<"], againstID:[" << againstID <<"], id_size:[" << playerIDVec.size() << "], success:[" << success << "]";
	CF_Guess::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_Guess>(guessType);
	if(NULL == pkData)
	{
		return;
	}

	int count = 0;
	StringType successMailDesc = pkData->_successMailDesc + STDSTR2TPSTR(name) + ";";
	StringType failMailDesc = pkData->_failMailDesc + STDSTR2TPSTR(name) + ";";
	for(uint32_t i=0; i<playerIDVec.size(); ++i, ++count)
	{
		ResultType res = RE_SUCCESS;
		if(success)
		{
			res = UT_ServerHelper::SendMailToPlayer(playerIDVec[i], pkData->_successMailTitle, successMailDesc, pkData->_guessReward);
		}
		else
		{
			res = UT_ServerHelper::SendMailToPlayer(playerIDVec[i], pkData->_failMailTitle, failMailDesc, "");
		}
		sLog << " playerId:" << playerIDVec[i];

		if(count >= 200)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", sLog.str().c_str());
			sLog.str("");
			sLog << "SendGuessMailToPlayer add PlayerID";
			count = 0;
		}
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", sLog.str().c_str());
}

void UT_ServerHelper::BroadCastRecoverTimesMsg(LC_ServerPlayer* pkOwnerPlayer, int32_t dropType)
{
	if(NULL == pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	CRecoverTimes* pkRecoverAsset = pkOwnerPlayer->GetPKRecoverAssetByType(dropType);
	if(NULL == pkRecoverAsset)
	{
		return;
	}

	pkRecoverAsset->BroadCastRecoverTimesMsg(pkOwnerPlayer);
}


std::string UT_ServerHelper::getDateString(time_t timestamp)
{
	tm t = *localtime(&timestamp);
	char buf[256] = {0};
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	return buf;
}


void UT_ServerHelper::UpdateRankOpenInfo()
{	
	if(GetGameServerApp()->IsNodeType_Global() == false)
		return;
	uint32_t uNowTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	CF_RankOpen* pkRankOpen = CF_RankOpen::GetSingletonPtr();
	// 检查发奖
	for (mem::map<uint32_t,RankOpenInfo>::iterator itc = gRankOpenInfoMap.begin(); itc != gRankOpenInfoMap.end(); ++itc)
	{
		RankOpenInfo &info = itc->second;
		if(info.uClearTime != 0 && info.uClearTime <= uNowTime )
		{
			// 过了清榜时间（不做补发， 以免出现bug 发奖 就是在结束时间发）
			// 1,检查奖励发放没有 补发奖励
			//if(!info.bGuildReward &&info.nConfigReward == 1)
			//{
			//	// 发奖
			//	UT_ServerHelper::SendSocietyGetMemberToGuildRankReward(info.iRankId);	
			//	info.bGuildReward = true;
			//}
			//if(!info.bPersonalReward &&info.nConfigReward == 1)
			//{
			//	// 发奖
			//	UT_ServerHelper::SendGuildFightPersonalRewardByStoryId(info.iRankId);
			//	info.bPersonalReward = true;
			//}
			// 2,清除榜单 ，
			// 通知db 清除榜单中 所有玩家 此id 的榜单数据
			// 通知 society 清除此榜单 id 下 所有数据（个人，公会）
			mem::vector<uint64_t> allIds;
			std::vector<int> vecInId;
			UT_ServerHelper::GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(allIds, GuildFightRankType::GFRT_PERSONAL,info.iRankId);
			if(info.bClearDbAndSocitey)
			{
				if(UT_ServerHelper::ReqRankToDbSociety(GameToDbSocietyRankType::GTDSRT_RankClear,allIds,vecInId,info.iRankId,0 ,0))
					info.bClearDbAndSocitey = false;
			}
			
			//UT_ServerHelper::ClearGuildFightRedisInfo(info.iRankId);

			//3,更新RankOpenInfo
			if(info.bGuildReward && info.bPersonalReward || info.nConfigReward == 0)
			{
				// 重新读取配置 更新redis
				CF_RankOpen::DataEntry* pkData = pkRankOpen->GetEntryPtr(itc->first);
				if(!pkData)
					continue;
				if(true == info.InitRankOpenTimeByExecl(pkData))
				{
					// 清榜时间 没过 新的一期 
					info.InitRankOpenInfoByExecl(pkData);
					UT_ServerHelper::ComitRankInfo(info);
				}
			}
			continue;
		}
		else if(info.uEndTime != 0 && info.uEndTime <= uNowTime )
		{
			if(info.nConfigReward == 0)
				continue;
			// 1,检查奖励发放没有 补发奖励
			if(!info.bGuildReward)
			{
				// 发奖
				UT_ServerHelper::SendSocietyGetMemberToGuildRankReward(info.iRankId);
				info.bGuildReward = true;
			}
			if(!info.bPersonalReward)
			{
				// 发奖
				UT_ServerHelper::SendGuildFightPersonalRewardByStoryId(info.iRankId);	
				info.bPersonalReward = true;
			}
			UT_ServerHelper::ComitRankInfo(info);
			continue;
		}
	}
}

void UT_ServerHelper::LoadRankOpenInfoByExcelToRedis()
{
	if(GetGameServerApp()->IsNodeType_Global() == false)
		return;
	std::stringstream logStr;
	logStr << "  LoadRankOpenInfoByExcelToRedis  ";
	//TODO 后续 排行榜进入条件判断移植回gs 去做 。 这里不需要判断是否是主节点
	for(CF_RankOpen::DataEntryMapExternIterator kIter = CF_RankOpen::GetSingletonPtr()->GetIterator(); kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_RankOpen::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		logStr <<"(_iId : " << pkEntry->_iId;
		rankIdCache[pkEntry->_iId] = 0;
		RankOpenInfo redisInfo;
		if(!UT_ServerHelper::GetRankInfoById(pkEntry->_iId,redisInfo))
		{
			// redis 无排行榜信息 第一次初始化
			RankOpenInfo newInfo;
			if(true == newInfo.InitRankOpenTimeByExecl(pkEntry))
			{
				// 清榜时间 没过 新的一期 
				newInfo.InitRankOpenInfoByExecl(pkEntry);
				gRankOpenInfoMap[pkEntry->_iId] = newInfo;
				ComitRankInfo(newInfo);
				logStr<<",first)";
			}
		
		}
		else
		{
			redisInfo.InitRankOpenTimeByExecl(pkEntry);
			redisInfo.InitRankOpenIdListByExecl(pkEntry);
			gRankOpenInfoMap[redisInfo.iRankId] = redisInfo;
			ComitRankInfo(redisInfo);
			logStr<<",refresh)";
		}
	}
	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s",logStr.str().c_str());
}

void UT_ServerHelper::ComitRankInfo(RankOpenInfo &info)
{
	mem::vector<uint64_t> input;
	input.push_back(info.iRankId);
	std::string inputStr;
	std::stringstream ss;
	boost::ajson::save_to_buff(info, ss);
	inputStr = ss.str();
	RedisSetData_Int_EntryServerID_String(UT_REDIS_KEY_RANK_START_END_TIME,input,inputStr);
}



bool UT_ServerHelper::GetRankInfoById(uint32_t Id,RankOpenInfo &info)
{
	std::string str;
	mem::vector<uint64_t> input;
	input.push_back(Id);
	bool tag = UT_ServerHelper::RedisGetData_Int_EntryServerID_String(UT_REDIS_KEY_RANK_START_END_TIME, input, str);
	if(!tag)
		return false;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, str.c_str(), errorMsg))
		return false;
	return true;
}

void UT_ServerHelper::UpdateCacheRankOpen()
{
	if(gbOpenFlag)
		return;
	for(mem::map<int,int>::iterator itc = rankIdCache.begin(); itc != rankIdCache.end();++itc)
	{
		mem::map<uint32_t,RankOpenInfo>::iterator itc_open = gRankOpenInfoMap.find(itc->first);
		if(itc_open == gRankOpenInfoMap.end())
			continue;
		RankOpenInfo &info = itc_open->second;
		mem::vector<uint64_t> vecId;
		bool ret =	UT_ServerHelper::ReqRankToDbSociety(GameToDbSocietyRankType::GTDSRT_RankOpenTime,vecId,info.vecInId,info.iRankId,info.uStartTime ,info.uEndTime);
		if(ret == true)
			itc->second = 1;
	}
	for(mem::map<int,int>::iterator itc = rankIdCache.begin(); itc != rankIdCache.end();++itc)
	{
		if(itc->second == 0)
			return;
	}
	gbOpenFlag = true;
}

bool UT_ServerHelper::TrySwitchMap(LC_ServerPlayer* pkPlayer, int32_t eInOutType, int32_t lTargetMapLogicType, int32_t srcMapResID, int32_t destMapLogicID, int32_t destMapResID, int32_t x, int32_t y, int32_t z, bool bQuitHookMap2CommonMap, bool bCertainSameServer)
{
	if(NULL == pkPlayer)
	{
		return false;
	}

	if (!bCertainSameServer
		&& (MT_NORMAL==lTargetMapLogicType || MT_NEWBEE==lTargetMapLogicType || MT_WATCHTOWER==lTargetMapLogicType || MT_GUILD_MAP==lTargetMapLogicType || MT_GUILD_MAP2==lTargetMapLogicType || lTargetMapLogicType == MT_GUILD_PARTYFUBEN)
		&& !bQuitHookMap2CommonMap
		&& (eInOutType==LMIOT_QUIT_TRANSCRIPTION || eInOutType == LMIOT_RETURN_CITY || eInOutType == LMIOT_TRANSFER || eInOutType == LMIOT_GROW_UP)
		)
	{
		MG_SS_REQ_Transfer_DifMap	kReqMsg;
		kReqMsg.m_iPlayerGuildID = pkPlayer->GetGuildID();
		kReqMsg.m_eInOutType = eInOutType;
		kReqMsg.m_lSrcMapResID = srcMapResID;
		kReqMsg.m_iSrcServerID = GetServerId();

		kReqMsg.m_iDstMapResID = destMapResID;
		kReqMsg.m_lDstMapLogicID = destMapLogicID;
		kReqMsg.m_lDstMapLogicType = lTargetMapLogicType;
		kReqMsg.m_iDstMapPosX	 = x;
		kReqMsg.m_iDstMapPosY	 = y;
		kReqMsg.m_iDstMapPosZ	 = z;

		SendMsgToLogin(MGPT_SS_REQ_TRANSFER_DIFMAP, &kReqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

		return true;
	}

	return false;
}

void UT_ServerHelper::BroadcastBeFightedToPlayer(unique_id_impl uCharId)
{
	MG_NoticeBeDefighted msg;
	msg.m_nCharID = uCharId;
	msg.m_nBeDefighted = true;
	SendMsgToChat(MGPT_NOTICEBEDEFIGHTED, &msg, 0, msg.m_nCharID);
}

void UT_ServerHelper::UpdateSinglePlayerSingleBuffId(uint64_t uCharId,uint32_t buffId,bool add)
{
	if(buffId == 0 || uCharId == 0)
		return;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkPlayerManager )
		return;
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(uCharId);
	if ( NULL == pkPlayer)
		return;
	if(pkPlayer->EnableGuildFightTowerBuff())
		return;
	if(add)
		pkPlayer->ActiveSkillState(buffId, 0, pkPlayer->GetID());
	else
		pkPlayer->DeactiveSkillState(buffId);
		
}
void UT_ServerHelper::UpdateSingleBuffIdToGuildAllMember(uint64_t GuildId,uint32_t buffId,bool add)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (NULL == pkGuildManager)
		return;
	LC_ServerGuildInfo* p_GuildInfo = pkGuildManager->GetGuildInfoByID(GuildId);
	if(NULL == p_GuildInfo)
		return;
	Guild_Member_Map*p_member = p_GuildInfo->GetAllMemberInfo();
	for (Guild_Member_Map::iterator it = p_member->begin(); it != p_member->end(); ++it)
	{
		uint64_t uCharID = it->second.m_uiCharID;
		UT_ServerHelper::UpdateSinglePlayerSingleBuffId(uCharID,buffId,add);
	}
}


void UT_ServerHelper::AddAllBuffIdToGuildAllMember(uint64_t GuildId)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (NULL == pkGuildManager)
		return;
	LC_ServerGuildInfo* p_GuildInfo = pkGuildManager->GetGuildInfoByID(GuildId);
	if(NULL == p_GuildInfo)
		return;
	CF_BestGuildBossInfo* pkGuildBossInfo = CF_BestGuildBossInfo::GetSingletonPtr();
	mem::map<uint32_t,uint32_t> towerId;
	p_GuildInfo->GetTowerId(towerId);
	for(mem::map<uint32_t,uint32_t>::iterator itc = towerId.begin(); itc != towerId.end(); ++itc)
	{
		CF_BestGuildBossInfo::DataEntry* pkData = pkGuildBossInfo->GetEntryPtr(itc->first);
		if(!pkData)
			continue;
		UT_ServerHelper::UpdateSingleBuffIdToGuildAllMember(GuildId,pkData->_iBuff,true);
	}
}

void UT_ServerHelper::AddAllBuffIdToSingleGuildMember(uint64_t GuildId,uint64_t uCharId)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (NULL == pkGuildManager)
		return;
	LC_ServerGuildInfo* p_GuildInfo = pkGuildManager->GetGuildInfoByID(GuildId);
	if(NULL == p_GuildInfo)
		return;
	CF_BestGuildBossInfo* pkGuildBossInfo = CF_BestGuildBossInfo::GetSingletonPtr();
	mem::map<uint32_t,uint32_t> towerId;
	p_GuildInfo->GetTowerId(towerId);
	for(mem::map<uint32_t,uint32_t>::iterator itc = towerId.begin(); itc != towerId.end(); ++itc)
	{
		CF_BestGuildBossInfo::DataEntry* pkData = pkGuildBossInfo->GetEntryPtr(itc->first);
		if(!pkData)
			continue;
		UT_ServerHelper::UpdateSinglePlayerSingleBuffId(uCharId,pkData->_iBuff,true);
	}
}

bool UT_ServerHelper::IsRegularBossMap(int32_t mapID, int& groupID)
{
	mem::map<int32_t, int32_t>::iterator findIt = regularBossMapGroupInfo.find(mapID);
	if(findIt == regularBossMapGroupInfo.end())
	{
		return false;
	}

	groupID = findIt->second;
	return true;
}

void UT_ServerHelper::InitRegularBossKillerInfo()
{
	if(initRegularBossKillerInfo)
	{
		return;
	}

	mem::map<uint64_t, std::string> vecRangeData;
	mem::vector<uint64_t> nameKey;
	nameKey.push_back(GetGameServerID());
	UT_ServerHelper::RedisReadStringRange(UT_REDIS_KEY_REGULARBOSS_KILLER_INFO, nameKey, vecRangeData);
	for(mem::map<uint64_t, std::string>::iterator it=vecRangeData.begin(); it!=vecRangeData.end(); ++it)
	{
		RegularBossKillerInfo info;
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(info, it->second.c_str(), errorMsg))
		{
			continue;
		}

		UpdateRegularBossKillerInfo(it->first, info);
	}

	initRegularBossKillerInfo = true;
}

void UT_ServerHelper::UpdateRegularBossKillerInfo(uint64_t key, RegularBossKillerInfo& info)
{
	mem::map<uint64_t, RegularBossKillerInfo>::iterator findIt = regularBossKillerInfo.find(key);
	if(findIt == regularBossKillerInfo.end())
	{
		RegularBossKillerInfo data;
		data.charName = info.charName;
		data.guildName = info.guildName;
		regularBossKillerInfo.insert(std::make_pair(key, info));
	}
	else
	{
		findIt->second.charName = info.charName;
		findIt->second.guildName = info.guildName;
	}
}

ResultType UT_ServerHelper::GetRegularBossKillerInfo(int npcCharType, int mapID, std::string& guildName, std::string& playerName)
{
	uint64_t key = LC_Helper::EncodeScore(npcCharType, mapID);
	mem::map<uint64_t, RegularBossKillerInfo>::iterator findIt = regularBossKillerInfo.find(key);
	if(findIt == regularBossKillerInfo.end())
	{
		return RE_FAIL;
	}

	guildName = findIt->second.guildName;
	playerName = findIt->second.charName;
	return RE_SUCCESS;
}

void UT_ServerHelper::SyncKillPlayerInfo(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	for(mem::map<uint64_t, std::string>::iterator it=bossKillerInfo.begin(); it!=bossKillerInfo.end(); ++it)
	{
		MG_SyncBossKillerInfo msg;
		msg.m_nKey = it->first;
		msg.m_nKillerName = it->second;
		pkPlayer->SendMsgToClient(MGPT_SYNCBOSSKILLERINFO, &msg);
	}
}

void UT_ServerHelper::UpdateKillPlayerInfo(int mapID, int npcCharID, LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	std::string name = TPSTR2STDSTR(pkPlayer->GetOwnerCharName());
	uint64_t key = LC_Helper::EncodeScore(mapID, npcCharID);
	mem::map<uint64_t, std::string>::iterator findIt = bossKillerInfo.find(key);
	if(findIt == bossKillerInfo.end())
	{
		bossKillerInfo.insert(std::make_pair(key, name));
	}
	else
	{
		findIt->second = name;
	}

	MG_SyncBossKillerInfo msg;
	msg.m_nKey = key;
	msg.m_nKillerName = name;
	BroadcastMsgToClient(MGPT_SYNCBOSSKILLERINFO, &msg, false, UT_ServerHelper::GetServerId());
}

void UT_ServerHelper::ReqPlayerOfflineToDb(OfflinePlayerFromDBType eType,std::vector<uint64_t>& vecId,uint32_t uStoryId,uint32_t uScore,uint32_t uCacheId,uint32_t userId ,uint64_t uInstance)
{
	MG_GG_REQ_OfflinePlayerFromDB reqMsg;
	reqMsg.m_eType = eType;
	//std::string vecIdStr;
	//std::stringstream ssIds;
	//ssIds <<" [ ";
	for(uint32_t i = 0; i < vecId.size();++i)
	{
		reqMsg.m_CharIDs.push_back(vecId[i]);
		//ssIds <<" id :" <<vecId[i];
		//ssIds <<" | ";
	}
	//ssIds <<" ]";
	
	reqMsg.m_uStroyId = uStoryId;
	reqMsg.m_uScore = uScore;
	reqMsg.m_uRatio = GetGlobalSetting.RobScoreRatio;
	reqMsg.m_uNowTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	reqMsg.m_uCacheId = uCacheId;
	reqMsg.m_uConfigRobbedNum = GetGlobalSetting.ConfigRobbedNum;

	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "ReqPlayerOfflineToDb eType:%d, uStoryId:%d, uScore:%d,uCacheId:%d, m_uRatio:%d , ids : %s \n",eType, uStoryId, uScore,uCacheId, GetGlobalSetting.RobScoreRatio,ssIds.str().c_str());
	bool ret = SendMsgToGameDB(MGPT_GG_REQ_OFFLINE_PLAYER_FROM_DB, &reqMsg, userId, uInstance);
	if(ret && uCacheId != 0)
	{
		UT_ServerHelper::SetSendRewardSendState(uCacheId,true,false);
		//UT_ServerHelper::SetEraseRewardSendState(uCacheId);
	}
	else if(false == ret && uCacheId != 0)
	{
		UT_ServerHelper::SetSendRewardSendState(uCacheId,false,true);
		UT_ServerHelper::SetSendRewardSendState(uCacheId,false,false);
	}
}

void UT_ServerHelper::ReqGuildFightInfoToSociety(UpdateGuildFightType eType,uint64_t msgUniqueKey,uint64_t uId,bool bAdd,int32_t uTowerId,uint32_t uCacheId,std::map<uint32_t, uint32_t>& m_score)
{
	//std::stringstream logStr;
	//logStr << "  ReqGuildFightInfoToSociety eType = " << eType <<" msgUniqueKey = " <<msgUniqueKey;
	//logStr << "  uId = " << uId <<" bAdd = " <<bAdd<<" uTowerId = " <<uTowerId<<" uCacheId = " <<uCacheId;
	MG_SS_REQ_Guild_Fight_Watch_Tower kSGMsg;
	kSGMsg.m_type = eType;
	kSGMsg.m_msgUniqueKey = msgUniqueKey;
	kSGMsg.m_uId = uId;
	kSGMsg.m_bAdd = bAdd;
	kSGMsg.m_uTowerId = uTowerId;
	kSGMsg.m_uCacheId = uCacheId;
	
	for (std::map<uint32_t, uint32_t>::iterator itc = m_score.begin(); itc != m_score.end();++itc)
	{
		kSGMsg.m_score[itc->first] = itc->second;
		//logStr <<" id =  " <<itc->first;
		//logStr <<" score =  " <<itc->second;
	}
	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s \n",logStr.str().c_str());
	bool ret = SendMsgToSociety(MGPT_GGU_REQ_GUILD_FIGHT_WATCH_TOWER, &kSGMsg);
	if(ret && uCacheId != 0)
		UT_ServerHelper::SetSendRewardSendState(uCacheId,true,true);
}

void UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType eType,int towerID, uint64_t guildID, int times,int nNextTimeStamp,bool bAddTowerId)
{
	if(GetGameServerApp()->IsNodeType_Global() == false)
		return;
	if(guildID <= 0 || times < 1)
	{
		return;
	}
	//std::stringstream logStr;
	//logStr <<" AddSendRewardCache " ;
	uint32_t autoNum = ++autoRewardNum;
	if(autoNum == (uint32_t)-1)
		autoNum = 0;
	RewardCache data;
	data.eType = eType;
	data.nTowerId = towerID;
	data.nGuildId = guildID;
	data.nNextTimeStamp = nNextTimeStamp;
	data.nTimes = times;
	data.bAddTowerId = bAddTowerId;
	RewardCacheMap.insert(std::make_pair(autoNum, data));
	//logStr<<" autoNum = "<<autoNum<<" eType = " <<data.eType<<" nTowerId = "<<data.nTowerId <<" nGuildId = " <<data.nGuildId <<" nNextTimeStamp = "<< data.nNextTimeStamp<<" nTimes = " <<data.nTimes<<" bAddTowerId = "<<data.bAddTowerId;
	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s \n",logStr.str().c_str());
}

void UT_ServerHelper::ApplySendReward()
{
	if(false == GetGameServerApp()->IsNodeType_Global()||false == GetGameServerApp()->IsServerConnect(SERVER_TYPE_SOCIETY))
		return;
	if(!gbOpenFlag)
		return;
	StringType cSplit(":");
	std::map<uint32_t, uint32_t> m_score;
	CF_BestGuildBossInfo* pkGuildBossInfo = CF_BestGuildBossInfo::GetSingletonPtr();
	mem::vector<int64_t> vecInt(2);
	for(mem::map<uint32_t, RewardCache>::iterator it=RewardCacheMap.begin(); it!=RewardCacheMap.end(); ++it)
	{
		if(false == it->second.bSendToSociety && false == it->second.bSendToDB)
		{
			uint32_t uTowerId = it->second.nTowerId;
			CF_BestGuildBossInfo::DataEntry* pkData = pkGuildBossInfo->GetEntryPtr(uTowerId);
			if(!pkData)
				continue;
			if(it->second.eType == GuildLoadRewardCacheType::GLRCT_BUFF)
			{
				if(it->second.nGuildId > 0)
					UT_ServerHelper::ReqGuildFightInfoToSociety(UpdateGuildFightType::UGFT_BUFF,it->second.nNextTimeStamp, it->second.nGuildId, it->second.bAddTowerId,it->second.nTowerId,it->first,m_score);
			}
			else if(it->second.eType == GuildLoadRewardCacheType::GLRCT_SCORE)
			{
				vecInt.clear();
				LC_Helper::SpliteString2VecInt(pkData->_iScore,vecInt,cSplit);
				if(vecInt.size() != 2)
					continue;
				int times = it->second.nTimes;
				UT_ServerHelper::ReqGuildFightInfoToSociety(UpdateGuildFightType::UGFT_GIVE_ALL_MEMBER_POINT,vecInt[1], it->second.nGuildId, false,vecInt[0]*times,it->first,m_score);	
			}
		}
	}
}

void UT_ServerHelper::EraseSendRewardCache()
{
	mem::map<uint32_t, RewardCache>::iterator it=RewardCacheMap.begin();
	while(it != RewardCacheMap.end())
	{
		if(true == it->second.bErase)
		{
			it = RewardCacheMap.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void UT_ServerHelper::SetSendRewardSendState(int id,bool bStat, bool bSociety)
{
	mem::map<uint32_t, RewardCache>::iterator findIt = RewardCacheMap.find(id);
	if(findIt != RewardCacheMap.end())
	{
		if(bSociety)
			findIt->second.bSendToSociety = bStat;
		else
			findIt->second.bSendToDB = bStat;
	}
}

void UT_ServerHelper::SetEraseRewardSendState(int id)
{
	mem::map<uint32_t, RewardCache>::iterator findIt = RewardCacheMap.find(id);
	if(findIt != RewardCacheMap.end())
		findIt->second.bErase = true;
}

void UT_ServerHelper::SetKillBossInfo(const StringType& killerName, int bossID, int mapResID)
{
	killBossInfo.killerName = killerName;
	killBossInfo.bossID = bossID;
	killBossInfo.mapResID = mapResID;
}

void UT_ServerHelper::GetKillBossInfo(StringType& killerName, int& bossID, int& mapResID)
{
	killerName = killBossInfo.killerName;
	bossID = killBossInfo.bossID;
	mapResID = killBossInfo.mapResID;
}

void UT_ServerHelper::ClearKillBossInfo()
{
	killBossInfo.clear();
}

bool UT_ServerHelper::ReqRankToDbSociety(GameToDbSocietyRankType eType,mem::vector<uint64_t>& vecId,std::vector<int>& vecInId,uint32_t uRankId,uint32_t uStartTime ,uint32_t uEndTime)
{
	bool retSociety = false;
	bool retDb = false;
	//std::stringstream logStr;
	//logStr << "  ReqRankToDbSociety eType = " << eType <<" uId = " <<uRankId;
	//logStr << "  uStartTime = " << uStartTime <<" uEndTime = " <<uEndTime;
	if(GTDSRT_RankClear == eType)
	{
		MG_GG_REQ_NotifyToSociety msgSociety;
		msgSociety.m_eType = eType;
		msgSociety.m_uRankId = uRankId;
		retSociety = SendMsgToSociety(MGPT_GG_REQ_NOTIFY_TO_SOCIETY, &msgSociety);
	}
	
	MG_GG_REQ_NotifyToDB msgDb;
	msgDb.m_eType = eType;
	msgDb.m_uRankId = uRankId;
	msgDb.m_uStartTime = uStartTime;
	msgDb.m_uEndTime = uEndTime;
	//logStr <<" [ ";
	for(uint32_t i = 0; i < vecInId.size();++i)
	{
		msgDb.m_VectorId.push_back(vecInId[i]);
		//logStr <<" id :" <<vecInId[i];
		//logStr <<" | ";
	}
	//logStr <<" ]";
	//logStr <<" [ ";
	for(uint32_t i = 0; i < vecId.size();++i)
	{
		msgDb.m_CharIDs.push_back(vecId[i]);
		//logStr <<" id :" <<vecId[i];
		//logStr <<" | "; 
	}
	//logStr <<" ]";
	retDb = SendMsgToGameDB(MGPT_GG_REQ_NOTIFY_TO_DB, &msgDb);
	
	//不要重新定义协议了，直接用MG_GG_REQ_NotifyToDB
	if(GTDSRT_RankOpenTime == eType)
	{
		SendMsgToSociety(MGPT_GG_REQ_NOTIFY_TO_DB, &msgDb);
	}

	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_INFO, "%s \n",logStr.str().c_str());
	if(GTDSRT_RankClear == eType)
	{
		return retDb && retSociety;
	}
	return retDb;
}

static int32_t ip_addr_to_string(uint32_t ip, char* ip_buff, size_t buff_size)
{
	// 192.168.111.222，格式长度为15
	if (NULL == ip_buff || buff_size == 0 || buff_size < 16)
	{
		return -1;
	}
	ip = htonl(ip);
	unsigned char* src = (unsigned char*)&ip;
	const char* fmt = "%u.%u.%u.%u";
	sprintf(ip_buff, fmt, src[0], src[1], src[2], src[3]);
	return 0;
}

void UT_ServerHelper::MergeItemList(const Utility::UT_SIMDataList& srcItemList, Utility::UT_SIMDataList& mergedItemList)
{
	mergedItemList.reserve(srcItemList.size());
	for(uint32_t i=0; i<srcItemList.size(); ++i)
	{
		bool tag = false;
		for(uint32_t j=0; j<mergedItemList.size(); ++j)
		{
			//id一致，绑定类型一致，则数量的堆叠
			if(mergedItemList[j].IID()==srcItemList[i].IID() && mergedItemList[j].ItemBindMode()==srcItemList[i].ItemBindMode())
			{
				mergedItemList[j].ItemCount(mergedItemList[j].ItemCount()+srcItemList[i].ItemCount());
				tag = true;
				break;
			}
		}

		if(!tag)
		{
			mergedItemList.push_back(srcItemList[i]);
		}
	}
}

bool UT_ServerHelper::CheckDoReward(int32_t IID, LC_ServerPlayer* pkPlayer)
{
	CF_ACTSubordinateRewardA::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTSubordinateRewardA>(IID);
	if(pkData == NULL)
	{
		return false;
	}

	int controllerLevel = pkPlayer->GetCommanderLevel();
	uint64_t vehicleCombatScore = pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_VEHICLE);//坐骑战斗力
	uint64_t devaeyeCombatScore = pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_DEVA_EYE);//宠物战斗力
	uint64_t wingCombatScore = pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_MUHUN);//背饰战斗力
	uint64_t petCombatScore = pkPlayer->GetTotalPetCombatScore();//玩具总战力	
	int lotteryCount = pkPlayer->GetLotteryTotalCount();//寻宝总次数

	//高级奖励，检测所有普通是否已经达标
	if(1 == pkData->_bHighReward)
	{
		CF_ACTSubordinateRewardA::DataEntryMapExternIterator kIter= CF_ACTSubordinateRewardA::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			const CF_ACTSubordinateRewardA::DataEntry* pkEntry = kIter.PeekNextValuePtr();
			if(0 == pkEntry->_bHighReward)
			{
				uint64_t value = pkEntry->_iLevel;
				switch(pkEntry->_iType)
				{
				case Rank_Type_Level:			//等级
					if(value > controllerLevel)
					{
						//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ACTSubordinateRewardA Rank_Type_Level error id:%d, condition:%d, value:%d\n", pkEntry->_iID, value, controllerLevel);
						return false;
					}
					break;
				case Rank_Type_Vehicle:			//坐骑
					if(value > vehicleCombatScore)
					{
						//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ACTSubordinateRewardA Rank_Type_Vehicle error id:%d, condition:%d, value:%d\n", pkEntry->_iID, value, vehicleCombatScore);
						return false;
					}
					break;
				case Rank_Type_Devaeye:			//宠物
					if(value > devaeyeCombatScore)
					{
						//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ACTSubordinateRewardA Rank_Type_Devaeye error id:%d, condition:%d, value:%d\n", pkEntry->_iID, value, devaeyeCombatScore);
						return false;
					}
					break;
				case Rank_Type_Wing:			//神翼
					if(value > wingCombatScore)
					{
						//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ACTSubordinateRewardA Rank_Type_Wing error id:%d, condition:%d, value:%d\n", pkEntry->_iID, value, wingCombatScore);
						return false;
					}
					break;
				case Rank_Type_Pet:
					if(value > petCombatScore)
					{
						//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ACTSubordinateRewardA Rank_Type_Pet error id:%d, condition:%d, value:%d\n", pkEntry->_iID, value, petCombatScore);
						return false;
					}
					break;
				default:
					{
						//GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ACTSubordinateRewardA UndefinedType error id:%d, typeID:%d\n", pkEntry->_iID, pkEntry->_iType);
					}
					return false;
				}
			}
		}
	}
	return true;
}

bool UT_ServerHelper::AddOnLinePlayer(LC_ServerPlayer* player)
{
	GameServerApp* pServerApp =  GetGameServerApp();

	Utility::Redis_Handler* pkRedisHandler = pServerApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return false;
	}
	Utility::Redis_Channel& kRedisChannel = pServerApp->GetRedisChannel();

	int32_t ret = 0;
	int32_t nServerID = player->GetCitizenship();
	char keyRedis_Snapshot[512] = {0};
	char szIP[64] = { '\0' };
	ip_addr_to_string(player->GetLoginIP(), szIP, sizeof(szIP));
	_role_data role_data;
	uint32_t uUserId = player->GetUserID();
	std::string strUserId = boost::lexical_cast<std::string>( uUserId );
	role_data.account_id = strUserId;
	role_data.level = player->GetLevel();
	role_data.role_name = player->GetOwnerCharName_utf8().c_str();
	//role_data.role_id = player->Get
	object_id_type nCharId = player->GetInstanceCharID();
	role_data.role_id = boost::lexical_cast<std::string>( nCharId );
	role_data.ip = szIP;

	std::stringstream strSnapshotStream;
	boost::ajson::save_to_buff(role_data, strSnapshotStream);

	sprintf_s(keyRedis_Snapshot, sizeof(keyRedis_Snapshot), "%s_%d", pServerApp->GetGamePlayerOnlineSnapshot().c_str(), nServerID);
	ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::hset, test_reply, keyRedis_Snapshot, strUserId, strSnapshotStream.str());
	return true;
}

bool UT_ServerHelper::RemoveOffLinePlayer(LC_ServerPlayer* player)
{
	GameServerApp* pServerApp =  GetGameServerApp();
	
	Utility::Redis_Handler* pkRedisHandler = pServerApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return false;
	}
	int ret = 0;
	Utility::Redis_Channel& kRedisChannel = pServerApp->GetRedisChannel();
	uint32_t uUserId = player->GetUserID();
	std::string strUserId = boost::lexical_cast<std::string>( uUserId );
	int32_t nServerID = player->GetCitizenship();
	char keyRedis_Snapshot[512] = {0};

	sprintf_s(keyRedis_Snapshot, sizeof(keyRedis_Snapshot), "%s_%d", pServerApp->GetGamePlayerOnlineSnapshot().c_str(), nServerID);
	ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::hdel, test_reply, keyRedis_Snapshot, strUserId );

	return true;
}

bool UT_ServerHelper::ResetOnLinePlayer()
{
	GameServerApp* pServerApp =  GetGameServerApp();
	
	Utility::Redis_Handler* pkRedisHandler = pServerApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return false;
	}
	Utility::Redis_Channel& kRedisChannel = pServerApp->GetRedisChannel();

	int32_t ret = 0;
	char keyRedis_Snapshot[512] = {0};

	sprintf_s(keyRedis_Snapshot, sizeof(keyRedis_Snapshot), "%s_%d", pServerApp->GetGamePlayerOnlineSnapshot().c_str(), pServerApp->GetServerInfo().m_iServerID);
	ASYNC_REDIS_REQUEST_NCB(kRedisChannel, &redispp::Connection::del, keyRedis_Snapshot);//, strUserName, strSnapshotStream.str());
	return true;
}

int32_t UT_ServerHelper::GetActivityEndTime(int scheduleID)
{
	static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleEntry* pkScheduleEntry = pkScheduleMgr->GetSchedule(GetGameServerID(), scheduleID);
	if (NULL == pkScheduleEntry)
	{
		return 0;
	}

	return pkScheduleEntry->CurrentEndTime()/1000;
}

bool UT_ServerHelper::CheckActivityVaild(int scheduleID)
{
	if(0 == scheduleID)
	{
		return true;
	}
	static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
	bool tag = pkScheduleMgr->IsScheduleValid(GetGameServerID(), scheduleID);
	return tag;
}

bool UT_ServerHelper::SpawnNPC(LC_ServerPlayer* pkPlayer, int32_t npcID)
{
	if(NULL == pkPlayer)
	{
		return false;
	}

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager     = ENGINE_GET_NPC_MANAGER();
	LC_LogicIDManager* pkLogicIDManager = LC_LogicIDManager::GetSingletonPtr();

	Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
	int nMapID = pkPlayer->GetMapLogicID();

	//获得NPC ID号
	object_id_type lNPCID = pkLogicIDManager->RequestLogicID(LOT_NPC);

	//创建NPC 对象
	LC_NPCBase* pkNPC = pkNPCManager->CreateNPC(lNPCID, npcID);
	if (pkNPC == NULL)
	{
		return false;
	}
	kCurrentLocation.x += UT_MathBase::RandInRangeInt( -100, 100);
	kCurrentLocation.y += UT_MathBase::RandInRangeInt( -100, 100);
	pkNPC->InitLogicData();
	pkNPC->SetProperty_WanderRadius(0);
	pkNPC->SetCurrentLocation(kCurrentLocation);
	pkNPC->SetTargetLocation(kCurrentLocation);
	pkNPC->SetSpawnLocation(kCurrentLocation);
	pkNPC->SetSpawnZRotAngle(180);
	pkNPC->SetSpawnPointID(0);
	pkNPC->SetSurvivalStartTime(GET_PROCESS_TIME);
	pkNPC->SetSpawnTime(GET_PROCESS_TIME);

	//将NPC 加入世界
	pkWorldManager->AddObjectToMap(nMapID, pkNPC);
	return true;
}

ResultType UT_ServerHelper::GetDisassembleItem(LC_ServerPlayer* pkPlayer, int32_t itemID, Utility::UT_SIMDataList& kItems, mem::map<int32_t, int32_t>& itemInfo)
{
	if(NULL == pkPlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem playerEmpry!");
		return RE_FAIL;
	}

	kItems.clear();
	const unique_id_impl& instance = pkPlayer->GetInstance();
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(itemID);
	if (!pkItem)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem itemID:[%d] error!c_cid:%d, c_citizenship:%d, c_uid:%d", 
			itemID, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return RE_FAIL;
	}

	int32_t lDisassembleUseFuncID = pkItem->GetDisassembleUseFuncID(0);
	if (lDisassembleUseFuncID <= 0)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem lDisassembleUseFuncID:[%d] error! c_cid:%d, c_citizenship:%d, c_uid:%d", 
			lDisassembleUseFuncID, instance.detail(), instance.catagory(), pkPlayer->GetUserID());		
		return RE_GUILDPRESENT_ERROR_7;
	}

	CF_ItemDisassembleUseFunc::DataEntry* pkFuncEntry =SafeGetCSVFileDataEntryPtr<CF_ItemDisassembleUseFunc>(lDisassembleUseFuncID);
	if (pkFuncEntry == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem CF_ItemDisassembleUseFunc:[%d] empty! c_cid:%d, c_citizenship:%d, c_uid:%d",
			lDisassembleUseFuncID, instance.detail(), instance.catagory(), pkPlayer->GetUserID());	
		return RE_GUILDPRESENT_ERROR_7;
	}

	int iNeedPackCount = UT_MathBase::MinInt(pkFuncEntry->_lMinPackCount, pkFuncEntry->_lBatchMaxItemCount);
	if(pkPlayer->GetPackAsset()->GetPackEmptyEntryCount(PACK_TYPE_BASIC) < iNeedPackCount)
	{
		return RE_ITEM_PACKAGE_FULL;
	}

	LC_ServerItemFlopManager* pkServerItemFlopManager = LC_ServerItemFlopManager::GetSingletonPtr();

	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	if (NULL == pkPackAsset)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem pkServerItemFlopManager empty! c_cid:%d, c_citizenship:%d, c_uid:%d",
			instance.detail(), instance.catagory(), pkPlayer->GetUserID());	
		return RE_FAIL;
	}

	LC_ItemFlopInfoList kItemFlopList(1);
	int iPackType = PACK_TYPE_UNDEFINE;

	int iChoice = 0;
	if(!pkServerItemFlopManager->CheckChoice(FTP_ITEM_DISASSEMBLE, lDisassembleUseFuncID, pkPlayer->GetWeaponAttribute(), iChoice))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem CheckChoice error! lDisassembleUseFuncID:%d, iChoice:%d, c_cid:%d, c_citizenship:%d, c_uid:%d", 
			lDisassembleUseFuncID, iChoice, instance.detail(), instance.catagory(), pkPlayer->GetUserID());	
		return RE_GUILDPRESENT_ERROR_7;
	}

	GameLogic::LC_RewardItemsExtend kExtend;
	kExtend.SetBind(0);
	kExtend.SetChoice(iChoice);
	pkServerItemFlopManager->GenerateItemFlopInfoOnDisassemble(kItemFlopList, pkPlayer, lDisassembleUseFuncID, &kExtend);

	int iItemFlopInfoCount = kItemFlopList.GetItemFlopInfoCount();
	if (iItemFlopInfoCount <= 0 || iItemFlopInfoCount > 1)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem iItemFlopInfoCount:[%d] error! c_cid:%d, c_citizenship:%d, c_uid:%d", 
			iItemFlopInfoCount, instance.detail(), instance.catagory(), pkPlayer->GetUserID());	
		return RE_GUILDPRESENT_ERROR_7;
	}

	LC_ItemFlopInfo* pkItemFlopInfo = kItemFlopList.GetItemFlopInfoByIndex(0);
	if (NULL == pkItemFlopInfo)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem pkItemFlopInfo empty! c_cid:%d, c_citizenship:%d, c_uid:%d",
			instance.detail(), instance.catagory(), pkPlayer->GetUserID());	
		return RE_GUILDPRESENT_ERROR_7;
	}

	LC_BackPackEntryList& rkBackEntryList = pkItemFlopInfo->GetBackPackEntryList();
	if (rkBackEntryList.empty())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetDisassembleItem rkBackEntryList empty! c_cid:%d, c_citizenship:%d, c_uid:%d",
			instance.detail(), instance.catagory(), pkPlayer->GetUserID());	
		return RE_GUILDPRESENT_ERROR_7;
	}

	for (LC_BackPackEntryListIter IterBe = rkBackEntryList.begin(); IterBe!=rkBackEntryList.end(); ++IterBe)
	{
		LC_BackPackEntry& rkBackPackEntry = *IterBe;

		int32_t itemID = rkBackPackEntry.GetBackPackValueByType(LPSFET_TYPE_ID);
		int32_t itemNum = rkBackPackEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
		int32_t bindMode = rkBackPackEntry.GetBackPackValueByType(LPSFET_BINDMODE);
		if(itemNum > 0)
		{
			UT_SIMDataInfo data;
			data.SetItemInfo(itemID, itemNum, bindMode);
			kItems.push_back(data);

			mem::map<int32_t, int32_t>::iterator findIt = itemInfo.find(itemID);
			if(findIt == itemInfo.end())
			{
				itemInfo[itemID] = itemNum;
			}
			else
			{
				findIt->second += itemNum;
			}
		}
	}

	return RE_SUCCESS;
}

void UT_ServerHelper::LoadMapIDAndMirrorTaskID()
{
	CF_TaskList::DataEntryMapExternIterator kIter = CF_TaskList::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_TaskList::DataEntry* pkTaskData = kIter.PeekNextValuePtr();

		int taskID = pkTaskData->_iID;
		int descID = LC_Helper::GetWorldObjecDescID(pkTaskData->_AcceptDescription);
		CF_WorldObjectDesc::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldObjectDesc>(descID);
		if(NULL!=pkData && 25==pkData->_lType)
		{
			//相位副本
			mapIDAndMirrorTaskIDRecords.insert(std::make_pair(pkData->_lLogicID, taskID));
		}
	}
}

const mem::map<int, int>& UT_ServerHelper::GetMapIDAndMirrorTaskIDRecords()
{
	return mapIDAndMirrorTaskIDRecords;
}

int UT_ServerHelper::GetMirrorTaskIDByMapID(int mapID)
{
	mem::map<int, int>::iterator findIt = mapIDAndMirrorTaskIDRecords.find(mapID);
	if(findIt == mapIDAndMirrorTaskIDRecords.end())
	{
		return -1;
	}
	return findIt->second;
}

bool UT_ServerHelper::CheckNeedUpdate(int32_t type, uint64_t oldValue, uint64_t newValue)
{
	bool res = false;
	switch(type)
	{
	case PlayerDamageUpdateType_IncrementalUpdate:
		res = true;
		break;
	case PlayerDamageUpdateType_GreaterUpdate:
		res = newValue>oldValue ? true : false;
		break;
	default:
		break;
	}

	return res;
}

uint64_t UT_ServerHelper::GetNewDamageData(int32_t type, uint64_t oldValue, uint64_t newValue)
{
	bool tag = UT_ServerHelper::CheckNeedUpdate(type, oldValue, newValue);
	if(!tag)
	{
		return 0;
	}

	uint64_t newData = 0;
	switch(type)
	{
	case PlayerDamageUpdateType_IncrementalUpdate:
		newData = oldValue+newValue;
		break;
	case PlayerDamageUpdateType_GreaterUpdate:
		newData = newValue;
		break;
	default:
		break;
	}

	return newData;
}

uint32_t UT_ServerHelper::GetExportRobotMaxNum(int robotType)
{
	switch(robotType)
	{
	case Robot_Type_Challenge:
		return MAX_SOLO_DULE_ROBOT_SIZE;
	case Robot_Type_Firmament:
		return MAX_SKY_SOLO_ROBOT_SIZE;
	}
	return 0;
}

char* UT_ServerHelper::GetRedisNameSpace(char* a, int b)
{
	char buff[256];
	itoa(b, buff, 10);

	int len = strlen(a);
	for(int i=0; i<strlen(buff); ++i)
	{
		a[len++] = buff[i];
	}
	return a;
}

int32_t UT_ServerHelper::GetFactionDefServer()
{
	int32_t iDefServerId = INVALID_SERVER_ID;
	LC_ServerLogicManager* logicMgr = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = logicMgr->GetGuildWarAssert();
	if(NULL != pWorldGuildWarAsset)
	{
		iDefServerId = pWorldGuildWarAsset->GetGlobalPowerServerID();
	}

	if (INVALID_SERVER_ID != iDefServerId)
	{
		return iDefServerId;
	}
	GameServerManager* pServerMgr = GetGameServerApp()->GetServerManager();
	return pServerMgr->GetTopAvgPowerServer();
}

bool UT_ServerHelper::RedisCreateData( const char* szNameSpace, mem::vector<uint64_t>& nKeys, bool bGlobalServer )
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisCreateData---EntryServeState init fail!");
		return false;
	}

	std::string sKey = GetRedisStr(szNameSpace, bGlobalServer);
	std::string strDetailKey= GenDetailKeyWithoutData(nKeys);
	std::string strValue = "0";

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (!kCharReply.result())
	{
		pkRedisConn->hset(sKey, strDetailKey, strValue);
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisCreateData_Fail ret:%d, Key:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisGetData_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, std::string& strResult, bool bGlobalServer )
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_String---EntryServeState init fail！");
		return false;
	}

	std::string sKey = GetRedisStr(szNameSpace, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);
	//std::string strValue = "";

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (kCharReply.result())
	{
		strResult = kCharReply;
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_String ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}

int UT_ServerHelper::RedisGetData_StringRange( const char* szNameSpace, mem::map<uint64_t, std::string>& result, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_StringRange---EntryServeState init fail！%d\n ",bGlobalServer);
		return false;
	}
	std::string sKey = "";
	if (bGlobalServer == false)
		sKey += pkApp->GetRedisEntryServerPredix();
	sKey += szNameSpace;

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	uint64_t nInputScore = 0;
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->hgetAll(sKey);

	std::string str;
	uint64_t time = 0;
	for ( int i = 0;multiReply.next(&str); i++ )
	{
		if (str.empty())
		{
			continue;
		}

		if(i%2 == 0)
		{
			time = _atoi64(str.c_str());
		}
		else
		{
			result.insert(std::make_pair(time, str));
		}
	}

	UT_REDIS_CMD_END(ret, pkRedisHandler);
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_StringRange ret:%d, sKey:%s\n", ret, sKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisCopy_String(const char* szNameSpace, const char* destNameSpace, bool bGlobalServer)
{
	mem::map<uint64_t, std::string> result;
	RedisGetData_StringRange(szNameSpace, result, bGlobalServer);
	for(mem::map<uint64_t, std::string>::iterator it=result.begin(); it!=result.end(); ++it)
	{
		mem::vector<uint64_t> key;
		key.push_back(it->first);
		RedisSetData_String(destNameSpace, key, it->second, bGlobalServer, true);
	}
	return true;
}

bool UT_ServerHelper::RedisSetData_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, const std::string& strInput, bool bGlobalServer , bool bForceSet )
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisSetData_String---EntryServeState init fail！");
		return false;
	}

	std::string sKey = GetRedisStr(szNameSpace, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if(bForceSet || kCharReply.result())
	{
		pkRedisConn->hset(sKey, strDetailKey, strInput);
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisSetData_String ret:%d, sKey:%s, strDetailKey:%s, strValue:%s\n", ret, sKey.c_str(), strDetailKey.c_str(), strInput.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisEraseData_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisEraseData_String---EntryServeState init fail！");
		return false;
	}

	std::string sKey = GetRedisStr(szNameSpace, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if(kCharReply.result())
	{
		pkRedisConn->hdel(sKey, strDetailKey);
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisEraseData_String ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisGetData_Int( const char* szNameSpace, mem::vector<uint64_t>& nKeys, int& nResult, bool bGlobalServer )
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_Int---EntryServeState init fail！");
		return false;
	}

	std::string sKey = GetRedisStr(szNameSpace, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);
	
	std::string strValue = "";

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (kCharReply.result())
	{
		strValue = kCharReply;
		nResult = atoi(strValue.c_str());
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_Int ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisSetData_Int( const char* szNameSpace, mem::vector<uint64_t>& nKeys, int nInput, bool bGlobalServer )
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisSetData_Int---EntryServeState init fail！");
		return false;
	}

	std::string sKey = GetRedisStr(szNameSpace, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	char strValue[64] = { '\0' };
	sprintf( strValue, "%d", nInput );

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::BoolReply boolReply = pkRedisConn->hset(sKey, strDetailKey, strValue);
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_ERROR_FILELINE, "RedisSetData_Int ret:%d, sKey:%s, strDetailKey:%s, strValue:%s\n", ret, sKey.c_str(), strDetailKey.c_str(), strValue);
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisRemoveData_Int(const char* szNameSpace, mem::vector<uint64_t>& nKeys, bool bGlobalServer)
{
	int ret = 0;
	bool bResult = true;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRemoveData_Int---EntryServeState init fail！");
		return false;
	}

	std::string sKey = GetRedisStr(szNameSpace, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::BoolReply nReply = pkRedisConn->hdel(sKey, strDetailKey);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRemoveData_Int ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}

int UT_ServerHelper::RedisGetData_IntRange( const char* szNameSpace, mem::map<uint64_t, uint64_t>& result, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_IntRange---EntryServeState init fail！");
		return false;
	}
	std::string sKey = "";
	if (bGlobalServer == false)
		sKey += pkApp->GetRedisEntryServerPredix();
	sKey += szNameSpace;

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	uint64_t nInputScore = 0;
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->hgetAll(sKey);

	std::string str;
	uint64_t key = 0;
	uint64_t value = 0;
	for ( int i = 0;multiReply.next(&str); i++ )
	{
		if (str.empty())
		{
			continue;
		}

		if(i%2 == 0)
		{
			key = _atoi64(str.c_str());
		}
		else
		{
			value = _atoi64(str.c_str());
			result.insert(std::make_pair(key, value));
		}
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_IntRange ret:%d, sKey:%s\n", ret, sKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisList_AddInt(const char* szNameSpace, uint64_t nInput, bool bGlobalServer)
{
	int ret = 0;
	bool bResult = true;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisList_AddInt---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer);

	std::string value = "";
	std::stringstream tempStream;
	tempStream << nInput;
	value += tempStream.str();

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::IntReply nReply = pkRedisConn->rpush(sKey, value);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisList_AddInt ret:%d, sKey:%s, value:%s\n", ret, sKey.c_str(), value.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisList_RemoveInt(const char* szNameSpace, uint64_t nInput, bool bGlobalServer)
{
	int ret = 0;
	bool bResult = true;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisList_RemoveInt---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer);

	std::string value = "";
	std::stringstream tempStream;
	tempStream << nInput;
	value += tempStream.str();

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::IntReply nReply = pkRedisConn->lrem(sKey, 0, value);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisList_RemoveInt ret:%d, sKey:%s, value:%s\n", ret, sKey.c_str(), value.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisList_GetRangeInt(const char* szNameSpace, mem::vector<uint64_t>& vecRangeData, bool bGlobalServer)
{
	int ret = 0;
	bool bResult = true;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisList_GetRangeInt---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->lrange(sKey, 0, -1);

	std::string str;
	uint64_t data = 0;
	for (int i = 0;multiReply.next(&str); ++i)
	{
		if (str.empty())
		{
			continue;
		}

		data = _atoi64(str.c_str());
		vecRangeData.push_back(data);		
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisList_GetRangeInt ret:%d, sKey:%s\n", ret, sKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisGetData_Int_EntryServerID( const char* szNameSpace, mem::vector<uint64_t>& nKeys, int& nResult)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_Int_EntryServerID---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr_EntryID(szNameSpace);

	std::string strDetailKey = "";
	char buff[64] = {'\0'};
	for ( int i = 0; i < nKeys.size(); i++ )
	{
		itoa( nKeys[i], buff, 10 );
		strDetailKey += buff;
	}

	std::string strValue = "";

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (kCharReply.result())
	{
		strValue = kCharReply;
		nResult = atoi(strValue.c_str());
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_Int_EntryServerID ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}
bool UT_ServerHelper::RedisGetData_Int_EntryServerID_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, std::string& strResult)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_Int_EntryServerID_String---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr_EntryID(szNameSpace);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (kCharReply.result())
	{
		strResult = kCharReply;
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_Int_EntryServerID_String ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}


bool UT_ServerHelper::RedisSetData_Int_EntryServerID_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, const std::string& strInput)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisSetData_Int_EntryServerID_String---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr_EntryID(szNameSpace);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	pkRedisConn->hset(sKey, strDetailKey, strInput);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisSetData_Int_EntryServerID_String ret:%d, sKey:%s, strDetailKey:%s, strInput:%s\n", ret, sKey.c_str(), strDetailKey.c_str(), strInput.c_str());
		return false;
	}
	return  true;
}
bool UT_ServerHelper::ClearRedisData_By_EntryServerID( const char* szNameSpace)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ClearRedisData_By_EntryServerID---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr_EntryID(szNameSpace);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
		return false;

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::BoolReply nReply = pkRedisConn->del(sKey);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ClearRedisData_By_EntryServerID ret:%d, sKey:%s\n", ret, sKey.c_str());
		return false;
	}
	return  true;
}

void UT_ServerHelper::InitBestGuild()
{
	bestGuildStoryID.clear();
	CF_BestGuild::DataEntryMapExternIterator kIter = CF_BestGuild::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BestGuild::DataEntry* pkData = kIter.PeekNextValuePtr();
		bestGuildStoryID.push_back(pkData->_iGamestory);
	}
}

void UT_ServerHelper::InitRegularBoss()
{
	regularBossMapGroupInfo.clear();
	CF_RegularBoss::DataEntryMapExternIterator kIter = CF_RegularBoss::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_RegularBoss::DataEntry* pkData = kIter.PeekNextValuePtr();
		regularBossMapGroupInfo.insert(std::make_pair(pkData->_lMapResID, pkData->_iGroupID));
	}
}

void UT_ServerHelper::InitEscortReward()
{
	escortNPCVec.clear();
	escortTaskVec.clear();
	CF_EscortReward::DataEntryMapExternIterator kIter = CF_EscortReward::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_EscortReward::DataEntry* pkData = kIter.PeekNextValuePtr();
		escortNPCVec.push_back(pkData->_iNpcID);
		escortTaskVec.push_back(pkData->_TaskID);
	}
}

void UT_ServerHelper::InitBestGuildGoal()
{
	goalTypeAndID.clear();
	CF_BestGuildGoal::DataEntryMapExternIterator kIter = CF_BestGuildGoal::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BestGuildGoal::DataEntry* pkData = kIter.PeekNextValuePtr();
		mem::map<int, mem::vector<int> >::iterator findIt = goalTypeAndID.find(pkData->_Type);
		if(findIt == goalTypeAndID.end())
		{
			mem::vector<int> idVec;
			idVec.push_back(pkData->_lID);
			goalTypeAndID.insert(std::make_pair(pkData->_Type, idVec));
		}
		else
		{
			findIt->second.push_back(pkData->_lID);
		}
	}
}

void UT_ServerHelper::InitTreasurePos()
{
	playerLevel2TreasuerPos.clear();
	CF_TreasurePos::DataEntryMapExternIterator kIter6 = CF_TreasurePos::GetSingletonPtr()->GetIterator();
	for(; kIter6.HasMoreElements(); kIter6.MoveNext())
	{
		CF_TreasurePos::DataEntry* pkData = kIter6.PeekNextValuePtr();
		mem::map<int, mem::map<int, mem::vector<UT_Vec3Int> > >::iterator findIt1 = playerLevel2TreasuerPos.find(pkData->_iMaxLevel);
		if(findIt1 == playerLevel2TreasuerPos.end())
		{
			mem::map<int, mem::vector<UT_Vec3Int> > mapID2TreasuerPos;
			mem::vector<UT_Vec3Int> vecPos;
			vecPos.reserve(10);
			UT_Vec3Int pos(pkData->_iPos);

			vecPos.push_back(pos);
			mapID2TreasuerPos.insert(std::make_pair(pkData->_iMapID, vecPos));
			playerLevel2TreasuerPos.insert(std::make_pair(pkData->_iMaxLevel, mapID2TreasuerPos));
		}
		else
		{
			mem::map<int, mem::vector<UT_Vec3Int> >::iterator findIt2 = findIt1->second.find(pkData->_iMapID);
			if(findIt2 == findIt1->second.end())
			{
				mem::vector<UT_Vec3Int> vecPos;
				vecPos.reserve(10);
				UT_Vec3Int pos(pkData->_iPos);

				vecPos.push_back(pos);
				findIt1->second.insert(std::make_pair(pkData->_iMapID, vecPos));
			}
			else
			{
				UT_Vec3Int pos(pkData->_iPos);
				findIt2->second.push_back(pos);
			}
		}
	}
}

void UT_ServerHelper::InitBuddy()
{
	m_nBuddyCount = 0;
	CF_Buddy::DataEntryMapExternIterator kIter = CF_Buddy::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		++m_nBuddyCount;
	}
}

void UT_ServerHelper::InitRuneStar()
{
	RuneStarInfo data;
	CF_RuneStar::DataEntryMapExternIterator kIter = CF_RuneStar::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_RuneStar::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		data._MaxLevel = pkEntry->_MaxLevel;
		data._PropertyChange = pkEntry->_PropertyChange;
		data._consumeItems1 = pkEntry->_consumeItems1;
		data._consumeItems2 = pkEntry->_consumeItems2;

		mem::map<int, mem::map<int, RuneStarInfo> >::iterator findIt = runeStarInfo.find(pkEntry->_StarUpType);
		if(findIt == runeStarInfo.end())
		{			
			mem::map<int, RuneStarInfo> info;			
			info.insert(std::make_pair(pkEntry->_StarLevel, data));
			runeStarInfo.insert(std::make_pair(pkEntry->_StarUpType, info));
		}
		else
		{
			findIt->second.insert(std::make_pair(pkEntry->_StarLevel, data));
		}
	}
}

bool UT_ServerHelper::GetRuneCSVInfo(int starType, int starLevel, int& maxLevel, Utility::UT_SIMDataList& consumeItems, int& consumeItemID1, int& consumeItemCount, int& consumeItemID2)
{
	maxLevel = 0;
	consumeItems.clear();
	consumeItemID1 = 0;
	consumeItemID2 = 0;
	consumeItemCount = 0;
	mem::map<int, mem::map<int, RuneStarInfo> >::iterator findIt = runeStarInfo.find(starType);
	if(findIt == runeStarInfo.end())
	{
		return false;
	}

	bool tag = false;
	for(mem::map<int, RuneStarInfo>::iterator it=findIt->second.begin(); it!=findIt->second.end(); ++it)
	{
		if(it->first == starLevel)
		{
			tag = true;
			maxLevel = it->second._MaxLevel;

			consumeItems.reserve(it->second._consumeItems1.size());
			for(Utility::UT_SIMDataList::iterator it1=it->second._consumeItems1.begin(); it1!=it->second._consumeItems1.end(); ++it1)
			{
				consumeItems.push_back(*it1);
			}

			if(it->second._consumeItems2.size() == 2)
			{
				consumeItemID1 = it->second._consumeItems2[0].IID();
				consumeItemID2 = it->second._consumeItems2[1].IID();
				consumeItemCount = it->second._consumeItems2[0].ParamA();				
			}
		}
	}

	return tag;
}

void UT_ServerHelper::GetTreasureMapVec(int playerLevel, mem::map<int, mem::vector<UT_Vec3Int> >& MapVec)
{
	for(mem::map<int, mem::map<int, mem::vector<UT_Vec3Int> > >::iterator it= playerLevel2TreasuerPos.begin(); it!=playerLevel2TreasuerPos.end(); ++it)
	{
		if(it->first >= playerLevel)
		{
			MapVec = it->second;
			break;
		}
	}
}

void UT_ServerHelper::UpdateGuildMapBossState(int32_t mapID, uint64_t guildID, PS_GuildBossInfo& data, int32_t oldState)
{
	if(GetGameServerApp()->IsNodeType_Global())
	{
		int state = data.m_nState;
		if(state != oldState)
		{
			static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
			if(GuildBossState_Alive==oldState && GuildBossState_Killed == state)
			{
				//被击杀后，清除redis存储的boss状态数据
				UT_ServerHelper::DeleteGuildMapBossHP(mapID, guildID);

				LC_ServerMapLogicPlaceVector placeVec;
				bool tag = pkWorldMgr->FindGuildMapBossLogicMap(mapID, guildID, placeVec);
				GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_ERROR, "UpdateGuildMapBossState, oldState:%d, state:%d, guildID:%lld, mapID:%d, tag:%d, placeVec_size:%d\n", oldState, state, guildID, mapID, tag, placeVec.size());

				//被击杀，结算伤害发排名奖，副本结束
				for(LC_ServerMapLogicPlaceVector::iterator it=placeVec.begin(); it!=placeVec.end(); ++it)
				{
					LC_ServerMapLogic_Place* pkPlace = *it;
					if(NULL != pkPlace)
					{
						pkPlace->SettleGuildMapBoss(data.m_nBossID);
						pkPlace->SetPlaceState(PLACESTATE_PREEND);	
					}
				}
			}
			else if(GuildBossState_Alive==oldState && GuildBossState_TimeOut == state)
			{
				//时间到，未被击杀
				//时间到后，清除redis存储的boss状态数据
				UT_ServerHelper::DeleteGuildMapBossHP(mapID, guildID);

				LC_ServerMapLogicPlaceVector placeVec;
				bool tag = pkWorldMgr->FindGuildMapBossLogicMap(mapID, guildID, placeVec);
				GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_ERROR, "UpdateGuildMapBossState, oldState:%d, state:%d, guildID:%lld, mapID:%d, tag:%d, placeVec_size:%d\n", oldState, state, guildID, mapID, tag, placeVec.size());

				for(LC_ServerMapLogicPlaceVector::iterator it=placeVec.begin(); it!=placeVec.end(); ++it)
				{
					LC_ServerMapLogic_Place* pkPlace = *it;
					if(NULL != pkPlace)
					{
						//结算
						GameLogic::LC_ActorPtrList players;
						pkPlace->GetAllRealPlayer(players);
						GameLogic::LC_ActorPtrList::iterator actorBeg = players.begin();
						for(; actorBeg != players.end(); ++actorBeg)
						{
							LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*actorBeg);
							if(NULL != pkPlayer)
							{
								pkPlayer->SetAttackFlag(false);
								pkPlace->ForceUpdateDamageData(pkPlayer, false);
							}
						}

						pkPlace->SetPlaceState(PLACESTATE_END);
					}
				}
				int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN17_OPERATE, OPERATE_LOGTYPE4);
				//发参与奖
				CF_GuildTower::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GuildTower>(mapID);			
				if(NULL!=pkData && false==pkData->_sJoinReward.empty())
				{
					Utility::UT_SIMDataList itemlist;
					Utility::ParseFmtString2DataIFs(pkData->_sJoinReward, itemlist);
				
					mem::vector<uint64_t> params;
					params.reserve(2);
					params.push_back(guildID);
					params.push_back(mapID);

					mem::vector<uint64_t> allPlayers;
					UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_GUILDMAPBOSS_DAMAGE_RANK, params, allPlayers);
					for(int i=0; i<allPlayers.size(); ++i)
					{
						UT_ServerHelper::SendToMail(pkData->_iMailTitle, pkData->_iMailDesc, allPlayers[i], "", 0, itemlist, "", nLogCode);
					}
				}
			}
		}

		//存储公会秘境数据
		UT_ServerHelper::SaveGuildMapBossInfo(mapID, guildID, data);
	}
}

void UT_ServerHelper::InitGuildMapBossHP()
{
	int size = GetGlobalSetting.vecGuildMapBossRewardHPPresent.size();
	CF_GuildTower::DataEntryMapExternIterator kIter = CF_GuildTower::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_GuildTower::DataEntry* pkData = kIter.PeekNextValuePtr();
		CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(pkData->_lCharType);
		if(NULL == pkEntry)
		{
			continue;
		}

		mem::map<int, UT_ServerHelper::GuildMapBossHpInfo>::iterator findIt = guildMapBossHpInfo.find(pkData->_lMapResID);
		if(findIt != guildMapBossHpInfo.end())
		{
			continue;
		}

		mem::vector<StringType> ItemInfo;
		char str = '|';
		LC_Helper::SpliteString2MultiString(pkData->_sTotalReward, ItemInfo, str);
		if(ItemInfo.size() != size)
		{
			continue;
		}

		UT_ServerHelper::GuildMapBossHpInfo data;
		data.maxHp = pkEntry->_lMaxHP;
		data.charTypeID = pkData->_lCharType;

		for(int i=0; i<ItemInfo.size(); ++i)
		{
			UT_ServerHelper::LC_ItemCntList Reward;

			mem::vector<StringType> rewardInfo;
			char str = ';';
			LC_Helper::SpliteString2MultiString(ItemInfo[i], rewardInfo, str);
			for(int j=0; j<rewardInfo.size(); ++j)
			{
				UT_SIMDataList rewardData;
				ParseFmtString2DataIFs(rewardInfo[j], rewardData);
				for(UT_SIMDataList::iterator it=rewardData.begin(); it!=rewardData.end(); ++it)
				{
					__ItemCnt rewardData;
					rewardData.m_nId = it->IID();
					rewardData.m_nCnt = it->ParamA();
					rewardData.m_nBindMode = it->ParamB();
					Reward.push_back(rewardData);
				}		
			}

			float percent = 1 - GetGlobalSetting.vecGuildMapBossRewardHPPresent[i];
			int64_t hp = pkEntry->_lMaxHP * percent;
			data.hp2RewardInfo.insert(std::make_pair(hp, Reward));
		}

		guildMapBossHpInfo.insert(std::make_pair(pkData->_lMapResID, data));
	}
}

ResultType UT_ServerHelper::CheckAddGuildMapBossReward(int mapID, int charTypeID, int64_t oldHp, int64_t currentHp, uint64_t guildID, mem::vector<int>& rewardIndex)
{
	if(oldHp < currentHp)
	{
		return RE_FAIL;
	}

	rewardIndex.clear();
	guildMapBossHpReward.clear();
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return RE_FAIL;
	}

	LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(guildID);
	if (pkGuild == NULL)
	{
		return RE_FAIL;
	}

	mem::map<int, UT_ServerHelper::GuildMapBossHpInfo>::iterator findIt = guildMapBossHpInfo.find(mapID);
	if(findIt == guildMapBossHpInfo.end())
	{
		return RE_FAIL;
	}

	if(charTypeID != findIt->second.charTypeID)
	{
		return RE_GUILDMAP_ERROR_16;
	}

	int i=0;
	for(mem::map<int64_t, UT_ServerHelper::LC_ItemCntList, std::greater<int64_t> >::iterator it=findIt->second.hp2RewardInfo.begin(); it!=findIt->second.hp2RewardInfo.end(); ++it, ++i)
	{
		if(currentHp <= it->first)
		{
			guildMapBossHpReward.push_back(i);
		}

		if(oldHp>it->first && currentHp<=it->first)
		{
			rewardIndex.push_back(i);
		}
	}

	return guildMapBossHpReward.empty() ? RE_GUILDMAP_ERROR_17 : RE_SUCCESS;
}

bool UT_ServerHelper::GetGuildMapReward(int mapID, int rewardIndex, UT_ServerHelper::LC_ItemCntList& rewardInfo)
{
	mem::map<int, UT_ServerHelper::GuildMapBossHpInfo>::iterator findIt = guildMapBossHpInfo.find(mapID);
	if(findIt == guildMapBossHpInfo.end())
	{
		return false;
	}

	int i=0;
	for(mem::map<int64_t, UT_ServerHelper::LC_ItemCntList, std::greater<int64_t> >::iterator it=findIt->second.hp2RewardInfo.begin(); it!=findIt->second.hp2RewardInfo.end(); ++it, ++i)
	{
		if(i == rewardIndex)
		{
			rewardInfo = it->second;
			return true;
		}
	}

	return false;
}


int64_t UT_ServerHelper::GetGuildMapBossMaxHp(int mapID, int bossID)
{
	mem::map<int, UT_ServerHelper::GuildMapBossHpInfo>::iterator findIt = guildMapBossHpInfo.find(mapID);
	if(findIt == guildMapBossHpInfo.end())
	{
		return -1;
	}

	return findIt->second.maxHp;
}

void UT_ServerHelper::GetGuildMapBossReward(std::vector<int32_t>& rewardIndex)
{
	rewardIndex.clear();
	rewardIndex.reserve(guildMapBossHpReward.size());
	for(int i=0; i<guildMapBossHpReward.size(); ++i)
	{
		rewardIndex.push_back(guildMapBossHpReward[i]);
	}
}

void UT_ServerHelper::SendGuildMapBossReward(int mapID, uint64_t guildID, const mem::vector<int>& rewardIndex)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(guildID);
	if(pkGuildInfo == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SendGuildMapBossReward guildEmpty! guildID:%lld\n", guildID);
		return;
	}

	for(int i=0; i<rewardIndex.size(); ++i)
	{
		UT_ServerHelper::LC_ItemCntList rewardInfo;
		bool tag = GetGuildMapReward(mapID, rewardIndex[i], rewardInfo);
		if(tag && !rewardInfo.empty())
		{
			UT_ServerHelper::SendToMailGuild("182;", "183;", guildID, 0, rewardInfo);
		}
	}	
}

void UT_ServerHelper::SaveGuildMapBossInfo(int32_t mapID, uint64_t guildID, const PS_GuildBossInfo& data)
{
	//global节点存储数据
	if(false == GetGameServerApp()->IsNodeType_Global())
	{
		return;
	}

	//只保存存活状态的信息
	if(GuildBossState_Alive != data.m_nState)
	{
		return;
	}

	GuildBossInfo info;
	info.m_nBossID = data.m_nBossID;
	info.m_nState = data.m_nState;
	info.m_nBossHP = data.m_nBossHP;
	info.m_nInvaildTime = data.m_nInvaildTime;

	std::stringstream ss;
	boost::ajson::save_to_buff(info, ss);
	std::string str = ss.str();

	mem::vector<uint64_t> nameKey;
	nameKey.push_back(mapID);

	mem::vector<uint64_t> key;
	key.push_back(guildID);
	UT_ServerHelper::RedisWriteString(UT_REDIS_KEY_GUILDMAPBOSS_STATE_INFO, nameKey, key, str);
}

void UT_ServerHelper::LoadGuildMapBossInfo()
{
	//global节点加载数据
	if(!loadGuildMapBossInfo && GetGameServerApp()->IsNodeType_Global())
	{
		int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		CF_GuildTower::DataEntryMapExternIterator kIt = CF_GuildTower::GetSingletonPtr()->GetIterator();
		for(; kIt.HasMoreElements(); kIt.MoveNext())
		{
			//std::stringstream logStr;
			CF_GuildTower::DataEntry* pkData = kIt.PeekNextValuePtr();
			mem::map<uint64_t, std::string> result;
			mem::vector<uint64_t> nameKey;
			nameKey.push_back(pkData->_lMapResID);
			RedisReadStringRange(UT_REDIS_KEY_GUILDMAPBOSS_STATE_INFO, nameKey, result);
			//logStr << "LoadGuildMapBossInfo mapID:[" << pkData->_lMapResID << "] resultSize:[" << result.size() << "]";
			for(mem::map<uint64_t, std::string>::iterator it=result.begin(); it!=result.end(); ++it)
			{
				GuildBossInfo info;
				std::string errorMsg;
				if(!boost::ajson::load_from_buff(info, it->second.c_str(), errorMsg))
				{
					//logStr << " ajsonLoadError";
					continue;
				}

				//logStr << "\nguildID:[" << it->first << "] bossState:[" << info.m_nState << "]";

				int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
				if(info.m_nInvaildTime <= currentTime)
				{
					//结算这个副本，发参与奖，清除榜单
					DeleteGuildMapBossHP(pkData->_lMapResID, it->first);
					
					//logStr << " the nextState is GuildBossState_TimeOut!";

					mem::vector<uint64_t> params;
					params.reserve(2);
					params.push_back(it->first);
					params.push_back(pkData->_lMapResID);
					
					mem::vector<uint64_t> allPlayers;			
					UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_GUILDMAPBOSS_DAMAGE_RANK, params, allPlayers);
					//logStr << " playerSize:[" << allPlayers.size() << "]";

					if(allPlayers.size()>0)
					{
						if(!pkData->_sJoinReward.empty())
						{
							int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN17_OPERATE, OPERATE_LOGTYPE4);

							//logStr << " sendJoinReward";
							Utility::UT_SIMDataList itemlist;
							Utility::ParseFmtString2DataIFs(pkData->_sJoinReward, itemlist);
							for(int i=0; i<allPlayers.size(); ++i)
							{
								UT_ServerHelper::SendToMail(pkData->_iMailTitle, pkData->_iMailDesc, allPlayers[i], "", 0, itemlist, "",nLogCode);
							}
						}
						//logStr << " Clear damage from redis!";
						UT_ServerHelper::RedisRank_ClearDamageRecord(UT_REDIS_KEY_GUILDMAPBOSS_DAMAGE_RANK, params);
					}
					continue;					
				}

				static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
				LC_ServerMapLogic_Place* pkPlace = pkWorldMgr->CreateServerMapLogic(0, pkData->_lMapResID, MT_GUILD_MAP, 0);
				if(NULL == pkPlace)
				{
					//logStr << " createMapError guildID:[" << it->first << "] mapID:[" << pkData->_lMapResID << "]";
					continue;
				}

				pkPlace->SetGuildID(it->first);
				pkPlace->SetPlaceInvaildTime(info.m_nInvaildTime);
				//logStr << " createMapSuccess guildID:[" << it->first << "] mapID:[" << pkData->_lMapResID << "] mapLogicID:[" << pkPlace->GetMapLogicID() << "]";

				mem::map<int, mem::map<uint64_t, GuildBossInfo> >::iterator findIt = guildMapBossInfo.find(pkData->_lMapResID);
				if(findIt == guildMapBossInfo.end())
				{
					mem::map<uint64_t, GuildBossInfo> guildMap;

					GuildBossInfo data;
					data.m_nBossHP = info.m_nBossHP;
					data.m_nBossID = info.m_nBossID;
					data.m_nState = info.m_nState;

					guildMap.insert(std::make_pair(it->first, data));

					guildMapBossInfo.insert(std::make_pair(pkData->_lMapResID, guildMap));
				}
				else
				{
					mem::map<uint64_t, GuildBossInfo>::iterator kIt = findIt->second.find(it->first);
					if(kIt == findIt->second.end())
					{
						GuildBossInfo data;
						data.m_nBossHP = info.m_nBossHP;
						data.m_nBossID = info.m_nBossID;
						data.m_nState = info.m_nState;

						findIt->second.insert(std::make_pair(it->first, data));
					}
				}
			}
			//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "logStr:%s\n", logStr.str().c_str());
		}

		loadGuildMapBossInfo = true;
	}
}

int64_t UT_ServerHelper::GetGuildMapBossHP(int mapID, uint64_t guildID, int bossID)
{
	mem::map<int, mem::map<uint64_t, GuildBossInfo> >::iterator findIt = guildMapBossInfo.find(mapID);
	if(findIt == guildMapBossInfo.end())
	{
		return 0;
	}

	mem::map<uint64_t, GuildBossInfo>::iterator kIt = findIt->second.find(guildID);
	if(kIt == findIt->second.end())
	{
		return 0;
	}

	if(kIt->second.m_nBossID != bossID)
	{
		return 0;
	}

	return kIt->second.m_nBossHP;
}

void UT_ServerHelper::DeleteGuildMapBossHP(int mapID, uint64_t guildID)
{
	mem::vector<uint64_t> nameKey;
	nameKey.push_back(mapID);

	mem::vector<uint64_t> key;
	key.push_back(guildID);
	RedisRemoveOneKey(UT_REDIS_KEY_GUILDMAPBOSS_STATE_INFO, nameKey, key);

	mem::map<int, mem::map<uint64_t, GuildBossInfo> >::iterator findIt = guildMapBossInfo.find(mapID);
	if(findIt == guildMapBossInfo.end())
	{
		return;
	}

	mem::map<uint64_t, GuildBossInfo>::iterator kIt = findIt->second.find(guildID);
	if(kIt == findIt->second.end())
	{
		return;
	}

	findIt->second.erase(kIt);

	if(findIt->second.empty())
	{
		guildMapBossInfo.erase(findIt);
	}
}

ResultType UT_ServerHelper::CheckGetRestartReward(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	int registerTime = pkPlayer->GetRegTime();
	if(registerTime >= gameStartTime)
	{
		return RE_GAMERESTART_ERROR_1;
	}

	mem::set<uint64_t>::iterator findIt = restartRewardPlayerID.find(pkPlayer->GetInstance());
	if(findIt == restartRewardPlayerID.end())
	{
		return RE_GAMERESTART_ERROR_2;
	}

	return RE_GAMERESTART_ERROR_3;
}

void UT_ServerHelper::UpdatePlayerRestartRewardState(uint64_t charID, bool broadcast)
{
	restartRewardPlayerID.insert(charID);

	if(broadcast)
	{
		MG_G2G_Update_RestartRewardPlayerID msg;
		msg.m_nCharID = charID;
		SendMsgToChat(MGPT_G2G_UPDATE_RESTARTREWARDPLAYERID, &msg, 0, 0, UT_ServerHelper::GetServerId(), 0, MG_SERVER_ROUTER_FLAG_SINGLE);
	}
}

void UT_ServerHelper::LoadOfflineAnswerResultFromRedis()
{
	offlinePlayerAnswerResult.clear();

	mem::map<uint64_t, std::string> result;
	RedisGetData_StringRange(UT_REDIS_KEY_OFFLINE_ANSWER_RESULT, result);
	for(mem::map<uint64_t, std::string>::iterator it=result.begin(); it!=result.end(); ++it)
	{
		AnswerResult info;
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(info, it->second.c_str(), errorMsg))
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "LoadOfflineAnswerResultFromRedis Failed parse string", errorMsg);
			continue;
		}

		offlinePlayerAnswerResult.insert(std::make_pair(it->first, info));
	}
}

void UT_ServerHelper::FlushOfflineAnswerResult(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	uint64_t charid = pkPlayer->GetInstance();
	mem::map<uint64_t, AnswerResult>::iterator findIt = offlinePlayerAnswerResult.find(charid);
	if(findIt == offlinePlayerAnswerResult.end())
	{
		return;
	}

	const AnswerResult& data = findIt->second;
	if(data.m_nRankIndex.size() != data.m_nTimes.size())
	{
		return;
	}

	for(int i=0; i<data.m_nRankIndex.size(); ++i)
	{
		AchievementEvent kEvent;
		kEvent.SetConditionInfo(data.m_nRankIndex[i]);
		kEvent.SetNum(data.m_nTimes[i]);
		pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ANSWER_RESULT, kEvent);
	}

	EraseOfflineAnswerResult(charid);
}

void UT_ServerHelper::UpdateOfflineAnswerResult(uint64_t charid, int32_t rankIndex)
{
	mem::map<uint64_t, AnswerResult>::iterator findIt = offlinePlayerAnswerResult.find(charid);
	if(findIt == offlinePlayerAnswerResult.end())
	{
		AnswerResult rankData;
		rankData.m_nRankIndex.push_back(rankIndex);
		rankData.m_nTimes.push_back(1);
		offlinePlayerAnswerResult.insert(std::make_pair(charid, rankData));
		findIt = offlinePlayerAnswerResult.find(charid);
	}
	else
	{
		bool tag = false;
		AnswerResult& data = findIt->second;
		if(data.m_nRankIndex.size() != data.m_nTimes.size())
		{
			return;
		}

		for(int i=0; i<data.m_nRankIndex.size(); ++i)
		{
			if(data.m_nRankIndex[i] == rankIndex)
			{
				data.m_nTimes[i] += 1;

				tag = true;
				break;
			}
		}

		if(!tag)
		{
			data.m_nRankIndex.push_back(rankIndex);
			data.m_nTimes.push_back(1);
		}
	}

	std::stringstream ss;
	boost::ajson::save_to_buff(findIt->second, ss);
	std::string data = ss.str();

	mem::vector<uint64_t> key;
	key.push_back(charid);
	RedisSetData_String(UT_REDIS_KEY_OFFLINE_ANSWER_RESULT, key, data,  false, true);
}

void UT_ServerHelper::EraseOfflineAnswerResult(uint64_t charid)
{
	mem::map<uint64_t, AnswerResult>::iterator findIt = offlinePlayerAnswerResult.find(charid);
	if(findIt == offlinePlayerAnswerResult.end())
	{
		return;
	}

	offlinePlayerAnswerResult.erase(findIt);

	mem::vector<uint64_t> key;
	key.push_back(charid);
	RedisEraseData_String(UT_REDIS_KEY_OFFLINE_ANSWER_RESULT, key, false);
}

void UT_ServerHelper::InitSpecialTask()
{
	CF_SpecialTask::DataEntryMapExternIterator kIt = CF_SpecialTask::GetSingletonPtr()->GetIterator();
	for(; kIt.HasMoreElements(); kIt.MoveNext())
	{
		CF_SpecialTask::DataEntry* pkData = kIt.PeekNextValuePtr();
		mem::vector<int> taskIDVec;
		taskIDVec.reserve(pkData->_kTaskIDs.size());
		taskIDVec = pkData->_kTaskIDs;

		specialTaskInfo[pkData->_iID] = taskIDVec;
	}
}

bool	UT_ServerHelper::CheckTaskMatch(int taskSystem, int taskID)
{
	mem::map<int32_t, mem::vector<int> >::iterator findIt = specialTaskInfo.find(taskSystem);
	if(findIt == specialTaskInfo.end())
	{
		return false;
	}

	for(int i=0; i<findIt->second.size(); ++i)
	{
		if(findIt->second[i] == taskID)
		{
			return true;
		}
	}
	return false;
}

void UT_ServerHelper::GetGameStoryBatch(LC_ServerPlayer* pkOwnerPlayer, int resMapID)
{
	if(NULL == pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetGameStoryBatch r1 mapID:%d", resMapID);
		return;
	}

	LC_ServerWorldManager* pkServerWorldMgr = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogicPlaceVector* pkPlaces = pkServerWorldMgr->GetMapLogicByRes(resMapID);
	if(NULL==pkPlaces || pkPlaces->size() != 1)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetGameStoryBatch r2 mapID:%d", resMapID);
		return;
	}

	LC_ServerMapLogic_Place* pkPlace = *pkPlaces->begin();
	if(NULL == pkPlace)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetGameStoryBatch r3 mapID:%d", resMapID);
		return;
	}

	int storyID = pkPlace->GetStoryID();
	if(0 == storyID)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetGameStoryBatch r4 mapID:%d", resMapID);
		return;
	}

	GameStory::LC_GameStory_Manager* pkMgr =  SERVER_GET_GAMESTORY_MANAGER;
	const GameStory::GameStoryRecord* pkGSRecord = pkMgr->GetStoryRecord(storyID);
	if(!pkGSRecord->m_bActive)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetGameStoryBatch r5 mapID:%d, storyID:%d", resMapID, storyID);
		return;
	}

	GameStory::LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	GameStory::LC_Server_GameStory_Base* pkStory = pkLogicMgr->GetStoryLogic(pkGSRecord->m_iStoryLogicId);
	if(NULL == pkStory)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetGameStoryBatch r6 mapID:%d, storyID:%d, storyLogicId:%d", resMapID, storyID, pkGSRecord->m_iStoryLogicId);
		return;
	}

	GameStory::GameStoryScheduleKey iKey = (GameStory::GameStoryScheduleKey)pkPlace->GetMapLogicID();
	uint32_t res = pkStory->NotifyPlayerGameStoryBatch(iKey, pkOwnerPlayer, Utility::TIMER_ID_NA, NULL);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetGameStoryBatch r7 mapID:%d, storyID:%d, storyLogicId:%d, iKey:%d, res:%d", resMapID, storyID, pkGSRecord->m_iStoryLogicId, iKey, res);
}

bool UT_ServerHelper::GenDetailKey( mem::vector<uint64_t>& nKeys, std::string& strKey )
{
	strKey = "data";
	for (int i = 0; i < nKeys.size(); i++)
	{
		strKey += ":";
		std::stringstream tempStream;
		tempStream << nKeys[i];
		strKey += tempStream.str();
	}
	return true;
}

std::string UT_ServerHelper::GenDetailKeyWithoutData( mem::vector<uint64_t>& nKeys)
{
	std::string strKey = "";
	for ( int i = 0; i < nKeys.size(); i++ )
	{
		std::ostringstream message;
		message << nKeys[i];
		std::string strType = message.str();

		if(i!=0)
		{
			strKey += ":";
		}
		strKey += strType;
	}
	return strKey;
}

bool UT_ServerHelper::RedisRank_WriteElement( const char* szNameSpace,  uint64_t nCharID,  uint64_t nScore, bool bGlobalServer )
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_WriteElement---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	char szUser[CHAR_ID_STRING_LEN] = { '\0' };
	sprintf( szUser, "%llu",  nCharID);
	redispp::IntReply nReply = pkRedisConn->zadd(sKey, nScore, szUser );

	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_WriteElement ret:%d, sKey:%s uint64_t:%d\n", ret, sKey.c_str(), nScore);
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisRank_ReadElement( const char* szNameSpace,  uint64_t nCharID,  uint64_t& nScore, int32_t& nIndex, bool bGlobalServer, bool addSnapShot)
{
	int ret = 0;
	bool bResult = true;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_ReadElement---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer, addSnapShot);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	char szUser[CHAR_ID_STRING_LEN] = { '\0' };
	sprintf( szUser, "%llu",  nCharID);
	redispp::IntReply nScoreReply = pkRedisConn->zscore(sKey, szUser );
	redispp::IntReply nIndexReply = pkRedisConn->zrevrank(sKey, szUser );
	nScore = nScoreReply.result()!=-1 ? nScoreReply.result() : 0;
	nIndex = nIndexReply.result();

	bResult = (nIndex != -1);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_ReadElement ret:%d, sKey:%s\n", ret, sKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisRank_EraseElement(const char* szNameSpace,  uint64_t nCharID, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_EraseElement---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	char szUser[CHAR_ID_STRING_LEN] = { '\0' };
	sprintf( szUser, "%llu",  nCharID);
	redispp::IntReply nReply = pkRedisConn->zrem(sKey, szUser );
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_EraseElement ret:%d, sKey:%s, id:%d\n", ret, sKey.c_str(), nCharID);
		return false;
	}
	return  true;
}

int UT_ServerHelper::RedisRank_ReadSimpleRange( const char* szNameSpace, mem::vector<uint64_t>& vecRangeData,  bool addSnapShot, int32_t nTopCount, bool bGlobalServer)
{
	vecRangeData.clear();
	vecRangeData.reserve(100);

	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_ReadSimpleRange---EntryServeState init fail！");
		return 0;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer, addSnapShot);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	uint64_t nInputScore = 0;
	int nSize = -1;
	if ( nTopCount  != -1 )
	{
		nSize = nTopCount - 1;
	}
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->zrevrange(sKey, 0, nSize);

	std::string str;
	for ( int i = 0;multiReply.next(&str); i++ )
	{
		if (str.empty())
		{
			continue;
		}
		if (i % 2 > 0)
			continue;
		uint64_t nCharID = _atoi64(str.c_str());
		vecRangeData.push_back(nCharID);
	}

	UT_REDIS_CMD_END(ret, pkRedisHandler);
	if( 0 != ret )
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_ReadSimpleRange ret:%d, sKey:%s\n", ret, sKey.c_str());
	return vecRangeData.size();
}

bool UT_ServerHelper::RedisRank_Clear( const char* szNameSpace, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pServerApp =  GetGameServerApp();
	

	if(!pServerApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_Clear---EntryServeState init fail！");
		return false;
	}

	Utility::Redis_Handler* pkRedisHandler = pServerApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer);

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::BoolReply nReply = pkRedisConn->del(sKey);
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_Clear ret:%d, sKey:%s, serverID:%d\n", ret, sKey.c_str(), GetGameServerID());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisRank_CreateSnapshot( const char* szNameSpace, bool bGlobalServer)
{
	int ret = 0;
	mem::vector<uint64_t> vecRangeData;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_CreateSnapshot---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer);
	std::string strSnapshotKey = UT_ServerHelper::GetRedisStr(szNameSpace, bGlobalServer, true);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	int nSize = -1;
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->zrevrange(sKey, 0, nSize );

	std::string str;
	uint64_t nCharID;
	uint64_t nScore;
	for ( int i = 0 ;multiReply.next(&str); i++ )
	{
		if (str.empty())
		{
			continue;
		}
		if ( i % 2 > 0 )
		{
			nScore = _atoi64(str.c_str());
			char szUser[CHAR_ID_STRING_LEN] = { '\0' };
			sprintf( szUser, "%llu",  nCharID);
			pkRedisConn->zadd(strSnapshotKey, nScore, szUser );
		}
		else
		{
			nCharID = _atoi64(str.c_str());
		}
	}

	UT_REDIS_CMD_END(ret, pkRedisHandler);
	if( 0 != ret )
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_CreateSnapshot ret:%d, sKey:%s\n", ret, sKey.c_str());
	return vecRangeData.size();
}

bool UT_ServerHelper::RedisWriteString(const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::vector<uint64_t>& nKeys, const std::string& strInput, bool bGlobalServer)
{
	int ret = 0;	
	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, nNameKeys, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	pkRedisConn->hset(sKey, strDetailKey, strInput);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisWriteString ret:%d, sKey:%s, strDetailKey:%d, strInput:%s\n", ret, sKey.c_str(), strDetailKey.c_str(), strInput.c_str());
		return false;
	}
	return  true;
}

int UT_ServerHelper::RedisReadStringRange( const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::map<uint64_t, std::string>& result, bool bGlobalServer)
{
	int ret = 0;
	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, nNameKeys, bGlobalServer);

	GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	uint64_t nInputScore = 0;
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->hgetAll(sKey);

	std::string str;
	uint64_t time = 0;
	for ( int i = 0;multiReply.next(&str); i++ )
	{
		if (str.empty())
		{
			continue;
		}

		if(i%2 == 0)
		{
			time = _atoi64(str.c_str());
		}
		else
		{
			result.insert(std::make_pair(time, str));
		}
	}

	UT_REDIS_CMD_END(ret, pkRedisHandler);
	if( 0 != ret )
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisReadStringRange ret:%d, sKey:%s\n", ret, sKey.c_str());
	
	return result.size();
}

bool UT_ServerHelper::RedisGetStringData( const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::vector<uint64_t>& nKeys, std::string& strResult, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetStringData---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, nNameKeys, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);
	std::string strValue = "";

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (kCharReply.result())
	{
		strResult = kCharReply;
	}	
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetStringData ret:%d, sKey:%s\n", ret, sKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisClearString( const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, bool bGlobalServer)
{
	GameServerApp* pServerApp =  GetGameServerApp();
	if(!pServerApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisClearString---EntryServeState init fail!");
		return false;
	}

	Utility::Redis_Handler* pkRedisHandler = pServerApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " NULL == pkRedisHandler!");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, nNameKeys, bGlobalServer);

	Utility::Redis_Channel& kRedisChannel = pServerApp->GetRedisChannel();
	ASYNC_REDIS_REQUEST_NCB(kRedisChannel, &redispp::Connection::del, sKey);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "RedisClearString sKey:%s\n", sKey.c_str());

	return true;
}

bool UT_ServerHelper::RedisRemoveOneKey(const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::vector<uint64_t>& nKeys, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRemoveOneKey---EntryServeState init fail!");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, nNameKeys, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	pkRedisConn->hdel(sKey, strDetailKey);
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRemoveOneKey ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisRank_ClearDamageRecord(const char* szNameSpace, mem::vector<uint64_t>& params, bool bGlobalServer)
{
	GameServerApp* pkApp = GetGameServerApp();
	
	int ret = 0;
	bool bResult = true;
	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);

	Utility::Redis_Channel& kRedisChannel = pkApp->GetRedisChannel();
	ASYNC_REDIS_REQUEST_NCB(kRedisChannel, &redispp::Connection::del, sKey);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "RedisRank_ClearDamageRecord ret:%d, sKey:%s\n", ret, sKey.c_str());
	return true;
}

bool UT_ServerHelper::RedisRank_EraseDamage( const char* szNameSpace, mem::vector<uint64_t>& params, uint64_t nCharID, bool bGlobalServer)
{
	int ret = 0;
	bool bResult = true;
	GameServerApp* pkApp = GetGameServerApp();

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);
	
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	char szUser[CHAR_ID_STRING_LEN] = { '\0' };
	sprintf( szUser, "%llu",  nCharID);
	redispp::IntReply nReply = pkRedisConn->zrem(sKey, szUser);
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	if( -1 == ret)
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_EraseDamage ret:%d, sKey:%s\n", ret, sKey.c_str());
	
	return bResult;
}

bool UT_ServerHelper::RedisRank_ReadDamage( const char* szNameSpace, mem::vector<uint64_t>& params, uint64_t nCharID,  uint64_t& nScore, int32_t& nIndex, bool bGlobalServer)
{
	int ret = 0;
	bool bResult = true;
	GameServerApp* pkApp = GetGameServerApp();

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	char szUser[CHAR_ID_STRING_LEN] = { '\0' };
	sprintf( szUser, "%llu",  nCharID);
	redispp::IntReply nScoreReply = pkRedisConn->zscore(sKey, szUser );
	redispp::IntReply nIndexReply = pkRedisConn->zrevrank(sKey, szUser );
	nScore = nScoreReply.result();
	nIndex = nIndexReply.result();

	if(nIndex == -1)
	{
		nScore = 0;
	}

	bResult = (nIndex != -1);
	UT_REDIS_CMD_END(ret, pkRedisHandler);	
	if( -1 == ret)
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_ReadDamage ret:%d, sKey:%s\n", ret, sKey.c_str());
	
	return bResult;
}

bool UT_ServerHelper::RedisRank_WriteDamage( const char* szNameSpace, mem::vector<uint64_t>& params, uint64_t nCharID,  uint64_t nScore, bool bGlobalServer)
{
	int ret = 0;
	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);

	GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	char szUser[CHAR_ID_STRING_LEN] = { '\0' };
	sprintf( szUser, "%llu",  nCharID);
	redispp::IntReply nReply = pkRedisConn->zadd(sKey, nScore, szUser );
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_WriteDamage ret:%d, sKey:%s, nScore:%d\n", ret, sKey.c_str(), nScore);
		return false;
	}
	return  true;
}

int UT_ServerHelper::RedisRank_ReadSimpleDamageRange( const char* szNameSpace, mem::vector<uint64_t>& params, mem::vector<uint64_t>& vecRangeData,  int32_t nTopCount, bool bGlobalServer)
{
	int ret = 0;
	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);

	GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	uint64_t nInputScore = 0;
	int nSize = -1;
	if ( nTopCount  != -1 )
	{
		nSize = nTopCount - 1;
	}
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->zrevrange(sKey, 0, nSize);

	std::string str;
	for ( int i = 0;multiReply.next(&str); i++ )
	{
		if (str.empty())
		{
			continue;
		}
		if (i % 2 > 0)
			continue;
		uint64_t nCharID = _atoi64(str.c_str());
		vecRangeData.push_back(nCharID);
	}

	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if( -1 == ret)
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisRank_ReadSimpleDamageRange ret:%d, sKey:%s\n", ret, sKey.c_str());
	return vecRangeData.size();
}

bool UT_ServerHelper::RedisSetData_DamageString( const char* szNameSpace, mem::vector<uint64_t>& params, mem::vector<uint64_t>& nKeys, const std::string& strInput, bool bGlobalServer, bool bForceSet)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisSetData_DamageString---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if(bForceSet || kCharReply.result())
	{
		pkRedisConn->hset(sKey, strDetailKey, strInput);
	}	
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if( -1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisSetData_DamageString ret:%d, sKey:%s, strDetailKey:%s, strInput:%s\n", ret, sKey.c_str(), strDetailKey.c_str(), strInput.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisGetData_DamageString( const char* szNameSpace, mem::vector<uint64_t>& params, mem::vector<uint64_t>& nKeys, std::string& strResult, bool bGlobalServer)
{
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	if(!pkApp->GetInitEntryServeState())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_DamageString---EntryServeState init fail！");
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);
	std::string strDetailKey = GenDetailKeyWithoutData(nKeys);
	std::string strValue = "";

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (kCharReply.result())
	{
		strResult = kCharReply;
	}	
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	
	if(-1 == ret)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisGetData_DamageString ret:%d, sKey:%s, strDetailKey:%s\n", ret, sKey.c_str(), strDetailKey.c_str());
		return false;
	}
	return  true;
}

bool UT_ServerHelper::RedisClearData_DamageString( const char* szNameSpace, mem::vector<uint64_t>& params, bool bGlobalServer)
{
	GameServerApp* pServerApp =  GetGameServerApp();
	if(!pServerApp->GetInitEntryServeState())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RedisClearData_DamageString---EntryServeState init fail!");
		return false;
	}

	Utility::Redis_Handler* pkRedisHandler = pServerApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return false;
	}

	std::string sKey = UT_ServerHelper::GetRedisStr(szNameSpace, params, bGlobalServer);

	Utility::Redis_Channel& kRedisChannel = pServerApp->GetRedisChannel();
	ASYNC_REDIS_REQUEST_NCB(kRedisChannel, &redispp::Connection::del, sKey);
	return true;
}

bool LC_ServerHelperDiff::CrossSafe( int32_t iIndex, int32_t sServerID )
{
	GameServerManager* servermanager = GetGameServerApp()->GetServerManager();
	GameServerManager::GroupServerList& cgs = servermanager->GetCurGroupServers();
	if  ( iIndex >= 0 && iIndex < (int32_t)cgs.size() )
	{
		return sServerID == cgs[iIndex];
	}
	return false;
}

float LC_ServerHelperDiff::CalMasterBuffState(int32_t serverID , int64_t iScore, int32_t worldLevel)
{
	SC_ServerScriptInterface* ssi = SC_ServerScriptInterface::GetSingletonPtr();
	return ssi->GetSkillInterface()->CalcuteMultiply(worldLevel, iScore);
}

bool LC_ServerHelperDiff::IsCrossPlayer(LC_PlayerBase* pkPlayer)
{
	return UT_ServerHelper::IsInCross(pkPlayer->GetInstance());
}





void UT_ServerHelper::UpdateRankList(int rankType, const std::vector<uint64_t>& idList)
{
	LC_ServerPlayerManager*  pkPlayManager = SERVER_GET_PLAYER_MANAGER();

	std::map<int, mem::map<uint64_t, int> > ::iterator findIt = rankList.find(rankType);
	if(findIt == rankList.end())
	{
		mem::map<uint64_t, int> playerList;
		rankList[rankType] = playerList;
	}

	findIt = rankList.find(rankType);
	findIt->second.clear();
	for(int index=0; index<idList.size(); ++index)
	{
		uint64_t charID = idList[index];
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayManager->FindPlayer(charID);
		if(NULL != pkPlayer)
		{
			pkPlayer->SyncTaskRankInfo(index);
		}

		findIt->second[charID] = index;
	}
}

int UT_ServerHelper::GetRankIndexByType(int rankType, uint64_t charID)
{
	std::map<int, mem::map<uint64_t, int> >::iterator findIt = rankList.find(rankType);
	if(findIt == rankList.end())
	{
		return -1;
	}

	mem::map<uint64_t, int>::iterator it = findIt->second.find(charID);
	if(it == findIt->second.end())
	{
		return -1;
	}

	return it->second;
}

void UT_ServerHelper::RankReward(int rankType, bool check)
{
	//entry请求发奖
	if(GetGameServerID() != GetGameServerEntryID())
	{
		return;
	}

	//活动有效，return
	if(check)
	{
		static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
		bool tag = pkScheduleMgr->IsScheduleValid(GetGameServerID(), GetGlobalSetting.ScheduleID);
		if(tag)
		{
			return;
		}
	}	

	int state = GetRankRewardState(rankType);
	if(1 == state)
	{
		return;
	}

	std::map<int, mem::map<uint64_t, int> >::iterator findIt = rankList.find(rankType);
	if(findIt!=rankList.end() && !findIt->second.empty())
	{
		time_t time = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		tm* curtm = localtime(&time);
		int year = curtm->tm_year+1900;
		int month = curtm->tm_mon+1;
		int day = curtm->tm_mday;

		std::stringstream ss;
		ss<<year<<"-"<<month<<"-"<<day;
		StringType notifyTime = STDSTR2TPSTR(ss.str());

		for(mem::map<uint64_t, int>::iterator it=findIt->second.begin(); it!=findIt->second.end(); ++it)
		{
			int rankRange = LC_Helper::GetTaskRankRange(it->second);
			CF_TaskRankRewards::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_TaskRankRewards>(rankRange);
			if(NULL == pkData)
			{
				continue;
			}

			stringstream id;
			id << pkData->_iID;

			StringType mailContent = "2;" + notifyTime + ";" + STDSTR2TPSTR(id.str()) + ";";
			int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MAINTASK_OPERATE, OPERATE_LOGTYPE3);
			UT_ServerHelper::SendMailToPlayer(it->first, "1;", mailContent, pkData->_kReward, false, nLogCode, it->second+1);
		}
	}

	//排名结算系统公告
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	noticeFmort.SetMsgID(30);
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort);

	SetRankRewardState(rankType);
}

int32_t UT_ServerHelper::GetSubCombatType(int32_t subType)
{
	int32_t combatScoreType = -1;
	switch(subType)
	{
	case LC_SUB_TP_VEHICLE:
		combatScoreType = CST_VehicleSkillScore;
		break;
	case LC_SUB_TP_DEVA_EYE:
		combatScoreType = CST_DevaeyeSkillScore;
		break;
	case LC_SUB_TP_MUHUN:
		combatScoreType = CST_WingSkillScore;
		break;
	default:
		break;
	}

	return combatScoreType;
}

int UT_ServerHelper::GetRankRewardState(int rankType)
{
	mem::vector<uint64_t> input;
	input.push_back(rankType);
	int res = 0;
	UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_RANK_STATE, input, res);
	return res;
}

void UT_ServerHelper::SetRankRewardState(int rankType)
{
	mem::vector<uint64_t> input;
	input.push_back(rankType);
	UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_RANK_STATE, input, 1);
}


bool UT_ServerHelper::SendSkyArenaReward(uint64_t nCharId, int32_t nRankId, int nIndex, int nScore, int nGrade)
{
	std::stringstream sLog;
	sLog<<"received  苍穹战场 发奖 nCharId = " <<nCharId<<" nRankId =  " << nRankId << " nScore:" << nScore<<" nGrade:" <<nGrade<<"" ;
	sLog<<" [";
	CF_SkyArenaRankReward::DataEntryMapExternIterator kIter = CF_SkyArenaRankReward::GetSingletonPtr()->GetIterator();
	//int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE1);
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_SkyArenaRankReward::DataEntry* pkData = kIter.PeekNextValuePtr();	
		if(pkData->_iType == nRankId && pkData->_iRankStartIndex <= nIndex && nIndex < pkData->_iRankEndIndex )
		{
			sLog<<" ( uCharID = " << nCharId <<" nIndex = "<<nIndex;
			stringstream ss;
			ss<< nScore << ";";

			stringstream index;
			index << (nIndex+1) << ";";
			stringstream grade;
			grade << (nGrade) << ";";

			StringType mailDesc = pkData->_iMailDesc + STDSTR2TPSTR(ss.str()) + STDSTR2TPSTR(index.str()) +STDSTR2TPSTR(grade.str());
			if(RE_SUCCESS != UT_ServerHelper::SendMailToPlayer(nCharId,pkData->_iMailTitle, mailDesc, pkData->_kReward))
			{
				sLog << " false) ";
				continue;
			}
			sLog << " true)";
		}
	}		

	sLog<<" ]";
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d]SendSkyArenaReward:%s", UT_ServerHelper::GetServerId(),sLog.str().c_str());
	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s",sLog.str().c_str());
	return true;
}

bool UT_ServerHelper::SendSummitArenaRiseUpMail(uint64_t nCharId, int nType,int nStageType)
{
	CF_SummitArenaMail::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SummitArenaMail>(nStageType);
	if(pkData == NULL)
	{
		return false;
	}
	StringType mailTitle;
	StringType mailDesc;
	stringstream ss;
	ss<< nStageType << ";";

	if(nType == 0)
	{
		mailDesc = pkData->_sMailDescFaile + STDSTR2TPSTR(ss.str());
		mailTitle = pkData->_sMailTitleFaile + STDSTR2TPSTR(ss.str());
	}
	else
	{
		mailDesc = pkData->_sMailDesc + STDSTR2TPSTR(ss.str());
		mailTitle = pkData->_sMailTitle + STDSTR2TPSTR(ss.str());
	}
	
	return UT_ServerHelper::SendMailToPlayer(nCharId,mailTitle, mailDesc, "");
}
bool UT_ServerHelper::SendSummitArenaRankAwardMail(uint64_t nCharId, int32_t nStageType, int nIndex)
{
	std::stringstream sLog;
	sLog<<"received  巅峰竞技场 发奖 nCharId = " <<nCharId<<" nStageType =  " << nStageType << " nIndex:" << nIndex;
	sLog<<" [";
	CF_SummitArenaRankRewards::DataEntryMapExternIterator kIter = CF_SummitArenaRankRewards::GetSingletonPtr()->GetIterator();
	//int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE1);
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_SummitArenaRankRewards::DataEntry* pkData = kIter.PeekNextValuePtr();	
		if( pkData->_iRankStartIndex <= nIndex && nIndex < pkData->_iRankEndIndex )
		{
			sLog<<" ( uCharID = " << nCharId <<" nIndex = "<<nIndex;
			stringstream ss;
			ss<< nStageType << ";";

			stringstream index;
			index << (nIndex+1) << ";";

			StringType mailDesc = pkData->_sMailDesc + STDSTR2TPSTR(ss.str()) + STDSTR2TPSTR(index.str());
			if(RE_SUCCESS != UT_ServerHelper::SendMailToPlayer(nCharId,pkData->_sMailTitle, mailDesc, pkData->_kReward))
			{
				sLog << " false) ";
				continue;
			}
			sLog << " true)";
		}
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d]SendSummitArenaRankAwardMail:%s", UT_ServerHelper::GetServerId(),sLog.str().c_str());
	return true;
}
bool UT_ServerHelper::SendSummitArenaGroupStageMail(uint64_t nCharId)
{
	stringstream sTitle;
	sTitle << GetGlobalSetting.iUpGroupStageMailTitleId << ";";
	stringstream sDesc;
	sDesc <<  GetGlobalSetting.iUpGroupStageMailContextId << ";";
	StringType mailTitle = STDSTR2TPSTR(sTitle.str());
	StringType mailDesc = STDSTR2TPSTR(sDesc.str());
	return UT_ServerHelper::SendMailToPlayer(nCharId,mailTitle, mailDesc, "");
}

bool UT_ServerHelper::SendSummitArenaGloryRankRewardMail(uint64_t nCharId, int32_t nType, int32_t nTimes)
{
	std::stringstream sLog;
	sLog<<"received  荣耀之巅 发奖 nCharId = " <<nCharId<<" nType =  " << nType << " nTimes:" << nTimes;
	sLog<<" [";
	CF_SummitArenaGloryRankRewards::DataEntryMapExternIterator kIter = CF_SummitArenaGloryRankRewards::GetSingletonPtr()->GetIterator();
	//int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE1);
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_SummitArenaGloryRankRewards::DataEntry* pkData = kIter.PeekNextValuePtr();	
		if( pkData->_iID == nType && nTimes == pkData->_Count )
		{


			sLog<<" ( uCharID = " << nCharId <<" nTimes = "<<nTimes;
			stringstream index;
			index << nType << ";";
			stringstream ss;
			ss<< pkData->_Count << ";";
			StringType mailDesc = pkData->_sMailDesc + STDSTR2TPSTR(index.str()) + STDSTR2TPSTR(ss.str());
			if(RE_SUCCESS != UT_ServerHelper::SendToMail(pkData->_sMailTitle, mailDesc, nCharId, "",0, pkData->_Rewards))
			{
				sLog << " false) ";
				continue;
			}
			sLog << " true)";
		}
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d]SendSummitArenaGloryRankRewardMail:%s", UT_ServerHelper::GetServerId(),sLog.str().c_str());
	return true;
}

bool UT_ServerHelper::NotifyServerErrorCode( LC_ServerPlayer* pkPlayer, uint32_t nErrorCode)
{
	if(pkPlayer)
	{
		MG_Notify_ServerErrorCode resp;//
		resp.m_nErrorCode = nErrorCode;
		pkPlayer->SendMsgToClient(MGPT_NOTIFY_SERVER_ERROR_CODE, &resp);
	}

	return true;
}

 void UT_ServerHelper::WheelFightReq(uint64_t uniqueID,uint64_t uChallengeId,uint64_t uBeChallengeId)
 {
	 // 创建地图
	 LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	 LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, SUMMIT_ARENA_SOLO_DULE_RES_ID, MT_ARENA_SUMMIT);
	 if (logic == NULL)
		 return ;
	 LC_ServerMapLogic_SummitWheelPlay* play = (LC_ServerMapLogic_SummitWheelPlay*)logic->GetPlay(PlayType_SummitWheel);
	 if (play == NULL)
		 return ;
	 play->ReservaDuleInfo(uChallengeId,uBeChallengeId,uniqueID);	
}

uint64_t UT_ServerHelper::GetGuildMapStateMutableParameter(uint64_t nGuildId)
{
	mem::map<uint64_t, uint64_t >::iterator it = m_nGuildMapStateMutableParams.find(nGuildId);
	if(it != m_nGuildMapStateMutableParams.end())
	{
		return it->second;
	}
	return 0;
}

uint64_t UT_ServerHelper::GetGuildBuyItemTimes(uint64_t nGuildId)
{
	mem::map<uint64_t, uint64_t >::iterator it = m_nGuildBuyItemTimes.find(nGuildId);
	if(it != m_nGuildBuyItemTimes.end())
	{
		return it->second;
	}
	return 0;
}

void UT_ServerHelper::AddGuildBuyItemTimes(uint64_t nGuildId, int nAddValue)
{
	uint64_t& nValue = m_nGuildBuyItemTimes[nGuildId];
	nValue += nAddValue;
	GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "AddGuildBuyItemTimes nGuildId:%lld,nAddValue:%d, nValue:%lld",nGuildId,nAddValue, nValue);
	mem::vector<uint64_t> input;
	input.push_back(nGuildId);
	UT_ServerHelper::RedisSetData_Int(UT_REDIS_BONFIREPARTY_ITEMTIMES, input, nValue);
}

uint32_t UT_ServerHelper::AddGuildMapStateMutableParameter(uint64_t nGuildId, int nAddValue)
{
	uint64_t& nValue = m_nGuildMapStateMutableParams[nGuildId];
	nValue += nAddValue;
	GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "AddGuildMapStateMutableParameter nGuildId:%lld,nAddValue:%d",nGuildId,nAddValue);
	return (uint32_t)nValue;
}

void UT_ServerHelper::GetAllGuildMapStateMutableParameterFromRedis()
{
	m_nGuildMapStateMutableParams.clear();
	m_nGuildBuyItemTimes.clear();
	int ret = 0;
	GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = UT_REDIS_BONFIREPARTY_ITEMTIMES;
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::MultiBulkEnumerator multiReply = pkRedisConn->hgetAll(sKey);
	std::string str;
	uint64_t key = 0;
	uint64_t value = 0;
	for ( int i = 0;multiReply.next(&str); i++ )
	{
		if (str.empty())
		{
			continue;
		}
		if(i%2 == 0)
		{
			key = _atoi64(str.c_str());
		}
		else
		{
			value = _atoi64(str.c_str());
			m_nGuildBuyItemTimes.insert(std::make_pair(key, value));
		}
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	mem::map<uint64_t, uint64_t >::iterator it = m_nGuildBuyItemTimes.begin();
	for(;it != m_nGuildBuyItemTimes.end(); it++)
	{
		AddGuildMapStateMutableParameter(it->first, GetGlobalSetting.GuildBonFirePartyExpRate * it->second);
	}
}


void UT_ServerHelper::ClearGuildMapStateMutableParameter()
{
	if(m_nGuildMapStateMutableParams.size() <= 0)
		return;
	mem::vector<uint64_t> input;
	mem::map<uint64_t, uint64_t >::iterator it = m_nGuildBuyItemTimes.begin();
	for(;it != m_nGuildBuyItemTimes.end(); it++)
	{
		input.push_back(it->first);
	}
	UT_ServerHelper::RedisRemoveData_Int(UT_REDIS_BONFIREPARTY_ITEMTIMES,input);
	m_nGuildBuyItemTimes.clear();
	m_nGuildMapStateMutableParams.clear();
}

void UT_ServerHelper::SendAnswerGuildRankAward()
{
	mem::vector<uint64_t> allGuilds;
	UT_ServerHelper::RedisRank_ReadSimpleRange(UT_REDIS_KEY_ANSWER_GUILD_POINT, allGuilds);
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN7_OPERATE, OPERATE_LOGTYPE3);
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	for(int i=0; i<allGuilds.size(); ++i)
	{
		int rankIndex = i;
		uint64_t nGuildId = allGuilds[i];
		CF_TestRankGuildReward::DataEntryMapExternIterator kIter = CF_TestRankGuildReward::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_TestRankGuildReward::DataEntry* pkData = kIter.PeekNextValuePtr();
			if(rankIndex>=pkData->_iRankStartIndex && rankIndex<pkData->_iRankEndIndex)
			{
				UT_ServerHelper::LC_ItemCntList Reward;
				mem::vector<StringType> rewardInfo;
				UT_SIMDataList rewardData;
				ParseFmtString2DataIFs(pkData->_kReward, rewardData);
				for(UT_SIMDataList::iterator it=rewardData.begin(); it!=rewardData.end(); ++it)
				{
					__ItemCnt rewardData;
					rewardData.m_nId = it->IID();
					rewardData.m_nCnt = it->ParamA();
					rewardData.m_nBindMode = it->ParamB();
					Reward.push_back(rewardData);
				}
				
				mem::vector<uint64_t> key;
				key.push_back(nGuildId);

				std::string data;
				UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_ANSWER_GUILD_INFO, key, data, false);
				AnswerInfo info;
				std::string errorMsg;
				int nValue = 0;
				if(boost::ajson::load_from_buff(info, data.c_str(), errorMsg))
				{
					nValue = info.accountPoint;	
				}

				StringType mailDesc;
				stringstream ss;
				ss<< nValue << ";";
				ss<< rankIndex+1 << ";";
				

				mailDesc = pkData->_iMailDesc + STDSTR2TPSTR(ss.str());
				UT_ServerHelper::SendToMailGuild(pkData->_iMailTitle, mailDesc, nGuildId, 0, Reward);
				break;
			}
		}
	}
}

void UT_ServerHelper::AddGuildAnswerPoint(uint32_t nValue, uint64_t nGuildId, const StringType& sGuildName)
{
	uint64_t value = 0;
	int32_t index = 0;

	UT_ServerHelper::RedisRank_ReadElement(UT_REDIS_KEY_ANSWER_GUILD_POINT, nGuildId, value, index);
	uint32_t nLowValue = 0;
	uint32_t nHighValue = 0;
	LC_Helper::DecodeScore(value, nHighValue, nLowValue);
	
	AnswerInfo info;
	info.accountPoint = nHighValue + nValue;
	info.charName = sGuildName.c_str();

	uint32_t currentTime  = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	uint32_t inputTime = LC_Helper::TimeStampReverse(currentTime);
	value = LC_Helper::EncodeScore(info.accountPoint, inputTime);
	UT_ServerHelper::RedisRank_WriteElement(UT_REDIS_KEY_ANSWER_GUILD_POINT, nGuildId, value);
	
	mem::vector<uint64_t> key;
	key.push_back(nGuildId);
	std::string data;
	
	std::stringstream ss;
	boost::ajson::save_to_buff(info, ss);
	data = ss.str().c_str();
	UT_ServerHelper::RedisSetData_String(UT_REDIS_KEY_ANSWER_GUILD_INFO, key, data, false, true);
}


int32_t UT_ServerHelper::GetGuildAnswerPointByGuildId(uint64_t nGuildId)
{
	mem::vector<uint64_t> key;
	key.push_back(nGuildId);
	std::string data;
	UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_ANSWER_GUILD_INFO, key, data);

	AnswerInfo info;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, data.c_str(), errorMsg))
	{
		return 0;
	}
	return info.accountPoint;

}


int32_t UT_ServerHelper::GetUserAnswerPointByCharId(uint64_t nCharId)
{
	mem::vector<uint64_t> key;
	key.push_back(nCharId);
	std::string data;
	UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_ANSWER_PLAYER_INFO, key, data);

	AnswerInfo info;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, data.c_str(), errorMsg))
	{
		return 0;
	}
	return info.accountPoint;
}



void UT_ServerHelper::PayActScore(LC_ServerPlayer* pkPlayer, Utility::UT_SIMDataList&kActScorePrices, int32_t iSchedule)
{
	if (kActScorePrices.empty())
	{
		return;
	}
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Utility::UT_SIMDataList::const_iterator iter = kActScorePrices.begin();
	for (; iter != kActScorePrices.end(); ++iter)
	{
		ACT_ScheduleActivityType iActType = (ACT_ScheduleActivityType)iter->IID();
		int32_t iCategory = iter->ParamA();
		int32_t iActShopScore = iter->ParamB();
		if (iActType <= 0 && iActShopScore <= 0)
		{
			continue;
		}
		Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset.GetScheduleSaveActivity(iActType);
		if (NULL == pkSaveActivity)
		{
			continue;
		}
		ACT_ScheduleSaveCatagory*pkSaveCatagory = pkSaveActivity->GetScheduleSaveCatagory(iCategory);
		if (NULL == pkSaveCatagory)
		{
			continue;
		}
		ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->GetScheduleSaveEntry(iSchedule);
		if (NULL == pkSaveEntry)
		{
			continue;
		}
		pkSaveEntry->AddActShopScore(-iActShopScore);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		rkAsset.UpdateDirtyFlag();
	}
}

//活动积分兑换
int32_t UT_ServerHelper::CheckActScore(LC_ServerPlayer* pkPlayer, Utility::UT_SIMDataList&kActScorePrices, int32_t iSchedule)
{
	if (kActScorePrices.empty())
	{
		return RE_SUCCESS;
	}
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Utility::UT_SIMDataList::const_iterator iter = kActScorePrices.begin();
	for (; iter != kActScorePrices.end(); ++iter)
	{		
		ACT_ScheduleActivityType iActType = (ACT_ScheduleActivityType)iter->IID();
		int32_t iCategory = iter->ParamA();
		int32_t iActShopScore = iter->ParamB();
		if (iActType <= 0 && iActShopScore <= 0)
		{
			continue;
		}
		Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset.GetScheduleSaveActivity(iActType);
		if (NULL == pkSaveActivity)
		{
			return RE_FAIL;
		}
		ACT_ScheduleSaveCatagory*pkSaveCatagory = pkSaveActivity->GetScheduleSaveCatagory(iCategory);
		if (NULL == pkSaveCatagory)
		{
			return RE_FAIL;
		}
		ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->GetScheduleSaveEntry(iSchedule);
		if (NULL == pkSaveEntry)
		{
			return RE_FAIL;
		}
		if ((int32_t)pkSaveEntry->GetActShopScore() < iActShopScore)
		{
			return RE_FAIL;
		}
	}
	return RE_SUCCESS;
}

void UT_ServerHelper::SendMail(LC_ServerPlayer* pkOwnerPlayer, MailInfo& mail)
{
	if(NULL == pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	if(mail.m_nPlayerID && !mail.m_nReward.empty()) 
	{
		Utility::UT_SIMDataList m_nReward = mail.m_nReward;
		mail.m_nReward.clear();
		UT_ServerHelper::MergeItemList(m_nReward, mail.m_nReward);
		UT_ServerHelper::SendMailToPlayer(pkOwnerPlayer->GetInstance(), mail.m_nMailTitle, mail.m_nMailDesc, mail.m_nReward, false, mail.nLogCode, mail.nExtraParam);
	}
}