#pragma once

#include "BS_IMInterface.h"
#include "GD_ModelBase.h"
#include "MG_MessageHandlerInterface.h"
#include "GD_ClientBreakModel.h"

class BM_Manager :public Memory::MM_BaseObject,
				  public MG_MessageHandlerInterface,
				  public Utility::UT_Singleton<BM_Manager>
{
public:
	BM_Manager();
	virtual ~BM_Manager();

	void Init();
	void UnInit();
	void ReSet();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	void Update(float fCurrentTime, float fDeltaTime);

	void Begin(long lCharType, object_id_type lLogicID, const NiPoint3& kPos, const Utility::UT_Vec2Float& kDir, object_id_type lAttackerID, int lAttackSkillType);
private:
	struct ModelInfo
	{
		ModelInfo()
		{
			Reset();
		}

		void Reset()
		{
			m_Scene = NULL;
			m_iHeartIndex = -1;
			m_iEndNum	= 0;
			m_fBeginTime = 0.0f;
			m_ImpulseDir = NiPoint3::ZERO;
			m_AttackSkillType = 0;
			m_fMinWeight = 0.0f;
			m_vSpeeds.clear();
			m_vEndList.clear();
			m_vAxisDirection.clear();
			m_vRotatingSpeed.clear();
			m_vSkipTimesList.clear();
		}

		GD_ClientBreakModel*	m_Scene;
		int						m_iDeadFx;
		int						m_iHeartIndex;
		int						m_iEndNum;
		float					m_fBeginTime;
		NiPoint3				m_ImpulseDir;
		int						m_AttackSkillType;
		float					m_fMinWeight;
		std::vector<NiPoint3>	m_vSpeeds;
		std::vector<bool>		m_vEndList;
		std::vector<NiPoint3>	m_vAxisDirection;
		std::vector<float>		m_vRotatingSpeed;
		std::vector<float>		m_vRotating;
		std::vector<int>		m_vSkipTimesList;
		std::vector<int>		m_vFxIdList;
	};
	mem::map<object_id_type, ModelInfo> m_mActors;


	void _eventTimerTick(MG_MessageBase& rkMessage);
	void _updateFragmentMove(object_id_type lLogicID, ModelInfo& Info, float fCurrentTime, float fDeltaTime);
	void _releaseFxAll(const ModelInfo& mMode);
	float _getWeight(float fMinWeight, float fWeight);
};