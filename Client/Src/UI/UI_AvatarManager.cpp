//---------------------------------------------------------------------------------------------------
#include "ClientPCH.h"
#include "MG_MessageBase.h"
#include "UI_AvatarManager.h"
#include "UI_AvatarRender.h"
#include "FX_FxManager.h"
#include "GD_ModelBase.h"
#include "GD_ClientActorModel.h"
#include "GD_ClientAsyncModelManager.h"
#include "CF_NPCAvatar.h"
#include "CF_WildPrinces.h"
#include "CF_Subordinate.h"
#include "CF_BuddyTransform.h"
#include "CF_Pet.h"
#include "CF_Bot.h"
#include "LC_ClientItem.h"
#include "LC_ClientGameEvent.h"
#include "LC_GuildRaidDataManager.h"
#include "UT_CutSceneManager.h"
#include "RG_AvatarRenderClick.h"
#include "FX_ClientFxManager.h"
#include "UI_UIManager.h"
#include "GlobalSettings.h"

using namespace CSVFile;
using namespace Data;
using namespace GameLogic;

//---------------------------------------------------------------------------------------------------
#define CHARACTER_WIDTH                 1024
#define CHARACTER_HEIGHT                1024
#define FULL_CHAR_WIDTH                 720
#define FULL_CHAR_HEIGHT                492
#define VEHICLE_WIDTH	                1100
#define VEHICLE_HEIGHT                  650
#define FULL_CHAR_WIDTH2                167
#define FULL_CHAR_HEIGHT2               291
#define RUNESCAPE_WIDTH					770
#define RUNESCAPE_HEIGHT				430
#define HEAD_WIDTH                      90
#define HEAD_HEIGHT                     100
#define HEAD_WIDTH2                     46
#define HEAD_HEIGHT2                    54
#define ESC_UI_CHAR_WIDTH				1200
#define ESC_UI_CHAR_HEIGHT				1200
#define WORLDMAP_WIDTH					1500
#define WORLDMAP_HEIGHT					1024
#define GAMESTORY_WILDBOSS_WIDTH		720
#define GAMESTORY_WILDBOSS_HEIGHT		492
#define RUNESCAPE_TIP_WIDTH				600
#define RUNESCAPE_TIP_HEIGHT			470
#define FRIEND_TIP_WIDTH				358
#define FRIEND_TIP_HEIGHT				270
#define ARENA_WIDTH						900
#define ARENA_HEIGHT					492
#define TREASURE_WIDTH					510
#define TREASURE_HEIGHT					350
#define REWARD_RANK_WIDTH				425
#define REWARD_RANK_HEIGHT				400
#define GREATKUNGFU_WIDTH				1012
#define GREATKUNGFU_HEIGHT				656
#define FUNCTION_PRIVIEW_WIDTH			256
#define FUNCTION_PRIVIEW_HEIGHT			300
#define SKIN_WIDTH						300
#define SKIN_HEIGHT						177
#define ACHIVEMENT_WIDTH				128
#define ACHIVEMENT_HEIGHT				128
#define TOYPET_WIDTH					512
#define TOYPET_HEIGHT					512
#define HOOKBOT_WIDTH					280
#define HOOKBOT_HEIGHT					210
#define COREBOSS_WIDTH					150
#define COREBOSS_HEIGHT					200
#define TIMELIMIT_WIDTH					768
#define TIMELIMIT_HEIGHT				768
#define SENDFLOWER_WIDTH				320
#define SENDFLOWER_HEIGHT				400

#define SCALE                           1.0f

//---------------------------------------------------------------------------------------------------
const float UI_AvatarManager::UPDATE_RENDER_TIME_INTERVAL = 1.0f / 30.0f;
const float UI_AvatarManager::UPDATE_BOT_SHOOT_TIME_INTERVAL = 1.0f;

const char* UI_AvatarManager::ACTOR_WITH_VEHICLE_ANIM_NAME	= RES_ANM_ACTOR_RIDE_IDLE_HAND;
const char* UI_AvatarManager::VEHICLE_ANIM_NAME				= RES_ANM_ACTOR_IDLE0;

//---------------------------------------------------------------------------------------------------
UI_AvatarManager::UI_AvatarManager(void)
: m_fLastUpdateTime(-UPDATE_RENDER_TIME_INTERVAL)
, m_fLastShootUpdateTime(-UPDATE_BOT_SHOOT_TIME_INTERVAL)
, m_bChangeStage(false)
{
	
}

UI_AvatarManager::~UI_AvatarManager(void)
{
	
}

void UI_AvatarManager::Init()
{
	m_pkRender = T_NEW_D UI_AvatarRender(this);

	_loadSceneMaps();
	_loadCameraSetting();
	_loadViewPorts(m_pkRender);
	_loadGUIImages(m_pkRender);
}

void UI_AvatarManager::PreUnInit()
{
	// 和UI系统相关的部分先释放掉；
	if ( m_pkRender )
	{
		m_pkRender->DestroyAllImages();
		m_pkRender->DestroyMaterial();
	}
}


void UI_AvatarManager::UnInit()
{
	// 和UI系统无关的部分在UI系统释放之后释放
	// 这样就不会产生引起renderTexture引用计数报错的问题。
	_destroyAllAvatarElements();
	_destroyAllAvatarBullets();

	// 清除场景
	_unLoadSceneMaps();

	// 删除渲染器
	if ( m_pkRender )
	{
		T_DELETE_D(m_pkRender);
		m_pkRender = NULL;
	}
}

void UI_AvatarManager::Reset()
{
	_destroyAllAvatarElements();
	_destroyAllAvatarBullets();
}

void UI_AvatarManager::Update( MG_MessageBase& rkMessage )
{
	float fCurrentTime = rkMessage.m_fParam1;
	float fDeltaTime = rkMessage.m_fParam2;

	if ( m_pkRender && 
		m_pkRender->isInitSuccess()  )
	{
		m_pkRender->UpdateObjects(fCurrentTime, fDeltaTime);

		if ( !m_pkRender->IsRenderTextureMsaaEnable() )
		{
			m_fLastUpdateTime = fCurrentTime;
			m_bIsElapsedRenderInterval = true;
		}
		else
		{
			if (fCurrentTime - m_fLastUpdateTime >= UPDATE_RENDER_TIME_INTERVAL)
			{
				m_fLastUpdateTime = fCurrentTime;
				m_bIsElapsedRenderInterval = true;
			}
		}

		_updateHookBotShoot(fCurrentTime);
		_updateHookBotState(fCurrentTime);
		_updateBullets(fCurrentTime, fDeltaTime);
	}
}

void UI_AvatarManager::_updateBullets(float fCurrentTime, float fDeltaTime)
{
	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();
	for(AvatarBulletList::iterator iter = m_kBulletList.begin(); iter!=m_kBulletList.end();)
	{
		AvatarBullet* pkBullet = *iter;
		const NiPoint3& kPos = pkBullet->pkFxDummyNode->GetWorldTranslate();
		AvatarElementPtr pkEle = NULL;
		if(pkBullet->uiKeyId > Invalid_AvatarElementKeyId)
			pkEle = GetAvatar(pkBullet->uiKeyId);
		
		bool bErase = false;
		if(pkEle != NULL)
		{
			GD_ClientActorModel* pkTargetModel = pkEle->pkActorModel;
			if(pkTargetModel != NULL)
			{
				const NiPoint3& kTargetLocation = pkTargetModel->GetFxDummyNodePosition(GD_ModelBase::FDT_HEART);
				int iNeedMoveDistance = UT_MathBase::LineLengthXYZInt(UT_Vec3Int(kPos.x, kPos.y, kPos.z), UT_Vec3Int(kTargetLocation.x, kTargetLocation.y, kTargetLocation.z));
				if(iNeedMoveDistance > 5)
				{
					NiPoint3 kDir = kTargetLocation - kPos;
					// 朝向
					NiMatrix3 kRotation;
					if (UT_EngineMath::LookAt(-kDir, NiPoint3::UNIT_Z, kRotation))
						pkBullet->pkFxDummyNode->SetWorldRotate(kRotation);

					pkBullet->pkFxDummyNode->SetWorldTranslate(kPos + kDir * (fCurrentTime - pkBullet->fStartTime)/ pkBullet->fFlightTime);
				}
				else
				{
					pkTargetModel->AttachFx(GD_ModelBase::FDT_HEART, 0, false, pkBullet->sBulletExplodeFx);

					int iDamage = UT_MathBase::RandInRangeInt(100, 1000);

					long lCharType = Invalid_AvatarElementCharTypeId;
					int iCharIndex = Invalid_AvatarElementCharIndex;
					CameraMode iMode = CM_HOOK_BOT_MONSTER;
					_extractAvatarElementInfoFromKeyId(pkBullet->uiKeyId, lCharType, iCharIndex, iMode);

					bool bDecrease = false;
					if(pkEle->iLife > 0)
					{
						pkEle->iLife--;
						bDecrease = true;
					}

					if(pkEle->iLife == 0)
						pkTargetModel->PlayAnimation(RES_ANM_ACTOR_DEAD);
					else
						pkTargetModel->PlayAnimation(RES_ANM_ACTOR_UNDERATTACK);
					_playDamageFxOnModel(iCharIndex, iDamage, bDecrease && pkEle->iLife == 0);

					bErase = true;
				}
			}
		}
		else
		{
			//目标不存在时，销毁子弹
			bErase = true;
		}

		if(bErase)
		{
			rkFxManager.ReleaseFx(pkBullet->iFlyingFxId, false);
			pkBullet->pkFxDummyNode = NULL;
			iter = m_kBulletList.erase(iter);
			T_SAFE_DELETE(pkBullet);
		}
		else
		{
			++iter;
		}
	}
}

void UI_AvatarManager::_playDamageFxOnModel(int iCharIndex, int iDamage, bool bDead)
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_HOOKBOT_DAMAGE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iDamage);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, iCharIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, bDead);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

bool UI_AvatarManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,	Update)
	END_MESSAGE_HANDLE
	return true;
}

void UI_AvatarManager::publicCallBack_ActiveGroup(const StringType& groupName, bool isActive)
{
	/*if ( groupName == "FunctionPreviewBtn" )
	{
		int i = 0;
	}
	else
	{
		int i = 0;
	}*/

	if ( m_pkRender )
	{
		if ( isActive )
			m_pkRender->ActiveObjects(groupName);
		else
			m_pkRender->DeactiveObjects(groupName);
	}
}

bool UI_AvatarManager::publicCallBack_IsActiveGroup(const StringType& groupName)
{
	if ( m_pkRender )
	{
		return m_pkRender->IsObjectsActived(groupName);
	}

	return false;
}

void UI_AvatarManager::publicCallBack_ReleaseAllDeactiveGroup()
{
	if ( m_pkRender )
	{
		m_pkRender->RemoveAllExpiredObjectGroup();
	}
}

void UI_AvatarManager::publicCallBack_ReleaseDeactiveGroup( int iMaxCount )
{
	if ( m_pkRender )
	{
		m_pkRender->RemoveExpiredObjectGroup( iMaxCount );
	}
}

void UI_AvatarManager::publicCallBack_SetAutoReleaseDeactiveGroupLifeScale( float fScale )
{
	if ( m_pkRender )
	{
		m_pkRender->SetObjectGroupExpiredLifeScale(fScale);
	}
}

bool UI_AvatarManager::publicCallBack_PrePerformRendering()
{
	if (// !m_bIsElapsedRenderInterval ||
		!m_pkRender ||
		!m_pkRender->isInitSuccess() ||
		!m_pkRender->IsAnyObjectActived() ||
		m_kAllAvatarElements.empty() ||
		!UT_CutSceneManager::GetSingleton().ShowUI() )
	{
		return false;
	}

	return true;
}

void UI_AvatarManager::publicCallBack_PerformRendering(RG_AvatarRenderClick* renderClick)
{
	if ( m_pkRender )
		m_pkRender->RenderObjects(renderClick);

	m_bIsElapsedRenderInterval = false;
}

void UI_AvatarManager::publicCallBack_PreRenderClickProc(NiRenderClick* renderClick)
{
	RG_AvatarRenderClick* pkAvatarRenderClick = (RG_AvatarRenderClick*)renderClick;
	RENDER_CLICK_INDEX rdClickIdx = (RENDER_CLICK_INDEX)pkAvatarRenderClick->GetIndex();
	if(rdClickIdx == RENDER_CLICK_NODEPTHBUFFER)
	{
		if ( m_pkRender )
			m_pkRender->ConfigObjectsShader(rdClickIdx, true);
	}
}

void UI_AvatarManager::publicCallBack_PostRenderClickProc(NiRenderClick* renderClick)
{
	RG_AvatarRenderClick* pkAvatarRenderClick = (RG_AvatarRenderClick*)renderClick;
	RENDER_CLICK_INDEX rdClickIdx = (RENDER_CLICK_INDEX)pkAvatarRenderClick->GetIndex();
	if(rdClickIdx == RENDER_CLICK_NODEPTHBUFFER)
	{
		if ( m_pkRender )
			m_pkRender->ConfigObjectsShader(rdClickIdx, false);
	}
}

void UI_AvatarManager::renderCallBack_Active(AvatarElementKeyId keyId)
{
	AvatarElementPtr pkAvatarElement = GetAvatar(keyId);

	if ( pkAvatarElement )
	{
		GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

		pkActorModel->SetVisibility(true);
		pkActorModel->DoDetachFxsOnAnim();
		pkActorModel->SetIsUpdatable(true);
	}
}

void UI_AvatarManager::renderCallBack_Deactive(AvatarElementKeyId keyId)
{
	AvatarElementPtr pkAvatarElement = GetAvatar(keyId);

	if ( pkAvatarElement )
	{
		GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

		pkActorModel->SetVisibility(false);
		pkActorModel->DoDetachFxsOnAnim();
		pkActorModel->SetIsUpdatable(false);
	}
}

void UI_AvatarManager::renderCallBack_Update(AvatarElementKeyId keyId, float fTime, float fDeltaTime)
{
	AvatarElementPtr pkAvatarElement = GetAvatar(keyId);

	if ( pkAvatarElement )
	{
		GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

		if ( pkActorModel->IsVisible() )
		{
			pkActorModel->SetIsUpdatable(true);
			pkActorModel->EntirelyManualUpdate(fTime, fDeltaTime);
			pkActorModel->SetIsUpdatable(false);
		}
	}
}

void UI_AvatarManager::renderCallBack_ConfigCamera(NiCameraPtr& spCamera, AvatarElementKeyId keyId)
{
	long lCharType = Invalid_AvatarElementCharTypeId;
	int iCharIndex = Invalid_AvatarElementCharIndex;
	CameraMode eCameraMode = CM_MAX_COUNT;
	_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, eCameraMode);

	switch (eCameraMode)
	{
	case CM_FULLBODY:
		T_ASSERT(iCharIndex >= 0 && iCharIndex < 7);
		if (iCharIndex >= 1 && iCharIndex < 5)
		{
			_applyCameraSetting(spCamera, CM_TEAM_FULLBODY, m_akFullBodyViewPorts[iCharIndex]);
		}
		else
		{
			_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		}
		break;
	case CM_BONESOUL:
		T_ASSERT(iCharIndex == 0);
		_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		break;
	case CM_TOYPET:
	case CM_TOYPET_GAIN:
		T_ASSERT(iCharIndex == 0);
		_applyCameraSetting(spCamera, eCameraMode,  m_kToyPet[0]);
		break;
	case CM_MUHUN:
	case CM_VEHICLE:
	case CM_DEVAEYE:
		T_ASSERT(iCharIndex == 0);
		_applyCameraSetting(spCamera, eCameraMode, m_kSubViewPorts[0]);
		//rkModel.Draw(m_spCamera, m_kCuller);
		break;
	case CM_PLOUGH:
	case CM_EVIL:
	case CM_FAN:
	case CM_SUPERBOW:
	case CM_HOOK:
	case CM_SPIRIT_FAN:
	case CM_STIGMAS:
		T_ASSERT(iCharIndex == 0);
		_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		break;
	
	case CM_FABAO:
		T_ASSERT(iCharIndex == 0);
		_applyCameraSetting(spCamera, eCameraMode, m_kSubViewPorts[0]);
		break;
	case CM_TEAM_FULLBODY:
		T_ASSERT_CRITICAL_S(iCharIndex >= 1 && iCharIndex <= 5);
		_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[iCharIndex]);
		break;
	case CM_RUNESCAPE_FULLBODY:
	case CM_RUNESCAPE_VORPAL_BLADE:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 1 && iCharIndex <= 5);
			_applyCameraSetting(spCamera, CM_RUNESCAPE_FULLBODY, m_kRunescapeViewPort[0]);
		}
		break;

	case CM_ALLIANCEMAP:
	case CM_DAILYRECHARGE_RIGHT:
	case CM_PRIVILEGE_VIP2:
		{
			T_ASSERT_CRITICAL_S(iCharIndex == 0);
			_applyCameraSetting(spCamera, eCameraMode, m_kRunescapeViewPort[0]);
		}
		break;
	case CM_RUNESCAPE_EFFECT:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 6 && iCharIndex <= 7);
			_applyCameraSetting(spCamera, eCameraMode, m_kRunescapeViewPort[0]);
		}
		break;
	case CM_RUNESCAPE_TIP:
	case CM_GODWEAPON_TIP:
		{
			T_ASSERT_CRITICAL_S(iCharIndex == 0);
			_applyCameraSetting(spCamera, eCameraMode, m_kRunescapeTipViewPort[iCharIndex]);
		}
		break;

	case CM_FLOWER:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 0 && iCharIndex < 2);
			_applyCameraSetting(spCamera, eCameraMode, m_kFlowerViewPort[0]);
		}
		break;
	case CM_ARENA_BATTLE_ACTOR:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 0 && iCharIndex <= 2);
			_applyCameraSetting(spCamera, eCameraMode, m_kRankingListViewPort[0]);
		}
		break;
	case CM_ARENA_BATTLE_ACTOR_OTHER:
		{
			T_ASSERT_CRITICAL_S(iCharIndex > 0 && iCharIndex < 4);
			_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		}
		break;
	case CM_MULTI_BUDDY:
	case CM_ROYAL_CITY:
	case CM_NONPAREIL_ITEM:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 0 && iCharIndex < 3);
			_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		}
		break;
	case CM_WILD_PRINCES_BOSS:
	case CM_DAILYRECHARGE_LEFT:
	case CM_PRIVILEGE_VIP1:
		{
			_applyCameraSetting(spCamera, eCameraMode, m_kGameStoryWildBossViewPort1[0]);
		}
		break;
	case CM_CHARACTER:
	case CM_WILD_PRINCES_BOSS_FAMILY:
	case CM_WILD_BOSS:
	case CM_GREAT_KUNGFU_REMAIN:
		{
			_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		}
		break;
	case CM_CORE_BOSS:
		_applyCameraSetting(spCamera, eCameraMode, m_kCoreBoss[0]);
		break;
	case CM_FASHION:
	case CM_SKY_ARENA:
	case CM_GEMBUFF:
	case CM_VIEWINFO:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 0 && iCharIndex < 2);
			_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		}
		break;
	case CM_RANKINGLIST:
		{
			T_ASSERT_CRITICAL_S(iCharIndex == 0);
			_applyCameraSetting(spCamera, eCameraMode, m_kRankingListViewPort[0]);
		}
		break;
	
	case CM_RAID_SETTLE:
		T_ASSERT_CRITICAL_S(iCharIndex == 0);
		_applyCameraSetting(spCamera, eCameraMode, m_kViewInfoViewPort[0]);
		break;
	case CM_TOYPET_CHOOSE:
		T_ASSERT_CRITICAL_S(iCharIndex == 0 || iCharIndex == 1);
		_applyCameraSetting(spCamera, eCameraMode, m_kViewInfoViewPort[0]);
		break;

	case CM_FRIEND_TIPS:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 0 && iCharIndex < 2);
			_applyCameraSetting(spCamera, eCameraMode, m_kFriendTipViewPort[0]);
		}
		break;
		
	case CM_VEHICLE_CRYSTAL:
		{
			T_ASSERT_CRITICAL_S(iCharIndex == 0);
			_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		}
		break;
	case CM_GREAT_KUNGFU_REMAIN2:
		{
			_applyCameraSetting(spCamera, eCameraMode, m_kSubViewPorts[0]);
		}
		break;
	case CM_SPECIALTRAIN:
		_applyCameraSetting(spCamera, eCameraMode, m_kSpecialTrain[0]);
		break;
	case CM_GREAT_KUNGFU:
		{
			T_ASSERT(false);
		}
		break;


	case CM_FUNCTION_PREVIEW_BUTTON:
	case CM_FUNCTION_PREVIEW_BUTTON2:
		{
			T_ASSERT_CRITICAL_S(iCharIndex == 0);
			_applyCameraSetting(spCamera, eCameraMode, m_kFunctionPreviewBtnViewPort[0]);
		}
		break;

	case CM_JIANGJUNFU:
		{
			T_ASSERT_CRITICAL_S(iCharIndex >= 0);
			_applyCameraSetting(spCamera, eCameraMode, m_akFullBodyViewPorts[0]);
		}
		break;
	case CM_HOOK_BOT:
	case CM_HOOK_BOT_MONSTER:
		{
			_applyCameraSetting(spCamera, eCameraMode, m_kHookBot[0]);
		}
	case CM_FIRST_RECHARGE_ADV:
		{
			_applyCameraSetting(spCamera, eCameraMode, m_kFirstRechargeAdv[0]);
		}
		break;
	case CM_AUTOPICK_ADV:
		{
			_applyCameraSetting(spCamera, eCameraMode, m_kAutoPickAdv[0]);
		}
		break;
	default:
		break;
	}
}

void UI_AvatarManager::renderCallBack_ConfigShader(int& shaderParamIndex, AvatarElementKeyId keyId)
{
	long lCharType = Invalid_AvatarElementCharTypeId;
	int iCharIndex = Invalid_AvatarElementCharIndex;
	CameraMode eCameraMode = CM_MAX_COUNT;
	_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, eCameraMode);

	shaderParamIndex = GetShaderParamIndexCameraModeMapping(eCameraMode);
}

void UI_AvatarManager::renderCallBack_ConfigObjectShader(AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx, bool bPre)
{
	if(rdClickIdx != RENDER_CLICK_NODEPTHBUFFER)
		return;

	long lCharType = Invalid_AvatarElementCharTypeId;
	int iCharIndex = Invalid_AvatarElementCharIndex;
	CameraMode eCameraMode = CM_MAX_COUNT;
	_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, eCameraMode);

	AvatarElementPtr pkAvatarElement = GetAvatar(keyId);
	if ( pkAvatarElement && pkAvatarElement->pkActorModel->IsInitedWithLoaded() )
	{
		GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

		NiColorA color;
		CF_CharType::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
		if(pkData && pkData->_OuterGlowColor.size() >= 3)
		{
			color.r = pkData->_OuterGlowColor[0]/255.0f;
			color.g = pkData->_OuterGlowColor[1]/255.0f;
			color.b = pkData->_OuterGlowColor[2]/255.0f;
			color.a = 1;
		}
		
		NiColorA kOutGlowColor = bPre ? color : NiColorA();
		pkActorModel->SetOuterGlowColor(kOutGlowColor);
	}
}

void UI_AvatarManager::renderCallBack_RenderSolidPass(NiCameraPtr& spCamera, NiCullingProcess& kCuller, AvatarElementKeyId keyId)
{
	long lCharType = Invalid_AvatarElementCharTypeId;
	int iCharIndex = Invalid_AvatarElementCharIndex;
	CameraMode eCameraMode = CM_MAX_COUNT;
	_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, eCameraMode);

	AvatarElementPtr pkAvatarElement = GetAvatar(keyId);
	if ( pkAvatarElement && 
		pkAvatarElement->pkActorModel->IsInitedWithLoaded() )
	{
		GD_ClientActorModel* pkActorModel = 
			pkAvatarElement->pkActorModel;

		switch (eCameraMode)
		{
		case CM_CHARACTER:
			{
				pkActorModel->DrawModelBase(spCamera, kCuller);

				const StringType& sAnimName = pkActorModel->GetCurrentAnimationName();
				if(sAnimName != RES_ANM_ACTOR_SHOW1 && sAnimName != RES_ANM_ACTOR_IDLE_IDEL0)
					return;
				
				float fAnimPlayTime = GET_PROCESS_TIME - pkActorModel->GetAnimationStartPlayTime();
				CF_BuddyTransform::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(lCharType);
				if(pkData != NULL)
				{
					float fAnimPlayTime = GET_PROCESS_TIME - pkActorModel->GetAnimationStartPlayTime();
					if(sAnimName == RES_ANM_ACTOR_SHOW1 && fAnimPlayTime > pkData->_showAnimLen
						|| sAnimName == RES_ANM_ACTOR_IDLE_IDEL0 && fAnimPlayTime > pkData->_standByAnimLen )
					{
						pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE_NORMAL);
					}
				}
			}
			break;
		case CM_VIEWINFO:
			{
				pkActorModel->DrawModelBase(spCamera, kCuller);
				const StringType& sAnimName = pkActorModel->GetCurrentAnimationName();
				float fAnimPlayTime = GET_PROCESS_TIME - pkActorModel->GetAnimationStartPlayTime();
				if(sAnimName == RES_ANM_CHEST_OPEN && fAnimPlayTime >= 1.4f )
				{
					pkActorModel->PlayAnimation(RES_ANM_ACTOR_BORN);
				}
				else if((sAnimName == RES_ANM_ACTOR_BORN && fAnimPlayTime >= 0.4f ))
				{
					pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE_NORMAL);
				}
			}
			break;
		case CM_RAID_SETTLE:
			{
				pkActorModel->DrawModelBase(spCamera, kCuller);

				const StringType& sAnimName = pkActorModel->GetCurrentAnimationName();
				if(sAnimName != RES_ANM_ACTOR_SHOW1)
					return;

				CF_BuddyTransform::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(lCharType);
				if(pkData != NULL)
				{
					float fAnimPlayTime = GET_PROCESS_TIME - pkActorModel->GetAnimationStartPlayTime();
					if(sAnimName == RES_ANM_ACTOR_SHOW1 && fAnimPlayTime > pkData->_showAnimLen )
					{
						pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE_NORMAL);
					}
				}
			}
			break;
		default:
			{
				pkActorModel->DrawModelBase(spCamera, kCuller); 
			}
			break;
		}
	}
}

