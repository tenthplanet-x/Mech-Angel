#include "ClientPCH.h"
#include "AS_ClientSoundManager.h"
#include "BL_AsyncOperationManager.h"
#include "CF_Buddy.h"
#include "CF_CharType.h"
#include "GD_ClientActorModel.h"
#include "LC_ClientLogicManager.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "MTF_Model.h"
#include "SE_SceneManager.h"
#include "UT_Win32Helper.h"
#include "RG_LightProbe.h"
#include "UI_UIManager.h"
#include "LC_CLientNPCManager.h"
#include "CA_CameraManager.h"
#include "CF_CameraCut.h"
#include "LC_ClientHelp.h"
#include "CF_Buddy.h"

using namespace CSVFile;
using namespace Data;
using namespace FileSystem;
using namespace GameLogic;
using namespace Utility;
using namespace Render;

const char* GD_ClientActorModel::IDLE_ANIM_NAMES[IDLE_ANIM_COUNT] = { 
    RES_ANM_ACTOR_IDLE0, 
    RES_ANM_ACTOR_IDLE1, 
    RES_ANM_ACTOR_IDLE2, 
    RES_ANM_ACTOR_IDLE3,
};

// 当 alpha 低于一定值时隐藏阴影的阈值
const float ALPHA_THRESHOLD_TO_HIDE_SHADOW = 0.2f;

GD_ClientActorModel::GD_ClientActorModel(void)
: m_lCharType(0)

, m_bNeedToRecalcPitching(false)
, m_bFirstPitching(true)
, m_fLastPitchingAngle(0.0f)

, m_bIsPlayer(false)

, m_fLastAlpha(GetAlpha())

, m_bHideBeforeSuitApplied(false)
, m_bAllClothingChanged(false)
, m_bVisibilityWhenHideBeforeSuitApplied(IsVisible())
, m_bRecordVisibilityWhenHideBeforeSuitApplied(true)

, m_bFakeShadow(true)
, m_eCurrentFakeShadowDummyNode(FDT_GROUND)
, m_kFakeShadowColor(0.1f, 0.1f, 0.1f)

, m_eCurrentAnimType(AT_SINGLE_ANIM)
, m_bAnimTypeChanged(false)
, m_eWeaponType(WT_NONE)
, m_iCurrIdleAnimIndex(-1)
, m_nCurrIdleAnimRemain(0)
, m_iCurrDisplayAnimIndex(0)
#ifdef _ENABLE_PHYSX
 , m_bPhysXInited(false)
 , m_bHasAddedPhyXPropToSceneMgr(false)
#endif 
, m_bIsActiveNormalDead(false)
, m_fNormalDeadStartTime(0.0f)
, m_fNormalDeadLastTime(0.0f)
#ifdef _ENABLE_PHYSX
, m_bIsActiveRagDoll(false)
, m_fRagDollStartTime(0.0f)
, m_fRagDollLastTime(0.0f)
, m_fForceStartTime(0.0f)
, m_fForceApplyTime(0.0f)
, m_bForceApply(false)
#endif
, m_bAreaEffectEnabled(false)
, m_eLastAreaEffectType(MTF_EffectTile::AET_UNKNOWN)
, m_fWaterAreaEffectHeight(-10000.0f)
, m_iCurrentAreaEffectFxId(-1)
, m_iCurrentAreaEffectSoundId(0)

, m_bAffectPointLight(false)
, m_spNiPointLight(NULL)

, m_spPickRootExtraData(NULL)

, m_pkVehicleModel(NULL)
, m_bIsVehicle(false)

, m_clietNoCull(false)
, m_bPitchByTerrain(true)
, m_fVehicleHeight(0.0f)
, m_fStepSoundTick(0.0f)
{
    m_kPitchingRotation.MakeIdentity();

    for (int i = 0; i < IDLE_ANIM_COUNT; ++i)
    {
        m_anIdleAnimRepeatMinNums[i] = 1;
        m_anIdleAnimRepeatMaxNums[i] = 1;
    }
}

GD_ClientActorModel::~GD_ClientActorModel(void)
{
}

void GD_ClientActorModel::ReUse()
{
	GD_ActorModel::ReUse();

	m_pkVehicleModel = NULL;
	m_clietNoCull = false;

	m_iCurrDisplayAnimIndex = 0;

	m_eCurrentAnimType = AT_SINGLE_ANIM;

	m_kClientActorPendingSettings.clear();

	DeactiveNormalDead();
}

void GD_ClientActorModel::HideBeforeSuitApplied(void)
{
    if (!m_bClothingChangeable)
    {
        //GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, 
        //    "Show after suit applied but the actor is not clothing-changeable! [%s]", GetModFileName().c_str());
        return;
    }

    GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
        "Reqest for hiding the actor model [%s]", GetModelFileName().c_str());

    m_bHideBeforeSuitApplied = true;
    m_bAllClothingChanged = false;
}

void GD_ClientActorModel::ShowAfterSuitApplied(void)
{
    if (!m_bClothingChangeable)
    {
        //GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, 
        //    "Show after suit applied but the actor is not clothing-changeable! [%s]", GetModFileName().c_str());
        return;
    }

    if (m_bHideBeforeSuitApplied)
    {
        GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
            "Request for showing the actor model [%s]", GetModelFileName().c_str());

        m_bAllClothingChanged = true;

        if ( _isInitedWithLoaded() && !_hasClothingModelInLoading() )
        {
			if ( m_bClothingChanged )
            {
                _doApplyNewSuit();
            }
            else
            {
                _onNewSuitApplied();
            }
        }
    }
}

NiPoint3 GD_ClientActorModel::GetHeadPosition(void) const
{
    if (m_aspFxDummyNiNode[FDT_HEAD] == NULL)
    {
        if (m_spNiRootNode == NULL)
        {
            return NiPoint3::ZERO;
        }

        return m_spNiRootNode->GetWorldTranslate();
    }

    return m_aspFxDummyNiNode[FDT_HEAD]->GetWorldTranslate();
}

NiNodePtr GD_ClientActorModel::GetFxDummyNode(FxDummyNodeType kType) const
{
	if (m_aspFxDummyNiNode[kType] == NULL)
	{
		if (m_spNiRootNode == NULL)
		{
			return NULL;
		}
		return m_spNiRootNode;
	}
	return m_aspFxDummyNiNode[kType];
}

void GD_ClientActorModel::PlayRandomIdleAnimation(void)
{
    if (_isInitedWithLoaded())
    {
        if (m_eCurrentAnimType == AT_IDLE_ANIM_GROUP)
        {
            return;
        }

        _changeToNextIdleAnimation();
    }
    else
    {
        m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
        m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_IDLE_ANIM_GROUP;
    }
}

void GD_ClientActorModel::PlayIdleAnimationWithAttackReady(void)
{
    PlayAnimation(RES_ANM_ACTOR_IDLE_ATTACKREADY);
}

void GD_ClientActorModel::PlayWeaponedRandomIdleAnimation(WeaponType eWeaponType)
{
    if (_isInitedWithLoaded())
    {
        if (eWeaponType != m_eWeaponType)
        {
            m_eWeaponType = eWeaponType;

            _updateIdleAnimationIndexList();
        }

        if (m_eCurrentAnimType == AT_WEAPONED_IDLE_ANIM_GROUP)
        {
            return;
        }

        _changeToNextWeaponedIdleAnimation();
    }
    else
    {
        m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
        m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_WEAPONED_IDLE_ANIM_GROUP;
        m_kClientActorPendingSettings.eWeaponType = eWeaponType;
    }
}

void GD_ClientActorModel::PlayWeaponedRandomIdleAnimationWithHobby(GameLogic::WeaponType eWeaponType, const StringType& strAnim)
{
	if (_isInitedWithLoaded())
	{
		if (eWeaponType != m_eWeaponType)
		{
			m_eWeaponType = eWeaponType;

			_updateIdleAnimationIndexList();
		}

		if (m_strCurrHobbyIdleAnim != strAnim)
		{
			if (_getAnimation(strAnim) == NULL)
			{
				 m_strCurrHobbyIdleAnim.clear(); //reset it to invalid value if the animation can't be found
			}
			else
			{
				m_strCurrHobbyIdleAnim = strAnim;
			}
		}

		if (m_eCurrentAnimType == AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP)
		{
			return;
		}

		_changeToNextWeaponedIdleAnimationWithHobby();
	}
	else
	{
		m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
		m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP;
		m_kClientActorPendingSettings.eWeaponType = eWeaponType;
		m_kClientActorPendingSettings.strHobbyAnim = strAnim;
	}
}

void GD_ClientActorModel::PlayWeaponedIdleAnimationWithAttackReady(WeaponType eWeaponType)
{
	StringType animName = LC_ClientHelp::GetWeaponedResName(eWeaponType, RES_ANM_ACTOR_IDLE_ATTACKREADY, true);
    PlayAnimation(animName);
}

void GD_ClientActorModel::PlayRandomAttackAnimation(void)
{
    if (_isInitedWithLoaded())
    {
        if (m_eCurrentAnimType == AT_ATTACK_ANIM_GROUP)
        {
            return;
        }

        _changeToNextAttackAnimation();
    }
    else
    {
        m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
        m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_ATTACK_ANIM_GROUP;
    }
}

void GD_ClientActorModel::PlayDefaultAnimation(bool bForce,  float fFrequency,  bool bAutoAdjustFrequency)
{
	_playAnimation(m_pkModelData->m_strDefaultAnimation, bForce, fFrequency, bAutoAdjustFrequency);
}

void GD_ClientActorModel::PlayAttackRepeatAnimation()
{
	if (_isInitedWithLoaded())
	{
		if (m_eCurrentAnimType == AT_ATTACK_NORMAL_GROUP)
		{
			return;
		}

		_changeToNextNormalAttackGroupAnimation();
	}
	else
	{
		m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
		m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_ATTACK_NORMAL_GROUP;
	}
}

void GD_ClientActorModel::PlayAttack1Animation()
{
	if (_isInitedWithLoaded())
	{
		if (m_eCurrentAnimType == AT_ATTACK_NORMAL_1)
		{
			return;
		}

		_changeToNextNormalAttack1Animation();
	}
	else
	{
		m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
		m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_ATTACK_NORMAL_1;
	}
}

void GD_ClientActorModel::PlayDisplayIdleAnimation(GameLogic::WeaponType eWeaponType)
{
	if (_isInitedWithLoaded())
	{		
		if (m_eCurrentAnimType == AT_DISPLAY_GROUP)
		{
			return;
		}
		m_eWeaponType = eWeaponType;
		m_iCurrDisplayAnimIndex = 0;
		_changeToNextDisplayIdleAnimation();
	}
	else
	{
		m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
		m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_DISPLAY_GROUP;
		m_kClientActorPendingSettings.eWeaponType = eWeaponType;
	}
}

void GD_ClientActorModel::PlayDisplayNPCIdleAnimation()
{
	if (_isInitedWithLoaded())
	{		
		if (m_eCurrentAnimType == AT_DISPLAY_NPC_GROUP)
		{
			return;
		}
		m_iCurrDisplayAnimIndex = 0;
		_changeToNextDisplayNPCIdleAnimation();
	}
	else
	{
		m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
		m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_DISPLAY_NPC_GROUP;
	}
}

