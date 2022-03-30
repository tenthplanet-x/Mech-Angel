#pragma once

#include "UT_Singleton.h"
#include "MG_MessageHandlerInterface.h"
#include "MG_MessageBase.h"
#include "UE_UiEffectPoolBase.h"
#include "UE_PlayerName.h"
#include "UE_PlayerBlood.h"
#include "UE_NPCBlood.h"
#include "UE_NameArrange.h"

class UE_UiEffectManager :	public Utility::UT_Singleton<UE_UiEffectManager>,
							public MG_MessageHandlerInterface,	
							public Memory::MM_BaseObject
{
public:
	UE_UiEffectManager();
	~UE_UiEffectManager();

	bool Init();
	bool UnInit();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	void GetVisibleArrary(NiVisibleArray& rkVisibleArray);

	void ClearAll();

	void ClearNameArrange();

	bool InsertPlaceHolderNameRect(long lTargetID, const GUISys::GUIPoint2& kStartPosition, GUISys::GUIRect& kRetNameRect);
	void DeletePlaceHolderName(long lTargetID);
	bool GetPlaceHolderNameRect(long lTargetID, GUISys::GUIRect& kRetNameRect);
	bool GetLocalPlaceHolderNameRect(long lTargetID, GUISys::GUIRect& kRetNameRect);
	bool SetNameRect(long lTargetID, const GUISys::GUIRect& kNameRect);

	UT_Vec2Int GetPlayerHight(object_id_type lTargetID);

	void OnResolutionChanged(int iWidth, int iHeight);

	static int		m_iActiveLevel;
	static bool		m_bActiveLevelChange;

private:
	void _event_TimerTick(MG_MessageBase& rkMessage);
	void _event_InputKeyAltPressed(MG_MessageBase& rkMessage);

	void _event_PlayerHPNormalCost(MG_MessageBase& rkMessage);
	void _event_PlayerHPCriticalCost(MG_MessageBase& rkMessage);
	void _event_NPCHPNormalCost(MG_MessageBase& rkMessage);
	void _event_NPCHPCriticalCost(MG_MessageBase& rkMessage);
	void _event_CommanderLevelUp(MG_MessageBase& rkMessage);

	void _event_PlayerCountPoint(MG_MessageBase& rkMessage);
	void _event_PlayerGameStoryTrials(MG_MessageBase& rkMessage);
	void _event_PlayerMiss(MG_MessageBase& rkMessage);
	void _event_NPCMiss(MG_MessageBase& rkMessage);
	void _event_PlayerStateChange(MG_MessageBase& rkMessage);

	void _event_PlayerJumpDodge(MG_MessageBase& rkMessage);

	void _event_EnterFight(MG_MessageBase& rkMessage);
	void _event_LeaveFight(MG_MessageBase& rkMessage);

	void _event_DeltaExp(MG_MessageBase& rkMessage);
	void _event_DeltaHp(MG_MessageBase& rkMessage);
	void _event_DeltaMp(MG_MessageBase& rkMessage);
	void _event_DeltaSp(MG_MessageBase& rkMessage);
	void _event_DeltaAttr(MG_MessageBase& rkMessage);
	
	void _event_ShowPlayerHeadALL(MG_MessageBase& rkMessage);
	void _event_ShowPlayerName(MG_MessageBase& rkMessage);
	void _event_HidePlayerName(MG_MessageBase& rkMessage);
	void _event_ResetTimePlayeName(MG_MessageBase& rkMessage);
	void _event_SetDropItemTime(MG_MessageBase& rkMessage);
	void _event_ChangePlayerTitle(MG_MessageBase& rkMessage);
	void _event_UpdateNPCName(MG_MessageBase& rkMessage);

	void _event_ShowDropItemName(MG_MessageBase& rkMessage);

	void _event_ShowPlayerBlood(MG_MessageBase& rkMessage);
	void _event_HidePlayerBlood(MG_MessageBase& rkMessage);
	void _event_UpdatePlayerBlood(MG_MessageBase& rkMessage);

	void _event_ShowNPCBlood(MG_MessageBase& rkMessage);
	void _event_HideNPCBlood(MG_MessageBase& rkMessage);
	void _event_UpdateNPCBlood(MG_MessageBase& rkMessage);

	void _event_NameRedraw(MG_MessageBase& rkMessage);

    //void _event_LostJewelleryRound(MG_MessageBase& rkMessage);
    //void _event_LostJewelleryState(MG_MessageBase& rkMessage);

	void _event_ChangeNameFrameState(MG_MessageBase& rkMessage);
	void _event_PlaceHolderNameRedraw(MG_MessageBase& rkMessage);
	void _event_SetInteractiveBtnVisible(MG_MessageBase& rkMessage);
	void _event_RemoveDisplayChest(MG_MessageBase& rkMessage);

	void _event_PickGold(MG_MessageBase& rkMessage);
	void _event_PickUnbindYuanbao(MG_MessageBase& rkMessage);
	void _event_PickSP(MG_MessageBase& rkMessage);
	void _event_ShowCombo(MG_MessageBase& rkMessage);
	void _event_DeltaCombo(MG_MessageBase& rkMessage);
	void _event_FinishTask(MG_MessageBase& rkMessage);
	void _event_FailTask(MG_MessageBase& rkMessage);
	void _event_VipLevelup(MG_MessageBase& rkMessage);
	void _event_TaskChapterOpen(MG_MessageBase& rkMessage);
	void _event_VipNewLevelup(MG_MessageBase& rkMessage);
	

	int _playUeFxOnActor(long lActorID, const StringType& strFxName, int64_t iFxExtParam, int eSlot);
	void _playUeFxOnActorWithoutDummy(long lActorID, const StringType& strFxName);
	void _redrawPlaceHolderName(float fCurrentTime);
	void _resetPlayerNameTime();
	StringType _findStateFxName(long lStateType);
private:
	NiCullingProcess							m_kCuller;

	UE_UiEffectPoolBase<UE_PlayerName>			m_kPlayerNamePool;
	UE_UiEffectPoolBase<UE_PlayerBlood>			m_kPlayerBloodPool;
	UE_UiEffectPoolBase<UE_NPCBlood>			m_kNPCBloodPool;

	UE_NameArrange								m_kNameArrange;
	float										m_fNameArrangeCurrentTime;
	const float									m_fNameArrangeRedrawInterval;
	int											m_iLastComboFxID;
	int											m_iLastDeltaComboFxID;

    StringType PLAYER_NORMAL_HP_COST_FX_NAMES[5];
    StringType PLAYER_CRITICAL_HP_COST_FX_NAME;
    StringType OTHER_EXTERNAL_HP_COST_FX_NAMES[4];
	StringType BOW_EXTERNAL_HP_COST_FX_NAMES[4];
    StringType OTHER_INTERNAL_HP_COST_FX_NAMES[4];
    StringType OTHER_EXTERNAL_CRITICAL_HP_COST_FX_NAME;
    StringType OTHER_INTERNAL_CRITICAL_HP_COST_FX_NAME[4];
    StringType PLAYER_MISS_FX_NAME;
	StringType PLAYER_JUMP_DODGE_FX_NAME;
    StringType OTHER_MISS_FX_NAME;
	StringType QINGONG_MINUS_ONE;
	StringType COURAGE_POINT;
	StringType GAMESTORY_TRIALS;
    StringType PLAYER_GET_EXP_FX_NAME;
    StringType PLAYER_GET_HP_FX_NAMES[2];
    StringType PLAYER_GET_MP_FX_NAMES[2];
	StringType PLAYER_GET_SP_FX_NAMES[2];
	StringType PLAYER_GET_GOLD_FX_NAME;
	StringType PLAYER_SHOW_COMBO_FX_NAME[4];
	StringType PLAYER_SHOW_COMBO_FX_NAME_RED[4];
	StringType PLAYER_DELTA_COMBO_FX_NAME;
	StringType PLAYER_DELTA_ATTR_FX_NAME;

    NiTransform SLOT_XFORM[6];
    static const float SLOT_XLATE;

	NiTransform SLOT_XFORM_COMBO;

    StringType ENTER_LEAVE_FIGHT_FX_NAME;

	float tick;
	mem::list<MG_MessageBase> m_attrChange;

    // 落英回合提示
    //StringType LOST_JEWELLERY_ROUND_FX_NAMES[5];
    //StringType LOST_JEWELLERY_STATE_FX_NAME;
};