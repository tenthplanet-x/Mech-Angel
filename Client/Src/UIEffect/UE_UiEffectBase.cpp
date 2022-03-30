#include "ClientPCH.h"
#include "CA_CameraManager.h"
#include "GD_ModelContentManager.h"
#include "UE_UiEffectBase.h"

#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientPlaceHolderManager.h"
#include "LC_ClientNPCManager.h"
#include "LC_ClientLogicManager.h"

#include "RS_ScreenElements.h"
#include "IM_Manager.h"
#include "UE_RenderManager.h"
#include "IG_Material.h"
#include "CR_HelpFunction.h"

#include "LC_ClientPlaceHolder.h"

using namespace CSVFile;
using namespace Data;
using namespace Utility;
using namespace GameLogic;
using namespace GUISys;
//------------------------------------------------------------------------------
UE_UiEffectBase::UE_UiEffectBase(UI_EFFECT_TYPE eType)
: m_eType(eType)
, m_lTargetID(INVALID_ACTOR_ID)
, m_fStartShowTime(0.0f)
, m_fDelayShowTime(0.0f)
, m_fLastChangeTime(0.0f)
, m_lShowPosIndex(1)
, m_fPlayTime(0.0f)
, m_pkModelContent(NULL)
, m_spEffectRoot(0)
, m_lActorHeight(0)
, m_bStartPlay(false)
, m_spInterpolator(NULL)
, m_bShow(false)
, m_fDelayInterval(0.0f)
, m_bVisible(false)
, m_fAlpha(1.0f)
, m_fScale(1.0f)
, m_fInterpolatorBeginTime(0)
, m_fInterpolatorEndTime(FLT_MAX)
{	
	m_kDeltaPos = NiPoint3::ZERO;
}
//------------------------------------------------------------------------------
UE_UiEffectBase::~UE_UiEffectBase()
{
	m_spEffectRoot = 0;
}
//------------------------------------------------------------------------------
bool UE_UiEffectBase::Update(float fCurrentTime)
{	
	T_PROFILE("Update::UE_UiEffec");
	if ( INVALID_ACTOR_ID == m_lTargetID || _targetDestroyed() )
	{
		return false;
	}
	//只有当后台加载已经完成才显示这些UI效果
	if ( !_targetLoaded() )
	{
		//GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "Can't show ui effect for %d" , m_lTargetID );
		return true;
	}
	//当完成角色加载后进行这里，第一次进入的时候更新StartTime
	if ( !m_bStartPlay )
	{
		m_fStartShowTime = fCurrentTime;
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  "start show ui effect for %d" , m_lTargetID );
		m_bStartPlay = true;
	}

	float dTime = fCurrentTime - m_fStartShowTime;

	//NiInterpolator::update，传入GenInterp的时间必须在在帧时间范围内。所以dtime的取值在GetActiveTimeRange中
	if(dTime > m_fInterpolatorEndTime)
	{
		dTime = 0.0f;
		m_fStartShowTime = fCurrentTime;
	}

	if(dTime < 0.01f /*|| dTime >= m_fPlayTime*/)
		return true;

	if(_IsForever() || dTime < m_fPlayTime)
	{	
		Utility::UT_Vec3Int kPosition;
		LC_ActorBase* pkTarget = _getTargetPosition(kPosition);
		if (!pkTarget)
		{		
			return false;
		}

		
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		//如果非主角角色隐身并且没有被反隐
		if(pkTarget->GetID() != pkLogicManager->GetOperationPlayerID() && pkTarget->GetSkillFlag(SK_INVISIBLE) && !pkTarget->GetRevealInvisibility())
		{
			if ( m_spEffectRoot )
			{
				m_spEffectRoot->SetAppCulled(true);
			}
			m_bVisible = false;
		}
		else
		{
			//根据距离玩家的远近决定是否显示UI效果
			NiPoint3 kPlayerPos = NiPoint3::ZERO;
			bool bGetPlayerPos = CA_CameraManager::GetSingletonPtr()->GetCurrActorPlayerLocation( kPlayerPos );
			if ( bGetPlayerPos )
			{
				kPosition.z += m_lActorHeight;
				NiPoint3 kMyPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition);
				NiPoint3 kDiff = kMyPos - kPlayerPos;

				float configDistance=g_settings.LOD_PLAYER_NAME_DISTANCE;
				float CullingDistance=IS_ARENA_MAP(pkTarget->GetMapLogicType())?configDistance*3:configDistance;      //距离当前玩家多远角色的名字就不显示了？;
				if( kDiff.Length() < CullingDistance )
				{
					m_bVisible = true;
					if ( m_spEffectRoot )
					{

						m_spEffectRoot->SetAppCulled( false );
						m_spEffectRoot->SetTranslate( kMyPos );
						m_kShowStartPos = kMyPos;
						m_spEffectRoot->Update(dTime);

						if(m_spInterpolator)
							m_spInterpolator->Update(dTime,m_spEffectRoot,m_kUpdateValue);
					}
			
					if(m_spMaterialProperty)
						m_fAlpha = m_spMaterialProperty->GetAlpha();
					//m_spPropertyNode->Update(dTime);
				}
				else
				{
					m_bVisible = false;
					if ( m_spEffectRoot )
					{
						m_spEffectRoot->SetAppCulled( true );
					}
				}
			}
		}

		return true;
	}
	else
	{		
		if ( m_spEffectRoot )
		{
			m_spEffectRoot->SetAppCulled(true);
		}

		Hide();

		return false;
	}		
}
//------------------------------------------------------------------------------
LC_ActorBase* UE_UiEffectBase::_getTargetPosition(Utility::UT_Vec3Int& rkPosition) const
{
	/*if ( IS_MONSTER_ID(m_lTargetID) )
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		T_ASSERT(pkNPCManager);

		LC_ClientNPC* pkNPC = (LC_ClientNPC*)(pkNPCManager->FindNPC(m_lTargetID));
		if (pkNPC)
		{
			rkPosition = pkNPC->GetVisibleLocation();

			return pkNPC;
		}
	}*/

	if ( IS_PLAYER_CLASS_ID(m_lTargetID) )
	{
		LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
		LC_PlayerBase* pkPlayer = NULL;
		pkPlayer = pkPlayerManager->FindPlayer(m_lTargetID);
		if(!pkPlayer)
		{
			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			pkPlayer = pkNetPlayerManager->FindPlayer(m_lTargetID);
		}

		if (pkPlayer)
		{
			rkPosition = pkPlayer->GetVisibleLocation();
			return pkPlayer;
		}
	}

	if ( IS_NPC_ID(m_lTargetID) || IS_LOCAL_NPC_ID(m_lTargetID))
	{
		LC_ClientNPCManager* pkNpcManager = CLIENT_GET_NPC_MANAGER();
		T_ASSERT(pkNpcManager);
		LC_ClientNPC* pkNPC = (LC_ClientNPC*)(pkNpcManager->FindNPC(m_lTargetID));
		if (pkNPC)
		{			
			rkPosition = pkNPC->GetVisibleLocation();
			return pkNPC;
		}
	}

	return NULL;
}
//------------------------------------------------------------------------------
void UE_UiEffectBase::_findPlayTime(NiObjectNET* pkObjectNET)
{	
	if(!pkObjectNET)
		return;

	NiTimeControllerPtr spTimeController = pkObjectNET->GetControllers();
	
	while(spTimeController)
	{
		if(NiIsKindOf(NiAlphaController,spTimeController) && NiIsKindOf(NiProperty,pkObjectNET))
		{
			NiAlphaControllerPtr spAlphaController = NiDynamicCast(NiAlphaController,spTimeController);
			NiPropertyPtr spProperty =  NiDynamicCast(NiProperty,pkObjectNET);
			//NiPropertyPtr spNewProperty =  NiDynamicCast(NiProperty,spProperty->Clone());
			//if (  m_spPropertyNode->GetProperty( spNewProperty->Type() ) == NULL )
			//{
			if(!m_spMaterialProperty)
				m_spMaterialProperty = NiDynamicCast(NiMaterialProperty,spProperty);
				//m_spPropertyNode->AttachProperty(spNewProperty);
				//m_spPropertyNode->UpdateProperties();
			//}
		}
		if(NiIsKindOf(NiTransformController,spTimeController))
		{
			NiTransformControllerPtr spTransformController = NiDynamicCast(NiTransformController,spTimeController);
			
			if(!m_spInterpolator)
			{
				m_spInterpolator = spTransformController->GetInterpolator();
				if(m_spInterpolator)
				{
					m_spInterpolator->GetActiveTimeRange(m_fInterpolatorBeginTime, m_fInterpolatorEndTime);
				}
			}
			
		}
		if (spTimeController->GetEndKeyTime() > m_fPlayTime && spTimeController->GetCycleType() != NiTimeController::LOOP)
		{
			m_fPlayTime = spTimeController->GetEndKeyTime();
		}

		spTimeController = spTimeController->GetNext();


	}

	if (NiIsKindOf(NiAVObject, pkObjectNET))
	{
		NiAVObject* pkObject = NiDynamicCast(NiAVObject, pkObjectNET);
		const NiPropertyList& rPropertyList = pkObject->GetPropertyList();
		NiTListIterator it = rPropertyList.GetHeadPos();

		while( !rPropertyList.IsEmpty() && it )
		{
			const NiProperty* pkProperty = rPropertyList.GetNext(it);
			if (pkProperty)
			{
				_findPlayTime( (NiObjectNET*)pkProperty );
			}
		}
	}

	if (NiIsKindOf(NiNode, pkObjectNET))
	{
		NiNode* pkNode = NiDynamicCast(NiNode, pkObjectNET);
		for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ++ui)
		{
			NiAVObject* pkChild = pkNode->GetAt(ui);
			if (pkChild)
			{
				_findPlayTime(pkChild);
			}
		}

		const NiDynamicEffectList& rEffectList = pkNode->GetEffectList();
		NiTListIterator it = rEffectList.GetHeadPos();
		while( !rEffectList.IsEmpty() && it )
		{
			const NiDynamicEffect* pkEffect = rEffectList.GetNext(it);
			if (pkEffect)
			{
				_findPlayTime( (NiObjectNET*)pkEffect );
			}
		}
	}
}
//------------------------------------------------------------------------------
bool UE_UiEffectBase::_findActorHeight()
{
	//LC_ActorBase* pkActor = NULL;

	//if ( IS_MONSTER_ID(m_lTargetID) )
	//{
	//	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_MONSTER_MANAGER();
	//	T_ASSERT(pkNPCManager);

	//	pkActor = pkNPCManager->FindNPC(m_lTargetID);	
	//}

	//if ( IS_PLAYER_ID(m_lTargetID) )
	//{
	//	LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
	//	LC_PlayerBase* pkPlayer = NULL;
	//	pkActor = pkPlayerManager->FindPlayer(m_lTargetID);
	//	if(!pkActor)
	//	{
	//		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	//		pkActor = pkNetPlayerManager->FindPlayer(m_lTargetID);
	//	}
	//}

	//if ( IS_NPC_ID(m_lTargetID) )
	//{
	//	LC_ClientNPCManager* pkNpcManager = CLIENT_GET_NPC_MANAGER();
	//	T_ASSERT(pkNpcManager);
	//	pkActor = pkNpcManager->FindNPC(m_lTargetID);
	//}

	LC_ActorBase* pkActor = _getTargetActor();
	if(pkActor == NULL)
		return false;

	if (pkActor->GetType() == LOT_PLACEHOLDER)
	{
		m_lActorHeight = 50;
	}
	else
		m_lActorHeight = pkActor->GetProperty_ShapeHeight();
	
	return true;
}

