//װ������Ʒ��tooltip������֯

#ifndef _LC_EUIQPITEMTOOLTIP_H
#define _LC_EUIQPITEMTOOLTIP_H

#include "LC_ItemTooltipBase.h"
#include "LC_ClientItem.h"
#include "LC_ClientGamePlayer.h"

namespace GameLogic
{
	struct SortItemEffect
	{  
		int iSortID;
		int iLevel;
		int iIndex;
		int iEffectTypeID;
		//bool bRefine;
		ItemEffect *pkItemEffect;
	}; 

	struct SuitInfo
	{
		int iItemID;
		int iSuitID;
		bool bLight;
	};
	//���ڲ���ָ����TooltipField
	class FindTooltipField
	{
	public:
		FindTooltipField(TooltipFieldType eFieldType, const StringType& kCaption)
		{
			m_eFieldType	= eFieldType;
			m_kCaption		= kCaption;
		}
		bool operator()(LC_TooltipField& kField)
		{
			return (kField.GetType() == m_eFieldType && kField.GetCaption() == m_kCaption);
		}
	private:
		TooltipFieldType	m_eFieldType;
		StringType			m_kCaption;
	};
	//���ڱ������︽�ӵ������ֶ�
	class KeepsakeField
	{
	public:
		enum KeepsakeFieldType
		{
			KFT_LONG,					//���Σ�ֵ���ͣ�
			KFT_FLOAT					//�����ͣ��������ͣ�
		};

		KeepsakeField(const StringType& kCaption, const StringType& kCaptioinDetail, KeepsakeFieldType eFieldType, int lMinVal, int lMaxVal)
		{
			m_kCaption		= kCaption;
			m_kCaptionDetail= kCaptioinDetail;
			m_eFieldType	= eFieldType;
			m_lMaxVal		= lMaxVal;
			m_lMinVal		= lMinVal;
			m_fMaxVal		= 0.0f;
			m_fMinVal		= 0.0f;
		}
		KeepsakeField(float fMinVal, float fMaxVal, const StringType& kCaption, const StringType& kCaptioinDetail, KeepsakeFieldType eFieldType)
		{
			m_kCaption		= kCaption;
			m_kCaptionDetail= kCaptioinDetail;
			m_eFieldType	= eFieldType;
			m_fMaxVal		= fMaxVal;
			m_fMinVal		= fMinVal;
			m_lMaxVal		= 0;
			m_lMinVal		= 0;
		}
		~KeepsakeField(){};

		StringType			m_kCaption;
		StringType			m_kCaptionDetail;
		KeepsakeFieldType	m_eFieldType;
		int				m_lMaxVal;
		int				m_lMinVal;
		float				m_fMaxVal;
		float				m_fMinVal;
	};
	//���������ƶ���KeepsakeField
	class FindKeepsakeField
	{
	public:
		FindKeepsakeField(const StringType& kCaptionDetail)
		{
			m_kCaptionDetail= kCaptionDetail;
		}
		bool operator()(KeepsakeField& kField)
		{
			return (kField.m_kCaptionDetail == m_kCaptionDetail);
		}
	private:
		StringType			m_kCaptionDetail;
	};

	typedef std::map<int, SuitInfo> SuitMap;

	class LC_EquipItemTooltip : public LC_ItemTooltipBase
	{
	public:
		LC_EquipItemTooltip(void);
		virtual ~LC_EquipItemTooltip(void);
		
		void Init(CSVFile::CF_ItemList::DataEntry *pkItemEntry, LC_BackPackEntry* pkBackPackEntry, LC_NPCShopItemEntry* pkShopEntry, LC_PlayerBase* pkObserver, LC_ClientRankUserInfo *pkRankUserInfo = NULL, 
			CSVFile::CF_ItemExistenceFunc::DataEntry *pkExistFunc = NULL, bool bShopOpen = false, bool bGuildWarehouse = false, int iStarry = 0);
		LC_BackPackEntry * GetCurrentEquip(){ return m_pkCurrentEquipBackPackEntry; };
		void SetCurrentEquip(LC_BackPackEntry *pkCurrentEquipBackPackEntry) { m_pkCurrentEquipBackPackEntry = pkCurrentEquipBackPackEntry; }

		virtual bool BuildTooltip();
		bool BuildTooltip(bool bCompareSelfEquip);

		bool BuildEnhanceTooltip();

		bool BuildCompositionTooltip(int iCompositionLevel);

		//��ȡ���������ַ���
		StringType GetItemAttributeFuncDescript(int lAttributeFuncID);

		//��ȡ���������ַ���
		StringType GetItemAttributeFuncDescript(GameLogic::LC_ActorAttributeAppendMap& rkBaseAttributeMap,GameLogic::LC_ActorAttributeAppendMap& rkAddAttributeMap);

