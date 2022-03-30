#include "BotInstanceMgr.h"
#include "IdDefine.h"
#include "BotInstance.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayer.h"
#include "LC_PackEntryFactory.h"
#include "CF_Bot.h"
#include "CF_BotUpgrade.h"
#include "MG_BotProtocol.h"
#include "../Buddy/BuddyHelper.h"
#include "CF_ItemGenMonsterDrop.h"
#include <GlobalSettings.h>

#define			BOT_SYSTEM_OPEN_LEVEL_KEY			22

BotInstanceMgr::BotInstanceMgr()
	: m_nOwnerPlayerId(INVALID_ACTOR_ID)
{
	ReUse();
}

BotInstanceMgr::~BotInstanceMgr()
{

}

void BotInstanceMgr::Init(object_id_type nOwnerPlayerId)
{
	m_nOwnerPlayerId = nOwnerPlayerId;
}

void BotInstanceMgr::Clear()
{

}

void BotInstanceMgr::InitNormalBonusTime( uint32_t nTime, bool bForce )
{
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		if ( m_nTimer_Normal[ i ] == 0 || bForce )
			m_nTimer_Normal[ i ] = nTime;
	}
}

void BotInstanceMgr::TryRecoverActivate()
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
	{
		return;
	}

	int nOwnerLevel = pOwnerPlayer->GetLevel();
	CF_OpenLevel::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_OpenLevel>(BOT_SYSTEM_OPEN_LEVEL_KEY);
	if ( pkData == NULL )
		return;
	if ( pkData->_CharLevel >= nOwnerLevel )
	{
		Activate();
	}
}

int BotInstanceMgr::ActivateDefaultBot()
{
	CF_Bot::DataEntryMapExternIterator kIter = CF_Bot::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_Bot::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_bDefaultActivate)
		{
			ActivateBot( pkEntry->_cID, true );
		}
		kIter.MoveNext();
	}
	return true;
}

int BotInstanceMgr::Activate()
{
	if ( GetCount() > 0 )
		return false;

	ActivateDefaultBot();

	return true;
}

bool BotInstanceMgr::IsActivated()
{
	return (GetCount() > 0);
}

int BotInstanceMgr::ActivateBot(int32_t nBotResId, bool bForce)
{
	int nResult = false;
	MG_SC_BotActivateResult result;
	result.m_nRetCode = RE_SUCCESS;
	Utility::UT_SIMDataList kItemReq;
	Utility::UT_SIMDataList::iterator it;

	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
	{
		return false;
	}

	int nOrigCount = GetCount();
	BotInstance* pInstance = GetBotInstance(nBotResId);
	if ( pInstance )
	{
		result.m_nRetCode = RE_BOT_ALREADY_ACTIVATE ;
		goto Exit0;
	}
	CF_Bot* pConfig = CF_Bot::GetSingletonPtr();
	if (pConfig == NULL)
	{
		goto Exit0;
	}

	CF_Bot::DataEntry* pData = pConfig->GetEntryPtr(nBotResId);
	if (pData == NULL)
	{
		goto Exit0;
	}
	if (bForce == false)
	{
		Utility::ParseFmtString2DataIFs(pData->_sConsumption, kItemReq);
		it = kItemReq.begin();
		// Item Checking..
		for (; it != kItemReq.end(); ++it)
		{
			UT_SIMDataInfo info = *it;
			int32_t nItemId = info.GetSkillID();
			int32_t nItemCount = info.GetSkillEnableLevel();
			int nFoundCount = pOwnerPlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_BASIC, nItemId);
			if ( nFoundCount < nItemCount )
			{
				result.m_nRetCode = RE_BOT_ACTIVATE_RESOURCE;
				goto Exit0;
			}
		}
	}

	pInstance = AddBot_Impl(pData);
	if (pInstance == NULL)
	{
		result.m_nRetCode = RE_BOT_ACTIVATE_ERROR;
		goto Exit0;
	}

	if (m_nBeginTime == 0)
	{
		uint32_t currentTime = time(NULL);
		Enable(currentTime);

		LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
		if(NULL != pkOwnerPlayer)
		{
			pkOwnerPlayer->EnableNewVipBot(currentTime);
		}
	}

	result.m_Data.m_nResId = pInstance->GetTemplateId();
	result.m_Data.m_nLevel = pInstance->GetLevel();
	result.m_Data.m_nTimeStamp = pInstance->GetTimeStamp();
	//if (pOwnerPlayer->GetLotteryStat())
	//	result.m_nMode = ProductMode::eProductMode_Lottery;
	//else
	//	result.m_nMode = ProductMode::eProductMode_Normal;
	if (bForce == false)
	{
		// Item Reducing
		it = kItemReq.begin();
		for (; it != kItemReq.end(); ++it)
		{
			UT_SIMDataInfo info = *it;
			int32_t nItemId = info.GetSkillID();
			int32_t nItemCount = info.GetSkillEnableLevel();
			pOwnerPlayer->DeleteItem(nItemId, nItemCount);
		}
	}

	AchievementEvent kBotEvent;
	kBotEvent.SetConditionInfo(pInstance->GetLevel());
	kBotEvent.SetNum(1);
	pOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BOT, kBotEvent);

	nResult = true;
