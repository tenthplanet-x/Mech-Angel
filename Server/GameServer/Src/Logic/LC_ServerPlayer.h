//��������Ϸ���
#ifndef _LC_SERVER_PLAYER_H
#define _LC_SERVER_PLAYER_H

#include "MC_DataDirtyFlag.h"
#include "LC_TranscriptionDataType.h"
#include "LC_Define.h"
#include "LC_SocietyStruct.h"
#include "LC_PackStruct.h"
#include "LC_ItemBase.h"
#include "LC_PlayerBase.h"
#include "LC_WorldLocationAsset.h"
#include "LC_ResourceRecord.h"
#include "LC_ResourceAlert.h"
#include "LC_FacebookReward.h"
#include "NW_ServerSession.h"
#include "LC_CombatRecorderManager.h"
#include "LC_CombatRecorder.h"
#include "UT_ComplexData.h"

#include "MG_Game2DB.h"
#include "MG_ItemProtocol.h"
#include "MG_ProtocolStruct.h"
#include "char_data.pb.h"
#include "msg.pb.h"
#include "UT_ServerHelper.h"
#include "MG_BuddyProtocol.h"

#include "LC_CombatEventData.h"

#include "UT_TimeManager.h"
#include "UT_LinePath.h"
#include "UT_GameServerPackFilter.h"

#include "publish_define.h"

#include "ACT_SecretTreasure.h"
#include "ACT_ActivityAsset.h"

#include "LC_ResourceRetrieveMgr.h"
#include "CF_Buddy.h"
#include "CF_BuddyUpgrade.h"
#include "CF_EndlessTower.h"
#include "CF_WorldMapList.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "CF_ExportRobotBuddyTemplateInfo.h"
#include "CF_TranscriptionRule.h"

#include "../../../../Common/GameLogic/Include/LC_ActorAttributeMap.h"
//using namespace Buddy;
#include "../Pet/PetInstanceMgr.h"
#include "../Pet/PetInstance.h"
#include "../Bot/BotInstanceMgr.h"
#include "../Bot/BotInstance.h"
#include "../Buddy/BuddyScheme.h"
#include "../Buddy/BuddyChain.h"
#include "LC_NewVIPReward.h"
#include "LC_Reward.h"
#include "LC_ServerPlayerGuild.h"
#include "LC_ServerGoalAsset.h"
#include "LC_CollectEquip.h"
#include "CommanderAttrManager.h"
#include "../EntrustTask/EntrustTask.h"
#include "LC_ServerRewardManager.h"
#include "MG_ProtocolStruct.h"
#include "../Activity/Lottery.h"
#include "../Activity/ChargeProfit.h"
#include "../Logic/EquipPack.h"
#include "LC_SubEquipPack.h"
#include "LC_BuddyEquipData.h"
#include "LC_RuneEquipPack.h"
#include "LC_RuneData.h"
#include "LC_RedPoint.h"
#include "LC_TaskExtraReward.h"
#include "LC_RotaryTable.h"
#include "LC_NoviceTutorial.h"
#include "LC_ServerPlayerMapInfo.h"
#include "LC_PlatformInfo.h"
#include "LC_ServerPlayerIDAsset.h"
#include "LC_NonpareilReturn.h"
#include "LC_QuickRaid.h"
#include "LC_ServerGuessAsset.h"
#include "LC_ServerPlayerRecoverTimes.h"

using namespace Skill;
//namespace GameLogic {
class BuddyInstance;
class Skill::SK_SkillExecute;
//}
//#include "ACT_BuddyMgr.h"
////////////////////////////////////////////////////////////////////////////
const int MAX_CAREER_SUCCEED_NUM	= 10;

class LC_ServerNPC;
typedef mem::vector<LC_ServerNPC*>		LC_NPCPtrList;
typedef LC_NPCPtrList::iterator			LC_NPCPtrListIter;

class BuddySnapshotData
{
public:
	attr_value_type m_nCurHP;
	bool m_bDead;
};

struct sTreasureNumStruct
{
	sTreasureNumStruct(): uType(0), uCount(0) {}
	uint8_t                       uType;
	uint32_t						uCount;
};

struct LotteryRecord
{
	LotteryRecord():m_bUsedFree(0), m_nCount(0) {}
	bool                        m_bUsedFree;
	uint32_t					  m_nCount;
};
class PlayerSnapshotData
{
public:
	int16_t m_nCurrentBuddyIndex;
	//attr_value_type m_nCurHP[MAX_ACTIVE_BUDDY_NUM];
	BuddySnapshotData m_Buddy[MAX_ACTIVE_BUDDY_NUM];
};

class LC_ServerPlayer;

typedef mem::vector<LC_ServerPlayer*>	LC_PlayerPtrList;

#define SAFE_SEND_TO_CLIENT(player,msgtype,msg)		IF_A_VALID_PLAYER(player) player->SendMsgToClient(msgtype,msg);
#define SAFE_SEND_CTO_CLIENT(player,msgtype,data,size)	IF_A_VALID_PLAYER(player) player->SendMsgToClient(msgtype,data,size);

class LC_ServerMapLogic_Place;
////////////////////////////////////////////////////////////////////////////
struct PropellInfo
{
	uint64_t m_uiID;
	uint32_t m_uiFlag;
	StringType m_sInfo;

	PropellInfo(): m_uiID(0), m_uiFlag(0) {}
};
typedef mem::vector<PropellInfo> PropellInfoList;

class LC_ServerPlayer;
typedef void (* on_summonfinish_handler)(LC_ServerPlayer* player);
typedef void (* on_answer_res_result_handler)(LC_ServerPlayer* player, int resid, bool result);

namespace StateMachine
{
	class SM_StateBase;
}

struct OfflineExpData
{
	uint32_t	m_nExp;
	uint32_t	m_nOffTime;
};

struct MySyicalShopMark
{
	uint32_t					 m_nTimeStamp;
	mem::map<uint32_t, uint32_t> m_buyCntMark;
	mem::map<uint32_t, uint32_t> m_resetCntMark;
	mem::vector<uint32_t>		 m_itemidlist;
};

struct RechargeRecord
{
	RechargeRecord() : m_iLastUpdateTime(0), m_iRecharge(0) {}

	uint32_t	m_iLastUpdateTime;
	int64_t		m_iRecharge;
};

struct platform_node
{
	platform_node(): vip_type(""), is_vip(0), is_year_vip(0), vip_level(0), is_high_vip(0), is_expand_vip(0), timestamp(0) {}
	inline uint8_t format_vip_info(void) const { return FormatTencentVipInfo(vip_level, is_vip, is_year_vip, is_high_vip, is_expand_vip);}

	StringType		vip_type;
	uint8_t			is_vip;
	uint8_t			is_year_vip;
	uint8_t			vip_level;
	uint8_t			is_high_vip;
	uint8_t			is_expand_vip;
	uint32_t		timestamp;
	StringType		extend;
};
typedef mem::map<StringType, platform_node> platform_vip_info;
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class LC_ServerPlayer : public GameLogic::LC_PlayerBase
{
public:
	enum
	{
		_DBP_UPDATE_CYCLE_DELAY		= 10,	  // ���»��浽DBServer Cache   10����
		_GSP_UPDATE_ITEM			= 60,     // ������Ʒ��Ϣ        60����
		_GSP_UPDATE_CASH_OP			= 60,     // ����������Ϣ		60����

		ASSET_TIMER_IMM = 0,
		ASSET_TIMER_FAST,
		ASSET_TIMER_MEDIUM,
		ASSET_TIMER_SLOW,
		ASSET_TIMER_LAZY,
		ASSET_TIMER_MAX,
	};

	enum
	{
		CLIENT_DEBUG_BUSY,
		CLIENT_DEBUG_NORMAL,
		CLIENT_DEBUG_NOT_BUSY,
	};

	enum ServerViewState
	{
		SERVER_STATE_INVALID,			//��ʼ״̬
		SERVER_STATE_LOGINING,			//�յ��ͻ���login���󣬻�û����ͻ��˷���login��Ӧ
		SERVER_STATE_LOGINED,			//��ͻ��˷���login��Ӧ���ȴ��ͻ�����һ����Ϣ�׶�
		SERVER_STATE_WAITING,			//�յ��ͻ��˷���enter����δ��ͻ��˷���enter��Ӧ��û����ӵ���Ϸ�����Ŷ��е�״̬
		SERVER_STATE_ENTERED,			//��ͻ��˷���enter��Ӧ����û����ӵ���Ϸ����Ľ׶�
		SERVER_STATE_INWORLD,			//������Ϸ����
	};

	enum E_PlayerGroupMapOpt
	{
		E_PlayerGroupMapOpt_None = 0,	//��ʼ��
		E_PlayerGroupMapOpt_InMap,		//�����ͼ
		E_PlayerGroupMapOpt_OutMap,		//�뿪��ͼ
		E_PlayerGroupMapOpt_Clear,		//���
	};

	enum E_PlayerGroupLimit
	{
		E_PlayerGroupLimit_None = 0,	//��Լ��
		E_PlayerGroupLimit_Local,		//���ض���
		E_PlayerGroupLimit_Cross,		//�������
		E_PlayerGroupLimit_All,			//���ж���
	};

	enum E_AttackType
	{
		E_AttackType_Attack	= 0,		//����
		E_AttackType_Kill,				//��ɱ
	};

	typedef enum 
	{
		eControlType_Init,
		eControlType_Player,
		eControlType_Buddy,
		eControlType_Count,
	} PLAYER_CONTROL_TYPE;

	typedef enum
	{
		eSwitchBuddyMode_Normal,
		eSwitchBuddyMode_DataOnly,
		eSwitchBuddyMode_NoticeOnly,
		eSwitchBuddyMode_Count,
	} SWITCH_BUDDY_MODE;

	typedef struct 
	{
		// �Զ����ߵİ뾶
		int32_t m_nProperty_ChaseRadius;
		// ����������ǵľ���
		int32_t m_nProperty_FollowDistance;
		// ֹͣ��ʱ��������ǵľ���
		int32_t m_nProperty_StandDistance;
	} PlayerAIData;

	typedef enum
	{
		eRelativeIndex_Error = -1,
		eRelativeIndex_A,
		eRelativeIndex_B,
	} BUDDY_RELATIVE_INDEX;

	typedef enum
	{
		eBuddyActionType_Init,
		eBuddyActionType_FollowOwner,
		eBuddyActionType_ForceFollowOwner,
		eBuddyActionType_FollowEnemy,
		eBuddyActionType_AttackEnemy,
		eBuddyActionType_Count,
	} BUDDY_ACTION_TYPE;

	typedef struct 
	{
		//����
		int32_t  nSeasonId;
		// ��ս����
		int32_t nDailyChallengTimes;
		// ˢ�´���
		int32_t nDailyRefreshTimes;
		int32_t  nGetGradeAward;
		int32_t  nGetHonorValue;
		int32_t  nTopScore;//��ǰ������߻���
		int32_t  nNowScore;//
		int32_t  nMaxScore;//��߻���
		void Reset()
		{
			nDailyChallengTimes = 0;
			nDailyRefreshTimes = 0;
		}
		void Clear(int nseasonId)
		{
			Reset();
			nSeasonId = nseasonId;
			nGetHonorValue = 0;
			nGetGradeAward = 0;
			nTopScore = 0;
		}
	} PlaySkyArenaData;
		

	LC_ServerPlayer();
	virtual ~LC_ServerPlayer();

	DECL_RTTI_NAME(LC_ServerPlayer);

	virtual void		InitRobotLogicData(int32_t soloDuleIndex);
	bool InitRobotOwnerPlayerInfo(const CF_ExportSoloDuleRobotInfo::DataEntry* pkData);
	BuddyInstance* InitBuddyInfo(int buddyID,int buddyTemplateID);
	void SetBuddyIndex(int index);
	void SetBuddyTemplateID(int buddyID);
	void SetAssignedBuddyInstance(int buddyID);


	virtual void		InitLogicData();
	virtual void		Release();
	virtual void		Update(float fCurrentTime, float fDeltaTime);
	virtual void		UpdateSync(float fCurrentTime, float fDeltaTime);
	void Update_CacheForPerf();
	//void				SetVIPLevel(int lVIPLevel){m_lVIPLevel = lVIPLevel;}

	void				SetLoginIP(uint32_t lIP) {m_uiLoginIP = lIP;}
	uint32_t			GetLoginIP() {return m_uiLoginIP;}

	void				SetClientIP(uint32_t lIP) {m_uiClientIP = lIP;}
	uint32_t			GetClientIP() {return m_uiClientIP;}

	void				SetClientMAC(uint64_t lMAC) {m_ulClientMAC = lMAC;}
	uint64_t			GetClientMAC() {return m_ulClientMAC;}

	void				SetGetGuildData(bool bValue) {m_bGetGuildData = bValue;}
	bool				IsGetGuildData(void) {return m_bGetGuildData;}

	//�ڴ渴��
	virtual void		ReUse();

	float				GetLastHeartBeatTime() {return m_fLastHeartBeatTime;};
	void				UpdateWithServerTime(int lServerTime);
	void				UpdateLogicAreaInfo();
	void				ResetLogicAreaID(GameLogic::LC_LogicAreaInfo* pkAreaInfo);
	void				UpdateWorldLocationAssetData(bool nChangeMap = false);
	void				ClearTemporaryLocationAssetData();

	bool IsInBaseSkill();

	void				UpdateVehicleSpeedUP(float fDelta);

	virtual void		SetDead(bool bIsDead);
	virtual void		SyncDead( bool bSyncOnly = false );

	virtual const LC_BackPackEntry* GetItemEntryByItemID(int32_t itemID);
	void SetPlayerBaseLevel(int lLevel);
	void SetAllPlayerBaseLevel(int lLevel);
	//����
	virtual void		SetLevel(int lLevel, bool bInit = false);
	virtual int			GetLevel();
	virtual void		SetIsShowFashion(bool bIsShowFashion);
	virtual int OnTitleChanged( int32_t nNewTitle );

	void TransferBattlePlaybackMap(int32_t mapResId);
	virtual ResultType TransferMap(int transferType, int mapID, int index, int serverID, bool tribuMap, int8_t lineNum);
	virtual void ChangeMap(int iMapLogicID, int lMapResID, int lMapLogicType, const Utility::UT_Vec3Int& rkLocation, bool bAsBuddy = false);
	void PostEnterMap(int iMapLogicID, int lMapResID, int lMapLogicType);

	int CheckEnterPlanesMap(int iDstMapLogicID, int lDstMapResID);

	virtual bool		AddEXP(attr_value_type lExp, int32_t eSrcType, uint64_t lSrcTypeID, bool bIgnoreEXPRate = false, int nLogCode = 0 ,const vector<int> &Params = vector<int>());
	virtual bool		AddEXP(attr_value_type lExp);
	virtual bool		ReduceEXP(attr_value_type lExp, int32_t eSrcType);
	virtual bool		ReduceSP(attr_value_type lValue, int32_t eSrcType);

	virtual void		AddSkillLevelupPoints(attr_value_type Value, int32_t eSrcType, uint64_t lSrcTypeID = 0, bool AntiWallow = true);
	virtual LC_SkillAsset* GetSkillAsset();
	virtual LC_SkillAsset* GetControllerSkillAsset();

	virtual LC_ItemCoolDownMapBase* GetControllerItemCoolDownMap();

	void NoticeAddAccount(int32_t eSrcType, MoneyType llCash, int32_t param);

	virtual bool Charge(int32_t chargeMoney, int32_t reason,const std::string& strOrderId);

	//��Ǯ
	virtual bool		AddCash(MoneyType llCash, CashType eCashType, int32_t eSrcType, uint64_t lSrcTypeID, int32_t param=0, bool AntiWallow = true, MoneyType* pllDelta = NULL,int nLogCode = 0 ,const vector<int> &Params = vector<int>());
	virtual bool		ReduceCash(MoneyType llCash, CashType eCashType, int32_t eSrcType, uint64_t lSrcTypeID, int nLogCode = 0 ,const vector<int> &Params = vector<int>());

	bool				SetCash(MoneyType llCash, CashType eCashType);

	virtual void ResetCash(CashType eCashType);

	virtual ResultType	GetYiJiTuXiRankInfo(int minIndex, int maxIndex, MG_RLT_YiJiTuXiRankInfo& msg);
	virtual void			UpdatePointToRedis(CashType cashType, MoneyType cashValue);

	virtual void SetSendGuildPresentInfoFlag(bool flag);
	virtual bool GetSendGuildPresentInfoFlag();
	virtual void SetGuildPresentFlag(bool tag);
	virtual bool GetGuildPresentFlag();
	void SyncGuildPresentFlag();

	virtual ResultType GetGuildPresentReward(const std::set<uint16_t>& idSet, std::map<uint16_t,string>& rewardInfo);
	virtual ResultType GetGuildCrystalReward(std::vector<uint32_t>& ids);
	virtual void AddGuildPresentExp(int32_t expType, int num);
	
	virtual ResultType GetTreasurePos(int& mapID, int& x, int& y, int& z);
	virtual void SyncTreasurePos();
	virtual int RandomTreasureEvent(int itemID);
	virtual int GetTreasureEventType(int eventIndex);
	virtual int CastTreasureEvent_AddItem(int eventIndex);
	virtual int CastTreasureEvent_SwapnNPC(int eventIndex);
	virtual int CastTreasureEvent_TransferMap(int eventIndex);
	virtual void ResetTreasurePos();

	int32_t UpdateGuildMapBossDamage(mem::vector<uint64_t>& params, uint64_t deltaData, uint64_t& totalDamage);
	ResultType GetGuildMapBossRank(MG_RLT_GetGuildMapBossRank& rltMsg);

	void SyncGameRestartNotice();
	ResultType GetGameRestartReward(int id);

	bool				AddCredit(int64_t iCredit, CashType eCashType, int32_t reason);

	virtual bool		AddGuildMoney(MoneyType delta, int32_t iSrcType, uint64_t iSrcID);
	virtual bool		DelGuildMoney(MoneyType delta, int32_t iSrcType, int32_t iSrcID);
	virtual bool		CheckGuildMoney(MoneyType delta);
	
	virtual void		SetTotalGuildMoney(MoneyType point);
	virtual MoneyType GetTotalGuildMoney();

	virtual void		SetGuildMoney(MoneyType point);
	virtual MoneyType	 GetGuildMoney();

	virtual void AddGuildPresent(int32_t itemID, int8_t way);

	virtual bool IsGuildAutoAdd();
	virtual void SetGuildAutoAdd(bool autoadd);

	virtual bool		AddCouragePoint(int64_t delta, int32_t iSrcType, uint64_t iSrcID, int nLogCode = 0 ,const vector<int> &Params = vector<int>());
	virtual bool		DelCouragePoint(int64_t delta, int32_t iSrcType, uint64_t iSrcID, int nLogCode = 0 ,const vector<int> &Params = vector<int>());
	virtual bool		CheckCouragePoint(int64_t delta);

	virtual bool		AddHonor(int64_t delta, int32_t iSrcType, uint64_t iSrcID);
	virtual bool		DelHonor(int64_t delta, int32_t iSrcType, uint64_t iSrcID);
	virtual bool		CheckHonor(int64_t delta);

	// CombatEventBonusData
	virtual void RegisterCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE nValue );
	virtual void UnregisterCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey );
	virtual void ModifyCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE nDeltaValue );
	virtual bool GetCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE& nValue );
	virtual COMBAT_EVENT_VALUE_TYPE GetCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey );

	virtual	bool		Respawn(attr_value_type lHP, int lMP);
	int					GetSpawnMapID() {return m_lSpawnMapID;};
	void				SetSpawnMapID(int lMapID) {m_lSpawnMapID = lMapID;};

	//Begin Skill �ӿ�
	virtual void		NotifySkillEvent(Skill::SK_SkillEvent* pkSkillEvent);

	//��鼼���Ƿ��ܹ�ʹ�ã��Լ�������Ʒ�Ƿ���Ч
	virtual ResultType	CheckExecuteSkill(float fCurrentTime, Skill::SK_SkillExecute* pkSkill, int lItemID);

	void				AIFillExecuteSkillParam(Skill::SK_SkillExecute* pkSkill);

	//ִ�м��ܣ�������״̬��������ִ��Update
	virtual ResultType	ExecuteSkill(float fCurrentTime, Skill::SK_SkillExecute* pkSkill, int lItemID);
	//������˲�����ܣ����޸�״̬����ǰ״̬
	ResultType			ExecuteSkillImmediately(float fCurrentTime, Skill::SK_SkillExecute* pkSkill, int lItemID);
	ResultType			ExecuteLCSkillTargetImmediately(float fCurrentTime, Skill::SK_SkillExecute* pkSkill,object_id_type lLockedID);
	//���������˲�����ܣ����޸�״̬����ǰ״̬
	ResultType			ExecuteLCSkillImmediately(float fCurrentTime, Skill::SK_SkillExecute* pkSkill,object_id_type lLockedID, UT_Vec3Int lockedLocation, float dirAngle);

	virtual LC_SkillBookAsset&		GetSkillBookAsset();

	virtual ResultType SkillBookUpgrade(int32_t id, int32_t pos);

	//�ж���ִ�еļ���
	virtual ResultType	InterruptSkill(float fCurrentTime, int lSkillTypeID, SkillInterruptType eInterruptType);

	virtual	bool		LearnSkill(int lSkillID, int& iSlotIdx, int32_t runeSystemType=-1, int32_t runeSystemParam=-1, GameLogic::LC_SkillIDSet* pkReplacedSkills = NULL);
	virtual	bool		RemoveSkill(int lSkillID);
 
	virtual	bool	LearnSkillToBuddyByIndex(int32_t nBuddyIndex, int lSkillID, int& iSlotIdx, int32_t runeSystemType=-1, int32_t runeSystemParam=-1, LC_SkillIDSet* pkReplacedSkills=NULL);
	virtual	bool	RemoveSkillToBuddyByIndex(int32_t nBuddyIndex, int lSkillID);

	bool LearnSkillToBuddy(int32_t nBuddyId, int lSkillID, int& iSlotIdx, int32_t runeSystemType=-1, int32_t runeSystemParam=-1, GameLogic::LC_SkillIDSet* pkReplacedSkills = NULL, bool bSyncLearnSkillEvent=true);
	bool RemoveSkillToBuddy(int32_t nBuddyId, int lSkillID);

	virtual ResultType  UpdateSkillInSkillAsset(int iCategory, int& iEntryIndex, int iMax);

	virtual LC_PKRecorderBase* GetPKRecorder();
	virtual PKStateType GetPKState();

	//End Skill  �ӿ�
	virtual LC_ShortcutBar*			GetShortcutBar();
	//�ʼ�ϵͳ
	ResultType			CheckWriteMail(Protocol::PS_WriteMailInfo& kWriteMail, MoneyType& iPostage);
	ResultType			WriteMail(Protocol::PS_WriteMailInfo& kWriteMail, Protocol::PS_MailInfo& kMailInfo);

	virtual void GetBuddyPresentElem(mem::vector<int>& elemVec);
	virtual DamageElementType GetDamageElementType();

	void				SetUserID(user_id_type dwUserID) { m_dwUserID = dwUserID; };
	user_id_type		GetUserID() { return m_dwUserID; };

	void				SetUserName(const StringType& rkName);
	const StringType& 	GetUserName();
	const StringType& 	GetUserName_utf8();

	virtual void SetCharName(const StringType& rkCharName);

	virtual const StringType& 	GetOwnerCharName();
	virtual const StringType& 	GetOwnerCharName_utf8();

	virtual ResultType HandleBonusItems(int32_t iPackType, const Utility::UT_SIMDataList& items, int32_t iReasonType, uint64_t iReasonDetail, const StringType& title, const StringType& Desc, bool toMail = true, bool bAutoUse = false, bool bLotteryLogic = false);

	void				Send(char* pMsg, uint16_t wSize);

	bool				SendMsgToClient(int32_t iMsgType, Protocol::MG_BODY* pkMsgBody = NULL, LC_ServerPlayer* pOwnerPlayer = NULL);
	bool				SendMsgToClient(int32_t iMsgType, const char* pMsg, uint16_t nSize);

	bool				SendNotifyStrToClient(const StringType& msg, PromptChannel msgchannel = PROMPT_CHANNEL_ANNOUNCE, PromptLevel msglevel = PROMPT_LEVEL_LOW);
	bool				BroadCastToArea(int32_t iMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter = NULL);

	bool				BroadCastTransferToAreaWithoutMe(int meID, int32_t iMsgType, Protocol::MG_BODY* pkMsgBody = NULL);

	void				PackNetPlayerAttribute(Protocol::MG_RLT_NetPlayerAttribute& rkMsg);
	void				EquipAllBuddyItem(bool login);

	virtual	void		PunchMove(SkillMoveActorPositionType Type, const Utility::UT_Vec3Int& kTargetPoint, float fTime);

	bool CheckItemReqInPackage(LC_PlayerBase* pkPlayer, int32_t nBuddyId, int iPackType, int iEntryID, int lItemID);

	void				UpdateCashOP(void);

	inline bool			IsAssetUPTimerExpired(size_t i)
	{
		if (m_kUpdateAssetTimers.empty())
		{
			return true;
		}
		int idx = UT_MathBase::MaxInt((int)i, (int)m_kUpdateAssetTimers.size() - 1);
		return m_kUpdateAssetTimers[idx].IsExpired();
	}

	//����˳��ƽ̨�ۼƵ�¼��������
	ResultType GetReward_ShunWangPlatform(int32_t type, int32_t param);
	//����˳��ƽ̨��¼����
	ResultType GetReward_ShunWangLoginReward(int32_t type, int32_t param);
	//˳��ƽ̨������ȼ�����
	ResultType GetReward_ShunWangLevel(int32_t type, int32_t param);
	//��������Խ���
	ResultType GetReward_AntiWallowReward(int32_t type, int32_t param);

	void				SyncGuildMapInfoToPlayer();
	void				SyncPetInfoToClient();
	void				SyncBotInfoToClient();
	void				SyncGuildRewardState();
	void				SyncControllerPropertyToClient();
	void				SyncPropertyToClient( bool bWithoutBuddy = false );
	void				SyncSocietyToClient();

	void				SyncCommanderAttributeToClient(uint32_t uSyncType,LC_ActorAttributeMap&SyncAttributeMap, bool login, uint64_t ucombatscore = 0, int32_t reasonType=SPART_None);
	virtual void	SyncBackPackToClient(int iPackType, bool bIsAllPack = false);

	void				SyncMailBoxToClient();
	void				SyncTaskTypeDataToClient();
	void				SyncAssistInfoSetToClient();
	void				SyncProcessingTaskMapToClient(bool login=false);
	void				SyncFinishedTaskMapToClient();
	void				SyncFailTaskMapToClient();
	void				SyncSkillAssetToClient( bool bForce = false );
	void				SyncBuddySkillAssetToClient( bool bForce = false );
	void				SyncSkillStatesToClient(int iStateCategory);
	void				SyncCashToClient();
	void				SyncYuanBaoToClient();
	void				SyncPentacleToClient();

	void				SyncRuneCreamToClient();
	void				SyncStarSoulToClient();
	
	void				SyncCustomCashToClient();
	void				SyncTradePackToClient();
	void				SyncPKStateToClient();
	void				SyncSummorFriendToClient();
	void				SyncFightStateToClient();
	void				SyncNPCInfoInFOVToClient();
	void				SyncPlayerInfoInFOVToClient();
	void				SyncPlaceHolderInfoInFOVToClient(float fCurrentTime);
	void				SyncPlayerShortcutBarToClient();
	void				SyncPlayerForbidListToClient();
	void				SyncTitleNameInfoToClient();
	void				SyncAchievementInfoToClient();
	void				SyncSkillCDModifyToClient();
	void				SyncItemCDToClient();
	void				SyncTutorialInfo();
	virtual void	SyncAssignTaskInfo();
	void				SyncTaskChapterRecord();
	void				SyncShapeShiftInfo();
	void				SyncTransportInfo();
	void				SyncRaidInfo();
	void				SyncRaidEnterMapRecord(int mapID=0);
	void				SyncCrossRealmTeamInfo();
	void				SyncPortal();
	void				SyncOpenBag();
	void				SyncGuildWarInfo();
	void				SyncSignIn(int type);
	void				SyncAIcontrol();
	void				SyncShortcutKeyInfo();
	void				SyncFactionToClient();
	void				SyncActionForbidInfoToClient();
	void				SyncArenaInfoToClient();
	void				SyncArenaBattleToClient();
	void				SyncRankAssertToClient();
	void				SyncDomainAssertToClient();
	void				SyncActorOperationAssetToClient();
	void				SyncDefaultTransferPos();
	void				SyncLocationInfo();
	void				SyncEnterFov();
	void				SyncLeaveFov( object_id_type nID );
	void				SyncAllBuddyLeaveFov();
	void				SyncAllBuddyEnterFov();
	void				SyncBuddyNetPlayer();
	void				SyncAIControlLocation();
	virtual void				SyncVIPInfo();
	void				SyncPromoteInfo();
	void				SyncWorldPowerGuild();
	void				SyncGreatKungFuInfo();
	void				SyncGMInfo();
	void				SyncUpdateBulletin();
	virtual void	SyncOpenAbilityInfo();
	virtual void	SyncSubordinateInfo(bool& bDirty);
	void				SyncSkillBookInfo();
	void				SyncOpenNewSkillBook(int32_t iID);
	void				SyncTreasureChestsInfo();
	void				SyncCommonStatInfo(bool& bDirty);
	void				SyncGameStoryAsset();
	void				SyncCommonShopInfo(bool bSelf = true, bool bGlobal = true);

	void				SyncFashionInfo(void);
	void				SyncSecretTreasureInfo(void);
	void				SyncACTScheduleInfo(bool& bDirty);

	void				SyncAllBuddyInstance();
	void				SyncActiviteBuddyProperty( bool bWithoutBuddy = false );
	void				SyncAllInfo(bool bAsBuddy = false, bool bRefresh = false);
	void				SyncAllInfo_Lite();
	void				SyncSocietyDynamicInfo();
	void				SyncDailyCleanInfo();
	void				SyncRewardGainInfo();
	void				SyncACTActiveRewardInfo(int32_t id, int32_t count);
	void				SyncBossFirstKillInfo(int32_t npcID, BossFirstKillPlayerInfo& info);
	void				SyncAllBuddyRuneLevelnfo();
	void				SyncACTActiveRewardInfoB(int32_t id, int32_t count);

	void				SyncAsControleeInfo(void);
	void				SyncAsControlerInfo(void);

	virtual int			GetTotalEquipSlotLevelByBuddyID(int buddyID);

	void				ActivePassiveSkill();

	void				FillLastBiographyPartID();
	void				UpdateBiographyOldData();
	void				CheckAndSyncPackInfo(float fCurrentTime);
	void				CheckCanAssignNextBiographyTask(bool tag = false);
	bool				IsBiographyPartAlreadyAssign(int biographyPartID);

	void				SyncPlayerEnterFOVToClient();
	void				SyncPlayerLeaveFOVToClient();

	void				AsyncUpdateToDBServer(void);
	void				UpdateToDBServer(bool isleave = false,ResultType nReason = 0);

	void				SetSession(NW_ServerSession* pkSession);
	
	virtual int32_t GetActiveServer();

	bool				FirstEnterWorld();

	NW_ServerSession*	GetSession();
	uint64_t            GetSessionId();

	void				WriteCharbaseTest(int32_t size);

	void SetBaseDataStr( int32_t nIndex, const StringType& strInput );
	void EchoBaseDataStr( int32_t nIndex);

	void SetBaseDataInt( int32_t nIndex, int32_t nValue );
	void EchoBaseDataInt( int32_t nIndex);
