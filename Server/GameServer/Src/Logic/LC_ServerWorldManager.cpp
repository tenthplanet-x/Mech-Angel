#include "GameServerApp.h"

#include "LC_ServerMap.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerMapLogicFactory.h"
#include "LC_ServerMapLogic_NewBee.h"
#include "LC_ServerMapLogic_TranscriptionRuleManger.h"
#include "LC_StoryLogic_Manager.h"
#include "MG_Common.h"
#include "UT_PathGrid.h"
#include "LC_ServerMapLogic_PlayManager.h"
#include "UT_ServerHelper.h"
#include "LC_GameStory_Manager.h"
#include "CF_WorldMapList.h"
#include "CF_TranscriptionRule.h"
#include "CF_MapCategory.h"
#include "LC_ServerAllianceManager.h"
#include "CF_ForcesBattle.h"
#include "LC_ServerPlaceHolderManager.h"
#include "CF_GloryExpedition.h"
#include "CF_RegularBoss.h"
#include "LC_ServerNPC.h"
#include "LC_ServerNPCManager.h"
#include "LC_GameEventManager.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
using namespace CSVFile;


//------------------------------------------------------------------
LC_ServerWorldManager::LC_ServerWorldManager()
 : LC_WorldManagerBase()
 , m_kLogicID2Logicmap(250)	/// Utility::CSUnorderedMap
{
}
//------------------------------------------------------------------
LC_ServerWorldManager::~LC_ServerWorldManager()
{
}
//------------------------------------------------------------------
bool LC_ServerWorldManager::Init()
{
	m_bInitializing = true;
	m_iMapLogicAllocID = MAP_LOGIC_ID_BASE;


	CF_WorldMapList::DataEntryMapExternIterator iter = CF_WorldMapList::GetSingleton().GetIterator();
	while (iter.HasMoreElements())
	{
		CF_WorldMapList::DataEntry* rkValue = iter.PeekNextValuePtr();
		/// 新手地图
		if (rkValue->_iMapLogicType == MT_NEWBEE)
		{
			m_kNewBeeResIDs.insert(rkValue->_iID);
		}

		if (rkValue->_AutoCreate == 1)
		{
			LC_ServerMapLogic_Place* place = _createServerMapLogic(m_iMapLogicAllocID++, rkValue->_iMapLogicType, rkValue->_iID, rkValue, true);
			if (place == NULL)
			{
				GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, " LC_ServerWorldManager Init _AutoCreate  create map [%s] LogicType[%d] ResID[%d] SizeType[%d] failed!", rkValue->_kName.c_str(), rkValue->_iMapLogicType, rkValue->_iID, rkValue->_iSizeType);
			}
			else
			{
				GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, " LC_ServerWorldManager Init _AutoCreate create map [%s] LogicType[%d] ResID[%d] SizeType[%d] successfully!", rkValue->_kName.c_str(), rkValue->_iMapLogicType, rkValue->_iID, rkValue->_iSizeType);
			}

			/// 分流地图
			if (rkValue->_iTributaryMapCount > 1)
			{
				if(rkValue->_EnterMode == 0)		/// 玩家进满一条线再进另一条
				{
					m_kTributaryMapResIDs.insert(rkValue->_iID);
				}
				else if(rkValue->_EnterMode == 1)		/// 玩家依次进入分线，因此要把分线先都创建出来
				{
					for(int i=1; i < rkValue->_iTributaryMapCount; ++i)
					{
						LC_ServerMapLogic_Place* place = _createServerMapLogic(m_iMapLogicAllocID++, rkValue->_iMapLogicType, rkValue->_iID, rkValue, true);
						if (place == NULL)
						{
							GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, " LC_ServerWorldManager Init _EnterMode  create map [%s] LogicType[%d] ResID[%d] SizeType[%d] failed!", rkValue->_kName.c_str(), rkValue->_iMapLogicType, rkValue->_iID, rkValue->_iSizeType);
						}
						else
						{
							GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, " LC_ServerWorldManager Init _EnterMode  create map [%s] LogicType[%d] ResID[%d] SizeType[%d] successfully!", rkValue->_kName.c_str(), rkValue->_iMapLogicType, rkValue->_iID, rkValue->_iSizeType);
						}
					}
				}
			}
			else if(rkValue->_lineMapCount > 1)
			{
				for(int i=1; i<rkValue->_lineMapCount; ++i)
				{
					LC_ServerMapLogic_Place* place = _createServerMapLogic(m_iMapLogicAllocID++, rkValue->_iMapLogicType, rkValue->_iID, rkValue, true, i);
					if (place == NULL)
					{
						GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, " LC_ServerWorldManager Init _lineMapCount  create map [%s] LogicType[%d] ResID[%d] SizeType[%d] failed!", rkValue->_kName.c_str(), rkValue->_iMapLogicType, rkValue->_iID, rkValue->_iSizeType);
					}
					else
					{
						GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, " LC_ServerWorldManager Init _lineMapCount  create map [%s] LogicType[%d] ResID[%d] SizeType[%d] successfully!", rkValue->_kName.c_str(), rkValue->_iMapLogicType, rkValue->_iID, rkValue->_iSizeType);
					}
				}
			}
		}

		iter.MoveNext();
	}

	m_kActiveSectorClusVector.resize(1000);
	m_uiActiveSectorClusSize = 0;

	{
		CF_MapCategory* inst = SafeGetSingleton<CF_MapCategory>();
		CF_MapCategory::DataEntryMapExternIterator i = inst->GetIterator();
		while (i.HasMoreElements())
		{
			const CF_MapCategory::DataEntry* pkEntry = i.PeekNextValuePtr();
			if (MST_TOWN == pkEntry->_iRaidType)
			{
				m_townmaps.insert(pkEntry->_lMaprResID);
			}

			i.MoveNext();
		}
	}

	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_FAST));
	m_kTribuMapTimer.SetTimer(GET_INTERVAL(CMN_INTER_DULL));

	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_INFO, "LC_ServerWorldManager::Init");
	m_bInitializing = false;
	return true;
}
//------------------------------------------------------------------
bool LC_ServerWorldManager::UnInit()
{
	for (LC_ServerMapLogicIDPtrMap::iterator it = m_kLogicID2Logicmap.begin(), itEnd=m_kLogicID2Logicmap.end();
			it != itEnd; ++it)
	{
		T_SAFE_DELETE((*it).second);
	}
	m_kLogicID2Logicmap.clear();
	m_kLogicMapIndices.clear();
	m_kNewBeeResIDs.clear();
	m_kTributaryRecords.clear();

	LC_MapIDPtrMapIter IterBe = m_kMapIDPtrMap.begin();
	LC_MapIDPtrMapIter IterEd = m_kMapIDPtrMap.end();

	for (; IterBe  != IterEd ; ++IterBe)
	{
		LC_MapBase* pkServerMap = IterBe->second;
		pkServerMap->UnInit();
		T_SAFE_DELETE(pkServerMap);
	}
	m_kMapIDPtrMap.clear();
	return true;
}
//------------------------------------------------------------------
void LC_ServerWorldManager::Update(float fCurrentTime, float fDeltaTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}
	
	// 地图逻辑更新，删除非Active的地图
	mem::vector<LC_ServerMapLogic_Place*> trush_ids ;	trush_ids.reserve(10);
	LC_ServerMapLogicIDPtrMap::iterator	it = m_kLogicID2Logicmap.begin();
	LC_ServerMapLogicIDPtrMap::iterator	itEnd = m_kLogicID2Logicmap.end();
	for (; it != itEnd; ++it)
	{
		LC_ServerMapLogic_Place* pkServerMapLogic = (*it).second;
		if (pkServerMapLogic)
		{
			int mapLogicID = pkServerMapLogic->GetMapLogicID();
			if (pkServerMapLogic->IsActive())
			{
				pkServerMapLogic->Update(fCurrentTime, fDeltaTime);
			}
			else if(pkServerMapLogic->GetMapResPtr()->_AutoCreate == 0)
			{
				trush_ids.push_back(pkServerMapLogic);
			}
		}
	}

	// 销毁非自动创建 且 非Active的地图
	for (size_t i=0;i<trush_ids.size();i++)
	{
		LC_ServerMapLogic_Place* pkServerMapLogic = trush_ids[i];
		_releaseServerMapLogic(pkServerMapLogic);
	}
	
	//分流地图动态销毁
	if (!m_kTribuMapTimer.IsExpired())
	{
		return;
	}

	uint32_t expiration_time = GetGlobalSetting.nTributaryMapExpirationTime;
	uint32_t now = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	mem::vector<LC_ServerMapLogic_Place*> kNeedReleaseList;		kNeedReleaseList.reserve(10);
	GameLogic::LC_IDSet::iterator IterTribuBe = m_kTributaryMapResIDs.begin();
	for(; IterTribuBe != m_kTributaryMapResIDs.end(); ++IterTribuBe)
	{
		kNeedReleaseList.clear();

		int32_t nMapResId = *IterTribuBe;
		LC_ServerMapLogicPlaceVector* pkMapLogicVec = GetMapLogicByRes(nMapResId);
		if (pkMapLogicVec == NULL || pkMapLogicVec->empty())
		{
			continue;
		}

		int i = 0;
		LC_ServerMapLogicPlaceVector::iterator IterPlaceBe = pkMapLogicVec->begin();
		for (; IterPlaceBe != pkMapLogicVec->end(); ++IterPlaceBe, i++)
		{
			LC_ServerMapLogic_Place* pkServerMapLogic = *IterPlaceBe;
			if (!pkServerMapLogic->GetTributaryAutoRelease())
				continue;

			int iCount = pkServerMapLogic->GetControllerPlayerCount();
			if (iCount <= 0 && i > 0)		// 不是第一张地图，第一张不删除的
			{
				uint32_t nClearTime = pkServerMapLogic->GetMap()->GetClearPlayerTime();
				if (nClearTime != 0 && now - nClearTime > expiration_time)
				{
					kNeedReleaseList.push_back(pkServerMapLogic);
				}
			}
		}

		mem::vector<LC_ServerMapLogic_Place*>::iterator IterReleaseBe = kNeedReleaseList.begin();
		for (; IterReleaseBe != kNeedReleaseList.end(); ++IterReleaseBe)
		{
			_releaseServerMapLogic(*IterReleaseBe);			
		}
	}
}

