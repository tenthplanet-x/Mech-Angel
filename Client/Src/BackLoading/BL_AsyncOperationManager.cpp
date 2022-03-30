#include "ClientPCH.h"
#include "BL_AsyncOperationManager.h"
#include "GD_ModelBase.h"
#include "MG_MessageBase.h"
#include "PF_ClientPathManager.h"
#include "SE_SceneVisibleChunkSelector.h"
#include "UI_UIManager.h"
#include "CA_CameraManager.h"
#include <fstream>
#include "UT_StatisticsManager.h"
#include "UT_FileManagerBase.h"
#include <iomanip>

using namespace std;
using namespace Data;
using namespace Utility;

class BL_AsyncOperationWorkerThread : public Utility::UT_Thread
{
public:
	BL_AsyncOperationWorkerThread( BL_AsyncOperationManager* pkLoader );
	virtual ~BL_AsyncOperationWorkerThread();

	virtual void Run();
	void		 Stop();
	void         Pause( bool bEnable ){ m_bPause = bEnable; }
	void         SetLoopFlag( bool bDone ){ m_bDone = bDone; }

private:
	BL_AsyncOperationManager* m_pkLoader;
	volatile bool             m_bDone;
	volatile bool             m_bPause;
public:
	SortedOpeartionQueue::POP_STRATEGY	m_eFetchStrategy;
};

BL_AsyncOperationWorkerThread::BL_AsyncOperationWorkerThread( BL_AsyncOperationManager* pkLoader ) 
: m_pkLoader( pkLoader )
, m_bDone(false)
, m_bPause(false)
, m_eFetchStrategy(SortedOpeartionQueue::POPS_TOP)
{
}

BL_AsyncOperationWorkerThread::~BL_AsyncOperationWorkerThread()
{
}

void BL_AsyncOperationWorkerThread::Run()
{
	/*
	当Loader中的任务队列都执行完之后，后台线程就Wait在核心事件对象上
	直到主线程有新的任务加入唤醒后台线程
	*/
	GfxWriteLog( LOG_TAG_BACKLOADING , LOG_SWITCH_BACKLOADING ,  "%s Start. fetchMethod=%d" , __FUNCTION__, m_eFetchStrategy );
	SetThreadName( "Async Terrain Loading Thread" );
	SetPriority( Priority_Normal );

	//外层循环只有在线程要退出的时候才会退出
	while( !m_bDone )
	{
		//通过核心对象和主线程进行同步
		//在主线程有新的加载任务时就会
		//激发LoaderEvent，使得我们可以
		//进入内层循环
		m_pkLoader->m_kLoaderEvent.Wait();
		m_pkLoader->m_kLoaderEvent.Reset();
		bool bLoadingEmpty = false;
		//内层循环不停的查询是否还有加载任务
		while( !bLoadingEmpty  )
		{		
			if ( !m_bPause )
			{
				bLoadingEmpty = m_pkLoader->ExecuteNextOperation(m_eFetchStrategy, this);
				if (this->m_interruptCtrl.canceled())	//中断了应该退出
					break;
			}
		}

		if (!m_bDone)
			::Sleep(10);
	}
}