bool UE_UiEffectBase::_findActorShowEffectTime()
{
	LC_ActorBase* pkActor = _getTargetActor();
	if (pkActor == NULL)
	{
		return false;
	}

	//一共提供3个特效位，依次检查哪个可用

	float fLastShowUiEffectTime;
	float fCurrentTime = GET_PROCESS_TIME;

	fLastShowUiEffectTime = Long2Float(pkActor->GetCommonParamter(ACPC_UE_POS1_SHOW_TIME)) / 1000.0f;

	if(fLastShowUiEffectTime < fCurrentTime + 1.5f)
	{
		if (m_fStartShowTime - fLastShowUiEffectTime < m_fDelayInterval )
			m_fStartShowTime = fLastShowUiEffectTime + m_fDelayInterval;

		pkActor->SetCommonParamter(ACPC_UE_POS1_SHOW_TIME,Float2Long(m_fStartShowTime * 1000.0f));

		m_lShowPosIndex = 1;

		return true;
	}
	
	
	fLastShowUiEffectTime = Long2Float(pkActor->GetCommonParamter(ACPC_UE_POS2_SHOW_TIME)) / 1000.0f;

	if(fLastShowUiEffectTime < fCurrentTime + 1.5f)
	{
		if (m_fStartShowTime - fLastShowUiEffectTime < m_fDelayInterval )
			m_fStartShowTime = fLastShowUiEffectTime + m_fDelayInterval;

		pkActor->SetCommonParamter(ACPC_UE_POS2_SHOW_TIME,Float2Long(m_fStartShowTime * 1000.0f));

		m_lShowPosIndex = 2;

		return true;
	}

	fLastShowUiEffectTime = Long2Float(pkActor->GetCommonParamter(ACPC_UE_POS3_SHOW_TIME)) / 1000.0f;

	if(fLastShowUiEffectTime < fCurrentTime + 1.5f)
	{
		if (m_fStartShowTime - fLastShowUiEffectTime < m_fDelayInterval )
			m_fStartShowTime = fLastShowUiEffectTime + m_fDelayInterval;

		pkActor->SetCommonParamter(ACPC_UE_POS3_SHOW_TIME,Float2Long(m_fStartShowTime * 1000.0f));

		m_lShowPosIndex = 3;

		return true;
	}

	//默认居中显示
	pkActor->SetCommonParamter(ACPC_UE_POS1_SHOW_TIME,Float2Long(m_fStartShowTime * 1000.0f));
	m_lShowPosIndex = 1;

	return true;
}

