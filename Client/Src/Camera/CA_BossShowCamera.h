/********************************************************************
	created:	1:7:2014
	file base:	CA_BossShowCamera
	file ext:	h
	author:		ayu
	
	purpose:	目标（npc、角色、目标点）镜头展示
*********************************************************************/
#pragma once

#include "CA_BaseCamera.h"

class CA_BossShowCamera : public CA_BaseCamera
{
public:
	explicit CA_BossShowCamera(NiCameraPtr pkObject);
	virtual ~CA_BossShowCamera();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	virtual bool  OnAttach();
	virtual void  OnDetach();

	void backLockPlayer();

protected:
	 void _eventTimerTick(MG_MessageBase& rkMessage);

	 enum	MoveState
	 {
		 CMS_NA = -1,
		 CMS_MOVE_TO_TARGET = 0,	//移动到目标身上
		 CMS_STOP_ON_TARGET,		//在目标身上停留
		 CMS_BACK_TO_MC,			//移回到主角
		 CMS_END,					//结束
	 };

public:
	 bool	isEnd(void) const {return m_enmState == CMS_END; }
	 void	go(object_id_type lTargetId);
	 void	go(const NiPoint3 & kTargetPos);
	 void	setYaw(float yaw)	{	m_lastYaw = yaw; }
	 void	setDistance(float dis)	{	m_caDistance = dis;}
	 void	back(void);
	 void	setMoveArg(float go, float back, float stop=FLT_MAX, bool bFollow=false)	//stop不传表示一直停留在怪物身上，直到外部控制返回(goBack)
	 {
		m_fGoTime = go;
		m_fBackTime = back;
		m_fStopTime = stop;
		m_bFollow = bFollow;
	 }

private:
	float			m_fGoTime;		//移过去时间
	float			m_fStopTime;	//停留时间
	float			m_fBackTime;	//移回来时间
	bool			m_bFollow;		//停留期间跟随目标

	float			m_fTimer;		//计时器
	MoveState		m_enmState;		//当前镜头状态
	NiPoint3		m_relOffset;	//镜头相对目标的偏移
	NiPoint3		m_srcPos;		//开始移动的位置
	NiPoint3		m_dstPos;		//结束移动的位置

	object_id_type	m_lTargetID;	//目标id

	float			m_lastYaw;		//上次旋转
	float			m_caDistance;		//摄像机距离

	bool			m_backLockPlayer;	//摄像机结束移动的时候是否释放玩家
};