void GD_ClientActorModel::PlayDisplayNPCIdle3Animation()
{

	if (_isInitedWithLoaded())
	{		
		if (m_eCurrentAnimType == AT_DISPLAY_NPC_GROUP3)
		{
			return;
		}
		m_iCurrDisplayAnimIndex = 0;
		_changeToNextDisplayNPCIdle3Animation();
	}
	else
	{
		m_kClientActorPendingSettings.eTypes |= CAPST_PLAY_ANIM;
		m_kClientActorPendingSettings.eAnimPendingPlayingType = AT_DISPLAY_NPC_GROUP3;
	}
}

NiNodePtr GD_ClientActorModel::GenerateNewSuitData(void)
{
    T_ASSERT(!GfxIsMainThread() 
        || BL_AsyncOperationManager::GetSingletonPtr() == NULL || !BL_AsyncOperationManager::GetSingleton().IsLoadingThreadAlive());

    if (m_pkSuit == NULL)
    {
        GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, 
            "Generate new suit-data but suit is NULL! [%s]", GetModFileName().c_str());
        return NULL;
    }

    return m_pkSuit->GenerateNewSuitData();
}

void GD_ClientActorModel::ApplyNewSuit(NiNodePtr spNewSuitNiRootNode)
{
    T_ASSERT(GfxIsMainThread());

    _applyNewSuit(spNewSuitNiRootNode);
}


void GD_ClientActorModel::SetWingRot(float x, float y, float z)
{
	NiMatrix3 all;
	all.MakeIdentity();
	NiMatrix3 rot;
	if ( z != 0 )
	{
		rot.MakeZRotation(z*UT_MathBase::ANGLE2RAD);
		all = all * rot;
	}
	if ( x != 0 )
	{
		rot.MakeXRotation(x*UT_MathBase::ANGLE2RAD);
		all = all * rot;
	}
	if ( y != 0 ) 
	{
		rot.MakeYRotation(y*UT_MathBase::ANGLE2RAD);
		all = all * rot;
	}

	m_pkWingLocalTransform[WSP_BACK][GD_WeaponModel::WST_RH]->m_Rotate =all; 

	if ( m_pkWing)
		m_pkWing->SetWeaponLocalPos(m_pkWingLocalTransform[WSP_BACK]);
}

void GD_ClientActorModel::SetWingXYZ(float x, float y, float z)
{
	m_pkWingLocalTransform[WSP_BACK][GD_WeaponModel::WST_RH]->m_Translate = NiPoint3(x,y,z); 

	if ( m_pkWing)
		m_pkWing->SetWeaponLocalPos(m_pkWingLocalTransform[WSP_BACK]);
}

#ifdef _ENABLE_PHYSX
bool GD_ClientActorModel::InitPhysXAnimation(const StringType& strAnimationName)
{
    if (m_pkModelContent == NULL)
    {
        return false;
    }

    if (!_addPhysXPropToSceneManager())
    {
        return false;
    }
    bool bInit = m_pkModelContent->InitPhysXAnimation(_getAnimationID(strAnimationName));
	if ( !bInit )
	{
		_removePhysXPropFromSceneManager();
		return false;
	}
    m_bPhysXInited = true;

    return true;
}
#endif

void GD_ClientActorModel::ActiveNormalDead(float fCurrentTime, float fDeadTime, int iPlayFlag)
{
    m_bIsActiveNormalDead   = true;
    m_fNormalDeadStartTime  = fCurrentTime;
    m_fNormalDeadLastTime   = fDeadTime;
	
	PlayAnimation(RES_ANM_ACTOR_DEAD, true);
    //_detachAllAttachedFxs();
}

void GD_ClientActorModel::ActiveDyingDead(float fCurrentTime, float fDeadTime)
{
	m_bIsActiveNormalDead   = true;
	m_fNormalDeadStartTime  = fCurrentTime;
	m_fNormalDeadLastTime   = fDeadTime;
}

void GD_ClientActorModel::DeactiveNormalDead(void)
{
    m_bIsActiveNormalDead = false;
}

bool GD_ClientActorModel::IsNormalDeadEnd(float fCurrentTime)
{
    if (!m_bIsActiveNormalDead)
    {
        return false;
    }

    return (fCurrentTime - m_fNormalDeadStartTime >= m_fNormalDeadLastTime);
}

#ifdef _ENABLE_PHYSX
void GD_ClientActorModel::ActiveRagDoll(float fCurrentTime, float fRagDollTime)
{
    T_ASSERT2(m_bIsActiveRagDoll == false, 
        "Actor's RagDoll has already been actived before");

    int eTargetAnim = _getAnimationID(RES_ANM_ACTOR_PHYSICS);
    if (m_pkModelContent == NULL || eTargetAnim < 0)
    {
        return;
    }

    m_spNiActorManager->Reset();
    m_spNiActorManager->SetTargetAnimation((NiActorManager::SequenceID)eTargetAnim);

    m_fRagDollStartTime = fCurrentTime;
    m_fRagDollLastTime  = fRagDollTime;
    m_bIsActiveRagDoll  = true;

    NiPhysXPropPtr spPhysXProp = m_pkModelContent->GetNiPhysXProp();
    if (spPhysXProp != NULL)
    {
        NiPhysXPropDesc* pkPropDesc = spPhysXProp->GetSnapshot();
        int iActorDescCount = pkPropDesc->GetActorCount();

        for (int i = 0; i < iActorDescCount; ++i)
        {
            NxActor* pkNxActor = pkPropDesc->GetActorAt(i)->GetActor();
            if (pkNxActor == NULL)
            {
                continue;
            }

            pkNxActor->putToSleep();
            pkNxActor->clearBodyFlag(NX_BF_FROZEN);
        }
    }

    //_detachAllAttachedFxs();
    if (m_bFakeShadow && !m_pkModelData->m_bDisableFakeShadow && _isExternalEnabledFakeShadow())
    {
        //DetachFx(m_eCurrentFakeShadowDummyNode, RES_FX_FAKE_SHADOW);
        m_eCurrentFakeShadowDummyNode = FDT_HEART;
        AttachFx(m_eCurrentFakeShadowDummyNode, MTF_ModelData::AFBM_TRANSLATE, false, RES_FX_FAKE_SHADOW, 
            NULL, NULL, -1.0f, false, m_kFakeShadowColor);
    }
}

void GD_ClientActorModel::DeactiveRagDoll(void)
{
    if (!m_bIsActiveRagDoll || m_pkModelContent == NULL)
    {
        return;
    }

    m_bIsActiveRagDoll = false;

    NiPhysXPropPtr spPhysXProp = m_pkModelContent->GetNiPhysXProp();
    if (spPhysXProp != NULL)
    {
        NiPhysXPropDesc* pkPropDesc = spPhysXProp->GetSnapshot();
        int iActorDescCount = pkPropDesc->GetActorCount();

        for (int i = 0; i < iActorDescCount; ++i)
        {
            NxActor* pkNxActor = pkPropDesc->GetActorAt(i)->GetActor();
            if (pkNxActor == NULL)
            {
                continue;
            }

            pkNxActor->putToSleep();
            pkNxActor->raiseBodyFlag(NX_BF_FROZEN);
        }
    }

    SetIsUpdatable(false);
}

void GD_ClientActorModel::SetRagDollMomentum(const NiPoint3& kLineMomentum)
{
    if (m_pkModelContent == NULL)
    {
        return;
    }

    NiPhysXPropPtr spPhysXProp = m_pkModelContent->GetNiPhysXProp();
    if (spPhysXProp != NULL)
    {
        NiPhysXPropDesc* pkPropDesc = spPhysXProp->GetSnapshot();
        int iActorDescCount = pkPropDesc->GetActorCount();

        for (int i = 0; i < iActorDescCount; ++i)
        {
            NiPhysXActorDesc* pkActorDesc = pkPropDesc->GetActorAt(i);
            NIASSERT(pkActorDesc != NULL);

			NxActor* pkNxActor = pkActorDesc->GetActor();
			if (pkNxActor == NULL)
			{
				continue;
			}

			//if(i == 0)
            {
		        pkNxActor->setLinearMomentum(
                    NxVec3(kLineMomentum.x * UT_MathBase::RandFloat01(), kLineMomentum.y * UT_MathBase::RandFloat01(), kLineMomentum.z));
	 		}
        }
    }
}

bool GD_ClientActorModel::IsRagDollEnd(float fCurrentTime)
{
    if (!m_bIsActiveRagDoll)
    {
        return false;
    }

    return (fCurrentTime - m_fRagDollStartTime >= m_fRagDollLastTime);
}

UT_Vec3Int GD_ClientActorModel::GetRagDollRootGlobalPos(void)
{
    if (m_pkModelContent == NULL)
    {
        return UT_Vec3Int(0, 0, 0);
    }

    return m_pkModelContent->GetPhysXRootActorGlobalPosition();
}

//void GD_ClientActorModel::AddRagDollForce(const NiPoint3& kForceDir, float fForceApplayTime, float fCurrentTime)
//{
//    if (m_pkModelContent == NULL || m_bForceApply)
//    {
//        return;
//    }
//
//    NiPhysXPropPtr spPhysXProp = m_pkModelContent->GetNiPhysXProp();
//    if (spPhysXProp != NULL)
//    {
//        m_bForceApply       = true;
//        //m_kForce            = kForceDir;
//        //m_fForceApplyTime   = fForceApplayTime;
//        //m_fForceStartTime   = fCurrentTime;
//    }
//}
#endif

bool GD_ClientActorModel::IsPickable(Base::BS_IMInterface::PickMask ePickMask)
{
    if (!GD_ActorModel::IsPickable(ePickMask)
        || IsActiveRagDoll() || IsActiveNormalDead())
    {
        return false;
    }

    return true;
}

void GD_ClientActorModel::EnableFakeShadow(bool bEnable)
{
	m_bFakeShadow = bEnable;

    if (_isInitedWithLoaded())
    {
        if (m_bFakeShadow && !m_pkModelData->m_bDisableFakeShadow && _isExternalEnabledFakeShadow())
        {
            _attachFakeShadowFx();
        }
        else
        {
            _detachFakeShadowFx();
        }
    }
}

void GD_ClientActorModel::SetAreaEffectInfo(MTF_EffectTile::AreaEffectType eAreaEffectType, 
                                            const StringType& strFxFileName, 
                                            const StringType& strSoundFileName)
{
    T_ASSERT(eAreaEffectType >= MTF_EffectTile::AET_UNKNOWN && eAreaEffectType < MTF_EffectTile::AET_MAX_COUNT);

    m_astrAreaEffectFxNames[eAreaEffectType] = strFxFileName;
    m_astrAreaEffectSoundNames[eAreaEffectType] = strSoundFileName;
}

void GD_ClientActorModel::EnableAreaEffect(void)
{
    if (!m_bAreaEffectEnabled)
    {
        m_bAreaEffectEnabled = true;

        _setCurrentAreaEffect(GetTranslate());
    }
}

