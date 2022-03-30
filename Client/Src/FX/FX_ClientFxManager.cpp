#include "ClientPCH.h"
#include "FX_Composite.h"
#include "FX_ClientFxManager.h"
#include "MG_MessageBase.h"
#include "FX_ModelElmt.h"
#include "MG_MessageCenter.h"
#include "CA_CameraManager.h"
#include "SE_SceneManager.h"
#include "AS_ClientSoundManager.h"
#include "UT_GlobalSettings.h"
#include "IM_Manager.h"

using namespace FX;
using namespace Render;
using namespace Utility;

#define DISCARDED_FX_OBJECT_CLEAN_INTERVAL  2.0f
#define DISCARDED_FX_OBJECT_TIMEOUT         15.0f

FX_ClientFxManager::FX_ClientFxManager(void)
: m_bRequestCleanDiscardedFxObjects(false)
, m_fLastDiscardedFxObjectsCleanTime(60.0f)
, m_nDiscardFxCount(0)
{
    _setHeightFieldGridSizeFunction(_getHeightFieldGridSize);
    _setGetHeightFieldHeightFunction(_getHeightFieldHeight);
    _setGetWorldNormalFunction(_getWorldHeightAndNormal);
	_setPlaySoundExFunction(_play3DSoundEx, _play2DSoundEx, _stopSoundEx);


	gfxReplace.insert(std::pair<StringType,StringType>("W_弹出提示_降龙十八掌.gfx","W_弹出提示_降龙掌.gfx"));
	gfxReplace.insert(std::pair<StringType,StringType>("W_弹出提示_擒龙功.gfx","W_弹出提示_龙爪手.gfx"));
	gfxReplace.insert(std::pair<StringType,StringType>("W_弹出提示_一阳指.gfx","W_弹出提示_少阳指.gfx"));
	gfxReplace.insert(std::pair<StringType,StringType>("W_弹出提示_梯云纵.gfx","W_弹出提示_梯云功.gfx"));
	gfxReplace.insert(std::pair<StringType,StringType>("W_弹出提示_六脉神剑.gfx","W_弹出提示_连锁闪电.gfx"));
}

FX_ClientFxManager::~FX_ClientFxManager(void)
{
}

bool FX_ClientFxManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
        MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
#ifdef ENABLE_CLEAR_FX_POOL_MANUALLY
        MESSAGE_HANDLE(MGT_INPUT_KEY_NUMPAD_MUL, _eventClearFxPool);
#endif // ENABLE_CLEAR_FX_POOL_MANUALLY
	END_MESSAGE_HANDLE

	return true;
}

void FX_ClientFxManager::GetFxModelVisibleArray(NiGeometryList& rkCasterList)
{
    for (FX_ElementRenderingSet::iterator itr = m_akShadowedModelElmtRenderingSet.begin(); 
        itr != m_akShadowedModelElmtRenderingSet.end(); ++itr)
    {
        FX_ModelElmt* pkModelElmt = (FX_ModelElmt*)(*itr);
        NiGeometry* pkFxModelGeometry = pkModelElmt->GetShadowedModelGeometry();
        if (pkFxModelGeometry != NULL)
        {
            rkCasterList.AddTail(pkFxModelGeometry);
        }
    }
}

void FX_ClientFxManager::ClearDischarGfx()
{
	_doUnInit();
}

void FX_ClientFxManager::_doInit(void)
{
    SetCurrentGrade((RenderGrade)g_settings.GetIntSetting(UT_GlobalSettings::GS_GFX_QUALITY));
}

