#include "ClientPCH.h"
#include "CA_CameraManager.h"
#include "FX_ClientFxManager.h"
#include "MG_MessageBase.h"
#include "RG_BloomEffect.h"
#include "RG_ClientRenderStep.h"
#include "RG_Dx9MsaaRenderedTex.h"
#include "RG_RadialBlurEffect.h"
#include "RG_SpaceWarpEffect.h"
#include "RG_OutlineEffect.h"
#include "RG_GreyEffect.h"
#include "RG_RenderManager.h"
#include "SE_SceneManager.h"
#include "UI_UIManager.h"
#include "UI_AvatarManager.h"
#include "UI_AvatarRender.h"
#include "UT_Curl.h"
#include "UT_GlobalSettings.h"
#include "RG_SSAOEffect.h"
#include "IM_Manager.h"
#include "BL_AsyncOperationManager.h"
#include <shlobj.h>
#include "SoftParticlesManager.h"
#include "SoftParticlesMaterial.h"
#include "UE_UiEffectManager.h"
//#ifdef PROC_INPUT
#include <Windows.h>
//#endif
#include "RenderManager.h"
#include "UT_CutSceneManager.h"


#include "TS_DynamicWeather.h"
#include "UT_ScreenConsole.h"
#include "UT_RenderHelper.h"

using namespace Render;
using namespace Terrain;
using namespace Utility;

#define RENDER_RECREATE_MAX_TIME 0.15f	// 优化参数：重新创建设备的间隔

#define RENDER_MINIMUM_WINDOW_WIDTH 10	// 优化参数：最小能够渲染的窗口宽度，游戏窗口在初始化的宽度时候必须大于这个参数；
#define RENDER_MINIMUM_WINDOW_HEIGHT 10	// 优化参数：最小能够渲染的窗口高度，游戏窗口在初始化的高度时候必须大于这个参数；

#define IS_RENDER_STEP_ACTIVE(mask, rs) ((mask & rs) > 0)

RG_RenderManager::RG_RenderManager(void)
: m_nWidth(1024)
, m_nHeight(768)
, m_bIsFullScreen(false)
, m_bIsMinimized(false)
, m_bIsDeviceLost(false)
, m_bIsActiveRender(true)
, m_spNiRenderer(NULL)
, m_spRenderFrame(NULL)
, m_spAvatarRenderStep(NULL)
, m_spAvatarRenderClick(NULL)
, m_spSmallAvatarRenderClick(NULL)
, m_spNoDepthBufferAvatarRenderClick(NULL)
, m_spAvatarBloomEffect(NULL)
, m_spAvatarOuterGlow(NULL)

, m_spShadowRenderStep(NULL)

, m_spMainSceneRenderStep(NULL)
, m_spNormalSceneRenderClick(NULL)
, m_spSceneRenderView(NULL)
, m_spInifiniteRenderView(NULL)
, m_spGrassRenderClick(NULL)
, m_bSceneDepthEnabled(false)
, m_spSceneDepthRenderClick(NULL)
, m_spSceneDepthRTG(NULL)
, m_spSceneDepthRenderedTex(NULL)

, m_spNormalFxRenderStep(NULL)

, m_spTransparentSceneRenderClick(NULL)

, m_spWeatherRenderStep(NULL)

, m_spPostProcessRenderStep(NULL)
, m_pkSpaceWarpEffect(NULL)
,m_pkOutlineEffect(NULL)
, m_pkBloomEffect(NULL)
, m_pkRadialBlurEffect(NULL)
, m_pkGreyEffect(NULL)
, m_bIsGreyEnable(false)
//, m_spSSAOEffect(NULL)
//, m_spClientNDPass(NULL)

, m_spUIRenderStep(NULL)
, m_spUE3DRenderClick(NULL)
, m_spUE2DRenderClick(NULL)

, m_spUIRenderClick(NULL)
, m_spCursorRenderClick(NULL)

, m_spScreenConsole(NULL)
, m_spScreenRenderStep(NULL)

, m_eLastRenderMode(RM_LOGIN_SCENE)

, m_bScreenStatVisibility(true)
, m_uiUpdateClicks(0)

, m_uiReflectObjects(0)
, m_fReflectRenderTime(0)
, m_fReflectCullTime(0)
, m_fLastReflectUpdate(0.0f)
, m_uiClipPlaneState(0)
, m_kReflectCuller(NULL)
, m_bSnapshot(false)
, m_bFullSnapshot(true)
, m_bTiledSnapshot(false)
, m_bTiledSnapshotMultiplier(5)
, m_SceneGreyState(SGFIOS_INACTIVE)
, m_fCurrentGrey(0)
, m_fFadeStartGrey(0)

, m_bIsNeedRecreateRender(false)
, m_fRecreateRenderTickTime(0.0f)

, m_pkReflectStaticShader(NULL)
, m_pkReflectDynamicShader(NULL)
, m_pkReflectTerrainShader(NULL)
, m_fFadeInMaxGrey(0.0f)
, m_fFadeInMinGrey(0.0f)
, m_fFadeStartTime(0.0f)
, m_fDeltaAlphaPerSecond(0.0f)
{
}

RG_RenderManager::~RG_RenderManager(void)
{
    T_ASSERT2(m_spNiRenderer == NULL, 
        "RG_RenderManager::UnInit() must be invoked before destructing!");
}

void RG_RenderManager::PostInit()
{

#if defined(T6_CLIENT)
	RS_ShaderManager::Init(true);
#else
	RS_ShaderManager::Init(false);
#endif

	g_settings.PostLoad();

	m_bScreenStatVisibility = g_settings.PROFILE_DISPLAY;

	T_NEW_D RenderManager;
}

bool RG_RenderManager::Init(void)
{
	T_ASSERT2(m_spNiRenderer == NULL, 
        "RG_RenderManager::Init() has already invoked before!");

	//@ 初始化设备信息
	{
		// 等待活动桌面是否恢复
		_checkDesktopState(10000);
		// 配置gameSetting中的设备信息
		bool bResult = g_settings.InitializeDeviceInfo();
		if ( !bResult )
		{
			return false;
		}

		// 异步初始化，不设置当前分辨率的话，字体不能适配 rev.26967
		UT_Vec3Int kRes = g_settings.GetResolution();
		g_settings.SetCurrentResolution( kRes.x , kRes.y);

		GfxWriteLog( LOG_TAG_RENDER, LOG_SWITCH_RENDER, 
			"Initialize Resolution ---- [%d, %d].", kRes.x, kRes.y );
	}


	//@ 初始化渲染器
	{
		// NiShadowManager 必须在 NiRenderer 之前初始化，否则阴影系统在渲染时会找不到自动生成的 shader 而崩溃
		NiShadowManager::Initialize();

		// 等待活动桌面是否恢复
		_checkDesktopState(10000);
		//创建和reset必须在同一线程
		if (!_createNiRenderer())
		{
			return false;
		}
	}

	return true;
}

void RG_RenderManager::PreUnInit()
{
	//postEffect影响到MaterialManager的释放, 先放到这里来
	_destroyClicksOutputToPostProcessing();
	if ( m_spPostProcessRenderStep )
		m_spPostProcessRenderStep->RemoveAllEffects();
	
}

bool RG_RenderManager::UnInit()
{
	m_spRenderFrame = NULL;

	m_spScreenConsole = NULL;
	m_spScreenRenderStep = NULL;

	// ui
	m_spUIRenderStep = NULL;
	m_spUE3DRenderClick = NULL;
	m_spUE2DRenderClick = NULL;
	m_spUIRenderClick = NULL;
	m_spCursorRenderClick = NULL;

	//postEffect
	_destroyClicksOutputToPostProcessing();
	m_spPostProcessRenderStep = NULL;
	m_pkSpaceWarpEffect = NULL;
	m_pkOutlineEffect = NULL;
	m_pkBloomEffect = NULL;
	m_pkRadialBlurEffect = NULL;
	m_pkGreyEffect = NULL;
	//m_spSSAOEffect = NULL;
	//m_spClientNDPass = NULL;

	// Fx
	m_spNormalFxRenderStep = NULL;
	// Weather
	m_spWeatherRenderStep = NULL;

	// NormalScene
	m_spMainSceneRenderStep = NULL;
	m_spNormalSceneRenderClick = NULL;
	m_spTransparentSceneRenderClick = NULL;
	m_spSceneRenderView = NULL;
	m_spInifiniteRenderView = NULL;
	m_spGrassRenderClick = NULL;
	m_spSceneDepthRenderClick = NULL;
	m_spSceneDepthRenderedTex = NULL;
	m_spSceneDepthRTG = NULL;

	// Shadow
	m_spShadowRenderStep = NULL;
	NiShadowManager::Shutdown();
	
	// Avatar
	m_spAvatarRenderStep = NULL;
	m_spAvatarRenderClick = NULL;
	m_spSmallAvatarRenderClick = NULL;
	m_spNoDepthBufferAvatarRenderClick = NULL;
	m_spAvatarBloomEffect = NULL;
	m_spAvatarOuterGlow = NULL;

	// Reflect
	m_spReflectStaticMaterialSwapper = NULL;
	m_spReflectDynamicMaterialSwapper = NULL;
	m_spTerrainReflectMaterialSwapper = NULL;

	if ( m_spWaterReflectRenderedTexture )
	{
		m_spNiRenderer->PurgeTexture(m_spWaterReflectRenderedTexture);
		m_spWaterReflectRenderedTexture = NULL;
	}
	m_spWaterReflectRenderTargetGroup = NULL;

	// Refract
	if ( m_spWaterRefractRenderedTexture )
	{
		m_spNiRenderer->PurgeTexture(m_spWaterRefractRenderedTexture);
		m_spWaterRefractRenderedTexture = NULL;
	}
	m_spWaterRefractRenderTargetGroup = NULL;

	// Shader
	RS_ShaderManager::UnInit();

	// Base 
	if ( RenderManager::GetSingletonPtr() )
	{
		RenderManager::GetSingleton().Shutdown();
		RenderManager::DestroyInstance();
	}
	_destoryNiRenderer();

	return true;
}

void RG_RenderManager::CreateRenderFrame(void)
{
    T_ASSERT2(m_spRenderFrame == NULL, 
        "Main render frame has already been created before!");

	// check dx device caps to see if the features we required are supported before other initialization
	// if not, some of the critical features will be disabled forcibly to improve the stability
	// the normal one could be controlled by config manually
	// users could have a better look game in low machine if they can bear slow FPS
	g_settings.CheckD3DCaps();

    m_spRenderFrame = NiNew NiRenderFrame;

	//long lAutoConfigLevel = UT_ConfigManager::GetSingletonPtr()->GetAutoConfigLevel();

	// avatar
	_createAvatarRenderStep();

	if (g_settings.CFG_ENABLE_SCENE_OBJECT_SHADOW)
	{
        // 阴影
	    _createShadowRenderStep();
	}
    // 主场景
    _createMainSceneRenderStep();
	// 普通特效
	_createNormalFxRenderStep();
	if (g_settings.CFG_ENABLE_WEATHER_EFFECT)
	{
	    // 天气
	    _createWeatherRenderStep();
	}
	// 后期处理
	_createPostProcessing();
    // UI
    _createUiRenderStep();
    // 屏幕调试信息
    _createStatsInfoRenderStep();

	if (g_settings.LOD_ENABLE_REFLECT)
	{
	    // 渲染反射图的时候使用简单的Shader
	    m_pkReflectStaticShader = NiSingleShaderMaterial::Create("StandardSceneObjectStaticReflect");
		m_pkReflectDynamicShader = NiSingleShaderMaterial::Create("StandardSceneObjectSkinnedReflect");
		m_pkReflectTerrainShader = NiSingleShaderMaterial::Create("TerrainShadingReflect");
		// 需要使用一个Swapper来包含SingleShaderMaterial对象否则会有Ref Count变成0造成崩溃的问题
		m_spReflectStaticMaterialSwapper = NiNew NiMaterialSwapProcessor(m_pkReflectStaticShader);
		m_spReflectDynamicMaterialSwapper = NiNew NiMaterialSwapProcessor(m_pkReflectDynamicShader);
		m_spTerrainReflectMaterialSwapper = NiNew NiMaterialSwapProcessor(m_pkReflectTerrainShader);
	}
}

