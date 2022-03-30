/********************************************************************
	created:	1:7:2009
	file base:	CA_TrackAnimCamera
	file ext:	h
	author:		ayu
	
	purpose:	For Camera Animation Control
*********************************************************************/
#pragma once

#include "CA_BaseCamera.h"

namespace Camera
{
	class CA_AnimationPath;
}

class CA_TrackAnimCamera : public CA_BaseCamera
{
public:
	explicit CA_TrackAnimCamera(NiCameraPtr pkObject);
	virtual ~CA_TrackAnimCamera();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	virtual bool  OnAttach();
	virtual void  OnDetach();

	bool	PlayAnim(int trackIdx);
	bool	PlayMaxAnim(int chartpyeid, const NiPoint3& src);
	void	StopAnim();
	bool	SetKeyCamera(int trackIdx, int keyIdx, NiPoint3& tp, NiQuaternion& qua, float& fov);
	int		GetKeyCameraCount(int trackIdx);
	void	ModifyKey(int trackIdx, int keyIdx, const NiPoint3& tp , const NiQuaternion& qua);
	void	AddKey(int trackIdx, int keyIdx, const NiPoint3& tp , const NiQuaternion& qua);
	void	RemoveKey(int trackIdx, int keyIdx);

	void	SaveAnimToFile();

	bool	IsNeedEnd();
	void	SetAutoStop(bool kFlag) { m_kAutoStop = kFlag; }

	bool	IsCurrentPlay();

protected:
	 void _eventTimerTick(MG_MessageBase& rkMessage);

protected:
	Camera::CA_AnimationPath* m_pkCurrentPath;
	bool m_kAutoStop;
	Camera::CA_AnimationPath* m_pkMaxPath;
};