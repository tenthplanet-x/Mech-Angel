#ifndef _RG_RENDERMANAGER_H
#define _RG_RENDERMANAGER_H

#include "RG_AvatarRenderClick.h"
#include "RG_AvatarBloomEffect.h"
#include "RG_AvatarOuterGlow.h"
#include "RG_GrassRenderClick.h"
#include "RG_SceneRenderView.h"
#include "RG_PostProcessing.h"
#include "RG_MiniMapRenderStep.h"
#include "RG_UERenderClick.h"
#include "RG_UiEffectRenderView.h"
#include "UIRenderClick.h"
#include "NiCursorRenderClick.h"
#include "MG_MessageHandlerInterface.h"

namespace Render
{
    class RG_Dx9MsaaRenderedTex;
    class RG_SpaceWarpEffect;
	class RG_OutlineEffect;
    class RG_BloomEffect;
    class RG_RadialBlurEffect;
	class RG_SSAOEffect;
	class RG_GreyEffect;
}

NiSmartPointer(SoftParticlesManager);

class RG_RenderManager : public Memory::MM_BaseObject,
						 public MG_MessageHandlerInterface,
						 public Utility::UT_Singleton<RG_RenderManager>
{
public:
    enum RenderMode
    {
        RM_LOGIN_SCENE,         // 登陆场景
        RM_CHAR_LIST_SCENE,     // 人物列表场景
        RM_CREATE_CHAR_SCENE,   // 创建人物场景
        RM_LOADING_SCENE,       // 进度条场景
        RM_GAME,                // 游戏
		RM_ARENA,				// 竞技场

        RM_MAX_COUNT
    };
private:
    enum RenderStepActivity
    {
        RSA_NONE        = 0,

        RSA_LOGIN_SCENE = 0x0001,
        RSA_SHADOW      = 0x0004,
        RSA_MAIN_SCENE  = 0x0008,
        RSA_NORMAL_FX   = 0x0010,
        RSA_WEATHER     = 0x0020,
        RSA_POST_PROC   = 0x0040,
        RSA_UI          = 0x0080,
        RSA_STATS       = 0x0100,

        // 均不包括 RSA_STATS
        RSA_LOGIN_SCENE_1 = RSA_UI,
        RSA_IN_GAME = RSA_SHADOW | RSA_MAIN_SCENE | RSA_NORMAL_FX | RSA_WEATHER | RSA_POST_PROC | RSA_UI
    };

    typedef mem::vector<NiRenderClick*> SpecialRenderTargetClickList;

public:
    RG_RenderManager(void);
    virtual ~RG_RenderManager(void);

	bool Init(void);
	void PostInit(void);
	bool UnInit(void);
	void PreUnInit(void);
    void Recreate(void);

	void CreateRenderFrame(void);

//    void AppendCursor(NiCursorPtr spCursor);

    virtual bool RecvMessage(MG_MessageBase& rkMessage);

	bool IsRenderActive() const ;
	void SetRenderActive( bool isActive );
	
    void SwitchRenderMode(RenderMode eRenderMode);
    void SetRadialBlurPostProcActivity(bool bActive);

    void SetScreenConsoleVisible(bool bVisible);
	void SetSceneBackgroundColor( const NiColor& kClr );
    
	void ClearAvatarRenderTargetGroup();

	void SetGreyScenePostProcActivity(bool bActive);

	void ShieldWeather(bool bValue);

	// TODO: 考虑把反射放到一个单独的click中管理
	NiTexture* GetWaterReflectRenderTex(void) { return NiSmartPointerCast(NiTexture, m_spWaterReflectRenderedTexture); }
	NiTexture* GetWaterRefractRenderTex(void) { return NiSmartPointerCast(NiTexture, m_spWaterRefractRenderedTexture); }

	enum SceneGreyFadeInOutState
	{
		SGFIOS_INACTIVE,
		SGFIOS_FADEIN,
		SGFIOS_FADEOUT
	};
	/// SceneGrey FadeIn FadeOut
	void GreySceneFadeIn( float fDeltaAlphaPerSecond, float fMaxAlpha = 1.0f );
	void GreySceneFadeOut( float fDeltaAlphaPerSecond, float fMinAlpha = 0.0f );

	void Snapshot(bool bWithUI, bool bTiled);

	int GetWidth() { return m_nWidth; }
	int GetHeight() { return m_nHeight; }

	void EnableUIRenderStep(bool bEnable);
	
private:

	// LostDevice
    static bool _callbackResetNotification(bool bBeforeReset, void* pvData);
    static bool _callbackLostDeviceNotification(void* pvData);
	// ShadowMapStep
    static bool _callbackShadowRenderStepPreProc(NiRenderStep* pkCurrentStep, void* pvCallbackData);
	// MainSceneStep-depthClick
    static bool _callbackMainSceneRenderStep_DepthClickPreProc(NiRenderClick* pkCurrentRenderClick, void* pvCallbackData);
	// MainSceneStep-softParticalClick
	static bool _callbackMainSceneRenderStep_SoftParticalesClickPreProc(NiRenderClick* pkCurrentRenderClick, void* pvCallbackData);
	// UIRenderStep
	//static bool _callbackUiRenderStepPreProc(NiRenderStep* pkCurrentStep, void* pvCallbackData);

    bool _createNiRenderer(void);
    void _destoryNiRenderer(void);
	
	void _createAvatarRenderStep(void);
    void _createShadowRenderStep(void);
    void _createMainSceneRenderStep(void);
    void _createNormalFxRenderStep(void);
    void _createWeatherRenderStep(void);
    void _createPostProcessing(void);
    void _createUiRenderStep(void);
    void _createStatsInfoRenderStep(void);

	void _preResetClicksOutputToPostProcessing(void);
	void _postResetClicksOutputToPostProcessing(void);
    void _updateClicksOutputToPostProcessing(void);
	void _destroyClicksOutputToPostProcessing(void);

    void _eventTimerTick(MG_MessageBase& rkMessage);
	void _eventWindowMessage(MG_MessageBase& rkMessage);
	void _eventWindowDeactive(MG_MessageBase& rkMessage);
    //void _eventReloadSettings(MG_MessageBase& rkMessage);

	void _processInput();

    void _renderFrame(void);
    void _displayFrame(void);

    void _updateRenderStepsActivities(unsigned int maskRenderStepActivity);

	void _updateFadeInOut(float fTime);

    void _updateScreenConsole(void);

    // TODO: 考虑把反射放到一个单独的click中管理
	void _createWaterReflect();
    void _renderReflectedScene(float fTime, bool bForce = false);
	void _reflectDefaultReturn();
	void _setClippingPlane(int index, const NiPlane& plane);
	NiPlane _transformClippingPlane(const NiPlane& rkClipPlane, const NiPoint3& kLoc, 
        const NiPoint3& kDir, const NiPoint3& kUp, const NiPoint3& kRight, const NiFrustum& kFrustum);
	void _clearClippingPlane(int index);

	/// ! 获取当前地图角色位置附近的水面高度，作为反射面绘制高度
	float _getCurrentMapWaterHeight(NiPoint3& actPos);
	
	/// 等待桌面恢复
	bool _checkDesktopState( DWORD dwMaxTime );

private:
	
	NiDX9RendererPtr                    m_spNiRenderer;
    NiRenderFramePtr				    m_spRenderFrame;

	// Avatar
	NiDefaultClickRenderStepPtr			m_spAvatarRenderStep;
	RG_AvatarRenderClickPtr				m_spAvatarRenderClick;				//无外发光
	RG_AvatarRenderClickPtr				m_spSmallAvatarRenderClick;		//有外发光
	RG_AvatarRenderClickPtr				m_spNoDepthBufferAvatarRenderClick;
	RG_AvatarBloomEffectPtr				m_spAvatarBloomEffect;		// avatar bloom，使得模型看上去更亮
	RG_AvatarOuterGlowPtr				m_spAvatarOuterGlow;		// avatar外发光，可以指定颜色

    // 阴影
    NiDefaultClickRenderStepPtr		    m_spShadowRenderStep;
    // 主场景
    NiDefaultClickRenderStepPtr		    m_spMainSceneRenderStep;
    // 普通场景
    NiViewRenderClickPtr			    m_spNormalSceneRenderClick;
	//半透明的场景物件
	NiViewRenderClickPtr			    m_spTransparentSceneRenderClick;
	//天空盒等不需要裁减的
	RG_SceneInfiniteRenderViewPtr       m_spInifiniteRenderView;
	RG_SceneRenderViewPtr               m_spSceneRenderView;
    // 草
    RG_GrassRenderClickPtr              m_spGrassRenderClick;
    // 水面
    NiRenderTargetGroupPtr              m_spWaterReflectRenderTargetGroup;
    NiRenderedTexturePtr                m_spWaterReflectRenderedTexture;
    NiRenderedTexturePtr                m_spWaterRefractRenderedTexture;
    NiRenderTargetGroupPtr              m_spWaterRefractRenderTargetGroup;
    float                               m_fLastReflectUpdate;
    NiSingleShaderMaterial*             m_pkReflectStaticShader;
    NiSingleShaderMaterial*             m_pkReflectDynamicShader;
	NiSingleShaderMaterial*             m_pkReflectTerrainShader;
    NiMaterialSwapProcessorPtr          m_spReflectStaticMaterialSwapper;
    NiMaterialSwapProcessorPtr          m_spReflectDynamicMaterialSwapper;
	NiMaterialSwapProcessorPtr          m_spTerrainReflectMaterialSwapper;
    NiVisibleArray                      m_kReflectedObjects;
	NiVisibleArray                      m_kReflectedTerrain;
    unsigned int                        m_uiClipPlaneState;
    // 场景深度图
    bool                                m_bSceneDepthEnabled;
    NiViewRenderClickPtr			    m_spSceneDepthRenderClick;
    NiRenderedTexturePtr                m_spSceneDepthRenderedTex;
    NiRenderTargetGroupPtr              m_spSceneDepthRTG;

    // 普通特效
    NiDefaultClickRenderStepPtr		    m_spNormalFxRenderStep;

    // 天气
    NiDefaultClickRenderStepPtr		    m_spWeatherRenderStep;

    // 后期处理
    Render::RG_PostProcessManagerPtr    m_spPostProcessRenderStep;
	Render::RG_SpaceWarpEffect*         m_pkSpaceWarpEffect;
	Render::RG_OutlineEffect*			m_pkOutlineEffect;
    Render::RG_BloomEffect*             m_pkBloomEffect;
    Render::RG_RadialBlurEffect*        m_pkRadialBlurEffect;
	Render::RG_GreyEffect*				m_pkGreyEffect;
	//Render::RG_SSAOEffect*            m_spSSAOEffect;
	//RG_ClientNDPass*                  m_spClientNDPass;
	SpecialRenderTargetClickList        m_kClicksOutputToPostProcessing;

	bool								m_bIsGreyEnable;

	float								m_fFadeInMaxGrey;
	float								m_fFadeInMinGrey;
	float								m_fCurrentGrey;
	float								m_fFadeStartGrey;
	float								m_fFadeStartTime;
	float								m_fDeltaAlphaPerSecond;
	SceneGreyFadeInOutState				m_SceneGreyState;

    NiDefaultClickRenderStepPtr		    m_spUIRenderStep;

    // UI Effect
    NiViewRenderClickPtr			    m_spUE3DRenderClick;
	RG_UERenderClickPtr					m_spUE2DRenderClick; // 人物名称，血条，蓝条
    // UI
    CUIRenderClickPtr			        m_spUIRenderClick;
    NiCursorRenderClickPtr              m_spCursorRenderClick;
	// UI Fx
	NiViewRenderClickPtr				m_spScreenFxRenderClick;
	NiViewRenderClickPtr				m_spUIFxRenderClick;

    // 屏幕调试信息
	Utility::UT_ScreenConsolePtr        m_spScreenConsole;
	NiDefaultClickRenderStepPtr         m_spScreenRenderStep;

    RenderMode                          m_eLastRenderMode;
	NiTransform                         m_kLastCameraXlate;

    bool                                m_bScreenStatVisibility;
    unsigned int                        m_uiUpdateClicks;

    unsigned int                        m_uiReflectObjects;
    float                               m_fReflectRenderTime;
    float                               m_fReflectCullTime;
    Utility::UT_NiCullingProcess        m_kReflectCuller;

    bool                                m_bSnapshot;
    bool                                m_bFullSnapshot;
	bool                                m_bTiledSnapshot;
	unsigned int						m_bTiledSnapshotMultiplier;

	bool                                m_bIsDeviceLost;
	bool								m_bIsActiveRender;

	bool								m_bIsNeedRecreateRender;
	float								m_fRecreateRenderTickTime;

	int                                 m_nWidth;
	int                                 m_nHeight;
	bool								m_bIsMinimized;
	bool                                m_bIsFullScreen;
};

#endif // _RG_RENDERMANAGER_H