void FX_ClientFxManager::_doUnInit(void)
{
    for (DiscardedFxObjectPool::iterator itr = m_kDiscardedFxObjects.begin(); itr != m_kDiscardedFxObjects.end(); ++itr)
    {
        FxObjectPool & fxl = itr->second;
        for (FxObjectPool::iterator jt = fxl.begin(); jt != fxl.end(); ++jt)
        {
            FxObject* pkFxObject = (*jt);

            T_SAFE_DELETE(pkFxObject->pkComposite);
            T_DELETE_E( pkFxObject, FxObject , Memory::MEMCATEGORY_GENERAL );
        }
        fxl.clear();
    }
    m_kDiscardedFxObjects.clear();
	m_nDiscardFxCount = 0;
}

FX_FxManager::FxObject* FX_ClientFxManager::_doGetNewFxObject(const StringType& strFxFileName)
{
    DiscardedFxObjectPool::iterator itr = m_kDiscardedFxObjects.find(strFxFileName);
    // 如果已有之前回收的特效对象，则复用
    if (itr != m_kDiscardedFxObjects.end())
    {
        FxObjectPool & fxl = itr->second;
        if (!fxl.empty())
        {
            --m_nDiscardFxCount;
            FxObject* pkFxObject = fxl.front();
            fxl.pop_front();

            pkFxObject->Reset();

			//我也不想这样做，但是因为FxObject在绑定到模型上的时候，模型的AttachedMap中会保存这个FxObject的ID，
			//但是这个特效可能因为播放结束自动删除，进入池子，
			//模型的AttachedMap却不会清除，仍然保存这个ID，万一这个ID的特效又被拿出在用在别的地方，这个模型又操作了这个ID的特效，就会出现问题
			pkFxObject->iID = (++FX_FxManager::sm_iFxObjectID);

			GfxWriteLog(LOG_FX_FILELINE, LOG_SWITCH_FX, 
				"Reuse FX (%d, 0x%08p, 0x%08p) from DiscardPool at FX_ClientFxManager::_doGetNewFxObject [%s]!", 
				pkFxObject->iID, pkFxObject, pkFxObject->pkComposite, pkFxObject->strFileName.c_str());

            return pkFxObject;
        }
    }
    
    // 否则创建新的特效对象
	FxObject* pkFxObject = T_NEW_E( FxObject , Memory::MEMCATEGORY_GENERAL );
    pkFxObject->strFileName = strFxFileName;
	pkFxObject->iID = (++FX_FxManager::sm_iFxObjectID);

	GfxWriteLog(LOG_FX_FILELINE, LOG_SWITCH_FX, 
		"Create New FX (%d, 0x%08p, 0x%08p) at FX_ClientFxManager::_doGetNewFxObject [%s]!", 
		pkFxObject->iID, pkFxObject, pkFxObject->pkComposite, pkFxObject->strFileName.c_str());

    return pkFxObject;
}

void FX_ClientFxManager::_doInitCompositeInFxObject(FxObject& rkFxObject)
{
    SendMessage(CreateMsg_ADD_GFX_TASK(rkFxObject));
}

void FX_ClientFxManager::_doReleaseFx(FxObject* pkFxObject)
{
	if ( pkFxObject->IsLoaded() || 
		pkFxObject->IsLoading() )
	{
		// 回收当前特效
		DiscardedFxObjectPool::iterator itr = m_kDiscardedFxObjects.find(pkFxObject->strFileName);
		if (itr != m_kDiscardedFxObjects.end())
		{
			FxObjectPool & fxl = itr->second;
			fxl.push_front(pkFxObject);
		}
		else
		{
			FxObjectPool	fxl;
			fxl.push_front(pkFxObject);
			m_kDiscardedFxObjects.insert(make_pair(pkFxObject->strFileName, fxl));
		}
		++m_nDiscardFxCount;

		GfxWriteLog(LOG_FX_FILELINE, LOG_SWITCH_FX, 
			"Recycle FX (%d, 0x%08p, 0x%08p) at FX_ClientFxManager::_doReleaseFx [%s]!", pkFxObject->iID, pkFxObject, pkFxObject->pkComposite, pkFxObject->strFileName.c_str());

		m_bRequestCleanDiscardedFxObjects = (m_kDiscardedFxObjects.size() > g_settings.LOD_FX_POOL_CAPACITY);
	}
	else
	{
		GfxWriteLog(LOG_FX_FILELINE, LOG_SWITCH_FX, 
			"UnloadMsg FX (%d, 0x%08p, 0x%08p) at FX_ClientFxManager::_doReleaseFx [%s]!", 
			pkFxObject->iID, pkFxObject, pkFxObject->pkComposite, pkFxObject->strFileName.c_str());

		// 其他的直接干掉
		SendMessage(CreateMsg_ADD_GFX_UNLOADING_TASK(*pkFxObject));
	}
  
}

