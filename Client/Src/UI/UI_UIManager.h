#ifndef _UI_UIMANAGER_H
#define _UI_UIMANAGER_H

#include "ClientDefine.h"
#include "MG_MessageHandlerInterface.h"
#include "UI_SysImpl.h"
#include "UI_Cursor.h"
#include "LS_LuaFunction.h"
#include "UI_CallBackFunc.h"
#include "CF_LoadingTip.h"
#include "UT_WaveGenerator.h"
#include "UI_ColorManager.h"
#include "UI_NameFilterManager.h"
#include "UT_TimeUtil.h"

namespace GUISys
{
	class UT_UILayoutParse;
	class WT_RootWidget;
	class UI_Cursor;
	class IWgtData;
	class UI_ControlRelation;
	class UI_ColorManager;
	class UI_NameFilterManager;
}
//namespace Utility
//{
//	class UT_UIHelpSystem;
//}
class CWinVideo;
class UI_VideoTexture;
class UI_AvatarManager;
class UI_UIManager : public MG_MessageHandlerInterface,
					 public Memory::MM_BaseObject,
					 public Utility::UT_Singleton<UI_UIManager>
{
public:
	enum eUIStage
	{
		NONESCREEN,
		ANNOUNCESCREEN,
		LOGINSCREEN,
		SELSERVERSCREEN,
		SELCHARSCREEN,
		SELCHARSERVERSCREEN,
		CREATECHARSCREEN,
		GAMEUISCREEN,
		LOADINGSCREEN,
		CHANGESERVERSCREEN,
	};

	enum eUILoadStage
	{
		UI_LOAD_STAGE_NONE,
		UI_LOAD_STAGE_START_RELOADING,
	};

	UI_UIManager();
	virtual ~UI_UIManager();
	DECL_RTTI_NAME(UI_UIManager)

	void GameInit();//游戏初始化数据，预先加载处理。
	void GameUninit();
	bool Init(HWND hWnd);
	void PostInit();
	bool UnInit(void);

	void RegisterToLua();
	void Reset();

	void				InitWidgetRelation();
	void				LoadAllUIScript();
	void				UnLoadAllUIScript();
	void				SetWidgetRelation(const char* sWgtName);

	void UpdateAvatarTexture(const StringType& strGroupName,long lCharType,int iAvatarIndex, int iWeaponID, int iCameraMode);
	void UpdateVehicleTexture(const StringType& strGroupName,long lCharType, int iIndex, int iRenderModel);
	void UpdateWildPrincesBossTexture(const StringType& strGroupName,int iAvatarIndex, long lTypeID, float fCameraDist, float fTranslateX, float fTranslateZ, int iCameraMode);
	void UpdateTalkNpcTexture(const StringType& strGroupName,int iAvatarIndex, long lTypeID, int iCameraMode);
	void UpdateDevaEyeTexture(const StringType& strGroupName,long lCharType, int iIndex, int iCameraMode);
	void UpdateFabaoTexture(const StringType& strGroupName,long lCharType, int iIndex, int iCameraMode);
	void UpdateGodWingTexture(const StringType& strGroupName,long lCharType, int iIndex, int iCameraMode);
	void UpdateArenaBattleActor(const StringType& strGroupName, int iCid, int iIndex, int iCameraMode);
	void UpdateNonpareilItemAvatar(const StringType& strGroupName, long lCharType, int iCharIndex, float fTranslateX, float fTranslateY, float fTranslateZ, int iCameraMode);
	void UpdatePlatform(const StringType& strGroupName, int iIndex, long lCharType, int iCameraMode);
	void UpdateToyPet(const StringType& strGroupName, int iIndex, long lResID, int iCameraMode);
	void UpdateRankingWholeBodyAvatar(const StringType& strGroupName, long lActorCharType, long lCharType, int iAvatarIndex, int iCameraMode);
	void UpdateGreatKungFuRemainAvatar(const StringType& strGroupName, long lCharType, int iAvatarIndex, int iCameraMode);

	void UpdateHookBot(const StringType& strGroupName, const StringType& strChartypes);
	void UpdateHookBotMonster(const StringType& strGroupName, const StringType& strChartypes);
	void UpdateHookBotBoxScene(const StringType& strGroupName, long lCharType, int iCharIndex);

	void ActiveAvatarGroup(const StringType& strGroupName, bool isActive);

	void RegisterAvatarManager(UI_AvatarManager* avatarManager);
	void UnRegisterAvatarManager();

	//播放角色动画
	void PlayCharAnimation(long lCharType, int iIndex, int iCameraMode, const StringType& sAnimName, bool bForce, int iLoopNum = 1);
	//播放特效
	void AttachFxOnAvatarModel(long lCharType, int iIndex, int iCameraMode, const StringType& sFxName, float fx, float fy, float fz);
	//bool SetWorldMapRoadLineColor(int iLineIdx, float fColorRed, float fColorGreen, float fColorBlue);
	void DetachFxOnAvatarModel(long lCharType, int iIndex, int iCameraMode, const StringType& sFxName);

	//视频播放
	bool		PlayNilVideo(int iVideoType);
	bool		PlayVideo(int iVideoType, const char *cpImagename, const char *cpFilename, unsigned uLoopCount = 1);
	void		EndVideo(int iVideoType);

	//开启全屏淡入淡出
	void		ScreenFadeIn(UINT srcColor, UINT dstColor, float keepTime, float rampTime);
	void		ScreenFadeOut(UINT srcColor, UINT dstColor, float rampTime, float keepTime);

	//重置鼠标指针
	bool		OnResetMouseLogicType(void);
	//---------------------------------------
	void				GameUIResolutionChange(int nWidth, int nHeight);
	void				GameAvatarGraphicSettingChange(int nWidth, int nHeight);

	size_t				GetAllImgAllocationSize() const;

	virtual bool		RecvMessage(MG_MessageBase& rkMessage);
	GUISys::UI_SysImpl *GetCUISysImpl(){ return m_pkUIImpl;}

	void				HandlerGameEvent(GameLogic::LC_GameEventBase& rGameEvent);
	void				HandlerMsgGameEvent(int iMsgType, const StringType& str);

	void				SetLoadingProcess(bool b) { m_bLoadingProcess = b; }
	bool				GetLoadingProcess() { return m_bLoadingProcess; }

	bool				GetResetFlag() { return m_bReset; }
	void				SetResetFlag(bool b) { m_bReset = b; }
	//---------------------------------------
	void ShowGameMessage(const StringType& rkGameMessage);
	void GoToAnnounceScreen();
	void GoToLoginScreen();
	void GoToSelServerScreen();
	void GoToSelCharScreen();
	void GotoSelCharServerScreen();
	void GoToCreateCharScreen();
	void GoToGameScreen();
	void GoToLoadingScreen();
	void GotoChangeServerScreen();
	bool IsLoadingScreen();
	bool IsGameScreen();
	void SetUIMouseType(int iUIType);
	void SetUIMouseLogicType(int iLogicType);//default logic type
	int  GetUIMouseLogicType();
	void UpdateLoadingPercent(size_t nLoadNum);
	void SetLoadingTimeOut();
	int  GetLoadingPercent();

	

    int PlaySound(const char *pkPath);
	int PlaySound(int iType);
	int  PlayEventSound(StringType& strSoundName, int iPlayCount, float fPlayTime );
	void StopAudioSource(int iSoundID);
	void StopAudioSourceFading(int iSoundID, float fadeTime);
	void RedrawAllWidget() const;
	unsigned int	 GetResolutionCount();
	const char *GetResolution(unsigned int iIndex);

    void SetCurrentScreenVisibility(bool bVisible);
	const char * GetSoundName(int iType);

	//LoadingTip
	StringType GetLoadingTip();
	//StringType GetLoadingImage();
	void SetWorldMapID(int iWorldMapID);
	long GetWorldMapID(){ return m_lWorldMapID;}
	
	void SetHandleMouseMessage(bool bHandleMouseMessage){m_bHandleMouseMessage = bHandleMouseMessage;};
	
	void ResetUIRoot();//递归重置所有UI状态

	StringType RemoveRichInfo(const StringType &sRichText);//移出XML信息，只留下文本信息
	int GetLoadState(){return m_eLoadState;}
	void SetLoadState(int eLoadState){m_eLoadState = eLoadState;}

	bool IsFirstHappyPageVisiable();

	void NotifyRestoreEasyModelUI();
	void NotifyLButtonUp();
	void NotifyReportDownLoadOver();
	void SetEasyModeTic(float tt1);
	bool IsEasyModel();
	void ResetEasyModelTick();

	void SetHookProfitTic(float tt1);
	bool IsHookProfit();
	void ResetHookProfitTick();

protected:
	void _event_InputMouseLButtonDown(MG_MessageBase& rkMessage);
	void _event_InputMouseLButtonUp(MG_MessageBase& rkMessage);
	void _event_InputMouseLButtonDBClik(MG_MessageBase& rkMessage);
	void _event_RedrawAllWidget(MG_MessageBase& rkMessage);
	void _event_InputMouseRButtonDown(MG_MessageBase& rkMessage);
	void _event_InputMouseMove(MG_MessageBase& rkMessage);
	void _event_InputMouseWheel(MG_MessageBase& rkMessage);
	void _event_InputKeyF11(MG_MessageBase& rkMessage);
	void _event_TimerTick(MG_MessageBase& rkMessage);
	void _event_Win32Msg(MG_MessageBase& rkMessage);
	void _event_MapChange(MG_MessageBase& rkMessage);
	void _event_ReloadAllUIScript(MG_MessageBase& rkMessage);
	void _changeCursor(MouseLogicModeType eCursorType);	
	void _setPercent(int iPercent);
	void _initsoundPath();
	void _initLoadingTip();
	void _eraseTip(CSVFile::CF_LoadingTip::DataEntry* pEntry);

	void _update_MapChanged_Hide(int MapID);
	
	HWND						m_hWnd;
	GUISys::UI_SysImpl*			m_pkUIImpl;
	//Utility::UT_UIHelpSystem*	m_pkHelpSystem;
	float				m_fStartPrintLogicTipTime;
	bool				m_bLogicTipVisible;
	MouseLogicModeType	m_eUIMouseLogicType;
	MouseLogicModeType	m_eUIMouseType;
	MouseLogicModeType	m_eLastUIMouseType;
	UI_AvatarManager*	m_pkAvatarManager;
	size_t				m_nLastLoadNum;
	size_t				m_nTotalLoadNum;
	int					m_iPercent;
	//eUIStage			m_CurStage;
	eUIStage			m_NextStage;

	Utility::LS_LuaFunction<int>* m_pkEventFunc;
	NiTObjectArray<NiFixedString> m_kResolutionList;
	GUISys::UT_UILayoutParse		*m_pLayoutObj;
	mem::vector<StringType>		m_kSoundName;

	long				m_lWorldMapID;
	
	mem::vector<CSVFile::CF_LoadingTip::DataEntry*> m_kUnUsedTipVector;//未使用的loadingtip
	mem::vector<CSVFile::CF_LoadingTip::DataEntry*> m_kUsedTipMap;//使用过的tip

	bool				m_bHandleMouseMessage;
	GUISys::UI_ControlRelation	*m_pUIControlRelation;
	GUISys::UI_ColorManager	*m_pUIColorManager;
	GUISys::UI_NameFilterManager *m_pUINameFilterManager;

	//视频播放
	UI_VideoTexture		*m_pkVideoTexture[VIDEOTYPE_COUNT];
	CWinVideo			*m_pkVideoPlayer[VIDEOTYPE_COUNT];

	//全屏淡入淡出
	GUISys::GUIColor		m_kFIFOSrcColor;
	GUISys::GUIColor		m_kFIFODstColor;
	CRampStepWaveGenerator	m_kFIFOController;

	//ui系统加载状态
	int						m_eLoadState;

	bool					m_bLoadingProcess;
	bool					m_bReset;

	Utility::TickCount				m_easyModelT;
	Utility::TickCount				m_hookProfit;
};

namespace Utility
{
	bool onBeginDownloadUIResources(const char* uiKey);
	bool onEndDownloadUIResources(const char* uiKey);
}

#endif