void GD_ClientActorModel::DisableAreaEffect(void)
{
    if (m_bAreaEffectEnabled)
    {
        m_bAreaEffectEnabled = false;

        if (!m_astrAreaEffectFxNames[m_eLastAreaEffectType].empty())
        {
            DetachFx(FDT_GROUND, m_astrAreaEffectFxNames[m_eLastAreaEffectType]);
        }
        if (!m_astrAreaEffectSoundNames[m_eLastAreaEffectType].empty())
        {
		    g_soundMgr.StopAudioSource( m_iCurrentAreaEffectSoundId );
		    /*GfxWriteLog( LOG_SOUND_FILELINE , LOG_SWITCH_SOUND ,
			    "[%f]Stop Area Sound ID %d" ,NiGetCurrentTimeInSec(),
			    m_iCurrentAreaEffectSoundId);*/
        }
        m_eLastAreaEffectType = MTF_EffectTile::AET_UNKNOWN;
    }
}

void GD_ClientActorModel::AffectPointLight(float fStart, float fEnd)
{
    m_spNiPointLight = NiDynamicCast(NiPointLight, m_spNiRootNode->GetObjectByName("object0"));
    if (m_spNiPointLight != NULL)
    {
        m_bAffectPointLight = true;
        m_kPointLightParam.x = fStart;
        m_kPointLightParam.y = fEnd;

        if (SE_SceneManager::GetSingletonPtr() != NULL)
        {
            SE_SceneManager::GetSingleton().UseExternalPointLight(true);
        }
    }
}

void GD_ClientActorModel::StopAffectPointLight(void)
{
    if (m_bAffectPointLight && m_spNiPointLight != NULL)
    {
        if (SE_SceneManager::GetSingletonPtr() != NULL)
        {
            SE_SceneManager::GetSingleton().UseExternalPointLight(false);
        }

        m_bAffectPointLight = false;
        m_spNiPointLight = NULL;
    }
}

bool GD_ClientActorModel::GetDynamicCollisionShape(NiPoint3& rkCenterPoint, 
                                                   NiPoint2& rkVertex1, NiPoint2& rkVertex2, NiPoint2& rkVertex3, NiPoint2& rkVertex4)
{
    if (!_isInitedWithLoaded())
    {
        return false;
    }

    NiAVObjectPtr spPickRootObject = GetPickRoot();
    NiGeometryPtr spPickRootNiGeometry = NiDynamicCast(NiGeometry, spPickRootObject);
    if (spPickRootNiGeometry == NULL)
    {
        NiNodePtr spPickRootNiNode = NiDynamicCast(NiNode, spPickRootObject);
        if (spPickRootNiNode != NULL)
        {
            spPickRootNiGeometry = NiDynamicCast(NiGeometry, spPickRootNiNode->GetAt(0));
        }
    }
    if (spPickRootNiGeometry == NULL)
    {
        rkVertex1 = rkVertex2 = rkVertex3 = rkVertex4 = NiPoint2::ZERO;
        return true;
    }
    //spPickRootNiGeometry->SetAppCulled(false);

    NiMatrix3 kOrigRotate = spPickRootNiGeometry->GetWorldRotate() * _getCurrentRotation().Inverse();
    float fMinX = 999999.0f, fMaxX = -999999.0f, fMinY = 999999.0f, fMaxY = -999999.0f;

    NiPoint3* pkVertices = spPickRootNiGeometry->GetVertices();
    NiPoint3 kVertex;
    for (unsigned short i = 0; i < spPickRootNiGeometry->GetActiveVertexCount(); ++i)
    {
        kVertex = /*kOrigRotate **/spPickRootObject->GetRotate()* pkVertices[i]/* * spPickRootNiGeometry->GetWorldScale()*/;

        fMinX = min(fMinX, kVertex.x);
        fMaxX = max(fMaxX, kVertex.x);
        fMinY = min(fMinY, kVertex.y);
        fMaxY = max(fMaxY, kVertex.y);
    }

	NiNode* pkPickParent = spPickRootObject->GetParent();
    float fHalfLength   = max(25.0f, (fMaxX - fMinX) / 2.0f);
    float fHalfWidth    = max(25.0f, (fMaxY - fMinY) / 2.0f);
    NiPoint3 kVertices[4];
    kVertices[0].x = fHalfLength;   kVertices[0].y = fHalfWidth;    kVertices[0].z = 0.0f;
    kVertices[1].x = fHalfLength;   kVertices[1].y = -fHalfWidth;   kVertices[1].z = 0.0f;
    kVertices[2].x = -fHalfLength;  kVertices[2].y = -fHalfWidth;   kVertices[2].z = 0.0f;
    kVertices[3].x = -fHalfLength;  kVertices[3].y = fHalfWidth;    kVertices[3].z = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		kVertices[i] = /*_getCurrentRotation() **/ pkPickParent->GetWorldRotate()* kVertices[i];
	}

    rkCenterPoint.x = (fMaxX + fMinX) / 2.0f;
    rkCenterPoint.y = (fMaxY + fMinY) / 2.0f;
    rkCenterPoint.z = 0.0f;
    rkCenterPoint = /*_getCurrentRotation() **/pkPickParent->GetWorldRotate()* rkCenterPoint;
    rkCenterPoint += pkPickParent->GetWorldTranslate();

    rkVertex1.x = kVertices[0].x; rkVertex1.y = kVertices[0].y;
    rkVertex2.x = kVertices[1].x; rkVertex2.y = kVertices[1].y;
    rkVertex3.x = kVertices[2].x; rkVertex3.y = kVertices[2].y;
    rkVertex4.x = kVertices[3].x; rkVertex4.y = kVertices[3].y;

    return true;
}

void GD_ClientActorModel::ReattachToLightEffect(void)
{
    if (_isInitedWithLoaded())
    {
        _initLightEffectAndShadow();
    }
}

void GD_ClientActorModel::SetVehicleLastPos(const NiPoint3& pos)
{
	m_vehicleLast = pos;
}

void GD_ClientActorModel::DoDetachAkkAttacgFxs()
{
	_detachAllAttachedFxs();
}

void GD_ClientActorModel::DoDetachFxsOnAnim()
{
	_detachAllAttachedFxsOnAnimation();
}

void GD_ClientActorModel::AttachFxExt(FxDummyNodeType eDummy, int eBindMode, bool bStayAfterBound, const StringType& strFxFileName, 
								   const NiPoint3* pkTranslate /*= NULL*/, const NiPoint3* pkRotate /*= NULL*/, float fScale /*= 1.0f*/, bool bScaledWithParent /*= false*/,
								   NiColor kColor /*= NiColor::WHITE*/, bool bExclusively /*= true*/, bool bPlayImmediately /*= true*/, const NiPoint3* pKScale /*= NULL*/, bool bOnAnim /*= false*/)
{
	//这个函数我也不想加，因为很多余
	//虽然在ClientActorModel的Update里面也会设置特效的缩放参数，
	//但是有一个debuff效果是冰冻，会FreeAnimation，导致ClientActorModel不进行Update，所以还是一开始就把这个缩放参数弄好吧，保险一点
	NiPoint3 delta = NiPoint3::ZERO;
	float fdelta = fScale;
	if( m_pkVehicleModel && eDummy == FDT_DYNAMIC_GROUND)
	{
		//位移偏移在ActorModel里面
		fdelta = m_pkVehicleModel->m_fFxAutoScale;
	}
	
	GD_ModelBase::AttachFx(eDummy, eBindMode, bStayAfterBound, strFxFileName, 
		&delta, pkRotate, fdelta, bScaledWithParent, kColor, bExclusively, bPlayImmediately, pKScale, bOnAnim);
	
}

void GD_ClientActorModel::DetachFxExt(FxDummyNodeType eDummy, const StringType& strFxFileName, bool bSmoothly /*= true*/)
{
// 	if( m_pkVehicleModel && eDummy == FDT_GROUND)
// 	{
// 		m_pkVehicleModel->DetachFx(eDummy, strFxFileName, bSmoothly);
// 		return ;
// 	}

	GD_ModelBase::DetachFx(eDummy, strFxFileName, bSmoothly);
}

void GD_ClientActorModel::FixPos(const NiPoint3& pos, FxDummyNodeType eType)
{
	if (m_aspFxDummyNiNode[eType] )
	{
		NiTransform tran = m_aspFxDummyNiNode[eType]->GetWorldTransform();
		tran.m_Translate = pos;
		m_aspFxDummyNiNode[eType]->SetLocalFromWorldTransform(tran);
	}
}

void GD_ClientActorModel::FixRot(const NiMatrix3& rot, GD_ModelBase::FxDummyNodeType eType)
{
	if (m_aspFxDummyNiNode[eType] != NULL )
	{
		NiTransform tran = m_aspFxDummyNiNode[eType]->GetWorldTransform();
		tran.m_Rotate = rot;
		m_aspFxDummyNiNode[eType]->SetLocalFromWorldTransform(tran);
	}
}

void GD_ClientActorModel::FixTransform(const NiTransform& tfm, GD_ModelBase::FxDummyNodeType eType)
{
	if (m_aspFxDummyNiNode[eType] != NULL )
	{
		NiTransform tran = m_aspFxDummyNiNode[eType]->GetWorldTransform();
		tran.m_Translate = tfm.m_Translate;
		tran.m_Rotate = tfm.m_Rotate;
		m_aspFxDummyNiNode[eType]->SetLocalFromWorldTransform(tran);
	}
}

void GD_ClientActorModel::SetFxAutoScale(float fScale)
{
	if ( fScale > 0.0f )
		m_fFxAutoScale = fScale;
}

void GD_ClientActorModel::SetClientCull(bool isCull)
{
	m_clietNoCull = isCull;
}

void GD_ClientActorModel::AnimActivated(NiActorManager* pkManager, NiActorManager::SequenceID eSequenceID, 
									 float fCurrentTime, float fEventTime)
{
	MTF_ModelData::AnimationItem* ite = _getAnimation(RES_ANM_ACTOR_BORN);
	if (ite && ite->iSequenceID == eSequenceID )
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		LC_ClientNPC* pkClientNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(GetLogicID());
		if ( pkClientNPC )
		{
			if ( Camera::CA_CameraAnimationManager::GetSingleton().GetMaxAnimationPath(pkClientNPC->GetCharType()) )
			{
				CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_TRACK_ANIM);
				CA_CameraManager::GetSingleton().PlayMaxAnim(pkClientNPC->GetCharType(), GetLogicID());

				CF_CameraCut::DataEntry* de = SafeGetCSVFileDataEntryPtr<CF_CameraCut>(pkClientNPC->GetCharType());
				if (de)
				{
					g_soundMgr.Play2DEx(de->_kSound);
				}
			}
		}
	}
}

bool GD_ClientActorModel::_doLoad(const StringType& strModFileName, int iActiveModelIndex)
{
    if (!GD_ActorModel::_doLoad(strModFileName, iActiveModelIndex))
    {
        return false;
    }

#ifdef _ENABLE_PHYSX
    if(m_pkModelContent != NULL)
    {
        NiPhysXPropPtr spPhysXProp = m_pkModelContent->GetNiPhysXProp();
        if (spPhysXProp != NULL)
        {
            spPhysXProp->SetScaleFactor(m_pkModelData->m_fScale);
        }
    }
#endif

    return true;
}

