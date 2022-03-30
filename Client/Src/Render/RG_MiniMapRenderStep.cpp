#include "ClientPCH.h"
#include "RG_MiniMapRenderStep.h"
#include "SE_WorldMiniMap.h"
#include "SE_SceneManager.h"
//-------------------------------------------------
RG_MiniMapRenderStep::RG_MiniMapRenderStep()
{
}
//-------------------------------------------------
RG_MiniMapRenderStep::~RG_MiniMapRenderStep()
{
}
//-------------------------------------------------
void RG_MiniMapRenderStep::PerformRendering()
{
	SE_SceneManager::GetSingleton().GetWorldMiniMap()->Render(NiRenderer::GetRenderer());
}
//-------------------------------------------------
bool RG_MiniMapRenderStep::SetOutputRenderTargetGroup( NiRenderTargetGroup* pkOutputRenderTargetGroup )
{
	return true;
}
//-------------------------------------------------
NiRenderTargetGroup* RG_MiniMapRenderStep::GetOutputRenderTargetGroup()
{
	return NULL;
}
//-------------------------------------------------
