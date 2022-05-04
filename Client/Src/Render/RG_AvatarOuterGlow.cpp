#include "ClientPCH.h"
#include "RG_AvatarOuterGlow.h"
#include "RG_Dx9MsaaRenderedTex.h"
#include "RenderUtils.h"

using namespace Render;

RG_AvatarOuterGlow::RG_AvatarOuterGlow(bool bActive, Render::RG_Dx9MsaaRenderedTex * pkAvatarTex, Render::RG_Dx9MsaaRenderedTex * pkAvatarTexColored) 
: RG_PostProcessEffect(bActive)
, m_fLastFrameTime(0.0f)
, m_pkAvatarTextureOriginal(pkAvatarTex)	// 原色RT
, m_pkAvatarTexture(pkAvatarTexColored)		// 外发光的纯色RT
{
	SetName("Avatar::OuterGlow");

	bool bOK = false;
	if (pkAvatarTex != NULL && pkAvatarTexColored != NULL)
	{
		NiRenderedTexturePtr spRT = m_pkAvatarTexture->GetRenderedTexture();
		if (spRT != NULL)
		{
			m_uiSceneWidth = spRT->GetWidth();
			m_uiSceneHeight = spRT->GetHeight();
			bOK = true;
		}
	}

	if (!bOK)
	{
		m_uiSceneWidth = 0;
		m_uiSceneHeight = 0;
		SetActive(false);
	}
}

RG_AvatarOuterGlow::~RG_AvatarOuterGlow()
{
	m_pkAvatarTextureOriginal = NULL;
	m_pkAvatarTexture = NULL;
}

//---------------------------------------------------------------------------
bool RG_AvatarOuterGlow::CreateEffect()
{
	if (!GetActive())
		return false;

	if (!CreateRenderPass())
	{
		SetActive(false);
		return false;
	}

	m_spFinalScenePassMaterial = NiSingleShaderMaterial::Create("AvatarOuterGlow_FinalPass");
	NIASSERT(m_spFinalScenePassMaterial);

	return true;
}
//---------------------------------------------------------------------------
void RG_AvatarOuterGlow::CleanupEffect()
{
	CleanupRenderedTextures();
}

//---------------------------------------------------------------------------
bool RG_AvatarOuterGlow::CreateRenderPass()
{
	bool bSuccess = CreateRenderedTextures();
	if (!bSuccess)
		return false;

	////////////////////////////
	//// downsampled
	NiMaterial* spDownScale4x4Material = NiSingleShaderMaterial::Create("AvatarOuterGlow_DownScale2x2");
	NIASSERT(spDownScale4x4Material);

	CreateFullScreenQuad(m_kHDRScene, false, m_kHDRSceneScaled, false,
		spDownScale4x4Material);

	////////////////////////////
	// Bloom
	NiMaterialPtr spBloomMaterial = NiSingleShaderMaterial::Create("AvatarOuterGlow_HDRScene_Bloom");
	NIASSERT(spBloomMaterial);

	// Bloom Pass for Horizontal
	CreateFullScreenQuad(m_kHDRSceneScaled, false, 
		m_aspBlooms[1], false, spBloomMaterial);
	// Bloom Pass for Vertical
	CreateFullScreenQuad(m_aspBlooms[1], false, 
		m_aspBlooms[0], false, spBloomMaterial);

	return true;
}

