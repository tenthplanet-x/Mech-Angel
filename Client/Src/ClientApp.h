#ifndef _CLIENT_APP_H
#define _CLIENT_APP_H

#include "BS_ClientModelContentManager.h"
#include "BS_ClientMaterialManager.h"
#include "RG_RenderManager.h"
#include "SE_SceneManager.h"
#include "SM_ClientStateFactory.h"
#include "SM_ClientControllerFactory.h"
#include "TM_ClientTimeManager.h"
#include "IP_InputManager.h"
#include "CA_CameraManager.h"
#include "NW_ClientNetManager.h"
#include "NW_ClientSessionManager.h"
#include "UI_UIManager.h"
#include "UI_AvatarManager.h"
#include "UE_UiEffectManager.h"
#include "LC_MapManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_LogicAreaManager.h"
#include "LC_LogicTriggerManager.h"
#include "LC_TooltipManager.h"
#include "LC_ClientTaskManager.h"
#include "LC_ClientShopCity.h"
#include "LC_ClientItemFactory.h"
#include "LC_NPCShopManager.h"
#include "LC_NPCTransferManager.h"
#include "LC_ClientActorOperationManager.h"

#include "FX_ClientFxManager.h" 
#include "SC_ClientScriptInterface.h"
#include "NW_Game2ClientSession.h"
#include "IM_Manager.h"
#include "TT_TextManager.h"
#include "UT_DebugCommandParser.h"
#include "CA_CameraPathAnimation.h"
#include "AS_ClientSoundManager.h"
#include "BL_AsyncOperationManager.h"
#include "LC_ClientDialogManager.h"
#include "LC_TooltipManager.h"
#include "LC_ClientMailManager.h"
#include "LC_ClientAuctionManager.h"
#include "LC_ClientGuildManager.h"
#include "GD_ClientBulletManager.h"
#include "UT_CutSceneManager.h"
#include "LC_ClientNPCInteractionManager.h"
#include "LC_FactionManager.h"
#include "LC_ActorFilterManager.h"
#include "LC_ClientArenaManager.h"
//#include "LC_ClientExchangeManager.h"
#include "LC_TitleManager.h"
//#include "LC_ClientCurrencyTradeManager.h"
#include "LC_ClientGuildHomeMgr.h"
#include "LC_ClientPackEntryFactory.h"
#include "LC_ConversationManager.h"
#include "LC_ClientMapTranscriptionManager.h"
#include "LC_ClientUICutScenesManager.h"
#include "LC_AutoFitNPCManager.h"
#include "LC_AchievementManager.h"
#include "SC_ScriptAction.h"
#include "UT_Curl.h"
#include "TT_TextFilter.h"
#include "LC_ClientNewBeeManager.h"
#include "LC_ClientShopAsset.h"
#include "LC_NoticeAnalysisMgr.h"
#include "ClientConfigManager.h"
#include "LC_ClientSoulRefineAsset.h"
#include "LC_SoulRefineAsset.h"
#include "LC_ClientToyPetManager.h"
#include "LC_ClientCollectionManager.h"
#include "LC_ClientBotManager.h"
#include "LC_ClientCombatRecordManager.h"
#include "LC_ClientGuildProfitManager.h"
#include "LC_ClientArenaPlayBackManager.h"
#include "LC_ClientReportManager.h"
#include "LC_HelperDiff.h"
#include "YunYingMgr.h"
#include "UT_Timer.h"
#include "BM_Manager.h"

class GD_ClientAsyncModelManager;
class PF_ClientPathManager;
class RG_ClientPointLightManager;
class RG_SplashWindow;
class GD_DynamicCollisionManager;
class IP_ShortcutKeyCodeMgr;
class SM_WalkTransferManager;

namespace GameLogic
{
	class LC_GuildRaidDataManager;
}
namespace Utility
{
	class UT_Pick_Manager;
}


