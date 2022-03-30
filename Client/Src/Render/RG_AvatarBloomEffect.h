#pragma once
#include "RG_PostProcessEffect.h"


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class RG_AvatarBloomEffect : public Render::RG_PostProcessEffect
	{
	public:
		RG_AvatarBloomEffect(bool bActive, Render::RG_Dx9MsaaRenderedTex * pkAvatarTex);
		virtual ~RG_AvatarBloomEffect();

		virtual int GetType() const { return Render::RG_PostProcessEffect::BLOOM; };
		virtual bool CreateEffect();
		virtual void CleanupEffect();

		virtual NiRenderTargetGroup* GetInputRenderTargetGroup();
		virtual NiRenderedTexture* GetTargetTexture();

		// Functions for getting and setting the render target group that will
		// contain the final output for this render step.
		virtual bool SetOutputRenderTargetGroup(NiRenderTargetGroup* pkOutputRenderTargetGroup);
		virtual NiRenderTargetGroup* GetOutputRenderTargetGroup();

	protected:
		virtual void PerformRendering();

		bool CreateRenderPass();
		bool CreateRenderedTextures();
		void CleanupRenderedTextures();

		// "Pass" draw routines
		bool DrawHDRSceneScaled();
		void RenderBloom();
		void DrawFinalToDisplay();	

	protected:
		Render::RG_Dx9MsaaRenderedTex * m_pkAvatarTexture;
		Render::RG_PostProcessPass m_kHDRScene;		// use m_pkAvatarTexture, not create by self

		// W/4 x H/4
		Render::RG_PostProcessPass m_kHDRSceneScaled;

		// W/4+2 x W/4+2
		Render::RG_PostProcessPass m_aspBlooms[NUM_BLOOM_TEXTURES];

		// WxH: "Dummy" AuxBuffer for output render target
		Render::RG_PostProcessPass m_kOutputBuffer;

		NiMaterialPtr m_spFinalScenePassMaterial;

		float m_fLastFrameTime;
	};
	NiSmartPointer(RG_AvatarBloomEffect);