bool UE_UiEffectBase::_targetLoaded()
{
	LC_ActorBase* pkActor = _getTargetActor();
	if ( pkActor == NULL )
	{
		return false;
	}
	//是否能安全转型为GD_ClientActorModel?
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(pkActor->GetID());
	if ( pkVisibleData)
	{
		return true;
	}
	return false;
}

LC_ActorBase* UE_UiEffectBase::_getTargetActor()
{
	LC_ActorBase* pkActor = NULL;
	/*if ( IS_MONSTER_ID(m_lTargetID) )
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		T_ASSERT(pkNPCManager);

		pkActor = pkNPCManager->FindNPC(m_lTargetID);	
	}*/

	if ( IS_PLAYER_CLASS_ID(m_lTargetID) )
	{
		LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
		LC_PlayerBase* pkPlayer = NULL;
		pkActor = pkPlayerManager->FindPlayer(m_lTargetID);
		if(!pkActor)
		{
			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			pkActor = pkNetPlayerManager->FindPlayer(m_lTargetID);
		}
	}

	else if ( IS_NPC_ID(m_lTargetID) || IS_LOCAL_NPC_ID(m_lTargetID))
	{
		LC_ClientNPCManager* pkNpcManager = CLIENT_GET_NPC_MANAGER();
		T_ASSERT(pkNpcManager);
		pkActor = pkNpcManager->FindNPC(m_lTargetID);
	}

	else if ( IS_CHEST_ID(m_lTargetID) )
	{
		LC_ClientPlaceHolderManager* pkPlaceHolderManager = CLIENT_GET_PLACEHOLDER_MANAGER();
		pkActor = pkPlaceHolderManager->FindPlaceHolder(m_lTargetID);
	}

	return pkActor;
}

bool UE_UiEffectBase::_targetDestroyed()
{
	LC_ActorBase* pkActor = _getTargetActor();
	if ( pkActor == NULL )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UE_UiEffectBase::Show()
{
	_findActorHeight();
	m_lActorHeight += 50;

	m_fStartShowTime = GET_PROCESS_TIME;
	_findActorShowEffectTime();
	m_fDelayShowTime = m_fStartShowTime;
	m_bShow = false;

	Utility::UT_Vec3Int kPosition;
	_getTargetPosition(kPosition);
	kPosition.z += m_lActorHeight;
	if(NULL == m_spEffectRoot)
		return;
	m_spEffectRoot->SetTranslate(UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition));
	m_spEffectRoot->Update(0.0f);
	m_kShowStartPos = m_spEffectRoot->GetWorldTranslate();

	m_fScale =  m_spEffectRoot->GetScale();

	if(m_spMaterialProperty)
		m_fAlpha = m_spMaterialProperty->GetAlpha();
}
//------------------------------------------------------------------------------