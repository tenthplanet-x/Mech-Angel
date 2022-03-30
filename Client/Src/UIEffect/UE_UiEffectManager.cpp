#include "ClientPCH.h"

#include "UE_UiEffectManager.h"
#include "MG_MessageDefine.h"
#include "CA_CameraManager.h"
#include "UT_TimeManager.h"
#include "UT_Profile.h"
#include "UE_RenderManager.h"
#include "UT_CutSceneManager.h"
#include "IG_Material.h"
#include "RS_GeometryBuffer.h"

#include "IM_Manager.h"
#include "GD_ClientActorModel.h"

#include "LC_ClientLogicManager.h"
#include "LC_LogicManagerBase.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientPlaceHolderManager.h"
#include "LC_ClientPlaceHolder.h"
#include "LC_ClientGameStoryManager.h"

#define AbsInt64(val)	_abs64(val)

using namespace Data;
using namespace Utility;

const float UE_UiEffectManager::SLOT_XLATE = 50.0f;
int UE_UiEffectManager::m_iActiveLevel = 0;
bool UE_UiEffectManager::m_bActiveLevelChange = false;

UE_UiEffectManager::UE_UiEffectManager()
: m_kCuller(NULL)

, m_kPlayerNamePool(UE_PLAYER_NAME)
, m_kPlayerBloodPool(UE_PLAYER_BLOOD)
, m_kNPCBloodPool(UE_MONSTER_BLOOD)

,m_kNameArrange()
,m_fNameArrangeRedrawInterval(0.0f)
,m_fNameArrangeCurrentTime(0.0f)
,m_iLastComboFxID(0)
,m_iLastDeltaComboFxID(0)
//, m_kTaskFinishPool(UE_FINISH_TASK)
{
	// UIEffectBase用的，就放这里把
	T_NEW_D UE_RenderManager();
	//m_kTaskFinishPool.SetPoolSize(50);

    PLAYER_NORMAL_HP_COST_FX_NAMES[0] = "W_弹出提示_普通伤害数字A1.gfx";
    PLAYER_NORMAL_HP_COST_FX_NAMES[1] = "W_弹出提示_普通伤害数字A2.gfx";
    PLAYER_NORMAL_HP_COST_FX_NAMES[2] = "W_弹出提示_普通伤害数字A3.gfx";
    PLAYER_NORMAL_HP_COST_FX_NAMES[3] = "W_弹出提示_普通伤害数字A4.gfx";
	PLAYER_NORMAL_HP_COST_FX_NAMES[4] = "W_弹出提示_普通伤害数字A5.gfx";

    PLAYER_CRITICAL_HP_COST_FX_NAME = "W_弹出提示_暴击伤害数字A1.gfx";

    OTHER_EXTERNAL_HP_COST_FX_NAMES[0] = "W_弹出提示_普通外功数字A1.gfx";
    OTHER_EXTERNAL_HP_COST_FX_NAMES[1] = "W_弹出提示_普通外功数字A2.gfx";
    OTHER_EXTERNAL_HP_COST_FX_NAMES[2] = "W_弹出提示_普通外功数字A3.gfx";
    OTHER_EXTERNAL_HP_COST_FX_NAMES[3] = "W_弹出提示_普通外功数字A4.gfx";

	BOW_EXTERNAL_HP_COST_FX_NAMES[0] = "W_弹出提示_弩炮伤害数字A1.gfx";
	BOW_EXTERNAL_HP_COST_FX_NAMES[1] = "W_弹出提示_弩炮伤害数字A2.gfx";
	BOW_EXTERNAL_HP_COST_FX_NAMES[2] = "W_弹出提示_弩炮伤害数字A3.gfx";
	BOW_EXTERNAL_HP_COST_FX_NAMES[3] = "W_弹出提示_弩炮伤害数字A4.gfx";

    OTHER_INTERNAL_HP_COST_FX_NAMES[0] = "W_弹出提示_普通内功数字A1.gfx";
    OTHER_INTERNAL_HP_COST_FX_NAMES[1] = "W_弹出提示_普通内功数字A2.gfx";
    OTHER_INTERNAL_HP_COST_FX_NAMES[2] = "W_弹出提示_普通内功数字A3.gfx";
    OTHER_INTERNAL_HP_COST_FX_NAMES[3] = "W_弹出提示_普通内功数字A4.gfx";

    OTHER_EXTERNAL_CRITICAL_HP_COST_FX_NAME = "W_弹出提示_暴击外功数字A1.gfx";
    OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[0] = "W_弹出提示_暴击内功数字A1.gfx";
    OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[1] = "W_弹出提示_暴击内功数字A2.gfx";		
    OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[2] = "W_弹出提示_暴击内功数字A3.gfx";
	OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[3] = "W_弹出提示_暴击内功数字A4.gfx";

    PLAYER_MISS_FX_NAME = "W_弹出提示_文字提示A1.gfx";
    OTHER_MISS_FX_NAME = "W_弹出提示_文字提示A1.gfx";
	QINGONG_MINUS_ONE = "W_弹出提示_文字提示A1.gfx";
	COURAGE_POINT = "W_弹出提示_勇武点荣誉点.gfx";
	GAMESTORY_TRIALS = "W_弹出提示_勇武点荣誉点.gfx";

	PLAYER_JUMP_DODGE_FX_NAME = "W_弹出提示_跳闪.gfx";

    PLAYER_GET_EXP_FX_NAME = "W_弹出提示_回血文字数字A1.gfx";
    PLAYER_GET_HP_FX_NAMES[0] = "W_弹出提示_回血文字数字A1.gfx";
    PLAYER_GET_HP_FX_NAMES[1] = "W_弹出提示_回血文字数字A2.gfx";
    PLAYER_GET_MP_FX_NAMES[0] = "W_弹出提示_回蓝文字数字A1.gfx";
    PLAYER_GET_MP_FX_NAMES[1] = "W_弹出提示_回蓝文字数字A2.gfx";
	PLAYER_GET_SP_FX_NAMES[0] = "W_弹出提示_回蓝文字数字A1.gfx";
	PLAYER_GET_SP_FX_NAMES[1] = "W_弹出提示_回蓝文字数字A2.gfx";

	PLAYER_GET_GOLD_FX_NAME = "W_弹出提示_拾取金币A1.gfx";
	PLAYER_SHOW_COMBO_FX_NAME[0] = "W_弹出提示_数字提示3.gfx";
	PLAYER_SHOW_COMBO_FX_NAME[1] = "W_弹出提示_数字提示2.gfx";
	PLAYER_SHOW_COMBO_FX_NAME[2] = "W_弹出提示_数字提示1.gfx";
	PLAYER_SHOW_COMBO_FX_NAME[3] = "W_弹出提示_数字提示4.gfx";
	PLAYER_SHOW_COMBO_FX_NAME_RED[0] = "W_弹出提示_数字提示红_1.gfx";
	PLAYER_SHOW_COMBO_FX_NAME_RED[1] = "W_弹出提示_数字提示红_2.gfx";
	PLAYER_SHOW_COMBO_FX_NAME_RED[2] = "W_弹出提示_数字提示红_3.gfx";
	PLAYER_SHOW_COMBO_FX_NAME_RED[3] = "W_弹出提示_数字提示红_4.gfx";
	PLAYER_DELTA_COMBO_FX_NAME = "W_弹出提示_数字加成01.gfx";
	PLAYER_DELTA_ATTR_FX_NAME = "W_弹出提示_属性变化.gfx";

    for (int i = 0; i < 6; ++i)
    {
        SLOT_XFORM[i].m_Translate.y = 0.0f;
        SLOT_XFORM[i].m_Rotate.MakeIdentity();
        SLOT_XFORM[i].m_fScale = 1.0f;
    }
    SLOT_XFORM[0].m_Translate.x = 0.0f;         SLOT_XFORM[0].m_Translate.z = 0.0f;
	SLOT_XFORM[0].m_fScale = 1.0f;
    SLOT_XFORM[1].m_Translate.x = -SLOT_XLATE;  SLOT_XFORM[1].m_Translate.z =  SLOT_XLATE;
    SLOT_XFORM[2].m_Translate.x = -SLOT_XLATE;  SLOT_XFORM[2].m_Translate.z = -SLOT_XLATE;
    SLOT_XFORM[3].m_Translate.x =  SLOT_XLATE;  SLOT_XFORM[3].m_Translate.z =  SLOT_XLATE;
    SLOT_XFORM[4].m_Translate.x =  SLOT_XLATE;  SLOT_XFORM[4].m_Translate.z =  SLOT_XLATE;
	SLOT_XFORM[5].m_Translate.x =  0.0f;		SLOT_XFORM[5].m_Translate.z =  0.0f;

	SLOT_XFORM_COMBO.m_Translate = NiPoint3::ZERO;
	SLOT_XFORM_COMBO.m_Rotate.MakeIdentity();
	SLOT_XFORM_COMBO.m_fScale = 0.7f;

    ENTER_LEAVE_FIGHT_FX_NAME = "W_弹出提示_战斗提示1.gfx";

    //LOST_JEWELLERY_ROUND_FX_NAMES[0] = "W_落英提示_峨眉派1.gfx";
    //LOST_JEWELLERY_ROUND_FX_NAMES[1] = "W_落英提示_金钱帮1.gfx";
    //LOST_JEWELLERY_ROUND_FX_NAMES[2] = "W_落英提示_圣水宫1.gfx";
    //LOST_JEWELLERY_ROUND_FX_NAMES[3] = "W_落英提示_铁掌帮1.gfx";
    //LOST_JEWELLERY_ROUND_FX_NAMES[4] = "W_落英提示_真武观1.gfx";
    //LOST_JEWELLERY_STATE_FX_NAME = "W_落英提示_挑战1.gfx";

	tick = 0.0f;
}

