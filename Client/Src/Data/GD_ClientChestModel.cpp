#include "ClientPCH.h"
#include "GD_ClientChestModel.h"
#include "CR_UICommon.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientPlaceHolderManager.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientNetPlayerManager.h"
#include "GlobalSettings.h"

#include "NW_Helper.h"
#include "NW_Game2ClientSession.h"
#include "LC_PlayerFlopManager.h"

#include "UE_UiEffectManager.h"

#include "CF_ItemList.h"
#include "AS_ClientSoundManager.h"
#include "LC_ClientPlaceHolder.h"

#include "IM_Manager.h"
#include "LC_ClientWorldManager.h"

#define CCM_FLY_SRC_HEIGHT_DELTA 120 //可飞行掉落物落地高度 
#define CCM_FLY_DEST_HEIGHT_DELTA 150 //可飞行掉落物飞向目标点高度偏移

using namespace Data;
using namespace CSVFile;

GD_ClientChestModel::GD_ClientChestModel(void)
: m_bIsCurrUsingMajorAppearence(true)
, m_fAnimLifeTime(-1.0f)
, m_fAnimCurPlayTime(0.0f)
, m_lLogicID(0)
, m_cfTotalAngle(360.0f)
, m_fOverturnTime(0.0f)
, m_fWaitingTime(0.0f)
, m_fAutoPickWaitTime(GetGlobalSetting.PickUpItemTime)
, m_bIsAutoPick(false)
, m_eModelState(CMS_INVALID)
, m_bIsFlyAble(false)
, m_fAnimCurFlyToTime(0.0f)
, m_fAnimFlyToLifeTime(0.0f)
, m_bIsDelayFly(false)
, m_fMaxHeight(0.0f)
, m_fOverturnHeight(0.0f)
, m_fAnimCurFlyHeighTime(0.0f)
, m_fAnimFlyHeighLiftTime(-1.0f)
, m_bDropSoundFlag(true)
, m_bIsClientDisplay(false)
, m_fYRotAngle(0.0f)
, m_fHeight(0.0f)
, m_kCurPos(NiPoint3::ZERO)
, m_fOverturnAngel(0.0f)
, m_fZRotAngle(0.0f)
, m_kDeltaPos(NiPoint3::ZERO)
, m_fGravity(0.0f)
, m_kDefaultDir(NiPoint3::ZERO)
, m_bShowGfxLevel(0)
, m_fLastTime(0.0f)
, m_flyModelType(CMFT_FLY_HEIGH)
{
}

GD_ClientChestModel::~GD_ClientChestModel(void)
{
}

void GD_ClientChestModel::AlternateAppearence(bool bUseMajorAppearence)
{
    if (bUseMajorAppearence == m_bIsCurrUsingMajorAppearence)
    {
        return;
    }

    m_bIsCurrUsingMajorAppearence = bUseMajorAppearence;

    // 交换主材质和第二材质，用于切换
    GD_Material* pkTempMaterial = m_pkBaseMaterial;
    m_pkBaseMaterial = m_pkSecondBaseMaterial;
    m_pkSecondBaseMaterial = pkTempMaterial;

    _applyLoadedTextures();
}

void GD_ClientChestModel::PlayOpenAnimation(void)
{
	PlayAnimation(RES_ANM_CHEST_OPEN);
}

void GD_ClientChestModel::PlayCloseAnimation(void)
{
    PlayAnimation(RES_ANM_CHEST_CLOSE);
}

void GD_ClientChestModel::_doInitAfterLoad(void)
{
    AlternateAppearence(true);

    GD_ChestModel::_doInitAfterLoad();

    //_initSunlightEffectAndShadow();

    PlayAnimation(RES_ANM_CHEST_DROP, true);
	m_fLastTime = GET_PROCESS_TIME;
	
	if (m_fAnimLifeTime <= FLOAT_PRECISION)
	{
		_showName();
	}

	m_eModelState = CMS_WAITING;
}

