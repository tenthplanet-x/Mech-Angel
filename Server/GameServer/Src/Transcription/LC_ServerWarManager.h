#pragma once

#include "LC_Server_War_Config.h"
#include "MG_Game2Guild.h"
#include "LC_ServerMapLogic_Place.h"
#include "UT_Cron.h"

using namespace DungeonConfig;
/////////////////////////////////////////////////////////////////////////////////
class LC_GuildWarCron
{
public:
	enum E_GuildWarCronTime_Type
	{
		E_GuildWarCronTime_Open = 0,	//开服
		E_GuildWarCronTime_Merge,		//合服
		E_GuildWarCronTime_Common,		//普通
	};
public:
	LC_GuildWarCron(): m_nCronIndex(0) {}
	~LC_GuildWarCron() {}

	time_t	getNextTime(int32_t iWarType, time_t nCurStamp);
	time_t	getPrevTime(int32_t iWarType, time_t nCurStamp);
	void	Init(int nType);
	void	fillNextTimeList(int32_t iWarType, uint32_t& nCurTime, uint32_t& nBaseTime, int nType, std::vector<uint32_t>& rList);
private:
	int		m_nCronIndex;
};
/////////////////////////////////////////////////////////////////////////////////
enum GuildWarState
{
	GuildWarState_None,
	GuildWarState_Peace,
	GuildWarState_War_Ownerless,
	GuildWarState_War_Owner,
};

struct GuildWarMap
{
	int m_MapLogicId;
	int m_MaxPlayer;
	int m_MapResid;
	bool m_Vaild;

	GuildWarMap() { Clear(); }
	void Clear()
	{
		m_MapLogicId = 0;
		m_MaxPlayer = 0;
		m_MapResid = 0;
		m_Vaild = false;
	}
};
/////////////////////////////////////////////////////////////////////////////////
class GuildWarUnit
{
public:
	GuildWarUnit();
	~GuildWarUnit();

	virtual void Clear();
	virtual bool Init(int nWarType, const StringType& rPath);
	virtual void UnInit();

	virtual void PrepareNearWar();
	virtual void PrepareCurPower();
	virtual bool IsWarFighting() { return m_WarState == GuildWarState_War_Ownerless || m_WarState == GuildWarState_War_Owner; }

	virtual void Update(float fCurrentTime, float fDeltaTime) {}
	virtual void SetNotice(time_t span, time_t curtimestamp);
	virtual void GetWarTimeStamp(time_t& beginstamp, time_t& endstamp);

	virtual void StartWar(time_t startTimestamp) {}
	virtual void EndWar(){}
	virtual void ForceEndWar(){}
	virtual void Order(int delaytime) {}

	virtual bool CharEnter(LC_ServerPlayer* player) { return true; }
	virtual ResultType EnterGuildWar(LC_ServerPlayer* player);
	virtual LC_ServerMapLogic_Place* GetMapLogic(int logicid) { return NULL; }
	virtual void NotifyPowerEnd(uint64_t power, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId) {}

	virtual void BroadCast(const StringType& notifystr, bool allserver = true, int logicid = 0);
	virtual void BroadCastGroup(const StringType& notifystr, bool allserver = true, int logicid = 0);

	virtual int32_t GetWarActivityID(uint64_t nTime = 0 );
	GuildWarManager_Config& getConfig() { return m_Config; }
protected:
	GuildWarManager_Config	m_Config;
	GuildWarState m_WarState;
	size_t	m_NoticeIndex;
	time_t	m_TimeStamp;
	GuildWarMap	m_CurMap;
	LC_GuildWarCron	m_WarCron;
	int		m_WarType;
};

/////////////////////////////////////////////////////////////////////////////////
class LC_ServerGuildInfo;
class GuildWarLocal: public GuildWarUnit
{
public:
	GuildWarLocal();
	~GuildWarLocal();

	virtual bool Init(int nWarType, const StringType& rPath);
	virtual void Update(float fCurrentTime, float fDeltaTime);
	virtual void StartWar(time_t startTimestamp);
	virtual void Order(int delaytime);
	virtual bool CharEnter(LC_ServerPlayer* player);
	virtual ResultType EnterGuildWar(LC_ServerPlayer* player);
	virtual LC_ServerMapLogic_Place* GetMapLogic(int logicid);
	virtual void PrepareNearWar();
	virtual void NotifyPowerEnd(uint64_t power, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId);

	virtual void EndWar();
	virtual void ForceEndWar();

	void OnWarBegin();
	void OnWarEnd(LC_ServerGuildInfo* pGuild, time_t timeStamp);
};

class GuildWarCross: public GuildWarUnit
{
public:
	GuildWarCross();
	~GuildWarCross();

	virtual void Clear();
	virtual bool Init(int nWarType, const StringType& rPath);
	virtual void Update(float fCurrentTime, float fDeltaTime);
	virtual bool CharEnter(LC_ServerPlayer* player);
	virtual LC_ServerMapLogic_Place* GetMapLogic(int logicid);
	virtual void NotifyPowerEnd(uint64_t power, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId);

	virtual void EndWar();
	virtual void Order(int delaytime);
	virtual void ForceEndWar();
	virtual void GetWarTimeStamp(time_t& beginstamp, time_t& endstamp);

	void StartWar(time_t starttime, time_t endtime);
	void EndWar(time_t starttime);
	void Notice(int noticeindex);
	void SetWarTime(time_t starttime, time_t endtime, int32_t nState);
	void SocietyShutDown();

