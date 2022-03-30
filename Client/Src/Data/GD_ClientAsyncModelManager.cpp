//***********************************************************************************
#include "ClientPCH.h"
#include "GD_ClientAsyncModelManager.h"

#include "GD_ModelBase.h"
#include "GD_ClientActorModel.h"
#include "GD_ClientChestModel.h"
#include "GD_ClientBreakModel.h"

#include "TS_WorldConfig.h"

#include "MG_MessageBase.h"
#include "BL_AsyncOperationManager.h"

#include "UT_ThreadPool.h"
#include "UT_Win32Helper.h"
#include "IM_Manager.h"
//***********************************************************************************
using namespace Data;
using namespace Utility;
//***********************************************************************************
// GD_ClientAsyncModelManager
//***********************************************************************************
GD_ClientAsyncModelManager::GD_ClientAsyncModelManager(void)
: m_pkUpdatePool(NULL)
, m_pkUpdateTaskQueue(NULL)
, m_bUpdateModels(true)
, m_fDiscardedPoolCapcitiesScale(1.0f)
, m_fDiscardedPoolAutoClearTimeInterval(5.0f)
, m_fDiscardedPoolAutoClearTimeIntervalScale(1.0f)
, m_fDiscardedPoolAutoClearTimeElapsed(0.0f)
, m_iDiscardedPoolAutoClearIntensity(1)
, m_fDiscardedPoolAutoClearIntensityScale(1.0f)

{
    m_anDiscardedModelCapacities[AMT_SCENE] = 0;
    m_anDiscardedModelCapacities[AMT_PLAYER] = 10;
    m_anDiscardedModelCapacities[AMT_ITEM] = 0;
    m_anDiscardedModelCapacities[AMT_NPC] = 5;
    m_anDiscardedModelCapacities[AMT_CHEST] = 5;
    m_anDiscardedModelCapacities[AMT_LOGIN] = 0;
    m_anDiscardedModelCapacities[AMT_LOGIN_CAMERA_ANIM] = 0;
    m_anDiscardedModelCapacities[AMT_LOGIN_WEAPON_TOTEM] = 0;

    if (g_settings.LOD_USE_ACTOR_THREAD_POOL)
    {
        m_pkUpdateTaskQueue = T_NEW_D UT_ModelUpdateTaskQueue;

        m_pkUpdatePool = T_NEW_D UT_UpdateThreadPool;
        m_pkUpdatePool->Init();
        m_pkUpdatePool->SetTaskQueue(m_pkUpdateTaskQueue);
    }
}
//-----------------------------------------------------------------------------------
GD_ClientAsyncModelManager::~GD_ClientAsyncModelManager(void)
{
    if (!m_kPendingModels.empty())
    {
        for (PendingModelMap::iterator itr = m_kPendingModels.begin(); itr != m_kPendingModels.end(); ++itr)
        {
            GD_ModelBase* pkModel = (*itr).first;

            T_DELETE_D pkModel;
        }
        m_kPendingModels.clear();
    }

    if (!m_kModels.empty())
    {
        for (ModelTypeMap::iterator itr = m_kModels.begin(); itr != m_kModels.end(); ++itr)
        {
            GD_ModelBase* pkModel = (*itr).first;

            pkModel->PreRelease();
            T_DELETE_D pkModel;
        }
        m_kModels.clear();
    }

    for (int i = 0; i < AMT_MAX_COUNT; ++i)
    {
        if (!m_kDiscardedModels[i].empty())
        {
            for (ModelList::iterator itr = m_kDiscardedModels[i].begin(); itr != m_kDiscardedModels[i].end(); ++itr)
            {
                GD_ModelBase* pkModel = (*itr);

                pkModel->PreRelease();
                T_DELETE_D pkModel;
            }
            m_kDiscardedModels[i].clear();
        }
    }

    T_SAFE_DELETE(m_pkUpdateTaskQueue);
    if (m_pkUpdatePool)
    {
        m_pkUpdatePool->UnInit();
        T_SAFE_DELETE(m_pkUpdatePool);
    }
}

