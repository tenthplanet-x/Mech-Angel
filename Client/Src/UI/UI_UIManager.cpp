#include "ClientPCH.h"
#include "ClientScriptPCH.h"
#include "AS_ClientSoundManager.h"
#include "SC_ClientScriptInterface.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayerManager.h"
#include "UI_SysImpl.h"
#include "UI_Cursor.h"
#include "UI_UIManager.h"
#include "UI_AvatarManager.h"
#include "WT_WidgetManager.h"
#include "WT_RootWidget.h"
#include "MG_MessageBase.h"
#include "UE_RenderManager.h"
#include "UE_UiEffectManager.h"
#include "UT_UILayoutParse.h"

#include "LS_LuaState.h"
#include "LS_LuaFunction.h"

#include "CF_LoadingTip.h"
#include "CF_Buddy.h"

#include "UT_CutSceneManager.h"
#include "IG_Material.h"
#include "UI_ControlRelation.h"
#include "RS_GUIRender.h"
#include "UT_UIHelpSystem.h"
#include "IG_ImageManager.h"

#include "AS_ClientSoundManager.h"

using namespace GUISys;
using namespace Script;
using namespace Utility;
using namespace CSVFile;
#include "ClientScriptPCH.h"
#include "Winvideo.h"
#include "UI_VideoTexture.h"
#include "UT_FileManagerBase.h"

#define DIRTY_FLAG_TIMEUPDATE1 1000
#define DIRTY_FLAG_TIMEUPDATE2 2000
#define DIRTY_FLAG_TIMEUPDATE3 3000

namespace Utility
{
	bool onBeginDownloadUIResources(const char* uiKey)
	{
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_BEGIN_DOWNLOAD_UI_RESOURCES);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(uiKey);
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		return true;
	}

	bool onEndDownloadUIResources(const char* uiKey)
	{
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_END_DOWNLOAD_UI_RESOURCES);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(uiKey);
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		return true;
	}
}

//---------------------------------------------------------------
UI_UIManager::UI_UIManager()
: m_pkUIImpl(NULL)
, m_hWnd(NULL)
, m_bLogicTipVisible(false)
, m_eUIMouseLogicType(MLMT_NORMAL)
, m_pkAvatarManager(NULL)
, m_nTotalLoadNum(0)
, m_nLastLoadNum(0)
, m_iPercent(-1)
, m_pkEventFunc(NULL)
, m_lWorldMapID(-1)
, m_bHandleMouseMessage(true)
, m_pUIControlRelation(NULL)
, m_pUIColorManager(NULL)
, m_pUINameFilterManager(NULL)
, m_eUIMouseType(MLMT_NONE)
, m_eLoadState(UI_LOAD_STAGE_NONE)
, m_bLoadingProcess(false)
, m_bReset(false)
, m_fStartPrintLogicTipTime(0.0f)
, m_eLastUIMouseType(MLMT_NONE)
, m_NextStage(NONESCREEN)
{
	m_kFIFOController.enable(false);
	//m_CurStage = NONESCREEN;
	m_pkUIImpl = T_NEW_D GUISys::UI_SysImpl();
	m_pLayoutObj = T_NEW_D GUISys::UT_UILayoutParse();
	m_kSoundName.reserve(UI_SOUND_MAXNUM);
	m_kSoundName.resize(UI_SOUND_MAXNUM);

	for (int i = 0; i < VIDEOTYPE_COUNT; ++i)
	{
		m_pkVideoTexture[i] = T_NEW_D UI_VideoTexture();
		m_pkVideoPlayer[i] = new CWinVideo();
	}

	SetUIMouseLogicType(MLMT_NORMAL);
	//m_pkHelpSystem = T_NEW_D UT_UIHelpSystem();
}

void UI_UIManager::GameInit()
{
	RegisterCallBackFuncs();
	m_pUIControlRelation = T_NEW_D UI_ControlRelation();
	m_pUIControlRelation->Init();
	m_pUIColorManager = T_NEW_D UI_ColorManager();
	m_pUIColorManager->Init();
	m_pUINameFilterManager = T_NEW_D UI_NameFilterManager();
	m_pUINameFilterManager->Init();
	m_pkUIImpl->GameInit();
	//m_pkHelpSystem->GenerateHelpFile();

	UT_FileManagerBase::GetSingleton().SetDownloadListenerCB(&Utility::onEndDownloadUIResources);
}


void UI_UIManager::GameUninit()
{
	//m_pkUIImpl->GameUninit();
}

//---------------------------------------------------------------
UI_UIManager::~UI_UIManager()
{
	for (int i = 0; i < VIDEOTYPE_COUNT; ++i)
	{
		T_DELETE_D m_pkVideoTexture[i];
		delete m_pkVideoPlayer[i];
	}
	::memset(m_pkVideoTexture, 0, sizeof(m_pkVideoTexture));
	::memset(m_pkVideoPlayer, 0, sizeof(m_pkVideoPlayer));

	T_DELETE_D m_pLayoutObj;
    T_DELETE_D m_pUIControlRelation;
	T_DELETE_D m_pUIColorManager;
	T_DELETE_D m_pUINameFilterManager;
	//T_DELETE_D m_pkHelpSystem;
}

//---------------------------------------------------------------
bool UI_UIManager::Init(HWND hWnd)
{
	WT_Widget::InitStringConsts();
	m_hWnd = hWnd;
	//::ShowCursor(false);

	::ShowCursor(GUISys::UI_Cursor::IsUseSystemCursor());

	m_hWnd = hWnd;

	NiRendererPtr spNirenderer = NiRenderer::GetRenderer();
	if(!spNirenderer)
		return false;

	spNirenderer->SetSafeZone(NiRect<float>(0.0f,1.0f,0.0f,1.0f));

	m_pkUIImpl->InitializeSys(m_hWnd);

	m_kResolutionList.RemoveAll();
	g_settings.GetResolutionList(m_kResolutionList);

    LoadAllUIScript();

	_initLoadingTip();
	LS_LuaState* pkLuaState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	LS_LuaObject* pkFuncObj = pkLuaState->DirectGet("OnGameEvent");

	if(pkFuncObj)
	{
		m_pkEventFunc = T_NEW_D LS_LuaFunction<int>(pkFuncObj);
	}
	
	return true;
}
//---------------------------------------------------------------
void UI_UIManager::PostInit()
{
	m_pkUIImpl->InitIme();
}
//---------------------------------------------------------------
bool UI_UIManager::UnInit()
{
	for (int i = 0; i < VIDEOTYPE_COUNT; ++i)
	{
		this->EndVideo(i);
	}

	UnLoadAllUIScript();
	
	if( m_pkUIImpl )
	{
		m_pkUIImpl->CloseSys();
		T_DELETE_D m_pkUIImpl;
	}
	
	T_SAFE_DELETE(m_pkEventFunc);
	WT_Widget::DeleteStringConsts();

	return true;
}