void RG_RenderManager::Recreate(void)
{
    if (m_spNiRenderer == NULL)
    {
        return;
    }
    // 重新创建 renderer
	GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
		"begin renderer recreation");

    if (!_createNiRenderer())
		return;

    if (m_spPostProcessRenderStep)
    {
		_preResetClicksOutputToPostProcessing();
        m_spPostProcessRenderStep->HandleReset();
        _postResetClicksOutputToPostProcessing();
    }
	if (m_spAvatarBloomEffect)
		m_spAvatarBloomEffect->HandleReset();
	if (m_spAvatarOuterGlow)
		m_spAvatarOuterGlow->HandleReset();

    CA_CameraManager::GetSingleton().OnRendererResize();
    FX_ClientFxManager::GetSingleton().OnRenderTargetResized();

    UI_UIManager::GetSingleton().GameUIResolutionChange(m_nWidth, m_nHeight);
	if(UE_UiEffectManager::GetSingletonPtr())
		UE_UiEffectManager::GetSingleton().OnResolutionChanged(m_nWidth, m_nHeight);


	UI_AvatarManager* avatarManager = UI_AvatarManager::GetSingletonPtr();
	if ( avatarManager )
	{
		/// 清除引用计数
		m_spAvatarRenderClick->SetRenderTargetGroup(NULL);
		m_spSmallAvatarRenderClick->SetRenderTargetGroup(NULL);
		m_spNoDepthBufferAvatarRenderClick->SetRenderTargetGroup(NULL);

		UI_AvatarRender * pkAvatarRenderer = avatarManager->getRender();
		pkAvatarRenderer->HandleReset();

		/// 重设新RT的引用计数
		pkAvatarRenderer->ResetRenderTargetGroup(m_spAvatarRenderClick, RENDER_CLICK_ORIGIN);
		pkAvatarRenderer->ResetRenderTargetGroup(m_spSmallAvatarRenderClick, RENDER_CLICK_SMALL);
		pkAvatarRenderer->ResetRenderTargetGroup(m_spNoDepthBufferAvatarRenderClick, RENDER_CLICK_NODEPTHBUFFER);
	}

	GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
		"end renderer recreation");
}

//void RG_RenderManager::AppendCursor(NiCursorPtr spCursor)
//{
//    m_spCursorRenderClick->AppendCursor(spCursor);
//}

bool RG_RenderManager::RecvMessage(MG_MessageBase& rkMessage)
{
    MG_MessageHandlerInterface::RecvMessage(rkMessage);

    BEGIN_MESSAGE_HANDLE
        MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
		MESSAGE_HANDLE(MGT_WIN32_MESSAGE, _eventWindowMessage)
		MESSAGE_HANDLE(MGT_WINDOWS_LOST_FOCUS, _eventWindowDeactive)
        //MESSAGE_HANDLE(MGT_INPUT_KEY_ADD, _eventReloadSettings);
    END_MESSAGE_HANDLE

    return true;
}

void RG_RenderManager::SetRenderActive( bool isActive )
{
	m_bIsActiveRender = isActive;
}

bool RG_RenderManager::IsRenderActive() const
{
	return m_bIsActiveRender;
}

void RG_RenderManager::SwitchRenderMode(RenderMode eRenderMode)
{
    unsigned int maskRenderStepActivity = RSA_NONE;

    switch (eRenderMode)
    {
    case RM_LOGIN_SCENE:
        maskRenderStepActivity = RSA_LOGIN_SCENE_1;
    	break;
    case RM_CHAR_LIST_SCENE:
        T_ASSERT(m_eLastRenderMode == RM_LOGIN_SCENE 
            || m_eLastRenderMode == RM_CREATE_CHAR_SCENE 
            || m_eLastRenderMode == RM_LOADING_SCENE 
            || m_eLastRenderMode == RM_GAME);
        maskRenderStepActivity = RSA_LOGIN_SCENE_1;
        break;
    case RM_CREATE_CHAR_SCENE:
        T_ASSERT(m_eLastRenderMode == RM_CHAR_LIST_SCENE
            || m_eLastRenderMode == RM_CREATE_CHAR_SCENE
			|| m_eLastRenderMode == RM_LOGIN_SCENE
			|| m_eLastRenderMode == RM_LOADING_SCENE);
        maskRenderStepActivity = RSA_LOGIN_SCENE_1;
        break;
    case RM_LOADING_SCENE:
#ifndef _MAP_VIEWER_BUILD
        T_ASSERT(m_eLastRenderMode == RM_CHAR_LIST_SCENE 
            || m_eLastRenderMode == RM_GAME
			|| m_eLastRenderMode == RM_CREATE_CHAR_SCENE
			|| m_eLastRenderMode == RM_LOGIN_SCENE
			|| m_eLastRenderMode == RM_LOADING_SCENE);
#endif // _MAP_VIEWER_BUILD
        maskRenderStepActivity = RSA_LOGIN_SCENE_1;
        break;
    case RM_GAME:
        //T_ASSERT(m_eLastRenderMode == RM_LOADING_SCENE);
        maskRenderStepActivity = RSA_IN_GAME;
		if ( !g_settings.CFG_ENABLE_WEATHER_EFFECT )
		{
			maskRenderStepActivity = maskRenderStepActivity^RSA_WEATHER;
		}
        break;
    default:
        break;
    }
    _updateRenderStepsActivities(maskRenderStepActivity);

    m_eLastRenderMode = eRenderMode;
}

void RG_RenderManager::ClearAvatarRenderTargetGroup()
{
	if ( m_spAvatarRenderClick )
		m_spAvatarRenderClick->SetRenderTargetGroup(NULL);

	if(m_spSmallAvatarRenderClick)
		m_spSmallAvatarRenderClick->SetRenderTargetGroup(NULL);

	if(m_spNoDepthBufferAvatarRenderClick)
		m_spNoDepthBufferAvatarRenderClick->SetRenderTargetGroup(NULL);

	if (m_spAvatarBloomEffect)
	{
		m_spAvatarBloomEffect->CleanupEffect();
		m_spAvatarBloomEffect = NULL;
	}
	if (m_spAvatarOuterGlow)
	{
		m_spAvatarOuterGlow->CleanupEffect();
		m_spAvatarOuterGlow = NULL;
	}
}

void RG_RenderManager::SetGreyScenePostProcActivity(bool bActive)
{
	if (m_pkGreyEffect != NULL && m_pkGreyEffect->CanActiveEffect())
	{
		m_pkGreyEffect->SetActive(bActive);
		_updateClicksOutputToPostProcessing();
	}
}

void RG_RenderManager::SetRadialBlurPostProcActivity(bool bActive)
{
    if (m_pkRadialBlurEffect != NULL && m_pkRadialBlurEffect->CanActiveEffect())
    {
        m_pkRadialBlurEffect->SetActive(bActive);
        if (bActive)
        {
            m_pkRadialBlurEffect->ResetEffect();
        }

        _updateClicksOutputToPostProcessing();
    }
}

void RG_RenderManager::SetScreenConsoleVisible(bool bVisible)
{
    m_bScreenStatVisibility = bVisible;

    m_spScreenRenderStep->SetActive(m_bScreenStatVisibility);

	if ( UT_StatisticsManager::GetSingletonPtr() )
	{
		UT_StatisticsManager::GetSingleton().SetShowMainLoopStat( g_settings.PROFILE_SHOW_MAIN_LOOP_STAT & bVisible );
	}
}

void RG_RenderManager::Snapshot(bool bWithUI, bool bTiled)
{
    m_bSnapshot = true;
    m_bFullSnapshot = bWithUI;
	m_bTiledSnapshot = bTiled;
	if (m_bTiledSnapshot)
	{
		m_bSnapshot = false;
	}

    if (!m_bFullSnapshot)
    {
		UI_UIManager::GetSingleton().SetCurrentScreenVisibility(false);
		if (m_bTiledSnapshot)
		{
			m_spUIRenderStep->SetActive(false);
			m_spUE3DRenderClick->SetActive(false);
			m_spUE2DRenderClick->SetActive(false);
		}
    }
}

void RG_RenderManager::ShieldWeather(bool bValue)
{
	if ( m_spWeatherRenderStep )
	{
		m_spWeatherRenderStep->SetActive(bValue);
	}
}

/// 当设备丢失时，回调的函数
bool RG_RenderManager::_callbackResetNotification(bool bBeforeReset, void* pvData)
{
    if (!bBeforeReset)
    {
        // 全屏模式模式下，窗体切换需要重置窗体，不然标题栏又被默认设上导致鼠标定位偏移
		if (!g_dllRunInBrowser)
		{
			SetupMainWindow(g_hWnd);
		}

        RG_RenderManager* pkThis = (RG_RenderManager*)pvData;
        pkThis->Recreate();

		if( BL_AsyncOperationManager::GetSingletonPtr() )
		{
			BL_AsyncOperationManager::GetSingleton().SuspendLoading( false );
		}
    }
	else
	{
		//第一时间停止后台线程的加载，防止多线程问题
		if( BL_AsyncOperationManager::GetSingletonPtr() )
		{
			BL_AsyncOperationManager::GetSingleton().SuspendLoading( true );
		}
	}

    return true;
}

bool RG_RenderManager::_callbackLostDeviceNotification(void* pvData)
{
    RG_RenderManager* pkThis = (RG_RenderManager*)pvData;
	//UI_UIManager::GetSingleton().GameUIResolutionChange(m_nWidth, m_nHeight);
    pkThis->m_bIsDeviceLost = true;
	

    return true;
}

bool RG_RenderManager::_callbackShadowRenderStepPreProc(NiRenderStep* pkCurrentStep, void* pvCallbackData)
{
    T_PROFILE("Render::PreRenderShadow");

    // Get the list of render clicks from the shadow manager.
    const NiTPointerList<NiRenderClick*>& kShadowClicks = NiShadowManager::GenerateRenderClicks();

	{
		const NiTPointerList<NiShadowGeneratorPtr>& kGenerators = NiShadowManager::GetActiveShadowGenerators();
		NiTListIterator kIter = kGenerators.GetHeadPos();
		while (kIter)
		{
			NiShadowGenerator* pkGenerator = kGenerators.GetNext(kIter);
			if (!pkGenerator || !pkGenerator->GetActive())
			{
				g_settings.SHADOW_BLEND_FAKE = false;
				g_settings.PostLoad();

				return false;
			}
		}
	}

	//-------------------------------------------------
	/*
		@ 由于Effect中会修改到D3DSAMP_MIPMAPLODBIAS这个值，
		而使用了NiFragmentMaterial的绘制体不会去更新这个值（也许是我没有发现），要自己手动更新。

		add by yadong.xu
		2016\8\16
	*/
	
	NiDX9Renderer* pkRenderer = NiDynamicCast(
		NiDX9Renderer, NiRenderer::GetRenderer());

	if ( pkRenderer && 
		pkRenderer->GetRenderState() )
	{
		NiDX9RenderState* pkRenderState = pkRenderer->GetRenderState();
		for (unsigned int i = 0; i < NiD3DPass::ms_uiMaxSamplers; i++)
			pkRenderState->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, 0);
	}
	//-------------------------------------------------

    // Replace the render clicks in the shadow render step with those provided
    // by the shadow manager.
    NIASSERT(NiIsKindOf(NiDefaultClickRenderStep, pkCurrentStep));
    NiDefaultClickRenderStep* pkClickRenderStep = (NiDefaultClickRenderStep*)pkCurrentStep;
    pkClickRenderStep->GetRenderClickList().RemoveAll();
    NiTListIterator kIter = kShadowClicks.GetHeadPos();
    while (kIter)
    {
		NiRenderClick* pkClick = kShadowClicks.GetNext(kIter);
		NiShadowClickValidator* pkValidator = (NiShadowClickValidator*)pkClick->GetValidator();
		pkValidator->SetCullingProcess( NULL );
		NiViewRenderClick* pkShadowClick = (NiViewRenderClick*)pkClick;
		Ni3DRenderViewPtr spView = NiSmartPointerCast( Ni3DRenderView , ( pkShadowClick->GetRenderViews().GetHead() ) );
		spView->SetCullingProcess( NULL );

        
        pkClick->SetClearAllBuffers(true);
        pkClickRenderStep->AppendRenderClick(pkClick);
    }

    return true;
}

