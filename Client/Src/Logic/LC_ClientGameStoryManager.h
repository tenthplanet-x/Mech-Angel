#ifndef _LC_CLIENT_GAMESTORY_MANAGER_H
#define _LC_CLIENT_GAMESTORY_MANAGER_H
//当前活动管理器
#include "LC_TaskDefine.h"
#include "LC_PlayerTeamAsset.h"
#include "LC_GameStoryDataDefine.h"
enum GameStoryDirtyFlagType
{
	ACTIVITY_DIRTY_FLAG_UPDATE_LIST = 0,
	ACTIVITY_DIRTY_FLAG_GAME_STORY	= 1,

	ACTIVITY_DIRTY_MAX_FLAG_TYPE 
};

enum GameStoryStaticOpenAndHideType
{
	GSI_GSSOAHT_NORMAL		= 0,
	GSI_GSSOAHT_MAPCHANGE	= 1,
};

enum GameStoryTabType
{
	ACTIVITY_TT_TIMELIMIT		= 0,	//限时活动
	ACTIVITY_TT_NUMBERLIMIT		= 1,	//限次活动
	ACTIVITY_TT_TIMELIMIT_EX	= 2,	//限时活动补充
	ACTIVITY_TT_WILD_BOSS		= 3,	//野外伏兵
	ACTIVITY_TT_GUILD_BOSS		= 4,	//帮会BOSS
	ACTIVITY_TT_WILD_PRINCES	= 5,	//野外首领
	ACTIVITY_TT_MAXIMUM			= 6,	//极限模式
	ACTIVITY_TT_TRIAL			= 7,	//试炼活动
	ACTIVITY_TT_SUB_UP			= 8,	//进阶日活动
	ACTIVITY_TT_YUNBIAO			= 9,	//运镖类型(实际上合入TIMELIMIT活动中)
	ACTIVITY_TT_REBATE			= 10,	//返利活动
	ACTIVITY_TT_LOULAN			= 11,	//楼兰地宫
	ACTIVITY_TT_ADVANCE_DUNGEON = 12,	//进阶副本 
	ACTIVITY_TT_GodWeapon_DUNGEON = 13,	//神兵副本
	ACTIVITY_TT_RAIDBOSSSPECIES = 14,	//异种侵袭
	ACTIVITY_TT_BEST_GUILD		= 15,	//最强公会
	ACTIVITY_TT_TOYISLAND		= 16,	//玩具岛
	ACTIVITY_TT_GUILDBOSSACTIVITY = 17, //公会秘境
	ACTIVITY_TT_TREASURETOY = 18,		//挖宝
	ACTIVITY_TT_GOLDBOSS = 19,			//黄金BOSS
	ACTIVITY_TT_SUBUPGRADE = 20,		//升阶试炼

	ACTIVITY_TT_STARSHIP = 21,		//星舰
	ACTIVITY_TT_RAIDBOSSBEFALL = 22,	//异种降临
	ACTIVITY_TT_ANSWERACTIVITY = 23 , //答题活动 

	ACTIVITY_TT_MAX
};