//---------------------------------------------------------------
bool UI_UIManager::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK,							_event_TimerTick)
	MESSAGE_HANDLE(MGT_UI_REDRAWALLWIDGET,					_event_RedrawAllWidget)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_DOWN,			_event_InputMouseLButtonDown)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_UP,				_event_InputMouseLButtonUp)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_DBCLK,			_event_InputMouseLButtonDBClik)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_RBUTTON_DOWN,			_event_InputMouseRButtonDown)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_MOVE,					_event_InputMouseMove)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_WHEEL,					_event_InputMouseWheel)
	MESSAGE_HANDLE(MGT_WIN32_MESSAGE,						_event_Win32Msg)
	MESSAGE_HANDLE(MGT_UI_MAP_CHANGE,						_event_MapChange)
	MESSAGE_HANDLE(MGT_INPUT_KEY_F11,						_event_InputKeyF11)
	MESSAGE_HANDLE(MGT_UI_RELOAD_ALL_SCRIPT,				_event_ReloadAllUIScript)

	END_MESSAGE_HANDLE
	return true;
}

void UI_UIManager::_event_ReloadAllUIScript(MG_MessageBase& rkMessage)
{
	UE_UiEffectManager* pkUERenderManager = UE_UiEffectManager::GetSingletonPtr();
	if(!pkUERenderManager)
		return;

	SetLoadState(UI_UIManager::UI_LOAD_STAGE_START_RELOADING);
	pkUERenderManager->ClearAll();
	UnLoadAllUIScript();
	LoadAllUIScript();
	//pkUIManager->GoToAnnounceScreen();
	//pkUIManager->GoToLoginScreen();
	//pkUIManager->GoToSelServerScreen();
	//pkUIManager->GoToSelCharScreen();
	//pkUIManager->GoToCreateCharScreen();
	//pkUIManager->GoToLoadingScreen();
	//pkUIManager->GotoSelCharServerScreen();
	GoToGameScreen();
	//YunYingPlatform.lua的热加载
	{
		UnInitPlatformLuaFunc();
		InitPlatformLuaFunc();
	}
	SetLoadState(UI_UIManager::UI_LOAD_STAGE_NONE);
}

void UI_UIManager::_event_MapChange(MG_MessageBase& rkMessage)
{
	SetWorldMapID(rkMessage.m_lParam2);

	//---------------- begin  nofify lua that map is changed --------------//
	//--这里在player:getmaplogicid值为旧值，需要使用事件中传递的值-----//
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_MAP_CHANGED);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rkMessage.m_lParam2);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rkMessage.m_lParam4);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, rkMessage.m_lParam1);
	this->HandlerGameEvent(kGameEvent);
	//---------------- end  nofify lua that map is changed --------------//

	_update_MapChanged_Hide(rkMessage.m_lParam2);
}

void UI_UIManager::_update_MapChanged_Hide(int MapID)
{
	CF_UIControlRelation::DataEntryMapExternIterator kIter = CF_UIControlRelation::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CSVFile::CF_UIControlRelation::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		WT_Widget *pWidget = WT_WidgetManager::GetSingletonPtr()->FindWidget(pkEntry->_kPath.c_str());
		if(pWidget && (MapID == MAPID_ARENA || pkEntry->_bMapChangedClose))
			pWidget->ShowWidget(false);

		kIter.MoveNext();
	}
}

//---------------------------------------------------------------
void UI_UIManager::HandlerGameEvent( LC_GameEventBase& rGameEvent)
{
	T_PROFILE("UIManager::HandlerGameEvent");
	//if (m_NextStage == GAMEUISCREEN)
	{
		if(m_pkEventFunc)
		{
			//m_pkUIImpl->UpdateGUIData(rGameEvent.);
			(*m_pkEventFunc)(&rGameEvent);
		}
	}
}

//---------------------------------------------------------------
void UI_UIManager::HandlerMsgGameEvent(int iMsgType, const StringType& str)
{
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_MSG_TO_LUA);
	LC_ClientGameEventUserData_Common kUserData = LC_ClientGameEventUserData_Common(iMsgType, str);
	kGameEvent.SetUserData(&kUserData);

	HandlerGameEvent(kGameEvent);
}

