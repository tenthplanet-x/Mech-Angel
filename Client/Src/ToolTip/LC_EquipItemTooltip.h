//装备类物品的tooltip数据组织

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
	//用于查找指定的TooltipField
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
	//用于保存信物附加的属性字段
	class KeepsakeField
	{
	public:
		enum KeepsakeFieldType
		{
			KFT_LONG,					//整形（值类型）
			KFT_FLOAT					//浮点型（比例类型）
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
	//用于搜索制定的KeepsakeField
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

		//获取属性描述字符串
		StringType GetItemAttributeFuncDescript(int lAttributeFuncID);

		//获取属性描述字符串
		StringType GetItemAttributeFuncDescript(GameLogic::LC_ActorAttributeAppendMap& rkBaseAttributeMap,GameLogic::LC_ActorAttributeAppendMap& rkAddAttributeMap);

	protected:
		//构造tooltip的提示自身装备字段
		bool _addTooltipFieldSelfEquipHit(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品强化字段
		bool _addTooltipFieldQuenchCount(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品合成等级字段
		bool _addTooltipFieldCompositionLevel(TooltipFieldVector& kFieldVector, int iCompositionLevel);
		//构造tooltip的物品装备子类型字段
		bool _addTooltipFieldSubCategory(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品品质字段
		bool _addTooltipFieldQuality(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品装备属性字段
		bool _addTooltipFieldEquipAttri(TooltipFieldVector& kFieldVector,int lEnhanceLevel);
		//构造tooltip的技能属性字段
		bool _addTooltipFieldSkill(TooltipFieldVector& kFieldVector);
		//构造tooltip的基础战斗力属性字段
		bool _addTooltipFieldBaseScore(TooltipFieldVector& kFieldVector);
		
		//构造tooltip的物品装备效果字段
		//bool _addTooltipFieldEquipResult(TooltipFieldVector& kFieldVector);
		//构造tooltip的附魔效果
		bool _addTooltipFieldAppendAttributeResult(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品套装属性字段
		//bool _addTooltipFieldSuiteAttri(TooltipFieldVector& kFieldVector);
		//构造tooltip的属性比较字段的名称
		bool _addTooltipFieldCompareAttrName(TooltipFieldVector &kFieldVector);
		//构造tooltip的属性比较字段
		bool _addTooltipFieldCompareAttr(TooltipFieldVector &kFieldVector);
		//构造当前战斗力的字段
		bool _addTooltipFieldCurrentGsAttr(TooltipFieldVector &kFieldVector);
		//构造战斗力比较字段
		bool _addTooltipFieldCompareGs(TooltipFieldVector &kFieldVecotr);
		//构造强化顶级战斗力的字段
		bool _addTooltipFieldMaxGsAttr(TooltipFieldVector &kFieldVector);
		//构造未鉴定提示的字段
		bool _addTooltipFieldUnidentifyTip(TooltipFieldVector &kFieldVector);
		//构造装备提示字段
		bool _addTooltipEquipTip(TooltipFieldVector &kFieldVector);
		
	
		StringType _getAttributeValueString(attr_value_type lBaseMinVal, attr_value_type lBaseMaxVal, attr_value_type lAddMinVal,attr_value_type lAddMaxVal, const StringType& kCaption);
		StringType _getAttributePercentString(attr_value_type lBaseMinVal, attr_value_type lBaseMaxVal, attr_value_type lAddMinVal,attr_value_type lAddMaxVal,const StringType& kCaption);
		StringType _getItemEffectAttrValueString(attr_value_type lBaseMinVal, attr_value_type lCompareVal, const StringType& kCaption, bool bPercent = false);
		attr_value_type _getBattleScore(bool bCurrentEquip, bool bMax = false);
		static bool _sortFunc(const SortItemEffect &v1, const SortItemEffect &v2);

		//解析装备的值属性
		bool _decodeValueAttri(attr_value_type lMinAttri, attr_value_type lMaxAttri, const StringType& kItemName, StringType& kAttri);
		//解析装备的比率属性
		bool _decodePercentAttri(attr_value_type lMinAttri, attr_value_type lMaxAttri, const StringType& kItemName, StringType& kAttri);

		//判断套装是否已经装备
		//bool _suitePartIsEquiped(int lSuitePartID, LC_PlayerBase* pkObserver);
		//bool _suitePartIsEquiped(int lSuitePartID, LC_ClientRankUserInfo *pkUserInfo);
		//bool _suiteAttriAdd(TooltipFieldVector& kFieldVector, int iSuitId, SuitMap &kMap);

		//添加套装配件
		//bool _addTooltipFieldSuitePart(int lSuitePartID, const StringType& kSuiteType, bool bSuitePartEquiped, TooltipFieldVector& kFieldVector);
		
		//补齐装备属性的对齐空格
		StringType _getAttriFmtGap(const StringType& kItemName);
		//补齐套装属性的对齐空格
		void _getSuiteAttriFmtGap(const StringType& kItemName, StringType& kPreTextGap, StringType& kFormatTextGap);

		bool m_bCompareSelfEquip;
		LC_BackPackEntry *m_pkCurrentEquipBackPackEntry;
		LC_ClientRankUserInfo *m_pkRankUserInfo;
	};
}

#endif