void GD_ClientChestModel::_doPreRelease(void)
{
    GD_ChestModel::_doPreRelease();
	m_lLogicID = 0;
	m_fAnimCurPlayTime = 0.0f;
	m_fAnimLifeTime = 0.0f;
	m_fMaxHeight = 0.0f;
	m_fAutoPickWaitTime = GetGlobalSetting.PickUpItemTime;
	m_fOverturnHeight = 0.0f;

	m_fWaitingTime = 0.0f;

	_flyParamReset();
}

void GD_ClientChestModel::_doPostUpdate(float fTime)
{
	GD_ChestModel::_doPostUpdate(fTime);
	
	float fDeltaTime = fTime - m_fLastTime;
	m_fLastTime = fTime;

	if( m_eModelState == CMS_INVALID/*m_fAnimCurPlayTime >= m_fAnimLifeTime*/ || m_lLogicID == 0 )
	{
		if (!m_bIsDelayFly)
		{
			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
			UT_Vec3Int curPos = UT_EngineMath::ConvertNiPoint3ToVec3Int(m_kCurPos);
			UT_Vec3Int tgtPos = pkGamePlayer->GetVisibleLocation();
			float dis = (tgtPos - curPos).Length();
			if (dis < 2000.0f)
			{
				_showName();
			}
			else
			{
				_hideName();
			}
		}
		return;
	}

	if (m_eModelState == CMS_WAITING)
	{
		m_fWaitingTime -= fDeltaTime;
		if (m_fWaitingTime <= 0)
		{
			m_eModelState = CMS_DROPING;
			if (!IsVisible())
				SetVisibility(true);
		}
	}
	else if ( m_eModelState == CMS_DROPING)
	{
		m_fAnimCurPlayTime += fDeltaTime;
		
		float fCurrentHeight = m_fMaxHeight - 0.5f * m_fGravity * ((m_fAnimCurPlayTime - m_fAnimLifeTime*0.5f) * (m_fAnimCurPlayTime - m_fAnimLifeTime*0.5f));
		NiMatrix3 kRotate;
//ToCheck: 物品掉落旋转取消
		//if (fCurrentHeight >= m_fOverturnHeight)
		//{
		//	m_fOverturnTime += fDeltaTime;
		//	//旋转
		//	m_fYRotAngle = m_fOverturnAngel * m_fOverturnTime;
		//	kRotate.MakeRotation(NI_PI * m_fYRotAngle / 180.0f, m_kDefaultDir);
		//	SetRotate(kRotate);
		//}
		
		//平移
		m_kCurPos = m_kSrcPos + m_kDeltaPos * m_fAnimCurPlayTime;
		
		m_kCurPos.z += fCurrentHeight;
		SetTranslate(m_kCurPos);

		LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
		GameLogic::LC_PlaceHolderBase* pChest = pkChestManager->FindDisplayPlaceHolder(m_lLogicID, IsClientDisplay());
		if (pChest)
			pChest->SetCurrentLocation(UT_EngineMath::ConvertNiPoint3ToVec3Int(m_kCurPos));

		/*LC_ClientPlaceHolder* pkCModel = (LC_ClientPlaceHolder*)pChest;
		if(m_bIsFlyAble && pkCModel !=NULL && pkCModel->GetReward().m_eType == LC_Reward::RT_BOSS_JP)
		{
			m_eModelState = CMS_INVALID;
			return;
		}*/

		if (m_fAnimCurPlayTime >= m_fAnimLifeTime)
		{
			if (m_bIsFlyAble)
				SetTranslate(NiPoint3(m_kDestPos.x, m_kDestPos.y, m_kDestPos.z + CCM_FLY_SRC_HEIGHT_DELTA));
			else
				SetTranslate(m_kDestPos);

			if (pChest && !m_bIsFlyAble)
				pChest->SetVisibleLocation(UT_EngineMath::ConvertNiPoint3ToVec3Int(m_kDestPos));
			else if (pChest && m_bIsFlyAble)
				pChest->SetVisibleLocation(UT_EngineMath::ConvertNiPoint3ToVec3Int(NiPoint3(m_kDestPos.x, m_kDestPos.y, m_kDestPos.z + CCM_FLY_SRC_HEIGHT_DELTA)));

			kRotate.MakeRotation(2.0f*NI_PI, m_kDefaultDir);
			SetRotate(kRotate);

			switch(m_bShowGfxLevel)
			{
			case 2:
				AttachFx(Data::GD_ModelBase::FDT_GROUND, FileSystem::MTF_ModelData::AFBM_TRANSLATE, false, RES_FX_DROPITEM_GREEN_BEAM, NULL, NULL, 1.0f, true, NiColor::WHITE, true, true);
				break;
			case 3:
				AttachFx(Data::GD_ModelBase::FDT_GROUND, FileSystem::MTF_ModelData::AFBM_TRANSLATE, false, RES_FX_DROPITEM_BLUE_BEAM, NULL, NULL, 1.0f, true, NiColor::WHITE, true, true);
				break;
			case 4:
				AttachFx(Data::GD_ModelBase::FDT_GROUND, FileSystem::MTF_ModelData::AFBM_TRANSLATE, false, RES_FX_DROPITEM_PURPLE_BEAM, NULL, NULL, 1.0f, true, NiColor::WHITE, true, true);
				break;
			case 5:
				AttachFx(Data::GD_ModelBase::FDT_GROUND, FileSystem::MTF_ModelData::AFBM_TRANSLATE, false, RES_FX_DROPITEM_BEAM, NULL, NULL, 1.0f, true, NiColor::WHITE, true, true);
				break;
			case 6:;
				AttachFx(Data::GD_ModelBase::FDT_GROUND, FileSystem::MTF_ModelData::AFBM_TRANSLATE, false, RES_FX_DROPITEM_RAINBOW_BEAM, NULL, NULL, 1.0f, true, NiColor::WHITE, true, true);
				break;
			default:
				break;
			}
			
			//CSVFile::CF_ItemList::DataEntry* pkData =  SafeGetCSVFileDataEntryPtr<CF_ItemList>(pkCModel->GetItemTypeID());
			if (/*pkData && */m_bDropSoundFlag)
			{
				//现在所有掉落物播放同一个音效
				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemAppear_Misc));

				//增加凋落物音效
				//switch (pkData->_lCategory2)
				//{
				//case ITEM_CATEGORY_MACHETE:			//刀
				//case ITEM_CATEGORY_SWORD:			//剑
				//case ITEM_CATEGORY_TWINBLADES:		//双刀
				//case ITEM_CATEGORY_CLAYMORE:		//大剑
				//case ITEM_CATEGORY_STICK:			//棍
				//case ITEM_CATEGORY_SPEAR:			//枪
				//	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemAppear_Weapon));
				//	break;
				//case ITEM_CATEGORY_SHOE:			//鞋子
				//case ITEM_CATEGORY_HELM:			//帽子
				//case ITEM_CATEGORY_CLOTH:			//衣服
				//	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemAppear_Armor));
				//	break;
				//case ITEM_CATEGORY_RING:			//戒指
				//case ITEM_CATEGORY_NECKLACE:		//项链
				//case ITEM_CATEGORY_AMULET:			//护身符
				//	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemAppear_Acces));
				//	break;
				//case ITEM_CATEGORY_DRUG:			//药瓶
				//	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemAppear_Drug));
				//	break;
				//case ITEM_CATEGORY_BOOK:			//书
				//	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemAppear_Misc));
				//	break;
				//case ITEM_CATEGORY_STONE:			//石头
				//	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemAppear_Diamond));
				//	break;
				//default:
				//	break;
				//}
			}

			_showName();
			if (m_bIsFlyAble)
			{
				m_eModelState = CMS_INVALID;
				//如果设置了掉落后立即飞行，则飞行
				if (m_bIsDelayFly)
				{
					UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
					pkUIEffectMgr->DeletePlaceHolderName(m_lLogicID);
					switch (m_flyModelType)
					{
					case CMFT_FLY_HEIGH:
						ResetFlyHeigh();
						break;
					case CMS_FLY_RANDOM:
						ResetFlyToRandom();
						break;
					case CMFT_FLY_LINE:
						ResetFlyTo();
						break;
					default:
						ResetFlyHeigh();
						break;
					}
				}
			}
			else
			{
				m_eModelState = CMS_INVALID;
			}
		}
	}
	else if (m_eModelState == CMS_FLYHEIGH)
	{
		_updateFlyHeighAnim(fDeltaTime);
	}
	else if (m_eModelState == CMS_FLYTO )
	{
 		_updateFlyToAnim(fDeltaTime);
	}
	else if (m_eModelState == CMS_FLYRANDOM)
	{
		_updateFlyRandomAnim(fDeltaTime);
	}
}

