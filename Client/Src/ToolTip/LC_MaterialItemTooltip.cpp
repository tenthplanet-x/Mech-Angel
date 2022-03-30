
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

	//清除所有的tooltip字段
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//物品名称
	_addTooltipFieldName(kFieldVector);

	//子类型
	_addTooltipFieldSubCategory(kFieldVector);

	////物品等级
	//_addTooltipFieldLevel(kFieldVector);

	//叠加数量
	//_addTooltipFieldOverlapCount(kFieldVector);

	//用途
	//_addTooltipFieldUse(kFieldVector);
	
	//物品描述
	//_addTooltipFieldDescript(kFieldVector);

	//详细描述
	_addTooltipDetailDescript(kFieldVector);

	//氛围描述
	_addTooltipFieldAtmosDescript(kFieldVector);

	//物品等级限制
	_addTooltipFieldLevelLimit<CF_ItemUseLimitFunc::DataEntry>(kFieldVector);

	//绑定性质
	_addTooltipFieldBind(kFieldVector);

	//价格
	_addTooltipFieldPrice(kFieldVector);
	//公会贡献
	_addTooltipFieldGuildCash(kFieldVector);

	if(!m_bShopOpen)
	{
		//拆分提示
		_addTooltipSplitInfo(kFieldVector);
		//镶嵌提示
		_addTooltipFieldSocketInfo(kFieldVector);
		//批量使用
		_addTooltipFieldBatchUseDescript(kFieldVector);
	}
	else
	{
		//出售提示
		_addTooltipSellTip(kFieldVector);
	}

	//发送道具信息
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
	//材料物品的“用途”字段从ItemList的“文字描述”字段中提取
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

	//不是宝石类不显示镶嵌提示
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
