#include "ClientPCH.h"

#include "LC_ClientMap.h"
#include "LC_ClientSector.h"

#include "CF_HookPos.h"

using namespace GameLogic;
using namespace Utility;
//--------------------------------------------------------
LC_ClientMap::LC_ClientMap(GameLogic::LC_MapSizeType eMapSizeType)
: LC_MapBase(eMapSizeType)
{

}
//--------------------------------------------------------
LC_ClientMap::~LC_ClientMap()
{

}
//--------------------------------------------------------
void LC_ClientMap::Init()
{
	int iSectorCount = m_iSectorXNum * m_iSectorYNum;
	T_ASSERT(iSectorCount > 0);
#ifndef _MAP_VIEWER_BUILD
	for(int iIndex = 0; iIndex < iSectorCount; iIndex++)
	{
		LC_SectorBase* pkSector = T_NEW_D LC_ClientSector;
		pkSector->SetSectorIndex(iIndex);
		pkSector->SetMap(this);

        int iNeighborRadius = 1;
		SectorIndexSet kNeighbors1;
		_computeSectorNeighbors(iIndex,iNeighborRadius,kNeighbors1);

		pkSector->SetSectorNeighborIndexSet(kNeighbors1, iNeighborRadius * 2 + 1);

		m_kSectorPtrVec.push_back(pkSector);
	}
#endif
}
//--------------------------------------------------------
void LC_ClientMap::Update( float fCurrentTime,float fDeltaTime )
{
	//无需更新
}
//--------------------------------------------------------
void LC_ClientMap::UnInit()
{
	LC_SectorPtrVectorIter IterBe = m_kSectorPtrVec.begin();
	LC_SectorPtrVectorIter IterEd = m_kSectorPtrVec.end();
	for( ; IterBe != IterEd; ++IterBe)
	{
		LC_SectorBase* pkSector = (*IterBe);
		T_SAFE_DELETE(pkSector);
	}
	m_kSectorPtrVec.clear();
	kHookPosVec.clear();
}
//------------------------------------------------------------------------
void LC_ClientMap::Reset()
{
	LC_SectorPtrVectorIter IterBe = m_kSectorPtrVec.begin();
	LC_SectorPtrVectorIter IterEd = m_kSectorPtrVec.end();
	for( ; IterBe != IterEd; ++IterBe)
	{
		LC_SectorBase* pkSector = (*IterBe);
		if (pkSector)
			pkSector->Reset();
	}
	kHookPosVec.clear();
}
//--------------------------------------------------------
void LC_ClientMap::ChangeSector( GameLogic::LC_LogicObject* pkLogicObject )
{
	LC_MapBase::ChangeSector(pkLogicObject);

#pragma TODO("发送跨越消息")
}
//--------------------------------------------------------
bool LC_ClientMap::IsInNeighborSector( SectorIndex dwCenterSector,SectorIndex dwCompareSector )
{
	LC_SectorBase* pkCenterSector = GetSector(dwCenterSector);
	if ( pkCenterSector )
	{
		SectorIndexSet& kNeighborSectors = pkCenterSector->GetSectorNeighborIndexSet();
		if(kNeighborSectors.find(dwCompareSector) != kNeighborSectors.end())
			return true;
	}
	return false;
}

void LC_ClientMap::SetHookPos()
{
	kHookPosVec.clear();
	CF_HookPos::DataEntryMapExternIterator iter = CF_HookPos::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_HookPos::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(m_iMapResID == pkEntry->_lMapID)
		{
			TStringVector kVector = UT_BaseStringUtility::SplitString(pkEntry->_sLocation, ';');
			if(kVector.size() == 0)
				return;
			for (TStringVector::iterator it = kVector.begin(); it != kVector.end(); ++it)
			{
				TStringVector loc = UT_BaseStringUtility::SplitString(it->c_str(), ',');
				T_ASSERT(loc.size() == 3);
				kHookPosVec.push_back(Utility::UT_Vec3Int(String2Int(loc[0]), String2Int(loc[1]), String2Int(loc[2])));
			}
			break;
		}
	}
}