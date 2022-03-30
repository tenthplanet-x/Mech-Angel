/********************************************************************
	created:	2013/12/09
	filename: 	UT_ServerHelper.h
	file path:	Src\Utility
	file base:	UT_ServerHelper
	file ext:	h
	author:		sunny

	purpose:
*********************************************************************/
#ifndef _UT_SEVERHELPER_H
#define _UT_SEVERHELPER_H

#include "UT_Func.h"
#include "LC_PlayerBase.h"
#include "UT_ComplexData.h"
#include "LC_HelperDiff.h"
#include "MG_GameProtocol.h"
#include "redis.pb.h"
#include "CF_RankOpen.h"
#include "UT_CSUnorderedMap.h"
#include "CF_RankCondition.h"
class LC_ServerMapLogic_TranscriptionBase;
class LC_ServerMapLogic_Place;
class LC_ServerPlayer;
#define SOLO_DULE_ROBOT_DIR "Data/CSV/CSVServer/DesignCSV/ExportSoloDuleRobot.csv"
enum GuildLoadRewardCacheType
{
	GLRCT_SCORE = 1,
	GLRCT_BUFF = 2,
};
struct RegularBossKillerInfo
{
	RegularBossKillerInfo()
	{
		charName.clear();
		guildName.clear();
	}

	std::string charName;//指挥官名字
	std::string guildName;//公会名字
};
AJSON(RegularBossKillerInfo, (charName)(guildName))

struct RuneStarInfo
{
	int _MaxLevel;
	int _PropertyChange;
	UT_SIMDataList _consumeItems1;
	UT_SIMDataList _consumeItems2;
};

struct AnswerResult
{
	std::vector<int32_t> m_nRankIndex;
	std::vector<int32_t> m_nTimes;
};
AJSON(AnswerResult, (m_nRankIndex)(m_nTimes))

struct RewardCache
{
	RewardCache():eType(0),nTowerId(0),nTimes(0),nGuildId(0),nNextTimeStamp(0),bAddTowerId(false),bSendToSociety(false),bSendToDB(false),bErase(false)
	{
		
	}
	int eType;
	int nTowerId;
	int nTimes;
	uint64_t nGuildId;
	int nNextTimeStamp;
	bool bAddTowerId;
	bool bSendToSociety;
	bool bSendToDB;
	bool bErase;
};

struct MailInfo
{
	void SetMailInfo(StringType& mailTitle, StringType& mailDesc, uint64_t playerID, Utility::UT_SIMDataList& reward)
	{
		m_nMailTitle = mailTitle;
		m_nMailDesc = mailDesc;
		m_nPlayerID = playerID;
		for(Utility::UT_SIMDataList::iterator it=reward.begin(); it!=reward.end(); ++it)
		{
			m_nReward.push_back(*it);
		}
		nLogCode = 0;
		nExtraParam = 0;
	}

	MailInfo()
	{
		m_nMailTitle = "";
		m_nMailDesc = "";
		m_nPlayerID = 0;
		nLogCode = 0;
		nExtraParam = 0;
		m_nReward.clear();
	}

	StringType m_nMailTitle;
	StringType m_nMailDesc;
	uint64_t m_nPlayerID;
	Utility::UT_SIMDataList m_nReward;
	int  nLogCode;
	int  nExtraParam;
};

struct KillBossInfo
{
	KillBossInfo():bossID(0), mapResID(0)
	{
		killerName.clear();
	}

	void clear()
	{
		killerName.clear();		
		bossID = 0;
		mapResID = 0;
	}
	
	StringType killerName;
	int bossID;
	int mapResID;
};

struct GuildBossInfo
{
	GuildBossInfo():m_nBossID(0), m_nState(0), m_nBossHP(0), m_nInvaildTime(0) {}
	
	int32_t                         m_nBossID;
	int32_t                         m_nState;
	int64_t                         m_nBossHP;
	int32_t						   m_nInvaildTime;
};
AJSON(GuildBossInfo, (m_nBossID)(m_nState)(m_nBossHP)(m_nInvaildTime))

struct AnswerAwardCheckTime
{
	mem::vector<uint32_t> time;
};
AJSON(AnswerAwardCheckTime, (time));

struct TestDataBase
{
	mem::vector<int32_t> topic;
};
AJSON(TestDataBase, (topic));

struct RankOpenInfo
{
	RankOpenInfo():iRankId(0),uStartTime(0),uEndTime(0),uClearTime(0),nConfigReward(false),bGuildReward(false),bPersonalReward(false),bClearDbAndSocitey(true)
	{
		vecInId.clear();
		sStartTime.clear();
		sEndTime.clear();
		sClearTime.clear();
	}
	bool InitRankOpenTimeByExecl(const CF_RankOpen::DataEntry* pkData);
	void InitRankOpenIdListByExecl(const CF_RankOpen::DataEntry* pkData);
	void InitRankOpenInfoByExecl(const CF_RankOpen::DataEntry* pkData);
	void LogRankOpenInfo(std::stringstream &logStr)
	{
		logStr<<" id: "<<iRankId<<" uSt: "<<uStartTime<<" uET: "<<uEndTime<<" uCT: "<<uClearTime;
		logStr<<" nCR: "<<nConfigReward<<" bGR: "<<bGuildReward<<" bPR: "<<bPersonalReward;
		logStr<<" sSt: "<<sStartTime<<" sEt: "<<sEndTime<<" sCT: "<<sClearTime;
		for (uint16_t i = 0; i < vecInId.size();++i)
		{
			if( 0 == i)
				logStr <<" < ";
			logStr <<vecInId[i];
			if(i +1 < vecInId.size())
				logStr <<" ,";
			else if(i +1 == vecInId.size())
				logStr <<" > ";
		}
	}
	int32_t  iRankId;
	uint32_t uStartTime;//开始时间
	uint32_t uEndTime;//结束时间 （发奖时间）
	uint32_t uClearTime;//结束时间
	int  nConfigReward;	//	配置发奖标记
	bool bGuildReward;	//	公会发奖标记
	bool bPersonalReward;	//	个人发奖标记
	bool bClearDbAndSocitey ; // 标记已经清过数据
	std::string sStartTime;
	std::string sEndTime;
	std::string sClearTime;
	std::vector<int> vecInId;
 
};
AJSON(RankOpenInfo, (iRankId)(sStartTime)(sEndTime)(sClearTime)(nConfigReward)(bGuildReward)(bPersonalReward)(bClearDbAndSocitey)(vecInId)(uStartTime)(uEndTime)(uClearTime));

struct BossDamageInfo
{
	BossDamageInfo()
	{
		m_nActorDamageInfo.clear();
		m_nPlayerIDToInstanceID.clear();
		m_nGuildDamageInfo.clear();
		m_nActorName.clear();
		m_nGuildName.clear();
	}

	std::map<uint64_t, uint64_t> m_nActorDamageInfo;
	std::map<uint64_t, unique_id_type> m_nPlayerIDToInstanceID;
	std::map<uint64_t, uint64_t> m_nGuildDamageInfo;
	std::map<object_id_type, StringType> m_nActorName;
	std::map<uint64_t, StringType> m_nGuildName;
};
AJSON(BossDamageInfo, (m_nActorDamageInfo)(m_nPlayerIDToInstanceID)(m_nGuildDamageInfo)(m_nActorName)(m_nGuildName));

struct MainTaskPlayerInfo
{
	MainTaskPlayerInfo() : m_nCommanderTitleID(0), m_nTaskID(0)
	{
		m_nCharName.clear();
	}

