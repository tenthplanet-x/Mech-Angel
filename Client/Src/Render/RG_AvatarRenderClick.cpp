#include "ClientPCH.h"
#include "RG_AvatarRenderClick.h"
#include "UI_AvatarManager.h"

RG_AvatarRenderClick::RG_AvatarRenderClick(void)
: m_uiObjects(0)
, m_fRenderTime(0.0f)
, m_iIndex(RENDER_CLICK_ORIGIN)
{
	m_kBackgroudColor = NiColorA(0.0f, 0.0f, 0.0f, 0.0f);
	SetUseRendererBackgroundColor(false);
	SetClearColorBuffers(true);

	m_spRenderValidator = NiNew RG_AvatarRenderValidator;
	SetValidator(m_spRenderValidator);

	SetPreProcessingCallbackFunc(_callbackPreProc);
	SetPostProcessingCallbackFunc(_callbackPostProc);
}

RG_AvatarRenderClick::~RG_AvatarRenderClick(void)
{
	SetValidator(NULL);
	m_spRenderValidator = NULL;
}

void RG_AvatarRenderClick::PerformRendering(unsigned int uiFrameID)
{
	float fBeginTime = NiGetCurrentTimeInSec();

	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0 , 200 , 0 , 255 ) , L"AvatarRender" );
	FX::FX_Element::ResetDPCounter();

	UI_AvatarManager* avatarManager = UI_AvatarManager::GetSingletonPtr();
	if ( avatarManager )
		avatarManager->publicCallBack_PerformRendering(this);

	D3DPERF_EndEvent();

	m_uiObjects = FX::FX_Element::GetDPPerRendering();
	m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;
}

bool RG_AvatarRenderClick::_callbackPreProc(NiRenderClick* pkRenderClick, void* pvCallbackData)
{
	UI_AvatarManager* avatarManager = UI_AvatarManager::GetSingletonPtr();
	if ( avatarManager )
	{
		avatarManager->publicCallBack_PreRenderClickProc(pkRenderClick);
	}

	return true;
}

bool RG_AvatarRenderClick::_callbackPostProc(NiRenderClick* pkRenderClick, void* pvCallbackData)
{
	UI_AvatarManager* avatarManager = UI_AvatarManager::GetSingletonPtr();
	if ( avatarManager )
	{
		avatarManager->publicCallBack_PostRenderClickProc(pkRenderClick);
	}

	return true;
}

bool RG_AvatarRenderValidator::ValidateClick( NiRenderClick* pkRenderClick, unsigned int uiFrameID )
{
	UI_AvatarManager* avatarManager = UI_AvatarManager::GetSingletonPtr();
	if ( avatarManager )
		return avatarManager->publicCallBack_PrePerformRendering();
	return false;
}

void RG_AvatarRenderClick::SetBgColorInfo(bool bSpecial)
{
	m_kBackgroudColor = NiColorA(0.0f, 0.0f, 0.0f, 1.0f);
	//SetClearColorBuffers(true);
}