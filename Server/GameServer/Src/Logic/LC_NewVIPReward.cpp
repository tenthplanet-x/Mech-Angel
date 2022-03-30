#include "LM_LocaleManager.h"
#include "GameLogicScriptPCH.h"
#include "LC_NewVIPReward.h"
#include "LC_ServerPlayer.h"
#include "GlobalSettings.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "MG_GameProtocol.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;
LC_NewVIPReward::LC_NewVIPReward()
	: m_nOwnerId(INVALID_ACTOR_ID)
	, m_nVIPLevel(0)
	, m_nBeginTime(-1)
{
	
}

LC_NewVIPReward::~LC_NewVIPReward()
{

}

void LC_NewVIPReward::Init( object_id_type nObjId )
{
	m_nOwnerId = nObjId;
}

void LC_NewVIPReward::ReUse()
{
	m_nOwnerId = INVALID_ACTOR_ID;
	m_bonusCache.clear();
}

void LC_NewVIPReward::ResetBeginTime(uint32_t beginTime)
{
	m_nBeginTime = beginTime;
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		m_nTimer_Normal[ i ] = m_nBeginTime;
	}
}

uint32_t LC_NewVIPReward::GetBeginTime()
{
	return m_nBeginTime;
}

void LC_NewVIPReward::ResetTimer_Normal(int32_t index, uint32_t time)
{
	if(index >= eNormalBonus_Count)
	{
		return;
	}

	m_nTimer_Normal[index] = time;
}

uint32_t LC_NewVIPReward::GetTimer_Normal(int32_t index)
{
	if(index >= eNormalBonus_Count)
	{
		return 0;
	}

	return m_nTimer_Normal[index];
}

void LC_NewVIPReward::Enable(uint32_t time)
{
	m_nBeginTime = time;
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		m_nTimer_Normal[ i ] = m_nBeginTime;
	}
	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	if ( pOwner == NULL )
		return;
	MG_SC_NewVIPSync_BeginTime sync_time;
	sync_time.m_nBeginTime = m_nBeginTime;
	pOwner->SendMsgToClient(MGPT_SC_NEWVIP_BEGIN_TIME_SYNC, &sync_time);
}

void LC_NewVIPReward::LoadLevel( int32_t nLevel )
{
	m_nVIPLevel = nLevel;
}

void LC_NewVIPReward::SetLevel( int32_t nLevel )
{
	if ( nLevel ==  m_nVIPLevel)
		return;

	if ( m_nVIPLevel <= 0 && m_nBeginTime==0)
	{
		uint32_t currentTime = time(NULL);
		Enable(currentTime);

		LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
		if(NULL != pkOwnerPlayer)
		{
			pkOwnerPlayer->EnableBotManager(currentTime);
		}
	}
	m_nVIPLevel = nLevel;
}

void LC_NewVIPReward::Update( float fCurrent, float fDelta)
{
	UpdateReward( fCurrent, fDelta);
}

int LC_NewVIPReward::CheckUpdate( uint32_t nNow )
{
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		if ( m_nTimer_Normal[i] <= nNow )
			return true;
	}
	return false;
}

int LC_NewVIPReward::CheckUpdateBonus( uint32_t nBeginTime )
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

	return false;

}

uint32_t LC_NewVIPReward::GetNormalBonusTimerByType( int nType )
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

LC_ServerPlayer* LC_NewVIPReward::GetOwnerPlayer()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nOwnerId);
	return pPlayer;
}

