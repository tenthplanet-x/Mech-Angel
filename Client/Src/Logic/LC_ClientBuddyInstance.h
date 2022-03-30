#ifndef _LC_CLIENT_BUDDY_INSTANCE_H
#define _LC_CLIENT_BUDDY_INSTANCE_H
#include "LC_PackAsset.h"
#include "MG_BuddyProtocol.h"
#include "LC_ClientShortcutBar.h"
#include "LC_ClientGamePlayerCommander.h"
#include "CF_Buddy.h"

namespace GameLogic
{
	class LC_ClientBuddyInstance : public Memory::MM_BaseObject
	{
	public:
		LC_ClientBuddyInstance();
		virtual ~LC_ClientBuddyInstance();
		DECL_RTTI_NAME( LC_ClientBuddyInstance );

		void							Reuse();
		void							Init();
		void							ParseProtocol(BuddyData_Base& buddy);
		LC_SkillAsset*					GetSkillAsset() { return &m_kSkillAsset; }
		LC_ActorAttributeMap*			GetAttributeMap() { return &m_kAttributeMap; };
		LC_ActorAttributeAppendMap*		GetAttributeAppendMap() { return &m_kAttributePlus; };
		LC_ShortcutBar*					GetShortcutBar() { return m_pkShortcurtBar; }
		void							InitAttributeMap();
		void                            InitCombate(long hp, int64_t def, long ack
													, long ackRate, long dog, long cir
													, long cirDef);

		int								GetLevel() const { return iLevel; }
		void							SetLevel(uint16_t val) { iLevel = val; }
		int								GetEXP() const { return iExp; }
		void							SetExp(int32_t val) { iExp = val; }
		int64_t							GetHP() const { return iHP; }
		void							SetHP(int64_t val) { iHP = val; }
		void							SetCombateScore(combat_score_type val) { iCombateScore = val; }
		combat_score_type				GetCombateScore() const { return iCombateScore; }
		int								GetStarLevel() const { return iStarLevel; }
		void							SetStarLevel(int32_t val) { iStarLevel = val; }
		int								GetCID() const { return iCID; }
		void							SetCID(uint16_t val) { iCID = val; }
		bool							GetInitial() { return m_initial; }
		void							SetInitial(bool flag) { m_initial = flag; }

		void							SetEquipShowLevel(int iLevel) { iEquipShowLevel = iLevel; }
		int								GetEquipShowLevel() { return iEquipShowLevel ; }
		void							SetBuddyRank(int iRank) { m_iRank = iRank; }
		int								GetBuddyRank() { return m_iRank ; }
		void							SetCurrentTransformID(int iID) {m_iCurrentTransformID = iID;}
		int								GetCurrentTransformID() {return m_iCurrentTransformID;}
		bool							SetActiveTransformRecord(std::vector<int32_t> m_vRecord); //�����Ѽ����Ƥ���б������Ƿ��б仯
		bool							GetIsActiveTransform(int iID);
		bool							SetActiveTransformInvaildTime(std::vector<BuddyTransformInvaildTime> m_vTime);
		int32_t							GetActiveTransformTime(int iID);

		void							SetBeEntrusted(bool bValue){m_bEntrusted=bValue;};
		bool							GetBeEntrusted(){return m_bEntrusted;};
		
		void							UpdateRuneEquip(int iIndex, const PS_ItemAssetEntry& assetEntry);
		void							ClearRuneEquipEntry(int iIndex);
		LC_BackPackEntry*				GetRuneEquipEntry(int iIndex) { return &m_vRuneEquipList[iIndex]; }


		void							UpdateEquip(int iIndex, const Equip_Base& baseInfo);
		void							UpdateEquip(int iIndex, const PS_ItemAssetEntry& assetEntry);
		void							UpdateEquipLevel(int iIndex, int iLevel);
		void							UpdateEquipStarLevel(int iIndex, int iStarLevel);
		void							ClearEquipEntry(int iIndex);
		LC_BackPackEntry*				GetEquipEntry(int iIndex);
		int								GetEquipIntensifyLevel(int iIndex);
		int								GetEquipRefineLevel(int iIndex);

	private:
		uint16_t						iCID;
		uint16_t						iLevel;
		int32_t							iExp;
		int64_t							iHP;
		combat_score_type				iCombateScore;
		int32_t							iStarLevel;
		int32_t							iEquipShowLevel;			//װ������ʾ�ȼ�
		LC_SkillAsset		  			m_kSkillAsset;
		LC_ShortcutBar*					m_pkShortcurtBar;
		LC_ActorAttributeMap			m_kAttributeMap;
		LC_ActorAttributeAppendMap		m_kAttributePlus;

		bool							m_initial;
		bool							m_bEntrusted;

		int32_t							m_iRank;				//Ӣ������
		int32_t							m_iCurrentTransformID;  //Ƥ��id
		mem::set<int32_t>				m_vActiveTransformRecord; //�Ѽ����Ƥ��
		mem::map<int32_t, int32_t>		m_mActiveTransformInvaildTime; //��ʱƤ���ĵ���ʱ��
		
		EquipVec						m_vRuneEquipList;

		struct EquipInfo
		{
			EquipInfo()
			{
				Reset();
			}

			void Reset()
			{
				itemEntry.Reset();
				intensifyLevel = 0;
				refineLevel = 0;
			}

			LC_BackPackEntry itemEntry;
			int intensifyLevel;		//��̵ȼ�
			int refineLevel;		//�����ȼ�
		};
		mem::vector<EquipInfo>			m_vEquipList;
	};
}
#endif