//------------------------------------------------------------------
LC_ServerMapLogic_Place* LC_ServerWorldManager::GetMapLogic(int32_t lMapLogicID)
{
	LC_ServerMapLogicIDPtrMap::iterator it = m_kLogicID2Logicmap.find(lMapLogicID);
	return (it != m_kLogicID2Logicmap.end()) ? it->second : NULL;
}

bool LC_ServerWorldManager::DelayMapLogicAutoRelease(int32_t lMapLogicID)
{
	LC_ServerMapLogic_Place* pkMapLogic = this->GetMapLogic(lMapLogicID);
	if (pkMapLogic != NULL)
	{
		pkMapLogic->GetMap()->ResetClearPlayerTime(2);	/// 延迟该地图无人时自动销毁的时间
		return true;
	}
	return false;
}
//------------------------------------------------------------------
void LC_ServerWorldManager::ReportOrigMapLineInfo()
{
	if ( GetGameServerApp()->GetToolMode() )
		return;
	UpdateAllMapLineTologin();
	MG_SS_MAP_LINE_STARTUP_REGISTER report;
	report.m_Data.reserve(m_mapLocalResLineData.size());

	MAP_LINE_TYPE_IT it = m_mapLocalResLineData.begin();
	for ( ; it != m_mapLocalResLineData.end(); ++it )
	{
		uint64_t nKey = it->first;
		int32_t nValue = it->second;

		MG_SS_MAP_LINE_REGISTER_DATA data;
		uint64_t lMapResID = 0;
		uint64_t lineNum = 0;
		LC_Helper::DecodeKey(nKey, lMapResID, lineNum );
		data.nMapResId = lMapResID;
		data.nOrigLineIndex = lineNum;
		data.nOrigLogicId = nValue;
		report.m_Data.push_back(data);
	}
	SendMsgToLogin(MGPT_SS_MAP_LINE_STARTUP_REGISTER, &report, 0, 0);
	return;
}

void LC_ServerWorldManager::SetGlobalMapLogicId_Lite( int32_t nServerNodeId, int32_t nOrigLogicId, int32_t nMapResId, int32_t nGlobalLine )
{
	int32_t nServerId = GetGameServerID();
	if ( nServerId != nServerNodeId )
		return;

	LC_MapBase* pMap = GetMap(nOrigLogicId);
	if ( pMap == NULL )
		return;
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "fenxian, SetGlobalMapLogicId_Lite: mapRes: %u origLogic: %u globalLine: %d", nMapResId, nOrigLogicId, nGlobalLine);
	pMap->SetGlobalLineNum( nGlobalLine );

}

LC_ServerMapLogicPlaceVector* LC_ServerWorldManager::GetMapLogicByRes(int32_t iMapResID)
{
	LC_ServerMapMapLogicIndices::iterator it = m_kLogicMapIndices.find(iMapResID);
	return (it != m_kLogicMapIndices.end()) ? (&it->second) : NULL;
}

//------------------------------------------------------------------
LC_ServerMapLogic_Place* LC_ServerWorldManager::GetMapLogicByResAndIndex(int32_t iMapResID, int iTributaryIndex)
{
	//TributaryIndex从1开始
	LC_ServerMapLogicPlaceVector* pkPlaceVec = GetMapLogicByRes(iMapResID);
	if (pkPlaceVec && iTributaryIndex > 0 && iTributaryIndex <= (int)pkPlaceVec->size())
	{
		return (*pkPlaceVec)[iTributaryIndex-1];
	}
	else
	{
		return NULL;
	}
}
LC_ServerMapLogic_Place* LC_ServerWorldManager::FindProperMapLogicByRes(int32_t iMapResID, bool bEnableTributary)
{
	LC_ServerMapMapLogicIndices::iterator itIndices = m_kLogicMapIndices.find(iMapResID);
	if (itIndices == m_kLogicMapIndices.end())
	{
		return NULL;
	}

	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(NULL == pkData)
	{
		return NULL;
	}

	int32_t iMaxFreeCount = -2000;
	LC_ServerMapLogic_Place* pkCandidate = NULL;
	if(pkData->_EnterMode == 0)
	{
		LC_ServerMapLogicPlaceVector & places = itIndices->second;
		for (LC_ServerMapLogicPlaceVector::iterator itPlace = places.begin(); itPlace != places.end(); ++itPlace)
		{
			LC_ServerMapLogic_Place* pkMapLogic = *itPlace;
			int freeCount = pkMapLogic->GetTributaryFreePlayerCount();
			if (freeCount > 0)
			{
				return pkMapLogic;
			}
			if (freeCount > iMaxFreeCount)
			{
				iMaxFreeCount = freeCount;
				pkCandidate = pkMapLogic;
			}
		}
		/// 没有合适的分线，看看是否需要创建一条
		if ( bEnableTributary
			&& iMaxFreeCount <= 0							/// 没有人数空闲的分线
			&& pkData->_AutoCreate == 1						/// 自动创建
			&& places.size() < pkData->_iTributaryMapCount	/// 分线数量未达上限
			)
		{
			bool bTributaryMap = m_kTributaryMapResIDs.find(iMapResID) != m_kTributaryMapResIDs.end();
			if (bTributaryMap)
			{
				LC_ServerMapLogic_Place * pkNewTributary = ForceCreateTributaryMap(iMapResID);
				if (pkNewTributary != NULL)			/// 创建成功才切过去，否则保留原值
				{
					pkNewTributary->SetTributaryAutoRelease(true);
					pkCandidate = pkNewTributary;
				}
			}
		}
	}
	else if(pkData->_EnterMode == 1)
	{
		LC_ServerMapTributaryRecords::iterator it = m_kTributaryRecords.find(iMapResID);
		if(it == m_kTributaryRecords.end())
		{
			for (LC_ServerMapLogicPlaceVector::iterator itPlace = itIndices->second.begin(); itPlace != itIndices->second.end(); ++itPlace)
			{
				LC_ServerMapLogic_Place* pkMapLogic = *itPlace;
				int freeCount = pkMapLogic->GetTributaryFreePlayerCount();
				if (freeCount > 0)
				{
					pkCandidate = pkMapLogic;
				}
				if (freeCount > iMaxFreeCount)
				{
					iMaxFreeCount = freeCount;
					pkCandidate = pkMapLogic;
				}

				if(NULL != pkCandidate)
				{
					break;
				}
			}
		}
		else
		{
			int nextIndex = it->second;
			do
			{
				nextIndex = (nextIndex+1)%itIndices->second.size();
				LC_ServerMapLogic_Place* pkMapLogic = itIndices->second[nextIndex];
				if(NULL != pkMapLogic)
				{
					int freeCount = pkMapLogic->GetTributaryFreePlayerCount();
					if (freeCount > 0)
					{
						pkCandidate = pkMapLogic;
					}
					if (freeCount > iMaxFreeCount)
					{
						iMaxFreeCount = freeCount;
						pkCandidate = pkMapLogic;
					}
				}
			}while(nextIndex!=it->second && pkCandidate==NULL);
		}
	}
	return pkCandidate;
}
//------------------------------------------------------------------
bool LC_ServerWorldManager::IsNewbee(LC_ServerPlayer* player)
{
	return GetGameServerApp()->GetUseNewPlayerMap() ? (player->GetLevel() < MIN_NOT_NEWBIE_LEVEL) : false;
}

LC_ServerMapLogic_Place* LC_ServerWorldManager::FindFreeNewbeeMapLogic(int32_t iMapResID, bool bEnableTributary)
{
	CF_WorldMapList::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (NULL == pkDataEntry)
	{
		LC_IDSet::const_iterator citID = m_kNewBeeResIDs.begin();
		for (; citID != m_kNewBeeResIDs.end(); ++citID)
		{
			return FindProperMapLogicByRes(*citID, bEnableTributary);
		}
		return NULL;
	}
	else
	{
		return FindProperMapLogicByRes(iMapResID, bEnableTributary);
	}
}
//------------------------------------------------------------------
bool LC_ServerWorldManager::IsTributaryMap(LC_ServerMapLogic_Place* pkPlace) const
{
	if (!IS_COMMON_MAP(pkPlace->GetMapLogicType()))
	{
		return false;
	}
	LC_ServerMapMapLogicIndices::const_iterator citPlaces = m_kLogicMapIndices.find(pkPlace->GetMapResID());
	return (citPlaces != m_kLogicMapIndices.end() && !citPlaces->second.empty()) ? (citPlaces->second.front() != pkPlace) : false;
}
//------------------------------------------------------------------

bool LC_ServerWorldManager::HasTributaryMap(int lMapResID) const
{
	CF_WorldMapList::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
	return pkDataEntry != NULL ? pkDataEntry->_iTributaryMapCount > 1 : false;
}

//------------------------------------------------------------------
LC_ServerMapLogic_Place* LC_ServerWorldManager::ForceCreateTributaryMap(int lMapResID)
{
	LC_ServerMapLogicPlaceVector* pkMapLogicVec = GetMapLogicByRes(lMapResID);
	if (pkMapLogicVec == NULL)
	{
		return NULL;
	}
	CF_WorldMapList::DataEntry* pkMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
	if (pkMapLogicVec->size() >= pkMapEntry->_iTributaryMapCount)	//分线数量达上限
	{
		return NULL;
	}

	//获取分线编号
	int32_t nFinalLineNo = CalcNewTributaryMapLineNo(pkMapLogicVec);

	LC_ServerMapLogic_Place* pkPlace = _createServerMapLogic(m_iMapLogicAllocID++, pkMapEntry->_iMapLogicType, pkMapEntry->_iID, pkMapEntry, true, nFinalLineNo);
	if(NULL == pkPlace)
	{
		GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "create distribute map failed [%s] LogicType[%d] ResID[%d] SizeType[%d]"
			, pkMapEntry->_kName.c_str(), pkMapEntry->_iMapLogicType, pkMapEntry->_iID, pkMapEntry->_iSizeType);
		return NULL;
	}
	return pkPlace;
}