//---------------------------------------------------------------
void UI_UIManager::GoToAnnounceScreen()
{
	//GoToGameScreen();
	m_NextStage = ANNOUNCESCREEN;
	GUISys::WT_RootWidget::GetSingleton().GotoScreen("AnnounceScreen");
}
//------------------------------------------------------------------------------
void UI_UIManager::GoToLoginScreen()
{	
	m_NextStage = LOGINSCREEN;
	GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("LoginScreen");
}
//----------------------------------------------------------------------------
void UI_UIManager::GoToSelServerScreen()
{
	T_PROFILE("UIManager::GoToSelServerScreen");
	m_NextStage = SELCHARSCREEN;
	GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("SelServerScreen");
}
//----------------------------------------------------------------------------
void UI_UIManager::GoToSelCharScreen()
{
	T_PROFILE("UIManager::GoToSelCharScreen");
	m_pkUIImpl->ScreenUninit();
	m_NextStage = SELCHARSCREEN;
	GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("SelCharScreen");
}
//----------------------------------------------------------------------------
void UI_UIManager::GotoSelCharServerScreen()
{
	T_PROFILE("UIManager::GotoSelCharServerScreen");
	m_NextStage = SELCHARSERVERSCREEN;
	GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("SelCharServerScreen");
}
//----------------------------------------------------------------------------
void UI_UIManager::GoToCreateCharScreen()
{
	T_PROFILE("UIManager::GoToCreateCharScreen");
	m_NextStage = CREATECHARSCREEN;
	GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("CreateCharScreen");
}
//---------------------------------------------------------------
void UI_UIManager::GoToGameScreen()
{
	T_PROFILE("UIManager::GoToGameScreen");
	m_NextStage = GAMEUISCREEN;
	GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("GameUIScreen");
    m_pkUIImpl->ScreenInit();
	m_iPercent = -1;
	m_nLastLoadNum = 0;
	m_bReset = false;
}
//-----------------------------------------------------------------------------------------------------------------
void UI_UIManager::GoToLoadingScreen()
{
	T_PROFILE("UIManager::GoToLoadingScreen");
	m_NextStage = LOGINSCREEN;
	GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("LoadingScreen");
	m_iPercent = -1;
	m_nLastLoadNum = 0;
}
//-----------------------------------------------------------------------------------------------------------------
void UI_UIManager::GotoChangeServerScreen()
{
	//T_PROFILE("UIManager::GotoChangeServerScreen");
	//m_NextStage = CHANGESERVERSCREEN;
	//GUISys::WT_RootWidget::GetSingletonPtr()->GotoScreen("ChangeServerScreen");
	//m_iPercent = -1;
	//m_nLastLoadNum = 0;
}
//-----------------------------------------------------------------------------------------------------------------
void UI_UIManager::SetUIMouseLogicType(int iLogicType)
{
	m_eUIMouseLogicType = (MouseLogicModeType)iLogicType;
}
//-----------------------------------------------------------------------------------------------------------------
void UI_UIManager::RegisterToLua()
{
	UI_SysImpl::RegisterToLua();
	//UINativeFun::RegisterNativeFun();
}
//------------------------------------------------------------------------
void UI_UIManager::Reset()
{
	m_easyModelT.ResetTimer();
	m_hookProfit.ResetTimer();
}
//-----------------------------------------------------------------------------------------------------------------
void UI_UIManager::LoadAllUIScript()
{
	if(!m_pkUIImpl)
		return;
	LS_LuaState* pkLuaState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	UT_DirConfig* pkDirConfig = UT_DirConfig::GetSingletonPtr();
	StringType kUIScriptRootPath = pkDirConfig->GetDir(CF_NODE_UI_SCRIPT);
	StringType kUIScriptRootFileName;
#ifdef _MAP_VIEWER_BUILD
	kUIScriptRootFileName = kUIScriptRootPath + "mapviewer_uires.lua";
#else
	kUIScriptRootFileName = kUIScriptRootPath + "uires.lua";
#endif

	if(MG_UIMessageCenter::GetSingletonPtr())
	{
		MG_UIMessageCenter::GetSingletonPtr()->ClearAll();
	}

	m_pkUIImpl->LoadUI(pkLuaState,kUIScriptRootFileName.c_str());

	_initsoundPath();


	if(UE_RenderManager::GetSingletonPtr())
		UE_RenderManager::GetSingletonPtr()->LoadImgsets();

	//游戏窗口初始化时不再全部创建完，所以不在这初始化窗口关系
	//InitWidgetRelation();
}
//---------------------------------------------------------------
void UI_UIManager::UnLoadAllUIScript()
{
	if(m_pkUIImpl)	
		m_pkUIImpl->UnLoadUI();
}
//---------------------------------------------------------------
void UI_UIManager::_event_TimerTick( MG_MessageBase& rkMessage )
{
	T_PROFILE( "Update::UI_UIManager" );

	float fCurrentTime = rkMessage.m_fParam1;
	float fDeltaTime = rkMessage.m_fParam2;

	m_pkUIImpl->UpdateGUI(fDeltaTime);

	if(m_bLogicTipVisible && fCurrentTime - m_fStartPrintLogicTipTime > 2.0f )
	{
		m_fStartPrintLogicTipTime = fCurrentTime;
	}
	
	m_pkUIImpl->UpdateMaterial(fDeltaTime);

	for (int i = 0; i < VIDEOTYPE_COUNT; ++i)
	{
		UI_VideoTexture		*pkVideoTexture = m_pkVideoTexture[i];
		CWinVideo			*pkVideoPlayer = m_pkVideoPlayer[i];

		if (pkVideoPlayer->BeginUpdate(fDeltaTime))
		{
			//get raster
			void *pBuf = NULL;
			int pitch =0;
			if (pkVideoTexture->LockBuffer(&pBuf, &pitch))
			{
				pkVideoPlayer->CopyImage(pBuf, pitch);

				pkVideoTexture->UnlockBuffer();
			}

			bool bEnd = pkVideoPlayer->isEnd();
			pkVideoPlayer->EndUpdate();		//stop internal

			if (bEnd)
			{
				pkVideoTexture->Destroy();	//销毁贴图，播放真的结束后贴图就不再需要了

				//launch event
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_CLIENT_VIDEO_END);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, i);
				this->HandlerGameEvent(kGameEvent);
			}
		}
	}

	//淡入淡出控制
	if (m_kFIFOController.enabled())
	{
		bool b = m_kFIFOController.update(fDeltaTime);
		WT_Widget* pWgt = WT_WidgetManager::GetSingleton().FindWidget("UI.GameUIScreen.LayerMask");
		if (!b)
		{
			m_kFIFOController.enable(false);	//自动关闭
			if (pWgt)
				pWgt->SetUseVerticesColor(false);
		}
		else
		{
			float t = m_kFIFOController.lastUpdate();
			GUISys::GUIColor c1 = m_kFIFODstColor - m_kFIFOSrcColor;
			GUISys::GUIColor c2 = m_kFIFOSrcColor  + c1 * t;
			if (pWgt)
				pWgt->SetVerticesColor(c2);
		}
	}

	m_easyModelT.tick(fDeltaTime);
	m_hookProfit.tick(fDeltaTime);
}
//---------------------------------------------------------------
void UI_UIManager::_event_InputMouseLButtonDown( MG_MessageBase& rkMessage )
{
	if(!m_bHandleMouseMessage)
		return;

	unsigned int uiMouseScreePosX = rkMessage.m_uiParam1;
	unsigned int uiMouseScreePoxY = rkMessage.m_uiParam2;

	LPARAM lParam = MAKELONG(uiMouseScreePosX,uiMouseScreePoxY);
	WPARAM wParam = 0;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_UI_LEFT_BUTTON_DOWN_NOMEAN);
	HandlerGameEvent(kGameEvent);

	if(m_pkUIImpl)
	{
		m_pkUIImpl->MarkMouseHandled(false);	
		m_pkUIImpl->WndProc(m_hWnd,WM_LBUTTONDOWN,wParam,lParam);
		if(m_pkUIImpl->GetMouseHandled())
			MARK_MESSAGE_HANDLED(rkMessage);
		else
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_UI_LEFT_BUTTON_DOWN_UNHANDLED);
			HandlerGameEvent(kGameEvent);
		}
	}
}
//---------------------------------------------------------------
void UI_UIManager::_event_InputMouseLButtonUp( MG_MessageBase& rkMessage )
{
	if(!m_bHandleMouseMessage)
		return;

	unsigned int uiMouseScreePosX = rkMessage.m_uiParam1;
	unsigned int uiMouseScreePoxY = rkMessage.m_uiParam2;

	LPARAM lParam = MAKELONG(uiMouseScreePosX,uiMouseScreePoxY);
	WPARAM wParam = 0;

	if(m_pkUIImpl)
	{
		m_pkUIImpl->MarkMouseHandled(false);	
		m_pkUIImpl->WndProc(m_hWnd,WM_LBUTTONUP,wParam,lParam);
		if(m_pkUIImpl->GetMouseHandled())
			MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//---------------------------------------------------------------
void UI_UIManager::_event_InputMouseLButtonDBClik(MG_MessageBase& rkMessage)
{
	if(!m_bHandleMouseMessage)
		return;

	if (TEST_MESSAGE_HANDLED(rkMessage))
		return;
	
	if(m_pkUIImpl)
	{
		unsigned int uiMouseScreePosX = rkMessage.m_uiParam1;
		unsigned int uiMouseScreePoxY = rkMessage.m_uiParam2;

		LPARAM lParam = MAKELONG(uiMouseScreePosX,uiMouseScreePoxY);
		WPARAM wParam = 0;

		m_pkUIImpl->MarkMouseHandled(false);
		m_pkUIImpl->WndProc(m_hWnd,WM_LBUTTONDBLCLK,wParam,lParam);
		if(m_pkUIImpl->GetMouseHandled())
			MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//---------------------------------------------------------------
void UI_UIManager::_event_InputMouseRButtonDown(MG_MessageBase& rkMessage)
{
	if(!m_bHandleMouseMessage)
		return;

	unsigned int uiMouseScreePosX = rkMessage.m_uiParam1;
	unsigned int uiMouseScreePoxY = rkMessage.m_uiParam2;

	LPARAM lParam = MAKELONG(uiMouseScreePosX,uiMouseScreePoxY);
	WPARAM wParam = 0;

	if(m_pkUIImpl)
	{
		m_pkUIImpl->MarkMouseHandled(false);	
		m_pkUIImpl->WndProc(m_hWnd,WM_RBUTTONDOWN,wParam,lParam);
		if(m_pkUIImpl->GetMouseHandled())
			MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//---------------------------------------------------------------
void UI_UIManager::_event_InputMouseMove(MG_MessageBase& rkMessage)
{

	T_PROFILE( "UIManager::InputMouseMove" );
	unsigned int uiMouseScreePosX = rkMessage.m_uiParam1;
	unsigned int uiMouseScreePoxY = rkMessage.m_uiParam2;
	
	LPARAM lParam = MAKELONG(uiMouseScreePosX,uiMouseScreePoxY);
	WPARAM wParam = rkMessage.m_lParam3;
	
	MouseLogicModeType eMouseType = MLMT_NORMAL;

	LC_ClientLogicManager* pkClientLogicManager = CLIENT_GET_LOGIC_MANAGER();
	object_id_type lID = pkClientLogicManager->GetInputControlActorID();
	if (IS_PLAYER_CLASS_ID(lID))
	{
		LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
		LC_ClientGamePlayer* pkClientGamePlayer =(LC_ClientGamePlayer*) pkPlayerManager->FindPlayer(lID);
		if(pkClientGamePlayer)
			eMouseType = pkClientGamePlayer->GetMouseLogicModeType();//获取玩家鼠标状态，这是最高优先级
	}

	if(m_bHandleMouseMessage)
	{
		if(m_pkUIImpl)
		{
			m_pkUIImpl->MarkMouseHandled(false);	
			bool bProc = m_pkUIImpl->WndProc(m_hWnd,WM_MOUSEMOVE,wParam,lParam);
			if(m_pkUIImpl->GetMouseHandled())
			{
				MARK_MESSAGE_HANDLED(rkMessage);
				if(m_eUIMouseType != MLMT_NONE)//游戏内UI可能会改变鼠标控制
					eMouseType = m_eUIMouseType;
			}
		}
	}

	if(eMouseType == MLMT_NORMAL)//如果是普通态，用UILogicType替换(可能会出现拆分等状态)
	{
		eMouseType = m_eUIMouseLogicType;//
	}
	


	_changeCursor(eMouseType);
}
//---------------------------------------------------------------
void UI_UIManager::_event_InputMouseWheel(MG_MessageBase& rkMessage)
{
	if(!m_bHandleMouseMessage)
		return;

	unsigned int uiMouseScreePosX = rkMessage.m_uiParam1;
	unsigned int uiMouseScreePoxY = rkMessage.m_uiParam2;

	LPARAM lParam = MAKELONG(uiMouseScreePosX,uiMouseScreePoxY);
	WPARAM wParam = rkMessage.m_lParam3;

	if(m_pkUIImpl)
	{
		m_pkUIImpl->MarkMouseHandled(false);	
		m_pkUIImpl->WndProc(m_hWnd,WM_MOUSEWHEEL,wParam,lParam);
		if(m_pkUIImpl->GetMouseHandled())
			MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//---------------------------------------------------------------
void UI_UIManager::_event_Win32Msg( MG_MessageBase& rkMessage )
{
	if(GetLoadState() == UI_LOAD_STAGE_START_RELOADING)
		return;

	UINT msg = rkMessage.m_uiParam1;
	if (msg == WM_MOUSELEAVE)
	{
		if (WT_RootWidget::GetSingletonPtr())
			WT_RootWidget::GetSingletonPtr()->LostFocus();
		return;
	}

	WPARAM wParam = rkMessage.m_uiParam2;
	LPARAM lParam = rkMessage.m_uiParam3;

	if(msg != WM_LBUTTONDOWN
		&& msg != WM_LBUTTONUP
		&& msg != WM_LBUTTONDBLCLK
		&& msg != WM_RBUTTONDOWN
		&& msg != WM_MOUSEMOVE
		&& msg != WM_MOUSEWHEEL)
	{
		bool result = true;
		if(m_pkUIImpl)
		{
			result = m_pkUIImpl->WndProc(m_hWnd,msg,wParam,lParam);
		}
	}
}
//---------------------------------------------------------------
void UI_UIManager::_event_RedrawAllWidget( MG_MessageBase& rkMessage )
{
	RedrawAllWidget();
}
//-----------------------------------------------------------------------------------------------------------------
void UI_UIManager::_changeCursor(MouseLogicModeType eCursorType)
{
	if(eCursorType == MLMT_NONE)//安全检查
		eCursorType = MLMT_NORMAL;

	NiCursor* spCursor=GUISys::WT_RootWidget::GetSingletonPtr()->GetCursor();
	GUISys::UI_Cursor* spSysCursor = (GUISys::UI_Cursor*)spCursor;
	spSysCursor->SetType(eCursorType);
	m_eLastUIMouseType = eCursorType;
}

int UI_UIManager::PlaySound(const char *pkPath)
{
	return g_soundMgr.Play2DEx( pkPath , AS_ClientSoundManager::SCT_2D );
}

int UI_UIManager::PlaySound( int iType )
{
	return g_soundMgr.Play2DEx( GetSoundName(iType) , AS_ClientSoundManager::SCT_2D );
}
int UI_UIManager::PlayEventSound(StringType& strSoundName, int iPlayCount, float fPlayTime )
{
	return g_soundMgr.Play2DEx( strSoundName, AS_ClientSoundManager::SCT_2D , false , iPlayCount , fPlayTime );
}

void UI_UIManager::StopAudioSource(int iSoundID)
{
	((AS_ClientSoundManager*) AS_ClientSoundManager::GetSingletonPtr())->
		StopAudioSource(iSoundID);
}

void UI_UIManager::StopAudioSourceFading(int iSoundID, float fadeTime)
{
	((AS_ClientSoundManager*) AS_ClientSoundManager::GetSingletonPtr())->
		StopAudioSourceFade(iSoundID, fadeTime);
}

size_t UI_UIManager::GetAllImgAllocationSize() const
{
	return m_pkUIImpl->GetAllImgAllocationSize();
}

void UI_UIManager::RedrawAllWidget() const
{
	m_pkUIImpl->RedrawAllWidget();
}

void UI_UIManager::UpdateAvatarTexture(const StringType& strGroupName,long lCharType,int iAvatarIndex, int iWeaponID, int iCameraMode)
{
    if (m_pkAvatarManager)
    {
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateActorScene(strGroupName, lCharType,iAvatarIndex,iCameraMode);
    }
}

void UI_UIManager::UpdateWildPrincesBossTexture(const StringType& strGroupName,int iAvatarIndex, long lTypeID, float fCameraDist, float fTranslateX, float fTranslateZ, int iCameraMode)
{
	if (m_pkAvatarManager)
	{
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateWildPrincesBossScene(strGroupName, iAvatarIndex, lTypeID, fCameraDist, fTranslateX, fTranslateZ, iCameraMode);
	}
}

void UI_UIManager::UpdateTalkNpcTexture(const StringType& strGroupName,int iAvatarIndex, long lTypeID, int iCameraMode)
{
	if (m_pkAvatarManager)
	{
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateTalkNpcScene(strGroupName, iAvatarIndex, lTypeID, iCameraMode);
	}
}

void UI_UIManager::UpdateVehicleTexture( const StringType& strGroupName, long lCharType, int iIndex, int iRenderModel)
{
    if (m_pkAvatarManager)
    {
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateVehicleScene(strGroupName, lCharType, iIndex, iRenderModel);
    }
}

void UI_UIManager::UpdateArenaBattleActor(const StringType& strGroupName, int iCid, int iIndex, int iCameraMode)
{
	if (m_pkAvatarManager)
	{
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateArenaBattleActorScene(strGroupName, iCid, iIndex, 0, iCameraMode);
	}
}

void UI_UIManager::UpdateNonpareilItemAvatar(const StringType& strGroupName, long lCharType, int iCharIndex, float fTranslateX, float fTranslateY, float fTranslateZ, int iCameraMode)
{
	if (m_pkAvatarManager 
		&& m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateNonpareilItemAvatarScene(strGroupName, lCharType, iCharIndex, fTranslateX, fTranslateY, fTranslateZ, iCameraMode);
}

void UI_UIManager::UpdateDevaEyeTexture(const StringType& strGroupName,long lCharType, int iIndex, int iCameraMode)
{
	if (m_pkAvatarManager)
	{
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateDevaEyeScene(strGroupName, lCharType, iIndex, iCameraMode);
	}
}

void UI_UIManager::UpdateFabaoTexture(const StringType& strGroupName,long lCharType, int iIndex, int iCameraMode)
{
	if (m_pkAvatarManager)
	{
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateFabaoScene(strGroupName, lCharType, iIndex, iCameraMode);
	}
}

void UI_UIManager::UpdateGodWingTexture(const StringType& strGroupName,long lCharType, int iIndex, int iCameraMode)
{
	if (m_pkAvatarManager)
	{
		if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			m_pkAvatarManager->UpdateGodWingTextureScene(strGroupName, lCharType, iIndex, iCameraMode);
	}
}

void UI_UIManager::UpdatePlatform(const StringType& strGroupName, int iIndex, long lCharType, int iCameraMode)
{
	if(!m_pkAvatarManager)
	{
		return;
	}

	if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
		m_pkAvatarManager->UpdatePlatformScene(strGroupName, lCharType, iIndex, iCameraMode);
}

void UI_UIManager::UpdateToyPet(const StringType& strGroupName, int iIndex, long lResID, int iCameraMode)
{
	if(!m_pkAvatarManager)
	{
		return;
	}

	if(m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
		m_pkAvatarManager->UpdateToyPetScene(strGroupName, lResID, iIndex, iCameraMode);
}

void UI_UIManager::UpdateRankingWholeBodyAvatar(const StringType& strGroupName, long lActorCharType, long lCharType, int iAvatarIndex, int iCameraMode)
{
	if (!m_pkAvatarManager)
		return;

	if(!m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
		return;

	long lActorType = lActorCharType;
	long lVehicleType = 0;
	long lDevaEyeType = 0;
	long lMuhunType = 0;
	long lPetToyType = 0;
	
	switch(iAvatarIndex)
	{
		case SIRAT_VEHICLE:
			{
				lActorType = 0;
				lVehicleType = lCharType;
			}
			break;
		case SIRAT_DEVA_EYE:
			{
				lDevaEyeType = lCharType;
			}
			break;
		case SIRAT_MUHUN:
			{
				lMuhunType = lCharType;
			}
			break;
		case SIRAT_TOY:
			{
				lPetToyType = lCharType;
			}
			break;
	}

	m_pkAvatarManager->UpdateActorScene(strGroupName, lActorType, 0, iCameraMode);
	m_pkAvatarManager->UpdateVehicleScene(strGroupName, lVehicleType, 0, iCameraMode);
	m_pkAvatarManager->UpdateGodWingTextureScene(strGroupName,lMuhunType,0,iCameraMode);
	m_pkAvatarManager->UpdateDevaEyeScene(strGroupName, lDevaEyeType, 0, iCameraMode);
	m_pkAvatarManager->UpdateToyPetScene(strGroupName, lPetToyType, 0, iCameraMode);
}

void UI_UIManager::UpdateGreatKungFuRemainAvatar( const StringType& strGroupName, long lCharType, int iAvatarIndex, int iCameraMode)
{
	if(m_pkAvatarManager)
	{
		if(!m_pkAvatarManager->publicCallBack_IsActiveGroup(strGroupName))
			return;

		long lVehicleType = 0;
		long lDevaEyeType = 0;
		long lMuhunType = 0;
		long lBuddyCID = 0;
		long lStigmasType = 0;

		switch(iAvatarIndex)
		{
		case SIRAT_VEHICLE:
			{
				lVehicleType = lCharType;
			}
			break;
		case SIRAT_DEVA_EYE:
			{
				lDevaEyeType = lCharType;
			}
			break;
		case SIRAT_MUHUN:
			{
				lMuhunType = lCharType;
			}
			break;
		
		case SIRAT_BUDDY:
			{
				lBuddyCID = lCharType;
			}
			break;
		case SIRAT_STIGMAS:
			{
				lStigmasType = lCharType;
			}
			break;
		default:
			break;
		}

		m_pkAvatarManager->UpdateVehicleScene(strGroupName, lVehicleType, 0, iCameraMode);
		m_pkAvatarManager->UpdateDevaEyeScene(strGroupName, lDevaEyeType, 0, iCameraMode);
		m_pkAvatarManager->UpdateGodWingTextureScene(strGroupName, lMuhunType, 0, iCameraMode);
		m_pkAvatarManager->UpdateActorScene(strGroupName, lBuddyCID, 0, iCameraMode);
		m_pkAvatarManager->UpdatePlatformScene(strGroupName, lStigmasType, 0, iCameraMode);
	}
}

void UI_UIManager::ActiveAvatarGroup(const StringType& strGroupName, bool bActive)
{
	if (m_pkAvatarManager)
		m_pkAvatarManager->publicCallBack_ActiveGroup(strGroupName, bActive);
}

void UI_UIManager::RegisterAvatarManager(UI_AvatarManager* avatarManager)
{
	T_ASSERT2(!m_pkAvatarManager, "Another avatarManager attempt to register!");
	m_pkAvatarManager = avatarManager;
}

void UI_UIManager::UnRegisterAvatarManager()
{
	m_pkAvatarManager = NULL;
}

void UI_UIManager::PlayCharAnimation(long lCharType, int iIndex, int iCameraMode, const StringType& sAnimName, bool bForce, int iLoopNum)
{
	if (!m_pkAvatarManager)
		return;

	m_pkAvatarManager->PlayAvatarAnim(lCharType, iIndex, iCameraMode, sAnimName, bForce, iLoopNum);
}

void UI_UIManager::AttachFxOnAvatarModel(long lCharType, int iIndex, int iCameraMode, const StringType& sFxName, float fx, float fy, float fz)
{
	if (!m_pkAvatarManager)
		return;

	NiPoint3 kPoint = NiPoint3(fx, fy, fz);
	m_pkAvatarManager->AttachAvatarFx(lCharType, iIndex, iCameraMode, sFxName, &kPoint);
}

void UI_UIManager::DetachFxOnAvatarModel(long lCharType, int iIndex, int iCameraMode, const StringType& sFxName)
{
	if (!m_pkAvatarManager)
		return;

	m_pkAvatarManager->DetachAvatarFx(lCharType, iIndex, iCameraMode, sFxName);
}

void UI_UIManager::GameUIResolutionChange( int nWidth, int nHeight )
{
	m_pkUIImpl->GameUIResolutionChange(nWidth,nHeight);

	for (int i = 0; i < VIDEOTYPE_COUNT; ++i)
	{
		m_pkVideoTexture[i]->RecreateRenderTarget();
	}
}

bool UI_UIManager::IsLoadingScreen()
{
	WT_Widget* pkWgt = GUISys::WT_RootWidget::GetSingleton().GetCurScreen();
	if (NULL == pkWgt || pkWgt->GetType() != WGTTYPE_SCREEN)
		return false;

	CL_CUIScreen* pkScreen = (CL_CUIScreen*)pkWgt;
	return (pkScreen->GetScreenID() == SCERRN_ID_Loading);
}

bool UI_UIManager::IsGameScreen()
{
	WT_Widget* pkWgt = GUISys::WT_RootWidget::GetSingleton().GetCurScreen();
	if (NULL == pkWgt || pkWgt->GetType() != WGTTYPE_SCREEN)
		return false;

	CL_CUIScreen* pkScreen = (CL_CUIScreen*)pkWgt;
	return (pkScreen->GetScreenID() == SCERRN_ID_Game);
}

void UI_UIManager::UpdateLoadingPercent( size_t nLoadNum )
{
	if(IsLoadingScreen())
	{
		if(m_nLastLoadNum > nLoadNum)
		{
			float fPercent = 100.0f - 
				( (float)nLoadNum / (float)m_nLastLoadNum  )* 100.0f;
			_setPercent((int)fPercent);
			if(nLoadNum <= 1 )
			{
				m_iPercent = 100;
				m_nLastLoadNum = 0;
			}
		}
		m_nLastLoadNum = max(m_nLastLoadNum,nLoadNum);
	}
	else
	{
		m_iPercent = -1;
		m_nLastLoadNum = 0;
	}	
}

int UI_UIManager::GetLoadingPercent()
{
	return m_iPercent;
}

void UI_UIManager::_setPercent( int iPercent )
{
	if(iPercent >= m_iPercent && m_nLastLoadNum)
		m_iPercent = iPercent;

}

unsigned int UI_UIManager::GetResolutionCount()
{
	return m_kResolutionList.GetSize();
}

const char * UI_UIManager::GetResolution( unsigned int iIndex )
{
	if( iIndex >= m_kResolutionList.GetSize() )
	{
		return "NULL";
	}
	return m_kResolutionList.GetAt(iIndex);
}

void UI_UIManager::SetCurrentScreenVisibility(bool bVisible)
{
    StringType strWidgetPath;
    switch (m_NextStage)
    {
    case SELCHARSCREEN:
        strWidgetPath = "UI.SelCharScreen";
    	break;
    case SELCHARSERVERSCREEN:
        strWidgetPath = "UI.SelCharServerScreen";
        break;
    case CREATECHARSCREEN:
        strWidgetPath = "UI.CreateCharScreen";
        break;
    case GAMEUISCREEN:
        strWidgetPath = "UI.GameUIScreen";
        break;
    default:
        return;
    }
    WT_Widget *pWidget = WT_WidgetManager::GetSingleton().FindWidget(strWidgetPath.c_str());
    if(pWidget)
    {
        pWidget->ShowWidget(bVisible);

		UI_SysImpl::GetSingleton().UpdateWidgetRenderList();
		RS_GUIRender::GetSingleton().SetDirtyFlag( NULL );
    }
	//g_settings.UI_RENDER_SHOW = bVisible;
}

void UI_UIManager::SetLoadingTimeOut()
{
	m_iPercent = 100;
	m_nLastLoadNum = 0;
}

void UI_UIManager::_initsoundPath()
{
	m_kSoundName[UI_SOUND_CLICK] = "SE_UI_Click_OK";
	m_kSoundName[UI_SOUND_MOUSEIN] = "UI_MOUSE_HOVER";
	m_kSoundName[UI_SOUND_PAGE] = "SE_UI_DLG_Page";
	m_kSoundName[UI_SOUND_NODE_EXTEND] = "SE_UI_MenuH_Exp";
	m_kSoundName[UI_SOUND_NODE_COLLAPSE] = "SE_UI_MenuH_Col";
}

const char * UI_UIManager::GetSoundName( int iType )
{
	if(iType > m_kSoundName.size() || iType < 0)
		return "";
	return m_kSoundName[iType].c_str();
}

void UI_UIManager::_initLoadingTip()
{
	m_kUnUsedTipVector.clear();
	CF_LoadingTip::DataEntryMapExternIterator kIter = CF_LoadingTip::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		CF_LoadingTip::DataEntry* pkEntry = kIter.PeekNextValuePtr();
	
		m_kUnUsedTipVector.push_back(pkEntry);
	}
	m_kUsedTipMap.clear();
}

StringType UI_UIManager::GetLoadingTip()
{
	if(m_kUnUsedTipVector.empty())
	{
		m_kUnUsedTipVector = m_kUsedTipMap;
		m_kUsedTipMap.clear();

		if(m_kUnUsedTipVector.empty())
		{
			return "";
		}
	}

	CF_LoadingTip::DataEntry* pkEntry = NULL;
	mem::vector<CF_LoadingTip::DataEntry*>  kWorldMapLoadingTip;
	mem::vector<CF_LoadingTip::DataEntry*>  kGlobalLoadingTip;
	for (size_t i = 0; i < m_kUnUsedTipVector.size(); i++)
	{
		pkEntry = m_kUnUsedTipVector[i];
		if(pkEntry &&  pkEntry->_iIsGlobal)
		{
			kGlobalLoadingTip.push_back(pkEntry);
		}
		if(pkEntry && m_lWorldMapID == pkEntry->_iWorldMapID)
		{
			kWorldMapLoadingTip.push_back(pkEntry);
		}
	}

	if(!kWorldMapLoadingTip.empty())
	{
		int iRandIndex = NiRand() % kWorldMapLoadingTip.size();
		pkEntry = kWorldMapLoadingTip[iRandIndex];
	}	
	else if(!kGlobalLoadingTip.empty())
	{
		int iRandIndex = NiRand() % kGlobalLoadingTip.size();
		pkEntry = kGlobalLoadingTip[iRandIndex];
	}
	_eraseTip(pkEntry);
	return pkEntry->_kTipDes;	
}

void UI_UIManager::_eraseTip( CSVFile::CF_LoadingTip::DataEntry* pEntry )
{
	mem::vector<CSVFile::CF_LoadingTip::DataEntry*>::iterator kIter;
	for( kIter = m_kUnUsedTipVector.begin(); kIter != m_kUnUsedTipVector.end(); )
	{
		CF_LoadingTip::DataEntry* pkEntry = *kIter;
		if(pkEntry == pEntry)
		{
			m_kUsedTipMap.push_back(*kIter);
			kIter = m_kUnUsedTipVector.erase(kIter);
			break;
		}
		else
			++kIter;
	}
}

//StringType UI_UIManager::GetLoadingImage()
//{
//	CF_LoadingTip::DataEntryMapExternIterator kIter = CF_LoadingTip::GetSingletonPtr()->GetIterator();
//	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
//	{
//		CF_LoadingTip::DataEntry* pkEntry = kIter.PeekNextValuePtr();
//		if(pkEntry && m_lWorldMapID == pkEntry->_iWorldMapID)
//			return pkEntry->_kWorldImagePath;
//	}
//	return "NULL";
//}

void UI_UIManager::SetWorldMapID( int iWorldMapID )
{
	m_lWorldMapID = iWorldMapID;
}

void UI_UIManager::_event_InputKeyF11( MG_MessageBase& rkMessage )
{
	if( g_settings.CFG_ENABLE_CUTSCENE )
		UT_CutSceneManager::GetSingleton().ReloadConfig();

    //if (m_pkAvatarTexture)
    //{
    //    m_pkAvatarTexture->ReloadCameraConfig();
    //}
}


void UI_UIManager::ResetUIRoot()
{
	WT_RootWidget::GetSingleton().RecursiveResetUI();
}

int UI_UIManager::GetUIMouseLogicType()
{
	return (MouseLogicModeType)m_eUIMouseLogicType;
}

void UI_UIManager::InitWidgetRelation()
{
	m_pUIControlRelation->InitWidget();
}

void UI_UIManager::SetWidgetRelation(const char* sWgtName)
{
	m_pUIControlRelation->InitWidget(sWgtName);
}

StringType UI_UIManager::RemoveRichInfo( const StringType &sRichText )
{
	CL_RichText *pRichText = getRichTextParse->ParseText(sRichText,0);
	
	StringType sText = pRichText->GetTextString();

	CL_RichTextManager::GetSingleton().DestroyRichText(pRichText->GetIndex());

	return sText;
}

bool UI_UIManager::IsFirstHappyPageVisiable()
{
	WT_Widget *pWidget = WT_WidgetManager::GetSingleton().FindWidget("UI.GameUIScreen.GuideText.Move");
	if(pWidget)
	{
		return pWidget->IsVisible();
	}

	return false;
}

void UI_UIManager::NotifyRestoreEasyModelUI()
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_UI_RESTORE_EASY_MODEL);
	HandlerGameEvent(kGameEvent);
}

void UI_UIManager::NotifyLButtonUp()
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_LBUTTONUP);
	HandlerGameEvent(kGameEvent);
}


void UI_UIManager::NotifyReportDownLoadOver()
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_REPORT_DOWNLOAD_OVER);
	HandlerGameEvent(kGameEvent);
}

void UI_UIManager::SetEasyModeTic(float tt1)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if( pkGamePlayer && pkGamePlayer->IsHooking() )
	{
		m_easyModelT.Start(tt1);
	}
}

bool UI_UIManager::IsEasyModel()
{
	return m_easyModelT.IsDown();
}

void UI_UIManager::ResetEasyModelTick()
{
	m_easyModelT.ResetTimer();
}

void  UI_UIManager::SetHookProfitTic(float tt1)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if( pkGamePlayer && pkGamePlayer->IsHooking() )
	{
		m_hookProfit.Start(tt1);
	}
}
bool  UI_UIManager::IsHookProfit()
{
	return m_hookProfit.IsDown();
}
void  UI_UIManager::ResetHookProfitTick()
{
	m_hookProfit.ResetTimer();
}

void UI_UIManager::SetUIMouseType( int iLogicType )
{
	if(iLogicType <= MLMT_NONE || iLogicType >= MLMT_MAX_COUNT)
	{
		iLogicType = MLMT_NONE;
	}
	m_eUIMouseType = (MouseLogicModeType)iLogicType;
}


//////////////////////////////////////////////////////////////////////////
bool UI_UIManager::PlayVideo(int iVideoType, const char *cpImagename, const char *cpFilename, unsigned uLoopCount)
{
	if(iVideoType < 0 || iVideoType >= VIDEOTYPE_COUNT)
		return false;

	CWinVideo			*pkVideoPlayer = m_pkVideoPlayer[iVideoType];
	UI_VideoTexture		*pkVideoTexture = m_pkVideoTexture[iVideoType];

	pkVideoPlayer->setLoops(uLoopCount);
	bool b = pkVideoPlayer->Play(cpFilename, m_hWnd);
	if (b)
	{
		//create texture
		bool b2 = pkVideoTexture->Create(cpImagename, pkVideoPlayer->getWidth(), pkVideoPlayer->getHeight());
		
		if (b2)
		{
			//launch event
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_VIDEO_BEGIN);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iVideoType);
			this->HandlerGameEvent(kGameEvent);

//			if (iVideoType == VIDEOTYPE_GAME_INTRO)
//				g_soundMgr.PauseCurrentBGM();
		}
		
		return b2;
	}

	return false;
}

