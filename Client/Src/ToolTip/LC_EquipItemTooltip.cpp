
#include "ClientPCH.h"
#include "LC_EquipItemTooltip.h"
#include "LM_LocaleManager.h"
#include "CF_ItemList.h"
#include "CF_ItemEffect.h"
#include "CF_ItemEffectFunc.h"


#include "LC_PlayerBase.h"
#include "CF_ItemCategory.h"
#include "CF_ItemEquipLimitFunc.h"
#include "CF_ItemGen.h"
#include "CF_ItemSuit.h"
#include "CF_SocketItemFunc.h"
#include "CF_Helper.h"
#include "CF_ItemGenEffect.h"
#include "CF_ItemAttrActive.h"
#include "LC_ItemSuitManager.h"
#include "LC_Helper.h"

using namespace Utility;
using namespace GameLogic;
using namespace CSVFile;
using namespace std;

LC_EquipItemTooltip::LC_EquipItemTooltip(void)
: m_pkRankUserInfo(NULL)
, m_bCompareSelfEquip(false)
, m_pkCurrentEquipBackPackEntry(NULL)
{
}

LC_EquipItemTooltip::~LC_EquipItemTooltip(void)
{
}

void LC_EquipItemTooltip::Init(CSVFile::CF_ItemList::DataEntry *pkItemEntry, LC_BackPackEntry* pkBackPackEntry, LC_NPCShopItemEntry* pkShopEntry,
							   LC_PlayerBase* pkObserver, LC_ClientRankUserInfo *pkRankUserInfo, CSVFile::CF_ItemExistenceFunc::DataEntry *pkExistFunc,
							   bool bShopOpen, bool bGuildWarehouse, int iStarry)
{
	LC_ItemTooltipBase::Init(pkItemEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse, iStarry);
	m_pkRankUserInfo = pkRankUserInfo;
}

bool LC_EquipItemTooltip::BuildTooltip()
{
	return BuildTooltip(false);
}

bool LC_EquipItemTooltip::BuildTooltip(bool bCompareSelfEquip)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	m_bCompareSelfEquip = bCompareSelfEquip;
	//������е�tooltip�ֶ�
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//���������װ�����бȽ�
	if (bCompareSelfEquip)
	{
		_addTooltipFieldSelfEquipHit(kFieldVector);
	}

	//��Ʒ����
	_addTooltipFieldName(kFieldVector);

	//��ƷƷ��
	//_addTooltipFieldQuality(kFieldVector);

	//��Ʒ������
	_addTooltipFieldSubCategory(kFieldVector);

	//if(NULL != pkBackPackEntry)
	//{
		//��ǰս����
		//_addTooltipFieldCurrentGsAttr(kFieldVector);
		//ǿ������ս����
		//_addTooltipFieldMaxGsAttr(kFieldVector);
		//�Ƚ�ǿ������ս����
		/*if(!bCompareSelfEquip && NULL != pkCurEquipEntry)
		{
			_addTooltipFieldCompareGs(kFieldVector);
		}*/
	//}

	//��������
	//addTooltipFieldSchoolLimit(kFieldVector);
	
	//�Ա�����
	//_addTooltipFieldSexLimit(kFieldVector);

	////��Ʒ�ȼ�
	//_addTooltipFieldLevel(kFieldVector);

	//ǿ������
	//_addTooltipFieldQuenchCount(kFieldVector);

	//�ɷֽ�
	//_addTooltipFieldDecomposition(kFieldVector);

	//��Ʒӵ��Ψһ
	//_addToolTipFieldOwnUnique(kFieldVector);

	//��Ʒ��Դ
	//_addTooltipFieldOrigin(kFieldVector);

	//δ��������
	//_addTooltipFieldUnidentifyTip(kFieldVector);

	//װ������
	_addTooltipFieldEquipAttri(kFieldVector, 0);

	//��ǰս����
	_addTooltipFieldCurrentGsAttr(kFieldVector);

	//װ������
	//_addTooltipFieldSkill(kFieldVector);
	
	//װ��Ч��
	//_addTooltipFieldEquipResult(kFieldVector);

	//ʹ��Ч��
	//_addTooltipFieldUseResultTriggerSkill(kFieldVector);

	//��װ����
	//_addTooltipFieldSuiteAttri(kFieldVector);

	//���ԱȽ�
	//ֻ���ڷ�װ�����鿴����ʾ���Բ���
	if (!bCompareSelfEquip && NULL != GetCurrentEquip())
	{
		_addTooltipFieldCompareAttr(kFieldVector);
	}
	
	//�ָ���
	//_addTooltipFieldCompareAttrSplitLine(kFieldVector);

	//��Ʒ����
	//_addTooltipFieldDescript(kFieldVector);

	//ʱ������
	//_addTooltipFieldTimeLimitDescript(kFieldVector);
	//�ȼ�����
	_addTooltipFieldLevelLimit<CF_ItemEquipLimitFunc::DataEntry>(kFieldVector);
	
	//������
	_addTooltipFieldBind(kFieldVector);

	//���ۼ۸�
	_addTooltipFieldPrice(kFieldVector);
	//���ṱ��
	_addTooltipFieldGuildCash(kFieldVector);

	//if(m_bShopOpen)
	//{
	//	//������ʾ
	//	_addTooltipSellTip(kFieldVector);
	//}
	//else if(!bCompareSelfEquip)
	//{
	//	//װ����ʾ
	//	_addTooltipEquipTip(kFieldVector);
	//}

	//���͵�����Ϣ
	//_addTooltipFieldAddItemLinkInfo(kFieldVector);

	//����ս����
	_addTooltipFieldBaseScore(kFieldVector);

	//��Χ����
	_addTooltipFieldAtmosDescript(kFieldVector);

	//��ʽ��tooltip
	_formatTooltip(kFieldVector);

	return true;
}

bool LC_EquipItemTooltip::BuildCompositionTooltip(int iCompositionLevel)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (pkItemData == 0)
	{
		return false;
	}

	//������е�tooltip�ֶ�
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//��Ʒ����
	_addTooltipFieldName(kFieldVector);

	//��Ʒ������
	_addTooltipFieldSubCategory(kFieldVector);

	//װ����������ֶ�
	//�ȼ�����
	_addTooltipFieldLevelLimit<CF_ItemEquipLimitFunc::DataEntry>(kFieldVector);

	//�Ա�����
	_addTooltipFieldSexLimit(kFieldVector);

	////��Ʒ�ȼ�
	//_addTooltipFieldLevel(kFieldVector);

	//������
	_addTooltipFieldBind(kFieldVector);

	//�ɷֽ�
	//_addTooltipFieldDecomposition(kFieldVector);

	//�ϳɵȼ�
	//_addTooltipFieldCompositionLevel(kFieldVector, iCompositionLevel);

	//��Ʒӵ��Ψһ
	_addToolTipFieldOwnUnique(kFieldVector);

	//�����޸ģ�ȡ��Ʒ�ʵ���ʾ
	////��ƷƷ��
	//_addTooltipFieldQuality(kFieldVector);

	//��Ʒ��Դ
	//_addTooltipFieldOrigin(kFieldVector);

	//װ������
	_addTooltipFieldEquipAttri(kFieldVector,iCompositionLevel);

	//װ��Ч��
	//_addTooltipFieldEquipResult(kFieldVector);

	//ʹ��Ч��
	//_addTooltipFieldUseResultTriggerSkill(kFieldVector);

	//��װ����
	//_addTooltipFieldSuiteAttri(kFieldVector);

	//��Ʒ����
	_addTooltipFieldDescript(kFieldVector);

	//��Χ����
	_addTooltipFieldAtmosDescript(kFieldVector);

	//���ۼ۸�
	_addTooltipFieldPrice(kFieldVector);
	//���ṱ��
	_addTooltipFieldGuildCash(kFieldVector);

	//ʱ������
	_addTooltipFieldTimeLimitDescript(kFieldVector);
	//��ʽ��tooltip
	_formatTooltip(kFieldVector);

	return true;
}

bool LC_EquipItemTooltip::BuildEnhanceTooltip()
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}

	LC_BackPackEntry* pkBackPackEntry = GetBackPackEntry();
	if(NULL == pkBackPackEntry)
		return false;

	//������е�tooltip�ֶ�
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//װ������
	_addTooltipFieldEquipAttri(kFieldVector,0);

	return true;
}

bool LC_EquipItemTooltip::_addTooltipFieldSelfEquipHit(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLDENROD;

	StringType kContent = GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_001);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SELF_EQUIP_HIT, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_002), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_EquipItemTooltip::_addTooltipFieldQuenchCount( TooltipFieldVector& kFieldVector )
{
	return false;
}

bool LC_EquipItemTooltip::_addTooltipFieldCompositionLevel(TooltipFieldVector& kFieldVector, int iCompositionLevel)
{
	return false;
}

