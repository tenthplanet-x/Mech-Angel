#include "ClientPCH.h"
#include "PF_ClientPathManager.h"
#include "SE_SceneManager.h"
#include "SE_SceneWorldChunk.h"
#include "TS_WorldConfig.h"
#include "MG_MessageBase.h"
#include "PF_PathMesh.h"
#include "PF_PathFinder.h"
#include "MG_MessageCenter.h"
using namespace Utility;
using namespace Path;
float PF_ClientTerrainCallback::getHeightAt( tSigned32 x, tSigned32 y )
{
	return getHeightAtWithPrecision( x , y , 0.1f , 0.1f );
}

float PF_ClientTerrainCallback::getHeightAtWithPrecision( tSigned32 x, tSigned32 y, float precisionX, float precisionY )
{
	T_PROFILE("SE_TerrainCallback::getHeightAtWithPrecision");
	static SE_SceneTerrainLocalInfo kTerrainInfo;
	kTerrainInfo._bGetHeight = true;
	kTerrainInfo._bGetNormal = false;
	const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
		SE_SceneManager::GetSingleton().GetTerrainLocalInfo(
		( Int2Float( x + m_lX ) + precisionX ) / GfxGetWorldConfig().CFG_PATHENGINE_SCALE ,
		( Int2Float( y + m_lY ) + precisionY ) / GfxGetWorldConfig().CFG_PATHENGINE_SCALE );

	//GfxWriteDefaultLog( LOG_INFO_TAG , "TerrainCB Get %d %d Height" , x , y );

	float fHeight = rkTerrainInfo._fLocalHeight * GfxGetWorldConfig().CFG_PATHENGINE_SCALE;

	return fHeight;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PF_ClientPathManager::_load_mesh_federation( const StringType& kPathFileBase , const StringType& rkDecoration )
{
	_load_master_federation(kPathFileBase,rkDecoration);
	if ( m_pkMeshFederation == NULL )
	{
		return;
	}
	_preload_federation_tiles( kPathFileBase , rkDecoration , false );
}

iTerrainCallBack* PF_ClientPathManager::_create_height_callback( tSigned32 tileIndex )
{ 
	if ( tileIndex == AVENUE_PATH_ID )
	{
		iTerrainCallBack* pkCb = T_NEW_D PF_ClientTerrainCallback( 0 , 0 );
		return pkCb;
	}
	else
	{
		tSigned32 x,y;
		m_pkMeshFederation->getTileCentre( tileIndex , x, y );
		iTerrainCallBack* pkCb = T_NEW_D PF_ClientTerrainCallback( x, y);
		return pkCb;
	}
}

PF_ClientPathManager::PF_ClientPathManager()
{
	m_bLoadAvenue = false;
}

PF_ClientPathManager::~PF_ClientPathManager()
{

}

bool PF_ClientPathManager::RecvMessage( MG_MessageBase& rkMessage )
{
	bool bMsgHandle = false;
	switch (rkMessage.m_eMsgType)
	{
	case MGT_TIMER_TICK:
		//Update( rkMessage.m_fParam1 );
		break;
	case MGT_LOGIC_PLAYER_MOVE:
		{
			bMsgHandle = true;
			/*
			当收到玩家移动的消息后，检查当前活跃的PathMesh ID集合
			如果不在集合中，那么就生成一个新的加载消息发送给后台线程执行
			同时把不在视野范围内的PathMesh删除，同样也是发送消息的方式
			*/
			T_PROFILE("ClientPathManager::PlayerMove");
			NiPoint3 kCameraLocation;
			kCameraLocation.x = rkMessage.m_fParam1;
			kCameraLocation.y = rkMessage.m_fParam2;
			kCameraLocation.z = rkMessage.m_fParam3;
			_compute_active_path_tile( kCameraLocation );

		}
	default:
		break;
	}
	return bMsgHandle;
}


void PF_ClientPathManager::_compute_active_path_tile( const NiPoint3& kEyeLocation )
{
	/*
	1.根据玩家当前位置和动态加载半径计算一个最大视野PlayerRange
	2.计算所有RepRange和PlayerRange相交的CurrentMeshSet
	3.对在当前活跃集合ActiveMeshSet里面而没有在CurrentMeshSet的Mesh就发送删除请求
	4.反之就发送加载请求
	*/
	TileIDSet kCurrentSet;
	cHorizontalRange cEyeRange;
	float fSelectionRadius = g_settings.LOD_PATH_MESH_LOAD_WIDTH;
	cEyeRange.minX = (int)(kEyeLocation.x - fSelectionRadius);
	cEyeRange.maxX = (int)(kEyeLocation.x + fSelectionRadius);
	cEyeRange.minY = (int)(kEyeLocation.y - fSelectionRadius);
	cEyeRange.maxY = (int)(kEyeLocation.y + fSelectionRadius);

	GroundMeshMapIter iter;
	TRAV( m_kMeshes , iter )
	{
		cHorizontalRange cMeshRange = iter->second->GetWorldRepRange();
		if ( RangesTouchOrOverlap( cEyeRange , cMeshRange ) )
		{
			kCurrentSet.insert( iter->first );
		}
	}
	//_log_set( kCurrentSet , "Current Path Tile:" );
	TileIDSet kToDelete;
	TileIDSet kToLoad;
	TileIDSet::iterator iter2;
	TRAV( kCurrentSet , iter2 )
	{
		bool bNotInActiveSet = ( m_kActiveTile.find( *iter2 ) == m_kActiveTile.end() );
		bool bNotInLoadingSet = ( m_kLoadingTile.find( *iter2 ) == m_kLoadingTile.end() );
		bool bNotInUnloadSet = ( m_kUnLoadingTile.find( *iter2 ) == m_kUnLoadingTile.end() );
		bool bLoaded = m_kMeshes[ *iter2 ]->IsInitialized();
		if ( bNotInActiveSet && bNotInLoadingSet && !bLoaded && bNotInUnloadSet )
		{
			kToLoad.insert( *iter2 );
		}
	}
	TRAV( m_kActiveTile , iter2 )
	{
		bool bNotInVisibleSet = ( kCurrentSet.find( *iter2 ) == kCurrentSet.end() );
		bool bNotInUnloadSet = ( m_kUnLoadingTile.find( *iter2 ) == m_kUnLoadingTile.end() );
		//有时候会出现已经删除，但是还没有重新加载，这个时候是不需要再UnLoad的
		bool bLoaded = m_kMeshes[ *iter2 ]->IsInitialized();
		if ( bNotInVisibleSet && bNotInUnloadSet && bLoaded )
		{
			kToDelete.insert( *iter2 );
		}
	}
	
	//_log_set( kToDelete , "Request Delete Path Mesh:" );
	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	
	TRAV( kToDelete , iter2 )
	{
		pkMessageCenter->SendMessage( CreateMsg_ADD_PATH_UNLOAD_TASK( *iter2 ) , true );
		m_kUnLoadingTile.insert( *iter2 );
		m_kActiveTile.erase( *iter2 );
	}
	//_log_set( kToLoad , "Request Load Path Mesh:" );
	TRAV( kToLoad , iter2 )
	{
		pkMessageCenter->SendMessage( CreateMsg_ADD_PATH_LOAD_TASK( *iter2 ) , true );
		m_kLoadingTile.insert( *iter2 );
	}
	//主干道寻路片需要后台加载
	if ( m_pkAvenueMesh && !m_pkAvenueMesh->IsInitialized() && !m_bLoadAvenue )
	{
		//大地图寻路片只能Load一次
		m_bLoadAvenue = true;
		pkMessageCenter->SendMessage( CreateMsg_ADD_PATH_LOAD_TASK( AVENUE_PATH_ID ) , true );
	}
}

void PF_ClientPathManager::_log_set( TileIDSet& rkSet , const char* name )
{
	if ( rkSet.empty() )
	{
		return;
	}
	StringStreamType output;
	output<<name;
	TileIDSet::iterator iter;
	TRAV( rkSet , iter )
	{
		output<<*iter<<",";
	}
	GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH, "%s" , output.str().c_str() );
}