private:
	Protocol::PS_CharBaseInfoLite m_PreLoad_Lite;
	std::string m_PreLoad_BaseData;
	std::string m_PreLoad_FullData;
	std::string m_PreLoad_Pack;
	std::string m_PreLoad_Development;
	std::string m_PreLoad_Hero;
	std::string m_PreLoad_PlayerBase;
	std::string m_PreLoad_Offline;
public:
	void PreLoadCharData_Lite(const Protocol::PS_CharBaseInfoLite& linfo);
	void PreLoadCharData_Base(const std::string& strInfo, bool bIsEnd);
	void PreLoadCharData_Full(const std::string& strInfo, bool bIsEnd);
	void PreLoadCharData_Pack(const std::string& strInfo, bool bIsEnd);
	void PreLoadCharData_Development(const std::string& strInfo, bool bIsEnd);
	void PreLoadCharData_Hero(const std::string& strInfo, bool bIsEnd);
	void PreLoadCharData_PlayerBase(const std::string& strInfo, bool bIsEnd);
	void PreLoadCharData_Offline(const std::string& strInfo, bool bIsEnd);
	void ParseCharDataFromPB(bool clonePlayer);

	bool PreSaveCharData_Lite(const Protocol::PS_CharBaseInfoLite& linfo);
	bool PreSaveCharData_Base(const std::string& strInfo);
	bool PreSaveCharData_Full(const std::string& strInfo);
	bool PreSaveCharData_Pack(const std::string& strInfo);
	bool PreSaveCharData_Development(const std::string& strInfo);
	bool PreSaveCharData_Hero(const std::string& strInfo);
	bool PreSaveCharData_PlayerBase(const std::string& strInfo);
	bool PreSaveCharData_Offline(const std::string& strInfo);
	bool PreSaveCharData_Status(int32_t nFlag,ResultType nReason = 0);
	bool SaveAllDataToDB( const Protocol::MG_GD_Save_Char_PB& data,ResultType nReason = 0 );	//��db����save

	void				LoadCharPB(const Protocol::PS_CharBaseInfoLite& linfo, const std::string& binfostr, const std::string& cinfostr, bool clonePlayer=false);
	void				LoadCharPB_Pack( const std::string& _infoStr );
	void				LoadCharPB_Development( const std::string& _infoStr );
	void				LoadCharPB_Hero( const std::string& _infoStr );
	void				LoadCharPB_PlayerBase( const std::string& _infoStr );
	void				LoadCharPB_Offline( const std::string& _infoStr );
	void				SaveCharPB(bool isleave = false,ResultType nReason = 0);
	void				SaveCharPB(Protocol::PS_CharBaseInfoLite& info, char_data_define::char_base_info& binfo, char_data_define::char_full_info& cinfo);
	void				SaveCharPB_Pack( char_data_define::PBPackData& _info );
	void				SaveCharPB_Development( char_data_define::PBDevelopmentData& _info );
	void				SaveCharPB_Hero( char_data_define::PBHeroData& _info );
	void				SaveCharPB_PlayerBase( char_data_define::PBPlayerBaseData& _info );
	void				SaveCharPB_Offline( char_data_define::PBOfflineData& _info );

	void				LoadCharLiteInfo(const Protocol::PS_CharBaseInfoLite& info, bool clonePlayer);
	void				SaveCharLiteInfo(Protocol::PS_CharBaseInfoLite& info);

	void				PBSaveCharLiteInfo(char_data_define::char_lite_info& info);

	void				PBLoadCharBaseInfo(const char_data_define::char_base_info& info);
	void				PBSaveCharBaseInfo(char_data_define::char_base_info& info);

	void				PBLoadCharDetailInfo(const char_data_define::char_detail_info& info);
	void				PBSaveCharDetailInfo(char_data_define::char_detail_info& info);

	void				PBLoadCharWorldLocationInfo(const char_data_define::world_location_asset& info);
	void				PBSaveCharWorldLocationInfo(char_data_define::world_location_asset& info);

	void				PBLoadCharSkillInfo(const char_data_define::skill_asset_info& info);
	void				PBSaveCharSkillInfo(char_data_define::skill_asset_info& info);

	// Pet
	void				PBSavePetDetailData(char_data_define::pet_detail_data & detail_data);
	void				PBLoadPetDetailData(const char_data_define::pet_detail_data & detail_data);

	// Bot
	void				PBSaveBotDetailData(char_data_define::bot_detail_data & detail_data);
	void				PBLoadBotDetailData(const char_data_define::bot_detail_data & detail_data);

	// NewVIP
	void				PBSaveNewVIPData(char_data_define::new_vip_reward & detail_data);
	void				PBLoadNewVIPData(const char_data_define::new_vip_reward & detail_data);

	// Buddy Chain
	void				PBSaveBuddyChain(char_data_define::buddy_chain_data & detail_data);
	void				PBLoadBuddyChain(const char_data_define::buddy_chain_data & detail_data);

	// Buddy
	void				PBSaveBuddySummaryData(char_data_define::buddy_summary_data & info);
	void				PBLoadBuddySummaryData(const char_data_define::buddy_summary_data & summary, const char_data_define::buddy_detail_data & detail );

	void				PBSaveBuddyDetailData(char_data_define::buddy_detail_data & detail);
	void				PBLoadBuddyDetailData(const char_data_define::buddy_detail_data & detail );

	void				PBLoadBuddyData_Usable(const char_data_define::buddy_summary_data& info);
	void				PBLoadBuddyData_Activate(const char_data_define::buddy_summary_data& info);
	void				PBLoadBuddyData_Detail(const char_data_define::buddy_summary_data & summary, const char_data_define::buddy_detail_data & detail );

	int				LoadBuddyData_Locked();

	void				LoadBuddyData_Usable();
	void				LoadBuddyData_Activate();
	
	BuddyInstance*		LoadBuddyData_SingleInstance( const CSVFile::CF_Buddy::DataEntry* pTemplate, bool bCreate);

	BuddyInstance* CreateSingleBuddyInstance( const CSVFile::CF_Buddy::DataEntry* pTemplate, bool bCreate);
	template<typename PackType>
	void				PBLoadCharItemPackInfo(const char_data_define::item_pack& info , PackType& pack , bool bIgnore = false)
	{
		pack.GetFromPB(info, bIgnore);
	}

	template<typename PackType>
	void				PBSaveCharITemPackInfo(char_data_define::item_pack& info , PackType& pack , bool bIgnore = false)
	{
		pack.SetToPB(info, bIgnore);
	}

	void				PBLoadTaskTypeInfo(const char_data_define::task_type_asset_info& info);
	void				PBSaveTaskTypeInfo(char_data_define::task_type_asset_info& info);

	void				PBLoadProcessingTaskInfo(const char_data_define::processing_task_asset_info& info);
	void				PBSaveProcessingTaskInfo(char_data_define::processing_task_asset_info& info);

	void				PBLoadTaskChapterRewardInfo(const ::google::protobuf::RepeatedField<::google::protobuf::uint32>& info);
	void				PBSaveTaskChapterRewardInfo(::google::protobuf::RepeatedField<::google::protobuf::uint32>& info);

	void				PSLoadCharFinishedTaskInfo(const char_data_define::finished_task_asset_info& info);
	void				PSSaveCharFinishedTaskInfo(char_data_define::finished_task_asset_info& info);

	void				PBLoadFailTaskInfo(const char_data_define::fail_task_asset_info& info);
	void				PBSaveFailTaskInfo(char_data_define::fail_task_asset_info& info);

	void				PBLoadLivenessInfo(const char_data_define::char_full_info& info);
	void				PBSaveLivenessInfo(char_data_define::char_full_info& info);

	void				PBLoadAssignData(const char_data_define::char_full_info& info);
	void				PBSaveAssignData(char_data_define::char_full_info& info);

	void				PBLoadCharShortcutBarInfo(const char_data_define::shortcutbar_info& info);
	void				PBSaveCharShortcutBarInfo(char_data_define::shortcutbar_info& info);

	void				PBLoadTitleNameInfo(const char_data_define::titlename_info& info);
	void				PBSaveTitleNameInfo(char_data_define::titlename_info& info);

	void				PBLoadGoalInfo(const char_data_define::goal_info& info);
	void				PBSaveGoalInfo(char_data_define::goal_info& info);

	void				PBLoadResourceRetrieveInfo(const char_data_define::char_ResourceRetrieve& info);
	void				PBSaveResourceRetrieveInfo(char_data_define::char_ResourceRetrieve& info);
	
	void				PBLoadPKStateInfo(const char_data_define::pk_state_info& info);
	void				PBSavePKStateInfo(char_data_define::pk_state_info& info);

	void				PBLoadCharItemCDInfo(const char_data_define::item_cd_info& info);
	void				PBSaveCharItemCDInfo(char_data_define::item_cd_info& info);

	void				PBLoadCharSkillStateInfo(const char_data_define::save_skill_state_info& info);
	void				PBSaveCharSkillStateInfo(char_data_define::save_skill_state_info& info);

	void				PBLoadGameStoryInfo(const char_data_define::game_story_asset_info& info);
	void				PBSaveGameStoryInfo(char_data_define::game_story_asset_info& info);

	void				PBLoadChar_Raid_Transport_AssetInfo(const char_data_define::transport_raid_asset_info& info);
	void				PBSaveChar_Raid_Transport_AssetInfo(char_data_define::transport_raid_asset_info& info);

	void				PBLoadCharShortcutKeyAssetInfo(const char_data_define::shortcut_key_asset_info& info);
	void				PBSaveCharShortcutKeyAssetInfo(char_data_define::shortcut_key_asset_info& info);

	void				PBLoadVIPInfo(const char_data_define::char_vip_info& info);
	void				PBSaveVIPInfo(char_data_define::char_vip_info& info);

	void				PBLoadAssistInfo(const char_data_define::char_assist_info& info);
	void				PBSaveAssistInfo(char_data_define::char_assist_info& info);

	void				PBLoadSubordinateInfo(const char_data_define::char_subordinate_info& info);
	void				PBSaveSubordinateInfo(char_data_define::char_subordinate_info& info);

	void				PBSaveArenaBattleAssertInfo(char_data_define::char_arena_info& info);
	void				PBLoadArenaBattleAssertInfo(const char_data_define::char_arena_info& info);

	void				PBSavePortalAssertInfo(char_data_define::char_portal_info& info);
	void				PBLoadPortalAssertInfo(const char_data_define::char_portal_info& info);

	void				PBSaveRankGoodRecord(char_data_define::char_rank_good_record& info);
	void				PBLoadRankGoodRecord(const char_data_define::char_rank_good_record& info);

	void				PBSaveDomain(char_data_define::char_full_info& info);
	void				PBLoadDomain(const char_data_define::char_full_info& info);

	void				PBSaveSignInAssertInfo(char_data_define::char_signin_info& info);
	void				PBLoadSignInAssertInfo(const char_data_define::char_signin_info& info);

	void				PBSave360MicroSignInAssertInfo(char_data_define::char_signin_info& info);
	void				PBLoad360MicroSignInAssertInfo(const char_data_define::char_signin_info& info);

	void				PBSaveGuildWarRecord(char_data_define::char_guildwar_info& info);
	void				PBLoadGuildWarRecord(const char_data_define::char_guildwar_info& info);

	void				PBSaveCrossRealmTeam(char_data_define::crossrealmteam_map_assert& info);
	void				PBLoadCrossRealmTeam(const char_data_define::crossrealmteam_map_assert& info);