bool LC_EquipItemTooltip::_addTooltipFieldSubCategory( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringType kContent = "";
	CF_ItemEquipLimitFunc* pkItemEquip = CF_ItemEquipLimitFunc::GetSingletonPtr();
	CF_ItemEquipLimitFunc::DataEntry* pkItemEquipData = pkItemEquip->GetEntryPtr( pkItemData->_lEquipLimitFuncID );
	if (NULL == pkItemEquipData)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemEquipLimitFunc.csv key==%ld  is not found!", pkItemData->_lEquipLimitFuncID );
	}

	CF_ItemCategory* pkItemCategoryList = CF_ItemCategory::GetSingletonPtr();
	if (NULL != pkItemCategoryList)
	{
		CF_ItemCategory::DataEntry* pkItemCategoryData = pkItemCategoryList->GetEntryPtr(pkItemData->_lCategory2);
		if (NULL != pkItemCategoryData && "" != pkItemCategoryData->_kName)
		{
			kContent = pkItemCategoryData->_kName;
		}
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SUBCATEGORY, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_003), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_EquipItemTooltip::_addTooltipFieldQuality( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GetGradeColor(pkItemData->_lGrade);
	bool bNewLine = false;

	int lQuality = pkItemData->_lGrade;
	//if (lQuality <= 0)
	//{
	//	return false;
	//}

	StringStreamType kStringStream;
	switch (lQuality)
	{
	case ITEM_GRADE_WHITE:
		kStringStream << GET_MSG(LC_HELPER_CPP_012);
		break;
	case ITEM_GRADE_BLUE:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_004);
		break;
	case ITEM_GRADE_GOLDEN:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_005);
		break;
	case ITEM_GRADE_DARKGOLDEN:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_006);
		break;
	case ITEM_GRADE_GREEN:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_007);
		break;
	case ITEM_GRADE_RED:
	case ITEM_GRADE_RED1:
	case ITEM_GRADE_RED2:
	case ITEM_GRADE_RED3:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_008);
		break;
	case ITEM_GRADE_GOLD:
	case ITEM_GRADE_GOLD1:
	case ITEM_GRADE_GOLD2:
	case ITEM_GRADE_GOLD3:
	case ITEM_GRADE_GOLD4:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_009);
		break;
	case ITEM_GRADE_PINK:
	case ITEM_GRADE_PINK1:
	case ITEM_GRADE_PINK2:
	case ITEM_GRADE_PINK3:
	case ITEM_GRADE_PINK4:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_080);
		break;
	case ITEM_GRADE_COLORFUL:
	case ITEM_GRADE_COLORFUL1:
	case ITEM_GRADE_COLORFUL2:
	case ITEM_GRADE_COLORFUL3:
	case ITEM_GRADE_COLORFUL4:
		kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_081);
		break;
	default:
		kStringStream << GET_MSG(LC_HELPER_CPP_012);
		break;
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_QUALITY, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_010), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine));

	return true;
}

