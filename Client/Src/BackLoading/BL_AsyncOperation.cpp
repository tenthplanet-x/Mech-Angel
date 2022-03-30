#include "ClientPCH.h"
#include "NiApplicationMetrics.h"
#include "BL_AsyncOperation.h"
#include "BL_AsyncOperationManager.h"
#include "FX_ClientFxManager.h"
#include "FX_Composite.h"
#include "GD_ClientAsyncModelManager.h"
#include "IG_Material.h"
#include "IG_MaterialManager.h"
#include "PF_PathMesh.h"
#include "SE_SceneWorldChunkCluster.h"
#include "SE_SceneWorldTile.h"
#include "UE_UiEffectManager.h"
#include "UT_FileManagerBase.h"
#include "UI_UIManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "ClientConfigManager.h"
#include "YunYingPlatform.h"
#include "CF_GfxSmoothRelease.h"
#include "AS_ClientSoundManager.h"
#include "UT_Curl.h"

using namespace Data;
using namespace FX;
using namespace GUISys;
using namespace Path;
using namespace Utility;

//==============================================================================
/*
	������USE_MULTI_LOAD_THREADS��ʹ�������첽�����̡߳�
	ԭ��BL_AsyncOperationManager�������ֻ��һ���첽�����̣߳����������в��������ڴ��߳��м��أ����̻߳������ܣ�
��˲������첽�����߳̾������⣬��Ϊֻ��һ���߳��ڴ�������߳�ÿ��ֻ����һ��BL_AsyncOperation������ͬʱ��������BL_AsyncOperation��
��˲��ÿ��Ǹ�Operation֮�侺�������⡣

	�������ֻ�����΢���»����һ�����⣺�����еĸ�Operation���е�����Դ��û׼���õģ��е��������Ѿ������ع��ģ�
��Ϊһ��ֻ�ܴ���һ��Op�������Op�������ʱ�������Դδ׼���ã��������ļ�ϵͳ������Դ����˶����к���Op����ʹ��Դ�Ѿ�׼���ã�
Ҳ���ܵõ���ʱ������Ҫ�ȴ�ǰһ��Op�����ꡢLoad�꣬�������һ�����������̣߳�ֻ������Դ�Ѿ�׼���õ�Op��δ׼���õ�
��Ȼ����ԭ�ȵļ����̴߳�����Op֮���ǲ�Ӧ�ô��ڼ���˳������ģ����򡮸��������̡߳�����ʹ�á�

	���ڿ����ˡ����������̡߳�������������Op��ͬʱ������ԭ�ȵ���ƻ��������ƣ���Op����ʱ��ʵ����race condition��
����UI��Terrain��Model���û����GD_MaterialManager::CreateMaterial�������ʣ����ú�����ʵ�ǲ�֧�ֶ��̵߳ġ�
�ֱ���LuaState������mod��gfx�õģ�Ҳ��֧�ֶ��̡߳�
�����Ҫ����Op Load֮ǰ���������������Э���������߳�֮��Ĺ�ϵ�������߳��޹أ�
��Ӧ�ü���BL_AsyncOperation::Load��BL_AsyncOperation::UnLoad������ΪBL_AsyncOperationManager::ExecuteNextOperationֻ������������������
BL_AsyncOperation���������������̵߳��õģ�ʹ�ô����ģ�����������m_bIsResReady=true��

	�����������̵߳�����£����߳̿��ڡ�Ԥȡ���׶Σ����أ���������ʵ��Load�׶Σ���������Prefetch���棨Prefetch�ڲ�Ҳ���������õ���race condition����
�������Ͳ�����ΪLoadһ����Դʱ���ȱ�Lock��Ȼ��ͨ���ڲ��ļ�ϵͳȥ������Դ����������Op��ΪLock���޷�������

	��Ȼ�����������̣߳���Ϊ���Ĺ�ϵ��ʵ������Ч����ֻ��һ�������̵߳�ʱ��һ������������
һ���߳̿�����Prefetch����һ�������Load
*/
#ifdef USE_MULTI_LOAD_THREADS
	static Utility::UT_Lock	gs_AsyncOpLock;
#endif

//==============================================================================
// class BL_AsyncTerrainOperation
//==============================================================================
BL_AsyncTerrainOperation::BL_AsyncTerrainOperation(void)
{
	m_eOpType = OT_TERRAIN;
	m_nPriorityValue = TERRAIN_LOAD_OP_PRIORITY;
	m_pkOperateMap = NULL;
}

BL_AsyncTerrainOperation::~BL_AsyncTerrainOperation(void)
{
}

void BL_AsyncTerrainOperation::Init(SE_SceneWorldMap* pkOperateMap , const SE_ChunkIndexPool& rkLoadingChunks)
{
	m_pkOperateMap = pkOperateMap;
#if (_MSC_VER == 1600)
	SE_ChunkIndexPool::const_iterator iter;
	TRAV( rkLoadingChunks , iter )
	{
		m_kWorkSetIndexMap.insert( *iter );
	}
#else
	std::copy(rkLoadingChunks.begin(), rkLoadingChunks.end(),
		inserter(m_kWorkSetIndexMap, m_kWorkSetIndexMap.begin()));
#endif

#ifdef USE_MULTI_LOAD_THREADS
	m_bIsResReady = false;	// �����ͼʱ��Tile�����أ�����Chunkδ����
#endif
}

void BL_AsyncTerrainOperation::Reset(void)
{
	m_pkOperateMap = NULL;
	m_eState = LS_END;
	m_kWorkSetIndexMap.clear();
}

bool BL_AsyncTerrainOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT( m_eState == LS_START_LOADING );

	SE_ChunkIndexPoolIter IterBe = m_kWorkSetIndexMap.begin();
	SE_ChunkIndexPoolIter IterEd = m_kWorkSetIndexMap.end();
	for(; IterBe != IterEd; ++IterBe)
	{
		SE_ChunkIndex& rkChunkIndex = (*IterBe).second;

		int iTileIndexX = rkChunkIndex._tileIndexX;
		int iTileIndexY = rkChunkIndex._tileIndexY;
		int iChunkClusterIndexX = rkChunkIndex._clusterIndexX;
		int iChunkClusterIndexY = rkChunkIndex._clusterIndexY;
		int iChunkIndexX = rkChunkIndex._chunkIndexX;
		int iChunkIndexY = rkChunkIndex._chunkIndexY;

		SE_SceneWorldTile* pkTile = m_pkOperateMap->GetWorldTile(iTileIndexX,iTileIndexY);
		pkTile->LoadTileFile();
		pkTile->SetActive( true );

		SE_SceneWorldChunkCluster* pkChunkCluster = pkTile->GetWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY);
		T_ASSERT(pkChunkCluster);

		int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,iTileIndexY,iChunkClusterIndexX,iChunkClusterIndexY,iChunkIndexX,iChunkIndexY);
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Begin LoadChunk work for %d" , iChunkID );

		bool bResult = pkChunkCluster->LoadChunk(iChunkIndexX,iChunkIndexY);
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "Finish LoadChunk work for %d" , iChunkID );
	}

	m_eState = LS_LOADING; 
	return true;
}

