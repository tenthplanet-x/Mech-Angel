#pragma once
//客户端宝箱模型
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
	//飞行拾取
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
    bool m_bIsCurrUsingMajorAppearence;      // 当前是否在使用主材质
	float	m_fLastTime;

	long	m_lLogicID;
	const float m_cfTotalAngle;
	int		m_bShowGfxLevel;

	bool	m_bDropSoundFlag;

	bool	m_bIsClientDisplay;
	
	//副本内道具自动飞行拾取,是否已改变模型为真气球的标志位，改变为真气球的才会飞行
	bool    m_bIsAutoPick;
	//等待掉落时间
	float m_fWaitingTime;
	
	//当模型加载完成后赋值，此后不再变化
	//掉落动画相关
	float	m_fAnimLifeTime;	//掉落时的翻转动画时长
	float	m_fAutoPickWaitTime;	//自动拾取等待时长
	NiPoint3 m_kSrcPos;			//出生坐标
	NiPoint3 m_kDestPos;		//最终坐标

	//根据上述赋值求出，此后不再变化
	float m_fOverturnAngel;			//单位时间内翻转角度
	float m_fZRotAngle;				//绕Z轴的旋转角度，通过m_kSrcPos 和m_kDestPos 计算得出 +_ 出生点方向
	NiPoint3 m_kDeltaPos;			//单位时间内平移变换
	float m_fGravity;				//重力加速
	NiPoint3 m_kDefaultDir;			//初始方向

	//实时计算更新。
	float m_fYRotAngle;				//绕Y轴的旋转角度，m_fOverturnAngel * 时间的累积
	float m_fHeight;				//当前高度的偏移
	NiPoint3 m_kCurPos;				//当前模型所在位置
	float	m_fAnimCurPlayTime;		//当前播放时间
	float	m_fOverturnTime;		//翻滚时长

	float m_fMaxHeight;				//上抛最大高度
	float m_fOverturnHeight;		//翻转高度
	
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

		float fV0;			//飞行初速度
		float fDump1;		//飞行方向速度的阻力（二次项系数）
		float fDump2;		//飞行方向速度的阻力（一次项系数）
		NiPoint3 fV0Dir;	//飞行速度方向
		float fAcce;		//飞行方向加速度
		float fDischan;		//加速度距离影响;
		object_id_type	iFlyToID;		//飞向的目标ID
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

		NiPoint3	target;		//向上飞行目标
		float		fRadius;			//向上飞行左右偏移
		float		fHeigh;				//向上飞行高度偏移
	};

	//飞行动画相关
	float	m_fAnimFlyToLifeTime;	//飞行的时间
	float	m_fAnimCurFlyToTime;	//当前飞行动作时间
	ChestModelState	m_eModelState;	//当前掉落更新的状态
	bool m_bIsFlyAble;				//是否能够飞行
	bool m_bIsDelayFly;				//是否准备飞行
	FLY_PARAM m_stFlyParam;			//飞向人动画参数

	ChestModelFlyType m_flyModelType;	//飞行模型种类
	
	//向上飞行
	float m_fAnimFlyHeighLiftTime;	//飞行高度时间
	float m_fAnimCurFlyHeighTime;	//当前飞行高度时间
	FLY_PARAM_HEIGH m_stFlyHeighParam;		//向上飞行动画参数
};
