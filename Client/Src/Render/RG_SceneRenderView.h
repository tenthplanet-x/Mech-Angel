#ifndef _RG_SCENE_RENDER_VIEW_H
#define _RG_SCENE_RENDER_VIEW_H

#include "NiRenderView.h"
#include "RG_SSAOEffect.h"
#include "UT_MetricsOutput.h"

//class RG_ClientNDPass : public Render::RG_SceneNDPass
//{
//public:
//	virtual void UpdateVisibleArray();
//};

class RG_SceneBatchRenderProcessor : public NiRenderListProcessor
{
	 NiDeclareRTTI;
public:
	RG_SceneBatchRenderProcessor(){}
	virtual ~RG_SceneBatchRenderProcessor(){}

	virtual void PreRenderProcessList(const NiVisibleArray* pkInput,
		NiVisibleArray& kOutput, void* pvExtraData);
};
NiSmartPointer(RG_SceneBatchRenderProcessor);

class RG_SceneInfiniteRenderView : public NiRenderView
{
public:

	RG_SceneInfiniteRenderView(): m_kCuller(NULL){}
	virtual ~RG_SceneInfiniteRenderView(){}

	virtual void SetCameraData(const NiRect<float>& kViewport);
protected:
	virtual void CalculatePVGeometry();

private:
	Utility::UT_NiCullingProcess m_kCuller;
};
NiSmartPointer(RG_SceneInfiniteRenderView);

class RG_SceneRenderView : public NiRenderView
{
public:
	
	RG_SceneRenderView();
	virtual ~RG_SceneRenderView();

	virtual void SetCameraData(const NiRect<float>& kViewport);

	virtual const NiFrustum& GetViewFrustum() const;
	virtual void SetCustomFrustum(const NiFrustum& kFrustum);
	virtual bool GetUseCustomFrustum() const;
	virtual void SetUseCustomFrustum(bool bUseCustomFrustum);

	void         GetSceneRenderStat( size_t Stats[Utility::S_STAT_MAX] );

protected:
	virtual void CalculatePVGeometry();

private:
    Utility::UT_NiCullingProcess m_kCuller;

	NiFrustum m_kFrustum;
	bool m_bUseCustomFrustum;

	size_t m_nStats[Utility::S_STAT_MAX];
};
NiSmartPointer(RG_SceneRenderView);

class RG_SceneTransparentRenderView : public NiRenderView
{
public:
	RG_SceneTransparentRenderView();
	virtual ~RG_SceneTransparentRenderView();

	virtual void SetCameraData(const NiRect<float>& kViewport);

	virtual const NiFrustum& GetViewFrustum() const;

protected:
	virtual void CalculatePVGeometry();
private:
	 Utility::UT_NiCullingProcess m_kCuller;
};
NiSmartPointer(RG_SceneTransparentRenderView);


//==============================================================================
// Material swap processor for render geometry(with/without skinned)
//==============================================================================

class RG_SceneDepthMatSwapProcessor : public NiMaterialSwapProcessor
{
public:
    RG_SceneDepthMatSwapProcessor(NiMaterial* pkMaterial = NULL, NiMaterial* pkSkinnedMaterial = NULL);

    // Pre-processing function called before any objects are rendered.
    virtual void PreRenderProcessList(const NiVisibleArray* pkInput, NiVisibleArray& kOutput, void* pvExtraData);

    // Functions for accessing the NiMaterial to apply.
    void SetMaterial(NiMaterial* pkMaterial, NiMaterial* pkSkinnedMaterial)
    {
        m_spMaterial = pkMaterial;
        m_spSkinnedMaterial = pkSkinnedMaterial;
    }

protected:
private:
    //RG_SceneDepthMatSwapProcessor(void) {}
    //RG_SceneDepthMatSwapProcessor(NiMaterial* pkMaterial = NULL,
    //    unsigned int uiMaterialExtraData =
    //    NiMaterialInstance::DEFAULT_EXTRA_DATA) {}

protected:
    NiMaterialPtr m_spSkinnedMaterial;
};
NiSmartPointer(RG_SceneDepthMatSwapProcessor);

//==============================================================================
// Render scene depth to RT tex for soft FX computation
//==============================================================================

class RG_SceneDepthRenderClick : public NiViewRenderClick
{
public:
    RG_SceneDepthRenderClick(void);
    virtual ~RG_SceneDepthRenderClick(void);

protected:
    virtual void PerformRendering(unsigned int uiFrameID);

private:
    Utility::UT_NiCullingProcess m_kCuller;

    NiVisibleArray m_kTerrainPVGeometry;
    NiVisibleArray m_kSceneModelPVGeometry;

    RG_SceneDepthMatSwapProcessor m_kTerrainProcessor;
    RG_SceneDepthMatSwapProcessor m_kSceneModelProcessor;
};
NiSmartPointer(RG_SceneDepthRenderClick);

#endif