void GD_ClientChestModel::_updateFlyHeighAnim(float fDeltaTime)
{
	if (!IsVisible())
		SetVisibility(true);

	m_fAnimCurFlyHeighTime += fDeltaTime;

	if (m_fAnimCurFlyHeighTime >= m_fAnimFlyHeighLiftTime+0.2f)
	{
		ResetFlyTo();
	}
	else if ( m_fAnimCurFlyHeighTime >= m_fAnimFlyHeighLiftTime)
	{

	}
	else
	{
		NiPoint3 dirUnit = m_stFlyHeighParam.fV0Dir;
		NiPoint3 a =  m_stFlyHeighParam.target;
		float dis = a.Unitize();
		dirUnit.Unitize();
		float velValue = m_stFlyHeighParam.fV0Dir.Length();
		m_stFlyHeighParam.fV0Dir = m_stFlyHeighParam.fV0Dir - dirUnit*(fDeltaTime*(m_stFlyHeighParam.fDump1*velValue*velValue+m_stFlyHeighParam.fDump2*velValue));
		a = a*(m_stFlyHeighParam.fAcce* (m_stFlyHeighParam.fDischan == 0.0f ? 1.0f:dis*m_stFlyHeighParam.fDischan));
		m_stFlyHeighParam.fV0Dir = m_stFlyHeighParam.fV0Dir + a*fDeltaTime;

		m_kCurPos = m_kCurPos + m_stFlyHeighParam.fV0Dir*fDeltaTime*1.5f;
		SetTranslate(m_kCurPos);

		LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
		GameLogic::LC_PlaceHolderBase* pChest = pkChestManager->FindDisplayPlaceHolder(m_lLogicID, IsClientDisplay());
		if (pChest)
			pChest->SetCurrentLocation(UT_EngineMath::ConvertNiPoint3ToVec3Int(m_kCurPos));

		NiPoint3 rotVel = m_stFlyHeighParam.fV0Dir;
		rotVel.Unitize();
		NiPoint3 kAxis(rotVel.z, 0.0f, -rotVel.x);
		kAxis.Unitize();
		NiMatrix3 kRotation;
		kRotation.MakeRotation(acos(-rotVel.y), kAxis);
		SetRotate(kRotation);
	}
}

