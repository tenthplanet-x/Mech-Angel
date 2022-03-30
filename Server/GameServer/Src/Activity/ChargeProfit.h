//充值累积
#ifndef _H_CHargeProfit_H_
#define _H_CHargeProfit_H_
#pragma once

#include "LC_Define.h"
#include "UT_ComplexData.h"

namespace char_data_define
{
	class PBChargeProfitData;
}


struct SChargeProfit
{
	SChargeProfit()
	{
		m_DayCount = 0;
		m_Timestamp = 0;
		if( !m_Status.empty() )
		{
			m_Status.clear();
		}
	}
	int32_t                         m_DayCount;                     			//今日充值数量
	int32_t							m_Timestamp;								//领取时间单日的24:00时间戳
	mem::map<int,int>       m_Status;                       	//奖励的领取状态 second:1未领取 2已经领取
};

class LC_ServerPlayer;
class CChargeProfit
{
	public:
		CChargeProfit();
		~CChargeProfit();

		//存储数据
		bool SaveData( char_data_define::PBChargeProfitData* _pData );

		//读取数据
		bool LoadData( const char_data_define::PBChargeProfitData& _data );
		
		//0点重置数据
		void ResetData( LC_ServerPlayer* _player );
		
		//玩家上线同步充值数据
		void SendChargeProfitData( LC_ServerPlayer* _player );

		//领取充值奖励
		void ChargeProfitReward( LC_ServerPlayer* _player, int32_t _id, int _param );

		//修改数值处理
		void ModifyValue( LC_ServerPlayer* _player, int32_t cashType, int _value, int type );

		//获得可领取励
		void FillCanChargeProfitReward(int id, Utility::UT_SIMDataList& rewardItemList);

		//重置玩家数据
		void ResetChargeProfitInfo(int id);

	private:
		mem::map<int, SChargeProfit> m_ChargeProfitMap;	//每日充值数据
};

#endif