UE_UiEffectManager::~UE_UiEffectManager()
{
}

bool UE_UiEffectManager::Init()
{

#ifndef _MAP_VIEWER_BUILD
	m_kPlayerNamePool.SetPoolSize(100);
	m_kPlayerNamePool.SetAutoextend(false);//同屏只能显示100个
	m_kPlayerBloodPool.SetPoolSize(50);
	m_kPlayerBloodPool.SetAutoextend(false);
	m_kNPCBloodPool.SetPoolSize(50);
	m_kNPCBloodPool.SetAutoextend(false);
#endif

	NiRendererPtr spRenderer = NiRenderer::GetRenderer();
	if (spRenderer)
	{
		NiRenderTargetGroupPtr spCurrRenderTargetGroup = spRenderer->GetDefaultRenderTargetGroup();
		NIASSERT(spCurrRenderTargetGroup);
		unsigned int uiWidth = spCurrRenderTargetGroup->GetWidth(0);
		unsigned int uiHeight = spCurrRenderTargetGroup->GetHeight(0);
		OnResolutionChanged(uiWidth, uiHeight);
	}
	
	//m_kTaskFinishPool.SetAutoextend(false);
	return true;
}

bool UE_UiEffectManager::UnInit()
{	
    m_kPlayerNamePool.DeletePool();
	m_kPlayerBloodPool.DeletePool();
	m_kNPCBloodPool.DeletePool();
	
	UE_RenderManager::GetSingleton().DestroyInstance();
	return true;
}

