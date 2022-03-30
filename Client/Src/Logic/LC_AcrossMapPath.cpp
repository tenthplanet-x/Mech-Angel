#include "ClientPCH.h"
#include "LC_AcrossMapPath.h"

#include "CF_WorldMapList.h"
#include "CF_TransferArea.h"


using namespace CSVFile;
using namespace Utility;
LC_AcrossMapPath::LC_AcrossMapPath()
{
	Init();
}

LC_AcrossMapPath::~LC_AcrossMapPath()
{

}

bool LC_AcrossMapPath::Init()
{
	for (int i = 0;i < (int)m_MapExitInfoList.size();++i)
	{
		m_MapExitInfoList[i].m_vecMapExitPointList.clear();
	}

	m_MapExitInfoList.clear();
	m_bFindingAcrossMapPath = false;

	CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
	if (NULL == pkWorldMapList)
		return false;

	CF_WorldMapList::DataEntryMapExternIterator kMapListIter = pkWorldMapList->GetIterator();

	while(kMapListIter.HasMoreElements())
	{
		const CF_WorldMapList::DataEntry* pkData = kMapListIter.PeekNextValuePtr();
		//if (pkData)
		{
			MapExitInfo obj;
			obj.m_iMapID = pkData->_iID;
			obj.m_vecMapExitPointList.clear();
			m_MapExitInfoList.push_back(obj);
		}

		kMapListIter.MoveNext();
	}

	CF_LogicTriggerList* pkLogicTrigger = CF_LogicTriggerList::GetSingletonPtr();
	if (NULL == pkLogicTrigger)
		return false;

	CF_TransferArea* pkTransferArea = CF_TransferArea::GetSingletonPtr();
	if (NULL == pkTransferArea)
		return false;

	CF_LogicTriggerList::DataEntryMapExternIterator kIter = pkLogicTrigger->GetIterator();

	while(kIter.HasMoreElements())
	{
		const CF_LogicTriggerList::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData&&pkData->_eType == GameLogic::LTT_TRANSFER)
		{
			for (int i = 0;i < (int)m_MapExitInfoList.size();++i)
			{
				if (m_MapExitInfoList[i].m_iMapID == pkData->_lMapID)
				{
					CF_TransferArea::DataEntry* pkTransferData = pkTransferArea->GetEntryPtr(pkData->_lTypeParam);
					if (NULL == pkTransferData)
						break;
					
					MapExitPointData objData;
					objData.m_iTriggerTransferPointID = pkData->_iID;
					objData.m_iDstMapID		   = pkTransferData->_lMapID;

					m_MapExitInfoList[i].m_vecMapExitPointList.push_back(objData);
				}
			}
		}
	
		kIter.MoveNext();
	}

	return true;
}

bool LC_AcrossMapPath::UnInit()
{
	return true;
}

void LC_AcrossMapPath::ReUse()
{
	Init();
}

bool LC_AcrossMapPath::SetAcrossMapPath( int iCurrentMapID ,int iNewMapID )
{
	if (iCurrentMapID == iNewMapID)
		return false;

	m_vecAcrossMapPath.clear();
	m_vecHoldMapList.clear();
	
	bool bFind = _findAcrossMapPath(iCurrentMapID,iNewMapID);
	if (bFind)
		m_bFindingAcrossMapPath = true;

	return false;
}

void LC_AcrossMapPath::ClearAcrossMapPath()
{
	m_vecAcrossMapPath.clear();
	m_vecHoldMapList.clear();
	m_bFindingAcrossMapPath =false;
}

bool LC_AcrossMapPath::IsFindingAcrossMap()
{
	return m_bFindingAcrossMapPath;
}

bool LC_AcrossMapPath::_findAcrossMapPath( int iCurrentMapID ,int iNewMapID )
{
	for (int i = 0;i < (int)m_MapExitInfoList.size();++i)
	{
		if (m_MapExitInfoList[i].m_iMapID == iCurrentMapID)
		{
			MapExitPointList& rExitList = m_MapExitInfoList[i].m_vecMapExitPointList;
			int iTranferPointID = 0;
			bool bFind = _findExitPoint(rExitList,iNewMapID,iTranferPointID);
			if(bFind)
			{
				m_vecAcrossMapPath.push_back(iTranferPointID);
				return true;
			}
			else
			{
				for(int j = 0;j < (int)rExitList.size();j++)
				{
					if (_isHoldMap(rExitList[j].m_iDstMapID) == false)
						m_vecHoldMapList.push_back(rExitList[j].m_iDstMapID);
					else
						continue;
					
					bool bFind = _findAcrossMapPath(rExitList[j].m_iDstMapID,iNewMapID);
					if(bFind)
					{
						m_vecAcrossMapPath.push_back(rExitList[j].m_iTriggerTransferPointID);
						return true;
					}
					else
						_unHoldMap(rExitList[j].m_iDstMapID);
				}
			}		
		}
	}

	return false;
}

bool LC_AcrossMapPath::_findExitPoint( MapExitPointList& rList, int iMapID ,int& iTransferPoint)
{
	for (int i = 0;i < (int)rList.size();++i)
	{
		if (rList[i].m_iDstMapID == iMapID)
		{
			iTransferPoint = rList[i].m_iTriggerTransferPointID;
			return true;
		}
	}

	return false;
}	

bool LC_AcrossMapPath::_isHoldMap( int iMapID )
{
	for (int i = 0;i < (int)m_vecHoldMapList.size();++i)
	{
		if (m_vecHoldMapList[i] == iMapID)
			return true;
	}

	return false;
}

void LC_AcrossMapPath::_unHoldMap( int iMapID )
{
	mem::vector<int>::iterator iter = m_vecHoldMapList.begin();
	for (;iter != m_vecHoldMapList.end();)
	{
		if (*iter == iMapID)
		{	
			m_vecHoldMapList.erase(iter);
			break;
		}

		++iter;
	}
}

bool LC_AcrossMapPath::GetTransferPosLocation( int iCurrentMapID ,UT_Vec3Int& rPos)
{
	if (m_bFindingAcrossMapPath == false)
		return false;

	CF_LogicTriggerList* pkLogicTrigger = CF_LogicTriggerList::GetSingletonPtr();
	if (NULL == pkLogicTrigger)
		return false;

	for(int i = 0;i < (int)m_vecAcrossMapPath.size();i++)
	{
		if (m_vecAcrossMapPath[i] <= 0)
			continue;
		
		CF_LogicTriggerList::DataEntry* pkEntry = pkLogicTrigger->GetEntryPtr(m_vecAcrossMapPath[i]);
		if (pkEntry == NULL)
			continue;

		if (pkEntry->_lMapID != iCurrentMapID)
			continue;

		rPos = pkEntry->_kCenter;
		return true;
	}

	return false;
}