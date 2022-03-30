#include "ClientPCH.h"
#include "ClientConfigManager.h"
#include "LC_AutoFitNPCManager.h"
#include "LC_Define.h"
#include "UT_Singleton.h"

#include "LC_CommonStatAsset.h"
#include "LC_OpenAbilityAsset.h"

#include "CF_OpenLevel.h"
#include "CF_TreasureChests.h"
#include "CF_TreasurePool.h"
#include "LC_ClientOpenAbilityManager.h"
#include "LC_ClientShopAsset.h"
#include "LC_ClientShotActivity.h"

#include "io.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_ClientGameEvent.h"
#include "UI_UIManager.h"
#include "CF_CSVSchedule.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "LC_ClientShopCity.h"
#include "LC_ClientActivityManager.h"
#include "CF_ACTOfferAReward.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Utility;

////////////////////////////////////////////////////////////////////////////////////////

static void _onEachHourCB(Utility::timer_id_type iTimerID)
{
	ClientCSVScheduleManager* pkCSVScheduleMgr = (ClientCSVScheduleManager*)ClientCSVScheduleManager::GetSingletonPtr();
	if (pkCSVScheduleMgr)
	{
		TStringVector kCSVNames;
		pkCSVScheduleMgr->GetSchedule(CSV_SCH_TP_HOUR, kCSVNames);

		if (!kCSVNames.empty())
		{
			ClientConfigReloadManager *pkConfigReloadManager = (ClientConfigReloadManager*)ClientConfigReloadManager::GetSingletonPtr();
			if (pkConfigReloadManager)
			{
				pkConfigReloadManager->ConfigReload(kCSVNames);
			}
		}
	}
}
static void _onEachDayCB(Utility::timer_id_type iTimerID)
{
	ClientCSVScheduleManager* pkCSVScheduleMgr = (ClientCSVScheduleManager*)ClientCSVScheduleManager::GetSingletonPtr();
	if (pkCSVScheduleMgr)
	{
		TStringVector kCSVNames;
		pkCSVScheduleMgr->GetSchedule(CSV_SCH_TP_DAILY, kCSVNames);

		ClientConfigReloadManager *pkConfigReloadManager = (ClientConfigReloadManager*)ClientConfigReloadManager::GetSingletonPtr();
		if (pkConfigReloadManager)
		{
			pkConfigReloadManager->ConfigReload(kCSVNames);
		}
	}
}

bool ClientCSVScheduleManager::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	if (GetOpenTime()<=0)
	{
		return false;
	}
	CancelTimer();
	Utility::UT_TimerManager* pkTimerMgr = Utility::UT_TimerManager::GetSingletonPtr();
	if (pkTimerMgr)
	{
		if (!Utility::IsTimerIDValid(m_iHourCron))
		{
			m_iHourCron = pkTimerMgr->SetCron("0 0 * ? ? ?", 60, _onEachHourCB);
		}

		if (!Utility::IsTimerIDValid(m_iDailyCron))
		{
			m_iDailyCron = pkTimerMgr->SetCron("0 0 0 * ? ?", 60, _onEachDayCB);
		}
	}

	CF_CSVSchedule* pkCSVSchedule = CF_CSVSchedule::GetSingletonPtr();
	if (!IS_VALID_RELATED_CSV(pkCSVSchedule, pkCSVs))
	{
		return false;
	}

	Clear();
	CF_CSVSchedule::DataEntryMapExternIterator kIter = pkCSVSchedule->GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_CSVSchedule::DataEntry* pkDataEntry = kIter.PeekNextValuePtr();

		CSVScheduleEntry& rkScheduleEntry = m_kScheduleMap[pkDataEntry->_sCSV];
		rkScheduleEntry.m_iID = pkDataEntry->_iID;
		rkScheduleEntry.m_sCSV = pkDataEntry->_sCSV;
		rkScheduleEntry.m_iScheduleType = pkDataEntry->_iScheduleType;
		rkScheduleEntry.SetSchedules(pkDataEntry->_kSchedule);		

		kIter.MoveNext();
	}
	///////////////////////////////
	/*TStringVector kCSVNames; 
	GetSchedule(CSV_SCH_TP_NA, kCSVNames);

	if (!kCSVNames.empty())
	{
		ClientConfigReloadManager *pkConfigReloadManager = (ClientConfigReloadManager*)ClientConfigReloadManager::GetSingletonPtr();
		if (pkConfigReloadManager)
		{
			pkConfigReloadManager->ConfigReload(kCSVNames);
		}
	}	*/
	return true;
}
void ClientCSVScheduleManager::UnInit(void)
{
	CancelTimer();
	CSVFile::CSVScheduleManager::UnInit();
}