ResultType LC_NewVIPReward::GetVIPExtraReward(std::vector<Protocol::PS_ItemRecord>& itemInfo)
{
	itemInfo.reserve(30);
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	CF_VipNew::DataEntry* pEntry = SafeGetCSVFileDataEntryPtr<CF_VipNew>(m_nVIPLevel);
	if(NULL == pEntry)
	{
		return RE_FAIL;
	}

	if(GetGlobalSetting.VIPExtraRewardTimes < 1)
	{
		return RE_VIPEXTRA_REWARD_ERROR_4;
	}

	UT_SIMDataList itemList;
	for(int productType=eNormalBonus_0; productType<eNormalBonus_Count; ++productType)
	{
		uint32_t itemID = 0;
		uint32_t itemCount = 0;
		int nRetCode = GetNormalRewardByType(pkOwnerPlayer, productType, itemID, itemCount, pEntry);
		if(true == nRetCode)
		{
			UT_SIMDataInfo item;
			item.SetItemInfo(itemID, itemCount*GetGlobalSetting.VIPExtraRewardTimes);
			itemList.push_back(item);

			PS_ItemRecord data;
			data.m_iItemID = itemID;
			data.m_iItemCount = itemCount*GetGlobalSetting.VIPExtraRewardTimes;
			itemInfo.push_back(data);
		}
	}

	mem::map<int, int> itemDataList;
	RewardTypeList reward_list;
	pkOwnerPlayer->GetVIPDropItem(pEntry->_iDropID1, GetGlobalSetting.VIPExtraRewardTimes/GetGlobalSetting.VIPDropTime, reward_list);
	for(RewardTypeList::iterator it=reward_list.begin(); it!=reward_list.end(); ++it)
	{
		int itemID = 0;
		int itemNum = 0;
		switch(it->m_eType)
		{
		case GameLogic::LC_Reward::RT_GOLD:
			{
				itemID = SPEC_ITEM_TID_CASH;
				itemNum = it->GetGoldCount();
			}
			break;
		case GameLogic::LC_Reward::RT_ITEM:
			{
				itemID = it->GetItemContext().GetBackPackValueByType(LPSFET_TYPE_ID);
				itemNum = it->GetItemContext().GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
			}
			break;
		case GameLogic::LC_Reward::RT_BOSS_JP:
			{
				itemID = SPEC_ITEM_TID_BOSS_JP;
				itemNum = it->GetBossJP();
			}
			break;
		case GameLogic::LC_Reward::RT_UNBINDYUANBAO:
			{
				itemID = SPEC_ITEM_TID_YUANBAO;
				itemNum = it->GetUnbindyuanbaoCount();
			}
			break;
		case GameLogic::LC_Reward::RT_RUNE_CREAM:
			{
				itemID = SPEC_ITEM_TID_RUNE_CREAM;
				itemNum = it->GetRuneCreamCount();
			}
			break;
		}

		if(0!=itemID && 0!=itemNum)
		{
			mem::map<int, int>::iterator findIt = itemDataList.find(itemID);
			if(findIt == itemDataList.end())
			{
				itemDataList.insert(std::make_pair(itemID, itemNum));
			}
			else
			{
				findIt->second += itemNum;
			}			
		}
	}

	for(mem::map<int, int>::iterator it=itemDataList.begin(); it!=itemDataList.end(); ++it)
	{
		UT_SIMDataInfo item;
		item.SetItemInfo(it->first, it->second);
		itemList.push_back(item);

		PS_ItemRecord data;
		data.m_iItemID = it->first;
		data.m_iItemCount = it->second;
		itemInfo.push_back(data);
	}

	for(UT_SIMDataList::iterator it=pEntry->_sReward.begin(); it!=pEntry->_sReward.end(); ++it)
	{
		itemList.push_back(*it);

		PS_ItemRecord data;
		data.m_iItemID = it->IID();
		data.m_iItemCount = it->ParamA();
		itemInfo.push_back(data);
	}
	
	//vip快速领奖--埋点日志
	vector<int> nParams;
	nParams.clear();
	int nTimes = pkOwnerPlayer->GetUsedVIPExtraRewardTimes();
	nParams.push_back(nTimes);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, VIP_OPERATE, OPERATE_LOGTYPE3);
	if(!itemList.empty())
	{
		StringType title;
		StringType desc;
		ResultType res = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, itemList, pkOwnerPlayer, SYS_FUNC_TP_ACTIVITY_CHARGE_INTERVAL, m_nVIPLevel, title, desc, false,false,false, nLogCode);
		if (RE_SUCCESS != res)
		{
			itemInfo.clear();
			return res;
		}
	}

	return RE_SUCCESS;
}

int LC_NewVIPReward::UpdateReward( float fCurrent, float fDelta )
{
	uint32_t nNow = time(NULL);
	//m_nVIPLevel = nLevel;

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

	if ( bCacheChanged )
	{
		SyncBonusCache();
	}

	return true;
}

int LC_NewVIPReward::TryUpdate_Normal( uint32_t nNow, uint32_t nMaxTimer, int nType )
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