//
enum GameStoryType
{
	GAMESTORY_TYPE_NONE					= 0,
	GAMESTORY_TYPE_YeWaiJingYing		= 1,	//野外精英
	GAMESTORY_TYPE_MingJiaoDaFanGong	= 2,	//明教大反攻
	GAMESTORY_TYPE_JiTuiWuXingQi		= 3,	//击退五行旗
	GAMESTORY_TYPE_LanJieTianYingJiao	= 4,	//拦截天鹰教
	GAMESTORY_TYPE_MingJiaoBoss			= 5,	//明教BOSS
	GAMESTORY_TYPE_QianLiHuLiang		= 6,	//千里护粮
	GAMESTORY_TYPE_ZhuaHouZi			= 7,	//抓猴子活动
	GAMESTORY_TYPE_YouLinMiJing			= 8,	//幽林秘境活动
	GAMESTORY_TYPE_WildPrinces			= 9,	//野外首领活动
	GAMESTORY_TYPE_ZangLongMoXue		= 10,	//葬龙魔穴
	GAMESTORY_TYPE_JiXianMoShi			= 11,	//极限模式
	GAMESTORY_TYPE_Species				= 12,	//异种侵袭
	GAMESTORY_TYPE_BangHuiYunBiao		= 13,	//帮会运镖
	GAMESTORY_TYPE_KangXingShiLian		= 14,	//抗性试炼
	GAMESTORY_TYPE_VehicleUp			= 15,	//坐骑进阶日
	GAMESTORY_TYPE_ArrowUp				= 16,	//弩炮进阶日
	GAMESTORY_TYPE_DevaEyeUp			= 17,	//宠物进阶日
	GAMESTORY_TYPE_VehicleFightingUp	= 18,	//骑兵进阶日
	GAMESTORY_TYPE_RechargeRebate		= 19,	//充值返利
	GAMESTORY_TYPE_Bat					= 20,	
	GAMESTORY_TYPE_Lion					= 21,
	GAMESTORY_TYPE_Hawk					= 22,
	GAMESTORY_TYPE_Loong				= 23,
	GAMESTORY_TYPE_MiJing				= 24,
	GAMESTORY_TYPE_Chest				= 25,
	GAMESTORY_TYPE_PK					= 26,
	GAMESTORY_TYPE_WORLDBOSS_NEW		= 27,
	GAMESTORY_TYPE_LOULAN				= 28,
	GAMESTORY_TYPE_GodWingUp			= 29,
	GAMESTORY_TYPE_BianJingYunBiao		= 30,
	GAMESTORY_TYPE_VEHICLE_DUNGEON		= 31,
	GAMESTORY_TYPE_WING_DUNGEON		    = 32,
	GAMESTORY_TYPE_PET_DUNGEON			= 33,
	GAMESTORY_TYPE_ARROW_DUNGEON		= 34,
	GAMESTORY_TYPE_VEHICLEFIGHT_DUNGEON	= 35,
	GAMESTORY_TYPE_SPIRIT_PET_DUNGEON	= 36,
	GAMESTORY_TYPE_SPIRIT_WEAPON_DUNGEON= 37,
	GAMESTORY_TYPE_KUAFU_BOSS			= 42,
	GAMESTORY_TYPE_LOULANBAOZANG		= 43,
	GAMESTORY_TYPE_SKYARENA 			= 96,
	GAMESTORY_TYPE_GUILDBOSSBEFALL		= 320,
	GAMESTORY_TYPE_CHAINSOULRAID_FAKE	= 201,
	GAMESTORY_TYPE_NORMAL				= 202,
	GAMESTORY_TYPE_GOD					= 203,
	GAMESTORY_TYPE_CHUANQI				= 204,
	GAMESTORY_TYPE_TOYISLAND			= 301,		//玩具岛
	GAMESTORY_TYPE_GameStory			= 10000,	//正常活动的最大类型
	GAMESTORY_TYPE_ShengHuoLing			= 10001,	//圣火令争霸战
	GAMESTORY_TYPE_TuLongDao			= 10002,	//屠龙刀争霸战
	GAMESTORY_TYPE_ShiLiZhengDuo		= 10003,	//势力争夺战
	GAMESTORY_TYPE_TEAM_RAID			= 20000,
	GAMESTORY_TYPE_TEAMRAID_1			= 20001,
	GAMESTORY_TYPE_TEAMRAID_2			= 20002,
	GAMESTORY_TYPE_TEAMRAID_3			= 20003,
	GAMESTORY_TYPE_TEAMRAID_4			= 20004,

	GAMESTORY_TYPE_SPECIAL_QIANLIHULIANG	= 30001,
	GAMESTORY_TYPE_SPECIAL_BANGHUIYUNBIAO	= 30002,
};

