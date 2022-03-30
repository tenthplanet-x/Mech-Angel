#include "LC_ServerTaskManager.h"
#include "LC_ServerMainTask.h"
#include "LC_ServerTaskMap.h"
#include "LC_PlayerBase.h"
#include "LC_TaskMap.h"
#include "LC_ServerNPC.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPCManager.h"
#include "UT_Timer.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerGuildManager.h"
#include "LS_LuaStateFactory.h"
#include "LC_ServerMap.h"
#include "LC_GameEventManager.h"
#include "LC_NPCTask.h"
#include "LC_ServerTask.h"
#include "CF_Biography.h"
#include "UT_Redis.h"
#include "CF_TaskCondition.h"
#include "CF_BestGuild.h"
#include "CF_BestGuildTask.h"
#include "CF_BestGuildDailyTask.h"
using namespace GameLogic;
using namespace CSVFile;
using namespace Utility;

//--------------------------------------------------------
TaskAssignCacheData::TaskAssignCacheData()
{
	Reset();
}

void TaskAssignCacheData::Reset()
{
	m_iCacheCount = 0;
	memset(m_lCacheTaskID, 0, sizeof(m_lCacheTaskID));
};

bool TaskAssignCacheData::AddCacheTask(int lTaskID)
{
	if (m_iCacheCount >= 0 && m_iCacheCount < MAX_ASSIGN_TASK_CACHE_COUNT)
	{
		m_lCacheTaskID[m_iCacheCount] = lTaskID;
		++m_iCacheCount;
		return true;
	}
	return false;
}

//--------------------------------------------------------
LC_ServerTaskManager::LC_ServerTaskManager(): LC_TaskManager()
{
}

LC_ServerTaskManager::~LC_ServerTaskManager()
{
}

bool LC_ServerTaskManager::Init()
{
	LC_TaskManager::Init();
	_initTaskData();
	return true;
}

bool LC_ServerTaskManager::UnInit()
{
	m_kNormalTaskIDPtrMap.clear();
	m_kBillBoardNPCTypeIDList.clear();
	m_kTP2TaskIDs.clear();
	
	for (int i=0; i<STT_MAX_TASK; ++i)
	{
		m_kLVL2TaskSets[i].clear();
	}

	TaskIDPtrMapIter IterExecute;
	TRAV(m_kAllTaskIDPtrMap, IterExecute)
	{
		LC_Task* pkTask = IterExecute->second;
		T_SAFE_DELETE(pkTask);
	}

	NPCTaskPtrMapIter IterNPC;
	TRAV(m_kNPCTaskMap, IterNPC)
	{
		LC_NPCTask* pkNPCTask = IterNPC->second;
		T_SAFE_DELETE(pkNPCTask);
	}

	MainTaskIDPtrMapIter IterMain;
	TRAV(m_kMainTaskIDPtrMap, IterMain)
	{
		LC_MainTask* pkMainTask = IterMain->second;
		T_SAFE_DELETE(pkMainTask);
	}
	return true;
}

const mem::vector<int32_t>& LC_ServerTaskManager::GetEveryDayTaskIDs()
{
	return m_nEveryDayTaskIDs;
}

bool LC_ServerTaskManager::_initTaskData()
{
	LS_LuaState* pkLuaState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	if (!pkLuaState)
	{
		return false;
	}
	m_kBillBoardNPCTypeIDList.clear();

	m_nEveryDayTaskIDs.clear();
	m_nEveryDayTaskIDs.reserve(30);

	CF_TaskList* pkTaskList = CF_TaskList::GetSingletonPtr();
	CF_TaskList::DataEntryMapExternIterator kIter = pkTaskList->GetIterator();
	while (kIter.HasMoreElements())
	{
		CF_TaskList::DataEntry* pkData = kIter.PeekNextValuePtr();
		{
			int lTaskID = pkData->_iID;
			LC_ServerTask* pkTask = T_NEW_D LC_ServerTask();
			pkTask->Init(pkLuaState, pkData);

			if(STT_EVERYDAY_TASK == pkData->_iTaskType)
			{
				m_nEveryDayTaskIDs.push_back(lTaskID);
			}

			TaskIDPtrMap::_Pairib _pb = m_kAllTaskIDPtrMap.insert(std::make_pair(lTaskID, pkTask));
			if (!_pb.second)
				return false;

			GameLogic::LC_TaskEventHandlerSet kHandlers;
			pkTask->GatherEventHandler(kHandlers);

			GameLogic::LC_TaskEventHandlerSet::iterator itEHandler = kHandlers.begin();
			for (; itEHandler != kHandlers.end(); ++itEHandler)
			{
				m_kEvent2Tasks[*itEHandler].push_back(pkTask);
			}

			//常规任务自动检测可否接受
			int iTaskType = pkTask->GetTaskType();
			if (iTaskType == STT_MAIN_SCENARIO || iTaskType == STT_BRANCH_SCENARIO || iTaskType == STT_DAILY_TASK || iTaskType == STT_INVISIBLE_TASK)
			{
				TaskIDPtrMapIter NormalMapIter = m_kNormalTaskIDPtrMap.find(lTaskID);
				if (NormalMapIter == m_kNormalTaskIDPtrMap.end())
				{
					m_kNormalTaskIDPtrMap[lTaskID] = pkTask;
				}
			}
			m_kTP2TaskIDs[iTaskType].push_back(lTaskID);
			if (iTaskType>=0 && iTaskType<STT_MAX_TASK)
			{
				m_kLVL2TaskSets[iTaskType][pkTask->GetTaskStarLevel()][pkData->_levelMin].insert(lTaskID);
			}

			//加入NPC 任务信息列表
			int lAcceptNPCID = pkTask->GetAcceptNPCID();
			LC_NPCTask* pkNPCTask = NULL;
			NPCTaskPtrMapIter AcceptIter = m_kNPCTaskMap.find(lAcceptNPCID);
			if (AcceptIter == m_kNPCTaskMap.end())
			{
				pkNPCTask = T_NEW_D LC_NPCTask();
				m_kNPCTaskMap[lAcceptNPCID] = pkNPCTask;
			}
			else
			{
				pkNPCTask = AcceptIter->second;
			}
			pkNPCTask->AddTaskToCanAcceptTaskList(lTaskID);

			int lFinishNPCID = pkTask->GetFinishNPCID();
			pkNPCTask = NULL;
			NPCTaskPtrMapIter FinishIter = m_kNPCTaskMap.find(lFinishNPCID);
			if (FinishIter == m_kNPCTaskMap.end())
			{
				pkNPCTask = T_NEW_D LC_NPCTask();
				m_kNPCTaskMap[lFinishNPCID] = pkNPCTask;
			}
			else
			{
				pkNPCTask = FinishIter->second;
			}
			pkNPCTask->AddTaskToCanFinishTaskList(lTaskID);
		}
		kIter.MoveNext();
	}

	CF_MainTaskList* pkMainTaskList = CF_MainTaskList::GetSingletonPtr();
	CF_MainTaskList::DataEntryMapExternIterator kMainIter = pkMainTaskList->GetIterator();
	while (kMainIter.HasMoreElements())
	{
		CF_MainTaskList::DataEntry* pkData = kMainIter.PeekNextValuePtr();
		int lMainTaskID = pkData->_iID;
		LC_ServerMainTask* pkTask = T_NEW_D LC_ServerMainTask();
		pkTask->Init(pkLuaState, pkData);
		m_kMainTaskIDPtrMap[lMainTaskID] = pkTask;
		kMainIter.MoveNext();
	}
	_initTaskPostInfo();
	return true;
}

