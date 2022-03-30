#ifndef _LC_SERVER_NPC_H
#define _LC_SERVER_NPC_H

#include "MC_DataDirtyFlag.h"
#include "LC_NPCBase.h"
#include "LC_ServerNPCOwnerList.h"
#include "GlobalSettings.h"
namespace GameLogic
{
	class LC_SkillTriggerEntry;
}
namespace StateMachine
{
	class SM_StateBase;
}
class LC_ServerNPC : public GameLogic::LC_NPCBase
{
public:
	enum ChaseType
	{
		NOTHING = 0,
		RANDOM = 1,
		SWING = 2,
	};

	//Chase状态下技能间隔乱跑的结构
	struct ChaseData
	{
		bool isSurrounder;
		float startTime;
		float moveTime;
		float swingAngle;
		float swingMaxAngle;
		float swingCurrentAngle;
		int innerRadius;
		int outerRadius;
		ChaseType moveType;
		float fHPRecoverTimer;
		int nHPRecoverRate;
		float fHPRecoverTimer_RT;
		bool bAutoRecover;
		float fSleepProbability;
		float fSleepTimeMin;
		float fSleepTimeMax;
	};
public:
	LC_ServerNPC();
	~LC_ServerNPC();

	//初始化逻辑数据
	virtual void 					InitLogicData();
	virtual void					InitAIData(CSVFile::CF_CharType::DataEntry* pkCharData, CSVFile::CF_MonsterAIFunc::DataEntry* pkAIData);

	//复用内存
	virtual void					ReUse();

	virtual void					Update(float fCurrentTime,float fDeltaTime); 
	virtual bool					NotifyStateEnd(int iEndState,float fCurrentTime,float fDeltaTime);	

	//移动目标位置
	virtual void					SetForwardDir(const Utility::UT_Vec2Float& rkDir);
	virtual	bool					SetTargetLocation(const Utility::UT_Vec3Int& rkTargetLocation);
	virtual void					SetMoveLineTargetLocation(const Utility::UT_Vec3Int& kLocation);
	virtual bool GetDenyDead();

	virtual bool CheckForceUpdate();
	virtual void SaveDamageToRedis();
	virtual void UpdateDamageToRedis();
	virtual void UpdateDamageInfoFromRedis(std::string& str);
	virtual bool UpdateBossCurrentHp();

	virtual void SetDead(bool bIsDead);

	int32_t GetNoticeTime();
	void SetNoticeTime(int32_t time);
	virtual void RecordDamage(LC_ActorBase* pkObject, attr_value_type lPara);
	bool SendNoticeToGuildPlayer(int noticeID);

	int32_t GetRandIndex();
	void UpdateRandIndex(int32_t randIndex);
	void ClearRandIndex();

	Utility::UT_LinePath&	GetMovePath() {return m_kMovePath;}
	void									FindPathTo( const Utility::UT_Vec3Int& rkTarget );
	void									CacheCurrentPath(const Utility::UT_Vec3Int&	rkCacheLocation);
	void									RecoverCurrentPath();
	Utility::UT_Vec3Int		GetCacheLocation(){return m_kCacheLocation;};
	bool									IsCachedMovePath(){return m_bCacheMovePath;};

	//骑乘接口
	virtual void			SetRiderCharType(int lCharType);

	//NPC 可以释放技能，参照Monster的实现
	//通知技能事件
	virtual void					NotifySkillEvent(Skill::SK_SkillEvent* pkSkillEvent);
	//技能执行
	virtual ResultType		CheckPendingSkill(int lSkillTypeID);
	virtual ResultType		CheckExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);	
	void									AIFillExecuteSkillParam(Skill::SK_SkillExecute* pkSkill);
	virtual ResultType		PrepareExecuteSkill(Skill::SK_SkillExecute* pkSkill,int lItemID);
	virtual ResultType		ExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);
	virtual ResultType		InterruptSkill(float fCurrentTime,int lSkillTypeID,SkillInterruptType eInterruptType);
	virtual ResultType		ExecuteSkill(int skillid);

	//战斗激活
	void						SaveActiveInfo();	//怪物被激活，保存相关信息
	void						ResetActiveInfo();	//重置怪物激活信息

	virtual void SetUpdateFlag(bool bUpdateFlag);