void BL_AsyncOperationWorkerThread::Stop()
{
	//把外层循环退出标记置为True
	m_bDone = true;
	//激发核心对象进入内层循环处理没有完成的加载任务
	m_pkLoader->m_kLoaderEvent.SetSignaled();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template<class T>
static void DefaultRemovePoolable( BL_AsyncOperation* pkOp , UT_PoolSet<T>* pkPool )
{
	pkOp->Reset();
	pkPool->RemovePoolable( (T*)pkOp );
}

BL_AsyncOperationManager::BL_AsyncOperationManager( SE_SceneWorldMap* pkProcessMap )
: m_kLastSelectEyeLocation( NiPoint2::ZERO )
, m_kLastCameraRot(NiPoint2::ZERO)
, m_pkProcessMap(pkProcessMap)
, m_pkLoaderThreadProc(NULL)
, m_pkLoaderThreadProcAux(NULL)
, m_pkVisibleChunkSelector(NULL)
, m_nSceneLoadingTask(0)
, m_nModelLoadingTask(0)
, m_bLastLoading(false)
, m_bTerrainLoading(false)
{
	//设置一个对象池的大小
	_initOpPool();

	m_bSingleCore = false;
	m_bLoading = false;
	m_uiWorkPerFrame = 0;
	m_bWaitForExit = false;
}

BL_AsyncOperationManager::~BL_AsyncOperationManager()
{
}

#define ADD_LOAD_TABLE_ENTRY( type , funcName , funcName2 )\
	pkNewOp = T_NEW_E( LoadOperationTable( &BL_AsyncOperationManager::funcName,&BL_AsyncOperationManager::funcName2 ) , Memory::MEMCATEGORY_GENERAL );\
	m_kLoadTable.insert( std::make_pair( BL_AsyncOperation::type , pkNewOp ) );\

void BL_AsyncOperationManager::Init()
{
	m_uiWorkPerFrame = g_settings.BL_OP_PER_FRAME;
	m_pkVisibleChunkSelector = T_NEW_D SE_SceneVisibleChunkSelector( m_pkProcessMap , 4200 );

	m_bSingleCore = ( NiSystemDesc::GetSystemDesc().GetLogicalProcessorCount() == 1 ? true : false );

	LoadOperationTable* pkNewOp = NULL;
	ADD_LOAD_TABLE_ENTRY( OT_TERRAIN , _onTerrainLoaded , _onTerrainUnLoaded );
	ADD_LOAD_TABLE_ENTRY( OT_ACTOR , _onActorLoaded, _onActorUnLoaded );
	ADD_LOAD_TABLE_ENTRY( OT_ACTOR_ITEM , _onChangeWeaponLoaded , _onChangeWeaponUnLoaded );
	//ADD_LOAD_TABLE_ENTRY( OT_UIEFFECT , _onUIEffectLoaded , _onUIEffectUnLoaded );
	ADD_LOAD_TABLE_ENTRY( OT_GFX , _onGfxLoaded , _onGfxUnLoaded );
	ADD_LOAD_TABLE_ENTRY( OT_UIIMG,_onUIImgLoaded , _onUIImgUnLoaded);
	ADD_LOAD_TABLE_ENTRY( OT_PATH,_onPathMeshLoaded,_onPathMeshUnLoaded);
	ADD_LOAD_TABLE_ENTRY( OT_VIDEO,_onVideoLoaded,_onVideoUnLoaded);
	ADD_LOAD_TABLE_ENTRY( OT_WORLD,_onTMapLoaded,_onTMapUnLoaded);
	ADD_LOAD_TABLE_ENTRY( OT_RELOAD_CONFIG,_onReloadConfigLoaded,_onReloadConfigUnLoaded);
	ADD_LOAD_TABLE_ENTRY( OT_DUOWAN_REPORT,_onDuowanReportLoaded,_onDuowanReportUnLoaded);
	ADD_LOAD_TABLE_ENTRY( OT_CURL,_onCurlOpLoaded,_onCurlOpLoaded);
}

void BL_AsyncOperationManager::UnInit()
{
	m_kActorOpPools.DeletePool();
	m_kTerrainOpPools.DeletePool();
	m_kChangeWeaponPools.DeletePool();
	//m_kUIEffectPools.DeletePool();
	m_kGfxPools.DeletePool();
	m_kUIImgPools.DeletePool();
	m_kPathMeshPools.DeletePool();
	m_kPlayVideoPools.DeletePool();
	m_kPlayAudioPools.DeletePool();
	m_kLoadMapPools.DeletePool();
	m_kReloadConfigPools.DeletePool();
	m_kDuowanReportPools.DeletePool();
	m_kCurlOpPools.DeletePool();
	T_DELETE_D m_pkVisibleChunkSelector;
	//确认这个时候后台线程已经干净的析构了
	T_ASSERT( m_pkLoaderThreadProc == NULL );
	T_ASSERT( m_pkLoaderThreadProcAux == NULL );
	LoadTableIterator iter;
	TRAV( m_kLoadTable , iter )
	{
		T_DELETE_E( iter->second , LoadOperationTable , Memory::MEMCATEGORY_GENERAL );
	}
	m_kLoadTable.clear();
	m_bTerrainLoading = false;
}

bool BL_AsyncOperationManager::RecvMessage( MG_MessageBase& rkMessage )
{	
	bool bMsgHandle = false;
	switch (rkMessage.m_eMsgType)
	{
	case MGT_TOGGLE_BACKLOADING:
		StopLoading();
		if ( rkMessage.m_lParam1 )
		{
			StartLoading( rkMessage.m_lParam2 );
		}
		break;
	case MGT_TIMER_TICK:
		{
			T_PROFILE_EX("AsyncOperationManager::Update" , 10.0f );
			Update( rkMessage.m_fParam1 );
		}
		break;
	case MGT_CAMERA_RESET:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::CameraReset" , 10.0f );
			bMsgHandle = true;
			_handleChunkLoading(  );
		}
		break;
	case MGT_CAMERA_ROTATE:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::CameraRot" , 10.0f );
			bMsgHandle = true;
			NiPoint2 kRotAngle = NiPoint2(rkMessage.m_fParam1,rkMessage.m_fParam2);
			float fMoveDistance = (kRotAngle - m_kLastCameraRot).Length();
			if ( fMoveDistance > 10 && m_bTerrainLoading  )
			{
				_handleChunkLoading(  );
				m_kLastCameraRot = kRotAngle;
			}
		}
		break;
	case MGT_LOGIC_PLAYER_MOVE:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::PlayerMove" , 10.0f );
			bMsgHandle = true;
			NiPoint2 kCurrentPlayerLocation = NiPoint2(rkMessage.m_fParam1,rkMessage.m_fParam2);
			float fMoveDistance = (kCurrentPlayerLocation - m_kLastSelectEyeLocation).Length();
			float fWorldChunkWidth = GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;
			if(fWorldChunkWidth > 0 && fMoveDistance > fWorldChunkWidth * 0.25f)
			{
				NiPoint3 kCameraLocation;
				kCameraLocation.x = rkMessage.m_fParam1;
				kCameraLocation.y = rkMessage.m_fParam2;
				kCameraLocation.z = rkMessage.m_fParam3;
				m_kLastSelectEyeLocation = NiPoint2(rkMessage.m_fParam1,rkMessage.m_fParam2);
				_handleChunkLoading(  );
			}
		}
		break;
	case MGT_ADD_MODEL_LOADING_TASK:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::AddModel" , 10.0f );
			bMsgHandle = true;
			BL_AsyncActorOperation* pkNewOp = m_kActorOpPools.GetPoolable();
			pkNewOp->Init((GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE)rkMessage.m_lParam1, 
				(GD_ModelBase*)rkMessage.m_pbyParam2, (char*)rkMessage.m_pbyParam3, (int)rkMessage.m_lParam4, (unsigned int)rkMessage.m_lParam5);
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
			
		}
		break;
	case MGT_ADD_MODEL_UNLOADING_TASK:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::UnloadModel" , 10.0f );
			bMsgHandle = true;
			BL_AsyncActorOperation* pkNewOp = m_kActorOpPools.GetPoolable();
			pkNewOp->Init(GD_ClientAsyncModelManager::AMT_INVALID, (GD_ModelBase*)rkMessage.m_pbyParam1, "", 0, 0);
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , false );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread(pkNewOp , false );
			}
		}
		break;
	case MGT_ADD_ACTOR_CHANGE_AVATAR_TASK:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::MGT_ADD_ACTOR_CHANGE_AVATAR_TASK" , 10.0f );
			bMsgHandle = true;
			BL_AsyncChangeAvatarOperation* pkNewOp = m_kChangeWeaponPools.GetPoolable();
			pkNewOp->Init((GD_ClientActorModel*)rkMessage.m_pbyParam1);
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;
	case MGT_ADD_GFX_TASK:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::AddGfx" , 10.0f );
			bMsgHandle = true;
			BL_AsyncFxLoadOperation* pkNewOp = m_kGfxPools.GetPoolable();
			pkNewOp->Init((FX_ClientFxManager::FxObject*)rkMessage.m_pbyParam1);
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;
	case MGT_ADD_GFX_UNLOAD_TASK:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::UnLoadGfx" , 10.0f );
			bMsgHandle = true;
			BL_AsyncFxLoadOperation* pkNewOp = m_kGfxPools.GetPoolable();
			pkNewOp->InitUnLoad((FX_ClientFxManager::FxObject*)rkMessage.m_pbyParam1);
			
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , false );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , false );
			}
		}
		break;
	case MGT_ADD_UI_LOAD_UIIMG :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::UIload" , 10.0f );
			bMsgHandle = true;

			StringType sName = static_cast<char *>((void*)rkMessage.m_pbyParam2);
			if(sName == "")
			{
				return bMsgHandle;
			}
			BL_AsyncUIImgLoadOperation* pkNewOp = m_kUIImgPools.GetPoolable();
			pkNewOp->Init( rkMessage.m_pbyParam2 );

			if ( m_pkLoaderThreadProc != NULL  )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;
	case MGT_ADD_UI_UNLOAD_UIIMG :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::UIUnLoad" , 10.0f );
			bMsgHandle = true;
			BL_AsyncUIImgLoadOperation* pkNewOp = m_kUIImgPools.GetPoolable();
			pkNewOp->InitUnLoad( rkMessage.m_pbyParam2, rkMessage.m_lParam1);
			
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , false );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , false );
			}
		}
		break;
	case MGT_ADD_PATH_LOAD_TASK :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::PathLoad" , 10.0f );
			bMsgHandle = true;
			BL_AsyncPathMeshOperation* pkNewOp = m_kPathMeshPools.GetPoolable();
			pkNewOp->Init( rkMessage.m_lParam1);
			
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;
	case MGT_ADD_PATH_UNLOAD_TASK :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::PathUnLoad" , 10.0f );
			bMsgHandle = true;
			BL_AsyncPathMeshOperation* pkNewOp = m_kPathMeshPools.GetPoolable();
			pkNewOp->Init( rkMessage.m_lParam1);
			
			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , false );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , false );
			}
		}
		break;
	case MGT_ADD_PLAY_VIDEO_LOAD_TASK :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::PlayVideo" , 10.0f );
			bMsgHandle = true;
			BL_AsyncPlayVideoOperation* pkNewOp = m_kPlayVideoPools.GetPoolable();
			pkNewOp->Init( rkMessage.m_lParam1, (const char *)rkMessage.m_pbyParam2, (const char *)rkMessage.m_pbyParam3, rkMessage.m_uiParam4);

			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;
	case MGT_ADD_PLAY_AUDIO_LOAD_TASK :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::PlayAudio" , 10.0f );
			bMsgHandle = true;
			BL_AsyncPlayAudioOperation* pkNewOp = m_kPlayAudioPools.GetPoolable();
			pkNewOp->Init( rkMessage.m_lParam1, (AudioLoadArg *)rkMessage.m_pbyParam2, (void *)rkMessage.m_pbyParam3);

			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;
	case MGT_ADD_TMAP_LOAD_TASK :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::LoadMap" , 10.0f );
			bMsgHandle = true;
			BL_AsyncLoadMapOperation* pkNewOp = m_kLoadMapPools.GetPoolable();
			pkNewOp->Init( (const char *)rkMessage.m_pbyParam1 );

			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;
	case MGT_ADD_RELOAD_CONFIG_TASK :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::ReloadConfig" , 10.0f );
			bMsgHandle = true;
			BL_AsyncReloadConfigOperation* pkNewOp = m_kReloadConfigPools.GetPoolable();
			pkNewOp->Init( (TStringVector *)rkMessage.m_pbyParam1 , rkMessage.m_uiParam2);

			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;

	case MGT_YUNYING_ADD_DUOWAN_GET_REPORTABLE:
	case MGT_YUNYING_ADD_DUOWAN_REPORT_CHAT:
	case MGT_YUNYING_ADD_DUOWAN_REPORT_LEVELUP:
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::DuowanReport" , 10.0f );
			bMsgHandle = true;

			BL_AsyncDuowanReportOperation* pkNewOp = m_kDuowanReportPools.GetPoolable();
			if (rkMessage.m_eMsgType == MGT_YUNYING_ADD_DUOWAN_GET_REPORTABLE)	  {
				pkNewOp->Init_1 ( (int)rkMessage.m_lParam1, 
					(const char *)rkMessage.m_pbyParam2, 
					(const char *)rkMessage.m_pbyParam3);
			}
			else if (rkMessage.m_eMsgType == MGT_YUNYING_ADD_DUOWAN_REPORT_CHAT)  {
				pkNewOp->Init_2((const char *)rkMessage.m_pbyParam1, 
								(const char *)rkMessage.m_pbyParam2, 
								(const char *)rkMessage.m_pbyParam3, 
								(int)         rkMessage.m_lParam4, 
								(int)         rkMessage.m_lParam5);
			}
			else if (rkMessage.m_eMsgType == MGT_YUNYING_ADD_DUOWAN_REPORT_LEVELUP)	{
				pkNewOp->Init_3(   (int)rkMessage.m_lParam1, 
					*((unique_id_type *)rkMessage.m_pbyParam2), 
					(const char *)		rkMessage.m_pbyParam3, 
					(int)               rkMessage.m_lParam4, 
					(int)               rkMessage.m_lParam5, 
					(int)               rkMessage.m_lParam5);
			}

			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;

	case MGT_ADD_REPORT_OPERATION_TASK :
		{
			T_PROFILE_EX("AsyncOperationManager::::RecvMessage::HttpUpload" , 10.0f );
			bMsgHandle = true;
			BL_AsyncCurlOperation* pkNewOp = m_kCurlOpPools.GetPoolable();
			pkNewOp->Init( 
				rkMessage.m_lParam3 != 0, 
				(const char*)rkMessage.m_pbyParam1, 
				(const char*)rkMessage.m_pbyParam6,
				(const char*)rkMessage.m_pbyParam2, 
				rkMessage.m_llParam,
				rkMessage.m_uiParam5
				);	// CreartMsg_ADD_REPORT_OPERATION_TASK

			if ( m_pkLoaderThreadProc != NULL )
			{
				_addToWorkingSet( pkNewOp , true );
				m_kLoaderEvent.SetSignaled();
			}
			else
			{
				_processInMainThread( pkNewOp , true );
			}
		}
		break;

	default:
		break;
	}

	return bMsgHandle;

}