// 	void				PBSavePromoteInfo(char_data_define::char_promote_info& info);
// 	void				PBLoadPromoteInfo(const char_data_define::char_promote_info& info);
// 
// 	void				PBSaveGreatKungFuInfo(char_data_define::char_great_kungfu_info& info);
// 	void				PBLoadGreatKungFuInfo(const char_data_define::char_great_kungfu_info& info);

	void				PBSaveOpenAbilityInfo(char_data_define::char_open_ability_info& info);
	void				PBLoadOpenAbilityInfo(const char_data_define::char_open_ability_info& info);

	void				PBSaveCommonStatInfo(char_data_define::char_common_stat_info& info);
	void				PBLoadCommonStatInfo(const char_data_define::char_common_stat_info& info);

	void				PBSaveCommonItemRecordSet(::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& info);
	void				PBLoadCommonItemRecordSet(const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& info);

	void				PBSaveTreasureChestsInfo(char_data_define::char_treasure_chests_info& info);
	void				PBLoadTreasureChestsInfo(const char_data_define::char_treasure_chests_info& info);

	void				PBSaveCommonShopInfo(char_data_define::char_common_shop_info& info);
	void				PBLoadCommonShopInfo(const char_data_define::char_common_shop_info& info);

	void				PBSaveActivationCodeInfo(char_data_define::char_activation_code_info& info);
	void				PBLoadActivationCodeInfo(const char_data_define::char_activation_code_info& info);

	void				PBSaveAttributePlusInfo(char_data_define::attribute_plus_info& info);
	void				PBLoadAttributePlusInfo(const char_data_define::attribute_plus_info& info);

	void				PBSaveChatMonitorInfo(char_data_define::char_chat_monitor_info& info);
	void				PBLoadChatMonitorInfo(const char_data_define::char_chat_monitor_info& info);

	void				PBSaveCombatRecorder(char_data_define::combat_recorder& info);
	void				PBLoadCombatRecorder(const char_data_define::combat_recorder& info);

	void				PBSaveSecretTreasureInfo(char_data_define::secret_treasure_info& info);
	void				PBLoadSecretTreasureInfo(const char_data_define::secret_treasure_info& info);

	void				PBSaveActivityInfo(char_data_define::char_activity_info& info);
	void				PBLoadActivityInfo(const char_data_define::char_activity_info& info);

	void				PBSaveBuddyAutoRespawnTimer( ::google::protobuf::RepeatedField< float > & vecData );
	void				PBLoadBuddyAutoRespawnTimer( const ::google::protobuf::RepeatedField< float > & vecData );

	void				PBSaveArenaInfo(char_data_define::char_global_arena_info& info);
	void				PBLoadArenaInfo(const char_data_define::char_global_arena_info& info);

	virtual void LoadSocietyBeID(int iSocietyType, mem::vector<unique_id_type>& id);

	//��ɫ��������
	void				SaveCharInfoToFriendDB(bool force);
	void				fillCharFriendInfo(PS_FriendInfo& kCharInfo);
	uint32_t			GetRelationFlag(const unique_id_impl& charID);
	ResultType			ChangeCharFriendRelation(ResultType rslt, const char_data_define::friend_relation_info& relation, const char_data_define::friend_char_info& char_info, const std::string& msg);

	//���ݿ�洢����
	bool				GetDBSaveMaskFlag(GameLogic::LC_PlayerDataSaveType eSaveType);
	void				SetDBSaveMaskFlag(GameLogic::LC_PlayerDataSaveType eSaveType, bool bSaveFlag);

	void				InitGameData(bool clonePlayer=true);
	void				InitGameData_AllBuddy();
	void				GetFriendDataFromDB();
	void				GetGuildDataFromDB(uint64_t guildId = 0, bool newAdd=false);
	bool				IsInGame();
	void				LeaveGame( bool bException = false ,ResultType nReason = 0 );
	void				StopTrade();

	bool				GetLoadDataFromLoginFlag() { return m_bLoadDataFromLoginFlag; };
	void				SetLoadDataFromLoginFlag(bool bLoad) { m_bLoadDataFromLoginFlag = bLoad; };

	bool				GetLoadDataFromDBFlag() { return m_bLoadDataFromDB; };
	void				SetLoadDataFromDBFlag(bool bLoad) { m_bLoadDataFromDB = bLoad; };

	bool				GetLoadDataSuccess() { return m_bLoadDataSuccess; };
	void				SetLoadDataSuccess(bool bLoad) { m_bLoadDataSuccess = bLoad; };

	bool				IsChangeTitle() {return m_bChangeTitle;};
	void				SetChangeTitleFlag(bool bChange) { m_bChangeTitle = bChange;};

	
	void				HeartBeat(void);

	virtual void		SendGameEvent(GameLogic::LC_GameEvent* pkEvent, bool bSendName = false);

	void				SetIsSyncTutorialInfo(bool bFlag) { m_bIsSyncTutorialInfo = bFlag;};
	bool				GetIsSyncTutorialInfo() { return m_bIsSyncTutorialInfo;};

	void				SetIsSyncShortcutKeyInfo(bool bFlag) { m_bIsSyncShortcutKeyInfo = bFlag;};
	bool				GetIsSyncShortcutKeyInfo() { return m_bIsSyncShortcutKeyInfo;};

	virtual int GetPackageSize();
	virtual LC_PackAsset*			GetPackAsset();

	//void				GetTaskKillNPCItem(GameLogic::LC_GameEvent* pkEvent);
	virtual LC_GameStoryAsset&				GetGameStoryAsset();
	virtual	LC_TaskMap*				GetTaskMap();
	//����ÿ������
	void ResetEveryDayTask();

	virtual bool CanFinish(int taskID);
	bool IsInProcessingTaskMap(int taskID);
	bool IsInAssignTaskMap(int taskID);
	virtual bool IsInFinishedTaskMap(int taskID);
	virtual bool IsInFinishAchievementMap(int achievementID);
	virtual ResultType CheckAchievementCanFinish(int achievementID);
	void RefreshAchievementSystem();

	void RefreshAchievement_ControllerEquip();
	void RefreshAchievement_SubInfo();
	void RefreshAchievement_BuddyInfo();
	void RefreshAchievement_RuneInfo();
	void RefreshAchievement_BuddyEquipSlotInfo();

	bool IsInFinishBiographyMap(int biographyID);
	bool IsInFinishBiographyRewardPartMap(int biographyPartID);
	bool IsInFinishBiographyPartMap(int biographyPartID);
	bool CheckBiographyFinishCondition(int biographyID);

	void				HandleProcessTaskForEnter();
	void				HandleProcessTaskForQuit();
	void				ShareMultiuserTaskToGroup(int lTaskID);
	void				ShareNormalTaskToGroup(int lTaskID);
	void				NoticeGroupJoinInTask(int lTaskID);
	void				ClearAssignTask(uint32_t uiAssignType);

	ResultType	CancleTaskOP(int lTaskID, int cancelType=0);
	void				AcceptTaskOP(int lTaskID, bool bManual = true);
	ResultType	FinishTaskOP(int lTaskID, int iBonusMulti = 10000, bool bGoldenFinger = false);
	//�������
	virtual ResultType	FinishTask(int lTaskID, int iBonusMulti, bool bGoldenFinger = false);

	ResultType CheckCanAccpetTask(LC_Task* pkTask);

	virtual int32_t GetMainTaskFinishCount();
	virtual void UpdateTaskReachRecord(int lTaskID);
	virtual ResultType	MainTaskReachRewardOP(int lTaskReachID);


	virtual bool IsInMainTaskReachRewardRecord(int lTaskReachID);
	virtual void UpdateMainTaskRewardRecord(int lTaskReachID);
	virtual void SyncMainTaskReachRecord();

	void CheckTaskChapterReward();
	bool IsInTaskChapterReward(int chapterID);
	virtual ResultType		TaskChapterRewardOP(int chapterID);

	CTaskExtraReward& GetTaskExtraRewardAsset();
	virtual void AddTaskExtraReward(LC_ItemsRewardList& itemList);
	
	BuddySchemeMgr& GetBuddySchemeMgr();

	CRotaryTable& GetRotaryTableAsset();
	virtual void UpdateRotaryTableVaildTimes(int32_t value);

	CNoviceTutorial& GetNoviceTutorialAsset();
	CPlayerMapInfo& GetPlayerMapInfoAsset();
	CPlatformInfo& GetPlayerPlatformInfoAsset();
	CIDAsset& GetIDAsset();
	CNonpareilReturn& GetNonpareilAsset();
	CQuickRaidAsset& GetQuickRaidAsset();
	CGuessAsset_Summit& GetSummitGuessAsset();
	CDropTimes& GetDropTimesAsset();
	CYuanZhengPoint& GetYuanZhengPointAsset();
	CDropTimes& GetStarShipTimesAsset();
	CRecoverTimes* GetPKRecoverAssetByType(int32_t type);
	CChargeProfit& GetChargeProfitAsset();
	int32_t GetCurrentRecoverTimes(int32_t type);
	void ReduceCurrentRecoverTimes(int32_t type);

	virtual void		AddTaskHonorPoint(int taskHonorPoint);
	virtual int32_t	GetTaskHonorPoint();

	virtual void				ModifyTaskRankInfo(int rankIndex);
	virtual void				SyncTaskRankInfo(int index=-1);

	//ȡ������
	virtual ResultType	CancelTask(int lTaskID);
	virtual bool RemoveAssignTaskByTaskID(int taskID);
	virtual bool RemoveFinishTaskByTaskID(int taskID);

	//������Բ�ͬϵͳ����������ж� [taskSystem] SpecialTask���id�ֶ�
	virtual bool CheckAcceptTaskCondition(int taskSystem, int taskID);

	virtual void RemoveTaskByTaskIDVec(mem::set<int32_t>& taskIDs);
	virtual bool AssignTaskByTaskIDVec(mem::set<int32_t>& taskIDs);

	void				Offline();
	void				Online();
	bool				IsOffline();
	float				GetOfflineTime();
	void				ClientLoading();
	void				ClientLoadFinished();
	bool				IsClientLoading();
	float				GetLoadingStartTime();
	bool				IsNormalQuit();
	void				SetNormalQuit(bool quit);
	void				RefreshDataFlag();
	bool				IsRegisterTask() {return m_bHandleTask;};
	void				SetRegisterTaskFlag(bool bHandle) {m_bHandleTask = bHandle;};

	//���ؽӿ�
	//@add by yt
	void				SetGameStoryInfo(int iGameStoryIndex, int iRecordCount);
	void				UpdateGameStoryContent(float fCurrentTime, float fDeltaTime);
	void				UpdateOpenBag(float fCurrentTime, float fDeltaTime);


	//���ؽӿ�END

	void				CancelCurrentSkill();

	//������ؽӿ�
	bool				CheckTranscriptionValidity();

	void				SetEnterTeamPlatformTimes(int32_t iPlayerEnterTeamPlatformTimes) { m_iPlayerEnterTeamPlatformTimes = iPlayerEnterTeamPlatformTimes;};
	int32_t				GetEnterTeamPlatformTimes() { return m_iPlayerEnterTeamPlatformTimes;};

	//������ؽӿ� end

	//��˽ӿ�
	virtual void		SetRiderCharType(int lCharType);
	virtual int			GetRiderCharType();
	virtual void				SummonVehicle();
	virtual void				DismissVehicle(bool upgrade=false, int PreLevel=0);

	void				GoRider();

	virtual int GetBornCharType();
	virtual void SetBornCharType( int nCharType );

	virtual void		HandlePackOPRecord(GameLogic::PackOPRecordSet* pkPackOpRecords, int32_t iReasonType, uint64_t iReasonDetail);
	//װ����ȡ�ӿ�
	ResultType			DoCommonItemOperation(GameLogic::LC_BackPackEntry* pkPackEntry, GameLogic::LC_ItemOperationDataEntry* pkOperation, bool bLock = false, int32_t iCustomProb = 0, GameLogic::PackOperateInfoList* pkCustomOP = NULL, int32_t iPay = 0);

	int					GetCurrentEquipItem(int iEquipType, bool bConsiderFashion = false);
	ResultType			EnhanceItem(int iPackType, int iEntryIndex , std::vector<Protocol::EnhanceItemInfo>& infos);

	virtual void GetMainControllerAllAttributeMap(LC_ActorAttributeMap* map);
	
	
	void	SyncControllerSkillStatesToClient(int iStateCategory);
	virtual LC_ActorAttributeMap* GetAttributeMap();
	virtual LC_ActorAttributeMap* GetFinalAttributeMap();
	
	//�鿴targetPlayerID������
	void	GetPlayerDetailInfo(uint64_t targetPlayerID, uint32_t systemType);
	//�����Ҳ鿴�����е�����
	void	FillPlayerDetailInfo(PS_User_Info& userData);
	//��ü����buddy��װ���ͱ�������
	void	FillPlayerRuneAndEquipInfoMap(std::map<int, PS_Buddy_Info_BuddyCS>& info);
	void	FillPlayerRuneAndEquipInfoVector(std::vector<PS_Buddy_Info_BuddyCS>& data);
	
	
	// ĳ�ŵ�ͼ ����/���� ���ж�����Ϣ
	void	FillSingleShowInfoByMapId(SingleShowInfo & info,int mapId,uint32_t schemeType );
	// ĳ�ŵ�ͼ ����/���� ����������Ϣ
	void	FillPS_SchemeBuddyDataByMapId(PS_SchemeBuddyData & info,int mapId,uint32_t schemeType,int teamId);
	
	

	
	//�������������������
	void GetOtherOfflineInfo(OfflinePlayerFromDBType eType,uint64_t uCharId);
	void FillUserInfo(User_Pk_Show_Info &UserInfo);
	void NotifyPkInfo(User_Pk_Show_Info &m_nUserInfo);

	//common op

	ResultType			CheckExternBasicBag(int slot, GameLogic::LC_ShopIDMap& shop_info);
	ResultType			ExternBasicBag(int slot);

	ResultType			DecompositItem(std::vector<Protocol::DecompositItemInfo>& items, std::vector<Protocol::DecompositItemResult>& resitems);

	ResultType			ExternWarehouse(int32_t rowNum);

	void						BroadcastChangeName();

	virtual bool			UpdateGuildMemberInfo(bool sendToClient, bool force);

	//buddy������ȡ
	void EquipSlotStar(int32_t buddyId, int32_t equipIndex);
	ResultType DoEquipSlotStar(int32_t buddyId, int32_t equipIndex, int32_t& newStarLevel);
	//buddy���µ��
	void			ReinforceEquipSlot(int reinforceType, int buddyID, int equipSlotIndex);
	ResultType DoReinforceEquipSlot(int buddyID, int equipSlotIndex, int32_t& newLevel);
	ResultType DoOneKeyReinforceEquipSlot(int buddyID, int equipSlotIndex, int32_t& newLevel);

	ResultType			DecompositionEquip(const std::vector<int32_t>& EquipIndexs, std::vector<PS_ItemInfo>& DecompositionResult, bool HandlePackOP=true, bool eMail=false);
	ResultType			CheckDecompositionEquip(const std::vector<int32_t>& EquipIndexs, Utility::LC_ComMap& consumeCashMap);
	ResultType			ExecuteDecompositionEquip(const std::vector<int32_t>& EquipIndexs, const Utility::LC_ComMap& consumeCashMap, std::vector<PS_ItemInfo>& DecompositionResult, bool HandlePackOP=true, bool eMail=false);


	void						GetRuneConsume(const LC_BackPackEntry* pkRuneData, Utility::UT_SIMDataList& consumeItem, Utility::LC_ComMap& moneyAfter);
	ResultType			RestituteRuneConsume(LC_ServerPlayer* pkPlayer, LC_BackPackEntry* pkRuneData, Utility::UT_SIMDataList& consumeItem, const Utility::LC_ComMap& moneyAfter);

	void			RecommendFriend();
	//GM��̨���
	void				SetFollowedByGM(bool bFollwed) { m_bFollowedByGM = bFollwed;};
	bool				GetFollowedByGM() { return m_bFollowedByGM;};
  
	//������Ҹ����͵�ͼ�����Ϣ
	GameLogic::LC_WorldLocationAsset*	GetWorldLocationAsset() { return &m_kWorldLocationAsset;};

	void				SetArenaData(Protocol::PS_CharArenaInfo& arenaData);
	inline void			ResetAsControler(void)
	{
		GetAsAControler().Reset();
		SyncAsControlerInfo();
		SetOutSector(false);
		SyncLocationInfo();
		SyncPropertyToClient();
		SyncFightStateToClient();
	}

	// ������������
	const Protocol::PS_ClientAuctionSell& GetAuctionSellData() const { return m_kAuctionSellData; }
	void				SetAuctionSellData(const Protocol::PS_ClientAuctionSell& kAuctionSellData);
	void				SetAuctionBuyID(const unique_id_impl& uiAuctionBuyID) { m_uiAuctionBuyID = uiAuctionBuyID; }
	const unique_id_impl& GetAuctionBuyID() const { return m_uiAuctionBuyID; }
	void				SetAuctionBuyPrice(int32_t iCashType, uint64_t uiAuctionBuyPrice) { m_kAuctionBuyPrice[iCashType] = uiAuctionBuyPrice; }
	int64_t				GetAuctionBuyPrice(int32_t iCashType) { return m_kAuctionBuyPrice[iCashType]; }

	ResultType AddDropByDisassemble( int nDisassemId, bool bCached = false, int nCount = 1 );

	void				SetShapeShiftInfo(int lShapeShiftTypeID);
	void				AddShapeShiftForbidAction(int lShapeShiftTypeID);
	void				RemoveShapeShiftForbidAction(int lShapeShiftTypeID);

	void				CostItemEndurance(bool bDead, int lSkillID, bool bSrc);

	virtual void		AddFriendShip(object_id_type lCharID, int iFriendShip);
	//����
	void				SyncPropellInfo(PropellInfoList& propellInfos);
	uint64_t			PropellID(void) {return m_uiPropellID;}
	void				SetPropellID(uint64_t uiPropellID) {m_uiPropellID = uiPropellID;}

	//promote
	ResultType			HandlePromoteReward(int32_t iType, int iLevel = 0);

	//AI����ʱ��ͬ������λ��
	void				SetAIControlNotify(bool flag) {m_AIControlNotify = flag;};
	bool				GetAIControlNotify() {return m_AIControlNotify;};

	void				AddAttributePlus(int32_t iAttributeType, int32_t iVal, int32_t iPercent);
private:
	bool m_bLotteryStat;
	LC_CombatEventData m_CombatEventData;
public:
	bool GetLotteryStat();
	void SetLotteryStat( bool bFlag );
	uint32_t GetLotteryTotalCount();
	bool CheckInitGame();
	void SetInitGame(bool bState);
	void SetAllInitGame(bool bState);
	void ACTEnd(uint32_t iSchedule);
	void LoginCheckACTEnd();
	ResultType ClearScheduleData(uint32_t ActType, std::map<uint32_t, GameLogic::LC_IDSet>& rkIndices);
private:
	void RecoverBuddyListData();
	void PostIncLotteryCounter();
protected:
	void TickOwnerMP( float fCurrent );
	void SyncOwnerMP_Impl(int32_t nCur, int32_t nMax);
	void ModifyOwnerMP_Impl( int32_t nDeltaValue );
	float m_fOwnerMPTimer;
public:
	virtual int32_t GetOwnerMaxMP();
	virtual void SetOwnerMaxMP( int32_t nValue );
	virtual void ModifyOwnerMaxMP( int32_t nDeltaValue );
	virtual int32_t GetOwnerMP();
	virtual void SetOwnerMP( int32_t nValue );
	virtual void ModifyOwnerMP( int32_t nDeltaValue );
	void SyncOwnerMP();
	void ModifyOwnerMPLite( int32_t nDeltaValue );
	//���ø��෽������������Ϣ���±�־
	virtual void		SetMoveSpeed(int fMoveSpeed);
	virtual void		AddHP(attr_value_type  lHP, bool isRecoverHP=false);
	virtual void		AddMP(attr_value_type  lMP);
	virtual void		SetHP(attr_value_type  lHP);
	virtual attr_value_type GetHP();
	virtual void		SetMP(attr_value_type  lMP);
	virtual attr_value_type GetMP();
	virtual void		SetActivetyBuddyFactionID(unsigned short usFactionID);
	virtual void		SetFactionID(unsigned short usFactionID);
	virtual unsigned short				GetFactionID();
	virtual void SetOldHP(attr_value_type lOldHP);
	virtual attr_value_type GetOldHP();

	virtual LC_ActorStateMap* GetStateMapOnBuddy();

	virtual LC_ActorStateMap* GetControllerStateMap();

	virtual int	GetCurrentLocationWithZ( Utility::UT_Vec3Int& location );
	virtual void		SetCurrentLocation(const Utility::UT_Vec3Int& rkLocation);
	virtual void		SetCurrentLocation(int iX, int iY, int iZ);
	virtual bool		SetTargetLocation(const Utility::UT_Vec3Int& rkTargetLocation);
	virtual void		SetForwardDir(const Utility::UT_Vec2Float& rkDir);
	virtual void		SetForwardDirAngle(float fDirAngle);
	virtual void		RotateByUp(float fDeltaAngle);

	virtual void SetSelectTargetID(object_id_type lID);
	virtual object_id_type GetSelectTargetID();

	virtual	void		SetCurrentStateType(int iCurrentState);

	virtual ResultType	EquipItemInPackage(int iPackType, int iEntryIndex, int32_t iEquipPackType, int& iEquipSlotIndex);
	virtual void		SetCharTitle(const StringType& rkCharTitle);
	virtual void		SetGuildID(uint64_t lGuildID);
	virtual void		SetGuildName(const StringType& strGuildName);
	virtual void		SetSkillFlag(int64_t llFlag, bool bFlag);
	virtual void		SetAllSkillFlag(int64_t llSkillFlag);
	virtual void		SetPlaceCanPK(bool pk);
	virtual void		SetBroadCastFlag(int nFlag);
	void		SetBroadCastFlagForce(int nFlag);

	int ActivateSkill(int32_t nBuddyId, int32_t nSkillId, int32_t& nErrorCode);
	// 1: activated
	// 0: deactivated
	// -1: no such buddy or skill
	int CheckActivateSkill(int32_t nBuddyId, int32_t nSkillId);

	// skill usable count
	virtual int ModifySkillUsableCount( int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount, bool bSelfOwner = false );
	virtual int GetSkillUsableCount( int32_t nBuddyId, int32_t nSkillId, bool bSelfOwner = false );
	virtual int ModifySkillMaxUsableCount( int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount );
	virtual int GetSkillMaxUsableCount( int32_t nBuddyId, int32_t nSkillId, bool bSelfOwner = false );
	virtual int GetSkillEnableUsableCount( int32_t nBuddyId, int32_t nSkillId, bool bSelfOwner = false );
	// Self
	virtual int ModifySkillUsableCount_Self( int32_t nSkillId, int32_t nUsableCount );
	virtual int GetSkillUsableCount_Self( int32_t nSkillId );
	virtual int ModifySkillMaxUsableCount_Self( int32_t nSkillId, int32_t nUsableCount );
	virtual int GetSkillMaxUsableCount_Self( int32_t nSkillId );
	// for single cd sync 
	virtual int SyncSingleSkillCDInfo(int32_t nBuddyId, int32_t nSkillId, float fCDTime);

	virtual int UpdateSkillAsset(float fCurrentTime, float fDeltaTime);

	virtual uint64_t								GetGuildID();
	virtual const StringType&				GetGuildName();
	virtual const StringType&				GetGuildName_utf8();
	virtual bool							IsExistGuild();

	virtual LC_VIPAsset& GetVIPAsset();
	virtual LC_AssistAsset& GetAssistAsset();
	uint32_t GetAddRateToVip();
	virtual int						GetServerID();
	virtual void						SetServerID(int nID);

	virtual bool IsCashEnough(MoneyType llCash, int iCashType);

	virtual int GetFriendsNum();
	virtual int GetJoinTimes();

	//���
	virtual LC_PlayerGroupBase*		GetPlayerGroup();
	virtual void		SetPlayerGroupIDType(uint32_t nGroupID, uint32_t nGroupType);
	virtual void		SetPlayerGroupGlobalInfo(uint32_t mapTypeId, uint32_t nGlobalTeam);
	virtual uint32_t GetPlayerGroupID();
	virtual uint32_t GetPlayerGroupType();
	//�ӳ���־
	virtual void SetPlayerGroupCheifFlag(bool bFlag);
	virtual bool GetPlayerGroupCheifFlag();
	//�������
	virtual int GetBeInviteGroupIDByIndex(int iIndex);
	virtual void SetBeInviteGroupIDByIndex(int iIndex, int lGroupID);
	virtual void RemoveBeInviteGroupByID(int lGroupID);
	virtual bool CheckGroupDistance(int lDistance);
	
	virtual void		ClearPlayerGroup(uint32_t nMapOpt, uint32_t nGroupOpt);					//����Ҵӵ�ǰ������
	virtual void		setExtPropertyDirty();
	MC_DataDirtyFlag&	GetMessageDirtyFlag();
	void	FlushMessageDirtyFlag();

	ServerViewState		GetServerViewState();
	void				SetServerViewState(ServerViewState state, bool bAsBuddy = false);

	//�����������
	bool				CheckSyncMoveValid(const Utility::UT_Vec3Int& rkSyncLocation);
	void				CheckTimePassValid(float fClientCurrentTime, float fServerCurrentTime, float fTolerateTime);
	Utility::UT_Vec3Int	GetLastMoveCheckLocation() { return m_kLastMoveCheckLocation;};
	void				SetLastMoveCheckLocation(const Utility::UT_Vec3Int& rkLocation) { m_kLastMoveCheckLocation = rkLocation;};
	void				SetLastServerSetLocationTime(float fTime) { m_fLastServerSetLocationTime = fTime;};
	void				RecountTime();

	inline void			SetLastPickItemTime(float fTime) { m_fLastPickItemTime = fTime;}
	inline float		GetLastPickItemTime(void) { return m_fLastPickItemTime;}

	virtual int OnHatredListAdd(object_id_type id, int64_t hatred, float time);
	virtual int OnHatredListRemove(object_id_type id);

	//�ر�SK_FLY
	void				SetFlyCountDownTime(float fTime);
	void				UpdateFlyCountDown(float fCurrentTime, float fDeltaTime);
	//�Ṧ�޵м���cd
	void				SetJumpImmortalEndTime(int jumpStage, float fTime);
	virtual bool		IsJumpImmortal(void);

	virtual ResultType	EquipVehicleEquipment(int iPackType, int iEntryIndex, int iVSlotIndex);
	virtual ResultType	UnEquipVehicleEquipment(int iVSlotIndex);

	void				ReduceTranscriptionEnterTimes(int iContentValue);		//�۳������������
public:
	// 360 ��ֵ�߼����
	int32_t FetchSinglePay( const std::string& strOrderId );
	bool RemovePlayerOrder(std::string& strRedisPrefixPlayerAdd,const std::string& strOrderId);
	bool ModifyOrderStatusAnd(std::string& strRedisPrefixOrderId,const std::string& strOrderId);
	int32_t FetchAllPay();
private:
	int32_t FetchSinglePayImpl(const std::string& strOrderId);
	int32_t FetchAllPayImpl();
public:
	struct itemInfo
	{
		int32_t m_itemId;
		int32_t m_itemNum;
	};

	//�����ҵ�buddy������ȡ�����Ƿ���
	virtual bool	GetBuddyEquipSlotStarState();
	//������ҵ�buddy������ȡ���ܿ���
	virtual bool	SetBuddyEquipSlotStarState();

	virtual int32_t	GetYiZhongQinXiEndTime();
	virtual void	UpdateYiZhongQinXiEndTime(int32_t endTime);
	virtual bool	GetYiZhongQinXiState();
	virtual void	SetYiZhongQinXiState(bool state);
	void SyncYiZhongQinXiState();
	void PackCommanderBaseInfo(CommanderBaseInfo &commanderBaseInfo);
	void PackSingleSideStruct(SingleSideStruct &singleSideInfo);
	void PackPS_BuddyByBuddyId(int buddyID , PS_Buddy & ps_buddyInfo);
	void PackBaseBattlefieldBuddyDataToVec(mem::vector<int>& buddyIDs,std::vector<BaseBattlefieldBuddyData> &vecResult);
	
	void		GetDamageRankInfo(int32_t mapResID);
	void		GetGuildDamageRankInfo(int32_t mapResID);
	void		UpdateDamageData(int32_t storyID, int32_t updateType, uint64_t param, bool syncClient=true);
	int32_t	UpdatePlayerDamageData(int storyID, uint64_t damageData, int32_t updateType);
	void		UpdateGuildDamageData(int storyID, uint64_t damageData);

	bool FillSettleInfo(SettleInfo& info, uint64_t damageData);

	virtual void SetCombatFlag(bool tag);
	virtual bool GetCombatFlag();
	virtual const mem::map<int32_t, LC_ActorBase::CombatRecord>* GetMonstarCombatDataMap();
	virtual LC_ActorBase::CombatRecord* GetMonstarCombatDataMapByID(int id);
	virtual LC_ActorBase::CombatRecord* GetPlayerCombatDataMapByID(int id);
	virtual void ClearMonstarCombatDataMap();
	virtual void ClearPlayerCombatDataMap();

	virtual uint64_t	GetAllRuneItemCombatScore();

	virtual int32_t GetTotalActivatedSkillCount();
	virtual int32_t GetBuddyTotalActivatedSkillCountByBuddyID(int buddyID);
private:
	int32_t m_nTotalActivatedSkillCount;

	bool		m_nYiZhongQinXiState;
	int32_t	m_nYiZhongQinXiEndTime;

	bool m_bCombatFlag;
	mem::map<int32_t, LC_ActorBase::CombatRecord> m_nMonstarCombatDataMap;
	mem::map<int32_t, LC_ActorBase::CombatRecord> m_nPlayerCombatDataMap;

	void OnNoticeActivatedSkillCount();
	void OnIncActivatedSkillCount();
public:
	//�ͻ�����ʱ

	void				BeKickedOff();
	int					GetChangeServerCDTime() { return Utility::Float2Long(m_fChangeServerCDTime);};
	void				SetChangeServerCDTime(int lCDTime) { m_fChangeServerCDTime = Utility::Long2Float(lCDTime);};
	void				UpdateChangeServerCDTime(float fCurrentTime);

	virtual void		AddSystemTitle(int iTitleID);										//���ϵͳ�ƺ�
	virtual void		RemoveSystemTitle(int iTitleID, uint32_t removetime);				//���ϵͳ�ƺ�
	virtual void		SetSystemTitle(int iTitleID);										//����ϵͳ�ƺ�
	virtual void		RemoveSystemTitleMsg(int iTitleID);									//�Ƴ�ϵͳ�ƺ���Ϣ
	void				CheckInbornTitle(void);												//�������Դ��ƺ�

