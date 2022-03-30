#include "ClientPCH.h"
#include "BS_ClientMaterialManager.h"
#include "BL_AsyncOperationManager.h"
#include "MG_MessageBase.h"

using namespace Utility;
//----------------------------------------------------------
BS_ClientMaterialManager::BS_ClientMaterialManager()
: GD_MaterialManager()
{
	m_fLastPurgeTime = 60.0f;
}
//----------------------------------------------------------
BS_ClientMaterialManager::~BS_ClientMaterialManager()
{
}
//----------------------------------------------------------
void BS_ClientMaterialManager::_preCreateProtoType()
{
	if (Utility::GfxIsMainThread() && BL_AsyncOperationManager::GetSingletonPtr() 
		&& BL_AsyncOperationManager::GetSingletonPtr()->IsLoadingThreadAlive() )
	{
		GfxWriteLog(LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING,
			"在主线程中创建材质，可能会和后台线程冲突造成纹理无法加载！");
	}
}

void BS_ClientMaterialManager::_preReleaseProtoType(void)
{
    if (Utility::GfxIsMainThread() && BL_AsyncOperationManager::GetSingletonPtr()  
		&& BL_AsyncOperationManager::GetSingletonPtr()->IsLoadingThreadAlive() )
    {
        GfxWriteLog(LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING,
            "在主线程中释放材质，可能会和后台线程冲突！");
    }
}

bool BS_ClientMaterialManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
	END_MESSAGE_HANDLE

	return true;
}


void BS_ClientMaterialManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
	this->_updateMetrics();
}

void BS_ClientMaterialManager::_eventTimerTick2(MG_MessageBase& rkMessage)
{
	/*
		邹凯所加入的遍历材质的部分；
		稍后再加入；

		author: xyd;
		date;   2016/3/5
	*/
	static const float PURGE_INTERVAL = 5.0f;
	T_PROFILE("Update::BS_ClientMaterialManager");

	float fCurrentTime = rkMessage.m_fParam1;
	if (fCurrentTime - m_fLastPurgeTime > PURGE_INTERVAL)
	{
		_purgeMaterials();

		m_fLastPurgeTime = fCurrentTime;
	}
}
