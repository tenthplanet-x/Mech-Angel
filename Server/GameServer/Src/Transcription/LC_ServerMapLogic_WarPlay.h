#ifndef _LC_SERVERMAPLOGIC_WARPLAY_H
#define _LC_SERVERMAPLOGIC_WARPLAY_H

#include "LC_Server_War_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"
using namespace GameLogic;
using namespace DungeonConfig;

class LC_ServerMapLogic_WarPlay;
struct WarCatapultBullet : public Memory::MM_BaseObject
{
	int		m_Id;
	int		m_Launcher;						//发射器
	int		m_Operator;						//发射者
	CatapultBulletState	m_BullteState;		//导弹状态
	float	m_Time;
	float	m_CruiseTime;
	float	m_WarnTime;
	int		m_MapResId;
	int		m_MapLogicId;
	int		m_BulletLevel;
	int		m_CharType;
	Utility::UT_Vec3Int	m_TargetPos;
	WarCatapultBullet(): m_Id(0), m_Launcher(0), m_Operator(0), m_BullteState(CatapultBulletState_Cruise), m_Time(0.0f), m_CruiseTime(0.0f), 
						m_WarnTime(0.0f), m_MapResId(0), m_MapLogicId(0), m_BulletLevel(0), m_CharType(0), m_TargetPos() {}

	bool	Init(int Id, int launcher, int operater, float cruiseTime, float warnTime, int ResId, int LogicId, const Utility::UT_Vec3Int& pos, int bulletlevel, int chartype);
	void	UnInit();
	void	Update(float fCurrentTime, float fDeltaTime);
	void	CatapultCruise();//巡航
	void	CatapultWarn();//预警
	void	CatapultExplode();//爆炸
	void	CatapultDud();
	inline CatapultBulletState GetState() { return m_BullteState; };
	bool	isEnd();
	bool	isInit();
	int		GetOperator();
};
typedef mem::vector<WarCatapultBullet* > WarCatapultBulletVector;
/////////////////////////////////////////////////////////////////////////////////////
struct WarCatapult : public  Memory::MM_BaseObject
{
	int		m_Id;//发射器
	Utility::UT_Vec3Int m_Pos;
	int		m_Radius;
	float	m_CruiseTime;
	float	m_WarnTime;
	int		m_Angel;
	float	m_CDTime;
	float	m_CDTimeStamp;
	int		m_MapResId;
	int		m_MapLogicId;
	int		m_Operator;
	int		m_VaildDis;
	int		m_IdBegin;
	WarCatapultBulletVector m_CatapultBullet;
	LC_ServerMapLogic_WarPlay* m_Parent;

	WarCatapult(): m_Id(0), m_Pos(), m_Radius(0), m_CruiseTime(0.0f), m_WarnTime(0.0f), m_Angel(0), m_CDTime(0.0f), m_CDTimeStamp(0.0f), m_MapResId(0),
					m_MapLogicId(0), m_Operator(INVALID_ACTOR_ID), m_VaildDis(0), m_IdBegin(0), m_CatapultBullet(), m_Parent(NULL) {}

	bool	Init(LC_ServerMapLogic_WarPlay* parent, int chartype, const Utility::UT_Vec3Int& pos, int radius, float cruiseTime, float warnTime, int angel, int ResId, int LogicId, int vaildDis);
	void	UnInit();
	void	Update(float fCurrentTime, float fDeltaTime);
	WarCatapultBullet* GetBullte(int id);
	ResultType	Fire(LC_ServerPlayer* player, Utility::UT_Vec3Int& TargetPos, int bulletlevel, int& iCost);
	int		GetControlPlayerBullteCount();
	bool	FireResult(LC_ServerPlayer* player, ResultType result, int bullteid);
	ResultType __CheckControl(object_id_type playerid);
	int		CheckCanControl(LC_ServerPlayer* player);
	ResultType Control(LC_ServerPlayer* player);
	void	ClearBullet();
	void	ClearControl(bool clearbullet = true);
	void	SetControl(LC_ServerPlayer* player);
};
/////////////////////////////////////////////////////////////////////////////////////
class LC_ServerGuildInfo;
class LC_ServerMapLogic_WarPlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME(LC_ServerMapLogic_WarPlay)
	typedef mem::map<object_id_type, mem::map<object_id_type, int> > KillMap;
	typedef mem::map<int, int> LogicAreaMap;
	typedef mem::map<int, WarCatapult* > CatapultMap;
