#include "ClientPCH.h"
#include "RG_SceneRenderView.h"

#include "CA_CameraManager.h"
#include "SE_SceneManager.h"
#include "IM_Manager.h"

NiImplementRTTI( RG_SceneBatchRenderProcessor , NiRenderListProcessor );

void RG_SceneBatchRenderProcessor::PreRenderProcessList( const NiVisibleArray* pkInput, NiVisibleArray& kOutput, void* pvExtraData )
{
	if (!pkInput)
	{
		return;
	}
	// Get renderer pointer.
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	NIASSERT(pkRenderer);
	const unsigned int uiInputCount = pkInput->GetCount();
#if 1
	NiGeometry& kGeometry = pkInput->GetAt(0);
	NiPropertyStatePtr spPropertyState = kGeometry.GetPropertyState();
	NiDynamicEffectStatePtr spEffectState = kGeometry.GetEffectState();

	pkRenderer->BeginBatch( spPropertyState , spEffectState );
	for (unsigned int ui = 0; ui < uiInputCount; ui++)
	{
		NiGeometry& kGeometry = pkInput->GetAt(ui);
		if ( NiIsExactKindOf( NiTriShape , &kGeometry) && kGeometry.GetSkinInstance() == NULL )
		{
			pkRenderer->BatchRenderShape( NiDynamicCast( NiTriShape , &kGeometry ) );
		}
		else if ( NiIsExactKindOf( NiTriStrips , &kGeometry ) && kGeometry.GetSkinInstance() == NULL )
		{
			pkRenderer->BatchRenderStrips( NiDynamicCast( NiTriStrips , &kGeometry ) );
		}
	}
	pkRenderer->EndBatch();
#else
	
	NiProperty* apkObjectProps[NiProperty::MAX_TYPES];
	NiViewerStringsArray kStrings;

	// Iterate over input array, swapping material and rendering.
	for (unsigned int ui = 0; ui < uiInputCount; ui++)
	{
		NiGeometry& kGeometry = pkInput->GetAt(ui);
		NiPropertyStatePtr spPropertyState = kGeometry.GetPropertyState();
		NiDynamicEffectStatePtr spEffectState = kGeometry.GetEffectState();

		pkRenderer->BeginBatch( spPropertyState , spEffectState );

		spPropertyState->GetProperties( apkObjectProps );

		GfxWriteLog( LOG_TAG_INFO , LOG_SWITCH_INFO , 
			"Dump Scene State %d Begin." , ui );
		for ( int i = 0 ; i < NiProperty::MAX_TYPES ; i++ )
		{
			if ( apkObjectProps[i] )
			{
				apkObjectProps[i]->GetViewerStrings(&kStrings);
				for ( unsigned int k = 6 ; k < kStrings.GetSize() ; k++ )
				{
					char* pcString = kStrings.GetAt(k);
					if ( pcString != NULL )
					{
						GfxWriteLog( LOG_TAG_INFO , LOG_SWITCH_INFO , 
							"%s" ,  pcString );
					}
				}
				kStrings.RemoveAll();
				
			}
		}

		kOutput.Add( kGeometry );
	}
#endif
}

void RG_SceneInfiniteRenderView::SetCameraData( const NiRect<float>& kViewport )
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	NIASSERT(pkRenderer);

	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();

	const NiFrustum& kFrustum = spCamera->GetViewFrustum();
	NiFrustum kNewFrustum = kFrustum;
	kNewFrustum.m_fFar = 500000.0f;

	pkRenderer->SetCameraData(spCamera->GetWorldLocation(),
		spCamera->GetWorldDirection(), spCamera->GetWorldUpVector(),
		spCamera->GetWorldRightVector(), kNewFrustum, spCamera->GetViewPort());

	m_kCuller.SetFrustumFromCamera(spCamera);
}

