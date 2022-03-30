#ifndef _LC_CLIENT_GAME_PLAYER_COMMANDER_H
#define _LC_CLIENT_GAME_PLAYER_COMMANDER_H
#include "MG_SyncProtocol.h"

namespace GameLogic
{
	typedef mem::map<int, mem::map<int, int>> EquipModingSpiritLvMap;
	typedef mem::vector<LC_BackPackEntry> EquipVec;

	class LC_ClientGamePlayerCommander : public LC_DirtyFlagAble
	{
	public:
		LC_ClientGamePlayerCommander();
		~LC_ClientGamePlayerCommander();

		void							Reuse();
		void							Reset();
		void							InitEquipList();
		void							ParseProtocol(const MG_SyncPlayerControllerProperty & playerProperty);
		void							UpdatePropertys(const MG_SyncPlayerControllerProperty & playerProperty);
		bool							CheckEXP(int64_t val);
		bool							CheckLevel(uint16_t val);
		bool							CheckTotalCombateScore(int64_t val);
		bool							isInited();
		void							SetMainControllerID(object_id_type id) { m_nMainControllerId = id; };
		object_id_type					GetMainControllerID() const { return m_nMainControllerId; }
		unique_id_type					GetUID() const { return m_nInstanceId; }
		void							SetCurMP(int32_t curMP) { m_iCommanderMP = curMP; }
		void							SetMaxMP(int32_t maxMP) { m_iCommanderMaxMP = maxMP; }
		int32_t							GetCurMP() { return m_iCommanderMP; }
		int32_t							GetMaxMP() { return m_iCommanderMaxMP; }
		void							SetTaskHonorPoint(uint32_t val) { m_iTaskHonorPoint = val; }
		uint32_t						GetTaskHonorPoint() { return m_iTaskHonorPoint; }
		void							SetMainTaskFinishCount(uint32_t val) { m_iMainTaskFinishCount = val; }
		uint32_t						GetMainTaskFinishCount() { return m_iMainTaskFinishCount; }
		void							SetLotteryCount(int32_t iCount) { m_iLotteryCount = iCount; }
		int32_t							GetLotteryCount() { return m_iLotteryCount; }
		int32_t							GetMaxRuneStarLevel() { return m_iMaxRuneStarLevel; }
		int32_t							GetMaxRuneLevel() { return m_iMaxRuneLevel; }
		LC_ActorAttributeMap*			GetAttributeMap() { return &m_kAttributeMap; };
		LC_ActorAttributeMap*			GetChainAttributeMap() { return &m_kChainAttributeMap; };
		LC_ActorAttributeAppendMap*		GetAttributeAppendMap() { return &m_kAttributePlus; };
		void							SetPlayerCommanderName(const StringType& InName);
		int								GetEquipReinforceLevel(int iEquipType);
		int								GetEquipRefineLevel(int iEquipType);
		int								GetEquipModingSpiritLevel(int iPackType, int iEquipType);
		void							SetEquipSuitLevel(int32_t iLevel) { m_iEquipSuitLevel = iLevel; }
		int32_t							GetEquipSuitLevel() { return m_iEquipSuitLevel; }
		void							GetSubMoldingSpiritSuitInfo(mem::map<int, int>& suitInfo, int iPackType);
		void							GenSubEquipSlotMoldingSpiritSuitAttr();
		EquipModingSpiritLvMap& GetSubEquipSlotMoldingSpiritPercentMap() { return m_mSubSuitAttrPercent; }
		EquipModingSpiritLvMap& GetSubEquipSuitMoldingSpiritValueMap() { return m_mSubSuitAttrValue; }
		EquipModingSpiritLvMap& GetSubEquipMoldingSpiritValueMap() { return m_mSubAttrValue; }

		//新装备协议相关
		void							UpdateEquipList(const MG_SC_SynCommandEquip_Ret& msg);
		void							UpdateCommanderEquip(const MG_SC_EquipCommand_Ret& msg);
		void							UpdateCommanderEquip(const MG_SC_IntensifyCommandEquip_Ret& msg);
		void							UpdateCommanderEquip(const MG_SC_RefineCommandEquip_Ret& msg);
		void							UpdateCommanderEquip(const MG_SC_SoulCommandEquip_Ret& msg);
		
		void							UpdateSubEquip(const MG_Rlt_EquipSubItem& msg);
		void							UpdateSubEquip(const MG_Rlt_UnEquipSubItem& msg);
		void							UpdateSubEquip(const MG_RLT_SubEquipSlotMoldingSpirit& msg);

		EquipVec&						GetEquipList(int iPackType) { return m_vEquipList[iPackType-1]; }
		LC_BackPackEntry*				GetEquipEntry(int iPackType, int iIndex);
		bool							IsExistSubEquip(int iItemID);

		void							UpdateCommanderEquipScore(int64_t iValue){m_iCommanderEquipScore=iValue;};
		combat_score_type				GetCommanderEquipScore(){return m_iCommanderEquipScore;};
	public:
		int64_t                         m_iEXP;
		uint16_t                        m_iLevel;
		int64_t							m_iTotalCombateScore;
		int64_t							m_iChainCombateScore;
		combat_score_type				m_iCommanderEquipScore;
		StringType						m_cPlayerControllerName;
		object_id_type					m_nMainControllerId;
		unique_id_type					m_nInstanceId;
		int32_t							m_iBornCharType;
		int32_t							m_iCommanderTitleLevel;
		int32_t							m_iSpecialTraningLevel;
		int32_t							m_nCommanderTitleFrameID;
		int32_t							m_iCharCreateTime;
	private:
		int								_getPackTypeBySubType(int iPackType);
		int								_getSubTypeByPackType(int iPackType);

		int32_t							m_iCommanderMP;
		int32_t							m_iCommanderMaxMP;
		uint32_t						m_iTaskHonorPoint;
		uint32_t						m_iMainTaskFinishCount;
		int32_t							m_iLotteryCount;
		int32_t							m_iMaxRuneStarLevel;
		int32_t							m_iMaxRuneLevel;
		int32_t							m_iEquipSuitLevel;

		mem::map<int, int>				m_mEquipReinforceLevels;
		mem::map<int, int>				m_mEquipRefineLevels;
		mem::vector<EquipVec>			m_vEquipList;
		
		EquipModingSpiritLvMap m_mEquipModingSpiritLevels;
		EquipModingSpiritLvMap m_mSubSuitAttrPercent;
		EquipModingSpiritLvMap m_mSubSuitAttrValue;
		EquipModingSpiritLvMap m_mSubAttrValue;

		LC_ActorAttributeMap			m_kAttributeMap;
		LC_ActorAttributeMap			m_kChainAttributeMap;
		LC_ActorAttributeAppendMap		m_kAttributePlus;
	};
}
#endif