bool UE_UiEffectManager::RecvMessage(MG_MessageBase& rkMessage)
{
	if(!g_settings.UI_RENDER_SHOW)
	{
		return false;
	}
	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK, _event_TimerTick)
	MESSAGE_HANDLE(MGT_INPUT_KEY_ALT, _event_InputKeyAltPressed)
	MESSAGE_HANDLE(MGT_UE_PLAYER_HP_NORMAL_COST, _event_PlayerHPNormalCost)
	MESSAGE_HANDLE(MGT_UE_PLAYER_HP_CRITICAL_COST, _event_PlayerHPCriticalCost)
	MESSAGE_HANDLE(MGT_UE_MONSTER_HP_NORMAL_COST, _event_NPCHPNormalCost)
	MESSAGE_HANDLE(MGT_UE_MONSTER_HP_CRITICAL_COST, _event_NPCHPCriticalCost)
	MESSAGE_HANDLE(MSG_UE_PLAYER_COURAGE_POINT, _event_PlayerCountPoint)
	MESSAGE_HANDLE(MSG_UE_PLAYER_GAMESTORY_TRIALS, _event_PlayerGameStoryTrials)
	MESSAGE_HANDLE(MGT_UE_PLAYER_MISS, _event_PlayerMiss)
	MESSAGE_HANDLE(MST_UE_PLAYER_STATE_CHANGE, _event_PlayerStateChange)
	MESSAGE_HANDLE(MGT_UE_PLAYER_JUMP_DODGE, _event_PlayerJumpDodge)
	MESSAGE_HANDLE(MGT_UE_MONSTER_MISS, _event_NPCMiss)
	MESSAGE_HANDLE(MGT_UE_ENTER_FIGHT, _event_EnterFight)
	MESSAGE_HANDLE(MGT_UE_LEAVE_FIGHT,_event_LeaveFight)
	MESSAGE_HANDLE(MSG_UE_PLAYER_PICK_GOLD, _event_PickGold)
	MESSAGE_HANDLE(MSG_UE_PLAYER_PICK_UNBINDYUANBAO, _event_PickUnbindYuanbao)
	MESSAGE_HANDLE(MSG_UE_PLAYER_PICK_SP, _event_PickSP)
	MESSAGE_HANDLE(MSG_UE_PLAYER_SHOW_COMBO, _event_ShowCombo)
	MESSAGE_HANDLE(MSG_UE_PLAYER_DELTA_COMBO, _event_DeltaCombo)
	MESSAGE_HANDLE(MGT_UE_FINISH_TASK, _event_FinishTask);
	MESSAGE_HANDLE(MGT_UE_FAIL_TASK, _event_FailTask);
	MESSAGE_HANDLE(MSG_UE_DELTA_EXP, _event_DeltaExp)
	MESSAGE_HANDLE(MSG_UE_DELTA_HP, _event_DeltaHp)
	MESSAGE_HANDLE(MSG_UE_DELTA_MP, _event_DeltaMp)
	MESSAGE_HANDLE(MSG_UE_DELTA_SP, _event_DeltaSp)
	MESSAGE_HANDLE(MSG_UE_DELTA_ATTR, _event_DeltaAttr)
	MESSAGE_HANDLE(MSG_UE_SHOW_PLAYER_HEADALL, _event_ShowPlayerHeadALL)
	MESSAGE_HANDLE(MSG_UE_SHOW_PLAYER_NAME, _event_ShowPlayerName)
	MESSAGE_HANDLE(MSG_UE_HIDE_PLAYER_NAME, _event_HidePlayerName)
	MESSAGE_HANDLE(MSG_UE_SHOW_PLAYER_BLOOD, _event_ShowPlayerBlood)
	MESSAGE_HANDLE(MSG_UE_HIDE_PLAYER_BLOOD, _event_HidePlayerBlood)
	MESSAGE_HANDLE(MSG_UE_UPDATE_PLAYER_BLOOD, _event_UpdatePlayerBlood)
    MESSAGE_HANDLE(MSG_UE_SHOW_MONSTER_NAME, _event_ShowPlayerName)
	MESSAGE_HANDLE(MSG_UE_HIDE_MONSTER_NAME, _event_HidePlayerName)
	MESSAGE_HANDLE(MSG_UE_SHOW_MONSTER_BLOOD, _event_ShowNPCBlood)
	MESSAGE_HANDLE(MSG_UE_HIDE_MONSTER_BLOOD, _event_HideNPCBlood)
	MESSAGE_HANDLE(MSG_UE_UPDATE_MONSTER_BLOOD, _event_UpdateNPCBlood)
	MESSAGE_HANDLE(MSG_UE_SHOW_NPC_NAME, _event_ShowPlayerName)
	MESSAGE_HANDLE(MSG_UE_HIDE_NPC_NAME, _event_HidePlayerName)
	MESSAGE_HANDLE(MSG_UE_CHANGE_PLAYER_TITLE,_event_ChangePlayerTitle)
	//MESSAGE_HANDLE(MGT_UE_LOST_JEWELLERY_ROUND, _event_LostJewelleryRound)
	//MESSAGE_HANDLE(MGT_UE_LOST_JEWELLERY_STATE,_event_LostJewelleryState)
	MESSAGE_HANDLE(MSG_UE_SHOW_DROPITEM_NAME, _event_ShowDropItemName)
	MESSAGE_HANDLE(MSG_UE_HIDE_DROPITEM_NAME, _event_HidePlayerName)
	MESSAGE_HANDLE(MSG_UE_RESET_TIME_DROPITEM_NAME, _event_ResetTimePlayeName)
	MESSAGE_HANDLE(MSG_UE_SET_DROPITEM_NAME_TIME, _event_SetDropItemTime)
	MESSAGE_HANDLE(MSG_UE_CHANGE_NAME_FRAME_STATE, _event_ChangeNameFrameState)
	MESSAGE_HANDLE(MSG_UE_PLACE_HOLDER_NAME_REDRAW, _event_PlaceHolderNameRedraw)
	MESSAGE_HANDLE(MSG_UE_NAME_REDRAW, _event_NameRedraw)
	MESSAGE_HANDLE(MSG_UE_SET_INTERACTIVE_BUTTON_VISIBLE, _event_SetInteractiveBtnVisible)
	MESSAGE_HANDLE(MSG_UE_REMOVE_DISPLAY_CHEST, _event_RemoveDisplayChest)
	MESSAGE_HANDLE(MGT_UE_VIP_LEVELUP, _event_VipLevelup);
	MESSAGE_HANDLE(MGT_UE_TASKCHAPTER_OPEN, _event_TaskChapterOpen);
	MESSAGE_HANDLE(MGT_UE_VIP_NEW_LEVELUP, _event_VipNewLevelup);
	MESSAGE_HANDLE(MGT_UE_COMMANDER_LEVELUP, _event_CommanderLevelUp);
	END_MESSAGE_HANDLE

	return true;
}

void UE_UiEffectManager::GetVisibleArrary(NiVisibleArray& rkVisibleArray)
{		

	//m_kTaskFinishPool.GetVisibleArrary(m_kCuller,rkVisibleArray);
}

void UE_UiEffectManager::ClearAll()
{
    m_kPlayerNamePool.DeletePool();
    m_kPlayerBloodPool.DeletePool();
    m_kNPCBloodPool.DeletePool();

    //m_kPlayerNamePool.RequestRedraw();
}

void UE_UiEffectManager::ClearNameArrange()
{
	m_kNameArrange.CleanUp();
}

void UE_UiEffectManager::_event_TimerTick(MG_MessageBase& rkMessage)
{	
	if(!UT_CutSceneManager::GetSingleton().ShowUE())
		return;

	UE_RenderManager* pkUERenderManager = UE_RenderManager::GetSingletonPtr();
	if(!pkUERenderManager)
		return;

	//pkUERenderManager->Update(0.0f);	
	T_PROFILE("Update::UEManager");

	pkUERenderManager->ClearAllBuffer();

	float fCurrentTime = rkMessage.m_fParam1;
	//_redrawPlaceHolderName(fCurrentTime);
	m_kNameArrange.Update(fCurrentTime);
    m_kPlayerNamePool.Update(fCurrentTime);
	m_kPlayerBloodPool.Update(fCurrentTime);
	m_kNPCBloodPool.Update(fCurrentTime);

	UE_UiEffectManager::m_bActiveLevelChange = false;

	if ( fCurrentTime - tick > 0.2f )
	{
		if ( !m_attrChange.empty() )
		{
			MG_MessageBase da = m_attrChange.back();
			m_attrChange.pop_back();

			int64_t am = da.m_lParam3 > 0 ? 1 : 0;
			int64_t param2 = (int64_t)da.m_lParam2;
			_playUeFxOnActor(da.m_lParam1, PLAYER_DELTA_ATTR_FX_NAME, 
				( AbsInt64((int64_t)da.m_lParam3) | (am << (27+32)) ) + (param2 << (28+32))  , da.m_lParam4);

		}
		tick = fCurrentTime;
	}

	//m_kTaskFinishPool.Update(fCurrentTime);
}