bool BL_AsyncTerrainOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );

	
	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Begin Terrain PostLoad" );
	ProfileStopWatch kWatch;
	kWatch.Start();

	SE_ChunkIndexPoolIter IterBe = m_kWorkSetIndexMap.begin();
	SE_ChunkIndexPoolIter IterEd = m_kWorkSetIndexMap.end();
	for(; IterBe != IterEd; ++IterBe)
	{
		SE_ChunkIndex& rkChunkIndex = (*IterBe).second;

		int iTileIndexX = rkChunkIndex._tileIndexX;
		int iTileIndexY = rkChunkIndex._tileIndexY;
		int iChunkClusterIndexX = rkChunkIndex._clusterIndexX;
		int iChunkClusterIndexY = rkChunkIndex._clusterIndexY;
		int iChunkIndexX = rkChunkIndex._chunkIndexX;
		int iChunkIndexY = rkChunkIndex._chunkIndexY;
		T_ASSERT(m_pkOperateMap);
		SE_SceneWorldTile* pkTile = m_pkOperateMap->GetWorldTile(iTileIndexX,iTileIndexY);
		T_ASSERT(pkTile);
		pkTile->PostLoad();

		SE_SceneWorldChunkCluster* pkChunkCluster = pkTile->GetWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY);
		T_ASSERT(pkChunkCluster);

		int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,iTileIndexY,iChunkClusterIndexX,iChunkClusterIndexY,iChunkIndexX,iChunkIndexY);
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Begin PosLoadChunk work for %d" , iChunkID );

		pkChunkCluster->PostLoadChunk(iChunkIndexX,iChunkIndexY);
		pkChunkCluster->SetActive( true );
		pkChunkCluster->SetLoadingFlag( iChunkIndexX , iChunkIndexY , false );

		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Finish PosLoadChunk work for %d" , iChunkID );
	}
	float fTime = kWatch.End();
	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "End Terrain PostLoad Use %f ms" , kWatch.End() );
	NIMETRICS_APPLICATION_AGGREGATEVALUE( CHUNK_POST_LOAD_TIME , fTime );

	m_eState = LS_LOADED;

	/*float fTime = kWatch.End();

	if ( fTime > 1.0f )
	{
		GfxWriteLog(LOG_TAG_STAT, LOG_SWITCH_STAT, 
			"Load Terrain Time %f", fTime );
	}*/
	return true;
}

bool BL_AsyncTerrainOperation::PreUnLoad(void)
{
	T_ASSERT( m_eState == LS_START_UNLOADING );
	SE_ChunkIndexPoolIter IterBe = m_kWorkSetIndexMap.begin();
	SE_ChunkIndexPoolIter IterEd = m_kWorkSetIndexMap.end();
	for (; IterBe != IterEd; ++IterBe)
	{
		SE_ChunkIndex& rkChunkIndex = (*IterBe).second;

		int iTileIndexX = rkChunkIndex._tileIndexX;
		int iTileIndexY = rkChunkIndex._tileIndexY;
		int iChunkClusterIndexX = rkChunkIndex._clusterIndexX;
		int iChunkClusterIndexY = rkChunkIndex._clusterIndexY;
		int iChunkIndexX = rkChunkIndex._chunkIndexX;
		int iChunkIndexY = rkChunkIndex._chunkIndexY;
		T_ASSERT(m_pkOperateMap);
		SE_SceneWorldTile* pkTile = m_pkOperateMap->GetWorldTile(iTileIndexX,iTileIndexY);
		T_ASSERT(pkTile);

		SE_SceneWorldChunkCluster* pkChunkCluster = pkTile->GetWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY);
		T_ASSERT(pkChunkCluster);

		int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,iTileIndexY,iChunkClusterIndexX,iChunkClusterIndexY,iChunkIndexX,iChunkIndexY);
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Begin PreUnLoadChunk work for %d" , iChunkID );

		pkChunkCluster->PreUnLoadChunk(iChunkIndexX,iChunkIndexY);
		pkChunkCluster->SetActive( false );
		pkChunkCluster->SetLoadingFlag( iChunkIndexX , iChunkIndexY , false );
		//���еļ��ر�Ƕ������߳�����
		if(pkTile->CanSafeUnLoadTileFile())
		{
			pkTile->PreUnLoad();
			pkTile->SetActive( false );
			/*GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, 
				"set tile active false 0x%x in preunload func" , pkTile->GetWorldTileFile() );*/
		}

		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "Finish PreUnLoadChunk work for %d" , iChunkID );
	}

	m_eState = LS_UNLOADING; 
	return true;
}

bool BL_AsyncTerrainOperation::UnLoad()
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	//ProfileStopWatch kWatch;
	//kWatch.Start();

	T_ASSERT( m_eState == LS_UNLOADING );
	SE_ChunkIndexPoolIter IterBe = m_kWorkSetIndexMap.begin();
	SE_ChunkIndexPoolIter IterEd = m_kWorkSetIndexMap.end();
	for(; IterBe != IterEd; ++IterBe)
	{
		SE_ChunkIndex& rkChunkIndex = (*IterBe).second;

		int iTileIndexX = rkChunkIndex._tileIndexX;
		int iTileIndexY = rkChunkIndex._tileIndexY;
		int iChunkClusterIndexX = rkChunkIndex._clusterIndexX;
		int iChunkClusterIndexY = rkChunkIndex._clusterIndexY;
		int iChunkIndexX = rkChunkIndex._chunkIndexX;
		int iChunkIndexY = rkChunkIndex._chunkIndexY;

		T_ASSERT(m_pkOperateMap);
		SE_SceneWorldTile* pkTile = m_pkOperateMap->GetWorldTile(iTileIndexX,iTileIndexY);
		T_ASSERT(pkTile);

		SE_SceneWorldChunkCluster* pkChunkCluster = pkTile->GetWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY);
		T_ASSERT(pkChunkCluster);

		int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,iTileIndexY,iChunkClusterIndexX,iChunkClusterIndexY,iChunkIndexX,iChunkIndexY);
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , "Begin UnLoadChunk work for %d" , iChunkID );

		pkChunkCluster->UnLoadChunk(iChunkIndexX,iChunkIndexY);
		pkChunkCluster->SetActive( false );
		//����Chunk��Unload�������ǲ������Tile�ļ���
		if ( !pkTile->GetActive() )
		{
			GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, 
				"unload tile  0x%x in Unload func" , pkTile->GetWorldTileFile() );
			pkTile->UnLoadTileFile();
		}
		

		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , "Finish UnLoadChunk work for %d" , iChunkID );
	}
	m_eState = LS_UNLOADED;

	//float fTime = kWatch.End();

	//if ( fTime > 1.0f )
	//{
	//	GfxWriteLog(LOG_TAG_STAT, LOG_SWITCH_STAT, 
	//		"Delete Terrain Time %f", fTime );
	//}

	return true;
}

void BL_AsyncTerrainOperation::SetStatus( bool bLoad )
{
	BL_AsyncOperation::SetStatus( bLoad );
	if ( bLoad )
	{
		SE_ChunkIndexPoolIter IterBe = m_kWorkSetIndexMap.begin();
		SE_ChunkIndexPoolIter IterEd = m_kWorkSetIndexMap.end();
		for(; IterBe != IterEd; ++IterBe)
		{
			SE_ChunkIndex& rkChunkIndex = (*IterBe).second;

			int iTileIndexX = rkChunkIndex._tileIndexX;
			int iTileIndexY = rkChunkIndex._tileIndexY;
			int iChunkClusterIndexX = rkChunkIndex._clusterIndexX;
			int iChunkClusterIndexY = rkChunkIndex._clusterIndexY;
			int iChunkIndexX = rkChunkIndex._chunkIndexX;
			int iChunkIndexY = rkChunkIndex._chunkIndexY;

			T_ASSERT(m_pkOperateMap);
			SE_SceneWorldTile* pkTile = m_pkOperateMap->GetWorldTile(iTileIndexX,iTileIndexY);
			T_ASSERT(pkTile);
			SE_SceneWorldChunkCluster* pkChunkCluster = pkTile->GetWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY);
			T_ASSERT(pkChunkCluster);
			int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,iTileIndexY,iChunkClusterIndexX,iChunkClusterIndexY,iChunkIndexX,iChunkIndexY);
			pkChunkCluster->SetLoadingFlag( iChunkIndexX , iChunkIndexY , true );
		}
	}
}