enum GameStoryID
{
	GAMESTORY_ID_QianLiHuLiang			= 6,
	GAMESTORY_ID_WildPrinces			= 12,
	GAMESTORY_ID_ZangLongMoXue			= 13,
	GAMESTORY_ID_QingGongShiLian		= 15,
	GAMESTORY_ID_BangHuiYunBiao			= 16,
	GAMESTORY_ID_BianJingYunBiao		= 37,
	GAMESTORY_ID_SUBUPGRADE_VEHICLE		= 41,
	GAMESTORY_ID_SUBUPGRADE_PET			= 42,
	GAMESTORY_ID_SUBUPGRADE_BACKWING	= 43,
};

inline bool IS_RAIDSINGLE_SUB(int iGameStoryID)
{
	return iGameStoryID == GAMESTORY_ID_SUBUPGRADE_VEHICLE
		|| iGameStoryID == GAMESTORY_ID_SUBUPGRADE_PET
		|| iGameStoryID == GAMESTORY_ID_SUBUPGRADE_BACKWING;
}

//活动副本列表
class GameStoryRaidInfo
{
public:
	enum GSRI_StateType
	{
		GSRIST_UNPASSED	= 0,	//未通关
		GSRIST_PASSED	= 1,	//已通关
		GSRIST_CURRENT	= 2,	//当前副本
		GSRIST_FAILED	= 3,	//失败
	};

	DECL_RTTI_NAME( GameStoryRaidInfo )
	GameStoryRaidInfo()
	:m_lID(0)
	,m_lBelongStoryID(0)
	,m_lMapResID(0)
	,m_iContentType(0)
	,m_iType(0)
	,m_lActive(0)
	,m_iState(GSRIST_UNPASSED)
	,m_iEnterShopID(0)
	,m_iRenterShopID(0)
	,m_iRespawnShopID(0)
	{
		m_kPreTasks.clear();
	}

	long		GetID(){return m_lID;}
	long		GetCurRaidTID(){return m_lMapResID;}
	long		GetCurRaidTID()const{return m_lMapResID;}
	long		GetStroyID(){return m_lBelongStoryID;}
	long		GetTrueStroyID();
	int			GetContentType(){return m_iContentType;}
	long		GetActive(){return IsActive() ? 1 : 0;};
	bool		IsRaid(){return true;}
	int			GetEnterMinLevel();
	int			GetEnterMaxLevel();
	const StringType& GetShowName();
	const StringType& GetIcon();
	int			GetPassState(){return m_iState;}
	bool		IsActive();
	int			GetEnterCost();
	int			GetType(){return m_iType;};

	long m_lID;
	long m_lBelongStoryID;
	long m_lMapResID;
	int  m_iContentType;
	int	 m_iType;
	int  m_lActive;
	StringType m_kBossName;
	int m_iState;	//

	int m_iEnterShopID;	//第一次进入花费
	int m_iRenterShopID;//后续进入花费
	int m_iRespawnShopID;//复活花费
	std::set<int> m_kPreTasks;//前置任务
};

//活动列表
class GameStoryInfo
{
public:
	enum BasicStatType
	{
		GSI_BST_TOTAL_EXP		= 0,
		GSI_BST_TOTAL_SCORE		= 1,
		GSI_BST_TOTAL_CASH		= 2,
		GSI_BST_TOTAL_SP		= 3,
		GSI_BST_COUNT_EXP		= 4,
		GSI_BST_COUNT_SCORE		= 5,
		GSI_BST_COUNT_KILLED	= 6,

		GSI_BST_CURRENT_INDEX	= 7,
		GSI_BST_ALL_INDEX		= 8,
		GSI_BST_BATCH_INDEX		= 9,
		GSI_BST_BATCH_FLAG		= 10,
		GSI_BST_CLEAR_TIME		= 11,
		GSI_BST_INIT_TIME		= 12,
		GSI_BST_TRIGER_SIZE		= 13,
		GSI_BST_COUNT_DEAD		= 14,
		GSI_BST_COUNT_KILLPLAYER = 15,
		GSI_BST_COUNT_KILLREWARD = 16,
	};

	enum GameStoryScheduleFlag
	{
		GS_SCH_FG_INITED = 0,
		GS_SCH_FG_FAILED,
	};
	struct GameStoryInfoTimesStat
	{
		GameStoryInfoTimesStat(int iMapResID, int iEnterTimes, int iFinishedTimes)
			:m_iMapResID(iMapResID)
			,m_iEnterTimes(iEnterTimes)
			,m_iFinishedTimes(iFinishedTimes)
		{
			
		}