	int32_t			m_nCommanderTitleID;//指挥官头衔
	int32_t			m_nTaskID; //任务id
	std::string		m_nCharName;//指挥官名字	
};
AJSON(MainTaskPlayerInfo, (m_nCommanderTitleID)(m_nTaskID)(m_nCharName));

struct WatchTowerVaildTime
{
	WatchTowerVaildTime(): m_nTimeLen(0), m_nEndTime(0){}

	int64_t m_nTimeLen;
	int64_t m_nEndTime;
};
AJSON(WatchTowerVaildTime,(m_nTimeLen)(m_nEndTime));

AJSON_CODE(PS_Buddy, (m_nBuddyIndex)(m_nBuddyData)(m_nDamageData)(m_nHurtData)(m_nHealData));
AJSON_CODE(CommanderBaseInfo, (m_nCharName)(m_nControllerLevel)(m_nDamageData)(m_nBornType)(m_nCommanderTitleID)(m_nCommanderTitleFrameID));
AJSON_CODE(SingleSideStruct, (m_CommanderBaseInfo)(m_nVecBuddyInfo));
AJSON_CODE(SingleFightRecord, (m_nTime)(m_nScore)(m_AttackInfo)(m_DefenceInfo));



//using namespace redis_define;
//class ACT_ScheduleEntry;
class UT_ServerHelper : public Memory::MM_BaseObject
{
public:
	/////////////////////////////////////////////////////////////

	struct __ItemCnt
	{
		uint32_t	m_nId;
		uint32_t	m_nCnt;
		uint32_t	m_nBindMode;
	};
	typedef mem::vector<__ItemCnt> LC_ItemCntList;

	struct GuildMapBossHpInfo
	{
		GuildMapBossHpInfo():charTypeID(0), maxHp(0)
		{
			hp2RewardInfo.clear();
		}

		int32_t charTypeID;
		int64_t maxHp;
		mem::map<int64_t, UT_ServerHelper::LC_ItemCntList, std::greater<int64_t> > hp2RewardInfo;
	};

	/////////////////////////////////////////////////////////////
	class _GameNoticeFmort
	{
	public:
		_GameNoticeFmort();
		~_GameNoticeFmort();

