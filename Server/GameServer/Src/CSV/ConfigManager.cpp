#include "ConfigManager.h"
#include "LC_AutoFitNPCManager.h"
#include "LC_Define.h"
#include "UT_Singleton.h"
#include "game_version.h"
#include "LC_CommonStatAsset.h"
#include "LC_ShotActivity.h"
#include "LC_ServerShopAsset.h"
#include "LC_TreasureChestsAsset.h"
#include "LC_VIPAsset.h"
#include "LC_ServerItemFactory.h"
#include "LC_OpenAbilityAsset.h"
#include "LC_ServerMysticalShop.h"
#include "LC_ServerChestCSVManager.h"
#include "LC_GameStory_Manager.h"
#include "GameServerApp.h"
#include "LC_MasterBlissManager.h"
#include "LC_HelperDiff.h"
#include "UT_ServerHelper.h"

#include "ACT_SecretTreasure.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "ACT_Lottery.h"

#include "CSV_Server_Helper.h"

#include "CF_OpenLevel.h"
#include "CF_Vigor.h"
#include "CF_MapCategory.h"
#include "CF_Vigor.h"
#include "CF_TreasureChests.h"
#include "CF_TreasurePool.h"
#include "CF_MysticalShop.h"
#include "CF_ItemDisassembleUseFunc.h"
#include "CF_ItemDecompositionFunc.h"
#include "CF_GameStory.h"
#include "CF_CSVSchedule.h"
#include "CF_ActivitySchedule.h"

#include "CF_SecretTreasurePool.h"
#include "CF_SecretTreasureChests.h"
#include "CF_SecretTreasureSchedule.h"
#include "CF_SecretTreasureShop.h"
#include "CF_SecretTreasurePut.h"

#include "CF_ACTSubordinateReward.h"
#include "CF_ACTChargeProfit.h"
#include "CF_ACTChargeInterval.h"
#include "CF_ACTContinuousCharge.h"
#include "CF_ACTConsumeProfit.h"
#include "CF_ACTConsumeRebates.h"
#include "CF_ACTShop.h"
#include "CF_ACTLogin.h"
#include "CF_ACTShot.h"
#include "CF_ACTSignIn.h"
#include "CF_ACTChargeRebates.h"
#include "CF_ACTOfferAReward.h"
#include "CF_ACTSubordinateRewardA.h"
#include "CF_ACTChargeProfitA.h"
#include "CF_ACTChargeIntervalA.h"
#include "CF_ACTContinuousChargeA.h"
#include "CF_ACTConsumeProfitA.h"
#include "CF_ACTConsumeRebatesA.h"
#include "CF_ACTShopA.h"
#include "CF_ACTLoginA.h"
#include "CF_ACTShotA.h"
#include "CF_ACTSignInA.h"
#include "CF_ACTMonopoly.h"
#include "CF_ACTMonopolyA.h"
#include "CF_ACTClimbTower.h"
#include "CF_ACTSnowMan.h"
#include "CF_ACTSpringTree.h"
#include "CF_ACTRefreshShop.h"
#include "CF_ACTSubordinateConsume.h"
#include "CF_ACTLottery.h"
#include "CF_ACTLotteryReward.h"

#include "MG_DebugProtocol.h"   

#ifdef _MSC_VER
#include "io.h"
#else
#include <unistd.h>
#endif //_MSC_VER

using namespace CSVFile;
using namespace GameLogic;
using namespace Utility;
using namespace Activity;

void MapRoadmapManager::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	CF_MapCategory* inst = SafeGetSingleton<CF_MapCategory>();
	if(!IS_VALID_RELATED_CSV(inst, pkCSVs))
	{
		return;
	}
	datas_.clear();
	m_FirstMap = 0;
	int minvalue = 999999999;

	CF_MapCategory::DataEntryMapExternIterator i = inst->GetIterator();
	while (i.HasMoreElements())
	{
		CF_MapCategory::DataEntry* entry = i.PeekNextValuePtr();
		datas_[entry->_lMaprResID] = entry;
		if (entry->_lID < minvalue)
		{
			m_FirstMap = entry->_lMaprResID;
			minvalue = entry->_lID;
		}
		i.MoveNext();
	}
}