		int m_iMapResID;
		int m_iEnterTimes;
		int m_iFinishedTimes;
	};

	GameStoryInfo()
	{
		Reset();
	}

	void Reset()
	{
		m_bValid				=	false;
		m_bIsOpened				=	true;
		m_lStoryID				=	0;
		m_lActive				=	0;
		m_lMaxEnteredTimes		=	0;
		m_iAlreadyBuyTimes		=	0;
		m_lAlreadyEnteredTimes	=	0;
		m_lAlreadyFinishedTimes =	0;
		m_strTimeSlot			=	"";
		m_bIsFinished			=   false;
		m_iCurRaidTID			=	0;
		m_iGameStoryInfoRaidID	=	0;
		m_iRaidBaseShopID		=	0;
		m_iRaidStepShopID		=	0;
		m_lMinEnterLevel		=	0;
		m_lMaxEnterLevel		=	0;
		m_iStartTimerID			=	0;
		m_iEndTimerID			=	0;
		pkConfig				=	NULL;
		m_iValidDays			=	 0;
		m_iFirstLightTaskID		=	0;
		m_iTimeBegin			=	0;
		m_iTimeEnd				=	0;
		m_iContentType			=	0;
		m_iCurAreaIndex			=	-1;
		m_iCurAreaState			=	0;
		m_iRaidTTLTimesLimit	=	0;
		m_iRaidFreeTimes		=	0;
		m_iRaidEnterTimes		=	0;
		m_iTabType				=	0;
		m_iType					=	0;
		m_kClientRewardType		=	0;
		m_iElementDamageType	=	0;
		m_IsGuildActivity		=	false;

		m_IsGuildActivity       =   0;

		m_iTimeLen				=	0;
		m_iCSVOpenTime			=	0;
		m_iCSVEndTime			=	0;
		m_iCSVStoryID			=	0;
		m_iClientUIGroupRecommendNumber	=	0;

	}
	void ResetActive()
	{
		m_lActive = 0;
	}
	DECL_RTTI_NAME( GameStoryInfo )

	int			GetValidDays(){return m_iValidDays;}
	bool		GetValid(){return m_bValid;};
	long		GetStroyID(){return m_lStoryID;};
	long		GetTrueStroyID();
	long		GetCurRaidTID(){return m_iCurRaidTID;}
	long		GetGameStoryInfoRaidID(){return m_iGameStoryInfoRaidID;}
	int			GetContentType(){return m_iContentType;}
	int			GetTabType(){return m_iTabType;}
	int			GetTabType()const {return m_iTabType;}
	int			GetType(){return m_iType;}
	long		GetActive();
	long		GetActive() const ;
	long		GetMaxEnteredTimes(){return m_lMaxEnteredTimes;};
	long		GetAlreadyEnteredTimes();
	long		GetAllAlreadyFinishedTimes();
	const StringType& GetTimeSlot(){return m_strTimeSlot;}
	const StringType& GetShowName(){return m_kClientShowName;}
	const StringType& GetGameStoryRule(){return m_kClientUIGameStoryRule;}
	const StringType& GetDescription(){return m_kClientUIDescription;}
	const StringType& GetStatisticsTips(){return m_kClientUIStatisticsTip;}
	const StringType& GetBackImage(){return m_kClientUIBackImage;}
	const StringType& GetNameImage(){return m_kClientNameImage;}
	int		GetGameStoryGroupRecommendNum(){return m_iClientUIGroupRecommendNumber;}
	