//---------------------------------------------------------------------------
void RG_AvatarOuterGlow::CleanupRenderedTextures()
{
	CleanupAuxBuffer(m_kOutputBuffer);

	for(int i = 0; i < NUM_OUTER_GLOW_TEXTURES; i++)
		CleanupAuxBuffer(m_aspBlooms[i]);

	CleanupAuxBuffer(m_kHDRSceneScaled);

	m_kHDRScene.m_pkTex = NULL;
	CleanupAuxBuffer(m_kHDRScene);

	m_kHDRSceneOriginal.m_pkTex = NULL;
	CleanupAuxBuffer(m_kHDRSceneOriginal);
}
//---------------------------------------------------------------------------
bool RG_AvatarOuterGlow::SetOutputRenderTargetGroup(
	NiRenderTargetGroup* pkOutputRenderTargetGroup)
{
	NIASSERT(pkOutputRenderTargetGroup);

	if (!m_kOutputBuffer.GetRenderTargetGroup() && m_aspBlooms[0].m_pkTex != NULL )
	{
		m_kOutputBuffer.SetRenderTargetGroup(pkOutputRenderTargetGroup);
		CreateFullScreenQuad(m_kHDRSceneOriginal, false,
			m_kOutputBuffer, false, m_spFinalScenePassMaterial,
			m_aspBlooms[0].m_pkTex->GetRenderedTexture());
	}

	m_kOutputBuffer.SetRenderTargetGroup(pkOutputRenderTargetGroup);
	return true;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* RG_AvatarOuterGlow::GetOutputRenderTargetGroup()
{
	return m_kOutputBuffer.GetRenderTargetGroup();
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* RG_AvatarOuterGlow::GetInputRenderTargetGroup()
{
	return m_kOutputBuffer.GetRenderTargetGroup();
}
//---------------------------------------------------------------------------
NiRenderedTexture* RG_AvatarOuterGlow::GetTargetTexture()
{
	if ( m_kHDRScene.m_pkTex != NULL )
	{
		return m_kHDRScene.m_pkTex->GetRenderedTexture();
	}
	else
	{
		return NULL;
	}
}
//---------------------------------------------------------------------------
bool RG_AvatarOuterGlow::CreateRenderedTextures()
{
	NIASSERT(m_pkD3DRenderer);

	NiTexture::FormatPrefs kPrefs;

	// Specify buffer sizes
	unsigned int uiSmallerWidths[1] = 
	{ 
		m_uiSceneWidth, 
	};
	unsigned int uiSmallerHeights[1] = 
	{ 
		m_uiSceneHeight, 
	};

	m_kHDRSceneOriginal.m_pkTex = m_pkAvatarTextureOriginal;
	m_kHDRScene.m_pkTex = m_pkAvatarTexture;

	// Scaled size HDR scene render target textures  十六分之一的m_kHDRSceneScaled
	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	bool bSuccess = CreateRenderTarget("avatar_outerglow_bloom_small" , uiSmallerWidths[0], uiSmallerHeights[0], kPrefs,
		m_kHDRSceneScaled);
	if (!bSuccess)
	{
		SetActive(false);
		return false;
	}

	// Create the temporary and final blooming effect textures
	// Texture has a black border of single texel thickness to fake border 
	// addressing using clamp addressing
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;

	for (int i = 0; i < NUM_OUTER_GLOW_TEXTURES; i++)
	{
		bool bSuccess = CreateRenderTarget( "avatar_outerglow_blur" , uiSmallerWidths[0] , uiSmallerHeights[0] ,
			kPrefs, m_aspBlooms[i]);
		if (!bSuccess)
			return false;
	}

	return true;
}
//---------------------------------------------------------------------------
bool RG_AvatarOuterGlow::DrawHDRSceneScaled()
{
	if ( m_kHDRScene.m_pkTex == NULL || m_kHDRSceneScaled.m_pkTex == NULL )
		return false;

	NiPoint2 akSampleOffsets[MAX_SAMPLES];
	GetSampleOffsets_DownScale2x2(m_uiSceneWidth, m_uiSceneHeight, akSampleOffsets);

	UpdateSampleConstants(akSampleOffsets);

	m_kHDRScene.m_pkTex->Update();
	// Draw to create the scaled BloomEffect
	m_kHDRSceneScaled.Render(m_pkD3DRenderer->GetFrameID());

	m_kHDRSceneScaled.m_pkTex->Update();

	// Update rendering statistics.
	m_uiNumObjectsDrawn += m_kHDRSceneScaled.GetNumObjectsDrawn();
	m_fCullTime += m_kHDRSceneScaled.GetCullTime();
	m_fRenderTime += m_kHDRSceneScaled.GetRenderTime();
	return true;
}
//---------------------------------------------------------------------------
void RG_AvatarOuterGlow::RenderBloom()
{
	NiPoint2 akSampleOffsets[MAX_SAMPLES];
	float afSampleOffsets[MAX_SAMPLES];
	Point4 akSampleWeights[MAX_SAMPLES];

	////////////////////////////
	// Bloom Pass for Horizontal
	/*
	GetSampleOffsets_Bloom(m_aspBlooms[1].GetRenderTargetGroup()->GetWidth(0), 
		afSampleOffsets, akSampleWeights, 3.0f, 1.0f );
	/*/
	float width = (float)(m_aspBlooms[1].GetRenderTargetGroup()->GetWidth(0));
	float tu = 1.0f / width;
	afSampleOffsets[0] = -2.0f * tu;
	afSampleOffsets[1] = -1.0f * tu;
	afSampleOffsets[2] =  0.0f * tu;
	afSampleOffsets[3] =  1.0f * tu;
	afSampleOffsets[4] =  2.0f * tu;
	//*/
	for (int i = 0; i < MAX_SAMPLES; i++ )
		akSampleOffsets[i] = NiPoint2(afSampleOffsets[i], 0.0f);

	UpdateSampleConstants(akSampleOffsets, akSampleWeights);

	// Render the horizontal bloom
	m_aspBlooms[1].Render(m_pkD3DRenderer->GetFrameID());

	m_aspBlooms[1].m_pkTex->Update();
	// Update rendering statistics.
	m_uiNumObjectsDrawn += m_aspBlooms[1].GetNumObjectsDrawn();
	m_fCullTime += m_aspBlooms[1].GetCullTime();
	m_fRenderTime += m_aspBlooms[1].GetRenderTime();

	////////////////////////////
	// Bloom Pass for Vertical
	/*
	GetSampleOffsets_Bloom(m_aspBlooms[0].GetRenderTargetGroup()->GetHeight(0),
		afSampleOffsets, akSampleWeights, 3.0f, 1.0f);
	/*/
	float height = (float)(m_aspBlooms[1].GetRenderTargetGroup()->GetHeight(0));
	float tv = 1.0f / height;
	afSampleOffsets[0] = -2.0f * tv;
	afSampleOffsets[1] = -1.0f * tv;
	afSampleOffsets[2] =  0.0f * tv;
	afSampleOffsets[3] =  1.0f * tv;
	afSampleOffsets[4] =  2.0f * tv;
	//*/
	for (int i = 0; i < MAX_SAMPLES; i++ )
		akSampleOffsets[i] = NiPoint2(0.0f, afSampleOffsets[i]);

	UpdateSampleConstants(akSampleOffsets, akSampleWeights);

	// Render the vertical bloom
	m_aspBlooms[0].Render(m_pkD3DRenderer->GetFrameID());

	m_aspBlooms[0].m_pkTex->Update();
	// Update rendering statistics.
	m_uiNumObjectsDrawn += m_aspBlooms[0].GetNumObjectsDrawn();
	m_fCullTime += m_aspBlooms[0].GetCullTime();
	m_fRenderTime += m_aspBlooms[0].GetRenderTime();
}   

//---------------------------------------------------------------------------
void RG_AvatarOuterGlow::DrawFinalToDisplay()
{
	// This "pass" draws a frame buffer size quad that is the composition
	// of data created in previous "passes".  It's target is not a 
	// rendered texture, but rather the backbuffer.

	m_kOutputBuffer.Render(m_pkD3DRenderer->GetFrameID());

	// Update rendering statistics.
	m_uiNumObjectsDrawn += m_kOutputBuffer.GetNumObjectsDrawn();
	m_fCullTime += m_kOutputBuffer.GetCullTime();
	m_fRenderTime += m_kOutputBuffer.GetRenderTime();
}


void RG_AvatarOuterGlow::CleanupRenderedTextures()
{
	CleanupAuxBuffer(m_kOutputBuffer);

	for(int i = 0; i < NUM_OUTER_GLOW_TEXTURES; i++)
		CleanupAuxBuffer(m_aspBlooms[i]);

	CleanupAuxBuffer(m_kHDRSceneScaled);

	m_kHDRScene.m_pkTex = NULL;
	CleanupAuxBuffer(m_kHDRScene);

	m_kHDRSceneOriginal.m_pkTex = NULL;
	CleanupAuxBuffer(m_kHDRSceneOriginal);
}
//---------------------------------------------------------------------------
void RG_AvatarOuterGlow::PerformRendering()
{
	T_PROFILE("Render::RG_AvatarOuterGlow::PerformRendering");
	D3DPERFGuadW kPerf(L"Render::Avatar::OuterGlow" );
	NIASSERT(m_pkD3DRenderer);

	// Reset rendering statistics.
	m_uiNumObjectsDrawn = 0;
	m_fCullTime = m_fRenderTime = 0.0f;

	// 与周围像素做模糊处理
	if( !DrawHDRSceneScaled() )
	{
		m_bActive = false;
		return;
	}

	if (m_kHDRSceneOriginal.m_pkTex == NULL  || m_kHDRScene.m_pkTex == NULL)
	{
		m_bActive = false;
		return;
	}

	// 纯色部分bloom
	this->SetOutputRenderTargetGroup(m_kHDRScene.m_pkTex->GetMsaaRenderTarget());

	RenderBloom();

	// 混合到原色上
	this->SetOutputRenderTargetGroup(m_kHDRSceneOriginal.m_pkTex->GetMsaaRenderTarget());

	DrawFinalToDisplay();

	m_kHDRSceneOriginal.m_pkTex->Update();
}



//---------------------------------------------------------------------------
bool RG_AvatarOuterGlow::CreateRenderPass()
{
	bool bSuccess = CreateRenderedTextures();
	if (!bSuccess)
		return false;

	////////////////////////////
	//// downsampled
	NiMaterial* spDownScale4x4Material = NiSingleShaderMaterial::Create("AvatarOuterGlow_DownScale2x2");
	NIASSERT(spDownScale4x4Material);

	////////////////////////////
	// Bloom
	NiMaterialPtr spBloomMaterial = NiSingleShaderMaterial::Create("AvatarOuterGlow_HDRScene_Bloom");
	NIASSERT(spBloomMaterial);

	// Bloom Pass for Horizontal
	CreateFullScreenQuad(m_kHDRSceneScaled, false, 
		m_aspBlooms[1], false, spBloomMaterial);
	// Bloom Pass for Vertical
	CreateFullScreenQuad(m_aspBlooms[1], false, 
		m_aspBlooms[0], false, spBloomMaterial);

	return true;
}

