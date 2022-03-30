#ifndef _LC_CLIENT_GAME_EVENT_H
#define _LC_CLIENT_GAME_EVENT_H

#include "LC_GameEvent.h"
#include "ClientDefine.h"

class LC_ClientGameEventUserDataBase;
class LC_ClientGameEvent : public GameLogic::LC_GameEventBase
{
public:
	LC_ClientGameEvent();
	~LC_ClientGameEvent();
	DECL_RTTI_NAME( LC_ClientGameEvent )

	int		GetClientEventType();
	void	SetClientEventType(int iClientEventType);
	void	SetUserData(LC_ClientGameEventUserDataBase* pData);
	LC_ClientGameEventUserDataBase*	GetUserData();
	void	Reset();
private:
	void	SetEventType(GameLogic::GameEventType eEventType);
private:
	int		m_iClientEventType;
	LC_ClientGameEventUserDataBase*	m_pUserData;			//该指针所指向的内存块在外部创建和卸载
};

class LC_ClientGameEventUserDataBase : public Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME( LC_ClientGameEventUserDataBase )
};

class LC_ClientGameEventUserData_RaidSettle : public LC_ClientGameEventUserDataBase
{
public:
	DECL_RTTI_NAME( LC_ClientGameEventUserData_RaidSettle )

	LC_ClientGameEventUserData_RaidSettle(const Protocol::MG_Notify_RaidSettle& kMsg,uint16_t _storyID,uint32_t _mapResID);
	long GetItem(int iIndex);
	int GetBuddyId(int iIndex);
	int GetItemCount(int iIndex);
	int GetBuddyExp(int iIndex);
	int GetBuddyNewLevel(int iIndex);
	int GetBuddyOldLevel(int iIndex);
public:
	uint32_t result;
	uint32_t level;
	uint32_t exp;
	uint32_t sp;
	uint32_t money;
	uint8_t  itemlen;
	uint32_t item[MAX_ITEM_LEN];
	uint32_t itemcount[MAX_ITEM_LEN];
	uint32_t monsters;
	uint32_t deads;
	uint32_t passtime;
	bool endFlag;
	uint32_t buddyCount;
	uint32_t controllerLevel;
	uint32_t buddyId[MAX_ACTIVE_BUDDY_NUM];
	uint32_t buddyExp[MAX_ACTIVE_BUDDY_NUM];
	uint32_t buddyLevel[MAX_ACTIVE_BUDDY_NUM];
	uint32_t buddyOldLevel[MAX_ACTIVE_BUDDY_NUM];
	uint16_t  storyID;
	uint32_t  mapResID;
	bool    hasBuddyDead;                 			//是否无buddy死亡
	bool    preEnd;                       			//是否提前n秒通过副本
	bool    bFirstPass;                   			//是否首次通关
};

class LC_ClientGameEventUserData_String : public LC_ClientGameEventUserDataBase
{
public:
	DECL_RTTI_NAME( LC_ClientGameEventUserData_String )

	LC_ClientGameEventUserData_String(const StringType &sValue);
	const StringType& GetString();

public:
	StringType kString;
};

class LC_ClientRankUserInfo : public Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME( LC_ClientRankUserInfo )

	LC_ClientRankUserInfo();
	LC_ClientRankUserInfo(const PS_User_Info& kUserInfo);

	LC_ClientRankUserInfo& operator= (const LC_ClientRankUserInfo& other);
	LC_ClientRankUserInfo& operator= (const PS_User_Info& other);

	void SetUserInfo(const Protocol::PS_Rank_PlayerBaseData& kUserInfo);
	void SetUserInfo(const Protocol::PS_Rank_BuddyData& pkBuddyData);
	void SetUserInfo(const Protocol::PS_Rank_PetData& pkPetData);
	void SetUserInfo(const Protocol::PS_Rank_SubData& pkSubData, int iRankType);
	void SetUserInfo(const Protocol::PS_RankTopNPlayerSingleInfo& pkUserData);
	void SetUserInfo(const std::vector<KeyValue_Base>& pkBuddyRankIndex);
	void SetUserInfo(const Protocol::PS_AllSchemeBuddyData& pkSchemeBuddyData);
	void SetRobotUserInfo(int32_t charID);

	void Reset();