void GD_ClientChestModel::_updateFlyRandomAnim(float fDeltaTime)
{
	if (!IsVisible())
		SetVisibility(true);

	m_fAnimCurFlyHeighTime += fDeltaTime;

	if (m_fAnimCurFlyHeighTime >= m_fAnimFlyHeighLiftTime)
	{
		ResetFlyTo1();
	}
	/*else if ( m_fAnimCurFlyHeighTime >= m_fAnimFlyHeighLiftTime)
	{

	}*/
	else
	{ 
		NiPoint3 dirUnit = m_stFlyHeighParam.fV0Dir;
		NiPoint3 a =  m_stFlyHeighParam.target;
		float dis = a.Unitize();
		dirUnit.Unitize();
		float velValue = m_stFlyHeighParam.fV0Dir.Length();
		m_stFlyHeighParam.fV0Dir = m_stFlyHeighParam.fV0Dir - dirUnit*(fDeltaTime*(m_stFlyHeighParam.fDump1*velValue*velValue+m_stFlyHeighParam.fDump2*velValue));
		a = a*(m_stFlyHeighParam.fAcce* (m_stFlyHeighParam.fDischan == 0.0f ? 1.0f:dis*m_stFlyHeighParam.fDischan));
		m_stFlyHeighParam.fV0Dir = m_stFlyHeighParam.fV0Dir + a*fDeltaTime;

		m_kCurPos = m_kCurPos + m_stFlyHeighParam.fV0Dir*fDeltaTime;
		SetTranslate(m_kCurPos);

		LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
		GameLogic::LC_PlaceHolderBase* pChest = pkChestManager->FindDisplayPlaceHolder(m_lLogicID, IsClientDisplay());
		if (pChest)
			pChest->SetCurrentLocation(UT_EngineMath::ConvertNiPoint3ToVec3Int(m_kCurPos));

		NiPoint3 rotVel = m_stFlyHeighParam.fV0Dir;
		rotVel.Unitize();
		NiPoint3 kAxis(rotVel.z, 0.0f, -rotVel.x);
		kAxis.Unitize();
		NiMatrix3 kRotation;
		kRotation.MakeRotation(acos(-rotVel.y), kAxis);
		SetRotate(kRotation);
	}
}

