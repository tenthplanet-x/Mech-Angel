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

	bool CreateSplashWindow(HWND hWnd, HINSTANCE hInst);	//用于遮掩主线程初始化过程，加载资源，需要尽早显示
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
	!! 必读 !!

	每当增删管理对象指针变量时，勿忘：
	(1) 在 _initPointers() 内置空对象指针；
	(2) 在 _initManagers() 中创建对象；
	(3) 在 _uninitManger() 中销毁对象！
	并保持各对象指针的顺序，避免遗漏或重复。
	*/

	// GB Memory Manager
	static NiAllocator*                 s_pkNiMemTracker;
	static NiInitOptions*               s_pkNiInitOptions;

	NiWindowRef							m_hWnd;
	HINSTANCE							m_hInst;
	bool                                m_bWindowsActive;
	HANDLE								m_hInitThread;	//< 异步初始化线程句柄

	//LC_HelperDiff管理
	LC_HelperDiffManager*				m_pkHelperDiffManager;

	// 日志管理
	UT_LogManager*						m_pkLogManager;
	// 资源目录管理
	Utility::UT_DirConfig*				m_pkDirConfig;
	// 配置管理
	Utility::UT_ConfigManager*			m_pkConfigManager;
	// RTTI 管理 
	Utility::UT_ClassRegistry*			m_pkClassRegistry;
	// 脚本管理
	Utility::LS_LuaStateFactory*		m_pkLuaStateFactory;
	SC_ClientScriptInterface*			m_pkScriptInterface;
	// 性能测试
	Utility::UT_ProfileManager*			m_pkProfileManager;
	// 寻路
	PF_ClientPathManager*				m_pkPathManager;

	// 世界描述
	Terrain::TS_WorldConfig*			m_pkWorldConfig;

	// 模型管理
	BS_ClientModelContentManager*		m_pkModelContentManager;

	// 材质管理
	BS_ClientMaterialManager*			m_pkMaterialManager;

	BM_Manager*							m_pkBreakModelManager;
	// 游戏内状态元管理
	SM_ClientStateFactory*				m_pkStateFactory;
	// 状态机管理
	StateMachine::SM_ClientControllerFactory* m_pkControllerFactory;

	// 消息中心
	MG_MessageCenter*					m_pkMessageCenter;

	// 输入管理
	IP_InputManager*					m_pkInputManager;
	IP_ShortcutKeyCodeMgr*				m_pkShortcutKeyCodeMgr;

	// 网络管理
	NW_ClientNetManager*				m_pkNetManager;
	NW_ClientSessionManager*			m_pkSessionManager;

	// 时间调动器
	TM_ClientTimeManager*				m_pkTimer;
	UT_TimerManager*					m_pkTimerManager;

	// 渲染器管理
	RG_RenderManager*					m_pkRenderManager;

	// 相机管理
	CA_CameraManager*					m_pkCameraManager ;
	// 相机动画管理
	Camera::CA_CameraAnimationManager*  m_pkCameraAnimManager;

	// 场景管理
	SE_SceneManager*					m_pkSceneManager;
	RG_ClientPointLightManager*         m_pkPointLightManager;

	// UI 管理
	UI_UIManager*						m_pkUIManager;
	// UI Avatar 管理
	UI_AvatarManager*					m_pkAvatarManager;
	// UI Effect 管理
	UE_UiEffectManager*					m_pkUiEffectManager;
	// UI map 管理
	LC_MapManager*						m_pkMapMananger;

	//地图分流管理
	LC_ClientNewBeeManager*				m_pkNewBeeManager;

	// 游戏逻辑管理
	LC_ClientLogicManager*				m_pkLogicManager;

	GD_ClientAsyncModelManager*         m_pkAsyncModelManager;
	// 游戏内可交互模型管理
	IM_Manager*							m_pkIMManager;
	GD_DynamicCollisionManager*         m_pkDynamicCollisionManager;

	// 子弹管理器
	GD_ClientBulletManager*             m_pkBulletManager;
	// 子弹用的场景碰撞数据
	Utility::UT_Pick_Manager*			m_pkPickMeshManager;

	// 特效管理
	FX_ClientFxManager*					m_pkFXManager;

	// 技能管理
	Skill::SK_Factory*					m_pkSkillFactory;

	// 调试器命令解析器
	UT_DebugCommandParser*              m_pkDebugCmdParser;

	//物品管理
	LC_ClientItemFactory*				m_pkItemFactory;

	//任务管理
	LC_ClientTaskManager*				m_pkTaskManager;

	// ID管理
	GameLogic::LC_LogicIDManager*		m_pkLogicIDManager;

	// 提示文字管理
	TT_TextManager*						m_pkTextManager;

	// 逻辑区域管理
	GameLogic::LC_LogicMapManager*		m_pkLogicMapManager;

	// 逻辑触发器管理
	GameLogic::LC_LogicTriggerManager*  m_pkLogicTriggerManager;

	// 音效管理
	AS_ClientSoundManager*				m_pkSoundManager;
	// 商城	
	LC_ClientShopCity*					m_pkClientShopCity;
	// 后台加载
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

	// 邮件资产
	LC_ClientMailManager*				m_pkMailManager;

	// 拍卖
	LC_ClientAuctionManager*			m_pkAuctionManager;

	// 门派
	LC_ClientGuildManager*				m_pkGuildManager;

	//门派家园
	LC_ClientGuildHomeMgr*				m_pkGuildHomeMgr;

	UT_CutSceneManager*                 m_pkCutSceneMgr;

	LC_ClientNPCInteractionManager*     m_pkNPCInteractionManager;

	//阵营管理器
	LC_FactionManager*					m_pkFactionManager;

	//目标过滤管理器
	LC_ActorFilterManager*				m_pkActorFilterManager;

	//竞技场管理器
	LC_ClientArenaManager*				m_pkArenaManager;

	//兑换系统