void UI_AvatarManager::renderCallBack_RenderTransparentPass(NiCameraPtr& spCamera, NiCullingProcess& kCuller, AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx)
{
	long lCharType = Invalid_AvatarElementCharTypeId;
	int iCharIndex = Invalid_AvatarElementCharIndex;
	CameraMode eCameraMode = CM_MAX_COUNT;
	_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, eCameraMode);

	AvatarElementPtr pkAvatarElement = GetAvatar(keyId);
	if ( pkAvatarElement && 
		pkAvatarElement->pkActorModel->IsInitedWithLoaded() )
	{
		GD_ClientActorModel* pkActorModel = 
			pkAvatarElement->pkActorModel;

		switch (eCameraMode)
		{
		case CM_HOOK_BOT:
			{
				FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();
				FX::FXIDList  kFxIds;	kFxIds.reserve(32);
				for(AvatarBulletList::iterator iter = m_kBulletList.begin(); iter!=m_kBulletList.end(); ++iter)
				{
					AvatarBullet* pkBullet = *iter;
					kFxIds.push_back(pkBullet->iFlyingFxId);
				}
				rkFxManager.ManualRenderList(kFxIds);
				pkActorModel->DrawAttachedFXs(spCamera, kCuller);
			}
			break;

		default:
			{
				if(rdClickIdx != RENDER_CLICK_NODEPTHBUFFER)
					pkActorModel->DrawAttachedFXs(spCamera, kCuller);
			}
			break;
		}
	}
}

bool UI_AvatarManager::IsShouldRender(AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx)
{
	AvatarElementPtr pkAvatarElement = GetAvatar(keyId);
	if ( pkAvatarElement)
	{
		return (rdClickIdx & pkAvatarElement->iRenderClickIndex) > 0;
	}

	return false;
}

void UI_AvatarManager::renderCallBack_RenderGroupExpired(const AvatarElementKeyIdList& kElementKeyIdList)
{
	_destroyExpiredAvatarElements(kElementKeyIdList);
}

bool UI_AvatarManager::UpdateActorScene(const StringType& strGroupName, long lCid, int iCharIndex, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_ACTOR;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 处理占用了当前位置的avatarElement;
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCid );
	if ( isNeedRecreate )
	{
		AvatarElementKeyId lastkeyId = _generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);
		AvatarElementPtr pkAvatarElement = GetAvatar(lastkeyId);

		// TODO: preDestroy
		if ( pkAvatarElement )
		{
			if (eCameraMode == CM_TREASURE_CHESTS && iCharIndex >= 0 && iCharIndex < 10)
			{
				//测试看代码会不会跑进来这里;
				T_ASSERT(false);
				//NiNodePtr pt = pkAvatarElement->GetFxDummyNode(Data::GD_ModelBase::FDT_GROUND);
				//if (pt->GetParent())  pt->GetParent()->DetachChild(pt);
			}
		}

		// 销毁
		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	if ( lCid <= 0
		|| (iCharIndex < 0 || iCharIndex >= MAX_AVATAR_ACTOR_IN_GAME)
		|| (eCameraMode > CM_MAX_COUNT))
	{
		return false;
	}

	AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCid, iCharIndex, (CameraMode)eCameraMode);
	AvatarElementPtr pkAvatarElement = isNeedRecreate ? _createActorAvatarElement(keyId, eSceneMapIndex, strGroupName) : GetAvatar(keyId);
	if ( !pkAvatarElement )
		return false;
	GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;
	if ( !pkActorModel )
		return false;

	pkActorModel->SetVisibility(true);

	if ( eCameraMode == CM_TEAM_FULLBODY 
		|| eCameraMode == CM_ARENA_BATTLE_ACTOR 
		|| eCameraMode == CM_ARENA_BATTLE_ACTOR_OTHER
		|| eCameraMode == CM_VIEWINFO
		|| eCameraMode == CM_RANKINGLIST
		|| eCameraMode == CM_FLOWER
		|| eCameraMode == CM_RUNESCAPE_TIP
		|| eCameraMode == CM_CHARACTER
		|| eCameraMode == CM_GREAT_KUNGFU_REMAIN
		|| eCameraMode == CM_MULTI_BUDDY
		|| eCameraMode == CM_ROYAL_CITY
		|| eCameraMode == CM_RAID_SETTLE)
	{

		int iCharType = lCid;

		switch(eCameraMode )
		{
		case CM_CHARACTER:
		case CM_GREAT_KUNGFU_REMAIN:
		case CM_RANKINGLIST:
		case CM_FLOWER:
		case CM_RUNESCAPE_TIP:
		case CM_RAID_SETTLE:
		case CM_TEAM_FULLBODY:
			{
				const CF_BuddyTransform::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(lCid);
				if(pkEntry != NULL)
				{
					iCharType = pkEntry->_buddy;
					NiPoint3 kPosition = NiPoint3::ZERO;
					float fDistance = 0;
					Utility::UT_SIMDataList::const_iterator cit = pkEntry->_Position.begin();
					for (; cit!=pkEntry->_Position.end(); ++cit)
					{
						if(cit->ID() == 1)
							kPosition.x = Int2Float(cit->ParamA());
						else if(cit->ID() == 2)
							kPosition.z =  Int2Float(cit->ParamA());
						else if(cit->ID() == 3)
							fDistance = Int2Float(cit->ParamA());
					}
					pkActorModel->SetTranslate(kPosition);

					m_fDistance[eCameraMode] = (eCameraMode == CM_CHARACTER || eCameraMode == CM_GREAT_KUNGFU_REMAIN) ? fDistance : fDistance - 370;
					if(eCameraMode == CM_GREAT_KUNGFU_REMAIN)
					{
						m_fFov[eCameraMode] = m_fFov[CM_CHARACTER];
						m_fPitch[eCameraMode] = m_fPitch[CM_CHARACTER];
						m_fYaw[eCameraMode] = m_fYaw[CM_CHARACTER];
						m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_CHARACTER];
						_calcCameraFrustum(m_fFov[eCameraMode], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}
					else if(eCameraMode == CM_RANKINGLIST)
					{
						m_fDistance[eCameraMode] += 200;
						m_fFov[eCameraMode] = m_fFov[CM_CHARACTER];
						m_fPitch[eCameraMode] = m_fPitch[CM_CHARACTER];
						m_fYaw[eCameraMode] = m_fYaw[CM_CHARACTER];
						m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_CHARACTER];
						_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}
					else if(eCameraMode == CM_FLOWER)
					{
						m_fFov[eCameraMode] = m_fFov[CM_CHARACTER];
						m_fPitch[eCameraMode] = m_fPitch[CM_CHARACTER];
						m_fYaw[eCameraMode] = m_fYaw[CM_CHARACTER];
						m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_CHARACTER];
						_calcCameraFrustum(m_fFov[eCameraMode], (float)SENDFLOWER_WIDTH / (float)SENDFLOWER_HEIGHT, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}
					else if(eCameraMode == CM_RUNESCAPE_TIP)
					{
						m_fDistance[eCameraMode] += g_settings.CAMERA_UI_RUNESCAPE_BUDDY_DISTANCEREVISE;
						m_fFov[eCameraMode] = m_fFov[CM_CHARACTER];
						m_fPitch[eCameraMode] = m_fPitch[CM_CHARACTER];
						m_fYaw[eCameraMode] = m_fYaw[CM_CHARACTER];
						m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_CHARACTER];
						_calcCameraFrustum(m_fFov[eCameraMode], (float)RUNESCAPE_TIP_WIDTH / (float)RUNESCAPE_TIP_HEIGHT, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}
					else if(eCameraMode == CM_RAID_SETTLE)
					{
						m_fDistance[eCameraMode] += 200;
						m_fFov[eCameraMode] = m_fFov[CM_CHARACTER];
						m_fPitch[eCameraMode] = m_fPitch[CM_CHARACTER];
						m_fYaw[eCameraMode] = m_fYaw[CM_CHARACTER];
						m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_CHARACTER];
						_calcCameraFrustum(m_fFov[eCameraMode], (float)TIMELIMIT_WIDTH / (float)TIMELIMIT_HEIGHT, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}
					else if(eCameraMode == CM_TEAM_FULLBODY)
					{
						m_fFov[eCameraMode] = m_fFov[CM_CHARACTER];
						m_fPitch[eCameraMode] = m_fPitch[CM_CHARACTER];
						m_fYaw[eCameraMode] = m_fYaw[CM_CHARACTER];
						m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_CHARACTER];
						_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH2 / (float)FULL_CHAR_HEIGHT2, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}
				}
			}
			break;
		case CM_VIEWINFO:
		case CM_REWARD_RANK:
			{
				m_fFov[eCameraMode] = g_settings.CAMERA_UI_VIEWINFO_FOV * NI_PI / 180.0f;
				m_fPitch[eCameraMode] = g_settings.CAMERA_UI_VIEWINFO_PITCH * NI_PI / 180.0f;
				m_fYaw[eCameraMode] = g_settings.CAMERA_UI_VIEWINFO_YAW * NI_PI / 180.0f;
				m_fDistance[eCameraMode] = g_settings.CAMERA_UI_VIEWINFO_DISTANCE;
				m_fNearCameraHeightBias[eCameraMode] = g_settings.CAMERA_UI_VIEWINFO_NEAR_HEIGHT_BIAS;
				if(eCameraMode == CM_REWARD_RANK)
					_calcCameraFrustum(m_fFov[eCameraMode], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[eCameraMode]);
				else
					_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
				_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
			}
			break;
		}
		
		pkActorModel->SetCharType(iCharType);
		if(eCameraMode == CM_GEMBUFF)
		{
			pkActorModel->PlayAnimation(NET_PLAYER_MEDITATION);
		}
		else
		{
			pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE0);
		}
	}

	return true;
}

bool UI_AvatarManager::UpdatePlatformScene(const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	int iOldIndex = iCharIndex;
	if (eCameraMode == CM_GUILD_RAID_BUST)
		iCharIndex = 0;

	SceneMapIndex eSceneMapIndex = SMI_PLATFORM;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 处理占用了当前位置的avatarElement;
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;

	if ( isNeedRecreate )
	{
		const CF_CharType::DataEntry* pkData = NULL;
		CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton();
		pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
		if (pkData == NULL)
			return false;

		AvatarModelParam modelParam(pkData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ PostCreate 
	GD_ClientActorModel* pkActorModel = 
		pkAvatarElement->pkActorModel;

	if ( pkActorModel && isNeedRecreate )
	{
		switch(eCameraMode)
		{
		case CM_GUILD_RAID_BUST:
			{
				pkActorModel->SetTranslate(g_settings.GUILDRAID_BUST_POSITION[iOldIndex]);
				pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE_ATTACKREADY);
				pkActorModel->SetScale(g_settings.GUILDRAID_BUST_SCALE[iOldIndex]);
			}
			break;
		case CM_DAILYRECHARGE_RIGHT:
			{
				pkActorModel->SetTranslate(g_settings.DAILYRECHARGE_R_POSITION);
			}
			break;
		case CM_PRIVILEGE_VIP2:
			{
				pkActorModel->SetTranslate(g_settings.PRIVILEGE_VIP2_POSITION);
			}
			break;
		case CM_MIJI:
		case CM_STIGMAS:
			{
				NiPoint3 kPosition = NiPoint3::ZERO;
				CF_CharType::DataEntry *pkCharType = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
				if(NULL != pkCharType)
				{
					_getSubordinatePosition(pkCharType, eCameraMode, kPosition);
				}

				pkActorModel->SetTranslate(kPosition);

				NiMatrix3 kRotation = NiMatrix3::IDENTITY;
				pkActorModel->SetRotate(kRotation);
				pkActorModel->PlayAttack1Animation();
			}
			break;
		case CM_WILD_BOSS:
			{
				pkActorModel->SetTranslate(g_settings.WILDBOSS_POSITION[iCharIndex]);
				pkActorModel->PlayRandomIdleAnimation();
			}
			break;
		case CM_WILD_PRINCES_BOSS_FAMILY:
			{
				pkActorModel->SetTranslate(g_settings.WILD_PRINCES_POSITION[iCharIndex]);
				pkActorModel->PlayRandomIdleAnimation();
			}
			break;
		case CM_CORE_BOSS:
			pkActorModel->PlayRandomIdleAnimation();
			break;
		case CM_VIEWINFO:
			pkActorModel->SetTranslate(g_settings.VIEWINFO_ACTOR_POSITION);
			pkActorModel->PlayRandomIdleAnimation();
			break;
		case CM_TOYPET_CHOOSE:
			{
				NiMatrix3 kRotation = NiMatrix3::IDENTITY;
				kRotation.MakeZRotation(30.0f/180.0f * NI_PI);
				pkActorModel->SetRotate(kRotation);
				pkActorModel->SetTranslate(g_settings.CAMERA_UI_TOYPET_CHOOSE_POS[iCharIndex]);
				pkActorModel->PlayRandomIdleAnimation();
			}
			break;
		default:
			{
				pkActorModel->SetTranslate(NiPoint3::ZERO);

				NiMatrix3 kRotation = NiMatrix3::IDENTITY;
				//kRotation.MakeXRotation(90.0f/180.0f * NI_PI);
				//kRotation.MakeZRotation(-150.0f/180.0f * NI_PI);
				pkActorModel->SetRotate(kRotation);
			}
			break;
		}
	}

	return true;
}

bool UI_AvatarManager::UpdateToyPetScene(const StringType& strGroupName, long lCharType, int iIndex, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	int iOldIndex = iIndex;

	SceneMapIndex eSceneMapIndex = SMI_PLATFORM;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 处理占用了当前位置的avatarElement;
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastCharType, iIndex, (CameraMode)eCameraMode);

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;

	if ( isNeedRecreate )
	{
		const CF_CharType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
		if (pkData == NULL)
			return false;

		AvatarModelParam modelParam(pkData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iIndex, (CameraMode)eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	
		if ( !pkAvatarElement )
			return false;

		//@ PostCreate 
		GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

		if ( pkActorModel )
		{
			switch(eCameraMode)
			{
			case CM_TOYPET:
			case CM_TOYPET_GAIN:
			case CM_RANKINGLIST:
			case CM_RUNESCAPE_TIP:
				{
					float fDistance = 0;
					NiPoint3 kPosition = NiPoint3::ZERO;
					const mem::vector<int16_t>& pos = pkData->_Position;
					if(pos.size() >= 3)
					{
						kPosition.x = Int2Float(pos[0]);
						kPosition.z = Int2Float(pos[1]);
						fDistance = Int2Float(pos[2]);
					}
					m_fDistance[eCameraMode] = fDistance;
					if(eCameraMode == CM_RUNESCAPE_TIP)
					{
						m_fFov[eCameraMode] = g_settings.CAMERA_UI_RUNESCAPE_TIP_FOV * NI_PI / 180.0f;
						m_fPitch[eCameraMode] = g_settings.CAMERA_UI_RUNESCAPE_TIP_PITCH * NI_PI / 180.0f;
						m_fYaw[eCameraMode] = g_settings.CAMERA_UI_RUNESCAPE_TIP_YAW * NI_PI / 180.0f;
						m_fNearCameraHeightBias[eCameraMode] = g_settings.CAMERA_UI_RUNESCAPE_TIP_NEAR_HEIGHT_BIAS;
						_calcCameraFrustum(m_fFov[eCameraMode], (float)RUNESCAPE_TIP_WIDTH / (float)RUNESCAPE_TIP_HEIGHT, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}
					else if(eCameraMode == CM_RANKINGLIST)
					{
						m_fFov[eCameraMode] = m_fFov[CM_TOYPET];
						m_fPitch[eCameraMode] = m_fPitch[CM_TOYPET];
						m_fYaw[eCameraMode] = m_fYaw[CM_TOYPET];
						m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_TOYPET];
						_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
						_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
					}

					pkActorModel->SetTranslate(kPosition);
					pkActorModel->SetPitchByTerrain(false);
					pkActorModel->PlayRandomIdleAnimation();
				}
				break;
			default:
				break;
			}
		}
	}
	return true;
}

bool UI_AvatarManager::UpdateTalkNpcScene(const StringType& strGroupName, int iCharIndex, long lCharType, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;
	/*if (iCharIndex != 0)
		return false;*/

	SceneMapIndex eSceneMapIndex = SMI_WORLDMAP;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 处理占用了当前位置的avatarElement;
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	
	// 销毁
	AvatarElementKeyId lastkeyId = _generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);
	_destroyAvatarElement(lastkeyId, strGroupName);

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton();
	const CF_CharType::DataEntry* pkData = pkCharTypeCsv.GetEntryPtr(lCharType);
	if (pkData == NULL)
		return false;

	AvatarModelParam modelParam(pkData->_kResName);
	modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
	modelParam.m_iModelIndex = 0;
	modelParam.m_bModelAsyncLoad = true;

	AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)eCameraMode);
	pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	
	if ( !pkAvatarElement )
		return false;

	//@ 配置创建的avatarElement
	GD_ClientActorModel* pkActorModel = 
		pkAvatarElement->pkActorModel;

	if ( pkActorModel )
	{
		m_fDistance[eCameraMode] = pkData->_CameraDist;
		NiPoint3 kPosition = NiPoint3::ZERO;
		if(pkData->_Position.size() >= 3)
		{
			kPosition.x = Int2Float(pkData->_Position[0]);
			kPosition.y = Int2Float(pkData->_Position[1]);
			kPosition.z = Int2Float(pkData->_Position[2]);
		}

		if(eCameraMode == CM_RANKINGLIST)
		{
			m_fFov[eCameraMode] = g_settings.CAMERA_UI_RANKINGLIST_FOV * NI_PI / 180.0f;
			m_fPitch[eCameraMode] = g_settings.CAMERA_UI_RANKINGLIST_PITCH * NI_PI / 180.0f;
			m_fYaw[eCameraMode] = g_settings.CAMERA_UI_RANKINGLIST_YAW * NI_PI / 180.0f;
			m_fNearCameraHeightBias[eCameraMode] = g_settings.CAMERA_UI_RANKINGLIST_NEAR_HEIGHT_BIAS;
			_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
			_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
		}

		pkActorModel->SetTranslate(kPosition);
		pkActorModel->SetCharType(lCharType);
        pkActorModel->SetPitchByTerrain(false);
		pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE0);
	}

    return true;
}

bool UI_AvatarManager::UpdateWildPrincesBossScene(const StringType& strGroupName, int iCharIndex, long lCharType, float fCameraDist, float fTranslateX, float fTranslateZ, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;
	/*if (iCharIndex != 0)
		return false;*/

	SceneMapIndex eSceneMapIndex = SMI_WORLDMAP;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 处理占用了当前位置的avatarElement;
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate  )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);\

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	int iCharType = lCharType;
	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	if ( isNeedRecreate )
	{
		StringType sMod;
		GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE iModelType = GD_ClientAsyncModelManager::AMT_NPC;

		switch(iCameraMode)
		{
		case CM_FUNCTION_PREVIEW_BUTTON:
			{
				CF_BuddyTransform& pkCharTypeCsv = CF_BuddyTransform::GetSingleton(); 
				const CF_BuddyTransform::DataEntry* pkData = pkCharTypeCsv.GetEntryPtr(lCharType);
				if (pkData == NULL)
					return false;

				iCharType = pkData->_buddy;
				sMod = pkData->_model;
				iModelType = GD_ClientAsyncModelManager::AMT_PLAYER;
			}
			break;
		default:
			{
				CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton(); 
				const CF_CharType::DataEntry* pkData = pkCharTypeCsv.GetEntryPtr(lCharType);
				if (pkData == NULL)
					return false;

				sMod = pkData->_kResName;
			}
			break;
		}
		
		AvatarModelParam modelParam(sMod);
		modelParam.m_iModelType = (int)iModelType;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ 配置创建的avatarElement
	GD_ClientActorModel* pkActorModel = 
		pkAvatarElement->pkActorModel;

	if ( isNeedRecreate && pkActorModel )
	{
		pkActorModel->SetCharType(iCharType);
		/*NiMatrix3 kRotation;
		kRotation.MakeZRotation(NI_PI * -0.80f);

        pkBossModel->SetRotate(NiMatrix3::IDENTITY * kRotation);*/
		pkActorModel->SetTranslate(NiPoint3(fTranslateX, 0, fTranslateZ));
        pkActorModel->SetPitchByTerrain(false);
		pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE0);

		m_fDistance[eCameraMode] = fCameraDist;
	}

    return true;
}

