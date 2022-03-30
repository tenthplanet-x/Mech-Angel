#ifndef _LC_CLIENT_COLLECTION_MANAGER_H
#define _LC_CLIENT_COLLECTION_MANAGER_H

#include "CF_CollectionActivation.h"
#include "CF_CollectionReward.h"

// ��װ  ����1����װ1�ĵȼ�1
namespace GameLogic 
{
	struct  ActiveLevelInfo
	{
		int  m_iID;     // ��ˮID
		bool m_bAcitve;       // �Ƿ񼤻�
		int m_iPropertyID;      // ��������
		int m_iGrade;               //Ʒ��
		const StringType * m_sActiveIcon;     // δ����ͼ��
		const StringType * m_sInactiveIcon;        // ����ͼ��
		int m_iActiveNeedID;      //�����������ID
		int m_iActiveNeedLevel;     // ������������Ǽ�
		const StringType * m_sName;     // ����
		
		ActiveLevelInfo(int _iID, bool _bAcitve, int _iPropertyID, int _iGrade, const StringType *  _sActiveIcon, 
			const StringType *  _sInactiveIcon, int _iActiveNeedID, int _iActiveNeedLevel, const StringType *  _sName
			)
			: m_iID(_iID)
			, m_bAcitve(_bAcitve)
			, m_iPropertyID(_iPropertyID)
			, m_iGrade(_iGrade)
			, m_sActiveIcon(_sActiveIcon)
			, m_sInactiveIcon(_sInactiveIcon)
			, m_iActiveNeedID(_iActiveNeedID)
			, m_iActiveNeedLevel(_iActiveNeedLevel)
			, m_sName(_sName)
		{
		}
		
	};

	enum COLLECTIONTYPE
	{
		COLLECTION_MONSTER		= 0,
		COLLECTION_EQUIP			= 1,
		COLLECTION_PET					= 2,
		COLLECTION_CHARACTER   = 3,
		COLLECTION_TOY				= 4,
		COLLECTION_RUNE				= 5,

		COLLECTION_WING				= 8,
	};

	enum COLLECTIONCondition
	{
		COLLECTION_StarLevel		= 0,
		COLLECTION_Num				= 1,
	};

	class LC_ClientCollectionInfo : public Memory::MM_BaseObject
	{
	public:
		LC_ClientCollectionInfo();
		~LC_ClientCollectionInfo();
		DECL_RTTI_NAME( LC_ClientCollectionInfo );

	public:
		bool        ConstructDataEntry(CF_CollectionActivation::DataEntry* pkData);



		int  m_iCollectionID;      // ��ID
		int  m_iSuitID;				 // ��װID
		int  m_iPos;					//λ�ã�6����������
		int  m_iType;					//����
		StringType m_sAchievePath;			// ��ȡ;��
		
		typedef mem::vector<ActiveLevelInfo> LC_ClientCollectionData;
		LC_ClientCollectionData m_iActiveLevel;         // key: AcitveLevel  - info

	};

	class LC_ClientCollectionManager : public Utility::UT_Singleton<LC_ClientCollectionManager>,
		public LC_DirtyFlagAble
	{
	public:
		LC_ClientCollectionManager();
		~LC_ClientCollectionManager();
		DECL_RTTI_NAME( LC_ClientCollectionManager )
		

		void								Init();
		void								Unit();
		void								Reset();
		//����������
		void								GetDataFromProtocolStruct(const Protocol::MG_SyncCollectionInfo & rltMsg);
		void								GetDataFromProtocolCollection(const std::vector<int32_t>& kIDs);
		void								GetDataFromProtocolSuit(int32_t SuitiID);
		//���ݽ���
		StringType						GetActiveCollectionInfo(int32_t collectionID);
		int								GetCollectionSuitTotalLevel(int32_t suitiID);    // ��ˮid
		int								GetCollectionMaxLevel(int32_t collectionID);
		StringType						GetCollectionSizeByType(int32_t type);
		StringType						GetPropertyAttriString(long propertyID);
		StringType						GetAllAttributes();
		//��⽻��
		bool								IsCollectionFirstActived(int32_t collectionID);
		bool								IsSuitActived(int32_t suitID);
		bool								CheckCanActive(int32_t suitID);
		bool								CheckCanUpgrad(int32_t suitID);
		bool								CheckUpgrad(int type, int activationID, int activationStarNeed);



		int								GetDirtyFlagRevisionID() { return LC_DirtyFlagAble::GetDirtyFlagRevisionID(); }

	protected:
		
		typedef mem::map<int, LC_ClientCollectionInfo*> LC_ClientCollectionMap;      // key: CollectionID  - AllData;
		typedef mem::map<int, LC_ClientCollectionInfo*>::iterator LC_ClientCollectionMapIter;
		typedef mem::vector<int32_t> LC_ClientCollextionSuitData;
		LC_ClientCollectionMap  m_kCollectionData; 
		LC_ClientCollextionSuitData  m_kCollectionSuitData; 

	};






















































}
#endif