bool LC_EquipItemTooltip::_addTooltipFieldEquipAttri(TooltipFieldVector& kFieldVector,int lEnhanceLevel)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
	{
		return false;
	}

	LC_ItemBase *pkItem = pkItemFactory->RequestItem(pkItemData->_iID);
	if(NULL == pkItem)
	{
		return false;
	}

	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = BROWN2;

	ItemEffectManage *pkItemEffectMgr = ItemEffectManage::GetSingletonPtr();
	LC_BackPackEntry* pkBackPackEntry = GetBackPackEntry();

	//ǿ��״̬

	/*int iStarry = 0;
	int iGloomy = 0;
	LC_BackPackEntry* pkBackPackEntry = GetBackPackEntry();
	if (NULL != pkBackPackEntry)
	{
		iStarry = pkBackPackEntry->GetReinforceStarry();
		iGloomy = pkBackPackEntry->GetReinforceGloomy();
		
	}
	else if(m_iStarry > 0)
	{
		iStarry = m_iStarry;
	}

	if(iStarry > 0 && iGloomy > 0)
	{
		StringType ReinforceStr = Int2String(iStarry) + "#" + Int2String(iGloomy);
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_REINFORCE, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_011), true, ReinforceStr, usFont, usFontSize, iFontColor, 0, 0) );
	}*/
	
	//��������
	//StringType sPre = "<color val=0xffffffff><image val=16>Common1/Common1_image_Point</image></color>";
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_ATTRI, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_020), true, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_020), usFont, usFontSize, iFontColor, 0, 0) );	
	
	/*bool b = (pkItemData->_lCategory2 == ITEM_CATEGORY_EQUIP_ATK_MEDAL || pkItemData->_lCategory2 == ITEM_CATEGORY_EQUIP_DEF_MEDAL || pkItemData->_lCategory2 == ITEM_CATEGORY_EQUIP_HP_MEDAL
		|| pkItemData->_lCategory2 == ITEM_CATEGORY_EQUIP_ATK_BADGE || pkItemData->_lCategory2 == ITEM_CATEGORY_EQUIP_DEF_BADGE || pkItemData->_lCategory2 == ITEM_CATEGORY_EQUIP_HP_BADGE);


	int iCount = GetGradeAttribNum(pkItemData->_lGrade);
	if(!b && (NULL == pkBackPackEntry || ITEM_IDENTIFIED != pkBackPackEntry->GetIdentify()))
	{
		if(b)
			kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_013), usFont, usFontSize, RED, 0, 0, true));
		else
			kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_014), usFont, usFontSize, RED, 0, 0, true));

		for(int i=0; i<iCount; ++i)
		{
			if(b)
				kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sPre + GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_015), usFont, usFontSize, BLUE, 0, 0, true));
			else
			{
				kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sPre + GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_016), usFont, usFontSize, BLUE, 0, 0, true));
			}
		}
	}
	//else
	//{
	//	//int iRefineIdx = LC_ClientItem::genItemModifyAttribIndex(pkBackPackEntry);
	//	iFontColor = BLUE;		
	//	std::vector<SortItemEffect> kVector;
	//	//�������		
	//	if(!b)
	//	{
	//		//��ħ����
	//		int iEnchant = pkBackPackEntry->GetEnchant();
	//		if(pkItemGen)
	//		{
	//			Utility::UT_SSIMDataListInt16& kEnchantEffect = pkItemGen->EnchantEffect;
	//			int32_t iIndex = -1;
	//			for (int32_t i = 0; i < kEnchantEffect.size(); ++i)
	//			{
	//				if (iEnchant >= kEnchantEffect[i].IID())
	//				{
	//					iIndex = i;
	//				}
	//				else
	//				{
	//					break;
	//				}
	//			}
	//			if (iIndex >= 0)
	//			{
	//				UT_SIMDataSimpleInfo<int16_t> kEffectData = kEnchantEffect[iIndex];
	//				if (kEffectData.ParamA() > 0 || kEffectData.ParamB() > 0 || kEffectData.ParamC() > 0 || kEffectData.ParamD() > 0)
	//					kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_073), usFont, usFontSize, RED, 0, 0, true));
	//				
	//				if (kEffectData.ParamA() > 0)
	//				{
	//					ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(kEffectData.ParamA());
	//					if(pkEffect)
	//					{
	//						StringStreamType sStream("");
	//						int level = iEnchant - kEffectData.ID() + 1;
	//						sStream << "<color val=0xffffffff><image val=16>Common1/Common1_image_Point</image></color>" << pkEffect->GetDesc(level) << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_074) << iEnchant <<")";
	//						kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, true));
	//					}
	//				}
	//					
	//				if (kEffectData.ParamB() > 0)  
	//				{
	//					ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(kEffectData.ParamB());
	//					if(pkEffect)
	//					{
	//						StringStreamType sStream("");
	//						int level = iEnchant - kEffectData.ID() + 1;
	//						sStream << "<color val=0xffffffff><image val=16>Common1/Common1_image_Point</image></color>" << pkEffect->GetDesc(level) << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_074) << iEnchant <<")";
	//						kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, true));
	//					}
	//				}
	//				if (kEffectData.ParamC() > 0)  
	//				{
	//					ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(kEffectData.ParamC());
	//					if(pkEffect)
	//					{
	//						StringStreamType sStream("");
	//						int level = iEnchant - kEffectData.ID() + 1;
	//						sStream << "<color val=0xffffffff><image val=16>Common1/Common1_image_Point</image></color>" << pkEffect->GetDesc(level) << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_074) << iEnchant <<")";
	//						kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, true));
	//					}
	//				}
	//				if (kEffectData.ParamD() > 0)  
	//				{
	//					ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(kEffectData.ParamD());
	//					if(pkEffect)
	//					{
	//						StringStreamType sStream("");
	//						int level = iEnchant - kEffectData.ID() + 1;
	//						sStream << "<color val=0xffffffff><image val=16>Common1/Common1_image_Point</image></color>" << pkEffect->GetDesc(level) << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_074) << iEnchant <<")";
	//						kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, true));
	//					}
	//				}
	//			}
	//		}

	//		for(int i=0; i<Protocol::MaxEquipAttribCount; i++)
	//		{
	//			Protocol::EquipAttrib pkEquipAttr;
	//			if(pkBackPackEntry->GetEquipAttrib(i, pkEquipAttr))
	//			{
	//				ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(pkEquipAttr.m_TypeID);
	//				if(pkEffect != NULL)
	//				{
	//					CF_ItemEffect::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_ItemEffect>(pkEquipAttr.m_TypeID);
	//					if(NULL != entry)
	//					{
	//						//����ʾװ�����ӿ�
	//						if(entry->_lItemEffectID != e_IE_ATTR_SOCKET)
	//						{
	//							CF_ItemEffectFunc::DataEntry *funcEntry = SafeGetCSVFileDataEntryPtr<CF_ItemEffectFunc>(entry->_lItemEffectID);
	//							if(NULL != funcEntry)
	//							{
	//								SortItemEffect kInfo;
	//								kInfo.iLevel = pkEquipAttr.m_Level;
	//								kInfo.pkItemEffect = pkEffect;
	//								kInfo.iSortID = funcEntry->_TooltipIndex;
	//								kInfo.iEffectTypeID = pkEquipAttr.m_TypeID;
	//								kInfo.iIndex = i;
	//								//kInfo.bRefine = iRefineIdx == i;
	//								kVector.push_back(kInfo);
	//							}
	//						}
	//					}					

	//					/*
	//					StringStreamType kStrStream;
	//					kStrStream << "(" << char('0' + pkEquipAttr.m_Level) << "/" << char('0' + pkEquipAttr.m_Max) << ")";

	//					StringType sMax = pkEffect->GetValueString(pkEquipAttr.m_Max);
	//					kStrStream << "(���" << sMax << ")";

	//					kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_QUENCH_COUNT, "ǿ���ȼ�", true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	//					*/
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		
	//		CF_ItemGenEffect::DataEntryMapExternIterator kIter = CF_ItemGenEffect::GetSingletonPtr()->GetIterator();
	//		
	//		
	//		if(pkItemGen && pkItemGen->InitEffectCount > 0)
	//		{
	//			int effects[] = {pkItemGen->GenEffect1, pkItemGen->GenEffect2, pkItemGen->GenEffect3,
	//							pkItemGen->GenEffect4, pkItemGen->GenEffect5, pkItemGen->GenEffect6};
	//			for(int i=0; i<pkItemGen->InitEffectCount; ++i)
	//			{
	//				for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	//				{
	//					const CF_ItemGenEffect::DataEntry* pkItemGenEffect = kIter.PeekNextValuePtr();
	//					if(pkItemGenEffect->_lItemDropGenID == effects[i])
	//					{
	//						ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(pkItemGenEffect->_lItemEffectID);
	//						if(pkEffect != NULL)
	//						{
	//							CF_ItemEffect::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_ItemEffect>(pkItemGenEffect->_lItemEffectID);
	//							if(NULL != entry)
	//							{
	//								//����ʾװ�����ӿ�
	//								if(entry->_lItemEffectID != e_IE_ATTR_SOCKET)
	//								{
	//									CF_ItemEffectFunc::DataEntry *funcEntry = SafeGetCSVFileDataEntryPtr<CF_ItemEffectFunc>(entry->_lItemEffectID);
	//									if(NULL != funcEntry)
	//									{
	//										SortItemEffect kInfo;
	//										kInfo.iLevel = 1;
	//										kInfo.pkItemEffect = pkEffect;
	//										kInfo.iSortID = funcEntry->_TooltipIndex;
	//										kInfo.iEffectTypeID = pkItemGenEffect->_lItemEffectID;
	//										kInfo.iIndex = i;
	//										kVector.push_back(kInfo);
	//									}
	//								}
	//							}	
	//						}
	//						break;
	//					}
	//				}
	//			}
	//		}
	//	}


	//	//std::sort(kVector.begin(), kVector.end(),  _sortFunc);
	//	
	//	if(iCount > 0)
	//	{
	//		if(b)
	//			kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_013), usFont, usFontSize, RED, 0, 0, true));
	//		else
	//			kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_014), usFont, usFontSize, RED, 0, 0, true));
	//	}

	//	for(std::vector<SortItemEffect>::iterator it = kVector.begin(); it != kVector.end(); ++it)
	//	{
	//		StringStreamType sStream("");
	//		sStream << "<color val=0xffffffff><image val=16>Common1/Common1_image_Point</image></color>" << it->pkItemEffect->GetDesc(it->iLevel);
	//		if(!b)
	//			sStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_017) << it->iLevel <<")";
	//		if(LC_Helper::GetIsItemEffectPerfect(pkItemData->_iID, it->iIndex, it->iEffectTypeID))
	//		{
	//			sStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_018);
	//		}
	//		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, true));
	//	}

	//	int iSize = kVector.size();
	//	if(iSize < iCount)
	//	{
	//		for(int i=iSize; i<iCount; ++i)
	//		{
	//			if(b)
	//				kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sPre + GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_015), usFont, usFontSize, BLUE, 0, 0, true));
	//			else
	//				kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sPre + GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_016), usFont, usFontSize, BLUE, 0, 0, true));
	//		}
	//	}

	//	if(pkItemGen && !pkItemGen->SuperAttrib.empty())
	//	{
	//		int iIdx = 0;
	//		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_SUPER_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_019), usFont, usFontSize, RED, 0, 0, true));
	//		UT_SIMDataList::iterator it = pkItemGen->SuperAttrib.begin();
	//		for( ; it != pkItemGen->SuperAttrib.end(); ++it)
	//		{
	//			StringType sColor = "0xffffcc00";
	//			StringStreamType sStream("");
	//			bool bActive = it->ParamB() == 0 || pkBackPackEntry->IsSuperAttrSet(iIdx);
	//			if(!bActive)
	//			{
	//				sColor = "0xff999999";
	//			}

	//			CF_ItemAttrActive::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_ItemAttrActive>(it->ParamB());
	//			if(entry)
	//			{
	//				ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(it->ID());
	//				if(pkEffect != NULL)
	//				{
	//					sStream << "<color val=" << sColor << ">" << pkEffect->GetDesc(0);
	//				}
	//				UT_SIMDataList::iterator itera = entry->_kSubordinate.begin();
	//				for( ; itera != entry->_kSubordinate.end(); ++itera)
	//				{
	//					if(itera->ID() < LC_SUB_TP_END)
	//						sStream << " [" << SUB_TP_NAME(itera->ID()) << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_020) << itera->ParamA() <<  GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_021);
	//				}
	//				sStream << "</color>";
	//				kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_SUPER_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, true));
	//			}

	//			++iIdx;
	//		}
	//	}

	//	//ͼ������
	//	if(b && pkBackPackEntry)
	//	{
	//		iFontColor = PURPLE3;
	//		kVector.clear();
	//		for(int i=0; i<MAX_ITEM_TOTEM_COUNT; i++)
	//		{
	//			Protocol::EquipAttrib pkEquipAttr;
	//			if(pkBackPackEntry->GetTotemAttrib(i, pkEquipAttr))
	//			{
	//				ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(pkEquipAttr.m_TypeID);
	//				if(pkEffect != NULL)
	//				{
	//					CF_ItemEffect::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_ItemEffect>(pkEquipAttr.m_TypeID);
	//					if(NULL != entry)
	//					{
	//						CF_ItemEffectFunc::DataEntry *funcEntry = SafeGetCSVFileDataEntryPtr<CF_ItemEffectFunc>(entry->_lItemEffectID);
	//						if(NULL != funcEntry)
	//						{
	//							SortItemEffect kInfo;
	//							kInfo.iLevel = pkEquipAttr.m_Level;
	//							kInfo.pkItemEffect = pkEffect;
	//							kInfo.iSortID = funcEntry->_TooltipIndex;
	//							kInfo.iEffectTypeID = pkEquipAttr.m_TypeID;
	//							kInfo.iIndex = i;
	//							kVector.push_back(kInfo);
	//						}
	//					}
	//				}
	//			}
	//			else
	//			{
	//				SortItemEffect kInfo;
	//				kInfo.iLevel = 0;
	//				kInfo.pkItemEffect = NULL;
	//				kInfo.iSortID = 0;
	//				kInfo.iEffectTypeID = 0;
	//				kInfo.iIndex = i;
	//				kVector.push_back(kInfo);
	//			}
	//		}

	//		if(kVector.size() > 0)
	//		{
	//			kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_069), usFont, usFontSize, RED, 0, 0, true));
	//			
	//			for(std::vector<SortItemEffect>::iterator it = kVector.begin(); it != kVector.end(); ++it)
	//			{
	//				StringStreamType sStream("");
	//				sStream << "<image val=70,22>" << TOTEM_ATTRIB_IMAGE[it->iIndex] << "</image>";

	//				int32_t totemExtra = pkBackPackEntry->GetTotemExtraAttr(it->iIndex);
	//				if(it->pkItemEffect != NULL)
	//				{
	//					if(totemExtra > 0)
	//					{
	//						StringType sDesc = "";
	//						CF_ItemEffect::DataEntry *itemEffectEntry = SafeGetCSVFileDataEntryPtr<CF_ItemEffect>(it->iEffectTypeID);
	//						if(NULL != itemEffectEntry)
	//						{
	//							CF_ItemEffectFunc::DataEntry *effecFunc = SafeGetCSVFileDataEntryPtr<CF_ItemEffectFunc>(itemEffectEntry->_lItemEffectID);
	//							if(NULL != effecFunc)
	//							{
	//								sDesc = effecFunc->_lDesc;
	//							}
	//						}

	//						char para[256] = {0};
	//						sprintf_s(para, 256, "%.2f(%s%.2f)", (it->pkItemEffect->GetValue(it->iLevel)+totemExtra)*0.01f, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_079), totemExtra*0.01f);

	//						char buffer[256] = {0};
	//						sprintf_s(buffer, 256, sDesc.c_str(), para);
	//						sStream << buffer;
	//					}
	//					else
	//					{
	//						sStream << it->pkItemEffect->GetDesc(it->iLevel);
	//					}
	//					//sStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_071);
	//				}
	//				else
	//				{
	//					sStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_070);
	//				}
	//				kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_TOTEM_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, true));
	//			}
	//		}
	//	}
	//}

	return true;
}