private:
	int OnEnableUpdateFlag();
	int OnDisableUpdateFlag();
public:
	//归属
	virtual GameLogic::LC_ActorOwnerListBase* GetNPCOwnerListPtr(void) {return &m_kNPCOwnerList;}
	LC_ServerNPCOwnerList&  GetNPCOwnerList(){return m_kNPCOwnerList;}
	void ClearOwnerList(){m_kNPCOwnerList.Clear();}

	object_id_type GetHitPlayerID();//归属者id
	virtual void SetHitPlayer(object_id_type id);

	object_id_type GetKillPlayerID();//击杀者id  击杀和归属可能不是同一个人
	virtual void SetKillPlayerID(object_id_type id);

	uint32_t GetHitTime();
	virtual void SetHitTime(uint32_t time);

	void				GetDamageList(object_id_set& id);
	void				AddDamagePlayer(object_id_type id);

	void                ConvertEncountToOwnerList();
	object_id_type				  ConvertHitPlayerToOwnerList();

	//位移
	virtual void				PunchMove(SkillMoveActorPositionType Type, const Utility::UT_Vec3Int & kTargetPoint,float fTime);

	void						ModifySMController(const StringType& rkControllerName);

	void						AutoFitLevelReInit(int iLevel);

	virtual void		SetSkillFlag(int64_t llFlag,bool bFlag);
	virtual void		SetAllSkillFlag(int64_t llSkillFlag);

	// CombatEventBonusData
	virtual void RegisterCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE nValue );
	virtual void UnregisterCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey );
	virtual void ModifyCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE nDeltaValue );
	virtual bool GetCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE& nValue );
	virtual COMBAT_EVENT_VALUE_TYPE GetCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey );
private:
	LC_CombatEventData m_CombatEventData;
public:
	float						GetLastPositiveActiveTime(){return m_fLastPositiveActiveTime;};
	void						SetLastPositiveActiveTime(float fTime){m_fLastPositiveActiveTime = fTime;}

	float						GetLastExecuteSkillTime(){return m_fLastExecuteSkillTime;};
	void						SetLastExecuteSkillTime(float fTime){m_fLastExecuteSkillTime = fTime;};

	void						AddSkill(GameLogic::LC_SkillTriggerEntry& data , bool AddSkillNameSlot = true);
	void						AddSkill(int32_t iSkillRuleID, skill_id_type iSkillID);

	void						SetShapeShiftInfo(int lShapeShiftTypeID);

	void						BeginRetreatAccumulate();
	void						StepRetreatAccumulate(float fDeltaTime);
	void						EndRetreatAccumulate();
	float						GetRetreatAccumulate() {	return m_kRetreatTimeAccumulate;}

	void CheckAddGuildMapBossReward(bool force, int64_t oldHp);

