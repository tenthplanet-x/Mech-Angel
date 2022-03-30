#include "ClientPCH.h"
#include "IG_ImageManager.h"
#include "IG_Material.h"
#include "IG_MaterialManager.h"
#include "RG_Dx9MsaaRenderedTex.h"
#include "UI_VideoTexture.h"


UI_VideoTexture::UI_VideoTexture(void)
: m_spRenderer((NiDX9Renderer*)NiRenderer::GetRenderer())
, m_spTexture(NULL)
, m_spImage(NULL)
, m_spMaterial(NULL)
, m_Width(0)
, m_Height(0)
{
	m_eTextureFmt.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	m_eTextureFmt.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;
	m_eTextureFmt.m_eMipMapped = NiTexture::FormatPrefs::NO;
}

UI_VideoTexture::~UI_VideoTexture(void)
{
	T_ASSERT_CRITICAL_S(m_spTexture == NULL);
	T_ASSERT_CRITICAL_S(m_spImage == NULL);
	T_ASSERT_CRITICAL_S(m_spMaterial == NULL);
}

void UI_VideoTexture::RecreateRenderTarget(void)	//called on device-lost
{
	if (m_Width >0 && m_Height > 0)
		this->Create(m_kImageName.c_str(), m_Width, m_Height);
}

bool UI_VideoTexture::Create(const char *cpImageName, int width, int height)
{
	T_ASSERT(width > 0);
	T_ASSERT(height > 0);
	m_Width = width;
	m_Height = height;
	m_kImageName = cpImageName;

	//////////////////////////////////////////////////////////////////////////
	m_spTexture = NULL;		//release old
	m_spTexture = NiDynamicTexture::Create(width, height, m_eTextureFmt);
	if(m_spTexture)
	{
		NiTexturePtr spTexture = NiSmartPointerCast(NiTexture, m_spTexture);

		if (m_spMaterial == NULL)
		{
			m_spMaterial = GUISys::IG_MaterialManager::GetSingleton().CreateMaterial(cpImageName, spTexture);
			if (m_spMaterial != NULL)
			{
				m_spImage = GUISys::IG_ImageManager::GetSingleton().CreateImage(m_spMaterial,
					cpImageName, GUISys::GUIRect(GUISys:: GUIPoint2(0, 0), GUISys::GUISize((float)width, (float)height) ));
			}
		}

		m_spMaterial->SetTexture(spTexture);
	}

	return m_spTexture != NULL;
}

void UI_VideoTexture::Destroy(void)
{
	if (m_spImage != NULL)
	{
		GUISys::IG_ImageManager::GetSingleton().DestroyImage(m_kImageName);
		m_spImage = NULL;	//release resource
		m_kImageName = "";
	}
	m_spMaterial = NULL;	//release resource
	m_spTexture = NULL;		//release resource
	m_Width = 0;
	m_Height = 0;
}

bool UI_VideoTexture::LockBuffer(void **ppBuf, int *pPitch)
{
	*ppBuf = NULL;
	*pPitch = 0;

	//////////////////////////////////////////////////////////////////////////
	if (m_spTexture == NULL)
		return false;

	int pitch = 0;
	void *ptr = m_spTexture->Lock(pitch);
	if (ptr == NULL)
		return false;

	*ppBuf = ptr;
	*pPitch = pitch;

	return true;
}

void UI_VideoTexture::UnlockBuffer(void)
{
	m_spTexture->UnLock();
}