bool LC_EquipItemTooltip::_addTooltipFieldSkill(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry *pkEntry = GetItemInfoEntry();
	if(NULL == pkEntry)
	{
		return false;
	}

	int iSkillID = pkEntry->_AffiliatedSkill;
	int iCurSkillID = 0;

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BLUE;

	int iCurLevel = 0;
	StringType sName = "";
	bool bLock = true;

	LC_BackPackEntry *pkBackPackEntry = GetBackPackEntry();
	if(NULL == pkBackPackEntry)
	{
		iCurSkillID = iSkillID;
	}
	else
	{
		iCurSkillID = pkBackPackEntry->GetEquipAffiliatedSkill();
		bLock = iCurSkillID == 0;
		if(bLock)
			iCurSkillID = iSkillID;
	}

	if(iCurSkillID == 0)
	{
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_EQUIP_SKILL, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_082), usFont, usFontSize, iFontColor, 0, 0, true));
		return true;
	}

	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(iCurSkillID);
	if(pkSkillAttrib != NULL)
		iCurLevel = pkSkillAttrib->_lCurLevel;
	
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(iCurSkillID);
	if(pkSkillData != NULL)
		sName = pkSkillData->_kName;

	StringStreamType sSteam("");
	sSteam << "<color val=0xFFE7E69A>" << sName << " <color val=0xFFE9E8E6>Lv." << iCurLevel << "</color></color>#";
	StringType kUseResult = "";
	LC_TooltipBase::_getUseResult<CSVFile::CF_SkillSpecialAttrib>(iCurSkillID, kUseResult);
	sSteam << pkEntry->_ReinforceType << "#" <<  (bLock ? iCurSkillID : (iCurSkillID+1)) << "#" << Bool2Long(bLock) << "#" <<  kUseResult;
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_EQUIP_SKILL, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_012), true, sSteam.str(), usFont, usFontSize, iFontColor, 0, 0, true));

	return true;
}

StringType LC_EquipItemTooltip::_getAttributeValueString(attr_value_type lBaseMinVal, attr_value_type lBaseMaxVal, attr_value_type lAddMinVal,attr_value_type lAddMaxVal, const StringType& kCaption)
{
	StringType kAttribute = "";
	StringStreamType kStrStream;
	//���������Ч
	if ( _decodeValueAttri(lBaseMinVal + lAddMinVal, lBaseMaxVal + lAddMaxVal, kCaption, kAttribute) )
	{
		if(lAddMinVal == 0 && lAddMaxVal == 0)
		{
			//kAttribute = "<color val=0xff1b140b>" + kAttribute + "</color>";
			return kAttribute;
		}
		
		if(lAddMinVal == lAddMaxVal)
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMinVal) << ") </rdmode></color>" ;
		else if( _abs64(lAddMinVal) == 0)
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMaxVal) << ") </rdmode></color>" ;
		else if( _abs64(lAddMaxVal) == 0)
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMinVal) << ") </rdmode></color>" ;
		else
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMinVal) << " - " << _abs64(lAddMaxVal) << ") </rdmode></color>" ;

		/*if(lAddMinVal == lAddMaxVal)
			kStrStream << "<color val=0xff1b140b>" << kAttribute << "</color><color val=0xff32cc28><rdmode val=1>  (+" << UT_MathBase::AbsInt(lAddMinVal) << ") </rdmode></color>" ;
		else if(UT_MathBase::AbsInt(lAddMinVal) == 0)
			kStrStream << "<color val=0xff1b140b>" << kAttribute << "</color><color val=0xff32cc28><rdmode val=1>  (+" << UT_MathBase::AbsInt(lAddMaxVal) << ") </rdmode></color>" ;
		else if(UT_MathBase::AbsInt(lAddMaxVal) == 0)
			kStrStream << "<color val=0xff1b140b>" << kAttribute << "</color><color val=0xff32cc28><rdmode val=1>  (+" << UT_MathBase::AbsInt(lAddMinVal) << ") <rdmode></color>" ;
		else
			kStrStream << "<color val=0xff1b140b>" << kAttribute << "</color><color val=0xff32cc28><rdmode val=1>  (+" << UT_MathBase::AbsInt(lAddMinVal) << " - " << UT_MathBase::AbsInt(lAddMaxVal) << ") <rdmode></color>" ;*/

		return kStrStream.str();
	}
	else
		return "";
}

StringType LC_EquipItemTooltip::_getAttributePercentString(attr_value_type lBaseMinVal, attr_value_type lBaseMaxVal, attr_value_type lAddMinVal,attr_value_type lAddMaxVal,const StringType& kCaption)
{
	StringType kAttribute = "";
	StringStreamType kStrStream;

	//���������Ч
	if ( _decodePercentAttri(lBaseMinVal + lAddMinVal, lBaseMaxVal + lAddMaxVal, kCaption, kAttribute) )
	{
		if(lAddMinVal == 0 && lAddMaxVal == 0)
		{
			//kAttribute = "<color val=0xff1b140b>" + kAttribute + "</color>";
			return kAttribute;
		}

		if(lAddMinVal == lAddMaxVal)
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMinVal) <<  "%)</rdmode></color>" ;
		else if( _abs64(lAddMinVal) == 0)
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMaxVal) << "%)</rdmode></color>" ;
		else if( _abs64(lAddMaxVal) == 0)
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMinVal) << "%)</rdmode></color>" ;
		else
			kStrStream <<  kAttribute << "<color val=0xff32cc28><rdmode val=1>  (+" << _abs64(lAddMinVal) << " - " << _abs64(lAddMaxVal)  << "%)</rdmode></color>" ;

		return kStrStream.str();
	}
	else
		return "";
}

StringType LC_EquipItemTooltip::_getItemEffectAttrValueString(attr_value_type lBaseMinVal, attr_value_type lCompareVal, const StringType& kCaption, bool bPercent)
{
	if(lBaseMinVal == lCompareVal || (lBaseMinVal == 0 && lCompareVal == 0))
	{
		return "";
	}
	
	StringStreamType kStrStream;
	attr_value_type lChangeVal = lCompareVal - lBaseMinVal;
	kStrStream << "|";
	if(lChangeVal > 0)
	{
		kStrStream << "<color val=0xff00ee00>+";
	}
	else
	{
		kStrStream << "<color val=0xffdb1a00>";
	}

	if(bPercent)
	{
		kStrStream << fixed << setprecision(2) << lChangeVal * 0.01f << "%";
	}
	else
	{
		kStrStream << lChangeVal;
	}

	kStrStream << "</color>" << kCaption;
	return kStrStream.str();
}

static int32_t _st_get_suit_func_weight(int32_t iID)
{
	static ItemSuitManager * inst = SafeGetSingleton<ItemSuitManager>();
	ItemSuitInfoType * pkSuitData = inst->GetSuitInfo(iID);
	return pkSuitData ? pkSuitData->m_iWeight : 0;
}
typedef mem::list<std::pair<int32_t, int32_t> > local_suit_list;
static bool _st_suit_comp(const local_suit_list::value_type& a, const local_suit_list::value_type& b)
{
	return _st_get_suit_func_weight(a.first) > _st_get_suit_func_weight(b.first);
}


