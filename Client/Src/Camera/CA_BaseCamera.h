#ifndef _CA_BASE_CAMERA_H
#define _CA_BASE_CAMERA_H

#include "MG_MessageHandlerInterface.h"
#include <NiCamera.h>

class CA_BaseCamera : public MG_MessageHandlerInterface,
					  public Memory::MM_BaseObject
{
public:
	explicit CA_BaseCamera(NiCameraPtr pkObject);
	virtual ~CA_BaseCamera();
	virtual void  ResetCamera();
	virtual bool  OnAttach();
	virtual void  OnDetach();

	void	SetFov(float fov);	//fov is angle
	void	OnRendererResize(float fAspectRadio);

	void SetNiCamera(NiCameraPtr spNiCamera) { m_spNiCamera = spNiCamera; }
	NiCameraPtr GetNiCamera(void) { return m_spNiCamera; }

protected:
	void _reset_camera( float fFov , float fYaw , float fPitch, float fRoll=1.0f);	//yaw和pitch都是相对世界坐标系的，yao是跟垂直轴的夹角

	NiCameraPtr		m_spNiCamera;
	NiMatrix3		m_kRotation;
};

#endif