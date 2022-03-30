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
	��Loader�е�������ж�ִ����֮�󣬺�̨�߳̾�Wait�ں����¼�������
	ֱ�����߳����µ�������뻽�Ѻ�̨�߳�
	*/
	GfxWriteLog( LOG_TAG_BACKLOADING , LOG_SWITCH_BACKLOADING ,  "%s Start. fetchMethod=%d" , __FUNCTION__, m_eFetchStrategy );
	SetThreadName( "Async Terrain Loading Thread" );
	SetPriority( Priority_Normal );

	//���ѭ��ֻ�����߳�Ҫ�˳���ʱ��Ż��˳�
	while( !m_bDone )
	{
		//ͨ�����Ķ�������߳̽���ͬ��
		//�����߳����µļ�������ʱ�ͻ�
		//����LoaderEvent��ʹ�����ǿ���
		//�����ڲ�ѭ��
		m_pkLoader->m_kLoaderEvent.Wait();
		m_pkLoader->m_kLoaderEvent.Reset();
		bool bLoadingEmpty = false;
		//�ڲ�ѭ����ͣ�Ĳ�ѯ�Ƿ��м�������
		while( !bLoadingEmpty  )
		{		
			if ( !m_bPause )
			{
				bLoadingEmpty = m_pkLoader->ExecuteNextOperation(m_eFetchStrategy, this);
				if (this->m_interruptCtrl.canceled())	//�ж���Ӧ���˳�
					break;
			}
		}

		if (!m_bDone)
			::Sleep(10);
	}
}