int32_t LC_ServerWorldManager::CalcNewTributaryMapLineNo(LC_ServerMapLogicPlaceVector* pkMapLogicVec)
{
	/// 获取分线编号
	std::vector<int32_t>	kLineNos;
	kLineNos.reserve(pkMapLogicVec->size() + 1);
	kLineNos.push_back(-1);
	for (LC_ServerMapLogicPlaceVector::iterator it = pkMapLogicVec->begin(); it != pkMapLogicVec->end(); ++it)
	{
		LC_ServerMapLogic_Place * pkMapLogic = *it;
		kLineNos.push_back(pkMapLogic->GetMap()->GetLocalLineNum());
	}
	std::sort(kLineNos.begin(), kLineNos.end());		// 从小到大排序

	/// 查找到第一条不连续的编号
	int32_t	nFinalLineNo = (*kLineNos.rbegin()) + 1;
	size_t	nCount = kLineNos.size();
	for (size_t i = 1; i < nCount; ++i)
	{
		if (kLineNos[i] - kLineNos[i-1] > 1)
		{
			nFinalLineNo = kLineNos[i-1] + 1;
			break;
		}
	}
	return nFinalLineNo;
}

bool LC_ServerWorldManager::RestartMapLogicByRes( int32_t nMapResId )
{
	LC_ServerMapMapLogicIndices::iterator it = m_kLogicMapIndices.find( nMapResId );
	if ( it == m_kLogicMapIndices.end() )
		return false;

	LC_ServerMapLogicPlaceVector& vecLogic = it->second;
	int32_t nSize = vecLogic.size();
	for ( int i = 0; i < nSize; i++ )
	{
		if ( vecLogic[i] == NULL )
			continue;
		vecLogic[i]->RestartLogic();
	}
	
	return true;
}

bool LC_ServerWorldManager::FindGuildMapBossLogicMap(int32_t mapID, uint64_t guildID, LC_ServerMapLogicPlaceVector& logicPlaceVec)
{
	logicPlaceVec.clear();
	logicPlaceVec.reserve(3);
	LC_ServerMapLogicPlaceVector* pkPlaceVec = GetMapLogicByRes(mapID);
	if(NULL != pkPlaceVec)
	{
		for(LC_ServerMapLogicPlaceVector::iterator it=pkPlaceVec->begin(); it!=pkPlaceVec->end(); ++it)
		{
			LC_ServerMapLogic_Place* pkPlace = *it;
			if(NULL!=pkPlace && pkPlace->GetMapLogicType()==MT_GUILD_MAP && pkPlace->GetMapResID()==mapID && pkPlace->GetGuildID()==guildID)
			{
				logicPlaceVec.push_back(pkPlace);
			}
		}
	}

	return logicPlaceVec.empty() ? false : true;
}
//------------------------------------------------------------------
void LC_ServerWorldManager::BroadCast(int32_t iMsgType, Protocol::MG_BODY* pkMsgBody, bool bGlobal, int32_t nServerId, uint16_t nRouterFlag)
{
	if (bGlobal == false)
	{
		for (LC_MapIDPtrMapIter IterBe = m_kMapIDPtrMap.begin(); IterBe  != m_kMapIDPtrMap.end() ; ++IterBe)
		{
			LC_ServerMap* pkServerMap = (LC_ServerMap*)(IterBe->second);
			pkServerMap->BroadCastToAll(iMsgType, pkMsgBody);
		}
	}
	else
	{
		SendMsgToChat(iMsgType, pkMsgBody, 0, 0, nServerId, 0, nRouterFlag);
	}
}
void LC_ServerWorldManager::UpdateTributaryRecords(int resMapID, int index)
{
	LC_ServerMapTributaryRecords::iterator it = m_kTributaryRecords.find(resMapID);
	if(it != m_kTributaryRecords.end())
	{
		it->second = index;
	}
	else
	{
		m_kTributaryRecords.insert(std::make_pair(resMapID, index));
	}
}
//------------------------------------------------------------------
ActiveSectorClusterVector& LC_ServerWorldManager::SetUpActiveSector(void)
{
	LC_MapIDPtrMapIter IterBe = m_kMapIDPtrMap.begin();
	LC_MapIDPtrMapIter IterEd = m_kMapIDPtrMap.end();

	m_uiActiveSectorClusSize = m_kMapIDPtrMap.size();
	if (m_uiActiveSectorClusSize > m_kActiveSectorClusVector.size())
	{
		m_kActiveSectorClusVector.resize(m_uiActiveSectorClusSize);
	}

	size_t index = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		LC_ServerMap* pkServerMap = (LC_ServerMap*)(IterBe->second);
		m_kActiveSectorClusVector[index].Clear();
		pkServerMap->FillActiveSector(m_kActiveSectorClusVector[index]);
		++index;
	}

	return m_kActiveSectorClusVector;
}
//------------------------------------------------------------------------------------
ResultType LC_ServerWorldManager::CheckLeaveServerWorld(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, float fOfflineTime)
{
	ResultType eResult = RE_SUCCESS;
	int32_t lMapLogicID = pkPlayer->GetMapLogicID();
	LC_ServerMapLogic_Place* pkMapLogic	= GetMapLogic(lMapLogicID);
	if (NULL == pkMapLogic)
	{
		return eResult;
	}
	eResult = pkMapLogic->CheckLeaveMap(pkPlayer, eInOutType, fOfflineTime);
	return eResult;
}
//------------------------------------------------------------------------------------
ResultType LC_ServerWorldManager::RequestEnterServerWorld(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, MapType eTargetMapLogicType, int lTargetMapLogicID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation)
{
	ResultType eResult = RE_FAIL;
	//在不确定进入目的地的情况下,通过玩家身上的数据查找目的地
	if (eInOutType == LMIOT_RECOVER)
	{
		eResult = _requestChangePlayerLocationByRecover(pkPlayer);
		//如果不成功，将玩家放入野外地图
		if (eResult != RE_SUCCESS)
		{
			eResult = _requestChangePlayerLocationToDefault(pkPlayer, eInOutType);
		}
	}
	if (eInOutType == LMIOT_OFFLINE)
	{
		eResult = _requestChangePlayerLocationByOffLine(pkPlayer);
		//如果不成功，将玩家放入野外地图
		if (eResult != RE_SUCCESS)
		{
			eResult = _requestChangePlayerLocationToDefault(pkPlayer, eInOutType);
		}
	}
	return eResult;
}
//------------------------------------------------------------------------------------
ResultType LC_ServerWorldManager::RequestChangePlayerLocation(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation, int lineNum, bool bCertainSameServer)
{
	const unique_id_impl& instance = pkPlayer->GetInstance();
	int buddyId = pkPlayer->GetBuddyID();
	int mapResID = pkPlayer->GetMapResID();
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	CF_WorldMapList::DataEntry* pWorldMapListdata = pkCSVWorldMapList->GetEntryPtr(iTargetMapResID);
	if (pWorldMapListdata == NULL)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "RequestChangePlayerLocation pWorldMapListdata == NULL cid:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d (%d,%d,%d) ",instance.detail(),instance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID(),(int)eInOutType,lTargetMapLogicID,iTargetMapResID,lineNum,rkTargetLocation.x,rkTargetLocation.y,rkTargetLocation.z);
		return RE_FAIL;
	}
	MapType eTargetMapLogicType = (MapType)(pWorldMapListdata->_iMapLogicType);
	ResultType eResult = RE_FAIL;
	LC_WorldLocationAssetEntry* pkPermenentLocation = pkPlayer->GetWorldLocationAsset()->GetWorldLocationAssetEntryByIndex(WLT_PERMANENT);
	if (NULL == pkPermenentLocation )
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "RequestChangePlayerLocation NULL == pkPermenentLocation cid:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d (%d,%d,%d) ",instance.detail(),instance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID(),(int)eInOutType,lTargetMapLogicID,iTargetMapResID,lineNum,rkTargetLocation.x,rkTargetLocation.y,rkTargetLocation.z);
		return RE_FAIL;
	}
	int32_t lPermenentMapResID = pkPermenentLocation->GetMapResID();
	UT_Vec3Int kPermentLocation	= pkPermenentLocation->GetLocation();

	//设置玩家的maplogicid
	LC_PlayerGroupBase* pkPlayerGroup = pkPlayer->GetPlayerGroup();
	if (pkPlayerGroup != NULL)
	{
		RaidPortalEntry* raidportal = pkPlayerGroup->GetRaidPortal();
		if (raidportal != NULL)
		{
			if (iTargetMapResID == raidportal->m_Resid)
			{
				lTargetMapLogicID = raidportal->m_LogicId;
			}
		}
	}
	switch ( eInOutType)
	{
		case LMIOT_CLEANUP: //由地图逻辑将玩家清理出去
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, MT_NORMAL, 0, lPermenentMapResID, kPermentLocation, lineNum, bCertainSameServer);
				//如果不成功，将玩家放入到野外地图
				if (eResult != RE_SUCCESS)
				{
					eResult = _requestChangePlayerLocationToDefault(pkPlayer, eInOutType);
				}
			}
			break;
		case LMIOT_RETURN_CITY: //回城，由逻辑系统控制传送位置
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, MT_NORMAL, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_TRANSFER: //正常地图传送，由逻辑系统控制传送位置
			{
				//请求传送，判断出战buddy
				if(false == CheckAllowToFightBuddyId(buddyId,pWorldMapListdata->_ActiveBuddyIds) )
				{
					GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "RequestChangePlayerLocation tag = false cid:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d (%d,%d,%d) ",instance.detail(),instance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID(),(int)eInOutType,lTargetMapLogicID,iTargetMapResID,lineNum,rkTargetLocation.x,rkTargetLocation.y,rkTargetLocation.z);
					return RE_FAIL;
				}
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_GROW_UP: //新手成长,坐标和地图不变
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, MT_NORMAL, 0, lPermenentMapResID, kPermentLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_CURRENT_RESPAWN: //原地复活,由逻辑系统控制传送位置
			{
				int lineNum = pkPlayer->GetLocalLineNum();
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_SAFE_RESPAWN: //原地复活,由逻辑系统控制传送位置
			{
				int lineNum = pkPlayer->GetLocalLineNum();
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
				//如果不成功，将玩家放入到野外地图
				if (eResult != RE_SUCCESS)
				{
					eResult = _requestChangePlayerLocationToDefault(pkPlayer, eInOutType);
				}
			}
			break;
		case LMIOT_SKILL_RESPAWN: //技能复活，由逻辑系统控制传送位置
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_FIRST_ENTER_TRANSCRIPTION: //第一次登录进入副本
			{
				eResult = _requestChangePlayerLocationByFirstEnterRaid(pkPlayer, LMIOT_TRANSFER, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, UT_Vec3Int());
			}
			break;
		case LMIOT_QUIT_TRANSCRIPTION://副本退出,恢复永久坐标
			{
				//请求退出地图
				// quit 副本 相关结算信息
				QuitTranscriptionOp(pkPlayer);
				int32_t iQuitMapResID = lPermenentMapResID;
				UT_Vec3Int kQuitLocation = kPermentLocation;
				if (UT_ServerHelper::IsInCross(pkPlayer))
					UT_ServerHelper::GetFrontierLocation(pkPlayer,kQuitLocation,iQuitMapResID);
				if (iQuitMapResID == 0)
					iQuitMapResID = iTargetMapResID;
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, MT_NORMAL, 0, iQuitMapResID, kQuitLocation, lineNum, bCertainSameServer);
			}
			break;
		default:
			break;
	}

	//复活，不能清空战斗数据记录
	if(LMIOT_SAFE_RESPAWN != eInOutType && LMIOT_CURRENT_RESPAWN!=eInOutType)
	{
		pkPlayer->ClearPlayerCombatDataMap();
	}

	if(RE_SUCCESS == eResult)
	{
		if(LMIOT_FIRST_ENTER_TRANSCRIPTION==eInOutType || LMIOT_TRANSFER==eInOutType)
		{
			LC_RaidAssert* pkRaidAsset = pkPlayer->GetRaidAsset();
			if(NULL!=pkRaidAsset)
			{
				if(pkRaidAsset->UpdateEnterMapTimes(iTargetMapResID))
				{
					pkPlayer->SyncRaidEnterMapRecord(iTargetMapResID);
				}				
			}
		}
		
		CF_WorldMapList::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iTargetMapResID);
		if(LMIOT_SAFE_RESPAWN!=eInOutType && LMIOT_CURRENT_RESPAWN!=eInOutType && 
			(pkEntry->_iMapLogicType==MT_REANSCRIPTION_RANK || pkEntry->_iMapLogicType==MT_GUILD_MAP || pkEntry->_iMapLogicType==MT_GUILD_MAP2) )
		{
			pkPlayer->EnableCombatRecorder(pkEntry->_iMapLogicType);
		}
	}
	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_INFO, "RequestChangePlayerLocation success cid:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d (%d,%d,%d) ",instance.detail(),instance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID(),(int)eInOutType,lTargetMapLogicID,iTargetMapResID,lineNum,rkTargetLocation.x,rkTargetLocation.y,rkTargetLocation.z);
	return eResult;
}
//------------------------------------------------------------------------------------
ResultType LC_ServerWorldManager::RequestLeaveServerWorld(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType)
{
	if (LMIOT_CLEANUP != eInOutType)
	{
		//如果是逻辑清下去的，不要再通知地图了
		int lMapLogicID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkMapLogic = GetMapLogic(lMapLogicID);
		if (pkMapLogic == NULL)
		{
			return RE_FAIL;
		}
		pkMapLogic->RequestLeaveMap(pkPlayer, eInOutType, pkMapLogic->GetMapResPtr());
	}
	pkPlayer->CancelCurrentSkill();
	if(!RemoveObjectFromMap(pkPlayer))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "fail to remove player from map", pkPlayer->GetID(), pkPlayer->GetInstance(), pkPlayer->GetMapResID(), pkPlayer->IsValid());
	}
	return RE_SUCCESS;
}
bool LC_ServerWorldManager::CheckAllowToFightBuddyId(int buddyId, Utility::UT_SIMDataList&_ActiveBuddyIds)
{
	if(_ActiveBuddyIds.empty())
		return true;
	for(Utility::UT_SIMDataList::iterator it=_ActiveBuddyIds.begin(); it!=_ActiveBuddyIds.end(); ++it)
	{
		if(buddyId == it->IID())
			return true;
	}
	return false;
}
void LC_ServerWorldManager::QuitTranscriptionOp(LC_ServerPlayer* pkPlayer)
{
	int mapResID = pkPlayer->GetMapResID();
	//是否有掉落物
	mem::vector<object_id_type> placeHolderLogicID;
	if(pkPlayer->GetPlaceHolder(mapResID, placeHolderLogicID))
	{
		LC_ServerPlaceHolderManager* pkPlaceHolderManager = SERVER_GET_PLACEHOLDER_MANAGER();
		MG_RLT_PickUpPlaceHolder kMsg;
		for (mem::vector<object_id_type>::iterator itc = placeHolderLogicID.begin(); itc != placeHolderLogicID.end() ; ++itc)
		{
			LC_ServerPlaceHolder* pkPlaceHolder = (LC_ServerPlaceHolder*)(pkPlaceHolderManager->FindPlaceHolder(*itc));
			if ( NULL == pkPlaceHolder )
				continue;
			pkPlaceHolder->RequestPickupItem(pkPlayer, kMsg, false);
		}
		pkPlayer->RemoveMapPlaceHolder(mapResID);
	}
	if(false ==  pkPlayer->IsMainController())
		return;
	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	//清除浩劫秘境怪物的击杀归属
	for(CF_RegularBoss::DataEntryMapExternIterator kIter = CF_RegularBoss::GetSingletonPtr()->GetIterator(); kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_RegularBoss::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(mapResID != pkData->_lMapResID)
			continue;
		int mapLogicID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(mapLogicID);
		if(NULL == pkPlace)
			continue;
		LC_ServerMapLogic_ActorManger& ActorMgr = pkPlace->GetActorManger();
		object_id_vector& npcList = ActorMgr.GetNpcList();
		for(object_id_vector::iterator it=npcList.begin(); it!=npcList.end(); ++it)
		{
			LC_ServerNPC* pkNpc = (LC_ServerNPC*)pkNPCManager->FindNPC(*it);
			if(NULL != pkNpc 
			&& pkNpc->GetCharType() == pkData->_lCharType
			&& pkNpc->GetHitPlayerID() == pkPlayer->GetOwnerObjectID())
			{
				pkNpc->SetHitPlayer(0);
				pkNpc->ClearOwnerList();
				break;
			}
		}
		break;		
	}
}

