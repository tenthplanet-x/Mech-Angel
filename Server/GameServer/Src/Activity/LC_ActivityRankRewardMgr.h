#ifndef _LC_ACTIVITYRANKREWARD_MGR_H_
#define _LC_ACTIVITYRANKREWARD_MGR_H_

#include "LC_Define.h"
#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "UT_ComplexData.h"

struct RewardInfo
{
	StringType m_sMailTitle;//邮件标题
	StringType m_sMailDesc;//邮件内容
	std::map<int32_t, StringType> m_mRewardItem;//key:排名 value:奖励道具
};

class CActivityRankRewardMgr : public Memory::MM_BaseObject, public Utility::UT_Singleton<CActivityRankRewardMgr>
{
public:
	CActivityRankRewardMgr();
	~CActivityRankRewardMgr();

	void Init();
	void Unit();

	const RewardInfo& GetRankRewardInfo(int type, int param);
	StringType GetRankRewardItem(int type, int param, int rankIndex, StringType& mailTitle, StringType& mailDesc);
private:
	std::map<uint64_t, RewardInfo> m_mRankRewardData;//key:EncodeScore(_GroupID,_iParam)  value:奖励信息

private:
	RewardInfo m_nEmptyRewardInfo;//空数据
};

#endif