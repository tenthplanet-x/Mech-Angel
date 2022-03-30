#include "ClientPCH.h"
#include "LC_LogicMapFile.h"
#include "CF_LogicAreaInfo.h"
#include "LC_ClientLogicAreaManager.h"
#include "CF_WorldMapList.h"
#include "CF_UIAreaNameFilter.h"

using namespace GameLogic;
using namespace FileSystem;
using namespace CSVFile;


/************************************************************************/
/*                                                                      */
/************************************************************************/


LC_ClientLogicAreaManager::LC_ClientLogicAreaManager()
{
}

LC_ClientLogicAreaManager::~LC_ClientLogicAreaManager()
{
}

void LC_ClientLogicAreaManager::Init( int nAmount )
{
	/*
	AreaGridIDList t;
	m_kEntireGridIDList.swap(t);
	/*/
    m_kEntireGridIDList.resize( nAmount * nAmount );
	//*/
    m_kEntireGridFuncList.resize( 0 );
    m_fMapWidth = (float)nAmount;
}

bool LC_ClientLogicAreaManager::LoadClientLogicAreaData( const long lMapID )
{
	LC_ClientLogicMapManager* pkMapManager = LC_ClientLogicMapManager::GetSingletonPtr();

	//< 查表 WorldMapList
    CF_WorldMapList::DataEntry* pkData = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr( lMapID );
    if ( !pkData )
    {
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CF_WorldMapList中找不到MapID == %d 项!", lMapID );
        return false;
    }
    const StringType & strAreaInfoFile = pkData->_kClientLogicAreaInfo;
    const StringType & strLogicBinFile = pkData->_kClientLogicArea;
    if ( strAreaInfoFile.empty() )
    {
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MapID == %d 的_kLogicAreaInfo为空!", lMapID );
        return false;
    }
    if ( strLogicBinFile.empty() )
    {
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MapID == %d 的_kLogicArea为空!", lMapID );
        return false;
    }

	//< 读取 _LogicData.bin
    LC_LogicMapFile kFile;
    kFile.Init( &m_kLogicAreaInfoList, &m_kEntireGridIDList );
    bool bRead = kFile.ReadFromFile( strLogicBinFile );
    if (!bRead)
        return false;
    Init( kFile.m_pkLCMF->GetMapWidth() );

	//< 读取 _LogicAreaInfo.csv
    CF_LogicAreaInfo* pkLogicAreaInfo = T_NEW_D CF_LogicAreaInfo;
    pkLogicAreaInfo->SetCSVFileName( strAreaInfoFile );
    if ( !pkLogicAreaInfo->Load(strAreaInfoFile, false) )
    {
		T_DELETE_D pkLogicAreaInfo;
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "逻辑区域文件%s加载失败，确定文件是否存在", strAreaInfoFile.c_str() );
        return false;
    }

	//< 用 csv 中的信息覆盖 bin 文件中读取出来的
    GameLogic::LogicAreaInfoListIter i;
    TRAV( m_kLogicAreaInfoList , i )
    {
        LC_LogicAreaInfo* pkArea = *i;
        CF_LogicAreaInfo::DataEntry* pkEntry = pkLogicAreaInfo->GetEntryPtr( pkArea->lLogicAreaID );
        if ( pkEntry )
        {
            pkArea->lLogicAreaID	 = pkEntry->_iID;
			pkArea->kOriginAreaName	 = pkEntry->_kName;
            pkArea->kAreaName		 = pkMapManager->GetFilterShowAreaName(pkEntry->_kName);
            pkArea->lLogicAreaFuncID = pkEntry->_lFuncID;
            pkArea->kBGM			 = pkEntry->_kBGM;
			pkArea->kSpawnPos.clear();
			for(UT_SIMDataList::iterator ite = pkEntry->_kSpawnLocations.begin(); ite != pkEntry->_kSpawnLocations.end(); ++ite)
			{
				pkArea->kSpawnPos.push_back( UT_Vec3Int((*ite).X(), (*ite).Y(), (*ite).Z()) );
			}
        }
    }
	T_DELETE_D pkLogicAreaInfo;

    //对于一个初始化创建，不再修改的数据结构，不需要用RB树
    //排序后用BinarySearch即可满足查询的需求
    std::sort( m_kLogicAreaInfoList.begin() , m_kLogicAreaInfoList.end() , LogicAreaSorter() );

    return true;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/


LC_ClientLogicMapManager::LC_ClientLogicMapManager()
{

}

LC_ClientLogicMapManager::~LC_ClientLogicMapManager()
{

}

bool LC_ClientLogicMapManager::AddLogicMapData( int nMapID )
{
	//< check
    LogicMapList* pkLogicMapList = GetLogicMapList();
	//T_ASSERT( pkLogicMapList->find(nMapID) == pkLogicMapList->end() );

	if (pkLogicMapList->find(nMapID) == pkLogicMapList->end())
	{
		//< new
		LC_ClientLogicAreaManager *pkLogicAreaMng = T_NEW_D LC_ClientLogicAreaManager;
		pkLogicAreaMng->SetMapID(nMapID);

		if (pkLogicAreaMng->LoadClientLogicAreaData( nMapID ))
		{
			pkLogicMapList->insert( std::make_pair(nMapID, pkLogicAreaMng ) );
			return true;
		}
		else
		{
			T_DELETE_D pkLogicAreaMng;
		}
	}
	
	return false;
}

void LC_ClientLogicMapManager::Load()
{
	CSVFile::CF_UIAreaNameFilter::DataEntryMapExternIterator kIter = CSVFile::CF_UIAreaNameFilter::GetSingletonPtr()->GetIterator();
	while ( kIter.HasMoreElements() )
	{
		CSVFile::CF_UIAreaNameFilter::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		m_kUIAreaNameFilterName[pkEntry->_OriginAreaName] = pkEntry->_ShowAreaName;
		kIter.MoveNext();
	}
	/*
    CF_WorldMapList::DataEntryMapExternIterator iter = CF_WorldMapList::GetSingleton().GetIterator();
    while( iter.HasMoreElements() )
    {
        CF_WorldMapList::DataEntry& rkData = iter.PeekNextValue();
        AddLogicMapData( rkData._iID );
        iter.MoveNext();
    }*/
}

const StringType & LC_ClientLogicMapManager::GetFilterShowAreaName(const StringType &kAreaName)
{
	mem::map<StringType, StringType>::iterator it = m_kUIAreaNameFilterName.find(kAreaName);
	if (it != m_kUIAreaNameFilterName.end())
	{
		return it->second;
	}
	else
	{
		return kAreaName;
	}
}