bool UI_AvatarManager::UpdateVehicleScene(const StringType& strGroupName, long vehicleCharType, int vehicleCharIndex, int iCameraMode /* = CM_VEHICLE*/)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_VEHICLE;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 处理占用了当前位置的avatarElement;
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(vehicleCharIndex, eCameraMode);
	long lastVehicleCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastVehicleCharType != vehicleCharType );
	if ( isNeedRecreate )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastVehicleCharType, vehicleCharIndex, eCameraMode);

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	if ( isNeedRecreate )
	{
		const CF_CharType::DataEntry* pkData = NULL;
		if((vehicleCharType > 1000 && vehicleCharType < 2000) || (vehicleCharType > 10000 && vehicleCharType < 20000) || vehicleCharType == 100004)
		{
			CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton();
			pkData = pkCharTypeCsv.GetEntryPtr(vehicleCharType);
		}
		if (pkData == NULL)
			return false;

		AvatarModelParam modelParam(pkData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(vehicleCharType, vehicleCharIndex, eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ 配置新的avatarElement 
	GD_ClientActorModel* pkActorModel = 
		pkAvatarElement->pkActorModel;

	if ( isNeedRecreate && pkActorModel )
	{
		GD_ClientActorModel* pkVehicleModel = pkActorModel;

		pkVehicleModel->SetCharType(vehicleCharType);
		NiMatrix3 kRotation = NiMatrix3::IDENTITY;
		NiPoint3 kPostion = NiPoint3::ZERO;

		if (eCameraMode == CM_TEAM_FULLBODY)
		{
			kPostion = g_settings.TEAM_VEHICLE_POSITION;
			kRotation.MakeZRotation(30.0f / 180.0f * NI_PI);	
		}
		else if(eCameraMode == CM_ARENA_BATTLE_ACTOR || eCameraMode == CM_ARENA_BATTLE_ACTOR_OTHER)
		{
			kPostion = g_settings.TEAM_VEHICLE_POSITION;
			kRotation.MakeZRotation(60.0f / 180.0f * NI_PI);	
		}
		else if(eCameraMode == CM_ESC_UI_CHAR || eCameraMode == CM_CHARACTER)
		{
			kPostion = g_settings.ESC_VEHICLE_POSITION;
			kRotation.MakeZRotation(325.0f / 180.0f * NI_PI);
		}
		else if(eCameraMode == CM_GREAT_KUNGFU_REMAIN2)
		{
			kPostion = g_settings.GREATKUNGFU_REMAIN_VEHICLE_POSITION;
			kRotation.MakeZRotation(g_settings.GREATKUNGFU_REMAIN_VEHICLE_ZROTATION * NI_PI);
		}
		/*else if(iRenderModel == CM_VIEWINFO || iRenderModel == CM_RANKINGLIST)
		{
			pkVehicleModel->SetTranslate(g_settings.VIEWINFO_VEHICLE_POSITION);
			kRotation.MakeZRotation(g_settings.GREATKUNGFU_VEHICLE_ROTATION_ANGLE / 180.0f * NI_PI);
		}*/
		else if(eCameraMode == CM_FUNCTION_PREVIEW_BUTTON)
		{
			kPostion = g_settings.FUNCTION_PREVIEW_BUTTON_VEHICLE_POSITION;
			kRotation.MakeZRotation(g_settings.FUNCTION_PREVIEW_BUTTON_VEHICLE_ZROTATION * NI_PI);
			m_fDistance[eCameraMode] = g_settings.FUNCTION_PREVIEW_BUTTON_VEHICLE_DISTANCE;
		}
		else
		{
			kRotation.MakeZRotation(325.0f / 180.0f * NI_PI);
		}
		
		//加上马具
		CF_NPCAvatar::DataEntry *pkAvatarEntry = CF_NPCAvatar::GetSingletonPtr()->GetEntryPtr(vehicleCharType);
		if(NULL != pkAvatarEntry)
		{
			LC_ClientItem *pkItem = (LC_ClientItem *)LC_ItemFactoryBase::GetSingletonPtr()->RequestItem(pkAvatarEntry->_lExtraID);
			if(pkItem)
				pkItem->OperationAvatarFunc(MALE_TYPE, pkVehicleModel, NULL, GET_PROCESS_TIME);
		}
		//缩放坐骑
		CF_CharType::DataEntry *pkCharType = SafeGetCSVFileDataEntryPtr<CF_CharType>(vehicleCharType);
		if(NULL != pkCharType)
		{
			if(eCameraMode == CM_GREAT_KUNGFU_REMAIN2 || eCameraMode == CM_VEHICLE || eCameraMode == CM_VIEWINFO ||
				eCameraMode == CM_RANKINGLIST || eCameraMode == CM_REWARD_RANK || eCameraMode == CM_RUNESCAPE_TIP || eCameraMode == CM_PRIVILEGE_VIP2 )
			{
				_getSubordinatePosition(pkCharType, eCameraMode, kPostion);
			}
		}

		if(eCameraMode == CM_GREAT_KUNGFU_REMAIN2 || eCameraMode == CM_VIEWINFO || eCameraMode == CM_RANKINGLIST ||
			eCameraMode == CM_REWARD_RANK || eCameraMode == CM_RUNESCAPE_TIP || eCameraMode == CM_PRIVILEGE_VIP2
			|| (eCameraMode >= CM_VEHICLE_SKIN_0 && eCameraMode <= CM_VEHICLE_SKIN_5))
		{
			m_fFov[eCameraMode] = m_fFov[CM_VEHICLE];
			m_fPitch[eCameraMode] = m_fPitch[CM_VEHICLE];
			m_fYaw[eCameraMode] = m_fYaw[CM_VEHICLE];
			m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_VEHICLE];
			if(eCameraMode == CM_REWARD_RANK)
			{
				_calcCameraFrustum(m_fFov[eCameraMode], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[eCameraMode]);
			}
			else if(eCameraMode == CM_GREAT_KUNGFU_REMAIN2)
			{
				_calcCameraFrustum(m_fFov[eCameraMode], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[eCameraMode]);
			}
			else if(eCameraMode == CM_RUNESCAPE_TIP)
			{
				_calcCameraFrustum(m_fFov[eCameraMode], (float)RUNESCAPE_TIP_WIDTH / (float)RUNESCAPE_TIP_HEIGHT, m_akFrustums[eCameraMode]);
				m_fDistance[eCameraMode] += g_settings.CAMERA_UI_RUNESCAPE_VEHICLE_DISTANCEREVISE;
			}
			else if(eCameraMode == CM_RANKINGLIST)
			{
				_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
			}
			else
			{
				_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
			}
			_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
		}
		else if(eCameraMode == CM_JIANGJUNFU)
		{
			kPostion = g_settings.JIANGJUNFU_VEHICLE_POSITION[vehicleCharIndex];
		}
		//else if(eCameraMode ==CM_RANKINGLIST )
		//{
		//	pkVehicleModel->SetScale(1.2f); // 排行榜模型扩大
		//}

		pkVehicleModel->SetTranslate(kPostion);
		pkVehicleModel->SetRotate(kRotation);
        pkVehicleModel->SetPitchByTerrain(false);
        pkVehicleModel->SetIsVehicleModel(false);
		if(eCameraMode == CM_FUNCTION_PREVIEW_BUTTON 
			|| eCameraMode == CM_FUNCTION_PREVIEW_BUTTON2)
			pkVehicleModel->PlayRandomIdleAnimation();

	}

    return true;
}

bool UI_AvatarManager::UpdateDevaEyeScene(const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_DEVAEYE;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	if ( isNeedRecreate )
	{
		const CF_CharType::DataEntry* pkData = NULL;
		if (lCharType > 3000 && lCharType < 4000)
		{
			CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton();
			pkData = pkCharTypeCsv.GetEntryPtr(lCharType);
		}
		if (pkData == NULL)
			return false;

		AvatarModelParam modelParam(pkData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ 配置新的avatarElement;
	if ( isNeedRecreate )
	{
		GD_ClientActorModel* pkDevaEyeModel = pkAvatarElement->pkActorModel;

		NiMatrix3 kRotation = NiMatrix3::IDENTITY;
		NiPoint3 kPostion = NiPoint3::ZERO;
		//float fScale = 2.0f;//pkDevaEyeModel->GetScale();
		if(eCameraMode == CM_ESC_UI_CHAR || eCameraMode == CM_CHARACTER)
		{
			kPostion = g_settings.ESC_DEVAEYE_POSITION;
			kRotation.MakeZRotation(35.0f / 180.0f * NI_PI);
		}
		else if(eCameraMode == CM_GREAT_KUNGFU_REMAIN2)
		{
			//kPostion = g_settings.GREATKUNGFU_REMAIN_DEVAEYE_POSITION;
			float xRotation = g_settings.GREATKUNGFU_REMAIN_DEVAEYE_XROTATION;
			float yRotation = g_settings.GREATKUNGFU_REMAIN_DEVAEYE_YROTATION;
			float zRotation = g_settings.GREATKUNGFU_REMAIN_DEVAEYE_ZROTATION;
			NiMatrix3 kTempx = NiMatrix3::IDENTITY;
			NiMatrix3 kTempy = NiMatrix3::IDENTITY;
			NiMatrix3 kTempz = NiMatrix3::IDENTITY;
			kTempx.MakeXRotation(xRotation * NI_PI);
			kTempy.MakeYRotation(yRotation * NI_PI);
			kTempz.MakeZRotation(zRotation * NI_PI);
			/////角度旋转右乘
			kRotation = kTempz * kTempy * kTempx;
		}
		else if(eCameraMode == CM_FUNCTION_PREVIEW_BUTTON)
		{
			kPostion = g_settings.FUNCTION_PREVIEW_BUTTON_DEVAEYE_POSITION;
			float xRotation = g_settings.FUNCTION_PREVIEW_BUTTON_DEVAEYE_XROTATION;
			float yRotation = g_settings.FUNCTION_PREVIEW_BUTTON_DEVAEYE_YROTATION;
			float zRotation = g_settings.FUNCTION_PREVIEW_BUTTON_DEVAEYE_ZROTATION;
			NiMatrix3 kTempx = NiMatrix3::IDENTITY;
			NiMatrix3 kTempy = NiMatrix3::IDENTITY;
			NiMatrix3 kTempz = NiMatrix3::IDENTITY;
			kTempx.MakeXRotation(xRotation * NI_PI);
			kTempy.MakeYRotation(yRotation * NI_PI);
			kTempz.MakeZRotation(zRotation * NI_PI);
			/////角度旋转右乘
			kRotation = kTempz * kTempy * kTempx;

			m_fDistance[eCameraMode] = g_settings.FUNCTION_PREVIEW_BUTTON_DEVAEYE_DISTANCE;
		}
		else if(eCameraMode == CM_JIANGJUNFU)
		{
			kPostion = g_settings.JIANGJUNFU_DEVAEYE_POSITION[iCharIndex];
		}
		else
		{
			//if(iRenderModel == CM_VIEWINFO || iRenderModel == CM_REWARD_RANK)
			//{
			//	kPostion = g_settings.VIEWINFO_DEVAEYE_POSITION;
			//	//kRotation.MakeZRotation(0.25f*NI_PI);
			//}

			kRotation.MakeZRotation(20/180.0f*NI_PI);
		}

		if(eCameraMode == CM_DEVAEYE || eCameraMode == CM_VIEWINFO || eCameraMode == CM_RANKINGLIST || eCameraMode == CM_REWARD_RANK
			|| eCameraMode == CM_RUNESCAPE_TIP || eCameraMode == CM_GREAT_KUNGFU_REMAIN2)
		{
			CF_CharType::DataEntry *pkCharType = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
			if(NULL != pkCharType)
			{
				_getSubordinatePosition(pkCharType, eCameraMode, kPostion);
			}

			m_fPitch[eCameraMode] = 90 * NI_PI / 180.0f;

			if(eCameraMode != CM_DEVAEYE)
			{
				m_fFov[eCameraMode] = m_fFov[CM_DEVAEYE];
				m_fYaw[eCameraMode] = m_fYaw[CM_DEVAEYE];
				m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_DEVAEYE];
				if(eCameraMode == CM_REWARD_RANK)
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[eCameraMode]);
				}
				else if(eCameraMode == CM_GREAT_KUNGFU_REMAIN2)
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[eCameraMode]);
				}
				else if(eCameraMode == CM_RUNESCAPE_TIP)
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)RUNESCAPE_TIP_WIDTH / (float)RUNESCAPE_TIP_HEIGHT, m_akFrustums[eCameraMode]);
					m_fDistance[eCameraMode] += g_settings.CAMERA_UI_RUNESCAPE_PET_DISTANCEREVISE;
				}
				else
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
				}
			}
			_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
		}

		pkDevaEyeModel->SetTranslate(kPostion);
		pkDevaEyeModel->SetRotate(NiMatrix3::IDENTITY * kRotation);		
		//pkDevaEyeModel->SetScale(fScale);
		pkDevaEyeModel->SetPitchByTerrain(false);
	}

	return true;
}

bool UI_AvatarManager::UpdateFabaoScene(const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_FABAO;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	if ( isNeedRecreate )
	{
		// 创建新的宝箱模型
		const CF_CharType::DataEntry* pkData = NULL;
		if (lCharType >= 50000 && lCharType < 60000)
		{
			CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton();
			pkData = pkCharTypeCsv.GetEntryPtr(lCharType);
		}
		if (pkData == NULL)
			return false;

		AvatarModelParam modelParam(pkData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ 配置新的avatarElement;
	if ( isNeedRecreate )
	{
		NiPoint3 kPostion = NiPoint3::ZERO;
		if(eCameraMode == CM_FABAO || eCameraMode == CM_VIEWINFO || eCameraMode == CM_RANKINGLIST || eCameraMode == CM_REWARD_RANK)
		{
			CF_CharType::DataEntry *pkCharType = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
			if(NULL != pkCharType)
			{
				_getSubordinatePosition(pkCharType, eCameraMode, kPostion);
			}

			if(eCameraMode != CM_FABAO)
			{
				m_fFov[eCameraMode] = m_fFov[CM_FABAO];
				m_fPitch[eCameraMode] = m_fPitch[CM_FABAO];
				m_fYaw[eCameraMode] = m_fYaw[CM_FABAO];
				m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_FABAO];
				if(eCameraMode == CM_REWARD_RANK)
					_calcCameraFrustum(m_fFov[eCameraMode], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[eCameraMode]);
				else
					_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
				_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
			}
		}
		else if(eCameraMode == CM_FUNCTION_PREVIEW_BUTTON)
		{
			kPostion = g_settings.FUNCTION_PREVIEW_BUTTON_FABAO_POSITION;
			m_fDistance[eCameraMode] = g_settings.FUNCTION_PREVIEW_BUTTON_FABAO_DISTANCE;
		}

		pkAvatarElement->pkActorModel->SetTranslate(kPostion);
		pkAvatarElement->pkActorModel->SetPitchByTerrain(false);
	}

	return true;
}

bool UI_AvatarManager::UpdateGodWingTextureScene(const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_GODWING;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	if ( isNeedRecreate )
	{
		// 创建新的翅膀模型
		const CF_CharType::DataEntry* pkData = NULL;
		if (lCharType > 1000 && lCharType < 8000)
		{
			CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton();
			pkData = pkCharTypeCsv.GetEntryPtr(lCharType);
		}
		if (pkData == NULL)
			return false;

		AvatarModelParam modelParam(pkData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ 配置新的avatarElement;
	GD_ClientActorModel* pkActorModel = 
		pkAvatarElement->pkActorModel;

	if ( isNeedRecreate )
	{
		GD_ClientActorModel* pkGodWingTextureModel = pkAvatarElement->pkActorModel;

		NiMatrix3 kRotation = NiMatrix3::IDENTITY;
		NiPoint3 kPostion = NiPoint3::ZERO;
		float fScale = 1.0f;//pkDevaEyeModel->GetScale();

		//float xRotation = -0.25f;
		//float yRotation = -0.1f;
		//float zRotation = 0.25f;
		//NiMatrix3 kTempx = NiMatrix3::IDENTITY;
		//NiMatrix3 kTempy = NiMatrix3::IDENTITY;
		//NiMatrix3 kTempz = NiMatrix3::IDENTITY;
		//kTempx.MakeXRotation(xRotation * NI_PI);
		//kTempy.MakeYRotation(yRotation * NI_PI);
		//kTempz.MakeZRotation(zRotation * NI_PI);
		///////角度旋转右乘
		//kRotation = kTempz * kTempy * kTempx;

		if(eCameraMode == CM_MUHUN || eCameraMode == CM_VIEWINFO || eCameraMode == CM_RANKINGLIST || eCameraMode == CM_REWARD_RANK 
			|| eCameraMode == CM_GREAT_KUNGFU_REMAIN2 || eCameraMode == CM_RUNESCAPE_TIP
			|| (eCameraMode >= CM_VEHICLE_SKIN_0 && eCameraMode <= CM_VEHICLE_SKIN_5))
		{
			CF_CharType::DataEntry *pkCharType = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
			if(NULL != pkCharType)
			{
				_getSubordinatePosition(pkCharType, eCameraMode, kPostion);
			}

			if(eCameraMode != CM_MUHUN)
			{
				m_fFov[eCameraMode] = m_fFov[CM_MUHUN];
				m_fPitch[eCameraMode] = m_fPitch[CM_MUHUN];
				m_fYaw[eCameraMode] = m_fYaw[CM_MUHUN];
				m_fNearCameraHeightBias[eCameraMode] = m_fNearCameraHeightBias[CM_MUHUN];
				if(eCameraMode == CM_REWARD_RANK)
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[eCameraMode]);
				}
				else if(eCameraMode == CM_GREAT_KUNGFU_REMAIN2)
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[eCameraMode]);
				}
				else if(eCameraMode == CM_RUNESCAPE_TIP)
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)RUNESCAPE_TIP_WIDTH / (float)RUNESCAPE_TIP_HEIGHT, m_akFrustums[eCameraMode]);
					m_fDistance[eCameraMode] += g_settings.CAMERA_UI_RUNESCAPE_BACKWING_DISTANCEREVISE;
				}
				else if(eCameraMode == CM_RANKINGLIST)
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
				}
				else
				{
					_calcCameraFrustum(m_fFov[eCameraMode], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[eCameraMode]);
				}
				_calcCameraRotationMatrix(m_fPitch[eCameraMode], m_fYaw[eCameraMode], m_akCameraRotation[eCameraMode]);
			}

			/*if(eCameraMode == CM_GREAT_KUNGFU_REMAIN)
				kPostion = g_settings.GREATKUNGFU_REMAIN_GODWING_POSITION;*/
		}
		else if(eCameraMode == CM_FUNCTION_PREVIEW_BUTTON || eCameraMode == CM_FUNCTION_PREVIEW_BUTTON2)
		{
			kPostion = g_settings.FUNCTION_PREVIEW_BUTTON_GODWING_POSITION;
			m_fDistance[eCameraMode] = g_settings.FUNCTION_PREVIEW_BUTTON_GODWING_DISTANCE;
		}
		else if(eCameraMode == CM_JIANGJUNFU)
		{
			kPostion = g_settings.JIANGJUNFU_GODWING_POSITION[iCharIndex];
		}

		pkGodWingTextureModel->SetTranslate(kPostion);
		pkGodWingTextureModel->SetRotate(NiMatrix3::IDENTITY * kRotation);		
		//pkGodWingTextureModel->SetScale(fScale);
		pkGodWingTextureModel->SetPitchByTerrain(false);
	}

	return true;
}

bool UI_AvatarManager::UpdateArenaBattleActorScene(const StringType& strGroupName, int iCid, int iIndex, long lWeaponID, int iCameraMode)
{
	return UpdateActorScene(strGroupName, iCid, iIndex, iCameraMode);
}

bool UI_AvatarManager::UpdateNonpareilItemAvatarScene(const StringType& strGroupName, long lCharType, int iCharIndex, float fTranslateX,float fTranslateY, float fTranslateZ, int iCameraMode)
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	SceneMapIndex eSceneMapIndex = SMI_WORLDMAP;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 处理占用了当前位置的avatarElement;
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate  )
	{
		// 销毁
		AvatarElementKeyId lastkeyId = 
			_generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);\

		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	if ( isNeedRecreate )
	{
		StringType sMod;
		GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE iModelType = GD_ClientAsyncModelManager::AMT_NPC;

		
		CF_CharType& pkCharTypeCsv = CF_CharType::GetSingleton();
		const CF_CharType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
		if (pkData == NULL)
			return false;

		sMod = pkData->_kResName;
		
		AvatarModelParam modelParam(sMod);
		modelParam.m_iModelType = (int)iModelType;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ 配置创建的avatarElement
	GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;
	if ( isNeedRecreate && pkActorModel )
	{
		pkActorModel->SetCharType(lCharType);
		pkActorModel->SetTranslate(NiPoint3(fTranslateX, fTranslateY, fTranslateZ));
		pkActorModel->SetPitchByTerrain(false);
		pkActorModel->PlayAnimation(RES_ANM_ACTOR_IDLE0);
	}

	return true;
}

bool UI_AvatarManager::UpdateHookBotScene( const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode )
{

	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_HOOKBOT;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 清空之前的actorAvatarElement
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		AvatarElementKeyId lastkeyId = _generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);
		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	if ( isNeedRecreate )
	{
		const CF_Bot::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_Bot>(lCharType);
		if (pkData == NULL)
			return false;

		const CF_CharType::DataEntry* pkCharTypeData = SafeGetCSVFileDataEntryPtr<CF_CharType>(pkData->_charType);
		if (pkCharTypeData == NULL)
			return false;

		AvatarModelParam modelParam(pkCharTypeData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, eCameraMode);
		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}
	if ( !pkAvatarElement )
		return false;

	//@ 配置这个新的AvatarElement
	//pkActorModel->SetVisibility(true);
	GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

	if (eCameraMode == CM_HOOK_BOT)
	{
		if(iCharIndex >= 0 && iCharIndex < 20)
		{
			pkActorModel->SetTranslate(g_settings.CAMERA_UI_HOOKBOT_POS[iCharIndex]);
		}
		
		pkActorModel->PlayAnimation(RES_ANM_ACTOR_BORN);
	}
	return true;
}

bool UI_AvatarManager::UpdateHookBotMonsterScene( const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode )
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_HOOKBOT_MONSTER;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 清空之前的actorAvatarElement
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		AvatarElementKeyId lastkeyId = _generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);
		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, eCameraMode);
	if ( isNeedRecreate )
	{
		const CF_CharType::DataEntry* pkCharTypeData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
		if (pkCharTypeData == NULL)
			return false;

		AvatarModelParam modelParam(pkCharTypeData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}

	if ( !pkAvatarElement )
		return false;

	//@ 配置这个新的AvatarElement
	//pkActorModel->SetVisibility(true);

	//怪物默认生命3点
	pkAvatarElement->iLife = GetGlobalSetting.iHookBotMonsterInitHp;
	GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

	if (eCameraMode == CM_HOOK_BOT_MONSTER)
	{
		if(iCharIndex >= 0 && iCharIndex <= 2)
		{
			pkActorModel->SetTranslate(g_settings.CAMERA_UI_HOOKBOT_MONSTER_POS[iCharIndex]);
		}

		pkActorModel->PlayRandomIdleAnimation();
	}
	return true;
}

bool UI_AvatarManager::UpdateHookBotBoxScene( const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode )
{
	//@ 参数检查
	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode == CM_MAX_COUNT )
		return false;

	//@ 初始化
	SceneMapIndex eSceneMapIndex = SMI_TREASUREITEM;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//@ 清空之前的actorAvatarElement
	AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, eCameraMode);
	long lastCharType = _getCharTypeFromScene(sceneIndex, sceneMap);
	bool isNeedRecreate = ( lastCharType != lCharType );
	if ( isNeedRecreate )
	{
		AvatarElementKeyId lastkeyId = _generateAvatarElementKeyId(lastCharType, iCharIndex, (CameraMode)eCameraMode);
		_destroyAvatarElement(lastkeyId, strGroupName);
	}

	//@ 创建新的avatarElement;
	AvatarElementPtr pkAvatarElement = NULL;
	AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, eCameraMode);
	if ( isNeedRecreate )
	{
		const CF_CharType::DataEntry* pkCharTypeData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
		if (pkCharTypeData == NULL)
			return false;

		AvatarModelParam modelParam(pkCharTypeData->_kResName);
		modelParam.m_iModelType = (int)GD_ClientAsyncModelManager::AMT_NPC;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = true;

		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, strGroupName, modelParam);
	}

	if ( !pkAvatarElement )
		return false;

	GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;
	pkActorModel->SetTranslate(g_settings.CAMERA_UI_HOOKBOT_BOX_POS);
	pkActorModel->PlayRandomIdleAnimation();
	return true;
}

AvatarElementKeyId UI_AvatarManager::_getHookBotShootTargetKeyID()
{
	SceneMapIndex eSceneMapIndex = SMI_HOOKBOT_MONSTER;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);

	//筛选出符合条件的，然后再随机选一个
	mem::vector<AvatarElementKeyId> kTmp;
	AvatarElementSceneMap::iterator sceneIter = sceneMap.begin();
	for(; sceneIter != sceneMap.end(); ++sceneIter)
	{
		AvatarElementSceneIndex sceneIndex = sceneIter->first;
		long lCharType = sceneIter->second;

		int iCharIndex = Invalid_AvatarElementCharIndex;
		CameraMode iCameraMode = CM_HOOK_BOT_MONSTER;
		_extractAvatarElementInfoFromSceneIndex(sceneIter->first, iCharIndex, iCameraMode);
		
		AvatarElementKeyId keyId = _generateAvatarElementKeyId(sceneIter->second, iCharIndex, iCameraMode);
		AvatarElementPtr avatarElement = GetAvatar(keyId);
		if(avatarElement != NULL && avatarElement->iLife > 0)
			kTmp.push_back(keyId);
	}

	if(kTmp.empty())
		return Invalid_AvatarElementKeyId;

	int rand = UT_MathBase::RandInRangeInt(0, kTmp.size() - 1);
	return kTmp[rand];
}

void UI_AvatarManager::_updateHookBotState(float fCurrentTime)
{
	//更新状态
	//怪物
	AvatarElementSceneMap& sceneMapMon = _getSceneMap(SMI_HOOKBOT_MONSTER);
	AvatarElementSceneMap::iterator sceneIterMon = sceneMapMon.begin();
	for(; sceneIterMon != sceneMapMon.end(); )
	{
		AvatarElementSceneIndex sceneIndex = sceneIterMon->first;
		long lCharType = sceneIterMon->second;

		int iCharIndex = Invalid_AvatarElementCharIndex;
		CameraMode iCameraMode = CM_HOOK_BOT_MONSTER;
		_extractAvatarElementInfoFromSceneIndex(sceneIterMon->first, iCharIndex, iCameraMode);

		CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, iCharIndex, iCameraMode);
		AvatarElementPtr avatarElement = GetAvatar(keyId);
		GD_ClientActorModel* pkModel = avatarElement->pkActorModel;

		const StringType& sAnimName = pkModel->GetCurrentAnimationName();
		float fAnimPlayTime = fCurrentTime - pkModel->GetAnimationStartPlayTime();
		if(sAnimName == RES_ANM_ACTOR_UNDERATTACK && fAnimPlayTime > 0.08f)
		{
			pkModel->PlayAnimation(RES_ANM_ACTOR_IDLE_NORMAL);
		}
		else if(sAnimName == RES_ANM_ACTOR_DEAD && fAnimPlayTime > 1.15f)
		{
			sceneIterMon = sceneMapMon.erase(sceneIterMon);
			_destroyAvatarElement(keyId, "RobotWorking");
			
			if(sceneMapMon.empty())
			{
				//最后一个怪也死了，向UI发起切换场景事件
				m_bChangeStage = true;
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_HOOK_BOT_NEXTSTAGE);

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
				break;
			}
			continue;
		}

		++sceneIterMon;
	}
	

	//机器人
	AvatarElementSceneMap& sceneMap = _getSceneMap(SMI_HOOKBOT);
	AvatarElementSceneMap::iterator sceneIter = sceneMap.begin();
	for(; sceneIter != sceneMap.end(); ++sceneIter)
	{
		AvatarElementSceneIndex sceneIndex = sceneIter->first;
		long lCharType = sceneIter->second;

		int iCharIndex = Invalid_AvatarElementCharIndex;
		CameraMode iCameraMode = CM_HOOK_BOT;
		_extractAvatarElementInfoFromSceneIndex(sceneIter->first, iCharIndex, iCameraMode);

		AvatarElementPtr avatarElement = GetAvatar(lCharType, iCharIndex, iCameraMode);
		GD_ClientActorModel* pkModel = avatarElement->pkActorModel;
		
		if(m_bChangeStage)
		{
			pkModel->PlayAnimation(RES_ANM_ACTOR_RUN);
		}
		else
		{
			const StringType& sAnimName = pkModel->GetCurrentAnimationName();
			float fAnimPlayTime = fCurrentTime - pkModel->GetAnimationStartPlayTime();
			if((sAnimName == RES_ANM_ATTACK_1 && fAnimPlayTime > 0.08f) || (sAnimName == RES_ANM_ACTOR_RUN))
			{
				pkModel->PlayAnimation(RES_ANM_ACTOR_IDLE_NORMAL);
			}
		}
	}
}