//------------------------------------------------------------------------------------
LC_ServerMapLogic_Place* LC_ServerWorldManager::CreateServerMapLogic(int lMapLogicID, int lMapResID, int lMapLogicType, int32_t lineNum ,bool start)
{
	int lServerMapLogicID  = 0;
	if (lMapLogicID > 0)
	{
		lServerMapLogicID = lMapLogicID;
	}
	else
	{
		lServerMapLogicID = m_iMapLogicAllocID++;
	}
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	CF_WorldMapList::DataEntry* data = pkCSVWorldMapList->GetEntryPtr(lMapResID);
	if (data == NULL)
	{
		return NULL;
	}
	return _createServerMapLogic(lServerMapLogicID, lMapLogicType, lMapResID, data, start, lineNum);
}
//-------------------------------------------------------------------------------
void LC_ServerWorldManager::ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent)
{
	if (LC_ServerMapLogic_Place::_bCareOfEvent(pkEvent) == true)
	{
		for (LC_ServerMapLogicIDPtrMap::iterator it = m_kLogicID2Logicmap.begin(), itEnd=m_kLogicID2Logicmap.end(); 
			it != itEnd; ++it)
		{
			LC_ServerMapLogic_Place* pkServerMapLogic = (*it).second;
			if (pkServerMapLogic->IsActive())
			{
				pkServerMapLogic->ProcessGameEvent(pkEvent);
			}
		}
	}
}
//------------------------------------------------------------------
ResultType LC_ServerWorldManager::_requestChangePlayerLocationByRecover(LC_ServerPlayer* pkPlayer)
{
	//根据进入进出类型的不同，选择恢复哪部分坐标
	bool bTestRecoverPermenentLocation = true;
	bool bTestRecoverTemporaryLocation = false;

	LC_WorldLocationAsset* pkWorldLocationAsset = pkPlayer->GetWorldLocationAsset();

	int		lTargetMapLogicID			= 0;
	int		lTargetMapResID				= 0;
	int		lTargetMapLogicType			= 0;
	UT_Vec3Int	kTargetLocation	;
	ResultType	eChangeResult				= RE_FAIL;
	LC_ServerMapLogic_Place* pkTargetMapLogic = NULL;

	if (bTestRecoverTemporaryLocation)
	{
		//首先尝试恢复临时坐标数据
		LC_WorldLocationAssetEntry* pkTemporaryEntry = pkWorldLocationAsset->GetWorldLocationAssetEntryByIndex(WLT_TEMPORARY);
		if (NULL == pkTemporaryEntry)
		{
			return RE_FAIL;
		}
		if (pkTemporaryEntry->GetValid())
		{
			lTargetMapLogicID		= pkTemporaryEntry->GetMapLogicID();
			lTargetMapResID			= pkTemporaryEntry->GetMapResID();
			lTargetMapLogicType		= pkTemporaryEntry->GetMapLogicType();
			kTargetLocation			= pkTemporaryEntry->GetLocation();

			pkTargetMapLogic = _findProperMapLogic(pkPlayer, LMIOT_RECOVER, lTargetMapLogicType, lTargetMapLogicID, lTargetMapResID);
			if (NULL == pkTargetMapLogic)
			{
				eChangeResult = RE_FAIL;
			}
			else
			{
				eChangeResult = pkTargetMapLogic->CheckEnterMap(pkPlayer, LMIOT_RECOVER);
				if (eChangeResult == RE_SUCCESS)
				{
					kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, LMIOT_RECOVER, kTargetLocation);
					eChangeResult = pkTargetMapLogic->RequestEnterMap(pkPlayer, LMIOT_RECOVER, kTargetLocation);
				}
			}
		}
		//恢复成功
		if (eChangeResult == RE_SUCCESS)
		{
			return eChangeResult;
		}
	}

	if (bTestRecoverPermenentLocation)
	{
		eChangeResult = RE_FAIL;
		//继续尝试恢复永久坐标数据
		LC_WorldLocationAssetEntry* pkPermanentEntry = pkWorldLocationAsset->GetWorldLocationAssetEntryByIndex(WLT_PERMANENT);
		if (NULL == pkPermanentEntry)
		{
			return RE_FAIL;
		}
		if (pkPermanentEntry->GetValid())
		{
			lTargetMapLogicID		= pkPermanentEntry->GetMapLogicID();
			lTargetMapResID			= pkPermanentEntry->GetMapResID();
			lTargetMapLogicType		= pkPermanentEntry->GetMapLogicType();
			kTargetLocation			= pkPermanentEntry->GetLocation();
			GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "kTargetLocation x:%d, y:%d, y:%d\n ", kTargetLocation.x, kTargetLocation.y,kTargetLocation.z);
			pkTargetMapLogic = _findProperMapLogic(pkPlayer, LMIOT_RECOVER, lTargetMapLogicType, lTargetMapLogicID, lTargetMapResID);
			if (NULL == pkTargetMapLogic)
			{
				eChangeResult = RE_FAIL;
			}
			else
			{
				eChangeResult = pkTargetMapLogic->CheckEnterMap(pkPlayer, LMIOT_RECOVER);
				if (eChangeResult == RE_SUCCESS)
				{
					UT_Vec3Int nTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, LMIOT_RECOVER, kTargetLocation);
					GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "kTargetLocation x:%d, y:%d, y:%d\n ", nTargetLocation.x, nTargetLocation.y,nTargetLocation.z);
					eChangeResult = pkTargetMapLogic->RequestEnterMap(pkPlayer, LMIOT_RECOVER, nTargetLocation);
				}
			}
		}
	}
	return eChangeResult;
}

