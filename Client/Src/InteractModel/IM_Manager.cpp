#include "ClientPCH.h"

#include "CA_CameraManager.h"

#include "GD_ClientAsyncModelManager.h"
#include "GD_ClientChestModel.h"
#include "GD_ClientActorModel.h"
#include "GD_ClientBreakModel.h"

#include "IM_Manager.h"
#include "BM_Manager.h"

#include "LC_ClientLogicManager.h"

#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "GD_SequenceManager.h"
#include "LC_ClientPlaceHolderManager.h"
#include "LC_ClientPlaceHolder.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientNPCManager.h"
#include "CF_BuddyTransform.h"

using namespace Base;
using namespace CSVFile;
using namespace Data;
using namespace Utility;

IM_Manager::IM_Manager(void)
: m_bInit(false)

, m_pkPick(NiNew NiPick)
, m_bPickSwitch(false)

, m_kCuller(NULL)
{
	m_pkPick->SetPickType(NiPick::FIND_FIRST);
	m_pkPick->SetSortType(NiPick::SORT);
	m_pkPick->SetCoordinateType(NiPick::WORLD_COORDINATES);
	m_pkPick->SetIntersectType(NiPick::TRIANGLE_INTERSECT);
	m_pkPick->SetFrontOnly(false);
	m_pkSequenceMgr = T_NEW_D GD_SequenceManager;
	m_bShowActor = true;

	for(int i = SHIELD_ALL ; i < SHIELD_MAX; i++)
	{
		CloseShieldPending(i);
		SetShieldVisibility(i,true);
	}
}

IM_Manager::~IM_Manager(void)
{
	if (m_pkPick)
	{
		NiDelete m_pkPick;
		m_pkPick = NULL;
	}
	T_DELETE_D( m_pkSequenceMgr );
}

void IM_Manager::Init(void)
{
    T_ASSERT(!m_bInit);

	m_bInit = true;
	m_bPickSwitch = true;
	m_pkSequenceMgr->Init();
}

void IM_Manager::UnInit(void)
{
	T_ASSERT (m_bInit);

	m_bInit = false;
	m_bPickSwitch = false;
	m_pkSequenceMgr->Shutdown();

	this->Reset();
}

void IM_Manager::Reset()
{
	// 这个pick的result中会保存sceneWorldMap中对象的smartpointer，
	// 所以在sceneWorldMap清空之前，把这个Pick的result先清空掉。
	if ( m_pkPick )
	{
		m_pkPick->RemoveTarget();
		m_pkPick->DeepClearResultsArray();
	}

	GD_ClientAsyncModelManager& pkAsyncModelManager = GD_ClientAsyncModelManager::GetSingleton();

	// 释放宝箱模型
	ChestIdPtrMap::iterator itrPlaceHolder;
	TRAV(m_kPlaceHolders, itrPlaceHolder)
	{
		pkAsyncModelManager.ReleaseModel((*itrPlaceHolder).second);
	}
	m_kPlaceHolders.clear();

	// 释放角色模型
	ActorIdPtrMap::iterator itrActor;
	TRAV(m_kActors, itrActor)
	{
		pkAsyncModelManager.ReleaseModel((*itrActor).second);
	}
	m_kActors.clear();

	// 释放坐骑模型
	TRAV(m_kRiderActors, itrActor)
	{
		pkAsyncModelManager.ReleaseModel((*itrActor).second);
	}
	m_kRiderActors.clear();

	// 释放法宝模型
	TRAV(m_kFabaoActors, itrActor)
	{
		pkAsyncModelManager.ReleaseModel((*itrActor).second);
	}
	m_kFabaoActors.clear();

	// 释放暗器模型
	TRAV(m_kAnqiActors, itrActor)
	{
		pkAsyncModelManager.ReleaseModel((*itrActor).second);
	}
	m_kAnqiActors.clear();

	// 释放角色死亡模型
	SceneIdPtrMap::iterator itrScene;
	TRAV(m_kNpcDeadActors, itrScene)
	{
		pkAsyncModelManager.ReleaseModel((*itrScene).second);
	}
	m_kNpcDeadActors.clear();
	BM_Manager* pkBMManager = BM_Manager::GetSingletonPtr();
	if (pkBMManager)
		pkBMManager->ReSet();

	for(int i = SHIELD_ALL ; i < SHIELD_MAX; i++)
	{
		CloseShieldPending(i);
		SetShieldVisibility(i,true);
	}
}

bool IM_Manager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
    MESSAGE_HANDLE(MGT_INPUT_MOUSE_MOVE,		    _event_InputMouseMove)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_DOWN,    _event_InputLButtonDown)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_DBCLK,   _event_InputLButtonDBClk)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_RBUTTON_DOWN,    _event_InputRButtonDown)
	MESSAGE_HANDLE(MGT_IM_PLAY_ANIMATION,			_event_IMPlayAnimation)
	END_MESSAGE_HANDLE

	return true;
}

void IM_Manager::PreCullModels(void)
{
    m_kVisibleArrayCache.RemoveAll();

    GetVisibleArray(CA_CameraManager::GetSingleton().GetNiCamera(), m_kCuller, m_kVisibleArrayCache);
}

void IM_Manager::GetVisibleArray(NiGeometryList& rkCasterList)
{
    for (unsigned int i = 0; i < m_kVisibleArrayCache.GetCount(); ++i)
    {
        rkCasterList.AddTail(&m_kVisibleArrayCache.GetAt(i));
    }
}