void LC_ServerTaskManager::GenerateTaskItemFlopInfoOnNPCDead(LC_ItemFlopInfoList& rkFlopInfoList, LC_ServerNPC* pkNPC)
{
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();

	LC_ServerNPCOwnerList& rkOwnerList = pkNPC->GetNPCOwnerList();
	int  lOwnerCount = rkOwnerList.GetOwnerCount();
	if (lOwnerCount <= 0)
	{
		return;
	}
	LC_EventHandler2TaskIndices::iterator it = m_kEvent2Tasks.find(LC_TaskEventHandler(GLET_KILL, pkNPC->GetCharType()));
	if (it == m_kEvent2Tasks.end())
	{
		return;
	}

	LC_TaskPtrs& kTaskPtrs = it->second;
	mem::vector<LC_PlayerGroupBase*> kGroupList;
	for (int i = 0; i < lOwnerCount; ++i)
	{
		object_id_type lHarmPlayerID = rkOwnerList.GetOwnerByIndex(i);
		LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(lHarmPlayerID);
		if (NULL == pkPlayer)
		{
			continue;
		}
		LC_ServerTaskMap* pkTaskMap  = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
		if (NULL == pkTaskMap)
		{
			continue;
		}
		pkTaskMap->GenerateTaskItemFlopInfoOnNPCDead(rkFlopInfoList, pkNPC, kTaskPtrs);
		LC_PlayerGroupBase* pkGroup = pkPlayer->GetPlayerGroup();
		if (NULL == pkGroup)
		{
			continue;
		}
		bool bInsert = false;
		for (int j = 0 ; j < (int)kGroupList.size(); ++j)
		{
			LC_PlayerGroupBase* pkTempGroup = kGroupList[j];
			if (pkTempGroup && pkTempGroup->GetID() == pkGroup->GetID())
			{
				bInsert = true;
			}
		}
		if (!bInsert)
		{
			kGroupList.push_back(pkGroup);
		}
	}
	for (int j = 0 ; j < (int)kGroupList.size(); ++j)
	{
		LC_PlayerGroupBase* pkTempGroup = kGroupList[j];
		if (pkTempGroup)
		{
			pkTempGroup->IncreaseTaskItemAssignStep();
		}
	}
}

struct LC_TaskPtrSorter
{
	bool operator()(const LC_Task* pTA, const LC_Task* pTB)
	{
		return pTA->GetDepth() < pTB->GetDepth();
	}
};

struct LC_TaskPreTaskIDSorter
{
	bool operator()(const LC_Task* pTA, const LC_Task* pTB)
	{
		return pTA->GetPreTaskID() > pTB->GetPreTaskID();
	}
};

struct LC_TaskRateSorter
{
	bool operator()(const LC_Task* pTA, const LC_Task* pTB)
	{
		return pTA->GetRate() < pTB->GetRate();
	}
};