public:
	unique_id_type		GetUserID();
	uint32_t			GetServerID();
	int64_t				GetValue();
	bool				GetIsRobot();
	int					GetLevel();
	const StringType&	GetUserName();
	guild_id_type		GetGuildID();
	StringType			GetGuildName();
	int64_t				GetCommonderScore();
	int64_t				GetActiveScore();


	int64_t				GetAttributeValue(int iAttributeType);
	int64_t				GetChainAttributeValue(int iAttributeType);
	int64_t*			GetAttribute(int iAttributeType);
	int64_t*			GetChainAttribute(int iAttributeType);

	int					GetCurrentBuddyCID();
	int					GetCurrentBuddyTransform();

	platform_info*		GetPlatformInfo(const StringType& sPlatformType);
	void				AddPlatformInfo(const StringType& sPlatType, uint8_t platInfo);
	int					GetPlatformVipLevel(const StringType& sPlatformType);
	bool				IsPlatformYearVip(const StringType& sPlatformType);
	bool				IsPlatformVip(const StringType& sPlatformType);
	bool				IsPlatformHighVip(const StringType& sPlatformType);

	bool				IsSubTransformActive(int iSubType, int iId);
	StringType			GetPetInfo();
	StringType			GetBuddyInfo();
	int64_t				GetAllSchemeScore();
	StringType			GetRanksInfo(uint32_t SchemeGroupID);
	StringType			GetBuddyRankInfo();
	int32_t				GetPetID();
	int32_t				GetPetStarLevel();
	StringType			GetSubInfo(int32_t subType);
	int32_t				GetBornCharType();
	int					GetCommanderHeadFrame();
	int					GetActiveBuddyCID(int iIdx);
	int					GetActivePetID() { return m_iActivatePetID; }
	int					GetCommanderTitleLevel();
	StringType			GetCommanderEquipInfo(int iEquipSlot);
	int64_t				GetBaseAttrMapScore();
	int64_t				GetChainAttrMapScore();