void GD_ClientActorModel::_doInitAfterLoad(void)
{
    m_bNeedToRecalcPitching = false;
    m_bFirstPitching = true;
    m_fLastPitchingAngle = 0.0f;

    m_fWaterAreaEffectHeight = -10000.0f;

    GD_ActorModel::_doInitAfterLoad();

    _initIdleAnimationIndexList();

    if (IsVisible())
    {
        _initLightEffectAndShadow();
    }

    if (m_spPickRootExtraData != NULL)
    {
        m_spPickRootExtraData->SetValue(GetLogicID());
    }
	else
	{
		this->_createPickRoot();
	}

    if (m_bIsVehicle)
    {
        SetTranslate(NiPoint3::ZERO);
    }
}

void GD_ClientActorModel::_doPreRelease(void)
{
	DeactiveRagDoll();
    DisableAreaEffect();
    StopAffectPointLight();

    if (m_bAreaEffectEnabled)
    {
        if (!m_astrAreaEffectFxNames[m_eLastAreaEffectType].empty())
        {
            DetachFx(FDT_GROUND, m_astrAreaEffectFxNames[m_eLastAreaEffectType]);
        }
        if (!m_astrAreaEffectSoundNames[m_eLastAreaEffectType].empty())
        {
			g_soundMgr.StopAudioSource( m_iCurrentAreaEffectSoundId );
			/*GfxWriteLog( LOG_SOUND_FILELINE , LOG_SWITCH_SOUND ,
				"[%f]Stop Area Sound ID %d" ,NiGetCurrentTimeInSec(),
				m_iCurrentAreaEffectSoundId);*/
        }
    }

    if (IsVisible())
    {
        _uninitLightEffectAndShadow();
    }
#ifdef _ENABLE_PHYSX
    _removePhysXPropFromSceneManager();
    if (m_pkModelContent != NULL)
    {
        m_pkModelContent->UninitPhysXAnimation();
    }
#endif

    m_kAvailableFreeIdleAnimIndices.clear();

    m_bHideBeforeSuitApplied = false;
    m_bAllClothingChanged = false;
    m_bVisibilityWhenHideBeforeSuitApplied = true;
    m_bPitchByTerrain = true;
	m_bIsVehicle = false;

	FX_ClientFxManager& fxClientFxManager = FX_ClientFxManager::GetSingleton();
	mem::set<int32_t>::iterator iter = m_pairGfxIDSet.begin();
	mem::set<int32_t>::iterator iend = m_pairGfxIDSet.end();
	for ( ; iter != iend ; ++iter )
		fxClientFxManager.NotifyGfxPairRelease(*iter);
	m_pairGfxIDSet.clear();

    GD_ActorModel::_doPreRelease();
}

void GD_ClientActorModel::_doTranslated(const NiPoint3& kTranslate)
{
    // 更新当前是否在阴影中
	//地宫中暂时不受阴影影响，否则会很奇怪
	if ( GfxGetWorldConfig().CFG_BAKING_TYPE == Terrain::TS_WorldConfig::NORMAL /*&& g_settings.LOD_CHAR_SHADER > 1*/ )
	{
		m_spShadowedExtraData->SetValue(SE_SceneManager::GetSingleton().QueryVisibilityMap(kTranslate));
	}
	if ( SE_SceneManager::GetSingleton().GetWorldMap() )
	{
		_updateSHCoeffs( kTranslate , SE_SceneManager::GetSingleton().GetWorldMap()->GetLightProbeMgr() );
	}
    // 计算俯仰角
    if (m_bPitchByTerrain && m_pkModelData->m_bPitchByTerrain)
    {
        m_bNeedToRecalcPitching = true;
    }
}

void GD_ClientActorModel::_doRotated(void)
{
    // 计算俯仰角
    if (m_bPitchByTerrain && m_pkModelData->m_bPitchByTerrain)
    {
        m_bNeedToRecalcPitching = true;
    }
}

//void GD_ClientActorModel::_doAlphaChanged(float fAlpha)
//{
//    GD_ActorModel::_doAlphaChanged(fAlpha);
//
//    if (!m_bFakeShadow && (
//        (GetAlpha() < ALPHA_THRESHOLD_TO_HIDE_SHADOW && ALPHA_THRESHOLD_TO_HIDE_SHADOW <= m_fLastAlpha)
//        || (m_fLastAlpha < ALPHA_THRESHOLD_TO_HIDE_SHADOW && ALPHA_THRESHOLD_TO_HIDE_SHADOW <= GetAlpha())))
//    {
//        SE_SceneManager& rkSceneMgr = SE_SceneManager::GetSingleton();
//        if (GetAlpha() >= ALPHA_THRESHOLD_TO_HIDE_SHADOW)
//        {
//        }
//        else
//        {
//        }
//    }
//
//    m_fLastAlpha = GetAlpha();
//}

void GD_ClientActorModel::_onRequestSettingVisibility(bool bVisible)
{
    // 换装期间记录真实可见性属性
    if (m_bHideBeforeSuitApplied && m_bRecordVisibilityWhenHideBeforeSuitApplied)
    {
        m_bVisibilityWhenHideBeforeSuitApplied = bVisible;
    }
}

void GD_ClientActorModel::_doVisibilityChanged(bool bVisible)
{
    GD_ActorModel::_doVisibilityChanged(bVisible);

    if (bVisible)
    {
        _initLightEffectAndShadow();
    }
    else
    {
        _uninitLightEffectAndShadow();
    }
}

void GD_ClientActorModel::_doUpdatePendingSettings(void)
{
    if ((m_kClientActorPendingSettings.eTypes & CAPST_PLAY_ANIM) > 0)
    {
        switch (m_kClientActorPendingSettings.eAnimPendingPlayingType)
        {
        case AT_IDLE_ANIM_GROUP:
            PlayRandomIdleAnimation();
        	break;
        case AT_WEAPONED_IDLE_ANIM_GROUP:
            PlayWeaponedRandomIdleAnimation(m_kClientActorPendingSettings.eWeaponType);
            break;
        case AT_ATTACK_ANIM_GROUP:
            PlayRandomAttackAnimation();
            break;
		case AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP:
			PlayWeaponedRandomIdleAnimationWithHobby(m_kClientActorPendingSettings.eWeaponType, m_kClientActorPendingSettings.strHobbyAnim);
			break;
		case AT_GREATKUNGFU_GROUP:
			T_ASSERT(false);
			break;
		case AT_ATTACK_NORMAL_GROUP:
			PlayAttackRepeatAnimation();
			break;
		case AT_ATTACK_NORMAL_1:
			PlayAttack1Animation();
			break;
		case AT_DISPLAY_GROUP:
			PlayDisplayIdleAnimation(m_kClientActorPendingSettings.eWeaponType);
			break;
		case AT_DISPLAY_NPC_GROUP:
			PlayDisplayNPCIdleAnimation();
			break;
		case AT_DISPLAY_NPC_GROUP3:
			PlayDisplayNPCIdle3Animation();
			break;
        }
    }

    GD_ActorModel::_doUpdatePendingSettings();

    m_kClientActorPendingSettings.clear();

    if (m_bHideBeforeSuitApplied)
    {
        m_bVisibilityWhenHideBeforeSuitApplied = IsVisible();
        m_bRecordVisibilityWhenHideBeforeSuitApplied = false;
        SetVisibility(false);
        m_bRecordVisibilityWhenHideBeforeSuitApplied = true;
    }

#ifndef _MAP_VIEWER_BUILD
    if (m_bHideBeforeSuitApplied && m_bAllClothingChanged)
    {
        ShowAfterSuitApplied();
    }
#else // _MAP_VIEWER_BUILD
    if (IsPlayer())
    {
        HideBeforeSuitApplied();

        UT_ConfigFile kConfig;
        int iSection = kConfig.Load( CLIENT_PREVIEW_MAP_CONFIG_FILE );
        if (iSection == 3)
        {
            StringType sModName = kConfig.GetSetting(CLIENT_MAP_CONFIG_HEAD, CLIENT_MAP_CONFIG_SECTION_ACTOR, "");
            ChangeClothing(GD_ClothingModel::CT_HEAD, sModName);

            sModName = kConfig.GetSetting(CLIENT_MAP_CONFIG_BODY, CLIENT_MAP_CONFIG_SECTION_ACTOR, "");
            ChangeClothing(GD_ClothingModel::CT_BODY, sModName);

            sModName = kConfig.GetSetting(CLIENT_MAP_CONFIG_FEET, CLIENT_MAP_CONFIG_SECTION_ACTOR, "");
            ChangeClothing(GD_ClothingModel::CT_FEET, sModName);

            sModName = kConfig.GetSetting(CLIENT_MAP_CONFIG_WEAPON, CLIENT_MAP_CONFIG_SECTION_ACTOR, "");
            ChangeWeapon(GD_WeaponModel::WST_LRH, sModName);
        }

        ShowAfterSuitApplied();
    }
#endif // _MAP_VIEWER_BUILD
}

void GD_ClientActorModel::_playAnimation(const StringType& strAnimationName, 
                                         bool bForce, 
                                         float fFrequency, 
                                         bool bAutoAdjustFrequency)
{
    if (m_bAnimTypeChanged)
    {
        m_bAnimTypeChanged = false;
    }
    else
    {
        m_eCurrentAnimType = AT_SINGLE_ANIM;
        m_iCurrIdleAnimIndex = -1;
        m_nCurrIdleAnimRemain = 0;
    }

    GD_ActorModel::_playAnimation(strAnimationName, bForce, fFrequency, bAutoAdjustFrequency);
}

void GD_ClientActorModel::_doEndOfAnimation(const StringType& strAnimationName, float fCurrentTime)
{
    switch (m_eCurrentAnimType)
    {
    case AT_SINGLE_ANIM:
    	break;
    case AT_IDLE_ANIM_GROUP:
        _changeToNextIdleAnimation();
        break;
    case AT_WEAPONED_IDLE_ANIM_GROUP:
        _changeToNextWeaponedIdleAnimation();
        break;
    case AT_ATTACK_ANIM_GROUP:
        _changeToNextAttackAnimation();
        break;
	case AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP:
		_changeToNextWeaponedIdleAnimationWithHobby();
		break;
	case AT_GREATKUNGFU_GROUP:
		T_ASSERT(false);
		break;
	case AT_ATTACK_NORMAL_GROUP:
		_changeToNextNormalAttackGroupAnimation();
		break;
	case AT_ATTACK_NORMAL_1:
		_changeToNextNormalAttack1Animation();		
		break;
	case AT_DISPLAY_GROUP:
		_changeToNextDisplayIdleAnimation();
		break;
	case AT_DISPLAY_NPC_GROUP:
		_changeToNextDisplayNPCIdleAnimation();
		break;
	case AT_DISPLAY_NPC_GROUP3:
		_changeToNextDisplayNPCIdle3Animation();
		break;
    default:
        break;
    }
}

bool GD_ClientActorModel::_doIsUpdatable(float fTime)
{
    return (GD_ActorModel::_doIsUpdatable(fTime) && !m_bIsVehicle);
}