void RG_SceneInfiniteRenderView::CalculatePVGeometry()
{
	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	SE_SceneManager::GetSingleton().GetInfiniteModel( spCamera , m_kCachedPVGeometry );
}
//----------------------------------------------------
RG_SceneRenderView::RG_SceneRenderView()
: m_kCuller(NULL), m_bUseCustomFrustum(false)
{
	memset(m_nStats, 0, sizeof(m_nStats));
}
//----------------------------------------------------
RG_SceneRenderView::~RG_SceneRenderView()
{
	m_kCachedPVGeometry.RemoveAll();
}
//----------------------------------------------------
void RG_SceneRenderView::SetCameraData(const NiRect<float>& kViewport)
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	NIASSERT(pkRenderer);

	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();

	const NiFrustum& kFrustum = spCamera->GetViewFrustum();
	NiFrustum kNewFrustum = kFrustum;
	if (m_bUseCustomFrustum)
	{
		kNewFrustum = m_kFrustum;
	}

	// Set camera data on renderer.
	const NiRect<float>* pkViewportToUse = NULL;
	pkViewportToUse = &spCamera->GetViewPort();

	//pkRenderer->SetCameraData(spCamera->GetWorldLocation(),
	//	spCamera->GetWorldDirection(), spCamera->GetWorldUpVector(),
	//	spCamera->GetWorldRightVector(), spCamera->GetViewFrustum(),
	//	*pkViewportToUse);
	pkRenderer->SetCameraData(spCamera->GetWorldLocation(),
		spCamera->GetWorldDirection(), spCamera->GetWorldUpVector(),
		spCamera->GetWorldRightVector(), kNewFrustum, *pkViewportToUse);

    m_kCuller.SetFrustumFromCamera(spCamera);
}
//----------------------------------------------------
const NiFrustum& RG_SceneRenderView::GetViewFrustum() const
{
	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	NIASSERT(spCamera);
	return spCamera->GetViewFrustum();
}
//----------------------------------------------------
void RG_SceneRenderView::SetCustomFrustum(const NiFrustum& kFrustum)
{
	m_kFrustum = kFrustum;
}
//----------------------------------------------------
bool RG_SceneRenderView::GetUseCustomFrustum() const
{
	return m_bUseCustomFrustum;
}
//----------------------------------------------------
void RG_SceneRenderView::SetUseCustomFrustum(bool bUseCustomFrustum)
{
	m_bUseCustomFrustum = bUseCustomFrustum;
}
//----------------------------------------------------
void RG_SceneRenderView::CalculatePVGeometry()
{
	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();

	//地形	
	SE_SceneManager::GetSingleton().GetTerrainVisibleArrary( spCamera , m_kCuller, m_kCachedPVGeometry );
	int nVisible = m_kCachedPVGeometry.GetCount();
	m_nStats[S_TERRAIN] = nVisible;

	//角色
	IM_Manager::GetSingleton().GetVisibleArray(spCamera, m_kCuller, m_kCachedPVGeometry );
	m_nStats[S_ACTOR] = m_kCachedPVGeometry.GetCount() - nVisible;		// 该数量只计入对象数量，并未计入SubMeshCount 和Multi Pass产生的DP数
	nVisible = m_kCachedPVGeometry.GetCount();

	//水
	SE_SceneManager::GetSingleton().GetWaterVisibleArray(spCamera, m_kCuller, m_kCachedPVGeometry , 9 );
	m_nStats[S_WATER] = m_kCachedPVGeometry.GetCount() - nVisible;
	nVisible = m_kCachedPVGeometry.GetCount();

	//场景物件
	SE_SceneManager::GetSingleton().GetModelVisibleArray(spCamera, m_kCuller, m_kCachedPVGeometry);
	m_nStats[S_SCENE] = m_kCachedPVGeometry.GetCount() - nVisible;
	nVisible = m_kCachedPVGeometry.GetCount();

	//场景特效
	SE_SceneManager::GetSingleton().GetSceneFxVisibleArray( spCamera , m_kCuller, m_kCachedPVGeometry );
	m_nStats[S_SCENE_FX] = m_kCachedPVGeometry.GetCount() - nVisible;

	//SE_SceneManager::GetSingleton().GetPathMeshVisibleArray( spCamera , m_kCachedPVGeometry );
}

void RG_SceneRenderView::GetSceneRenderStat( size_t Stats[S_STAT_MAX] )
{
	for ( size_t i = 0 ; i < S_STAT_MAX ; i++ )
	{
		Stats[i] = m_nStats[i];
	}
}

