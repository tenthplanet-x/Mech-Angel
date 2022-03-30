/********************************************************************
	created:	1:7:2014
	file base:	CA_BossShowCamera
	file ext:	h
	author:		ayu
	
	purpose:	Ŀ�꣨npc����ɫ��Ŀ��㣩��ͷչʾ
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
		 CMS_MOVE_TO_TARGET = 0,	//�ƶ���Ŀ������
		 CMS_STOP_ON_TARGET,		//��Ŀ������ͣ��
		 CMS_BACK_TO_MC,			//�ƻص�����
		 CMS_END,					//����
	 };

public:
	 bool	isEnd(void) const {return m_enmState == CMS_END; }
	 void	go(object_id_type lTargetId);
	 void	go(const NiPoint3 & kTargetPos);
	 void	setYaw(float yaw)	{	m_lastYaw = yaw; }
	 void	setDistance(float dis)	{	m_caDistance = dis;}
	 void	back(void);
	 void	setMoveArg(float go, float back, float stop=FLT_MAX, bool bFollow=false)	//stop������ʾһֱͣ���ڹ������ϣ�ֱ���ⲿ���Ʒ���(goBack)
	 {
		m_fGoTime = go;
		m_fBackTime = back;
		m_fStopTime = stop;
		m_bFollow = bFollow;
	 }

private:
	float			m_fGoTime;		//�ƹ�ȥʱ��
	float			m_fStopTime;	//ͣ��ʱ��
	float			m_fBackTime;	//�ƻ���ʱ��
	bool			m_bFollow;		//ͣ���ڼ����Ŀ��

	float			m_fTimer;		//��ʱ��
	MoveState		m_enmState;		//��ǰ��ͷ״̬
	NiPoint3		m_relOffset;	//��ͷ���Ŀ���ƫ��
	NiPoint3		m_srcPos;		//��ʼ�ƶ���λ��
	NiPoint3		m_dstPos;		//�����ƶ���λ��

	object_id_type	m_lTargetID;	//Ŀ��id

	float			m_lastYaw;		//�ϴ���ת
	float			m_caDistance;		//���������

	bool			m_backLockPlayer;	//����������ƶ���ʱ���Ƿ��ͷ����
};