void GD_ClientActorModel::_doPreUpdate(float fTime)
{
    GD_ActorModel::_doPreUpdate(fTime);

    // 计算俯仰角
    if (m_bNeedToRecalcPitching)
    {
        m_bNeedToRecalcPitching = false;

        const NiPoint3& kLocalXlate = m_spNiRootNode->GetTranslate();
        const SE_SceneTerrainLocalInfo& kTerrainLocalInfo = SE_SceneManager::GetSingleton().GetWalkableInfo(
            kLocalXlate.x, kLocalXlate.y, false, true);
        const NiPoint3& rkNormal = kTerrainLocalInfo._kLocalNormal;

        const NiMatrix3& kCurrRot = _getCurrentRotation();

        bool bFirstPitching = m_bFirstPitching;
        if (m_bFirstPitching && kCurrRot != NiMatrix3::IDENTITY)
        {
            m_bFirstPitching = false;
        }

        // 反转回模型局部坐标系下
        NiPoint3 kN = (rkNormal * kCurrRot);
        kN.x = 0.0f;
        kN.Unitize();

        if (kN != NiPoint3::UNIT_Z)
        {
            // 根据点积的特性求得向量间的弧度
            float fAcos = acos(kN.z/*kN.Dot(NiPoint3::UNIT_Z)*/);
            if (kN.y < 0.0f)
            {
                fAcos = -fAcos;
            }
            if (!bFirstPitching && fabs(fAcos - m_fLastPitchingAngle) > 0.02f)
            {
                if (fAcos >= m_fLastPitchingAngle)
                {
                    m_fLastPitchingAngle += 0.02f;
                }
                else
                {
                    m_fLastPitchingAngle -= 0.02f;
                }
            }
            else
            {
                m_fLastPitchingAngle = fAcos;
            }
            m_kPitchingRotation.MakeXRotation(m_fLastPitchingAngle);

            m_spNiRootNode->SetRotate(kCurrRot * m_kPitchingRotation);
        }
    }

    if (m_pkVehicleModel != NULL )
	{
        m_pkVehicleModel->SetIsVehicleModel(false);
		//偷改坐骑骑乘坐标，让坐骑骑乘坐标和玩家逻辑坐标一直，保证骑乘点的坐标是摄像机的lookup点
		m_pkVehicleModel->EntirelyUpdate(fTime);
		NiPoint3 sd = m_pkVehicleModel->GetSeatLocalLocation(GD_ActorModel::ST_MAJOR);
		sd.z = 0;
		m_pkVehicleModel->SetTranslate(m_pkVehicleModel->GetVehicleLastPos() - sd);
		m_pkVehicleModel->EntirelyUpdate(fTime, true);
        m_pkVehicleModel->SetIsVehicleModel(true);

		if ( m_pkVehicleModel->IsVisible() )
		{
			NiPoint3 pos = m_pkVehicleModel->GetSeatLocation(GD_ActorModel::ST_MAJOR);
			pos.z += m_fVehicleHeight;
			SetTranslate(pos);
		}

		if(m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND])
		{
			//NiPoint3 posMaj = m_pkVehicleModel->GetSeatLocation(GD_ActorModel::ST_MAJOR);
			NiPoint3 tmp = m_pkVehicleModel->GetFxDummyNodePosition(GD_ActorModel::FDT_GROUND);
			NiPoint3 top = m_aspFxDummyNiNode[FDT_GROUND]->GetWorldTranslate();	
			m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND]->SetTranslate(NiPoint3(0,0,tmp.z - top.z));

			FX_ClientFxManager& fxMgr = FX_ClientFxManager::GetSingleton();
			AttachedFxMap& rkAttachedFxMap = m_kAttachedFxMap[FDT_DYNAMIC_GROUND];
			for (AttachedFxMap::iterator itr = rkAttachedFxMap.begin(); itr != rkAttachedFxMap.end(); ++itr)
			{
				fxMgr.SetFxScale(itr->second, m_pkVehicleModel->m_fFxAutoScale);
			}
		}

		if (m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND_NOSCALL])
		{
			NiPoint3 tmp = m_pkVehicleModel->GetFxDummyNodePosition(GD_ActorModel::FDT_GROUND);
			NiPoint3 top = m_aspFxDummyNiNode[FDT_GROUND]->GetWorldTranslate();	
			m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND_NOSCALL]->SetTranslate(NiPoint3(0,0,tmp.z - top.z));
		}
    }
	else
	{
		if(m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND])
		{
			m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND]->SetTranslate(NiPoint3::ZERO);

			FX_ClientFxManager& fxMgr = FX_ClientFxManager::GetSingleton();
			AttachedFxMap& rkAttachedFxMap = m_kAttachedFxMap[FDT_DYNAMIC_GROUND];
			for (AttachedFxMap::iterator itr = rkAttachedFxMap.begin(); itr != rkAttachedFxMap.end(); ++itr)
			{
				fxMgr.SetFxScale(itr->second, m_fFxAutoScale);
			}
		}

		if (m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND_NOSCALL])
		{
			m_aspFxDummyNiNode[FDT_DYNAMIC_GROUND_NOSCALL]->SetTranslate(NiPoint3::ZERO);
		}
	}
	
	if (m_aspFxDummyNiNode[FDT_STABLE_HEAD_TOP])
	{
		m_aspFxDummyNiNode[FDT_STABLE_HEAD_TOP]->Update(fTime);
	}

	if(m_aspFxDummyNiNode[FDT_STABLE_GROUND])
		m_aspFxDummyNiNode[FDT_STABLE_GROUND]->Update(fTime);
	if(m_aspFxDummyNiNode[FDT_STABLE_FDT_HEART])
		m_aspFxDummyNiNode[FDT_STABLE_FDT_HEART]->Update(fTime);
}

void GD_ClientActorModel::_doPostUpdate(float fTime)
{
    //_applyForce(fTime);

    GD_ActorModel::_doPostUpdate(fTime);

    // 更新效果层
    if (m_bAreaEffectEnabled)
    {
        _setCurrentAreaEffect(GetTranslate());
    }

    if (m_bAffectPointLight)
    {
        m_kPointLightParam.z = m_spNiPointLight->GetDimmer() * 10.0f;

        SE_SceneManager::GetSingleton().SetPlayerPointLightParams(
            m_spNiPointLight->GetDiffuseColor(), NiPoint3::ZERO, m_kPointLightParam);
    }
}

void GD_ClientActorModel::_onCullStateChanged(bool bCulled)
{
	m_bIsCulled = m_clietNoCull ? false : m_bIsCulled;
    if (m_pkVehicleModel != NULL)
    {
        GD_ActorModel::_onCullStateChanged(bCulled && m_pkVehicleModel->_isCulled());
    }
    else
    {
        GD_ActorModel::_onCullStateChanged(bCulled);
    }
}

//void GD_ClientActorModel::_doSetSubnodeVisibility(NiNodePtr spSubNiNode, bool bVisible)
//{
//	GD_ActorModel::_doSetSubnodeVisibility(spSubNiNode, bVisible);
//
//}

bool GD_ClientActorModel::_doIsAttachedFxUnrestricted(void)
{
    long lID = GetLogicID();
    if (IS_PLAYER_CLASS_ID(lID))
    {
        if (lID == ((LC_ClientLogicManager&)LC_ClientLogicManager::GetSingleton()).GetOperationPlayerID())
        {
            return true;
        }
    }
    else if (IS_NPC_ID(lID) || IS_LOCAL_NPC_ID(lID))
    {
        return true;
    }
    return false;
}

void GD_ClientActorModel::_createPickRoot(void)
{
    GD_ActorModel::_createPickRoot();
    if (m_spPickRoot == NULL)
    {
        return;
    }

    m_spPickRoot->SetAppCulled(true);

    m_spPickRootExtraData = NiNew NiInteger64ExtraData(GetLogicID());
    m_spPickRootExtraData->SetName("Actor_ID");
    m_spPickRoot->AddExtraData(m_spPickRootExtraData);
}

void GD_ClientActorModel::_doApplyNewSuit(void)
{
    if (MG_MessageCenter::GetSingletonPtr() != NULL)
    {
        GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
            "Send out apply-new-suit message for actor model [%s]", GetModelFileName().c_str());

        MG_MessageCenter::GetSingleton().SendMessage(
            CreateMsg_ADD_CHANGE_AVATAR_TASK(this) /*true*/);// 这个true是否要加入，有空看一下；
    }
}

void GD_ClientActorModel::_onNewSuitApplied(void)
{
    if (m_bHideBeforeSuitApplied && m_bAllClothingChanged)
    {
        GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
            "Show(%s) the actor model after new suit applied [%s]", 
            m_bVisibilityWhenHideBeforeSuitApplied ? "true" : "false", GetModelFileName().c_str());

        m_bHideBeforeSuitApplied = false;

        SetVisibility(m_bVisibilityWhenHideBeforeSuitApplied);
    }
}

void GD_ClientActorModel::_doInitClothingModel(GD_ClothingModel* pkClothingModel)
{
	T_ASSERT(pkClothingModel != NULL && 
		!(pkClothingModel->GetFileName().empty()));

	if ( pkClothingModel )
	{
		bool isMessageSend =  false;
		if ( MG_MessageCenter::GetSingletonPtr() )
		{

			GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
				"Send out load-clothing-model message [%s] for actor model [%s]", 
				pkClothingModel->GetFileName().c_str(), GetModelFileName().c_str());

			// 添加模型加载的异步任务
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage( 
				CreateMsg_ADD_MODEL_LOADING_TASK( GD_ClientAsyncModelManager::AMT_ITEM, 
				pkClothingModel, pkClothingModel->GetFileName(), 0, GetShaderIdentifier() ), true );
		}

		if ( !isMessageSend )
		{
			T_ASSERT(isMessageSend);
			// 如果测试之后发现真的可能跑到这个代码里来再取消注释
			GD_ActorModel::_doInitClothingModel(pkClothingModel);
		}
	}
}

void GD_ClientActorModel::_doReleaseClothingModel(GD_ClothingModel* pkClothingModel, bool isExtraClothing)
{
	if (pkClothingModel != NULL)
	{
		if ( isExtraClothing )
			pkClothingModel->UnbindFromActor();
		else
			pkClothingModel->DetachFxsFromActor();

		_detachChildModel(pkClothingModel);

		pkClothingModel->PreRelease();
		
		bool isMessageSend = false;
		if ( MG_MessageCenter::GetSingletonPtr() )
		{
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage(
				CreateMsg_ADD_MODEL_UNLOADING_TASK(pkClothingModel));
		}

		// 整个客户端反初始化时候会出现isMessageSend为false的情况
		if ( !isMessageSend )
		{
			GD_ActorModel::_doReleaseClothingModel(pkClothingModel, isExtraClothing);
		}
	}
}

void GD_ClientActorModel::_doReleaseLoadingClothingModel(GD_ClothingModel* pkLoadingClothingModel)
{
	if ( pkLoadingClothingModel )
	{
		// 通知加载任务中止, 通知失败直接干掉
		if ( BL_AsyncOperationManager::GetSingletonPtr() && 
			BL_AsyncOperationManager::GetSingleton().IsLoadingThreadAlive() )
		{
			pkLoadingClothingModel->SetReleaseFlag(true);
			BL_AsyncOperationManager::GetSingleton().NotifyInvalidModel( pkLoadingClothingModel );
		}
		else
		{
			GD_ActorModel::_doReleaseLoadingClothingModel(pkLoadingClothingModel);
		}
	}
}