void IM_Manager::GetShadowCasters(NiGeometryList& rkCasterList)
{
	for (unsigned int i = 0; i < m_kVisibleArrayCache.GetCount(); ++i)
	{
		NiGeometry* pkGeom = &m_kVisibleArrayCache.GetAt(i);
		NiAlphaProperty* pkAlpha = (NiAlphaProperty*)pkGeom->GetProperty(NiProperty::ALPHA);
		//NiZBufferProperty* pkZ =  (NiZBufferProperty*)pkGeom->GetProperty(NiProperty::ZBUFFER);

		/// Trick: 特效墙实现机制 ShaderConst 20140111
		/// 1、特效墙只能通过NPC的方式实现，且其不能投射阴影
		/// 2、好多NPC都是使用的ALPHABLEND来制作，修改量较大。
		if (pkAlpha /*&& pkZ*/)
		{
			if (pkAlpha->GetAlphaBlending() && ( pkGeom->GetName() == ("Box@#0") || pkGeom->GetName() == ("Box") )   /*&& pkZ->GetZBufferWrite() == false*/)
			{
				continue;
			}
		}

		rkCasterList.AddTail(pkGeom);
	}
}

void IM_Manager::GetVisibleArray(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray)
{
    if (m_kVisibleArrayCache.GetCount() == 0)
    {
        T_PROFILE("Cull::Actors&Chests");

	    if(!m_bShowActor)
		    return;
	    int iRenderCount = 0;

	    // 遍历宝箱模型
        ChestIdPtrMap::iterator itrPlaceHolder;
	    TRAV(m_kPlaceHolders, itrPlaceHolder)
	    {
            iRenderCount += (*itrPlaceHolder).second->Cull(pkNiCamera, rkCuller, rkVisibleArray);
	    }

	    // 遍历角色模型
	    ActorIdPtrMap::iterator itrActor;
	    TRAV(m_kActors, itrActor)
	    {
            GD_ClientActorModel* pkActorModel = (*itrActor).second;

            iRenderCount += pkActorModel->Cull(pkNiCamera, rkCuller, rkVisibleArray);
	    }

		//遍历坐骑模型
		TRAV(m_kRiderActors, itrActor)
		{
			GD_ClientActorModel* pkActorModel = (*itrActor).second;

			iRenderCount += pkActorModel->Cull(pkNiCamera, rkCuller, rkVisibleArray);
		}
		//遍历法宝模型
		TRAV(m_kFabaoActors, itrActor)
		{
			GD_ClientActorModel* pkActorModel = (*itrActor).second;

			iRenderCount += pkActorModel->Cull(pkNiCamera, rkCuller, rkVisibleArray);
		}
		//遍历暗器模型
		TRAV(m_kAnqiActors, itrActor)
		{
			GD_ClientActorModel* pkActorModel = (*itrActor).second;

			iRenderCount += pkActorModel->Cull(pkNiCamera, rkCuller, rkVisibleArray);
		}

		// 遍历角色死亡模型
		SceneIdPtrMap::iterator itrScene;
		TRAV(m_kNpcDeadActors, itrScene)
		{
			GD_ClientBreakModel* pkSceneModel = (*itrScene).second;

			iRenderCount += pkSceneModel->Cull(pkNiCamera, rkCuller, rkVisibleArray);
		}
    }
    else
    {
        for (unsigned int i = 0; i < m_kVisibleArrayCache.GetCount(); ++i)
        {
            rkVisibleArray.Add(m_kVisibleArrayCache.GetAt(i));
        }
    }
}

unsigned int IM_Manager::GetCurrentIMPlayerCreationCapacity(void)
{
    return GD_ClientAsyncModelManager::GetSingleton().GetCurrentPlayerModelCreationCapacity();
}