//bool LC_EquipItemTooltip::_addTooltipFieldSuiteAttri( TooltipFieldVector& kFieldVector )
//{
//	LC_BackPackEntry *pkItemEntry = GetBackPackEntry();
//	if(NULL != pkItemEntry && ITEM_UNIDENTIFIED == pkItemEntry->GetIdentify())
//	{
//		return false;
//	}
//
//	CF_ItemList::DataEntry *pkItemInfo = GetItemInfoEntry();
//	if(NULL == pkItemInfo)
//	{
//		return false;
//	}
//
//	CF_ItemGen::DataEntry * gen_entery = SafeGetCSVFileDataEntryPtr<CF_ItemGen>(pkItemInfo->_iID, false, true);
//	if(NULL == gen_entery)
//		return false;
//
//	if(gen_entery->EquipeSetID.empty())
//	{
//		return false;
//	}
//
//	ItemSuitManager *inst = SafeGetSingleton<ItemSuitManager>();
//
//	SuitMap kMap;
//	ItemSuitInfoType *pkItemSuiteData;
//
//	UT_SSIMDataListInt16::iterator it = gen_entery->EquipeSetID.begin();
//	for(; it!=gen_entery->EquipeSetID.end(); ++it)
//	{
//		pkItemSuiteData = inst->GetSuitInfo(it->ID());
//		if(pkItemSuiteData)
//		{
//			if(!_suiteAttriAdd(kFieldVector, it->ID(), kMap))
//				return false;
//		}
//	}
//
//	if(NULL == pkItemSuiteData)
//	{
//		return false;
//	}
//
//	LC_PlayerBase *pkPlayer = GetObserver();
//
//	local_suit_list suitIDs;
//	if(pkPlayer)
//	{
//		if(pkItemInfo->_lCategory2 >= ITEM_CATEGORY_SUB_EQUIP_COST && pkItemInfo->_lCategory2 <= ITEM_CATEGORY_SUB_EQUIP_END)
//		{
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMap& kPlayerEquipSuitMap = ((LC_ClientGamePlayer*)pkPlayer)->GetPlayerEquipSubSuitMap();
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMapIter Iter = kPlayerEquipSuitMap.begin();
//			for (; Iter != kPlayerEquipSuitMap.end(); ++Iter)
//			{
//				suitIDs.push_back(*Iter);
//			}
//		}
//		else
//		{
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMap& kPlayerEquipSuitMap = pkPlayer->GetPlayerEquipSuitMap();
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMapIter Iter = kPlayerEquipSuitMap.begin();
//			for (; Iter != kPlayerEquipSuitMap.end(); ++Iter)
//			{
//				suitIDs.push_back(*Iter);
//			}
//		}
//	}
//	else if(m_pkRankUserInfo)
//	{
//		if(pkItemInfo->_lCategory2 >= ITEM_CATEGORY_EQUIP_CLOAK && pkItemInfo->_lCategory2 <= ITEM_CATEGORY_EQUIP_SHIELD)
//		{
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMap& kPlayerEquipSuitMap = m_pkRankUserInfo->GetEquipSubSuitMap();
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMapIter Iter = kPlayerEquipSuitMap.begin();
//			for (; Iter != kPlayerEquipSuitMap.end(); ++Iter)
//			{
//				suitIDs.push_back(*Iter);
//			}
//		}
//		else
//		{
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMap& kPlayerEquipSuitMap = m_pkRankUserInfo->GetEquipSuitMap();
//			LC_PlayerBase::LC_PlayerEquipSuitPtrMapIter Iter = kPlayerEquipSuitMap.begin();
//			for (; Iter != kPlayerEquipSuitMap.end(); ++Iter)
//			{
//				suitIDs.push_back(*Iter);
//			}
//		}		
//	}
//
//	unsigned short usFont = FONT_LARGE;
//	unsigned short usFontSize = FONTSIZE_LARGE;
//	int iFontColor = 0xFFFFB400;
//
//	int iSuiteEquipNum = 0;
//	int iSuiteTotalNum = kMap.size();
//	
//	for(SuitMap::iterator i = kMap.begin(); i != kMap.end(); ++i )
//	{
//		if(i->second.bLight)
//		{
//			++iSuiteEquipNum;
//		}		
//	}
//	
//	StringStreamType kSuiteStrStream("");
//	kSuiteStrStream << "��װ��" << pkItemSuiteData->m_kName << "(" << iSuiteEquipNum << "/" << iSuiteTotalNum << ")";
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SUITE_NAME, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_027), true, kSuiteStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
//
//	StringStreamType kStringStream("");
//	int iIdx = 1;
//	for(SuitMap::iterator i = kMap.begin(); i != kMap.end(); ++i )
//	{
//		CF_ItemList::DataEntry* pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(i->second.iItemID);
//		if(pkItemData)
//		{
//			CF_ItemCategory::DataEntry *pkItemCategory = SafeGetCSVFileDataEntryPtr<CF_ItemCategory>(pkItemData->_lCategory2);
//			if(pkItemCategory)
//			{
//				int iFontColor = GRAY;
//				if (i->second.bLight)
//					iFontColor = GREEN;
//
//				kStringStream << "<color val=" << iFontColor << ">" << pkItemCategory->_kName << "  </color>";
//				if(iIdx/4 > 0 && iIdx%4 == 0)
//				{
//					kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SUITE_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_007), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));		//������װ��Ϣ
//					kStringStream.str("");
//				}
//
//				++iIdx;
//			}
//		}
//	}
//
//	int iNumber = 0;
//	ItemEffectManage *pkItemEffectMgr = ItemEffectManage::GetSingletonPtr();
//	if (!suitIDs.empty())
//	{
//		suitIDs.sort(_st_suit_comp);
//		local_suit_list::iterator it = suitIDs.begin();
//		mem::set<int> kSet;
//
//		mem::map<int, ItemSuitInfoType::ItemSuitEffectTypes> kEffectMap;
//		for (; it!=suitIDs.end(); ++it)
//		{
//			UT_SSIMDataListInt16::const_iterator setItr=gen_entery->EquipeSetID.begin();
//			for( ; setItr!=gen_entery->EquipeSetID.end(); ++setItr)
//			{
//				if(setItr->ID() == it->first)
//				{
//					ItemSuitInfoType *pkFunc = inst->GetSuitInfo(it->first);
//					if(pkFunc)
//					{
//						int iNum = it->second;
//						pkFunc->GetEfectTypes(iNum, kEffectMap, &kSet);
//					}
//				}
//			}
//		}
//
//		iFontColor = YELLOW;
//		for(mem::set<int>::iterator i=kSet.begin(); i!=kSet.end(); ++i)
//		{
//			iNumber = *i;
//			StringStreamType sStream;
//			sStream <<  "<color val= " <<  iFontColor << ">[" << iNumber << "����] </color>";
//
//			ItemSuitInfoType::ItemSuitEffectTypes& effects = kEffectMap[iNumber];
//			ItemSuitInfoType::ItemSuitEffectTypes::const_iterator iter = effects.begin();
//			for(; iter!=effects.end(); ++iter)
//			{
//				ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(*iter);
//				if(NULL != pkEffect)
//				{
//					sStream << pkEffect->GetDesc(0) << " ";
//				}
//			}
//			
//			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SUITE_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_028), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0) );
//		}
//	}
//
//	iFontColor = GRAY;
//	for( int i = iNumber + 1 ; i <= iSuiteTotalNum ; ++i)
//	{
//		ItemSuitInfoType::ItemSuitEffectTypes& effectList = pkItemSuiteData->m_effect_data_current[i];
//		StringStreamType sStream;
//		bool bAttrib = effectList.size() > 0;
//		if(bAttrib)
//		{
//			sStream << "[" << i << "����] ";
//
//			for(ItemSuitInfoType::ItemSuitEffectTypes::const_iterator j = effectList.begin(); j != effectList.end() ; ++j)
//			{
//				ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(*j);
//				if(NULL != pkEffect)
//				{
//					sStream << pkEffect->GetDesc(0) << " ";
//				}
//			}
//
//			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SUITE_ATTRI, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_028), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0) );
//		}
//	}
//
//	return true;
//}

