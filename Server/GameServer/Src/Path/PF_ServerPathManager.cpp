
#include "PF_ServerPathManager.h"
#include "MTF_MemFileStream.h"
#include "CF_WorldMapList.h"
#include "LC_Define.h"
#include "UT_Macro.h"
#include "LC_LogicMapFileCache.h"

using namespace Memory;
using namespace Utility;
using namespace FileSystem;
using namespace CSVFile;

PF_ServerPathManager::PF_ServerPathManager()
{

}

PF_ServerPathManager::~PF_ServerPathManager()
{
	PathGridMap::iterator iter;
	TRAV( m_kPathGrids , iter )
	{
		T_DELETE_D iter->second;
	}
	m_kPathGrids.clear();
}

bool PF_ServerPathManager::FindDirectFarawayPoint(int nMapID ,const Utility::UT_Vec3Int& srcpoint,const Utility::UT_Vec3Int& destpoint,int length,Utility::UT_Vec3Int& endpoint)
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if ( iter != m_kPathGrids.end() && iter->second)
	{
		return iter->second->FindDirectFarawayPoint( srcpoint , destpoint , length,endpoint );
	}
	return false;
}

bool PF_ServerPathManager::FindDirectFarawayPointByDis(int nMapID ,const Utility::UT_Vec3Int& srcpoint,const Utility::UT_Vec3Int& destpoint,int length,Utility::UT_Vec3Int& endpoint)
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if ( iter != m_kPathGrids.end() && iter->second)
	{
		return iter->second->FindDirectFarawayPointByDis( srcpoint , destpoint , length,endpoint );
	}
	return false;
}

bool PF_ServerPathManager::FindPath( int nMapID , UT_LinePath& rkPath , const Utility::UT_Vec3Int& rkStart , const Utility::UT_Vec3Int& rkEnd )
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if ( iter != m_kPathGrids.end() && iter->second)
	{
		return iter->second->FindPath( rkPath , rkStart , rkEnd );
	}
	return false;
}

bool PF_ServerPathManager::IsPosValid( int nMapID , const Utility::UT_Vec3Int& rkP0 )
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if ( iter != m_kPathGrids.end() && iter->second)
	{
		return !( iter->second->TestPointCollision( rkP0 ) );
	}
	return false;
}

bool PF_ServerPathManager::GetAllVaildCell( int nMapID , const Utility::UT_Vec3Int& center, int radius,mem::vector<Utility::UT_Vec2Int>& result)
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if ( iter != m_kPathGrids.end() && iter->second)
	{
		return iter->second->GetAllVaildCell( center,radius,result);
	}
	return false;
}

bool PF_ServerPathManager::RandomVaildPoint( int nMapID , Utility::UT_Vec3Int& center)
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if ( iter != m_kPathGrids.end() && iter->second)
	{
		return iter->second->RandomVaildPoint( center);
	}
	return false;
}

bool PF_ServerPathManager::GetCellPoint( int nMapID ,Utility::UT_Vec2Int& cell, Utility::UT_Vec3Int& point)
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if ( iter != m_kPathGrids.end() && iter->second)
	{
		return iter->second->GetCellPoint( cell,point);
	}
	return false;
}

long PF_ServerPathManager::GetMapSize(int nMapID)
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if (iter==m_kPathGrids.end() || NULL==iter->second)
	{
		return 0;
	}
	return iter->second->GetPathGridSize();
}

bool PF_ServerPathManager::CheckCellVaild(int nMapID,int x,int y)
{
	PathGridMap::iterator iter = m_kPathGrids.find( nMapID );
	if (iter==m_kPathGrids.end() || NULL==iter->second)
	{
		return false;
	}
	return iter->second->TestCellCollision(x,y);
}

bool PF_ServerPathManager::Load()
{
	CF_WorldMapList::DataEntryMapExternIterator iter = CF_WorldMapList::GetSingleton().GetIterator();
	int nPathCount = CF_WorldMapList::GetSingleton().GetEntryCount();
	LC_LogicMapFileCache* pCache = LC_LogicMapFileCache::GetSingletonPtr();

	while( iter.HasMoreElements() )
	{
		const CF_WorldMapList::DataEntry& rkValue = iter.PeekNextValue();
		//UT_ServerPathGrid* pkNewMap = T_NEW_D UT_ServerPathGrid;
		UT_ServerPathGrid* pkNewMap = pCache->AllocateServerPathGrid(rkValue._kPathFile);
		// memory checking: cache data..
		if( pkNewMap->Load( rkValue._kPathFile ) )
		{
			m_kPathGrids[rkValue._iID] = pkNewMap;
			int map_size = 0;
			switch(pkNewMap->GetPathGridSize())
			{
			case 64:
				{
					map_size = GameLogic::LMST_4_4;
					break;
				}
			case 128:
				{
					map_size = GameLogic::LMST_8_8;
					break;
				}
			case 256:
				{
					map_size = GameLogic::LMST_16_16;
					break;
				}
			case 512:
				{
					map_size = GameLogic::LMST_32_32;
					break;
				}
			case 768:
				{
					map_size = GameLogic::LMST_48_48;
					break;
				}
			}

			if( 0 == map_size)
			{
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"invaild map_size! size:[%d]",map_size);
			}
			//由于策划经常搞错地图尺寸，而程序无法直接获取正确数据需要美术提供，经常因为这个问题扯皮，每次时间至少半小时
			//为避免这个官僚问题继续浪费时间，做出了一个艰难的决定，读取地图寻路格子信息由此反推出地图的尺寸
			if( rkValue._iSizeType && map_size !=  rkValue._iSizeType)
			{
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"WorldMapList.csv，id[%d]:_iSizeType[%d] not same as[%d] , file%s:",rkValue._iID,rkValue._iSizeType,map_size,rkValue._kPathFile.c_str());
				CF_WorldMapList::DataEntry* ptr = const_cast<CF_WorldMapList::DataEntry*>(&rkValue);
				ptr->_iSizeType = map_size;
			}
		}
		else
		{
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"map resource load error! [%d]:%s",rkValue._iID,rkValue._kPathFile.c_str());
			T_DELETE_D pkNewMap;
		}
		iter.MoveNext();
	}
	return true;
}