//==============================================================================
// class BL_AsyncActorOperation
//==============================================================================

BL_AsyncActorOperation::BL_AsyncActorOperation(void)
: m_pkAsyncModel(NULL)
, m_iModelIndex(0)
, m_uiShaderIdentifier(0)
{
	Reset();

	m_eOpType = OT_ACTOR;
	m_nPriorityValue = COMMON_MODEL_LOAD_OP_PRIORITY;
}

BL_AsyncActorOperation::~BL_AsyncActorOperation(void)
{
}

void BL_AsyncActorOperation::Init(	GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE eModelType, 
									Data::GD_ModelBase* pkAsyncModel, 
									const StringType& strModFileName, 
									int iModelIndex, unsigned int uiShaderIdentifier )
{
	m_pkAsyncModel      = pkAsyncModel;

    m_eModelType        = eModelType;
    m_strModFileName    = strModFileName;
	m_iModelIndex		= iModelIndex;
	m_uiShaderIdentifier = uiShaderIdentifier;

	m_bIsResReady       = false;

    switch (m_eModelType)
	{
	case GD_ClientAsyncModelManager::AMT_SCENE:
		{
			GD_ClientSceneModel* pkSceneModel = static_cast<GD_ClientSceneModel*>(pkAsyncModel);
			if ( pkSceneModel->IsWalkableModel() )
			{
				m_nPriorityValue = WALKABLE_LOAD_OP_PRIORITY;
			}
			else
			{
				m_nPriorityValue = SCENE_MODEL_LOAD_OP_PRIORITY;
			}
		}
    	break;
    case GD_ClientAsyncModelManager::AMT_PLAYER:
		{
			m_nPriorityValue = PLAYER_MODEL_LOAD_OP_PRIORITY;

			NiNodePtr spProxyNode = GD_ModelContentManager::GetSingletonPtr()->GetProxyNode();
			pkAsyncModel->SetProxyRootNode(spProxyNode);
		}
        break;
    case GD_ClientAsyncModelManager::AMT_NPC:
		{
			m_nPriorityValue = NPC_MODEL_LOAD_OP_PRIORITY;

			NiNodePtr spProxyNode = GD_ModelContentManager::GetSingletonPtr()->GetProxyNode();
			pkAsyncModel->SetProxyRootNode(spProxyNode);
		}
        break;
    case GD_ClientAsyncModelManager::AMT_ITEM:
        m_nPriorityValue = ITEM_MODEL_LOAD_OP_PRIORITY;
       break;
    case GD_ClientAsyncModelManager::AMT_INVALID:
        m_nPriorityValue = UNLOAD_OP_PRIORITY;
        break;
    default:
        m_nPriorityValue = COMMON_MODEL_LOAD_OP_PRIORITY;
        break;
    }

#ifdef USE_MULTI_LOAD_THREADS
	m_strResPath.clear();
	switch (m_eModelType)
	{
	case GD_ClientAsyncModelManager::AMT_SCENE:
	case GD_ClientAsyncModelManager::AMT_PLAYER:
	case GD_ClientAsyncModelManager::AMT_NPC:
	case GD_ClientAsyncModelManager::AMT_ITEM:
		{
			Data::ModelContentType	mct = pkAsyncModel->_doGetModelContentType();
			GD_ModelContentManager::GetSingletonPtr()->GetModelPathFileName(strModFileName, mct, m_strResPath);
			m_bIsResReady = UT_FileManagerBase::GetSingletonPtr()->AccessKey(m_strResPath.c_str());	//AccessFileֻ���ڴ���pkg��downloader��ʱ����Ч�������������Ч�����Բ��ı俪���������Ϊ
		}
		break;
	}
#endif
}
void BL_AsyncActorOperation::SetStatus( bool bLoad )
{
	BL_AsyncOperation::SetStatus( bLoad );
	if ( bLoad )
	{
		BL_AsyncOperationManager::GetSingleton().AddModelOperationMap( m_pkAsyncModel , this );
	}
}

void BL_AsyncActorOperation::Reset(void)
{
	m_eState = LS_END;

    m_eModelType = GD_ClientAsyncModelManager::AMT_INVALID;
    m_pkAsyncModel  = NULL;
    m_strModFileName = "";
	m_iModelIndex = 0;
	m_uiShaderIdentifier = 0;

    m_nPriorityValue = 80;
}

bool BL_AsyncActorOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	if (!m_bIsResReady && !m_strResPath.empty())
	{
		bool b = UT_FileManagerBase::GetSingletonPtr()->PrefetchFile(m_strResPath.c_str(), pCtrl);
		if (!b)
		{
			m_eState = LS_FAIL;
			return false;
		}
	}

	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT_CRITICAL_S( m_eState == LS_START_LOADING );
    T_ASSERT_CRITICAL_S(m_pkAsyncModel != NULL);
	T_ASSERT_CRITICAL_S(!m_strModFileName.empty());

	bool ret = m_pkAsyncModel->LoadResources(m_strModFileName, m_iModelIndex, m_uiShaderIdentifier);
	if (!ret)
	{
		GfxWriteLog(LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL , "load failed file %s in BL_AsyncActorOperation" , m_strModFileName.c_str() );
	}
    m_eState = LS_LOADING;

	//GfxWriteLog(LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, 
	//	"Finish Load Model[0x%x] %s ", m_pkAsyncModel , m_pkAsyncModel->GetResourceName().c_str());

	return true;
}

bool BL_AsyncActorOperation::PostLoad(void)
{
    T_ASSERT(m_eState == LS_LOADING);

    if (m_pkAsyncModel->NeedToBeReleased())
    {
        m_eState = LS_TRANSFER_TO_UNLOAD;
		GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL,
			"skip postload for model [0x%x]",
			m_pkAsyncModel );

        return true;
    }
/*
    ProfileStopWatch kWatch;
    kWatch.Start();
*/
    m_pkAsyncModel->InitAfterLoad();
/*
	float fTime = kWatch.End();
	unsigned int nFrame = 0;
	if ( NiRenderer::GetRenderer() )
	{
		nFrame = NiRenderer::GetRenderer()->GetFrameID();
	}
	GfxWriteLog(LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, 
        "End PostLoad Model[0x%x] %s in frame %d", m_pkAsyncModel , m_pkAsyncModel->GetResourceName().c_str() , nFrame );

	NIMETRICS_APPLICATION_AGGREGATEVALUE(ACTOR_POST_LOAD_TIME, fTime);
*/
	m_eState = LS_LOADED;

	return true;
}

bool BL_AsyncActorOperation::PreUnLoad(void)
{
	T_ASSERT( m_eState == LS_START_UNLOADING );
    T_ASSERT(m_pkAsyncModel != NULL);

	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
		"PreUnLoad Model[0x%x] %s" ,m_pkAsyncModel, m_pkAsyncModel->GetResourceName().c_str() );

    m_pkAsyncModel->PreRelease();

	m_eState = LS_UNLOADING;
    return true;
}

bool BL_AsyncActorOperation::UnLoad(void)
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT( m_eState == LS_UNLOADING );
    T_ASSERT(m_pkAsyncModel != NULL);

	/*ProfileStopWatch kWatch;
	kWatch.Start();*/


	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
		"UnLoad Model[0x%x] %s" , m_pkAsyncModel , m_pkAsyncModel->GetResourceName().c_str() );

    T_SAFE_DELETE(m_pkAsyncModel);

	//float fTime = kWatch.End();

	//if ( fTime > 1.0f )
	//{
	//	GfxWriteLog(LOG_TAG_STAT, LOG_SWITCH_STAT, 
	//	"Delete model Time %f", fTime );
	//}

	m_eState = LS_UNLOADED;

    return true;
}

