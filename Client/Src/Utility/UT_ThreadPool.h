//�������UpdateЧ�ʵ��̳߳�
#pragma once

#include "NiLoopedThreadProcedure.h"
#include "NiLoopedThread.h"
#include "UT_MemoryPool.h"
#include "GD_ClientAsyncModelManager.h"

/**
	�̳߳���һ��Update����Ľӿ���
*/
class UT_ThreadPoolTask : public Memory::MM_BaseObject
{
public:
	UT_ThreadPoolTask() {}
	virtual ~UT_ThreadPoolTask() {}

	virtual void DoTask( float fTime ) = 0;
};

/**
	��ɫUpdate������ʵ����
	���GD_ClientActorModel��Update����
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
	һ��Update����Ķ�����
	�����е�Task���Ա������̳߳��в���ִ��
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
	��ɫ��Update���������
	��IM_Manager�л�õ�ǰ֡��UpdateTask�б�
	�����䵽��ǰ���̳߳���ȥִ��
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
	Update�̳߳�
	��һ���������������л�ȡUpdate����
	���֧��8���߳�ͬʱ���в��е�Update
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