void GD_ClientActorModel::_doInitWeaponModel(GD_WeaponModel* pkWeaponModel)
{
    if ( pkWeaponModel )
    {
		bool isMessageSend =  false;
		if ( MG_MessageCenter::GetSingletonPtr() )
		{
			GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
				"Send out load-weapon-model message [%s] for actor model [%s]", 
				pkWeaponModel->GetFileName().c_str(), GetModelFileName().c_str());

			// 添加模型加载的异步任务
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage( 
				CreateMsg_ADD_MODEL_LOADING_TASK( GD_ClientAsyncModelManager::AMT_ITEM, 
				pkWeaponModel, pkWeaponModel->GetFileName(), 0, GetShaderIdentifier() ), true );
		}

		if ( !isMessageSend )
		{
			T_ASSERT(isMessageSend);
			// 如果测试之后发现真的可能跑到这个代码里再注释
			GD_ActorModel::_doInitWeaponModel(pkWeaponModel);
		}
    }
}

void GD_ClientActorModel::_doReleaseWeaponModel(GD_WeaponModel* pkWeaponModel)
{
	if (pkWeaponModel != NULL)
	{
		pkWeaponModel->DetachFromActor();
		_detachChildModel(pkWeaponModel);

		pkWeaponModel->PreRelease();

		bool isMessageSend = false;
		if ( MG_MessageCenter::GetSingletonPtr() )
		{
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage(
				CreateMsg_ADD_MODEL_UNLOADING_TASK(pkWeaponModel));
		}

		// 整个客户端反初始化时候会出现isMessageSend为false的情况
		if ( !isMessageSend )
		{
			GD_ActorModel::_doReleaseWeaponModel(pkWeaponModel);
		}
	}
}

void GD_ClientActorModel::_doReleaseLoadingWeaponModel(GD_WeaponModel* pkLoadingWeaponModel)
{
	if ( pkLoadingWeaponModel )
	{
		// 通知加载任务中止, 通知失败直接干掉
		if ( BL_AsyncOperationManager::GetSingletonPtr() && 
			BL_AsyncOperationManager::GetSingleton().IsLoadingThreadAlive() )
		{
			pkLoadingWeaponModel->SetReleaseFlag(true);
			BL_AsyncOperationManager::GetSingleton().NotifyInvalidModel( pkLoadingWeaponModel );
		}
		else
		{
			GD_ActorModel::_doReleaseLoadingWeaponModel(pkLoadingWeaponModel);
		}
	}
}

void GD_ClientActorModel::_doInitBowModel(GD_WeaponModel* pkBowModel)
{
	if ( pkBowModel )
	{
		bool isMessageSend =  false;

		if (MG_MessageCenter::GetSingletonPtr() != NULL)
		{
			GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
				"Send out load-weapon-model message [%s] for actor model [%s]", 
				pkBowModel->GetFileName().c_str(), GetModelFileName().c_str());

			// 添加模型加载的异步任务
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage( 
				CreateMsg_ADD_MODEL_LOADING_TASK( GD_ClientAsyncModelManager::AMT_ITEM, 
				pkBowModel, pkBowModel->GetFileName(), 0, GetShaderIdentifier() ), true );
		}

		if ( !isMessageSend )
		{
			T_ASSERT(isMessageSend);
			// 如果测试之后发现真的可能跑到这个代码里来再注释
			GD_ActorModel::_doInitBowModel(pkBowModel);
		}
	}
}

void GD_ClientActorModel::_doReleaseBowModel(GD_WeaponModel* pkBowModel)
{
	if (pkBowModel != NULL)
	{
		pkBowModel->DetachFromActor();
		_detachChildModel(pkBowModel);

		pkBowModel->PreRelease();
		
		bool isMessageSend = false;
		if ( MG_MessageCenter::GetSingletonPtr() )
		{
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage(
				CreateMsg_ADD_MODEL_UNLOADING_TASK(pkBowModel));
		}

		// 整个客户端反初始化时候会出现isMessageSend为false的情况
		if ( !isMessageSend )
		{
			GD_ActorModel::_doReleaseBowModel(pkBowModel);
		}
	}
}

void GD_ClientActorModel::_doReleaseLoadingBowModel(GD_WeaponModel* pkLoadingBowModel)
{
	if ( pkLoadingBowModel )
	{
		// 通知加载任务中止, 通知失败直接干掉
		if ( BL_AsyncOperationManager::GetSingletonPtr() && 
			BL_AsyncOperationManager::GetSingleton().IsLoadingThreadAlive() )
		{
			pkLoadingBowModel->SetReleaseFlag(true);
			BL_AsyncOperationManager::GetSingleton().NotifyInvalidModel( pkLoadingBowModel );
		}
		else
		{
			GD_ActorModel::_doReleaseLoadingBowModel(pkLoadingBowModel);
		}
	}
}

void GD_ClientActorModel::_doInitWingModel(GD_WeaponModel* pkWingModel)
{
	if ( pkWingModel )
	{
		bool isMessageSend =  false;

		if (MG_MessageCenter::GetSingletonPtr() != NULL)
		{
			GfxWriteLog(LOG_MODEL_FILELINE, LOG_SWITCH_MODEL, 
				"Send out load-weapon-model message [%s] for actor model [%s]", 
				pkWingModel->GetFileName().c_str(), GetModelFileName().c_str());

			// 添加模型加载的异步任务
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage( 
				CreateMsg_ADD_MODEL_LOADING_TASK( GD_ClientAsyncModelManager::AMT_ITEM, 
				pkWingModel, pkWingModel->GetFileName(), 0, GetShaderIdentifier() ), true );
		}

		if ( !isMessageSend )
		{
			T_ASSERT(isMessageSend);
			// 如果测试之后发现真的可能跑到这个代码里来再注释
			GD_ActorModel::_doInitWingModel(pkWingModel);
		}
	}
}

void GD_ClientActorModel::_doReleaseWingModel(GD_WeaponModel* pkWingModel)
{
	if (pkWingModel != NULL)
	{
		pkWingModel->DetachFromActor();
		_detachChildModel(pkWingModel);

		pkWingModel->PreRelease();
		
		bool isMessageSend = false;
		if ( MG_MessageCenter::GetSingletonPtr() )
		{
			isMessageSend = MG_MessageCenter::GetSingleton().SendMessage(
				CreateMsg_ADD_MODEL_UNLOADING_TASK(pkWingModel));
		}

		// 整个客户端反初始化时候会出现isMessageSend为false的情况
		if ( !isMessageSend )
		{
			GD_ActorModel::_doReleaseWingModel(pkWingModel);
		}
	}
}

void GD_ClientActorModel::_doReleaseLoadingWingModel(GD_WeaponModel* pkLoadingWingModel)
{
	if ( pkLoadingWingModel )
	{
		// 通知加载任务中止, 通知失败直接干掉
		if ( BL_AsyncOperationManager::GetSingletonPtr() && 
			BL_AsyncOperationManager::GetSingleton().IsLoadingThreadAlive() )
		{
			pkLoadingWingModel->SetReleaseFlag(true);
			BL_AsyncOperationManager::GetSingleton().NotifyInvalidModel( pkLoadingWingModel );
		}
		else
		{
			GD_ActorModel::_doReleaseLoadingBowModel(pkLoadingWingModel);
		}
	}
}

bool GD_ClientActorModel::_doIsColorAffectedByFx(void)
{
	return m_bIsPlayer && g_settings.LOD_CHAR_SHADER >= 2;
}

void GD_ClientActorModel::_initLightEffectAndShadow(void)
{
    SE_SceneManager* pkSceneMgr = SE_SceneManager::GetSingletonPtr();
	if (pkSceneMgr != NULL)
	{
		pkSceneMgr->AttachToLightEffect(m_spNiRootNode, false);
        m_spNiRootNode->UpdateEffects();

        if (m_bFakeShadow && !m_pkModelData->m_bDisableFakeShadow && _isExternalEnabledFakeShadow())
		{
            _attachFakeShadowFx();
		}
        else
        {
            _detachFakeShadowFx();
        }
	}
}

void GD_ClientActorModel::_uninitLightEffectAndShadow(void)
{
    SE_SceneManager* pkSceneMgr = SE_SceneManager::GetSingletonPtr();
    if (pkSceneMgr != NULL)
	{
		if (m_bFakeShadow)
		{
            _detachFakeShadowFx();
		}
	
		pkSceneMgr->DetachToLightEffect(m_spNiRootNode, false);
        m_spNiRootNode->UpdateEffects();
	}
}

void GD_ClientActorModel::_initIdleAnimationIndexList(void)
{
    if (m_lCharType <= 0)
    {
        return;
    }

    T_ASSERT(m_kAvailableFreeIdleAnimIndices.empty());

#pragma TODO("休闲待机动作切换的信息需要移到 MOD 中的 animation group 中去")
	if (m_bIsPlayer)
	{
		const CF_Buddy::DataEntry* pData = CF_Buddy::GetSingletonPtr()->GetEntryPtr(m_lCharType);
		if (pData != NULL)
		{
			StringType strIdleAnimDef = pData->_kIdleConfig.c_str();
			TStringVector strCountList = Utility::UT_BaseStringUtility::SplitString(strIdleAnimDef, '.');
			if (strCountList.size() == 8)
			{
				for (int i = 0; i < IDLE_ANIM_COUNT; ++i)
				{
					m_anIdleAnimRepeatMinNums[i] = atoi(strCountList[i * 2].c_str());
					m_anIdleAnimRepeatMaxNums[i] = atoi(strCountList[i * 2 + 1].c_str());
				}
			}
			else
			{
				GfxWriteLog(LOG_TAG_WARNING, LOG_SWITCH_WARNING, 
					"Idle animation definition with CharType (%d) is in the wrong format!", m_lCharType);
			}
		}
		else
		{
			GfxWriteLog(LOG_TAG_WARNING, LOG_SWITCH_WARNING, 
				"Cannot find idle animation definition with CharType (%d)!", GetCharType());
		}
	}
	else
	{
		const CF_CharType::DataEntry* pData = CF_CharType::GetSingletonPtr()->GetEntryPtr(m_lCharType);
		if (pData != NULL)
		{
			StringType strIdleAnimDef = pData->_kIdleConfig.c_str();
			TStringVector strCountList = Utility::UT_BaseStringUtility::SplitString(strIdleAnimDef, '.');
			if (strCountList.size() == 8)
			{
				for (int i = 0; i < IDLE_ANIM_COUNT; ++i)
				{
					m_anIdleAnimRepeatMinNums[i] = atoi(strCountList[i * 2].c_str());
					m_anIdleAnimRepeatMaxNums[i] = atoi(strCountList[i * 2 + 1].c_str());
				}
			}
			else
			{
				GfxWriteLog(LOG_TAG_WARNING, LOG_SWITCH_WARNING, 
					"Idle animation definition with CharType (%d) is in the wrong format!", m_lCharType);
			}
		}
		else
		{
			GfxWriteLog(LOG_TAG_WARNING, LOG_SWITCH_WARNING, 
				"Cannot find idle animation definition with CharType (%d)!", GetCharType());
		}
	}
   
    _updateIdleAnimationIndexList();
}

