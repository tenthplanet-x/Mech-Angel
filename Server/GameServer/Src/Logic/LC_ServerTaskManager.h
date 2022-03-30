#ifndef _LC_SERVER_TASK_MANAGER_H
#define _LC_SERVER_TASK_MANAGER_H

#include "SystemDefine.h"
#include <cstring>
#include "LC_GameEvent.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_Task.h"
#include "LC_ServerNPC.h"
#include "LC_ServerPlayer.h"
#include "LC_TaskMap.h"
#include "LC_ServerTask.h"
#include "LC_TaskManager.h"

//--------------------------------------------------------
enum TaskTimeTriggerType
{
	TASK_TIME_CLICK		= 0,
	TASK_TIME_DESTINE	= 1,
};
//--------------------------------------------------------
struct UpdateTaskInfo
{
	UpdateTaskInfo()
	{
		m_iTimeTriggerType = TASK_TIME_CLICK;
		m_lTaskDataIndex   = -1;
		m_fLastUpdateTime  = 0.0f;
		m_fLeftTime		   = 0.0f;
	}

	int		m_lTaskDataIndex;
	float	m_fLastUpdateTime;
	float	m_fLeftTime;
	int		m_iTimeTriggerType;
};
//--------------------------------------------------------
struct TaskAssignCacheData
{
	TaskAssignCacheData();
	void Reset();
	bool AddCacheTask(int lTaskID);

	int m_iCacheCount;
	int m_lCacheTaskID[MAX_ASSIGN_TASK_CACHE_COUNT];

};
//--------------------------------------------------------
class LC_ServerTaskManager :public GameLogic::LC_TaskManager
{
public:
	LC_ServerTaskManager();
	virtual ~LC_ServerTaskManager();

	virtual bool					Init();
	virtual bool					UnInit();

	
	virtual void					DispatchGameEvent(GameLogic::LC_GameEvent* pkEvent);
	void							GenerateTaskItemFlopInfoOnNPCDead(LC_ItemFlopInfoList& rkFlopInfoList, LC_ServerNPC* pkNPC);
	ResultType				AssignTask(LC_ServerPlayer* pkPlayer, uint32_t uiAssignType, bool refreshByClient=false, int assignParam1=NONE_ASSIGN, int assignParam2=NONE_ASSIGN, int assignParam3=NONE_ASSIGN);
	void							ReplaceAssignTask(LC_ServerPlayer* pkPlayer, uint32_t uiAssignType, int32_t iTaskID);

	void CheckResetBestGuildDailyTask(LC_ServerPlayer* pkPlayer);

	void CancelBestGuildTaskDailyTask(LC_ServerPlayer* pkPlayer, int32_t storyID);

	void							CheckReset(LC_ServerPlayer* pkPlayer, uint32_t curTimeStamp);
	//void							CheckRefreshRewardTask(LC_ServerPlayer* pkPlayer, uint32_t curTimeStamp);

	void							ClearPostTask(LC_ServerPlayer* pkPlayer, GameLogic::LC_Task* pkTask);
	void							ResetTaskTree(LC_ServerPlayer* pkPlayer, int lTaskID);

	ResultType						Phagotrophy(LC_ServerPlayer* pkPlayer, int lPhagotrophID, int lPhagocytableID, int iType, float& propo);

	bool							AssignTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID);
	void							ClearTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID);
	void							ClearAssignTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID);
	void							ClearProcessTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID);

	const mem::vector<int32_t>& GetEveryDayTaskIDs();
private:
	bool				_initTaskData();
	
	void				_notifyEventToTask(GameLogic::LC_GameEvent* pkEvent, object_id_type lID);

	void		_dispatchDefaultEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerDieEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerKillNPCEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerChallengeWinEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerVipLevelUpEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPresentBuddyElemEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchRaidMapEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchAliveBuddyNumEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchKillBossDropTimesEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchTitleLevelEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchAddGuildEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchTreasureTimesEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchVipExtareRewardEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerLoginInEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerGatherTaskNpcEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchKillControllerEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerKillEliteEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerPickItemEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerArriveAreaEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchTaskTimeTickEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchNPCArriveAreaEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchTalkToNPCEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchUseItemEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchSelectCareerEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchEquipItemEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchLearnSkillEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchSkillLevelUpEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchLearnFormulaEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchChangeSystemTitleEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchPlayerLevelUpEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchSkillExecuteEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchLeaveGroupEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchDismissGroupEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchTranscriptionEvent(GameLogic::LC_GameEvent* pkEvent);
	void		_dispatchGuildKillEvent(GameLogic::LC_GameEvent* pkEvent);

	GameLogic::LC_EventHandler2TaskIndices			m_kEvent2Tasks;

	mem::vector<int32_t> m_nEveryDayTaskIDs;//每日任务的id（每日凌晨从已完成列表中删除，以便第二天可接取）
};
#endif