void MapRoadmapManager::UnInit()
{

}

CSVFile::CF_MapCategory::DataEntry* MapRoadmapManager::GetRoadmap(int map_res_id)
{
	roadmap_type::iterator it = datas_.find(map_res_id);
	if (it == datas_.end())
	{
		return NULL;
	}
	return it->second;
}
////////////////////////////////////////////////////////////////////////////////////////
void VigorConfig::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	CF_Vigor* inst = SafeGetSingleton<CF_Vigor>();
	if(!IS_VALID_RELATED_CSV(inst, pkCSVs))
	{
		return;
	}
	memset(m_rate_, 0, sizeof(m_rate_));
	CF_Vigor::DataEntryMapExternIterator i = inst->GetIterator();
	while (i.HasMoreElements())
	{
		CF_Vigor::DataEntry* entry = i.PeekNextValuePtr();
		if (entry->_iID > 0 && entry->_iID <= 24)
		{
			m_rate_[(entry->_iID - 1)] = entry->_iVigor;
		}
		i.MoveNext();
	}
}

void VigorConfig::UnInit()
{}

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
static void _onEachHourCB(Utility::timer_id_type iTimerID)
{
	ServerCSVSchedule* pkCSVScheduleMgr = (ServerCSVSchedule*)ServerCSVSchedule::GetSingletonPtr();
	if (pkCSVScheduleMgr)
	{
		TStringVector kCSVNames;
		pkCSVScheduleMgr->GetSchedule(CSV_SCH_TP_HOUR, kCSVNames);

		if (!kCSVNames.empty())
		{
			ConfigManagerReload(g_svr_version_info.custom_res_version(), kCSVNames);
		}
	}
}
static void _onEachDayCB(Utility::timer_id_type iTimerID)
{
	ServerCSVSchedule* pkCSVScheduleMgr = (ServerCSVSchedule*)ServerCSVSchedule::GetSingletonPtr();
	if (pkCSVScheduleMgr)
	{
		TStringVector kCSVNames;
		pkCSVScheduleMgr->GetSchedule(CSV_SCH_TP_DAILY, kCSVNames);

		if (!kCSVNames.empty())
		{
			ConfigManagerReload(g_svr_version_info.custom_res_version(), kCSVNames);
		}
	}
}
void ServerCSVSchedule::OnGameServerLoaded(uint32_t uiOpenTime)
{
	SetOpenTime(uiOpenTime);
	Init();
}
bool ServerCSVSchedule::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ServerCSVSchedule::Init, OpenTime:%u", GetOpenTime());
	if (GetOpenTime()<=0)
	{
		return false;
	}

	Utility::UT_TimerManager* pkTimerMgr = Utility::UT_TimerManager::GetSingletonPtr();
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
	TStringVector kCSVNames; 
	GetSchedule(CSV_SCH_TP_NA, kCSVNames);

	if (!kCSVNames.empty())
	{
		ConfigManagerReload(g_svr_version_info.custom_res_version(), kCSVNames);
	}	
	return true;
}
void ServerCSVSchedule::UnInit(void)
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
	CSVFile::CSVScheduleManager::UnInit();
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
		if (entry != NULL)
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
typedef CommonReloadCallBack<AutoFitNPCManager> CRCB_AutoFitNPCManager;
template<> CRCB_AutoFitNPCManager::FuncType CRCB_AutoFitNPCManager::_func = &AutoFitNPCManager::Init;

typedef CommonReloadCallBack<VigorConfig> CRCB_VigorConfig;
template<> CRCB_VigorConfig::FuncType CRCB_VigorConfig::_func = &VigorConfig::Init;

typedef CommonReloadCallBack<ServerCSVSchedule, bool> CRCB_ServerCSVSchedule;
template<> CRCB_ServerCSVSchedule::FuncType CRCB_ServerCSVSchedule::_func = &ServerCSVSchedule::Init;

typedef CommonReloadCallBack<LC_CommonStatManager> CRCB_LC_CommonStatManager;
template<> CRCB_LC_CommonStatManager::FuncType CRCB_LC_CommonStatManager::_func = &LC_CommonStatManager::Init;

