//客户端CSV管理类
#ifndef _CLIENT_CONFIG_MANAGER_H
#define _CLIENT_CONFIG_MANAGER_H

#include "SystemDefine.h"
#include "LC_Define.h"
#include "CSVSchedule.h"
#include "UT_Timer.h"
#include "CF_GuildCreateLimit.h"
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
class ClientCSVScheduleManager : public CSVFile::CSVScheduleManager
{
public:
	ClientCSVScheduleManager(void)
		: CSVFile::CSVScheduleManager()
		, m_iHourCron(Utility::TIMER_ID_NA)
		, m_iDailyCron(Utility::TIMER_ID_NA)
	{}
	virtual ~ClientCSVScheduleManager(void){}

	virtual bool Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	virtual void UnInit(void);
	virtual void CancelTimer();

	void	OnGameLoaded(uint32_t uiOpenTime);
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
class ClientConfigReloadManager : public Memory::MM_BaseObject
	, public Utility::UT_Singleton<ClientConfigReloadManager>
{
public:
	ClientConfigReloadManager(void){}
	~ClientConfigReloadManager(void){}

	void Init();
	void UnInit();

	bool ConfigReload(const TStringVector& kCSVs, uint32_t uiTimeStamp = 0);

	void SetResVersion(const StringType &ResVersion, const TStringVector& kCSVs, uint32_t uiTimeStamp = 0);

	void ConfigReloadAll(uint32_t uiTimeStamp = 0);
private:
	StringType m_sResVersion;

	TStringVector m_kReloadFileList;
	bool _ConfigReload(const StringType& sVersion, const TStringVector& kCSVs, uint32_t uiTimeStamp = 0);

	void _InitReloadList();
};
#endif