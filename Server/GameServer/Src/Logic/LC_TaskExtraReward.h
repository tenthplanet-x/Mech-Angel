#ifndef _H_TaskExtraReward_H_
#define _H_TaskExtraReward_H_
#pragma once

#include "LC_Define.h"

namespace char_data_define
{
	class PBTaskExtraReward;
	class PBKeyValue;
}

class LC_ServerPlayer;
class CTaskExtraReward
{
public:
	CTaskExtraReward();
	~CTaskExtraReward();

	void SaveData(char_data_define::PBTaskExtraReward* pkInfo);
	void LoadData(const char_data_define::PBTaskExtraReward& info);

	void SyncTaskExtraReward(LC_ServerPlayer* pkPlayer);
	void GetTaskExtraReward(LC_ServerPlayer* pkPlayer);
	void UpdateTaskExtraReward(int32_t itemID, int32_t itemNum, int32_t bindMode);
protected:
private:
	mem::map<int32_t, mem::map<int32_t, int32_t> > m_ExtraRewardData;//绑定的道具 key:bindMode
};

#endif