Exit0:
	pOwnerPlayer->SendMsgToClient(MGPT_SC_BOT_ACTIVATE_RESULT, &result);
	if (nResult && nOrigCount == 0)
	{
		MG_SC_BotDataSync_BeginTime sync_time;
		sync_time.m_nBeginTime = m_nBeginTime;
		pOwnerPlayer->SendMsgToClient(MGPT_SC_BOT_BEGIN_TIME_SYNC, &sync_time);
	}

	return nResult;
}

const mem::map<int32_t, BotInstance*>& BotInstanceMgr::GetRawData()
{
	return m_ppBotInstance;
}

int BotInstanceMgr::CheckUpdate( uint32_t nNow )
{
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		if ( m_nTimer_Normal[i] <= nNow )
			return true;
	}

	if ( m_nTimer_Talent <= nNow )
		return true;

	return false;
}

int BotInstanceMgr::CheckUpdateBonus( uint32_t nBeginTime )
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return 0;
	}

	int maxBotTime = pkOwnerPlayer->GetMaxBotBonusTimer();

	//int32_t nBonusTimeDiff = m_nTimer_Normal - m_nBeginTime;
	//if ( nBonusTimeDiff + GetGlobalSetting.nBotBonusTimer_Normal_0 >= maxBotTime )
	//	return false;
	for ( int i = 0; i < eNormalBonus_Count; i++)
	{
		int32_t nNormalBonusConfigTimer = GetNormalBonusTimerByType(i);
		int32_t nNormalBonusTimer = m_nTimer_Normal[i] - m_nBeginTime;
		if ( nNormalBonusConfigTimer + nNormalBonusTimer <= maxBotTime )
			return true;
	}

	int32_t nTalentBonusTimer = 0;
	nTalentBonusTimer = m_nTimer_Talent - m_nBeginTime;
	if ( nTalentBonusTimer + GetGlobalSetting.nBotBonusTimer_Talent <= maxBotTime )
		return true;

	return false;
}

int BotInstanceMgr::TryUpdate_Normal( uint32_t nNow, uint32_t nMaxTimer, int nType )
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return 0;
	}

	int maxBotTime = pkOwnerPlayer->GetMaxBotBonusTimer();

	int32_t nDelta_Normal = nMaxTimer - m_nTimer_Normal[nType];
	if ( nDelta_Normal < 0 )
		return 0;
	if (nDelta_Normal > maxBotTime)
		nDelta_Normal = maxBotTime;

	uint32_t nConfTime = GetNormalBonusTimerByType(nType);

	int nCount = 0;
	if ( nDelta_Normal >= nConfTime )
	{
		nCount = nDelta_Normal / nConfTime;
		OnUpdate_Normal( nCount, nType );
		m_nTimer_Normal[nType] = nNow + nDelta_Normal - nConfTime * nCount;
	}

	return nCount;
}

int BotInstanceMgr::TryUpdate_Talent( uint32_t nNow, uint32_t nMaxTimer )
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return 0;
	}

	int maxBotTime = pkOwnerPlayer->GetMaxBotBonusTimer();

	int32_t nDelta_Talent = nMaxTimer - m_nTimer_Talent;
	if ( nDelta_Talent < 0 )
		return 0;
	if (nDelta_Talent > maxBotTime)
		nDelta_Talent = maxBotTime;

	uint32_t nConfTime = GetGlobalSetting.nBotBonusTimer_Talent;
	int nCount = 0;
	if ( nDelta_Talent >= nConfTime )
	{
		nCount = nDelta_Talent / nConfTime;
		OnUpdate_Talent( nCount );
		m_nTimer_Talent = nNow + nDelta_Talent - nConfTime * nCount;
		//m_nBeginTime = m_nTimer_Normal;
	}

	return nCount;
}

