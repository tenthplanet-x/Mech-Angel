#include "ClientPCH.h"
#include "RG_GrassRenderClick.h"
#include "SE_SceneManager.h"

RG_GrassRenderClick::RG_GrassRenderClick(void)
{
}

RG_GrassRenderClick::~RG_GrassRenderClick(void)
{
}

void RG_GrassRenderClick::PerformRendering(unsigned int uiFrameID)
{
	T_START_PROFILE( "Render::Scene::GrassClick" );

	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0 , 200 , 0 , 255 ) , L"Render::Scene::GrassClick" );

    float fBeginTime = NiGetCurrentTimeInSec();

	m_uiNumObjectsDrawn = SE_SceneManager::GetSingleton().PerformGrassInstanceRendering();

    m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;

	D3DPERF_EndEvent();

	T_END_PROFILE();
}
