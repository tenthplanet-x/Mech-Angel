//���tooltip������֯

#ifndef _LC_GAMESTORYMTOOLTIP_H
#define _LC_GAMESTORYMTOOLTIP_H


#include "LC_TooltipBase.h"
#include "CF_GameStoryRaidList.h"
#include "CF_GameStoryClientUI.h"
#include "LC_ClientGameStoryManager.h"
namespace GameLogic
{
	class LC_GameStoryTooltip : public LC_TooltipBase
	{
	public:
		LC_GameStoryTooltip(void);
		virtual ~LC_GameStoryTooltip(void);

		virtual bool BuildTooltip();
		void Init(int iGameStoryID, int iTotalCost, int iRaidInfoID, GameStoryInfo *pkGameStoryInfo);

	protected:
		//���������ֶ�
		bool _addTooltipFieldGameStoryName(TooltipFieldVector& kFieldVector);
		//bossƷ��
		bool _addTooltipFieldBossDesc(TooltipFieldVector& kFieldVector);
		//boss�ȼ�
		bool _addTooltipFieldBossLevel(TooltipFieldVector& kFieldVector);
		//�����淨˵�������ֶ�
		bool _addTooltipFieldPlayRuleTitle(TooltipFieldVector& kFieldVector);
		//�����淨˵�������ֶ�
		bool _addTooltipFieldPlayRule(TooltipFieldVector& kFieldVector);
		//����Я����������ֶ�
		bool _addTooltipFieldRewardTitle(TooltipFieldVector& kFieldVector);
		//����Я�����������ֶ�
		bool _addTooltipFieldReward(TooltipFieldVector& kFieldVector);
		//����ˢ��ʱ������ֶ�
		bool _addTooltipFieldResetTimeTitle(TooltipFieldVector& kFieldVector);
		//����ˢ��ʱ�������ֶ�
		bool _addTooltipFieldResetTime(TooltipFieldVector& kFieldVector);
		//����ˢ�µ�ͼ�����ֶ�
		bool _addTooltipFieldResetMapTitle(TooltipFieldVector& kFieldVector);
		//����ˢ�µ�ͼ�����ֶ�
		bool _addTooltipFieldResetMap(TooltipFieldVector& kFieldVector);
		//����bossפ�ص�ͼ����
		bool _addTooltipFieldMapTitle(TooltipFieldVector& kFieldVector);
		//����bossפ�ص�ͼ����
		bool _addTooltipFieldMap(TooltipFieldVector& kFieldVector);
		//����������Ա���
		bool _addTooltipFieldBaseAttrTitle(TooltipFieldVector& kFieldVector);
		//���������������
		bool _addTooltipFieldBaseAttr(TooltipFieldVector& kFieldVector);
		//������ʾ��Ϣ�ֶ�
		bool _addTooltipFieldGameStoryTip(TooltipFieldVector& kFieldVector);
	private:
		GameStoryInfo *m_pkGameStoryInfo;
		CSVFile::CF_GameStory::DataEntry *m_pkInfoEntry;
		CSVFile::CF_GameStoryRaidList::DataEntry *m_pStoryRaidInfoEntry;
		int		m_iTotalCost;
	};
}

#endif
