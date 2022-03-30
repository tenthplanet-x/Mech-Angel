//用于提高Update效率的线程池
#pragma once

#include "NiLoopedThreadProcedure.h"
#include "NiLoopedThread.h"
#include "UT_MemoryPool.h"
#include "GD_ClientAsyncModelManager.h"

/**
	线程池中一个Update任务的接口类
*/
class UT_ThreadPoolTask : public Memory::MM_BaseObject
{
public:
	UT_ThreadPoolTask() {}
	virtual ~UT_ThreadPoolTask() {}

	virtual void DoTask( float fTime ) = 0;
};

/**
	角色Update的任务实现类
	完成GD_ClientActorModel的Update操作
*/
class UT_ModelUpdateTask : public UT_ThreadPoolTask
{
public:
	UT_ModelUpdateTask() : m_pkModel(NULL) {}
	virtual ~UT_ModelUpdateTask() {}

    virtual void DoTask( float fTime );

    void SetActor( Data::GD_ModelBase* pkModel ) { m_pkModel = pkModel; }

protected:
	Data::GD_ModelBase* m_pkModel;
};
typedef mem::vector<UT_ThreadPoolTask*> PoolTaskList;

/**
	一次Update任务的队列类
	队列中的Task可以被放入线程池中并行执行
*/
class UT_ThreadPoolTaskQueue : public Memory::MM_BaseObject
{
public:
	UT_ThreadPoolTaskQueue() {}
	virtual ~UT_ThreadPoolTaskQueue() {}

	//virtual void				PrepareTaskQueue() = 0;
	virtual void				ClearQueue() = 0;
	virtual unsigned int		GetTaskCount() = 0;
	virtual UT_ThreadPoolTask*  GetTaskAt( unsigned int k ) = 0;
};

/**
	角色的Update任务队列类
	从IM_Manager中获得当前帧的UpdateTask列表
	并分配到当前的线程池中去执行
*/
class UT_ModelUpdateTaskQueue : public UT_ThreadPoolTaskQueue
{
public:
	UT_ModelUpdateTaskQueue();
	virtual ~UT_ModelUpdateTaskQueue();

	//virtual void				 PrepareTaskQueue();
	virtual void				 ClearQueue();
	virtual unsigned int		 GetTaskCount();
	virtual UT_ThreadPoolTask*   GetTaskAt( unsigned int k );

    void AddTask(Data::GD_ModelBase* pkModel);

protected:
    GD_ClientAsyncModelManager&                 m_rkModelManager;

	Utility::UT_MemoryPool<UT_ModelUpdateTask>* m_pkTaskPool;
	PoolTaskList                                m_kTaskQueue;
};

/**
	Update线程池
	从一个共享的任务队列中获取Update任务
	最多支持8个线程同时进行并行的Update
*/
class UT_UpdateThreadPool : public Memory::MM_BaseObject
{
public:
	enum
	{
		MAX_THREADS = 8,
	};

	UT_UpdateThreadPool();
	virtual ~UT_UpdateThreadPool();

	void Init();
	void UnInit();

    void SetTaskQueue( UT_ThreadPoolTaskQueue* pkWorkSet ) { m_pkWorkSet = pkWorkSet; }
    void SetThreadsCount( unsigned int uiCount ) { if (uiCount < MAX_THREADS && uiCount > 0) m_uiNumThreads = uiCount; }

	void Update( float fTime );

protected:
	unsigned int            m_uiThreadCount;
	unsigned int            m_uiNumThreads;
	UT_ThreadPoolTaskQueue* m_pkWorkSet;

	class UpdateProc : public NiLoopedThreadProcedure
	{
	public:
		virtual bool LoopedProcedure(void* pvArg);

		static UT_UpdateThreadPool* ms_pkPool;
		static float				ms_fTime;

		unsigned int				m_uiPos;
		unsigned int				m_uiChildren;
	};
	UpdateProc       m_akUpdateTProcs[MAX_THREADS];
	NiLoopedThread*  m_apkUpdateThreads[MAX_THREADS];
};