//------------------------------------------------------------------
ResultType LC_ServerWorldManager::_requestChangePlayerLocationByOffLine(LC_ServerPlayer* pkPlayer)
{
	int		lTargetMapLogicID			= pkPlayer->GetMapLogicID();
	int		lTargetMapResID				= pkPlayer->GetMapResID();
	int		lTargetMapLogicType			= pkPlayer->GetMapLogicType();
	UT_Vec3Int	kTargetLocation				= pkPlayer->GetCurrentLocation();
	ResultType	eChangeResult				= RE_FAIL;
	LC_ServerMapLogic_Place* pkTargetMapLogic = NULL;

	pkTargetMapLogic = _findProperMapLogic(pkPlayer, LMIOT_OFFLINE, lTargetMapLogicType, lTargetMapLogicID, lTargetMapResID);
	if (NULL == pkTargetMapLogic)
	{
		eChangeResult = RE_FAIL;
	}
	else
	{
		eChangeResult = pkTargetMapLogic->CheckEnterMap(pkPlayer, LMIOT_OFFLINE);
		if (eChangeResult == RE_SUCCESS)
		{
			kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, LMIOT_OFFLINE, kTargetLocation);
			eChangeResult = pkTargetMapLogic->RequestEnterMap(pkPlayer, LMIOT_OFFLINE, kTargetLocation);
		}
	}
	//恢复成功
	if (eChangeResult == RE_SUCCESS)
	{
		return eChangeResult;
	}
	return eChangeResult;
}
//------------------------------------------------------------------
ResultType LC_ServerWorldManager::_requestChangePlayerLocationByLogic(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicType, int lTargetMapLogicID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation, int lineNum, bool bCertainSameServer)
{
	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if( NULL == pkOwnerPlayer)
	{
		const unique_id_impl& playerInstance = pkPlayer->GetInstance();
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic NULL == pkOwnerPlayer c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",playerInstance.detail(),playerInstance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID(),eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
		return RE_FAIL;
	}
	const unique_id_impl& playerInstance = pkOwnerPlayer->GetInstance();
	int userID = pkOwnerPlayer->GetUserID();
	int uID = pkOwnerPlayer->GetID();
	LC_ServerMapLogic_Place* pkTargetMapLogic = NULL;
	LC_ServerMapLogic_Place* pkSourceMapLogic = NULL;
	ResultType  eChangeResult = RE_FAIL;
	UT_Vec3Int	kTargetLocation;
	bool		bQuitHookMap2CommonMap = false;

	pkPlayer->GetRotaryTableAsset().SetRotaryTableCDStartTime(0);

	//玩家此时应该在世界中
	pkSourceMapLogic = GetMapLogic(pkPlayer->GetMapLogicID());

	//ret:在非global节点，请求进入公会秘境地图
	bool ret = UT_ServerHelper::CheckChangeServer(LMIOT_TRANSFER, lTargetMapLogicType, bCertainSameServer);
	int32_t srcMapLogicID = pkSourceMapLogic==NULL ? 0 : pkSourceMapLogic->GetMapResID();

	//进入普通地图，需要区分新手和老手
	if (IS_COMMON_MAP(lTargetMapLogicType) || lTargetMapLogicType == MT_HOOK_MAP)
	{
		int currentResID = pkPlayer->GetMapResID();
		if (lTargetMapLogicID == 0 && iTargetMapResID == currentResID)	// 同地图切换，比如飞鞋
		{
			pkTargetMapLogic = pkSourceMapLogic;
		}
		else if (pkSourceMapLogic != NULL && pkSourceMapLogic->GetMapLogicType() == MT_HOOK_MAP)	// 从挂机荒野退出到野外，不属于 LMIOT_QUIT_TRANSCRIPTION
		{
			bQuitHookMap2CommonMap = true;			
		}
		//查找到一张合适的普通地图
		if (NULL == pkTargetMapLogic)
		{
			pkTargetMapLogic = _findProperMapLogic(pkPlayer, eInOutType, lTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, lineNum);
		}
		if (NULL == pkTargetMapLogic)
		{
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic NULL == pkTargetMapLogic r1 c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
			return RE_FAIL;
		}
	}
	else if (MT_TRANSCRIPTION_BEGIN < lTargetMapLogicType && MT_TRANSCRIPTION_END > lTargetMapLogicType)
	{
		int currentResID = pkPlayer->GetMapResID();
		CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(currentResID);
		if(NULL != pkData 
		&& 1 == pkData->_iShowRaidStage  
		&& eInOutType != LMIOT_SAFE_RESPAWN 
		&& eInOutType != LMIOT_CURRENT_RESPAWN)
		{
			
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic 1 == pkData->_iShowRaidStage c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
			return RE_FAIL;
		}

		//创建地图
		pkTargetMapLogic = _findProperMapLogic(pkPlayer, eInOutType, lTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, lineNum);
		if ( NULL == pkTargetMapLogic)
		{
			MG_Message rltMsg;
			rltMsg.m_iMessageID = RE_TRANSCRIPTION_FULL;
			rltMsg.m_iParameter1 = lTargetMapLogicID;
			rltMsg.m_iParameter2 = iTargetMapResID;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &rltMsg);
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic NULL == pkTargetMapLogic r2  c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
			return RE_FAIL;
		}
	}
	else
	{
		pkTargetMapLogic = _findProperMapLogic(pkPlayer, eInOutType, lTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, lineNum);
		if (NULL == pkTargetMapLogic 
			&& false == ret)
		{
			MG_Message rltMsg;
			rltMsg.m_iMessageID = RE_TRANSCRIPTION_FULL;
			rltMsg.m_iParameter1 = lTargetMapLogicID;
			rltMsg.m_iParameter2 = iTargetMapResID;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &rltMsg);
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic NULL == pkTargetMapLogic r3 c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
			return RE_FAIL;
		}
	}

	if(NULL == pkTargetMapLogic)
	{
		if(false == UT_ServerHelper::TrySwitchMap(pkPlayer, eInOutType, lTargetMapLogicType, srcMapLogicID, 0, iTargetMapResID, 0, 0, 0, bQuitHookMap2CommonMap, bCertainSameServer))
		{
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic NULL == pkTargetMapLogic r4 c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
			return RE_FAIL;
		}
	}
	else
	{
		//同地图传送
		if (pkSourceMapLogic != NULL 
		&& pkSourceMapLogic->GetMapLogicID() == pkTargetMapLogic->GetMapLogicID() 
		&& !pkPlayer->GetReliveBeforeEnterMapTag())
		{
			eChangeResult = pkTargetMapLogic->CheckChangeLocation(pkPlayer, eInOutType);
			if (eChangeResult != RE_SUCCESS)
			{
				GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic 1 eChangeResult:%d ,c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",eChangeResult,playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
				return eChangeResult;
			}
			//计算进入坐标
			kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, eInOutType, rkTargetLocation);
			pkTargetMapLogic->RequestChangeLocation(pkPlayer, eInOutType, kTargetLocation);
		}
		else
		{
			//目标地图是否可以进入
			if ( !bCertainSameServer )
			{
				eChangeResult = pkTargetMapLogic->CheckEnterMap(pkPlayer, eInOutType);
				if (eChangeResult != RE_SUCCESS)
				{
					GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic 2 eChangeResult:%d ,c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",eChangeResult,playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
					return eChangeResult;
				}
			}
			if (eInOutType != LMIOT_CLEANUP)
			{
				//当前地图是否可以离开
				if (pkSourceMapLogic != NULL)
				{
					eChangeResult = pkSourceMapLogic->CheckLeaveMap(pkPlayer, eInOutType, 0.0f);
					if (eChangeResult != RE_SUCCESS)
					{
						GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic 3 eChangeResult:%d ,c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",eChangeResult,playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
						return eChangeResult;
					}
				}
			}

			//计算进入坐标
			kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, eInOutType, rkTargetLocation);

			int32_t destMapLogicID = pkTargetMapLogic==NULL ? 0 : pkTargetMapLogic->GetMapLogicID();
			bool srcIsShowRaidStage = pkSourceMapLogic==NULL ? false : pkSourceMapLogic->IsShowRaidStage();
			bool iDestIsShowRaidStage = pkTargetMapLogic==NULL ? false : pkTargetMapLogic->IsShowRaidStage();
			// 切地图是否要跨服；客户端请求的时候，bCertainSameServer=false, login回过来的时候，bCertainSameServer=true；如果一直是false，消息会始终在GS和Login中循环
			//进入或者退出相位副本，不调用TrySwitchMap
			if(srcIsShowRaidStage || iDestIsShowRaidStage || !UT_ServerHelper::TrySwitchMap(pkPlayer, eInOutType, lTargetMapLogicType, srcMapLogicID, destMapLogicID, iTargetMapResID, kTargetLocation.x, kTargetLocation.y, kTargetLocation.z, bQuitHookMap2CommonMap, bCertainSameServer))
			{
				if (eInOutType != LMIOT_CLEANUP)
				{
					//从当前地图离开
					if (pkSourceMapLogic != NULL)
					{
						ResultType ret = pkSourceMapLogic->RequestLeaveMap(pkPlayer, eInOutType, pkSourceMapLogic->GetMapResPtr());
						if (ret != RE_SUCCESS)
						{
							GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "_requestChangePlayerLocationByLogic  ret:%d ,c_id:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d ",ret,playerInstance.detail(),playerInstance.catagory(),userID,uID,eInOutType,lTargetMapLogicType,lTargetMapLogicID,iTargetMapResID,lineNum);
							return ret;
						}
					}
				}
				//从目标地图进入,考虑到可能已经跨服离开
				pkTargetMapLogic->RequestEnterMap(pkPlayer, eInOutType, kTargetLocation);
				UT_ServerHelper::AddAllBuffIdToSingleGuildMember(pkPlayer->GetGuildID(),pkPlayer->GetInstance());
			}
		}
	}

	if(pkPlayer->GetReliveBeforeEnterMapTag())
	{
		pkPlayer->SetReliveBeforeEnterMapTag(false);
	}

	//退出地图，通知任务系统存活buddy数量
	if(false ==  bCertainSameServer
	&& pkPlayer->IsMainController()
	&& (eInOutType==LMIOT_QUIT_TRANSCRIPTION || eInOutType==LMIOT_RETURN_CITY) )
	{
		LC_GameEventManager* pkGameEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkGameEventManager->AllocGameEvent();
		if (NULL != pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_ALIVE_BUDDYNUM);
			pkGameEvent->SetEventParam(EP_ALIVEBUDDY_PLAYER_ID, pkPlayer->GetID());
			pkGameEvent->SetEventParam(EP_ALIVEBUDDY_MAPID, pkSourceMapLogic->GetMapResID());
			pkGameEvent->SetEventParam(EP_ALIVEBUDDY_NUM, pkPlayer->GetAliveBuddySize());
			pkGameEventManager->InsertGameEvent(pkGameEvent);
		}
	}

	return eChangeResult;
}
//------------------------------------------------------------------
ResultType LC_ServerWorldManager::_requestChangePlayerLocationByFirstEnterRaid(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicType, int lTargetMapLogicID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation)
{
	LC_ServerMapLogic_Place* pkTargetMapLogic = NULL;
	ResultType  eChangeResult				= RE_FAIL;
	pkTargetMapLogic = _findProperMapLogic(pkPlayer, eInOutType, lTargetMapLogicType, lTargetMapLogicID, iTargetMapResID);
	if (pkTargetMapLogic == NULL)
	{
		MG_Message rltMsg;
		rltMsg.m_iMessageID = RE_TRANSCRIPTION_FULL;
		rltMsg.m_iParameter1 = lTargetMapLogicID;
		rltMsg.m_iParameter2 = iTargetMapResID;
		pkPlayer->SendMsgToClient(MGPT_MESSAGE, &rltMsg);

		const unique_id_impl& instance = pkPlayer->GetInstance();
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "RE_TRANSCRIPTION_FULL guildId:%lld, c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d, iTargetMapResID:%d",
			pkPlayer->GetGuildID(), instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkPlayer->GetID(), iTargetMapResID);
		return eChangeResult;
	}
	//目标地图是否可以进入
	eChangeResult = pkTargetMapLogic->CheckEnterMap(pkPlayer, eInOutType);
	if (eChangeResult != RE_SUCCESS)
	{
		return eChangeResult;
	}
	//计算进入坐标
	UT_Vec3Int	kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, eInOutType, rkTargetLocation);
	//从目标地图进入
	pkTargetMapLogic->RequestEnterMap(pkPlayer, eInOutType, kTargetLocation);
	return eChangeResult;
}

