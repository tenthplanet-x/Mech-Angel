//服务器端CSV管理类
#ifndef _CONFIG_MANAGER_H
#define _CONFIG_MANAGER_H

#include "SystemDefine.h"
#include "LC_Define.h"

#include "UT_Choice.h"
#include "UT_Timer.h"

#include "CSVSchedule.h"
#include "CF_MapCategory.h"

#include "CF_GuildCreateLimit.h"

class MapRoadmapManager : public Memory::MM_BaseObject,	public Utility::UT_Singleton<MapRoadmapManager>
{
public:
	MapRoadmapManager() {}
	~MapRoadmapManager() {}

	void Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	void UnInit(void);
	CSVFile::CF_MapCategory::DataEntry* GetRoadmap(int map_res_id);
	inline int GetFirstMap() { return m_FirstMap; }
private:
	typedef mem::map<int, CSVFile::CF_MapCategory::DataEntry*> roadmap_type;
	roadmap_type		datas_;
	int					m_FirstMap;
};
////////////////////////////////////////////////////////////////////////////////////////
class VigorConfig : public Memory::MM_BaseObject, public Utility::UT_Singleton<VigorConfig>
{
public:
	void Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	void UnInit(void);
public:
	inline int get_rate(uint32_t hour) { return m_rate_[hour % 24]; }
private:
	int m_rate_[24];
};
////////////////////////////////////////////////////////////////////////////////////////
class ServerCSVSchedule : public CSVFile::CSVScheduleManager
{
public:
	ServerCSVSchedule(void)
		: CSVFile::CSVScheduleManager()
		, m_iHourCron(Utility::TIMER_ID_NA)
		, m_iDailyCron(Utility::TIMER_ID_NA)
	{}
	virtual ~ServerCSVSchedule(void){}

	void OnGameServerLoaded(uint32_t uiOpenTime);

	virtual bool Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	virtual void UnInit(void);

protected:
	Utility::timer_id_type m_iHourCron;
	Utility::timer_id_type m_iDailyCron;
};
////////////////////////////////////////////////////////////////////////////////////////
class GuildCreateConfig: public Memory::MM_BaseObject, public Utility::UT_Singleton<GuildCreateConfig>
{
public:
	GuildCreateConfig():m_pCreateLimit(NULL){}
	~GuildCreateConfig() {}

	void Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	void UnInit(void);

	CSVFile::CF_GuildCreateLimit::DataEntry* getLimit() { return m_pCreateLimit; }
private:
	CSVFile::CF_GuildCreateLimit::DataEntry*	m_pCreateLimit;
};
////////////////////////////////////////////////////////////////////////////////////////
extern void ConfigManagerInit(void);
extern bool ConfigManagerReload(const StringType& sVersion, const TStringVector& kCSVs, bool bForceSync=false);
extern void ConfigManagerUnInit(void);

#endif