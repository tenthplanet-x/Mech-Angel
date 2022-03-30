#ifndef _LC_ROTARYTABLE_H_
#define _LC_ROTARYTABLE_H_

#include "LC_Define.h"
#include "SystemError.h"
#include "char_data.pb.h"

class LC_ServerPlayer;
class CRotaryTable
{
public:
	CRotaryTable();
	~CRotaryTable();

	void LoadData(const char_data_define::PB_RotaryTable& data);
	void SaveData(char_data_define::PB_RotaryTable* pkData);

	//计算幸运转盘活动的结束时间	
	void InitRotaryTableData(int32_t time);
	//转盘抽抽奖
	void GetRotaryTableReward(LC_ServerPlayer* pkOwnerPlayer);
	//设置跳过动画标记
	void SetRotaryTableSkipAnimation(LC_ServerPlayer* pkOwnerPlayer, bool flag);
	//更新转盘旋转时间
	void UpdateRotaryTableReward(LC_ServerPlayer* pkOwnerPlayer, bool force=false);

	//登录的时候，同步转盘所有数据
	void SyncAllRotaryTableInfo(LC_ServerPlayer* pkOwnerPlayer);
	
	void SetRotaryTableCDStartTime(int64_t time);

	void UpdateRotaryTableVaildTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t value);

protected:
private:
	void SyncRotaryTableVaildTimes(LC_ServerPlayer* pkOwnerPlayer);
	void SyncRotaryTableRewardRecord(LC_ServerPlayer* pkOwnerPlayer);
	void SyncRotaryTableEndTime(LC_ServerPlayer* pkOwnerPlayer);
	void SyncRotaryTableSkipAnimation(LC_ServerPlayer* pkOwnerPlayer);

	ResultType CheckRotaryTableVaild();

	//抽奖次数
	int32_t	GetRotaryTableVaildTimes();
	int32_t	GetRotaryTableTimes();

	ResultType RandomRotaryTableRewardIndex(int32_t times, int32_t& rewardIndex);
	void UpdateRotaryTableRewardIndex(LC_ServerPlayer* pkOwnerPlayer, int32_t rewardIndex);
	bool	CheckRotaryTableRewardIndexVaild(int32_t rewardIndex);
	void SetCurrentRotaryTableRewardIndex(int32_t rewardIndex);

	int32_t m_nRotaryTableEndTime;//转盘活动结束的时间

	int64_t m_nRotaryTableCDStartTime;//转盘开始转动的时间
	int32_t m_nCurrentRTRewardIndex;//本次随机的奖励index

	// 存db
	int32_t m_nRotaryTableVaildTimes;//转盘有效抽奖次数
	mem::vector<int32_t> m_nRotaryTableRewardIndex;//已经获得过的转盘奖励
	bool m_bSkipAnimation;//是否跳过动画(true:跳过 false:不跳过)
};

#endif