void UE_UiEffectManager::_event_InputKeyAltPressed(MG_MessageBase& rkMessage)
{
	_resetPlayerNameTime();
}

void UE_UiEffectManager::_redrawPlaceHolderName(float fCurrentTime)
{
	float dTime = fCurrentTime - m_fNameArrangeCurrentTime;
	if (dTime >= m_fNameArrangeRedrawInterval)
	{
		//m_kNameArrange.Reset();
		//m_kPlayerNamePool.RequestRedrawPlaceHolder();
		m_fNameArrangeCurrentTime = fCurrentTime;
	}
}

void UE_UiEffectManager::_event_CommanderLevelUp(MG_MessageBase& rkMessage)
{
	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

	int iFxID = rkFxManager.CreateFx(RES_FX_COMMANDER_LEVELUP, true);
	rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, rkMessage.m_llParam);
}

void UE_UiEffectManager::_event_PlayerHPNormalCost(MG_MessageBase& rkMessage)
{
    T_ASSERT(rkMessage.m_lParam3 >= 0 && rkMessage.m_lParam3 <= 5);
	long lMapID = rkMessage.m_lParam4;
	LC_ClientGameStoryManager *pkGameStory = CLIENT_GET_GAMESTORY_MANAGER();//BUG #21998 【BUG】【冥狱水牢】场景内出现错误文字
	if(lMapID == QINGGONG_TRIAL_MAPID && pkGameStory && pkGameStory->IsBatchBeginByGameStoryID(GAMESTORY_ID_QingGongShiLian))
	{
		 _playUeFxOnActor(rkMessage.m_lParam1, QINGONG_MINUS_ONE, (int64_t)4 << (28+32), 0);
	}
	else
	{
		int64_t am = rkMessage.m_llParam > 0 ? 1 : 0;
		int64_t bm = rkMessage.m_lParam5;
		if (rkMessage.m_lParam5 == DamageElementType_2)
		{
			_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_NORMAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)1 << (28+32)), rkMessage.m_lParam3);
		}
		else if(rkMessage.m_lParam5 == DamageElementType_3)
		{
			_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_NORMAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)0 << (28+32)), rkMessage.m_lParam3);
		}
		else
			_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_NORMAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], 
			( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)2 << (28+32)), rkMessage.m_lParam3);
	}
}

void UE_UiEffectManager::_event_PlayerHPCriticalCost(MG_MessageBase& rkMessage)
{
	T_ASSERT(rkMessage.m_lParam3 >= 0 && rkMessage.m_lParam3 <= 4);
	int64_t am = rkMessage.m_llParam > 0 ? 1 : 0;
	int64_t bm = rkMessage.m_lParam5;
	if (rkMessage.m_lParam5 == DamageElementType_2)
	{
		_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_CRITICAL_HP_COST_FX_NAME,
			( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)2 << (28+32)), rkMessage.m_lParam3);
	}
	else if(rkMessage.m_lParam5 == DamageElementType_3)
	{
		_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_CRITICAL_HP_COST_FX_NAME,
			( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)0 << (28+32)), rkMessage.m_lParam3);
	}
	else
		_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_CRITICAL_HP_COST_FX_NAME, 
			( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)1 << (28+32)), rkMessage.m_lParam3);
}

void UE_UiEffectManager::_event_NPCHPNormalCost(MG_MessageBase& rkMessage)
{
    T_ASSERT(rkMessage.m_lParam3 >= 0 && rkMessage.m_lParam3 <= 4);
	if (rkMessage.m_lParam4 == 2)
	{
		_playUeFxOnActor(rkMessage.m_lParam1, BOW_EXTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], rkMessage.m_llParam, rkMessage.m_lParam3);
	}
	else
	{
		int64_t am = rkMessage.m_llParam > 0 ? 1 : 0;
		int64_t bm = rkMessage.m_lParam5;
		if (rkMessage.m_lParam5 == DamageElementType_2)
		{
			_playUeFxOnActor(rkMessage.m_lParam1, 
				(rkMessage.m_lParam4 == 0) ? OTHER_EXTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1] : OTHER_INTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)1 << (28+32)), rkMessage.m_lParam3);
		}
		else if(rkMessage.m_lParam5 == DamageElementType_3)
		{
			_playUeFxOnActor(rkMessage.m_lParam1, 
				(rkMessage.m_lParam4 == 0) ? OTHER_EXTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1] : OTHER_INTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)0 << (28+32)), rkMessage.m_lParam3);
		}
		else
			_playUeFxOnActor(rkMessage.m_lParam1, 
				(rkMessage.m_lParam4 == 0) ? OTHER_EXTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1] : OTHER_INTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)2 << (28+32)), rkMessage.m_lParam3);
	}
		
}

void UE_UiEffectManager::_event_NPCHPCriticalCost(MG_MessageBase& rkMessage)
{
    T_ASSERT(rkMessage.m_lParam3 >= 0 && rkMessage.m_lParam3 <= 4);
	if (rkMessage.m_lParam4 == 2)
	{
		_playUeFxOnActor(rkMessage.m_lParam1, BOW_EXTERNAL_HP_COST_FX_NAMES[rkMessage.m_lParam3 - 1], rkMessage.m_llParam, rkMessage.m_lParam3);
	}
	else
	{
		int64_t am = rkMessage.m_llParam > 0 ? 1 : 0;
		int64_t bm = rkMessage.m_lParam5;
		if (rkMessage.m_lParam5 == DamageElementType_2)
		{
			_playUeFxOnActor(rkMessage.m_lParam1, 
				(rkMessage.m_lParam4 == 0) ? OTHER_EXTERNAL_CRITICAL_HP_COST_FX_NAME : OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[rkMessage.m_lParam3-1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)2 << (28+32)), rkMessage.m_lParam3);
		}
		else if(rkMessage.m_lParam5 == DamageElementType_3)
		{
			_playUeFxOnActor(rkMessage.m_lParam1, 
				(rkMessage.m_lParam4 == 0) ? OTHER_EXTERNAL_CRITICAL_HP_COST_FX_NAME : OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[rkMessage.m_lParam3-1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)0 << (28+32)), rkMessage.m_lParam3);
		}
		else
		{
			_playUeFxOnActor(rkMessage.m_lParam1, 
				(rkMessage.m_lParam4 == 0) ? OTHER_EXTERNAL_CRITICAL_HP_COST_FX_NAME : OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[rkMessage.m_lParam3-1], 
				( AbsInt64(rkMessage.m_llParam) | (am << (27+32)) )  + ((int64_t)1 << (28+32)), rkMessage.m_lParam3);
		}
	}
}