//==============================================================================
// class BL_AsyncChangeAvatarOperation
//==============================================================================

BL_AsyncChangeAvatarOperation::BL_AsyncChangeAvatarOperation(void)
{
	Reset();
	m_nPriorityValue = CHANGE_AVATAR_OP_PRIORITY;
}

BL_AsyncChangeAvatarOperation::~BL_AsyncChangeAvatarOperation(void)
{
}

void BL_AsyncChangeAvatarOperation::Init(GD_ClientActorModel* pkActorModel)
{
	m_pkActorModel  = pkActorModel;
}

void BL_AsyncChangeAvatarOperation::Reset(void)
{
    m_eOpType                   = OT_ACTOR_ITEM;
    m_eState                    = LS_END;

	m_pkActorModel              = NULL;

    m_spActorNewSuitNiRootNode  = NULL;
}

bool BL_AsyncChangeAvatarOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT( m_eState == LS_START_LOADING );

    if (m_pkActorModel == NULL)
    {
        GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, 
            "Generate new suit-data but the actor model is NULL!");
        m_eState = LS_FAIL;
        return false;
    }
    m_spActorNewSuitNiRootNode = m_pkActorModel->GenerateNewSuitData();
    if (m_spActorNewSuitNiRootNode == NULL)
    {
		GfxWriteLog( LOG_MODEL_FILELINE, LOG_SWITCH_MODEL , 
			"Change Avatar for model failed %s" , m_pkActorModel->GetResourceName().c_str() );
        m_eState = LS_FAIL;
        return false;
    }
	GfxWriteLog( LOG_MODEL_FILELINE, LOG_SWITCH_MODEL , 
		"Change Avatar for model %s" , m_pkActorModel->GetResourceName().c_str() );
    m_eState = LS_LOADING;
	return true;
}

bool BL_AsyncChangeAvatarOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );

    T_ASSERT(m_spActorNewSuitNiRootNode != NULL);

    if (!GD_ClientAsyncModelManager::GetSingleton().IsAsyncModel(m_pkActorModel))
    {
		GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
			"Cannot apply new suit for inexist actor model [%s]", m_pkActorModel != NULL ? m_pkActorModel->GetModelFileName().c_str() : "NULL");

        m_eState = LS_FAIL;
        return false;
    }

    m_pkActorModel->ApplyNewSuit(m_spActorNewSuitNiRootNode);

	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
		"Postload for ChangeAvatar Model %s" , m_pkActorModel->GetResourceName().c_str() );

	m_eState = LS_LOADED;
    return true;
}

//==============================================================================
// class BL_AsyncFxLoadOperation
//==============================================================================

BL_AsyncFxLoadOperation::BL_AsyncFxLoadOperation(void)
{
	Reset();
}

BL_AsyncFxLoadOperation::~BL_AsyncFxLoadOperation(void)
{
}

void BL_AsyncFxLoadOperation::Init(FX_ClientFxManager::FxObject* pkFxObject)
{
	m_pkFxObject = pkFxObject;
    m_nPriorityValue = FX_LOAD_OP_PRIORITY;

#ifdef USE_MULTI_LOAD_THREADS
	m_strResPath = GfxGetDirConfig().GetDir(DC_MODEL_FX_PATH);
	m_strResPath += pkFxObject->strFileName;
	m_bIsResReady = UT_FileManagerBase::GetSingletonPtr()->AccessKey(m_strResPath.c_str());
#endif
}

void BL_AsyncFxLoadOperation::InitUnLoad(FX_ClientFxManager::FxObject* pkFxObject)
{
    m_pkFxObject = pkFxObject;
    m_nPriorityValue = UNLOAD_OP_PRIORITY;
	m_bIsResReady = true;
}

void BL_AsyncFxLoadOperation::Reset(void)
{
	m_eOpType = OT_GFX;
	m_pkFxObject = NULL;
	m_eState = LS_END;
}

bool BL_AsyncFxLoadOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	if (!m_bIsResReady)
	{
		bool b = UT_FileManagerBase::GetSingletonPtr()->PrefetchFile(m_strResPath.c_str(), pCtrl);
		if (!b)
		{
			m_eState = LS_FAIL;
			return false;
		}
	}

	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT( m_eState == LS_START_LOADING );
    T_ASSERT(m_pkFxObject != NULL);
    FX_ClientFxManager::GetSingleton().InitCompositeForFxObject(*m_pkFxObject);
	this->QueryDestroySmoothly(*m_pkFxObject);

	m_eState = LS_LOADING;

	return true;
}

void BL_AsyncFxLoadOperation::QueryDestroySmoothly(FX_ClientFxManager::FxObject& rkFxObject)
{
	CF_GfxSmoothRelease::DataEntryMapExternIterator kIter = CF_GfxSmoothRelease::GetSingleton().GetIterator();

	while( kIter.HasMoreElements() )
	{
		const CF_GfxSmoothRelease::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_kResName == rkFxObject.strFileName)	// ��Сд������ȫƥ��
		{
			rkFxObject.bIsDestroySmoothly = true;
			return;
		}

		kIter.MoveNext();
	}
}

bool BL_AsyncFxLoadOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );
    T_ASSERT(m_pkFxObject != NULL);
	FX_ClientFxManager::GetSingleton().PostCreateFxObject(*m_pkFxObject);

	m_eState = LS_LOADED;
	return true;
}

bool BL_AsyncFxLoadOperation::PreUnLoad(void)
{
    m_eState = LS_UNLOADING;

    return true;
}

bool BL_AsyncFxLoadOperation::UnLoad(void)
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT_CRITICAL_S(m_pkFxObject != NULL);
    T_SAFE_DELETE(m_pkFxObject->pkComposite);
    T_DELETE_E(m_pkFxObject, FxObject, Memory::MEMCATEGORY_GENERAL);

	m_eState = LS_UNLOADED;

	return true;
}

//==============================================================================
// class BL_AsyncUIImgLoadOperation
//==============================================================================

BL_AsyncUIImgLoadOperation::BL_AsyncUIImgLoadOperation(void)
{
	Reset();
	m_nPriorityValue = UI_LOAD_OP_PRIORITY;
	m_bLoad = false;
	m_iErase = 0;
	m_pkToDelete = NULL;
}

BL_AsyncUIImgLoadOperation::~BL_AsyncUIImgLoadOperation(void)
{
}

void BL_AsyncUIImgLoadOperation::Init( BYTE* pValue )
{
	m_sImgName = static_cast<char *>((void*)pValue);
	m_pkToDelete = NULL;

#ifdef USE_MULTI_LOAD_THREADS
	m_strResPath = GfxGetDirConfig().GetDir(DC_TEXTURE_UI);
	m_strResPath += m_sImgName;
	m_bIsResReady = UT_FileManagerBase::GetSingletonPtr()->AccessFile(m_strResPath.c_str());
#endif
}
void BL_AsyncUIImgLoadOperation::InitUnLoad( BYTE* pValue ,long iErase)
{
	m_pkToDelete = static_cast<IG_Material *>((void*)pValue);
	T_ASSERT_CRITICAL_S(m_pkToDelete->GetRefCount() == 0);
	m_iErase = (int)iErase;
	m_bIsResReady = true;
}

void BL_AsyncUIImgLoadOperation::Reset(void)
{
	m_eOpType = OT_UIIMG;
	m_eState = LS_END;
}