ResultType LC_ServerTaskManager::AssignTask(LC_ServerPlayer* pkPlayer, uint32_t uiAssignType, bool refreshByClient, int assignParam1, int assignParam2, int assignParam3)
{
	ResultType res = RE_FAIL;
	if (pkPlayer == NULL)
	{
		return res;
	}

	LC_ServerTaskMap* pkTaskMap  = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return res;
	}
	LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();

	LC_TaskAssignMap::iterator  it = m_kTaskAssignMap.find(uiAssignType);
	if (it != m_kTaskAssignMap.end())
	{
		//现在只一种吧
		uint32_t uiAssignType = it->first;
		switch (it->second._iAssignRule)
		{
			case LC_TASK_AS_RL_RANDOM:
			case LC_TASK_AS_RL_SEQUENCE:
				break;
			case LC_TASK_AS_RL_ALL:
				break;
			case LC_TASK_AS_RL_PREMAX:
				break;
			case LC_TASK_AD_RL_RANDOM_WEIGHT:
				break;
			case LC_TASK_AS_RL_GUILD:
				{
					if (!pkPlayer->IsExistGuild())
					{
						return res;
					}
				}
				break;
			case LC_TASK_AS_RL_STORY:
			default:
				{
					return res;
				}
				break;
		}
		
		uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		LC_AssignTaskData*	assignData = pkAssignInfo->AddAssignType(uiAssignType, timestamp);
		if(NULL == assignData)
			return res;

		LC_TaskManager* pkTaskManager = LC_TaskManager::GetSingletonPtr();
		if (pkTaskManager == NULL)
			return res;

		mem::vector<int> assignTask;		
		if(it->second._iTaskType==STT_REWARD_TASK && refreshByClient==true)
		{
			//玩家请求刷新悬赏任务
			TaskIDSet assignTaskIDs = assignData->GetAssignTaskIDs();
			for(TaskIDSet::iterator it=assignTaskIDs.begin(); it!=assignTaskIDs.end(); ++it)
			{
				bool tag = pkPlayer->IsInProcessingTaskMap(*it);
				if(tag)
				{
					assignTask.push_back(*it);
				}
				else
				{
					pkAssignInfo->RemoveTask(uiAssignType, *it, true);					
				}
			}
		}

		int iCount = 0;
		if (RE_SUCCESS != assignData->IsAssignAvailable(&it->second, iCount) || iCount <= 0)
		{
			return res;
		}

		if(it->second._iTaskType==STT_REWARD_TASK && refreshByClient==true)
		{
			iCount += assignTask.size();
		}

		TaskIDVec id_vec;
		//如果刷新的是角色传记任务
		//根据BiographyTask表的配置，派发固定的任务
		if(it->second._iTaskType == STT_BIOGRAPHY_TASK)
		{
			mem::vector<int32_t> biographyPartID;
			switch(assignParam1)
			{
			case BUDDY_ASSIGN:
			case PRETASK_ASSIGN:
			case PREBIOGRAPHY_ASSIGN:
				pkPlayer->GetAssignBiographyPartIDByParam(biographyPartID, assignParam1, assignParam2);
				for(int i=0; i<biographyPartID.size(); ++i)
				{
					pkPlayer->GetBiographyTaskID(biographyPartID[i], id_vec);
				}
				break;
			case FIRST_ASSIGN:
				pkPlayer->GetFirstAssignBiographyPartID(biographyPartID);
				for(int i=0; i<biographyPartID.size(); ++i)
				{
					pkPlayer->GetBiographyTaskID(biographyPartID[i], id_vec);
				}	
				break;
			default:
				break;
			}

			if(assignParam3 != NONE_ASSIGN)
			{
				int nextBiographyID = 0;
				bool tag = pkPlayer->GetNextBiographyPartID(assignParam3, nextBiographyID);
				if(tag && nextBiographyID!=0)
				{
					biographyPartID.push_back(nextBiographyID);
					pkPlayer->GetBiographyTaskID(nextBiographyID, id_vec);
				}
			}
		}
		else
		{
			id_vec = GetTaskIDs(it->second._iTaskType);
		}
		
		LC_TaskIndices kUnspecialTasks;
		kUnspecialTasks.clear();
		LC_TaskIndices kRoundAppointedTasks;
		kRoundAppointedTasks.clear();

		for (TaskIDVec::const_iterator itid = id_vec.begin(); itid != id_vec.end(); ++itid)
		{
			int iTaskID = *itid;
			LC_Task* pkTask = GetTask(iTaskID);
			if (NULL == pkTask)
			{
				continue;
			}

			if(STT_BIOGRAPHY_TASK == pkTask->GetTaskType())
			{
				//角色传记任务，已完成的任务不能再派发
				if(pkTaskMap->IsInFinishedTaskMap(iTaskID))
				{
					continue;
				}
			}
			else
			{
				//环任务  做特殊处理  使可重复接
				if (pkTaskMap->IsInFinishedTaskMap(iTaskID))
				{
					pkTaskMap->RemoveTaskFinishRecord(iTaskID);
				}
			}
			
			if (pkTask->GetRoundAppointed() > 0 && assignData->GetRound() + 1 != (uint32_t)pkTask->GetRoundAppointed())
			{
				continue;
			}
			//if (RE_TASK_START_SUCCESS != pkTask->CanStart(pkPlayer, false))
			if (RE_SUCCESS != pkTask->CanAssign(pkPlayer))			
			{
				continue;
			}
			if (pkTask->GetRoundAppointed() > 0)
			{
				kRoundAppointedTasks[pkTask->GetAcceptLevel()].push_back(pkTask);
			}
			else
			{
				kUnspecialTasks[pkTask->GetAcceptLevel()].push_back(pkTask);
			}
		}
		LC_TaskIndices* pkAvailTasks = NULL;
		if (kRoundAppointedTasks.empty())
		{
			pkAvailTasks = &kUnspecialTasks;
		}
		else
		{
			pkAvailTasks = &kRoundAppointedTasks;
		}

		if(pkAvailTasks->empty())
		{
			return RE_FAIL;
		}

		int32_t iProperLevel = 0;
		LC_TaskIndices::iterator itIndex = pkAvailTasks->begin();
		for (; itIndex != pkAvailTasks->end(); ++itIndex)
		{
			iProperLevel = UT_MathBase::MaxInt(iProperLevel, itIndex->first);
		}

		LC_TaskPtrs& kAvailTaskList = pkAvailTasks->operator [](iProperLevel);
		if (!kAvailTaskList.empty())
		{
			std::sort(kAvailTaskList.begin(), kAvailTaskList.end(), LC_TaskPtrSorter());
			int sz = UT_MathBase::MinInt((int)kAvailTaskList.size(), it->second._iAssignPoolSize);
			switch (it->second._iAssignRule)
			{
				case LC_TASK_AS_RL_RANDOM:
					{
						std::random_shuffle(kAvailTaskList.rbegin(), kAvailTaskList.rbegin() + sz);
						LC_TaskPtrs::reverse_iterator itptr = kAvailTaskList.rbegin();
						for (int i = 0; itptr != kAvailTaskList.rend() && i < iCount && i < sz; ++itptr, ++i)
						{
							pkAssignInfo->AddTask(uiAssignType, (*itptr)->GetTaskID());
						}
						pkAssignInfo->SetRound(uiAssignType, assignData->GetRound() + 1);
					}
					break;
				case LC_TASK_AS_RL_GUILD:
					{
						/*
						time_t _tm = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
						tm* _ltm = localtime(&_tm);
						int _base = pkPlayer->GetGuildID() + _ltm->tm_mday * 2 + (int)UT_MathBase::Pow((float)assignData->GetRound(), 2.0);
						int _size = (int)kAvailTaskList.size();
						for (int i = 0; i < iCount && i < _size; ++i)
						{
							LC_Task* pkSel = kAvailTaskList[(_base + i) % _size];
							pkAssignInfo->AddTask(uiAssignType, pkSel->GetTaskID());
						}
						pkAssignInfo->SetRound(uiAssignType, assignData->GetRound() + 1);
						*/
						std::sort(kAvailTaskList.begin(), kAvailTaskList.end(), LC_TaskPreTaskIDSorter());
						int maxPreTaskID = kAvailTaskList[0]->GetPreTaskID();
						LC_TaskPtrs taskVec;
						for(LC_TaskPtrs::iterator it=kAvailTaskList.begin(); it!=kAvailTaskList.end(); ++it)
						{
							int preTaskID = (*it)->GetPreTaskID();
							if(preTaskID == maxPreTaskID)
							{
								//再检查历练
								const mem::vector<int>& preAchievement = (*it)->GetPreAchievement();
								for(uint32_t i=0; i<preAchievement.size(); ++i)
								{
									ResultType res = pkPlayer->CheckAchievementCanFinish(preAchievement[i]);
									if(RE_ACHIEVEMENT_REWARD_HAS_RECEIVED!=res && RE_SUCCESS!=res)
									{
										continue;
									}
								}

								taskVec.push_back(*it);
							}
						}

						int num = taskVec.size();
						std::random_shuffle(taskVec.begin(), taskVec.end());
						LC_TaskPtrs::iterator itptr = taskVec.begin();
						for (int i = 0; itptr != taskVec.end() && i < iCount && i < num; ++itptr, ++i)
						{
							pkAssignInfo->AddTask(uiAssignType, (*itptr)->GetTaskID());
						}
						pkAssignInfo->SetRound(uiAssignType, assignData->GetRound() + 1);
					}
					break;
				case LC_TASK_AS_RL_SEQUENCE:
					{
						LC_Task* pkSel = kAvailTaskList[assignData->GetRound() % kAvailTaskList.size()];
						if (pkSel)
						{
							pkAssignInfo->AddTask(uiAssignType, pkSel->GetTaskID());
						}
						pkAssignInfo->SetRound(uiAssignType, assignData->GetRound() + 1);
					}
					break;
				case LC_TASK_AS_RL_ALL:
					{
						for(int i=0; i<kAvailTaskList.size(); ++i)
						{
							LC_Task* pkSel = kAvailTaskList[i];
							if (pkSel)
							{
								pkAssignInfo->AddTask(uiAssignType, pkSel->GetTaskID());
							}
						}						
					}
					break;
				case LC_TASK_AS_RL_PREMAX:
					{
						std::sort(kAvailTaskList.begin(), kAvailTaskList.end(), LC_TaskPreTaskIDSorter());
						int maxPreTaskID = kAvailTaskList[0]->GetPreTaskID();
						LC_TaskPtrs taskVec;
						for(LC_TaskPtrs::iterator it=kAvailTaskList.begin(); it!=kAvailTaskList.end(); ++it)
						{
							int preTaskID = (*it)->GetPreTaskID();
							if(preTaskID == maxPreTaskID)
							{
								//再检查历练
								const mem::vector<int>& preAchievement = (*it)->GetPreAchievement();
								for(uint32_t i=0; i<preAchievement.size(); ++i)
								{
									ResultType res = pkPlayer->CheckAchievementCanFinish(preAchievement[i]);
									if(RE_ACHIEVEMENT_REWARD_HAS_RECEIVED!=res && RE_SUCCESS!=res)
									{
										continue;
									}
								}

								taskVec.push_back(*it);
							}
						}

						int num = taskVec.size();
						std::random_shuffle(taskVec.begin(), taskVec.end());
						LC_TaskPtrs::iterator itptr = taskVec.begin();
						for (int i = 0; itptr != taskVec.end() && i < iCount && i < num; ++itptr, ++i)
						{
							pkAssignInfo->AddTask(uiAssignType, (*itptr)->GetTaskID());
						}
						pkAssignInfo->SetRound(uiAssignType, assignData->GetRound() + 1);
					}
					break;
				case LC_TASK_AD_RL_RANDOM_WEIGHT:
					{
						std::sort(kAvailTaskList.begin(), kAvailTaskList.end(), LC_TaskRateSorter());
						LC_TaskPtrs taskVec;
						int totalWeight = 0;
						for(LC_TaskPtrs::iterator it=kAvailTaskList.begin(); it!=kAvailTaskList.end(); ++it)
						{
							int weight = (*it)->GetRate();
							if(weight > 0)
							{
								totalWeight += weight;
								taskVec.push_back(*it);
							}			
						}

						while(assignTask.size() < iCount)
						{
							int total = 0;
							int random = UT_MathBase::RandInRangeInt(0, totalWeight);
							for(LC_TaskPtrs::iterator itptr=taskVec.begin(); itptr!=taskVec.end(); ++itptr)
							{
								int rate = (*itptr)->GetRate();
								total += rate;
								int taskID = (*itptr)->GetTaskID();
								if(total>random && find(assignTask.begin(), assignTask.end(), taskID)==assignTask.end())
								{
									assignTask.push_back(taskID);
									break;
								}				
							}
						}

						for(int i=0; i<assignTask.size(); ++i)
						{
							pkAssignInfo->AddTask(uiAssignType, assignTask[i]);
						}
					}
					break;
				default:
					break;
			}
		}
	}
	return RE_SUCCESS;
}