//-----------------------------------------------------------------------------------
// MG_Message
//-----------------------------------------------------------------------------------
bool GD_ClientAsyncModelManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
	END_MESSAGE_HANDLE

	return true;
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
	//@ 更新discaredPool
	_updateDiscardedPoolModels(rkMessage.m_fParam2);

	//@ 激活所有的PendingModel 
	if (!m_kPendingModelsFromDiscardedPool.empty())
	{
		ModelList::iterator iter = 
			m_kPendingModelsFromDiscardedPool.begin();
		ModelList::iterator iend = 
			m_kPendingModelsFromDiscardedPool.end();

		while ( iter != iend )
			(*(iter++))->Activate();

		m_kPendingModelsFromDiscardedPool.clear();
	}

	//@ 更新
	if (!m_bUpdateModels)
	{
		return;
	}

	float fCurrentTime = rkMessage.m_fParam1;

	if (g_settings.LOD_USE_ACTOR_THREAD_POOL)
	{
		T_PROFILE("Update::GD_ClientAsyncModelManager::ParallelUpdateModel");
		m_pkUpdateTaskQueue->ClearQueue();
		for (int i = 0; i < AMT_MAX_COUNT; ++i)
		{
			for (ModelSet::iterator itr = m_kDynamicModels[i].begin(); itr != m_kDynamicModels[i].end(); ++itr)
			{
				GD_ModelBase* pkModel = (*itr);
				if (pkModel != NULL)
				{
					m_pkUpdateTaskQueue->AddTask(*itr);
				}
			}
		}
		{
			m_pkUpdatePool->Update(fCurrentTime);
		}
		{
			for (int i = 0; i < AMT_MAX_COUNT; ++i)
			{
				for (ModelSet::iterator itr = m_kDynamicModels[i].begin(); itr != m_kDynamicModels[i].end(); ++itr)
				{
					GD_ModelBase* pkModel = (*itr);
					if (pkModel != NULL)
					{
						pkModel->PostUpdate();
					}
				}
			}
		}
	}
	else
	{
		T_PROFILE("Update::GD_ClientAsyncModelManager::UpdateModel");
		for (int i = 0; i < AMT_MAX_COUNT; ++i)
		{
			for (ModelSet::iterator itr = m_kDynamicModels[i].begin(); itr != m_kDynamicModels[i].end(); ++itr)
			{
				GD_ModelBase* pkModel = (*itr);
				if (pkModel != NULL)
				{
					pkModel->EntirelyUpdate(fCurrentTime);
				}
			}
		}

		if ( IM_Manager::GetSingletonPtr()->IsPendingShowOrHideShield(IM_Manager::SHIELD_SCENE_FX) )
			IM_Manager::GetSingletonPtr()->CloseShieldPending(IM_Manager::SHIELD_SCENE_FX);
	}
}
//-----------------------------------------------------------------------------------
// Creation & Release
//-----------------------------------------------------------------------------------
bool GD_ClientAsyncModelManager::IsAsyncModel(GD_ModelBase* pkModel)
{
	if (pkModel != NULL)
	{
		ModelTypeMap::iterator itr = m_kModels.find(pkModel);
		if (itr != m_kModels.end())
		{
			return true;
		}
	}

	return false;
}

GD_ModelBase* GD_ClientAsyncModelManager::CreateModel(ASYNC_MODEL_TYPE eModelType, 
								const StringType& strModFileName, 
								int iModelIndex/* = 0*/, bool bAsyncLoad/* = true */)

