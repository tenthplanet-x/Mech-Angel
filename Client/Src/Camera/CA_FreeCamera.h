#ifndef _CA_FREE_CAMERA_H
#define _CA_FREE_CAMERA_H

#include "CA_BaseCamera.h"

class CA_FreeCamera : public CA_BaseCamera
{
public:
	explicit CA_FreeCamera(NiCameraPtr pkObject);
	virtual ~CA_FreeCamera();

	virtual bool  OnAttach();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	virtual void ResetCamera();

	NiPoint3 GetLockPos();	

	void MoveRotateTo(const NiPoint3& tp, const NiQuaternion& qua, float fov);

protected:
    void	_eventTimerTick(MG_MessageBase& rkMessage);
	void	_event_InputKeyW(MG_MessageBase& rkMessage);
	void	_event_InputKeyS(MG_MessageBase& rkMessage);
	void	_event_InputKeyA(MG_MessageBase& rkMessage);
	void	_event_InputKeyD(MG_MessageBase& rkMessage);
	void	_event_InputPgUp(MG_MessageBase& rkMessage);
	void	_event_InputpgDn(MG_MessageBase& rkMessage);
	void	_event_InputComma(MG_MessageBase& rkMessage);
	void	_event_InputPeriod(MG_MessageBase& rkMessage);
	void	_event_InputMouseMove(MG_MessageBase& rkMessage);
	void	_event_InputMouseWheel(MG_MessageBase& rkMessage);
	void	_event_InputMouseMButtonDown(MG_MessageBase& rkMessage);
	void	_event_InputMouseMButtonUp(MG_MessageBase& rkMessage);

	void	_OnMouseMoveCamera(const POINT& kDelta , POINT kPos );
	void	_OnRotateCamera(const POINT& kDelta,const POINT& kPos);

    void	_moveCamera(const NiPoint3& kCameraWorldXlate);


	NiPoint3 _calcuteDelta(int iIndex);

protected:
	float		m_fCameraMoveSpeed;
	float		m_fCameraRotSpeed;
	int			m_iLastMousePosX;
	int			m_iLastMousePosY;
	bool		m_bTrackingMouse;
	NiPoint3	m_kOrbitPt;

	float		m_lastYaw;
	float		m_lastPitch;
	float		m_lastDistance;
	NiPoint3	m_lockLocation;
	float		m_lastZ;

    NiPoint3    m_kRelativeXlate;
    NiPoint3    m_kLastWorldXlate;

	enum CA_FREE_JOY
	{
		CA_FREE_UP = 0,
		CA_FREE_DOWN = 1,
		CA_FREE_LEFT = 2,
		CA_FREE_RIGHT = 3,
		CA_FREE_ZUP	= 4,
		CA_FREE_ZDOWN = 5,
		CA_FREE_DIS	= 6,
		CA_FREE_YAW = 7,
		CA_FREE_MAX = CA_FREE_YAW + 1,
	};

	bool m_UDLR[CA_FREE_MAX];
	float m_cspeed;
};

#endif