public:
	bool GetSevenDayTrailFlag();
	void SetSevenDayTrailFlagInvalid();
	void CheckSevenDayTrialFinished();
private:
	bool m_nSevenDayTrailFlag;

	std::string m_nTest;//baseInfo��������
public:
	//�ɾ����
	//������ȡ�ɾͽ���
	ResultType AddAchievementReward(int32_t iAchievementID);
	//��óɾ͵㽱��
	ResultType AddAchievementPointReward(int32_t rewardID);
	//��������㽱��
	ResultType AddSevenDayPointReward(int32_t rewardID);

	//��ɳɾͣ��㲥
	virtual void PostGetAchievement(const GameLogic::LC_AchievementDataEntry_New* pkEntry);
	//���³ɾͽ���
	virtual void UpdateAchievementRecord(int32_t iAchievementtype, AchievementEvent& kEvent);

	//������ɳɾ�id
	virtual void SyncFinishAchievement(int32_t id);
	//���³ɾ���ɽ���
	virtual void SyncAchievementRecord(int32_t groupID, uint64_t value);
protected:
	void SendSevenDayReward();
	//Update�У����Ͷ�ʿ���Ƿ����	
	void CheckInvestmentsFinished();
	void SendInvestmentReward(CSVFile::CF_InvestmentPlan::DataEntry* pkData);
	
public:
	void				SetFightState(bool bPVPFightState, bool bPVEFightState);			//ս��״̬

	ResultType			DeleteItemInPackage(int iPackType, int iEntryIndex,int nLogCode = 0,const vector<int> &Params = vector<int>());					//ǿ��ɾ��ĳ��λ�õ���Ʒ
	virtual int OnUseItem(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  uint32_t type);
	ResultType			NotifyAddItem(PackOperateInfoList& kItem, mem::vector<int32_t>& slotIndex, bool isItemAdd=false);
	virtual ResultType	AddItem(int lItemID, int lItemCount, bool bAutoUsed = false, int goalID=0, int storyID=0,int nLogCode = 0,const vector<int> &Params = vector<int>());
	ResultType			TryDeleteItem(int lItemID, int lItemCount);
	ResultType			DeleteItem(int lItemID, int lItemCount,int nLogCode = 0,const vector<int> &Params = vector<int>());
	ResultType			DeleteItemByIndex(int iPackType, int nIndex, int lItemCount,int nLogCode = 0,const vector<int> &Params = vector<int>());

	void				UpdateItemLog(uint32_t lServerTime);										//��ʱ��ӡ��Ʒ����

	virtual ResultType	ConsumeItemsInPackageByIndex(int iPackType, int iEntryIndex, int iConsumeNum = 1, int nLogCode = 0, const vector<int>& nParams = vector<int>());		//��װLC_PlayerBase::ConsumeItemsInPackageByID��Ŀǰ�����ڴ�ӡ��Ʒʹ�õ�oss log
	virtual ResultType	ConsumeItemsInPackageByID(int lItemID, int iConsumeNum = 1);

	//����ս��״̬��ʱ��
	float				GetFightStateBeginTime() {return m_fFightStateBeginTime;};

	//�ۼ��ƶ�����
	void				AccumulateMoveDistance(const Utility::UT_Vec3Int& rkClientSyncLocation);
	int					GetAccumulateMoveDistance();
	void				ClearAccumulateMoveDistance();

	//�հ�Ƶ�ʿ���
	UT_GameServerPackFilter&	GetPackFilter()  {return m_kGameServerPackFilter;};

	//��鼼���Ƿ�Ϸ�
	virtual bool		CheckSkillValid(CSVFile::CF_SkillSpecialAttrib::DataEntry* pkSkillData, int lItemID);
	//�ʺ�����ʱ��
	void				SetAccountLoginTime(int lTime) { m_lAccountLoginTime = lTime;};
	int					GetAccountLoginTime() { return m_lAccountLoginTime;}

	bool				GetCourage() {return m_Courage;}
	void				SetCourage(bool Value) { m_Courage = Value;}

	void				AddCourageState();

	ResultType			Upgrad_Subordinate(int32_t iSubType, int32_t iPayIndex, std::vector<int32_t>& attrIDVec, std::vector<int32_t>& attrLevelVec);
	/*
	ResultType			Upgrad_Vehicle(int32_t iPayIndex = 0);
	ResultType			Upgrad_Bow(int32_t iPayIndex = 0);
	ResultType			Upgrad_DevaEye(int32_t iPayIndex = 0);
	ResultType			Upgrad_VehicleFighting(int32_t iPayIndex = 0);
	ResultType			Upgrad_Talisman(int32_t iPayIndex = 0);
	ResultType			Upgrad_SpiritPet(int32_t iPayIndex = 0);
	ResultType			Upgrad_SpiritWeapon(int32_t iPayIndex = 0);
	ResultType			Upgrad_HiddenWeapon(int32_t iPayIndex = 0);
	ResultType			Upgrad_Muhun(int32_t iPayIndex = 0);
	ResultType			Upgrad_Artifact(int32_t iSubType, int32_t iPayIndex = 0);
	*/
	virtual void		OnSubUpgradeSucessfully(int32_t iSubType);
	virtual void		OnSubDowngrade(int32_t iSubType);
	void				OnVehicleUpgrade(int32_t iPreLevel);

	void				SetSurvivalTime(uint32_t lTime)	{ m_lSurvivalTime = lTime;};
	uint32_t			GetSurvivalTime()			{ return m_lSurvivalTime;};
	void				SetSurvivalStartTime(float lTime)	{ m_fSurvivalStartTime = lTime;};
	float				GetSurvivalStartTime()			{ return m_fSurvivalStartTime;};
	void				SetSpawnTime(float lTime)	{ m_fSpawnTime = lTime;};
	float				GetSpawnTime()			{ return m_fSpawnTime;};

	float				GetLastPositiveActiveTime() { return m_fLastPositiveActiveTime;};
	void				SetLastPositiveActiveTime(float fTime) { m_fLastPositiveActiveTime = fTime;}

	float				GetLastExecuteSkillTime() { return m_fLastExecuteSkillTime;};
	void				SetLastExecuteSkillTime(float fTime);

	//�������ʱ��
	float			 	GetLastIdleTime() { return m_fLastIdleTime;};
	void			 	SetLastIdleTime(float fTime) { m_fLastIdleTime = fTime;};

	uint32_t		GetLastOfflineTime();
	void			 	SetLastOfflineTime(uint32_t timestamp);

	virtual LC_ActorFOVMapBase*				GetFOVMap();

	virtual void SetAIControl(bool iscontrol);
	virtual bool GetAIControl(void);

	void RefreshCurrentOPIndex( int32_t nIndex, bool bWithAI = true );

	void GenSkillRandList();

	void                SetSummonFinishHandle(on_summonfinish_handler finishhandle) { m_SummonFinishHandle = finishhandle;};

	uint64_t		GetRankValueByRankType(int32_t rankType);
	virtual void	UpdateRankUserValueByRankType(int32_t rankType);
	virtual void	UpdatePlayerBaseDataToGroup();

	ResultType			TryRealizeGreatKungFu(uint32_t uiID);

	virtual ResultType	OpenAbility(int32_t id, int32_t iType, int32_t iKey, bool send=true, bool force=false);
	virtual ResultType	CloseAbility(int32_t iType, int32_t iKey);

	ResultType OpenPetAbility(int32_t abilityID, int32_t petID=0, bool force=false);

	void				BuyVIPTimes(int32_t storyId, int32_t buyType, uint32_t times=1);
	void				GetBuyVipInfo(int32_t storyId, uint32_t buyType, uint32_t& buyTimes, uint32_t& totalTimes, uint32_t& costCount, uint32_t& costType);
	
	//�ʻ��߼�
	ResultType			SendFlowerToPlayer(const unique_id_impl& charID, const StringType& sTargetName, int32_t nOptId);
	int32_t				GetDailyCleanValue(uint32_t nType);
	void				SetDailyCleanValue(uint32_t nType, int32_t nValue, bool bModify = true);
	void				ClearDailyClean(uint32_t timestamp);
	uint64_t			GetRewadGainValue(uint32_t nType);
	void				SetRewadGainValue(uint32_t nType, uint64_t nValue, bool bModify = true);
	mem::map<uint32_t, int32_t>&		GetDailyCleanMap() { return m_dailyCleanMap; }
	mem::map<uint32_t, uint64_t>&	GetRewadGainMap() { return m_rewardGainMap; }
	mem::vector<uint32_t>&	GetFlowerSendCntList() { return m_flowerSendCnt;}
	void				AddSendFlowerLog(const unique_id_impl& charID, const StringType& sTargetName, int nKeyId);

	virtual void		OnPKValueChange(int nValue);
	int32_t				GainOffLineExp(int32_t nGainType);
	uint32_t			GetOffLineExp();
	int32_t				GetOfflineExpTime();
	void				SetOffLineExp(uint32_t nValue, uint32_t nTime);
	void				AddOffLineExp(uint32_t nValue, int nTime);
	void				calculateOfflineTimeExp();

	void				SetMysticalShopResetCnt(uint32_t nType, uint32_t nCount);
	uint32_t			GetMysticalShopResetCnt(uint32_t nType);
	void				SetMysticalShopBuyCnt(uint32_t nType, uint32_t nCount);
	uint32_t			GetMysticalShopBuyCnt(uint32_t nType);
	inline mem::vector<uint32_t>& GetMysticalShopItemList() { return m_MySyicalShopMark.m_itemidlist; }
	inline void			SetMysticalShopTimeStamp(uint32_t nTimeStamp) { m_MySyicalShopMark.m_nTimeStamp = nTimeStamp; }
	inline uint32_t		GetMysticalShopTimeStamp() { return m_MySyicalShopMark.m_nTimeStamp; }
	inline MySyicalShopMark& GetMySyicalShopMark() { return m_MySyicalShopMark;}
	void				sendMysticalShopInfo();

	inline const platform_vip_info& GetPlatformVipInfo(void) { return m_platformvipinfo;}
	bool				SetPlatformVipInfo(const std::string& info);
	void				SetPlatformVipInfoTest(platform_vip_info& info);

	void				PullGMInfo(void);
	virtual uint16_t	GetPlayerPlatformType();
	virtual uint16_t	GetPlayerLoginPlatformPage();

	//�һ��̵������������
	uint32_t			CheckCanBuyItemWithVipLimit(uint32_t nCount, uint32_t nID, uint32_t nTypeClean = GameLogic::E_DailyClean_Common, uint32_t nTypeVip = GameLogic::LC_VIP_PROF_TP_STORY);
	void				AddBuyItemWithVipLimit(uint32_t nCount, uint32_t nID, uint32_t nTypeClean = GameLogic::E_DailyClean_Common, uint32_t nTypeVip = GameLogic::LC_VIP_PROF_TP_STORY);

	virtual uint32_t	CheckPayForShop_Cash(int64_t nCash, uint32_t nCashType, uint32_t iSrcType, uint64_t iSrcID);
	virtual uint32_t	CheckPayForShop_Score(int32_t nCash, uint32_t nCashType, uint32_t iSrcType, uint64_t iSrcID);
	virtual void		OnPayForShop_Score(int32_t nCash, uint32_t nCashType, uint32_t iSrcType, uint64_t iSrcID);

	virtual void		OnSubordinateConsume(int32_t iSubType, int iPayType, int32_t iValue);

	virtual ResultType AddSubTransform(int32_t iSubType, int32_t iTransfrom, bool useItem);
	virtual LC_SubordinateAsset&	GetSubordinateAsset(void);

	bool				DealSelfGMRequest(int gmType, const StringType& strInfo);
	static void			SendNoticeOnPlayerAttack(uint32_t nAttackType, LC_ServerMapLogic_Place* pLogicPlace, LC_ServerPlayer* pSrcPlayer, LC_ServerPlayer* pDstPlayer, int32_t nSrcSId, int32_t nDstSId);
	void				SendNoticeEnterGame(uint32_t nEnterResult = RE_SUCCESS, bool bSwitchLine = false);

	void				SetLCLockedID(object_id_type sID);
	object_id_type		GetLCLockedID()	{ return m_lastLcLockedID; }

	int32_t GetGameStoryEnterTimes(int storyID);
	int32_t GetGameStoryFinishTimes(int storyID);

	LC_ResourceRecord&	GetOssResourceCache(int32_t iTypeID, int32_t iSrcType);
	void				AddOssResourceCache(int32_t iTypeID, int32_t iSrcType, uint64_t iCount);
	void				ResetOssResourceCache(int32_t iTypeID, int32_t iSrcType);

	void NotifyEndlessTowerGameEvent(LC_GameEvent* pkEvent);

	Activity::ACT_SecretTreasureAsset&	GetSecreteTreasureAsset(void) {return m_kSecretTreasureAsset;}
	Activity::ACT_ScheduleAsset&		GetACTScheduleAsset(void) {return m_kACTScheduleAsset;}

	void				DoLimitReward(int activityType, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, int count);
	void				AddRechargeRecord(int64_t iVal);
	void				SettleRechargeReturn(int32_t iPlatform);
	void				DailyRechargeReturnSettle();

	void				AddResource(int32_t iTypeID, int32_t iSrcType, uint64_t iCount) { m_kResourceAlert.AddResource(iTypeID, iSrcType, iCount); }

	LC_FacebookRecord&	GetFacebookRecord() { return m_kFacebookRecord; }

	bool				CheckAllianceDirty();
	void				SyncAllianceUnitInfo();

	inline	uint32_t	_GetDailyCleanKeyID(uint32_t nType, uint32_t nID) { return nType * 1000 + nID; }

	void				AddBoneSoul(int skillID);
	int					GetBoneSoul(int iIndex);
	void				RemoveBoneSoul(int skillID);

	void RefreshSMController();

public:
	bool			GetSaveFlag(){return m_bSaveFlag;}
	void			SetSaveFlag(bool bSaveFlag = true){m_bSaveFlag = bSaveFlag;}
	void			ReceiveDailyReward_RongGuangYuanZheng();

	virtual void	UpdateDailySystem(uint32_t currentTime);

	ResultType			TreasureReward(int nType,mem::vector<int>& vecReward);
	void				AddTreasureRewardNum(int nType,int num);
	void				SyncTreasureNum();
protected:
	void				_initSMController(const StringType& rkScriptName);
	void				_initStateMap(const StringType& rkScriptName, bool bForce = false);
	void				_uninitStateMap();
	StateMachine::SM_StateBase*	_findState(int iStateType);	//���
	void				_Learn_openAblitySkill(int SkillID);

	void				_updateInvitePlayerGroupList();
	void				_updatePairSitRecover(float fCurrentTime);
	void				_updateMuhunAvatar(int iMuhunLevel);
	void				_clearMuhunAvatar(void);

	void				_initSubordinate(void);

	void				_removeTimeLimitTask();

	void				_sendEventToSelf(GameLogic::LC_GameEvent* pkEvent, bool bSendName);
	void				_sendEventToGroup(GameLogic::LC_GameEvent* pkEvent, bool bSendName);
	void				_sendEventToArea(GameLogic::LC_GameEvent* pkEvent, bool bSendName);
	void				_sendEventToWorld(GameLogic::LC_GameEvent* pkEvent, bool bSendName);

	void				_SyncEventRelatedData(GameLogic::LC_GameEvent* pkEvent);

	void				_updatePlayerRecover(float fCurrentTime);			//���ָ�����ֵ

	void				_updateNewVIPReward( float fCurrent, float fDelta );

	void				_updateAntiWallow(float fCurrentTime);
	void				_updateDaily();
	void				_updateComboCount(int iOldValue, int iAddCount);
	void				_updateComboEffect(int iOldValue, int iAddCount);
	void				_updateComboEffectCount(int iCount);
	void				_endComboEffectRound();
	void				_clearComboCount();

	LC_ResourceAlert& GetResourceAlert();

	void				DailyReceiveReward(bool refreshFlag = false);

	virtual ResultType	_operationSuitAttribute(int iFuncID, int iSuitNum, mem::set<int32_t>* pkHappenedKeys, uint64_t* pnScore = NULL);
	virtual ResultType	_unoperationSuitAttribute(int iFuncID, int iSuitNum, mem::set<int32_t>* pkHappenedKeys, uint64_t* pnScore = NULL);

	int					_placeHolderMsg(MG_SyncPlaceHoldersInFOV& syncMsg, const CSObjectIdSet& rkIDList, float fCurrentTime, int iCount);

	NW_ServerSession*	m_pkSession;
	uint64_t            m_nSessionId;
	uint32_t			m_updateGuildStamp;
	float				m_fLastRecover;		//�ָ�����ʱ���
	int32_t				m_DailyReward;	// ÿ���콱���λ
	
	mem::vector<sTreasureNumStruct> m_vecTreasureNum;

	//��Ʒ�������� ����
	bool				m_bPendingItemSkill;
	int					m_lPendingItemSkillTypeID;
	int					m_lPendingItemTypeID;

	user_id_type		m_dwUserID;
	int					m_lSpawnMapID;

	bool				m_bLoadDataFromLoginFlag;

	bool				m_bLoadDataFromDB;
	bool				m_bLoadDataSuccess;
	float				m_fLastUnidonUpdateTime;

	Utility::UT_Timer	m_DBUpdateTimer;
	Utility::UT_Timer	m_UpdateItemTimer;
	Utility::UT_Timer	m_kLogTimer;
	Utility::UT_Timer	m_kAttributeTimer;
	Utility::UT_Timer	m_kUpdateLogicAreaInfo;
	Utility::UT_Timer	m_JumpDodgeTimer;
	Utility::UT_Timer	m_MPRecoverTimer;

	Utility::UT_Timer	m_kUpdateCashOPTimer;

	mem::vector<Utility::UT_Timer> m_kUpdateAssetTimers;

	bool				m_bChangeTitle;
	BYTE				m_byBusyState;
	unsigned int		m_iHeartBeatCount;

	float				m_fLastOSSPosTime;

	int					m_lCharLevel;

	bool				m_bIsSyncTutorialInfo;							//�����ʲ�ͬ��Flag,ֻ�е�½��ͬ��һ��
	bool				m_bIsSyncShortcutKeyInfo;						//��ݼ��ʲ�ͬ��Flag,ֻ�е�½��ͬ��һ��
	uint32_t			m_DailyTimeStamp;								//ÿ��ˢ��ʱ���
	uint32_t			m_UpdateUserInfoTimeStamp;						//pkGuildManagerˢ��ʱ���
	bool				m_updateToGroupFlag;							// ��Ҫˢ�±��λ
	uint32_t			m_VigorStamp;									//����ˢ��ʱ���
	//�Ƿ����ߣ�����ʱ��
	bool				m_isOffline;
	float				m_offlingTime;
	bool				m_clientLoading;
	float				m_loadTime;
	bool				m_normalQuit;
	bool				m_bIsRelogin;
	bool				m_bHandleTask;

	uint32_t			m_uiHeadPic;
	uint8_t				m_byDeleteFlag;		//ֵΪ0��ʾ��ɫ�����������ʾ��ǰ��ɫ�ѱ�ɾ��
	uint32_t			m_iDeleteTime;
	
	bool m_bSendGuildPresentInfoFlag;//Ĭ��false����һ�ε���SendGuildPresentInfo������Ϊtrue��������
	bool m_nShowGuildPresentName;//Ĭ��true
	bool m_bSaveFlag;//Ĭ��false
	int32_t m_nTreasureMapID;
	UT_Vec3Int m_nTreasurePos;

	uint32_t			m_iLastOfflineTime;

	int					m_UpgradVehicleCount;

	int					m_UpgradBowCount;

	MC_DataDirtyFlag	m_dataChangeFlag;

	//����ϴι�����Ŀ��
	int					m_lastLcLockedID;

	int32_t						m_iPlayerEnterTeamPlatformTimes;
	mem::map<uint32_t, int32_t>		m_dailyCleanMap;
	uint32_t					m_dailyCleanStramp;
	mem::vector<uint32_t>			m_flowerSendCnt;
	mem::map<uint32_t, uint64_t>		m_rewardGainMap;

public:
	ResultType SetRankTransferFailed(int mapResID);
	int32_t GetSkyArenaGroupNum();
	int32_t GetSkyArenaMaxStage();
	ResultType SetPlayStateEnd(int mapResID);
	//��Ҫ���ƿͻ���Э��Ƶ���б�
public:
	float				GetLastProfileTime() const { return m_fLastProfileTime; }
	void				SetLastProfileTime(float fLastProfileTime) { m_fLastProfileTime = fLastProfileTime; }
	void                setUserUUID(uint64_t uuid) { m_userKey = uuid; }
	uint64_t            getUserUUID() { return m_userKey; }

public:
	virtual void EnableBotManager(uint32_t time);
	virtual void EnableNewVipBot(uint32_t time);

	virtual void ResetBeginTime(uint32_t beginTime);
	virtual uint32_t GetBeginTime();

	virtual void ResetTimer_Normal(int index, uint32_t time);
	virtual uint32_t GetTimer_Normal(int index);

	virtual void SetNewVipState(bool state);
	virtual bool GetNewVipState();
	virtual int32_t GetNewVipLevel();
	virtual void SetNewVipLevel(int32_t level);
	virtual ResultType AddNewVipReward(int vipLevel);
	virtual void SyncNewVipInfo();
	bool IsInVipRewardRecord(int vipLevel);

	ResultType TryFetchNewVIPBonus();
	void SyncNewVIPRewardToClient();

	void UpdateNewVipLevel();
	bool CheckCatchVipUpgradeCondition(int nextVipLevel);
private:
	bool m_bNewVipState;
	int32_t m_nNewVipLevel;
	mem::set<int32_t> m_nVipRewardRecord;

public:
	void ResetGoalProcess();
	bool DoGoalEvent(int type, GoalEvent* event);
	void AddGoalAccountByType(int type, int64_t deltaValue, int param);
	void AddGoalAccountByID(int id, int64_t deltaValue);
	void SyncGoalRecord();

	int32_t GetKillControllerAccountDailyCount();
	void AddKillControllerAccountDailyCount(int num);
	void ResetKillControllerAccountDailyCount();
private:
	GameLogic::GoalAsset m_nGoalAsset;
	int32_t m_nKillControllerAccountDailyCount;

public:
	virtual void ResetGameStoryJoinTimes();
	virtual void UpdateGameStoryJoinTimes(int storyID);
	virtual int32_t GetGameStoryJoinTimes(int storyID);
private:
	mem::map<int32_t, int32_t> m_nGameStoryJoinTimes;
	int32_t m_nJoinTimeStamp;
public:
	virtual void UpdateResourceRetrieveRecord();
	virtual ResultType OpenResourceRetrieve();
	virtual ResultType GetResourceRetrieverReward(int id, bool specialFind);
	virtual void SyncResourceRetrieverRecord();
	virtual bool ResourceRetrieveState();
	virtual void SyncResourceRetrieveState();
private:
	LC_ResourceRetrieveMgr m_nResourceRetrieveMgr;
public:
	virtual uint32_t GetAssignEscortTaskTime();
	virtual void UpdateAssignEscortTaskTime(uint32_t time);

	virtual void ResetEscortInfo();

	virtual ResultType CheckAcceptEscortTask();
	virtual int32_t GetEscortTimes();
	virtual void AddEscortTimes(int times=1);

	virtual ResultType AddEscortReward(int EscortType, int npcID);
	virtual void AddEscortRecord(int EscortType, int npcID, const StringType& name);
	virtual void SyncEscortState(int EscortType, int npcID, const StringType& name);

	virtual void SyncRobbingTimes();
	virtual void SyncEscortTimes();

	virtual void SendEscortRewardToMail();

	virtual void SyncEscortNpcState(int objectID, int charTypeID, int state);

	virtual void SyncTaskFail(int taskID);
private:
	bool CheckEscortVaild(int EscortType, int npcID);
	ResultType TryAddEscortReward(int EscortType, int npcID);
	void UpdateEscortNpcID(int EscortType, int npcID);

	void AddRobbingTimes(int times=1);

	uint32_t m_nAssignEscortTaskTime;//���Ѻ�������ʱ��
	int32_t m_nEscortTimes;//���ʹ���
	int32_t m_nRobbingTimes;//���ڴ���

	mem::map<int, mem::vector<int32_t> > m_nEscortTypeNpcID;

public:
	bool GetGuildRewardState();
	void SetGuildRewardState(bool state);

	void SetBuffLoginFlag(bool login);
	bool GetBuffLoginFlag();

	bool GetGuildRewardFlag();
	void SetGuildRewardFlag(bool flag);