void BL_AsyncOperationWorkerThread::Stop()
{
	//�����ѭ���˳������ΪTrue
	m_bDone = true;
	//�������Ķ�������ڲ�ѭ������û����ɵļ�������
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
	//����һ������صĴ�С
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
	//ȷ�����ʱ���̨�߳��Ѿ��ɾ���������
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
	�����ڽ����Ѿ���ɼ��صĲ�������ʱ�д������ظ�����
	�������Щ����ͳһ��һ�����ұ���ȥ���ڳ�ʼ����ʱ��
	�������������͵�ÿһ֡�ò�������������ɲ����������
	����ָ���ӳ��������Ϳ���ͳһ�����̨������ɺ��
	�������
	*/
	//����ֵ����bDiscard������Դ������ʱ������false��
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
	//���ü��ر��λ
	m_bLoading = false;
	//m_nTerrainLoadingTask = 0;

		m_kWorkingQueueLock.Lock();
		if ( !m_kWorkingQueue.empty() )
		{
			/*
			����ɫ�ƶ��ıȽ�����ʱ�򣬻����LoadingQueue���ж���
			���Ǻ�̨�߳��Ѿ�Wait��EventObject�ϵ����,Ҫ���ڼ�飬���Ѻ�̨���м���*/
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

	// ����invalid
	_processInvalidModels();

	/*
		Ϊ�˱���WorkingQueue�жѻ��������ͬģ�͵�Loading/UnLoading�������¼��ع���
		��Ҫ����������һ�ζ��У���Ҫע�⣬��Update�ĺ����У�����WorkingQueue��������
		��Χ�ڵģ�Ҳ����˵�����뵽�����ʱ�����ǿ��Ա�֤�ڵ�ǰworkingqueue�еĲ�������
		����û����ִ�еġ���Ϊ��̨�߳���ExecuteNextOperation��ʱ�����ȴ�WorkingQueue
		�е���������Ȼ������ѹ��UpdateQueue֮��Ž�����
	*/
	/*
		�ڼ����߳��е���Դһ��������״̬
		1. �ȴ�����һ����workingQueue�У�
		2. �Ѿ�������ϣ���updateQueue�У�
		3. ���ڴ���������updateQueue�����һ��Ԫ���У�

		���ģ���ڶ�ȡ���֮ǰ���߳�Ҫ��������٣�
		��ô��workingQueue�е�ģ�Ϳ���ֱ�����٣�
		��updateQueue�д�����ϵ�ģ�ͣ���postLoad���ֶ�����״̬��BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD��������postLoad�����٣�
		��updateQueue�����ڴ����ģ�ͣ��ȴ����������֮����postLoad���ֶ�����״̬��BL_AsyncOperation::LS_TRANSFER_TO_UNLOAD��������postLoad�����٣�
		
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

	//���һЩ�����Ķ�����ǲ��ǹ����ˣ����̫�󣬾�ɾ�����·���һ��
	_checkOpPoolSize();
	//����ǵ�����ô����ҪΪ��̨�߳�����ִ��ʱ��
	if ( m_bSingleCore )
	{
		unsigned int nLoadingOpCount = m_kWorkingQueue.size();
		if ( nLoadingOpCount != 0 )
		{
			NiSleep( 1 );
		}
	}

	/// ����ͳ����Ϣ
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

		//�п��ܺ�̨�̸߳պð�����ѹ��Update����
		//���ǻ�û����ɼ��ؾͽ���������
		//����������ܻ����bDiscard == false�����

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
	ÿ���յ�����ƶ���Ϣ����Ҫ����Ŀǰ��Chunk���Ӽ���Ŀǰ�Ѿ����غ��ڼ��ض����е�Chunk�����¼������Ҫ���غ�ж�ص�Chunk������
	��Ҫע�ⲻ���ظ����Ŀǰ���ж������Ѿ����ڵ�Chunk
	*/
	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "_computeChunkLoadingUnLoadingTask" );
	unsigned int nChunkCount = 0;
	SE_ChunkIndexPoolIter IterBe,IterEd,IterFind,IterFind2;
	{
		// ������Щ�ɼ���Chunk
		IterBe = rkVisibleChunkIndexMap.begin();
		IterEd = rkVisibleChunkIndexMap.end();

		for(; IterBe != IterEd; ++IterBe)
		{
			int iChunkID = (*IterBe).first;
			IterFind = m_kChunkLoadedIndexMap.find( iChunkID );
			IterFind2 = m_kChunkLoadingIndexMap.find( iChunkID );
			bool bInLoadedList = ( IterFind != m_kChunkLoadedIndexMap.end() );		// �����Ѿ�������
			bool bInLoadingList = ( IterFind2 != m_kChunkLoadingIndexMap.end() );	// �������ڼ���

			if( !bInLoadingList && !bInLoadedList )
			{
				SE_ChunkIndex& rkChunkIndex = (*IterBe).second;
				rkNewLoadingWorkSet[iChunkID] = rkChunkIndex;		// ���м���
				GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, "Request Loading Chunk %d" , iChunkID );
				nChunkCount++;
			}
		}
	}
	
	/*
		��δ��벻֪Ϊʲô��ע���ˣ������뿪��Ұ�ĵ���chunk���ܼ�ʱ�ı��ͷš�
		��������Ĺһ����Ժͼ�飬��û�з���ʲô���⣬����������Ч��

		author: xyd.
	*/

	//*********************************************************************************************************************
	{
		IterBe = m_kChunkLoadedIndexMap.begin();
		IterEd = m_kChunkLoadedIndexMap.end();

		// ������Щ�Ѽ������Chunk
		for( ; IterBe != IterEd; ++IterBe)
		{
			int iChunkID = (*IterBe).first;
			IterFind = rkVisibleChunkIndexMap.find(iChunkID);
			IterFind2 = m_kChunkUnLoadingIndexMap.find( iChunkID );
			bool bNotInVisibleList = ( IterFind == rkVisibleChunkIndexMap.end() );		// ���ڿ��ӷ�Χ��
			bool bNotInUnLoadingList = ( IterFind2 == m_kChunkUnLoadingIndexMap.end() );// ��������ж��
			if(bNotInVisibleList && bNotInUnLoadingList )
			{
				SE_ChunkIndex& rkChunkIndex = (*IterBe).second;
				rkNewUnLoadingWorkSet[iChunkID] = rkChunkIndex;		// ����ж��
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
	//����������̨��������
	//��չ�������Ԫ�ص�˳��ID
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
			"��̨�̴߳���ʧ�ܣ�");
		T_SAFE_DELETE( m_pkLoaderThreadProc );
		return;
	}

#ifdef USE_MULTI_LOAD_THREADS	//����΢�����أ��������������̣߳�ֻ������׼���õ���Դ
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
	//�ȴ���̨�����߳��˳�
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
			����ֱ����ն��лᷢ��й©��
			���뾭��workQueue--->UpdateQueue���������̲��С�

			author: xyd.
		*/
		/*	m_kWorkingQueueLock.Lock();
		
		AsyncOperationQueueIter iter;
		
		m_kUpdateQueue.clear();
		m_kWorkingQueue.clear();

		m_kWorkingQueueLock.Unlock();*/
	}
	//�������Pause״̬��������ʱ�������ˣ����ͻָ�ִ�У������������߳�����
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

	// ��update������һ����Ҫ�ȴ���invalidModels
	// author: xyd.
	_processInvalidModels();

	//��̨�߳��˳�֮��workingqueueӦ���ǿյ�

	unsigned int nLoop = 0;
	do
	{
		if ( !m_kWorkingQueue.empty() )
		{
			GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL,
			"[%d]found unprocessed working queue with size %d" , nLoop , m_kWorkingQueue.size() );
			//��Ϊ�����߳�ִ��һ�����е�������ʱ��̨�߳��Ѿ��˳������ǰ�ȫ��
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
			//�����ܳ�����ô���ѭ��
			T_ASSERT_CRITICAL_S(false);
		}

		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"begin process update queue..." );
		AsyncOperationQueueIter iter;
		AsyncOperationQueue kNewQueue;
		TRAV( m_kUpdateQueue , iter )
		{
			//����պ����Load���ǻ�û�����PostLoad�Ĳ���
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
					//�����ܳ������������
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
					//��̨��û�����ж������,���ں�̨�߳��Ѿ��˳���������Ҫ�ֶ�ִ��һ���ͷ�
					pkOp->UnLoad();
					break;
				default:
					//�����ܳ������������
					T_ASSERT(false);
					break;
				}
			}
		}
		kNewQueue.clear();
	}while( !m_kWorkingQueue.empty() );

	//����������Ҫ��յ�ǰChunk�����б�����ᵼ�¼�����Ҫж�ص�Chunkʱ��WorldMap���Ѿ�ɾ����ChunkҲ���ȥ������
	m_kChunkLoadedIndexMap.clear();
	m_kChunkLoadingIndexMap.clear();
    m_kLastSelectEyeLocation = NiPoint2::ZERO;
	m_kLastCameraRot = NiPoint2::ZERO;
	m_kInvalidModels.clear();
	//ǿ��D3D��ManagedPool��һ�������Ա��ͷŲ���Ҫ���ڴ�
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
	//�ں�̨�߳���ִ��
	m_kWorkingQueueLock.Lock();

	//������ܻ�Ӱ��Ч�ʣ������ȴӶ��е�������ִ��Load/Unload����
	BL_AsyncOperation * pkNextOp = NULL;
	while( (pkNextOp = m_kWorkingQueue.pop_top(fetchNext)) != NULL )
	{
		m_kUpdateQueue.push_back( pkNextOp );	//���ܲ����ɹ�ʧ�ܣ��������m_kUpdateQueue

		//���������ʱ����뷵��False��������������ļ���
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
			//�����������治���ܳ�������״̬
			T_ASSERT(false);
		}
	}
	//��û�������ڶ����е�ʱ��Ҫ����True
	m_kWorkingQueueLock.Unlock();
	return true;
}