void GD_ClientActorModel::_updateIdleAnimationIndexList(void)
{
    StringType strNormalIdleAnimName = IsPlayer() 
		? LC_ClientHelp::GetWeaponedResName(m_eWeaponType, IDLE_ANIM_NAMES[0], true)
		: IDLE_ANIM_NAMES[0];
    if (_getAnimation(strNormalIdleAnimName) == NULL)
    {
        GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, 
            "There's no \'%s\' animation in actor model [%s]", strNormalIdleAnimName.c_str(), m_kModFile.GetFileName().c_str());

        m_anIdleAnimRepeatMinNums[0] = 0;
        m_anIdleAnimRepeatMaxNums[0] = 0;
    }

    m_kAvailableFreeIdleAnimIndices.clear();

    for (int i = 1; i < IDLE_ANIM_COUNT; ++i)
    {
        StringType strIdleAnimName = IsPlayer() 
			? LC_ClientHelp::GetWeaponedResName(m_eWeaponType, IDLE_ANIM_NAMES[i], true) 
            : IDLE_ANIM_NAMES[i];
        if (_getAnimation(strIdleAnimName) != NULL)
        {
            m_kAvailableFreeIdleAnimIndices.push_back(i);
        }
        else
        {
            m_anIdleAnimRepeatMinNums[i] = 0;
            m_anIdleAnimRepeatMaxNums[i] = 0;
        }
    }
}

void GD_ClientActorModel::_changeToNextIdleAnimation(void)
{
    --m_nCurrIdleAnimRemain;
    if (m_nCurrIdleAnimRemain >= 0)
    {
        if (m_iCurrIdleAnimIndex != 0)
        {
            _changeAnimationType(AT_IDLE_ANIM_GROUP);
            PlayAnimation(IDLE_ANIM_NAMES[m_iCurrIdleAnimIndex], true);
        }
        return;
    }

    // 之前的待机动作不为普通待机，则转入普通待机
    // 如果没有休闲待机，则一直播放普通待机
    if (m_iCurrIdleAnimIndex != 0 || m_kAvailableFreeIdleAnimIndices.empty())
    {
        m_iCurrIdleAnimIndex = 0;
    }
    // 随机播放一个存在的休闲待机
    else
    {
        int iFreeIdleAnimIndex = UT_MathBase::RandInRangeInt(1, (int)m_kAvailableFreeIdleAnimIndices.size());

        m_iCurrIdleAnimIndex = m_kAvailableFreeIdleAnimIndices[iFreeIdleAnimIndex - 1];
    }
    
    m_nCurrIdleAnimRemain = UT_MathBase::RandInRangeInt(
        m_anIdleAnimRepeatMinNums[m_iCurrIdleAnimIndex], m_anIdleAnimRepeatMaxNums[m_iCurrIdleAnimIndex]);
    --m_nCurrIdleAnimRemain;

    _changeAnimationType(AT_IDLE_ANIM_GROUP);
    PlayAnimation(IDLE_ANIM_NAMES[m_iCurrIdleAnimIndex]);
}

void GD_ClientActorModel::_changeToNextWeaponedIdleAnimation(void)
{
    --m_nCurrIdleAnimRemain;
    if (m_nCurrIdleAnimRemain >= 0)
    {
        if (m_iCurrIdleAnimIndex != 0)
        {
            _changeAnimationType(AT_WEAPONED_IDLE_ANIM_GROUP);
			StringType	animName = LC_ClientHelp::GetWeaponedResName(m_eWeaponType, IDLE_ANIM_NAMES[m_iCurrIdleAnimIndex], true);
            PlayAnimation(animName, true);
        }
        return;
    }

    // 之前的待机动作不为普通待机，则转入普通待机
    // 如果没有休闲待机，则一直播放普通待机
    if (m_iCurrIdleAnimIndex != 0 || m_kAvailableFreeIdleAnimIndices.empty())
    {
        m_iCurrIdleAnimIndex = 0;
    }
    // 随机播放一个存在的休闲待机
    else
    {
        int iFreeIdleAnimIndex = UT_MathBase::RandInRangeInt(1, (int)m_kAvailableFreeIdleAnimIndices.size());

        m_iCurrIdleAnimIndex = m_kAvailableFreeIdleAnimIndices[iFreeIdleAnimIndex - 1];
    }

    m_nCurrIdleAnimRemain = UT_MathBase::RandInRangeInt(
        m_anIdleAnimRepeatMinNums[m_iCurrIdleAnimIndex], m_anIdleAnimRepeatMaxNums[m_iCurrIdleAnimIndex]);
    --m_nCurrIdleAnimRemain;

    _changeAnimationType(AT_WEAPONED_IDLE_ANIM_GROUP);
	StringType	animName = LC_ClientHelp::GetWeaponedResName(m_eWeaponType, IDLE_ANIM_NAMES[m_iCurrIdleAnimIndex], true);
    PlayAnimation(animName);
}

void GD_ClientActorModel::_changeToNextWeaponedIdleAnimationWithHobby(void)
{
	--m_nCurrIdleAnimRemain;
	if (m_nCurrIdleAnimRemain >= 0)
	{
		if (m_iCurrIdleAnimIndex == -1 && !m_strCurrHobbyIdleAnim.empty())
		{
			_changeAnimationType(AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP);
			PlayAnimation(m_strCurrHobbyIdleAnim, true);
		}
		else if (m_iCurrIdleAnimIndex != 0)
		{
			_changeAnimationType(AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP);
			StringType	animName = LC_ClientHelp::GetWeaponedResName(m_eWeaponType, IDLE_ANIM_NAMES[m_iCurrIdleAnimIndex], true);
			PlayAnimation(animName, true);
		}
		return;
	}

	// 之前的待机动作不为普通待机，则转入普通待机
	// 如果没有休闲待机和爱好待机，则一直播放普通待机
	if (m_iCurrIdleAnimIndex != 0 || (m_kAvailableFreeIdleAnimIndices.empty() && m_strCurrHobbyIdleAnim.empty()))
	{
		m_iCurrIdleAnimIndex = 0;
	}
	// 在休闲和爱好中选一个, 其权重目前设置成1:1, 以后可以通过外部参数来调整
	else
	{
		//如果没有爱好待机, 则播放休闲待机
		if (m_strCurrHobbyIdleAnim.empty())
		{
			int iFreeIdleAnimIndex = UT_MathBase::RandInRangeInt(1, (int)m_kAvailableFreeIdleAnimIndices.size());

			m_iCurrIdleAnimIndex = m_kAvailableFreeIdleAnimIndices[iFreeIdleAnimIndex - 1];
		}
		else //在休闲和爱好中选一个
		{
			int n = g_settings.HOBBY_WEIGHT;
			if (n > 100) n = 100; else if (n < 0) n = 0;
			if (UT_MathBase::RandInRangeInt(1, 100) > n) //休闲待机
			{
				int iFreeIdleAnimIndex = UT_MathBase::RandInRangeInt(1, (int)m_kAvailableFreeIdleAnimIndices.size());

				m_iCurrIdleAnimIndex = m_kAvailableFreeIdleAnimIndices[iFreeIdleAnimIndex - 1];
			}
			else //爱好待机
			{
				m_iCurrIdleAnimIndex = -1; //播放爱好待机时, 设置休闲和普通待机的index为负
				m_nCurrIdleAnimRemain = 1; //爱好待机目前只播放一次				
			}
		}
	}

	if (m_iCurrIdleAnimIndex != -1)
	{
		m_nCurrIdleAnimRemain = UT_MathBase::RandInRangeInt(
			m_anIdleAnimRepeatMinNums[m_iCurrIdleAnimIndex], m_anIdleAnimRepeatMaxNums[m_iCurrIdleAnimIndex]);
	}

	--m_nCurrIdleAnimRemain;

	_changeAnimationType(AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP);
	if (m_iCurrIdleAnimIndex == -1)
	{
		SetWeaponVisibility(false);
		PlayAnimation(m_strCurrHobbyIdleAnim);
	}
	else 
	{
		SetWeaponVisibility(true);
		StringType	animName = LC_ClientHelp::GetWeaponedResName(m_eWeaponType, IDLE_ANIM_NAMES[m_iCurrIdleAnimIndex], true);
		PlayAnimation(animName);
	}
}

bool GD_ClientActorModel::IsPlayingHobbyAnimation()
{
	return (m_iCurrIdleAnimIndex == -1 && !m_strCurrHobbyIdleAnim.empty() && m_eCurrentAnimType == AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP);
}

void GD_ClientActorModel::_changeToNextAttackAnimation(void)
{
    // TODO

    _changeAnimationType(AT_ATTACK_ANIM_GROUP);
    //PlayAnimation();
}

void GD_ClientActorModel::_changeToNextNormalAttackGroupAnimation()
{
	int iIndex = UT_MathBase::RandInRangeInt(1,2);
	_changeAnimationType(AT_ATTACK_NORMAL_GROUP);
	PlayAnimation(iIndex == 1 ? RES_ANM_ATTACK_1 : RES_ANM_ATTACK_2, true);
}

void GD_ClientActorModel::_changeToNextNormalAttack1Animation()
{
	_changeAnimationType(AT_ATTACK_NORMAL_1);
	PlayAnimation(RES_ANM_ATTACK_1,true);
}

void GD_ClientActorModel::_changeToNextDisplayIdleAnimation()
{
	_changeAnimationType(AT_DISPLAY_GROUP);
	if ( m_iCurrDisplayAnimIndex >= g_settings.SEQ2_COUNT1)
	{
		PlayAnimation( LC_ClientHelp::GetWeaponedResName(m_eWeaponType, RES_ANM_ACTOR_IDLE_IDEL0, true) , true);
	}
	else if ( m_iCurrDisplayAnimIndex >= 0 )
	{
		PlayAnimation( LC_ClientHelp::GetWeaponedResName(m_eWeaponType, RES_ANM_ACTOR_IDLE_NORMAL, true) , true);
	}

	m_iCurrDisplayAnimIndex++;
	if ( m_iCurrDisplayAnimIndex >= g_settings.SEQ2_COUNT2 + g_settings.SEQ2_COUNT1 )
	{
		m_iCurrDisplayAnimIndex = 0;
	} 
}

void GD_ClientActorModel::_changeToNextDisplayNPCIdleAnimation()
{
	_changeAnimationType(AT_DISPLAY_NPC_GROUP);
	if ( m_iCurrDisplayAnimIndex >= g_settings.SEQ2_COUNT1)
	{
		PlayAnimation( RES_ANM_SEQ2 , true);
	}
	else if ( m_iCurrDisplayAnimIndex >= 0 )
	{
		PlayAnimation( RES_ANM_SEQ1 , true);
	}

	m_iCurrDisplayAnimIndex++;
	if ( m_iCurrDisplayAnimIndex >= g_settings.SEQ2_COUNT1 + g_settings.SEQ2_COUNT2)
		m_iCurrDisplayAnimIndex = 0;
}