bool RG_RenderManager::_callbackMainSceneRenderStep_DepthClickPreProc(NiRenderClick* pkCurrentRenderClick, void* pvCallbackData)
{
	bool isNeedDepthRendering = ( FX_ClientFxManager::GetSingleton().NeedSceneDepth() ||
								!RenderManager::GetSingleton().IsDecalsEmpty() ||
								( SoftParticlesManager::GetInstance() && !SoftParticlesManager::GetInstance()->IsParticlesEmpty() ) );
								

     if ( isNeedDepthRendering )
     {
         const NiTransform& kCurrentCameraXlate = CA_CameraManager::GetSingleton().GetNiCamera()->GetLocalTransform();
         RG_RenderManager& rkRenderManager = RG_RenderManager::GetSingleton();
         if (rkRenderManager.m_kLastCameraXlate != kCurrentCameraXlate)
         {
             rkRenderManager.m_kLastCameraXlate = kCurrentCameraXlate;
            return true;
         }
     }
     return false;
}

bool RG_RenderManager::_callbackMainSceneRenderStep_SoftParticalesClickPreProc(NiRenderClick* pkCurrentRenderClick, void* pvCallbackData)
{
	/*
		@ 由于Effect中会修改到D3DSAMP_MIPMAPLODBIAS这个值，
		而使用了NiFragmentMaterial的绘制体不会去更新这个值（也许是我没有发现），要自己手动更新。

		add by yadong.xu
		2016\8\16
	*/
	
	NiDX9Renderer* pkRenderer = NiDynamicCast(
		NiDX9Renderer, NiRenderer::GetRenderer());

	if ( pkRenderer && 
		pkRenderer->GetRenderState() )
	{
		NiDX9RenderState* pkRenderState = pkRenderer->GetRenderState();
		for (unsigned int i = 0; i < NiD3DPass::ms_uiMaxSamplers; i++)
			pkRenderState->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, 0);
	}

	return true;
}

//bool RG_RenderManager::_callbackUiRenderStepPreProc(NiRenderStep* pkCurrentStep, void* pvCallbackData)
//{
//	/*
//		@ 由于Effect中会修改到D3DSAMP_MIPMAPLODBIAS这个值，
//		而使用了NiFragmentMaterial的绘制体不会去更新这个值（也许是我没有发现），要自己手动更新。
//
//		add by yadong.xu
//		2016\8\16
//	*/
//	
//	NiDX9Renderer* pkRenderer = NiDynamicCast(
//		NiDX9Renderer, NiRenderer::GetRenderer());
//
//	if ( pkRenderer && 
//		pkRenderer->GetRenderState() )
//	{
//		NiDX9RenderState* pkRenderState = pkRenderer->GetRenderState();
//		for (unsigned int i = 0; i < NiD3DPass::ms_uiMaxSamplers; i++)
//			pkRenderState->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, 0);
//	}
//}


bool RG_RenderManager::_createNiRenderer(void)
{
	m_bIsDeviceLost = false;
	m_bIsFullScreen = g_settings.GetBoolSetting( UT_GlobalSettings::GS_FULLSCREEN );
    GfxGetClientWindowConfig(m_nWidth, m_nHeight, m_bIsFullScreen);
	if(g_dllRunInBrowser)
	{
		g_settings.SetCurrentResolution(m_nWidth, m_nHeight);
	}
	//如果是全屏模式，那么要把分辨率改成最大的，并用窗口创建
	m_bIsFullScreen = false;

    unsigned int uiUseFlag = m_bIsFullScreen
        ? (NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL | NiDX9Renderer::USE_FULLSCREEN)
        : (NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL);

    unsigned int uiAdapter = D3DADAPTER_DEFAULT;
    NiDX9Renderer::DeviceDesc eDevType = NiDX9Renderer::DEVDESC_PURE;

    NiDX9Renderer::PresentationInterval ePresentInterval =  g_settings.CFG_ENABLE_VSYNC 
        ? NiDX9Renderer::PRESENT_INTERVAL_ONE 
        : NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE;

	NiDX9Renderer::FrameBufferFormat eBufferFmt = NiDX9Renderer::FBFMT_UNKNOWN;
	NiDX9Renderer::DepthStencilFormat eDepthFmt = NiDX9Renderer::DSFMT_UNKNOWN;

	if (m_spNiRenderer == NULL)
    {
        m_spNiRenderer = NiDX9InstanceRenderer::Create(
            m_nWidth, m_nHeight, 
            uiUseFlag, 
            g_hWnd, g_hWnd, 
            uiAdapter, 
            eDevType,
            eBufferFmt, 
            eDepthFmt,
            ePresentInterval,
            NiDX9Renderer::SWAPEFFECT_DEFAULT,
            g_settings.CFG_MSAA_LEVEL);

        if (m_spNiRenderer == NULL)
        {
            return false;
        }

        m_spNiRenderer->AddResetNotificationFunc(_callbackResetNotification, this);
        m_spNiRenderer->AddLostDeviceNotificationFunc(_callbackLostDeviceNotification, this);
    } 
    else
    {
        m_spNiRenderer->RemoveResetNotificationFunc(_callbackResetNotification);

        NiDX9Renderer::RecreateStatus eRet = m_spNiRenderer->Recreate(
            m_nWidth, m_nHeight, 
            uiUseFlag, 
            g_hWnd, 
            NiDX9Renderer::FBFMT_UNKNOWN, 
            NiDX9Renderer::DSFMT_UNKNOWN,
            ePresentInterval,
            NiDX9Renderer::SWAPEFFECT_DEFAULT,
            g_settings.CFG_MSAA_LEVEL);

        m_spNiRenderer->AddResetNotificationFunc(_callbackResetNotification, this);
		return eRet == NiDX9Renderer::RECREATESTATUS_OK;
    }
	

#ifdef _DISABLE_PIXWIN
	//禁用PIX
	D3DPERF_SetOptions(1);
#endif

    return true;
}

void RG_RenderManager::_destoryNiRenderer(void)
{
    m_spNiRenderer = NULL;
}

void RG_RenderManager::_createAvatarRenderStep(void)
{
	m_spAvatarRenderStep = NiNew RG_ClientRenderStep("Render::Avatar");
	m_spRenderFrame->AppendRenderStep(m_spAvatarRenderStep);

	m_spAvatarRenderClick = NiNew RG_AvatarRenderClick;
	m_spAvatarRenderClick->SetBgColorInfo(false);
	m_spAvatarRenderClick->SetIndex(RENDER_CLICK_ORIGIN);
	m_spAvatarRenderClick->SetName("Avatar Render Click");
	m_spAvatarRenderClick->SetClearAllBuffers(true);

	m_spSmallAvatarRenderClick = NiNew RG_AvatarRenderClick;
	m_spSmallAvatarRenderClick->SetBgColorInfo(false);
	m_spSmallAvatarRenderClick->SetIndex(RENDER_CLICK_SMALL);
	m_spSmallAvatarRenderClick->SetName("Avatar Small Render Click");
	m_spSmallAvatarRenderClick->SetClearAllBuffers(true);

	m_spNoDepthBufferAvatarRenderClick = NiNew RG_AvatarRenderClick;
	m_spNoDepthBufferAvatarRenderClick->SetBgColorInfo(false);
	m_spNoDepthBufferAvatarRenderClick->SetIndex(RENDER_CLICK_NODEPTHBUFFER);
	m_spNoDepthBufferAvatarRenderClick->SetName("Avatar NoDepthBuffer Render Click");
	m_spNoDepthBufferAvatarRenderClick->SetClearAllBuffers(true);

	UI_AvatarManager* avatarManager = UI_AvatarManager::GetSingletonPtr();
	if ( avatarManager )
	{
		UI_AvatarRender * pkAvatarRenderer = avatarManager->getRender();

		pkAvatarRenderer->ResetRenderTargetGroup(m_spAvatarRenderClick, RENDER_CLICK_ORIGIN);
		pkAvatarRenderer->ResetRenderTargetGroup(m_spSmallAvatarRenderClick, RENDER_CLICK_SMALL);
		pkAvatarRenderer->ResetRenderTargetGroup(m_spNoDepthBufferAvatarRenderClick, RENDER_CLICK_NODEPTHBUFFER);
	}

	m_spAvatarRenderStep->AppendRenderClick(m_spAvatarRenderClick);
	m_spAvatarRenderStep->AppendRenderClick(m_spSmallAvatarRenderClick);
	m_spAvatarRenderStep->AppendRenderClick(m_spNoDepthBufferAvatarRenderClick);

	//////////////////////////////////////////////////////////////////////////
	// avatar的bloom效果
	if (g_settings.CFG_ENABLE_POST_PROCESS)
	{
		UI_AvatarRender * pkAvatarRenderer = avatarManager->getRender();

		RG_RenderTexture * pkRT = pkAvatarRenderer->getRenderTexture();
		m_spAvatarBloomEffect = NiNew RG_AvatarBloomEffect(true, pkRT);
		bool bSuccess = m_spAvatarBloomEffect->CreateEffect();
		if (bSuccess)
			m_spRenderFrame->AppendRenderStep(m_spAvatarBloomEffect);

		// 外发光
		RG_RenderTexture * pkRTs = pkAvatarRenderer->getRenderSmallTexture();
		RG_RenderTexture * pkRTc = pkAvatarRenderer->getRenderTextureColored();
		m_spAvatarOuterGlow = NiNew RG_AvatarOuterGlow(true, pkRTs, pkRTc);
		bSuccess = m_spAvatarOuterGlow->CreateEffect();
		if (bSuccess)
			m_spRenderFrame->AppendRenderStep(m_spAvatarOuterGlow);
	}
}

void RG_RenderManager::_createShadowRenderStep(void)
{
    m_spShadowRenderStep = NiNew RG_ClientRenderStep("Render::Shadow");
    m_spShadowRenderStep->SetPreProcessingCallbackFunc(&RG_RenderManager::_callbackShadowRenderStepPreProc);

    m_spRenderFrame->AppendRenderStep(m_spShadowRenderStep);
}