{
	 LoadingModelInfo kModelInfo;
	 kModelInfo.iModelIndex = iModelIndex;

	 return CreateModel(eModelType, strModFileName, kModelInfo, bAsyncLoad);
}
//-----------------------------------------------------------------------------------
GD_ModelBase* GD_ClientAsyncModelManager::CreateModel( ASYNC_MODEL_TYPE eModelType, 
                                                      const StringType& strModFileName, 
                                                      const LoadingModelInfo& kModelInfo, bool bAsyncLoad/* = true */)
{
    T_ASSERT_CRITICAL_S(GfxIsMainThread());

    GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
        "Start to create model [%s] in type %d", strModFileName.c_str(), eModelType);

	bool bIsModelInited = false;

	// 解析shaderIdentifer
	Terrain::TS_WorldConfig::ShaderIdentifier kShaderIdentifier;
	kShaderIdentifier.bRenderToUITexture = kModelInfo.bRenderToUITexture;
	kShaderIdentifier.bUseUltraShaderMode = kModelInfo.bRenderUltraMode;
	kShaderIdentifier.ucUltraShaderIndex = kModelInfo.ucRenderUltraModeIndex;
	unsigned int uiShaderIdentifier = GfxGetWorldConfig().CompressShaderIdentifier(kShaderIdentifier);

    // 根据类型创建模型对象
    GD_ModelBase* pkModel = _doNewAsyncModel( 
		eModelType, strModFileName, kModelInfo.iModelIndex, uiShaderIdentifier, bIsModelInited );

    if (pkModel == NULL)
        return NULL;

	for(int i = 0; i < g_settings.MOD_FX_WITHOUT_CULL.size(); ++i)
	{
		if (strModFileName.find(g_settings.MOD_FX_WITHOUT_CULL[i]) != StringType::npos)
		{
			pkModel->SetFxUseModelCullLogic(false);
			break;
		}
	}

	pkModel->SetManualUpdateEnable( kModelInfo.bManualUpdate );
    pkModel->SetPostInitCallbackFunc(_modelPostInitCallback);
    pkModel->SetLoadFailedCallbackFunc(_modelPostInitCallback);

	// 默认不关闭特效震屏
	pkModel->SetProperty_AttachFxDisableCameraShake(true);

    // 模型对象加到模型管理器的异步队列中去
    std::pair<PendingModelMap::iterator, bool> kPair = m_kPendingModels.insert(
        std::make_pair(pkModel, PendingModelInfo(eModelType, strModFileName)));
    T_ASSERT(kPair.second);

    if (!bIsModelInited)
    {
        if ( bAsyncLoad )
        {
            // 添加异步加载任务给后台线程
            SendMessage( CreateMsg_ADD_MODEL_LOADING_TASK(
                eModelType, pkModel, (*(kPair.first)).second.strModFileName, kModelInfo.iModelIndex, uiShaderIdentifier ) , true );

            GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
                "Send out load-model message for model [0x%x] [%s]", 
				pkModel, strModFileName.c_str() );
        }
        else
        {
            pkModel->InitInMainThread( strModFileName, kModelInfo.iModelIndex, uiShaderIdentifier );
        }
    }
    else
    {
        pkModel->SetVisibility(true);
        pkModel->SetIsUpdatable(true);

        // 从缓存池中取回复用的模型不能立即激活，因为有些初始化操作依赖创建后设入的属性，如 CharType 等
        m_kPendingModelsFromDiscardedPool.push_back(pkModel);
        T_ASSERT(kPair.second);
    }

    return pkModel;
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::ReleaseModel(GD_ModelBase* pkModel)
{
    T_ASSERT_CRITICAL_S(GfxIsMainThread() && pkModel != NULL);

    ModelTypeMap::iterator iter = m_kModels.find(pkModel);
    if (iter != m_kModels.end())
    {
        ASYNC_MODEL_TYPE eModelType = (*iter).second;

		if ( _addDiscardedPoolModel(eModelType, pkModel ) )
		{
			pkModel->Deactivate();
			m_kModels.erase(iter);
		}
		else
		{
			if (_releaseModel(pkModel))
			{
				m_kModels.erase(iter);
			}
		}

        ModelSet::iterator itrDynamicModel = m_kDynamicModels[eModelType].find(pkModel);
        if (itrDynamicModel != m_kDynamicModels[eModelType].end())
        {
            m_kDynamicModels[eModelType].erase(itrDynamicModel);
        }
    }
    else
    {
        PendingModelMap::iterator iter = m_kPendingModels.find(pkModel);
        T_ASSERT(iter != m_kPendingModels.end());

        // 复用的模型对象还没正式启用就要被释放
        bool bReusableModelDiscarded = false;
        if (!m_kPendingModelsFromDiscardedPool.empty())
        {
            ModelList::iterator iter2 = find(m_kPendingModelsFromDiscardedPool.begin(), m_kPendingModelsFromDiscardedPool.end(), pkModel);
            if (iter2 != m_kPendingModelsFromDiscardedPool.end())
            {
                m_kPendingModelsFromDiscardedPool.erase(iter2);
                _addDiscardedPoolModel((*iter).second.eType, pkModel, true);

                bReusableModelDiscarded = true;
            }
        }
        
        if (!bReusableModelDiscarded)
        {
            GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
                "Model [0x%x] needs to be released before loaded [%s]",
                (*iter).first, (*iter).second.strModFileName.c_str());

            // 还没有加载完就企图删除了，不能直接删除模型，否则可能造成数据破坏
            // 向后台线程管理器注册这个模型已经不需要加载了。同时从Pending队列中
            // 删除掉。后台线程管理器在主线程Update的时候则查询这些模型所属的Operation
            // 执行的状态，如果还没有在后台加载，那么就直接删除掉这个Operation和模型本身
            // 如果已经在后台加载了，那么就不管了，在执行PostLoad时候会检查到有ReleaseFlag
            // 则会直接跳过Postload转入Unload队列
            (*iter).second.bNeedToBeReleased = true;
            (*iter).first->SetReleaseFlag(true);

            if ( BL_AsyncOperationManager::GetSingletonPtr() && 
                BL_AsyncOperationManager::GetSingleton().IsLoadingThreadAlive() )
            {
                BL_AsyncOperationManager::GetSingleton().NotifyInvalidModel( (*iter).first );
            }
            else
            {
                GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
                    "Release model [0x%x] before loaded when loading-thread is NOT alive [%s]",
                    (*iter).first, (*iter).second.strModFileName.c_str());

                T_DELETE_D (*iter).first;
            }
        }
        m_kPendingModels.erase(iter);
    }
}
//-----------------------------------------------------------------------------------
GD_ModelBase* GD_ClientAsyncModelManager::_doNewAsyncModel(ASYNC_MODEL_TYPE eModelType, 
														   const StringType& strModFileName, 
														   int iModFileIndex, unsigned int uiShaderIdentifier,
														   bool& bIsModelInited )
{
	GD_ModelBase* pkModel = _getDiscardedPoolModel( eModelType, strModFileName, iModFileIndex, uiShaderIdentifier );
	if ( pkModel )
		bIsModelInited = true;

	if (pkModel == NULL)
	{
		switch (eModelType)
		{
		case AMT_SCENE:
			pkModel = T_NEW_D GD_ClientSceneModel;
			break;
		case AMT_PLAYER:
			pkModel = T_NEW_D GD_ClientActorModel;
			((GD_ClientActorModel*)pkModel)->SetIsPlayer(true);
			break;
		case AMT_NPC:
			pkModel = T_NEW_D GD_ClientActorModel;
			if ( strModFileName.find(ASYNCMODE_MAIN_CHAR) != -1 )
			{
				((GD_ClientActorModel*)pkModel)->SetIsPlayer(true);
			}
			break;
		case AMT_CHEST:
			pkModel = T_NEW_D GD_ClientChestModel;
			break;
		case AMT_NPC_DEAD:
			pkModel = T_NEW_D GD_ClientBreakModel;
			break;
		default:
			T_ASSERT(false);
			break;
		}
	}

	return pkModel;
}

