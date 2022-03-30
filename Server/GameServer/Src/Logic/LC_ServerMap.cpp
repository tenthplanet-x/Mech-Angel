#include "LC_ServerMap.h"
#include "LC_ServerSector.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlaceHolderManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerNPCManager.h"
#include "LC_ServerMapLogicFactory.h"
#include "GlobalSettings.h"

#include "NW_Helper.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
using namespace CSVFile;
//------------------------------------------------------------------
LC_ServerMap::LC_ServerMap(LC_MapSizeType eMapSizeType)
: LC_MapBase(eMapSizeType)
,m_fLastUpdateTime(0.0f)
,m_fTriggerLastUpdateTime(0.0f)
,m_AllTrigger()
,m_TriggerIdBegin(0)
,m_DelTrigger()
, m_bGlobalMode(false)
{

}
//------------------------------------------------------------------
LC_ServerMap::~LC_ServerMap()
{
	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	for(LC_SectorPtrVector::iterator i = m_kSectorPtrVec.begin(); i !=  m_kSectorPtrVec.end(); ++i)
	{
		LC_SectorBase* pkSector = *i;
		pkServerMapLogicFactory->ReleaseSector((LC_ServerSector*)pkSector);
	}
	m_kSectorPtrVec.clear();	
}
//------------------------------------------------------------------
void LC_ServerMap::Init()
{
	T_ASSERT_CRITICAL_S(m_kSectorPtrVec.empty());
	m_kDirtySectorIndexPtrMap.clear();

	int iSectorCount = m_iSectorXNum * m_iSectorYNum;
	if(iSectorCount <= 0)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"LC_ServerMap::Init error");
		return;
	}

	bool bGlobalMapMode = m_bGlobalMode;
	//if ( m_iSectorXNum == 32 || m_iMapResID == 1 )
	int neighbourBound = GetGlobalSetting.nNeighbourGridCount;
	//int neighbourBound = 1;
	//int neighbourBound = 2;
	int sqrtRoot = 2*neighbourBound+1;
	// 全图模式
	if ( bGlobalMapMode )
	{
		//sqrtRoot = m_iSectorXNum;
		//neighbourBound = m_iSectorXNum / 2;
		m_iSectorXNum = 1;//m_iMapWidth / m_iSectorSize;
		m_iSectorYNum = 1;//m_iMapHeight / m_iSectorSize;
		m_iSectorSize = m_iMapWidth;
		sqrtRoot = 1;
		neighbourBound = 1;
		iSectorCount = 1;
	}
	m_kSectorPtrVec.reserve(iSectorCount);

	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	for(int iIndex = 0; iIndex < iSectorCount; iIndex++)
	{
		LC_SectorBase* pkSector = pkServerMapLogicFactory->RequestSector();

		pkSector->SetSectorIndex(iIndex);
		pkSector->SetMap(this);

		SectorIndexSet& kNeighbors1 = pkSector->GetSectorNeighborIndexSet();
		//if (bGlobalMapMode)
		//{
		//	for ( int i = 0; i < iSectorCount; i++ )
		//		kNeighbors1.insert( i );
		//}
		//else
		{
			_computeSectorNeighbors(iIndex,neighbourBound,kNeighbors1);
		}

		//if (bGlobalMapMode)
		//{
		//	pkSector->SetSectorNeighborIndexSet();
		//}
		//else
		{
			pkSector->SetSectorNeighborIndexSet(sqrtRoot);
		}

		m_kSectorPtrVec.push_back(pkSector);
	}
	m_AllTrigger.clear();
	m_TriggerIdBegin=0;
	m_DelTrigger.clear();
	m_fLastUpdateTime=0.0f;
	m_fTriggerLastUpdateTime=0.0f;
	m_kPlayerIDPtrMap.clear();
	m_kControllerPlayerIDPtrMap.clear();
	m_kNPCIDPtrMap.clear();
	m_kPlaceHolderIDPtrMap.clear();
}
//------------------------------------------------------------------
void LC_ServerMap::UnInit()
{
//#pragma TODO("删除Sector")
	LC_ServerNPCManager * mgr = SERVER_GET_NPC_MANAGER();

	LC_NPCIDPtrMapIter IterBe = m_kNPCIDPtrMap.begin();
	LC_NPCIDPtrMapIter IterEd = m_kNPCIDPtrMap.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		LC_NPCBase* pkNPCBase = (*IterBe).second;
		mgr->DestoryNPC(pkNPCBase);
	}

	m_kNPCIDPtrMap.clear();

	LC_ServerPlaceHolderManager * pmgr = SERVER_GET_PLACEHOLDER_MANAGER();

	for( LC_PlaceHolderIDPtrMapIter i = m_kPlaceHolderIDPtrMap.begin() ; i != m_kPlaceHolderIDPtrMap.end(); ++i )
	{
		LC_PlaceHolderBase * base = i->second;
		pmgr->DestoryPlaceHolder(base);
	}
	m_kPlaceHolderIDPtrMap.clear();

	mem::map<int,LC_ServerMap_Trigger*>::iterator beg=m_AllTrigger.begin();
	for (;beg!=m_AllTrigger.end();++beg)
	{
		T_DELETE_D beg->second;
	}
	m_AllTrigger.clear();
	m_TriggerIdBegin=0;
	m_DelTrigger.clear();
}
//------------------------------------------------------------------
void LC_ServerMap::Update(float fCurrentTime,float fDeltaTime)
{
	if(fCurrentTime>=m_fLastUpdateTime)
	{
		_updateDirtySector();
		/*取消宝箱状态更新
		LC_DirtySectorIndexPtrMapIter Iter;
		TRAV(m_kDirtySectorIndexPtrMap,Iter)
		{
			LC_ServerSector* pkDirtySector = (LC_ServerSector*)Iter->second;
			SectorIndexSet& rkNeighborSectorIndexSet = pkDirtySector->GetSectorNeighborIndexSet();

			SectorIndexSetIter Iter2;
			TRAV(rkNeighborSectorIndexSet,Iter2)
			{
				SectorIndex iNeighborIndex = *Iter2;
				LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(iNeighborIndex);
				if(pkNeighborSector)
				{
				}
			}
		}	
		*/
		m_fLastUpdateTime = fCurrentTime+0.2f;
	}
	if(fCurrentTime >= m_fTriggerLastUpdateTime)
	{
		mem::set<int>::iterator beg=m_DelTrigger.begin();
		for (;beg!=m_DelTrigger.end();++beg)
		{
			mem::map<int,LC_ServerMap_Trigger*>::iterator it=m_AllTrigger.find(*beg);
			if (it==m_AllTrigger.end())
			{
				continue;
			}
			T_DELETE_D it->second;
			m_AllTrigger.erase(it);
		}
		m_DelTrigger.clear();

		mem::map<int,LC_ServerMap_Trigger*>::iterator trigbeg=m_AllTrigger.begin();
		for (;trigbeg!=m_AllTrigger.end();++trigbeg)
		{
			trigbeg->second->Update(fCurrentTime,fDeltaTime);
		}
		m_fTriggerLastUpdateTime = fCurrentTime+0.5f;
	}
	

}
//------------------------------------------------------------------
void LC_ServerMap::ChangeSector(LC_LogicObject* pkLogicObject )
{
	bool bDenyBroadcastFlag = false;
	SectorIndex iOldSectorIndex = pkLogicObject->GetOldSectorIndex();
	//pkLogicObject->GetType();
	if ( iOldSectorIndex != 0xffffffff && m_bGlobalMode)
		bDenyBroadcastFlag = true;
	SectorIndex iNewSectorIndex = pkLogicObject->GetSectorIndex();
	LC_SectorBase* pkOldSector = GetSector(iOldSectorIndex);
	LC_SectorBase* pkNewSector = GetSector(iNewSectorIndex);
	object_id_type lID = pkLogicObject->GetID();
	uint32_t nNPCCounter = 0;
	if(pkLogicObject->GetType() == LOT_PLAYER)
	//if(pkLogicObject->GetType() == LOT_PLAYER && !bDenyBroadcastFlag)
	{
		/*if(pkOldSector)
		{
			T_ASSERT(pkOldSector->FindObject(pkLogicObject->GetID()) == pkLogicObject && "map error");
			if(pkOldSector->GetPlayerCount() == 1)
				_removeDirtySector(iOldSectorIndex);
		}*/

		if(pkNewSector)
		{
			_addDirtySector(pkNewSector);
		}
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkLogicObject;
		LC_ActorFOVMapBase* pkFOVMap = pkPlayer->GetFOVMap();

		//if ( !bDenyBroadcastFlag )
		{
			//删除视野中的旧内容
			pkFOVMap->ClearPlayersInFOV();
			pkFOVMap->ClearNPCsInFOV();
			pkFOVMap->ClearPlaceHoldersInFOV();

			//从周围玩家的视野中删除
			if(pkOldSector)
			{
				SectorIndexSet& rkNeighborSectorIndexSet = pkOldSector->GetSectorNeighborIndexSet();
				SectorIndexSetIter Iter2;
				TRAV(rkNeighborSectorIndexSet,Iter2)
				{
					SectorIndex iNeighborIndex = *Iter2;
					LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(iNeighborIndex);
					if(pkNeighborSector)
					{
						pkNeighborSector->RemovePlayerFromFOV(lID);
					}
				}
			}
		}		
		//添加新视野中的内容
		if(pkNewSector)
		{
			SectorIndexVector& rkNeighborSectorIndexSet = pkNewSector->GetSectorNeighborIndexVector();
			SectorIndexVectorIter Iter1;
			uint32_t nCounter = 0;
			
			TRAV(rkNeighborSectorIndexSet,Iter1)
			{
				SectorIndex iNeighborIndex = *Iter1;
				LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(iNeighborIndex);
				nCounter++;
				if(pkNeighborSector)
				{
					//添加到周围玩家的视野中
					pkNeighborSector->AddPlayerToFOV(lID);

					//获取Sector中的NPC列表
					LC_NPCIDPtrMap& rkNPCMap = pkNeighborSector->GetNPCIDPtrMap();
					LC_NPCIDPtrMapIter Iter3;
					uint32_t nSecNPCSize = rkNPCMap.size();
					TRAV(rkNPCMap,Iter3)
					{
						object_id_type lID = Iter3->first;
						pkFOVMap->AddNPCIDToFOV(lID);
						nNPCCounter++;
					}

					//获取Sector中的Player列表
					LC_PlayerIDPtrMap& rkPlayerMap = pkNeighborSector->GetPlayerIDPtrMap();
					LC_PlayerIDPtrMapIter Iter4;
					TRAV(rkPlayerMap,Iter4)
					{
						object_id_type lID = Iter4->first;
						pkFOVMap->AddPlayerIDToFOV(lID);
						//GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "AddPlayerIDToFOV", pkPlayer->GetMapLogicID(), pkPlayer->GetID(), lID);
					}

					CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
					//获取Sector中的PlaceHolder列表
					LC_PlaceHolderIDPtrMap& rkPlaceHolderMap = pkNeighborSector->GetPlaceHolderIDPtrMap();
					LC_PlaceHolderIDPtrMapIter Iter5;
					TRAV(rkPlaceHolderMap, Iter5)
					{
						object_id_type lPlaceHolderID = Iter5->first;
						LC_ServerPlaceHolderManager* pkPlaceHolderManager = SERVER_GET_PLACEHOLDER_MANAGER();
						LC_PlaceHolderBase* pkPlaceHolder = pkPlaceHolderManager->FindPlaceHolder(lPlaceHolderID);
						if (!pkPlaceHolder  || (!pkPlaceHolder->IsVisibleToMe(lID)))
						{
							continue;
						}

						if ( pkFOVMap->GetPlaceHolderShowLevel(lPlaceHolderID) == PHL_INVALD )
						{
							CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(pkPlaceHolder->GetReward().GetItemContext().GetBackPackValueByType(LPSFET_TYPE_ID));
							pkFOVMap->AddPlaceHolderShowLevel(lPlaceHolderID, pkItemData ? pkItemData->_lGrade : ITEM_GRADE_WHITE,  pkPlaceHolder->IsOwner(lID));
						}
						pkFOVMap->AddPlaceHolderIDToFOV(lPlaceHolderID);
					}
				}
			}
		}									
	}

	if(pkLogicObject->GetType() == LOT_NPC)
	{
		//从周围玩家的视野中删除
		if(pkOldSector)
		{
			SectorIndexSet& rkNeighborSectorIndexSet = pkOldSector->GetSectorNeighborIndexSet();
			SectorIndexSetIter Iter2;
			TRAV(rkNeighborSectorIndexSet,Iter2)
			{
				SectorIndex iNeighborIndex = *Iter2;
				LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(iNeighborIndex);
				if(pkNeighborSector)
				{
					pkNeighborSector->RemoveNPCFromFOV(lID);
				}
			}
		}

		//添加到周围玩家的视野中
		if(pkNewSector)
		{
			SectorIndexSet& rkNeighborSectorIndexSet = pkNewSector->GetSectorNeighborIndexSet();
			SectorIndexSetIter Iter2;
			TRAV(rkNeighborSectorIndexSet,Iter2)
			{
				SectorIndex iNeighborIndex = *Iter2;
				LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(iNeighborIndex);
				if(pkNeighborSector)
				{
					pkNeighborSector->AddNPCToFOV(lID);
				}
			}
		}
	}

	if (pkLogicObject->GetType() == LOT_PLACEHOLDER)
	{
		//从周围玩家的视野中删除
		if(pkOldSector)
		{
			SectorIndexSet& rkNeighborSectorIndexSet = pkOldSector->GetSectorNeighborIndexSet();
			SectorIndexSetIter Iter2;
			TRAV(rkNeighborSectorIndexSet,Iter2)
			{
				SectorIndex iNeighborIndex = *Iter2;
				LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(iNeighborIndex);
				if(pkNeighborSector)
				{
					pkNeighborSector->RemovePlaceHolderFromFOV(lID);
				}
			}
		}

		//添加到周围玩家的视野中
		if(pkNewSector)
		{
			SectorIndexSet& rkNeighborSectorIndexSet = pkNewSector->GetSectorNeighborIndexSet();
			SectorIndexSetIter Iter2;
			TRAV(rkNeighborSectorIndexSet,Iter2)
			{
				SectorIndex iNeighborIndex = *Iter2;
				LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(iNeighborIndex);
				if(pkNeighborSector)
				{
					pkNeighborSector->AddPlaceHolderToFOV(lID);
				}
			}
		}
	}
	LC_MapBase::ChangeSector(pkLogicObject);
}
//------------------------------------------------------------------
bool LC_ServerMap::RemoveObject(LC_LogicObject* pkObject)
{
	return LC_MapBase::RemoveObject(pkObject);
}
//------------------------------------------------------------------

