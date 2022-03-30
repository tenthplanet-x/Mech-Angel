#pragma once
//公会礼物数据管理

namespace GameLogic 
{
	struct ProfitInfo
	{
		ProfitInfo()
		{
			Reset();
		}

		void Reset()
		{
			m_iProfitID = 0;
			m_iItemID	= 0;
			m_sCharName = "";
			m_iPresentType = 0;
			m_iInvaildTime = 0;
			m_bIsDrawed = false;
			m_sRewardList  = "";
		}

		uint16_t	m_iProfitID;
		int32_t		m_iItemID;
		StringType	m_sCharName;
		int8_t		m_iPresentType;
		int32_t		m_iInvaildTime;
		bool		m_bIsDrawed;
		StringType	m_sRewardList;
	};

	class LC_ClientGuildProfitManager : public Utility::UT_Singleton<LC_ClientGuildProfitManager>,
		public MG_MessageHandlerInterface,
		public Memory::MM_BaseObject
	{
	public:
		LC_ClientGuildProfitManager();
		~LC_ClientGuildProfitManager();
		DECL_RTTI_NAME( LC_ClientGuildProfitManager )

		void Init();
		void Unit();
		void Reset();
		void ReUse();

		void	InitData(const StringType& str);

		uint16_t	GetProfitIDByKey(int key, bool bNormal);
		int32_t		GetItemIDByKey(int key, bool bNormal);
		StringType	GetCharNameByKey(int key, bool bNormal);
		int			GetPresentTypeByKey(int key, bool bNormal);
		int32_t		GetInvaildTimeByKey(int key, bool bNormal);
		bool		GetProfitIsDrawedByKey(int key, bool bNormal);
		StringType	GetProfitRecordByKey(int key, bool bNormal);

		void	SyncProfitRecord(const std::map<uint16_t,PS_PersonGuildPresentInfo>& info); //登录时的同步协议，记录未领取礼物时间戳
		bool	AddProfitRecord(const MG_AddGuildPresentInfo& Info); //游戏中新增礼物，记录时间戳，返回是否是普通礼物
		bool	AddProfitRecord(const std::map<uint16_t,std::string>& info); //领奖回复，把奖励道具记录一下，返回是否是普通礼物，不存在同时领取多个不同类型的礼物
		bool	DelProfitRecord(uint16_t id); //礼物过期，删除记录，返回是否是普通礼物
		void	SortProfitRecord(bool bNormal, bool bRare);

		mem::vector<ProfitInfo*>&	GetNormalList(){ return m_vProfitNormalList;};

		int		GetNoDrawedCount(bool bNormal);

		void LoadFile(unique_id_type uiID);
		void SaveFile();

	private:
		StringType _getSaveString();
		void	SetRecord(mem::map<uint16_t, ProfitInfo>& list, const StringType& str);
		static bool _sortGuildProfit(const ProfitInfo* v1, const ProfitInfo* v2);

		mem::map<uint16_t, ProfitInfo> m_mDetailNormalMap;
		mem::vector<ProfitInfo*> m_vProfitNormalList;
		int		NoDrawedNormalProfit;
		mem::map<uint16_t, ProfitInfo> m_mDetailRareMap;
		mem::vector<ProfitInfo*> m_vProfitRareList;
		int		NoDrawedRareProfit;
		unique_id_type m_uiID;
	};
};