class ClientApp : public Memory::MM_BaseObject,
	public Utility::UT_Singleton<ClientApp>
{
public:
	ClientApp();
	~ClientApp();

	bool CreateSplashWindow(HWND hWnd, HINSTANCE hInst);	//????????????????????????????????????????????????
	RG_SplashWindow * GetSplashWindow(void) { return m_pkSplash; }
	bool Init(HWND hWnd, HINSTANCE hInst);
	void InitRun();
	void InitComplete();
	static unsigned int __stdcall AsyncInitThread(void *p);
	
	bool UnInit();

	bool Start(void);
	bool Run();

	void ReqestQuit();

	void HideWindow();
	bool ProcessWindowsMsg( UINT msg, WPARAM wparam, LPARAM lparam, bool isDectectActiveWindow = true );

	RG_SplashWindow*                    m_pkSplash;
	bool								m_bInitlizing;

protected:

	bool _initEngineBase();
	bool _initRenderModule();

	bool _initGamebryo();
	bool _uninitGameBryo();

	void _initPointers();
	bool _initManagers();
	bool _uninitManger();

	bool _registerMessageHandler();

private:
	/*
	!! ???? !!

	??????????????????????????????????
	(1) ?? _initPointers() ????????????????
	(2) ?? _initManagers() ????????????
	(3) ?? _uninitManger() ????????????
	????????????????????????????????????????
	*/

	// GB Memory Manager
	static NiAllocator*                 s_pkNiMemTracker;
	static NiInitOptions*               s_pkNiInitOptions;

	NiWindowRef							m_hWnd;
	HINSTANCE							m_hInst;
	bool                                m_bWindowsActive;
	HANDLE								m_hInitThread;	//< ??????????????????

	//LC_HelperDiff????
	LC_HelperDiffManager*				m_pkHelperDiffManager;

	// ????????
	UT_LogManager*						m_pkLogManager;
	// ????????????
	Utility::UT_DirConfig*				m_pkDirConfig;
	// ????????
	Utility::UT_ConfigManager*			m_pkConfigManager;
	// RTTI ???? 
	Utility::UT_ClassRegistry*			m_pkClassRegistry;
	// ????????
	Utility::LS_LuaStateFactory*		m_pkLuaStateFactory;
	SC_ClientScriptInterface*			m_pkScriptInterface;
	// ????????
	Utility::UT_ProfileManager*			m_pkProfileManager;
	// ????
	PF_ClientPathManager*				m_pkPathManager;

	// ????????
	Terrain::TS_WorldConfig*			m_pkWorldConfig;

	// ????????
	BS_ClientModelContentManager*		m_pkModelContentManager;

	// ????????
	BS_ClientMaterialManager*			m_pkMaterialManager;

	BM_Manager*							m_pkBreakModelManager;
	// ????????????????
	SM_ClientStateFactory*				m_pkStateFactory;
	// ??????????
	StateMachine::SM_ClientControllerFactory* m_pkControllerFactory;

	// ????????
	MG_MessageCenter*					m_pkMessageCenter;

	// ????????
	IP_InputManager*					m_pkInputManager;
	IP_ShortcutKeyCodeMgr*				m_pkShortcutKeyCodeMgr;

	// ????????
	NW_ClientNetManager*				m_pkNetManager;
	NW_ClientSessionManager*			m_pkSessionManager;

	// ??????????
	TM_ClientTimeManager*				m_pkTimer;
	UT_TimerManager*					m_pkTimerManager;

	// ??????????
	RG_RenderManager*					m_pkRenderManager;

	// ????????
	CA_CameraManager*					m_pkCameraManager ;
	// ????????????
	Camera::CA_CameraAnimationManager*  m_pkCameraAnimManager;

	// ????????
	SE_SceneManager*					m_pkSceneManager;
	RG_ClientPointLightManager*         m_pkPointLightManager;

	// UI ????
	UI_UIManager*						m_pkUIManager;
	// UI Avatar ????
	UI_AvatarManager*					m_pkAvatarManager;
	// UI Effect ????
	UE_UiEffectManager*					m_pkUiEffectManager;
	// UI map ????
	LC_MapManager*						m_pkMapMananger;

	//????????????
	LC_ClientNewBeeManager*				m_pkNewBeeManager;

	// ????????????
	LC_ClientLogicManager*				m_pkLogicManager;

	GD_ClientAsyncModelManager*         m_pkAsyncModelManager;
	// ????????????????????
	IM_Manager*							m_pkIMManager;
	GD_DynamicCollisionManager*         m_pkDynamicCollisionManager;

	// ??????????
	GD_ClientBulletManager*             m_pkBulletManager;
	// ????????????????????
	Utility::UT_Pick_Manager*			m_pkPickMeshManager;

	// ????????
	FX_ClientFxManager*					m_pkFXManager;

	// ????????
	Skill::SK_Factory*					m_pkSkillFactory;

	// ????????????????
	UT_DebugCommandParser*              m_pkDebugCmdParser;

	//????????
	LC_ClientItemFactory*				m_pkItemFactory;

	//????????
	LC_ClientTaskManager*				m_pkTaskManager;

	// ID????
	GameLogic::LC_LogicIDManager*		m_pkLogicIDManager;

	// ????????????
	TT_TextManager*						m_pkTextManager;

	// ????????????
	GameLogic::LC_LogicMapManager*		m_pkLogicMapManager;

	// ??????????????
	GameLogic::LC_LogicTriggerManager*  m_pkLogicTriggerManager;

	// ????????
	AS_ClientSoundManager*				m_pkSoundManager;
	// ????	
	LC_ClientShopCity*					m_pkClientShopCity;
	// ????????
	BL_AsyncOperationManager*           m_pkBackLoadingProcess;

	TT_TextFilter*						m_pkTextFilter;

	LC_ClientDialogManager*				m_pkDialogManager;
	//tooltip
	LC_TooltipManager*					m_pkTooltipManager;

	LC_NPCShopManager*					m_pkNPCShopManager;

	LC_ConversationManager*				m_pkConversationManager;

	LC_ClientMapTranscriptionManager*	m_pkMapTranscriptionManager;

	LC_ClientUICutScenesManager*		m_pkUICutScenesManager;

	LC_NPCTransferManager*				m_pkNPCTransferManager;

	// ????????
	LC_ClientMailManager*				m_pkMailManager;

	// ????
	LC_ClientAuctionManager*			m_pkAuctionManager;

	// ????
	LC_ClientGuildManager*				m_pkGuildManager;

	//????????
	LC_ClientGuildHomeMgr*				m_pkGuildHomeMgr;

	UT_CutSceneManager*                 m_pkCutSceneMgr;

	LC_ClientNPCInteractionManager*     m_pkNPCInteractionManager;

	//??????????
	LC_FactionManager*					m_pkFactionManager;

	//??????????????
	LC_ActorFilterManager*				m_pkActorFilterManager;

	//????????????
	LC_ClientArenaManager*				m_pkArenaManager;

	//????????
//	LC_ClientExchangeManager*           m_pkClientExchangeManager;

	////??????????????
	//LC_ClientCurrencyTradeManager*	m_pkClientCurrencyTradeManager;

	//????????
	LC_ClientActorOperationManager*		m_pkClientActorOperationManager;

	//????????
	LC_TitleManager*			m_pkTitleManger;

	LC_ClientPackEntryFactory*			m_pkPackEntryFactory;
	LC_VIPCmnManager*					m_pkVIPCmnManager;

	//????????????
	LC_CommonStatManager*				m_pkCommonStatManager;

	//??????????
	LC_TreasureChestsManager*			m_pkTreasureChestsManager;
	
	//ScriptAction.lua
	ScriptAction::SA_ActionManager*		m_pkScriptActionManager;

	//????????????????????
	AutoFitNPCManager*					m_pkAutoFitNPCManager;

	//????????????id??????
	GameLogic::LC_GuildRaidDataManager*	m_pkGuildRaidDataManager;

	LC_ClientShopManager*				m_pkCommonShopManager;

	//??????????
	LC_AchievementManager*				m_pkAchievementManager;

	//??????????
	SM_WalkTransferManager*				m_pkWalkTransferManager;

	//????????????
	LC_NoticeAnalysisMgr*				m_pkNoticeAnalysisManager;

	//????????????
	ClientConfigReloadManager*			m_pkConfigReloadManager;

	//????????
	LC_ClientSoulRefineManager*			m_pkSoulRefineManager;
	LC_SoulRefineDataAsset*				m_pkSoulRefineDataAsset;

	//????????
	LC_ClientShotActivityManager*		m_pkShotActivityManager;

	//????????????
	LC_YunYingManager*					m_pkYunYingManager;

	//????????????
	LC_ClientActivityManager*			m_pkActivityManager;

	//??????
	LC_MeltDataManager*					m_pkMeltDataManager;
	//????????
	LC_BoneSoulManager*					m_pkBoneSoulManager;
	//????????????
	LC_StarsRuneManager*				m_pkStarsRuneManager;
	//????????
	LC_MillionWeaponManager*			m_pkMillionWeaponManager;

	//????????
	LC_ClientToyPetManager*				m_pkPetManager;

	//????????
	LC_ClientCollectionManager*			m_pkCollectionManager;
	//??????????????
	LC_ClientBotManager*				m_pkBotManager;

	//????????????
	LC_ClientCombatRecordManager*		m_pkCombatRecordManager;

	//????????????
	CAchievementManager*				m_pkAchievementManager_Common;

	//????????????????????
	LC_ClientGuildProfitManager*		m_pkGuildProfitManager;

	//??????????????
	LC_ClientArenaPlayBackManager*		m_pkArenaPlayBackManager;

	LC_ClientReportManager*				m_pkReportManager;
};

#endif