void RG_RenderManager::_createMainSceneRenderStep(void)
{
	m_spMainSceneRenderStep = NiNew RG_ClientRenderStep("Render::Scene"); 
	m_spRenderFrame->AppendRenderStep(m_spMainSceneRenderStep);

	//@ 水面（这部分以后挪出）
	if (g_settings.LOD_ENABLE_REFLECT) 
	{ 
		_createWaterReflect(); 
	}

	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();

    //@ 普通场景
	m_spNormalSceneRenderClick = NiNew RG_ClientRenderClick("Render::Scene::MainSceneClick");
	m_spSceneRenderView = NiNew RG_SceneRenderView;
	m_spInifiniteRenderView = NiNew RG_SceneInfiniteRenderView;
	//NiAlphaSortProcessor* spSorter = NiNew NiAlphaSortProcessor;
	//m_spNormalSceneRenderClick->SetProcessor( spSorter );
	//m_spBatchRenderer = NiNew RG_SceneBatchRenderProcessor;
	//m_spNormalSceneRenderClick->SetProcessor( m_spBatchRenderer );
	m_spNormalSceneRenderClick->AppendRenderView( m_spInifiniteRenderView );
    m_spNormalSceneRenderClick->AppendRenderView(m_spSceneRenderView);
    m_spNormalSceneRenderClick->SetViewport(spCamera->GetViewPort());
    m_spNormalSceneRenderClick->SetClearAllBuffers(true);
	m_spNormalSceneRenderClick->SetUseRendererBackgroundColor(false);
	m_spNormalSceneRenderClick->SetBackgroundColor( NiColorA( 0.0f , 0.0f , 0.0f , 0.0f ) );

	m_spMainSceneRenderStep->AppendRenderClick(m_spNormalSceneRenderClick);
	m_kClicksOutputToPostProcessing.push_back(m_spNormalSceneRenderClick);
  
    //@ 场景深度图
	if (g_settings.LOD_SCENE_DEPTH_TEXTURE_CLICK)
	{
		m_spSceneDepthRenderClick = NiNew RG_SceneDepthRenderClick;
		m_spSceneDepthRenderClick->SetName("Scene Depth Render Click");
		m_spSceneDepthRenderClick->SetPreProcessingCallbackFunc(_callbackMainSceneRenderStep_DepthClickPreProc);
		m_spSceneDepthRenderClick->SetClearAllBuffers(true);
		m_spSceneDepthRenderClick->SetUseRendererBackgroundColor(false);
		m_spSceneDepthRenderClick->SetBackgroundColor(NiColorA(0.0f, 0.0f, 0.0f, 0.0f));

		NiMaterialSwapProcessorPtr spProcessor = NiNew NiMaterialSwapProcessor(NiSingleShaderMaterial::Create("TerrainDepth"));
		m_spSceneDepthRenderClick->SetProcessor(spProcessor);
	}

	RenderManager::Instance()->SetCamera(spCamera);
	RenderManager::Instance()->Recreate(m_spSceneDepthRenderClick, g_settings.LOD_SCENE_DEPTH_TEXTURE_CLICK);

	if(g_settings.LOD_SCENE_DEPTH_TEXTURE_CLICK)
		m_spMainSceneRenderStep->AppendRenderClick(RenderManager::Instance()->modelDepthClick_);

	//@ 草
	m_spGrassRenderClick = NiNew RG_GrassRenderClick;
	m_spGrassRenderClick->SetName( "Grass Render Click" );

	m_kClicksOutputToPostProcessing.push_back(m_spGrassRenderClick);
	m_spMainSceneRenderStep->AppendRenderClick(m_spGrassRenderClick);

    //@ 3D UE 需要在主场景中渲染，而不是跟着 UI step
    m_spUE3DRenderClick = NiNew RG_ClientRenderClick( "Render::Scene::UEClick" );
    m_spUE3DRenderClick->AppendRenderView(NiNew RG_UiEffectRenderView);

	m_kClicksOutputToPostProcessing.push_back(m_spUE3DRenderClick);
	m_spMainSceneRenderStep->AppendRenderClick(m_spUE3DRenderClick);

	//@ 软粒子
	if(g_settings.ENABLE_SOFTPARTICLE)
	{
		SoftParticlesManagerPtr spSoftParticle = RenderManager::Instance()->softParticlesManager_;
		spSoftParticle->SetPreProcessingCallbackFunc(
			_callbackMainSceneRenderStep_SoftParticalesClickPreProc);

		m_spMainSceneRenderStep->AppendRenderClick(spSoftParticle);
		m_kClicksOutputToPostProcessing.push_back(spSoftParticle);
	}

	//@ 天气
	if(g_settings.CFG_ENABLE_WEATHER_EFFECT)
	{
		if (WeatherManager::Instance())
			WeatherManager::Instance()->SetHeightFieldHeightFunc(&FX_ClientFxManager::_getHeightFieldHeight);

		WeatherManagerPtr spWeather = RenderManager::Instance()->weatherManager_;
		m_spMainSceneRenderStep->AppendRenderClick(spWeather);
		m_kClicksOutputToPostProcessing.push_back(spWeather);
	}
}

void RG_RenderManager::_createNormalFxRenderStep(void)
{
	FX_ClientFxManager::GetSingleton().SetSceneDepthRenderedTexture(RenderManager::Instance()->depthRenderedTex_);

    NiViewRenderClickPtr spNormalFxRenderClick = FX_ClientFxManager::GetSingleton().CreateNormalRenderClick();

    m_kClicksOutputToPostProcessing.push_back(spNormalFxRenderClick);

    m_spNormalFxRenderStep = NiNew RG_ClientRenderStep("Render::FX::Normal");
    m_spNormalFxRenderStep->AppendRenderClick(spNormalFxRenderClick);

	m_spTransparentSceneRenderClick = NiNew NiViewRenderClick;
	m_spTransparentSceneRenderClick->SetName( "Transparent Render Click" );
	m_spTransparentSceneRenderClick->AppendRenderView( NiNew RG_SceneTransparentRenderView );
	m_spNormalFxRenderStep->AppendRenderClick( m_spTransparentSceneRenderClick );

	m_kClicksOutputToPostProcessing.push_back( m_spTransparentSceneRenderClick );

    m_spRenderFrame->AppendRenderStep(m_spNormalFxRenderStep);
}

void RG_RenderManager::_createWeatherRenderStep(void)
{
    NiRenderClickPtr spWeatherRenderClick = NiNew TS_WeatherRenderClick(
        SE_SceneManager::GetSingleton().GetWeatherManager(), CA_CameraManager::GetSingleton().GetNiCamera());

    m_kClicksOutputToPostProcessing.push_back(spWeatherRenderClick);
	spWeatherRenderClick->SetName( "Weather Render Click" );

    m_spWeatherRenderStep = NiNew RG_ClientRenderStep("Render::Weather");
    m_spWeatherRenderStep->AppendRenderClick(spWeatherRenderClick);
    m_spWeatherRenderStep->SetActive(g_settings.CFG_ENABLE_WEATHER_EFFECT);

    m_spRenderFrame->AppendRenderStep(m_spWeatherRenderStep);
}

void RG_RenderManager::_createPostProcessing(void)
{
	bool bPostProcess = g_settings.CFG_ENABLE_POST_PROCESS;

	//m_spClientNDPass = NiNew RG_ClientNDPass;
	//m_spSSAOEffect = NiNew RG_SSAOEffect( true , m_spClientNDPass );
    m_pkSpaceWarpEffect = NiNew RG_SpaceWarpEffect(false);

	//这个OutlineEffect描边特效现在没用了，所以干掉
	//m_pkOutlineEffect = NiNew RG_OutlineEffect(false);

	if (bPostProcess)
		m_pkBloomEffect = NiNew RG_BloomEffect(true);
	
	//这个RadialBlurEffect只有用在山洞壁画的特殊功能里面，现在根本看不到，我何必要创建这个东西   
	//m_pkRadialBlurEffect = NiNew RG_RadialBlurEffect(false);
	
	if (bPostProcess)
		m_pkGreyEffect = NiNew RG_GreyEffect(true);

    m_spPostProcessRenderStep = NiNew RG_PostProcessManager;
	m_spPostProcessRenderStep->SetName("Render::PostProcess");
	//m_spPostProcessRenderStep->AddEffect( m_spSSAOEffect );
    m_spPostProcessRenderStep->AddEffect(m_pkSpaceWarpEffect);

	if (bPostProcess)
		m_spPostProcessRenderStep->AddEffect(m_pkBloomEffect);
	
	//这个RadialBlurEffect只有用在山洞壁画的特殊功能里面，现在根本看不到，我何必要创建这个东西
	//m_spPostProcessRenderStep->AddEffect(m_pkRadialBlurEffect);
	
	if (bPostProcess)
		m_spPostProcessRenderStep->AddEffect(m_pkGreyEffect);
	/// 还必须放最后边
	//这个OutlineEffect描边特效现在没用了，所以干掉
	//m_spPostProcessRenderStep->AddEffect(m_pkOutlineEffect);


    m_spRenderFrame->AppendRenderStep(m_spPostProcessRenderStep);

    _updateClicksOutputToPostProcessing();
}

void RG_RenderManager::_createUiRenderStep(void)
{
#ifndef _MAP_VIEWER_BUILD
    m_spUE2DRenderClick = NiNew RG_UERenderClick;

    m_spScreenFxRenderClick = FX_ClientFxManager::GetSingleton().CreateScreenRenderClick();

   
#endif // _MAP_VIEWER_BUILD

    m_spUIRenderClick =	NiNew CUIRenderClick;
    m_spUIRenderClick->SetName("UI Render Click");

    m_spUIFxRenderClick = FX_ClientFxManager::GetSingleton().CreateUIRenderClick();

    //m_spCursorRenderClick = NiNew NiCursorRenderClick;
    //m_spCursorRenderClick->SetName("Cursor Render Click");

    m_spUIRenderStep = NiNew RG_ClientRenderStep("Render::UI");
#ifndef _MAP_VIEWER_BUILD
    m_spUIRenderStep->AppendRenderClick(m_spUE2DRenderClick);
    m_spUIRenderStep->AppendRenderClick(m_spScreenFxRenderClick);
#endif // _MAP_VIEWER_BUILD
    m_spUIRenderStep->AppendRenderClick(m_spUIRenderClick);
    m_spUIRenderStep->AppendRenderClick(m_spUIFxRenderClick);
    //m_spUIRenderStep->AppendRenderClick(m_spCursorRenderClick);

    m_spRenderFrame->AppendRenderStep(m_spUIRenderStep);
}

void RG_RenderManager::_createStatsInfoRenderStep(void)
{
    if (m_spScreenConsole == NULL)
    {
        UT_ScreenConsole::SetDefaultFontPath(".\\Data\\Editor\\");

        m_spScreenConsole = NiNew UT_ScreenConsole;
        m_spScreenConsole->SetDimensions(NiPoint2(1920.0f, 1200.0f));
        m_spScreenConsole->SetOrigin(NiPoint2(9.0f, 120.0f));
        m_spScreenConsole->SetFont(m_spScreenConsole->CreateConsoleFont());
        m_spScreenConsole->SetCamera(CA_CameraManager::GetSingleton().GetNiCamera());
    }

    NiScreenTexturePtr spScreenTexture = m_spScreenConsole->GetActiveScreenTexture();
    if (spScreenTexture != NULL)
    {
        NiScreenTextureRenderClickPtr spScreenConsoleRenderClick = NiNew NiScreenTextureRenderClick;
        spScreenConsoleRenderClick->AppendScreenTexture(spScreenTexture);

		/// 增加背景图，方便看清楚文字
		NiTexture* pkTexture = NiSourceTexture::Create(".\\Data\\Editor\\4p_black_0.5a.dds");
		NiScreenTexturePtr spConsoleBG = NiNew NiScreenTexture(pkTexture);
		spConsoleBG->SetClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
		NiPoint2 kOrigin = m_spScreenConsole->GetOrigin();
		spConsoleBG->AddNewScreenRect(kOrigin.y, 0, 800, 1280, 0, 0);
		spScreenConsoleRenderClick->PrependScreenTexture(spConsoleBG);

        m_spScreenRenderStep = NiNew RG_ClientRenderStep("Render::ScreenConsole");
        m_spScreenRenderStep->AppendRenderClick(spScreenConsoleRenderClick);

        m_spRenderFrame->AppendRenderStep(m_spScreenRenderStep);
    }
    SetScreenConsoleVisible(m_bScreenStatVisibility);
}


void RG_RenderManager::_preResetClicksOutputToPostProcessing(void)
{
	for (SpecialRenderTargetClickList::iterator itr = m_kClicksOutputToPostProcessing.begin(); 
		itr != m_kClicksOutputToPostProcessing.end(); ++itr)
	{
		(*itr)->SetRenderTargetGroup(NULL);
	}

	m_spPostProcessRenderStep->SetActive((false));
}