int LC_NewVIPReward::OnUpdate_Normal( int nCount, int nType )
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
		return false;

	DoNormalReward(pOwnerPlayer, nCount, false, nType);

	return true;
}

int LC_NewVIPReward::DoNormalReward( LC_ServerPlayer* pOwner,  int nCount, bool bSimple, int nType )
{
	CF_VipNew* pCfg = CF_VipNew::GetSingletonPtr();
	const CF_VipNew::DataEntry* pEntry = pCfg->GetEntryPtr(m_nVIPLevel);
	if ( pEntry == NULL )
		return false;

	uint32_t nSimpleItemId = 0;
	uint32_t nSimpleCount = 0;
	int nRetCode = GetNormalRewardByType(pOwner,nType, nSimpleItemId, nSimpleCount, pEntry);
	if (nRetCode == false)
		return false;

	if ( bSimple )
	{
		ModifyBonusCache_Simple( nSimpleItemId, nSimpleCount * nCount );
	}
	else
	{
		ModifyBonusCache( nSimpleItemId, nSimpleCount * nCount );
	}
	if (nSimpleCount == 0)
		return false;

	DoFetchBonus_Simple(pOwner);
	return true;
}

int LC_NewVIPReward::DoFetchBonus_Simple( LC_ServerPlayer* pOwner )
{
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache_Simple.begin();
	for ( ; it != m_bonusCache_Simple.end(); ++it )
	{
		pOwner->AddItem( it->first, it->second );
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_NewVIPReward::DoFetchBonus_Simple: Item: %d Count: %d now: %d", it->first, it->second, time(NULL));
	}
	pOwner->SyncBackPackToClient(PACK_TYPE_BASIC);
	m_bonusCache_Simple.clear();
	return true;
}

void LC_NewVIPReward::ModifyBonusCache_Simple( uint32_t nItemId, int32_t nCount )
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

	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_NewVIPReward::BonusCacheSimple Add: %d, count: %d total: %d now: %d", nItemId, nCount, nCurCount, time(NULL));
}

void LC_NewVIPReward::ModifyBonusCache( uint32_t nItemId, int32_t nCount )
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
}

void LC_NewVIPReward::ClearBonusCache()
{
	m_bonusCache.clear();
	SyncBonusCache();
}

void LC_NewVIPReward::SyncBonusCache()
{
	LC_ServerPlayer* pOwner  = GetOwnerPlayer();
	if ( pOwner == NULL )
		return ;

	MG_SC_NewVIPBonusCacheSync sync;
	sync.m_nCount = 0;
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache.begin(); 
	float fCurrent = GET_PROCESS_TIME;
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_NewVIPReward::SyncBonusCache <%d> LV: <%d> Begin: %f", m_nOwnerId, m_nVIPLevel, fCurrent );
	for ( ; it != m_bonusCache.end(); ++it )
	{
		if(sync.m_nCount >= MAX_BOT_BONUS_CACHE_COUNT)
		{
			//GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_NewVIPReward::SyncBonusCache size>MAX_BOT_BONUS_CACHE_COUNT\n");
			break;
		}
		sync.m_Data[sync.m_nCount].m_nItemId = it->first;
		sync.m_Data[sync.m_nCount].m_nCount = it->second;
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_NewVIPReward::SyncBonusCacheDetail <%d> now: %f Item: %d Count: %d", m_nOwnerId, fCurrent, sync.m_Data[sync.m_nCount].m_nItemId, sync.m_Data[sync.m_nCount].m_nCount );
		sync.m_nCount++;
	}

	pOwner->FillVIPDropItem(sync);
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_NewVIPReward::SyncBonusCache End <%d> Count: %d", m_nOwnerId, sync.m_nCount );
	pOwner->SendMsgToClient( MGPT_SC_NEWVIP_BONUS_CACHE_SYNC, &sync);
}