typedef CommonReloadCallBack<LC_ShotActivityManager, ResultType> CRCB_LC_ShotActivityManager;
template<> CRCB_LC_ShotActivityManager::FuncType CRCB_LC_ShotActivityManager::_func = &LC_ShotActivityManager::InitConfig;

typedef CommonReloadCallBack<LC_ServerShopManager, bool> CRCB_LC_CommonShopManager;
template<> CRCB_LC_CommonShopManager::FuncType CRCB_LC_CommonShopManager::_func = &LC_ServerShopManager::Init;

typedef CommonReloadCallBack<LC_TreasureChestsManager> CRCB_LC_TreasureChestsManager;
template<> CRCB_LC_TreasureChestsManager::FuncType CRCB_LC_TreasureChestsManager::_func = &LC_TreasureChestsManager::Init;

typedef CommonReloadCallBack<LC_VIPCmnManager, bool> CRCB_LC_VIPCmnManager;
template<> CRCB_LC_VIPCmnManager::FuncType CRCB_LC_VIPCmnManager::_func = &LC_VIPCmnManager::Init;

typedef CommonReloadCallBack<LC_ServerItemFactory, bool> CRCB_LC_ServerItemFactory;
template<> CRCB_LC_ServerItemFactory::FuncType CRCB_LC_ServerItemFactory::_func = &LC_ServerItemFactory::Init;

typedef CommonReloadCallBack<LC_OpenAbilityMgr> CRCB_LC_OpenAbilityMgr;
template<> CRCB_LC_OpenAbilityMgr::FuncType CRCB_LC_OpenAbilityMgr::_func = &LC_OpenAbilityMgr::Init;

typedef CommonReloadCallBack<ServerMysticalShop> CRCB_ServerMysticalShop;
template<> CRCB_ServerMysticalShop::FuncType CRCB_ServerMysticalShop::_func = &ServerMysticalShop::InitConfig;

typedef CommonReloadCallBack<LC_ServerChestCSVManager> CRCB_LC_ServerChestCSVManager;
template<> CRCB_LC_ServerChestCSVManager::FuncType CRCB_LC_ServerChestCSVManager::_func = &LC_ServerChestCSVManager::Init;

typedef CommonReloadCallBack<GameStory::LC_GameStory_Manager> CRCB_LC_GameStoryCSVManager;
template<> CRCB_LC_GameStoryCSVManager::FuncType CRCB_LC_GameStoryCSVManager::_func = &GameStory::LC_GameStory_Manager::Init;

typedef CommonReloadCallBack<GuildCreateConfig> CRCB_GuildCreateConfig;
template<> CRCB_GuildCreateConfig::FuncType CRCB_GuildCreateConfig::_func = &GuildCreateConfig::Init;

typedef CommonReloadCallBack<ACT_SecretTreasureManager, ResultType> CRCB_ACT_SecretTreasureManager;
template<> CRCB_ACT_SecretTreasureManager::FuncType CRCB_ACT_SecretTreasureManager::_func = &ACT_SecretTreasureManager::InitConfig;

typedef CommonReloadCallBack<ACT_ScheduleDataManager, ResultType> CRCB_ACT_ScheduleDataManager;
template<> CRCB_ACT_ScheduleDataManager::FuncType CRCB_ACT_ScheduleDataManager::_func = &ACT_ScheduleDataManager::InitConfig;

typedef CommonReloadCallBack<ACT_ScheduleManager, bool> CRCB_ACT_ScheduleManager;
template<> CRCB_ACT_ScheduleManager::FuncType CRCB_ACT_ScheduleManager::_func = &ACT_ScheduleManager::Init;
////////////////////////////////////////////////////////////////////////////////////////
template <typename CSVType, typename CfgType>
inline void templateSetReloadCB()
{
	CSVType::GetSingletonPtr()->SetReloadCB(&CfgType::invoke);
}

////////////////////////////////////////////////////////////////////////////////////////

