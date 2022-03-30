#include "ClientPCH.h"
#include "RG_AvatarBloomEffect.h"
#include "RG_Dx9MsaaRenderedTex.h"
#include "RenderUtils.h"

using namespace Render;

RG_AvatarBloomEffect::RG_AvatarBloomEffect(bool bActive, Render::RG_Dx9MsaaRenderedTex * pkAvatarTex) 
: RG_PostProcessEffect(bActive)
, m_fLastFrameTime(0.0f)
, m_pkAvatarTexture(pkAvatarTex)
{
	SetName("Avatar::BloomEffect");

	bool bOK = false;
	if (pkAvatarTex != NULL)
	{
		NiRenderedTexturePtr spRT = pkAvatarTex->GetRenderedTexture();
		if (spRT != NULL)
		{
			m_uiSceneWidth = spRT->GetWidth();
			m_uiSceneHeight = spRT->GetHeight();
			bOK = true;
		}
	}
	
	if (!bOK)
	{
		m_uiSceneWidth = 2048;
		m_uiSceneHeight = 2048;
		SetActive(false);
	}
}

RG_AvatarBloomEffect::~RG_AvatarBloomEffect()
{
	m_pkAvatarTexture = NULL;
}

//---------------------------------------------------------------------------
bool RG_AvatarBloomEffect::CreateEffect()
{
	if (!GetActive())
		return false;

	if (!CreateRenderPass())
	{
		SetActive(false);
		return false;
	}

	m_spFinalScenePassMaterial = NiSingleShaderMaterial::Create("AvatarHDRScene_FinalScenePass");
	NIASSERT(m_spFinalScenePassMaterial);

	return true;
}
//---------------------------------------------------------------------------
void RG_AvatarBloomEffect::CleanupEffect()
{
	CleanupRenderedTextures();
}

//---------------------------------------------------------------------------
bool RG_AvatarBloomEffect::CreateRenderPass()
{
	bool bSuccess = CreateRenderedTextures();
	if (!bSuccess)
		return false;

	////////////////////////////
	//// downsampled
	NiMaterial* spDownScale4x4Material = NiSingleShaderMaterial::Create("AvatarHDRScene_DownScale4x4");
	NIASSERT(spDownScale4x4Material);

	CreateFullScreenQuad(m_kHDRScene, false, m_kHDRSceneScaled, false,
		spDownScale4x4Material);

	////////////////////////////
	// Bloom
	NiMaterialPtr spBloomMaterial = NiSingleShaderMaterial::Create("HDRScene_Bloom");
	NIASSERT(spBloomMaterial);

	// Bloom Pass for Horizontal
	CreateFullScreenQuad(m_kHDRSceneScaled, false, 
		m_aspBlooms[2], false, spBloomMaterial);
	// Bloom Pass for Vertical
	CreateFullScreenQuad(m_aspBlooms[2], false, 
		m_aspBlooms[1], false, spBloomMaterial);

	////////////////////////////
	// Gamma
	NiMaterialPtr spGammaMaterial = NiSingleShaderMaterial::Create("HDRScene_Gamma");
	NIASSERT(spGammaMaterial);

	CreateFullScreenQuad(m_aspBlooms[1], false, 
		m_aspBlooms[0], false, spGammaMaterial);

	return true;
}

