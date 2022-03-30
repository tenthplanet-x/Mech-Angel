#include "ClientPCH.h"
#include "GD_MaterialManager.h"
#include "GD_ModelContent.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientLogicManager.h"
#include "UE_PlayerBlood.h"
#include "UE_RenderManager.h"
#include "UT_CutSceneManager.h"
#include "IG_Material.h"
#include "CF_ActorFilterFunc.h"
#include "LC_ClientArenaPlayBackManager.h"

#include "IM_Manager.h"

using namespace Data;
using namespace CSVFile;
using namespace Utility;
using namespace GameLogic;

//------------------------------------------------------------------------------
UE_PlayerBlood::UE_PlayerBlood(UI_EFFECT_TYPE eType)
: UE_UiEffectBase(eType)
, m_bMouseType(false)
{
}
//------------------------------------------------------------------------------
UE_PlayerBlood::~UE_PlayerBlood()
{

}
//------------------------------------------------------------------------------
bool UE_PlayerBlood::Init()
{
	return true;
}
//------------------------------------------------------------------------------
void UE_PlayerBlood::SetPendingParam(long lParam1, long /*lParam2*/)
{
	m_lTargetID = lParam1;
	Show();
}
//------------------------------------------------------------------------------
void UE_PlayerBlood::Show()
{	
	_findActorHeight();
}
//------------------------------------------------------------------------------
void UE_PlayerBlood::Hide()
{
	m_lTargetID = INVALID_ACTOR_ID;
}
//------------------------------------------------------------------------------
bool UE_PlayerBlood::_updateInfo()
{
	if(!m_bVisible || !IS_PLAYER_CLASS_ID(m_lTargetID))
		return false ;

	//_findActorHeight();

	UE_RenderManager* pUERender  = UE_RenderManager::GetSingletonPtr();
	T_ASSERT(pUERender);
	LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
	T_ASSERT(pkPlayerManager);
	bool bShowHp = true;
	bool bShowMp = true;
	_findActorHeight();
	bool bCanAttack = false;
	LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(m_lTargetID);
	attr_value_type lMaxHP = 0;
	if(!pkPlayer)
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkPlayer = pkNetPlayerManager->FindPlayer(m_lTargetID);
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)pkPlayer;
		if(pkPlayer)
		{
			CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
			CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
			LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
			LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
			bCanAttack = (LC_Helper::IsValidTarget(pkGamePlayer,m_lTargetID,pkData) == RE_SKILL_CHECK_VALID_TARGET);
			if ( bCanAttack )
			{
				bShowHp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_ENEMY_HP );
				bShowMp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_ENEMY_MP );
			}
			else if ( pkNetPlayer->GetOwnerObjectID() == pkGamePlayer->GetMainControllerID() )
			{
				bShowHp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_PLAYER_HP );
				bShowMp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_PLAYER_MP );
			}
			else
			{
				bShowHp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_FRIEND_HP );
				bShowMp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_FRIEND_MP );
			}
		}
		
		lMaxHP = pkNetPlayer->GetAttributeMap()->GetAttribute(ATT_MAX_HP);
	}
	else
	{
		bShowHp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_PLAYER_HP );
		bShowMp = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_PLAYER_MP );
		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;
		lMaxHP = pkGamePlayer->GetBuddyAttributeMap(pkGamePlayer->GetCharType())->GetAttribute(ATT_MAX_HP);
	}

	if (!bShowHp)
	{
		bShowHp = GetMouseType();
	}

	if (pkPlayer && !pkPlayer->GetDead())
	{
		
		attr_value_type lCurHP = pkPlayer->GetHP();
		if(lMaxHP == 0)
			return false;

		if(lCurHP == 0)
			return false;

		//控制hppercent 边界
		float fHPPercent = UT_MathBase::ClampFloat((float)lCurHP / (float)lMaxHP, 0.0f, 1.0f);

		//attr_value_type lCurMP = (attr_value_type)pkPlayer->GetMP();
		//attr_value_type lMaxMP = pkPlayer->GetAttributeMap()->GetAttribute(ATT_MAX_MP);
//origin
		/*if(lMaxMP == 0)
			return false;

		float fMPPercent = (float)lCurMP / (float)lMaxMP;
		if (fMPPercent < 0.0f) fMPPercent = 0.0f;	*/	
//new
		float fMPPercent = 0.0f;
//--new
		
		UT_Vec3Int kPosition = pkPlayer->GetVisibleLocation();
		/*if ( IS_PLAYER_CLASS_ID( pkPlayer->GetID()) 
			&& (StateMachine::IS_PLAYER_JUMP_STATE(pkPlayer->GetCurrentStateType()))
			&& (pkPlayer->GetRiderCharType() != 0) )
		{
			kPosition.z  += UE_PLAY_NAME_BOLLD_CORRECT;
		}*/
		if ( pkPlayer->GetStateExtendData(GOD_STATE_ID, GOD_STATE_ITEMTYPE) == GOD_ITEM_4 )
		{
			kPosition.z += 40;
		}
		kPosition.z += m_lActorHeight;
		NiPoint3 kMyPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition);

		if(!UT_CutSceneManager::GetSingleton().ShowPlayerBlood())
			return true;
		if ( !bShowMp && !bShowHp )
		{
			return true;
		}
		if ( !bShowHp )
		{
			fHPPercent = -1;
		}
		if ( !bShowMp )
		{
			fMPPercent = -1;
		}
		
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		if (!pkGamePlayer)
			return true;
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();

		attr_value_type lOldHP = pkPlayer->GetOldHP();
		float fCurrentTime = GET_PROCESS_TIME;
		float fChangeTimeHP = pkPlayer->GetChangeTimeHP();
		float HpLightPercent = 0.0f;
		if(fCurrentTime - fChangeTimeHP < 0.5f)
		{
			HpLightPercent = (lCurHP + (float)(lOldHP - lCurHP)*(0.5f-fCurrentTime + fChangeTimeHP)/0.5f) / (float)lMaxHP;
		}
		
		//buddy血条跟主战角色一致
		if(pkGamePlayer == pkPlayer || pkGamePlayer->IsBuddy(pkPlayer->GetID()))
		{
			int level;
			if (pkGamePlayer == pkPlayer)
			{
				level = pkGamePlayer->GetBuddyInstance(pkGamePlayer->GetCharType())->GetLevel();
			}
			else if (pkGamePlayer->IsBuddy(pkPlayer->GetID()))
			{
				level = pkGamePlayer->GetBuddyInstance(pkPlayer->GetCharType())->GetLevel();
			}
			else
			{
				level = pkPlayer->GetLevel();
			}
			int id = pkPlayer->GetCharType();
			bool isActivate = pkGamePlayer->GetSkillActivateAll(id);
			mem::vector<int> runelevel = pkGamePlayer->GetBuddyRuneLevel(id);
			mem::vector<int> equipGrades = pkGamePlayer->GetBuddyEquipGrades(id);

			pUERender->AddPlayerBloodInfo(level, kMyPos, fHPPercent, HpLightPercent, BloodType_GREEN_Big_deep, 
				pkPlayer->GetDamageElementType(), true, isActivate,pkGamePlayer->GetBuddyStarLevel(id),runelevel,
				pkGamePlayer->GetRuneBuddySlotOpen(),equipGrades,pkGamePlayer->GetActiveEquipSlot(),pkGamePlayer->GetBuddyAllEquipReinforceLevel(id));
		}
		else if (pkGamePlayer->IsPlayBackFlag())
		{
			BloodType bloodType = BloodType_RED_Big_deep;
			if (pkArenaPlayBackManager->CheckPlayerSideByID(m_lTargetID))
				bloodType = BloodType_GREEN_Big_deep;

			pUERender->AddPlayerBloodInfo(pkPlayer->GetLevel(), kMyPos, fHPPercent, HpLightPercent, bloodType, 
				pkPlayer->GetDamageElementType(), true, pkPlayer->GetSkillActivateAll(),pkPlayer->GetBuddyStarLevel(),pkPlayer->GetRuneStarLevel(),
				pkPlayer->GetActiveRuneSlot(),pkPlayer->GetEquipItemIDs(),pkPlayer->GetActiveEquipSlot(),pkPlayer->GetAllEquipReinforceLevel());
		}
		else if(bCanAttack)
		{
			pUERender->AddPlayerBloodInfo(pkPlayer->GetLevel(), kMyPos, fHPPercent, HpLightPercent, BloodType_RED_Big_deep, 
				pkPlayer->GetDamageElementType(), true, pkPlayer->GetSkillActivateAll(),pkPlayer->GetBuddyStarLevel(),pkPlayer->GetRuneStarLevel(),
				pkPlayer->GetActiveRuneSlot(),pkPlayer->GetEquipItemIDs(),pkPlayer->GetActiveEquipSlot(),pkPlayer->GetAllEquipReinforceLevel());
		}
		else
		{
			pUERender->AddPlayerBloodInfo(pkPlayer->GetLevel(), kMyPos, fHPPercent, HpLightPercent, BloodType_BLUE_Big_deep,
				pkPlayer->GetDamageElementType(), true, pkPlayer->GetSkillActivateAll(),pkPlayer->GetBuddyStarLevel(),
				pkPlayer->GetRuneStarLevel(),pkPlayer->GetActiveRuneSlot(),pkPlayer->GetEquipItemIDs(),pkPlayer->GetActiveEquipSlot(),pkPlayer->GetAllEquipReinforceLevel());
		}

		return true;
	}

	return false;
}

bool UE_PlayerBlood::Update( float fCurrentTime )
{
	bool bRes = UE_UiEffectBase::Update(fCurrentTime);
	if (bRes && _updateInfo() )
	{
		m_fStartShowTime = fCurrentTime;
	}	
	return bRes;
}
//------------------------------------------------------------------------------