uint32_t BotInstanceMgr::GetNormalBonusTimerByType( int nType )
{
	uint32_t nResult = 0;
	switch (nType)
	{
	case eNormalBonus_0:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_0;
		break;
	case eNormalBonus_1:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_1;
		break;
	case eNormalBonus_2:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_2;
		break;
	case eNormalBonus_3:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_3;
		break;
	case eNormalBonus_4:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_4;
		break;
	case eNormalBonus_5:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_5;
		break;
	case eNormalBonus_6:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_6;
		break;
	case eNormalBonus_7:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_7;
		break;
	case eNormalBonus_8:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_8;
		break;
	case eNormalBonus_9:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_9;
		break;
	default:
		nResult = GetGlobalSetting.nBotBonusTimer_Normal_0;
		break;
	}
	return nResult;
}

int BotInstanceMgr::Update_Modified( float fDelta, float fCurrent )
{
	uint32_t nNow = time(NULL);
	if (GetCount() == 0 )
		return false;
	if ( CheckUpdate( nNow ) == false )
		return false;

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return 0;
	}

	int nMax = pkOwnerPlayer->GetMaxBotBonusTimer();
	int nLogicNow = m_nBeginTime + nMax;

	uint32_t nMaxTimer = UT_MathBase::MinInt(nLogicNow, nNow);

	if ( CheckUpdateBonus( m_nBeginTime ) == false )
		return false;

	bool bCacheChanged = false;
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		if ( TryUpdate_Normal( nNow, nMaxTimer, i ) )
			bCacheChanged = true;
	}

	if ( TryUpdate_Talent(nNow, nMaxTimer) )
		bCacheChanged = true;

	if ( bCacheChanged )
	{
		SyncBonusCache();
	}

	return true;
}

int BotInstanceMgr::Update( float fDelta, float fCurrent )
{
	Update_Modified( fDelta, fCurrent );
	return true;
}

int BotInstanceMgr::DoFetchBonus_Simple( LC_ServerPlayer* pOwner )
{
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache_Simple.begin();
	for ( ; it != m_bonusCache_Simple.end(); ++it )
	{
		LC_Helper::AddItem(pOwner->GetPackAsset(), it->first, it->second, 0, false);
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "DoFetchBonus_Simple: Item: %d Count: %d now: %d", it->first, it->second, time(NULL));
	}
	pOwner->SyncBackPackToClient(PACK_TYPE_BASIC);
	m_bonusCache_Simple.clear();
	return true;
}

int BotInstanceMgr::DoFetchBonus_Normal( LC_ServerPlayer* pOwner )
{
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache.begin();
	for ( ; it != m_bonusCache.end(); ++it )
	{
		pOwner->AddItem( it->first, it->second );
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "DoFetchBonus_Normal: Item: %d Count: %d now: %d", it->first, it->second, time(NULL));
	}
	pOwner->SyncBackPackToClient(PACK_TYPE_BASIC);
	return true;
}

int BotInstanceMgr::DoFetchBonus_Talent( LC_ServerPlayer* pOwner )
{
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (pkPackEntryFactory == NULL)
	{
		return false;
	}
	int nSize = m_bonusCache_Talent.size();
	int nEmpty = pOwner->GetPackAsset()->GetPackEmptyEntryCount(PACK_TYPE_BASIC);
	if ( nSize > nEmpty )
	{
		return false;
	}
	LC_BackPackEntryList kEntryList;

	PackOperateInfoList objOPList;
	objOPList.clear();
	mem::map<uint32_t, uint32_t>::iterator it2 = m_bonusCache_Talent.begin();
	for ( ; it2 != m_bonusCache_Talent.end(); ++it2 )
	{
		kEntryList.clear();
		pkPackEntryFactory->CreateItemEntry(it2->first, it2->second, CIET_NONE, kEntryList);
		PackOperateInfo objData;
		objData.Reset();
		objData.m_iOPType = OPT_ADD_ITEM;
		objData.m_bMerger  = true;
		objData.m_iPackType  = PACK_TYPE_BASIC;
		objData.m_bOPEntry = true;
		objData.m_kPackEntry.Clone(kEntryList[0]);
		objOPList.push_back(objData);
	}

	ResultType iResult = pOwner->GetPackAsset()->ExecutePackOP(objOPList, true);
	if (iResult == RE_SUCCESS)
	{
		pOwner->SyncBackPackToClient(PACK_TYPE_BASIC);
		PackOPRecordSet* record_set = pOwner->GetPackAsset()->GetRecordSet();
		pOwner->HandlePackOPRecord(record_set, 1, 0);
	}
	return true;
}

