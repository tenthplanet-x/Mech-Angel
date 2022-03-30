#include "ClientPCH.h"
#include "RG_ClientRenderStep.h"
#include "UT_Convert.h"

//////////////////////////////////////////////////////////////////////////

RG_ClientRenderStep::RG_ClientRenderStep(const char* name)
{
	SetName(name);
	m_pMbcsName = (const char*)GetName();
	m_kWideCharName = CodeConvert::MbsToWcs( name );
}

RG_ClientRenderStep::~RG_ClientRenderStep(void)
{
}

void RG_ClientRenderStep::PerformRendering(void)
{
	T_START_PROFILE( m_pMbcsName );
	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0 , 200 , 0 , 255 ) , m_kWideCharName.c_str() );
    NiDefaultClickRenderStep::PerformRendering();
	D3DPERF_EndEvent();
	T_END_PROFILE();
}


//////////////////////////////////////////////////////////////////////////


RG_ClientRenderClick::RG_ClientRenderClick(const char* name)
{
	SetName(name);
	m_pMbcsName = (const char*)GetName();
	m_kWideCharName = CodeConvert::MbsToWcs( name );
}

RG_ClientRenderClick::~RG_ClientRenderClick(void)
{
}

void RG_ClientRenderClick::PerformRendering(unsigned int uiFrameID)
{
	T_START_PROFILE( m_pMbcsName );
	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0 , 200 , 0 , 255 ) , m_kWideCharName.c_str() );
	NiViewRenderClick::PerformRendering(uiFrameID);
	D3DPERF_EndEvent();
	T_END_PROFILE();
}