void GD_ClientChestModel::_updateFlyToAnim(float fDeltaTime)
{
	if (!IsVisible())
		SetVisibility(true);

	//从掉落位置飞向人

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	LC_PlayerBase* pkPlayer = NULL;
	if (m_stFlyParam.iFlyToID != pkGamePlayer->GetMainControllerID() && m_stFlyParam.iFlyToID != 0)
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkPlayer = pkNetPlayerManager->FindPlayer(m_stFlyParam.iFlyToID);
	}
	else
	{
		pkPlayer = pkGamePlayer;
	}

	if (pkPlayer == NULL)
	{
		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UE_REMOVE_DISPLAY_CHEST(m_lLogicID, (int)IsClientDisplay()));
		return ;
	}

	m_fAnimCurFlyToTime += fDeltaTime;
	UT_Vec3Int tgtPos = pkPlayer->GetVisibleLocation();
	tgtPos.z += CCM_FLY_DEST_HEIGHT_DELTA;
	NiPoint3 destPos = UT_EngineMath::ConvertVec3IntToNiPoint3(tgtPos);

	UT_Vec3Int curPos = UT_EngineMath::ConvertNiPoint3ToVec3Int(m_kCurPos);
	UT_Vec3Int gap = tgtPos - curPos;
	//gap.z = 0;
	float fGapLength = gap.Length();
	if (fGapLength < 60.0f)
	{
		/*int iMapResID = pkGamePlayer->GetMapResID();
		if(pkGamePlayer->GetIsInParalle() && pkGamePlayer->GetRaidScore(iMapResID) > 0)
		{
			CF_WorldMapList::DataEntry *pkMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
			if(pkMapData != NULL && pkMapData->_iShowRaidStage == 1)
			{
				pkGamePlayer->RequestLeaveTranscription();
				pkGamePlayer->SetIsInParalle(false);
				return;
			}
		}*/
		

		if (IsClientDisplay() || (m_stFlyParam.iFlyToID != pkGamePlayer->GetMainControllerID() && m_stFlyParam.iFlyToID != 0))
		{
			MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UE_REMOVE_DISPLAY_CHEST(m_lLogicID, (int)IsClientDisplay()));
		}
		else
		{
			Protocol::MG_REQ_PickUpPlaceHolder reqMsg;
			Protocol::MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = false;

			reqMsg.m_nCheckCD = false;
			reqMsg.m_lIDs.push_back(m_lLogicID);
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHEST_PICKUP, &reqMsg);
			if(bCreateMsg)
			{
				pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
			g_soundMgr.PlayAutoEx(VARIABLE2STRING(SE_Absorb), m_kCurPos, AS_ClientSoundManager::SCT_PLAYER_SKILL_EVENT);
			//真气球吸入特效
			IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
			if (pkInteractManager)
			{	
				GD_ClientActorModel* pkVisibleData = pkInteractManager->GetActor(pkGamePlayer->GetMainControllerID());
				if (pkVisibleData != NULL)
				{
					pkVisibleData->AttachFx(GD_ModelBase::FDT_HEART,0,false,RES_FX_DROPITEM_SP_HAVING, NULL, NULL);	//胸口飙血
				}
			}
		}
	}
	else
	{
		NiPoint3 a =  destPos - m_kCurPos;
		float dis = a.Unitize();
		NiPoint3 dirUnit = m_stFlyParam.fV0Dir;
		dirUnit.Unitize();
		float velValue = m_stFlyParam.fV0Dir.Length();
		m_stFlyParam.fV0Dir = m_stFlyParam.fV0Dir - dirUnit*(fDeltaTime*(m_stFlyParam.fDump1*velValue*velValue+m_stFlyParam.fDump2*velValue));
		a = a*(m_stFlyParam.fAcce* (m_stFlyParam.fDischan == 0.0f ? 1.0f:dis*m_stFlyParam.fDischan));
		m_stFlyParam.fV0Dir = m_stFlyParam.fV0Dir + a*fDeltaTime;
		
		m_kCurPos = m_kCurPos + m_stFlyParam.fV0Dir*fDeltaTime*1.5f;
		SetTranslate(m_kCurPos);

		LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
		GameLogic::LC_PlaceHolderBase* pChest = pkChestManager->FindDisplayPlaceHolder(m_lLogicID, IsClientDisplay());
		if (pChest)
			pChest->SetCurrentLocation(UT_EngineMath::ConvertNiPoint3ToVec3Int(m_kCurPos));

		NiPoint3 rotVel = m_stFlyParam.fV0Dir;
		rotVel.Unitize();
		NiPoint3 kAxis(rotVel.z, 0.0f, -rotVel.x);
		kAxis.Unitize();
		NiMatrix3 kRotation;
		kRotation.MakeRotation(acos(-rotVel.y), kAxis);
		SetRotate(kRotation);
	}
}