public:
	LC_ServerMapLogic_WarPlay();
	virtual ~LC_ServerMapLogic_WarPlay();
	bool Init(LC_ServerMapLogic_Place* place, War_Config* config);
	virtual void UnInit();
	virtual bool Start();
	virtual void End();
	virtual bool Update(float fCurrentTime, float fDeltaTime);
	virtual Utility::UT_Vec3Int	ComputeEnterLocation(LC_ServerPlayer* player, const Utility::UT_Vec3Int& targetlocation);
	virtual void EventNotify(MapLogicEvent& kEvent);
	virtual void PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent);
	virtual bool CheckEnd();
	virtual bool CheckRun();
	void Settle();

	bool UpdateOwnerless(float fCurrentTime, float fDeltaTime);
	void OwnerlessFinish();

	bool UpdateRune(float fCurrentTime, float fDeltaTime);
	void GenRune();

	void PlayerEnterWar(LC_ServerPlayer* player);
	void PlayerLeaveWar(LC_ServerPlayer* player);

	ResultType CheckPowerOwnerless();
	bool isinPowerGuild(LC_ServerPlayer* player);
	bool isinGuild(LC_ServerPlayer* player, uint64_t guildid);
	bool isPowerGuild(int guildid);
	bool isChallengeGuild(int guildid);

	WarCatapult* GetCatapult(int catapultId);
	void AddCatapultControlIndex(int Catapultid, object_id_type playerid);
	void ClearCatapultControlIndex(object_id_type playerid);
	void ClearAllCatapultControl();
	WarCatapult* GetPlayerControlCatapult(object_id_type playerid);

	ResultType CheckGivenPower(GameLogic::LC_PlayerBase* pkPlayer, bool checkmoney = true);
	ResultType PreGivenPower(GameLogic::LC_PlayerBase* PowerPlayer);
	ResultType GivenPower(GameLogic::LC_PlayerBase* PowerPlayer, ResultType result, uint64_t guildid, int64_t transid);
	void GuildCatapultCostResult(LC_ServerPlayer* player, ResultType result, uint64_t guildid, int64_t transid = 0, int p1 = 0, int p2 = 0, int p3 = 0);

	inline void SetWarType(WarType wartype) { m_WarType = wartype; }
	inline WarType GetWarType() { return m_WarType; }
	inline WarStateType GetState() { return m_State;}
	void SetState(WarStateType state);

	uint64_t GetPowerGuild();
	int GetPowerPlayer();
	LC_ServerGuildInfo* GetPlayerGuild(LC_ServerPlayer* player);
	LC_ServerGuildInfo* GetGuild(uint64_t guildid);

	void SyncPowerPlayerInfo(bool force = false, object_id_type playerid = INVALID_ACTOR_ID);
	void SyncPowerGuildInfo(object_id_type playerid = INVALID_ACTOR_ID);
	void SyncAllEnemyPlayerInfo(object_id_type playerid = INVALID_ACTOR_ID);

	void FillGuildWarInfo(Protocol::MG_WarInfo& guildwarinfo);
	void FillGuildWarPlayerInfo(Protocol::MG_WarPlayerInfo& guildwarplayerinfo);
private:
	void __setPowerNPCState(bool sleep);
	void __setPowerGuild(uint64_t guildid, object_id_type playerid, bool notify = true);

private:
	War_Config* m_Config;
	WarType m_WarType;
	WarStateType m_State;

	uint64_t m_PowerTransId;//盟主传送点的行会

	uint64_t m_PowerGuild;//执令方
	object_id_type m_PowerPlayer;//执令人
	float m_PowerTimeStamp;//执令时间

	//火令台
	int m_PowerNpcId;
	//人物身上BUFF
	LogicAreaMap m_LogicArea;

	//符文3
	RuneState m_RuneState;
	float m_RuneTime;

	//攻城车
	CatapultMap m_CatapultMap;
	mem::map<int, int> m_CatapultPlayerMap;
	int m_CatapultIdBegin;

	KillMap m_KillMap;

	//副本同步消息时间戳
	float m_SyncTimeStamp;
	float m_SyncEmenyTimeStamp;
};
#endif