BS_IMInterface* IM_Manager::CreateChestModel(const StringType& strModel,
											 object_id_type lLogicID, 
											 const NiPoint3& kSrcPos, 
											 const NiPoint3& kDestPos, 
											 bool bNewModel, 
											 float fSpawnZRotAngle,
											 int bShowGfxLevel /*= 0*/,
											 float fAlpha /*= 1.0f*/)
{
	ChestIdPtrMap::iterator itrPlaceHolder = m_kPlaceHolders.find(lLogicID);
	if (itrPlaceHolder != m_kPlaceHolders.end())
	{
		T_ASSERT2(false, "Try to re-create chest model with the same logic ID!");
		return (*itrPlaceHolder).second;
	}
	
	//已鉴定和未鉴定。

	BS_IMInterface* pkInteractModel = NULL;
	GD_ModelBase* pkAsyncModel = NULL;

	pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
		GD_ClientAsyncModelManager::AMT_CHEST, strModel);

	GD_ClientChestModel* pkClientChestModel = (GD_ClientChestModel*)pkAsyncModel;
	if (!bNewModel)
	{
		//不是能飞行的物品才重置位置
		if(!pkClientChestModel->IsFlyAble())
		{
			pkClientChestModel->SetUserValue(lLogicID, 0, kDestPos, kDestPos, fSpawnZRotAngle, bShowGfxLevel);
			pkClientChestModel->SetTranslate(kDestPos);
			pkClientChestModel->SetDropSoundFlag(false);
		}
		
		//飞行相关
		pkClientChestModel->SetFlyToTime(g_settings.DS_DROPITEM_ANIM_FLY_LIFE_TIME);
		pkClientChestModel->SetFlyToAcceleration(g_settings.DS_DROPITEM_ANIM_FLYTO_ACCELERATION);
		pkClientChestModel->SetFlyToV0(g_settings.DS_DROPITEM_ANIM_FLYTO_V0);
		pkClientChestModel->SetFlyToVDump(g_settings.DS_DROPITEM_ANIM_FLYTO_DUMP);
		pkClientChestModel->SetFlyToVDump2(g_settings.DS_DROPITEM_ANIM_FLYTO_DUMP2);
		pkClientChestModel->SetFlyAcceDisFactor(g_settings.DS_DROPItem_ANIM_FLY_ACCE_DIS_FACTOR);
		pkClientChestModel->SetFlyHeighTime(g_settings.DS_DROPITEM_ANIM_FLY_HEIGH_LIFE_TIME);
		pkClientChestModel->SetFlyHeighAcceleration(g_settings.DS_DROPITEM_ANIM_FLY_ACCELERATION);
		pkClientChestModel->SetFlyHeighV0(g_settings.DS_DROPITEM_ANIM_FLY_V0);
		pkClientChestModel->SetFlyHeighRadius(g_settings.DS_DROPITEM_ANIM_FLY_HEIGH_RADIUS);
		pkClientChestModel->SetFlyHeighH(g_settings.DS_DROPITEM_ANIM_FLY_HEIGH_H);
		
	}
	else
	{
		//float fWaitTime = UT_MathBase::RandFloat01() * (g_settings.DS_DROPITEM_WAITTIME_MAX - g_settings.DS_DROPITEM_WAITTIME_MIN) + g_settings.DS_DROPITEM_WAITTIME_MIN;
		float fWaitTime = 0;
		pkClientChestModel->SetWaitingTime(fWaitTime);
		pkClientChestModel->SetTranslate(kSrcPos);
		pkClientChestModel->SetUserValue(lLogicID, g_settings.DS_DROPITEM_ANIM_LIFE_TIME, kSrcPos, kDestPos, fSpawnZRotAngle, bShowGfxLevel);
		pkClientChestModel->SetDropSoundFlag(true);

		//飞行相关
		pkClientChestModel->SetFlyToTime(g_settings.DS_DROPITEM_ANIM_FLY_LIFE_TIME);
		pkClientChestModel->SetFlyToAcceleration(g_settings.DS_DROPITEM_ANIM_FLYTO_ACCELERATION);
		pkClientChestModel->SetFlyToV0(g_settings.DS_DROPITEM_ANIM_FLYTO_V0);
		pkClientChestModel->SetFlyToVDump(g_settings.DS_DROPITEM_ANIM_FLYTO_DUMP);
		pkClientChestModel->SetFlyToVDump2(g_settings.DS_DROPITEM_ANIM_FLYTO_DUMP2);
		pkClientChestModel->SetFlyAcceDisFactor(g_settings.DS_DROPItem_ANIM_FLY_ACCE_DIS_FACTOR);
		pkClientChestModel->SetFlyHeighTime(g_settings.DS_DROPITEM_ANIM_FLY_HEIGH_LIFE_TIME);
		pkClientChestModel->SetFlyHeighAcceleration(g_settings.DS_DROPITEM_ANIM_FLY_ACCELERATION);
		pkClientChestModel->SetFlyHeighV0(g_settings.DS_DROPITEM_ANIM_FLY_V0);
		pkClientChestModel->SetFlyHeighRadius(g_settings.DS_DROPITEM_ANIM_FLY_HEIGH_RADIUS);
		pkClientChestModel->SetFlyHeighH(g_settings.DS_DROPITEM_ANIM_FLY_HEIGH_H);
	}
	
	pkClientChestModel->SetAlpha(fAlpha);
	pkClientChestModel->SetPickFlag(BS_IMInterface::PickMask(BS_IMInterface::PM_RIGHT | BS_IMInterface::PM_LEFT));
	

	m_kPlaceHolders[lLogicID] = pkClientChestModel;

	pkInteractModel = (BS_IMInterface*)pkClientChestModel;
	pkInteractModel->SetLogicID(lLogicID);

	return pkInteractModel;
}

BS_IMInterface* IM_Manager::CreateInteractModel(long lCharType, 
                                                object_id_type lLogicID, 
												bool  bIsPlayer,
                                                const NiPoint3& kPos, 
                                                const NiMatrix3& kRot /*= NiMatrix3::IDENTITY*/, 
                                                float fAlpha /*= 1.0f*/)
{
    //T_ASSERT(IS_PLAYER_ID(lLogicID) || IS_NPC_ID(lLogicID) || /*IS_MONSTER_ID(lLogicID) ||*/ IS_VEHICLE_ID(lLogicID));

    ActorIdPtrMap::iterator itrActor = m_kActors.find(lLogicID);
    if (itrActor != m_kActors.end())
    {
        return (*itrActor).second;
    }

    const CF_CharType::DataEntry* pkData = CF_CharType::GetSingleton().GetEntryPtr(lCharType);

    GD_ModelBase* pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
        bIsPlayer ? GD_ClientAsyncModelManager::AMT_PLAYER : GD_ClientAsyncModelManager::AMT_NPC, 
        pkData->_kResName);

    if (bIsPlayer)
    {
        pkAsyncModel->SetImportance(IM_NORMAL);
    }

    GD_ClientActorModel* pkClientActorModel = (GD_ClientActorModel*)pkAsyncModel;
    pkClientActorModel->SetCharType(lCharType);
    pkClientActorModel->SetTranslate(kPos);
    pkClientActorModel->SetRotate(kRot);
    pkClientActorModel->SetAlpha(fAlpha);

    m_kActors[lLogicID] = pkClientActorModel;

    BS_IMInterface* pkInteractModel = (BS_IMInterface*)pkClientActorModel;

    pkInteractModel->SetLogicID(lLogicID);

	return pkInteractModel;
}

