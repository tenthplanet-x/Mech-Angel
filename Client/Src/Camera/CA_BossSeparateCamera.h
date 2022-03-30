
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
		 CMS_STOP_ON_TARGET,		//��Ŀ������ͣ��
		 CMS_BACK_TO_MC,			//�ƻص�����
		 CMS_END,					//����
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
	float			m_fStopTime;	//ͣ��ʱ��

	bool			m_bBackToMc;	//�������Ƿ��ƻ�����

	float			m_fTimer;		//��ʱ��
	MoveState		m_enmState;		//��ǰ��ͷ״̬

	NiPoint3		m_srcPos;
	NiPoint3		m_targetPos;

	object_id_type	m_lTargetID;	//Ŀ��id

	float			m_startYaw;		//��ʼ��ת
	float			m_lastYaw;		//�ϴ���ת
	float			m_targetYaw;	//Ŀ����ת

	float			m_fStartPitch;	//��ʼ����
	float			m_fPitch;		//��ǰ����
	float			m_fTargetPitch;	//Ŀ������

	float			m_fStartRoll;	//��ʼб��
	float			m_fRoll;		//��ǰб��
	float			m_fTargetRoll;	//Ŀ��б��

	float			m_StartDistance;//�������ʼ����
	float			m_Distance;		//�������ǰ����
	float			m_TargetDistance;//�����Ŀ�����

	float			m_fStartHeight;	//�������ʼ�߶�
	float			m_fHeight;		//������߶�
	float			m_fTargetHeight;//�����Ŀ��߶�
};