bool BL_AsyncOperationManager::_onOpLoaded( BL_AsyncOperation* pkOp )
{
	/*
	由于在进行已经完成加载的操作处理时有大量的重复代码
	这里把这些代码统一到一个查找表中去，在初始化的时候
	建立操作的类型到每一帧该操作的数量和完成操作后的清理
	函数指针的映射表。这样就可以统一处理后台操作完成后的
	处理代码
	*/
	//返回值传给bDiscard，当资源不存在时，返回false。
	bool postLoad = pkOp->PostLoad();
	if (!postLoad)
	{
		return false;
	}

	BL_AsyncOperation::OperationType eType = pkOp->GetType();
	LoadTableIterator iter = m_kLoadTable.find( eType );
	if( iter == m_kLoadTable.end() )
	{
		return true;
	}


	if ( pkOp->GetType() == BL_AsyncOperation::OT_ACTOR 
		&& pkOp->GetOperationState() == BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD )
	{
		BL_AsyncActorOperation* pkActorOp = static_cast<BL_AsyncActorOperation*>(pkOp);
		if ( pkActorOp->IsSceneModel() )
		{
			m_nSceneLoadingTask--;
		}
		else
		{
			m_nModelLoadingTask--; 
		}
	}

	if (pkOp->GetOperationState() != BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD)
	{
		LoadMemFunc func = (*iter->second).pfnLoadedFunc;
		( this->*func )( pkOp );
		return true;
	}
	else
	{
		return false;
	}
}

void BL_AsyncOperationManager::_onOpEnd( BL_AsyncOperation* pkOp )
{
	BL_AsyncOperation::OperationType eType = pkOp->GetType();
	LoadTableIterator iter = m_kLoadTable.find( eType );

	if( iter != m_kLoadTable.end() )
	{
		LoadMemFunc func = (*iter->second).pfnUnLoadedFunc;
		( this->*func )( pkOp );
	}

}
void BL_AsyncOperationManager::_onOpFailed( BL_AsyncOperation* pkOp )
{
	BL_AsyncOperation::OperationType eType = pkOp->GetType();
	LoadTableIterator iter = m_kLoadTable.find( eType );
	if( iter != m_kLoadTable.end() )
	{
		if( eType == BL_AsyncOperation::OT_PATH )
		{
			DefaultRemovePoolable( pkOp , &m_kPathMeshPools );	
			return;
		}
		else if( eType == BL_AsyncOperation::OT_TERRAIN )
		{
			DefaultRemovePoolable( pkOp , &m_kTerrainOpPools );	
			return;
		}
		else
		{
			LoadMemFunc func = (*iter->second).pfnLoadedFunc;
			( this->*func )( pkOp );
		}
	}
	

}