		void SetChannelType(int32_t ChannelType) {m_nChannelType = ChannelType;}
		void	SetMsgID(int32_t nMsgID) { m_nMsgID = nMsgID; }
		void 	AddBuddyCID(int32_t buddyCID);
		void 	AddCharType(int32_t charType);
		void	AddString(const StringType& str);
		void	AddNumber(int32_t num);
		void	AddBuddyStarLevel(int32_t starLevel);
		void AddUInt64Number(uint64_t num);
		void AddCollectionRewardName(int32_t nID);
		void AddTitleName(int32_t nID);
		void AddCommanderTitleName(int32_t nID);
		void AddMapID(int32_t nID);
		void AddAchievementName(int32_t nID);
		void AddItemName(int32_t nID);
		void AddItemNames(mem::set<int32_t>& itemID);
		void	AddItemQuality(int32_t nID);
		void	AddSubordinateName(int32_t nID);
		void	AddFlyKicks(int32_t nServerID, int32_t nMapLogicID, int32_t nMapResID, const Utility::UT_Vec3Int& rPos);
		void	AddServerID(int32_t nID);
		void	AddPathFound(int32_t nServerID, int32_t nMapLogicID, int32_t nMapResID, const Utility::UT_Vec3Int& rPos);
		void	AddSendFlower(const unique_id_impl& userid);
		void	AddPlayerInfo(const unique_id_impl& userid, const StringType& name);
		void AddActivityId( int32_t nActivityId );
		void	AddSkyGrade(int32_t num);
	public:
		int32_t		m_nChannelType;
		int32_t		m_nMsgID;
		mem::vector<StringType> m_strArgs;
		mem::vector<int32_t>	m_numArgs;
		mem::vector<uint64_t>	m_i64Args;
		StringStreamType		m_argDesc;
	};
	/////////////////////////////////////////////////////////////
	static ResultType SendMailToPlayer(unique_id_type charID, const StringType& subject, const StringType& content, const Utility::UT_SIMDataList& itemlist, bool bGb2312=false,int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType SendMailToPlayer(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& rewardMsg, bool bGb2312=false,int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, GameLogic::LC_BackPackEntryList& itemlist , const StringType& send_name = "",int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, GameLogic::LC_BackPackEntryPtrList& itemlist , const StringType& send_name = "",int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, LC_ItemCntList& itemlist, const StringType& send_name = "",int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType SendToMail(const StringType& mailname, const StringType& mailcontent, const unique_id_impl& receiveid, const StringType& receivename, int money, const Utility::UT_SIMDataList& itemlist, const StringType& send_name = "",int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType SendToMailGuild(const StringType& mailname, const StringType& mailcontent, uint64_t guildId, int money, LC_ItemCntList& itemlist , const StringType& send_name = "",int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType SendToMailAll(const StringType& mailname, const StringType& mailcontent, int money, GameLogic::LC_BackPackEntryList& itemlist, int32_t iServerID, const StringType& send_name = "",int nLogCode = 0 ,int nExtraParams = 0);
	static ResultType GM_SendToMailAll(const StringType& mailname, const StringType& mailcontent, int money, GameLogic::LC_BackPackEntryList& itemlist, int32_t iServerID, const StringType& send_name = "");
	static ResultType AddPlaceHolderEffect(int map_logic_id, int effect_id, int second, const Utility::UT_Vec3Int& pos);
	static LC_ServerMapLogic_Place* GetPlayerPlace(LC_ServerPlayer* player);
	static ResultType BrocastMsgStrServer(const StringType& msg, PromptChannel msgchannel, PromptLevel msglevel, int32_t nServerID = 0);
	static int GetServerId(void);
	static int GetSrcServerId(LC_ServerPlayer* player);
	static bool CheckLocal(int serverid);
	static bool CheckLocal(LC_ServerPlayer* player);
	static bool CheckLocal(LC_ServerPlayer* player, int32_t nServerId);
	//判断玩家是否跨服
	static bool IsInCross(LC_ServerPlayer* player);
	static bool IsInCross(const unique_id_impl& id);
	static bool GetFrontierLocation(LC_ServerPlayer* player,UT_Vec3Int &Location,int32_t& MapResID);
	static bool GetRandomLocationXYZ(UT_Vec3Int &outPos,int mapId,int Citizenship,bool CrossFlag = false);
	static object_id_type QueryPlayerID(const unique_id_impl& charID);//从角色信息查询玩家信息
	static bool CanCreateTeamInCross(LC_ServerPlayer* player);
	static int GetActiveId(LC_ServerPlayer* player);
	static int GetActiveId(int nServerId = 0);
	static int GetServerCroup(int nServerID);
	static int GetCroupServerIndex(mem::vector<int>& idVector, int iPickType, int nServerID);

	static bool GetGameNoticeFromString(const StringType& strContent, UT_ServerHelper::_GameNoticeFmort& noticeFmort);

	static void NoticeNpcInfoToPlayer(LC_ServerPlayer* pkOwnerPlayer, int32_t storyID, int32_t mapResID);
	static void BroadcastNpcInfoToPlayer(int32_t storyID);
	
	static bool CheckStoryIsActive(int32_t storyID);
	static uint32_t GetStoryStartTime(int32_t storyID);
	static uint32_t GetStoryEndTime(int32_t storyID);
	static void GetTopicVaildTime(int topicIndex, uint32_t& minTime, uint32_t& maxTime);
	static uint32_t GetCheckAwardTime(int topicIndex);

	static bool SyncBasicPackToPlayer(int32_t reasonType);
	static ResultType HandleBonusItems(int32_t iPackType, const Utility::UT_SIMDataList& items, GameLogic::LC_PlayerBase* pkPlayer, int32_t iReasonType, uint64_t iReasonDetail, const StringType& title, const StringType& Desc, bool toMail = true, bool bAutoUse = false, bool bLotteryLogic = false,int nLogCode = 0 ,const vector<int> &Params=vector<int>());

	static void HandlePackOPRecord(LC_ServerPlayer* player, GameLogic::PackOPRecordSet* pkPackOpRecords, int32_t iReasonType, uint64_t iReasonDetail);

	static void TryAutoUseItem(LC_ServerPlayer* player, uint32_t nBuddyId, int32_t nItemId, int nPackType, int nSlot, int nCount);

	static void FillGameNoticeMsg(const _GameNoticeFmort& msgFmort, MG_GameNotice& notifymsg);
	static ResultType BrocastNoticeMsg(const _GameNoticeFmort& msgFmort, uint64_t nServerID = 0, LC_ServerPlayer* player = NULL, LC_ServerMapLogic_Place* pLogicPlace = NULL);

	static int32_t GetFactionDefServer();

	static void MergeItemList(const Utility::UT_SIMDataList& srcItemList, Utility::UT_SIMDataList& mergedItemList);

	static bool CheckDoReward(int32_t IID, LC_ServerPlayer* pkPlayer);

	static bool AddOnLinePlayer(LC_ServerPlayer* player);
	static bool RemoveOffLinePlayer(LC_ServerPlayer* player);
	static bool ResetOnLinePlayer();

	static int32_t	GetActivityEndTime(int scheduleID);
	static bool	CheckActivityVaild(int scheduleID);

	static bool SpawnNPC(LC_ServerPlayer* pkPlayer, int32_t npcID);

	static ResultType GetDisassembleItem(LC_ServerPlayer* pkPlayer, int32_t itemID, Utility::UT_SIMDataList& kItems, mem::map<int32_t, int32_t>& itemInfo);

	static void LoadMapIDAndMirrorTaskID();
	static const mem::map<int, int>& GetMapIDAndMirrorTaskIDRecords();
	static int GetMirrorTaskIDByMapID(int mapID);

	static bool CheckNeedUpdate(int32_t type, uint64_t oldValue, uint64_t newValue);
	static uint64_t GetNewDamageData(int32_t type, uint64_t oldValue, uint64_t newValue);

	static uint32_t GetExportRobotMaxNum(int robotType);
	static char* GetRedisNameSpace(char* a, int b);

	// wrapper for redis io 
	static bool RedisCreateData( const char* szNameSpace, mem::vector<uint64_t>& nKeys, bool bGlobalServer = false );
	static bool RedisGetData_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, std::string& strResult, bool bGlobalServer = false );
	static bool RedisSetData_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, const std::string& strInput, bool bGlobalServer = false, bool bForceSet = false);
	static bool RedisEraseData_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, bool bGlobalServer = false );
	static int RedisGetData_StringRange( const char* szNameSpace, mem::map<uint64_t, std::string>& result, bool bGlobalServer = false );
	static bool RedisCopy_String(const char* szNameSpace, const char* destNameSpace, bool bGlobalServer = false);

	static bool RedisGetData_Int( const char* szNameSpace, mem::vector<uint64_t>& nKeys, int& nResult, bool bGlobalServer = false );
	static bool RedisSetData_Int( const char* szNameSpace, mem::vector<uint64_t>& nKeys, int nInput, bool bGlobalServer = false );
	static bool RedisRemoveData_Int(const char* szNameSpace, mem::vector<uint64_t>& nKeys, bool bGlobalServer = false);
	static int RedisGetData_IntRange( const char* szNameSpace, mem::map<uint64_t, uint64_t>& result, bool bGlobalServer = false );


	static bool RedisList_AddInt(const char* szNameSpace, uint64_t nInput, bool bGlobalServer = false);
	static bool RedisList_RemoveInt(const char* szNameSpace, uint64_t nInput, bool bGlobalServer = false);
	static bool RedisList_GetRangeInt(const char* szNameSpace, mem::vector<uint64_t>& vecRangeData, bool bGlobalServer = false);

	static bool RedisGetData_Int_EntryServerID( const char* szNameSpace, mem::vector<uint64_t>& nKeys, int& nResult);
	static bool RedisGetData_Int_EntryServerID_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, std::string& strResult);
	static bool RedisSetData_Int_EntryServerID_String( const char* szNameSpace, mem::vector<uint64_t>& nKeys, const std::string& strInput);
	static bool ClearRedisData_By_EntryServerID( const char* szNameSpace);
	// redis rank operation
	static bool RedisRank_WriteElement( const char* szNameSpace,  uint64_t nCharID,  uint64_t nScore, bool bGlobalServer = false );
	static bool RedisRank_ReadElement( const char* szNameSpace,  uint64_t nCharID,  uint64_t& nScore, int32_t& nIndex, bool bGlobalServer = false, bool addSnapShot = false);
	static bool RedisRank_EraseElement(const char* szNameSpace,  uint64_t nCharID, bool bGlobalServer = false);
	static int RedisRank_ReadSimpleRange( const char* szNameSpace, mem::vector<uint64_t>& vecRangeData, bool addSnapShot=false, int32_t nTopCount = -1, bool bGlobalServer = false );
	static bool RedisRank_Clear( const char* szNameSpace, bool bGlobalServer = false);
	static bool RedisRank_CreateSnapshot( const char* szNameSpace, bool bGlobalServer = false );

	static bool RedisWriteString(const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::vector<uint64_t>& nKeys, const std::string& strInput, bool bGlobalServer = false);
	static int RedisReadStringRange( const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::map<uint64_t, std::string>& result, bool bGlobalServer = false );
	static bool RedisGetStringData( const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::vector<uint64_t>& nKeys, std::string& strResult, bool bGlobalServer = false );
	static bool RedisClearString( const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, bool bGlobalServer = false);
	static bool RedisRemoveOneKey(const char* szNameSpace, mem::vector<uint64_t>& nNameKeys, mem::vector<uint64_t>& nKeys, bool bGlobalServer = false);

	static bool RedisRank_ClearDamageRecord(const char* szNameSpace, mem::vector<uint64_t>& params, bool bGlobalServer = false);
	static bool RedisRank_EraseDamage( const char* szNameSpace, mem::vector<uint64_t>& params, uint64_t nCharID, bool bGlobalServer = false );
	static bool RedisRank_ReadDamage( const char* szNameSpace, mem::vector<uint64_t>& params, uint64_t nCharID,  uint64_t& nScore, int32_t& nIndex, bool bGlobalServer = false );
	static bool RedisRank_WriteDamage( const char* szNameSpace, mem::vector<uint64_t>& params, uint64_t nCharID,  uint64_t nScore, bool bGlobalServer = false );
	static int RedisRank_ReadSimpleDamageRange( const char* szNameSpace, mem::vector<uint64_t>& params, mem::vector<uint64_t>& vecRangeData,  int32_t nTopCount = -1, bool bGlobalServer = false );
	static bool RedisSetData_DamageString( const char* szNameSpace, mem::vector<uint64_t>& params, mem::vector<uint64_t>& nKeys, const std::string& strInput, bool bGlobalServer = false, bool bForceSet = false);
	static bool RedisGetData_DamageString( const char* szNameSpace, mem::vector<uint64_t>& params, mem::vector<uint64_t>& nKeys, std::string& strResult, bool bGlobalServer = false );
	static bool RedisClearData_DamageString( const char* szNameSpace, mem::vector<uint64_t>& params, bool bGlobalServer = false);