int BotInstanceMgr::DoFetchBonus( LC_ServerPlayer* pOwner, ResultType& resType )
{
	//int32_t nBonusTimer = m_nTimer_Normal - m_nBeginTime;
	//if (m_bonusCache.size() == 0 && m_bonusCache_Talent.size() == 0 && nBonusTimer < GetGlobalSetting.nBotBonusTimer_Normal_0)
	//{
	//	resType = RE_BOT_BONUS_WAIT;
	//	return false;
	//}
	int nRetCode = DoFetchBonus_Talent(pOwner);
	if ( nRetCode == false )
	{
		resType = RE_PRODUCTION_BACKPACK_FULL;
		return false;
	}
	resType = RE_SUCCESS;
	DoFetchBonus_Normal(pOwner);
	ClearBonusCache();
	m_nBeginTime = time(NULL);
	ResetBonusTimes(m_nBeginTime);
	SyncTimeStamp();
	return true;
}

void BotInstanceMgr::ResetBonusTimes( uint32_t nBeginTime )
{
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		uint32_t nTimer = GetNormalBonusTimerByType(i);
		DoResetBonusTimes( nBeginTime, m_nTimer_Normal[i], nTimer );
	}

	DoResetBonusTimes( nBeginTime, m_nTimer_Talent, GetGlobalSetting.nBotBonusTimer_Talent );
}

void BotInstanceMgr::DoResetBonusTimes( uint32_t nBeginTime, uint32_t& nOrigTime, uint32_t nTimer )
{
	int32_t nSpace = nBeginTime - nOrigTime;
	if (nSpace <= 0 || nTimer == 0 || nSpace < nTimer)
		return;

	if ( nSpace >= GetGlobalSetting.nBotBonusTimer_Max )
	{
		nOrigTime = nBeginTime;
		return;
	}

	int32_t nStepTimer = nSpace % nTimer;
	nOrigTime = nBeginTime - nStepTimer;
	return;
}

void BotInstanceMgr::ModifyBonusCache_Simple( uint32_t nItemId, int32_t nCount )
{
	if (nItemId == 0 || nCount == 0)
		return;
	int32_t nCurCount = nCount;
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache_Simple.find( nItemId );
	if (it != m_bonusCache_Simple.end())
	{
		it->second += nCount;
		nCurCount = it->second;
	}
	else
	{
		m_bonusCache_Simple.insert( std::make_pair(nItemId, nCount) );
	}
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ModifyBonusCacheSimple Add: %d, count: %d total: %d now: %d", nItemId, nCount, nCurCount, time(NULL));
}

void BotInstanceMgr::ModifyBonusCache( uint32_t nItemId, int32_t nCount )
{
	if (nItemId == 0 || nCount == 0)
		return;
	int32_t nCurCount = nCount;
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache.find( nItemId );
	if (it != m_bonusCache.end())
	{
		it->second += nCount;
		nCurCount = it->second;
	}
	else
	{
		m_bonusCache.insert( std::make_pair(nItemId, nCount) );
	}
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ModifyBonusCache Add: %d, count: %d total: %d now: %d", nItemId, nCount, nCurCount, time(NULL));
}

void BotInstanceMgr::ModifyBonusCache_Talent( uint32_t nItemId, int32_t nCount )
{
	if (nItemId == 0 || nCount == 0)
		return;
	int32_t nCurCount = nCount;
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache_Talent.find( nItemId );
	if (it != m_bonusCache_Talent.end())
	{
		it->second += nCount;
		nCurCount = it->second;
	}
	else
	{
		m_bonusCache_Talent.insert( std::make_pair(nItemId, nCount) );
	}
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ModifyBonusCache_Talent Add: %d, count: %d total: %d now: %d", nItemId, nCount, nCurCount, time(NULL));
}