	bool		IsFinished(){return m_bIsFinished;};
	bool		IsTimePassToday();
	int			GetAlreadyFinishedTimes();
	const StringType& GetNoticeText(){return m_kClientUINotice;}
	const StringType& GetRewardString(){return m_kClientUIReward;}
	const StringType& GetResetTime(){return m_kClientUIResetTime;}
	const StringType& GetMapListText(){return m_kClientUIMapListText;}
	int			GetMapListCount()const{return m_kRaidStat.size();}
	int			GetMapListCount(){return m_kRaidStat.size();}
	int			GetMapID(int iIndex);
	int			GetMapAreaListCount(){return m_kAreaList.size();}
	int			GetMapAreaID(int iIndex){return m_kAreaList[iIndex];}
	int			GetCurMapAreaIndex(){return m_iCurAreaIndex;}
	int			GetCurMapAreaState(){return m_iCurAreaState;}
	bool		IsOpened(){return m_bIsOpened;}
	int64_t		GetBasicStatInfoCount(int iType);
	int64_t		GetBeginTimes(){return m_iTimeBegin/1000;}
	int64_t		GetEndTimes(){return m_iTimeEnd/1000;}
	uint32_t		GetCsvOpenTime(){return m_iCSVOpenTime;}
	uint32_t		GetCsvEndTime(){return m_iCSVEndTime;}
	int			GetBasicStatItemCount(){return m_kBasicStat.m_kItems.size();}
	StringType	GetBasicStatItemInfo(int iIndex);
	int			GetBasicStatTotalItemCount(){return m_kBasicStat.m_kTotalItems.size();}
	const StringType&	GetScript(){return m_kScript;}
	const StringType&	GetPlayRule(){return m_kClientUIPlayRule;}
	long		GetTimeStamp();
	long		GetBeginTimeStamp();
	int			GetTimeLen(){return m_iTimeLen;}
	long		GetNextMonsterTimeStamp();
	long		GetNextMonsterTimeStampByBatchIndex(int index);
	bool		IsRaid(){return false;}
	int			GetEnterMinLevel()const {return m_lMinEnterLevel;}
	int			GetEnterMinLevel(){return m_lMinEnterLevel;}
	int			GetEnterMaxLevel()const{return m_lMaxEnterLevel;}
	int			GetEnterMaxLevel(){return m_lMaxEnterLevel;}
	int			GetPassedRaidCount();
	int			GetCanJoinRaidCount();
	int			GetRaidFreeTimes(){return m_iRaidFreeTimes;}
	void		AddRaidInfo(const GameStoryRaidInfo& info);
	void		UpdateRaidPassed();
	int			GetEnterCost();
	bool		IsFailed() {return (m_kBatchInfo.m_uiFlag & (1 << GS_SCH_FG_FAILED)) > 0; }
	bool		IsInit() {return  (m_kBatchInfo.m_uiFlag & (1 << GS_SCH_FG_INITED)) > 0;}
	int			GetFirstLightTaskID(){return m_iFirstLightTaskID;}

	int			GetStoryAlreadyBuyTimes() { return m_iAlreadyBuyTimes;};
	bool		IsCrossMap(int iMapResID);

	int			GetRewardType() { return m_kClientRewardType; }
	int			GetElementDamageType() { return m_iElementDamageType; }
	bool         GetIsGuildActivity(){return m_IsGuildActivity;} 
	StringType  GetBatchSecheduleInfo(int index);
	StringType  GetBatchSecheduleExpAndCash(int curIndex, int maxIndex);

	StringType	GetMonsterSetting(int index);

	Protocol::PS_GAMESTORY_BATCH_DETAIL* GetBatchDetail(void)
	{
		if (m_kBatchInfo.m_kDetails.empty())
		{
			return NULL;
		}
		return &m_kBatchInfo.m_kDetails[0];
	}

	Protocol::PS_GAMESTORY_BATCH_DETAIL* GetBatchDetailByIndex(int index)
	{
		std::vector<PS_GAMESTORY_BATCH_DETAIL> &kBatchDetail = m_kBatchInfo.m_kDetails;
		if (kBatchDetail.empty())
		{
			return NULL;
		}
		for (uint32_t i = 0; i < kBatchDetail.size(); ++i)
		{
			if (kBatchDetail[i].m_uiBatchIdx == index)
			{
				return &kBatchDetail[i];
			}
		}
		return NULL;
	}