void RG_RenderManager::_postResetClicksOutputToPostProcessing(void)
{
	_updateClicksOutputToPostProcessing();
}

void RG_RenderManager::_updateClicksOutputToPostProcessing(void)
{
	if ( !m_spPostProcessRenderStep )
		return;

    NiRenderTargetGroupPtr spPostProcessingRTG = m_spPostProcessRenderStep->Init(0.0f);

    for (SpecialRenderTargetClickList::iterator itr = m_kClicksOutputToPostProcessing.begin(); 
        itr != m_kClicksOutputToPostProcessing.end(); ++itr)
    {
        (*itr)->SetRenderTargetGroup(spPostProcessingRTG);
    }

    m_spPostProcessRenderStep->SetActive((spPostProcessingRTG != NULL));
}

void RG_RenderManager::_destroyClicksOutputToPostProcessing(void)
{
	for (SpecialRenderTargetClickList::iterator itr = m_kClicksOutputToPostProcessing.begin(); 
		itr != m_kClicksOutputToPostProcessing.end(); ++itr)
	{
		(*itr)->SetRenderTargetGroup(NULL);
	}

	if ( m_spPostProcessRenderStep )
		m_spPostProcessRenderStep->SetActive((false));

	m_kClicksOutputToPostProcessing.clear();
}

void RG_RenderManager::_processInput()
{
	static BYTE savedState[256];
	BYTE currState[256];
	GetKeyboardState(currState);

	bool bCtrlDown=GetKeyState(VK_CONTROL)<0;
	bool bShiftDown=GetKeyState(VK_SHIFT)<0;

	for (unsigned int  i=0; i<256; ++i)
	{
		if (currState[i]&0x80)
		{
			if (!(savedState[i]&0x80))
			{
				if (bShiftDown)
				{
					switch(i)
					{
#if 0	//#ifndef _SHIPPING
					case VK_F1:
						{
							// 地表
							bool bActive = SE_SceneManager::GetSingleton().IsTerrainShow();
							SE_SceneManager::GetSingleton().SetTerrainShow(!bActive);
						}
						break;
					case VK_F2:
						{
							// 场景物件
							bool bActive = SE_SceneManager::GetSingleton().IsModelShow();
							SE_SceneManager::GetSingleton().SetModelShow(!bActive);
						}
						break;
					case VK_F3:
						{
							// 水
							bool bActive = SE_SceneManager::GetSingleton().IsWaterShow();
							SE_SceneManager::GetSingleton().SetWaterShow(!bActive);
						}
						break;
					case VK_F4:
						{
							// 草
							bool bActive = m_spGrassRenderClick->GetActive();
							m_spGrassRenderClick->SetActive(!bActive);
						}
						break;
					case VK_F5:
						{
							// 人物
							bool bActive = IM_Manager::GetSingleton().GetActorShow();
							IM_Manager::GetSingleton().EnableActorShow(!bActive);
						}
						break;
					case VK_F6:
						{
							// UE
							bool bActive = UT_CutSceneManager::GetSingleton().ShowUE();
							UT_CutSceneManager::GetSingleton().SetShowUE(!bActive);
						}
						break;
#endif
					case VK_F9:
						// works if batch renderer created
						FX::FX_GlobalParams::ToggleBatchRender();
						break;
					}
				}

				if (bCtrlDown)
				{
					switch(i)
					{
#if 0	//#ifndef _SHIPPING
					case VK_F1:
						{
							// 反射
							UT_GlobalSettings& rkGlobalSettings = g_settings;
							rkGlobalSettings.LOD_ENABLE_REFLECT = !rkGlobalSettings.LOD_ENABLE_REFLECT;
						}
						break;
					case VK_F2:
						{
							// Avatar
							bool bActive = m_spAvatarRenderStep->GetActive();
							m_spAvatarRenderStep->SetActive(!bActive);
						}
						break;
					case VK_F3:
						{
							// 阴影
							bool bActive = m_spShadowRenderStep->GetActive();
							m_spShadowRenderStep->SetActive(!bActive);
						}
						break;
					case VK_F4:
						{
							// 主场景
							bool bActive = m_spMainSceneRenderStep->GetActive();
							m_spMainSceneRenderStep->SetActive(!bActive);
						}
						break;
					case VK_F5:
						{
							// 特效
							bool bActive = m_spNormalFxRenderStep->GetActive();
							m_spNormalFxRenderStep->SetActive(!bActive);
						}
						break;
#endif

					case VK_F8:
						{
							// UI
							bool bActive = m_spUIRenderStep->GetActive();
							m_spUIRenderStep->SetActive(!bActive);
						}
						break;
					case VK_F9:
						{
							// 控制台
							SetScreenConsoleVisible(!m_bScreenStatVisibility );
						}
						break;
					}
				}
			}
		}
	}
	memcpy(savedState, currState, 256);
}

void RG_RenderManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
    if ( m_spNiRenderer == NULL /*|| m_bIsDeviceLost*/ || !m_bIsActiveRender )
    {
        return;
    }

    float fCurrentTime = rkMessage.m_fParam1;

	if ( m_bIsNeedRecreateRender )
	{
		//float fDeltaTime = rkMessage.m_fParam2;
		m_fRecreateRenderTickTime -= rkMessage.m_fParam2;
		if ( m_fRecreateRenderTickTime < 0.0f )
		{
			Recreate();

			m_bIsNeedRecreateRender = false;
			m_fRecreateRenderTickTime = RENDER_RECREATE_MAX_TIME;
		}
		else
		{
			return;
		}
	}

	if ( m_spMainSceneRenderStep->GetActive() )
	{
        if (SE_SceneManager::GetSingletonPtr() != NULL)
        {
            SE_SceneManager::GetSingleton().MarkVisibleArrayDirty();

            SE_SceneManager::GetSingleton().PreCullModels();
        }
        if (IM_Manager::GetSingletonPtr() != NULL)
        {
            IM_Manager::GetSingleton().PreCullModels();
        }

		RenderManager::Instance()->Update(fCurrentTime);
	}

	{
	#ifdef PROC_INPUT
			_processInput();
	#endif
	}

    _renderReflectedScene(fCurrentTime);
	_updateFadeInOut(fCurrentTime);

    _renderFrame();

    if (m_bSnapshot)
    {
        m_bSnapshot = false;

        NiDX92DBufferData* pkDefaultBBRendererData = NiStaticCast(NiDX92DBufferData, 
            m_spNiRenderer->GetDefaultBackBuffer()->GetRendererData());
        if (pkDefaultBBRendererData != NULL)
        {
            TCHAR szSnapshotFilePathName[FILENAME_MAX] = _T("");
            if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szSnapshotFilePathName)))
            {
                _tcscat_s(szSnapshotFilePathName, _T("\\RGame\\"));
            }
            _tcscat_s(szSnapshotFilePathName, _T("Screenshot"));

            SHCreateDirectoryEx(NULL, szSnapshotFilePathName, NULL);

            // 以当前日期时间做文件名
            _tcscat_s(szSnapshotFilePathName, "\\RG");
            TCHAR szSnapshotFileName[FILENAME_MAX] = _T("");
            GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, _T("yyyy'-'MM'-'dd"), szSnapshotFileName, FILENAME_MAX);
            _tcscat_s(szSnapshotFilePathName, szSnapshotFileName);
            _tcscat_s(szSnapshotFilePathName, _T("_"));
            GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, _T("HH'-'mm'-'ss"), szSnapshotFileName, FILENAME_MAX);
            _tcscat_s(szSnapshotFilePathName, szSnapshotFileName);
            _tcscat_s(szSnapshotFilePathName, _T(".png"));

            D3DXSaveSurfaceToFile(szSnapshotFilePathName, D3DXIFF_PNG, pkDefaultBBRendererData->GetSurface(), NULL, NULL);
        }

        if (!m_bFullSnapshot)
        {
            m_bFullSnapshot = true;
            UI_UIManager::GetSingleton().SetCurrentScreenVisibility(true);
        }
    }
	else if (m_bTiledSnapshot)
	{
		m_bTiledSnapshot = false;

		m_spSceneRenderView->SetUseCustomFrustum(true);
		const NiFrustum& kFrustum = m_spSceneRenderView->GetViewFrustum();

		unsigned int uiNumFrustums = m_bTiledSnapshotMultiplier * m_bTiledSnapshotMultiplier;
		std::vector<NiFrustum> kFrustums;
		for (unsigned int i = 0; i < uiNumFrustums; i++)
		{
			kFrustums.push_back(kFrustum);
		}

		for (unsigned int j = 0; j < m_bTiledSnapshotMultiplier; j++)
		{
			for (unsigned int i = 0; i < m_bTiledSnapshotMultiplier; i++)
			{
				unsigned int uiIndex = j * m_bTiledSnapshotMultiplier + i;

				kFrustums[uiIndex].m_fLeft *= 3.0f;
				kFrustums[uiIndex].m_fRight *= 3.0f;
				kFrustums[uiIndex].m_fTop *= 3.0f;
				kFrustums[uiIndex].m_fBottom *= 3.0f;

				kFrustums[uiIndex].m_fLeft = kFrustum.m_fLeft + (kFrustum.m_fRight -
					kFrustum.m_fLeft) / m_bTiledSnapshotMultiplier * i;
				kFrustums[uiIndex].m_fRight = kFrustum.m_fLeft + (kFrustum.m_fRight -
					kFrustum.m_fLeft) / m_bTiledSnapshotMultiplier * (i + 1);
				kFrustums[uiIndex].m_fTop = kFrustum.m_fTop + (kFrustum.m_fBottom -
					kFrustum.m_fTop) / m_bTiledSnapshotMultiplier * j;
				kFrustums[uiIndex].m_fBottom = kFrustum.m_fTop + (kFrustum.m_fBottom -
					kFrustum.m_fTop) / m_bTiledSnapshotMultiplier * (j + 1);
				//kFrustums[uiIndex].m_bOrtho = true;
			}
		}

		for (unsigned int i = 0; i < uiNumFrustums; i++)
		{
			NiFrustum kNewFrustum = kFrustums[i];
			// 
			m_spSceneRenderView->SetCustomFrustum(kNewFrustum);

			_renderFrame();
			_displayFrame();

			char str[256];
			sprintf_s(str, 256, "JZ_%02d.png", i);

			NiDX92DBufferData* pkDefaultBBRendererData = NiStaticCast(NiDX92DBufferData, 
				m_spNiRenderer->GetDefaultBackBuffer()->GetRendererData());
			if (pkDefaultBBRendererData != NULL)
			{
				D3DXSaveSurfaceToFile(str, D3DXIFF_PNG, pkDefaultBBRendererData->GetSurface(),
					NULL, NULL);
			}
		}

		m_spSceneRenderView->SetUseCustomFrustum(false);

		if (!m_bFullSnapshot)
		{
			m_bFullSnapshot = true;
			UI_UIManager::GetSingleton().SetCurrentScreenVisibility(true);

			m_spUIRenderStep->SetActive(true);
			m_spUE3DRenderClick->SetActive(true);
			m_spUE2DRenderClick->SetActive(true);
		}
	}

    _displayFrame();

	if ( g_settings.PROFILE_SHOW_MAIN_LOOP_STAT )
	{
		T_PROFILE("Update::Stat");
		unsigned int uiNumObjectsDrawn;
		float fCullTime, fRenderTime;
		m_spRenderFrame->GatherStatistics(uiNumObjectsDrawn, fCullTime, fRenderTime);

		UT_StatisticsManager& rkStatsManager = UT_StatisticsManager::GetSingleton();
		rkStatsManager.SetNumObjectsDrawnPerFrame(uiNumObjectsDrawn);
		rkStatsManager.SetCullTime(fCullTime);
		rkStatsManager.SetRenderTime(fRenderTime);
		rkStatsManager.UpdateMetrics();

		_updateScreenConsole();
	}
}