//------------------------------------------------------------------
ResultType LC_ServerWorldManager::_requestChangePlayerLocationToDefault(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType)
{
	LC_ServerMapLogic_Place* pkTargetMapLogic = NULL;
	ResultType  eChangeResult			= RE_FAIL;

	//查找到一张合适的普通地图
	pkTargetMapLogic = _findProperMapLogic(pkPlayer, eInOutType, MT_NORMAL, 0, 1);
	if (NULL == pkTargetMapLogic)
	{
		return RE_FAIL;
	}
	eChangeResult = pkTargetMapLogic->CheckEnterMap(pkPlayer, eInOutType);
	if (eChangeResult == RE_SUCCESS)
	{
		UT_Vec3Int	kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, eInOutType, UT_Vec3Int());
		eChangeResult = pkTargetMapLogic->RequestEnterMap(pkPlayer, eInOutType, kTargetLocation);
	}
	return eChangeResult;
}
//------------------------------------------------------------------
LC_ServerMapLogic_Place* LC_ServerWorldManager::_findProperMapLogic(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lMapLogicType, int lMapLogicID, int lMapResID, int lineNum)
{
	LC_ServerMapLogic_Place* placeResult = NULL;
	if (MT_NORMAL == lMapLogicType && IsNewbee(pkPlayer))
	{
		lMapLogicType = MT_NEWBEE;
	}

	//异种降临，必须在global节点查找地图
	if(lMapLogicType==MT_GUILD_MAP2 && !GetGameServerApp()->IsNodeType_Global())
	{
		return NULL;
	}

	if (IS_COMMON_MAP(lMapLogicType) || lMapLogicType == MT_HOOK_MAP)
	{
		if (lMapLogicID > 0 && lMapResID > 0)
		{
			LC_ServerMapLogic_Place* pkPlace = GetMapLogic(lMapLogicID);
			if(pkPlace != NULL && pkPlace->GetMapResID() == lMapResID)
			{
				placeResult = pkPlace;
				goto Exit0;
			}
		}
		placeResult =  FindFreeNewbeeMapLogic(lMapResID, true);
		goto Exit0;
	}
	else if (lMapLogicType > MT_ARENA_BEGIN && lMapLogicType < MT_ARENA_END)
	{
		return GetMapLogic(lMapLogicID);
	}
	else if (lMapLogicType == MT_GUILDWAR)
	{
		return GetMapLogic(lMapLogicID);
	}
	else if (lMapLogicType == MT_GUILD_LADDER_WAR)
	{
		return GetMapLogic(lMapLogicID);
	}
	else if(lMapLogicType==MT_GUILD_MAP)
	{
		if(INVALID_GUILD_ID == pkPlayer->GetGuildID())
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_findProperMapLogic type:%d r1", lMapLogicType);
			return NULL;
		}

		LC_ServerMapLogic_Place* pkPlace = NULL;
		for (LC_ServerMapLogicIDPtrMap::iterator it = m_kLogicID2Logicmap.begin(), itEnd=m_kLogicID2Logicmap.end(); 
			it != itEnd; ++it)
		{			
			LC_ServerMapLogic_Place* pkServerMapLogic = (*it).second;
			if( NULL == pkServerMapLogic || pkServerMapLogic->GetMapResID()!=lMapResID)
			{
				continue;
			}
			LC_MapBase* pkMap = pkServerMapLogic->GetMap();
			if(NULL==pkMap)
			{
				continue;
			}
			if(pkServerMapLogic->GetGuildID() != pkPlayer->GetGuildID())
			{
				continue;
			}

			pkPlace = pkServerMapLogic;
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_findProperMapLogic type:%d find vaild map! mapID:%d, mapLogicID:%d", lMapLogicType, lMapResID, pkPlace->GetMapLogicID());
			break;
		}

		return pkPlace;
	}
	else if (lMapLogicType > MT_TRANSCRIPTION_BEGIN && lMapLogicType < MT_TRANSCRIPTION_END || lMapLogicType==MT_WATCHTOWER || lMapLogicType==MT_GUILD_MAP2 || MT_GUILD_PARTYFUBEN == lMapLogicType)
	{
		//副本类型必须要有规则
		LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
		CF_TranscriptionRule::DataEntry* pkEntry = rulemanager->GetRule(lMapResID);
		if (pkEntry == NULL)
		{
			return NULL;
		}
		if (lMapLogicID > 0 && MT_GUILD_PARTYFUBEN != lMapLogicType)
		{
			//指定具体地图
			return GetMapLogic(lMapLogicID);
		}
		else
		{
			switch (pkEntry->_EnterType)
			{
				case 0:
					{
						return CreateServerMapLogic(0, lMapResID, lMapLogicType);
					}
					break;
				case 1:
					{
						for (LC_ServerMapLogicIDPtrMap::iterator it = m_kLogicID2Logicmap.begin(), itEnd=m_kLogicID2Logicmap.end(); it != itEnd; ++it)
						{
							LC_ServerMapLogic_Place* pkServerMapLogic = (*it).second;
							if(NULL == pkServerMapLogic)
							{
								continue;
							}

							LC_MapBase* pkMap = pkServerMapLogic->GetMap();
							if(NULL==pkMap)
							{
								continue;
							}

							if (pkServerMapLogic->IsEnterLocked() || pkServerMapLogic->GetMapResID() != lMapResID || pkMap->GetLocalLineNum()!=lineNum)
							{
								continue;
							}

							return pkServerMapLogic;
						}
						return CreateServerMapLogic(0, lMapResID, lMapLogicType);
					}
					break;
				case 2:
					{
						//指定具体地图
						for (LC_ServerMapLogicIDPtrMap::iterator it = m_kLogicID2Logicmap.begin(), itEnd=m_kLogicID2Logicmap.end(); 
							it != itEnd; ++it)
						{
							LC_ServerMapLogic_Place* pkServerMapLogic = (*it).second;
							if(NULL == pkServerMapLogic)
							{
								continue;
							}

							if (pkServerMapLogic->GetMapResID() != lMapResID || pkServerMapLogic->IsEnterLocked() || (pkServerMapLogic->GetMapLogicType()!=MT_WATCHTOWER && pkServerMapLogic->IsPlayerFull()))
							{
								continue;
							}

							if (pkServerMapLogic->IsEnterLocked() || ( (pkServerMapLogic->GetMapLogicType()!=MT_WATCHTOWER || MT_GUILD_PARTYFUBEN !=pkServerMapLogic->GetMapLogicType()) && pkServerMapLogic->IsPlayerFull()))
							{
								continue;
							}

							return pkServerMapLogic;
						}
					}
					break;
				case 3:
					{
						//有空地图，进  
						//没空地图，创建
						int currentLineCount = 0;
						int32_t createLineNum = 0;
						bool changeLineNum = false;
						mem::set<int32_t> AllLineNum;//现在创建的所有的地图num
						mem::set<LC_ServerMapLogic_Place*> pkPlace;//现在所有未满的地图，
						LC_ServerMapLogic_Place* pkFirstPlace;//linenum=0的地图
						for (LC_ServerMapLogicIDPtrMap::iterator it = m_kLogicID2Logicmap.begin(), itEnd=m_kLogicID2Logicmap.end(); 
							it != itEnd; ++it)
						{			
							LC_ServerMapLogic_Place* pkServerMapLogic = (*it).second;
							if(NULL == pkServerMapLogic)
							{
								continue;
							}

							LC_MapBase* pkMap = pkServerMapLogic->GetMap();
							if(NULL==pkMap)
							{
								continue;
							}

							if (pkServerMapLogic->IsEnterLocked() || pkServerMapLogic->GetMapResID()!=lMapResID)
							{
								continue;
							}

							if(pkServerMapLogic->GetGuildID()!=0 &&  pkServerMapLogic->GetGuildID()!= pkPlayer->GetGuildID())
							{
								continue;
							}

							++currentLineCount;

							int playerResID = pkPlayer->GetMapResID();
							if(pkServerMapLogic->GetMapResID() == playerResID)
							{
								changeLineNum = true;
							}

							//切线，直接进入选的线
							int num = pkMap->GetLocalLineNum();
							if(changeLineNum && num==lineNum)
							{
								return pkServerMapLogic;
							}

							if(num == 0)
							{
								pkFirstPlace = pkServerMapLogic;
							}

							AllLineNum.insert(num);

							//未满的地图
							bool tag = pkServerMapLogic->GetTributaryFreePlayerCount() <= 0;
							if(!tag)
							{
								pkPlace.insert(pkServerMapLogic);
							}
						}

						if(changeLineNum)
						{
							createLineNum = lineNum;
						}
						else
						{
							//有空线，进第一个空线
							if(!pkPlace.empty())
							{
								return *pkPlace.begin();
							}
							else if(!AllLineNum.empty())
							{//没空线，找当前最小的线号，创建
								int maxLineNum = *AllLineNum.rbegin()+1;
								for(int i=0; i<=maxLineNum; ++i)
								{
									mem::set<int32_t>::iterator findIt = AllLineNum.find(i);
									if(findIt == AllLineNum.end())
									{
										createLineNum = i;
										break;
									}
								}
							}
						}

						CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
						if(pkData->_iTributaryMapCount == currentLineCount)
						{
							//创建的线已经达到了最大值，进1线
							return pkFirstPlace;
						}
						else
						{
							pkFirstPlace = CreateServerMapLogic(0, lMapResID, lMapLogicType, createLineNum);
							if(NULL != pkFirstPlace && pkFirstPlace->GetMapLogicType()==MT_GUILD_MAP2)
							{
								pkFirstPlace->SetGuildID(pkPlayer->GetGuildID());
							}
							return pkFirstPlace;
						}
					}
					break;
				default:
					break;
			}
		}
		return NULL;
	}