void GD_ClientChestModel::_flyParamReset()
{
	//飞行动画相关变量重置
	m_fAnimCurFlyToTime = 0.0f;
	m_fAnimFlyToLifeTime = 0.0f;
	m_bIsFlyAble = false;
	m_bIsDelayFly = false;

	m_stFlyParam.reset();

	m_eModelState = CMS_INVALID;

	m_fAnimCurFlyHeighTime = 0.0f;
	m_fAnimFlyHeighLiftTime = 0.0f;

	m_stFlyHeighParam.reset();
}

void GD_ClientChestModel::_showName()
{
	bool isOwner = true;
	LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
	if ( pkChestManager )
	{
		LC_ClientPlaceHolder* phb = (LC_ClientPlaceHolder*)pkChestManager->FindPlaceHolder(m_lLogicID);
		if ( phb )
		{
			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
			if ( pkGamePlayer )
				isOwner = phb->IsOwner(pkGamePlayer->GetMainControllerID());
		}
	}

	if ( isOwner )
		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UE_SHOW_DROPITEM_NAME(m_lLogicID, 0, 1) );
	else
		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UE_SHOW_DROPITEM_NAME(m_lLogicID, 0, 0) );
}

void GD_ClientChestModel::_hideName()
{
	MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UE_HIDE_DROPITEM_NAME(m_lLogicID) );
}

void GD_ClientChestModel::ResetFlyTo()
{
	m_fAnimCurFlyToTime = 0.0f;
	m_eModelState = CMS_FLYTO;
	m_stFlyParam.fV0Dir = NiPoint3::ZERO;
}

void GD_ClientChestModel::ResetFlyTo1()
{
	m_fAnimCurFlyToTime = 0.0f;
	m_eModelState = CMS_FLYTO;
	m_stFlyParam.fV0Dir = m_stFlyHeighParam.fV0Dir;
	m_stFlyParam.fV0Dir.Unitize();
	m_stFlyParam.fV0Dir = m_stFlyParam.fV0 * m_stFlyParam.fV0Dir;
}

