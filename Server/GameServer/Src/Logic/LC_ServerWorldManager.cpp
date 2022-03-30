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
		/// ���ֵ�ͼ
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

			/// ������ͼ
			if (rkValue->_iTributaryMapCount > 1)
			{
				if(rkValue->_EnterMode == 0)		/// ��ҽ���һ�����ٽ���һ��
				{
					m_kTributaryMapResIDs.insert(rkValue->_iID);
				}
				else if(rkValue->_EnterMode == 1)		/// ������ν�����ߣ����Ҫ�ѷ����ȶ���������
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
	
	// ��ͼ�߼����£�ɾ����Active�ĵ�ͼ
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

	// ���ٷ��Զ����� �� ��Active�ĵ�ͼ
	for (size_t i=0;i<trush_ids.size();i++)
	{
		LC_ServerMapLogic_Place* pkServerMapLogic = trush_ids[i];
		_releaseServerMapLogic(pkServerMapLogic);
	}
	
	//������ͼ��̬����
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
			if (iCount <= 0 && i > 0)		// ���ǵ�һ�ŵ�ͼ����һ�Ų�ɾ����
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
		pkMapLogic->GetMap()->ResetClearPlayerTime(2);	/// �ӳٸõ�ͼ����ʱ�Զ����ٵ�ʱ��
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
	//TributaryIndex��1��ʼ
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
		/// û�к��ʵķ��ߣ������Ƿ���Ҫ����һ��
		if ( bEnableTributary
			&& iMaxFreeCount <= 0							/// û���������еķ���
			&& pkData->_AutoCreate == 1						/// �Զ�����
			&& places.size() < pkData->_iTributaryMapCount	/// ��������δ������
			)
		{
			bool bTributaryMap = m_kTributaryMapResIDs.find(iMapResID) != m_kTributaryMapResIDs.end();
			if (bTributaryMap)
			{
				LC_ServerMapLogic_Place * pkNewTributary = ForceCreateTributaryMap(iMapResID);
				if (pkNewTributary != NULL)			/// �����ɹ����й�ȥ��������ԭֵ
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
	if (pkMapLogicVec->size() >= pkMapEntry->_iTributaryMapCount)	//��������������
	{
		return NULL;
	}

	//��ȡ���߱��
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
	/// ��ȡ���߱��
	std::vector<int32_t>	kLineNos;
	kLineNos.reserve(pkMapLogicVec->size() + 1);
	kLineNos.push_back(-1);
	for (LC_ServerMapLogicPlaceVector::iterator it = pkMapLogicVec->begin(); it != pkMapLogicVec->end(); ++it)
	{
		LC_ServerMapLogic_Place * pkMapLogic = *it;
		kLineNos.push_back(pkMapLogic->GetMap()->GetLocalLineNum());
	}
	std::sort(kLineNos.begin(), kLineNos.end());		// ��С��������

	/// ���ҵ���һ���������ı��
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
	//�ڲ�ȷ������Ŀ�ĵص������,ͨ��������ϵ����ݲ���Ŀ�ĵ�
	if (eInOutType == LMIOT_RECOVER)
	{
		eResult = _requestChangePlayerLocationByRecover(pkPlayer);
		//������ɹ�������ҷ���Ұ���ͼ
		if (eResult != RE_SUCCESS)
		{
			eResult = _requestChangePlayerLocationToDefault(pkPlayer, eInOutType);
		}
	}
	if (eInOutType == LMIOT_OFFLINE)
	{
		eResult = _requestChangePlayerLocationByOffLine(pkPlayer);
		//������ɹ�������ҷ���Ұ���ͼ
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

	//������ҵ�maplogicid
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
		case LMIOT_CLEANUP: //�ɵ�ͼ�߼�����������ȥ
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, MT_NORMAL, 0, lPermenentMapResID, kPermentLocation, lineNum, bCertainSameServer);
				//������ɹ�������ҷ��뵽Ұ���ͼ
				if (eResult != RE_SUCCESS)
				{
					eResult = _requestChangePlayerLocationToDefault(pkPlayer, eInOutType);
				}
			}
			break;
		case LMIOT_RETURN_CITY: //�سǣ����߼�ϵͳ���ƴ���λ��
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, MT_NORMAL, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_TRANSFER: //������ͼ���ͣ����߼�ϵͳ���ƴ���λ��
			{
				//�����ͣ��жϳ�սbuddy
				if(false == CheckAllowToFightBuddyId(buddyId,pWorldMapListdata->_ActiveBuddyIds) )
				{
					GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "RequestChangePlayerLocation tag = false cid:%d,citizenship:%d,uid:%d,id:%d,eInOutType:%d,lTargetMapLogicID:%d,iTargetMapResID:%d,lineNum:%d (%d,%d,%d) ",instance.detail(),instance.catagory(),pkPlayer->GetUserID(),pkPlayer->GetID(),(int)eInOutType,lTargetMapLogicID,iTargetMapResID,lineNum,rkTargetLocation.x,rkTargetLocation.y,rkTargetLocation.z);
					return RE_FAIL;
				}
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_GROW_UP: //���ֳɳ�,����͵�ͼ����
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, MT_NORMAL, 0, lPermenentMapResID, kPermentLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_CURRENT_RESPAWN: //ԭ�ظ���,���߼�ϵͳ���ƴ���λ��
			{
				int lineNum = pkPlayer->GetLocalLineNum();
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_SAFE_RESPAWN: //ԭ�ظ���,���߼�ϵͳ���ƴ���λ��
			{
				int lineNum = pkPlayer->GetLocalLineNum();
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
				//������ɹ�������ҷ��뵽Ұ���ͼ
				if (eResult != RE_SUCCESS)
				{
					eResult = _requestChangePlayerLocationToDefault(pkPlayer, eInOutType);
				}
			}
			break;
		case LMIOT_SKILL_RESPAWN: //���ܸ�����߼�ϵͳ���ƴ���λ��
			{
				eResult = _requestChangePlayerLocationByLogic(pkPlayer, eInOutType, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, rkTargetLocation, lineNum, bCertainSameServer);
			}
			break;
		case LMIOT_FIRST_ENTER_TRANSCRIPTION: //��һ�ε�¼���븱��
			{
				eResult = _requestChangePlayerLocationByFirstEnterRaid(pkPlayer, LMIOT_TRANSFER, eTargetMapLogicType, lTargetMapLogicID, iTargetMapResID, UT_Vec3Int());
			}
			break;
		case LMIOT_QUIT_TRANSCRIPTION://�����˳�,�ָ���������
			{
				//�����˳���ͼ
				// quit ���� ��ؽ�����Ϣ
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

	//����������ս�����ݼ�¼
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
		//������߼�����ȥ�ģ���Ҫ��֪ͨ��ͼ��
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
	//�Ƿ��е�����
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
	//����ƽ��ؾ�����Ļ�ɱ����
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
	//���ݽ���������͵Ĳ�ͬ��ѡ��ָ��Ĳ�������
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
		//���ȳ��Իָ���ʱ��������
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
		//�ָ��ɹ�
		if (eChangeResult == RE_SUCCESS)
		{
			return eChangeResult;
		}
	}

	if (bTestRecoverPermenentLocation)
	{
		eChangeResult = RE_FAIL;
		//�������Իָ�������������
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
	//�ָ��ɹ�
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

	//��Ҵ�ʱӦ����������
	pkSourceMapLogic = GetMapLogic(pkPlayer->GetMapLogicID());

	//ret:�ڷ�global�ڵ㣬������빫���ؾ���ͼ
	bool ret = UT_ServerHelper::CheckChangeServer(LMIOT_TRANSFER, lTargetMapLogicType, bCertainSameServer);
	int32_t srcMapLogicID = pkSourceMapLogic==NULL ? 0 : pkSourceMapLogic->GetMapResID();

	//������ͨ��ͼ����Ҫ�������ֺ�����
	if (IS_COMMON_MAP(lTargetMapLogicType) || lTargetMapLogicType == MT_HOOK_MAP)
	{
		int currentResID = pkPlayer->GetMapResID();
		if (lTargetMapLogicID == 0 && iTargetMapResID == currentResID)	// ͬ��ͼ�л��������Ь
		{
			pkTargetMapLogic = pkSourceMapLogic;
		}
		else if (pkSourceMapLogic != NULL && pkSourceMapLogic->GetMapLogicType() == MT_HOOK_MAP)	// �ӹһ���Ұ�˳���Ұ�⣬������ LMIOT_QUIT_TRANSCRIPTION
		{
			bQuitHookMap2CommonMap = true;			
		}
		//���ҵ�һ�ź��ʵ���ͨ��ͼ
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

		//������ͼ
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
		//ͬ��ͼ����
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
			//�����������
			kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, eInOutType, rkTargetLocation);
			pkTargetMapLogic->RequestChangeLocation(pkPlayer, eInOutType, kTargetLocation);
		}
		else
		{
			//Ŀ���ͼ�Ƿ���Խ���
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
				//��ǰ��ͼ�Ƿ�����뿪
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

			//�����������
			kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, eInOutType, rkTargetLocation);

			int32_t destMapLogicID = pkTargetMapLogic==NULL ? 0 : pkTargetMapLogic->GetMapLogicID();
			bool srcIsShowRaidStage = pkSourceMapLogic==NULL ? false : pkSourceMapLogic->IsShowRaidStage();
			bool iDestIsShowRaidStage = pkTargetMapLogic==NULL ? false : pkTargetMapLogic->IsShowRaidStage();
			// �е�ͼ�Ƿ�Ҫ������ͻ��������ʱ��bCertainSameServer=false, login�ع�����ʱ��bCertainSameServer=true�����һֱ��false����Ϣ��ʼ����GS��Login��ѭ��
			//��������˳���λ������������TrySwitchMap
			if(srcIsShowRaidStage || iDestIsShowRaidStage || !UT_ServerHelper::TrySwitchMap(pkPlayer, eInOutType, lTargetMapLogicType, srcMapLogicID, destMapLogicID, iTargetMapResID, kTargetLocation.x, kTargetLocation.y, kTargetLocation.z, bQuitHookMap2CommonMap, bCertainSameServer))
			{
				if (eInOutType != LMIOT_CLEANUP)
				{
					//�ӵ�ǰ��ͼ�뿪
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
				//��Ŀ���ͼ����,���ǵ������Ѿ�����뿪
				pkTargetMapLogic->RequestEnterMap(pkPlayer, eInOutType, kTargetLocation);
				UT_ServerHelper::AddAllBuffIdToSingleGuildMember(pkPlayer->GetGuildID(),pkPlayer->GetInstance());
			}
		}
	}

	if(pkPlayer->GetReliveBeforeEnterMapTag())
	{
		pkPlayer->SetReliveBeforeEnterMapTag(false);
	}

	//�˳���ͼ��֪ͨ����ϵͳ���buddy����
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
	//Ŀ���ͼ�Ƿ���Խ���
	eChangeResult = pkTargetMapLogic->CheckEnterMap(pkPlayer, eInOutType);
	if (eChangeResult != RE_SUCCESS)
	{
		return eChangeResult;
	}
	//�����������
	UT_Vec3Int	kTargetLocation = pkTargetMapLogic->ComputeEnterLocation(pkPlayer, eInOutType, rkTargetLocation);
	//��Ŀ���ͼ����
	pkTargetMapLogic->RequestEnterMap(pkPlayer, eInOutType, kTargetLocation);
	return eChangeResult;
}

//------------------------------------------------------------------
ResultType LC_ServerWorldManager::_requestChangePlayerLocationToDefault(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType)
{
	LC_ServerMapLogic_Place* pkTargetMapLogic = NULL;
	ResultType  eChangeResult			= RE_FAIL;

	//���ҵ�һ�ź��ʵ���ͨ��ͼ
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

	//���ֽ��٣�������global�ڵ���ҵ�ͼ
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
		//�������ͱ���Ҫ�й���
		LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
		CF_TranscriptionRule::DataEntry* pkEntry = rulemanager->GetRule(lMapResID);
		if (pkEntry == NULL)
		{
			return NULL;
		}
		if (lMapLogicID > 0 && MT_GUILD_PARTYFUBEN != lMapLogicType)
		{
			//ָ�������ͼ
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
						//ָ�������ͼ
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
						//�пյ�ͼ����  
						//û�յ�ͼ������
						int currentLineCount = 0;
						int32_t createLineNum = 0;
						bool changeLineNum = false;
						mem::set<int32_t> AllLineNum;//���ڴ��������еĵ�ͼnum
						mem::set<LC_ServerMapLogic_Place*> pkPlace;//��������δ���ĵ�ͼ��
						LC_ServerMapLogic_Place* pkFirstPlace;//linenum=0�ĵ�ͼ
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

							//���ߣ�ֱ�ӽ���ѡ����
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

							//δ���ĵ�ͼ
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
							//�п��ߣ�����һ������
							if(!pkPlace.empty())
							{
								return *pkPlace.begin();
							}
							else if(!AllLineNum.empty())
							{//û���ߣ��ҵ�ǰ��С���ߺţ�����
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
							//���������Ѿ��ﵽ�����ֵ����1��
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
		if (!m_bInitializing)		// ��ʼ�����̲��ϱ���ֻ���浽 m_mapLocalResLineData
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
			// ԭ��pData->_lineMapCount ���ڶ�ڵ�����֮ǰ�������ڵ�ǰ���£��������ƹ̶������ߵ��߼�
			// �����ڵ�֮�󣬷������߼�����Ϊ��ʱ��ҿ���������·��ȡ���ڿ����Ľڵ���������ڵ����Ѿ�û���������ơ�
			//|| ( ( pData->_iTributaryMapCount > 1 && pData->_AutoCreate == 1  ) || pData->_lineMapCount > 0 )
			// ԭ�򣬴��ͻ��ˣ����������������������£��Ƿ���ʾ��·
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
		_unregisterGlobalMapLine(pkMapLogic);		//< ��loginע��
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