void LC_ServerTaskManager::ReplaceAssignTask(LC_ServerPlayer* pkPlayer, uint32_t uiAssignType, int iTaskID)
{
	if (pkPlayer == NULL)
	{
		return;
	}
	LC_ServerTaskMap* pkTaskMap  = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return;
	}
	LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
	pkAssignInfo->RemoveTask(uiAssignType, iTaskID);

	//悬赏任务和最强公会任务有自己的派发机制，replace的时候，不做刷新
	if(uiAssignType == 7 || uiAssignType==12 || uiAssignType==13)
		return;

	AssignTask(pkPlayer, uiAssignType);
}

void LC_ServerTaskManager::CheckResetBestGuildDailyTask(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer->ResetBestGuildTaskDailyTask();
}

void LC_ServerTaskManager::CancelBestGuildTaskDailyTask(LC_ServerPlayer* pkPlayer, int32_t storyID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if(NULL == pkTaskMap)
	{
		return;
	}

	CF_BestGuildDailyTask::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BestGuildDailyTask>(storyID);
	if(NULL == pkData)
	{
		return;
	}

	for(Utility::UT_SIMDataList::iterator kIt=pkData->_sDailyTaskID2.begin(); kIt!=pkData->_sDailyTaskID2.end(); ++kIt)
	{
		int taskID = kIt->IID();
		bool tag = pkPlayer->IsInProcessingTaskMap(taskID);
		if(tag)
		{
			pkPlayer->CancelTask(taskID);
		}

		tag = pkPlayer->IsInAssignTaskMap(taskID);
		if(tag)
		{
			pkPlayer->RemoveAssignTaskByTaskID(taskID);
		}
	}
}