BS_IMInterface* IM_Manager::CreateInteractPlayerModel(long lCharType, object_id_type lLogicID,bool bIsGamePlayer,	int weaponType,
										   const NiPoint3& kPos, bool bTransform/* = true*/,  const NiMatrix3& kRot /* = NiMatrix3::IDENTITY */, float fAlpha /* = 1.0f */)
{
	//T_ASSERT(IS_PLAYER_ID(lLogicID) || IS_NPC_ID(lLogicID) || /*IS_MONSTER_ID(lLogicID) ||*/ IS_VEHICLE_ID(lLogicID));

	ActorIdPtrMap::iterator itrActor = m_kActors.find(lLogicID);
	if (itrActor != m_kActors.end())
	{
		return (*itrActor).second;
	}

	int iID = lCharType;
	if(!bTransform)
	{
		const CF_Buddy::DataEntry* pkBuddyData = CF_Buddy::GetSingleton().GetEntryPtr(lCharType);
		T_ASSERT_CRITICAL_S(pkBuddyData != NULL);
		iID = pkBuddyData->_iDefaultTransform;
	}
	
	const CF_BuddyTransform::DataEntry* pkData = CF_BuddyTransform::GetSingleton().GetEntryPtr(iID);
	T_ASSERT_CRITICAL_S(pkData != NULL);
	const StringType & resName = pkData->_model;

	GD_ModelBase* pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
		GD_ClientAsyncModelManager::AMT_PLAYER, 
		resName);

	if (bIsGamePlayer)
	{
		pkAsyncModel->SetImportance(IM_VERY_IMPORTANT);
	}
	else
	{
		pkAsyncModel->SetImportance(IM_NORMAL);
// 			if ( !GetShieldVisibility(SHIELD_PLAYER) )
// 			{
// 				pkAsyncModel->SetVisibility(false);
// 			}
	}

	GD_ClientActorModel* pkClientActorModel = (GD_ClientActorModel*)pkAsyncModel;
	pkClientActorModel->SetCharType(pkData->_buddy);
	pkClientActorModel->SetTranslate(kPos);
	pkClientActorModel->SetRotate(kRot);
	pkClientActorModel->SetAlpha(fAlpha);

	m_kActors[lLogicID] = pkClientActorModel;

	BS_IMInterface* pkInteractModel = (BS_IMInterface*)pkClientActorModel;

	pkInteractModel->SetLogicID(lLogicID);

	return pkInteractModel;
}

Base::BS_IMInterface* IM_Manager::CreateInteractRiderModel( long lCharType, object_id_type lLogicID, const NiPoint3& kPos, const NiMatrix3& kRot /*= NiMatrix3::IDENTITY*/, float fAlpha /*= 1.0f*/ )
{
	ActorIdPtrMap::iterator itrActor = m_kRiderActors.find(lLogicID);
	if (itrActor != m_kRiderActors.end())
	{
		return (*itrActor).second;
	}

	const CF_CharType::DataEntry* pkData = CF_CharType::GetSingleton().GetEntryPtr(lCharType);

	GD_ModelBase* pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
		GD_ClientAsyncModelManager::AMT_NPC, 
		pkData->_kResName);

	GD_ClientActorModel* pkClientActorModel = (GD_ClientActorModel*)pkAsyncModel;
	pkClientActorModel->SetCharType(lCharType);
	pkClientActorModel->SetTranslate(kPos);
	pkClientActorModel->SetRotate(kRot);
	pkClientActorModel->SetAlpha(fAlpha);

	m_kRiderActors[lLogicID] = pkClientActorModel;

	BS_IMInterface* pkInteractModel = (BS_IMInterface*)pkClientActorModel;

	pkInteractModel->SetLogicID(lLogicID);

	return pkInteractModel;
}

Base::BS_IMInterface* IM_Manager::CreateInteractFabaoModel( long lCharType, object_id_type lLogicID, const NiPoint3& kPos, const NiMatrix3& kRot /*= NiMatrix3::IDENTITY*/, float fAlpha /*= 1.0f */ )
{
	ActorIdPtrMap::iterator itrActor = m_kFabaoActors.find(lLogicID);
	if (itrActor != m_kFabaoActors.end())
	{
		return (*itrActor).second;
	}

	const CF_CharType::DataEntry* pkData = CF_CharType::GetSingleton().GetEntryPtr(lCharType);

	GD_ModelBase* pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
		GD_ClientAsyncModelManager::AMT_NPC, 
		pkData->_kResName);

	GD_ClientActorModel* pkClientActorModel = (GD_ClientActorModel*)pkAsyncModel;
	pkClientActorModel->SetCharType(lCharType);
	pkClientActorModel->SetTranslate(kPos);
	pkClientActorModel->SetRotate(kRot);
	pkClientActorModel->SetAlpha(fAlpha);

	m_kFabaoActors[lLogicID] = pkClientActorModel;

	BS_IMInterface*pkInteractModel = (BS_IMInterface*)pkClientActorModel;

	pkInteractModel->SetLogicID(lLogicID);

	return pkInteractModel;
}