void FX_ClientFxManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
	T_PROFILE("Update::FX_ClientFxManager");

    float fCurrentTime = rkMessage.m_fParam1;

    Update(fCurrentTime);

    // 定时清理回收池里已过期的特效
	m_bRequestCleanDiscardedFxObjects = true;
    if (m_bRequestCleanDiscardedFxObjects 
        && fCurrentTime - m_fLastDiscardedFxObjectsCleanTime > DISCARDED_FX_OBJECT_CLEAN_INTERVAL)
    {
        int  nCleaned = 0;

        for (DiscardedFxObjectPool::iterator itr = m_kDiscardedFxObjects.begin(); itr != m_kDiscardedFxObjects.end(); )
        {
            FxObjectPool & fxl = itr->second;
            for (FxObjectPool::iterator jt = fxl.begin(); jt != fxl.end(); )
            {
				FxObject* pkFxObject = (*jt);

				if (fCurrentTime - pkFxObject->fLastUpdateTimeInGlobal > DISCARDED_FX_OBJECT_TIMEOUT)
				{
					GfxWriteLog(LOG_FX_FILELINE, LOG_SWITCH_FX, 
						"UnloadMsg FX OnClean (%d, 0x%08p, 0x%08p) at FX_ClientFxManager::_eventTimerTick, %f [%s]!", 
						pkFxObject->iID, pkFxObject, pkFxObject->pkComposite, fCurrentTime, pkFxObject->strFileName.c_str());

					SendMessage(CreateMsg_ADD_GFX_UNLOADING_TASK(*pkFxObject));
					jt = fxl.erase(jt);
					--m_nDiscardFxCount;

					nCleaned++;
					//< 是否要限定一次性处理的数量
				}
				else
				{
					++jt;
				}
            }

			if (fxl.empty())	//< 列表空
				itr = m_kDiscardedFxObjects.erase(itr);
			else
				++itr;
        }

        if (!nCleaned)
        {
            m_bRequestCleanDiscardedFxObjects = false;
            m_fLastDiscardedFxObjectsCleanTime = fCurrentTime;
        }

		GfxWriteLog(LOG_FX_FILELINE, LOG_SWITCH_FX, 
			"Discarded FX count=%d, rest=%d, active=%d", nCleaned, m_nDiscardFxCount, this->GetFxCount());
    }

	NIMETRICS_APPLICATION_AGGREGATEVALUE(FX_POOLED_OBJ_COUNT, m_kDiscardedFxObjects.size());
}

#ifdef ENABLE_CLEAR_FX_POOL_MANUALLY
void FX_ClientFxManager::_eventClearFxPool(MG_MessageBase& rkMessage)
{
    for (DiscardedFxObjectPool::iterator itr = m_kDiscardedFxObjects.begin(); itr != m_kDiscardedFxObjects.end(); ++itr)
    {
        FxObject* pkFxObject = (*itr).second;

        SendMessage(CreateMsg_ADD_GFX_UNLOADING_TASK(*pkFxObject));
    }
    m_kDiscardedFxObjects.clear();
}
#endif // ENABLE_CLEAR_FX_POOL_MANUALLY

float FX_ClientFxManager::_getHeightFieldGridSize(void)
{
    return GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
}