public:
	struct BuddyInfo
	{
		BuddyInfo()
		{
			Clear();
		}

		inline void Clear(void)
		{
			iCid = 0;
			iTransformId = 0;
			iLevel = 0;
			iStarLevel = 0;
			iCombatScore = 0;
			iAllEquipReinforceLevel = 0;
			iActivateAllSkill = false;
			iRuneInfo.clear();
			iEquipInfo.clear();
			iRank = -1;
		}

		int iCid;
		int iTransformId;
		int iLevel;
		int iStarLevel;
		combat_score_type iCombatScore;
		int iAllEquipReinforceLevel;
		bool iActivateAllSkill;
		std::map<int32_t,PS_RuneItemInfo_BuddyCS> iRuneInfo;
		std::map<int32_t,PS_EquipItemInfo_BuddyCS> iEquipInfo;
		int iRank;
	};

	typedef mem::vector<BuddyInfo> BuddyInfoVec;
	typedef mem::map<int32_t,BuddyInfoVec> SchemeInfo;

	struct SubDetail
	{
		SubDetail()
		{
			Clear();
		}

		inline void Clear(void)
		{
			m_nSubType = 0;
			m_nSubStage = 0;
			m_nSubTransform = 0;
			m_nSubCombatScore = 0;
			m_SubEquipInfo.clear();
		}

		int32_t							m_nSubType;
		int32_t                         m_nSubStage;                    			//进阶线等阶
		int32_t                         m_nSubTransform;                			//进阶线皮肤
		uint64_t                        m_nSubCombatScore;              			//进阶线战斗力
		mem::map<int32_t,PS_SubEquipSlotItemInfo>  m_SubEquipInfo;                 			//进阶线装备
	};

	typedef mem::map<int, SubDetail> SubInfoMap;


	unique_id_type		m_lUserID;
	uint32_t			m_ServerId;
	int					m_iRanking;
	int					m_iLevel;
	StringType			m_kName;
	int64_t				m_Value;		//排行榜榜单上的值
	guild_id_type		m_GuildID;
	bool				m_bIsRobot;


	//战斗力
	int64_t 			m_CommonderScore;	//指挥官战力  
	int64_t 			m_ActiveScore;		//上阵战斗力

	//属性
	int64_t 	m_Life;
	int64_t 	m_Mp;
	int64_t 	m_Atk;
	int64_t 	m_Cirrate;
	int64_t 	m_Cirdam;
	int64_t 	m_Def;
	int64_t 	m_Dograte;
	int64_t 	m_Movespeed;
	int64_t		m_CirDef;
	int64_t		m_DmgRate;
	int64_t		m_DmgDeRate;
	int32_t		m_PkValue;
	int64_t		m_CirDamDerate;
	int64_t		m_AtkRate;
	int64_t		m_ExpRate;

	//连锁属性
	int64_t 	m_ChainLife;
	int64_t 	m_ChainAtk;
	int64_t 	m_ChainAtkRate;
	int64_t 	m_ChainCirrate;
	int64_t 	m_ChainCirdam;
	int64_t 	m_ChainDef;
	int64_t 	m_ChainDograte;
	int64_t		m_ChainCirDef;
	int64_t		m_ChainDmgRate;
	int64_t		m_ChainDmgDeRate;
	int64_t		m_ChainCirDamDerate;

	StringType	m_GuildName;

	//进阶线详细信息
	SubInfoMap						m_SubInfos;
	//展示buddy详细信息
	BuddyInfoVec					m_buddyInfos;
	//所有buddy排名信息
	mem::map<int, int>				m_buddyRankMap;
	//阵容信息
	SchemeInfo						m_SchemeInfo;

	//战力最高的玩具
	int32_t							m_MaxCombatScorePetID;
	int32_t							m_MaxCombatScorePetIDStarLevel;

	//平台登录的一些玩家信息
	PlatformMap						m_platforminfo; 

	//当前主控cid
	int32_t							m_CurrentBuddyID;
	int32_t							m_CurrentBuddyTransform;
	//已出战玩具ID
	int32_t							m_iActivatePetID;		

	int32_t							m_bornCharType;			//头像
	int32_t							m_CommanderHeadFrame;	//头像框
	int32_t							m_CommanderTitleLevel;	//头衔
	
	mem::map<int, mem::set<int>>	m_kSubInfoMap;			//进阶线皮肤数据
	mem::map<int, int>				m_kPetInfoMap;			//宠物数据
	mem::vector<int>				m_kActiveBuddyVec;		//已出战buddy
	mem::map<int, PS_ControllerEquipSlotItemInfo> m_mControllerEquipSlotMap; //指挥官装备信息
};

class LC_ClientGameEventUserData_ArenaBattle_BattleInfo : public LC_ClientGameEventUserDataBase
{
public:
	DECL_RTTI_NAME(LC_ClientGameEventUserData_ArenaBattle_BattleInfo)
	LC_ClientGameEventUserData_ArenaBattle_BattleInfo(const Protocol::MG_RESP_BattleInfo& kMsg);
	unique_id_type GetTopChallengeID(int iIndex);
	const char* GetTopChallengeName(int iIndex);
	unique_id_type GetTopBeChallengeID(int iIndex);
	const char* GetTopBeChallengeName(int iIndex);
	bool TopIsSuccess(int iIndex);
	int GetTopFinalRankIndex(int iIndex);
	unique_id_type GetUserChallengeID(int iIndex);
	const char* GetUserChallengeName(int iIndex);
	int GetUserChallengeRank(int iIndex);
	unique_id_type GetUserBeChallengeID(int iIndex);
	const char* GetUserBeChallengeName(int iIndex);
	int GetUserBeChallengeRank(int iIndex);
	bool UserIsSuccess(int iIndex);
	bool UserIsExchange(int iIndex);
public:
	int battlesize;
	Protocol::PS_Rank_User_Battle_Info m_UserBattleInfos[MAX_RANK_BATTLE_INFO];
	int topsize;
	Protocol::PS_Rank_User_Battle_Info m_TopBattleInfos[MAX_RANK_BATTLE_INFO];
	int32_t serverFlag;
};