void RG_RenderManager::_eventWindowMessage(MG_MessageBase& rkMessage)
{
	/*WPARAM& wParam = rkMessage.m_uiParam2;
	LPARAM& lParam = rkMessage.m_uiParam3;*/

	switch ( rkMessage.m_uiParam1 )
	{
		case WM_SIZE:
		{
			if ( g_dllRunInBrowser )
			{
				m_bIsMinimized = false;

				if ( g_settings.WINDOW_WIDTH != m_nWidth ||
					g_settings.WINDOW_HEIGHT != m_nHeight )
				{
					if ( g_settings.WINDOW_WIDTH < RENDER_MINIMUM_WINDOW_WIDTH ||
						g_settings.WINDOW_HEIGHT < RENDER_MINIMUM_WINDOW_HEIGHT )
					{
						/*
							@ remark:
							 1. 在微端和浏览器中最小化窗口时，没有指明特定的消息，所以只能暂时用这个trick来代替最小化窗口消息.
							 2. 使用这个trick要注意，网页端真的支持玩家手动把窗口拉到非常小的尺寸，所以要保证当玩家恢复窗口时，渲染器要正常运行.
							 3. 网页端的效果有待验证.
						*/
						m_bIsMinimized = true;
					}
					else
					{
						m_bIsNeedRecreateRender = true;
						m_fRecreateRenderTickTime = RENDER_RECREATE_MAX_TIME;
					}
				}

				SetRenderActive(!m_bIsMinimized);
			}
			else
			{
				if ( ( rkMessage.m_uiParam2 != SIZE_MINIMIZED ) && 
					!( m_bIsMinimized && ( rkMessage.m_uiParam2 == SIZE_RESTORED || rkMessage.m_uiParam2 == SIZE_MAXIMIZED ) ) )
				{
					/*
						@ 进入这里的逻辑条件：
						1.当这次操作不是最小化窗口
						2.当这次操作不是从最小化恢复窗口
					*/

					Recreate();
				}
				
				m_bIsMinimized = 
					( rkMessage.m_uiParam2 == SIZE_MINIMIZED );

				SetRenderActive(!m_bIsMinimized);
			}
		}   
		break;
		case WM_EXITSIZEMOVE:
		{
			if ( g_dllRunInBrowser )
			{
				// 浏览器和微端不会有WM_EXITSIZEMOVE消息;
			}
			else
			{
				Recreate();
			}
		}
		break;
		case WM_DISPLAYCHANGE:
		{
			Recreate();
		}
		break;
		case WM_INIT_ON_RESOLUTION_CHANGE:
		{
			Recreate();
		}
		break;
	}
}

void RG_RenderManager::_eventWindowDeactive(MG_MessageBase& rkMessage)
{
	if ( g_dllRunInBrowser )
	{
		// 响应微端下的老板键
		SetRenderActive( IsWindowVisible(g_hMainWnd) == TRUE );
	}
}

//void RG_RenderManager::_eventReloadSettings(MG_MessageBase& rkMessage)
//{
//    g_settings.Load(true);
//	if ( m_pkRadialBlurEffect )
//	{
//		m_pkRadialBlurEffect->ReloadSettings();
//	}
//}

void RG_RenderManager::_renderFrame(void)
{
	T_PROFILE("Render::All");

    m_spRenderFrame->Draw();
}

void RG_RenderManager::_displayFrame(void)
{
    T_PROFILE_EX("Render::Display", 30.0f);

    UT_StatisticsManager& rkStatsManager = UT_StatisticsManager::GetSingleton();
    rkStatsManager.StartSwapTimer();

    m_spRenderFrame->Display();

    rkStatsManager.StopSwapTimer();
}

void RG_RenderManager::_updateRenderStepsActivities(unsigned int maskRenderStepActivity)
{
    if (m_spShadowRenderStep != NULL)
    {
        m_spShadowRenderStep->SetActive(IS_RENDER_STEP_ACTIVE(maskRenderStepActivity, RSA_SHADOW));
    }

    m_spMainSceneRenderStep->SetActive(IS_RENDER_STEP_ACTIVE(maskRenderStepActivity, RSA_MAIN_SCENE));

    bool bFxRenderActive = IS_RENDER_STEP_ACTIVE(maskRenderStepActivity, RSA_NORMAL_FX);
    m_spNormalFxRenderStep->SetActive(bFxRenderActive);

    if (m_spWeatherRenderStep != NULL)
    {
        m_spWeatherRenderStep->SetActive(IS_RENDER_STEP_ACTIVE(maskRenderStepActivity, RSA_WEATHER));
    }

    if (m_spPostProcessRenderStep != NULL)
    {
		bool bPostProcEnable = IS_RENDER_STEP_ACTIVE(maskRenderStepActivity, RSA_POST_PROC);
        m_spPostProcessRenderStep->SetActive(bPostProcEnable);
		m_pkSpaceWarpEffect->SetActive(bFxRenderActive && g_settings.CFG_ENABLE_SPACEWARP && m_pkSpaceWarpEffect->CanActiveEffect());

		/// 暂时毙掉OUTLINE的渲染
		//m_pkOutlineEffect->SetActive(false && g_settings.CFG_ENABLE_POST_PROCESS && m_pkOutlineEffect->CanActiveEffect());
		if (m_pkBloomEffect)
			m_pkBloomEffect->SetActive(g_settings.CFG_ENABLE_POST_PROCESS && m_pkBloomEffect->CanActiveEffect());
		if (m_pkGreyEffect)
			m_pkGreyEffect->SetActive(m_bIsGreyEnable && g_settings.CFG_ENABLE_POST_PROCESS && m_pkGreyEffect->CanActiveEffect());
	    _updateClicksOutputToPostProcessing();	// 这里会重新active m_spPostProcessRenderStep
    }

    m_spUIRenderStep->SetActive(IS_RENDER_STEP_ACTIVE(maskRenderStepActivity, RSA_UI));
    m_spUIRenderClick->SetClearAllBuffers(RSA_UI == maskRenderStepActivity);
}

void RG_RenderManager::_updateScreenConsole(void)
{
	UT_StatisticsManager* pkStatsMgr = UT_StatisticsManager::GetSingletonPtr();	
	if (pkStatsMgr == NULL)
	{
        return;
    }

	unsigned int nNumDrawn = 0;
	float fRenderTime = 0;
	float fCullTime = 0;

	//////////////////////////////////////////////////////////////////////////
	if (m_spAvatarRenderStep)
	{
		nNumDrawn   = m_spAvatarRenderStep->GetNumObjectsDrawn();
		fRenderTime = m_spAvatarRenderStep->GetRenderTime();
		fCullTime   = m_spAvatarRenderStep->GetCullTime();

		if (m_spAvatarBloomEffect)
		{
			nNumDrawn   += m_spAvatarBloomEffect->GetNumObjectsDrawn();
			fRenderTime += m_spAvatarBloomEffect->GetRenderTime();
			fCullTime   += m_spAvatarBloomEffect->GetCullTime();
		}
		if (m_spAvatarOuterGlow)
		{
			nNumDrawn   += m_spAvatarOuterGlow->GetNumObjectsDrawn();
			fRenderTime += m_spAvatarOuterGlow->GetRenderTime();
			fCullTime   += m_spAvatarOuterGlow->GetCullTime();
		}
		pkStatsMgr->SetAvatarRenderStat(
			nNumDrawn, 
			fRenderTime * 1000.0f, 
			fCullTime * 1000.0f);
	}

	//////////////////////////////////////////////////////////////////////////
	if (m_spShadowRenderStep)
	{
		pkStatsMgr->SetShadowRenderStat(
			m_spShadowRenderStep->GetNumObjectsDrawn(), 
			m_spShadowRenderStep->GetRenderTime() * 1000.0f, 
			m_spShadowRenderStep->GetCullTime() * 1000.0f);
	}
	if ( m_spScreenRenderStep )
	{
		pkStatsMgr->SetStatTextDp( 
			m_spScreenRenderStep->GetNumObjectsDrawn() , 
			m_spScreenRenderStep->GetRenderTime()
			);
	}

	size_t nStat[S_STAT_MAX];
	m_spSceneRenderView->GetSceneRenderStat( nStat );
	pkStatsMgr->SetSceneRenderStat( 
		m_spNormalSceneRenderClick->GetNumObjectsDrawn() , nStat, 
		m_spNormalSceneRenderClick->GetRenderTime() * 1000.0f, 
        m_spNormalSceneRenderClick->GetCullTime() * 1000.0f);

	//////////////////////////////////////////////////////////////////////////
	if (m_spGrassRenderClick)
	{
		pkStatsMgr->SetGrassRenderStat(
			m_spGrassRenderClick->GetNumObjectsDrawn(), 
			m_spGrassRenderClick->GetRenderTime() * 1000.0f, 
			m_spGrassRenderClick->GetCullTime() * 1000.0f);
	}
	if (m_spUE3DRenderClick)
	{
		pkStatsMgr->SetUIEffectsRenderStat(
			m_spUE3DRenderClick->GetNumObjectsDrawn(), 
			m_spUE3DRenderClick->GetRenderTime() * 1000.0f, 
			m_spUE3DRenderClick->GetCullTime() * 1000.0f);
	}
	SoftParticlesManagerPtr spSoftParticle = RenderManager::Instance()->softParticlesManager_;
	if (spSoftParticle)
	{
		pkStatsMgr->SetSoftParticleRenderStat(
			spSoftParticle->GetNumObjectsDrawn(), 
			spSoftParticle->GetRenderTime() * 1000.0f, 
			spSoftParticle->GetCullTime() * 1000.0f);
	}

	pkStatsMgr->SetReflectRenderStat(
        m_uiReflectObjects, 
        m_fReflectRenderTime * 1000.0f, 
        m_fReflectCullTime * 1000.0f);

	pkStatsMgr->SetNormalFxRenderStat(
		m_spNormalFxRenderStep->GetNumObjectsDrawn(), 
		m_spNormalFxRenderStep->GetRenderTime() * 1000.0f, 
		m_spNormalFxRenderStep->GetCullTime() * 1000.0f);

	pkStatsMgr->SetAvatarFxRenderStat(
		m_spAvatarRenderStep->GetNumObjectsDrawn(), 
		m_spAvatarRenderStep->GetRenderTime() * 1000.0f, 
		m_spAvatarRenderStep->GetCullTime() * 1000.0f);

	if (m_spWeatherRenderStep)
	{
		pkStatsMgr->SetWeatherRenderStat( 
			m_spWeatherRenderStep->GetNumObjectsDrawn(),
			m_spWeatherRenderStep->GetRenderTime() * 1000.0f
			);
	}

	//////////////////////////////////////////////////////////////////////////
	if ( m_spPostProcessRenderStep )
	{
		RG_PostProcessEffect* pkEffect = m_spPostProcessRenderStep->GetEffectAt( RG_PostProcessEffect::SPACE_WARP );
		if ( pkEffect )
		{
			nNumDrawn = 0, fRenderTime = 0, fCullTime = 0;
			if (pkEffect->GetActive())
			{
				nNumDrawn   = pkEffect->GetNumObjectsDrawn();
				fRenderTime = pkEffect->GetRenderTime();
				fCullTime   = pkEffect->GetCullTime();
			}

			pkStatsMgr->SetFxSpacewarpRenderStat(
				nNumDrawn, 
				fRenderTime * 1000.0f , 
				fCullTime * 1000.0f );
		}

		pkEffect = m_spPostProcessRenderStep->GetEffectAt( RG_PostProcessEffect::BLOOM );
		if ( pkEffect )
		{
			nNumDrawn = 0, fRenderTime = 0, fCullTime = 0;
			if (pkEffect->GetActive())
			{
				nNumDrawn   = pkEffect->GetNumObjectsDrawn();
				fRenderTime = pkEffect->GetRenderTime();
				fCullTime   = pkEffect->GetCullTime();
			}

			pkStatsMgr->SetFxPostPRenderStat(
				nNumDrawn, 
				fRenderTime * 1000.0f , 
				fCullTime * 1000.0f );
		}
	}

	size_t nUIMem = 0;
	if( UI_UIManager::GetSingletonPtr() )
	{
		nUIMem = UI_UIManager::GetSingleton().GetAllImgAllocationSize();
		pkStatsMgr->SetUIMemoryInKB( nUIMem >> 10 );
	}

	pkStatsMgr->SetUIRenderStat(
        m_spUIRenderStep->GetNumObjectsDrawn(), 
        m_spUIRenderStep->GetRenderTime() * 1000.0f, 
        m_spUIRenderStep->GetCullTime() * 1000.0f);

	pkStatsMgr->SetUIRenderDetail(
		m_spUE2DRenderClick->GetNumObjectsDrawn(),
		m_spScreenFxRenderClick->GetNumObjectsDrawn(),
		m_spUIRenderClick->GetNumObjectsDrawn(),
		m_spUIFxRenderClick->GetNumObjectsDrawn()
		);

	//////////////////////////////////////////////////////////////////////////
	if (m_spScreenConsole != NULL && m_bScreenStatVisibility)
	{
		m_uiUpdateClicks += 2;
		unsigned int uiMod = (unsigned int)pkStatsMgr->GetFrameRate() * 2;
		if ( uiMod == 0 )
		{
			uiMod = 100;
		}
		if ( m_uiUpdateClicks % uiMod != 0  )
		{
			return;
		}
		pkStatsMgr->UpdateStats( m_spScreenConsole , 90 );
		m_spScreenConsole->RecreateText();
	}
	else
	{
		m_spScreenConsole->SetCamera(NULL);
	}
}

