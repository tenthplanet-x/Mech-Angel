//***********************************************************************************
#pragma once
//***********************************************************************************
#include "GD_ModelBase.h"
#include "MG_MessageHandlerInterface.h"
//***********************************************************************************
class UT_UpdateThreadPool;
class UT_ModelUpdateTaskQueue;
//***********************************************************************************
// GD_ClientAsyncModelManager
// @ 客户端异步模型管理器
//***********************************************************************************
class GD_ClientAsyncModelManager : public Memory::MM_BaseObject, 
                                   public MG_MessageHandlerInterface,
                                   public Utility::UT_Singleton<GD_ClientAsyncModelManager>
{

	

public:

	//=======================
	// 异步类型定义
	//=======================
    enum ASYNC_MODEL_TYPE {
        AMT_INVALID = -1,

        AMT_SCENE,
        AMT_PLAYER,
        AMT_ITEM,
        AMT_NPC,
        AMT_CHEST,
        AMT_LOGIN,
        AMT_LOGIN_CAMERA_ANIM,
        AMT_LOGIN_WEAPON_TOTEM,
		AMT_NPC_DEAD,

        AMT_MAX_COUNT
    };


	//=======================
	// 模型信息
	//=======================
	struct LoadingModelInfo
	{
		int					iModelIndex;

		bool				bManualUpdate;
		bool				bRenderToUITexture;
		bool				bRenderUltraMode;
		unsigned char		ucRenderUltraModeIndex;
		
		LoadingModelInfo() : iModelIndex(0)
				,bManualUpdate(false)
				,bRenderToUITexture(false)
				,bRenderUltraMode(false)
				,ucRenderUltraModeIndex(0)
		{}

	};

	//=======================
	// 模型异步信息
	//=======================
    struct PendingModelInfo 
    {
        ASYNC_MODEL_TYPE    eType;
        StringType          strModFileName;     // MOD 文件名
        bool                bNeedToBeReleased;  // 创建过程中就已经请求释放了

        PendingModelInfo(void)
            : eType(AMT_INVALID)
            , bNeedToBeReleased(false)
        {
        }

        PendingModelInfo(ASYNC_MODEL_TYPE eTypeParam, const StringType& strModFileNameParam, bool bNeedToBeReleasedParam = false)
            : eType(eTypeParam)
            , strModFileName(strModFileNameParam)
            , bNeedToBeReleased(bNeedToBeReleasedParam)
        {
        }
    };

	//=======================
	// 类型定义
	//=======================
	typedef mem::set<Data::GD_ModelBase*>                   ModelSet;			// 模型集合
	typedef mem::list<Data::GD_ModelBase*>                  ModelList;			// 模型列表
	typedef mem::map<Data::GD_ModelBase*, ASYNC_MODEL_TYPE> ModelTypeMap;		// 模型对象指针 => 模型异步类型
	typedef mem::map<Data::GD_ModelBase*, PendingModelInfo> PendingModelMap;	// 模型对象指针 => 模型加载信息

public:

    GD_ClientAsyncModelManager(void);
	virtual ~GD_ClientAsyncModelManager(void);


//**********************************************************
// MG_Message
//**********************************************************
public:

	 /*virtual*/bool RecvMessage(MG_MessageBase& rkMessage);

private:

	void _eventTimerTick(MG_MessageBase& rkMessage);

//**********************************************************
// Creation & Release 
//**********************************************************
public:

	Data::GD_ModelBase* CreateModel(ASYNC_MODEL_TYPE eModelType, 
									const StringType& strModFileName, 
									int iModelIndex = 0, bool bAsyncLoad = true );

	Data::GD_ModelBase* CreateModel(ASYNC_MODEL_TYPE eModelType, 
									const StringType& strModFileName, 
									const LoadingModelInfo& kModelInfo, bool bAsyncLoad = true );

	void ReleaseModel(Data::GD_ModelBase* pkModel);

	bool IsAsyncModel(Data::GD_ModelBase* pkModel);

private:

	Data::GD_ModelBase* _doNewAsyncModel( ASYNC_MODEL_TYPE eModelType, 
										const StringType& strModFileName, 
										int iModFileIndex, unsigned int uiShaderIdentifier,
										bool& bIsModelInited );

	bool _releaseModel(Data::GD_ModelBase* pkModel);

	void _onModelInited(Data::GD_ModelBase* pkModel, bool bLoadFailed = false);

	static void _modelPostInitCallback(Data::GD_ModelBase* pkModel, void* pvCallbackData);
	static void _modelLoadFailedCallback(Data::GD_ModelBase* pkModel, void* pvCallbackData);

//**********************************************************
// DicardedPool
//**********************************************************
public:

	bool IsDiscardedPoolModel(Data::GD_ModelBase* pkModel);

	void SetDiscardedPoolCapcityScale(float fTimeScale);
	int GetDiscardedPoolCapcity( ASYNC_MODEL_TYPE modelType ) const;

	void SetDiscardedPoolClearIntensityScale(float fIntensityScale);

private:

	Data::GD_ModelBase* _getDiscardedPoolModel( ASYNC_MODEL_TYPE eModelType, 
												const StringType& strModFileName, 
												int iModFileIndex, unsigned int uiShaderIdentifer );

	bool _addDiscardedPoolModel( ASYNC_MODEL_TYPE eModelType, Data::GD_ModelBase* pkModel, bool bIsForce = false );

	void _removeDiscardedPoolOverloadedModels( ASYNC_MODEL_TYPE eModelType );
	void _removeDiscardedPoolModels( ASYNC_MODEL_TYPE eModelType, int iReleaseCount );

	void _updateDiscardedPoolModels( float fDeltaTime );

//**********************************************************
// Update & Thread
//**********************************************************
public:

	void SetActorUpdateThread(unsigned int uiCount);

	inline void SetModelsUpdatable(bool bEnable) { m_bUpdateModels = bEnable; }

//**********************************************************
// Utility
//**********************************************************
public:

	unsigned int GetCurrentPlayerModelCreationCapacity(void);
	
//**********************************************************
// Members
//**********************************************************
private:

	//@ 总表
	ModelTypeMap                m_kModels;

	//@ 正在加载的模型信息 
    PendingModelMap             m_kPendingModels;
    ModelList                   m_kPendingModelsFromDiscardedPool;

	//@ 缓存模型池  
	float						m_fDiscardedPoolCapcitiesScale;

	float						m_fDiscardedPoolAutoClearTimeInterval;
	float						m_fDiscardedPoolAutoClearTimeIntervalScale;
	float						m_fDiscardedPoolAutoClearTimeElapsed;
	int							m_iDiscardedPoolAutoClearIntensity;
	float						m_fDiscardedPoolAutoClearIntensityScale;

	ModelList                   m_kDiscardedModels[AMT_MAX_COUNT];
    unsigned int                m_anDiscardedModelCapacities[AMT_MAX_COUNT];

	//@ 更新队列
	bool                        m_bUpdateModels;

	UT_UpdateThreadPool*        m_pkUpdatePool;
	UT_ModelUpdateTaskQueue*    m_pkUpdateTaskQueue;
	
	ModelSet                    m_kDynamicModels[AMT_MAX_COUNT];
};
//***********************************************************************************