void BL_AsyncOperationManager::Update( float fTime )
{
	T_PROFILE( "Update::BL_AsyncOperationManager" );
	//重置加载标记位
	m_bLoading = false;
	//m_nTerrainLoadingTask = 0;

		m_kWorkingQueueLock.Lock();
		if ( !m_kWorkingQueue.empty() )
		{
			/*
			当角色移动的比较慢的时候，会出现LoadingQueue还有东西
			但是后台线程已经Wait在EventObject上的情况,要定期检查，唤醒后台进行加载*/
			m_kLoaderEvent.SetSignaled();
			//m_nTerrainLoadingTask += m_kWorkingQueue.size();
			GfxWriteLog( LOG_BACKLOADING_FILELINE , LOG_SWITCH_BACKLOADING,
				"Working Queue is %d" , m_kWorkingQueue.size() );
		}
		if( UT_StatisticsManager::GetSingletonPtr() )
		{
			UT_StatisticsManager::GetSingletonPtr()->SetLoadingQueueStat( m_kUpdateQueue.size() , 
				m_kWorkingQueue.size() , m_kChunkLoadedIndexMap.size() );
		}

	// 处理invalid
	_processInvalidModels();

	/*
		为了避免WorkingQueue中堆积过多的相同模型的Loading/UnLoading操作导致加载过慢
		需要在这里清理一次队列，需要注意，在Update的函数中，是在WorkingQueue的锁保护
		范围内的，也就是说，进入到这里的时候我们可以保证在当前workingqueue中的操作对象
		都是没有在执行的。因为后台线程在ExecuteNextOperation的时候是先从WorkingQueue
		中弹出操作，然后立即压入UpdateQueue之后才解锁的
	*/
	/*
		在加载线程中的资源一般有三个状态
		1. 等待处理，一般在workingQueue中；
		2. 已经处理完毕，在updateQueue中；
		3. 正在处理，可能在updateQueue的最后一个元素中；

		如果模型在读取完成之前主线程要求进行销毁，
		那么在workingQueue中的模型可以直接销毁，
		在updateQueue中处理完毕的模型，在postLoad中手动更改状态到BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD，在最后的postLoad中销毁；
		在updateQueue中正在处理的模型，等待他处理完毕之后，在postLoad中手动更改状态到BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD，在最后的postLoad中销毁；
		
	*/
	AsyncOperationQueue kToRemove;
	int nHandledOpCount = 0;
	int nHandledLoadOpCount = 0;
	
	AsyncOperationQueueIter iter;
	TRAV( m_kUpdateQueue , iter )
	{
		BL_AsyncOperation* pkOp = *iter;
		BL_AsyncOperation::OperationType oType = pkOp->GetType();

		bool bDiscard = _processWorkSet( pkOp );
		bool bReinsert = 
			( pkOp->GetOperationState() == 
			BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD );
		if ( bDiscard || bReinsert )
		{
			kToRemove.push_back( pkOp );
		}

		++nHandledOpCount;
		if ( pkOp->IsLoadOp() )
			++nHandledLoadOpCount;

		if( nHandledLoadOpCount > m_uiWorkPerFrame )
			break;
	}

	_cleanUpUpdateQueue(kToRemove);

	//检查一些操作的对象池是不是过大了，如果太大，就删除重新分配一次
	_checkOpPoolSize();
	//如果是单核那么必须要为后台线程留出执行时间
	if ( m_bSingleCore )
	{
		unsigned int nLoadingOpCount = m_kWorkingQueue.size();
		if ( nLoadingOpCount != 0 )
		{
			NiSleep( 1 );
		}
	}

	/// 更新统计信息
	this->_updateMetrics();
	
	m_kWorkingQueueLock.Unlock();
}
bool BL_AsyncOperationManager::_processUpdateSet( AsyncOperationQueue& kToRemove , 
												  int nCount , 
												  BL_AsyncOperation::OperationType eType )
{
	unsigned int uiOpCount = 0;
	bool bLoadWalkable = false;
	if ( eType == BL_AsyncOperation::OT_ACTOR )
	{
		bLoadWalkable = true;
	}
	AsyncOperationQueueIter iter;
	TRAV( m_kUpdateQueue , iter )
	{
		BL_AsyncOperation* pkOp = NULL;
		pkOp = *iter;
		if ( pkOp->GetType() == eType || eType == BL_AsyncOperation::OT_END )
		{
			uiOpCount++;
		}
		else if ( bLoadWalkable && pkOp->GetPriority() != WALKABLE_LOAD_OP_PRIORITY || 
				  pkOp->GetType() != eType/* && eType != BL_AsyncOperation::OT_END*/ )
		{
			continue;
		}
		if( uiOpCount > nCount )
			break;

		//有可能后台线程刚好把任务压入Update队列
		//但是还没有完成加载就进入这里了
		//所以这里可能会出现bDiscard == false的情况

		bool bDiscard = _processWorkSet( pkOp );
		bool bReinsert = 
			( pkOp->GetOperationState() == 
			BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD );
		if ( bDiscard || bReinsert )
		{
			kToRemove.push_back( pkOp );
		}
	}
	if ( uiOpCount > 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
void BL_AsyncOperationManager::_computeChunkLoadingUnLoadingTask( SE_ChunkIndexPool& rkVisibleChunkIndexMap , SE_ChunkIndexPool& rkNewLoadingWorkSet , SE_ChunkIndexPool& rkNewUnLoadingWorkSet )
{
	/**
	每次收到玩家移动消息，需要根据目前的Chunk可视集和目前已经加载和在加载队列中的Chunk，重新计算出需要加载和卸载的Chunk索引号
	需要注意不能重复添加目前已有队列中已经存在的Chunk
	*/
	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "_computeChunkLoadingUnLoadingTask" );
	unsigned int nChunkCount = 0;
	SE_ChunkIndexPoolIter IterBe,IterEd,IterFind,IterFind2;
	{
		// 对于那些可见的Chunk
		IterBe = rkVisibleChunkIndexMap.begin();
		IterEd = rkVisibleChunkIndexMap.end();

		for(; IterBe != IterEd; ++IterBe)
		{
			int iChunkID = (*IterBe).first;
			IterFind = m_kChunkLoadedIndexMap.find( iChunkID );
			IterFind2 = m_kChunkLoadingIndexMap.find( iChunkID );
			bool bInLoadedList = ( IterFind != m_kChunkLoadedIndexMap.end() );		// 不是已经加载了
			bool bInLoadingList = ( IterFind2 != m_kChunkLoadingIndexMap.end() );	// 不是正在加载

			if( !bInLoadingList && !bInLoadedList )
			{
				SE_ChunkIndex& rkChunkIndex = (*IterBe).second;
				rkNewLoadingWorkSet[iChunkID] = rkChunkIndex;		// 进行加载
				GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "Request Loading Chunk %d" , iChunkID );
				nChunkCount++;
			}
		}
	}
	
	/*
		这段代码不知为什么被注掉了，导致离开视野的地形chunk不能及时的被释放。
		经过两天的挂机测试和检查，并没有发现什么问题，重新让它生效。

		author: xyd.
	*/

	//*********************************************************************************************************************
	{
		IterBe = m_kChunkLoadedIndexMap.begin();
		IterEd = m_kChunkLoadedIndexMap.end();

		// 对于那些已加载完的Chunk
		for( ; IterBe != IterEd; ++IterBe)
		{
			int iChunkID = (*IterBe).first;
			IterFind = rkVisibleChunkIndexMap.find(iChunkID);
			IterFind2 = m_kChunkUnLoadingIndexMap.find( iChunkID );
			bool bNotInVisibleList = ( IterFind == rkVisibleChunkIndexMap.end() );		// 不在可视范围内
			bool bNotInUnLoadingList = ( IterFind2 == m_kChunkUnLoadingIndexMap.end() );// 不是正在卸载
			if(bNotInVisibleList && bNotInUnLoadingList )
			{
				SE_ChunkIndex& rkChunkIndex = (*IterBe).second;
				rkNewUnLoadingWorkSet[iChunkID] = rkChunkIndex;		// 进行卸载
				GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "Request UnLoading Chunk %d" , iChunkID );
			}
		}
	}
	//*********************************************************************************************************************
}
void BL_AsyncOperationManager::SuspendLoading( bool bEnable )
{
	if ( m_pkLoaderThreadProc )
	{
		m_pkLoaderThreadProc->Pause( bEnable );
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"set backloading thread as %s" ,bEnable ? "pause":"run" );
	}
	if (m_pkLoaderThreadProcAux)
		m_pkLoaderThreadProcAux->Pause(bEnable);
}

void BL_AsyncOperationManager::StartLoading( long fRadius )
{
	//重新启动后台加载任务
	//清空工作集合元素的顺序ID
	for ( int i = 0 ; i < (int)BL_AsyncOperation::OT_END ; i++ )
	{
		BL_AsyncOperation::ms_iWorkingQueueItemOrderID[i] = 0;
	}
	T_ASSERT( m_pkLoaderThreadProc == NULL );
	m_pkLoaderThreadProc = T_NEW_D BL_AsyncOperationWorkerThread( this );
	bool bStart = m_pkLoaderThreadProc->Start( "BL_AsyncOperationManager::StartThread" );
	if ( !bStart )
	{
		GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
			"后台线程创建失败！");
		T_SAFE_DELETE( m_pkLoaderThreadProc );
		return;
	}

#ifdef USE_MULTI_LOAD_THREADS	//开启微端下载，则开启辅助加载线程，只加载已准备好的资源
	T_ASSERT( m_pkLoaderThreadProcAux == NULL );
	m_pkLoaderThreadProcAux = T_NEW_D BL_AsyncOperationWorkerThread( this );
	m_pkLoaderThreadProcAux->m_eFetchStrategy = SortedOpeartionQueue::POPS_TOP_READY_LOAD_OP;
	bool bStartAux = m_pkLoaderThreadProcAux->Start( "BL_AsyncOperationManager::StartThread Aux" );
	if ( !bStartAux )
	{
		T_SAFE_DELETE( m_pkLoaderThreadProcAux );
	}
#endif

	m_pkVisibleChunkSelector->SetSelectionRadius( (float)fRadius );
	m_bWaitForExit = false;
}