Base::BS_IMInterface* IM_Manager::CreateInteractAnQiModel( long lCharType, object_id_type lLogicID, const NiPoint3& kPos, const NiMatrix3& kRot /*= NiMatrix3::IDENTITY*/, float fAlpha /*= 1.0f*/ )
{
	ActorIdPtrMap::iterator itrActor = m_kAnqiActors.find(lLogicID);
	if (itrActor != m_kAnqiActors.end())
	{
		return (*itrActor).second;
	}

	const CF_CharType::DataEntry* pkData = CF_CharType::GetSingleton().GetEntryPtr(lCharType);

	GD_ModelBase* pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
		GD_ClientAsyncModelManager::AMT_NPC, 
		pkData->_kResName);

	GD_ClientActorModel* pkClientActorModel = (GD_ClientActorModel*)pkAsyncModel;
	pkClientActorModel->SetCharType(lCharType);
	pkClientActorModel->SetTranslate(kPos);
	pkClientActorModel->SetRotate(kRot);
	pkClientActorModel->SetAlpha(fAlpha);

	m_kAnqiActors[lLogicID] = pkClientActorModel;

	BS_IMInterface* pkInteractModel = (BS_IMInterface*)pkClientActorModel;

	pkInteractModel->SetLogicID(lLogicID);

	return pkInteractModel;
}

GD_ClientBreakModel* IM_Manager::CreateInteractDeadModel(const StringType& _ResName, object_id_type lLogicID, const NiPoint3& kPos, const NiMatrix3& kRot /*= NiMatrix3::IDENTITY*/, float fAlpha /*= 1.0f*/)
{
	SceneIdPtrMap::iterator itrScene = m_kNpcDeadActors.find(lLogicID);
	if (itrScene != m_kNpcDeadActors.end())
	{
		return (*itrScene).second;
	}

	GD_ModelBase* pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
		GD_ClientAsyncModelManager::AMT_NPC_DEAD, _ResName);

	GD_ClientBreakModel* pkClientSceneModel = (GD_ClientBreakModel*)pkAsyncModel;
	/*pkClientActorModel->SetCharType(lCharType);*/
	pkClientSceneModel->SetTranslate(kPos);
	pkClientSceneModel->SetRotate(kRot);
	pkClientSceneModel->SetAlpha(fAlpha);

	m_kNpcDeadActors[lLogicID] = pkClientSceneModel;

	return pkClientSceneModel;
}

void IM_Manager::DestroyInteractModel(object_id_type lLogicID)
{
    if (IS_CHEST_ID(lLogicID) || IS_DISPLAY_CHEST_ID(lLogicID))
    {
        ChestIdPtrMap::iterator itrPlaceHolder = m_kPlaceHolders.find(lLogicID);
        if (itrPlaceHolder != m_kPlaceHolders.end())
        {
            GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((*itrPlaceHolder).second);
            m_kPlaceHolders.erase(itrPlaceHolder);
        }
        else
        {
            GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, 
                "Try to destroy inexistent chest model [%s]", Int642String(lLogicID).c_str());
        }
    }
    else
    {
        ActorIdPtrMap::iterator itrActor = m_kActors.find(lLogicID);
        if (itrActor != m_kActors.end())
        {
            GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((*itrActor).second);
            m_kActors.erase(itrActor);
        }
    }
}

void IM_Manager::DestroyRiderInteractModel( object_id_type lLogicID )
{
	ActorIdPtrMap::iterator itrActor = m_kRiderActors.find(lLogicID);
	if (itrActor != m_kRiderActors.end())
	{
		GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((*itrActor).second);
		m_kRiderActors.erase(itrActor);
	}
}

void IM_Manager::DestroyFabaoInteractModel( object_id_type lLogicID )
{
	ActorIdPtrMap::iterator itrActor = m_kFabaoActors.find(lLogicID);
	if (itrActor != m_kFabaoActors.end())
	{
		GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((*itrActor).second);
		m_kFabaoActors.erase(itrActor);
	}
}

void IM_Manager::DestroyAnqiInteractModel( object_id_type lLogicID )
{
	ActorIdPtrMap::iterator itrActor = m_kAnqiActors.find(lLogicID);
	if (itrActor != m_kAnqiActors.end())
	{
		GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((*itrActor).second);
		m_kAnqiActors.erase(itrActor);
	}
}

void IM_Manager::DestoryDeadInteractModel(object_id_type lLogicID)
{
	SceneIdPtrMap::iterator itrScene = m_kNpcDeadActors.find(lLogicID);
	if (itrScene != m_kNpcDeadActors.end())
	{
		GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((*itrScene).second);
		m_kNpcDeadActors.erase(itrScene);
	}
}

GD_ModelBase* IM_Manager::GetModel(object_id_type lLogicID)
{
    GD_ModelBase* pkModel = NULL;

    pkModel = GetActor(lLogicID);
    if (pkModel == NULL)
    {
        pkModel = GetChest(lLogicID);
    }

    return pkModel;
}

GD_ClientChestModel* IM_Manager::GetChest(object_id_type lChestLogicID)
{
    ChestIdPtrMap::iterator itrPlaceHolder = m_kPlaceHolders.find(lChestLogicID);
    if (itrPlaceHolder == m_kPlaceHolders.end())
    {
        return NULL;
    }

    return (*itrPlaceHolder).second;
}

GD_ClientActorModel* IM_Manager::GetActor(object_id_type lActorLogicID)
{
    ActorIdPtrMap::iterator itrActor = m_kActors.find(lActorLogicID);
    if (itrActor == m_kActors.end())
	{
		return NULL;
	}

    return (*itrActor).second;
}