bool BL_AsyncUIImgLoadOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	if (!m_bIsResReady)
	{
		bool b = UT_FileManagerBase::GetSingletonPtr()->PrefetchFile(m_strResPath.c_str(), pCtrl);
		if (!b)
		{
			m_eState = LS_FAIL;
			return false;
		}
	}

	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT( m_eState == LS_START_LOADING );
	m_bLoad = false;
	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "UIImg %s Load Start!" ,m_sImgName.c_str() );

	IG_MaterialManager* pkMgr = IG_MaterialManager::GetSingletonPtr();
	if ( pkMgr )
	{
		IG_Material* spMaterial = pkMgr->GetMaterial( m_sImgName );
		if ( spMaterial == NULL )
		{
			GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Can't Get UIImg %s" , m_sImgName.c_str());

			m_eState = LS_FAIL;
			return false;
		}

		if(!spMaterial->IsLoaded())
		{
			m_bLoad = spMaterial->LoadImg();//ֱ������ͼƬʧ��
		}

		if ( m_bLoad == false )//����Img�ļ�ʧ��
		{
			GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Init UIImg %s failed" , m_sImgName.c_str());

			m_eState = LS_FAIL;
			return false;
		}

		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "UIImg %s Load Complete!" ,m_sImgName.c_str());
		m_eState = LS_LOADING;
		return true;
	}

	T_ASSERT(false);
	m_eState = LS_FAIL;

	return false;
}

bool BL_AsyncUIImgLoadOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );
	if(m_bLoad == false)
	{
		m_eState = LS_FAIL;
		return false;
	}

	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "UIImg %s PostLoad Start!" ,m_sImgName.c_str() );

	IG_MaterialManager* pkMgr = IG_MaterialManager::GetSingletonPtr();
	if ( pkMgr )
	{
		bool bResult = pkMgr->FinishLoadingMaterial( m_sImgName );

		if ( !bResult )
		{
			GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Can't do PostLoad for UIImg %s" , m_sImgName.c_str() );

			m_eState = LS_FAIL;
			return false;
		}
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "UIImg name %s PostLoad Complete!" ,m_sImgName.c_str() );

	}
	m_eState = LS_LOADED;

	return true;
}

bool BL_AsyncUIImgLoadOperation::PreUnLoad(void)
{
	m_eState = LS_UNLOADING;

	//GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Finish UIImg UnLoading Task for Pointer %0x" ,m_pkToDelete);
	if(m_pkToDelete)
	{
		m_pkToDelete->PreUnloadImg();
	}

	return true;
}

bool BL_AsyncUIImgLoadOperation::UnLoad(void)
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif


	T_ASSERT( m_eState == LS_UNLOADING );
	if(m_pkToDelete)
	{
		m_pkToDelete->UnloadImg();

		unsigned int nRefCnt = m_pkToDelete->GetRefCount();
		if (nRefCnt != 0)
		{
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "BL_AsyncUIImgLoadOperation::UnLoad, FatalError, Delete Ref Object, RefCnt=%d, IG_Material=0x%08x", nRefCnt, m_pkToDelete);
		}
		NiDelete m_pkToDelete;
		m_pkToDelete = NULL;
	}
	m_eState = LS_UNLOADED;

	return true;
}
//==============================================================================
// class BL_AsyncPathMeshOperation
//==============================================================================

BL_AsyncPathMeshOperation::BL_AsyncPathMeshOperation(void)
{
	Reset();
	m_nPriorityValue = PATHMESH_LOAD_OP_PRIORITY;
}

BL_AsyncPathMeshOperation::~BL_AsyncPathMeshOperation(void)
{
}

void BL_AsyncPathMeshOperation::Init( long lTileIndex )
{
	m_lTileIndex = lTileIndex;
#ifdef USE_MULTI_LOAD_THREADS
	m_bIsResReady = true;	// ��ȥ��ͼǰ��Path�����أ��� Map_Res_.xml
#endif
}

void BL_AsyncPathMeshOperation::Reset(void)
{
	m_eOpType = OT_PATH;
	m_eState = LS_END;
	m_lTileIndex = -1;
}

bool BL_AsyncPathMeshOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	T_ASSERT( m_eState == LS_START_LOADING );
	GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH ,  "PathMesh %d Load Start!" , m_lTileIndex );
	PF_PathManager* pkMgr = PF_PathManager::GetSingletonPtr();
	if ( m_lTileIndex != -1 && pkMgr )
	{
		PF_PathMesh* pkMesh = pkMgr->GetMeshByTile( m_lTileIndex );
		if ( pkMesh == NULL )
		{
			GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  "Can't Find Pathmesh %d" , m_lTileIndex );
			m_eState = LS_FAIL;
			return false;
		}
		if ( pkMesh->Load() == false )
		{
			GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  "Load PathMesh %d Failed" , m_lTileIndex );
			m_eState = LS_FAIL;
			return false;
		}
		m_eState = LS_LOADING;
		return true;
	}
	T_ASSERT(false);
	m_eState = LS_FAIL;

	return false;
}

bool BL_AsyncPathMeshOperation::PostLoad(void)
{
	PF_PathManager* pkMgr = PF_PathManager::GetSingletonPtr();
	if ( m_lTileIndex != -1 && pkMgr )
	{
		PF_PathMesh* pkMesh = pkMgr->GetMeshByTile( m_lTileIndex );
		if ( pkMesh == NULL )
		{
			GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  "Can't Find Pathmesh %d" , m_lTileIndex );
			m_eState = LS_FAIL;
			return false;
		}
		pkMesh->PostLoad();
		GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  
			"PathMesh %d Load Complete! Order ID %d" ,m_lTileIndex , m_nWorkingQueueID );
		m_eState = LS_LOADED;
		return true;
	}
	m_eState = LS_FAIL;
	return false;
}

bool BL_AsyncPathMeshOperation::PreUnLoad(void)
{
	PF_PathManager* pkMgr = PF_PathManager::GetSingletonPtr();
	if ( m_lTileIndex != -1 && pkMgr )
	{
		PF_PathMesh* pkMesh = pkMgr->GetMeshByTile( m_lTileIndex );
		if ( pkMesh == NULL )
		{
			GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  "Can't Find Pathmesh %d" , m_lTileIndex );
			return false;
		}
		GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  
			"PathMesh %d PreUnload Complete! Order ID %d" ,m_lTileIndex , m_nWorkingQueueID );
		pkMesh->PreUnLoad();
		m_eState = LS_UNLOADING;
		return true;
	}
	m_eState = LS_UNLOADING;
	return true;
}

bool BL_AsyncPathMeshOperation::UnLoad(void)
{
#ifdef USE_MULTI_LOAD_THREADS
	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	PF_PathManager* pkMgr = PF_PathManager::GetSingletonPtr();
	if ( m_lTileIndex != -1 && pkMgr )
	{
		PF_PathMesh* pkMesh = pkMgr->GetMeshByTile( m_lTileIndex );
		if ( pkMesh == NULL )
		{
			GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  "Can't Find Pathmesh %d" , m_lTileIndex );
			return false;
		}
		pkMesh->UnLoad();
		GfxWriteLog( LOG_PATH_FILELINE, LOG_SWITCH_PATH  ,  
			"PathMesh %d UnLoad Complete! Order ID %d" ,m_lTileIndex , m_nWorkingQueueID );
	}
	m_eState = LS_UNLOADED;
	return true;
}

//==============================================================================
// class BL_AsyncPlayVideoOperation
//==============================================================================
char	BL_AsyncPlayVideoOperation::ms_kValidVideo[MAX_PATH];

BL_AsyncPlayVideoOperation::BL_AsyncPlayVideoOperation(void)
{
	Reset();
	m_iVideoType = 0;
	m_nPriorityValue = VIDEO_LOAD_OP_PRIORITY;
	m_bLoad = false;
	m_uLoopCount = 0;
}

BL_AsyncPlayVideoOperation::~BL_AsyncPlayVideoOperation(void)
{
}