//-----------------------------------------------------------------------------------
bool GD_ClientAsyncModelManager::_releaseModel(GD_ModelBase* pkModel)
{
	GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
		"Start to release model [%s], ptr [0x%x]",
		pkModel->GetModelFileName().c_str() , pkModel );

	pkModel->PreRelease();

	// 发送卸载消息成功才把对象从列表中移除，避免内存泄漏
	return SendMessage(CreateMsg_ADD_MODEL_UNLOADING_TASK(pkModel));
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::_onModelInited(GD_ModelBase* pkModel, bool bLoadFailed /*= false*/)
{
	T_ASSERT_CRITICAL_S(GfxIsMainThread());

	PendingModelMap::iterator itr = m_kPendingModels.find(pkModel);
	if (itr != m_kPendingModels.end())
	{
		PendingModelInfo kPendingModelInfo = (*itr).second;
		m_kPendingModels.erase(itr);

		std::pair<ModelTypeMap::iterator, bool> kPair = m_kModels.insert(std::make_pair(pkModel, kPendingModelInfo.eType));
		T_ASSERT(kPair.second);

		if (!bLoadFailed)
		{
			if (pkModel->IsDynamic() /*&& kPendingModelInfo.bUpdateable*/)
			{
				m_kDynamicModels[kPendingModelInfo.eType].insert(pkModel);
			}
		}

		if (kPendingModelInfo.bNeedToBeReleased)
		{
			GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
				"Release unfinish model %s" , pkModel->GetResourceName().c_str() );
			ReleaseModel(pkModel);
			return;
		}
	}
	else
	{
		T_ASSERT(false);
	}
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::_modelPostInitCallback(GD_ModelBase* pkModel, void* pvCallbackData)
{
	GD_ClientAsyncModelManager::GetSingleton()._onModelInited(pkModel);
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::_modelLoadFailedCallback(Data::GD_ModelBase* pkModel, void* pvCallbackData)
{
	GD_ClientAsyncModelManager::GetSingleton()._onModelInited(pkModel, true);
}
//-----------------------------------------------------------------------------------
// DiscardedPool
//-----------------------------------------------------------------------------------

bool GD_ClientAsyncModelManager::IsDiscardedPoolModel(Data::GD_ModelBase* pkModel)
{
	ModelList::iterator iter = find(m_kPendingModelsFromDiscardedPool.begin(), 
								m_kPendingModelsFromDiscardedPool.end(), pkModel);

	return (iter != m_kPendingModelsFromDiscardedPool.end());
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::SetDiscardedPoolCapcityScale(float fScale)
{
	m_fDiscardedPoolCapcitiesScale = fScale;
}
//-----------------------------------------------------------------------------------
int GD_ClientAsyncModelManager::GetDiscardedPoolCapcity( ASYNC_MODEL_TYPE modelType ) const
{
	return (int)((float)m_anDiscardedModelCapacities[modelType] * m_fDiscardedPoolCapcitiesScale);
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::SetDiscardedPoolClearIntensityScale(float fIntensityScale)
{
	m_fDiscardedPoolAutoClearIntensityScale = fIntensityScale;
}
//-----------------------------------------------------------------------------------
GD_ModelBase* GD_ClientAsyncModelManager::_getDiscardedPoolModel( ASYNC_MODEL_TYPE eModelType, 
										   const StringType& strModFileName, int iModFileIndex, unsigned int uiShaderIdentifier )
{
	GD_ModelBase* pkPooledModel = NULL;

	if (!m_kDiscardedModels[eModelType].empty())
	{
		ModelList::iterator iter = m_kDiscardedModels[eModelType].begin(); 
		ModelList::iterator iend = m_kDiscardedModels[eModelType].end();

		while( iter != iend )
		{
			pkPooledModel = *iter;

			if ( pkPooledModel->GetModFileName() == strModFileName && 
				 pkPooledModel->GetCurrentModelContentIndex() == iModFileIndex &&
				 pkPooledModel->GetShaderIdentifier() == uiShaderIdentifier )
			{
				m_kDiscardedModels[eModelType].erase(iter);
				pkPooledModel->ReUse();
				break;
			}
			pkPooledModel = NULL;
			++iter;
		}
	}

	return pkPooledModel;
}
//-----------------------------------------------------------------------------------
bool GD_ClientAsyncModelManager::_addDiscardedPoolModel( ASYNC_MODEL_TYPE eModelType, Data::GD_ModelBase* pkModel, bool bIsForce/* = false */)
{
	if ( !bIsForce )
	{
		if ( !pkModel || !pkModel->IsReusable() )
			return false;

		if ( GetDiscardedPoolCapcity(eModelType) == 0 )
			return false;
	}

	m_kDiscardedModels[eModelType].push_front(pkModel);
	_removeDiscardedPoolOverloadedModels(eModelType);

	return true;
}

//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::_removeDiscardedPoolOverloadedModels(ASYNC_MODEL_TYPE eModelType)
{
	int iPoolCapcity = GetDiscardedPoolCapcity(eModelType);
	int iOverLoadedCount = m_kDiscardedModels[eModelType].size() - iPoolCapcity;

	if ( iOverLoadedCount > 0 )
		_removeDiscardedPoolModels(eModelType, iOverLoadedCount);
}
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::_removeDiscardedPoolModels( ASYNC_MODEL_TYPE eModelType, int iReleaseCount )
{
	int iCurrentPoolSize = m_kDiscardedModels[eModelType].size();
	if ( iCurrentPoolSize == 0 )
		return;

	int iFinalReleaseCount = iReleaseCount;
	if ( iFinalReleaseCount < 0 )
		iFinalReleaseCount = 0;
	if ( iFinalReleaseCount > iCurrentPoolSize )
		iFinalReleaseCount = iCurrentPoolSize;

	bool bReleaseOk = false;
	int iRelaseRepeat = 10;
	GD_ModelBase* pkModel = NULL;

	// 从尾部开始清空
	ModelList::iterator revIEnd = m_kDiscardedModels[eModelType].begin();
	ModelList::iterator revIter = m_kDiscardedModels[eModelType].end();
	--revIter;
	
	while ( iFinalReleaseCount > 0 )
	{
		pkModel = (*revIter);

		// 采用这种方法是为了处理_rleaseModel失败的情况；
		// 防止出现内存泄漏或者是无限循环;
		bReleaseOk = false;
		iRelaseRepeat = 10;
		while ( iRelaseRepeat > 0 )
		{
			if ( _releaseModel(pkModel) )
			{
				bReleaseOk = true;
				break;
			}
			--iRelaseRepeat;
		}

		if ( bReleaseOk )
		{
			// 释放成功，减少Release计数
			--iFinalReleaseCount;

			if ( revIter == revIEnd )
			{
				// 如果是最后一个iter直接清空并且退出：
				m_kDiscardedModels[eModelType].clear();
				break;
			}
			else
			{
				// 跳到前面一个迭代器，并把原来的迭代器删除
				m_kDiscardedModels[eModelType].erase(revIter--);
			}
		}
		else
		{
			// 释放失败，如果是最后一个iter直接退出
			if ( revIter == revIEnd )
				break;

			// 释放失败，继续尝试释放更上面一个iter：
			--revIter;
		}
	}
}

//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::_updateDiscardedPoolModels( float fDeltaTime )
{
	float fDiscardedPoolAutoClearTimeInterval = 
		m_fDiscardedPoolAutoClearTimeInterval * m_fDiscardedPoolAutoClearTimeIntervalScale;

	int iDiscaredPoolAutoClearIntensity = 
		int((float)m_iDiscardedPoolAutoClearIntensity * m_fDiscardedPoolAutoClearIntensityScale);
	if ( iDiscaredPoolAutoClearIntensity < 1 )
		iDiscaredPoolAutoClearIntensity = 1;

	m_fDiscardedPoolAutoClearTimeElapsed += fDeltaTime;

	if ( m_fDiscardedPoolAutoClearTimeElapsed  > m_fDiscardedPoolAutoClearTimeInterval )
	{
		m_fDiscardedPoolAutoClearTimeElapsed = 0.0f;	

		for ( int i = (int)AMT_INVALID + 1; i < (int)AMT_MAX_COUNT; ++i )
		{
			_removeDiscardedPoolModels((ASYNC_MODEL_TYPE)i, iDiscaredPoolAutoClearIntensity );
		}
	}
}


//-----------------------------------------------------------------------------------
// Update & Thread
//-----------------------------------------------------------------------------------
void GD_ClientAsyncModelManager::SetActorUpdateThread(unsigned int uiCount)
{
    if (m_pkUpdatePool != NULL)
    {
        m_pkUpdatePool->SetThreadsCount(uiCount);
    }
}
//-----------------------------------------------------------------------------------
// Utility
//-----------------------------------------------------------------------------------
unsigned int GD_ClientAsyncModelManager::GetCurrentPlayerModelCreationCapacity(void)
{
	return BL_AsyncOperationManager::GetSingleton().GetCurrentPlayerModelCreationCapacity();
}
//***********************************************************************************