void GD_ClientChestModel::ResetFlyHeigh()
{
	m_fAnimCurFlyHeighTime = 0.0f;
	m_kCurPos = m_kDestPos;
	m_kCurPos.z += CCM_FLY_SRC_HEIGHT_DELTA;
	m_eModelState = CMS_FLYHEIGH;

	m_stFlyHeighParam.fV0Dir = NiPoint3::ZERO;
	m_stFlyHeighParam.fV0Dir.z = m_stFlyHeighParam.fV0;
	m_stFlyHeighParam.target = NiPoint3::UNIT_Z;
	m_stFlyHeighParam.target.z = m_stFlyHeighParam.fHeigh;
	float angle = UT_MathBase::PI2*UT_MathBase::RandFloat01();
	m_stFlyHeighParam.target.x = m_stFlyHeighParam.fRadius*UT_MathBase::Cos(angle);
	m_stFlyHeighParam.target.y = m_stFlyHeighParam.fRadius*UT_MathBase::Sin(angle);
}

void GD_ClientChestModel::ResetFlyToRandom()
{
	m_fAnimCurFlyHeighTime = 0.0f;
	m_kCurPos = m_kDestPos;
	m_kCurPos.z += + CCM_FLY_SRC_HEIGHT_DELTA;
	m_eModelState = CMS_FLYRANDOM;

	m_stFlyHeighParam.fV0Dir = NiPoint3::ZERO;
	m_stFlyHeighParam.fV0Dir.z = m_stFlyHeighParam.fV0;
	float agT = UT_MathBase::RandFloat01() * NI_PI * 2;
	float agW = UT_MathBase::RandFloat01() * NI_PI * 2;

	m_stFlyHeighParam.target.x = m_stFlyHeighParam.fHeigh * UT_MathBase::Cos(agT);
	m_stFlyHeighParam.target.y = m_stFlyHeighParam.fHeigh * UT_MathBase::Sin(agT);
	m_stFlyHeighParam.target.z = m_stFlyHeighParam.fHeigh * tanf(agW);
}

void GD_ClientChestModel::SetUserValue(long lLogicID, float fTime, const NiPoint3& kSrcPos, const NiPoint3& kDestPos, float fSpawnZRotAngle, int bShowGfxLevel)
{
	m_lLogicID = lLogicID;
	m_bShowGfxLevel = bShowGfxLevel;

	m_fAnimLifeTime = fTime;
	m_kSrcPos = kSrcPos;
	m_kDestPos = kDestPos;

	if (m_fAnimLifeTime <= FLOAT_PRECISION)
	{
		_showName();
		return;
	}
	//设置当前的位置
	m_kCurPos = m_kSrcPos;
	//m_fOverturnAngel = m_cfTotalAngle / m_fAnimLifeTime;

	//单位时间平移变换
	m_kDeltaPos = (kDestPos - kSrcPos) / m_fAnimLifeTime;

	//z轴旋转
	m_fZRotAngle = fSpawnZRotAngle;
	if ((kDestPos - kSrcPos).Length() > FLOAT_PRECISION)
	{
		NiPoint3 kHVector = NiPoint3(kDestPos.x - kSrcPos.x, kDestPos.y - kSrcPos.y, 0);
		NiPoint3 kXNormalVector = NiPoint3(kDestPos.x - kSrcPos.x, 0, 0);
		float fDot = kHVector.Dot(kXNormalVector);
		float fSize = kHVector.SqrLength() * kXNormalVector.SqrLength();
		if (fSize > FLOAT_PRECISION)
		{
			float fZRotAngle= UT_MathBase::ACos(fDot / fSize);
			m_fZRotAngle += fZRotAngle;
		}
	}	
	NiMatrix3 kRotate;
	kRotate.MakeRotation(NI_PI * m_fZRotAngle / 180.0f, NiPoint3::UNIT_Z);
	SetRotate(kRotate);
	m_kDefaultDir = kRotate * NiPoint3::UNIT_Y;

	m_fMaxHeight = g_settings.DS_DROPITEM_MAX_HEIGHT;
	m_fOverturnHeight = g_settings.DS_DROPITEM_OVERTURN_HEIGHT;

	float fScale = UT_MathBase::RandFloat01() * (g_settings.DS_DROPITEM_HEIGHT_SCALE_MAX - g_settings.DS_DROPITEM_HEIGHT_SCALE_MIN) + g_settings.DS_DROPITEM_HEIGHT_SCALE_MIN;
	
	m_fMaxHeight = m_fMaxHeight * fScale;
	m_fOverturnHeight = m_fOverturnHeight * fScale;

	m_fGravity = m_fMaxHeight * 8.0f / (m_fAnimLifeTime * m_fAnimLifeTime);
	if (m_fMaxHeight - m_fOverturnHeight <= FLOAT_PRECISION)
		m_fOverturnAngel = m_cfTotalAngle;
	else
		m_fOverturnAngel = m_cfTotalAngle * 0.5f * NiSqrt(m_fGravity * 0.5f / (m_fMaxHeight - m_fOverturnHeight));
}