private:
	bool				m_bGuildRewardFlag;
	bool				m_bGuildRewardIsVaild;//����ÿ�ո����Ƿ������ȡ
	bool				m_bLoginBuff;//�Ƿ��ǵ�¼ˢ������
private:
	int32_t m_nDebugCode; 
	bool m_nDebugCodeFlag;
	static int32_t m_nUpdateResult;

	int32_t m_nUpdateTimes;
public:
	void SetDebugCode( int nInput );
	static int32_t GetUpdateResult();
public:
	void AddLotteryConsumeUnBindYuanBao(int num);
	int32_t GetLotteryConsumeUnBindYuanBao();
	void SyncLotteryConsumeUnBindYuanBao();
private:
	int32_t m_nLotteryConsumeUnBindYuanBaoCount;

public:
	virtual int32_t ModifyDailyActivity(int32_t val);
	virtual ResultType ReceiveDailyAcivityRewards(int32_t rewardID);
	virtual void syncDailyActivity();
private:
	virtual void ResetDailyActivity();
	int ModifySkillUsableCountImpl(int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount);
	int ModifySkillMaxUsableCountImpl(int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount);
	int32_t GetSkillUsableCountImpl(int32_t nBuddyId, int32_t nSkillId);
	int32_t GetSkillMaxUsableCountImpl(int32_t nBuddyId, int32_t nSkillId);
	int32_t GetSkillEnableUsableCountImpl(int32_t nBuddyId, int32_t nSkillId);
private:
	int32_t			m_nDailyActivity;//��������ֵ
	mem::vector<int32_t> m_nDailyActivityRewardID;//�Ѿ���ȡ������ֵ����
public:
	virtual void InitActivateStarVein();
	virtual ResultType ActivateStarVein(int32_t starVeinID);
	virtual ResultType GetStarVeinReward(int32_t id);
	virtual void SyncActivateStarVein();
	virtual void SyncStarVeinGoalRecord();

	int32_t GetAllBuddyLevel();
	int32_t GetAllBuddyStarLevel();

	virtual int32_t GetFinishMainTaskID();
	virtual void SetFinishMainTaskID(int32_t finishMainTaskID);
	void SyncFinishMainTask();

	int32_t GetRankTypeByRankGameStory(int32_t storyID);
	virtual int32_t GetMapFinishCountFromGameStoryID(int32_t storyID);
	virtual void UpdateFinishCountFromGameStoryID(int32_t mapID);
	int32_t GetFinishMaxMapIDByStoryID(int32_t storyID);
private:
	int32_t m_nFinishMainTaskID;
	mem::map<int32_t, mem::set<int32_t> > m_nFinishGameStoryMapID;

private:
	virtual bool IsInActivateStarVeinRecord(int32_t starVeinID);
	virtual bool IsInStarVeinGoldRecord(int32_t goldID);
	virtual void AddStarVein(int32_t starVeinID);
	virtual void AddStarVeinGold(int32_t starVeinGoldID);
	virtual ResultType CanActivateStarVein(int currentID, int preID);
	virtual ResultType CanGetStarVeinReward(int currentID, int preID);
	virtual ResultType	CheckBuddyStarLevel(int conditions);
	virtual ResultType	CheckBuddyLevel(int conditions);
	virtual ResultType	CheckBuddySkillLevel(int conditions);
	virtual ResultType	CheckBuddyEquipSlotLevel(int conditions);
public:
	int GetBuddyStarLevel();
private:
	mem::vector<int32_t> m_nActivateStarVeinID;
	mem::vector<int32_t> m_nStarVeinGoalID;
public:
	virtual void CheckRefreshFDDayCron(int32_t deltaTime=1);
	virtual int32_t GetNextFDTime();
	virtual ResultType CanGetFirstDailyChargeReward();
	virtual int32_t GetFDDay();
	virtual void SyncFDDay();
	virtual void SyncFDRewardRecord();

	virtual ResultType GetFreeReward(uint32_t day, uint32_t RewarsID);
	virtual ResultType GetDisCountReward(uint32_t day, uint32_t RewarsID);
private:
	virtual bool CheckCanReward(uint32_t day, uint32_t RewardID, bool free);
	virtual void AddFDReward(uint32_t day, uint32_t RewardID, bool free);
	virtual void CheckRewardAll(uint32_t day);
private:
	int32_t m_nFDFlag;
	typedef mem::map<int32_t, PS_FirstDailyChargeReward> FirstDailyChargeRewardsMap;
	FirstDailyChargeRewardsMap m_firstDailyChargeRewards;

//ʥ��
public:
	virtual int GetSkillBookCount();
	virtual void SyncSkillBookSlotInfo();
	virtual ResultType ActivateSkillBook(int32_t skillBookID);
	virtual ResultType UnlockSkillBookSlot(int32_t index, int32_t slotID);
	virtual ResultType WearSkillBook(int32_t index, int32_t slotID, int32_t skillBookID);
	virtual ResultType UnWearSkillBook(int32_t index, int32_t slotID, int32_t skillBookID);
private:
	virtual void OnRefreshPlayerLevel_UnlockSkillBookSlot(bool loginIn);
	virtual ResultType CheckSkillBookSlotState(int32_t index, int32_t slotID);
	virtual void SetSkillBookSlotState(int32_t index, int32_t slotID, bool slotState);
	virtual void SyncModifiedSkillBookSlotData(int32_t index, int32_t slotID, bool slotState, int32_t skillBookID=0);
	ResultType TakeOffSkillBook(int32_t index, int32_t slotID);
	ResultType PutOnSkillBook(int32_t index, int32_t slotID, int32_t skillBookID);
	void InitSkillBookProperty(int32_t index, int32_t slotID, int32_t skillBookID);

	virtual int GetActivateSkillBookCount();
private:
	mem::vector<PS_SkillBookSlotData> m_nSkillBookSlotDataInfo[MAX_ACTIVE_BUDDY_NUM];

//��ɫ��������
public:
	void SyncBiographyRewardInfo();
	void SyncBiography();
	ResultType BiographyPartReward(int32_t biographyPartID);

	void GetFirstAssignBiographyPartID(mem::vector<int32_t>& biographyPartID);
	void GetAssignBiographyPartIDByParam(mem::vector<int32_t>& biographyPartID, int param1, int param2);
	void GetBiographyTaskID(int32_t biographyPartID, TaskIDVec& taskID);
	bool GetNextBiographyPartID(int32_t preBiographyID, int32_t& nextBiographyID);
	bool IsBiographyPreTask(int taskID);

	void AddAssignBiographyPartID(int32_t birgraphyID, int32_t biographyPartID);
	bool IsAssignBiography(int32_t birgraphyID);
	bool IsAssignBiographyPart(int32_t biographyPartID);

	void AddFinishBiographyPartID(int32_t biographyPartID);
	bool IsInFinishBiographyPartID(int32_t biographyPartID);

	bool CheckBiographyFinish(int32_t biographyID);
	bool AddFinishBiographyID(int32_t biographyID);

	void AddNextAssignBiographyPart(int32_t biographyPartID);
	void RemoveNextAssignBiographyPart(int32_t biographyPartID);

	bool IsLastBiographyPartID(int32_t biographyPartID);
private:	
	ResultType CheckCanFinishBiographyPart(int32_t biographyID);
	ResultType CheckFinishBiographyPart(int32_t biographyID);
	ResultType GetBiographyPartReward(int32_t biographyID);
	void UpdateFinishBiography();
private:
	bool m_nCheckBiography;
	typedef mem::set<int32_t> BiographyRewardSet;
	BiographyRewardSet m_biographytRewardPartSet;
	typedef mem::set<int32_t> BiographySet;
	BiographySet m_biographySet;

	//�Ѿ��ɷ��Ĵ��Ǻʹ����½�
	typedef mem::map<int32_t, mem::set<int32_t> > AssignBiographySet;	
	AssignBiographySet m_nAssignBiographySet;

	//����ɵĴ����½�
	typedef mem::set<int32_t> FinishBiographyPartSet;	
	FinishBiographyPartSet m_nFinishBiographyPartSet;

	//���ɷ���һ�´��ǵĴ����½�id
	typedef mem::set<int32_t> nextAssignBiographyPartSet;	
	nextAssignBiographyPartSet m_nNextAssignBiographyPartSet;

	//����Ҫ�洢�����ÿ�����ߣ��ӱ��и���
	mem::set<int32_t> m_nLastBiographyPartID;

	typedef mem::set<uint32_t> CommonItemRecordSet;
	CommonItemRecordSet m_CommonItemRecordSet;
public:
	bool TryInsertItemToRecordSet( uint32_t nItemId );
	bool InsertItemToRecordSet( uint32_t nItemId );
	bool FindItemInRecordSet( uint32_t nItemId );
public:
	void GetActSubPlayerInfoCache(PlayerACTSubInfo& info);
	void SetActSubPlayerInfoCache(const PlayerACTSubInfo& info);
	void SyncPlayerACTSubInfo(const PlayerACTSubInfo& info);
	void UpdateACTSubInfo(float currentTime);
	bool GetSaveState();
	void SetSaved();

	void GetActSubPlusPlayerInfoCache(PlayerACTSubInfo& info);
	void SetActSubPlusPlayerInfoCache(const PlayerACTSubInfo& info);
	void SyncPlayerACTSubInfo_Plus(const PlayerACTSubInfo& info);
	void UpdateACTSubInfo_Plus(float currentTime);
	bool GetSaveState_Plus();
	void SetSaved_Plus();
private:
	bool m_bACTSubSave;//��ͨ�����ҵĶ�ʱʱ�������Ƿ��Ѿ��浽redis
	bool m_bACTSubSave_Plus;//���״����ҵĶ�ʱʱ�������Ƿ��Ѿ��浽redis

	PlayerACTSubInfo m_nActSubPlayerInfo;
	PlayerACTSubInfo m_nActSubPlusPlayerInfo;
public:
	ResultType ResetShopTimes(int32_t shopType, int& resetTimes);
	int32_t GetShopResetTimes(int32_t shopType);
	ResultType AddShopResetTimes(int32_t shopType, int value);

	void clearShopResetTimes(int32_t shopType);
	void UpdataACTShopInfo(int32_t scheduleID);
	void GetShopItem(int32_t shopType, std::map<int32_t, int32_t>& itemInfo);
	bool CheckShopVaild(int32_t shopType, int shopAID);
private:
	ResultType checkReset(LC_ServerPlayer* pkPlayer, int32_t shopType, int32_t& shopTimes, CashType& moneyType, int32_t& moneyCount);
	ResultType ResetShop(int32_t shopType);
	void RefreshShopItem(int32_t shopType);
	void ClearShopItem(int32_t scheduleID);
	
	int32_t GetShopStartTime(int32_t shopType);
	void SetShopStartTime(int32_t shopType, int32_t startTime);

	mem::map<int, int> m_ShopTimes;
	mem::map<int, int> m_ShopStartTime;

	mem::map<int32_t, int32_t> m_nUnbindYuanbaoShopItemInfo;
	mem::map<int32_t, int32_t> m_nGuildShopItemInfo;
	mem::map<int32_t, int32_t> m_nMoneyCourageShopItemInfo;
	mem::map<int32_t, int32_t> m_nBossJPShopItemInfo;
	mem::map<int32_t, int32_t> m_nLimitShopItemInfo;


public:
	int32_t GetCollectBossJP();
	void AddCollectBossJP(int32_t value);
	void ResetCollectBossJP();
	void SyncCollectBossJP();

private:
	int32_t m_nCollectBossJP;
	
public:
	void UpdataInvestmentCardStartTime();
	void CheckInvestmentCardFinished();
	void SyncInvestmentCardRecord();
	ResultType BuyInvestmentCard(int32_t invesmentCardType);

	void SyncInvestmentCardRewardRecord();
	ResultType AddInvestmentCardReward(int id, int& rewardType);
private:
	void investmentCardReward(int32_t invesmentCardType);
	int32_t getInvestmentCardReward(int32_t id, UT_SIMDataList& rewardList, bool addRewardRecord);
	ResultType checkInvestmentAchievementCondition(int achievementID, bool checkAndAdd);

	int32_t GetInvestmentCardActivityStartTime(int32_t invesmentCardType);
	void UpdateInvestmentCardActivityStartTime(int32_t invesmentCardType, int startTime);
	void RemoveInvestmentCardActivityStartTime(int32_t invesmentCardType);

	CashType GetCashByInvesmentCardType(int32_t invesmentCardType);

	bool IsInInvestmentCardRecord(int32_t invesmentCardType);
	bool addInvestmentCardRecord(int32_t invesmentCardType);
	void removeInvestmentCard(int32_t invesmentCardType);

	bool IsInFreeRewardRecord(int32_t id);
	bool IsInHighRewardRecord(int32_t id);
	bool IsRewardVaild(int32_t id);
	void clearInvestmentRewardRecord(int32_t invesmentCardType);

	void addFreeRewardRecord(int32_t id);
	void addHighRewardRecord(int32_t id);
	void updateInvestmentCardAchievementFinished(int32_t id);

	mem::map<int32_t, int32_t> m_nInvestmentCardStartTime;
	mem::set<int32_t> m_nInvestmentCardRecord;
	mem::set<int32_t> m_nInvestmentCardFreeRewardID;
	mem::set<int32_t> m_nInvestmentCardHighRewardID;

public:
	bool CheckTimeLimitedActivityValid(int32_t id);
	void SetTimeLimitedActivityState(int32_t ACTShopTimeLimitedID, ActivityState state);
	void SyncPlayerTimeLimitedActivity();
	virtual void TimeLimitInfoEvent(TimeLimitEvent& event);
	void UpdateTimeLimitedActivity();
private:
	mem::vector<TimeLimitedInfo> m_nTimeLimitedRecord;

public:
	ResultType AssignRewardTask(int assignChannel, int endTime=0);
	ResultType RefreshRewardTask();
	void SyncRefreshRewardTaskTimes();
	int32_t AddRefreshRewardTaskTimes();
private:
	int32_t GetRefreshRewardTaskTimes();	
	void ResetRefreshRewardTaskTimes();
	int32_t m_nRefreshRewardTaskTimes;

public:
	virtual bool CheckRefreshRewardTask();
	virtual int GetAcceptRewardTaskTimes();
	virtual void ModifyAcceptRewardTaskTimes(bool add, int value=1);
	virtual void ResetAcceptRewardTaskTimes();
	virtual void SyncAcceptRewardTaskTimes();

	virtual int32_t GetRewardTaskEndTime();
	virtual void SetRewardTaskEndTime(int32_t time);
private:
	int32_t m_nAcceptRewardTaskTimes;
	int32_t m_nRewardTaskEndTime;

public:
	virtual ResultType GetChapterRaidStarReward(int chapterID, int rewardIndex);
	virtual void AddRaidStarNumByChapterID(int mapID, int chapterID, int num);
	virtual int32_t GetRaidStarNumByChapterID(int chapterID);
	virtual int32_t GetRaidStarNum();

	void SyncTotalRaidStarNum();
	virtual void SyncAllChapterRaidStarNum();
	virtual void SyncAllChapterRaidStarRewardRecord();

private:
	void UpdateChapterRaidStarRewardRecord(int chapterID, int rewardIndex);
	int32_t AddTotalRaidStarNum(int32_t num);
	uint64_t GetRongGuangYuanZhengMapValue();
	void SyncChapterRaidStarNum(int chapterID, int num);
	void SyncChapterRaidStarRewardRecord(int chapterID);

	void LoadRongGuangYuanZhengData(const ::char_data_define::PBRongGuangYuanZheng& data);
	void SaveRongGuangYuanZhengData(::char_data_define::PBRongGuangYuanZheng* pkData);
	
	mem::map<int, mem::vector<int> > m_nChapterRewardRecord;
	mem::map<int, int> m_nChapterRaidStarNum;	
	int32_t m_nTotalRaidStarNum;
public:
	int32_t GetControllerEquipSlotMoldingSpiritLevel(int slotIndex);
	int32_t GetControllerEquipSlotReinforceLevel(int slotIndex);
	int32_t GetControllerEquipSlotRefineLevel(int slotIndex);
public:
	void UpgradeSubStage(int32_t subType, int stage);

	//������װ���������
	virtual mem::map<int, int>* GetControllerMoldingSpiritSuitInfo();
	//��ý�����װ������
	void GetSubEquipItemInfo(int32_t subType, std::map<int32_t,PS_SubEquipSlotItemInfo>& data);
	//��ý�����װ��pack
	CCommanderSubEquipPack* GetSubEquipPackByType(int32_t subType);
	//���½���������
	void UpdateSubAttributeMapBySubType(int32_t subType, LC_ActorAttributeMap& attrMap);
	//��ý�����װ������
	virtual void GetSubEquipAttr(int32_t subType, LC_ActorAttributeMap* pkAttrMap, int& nAppendixScore);
	//��ý���������������
	virtual void GetSubEquipSlotMoldingAttr(int subType, LC_ActorAttributeMap* pkAttrMap);
	//��ý���������ֵ����
	virtual void GetSubEquipSlotMoldingSuitValueAttr(int subType, LC_ActorAttributeMap* pkAttrMap);
	//��ý���������ٷֱ�����
	virtual void GetSubEquipSlotMoldingSuitPercentAttr(int subType, LC_ActorAttributeMap* pkAttrMap);

	//��¼��ˢ�½���������
	void EquipAllSubItem(bool login);
	//������װ����λ����
	void UpdateSubEquipSlotMoldingSpiritLevel(int32_t subType, int32_t slotIndex);
	//�����ߴ�װ��
	void EquipSubItem(int32_t subType, int32_t srcSlotIndex);
	//��������װ��
	void UnEquipSubItem(int32_t subType, int32_t srcSlotIndex);
	//������װ������
	void SubEquipAdvance(int32_t subType, int32_t slotIndex);

	//����װ�����
	//��ҵ�¼����ʼ����������
	void EquipAllRuneItem(bool login);
	//��������
	void RuneUpgrade_New(int32_t buddyId, int32_t slotIndex, int32_t targetLevel);
	//��������
	void RuneStar_New(int32_t buddyId, int32_t slotIndex);
	//��������
	void ResetRune_New(int32_t buddyId, int32_t slotIndex);
	//��buddy��������
	void EquipRuneToBuddy(int32_t srcBuddyID, int32_t srcSlotIndex, int32_t destBuddyID, int32_t destSlotIndex);
	//���±���
	void UnEquipRuneFromBuddy(int32_t srcBuddyID, int32_t srcSlotIndex);
	//һ�����ϱ���
	void OneKeyEquipRuneToBuddy(int32_t buddyID);
	//һ��ж�±���
	void OneKeyUnEquipRuneFromBuddy(int32_t buddyID);

	//��ȡ�����ռ�����
	void RuneReward(int32_t lItemID);
	//��ȡbuddy�ı��߲�λ״̬
	bool	GetBuddyRuneEquipState();
	//����buddy�ı��߲�λ
	bool	SetBuddyRuneSlotState();

	//���߲�λ�Ƿ���Դ���
	bool GetRuneSlotDressState(int32_t buddyID, int32_t slotIndex);
	//���ñ��߲�λ���Դ���
	void SetRuneSlotDressState(int32_t buddyID, int32_t slotIndex);
protected:
	//����ͬ���������ͻ���
	void SyncAllBuddyRuneEquipToClient();

	//����������������
	void BasicPackRuneUpgrade(int32_t slotIndex, int32_t targetLevel);
	ResultType DoBasicPackRuneUpgrade(int32_t slotIndex, int32_t targetLevel, PS_ItemAssetEntry& slotData);
	//buddy��������
	void BuddyRuneUpgrade(int32_t buddyId, int32_t slotIndex, int32_t targetLevel);

	//����������������
	void BasicPackRuneStar(int32_t slotIndex);
	ResultType DoBasicPackRuneStar(int32_t slotIndex, PS_ItemAssetEntry& slotData);
	//buddy��������
	void BuddyRuneStar(int32_t buddyId, int32_t slotIndex);

	//����������������
	void BasicPackRuneReset(int32_t slotIndex);
	ResultType DoBasicPackRuneReset(int32_t slotIndex, PS_ItemAssetEntry& slotData);
	//buddy��������
	void BuddyRuneReset(int32_t buddyId, int32_t slotIndex);

	//��buddy��������
	ResultType DoEquipRuneToBuddy(int32_t srcBuddyID, int32_t srcSlotIndex, int32_t destBuddyID, int32_t destSlotIndex, PS_ItemAssetEntry& srcData, PS_ItemAssetEntry& destData);
	//���±���
	ResultType DoUnEquipRuneFromBuddy(int32_t srcBuddyID, int32_t srcSlotIndex);
	//һ�����ϱ���
	ResultType DoOneKeyEquipRuneToBuddy(int32_t buddyID, std::map<int32_t, PS_ItemAssetEntry>& destSlotData);
	//һ��ж�±���
	ResultType DoOneKeyUnEquipRuneFromBuddy(int32_t buddyID, std::set<int32_t>& slotIndexVec);

	//��ȡbuddy���߲�λ��ı�������
	const LC_BackPackEntry* GetRuneDataFromBuddyRune(int32_t itemID);
	//����buddyID��index����ȡ���ߵ�����
	LC_BackPackEntry* GetRuneItemFromBuddy(int32_t buddyID, int32_t slotIndex);
	//���buddy���ߵ�itemid�͵ȼ�
	void GetAllBuddyRuneItemLevelData(mem::map<int32_t, int32_t>& itemID2LevelMap);
	//���buddy���ߵ�itemid���Ǽ�
	void GetAllBuddyRuneItemStarLevelData(mem::map<int32_t, int32_t>& itemID2StarLevelMap);

	//���±����ռ���Ϣ
	void UpdateRuneRewardRecord(int32_t lItemID, bool addReward=false, bool modifyAddReward=false);
	//ˢ��buddy���߿ɴ���״̬
	void CheckRuneSlotDressState();
public:
	virtual void UpdateStoryEndTime(int storyID, int32_t time);

	//�ż�ͻϮ
	ResultType GetYiJiTuXiStageReward(int id);
	bool GetYiJiTuXiRewardID(int id);
	void AddYiJiTuXiRewardID(int id, bool sync=false);
	void SyncYiJiTuXiRewardRecord(bool checkVaild=false);
	virtual void SendYiJiTuXiReward();
	virtual int32_t GetYiJiTuXiEndTime();
	virtual void ResetYiJiTuXiRewardRecord(int32_t time=0);

	//�ż�Ѱ��
	ResultType GetYiJiXunBaoStageReward(int id);
	bool GetYiJiXunBaoRewardID(int id);
	void AddYiJiXunBaoRewardID(int id, bool sync=false);
	void SyncYiJiXunBaoRewardRecord(bool checkVaild=false);
	virtual void SendYiJiXunBaoReward();
	virtual int32_t GetYiJiXunBaoEndTime();	
	virtual void ResetYiJiXunBaoRewardRecord(int32_t time=0);

	//��ߵ�
	ResultType GetPetIslandsStageReward(int id);
	bool GetPetIslandsRewardID(int id);
	void AddPetIslandsRewardID(int id, bool sync=false);
	void SyncPetIslandsRewardRecord(bool checkVaild=false);
	virtual void SendPetIslandsReward();
	virtual int32_t GetPetIslandsEndTime();	
	virtual void ResetPetIslandsRewardRecord(int32_t time=0);

	virtual void UpdateResetAllCommonStatFlag(int storyID);
	virtual bool GetResetAllCommonStatFlag(int storyID);
	virtual void ClearResetAllCommonStatFlag(int storyID);
private:
	void UpdateYiJiTuXiEndTime(uint32_t endTime);
	mem::vector<int> m_nYiJiTuXiStageRewardRecord;
	int32_t m_nYiJiTuXiEndTime;
	bool m_nYiJiTuXiSendFlag;

	void UpdateYiJiXunBaoEndTime(uint32_t endTime);
	mem::vector<int> m_nYiJiXunBaoStageRewardRecord;
	int32_t m_nYiJiXunBaoEndTime;
	bool m_nYiJiXunBaoSendFlag;

	void UpdatePetIslandsEndTime(uint32_t endTime);
	mem::vector<int> m_nPetIslandsStageRewardRecord;
	int32_t m_nPetIslandsEndTime;
	bool m_nPetIslandsSendFlag;

	mem::set<int> m_nResetAllCommonStatID;

public:
	ResultType BuyRareSubDevaEyeTransform(int shopID);
	virtual void SetKillNPCExpFlag(bool tag);
	virtual bool GetKillNPCExpFlag();
	virtual float GetKillNPCExpPercent();