float FX_ClientFxManager::_getHeightFieldHeight(float fHeightFieldCoordX, float fHeightFieldCoordY)
{
    return SE_SceneManager::GetSingleton().GetHeightFieldHeight(fHeightFieldCoordX, fHeightFieldCoordY);
}

StringType FX_ClientFxManager::findPlatformReplace(const StringType& str)
{
// 	if ( YYP_TX == getYunYingplatform() ) 
	{
		mem::map<StringType, StringType>::iterator fd = gfxReplace.find(str);
		if ( gfxReplace.end() != fd )
		{
			return fd->second;
		}
		else
		{
			return str;
		}
	}
// 	else
// 	{
// 		return str;
// 	}
}

void FX_ClientFxManager::RegisterGfxPairActor(logic_id_type lGfxID , logic_id_type lActorID, logic_id_type lTargetID)
{
	if ( lGfxID > 0 )
		m_kPairGfxInfo.insert(pair<int, ActorPair>(lGfxID, ActorPair(lActorID, lTargetID)));
}

void FX_ClientFxManager::NotifyModelReleasePair(logic_id_type lGfxID)
{
	PairActorGfxInfo::iterator fd = m_kPairGfxInfo.find(lGfxID);
	if ( fd != m_kPairGfxInfo.end() )
	{
		ActorPair& tmp = fd->second;
		GD_ClientActorModel* model = IM_Manager::GetSingleton().GetActor(tmp.lActorID);
		if ( model )
		{
			model->RemovePairGfxID(lGfxID);
		}
		model = IM_Manager::GetSingleton().GetActor(tmp.lTargetID);
		if ( model )
		{
			model->RemovePairGfxID(lGfxID);
		}
		m_kPairGfxInfo.erase(lGfxID);
	}
}

void FX_ClientFxManager::NotifyGfxPairRelease(logic_id_type lGfxID)
{																						    
	FxObject* pkFxObject = _getFxObjectByID(lGfxID);
	if (pkFxObject && pkFxObject->IsLoaded() && pkFxObject->pkComposite)
	{
		pkFxObject->pkComposite->DisableExternTranform();
	}
	else if ( pkFxObject )
	{
		pkFxObject->kPendingSettings.pkWorldTransform = NULL;
		pkFxObject->kPendingSettings.pkTargetWorldLocation = NULL;
	}
	m_kPairGfxInfo.erase(lGfxID);
}

bool FX_ClientFxManager::_getWorldHeightAndNormal(float fWorldLocationX, float fWorldLocationY, float& rfWorldHeight, NiPoint3& rkWorldNormal)
{
    const SE_SceneTerrainLocalInfo& kTerrainLocalInfo = SE_SceneManager::GetSingleton().GetWalkableInfo(
        fWorldLocationX, fWorldLocationY, true, true);

    rfWorldHeight = kTerrainLocalInfo._fLocalHeight;
    rkWorldNormal = kTerrainLocalInfo._kLocalNormal;
    return true;
}

int FX_ClientFxManager::_play3DSoundEx(const NiPoint3& kPos, const StringType& strKeyName, int eChannelType, bool bFadeToGain, bool bLoop, float fControlVolume)
{
	int nSoundID = g_soundMgr.Play3DEx(kPos, strKeyName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)eChannelType, bFadeToGain, bLoop, fControlVolume);
	return nSoundID;
}

int FX_ClientFxManager::_play2DSoundEx(const StringType& strKeyName, int eChannelType, bool bFadeToGain, bool bLoop)
{
	int nSoundID = g_soundMgr.Play2DEx(strKeyName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)eChannelType, bFadeToGain, bLoop ? 0 : 1);
	return nSoundID;
}

void FX_ClientFxManager::_stopSoundEx(int nSoundID, bool bFade, float fFadeTime)
{
	if (bFade)
		g_soundMgr.StopAudioSourceFade(nSoundID, fFadeTime);
	else
		g_soundMgr.StopAudioSource(nSoundID);
}
