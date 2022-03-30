#pragma once
//�����������ݹ���

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

		void	SyncProfitRecord(const std::map<uint16_t,PS_PersonGuildPresentInfo>& info); //��¼ʱ��ͬ��Э�飬��¼δ��ȡ����ʱ���
		bool	AddProfitRecord(const MG_AddGuildPresentInfo& Info); //��Ϸ�����������¼ʱ����������Ƿ�����ͨ����
		bool	AddProfitRecord(const std::map<uint16_t,std::string>& info); //�콱�ظ����ѽ������߼�¼һ�£������Ƿ�����ͨ���������ͬʱ��ȡ�����ͬ���͵�����
		bool	DelProfitRecord(uint16_t id); //������ڣ�ɾ����¼�������Ƿ�����ͨ����
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