	static std::string GetRedisStr(const char* szNameSpace, mem::vector<uint64_t>& params, bool bGlobalServer);
	static std::string GetRedisStr(const char* szNameSpace, bool bGlobalServer, bool snapShot=false);
	static std::string GetRedisStr_EntryID(const char* szNameSpace);

	static void SettleYiZhongJiangLinReward();
	static void SendGuildMail(const MG_RESP_SendMailToGuild& msg);
	static void UpdateSocietySendMailMsg(const MG_REQ_SendMailToGuild& msg);
	static void SendMailMsgToSociety();

	static void SettleYiZhongQinXiGameStory();
	static void SettleYiZhongQinXi(int storyID, bool init);

	static int32_t RandomTestDatabase();
	static void GetTestDatabase(std::vector<int32_t>& topicID);
	
	static void SettleAnswer(bool init=false);
	static void SetAnswerPlayer(mem::vector<uint64_t>& playerIDs);
	static void GetAnswerPlayer(mem::vector<uint64_t>& playerIDs);
	static void ClearAnswerPlayer();
	static void SetAnswerCloseState(int nState);
	static int  GetAnswerCloseState();
	static void SettleYiJiTuXi(bool init=false);
	static void ClearAnswerRedisRank();
	static void UpdateBattlePassStartTime(uint64_t key, int32_t startTime);
	static int32_t GetBattlePassStartTime(uint64_t key);

	static void UpdateBattlePassEndTime(uint64_t key, int32_t endTime);
	static int32_t GetBattlePassEndTime(uint64_t key);

	static int32_t GetSubSaveTime();
	static void SetSubSaveTime(int32_t time);

	static int32_t GetSubPlusSaveTime();
	static void SetSubPlusSaveTime(int32_t time);

	static int32_t GetSevenDayTrailEndTime();
	static void SetSevenDayTrailEndTime(int32_t time);

	static int32_t GetInvestmentEndTime(int scheduleID);
	static void SetInvestmentEndTime(int scheduleID, int time);

	static int32_t GetYiZhongQinXiGameStoryEndTime(int storyID);
	static void SetYiZhongQinXiGameStoryEndTime(int storyID, int32_t time);

	static bool GetYiZhongQinXiPlayer(int storyID, mem::vector<uint64_t>& playerIDs);
	static void SetYiZhongQinXiPlayer(int storyID, mem::vector<uint64_t>& playerIDs);

	static void SetYiZhongQinXiGameEndTime(int32_t time);
	static int32_t GetYiZhongQinXiGameEndTime();

	static bool CheckRecordDestActorDamage(LC_ActorBase* pkSrcActor, LC_ActorBase* pkDestActor);

	static bool CheckHuanJingVaild();
	static void SetHuanJingEndTime(int32_t storyID, int32_t endTime);
	static void CheckSettleHuanJingDamage();
	static void SettleHuanJingDamage(int32_t storyID);

	static void SetAnswerActivityEndTime(int32_t time);
	static int32_t GetAnswerActivityEndTime();

	static void SetLoadTime(uint32_t loadTime);
	static uint32_t GetLoadTime();

	static ResultType CheckOperGuild();

	static void UpdateTestDatabase(const std::vector<int32_t>& data);
	static int32_t SetTestDatabase();
	static int32_t GetTopicIDFromTopicIndex(int32_t index);

	static void SetCheckAwardTimeToRedis(mem::vector<uint32_t>& time);
	static bool SetCheckAwardTime();
	static const mem::vector<uint32_t>& GetCheckTime();
	
	static void GetPlayerDamageData(LC_ServerPlayer* pkOwnerPlayer, int storyID, uint64_t charID);
	static void GetDetailDamageData(LC_ServerPlayer* pkOwnerPlayer, int storyID, uint32_t time, uint64_t charID);

	static void GetGuildDamageRankFromRedis(LC_ServerPlayer* pkOwnerPlayer, int storyID, int minIndex, int maxIndex);
	static void GetPlayerDamageRankFromRedis(LC_ServerPlayer* pkOwnerPlayer, int storyID, int minIndex, int maxIndex);

	static void GetGuildTotalDamageInfo(uint64_t storyID, uint64_t guildID, PS_GuildDamageInfo& data);
	static void GetPlayerTotalDamageInfo(uint64_t storyID, uint64_t charID, PS_SettleInfo& data);

	static bool CheckChangeServer(GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicType, bool bCertainSameServer);

	static void UpdateLimitRewardRecord(int key, int value);
	static void GetAllLimitRewardRecord(mem::map<int, int>& record);
	static int GetLimitRewardRecordByID(int id);

	static void UpdateLimitRewardRecordB(int key, int value);
	static int GetLimitRewardRecordBByID(int id);

	static void RemoveLimitActivityB(int id);
	static void AddLimitActivityIDB(int id);
	static void CreateLimitActivityB();

	static void RemoveLimitActivity(int id);
	static void CreateLimitActivity();
	static void AddLimitActivityID(int id);

	static void SetSubBEndTime(int groupID, uint32_t endTime);
	static void GetSubBEndTime(mem::map<int, uint32_t>& subBEndTime);
	static const mem::map<int, uint32_t> & GetSubBEndTimeRef();
	static void ClearSubBEndTime(int groupID);
	static bool GetSubBData(LC_ServerPlayer* pkPlayer, int type, const Utility::UT_SIMDataList& paramList, int param, uint32_t& value, uint32_t& result);

	static void DoUpdateRankInfoToRedis(LC_ServerPlayer* pkOwnerPlayer, int type, int param, uint32_t newValue);
	static void GetRankValueNameSpace(int type, int key, std::string& space);
	static void GetRankPlayerInfoNameSpace(int type, int key, std::string& space);
	static void GetRankClearNameSpace(int type, int key, std::string& space);
	static bool GetPlayerRankRangeFromRedis(int type, int key, mem::vector<uint64_t>& playerIDVec);
	static void DoSendRankRewardToPlayer(int type, int key, const mem::vector<uint64_t>& playerIDVec, int xlsxType);
	

	static void RankSubB(int groupID);
	static void SettleSubB();

	static void SetSubBClearTime(int groupID, uint32_t time);
	static void CheckClearSubB();

	static void CheckRankSubB();
	static bool CanRank(int groupID);
	static void UpdateRankRecord(int groupID);

	static CF_RankCondition::DataEntry* GetRankConditionPtrByType(int32_t type);
	static ResultType GetSubBRankInfo(LC_ServerPlayer* pkOwnerPlayer, bool active, int type, const char* szNameSpace1, const char* szNameSpace2, int minIndex, int maxIndex, MG_RLT_GetRankSubBInfo& msg);

	static void InitBestGuildInfo();
	static bool IsBestGuildStoryID(int storyID);
	static bool IsBestGuildTaskScheduleID(int scheduleID);
	static void SetBestGuildEndTime(int32_t endTime);
	static int32_t GetBestGuildEndTime();
	static void SettleBestGuildReward();
	static void GameServerAppToSendBestGuildReward1(bool tag);
	static void GameServerAppToSendBestGuildReward2(bool tag);
	static void UpdateBestGuildActivityEndTime(int storyID, int32_t endTime);
	static void SettleBestGuildActivityReward();