void UE_UiEffectManager::_event_PlayerCountPoint(MG_MessageBase& rkMessage)
{
	if ( rkMessage.m_lParam2 > 0 )
	{
		int64_t am = rkMessage.m_lParam2 > 0 ? 1 : 0;
		_playUeFxOnActor(rkMessage.m_lParam1, COURAGE_POINT,( AbsInt64((int64_t)rkMessage.m_lParam2) | (am << (27+32) ) )  + ((int64_t)7 << (28+32)), 0);
	}
}

void UE_UiEffectManager::_event_PlayerGameStoryTrials(MG_MessageBase& rkMessage)
{
	if ( rkMessage.m_lParam2 > 0 )
	{
		int64_t am = rkMessage.m_lParam2 > 0 ? 1 : 0;
		_playUeFxOnActor(rkMessage.m_lParam1, GAMESTORY_TRIALS,( AbsInt64((int64_t)rkMessage.m_lParam2) | (am << (27+32) ) )  + ((int64_t)6 << (28+32)), 0);
	}
}

void UE_UiEffectManager::_event_PlayerMiss(MG_MessageBase& rkMessage)
{
    _playUeFxOnActor(rkMessage.m_lParam1, PLAYER_MISS_FX_NAME, (int64_t)1 << (28+32), 0);
}

void UE_UiEffectManager::_event_PlayerJumpDodge(MG_MessageBase& rkMessage)
{
	long lMapID = rkMessage.m_lParam2;
	if(lMapID == QINGGONG_TRIAL_MAPID)
	{
	   _playUeFxOnActor(rkMessage.m_lParam1, PLAYER_JUMP_DODGE_FX_NAME, (int64_t)7 << (28+32), 0);
	}
	else
	{
		_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_JUMP_DODGE_FX_NAME, (int64_t)5 << (28+32), 0);
	}
}

void UE_UiEffectManager::_event_NPCMiss(MG_MessageBase& rkMessage)
{
    _playUeFxOnActor(rkMessage.m_lParam1, OTHER_MISS_FX_NAME, (int64_t)1 << (28+32), 0);
}

void UE_UiEffectManager::_event_PlayerStateChange(MG_MessageBase& rkMessage)
{
	StringType sFxName = _findStateFxName(rkMessage.m_lParam2);
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor(rkMessage.m_lParam1);
	if (pkVisibleData != NULL && sFxName != "NULL")
	{
		NiPoint3* pkRotate = NULL;
		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		/*StringType rp = pkFxManager->findPlatformReplace(sFxName);*/
		pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)0, 1, 0, sFxName, NULL, pkRotate);
	}
}

void UE_UiEffectManager::_event_EnterFight(MG_MessageBase& rkMessage)
{
    FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

    int iFxID = rkFxManager.CreateFx(ENTER_LEAVE_FIGHT_FX_NAME, true);
    rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, (int64_t)2 << (28+32));
}

void UE_UiEffectManager::_event_LeaveFight(MG_MessageBase& rkMessage)
{
    FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

    int iFxID = rkFxManager.CreateFx(ENTER_LEAVE_FIGHT_FX_NAME, true);
    rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, (int64_t)3 << (28+32));
}

void UE_UiEffectManager::_event_PickGold(MG_MessageBase& rkMessage)
{
	int64_t am = rkMessage.m_lParam2 > 0 ? 1 : 0;
	_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_GET_GOLD_FX_NAME, ((int64_t)4 << (28+32)) | ( AbsInt64((int64_t)rkMessage.m_lParam2) | (am << (27+32) ) ), 1);
	//int iFxID = rkFxManager.CreateFx(PLAYER_GET_GOLD_FX_NAME, true);
	//rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, );
}

void UE_UiEffectManager::_event_PickUnbindYuanbao(MG_MessageBase& rkMessage)
{
	int64_t am = rkMessage.m_lParam2 > 0 ? 1 : 0;
	_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_GET_SP_FX_NAMES[0], ((int64_t)0 << (28+32)) | ( AbsInt64((int64_t)rkMessage.m_lParam2) | (am << (27+32) ) ), 1);
}

void UE_UiEffectManager::_event_PickSP(MG_MessageBase& rkMessage)
{
	_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_GET_SP_FX_NAMES[0], ((int64_t)3 << (28+32)) | (int64_t)rkMessage.m_lParam2, 1);
}

void UE_UiEffectManager::_event_ShowCombo(MG_MessageBase& rkMessage)
{
	if (rkMessage.m_lParam3 < 0 || rkMessage.m_lParam3 > 3)
		return ;


	int64_t iFxExtParam = ((int64_t)rkMessage.m_lParam3 << (28+32)) | (int64_t)rkMessage.m_lParam2;

	GD_ClientActorModel* pkModel = (GD_ClientActorModel*)IM_Manager::GetSingleton().GetModel(rkMessage.m_lParam1);
	if (pkModel == NULL)
	{
		return;
	}

	NiAVObjectPtr kDummyNode = pkModel->GetFxDummyNode(GD_ModelBase::FDT_STABLE_GROUND);
	
	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

	int iFxID = 0;

	if (rkMessage.m_lParam4 > 0)
		iFxID = rkFxManager.CreateFx(PLAYER_SHOW_COMBO_FX_NAME_RED[rkMessage.m_lParam3], true);
	else
		iFxID = rkFxManager.CreateFx(PLAYER_SHOW_COMBO_FX_NAME[rkMessage.m_lParam3], true);
	
	rkFxManager.BindFxToDummyNode(iFxID, kDummyNode, false, false, &SLOT_XFORM_COMBO);
	rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, iFxExtParam);

	if (m_iLastComboFxID > 0)
	{
		rkFxManager.ReleaseFx(m_iLastComboFxID, true);
	}

	m_iLastComboFxID = iFxID;

	//GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, "连击特效：lastFxID = %d, curFxID = %d, 连击数 = %d", m_iLastComboFxID, iFxID, iFxExtParam&0xFFFFFFF);
}