static mem::vector<const LC_PlayerIDPtrMap*> playersCache(9, NULL);

void LC_ServerMap::BroadCastToArea(SectorIndex dwSectorIndex, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, LC_PlayerOperator* pkFilter)
{
	playersCache.clear();
	GetAreaPlayerList(dwSectorIndex, playersCache);
	MulticastMsgToGateway(playersCache, usMsgType, pkMsgBody, pkFilter);
}
//------------------------------------------------------------------
void LC_ServerMap::BroadCastToServer(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, GameLogic::LC_PlayerOperator* pkFilter)
{
	MulticastMsgToGateway(m_kPlayerIDPtrMap, usMsgType, pkMsgBody, pkFilter);
}

void LC_ServerMap::BroadCastToAll(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, LC_PlayerOperator* pkFilter)
{
	MulticastMsgToGateway(m_kPlayerIDPtrMap, usMsgType, pkMsgBody, pkFilter);
}
//------------------------------------------------------------------
void LC_ServerMap::GetAreaPlayerList( SectorIndex dwSectorIndex,LC_PlayerPtrList& playerList )
{
	LC_SectorBase* pkCenterSector = GetSector(dwSectorIndex);
	if(!pkCenterSector)
	{
		return;
	}

	SectorIndexVector& kAroundIndex = pkCenterSector->GetSectorNeighborIndexVector();
	SectorIndexVectorIter IterBe = kAroundIndex.begin();
	SectorIndexVectorIter IterEd = kAroundIndex.end();

	for ( ;IterBe != IterEd; ++IterBe)
	{
		LC_ServerSector* pkSector = (LC_ServerSector*)GetSector(*IterBe);
		if(!pkSector)
		{
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"can't find sector m_iMapResID:%d", m_iMapResID);
			continue;
		}
		pkSector->GetPlayerPtrList(playerList);
	}
}
void LC_ServerMap::GetAreaPlayerList(SectorIndex dwSectorIndex, mem::vector<const LC_PlayerIDPtrMap*>& playersVec)
{
	LC_SectorBase* pkCenterSector = GetSector(dwSectorIndex);
	if(!pkCenterSector)
	{
		return;
	}

	SectorIndexVector& kAroundIndex = pkCenterSector->GetSectorNeighborIndexVector();
	SectorIndexVectorIter IterBe = kAroundIndex.begin();
	SectorIndexVectorIter IterEd = kAroundIndex.end();

	for ( ;IterBe != IterEd; ++IterBe)
	{
		LC_ServerSector* pkSector = (LC_ServerSector*)GetSector(*IterBe);
		if(!pkSector)
		{
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"can't find sector! m_iMapResID:%d", m_iMapResID);
			continue;
		}
		playersVec.push_back(&pkSector->GetPlayerIDPtrMap());
	}
}
//------------------------------------------------------------------
void LC_ServerMap::FillActivePlayerSector(LC_ServerSectorVector& activePlayerSectors)
{
	LC_DirtySectorIndexPtrMapIter endActiveSecotr = m_kDirtySectorIndexPtrMap.end();
	for (LC_DirtySectorIndexPtrMapIter activeIt = m_kDirtySectorIndexPtrMap.begin() ; 
		activeIt != endActiveSecotr; ++activeIt)
	{
		//LC_ServerSector* activeSector = (LC_ServerSector*)activeIt->second;
		activePlayerSectors.push_back((LC_ServerSector*)activeIt->second);
	}
}
//------------------------------------------------------------------
void LC_ServerMap::FillActiveNpcSector(LC_ServerSectorVector& activeNPCSectors)
{
	LC_ServerSectorSet sectorStore;
	sectorStore.insert(activeNPCSectors.begin(), activeNPCSectors.end());
// 	for (size_t i =0 ; i < activeNPCSectors.size(); ++i)
// 	{
// 		sectorStore.insert(activeNPCSectors[i]);
// 	}
	LC_DirtySectorIndexPtrMapIter endActiveSecotr = m_kDirtySectorIndexPtrMap.end();
	for (LC_DirtySectorIndexPtrMapIter activeIt = m_kDirtySectorIndexPtrMap.begin() ;  activeIt != endActiveSecotr; ++activeIt)
	{
		LC_ServerSector* activeSector = (LC_ServerSector*)(*activeIt).second;
		SectorIndexSet& neighbourSectorSet = activeSector->GetSectorNeighborIndexSet();
		for (SectorIndexSet::iterator neighbourIt = neighbourSectorSet.begin(); neighbourIt != neighbourSectorSet.end();
			++neighbourIt)
		{
			LC_ServerSector* neighbourSector = (LC_ServerSector*)GetSector(*neighbourIt);
			//LC_ServerSectorSet findIt = sectorStore.find(neighbourSector);
			if (sectorStore.find(neighbourSector) == sectorStore.end())
			{
				activeNPCSectors.push_back(neighbourSector);
				sectorStore.insert(neighbourSector);
			}
		}
	}

}
void LC_ServerMap::FillActiveSector(ActiveSectorCluster& activeSectorClus)
{
	size_t activeCnt = m_kDirtySectorIndexPtrMap.size();
	activeSectorClus.activeCnt = activeCnt;

	LC_ServerSectorVector& kSectors = activeSectorClus.kSectors;
	mem::map<SectorIndex, size_t>& kReverseIndices = activeSectorClus.kReverseIndices;

	kSectors.resize(activeCnt, NULL);
	
	LC_DirtySectorIndexPtrMapIter endActiveSecotr = m_kDirtySectorIndexPtrMap.end();
	LC_DirtySectorIndexPtrMapIter activeIt = m_kDirtySectorIndexPtrMap.begin();
	for (int i=0; activeIt!=endActiveSecotr; ++activeIt, ++i)
	{
		LC_ServerSector* activeSector = (LC_ServerSector*)activeIt->second;
		kSectors[i] = (activeSector);

		kReverseIndices[activeIt->first] = i;

		SectorIndexSet& neighbourSectorSet = activeSector->GetSectorNeighborIndexSet();
		for (SectorIndexSet::iterator neighbourIt = neighbourSectorSet.begin(); neighbourIt!=neighbourSectorSet.end();
			++neighbourIt)
		{
			LC_DirtySectorIndexPtrMapIter itNbour = m_kDirtySectorIndexPtrMap.find(*neighbourIt);
			if (itNbour == m_kDirtySectorIndexPtrMap.end())
			{
				kSectors.push_back((LC_ServerSector*)GetSector(*neighbourIt));
				kReverseIndices[*neighbourIt] = kSectors.size()-1;
			}
		}
	}
}
//------------------------------------------------------------------
LC_SectorBase* LC_ServerMap::_findDirtySector(GameLogic::SectorIndex iIndex)
{
	LC_DirtySectorIndexPtrMapIter IterFind = m_kDirtySectorIndexPtrMap.find(iIndex);
	if(IterFind == m_kDirtySectorIndexPtrMap.end())
		return NULL;
	else
		return IterFind->second;
}
//------------------------------------------------------------------
void LC_ServerMap::_removeDirtySector(GameLogic::SectorIndex iIndex)
{
	LC_DirtySectorIndexPtrMapIter IterFind = m_kDirtySectorIndexPtrMap.find(iIndex);
	//T_ASSERT(IterFind != m_kDirtySectorIndexPtrMap.end());

	if(IterFind != m_kDirtySectorIndexPtrMap.end() &&m_bGlobalMode == false)
		m_kDirtySectorIndexPtrMap.erase(IterFind);
}
//------------------------------------------------------------------
void LC_ServerMap::_addDirtySector(GameLogic::LC_SectorBase* pkSector)
{
	SectorIndex iIndex = pkSector->GetSectorIndex();
	LC_DirtySectorIndexPtrMapIter IterFind = m_kDirtySectorIndexPtrMap.find(iIndex);
	if(IterFind != m_kDirtySectorIndexPtrMap.end())
		return;
	else
	{
		m_kDirtySectorIndexPtrMap[iIndex] = pkSector;

		//该格子周围9个格子被激活
		SectorIndexSet& kAroundIndex = pkSector->GetSectorNeighborIndexSet();
		SectorIndexSetIter IterBe = kAroundIndex.begin();
		SectorIndexSetIter IterEd = kAroundIndex.end();

		for ( ;IterBe != IterEd; ++IterBe)
		{
			LC_ServerSector* pkSector = (LC_ServerSector*)GetSector(*IterBe);
			if(!pkSector)
			{
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"can't find sector! m_iMapResID:%d", m_iMapResID);
				continue;
			}
			pkSector->SetUpdateFlag(true);
		}
	}
}
//------------------------------------------------------------------
void LC_ServerMap::_updateDirtySector()
{
	LC_DirtySectorIndexPtrMapIter IterBe = m_kDirtySectorIndexPtrMap.begin();

	for( ; IterBe != m_kDirtySectorIndexPtrMap.end(); )
	{
		LC_ServerSector* pkSector = (LC_ServerSector*)(IterBe->second);
		//if(pkSector->GetPlayerCount() == 0)
		if(pkSector->GetPlayerCount() == 0 && m_bGlobalMode == false)
		{
			//检查该格子周围9个格子是否可以取消激活
			SectorIndexSet& kAroundIndex = pkSector->GetSectorNeighborIndexSet();
			SectorIndexSetIter IterNeighborBe = kAroundIndex.begin();
			SectorIndexSetIter IterNeighborEd = kAroundIndex.end();
			
//			bool bSafeClearSector = true;
			for ( ;IterNeighborBe != IterNeighborEd; ++IterNeighborBe)
			{
				LC_ServerSector* pkNeighborSector = (LC_ServerSector*)GetSector(*IterNeighborBe);
				if(!pkNeighborSector)
				{
					//GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"can't find sector");
					continue;
				}
	
				pkNeighborSector->CheckUpdateFlag();
			}
			m_kDirtySectorIndexPtrMap.erase(IterBe++);
		}
		else
		{
			++IterBe;
		}
	}
}