void ClientCSVScheduleManager::CancelTimer()
{
	Utility::UT_TimerManager* pkTimerMgr = Utility::UT_TimerManager::GetSingletonPtr();
	if (pkTimerMgr)
	{
		if (Utility::IsTimerIDValid(m_iHourCron))
		{
			pkTimerMgr->CancelTimer(m_iHourCron);
			m_iHourCron = Utility::TIMER_ID_NA;
		}

		if (Utility::IsTimerIDValid(m_iDailyCron))
		{
			pkTimerMgr->CancelTimer(m_iDailyCron);
			m_iDailyCron = Utility::TIMER_ID_NA;
		}
	}
}

void ClientCSVScheduleManager::OnGameLoaded(uint32_t uiOpenTime)
{
	SetOpenTime(uiOpenTime);
	Init();
}

////////////////////////////////////////////////////////////////////////////////////////
void GuildCreateConfig::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	CF_GuildCreateLimit* inst = SafeGetSingleton<CF_GuildCreateLimit>();
	if(!IS_VALID_RELATED_CSV(inst, pkCSVs))
	{
		return;
	}
	CF_GuildCreateLimit::DataEntryMapExternIterator i = inst->GetIterator();
	while (i.HasMoreElements())
	{
		CF_GuildCreateLimit::DataEntry* entry = i.PeekNextValuePtr();
		//if (entry != NULL)
		{
			m_pCreateLimit = entry;
			break;
		}
		i.MoveNext();
	}
}