void BL_AsyncPlayVideoOperation::Init( int iVideoType, const char *cpImagename, const char *cpFilename, unsigned uLoopCount )
{
	m_iVideoType	= iVideoType;
	m_kImagename	= cpImagename;
	m_kFilename		= cpFilename;
	m_uLoopCount	= uLoopCount;
	strncpy(ms_kValidVideo, cpFilename, MAX_PATH-1);	//���µĲ�������������ڲ��ţ����߳���ִ��

	UI_UIManager::GetSingletonPtr()->PlayNilVideo(iVideoType);

#ifdef USE_MULTI_LOAD_THREADS
	m_bIsResReady = false;	//< �����ж��ļ��Ƿ�����ˣ���ΪҪ��������dll�����̶���downloader��һ�飬ȷ���ļ�����
#endif
}

void BL_AsyncPlayVideoOperation::Reset(void)
{
	m_eOpType = OT_VIDEO;
	m_eState = LS_END;
}

bool BL_AsyncPlayVideoOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	if (!m_bIsResReady)
	{
		IDownloader* downloader = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
		if (downloader != NULL)
		{
			//< ������dll��ǰ��4��dll��WinVideo.libֱ�����ã�����2������4��dll����
			downloader->DownloadExt("avcodec-52.dll");
			downloader->DownloadExt("avformat-52.dll");
			downloader->DownloadExt("avutil-49.dll");
			downloader->DownloadExt("swscale-0.dll");
			downloader->DownloadExt("libmp3lame-0.dll");
			downloader->DownloadExt("libx264-102.dll");

			//< pre-load
			//< ����Ԥ�ȼ�����Ƶdll�������Ϸ�����д���Ƶ��ؽ��棬�ᵼ�����̳߳�ʱ�俨��
			static HANDLE 	h_avcodec	= LoadLibrary("avcodec-52.dll");
			static HANDLE	h_avformat	= LoadLibrary("avformat-52.dll");
			static HANDLE 	h_util		= LoadLibrary("avutil-49.dll");
			static HANDLE 	h_swscale	= LoadLibrary("swscale-0.dll");
			static HANDLE 	h_mp3lame	= LoadLibrary("libmp3lame-0.dll");
			static HANDLE 	h_x264		= LoadLibrary("libx264-0.dll");

			//< ��������Ƶ�ļ�
			StringType strFileName(m_kFilename);
			std::replace_if( strFileName.begin(), strFileName.end(), bind2nd(std::equal_to<char>(),'/'), '\\' );
			downloader->DownloadExt(strFileName.c_str());	//�Ѿ�������ɵģ�DownloadExt�����ظ��£���ʹm_bIsResReady�ж�ʧ��Ҳ��Ҫ��
		}
	}

#else

	//< pre-load
	static HANDLE 	h_avcodec	= LoadLibrary("avcodec-52.dll");
	static HANDLE	h_avformat	= LoadLibrary("avformat-52.dll");
	static HANDLE 	h_util		= LoadLibrary("avutil-49.dll");
	static HANDLE 	h_swscale	= LoadLibrary("swscale-0.dll");
	static HANDLE 	h_mp3lame	= LoadLibrary("libmp3lame-0.dll");
	static HANDLE 	h_x264		= LoadLibrary("libx264-0.dll");

#endif

	T_ASSERT( m_eState == LS_START_LOADING );
	m_bLoad = true;
	m_eState = LS_LOADING;

	return true;
}

bool BL_AsyncPlayVideoOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );
	m_eState = LS_LOADED;

	if (UI_UIManager::GetSingletonPtr())
	{
		if (ms_kValidVideo == m_kFilename)	//ͨ����֤�Ĳ������ڲ��ţ����߳���ִ��
		{
			ms_kValidVideo[0] = 0;
			UI_UIManager::GetSingletonPtr()->EndVideo(m_iVideoType);
			UI_UIManager::GetSingletonPtr()->PlayVideo(m_iVideoType, m_kImagename.c_str(), m_kFilename.c_str(), m_uLoopCount);
			return true;
		}
	}

	// ���m_eState�ڽ���postload֮ǰ��LS_LOADING�����뷵��true;
	// ����ᵼ���̶߳�����
	return true;
}

bool BL_AsyncPlayVideoOperation::PreUnLoad(void)
{
	m_eState = LS_UNLOADING;

	return true;
}

bool BL_AsyncPlayVideoOperation::UnLoad(void)
{
	m_eState = LS_UNLOADED;

	return true;
}

//==============================================================================
// class BL_AsyncPlayAudioOperation
//==============================================================================

BL_AsyncPlayAudioOperation::BL_AsyncPlayAudioOperation()
{
	Reset();
	m_nPriorityValue = AUDIO_LOAD_OP_PRIORITY;
	m_bLoad = false;
	m_bPlayOK = false;
	m_iID = 0;
	m_pkArg = NULL;
	m_pkSoundInfo = NULL;
	m_uStartTimeMs = 0;
}

BL_AsyncPlayAudioOperation::~BL_AsyncPlayAudioOperation(void)
{
}

void BL_AsyncPlayAudioOperation::Init( int iID, AudioLoadArg * pkArg, void * pkSoundInfo )
{
	m_iID			= iID;
	m_pkArg			= pkArg;
	m_pkSoundInfo	= pkSoundInfo;
	m_uStartTimeMs	= timeGetTime();

#ifdef USE_MULTI_LOAD_THREADS
	m_strResPath	= GfxGetDirConfig().GetDir(CF_NODE_AUDIO);
	m_strResPath   += pkArg->strSoundName;
	m_bIsResReady = UT_FileManagerBase::GetSingletonPtr()->AccessFile(m_strResPath.c_str());
#endif
}

void BL_AsyncPlayAudioOperation::Reset(void)
{
	m_eOpType = OT_AUDIO;
	m_eState = LS_END;
}

bool BL_AsyncPlayAudioOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	if (!m_bIsResReady)
	{
		bool b = UT_FileManagerBase::GetSingletonPtr()->PrefetchFile(m_strResPath.c_str(), pCtrl);
		if (!b)
		{
			m_eState = LS_FAIL;
			return false;
		}
	}

	Utility::UT_AutoLock	lock(&gs_AsyncOpLock);
#endif

	m_bPlayOK = AS_ClientSoundManager::GetSingleton().PlaySound_AsyncLoading(m_pkArg);

	T_ASSERT( m_eState == LS_START_LOADING );
	m_bLoad = true;
	m_eState = LS_LOADING;

	return true;
}

bool BL_AsyncPlayAudioOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );
	m_eState = LS_LOADED;

	unsigned int nCostTime = timeGetTime() - m_uStartTimeMs;
	bool bTimeOut = bool(nCostTime >= m_pkArg->uTimeOutMs);

	if (bTimeOut)
		GfxWriteLog( LOG_TAG_WARNING, LOG_SWITCH_SOUND,  "%s, timeout: %d(ms), %d, %s", __FUNCTION__, nCostTime , m_iID, m_pkArg->strSoundName.c_str() );
	else
		GfxWriteLog( LOG_TAG_SOUND, LOG_SWITCH_SOUND,  "%s, cost: %d(ms), %d, %s", __FUNCTION__, nCostTime , m_iID, m_pkArg->strSoundName.c_str() );

	AS_ClientSoundManager::GetSingleton().PlaySound_AsyncPostLoad(m_bPlayOK, bTimeOut, m_iID, m_pkArg, m_pkSoundInfo);
	return true;
}

bool BL_AsyncPlayAudioOperation::PreUnLoad(void)
{
	m_eState = LS_UNLOADING;

	return true;
}