Exit0:
	if(NULL != placeResult)
	{
		int mapLogicID = placeResult->GetMapLogicID();
		int mapResID = placeResult->GetMapResID();
		CF_WorldMapList::DataEntry* pkData = placeResult->GetMapResPtr();
		if(pkData!=NULL && pkData->_EnterMode==1)
		{
			LC_ServerWorldManager*	pkWorldManager	= SERVER_GET_WORLD_MANAGER(); 
			LC_ServerMapLogic_Place* pkLogicPlace = pkWorldManager->GetMapLogic(mapLogicID);
			if(NULL != pkLogicPlace)
			{
				int mapIndex = pkLogicPlace->getTributaryIndex()-1;
				pkWorldManager->UpdateTributaryRecords(mapResID, mapIndex);
			}
		}
	}

	return placeResult;
}
//------------------------------------------------------------------
LC_ServerMapLogic_Place* LC_ServerWorldManager::_createServerMapLogic(int lMapLogicID, int lMapLogicType, int lMapResID, CF_WorldMapList::DataEntry* entry, bool start, int lineNum)
{
	LC_ServerMapLogicFactory* pkServerMapLogicFactory = SERVER_GET_MAP_LOGIC_FACTORY();
	LC_ServerMapLogic_Place* pkServerMapLogic = pkServerMapLogicFactory->RequestMapLogic(lMapLogicType);
	if (NULL == pkServerMapLogic)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "failed to create map logic type:[%d]", lMapLogicType);
		return NULL;
	}
	LC_ServerMap* pkServerMap = _createServerMap((LC_MapSizeType)entry->_iSizeType, entry->_bGlobal);
	if (NULL == pkServerMap)
	{
		pkServerMapLogicFactory->ReleaseMapLogic(pkServerMapLogic);
		return NULL;
	}
	else
	{
		GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "_createServerMapLogic lMapResID:%d, MapLogicID:%d, lMapLogicType:%d, storyID:%d, lineNum:%d", 
			lMapResID, lMapLogicID, lMapLogicType, entry->_iStoryID, lineNum);
	}

	pkServerMapLogic->SetMapLogicID(lMapLogicID);
	pkServerMapLogic->SetMapLogicType((MapType)lMapLogicType);
	pkServerMapLogic->SetMapResID(lMapResID);
	pkServerMapLogic->SetMap(pkServerMap);
	pkServerMapLogic->SetStoryID(entry->_iStoryID);
	pkServerMapLogic->SetTributaryMaxPlayer(entry->_iTributaryPlayerCount);

	pkServerMap->SetLocalLineNum(lineNum);
	pkServerMap->SetGlobalLineNum(-1);
	pkServerMap->SetMapLogicID(lMapLogicID);
	pkServerMap->SetMapLogicType(lMapLogicType);
	pkServerMap->SetMapResID(lMapResID);
	pkServerMapLogic->Init();

	_addMap(pkServerMap);
	_addMapLogic(pkServerMapLogic);

	ResultType ret = LC_ServerMapLogic_PlayManager::AssemblePlay(pkServerMapLogic);
	if (ret != RE_SUCCESS)
	{
		_releaseServerMapLogic(pkServerMapLogic);
		return NULL;
	}
	if (start)
	{
		pkServerMapLogic->Start();
	}
	//if ( ( entry->_iResourceGroup == 0 ||  entry->_iResourceGroup == lMapResID ) && ( lMapLogicType < MT_TRANSCRIPTION_BEGIN || lMapLogicType == MT_HOOK_MAP ))
	if ( _checkAllowGlobalMapLine(entry, lMapLogicType) )
	{
		if (!m_bInitializing)		// 初始化过程不上报，只缓存到 m_mapLocalResLineData
		{
			// Report to Login server
			MG_SS_MAP_LINE_REGISTER report;
			report.m_Data.nMapResId = lMapResID;
			report.m_Data.nOrigLineIndex = lineNum;
			report.m_Data.nOrigLogicId = lMapLogicID;
			SendMsgToLogin(MGPT_SS_MAP_LINE_REGISTER, &report, 0, 0);
			//GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_SYSTEM, "fenxian, send to login MG_SS_MAP_LINE_REGISTER, lMapResID=%d, lMapLogicID=%d, Local lineNum=%d", lMapResID, lMapLogicID, lineNum);;
		}
		/*else
		{
			uint64_t nKey = LC_Helper::BuildKey( lMapResID, lineNum );
			std::pair< MAP_LINE_TYPE_IT,bool > itReturn = m_mapLocalResLineData.insert( std::make_pair( nKey, lMapLogicID ) );
		}*/
	}

	return pkServerMapLogic;
}