//bool LC_EquipItemTooltip::_suiteAttriAdd(TooltipFieldVector& kFieldVector, int iSuitId, SuitMap &kMap)
//{
//	ItemSuitManager *inst = SafeGetSingleton<ItemSuitManager>();
//	CF_ItemList::DataEntry *pkItemInfo = GetItemInfoEntry();
//	if(NULL == pkItemInfo)
//	{
//		return false;
//	}
//
//	ItemSuitInfoType * pkItemSuiteData = inst->GetSuitInfo(iSuitId);
//	if (!pkItemSuiteData)
//	{
//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "��װ[%i]�����޷�ʹ��", iSuitId);
//		T_ASSERT_CRITICAL_S(false);
//		return false;
//	}
//
//	//int lSuiteFuncID = pkItemInfo->_lSuiteFuncID;
//	////����Ʒ��������װ����
//	//if (lSuiteFuncID <= 0)
//	//{
//	//	//kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SUITE_ATTRI, "����װ", false, "", usFont, usFontSize, iFontColor, 0, 0));
//	//	//return true;
//	//	return false;
//	//}
//
//	LC_PlayerBase* pkObserver = GetObserver();
//
//	bool hasObserver = pkObserver != NULL;
//	bool hasRankUser = m_pkRankUserInfo != NULL;
//
//	for( ItemSuitInfoType::ItemTypeIDListType::iterator parti = pkItemSuiteData->m_item_typeid_.begin(); parti != pkItemSuiteData->m_item_typeid_.end(); ++parti )
//	{
//		bool has = false;
//		if(!hasRankUser)
//			has = hasObserver ? _suitePartIsEquiped(*parti, pkObserver) : false;
//		else
//			has = _suitePartIsEquiped(*parti, m_pkRankUserInfo);
//
//		CF_ItemList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(*parti);
//		if(pkEntry)
//		{
//			CF_ItemEquipLimitFunc::DataEntry *pkEquipLmt = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(pkEntry->_lEquipLimitFuncID);
//			if(pkEquipLmt)
//			{
//				if(has || kMap.find(pkEquipLmt->_lEquipType) == kMap.end())
//				{
//					SuitInfo kInfo;
//					kInfo.iItemID = *parti;
//					kInfo.bLight = has;
//					kInfo.iSuitID = iSuitId;
//					kMap[pkEquipLmt->_lEquipType] = kInfo;
//				}
//			}
//		}
//	}
//
//	//std::sort(kMap.begin(), kMap.end(), _isLess);
//
//
//	return true;
//}

bool LC_EquipItemTooltip::_addTooltipFieldCompareAttr(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry *pkItemInfo = GetItemInfoEntry();
	if(NULL == pkItemInfo)
	{
		return false;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase *pkItem = pkItemFactory->RequestItem(pkItemInfo->_iID);
	if(NULL == pkItem)
	{
		return false;
	}
	
	LC_BackPackEntry *pkItemEntry = GetBackPackEntry();
	if(NULL == pkItemEntry)
	{
		return false;
	}

	LC_PlayerBase *pkPlayer = GetObserver();
	if(NULL == pkPlayer)
	{
		return false;
	}

	LC_ActorAttributeMap attrMap;
	pkItem->ComputerAttribMapModify(&attrMap, pkItemEntry);

	LC_BackPackEntry *pkCurrentEquipEntry = GetCurrentEquip();
	if(NULL == pkCurrentEquipEntry)
	{
		return false;
	}

	LC_ItemBase *pkCurEquip = pkItemFactory->RequestItem(pkCurrentEquipEntry->GetTypeID());
	if(NULL == pkCurEquip)
	{
		return false;
	}

	LC_ActorAttributeMap curEquipAttrMap;
	pkCurEquip->ComputerAttribMapModify(&curEquipAttrMap, pkCurrentEquipEntry);
	
	LC_ActorAttributeMap plyAttrMap = *(pkPlayer->GetAttributeMap());
	for(int i=0; i<ATT_MAX_COUNT; ++i)
	{
		plyAttrMap.AddAttributePercentFunc(i, curEquipAttrMap.GetAttributePercent(i)*-1);
		plyAttrMap.AddAttributePercentFunc(i, attrMap.GetAttributePercent(i));
		plyAttrMap.AddAttributeValueFunc(i, curEquipAttrMap.GetAttributeValue(i)*-1);
		plyAttrMap.AddAttributeValueFunc(i, attrMap.GetAttributeValue(i));
	}
	plyAttrMap.RecomputeAll();

	LC_ActorAttributeMap *curPlyAttrMap = pkPlayer->GetAttributeMap();
	curPlyAttrMap->RecomputeAll();

	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	
	StringType kResult = "";
	//����
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MAX_HP), plyAttrMap.GetAttribute(ATT_MAX_HP), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_029));

	//��Ϣ����
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MAX_MP), plyAttrMap.GetAttribute(ATT_MAX_MP), "��Ϣ����");
	
	//��������
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MAX_STM), plyAttrMap.GetAttribute(ATT_MAX_STM), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_030));
		
	//���ڵ�ҩ����
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MAX_AMM), plyAttrMap.GetAttribute(ATT_MAX_AMM), "������������");
	
	//����
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MAX_ATK), plyAttrMap.GetAttribute(ATT_MAX_ATK), GET_MSG(UT_GAMESERVERLOGMANAGER_CPP_001));
	
	//����
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_DEF), plyAttrMap.GetAttribute(ATT_DEF), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_031));
	
	//�����˺�
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_CIR_DAM), plyAttrMap.GetAttribute(ATT_CIR_DAM), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_032), true);
	
	//�����ȼ�
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_CIR_RATE), plyAttrMap.GetAttribute(ATT_CIR_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_033));
	
	//���ܵȼ�
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_DOG_RATE), plyAttrMap.GetAttribute(ATT_DOG_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_034));
	
	//�ƶ��ٶ�
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MOVE_SPEED), plyAttrMap.GetAttribute(ATT_MOVE_SPEED), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_035));
	
	//�����ظ�
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_HP_RECOVER), plyAttrMap.GetAttribute(ATT_HP_RECOVER), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_036));
	
	//��Ϣ�ظ�
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MP_RECOVER), plyAttrMap.GetAttribute(ATT_MP_RECOVER), "��Ϣ�ظ�");
	
	//�����ȡ
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_EXP_RATE), plyAttrMap.GetAttribute(ATT_EXP_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_037), true);
	
	//��Ǯ��ȡ
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_CASH_RATE), plyAttrMap.GetAttribute(ATT_CASH_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_038), true);
	
	//ħ����Ʒ��ȡ����
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_MF_RATE), plyAttrMap.GetAttribute(ATT_MF_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_039), true);
	
	//���ʰȡ��Χ
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_PICK_CRASH_RNG), plyAttrMap.GetAttribute(ATT_PICK_CRASH_RNG), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_040));

	//����
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_ATK_RATE), plyAttrMap.GetAttribute(ATT_ATK_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_041));

	//����
	kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_CIR_DEF), plyAttrMap.GetAttribute(ATT_CIR_DEF), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_042));

	//����ս����
	kResult += _getItemEffectAttrValueString(LC_Helper::GetEquipCombatScoreByItemID(pkCurrentEquipEntry->GetTypeID()), LC_Helper::GetEquipCombatScoreByItemID(pkItemInfo->_iID), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_043));

	//�����ظ�
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_STM_RECOVER_RATE), plyAttrMap.GetAttribute(ATT_STM_RECOVER_RATE), "�����ظ�");

	//���������ظ�
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_AMM_RECOVER_RATE), plyAttrMap.GetAttribute(ATT_AMM_RECOVER_RATE), "���������ظ�");
	
	//������
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_RES_FREEZ), plyAttrMap.GetAttribute(ATT_RES_FREEZ), "������");

	//���ٿ���
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_RES_SLOW), plyAttrMap.GetAttribute(ATT_RES_SLOW), "���ٿ���");

	//��Ĭ����
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_RES_SILENCE), plyAttrMap.GetAttribute(ATT_RES_SILENCE), "��Ĭ����");

	//���Կ���
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_RES_DIZZ), plyAttrMap.GetAttribute(ATT_RES_DIZZ), "���Կ���");

	//˯�߿���
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_RES_SLEEP), plyAttrMap.GetAttribute(ATT_RES_SLEEP), "˯�߿���");

	//�˺�����
	//kResult += _getItemEffectAttrValueString(curPlyAttrMap->GetAttribute(ATT_DMG_RATE), plyAttrMap.GetAttribute(ATT_DMG_RATE), "�˺�", true);

	if(kResult != "")
	{
		_addTooltipFieldCompareAttrName(kFieldVector);
		
		TStringVector kAttributeVector = UT_BaseStringUtility::SplitString(kResult,'|');
		if (!kAttributeVector.empty())
		{
			for(unsigned int i = 0; i < kAttributeVector.size(); i++)
			{
				StringType s = kAttributeVector[i];
				if ("" != s)
				{
					int iFontColor = GOLD2;
					kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_COMPARE_DESCRIPT, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_044), true, kAttributeVector[i], usFont, usFontSize, iFontColor, 0, 0, true));
				}
			}
		}
	}

	return true;	
}

bool LC_EquipItemTooltip::_addTooltipFieldCompareAttrName(TooltipFieldVector &kFieldVector)
{
	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = GOLD2;

	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_045);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_COMPARE_DESCRIPT_NAME, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_046), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));	

	return true;
}

bool LC_EquipItemTooltip::_addTooltipFieldUnidentifyTip(TooltipFieldVector &kFieldVector)
{
	LC_BackPackEntry *pkBackPackEntry = GetBackPackEntry();
	if(NULL == pkBackPackEntry)
	{
		return false;
	}

	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = RED;

	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_047);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_UNIDENTIFY_TIP, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_048), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));	

	return true;
}