void UI_AvatarManager::_updateHookBotShoot(float fCurrentTime)
{
	if (fCurrentTime - m_fLastShootUpdateTime < UPDATE_BOT_SHOOT_TIME_INTERVAL)
		return;

	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

	m_fLastShootUpdateTime = fCurrentTime;
	SceneMapIndex eSceneMapIndex = SMI_HOOKBOT;
	AvatarElementSceneMap& sceneMap = _getSceneMap(eSceneMapIndex);
	AvatarElementSceneMap::iterator sceneIter = sceneMap.begin();
	AvatarElementSceneMap::iterator sceneIEnd = sceneMap.end();
	for(; sceneIter != sceneIEnd; ++sceneIter)
	{
		AvatarElementSceneIndex sceneIndex = sceneIter->first;
		long lCharType = sceneIter->second;

		int iCharIndex = Invalid_AvatarElementCharIndex;
		CameraMode iCameraMode = CM_HOOK_BOT;
		_extractAvatarElementInfoFromSceneIndex(sceneIter->first, iCharIndex, iCameraMode);

		AvatarElementPtr avatarElement = GetAvatar(lCharType, iCharIndex, iCameraMode);
		GD_ClientActorModel* pkModel = avatarElement->pkActorModel;
		if(pkModel != NULL && pkModel->IsLoaded())
		{
			//寻找攻击目标
			AvatarElementKeyId pkTargetKeyId = _getHookBotShootTargetKeyID();
			if(pkTargetKeyId  > Invalid_AvatarElementKeyId)
			{
				pkModel->PlayAnimation(RES_ANM_ATTACK_1, true);
				AvatarBullet* bullet = T_NEW_D AvatarBullet;
				bullet->iIndex = iCharIndex;
				bullet->fStartTime = fCurrentTime;
				bullet->fFlightTime = 1.0f;

				NiPoint3 kStartLocation = pkModel->GetFxDummyNodePosition(GD_ModelBase::FDT_HOBBY_R);
				bullet->uiKeyId = pkTargetKeyId;

				//创建子弹
				NiNodePtr pkFxDummyNode = NiNew NiNode;
				pkFxDummyNode->SetWorldTranslate(kStartLocation);

				CF_Bot::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Bot>(lCharType);
				if(pkEntry != NULL)
				{
					bullet->sBulletExplodeFx = pkEntry->_sBulletExplodeFx;

					int iFlyingFxId = rkFxManager.CreateFx(pkEntry->_sBulletFx, true);
					bullet->iFlyingFxId = iFlyingFxId;
					bullet->pkFxDummyNode = pkFxDummyNode;

					rkFxManager.BindFxToDummyNode(iFlyingFxId, NiSmartPointerCast(NiAVObject, bullet->pkFxDummyNode), true, false);
					rkFxManager.PlayFx(iFlyingFxId, fCurrentTime);
				}

				m_kBulletList.push_back(bullet);
			}
		}
	}
}

void UI_AvatarManager::_loadSceneMaps()
{
	AvatarElementSceneMap kEmptySceneMap;

	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_DEFAULT, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_ACTOR, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_VEHICLE, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_ARROW, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_DEVAEYE, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_GODWING, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_FABAO, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_WORLDMAP, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_PLATFORM, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_MENTAL, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_VEHICLEFIGHTING, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_TREASUREITEM, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_GAMESTORYWILDBOSS, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_SPIRITPET, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_SPIRITWEAPON, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_HIDDENWEAPON, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_GODWEAPON, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_PLOUGH, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_FAN, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_SUPERBOW, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_HOOK, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_SPIRITFAN, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_HOOKBOT, kEmptySceneMap));
	m_kAllSceneMaps.insert(AvatarElementSceneMaps::value_type(SMI_HOOKBOT_MONSTER, kEmptySceneMap));
}

void UI_AvatarManager::_unLoadSceneMaps()
{
	m_kAllSceneMaps.clear();
}

AvatarElementSceneMap& UI_AvatarManager::_getSceneMap(int iSceneMapIndex)
{
	if ( iSceneMapIndex < (int)SMI_DEFAULT || 
		iSceneMapIndex >= (int)SMI_MAX )
	{
		iSceneMapIndex = 0;
	}

	return _getSceneMap(SceneMapIndex(iSceneMapIndex));
}

AvatarElementSceneMap& UI_AvatarManager::_getSceneMap( UI_AvatarManager::SceneMapIndex eSceneMapIndex )
{
	AvatarElementSceneMaps::iterator iter = m_kAllSceneMaps.find(eSceneMapIndex);
	if ( iter != m_kAllSceneMaps.end() )
		return iter->second;

	return m_kAllSceneMaps[SMI_DEFAULT];
}

int UI_AvatarManager::_generateAvatarElementSceneIndex(int iCharIndex, CameraMode iCameraMode)
{
	return ((iCharIndex + 1) << 8) | ((int)iCameraMode);
}

void UI_AvatarManager::_extractAvatarElementInfoFromSceneIndex(AvatarElementSceneIndex sceneIndex, int& iCharIndex, CameraMode& eCameraMode)
{
	iCharIndex = ((sceneIndex >> 8) - 1);
	eCameraMode = GetCameraModeIntMapping(sceneIndex & 0x0FF);
}

long UI_AvatarManager::_getCharTypeFromScene(AvatarElementSceneIndex sceneIndex, const AvatarElementSceneMap& SceneMap)
{
	long lCharType = 0;

	AvatarElementSceneMap::const_iterator iter = SceneMap.find(sceneIndex);
	if ( iter != SceneMap.end() )
		lCharType = iter->second;

	return lCharType;
}

void UI_AvatarManager::_addCharTypeToScene(AvatarElementSceneIndex sceneIndex, long charTypeId, AvatarElementSceneMap& SceneMap)
{
	AvatarElementSceneMap::const_iterator iter = SceneMap.find(sceneIndex);
	T_ASSERT2( iter == SceneMap.end(), "有重复的avatar元素被加入avatar角色列表中！" );

	SceneMap.insert(AvatarElementSceneMap::value_type(sceneIndex, charTypeId));
}


void UI_AvatarManager::_removeCharTypeFromScene(AvatarElementSceneIndex sceneIndex, AvatarElementSceneMap& SceneMap)
{
	AvatarElementSceneMap::iterator iter = SceneMap.find(sceneIndex);
	//T_ASSERT2( iter != charTypeMap.end() );

	if ( iter != SceneMap.end() )
		SceneMap.erase(iter);
}

void UI_AvatarManager::_removeAllCharTypeScenes()
{
	AvatarElementSceneMaps::iterator iter = m_kAllSceneMaps.begin();
	AvatarElementSceneMaps::iterator iend = m_kAllSceneMaps.end();

	while( iter != iend )
	{
		AvatarElementSceneMap& kSceneMap = (iter++)->second; 
		kSceneMap.clear();
	}
}

AvatarElementPtr UI_AvatarManager::GetAvatar(AvatarElementKeyId keyId)
{
	AvatarElementPtr avatarElement = NULL;

	AvatarElementMap::iterator iter = m_kAllAvatarElements.find(keyId);
	if ( iter != m_kAllAvatarElements.end() )
		avatarElement = iter->second;

	return avatarElement;
}

AvatarElementPtr UI_AvatarManager::GetAvatar(long lCharType, int CharIndex, int iCameraMode)
{
	AvatarElementPtr avatarElement = NULL;

	CameraMode eCameraMode = GetCameraModeIntMapping(iCameraMode);
	if ( eCameraMode != CM_MAX_COUNT )
	{
		AvatarElementKeyId keyId = _generateAvatarElementKeyId(lCharType, CharIndex, eCameraMode);
		AvatarElementMap::iterator iter = m_kAllAvatarElements.find(keyId);
		if ( iter != m_kAllAvatarElements.end() )
			avatarElement = iter->second;
	}

	return avatarElement;
}

void UI_AvatarManager::RotateAvatar(long lCharType, int iCharIndex, int iCameraMode, float fRotAngle)
{
	AvatarElementPtr avatarElement = 
		GetAvatar(_generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)iCameraMode));

	if ( avatarElement )
	{
		NiMatrix3 kRotation;
		kRotation.MakeZRotation(fRotAngle / 180.0f * NI_PI);
		avatarElement->pkActorModel->SetRotate(kRotation * (avatarElement->pkActorModel->GetRotate()));
	}
}

void UI_AvatarManager::SetAvatarRotation(long lCharType, int iCharIndex, int iCameraMode, float fRotAngleX, float fRotAngleY, float fRotAngleZ)
{
	AvatarElementPtr avatarElement = 
		GetAvatar(_generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)iCameraMode));

	if ( avatarElement )
	{
		NiMatrix3 kRotation;
		NiMatrix3 kTempx = NiMatrix3::IDENTITY;
		NiMatrix3 kTempy = NiMatrix3::IDENTITY;
		NiMatrix3 kTempz = NiMatrix3::IDENTITY;
		kTempx.MakeXRotation(fRotAngleX/180.0f * NI_PI);
		kTempy.MakeYRotation(fRotAngleY/180.0f * NI_PI);
		kTempz.MakeZRotation(fRotAngleZ/180.0f * NI_PI);
		kRotation = kTempz * kTempy * kTempx;
		avatarElement->pkActorModel->SetRotate(kRotation);
	}
}

void UI_AvatarManager::RotatePlayerAvatar(long lCharType, int iCharIndex, float fRotAngle)
{
	if (lCharType <= 0 || iCharIndex < 0 || iCharIndex >= MAX_AVATAR_ACTOR_IN_GAME)
	{
		return;
	}

	AvatarElementPtr avatarElement = GetAvatar(_generateAvatarElementKeyId(lCharType, iCharIndex, CM_FULLBODY));
	if ( avatarElement )
	{
		NiMatrix3 kRotation;
		kRotation.MakeZRotation(fRotAngle / 180.0f * NI_PI);

		avatarElement->pkActorModel->SetRotate(kRotation * (avatarElement->pkActorModel->GetRotate()));
	}
}

void UI_AvatarManager::PlayAvatarAnim(long lCharType, int iCharIndex, int iCameraMode, const StringType& sAnimName, bool bForce, int iLoopNum /*= 1*/)
{
	if (lCharType <= 0 || iCharIndex < 0 || iCharIndex >= MAX_AVATAR_ACTOR_IN_GAME)
	{
		return;
	}

	AvatarElementPtr avatarElement = GetAvatar(_generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)iCameraMode));
	if ( avatarElement )
	{
		//lua脚本是utf8编码，文件系统是gbk的，需要进行转换
		GD_ClientActorModel* pkActorModel = avatarElement->pkActorModel;
		pkActorModel->PlayAnimation(CodeConvert::Utf8ToMbs(sAnimName, CP_gbk), bForce);
	}
}

void UI_AvatarManager::AttachAvatarFx(long lCharType, int iCharIndex, int iCameraMode, const StringType& sFxName, const NiPoint3 *pkPoint)
{
	AvatarElementPtr avatarElement = GetAvatar(_generateAvatarElementKeyId(lCharType, iCharIndex, (CameraMode)iCameraMode));
	if ( avatarElement )
	{
		GD_ClientActorModel* pkModel = avatarElement->pkActorModel;
		if(NULL != pkModel)
		{
			NiPoint3 kPoint(*pkPoint);
			switch(iCameraMode)
			{
			case CM_TOYPET_GAIN:
				CF_CharType::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
				if(pkData != NULL)
				{
					const mem::vector<int16_t>& pos = pkData->_Position;
					if(pos.size() >= 2)
					{
						kPoint.z = Int2Float(20 - pos[1]);
					}
				}
				break;
			}
			//lua脚本是utf8编码，文件系统是gbk的，需要进行转换
			pkModel->AttachFx(GD_ClientActorModel::FDT_GROUND, 0, false, CodeConvert::Utf8ToMbs(sFxName), &kPoint, NULL);
		}
	}
}

void UI_AvatarManager::DetachAvatarFx(long lCharType, int CharIndex, int iCameraMode, const StringType& sFxName)
{
	AvatarElementPtr avatarElement = GetAvatar(_generateAvatarElementKeyId(lCharType, CharIndex, (CameraMode)iCameraMode));
	if ( avatarElement )
	{
		GD_ClientActorModel* pkModel = avatarElement->pkActorModel;
		if(NULL != pkModel)
		{
			//lua脚本是utf8编码，文件系统是gbk的，需要进行转换
			pkModel->DetachFx(GD_ClientActorModel::FDT_GROUND, CodeConvert::Utf8ToMbs(sFxName));
		}
	}
}

AvatarElementKeyId UI_AvatarManager::_generateAvatarElementKeyId(long lCharType, int iCharIndex, CameraMode iCameraMode)
{
	return (((uint64_t)lCharType) << 16) 
		+ (((uint64_t)iCharIndex) << 8)
		+ (uint64_t)iCameraMode;
}

void UI_AvatarManager::_extractAvatarElementInfoFromKeyId( AvatarElementKeyId keyId, long& lCharType, int& iCharIndex, CameraMode& iCameraMode )
{
	lCharType = (long)(keyId >> 16);
	iCharIndex = (int)((keyId & 0xFF00) >> 8);
	iCameraMode = (CameraMode)(keyId & 0xFF);
}

AvatarElementPtr UI_AvatarManager::_createAvatarElement(AvatarElementKeyId keyId, SceneMapIndex sceneMapIndex, const StringType& renderGroup, const AvatarModelParam& modelParam)
{
	//@ 检查合法性
	long lCharType = Invalid_AvatarElementCharTypeId;
	int iCharIndex = Invalid_AvatarElementCharIndex;
	CameraMode eCameraMode = CM_MAX_COUNT;
	_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, (CameraMode)eCameraMode);
	if ( lCharType == Invalid_AvatarElementCharTypeId ||iCharIndex == Invalid_AvatarElementCharIndex || 
		eCameraMode == CM_MAX_COUNT )
	{
		return NULL;
	}

	AvatarElementMap::iterator iter = m_kAllAvatarElements.find( keyId );
	if ( iter != m_kAllAvatarElements.end() )
	{
		T_ASSERT2(false, "重复创建avatar元素！");
		return NULL;
	}

	//@ 创建新avatar
	AvatarElementPtr pkAvatarElement = T_NEW_D AvatarElement;
	pkAvatarElement->iSceneMapIndex = sceneMapIndex;
	pkAvatarElement->iRenderClickIndex = _getRenderClickIndex(eCameraMode);
	pkAvatarElement->iLife = 0;

	GD_ClientAsyncModelManager::LoadingModelInfo kLoadingModelInfo;
	kLoadingModelInfo.bManualUpdate = true;
	kLoadingModelInfo.bRenderToUITexture = true;
	kLoadingModelInfo.bRenderUltraMode = true;
	kLoadingModelInfo.ucRenderUltraModeIndex = 0;
	kLoadingModelInfo.iModelIndex = modelParam.m_iModelIndex;

	GD_ClientAsyncModelManager& pkAsyncModelManager = GD_ClientAsyncModelManager::GetSingleton();
	pkAvatarElement->pkActorModel = (GD_ClientActorModel*)pkAsyncModelManager.CreateModel(
		(GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE)modelParam.m_iModelType, 
		modelParam.m_szModFileName, kLoadingModelInfo, modelParam.m_bModelAsyncLoad );

	if ( pkAvatarElement->pkActorModel )
	{
		//1. 设置自我更新特效 
		pkAvatarElement->pkActorModel->SetManualUpdateEnable(true);

		//2. 先插入到总表
		m_kAllAvatarElements[keyId] = pkAvatarElement;

		//3. 插入到角色场景表
		_addCharTypeToScene(_generateAvatarElementSceneIndex(iCharIndex, eCameraMode), lCharType, _getSceneMap(sceneMapIndex));

		//4. 插入到绘制队列
		m_pkRender->AddObject(keyId, renderGroup);
	}
	else
	{
		T_SAFE_DELETE(pkAvatarElement)
		pkAvatarElement = NULL;
	}

	return pkAvatarElement;
}

int UI_AvatarManager::_getRenderClickIndex(CameraMode eMode)
{
	switch(eMode)
	{
	case CM_VEHICLE:
	case CM_DEVAEYE:
	case CM_DAILYRECHARGE_RIGHT:
	case CM_MUHUN:
	case CM_GREAT_KUNGFU_REMAIN2:
	case CM_TOYPET:
	case CM_TOYPET_GAIN:
	case CM_PRIVILEGE_VIP2:
	case CM_RUNESCAPE_TIP:
	case CM_FABAO:
		return RENDER_CLICK_SMALL + RENDER_CLICK_NODEPTHBUFFER;
	default:
		return RENDER_CLICK_ORIGIN;
	}
}

void UI_AvatarManager::_destroyAvatarElement(AvatarElementKeyId keyId, const StringType& renderGroup)
{
	AvatarElementMap::iterator iter = m_kAllAvatarElements.find(keyId);
	if (iter != m_kAllAvatarElements.end())
	{
		AvatarElementPtr pkAvatarElement = iter->second;


		//1. 从绘制队列移除
		m_pkRender->RemoveObject(keyId, renderGroup);

		//2. 从角色场景表移除
		long lCharType = Invalid_AvatarElementCharTypeId;
		int iCharIndex = Invalid_AvatarElementCharIndex;
		CameraMode iCameraMode = CM_MAX_COUNT;
		_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, (CameraMode)iCameraMode);
		AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, iCameraMode);
		_removeCharTypeFromScene( sceneIndex, _getSceneMap(pkAvatarElement->iSceneMapIndex));

		//3. 从总表移除
		m_kAllAvatarElements.erase(iter);

		//4. 卸载
		GD_ClientAsyncModelManager::GetSingleton().ReleaseModel(pkAvatarElement->pkActorModel);

		//5. 回收内存
		T_SAFE_DELETE(pkAvatarElement)
		pkAvatarElement = NULL;
	}
}

void UI_AvatarManager::_destroyExpiredAvatarElements(const AvatarElementKeyIdList& kElementKeyIdList)
{
	AvatarElementPtr pkAvatarElement = NULL;
	AvatarElementKeyId keyId = Invalid_AvatarElementKeyId;

	AvatarElementKeyIdList::const_iterator iter_keyId = kElementKeyIdList.begin();
	AvatarElementKeyIdList::const_iterator iend_keyId = kElementKeyIdList.end();

	while( iter_keyId != iend_keyId )
	{
		keyId = *(iter_keyId++);
		AvatarElementMap::iterator iter = m_kAllAvatarElements.find(keyId);
		if (iter != m_kAllAvatarElements.end())
		{
			pkAvatarElement = iter->second;

			//1. 不需要移除，因为expired的东西本身就不在active的renderGroup中
			// m_pkRender->RemoveObject(keyId, renderGroup);

			//2. 从角色场景表移除
			long lCharType = Invalid_AvatarElementCharTypeId;
			int iCharIndex = Invalid_AvatarElementCharIndex;
			CameraMode iCameraMode = CM_MAX_COUNT;
			_extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, (CameraMode)iCameraMode);
			AvatarElementSceneIndex sceneIndex = _generateAvatarElementSceneIndex(iCharIndex, iCameraMode);
			_removeCharTypeFromScene( sceneIndex, _getSceneMap(pkAvatarElement->iSceneMapIndex));

			//3. 从总表移除
			m_kAllAvatarElements.erase(iter);

			//4. 卸载
			GD_ClientAsyncModelManager::GetSingleton().ReleaseModel(pkAvatarElement->pkActorModel);

			//5. 回收内存
			T_SAFE_DELETE(pkAvatarElement)
			pkAvatarElement = NULL;
		}
	}
}

void UI_AvatarManager::_destroyAllAvatarBullets()
{
	AvatarBulletList::iterator iter = m_kBulletList.begin();
	AvatarBulletList::iterator iend = m_kBulletList.end();
	for ( ;iter != iend ;++iter )
	{
		T_SAFE_DELETE((*iter));
	}

	m_kBulletList.clear();
}

void UI_AvatarManager::_destroyAllAvatarElements()
{
	//1. 清空绘制队列
	m_pkRender->RemoveAllObjects();

	//2. 清空场景表
	_removeAllCharTypeScenes();

	//3. 卸载所有角色
	GD_ClientAsyncModelManager* pkModelManager = 
		GD_ClientAsyncModelManager::GetSingletonPtr();

	AvatarElementMap::iterator iter = m_kAllAvatarElements.begin();
	AvatarElementMap::iterator iend = m_kAllAvatarElements.end();
	for ( ;iter != iend ;++iter )
	{
		pkModelManager->ReleaseModel(iter->second->pkActorModel);
		T_SAFE_DELETE(iter->second);
	}

	m_kAllAvatarElements.clear();

}