bool UI_UIManager::PlayNilVideo(int iVideoType)
{
	if(iVideoType < 0 || iVideoType >= VIDEOTYPE_COUNT)
		return false;

	EndVideo(iVideoType);

	//launch event
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_VIDEO_INIT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iVideoType);
	this->HandlerGameEvent(kGameEvent);

	return true;
}

void UI_UIManager::EndVideo(int iVideoType)
{
// 四测bug，VIDEOTYPE_GAME_INTRO useless
//	if (iVideoType == VIDEOTYPE_GAME_INTRO)
//		g_soundMgr.ResumeCurrentBGM();

	if(iVideoType < 0 || iVideoType >= VIDEOTYPE_COUNT)
		return;

	CWinVideo			*pkVideoPlayer = m_pkVideoPlayer[iVideoType];
	UI_VideoTexture		*pkVideoTexture = m_pkVideoTexture[iVideoType];

	bool bInterrupted = !pkVideoPlayer->isEnd();

	pkVideoPlayer->Stop();
	pkVideoTexture->Destroy();

	//launch event
	if (bInterrupted)	//没播放完，被中断，发一个结束事件；如果自然中断，见UI_UIManager::_event_TimerTick
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_VIDEO_END);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iVideoType);
		this->HandlerGameEvent(kGameEvent);
	}
}