int LC_NewVIPReward::GetNormalRewardByType(LC_ServerPlayer* pOwner, int32_t nType, uint32_t& nItemId, uint32_t& nCount, const CSVFile::CF_VipNew::DataEntry* pCfg  )
{
	uint32_t uRate = 0;
	uRate = pOwner->GetAddRateToVip();
	switch ( nType )
	{
	case eNormalBonus_0:
		nItemId = pCfg->_iReward1;
		nCount = pCfg->_iEfficiency1;
		nCount = nCount + nCount*uRate/10000;
		break;
	case eNormalBonus_1:
		nItemId = pCfg->_iReward2;
		nCount = pCfg->_iEfficiency2;
		nCount = nCount + nCount*uRate/10000;
		break;
	case eNormalBonus_2:
		nItemId = pCfg->_iReward3;
		nCount = pCfg->_iEfficiency3;
		nCount = nCount + nCount*uRate/10000;
		break;
	case eNormalBonus_3:
		nItemId = pCfg->_iReward4;
		nCount = pCfg->_iEfficiency4;
		nCount = nCount + nCount*uRate/10000;
		break;
	case eNormalBonus_4:
		nItemId = pCfg->_iReward5;
		nCount = pCfg->_iEfficiency5;
		break;
	case eNormalBonus_5:
		nItemId = pCfg->_iReward6;
		nCount = pCfg->_iEfficiency6;
		break;
	case eNormalBonus_6:
		nItemId = pCfg->_iReward7;
		nCount = pCfg->_iEfficiency7;
		break;
	case eNormalBonus_7:
		nItemId = pCfg->_iReward8;
		nCount = pCfg->_iEfficiency8;
		break;
	case eNormalBonus_8:
		nItemId = pCfg->_iReward9;
		nCount = pCfg->_iEfficiency9;
		break;
	case eNormalBonus_9:
		nItemId = pCfg->_iReward10;
		nCount = pCfg->_iEfficiency10;
		break;
	default:
		return false;
		break;
	}

	return nCount!=0 ? true : false;
}

void LC_NewVIPReward::SyncTimeStamp()
{
	LC_ServerPlayer* pOwner  = GetOwnerPlayer();
	if ( pOwner == NULL )
		return ;

	MG_SC_NewVIPSync_BeginTime sync;
	sync.m_nBeginTime = m_nBeginTime;
	pOwner->SendMsgToClient( MGPT_SC_NEWVIP_BEGIN_TIME_SYNC, &sync);

}

ResultType LC_NewVIPReward::TryFetchBonus()
{	
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
	{
		return RE_FAIL;
	}

	if ( m_nBeginTime == 0 )
	{
		return RE_BOT_BONUS_DISABLE;
	}
	
	ResultType resType = RE_FAIL;
	bool bRetCode = DoFetchBonus( pOwnerPlayer, resType );
	return resType;
}

void LC_NewVIPReward::Save(char_data_define::new_vip_reward & detail_data)
{
	detail_data.set_begin_time(m_nBeginTime);
	//detail_data.set_bonus_time_normal(m_nTimer_Normal);
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		detail_data.add_bonus_time_normal( m_nTimer_Normal[i] );
		//char_data_define::bot_bonus_data* bonus_data = detail_data.add_data_cache();
	}
	//detail_data.set_bonus_time_talent(m_nTimer_Talent);
	//map<int32_t, BotInstance*>::iterator it = m_ppBotInstance.begin();
	//for ( ; it != m_ppBotInstance.end(); ++it )
	//{
	//	BotInstance* pInstance = it->second;
	//	pInstance->Save(*detail_data.add_data() );
	//}

	mem::map<uint32_t, uint32_t>::iterator itBase = m_bonusCache.begin();
	for ( ; itBase != m_bonusCache.end(); itBase++ )
	{
		if (itBase->first == 0)
			continue;
		char_data_define::bot_bonus_data* bonus_data = detail_data.add_data_cache();
		bonus_data->set_item_id(itBase->first);
		bonus_data->set_item_count(itBase->second);
	}
	//map<uint32_t, uint32_t>::iterator itTalent = m_bonusCache_Talent.begin();
	//for ( ; itTalent != m_bonusCache_Talent.end(); itTalent++ )
	//{
	//	if (itTalent->first == 0)
	//		continue;
	//	char_data_define::bot_bonus_data* bonus_data = detail_data.add_data_cache_talent();
	//	bonus_data->set_item_id(itTalent->first);
	//	bonus_data->set_item_count(itTalent->second);
	//}
}