void BotInstanceMgr::ClearBonusCache()
{
	m_bonusCache.clear();
	m_bonusCache_Talent.clear();
	SyncBonusCache();
}

void BotInstanceMgr::SyncTimeStamp()
{
	LC_ServerPlayer* pOwner  = GetOwnerPlayer();
	if ( pOwner == NULL )
		return ;

	MG_SC_BotDataSync_BeginTime sync;
	sync.m_nBeginTime = m_nBeginTime;
	pOwner->SendMsgToClient( MGPT_SC_BOT_BEGIN_TIME_SYNC, &sync);
}

void BotInstanceMgr::SyncBonusCache()
{
	LC_ServerPlayer* pOwner  = GetOwnerPlayer();
	if ( pOwner == NULL )
		return ;

	MG_SC_BotBonusCacheSync sync;
	sync.m_nCount = 0;
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache.begin(); 
	for ( ; it != m_bonusCache.end(); ++it )
	{
		if (sync.m_nCount >= MAX_BOT_BONUS_CACHE_COUNT)
			break;
		uint32_t nDebutItemId = it->first;
		uint32_t nDebutItemCount = it->second;
		sync.m_Data[sync.m_nCount].m_nItemId = it->first;
		sync.m_Data[sync.m_nCount].m_nCount = it->second;
		sync.m_nCount++;
	}
	mem::map<uint32_t, uint32_t>::iterator it2 = m_bonusCache_Talent.begin();
	for ( ; it2 != m_bonusCache_Talent.end(); ++it2 )
	{
		if (sync.m_nCount >= MAX_BOT_BONUS_CACHE_COUNT)
			break;
		sync.m_Data[sync.m_nCount].m_nItemId = it2->first;
		sync.m_Data[sync.m_nCount].m_nCount = it2->second;
		sync.m_nCount++;
	}
	pOwner->SendMsgToClient( MGPT_SC_BOT_BONUS_CACHE_SYNC, &sync);
}

int BotInstanceMgr::FetchNormalBonus( int nSeconds, int nType )
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
		return false;

	int nTimer = GetNormalBonusTimerByType(nType);
	int nCount = nSeconds / nTimer;
	// Create cache
	mem::map<int32_t, BotInstance*>::iterator it = m_ppBotInstance.begin();
	int nRetCode = false;
	for ( ;it != m_ppBotInstance.end(); ++it)
	{
		BotInstance* pInstance = it->second;
		if ( pInstance == NULL )
			continue;
		if (pInstance->DoNormalReward(pOwnerPlayer, this, nCount, true, nType))
			nRetCode = true;
	}

	// fetch
	DoFetchBonus_Simple(pOwnerPlayer);

	return nRetCode;
}

int BotInstanceMgr::TryFetchBonus()
{
	int nResult = false;
	MG_SC_BotRewardResult data;
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
	{
		return false;
	}
	data.m_nError = RE_SUCCESS;
	if ( m_nBeginTime == 0 )
	{
		data.m_nError = RE_SUCCESS;
		goto Exit0;
	}
	ResultType resType = RE_SUCCESS;
	bool bRetCode = DoFetchBonus( pOwnerPlayer, resType );
	data.m_nError = resType;
	nResult = true;
Exit0:
	pOwnerPlayer->SendMsgToClient( MGPT_SC_BOT_REWARD_RESULT, &data );
	return nResult;
}