	static bool IsShengYuDiGongStoryID(int storyID);

	static void SendGuildPresentExpInfoToSociety();
	static void TryExportRobotToGroup();
	static void SetExportRobotFlag(int32_t robotType, int32_t exportState);
	static void InitExportRobot();
	static void GetRobotIDVecByRobotType(int32_t robotType, mem::vector<uint32_t>& idVec);

	static void InitCSV();
	static bool IsEscortNPC(int charType);
	static bool IsEscortTask(int taskID);
	static bool GetGoalVec(int type, mem::vector<int>& goalIDVec);

	static void LoadServerFirstKillBossInfo();
	static bool CheckBossBeKilled(int npcID);
	static void UpdateServerFirstKillBossPlayerInfo(int npcID, std::string& playerName, int playerLevel, int commanderTitle, int profilePictureID, int commanderTitleFrameID);
	static void UpdateServerFirstKillBossRewardPlayerInfo(LC_ServerPlayer* pkPlayer, int npcID, std::string& playerName, int playerLevel, int commanderTitle, int profilePictureID, int commanderTitleFrameID);
	static void UpdateServerFirstKillBossRewardPlayerInfo(int npcID, const std::string& info);

	static bool GetServeFirstKillBossInfo(int npcID, struct BossFirstKillPlayerInfo& info);

	static void UpdateWatchTowerInfoCache(uint64_t id, const std::string& str);
	static void UpdateWatchTowerInfoCacheToRedis();

	static void UpdateWatchTowerTime(bool vaild, int64_t timeLen, int64_t endTime);
	static bool CheckWatchTowerInfoValid(int64_t timeLen, int64_t endTime);
	static bool GetWatchTowerInfoValidFlag();

	static void LoadWatchTowerFromRedis(bool vaild);
	static void LoadWatchTowerFromRedis_finish();
	static void UpdateWatchTowerFromSaveState();

	//非global节点调用这个API，从redis加载哨塔数据
	static bool UpdateWatchTowerFromRedis(bool broadcast=false);

	static void LoadBossDamageInfoFromRedis(bool vaild);
	static void UpdateBossDamageInfo(LC_NPCBase* pkNpc);

	static uint32_t GetBestGuildRankClearTime();

	static void SetRewardTaskEndTime(int32_t time);
	static int32_t GetRewardTaskEndTime();

	static void GetRealtimeCombatRecorder(LC_ServerPlayer* pkOwnerPlayer);

	static uint64_t GetActorDamageData(LC_ServerPlayer* pkOwnerPlayer, object_id_type id, int32_t type);

	static void AddMail(MailInfo& info);
	static void UpdateMail();

	static void GetSuffixByType(StringType &outStr, int32_t nType,int32_t nStoryId);
	static void GetRedisRankPonitNameByType(StringType &outStr, int32_t nType,int32_t nStoryId);
	static void GetRedisRankPlayerInfoNameByType(StringType &outStr, int32_t nType,int32_t nStoryId);
	static uint64_t MakeValueByTimeAndPoint(uint32_t uPoint);
	static void MakeGuildOPersonalInfoToJson(std::string &outStr,uint64_t uPoint,std::string &sGuildName,std::string &sPlayerName , int32_t nTitleId,uint64_t nGuild);
	// nStoryId  	GFRT_TOTAL		= 0,  
	static void CommitPersonalPointByStageType(int32_t nStoryId,uint64_t uCharID ,uint32_t uPoint,std::string &sGuildName,std::string &sPlayerName, int32_t nTitleId);
	static void CommitGuildPointByStageType(int32_t nStoryId,uint64_t uGuildID ,uint32_t uPoint,std::string &sGuildName);

	static void UpdatePlayerInfoGuildName(uint64_t uId,std::string sPlayerName, std::string sNewGuildName,uint64_t uGuildMasterId,uint64_t nGuild,int nTitleId);
	static int GetAccountPointRankIndexFromRedisByTypeAndStoryId(uint64_t guildId, uint64_t playerID, int32_t type, int storyID);

	static void GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(mem::vector<uint64_t>& allIds,int32_t nType,int32_t nStoryId);
	static bool Pack_Single_info(StringType &redisPlayerInfoStr,int32_t nIndex,uint64_t uId,PS_RANK_STRUCT& outInfo);
	static bool SendGuildFightPersonalRewardByStoryId(int32_t nStoryId);
	static void SendSocietyGetMemberToGuildRankReward(int32_t id);
	static bool SendGuildFightGuildRewardByStoryId(uint64_t nGuild, int32_t rankId,int32_t index ,std::map<uint64_t,uint32_t> & memberLevel);
	static bool SendSkyArenaReward(uint64_t nCharId, int32_t nRankId, int nIndex, int nScore, int nGrade);
	static bool SendSummitArenaRiseUpMail(uint64_t nCharId, int nType,int nStageType);
	static bool SendSummitArenaRankAwardMail(uint64_t nCharId, int32_t nType, int nIndex);
	static bool SendSummitArenaGroupStageMail(uint64_t nCharId);
	static bool SendSummitArenaGloryRankRewardMail(uint64_t nCharId, int32_t nType, int32_t nTimes);
	static void ClearGuildFightRedisInfo(int32_t nId); 
	static void ClearGuildFightRedisInfoByStoryIdAndType(int32_t nStoryId,int8_t nType); 
	
	static int32_t GetSkyArenaRankPtrByScore(int32_t score);
	static int32_t GetSchemeGroupNum(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID);
	static void GetSchemeLinkID(uint32_t mapID, std::vector<uint32_t>& schemeLinkID);
	static bool IsSummitArenaMap(uint32_t mapID);
	static void SendGuessMailToPlayer(uint32_t guessType, bool success, std::string name, uint32_t summitType, uint32_t againstID, std::vector<uint64_t>& playerIDVec);

	static ResultType CheckBuyTypeVaild(uint32_t buyType);

	static void BroadCastRecoverTimesMsg(LC_ServerPlayer* pkPlayer, int32_t dropType);

	//
	static std::string getDateString(time_t timestamp);
	static void LoadRankOpenInfoByExcelToRedis();
	static void UpdateRankOpenInfo();
	static void ComitRankInfo(RankOpenInfo &info);
	static bool GetRankInfoById(uint32_t Id,RankOpenInfo &info);
	static void UpdateCacheRankOpen();

	static bool TrySwitchMap(LC_ServerPlayer* pkPlayer, int32_t eInOutType, int32_t lTargetMapLogicType, int32_t srcMapResID, int32_t destMapLogicID, int32_t destMapResID, int32_t x, int32_t y, int32_t z, bool bQuitHookMap2CommonMap, bool bCertainSameServer);
	
	static void BroadcastBeFightedToPlayer(unique_id_impl uCharId);

	static void UpdateSinglePlayerSingleBuffId(uint64_t uCharId,uint32_t buffId,bool add);
	static void UpdateSingleBuffIdToGuildAllMember(uint64_t GuildId,uint32_t buffId,bool add);
	static void AddAllBuffIdToGuildAllMember(uint64_t GuildId);
	static void AddAllBuffIdToSingleGuildMember(uint64_t GuildId,uint64_t uCharId);
	// 公会战 抢夺哨塔
	static void CommitWatchTower(uint64_t id);

	static bool IsRegularBossMap(int32_t mapID, int& groupID);
	static void InitRegularBossKillerInfo();
	static void UpdateRegularBossKillerInfo(uint64_t key, RegularBossKillerInfo& info);
	static ResultType GetRegularBossKillerInfo(int npcCharType, int mapID, std::string& guildName, std::string& playerName);