void UE_UiEffectManager::_event_DeltaCombo(MG_MessageBase& rkMessage)
{
	if (rkMessage.m_lParam3 < 0 || rkMessage.m_lParam3 >= 3)
		return ;


	int64_t iFxExtParam = ((int64_t)rkMessage.m_lParam3 << (28+32)) | (int64_t)rkMessage.m_lParam2;

	GD_ClientActorModel* pkModel = (GD_ClientActorModel*)IM_Manager::GetSingleton().GetModel(rkMessage.m_lParam1);
	if (pkModel == NULL)
	{
		return;
	}

	NiAVObjectPtr kDummyNode = pkModel->GetFxDummyNode(GD_ModelBase::FDT_DYNAMIC_GROUND_NOSCALL);

	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

	int iFxID = rkFxManager.CreateFx(PLAYER_DELTA_COMBO_FX_NAME, true);
	rkFxManager.BindFxToDummyNode(iFxID, kDummyNode, false, false, &SLOT_XFORM_COMBO);
	rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, iFxExtParam);
}

void UE_UiEffectManager::_event_FinishTask(MG_MessageBase& rkMessage)
{ 
	_playUeFxOnActorWithoutDummy(rkMessage.m_lParam1, RES_FX_FINISH_TASK);
}

void UE_UiEffectManager::_event_FailTask(MG_MessageBase& rkMessage)
{
	_playUeFxOnActorWithoutDummy(rkMessage.m_lParam1, RES_FX_FAIL_TASK);
}

void UE_UiEffectManager::_event_VipLevelup(MG_MessageBase& rkMessage)
{
	StringType sFxName;
	UT_BaseStringUtility::format(sFxName, "%s%d.gfx", RES_FX_PRE_VIP_LEVELUP, rkMessage.m_lParam2);
	_playUeFxOnActorWithoutDummy(rkMessage.m_lParam1, sFxName);
}

void UE_UiEffectManager::_event_TaskChapterOpen(MG_MessageBase& rkMessage)
{
	StringType sFxName;
	UT_BaseStringUtility::format(sFxName, "%s_%02d.gfx", RES_FX_TASKCHAPTER_OPEN, rkMessage.m_lParam2);
	_playUeFxOnActorWithoutDummy(rkMessage.m_lParam1, sFxName);
}

void UE_UiEffectManager::_event_VipNewLevelup(MG_MessageBase& rkMessage)
{
	StringType sFxName;
	UT_BaseStringUtility::format(sFxName, "%s%d.gfx", RES_FX_PRE_VIP_NEW_LEVELUP, rkMessage.m_lParam2);
	_playUeFxOnActorWithoutDummy(rkMessage.m_lParam1, sFxName);
}

void UE_UiEffectManager::_event_DeltaExp(MG_MessageBase& rkMessage)
{
	int64_t am = rkMessage.m_lParam2 > 0 ? 1 : 0;
    _playUeFxOnActor(rkMessage.m_lParam1, PLAYER_GET_EXP_FX_NAME, 
        ( AbsInt64((int64_t)rkMessage.m_lParam2) | (am << (27+32) ) ) + ((int64_t)1 << (28+32)), 0);
}

void UE_UiEffectManager::_event_DeltaHp(MG_MessageBase& rkMessage)
{
	int64_t am = rkMessage.m_llParam > 0 ? 1 : 0;
    T_ASSERT(rkMessage.m_lParam3 >= 0 && rkMessage.m_lParam3 < 2);
    _playUeFxOnActor(rkMessage.m_lParam1, PLAYER_GET_HP_FX_NAMES[rkMessage.m_lParam3], 
        ( AbsInt64(rkMessage.m_llParam) | (am << (27+32) ) ) + ((int64_t)2 << (28+32)), 0);
}

void UE_UiEffectManager::_event_DeltaMp(MG_MessageBase& rkMessage)
{
	int64_t am = rkMessage.m_llParam > 0 ? 1 : 0;
    T_ASSERT(rkMessage.m_lParam3 >= 0 && rkMessage.m_lParam3 < 2);
    _playUeFxOnActor(rkMessage.m_lParam1, PLAYER_GET_MP_FX_NAMES[rkMessage.m_lParam3], 
       ( AbsInt64(rkMessage.m_llParam) | (am << (27+32) ) ) /*+ (0 << 28)*/, 0);
}

void UE_UiEffectManager::_event_DeltaSp(MG_MessageBase& rkMessage)
{
	int64_t am = rkMessage.m_llParam > 0 ? true : false;
	T_ASSERT(rkMessage.m_lParam3 >= 0 && rkMessage.m_lParam3 < 2);
	_playUeFxOnActor(rkMessage.m_lParam1, PLAYER_GET_SP_FX_NAMES[rkMessage.m_lParam3], 
		( AbsInt64(rkMessage.m_llParam) | (am << (27+32) ) ) + ((int64_t)3 << (28+32)), 0);
}

void UE_UiEffectManager::_event_DeltaAttr(MG_MessageBase& rkMessage)
{
	if ( m_attrChange.size() > 10 )
	{
		m_attrChange.pop_front();
		m_attrChange.push_back(rkMessage);
	}
	else
	{
		m_attrChange.push_back(rkMessage);
	}
}
void UE_UiEffectManager::_event_ShowPlayerHeadALL(MG_MessageBase& rkMessage)
{
	m_bActiveLevelChange = true;
	m_iActiveLevel = rkMessage.m_lParam1;
}


void UE_UiEffectManager::_event_ShowPlayerName(MG_MessageBase& rkMessage)
{	
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if(!pkUE)
	{
		pkUE = m_kPlayerNamePool.GetPoolable();
		if(pkUE)//超过40个，不显示更新
		{
			pkUE->SetPendingParam( rkMessage.m_lParam1 , 0 );
			pkUE->Update(0.0f);
		}

	}
}

void UE_UiEffectManager::_event_ShowDropItemName(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if(!pkUE)
	{
		pkUE = m_kPlayerNamePool.GetPoolable();
		if(pkUE)//超过40个，不显示更新
		{
			float ft = rkMessage.m_lParam3 == 1 ? g_settings.UE_PLACEHOLDER_NAME_LIFE_TIME_OWNER : g_settings.UE_PLACEHOLDER_NAME_LIFE_TIME;
			pkUE->SetPendingParam( rkMessage.m_lParam1 , rkMessage.m_lParam2, ft);
			pkUE->Update(0.0f);
		}
	}
}

void UE_UiEffectManager::_event_ResetTimePlayeName(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if (pkUE)
	{
		pkUE->ResetTime(rkMessage.m_lParam2 == 1);
	}
}

void UE_UiEffectManager::_event_SetDropItemTime(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if (pkUE)
	{
		float ft = rkMessage.m_lParam2 == 1 ? g_settings.UE_PLACEHOLDER_NAME_LIFE_TIME_OWNER : g_settings.UE_PLACEHOLDER_NAME_LIFE_TIME;
		pkUE->SetPlaceHoldeTime(ft);
		if (rkMessage.m_lParam3 == 1)
			pkUE->ResetTime();
	}
}