	void ReportWarParm();
	void WarPowerNotify(Protocol::MG_SS_CrossRealmGuildWar_PowerNotify& info);
	void WarPlaceStart(time_t starttime, time_t endtime, int groupid, int powerguildid, int64_t nRaidId);
	void WarPlaceEnd(time_t starttime, bool force, bool ignore);
	ResultType EnterGuildWar(LC_ServerPlayer* player);
	ResultType AppointEnterGuildWar(object_id_type playerID, user_id_type userid);
private:
	time_t	m_tStartTimeStamp;
	time_t	m_tEndTimeStamp;
	bool	m_bIgnoreResult;
};

class GuildWarDomain: public GuildWarUnit
{
public:
	GuildWarDomain();
	~GuildWarDomain();

	virtual void Clear();
	virtual bool Init(int nWarType, const StringType& rPath);
	virtual void Update(float fCurrentTime, float fDeltaTime);
	virtual void StartWar(time_t startTimestamp);
	virtual void Order(int delaytime);
	virtual bool CharEnter(LC_ServerPlayer* player);
	virtual LC_ServerMapLogic_Place* GetMapLogic(int logicid);
	virtual void PrepareNearWar();
	virtual void NotifyPowerEnd(uint64_t power, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId);

	virtual void EndWar();
private:
	LC_ServerMapLogicPlaceVector m_DomainMap;
};
/////////////////////////////////////////////////////////////////////////////////
class LC_ServerGuildInfo;
class LC_ServerMapLogic_Place;
class LC_WarManager : public  Memory::MM_BaseObject
{
public:
	LC_WarManager();
	virtual ~LC_WarManager();
	DECL_RTTI_NAME(LC_WarManager)

	void Clear();
	bool Init();
	void UnInit();

	bool IsWarFighting();
	void CharEnter(LC_ServerPlayer* player);
	void Update(float fCurrentTime, float fDeltaTime);
	LC_ServerMapLogic_Place* GetMapLogic(WarType wartype, int logicid);
	void GetWarTimeStamp(WarType wartype, time_t& beginstamp, time_t& endstamp);
	void NotifyPowerEnd(uint64_t power, WarType wartype, int logicid, const unique_id_vector& idList, const unique_id_impl& uiPowerId);
	void NotifyGivenPower(uint64_t power, WarType wartype, int logicid, const unique_id_impl& uiPowerId);

	//本地公会战
	void PrepareLocalCurPower() { m_LocalWarUnit.PrepareCurPower(); }
	void OrderLocal(int delaytime) { m_LocalWarUnit.Order(delaytime); }
	ResultType EnterLocalGuildWar(LC_ServerPlayer* player) { return m_LocalWarUnit.EnterGuildWar(player); }
	void ForceEndLocalWar() { m_LocalWarUnit.ForceEndWar(); }
	inline int32_t GetLoaclWarActivityID(uint64_t nTime = 0) { return m_LocalWarUnit.GetWarActivityID(nTime); }

	//跨服公会战
	void ReportCrossRealmGuildWarInfo() { m_CrossWarUnit.ReportWarParm(); }
	void CrossRealmGuildWarPowerNotify(Protocol::MG_SS_CrossRealmGuildWar_PowerNotify& info) { m_CrossWarUnit.WarPowerNotify(info); }
	void CrossRealmGuildWarPlaceStart(time_t starttime, time_t endtime, int groupid, int powerguildid, int64_t nRaidId) { m_CrossWarUnit.WarPlaceStart(starttime, endtime, groupid, powerguildid, nRaidId); }
	void CrossRealmGuildWarPlaceEnd(time_t starttime, bool force, bool ignore) { m_CrossWarUnit.WarPlaceEnd(starttime, force, ignore); }
	ResultType EnterCrossRealmGuildWar(LC_ServerPlayer* player) { return m_CrossWarUnit.EnterGuildWar(player); }
	ResultType AppointEnterCrossRealmGuildWar(object_id_type playerID, user_id_type userid) { return m_CrossWarUnit.AppointEnterGuildWar(playerID, userid); }
	void SetCrossRealmGuildWarTime(time_t starttime, time_t endtime, int32_t nState) { return m_CrossWarUnit.SetWarTime(starttime, endtime, nState); }
	void CrossRealmGuildWarNotice(int noticeindex) { return m_CrossWarUnit.Notice(noticeindex); }
	void CrossRealmGuildWarGuildStart(time_t starttime, time_t endtime) { m_CrossWarUnit.StartWar(starttime, endtime); }
	void CrossRealmGuildWarGuildEnd(time_t starttime) { m_CrossWarUnit.EndWar(starttime); }
	void SocietyServerShutDown() { m_CrossWarUnit.SocietyShutDown(); }
	void OrderCrossRealm(int delaytime) { m_CrossWarUnit.Order(delaytime); }
	void ForceEndCrossRealmWar() { m_CrossWarUnit.ForceEndWar(); }

	//势力争夺战
	void PrepareDomainCurPower() { m_DomainUnit.PrepareCurPower(); }
	void StartDomainWar(time_t startTimestamp) { m_DomainUnit.StartWar(startTimestamp); }
	void EndDomainWar() { m_DomainUnit.EndWar(); }
	void OrderDomain(int delaytime) { m_DomainUnit.Order(delaytime); }
private:
	GuildWarLocal m_LocalWarUnit;			//圣火令
	GuildWarCross m_CrossWarUnit;			//屠龙刀
	GuildWarDomain m_DomainUnit;			//势力争夺战
	Utility::UT_Timer m_kUpdateTimer;
};