void UI_UIManager::UpdateHookBot(const StringType& strGroupName, const StringType& strChartypes)
{
	if(m_pkAvatarManager == NULL)
		return;

	TStringVector kVec = UT_BaseStringUtility::SplitString(strChartypes, ';');

	for(int i=0; i<kVec.size(); ++i)
	{
		m_pkAvatarManager->UpdateHookBotScene(strGroupName, String2Int(kVec[i]), i, UI_AvatarManager::CM_HOOK_BOT);
	}
}

void UI_UIManager::UpdateHookBotMonster(const StringType& strGroupName, const StringType& strChartypes)
{
	if(m_pkAvatarManager == NULL)
		return;

	m_pkAvatarManager->SetChangeStage(false);

	TStringVector kVec = UT_BaseStringUtility::SplitString(strChartypes, ';');
	for(int i=0; i<kVec.size(); ++i)
	{
		m_pkAvatarManager->UpdateHookBotMonsterScene(strGroupName, String2Int(kVec[i]), i, UI_AvatarManager::CM_HOOK_BOT_MONSTER);
	}
}

void UI_UIManager::UpdateHookBotBoxScene(const StringType& strGroupName, long lCharType, int iCharIndex)
{
	if(m_pkAvatarManager == NULL)
		return;

	m_pkAvatarManager->UpdateHookBotBoxScene(strGroupName, lCharType, iCharIndex, UI_AvatarManager::CM_HOOK_BOT);
}

