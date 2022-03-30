//活动的tooltip数据组织

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
		//构造活动名称字段
		bool _addTooltipFieldGameStoryName(TooltipFieldVector& kFieldVector);
		//boss品质
		bool _addTooltipFieldBossDesc(TooltipFieldVector& kFieldVector);
		//boss等级
		bool _addTooltipFieldBossLevel(TooltipFieldVector& kFieldVector);
		//构造玩法说明标题字段
		bool _addTooltipFieldPlayRuleTitle(TooltipFieldVector& kFieldVector);
		//构造玩法说明内容字段
		bool _addTooltipFieldPlayRule(TooltipFieldVector& kFieldVector);
		//构造携带宝物标题字段
		bool _addTooltipFieldRewardTitle(TooltipFieldVector& kFieldVector);
		//构造携带宝物内容字段
		bool _addTooltipFieldReward(TooltipFieldVector& kFieldVector);
		//构造刷新时间标题字段
		bool _addTooltipFieldResetTimeTitle(TooltipFieldVector& kFieldVector);
		//构造刷新时间内容字段
		bool _addTooltipFieldResetTime(TooltipFieldVector& kFieldVector);
		//构造刷新地图标题字段
		bool _addTooltipFieldResetMapTitle(TooltipFieldVector& kFieldVector);
		//构造刷新地图内容字段
		bool _addTooltipFieldResetMap(TooltipFieldVector& kFieldVector);
		//构造boss驻守地图标题
		bool _addTooltipFieldMapTitle(TooltipFieldVector& kFieldVector);
		//构造boss驻守地图内容
		bool _addTooltipFieldMap(TooltipFieldVector& kFieldVector);
		//构造基本属性标题
		bool _addTooltipFieldBaseAttrTitle(TooltipFieldVector& kFieldVector);
		//构造基本属性内容
		bool _addTooltipFieldBaseAttr(TooltipFieldVector& kFieldVector);
		//构造提示信息字段
		bool _addTooltipFieldGameStoryTip(TooltipFieldVector& kFieldVector);
	private:
		GameStoryInfo *m_pkGameStoryInfo;
		CSVFile::CF_GameStory::DataEntry *m_pkInfoEntry;
		CSVFile::CF_GameStoryRaidList::DataEntry *m_pStoryRaidInfoEntry;
		int		m_iTotalCost;
	};
}

#endif
