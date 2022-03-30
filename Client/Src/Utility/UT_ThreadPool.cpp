#include "ClientPCH.h"
#include "GD_ClientActorModel.h"
#include "UT_ThreadPool.h"

//==============================================================================
// class UT_ModelUpdateTask
//==============================================================================

void UT_ModelUpdateTask::DoTask( float fTime )
{
	// 更新Actor
	// 注意，灯光和物理的Update必须在这之前就已经完成了
	m_pkModel->UpdateModelContent( fTime );
}

//==============================================================================
// class UT_ModelUpdateTaskQueue
//==============================================================================

UT_ModelUpdateTaskQueue::UT_ModelUpdateTaskQueue()
: m_rkModelManager(GD_ClientAsyncModelManager::GetSingleton())
{
	m_pkTaskPool = T_NEW_D Utility::UT_MemoryPool<UT_ModelUpdateTask>();
	m_pkTaskPool->Init( 64 , 64 );
}

UT_ModelUpdateTaskQueue::~UT_ModelUpdateTaskQueue()
{
	ClearQueue();
	T_DELETE_D m_pkTaskPool;
}

UT_ThreadPoolTask* UT_ModelUpdateTaskQueue::GetTaskAt( unsigned int k )
{
	if ( k >=0 && k < m_kTaskQueue.size() )
	{
		return m_kTaskQueue[k];
	}
	else
	{
		return NULL;
	}
}

//void UT_ModelUpdateTaskQueue::PrepareTaskQueue()
//{
//	ClearQueue();
//
//}

void UT_ModelUpdateTaskQueue::ClearQueue()
{
	PoolTaskList::iterator iter;
	TRAV( m_kTaskQueue , iter )
	{
		m_pkTaskPool->Free( (UT_ModelUpdateTask*)(*iter) );
	}
	m_kTaskQueue.clear();
}

unsigned int UT_ModelUpdateTaskQueue::GetTaskCount()
{
	return m_kTaskQueue.size();
}

void UT_ModelUpdateTaskQueue::AddTask(Data::GD_ModelBase* pkModel)
{
    UT_ModelUpdateTask* pkNewTask = m_pkTaskPool->Alloc();
    pkNewTask->SetActor(pkModel);

    m_kTaskQueue.push_back(pkNewTask);
}

//==============================================================================
// class UT_UpdateThreadPool
//==============================================================================

float UT_UpdateThreadPool::UpdateProc::ms_fTime = 0.0f;
UT_UpdateThreadPool* UT_UpdateThreadPool::UpdateProc::ms_pkPool = NULL;

UT_UpdateThreadPool::UT_UpdateThreadPool()
{
}

UT_UpdateThreadPool::~UT_UpdateThreadPool()
{
}

void UT_UpdateThreadPool::Init()
{
	//获取当前系统的处理器个数，决定线程池的大小
	m_uiThreadCount = min(NiSystemDesc::GetSystemDesc().GetLogicalProcessorCount(), g_settings.LOD_ACTOR_THREAD_POOL_SIZE);
    m_uiThreadCount = min(m_uiThreadCount, MAX_THREADS);

    m_uiNumThreads = m_uiThreadCount;

	for ( unsigned int uiT = 0; uiT < m_uiThreadCount; uiT++ )
	{
		m_apkUpdateThreads[uiT] = NiLoopedThread::Create(&m_akUpdateTProcs[uiT]);
		m_apkUpdateThreads[uiT]->SetPriority(NiThread::NORMAL);
		m_apkUpdateThreads[uiT]->SetName( "Actor Update Thread" );
		m_apkUpdateThreads[uiT]->Resume();
	}
}

void UT_UpdateThreadPool::UnInit()
{
	for (unsigned int uiT = 0; uiT < m_uiThreadCount; uiT++)
	{
		NiDelete m_apkUpdateThreads[uiT];
	}
}

void UT_UpdateThreadPool::Update( float fTime )
{
	//从共享的任务队列中分配任务给线程池中的线程对象
	unsigned int uiRemaining = m_pkWorkSet->GetTaskCount();
	unsigned int uiNumThreads = uiRemaining >= m_uiNumThreads ? m_uiNumThreads : uiRemaining;

	unsigned int uiPos = 0;
	if (uiNumThreads > 0)
	{
		unsigned int uiDivSize = uiRemaining / uiNumThreads;

		UpdateProc::ms_fTime = fTime;
		UpdateProc::ms_pkPool = this;

		for (unsigned int uiT = 0; uiT < uiNumThreads - 1; uiT++)
		{
			m_akUpdateTProcs[uiT].m_uiPos = uiPos;
			m_akUpdateTProcs[uiT].m_uiChildren = uiDivSize;
			uiRemaining -= uiDivSize;
			m_apkUpdateThreads[uiT]->DoLoop();
			uiPos+=uiDivSize;
		}
		unsigned int uiLast = uiNumThreads - 1;
		m_akUpdateTProcs[uiLast].m_uiPos = uiPos;
		m_akUpdateTProcs[uiLast].m_uiChildren = uiRemaining;
		m_apkUpdateThreads[uiLast]->DoLoop();

        for (unsigned int uiTWait = 0; uiTWait < uiNumThreads; uiTWait++)
        {
            m_apkUpdateThreads[uiTWait]->WaitForLoopCompletion();
        }
	}
}

bool UT_UpdateThreadPool::UpdateProc::LoopedProcedure( void* pvArg )
{
	for (unsigned int uiC = 0; uiC < m_uiChildren; uiC++)
	{
		UT_ThreadPoolTask* pkTask = ms_pkPool->m_pkWorkSet->GetTaskAt( m_uiPos + uiC );
		pkTask->DoTask( ms_fTime );
	}

	return true;
}