bool BL_AsyncPlayAudioOperation::UnLoad(void)
{
	m_eState = LS_UNLOADED;

	return true;
}

//==============================================================================
// class BL_AsyncLoadMapOperation
//==============================================================================

BL_AsyncLoadMapOperation::BL_AsyncLoadMapOperation(void)
{
	Reset();
	m_nPriorityValue = TMAP_LOAD_OP_PRIORITY;
}

BL_AsyncLoadMapOperation::~BL_AsyncLoadMapOperation(void)
{
}

void BL_AsyncLoadMapOperation::Init( const char *cpWorldMapName )
{
	m_strResPath	= cpWorldMapName;

#ifdef USE_MULTI_LOAD_THREADS
	m_bIsResReady = UT_FileManagerBase::GetSingletonPtr()->AccessFile(m_strResPath.c_str());
#endif
}

void BL_AsyncLoadMapOperation::Reset(void)
{
	m_eOpType = OT_WORLD;
	m_eState = LS_END;
}

bool BL_AsyncLoadMapOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	if (!m_bIsResReady)
	{
		bool b = UT_FileManagerBase::GetSingletonPtr()->PrefetchFile(m_strResPath.c_str(), pCtrl);
		if (!b)
		{
			m_eState = LS_FAIL;
			return false;
		}
	}

#endif

	T_ASSERT( m_eState == LS_START_LOADING );
	m_eState = LS_LOADING;

	return true;
}

bool BL_AsyncLoadMapOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );
	m_eState = LS_LOADED;

	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if (pkGamePlayer)
	{
		pkGamePlayer->SetMapLoaded();
		return true;
	}
	return false;
}

bool BL_AsyncLoadMapOperation::PreUnLoad(void)
{
	m_eState = LS_UNLOADING;

	return true;
}

bool BL_AsyncLoadMapOperation::UnLoad(void)
{
	m_eState = LS_UNLOADED;

	return true;
}

//==============================================================================
// class BL_AsyncReloadConfigOperation
//==============================================================================

BL_AsyncReloadConfigOperation::BL_AsyncReloadConfigOperation(void)
{
	Reset();
	m_nPriorityValue = CONFIG_RELOAD_OP_PRIORITY;
	m_uitimestamp = 0;
}

BL_AsyncReloadConfigOperation::~BL_AsyncReloadConfigOperation(void)
{
}

void BL_AsyncReloadConfigOperation::Init( TStringVector * lst, uint32_t timestamp )
{
	m_strResPath.clear();
	m_bIsResReady	= false;

	m_kCfgFileList.clear();
	if (lst != NULL)
	{
		m_kCfgFileList.resize(lst->size());
		std::copy(lst->begin(), lst->end(), m_kCfgFileList.begin());
	}
	m_uitimestamp = timestamp;
}

void BL_AsyncReloadConfigOperation::Reset(void)
{
	m_eOpType = OT_RELOAD_CONFIG;
	m_eState = LS_END;
}

bool BL_AsyncReloadConfigOperation::Load(IThreadControl *pCtrl)
{
#ifdef USE_MULTI_LOAD_THREADS
	IDownloader* downloader = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
	if (downloader != NULL)
	{
		IDownloaderInitArg::ExtConfig  extCfg = {"reload_catalog.txt",	false,	true,	true , true, true};
		bool b = downloader->ReloadExtCatalog(&extCfg);
		//if (!b)	//< ����bֵ��reload config��ʹ����ʧ�ܣ�Ҳ����true������ȥ��ȡ�ļ���ֻ�Ƕ����Ŀ������ϵġ��ͻ�����ʾ�������
		//{
		//	m_eState = LS_FAIL;
		//	return false;
		//}
	}
#endif

	T_ASSERT( m_eState == LS_START_LOADING );
	m_eState = LS_LOADING;

	return true;
}

bool BL_AsyncReloadConfigOperation::PostLoad(void)
{
	T_ASSERT( m_eState == LS_LOADING );
	m_eState = LS_LOADED;

	if (m_kCfgFileList.empty())
		ClientConfigReloadManager::GetSingletonPtr()->ConfigReloadAll(m_uitimestamp);
	else
		ClientConfigReloadManager::GetSingletonPtr()->ConfigReload(m_kCfgFileList,m_uitimestamp);

	return true;
}

bool BL_AsyncReloadConfigOperation::PreUnLoad(void)
{
	m_eState = LS_UNLOADING;

	return true;
}

bool BL_AsyncReloadConfigOperation::UnLoad(void)
{
	m_eState = LS_UNLOADED;

	return true;
}

//==============================================================================
// class BL_AsyncDuowanReportOperation
//==============================================================================

BL_AsyncDuowanReportOperation::BL_AsyncDuowanReportOperation()
{
	Reset();
	m_nPriorityValue = DUOWAN_REPORT_OP_PRIORITY;
}

BL_AsyncDuowanReportOperation::~BL_AsyncDuowanReportOperation()
{

}

void BL_AsyncDuowanReportOperation::Init_1( int sceneId, const char * sid, const char * rolename )
{
	m_opType = 1;
	m_bIsResReady = true;

	o1.m_sceneId		= sceneId;
	o1.m_sid			= sid;
	o1.m_role_name		= rolename;
}

void BL_AsyncDuowanReportOperation::Init_2( const char * chatContent, const char * toUserName, const char * fromUserName, int bMultipy, int sceneId )
{
	m_opType = 2;
	m_bIsResReady = true;

	o2.m_chatmsg		= chatContent;
	o2.m_toUserName		= toUserName;
	o2.m_fromUserName	= fromUserName;
	o2.m_bMultipy		= bMultipy;
	o2.m_sceneId		= sceneId;
}

void BL_AsyncDuowanReportOperation::Init_3( int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power )
{
	m_opType = 3;
	m_bIsResReady = true;

	o3.m_role_level		= newLevel;
	o3.m_uuid			= uuid;
	o3.m_role_name		= role_name;
	o3.m_role_gender	= role_gender;
	o3.m_role_profession= role_profession;
	o3.m_role_power		= role_power;
}

void BL_AsyncDuowanReportOperation::Reset( void )
{
	m_eOpType = OT_DUOWAN_REPORT;
	m_eState = LS_END;
	m_opType = 0;
}

bool BL_AsyncDuowanReportOperation::Load( IThreadControl *pCtrl )
{
	T_ASSERT( m_eState == LS_START_LOADING );
	m_eState = LS_LOADING;

	switch(m_opType)
	{
	case 1:
		report2duowan_getreportable(o1.m_sceneId, o1.m_sid, o1.m_role_name);
		break;
	case 2:
		report2duowan_chatmsg(o2.m_chatmsg, o2.m_toUserName, o2.m_fromUserName, o2.m_bMultipy, o2.m_sceneId);
		break;
	case 3:
		report2duowan_levelup(o3.m_role_level, o3.m_uuid, o3.m_role_name.c_str(), o3.m_role_gender, o3.m_role_profession, o3.m_role_power);
		break;
	}

	return true;
}

bool BL_AsyncDuowanReportOperation::PostLoad( void )
{
	T_ASSERT( m_eState == LS_LOADING );
	m_eState = LS_LOADED;

	return true;
}

bool BL_AsyncDuowanReportOperation::PreUnLoad( void )
{
	m_eState = LS_UNLOADING;

	return true;
}

bool BL_AsyncDuowanReportOperation::UnLoad( void )
{
	m_eState = LS_UNLOADED;

	return true;
}

//==============================================================================
// class BL_AsyncCurlOperation
//==============================================================================

BL_AsyncCurlOperation::BL_AsyncCurlOperation()
{
	Reset();
	m_nPriorityValue = HTTP_UPLOAD_OP_PRIORITY;
}