bool LC_EquipItemTooltip::_addTooltipEquipTip(TooltipFieldVector &kFieldVector)
{
	LC_BackPackEntry *pkItemEntry = GetBackPackEntry();
	if(NULL == pkItemEntry)
	{
		return false;
	}

	CF_ItemList::DataEntry *pkEntry = GetItemInfoEntry();
	if(NULL == pkEntry)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GRAY2;
	
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_EQUIP_TIP, "װ����ʾ", true, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_072), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_EquipItemTooltip::_decodeValueAttri( attr_value_type lMinAttri, attr_value_type lMaxAttri, const StringType& kItemName, StringType& kAttri )
{
	StringType kFormatTextGap = _getAttriFmtGap(kItemName);
	StringStreamType kStringStream;
	kStringStream << kAttri;

	lMinAttri = _abs64(lMinAttri);
	lMaxAttri = _abs64(lMaxAttri);

	if (lMinAttri == 0 && lMaxAttri == 0)
	{
		return false;	
	}

	if ("" != kAttri)
	{
		kStringStream << "\n";
	}

	if (lMinAttri == lMaxAttri)
	{
		//��������ͬ
		kStringStream << kItemName << kFormatTextGap << "+" << lMinAttri;
	}
	else
	{
		if(lMinAttri == 0 && lMaxAttri > 0)
			kStringStream << kItemName << kFormatTextGap << "+" << lMaxAttri;

		if(lMinAttri >0 && lMaxAttri == 0)
			kStringStream << kItemName << kFormatTextGap << "+" << lMinAttri;

		if(lMinAttri > 0 && lMaxAttri > 0)
			kStringStream << kItemName << kFormatTextGap << lMinAttri  << " -" << lMaxAttri  << "";
	}

	kAttri = kStringStream.str();
	return true;
}

bool LC_EquipItemTooltip::_decodePercentAttri(attr_value_type lMinAttri, attr_value_type lMaxAttri, const StringType& kItemName, StringType& kAttri)
{
	StringType kFormatTextGap = _getAttriFmtGap(kItemName);
	StringStreamType kStringStream;
	kStringStream << kAttri;

	lMinAttri = _abs64(lMinAttri);
	lMaxAttri = _abs64(lMaxAttri);

	if (lMinAttri == 0 && lMaxAttri == 0)
	{
		return false;	
	}

	if ("" != kAttri)
	{
		kStringStream << "\n";
	}
	
	if (lMinAttri == lMaxAttri)
	{
		//��������ͬ����ʾһλС��
		kStringStream << kItemName << kFormatTextGap << "+" << setprecision(1) << (float)lMinAttri/100.0 << "%";
	}
	else
	{
		if(lMinAttri <= 0 && lMaxAttri > 0)
			kStringStream << kItemName << kFormatTextGap << "+" << setprecision(1) << (float)lMaxAttri/100.0 << "%";
		
		if(lMinAttri >0 && lMaxAttri <= 0)
			kStringStream << kItemName << kFormatTextGap << "+" << setprecision(1) << (float)lMinAttri/100.0 << "%";

		if(lMinAttri > 0 && lMaxAttri > 0)
			kStringStream << kItemName << kFormatTextGap << setprecision(1) << (float)lMinAttri/100.0 << "% -" << (float)lMaxAttri/100.0 << "%";
	}

	kAttri = kStringStream.str();
	return true;
}

//bool LC_EquipItemTooltip::_suitePartIsEquiped( int lSuitePartID, LC_PlayerBase* pkPlayer )
//{
//	if(NULL == pkPlayer)
//		return false;
//
//	if (lSuitePartID > 0)
//	{
//		//����Ѿ�װ��		
//		if (pkPlayer->GetPackAsset()->IsExistItem(PACK_TYPE_EQUIP_SLOT, lSuitePartID))
//		{
//			return true;
//		}
//		//����Ѿ�װ����������װ		
//		if (pkPlayer->GetPackAsset()->IsExistItem(PACK_TYPE_SUB_EQUIP_SLOT, lSuitePartID))
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
//
//bool LC_EquipItemTooltip::_suitePartIsEquiped( int lSuitePartID, LC_ClientRankUserInfo *pkUserInfo)
//{
//	if(NULL == pkUserInfo)
//		return false;
//
//	if (lSuitePartID > 0)
//	{
//		//����Ѿ�װ��		
//		if (pkUserInfo->IsExistEquip(lSuitePartID))
//		{
//			return true;
//		}
//	}
//
//	return false;
//}


//bool LC_EquipItemTooltip::_addTooltipFieldSuitePart( int lSuitePartID, const StringType& kSuiteType, bool bSuitePartEquiped, TooltipFieldVector& kFieldVector )
//{
//	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
//	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(lSuitePartID);
//	if (0 == pkItemData)
//	{
//		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Can't find %d in ItemList!", lSuitePartID);
//		return false;		
//	}
//
//	CF_ItemCategory::DataEntry *pkItemCategory = SafeGetCSVFileDataEntryPtr<CF_ItemCategory>(pkItemData->_lCategory2);
//	if(NULL == pkItemCategory)
//	{
//		return false;
//	}
//
//	StringStreamType kStringStream;
//	kStringStream << "    " << pkItemCategory->_kName;
//
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = GOLDENROD;
//	if (bSuitePartEquiped)
//	{
//		iFontColor = WHITE;
//	}
//	else
//	{
//		iFontColor = GRAY;
//	}
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SUITE_NAME, kSuiteType, true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
//
//	return true;
//}

bool LC_EquipItemTooltip::_addTooltipFieldCurrentGsAttr(TooltipFieldVector &kFieldVector)
{
	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = YELLOW;

	int64_t iScore = _getBattleScore(false, false);
	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_084) << iScore;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_COMPARE_CURRENT_GS, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_050), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));	

	return true;
}

bool LC_EquipItemTooltip::_addTooltipFieldCompareGs(TooltipFieldVector &kFieldVecotr)
{
	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = GOLD;

	attr_value_type lGs = _getBattleScore(false, true);
	attr_value_type lCurEquipGs = _getBattleScore(true, true);
	
	attr_value_type lDiff = lGs - lCurEquipGs;
	if(lDiff != 0)
	{
		StringStreamType kStringStream;
		kStringStream << "(";

		StringType sSymbol = lDiff > 0 ? "+" : "-";
		kStringStream << sSymbol;
		kStringStream << UT_MathBase::AbsInt64(lDiff) << ")";
		iFontColor = lDiff > 0 ? GREEN : RED;
		kFieldVecotr.push_back(LC_TooltipField(TOOLTIP_FIELD_COMPARE_MAX_GS_DIFF, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_051), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
		return true;
	}
	return false;
}

bool LC_EquipItemTooltip::_addTooltipFieldMaxGsAttr(TooltipFieldVector &kFieldVector)
{
	

	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = PINK;

	attr_value_type gs = _getBattleScore(false, true);
	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_052) << gs;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_COMPARE_CURRENT_GS, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_053), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));	

	return true;
}

StringType LC_EquipItemTooltip::_getAttriFmtGap( const StringType& kItemName )
{
	int FORMAT_TEXT_LENGTH = 10;

	//��������Ҫ����Ŀո�����
	int iGapLength = FORMAT_TEXT_LENGTH - kItemName.size();
	StringType kFormatTextGap = "";
	for (int i = 0; i < iGapLength; i ++)
	{
		kFormatTextGap += " ";
	}

	return kFormatTextGap;
}

void LC_EquipItemTooltip::_getSuiteAttriFmtGap( const StringType& kItemName, StringType& kPreTextGap, StringType& kFormatTextGap )
{
	//int PRE_TEXT_LENGTH = 8;
	//int FORMAT_TEXT_LENGTH = 10;
	int PRE_TEXT_LENGTH = 0;
	int FORMAT_TEXT_LENGTH = 0;

  /*
	//����ǰ��Ҫ����Ŀո�
	StringType kTmpPreTextGap = "";
	for (int i = 0; i < PRE_TEXT_LENGTH; i ++)
	{
		kTmpPreTextGap += " ";
	}
	//��������Ҫ����Ŀո�����
	int iGapLength = FORMAT_TEXT_LENGTH - kItemName.size();
	StringType kTmpFormatTextGap = "";
	for (int i = 0; i < iGapLength; i ++)
	{
		kTmpFormatTextGap += " ";
	}
	
	kPreTextGap = kTmpPreTextGap;
	kFormatTextGap = kTmpFormatTextGap;
  */

  //����ǰ��Ҫ����Ŀո�
  kPreTextGap = "";
  kPreTextGap.append(PRE_TEXT_LENGTH,' ');
  //��������Ҫ����Ŀո�����
  int iGapLength = FORMAT_TEXT_LENGTH - kItemName.size();
  kFormatTextGap = "";
  kFormatTextGap.append(iGapLength,' ');
}

