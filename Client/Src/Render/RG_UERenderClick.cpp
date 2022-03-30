#include "ClientPCH.h"
#include "RG_UERenderClick.h"
#include "UE_RenderManager.h"
#include "IG_RenderCacheManager.h"
#include "UT_CutSceneManager.h"
using namespace GUISys;

RG_UERenderClick::RG_UERenderClick(void)
: m_uiObjects(0)
, m_fRenderTime(0.0f)
{
}

RG_UERenderClick::~RG_UERenderClick(void)
{
}

void RG_UERenderClick::PerformRendering(unsigned int uiFrameID)
{
	m_uiObjects = 0;
#ifndef _MAP_VIEWER_BUILD
	if(!UT_CutSceneManager::GetSingleton().ShowUI())
		return;

	T_PROFILE("Render::UI::UE");
	NiRenderer *pRender = NiRenderer::GetRenderer();
	pRender->SetScreenSpaceCameraData(&m_kViewport);
	UE_RenderManager* pkUERenderManager = UE_RenderManager::GetSingletonPtr();
	if(!pkUERenderManager)
		return;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0 , 200 , 0 , 255 ) , L"Render::UI::UE" );
	
	IG_RenderCacheManager::GetSingleton().NextFrame();
	float fBeginTime = NiGetCurrentTimeInSec();
	m_uiObjects = pkUERenderManager->RenderUEScreen();
	m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;

	D3DPERF_EndEvent();
#endif
}