void LC_ServerTaskManager::CheckReset(LC_ServerPlayer* pkPlayer, uint32_t curTimeStamp)
{
	if (pkPlayer)
	{
		LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
		for (int i = STT_MAIN_SCENARIO; i < STT_MAX_TASK; ++i)
		{
			pkTaskMap->CheckReset(curTimeStamp, i, false, true);
		}

		//STT_BESTGUILD_DAILYTASK
		CheckResetBestGuildDailyTask(pkPlayer);

		LC_OpenAbilityAsset& kOABAsset = pkPlayer->GetOpenAbilityAsset();
		if (kOABAsset.IsAbilityEnabled(LC_ABT_TASK, STT_DAILY_TASK))
		{
			AssignTask(pkPlayer, GetAssignType(STT_DAILY_TASK));
		}
		if (pkPlayer->IsExistGuild() && kOABAsset.IsAbilityEnabled(LC_ABT_TASK, STT_GUILD_TASK))
		{
			AssignTask(pkPlayer, GetAssignType(STT_GUILD_TASK));
		}
		if (kOABAsset.IsAbilityEnabled(LC_ABT_TASK, STT_FRONTIER))
		{
			AssignTask(pkPlayer, GetAssignType(STT_FRONTIER));
		}
	}
}

/*
void LC_ServerTaskManager::CheckRefreshRewardTask(LC_ServerPlayer* pkPlayer, uint32_t curTimeStamp)
{
	if (pkPlayer)
	{
		LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
		LC_OpenAbilityAsset& kOABAsset = pkPlayer->GetOpenAbilityAsset();
		if (kOABAsset.IsAbilityEnabled(LC_ABT_TASK, STT_REWARD_TASK))
		{
			AssignTask(pkPlayer, GetAssignType(STT_REWARD_TASK));
		}
	}
}
*/

void LC_ServerTaskManager::ClearPostTask(LC_ServerPlayer* pkPlayer, LC_Task* pkTask)
{
	if (NULL == pkTask || NULL == pkPlayer)
	{
		return;
	}
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return;
	}
	const TaskIDSet& kTaskIDs = pkTask->GetPostTaskIDs();
	TaskIDSet::const_iterator cit = kTaskIDs.begin();
	for (; cit != kTaskIDs.end(); ++cit)
	{
		int32_t iPostTID = *cit;
		LC_Task* pkPost = GetTask(*cit);
		bool flag = false;
		if (pkTaskMap->IsInFinishedTaskMap(iPostTID))
		{
			pkTaskMap->RemoveTaskFinishRecord(iPostTID);
			flag = true;
		}
		if (pkTaskMap->IsInProcessingTaskMap(iPostTID))
		{
			pkPlayer->CancelTask(iPostTID);
			flag = true;
		}
		if (pkPost && flag)
		{
			ClearPostTask(pkPlayer, pkPost);
		}
	}
}

void LC_ServerTaskManager::ResetTaskTree(LC_ServerPlayer* pkPlayer, int lTaskID)
{
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return ;
	}
	LC_Task* pkTask = GetTask(lTaskID);
	if (pkTask == NULL)
	{
		return ;
	}
	LC_Task* pretask = pkTask;
	LC_Task* curtask = pkTask;
	while (curtask)
	{
		int pre_task_id = curtask->GetPreTaskID();
		pretask = curtask;
		curtask = GetTask(pre_task_id);
	}
	if (pretask)
	{
		if (pkTaskMap->IsInFinishedTaskMap(pretask->GetTaskID()))
		{
			pkTaskMap->RemoveTaskFinishRecord(pretask->GetTaskID());
		}
		if (pkTaskMap->IsInProcessingTaskMap(pretask->GetTaskID()))
		{
			pkPlayer->CancelTask(pretask->GetTaskID());
		}
	}
	ClearPostTask(pkPlayer, pretask);
}

ResultType LC_ServerTaskManager::Phagotrophy(LC_ServerPlayer* pkPlayer, int lPhagotrophID, int lPhagocytableID, int iType, float& propo)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return RE_FAIL;
	}
	LC_Task* pkPhagotrophTask = GetTask(lPhagotrophID);
	if (pkPhagotrophTask == NULL)
	{
		return RE_FAIL;
	}
	LC_Task* pkPhagocytableTask = GetTask(lPhagocytableID);
	if (pkPhagocytableTask == NULL)
	{
		return RE_FAIL;
	}
	return pkTaskMap->PhagotrophyOperation(pkPhagotrophTask, pkPhagocytableTask, iType, propo);
}

bool LC_ServerTaskManager::AssignTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID)
{
	const unique_id_impl& instance = pkPlayer->GetInstance();
	LC_Task* pkTask = GetTask(taskID);
	if(NULL == pkTask)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "AssignTaskByTaskID taskid error! c_cid:%d, c_citizenship:%d, c_uid:%d, taskID:%d", instance.detail(), instance.catagory(), pkPlayer->GetUserID(), taskID);
		return false;
	}

	ResultType res = pkTask->CanAssign(pkPlayer);
	if(RE_SUCCESS != res)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "AssignTaskByTaskID CanAssign error! res:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, taskID:%d", res, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), taskID);
		return false;
	}

	int taskType = pkTask->GetTaskType();
	int uiAssignType = GetAssignType(taskType);
	if(0 == uiAssignType)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "AssignTaskByTaskID uiAssignType error! taskType:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, taskID:%d", taskType, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), taskID);
		return false;
	}

	LC_ServerTaskMap* pkTaskMap  = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "AssignTaskByTaskID pkTaskMap error! taskType:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, taskID:%d", taskType, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), taskID);
		return false;
	}

	LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
	return pkAssignInfo->AddTask(uiAssignType, taskID);
}

void LC_ServerTaskManager::ClearTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	bool tag = pkPlayer->IsInAssignTaskMap(taskID);
	if(tag)
	{
		ClearAssignTaskByTaskID(pkPlayer, taskID);
		return;
	}

	tag = pkPlayer->IsInProcessingTaskMap(taskID);
	if(tag)
	{
		ClearProcessTaskByTaskID(pkPlayer, taskID);
	}

	return;
}

void LC_ServerTaskManager::ClearAssignTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if(NULL != pkTaskMap)
	{
		pkTaskMap->RemoveAssignTask(taskID);
	}
}

void LC_ServerTaskManager::ClearProcessTaskByTaskID(LC_ServerPlayer* pkPlayer, int taskID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if(NULL == pkTaskMap)
	{
		return;
	}
	pkTaskMap->RemoveTaskProcessing(taskID);
}

