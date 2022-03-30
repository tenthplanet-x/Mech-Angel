#include "LC_ServerSector.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlaceHolderManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_PlayerHatred.h"
#include "NW_Helper.h"
#include "MC_MessagePackageHelp.h"
#include "CF_ItemList.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
//------------------------------------------------------------------
LC_ServerSector::LC_ServerSector()
: LC_SectorBase()
{
}
//------------------------------------------------------------------
LC_ServerSector::~LC_ServerSector()
{
}
//------------------------------------------------------------------
void LC_ServerSector::Update(float fCurrentTime,float fDeltaTime)
{
	//无需更新
}
//------------------------------------------------------------------
void LC_ServerSector::CheckUpdateFlag()
{
	if(!m_pkMap)
	{
		return;
	}
	//检查该格子周围9个格子是否可以取消激活
	SectorIndexSet& kAroundIndex = GetSectorNeighborIndexSet();
	SectorIndexSetIter IterNeighborBe = kAroundIndex.begin();
	SectorIndexSetIter IterNeighborEd = kAroundIndex.end();
	bool bHasPlayer = false;

	for ( ;IterNeighborBe != IterNeighborEd; ++IterNeighborBe)
	{
		LC_SectorBase* pkNeighborSector = m_pkMap->GetSector(*IterNeighborBe);
		if(!pkNeighborSector)
		{
			//GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"can't find sector");
			continue;
		}
		if(pkNeighborSector->GetPlayerCount() > 0)
		{
			bHasPlayer = true;
			break;
		}
	}

	if(bHasPlayer)
	{
		SetUpdateFlag(true);
	}
	else
	{
		SetUpdateFlag(false);
	}
}
//------------------------------------------------------------------
void LC_ServerSector::BroadCast(int32_t iMsgType, Protocol::MG_BODY* pkMsgBody)
{
	MulticastMsgToGateway(m_kPlayerIDPtrMap, iMsgType, pkMsgBody);
}
//------------------------------------------------------------------------
void LC_ServerSector::GetPlayerPtrList( LC_PlayerPtrList& playerPtrList )
{
	LC_PlayerIDPtrMapIter IterEd = m_kPlayerIDPtrMap.end();
	for(LC_PlayerIDPtrMapIter IterBe = m_kPlayerIDPtrMap.begin(); IterBe != IterEd; ++IterBe)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(IterBe->second);
		playerPtrList.push_back(pkServerPlayer);
	}
}
//------------------------------------------------------------------------
void LC_ServerSector::GetNPCPtrList(LC_NPCPtrList& npcPtrList)
{
	LC_NPCIDPtrMapIter itEnd = m_kNPCIDPtrMap.end();
	for (LC_NPCIDPtrMapIter it = m_kNPCIDPtrMap.begin();it != itEnd;++it)
	{
		LC_ServerNPC* npcPtr = (LC_ServerNPC*)((*it).second);
		npcPtrList.push_back(npcPtr);
	}
}
//------------------------------------------------------------------------
void LC_ServerSector::GetPlaceHolderPtrList(PlaceHolderListType& phPtrList)
{
	LC_PlaceHolderIDPtrMapIter IterBe = m_kPlaceHolderIDPtrMap.begin();
	LC_PlaceHolderIDPtrMapIter IterEd = m_kPlaceHolderIDPtrMap.end();

	for(  ; IterBe != IterEd; ++IterBe)
	{
		LC_ServerPlaceHolder* pkPlaceHolder = (LC_ServerPlaceHolder*)(*IterBe).second;
		if(!pkPlaceHolder->Empty())
		{
			phPtrList.push_back(pkPlaceHolder);
		}
	}
}
//------------------------------------------------------------------
void LC_ServerSector::AddNPCToFOV(object_id_type lNPCID)
{
	//更新FOV
	LC_PlayerBase* pkPlayer = NULL;
	LC_ActorFOVMapBase* pkFOVMap = NULL;

	LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter itEnd = m_kPlayerIDPtrMap.end();
	for( ; it != itEnd; ++it)
	{
		pkPlayer = it->second;
		pkFOVMap = pkPlayer->GetFOVMap();

		pkFOVMap->AddNPCIDToFOV(lNPCID);
	}
}
//------------------------------------------------------------------
void LC_ServerSector::RemoveNPCFromFOV(object_id_type lNPCID)
{
	//更新FOV
	LC_PlayerBase* pkPlayer = NULL;
	LC_ActorFOVMapBase* pkFOVMap = NULL;

	LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter itEnd = m_kPlayerIDPtrMap.end();
	for( ; it != itEnd; ++it)
	{
		pkPlayer = it->second;
		pkFOVMap = pkPlayer->GetFOVMap();

		pkFOVMap->RemoveNPCIDFromFOV(lNPCID);
	}
}
//------------------------------------------------------------------
void LC_ServerSector::AddPlayerToFOV(object_id_type lPlayerID)
{
	//更新FOV
	LC_PlayerBase* pkPlayer = NULL;
	LC_ActorFOVMapBase* pkFOVMap = NULL;

	LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter itEnd = m_kPlayerIDPtrMap.end();
	for( ; it != itEnd; ++it)
	{
		pkPlayer = it->second;
		pkFOVMap = pkPlayer->GetFOVMap();

		pkFOVMap->AddPlayerIDToFOV(lPlayerID);
	}
}
//------------------------------------------------------------------
void LC_ServerSector::RemovePlayerFromFOV(object_id_type lPlayerID)
{
	//更新FOV
	LC_PlayerBase* pkPlayer = NULL;
	LC_ActorFOVMapBase* pkFOVMap = NULL;

	LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter itEnd = m_kPlayerIDPtrMap.end();
	for( ; it != itEnd; ++it)
	{
		pkPlayer = it->second;
		pkFOVMap = pkPlayer->GetFOVMap();

		pkFOVMap->RemovePlayerIDFromFOV(lPlayerID);
	}
}
//------------------------------------------------------------------
void LC_ServerSector::AddPlaceHolderToFOV(object_id_type lPlaceHolderID)
{
	//更新FOV
	LC_PlayerBase* pkPlayer = NULL;
	LC_ActorFOVMapBase* pkFOVMap = NULL;
	LC_ServerPlaceHolderManager* pkPlaceHolderManager = SERVER_GET_PLACEHOLDER_MANAGER();

	LC_PlaceHolderBase* pkPlaceHolder = pkPlaceHolderManager->FindPlaceHolder(lPlaceHolderID);
	if (!pkPlaceHolder)
		return;

	CSVFile::CF_ItemList* pkItemList = CSVFile::CF_ItemList::GetSingletonPtr();

	LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter itEnd = m_kPlayerIDPtrMap.end();
	for( ; it != itEnd; ++it)
	{
		pkPlayer = it->second;
		if (!pkPlaceHolder->IsVisibleToMe(pkPlayer->GetID()))
			continue;

		pkFOVMap = pkPlayer->GetFOVMap();
		if ( pkFOVMap->GetPlaceHolderShowLevel(lPlaceHolderID) == PHL_INVALD )
		{
			CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(pkPlaceHolder->GetReward().GetItemContext().GetBackPackValueByType(LPSFET_TYPE_ID));
			pkFOVMap->AddPlaceHolderShowLevel(lPlaceHolderID,pkItemData ? pkItemData->_lGrade : ITEM_GRADE_WHITE, pkPlaceHolder->IsOwner(pkPlayer->GetID()));
		}

		pkFOVMap = pkPlayer->GetFOVMap();
		pkFOVMap->AddPlaceHolderIDToFOV(lPlaceHolderID);
	}
}
//------------------------------------------------------------------
void LC_ServerSector::RemovePlaceHolderFromFOV(object_id_type lPlaceHolderID)
{
	//更新FOV
	LC_PlayerBase* pkPlayer = NULL;
	LC_ActorFOVMapBase* pkFOVMap = NULL;

	LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter itEnd = m_kPlayerIDPtrMap.end();
	for( ; it != itEnd; ++it)
	{
		pkPlayer = it->second;
		pkFOVMap = pkPlayer->GetFOVMap();

		//LC_ServerPlaceHolderManager* pkPlaceHolderManager = SERVER_GET_PLACEHOLDER_MANAGER();
		//LC_PlaceHolderBase* pkPlaceHolder = pkPlaceHolderManager->FindPlaceHolder(lPlaceHolderID);

		pkFOVMap->RemovePlaceHolderIDFromFOV(lPlaceHolderID);
	}
}
//------------------------------------------------------------------