void GuildCreateConfig::UnInit(void)
{
	m_pCreateLimit = NULL;
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
template <typename ManagerType, typename RT=void, typename P0=CSVDataLoader*, typename P1=const CSVDataSourceList*>
struct CommonReloadCallBack
{
	typedef RT (ManagerType::*FuncType)(P0, P1);
	static void invoke(P0 p0, P1 p1)
	{
		((ManagerType*)(ManagerType::GetSingletonPtr())->*_func)(p0, p1);
	}
	static FuncType _func;
};

////////////////////////////////////////////////////////////////////////////////////////

static void ShopListCallBackFunc(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	((LC_ClientShopCity*)LC_ClientShopCity::GetSingletonPtr())->Init(pkLoader, pkCSVs);
	((LC_ClientShopManager*)LC_ClientShopManager::GetSingletonPtr())->Init(pkLoader, pkCSVs);
}

static void OpenLevelCallBackFunc(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	((LC_OpenAbilityMgr*)LC_OpenAbilityMgr::GetSingletonPtr())->Init(pkLoader, pkCSVs);
	((LC_ClientOpenAbilityManager*)LC_ClientOpenAbilityManager::GetSingletonPtr())->Init(pkLoader, pkCSVs);
}
/////////////////////////////////////////////////////////////
typedef CommonReloadCallBack<ClientCSVScheduleManager, bool> CRCB_ClientCSVScheduleManager;
template<> CRCB_ClientCSVScheduleManager::FuncType CRCB_ClientCSVScheduleManager::_func = &ClientCSVScheduleManager::Init;

typedef CommonReloadCallBack<LC_CommonStatManager> CRCB_LC_CommonStatManager;
template<> CRCB_LC_CommonStatManager::FuncType CRCB_LC_CommonStatManager::_func = &LC_CommonStatManager::Init;

typedef CommonReloadCallBack<LC_ClientShotActivityManager> CRCB_LC_ClientShotActivityManager;
template<> CRCB_LC_ClientShotActivityManager::FuncType CRCB_LC_ClientShotActivityManager::_func = &LC_ClientShotActivityManager::Init;

typedef CommonReloadCallBack<LC_ClientShopManager, bool> CRCB_LC_CommonShopManager;
template<> CRCB_LC_CommonShopManager::FuncType CRCB_LC_CommonShopManager::_func = &LC_ClientShopManager::Init;

typedef CommonReloadCallBack<LC_OpenAbilityMgr> CRCB_LC_OpenAbilityMgr;
template<> CRCB_LC_OpenAbilityMgr::FuncType CRCB_LC_OpenAbilityMgr::_func = &LC_OpenAbilityMgr::Init;

typedef CommonReloadCallBack<LC_ClientOpenAbilityManager> CRCB_LC_ClientOpenAbilityMgr;
template<> CRCB_LC_ClientOpenAbilityMgr::FuncType CRCB_LC_ClientOpenAbilityMgr::_func = &LC_ClientOpenAbilityManager::Init;

typedef CommonReloadCallBack<GuildCreateConfig> CRCB_GuildCreateConfig;
template<> CRCB_GuildCreateConfig::FuncType CRCB_GuildCreateConfig::_func = &GuildCreateConfig::Init;

typedef CommonReloadCallBack<LC_ClientActivityManager> CRCB_ACT_ClientActivityManager;
template<> CRCB_ACT_ClientActivityManager::FuncType CRCB_ACT_ClientActivityManager::_func = &LC_ClientActivityManager::Init;
////////////////////////////////////////////////////////////////////////////////////////
template <typename CSVType, typename CfgType>
inline void templateSetReloadCB()
{
	CSVType::GetSingletonPtr()->SetReloadCB(&CfgType::invoke);
}

////////////////////////////////////////////////////////////////////////////////////////


void InitReloadCallback(void)
{
	templateSetReloadCB<CF_CSVSchedule, CRCB_ClientCSVScheduleManager>();

	templateSetReloadCB<CF_CommonStatConfig, CRCB_LC_CommonStatManager>();
	templateSetReloadCB<CF_CommonStatReward, CRCB_LC_CommonStatManager>();
	templateSetReloadCB<CF_CommonStatReplevy, CRCB_LC_CommonStatManager>();

	templateSetReloadCB<CF_ShotActivity, CRCB_LC_ClientShotActivityManager>();

	CF_ShopList::GetSingletonPtr()->SetReloadCB(&ShopListCallBackFunc);
	CF_OpenLevel::GetSingletonPtr()->SetReloadCB(&OpenLevelCallBackFunc);
	//templateSetReloadCB<CF_ShopList, CRCB_LC_CommonShopManager>();

	//templateSetReloadCB<CF_OpenLevel, CRCB_LC_OpenAbilityMgr>();

	//templateSetReloadCB<CF_OpenLevel, CRCB_LC_ClientOpenAbilityMgr>();

	templateSetReloadCB<CF_GuildCreateLimit, CRCB_GuildCreateConfig>();

	templateSetReloadCB<CF_ACTSubordinateReward, CRCB_ACT_ClientActivityManager>();
	templateSetReloadCB<CF_ACTShop, CRCB_ACT_ClientActivityManager>();
	templateSetReloadCB<CF_ACTSubordinateRewardA, CRCB_ACT_ClientActivityManager>();
	templateSetReloadCB<CF_ACTShopA, CRCB_ACT_ClientActivityManager>();

	templateSetReloadCB<CF_ActivitySchedule, CRCB_ACT_ClientActivityManager>();
 }

void ClientConfigReloadManager::Init()
{
	{
		ClientCSVScheduleManager* obj = T_NEW_D ClientCSVScheduleManager;
		obj->Init();
	}
	{
		GuildCreateConfig* obj = T_NEW_D GuildCreateConfig;
		obj->Init();
	}

	InitReloadCallback();
	_InitReloadList();
}

void ClientConfigReloadManager::UnInit()
{
	{
		ClientCSVScheduleManager* obj = (ClientCSVScheduleManager*)ClientCSVScheduleManager::GetSingletonPtr();
		obj->UnInit();
		T_SAFE_DELETE(obj);
	}

	{
		GuildCreateConfig* obj = GuildCreateConfig::GetSingletonPtr();
		obj->UnInit();
		T_SAFE_DELETE(obj);
	}
}

bool ClientConfigReloadManager::ConfigReload(const TStringVector& kCSVs, uint32_t uiTimeStamp /*= 0*/)
{
	return _ConfigReload(m_sResVersion, kCSVs, uiTimeStamp);
}

bool ClientConfigReloadManager::_ConfigReload(const StringType& sVersion, const TStringVector& kCSVs, uint32_t uiTimeStamp /*= 0*/)
{
	StringType sVersionPath = Utility::UT_BaseStringUtility::GetVersionPath(sVersion);
	if (!sVersionPath.empty() && access(sVersionPath.c_str(), 0) == -1)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ConfigManagerReload Failed! Version Directory:%s Not Exists!", sVersion.c_str());
		return false;
	}

	if (uiTimeStamp == 0)
	{
		uiTimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	}
	CSVDataLoader kReloadOP(sVersion, uiTimeStamp);

	StringStreamType sStream("");
	bool bReload = false;

	
	TStringVector::const_iterator cit = kCSVs.begin();
	for (; cit!=kCSVs.end(); ++cit)
	{
		const StringType& csvFile = UT_BaseStringUtility::SplitBaseFileName(*cit).first;;
		CSVDataSourceBase* pkCsvFile = CSVDataSourceBase::GetCSV(csvFile);
		if (pkCsvFile)
		{
			bool TmpReload = kReloadOP(pkCsvFile);
			if (TmpReload)
			{
				sStream << csvFile << "#";
			}
			bReload = TmpReload || bReload;
		}
		else
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ReloadCSV Failed! File:%s MD5:%s", csvFile.c_str(), "");
		}
	}
	

	if (bReload)
	{
		kReloadOP.PostReload();

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_RELOAD_CSV_FILE);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	return bReload;
}