private:
	bool m_bKillNPCExpFlag;
public:
	void				SyncBuddyGrowthPackage();
	ResultType	BuyBuddyGrowthPackage(int32_t buddyID);
	ResultType	GetBuddyGrowthReward(int32_t buddyID);
public:
	virtual void SyncCollectionInfo();
	virtual ResultType CollectionActivation(int32_t collectActivateID);
	virtual ResultType CollectionReward(int32_t collectRewardID);	
	virtual int32_t GetActivateCollectionCount();
private:
	virtual ResultType CheckCollectionActivation(int32_t collectActivateID);
	virtual ResultType CheckCollectionReward(int32_t collectRewardID);

public:
	typedef mem::set<int32_t> CollectionRewardSet;
	CollectionIDMap &GetCollectionIDMap(){return m_collectionActivationIDMap;}
private:
	CollectionIDMap m_collectionActivationIDMap;
	CollectionRewardSet m_collectionRewardSet;
	CollectionRewardSet m_collectionAchievementIDSet;
public:


	void UpdateStateCDTime(float fCurrentTime, float fDeltaTime);
	virtual float GetStateVaildTime(int32_t stateID, Skill::SkillEventTypeEx eEventType);
	virtual void SetStateTime(int32_t stateID, float time, Skill::SkillEventTypeEx eEventType);

	virtual void SetStateExtendDataOnController(int lStateTypeID, int iParameterIndex , int64_t iValue);
	virtual int64_t GetStateExtendDataOnController(int lStateTypeID, int iParameterIndex);

	virtual void SetStateMutableParameterOnController(int lStateTypeID, int iParameterIndex, double fValue);
	virtual double GetStateMutableParameterOnController(int lStateTypeID, int iParameterIndex);

private:
	typedef mem::vector<float> ShareStateCDTime;
	typedef mem::map<int32_t, ShareStateCDTime> ShareStateCDTimeMap;
	ShareStateCDTimeMap m_nShareCDTimeRecord;
	bool UpdateSingleStateCDTime( float fDeltaTime, ShareStateCDTime& data );

public:
	uint64_t GetInviteTime(unique_id_type charID);
	void SetInviteTime(unique_id_type charID, uint64_t currentTime);
private:
	typedef mem::map<unique_id_type, uint64_t> InviteRecordsMap;
	InviteRecordsMap m_inviteRecordsMap;//���빫����ѽ�����Ӹ�����ʱ���¼
public:

	virtual void OnVIPReport(uint32_t optype, uint32_t viptype, uint32_t buytime, uint32_t viplv);

	

public:
	void UpdateHolyDeedRecord(int itemID, int itemCount=1);
	void NotifyAddNewHolyDeed(int itemID);
	void SyncHolyDeedRecord();
private:
	mem::map<int32_t, int32_t> m_nHolyDeedRecords;

public:
	virtual void SetDecompositionSetting(MG_REQ_DecompositionSetting& msg);
	void SyncDecompositionSetting();

	virtual bool SetAutoDecomposition(bool tag);
	virtual bool GetAutoDecomposition();
	virtual void CheckDecompositionItem();
private:
	bool m_bDecompositionInit;//�Ƿ����ù�
	bool m_bAutoDecomposition;//�Ƿ��Զ��ֽ�
	bool m_bDecompositionController;
	int32_t m_nControllerItemlevel;//ָ�ӹ�װ���ֽ�ȼ�
	int32_t m_nControllerItemGrade;//ָ�ӹ�װ���ֽ�Ʒ��
	bool m_bDecompositionVehicle;
	int32_t m_nVehicleItemLevel;//����װ���ֽ�ȼ�
	int32_t m_nVehicleItemGrade;//����װ���ֽ�Ʒ��
	bool m_bDecompositionDevaeye;
	int32_t m_nDevaeyeItemLevel;//�ȳ�װ���ֽ�ȼ�
	int32_t m_nDevaeyeItemGrade;//�ȳ�װ���ֽ�Ʒ��
	bool m_bDecompositionWing;
	int32_t m_nWingItemLevel;//���װ���ֽ�ȼ�
	int32_t m_nWingItemGrade;//���װ���ֽ�Ʒ��

public:
	virtual ResultType GetFirstChargeReward(int id);
	virtual void UpdateFirstChargeTime();
	virtual void SyncFirstChargeInfo();

	virtual ResultType CheckGetFirstChargeReward(int id);
	virtual void UpdateFirstChargeRewardState(int id);
private:	
	uint32_t m_nFirstChargeTime;
	mem::map<int, bool> m_nFirstChargeRewardState;

public:
	virtual ResultType GetVIPExtraReward(std::vector<PS_ItemRecord>& itemInfo);
	virtual int GetUsedVIPExtraRewardTimes();
	virtual void AddUsedVIPExtraRewardTimes(int times = 1);
	virtual void ResetUsedVIPExtraRewardTimes();

	virtual void SyncVIPExtraRewardUsedTimes();
private:
	int32_t m_nUsedVIPExtraRewardTimes;

public:
	virtual void CheckAnswerActivityTime();
	virtual int32_t GetAnswerActivityEndTime();
	virtual void RestartAnswerActivity(int32_t time);
	virtual void SyncTestDatabase(bool checkVaild=false);

	virtual ResultType GetAnswerRankInfo(int32_t minIndex, int maxIndex, MG_RLT_AnswerRankInfo& msg);
	virtual bool CommitAnswer(int topicIndex, int choice);
	
	bool CheckCommitVaild(int topicIndex);
	bool CheckAnswerChoice(int topicIndex, int choice);
	void CheckAnswerAward();
	void SyncAnswerInfo(bool checkVaild=false);
public:
	//����ս ���ְ� ��ؽӿ�
	// 
	ResultType FillGuildFightRankMyInfo(PS_RANK_STRUCT &outInfo,int32_t myIndex,int nType);
	ResultType GetGuildFightRankInfo(MG_REQ_GuildFightRankInfo &reqMsg, MG_RLT_GuildFightRankInfo& msg);
private:

	bool SaveAnswerData(::char_data_define::PBAnswerData* pkData);
	bool LoadAnswerData(const ::char_data_define::PBAnswerData& data);
	int32_t	m_nAnswerActivityEndTime;

	bool m_bUpdataRedis;
	mem::map<int32_t, bool> m_nAnswerRecord;//�Ѿ��������indexID�������¼
	int32_t m_nAnswerAccPoint;//����õ��Ļ���
	int32_t m_nAnswerGuildAccPoint;//���⹤��õ��Ļ���
	mem::vector<int32_t> m_nAnswerRewardRecord;//������ַ�����¼

public:
	void GetNearbyPlayer(MG_RLT_GetNearbyPlayer& msg);
public:
	void RecomputerRuneTotalLevel();
	void RecomputerRuneTotalStarLevel();

	int32_t GetRuneTotalLevel();
	int32_t GetRuneTotalStarLevel();

	void FillRuneLevelInfoPB( ::google::protobuf::RepeatedField< ::google::protobuf::int32 > & item_info, ::google::protobuf::RepeatedField< ::google::protobuf::int32 > & level_info );
	
	void SyncBuddyRuneSlotState(bool broadcase=false);
	void BroadcastBuddyStarLevel(object_id_type nPlayerId, int32_t nBuddyId, int32_t nLevel);
	void BroadcastBuddyRuneStarLevel(object_id_type nPlayerId, int32_t nBuddyId, int16_t nPos, int32_t nItemId, int16_t nLevel);
	void BroadcastBuddyEquipSlotStarLevel(object_id_type nPlayerId, int32_t nBuddyId, int32_t equipSlotIndex, int32_t equipSlotStarLevel);
	void BroadcastBuddyEquipSlotState(bool broadCast);
	void BroadcastBuddyEquipSlotLevelCount(object_id_type nPlayerId, int32_t nBuddyId, int level);
	void BroadcastBuddyEquipItem(object_id_type nPlayerId, int32_t nBuddyId, int32_t equipSlotIndex, int32_t equipItem);
	void BroadcastBuddyCurrentTransformID(object_id_type nPlayerId, int32_t nBuddyId, int32_t nBuddyCurrentTransformID);
private:
	bool DecompositionRune(int32_t lItemID, int32_t slotIndex, std::vector<PS_AddRuneInfo>& info);
	
public:
	virtual void ApplyCommanderTitleSystem();
	virtual ResultType UpgradeCommanderTitle();
	virtual int GetCommanderTitle();

private:
	virtual ResultType UpdateCommanderTitle(int newTitle);
	virtual ResultType CheckUpgradeCommanderTitle();
	bool SetCommanderTitle(int32_t titleID);

	int m_nCommanderTitle;//ָ�ӹ�ͷ��id��¼

public:
	virtual ResultType GetServerFirstKillBossReward(int npcID);
	virtual ResultType CheckCanGetServerFirstKillBossReward(int npcID);
	virtual ResultType UpdataServerFirstKillBossReward(int npcID);
	virtual void SyncServerFirstKillBossReward();

	virtual ResultType GetKillBossReward(int npcID);
	virtual ResultType CheckCanGetKillBossReward(int npcID);
	virtual void AddKillBossRecord(int npcID);
	virtual ResultType UpdateKillBossRecord(int npcID);
	virtual void SyncKillBossReward();

	virtual void SyncServeFirstKillBossInfo();
private:
	mem::set<int32_t> m_nFirstKillBossRewardRecord;
	mem::map<int32_t, bool> m_nKillBossRecord;

public:
	virtual bool CheckSevenDayTime1Vaild();
	virtual bool CheckSevenDayTime2Vaild();
	virtual void SyncSevenDayTime();
	virtual uint32_t GetSevenDayEndTime1();
	virtual uint32_t GetSevenDayEndTime2();
private:
	uint32_t m_nSevenDayEndTime1;
	uint32_t m_nSevenDayEndTime2;

public:
	virtual void SyncFirstDayLoginRewardTime();
	virtual uint32_t GetFirstDayLoginRewardTime();
private:
	uint32_t m_nFirstDayLoginRewardTime;

public:
	virtual ResultType GetSpecialTrainingReward(int id);
	virtual void SyncSTRewardRecord();
private:
	virtual bool CheckSTRewardRecord(int id);
	virtual ResultType CheckSTRewardCondition(int id);
	virtual ResultType AddSTTaskReward(int id);
	virtual ResultType UpdateSTRewardRecord(int id, const Utility::UT_SIMDataList& reward);

	mem::set<int32_t> m_nSTRewardRecord;

public:
	void SyncSubBData();
	void GetSubBDataByType(const std::vector<int32_t>& typeVec);
	void GetRuneItem(mem::set<int32_t>& runeItemID);
	void UpdateSubBInfo(bool force=false);
	virtual void UpdateSubB(int type, bool force=false, int param=0);
	uint64_t GetCombatEquipSlotCombatScoreByBuddys(const Utility::UT_SIMDataList& params, int param=-1);
	int GetStarLevelByBuddys(const Utility::UT_SIMDataList& params, int param=-1);
	uint64_t GetRuneItemCombatScoreByItemID(const Utility::UT_SIMDataList& params, int param=-1);

	void CheckUpdateSubBInfo();

	bool IsInRankGroup(int groupID);
	void UpdateRankGroup(int groupID);

	uint64_t GetSubBValue(int groupID);
	void UpdateSubBValue(int groupID, uint64_t value);

	uint64_t GetSubBSendValue(int groupID);
	void UpdateSendSubBValue(int groupID, uint64_t value);
	ResultType GetActivityRankInfo(int type, int param, int minIndex, int maxIndex, MG_RLT_GetRankSubBInfo& msg);

	void UpdateRankInfoToRedis(int type, bool force=false, int param=0, int value=0);
	void UpdateCharge(int type, int id, int value);

	ResultType CheckSubBParam(int type, int param);
	ResultType CheckChargeParam(int holidayType, int param);
private:
	mem::map<int, uint64_t> m_nSubBInfo;
	mem::map<int, uint64_t> m_nSendSubInfo;
	mem::set<int> m_nRankGroupID;
public:
	int8_t GetMoveActionMode();
	void SetMoveActionMode( int8_t nInput );
private:
	int8_t m_nMoveActionMode;
	float				m_fLastProfileTime;
	
	ServerViewState		m_serverViewState;

	//�ͻ�����ʱ
	float				m_fLastClientCheckTime;
	float				m_fLastServerCheckTime;
	float				m_fLastTimerCheckTime;
	float				m_fLastSyncMoveTime;
	float				m_fLastServerSetLocationTime;
	float				m_fLastPickItemTime;
	int					m_lAccumulateMoveDistance;
	Utility::UT_Vec3Int	m_kLastMoveCheckLocation;

	//�Ṧ
	float				m_CloseFlyFlagTime;	//(s)�ر�SK_FLY��־λ����ʱ��FLY����Ϊ�޵�״̬�������������У�飬������ȫ���ſͻ���
	float				m_CloseFlyFlagBeginTime;	//(s)
	//�Ṧ�ŵغ�һ��ʱ�����Ṧ��������Ϊ���޵�
	float				m_LastJumpBeginTime;	//�Ṧ����ʱ��s
	float				m_LastJumpEndTime;	//�Ṧ�ŵ�ʱ��s

	size_t				m_gatewayIndex;

	//������ҵ�ͼλ����Ϣ
	GameLogic::LC_WorldLocationAsset	m_kWorldLocationAsset;

	//���ݿ�洢����
	unsigned int		m_ulDBSaveMaskFlag;

	bool m_nAttackFlag;

	Protocol::PS_CharArenaInfo			m_arenaInfo;

	// ������������
	Protocol::PS_ClientAuctionSell		m_kAuctionSellData;
	unique_id_impl						m_uiAuctionBuyID;
	//uint64_t			m_uiAuctionBuyPrice;
	Utility::LC_CashMap					m_kAuctionBuyPrice;

	bool				m_bFollowedByGM;

	float				m_fChangeServerCDTime;
	float				m_fLastUpdateChangeServerTime;

	float				m_fLastHeartBeatTime;

	int					m_lVIPLevel;
	int					m_uiLoginIP;
	StringType			m_kUserName;
	uint32_t			m_uiClientIP;
	uint64_t			m_ulClientMAC;

	int32_t				m_iDBUpdateTick;

	bool				m_bAddShapeShiftForbidAction;
	float				m_fLastCheckEquipEnduranceTime;

	uint32_t			m_uiGetAddHookTimeItemDay;
	bool				m_bHookForbid;

	uint32_t			m_iLastUpdateItemLogTime;
	float				m_fFightStateBeginTime;						//��¼����ս����ʼʱ��
	float				m_fFightOutBeginTime;
	UT_GameServerPackFilter	m_kGameServerPackFilter;				//�հ�Ƶ�ʿ���

	bool				m_bGetGuildData;
	uint32_t			m_lAccountLoginTime;						//�ʺŵ�¼ʱ��
	bool				m_Courage;									//�Ƿ������5������buffer

	uint32_t			m_lSurvivalTime;
	float				m_fSurvivalStartTime;
	float				m_fSpawnTime;

	float				m_fLastPositiveActiveTime;
	float				m_fLastExecuteSkillTime;					//npc�ϴ��ͷż��ܵ�ʱ�䣬����npc�ͷż��ܵ�cd����

	// ForBuddyAIOnly
	float				m_fLastCastSkillTime;

	float				m_fLastIdleTime;
	float				m_fLastTalkTime;

	Utility::UT_LinePath    m_kMovePath;							//·������

	bool					m_bCacheMovePath;
	Utility::UT_Vec3Int		m_kCacheLocation;
	Utility::UT_Vec3Int		m_kMoveLineTargetLocation;
	Utility::UT_LinePath	m_kCacheMovePath;

	bool					AutoEnterBornRaid;

	bool					m_Adult;								//�Ƿ����������
	uint32_t				m_anitWallowOnlineTime;					//����������ʱ��
	int32_t					m_antiWallowIntervalTS;					//�������ж�ʱ����

	uint32_t				m_TotalOnlineTime;						//������ʱ��
	uint32_t				m_nSessionOnlineTime;					// �Ự�ڼ�����ʱ��

	int32_t					m_MapRaodmap;

	on_summonfinish_handler m_SummonFinishHandle;
	on_answer_res_result_handler    m_AnswerResHandle;

	float					m_fLastUpdatePropellTime;
	uint64_t				m_uiPropellID;

	int						m_iComboEffectRound;					//������Ч����
	int						m_iComboCountAtLastRound;				//��һ����������
	float					m_fComboEffectTime;						//�ٴβ���������Ѫ��Ч��ʱ������

	bool					m_AIControlNotify;
	int32_t					m_auction_packtype;
	int32_t					m_auction_index;
	int32_t					m_LoginCount;

	//���Utf-8������Ϣ
	StringType				m_kUserName_utf8;

	uint64_t                m_userKey; // ���ε�¼��UUID
	platform_vip_info		m_platformvipinfo; //ƽ̨��¼��һЩ�����Ϣ

	OfflineExpData			m_OffLineExpData;
	MySyicalShopMark		m_MySyicalShopMark;

	uint32_t				m_tGlobalShopAssetDirty;
	
	mem::map<int64_t, LC_ResourceRecord>	m_kOssResourceCache;
	LC_ResourceAlert		m_kResourceAlert;

	RechargeRecord			m_kRechargeRecord;

	LC_FacebookRecord		m_kFacebookRecord;
protected:
	Activity::ACT_SecretTreasureAsset		m_kSecretTreasureAsset;
	Activity::ACT_ScheduleAsset				m_kACTScheduleAsset;
public:
	//virtual void ModifyAttributeBySubSystem(int subSystemType, StringType& attrString, bool add=true);
private:
	//void ModifyAttribute(int subSystemType, int partIndex, int iAttributeType, attr_value_type iValue, bool add=true);
	//attr_value_type GetModifiedAttributeDeltatValue(int subSystemType, int partIndex, int iAttributeType, attr_value_type iValue, bool add=true);
	void PostInitGameData( int nCharDataVersion );
	void OnAchieveEvent_BuddyInstanceCreate( BuddyInstance* pInput );
private:
	typedef mem::map<int32_t, LC_ActorAttributeMap> LC_SubSystemAttributeMap;
	LC_SubSystemAttributeMap m_SubSystemAttributeMap;
public:
	void					SetAntiWallowIntervalTime(int32_t value) { m_antiWallowIntervalTS = value; }

	void					SetAdult(bool value);
	bool					GetAdult()const { return m_Adult;}

	void					SetAntiWallowState(GameLogic::AnitWallowState state);

	uint64_t GetMJBossValue();
	int32_t GetEndlessTotalFloor();
	virtual void GetEndlessRecords(mem::map<int, int>& records);
	bool checkEnterEndLessTower(CF_EndlessTower::DataEntry* pkData);
	bool CheckEndLessTowerIsFinished(int type);

	void					SetAnitWallowOnlineTime(int32_t value);
	uint32_t				GetAntiWallowOnlineTime()const { return m_anitWallowOnlineTime; }
	void					Sync_AnitWallowOnlineTime();

	uint32_t				GetTotalOnlineTime() { return m_TotalOnlineTime;}
	void						SetSessionOnlineTime( uint32_t nTime );
	uint32_t				GetSessionOnlineTime();

	bool					CheckUnSummorSoon(float current_time);
	bool					CheckUnSummor(float current_time);

	const Utility::UT_Vec3Int&  GetMoveLineTargetLocation() { return m_kMoveLineTargetLocation;}
	virtual void				SetMoveLineTargetLocation(const Utility::UT_Vec3Int& kLocation);

	Utility::UT_LinePath&	GetMovePath() { return m_kMovePath;}
	void					FindPathTo(const Utility::UT_Vec3Int& rkTarget);
	void					CacheCurrentPath(const Utility::UT_Vec3Int&	rkCacheLocation);
	void					RecoverCurrentPath();
	const Utility::UT_Vec3Int& 	GetCacheLocation() { return m_kCacheLocation;};
	bool					IsCachedMovePath() { return m_bCacheMovePath;};

	void					FinishPlayerCloneLoad(int nIndex, bool isSoloDuleRobot, uint32_t schemeType, uint32_t groupID = 1);
	void					ExecuteSummonFinishHandle();
	void					NoticeFriendOffline();
	void					UpdateSocietyAsset(GameLogic::LC_SocietyBasicInfo& rkBaseInfo);
	void					SummorFriendTalk(int type , float CurrentTime);

	bool					NotifyStateEnd(int iEndState, float fCurrentTime, float fDeltaTime);

	int						TestBornQuest();

	int32_t					GetMapRaodmap();
	void					SetMapRaodmap(int32_t map_res_id);

	int32_t					GetAuctionItemPackType() { return m_auction_packtype; }
	void					SetAuctionItemPackType(int value) { m_auction_packtype = value; }

	int32_t					GetAuctionItemPackIndex() { return m_auction_index; }
	void					SetAuctionItemPackIndex(int value) { m_auction_index = value; }

	void					RegisterCharInfo();

	int32_t					GetLoginCount() { return m_LoginCount; }

	bool					IsAIControlStand() { return m_AIControlStand;}
	void					SetAIControlStand(bool flag) {	m_AIControlStand = flag; }

	bool OpenFDeposit(bool bTest=false);
	virtual void NotifyFD();

	void FillAttrMap(std::map<int32_t, int64_t>& attrMap, LC_ActorAttributeMap* map);

	//�����Ϊ���ɿ��������ߵ�ʱ����Ҫǿ��ִ�е�ϵͳ�߼�
	void					ForceSaveSystemData();

	void					AccumulateRunTime(float fCurrentTime);
	void					ResetRunTime(float t) {	m_fRunStateAccumulator = t; m_fRunStateTick = GET_PROCESS_TIME;}
	bool					IsDown() { return m_fRunStateAccumulator < 0.0f; }
	bool					GetSyncPlayerMove()	{ return m_syncPlayerMove; }
	void					SetSyncPlayerMove(bool val);
	void					SetRunMoveTick(float tick) { m_fRecvMoveTick = tick; }
	float					GetRunMoveTick() { return m_fRecvMoveTick; }
	int32_t					GetCrossServerType() { return m_nCrossServerType; }
	void					SetCrossServerType(int32_t nType) { m_nCrossServerType = nType; }
	uint64_t				GetLastRechargeTime() const { return m_iLastRechargeTime; }
	void					SetLastRechargeTime(int64_t iVal) { m_iLastRechargeTime = iVal; }

protected:
	void					_notifyItemTimelimit(GameLogic::LC_BackPackEntry* pkEnrty, int32_t deletTime, int32_t curentTime);
	void					_updateCrossServerCheck(uint32_t nTime);
	void					_autoRecharge();

	StateMachine::SM_StateBase**		m_kStateMap;	//״̬������״̬ȡ��StateFactory��ֻ�������ã�����Ҫdelete��ayu��
	int						m_stateCount;

	bool					m_AIControlStand;
	float					m_fRunStateAccumulator;
	float					m_fRunStateTick;
	float					m_fRecvMoveTick;
	bool					m_syncPlayerMove;
	bool					m_RunSpeedUpDirty;
	uint32_t				m_nAttackNoticeCD;
	int32_t					m_nCrossServerType;
	uint32_t				m_tCrossServerCheck;

	uint64_t				m_iLastRechargeTime;
	uint64_t				m_iLastSyncAllianceTime;
	int m_nCharDataVersion;
	LC_SkillIDVector		m_kBoneSoulSkill;
	int OnBuddyDead( object_id_type nPlayerId, int32_t nBuddyId, bool bSyncOnly = false );
	int OnBuddyRelive( object_id_type nPlayerId );
public:
// ������ ���ս�� ��ս������Ϣ
	void SendChallengeResToClient(ResultType iResult,uint64_t beChallengeCharID,GameLogic::SOLO_DULE_TYPE iChallengeType);
// about buddy impl
public:
	virtual object_id_type GetOwnerObjectID();
	virtual int GetAliveBuddySize();
	virtual bool CheckOwnerDead();
	void OnOwnerDead(object_id_type nPlayerId, int32_t nCurRespawnTime, int32_t nSafeRespawnTime );
	LC_ServerPlayer* GetOwnerPlayer();
private:
	LC_ServerPlayer* GetOwnerPlayer_Impl();