class LC_ClientGameEventUserData_ArenaSettleInfo : public LC_ClientGameEventUserDataBase
{
public:
	DECL_RTTI_NAME(LC_ClientGameEventUserData_ArenaSettleInfo)
	LC_ClientGameEventUserData_ArenaSettleInfo(const Protocol::MG_NOTIFY_Challenge& kMsg);

public:
	int iType;
	int PreIndex;
	int CurIndex;
	bool Successed;
	int Exp;
	int Money;
	int SP;
	int Courage;
	int LiJin;
	StringType BeChallengerUserName;
};

class LC_ClientGameEventUserData_Common : public LC_ClientGameEventUserDataBase
{
public:
	DECL_RTTI_NAME(LC_ClientGameEventUserData_Common)
	LC_ClientGameEventUserData_Common(int iParam1, const StringType& content);

public:
	int Param1;
	StringType Content;
};
class LC_ClientBattleRecordDataBase : public Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME( LC_ClientBattleRecordDataBase )
	LC_ClientBattleRecordDataBase();
	void Reset();
	void SetUserBaseData(PS_Rank_PlayerBaseData& data, bool bMine);
	void SetDeatil(const std::vector<BaseBattlefieldReport>& report, bool bLeft);

public:
	uint64_t		GetUniqueID();
	int32_t			GetFinalResult();
	int32_t			GetFinalValue();
	int32_t			GetLastValue();
	int32_t			GetChangeValue();
	int32_t			GetLogTime();
	StringType		GetUserBaseData(bool bMine);
	uint64_t		GetRepalyIdByIndex(int index);
	bool			GetRepalyIdValidByIndex(int index);
	int				GetBuddyCombatResult(bool bMine, int index);
	StringType		GetBuddyBaseDataByIndex(bool bMine, int index);
	StringType		GetBuddyBaseDate(BattleBuddyRecord& data);
	int				GetReportResultByIndex(bool bMine, int index);
	int				GetReportGroupIDByIndex(bool bMine, int index);
	bool			GetIsDetail();
	int				GetDetailNumber();
public:
	uint64_t							UniqueID;				//战斗索引
	PS_Rank_PlayerBaseData				m_nMyUserBaseData;		//
	PS_Rank_PlayerBaseData				m_nOtherUserBaseData;	//
	bool								m_bLeft;				//我是left还是right（这也太**了****）
	int32_t								m_nResult;				//战斗结果
	int32_t								m_CurrentValue;			//当前（排名、积分）
	int32_t								m_LastValue;			//战斗前（排名、积分）
	int32_t								m_nAddValue;			//(排名、积分)变化量
	int32_t								m_nLogTime;				//战斗发生时间戳
	bool								m_bDetail;				//是否请求过详细信息
	mem::vector<BaseBattlefieldReport>  m_nDetail;				//战斗场次详细信息
};

class LC_ClientGuessDataBase : public Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME( LC_ClientGuessDataBase )
	LC_ClientGuessDataBase() {Reset();};
	void Reset();
	void SetGuessBaseData(PS_GuessInfo_SummitInfo& data);
	void SetChoseCharID(uint64_t ChoseCharID);
	void SetPrecent(int32_t Precent,bool isLeft);
public:
	StringType		GetUserBaseData(bool bleft);
	bool			GetLooked();
	uint32_t		GetAgainstID();
	uint32_t		GetSummitType();
	uint64_t		GetChoseCharID();
	uint64_t		GetWinerCharID();
public:
	PS_Rank_PlayerBaseData				m_nLeftUserBaseData;	//左边玩家基本信息
	PS_Rank_PlayerBaseData				m_nRightUserBaseData;	//右边玩家基本信息
//	int32_t								m_LeftPrecent;          //目前左边的支持比例
//	int32_t								m_RighttPrecent;        //目前右边的支持比例
	bool								m_bGetWiner;			//是否查看过winer
	uint32_t							m_nSummitType;			//竞猜的类型
	uint32_t							m_nAgainstID;           //对战id
	uint64_t							m_nChoseCharID;         //选中的玩家id
	uint64_t							m_nWinerCharID;			//胜利者id

};

#endif
