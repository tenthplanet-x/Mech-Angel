//抽奖配置
#ifndef _H_ChargeProfitConfigManager_H_
#define _H_ChargeProfitConfigManager_H_
#include "LC_Define.h"
#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "UT_ComplexData.h"

struct SChargeProfitConfig
{
	SChargeProfitConfig()
	{
		m_Type = 0;
		m_Schedule = 0;
		m_CashType = 0;
		m_FinishSchedule = 0;
	}

	int m_Type;			//类型
	int m_Schedule;		//计划时间id
	int m_FinishSchedule;//结束时间id
	int m_CashType;		//货币类型
	mem::vector<int> m_CondtionVec;	//完成的条件数组
	mem::vector<int> m_LimitNumVec;	//限制领取次数数组
	mem::vector<int> m_IdVec;		//奖励id数组，中间变量
	std::vector<Utility::UT_SIMDataList>	m_RewardVec;    //奖励数组
	StringType m_sMailTitle;//邮件标题
	StringType m_sMailDesc;//邮件内容
};

class CChargeProfitConfigManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<CChargeProfitConfigManager>
{
public:
	CChargeProfitConfigManager();
	~CChargeProfitConfigManager();

	//初始化
	void Init();
	//释放
	void Unit();
	//重加载
	void Reload();

	//得到充值数据
	SChargeProfitConfig* GetChargeProfitConfig( int _id );

	//得到所有充值的计划时间
	void GetChargeProfitScheduleByCashType( int32_t cashType, int type, mem::map<int,int>& _map );

protected:
	void InitChargeProfit();
	void InitChargeProfitReward();

private:
	mem::map<int, SChargeProfitConfig> m_ChargeProfitConfigMap;	//累积充值配置数据
};


#endif