void LC_ServerMap::GetAllPlaceHolder(PlaceHolderListType& pkList)
{
	LC_PlaceHolderIDPtrMapIter IterBe = m_kPlaceHolderIDPtrMap.begin();
	LC_PlaceHolderIDPtrMapIter IterEd = m_kPlaceHolderIDPtrMap.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		LC_ServerPlaceHolder* pkobj = (LC_ServerPlaceHolder*)IterBe->second;
		pkList.push_back(pkobj);
	}
}

LC_ServerMap_Trigger* LC_ServerMap::AddCircleTrigger(Utility::UT_Circle& circle,LC_ServerMapLogic_Place* maplogic,LC_ServerMap_Trigger::TriggerCallBack callback)
{
	LC_ServerMap_Trigger* trig=T_NEW_D LC_ServerMap_Trigger(++m_TriggerIdBegin,circle,callback,this,maplogic);
	m_AllTrigger[trig->GetId()] = trig;
	return trig;
}

LC_ServerMap_Trigger* LC_ServerMap::AddRectTrigger(Utility::UT_OBB& obb,LC_ServerMapLogic_Place* maplogic,LC_ServerMap_Trigger::TriggerCallBack callback)
{
	LC_ServerMap_Trigger* trig=T_NEW_D LC_ServerMap_Trigger(++m_TriggerIdBegin,obb,callback,this,maplogic);
	m_AllTrigger[trig->GetId()] = trig;
	return trig;
}