//	LC_ClientExchangeManager*           m_pkClientExchangeManager;

	////游戏币交易系统
	//LC_ClientCurrencyTradeManager*	m_pkClientCurrencyTradeManager;

	//操作管理
	LC_ClientActorOperationManager*		m_pkClientActorOperationManager;

	//称号管理
	LC_TitleManager*			m_pkTitleManger;

	LC_ClientPackEntryFactory*			m_pkPackEntryFactory;
	LC_VIPCmnManager*					m_pkVIPCmnManager;

	//活动奖励管理
	LC_CommonStatManager*				m_pkCommonStatManager;

	//开宝箱管理
	LC_TreasureChestsManager*			m_pkTreasureChestsManager;
	
	//ScriptAction.lua
	ScriptAction::SA_ActionManager*		m_pkScriptActionManager;

	//自适应等级刷怪管理器
	AutoFitNPCManager*					m_pkAutoFitNPCManager;

	//公会副本活动id管理器
	GameLogic::LC_GuildRaidDataManager*	m_pkGuildRaidDataManager;

	LC_ClientShopManager*				m_pkCommonShopManager;

	//成就管理器
	LC_AchievementManager*				m_pkAchievementManager;

	//跨地图寻路
	SM_WalkTransferManager*				m_pkWalkTransferManager;

	//公告解析管理
	LC_NoticeAnalysisMgr*				m_pkNoticeAnalysisManager;

	//热更新管理器
	ClientConfigReloadManager*			m_pkConfigReloadManager;

	//炼魂管理
	LC_ClientSoulRefineManager*			m_pkSoulRefineManager;
	LC_SoulRefineDataAsset*				m_pkSoulRefineDataAsset;

	//开服冲榜
	LC_ClientShotActivityManager*		m_pkShotActivityManager;

	//运营平台管理
	LC_YunYingManager*					m_pkYunYingManager;

	//新版活动管理
	LC_ClientActivityManager*			m_pkActivityManager;

	//炼化炉
	LC_MeltDataManager*					m_pkMeltDataManager;
	//命骨管理
	LC_BoneSoulManager*					m_pkBoneSoulManager;
	//星辰符文管理
	LC_StarsRuneManager*				m_pkStarsRuneManager;
	//百兵管理
	LC_MillionWeaponManager*			m_pkMillionWeaponManager;

	//玩具管理
	LC_ClientToyPetManager*				m_pkPetManager;

	//收藏管理
	LC_ClientCollectionManager*			m_pkCollectionManager;
	//挂机机器人管理
	LC_ClientBotManager*				m_pkBotManager;

	//战斗记录管理
	LC_ClientCombatRecordManager*		m_pkCombatRecordManager;

	//新成就管理器
	CAchievementManager*				m_pkAchievementManager_Common;

	//公会礼物领取记录管理
	LC_ClientGuildProfitManager*		m_pkGuildProfitManager;

	//竞技场回放管理
	LC_ClientArenaPlayBackManager*		m_pkArenaPlayBackManager;

	LC_ClientReportManager*				m_pkReportManager;
};

#endif