void UI_UIManager::ScreenFadeIn(UINT srcColor, UINT dstColor, float keepTime, float rampTime)
{
	m_kFIFOSrcColor.SetARGB(srcColor);
	m_kFIFODstColor.SetARGB(dstColor);
	m_kFIFOController.setArg(keepTime, keepTime + rampTime, keepTime + rampTime);

	WT_Widget* pWgt = WT_WidgetManager::GetSingleton().FindWidget("UI.GameUIScreen.LayerMask");
	if (pWgt)
	{
		pWgt->SetUseVerticesColor(true);
		pWgt->SetVerticesColor(m_kFIFOSrcColor);
	}
}

void UI_UIManager::ScreenFadeOut(UINT srcColor, UINT dstColor, float rampTime, float keepTime)
{
	m_kFIFOSrcColor.SetARGB(srcColor);
	m_kFIFODstColor.SetARGB(dstColor);
	m_kFIFOController.setArg(0, rampTime, rampTime + keepTime);

	WT_Widget* pWgt = WT_WidgetManager::GetSingleton().FindWidget("UI.GameUIScreen.LayerMask");
	if (pWgt)
	{
		pWgt->SetUseVerticesColor(true);
		pWgt->SetVerticesColor(m_kFIFOSrcColor);
	}
}

bool UI_UIManager::OnResetMouseLogicType(void)
{
	if(m_eUIMouseLogicType == MLMT_SHOP   ||
		m_eUIMouseLogicType == MLMT_SPLIT || 
		m_eUIMouseLogicType == MLMT_DRAG)
	{
		SetUIMouseLogicType(MLMT_NORMAL);
		return true;
	}

	return false;
}