void BL_AsyncOperationManager::StopLoading(bool bQuitClient)
{
	//等待后台加载线程退出
	if ( m_pkLoaderThreadProc == NULL )
	{
		return;
	}
	GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
		"begin killing backloading thread..." );
	//cancel thread, quitting client
	if (bQuitClient)
	{
		m_pkLoaderThreadProc->interrupt();
		if (m_pkLoaderThreadProcAux != NULL)
			m_pkLoaderThreadProcAux->interrupt();

#ifdef USE_DOWNLOADER
		IDownloader* downloader = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
		if (downloader != NULL)
			downloader->Interrupt();
#endif

		/*
			这里直接清空队列会发生泄漏。
			必须经过workQueue--->UpdateQueue的正常流程才行。

			author: xyd.
		*/
		/*	m_kWorkingQueueLock.Lock();
		
		AsyncOperationQueueIter iter;
		
		m_kUpdateQueue.clear();
		m_kWorkingQueue.clear();

		m_kWorkingQueueLock.Unlock();*/
	}
	//如果处于Pause状态（比如这时候锁屏了），就恢复执行，否则会造成主线程死锁
	SuspendLoading(false);
	m_pkLoaderThreadProc->Stop();
	if (m_pkLoaderThreadProcAux != NULL)
		m_pkLoaderThreadProcAux->Stop();

	m_bWaitForExit = true;
	m_pkLoaderThreadProc->WaitForExit();
	m_bWaitForExit = false;
	T_SAFE_DELETE( m_pkLoaderThreadProc );

	if (m_pkLoaderThreadProcAux != NULL)
	{
		m_pkLoaderThreadProcAux->WaitForExit();
		T_SAFE_DELETE( m_pkLoaderThreadProcAux );
	}

	// 和update的流程一样，要先处理invalidModels
	// author: xyd.
	_processInvalidModels();

	//后台线程退出之后workingqueue应该是空的

	unsigned int nLoop = 0;
	do
	{
		if ( !m_kWorkingQueue.empty() )
		{
			GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL,
			"[%d]found unprocessed working queue with size %d" , nLoop , m_kWorkingQueue.size() );
			//改为在主线程执行一次所有的任务，这时后台线程已经退出所以是安全的
			bool bFinish = false;
			while( !bFinish  )
			{		
				bFinish = ExecuteNextOperation();
			}
			GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL,
				"finish working queue clean up" );
		}
		nLoop++;
		if ( nLoop > 100 )
		{
			//不可能出现这么多次循环
			T_ASSERT_CRITICAL_S(false);
		}

		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"begin process update queue..." );
		AsyncOperationQueueIter iter;
		AsyncOperationQueue kNewQueue;
		TRAV( m_kUpdateQueue , iter )
		{
			//处理刚好完成Load但是还没有完成PostLoad的操作
			BL_AsyncOperation* pkOp = *iter;
			bool bDiscard = _processWorkSet( pkOp  );
			if ( !bDiscard )
			{
				switch( pkOp->GetOperationState() )
				{
				case BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD:
					pkOp->SetStatus(false);
					kNewQueue.push_back(pkOp);
					break;
				default:
					//不可能出现这种情况！
					//T_ASSERT(false);
					break;
				}
			}
		}
		m_kUpdateQueue.clear();

		TRAV( kNewQueue , iter )
		{
			BL_AsyncOperation* pkOp = *iter;
			bool bDiscard = _processWorkSet( pkOp );
			if ( !bDiscard )
			{
				switch( pkOp->GetOperationState() )
				{
				case BL_AsyncOperation::LS_UNLOADING:
					//后台还没有完成卸载任务,由于后台线程已经退出，这里需要手动执行一次释放
					pkOp->UnLoad();
					break;
				default:
					//不可能出现这种情况！
					T_ASSERT(false);
					break;
				}
			}
		}
		kNewQueue.clear();
	}while( !m_kWorkingQueue.empty() );

	//完成这个后需要清空当前Chunk加载列表，否则会导致计算需要卸载的Chunk时把WorldMap中已经删除的Chunk也算进去的问题
	m_kChunkLoadedIndexMap.clear();
	m_kChunkLoadingIndexMap.clear();
    m_kLastSelectEyeLocation = NiPoint2::ZERO;
	m_kLastCameraRot = NiPoint2::ZERO;
	m_kInvalidModels.clear();
	//强制D3D把ManagedPool做一次清理以便释放不需要的内存
	/*NiDX9Renderer* pDevice = NiDynamicCast(NiDX9Renderer,NiRenderer::GetRenderer());

	if(pDevice)
	{
		LPDIRECT3DDEVICE9 lpDD9 = pDevice->GetD3DDevice();

		if(lpDD9)
		{
			lpDD9->EvictManagedResources();
		}
	}*/

	GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
		"finish killing backloading thread." );

	if ( !m_kWorkingQueue.empty() )
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL,
			"after clean up working queue is not empty , size %d" , m_kWorkingQueue.size() );
		T_ASSERT_CRITICAL_S(false);
	}
}

bool BL_AsyncOperationManager::ExecuteNextOperation(SortedOpeartionQueue::POP_STRATEGY fetchNext, BL_AsyncOperationWorkerThread *OpThread)
{
	//在后台线程中执行
	m_kWorkingQueueLock.Lock();

	//这里可能会影响效率！所以先从队列弹出后再执行Load/Unload操作
	BL_AsyncOperation * pkNextOp = NULL;
	while( (pkNextOp = m_kWorkingQueue.pop_top(fetchNext)) != NULL )
	{
		m_kUpdateQueue.push_back( pkNextOp );	//不管操作成功失败，都会进入m_kUpdateQueue

		//当有任务的时候必须返回False，继续进行任务的加载
		if ( pkNextOp->GetOperationState() == BL_AsyncOperation::LS_START_LOADING )
		{
			m_kWorkingQueueLock.Unlock();
			
			IThreadControl *pCtrl = OpThread ? &OpThread->m_interruptCtrl : NULL;
			GfxWriteLog( LOG_TAG_BACKLOADING , LOG_SWITCH_BACKLOADING ,  "%s, 0x%08x, fetchMethod=%d, Loading... OpType=%d, Res=%s" , __FUNCTION__, pkNextOp, fetchNext, pkNextOp->GetType(), pkNextOp->getResPath().c_str() );
			pkNextOp->Load(pCtrl);
			
			return false;
		}
		else if ( pkNextOp->GetOperationState() == BL_AsyncOperation::LS_UNLOADING )
		{
			m_kWorkingQueueLock.Unlock();
			
			GfxWriteLog( LOG_TAG_BACKLOADING , LOG_SWITCH_BACKLOADING ,  "%s,  0x%08x, fetchMethod=%d, Unloading... OpType=%d, Res=%s" , __FUNCTION__, pkNextOp, fetchNext, pkNextOp->GetType(), pkNextOp->getResPath().c_str() );
			pkNextOp->UnLoad();
			
			return false;
		}
		else
		{
			//工作集合里面不可能出现其他状态
			T_ASSERT(false);
		}
	}
	//当没有任务在队列中的时候要返回True
	m_kWorkingQueueLock.Unlock();
	return true;
}

void BL_AsyncOperationManager::_onTerrainLoaded( BL_AsyncOperation* pkOp )
{
	//更新当前已经加载的Chunk的工作集集合
	BL_AsyncTerrainOperation* pkTerrainOp = static_cast<BL_AsyncTerrainOperation*>(pkOp);
	const SE_ChunkIndexPool& rkWorkSet = pkTerrainOp->GetWorkSetIndexMap();
	SE_ChunkIndexPool::const_iterator iter2;
	TRAV( rkWorkSet , iter2 )
	{
		const SE_ChunkIndex& rkChunkIndex = (*iter2).second;
		int iTileIndexX = rkChunkIndex._tileIndexX;
		int iTileIndexY = rkChunkIndex._tileIndexY;
		int iChunkClusterIndexX = rkChunkIndex._clusterIndexX;
		int iChunkClusterIndexY = rkChunkIndex._clusterIndexY;
		int iChunkIndexX = rkChunkIndex._chunkIndexX;
		int iChunkIndexY = rkChunkIndex._chunkIndexY;
		int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,iTileIndexY,iChunkClusterIndexX,iChunkClusterIndexY,iChunkIndexX,iChunkIndexY);
		//加入到LoadedSet队列中
		m_kChunkLoadedIndexMap[iChunkID] = rkChunkIndex;
		//从Loading队列中删除掉
		SE_ChunkIndexPoolIter iter = m_kChunkLoadingIndexMap.find( iChunkID );
		if ( iter != m_kChunkLoadingIndexMap.end() )
		{
			m_kChunkLoadingIndexMap.erase( iter );
		}
	}
	//删除指针
	DefaultRemovePoolable( pkOp , &m_kTerrainOpPools );	
	m_nSceneLoadingTask--;
}