PF_ClientPathManager& PF_ClientPathManager::GetSingleton()
{
	return *(static_cast<PF_ClientPathManager*>( Path::PF_PathManager::GetSingletonPtr() ) );
}

void PF_ClientPathManager::FinishTileLoad( long lIndex )
{
	if ( lIndex != AVENUE_PATH_ID )
	{
		TileIDSet::iterator iter = m_kLoadingTile.find( lIndex );
		T_ASSERT( iter != m_kLoadingTile.end() );
		m_kLoadingTile.erase( iter );
		bool bNoDuplicate = m_kActiveTile.insert( lIndex ).second;
		T_ASSERT( bNoDuplicate );
	}
}
void PF_ClientPathManager::FinishTileUnLoad( long lIndex )
{
	TileIDSet::iterator iter = m_kUnLoadingTile.find( lIndex );
	T_ASSERT( iter != m_kUnLoadingTile.end() );
	m_kUnLoadingTile.erase( iter );
}

bool PF_ClientPathManager::UnLoadGroundMesh( )
{
	//切换地图的时候执行这里，需要保证后台线程已经退出
	//那么我们清理当前地图的寻路片加载数据结构，并释放
	//当前活跃的所有的寻路片对象资源
	m_kActiveTile.clear();
	m_kLoadingTile.clear();
	m_kUnLoadingTile.clear();
	m_bLoadAvenue = false;
	return PF_PathManager::UnLoadGroundMesh(  );
}