void BL_AsyncOperationManager::_onTerrainLoaded( BL_AsyncOperation* pkOp )
{
	//���µ�ǰ�Ѿ����ص�Chunk�Ĺ���������
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
		//���뵽LoadedSet������
		m_kChunkLoadedIndexMap[iChunkID] = rkChunkIndex;
		//��Loading������ɾ����
		SE_ChunkIndexPoolIter iter = m_kChunkLoadingIndexMap.find( iChunkID );
		if ( iter != m_kChunkLoadingIndexMap.end() )
		{
			m_kChunkLoadingIndexMap.erase( iter );
		}
	}
	//ɾ��ָ��
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
			GfxWriteLog( LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "���ֲ�һ�µļ��ض�������!Chunk ID %d" , iChunkID );
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
	//Update��WorkingQueue�����������������
	bool bDiscard = false;
	switch ( pkOp->GetOperationState() )
	{
	case BL_AsyncOperation::LS_START_LOADING:
		//��̨�̻߳�û����ɼ�������
		bDiscard = false;
		break;
	case BL_AsyncOperation::LS_LOADING:
		//��̨�߳��Ѿ���ɼ���������ôִ��PostLoad
		bDiscard = _onOpLoaded( pkOp );
		break;
	case BL_AsyncOperation::LS_START_UNLOADING:
		//�տ�ʼ����ж������
		pkOp->PreUnLoad();
		bDiscard = false;
		break;
	case BL_AsyncOperation::LS_UNLOADING:
		//��̨��û�����ж������
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
	// ��ȡ���п���Chunk
	NiCameraPtr spCam = CA_CameraManager::GetSingleton().GetNiCamera();
	m_pkVisibleChunkSelector->SelectVisibleChunks( m_kLastSelectEyeLocation , spCam );

	// ��ϵ�ǰ�Ѵ���������������Щ��Ҫ���أ���Щ��Ҫж��
	SE_ChunkIndexPool kVisibleChunkPool = m_pkProcessMap->GetVisibleChunks();
	SE_ChunkIndexPool kNewLoadingWorkSet,kNewUnLoadingWorkSet;
	_computeChunkLoadingUnLoadingTask( kVisibleChunkPool , kNewLoadingWorkSet , kNewUnLoadingWorkSet );

	//1.����Loading�Ĳ������󣬷���Worker�̵߳Ķ�����
	//2.����UnLoading�Ĳ������󣬵�����ʱ������Worker�̶߳��У���Ҫ���߳�ִ����PreUnLoading�ٲ���
	//����һ��Chunk��PostLoadʱ��Ҳ��ȽϾã����ԣ�������Ҫ����һ��TerrainOperation���������Chunk����
	if ( !kNewLoadingWorkSet.empty() )
	{
		SE_ChunkIndexPoolIter iter;
		SE_ChunkIndexPool kSet;
		TRAV( kNewLoadingWorkSet , iter )
		{
			if ( kSet.size() == 1 )		// ΪʲôҪ��ôд��һ��Opֻ����һ��Chunk��
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
		//���Ѻ�̨�����߳�
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
	//��̨����������������ֻ������������״̬��
	T_ASSERT( ( pkTask->GetOperationState() == BL_AsyncOperation::LS_START_LOADING ) ||
		( pkTask->GetOperationState() == BL_AsyncOperation::LS_UNLOADING ) );
}

void BL_AsyncOperationManager::_processInvalidModels()
{
	/*
		 @ ����GD_ModelBase��ר���߼�;
		   �����л�û�м������Ҫɾ����ģ��ֱ�Ӵ�WorkingQueue��ȥ������ض���
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

				@ ���е�InvalidModel�ڿͻ��˻������������е����þ��Ѿ��������
				����ܱ�֤������һ��"���ڼ�����"�����񣬿������ɴ���ɾ�����߷���ж�ض��У���
				
				@ InvalidModel��Я����BL_AsyncOperation���ܻ������������
				1. BL_AsyncOperationΪ�գ����������Ϊ��model���������������Ϣʱ�ķ��ͷ�ʽ����immediate�������model��BL_AsyncOperation����֮ǰ��Ҫ���ͷţ�
										  BL_AsyncOperation��load����LS_FAIL�������Ҫ����һ�£���
				2. BL_AsyncOperationλ��workQueue���У�ֱ���ͷ�model��
				3. BL_AsyncOperationλ��updateQeueu���У���model��ȡ���߼��У����operatestate��ȷ��ΪLS_LOADING����LS_FAIL��˵���ⲻ��һ�������ڼ����С������񣬿������ɴ���
			*/

			if( pkOp == NULL )
			{
				// Invalidmodel��һ�����������
				// ���������Ϊ��model���������������Ϣʱ�ķ��ͷ�ʽ����immediate�������model��BL_AsyncOperation����֮ǰ��Ҫ���ͷţ�

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
				// Invalidmodel��Ȼ��workQueue
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
				// Invalidmodel�Ѿ�������updateQueue.
				BL_AsyncOperation::LoadState loadState = pkOp->GetOperationState(); 

				if ( loadState == BL_AsyncOperation::LS_LOADING )
				{
					//�п������ʱ���Ѿ��ں�̨���м����ˣ����Ի��Ҳ����������
					//����������PostLoad��ʱ��ֱ��������Ȼ�����UnLoad����
					//����ֱ�Ӵ�����������
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