//----------------------------------------------------
RG_SceneTransparentRenderView::RG_SceneTransparentRenderView()
: m_kCuller(NULL)
{
}
//----------------------------------------------------
RG_SceneTransparentRenderView::~RG_SceneTransparentRenderView()
{
	m_kCachedPVGeometry.RemoveAll();
}
//----------------------------------------------------
void RG_SceneTransparentRenderView::SetCameraData(const NiRect<float>& kViewport)
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	NIASSERT(pkRenderer);

	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	const NiFrustum& kFrustum = spCamera->GetViewFrustum();
	// Set camera data on renderer.
	const NiRect<float>* pkViewportToUse = NULL;
	pkViewportToUse = &spCamera->GetViewPort();

	pkRenderer->SetCameraData(spCamera->GetWorldLocation(),
		spCamera->GetWorldDirection(), spCamera->GetWorldUpVector(),
		spCamera->GetWorldRightVector(), kFrustum, *pkViewportToUse);

	m_kCuller.SetFrustumFromCamera(spCamera);
}
//----------------------------------------------------
const NiFrustum& RG_SceneTransparentRenderView::GetViewFrustum() const
{
	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	NIASSERT(spCamera);
	return spCamera->GetViewFrustum();
}
//----------------------------------------------------
void RG_SceneTransparentRenderView::CalculatePVGeometry()
{
	CA_CameraManager* pkCameraManager = CA_CameraManager::GetSingletonPtr();
	NiCameraPtr spCamera = pkCameraManager->GetNiCamera();
	SE_SceneManager::GetSingleton().GetSceneTransparentVisibleArray( 
		spCamera , m_kCuller, m_kCachedPVGeometry );
}
//void RG_ClientNDPass::UpdateVisibleArray()
//{
//	if ( SE_SceneManager::GetSingletonPtr() == NULL )
//	{
//		return;
//	}
//	CA_CameraManager* pkCameraManager = CA_CameraManager::GetSingletonPtr();
//	NiCameraPtr spCamera = pkCameraManager->GetNiCamera();
//
//	ClearVisibleArray();
//	SE_SceneManager::GetSingleton().GetTerrainVisibleArrary( 
//		spCamera , mkcum_kSceneTerrain );
//	NiVisibleArray kTemp;
//	SE_SceneManager::GetSingleton().GetModelVisibleArray( 
//		spCamera , kTemp );
//	IM_Manager::GetSingleton().GetVisibleArray( spCamera , kTemp );
//	unsigned int uiQuantity = kTemp.GetCount();
//	unsigned int i;
//	for (i = 0; i < uiQuantity; i++)
//	{
//		NiGeometry& kGeometry = kTemp.GetAt(i);
//		kGeometry.GetSkinInstance() == NULL ?
//		m_kSceneStatic.Add( kGeometry ):
//		m_kSceneSkinned.Add( kGeometry );
//	}
//}


//==============================================================================
// class RG_SceneDepthRenderClick
//==============================================================================

RG_SceneDepthRenderClick::RG_SceneDepthRenderClick(void)
: m_kCuller(NULL)

, m_kTerrainProcessor(NiSingleShaderMaterial::Create("TerrainDepth"), NULL)
, m_kSceneModelProcessor(NiSingleShaderMaterial::Create("StandardSceneObjectStaticDepth"), NiSingleShaderMaterial::Create("StandardSceneObjectSkinnedDepth"))
{
}

RG_SceneDepthRenderClick::~RG_SceneDepthRenderClick(void)
{
}

