#pragma once
//登陆场景用的相机
#include "CA_BaseCamera.h"

class CA_SelCharCamera : public CA_BaseCamera
{
public:
	explicit CA_SelCharCamera(NiCameraPtr pkObject);
	virtual ~CA_SelCharCamera();
};