	static void SyncKillPlayerInfo(LC_ServerPlayer* pkPlayer);
	static void UpdateKillPlayerInfo(int mapID, int npcCharID, LC_ServerPlayer* pkPlayer);

	// 离线数据 请求 gs->db
	static void ReqPlayerOfflineToDb(OfflinePlayerFromDBType eType,std::vector<uint64_t>& vecId,uint32_t uStoryId,uint32_t uScore,uint32_t uCacheId = 0,uint32_t userId = 0,uint64_t uInstance = 0);

	// 从 Society更新 及 请求 公会战相关信息 
	static void ReqGuildFightInfoToSociety(UpdateGuildFightType eType,uint64_t msgUniqueKey,uint64_t uId,bool bAdd,int32_t uTowerId,uint32_t uCacheId,std::map<uint32_t, uint32_t>& m_score);
	// rank 通知 society db 更新rank_id 及开启结束时间 以及入榜 id  和 清榜
	static bool ReqRankToDbSociety(GameToDbSocietyRankType eType,mem::vector<uint64_t>& vecId,std::vector<int>& vecInId,uint32_t uId,uint32_t uStartTime = 0,uint32_t uEndTime = 0);

	static void AddSendRewardCache(GuildLoadRewardCacheType eType,int towerID, uint64_t guildID, int times=1,int nNextTimeStamp = 0,bool bAddTowerId = false);
	static void ApplySendReward();
	static void EraseSendRewardCache();
	static void SetSendRewardSendState(int id,bool bStat,bool bSociety = false);
	static void SetEraseRewardSendState(int id);

	static void SetKillBossInfo(const StringType& killerName, int bossID, int mapResID);
	static void GetKillBossInfo(StringType& killerName, int& bossID, int& mapResID);
	static void ClearKillBossInfo();

	static void InitBestGuild();
	static void InitRegularBoss();
	static void InitEscortReward();
	static void InitBestGuildGoal();
	static void InitTreasurePos();
	static void InitBuddy();

	static void InitRuneStar();
	static bool GetRuneCSVInfo(int starType, int starLevel, int& maxLevel, Utility::UT_SIMDataList& consumeItems, int& consumeItemID1, int& consumeItemCount, int& consumeItemID2);

	static void GetTreasureMapVec(int playerLevel, mem::map<int, mem::vector<UT_Vec3Int> >& MapVec);

	static void UpdateGuildMapBossState(int32_t mapID, uint64_t guildID, PS_GuildBossInfo& data, int32_t oldState);
	static void InitGuildMapBossHP();
	static ResultType CheckAddGuildMapBossReward(int mapID, int charTypeID, int64_t oldHp, int64_t currentHp, uint64_t guildID, mem::vector<int>& rewardIndex);
	static bool GetGuildMapReward(int mapID, int rewardIndex, UT_ServerHelper::LC_ItemCntList& rewardInfo);
	static int64_t GetGuildMapBossMaxHp(int mapID, int bossID);
	static void GetGuildMapBossReward(std::vector<int32_t>& rewardIndex);
	static void SendGuildMapBossReward(int mapID, uint64_t guildID, const mem::vector<int>& rewardIndex);

	static void SaveGuildMapBossInfo(int32_t mapID, uint64_t guildID, const PS_GuildBossInfo& data);
	static void LoadGuildMapBossInfo();
	static int64_t GetGuildMapBossHP(int mapID, uint64_t guildID, int bossID);
	static void DeleteGuildMapBossHP(int mapID, uint64_t guildID);

	static ResultType CheckGetRestartReward(LC_ServerPlayer* pkPlayer);
	static void UpdatePlayerRestartRewardState(uint64_t charID, bool broadcast=false);

	static void LoadOfflineAnswerResultFromRedis();
	static void FlushOfflineAnswerResult(LC_ServerPlayer* pkPlayer);
	static void UpdateOfflineAnswerResult(uint64_t charid, int32_t rankIndex);
	static void EraseOfflineAnswerResult(uint64_t charid);
	

	static void UpdateRankList(int rankType, const std::vector<uint64_t>& idList);
	static int GetRankIndexByType(int rankType, uint64_t charID);
	static void RankReward(int rankType, bool check);

	static int32_t GetSubCombatType(int32_t subType);

	static int GetRankRewardState(int rankType);
	static void SetRankRewardState(int rankType);
	static void SendTranscribeFightByUniqueId(LC_ServerPlayer* player,uint64_t uUniqueId );

	static void GetGameStoryBatch(LC_ServerPlayer* pkOwnerPlayer, int resMapID);
	

	static void		InitSpecialTask();
	static bool	CheckTaskMatch(int taskSystem, int taskID);
	//**************************
	//
	static bool GenDetailKey( mem::vector<uint64_t>& nKeys, std::string& strKey );
	static std::string GenDetailKeyWithoutData( mem::vector<uint64_t>& nKeys);

	///**************************
	static bool NotifyServerErrorCode( LC_ServerPlayer* pkPlayer, uint32_t nErrorCode);

	static void WheelFightReq(uint64_t uniqueID,uint64_t uChallengeId,uint64_t uBeChallengeId);
	//////////////////////////////////////////////////////////////////////////
	static uint64_t GetGuildMapStateMutableParameter(uint64_t nGuildId);
	static void GetAllGuildMapStateMutableParameterFromRedis();
	static uint32_t AddGuildMapStateMutableParameter(uint64_t nGuildId, int nAddValue);
	static void ClearGuildMapStateMutableParameter();
	static uint64_t GetGuildBuyItemTimes(uint64_t nGuildId);
	static void AddGuildBuyItemTimes(uint64_t nGuildId, int nAddValue);
	/////
	static void SendAnswerGuildRankAward();
	static void AddGuildAnswerPoint(uint32_t nValue, uint64_t nGuildId, const StringType& sGuildName);
	static int32_t GetGuildAnswerPointByGuildId(uint64_t nGuildId);

	static int32_t GetUserAnswerPointByCharId(uint64_t nCharId);

	static int32_t CheckActScore(LC_ServerPlayer* pkPlayer, Utility::UT_SIMDataList&kActScorePrices, int32_t iSchedule);
	static void PayActScore(LC_ServerPlayer* pkPlayer, Utility::UT_SIMDataList&kActScorePrices, int32_t iSchedule);

	static void SendMail(LC_ServerPlayer* pkOwnerPlayer, MailInfo& mail);

private:
	static bool m_nSettleYiZhongQinXiGameStory;
	static mem::map<uint64_t, int32_t> battlePassStartTime;
	static mem::map<uint64_t, int32_t> battlePassEndTime;
	static int32_t subPlusSaveTime;
	static int32_t subSaveTime;
	static int32_t sevenDayTrailEndTime;
	static mem::map<int32_t, int32_t> investmentEndTime;
	static mem::map<int32_t, int32_t> YiZhongQinXiGameStoryEndTime;
	static mem::map<int32_t, mem::vector<uint64_t> > YiZhongQinXiRankPlayers;
	static int32_t YiZhongQinXiGameEndTime;
	static mem::map<int32_t, int32_t> HuanJingEndTime;
	
	static TestDataBase TestData;
	static mem::vector<uint64_t> AnswerPlayers;
	static int32_t AnswerActivityEndTime;
	static bool initAnswer;
	static uint32_t LoadAnswerInfoTime;
	static bool m_nSettleAnswer;
	static AnswerAwardCheckTime CheckAwardTime;