public:
	object_id_type GetOwnerPlayerId();
	bool SetOwnerPlayerId(object_id_type nObjId);
	virtual void GetBuddyRecords(mem::vector<GameLogic::BuddyInfo>& records);
	virtual int GetActivateBuddyCount();
	int GetUnlockedActivateBuddyCount();
	int IncUnlockedActivateBuddyCount();
	int IncActivateBuddyInSceneCount();
	void InitUnlockedActivateBuddyCount();
	BuddyInstance* GetBuddyInstanceByID(int32_t nID);
	virtual BuddyInstance* GetAssignedBuddyInstance();
	void SyncBuddyAttribute();
	LC_ServerPlayer* GetActivateBuddyByIndex(uint32_t nIndex);
	LC_ServerPlayer* GetActivateBuddyByID(uint32_t nBuddyID);
	void GetAllBuddyPtrPlayer(std::vector<LC_ServerPlayer*>& vecBuddyPtrPlayer);
	void GetAllBuddyInstnace(mem::vector<BuddyInstance*>& vecAllBuddys);
	void GetActivateBuddyIDs(std::vector<int32_t>& vecBuddyIDs);
	void GetActivateBuddyIDs(mem::vector<int32_t>& vecBuddyIDs);
	//������������Ѿ������buddy��ģ��id 
	const mem::map<int32_t, BuddyInstance*>* GetUsableBuddyMap();
	// ��ȡ���е�ǰ��սbuddy��ʵ��ָ��
	// ����ǹ֣����սʵ������ֻ��һ�����������Լ�
	// return: false: ϵͳ���󣬱���serverplayerû�л�ȡ��owner
	// ���������ܳ��֣�������Ҫ����
	virtual bool GetActivateBuddyPtr(LC_ActorPtrList& vecPtrList);
	LC_ServerPlayer* GetCurrentActivateBuddy();
	LC_ServerPlayer* GetNormalCurrentActivateBuddy();
	virtual int32_t GetCurrentActivateBuddyID();
	int32_t GetCurrentActivateBuddyIndex();
	virtual bool IsActivateBuddy(int32_t buddyCID);
	virtual bool IsCurrentActivateBuddy();
	SingleBuddyData* GetUsableBuddyByTemplateID(int32_t nTemplateId);
	bool CheckUnlockedBuddyByTemplateID(int32_t nTemplateId);
	bool CheckActivateBuddyByTemplateID(int32_t nTemplateId);
	bool CheckUsableBuddyByTemplateID(int32_t nTemplateId);
	void SetControlType(PLAYER_CONTROL_TYPE nType);
	PLAYER_CONTROL_TYPE GetControlType();
	int SetBuddyLevel(int lLevel);
	int32_t GetBuddyLevel();
	virtual int32_t GetBuddyID();
	void OnRefreshBuddyLevel(int32_t nBuddyId, int nCurLevel);
	int TryBuddyLevelUp( int32_t nBuddyId );

	void OnRefreshPlayerLevel(bool loginIn);

	void AddHatredToAllBuddy(object_id_type id, int64_t hatred, float time, object_id_type nExcludeId);
	void RemoveHatredToAllBuddy(object_id_type id, object_id_type nExcludeId = 0);
	//bool IsMainController();

	int32_t UpgradeBuddyInstanceStar(int32_t nBuddyID);

	// for interface
	void TrySwitchCurrentBuddy( object_id_type nPlayerId );
	void SetForceLocalRelive( bool bInput );
	bool GetForceLocalRelive();
	bool GetBuddyReliveFlag();
	bool GetOwnerReliveFlag_Recover();
	void SetOwnerReliveFlag_Recover(bool bFlag);
	int32_t SwitchCurrentBuddy();
	int32_t SetCurrentBuddy( uint32_t nIndex, bool bForce = false, bool bInit = false, int nMode = eSwitchBuddyMode_Normal );
	int32_t ReliveBuddy( uint32_t nIndex, bool bFreeMode = false );
	int32_t ReliveAllBuddy( int nReliveType = 0 , bool bAuto = false, bool bForce = false);
	void  Send_SC_BuddyReply(int buddyTid,uint32_t errorCode,int8_t nMode,int32_t cdTime);
	bool ReliveSelf(int nReliveType = 0, bool bAllReliveMode = false, int32_t playerIndex=0);
	int32_t BuyBuddy(int32_t nTemplateId, bool bForce = false);
	int32_t BindBuddyInstance(int32_t nIndex, int32_t nBuddyId, bool bCheckCD = true, int32_t nTargetType = -1);
	int32_t UnBindBuddyInstance(int32_t nIndex, bool bCheckCD = true);
	virtual int32_t TryBuyBuddyByItem(int32_t nItemId, int32_t nBuddyId);

	virtual ResultType ReliveAllBuddyBeforeEnterMap(bool full, bool bAliveFull = false);
	virtual bool GetAutoSafeRelive();
	bool GetInternalSwitchScene();
	void SetReliveBeforeEnterMapTag(bool tag);
	bool GetReliveBeforeEnterMapTag();

	// return value: true: caller continue. false: caller return.
	virtual bool LearnSkillToController( int nSkillId, int& nSlotIndex, bool send=true, int32_t runeSystemType=-1, int32_t runeSystemParam=-1, GameLogic::LC_SkillIDSet* pkReplacedSkills = NULL );
	virtual bool RemoveSkillToController( int nSkillId, int32_t subType=-1);
	virtual ResultType ActiveSkillStateToAllBuddyInstance(int lStateTypeID, int lStateLifeTime, object_id_type lStateAttacherID, float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);
	virtual ResultType ActiveSkillStateToAllBuddyInstance(Skill::SkillStateParam& rParam);
	virtual ResultType DeactiveSkillStateToAllBuddyInstance(int lStateTypeID);
	virtual ResultType DeactiveSkillStateToAllBuddyInstance(int iBuffType, int iKind, int iLevel, int iDeactiveMaxCount);
	bool FlushSkillStateToOneBuddyInstance(int32_t nBuddyId);
	int DeactiveAllSkillState();
	
	void SyncSkillStateToActivateBuddyInstance();

	virtual void    ModifyControllerAttributeBySkill(LC_SkillAssetEntry* pkSkillEntry, int nSkillId, bool add, int runeSystemType=-1, int runeSystemParam=-1);
	

	virtual int GetCitizenship();
	// Ĭ�ϳ�ս��һ��
	int32_t PostEnterMap(uint32_t schemeType=SchemeType_Offensive, uint32_t groupID = 1);

	virtual int32_t GetEntryServerStartTime();
	virtual void NoticeNewAchievementToClient(const mem::set<int32_t>& idSet);

	int32_t PreLeaveMap( int32_t nMapResId, int32_t nDstMapResId );

	bool OnInitBuddyTemplateId();

	int InitBuddyInstanceData(BuddyInstance* pBuddy, int nIndex, bool bCreation);

	void BuddyControllerOperation_Move(MG_Sync_GamePlayerMove& syncMsg, float fCurrentTime);

	void BuddyOperation_Move(MG_Sync_GamePlayerMove& syncMsg, float fCurrentTime, bool bForce = false);

	int ResetBuddy();
	bool AllocateBuddy(int32_t nMapLogicId, bool bControllee = false);
	int DeallocateBuddy();

	void SetCurrentOpBuddy(int32_t nBuddyId = 0);
	int32_t GetCurrentOpBuddy();
	// eSrcType: enum SystemFunctionType [SystemDefine.h]
	// lSrcTypeID: id about detail SrcType, default 0
	virtual bool AddExpWithGamePlay(attr_value_type lExp, int32_t eSrcType, int lSrcTypeID, bool bIgnoreEXPRate = false);
	virtual void AddExpToActivateBuddy( attr_value_type lExp, int32_t eSrcType, int lSrcTypeID = 0, bool bIgnoreEXPRate = false );
	virtual bool AddExpToActivateBuddyByID( int32_t nBuddyID, attr_value_type lExp, int32_t eSrcType, int lSrcTypeID = 0, bool bIgnoreEXPRate = false );
	virtual void AddExpToAllBuddyInstance( attr_value_type lExp, int32_t eSrcType, int lSrcTypeID = 0, bool bIgnoreEXPRate = false );

	virtual int64_t GetDetalExp(int32_t buddyID);

	virtual void NotifyCastSkillToAchievement(int achievementType);

	virtual void NotifyEventToGoal(int goalType, int param);

	virtual void NotifyCastSkillToGatherTaskNpc(int npcCharID);
	void NotifyPlayerLoginIn(int taskID=0);
	void NotifyPlayerVipLevel();
	void NotifyPlayerAddGuild();
	void NotifyTitleLevelToTask();
	void NotifyTotalCashToTask(int cashType);
	void NotifyControllerLevel();

	void NoticeAcceptTask(LC_Task* pkTask);

	int CastSkill( uint32_t nSkillId, float fCurTime, int32_t nItemID, object_id_type nTargetId,  const UT_Vec3Int & rkCur, const UT_Vec3Int & rkLock, const UT_Vec3Int & rkMouse, int16_t nForwardAngle);//, bool bServerCast = false );
	int TryInterruptSkill( uint32_t nSkillId, float fCurTime, int nInterruptType );
	int TryFinishSkill( uint32_t nSkillId, float fCurTime, int32_t nCurX, int32_t nCurY );
	int TrySetDefaultTargetID( object_id_type nTargetID );
	int ForceFinishCurrentSkill();
	float GetLastCastSkillTime();
	void RollNextSkill();
	void ClearNextSkill();
	LC_SkillAssetEntry* GetNextSkill();
public:
	uint64_t GetAllBuddyEquipCombatScore();
	virtual ResultType SetProfilePictureID(int id);
	virtual int32_t GetProfilePictureID();
	virtual void SyncProfilePictureRecord();
	ResultType SetProfilePictureFlag(int id, bool tag=false);
private:
	int32_t m_nProfilePictureID;//ͷ��id
	mem::map<int32_t, bool> m_nProfilePictureRecord;

public:
	virtual ResultType SetCurrentCommanderTitleFrameID(int id);
	virtual int32_t GetCurrentCommanderTitleFrameID();
	bool ActivateCommanderTitleFrameID(int id);
	void SyncCommanderTitleFrameRecord();
	ResultType SetCommanderTitleFlag(int id);
private:
	int32_t m_nCurrentCommanderTitleFrameID;
	mem::map<int32_t, bool> m_nCommanderTitleFrameIDRecord;

public:
	void AddSubSkill(int subType, int skillID);
	void RefreshAllSubSkillScore();
	void RefreshSubSkillScore(int subType);
	void LoadSubSkillInfo(const ::char_data_define::PBSubSkillInfo& data);
	void SaveSubSkillInfo(::char_data_define::PBSubSkillInfo* pkData);
	virtual void LoginLearnSubSkillInfo();
private:
	//���id��Ҫ���ߵ�ʱ������ѧϰ��
	mem::map<int, mem::set<int> > m_nSubSkillID;

public:
	virtual bool GetAttackFlag();
	virtual void SetAttackFlag(bool tag);

	void ForceUpdateDamageData(bool syncClient);
	virtual ResultType ActivityGuildBossMap(int mapID);
	virtual ResultType GetVIPDropReward();
	void CheckAddVIPDropItem();
	void GetVIPDropItem(int dropID, int times, RewardTypeList& item);
	void AddVIPDropItem(RewardTypeList& item);
	void SyncVIPDropItem();
	void FillVIPDropItem(MG_SC_NewVIPBonusCacheSync& msg);
	void FillBonusCache(MG_SC_NewVIPBonusCacheSync& msg);
private:
	int32_t m_nNewVipDropTime;
	mem::map<int, int> m_nVIPDropItemInfo;

public:
	void ResetBestGuild(bool login, int endTime=0);
	void CancelBestGuildTaskDailyTask();

	void SetAssignBestGuildTaskTime(int scheduleID, int32_t endTime);
	int32_t GetAssignBestGuildTaskTime(int scheduleID);

	void SetAssignBestGuildDailyTaskTime(int storyID, uint32_t time);
	uint32_t GetAssignBestGuildDailyTaskTime(int storyID);

	uint32_t GetBestGuildActivityEndTime(int storyID);
	void SetBestGuildActivityEndTime(int storyID, uint32_t time);
	
	
	// 
	// ����ս���˽ӿ�
	void UpdateGuildFightScore(std::map<uint32_t,uint32_t> & updateScore);
	uint32_t GetPersonalGuildTotalScore();
	uint32_t GetGuildPersonalScore(int32_t storyID);
	//

	void CheckProcessingTaskVaild(LC_TaskMap* pkTaskMap);

	void ResetBestGuildTaskAchievementID();
	void ResetBestGuildTaskDailyTask();
	ResultType GetBestGuildReward();
	int32_t GetBestGuildTaskCount();
	void UpdateBestGuildRewardFlag(bool tag);
	bool GetBestGuildRewardFlag();
	void SyncBestGuildTaskInfo();

	//������ǿ��������콱�ĳɾ�id  
	virtual void AddBestGuildAchievementID(const LC_AchievementDataEntry_New* pkAchievement);
	virtual void AddBestGuildTaskID(GameLogic::LC_Task* pkTask);
	//�ɾ��콱ʱ�俪ʼ��������ҵ�½��ˢ�³ɾ���ɽ���
	void FlushBestGuildCounter(int scheduleID=0);
	void CalcuBestGuildCounter(bool reset=false);
	
	void LoadBestGuildAchievementInfo(const char_data_define::PBBestGuildAchievement_Info& data);
	void SaveBestGuildAchievementInfo(char_data_define::PBBestGuildAchievement_Info* pkData);

	int GetBestGuildSettleAccountPoint();
	void SetBestGuildSettleAccountPoint(int score);

	void SetLastAccountPointRankIndexByTypeAndStoryId(int type, int storyID);
	int GetLastAccountPointRankIndexByTypeAndStoryId(int type, int storyID);

	void SetCurrentAccountPointRankIndexByTypeAndStoryId(int type, int storyID);
	int GetCurrentAccountPointRankIndexByTypeAndStoryId(int type, int storyID);
	
private:
	int32_t m_nBestGuildCounter;
	mem::set<int32_t> m_nBestGuildTaskID1;
	mem::set<int32_t> m_nBestGuildTaskID2;
	mem::set<int32_t> m_BestGuildFinishedAchievement1;//�ﵽ�����������콱ʱ�䵽�ˣ���ACHIEVEMENT_BESTGUILD���͵�id
	mem::set<int32_t> m_BestGuildFinishedAchievement2;//�ﵽ�����������콱ʱ��û������ACHIEVEMENT_BESTGUILD���͵�id
	bool m_nBestGuildRewardFlag;

	int32_t m_nSettleAccountPoint;//���ι������������ķ�
	mem::map<uint32_t, uint32_t> m_nGuildFightScore; // key:storyId (0, �����ܷ� 1, ���˱����ܷ�) value:score 

	mem::map<int32_t, uint32_t> m_nBestGuildActivityEndTime;//��ǿ�����Ľ���ʱ��
	mem::map<int32_t, uint32_t> m_nStoryAssignTaskTime;//��ǿ�����ճ�������ɷ�ʱ��
	mem::map<int32_t, int32_t> m_nAssignBestGuildTaskEndTime;//��ǿ����������ɷ�scheduleID�Ľ���ʱ�� 

	mem::map<uint64_t, int32_t> m_nPlayerLastRankIndex;//����ϴ��ڰ��е�����
	mem::map<uint64_t, int32_t> m_nPlayerCurrentRankIndex;//��ұ����ڰ��е�����

	mem::map<uint32_t,SingleFightRecord> m_mapGuildFightRecord;  // key ʱ��� ��value �˳�ս����¼
public:
	virtual void SyncChainFlag();
	virtual uint32_t GetChainFlag();
	virtual ResultType SetChainFlag(int index);

	virtual ResultType SetEquipSuitLevel(int level);
	virtual void SyncEquipSuitLevel();
private:
	uint32_t m_nChainFlag;

	int32_t m_nEquipSuitLevel;

public:
	void UpdateRegularBossGroupInfo();
	void SyncRegularBossGroupInfo();
	ResultType CheckCanEnter(int mapID);
private:
	int32_t m_nRegularBossGroupID;
	uint32_t m_nRegularBossGroupCDTime;

public:
	virtual ResultType GetNewAchievementReward(int32_t id);
private:
	void PreLeaveForRelive(CSVFile::CF_WorldMapList::DataEntry* pData);
	void PostEnterForRelive(CSVFile::CF_WorldMapList::DataEntry* pData);

	virtual void SyncNewAchievementStarNum();
	virtual void SyncNewAchievementReward();

	virtual bool IsInNewAchievementReward(int32_t id);
	virtual void UpdateNewAchievementReward(int32_t id);

	virtual void AddNewAchievementAchievementStarNum(int num);
	virtual int32_t GetNewAchievementStarNum();
private:	
	int32_t m_nNewAchievementStarNum;
	mem::set<int> m_nNewAchievementRewardRecord;
public:
	virtual int64_t GetYaZhiCombatScore();
	virtual uint64_t GetControllerCombatScore();
	uint64_t GetCombatScore();
	virtual uint64_t GetActivateBuddyCombatScore();
	virtual uint64_t GetBuddyAllBaseCombatScore(int buddyID);
	virtual int32_t GetCombatScoreMaxBuddyID(uint64_t& maxCS);

	virtual ResultType ActiveBuddyTransform(int buddyID, int transformID, bool useItem);
	virtual ResultType SetBuddyCurrentTransformID(int buddyID, int transformID);
	virtual int GetCurrentActivateBuddyTransformID();
	virtual int GetBuddyCurrentTransformID(int buddyID);
	virtual void UpdateGroupTransformID();
	virtual ResultType AddBuddyTransformItem(int transformID);
	virtual ResultType AddSubTransformItem(int transformID);
	int32_t GetTransformNum_NotInitID();
public:
	uint64_t GetSumSubSystemCombatScore(int32_t iSubType);
	virtual int32_t GetSubScore_Suit( int32_t nType );
	virtual void SetSubScore_Suit( int32_t nType, int32_t nScore );
	virtual bool ModifySubScore_Suit( int32_t nType, int32_t nDeltaScore );

	void UpdateControllerCombatScoreAndOtherOpen();
public:
	bool CheckCanRespaw(int32_t& cdTime);
	void UpdateControllerPlayerRespawn();
	void ClearCombatStatusAsBuddy();
private:
	struct ControllerPlayerRespawnData
	{
		int m_DeadTime;
		int m_CurRespawnTime;
		time_t m_TimeStamp;
	};
	//ָ�ӹٸ����¼
	ControllerPlayerRespawnData m_ControllerPlayerRespawnData;
public:
	void AddPlaceHolder(int mapResID, mem::vector<object_id_type>& placeHolderLogicID);
	bool GetPlaceHolder(int mapResID, mem::vector<object_id_type>& placeHolderLogicID);
	void RemoveMapPlaceHolder(int mapResID);

	void AddDropItemInfo(int npcID, mem::map<int, int>& dropItems);
	void GetDropItemInfo(int npcID, std::map<int, int>& dropItems);
private:
	mem::map<int, mem::vector<object_id_type> > m_nPlaceHolderLogicID;
	mem::map<int, mem::map<int, int> > m_nDropItems;

public:
	virtual int RemoveErrorProcessingMainTask();
	virtual bool RemoveProcessingTaskByTaskID(int taskID);
	virtual void FinishTask_bug(int index);
private:
	bool m_fixMainTaskBug;
private:
	int CastSkill_Dir( SK_SkillExecute* pkSkill, float fCurTime, int32_t nItemID, object_id_type nTargetId, const UT_Vec3Int & rkLock, int16_t nForwardAngle, bool& bLCSkill);
	int CastSkill_Location( SK_SkillExecute* pkSkill, float fCurTime, int32_t nItemID, const UT_Vec3Int & rkCur, const UT_Vec3Int & rkLock, bool& bLCSkill);
	int CastSkill_Target( SK_SkillExecute* pkSkill, float fCurTime, int32_t nItemID, object_id_type nTargetId, const UT_Vec3Int & rkCur, bool& bLCSkill);

	uint32_t m_nCurrentSkillId;

	int32_t CheckBindBuddyInstance_Logic(BuddyInstance* pBuddy, bool bSwitch);
	int32_t CheckUnBindBuddyInstance_Logic();
	LC_SkillAssetEntry* m_pNextSkillEntry;
public:
	int CheckEnterMap_Task( int32_t nMapResId );
	virtual int GetControllerLevel();
private:	
	int InitBuddyLogicData_ServerPlayer(BuddyInstance* pBuddy, int32_t nBuddyID);
	int InitBuddyLogicData_BasePlayer(BuddyInstance* pBuddy, int32_t nBuddyID);
	int InitBuddyLogicData_BaseActor(BuddyInstance* pBuddy, int32_t nBuddyID);

	void OnRefreshPlayerLevel_UnlockActivateBuddy(bool loginIn);
	void UpdateHpAndSetDeadState();
	int PushBuddyInstance(LC_ServerPlayer* pBuddy, int nIndex, bool bCreation);

	void SyncBuddyData();
	void SyncBuddyData_Usable();
	void SyncBuddyData_Activate();
	void SyncBuddyData_ActivateCount();	

	void InitAIData();//CF_Buddy::DataEntry* pkBuddy);
	void SwitchSMToBuddy();
	void SwitchSMToPlayer();
	int32_t BuyBuddy_Impl(int32_t nTemplateId);
	int32_t TryReduceBuddyMoney(CSVFile::CF_Buddy::DataEntry* pTemplate);
	int32_t TryAddBuddyMoney(CSVFile::CF_Buddy::DataEntry* pTemplate);
	// update
	void UpdateAllBuddyInstance(float fCurrentTime, float fDeltaTime);
	void PostInitAllBuddyInstance();

	void UpdateForBuddyController(float fCurrentTime, float fDeltaTime);
	void UpdateForBuddyController_CheckCDReady(float fCurrentTime, float fDeltaTime);
	void UpdateForBuddyController_CheckUsableCount(float fCurrentTime, float fDeltaTime);
	int32_t GetOtherActivateBuddy( bool bForce = false );

	void TryStopAllBuddyAttack();

	bool CalcAdvanceFollowLocationImpl( LC_ServerPlayer* pBuddyActor, int nRelativeIndex, int nRadius, Utility::UT_Vec3Int& rkResult );
	void SetBuddyLocationImpl(Utility::UT_Vec3Int& pos);
	void OnSkillFlyEnd();

	void _UpdateForBuddyDebug(float fCurrent, float fDeltaTime);

	void _TryRecoverDeadState();
public:
	virtual void SetOpenLevelCheat(bool bFlag);
	virtual bool GetOpenLevelCheat();

	bool CheckBuddyControllerUpdate_Fight(float fCurrent, float fDeltaTime);
	void SetBuddyControllerUpdate_Fight(float fCurrent);

	void SetFollowBaseLocation(const Utility::UT_Vec3Int& rkLocation);
	const Utility::UT_Vec3Int& GetFollowBaseLocation();

	void SetFollowBaseTargetLocation(const Utility::UT_Vec3Int& rkLocation);
	const Utility::UT_Vec3Int& GetFollowBaseTargetLocation();

	void SetFollowLocation(const Utility::UT_Vec3Int& rkLocation, bool bCombat, object_id_type nTargetId);
	const Utility::UT_Vec3Int& GetFollowLocation();
	bool GetFollowMode_Combat();
	object_id_type GetFollowTargetObjectId();

	virtual bool CalcAdvanceFollowLocation( LC_ActorBase* pBuddyActor, int nRadius, Utility::UT_Vec3Int& rkResult );

	void SetBuddyLocation( Utility::UT_Vec3Int& pos );

	bool GetLogicCombatFlag();
	void SetLogicCombatFlag( bool bFlag );

	// ��ȡָ����BuddyIndex��Index�����е�λ��
	// ��������Buddy����λ�÷���֮����߼�
	// ��������buddy�������ж��ڵ�ǰ������������µ�˳λ
	int GetBuddyRelativeIndex( int nBuddyIndex );
	// ��ȡ����ǰ�ľ���������0 1 2���� Q E R
	int GetBuddyAbsoluteIndex();

	void ResetAllBuddyAIParam();

	void SetBuddyAIParam_ForceFollowMode( bool bFlag );
	bool GetBuddyAIParam_ForceFollowMode();

	void SetBuddyAIParam_DebugMode( bool bFlag );
	bool GetBuddyAIParam_DebugMode();

	void SetBuddyAIParam_AttackMode( bool bFlag );
	bool GetBuddyAIParam_AttackMode();

	void SetBuddyAIParam_FollowAttackMode( bool bFlag );
	bool GetBuddyAIParam_FollowAttackMode();


	BUDDY_ACTION_TYPE GetBuddyActionType();
	int SetBuddyActionType(BUDDY_ACTION_TYPE nType);

	void SetBuddyControllerDebugMode(int nIndex, int nMode);
	int GetBuddyControllerDebugMode(int nIndex);

	virtual void SetDenyDead(bool bDenyDead);
	virtual bool GetDenyDead();

	void SetDenyMove(bool bFlag);
	bool GetDenyMove();

	bool EnableGuildFightTowerBuff();
	//QΪ������������
	void ModifyOffsetLocationByIndex(int _nIndex, Utility::UT_Vec3Int& pos, int nFace = 0);
	
	
	
