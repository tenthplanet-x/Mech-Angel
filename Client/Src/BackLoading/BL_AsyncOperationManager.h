#pragma once
//�첽���ز�������Ĺ�����
#include "BL_AsyncOperation.h"

#include "SE_SceneWorldMap.h"
#include "UT_PoolSet.h"

#define DECLARE_OP_POOL( className , opName )\
class className : public Utility::UT_PoolSet<opName>\
{\
protected:\
	opName* Allocate()\
	{\
		return T_NEW_D opName;\
	}\
	void Deallocate( opName* pkOp )\
	{\
		if ( pkOp )\
		{\
			T_DELETE_D pkOp;\
		}\
	}\
};\

class SE_SceneVisibleChunkSelector;
class BL_AsyncOperationWorkerThread;

namespace Data
{
	class GD_ModelBase;
}

class BL_AsyncOperationManager : public MG_MessageHandlerInterface,
								 public Memory::MM_BaseObject,
								 public Utility::UT_Singleton<BL_AsyncOperationManager>
{
	friend class BL_AsyncOperationWorkerThread;
public:
	
	BL_AsyncOperationManager( SE_SceneWorldMap* pkProcessMap );

	void _initOpPool();
	virtual ~BL_AsyncOperationManager();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	void		 Update(float fTime);

	void	   	 Init();
	void         UnInit();

	//��̨�Ƿ��л�Ծ�ļ�������
    bool         IsLoadingActive() { T_ASSERT( Utility::GfxIsMainThread() ); return m_bLoading; }
    bool         IsLoadingThreadAlive() { return (m_pkLoaderThreadProc != NULL); }
	
	//void		 ProcessLoadedOperation();
	//void		 ProcessUnLoadedOperation();

	//void         EnqueueLoadOperation();
	//void         EnqueueUnLoadOperation();

	bool         ExecuteNextOperation(SortedOpeartionQueue::POP_STRATEGY fetchNext = SortedOpeartionQueue::POPS_TOP, BL_AsyncOperationWorkerThread *OpThread = NULL);
	//������Ҫ���е��μ��ص�ʱ��Radius���Դ���0
	void         StartLoading( long lRadius );
	void         StopLoading( bool bQuitClient = false );	//bQuitClient��ʾ�����˳��ͻ��ˣ�������Toggle Back Loading
	void         SuspendLoading( bool bEnable );

	void         NotifyInvalidModel( Data::GD_ModelBase* pkModel );
	void         AddModelOperationMap( Data::GD_ModelBase* pkModel , BL_AsyncOperation* pkOp );

	//���õ�ǰ���ؽ����������ֲ���Ϊ׼
	int          GetSceneLoadingTaskCount(){ return m_nSceneLoadingTask; }
	int          GetModelLoadingTaskCount(){ return m_nModelLoadingTask; }
	
	//int          GetWorkingQueueID( BL_AsyncOperation::OperationType eType );

	void         EnableTerrainLoading( bool bEnable ){ m_bTerrainLoading = bEnable; }

    unsigned int GetCurrentPlayerModelCreationCapacity(void)
    {
        m_kWorkingQueueLock.Lock();
        unsigned int nRet = (m_kWorkingQueue.count_if_priority_is_ge(PLAYER_MODEL_LOAD_OP_PRIORITY) > 0) ? 0 : 1;
        m_kWorkingQueueLock.Unlock();
        return nRet;
    }

	void                    _addToWorkingSet(  BL_AsyncOperation* pkTask , bool bLoading );	//true:load, false:unload
protected:

	
	void                    _cleanUpUpdateQueue( AsyncOperationQueue& kToRemove );
	void                    _handleChunkLoading();
	void                    _checkOpPoolSize();
	void					_computeChunkLoadingUnLoadingTask( SE_ChunkIndexPool& rkVisibleChunkIndexMap , SE_ChunkIndexPool& rkNewLoadingWorkSet , SE_ChunkIndexPool& rkNewUnLoadingWorkSet );
	void					_addNewChunkTerrainOperation( SE_ChunkIndexPool &kSet );
	void					_updateMetrics();

	void                    _processInMainThread( BL_AsyncOperation* pkNewOp , bool bLoading );
	void                    _preprocessTask( BL_AsyncOperation* pkTask, bool bLoading );
	bool                    _processWorkSet( BL_AsyncOperation* pkOp);
	bool                    _processUpdateSet( AsyncOperationQueue& rkRemove , int nCount , BL_AsyncOperation::OperationType eType );
	void					_processInvalidModels();

	void                    _onTerrainLoaded( BL_AsyncOperation* pkOp );
	void                    _onTerrainUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onActorLoaded( BL_AsyncOperation* pkOp );
	void                    _onActorUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onChangeWeaponLoaded( BL_AsyncOperation* pkOp );
	void                    _onChangeWeaponUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onGfxLoaded( BL_AsyncOperation* pkOp );
	void                    _onGfxUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onUIImgUnLoaded( BL_AsyncOperation* pkOp );
	void					_onUIImgLoaded( BL_AsyncOperation* pkOp );
	void                    _onPathMeshUnLoaded( BL_AsyncOperation* pkOp );
	void					_onPathMeshLoaded( BL_AsyncOperation* pkOp );
	void                    _onVideoLoaded( BL_AsyncOperation* pkOp );
	void                    _onVideoUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onAudioLoaded( BL_AsyncOperation* pkOp );
	void                    _onAudioUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onTMapLoaded( BL_AsyncOperation* pkOp );
	void                    _onTMapUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onReloadConfigLoaded( BL_AsyncOperation* pkOp );
	void                    _onReloadConfigUnLoaded( BL_AsyncOperation* pkOp );
	void                    _onDuowanReportLoaded( BL_AsyncOperation* pkOp );
	void                    _onDuowanReportUnLoaded( BL_AsyncOperation* pkOp );

	void                    _onCurlOpLoaded( BL_AsyncOperation* pkOp );
	
	bool					_onOpLoaded( BL_AsyncOperation* pkOp );
	void                    _onOpEnd( BL_AsyncOperation* pkOp );
	void                    _onOpFailed( BL_AsyncOperation* pkOp );



	DECLARE_OP_POOL( TerrainOpPoolSet ,      BL_AsyncTerrainOperation )
	DECLARE_OP_POOL( ActorOpPoolSet ,        BL_AsyncActorOperation )
	DECLARE_OP_POOL( ChangeWeaponOpPoolSet , BL_AsyncChangeAvatarOperation )
	DECLARE_OP_POOL( LoadGfxPoolSet ,        BL_AsyncFxLoadOperation )
	DECLARE_OP_POOL( LoadUIImgPoolSet,		 BL_AsyncUIImgLoadOperation)
	DECLARE_OP_POOL( LoadPathMeshPoolSet,	 BL_AsyncPathMeshOperation)
	DECLARE_OP_POOL( PlayVideoOpPoolSet,	 BL_AsyncPlayVideoOperation)
	DECLARE_OP_POOL( PlayAudioOpPoolSet,	 BL_AsyncPlayAudioOperation)
	DECLARE_OP_POOL( LoadMapOpPoolSet,		 BL_AsyncLoadMapOperation)
	DECLARE_OP_POOL( ReloadConfigOpPoolSet,	 BL_AsyncReloadConfigOperation)
	DECLARE_OP_POOL( DuowanReportOpPoolSet,	 BL_AsyncDuowanReportOperation)

	DECLARE_OP_POOL( CurlOpPoolSet,			 BL_AsyncCurlOperation)

	typedef void ( BL_AsyncOperationManager::*LoadMemFunc )( BL_AsyncOperation* );
	struct  LoadOperationTable
	{
		LoadOperationTable( LoadMemFunc func , LoadMemFunc func2 )
			:pfnLoadedFunc( func ),pfnUnLoadedFunc(func2){}
		LoadMemFunc       pfnLoadedFunc;
		LoadMemFunc       pfnUnLoadedFunc;
	};
	typedef mem::map<BL_AsyncOperation::OperationType,LoadOperationTable*> LoadTable;
	typedef LoadTable::iterator LoadTableIterator;

	typedef mem::vector<Data::GD_ModelBase*>			ModelSet;
	ModelSet                                            m_kInvalidModels;


	LoadTable                                           m_kLoadTable;			//����Դ���Ͷ�Ӧ��Load��Unload��������_onTerrainLoaded/_onTerrainUnLoaded�Ⱥ�����
	bool                                                m_bTerrainLoading;		//GameStage��������μ���

	BL_AsyncOperationWorkerThread*						m_pkLoaderThreadProc;	//�����߳�
	BL_AsyncOperationWorkerThread*						m_pkLoaderThreadProcAux;//���������̣߳�΢����������ȴ����Ѿ����غõ���Դ����ֹm_pkLoaderThreadProc�������������ʱ�򣬶����к�����׼������Դ����ȴ�
	Utility::UT_ThreadEvent								m_kLoaderEvent;			//�߳���
	//For Terrain Backloading	
	SE_ChunkIndexPool									m_kChunkLoadingIndexMap;
	SE_ChunkIndexPool									m_kChunkUnLoadingIndexMap;
	SE_ChunkIndexPool									m_kChunkLoadedIndexMap;
	SE_SceneVisibleChunkSelector*						m_pkVisibleChunkSelector;
	NiPoint2											m_kLastSelectEyeLocation;
	NiPoint2                                            m_kLastCameraRot;
	SE_SceneWorldMap*									m_pkProcessMap;

	AsyncOperationHeap                                  m_kWorkingQueue;		//�����Operation List
	AsyncOperationQueue                                 m_kUpdateQueue;			//������ɱ������߳�������������
	NiCriticalSection                                   m_kWorkingQueueLock;	//������
	
	NiCriticalSection									m_kPoolLock;
	TerrainOpPoolSet                                   	m_kTerrainOpPools;
	ActorOpPoolSet										m_kActorOpPools;
	ChangeWeaponOpPoolSet                               m_kChangeWeaponPools;
	//LoadUIEffectPoolSet                                 m_kUIEffectPools;
	LoadGfxPoolSet                                      m_kGfxPools;
	LoadUIImgPoolSet									m_kUIImgPools;
	LoadPathMeshPoolSet                                 m_kPathMeshPools;
	PlayVideoOpPoolSet                                  m_kPlayVideoPools;
	PlayAudioOpPoolSet                                  m_kPlayAudioPools;
	LoadMapOpPoolSet                                    m_kLoadMapPools;
	ReloadConfigOpPoolSet                               m_kReloadConfigPools;
	DuowanReportOpPoolSet                               m_kDuowanReportPools;
	CurlOpPoolSet										m_kCurlOpPools;

	bool                                                m_bSingleCore;
	unsigned int                                        m_uiWorkPerFrame;

	bool                                                m_bLoading;
	bool												m_bLastLoading;//��һ֡�Ƿ��ڼ�����
	int          										m_nSceneLoadingTask;
	int                                                 m_nModelLoadingTask;  
	bool                                                m_bWaitForExit;
};