	int32_t GetNextRefreshTime(void)
	{
		int64_t curTime = GET_SERVER_SYSTEM_TIME;
		if(m_kBatchInfo.m_kDetails.empty())
			return -1;
		int32_t destTime = INT_MAX;
		for(int i = 0; i < m_kBatchInfo.m_kDetails.size(); i++)
		{
			uint32_t t = m_kBatchInfo.m_kDetails[i].m_iTriTimestamp;
			if(curTime < t)
			{
				if (destTime > t)
					destTime = t;
			}
		}
		if (destTime == INT_MAX)
			return -1;
		return destTime;
	}

	GameStoryRaidInfo* GetStoryRaidInfoByIndex(int iIndex)
	{
		if (iIndex < 0 || iIndex >= m_kStoryRaidList.size())
			return NULL;
		return &m_kStoryRaidList[iIndex];
	}
	int			GetRaidInfoIndex(long lRaidID);

	int			GetLastEnteredRaidID(void)
	{
		if (m_lAlreadyEnteredTimes<=0)
		{
			return -1;
		}
		mem::vector<GameStoryInfoTimesStat>::iterator it = m_kRaidStat.begin();

		for (; it!=m_kRaidStat.end(); ++it)
		{
			if (it->m_iEnterTimes>0 && it->m_iFinishedTimes<=0)
			{
				return it->m_iMapResID;
			}
		}
		return -1;

	}
	
	int			GetLastAreaInfosStateByIndex(int iIndex);
	bool		m_bIsOpened;
	bool		m_bValid;
	long		m_lStoryID;
	int			m_iContentType;
	long		m_lActive;
	long		m_lMaxEnteredTimes;
	int			m_iAlreadyBuyTimes;
	long		m_lAlreadyEnteredTimes;
	long		m_lAlreadyFinishedTimes;
	StringType	m_strTimeSlot;
	int			m_lMaxEnterLevel;
	int			m_lMinEnterLevel;
	bool		m_bIsFinished;
	long		m_iCurRaidTID;
	int64_t		m_iTimeBegin;
	int64_t		m_iTimeEnd;
	int			m_iCurAreaIndex;
	int			m_iCurAreaState;
	int			m_iRaidTTLTimesLimit;
	int			m_iRaidFreeTimes;
	int			m_iRaidEnterTimes;
	int			m_iTabType;
	int			m_iType;
	int			m_iRaidBaseShopID;
	int			m_iRaidStepShopID;
	int			m_iTimeLen;
	long		m_iGameStoryInfoRaidID;//组队副本使用较多
	int			m_iValidDays;
	int			m_iFirstLightTaskID;
	Protocol::PS_GAME_STORY_BASIC_STAT	m_kBasicStat;
	Protocol::GAMESTORY_BATCH_INFO		m_kBatchInfo;
	mem::vector<int>					m_kAreaList;
	mem::vector<GameStoryInfoTimesStat> m_kRaidStat;
	mem::vector<GameStoryRaidInfo>		m_kStoryRaidList;
	mem::map<long, int>					m_kRaidIDWithIndexMap;
	uint32_t							m_iCSVOpenTime;
	uint32_t							m_iCSVEndTime;
	int									m_iCSVStoryID;
	int									m_iStartTimerID;
	int									m_iEndTimerID;
	///////UI显示相关，由于有两个不同的表，所以存储在这里
	StringType	m_kScript;
	StringType  m_kClientShowName;
	int			m_iClientUIGroupRecommendNumber;
	StringType  m_kClientUIReward;
	StringType	m_kClientUINotice;
	StringType	m_kClientUIDescription;
	StringType	m_kClientUIPlayRule;
	StringType	m_kClientUIGameStoryRule;
	StringType	m_kClientUIResetTime;
	StringType	m_kClientUIResetMap;
	StringType	m_kClientUITooltipReward;
	StringType	m_kClientUITip;
	StringType	m_kClientUIMapListText;
	StringType	m_kCronTime;
	StringType	m_kClientUIStatisticsTip;

