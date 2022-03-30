#pragma once
//战斗数据管理

namespace GameLogic 
{
	struct CombatRecord
	{
		CombatRecord()
		{
			Reset();
		}

		void Reset()
		{
			iDamage = 0;
			iHeal = 0;
			iHurt = 0;
			iCharType = 0;
			iLevel = 0;
		}

		int64_t iDamage;
		int64_t iHeal;
		int64_t iHurt;
		int		iCharType;
		int		iLevel;
	};

	class LC_ClientCombatRecordManager : public Utility::UT_Singleton<LC_ClientCombatRecordManager>,
		public MG_MessageHandlerInterface,
		public Memory::MM_BaseObject
	{
	public:
		LC_ClientCombatRecordManager();
		~LC_ClientCombatRecordManager();
		DECL_RTTI_NAME( LC_ClientCombatRecordManager )

		void Init();
		void Unit();
		void Reset();

		virtual bool RecvMessage(MG_MessageBase& rkMessage);

		StringType GetPlayerCombatInfo();
		StringType GetMonsterCombatInfo();

		int GetSelfMvpActiveIdx(LC_ClientGamePlayer* pkPlayer);

	private:
		typedef mem::map<object_id_type, CombatRecord> CombatRecordMap;
		typedef CombatRecordMap::const_iterator CombatRecordMapConstIter;

		void _event_ReceiveDamage(const MG_MessageBase& rkMessage);
		void _event_DoHeal(const MG_MessageBase& rkMessage);
		StringType _getCombatData(const CombatRecordMap& recordMap, bool bPlayer);

		void _doInitCharType(CombatRecord* pkRecord, object_id_type objID);

		CombatRecordMap m_kPlayerCombatDataMap;
		CombatRecordMap m_kMonsterCombatDataMap;
	};
};