void BL_AsyncOperationManager::_onTerrainUnLoaded( BL_AsyncOperation* pkOp )
{
	BL_AsyncTerrainOperation* pkTerrainOp = static_cast<BL_AsyncTerrainOperation*>(pkOp);
	const SE_ChunkIndexPool& rkWorkSet = pkTerrainOp->GetWorkSetIndexMap();
	SE_ChunkIndexPool::const_iterator iter2;
	TRAV( rkWorkSet , iter2 )
	{
		const SE_ChunkIndex& rkChunkIndex = (*iter2).second;
		int iTileIndexX = rkChunkIndex._tileIndexX;
		int iTileIndexY = rkChunkIndex._tileIndexY;
		int iChunkClusterIndexX = rkChunkIndex._clusterIndexX;
		int iChunkClusterIndexY = rkChunkIndex._clusterIndexY;
		int iChunkIndexX = rkChunkIndex._chunkIndexX;
		int iChunkIndexY = rkChunkIndex._chunkIndexY;
		int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,iTileIndexY,iChunkClusterIndexX,iChunkClusterIndexY,iChunkIndexX,iChunkIndexY);
		SE_ChunkIndexPoolIter IterFind = m_kChunkLoadedIndexMap.find(iChunkID);
		if(IterFind != m_kChunkLoadedIndexMap.end())
		{
			m_kChunkLoadedIndexMap.erase(IterFind);
			GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "Unload Chunk ID%d" , iChunkID );
		}
		else
		{
			GfxWriteLog( LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "发现不一致的加载队列数据!Chunk ID %d" , iChunkID );
		}
		SE_ChunkIndexPoolIter IterFind2 = m_kChunkUnLoadingIndexMap.find( iChunkID );
		if ( IterFind2 != m_kChunkUnLoadingIndexMap.end() )
		{
			m_kChunkUnLoadingIndexMap.erase( IterFind2 );
		}
	}
	DefaultRemovePoolable( pkOp , &m_kTerrainOpPools );	
}

void BL_AsyncOperationManager::_onActorLoaded( BL_AsyncOperation* pkOp )
{
	BL_AsyncActorOperation* pkActorOp = static_cast<BL_AsyncActorOperation*>(pkOp);
	if ( pkActorOp->IsSceneModel() )
	{
		m_nSceneLoadingTask--;
	}
	else
	{
		m_nModelLoadingTask--;
	}
	if ( pkActorOp->GetModel() )
	{
		pkActorOp->GetModel()->SetAsyncOperation( NULL );
	}
	DefaultRemovePoolable( pkOp , &m_kActorOpPools );
}

void BL_AsyncOperationManager::_onActorUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kActorOpPools );	
}

void BL_AsyncOperationManager::_onChangeWeaponLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kChangeWeaponPools );	
}

void BL_AsyncOperationManager::_onChangeWeaponUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kChangeWeaponPools );	
}

void BL_AsyncOperationManager::_onGfxLoaded( BL_AsyncOperation* pkOp )
{
	if ( pkOp->GetOperationState() == BL_AsyncOperation::LS_FAIL )
	{
		BL_AsyncFxLoadOperation* pkFxOp = static_cast<BL_AsyncFxLoadOperation*>(pkOp);
		if ( pkFxOp && pkFxOp->GetFxObject() )
		{
			FX_ClientFxManager::GetSingleton().PostCreateFxObject(
				*pkFxOp->GetFxObject(), false);
		}
	}
	
	DefaultRemovePoolable( pkOp , &m_kGfxPools );	
}

void BL_AsyncOperationManager::_onGfxUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kGfxPools );	
}

void BL_AsyncOperationManager::_onUIImgLoaded( BL_AsyncOperation* pkOp )
{
	//BL_AsyncUIImgLoadOperation* pkImgOp = (BL_AsyncUIImgLoadOperation*)pkOp;
	DefaultRemovePoolable( pkOp , &m_kUIImgPools );	
}

void BL_AsyncOperationManager::_onUIImgUnLoaded( BL_AsyncOperation* pkOp )
{
	//BL_AsyncUIImgLoadOperation* pkImgOp = (BL_AsyncUIImgLoadOperation*)pkOp;
	DefaultRemovePoolable( pkOp , &m_kUIImgPools );	
}

void BL_AsyncOperationManager::_onPathMeshLoaded( BL_AsyncOperation* pkOp )
{
	BL_AsyncPathMeshOperation* pkMeshOp = (BL_AsyncPathMeshOperation*)pkOp;
	PF_ClientPathManager::GetSingleton().FinishTileLoad( pkMeshOp->GetTileIndex() );
	DefaultRemovePoolable( pkOp , &m_kPathMeshPools );	
}

void BL_AsyncOperationManager::_onPathMeshUnLoaded( BL_AsyncOperation* pkOp )
{
	BL_AsyncPathMeshOperation* pkMeshOp = (BL_AsyncPathMeshOperation*)pkOp;
	PF_ClientPathManager::GetSingleton().FinishTileUnLoad( pkMeshOp->GetTileIndex() );
	DefaultRemovePoolable( pkOp , &m_kPathMeshPools );	
}

void BL_AsyncOperationManager::_onVideoLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kPlayVideoPools );	
}

void BL_AsyncOperationManager::_onVideoUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kPlayVideoPools );	
}

void BL_AsyncOperationManager::_onAudioLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kPlayAudioPools );	
}

void BL_AsyncOperationManager::_onAudioUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kPlayAudioPools );	
}

void BL_AsyncOperationManager::_onTMapLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kLoadMapPools );
}

void BL_AsyncOperationManager::_onTMapUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kLoadMapPools );
}

void BL_AsyncOperationManager::_onReloadConfigLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kReloadConfigPools );
}

void BL_AsyncOperationManager::_onReloadConfigUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kReloadConfigPools );
}

void BL_AsyncOperationManager::_onDuowanReportLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kDuowanReportPools );
}

void BL_AsyncOperationManager::_onDuowanReportUnLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kDuowanReportPools );
}

void BL_AsyncOperationManager::_onCurlOpLoaded( BL_AsyncOperation* pkOp )
{
	DefaultRemovePoolable( pkOp , &m_kCurlOpPools );
}


void BL_AsyncOperationManager::_addNewChunkTerrainOperation( SE_ChunkIndexPool &kSet )
{
	BL_AsyncTerrainOperation* pkNewLoadingTask = m_kTerrainOpPools.GetPoolable();
	pkNewLoadingTask->Init( m_pkProcessMap , kSet );
	if ( m_pkLoaderThreadProc != NULL )
	{
		_addToWorkingSet( pkNewLoadingTask , true );
	}
	else
	{
		_processInMainThread( pkNewLoadingTask , true );
	}

	kSet.clear();
}

