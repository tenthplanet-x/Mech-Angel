//‰÷»ævideoµΩŒ∆¿ÌÕº÷–
#pragma once

#include "IG_Image.h"
#include <NiDynamicTexture.h>

namespace GUISys
{
	class IG_Image;
}

class UI_VideoTexture : public Memory::MM_BaseObject
{
public:
	UI_VideoTexture(void);
	~UI_VideoTexture(void);
	void RecreateRenderTarget(void);

	bool Create(const char *cpImageName, int width, int height);
	void Destroy(void);

	bool LockBuffer(void **ppBuf, int *pPitch);
	void UnlockBuffer(void);

private:
	NiDX9RendererPtr				m_spRenderer;	//renderer
	NiDynamicTexturePtr				m_spTexture;	//texture to save image
	GUISys::IG_ImagePtr				m_spImage;		//image who tell UIRenderer to render
	GUISys::IG_MaterialPtr			m_spMaterial;	//settexture

	StringType		m_kImageName;
	unsigned int	m_Width, m_Height;
	NiTexture::FormatPrefs	m_eTextureFmt;
};