GD_ClientActorModel* IM_Manager::GetRiderActor( object_id_type lActorLogicID )
{
	ActorIdPtrMap::iterator itrActor = m_kRiderActors.find(lActorLogicID);
	if (itrActor == m_kRiderActors.end())
	{
		return NULL;
	}
	return (*itrActor).second;
}

GD_ClientActorModel* IM_Manager::GetFabaoActor( object_id_type lActorLogicID )
{
	ActorIdPtrMap::iterator itrActor = m_kFabaoActors.find(lActorLogicID);
	if (itrActor == m_kFabaoActors.end())
	{
		return NULL;
	}
	return (*itrActor).second;
}

GD_ClientActorModel* IM_Manager::GetAnQiActor( object_id_type lActorLogicID )
{
	ActorIdPtrMap::iterator itrActor = m_kAnqiActors.find(lActorLogicID);
	if (itrActor == m_kAnqiActors.end())
	{
		return NULL;
	}
	return (*itrActor).second;
}

void IM_Manager::ModifyPlayerModeID( object_id_type lOldLogicID, object_id_type lNewLogicID )
{
	if ( lOldLogicID != lNewLogicID )
	{
		ActorIdPtrMap::iterator itrActor = m_kActors.find(lOldLogicID);
	
		if (itrActor != m_kActors.end())
		{
			GD_ClientActorModel* modl = itrActor->second;
			m_kActors.erase(itrActor);
			m_kActors[lNewLogicID] = modl;
		}	
	}
}

void IM_Manager::ModifyRiderModeID( object_id_type lOldLogicID, object_id_type lNewLogicID )
{
	if ( lOldLogicID != lNewLogicID )
	{
		ActorIdPtrMap::iterator itrActor = m_kRiderActors.find(lOldLogicID);

		if (itrActor != m_kRiderActors.end())
		{
			GD_ClientActorModel* modl = itrActor->second;
			m_kRiderActors.erase(itrActor);
			m_kRiderActors[lNewLogicID] = modl;
		}	
	}
}

void IM_Manager::ModifyFabaoModeID( object_id_type lOldLogicID, object_id_type lNewLogicID )
{
	if ( lOldLogicID != lNewLogicID )
	{
		ActorIdPtrMap::iterator itrActor = m_kFabaoActors.find(lOldLogicID);

		if (itrActor != m_kFabaoActors.end())
		{
			GD_ClientActorModel* modl = itrActor->second;
			m_kFabaoActors.erase(itrActor);
			m_kFabaoActors[lNewLogicID] = modl;
		}	
	}
}

void IM_Manager::ModifyAnQiModeID( object_id_type lOldLogicID, object_id_type lNewLogicID )
{
	if ( lOldLogicID != lNewLogicID )
	{
		ActorIdPtrMap::iterator itrActor = m_kAnqiActors.find(lOldLogicID);

		if (itrActor != m_kAnqiActors.end())
		{
			GD_ClientActorModel* modl = itrActor->second;
			m_kAnqiActors.erase(itrActor);
			m_kAnqiActors[lNewLogicID] = modl;
		}	
	}
}