#pragma TODO("考虑把水面反射放到一个单独的click中管理")

void RG_RenderManager::_createWaterReflect()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	if(m_spWaterReflectRenderedTexture)
	{
		pkRenderer->PurgeTexture(m_spWaterReflectRenderedTexture);
		m_spWaterReflectRenderedTexture = NULL;
	}

	NiTexture::FormatPrefs kFormatPrefs;
	kFormatPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	kFormatPrefs.m_eMipMapped   = NiTexture::FormatPrefs::NO;
	kFormatPrefs.m_eAlphaFmt    = NiTexture::FormatPrefs::SMOOTH;

	// 反射
	m_spWaterReflectRenderedTexture = NiRenderedTexture::Create(
		g_settings.LOD_REFLECT_MAP_SIZE, g_settings.LOD_REFLECT_MAP_SIZE, 
		m_spNiRenderer, 
		kFormatPrefs);
	if( m_spWaterReflectRenderedTexture == NULL )
	{
		GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
			"无法创建反射纹理%d" , g_settings.LOD_REFLECT_MAP_SIZE );
		//这里应该只是关闭反射渲染
		g_settings.LOD_ENABLE_REFLECT = false;
		return;
	}
	m_spWaterReflectRenderedTexture->SetName( "reflect" );

	if(m_spWaterReflectRenderTargetGroup)
	{
		m_spWaterReflectRenderTargetGroup = NULL;
	}

	m_spWaterReflectRenderTargetGroup = NiRenderTargetGroup::Create(
		m_spWaterReflectRenderedTexture->GetBuffer(), m_spNiRenderer, true, true);

	// 折射
	//if(m_spWaterRefractRenderedTexture)
	//{
	//	pkRenderer->PurgeTexture(m_spWaterRefractRenderedTexture);
	//	m_spWaterRefractRenderedTexture = NULL;
	//}
	//m_spWaterRefractRenderedTexture = NiRenderedTexture::Create(
	//	g_settings.LOD_REFLECT_MAP_SIZE, g_settings.LOD_REFLECT_MAP_SIZE, 
	//	m_spNiRenderer, 
	//	kFormatPrefs);

	//if(m_spWaterRefractRenderTargetGroup)
	//{
	//	m_spWaterRefractRenderTargetGroup = NULL;
	//}

	//m_spWaterRefractRenderTargetGroup = NiRenderTargetGroup::Create(
	//	m_spWaterRefractRenderedTexture->GetBuffer(), m_spNiRenderer, true, true);
}

float RG_RenderManager::_getCurrentMapWaterHeight(NiPoint3& actPos)
{
	SE_SceneWorldMap* m_pkSceneWorldMap = SE_SceneManager::GetSingleton().GetWorldMap();

	if(m_pkSceneWorldMap)
		return m_pkSceneWorldMap->GetCurrentWaterHeight(actPos);

	return actPos.z - 500.0f;
}

bool RG_RenderManager::_checkDesktopState(DWORD dwMaxWaitingTime)
{
	// 执行
	DWORD dwRealWaitTime = 0;
	DWORD dwWin32ErrorCode = 0;
	StringType szOtherDeskTopName;

	bool isRecoverd = Utility::WaitingInteractDesktopRecovered( 
		dwMaxWaitingTime, dwRealWaitTime, dwWin32ErrorCode, szOtherDeskTopName );

	// Log
	if ( isRecoverd )
	{
		GfxWriteLog( LOG_TAG_RENDER, LOG_SWITCH_RENDER, 
			"CheckDesktopState --- Success: "
			"RealWaitTime = [%d], Win32ErrorCode = [%d], CurrDeskTop = [%s].", 
			dwRealWaitTime, dwWin32ErrorCode, szOtherDeskTopName.c_str() );
	}
	else
	{
		GfxWriteLog( LOG_TAG_RENDER, LOG_SWITCH_RENDER, 
			"CheckDesktopState --- Failed!: . "
			"RealWaitTime = [%d], Win32ErrorCode = [%d], CurrDeskTop = [%s].", 
			dwRealWaitTime, dwWin32ErrorCode, szOtherDeskTopName.c_str() );
	}

	// 上传资料
#ifdef _SHIPPING

	static bool isUploadLog = true;

	if ( isUploadLog && dwRealWaitTime > dwMaxWaitingTime )
	{
		isUploadLog = false;

		jzUploadFtpFile("InvalidDesktopState_V1_1", "log\\Client_Render.log", true, false);
	}

#endif

	return isRecoverd;
};

void RG_RenderManager::_renderReflectedScene(float fCurrentTime, bool bForce/* = false*/)
{
	// 目前水面反射贴图的渲染流程也需要改进一下
	// 就算没有任何物件需要渲染，也会白白的切入一次renderTarget,造成性能损失！

    T_PROFILE_EX("Render::Reflect", 10.0f);

	m_uiReflectObjects = 0;

	if(!m_spMainSceneRenderStep->GetActive())//不激活不渲染
		return;
    UT_GlobalSettings& rkGlobalSettings = g_settings;

	if (!m_spWaterReflectRenderTargetGroup)
	{
		return;
	}
	//在渲染登陆场景的时候，由于天空盒位置很高，会造成无法反射，从而对水的效果造成影响
	if ( !rkGlobalSettings.LOD_ENABLE_REFLECT )
	{
		return;
	}
	if ( fCurrentTime < (m_fLastReflectUpdate + rkGlobalSettings.LOD_REFLECT_UPDATE_FREQ)  )
	{
		//return;
	}
	m_fLastReflectUpdate = fCurrentTime;
	
	CA_CameraManager* pkCameraManager = CA_CameraManager::GetSingletonPtr();
	NiPoint3 kPlayerPos = NiPoint3::ZERO;
	pkCameraManager->GetCurrActorPlayerLocation( kPlayerPos );

	float fWaterHeight = _getCurrentMapWaterHeight(kPlayerPos);
	/// keng
	//float fWaterHeight = kPlayerPos.z - 500.0f;

	NiCameraPtr spRefCamera = pkCameraManager->GetNiReflectCamera( fWaterHeight , true );
	NiCameraPtr spNiCamera = pkCameraManager->GetNiCamera();
	float fStartTime = NiGetCurrentTimeInSec();

    m_kReflectCuller.SetFrustumFromCamera(spNiCamera);

	if( !m_spNiRenderer->BeginOffScreenFrame() )
		return;

	m_spNiRenderer->SetBackgroundColor( NiColorA( 0.0f , 0.0f, 0.0f, 0.0f ) );
	m_spNiRenderer->BeginUsingRenderTargetGroup(m_spWaterReflectRenderTargetGroup,NiRenderer::CLEAR_ALL);
	m_kReflectedObjects.RemoveAll();

	SE_SceneManager::GetSingleton().GetWaterVisibleArray(spNiCamera, m_kReflectCuller, 
		m_kReflectedObjects,0 );
	
    m_kReflectCuller.SetFrustumFromCamera(spNiCamera);
	if ( m_kReflectedObjects.GetCount() < 5  )
	{
		_reflectDefaultReturn();
		return;
	}
	/*GfxWriteLog( LOG_TAG_RES , LOG_SWITCH_RES ,
		"Render reflect for %d visible water" , m_kReflectedObjects.GetCount() );*/

    m_kReflectCuller.SetFrustumFromCamera(spRefCamera);

	m_kReflectedObjects.RemoveAll();
	m_kReflectedTerrain.RemoveAll();
	SE_SceneManager::GetSingleton().GetReflectVisibleArray(spRefCamera, m_kReflectCuller, m_kReflectedObjects , false );
	SE_SceneManager::GetSingleton().GetReflectVisibleArray(spRefCamera, m_kReflectCuller, m_kReflectedTerrain , true );
	//if ( m_kReflectedObjects.GetCount() < 50 )
	//{
	//	//地形的倒影没什么意思，如果物件太多就不渲染了
	//	SE_SceneManager::GetSingleton().GetReflectVisibleArray(spRefCamera,m_kReflectedTerrain , true );
	//}


	if ( m_kReflectedObjects.GetCount() == 0 && m_kReflectedTerrain.GetCount() == 0 )
	{
		_reflectDefaultReturn();
		return;
	}
	m_fReflectCullTime = NiGetCurrentTimeInSec() - fStartTime;
	fStartTime = NiGetCurrentTimeInSec();

	const NiRect<float>* pkViewportToUse = NULL;
	pkViewportToUse = &spRefCamera->GetViewPort();

	m_spNiRenderer->SetCameraData(spRefCamera->GetWorldLocation(),
		spRefCamera->GetWorldDirection(), spRefCamera->GetWorldUpVector(),
		spRefCamera->GetWorldRightVector(), spRefCamera->GetViewFrustum(),
		*pkViewportToUse);
	if ( rkGlobalSettings.LOD_ENABLE_REFLECT_CLIP_PLANE )
	{
		//由于我们的场景物件全部使用shader渲染，所以要首先计算ClipSpace的裁剪平面
		NiPlane waterPlane( NiPoint3::UNIT_Z , fWaterHeight );
		NiPlane clipPlane = _transformClippingPlane( waterPlane , spRefCamera->GetWorldLocation(),
			spRefCamera->GetWorldDirection(), spRefCamera->GetWorldUpVector(),
			spRefCamera->GetWorldRightVector(), spRefCamera->GetViewFrustum() );
		//使用UserClipPlane裁剪掉水面下的部分
		_setClippingPlane( 0 , clipPlane );
	}
	if ( rkGlobalSettings.LOD_ENABLE_SIMPLE_REFLECT )
	{
		//在渲染反射场景的时候切换到简单的Shader
		D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0 , 200 , 0 , 255 ) , L"Render::Reflect" );
		unsigned int uiQuantity = m_kReflectedObjects.GetCount();
		unsigned int i;
		for (i = 0; i < uiQuantity; i++)
		{
			if ( i > rkGlobalSettings.LOD_MAX_REFLECT_MODEL )
			{
				break;
			}
			NiGeometry& kGeometry = m_kReflectedObjects.GetAt(i);
	
			const NiMaterial* pkPreviousMaterial = kGeometry.GetActiveMaterial();
	
			// Apply and set active material to new material.
            kGeometry.ApplyAndSetActiveMaterial(
                (kGeometry.GetSkinInstance() != NULL) ? m_pkReflectDynamicShader : m_pkReflectStaticShader);
			// Render object.
			kGeometry.RenderImmediate(m_spNiRenderer);
			// Restore previous active material.
			kGeometry.SetActiveMaterial(pkPreviousMaterial);
		}
		m_uiReflectObjects += i;
		uiQuantity = m_kReflectedTerrain.GetCount();
		for (i = 0; i < uiQuantity; i++)
		{
			if ( i + m_uiReflectObjects > rkGlobalSettings.LOD_MAX_REFLECT_MODEL )
			{
				break;
			}
			NiGeometry& kTerrain = m_kReflectedTerrain.GetAt(i);
			// Backup current active material.
			const NiMaterial* pkPreviousTerrainMat = kTerrain.GetActiveMaterial();
			// Apply and set active material to new material.
			kTerrain.ApplyAndSetActiveMaterial( m_pkReflectTerrainShader );
			// Render object.
			kTerrain.RenderImmediate( m_spNiRenderer );
			kTerrain.SetActiveMaterial( pkPreviousTerrainMat );
		}
		D3DPERF_EndEvent();
		m_uiReflectObjects += i;
	}
	else
	{
		unsigned int uiCount = NiDrawVisibleArray( spRefCamera , m_kReflectedObjects );
		uiCount+= NiDrawVisibleArray( spRefCamera , m_kReflectedTerrain );
		m_uiReflectObjects = uiCount;
	}
	if ( rkGlobalSettings.LOD_ENABLE_REFLECT_CLIP_PLANE )
	{
		_clearClippingPlane( 0 );
	}
	m_spNiRenderer->EndUsingRenderTargetGroup();
	m_spNiRenderer->EndOffScreenFrame();
	m_fReflectRenderTime = NiGetCurrentTimeInSec() - fStartTime;