//---------------------------------------------------------------------------
void RG_AvatarBloomEffect::CleanupRenderedTextures()
{
	CleanupAuxBuffer(m_kOutputBuffer);

	for(int i = 0; i < NUM_BLOOM_TEXTURES; i++)
		CleanupAuxBuffer(m_aspBlooms[i]);

	CleanupAuxBuffer(m_kHDRSceneScaled);

	m_kHDRScene.m_pkTex = NULL;
	CleanupAuxBuffer(m_kHDRScene);
}
//---------------------------------------------------------------------------
bool RG_AvatarBloomEffect::SetOutputRenderTargetGroup(
	NiRenderTargetGroup* pkOutputRenderTargetGroup)
{
	NIASSERT(pkOutputRenderTargetGroup);

	if (!m_kOutputBuffer.GetRenderTargetGroup() && m_aspBlooms[0].m_pkTex != NULL )
	{
		m_kOutputBuffer.SetRenderTargetGroup(pkOutputRenderTargetGroup);
		CreateFullScreenQuad(m_kHDRScene, false,
			m_kOutputBuffer, false, m_spFinalScenePassMaterial,
			m_aspBlooms[0].m_pkTex->GetRenderedTexture());
	}

	m_kOutputBuffer.SetRenderTargetGroup(pkOutputRenderTargetGroup);
	return true;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* RG_AvatarBloomEffect::GetOutputRenderTargetGroup()
{
	return m_kOutputBuffer.GetRenderTargetGroup();
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* RG_AvatarBloomEffect::GetInputRenderTargetGroup()
{
	return m_kOutputBuffer.GetRenderTargetGroup();
}
//---------------------------------------------------------------------------
NiRenderedTexture* RG_AvatarBloomEffect::GetTargetTexture()
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
bool RG_AvatarBloomEffect::CreateRenderedTextures()
{
	NIASSERT(m_pkD3DRenderer);

	NiTexture::FormatPrefs kPrefs;

	// Specify buffer sizes
	unsigned int uiSmallerWidths[2] = 
	{ 
		m_uiSceneWidth / 4, 
		m_uiSceneWidth / 8
	};
	unsigned int uiSmallerHeights[2] = 
	{ 
		m_uiSceneHeight / 4, 
		m_uiSceneHeight / 8
	};

	m_kHDRScene.m_pkTex = m_pkAvatarTexture;

	// Scaled size HDR scene render target textures  十六分之一的m_kHDRSceneScaled
	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	bool bSuccess = CreateRenderTarget("avatar_bloom_small" , uiSmallerWidths[0], uiSmallerHeights[0], kPrefs,
		m_kHDRSceneScaled);
	if (!bSuccess)
		return false;

	// Create the temporary and final blooming effect textures
	// Texture has a black border of single texel thickness to fake border 
	// addressing using clamp addressing
    kPrefs.m_ePixelLayout = g_settings.CFG_USE_FLOAT_TEXTURE 
        ? NiTexture::FormatPrefs::FLOAT_COLOR_64 
        : NiTexture::FormatPrefs::TRUE_COLOR_32;

	for (int i = 1; i < NUM_BLOOM_TEXTURES; i++)
	{
		bSuccess = CreateRenderTarget( "avatar_bloom_blur" , uiSmallerWidths[0] , uiSmallerHeights[0] ,
			kPrefs, m_aspBlooms[i]);
		if (!bSuccess)
			return false;
	}

	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	bSuccess = CreateRenderTarget( "avatar_bloom_final" , uiSmallerWidths[0], uiSmallerHeights[0],
		kPrefs, m_aspBlooms[0]);
	if (!bSuccess)
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool RG_AvatarBloomEffect::DrawHDRSceneScaled()
{
	if ( m_kHDRScene.m_pkTex == NULL || m_kHDRSceneScaled.m_pkTex == NULL )
		return false;

	NiPoint2 akSampleOffsets[MAX_SAMPLES];
	GetSampleOffsets_DownScale4x4(m_uiSceneWidth, m_uiSceneHeight, akSampleOffsets);

	UpdateSampleConstants(akSampleOffsets);

	// m_kHDRScene.m_pkTex->Update(); called by outside
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
void RG_AvatarBloomEffect::RenderBloom()
{
	NiPoint2 akSampleOffsets[MAX_SAMPLES];
	float afSampleOffsets[MAX_SAMPLES];
	Point4 akSampleWeights[MAX_SAMPLES];

	////////////////////////////
	// Bloom Pass for Horizontal
	GetSampleOffsets_Bloom(m_aspBlooms[2].GetRenderTargetGroup()->GetWidth(0), 
		afSampleOffsets, akSampleWeights, 3.0f, 2.0f );
	for (int i = 0; i < MAX_SAMPLES; i++ )
		akSampleOffsets[i] = NiPoint2(afSampleOffsets[i], 0.0f);

	UpdateSampleConstants(akSampleOffsets, akSampleWeights);

	// Render the horizontal bloom
	m_aspBlooms[2].Render(m_pkD3DRenderer->GetFrameID());

	m_aspBlooms[2].m_pkTex->Update();
	// Update rendering statistics.
	m_uiNumObjectsDrawn += m_aspBlooms[2].GetNumObjectsDrawn();
	m_fCullTime += m_aspBlooms[2].GetCullTime();
	m_fRenderTime += m_aspBlooms[2].GetRenderTime();

	////////////////////////////
	// Bloom Pass for Vertical
	GetSampleOffsets_Bloom(m_aspBlooms[1].GetRenderTargetGroup()->GetHeight(0),
		afSampleOffsets, akSampleWeights, 3.0f, 2.0f);
	for (int i = 0; i < MAX_SAMPLES; i++ )
		akSampleOffsets[i] = NiPoint2(0.0f, afSampleOffsets[i]);

	UpdateSampleConstants(akSampleOffsets, akSampleWeights);

	// Render the vertical bloom
	m_aspBlooms[1].Render(m_pkD3DRenderer->GetFrameID());

	m_aspBlooms[1].m_pkTex->Update();
	// Update rendering statistics.
	m_uiNumObjectsDrawn += m_aspBlooms[1].GetNumObjectsDrawn();
	m_fCullTime += m_aspBlooms[1].GetCullTime();
	m_fRenderTime += m_aspBlooms[1].GetRenderTime();

	////////////////////////////
	//Gamma Correct
	m_aspBlooms[0].Render(m_pkD3DRenderer->GetFrameID());

	m_aspBlooms[0].m_pkTex->Update();
	// Update rendering statistics.
	m_uiNumObjectsDrawn += m_aspBlooms[0].GetNumObjectsDrawn();
	m_fCullTime += m_aspBlooms[0].GetCullTime();
	m_fRenderTime += m_aspBlooms[0].GetRenderTime();
}   

//---------------------------------------------------------------------------
void RG_AvatarBloomEffect::DrawFinalToDisplay()
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
//---------------------------------------------------------------------------
void RG_AvatarBloomEffect::PerformRendering()
{
	T_PROFILE("Render::RG_AvatarBloomEffect::PerformRendering");
	D3DPERFGuadW kPerf(L"Render::Avatar::Bloom" );

	if (m_pkAvatarTexture != NULL)
		this->SetOutputRenderTargetGroup(m_pkAvatarTexture->GetMsaaRenderTarget());
	else
		return;

	NIASSERT(m_pkD3DRenderer);

	m_pkD3DRenderer->SetBackgroundColor(NiColor::BLACK);

	// Reset rendering statistics.
	m_uiNumObjectsDrawn = 0;
	m_fCullTime = m_fRenderTime = 0.0f;

	if( !DrawHDRSceneScaled() )
	{
		m_bActive = false;
		return;
	}

	RenderBloom();

	DrawFinalToDisplay();

	if (m_kHDRScene.m_pkTex)
		m_kHDRScene.m_pkTex->Update();
}