void LC_NewVIPReward::Load(const char_data_define::new_vip_reward & detail_data)
{
	if ( detail_data.bonus_time_normal_size() != eNormalBonus_Count )
	{
		InitNormalBonusTime(m_nBeginTime, true);
	}
	m_nBeginTime = detail_data.begin_time();
	detail_data.bonus_time_normal_size();
	for ( int i = 0; i < detail_data.bonus_time_normal_size(); i++ )
	{
		m_nTimer_Normal[ i ] = detail_data.bonus_time_normal(i);
	}
	if (m_nTimer_Normal[0] == 0)
		InitNormalBonusTime(m_nBeginTime);
	for ( int i = 0; i < detail_data.data_cache_size(); i++ )
	{
		ModifyBonusCache( detail_data.data_cache(i).item_id(), detail_data.data_cache(i).item_count() );
	}
}

void LC_NewVIPReward::SyncToClient()
{
	if ( m_nVIPLevel <= 0 )
		return;
	SyncTimeStamp();
	SyncBonusCache();
}

int LC_NewVIPReward::FetchNormalBonus( int nSeconds, int nType )
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
		return false;

	int nTimer = GetNormalBonusTimerByType(nType);
	int nCount = nSeconds / nTimer;
	int nRetCode = false;
	nRetCode = DoNormalReward(pOwnerPlayer, nCount, true, nType);

	// fetch
	//DoFetchBonus_Simple(pOwnerPlayer);

	return nRetCode;
}

int LC_NewVIPReward::DoFetchBonus( LC_ServerPlayer* pOwner, ResultType& resType )
{
	resType = RE_SUCCESS;
	DoFetchBonus_Normal(pOwner);
	ClearBonusCache();
	m_nBeginTime = time(NULL);
	ResetBonusTimes(m_nBeginTime);
	SyncTimeStamp();
	return true;
}

int LC_NewVIPReward::DoFetchBonus_Normal( LC_ServerPlayer* pOwner )
{
	mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache.begin();
	for ( ; it != m_bonusCache.end(); ++it )
	{
		//pOwner->AddItem( it->first, it->second );
		LC_Helper::AddItem(pOwner->GetPackAsset(), it->first,it->second, 0, false);
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_NewVIPReward::DoFetchBonus_Normal: Item: %d Count: %d now: %d", it->first, it->second, time(NULL));
	}
	pOwner->SyncBackPackToClient(PACK_TYPE_BASIC);
	return true;
}

void LC_NewVIPReward::ResetBonusTimes( uint32_t nBeginTime )
{
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		uint32_t nTimer = GetNormalBonusTimerByType(i);
		DoResetBonusTimes( nBeginTime, m_nTimer_Normal[i], nTimer );
	}
}

void LC_NewVIPReward::DoResetBonusTimes( uint32_t nBeginTime, uint32_t& nOrigTime, uint32_t nTimer )
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

void LC_NewVIPReward::InitNormalBonusTime( uint32_t nTime, bool bForce )
{
	for ( int i = 0; i < eNormalBonus_Count; i++ )
	{
		if ( m_nTimer_Normal[ i ] == 0 || bForce )
			m_nTimer_Normal[ i ] = nTime;
	}
}

void LC_NewVIPReward::FillBonusCache(Protocol::MG_SC_NewVIPBonusCacheSync& msg)
{
	LC_ServerPlayer* pkPlayer = GetOwnerPlayer();
	if(NULL == pkPlayer)
	{
		return;
	}

	const unique_id_impl& instance = pkPlayer->GetInstance();
	if(msg.m_nCount >= MAX_BOT_BONUS_CACHE_COUNT)
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "FillBonusCache-1 error! size:%d, c_cid:%d, c_citizenship:%d, c_uid:%d", msg.m_nCount, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return;
	}

	for (mem::map<uint32_t, uint32_t>::iterator it = m_bonusCache.begin() ; it != m_bonusCache.end(); ++it )
	{
		if(msg.m_nCount >= MAX_BOT_BONUS_CACHE_COUNT)
		{
			GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "FillBonusCache-2 error! size:%d, c_cid:%d, c_citizenship:%d, c_uid:%d", msg.m_nCount, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
			break;
		}
		msg.m_Data[msg.m_nCount].m_nItemId = it->first;
		msg.m_Data[msg.m_nCount].m_nCount = it->second;
		msg.m_nCount++;
	}
}

bool LC_NewVIPReward::CheckUpdateBonus()
{
	return CheckUpdateBonus(0)==0 ? false : true;
}