#if 0
	if ( rkGlobalSettings.LOD_ENABLE_REFRACT )
	{

		m_spNiRenderer->BeginOffScreenFrame();
		m_spNiRenderer->SetBackgroundColor( NiColorA( 0 , 0 , 0 , 0 ) );
		m_spNiRenderer->BeginUsingRenderTargetGroup(m_spWaterRefractRenderTargetGroup,NiRenderer::CLEAR_ALL);

		m_kReflectedObjects.RemoveAll();
		SE_SceneManager::GetSingleton().GetTerrainVisibleArrary(spNiCamera,m_kReflectedObjects);
		SE_SceneManager::GetSingleton().GetModelVisibleArray(spNiCamera,m_kReflectedObjects);

		m_spNiRenderer->SetCameraData(spNiCamera->GetWorldLocation(),
			spNiCamera->GetWorldDirection(), spNiCamera->GetWorldUpVector(),
			spNiCamera->GetWorldRightVector(), spNiCamera->GetViewFrustum(),
			*pkViewportToUse);

		//由于我们的场景物件全部使用shader渲染，所以要首先计算ClipSpace的裁剪平面
		//和渲染反射的时候相反，我们要裁掉水面上的部分
		NiPlane waterPlane( -NiPoint3::UNIT_Z , -fWaterHeight );
		NiPlane clipPlane = _transformClippingPlane( waterPlane , spNiCamera->GetWorldLocation(),
			spNiCamera->GetWorldDirection(), spNiCamera->GetWorldUpVector(),
			spNiCamera->GetWorldRightVector(), spNiCamera->GetViewFrustum() );
		//使用UserClipPlane裁剪掉水面上的部分
		_setClippingPlane( 0 , clipPlane );
		unsigned int uiCount = NiDrawVisibleArray( spNiCamera , m_kReflectedObjects );
		_clearClippingPlane( 0 );

		m_spNiRenderer->EndUsingRenderTargetGroup();
		m_spNiRenderer->EndOffScreenFrame();
	}
#endif
}

void RG_RenderManager::_reflectDefaultReturn()
{
	m_fReflectCullTime = 0;
	m_fReflectRenderTime = 0;
	m_uiReflectObjects = 0;
	m_spNiRenderer->EndUsingRenderTargetGroup();
	m_spNiRenderer->EndOffScreenFrame();
}

void RG_RenderManager::_setClippingPlane( int index, const NiPlane& plane )
{
	LPDIRECT3DDEVICE9 pkDx9Device = m_spNiRenderer->GetD3DDevice();
	float d3dPlane[4];
	d3dPlane[0] = plane.GetNormal().x;
	d3dPlane[1] = plane.GetNormal().y;
	d3dPlane[2] = plane.GetNormal().z;
	d3dPlane[3] = -plane.GetConstant();

	HRESULT result = pkDx9Device->SetClipPlane(index, d3dPlane);

	if (FAILED(result))
	{
		//GfxWriteLog( LOG_SYSTEM_FILELINE , LOG_SWITCH_SYSTEM , "SetClipPlane failed");
		return;
	}

	m_uiClipPlaneState = m_uiClipPlaneState | (1 << index);
	if ( m_spNiRenderer && m_spNiRenderer->GetRenderState() )
		m_spNiRenderer->GetRenderState()->SetRenderState(D3DRS_CLIPPLANEENABLE, m_uiClipPlaneState);
}

NiPlane RG_RenderManager::_transformClippingPlane(const NiPlane& rkClipPlane, 
                                                  const NiPoint3& kLoc,
                                                  const NiPoint3& kDir, 
                                                  const NiPoint3& kUp, 
                                                  const NiPoint3& kRight,
												  const NiFrustum& kFrustum)
{
	D3DXMATRIX view;
	// View matrix update
	view._11 = kRight.x;    
	view._12 = kUp.x;    
	view._13 = kDir.x;
	view._14 = 0.0f;
	view._21 = kRight.y; 
	view._22 = kUp.y;    
	view._23 = kDir.y;
	view._24 = 0.0f;
	view._31 = kRight.z; 
	view._32 = kUp.z;    
	view._33 = kDir.z;
	view._34 = 0.0f;
	view._41 = -(kRight * kLoc);
	view._42 = -(kUp * kLoc);
	view._43 = -(kDir * kLoc);
	view._44 = 1.0f;

	float fDepthRange = kFrustum.m_fFar - kFrustum.m_fNear;

	// Projection matrix update
	float fRmL = kFrustum.m_fRight - kFrustum.m_fLeft;
	float fRpL = kFrustum.m_fRight + kFrustum.m_fLeft;
	float fTmB = kFrustum.m_fTop - kFrustum.m_fBottom;
	float fTpB = kFrustum.m_fTop + kFrustum.m_fBottom;
	float fInvFmN = 1.0f / fDepthRange;

    D3DXMATRIX proj;
	proj._11 = 2.0f / fRmL;
	proj._21 = 0.0f;
	proj._31 = -fRpL / fRmL;
	proj._41 = 0.0f;

	proj._12 = 0.0f;
	proj._22 = 2.0f / fTmB;
	proj._32 = -fTpB / fTmB;
	proj._42 = 0.0f;
	proj._13 = 0.0f;
	proj._23 = 0.0f;
	proj._33 = kFrustum.m_fFar * fInvFmN;
	proj._43 = -(kFrustum.m_fNear * kFrustum.m_fFar * fInvFmN);

	// A "w-friendly" projection matrix to make fog, w-buffering work
	proj._14 = 0.0f;
	proj._24 = 0.0f;
	proj._34 = 1.0f;
	proj._44 = 0.0f;

    D3DXMATRIX viewProj = view * proj;

	NiPlane kPlane( NiPlane( NiPoint3::UNIT_Z , 0.1f ) );

	D3DXPLANE   planeNew;
	D3DXPLANE   plane(
        rkClipPlane.GetNormal().x, rkClipPlane.GetNormal().y, rkClipPlane.GetNormal().z, -rkClipPlane.GetConstant());
	
	D3DXMatrixInverse(&viewProj, NULL, &viewProj);
	D3DXMatrixTranspose(&viewProj, &viewProj);

	D3DXPlaneTransform( &planeNew , &plane , &viewProj );
	D3DXPlaneNormalize(&planeNew, &planeNew);

	kPlane.SetNormal( NiPoint3( planeNew.a , planeNew.b , planeNew.c  ) );
	kPlane.SetConstant( - planeNew.d );

	return kPlane;
}

void RG_RenderManager::_clearClippingPlane(int index)
{
    m_uiClipPlaneState = m_uiClipPlaneState & ~(1 << index);

	if ( m_spNiRenderer && m_spNiRenderer->GetRenderState() )
		m_spNiRenderer->GetRenderState()->SetRenderState(D3DRS_CLIPPLANEENABLE, m_uiClipPlaneState);
}

void RG_RenderManager::SetSceneBackgroundColor( const NiColor& kClr )
{
	if ( m_spNormalSceneRenderClick )
	{
		m_spNormalSceneRenderClick->SetBackgroundColor( NiColorA( kClr.r , kClr.g , kClr.b , 1.0f ) );
	}
}


// 从灰到亮
void RG_RenderManager::GreySceneFadeIn( float fDeltaAlphaPerSecond, float fMaxAlpha  )
{
	SetGreyScenePostProcActivity(true);
	m_fDeltaAlphaPerSecond = fDeltaAlphaPerSecond;
	m_fFadeInMaxGrey = fMaxAlpha;

	m_fFadeStartGrey = 0.0f;
	m_SceneGreyState = SGFIOS_FADEIN;
}


void RG_RenderManager::GreySceneFadeOut( float fDeltaAlphaPerSecond, float fMinAlpha  )
{
	m_fDeltaAlphaPerSecond = fDeltaAlphaPerSecond;
	m_fFadeInMinGrey = fMinAlpha;

	m_fFadeStartGrey = 1.0f;

	m_SceneGreyState = SGFIOS_FADEOUT;
}

/// 0 灰， 1 亮
void RG_RenderManager::_updateFadeInOut(float fTime)
{
	if (m_SceneGreyState != SGFIOS_INACTIVE)
	{
		float fDeltaTime = fTime - m_fFadeStartTime;
		if (m_SceneGreyState == SGFIOS_FADEOUT)
		{
			if (m_fCurrentGrey >= m_fFadeInMinGrey)
			{
				m_fCurrentGrey = m_fFadeStartGrey - fDeltaTime * m_fDeltaAlphaPerSecond;
			}
			else
			{
				m_fCurrentGrey = m_fFadeInMinGrey;
				m_SceneGreyState = SGFIOS_INACTIVE;
			}
		}
		else if(m_SceneGreyState == SGFIOS_FADEIN)
		{
			if (m_fCurrentGrey <= m_fFadeInMaxGrey)
			{
				m_fCurrentGrey = m_fFadeStartGrey + fDeltaTime * m_fDeltaAlphaPerSecond;
			}
			else
			{
				m_fCurrentGrey = m_fFadeInMaxGrey;
				m_SceneGreyState = SGFIOS_INACTIVE;
				/// 亮起来后，自动设置
				if (m_pkGreyEffect != NULL && m_pkGreyEffect->CanActiveEffect())
				{
					m_pkGreyEffect->SetActive(false);
					_updateClicksOutputToPostProcessing();
				}
			}
		}

		NiShaderFactory::UpdateGlobalShaderConstant("g_fGreyFactor",   sizeof(float),    &m_fCurrentGrey);
	}
	else
	{
		m_fFadeStartTime = fTime;
	}
}

void RG_RenderManager::EnableUIRenderStep(bool bEnable)
{
	m_spUE2DRenderClick->SetActive(bEnable);
	//m_spScreenFxRenderClick->SetActive(bEnable);
	m_spUIRenderClick->SetActive(bEnable);
	m_spUIFxRenderClick->SetActive(bEnable);
}