LC_ServerMap_Trigger* LC_ServerMap::GetTrigger(int id)
{
	mem::map<int,LC_ServerMap_Trigger*>::iterator it=m_AllTrigger.find(id);
	if (it==m_AllTrigger.end())
	{
		return NULL;
	}
	return it->second;
}

void LC_ServerMap::DelTrigger(int id)
{
	m_DelTrigger.insert(id);
}

void LC_ServerMap::SetGlobalMode( bool bMode )
{
	m_bGlobalMode = bMode;
}

//void LC_ServerMap::AddTrigger2Sector(LC_ServerMap_Trigger* trigger)
//{
//	if (trigger==NULL)
//	{
//		return;
//	}
//	MapTrigger_Type triggertype= trigger->GetTriggerType();
//	SectorIndexSet kNeighborSectorIndices;
//	if (triggertype==MapTrigger_Type_Circle)
//	{
//		Utility::UT_Circle circle=trigger->GetCircle();
//		int blocks_near = circle.iRadius / m_iSectorSize;
//		if(0 == blocks_near)
//		{
//			blocks_near = 1;
//		}
//		SectorIndex iCenterSector = ComputeSecctorIndex(circle.kCenterPoint.x,circle.kCenterPoint.y);
//		kNeighborSectorIndices = _computeSectorNeighbors(iCenterSector,blocks_near);
//		
//	}
//	else
//	{
//		Utility::UT_OBB obb=trigger->GetOBB();
//		int maxradius=UT_MathBase::MaxInt(obb.iHalfExtentX,obb.iHalfExtentY);
//		int blocks_near = maxradius / (m_iSectorSize);
//		if(0 == blocks_near)
//		{
//			blocks_near = 1;
//		}
//		SectorIndex iCenterSector = ComputeSecctorIndex(obb.kCenterPoint.x,obb.kCenterPoint.y);
//		kNeighborSectorIndices = _computeSectorNeighbors(iCenterSector,blocks_near);
//	}
//	mem::map<int,mem::set<SectorIndex> >::iterator triggerit=m_Trigger2Sector.find(trigger->GetId());
//	if (triggerit==m_Trigger2Sector.end())
//	{
//		mem::set<SectorIndex> temp;
//		std::pair<mem::map<int,mem::set<SectorIndex> >::iterator,bool> ret=m_Trigger2Sector.insert(std::make_pair(trigger->GetId(),temp));
//		triggerit=ret.first;
//	}
//
//	SectorIndexSetIter kSectorIndexIterBe = kNeighborSectorIndices.begin();
//	SectorIndexSetIter kSectorIndexIterEd = kNeighborSectorIndices.end();
//	
//	for( ; kSectorIndexIterBe != kSectorIndexIterEd; ++kSectorIndexIterBe)
//	{
//		mem::map<SectorIndex,mem::set<int> >::iterator sectorit=m_Sector2Trigger.find(*kSectorIndexIterBe);
//		if (sectorit==m_Sector2Trigger.end())
//		{
//			mem::set<int> temp;
//			temp.insert(trigger->GetId());
//			m_Sector2Trigger.insert(std::make_pair(*kSectorIndexIterBe,temp));
//		}
//		else
//		{
//			sectorit->second.insert(trigger->GetId());
//		}
//		triggerit->second.insert(*kSectorIndexIterBe);
//	}
//}
//
//void LC_ServerMap::DelTriggerInSector(LC_ServerMap_Trigger* trigger)
//{
//	
//}
//
//void LC_ServerMap::GetSectorTrigger(int sectorindex,mem::vector<LC_ServerMap_Trigger*>& result)
//{
//	
//}
//
//int LC_ServerMap::GetAllTriggerSectorPlayerCount(LC_ServerMap_Trigger* trigger)
//{
//	
//}