	StringType	m_kClientUIBackImage;
	StringType	m_kClientNameImage;
	int			m_kClientRewardType;
	int			m_iElementDamageType;
	bool		m_IsGuildActivity;
	GameStory::GameStoryConfig *pkConfig;
};

class LC_ClientGameStoryManager:public Memory::MM_BaseObject
{
public:
	LC_ClientGameStoryManager();
	~LC_ClientGameStoryManager();
	DECL_RTTI_NAME( LC_ClientGameStoryManager )

	typedef mem::map<long, int>			StoryStateMap; //0：不变，-1：由开启到关闭，1：由关闭到开启

	struct GameStoryMapForVector
	{
		GameStoryMapForVector(){ m_iType = 0; m_iIndex = 0; }
		GameStoryMapForVector(int iType, int iIndex)
			:m_iType(iType)
			,m_iIndex(iIndex)
		{}
		int m_iType;		//GameStoryTabType
		int m_iIndex;		//对应vector中的索引
	};

	struct GameStoryVIPChallengeInfo
	{
		GameStoryVIPChallengeInfo(){ m_iGameStoryID = m_iBuyTimes = m_iTotalTimes = m_iCostType = m_iCostCount = 0; }
		GameStoryVIPChallengeInfo(int iGameStoryId, int iBuyTimes, int iTotalTimes, int iCostType, int iCostCount)
			: m_iGameStoryID(iGameStoryId)
			, m_iBuyTimes(iBuyTimes)
			, m_iTotalTimes(iTotalTimes)
			, m_iCostType(iCostType)
			, m_iCostCount(iCostCount)
		{
		}
		int m_iGameStoryID;
		int m_iBuyTimes;
		int m_iTotalTimes;
		int m_iCostType;
		int m_iCostCount;
	};

	void					Update(float fCurrentTime,float fDeltaTime); 
	//void					ReceiveTranscriptionGameStoryInfo(Protocol::MG_NOTIFY_CLIENT_GAMESTORY& pkMsg);
	//void					ReceiveTaskPerDayGameStoryInfo(Protocol::MG_NOTIFY_CLIENT_GAMESTORY& pkMsg);
	//void					ReceiveItemExchangeGameStoryInfo(Protocol::MG_NOTIFY_CLIENT_GAMESTORY& pkMsg);
	void					ReceiveGameStoryBatchInfosync(Protocol::MG_GAMESTORY_BATCH_INFO_SYNC& pkMsg);
	void					ReceiveGameStoryContentInfosync(Protocol::MG_GAMESTORY_CONTENT_INFO_SYNC& pkMsg);
	void					ReceiveCrossRealmTeamAssetSync(GameLogic::LC_CrossRealmTeamAsset *pkCrossRealTeamAsset);
	bool					CheckGameStoryStartInfo(long lStoryID);
	void					ReceiveStoryTipsContent(Protocol::MG_TIPS_SERVER_TO_CLIENT& pkMsg);

	void					GetDataFromProtocolStruct(Protocol::MG_GAMESTORY_RANK_INFO_SYNC& rkMsg);
	//活动列表
	GameStoryInfo*			GetGameStoryInfo(long lStoryID);

	GameStoryInfo*			GetActiveGameStoryInfoByTrueID(long lStoryID);
	//下面函数为包含工会战等活动信息
	GameStoryInfo*			GetGameStoryInfoByIndex(int iType, int iIndex);
	int						GetGameStoryInfoCount(int iType);

	//获取下一个即将开始的活动INDEX
	int						GetGameStoryType(long lGameStoryID);							//活动类型
	int						GetGameStoryTabType(long lGameStoryID);							//活动类型
	int						GetGameStoryTodayActiveStoryNum(int iTabType);
	int						GetGameStoryTodayFinisnedStoryNum(int iTabType);

	int						GetGameStoryMonsterState(long lGameStoryID, int CharTypeID);
	int						GetGameStoryMonsterTimestamp(long lGameStoryID, int CharTypeID);