AvatarElementPtr UI_AvatarManager::_createActorAvatarElement(AvatarElementKeyId keyId, SceneMapIndex eSceneMapIndex, const StringType& renderGroup )
{
	//@ 检查合法性 
	AvatarElementPtr pkAvatarElement = NULL;
	AvatarElementMap::iterator iter = m_kAllAvatarElements.find( keyId );
	if ( iter != m_kAllAvatarElements.end() )
	{
		T_ASSERT2(false, "重复创建avatar元素！");
		return NULL;
	}

	//@ preCreate ActorAvatar
    long lCharType = Invalid_AvatarElementCharTypeId;
    int iCharIndex = Invalid_AvatarElementCharIndex;
    CameraMode iCameraMode = CM_MAX_COUNT;
    _extractAvatarElementInfoFromKeyId(keyId, lCharType, iCharIndex, (CameraMode)iCameraMode);

	const CF_BuddyTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(lCharType);
	CF_ItemList& pkItemListCsv = CF_ItemList::GetSingleton();
	const CF_ItemList::DataEntry* pkItemData = pkItemListCsv.GetEntryPtr(lCharType);
	if (pkData == NULL && pkItemData == NULL)
		return pkAvatarElement;

	GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE eModelType = GD_ClientAsyncModelManager::AMT_PLAYER;
	StringType strModResName;
	bool bUseAsyncLoad = true;
	switch (iCameraMode)
    {
    case CM_FULLBODY:
    //case CM_FULLBODY_WITHOUT_VEHICLE:
    case CM_TEAM_FULLBODY:
	case CM_RUNESCAPE_FULLBODY:
	case CM_RUNESCAPE_VORPAL_BLADE:
	case CM_RUNESCAPE_EFFECT:
	case CM_ARENA_BATTLE_ACTOR:
	case CM_ARENA_BATTLE_ACTOR_OTHER:
	case CM_ESC_UI_CHAR:
	case CM_FRIEND_TIPS:
	case CM_RANKINGLIST:
	case CM_FLOWER:
	case CM_REWARD_RANK:
	case CM_RUNESCAPE_TIP:
	case CM_GEMBUFF:
	case CM_SKY_ARENA:
	case CM_FASHION:
	case CM_PEEK_BATTLEFIELD:
	case CM_CHARACTER:
	case CM_GREAT_KUNGFU_REMAIN:
	case CM_MULTI_BUDDY:
	case CM_ROYAL_CITY:
	case CM_RAID_SETTLE:
		{
			strModResName = pkData->_model;
			eModelType = GD_ClientAsyncModelManager::AMT_PLAYER;
		}		
		break;
	case CM_VIEWINFO:
	case CM_NONPAREIL_ITEM:
		{
			strModResName = pkData->_model;
			eModelType = GD_ClientAsyncModelManager::AMT_NPC;
		}
		break;
	
	case CM_TREASURE_CHESTS:
		{
			if (iCharIndex >= 10)
			{
				strModResName = pkData->_model;
				eModelType = GD_ClientAsyncModelManager::AMT_NPC;
				if (iCharIndex > 10)
					bUseAsyncLoad = false;
			}
			else
			{
				strModResName = pkItemData->_kModel;
				eModelType = GD_ClientAsyncModelManager::AMT_CHEST;
				bUseAsyncLoad = false;
			}
		}
		break;
    case CM_VEHICLE:
		return pkAvatarElement;
     }

	//@ 主逻辑，创建新的actorAvatar
	{
		AvatarModelParam modelParam(strModResName);
		modelParam.m_iModelType = (int)eModelType;
		modelParam.m_iModelIndex = 0;
		modelParam.m_bModelAsyncLoad = bUseAsyncLoad;

		pkAvatarElement = _createAvatarElement(keyId, eSceneMapIndex, renderGroup, modelParam);
		if ( !pkAvatarElement )
			return NULL;
	}

	//@ postCreate actorAvatar
	GD_ClientActorModel* pkActorModel = pkAvatarElement->pkActorModel;

	pkActorModel->SetScale(1.0f);
	pkActorModel->SetRotate(NiMatrix3::IDENTITY);
	pkActorModel->SetTranslate(NiPoint3::ZERO);

    switch (iCameraMode)
    {
	case CM_RUNESCAPE_FULLBODY:
	case CM_RUNESCAPE_VORPAL_BLADE:
	case CM_RUNESCAPE_EFFECT:
	case CM_RUNESCAPE_TIP:
	case CM_PEEK_BATTLEFIELD:
		{
			pkActorModel->SetVehicleModel(NULL);
			if (iCameraMode == CM_RUNESCAPE_FULLBODY || iCameraMode == CM_RUNESCAPE_VORPAL_BLADE)
			{
				float fScale = 1.0f;
				NiMatrix3 kRotation = NiMatrix3::IDENTITY;
				if (iCharIndex == 1)
				{
					kRotation.MakeZRotation(0.0f);
				}
				else if (iCharIndex & 1)
				{
					kRotation.MakeZRotation(30/180.0f * NI_PI);
				}
				else
				{
					kRotation.MakeZRotation(-30/180.0f * NI_PI);
				}
				pkActorModel->SetRotate(kRotation);
				if (iCameraMode == CM_RUNESCAPE_VORPAL_BLADE)
				{
					pkActorModel->SetTranslate(g_settings.RUNESCAPE_POSITION[iCharIndex - 1 + 5]);
					fScale = g_settings.RUNESCAPE_SCALE[iCharIndex - 1 + 5];
				}
				else
				{
					pkActorModel->SetTranslate(g_settings.RUNESCAPE_POSITION[iCharIndex - 1]);
					fScale = g_settings.RUNESCAPE_SCALE[iCharIndex - 1];
				}
				pkActorModel->SetScale(fScale);

			}
			else if(iCameraMode == CM_PEEK_BATTLEFIELD)
			{
				float fScale = 1.0f;
				NiMatrix3 kRotation = NiMatrix3::IDENTITY;
				if (iCharIndex == 1)
				{
					kRotation.MakeZRotation(0.0f);
				}
				else if (iCharIndex & 1)
				{
					kRotation.MakeZRotation(30/180.0f * NI_PI);
				}
				else
				{
					kRotation.MakeZRotation(-30/180.0f * NI_PI);
				}
				pkActorModel->SetRotate(kRotation);
				pkActorModel->SetTranslate(g_settings.PEEK_BATTLEFIELD_POSITION[iCharIndex - 1]);
				fScale = g_settings.PEEK_BATTLEFIELD_SCALE[iCharIndex - 1];

				pkActorModel->SetScale(fScale);
			}
			else if (iCameraMode == CM_RUNESCAPE_EFFECT)
			{
				if (iCharIndex == 6)
					pkActorModel->SetTranslate(g_settings.RUNESCAPE_POSITION[10]);
				else if(iCharIndex == 7)
					pkActorModel->SetTranslate(g_settings.RUNESCAPE_POSITION[11]);
			}
			else
				_rotateActorAvatarElementToFront(*pkActorModel, false);

			pkActorModel->SetVisibility(false);
			if (lCharType == MALE_TYPE || lCharType == FEMALE_TYPE)
				pkActorModel->SetClothingChangeable(false);
			pkActorModel->SetIsUpdatable(false);

			if(CM_RUNESCAPE_TIP == iCameraMode)
				pkActorModel->SetTranslate(NiPoint3::ZERO);
		}
        break;
    case CM_VEHICLE:
	case CM_VIEWINFO:
       pkActorModel->SetTranslate(g_settings.VIEWINFO_ACTOR_POSITION);
		break;
    //case CM_FULLBODY_WITHOUT_VEHICLE:
	case CM_TEAM_FULLBODY:
		{
			_rotateActorAvatarElementToFront(*pkActorModel, true);
			pkActorModel->SetTranslate(g_settings.TEAM_ACTOR_POSITION);
		}
		break;
	case CM_ARENA_BATTLE_ACTOR:
	case CM_ARENA_BATTLE_ACTOR_OTHER:
		{
			_rotateActorAvatarElementToFront(*pkActorModel, true);
			pkActorModel->SetTranslate(g_settings.ARENA_BATTLE_ACTOR_POSITION[iCharIndex]);
		}
		break;
	case CM_MULTI_BUDDY:
		{
			_rotateActorAvatarElementToFront(*pkActorModel, true);
			pkActorModel->SetTranslate(g_settings.MULTI_BUDDY_ACTOR_POSITION[iCharIndex]);
		}
		break;
	case CM_ROYAL_CITY:
		{
			_rotateActorAvatarElementToFront(*pkActorModel, true);
			pkActorModel->SetTranslate(g_settings.ROYALCITY_BUDDY_ACTOR_POSITION[iCharIndex]);
		}
		break;
	
	case CM_ESC_UI_CHAR:
	case CM_RANKINGLIST:
	case CM_FLOWER:
	case CM_REWARD_RANK:
	case CM_CHARACTER:
	case CM_GEMBUFF:
	case CM_SKY_ARENA:
	case CM_FASHION:
	case CM_BONESOUL:
		{
			pkActorModel->SetVisibility(false);
			
			if(iCameraMode == CM_ESC_UI_CHAR || iCameraMode == CM_FASHION || iCameraMode == CM_GEMBUFF)
				pkActorModel->SetTranslate(g_settings.ESC_ACTOR_POSITION);
			else if(iCameraMode == CM_SKY_ARENA)
				pkActorModel->SetTranslate(g_settings.CAMERA_UI_AVATAR_FULL_SKYARENA_PLAYERPOS);
			else
				pkActorModel->SetTranslate(g_settings.VIEWINFO_ACTOR_POSITION);
			
			pkActorModel->SetIsUpdatable(false);
		}
		break;
	case CM_FRIEND_TIPS:
		{
			pkActorModel->SetVisibility(false);
			if(lCharType == MALE_TYPE || lCharType == FEMALE_TYPE)
			{
				pkActorModel->SetClothingChangeable(false);
				/*NiMatrix3 kRotation = NiMatrix3::IDENTITY;
				kRotation.MakeZRotation(95.0f / 180.0f * NI_PI);
				pkModel->SetRotate(kRotation);*/

				pkActorModel->SetTranslate(g_settings.UI_FRIEND_TIPS_ACTOR_POSITION);
			}
			pkActorModel->SetIsUpdatable(false);
		}
		break;
	case CM_TREASURE_CHESTS:
		{
			if (iCharIndex == 10)
			{
				pkActorModel->SetTranslate(g_settings.CAMERA_UI_TREASURE_CHESTS_POS);
				pkActorModel->SetScale(g_settings.CAMERA_UI_TREASURE_CHESTS_SCALE);
			}
			else
			{
				if (iCharIndex > 10)
				{
					pkActorModel->SetTranslate(g_settings.CAMERA_UI_TREASURE_CHESTS_ITEM[iCharIndex-1]);
				}
				else
				{
					pkActorModel->SetTranslate(g_settings.CAMERA_UI_TREASURE_CHESTS_ITEM[iCharIndex]);
				}
				pkActorModel->SetScale(3.0f);
			}
		}
		break;
    }

    return pkAvatarElement;
}



void UI_AvatarManager::_rotateActorAvatarElementToFront(GD_ClientActorModel& rkActor, bool bIsFullBodyWithVehicle)
{
    rkActor.SetTranslate(NiPoint3::ZERO);

    NiMatrix3 kRotation = NiMatrix3::IDENTITY;
    if (!bIsFullBodyWithVehicle)
    {
        // 将人物旋转到正面
        kRotation.MakeZRotation(0.0f);
    }
    rkActor.SetRotate(kRotation);
}

void UI_AvatarManager::_changeActorAvatarAttachedItem(GD_ClientActorModel& rkActorModel, 
													  int iChangeType, long lItemID, long lCharType)
{
	if (IS_ITEM_ID(lItemID))
	{
		LC_ClientItem* pkItem = (LC_ClientItem*)LC_ItemFactoryBase::GetSingleton().RequestItem(lItemID);
		if (pkItem != NULL)
		{
			pkItem->OperationAvatarFunc(lCharType, &rkActorModel, NULL, 0.0f);
		}
	}
	else
	{
		if (iChangeType == 3)
		{
			rkActorModel.ChangeWeapon((GD_WeaponModel::WeaponSuitType)iChangeType, "");
		}
		else
		{
			rkActorModel.ChangeClothing((GD_ClothingModel::ClothingType)iChangeType, "");
		}
	}
}

void UI_AvatarManager::ReloadCameraSetting(void)
{
	/*
		这是一个debug接口，没有被调用；
	*/

	g_settings.Load(true);
	g_settings.UpdateSettings();

	_loadCameraSetting();
}

UI_AvatarManager::CameraMode UI_AvatarManager::GetCameraModeIntMapping(int iCameraMode)
{
	if ( iCameraMode < 0 || iCameraMode >= (int)CM_MAX_COUNT )
		return CM_MAX_COUNT;

	return (CameraMode)iCameraMode;
}

int	UI_AvatarManager::GetShaderParamIndexCameraModeMapping(CameraMode eCameraMode)
{
	int iShaderParamIndex = 0;
	switch(eCameraMode)
	{
	case CM_TREASURE_CHESTS:
		iShaderParamIndex = 2;
		break;
	default:
		iShaderParamIndex = 0;
	}

	return iShaderParamIndex;
}

