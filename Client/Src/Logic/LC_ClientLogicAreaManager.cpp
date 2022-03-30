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

	//< ��� WorldMapList
    CF_WorldMapList::DataEntry* pkData = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr( lMapID );
    if ( !pkData )
    {
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CF_WorldMapList���Ҳ���MapID == %d ��!", lMapID );
        return false;
    }
    const StringType & strAreaInfoFile = pkData->_kClientLogicAreaInfo;
    const StringType & strLogicBinFile = pkData->_kClientLogicArea;
    if ( strAreaInfoFile.empty() )
    {
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MapID == %d ��_kLogicAreaInfoΪ��!", lMapID );
        return false;
    }
    if ( strLogicBinFile.empty() )
    {
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MapID == %d ��_kLogicAreaΪ��!", lMapID );
        return false;
    }

	//< ��ȡ _LogicData.bin
    LC_LogicMapFile kFile;
    kFile.Init( &m_kLogicAreaInfoList, &m_kEntireGridIDList );
    bool bRead = kFile.ReadFromFile( strLogicBinFile );
    if (!bRead)
        return false;
    Init( kFile.m_pkLCMF->GetMapWidth() );

	//< ��ȡ _LogicAreaInfo.csv
    CF_LogicAreaInfo* pkLogicAreaInfo = T_NEW_D CF_LogicAreaInfo;
    pkLogicAreaInfo->SetCSVFileName( strAreaInfoFile );
    if ( !pkLogicAreaInfo->Load(strAreaInfoFile, false) )
    {
		T_DELETE_D pkLogicAreaInfo;
        GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "�߼������ļ�%s����ʧ�ܣ�ȷ���ļ��Ƿ����", strAreaInfoFile.c_str() );
        return false;
    }

	//< �� csv �е���Ϣ���� bin �ļ��ж�ȡ������
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

    //����һ����ʼ�������������޸ĵ����ݽṹ������Ҫ��RB��
    //�������BinarySearch���������ѯ������
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
