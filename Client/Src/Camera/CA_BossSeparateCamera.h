
#pragma once

#include "CA_BaseCamera.h"

class CA_BossSeparateCamera : public CA_BaseCamera
{
public:
	explicit CA_BossSeparateCamera(NiCameraPtr pkObject);
	virtual ~CA_BossSeparateCamera();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	virtual bool  OnAttach();
	virtual void  OnDetach();

	void ShowOther(bool bShow);

protected:
	 void _eventTimerTick(MG_MessageBase& rkMessage);
	 void _releaseScreenFx();

	 enum	MoveState
	 {
		 CMS_NA = -1,
		 CMS_STOP_ON_TARGET,		//在目标身上停留
		 CMS_BACK_TO_MC,			//移回到主角
		 CMS_END,					//结束
	 };

public:
	 bool	isEnd(void) const {return m_enmState == CMS_END; }
	 void	go(bool bBackToMc=false);
	 void	setYaw(float yaw)	{	m_startYaw = yaw; m_lastYaw = yaw; }
	 void	setTargetYaw(float yaw) { m_targetYaw = yaw; }
	 void	setDistance(float dis)	{	m_StartDistance = dis; m_Distance = dis;}
	 void	setTargetDistance(float dis)	{	m_TargetDistance = dis; }
	 void	setHeight(float fHeight) { m_fStartHeight = fHeight; m_fHeight = fHeight; }
	 void	setTargetHeight(float fHeight) { m_fTargetHeight = fHeight; }
	 void	setPitch(float fPitch) { m_fStartPitch = fPitch; m_fPitch = fPitch; }
	 void	setTargetPitch(float fPitch) { m_fTargetPitch = fPitch; }
	 void	setRoll(float fRoll) { m_fStartRoll = fRoll; m_fRoll = fRoll; }
	 void	setTargetRoll(float m_fRoll) { m_fTargetRoll = m_fRoll; }

	 void	back(void);
	 void	setMoveArg(float stop, object_id_type iTargetID)
	 {
		m_fStopTime = stop;
		m_lTargetID = iTargetID;
	 }

private:
	float			m_fStopTime;	//停留时间

	bool			m_bBackToMc;	//结束后是否移回主角

	float			m_fTimer;		//计时器
	MoveState		m_enmState;		//当前镜头状态

	NiPoint3		m_srcPos;
	NiPoint3		m_targetPos;

	object_id_type	m_lTargetID;	//目标id

	float			m_startYaw;		//初始旋转
	float			m_lastYaw;		//上次旋转
	float			m_targetYaw;	//目标旋转

	float			m_fStartPitch;	//初始仰角
	float			m_fPitch;		//当前仰角
	float			m_fTargetPitch;	//目标仰角

	float			m_fStartRoll;	//初始斜角
	float			m_fRoll;		//当前斜角
	float			m_fTargetRoll;	//目标斜角

	float			m_StartDistance;//摄像机初始距离
	float			m_Distance;		//摄像机当前距离
	float			m_TargetDistance;//摄像机目标距离

	float			m_fStartHeight;	//摄像机初始高度
	float			m_fHeight;		//摄像机高度
	float			m_fTargetHeight;//摄像机目标高度
};