void LC_ServerTaskManager::DispatchGameEvent(GameLogic::LC_GameEvent* pkEvent)
{
	switch (pkEvent->GetEventType())
	{
		case GLET_PLAYER_DIE:
			{
				_dispatchPlayerDieEvent(pkEvent);
			}
			break;
		case GLET_KILL:
			{
				_dispatchPlayerKillNPCEvent(pkEvent);
			}
			break;
		case GLET_GATHER_TASKNPC:
			{
				_dispatchPlayerGatherTaskNpcEvent(pkEvent);
			}
			break;
		case GLET_LOGIN_IN:
			{
				_dispatchPlayerLoginInEvent(pkEvent);
			}
			break;
		case GLET_CHALLENGE_WIN:
			{
				_dispatchPlayerChallengeWinEvent(pkEvent);
			}
			break;
		case GLET_VIPLEVELUP:
			{
				_dispatchPlayerVipLevelUpEvent(pkEvent);
			}
			break;
		case GLET_VIPEXTARE_REWARD:
			{
				_dispatchVipExtareRewardEvent(pkEvent);
			}
			break;
		case GLET_TREASURE_TIMES:
			{
				_dispatchTreasureTimesEvent(pkEvent);
			}
			break;
		case GLET_ADD_GUILD:
			{
				_dispatchAddGuildEvent(pkEvent);
			}
			break;
		case GLET_TITLE_LEVEL:
			{
				_dispatchTitleLevelEvent(pkEvent);
			}
			break;
		case GLET_KILLBOSS_DROPTIMES:
			{
				_dispatchKillBossDropTimesEvent(pkEvent);
			}
			break;
		case GLET_ALIVE_BUDDYNUM:
			{
				_dispatchAliveBuddyNumEvent(pkEvent);
			}
			break;
		case GLET_RAIDMAP:
			{
				_dispatchRaidMapEvent(pkEvent);
			}
			break;
		case GLET_PRESENT_BUDDYELEM:
			{
				_dispatchPresentBuddyElemEvent(pkEvent);
			}
			break;
		case GLET_KILL_CONTROLLER:
			{
				_dispatchKillControllerEvent(pkEvent);
			}
			break;
		case GLET_KILL_ELITE:
			{
				_dispatchPlayerKillEliteEvent(pkEvent);
			}
			break;
		case GLET_PLAYER_PICK_ITEM:
			{
				_dispatchPlayerPickItemEvent(pkEvent);
			}
			break;
		case GLET_PLAYER_ARRIVE_AREA:
			{
				_dispatchPlayerArriveAreaEvent(pkEvent);
			}
			break;
		case GLET_TASK_TIME_TICK:
			{
				_dispatchTaskTimeTickEvent(pkEvent);
			}
			break;
		case GLET_NPC_ARRIVE_AREA:
			{
				_dispatchNPCArriveAreaEvent(pkEvent);
			}
			break;
		case GLET_TALK_TO_NPC:
			{
				_dispatchTalkToNPCEvent(pkEvent);
			}
			break;
		case GLET_USE_ITEM:
			{
				_dispatchUseItemEvent(pkEvent);
			}
			break;
		case GLET_SELECT_CAREER:
			{
				_dispatchSelectCareerEvent(pkEvent);
			}
			break;
		case GLET_EQUIP_ITEM:
			{
				_dispatchEquipItemEvent(pkEvent);
			}
			break;
		case GLET_LEARN_SKILL_END:
			{
				_dispatchLearnSkillEvent(pkEvent);
			}
			break;
		case GLET_SKILL_LEVEL_UP:
			{
				_dispatchSkillLevelUpEvent(pkEvent);
			}
			break;
		case GLET_LEARN_FORMULA:
			{
				_dispatchLearnFormulaEvent(pkEvent);
			}
			break;
		case GLET_CHANGE_SYSTEM_TITLE:
			{
				_dispatchChangeSystemTitleEvent(pkEvent);
			}
			break;
		case GLET_PLAYER_LEVEL_UP:
			{
				_dispatchPlayerLevelUpEvent(pkEvent);
			}
			break;
		case GLET_SKILL_EXECUTE:
			{
				_dispatchSkillExecuteEvent(pkEvent);
			}
			break;
		case GLET_PLAYER_LEAVE_GROUP:
			{
				_dispatchLeaveGroupEvent(pkEvent);
			}
			break;
		case GLET_PLAYERGROUP_DISMISS:
			{
				_dispatchDismissGroupEvent(pkEvent);
			}
			break;
		case GLET_FINISH_TASK:
		case GLET_GAME_STORY_PROGRESS:
		case GLET_ARENA_CHALLENGE:
		case GLET_CROSS_ARENA_CHALLENGE:
		case GLET_ADD_CASH:
		case GLET_TOTAL_ADD_CASH:
		case GLET_REDUCE_CASH:
		case GLET_SP_ADD:
		case GLET_COMBO_COUNT:
		case GLET_ITEM_IDENTIFY:
		case GLET_DECORATE_TEXT:
		case GLET_SAND_TEXT:
		case GLET_APPEND_ATTRIBUTE:
		case GLET_COMPOSITION_SUCCESS:
		case GLET_TRANSCRIPTION_EVENT:
		case GLET_ADD_ITEM:
		case GLET_SWITCH_ITEM:
		case GLET_COMMON_STAT_OP:
		case GLET_TREASURE_CHEST_OP:
		case GLET_SIGNIN:
		case GLET_GUILD_OFFER:
		case GLET_ITEM_OPERATION:
		case GLET_ACTI_STAT_OP:
			{
				_dispatchDefaultEvent(pkEvent);
			}
			break;
		case GLET_KILL_PROGRESS:
			{
				_dispatchGuildKillEvent(pkEvent);
			}
			break;
		default:
			break;
	}
}

void LC_ServerTaskManager::_notifyEventToTask(GameLogic::LC_GameEvent* pkEvent, object_id_type lID)
{
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lID);
	if (pkPlayer == NULL)
	{
		return;
	}
	LC_TaskMap* pkTaskMap  = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return;
	}
	LC_TaskEventFilter kFilter;
	pkTaskMap->NotifyEvent(pkEvent, kFilter);
}

