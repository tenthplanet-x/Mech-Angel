#ifndef _LC_SREVER_TASKMAP_H
#define _LC_SREVER_TASKMAP_H

#include "LC_TaskMap.h"
#include "LC_Task.h"

class LC_ItemFlopInfoList;
class LC_ServerNPC;

class LC_ServerTaskMap : public GameLogic::LC_TaskMap
{
public:
	LC_ServerTaskMap();
	virtual ~LC_ServerTaskMap();

	void ResetTypeData(uint32_t timestamp);

	void CheckReset(uint32_t timestamp, int32_t iTaskType, bool bforce=false, bool bKeepProcessing=false);

	void ClearAllTaskRecordOfType(int32_t iTaskType, bool bKeepProcessing, bool bResetProcessing=false);

	virtual void		Clear();
	virtual bool		Init(){return true;};
	virtual bool		UnInit(){return true;};

	ResultType			DrawLevelReward(uint32_t iTaskType, uint32_t level);
	ResultType			OneKeyOperation(uint32_t iTaskAssignType);
	ResultType			DrawAssignRoundOffReward(uint32_t iTaskAssignType);
	ResultType			PhagotrophyOperation(GameLogic::LC_Task* pkPhagotrophTask, GameLogic::LC_Task* pkPhagocytableTask, int iType, float& propo);

	ResultType			Phagotrophy(GameLogic::LC_Task* pkPhagotrophTask, GameLogic::LC_Task* pkPhagocytableTask, float propo);

	virtual void        GenerateTaskItemFlopInfoOnNPCDead(LC_ItemFlopInfoList& rkFlopInfoList, LC_ServerNPC* pkNPC, GameLogic::LC_TaskPtrs& pkTask);	
	void		InitBillboardTaskList();

	void PBLoadProcessingTaskInfo(const char_data_define::processing_task_asset_info& info);
	void PBSaveProcessingTaskInfo(char_data_define::processing_task_asset_info& info);

	void PBLoadFailTaskInfo(const char_data_define::fail_task_asset_info& info);
	void PBSaveFailTaskInfo(char_data_define::fail_task_asset_info& info);

	void PBLoadTaskChapterRewardInfo(const ::google::protobuf::RepeatedField<::google::protobuf::uint32>& info);
	void PBSaveTaskChapterRewardInfo(::google::protobuf::RepeatedField<::google::protobuf::uint32>& info);
};

#endif