bool LC_ServerWorldManager::_checkAllowGlobalMapLine( CF_WorldMapList::DataEntry* pData, int32_t nMapLogicType )
{
	bool bResult = ( pData->_iResourceGroup == 0 ||  pData->_iResourceGroup == pData->_iID ) 
		&& 
		( 
			nMapLogicType < MT_TRANSCRIPTION_BEGIN 
			|| nMapLogicType == MT_HOOK_MAP 
			|| ( pData->_iTributaryMapCount > 1 && pData->_AutoCreate == 1  )
			// 原因：pData->_lineMapCount 是在多节点引入之前，纯单节点前提下，用来控制固定几条线的逻辑
			// 引入多节点之后，废弃此逻辑。因为此时玩家看来的总线路，取决于开启的节点数。具体节点上已经没有能力控制。
			//|| ( ( pData->_iTributaryMapCount > 1 && pData->_AutoCreate == 1  ) || pData->_lineMapCount > 0 )
			// 原因，纯客户端，在上述条件不满足的情况下，是否显示线路
			//|| pData->_bShowLine 
			|| pData->_iID == GetGlobalSetting.BestGuildMapID
			//|| CheckExcludedMapResId(pData->_iID)
		);
	return bResult;
}

//------------------------------------------------------------------
void LC_ServerWorldManager::_unregisterGlobalMapLine(LC_ServerMapLogic_Place* pMapLogic)
{
	MG_SS_MAP_LINE_UNREGISTER data;
	int32_t nMapResID = pMapLogic->GetMapResID();
	int32_t nMapLogicID = pMapLogic->GetMapLogicID();
	int32_t nLocalLineNum = pMapLogic->GetMap()->GetLocalLineNum();
	int32_t nGlobalLineNum = pMapLogic->GetMap()->GetGlobalLineNum();

	data.m_Data.nGlobalLineIndex = nGlobalLineNum;
	data.m_Data.nOrigLogicId = nMapLogicID;
	data.m_Data.nMapResId = nMapResID;
	SendMsgToLogin(MGPT_SS_MAP_LINE_UNREGISTER, &data, 0, 0 );
	//GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_SYSTEM, "fenxian, send to login MGPT_SS_MAP_LINE_UNREGISTER, lMapResID=%d, lMapLogicID=%d, LocalLineNum=%d, GlobalLineNum=%d", nMapResID, nMapLogicID, nLocalLineNum, nGlobalLineNum);
}

void LC_ServerWorldManager::_releaseServerMapLogic(LC_ServerMapLogic_Place* pkMapLogic)
{
	CF_WorldMapList::DataEntry* entry = pkMapLogic->GetMapResPtr();

	if(_checkAllowGlobalMapLine(entry, pkMapLogic->GetMapLogicType()))
	{
		_unregisterGlobalMapLine(pkMapLogic);		//< 向login注销
	}

	GameStory::LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	if (pkLogicMgr)
	{
		pkLogicMgr->PreMapDestoryed(pkMapLogic);
	}

	int32_t lMapLogicID = pkMapLogic->GetMapLogicID();
	_removeMap(lMapLogicID);
	_removeMapLogic(lMapLogicID);

	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_INFO, "_releaseServerMapLogic mapResID:[%d], lMapLogicID:[%d]", pkMapLogic->GetMapResID(), lMapLogicID);
	
	_freeServerMap((LC_ServerMap*)pkMapLogic->GetMap());
	pkMapLogic->SetMap(NULL);

	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	pkServerMapLogicFactory->ReleaseMapLogic(pkMapLogic);
}

//----------------------------------------------------------------------------------
void LC_ServerWorldManager::ForAllPlayer(GameLogic::LC_ActorOperator* oper)
{
	LC_MapIDPtrMapIter IterBe = m_kMapIDPtrMap.begin();
	LC_MapIDPtrMapIter IterEd = m_kMapIDPtrMap.end();
	for (; IterBe  != IterEd ; ++IterBe)
	{
		LC_MapBase* pkMap = IterBe->second;
		pkMap->ForAllPlayer(oper);
	}
}
//----------------------------------------------------------------------------------
void LC_ServerWorldManager::ForAllNPC(GameLogic::LC_ActorOperator* oper)
{
	LC_MapIDPtrMapIter IterBe = m_kMapIDPtrMap.begin();
	LC_MapIDPtrMapIter IterEd = m_kMapIDPtrMap.end();
	for (; IterBe  != IterEd ; ++IterBe)
	{
		LC_MapBase* pkMap = IterBe->second;
		pkMap->ForAllNPC(oper);
	}
}
//-------------------------------------------------------------------------------------------
void LC_ServerWorldManager::GetLogicMapIDListFromResID(GameLogic::LC_IDList& kLogicIDs, int32_t iResID)
{
	kLogicIDs.clear();
	LC_ServerMapMapLogicIndices::iterator itIndices = m_kLogicMapIndices.find(iResID);
	if (itIndices != m_kLogicMapIndices.end())
	{
		for (LC_ServerMapLogicPlaceVector::iterator itPlace = itIndices->second.begin(); itPlace != itIndices->second.end(); ++itPlace)
		{
			LC_ServerMapLogic_Place* mapLogc = *itPlace;
			kLogicIDs.push_back(mapLogc->GetMapLogicID());
		}
	}
}
//------------------------------------------------------------------
LC_ServerMap* LC_ServerWorldManager::_createServerMap(LC_MapSizeType eSizeType, bool bGlobalMode)
{
	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	LC_ServerMap* themap = pkServerMapLogicFactory->RequestMap(eSizeType);
	if (themap != NULL)
	{
		themap->SetGlobalMode(bGlobalMode);
		themap->Init();
	}
	return themap;
}
void LC_ServerWorldManager::_freeServerMap(LC_ServerMap* pkServerMap)
{
	pkServerMap->UnInit();
	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	pkServerMapLogicFactory->ReleaseMap(pkServerMap);
}

ResultType LC_ServerWorldManager::_addMapLogic(LC_ServerMapLogic_Place* pkPlace)
{
	int32_t iMapLogicID = pkPlace->GetMapLogicID();
	LC_ServerMapLogicIDPtrMap::iterator itPlace = m_kLogicID2Logicmap.find(iMapLogicID);
	if (itPlace != m_kLogicID2Logicmap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "try to add map place duplicated!logic id[%d] res id[%d]", iMapLogicID, pkPlace->GetMapResID());
		return RE_FAIL;
	}

	m_kLogicID2Logicmap[iMapLogicID] = pkPlace;
	int32_t iMapResID = pkPlace->GetMapResID();
	LC_ServerMapLogicPlaceVector& rPlaceVector = m_kLogicMapIndices[iMapResID];
	rPlaceVector.push_back(pkPlace);
	pkPlace->setTributaryIndex((int32_t)rPlaceVector.size());
	return RE_SUCCESS;
}

ResultType LC_ServerWorldManager::_removeMapLogic(int32_t iMapLogicID)
{
	LC_ServerMapLogicIDPtrMap::iterator itPlace = m_kLogicID2Logicmap.find(iMapLogicID);
	if (itPlace == m_kLogicID2Logicmap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "try to remove map place not existed!logic id[%d]", iMapLogicID);
		return RE_FAIL;
	}

	LC_ServerMapLogic_Place* pkPlace = itPlace->second;
	int32_t iMapResID = pkPlace->GetMapResID();
	LC_ServerMapLogicPlaceVector& kIndices = m_kLogicMapIndices[iMapResID];
	LC_ServerMapLogicPlaceVector::iterator itIdx = std::find(kIndices.begin(), kIndices.end(), pkPlace);
	if (itIdx != kIndices.end())
	{
		kIndices.erase(itIdx);
	}
	for(size_t i = 0; i < kIndices.size(); i++)
	{
		LC_ServerMapLogic_Place* pLogicPlace = kIndices[i];
		if(pLogicPlace != NULL)
		{
			pLogicPlace->setTributaryIndex((int32_t)i+1);
		}
	}
	m_kLogicID2Logicmap.erase(itPlace);
	return RE_SUCCESS;
}
//------------------------------------------------------------------

bool LC_ServerWorldManager::_checkWarPlayRun(LC_ServerMapLogic_Place* pkMapLogic)
{
	int iMapResID = pkMapLogic->GetMapResID();
	CF_ForcesBattle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ForcesBattle>(iMapResID);
	if (NULL == pkData)
	{
		return false;
	}
	LC_ServerMapLogic_Place *pkWarMapLogic = GetMapLogicByResAndIndex(iMapResID,pkData->_iLineID);
	if (NULL == pkWarMapLogic)
	{
		return false;
	}
	LC_ServerMapLogic_PlayBase *pkPlay = pkWarMapLogic->GetPlay(PlayType_War);
	if (NULL == pkPlay)
	{
		return false;
	}
	return pkPlay->CheckRun();
}

void LC_ServerWorldManager::UpdateAllMapLineTologin()
{
	m_mapLocalResLineData.clear();
	for (LC_MapIDPtrMapIter IterBe = m_kMapIDPtrMap.begin(); IterBe  != m_kMapIDPtrMap.end() ; ++IterBe)
	{
		LC_ServerMap* pkServerMap = (LC_ServerMap*)(IterBe->second);
		if( NULL == pkServerMap )
			continue;
		int lMapResID = pkServerMap->GetMapResID();
		int lineNum = pkServerMap->GetLocalLineNum();
		int lMapLogicID = pkServerMap->GetMapLogicID();
		CF_WorldMapList::DataEntry* pMap = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
		if ( pMap && _checkAllowGlobalMapLine(pMap,pMap->_iMapLogicType) )
		{
			uint64_t nKey = LC_Helper::BuildKey( lMapResID, lineNum );
			/*std::pair< MAP_LINE_TYPE_IT,bool > itReturn =*/ 
			m_mapLocalResLineData.insert( std::make_pair( nKey, lMapLogicID ) );
		}
	}
}