	void					InitAllGameStoryInfo();
	int						GetGameStoryRevisionID(int iFlag, long iStoryID = 0);
	void					AddGameStoryRevisionID(int iFlag, long iStoryID = 0);
	void					OnMapChanged(int lMapResID);
	bool					CanJoin(const GameStoryInfo& kGameStoryInfo);
	bool					CanJoin(const GameStoryInfo& kGameStoryInfo, int iMapID);
	bool					GameStoryCanJoin(GameStoryInfo *pkGameStoryInfo){return CanJoin(*pkGameStoryInfo);}
	void					UpdateMapAreaInfo(int lMapResID, int iAreaIndex, int iState);
	void					UpdateGameStoryRaidPassed(int iType);
	long					GetActiveStoryID(int lMapResID);
	long					GetGameStoryIDBylMapResID(int lMapResID);
	void					Reset();
	bool					IsMapResIDInGameStory(long lGameStoryID);
	StringType				GetGameStoryTimeSlot(long lGameStoryID);
	StringType              GetGameStoryTimeSlotOnWeekday(long lGameStoryID);

	int				GetTargetPosX(void){return m_iTargetPosX;}
	int				GetTargetPosY(void){return m_iTargetPosY;}
	int				GetTargetGameStoryID(){return m_TargetGameStoryID;}
	void			SetTargetPos(int iPosX, int iPosY, int iGameStoryID){m_iTargetPosX = iPosX;m_iTargetPosY = iPosY;m_TargetGameStoryID = iGameStoryID;}
	int				GetDirectionFxID(){return m_iDirectionFxID;}
	void			SetDirectionFxID(int iID){m_iDirectionFxID = iID;}

	void			ResetNumberLimitRemainTimes();
	// VIP
	void			ReceiveDataFormServer(const MG_RLT_GetBuyTimes& rltMsg);
	int			GetVIPBuyTimesByStoryID(int iGameStoryID);
	int			GetVIPTotalTimesByStoryID(int iGameStoryID);
	int			GetVIPCostTimesByStoryID(int iGameStoryID);

	//定时器专用

	bool			IsBatchBeginByGameStoryID(long lGameStoryID);

public:
	static void				ActiveStory(Utility::timer_id_type timerID, int storyID);
	static void				DeactiveStory(Utility::timer_id_type timerID, int storyID);
private:
	void					_updateGameStoryYunBiao();
	void					_updateGameStoryState(const StoryStateMap & rkSSMap);
	bool					_isMapResIDInGameStory(const GameStoryInfo& kGameStoryInfo, int lMapResID);
	void					_ClearAndAddNormalGameStoryInfoList();
	static bool				_sortTimeLimitGameStoryList(GameStoryInfo *a,GameStoryInfo *b);
	static bool				_sortByLevelGameStoryList(GameStoryInfo *a,GameStoryInfo *b);
	void					_FilterCronTimestampSameDay(const StringType& cronStr, std::vector<uint32_t>& vecTime);

	bool					_IsGameStoryTypeSpecial(GameStoryInfo& kGameStoryInfo);
protected:

	mem::vector<GameStoryInfo>				m_kNormalGameStoryInfoList[ACTIVITY_TT_MAX];
	mem::map<long, GameStoryMapForVector>	m_kNormalIDWithIndexMap;

	mem::vector<GameStoryInfo>				m_kSpecialGameStoryInfoList[ACTIVITY_TT_MAX];
	mem::map<long, GameStoryMapForVector>	m_kSpecialIDWithIndexMap;
	
	mem::vector<GameStoryInfo*>			m_pkClientGameStoryInfoList[ACTIVITY_TT_MAX];//用于UI显示

	mem::map<int, GameStoryVIPChallengeInfo> m_kClientGameStoryVIPInfo;

	int									m_iTodayFinishedStoryNum[ACTIVITY_TT_MAX];		//今日已完成的所有活动

	int									m_iTargetPosX;//目标位置
	int									m_iTargetPosY;
	int									m_TargetGameStoryID;
	int									m_iDirectionFxID;

	long								m_lGameStoryInfoRevisionID;
	mem::map<long, int>					m_lGameStoryIDRevisionMap;	//活动id版本号

	std::vector<GAMESTORY_RANK_INFO> m_kRanks;
};

#endif