void UI_AvatarManager::_loadCameraSetting(void)
{
	m_fFov[CM_DAILYRECHARGE_LEFT]				        = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_DAILYRECHARGE_LEFT]                   = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_DAILYRECHARGE_LEFT]			            = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_DAILYRECHARGE_LEFT]		        = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_DISTANCE;
	m_fNearCameraHeightBias[CM_DAILYRECHARGE_LEFT]	= g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_DAILYRECHARGE_LEFT], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[CM_DAILYRECHARGE_LEFT]);
	_calcCameraRotationMatrix(m_fPitch[CM_DAILYRECHARGE_LEFT], m_fYaw[CM_DAILYRECHARGE_LEFT], m_akCameraRotation[CM_DAILYRECHARGE_LEFT]);

	m_fFov[CM_FIRST_RECHARGE_ADV]                     = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FIRST_RECHARGE_ADV]                    = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FIRST_RECHARGE_ADV]                     = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FIRST_RECHARGE_ADV]                = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_DISTANCE;
	m_fNearCameraHeightBias[CM_FIRST_RECHARGE_ADV]	  = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FIRST_RECHARGE_ADV], (float)TOYPET_WIDTH / (float)TOYPET_HEIGHT, m_akFrustums[CM_FIRST_RECHARGE_ADV]);
	_calcCameraRotationMatrix(m_fPitch[CM_FIRST_RECHARGE_ADV], m_fYaw[CM_FIRST_RECHARGE_ADV], m_akCameraRotation[CM_FIRST_RECHARGE_ADV]);

	m_fFov[CM_AUTOPICK_ADV]                     = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_AUTOPICK_ADV]                    = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_AUTOPICK_ADV]                     = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_AUTOPICK_ADV]                = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_DISTANCE;
	m_fNearCameraHeightBias[CM_AUTOPICK_ADV]	  = g_settings.CAMERA_UI_AVATAR_DAILYRECHARGE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_AUTOPICK_ADV], (float)TOYPET_WIDTH / (float)TOYPET_HEIGHT, m_akFrustums[CM_AUTOPICK_ADV]);
	_calcCameraRotationMatrix(m_fPitch[CM_AUTOPICK_ADV], m_fYaw[CM_AUTOPICK_ADV], m_akCameraRotation[CM_AUTOPICK_ADV]);
	

	m_fFov[CM_TEAM_FULLBODY]                     = g_settings.CAMERA_UI_TEAM_FOV * NI_PI / 180.0f;
	m_fPitch[CM_TEAM_FULLBODY]                    = g_settings.CAMERA_UI_TEAM_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_TEAM_FULLBODY]                     = g_settings.CAMERA_UI_TEAM_YAW * NI_PI / 180.0f;
	m_fDistance[CM_TEAM_FULLBODY]                = g_settings.CAMERA_UI_TEAM_DISTANCE;
	m_fNearCameraHeightBias[CM_TEAM_FULLBODY]	  = g_settings.CAMERA_UI_TEAM_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_TEAM_FULLBODY], (float)FULL_CHAR_WIDTH2 / (float)FULL_CHAR_HEIGHT2, m_akFrustums[CM_TEAM_FULLBODY]);
	_calcCameraRotationMatrix(m_fPitch[CM_TEAM_FULLBODY], m_fYaw[CM_TEAM_FULLBODY], m_akCameraRotation[CM_TEAM_FULLBODY]);

	m_fFov[CM_RUNESCAPE_FULLBODY]                     = g_settings.CAMERA_UI_RUNESCAPE_FOV[0] * NI_PI / 180.0f;
	m_fPitch[CM_RUNESCAPE_FULLBODY]                    = g_settings.CAMERA_UI_RUNESCAPE_PITCH[0] * NI_PI / 180.0f;
	m_fYaw[CM_RUNESCAPE_FULLBODY]                     = g_settings.CAMERA_UI_RUNESCAPE_YAW[0] * NI_PI / 180.0f;
	m_fDistance[CM_RUNESCAPE_FULLBODY]                = g_settings.CAMERA_UI_RUNESCAPE_DISTANCE[0];
	m_fNearCameraHeightBias[CM_RUNESCAPE_FULLBODY]	  = g_settings.CAMERA_UI_RUNESCAPE_NEAR_HEIGHT_BIAS[0];
	_calcCameraFrustum(m_fFov[CM_RUNESCAPE_FULLBODY], (float)RUNESCAPE_WIDTH / (float)RUNESCAPE_HEIGHT, m_akFrustums[CM_RUNESCAPE_FULLBODY]);
	_calcCameraRotationMatrix(m_fPitch[CM_RUNESCAPE_FULLBODY], m_fYaw[CM_RUNESCAPE_FULLBODY], m_akCameraRotation[CM_RUNESCAPE_FULLBODY]);

	m_fFov[CM_PEEK_BATTLEFIELD]                     = g_settings.CAMERA_UI_RUNESCAPE_FOV[0] * NI_PI / 180.0f;
	m_fPitch[CM_PEEK_BATTLEFIELD]                    = g_settings.CAMERA_UI_RUNESCAPE_PITCH[0] * NI_PI / 180.0f;
	m_fYaw[CM_PEEK_BATTLEFIELD]                     = g_settings.CAMERA_UI_RUNESCAPE_YAW[0] * NI_PI / 180.0f;
	m_fDistance[CM_PEEK_BATTLEFIELD]                = g_settings.CAMERA_UI_RUNESCAPE_DISTANCE[0];
	m_fNearCameraHeightBias[CM_PEEK_BATTLEFIELD]	  = g_settings.CAMERA_UI_RUNESCAPE_NEAR_HEIGHT_BIAS[0];
	_calcCameraFrustum(m_fFov[CM_PEEK_BATTLEFIELD], (float)ARENA_WIDTH / (float)ARENA_HEIGHT, m_akFrustums[CM_PEEK_BATTLEFIELD]);
	_calcCameraRotationMatrix(m_fPitch[CM_PEEK_BATTLEFIELD], m_fYaw[CM_PEEK_BATTLEFIELD], m_akCameraRotation[CM_PEEK_BATTLEFIELD]);

	m_fFov[CM_CHAIN_SOUL]                     = g_settings.CAMERA_UI_CHAINSOUL_FOV * NI_PI / 180.0f;
	m_fPitch[CM_CHAIN_SOUL]                    = g_settings.CAMERA_UI_CHAINSOUL_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_CHAIN_SOUL]                     = g_settings.CAMERA_UI_CHAINSOUL_YAW * NI_PI / 180.0f;
	m_fDistance[CM_CHAIN_SOUL]                = g_settings.CAMERA_UI_CHAINSOUL_DISTANCE;
	m_fNearCameraHeightBias[CM_CHAIN_SOUL]	  = g_settings.CAMERA_UI_CHAINSOUL_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_CHAIN_SOUL], (float)ARENA_WIDTH / (float)ARENA_HEIGHT, m_akFrustums[CM_CHAIN_SOUL]);
	_calcCameraRotationMatrix(m_fPitch[CM_CHAIN_SOUL], m_fYaw[CM_CHAIN_SOUL], m_akCameraRotation[CM_CHAIN_SOUL]);

	m_fFov[CM_ALLIANCEMAP]                     = g_settings.CAMERA_UI_ALLIANCEMAP_FOV * NI_PI / 180.0f;
	m_fPitch[CM_ALLIANCEMAP]                    = g_settings.CAMERA_UI_ALLIANCEMAP_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_ALLIANCEMAP]                     = g_settings.CAMERA_UI_ALLIANCEMAP_YAW * NI_PI / 180.0f;
	m_fDistance[CM_ALLIANCEMAP]                = g_settings.CAMERA_UI_ALLIANCEMAP_DISTANCE;
	m_fNearCameraHeightBias[CM_ALLIANCEMAP]	  = g_settings.CAMERA_UI_ALLIANCEMAP_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_ALLIANCEMAP], (float)RUNESCAPE_WIDTH / (float)RUNESCAPE_HEIGHT, m_akFrustums[CM_ALLIANCEMAP]);
	_calcCameraRotationMatrix(m_fPitch[CM_ALLIANCEMAP], m_fYaw[CM_ALLIANCEMAP], m_akCameraRotation[CM_ALLIANCEMAP]);

	m_fFov[CM_DAILYRECHARGE_RIGHT]                     = g_settings.CAMERA_UI_DAILYRECHARGE_R_FOV * NI_PI / 180.0f;
	m_fPitch[CM_DAILYRECHARGE_RIGHT]                    = g_settings.CAMERA_UI_DAILYRECHARGE_R_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_DAILYRECHARGE_RIGHT]                     = g_settings.CAMERA_UI_DAILYRECHARGE_R_YAW * NI_PI / 180.0f;
	m_fDistance[CM_DAILYRECHARGE_RIGHT]                = g_settings.CAMERA_UI_DAILYRECHARGE_R_DISTANCE;
	m_fNearCameraHeightBias[CM_DAILYRECHARGE_RIGHT]	  = g_settings.CAMERA_UI_DAILYRECHARGE_R_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_DAILYRECHARGE_RIGHT], (float)RUNESCAPE_WIDTH / (float)RUNESCAPE_HEIGHT, m_akFrustums[CM_DAILYRECHARGE_RIGHT]);
	_calcCameraRotationMatrix(m_fPitch[CM_DAILYRECHARGE_RIGHT], m_fYaw[CM_DAILYRECHARGE_RIGHT], m_akCameraRotation[CM_DAILYRECHARGE_RIGHT]);

	m_fFov[CM_PRIVILEGE_VIP1]                     = g_settings.CAMERA_UI_PRIVILEGE_VIP1_FOV * NI_PI / 180.0f;
	m_fPitch[CM_PRIVILEGE_VIP1]                    = g_settings.CAMERA_UI_PRIVILEGE_VIP1_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_PRIVILEGE_VIP1]                     = g_settings.CAMERA_UI_PRIVILEGE_VIP1_YAW * NI_PI / 180.0f;
	m_fDistance[CM_PRIVILEGE_VIP1]                = g_settings.CAMERA_UI_PRIVILEGE_VIP1_DISTANCE;
	m_fNearCameraHeightBias[CM_PRIVILEGE_VIP1]	  = g_settings.CAMERA_UI_PRIVILEGE_VIP1_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_PRIVILEGE_VIP1], (float)GAMESTORY_WILDBOSS_WIDTH / (float)GAMESTORY_WILDBOSS_HEIGHT, m_akFrustums[CM_PRIVILEGE_VIP1]);
	_calcCameraRotationMatrix(m_fPitch[CM_PRIVILEGE_VIP1], m_fYaw[CM_PRIVILEGE_VIP1], m_akCameraRotation[CM_PRIVILEGE_VIP1]);

	m_fFov[CM_SPECIALTRAIN]                     = g_settings.CAMERA_UI_PRIVILEGE_VIP1_FOV * NI_PI / 180.0f;
	m_fPitch[CM_SPECIALTRAIN]                    = g_settings.CAMERA_UI_PRIVILEGE_VIP1_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_SPECIALTRAIN]                     = g_settings.CAMERA_UI_PRIVILEGE_VIP1_YAW * NI_PI / 180.0f;
	m_fDistance[CM_SPECIALTRAIN]                = g_settings.CAMERA_UI_PRIVILEGE_VIP1_DISTANCE;
	m_fNearCameraHeightBias[CM_SPECIALTRAIN]	  = g_settings.CAMERA_UI_PRIVILEGE_VIP1_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_SPECIALTRAIN], (float)GAMESTORY_WILDBOSS_WIDTH / (float)GAMESTORY_WILDBOSS_HEIGHT, m_akFrustums[CM_SPECIALTRAIN]);
	_calcCameraRotationMatrix(m_fPitch[CM_SPECIALTRAIN], m_fYaw[CM_SPECIALTRAIN], m_akCameraRotation[CM_SPECIALTRAIN]);



	m_fFov[CM_PRIVILEGE_VIP2]                     = g_settings.CAMERA_UI_PRIVILEGE_VIP2_FOV * NI_PI / 180.0f;
	m_fPitch[CM_PRIVILEGE_VIP2]                    = g_settings.CAMERA_UI_PRIVILEGE_VIP2_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_PRIVILEGE_VIP2]                     = g_settings.CAMERA_UI_PRIVILEGE_VIP2_YAW * NI_PI / 180.0f;
	m_fDistance[CM_PRIVILEGE_VIP2]                = g_settings.CAMERA_UI_PRIVILEGE_VIP2_DISTANCE;
	m_fNearCameraHeightBias[CM_PRIVILEGE_VIP2]	  = g_settings.CAMERA_UI_PRIVILEGE_VIP2_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_PRIVILEGE_VIP2], (float)RUNESCAPE_WIDTH / (float)RUNESCAPE_HEIGHT, m_akFrustums[CM_PRIVILEGE_VIP2]);
	_calcCameraRotationMatrix(m_fPitch[CM_PRIVILEGE_VIP2], m_fYaw[CM_PRIVILEGE_VIP2], m_akCameraRotation[CM_PRIVILEGE_VIP2]);

	m_fFov[CM_MIJI]                     = g_settings.CAMERA_UI_MIJI_FOV * NI_PI / 180.0f;
	m_fPitch[CM_MIJI]                    = g_settings.CAMERA_UI_MIJI_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_MIJI]                     = g_settings.CAMERA_UI_MIJI_YAW * NI_PI / 180.0f;
	m_fDistance[CM_MIJI]                = g_settings.CAMERA_UI_MIJI_DISTANCE;
	m_fNearCameraHeightBias[CM_MIJI]	  = g_settings.CAMERA_UI_MIJI_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_MIJI], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_MIJI]);
	_calcCameraRotationMatrix(m_fPitch[CM_MIJI], m_fYaw[CM_MIJI], m_akCameraRotation[CM_MIJI]);

	m_fFov[CM_ACHIVEMENT]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_ACHIVEMENT]				= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_ACHIVEMENT]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_ACHIVEMENT]		        = g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_DISTANCE;
	m_fNearCameraHeightBias[CM_ACHIVEMENT]	= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_ACHIVEMENT], (float)ACHIVEMENT_WIDTH / (float)ACHIVEMENT_HEIGHT, m_akFrustums[CM_ACHIVEMENT]);
	_calcCameraRotationMatrix(m_fPitch[CM_ACHIVEMENT], m_fYaw[CM_ACHIVEMENT], m_akCameraRotation[CM_ACHIVEMENT]);

	m_fFov[CM_VEHICLE_CRYSTAL]                     = g_settings.CAMERA_UI_MIJI_FOV * NI_PI / 180.0f;
	m_fPitch[CM_VEHICLE_CRYSTAL]                    = g_settings.CAMERA_UI_MIJI_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_VEHICLE_CRYSTAL]                     = g_settings.CAMERA_UI_MIJI_YAW * NI_PI / 180.0f;
	m_fDistance[CM_VEHICLE_CRYSTAL]                = g_settings.CAMERA_UI_MIJI_DISTANCE;
	m_fNearCameraHeightBias[CM_VEHICLE_CRYSTAL]	  = g_settings.CAMERA_UI_MIJI_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_VEHICLE_CRYSTAL], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_VEHICLE_CRYSTAL]);
	_calcCameraRotationMatrix(m_fPitch[CM_VEHICLE_CRYSTAL], m_fYaw[CM_VEHICLE_CRYSTAL], m_akCameraRotation[CM_VEHICLE_CRYSTAL]);

	m_fFov[CM_RUNESCAPE_EFFECT]                     = g_settings.CAMERA_UI_RUNESCAPE_FOV[1] * NI_PI / 180.0f;
	m_fPitch[CM_RUNESCAPE_EFFECT]                    = g_settings.CAMERA_UI_RUNESCAPE_PITCH[1] * NI_PI / 180.0f;
	m_fYaw[CM_RUNESCAPE_EFFECT]                     = g_settings.CAMERA_UI_RUNESCAPE_YAW[1] * NI_PI / 180.0f;
	m_fDistance[CM_RUNESCAPE_EFFECT]                = g_settings.CAMERA_UI_RUNESCAPE_DISTANCE[1];
	m_fNearCameraHeightBias[CM_RUNESCAPE_EFFECT]	  = g_settings.CAMERA_UI_RUNESCAPE_NEAR_HEIGHT_BIAS[1];
	_calcCameraFrustum(m_fFov[CM_RUNESCAPE_EFFECT], (float)RUNESCAPE_WIDTH / (float)RUNESCAPE_HEIGHT, m_akFrustums[CM_RUNESCAPE_EFFECT]);
	_calcCameraRotationMatrix(m_fPitch[CM_RUNESCAPE_EFFECT], m_fYaw[CM_RUNESCAPE_EFFECT], m_akCameraRotation[CM_RUNESCAPE_EFFECT]);

	m_fFov[CM_RUNESCAPE_TIP]				    = g_settings.CAMERA_UI_RUNESCAPE_TIP_FOV * NI_PI / 180.0f;
	m_fPitch[CM_RUNESCAPE_TIP]                  = g_settings.CAMERA_UI_RUNESCAPE_TIP_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_RUNESCAPE_TIP]			        = g_settings.CAMERA_UI_RUNESCAPE_TIP_YAW * NI_PI / 180.0f;
	m_fDistance[CM_RUNESCAPE_TIP]		        = g_settings.CAMERA_UI_RUNESCAPE_TIP_DISTANCE;
	m_fNearCameraHeightBias[CM_RUNESCAPE_TIP]   = g_settings.CAMERA_UI_RUNESCAPE_TIP_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_RUNESCAPE_TIP], (float)RUNESCAPE_TIP_WIDTH / (float)RUNESCAPE_TIP_HEIGHT, m_akFrustums[CM_RUNESCAPE_TIP]);
	_calcCameraRotationMatrix(m_fPitch[CM_RUNESCAPE_TIP], m_fYaw[CM_RUNESCAPE_TIP], m_akCameraRotation[CM_RUNESCAPE_TIP]);

	m_fFov[CM_GODWEAPON_TIP]				    = g_settings.CAMERA_UI_RUNESCAPE_TIP_FOV * NI_PI / 180.0f;
	m_fPitch[CM_GODWEAPON_TIP]                  = g_settings.CAMERA_UI_RUNESCAPE_TIP_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_GODWEAPON_TIP]			        = g_settings.CAMERA_UI_RUNESCAPE_TIP_YAW * NI_PI / 180.0f;
	m_fDistance[CM_GODWEAPON_TIP]		        = g_settings.CAMERA_UI_RUNESCAPE_TIP_DISTANCE;
	m_fNearCameraHeightBias[CM_GODWEAPON_TIP]   = g_settings.CAMERA_UI_RUNESCAPE_TIP_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_GODWEAPON_TIP], (float)RUNESCAPE_TIP_WIDTH / (float)RUNESCAPE_TIP_HEIGHT, m_akFrustums[CM_GODWEAPON_TIP]);
	_calcCameraRotationMatrix(m_fPitch[CM_GODWEAPON_TIP], m_fYaw[CM_GODWEAPON_TIP], m_akCameraRotation[CM_GODWEAPON_TIP]);
	
	m_fFov[CM_FLOWER]                     = g_settings.CAMERA_UI_GUILDRAID_BOSS_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FLOWER]                    = g_settings.CAMERA_UI_GUILDAID_BOSS_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FLOWER]                     = g_settings.CAMERA_UI_GUILDAID_BOSS_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FLOWER]                = g_settings.CAMERA_UI_GUILDAID_BOSS_DISTANCE;
	m_fNearCameraHeightBias[CM_FLOWER]	  = g_settings.CAMERA_UI_GUILDAID_BOSS_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FLOWER], (float)SENDFLOWER_WIDTH / (float)SENDFLOWER_HEIGHT, m_akFrustums[CM_FLOWER]);
	_calcCameraRotationMatrix(m_fPitch[CM_FLOWER], m_fYaw[CM_FLOWER], m_akCameraRotation[CM_FLOWER]);

	m_fFov[CM_GUILD_RAID_BUST]                     = g_settings.CAMERA_UI_GUILDRAID_BUST_FOV * NI_PI / 180.0f;
	m_fPitch[CM_GUILD_RAID_BUST]                    = g_settings.CAMERA_UI_GUILDAID_BUST_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_GUILD_RAID_BUST]                     = g_settings.CAMERA_UI_GUILDAID_BUST_YAW * NI_PI / 180.0f;
	m_fDistance[CM_GUILD_RAID_BUST]                = g_settings.CAMERA_UI_GUILDAID_BUST_DISTANCE;
	m_fNearCameraHeightBias[CM_GUILD_RAID_BUST]	  = g_settings.CAMERA_UI_GUILDAID_BUST_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_GUILD_RAID_BUST], (float)ESC_UI_CHAR_WIDTH / (float)ESC_UI_CHAR_HEIGHT, m_akFrustums[CM_GUILD_RAID_BUST]);
	_calcCameraRotationMatrix(m_fPitch[CM_GUILD_RAID_BUST], m_fYaw[CM_GUILD_RAID_BUST], m_akCameraRotation[CM_GUILD_RAID_BUST]);
	
	m_fFov[CM_VEHICLE]				        = g_settings.CAMERA_UI_VEHICLE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_VEHICLE]		            = g_settings.CAMERA_UI_VEHICLE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_VEHICLE]			            = g_settings.CAMERA_UI_VEHICLE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_VEHICLE]		            = g_settings.CAMERA_UI_VEHICLE_DISTANCE;
	m_fNearCameraHeightBias[CM_VEHICLE]	    = g_settings.CAMERA_UI_VEHICLE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_VEHICLE], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_VEHICLE]);
	_calcCameraRotationMatrix(m_fPitch[CM_VEHICLE], m_fYaw[CM_VEHICLE], m_akCameraRotation[CM_VEHICLE]);

	m_fFov[CM_MUHUN]				        = g_settings.CAMERA_UI_MUHUN_FOV * NI_PI / 180.0f;
	m_fPitch[CM_MUHUN]		            = g_settings.CAMERA_UI_MUHUN_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_MUHUN]			            = g_settings.CAMERA_UI_MUHUN_YAW * NI_PI / 180.0f;
	m_fDistance[CM_MUHUN]		            = g_settings.CAMERA_UI_MUHUN_DISTANCE;
	m_fNearCameraHeightBias[CM_MUHUN]	    = g_settings.CAMERA_UI_MUHUN_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_MUHUN], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_MUHUN]);
	_calcCameraRotationMatrix(m_fPitch[CM_MUHUN], m_fYaw[CM_MUHUN], m_akCameraRotation[CM_MUHUN]);

	m_fFov[CM_ARROW]				    = g_settings.CAMERA_UI_ARROW_FOV * NI_PI / 180.0f;
	m_fPitch[CM_ARROW]                  = g_settings.CAMERA_UI_ARROW_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_ARROW]			        = g_settings.CAMERA_UI_ARROW_YAW * NI_PI / 180.0f;
	m_fDistance[CM_ARROW]		        = g_settings.CAMERA_UI_ARROW_DISTANCE;
	m_fNearCameraHeightBias[CM_ARROW]   = g_settings.CAMERA_UI_ARROW_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_ARROW], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_ARROW]);
	_calcCameraRotationMatrix(m_fPitch[CM_ARROW], m_fYaw[CM_ARROW], m_akCameraRotation[CM_ARROW]);

	m_fFov[CM_SPIRIT_PET]				    = g_settings.CAMERA_UI_SPIRIT_PET_FOV * NI_PI / 180.0f;
	m_fPitch[CM_SPIRIT_PET]                  = g_settings.CAMERA_UI_SPIRIT_PET_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_SPIRIT_PET]			        = g_settings.CAMERA_UI_SPIRIT_PET_YAW * NI_PI / 180.0f;
	m_fDistance[CM_SPIRIT_PET]		        = g_settings.CAMERA_UI_SPIRIT_PET_DISTANCE;
	m_fNearCameraHeightBias[CM_SPIRIT_PET]   = g_settings.CAMERA_UI_SPIRIT_PET_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_SPIRIT_PET], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_SPIRIT_PET]);
	_calcCameraRotationMatrix(m_fPitch[CM_SPIRIT_PET], m_fYaw[CM_SPIRIT_PET], m_akCameraRotation[CM_SPIRIT_PET]);

	m_fFov[CM_SPIRIT_WEAPON]				    = g_settings.CAMERA_UI_SPIRIT_WEAPON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_SPIRIT_WEAPON]                  = g_settings.CAMERA_UI_SPIRIT_WEAPON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_SPIRIT_WEAPON]			        = g_settings.CAMERA_UI_SPIRIT_WEAPON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_SPIRIT_WEAPON]		        = g_settings.CAMERA_UI_SPIRIT_WEAPON_DISTANCE;
	m_fNearCameraHeightBias[CM_SPIRIT_WEAPON]   = g_settings.CAMERA_UI_SPIRIT_WEAPON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_SPIRIT_WEAPON], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_SPIRIT_WEAPON]);
	_calcCameraRotationMatrix(m_fPitch[CM_SPIRIT_WEAPON], m_fYaw[CM_SPIRIT_WEAPON], m_akCameraRotation[CM_SPIRIT_WEAPON]);

	m_fFov[CM_HIDDEN_WEAPON]				    = g_settings.CAMERA_UI_HIDDEN_WEAPON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_HIDDEN_WEAPON]                  = g_settings.CAMERA_UI_HIDDEN_WEAPON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_HIDDEN_WEAPON]			        = g_settings.CAMERA_UI_HIDDEN_WEAPON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_HIDDEN_WEAPON]		        = g_settings.CAMERA_UI_HIDDEN_WEAPON_DISTANCE;
	m_fNearCameraHeightBias[CM_HIDDEN_WEAPON]   = g_settings.CAMERA_UI_HIDDEN_WEAPON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_HIDDEN_WEAPON], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_HIDDEN_WEAPON]);
	_calcCameraRotationMatrix(m_fPitch[CM_HIDDEN_WEAPON], m_fYaw[CM_HIDDEN_WEAPON], m_akCameraRotation[CM_HIDDEN_WEAPON]);

	m_fFov[CM_GOD_WEAPON]				    = g_settings.CAMERA_UI_GOD_WEAPON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_GOD_WEAPON]                  = g_settings.CAMERA_UI_GOD_WEAPON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_GOD_WEAPON]			        = g_settings.CAMERA_UI_GOD_WEAPON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_GOD_WEAPON]		        = g_settings.CAMERA_UI_GOD_WEAPON_DISTANCE;
	m_fNearCameraHeightBias[CM_GOD_WEAPON]   = g_settings.CAMERA_UI_GOD_WEAPON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_GOD_WEAPON], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_GOD_WEAPON]);
	_calcCameraRotationMatrix(m_fPitch[CM_GOD_WEAPON], m_fYaw[CM_GOD_WEAPON], m_akCameraRotation[CM_GOD_WEAPON]);

	m_fFov[CM_PLOUGH]				    = g_settings.CAMERA_UI_PLOUGH_FOV * NI_PI / 180.0f;
	m_fPitch[CM_PLOUGH]                  = g_settings.CAMERA_UI_PLOUGH_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_PLOUGH]			        = g_settings.CAMERA_UI_PLOUGH_YAW * NI_PI / 180.0f;
	m_fDistance[CM_PLOUGH]		        = g_settings.CAMERA_UI_PLOUGH_DISTANCE;
	m_fNearCameraHeightBias[CM_PLOUGH]   = g_settings.CAMERA_UI_PLOUGH_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_PLOUGH], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_PLOUGH]);
	_calcCameraRotationMatrix(m_fPitch[CM_PLOUGH], m_fYaw[CM_PLOUGH], m_akCameraRotation[CM_PLOUGH]);

	m_fFov[CM_FAN]				    = g_settings.CAMERA_UI_FAN_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FAN]                  = g_settings.CAMERA_UI_FAN_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FAN]			        = g_settings.CAMERA_UI_FAN_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FAN]		        = g_settings.CAMERA_UI_FAN_DISTANCE;
	m_fNearCameraHeightBias[CM_FAN]   = g_settings.CAMERA_UI_FAN_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FAN], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_FAN]);
	_calcCameraRotationMatrix(m_fPitch[CM_FAN], m_fYaw[CM_FAN], m_akCameraRotation[CM_FAN]);

	m_fFov[CM_SUPERBOW]				    = g_settings.CAMERA_UI_SUPERBOW_FOV * NI_PI / 180.0f;
	m_fPitch[CM_SUPERBOW]                  = g_settings.CAMERA_UI_SUPERBOW_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_SUPERBOW]			        = g_settings.CAMERA_UI_SUPERBOW_YAW * NI_PI / 180.0f;
	m_fDistance[CM_SUPERBOW]		        = g_settings.CAMERA_UI_SUPERBOW_DISTANCE;
	m_fNearCameraHeightBias[CM_SUPERBOW]   = g_settings.CAMERA_UI_SUPERBOW_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_SUPERBOW], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_SUPERBOW]);
	_calcCameraRotationMatrix(m_fPitch[CM_SUPERBOW], m_fYaw[CM_SUPERBOW], m_akCameraRotation[CM_SUPERBOW]);
	
	m_fFov[CM_HOOK]				    = g_settings.CAMERA_UI_HOOK_FOV * NI_PI / 180.0f;
	m_fPitch[CM_HOOK]                  = g_settings.CAMERA_UI_HOOK_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_HOOK]			        = g_settings.CAMERA_UI_HOOK_YAW * NI_PI / 180.0f;
	m_fDistance[CM_HOOK]		        = g_settings.CAMERA_UI_HOOK_DISTANCE;
	m_fNearCameraHeightBias[CM_HOOK]   = g_settings.CAMERA_UI_HOOK_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_HOOK], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_HOOK]);
	_calcCameraRotationMatrix(m_fPitch[CM_HOOK], m_fYaw[CM_HOOK], m_akCameraRotation[CM_HOOK]);
	
	m_fFov[CM_SPIRIT_FAN]				    = g_settings.CAMERA_UI_SPIRITFAN_FOV * NI_PI / 180.0f;
	m_fPitch[CM_SPIRIT_FAN]                  = g_settings.CAMERA_UI_SPIRITFAN_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_SPIRIT_FAN]			        = g_settings.CAMERA_UI_SPIRITFAN_YAW * NI_PI / 180.0f;
	m_fDistance[CM_SPIRIT_FAN]		        = g_settings.CAMERA_UI_SPIRITFAN_DISTANCE;
	m_fNearCameraHeightBias[CM_SPIRIT_FAN]   = g_settings.CAMERA_UI_SPIRITFAN_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_SPIRIT_FAN], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_SPIRIT_FAN]);
	_calcCameraRotationMatrix(m_fPitch[CM_SPIRIT_FAN], m_fYaw[CM_SPIRIT_FAN], m_akCameraRotation[CM_SPIRIT_FAN]);

	m_fFov[CM_EVIL]				    = g_settings.CAMERA_UI_PLOUGH_FOV * NI_PI / 180.0f;
	m_fPitch[CM_EVIL]                  = g_settings.CAMERA_UI_PLOUGH_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_EVIL]			        = g_settings.CAMERA_UI_PLOUGH_YAW * NI_PI / 180.0f;
	m_fDistance[CM_EVIL]		        = g_settings.CAMERA_UI_PLOUGH_DISTANCE;
	m_fNearCameraHeightBias[CM_EVIL]   = g_settings.CAMERA_UI_PLOUGH_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_EVIL], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_EVIL]);
	_calcCameraRotationMatrix(m_fPitch[CM_EVIL], m_fYaw[CM_EVIL], m_akCameraRotation[CM_EVIL]);
	
	m_fFov[CM_HIDDEN_WEAPON_INFABAO]				    = g_settings.CAMERA_UI_HIDDEN_WEAPON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_HIDDEN_WEAPON_INFABAO]                  = g_settings.CAMERA_UI_HIDDEN_WEAPON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_HIDDEN_WEAPON_INFABAO]			        = g_settings.CAMERA_UI_HIDDEN_WEAPON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_HIDDEN_WEAPON_INFABAO]		        = g_settings.CAMERA_UI_HIDDEN_WEAPON_DISTANCE;
	m_fNearCameraHeightBias[CM_HIDDEN_WEAPON_INFABAO]   = g_settings.CAMERA_UI_HIDDEN_WEAPON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_HIDDEN_WEAPON], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[CM_HIDDEN_WEAPON_INFABAO]);
	_calcCameraRotationMatrix(m_fPitch[CM_HIDDEN_WEAPON_INFABAO], m_fYaw[CM_HIDDEN_WEAPON_INFABAO], m_akCameraRotation[CM_HIDDEN_WEAPON_INFABAO]);

	m_fFov[CM_GOD_WEAPON_PREVIEW]				    = g_settings.CAMERA_UI_GODWEAPON_PREVIEW_FOV * NI_PI / 180.0f;
	m_fPitch[CM_GOD_WEAPON_PREVIEW]                  = g_settings.CAMERA_UI_GODWEAPON_PREVIEW_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_GOD_WEAPON_PREVIEW]			        = g_settings.CAMERA_UI_GODWEAPON_PREVIEW_YAW * NI_PI / 180.0f;
	m_fDistance[CM_GOD_WEAPON_PREVIEW]		        = g_settings.CAMERA_UI_GODWEAPON_PREVIEW_DISTANCE;
	m_fNearCameraHeightBias[CM_GOD_WEAPON_PREVIEW]   = g_settings.CAMERA_UI_GODWEAPON_PREVIEW_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_GOD_WEAPON_PREVIEW], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[CM_GOD_WEAPON_PREVIEW]);
	_calcCameraRotationMatrix(m_fPitch[CM_GOD_WEAPON_PREVIEW], m_fYaw[CM_GOD_WEAPON_PREVIEW], m_akCameraRotation[CM_GOD_WEAPON_PREVIEW]);
	
	m_fFov[CM_ARENA_BATTLE_ACTOR]				    = g_settings.CAMERA_UI_ARENA_BATTLE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_ARENA_BATTLE_ACTOR]                  = g_settings.CAMERA_UI_ARENA_BATTLE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_ARENA_BATTLE_ACTOR]			        = g_settings.CAMERA_UI_ARENA_BATTLE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_ARENA_BATTLE_ACTOR]		        = g_settings.CAMERA_UI_ARENA_BATTLE_DISTANCE;
	m_fNearCameraHeightBias[CM_ARENA_BATTLE_ACTOR]   = g_settings.CAMERA_UI_ARENA_BATTLE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_ARENA_BATTLE_ACTOR], (float)GAMESTORY_WILDBOSS_WIDTH / (float)GAMESTORY_WILDBOSS_HEIGHT, m_akFrustums[CM_ARENA_BATTLE_ACTOR]);
	_calcCameraRotationMatrix(m_fPitch[CM_ARENA_BATTLE_ACTOR], m_fYaw[CM_ARENA_BATTLE_ACTOR], m_akCameraRotation[CM_ARENA_BATTLE_ACTOR]);

	m_fFov[CM_ARENA_BATTLE_ACTOR_OTHER]				    = g_settings.CAMERA_UI_ARENA_BATTLE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_ARENA_BATTLE_ACTOR_OTHER]                  = g_settings.CAMERA_UI_ARENA_BATTLE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_ARENA_BATTLE_ACTOR_OTHER]			        = g_settings.CAMERA_UI_ARENA_BATTLE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_ARENA_BATTLE_ACTOR_OTHER]		        = g_settings.CAMERA_UI_ARENA_BATTLE_DISTANCE2;
	m_fNearCameraHeightBias[CM_ARENA_BATTLE_ACTOR_OTHER]   = g_settings.CAMERA_UI_ARENA_BATTLE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_ARENA_BATTLE_ACTOR_OTHER], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_ARENA_BATTLE_ACTOR_OTHER]);
	_calcCameraRotationMatrix(m_fPitch[CM_ARENA_BATTLE_ACTOR_OTHER], m_fYaw[CM_ARENA_BATTLE_ACTOR_OTHER], m_akCameraRotation[CM_ARENA_BATTLE_ACTOR_OTHER]);

	m_fFov[CM_MULTI_BUDDY]				    = g_settings.CAMERA_UI_ARENA_BATTLE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_MULTI_BUDDY]                  = g_settings.CAMERA_UI_ARENA_BATTLE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_MULTI_BUDDY]			        = g_settings.CAMERA_UI_ARENA_BATTLE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_MULTI_BUDDY]		        = g_settings.CAMERA_UI_ARENA_BATTLE_DISTANCE2;
	m_fNearCameraHeightBias[CM_MULTI_BUDDY]   = g_settings.CAMERA_UI_ARENA_BATTLE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_MULTI_BUDDY], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_MULTI_BUDDY]);
	_calcCameraRotationMatrix(m_fPitch[CM_MULTI_BUDDY], m_fYaw[CM_MULTI_BUDDY], m_akCameraRotation[CM_MULTI_BUDDY]);

	m_fFov[CM_ROYAL_CITY]				    = g_settings.CAMERA_UI_ARENA_BATTLE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_ROYAL_CITY]                  = g_settings.CAMERA_UI_ARENA_BATTLE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_ROYAL_CITY]			        = g_settings.CAMERA_UI_ARENA_BATTLE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_ROYAL_CITY]		        = g_settings.CAMERA_UI_ARENA_BATTLE_DISTANCE2;
	m_fNearCameraHeightBias[CM_ROYAL_CITY]   = g_settings.CAMERA_UI_ARENA_BATTLE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_ROYAL_CITY], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_ROYAL_CITY]);
	_calcCameraRotationMatrix(m_fPitch[CM_ROYAL_CITY], m_fYaw[CM_ROYAL_CITY], m_akCameraRotation[CM_ROYAL_CITY]);

	m_fFov[CM_NONPAREIL_ITEM]				    = g_settings.CAMERA_UI_NONPAREIL_ITEM_FOV * NI_PI / 180.0f;
	m_fPitch[CM_NONPAREIL_ITEM]                  = g_settings.CAMERA_UI_NONPAREIL_ITEM_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_NONPAREIL_ITEM]			        = g_settings.CAMERA_UI_NONPAREIL_ITEM_YAW * NI_PI / 180.0f;
	m_fDistance[CM_NONPAREIL_ITEM]		        = g_settings.CAMERA_UI_NONPAREIL_ITEM_DISTANCE;
	m_fNearCameraHeightBias[CM_NONPAREIL_ITEM]   = g_settings.CAMERA_UI_NONPAREIL_ITEM_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_NONPAREIL_ITEM], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_NONPAREIL_ITEM]);
	_calcCameraRotationMatrix(m_fPitch[CM_NONPAREIL_ITEM], m_fYaw[CM_NONPAREIL_ITEM], m_akCameraRotation[CM_NONPAREIL_ITEM]);

	m_fFov[CM_DEVAEYE]				    = g_settings.CAMERA_UI_DEVAEYE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_DEVAEYE]                  = g_settings.CAMERA_UI_DEVAEYE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_DEVAEYE]			        = g_settings.CAMERA_UI_DEVAEYE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_DEVAEYE]		        = g_settings.CAMERA_UI_DEVAEYE_DISTANCE;
	m_fNearCameraHeightBias[CM_DEVAEYE]   = g_settings.CAMERA_UI_DEVAEYE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_DEVAEYE], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_DEVAEYE]);
	_calcCameraRotationMatrix(m_fPitch[CM_DEVAEYE], m_fYaw[CM_DEVAEYE], m_akCameraRotation[CM_DEVAEYE]);

	m_fFov[CM_VEHICLEFIGHTING]				    = g_settings.CAMERA_UI_VEHICLEFIGHTING_FOV * NI_PI / 180.0f;
	m_fPitch[CM_VEHICLEFIGHTING]                  = g_settings.CAMERA_UI_VEHICLEFIGHTING_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_VEHICLEFIGHTING]			        = g_settings.CAMERA_UI_VEHICLEFIGHTING_YAW * NI_PI / 180.0f;
	m_fDistance[CM_VEHICLEFIGHTING]		        = g_settings.CAMERA_UI_VEHICLEFIGHTING_DISTANCE;
	m_fNearCameraHeightBias[CM_VEHICLEFIGHTING]   = g_settings.CAMERA_UI_VEHICLEFIGHTING_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_VEHICLEFIGHTING], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_VEHICLEFIGHTING]);
	_calcCameraRotationMatrix(m_fPitch[CM_VEHICLEFIGHTING], m_fYaw[CM_VEHICLEFIGHTING], m_akCameraRotation[CM_VEHICLEFIGHTING]);

	m_fFov[CM_FABAO]				    = g_settings.CAMERA_UI_DIVINITIES_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FABAO]                  = g_settings.CAMERA_UI_DIVINITIES_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FABAO]			        = g_settings.CAMERA_UI_DIVINITIES_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FABAO]		        = g_settings.CAMERA_UI_DIVINITIES_DISTANCE;
	m_fNearCameraHeightBias[CM_FABAO]   = g_settings.CAMERA_UI_DIVINITIES_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FABAO], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_FABAO]);
	_calcCameraRotationMatrix(m_fPitch[CM_FABAO], m_fYaw[CM_FABAO], m_akCameraRotation[CM_FABAO]);

	m_fFov[CM_WILD_PRINCES_BOSS]				    = g_settings.CAMERA_UI_WORLDMAP_BOSS_FOV * NI_PI / 180.0f;
	m_fPitch[CM_WILD_PRINCES_BOSS]                  = g_settings.CAMERA_UI_WORLDMAP_BOSS_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_WILD_PRINCES_BOSS]			        = g_settings.CAMERA_UI_WORLDMAP_BOSS_YAW * NI_PI / 180.0f;
	m_fDistance[CM_WILD_PRINCES_BOSS]		        = g_settings.CAMERA_UI_WORLDMAP_BOSS_DISTANCE;
	m_fNearCameraHeightBias[CM_WILD_PRINCES_BOSS]   = g_settings.CAMERA_UI_WORLDMAP_BOSS_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_WILD_PRINCES_BOSS], (float)GAMESTORY_WILDBOSS_WIDTH / (float)GAMESTORY_WILDBOSS_HEIGHT, m_akFrustums[CM_WILD_PRINCES_BOSS]);
	_calcCameraRotationMatrix(m_fPitch[CM_WILD_PRINCES_BOSS], m_fYaw[CM_WILD_PRINCES_BOSS], m_akCameraRotation[CM_WILD_PRINCES_BOSS]);

	m_fFov[CM_WILD_PRINCES_BOSS_FAMILY]				    = g_settings.CAMERA_UI_WILD_PRINCES_FOV * NI_PI / 180.0f;
	m_fPitch[CM_WILD_PRINCES_BOSS_FAMILY]                  = g_settings.CAMERA_UI_WILD_PRINCES_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_WILD_PRINCES_BOSS_FAMILY]			        = g_settings.CAMERA_UI_WILD_PRINCES_YAW * NI_PI / 180.0f;
	m_fDistance[CM_WILD_PRINCES_BOSS_FAMILY]		        = g_settings.CAMERA_UI_WILD_PRINCES_DISTANCE;
	m_fNearCameraHeightBias[CM_WILD_PRINCES_BOSS_FAMILY]   = g_settings.CAMERA_UI_WILD_PRINCES_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_WILD_PRINCES_BOSS_FAMILY], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_WILD_PRINCES_BOSS_FAMILY]);
	_calcCameraRotationMatrix(m_fPitch[CM_WILD_PRINCES_BOSS_FAMILY], m_fYaw[CM_WILD_PRINCES_BOSS_FAMILY], m_akCameraRotation[CM_WILD_PRINCES_BOSS_FAMILY]);

	m_fFov[CM_WILD_BOSS]				    = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_FOV * NI_PI / 180.0f;
	m_fPitch[CM_WILD_BOSS]                  = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_WILD_BOSS]			        = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_YAW * NI_PI / 180.0f;
	m_fDistance[CM_WILD_BOSS]		        = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_DISTANCE;
	m_fNearCameraHeightBias[CM_WILD_BOSS]   = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_WILD_BOSS], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_WILD_BOSS]);
	_calcCameraRotationMatrix(m_fPitch[CM_WILD_BOSS], m_fYaw[CM_WILD_BOSS], m_akCameraRotation[CM_WILD_BOSS]);

	m_fFov[CM_CORE_BOSS]				    = g_settings.CAMERA_UI_COREBOSS_FOV * NI_PI / 180.0f;
	m_fPitch[CM_CORE_BOSS]                  = g_settings.CAMERA_UI_COREBOSS_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_CORE_BOSS]			        = g_settings.CAMERA_UI_COREBOSS_YAW * NI_PI / 180.0f;
	m_fDistance[CM_CORE_BOSS]		        = g_settings.CAMERA_UI_COREBOSS_DISTANCE;
	m_fNearCameraHeightBias[CM_CORE_BOSS]   = g_settings.CAMERA_UI_COREBOSS_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_CORE_BOSS], (float)COREBOSS_WIDTH / (float)COREBOSS_HEIGHT, m_akFrustums[CM_CORE_BOSS]);
	_calcCameraRotationMatrix(m_fPitch[CM_CORE_BOSS], m_fYaw[CM_CORE_BOSS], m_akCameraRotation[CM_CORE_BOSS]);

	m_fFov[CM_GREAT_KUNGFU]						= g_settings.CAMERA_UI_GREATKUNGFU_FOV * NI_PI / 180.0f;
	m_fPitch[CM_GREAT_KUNGFU]					= g_settings.CAMERA_UI_GREATKUNGFU_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_GREAT_KUNGFU]						= g_settings.CAMERA_UI_GREATKUNGFU_YAW * NI_PI / 180.0f;
	m_fDistance[CM_GREAT_KUNGFU]		        = g_settings.CAMERA_UI_GREATKUNGFU_DISTANCE;
	m_fNearCameraHeightBias[CM_GREAT_KUNGFU]	= g_settings.CAMERA_UI_GREATKUNGFU_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_GREAT_KUNGFU], (float)GREATKUNGFU_WIDTH / (float)GREATKUNGFU_HEIGHT, m_akFrustums[CM_GREAT_KUNGFU]);
	_calcCameraRotationMatrix(m_fPitch[CM_GREAT_KUNGFU], m_fYaw[CM_GREAT_KUNGFU], m_akCameraRotation[CM_GREAT_KUNGFU]);

	m_fFov[CM_ESC_UI_CHAR]				    = g_settings.CAMERA_UI_ESC_FOV * NI_PI / 180.0f;
	m_fPitch[CM_ESC_UI_CHAR]                  = g_settings.CAMERA_UI_ESC_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_ESC_UI_CHAR]			        = g_settings.CAMERA_UI_ESC_YAW * NI_PI / 180.0f;
	m_fDistance[CM_ESC_UI_CHAR]		        = g_settings.CAMERA_UI_ESC_DISTANCE;
	m_fNearCameraHeightBias[CM_ESC_UI_CHAR]   = g_settings.CAMERA_UI_ESC_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_ESC_UI_CHAR], (float)WORLDMAP_WIDTH / (float)WORLDMAP_HEIGHT, m_akFrustums[CM_ESC_UI_CHAR]);
	_calcCameraRotationMatrix(m_fPitch[CM_ESC_UI_CHAR], m_fYaw[CM_ESC_UI_CHAR], m_akCameraRotation[CM_ESC_UI_CHAR]);

	m_fFov[CM_CHARACTER]				    = g_settings.CAMERA_UI_AVATAR_FULL_FOV * NI_PI / 180.0f;
	m_fPitch[CM_CHARACTER]                  = g_settings.CAMERA_UI_AVATAR_FULL_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_CHARACTER]			        = g_settings.CAMERA_UI_AVATAR_FULL_YAW * NI_PI / 180.0f;
	m_fDistance[CM_CHARACTER]		        = g_settings.CAMERA_UI_AVATAR_FULL_DISTANCE;
	m_fNearCameraHeightBias[CM_CHARACTER]   = g_settings.CAMERA_UI_AVATAR_FULL_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_CHARACTER], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_CHARACTER]);
	_calcCameraRotationMatrix(m_fPitch[CM_CHARACTER], m_fYaw[CM_CHARACTER], m_akCameraRotation[CM_CHARACTER]);

	m_fFov[CM_STIGMAS]				    = g_settings.CAMERA_UI_AVATAR_FULL_FOV * NI_PI / 180.0f;
	m_fPitch[CM_STIGMAS]                  = g_settings.CAMERA_UI_AVATAR_FULL_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_STIGMAS]			        = g_settings.CAMERA_UI_AVATAR_FULL_YAW * NI_PI / 180.0f;
	m_fDistance[CM_STIGMAS]		        = g_settings.CAMERA_UI_AVATAR_FULL_DISTANCE;
	m_fNearCameraHeightBias[CM_STIGMAS]   = g_settings.CAMERA_UI_AVATAR_FULL_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_STIGMAS], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_STIGMAS]);
	_calcCameraRotationMatrix(m_fPitch[CM_STIGMAS], m_fYaw[CM_STIGMAS], m_akCameraRotation[CM_STIGMAS]);

	m_fFov[CM_TOYPET]				    = g_settings.CAMERA_UI_AVATAR_FULL_FOV * NI_PI / 180.0f;
	m_fPitch[CM_TOYPET]                  = g_settings.CAMERA_UI_AVATAR_FULL_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_TOYPET]			        = g_settings.CAMERA_UI_AVATAR_FULL_YAW * NI_PI / 180.0f;
	m_fDistance[CM_TOYPET]		        = g_settings.CAMERA_UI_AVATAR_FULL_DISTANCE;
	m_fNearCameraHeightBias[CM_TOYPET]   = g_settings.CAMERA_UI_AVATAR_FULL_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_TOYPET], (float)TOYPET_WIDTH / (float)TOYPET_HEIGHT, m_akFrustums[CM_TOYPET]);
	_calcCameraRotationMatrix(m_fPitch[CM_TOYPET], m_fYaw[CM_TOYPET], m_akCameraRotation[CM_TOYPET]);

	m_fFov[CM_TOYPET_CHOOSE]				    = g_settings.CAMERA_UI_TOYPET_CHOOSE_FOV * NI_PI / 180.0f;
	m_fPitch[CM_TOYPET_CHOOSE]                  = g_settings.CAMERA_UI_TOYPET_CHOOSE_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_TOYPET_CHOOSE]			        = g_settings.CAMERA_UI_TOYPET_CHOOSE_YAW * NI_PI / 180.0f;
	m_fDistance[CM_TOYPET_CHOOSE]		        = g_settings.CAMERA_UI_TOYPET_CHOOSE_DISTANCE;
	m_fNearCameraHeightBias[CM_TOYPET_CHOOSE]   = g_settings.CAMERA_UI_TOYPET_CHOOSE_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_TOYPET_CHOOSE], (float)TIMELIMIT_WIDTH / (float)TIMELIMIT_HEIGHT, m_akFrustums[CM_TOYPET_CHOOSE]);
	_calcCameraRotationMatrix(m_fPitch[CM_TOYPET_CHOOSE], m_fYaw[CM_TOYPET_CHOOSE], m_akCameraRotation[CM_TOYPET_CHOOSE]);

	m_fFov[CM_TOYPET_GAIN]				    = g_settings.CAMERA_UI_AVATAR_FULL_FOV * NI_PI / 180.0f;
	m_fPitch[CM_TOYPET_GAIN]                  = g_settings.CAMERA_UI_AVATAR_FULL_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_TOYPET_GAIN]			        = g_settings.CAMERA_UI_AVATAR_FULL_YAW * NI_PI / 180.0f;
	m_fDistance[CM_TOYPET_GAIN]		        = g_settings.CAMERA_UI_AVATAR_FULL_DISTANCE;
	m_fNearCameraHeightBias[CM_TOYPET_GAIN]   = g_settings.CAMERA_UI_AVATAR_FULL_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_TOYPET_GAIN], (float)TOYPET_WIDTH / (float)TOYPET_HEIGHT, m_akFrustums[CM_TOYPET_GAIN]);
	_calcCameraRotationMatrix(m_fPitch[CM_TOYPET_GAIN], m_fYaw[CM_TOYPET_GAIN], m_akCameraRotation[CM_TOYPET_GAIN]);

	m_fFov[CM_GEMBUFF]				    = g_settings.CAMERA_UI_GEMBUFF_FOV * NI_PI / 180.0f;
	m_fPitch[CM_GEMBUFF]                  = g_settings.CAMERA_UI_GEMBUFF_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_GEMBUFF]			        = g_settings.CAMERA_UI_GEMBUFF_YAW * NI_PI / 180.0f;
	m_fDistance[CM_GEMBUFF]		        = g_settings.CAMERA_UI_GEMBUFF_DISTANCE;
	m_fNearCameraHeightBias[CM_GEMBUFF]   = g_settings.CAMERA_UI_GEMBUFF_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_GEMBUFF], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_GEMBUFF]);
	_calcCameraRotationMatrix(m_fPitch[CM_GEMBUFF], m_fYaw[CM_GEMBUFF], m_akCameraRotation[CM_GEMBUFF]);

	m_fFov[CM_SKY_ARENA]				    = g_settings.CAMERA_UI_AVATAR_FULL_FOV * NI_PI / 180.0f;
	m_fPitch[CM_SKY_ARENA]                  = g_settings.CAMERA_UI_AVATAR_FULL_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_SKY_ARENA]			        = g_settings.CAMERA_UI_AVATAR_FULL_YAW * NI_PI / 180.0f;
	m_fDistance[CM_SKY_ARENA]		        = g_settings.CAMERA_UI_AVATAR_FULL_DISTANCE;
	m_fNearCameraHeightBias[CM_SKY_ARENA]   = g_settings.CAMERA_UI_AVATAR_FULL_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_SKY_ARENA], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_SKY_ARENA]);
	_calcCameraRotationMatrix(m_fPitch[CM_SKY_ARENA], m_fYaw[CM_SKY_ARENA], m_akCameraRotation[CM_SKY_ARENA]);

	m_fFov[CM_FASHION]				    = g_settings.CAMERA_UI_AVATAR_FULL_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FASHION]                  = g_settings.CAMERA_UI_AVATAR_FULL_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FASHION]			        = g_settings.CAMERA_UI_AVATAR_FULL_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FASHION]		        = g_settings.CAMERA_UI_AVATAR_FULL_DISTANCE;
	m_fNearCameraHeightBias[CM_FASHION]   = g_settings.CAMERA_UI_AVATAR_FULL_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FASHION], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_FASHION]);
	_calcCameraRotationMatrix(m_fPitch[CM_FASHION], m_fYaw[CM_FASHION], m_akCameraRotation[CM_FASHION]);

	m_fFov[CM_VIEWINFO]				    = g_settings.CAMERA_UI_VIEWINFO_FOV * NI_PI / 180.0f;
	m_fPitch[CM_VIEWINFO]                  = g_settings.CAMERA_UI_VIEWINFO_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_VIEWINFO]			        = g_settings.CAMERA_UI_VIEWINFO_YAW * NI_PI / 180.0f;
	m_fDistance[CM_VIEWINFO]		        = g_settings.CAMERA_UI_VIEWINFO_DISTANCE;
	m_fNearCameraHeightBias[CM_VIEWINFO]   = g_settings.CAMERA_UI_VIEWINFO_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_VIEWINFO], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_VIEWINFO]);
	_calcCameraRotationMatrix(m_fPitch[CM_VIEWINFO], m_fYaw[CM_VIEWINFO], m_akCameraRotation[CM_VIEWINFO]);

	m_fFov[CM_RANKINGLIST]				    = g_settings.CAMERA_UI_RANKINGLIST_FOV * NI_PI / 180.0f;
	m_fPitch[CM_RANKINGLIST]				= g_settings.CAMERA_UI_RANKINGLIST_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_RANKINGLIST]					= g_settings.CAMERA_UI_RANKINGLIST_YAW * NI_PI / 180.0f;
	m_fDistance[CM_RANKINGLIST]		        = g_settings.CAMERA_UI_RANKINGLIST_DISTANCE;
	m_fNearCameraHeightBias[CM_RANKINGLIST]	= g_settings.CAMERA_UI_RANKINGLIST_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_RANKINGLIST], (float)FULL_CHAR_WIDTH / (float)FULL_CHAR_HEIGHT, m_akFrustums[CM_RANKINGLIST]);
	_calcCameraRotationMatrix(m_fPitch[CM_RANKINGLIST], m_fYaw[CM_RANKINGLIST], m_akCameraRotation[CM_RANKINGLIST]);

	m_fFov[CM_FRIEND_TIPS]				    = g_settings.CAMERA_UI_FRIEND_TIPS_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FRIEND_TIPS]                  = g_settings.CAMERA_UI_FRIEND_TIPS_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FRIEND_TIPS]			        = g_settings.CAMERA_UI_FRIEND_TIPS_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FRIEND_TIPS]		        = g_settings.CAMERA_UI_FRIEND_TIPS_DISTANCE;
	m_fNearCameraHeightBias[CM_FRIEND_TIPS]   = g_settings.CAMERA_UI_FRIEND_TIPS_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FRIEND_TIPS], (float)FRIEND_TIP_WIDTH / (float)FRIEND_TIP_HEIGHT, m_akFrustums[CM_FRIEND_TIPS]);
	_calcCameraRotationMatrix(m_fPitch[CM_FRIEND_TIPS], m_fYaw[CM_FRIEND_TIPS], m_akCameraRotation[CM_FRIEND_TIPS]);

	m_fFov[CM_VIP_RAID]				    = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_FOV * NI_PI / 180.0f;
	m_fPitch[CM_VIP_RAID]                  = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_VIP_RAID]			        = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_YAW * NI_PI / 180.0f;
	m_fDistance[CM_VIP_RAID]		        = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_DISTANCE;
	m_fNearCameraHeightBias[CM_VIP_RAID]   = g_settings.CAMERA_UI_GAMESTORY_WILDBOSS_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_VIP_RAID], (float)ARENA_WIDTH / (float)ARENA_HEIGHT, m_akFrustums[CM_VIP_RAID]);
	_calcCameraRotationMatrix(m_fPitch[CM_VIP_RAID], m_fYaw[CM_VIP_RAID], m_akCameraRotation[CM_VIP_RAID]);

	m_fFov[CM_GREAT_KUNGFU_REMAIN2]				    = g_settings.CAMERA_UI_GREATKUNGFU_REMAIN_FOV * NI_PI / 180.0f;
	m_fPitch[CM_GREAT_KUNGFU_REMAIN2]                  = g_settings.CAMERA_UI_GREATKUNGFU_REMAIN_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_GREAT_KUNGFU_REMAIN2]			        = g_settings.CAMERA_UI_GREATKUNGFU_REMAIN_YAW * NI_PI / 180.0f;
	m_fDistance[CM_GREAT_KUNGFU_REMAIN2]		        = g_settings.CAMERA_UI_GREATKUNGFU_REMAIN_DISTANCE;
	m_fNearCameraHeightBias[CM_GREAT_KUNGFU_REMAIN2]   = g_settings.CAMERA_UI_GREATKUNGFU_REMAIN_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_GREAT_KUNGFU_REMAIN2], (float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT, m_akFrustums[CM_GREAT_KUNGFU_REMAIN2]);
	_calcCameraRotationMatrix(m_fPitch[CM_GREAT_KUNGFU_REMAIN2], m_fYaw[CM_GREAT_KUNGFU_REMAIN2], m_akCameraRotation[CM_GREAT_KUNGFU_REMAIN2]);

	m_fFov[CM_TREASURE_CHESTS]				    = g_settings.CAMERA_UI_TREASURE_CHESTS_FOV * NI_PI / 180.0f;
	m_fPitch[CM_TREASURE_CHESTS]                  = g_settings.CAMERA_UI_TREASURE_CHESTS_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_TREASURE_CHESTS]			        = g_settings.CAMERA_UI_TREASURE_CHESTS_YAW * NI_PI / 180.0f;
	m_fDistance[CM_TREASURE_CHESTS]		        = g_settings.CAMERA_UI_TREASURE_CHESTS_DISTANCE;
	m_fNearCameraHeightBias[CM_TREASURE_CHESTS]   = g_settings.CAMERA_UI_TREASURE_CHESTS_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_TREASURE_CHESTS], (float)TREASURE_WIDTH / (float)TREASURE_HEIGHT, m_akFrustums[CM_TREASURE_CHESTS]);
	_calcCameraRotationMatrix(m_fPitch[CM_TREASURE_CHESTS], m_fYaw[CM_TREASURE_CHESTS], m_akCameraRotation[CM_TREASURE_CHESTS]);

	m_fFov[CM_HOOK_BOT]				    = g_settings.CAMERA_UI_HOOKBOT_FOV * NI_PI / 180.0f;
	m_fPitch[CM_HOOK_BOT]                  = g_settings.CAMERA_UI_HOOKBOT_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_HOOK_BOT]			        = g_settings.CAMERA_UI_HOOKBOT_YAW * NI_PI / 180.0f;
	m_fDistance[CM_HOOK_BOT]		        = g_settings.CAMERA_UI_HOOKBOT_DISTANCE;
	m_fNearCameraHeightBias[CM_HOOK_BOT]   = g_settings.CAMERA_UI_HOOKBOT_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_HOOK_BOT], (float)HOOKBOT_WIDTH / (float)HOOKBOT_HEIGHT, m_akFrustums[CM_HOOK_BOT]);
	_calcCameraRotationMatrix(m_fPitch[CM_HOOK_BOT], m_fYaw[CM_HOOK_BOT], m_akCameraRotation[CM_HOOK_BOT]);

	m_fFov[CM_HOOK_BOT_MONSTER]				    = g_settings.CAMERA_UI_HOOKBOT_FOV * NI_PI / 180.0f;
	m_fPitch[CM_HOOK_BOT_MONSTER]                  = g_settings.CAMERA_UI_HOOKBOT_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_HOOK_BOT_MONSTER]			        = g_settings.CAMERA_UI_HOOKBOT_YAW * NI_PI / 180.0f;
	m_fDistance[CM_HOOK_BOT_MONSTER]		        = g_settings.CAMERA_UI_HOOKBOT_DISTANCE;
	m_fNearCameraHeightBias[CM_HOOK_BOT_MONSTER]   = g_settings.CAMERA_UI_HOOKBOT_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_HOOK_BOT_MONSTER], (float)HOOKBOT_WIDTH / (float)HOOKBOT_HEIGHT, m_akFrustums[CM_HOOK_BOT_MONSTER]);
	_calcCameraRotationMatrix(m_fPitch[CM_HOOK_BOT_MONSTER], m_fYaw[CM_HOOK_BOT_MONSTER], m_akCameraRotation[CM_HOOK_BOT_MONSTER]);

	m_fFov[CM_REWARD_RANK]					= m_fFov[CM_VIEWINFO];
	m_fPitch[CM_REWARD_RANK]				= m_fPitch[CM_VIEWINFO];
	m_fYaw[CM_REWARD_RANK]					= m_fYaw[CM_VIEWINFO];
	m_fDistance[CM_REWARD_RANK]		        = m_fDistance[CM_VIEWINFO];
	m_fNearCameraHeightBias[CM_REWARD_RANK]	= m_fNearCameraHeightBias[CM_VIEWINFO];
	_calcCameraFrustum(m_fFov[CM_REWARD_RANK], (float)REWARD_RANK_WIDTH / (float)REWARD_RANK_HEIGHT, m_akFrustums[CM_REWARD_RANK]);
	_calcCameraRotationMatrix(m_fPitch[CM_REWARD_RANK], m_fYaw[CM_REWARD_RANK], m_akCameraRotation[CM_REWARD_RANK]);

	m_fFov[CM_FUNCTION_PREVIEW_BUTTON]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FUNCTION_PREVIEW_BUTTON]				= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FUNCTION_PREVIEW_BUTTON]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FUNCTION_PREVIEW_BUTTON]		        = g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_DISTANCE;
	m_fNearCameraHeightBias[CM_FUNCTION_PREVIEW_BUTTON]	= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FUNCTION_PREVIEW_BUTTON], (float)FUNCTION_PRIVIEW_WIDTH / (float)FUNCTION_PRIVIEW_HEIGHT, m_akFrustums[CM_FUNCTION_PREVIEW_BUTTON]);
	_calcCameraRotationMatrix(m_fPitch[CM_FUNCTION_PREVIEW_BUTTON], m_fYaw[CM_FUNCTION_PREVIEW_BUTTON], m_akCameraRotation[CM_FUNCTION_PREVIEW_BUTTON]);

	m_fFov[CM_FUNCTION_PREVIEW_BUTTON2]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_FUNCTION_PREVIEW_BUTTON2]				= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_FUNCTION_PREVIEW_BUTTON2]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_FUNCTION_PREVIEW_BUTTON2]		        = g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_DISTANCE;
	m_fNearCameraHeightBias[CM_FUNCTION_PREVIEW_BUTTON2]	= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_FUNCTION_PREVIEW_BUTTON2], (float)FUNCTION_PRIVIEW_WIDTH / (float)FUNCTION_PRIVIEW_HEIGHT, m_akFrustums[CM_FUNCTION_PREVIEW_BUTTON2]);
	_calcCameraRotationMatrix(m_fPitch[CM_FUNCTION_PREVIEW_BUTTON2], m_fYaw[CM_FUNCTION_PREVIEW_BUTTON2], m_akCameraRotation[CM_FUNCTION_PREVIEW_BUTTON2]);

	m_fFov[CM_JIANGJUNFU]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_FOV * NI_PI / 180.0f;
	m_fPitch[CM_JIANGJUNFU]				= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_PITCH * NI_PI / 180.0f;
	m_fYaw[CM_JIANGJUNFU]					= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_YAW * NI_PI / 180.0f;
	m_fDistance[CM_JIANGJUNFU]		        = g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_DISTANCE;
	m_fNearCameraHeightBias[CM_JIANGJUNFU]	= g_settings.CAMERA_UI_FUNCTION_PREVIEW_BUTTON_NEAR_HEIGHT_BIAS;
	_calcCameraFrustum(m_fFov[CM_JIANGJUNFU], (float)VEHICLE_WIDTH / (float)VEHICLE_HEIGHT, m_akFrustums[CM_JIANGJUNFU]);
	_calcCameraRotationMatrix(m_fPitch[CM_JIANGJUNFU], m_fYaw[CM_JIANGJUNFU], m_akCameraRotation[CM_JIANGJUNFU]);
}