StringType LC_EquipItemTooltip::GetItemAttributeFuncDescript(int lAttributeFuncID)
{
	StringType kResult = "";
	return kResult;
}

StringType LC_EquipItemTooltip::GetItemAttributeFuncDescript(GameLogic::LC_ActorAttributeAppendMap& rkBaseAttributeMap,GameLogic::LC_ActorAttributeAppendMap& rkAddAttributeMap)
{
	rkBaseAttributeMap.RecomputeAll();
	rkAddAttributeMap.RecomputeAll();
	
	StringType kResult = "";

	kResult +=  _getAttributeValueString( 0, rkBaseAttributeMap.GetAttributeValue(ATT_MAX_HP), 0, rkAddAttributeMap.GetAttributeValue(ATT_MAX_HP), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_054));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_MAX_HP), 0, rkAddAttributeMap.GetAttributePercent(ATT_MAX_HP), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_054));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_MAX_MP), 0, rkAddAttributeMap.GetAttributeValue(ATT_MAX_MP),GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_055));
	kResult += "|";
	kResult +=  _getAttributePercentString(0, rkBaseAttributeMap.GetAttributePercent(ATT_MAX_MP), 0, rkAddAttributeMap.GetAttributePercent(ATT_MAX_MP), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_055));

	kResult += "|";
	kResult +=  _getAttributeValueString(0, rkBaseAttributeMap.GetAttributeValue(ATT_MAX_STM),0, rkAddAttributeMap.GetAttributeValue(ATT_MAX_STM),  GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_056));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_MAX_STM),0, rkAddAttributeMap.GetAttributePercent(ATT_MAX_STM), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_056));

	kResult += "|";
	kResult +=  _getAttributeValueString(0, rkBaseAttributeMap.GetAttributeValue(ATT_MAX_AMM),0, rkAddAttributeMap.GetAttributeValue(ATT_MAX_AMM),  GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_057));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_MAX_AMM),0, rkAddAttributeMap.GetAttributePercent(ATT_MAX_AMM), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_057));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0, rkBaseAttributeMap.GetAttributeValue(ATT_MAX_ATK), 0, rkAddAttributeMap.GetAttributeValue(ATT_MAX_ATK), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_058));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_MAX_ATK), 0, rkAddAttributeMap.GetAttributePercent(ATT_MAX_ATK), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_058));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_DEF), 0,  rkAddAttributeMap.GetAttributeValue(ATT_DEF),GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_059));
	kResult += "|";
	kResult +=  _getAttributePercentString(0, rkBaseAttributeMap.GetAttributePercent(ATT_DEF), 0, rkAddAttributeMap.GetAttributePercent(ATT_DEF), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_059));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_CIR_DAM), 0,  rkAddAttributeMap.GetAttributeValue(ATT_CIR_DAM), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_060));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_CIR_DAM), 0, rkAddAttributeMap.GetAttributePercent(ATT_CIR_DAM),GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_060));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_CIR_RATE), 0,  rkAddAttributeMap.GetAttributeValue(ATT_CIR_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_061));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_CIR_RATE), 0, rkAddAttributeMap.GetAttributePercent(ATT_CIR_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_061));

	//kResult += "|";
	//kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_ATT_RATE), 0,  rkAddAttributeMap.GetAttributeValue(ATT_ATT_RATE), "������    ");
	//kResult += "|";
	//kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_ATT_RATE),  0, rkAddAttributeMap.GetAttributePercent(ATT_ATT_RATE),"������    ");

	kResult += "|";
	kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_DOG_RATE), 0,  rkAddAttributeMap.GetAttributeValue(ATT_DOG_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_062));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_DOG_RATE), 0, rkAddAttributeMap.GetAttributePercent(ATT_DOG_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_062));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0, rkBaseAttributeMap.GetAttributeValue(ATT_MOVE_SPEED), 0,  rkAddAttributeMap.GetAttributeValue(ATT_MOVE_SPEED), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_063));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_MOVE_SPEED), 0, rkAddAttributeMap.GetAttributePercent(ATT_MOVE_SPEED), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_063));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0, rkBaseAttributeMap.GetAttributeValue(ATT_MP_COST_RATE), 0,  rkAddAttributeMap.GetAttributeValue(ATT_MP_COST_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_064));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_MP_COST_RATE), 0, rkAddAttributeMap.GetAttributePercent(ATT_MP_COST_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_064));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0, rkBaseAttributeMap.GetAttributeValue(ATT_HP_COST_RATE), 0,  rkAddAttributeMap.GetAttributeValue(ATT_HP_COST_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_065));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_HP_COST_RATE), 0, rkAddAttributeMap.GetAttributePercent(ATT_HP_COST_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_065));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_HP_RECOVER_RATE),0,  rkAddAttributeMap.GetAttributeValue(ATT_HP_RECOVER_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_066));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_HP_RECOVER_RATE),  0, rkAddAttributeMap.GetAttributePercent(ATT_HP_RECOVER_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_066));

	kResult += "|";
	kResult +=  _getAttributeValueString( 0,  rkBaseAttributeMap.GetAttributeValue(ATT_MP_RECOVER_RATE),0,  rkAddAttributeMap.GetAttributeValue(ATT_MP_RECOVER_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_067));
	kResult += "|";
	kResult +=  _getAttributePercentString( 0, rkBaseAttributeMap.GetAttributePercent(ATT_MP_RECOVER_RATE),  0, rkAddAttributeMap.GetAttributePercent(ATT_MP_RECOVER_RATE), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_067));

	return kResult;
}

attr_value_type LC_EquipItemTooltip::_getBattleScore(bool bCurrentEquip, bool bMax)
{
	CF_ItemList::DataEntry *pkItemInfo = GetItemInfoEntry();
	if(NULL == pkItemInfo)
		return 0;

	LC_ItemFactoryBase *pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_BackPackEntry *pkCurrentEquipEntry = GetCurrentEquip();

	LC_ItemBase *pkItem = pkItemFactory->RequestItem(pkItemInfo->_iID);
	LC_BackPackEntry *pkItemEntry;
	if(NULL != pkCurrentEquipEntry && bCurrentEquip)
	{
		pkItemEntry = GetCurrentEquip();
	}
	else
	{
		pkItemEntry = GetBackPackEntry();
	}

	
	if(NULL == pkItem)
	{
		return 0;
	}
	
	/*LC_ActorAttributeMap attrMap;
	if(bMax)
	{
		pkItem->ComputerMaxAttribMapModify(&attrMap, pkItemEntry);
	}
	else
	{
		pkItem->ComputerAttribMapModify(&attrMap, pkItemEntry);
	}*/
	
	attr_value_type iGs = 0;
	if(m_iStarry == 0)
	{
		if(pkItemEntry != NULL)
		{
			iGs =  LC_Helper::GetEquipCombatScore(pkItemEntry);

			EquipType eType = (EquipType)pkItem->GetEquipTypeLimit();
			int iLevel = 0;
			LC_PlayerBase *pkPlayer = GetObserver();
			if(pkPlayer)
			{
				LC_GemReinforceData *pkData = pkPlayer->GetGemReinforceAsset().GetGemReinforceData(eType);
				if(pkData)
				{
					iLevel = pkData->GetLevel();
				}
			}
		}
		else
		{
			iGs = LC_Helper::GetEquipCombatScoreByItemID(pkItemInfo->_iID);
		}
	}
	else
	{
		iGs = LC_Helper::GetEquipCombatScoreByItemIDWithStarry(pkItemInfo->_iID, m_iStarry);
	}

	return iGs;
}

bool LC_EquipItemTooltip::_sortFunc(const SortItemEffect &v1, const SortItemEffect &v2)
{
	return ((v1.iSortID < v2.iSortID) || (v1.iSortID == v2.iSortID && v1.iLevel < v2.iLevel));
}

bool LC_EquipItemTooltip::_addTooltipFieldBaseScore(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry *pkItemInfo = GetItemInfoEntry();
	if(NULL == pkItemInfo)
		return 0;

	unsigned short usFont = FONT_SMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = GREEN;

	attr_value_type iBaseScore = LC_Helper::GetEquipCombatScoreByItemID(pkItemInfo->_iID);
	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_068) << iBaseScore;

	/*LC_BackPackEntry *pkCurEquipEntry = GetCurrentEquip();
	if (!m_bCompareSelfEquip && NULL != pkCurEquipEntry)
	{
		attr_value_type iCurEquipBaseScore = LC_Helper::GetEquipCombatScoreByItemID(pkCurEquipEntry->GetTypeID());
		attr_value_type iDelta = iBaseScore - iCurEquipBaseScore;
		if(iDelta > 0)
			kStringStream << "(<color val=0xff00ee00>+" << iDelta << "</color>)";
		else if(iDelta < 0)
			kStringStream << "(<color val=0xffdb1a00>" << iDelta << "</color>)";
		kStringStream << "</color>";
	}*/
	
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_BASE_SCORE, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_043), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));	

	return true;
}
