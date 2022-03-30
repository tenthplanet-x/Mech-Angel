#include "ClientPCH.h"
#include "LC_ConsumeItemTooltip.h"

#include "LC_PackStruct.h"
#include "LC_ActorBase.h"
#include "CF_ItemList.h"

using namespace GameLogic;
using namespace CSVFile;

LC_ConsumeItemTooltip::LC_ConsumeItemTooltip(void)
{
}

LC_ConsumeItemTooltip::~LC_ConsumeItemTooltip(void)
{
}

bool LC_ConsumeItemTooltip::BuildTooltip()
{
	return BuildTooltip(false);
}

bool LC_ConsumeItemTooltip::BuildMultiGiftTooltip(bool bCompareSelfEquip)
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

	//氛围描述
	_addTooltipFieldAtmosDescript(kFieldVector);

	//物品描述
	//_addTooltipFieldDescript(kFieldVector);

	//详细描述
	_addTooltipDetailDescript(kFieldVector);	

	//物品等级限制
	_addTooltipFieldLevelLimit<CF_ItemUseLimitFunc::DataEntry>(kFieldVector);

	//绑定性质.
	_addTooltipFieldBind(kFieldVector);

	//价格
	_addTooltipFieldPrice(kFieldVector);
	//公会贡献
	_addTooltipFieldGuildCash(kFieldVector);

	if(!m_bShopOpen)
	{
		//拆分提示
		//_addTooltipSplitInfo(kFieldVector);
		//批量使用
		_addTooltipFieldBatchUseDescript(kFieldVector);
	}
	else
	{
		//出售提示
		_addTooltipSellTip(kFieldVector);
	}

	//发送道具信息
	//_addTooltipFieldAddItemLinkInfo(kFieldVector);

	//格式化tooltip
	_formatTooltip(kFieldVector);

	return true;
}

bool LC_ConsumeItemTooltip::BuildTooltip(bool bCompareSelfEquip)
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

	//拥有唯一
	//_addToolTipFieldOwnUnique(kFieldVector);

	//物品来源
	//_addTooltipFieldOrigin(kFieldVector);

	//使用效果相关字段
	//瞬发效果
	//_addTooltipFieldUseResultProperty(kFieldVector);
	//触发技能效果
	//_addTooltipFieldUseResultTriggerSkill(kFieldVector);

	//使用次数
	//_addTooltipFieldUseCount(kFieldVector);

	//剩余时间
	//_addTooltipFieldRemainTime(kFieldVector);

	//物品叠加数量
	//_addTooltipFieldOverlapCount(kFieldVector);

	//物品描述
	//_addTooltipFieldDescript(kFieldVector);

	//详细描述
	_addTooltipDetailDescript(kFieldVector);

	//氛围描述
	_addTooltipFieldAtmosDescript(kFieldVector);

	//容量描述
	_addTooltipFieldUseCount(kFieldVector);

	//物品等级限制
	_addTooltipFieldLevelLimit<CF_ItemUseLimitFunc::DataEntry>(kFieldVector);

	//绑定性质.
	_addTooltipFieldBind(kFieldVector);

	//价格
	_addTooltipFieldPrice(kFieldVector);
	//公会贡献
	_addTooltipFieldGuildCash(kFieldVector);

	//时间描述
	//_addTooltipFieldTimeLimitDescript(kFieldVector);

	if(!m_bShopOpen)
	{
		//拆分提示
		_addTooltipSplitInfo(kFieldVector);
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

	//格式化tooltip
	_formatTooltip(kFieldVector);

	return true;
}