BL_AsyncCurlOperation::~BL_AsyncCurlOperation()
{

}

void BL_AsyncCurlOperation::Init( bool bUpload, const char *cpUrl, const char *cpAuth, const char *cpFilename, int64_t nTransId, unsigned nTimeOutMs )
{
	this->m_bIsResReady = false;
	this->m_bUpload = bUpload;
	this->m_strUrl = cpUrl;
	this->m_strAuth = cpAuth;
	this->m_strFilename = cpFilename;
	this->m_nTransId = nTransId;
	this->m_nTimeoutMs = nTimeOutMs;
	this->m_nTimeCostMs = 0;
	this->m_eTaskCode = 0;
	this->m_eErrCode = 0;
}

void BL_AsyncCurlOperation::Reset( void )
{
	m_eOpType = OT_CURL;
	m_eState = LS_END;
}

bool BL_AsyncCurlOperation::Load( IThreadControl *pCtrl )
{
	T_ASSERT( m_eState == LS_START_LOADING );
	m_eState = LS_LOADING;

	do
	{
		DWORD nStartTime = ::timeGetTime();

		//< ����׼��
		JZCurlTaskMetrics	metrics;
		JZCurlTaskState		state;
		JZCurlTaskParam		param;
		param.d_pUrl		= m_strUrl.c_str();
		param.d_pAuth		= m_strAuth.c_str();
		param.d_pFilename	= m_strFilename.c_str();
		param.d_bPackUnpack = true;

		//< ִ��
		if (m_bUpload)
			ICurlHttp::UploadFromFile(&param, &state, &metrics);
		else
			ICurlHttp::DownloadToFile(&param, &state, &metrics);

		//< �ռ�������
		this->m_eTaskCode = state.d_taskCode;
		switch(state.d_taskCode)
		{
		case ETPC_Perform_Error:
			this->m_eErrCode = state.d_httpOpCode;
			break;
		case ETPC_Lzma_Error:
			this->m_eErrCode = state.d_lzmaOpCode;
			break;
		case ETPC_IO_Error:
			this->m_eErrCode = state.d_fileOpCode;
			break;
		}

		// ��ʱ
		DWORD nEndTime = ::timeGetTime();
		this->m_nTimeCostMs = nEndTime - nStartTime;

	}while(0);

	return true;
}

bool BL_AsyncCurlOperation::PostLoad( void )
{
	T_ASSERT( m_eState == LS_LOADING );
	m_eState = LS_LOADED;

	MG_MessageBase msg = CreartMsg_CALLBACK_REPORT_OPERATION_TASK(m_nTransId, m_eTaskCode, m_eErrCode, m_nTimeCostMs);
	MG_MessageCenter::GetSingleton().SendMessage( msg, true);	//����true


	return true;
}

bool BL_AsyncCurlOperation::PreUnLoad( void )
{
	m_eState = LS_UNLOADING;

	return true;
}

bool BL_AsyncCurlOperation::UnLoad( void )
{
	m_eState = LS_UNLOADED;

	return true;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool SortedOpeartionQueue::empty() const
{
	OperationQueueMap::const_iterator iter;
	TRAV( c , iter )
	{
		if ( iter->second->empty() == false )
		{
			return false;
		}
	}
	return true;
}

size_t SortedOpeartionQueue::size() const
{
	size_t n = 0;
	OperationQueueMap::const_iterator iter;
	TRAV( c , iter )
	{
		n+= iter->second->size();
	}
	return n;
}

void SortedOpeartionQueue::push( BL_AsyncOperation* p )
{
	T_ASSERT_CRITICAL_S(p != NULL);
	OperationQueueMap::iterator iter = c.find( p->GetPriority() );
	T_ASSERT_CRITICAL_S( iter != c.end() )

	iter->second->push_back( p );
}

BL_AsyncOperation* SortedOpeartionQueue::pop_top()
{
	OperationQueueMap::iterator iter;
	TRAV( c , iter )
	{
		if ( !iter->second->empty() )
		{
			BL_AsyncOperation *v = *(iter->second->begin());	//front()
			iter->second->pop_front();
			return v;
		}
	}

	return NULL;
}

BL_AsyncOperation* SortedOpeartionQueue::pop_top_ready_load()
{
	OperationQueueMap::iterator iter;
	OperationQueue::iterator j;
	TRAV( c , iter )
	{
		TRAV( (*iter->second) , j )
		{
			BL_AsyncOperation *p = *j;
			if (p->IsLoadOp() && p->ready2load())	//�����غõ�load���������ȵ������д�����Ӱ�쿪������Ϊ����AccessFile
			{
				iter->second->erase(j);
				return p;
			}
		}
	}

	return NULL;
}

BL_AsyncOperation* SortedOpeartionQueue::pop_top(POP_STRATEGY s)
{
	switch(s)
	{
	case POPS_TOP:					return this->pop_top();
		break;
	case POPS_TOP_READY_LOAD_OP:	return this->pop_top_ready_load();
		break;
	}
	return	this->pop_top();
}

SortedOpeartionQueue::SortedOpeartionQueue()
{
	OperationQueue* pkNewQueue = T_NEW_E( OperationQueue , Memory::MEMCATEGORY_GENERAL );
	c.insert( std::make_pair( BPV_1 , pkNewQueue ) );

	pkNewQueue = T_NEW_E( OperationQueue , Memory::MEMCATEGORY_GENERAL );
	c.insert( std::make_pair( BPV_2 , pkNewQueue ) );

	pkNewQueue = T_NEW_E( OperationQueue , Memory::MEMCATEGORY_GENERAL );
	c.insert( std::make_pair( BPV_3 , pkNewQueue ) );

	pkNewQueue = T_NEW_E( OperationQueue , Memory::MEMCATEGORY_GENERAL );
	c.insert( std::make_pair( BPV_4 , pkNewQueue ) );

	pkNewQueue = T_NEW_E( OperationQueue , Memory::MEMCATEGORY_GENERAL );
	c.insert( std::make_pair( BPV_5 , pkNewQueue ) );

	pkNewQueue = T_NEW_E( OperationQueue , Memory::MEMCATEGORY_GENERAL );
	c.insert( std::make_pair( BPV_6 , pkNewQueue ) );

	pkNewQueue = T_NEW_E( OperationQueue , Memory::MEMCATEGORY_GENERAL );
	c.insert( std::make_pair( BPV_7 , pkNewQueue ) );
}

SortedOpeartionQueue::~SortedOpeartionQueue()
{
	OperationQueueMap::iterator iter;
	TRAV( c , iter )
	{
		T_DELETE_E( iter->second , OperationQueue , Memory::MEMCATEGORY_GENERAL );
	}
}

unsigned int SortedOpeartionQueue::count_if_priority_is_ge( int nPriority )
{
	if ( empty() )
	{
		return 0;
	}
	OperationQueueMap::iterator iter;
	size_t diff = 0;
	TRAV( c , iter )
	{
		if( iter->first >= nPriority )
			diff += iter->second->size();
	}
	return (unsigned int)diff;
}

bool SortedOpeartionQueue::remove( BL_AsyncOperation* p )
{
	T_ASSERT_CRITICAL_S( p != NULL );

	T_PROFILE("remove working queue");
	OperationQueueMap::iterator iter = c.find( p->GetPriority() );
	if( iter == c.end() )
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"op with priority value %d not existed!" , p->GetPriority() );
		T_ASSERT_CRITICAL_S(false);
	}
	size_t b = iter->second->size();
	iter->second->remove(p);
	if ( b != iter->second->size() )
	{
		return true;
	}
	return false;
}

void SortedOpeartionQueue::clear()
{
	OperationQueueMap::iterator iter;
	TRAV( c , iter )
	{
		iter->second->clear();
	}
}
