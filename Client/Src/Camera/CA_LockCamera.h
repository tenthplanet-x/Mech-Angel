#ifndef _CA_LOCK_CAMERA_H
#define _CA_LOCK_CAMERA_H

#include "CA_BaseCamera.h"

class CA_LockCamera : public CA_BaseCamera
{
public:
	explicit CA_LockCamera( NiCameraPtr pkObject);
	virtual ~CA_LockCamera();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	virtual void  ResetCamera();

    float GetPitch(void) { return m_fPitch[m_iActiveLevel]; }
    float GetFov(void) { return m_fFov[m_iActiveLevel]; }
    float GetDistance(void) { return m_fDistance[m_iActiveLevel]; }
    int	  GetActiveLevel(void) { return m_iActiveLevel; }
	float GetHeightOffset(void);
	float GetScale(void);//计算相机缩放比例

	void         SetPitch( int iLevel , float fPitch );
	void         SetDistance( int iLevel , float fDistance );

protected:
	void _event_timerTick(MG_MessageBase& rkMessage);
	void _event_LogicPlayerMove(MG_MessageBase& rkMessage);
	void _event_InputMouseWheel(MG_MessageBase& rkMessage);

	virtual void _compute_camera_translate( float fDistance , float fNearHeight , float fFarHeight );
	virtual void _on_active_level_changed(int newLv);

protected:
	NiPoint3 m_kCurrentLookAtLocation;
	NiPoint3 m_kLastLookAtLocation;

	NiPoint3 m_kListenerLocation;
	float    m_fMoveAccel;
	float    m_fMoveVelocity;
	//0 is far 1 is med 2 is near
	float	 m_fPitch[3];
	float    m_fYaw[3];
	float    m_fDistance[3];
	float    m_fFov[3];
	float    m_fCharOutlineWidth[3];
	float    m_fNearCameraHeightBias;
	float    m_fFarCameraHeightBias;
	int		 m_iActiveLevel;

	//Smooth transition between camera levels
	bool     m_bInTransitionProc;
	float    m_fTransitionStart;
	int      m_iPrevLevel;

	Utility::UT_SmoothFunctor<float> m_kDistanceSmoother;
	Utility::UT_SmoothFunctor<float> m_kYawSmoother;
	Utility::UT_SmoothFunctor<float> m_kPitchSmoother;
	Utility::UT_SmoothFunctor<float> m_kFovSmoother;
	Utility::UT_SmoothFunctor<float> m_kNearHeightSmoother;
	Utility::UT_SmoothFunctor<float> m_kFarHeightSmoother;
	float    m_fSmoothTotalTime;
	float	 m_fCurrentOffsetTotal;
};

#endif