int BotInstanceMgr::UpgradeLevel(int32_t nBotResId, bool bForce)
{
	int32_t nCurLevel = 0;
	Utility::UT_SIMDataList kItemReq;
	Utility::UT_SIMDataList::iterator it;
	int32_t nResult = false;
	MG_SC_BotUpgradeResult data;
	data.m_nErrorCode = RE_SUCCESS;
	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	if (pOwner == NULL)
	{
		data.m_nErrorCode = RE_BOT_INVALID_PLAYER;
		goto Exit0;
	}

	BotInstance* pInstance = GetBotInstance(nBotResId);
	if (pInstance == NULL)
	{
		data.m_nErrorCode = RE_BOT_INVALID_INSTANCE;
		goto Exit0;
	}

	CF_Bot::DataEntry* pResData = pInstance->GetConfig();
	if (pResData == NULL)
	{
		data.m_nErrorCode = RE_BOT_INVALID_RES;
		goto Exit0;
	}

	nCurLevel = pInstance->GetLevel();
	CSVFile::CF_BotUpgrade::DataEntry* pCfg = const_cast<CSVFile::CF_BotUpgrade::DataEntry*>(BuddyHelper::GetBotUpgradeData(pResData->_cID, nCurLevel ));
	CSVFile::CF_BotUpgrade::DataEntry* pNextCfg = const_cast<CSVFile::CF_BotUpgrade::DataEntry*>(BuddyHelper::GetBotUpgradeData(pResData->_cID, nCurLevel + 1 ));
	if (pCfg == NULL || pNextCfg == NULL)
	{
		data.m_nErrorCode = RE_BOT_INVALID_LEVEL;
		goto Exit0;
	}

	if (bForce == false)
	{
		Utility::ParseFmtString2DataIFs(pCfg->_sConsumption, kItemReq);
		it = kItemReq.begin();
		// Item Checking..
		for (; it != kItemReq.end(); ++it)
		{
			UT_SIMDataInfo info = *it;
			int32_t nItemId = info.GetSkillID();
			int32_t nItemCount = info.GetSkillEnableLevel();
			int nFoundCount = pOwner->GetPackAsset()->GetItemCountInPack(PACK_TYPE_BASIC, nItemId);
			if ( nFoundCount < nItemCount )
			{
				data.m_nErrorCode = RE_BOT_UPGRADE_RESOURCE;
				goto Exit0;
			}
		}
	}

#if 0
	// Money Checking
	if ( pOwner->GetUnBindCash().GetMoney() < pCfg->_needgold && bForce == false)
	{
		data.m_nErrorCode = RE_UNBIND_CASH_NOT_ENOUGH;
		goto Exit0;
	}
#endif

	if (bForce == false)
	{
		// Item Reducing
		it = kItemReq.begin();
		for (; it != kItemReq.end(); ++it)
		{
			UT_SIMDataInfo info = *it;
			int32_t nItemId = info.GetSkillID();
			int32_t nItemCount = info.GetSkillEnableLevel();
			pOwner->DeleteItem(nItemId, nItemCount);
		}
	}
#if 0
	// Money Reducing
	if (bForce == false)
		pOwner->ReduceCash(pCfg->_needgold, CT_UNBIND_CASH, SYS_FUNC_TP_BUDDY_STAR_LEVEL, pResData->_cID);
#endif
	// just a impl.
	pInstance->UpgradeLevel();
	data.m_Data.m_nTimeStamp = pInstance->GetTimeStamp();
	data.m_Data.m_nResId = pInstance->GetTemplateId();
	data.m_Data.m_nLevel = pInstance->GetLevel();

	//升级后，通知成就系统
	AchievementEvent kBotEvent;
	kBotEvent.SetConditionInfo(pInstance->GetLevel());
	kBotEvent.SetNum(1);
	pOwner->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BOT, kBotEvent);

Exit0:
	pOwner->SendMsgToClient(MGPT_SC_BOT_UPGRADE_RESULT, &data);
	nResult = true;
	return nResult;
}

int BotInstanceMgr::OnUpdate_Normal( int nCount, int nType )
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
		return false;

	mem::map<int32_t, BotInstance*>::iterator it = m_ppBotInstance.begin();
	for ( ;it != m_ppBotInstance.end(); ++it)
	{
		BotInstance* pInstance = it->second;
		if ( pInstance == NULL )
			continue;
		pInstance->DoNormalReward(pOwnerPlayer, this, nCount, false, nType);
	}
	return true;
}

int BotInstanceMgr::OnUpdate_Talent( int nCount )
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
		return false;

	mem::map<int32_t, BotInstance*>::iterator it = m_ppBotInstance.begin();
	for ( ;it != m_ppBotInstance.end(); ++it)
	{
		BotInstance* pInstance = it->second;
		if ( pInstance == NULL )
			continue;
		pInstance->DoTalentReward(pOwnerPlayer, this, nCount);
	}
	return true;
}

void BotInstanceMgr::ResetBeginTimes(uint32_t time)
{
	m_nBeginTime = time;
	InitNormalBonusTime(m_nBeginTime, true);
	m_nTimer_Talent = m_nBeginTime;
}

uint32_t BotInstanceMgr::GetBeginTime()
{
	return m_nBeginTime;
}

uint32_t BotInstanceMgr::GetTimer_Normal(int index)
{
	if(index >= eNormalBonus_Count)
	{
		return 0;
	}

	return m_nTimer_Normal[index];
}

