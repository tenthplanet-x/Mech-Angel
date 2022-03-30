#ifndef _LC_ENDACTIVITYMANAGER_H_
#define _LC_ENDACTIVITYMANAGER_H_

#include "LC_Define.h"
#include "LC_ActorBase.h"
#include "UT_ServerHelper.h"
#include "CF_ACTShopA.h"

class LC_ServerPlayer;
struct SendRewardMailAndResetCash : public GameLogic::LC_ActorOperator
{
	SendRewardMailAndResetCash(StringType& str, int type);
	~SendRewardMailAndResetCash();

	void GetACTScheduleEndInfo(LC_ActorBase* pkActor, const CSVFile::CF_ACTShopA::DataEntry* pkData, MailInfo& mailInfo, std::set<int>& cashType);
	virtual bool operator()(GameLogic::LC_ActorBase* pkActor);

	int m_iType;//活动类型的type
	mem::vector<int> m_vTypes;//ACT_ShopA.xlsx表的_iType
};


//活动结束后，需要按照上榜名次发奖
struct SendRankRewardMailToPlayer
{
	SendRankRewardMailToPlayer(StringType& str);
	~SendRankRewardMailToPlayer();

	//活动结束发邮件 或者服务器重启补发邮件
	void DoEndActivity();

	int m_iType;
	mem::vector<int> m_vChargeProfitID;//
};

struct SendChargeProfitReward : public GameLogic::LC_ActorOperator
{
	SendChargeProfitReward(int id);
	~SendChargeProfitReward();

	virtual bool operator()(GameLogic::LC_ActorBase* pkActor);

	int m_vChargeProfitBID;//ACTChargeProfitB.xlsx表的_iID
};

class CEndActivityManager: public Memory::MM_BaseObject, public Utility::UT_Singleton<CEndActivityManager>
{
public:
	CEndActivityManager();
	~CEndActivityManager();

	void Init();
	void Unit();
	void EndActivity(int32_t scheduleID);
	void CheckEndActivity(LC_ServerPlayer* pkOwnerPlayer);
	void RestartCheckEndActivity();

private:
	std::map<int32_t, SendRewardMailAndResetCash> m_SendRewardMailAndResetCashMap;//key:scheduleID
	std::map<int32_t, SendRankRewardMailToPlayer> m_SendRankRewardMailToPlayerMap;//key:scheduleID
	std::map<int32_t, SendChargeProfitReward> m_SendChargeProfitRewardMap;//key:scheduleID
};


#endif