void RG_SceneDepthRenderClick::PerformRendering(unsigned int uiFrameID)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 0 , 200 , 0 , 255 ) , L"SceneDepth");
    // Reset rendering statistics.
    m_uiNumObjectsDrawn = 0;
    m_fCullTime = m_fRenderTime = 0.0f;

    float fBeginTime = NiGetCurrentTimeInSec();
    CA_CameraManager& rkCameraManager = CA_CameraManager::GetSingleton();
    NiCameraPtr spCamera = rkCameraManager.GetNiCamera();

    // 地形	
    SE_SceneManager::GetSingleton().GetTerrainVisibleArrary(spCamera, m_kCuller, m_kTerrainPVGeometry);
    m_uiNumObjectsDrawn += m_kTerrainPVGeometry.GetCount();
    // 角色
    //IM_Manager::GetSingleton().GetVisibleArray(spCamera, m_kCuller, m_kCachedPVGeometry );
    //m_uiNumObjectsDrawn += m_kActorModelPVGeometry.GetCount();
    // 场景物件
  //  SE_SceneManager::GetSingleton().GetWalkableModelVisibleArray(spCamera, m_kCuller, m_kSceneModelPVGeometry);
	SE_SceneManager::GetSingleton().GetModelVisibleArray(spCamera, m_kCuller, m_kSceneModelPVGeometry);
    m_uiNumObjectsDrawn += m_kSceneModelPVGeometry.GetCount();
	//SE_SceneManager::GetSingleton().GetSceneTransparentVisibleArray(spCamera, m_kCuller, m_kSceneModelPVGeometry);
    // Update rendering statistics.
    m_fCullTime += NiGetCurrentTimeInSec() - fBeginTime;

    // Resize processed geometry list, if necessary (grow only).
    NIASSERT(m_kProcessedGeometry.GetCount() == 0);
    //if (m_kProcessedGeometry.GetAllocatedSize() < m_uiNumObjectsDrawn)
    //{
    //    m_kProcessedGeometry.SetAllocatedSize(m_uiNumObjectsDrawn);
    //}

    fBeginTime = NiGetCurrentTimeInSec();

    // Use render list processor to pre-process the array of geometry.
    m_kTerrainProcessor.PreRenderProcessList(&m_kTerrainPVGeometry, m_kProcessedGeometry, NULL);
    m_kSceneModelProcessor.PreRenderProcessList(&m_kSceneModelPVGeometry, m_kProcessedGeometry, NULL);

    // Update rendering statistics.
    m_fRenderTime += NiGetCurrentTimeInSec() - fBeginTime;

    // Ensure array is clear for next render view.
    T_ASSERT(m_kProcessedGeometry.GetCount() == 0);
    m_kTerrainPVGeometry.RemoveAll();
    m_kSceneModelPVGeometry.RemoveAll();
	D3DPERF_EndEvent();
}

//==============================================================================
// class RG_SceneDepthMatSwapProcessor
//==============================================================================

RG_SceneDepthMatSwapProcessor::RG_SceneDepthMatSwapProcessor(NiMaterial* pkMaterial /*= NULL*/, NiMaterial* pkSkinnedMaterial /*= NULL*/)
: m_spSkinnedMaterial(pkSkinnedMaterial)
{
    m_spMaterial = pkMaterial;
    m_uiMaterialExtraData = NiMaterialInstance::DEFAULT_EXTRA_DATA;
}

void RG_SceneDepthMatSwapProcessor::PreRenderProcessList(const NiVisibleArray* pkInput, NiVisibleArray& kOutput, void* pvExtraData)
{
    if (!pkInput)
    {
        return;
    }

    if (!m_spMaterial && !m_spSkinnedMaterial)
    {
        return;
    }

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    const unsigned int uiInputCount = pkInput->GetCount();
    for (unsigned int ui = 0; ui < uiInputCount; ++ui)
    {
        NiGeometry& kGeometry = pkInput->GetAt(ui);

        NiMaterial* pkMat = (kGeometry.GetSkinInstance() == NULL) ? m_spMaterial : m_spSkinnedMaterial;
        if (pkMat == NULL)
        {
            continue;
        }

        // Backup current active material.
        const NiMaterial* pkPreviousMaterial = kGeometry.GetActiveMaterial();

        // Apply and set active material to new material.
        kGeometry.ApplyAndSetActiveMaterial(pkMat);

        // Render object.
        kGeometry.RenderImmediate(pkRenderer);

        // Restore previous active material.
        kGeometry.SetActiveMaterial(pkPreviousMaterial);
    }

    // Don't add any geometry objects to the output array.
}