void ClientConfigReloadManager::SetResVersion(const StringType &ResVersion, const TStringVector& kCSVs, uint32_t uiTimeStamp /*= 0*/)
{ 
	if (!m_sResVersion.empty() && m_sResVersion.compare(ResVersion) == 0)
	{
		return;
	}
	m_sResVersion = ResVersion;
	m_kReloadFileList.clear();
	m_kReloadFileList.assign(kCSVs.begin(),kCSVs.end());

	MG_MessageCenter::GetSingleton().SendMessage(CreateMsg_ADD_RELOAD_CONFIG_TASK(m_kReloadFileList.empty() ? NULL : &m_kReloadFileList,uiTimeStamp),true);
	//ConfigReloadAll();
}

void ClientConfigReloadManager::ConfigReloadAll(uint32_t uiTimeStamp /*= 0*/)
{
	_InitReloadList();
	_ConfigReload(m_sResVersion, m_kReloadFileList, uiTimeStamp);
}

void ClientConfigReloadManager::_InitReloadList()
{
	m_kReloadFileList.clear();
	m_kReloadFileList.push_back("CSVSchedule.csv");
	m_kReloadFileList.push_back("CommonStatConfig.csv");
	m_kReloadFileList.push_back("CommonStatReplevy.csv");
	m_kReloadFileList.push_back("CommonStatReward.csv");
	m_kReloadFileList.push_back("ShopList.csv");
	m_kReloadFileList.push_back("OpenLevel.csv");
}
