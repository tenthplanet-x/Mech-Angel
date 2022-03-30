#pragma once

#include "FX_FxManager.h"
#include "MG_MessageHandlerInterface.h"

//#define ENABLE_CLEAR_FX_POOL_MANUALLY

class FX_ClientFxManager : public FX::FX_FxManager
                         , public MG_MessageHandlerInterface
{
protected:
	typedef mem::list<FX::FX_FxManager::FxObject*>	FxObjectPool;
    typedef mem::map<StringType, FxObjectPool>		DiscardedFxObjectPool;
	//< 修改DiscardedFx列表的管理方式，使得回收的fx对象塞到列表前面，这样列表后面的fx对象更少的被使用、更容易满足LRU条件
	//<	原先的管理方式中，回收列表中的所有fx都平均的被弹出到active list，导致它们都不容易满足LRU，fx对象几乎不被删除

public:
    static FX_ClientFxManager& GetSingleton(void)
    {
        return *(static_cast<FX_ClientFxManager*>(FX_FxManager::GetSingletonPtr()));
    }
    static FX_ClientFxManager* GetSingletonPtr(void)
    {
        return static_cast<FX_ClientFxManager*>(FX_FxManager::GetSingletonPtr());
    }

    FX_ClientFxManager(void);
	virtual ~FX_ClientFxManager(void);

    virtual bool RecvMessage(MG_MessageBase& rkMessage);

    void GetFxModelVisibleArray(NiGeometryList& rkCasterList);

	void ClearDischarGfx();

	StringType findPlatformReplace(const StringType& str);

	virtual void RegisterGfxPairActor(logic_id_type lGfxID , logic_id_type lActorID, logic_id_type lTargetID);
	virtual void NotifyGfxPairRelease(logic_id_type lGfxID);
	virtual void NotifyModelReleasePair(logic_id_type lGfxID);

	static float _getHeightFieldHeight(float fHeightFieldCoordX, float fHeightFieldCoordY);
	
private:
    virtual void _doInit(void);
    virtual void _doUnInit(void);

    virtual FxObject* _doGetNewFxObject(const StringType& strFxFileName);
    virtual void _doInitCompositeInFxObject(FxObject& rkFxObject);

    virtual void _doReleaseFx(FxObject* pkFxObject);

    void _eventTimerTick(MG_MessageBase& rkMessage);
#ifdef ENABLE_CLEAR_FX_POOL_MANUALLY
    void _eventClearFxPool(MG_MessageBase& rkMessage);
#endif // ENABLE_CLEAR_FX_POOL_MANUALLY

    static float _getHeightFieldGridSize(void);

    static bool _getWorldHeightAndNormal(float fWorldLocationX, float fWorldLocationY, float& rfWorldHeight, NiPoint3& rkWorldNormal);
	static int  _play3DSoundEx(const NiPoint3& kPos, const StringType& strKeyName, int eChannelType, bool bFadeToGain, bool bLoop, float fControlVolume);
	static int  _play2DSoundEx(const StringType& strKeyName, int eChannelType, bool bFadeToGain, bool bLoop);
	static void _stopSoundEx(int nSoundID, bool bFade, float fFadeTime);

private:
    DiscardedFxObjectPool   m_kDiscardedFxObjects;      // 特效对象回收池
	int						m_nDiscardFxCount;	// 调试用
    bool                    m_bRequestCleanDiscardedFxObjects;
    float                   m_fLastDiscardedFxObjectsCleanTime;

	mem::map<StringType, StringType> gfxReplace;

	struct ActorPair
	{
		ActorPair()
		{
			lActorID = 0;
			lTargetID = 0;
		}
		ActorPair(logic_id_type _lActorID, logic_id_type _lTargetID)
		{
			lActorID = _lActorID;
			lTargetID = _lTargetID;
		}
		logic_id_type lActorID;
		logic_id_type lTargetID;
	};

	typedef mem::map<logic_id_type,ActorPair> PairActorGfxInfo;
	PairActorGfxInfo m_kPairGfxInfo;
};