void BotInstanceMgr::ResetTimer_Normal(int index, uint32_t time)
{
	if(index >= eNormalBonus_Count)
	{
		return;
	}

	m_nTimer_Normal[index] = time;
}

void BotInstanceMgr::Enable(uint32_t time)
{
	m_nBeginTime = time;
	InitNormalBonusTime(m_nBeginTime, true);
	m_nTimer_Talent = m_nBeginTime;
}

int BotInstanceMgr::GetCount()
{
	return m_ppBotInstance.size();
}

BotInstance* BotInstanceMgr::GetBotInstance( int32_t nBotResId )
{
	BotInstance* pResult = NULL;
	mem::map<int32_t, BotInstance*>::iterator it = m_ppBotInstance.find(nBotResId);
	if (it == m_ppBotInstance.end())
		return pResult;

	pResult = it->second;
	return pResult;
}

void BotInstanceMgr::RecoverAllData()
{
	object_id_type nOrigOwnerId = m_nOwnerPlayerId;
	ReUse();
	Init( nOrigOwnerId );
}

void BotInstanceMgr::ReUse()
{
	m_nOwnerPlayerId = INVALID_ACTOR_ID;
	m_nBeginTime = 0;
	m_nTimer_Talent = 0;
	for ( int i = 0; i < eNormalBonus_Count; i++ )
		m_nTimer_Normal[ i ] = 0;
	m_bonusCache.clear();
	m_bonusCache_Simple.clear();
	m_bonusCache_Talent.clear();
	m_nLastBonusTimer = 0;
	ReUse_BotInstance();
}

void BotInstanceMgr::ReUse_BotInstance()
{
	// avoid mem-leak. release all instance.
	mem::map<int32_t, BotInstance*>::iterator it = m_ppBotInstance.begin();
	for ( ; it != m_ppBotInstance.end(); ++it )
	{
		BotInstance* pData = it->second;
		T_DELETE_D (pData);
	}
	m_ppBotInstance.clear();
}

BotInstance* BotInstanceMgr::AddBot_Impl(CSVFile::CF_Bot::DataEntry* pResData)
{
	BotInstance* pInstance = NULL;
	pInstance = CreateBotInstance(  pResData );
	if (pInstance == NULL)
		return pInstance;

	m_ppBotInstance.insert(std::make_pair(pResData->_cID, pInstance));

	return pInstance;
}

int BotInstanceMgr::RemoveBot( BotInstance* pInstance)
{
	return true;
}

int BotInstanceMgr::GenBotTypeData()
{
	return 0;
}


void BotInstanceMgr::SyncAllBotData()
{
	SyncBotBaseData();
	SyncBonusCache();
}

void BotInstanceMgr::SyncBotBaseData()
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
		return;

	MG_SC_BotDataSync sync;
	sync.m_nBeginTime = m_nBeginTime;
	sync.m_nCount = 0;
	mem::map<int32_t, BotInstance*>::iterator it = m_ppBotInstance.begin();
	for ( ; it != m_ppBotInstance.end() ; ++it )
	{
		BotInstance* pInstance = it->second;
		if (pInstance == NULL)
			continue;

		sync.m_Data[sync.m_nCount].m_nTimeStamp = pInstance->GetTimeStamp();
		sync.m_Data[sync.m_nCount].m_nResId = pInstance->GetTemplateId();
		sync.m_Data[sync.m_nCount].m_nLevel = pInstance->GetLevel();

		sync.m_nCount++;
	}

	pOwnerPlayer->SendMsgToClient(MGPT_SC_BOT_BASE_DATA_SYNC, &sync);
}

void BotInstanceMgr::SyncBotBonusData()
{
	SyncBonusCache();
}


object_id_type BotInstanceMgr::GetOwnerPlayerId()
{
	return m_nOwnerPlayerId;
}

LC_ServerPlayer* BotInstanceMgr::GetOwnerPlayer()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nOwnerPlayerId);
	return pPlayer;
}

BotInstance* BotInstanceMgr::CreateBotInstance( CSVFile::CF_Bot::DataEntry* pResData )
{
	BotInstance* pInstance = T_NEW_D BotInstance(m_nOwnerPlayerId, this);

	pInstance->InitTemplateData(pResData, true);

	return pInstance;
}
