#ifndef _LC_SREVER_TASK_H
#define _LC_SREVER_TASK_H

#include "LC_Task.h"
#include "LC_TaskMap.h"
#include "LC_ServerItemFlopManager.h"

namespace GameLogic
{
	class LC_PlayerBase;
}

class LC_ServerPlayer;
class LC_ServerNPC;

class LC_ServerTask : public GameLogic::LC_Task
{
public:
	LC_ServerTask();
	virtual ~LC_ServerTask();

	DECL_RTTI_NAME(LC_ServerTask)

	virtual void		ResolveParams(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry);

	virtual void		RecordTaskOSS(int iType, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry);

	virtual void		NotifyEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	
	//为了解决普通物品与任务物品掉落问题，并且减少逻辑事件前置带来的影响，将任务物品掉落从怪物死亡事件中分离
	virtual void        GenerateTaskItemFlopInfoOnNPCDead(LC_ServerNPC* pkNPC, GameLogic::LC_PlayerBase* pkPlayer
											, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry,LC_ItemFlopInfoList& rkFlopInfoList);

	//吞钱的操作
	ResultType			ReduceDiffStar(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry);
	ResultType			PromoteRewardStar(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry);
	ResultType			OneKeyOperationQuickFinish(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry);
	ResultType			OneKeyOperationMultiFinish(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry);
	ResultType			OneKeyOperationComboFinish(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry);

protected:	
	virtual void		_taskFininshGuildOperation(GameLogic::LC_PlayerBase* pkPlayer);

	void _ProcessControllerLevel(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessPresentBuddyElem(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessRaidMapTimes(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessAliveBuddyNum(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessKillBossDropTimes(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessTitleLevel(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessAddGuild(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessTreasureTimes(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessVipExtareReward(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessVIPUpLevel(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessChallengeWinEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessLoginInEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessGatherTaskNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void _ProcessKillControllerEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessTalkToNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessKillNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void 	_ProcessKillGroupNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessTaskAreaEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessPickItemEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessTimeClickEvent(GameLogic::LC_GameEvent* pkGameEvent,GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessNPCArriveEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessUseItemEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessPlayerDieEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessSkillExecuteEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessTranscriptionEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessFinishTaskEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessStoryProcessEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessChallengeEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessTotalAddCashEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessAddCashEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessReduceCashEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessSPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessComboEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessItemIdentifyEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessKillEliteEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessItemGotEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessItemSwitchEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessCommonStatOPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessActiStatOPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessTreasureChestOPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessSigninEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessGuildOfferEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessGuildKillEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessItemOperationEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);
	void	_ProcessSkillLevelUPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter);

	virtual void    _handleBonusItem(GameLogic::LC_PlayerBase* pkPlayer, int32_t iMulti, int32_t addExtraReward);
};
#endif	