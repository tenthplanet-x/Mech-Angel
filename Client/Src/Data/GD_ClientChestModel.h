#pragma once
//�ͻ��˱���ģ��
#include "GD_ChestModel.h"

class GD_ClientChestModel : public Data::GD_ChestModel
{
public:
    GD_ClientChestModel(void);
	virtual ~GD_ClientChestModel(void);

	enum ChestModelState
	{	
		CMS_INVALID = -1,
		CMS_WAITING,
		CMS_DROPING,
		CMS_FLYTO,
		CMS_FLYHEIGH,
		CMS_FLYRANDOM,
	};

	enum ChestModelFlyType
	{
		CMFT_FLY_HEIGH,
		CMS_FLY_RANDOM,
		CMFT_FLY_LINE,
	};

    void AlternateAppearence(bool bUseMajorAppearence);

    void PlayOpenAnimation(void);
    void PlayCloseAnimation(void);

	void SetUserValue(long lLogicID, float fTime, const NiPoint3& kSrcPos, const NiPoint3& kDestPos, float fSpawnZRotAngle, int bShowGfxLevel);
	void SetFlyToTime(float fTime) {m_fAnimFlyToLifeTime = fTime;}
	void SetFlyToAcceleration(float fAcce) {m_stFlyParam.fAcce = fAcce+0.5f*fAcce*UT_MathBase::RandFloat01();}
	void SetFlyToV0(float fVel) { m_stFlyParam.fV0 = fVel+0.5f*fVel*UT_MathBase::RandFloat01();}
	void SetFlyToVDump(float fDump) {m_stFlyParam.fDump1 = fDump;	m_stFlyHeighParam.fDump1 = fDump;}
	void SetFlyToVDump2(float fDump) {m_stFlyParam.fDump2 = fDump; m_stFlyHeighParam.fDump2 = fDump;}
	void SetFlyAcceDisFactor(float fFactor) {m_stFlyParam.fDischan = fFactor;	m_stFlyHeighParam.fDischan = fFactor;}

	void SetFlyHeighTime(float fTime)  { m_fAnimFlyHeighLiftTime = fTime; }
	void SetFlyHeighAcceleration(float fAcce) { m_stFlyHeighParam.fAcce = fAcce;}
	void SetFlyHeighV0(float v0)	{	m_stFlyHeighParam.fV0 = v0+0.5f*v0*UT_MathBase::RandFloat01();	}
	void SetFlyHeighRadius(float fRadius) { m_stFlyHeighParam.fRadius = fRadius;}
	void SetFlyHeighH(float fHeigh)  { m_stFlyHeighParam.fHeigh = fHeigh; }
	
	void SetWaitingTime(float fTime); 
	void SetDropSoundFlag(bool bFlag) {	m_bDropSoundFlag = bFlag; }
	void UpdateHeight(const NiPoint3& kSrcPos, const NiPoint3& kDestPos);
	const NiPoint3& GetDestPos()const{return m_kDestPos;}

	bool IsFlyAble() {	return m_bIsFlyAble;}
	void SetFlyAble(bool bFlag) { m_bIsFlyAble = bFlag;}
	void SetFlyToID(int FlyToID)  { m_stFlyParam.iFlyToID = FlyToID;}
	//����ʰȡ
	void FlyToPick();
	void FlyToRandom();
	void QuickFlyToPick();

	void SetClientDisplay(bool bFlag) {	m_bIsClientDisplay = bFlag;}
	bool IsClientDisplay()			{return m_bIsClientDisplay;} 

	void		ResetFlyTo();
	void		ResetFlyTo1();
	void		ResetFlyHeigh();
	void		ResetFlyToRandom();
	virtual void ReUse();

	bool		GetChangeModelAutoPick(){return m_bIsAutoPick;}
	void		SetChangeMOdelAutoPick(bool bIsAutoPick){ m_bIsAutoPick = bIsAutoPick;}

protected:
    virtual void _doInitAfterLoad(void);
    virtual void _doPreRelease(void);
	virtual void _doPostUpdate(float fTime);

	virtual bool _doIsUpdatable(float fTime)
	{ 
		return (m_bIsFlyAble || GD_ModelBase::_doIsUpdatable(fTime)); 
	}

	void		_updateFlyHeighAnim(float fDeltaTime);
	void		_updateFlyRandomAnim(float fDeltaTime);
	void		_updateFlyToAnim(float fDeltaTime);