public:
	//重置父类方法，以设置消息更新标志
	virtual void		SetMoveSpeed(int fMoveSpeed);
	virtual void		SetWalkSpeedChange();
	virtual void		SetHP(attr_value_type  lHP);
	virtual void		SetMP(attr_value_type  lMP);
	virtual void		SetFactionID(unsigned short usFactionID);

	virtual bool		SetEscortNpcState(int state);

	virtual void		SetCurrentLocation(const Utility::UT_Vec3Int& rkLocation);
	virtual void		SetCurrentLocation(int iX,int iY,int iZ);
	virtual int	GetCurrentLocationWithZ( Utility::UT_Vec3Int& location );

	virtual	void		SetCurrentStateType(int iCurrentState);

	virtual void		SetInteractionValue(int lValue);

	virtual void		SetSummorID(object_id_type lID);
	virtual object_id_type	GetSummorID()	{return m_lSummorID;}
	virtual void		SetSummonOwnerID(object_id_type lID);

	virtual void		UpdateDirtyFlag(int iFlag);
	virtual void		SetDirtyFlag(int iFlag,bool bFlag);
	virtual void		ClearDirtyFlag();
	void						SetFixedOwnerListFlag(bool bFlag);

	virtual void SetLevel(int lLevel, bool bInit = false);

	MC_DataDirtyFlag& GetMessageDirtyFlag();

	float						GetIdleWhenBorn(){return m_IdelWhenBron;}

	void						SetGroupEliteLevel(int iLevel){m_Group_Elite_Level = iLevel;}
	int							GetGroupEliteLevel(){return m_Group_Elite_Level;}

	//*****ChaseData relevant functions	BEGIN
	void						ResetChaseData();
	bool						CheckChaseValid(float currTime);
	void						ResetChaseTime(float currTime = 0, float moveTime = 0);
	void						SetChaseMoveTime(float moveTime);
	float						GetChaseMoveTime() { return m_kChaseData.moveTime; }
	void						SetChaseMoveType(ChaseType moveType);
	ChaseType					GetChaseMoveType() { return m_kChaseData.moveType; }
	void						SetChaseInnerRadius(int distance);
	int							GetChaseInnerRadius() { return m_kChaseData.innerRadius; }
	void						SetChaseOuterRadius(int distance);
	int							GetChaseOuterRadius() { return m_kChaseData.outerRadius; }
	void						SetIsSurrounder(bool isSurrounder);
	bool						GetIsSurrounder() { return m_kChaseData.isSurrounder; }
	void						SetSwingAngle(float swingAngle);
	float						GetSwingAngle() { return m_kChaseData.swingAngle; }
	void						SetSwingMaxAngle(float swingMaxAngle);
	float						GetSwingMaxAngle() { return m_kChaseData.swingMaxAngle; }
	void						SetSwingCurrentAngle(float clockwiseCurrentAngle);
	float						GetSwingCurrentAngle() { return m_kChaseData.swingCurrentAngle; }
	void						SetSleepProbability(float probability);
	float						GetSleepProbability() { return m_kChaseData.fSleepProbability; }
	void						SetSleepTimeMin(float minTime);
	float						GetSleepTimeMin() { return m_kChaseData.fSleepTimeMin; }
	void						SetSleepTimeMax(float maxTime);
	float						GetSleepTimeMax() { return m_kChaseData.fSleepTimeMax; }

	void						SetHPRecoverTimer(float fInput);
	float						GetHPRecoverTimer();
	void						SetHPRecoverRate(int nInput);
	int							GetHPRecoverRate();
	int							UpdateHPRecoverTimer(float fInput);
	void						ResetHPRecoverTimer();

	void SetAutoRecover(bool bFlag);
	bool GetAutoRecover();

	void UpdateForAutoRecover(float fDelta);

	//*****ChaseData relevant functions	END

protected:
	void						_initSMController(const StringType& rkScriptName);
	void						_initStateMap(const StringType& rkScriptName);
	void						_uninitStateMap();
	StateMachine::SM_StateBase*	_findState(int iStateType);	//查表
	void						_updateOwnerByFollowTarget();

	//需要锁定目标
	bool					m_bNeedLockTarget;

	//路径数据
	Utility::UT_LinePath    m_kMovePath;

	int32_t m_nNoticeTime;

	bool					m_bCacheMovePath;
	Utility::UT_Vec3Int		m_kCacheLocation;
	Utility::UT_LinePath	m_kCacheMovePath;

	int32_t					m_nDropRandIndex;
	//归属列表
	LC_ServerNPCOwnerList	m_kNPCOwnerList;

	//首击指挥官的id和首击指挥官该次的攻击时间
	object_id_type			m_kHitPlayerID;//怪物的归属者
	uint32_t				m_kHitTime;

	int32_t					m_nUpdateHpTime;//给社交服通知血量的时间

	//击杀者id
	object_id_type			m_kKillPlayerID;

	object_id_type			m_nEscortNpcState;

	MC_DataDirtyFlag		m_dataChangeFlag;

	float					m_fLastPositiveActiveTime;
	float					m_fForceSyncPositionTime;
	//npc上次释放技能的时间，用于npc释放技能的cd控制
	float					m_fLastExecuteSkillTime;

	float					m_IdelWhenBron;

	int						m_Group_Elite_Level;

	int						m_lSummorID;

	//回撤最长时间间隔
	float					m_kRetreatTimeAccumulate;
	StateMachine::SM_StateBase**		m_kStateMap;	//状态表（所有状态取自StateFactory，只保存引用，不需要delete，ayu）
	int						m_stateCount;

	//Chase状态下技能间隔乱跑的结构
	ChaseData				m_kChaseData;
};

#endif