	static bool loadYiJiTuXi;

	static mem::map<int, int> limitRewardRecord;
	static mem::set<int> limitActivityID;

	static mem::map<int, int> limitRewardRecordB;
	static mem::set<int> limitActivityIDB;

	static bool initSubB;
	static mem::map<int, uint32_t> subBEndTime;

	static mem::map<int, uint32_t> subBClearTime;
	static mem::set<int32_t> rankGroupID;

	static mem::map<int, int> mapIDAndMirrorTaskIDRecords;

	static bool loadServerFirstKillBossInfo;
	static mem::map<int, struct BossFirstKillPlayerInfo> serverFirstKillBossRecord;	

	static bool initGuildStoryID;
	static int32_t bestGuildEndTime;
	static bool gsAppSendMail1;
	static bool gsAppSendMail2;
	static mem::map<int, int32_t> bestGuildActivityEndTime;

	static bool initRegularBossKillerInfo;
	static mem::map<uint64_t, RegularBossKillerInfo> regularBossKillerInfo;
	static mem::map<int32_t, int32_t> regularBossMapGroupInfo;

	static mem::map<uint64_t, std::string> bossKillerInfo;

	static mem::map<uint64_t, std::string> watchTowerStateResult;
	static mem::map<uint64_t, std::string> bossDamageInfo;

	static bool guildPresentExpInfoFlag;

	static bool initCSV;
	static mem::vector<int> escortNPCVec;
	static mem::vector<int> escortTaskVec;
	static mem::map<int, mem::vector<int> > goalTypeAndID;

	static int32_t rewardTaskEndTime;
	static mem::map<uint64_t, std::string> watchTowerInfoCache;

	static mem::vector<int> bestGuildStoryID;
	//static mem::vector<int> bestGuildTaskScheduleID;

	static bool watchTowerInfoFlag;
	static KillBossInfo killBossInfo;

	static mem::map<int, mem::map<int, RuneStarInfo> > runeStarInfo;

	//map<指挥官等级最大值，map<地图id, 坐标>>
	static mem::map<int, mem::map<int, mem::vector<UT_Vec3Int> > > playerLevel2TreasuerPos;

	static mem::map<int, UT_ServerHelper::GuildMapBossHpInfo> guildMapBossHpInfo;
	static mem::vector<int> guildMapBossHpReward;

	//map<mapResID, map<guildID, GuildBossInfo>>
	static bool loadGuildMapBossInfo;
	static mem::map<int, mem::map<uint64_t, GuildBossInfo> > guildMapBossInfo;

	static int32_t gameStartTime;
	static mem::set<uint64_t> restartRewardPlayerID;

	static std::map<int, mem::map<uint64_t, int> >  rankList;

	static mem::map<uint64_t, AnswerResult> offlinePlayerAnswerResult;//离线的玩家的答题排名 key:charid  value-key:rankIndex value-value:times
public:
	static int32_t m_nBuddyCount;//buddy的总数量

	static mem::map<int, mem::set<uint32_t> > subRewardPlayerIDs;
	static mem::map<int, mem::set<uint32_t> > subBRewardPlayerIDs;
	static mem::map<int,int>  rankIdCache;
	static bool gbOpenFlag;
	static uint32_t autoRewardNum;
	static mem::map<uint32_t, RewardCache>  RewardCacheMap; // key 哨塔id
	static mem::vector<MailInfo> MainInfo;
	
	static mem::map<uint64_t,SingleFightRecord>  gCacheGuildDefendMap; // key 为主动攻击者 借助主动攻击者离线 结算
	static mem::map<uint32_t,RankOpenInfo> gRankOpenInfoMap;

	static mem::map<int32_t, int32_t> exportExportFlag;//机器人导入状态
	static mem::map<int32_t, mem::vector<uint32_t> > exportRobotInfo;//CF_ExportSoloDuleRobotInfo的id  key:robotType
	static mem::map<int32_t, mem::vector<int> > specialTaskInfo;//CF_SpecialTask

	static std::vector<MG_REQ_SendMailToGuild> sendMailSocietyMsg;
	static mem::map<uint64_t, uint64_t > m_nGuildMapStateMutableParams;
	static mem::map<uint64_t, uint64_t > m_nGuildBuyItemTimes;
 };

////////////////////////////////////////////////////////////////////////////////
namespace GameLogic 
{
	class LC_PlayerBase;
}


struct PlayerACTSubInfo
{
	PlayerACTSubInfo() : controller_level(0), controller_combatScore(0), subvehicle_combatscore(0), subdevaeye_combatscore(0), subwing_combatscore(0), pet_combatscore(0){}

	int controller_level;//指挥官等级
	uint64_t controller_combatScore;//玩家总战力
	uint64_t subvehicle_combatscore;//坐骑战力
	uint64_t subdevaeye_combatscore;//宠物战力
	uint64_t subwing_combatscore;//背饰战力
	uint64_t pet_combatscore;//玩具总战力
};
AJSON(PlayerACTSubInfo, (controller_level)(controller_combatScore)(subvehicle_combatscore)(subdevaeye_combatscore)(subwing_combatscore)(pet_combatscore))

struct SettleInfo
{
	SettleInfo():controllerLevel(0), damageData(0), bornType(0), commanderTitleFrameID(0), commanderTitle(0)
	{
		buddyIndexs.clear();
		charName.clear();
		buddyIDs.clear();
		buddyStarLevels.clear();
		buddyRuneStarLevelInfo.clear();
		equipSlotStarLevel.clear();
		equipItemID.clear();
		equipSlotLevelCount.clear();
		buddyDamageDatas.clear();
		buddyHurtDatas.clear();
		buddyHealDatas.clear();

		bossCharTypeID.clear();
		bossDamageDatas.clear();
		bossHurtDatas.clear();
		bossHealDatas.clear();
	}

	std::string charName;//指挥官名字
	int32_t controllerLevel;//指挥官等级
	uint64_t damageData;//伤害数据
	int32_t bornType;//头像id
	int32_t commanderTitleFrameID;//头像框id
	int32_t	commanderTitle;//头衔id
	
	std::vector<int32_t> buddyIndexs;
	std::vector<int32_t> buddyIDs;
	std::vector<int32_t> buddyLevels;
	std::vector<int32_t> buddyStarLevels;
	std::vector<int32_t> activeAllSkills;
	std::vector<int32_t> equipSlotLevelCount;//buddy装备槽位的强化等级之和
	mem::map<int32_t, mem::map<int32_t, int32_t> > buddyRuneStarLevelInfo;
	mem::map<int32_t, mem::map<int32_t, int32_t> > buddyRuneItemInfo;
	mem::map<int32_t, mem::map<int32_t, int32_t> > equipSlotStarLevel;
	mem::map<int32_t, mem::map<int32_t, int32_t> > equipItemID;

	std::vector<uint64_t> buddyDamageDatas;
	std::vector<uint64_t> buddyHurtDatas;
	std::vector<uint64_t> buddyHealDatas;

	std::vector<int32_t> bossCharTypeID;//boss的id
	std::vector<uint64_t> bossDamageDatas;//伤害数据
	std::vector<uint64_t> bossHurtDatas;
	std::vector<uint64_t> bossHealDatas;
};
AJSON(SettleInfo, (charName)(controllerLevel)(damageData)(bornType)(commanderTitleFrameID)(commanderTitle)(buddyIDs)(buddyLevels)(buddyStarLevels)(activeAllSkills)(equipSlotLevelCount)(buddyRuneStarLevelInfo)(buddyRuneItemInfo)(equipSlotStarLevel)(equipItemID)(buddyDamageDatas)(buddyHurtDatas)(buddyHealDatas)(bossCharTypeID)(bossDamageDatas)(bossHurtDatas)(bossHealDatas)(buddyIndexs))
//