public:
	virtual void SetMouseLocation(const Utility::UT_Vec3Int& rkLocation);
	virtual const Utility::UT_Vec3Int&	GetMouseLocation();
private:
	Utility::UT_Vec3Int m_kMouseLocation;
public:
	void SetRobot();
	bool IsRobot();
private:
	bool m_nIsRobot;
private:
	void UpdateBuddyAction( float fCurrent, float fDelta );
	int EnterBuddyAction( BUDDY_ACTION_TYPE nType );
	int LeaveBuddyAction( BUDDY_ACTION_TYPE nType );
public:
	// ActiveBuddy������ӵ�
	bool ReqBulletShot(MG_REQ_Bullet_Shot & req);

	void SetSoloDulePlay( bool bFlag );
	bool GetSoloDulePlay();

	void ResetSnapshotData();
	void SaveSnapshotData();
	void LoadSnapshotData();
private:
	// Buddy Auto Respawn timer
	float m_fBuddyAutoRespawnTimer[MAX_ACTIVE_BUDDY_NUM];
	float m_fBuddyRespawnTickCount;
private:
	void UpdateBuddyAutoRespawnTimer( float fCurrent, float fDelta );
	int OnAutoRespawnBuddyByIndex( int nIndex );
	void SetBuddyAutoRespawnTimerByIndex(int nIndex);
	void SetBuddyAutoRespawnTimerById(int nId);
	void ResetBuddyAutoRespawnTimer();
	void ResetBuddyAutoRespawnTimerByIndex(int nIndex);
private:
	int m_nBuddyActionType;
public:
	virtual void RefreshLastPVPTime( object_id_type id );
	// Check whether tripping map is allowed by last pvp time.
	bool CheckTripMap_PVPTime();

	bool GetDailyFlag();
	void SetDailyFlag(bool flag);

	void AddAliveBuddyCount();
	void SubAliveBuddyCount();
	void SetAliveBuddyCount(int nAliveCount);
	int32_t GetAliveBuddyCount();

	ResultType SignInByType(int type);
	ResultType ReSignInByType(int type, int day);
	ResultType GetSignInRewardItem(int type, int id);

	int32_t   GetSkyArenaChallengeTimes();
	void	  AddSkyArenaChallengeTimes(int nTimes);
	int32_t   GetSkyArenaRefreshTimes();
	void      AddSkyArenaRefreshTimes(int nTimes);
	int32_t   GetSkyArenaHonorValue();
	void      AddSkyArenaHonorValue(int nValue);
	int32_t GetSkyArenaScoreValue();
	void SetSkyArenaScoreValue(int nValue);
	bool GetSkyArenaGradeAwardState(int nGrade);
	void SetSkyArenaTopScore(int nTopScore);

	int32_t GetSkyArenaTopScore();
		
	void SetSkyArenaMaxScoreValue(int nValue);
	int32_t GetSkyArenaMaxScoreValue();
	void SetSkyArenaGradeAwardState(int nGradeBit);
	void NotifyRankOpenTime();
	void NotifyGroupUserGuildData();
	int32_t GetSkyArenaGradeLimitPoint();
	bool NotifySkyArenaGetHonor();
	int32_t m_nSendOSSLoginTime;
	void SetSummitArenaHonorTimes(std::map<int8_t, int32_t>& nHonorTimes);
	int32_t GetSummitArenaHonorTimesByStageType(int nType);

	void SetActivateSummitArena(int32_t bActivate);
	bool NotifyActivateArena2Client();

	void SetActivateSummitArenaState(bool bActivate);
	bool GetActivateSummitArenaState();
private:
	bool LoadSkyArenaData(const ::char_data_define::PBSkyArenaData& data);
	bool SaveSkyArenaData(::char_data_define::PBSkyArenaData* pkData);

	PlaySkyArenaData  m_SkyArenaData; //���ս��
	bool              b_ActivateSummitArena;//
	bool			  b_NotifySummitArenaState;
	std::map<int8_t, int32_t> m_nSummitArenaHonorMaps;//
public:
	bool SetAtvChargeRedPointByScheduleId(uint32_t nScheduleId);
	bool NotifyAtvChargeRedPoint2Client();
private:
	bool LoadAtvChargeRedPointData(const ::char_data_define::PBAtvChargeRedPointInfo& data);
	bool SaveAtvChargeRedPointData(::char_data_define::PBAtvChargeRedPointInfo* pkData);
	
	std::vector<uint32_t > m_nChargeScheduleIdRedPoints;//ÿ�ճ�ֵ���ۻ���ֵ�������
private:
	float m_fLastPVPTime;
	// Buddy data define section.
	//LC_ServerPlayer* m_pOwnerPlayer;
	object_id_type m_nOwnerPlayerId;
	// Current operation buddy index
	int16_t m_nCurrentBuddyIndex;
	// index as buddy of the current __USER__
	int16_t m_nIndexAsBuddy;
	int32_t m_nUnlockedActivateBuddyCount;
	int32_t m_nAllocatedActivateBuddyCount;
	int32_t m_nActivateBuddyInSceneCount;
	int32_t m_nAliveBuddyCount;

	// ���п����嵥���գ�����δ����
	mem::map<int32_t, SingleBuddyData> m_ppUsableBuddyData;

	// ӵ�е�BuddyInstance������m_nUnlockedActivateBuddyCountӰ��
	mem::map<int32_t, BuddyInstance*> m_ppUsableBuddyInstance;

	// ��ǰ��ս��BuddyId����m_nUnlockedActivateBuddyCountӰ��
	mem::vector<int32_t> m_ppActivateBuddyTemplateId;
	mem::vector<uint32_t> m_ppActivateBuddyObjectId;

	// ��ǰ��ս��Buddy��Playerʵ������m_nUnlockedActivateBuddyCountӰ��
	mem::vector<LC_ServerPlayer*> m_ppActivateBuddyDataPtr;

	PLAYER_CONTROL_TYPE m_nPlayerControlType;
	PlayerAIData m_AIData;
	
	//bool m_bMainController;
	int32_t m_nBuddyTemplateId;
	int32_t m_nCurrentOpBuddyId;
	BuddyInstance* m_pAssignedBuddyInstance;
	bool m_bBuddyRelive;
	bool m_bRelieveBeforeEnterMapTag;
	bool m_bReliveFlag_Recover;
	bool m_bAutoSafeRelive;
	bool m_bForceLocalRelive;
	bool m_bInternalSwitchScene;
	float m_fLastBuddyControllerUpdate_Fight;
	// �����յ�����
	Utility::UT_Vec3Int m_kFollowLocation;
	// �������Ļ�׼������
	Utility::UT_Vec3Int m_kBaseFollowLocation;
	// �������Ļ�׼Ŀ�������
	Utility::UT_Vec3Int m_kBaseTargetFollowLocation;
	// �߼�ս��������ر𳶣�������ս��״̬�£��ҿ������·�����ִ򣬴򲻻�������ڲ��㡣
	bool m_bLogicCombatFlag;
	// �Ƿ���ս��״̬����
	bool m_bFollowMode_Combat;
	object_id_type m_nFollowTargetObjId;
	float m_fRollSkillTimer;
	LC_SkillAsset* m_pDynamicSkillAsset;
	bool m_bBuddyAIParam_ForceFollowMode;
	bool m_bBuddyAIParam_DebugMode;
	bool m_bBuddyAIParam_AttackMode;
	bool m_bBuddyAIParam_FollowAttackMode;
	bool m_bBuddyAIParam_ControllerDebugMode;
	BUDDY_ACTION_TYPE m_eBuddyActionType;
	int32_t m_nLastTargetDistance;
	uint64_t m_nControllerCombatScore;//ָ�ӹٵ�ս����
	bool m_RecomputerControllerScore;//�Ƿ����¼���ָ�ӹ�ս��
// Buddy design
	bool m_bStateMapInitFlag;
	
	bool m_bSoloDulePlay;
	bool m_bRecoverFromSoloPlay;
	bool m_bSwitchLogicMap;
	int32_t m_nOrigMapLogicType;
	PlayerSnapshotData m_Snapshot;
	bool m_bInitGame;
	int32_t m_nTaskHonorPoints;//����������
	int32_t m_nTaskRankIndex;//ָ�ӹ���������
	int32_t m_nGameStoryTimerId;

	int32_t m_nMainTaskFinishCount;//����������������
	mem::set<int32_t> m_nMainTaskReachRewardRecord;//��������������ս��������ȡ��¼

	bool m_nDailyFlag;
	int32_t m_nEnterServerMode;

	CEntrustTask m_EntrustTask;		//ί���������
	CLottery m_Lottery;		//�齱���
	CChargeProfit m_ChargeProfit;		//��ֵ�ۻ����
	mem::map<int,int> m_ThemeStatusMap;	//����ͬ�����

	CCommandEquipPack m_CommandEquipPack;	//ָ�ӹ�װ�����
	CCommanderSubEquipPack m_nCommanderSubEquipPack_Vehicle;//ָ�ӹ�����װ������
	CCommanderSubEquipPack m_nCommanderSubEquipPack_DevaEye;//ָ�ӹ��ȳ�װ������
	CCommanderSubEquipPack m_nCommanderSubEquipPack_Muhun;//ָ�ӹٱ���װ������
	CBuddyEquipData m_BuddyEquipData;//buddyװ������
	CRuneData m_RuneData;//�������
	CRedPoint m_RedPoint;//��������Ҫ�洢�Ŀͻ�������

	LC_SignInAssert m_n360MicroSignInAssert;//360΢��ǩ������	
	CTaskExtraReward m_TaskExtraRewardAsset;//������⽱��
	CRotaryTable m_RotaryTableAsset;//����ת��
	CNoviceTutorial m_NoviceTutorialAsset;//���ֽ̳�
	CPlayerMapInfo m_PlayerMapInfoAsset;//������õģ����ͼ�йص�����
	CPlatformInfo m_PlayerPlatformInfoAsset;//ƽ̨��ص�����
	CIDAsset m_IDAsset;//һЩid��¼
	CNonpareilReturn m_NonpareilReturnAsset;//��Ʒ����
	CQuickRaidAsset m_QuickRaidAsset;//ɨ�����
	CGuessAsset_Summit m_GuessAsset_Summit;//�۷徺��������
	CDropTimes m_DropTimesAsset;//boss�������
	CYuanZhengPoint m_YuanZhengPointAsset;//Զ����
	CDropTimes m_StarShipTimesAsset;//�ǽ������ʴ���
public:
	void SetEnterServerMode( int32_t nValue );
	int32_t GetEnterServerMode();
private:
	// for perf cache
	LC_PKRecorderBase* m_pPKRecorder_Cache;
	LC_TaskMap* m_pTaskMap_Cache;
	LC_ServerPlayer* m_pOwnerPlayer_Cache;
	int32_t m_nActiveServer_Cache;
	bool m_bLogicCombatFlag_Cache;
public:
	int32_t GetScheduleKey();
	void SetScheduleKey( int32_t nTimerId );
	int32_t GetReuseCount();
	void InitDataAsBuddy(uint32_t nBuddyTemplateId, bool bCreation );

	int GetPetSkillDamageByMode(PetDamageMode eMode);
//////////////////////////////////////////////////////////////////////////
// BuddyScheme logic
//////////////////////////////////////////////////////////////////////////
public:
	bool SwitchBuddyScheme(uint32_t mapID, uint32_t groupID, uint32_t schemeType, bool bRelive, bool forceSwitch);
private:
	bool SwitchBuddyScheme_Impl(uint32_t mapID, uint32_t groupID, uint32_t schemeType, bool bRelive, bool forceSwitch);
	// Ĭ�� ��ս��һ��
	void AutoSwitchBuddyScheme(uint32_t schemeType, uint32_t groupID );
//////////////////////////////////////////////////////////////////////////
// BuddyChain logic
//////////////////////////////////////////////////////////////////////////
public:
	// 2.ˢ������Ӣ��ϵͳ������
	void RefreshAllHeroAllSysAttrMap();
	//4.ˢ��Ӣ�����ĳ�ս����
	void RefreshAllHeroAllNewFinalAttrMap(bool login);
	void RefreshAllHeroAllNewFinalAttrMapByTypeValue(int iAttributeType,attr_value_type iValueFunc,bool add = true);
	// ˢ�������������Ӣ���ṩ��ȥ�����������
	void RefreshChainAllBuddyToGiveCoreAttr();
	// ˢ������� ���� �����������������Ӣ��
	void RefreshChainBuddyCoreAttr(bool login, int32_t reasonType=SPART_None);
	// ����������ϵ�Ӣ�� ���Ա仯 ˢ��
	void RefreshChainBuddyCoreAttrByOnChain(int nBuddyId, bool login, int32_t reasonType=SPART_None);
	//��� ��player��ս������{��Ϊ��player��buddy ����������(Ӣ������=��Ӣ�ۻ���+ָ�ӹ���������+ָ�ӹ����������(��Ҫ�ų��Լ����ṩ��������ԣ���ֹ������������)��*��1+���ǹ���Ѫ��ǿ%��*(1+���߼���%+Ƥ���ٷֱ�+ָ�ӹ���װ�İٷֱ� ))}
	virtual LC_ActorAttributeMap* GetNewFinalAttributeMap();
	int32_t BuddyChain_Add( int16_t nIndex, int32_t nBuddyId );
	int32_t BuddyChain_Del( int16_t nIndex);
	int32_t BuddyChain_Switch( int16_t nIndex, int32_t nBuddyId );
	bool BuddyChain_SetIndex( int16_t nIndex );
	//int32_t BuddyChain_GetCount();
	void BuddyChain_SyncToClient();
	void BuddyChain_CalcCount( int32_t nTaskId = INVALID_TASK_ID );
	
	
	LC_ActorAttributeMap* GetShowCoreAttrMap(){return &m_showCoreArrMap;}
private:
	LC_ActorAttributeMap  m_showCoreArrMap;
	bool BuddyChain_SetIndexImpl( int16_t nCount );
	int16_t BuddyChain_RefreshIndexImpl( int16_t nBeginCount, int32_t nTaskId = INVALID_TASK_ID );
	int32_t OnBuddyChainEvent_Add( int32_t nBuddyId, bool bLoad = false );
	int32_t OnBuddyChainEvent_Del( int32_t nBuddyId, bool bLoad = false  );
	int32_t OnBuddyChainEvent_Switch( int32_t nBuddyId, int32_t nOrigBuddyId );
	//int32_t OnBuddyChainEvent_Load();
	int32_t OnBuddyChainEvent_Reload(int32_t nBuddyId);
//////////////////////////////////////////////////////////////////////////
// Bot logic
//////////////////////////////////////////////////////////////////////////
public:
	int ActivateBot( int32_t nResId, bool bForce = false );
	int UpgradeBot( int32_t nResId, bool bForce = false );
	int UpdateBot( float fDelta, float fCurrent );
	int TryFetchBotBonus();
	int FetchBotNormalBonus( int nSeconds, int nType );

	int32_t GetMaxBotBonusTimer();
//////////////////////////////////////////////////////////////////////////
// Pet logic
//////////////////////////////////////////////////////////////////////////
public:
	ResultType OpenPetSystem(int petID);
	int CapturePet( int32_t nResId );
	int EnablePet( int16_t nIndex );
	int DisablePet();
	int DecomposePet(int16_t nIndex);
	int UpgradePetStarLevel(int16_t nIndex, bool bForce = false);
	int SyncPet();
	int32_t GetEnabledPetInstanceResId();
	uint64_t GetTotalPetCombatScore();
	int32_t GetMaxCombatScorePetID(int32_t& petStarLevel);
	const mem::map<int16_t, PetInstance*>& GetPetRawData(int& nResult);
public:
	void CheckResetYiZhongQinXiDamage();
	ResultType GetSingleDamageReward(int mapID, int rewardIndex);
	void SyncSingleDamageReward();
	void UpdateRankTypeDamage(int mapID, int64_t damageData);
	void UpdateHuanjingEndTime(int storyID, int32_t endTime);
	//void CheckResetHuanJingData();
public:
	// CombatRecorder;
	virtual void EnableCombatRecorder(int32_t nMapType);
	virtual void DisableCombatRecorder(int32_t nRankIndex, uint64_t deltaData, uint64_t param, bool syncClient=true);
	bool CheckCombatRecorderEnable();
	void FinishCombatRecorder(int32_t nRankIndex, uint64_t deltaData, uint64_t param, bool syncClient);
	int32_t GetCombatRecorderRank();
	void SetCombatRecorderRank(int32_t nInput);
	bool AddCombatRecorderValue(uint64_t damage);
	const LC_CombatRecorder& GetCombatRecorderData();
	const LC_CombatRecorder& GetMaxCombatRecorderData();
	const LC_CombatRecorder& GetWriteCombatRecorderData();
	void SyncCombatRecorderDataToClient();

	int32_t GetCombatRecorderMapType();

	//����ί������
	void Player_AcceptEntrustTask( int32_t _id, std::vector<int32_t>& _vec );

	//����ί������
	void Player_SpeedUpEntrustTask( int32_t _index );

	//��ȡί�ɽ���
	void Player_GetAwardEntrustTask( std::vector<int32_t>& _vec );

	//ˢ��ί������
	void Player_UpdateEntrustTask();

	//Ѱ���齱
	void LotteryDraw( int32_t _id, int _num );

	//Ѱ����ȡ���⽱��
	void LotteryExtraReward( int32_t _id, int _param );

	//��ȡ��ֵ����
	void ChargeProfitReward( int32_t _id, int _param );

	//ÿ�ճ�ֵ�ۻ�
	void OnCharge( int32_t cashType, int _value );

	//ÿ���ۼ�����
	void OnReduce( int32_t cashType, int _value );

	//����������ʣ������Զ��һ�
	void ThemeAutoExchage( int _id );

	//��������ҵ�½����һ�λ����ʱ���ͬ�����
	void ThemeFlagChange();
	
	//�������ͬ��װ������
	void SyncCommandEquip();

	//װ��ָ�ӹ�װ��
	void EquipCommandEquip( int _index );

	//ָ�ӹ�װ��ǿ��
	void IntensifyCommandEquip( int _index );

	//ָ�ӹ�װ������
	void RefineCommandEquip( int _index );

	//ָ�ӹ�װ������
	void SoulCommandEquip( int _index );

	//�õ��û���װ�����
	CCommandEquipPack& GetCommandEquipPack()
	{
		return m_CommandEquipPack;
	}

	//װ������
	void EquipAdvance( int _type, int _index );
	//���ս������
	int GetSkyArenaGrade(int nGrade, int nScore, int nGradeBits);
	void CheckSkyRankNewSeanson();
	void SkyArenaChallengeResult(bool bSucess, int nGradeId, std::vector<PS_ItemRecord>& nSoloAward, int& nErrorCode);
public:
	//���ûþ��������ʧЧ
	void SetHuanJingInvaild(int32_t id);
	//���ûþ��������״̬
	void ResetHuanJingInvaildID();
	//֪ͨ�ͻ��˻þ��������״̬
	void SyncHuanJingInvaildID();

	CRedPoint& GetRedPointAsset();
private:
	void SyncSingleCombatRecorderDataToClient(int nType, int nDataKey, int nRank, uint64_t deltaData, uint64_t param, const LC_CombatRecorder& recorder);
	void SyncYiZhongQinXiDamageToClient();

	void ReuseHuanJingData();
	void SaveHuanJingData(char_data_define::PBHuanJingData* pkData);
	void LoadHuanJingData(const char_data_define::PBHuanJingData& data);

	mem::map<int32_t, mem::map<int32_t, bool> > m_nDamageRankReward;
	mem::map<int32_t, int32_t> m_nHuanJingStoryEndTime;
	mem::set<int32_t> m_nHuanJingInvaildID;//�þ�����û�к���id

	CommanderAttrMgr m_CommanderAttrMgr;// ָ�ӹ�����

public:
	CommanderAttrMgr& GetCommanderAttrMgr(){return m_CommanderAttrMgr;}
	LC_ServerPlayerGuild* GetPlayerGuildRobInfo(){return &m_GuildRobInfo;}
private:
	LC_ServerPlayerGuild m_GuildRobInfo;
public:
	
	ResultType activateSuitCollection(uint32_t suitid);
	ResultType ReceiveRewardBySuitCollection(uint32_t uStepLevel,uint32_t uType);
	void NotifyCollectEquipInfo();
	LC_CollectEquipRecord* getCollectEquipRecord() {return &m_CollectEquipRecord;}
private:
	LC_CollectEquipRecord m_CollectEquipRecord;


public:
	float GetFinalDamageRate();
	// ���룺��ֱ�
	void ResetFinalDamageRate( float fInput );
	void ResetFinalDamageRateByMap();
	uint64_t GetAllBuddyCoreScoreExceptInputID( int32_t nInputBuddyId );
	bool CheckBuddyChain(int32_t nBuddyId){return m_BuddyChain.Check(nBuddyId);}
private:
	float m_fFinalDamageRate;
	int32_t m_nLastMapResId;
private:
	PetInstanceMgr m_PetInstanceMgr;
	//BotInstanceMgr m_BotInstanceMgr;
	BuddySchemeMgr m_BuddySchemeMgr;//��������
	BuddyChain m_BuddyChain;
	LC_CombatRecorderManager m_CombatRecorderMgr;
	LC_NewVIPReward m_NewVIPReward;
private:
	friend class BuddyInstance;
private:
	// �չ�cd
	struct NormalAttackCD
	{
		uint32_t	kBulletFuncIdxCD[10];				// CD
		uint32_t	kBulletFuncIdx1stShotTime[10];		// GetTickCount����һ�������������ʼʱ��
		uint32_t	kBulletFuncIdxShotCount[10];		// ����ʼʱ�䵽��������Ĵ���
		int32_t		iSkillID;			// �չ�id / 1000
		uint32_t	nCurCheatCount;			// ���״���
		uint32_t    nCurCheatProfitTime;	// ��������ʱ��, ms
		uint32_t	nAccCheatCount;			// ���״���
		uint32_t    nAccCheatProfitTime;	// ��������ʱ��, ms
	};
	NormalAttackCD	m_NormalAttackCD;
};
////////////////////////////////////////////////////////////////////////////
class LC_SimPlayerFilter : public GameLogic::LC_PlayerOperator
{
public:
	LC_SimPlayerFilter(void)
		: m_uiFlag(0)
		, m_uiActorID(INVALID_ACTOR_ID)
		, m_uiGroupID(INVALID_GROUP_ID)
		, m_iGuildID(INVALID_GUILD_ID)
		, m_iNationID(0)
	{}

	inline void SetActor(object_id_type uiActorID, bool bExclude)
	{
		if (bExclude)
		{
			m_uiFlag &= ~(1 << ACT_FLT_FG_ACTOR);
		}
		else
		{
			m_uiFlag |= (1 << ACT_FLT_FG_ACTOR);
		}
		m_uiActorID = uiActorID;
	}

	inline void SetGroup(object_id_type uiGroupID, bool bExclude)
	{
		if (bExclude)
		{
			m_uiFlag &= ~(1 << ACT_FLT_FG_GROUP);
		}
		else
		{
			m_uiFlag |= (1 << ACT_FLT_FG_GROUP);
		}
		m_uiGroupID = uiGroupID;
	}

	inline void SetGuild(uint64_t iGuildID, bool bExclude)
	{
		if (bExclude)
		{
			m_uiFlag &= ~(1 << ACT_FLT_FG_GUILD);
		}
		else
		{
			m_uiFlag |= (1 << ACT_FLT_FG_GUILD);
		}
		m_iGuildID = iGuildID;
	}

	inline void SetNation(int32_t iNationID, bool bExclude)
	{
		if (bExclude)
		{
			m_uiFlag &= ~(1 << ACT_FLT_FG_NATION);
		}
		else
		{
			m_uiFlag |= (1 << ACT_FLT_FG_NATION);
		}
		m_iNationID = iNationID;
	}

	virtual bool operator()(GameLogic::LC_PlayerBase* pkPlayer);
protected:
	uint32_t m_uiFlag;
	object_id_type m_uiActorID;
	object_id_type m_uiGroupID;
	uint64_t m_iGuildID;
	int32_t m_iNationID;
};

class LC_ControllerOnlyPlayerFilter : public GameLogic::LC_PlayerOperator
{
public:
	LC_ControllerOnlyPlayerFilter(void)
	{
	}
	virtual bool operator()(GameLogic::LC_PlayerBase* pkPlayer);
protected:
};

#endif