void LC_ServerTaskManager::_dispatchDefaultEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EVENT_BASE_PARAM_1);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPlayerDieEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_DIE_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}
///////////////////////////////////////////////////////////////////////////
class RecordEventOP : public LC_ActorOperator
{
public:
	RecordEventOP(LC_TaskEventFilterMap& eventFltMap, mem::set<uint64_t>& gldIDs)
		: kEventFltMap(eventFltMap), kGldIDs(gldIDs)
	{}
	virtual bool operator()(LC_ActorBase* pkActor)
	{
		if (NULL == pkActor || pkActor->GetDead() || !IS_PLAYER_ID(pkActor->GetID()))
		{
			return false;
		}
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkActor;
		if (kEventFltMap.find(pkPlayer->GetID()) != kEventFltMap.end())
		{
			return false;
		}
		if (/*pkPlayer &&*/ kGldIDs.find(pkPlayer->GetGuildID()) != kGldIDs.end())
		{
			kEventFltMap[pkPlayer->GetID()] = pkPlayer;
			kEventFltMap[pkPlayer->GetID()].RegTypes(STT_GUILD_TASK);
		}
		return true;
	}
public:
	LC_TaskEventFilterMap&	kEventFltMap;
	mem::set<uint64_t>&			kGldIDs;
};

class LeftEncountedEventOP : public LC_EncountedIDOperator
{
public:
	LeftEncountedEventOP(LC_ServerMap* map, LC_TaskEventFilterMap& eventFltMap, LC_TaskPtrs& taskPtrs)
		: pkMap(map), kEventFltMap(eventFltMap), kTaskPtrs(taskPtrs)
	{}

	virtual bool operator ()(object_id_type id)
	{
		if (!IS_PLAYER_ID(id) || kEventFltMap.find(id) != kEventFltMap.end())
		{
			return false;
		}

		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkMap->FindPlayer(id);
		if (NULL == pkPlayer || pkPlayer->GetDead())
		{
			return false;
		}

		kEventFltMap[id] = pkPlayer;

		LC_TaskPtrs::iterator itTask = kTaskPtrs.begin();
		for (; itTask != kTaskPtrs.end(); ++itTask)
		{
			kEventFltMap[id].RegTasks((*itTask)->GetTaskID());
		}
		return true;
	}
	
	LC_ServerMap* pkMap;
	LC_TaskEventFilterMap&	kEventFltMap;
	LC_TaskPtrs& kTaskPtrs;
};
///////////////////////////////////////////////////////////////////////////
void LC_ServerTaskManager::_dispatchPlayerKillNPCEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_KILL_DST_ID);
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_ServerNPCManager* npc_mgr = (LC_ServerNPCManager*)ENGINE_GET_NPC_MANAGER();
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)npc_mgr->FindNPC(lID);
	if (pkNPC == NULL)
	{
		GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "TaskMgr _dispatchPlayerKillNPCEvent FindNPC Failed!", lID);
		return;
	}
	LC_ServerMap* pkMap = (LC_ServerMap*)pkNPC->GetMap();
	if (NULL == pkMap)
	{
		return;
	}
	LC_ServerNPCOwnerList& rkOwnerList = pkNPC->GetNPCOwnerList();
	int  lOwnerCount = rkOwnerList.GetOwnerCount();
	if (lOwnerCount <= 0)
	{
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "TaskMgr _dispatchPlayerKillNPCEvent GetNPCOwnerList size[%d] npctypeID[%d] npcID:[%d] srcID:[%d]!", lOwnerCount, pkEvent->GetEventParam(EP_KILL_DST_CHAR_TYPE_ID), lID, pkEvent->GetEventParam(EP_KILL_SRC_ID));
		return;
	}

	mem::set<uint64_t> kGuildIDs;
	LC_TaskEventFilterMap kEventFilterMap;
	LC_TaskEventFilterMap groupEventFilterMap;
	object_id_type pkFirstHitPlayerID = pkNPC->GetHitPlayerID();
	for (int i = 0; i < lOwnerCount; ++i)
	{
		object_id_type lHarmPlayerID = rkOwnerList.GetOwnerByIndex(i);
		LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(lHarmPlayerID);
		if (NULL == pkPlayer)
		{
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "TaskMgr FindPlayer[%d] Failed!", lHarmPlayerID);
			continue;
		}

		LC_PlayerBase* pkOwnerPlayer = pkPlayer;
		object_id_type ownerID = pkPlayer->GetOwnerObjectID();
		if(ownerID != lHarmPlayerID)
		{
			pkOwnerPlayer = pkPlayerManager->FindPlayer(ownerID);
		}

		if (NULL == pkOwnerPlayer)
		{
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "TaskMgr FindOwnerPlayer[%d] Failed!", ownerID);
			continue;
		}

		if (pkOwnerPlayer->IsExistGuild())
		{
			kGuildIDs.insert(pkOwnerPlayer->GetGuildID());
		}

		kEventFilterMap[ownerID] = pkOwnerPlayer;

		//如果首杀玩家有队伍
		if(pkFirstHitPlayerID == ownerID)
		{
			groupEventFilterMap[ownerID] = pkOwnerPlayer;
			LC_PlayerGroupBase* pkGroup = pkOwnerPlayer->GetPlayerGroup();
			if(NULL != pkGroup)
			{
				mem::vector<unique_id_impl> charIDs;
				pkGroup->GetOnlineGroupNumber(charIDs);
				for(int i=0; i<charIDs.size(); ++i)
				{
					LC_PlayerBase* pkGroupPlayer = pkPlayerManager->FindPlayer(charIDs[i]);
					if(NULL != pkGroupPlayer)
					{
						object_id_type id = pkGroupPlayer->GetOwnerObjectID();
						groupEventFilterMap[id] = pkGroupPlayer;
					}
				}
			}
		}
	}

	if (kGuildIDs.empty() == false)
	{
		RecordEventOP kOP(kEventFilterMap, kGuildIDs);
		pkMap->ForAllPlayer(&kOP);
	}

	LC_EncountedStat* pkStat = pkNPC->GetEncountedStatMgr().GetStat(LC_ECT_TP_Damage);
	if (pkStat)
	{
		LC_TaskEventHandler kHandler(pkEvent->GetEventType(), pkNPC->GetCharType());
		LC_EventHandler2TaskIndices::iterator itTaskptrs = m_kEvent2Tasks.find(kHandler);
		if (itTaskptrs != m_kEvent2Tasks.end())
		{
			LeftEncountedEventOP kLeftOper(pkMap, kEventFilterMap, itTaskptrs->second);
			pkStat->ForAllEncounted(LC_ECT_CAT_ACTOR, kLeftOper);
		}
	}

	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGroupEvent = NULL;
	if(!groupEventFilterMap.empty())
	{
		pkGroupEvent = pkEventManager->AllocGameEvent();
		if (pkGroupEvent)
		{
			pkGroupEvent->SetEventType(GLET_KILL_GROUP_NPC);
			pkGroupEvent->SetEventParam(EP_KILL_SRC_ID, pkEvent->GetEventParam(EP_KILL_SRC_ID));
			pkGroupEvent->SetEventParam(EP_KILL_DST_CHAR_TYPE_ID, pkEvent->GetEventParam(EP_KILL_DST_CHAR_TYPE_ID));
			pkGroupEvent->SetEventParam(EP_KILL_DST_ID, pkEvent->GetEventParam(EP_KILL_DST_ID));
			pkGroupEvent->SetEventParam(EP_KILL_MAP_RES_ID, pkEvent->GetEventParam(EP_KILL_MAP_RES_ID));
		}
	}

	for(LC_TaskEventFilterMap::iterator kIt=groupEventFilterMap.begin(); kIt!=groupEventFilterMap.end(); ++kIt)
	{
		LC_TaskEventFilter& kGroupEventfilter = kIt->second;
		LC_ServerTaskMap* pkTaskMap  = (LC_ServerTaskMap*)kGroupEventfilter.GetPlayer()->GetTaskMap();
		if (NULL == pkTaskMap)
		{
			continue;
		}
		pkTaskMap->NotifyEvent(pkGroupEvent, kGroupEventfilter);
	}
	
	if (pkGroupEvent)
	{
		pkEventManager->FreeGameEvent(pkGroupEvent);
	}
	
	for(LC_TaskEventFilterMap::iterator it = kEventFilterMap.begin(); it != kEventFilterMap.end(); ++it)
	{
		LC_TaskEventFilter& kEventfilter = it->second;
		LC_ServerTaskMap* pkTaskMap  = (LC_ServerTaskMap*)kEventfilter.GetPlayer()->GetTaskMap();
		if (NULL == pkTaskMap)
		{
			continue;
		}
		pkTaskMap->NotifyEvent(pkEvent, kEventfilter);
	}
}

