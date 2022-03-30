//天龙相机模式
#pragma once
#include "CA_LockCamera.h"

class CA_DraconisCamera : public CA_LockCamera
{
public:
	explicit CA_DraconisCamera(NiCameraPtr pkObject);
	virtual ~CA_DraconisCamera();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	void         ResetCamera();

	void         SetPitchRange( float fStart , float fEnd );
	float		 GetYaw()	 {	return m_fAngleZ;}

	NiPoint3	 GetListenerLocation();

protected:
	void _event_InputRMouseDown(MG_MessageBase& rkMessage);
	void _event_InputRMouseUp(MG_MessageBase& rkMessage);
	void _event_InputKeyAlt_VDown(MG_MessageBase& rkMessage);
	//void _event_LogicPlayerMove(MG_MessageBase& rkMessage);
	void _event_InputMouseMove(MG_MessageBase& rkMessage);
	void _event_InputMouseWheel(MG_MessageBase& rkMessage);
	void _event_InputMouseMButtonDown(MG_MessageBase& rkMessage);
	void _event_InputMouseMButtonUp(MG_MessageBase& rkMessage);
	void _event_LogicPlayerMove(MG_MessageBase& rkMessage);
	void _event_timerTick(MG_MessageBase& rkMessage);
	void		 _OnMouseMoveCamera(const POINT& kDelta , POINT kPos );
	void		 _OnRotateCamera(const POINT& kDelta,const POINT& kPos);
	
protected:
	bool        m_bShift;
	float       m_fFarPitchStart;
	float       m_fFarPitchEnd;

	float		m_fCameraMoveSpeed;
	float		m_fCameraRotSpeed;
	int			m_iLastMousePosX;
	int			m_iLastMousePosY;
	bool		m_bTrackingMouse;
	NiPoint3	m_kOrbitPt;
	NiPoint3	m_kWheelDistance;
	float		m_fDistanceMin;
	float		m_fDistanceMax;
	float		m_fDraconisFov;
	float		m_fDraconisPitch;
	float		m_fDraconisYaw;
	float		m_fDraconisDistance;
	float		m_fAngleZ;

	float		m_fRotateJudge;
};