void InitReloadCallback(void)
{
	templateSetReloadCB<CF_MapCategory, CRCB_AutoFitNPCManager>();
	templateSetReloadCB<CF_Vigor, CRCB_VigorConfig>();
	templateSetReloadCB<CF_CSVSchedule, CRCB_ServerCSVSchedule>();

	templateSetReloadCB<CF_CommonStatConfig, CRCB_LC_CommonStatManager>();
	templateSetReloadCB<CF_CommonStatReward, CRCB_LC_CommonStatManager>();
	templateSetReloadCB<CF_CommonStatReplevy, CRCB_LC_CommonStatManager>();

	templateSetReloadCB<CF_ShotActivity, CRCB_LC_ShotActivityManager>();

	templateSetReloadCB<CF_ShopList, CRCB_LC_CommonShopManager>();

	templateSetReloadCB<CF_TreasureChests, CRCB_LC_TreasureChestsManager>();
	templateSetReloadCB<CF_TreasurePool, CRCB_LC_TreasureChestsManager>();

	templateSetReloadCB<CF_VIPFDeposit, CRCB_LC_VIPCmnManager>();
	templateSetReloadCB<CF_VIPProfit, CRCB_LC_VIPCmnManager>();

	templateSetReloadCB<CF_ItemList, CRCB_LC_ServerItemFactory>();

	templateSetReloadCB<CF_OpenLevel, CRCB_LC_OpenAbilityMgr>();

	templateSetReloadCB<CF_MysticalShop, CRCB_ServerMysticalShop>();

	templateSetReloadCB<CF_ItemDisassembleUseFunc, CRCB_LC_ServerChestCSVManager>();
	templateSetReloadCB<CF_ItemDecompositionFunc, CRCB_LC_ServerChestCSVManager>();

	templateSetReloadCB<CF_GameStory, CRCB_LC_GameStoryCSVManager>();

	templateSetReloadCB<CF_GuildCreateLimit, CRCB_GuildCreateConfig>();

	templateSetReloadCB<CF_SecretTreasureSchedule, CRCB_ACT_SecretTreasureManager>();
	templateSetReloadCB<CF_SecretTreasurePool, CRCB_ACT_SecretTreasureManager>();
	templateSetReloadCB<CF_SecretTreasurePut, CRCB_ACT_SecretTreasureManager>();
	templateSetReloadCB<CF_SecretTreasureChests, CRCB_ACT_SecretTreasureManager>();
	templateSetReloadCB<CF_SecretTreasureShop, CRCB_ACT_SecretTreasureManager>();

	templateSetReloadCB<CF_ACTSubordinateReward, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTChargeProfit, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTChargeInterval, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTContinuousCharge, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTConsumeProfit, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTConsumeRebates, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTShop, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTLogin, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTShot, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTSignIn, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTOfferAReward, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTSubordinateRewardA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTChargeProfitA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTChargeIntervalA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTContinuousChargeA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTConsumeProfitA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTConsumeRebatesA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTShopA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTLoginA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTShotA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTSignInA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTChargeRebates, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTMonopoly, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTMonopolyA, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTClimbTower, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTSnowMan, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTSpringTree, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTRefreshShop, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTSubordinateConsume, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTLottery, CRCB_ACT_ScheduleDataManager>();
	templateSetReloadCB<CF_ACTLotteryReward, CRCB_ACT_ScheduleDataManager>();

	templateSetReloadCB<CF_ActivitySchedule, CRCB_ACT_ScheduleManager>();
 }


void ConfigManagerInit(void)
{
	{
		AutoFitNPCManager* obj = T_NEW_D AutoFitNPCManager;
		obj->Init();
	}
	{
		MapRoadmapManager* obj = T_NEW_D MapRoadmapManager;
		obj->Init();
	}
	{
		VigorConfig* obj = T_NEW_D VigorConfig;
		obj->Init();
	}
	{
		ServerCSVSchedule* obj = T_NEW_D ServerCSVSchedule;
		obj->Init();
	}
	{
		LC_MasterBlissManager* obj = T_NEW_D LC_MasterBlissManager;
		obj->Init();
	}

	{
		LC_HelperDiffManager* pkVal = T_NEW_D LC_HelperDiffManager;
		LC_HelperDiff* hd = T_NEW_D LC_ServerHelperDiff;
		pkVal->SetDiff(hd);
	}

	{
		GuildCreateConfig* obj = T_NEW_D GuildCreateConfig;
		obj->Init();
	}
	InitReloadCallback();
}