void UE_UiEffectManager::_resetPlayerNameTime()
{
	m_kPlayerNamePool.ResetTime();
}

StringType UE_UiEffectManager::_findStateFxName(long lStateType)
{
	StringType res = "NULL";

	switch (lStateType)
	{
	case 2:
		res = "W_弹出提示_战斗攻击.gfx";
		break;
	case 6:
		res = "W_弹出提示_战斗防御.gfx";
		break;
	case 8:
		res = "W_弹出提示_战斗闪避.gfx";
		break;
	case 9:
		res = "W_弹出提示_战斗暴击.gfx";
		break;
	case 11:
		res = "W_弹出提示_战斗速度.gfx";
		break;
	case 33:
		res = "W_弹出提示_战斗命中.gfx";
		break;
	case 102:
		res = "W_弹出提示_战斗攻击2.gfx";
		break;
	case 106:
		res = "W_弹出提示_战斗防御2.gfx";
		break;
	case 108:
		res = "W_弹出提示_战斗闪避2.gfx";
		break;
	case 109:
		res = "W_弹出提示_战斗暴击2.gfx";
		break;
	case 111:
		res = "W_弹出提示_战斗速度2.gfx";
		break;
	case 133:
		res = "W_弹出提示_战斗命中2.gfx";
		break;
	case 204:	//缠绕
		res = "W_弹出提示_状态禁锢.gfx";
		break;
	case 205:	//冰冻
		res = "W_弹出提示_状态冰冻.gfx";
		break;
	case 206:	//眩晕
		res = "W_弹出提示_状态晕眩.gfx";
		break;
	case 207:	//击退
		res = "W_弹出提示_状态击退.gfx";
		break;
	case 208:	//击飞
		res = "W_弹出提示_状态击飞.gfx";
		break;
	case 209:	//诅咒
		res = "W_弹出提示_状态诅咒.gfx";
		break;
	//case 210:	//龙渊之矢
	//	res = "W_弹出提示_状态龙渊.gfx";
	//	break;
	case 301:	//护盾
		res = "W_弹出提示_状态护盾.gfx";
		break;
	case 302:	//免疫
	case 304:	//无敌
		res = "W_弹出提示_状态免疫.gfx";
		break;
	case 303:	//隐身
		res = "W_弹出提示_状态隐身.gfx";
		break;
	case 401:	
		res = "W_弹出提示_战斗受伤.gfx";
		break;
	case 402:
		res = "W_弹出提示_战斗受伤2.gfx";
		break;
	case 403:
		res = "W_弹出提示_战斗伤害.gfx";
		break;
	case 404:
		res = "W_弹出提示_战斗伤害2.gfx";
		break;
	}

	return res;
}

void UE_UiEffectManager::_event_HidePlayerName(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);

	if(pkUE != NULL)
		pkUE->Hide();
}
//------------------------------------------------------------------------
void UE_UiEffectManager::_event_ChangeNameFrameState(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if(NULL != pkUE)
	{
		pkUE->ChangeNameFrameState(rkMessage.m_lParam2);
		pkUE->Update(0.0f);
	}
}
//------------------------------------------------------------------------
void UE_UiEffectManager::_event_SetInteractiveBtnVisible(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if(NULL != pkUE)
	{
		pkUE->SetInteractiveBtnVisible(rkMessage.m_lParam2 != 0);
		pkUE->Update(0.0f);
	}
}
//------------------------------------------------------------------------
void UE_UiEffectManager::_event_RemoveDisplayChest(MG_MessageBase& rkMessage)
{
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ClientPlaceHolderManager* pkPlaceHolderManager = (LC_ClientPlaceHolderManager*)ENGINE_GET_CHEST_MANAGER();

	LC_ClientPlaceHolder* pkPlaceHolderBase  = (LC_ClientPlaceHolder*)pkPlaceHolderManager->FindDisplayPlaceHolder(rkMessage.m_lParam1, rkMessage.m_lParam2 == 1);
	if (pkPlaceHolderBase)
	{
		if(pkPlaceHolderBase->IsCreateVisibleData())
			pkPlaceHolderBase->DestoryVisibleData();
		pkWorldManager->RemoveObjectFromMap(pkPlaceHolderBase);
		pkPlaceHolderManager->DestroyDisplayPlaceHolder(pkPlaceHolderBase, rkMessage.m_lParam2 == 1);
	}
}
//------------------------------------------------------------------------
void UE_UiEffectManager::_event_PlaceHolderNameRedraw(MG_MessageBase& rkMessage)
{
	m_kPlayerNamePool.RedrawPlaceHolderNameRect();
}
//------------------------------------------------------------------------
void UE_UiEffectManager::_event_ChangePlayerTitle(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);

	if(pkUE != NULL)
		pkUE->ChangePlayerTitle();
}

void UE_UiEffectManager::_event_ShowPlayerBlood(MG_MessageBase& rkMessage)
{
	UE_PlayerBlood*  pkUE = m_kPlayerBloodPool.GetUiEffect(rkMessage.m_lParam1);
	if(pkUE != NULL)
	{
		pkUE->Show();
	}
	else
	{
		{
			pkUE = m_kPlayerBloodPool.GetPoolable();
			if(!pkUE)
				return;
			pkUE->SetPendingParam(rkMessage.m_lParam1, 0);
		}
	}
}

void UE_UiEffectManager::_event_HidePlayerBlood(MG_MessageBase& rkMessage)
{
	UE_PlayerBlood* pkUE = m_kPlayerBloodPool.GetUiEffect(rkMessage.m_lParam1);
	
	if(pkUE != NULL)
		pkUE->Hide();
}

void UE_UiEffectManager::_event_UpdatePlayerBlood(MG_MessageBase& rkMessage)
{
	UE_PlayerBlood* pkUE = m_kPlayerBloodPool.GetUiEffect(rkMessage.m_lParam1);	
	if(pkUE == NULL)
	{
		pkUE = m_kPlayerBloodPool.GetPoolable();
		if(!pkUE)
			return;
		pkUE->SetPendingParam(rkMessage.m_lParam1, 0);	
	}
	
	if(rkMessage.m_lParam2 > 0)
		pkUE->SetMouseType(true);
	else
		pkUE->SetMouseType(false);
	pkUE->Show();
}

void UE_UiEffectManager::_event_ShowNPCBlood(MG_MessageBase& rkMessage)
{
	UE_NPCBlood* pkUE = m_kNPCBloodPool.GetUiEffect(rkMessage.m_lParam1);

	if(pkUE != NULL)
	{
		pkUE->Show();
	}
	else
	{
		//if ( m_kNPCBloodPool.GetPendingEffectByTargetID(rkMessage.m_lParam1) == NULL)
		{
			pkUE = m_kNPCBloodPool.GetPoolable();
			if(!pkUE)
				return;
			pkUE->SetPendingParam(rkMessage.m_lParam1, 0);
		}
	}
}