void GD_ClientChestModel::UpdateHeight(const NiPoint3& kSrcPos, const NiPoint3& kDestPos)
{
	m_kSrcPos = kSrcPos;
	m_kDestPos = kDestPos;
	if (m_fAnimLifeTime > 0)
		m_kDeltaPos = (kDestPos - kSrcPos) / m_fAnimLifeTime;
	if (m_fAnimLifeTime <= 0 || m_lLogicID == 0)
		SetTranslate(m_kSrcPos);
	else if (m_fAnimCurPlayTime >= m_fAnimLifeTime)
		SetTranslate(m_kDestPos);
}

void GD_ClientChestModel::FlyToPick()
{
	//如果当前是掉落状态，则设置为掉落后立即飞行
	//if (m_eModelState == CMS_DROPING)
	//{
		m_bIsDelayFly = true;
	//}
	/*else */if (m_eModelState != CMS_FLYTO && m_eModelState != CMS_FLYHEIGH)
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		pkUIEffectMgr->DeletePlaceHolderName(m_lLogicID);
		m_flyModelType = CMFT_FLY_HEIGH;
		ResetFlyHeigh();
	}
}

void GD_ClientChestModel::QuickFlyToPick()
{
	//如果当前是掉落状态，则设置为掉落后立即飞行
	//if (m_eModelState == CMS_DROPING)
	//{
	m_bIsDelayFly = true;
	//}
	/*else */if (m_eModelState != CMS_FLYTO && m_eModelState != CMS_FLYHEIGH)
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		pkUIEffectMgr->DeletePlaceHolderName(m_lLogicID);
		m_flyModelType = CMFT_FLY_LINE;
		ResetFlyTo();
	}
}

void GD_ClientChestModel::FlyToRandom()
{
	m_bIsDelayFly = true;
	if (m_eModelState != CMS_FLYTO && m_eModelState != CMS_FLYHEIGH)
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		pkUIEffectMgr->DeletePlaceHolderName(m_lLogicID);
		m_flyModelType = CMS_FLY_RANDOM;
		ResetFlyToRandom();
	}
}

void GD_ClientChestModel::SetWaitingTime(float fTime)
{
	m_fWaitingTime = fTime;
	SetVisibility(m_fWaitingTime <= 0.0f);

	m_eModelState = CMS_WAITING;
}

void GD_ClientChestModel::ReUse()
{
	GD_ChestModel::ReUse();

	m_eModelState = CMS_INVALID;
	m_flyModelType = CMFT_FLY_HEIGH;
	SetRotate(NiMatrix3::IDENTITY);
	m_bIsClientDisplay = false;
	m_bIsAutoPick = false;

	m_bIsCurrUsingMajorAppearence = true;
	m_fLastTime = 0.0f;

	m_fAnimLifeTime = 0.0f;
	m_kSrcPos = NiPoint3::ZERO;
	m_kDestPos = NiPoint3::ZERO;

	m_fOverturnAngel = 0.0f;
	m_fZRotAngle = 0.0f;
	m_kDeltaPos = NiPoint3::ZERO;
	m_fGravity = 0.0f;
	m_kDefaultDir = NiPoint3::ZERO;

	m_fYRotAngle = 0.0f;
	m_fHeight = 0.0f;
	m_kCurPos = NiPoint3::ZERO;
	m_fAnimCurPlayTime = 0.0f;
	m_fOverturnTime = 0.0f;

	m_fMaxHeight = 0.0f;
	m_fOverturnHeight = 0.0;

	m_fWaitingTime = 0.0f;

	m_lLogicID = 0;
	m_bDropSoundFlag = false;

	_flyParamReset();
}