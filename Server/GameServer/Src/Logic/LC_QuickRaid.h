#ifndef _LC_QUICKRAID_H_
#define _LC_QUICKRAID_H_

#include "LC_Define.h"
#include "SystemError.h"
#include "UT_OperateLog.h"
#include "char_data.pb.h"

#include "CF_TranscriptionRule.h"

using namespace CSVFile;
using namespace OPERATELOG;

class LC_ServerPlayer;
class CQuickRaidAsset
{
public:
	CQuickRaidAsset();
	~CQuickRaidAsset();

	void Clear();
	void SaveData(::char_data_define::PBQuickRaidData* pkData);
	void LoadData(const ::char_data_define::PBQuickRaidData& data);

	void	QuickRaid_New(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times);
	void	ResetQuickRaidTimes(LC_ServerPlayer* pkOwnerPlayer);
	void	SyncQuickRaidAsset(LC_ServerPlayer* pkOwnerPlayer);
protected:
	
private:
	ResultType CheckQuick_Raid(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times, int32_t storyID, CF_TranscriptionRule::DataEntry* pkData);
	ResultType DoQuick_Raid_New(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times, int32_t storyID, CF_TranscriptionRule::DataEntry* pkData);

	void			AddQuickRaidTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times, int32_t storyID);
	uint32_t	GetUsedRaidTimes(int32_t key);
	void			AddUsedRaidTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t key, uint32_t times);
	void			SyncUsedRaidCount(LC_ServerPlayer* pkOwnerPlayer, int32_t key, uint32_t times);
private:
	mem::map<int32_t, uint32_t> m_nUsedRaidTimesRecords;//key:mapID value:É¨µ´´ÎÊý 
};

#endif