void BL_AsyncOperationManager::_updateMetrics()
{
	int nChunkTask		= m_kTerrainOpPools.GetActivePoolableSize();
	int nActorTask		= m_kActorOpPools.GetActivePoolableSize();
	int nPathMeshTask	= m_kPathMeshPools.GetActivePoolableSize();
	int nActorItemTask	= m_kChangeWeaponPools.GetActivePoolableSize();
	int nGfxTask		= m_kGfxPools.GetActivePoolableSize();
	int nUIImgTask		= m_kUIImgPools.GetActivePoolableSize();

	NIMETRICS_APPLICATION_AGGREGATEVALUE( ACTIVE_CHUNK_LOADING_TASK , Utility::Int2Float( nChunkTask ) );
	NIMETRICS_APPLICATION_AGGREGATEVALUE( ACTIVE_ACTOR_LOADING_TASK , Utility::Int2Float( nActorTask ) );
	NIMETRICS_APPLICATION_AGGREGATEVALUE( ACTIVE_PATH_MESH_LOADING_TASK , Utility::Int2Float( nPathMeshTask ) );
	NIMETRICS_APPLICATION_AGGREGATEVALUE( ACTIVE_AVATAR_LOADING_TASK , Utility::Int2Float( nActorItemTask ) );
	NIMETRICS_APPLICATION_AGGREGATEVALUE( ACTIVE_GFX_LOADING_TASK , Utility::Int2Float( nGfxTask ) );
	NIMETRICS_APPLICATION_AGGREGATEVALUE( ACTIVE_UI_LOADINGIMG_TASK , Utility::Int2Float( nUIImgTask ) );
}

void BL_AsyncOperationManager::_addToWorkingSet( BL_AsyncOperation* pkTask , bool bLoading )
{
	_preprocessTask(pkTask, bLoading);

	m_kWorkingQueueLock.Lock();
	m_kWorkingQueue.push( pkTask );
	//m_kWorkingQueue.print();
	m_kWorkingQueueLock.Unlock();
}

bool BL_AsyncOperationManager::_processWorkSet( BL_AsyncOperation* pkOp )
{
	//Update和WorkingQueue都会走这个函数处理
	bool bDiscard = false;
	switch ( pkOp->GetOperationState() )
	{
	case BL_AsyncOperation::LS_START_LOADING:
		//后台线程还没有完成加载任务
		bDiscard = false;
		break;
	case BL_AsyncOperation::LS_LOADING:
		//后台线程已经完成加载任务，那么执行PostLoad
		bDiscard = _onOpLoaded( pkOp );
		break;
	case BL_AsyncOperation::LS_START_UNLOADING:
		//刚开始进行卸载任务
		pkOp->PreUnLoad();
		bDiscard = false;
		break;
	case BL_AsyncOperation::LS_UNLOADING:
		//后台还没有完成卸载任务
		bDiscard = false;
		break;
	case BL_AsyncOperation::LS_UNLOADED:
		_onOpEnd( pkOp );
		bDiscard = true;
		break;
	case BL_AsyncOperation::LS_FAIL:
		_onOpFailed(pkOp);
		bDiscard = true;
		break;
	default:
		bDiscard = false;
		//T_ASSERT(false);
		break;
	}
	return bDiscard;
}
#define CHECK_OP_POOL( name , size ) \
	if( name.GetPoolSize() > size && name.GetActivePoolableSize() == 0 ) \
	{\
		name.DeletePool();\
		name.SetPoolSize( size );\
	}\

void BL_AsyncOperationManager::_checkOpPoolSize()
{
	CHECK_OP_POOL( m_kTerrainOpPools , 40 );
	CHECK_OP_POOL( m_kActorOpPools , 200 );
	CHECK_OP_POOL( m_kChangeWeaponPools , 40 );
	CHECK_OP_POOL( m_kGfxPools , 40 );
	CHECK_OP_POOL( m_kUIImgPools , 40 );
}

void BL_AsyncOperationManager::_initOpPool()
{
	m_kTerrainOpPools.SetPoolSize( 40 );
	m_kActorOpPools.SetPoolSize( 200 );
	m_kChangeWeaponPools.SetPoolSize( 40 );
	m_kGfxPools.SetPoolSize( 40 );
	m_kUIImgPools.SetPoolSize( 40 );
}

void BL_AsyncOperationManager::_handleChunkLoading()
{
	// 获取所有可视Chunk
	NiCameraPtr spCam = CA_CameraManager::GetSingleton().GetNiCamera();
	m_pkVisibleChunkSelector->SelectVisibleChunks( m_kLastSelectEyeLocation , spCam );

	// 结合当前已处理的情况，计算哪些需要加载，哪些需要卸载
	SE_ChunkIndexPool kVisibleChunkPool = m_pkProcessMap->GetVisibleChunks();
	SE_ChunkIndexPool kNewLoadingWorkSet,kNewUnLoadingWorkSet;
	_computeChunkLoadingUnLoadingTask( kVisibleChunkPool , kNewLoadingWorkSet , kNewUnLoadingWorkSet );

	//1.生成Loading的操作对象，放入Worker线程的队列中
	//2.生成UnLoading的操作对象，但是暂时不放入Worker线程队列，需要主线程执行了PreUnLoading再插入
	//由于一个Chunk的PostLoad时间也会比较久，所以，我们需要限制一个TerrainOperation里面包含的Chunk数量
	if ( !kNewLoadingWorkSet.empty() )
	{
		SE_ChunkIndexPoolIter iter;
		SE_ChunkIndexPool kSet;
		TRAV( kNewLoadingWorkSet , iter )
		{
			if ( kSet.size() == 1 )		// 为什么要这么写，一个Op只加载一个Chunk？
			{
				_addNewChunkTerrainOperation(kSet);
			}
			kSet.insert( std::make_pair( iter->first , iter->second ) );
		}
		if ( !kSet.empty() )
		{
			_addNewChunkTerrainOperation( kSet );
		}
	}
	if ( !kNewUnLoadingWorkSet.empty() )
	{
		BL_AsyncTerrainOperation* pkNewUnLoadingTask = m_kTerrainOpPools.GetPoolable();
		pkNewUnLoadingTask->Init( m_pkProcessMap , kNewUnLoadingWorkSet );
		if ( m_pkLoaderThreadProc != NULL )
		{
			_addToWorkingSet(pkNewUnLoadingTask , false );
		}
		else
		{
			_processInMainThread( pkNewUnLoadingTask , false );
		}
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING,"Add New UnLoading BL Op with %d chunks" , kNewUnLoadingWorkSet.size() );
	}
	
	if ( m_pkLoaderThreadProc != NULL && ( !kNewLoadingWorkSet.empty() || !kNewUnLoadingWorkSet.empty() ) )
	{
		//唤醒后台加载线程
		m_kLoaderEvent.SetSignaled();
	}
#if (_MSC_VER == 1600)
	SE_ChunkIndexPool::iterator it;
	TRAV( kNewLoadingWorkSet , it )
	{
		m_kChunkLoadingIndexMap.insert( *it );
	}
	TRAV( kNewUnLoadingWorkSet , it )
	{
		m_kChunkUnLoadingIndexMap.insert( *it );
	}
#else
	std::copy( kNewLoadingWorkSet.begin() , kNewLoadingWorkSet.end() , inserter( m_kChunkLoadingIndexMap , m_kChunkLoadingIndexMap.begin() ) );
	std::copy( kNewUnLoadingWorkSet.begin() , kNewUnLoadingWorkSet.end() , inserter( m_kChunkUnLoadingIndexMap , m_kChunkUnLoadingIndexMap.begin() ) );
#endif
#if 1
	if ( g_settings.CFG_ENABLE_BACKLOADING_LOG )
	{
		StringStreamType output;
		SE_ChunkIndexPoolIter iter;
		TRAV( m_kChunkLoadingIndexMap , iter )
		{
			output<<iter->first<<",";
		}
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "Current Loading Chunk List is %s" , output.str().c_str() );

		StringStreamType output2;
		SE_ChunkIndexPoolIter iter2;
		TRAV( m_kChunkUnLoadingIndexMap , iter2 )
		{
			output2<<iter2->first<<",";
		}
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING,"Current UnLoading Chunk List is %s" , output2.str().c_str() );
	}
#endif
}

