#pragma once
//For Camera Animation Control
#include "CA_BaseCamera.h"

class CA_AnimCamera : public CA_BaseCamera
{
public:
	explicit CA_AnimCamera(NiCameraPtr pkObject);
	virtual ~CA_AnimCamera();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

protected:
	 void _eventTimerTick(MG_MessageBase& rkMessage);
};