void UI_AvatarManager::_calcCameraFrustum(float fFov, float fAspectRatio, NiFrustum& rkFrustum)
{
    float fViewPlaneHalfHeight = tanf(fFov * 0.5f);
    float fViewPlaneHalfWidth = fViewPlaneHalfHeight * fAspectRatio;

    rkFrustum.m_fLeft   = -fViewPlaneHalfWidth;
    rkFrustum.m_fRight  = fViewPlaneHalfWidth;
    rkFrustum.m_fTop    = fViewPlaneHalfHeight;
    rkFrustum.m_fBottom = -fViewPlaneHalfHeight;
    rkFrustum.m_fNear   = 0.0f;
    rkFrustum.m_fFar    = 500000.0f;
}

void UI_AvatarManager::_calcCameraRotationMatrix(float fPitch, float fYaw, NiMatrix3& rkRotation)
{
    NiMatrix3 kRot(
        NiPoint3(0.0f, 1.0f, 0.0f),
        NiPoint3(0.0f, 0.0f, 1.0f),
        NiPoint3(1.0f, 0.0f, 0.0f));

    NiMatrix3 kZRot;
    kZRot.MakeZRotation(fYaw);
    NiMatrix3 kXRot;
    kXRot.MakeXRotation(fPitch);

    NiMatrix3 kCamRot;
    kCamRot = kZRot * kXRot * kRot;

    NiPoint3 kDir;
    NiPoint3 kRight;
    NiPoint3 kUp;
    NiPoint3 kNeg = NiPoint3(-1 , 1 , -1);
    NiPoint3 kNegY = NiPoint3(1 , -1 , 1);

    kCamRot.GetCol(0 , kDir);
    kCamRot.GetCol(1 , kUp);
    kCamRot.GetCol(2 , kRight);

    kDir    = NiPoint3::ComponentProduct(kDir,      kNeg);
    kUp     = NiPoint3::ComponentProduct(kUp,       kNeg);
    kRight  = NiPoint3::ComponentProduct(kRight,    kNegY);

    rkRotation.SetCol(0, kUp);
    rkRotation.SetCol(1, kDir);
    rkRotation.SetCol(2, kRight);
}


