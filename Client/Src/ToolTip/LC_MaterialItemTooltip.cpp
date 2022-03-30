
#include "ClientPCH.h"
#include "LC_MaterialItemTooltip.h"
#include "LM_LocaleManager.h"
#include "CF_ItemList.h"
#include "CF_ItemCategory.h"

using namespace GameLogic;
using namespace CSVFile;

LC_MaterialItemTooltip::LC_MaterialItemTooltip(void)
{
}

LC_MaterialItemTooltip::~LC_MaterialItemTooltip(void)
{
}

bool LC_MaterialItemTooltip::BuildTooltip()
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	//������е�tooltip�ֶ�
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//��Ʒ����
	_addTooltipFieldName(kFieldVector);

	//������
	_addTooltipFieldSubCategory(kFieldVector);

	////��Ʒ�ȼ�
	//_addTooltipFieldLevel(kFieldVector);

	//��������
	//_addTooltipFieldOverlapCount(kFieldVector);

	//��;
	//_addTooltipFieldUse(kFieldVector);
	
	//��Ʒ����
	//_addTooltipFieldDescript(kFieldVector);

	//��ϸ����
	_addTooltipDetailDescript(kFieldVector);

	//��Χ����
	_addTooltipFieldAtmosDescript(kFieldVector);

	//��Ʒ�ȼ�����
	_addTooltipFieldLevelLimit<CF_ItemUseLimitFunc::DataEntry>(kFieldVector);

	//������
	_addTooltipFieldBind(kFieldVector);

	//�۸�
	_addTooltipFieldPrice(kFieldVector);
	//���ṱ��
	_addTooltipFieldGuildCash(kFieldVector);

	if(!m_bShopOpen)
	{
		//�����ʾ
		_addTooltipSplitInfo(kFieldVector);
		//��Ƕ��ʾ
		_addTooltipFieldSocketInfo(kFieldVector);
		//����ʹ��
		_addTooltipFieldBatchUseDescript(kFieldVector);
	}
	else
	{
		//������ʾ
		_addTooltipSellTip(kFieldVector);
	}

	//���͵�����Ϣ
	_addTooltipFieldAddItemLinkInfo(kFieldVector);
	
	return true;
}

bool LC_MaterialItemTooltip::_addTooltipFieldUse( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	if (pkItemData->_kDescript == "")
	{
		return false;
	}

	StringStreamType kStrStream;
	//������Ʒ�ġ���;���ֶδ�ItemList�ġ������������ֶ�����ȡ
	kStrStream << GET_MSG(LC_MATERIALITEMTOOLTIP_CPP_001) << pkItemData->_kDescript;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_USE, GET_MSG(LC_MATERIALITEMTOOLTIP_CPP_002), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_MaterialItemTooltip::_addTooltipFieldSocketInfo(TooltipFieldVector &kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	//���Ǳ�ʯ�಻��ʾ��Ƕ��ʾ
	if(1143 != pkItemData->_lCategory2)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE2;

	StringType sStr = GET_MSG(LC_MATERIALITEMTOOLTIP_CPP_003);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SOCKET_TIP, GET_MSG(LC_MATERIALITEMTOOLTIP_CPP_004), true, sStr, usFont, usFontSize, iFontColor, 0, 0));
	return true;
}