void GD_ClientActorModel::_changeToNextDisplayNPCIdle3Animation()
{
	_changeAnimationType(AT_DISPLAY_NPC_GROUP3);
	if ( m_iCurrDisplayAnimIndex >=  g_settings.SEQ3_COUNT2 + g_settings.SEQ3_COUNT3)
	{
		PlayAnimation( RES_ANM_SEQ3 , true);
	}
	else if ( m_iCurrDisplayAnimIndex >= g_settings.SEQ3_COUNT1 )
	{
		PlayAnimation( RES_ANM_SEQ2 , true);
	}
	else if ( m_iCurrDisplayAnimIndex >= 0 )
	{
		PlayAnimation( RES_ANM_SEQ1 , true);
	}
	
	m_iCurrDisplayAnimIndex++;
	if ( m_iCurrDisplayAnimIndex >= g_settings.SEQ3_COUNT1 + g_settings.SEQ3_COUNT2 + g_settings.SEQ3_COUNT3)
		m_iCurrDisplayAnimIndex = 0;
}

#ifdef _ENABLE_PHYSX 
bool GD_ClientActorModel::_addPhysXPropToSceneManager(void)
{
    if (m_pkModelContent == NULL)
    {
        return false;
    }

    NiPhysXPropPtr spNiPhysXProp = m_pkModelContent->GetNiPhysXProp();
    if (spNiPhysXProp == NULL)
    {
#pragma TODO("要在资源同步工具里面检查")
        GfxWriteLog(LOG_RES_FILELINE, LOG_SWITCH_RES, 
			"找不到 NiPhysXProp [%s]", m_kModFile.GetFileName().c_str());
        return false;
    }

    m_bHasAddedPhyXPropToSceneMgr = SE_SceneManager::GetSingleton().AddPhyxProp(spNiPhysXProp);

    return m_bHasAddedPhyXPropToSceneMgr;
}

bool GD_ClientActorModel::_removePhysXPropFromSceneManager(void)
{
    if (m_pkModelContent == NULL)
    {
        return false;
    }

    NiPhysXPropPtr spNiPhysXProp = m_pkModelContent->GetNiPhysXProp();
    if (!m_bHasAddedPhyXPropToSceneMgr || spNiPhysXProp == NULL)
    {
        return false;
    }

    if (SE_SceneManager::GetSingletonPtr() != NULL)
    {
        SE_SceneManager::GetSingleton().RemovePhyxProp(spNiPhysXProp);
    }
    m_bHasAddedPhyXPropToSceneMgr = false;

    return true;
}

bool GD_ClientActorModel::CanActiveRagDoll( void )
{
	int iAnim = _getAnimationID(RES_ANM_ACTOR_PHYSICS);
	if ( iAnim < 0 )
	{
		return false;
	}
	NiActorManager::SequenceID eTargetAnim = (NiActorManager::SequenceID)iAnim;
	return (m_pkModelContent != NULL && m_pkModelContent->GetNiPhysXProp() != NULL );
}
#endif

void GD_ClientActorModel::SetVehicleModel(GD_ClientActorModel* pkVehicleModel)
{
    m_pkVehicleModel = pkVehicleModel;
    if (m_pkVehicleModel != NULL)
    {
        m_pkVehicleModel->SetIsVehicleModel(true);

        if (m_pkVehicleModel->IsUpdatable())
        {
            SetIsUpdatable(true);
        }
    }
}

//void GD_ClientActorModel::_applyForce(float fCurrentTime)
//{
//	if(!m_pkModelContent)
//		return;
//	if(!m_bForceApply)
//		return;
//
//	if(fCurrentTime - m_fForceStartTime > m_fForceApplyTime)
//	{
//		m_bForceApply = false;	
//		return;
//	}
//
//	NiPhysXPropPtr spPhysXProp = m_pkModelContent->GetNiPhysXProp();
//	if(spPhysXProp)
//	{
//		NiPhysXPropDesc* pkSnapshot = spPhysXProp->GetSnapshot();
//		NiPhysXActorDesc* pkActorDesc = pkSnapshot->GetActorAt(0);
//		NIASSERT(pkActorDesc);
//		NxActor* pkActor = pkActorDesc->GetActor();
//		pkActor->addForceAtPos(NxVec3(m_kForce.x,m_kForce.y,m_kForce.z),NxVec3(0,0,0), NX_FORCE);
//	}
//}

void GD_ClientActorModel::_setCurrentAreaEffect(const NiPoint3& kTranslate)
{
    if (m_aspFxDummyNiNode[FDT_GROUND] == NULL)
    {
        return;
    }

    MTF_EffectTile::AreaEffectType eCurrentAreaEffectType = 
        SE_SceneManager::GetSingleton().GetLocationAreaEffect(UT_EngineMath::ConvertNiPoint3ToVec3Int(kTranslate));

    T_ASSERT(eCurrentAreaEffectType >= MTF_EffectTile::AET_UNKNOWN && eCurrentAreaEffectType < MTF_EffectTile::AET_MAX_COUNT);

    if (eCurrentAreaEffectType == MTF_EffectTile::AET_WATER)
    {
        if (m_fWaterAreaEffectHeight < -9999.0f)
        {
            m_fWaterAreaEffectHeight = m_aspFxDummyNiNode[FDT_GROUND]->GetWorldTranslate().z;
        }
    }
    else
    {
        m_fWaterAreaEffectHeight = -10000.0f;
    }

    // 特效仅在地表效果类型改变时才做切换
    if (eCurrentAreaEffectType != m_eLastAreaEffectType)
    {
        if (!m_astrAreaEffectFxNames[m_eLastAreaEffectType].empty())
        {
            DetachFx(FDT_GROUND, m_astrAreaEffectFxNames[m_eLastAreaEffectType]);
        }

        if (!m_astrAreaEffectFxNames[eCurrentAreaEffectType].empty())
        {
            AttachFx(FDT_GROUND, 0, false, m_astrAreaEffectFxNames[eCurrentAreaEffectType]);

            if (eCurrentAreaEffectType == MTF_EffectTile::AET_WATER)
            {
                AttachedFxMap& rkAttachedFxMap = m_kAttachedFxMap[FDT_GROUND];
                AttachedFxMap::iterator itrAttachedFx = 
                    rkAttachedFxMap.find(m_astrAreaEffectFxNames[eCurrentAreaEffectType]);
                if (itrAttachedFx != rkAttachedFxMap.end())
                {
                    m_iCurrentAreaEffectFxId = itrAttachedFx->second;
                }
            }
        }
    }
 
	// 音效每次动作结束都重新播放，以对齐步点
//     if (!m_astrAreaEffectSoundNames[m_eLastAreaEffectType].empty())
//     {
// 		//g_soundMgr.StopAudioSource( m_iCurrentAreaEffectSoundId );
// 		//tick = 0;
// 		/*GfxWriteLog( LOG_SOUND_FILELINE , LOG_SWITCH_SOUND ,
// 			"[%f]Stop Area Sound ID %d" ,NiGetCurrentTimeInSec(),
// 			m_iCurrentAreaEffectSoundId);*/
//     }

	//现在先修改成每隔0.4s播放一次脚本音效
//     if (!m_astrAreaEffectSoundNames[eCurrentAreaEffectType].empty())
//     {
// 		if (GET_PROCESS_TIME - m_fStepSoundTick > 0.4f )
// 		{
// 			g_soundMgr.StopAudioSource( m_iCurrentAreaEffectSoundId );
// 			//m_iCurrentAreaEffectSoundId = g_soundMgr.Play2D(
// 			//	m_astrAreaEffectSoundNames[eCurrentAreaEffectType],
// 			//	AS_ClientSoundManager::SCT_AREA_EFFECT , false , 1 );
// 			//m_iCurrentAreaEffectSoundId = g_soundMgr.PlayAreaEffect(m_astrAreaEffectSoundNames[eCurrentAreaEffectType]);
// 			m_iCurrentAreaEffectSoundId = g_soundMgr.PlayAreaEffectPos(m_astrAreaEffectSoundNames[eCurrentAreaEffectType], kTranslate);
// 			GfxWriteLog( LOG_SOUND_FILELINE , LOG_SWITCH_SOUND ,
// 				"[%f]Play Area Sound %s with ID %d" ,NiGetCurrentTimeInSec(),
// 				m_astrAreaEffectSoundNames[eCurrentAreaEffectType].c_str() , m_iCurrentAreaEffectSoundId);
// 			m_fStepSoundTick = GET_PROCESS_TIME;
// 		}
//     }

    // 水面水花高度特殊处理
    if (eCurrentAreaEffectType == MTF_EffectTile::AET_WATER && m_iCurrentAreaEffectFxId >= 0)
    {
        NiPoint3 kFxXlate(0.0f, 0.0f, m_fWaterAreaEffectHeight - m_aspFxDummyNiNode[FDT_GROUND]->GetWorldTranslate().z);

        _bindFxToDummyNode(m_iCurrentAreaEffectFxId, m_aspFxDummyNiNode[FDT_GROUND], 0, false, &kFxXlate, NULL, 1.0f);
    }

    if (eCurrentAreaEffectType != m_eLastAreaEffectType)
    {
        m_eLastAreaEffectType = eCurrentAreaEffectType;
    }
}

bool GD_ClientActorModel::_isExternalEnabledFakeShadow(void)
{
	return ((!g_settings.CFG_ENABLE_SCENE_OBJECT_SHADOW) || GfxGetWorldConfig().CFG_BAKING_TYPE != Terrain::TS_WorldConfig::NORMAL);
}

void GD_ClientActorModel::_attachFakeShadowFx(void)
{
    //m_kFakeShadowColor = pkSceneMgr->GetCurrentShadowColor();
    AttachFx(m_eCurrentFakeShadowDummyNode, MTF_ModelData::AFBM_TRANSLATE, false, RES_FX_FAKE_SHADOW, 
        NULL, NULL, -1.0f, false, m_kFakeShadowColor);
}

void GD_ClientActorModel::_detachFakeShadowFx(void)
{
    DetachFx(m_eCurrentFakeShadowDummyNode, RES_FX_FAKE_SHADOW);
}

int GD_ClientActorModel::_extraAnimSoundEventDeal()
{
	NiPoint3 trans = GetTranslate();
	MTF_EffectTile::AreaEffectType eCurrentAreaEffectType = 
		SE_SceneManager::GetSingleton().GetLocationAreaEffect(UT_EngineMath::ConvertNiPoint3ToVec3Int(trans));
	
	//g_soundMgr.StopAudioSource( m_iCurrentAreaEffectSoundId );
	if ( m_astrAreaEffectSoundNames[eCurrentAreaEffectType].empty() )
	{
		m_iCurrentAreaEffectSoundId = INVALID_ID_RULE_FAIL;
	}
	else if ( IsPlayer() || (GetCharType() >= 1001 && GetCharType() <= 1020) || (GetCharType() >= 1051 && GetCharType() <= 1060) )	//如果是玩家或者是坐骑的CharType，就用玩家技能的声道
		m_iCurrentAreaEffectSoundId = g_soundMgr.PlayAreaEffectPos(m_astrAreaEffectSoundNames[eCurrentAreaEffectType], trans, AS_ClientSoundManager::SCT_PLAYER_SKILL);
	else
		m_iCurrentAreaEffectSoundId = g_soundMgr.PlayAreaEffectPos(m_astrAreaEffectSoundNames[eCurrentAreaEffectType], trans,AS_ClientSoundManager::SCT_MONSTER_SKILL);

	return m_iCurrentAreaEffectSoundId;
}