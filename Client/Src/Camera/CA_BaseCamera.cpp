#include "ClientPCH.h"
#include "CA_BaseCamera.h"

CA_BaseCamera::CA_BaseCamera(NiCameraPtr pkObject)
: m_spNiCamera(pkObject)
{
    m_kRotation.MakeIdentity();
}

CA_BaseCamera::~CA_BaseCamera()
{

}

void	CA_BaseCamera::SetFov(float fov)
{
	NiFrustum kFrustum = m_spNiCamera->GetViewFrustum();

	float a = NI_PI / 180.0f * fov;
	float y = tanf(a * 0.5f);
	float x = kFrustum.m_fRight / kFrustum.m_fTop * y;

	kFrustum.m_fLeft = -x;
	kFrustum.m_fRight = x;
	kFrustum.m_fBottom = -y;
	kFrustum.m_fTop = y;

	m_spNiCamera->SetViewFrustum(kFrustum);
	m_spNiCamera->Update(0.0f);
}

void CA_BaseCamera::_reset_camera( float fFov , float fYaw , float fPitch, float fRoll )
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	float fAspectRadio = 4.0f / 3.0f;
	if (pkRenderer)
	{
		Ni2DBuffer* pkBackbuffer = pkRenderer->GetDefaultBackBuffer();
		fAspectRadio = (float)pkBackbuffer->GetWidth() / (float)pkBackbuffer->GetHeight();
	}

	float fVerticalFieldOfViewDegrees = fFov;
	float fVerticalFieldOfViewRad = NI_PI / 180.0f * fVerticalFieldOfViewDegrees;
	float fViewPlaneHalfHeight = tanf(fVerticalFieldOfViewRad * 0.5f);
	float fViewPlaneHalfWidth = fViewPlaneHalfHeight * fAspectRadio;
	NiFrustum kFrustum = NiFrustum(
		-fViewPlaneHalfWidth, fViewPlaneHalfWidth, 
		fViewPlaneHalfHeight, -fViewPlaneHalfHeight,
		g_settings.CAMERA_CLIP_NEAR ,g_settings.CAMERA_CLIP_FAR);
	kFrustum.m_bOrtho = false;

	NiRect<float> kPort(0.0f, 1.0f, 1.0f, 0.0f);
	m_spNiCamera->SetViewFrustum(kFrustum);
	m_spNiCamera->SetViewPort(kPort);

	NiMatrix3 kRot(
		NiPoint3(0.0f, 1.0f, 0.0f),
		NiPoint3(0.0f, 0.0f, 1.0f),
		NiPoint3(1.0f, 0.0f, 0.0f));

	NiMatrix3 kZRot;
	kZRot.MakeZRotation( fYaw / 180.0f * NI_PI );
	NiMatrix3 kXRot;
	kXRot.MakeXRotation( fPitch / 180.0f * NI_PI );
	NiMatrix3 kYRot;
	kYRot.MakeYRotation( fRoll / 180.0f * NI_PI );

	NiMatrix3 kCamRot;
	kCamRot = kZRot * kYRot * kXRot * kRot;

	NiPoint3 kDir = NiPoint3::ZERO;
	NiPoint3 kRight = NiPoint3::ZERO;
	NiPoint3 kUp = NiPoint3::ZERO;
	NiPoint3 kNeg = NiPoint3( -1 , 1 , -1 );
	NiPoint3 kNegY = NiPoint3( 1 , -1 , 1 );

	kCamRot.GetCol( 0 , kDir );
	kCamRot.GetCol( 1 , kUp );
	kCamRot.GetCol( 2 , kRight );

	kDir = NiPoint3::ComponentProduct( kDir , kNeg );
	kUp  = NiPoint3::ComponentProduct( kUp , kNeg );
	kRight= NiPoint3::ComponentProduct( kRight , kNegY );

	m_kRotation.SetCol(0, kUp);
    m_kRotation.SetCol(1, kDir);
    m_kRotation.SetCol(2, kRight);

	m_spNiCamera->SetRotate(m_kRotation);
	m_spNiCamera->Update(0.0f);
}

void CA_BaseCamera::OnRendererResize(float fAspectRadio)
{
	NiFrustum kFrustum = m_spNiCamera->GetViewFrustum();

	float fHalfH = kFrustum.m_fTop;
	kFrustum.m_fRight = fHalfH * fAspectRadio;
	kFrustum.m_fLeft  = -kFrustum.m_fRight;

	m_spNiCamera->SetViewFrustum(kFrustum);
	m_spNiCamera->Update(0.0f);
}

void  CA_BaseCamera::ResetCamera()
{
}

bool  CA_BaseCamera::OnAttach()
{
	this->ResetCamera();
	return true;
}

void  CA_BaseCamera::OnDetach()
{

}