void IM_Manager::_event_InputMouseMove(MG_MessageBase& rkMessage)
{
	if(!m_bPickSwitch)
		return;

	if (TEST_MESSAGE_HANDLED(rkMessage))
    {
        return;
    }

	T_PROFILE("IM_Manager::InputMouseMove");

	long lIMLogicID = INVALID_ACTOR_ID;

	lIMLogicID = _pickNpcByInteractiveBtnArea(rkMessage.m_lParam1, rkMessage.m_lParam2, BS_IMInterface::PM_ALL);
	if (IS_PLAYER_CLASS_ID(lIMLogicID) || IS_NPC_ID(lIMLogicID))
	{
		SendMessage( CreateMsg_IM_MOUSE_OVER_INTERACTIVE_BUTTON(lIMLogicID, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
		return;
	}

	lIMLogicID = _pickPlaceHolderOrActorByMouse(rkMessage.m_lParam1, rkMessage.m_lParam2, BS_IMInterface::PM_ALL);
	if (IS_CHEST_ID(lIMLogicID))
	{
		SendMessage(CreateMsg_IM_MOUSE_OVER_CHEST(lIMLogicID, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
	else if (lIMLogicID != INVALID_ACTOR_ID)
	{
		SendMessage(CreateMsg_IM_MOUSE_OVER_ACTOR(lIMLogicID, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}

void IM_Manager::_event_InputLButtonDown(MG_MessageBase& rkMessage)
{
	if(!m_bPickSwitch)
		return;
	
	if (TEST_MESSAGE_HANDLED(rkMessage))
    {
        return;
    }

	long lIMLogicID = INVALID_ACTOR_ID;

	lIMLogicID = _pickNpcByInteractiveBtnArea(rkMessage.m_lParam1, rkMessage.m_lParam2, BS_IMInterface::PM_ALL);
	if (IS_PLAYER_CLASS_ID(lIMLogicID) || IS_NPC_ID(lIMLogicID))
	{
		SendMessage( CreateMsg_IM_LBUTTON_PICK_INTERACTIVE_BUTTON(lIMLogicID, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
		return;
	}

	lIMLogicID = _pickPlaceHolderOrActorByMouse(rkMessage.m_lParam1, rkMessage.m_lParam2, BS_IMInterface::PM_LEFT);
	if (IS_CHEST_ID(lIMLogicID))
	{
		SendMessage(CreateMsg_IM_LBUTTON_PICK_CHEST(lIMLogicID, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
	else if (lIMLogicID != INVALID_ACTOR_ID)
	{
		SendMessage(CreateMsg_IM_LBUTTON_PICK_ACTOR(lIMLogicID, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}

void IM_Manager::_event_InputLButtonDBClk(MG_MessageBase& rkMessage)
{
	if(!m_bPickSwitch)
		return;

	if (TEST_MESSAGE_HANDLED(rkMessage))
	{
		return;
	}

	long lIMLogicID = _pickPlaceHolderOrActorByMouse(rkMessage.m_lParam1, rkMessage.m_lParam2, BS_IMInterface::PM_LEFT);
	if (lIMLogicID != INVALID_ACTOR_ID)
	{
		SendMessage(CreateMsg_IM_LBUTTON_DOUBLE_PICK_ACTOR(lIMLogicID, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}

void IM_Manager::_event_InputRButtonDown(MG_MessageBase& rkMessage)
{
	if(!m_bPickSwitch)
		return;
	
	if (TEST_MESSAGE_HANDLED(rkMessage))
    {
        return;
    }

    long lIMLogicID = _pickPlaceHolderOrActorByMouse(rkMessage.m_lParam1, rkMessage.m_lParam2, BS_IMInterface::PM_RIGHT);
    if (IS_CHEST_ID(lIMLogicID))
    {
        SendMessage(CreateMsg_IM_RBUTTON_PICK_CHEST(lIMLogicID, rkMessage.m_lParam3));
        MARK_MESSAGE_HANDLED(rkMessage);
    }
    else if (lIMLogicID != INVALID_ACTOR_ID)
    {
        SendMessage(CreateMsg_IM_RBUTTON_PICK_ACTOR(lIMLogicID, rkMessage.m_lParam3));
        MARK_MESSAGE_HANDLED(rkMessage);
    }
}
//--------------------------------------------------------------
void IM_Manager::_event_IMPlayAnimation(MG_MessageBase& rkMessage)
{
	long lID = rkMessage.m_lParam1;
	GD_ClientActorModel* pkModel = GetActor(lID);
	if(!pkModel)
		return;

	//pkModel->PlayAnimation((char*)(rkMessage.m_pbyParam2),(bool)(rkMessage.m_lParam3),rkMessage.m_fParam4);
}
//--------------------------------------------------------------
long IM_Manager::_pickPlaceHolderOrActorByMouse(long lMousePosX, long lMousePosY, BS_IMInterface::PickMask ePickMask)
{
	long lPickedImLogicID;
	lPickedImLogicID = _pickPlaceHolderByNameArea(lMousePosX, lMousePosY, ePickMask);
	if (lPickedImLogicID != INVALID_ACTOR_ID)
	{
		return lPickedImLogicID;
	}

    NiCameraPtr spCurrentCamera = CA_CameraManager::GetSingleton().GetNiCamera();
    NiPoint3 kPickRayOrigin, kPickRayDir;
    bool bRay = spCurrentCamera->WindowPointToRay(lMousePosX, lMousePosY, kPickRayOrigin, kPickRayDir);

	if (bRay)
	{
		// 先对宝箱做 pick，避免人物站在宝箱前导致宝箱无法被选中
		lPickedImLogicID = _pickPlaceHolder(kPickRayOrigin, kPickRayDir, ePickMask);
		if (lPickedImLogicID != INVALID_ACTOR_ID)
		{
			return lPickedImLogicID;
		}
		else
		{
			lPickedImLogicID = _pickActor(kPickRayOrigin, kPickRayDir, ePickMask);
			if (lPickedImLogicID != INVALID_ACTOR_ID)
			{
				return lPickedImLogicID;
			}
		}
	}

    return INVALID_ACTOR_ID;
}

long IM_Manager::_pickPlaceHolderByNameArea(long lMousePosX, long lMousePosY, BS_IMInterface::PickMask ePickMask) const
{
	LC_ClientPlaceHolderManager* pkPlaceHolderManager = CLIENT_GET_PLACEHOLDER_MANAGER();
	ChestIdPtrMap::const_iterator itrPlaceHolder;
	TRAV(m_kPlaceHolders, itrPlaceHolder)
	{
		GD_ClientChestModel* pkPlaceHolderModel = (*itrPlaceHolder).second;
		if (!pkPlaceHolderModel->IsPickable(ePickMask))
		{
			continue;
		}
		long lPickedImLogicID = (*itrPlaceHolder).first;
		LC_ClientPlaceHolder* pkPlaceHolder = (LC_ClientPlaceHolder*)pkPlaceHolderManager->FindDisplayPlaceHolder(lPickedImLogicID, pkPlaceHolderModel->IsClientDisplay());
		if (pkPlaceHolder->IsPointInNameArea(lMousePosX, lMousePosY))
			return lPickedImLogicID;
	}
	return INVALID_ACTOR_ID;
}

long IM_Manager::_pickNetPlayerByInteractiveBtnArea(long lMousePosX, long lMousePosY, Base::BS_IMInterface::PickMask ePickMask) const
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	int iCount = pkNetPlayerManager->GetNetPlayerCount();
	LC_ClientNetPlayer* pkNetPlayer = NULL;
	for (int i = 0; i < iCount; i++)
	{
		pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->GetNetPlayerPtrByIndex(i));
		if (pkNetPlayer->IsPointInInteractiveBtnArea(lMousePosX, lMousePosY))
			return pkNetPlayer->GetID();
	}

	return INVALID_ACTOR_ID;
}

long IM_Manager::_pickNpcByInteractiveBtnArea(long lMousePosX, long lMousePosY, Base::BS_IMInterface::PickMask ePickMask) const
{
	LC_ClientNPCManager* pkNpcMgr = CLIENT_GET_NPC_MANAGER();

	const LC_NPCIDPtrMap& kNpcMap = pkNpcMgr->GetNPCIDMap();
	LC_NPCIDPtrMap::const_iterator iter = kNpcMap.begin();
	
	float fMouseX = (float)lMousePosX;
	float fMouseY = (float)lMousePosY;

	for ( ; iter!=kNpcMap.end(); ++iter)
	{
		LC_ClientNPC* pkNpc = (LC_ClientNPC*)iter->second;
		if (pkNpc->IsPointInInteractiveBtnArea(fMouseX, fMouseY))
			return pkNpc->GetID();
	}

	return INVALID_ACTOR_ID;
}

long IM_Manager::_pickPlaceHolder(const NiPoint3& rkOrigin, const NiPoint3& rkRay, BS_IMInterface::PickMask ePickMask)
{
    ChestIdPtrMap::iterator itrPlaceHolder;
    TRAV(m_kPlaceHolders, itrPlaceHolder)
    {
        GD_ClientChestModel* pkPlaceHolder = (*itrPlaceHolder).second;

        if (!pkPlaceHolder->IsPickable(ePickMask))
        {
            continue;
        }

        m_pkPick->SetTarget(pkPlaceHolder->GetPickRoot());
        bool bPick = m_pkPick->PickObjects(rkOrigin, rkRay);
        m_pkPick->SetTarget(NULL);

        // 宝箱暂不对有遮挡的情况做处理，
        // 如果宝箱遮挡情况严重，需要像角色那样先 pick 所有角色，再取最靠前的
        if (bPick)
        {
            return (*itrPlaceHolder).first;
        }
    }

    return INVALID_ACTOR_ID;
}

long IM_Manager::_pickActor(const NiPoint3& rkOrigin, const NiPoint3& rkRay, BS_IMInterface::PickMask ePickMask)
{
	if (!m_pkPick)
	{
        return INVALID_ACTOR_ID;
	}
	
	m_pkPick->ClearResultsArray();

	GD_ClientActorModel* pkActor = NULL;

    ActorIdPtrMap::iterator itrActor;
	TRAV(m_kActors, itrActor)
	{
		pkActor = (*itrActor).second;
        if (!pkActor->IsPickable(ePickMask))
        {
            continue;
        }

		m_pkPick->SetTarget(pkActor->GetPickRoot());
		m_pkPick->PickObjects(rkOrigin, rkRay, true);
		m_pkPick->SetTarget(NULL);
	}

	TRAV(m_kRiderActors, itrActor)
	{
		pkActor = (*itrActor).second;
		if (!pkActor->IsPickable(ePickMask))
		{
			continue;
		}

		m_pkPick->SetTarget(pkActor->GetPickRoot());
		m_pkPick->PickObjects(rkOrigin, rkRay, true);
		m_pkPick->SetTarget(NULL);
	}
	
	NiPick::Results& rkResult = m_pkPick->GetResults();
	if (rkResult.GetEffectiveSize() == 0)
    {
        return INVALID_ACTOR_ID;
    }
	NiPick::Record* pkPickRecord = rkResult.GetAt(0);
	if (pkPickRecord == NULL)
    {
        return INVALID_ACTOR_ID;
    }
	NiAVObjectPtr spPickedRoot = pkPickRecord->GetAVObject();
	if (spPickedRoot == NULL)
    {
        return INVALID_ACTOR_ID;
    }
	NiAVObjectPtr spParent = spPickedRoot->GetParent();
	if (spParent == NULL || spParent->GetName() != "PICK_ROOT")
    {
        return NULL;
    }
	NiInteger64ExtraDataPtr spExtraData = NiDynamicCast(NiInteger64ExtraData, spParent->GetExtraData("Actor_ID"));
	if (spExtraData == NULL)
    {
        return INVALID_ACTOR_ID;
    }

	return (long)spExtraData->GetValue();
}

void IM_Manager::_debug_SwitchPickRootVisible()
{
	bool bPick = false;

    ActorIdPtrMap::iterator itrActor;
	TRAV(m_kActors, itrActor)
	{
        GD_ClientActorModel* pkActor = itrActor->second;
		NiAVObjectPtr spPickRoot = pkActor->GetPickRoot();
		if(spPickRoot != NULL)
        {
            spPickRoot->SetAppCulled(!spPickRoot->GetAppCulled());
        }
	}

	TRAV(m_kRiderActors, itrActor)
	{
		GD_ClientActorModel* pkActor = itrActor->second;
		NiAVObjectPtr spPickRoot = pkActor->GetPickRoot();
		if(spPickRoot != NULL)
		{
			spPickRoot->SetAppCulled(!spPickRoot->GetAppCulled());
		}
	}

	TRAV(m_kFabaoActors, itrActor)
	{
		GD_ClientActorModel* pkActor = itrActor->second;
		NiAVObjectPtr spPickRoot = pkActor->GetPickRoot();
		if(spPickRoot != NULL)
		{
			spPickRoot->SetAppCulled(!spPickRoot->GetAppCulled());
		}
	}

	TRAV(m_kAnqiActors, itrActor)
	{
		GD_ClientActorModel* pkActor = itrActor->second;
		NiAVObjectPtr spPickRoot = pkActor->GetPickRoot();
		if(spPickRoot != NULL)
		{
			spPickRoot->SetAppCulled(!spPickRoot->GetAppCulled());
		}
	}
}