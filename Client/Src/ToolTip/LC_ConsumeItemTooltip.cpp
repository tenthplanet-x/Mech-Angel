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

	//������е�tooltip�ֶ�
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//��Ʒ����
	_addTooltipFieldName(kFieldVector);

	//������
	_addTooltipFieldSubCategory(kFieldVector);

	//��Χ����
	_addTooltipFieldAtmosDescript(kFieldVector);

	//��Ʒ����
	//_addTooltipFieldDescript(kFieldVector);

	//��ϸ����
	_addTooltipDetailDescript(kFieldVector);	

	//��Ʒ�ȼ�����
	_addTooltipFieldLevelLimit<CF_ItemUseLimitFunc::DataEntry>(kFieldVector);

	//������.
	_addTooltipFieldBind(kFieldVector);

	//�۸�
	_addTooltipFieldPrice(kFieldVector);
	//���ṱ��
	_addTooltipFieldGuildCash(kFieldVector);

	if(!m_bShopOpen)
	{
		//�����ʾ
		//_addTooltipSplitInfo(kFieldVector);
		//����ʹ��
		_addTooltipFieldBatchUseDescript(kFieldVector);
	}
	else
	{
		//������ʾ
		_addTooltipSellTip(kFieldVector);
	}

	//���͵�����Ϣ
	//_addTooltipFieldAddItemLinkInfo(kFieldVector);

	//��ʽ��tooltip
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

	//������е�tooltip�ֶ�
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//��Ʒ����
	_addTooltipFieldName(kFieldVector);

	//������
	_addTooltipFieldSubCategory(kFieldVector);

	////��Ʒ�ȼ�
	//_addTooltipFieldLevel(kFieldVector);

	//ӵ��Ψһ
	//_addToolTipFieldOwnUnique(kFieldVector);

	//��Ʒ��Դ
	//_addTooltipFieldOrigin(kFieldVector);

	//ʹ��Ч������ֶ�
	//˲��Ч��
	//_addTooltipFieldUseResultProperty(kFieldVector);
	//��������Ч��
	//_addTooltipFieldUseResultTriggerSkill(kFieldVector);

	//ʹ�ô���
	//_addTooltipFieldUseCount(kFieldVector);

	//ʣ��ʱ��
	//_addTooltipFieldRemainTime(kFieldVector);

	//��Ʒ��������
	//_addTooltipFieldOverlapCount(kFieldVector);

	//��Ʒ����
	//_addTooltipFieldDescript(kFieldVector);

	//��ϸ����
	_addTooltipDetailDescript(kFieldVector);

	//��Χ����
	_addTooltipFieldAtmosDescript(kFieldVector);

	//��������
	_addTooltipFieldUseCount(kFieldVector);

	//��Ʒ�ȼ�����
	_addTooltipFieldLevelLimit<CF_ItemUseLimitFunc::DataEntry>(kFieldVector);

	//������.
	_addTooltipFieldBind(kFieldVector);

	//�۸�
	_addTooltipFieldPrice(kFieldVector);
	//���ṱ��
	_addTooltipFieldGuildCash(kFieldVector);

	//ʱ������
	//_addTooltipFieldTimeLimitDescript(kFieldVector);

	if(!m_bShopOpen)
	{
		//�����ʾ
		_addTooltipSplitInfo(kFieldVector);
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

	//��ʽ��tooltip
	_formatTooltip(kFieldVector);

	return true;
}