void UE_UiEffectManager::_event_HideNPCBlood(MG_MessageBase& rkMessage)
{
	UE_NPCBlood* pkUE = m_kNPCBloodPool.GetUiEffect(rkMessage.m_lParam1);

	if(pkUE != NULL)
		pkUE->Hide();
}

void UE_UiEffectManager::_event_UpdateNPCBlood(MG_MessageBase& rkMessage)
{
	UE_NPCBlood* pkUE = m_kNPCBloodPool.GetUiEffect(rkMessage.m_lParam1);
	
	if(pkUE != NULL)
	{
		if(rkMessage.m_lParam2 > 0)
			pkUE->SetMouseType(true);
		else if (rkMessage.m_lParam2 < 0)
			pkUE->SetMouseType(false);
		pkUE->Show();
	}
	else
	{
		////if ( m_kNPCBloodPool.GetPendingEffectByTargetID(rkMessage.m_lParam1) == NULL)
		//{
		//	pkUE = m_kNPCBloodPool.GetPoolable();
		//	if(!pkUE)
		//		return;
		//	pkUE->SetPendingParam(rkMessage.m_lParam1, 0);
		//}
	}
	_event_UpdateNPCName(rkMessage);
}

void UE_UiEffectManager::_event_UpdateNPCName(MG_MessageBase& rkMessage)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if(pkUE != NULL)
	{
		if(rkMessage.m_lParam2 > 0)
			pkUE->SetMouseType(true);
		else if (rkMessage.m_lParam2 < 0)
			pkUE->SetMouseType(false);
		pkUE->Show();
	}
}

//void UE_UiEffectManager::_event_LostJewelleryRound(MG_MessageBase& rkMessage)
//{
//    long lAreaIndex = min(max(rkMessage.m_lParam1, 0), 4);
//
//    FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();
//
//    int iFxID = rkFxManager.CreateFx(LOST_JEWELLERY_ROUND_FX_NAMES[lAreaIndex], true);
//    rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, rkMessage.m_lParam2 << 28);
//}
//
//void UE_UiEffectManager::_event_LostJewelleryState(MG_MessageBase& rkMessage)
//{
//    FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();
//
//    int iFxID = rkFxManager.CreateFx(LOST_JEWELLERY_STATE_FX_NAME, true);
//    rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, rkMessage.m_lParam1 << 28);
//}

int UE_UiEffectManager::_playUeFxOnActor(long lActorID, const StringType& strFxName, int64_t iFxExtParam, int eSlot)
{
     T_ASSERT(eSlot >= 0 && eSlot <= 5);
 
     GD_ModelBase* pkModel = IM_Manager::GetSingleton().GetModel(lActorID);
     if (pkModel == NULL)
     {
       return -1;
     }

     const NiPoint3& kDummyPos = pkModel->GetFxDummyNodePosition(GD_ModelBase::FDT_HEAD);
   
     FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();
    
	 int iFxID = rkFxManager.CreateFx(strFxName, true);
     rkFxManager.BindFxToLocation(iFxID, kDummyPos, NULL, &(SLOT_XFORM[eSlot]));
     rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME, iFxExtParam);
	 return iFxID;
}
void UE_UiEffectManager::_playUeFxOnActorWithoutDummy(long lActorID, const StringType& strFxName)
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(lActorID);
	if(pkVisibleData)
	  	 pkVisibleData->AttachFx(GD_ClientActorModel::FDT_INVALID, FileSystem::MTF_ModelData::AFBM_TRANSLATE_ROTATE, false, strFxName);

}

bool UE_UiEffectManager::GetPlaceHolderNameRect(long lTargetID, GUISys::GUIRect& kRetNameRect)
{
	return m_kNameArrange.GetNameRect(lTargetID, kRetNameRect);
}

bool UE_UiEffectManager::InsertPlaceHolderNameRect(long lTargetID, const GUISys::GUIPoint2& kStartPosition, GUISys::GUIRect& kRetNameRect)
{
	kRetNameRect.SetPosition(kStartPosition.x, kStartPosition.y);
	kRetNameRect.SetWidth(g_settings.UE_NAME_BG_WIDTH);
	kRetNameRect.SetHeight(g_settings.UE_NAME_BG_HEIGHT);

	m_kNameArrange.InsertNameRect(lTargetID, kRetNameRect);
	return true;
}

void UE_UiEffectManager::DeletePlaceHolderName(long lTargetID)
{
	m_kNameArrange.DeleteNameRect(lTargetID);
}

bool UE_UiEffectManager::GetLocalPlaceHolderNameRect(long lTargetID, GUISys::GUIRect& kRetNameRect)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(lTargetID);
	if(pkUE)
	{
		return pkUE->GetLocalNameRect(lTargetID, kRetNameRect);
	}
	return false;
}

bool UE_UiEffectManager::SetNameRect(long lTargetID, const GUISys::GUIRect& kNameRect)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(lTargetID);
	if(pkUE)
	{
		pkUE->SetNameRect(kNameRect);
		return true;
	}
	return false;
}

UT_Vec2Int UE_UiEffectManager::GetPlayerHight(object_id_type lTargetID)
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(lTargetID);
	if ( pkUE )
	{
		return UT_Vec2Int(int(pkUE->GetScreentPos().x), int(pkUE->GetScreentPos().y));
	}
	return UT_Vec2Int();
}

void UE_UiEffectManager::OnResolutionChanged(int iWidth, int iHeight)
{
	//用一条近似的直线计算，线性规划偏移量
	//float dx = 230+200;
	//float dy = 247+140;
	//SLOT_XFORM_COMBO.m_Translate.x = g_settings.DS_COMBO_XPOS + dx * (iWidth-1024.0f) / (1920.0f-1024.0f);         
	//SLOT_XFORM_COMBO.m_Translate.z = g_settings.DS_COMBO_YPOS + dy * (iHeight -768.0f) / (1200.0f - 768.0f);
	//SLOT_XFORM_COMBO.m_fScale = 0.7f * iHeight / 768.0f;
}

void UE_UiEffectManager::_event_NameRedraw( MG_MessageBase& rkMessage )
{
	UE_PlayerName* pkUE = m_kPlayerNamePool.GetUiEffect(rkMessage.m_lParam1);
	if(NULL != pkUE)
	{
		LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		UT_Vec3Float kNameColor;
		pkUE->CalCharName(pkGamePlayer, pkWorldManager, pkLogicManager, kNameColor);
		pkUE->RequestRedraw();
		pkUE->Update(0.0f);
	}
}