void LC_ServerTaskManager::_dispatchPlayerChallengeWinEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_CHALLENGE_WIN_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPlayerVipLevelUpEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_VIP_LEVEL_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPresentBuddyElemEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_PRESENT_BUDDYELEM_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchRaidMapEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_RAIDMAP_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchAliveBuddyNumEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_ALIVEBUDDY_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchKillBossDropTimesEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_KILLBOSS_DROP_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchTitleLevelEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_TITLE_LEVEL_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchAddGuildEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_ADD_GUILD_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchTreasureTimesEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_TREASURE_TIMES_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchVipExtareRewardEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_VIP_EXTAREREWARD_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPlayerLoginInEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_LOGIN_IN_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPlayerGatherTaskNpcEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_GATHERTASK_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchKillControllerEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_SRC_KILLER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPlayerKillEliteEvent(GameLogic::LC_GameEvent* pkEvent)
{
	_dispatchPlayerKillNPCEvent(pkEvent);
}

void LC_ServerTaskManager::_dispatchPlayerPickItemEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_PICK_ITEM_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPlayerArriveAreaEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_ARRIVE_AREA_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchTaskTimeTickEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_TASK_TIME_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchNPCArriveAreaEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_NPC_ARRIVE_AREA_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchTalkToNPCEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_TALK_TO_NPC_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchUseItemEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_USE_ITEM_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchSelectCareerEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_SELECT_CAREER_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchEquipItemEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_EQUIP_ITEM_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchLearnSkillEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_LEARN_SKILL_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchSkillLevelUpEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_SKILL_LEVEL_UP_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchLearnFormulaEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_LEARN_FORMULA_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchChangeSystemTitleEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_CHANGE_SYSTEM_TITlE_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchPlayerLevelUpEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_LEVEL_UP_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchSkillExecuteEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_SKILL_SKILL_EXECUTE_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchLeaveGroupEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_LEAVE_GROUP_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}

void LC_ServerTaskManager::_dispatchDismissGroupEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_DISMISS_GROUP_PLAYER_ID);
	_notifyEventToTask(pkEvent, lID);
}
void LC_ServerTaskManager::_dispatchTranscriptionEvent(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_1);
	_notifyEventToTask(pkEvent, lID);
}
void LC_ServerTaskManager::_dispatchGuildKillEvent(GameLogic::LC_GameEvent* pkEvent)
{
	int32_t iCat = (int32_t)pkEvent->GetEventParam(EP_ACTOR_ECT_STAT_TPCAT);
	if (LC_ECT_CAT_GROUP != iCat)
	{
		return;
	}
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EP_ACTOR_ECT_TRAGET_TID);
	//uint64_t	iGuildID = pkEvent->GetEventParam(EP_ACTOR_ECT_STAT_ID);
	uint64_t iGuildID = pkEvent->GetUint64Param();

	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_ServerNPCManager* npc_mgr = (LC_ServerNPCManager*)ENGINE_GET_NPC_MANAGER();
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)npc_mgr->FindNPC(lID);
	if (pkNPC == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "TaskMgr _dispatchPlayerKillNPCEvent FindNPC[%d] Failed!", lID);
		return;
	}
	LC_ServerMap* pkMap = (LC_ServerMap*)pkNPC->GetMap();
	if (NULL == pkMap)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildMgr = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo*	pkGuild = pkGuildMgr->GetGuildInfoByID(iGuildID);
	if (NULL ==  pkGuild)
	{
		return;
	}

	unique_id_vector kAllMemberIDs;
	kAllMemberIDs.reserve(pkGuild->GetMemberSize());
	pkGuild->GetAllMember(kAllMemberIDs);
	unique_id_vector::iterator it = kAllMemberIDs.begin();
	for (; it != kAllMemberIDs.end(); ++it)
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkPlayerManager->FindPlayer(*it);
		if (NULL == pkPlayer)
		{
			continue;
		}
		_notifyEventToTask(pkEvent, pkPlayer->GetID());
	}
}