	protected:
		//����tooltip����ʾ����װ���ֶ�
		bool _addTooltipFieldSelfEquipHit(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒǿ���ֶ�
		bool _addTooltipFieldQuenchCount(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ�ϳɵȼ��ֶ�
		bool _addTooltipFieldCompositionLevel(TooltipFieldVector& kFieldVector, int iCompositionLevel);
		//����tooltip����Ʒװ���������ֶ�
		bool _addTooltipFieldSubCategory(TooltipFieldVector& kFieldVector);
		//����tooltip����ƷƷ���ֶ�
		bool _addTooltipFieldQuality(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒװ�������ֶ�
		bool _addTooltipFieldEquipAttri(TooltipFieldVector& kFieldVector,int lEnhanceLevel);
		//����tooltip�ļ��������ֶ�
		bool _addTooltipFieldSkill(TooltipFieldVector& kFieldVector);
		//����tooltip�Ļ���ս���������ֶ�
		bool _addTooltipFieldBaseScore(TooltipFieldVector& kFieldVector);
		
		//����tooltip����Ʒװ��Ч���ֶ�
		//bool _addTooltipFieldEquipResult(TooltipFieldVector& kFieldVector);
		//����tooltip�ĸ�ħЧ��
		bool _addTooltipFieldAppendAttributeResult(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ��װ�����ֶ�
		//bool _addTooltipFieldSuiteAttri(TooltipFieldVector& kFieldVector);
		//����tooltip�����ԱȽ��ֶε�����
		bool _addTooltipFieldCompareAttrName(TooltipFieldVector &kFieldVector);
		//����tooltip�����ԱȽ��ֶ�
		bool _addTooltipFieldCompareAttr(TooltipFieldVector &kFieldVector);
		//���쵱ǰս�������ֶ�
		bool _addTooltipFieldCurrentGsAttr(TooltipFieldVector &kFieldVector);
		//����ս�����Ƚ��ֶ�
		bool _addTooltipFieldCompareGs(TooltipFieldVector &kFieldVecotr);
		//����ǿ������ս�������ֶ�
		bool _addTooltipFieldMaxGsAttr(TooltipFieldVector &kFieldVector);
		//����δ������ʾ���ֶ�
		bool _addTooltipFieldUnidentifyTip(TooltipFieldVector &kFieldVector);
		//����װ����ʾ�ֶ�
		bool _addTooltipEquipTip(TooltipFieldVector &kFieldVector);
		
	
		StringType _getAttributeValueString(attr_value_type lBaseMinVal, attr_value_type lBaseMaxVal, attr_value_type lAddMinVal,attr_value_type lAddMaxVal, const StringType& kCaption);
		StringType _getAttributePercentString(attr_value_type lBaseMinVal, attr_value_type lBaseMaxVal, attr_value_type lAddMinVal,attr_value_type lAddMaxVal,const StringType& kCaption);
		StringType _getItemEffectAttrValueString(attr_value_type lBaseMinVal, attr_value_type lCompareVal, const StringType& kCaption, bool bPercent = false);
		attr_value_type _getBattleScore(bool bCurrentEquip, bool bMax = false);
		static bool _sortFunc(const SortItemEffect &v1, const SortItemEffect &v2);

		//����װ����ֵ����
		bool _decodeValueAttri(attr_value_type lMinAttri, attr_value_type lMaxAttri, const StringType& kItemName, StringType& kAttri);
		//����װ���ı�������
		bool _decodePercentAttri(attr_value_type lMinAttri, attr_value_type lMaxAttri, const StringType& kItemName, StringType& kAttri);

		//�ж���װ�Ƿ��Ѿ�װ��
		//bool _suitePartIsEquiped(int lSuitePartID, LC_PlayerBase* pkObserver);
		//bool _suitePartIsEquiped(int lSuitePartID, LC_ClientRankUserInfo *pkUserInfo);
		//bool _suiteAttriAdd(TooltipFieldVector& kFieldVector, int iSuitId, SuitMap &kMap);

		//�����װ���
		//bool _addTooltipFieldSuitePart(int lSuitePartID, const StringType& kSuiteType, bool bSuitePartEquiped, TooltipFieldVector& kFieldVector);
		
		//����װ�����ԵĶ���ո�
		StringType _getAttriFmtGap(const StringType& kItemName);
		//������װ���ԵĶ���ո�
		void _getSuiteAttriFmtGap(const StringType& kItemName, StringType& kPreTextGap, StringType& kFormatTextGap);

		bool m_bCompareSelfEquip;
		LC_BackPackEntry *m_pkCurrentEquipBackPackEntry;
		LC_ClientRankUserInfo *m_pkRankUserInfo;
	};
}

#endif