void UI_AvatarManager::_applyCameraSetting(NiCameraPtr& spCamera, CameraMode eCameraMode, const NiRect<float>& kViewPort)
{
	spCamera->SetViewPort(kViewPort);
	spCamera->SetViewFrustum(m_akFrustums[eCameraMode]);
	
	// 设置的是world坐标系！
	spCamera->SetWorldRotate(m_akCameraRotation[eCameraMode]);
    NiPoint3 kCamPos = -spCamera->GetWorldDirection() * m_fDistance[eCameraMode];
	
	// 设置的是world坐标系！
	kCamPos.z += m_fNearCameraHeightBias[eCameraMode];
    spCamera->SetWorldTranslate(kCamPos);
}

void UI_AvatarManager::_loadViewPorts(UI_AvatarRender* pkRender)
{
    /*
        +----+----+-----------+
        | 0  | N  |     V     |
        |    |    |-----------+
        +---++--+-+-+---+---+
        | 1 | 2 | 3 | 4 | 5 |
        +-+-+-+-+-+-+---+---+
        +-+-+-+-+-+-+
    */

    // 缓存各avatar的viewport信息
    if (pkRender != NULL)
    {
        float fWidth = (float)pkRender->GetRenderTargetWidth();
        float fHeight = (float)pkRender->GetRenderTargetHeight();

		Utility::UT_ConfigFile kConfig, kConfig2;
		if (!kConfig.Load(StringType(CLIENT_CONFIG_DIR) + "Avatar.ini" ) || !kConfig2.Load(StringType(CLIENT_CONFIG_DIR) + "Avatar2.ini" ))
		{
			return;
		}

		char szName[20];
		float fLeft, fRight, fTop, fBottom;

		fTop = 1.0f;

		//Avatar0~4
		//组队全身 (1~4)
		for(int i=0; i<5; ++i)
		{
			sprintf(szName, "Avatar%d", i);
			fLeft = kConfig.GetIntSetting("x", szName, 0) * SCALE / fWidth;
			fTop = 1.0f - kConfig.GetIntSetting("y", szName, 0) * SCALE / fHeight;
			m_akFullBodyViewPorts[i].m_left     = fLeft;
			m_akFullBodyViewPorts[i].m_right    = fLeft + kConfig.GetIntSetting("w", szName, 0) * SCALE / fWidth;
			m_akFullBodyViewPorts[i].m_top      = fTop;
			m_akFullBodyViewPorts[i].m_bottom   = fTop - kConfig.GetIntSetting("h", szName, 0) * SCALE / fHeight;
		}

		//Avatar5
		//机器人
		fLeft = kConfig.GetIntSetting("x", "Avatar5", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar5", 0) * SCALE / fHeight;
		m_kHookBot[0].m_left = fLeft;
		m_kHookBot[0].m_right = fLeft + kConfig.GetIntSetting("w", "Avatar5", 0) * SCALE / fWidth;
		m_kHookBot[0].m_top = fTop;
		m_kHookBot[0].m_bottom = fTop - kConfig.GetIntSetting("h", "Avatar5", 0) * SCALE / fHeight;

		//Avatar6
		//排行榜
		fLeft = kConfig.GetIntSetting("x", "Avatar6", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar6", 0) * SCALE / fHeight;
		m_kRankingListViewPort[0].m_left = fLeft;
		m_kRankingListViewPort[0].m_right = fLeft + kConfig.GetIntSetting("w", "Avatar6", 0) * SCALE / fWidth;
		m_kRankingListViewPort[0].m_top = fTop;
		m_kRankingListViewPort[0].m_bottom = fTop - kConfig.GetIntSetting("h", "Avatar6", 0) * SCALE / fHeight;

		//Avatar7
		//查看资料
		fLeft = kConfig.GetIntSetting("x", "Avatar7", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar7", 0) * SCALE / fHeight;
		fRight = fLeft + kConfig.GetIntSetting("w", "Avatar7", 0) * SCALE / fWidth;
		fBottom = fTop - kConfig.GetIntSetting("h", "Avatar7", 0) * SCALE / fHeight;
		m_kViewInfoViewPort[0].m_left = fLeft;
		m_kViewInfoViewPort[0].m_right = fRight;
		m_kViewInfoViewPort[0].m_top = fTop;
		m_kViewInfoViewPort[0].m_bottom = fBottom;
		

		//Avatar8
		//鲜花
		fLeft = kConfig.GetIntSetting("x", "Avatar8", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar8", 0) * SCALE / fHeight;
		fRight = fLeft + kConfig.GetIntSetting("w", "Avatar8", 0) * SCALE / fWidth;
		fBottom = fTop - kConfig.GetIntSetting("h", "Avatar8", 0) * SCALE / fHeight;
		m_kFlowerViewPort[0].m_left = fLeft;
		m_kFlowerViewPort[0].m_right = fRight;
		m_kFlowerViewPort[0].m_top = fTop;
		m_kFlowerViewPort[0].m_bottom = fBottom;

		//Avatar9
		//功能预览按钮
		fLeft = kConfig.GetIntSetting("x", "Avatar9", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar9", 0) * SCALE / fHeight;
		fRight = fLeft + kConfig.GetIntSetting("w", "Avatar9", 0) * SCALE / fWidth;
		fBottom = fTop - kConfig.GetIntSetting("h", "Avatar9", 0) * SCALE / fHeight;
		m_kFunctionPreviewBtnViewPort[0].m_left = fLeft;
		m_kFunctionPreviewBtnViewPort[0].m_right = fRight;
		m_kFunctionPreviewBtnViewPort[0].m_top = fTop;
		m_kFunctionPreviewBtnViewPort[0].m_bottom = fBottom;

		//Avatar10
		//明教禁地BOSS & 野外首领
		fLeft = kConfig.GetIntSetting("x", "Avatar10", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar10", 0) * SCALE / fHeight;
		m_kGameStoryWildBossViewPort1[0].m_left = fLeft;
		m_kGameStoryWildBossViewPort1[0].m_right = fLeft + kConfig.GetIntSetting("w", "Avatar10", 0) * SCALE / fWidth;
		m_kGameStoryWildBossViewPort1[0].m_top = fTop;
		m_kGameStoryWildBossViewPort1[0].m_bottom = fTop - kConfig.GetIntSetting("h", "Avatar10", 0) * SCALE / fHeight;

		//Avatar11
		//核芯BOSS
		fLeft = kConfig.GetIntSetting("x", "Avatar11", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar11", 0) * SCALE / fHeight;
		m_kCoreBoss[0].m_left = fLeft;
		m_kCoreBoss[0].m_right = fLeft + kConfig.GetIntSetting("w", "Avatar11", 0) * SCALE / fWidth;
		m_kCoreBoss[0].m_top = fTop;
		m_kCoreBoss[0].m_bottom = fTop - kConfig.GetIntSetting("h", "Avatar11", 0) * SCALE / fHeight;

		//Avatar12
		//特训
		fLeft = kConfig.GetIntSetting("x", "Avatar12", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar12", 0) * SCALE / fHeight;
		m_kSpecialTrain[0].m_left = fLeft;
		m_kSpecialTrain[0].m_right = fLeft + kConfig.GetIntSetting("w", "Avatar12", 0) * SCALE / fWidth;
		m_kSpecialTrain[0].m_top = fTop;
		m_kSpecialTrain[0].m_bottom = fTop - kConfig.GetIntSetting("h", "Avatar12", 0) * SCALE / fHeight;

		//Avatar13
		//特训
		fLeft = kConfig.GetIntSetting("x", "Avatar13", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar13", 0) * SCALE / fHeight;
		m_kFirstRechargeAdv[0].m_left = fLeft;
		m_kFirstRechargeAdv[0].m_right = fLeft + kConfig.GetIntSetting("w", "Avatar13", 0) * SCALE / fWidth;
		m_kFirstRechargeAdv[0].m_top = fTop;
		m_kFirstRechargeAdv[0].m_bottom = fTop - kConfig.GetIntSetting("h", "Avatar13", 0) * SCALE / fHeight;

		//Avatar14
		//自动拾取广告
		fLeft = kConfig.GetIntSetting("x", "Avatar14", 0) * SCALE / fWidth;
		fTop = 1.0f - kConfig.GetIntSetting("y", "Avatar14", 0) * SCALE / fHeight;
		m_kAutoPickAdv[0].m_left = fLeft;
		m_kAutoPickAdv[0].m_right = fLeft + kConfig.GetIntSetting("w", "Avatar14", 0) * SCALE / fWidth;
		m_kAutoPickAdv[0].m_top = fTop;
		m_kAutoPickAdv[0].m_bottom = fTop - kConfig.GetIntSetting("h", "Avatar14", 0) * SCALE / fHeight;

		float fWidth2 = (float)pkRender->GetSmallRenderTargetWidth();
		float fHeight2 = (float)pkRender->GetSmallRenderTargetHeight();
		//AvatarSub
		//Avatar0
		//进阶线
		fLeft = kConfig2.GetIntSetting("x", "Avatar0", 0) * SCALE / fWidth2;
		fTop = 1.0f - kConfig2.GetIntSetting("y", "Avatar0", 0) * SCALE / fHeight2;
		m_kSubViewPorts[0].m_left = fLeft;
		m_kSubViewPorts[0].m_right = fLeft + kConfig2.GetIntSetting("w", "Avatar0", 0) * SCALE / fWidth2;
		m_kSubViewPorts[0].m_top = fTop;
		m_kSubViewPorts[0].m_bottom = fTop - kConfig2.GetIntSetting("h", "Avatar0", 0) * SCALE / fHeight2;


		//Avatar1
		//道具tips
		fLeft = kConfig2.GetIntSetting("x", "Avatar1", 0) * SCALE / fWidth2;
		fTop = 1.0f - kConfig2.GetIntSetting("y", "Avatar1", 0) * SCALE / fHeight2;
		fRight = fLeft + kConfig2.GetIntSetting("w", "Avatar1", 0) * SCALE / fWidth2;
		fBottom = fTop - kConfig2.GetIntSetting("h", "Avatar1", 0) * SCALE / fHeight2;
		m_kRunescapeTipViewPort[0].m_left = fLeft;
		m_kRunescapeTipViewPort[0].m_right = fRight;
		m_kRunescapeTipViewPort[0].m_top = fTop;
		m_kRunescapeTipViewPort[0].m_bottom = fBottom;

		//Avatar2
		//玩具
		fLeft = kConfig2.GetIntSetting("x", "Avatar2", 0) * SCALE / fWidth2;
		fTop = 1.0f - kConfig2.GetIntSetting("y", "Avatar2", 0) * SCALE / fHeight2;
		fRight = fLeft + kConfig2.GetIntSetting("w", "Avatar2", 0) * SCALE / fWidth2;
		fBottom = fTop - kConfig2.GetIntSetting("h", "Avatar2", 0) * SCALE / fHeight2;
		m_kToyPet[0].m_left = fLeft;
		m_kToyPet[0].m_right = fRight;
		m_kToyPet[0].m_top = fTop;
		m_kToyPet[0].m_bottom = fBottom;

	}
}

void UI_AvatarManager::_loadGUIImages(UI_AvatarRender* kRender)
{
    /*
        +----+----+-----------+
        | 0  | N  |     V     |
        |    |    |-----------+
        +---++--+-+-+---+---+
        | 1 | 2 | 3 | 4 | 5 |
        +-+-+-+-+-+-+---+---+
        +-+-+-+-+-+-+
    */

	if ( !kRender )
		return;

	Utility::UT_ConfigFile kConfig, kConfig2;
	if (!kConfig.Load(StringType(CLIENT_CONFIG_DIR) + "Avatar.ini" ) || !kConfig2.Load(StringType(CLIENT_CONFIG_DIR) + "Avatar2.ini" ))
		return;

	int iX = 0;
	int iY = 0;
	int iW = 0;
	int iH = 0;

	for(int i=0; i<15; ++i)
	{
		StringType szName;
		UT_BaseStringUtility::format(szName, "Avatar%d", i);
		iX = kConfig.GetIntSetting("x", szName, 0);
		iY = kConfig.GetIntSetting("y", szName, 0);
		iW = kConfig.GetIntSetting("w", szName, 0);
		iH = kConfig.GetIntSetting("h", szName, 0);
		kRender->CreateImage(StringType("Avatar/") + szName, iX, iY, iW, iH);
	}

	for(int i=0; i<3; ++i)
	{
		StringType szName;
		UT_BaseStringUtility::format(szName, "Avatar%d", i);
		iX = kConfig2.GetIntSetting("x", szName, 0);
		iY = kConfig2.GetIntSetting("y", szName, 0);
		iW = kConfig2.GetIntSetting("w", szName, 0);
		iH = kConfig2.GetIntSetting("h", szName, 0);
		kRender->CreateImage(StringType("AvatarSub/") + szName, iX, iY, iW, iH, true);
	}
}

void UI_AvatarManager::_getSubordinatePosition(const CSVFile::CF_CharType::DataEntry *pkEntry, CameraMode eCameraMode, NiPoint3 &kPosition)
{
	m_fDistance[eCameraMode] = pkEntry->_CameraDist;
	if(eCameraMode == CM_GODWEAPON_TIP)
		m_fDistance[eCameraMode] += 300;

	kPosition = NiPoint3::ZERO;
	const mem::vector<int16_t>& pos = pkEntry->_Position;
	if(pos.size() >= 3)
	{
		kPosition.x = Int2Float(pos[0]);
		kPosition.y = Int2Float(pos[1]);
		kPosition.z = Int2Float(pos[2]);
	}
}

//StringType UI_AvatarManager::_updateAvatarRoadPointInfo(const NiNodePtr& pkRootNode)
//{
//	//相对坐标系，会自动缩放,取最大分辨率
//	GUIRect kScreenBounds(0, 0, 1920, 1200);
//
//	StringStreamType sStream("");
//	//城镇
//	for(int i=1; i<3; ++i)
//	{
//		char szName[20];
//		sprintf(szName, "T_%02d", i);
//		
//		float fX, fY;
//		NiPoint3 kPosition = _getNodePosition(pkRootNode, szName);
//
//		if(m_spCamera->WorldPtToScreenPt(kPosition, fX, fY))
//		{
//			fX = fX * kScreenBounds.GetWidth();
//			fY = (1 - fY) * kScreenBounds.GetHeight();
//		}
//		else
//		{
//			fX = fY = 0;
//		}
//		sStream << szName << "|" << fX << "|" << fY << "|\n";
//	}
//
//	//BOSS
//	for(int i=1; i<14; ++i)
//	{
//		char szName[20];
//		sprintf(szName, "B_%02d", i);
//		
//		float fX, fY;
//		NiPoint3 kPosition = _getNodePosition(pkRootNode, szName);
//
//		if(m_spCamera->WorldPtToScreenPt(kPosition, fX, fY))
//		{
//			fX = fX * kScreenBounds.GetWidth();
//			fY = (1 - fY) * kScreenBounds.GetHeight();
//		}
//		else
//		{
//			fX = fY = 0;
//		}
//		sStream << szName << "|" << fX << "|" << fY << "|" << kPosition.x << "|" << kPosition.y << "|" << kPosition.z << "|\n";
//	}
//
//	//普通
//	for(int i=1; i<43; ++i)
//	{
//		char szName[20];
//		sprintf(szName, "M_%02d", i);
//
//		float fX, fY;
//		NiPoint3 kPosition = _getNodePosition(pkRootNode, szName);
//		
//		if(m_spCamera->WorldPtToScreenPt(kPosition, fX, fY))
//		{
//			fX = fX * kScreenBounds.GetWidth();
//			fY = (1 - fY) * kScreenBounds.GetHeight();
//		}
//		else
//		{
//			fX = fY = 0;
//		}
//
//		sStream << szName << "|" << fX << "|" << fY << "|\n";
//	}
//
//	//野外
//	/*for(int i=1; i<8; ++i)
//	{
//		char szName[20];
//		sprintf(szName, "V_%02d", i);
//
//		float fX, fY;
//		NiPoint3 kPosition = _getNodePosition(pkRootNode, szName);
//
//		if(m_spCamera->WorldPtToScreenPt(kPosition, fX, fY))
//		{
//			fX = fX * kScreenBounds.GetWidth();
//			fY = (1 - fY) * kScreenBounds.GetHeight();
//		}
//		else
//		{
//			fX = fY = 0;
//		}
//		sStream << szName << "|" << fX << "|" << fY << "|\n";
//	}*/
//
//	return sStream.str();
//}

const NiPoint3& UI_AvatarManager::_getNodePosition(const NiNodePtr& pkRootNode, const char *szName)
{
	NiAVObjectPtr spPosNiNode = pkRootNode->GetObjectByName(szName);
	T_ASSERT_CRITICAL_S(spPosNiNode != NULL);
	
	return spPosNiNode->GetWorldTranslate();
}


//bool UI_AvatarTexture::SetWorldMapRoadLineColor(int iLineIdx, float fColorRed, float fColorGreen, float fColorBlue)
//{
//	ModelKeyPtrMap::iterator itr = m_kCurrentModels.find(_getActorModelKey(100002, 0, CM_WORLDMAP));
//	if (itr == m_kCurrentModels.end())
//	{
//		return false;
//	}
//
//	GD_ClientActorModel* pkModel = (*itr).second;
//	if(NULL == pkModel)
//		return false;
//
//	Data::GD_ModelBase::AttachedFxMap kFxMap = pkModel->GetAttachedFxMap(GD_ClientActorModel::FDT_GROUND);
//	if(!kFxMap.empty())
//	{
//		bool bResult = false;
//		FX::FX_FxManager &pkFxMgr = FX::FX_FxManager::GetSingleton();
//		for (Data::GD_ModelBase::AttachedFxMap::iterator itr = kFxMap.begin(); itr != kFxMap.end(); ++itr)
//		{
//			if((*itr).first == RES_FX_WORLDMAP_ROADLINE)
//			{
//				int iFxObjectID = (*itr).second;
//				FX::FX_FxManager::FxObject *pkFxObj = pkFxMgr.GetFxObjectByID(iFxObjectID);
//				if(NULL != pkFxObj)
//				{
//					FX::FX_Composite *pkFxComposite = pkFxObj->pkComposite;
//					if(NULL != pkFxComposite)
//					{
//						for(int i=0; i<pkFxComposite->GetElementCountForClient(); ++i)
//						{
//							FX::FX_Element *pkElement = pkFxComposite->GetElementForClient(i);
//							if(NULL == pkElement)
//							{
//								return false;
//							}
//							else
//							{
//								if(i <= iLineIdx)
//									pkElement->SetExternalColor(NiColor(fColorRed, fColorGreen, fColorBlue));
//								else
//									pkElement->SetExternalColor(NiColor(255, 0, 0));
//							}
//						}
//
//						bResult = true;
//					}
//				}
//			}
//		}
//		return bResult;
//	}
//
//	return false;
//}

//bool UI_AvatarTexture::SetWorldMapRoadLineVisible(int iLineIdx)
//{
//	ModelKeyPtrMap::iterator itr = m_kCurrentModels.find(_getActorModelKey(100002, 0, CM_WORLDMAP));
//	if (itr == m_kCurrentModels.end())
//	{
//		return false;
//	}
//
//	GD_ClientActorModel* pkModel = (*itr).second;
//	if(NULL == pkModel)
//		return false;
//
//	Data::GD_ModelBase::AttachedFxMap kFxMap = pkModel->GetAttachedFxMap(GD_ClientActorModel::FDT_GROUND);
//	if(!kFxMap.empty())
//	{
//		bool bResult = false;
//		FX::FX_FxManager &pkFxMgr = FX::FX_FxManager::GetSingleton();
//		for (Data::GD_ModelBase::AttachedFxMap::iterator itr = kFxMap.begin(); itr != kFxMap.end(); ++itr)
//		{
//			if((*itr).first == RES_FX_WORLDMAP_ROADLINE)
//			{
//				int iFxObjectID = (*itr).second;
//				FX::FX_FxManager::FxObject *pkFxObj = pkFxMgr.GetFxObjectByID(iFxObjectID);
//				if(NULL != pkFxObj)
//				{
//					FX::FX_Composite *pkFxComposite = pkFxObj->pkComposite;
//					if(NULL != pkFxComposite)
//					{
//						for(int i=0; i<pkFxComposite->GetElementCountForClient(); ++i)
//						{
//							FX::FX_Element *pkElement = pkFxComposite->GetElementForClient(i);
//							if(NULL == pkElement)
//							{
//								return false;
//							}
//							else
//							{
//								bool bShow = i <= iLineIdx;
//								pkElement->SetVisibility(bShow);
//								if(bShow)
//									pkElement->SetExternalColor(NiColor(0, 255, 0));
//							}
//						}
//
//						bResult = true;
//					}
//				}
//			}
//		}
//		return bResult;
//	}
//
//	return false;
//}

//void UI_AvatarManager::SetAvatarViewPort(int cameraMode, int charIndex, const StringType& sTextureName, float fWidth, float fHeight)
//{
//	/*NiRect<float> kViewPort;
//	float fOriginW = 0.0f;
//	float fOriginH = 0.0f;
//	float fL = 0.0f;
//	float fT = 0.0f;
//
//	switch(iCameraMode)
//	{
//	case CM_WORLDMAP:
//		{
//			kViewPort = m_kWorldMapViewPort[iIndex];
//			fOriginW = WORLDMAP_WIDTH;
//			fOriginH = WORLDMAP_HEIGHT;
//			fL = 0.0f;
//			fT = 0.0f;
//			if(iIndex > 0)
//				fT = WORLDMAP_HEIGHT;
//		}
//		break;
//	default:
//		break;
//	}
//
//	float fLeft = kViewPort.m_left;
//	float fTop = kViewPort.m_top;
//	kViewPort.m_right = fLeft + fWidth * fOriginW * SCALE / (AVATAR_TEX_WIDTH * g_settings.WINDOW_WIDTH);
//	kViewPort.m_bottom = fTop - fHeight * fOriginH * SCALE / (AVATAR_TEX_HEIGHT * g_settings.WINDOW_HEIGHT);
//
//	IG_Image *pkImg = IG_ImageManager::GetSingleton().GetImage(sTextureName);
//	SAFE_RELEASE(pkImg);
//
//	_createImage(sTextureName, fL, fT, fWidth, fHeight);
//
//	_calcCameraFrustum(m_fFov[iCameraMode], (float)fWidth / (float)fHeight, m_akFrustums[iCameraMode]);
//	_calcCameraRotationMatrix(m_fPitch[iCameraMode], m_fYaw[iCameraMode], m_akCameraRotation[iCameraMode]);*/
//}

//void UI_AvatarManager::SetEscUiViewPort(float fLeft, float fTop, float fRight, float fBottom)
//{
//	m_kEscUICharViewPortNoUseRT[0].m_left = fLeft;
//	m_kEscUICharViewPortNoUseRT[0].m_right = fRight;
//	m_kEscUICharViewPortNoUseRT[0].m_top = fTop;
//	m_kEscUICharViewPortNoUseRT[0].m_bottom = fBottom;
//}
//
//void UI_AvatarManager::SetGreatKungfuViewPort(float fLeft, float fTop, float fRight, float fBottom)
//{
//	m_kGreatKungfuViewPortNoUseRT[0].m_left = fLeft;
//	m_kGreatKungfuViewPortNoUseRT[0].m_right = fRight;
//	m_kGreatKungfuViewPortNoUseRT[0].m_top = fTop;
//	m_kGreatKungfuViewPortNoUseRT[0].m_bottom = fBottom;
//}