struct CSVReloadOperator : public CSVDataLoader, public GameStory::GameStoryConfigOperator
{
	CSVReloadOperator(const StringType& ver, uint32_t timestamp)
		:CSVDataLoader(ver, timestamp)
	{
		kNotify.m_uiTimestamp = timestamp;
		TPSTR2STDSTR(sVersion, kNotify.m_strResVersion);
	}
	virtual bool operator ()(CSVDataSourceBase* pkCsvFile) 
	{
		if (CSVDataLoader::operator ()(pkCsvFile))
		{
			Protocol::PS_FileInfo psCSVFile;
			psCSVFile.m_sName = pkCsvFile->GetCSVFileName().c_str();
			psCSVFile.m_sMd5 = pkCsvFile->GetMd5().c_str();
			kNotify.m_kCSVFiles.push_back(psCSVFile);
			return true;
		}
		return false;
	}

	virtual bool operator ()(GameStory::GameStoryConfig* pkConfig)
	{
		GameStory::GameStoryConfigOperator::operator ()(pkConfig);
		if (pkConfig)
		{
			Protocol::PS_FileInfo psCSVFile;
			psCSVFile.m_sName = pkConfig->FileName();
			psCSVFile.m_sMd5 = pkConfig->Md5();
			kNotify.m_kScripts.push_back(psCSVFile);
			return true;
		}
		return false;
	}

	virtual void PostReload(void) 
	{
		CSVDataLoader::PostReload();
		BroadcastMsgToClient(MGPT_DEBUG_RELOAD_NOTIFY, &kNotify);
	}
	Protocol::MG_DebugReloadNotify kNotify;
	CSVScheduleManager* pkCSVScheduleMgr;
};

bool ConfigManagerReload(const StringType& sVersion, const TStringVector& kCSVs, bool bForceSync)
{
	StringType sVersionPath = Utility::UT_BaseStringUtility::GetVersionPath(sVersion);
	if (!sVersionPath.empty() && access(sVersionPath.c_str(), 0) == -1)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ConfigManagerReload Failed! Version Directory:%s Not Exists!", sVersion.c_str());
		return false;
	}
	
	CSVReloadOperator kReloadOP(sVersion, GET_CURRENT_TIMESTAMP_IN_SECONDS());

	bool bReload = false;
	if(kCSVs.empty())
	{
		bReload = Reload_Server_CSV_File(&kReloadOP);
	}
	else
	{
		TStringVector::const_iterator cit = kCSVs.begin();
		for (; cit!=kCSVs.end(); ++cit)
		{
			const StringType& csvFile = *cit;
			CSVDataSourceBase* pkCsvFile = CSVDataSourceBase::GetCSV(csvFile);
			if (pkCsvFile)
			{
				bReload = kReloadOP(pkCsvFile) || bReload;
			}
			else
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ReloadCSV Failed! File:%s MD5:%s", csvFile.c_str(), "");
			}
		}
	}

	if (bReload || bForceSync)
	{
		kReloadOP.PostReload();
	}
	return bReload;
}

void ConfigManagerUnInit(void)
{
	{
		AutoFitNPCManager* obj = AutoFitNPCManager::GetSingletonPtr();
		obj->UnInit();
		T_SAFE_DELETE(obj);
	}
	{
		MapRoadmapManager* obj = MapRoadmapManager::GetSingletonPtr();
		obj->UnInit();
		T_SAFE_DELETE(obj);
	}
	{
		VigorConfig* obj = VigorConfig::GetSingletonPtr();
		obj->UnInit();
		T_SAFE_DELETE(obj);
	}
	{
		ServerCSVSchedule* obj = (ServerCSVSchedule*)ServerCSVSchedule::GetSingletonPtr();
		obj->UnInit();
		T_SAFE_DELETE(obj);
	}

	{
		LC_MasterBlissManager* obj = LC_MasterBlissManager::GetSingletonPtr();
		if ( obj )
			obj->UnInit();
		T_SAFE_DELETE(obj);
	}

	{
		LC_HelperDiffManager* obj = LC_HelperDiffManager::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}

	{
		GuildCreateConfig* obj = GuildCreateConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
}