	void		_flyParamReset();

	void		_showName();
	void		_hideName();
private:
    bool m_bIsCurrUsingMajorAppearence;      // ��ǰ�Ƿ���ʹ��������
	float	m_fLastTime;

	long	m_lLogicID;
	const float m_cfTotalAngle;
	int		m_bShowGfxLevel;

	bool	m_bDropSoundFlag;

	bool	m_bIsClientDisplay;
	
	//�����ڵ����Զ�����ʰȡ,�Ƿ��Ѹı�ģ��Ϊ������ı�־λ���ı�Ϊ������ĲŻ����
	bool    m_bIsAutoPick;
	//�ȴ�����ʱ��
	float m_fWaitingTime;
	
	//��ģ�ͼ�����ɺ�ֵ���˺��ٱ仯
	//���䶯�����
	float	m_fAnimLifeTime;	//����ʱ�ķ�ת����ʱ��
	float	m_fAutoPickWaitTime;	//�Զ�ʰȡ�ȴ�ʱ��
	NiPoint3 m_kSrcPos;			//��������
	NiPoint3 m_kDestPos;		//��������

	//����������ֵ������˺��ٱ仯
	float m_fOverturnAngel;			//��λʱ���ڷ�ת�Ƕ�
	float m_fZRotAngle;				//��Z�����ת�Ƕȣ�ͨ��m_kSrcPos ��m_kDestPos ����ó� +_ �����㷽��
	NiPoint3 m_kDeltaPos;			//��λʱ����ƽ�Ʊ任
	float m_fGravity;				//��������
	NiPoint3 m_kDefaultDir;			//��ʼ����

	//ʵʱ������¡�
	float m_fYRotAngle;				//��Y�����ת�Ƕȣ�m_fOverturnAngel * ʱ����ۻ�
	float m_fHeight;				//��ǰ�߶ȵ�ƫ��
	NiPoint3 m_kCurPos;				//��ǰģ������λ��
	float	m_fAnimCurPlayTime;		//��ǰ����ʱ��
	float	m_fOverturnTime;		//����ʱ��

	float m_fMaxHeight;				//�������߶�
	float m_fOverturnHeight;		//��ת�߶�
	
	struct FLY_PARAM
	{
		
		FLY_PARAM()
		{
			reset();
		}

		void reset() 
		{
			fV0 = 0.0f;
			fDump2 = 0.0f;
			fDump1 = 0.0f;
			fV0Dir = NiPoint3::ZERO;
			fAcce = 0.0f; 
			fDischan = 0.0f;
			iFlyToID =  0;
		}

		float fV0;			//���г��ٶ�
		float fDump1;		//���з����ٶȵ�������������ϵ����
		float fDump2;		//���з����ٶȵ�������һ����ϵ����
		NiPoint3 fV0Dir;	//�����ٶȷ���
		float fAcce;		//���з�����ٶ�
		float fDischan;		//���ٶȾ���Ӱ��;
		object_id_type	iFlyToID;		//�����Ŀ��ID
	};

	struct FLY_PARAM_HEIGH :public FLY_PARAM
	{
		FLY_PARAM_HEIGH()
		{
			reset();
		}

		void reset() 
		{
			target =NiPoint3::ZERO;
			fRadius = 0.0f;
			fHeigh = 0.0f;
		}

		NiPoint3	target;		//���Ϸ���Ŀ��
		float		fRadius;			//���Ϸ�������ƫ��
		float		fHeigh;				//���Ϸ��и߶�ƫ��
	};

	//���ж������
	float	m_fAnimFlyToLifeTime;	//���е�ʱ��
	float	m_fAnimCurFlyToTime;	//��ǰ���ж���ʱ��
	ChestModelState	m_eModelState;	//��ǰ������µ�״̬
	bool m_bIsFlyAble;				//�Ƿ��ܹ�����
	bool m_bIsDelayFly;				//�Ƿ�׼������
	FLY_PARAM m_stFlyParam;			//�����˶�������

	ChestModelFlyType m_flyModelType;	//����ģ������
	
	//���Ϸ���
	float m_fAnimFlyHeighLiftTime;	//���и߶�ʱ��
	float m_fAnimCurFlyHeighTime;	//��ǰ���и߶�ʱ��
	FLY_PARAM_HEIGH m_stFlyHeighParam;		//���Ϸ��ж�������
};
