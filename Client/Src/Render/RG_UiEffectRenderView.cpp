#include "ClientPCH.h"

#include "RG_UiEffectRenderView.h"
#include "UE_UiEffectManager.h"
#include "CA_CameraManager.h"
#include "UE_RenderManager.h"
#include "IG_Material.h"
#include "UT_CutSceneManager.h"
//------------------------------------------------------------------------------
RG_UiEffectRenderView::RG_UiEffectRenderView()
{

}
//------------------------------------------------------------------------------
RG_UiEffectRenderView::~RG_UiEffectRenderView()
{

}
//------------------------------------------------------------------------------
void RG_UiEffectRenderView::SetCameraData(const NiRect<float>& /*kViewport*/)
{
	NiRendererPtr spRenderer = NiRenderer::GetRenderer();
	
	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	 //Set camera data on renderer.
	spRenderer->SetCameraData(spCamera->GetWorldLocation(),
		spCamera->GetWorldDirection(), spCamera->GetWorldUpVector(),
		spCamera->GetWorldRightVector(), spCamera->GetViewFrustum(),
		spCamera->GetViewPort() );
}
//------------------------------------------------------------------------------
void RG_UiEffectRenderView::CalculatePVGeometry()
{	
#ifndef _MAP_VIEWER_BUILD
	if( UT_CutSceneManager::GetSingletonPtr() && 
		!UT_CutSceneManager::GetSingleton().ShowUE())
	{
		return;
	}
	UE_UiEffectManager::GetSingleton().GetVisibleArrary(m_kCachedPVGeometry);
	//m_kCachedPVGeometry.RemoveAll();
#endif
}
//------------------------------------------------------------------------------