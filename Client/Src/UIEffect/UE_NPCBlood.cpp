#include "ClientPCH.h"

#include "UE_NPCBlood.h"
#include "LC_ClientNPC.h"
#include "LC_ClientNPCManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "UE_RenderManager.h"
#include "UT_CutSceneManager.h"
#include "IG_Material.h"
#include "SK_Factory.h"

using namespace Data;
using namespace Utility;
using namespace GameLogic;

//------------------------------------------------------------------------------
UE_NPCBlood::UE_NPCBlood(UI_EFFECT_TYPE eType)
: UE_UiEffectBase(eType)
, m_bMouseType(false)
{
}
//------------------------------------------------------------------------------
UE_NPCBlood::~UE_NPCBlood()
{

}
//------------------------------------------------------------------------------
bool UE_NPCBlood::Init()
{
	return true;
}
//------------------------------------------------------------------------------
void UE_NPCBlood::SetPendingParam(long lParam1, long /*lParam2*/)
{
	m_lTargetID	= lParam1;
	Show();
}
//------------------------------------------------------------------------------
void UE_NPCBlood::Show()
{	
	_findActorHeight();

	if ( _updateInfo() )
	{
		m_fStartShowTime = GET_PROCESS_TIME;
	}	
}
//------------------------------------------------------------------------------
void UE_NPCBlood::Hide()
{
	m_lTargetID = INVALID_ACTOR_ID;
	m_bMouseType = false;
}
//------------------------------------------------------------------------------
bool UE_NPCBlood::_updateInfo()
{
	if( !m_bVisible || !IS_NPC_ID(m_lTargetID) )
		return false;

	UE_RenderManager* pUERender  = UE_RenderManager::GetSingletonPtr();
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();

	LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(m_lTargetID);
	bool bShowBlood = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_MONSTER_HP );
	if (!bShowBlood)
	{
		bShowBlood = GetMouseType();
	}
	if (bShowBlood && pkNPC && !pkNPC->GetDead()&&pkNPC->IsShowBlood())
	{
		attr_value_type lCurHP = pkNPC->GetHP();
		attr_value_type lOldHP = pkNPC->GetOldHP();
		float fCurrentTime = GET_PROCESS_TIME;
		float fChangeTimeHP = pkNPC->GetChangeTimeHP();
		
		attr_value_type lMaxHP = pkNPC->GetAttributeMap()->GetAttribute(ATT_MAX_HP);
		if(lMaxHP == 0)
			return false;

		float HpDeepPercent = (float)lCurHP / (float)lMaxHP;
		if (HpDeepPercent < 0.0f) HpDeepPercent = 0.0f;
		
		float HpLightPercent = 0.0f;
		if(fCurrentTime - fChangeTimeHP < 0.5f)
		{
			HpLightPercent = (lCurHP + (float)(lOldHP - lCurHP)*(0.5f-fCurrentTime + fChangeTimeHP)/0.5f) / (float)lMaxHP;
		}

		UT_Vec3Int kPosition = pkNPC->GetVisibleLocation();
		kPosition.z += m_lActorHeight;
		NiPoint3 kMyPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition);

		if(!UT_CutSceneManager::GetSingleton().ShowNPCBlood())
			return true;
		
		
		int lNpcCharType = pkNPC->GetCharType();
		CSVFile::CF_CharType::DataEntry* pkData = CSVFile::CF_CharType::GetSingletonPtr()->GetEntryPtr(lNpcCharType);

		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

		if (pkData->_bBoss == 1)			//副本boss，见CharType表
		{
			pUERender->AddMonsterBloodInfo(pkNPC->GetLevel(), kMyPos, HpDeepPercent, HpLightPercent, BloodType_RED_Big_deep, pkData->_bIsWildBoss, pkGamePlayer->GetRaidBossDropTimes());
			Skill::SK_Factory* pkSkillFactory = Skill::SK_Factory::GetSingletonPtr();
			if (pkNPC->IsSkillCast())
			{

					Skill::SK_SkillExecute* pkPendingSkill = pkSkillFactory->RequestSkillExcute(pkNPC->GetCurrentStateType());//如果state是一个技能，那返回值不为空
					if (pkPendingSkill)
					{
						float CurTime=pkNPC->GetSkillCastElapseTime();
						float TotalTime=pkNPC->GetSkillCastTotalTime();
						float SkillCastPercent=-1.f;
						if (TotalTime>1.f)
						{
							SkillCastPercent=CurTime/TotalTime;
						}
						pUERender->AddMonsterSkillCastInfo(SkillCastPercent,pkNPC->GetSkillCastName(),kMyPos);			
					}	
			}
			
		}
		else if (pkData->_lFaction == 8 )	//阵营8是机器人阵容，特殊显示为玩家的血条
		{
			/*pUERender->AddPlayerBloodInfo(pkNPC->GetLevel(), kMyPos, HpDeepPercent, HpLightPercent, BloodType_BLUE_Big_deep);*/
		}
		else if (pkGamePlayer && pkGamePlayer->IsNpcBelongToBuddy(pkNPC))
		{
			/*pUERender->AddPlayerBloodInfo(pkNPC->GetLevel(), kMyPos, HpDeepPercent, HpLightPercent, BloodType_GREEN_Big_deep);*/
		}
		else
		{
			pUERender->AddMonsterBloodInfo(pkNPC->GetLevel(), kMyPos, HpDeepPercent, HpLightPercent, BloodType_RED_Small_deep);
		}

		return true;
	}

	return false;
}

bool UE_NPCBlood::Update( float fCurrentTime )
{
	bool bRes = UE_UiEffectBase::Update(fCurrentTime);
	if (bRes && _updateInfo() )
	{
		m_fStartShowTime = fCurrentTime;
	}	
	return bRes;
}
//------------------------------------------------------------------------------