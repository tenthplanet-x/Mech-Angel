//抽奖配置
#ifndef _H_LotteryConfigManager_H_
#define _H_LotteryConfigManager_H_
#include "LC_Define.h"
#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "UT_ComplexData.h"

//抽奖总表
struct SLotteryConfig
{
	SLotteryConfig()
	{
		m_Schedule = 0;
		m_ExtraSchedule = 0;
		m_Category = 0;
		m_Cost = 0;
		m_MaxNum = 0;
		m_VipCost = 0;
		m_VipItem = 0;
		m_DayMaxNum = 0;
		m_ItemRewardId = 0;
		m_CostNum = 10;
	}

	int m_Schedule;	//计划时间id
	int m_ExtraSchedule;	//失效时间id 领取额外奖励时候用到
	int m_Category;	//抽奖类型
	int m_Cost;	//1次钻石消耗
	int m_MaxNum;	//连抽最大数
	int m_DayMaxNum;	//每日抽奖最大数
	int m_VipCost;	//钻石vip打折id
	int m_VipItem;	//道具bip打折id
	int m_ItemRewardId;	//m_Category==1时，用到的额外奖励数据id
	int m_CostNum;	//m_Category==1时，扣除的次数
	mem::vector<int> m_CountVec;	//控制奖池的次数权重 
	mem::vector<int> m_RewardPondVec;	//奖池id数组,当count % m_CountVec[index] == 0,则使用当前index的奖池id
	Utility::UT_SIMDataList m_ItemCost;	//1次道具消耗
};

//m_Category==1时，用到的额外奖励数据
struct SLotteryItemConfig
{
	mem::vector<int> m_WeightVec;	//权重数组
	mem::vector<int> m_LevelVec;	//等级数组
	Utility::UT_SIMDataList m_RewardVec;	//道具奖励数组
};

//m_Category==2时，用到的额外奖励数据
struct SLotteryRewardConfig
{
	mem::vector<int> m_NumVec;	//领取次数奖励需要的次数vec
	Utility::UT_SIMDataList m_RewardVec;	//道具奖励数组 2个数字的size相等
};

//奖池奖励的数据
struct SLotteryPondConfig
{
	SLotteryPondConfig()
	{
		m_TotalWeight = 0;
	}
	int m_TotalWeight;		//总权重
	mem::vector<int> m_WeightVec;	//权重数组
	mem::vector<int> m_BroadcastVec;	//广播id数组
	Utility::UT_SIMDataList m_RewardVec;	//道具奖励数组
};

class CLotteryConfigManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<CLotteryConfigManager>
{
	public:
		CLotteryConfigManager();
		~CLotteryConfigManager();

		//初始化
		void Init();
		//释放
		void Unit();
		//重加载
		void Reload();

		//得到总表数据
		SLotteryConfig* GetLotteryConfig( int _id );

		//得到额外奖励数据1 的数据
		SLotteryItemConfig* GetLotteryItemConfig( int _id );

		//得到额外奖励数据2 的数据
		SLotteryRewardConfig* GetLotteryRewardConfig( int _id );

		//得到奖池奖励
		bool GetLotteryPondReward( Utility::UT_SIMDataList& _rewardItem, int _id, int _totalNum, int _num, bool _bFree );

		//得到额外奖励1的奖励
		int GetLotteryItemReward( Utility::UT_SIMDataList& _rewardItem, int _id, int _level, int remainTimes);

	protected:
		//初始化总表
		void InitTreasureHunt();
		//初始化奖池数据
		void InitTreasureHuntReward();
		//初始化m_Category==1时，用到的额外奖励数据
		void InitTreasureHuntStageReward();

	private:
		mem::map<int, SLotteryConfig > m_LotteryConfigMap;	//总表数据
		mem::map<int, SLotteryItemConfig> m_LotteryItemConfigMap;	//额外奖励数据1
		mem::map<int, SLotteryRewardConfig> m_LotteryRewardConfigMap;//额外奖励数据2
		mem::map<int, SLotteryPondConfig> m_LotteryPondConfigMap;	//奖池数据
};

#endif