struct GuildInfo
{
	GuildInfo(uint64_t id, std::string name, uint64_t damage) : guildID(id), totalDamage(damage)
	{
		guildName = name;
	}

	GuildInfo() {}

	uint64_t guildID;//公会id
	std::string guildName;//公会名字
	uint64_t totalDamage;//伤害
};
AJSON(GuildInfo, (guildID)(guildName)(totalDamage))

struct GuildMapDamagePlayerInfo
{
	GuildMapDamagePlayerInfo() : charID(0),playerName(""), playerLevel(0), pictureID(0), titleFrameID(0), damageData(0){}
	
	uint64_t charID;
	std::string playerName;//指挥官名字
	int32_t playerLevel;//指挥官等级
	int32_t pictureID;//头像id
	int32_t titleFrameID;//头像框id
	uint64_t damageData;//伤害值
};
AJSON(GuildMapDamagePlayerInfo, (charID)(playerName)(playerLevel)(pictureID)(titleFrameID)(damageData))

struct AnswerInfo
{
	AnswerInfo():accountPoint(0)
	{
		charName.clear();
	}
	std::string charName;//指挥官名字
	int32_t accountPoint;//积分
};
AJSON(AnswerInfo, (charName)(accountPoint));

struct GuildOPersonalInfo
{
	GuildOPersonalInfo():uPoint(0),nTitleId(0),nGuild(0),uGuildMasterId(0)
	{
		sPlayerName.clear();
		sGuildName.clear();
		sLastTime.clear();
	}
	std::string sPlayerName;//指挥官名字
	std::string sGuildName;//公会名字
	uint64_t nGuild;
	uint64_t uPoint;// timestamp + 积分
	std::string sLastTime;//最后一次上榜时间
	int32_t nTitleId;//头衔
	uint64_t uGuildMasterId;
	
};
AJSON(GuildOPersonalInfo, (sPlayerName)(sGuildName)(nGuild)(nTitleId)(uPoint)(sLastTime)(uGuildMasterId));

struct GuildWatchTowerInfo
{
	GuildWatchTowerInfo():m_Id(0),m_GuildId(0),m_nState(0),m_SafeTimeStamp(0),m_TempTimeStamp(0),m_RewardTimeStamp(0)
	{
		m_sGuildLeaderName.clear();
		m_sGuildName.clear();
	}
	uint64_t	m_Id;				// 高32位 storyId  低32位怪物id
	uint64_t	m_GuildId;				//公会id
	int			m_nState;				//状态 / 未开启状态 开启争夺 临时占领 确认占领
	std::string m_sGuildLeaderName;	// 公会会长名字
	std::string m_sGuildName;		// 公会名字
	
	uint32_t m_SafeTimeStamp;		// 安全期（未开启时间戳） 争夺中 此时间戳无效 （保护时间一到 立马清除 m_GuildId 状态切换为 开启争夺）
	uint32_t m_TempTimeStamp;		// 临时占领 时间戳  本公会人员不允许攻击。临时占领时间一结束， 立马刷新  新的安全期m_SafeTimeStamp
	uint32_t m_RewardTimeStamp;		//占领期 奖励发放时间戳
};
AJSON(GuildWatchTowerInfo, (m_Id)(m_GuildId)(m_nState)(m_sGuildLeaderName)(m_SafeTimeStamp)(m_TempTimeStamp)(m_RewardTimeStamp));

struct SubBInfo
{
	SubBInfo():value(0), cid(0), charID(0), activeBuddyTransformID(0)
	{
		charName.clear();
	}
	std::string charName;//指挥官名字
	int32_t cid;//主控buddy的id
	uint64_t value;
	uint64_t charID;
	int32_t activeBuddyTransformID;
};
AJSON(SubBInfo, (charName)(cid)(value)(charID)(activeBuddyTransformID));

struct BossFirstKillPlayerInfo
{
	BossFirstKillPlayerInfo():killerLevel(0), killerbornCharType(0), killerCommanderTitleID(0), KillerCommanderTitleFrameID(0)
	{
		killerName.clear();
		rewardPlayerName.clear();
		rewardPlayerLevel.clear();
		rewardPlayerBornCharType.clear();
		rewardPlayerCommanderTitleID.clear();
		rewardPlayerCommanderTitleFrameID.clear();
	}

	std::string killerName;
	int32_t killerLevel;
	int32_t killerbornCharType;
	int32_t killerCommanderTitleID;
	int32_t KillerCommanderTitleFrameID;

	std::vector<std::string> rewardPlayerName;
	std::vector<int32_t> rewardPlayerLevel;
	std::vector<int32_t> rewardPlayerBornCharType;
	std::vector<int32_t> rewardPlayerCommanderTitleID;
	std::vector<int32_t> rewardPlayerCommanderTitleFrameID;
};
AJSON(BossFirstKillPlayerInfo, (killerName)(killerLevel)(killerbornCharType)(killerCommanderTitleID)(KillerCommanderTitleFrameID)(rewardPlayerName)(rewardPlayerLevel)(rewardPlayerBornCharType)(rewardPlayerCommanderTitleID)(rewardPlayerCommanderTitleFrameID));

struct RongGuangYuanZhengPlayerInfo
{
	RongGuangYuanZhengPlayerInfo():commanderTitleID(0), mapResID(0), raidStarNum(0)
	{
		playerName.clear();
	}

	std::string playerName;
	int32_t commanderTitleID;
	int32_t mapResID;
	int32_t raidStarNum;
};
AJSON(RongGuangYuanZhengPlayerInfo, (playerName)(commanderTitleID)(mapResID)(raidStarNum));

struct RankPlayerInfo
{
	RankPlayerInfo() : m_nCommanderTitleID(0), m_nValue(0)
	{
		m_nPlayerName.clear();
	}

	std::string m_nPlayerName;
	int32_t m_nCommanderTitleID;
	int32_t m_nValue;
};
AJSON(RankPlayerInfo, (m_nPlayerName)(m_nCommanderTitleID)(m_nValue));

class LC_ServerHelperDiff : public GameLogic::LC_HelperDiff
{
public:
	virtual bool CrossSafe(int32_t iIndex, int32_t sServerID);

	virtual float CalMasterBuffState(int32_t serverID , int64_t iScore, int32_t worldLevel);

	virtual bool IsCrossPlayer(GameLogic::LC_PlayerBase* pkPlayer);
};
struct Order_info // same to toolsvrd 
{
	std::string strOrderId;//(订单id),
	uint32_t serverId;	//(注册时 serverId),
	uint32_t userId;	//(用户id),
	uint64_t money;		//(金额),
	uint32_t cid;		//(自增角色id),
	uint32_t status;	//0,未领取 1,已领取
	uint32_t orderTimeStamp;	//(订单时间戳)}
	uint32_t receiveTimeStamp;	//(领取时间戳)}
	void	Clear()
	{
		strOrderId.clear();
		serverId = 0;
		userId = 0;
		money = 0;
		cid = 0;
		status = 0;
		orderTimeStamp = 0;
		receiveTimeStamp = 0;
	}
};
AJSON(Order_info, (strOrderId)(serverId)(userId)(money)(cid)(status)(orderTimeStamp)(receiveTimeStamp));

#endif