void BL_AsyncOperationManager::NotifyInvalidModel( GD_ModelBase* pkModel )
{
	//m_kWorkingQueueLock.Lock();
	T_ASSERT( pkModel != NULL );
	m_kInvalidModels.push_back( pkModel );
	//m_kWorkingQueueLock.Unlock();
}

void BL_AsyncOperationManager::AddModelOperationMap( Data::GD_ModelBase* pkModel , BL_AsyncOperation* pkOp )
{
	T_ASSERT_CRITICAL_S( pkModel );
	T_ASSERT_CRITICAL_S( pkOp );
	T_ASSERT_CRITICAL_S( pkModel->GetAsyncOperation() == NULL );
	pkModel->SetAsyncOperation( pkOp );
	GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL,
		"attach operation to model [0x%x]",
		pkModel );

}

void BL_AsyncOperationManager::_cleanUpUpdateQueue( AsyncOperationQueue& kToRemove )
{
	AsyncOperationQueueIter iter;
	TRAV( kToRemove , iter )
	{
		BL_AsyncOperation* pkOp = NULL;
		pkOp = *iter;
		m_kUpdateQueue.remove( pkOp );
		bool bReinsert = 
			( pkOp->GetOperationState() == 
			BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD );
		if ( bReinsert )
		{
			pkOp->SetStatus(false);
			pkOp->PreUnLoad();
			m_kWorkingQueue.push( pkOp );
		}
	}
	kToRemove.clear();
}

void BL_AsyncOperationManager::_processInMainThread( BL_AsyncOperation* pkNewOp , bool bLoading )
{
	_preprocessTask( pkNewOp , bLoading );

	if ( pkNewOp->GetOperationState() == BL_AsyncOperation::LS_START_LOADING )
	{
		pkNewOp->Load(NULL);
	}
	else if ( pkNewOp->GetOperationState() == BL_AsyncOperation::LS_UNLOADING )
	{
		pkNewOp->UnLoad();
	}
	else
	{
		T_ASSERT(false);
	}
	bool bDiscard = _processWorkSet( pkNewOp  );
	if ( !bDiscard && 
		pkNewOp->GetOperationState() == BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD )
	{
		pkNewOp->SetStatus(false);
		bDiscard = _processWorkSet( pkNewOp );
		if ( !bDiscard && 
			pkNewOp->GetOperationState() == BL_AsyncOperation::LS_UNLOADING )
		{
			pkNewOp->UnLoad();
		}
	}
}

void BL_AsyncOperationManager::_preprocessTask( BL_AsyncOperation* pkTask, bool bLoading )
{
	if ( pkTask->GetType() == BL_AsyncOperation::OT_TERRAIN && bLoading )
	{
		m_nSceneLoadingTask++;
	}
	if ( pkTask->GetType() == BL_AsyncOperation::OT_ACTOR && bLoading )
	{
		BL_AsyncActorOperation* pkActorOp = static_cast<BL_AsyncActorOperation*>(pkTask);
		if ( pkActorOp->IsSceneModel() )
			m_nSceneLoadingTask++;
		else
			m_nModelLoadingTask++;
	}

	pkTask->SetStatus( bLoading );
	if ( !bLoading )
	{
		pkTask->PreUnLoad();
	}
	//后台处理队列里面的任务只可能在这两种状态中
	T_ASSERT( ( pkTask->GetOperationState() == BL_AsyncOperation::LS_START_LOADING ) ||
		( pkTask->GetOperationState() == BL_AsyncOperation::LS_UNLOADING ) );
}

void BL_AsyncOperationManager::_processInvalidModels()
{
	/*
		 @ 处理GD_ModelBase的专用逻辑;
		   对所有还没有加载完就要删除的模型直接从WorkingQueue中去掉其加载对象
	*/

	AsyncOperationQueue kToRemove;
	if ( !m_kInvalidModels.empty() )
	{
		ModelSet::iterator mIter;
		TRAV( m_kInvalidModels , mIter )
		{
			GD_ModelBase* pkModel = *mIter;
			BL_AsyncOperation* pkOp = pkModel->GetAsyncOperation();


			/*

				@ 所有的InvalidModel在客户端或者其他引擎中的引用均已经被解除，
				如果能保证他不是一个"正在加载中"的任务，可以自由处理（删除或者放入卸载队列）。
				
				@ InvalidModel所携带的BL_AsyncOperation可能会有三种情况：
				1. BL_AsyncOperation为空：发起加载行为的model，发送请求加载消息时的发送方式不是immediate，且这个model在BL_AsyncOperation生成之前被要求释放；
										  BL_AsyncOperation的load返回LS_FAIL（这个需要修正一下）；
				2. BL_AsyncOperation位于workQueue队列：直接释放model。
				3. BL_AsyncOperation位于updateQeueu队列：在model读取的逻辑中，如果operatestate已确认为LS_LOADING或者LS_FAIL，说明这不是一个“正在加载中”的任务，可以自由处理。
			*/

			if( pkOp == NULL )
			{
				// Invalidmodel的一个极端情况：
				// 发起加载行为的model，发送请求加载消息时的发送方式不是immediate，且这个model在BL_AsyncOperation生成之前被要求释放；

				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,
					"invalid model [0x%08x][%s] has no operation attached!",
					pkModel, pkModel->GetModFileName().c_str() );

#ifdef _SHIPPING
				
				GfxWriteLog(LOG_TAG_RENDER, LOG_SWITCH_RENDER, 
					"[Thread State] BL_AsyncOperation::invalid model [0x%08x][%s] has no operation attached!", 
					pkModel, pkModel->GetModFileName().c_str() );
#else

				//T_ASSERT_CRITICAL_S(false);
#endif

				pkModel->PreRelease();
				T_SAFE_DELETE(pkModel);

				continue;
			}
		
			bool bRemoved = m_kWorkingQueue.remove( pkOp );
			if( bRemoved )
			{
				// Invalidmodel仍然在workQueue
				GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL,
					"remove invalid model [0x%08x][%s] from working queue.", 
					pkModel,  pkModel->GetModFileName().c_str() );

				_onOpFailed( pkOp );

				pkModel->SetAsyncOperation(NULL);
				pkModel->PreRelease();
				T_DELETE_D pkModel;
			}
			else
			{
				// Invalidmodel已经进入了updateQueue.
				BL_AsyncOperation::LoadState loadState = pkOp->GetOperationState(); 

				if ( loadState == BL_AsyncOperation::LS_LOADING )
				{
					//有可能这个时候已经在后台进行加载了，所以会找不到这个任务
					//这个任务会在PostLoad的时候直接跳过，然后进入UnLoad队列
					//这里直接处理掉这个任务
					GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
						"remove invalid model [0x%08x][%s][%d] from update queue.", 
						pkModel, pkModel->GetModFileName().c_str(), loadState );

					_onOpLoaded( pkOp );
					/*bool bReinsert = 
						( pkOp->GetOperationState() == 
						BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD );
					T_ASSERT_CRITICAL_S( bReinsert );
					kToRemove.push_back( pkOp );*/
				}
				else if ( loadState == BL_AsyncOperation::LS_FAIL )
				{
					GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
						"remove invalid model [0x%08x][%s] from update queue [%d]!", 
						pkModel, pkModel->GetModFileName().c_str(), loadState );

					_onOpFailed( pkOp );

					pkModel->SetAsyncOperation(NULL);
					pkModel->PreRelease();
					T_SAFE_DELETE(pkModel);

					kToRemove.push_back( pkOp );
				}
				else
				{
					GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
						"remove invalid a loading model [0x%08x][%s] from update queue [%d]!", 
						pkModel, pkModel->GetModFileName().c_str(), loadState);

				}
			}
		}
		if ( kToRemove.empty() == false )
		{
			GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL,
				"remove %d invalid op from update queue" , 
				kToRemove.size() );
		}
		_cleanUpUpdateQueue(kToRemove);
		m_kInvalidModels.clear();
	}
}
