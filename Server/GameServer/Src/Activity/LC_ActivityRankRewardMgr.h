#ifndef _LC_ACTIVITYRANKREWARD_MGR_H_
#define _LC_ACTIVITYRANKREWARD_MGR_H_

#include "LC_Define.h"
#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "UT_ComplexData.h"

struct RewardInfo
{
	StringType m_sMailTitle;//�ʼ�����
	StringType m_sMailDesc;//�ʼ�����
	std::map<int32_t, StringType> m_mRewardItem;//key:���� value:��������
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
	std::map<uint64_t, RewardInfo> m_mRankRewardData;//key:EncodeScore(_GroupID,_iParam)  value:������Ϣ

private:
	RewardInfo m_nEmptyRewardInfo;//������
};

#endif