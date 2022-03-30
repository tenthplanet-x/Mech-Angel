//抽奖
#ifndef _H_Lottery_H_
#define _H_Lottery_H_
#pragma once

#include "LC_Define.h"

namespace char_data_define
{
	class PBLotteryData;
}

struct SLotteryData
{
	SLotteryData()
	{
		m_Free = true;
		m_DayCount = 0;
		m_TotalCount = 0;
		m_RewardNum = 0;
		if( !m_Status.empty() )
		{
			m_Status.clear();
		}
	}
	bool                            m_Free;                         			//每日免费状态
	int32_t                         m_DayCount;                     			//今日抽奖次数
	int32_t                         m_TotalCount;                   			//总抽奖次数
	int32_t                         m_RewardNum;                    			//类型是1时候，额外奖励的次数计数 10的倍数
	mem::set<int32_t>            	m_Status;                       			//类型是2时候，额外奖励的领取状态
};

class LC_ServerPlayer;
class CLottery
{
	public:
		CLottery();
		~CLottery();

		//存储数据
		bool SaveData( char_data_define::PBLotteryData* _pData );

		//读取数据
		bool LoadData( const char_data_define::PBLotteryData& _data );

		//0点重置数据
		void ResetData( LC_ServerPlayer* _player );

		//玩家上线同步抽奖数据
		void SendLotteryData( LC_ServerPlayer* _player );

		//抽奖
		void LotteryDraw( LC_ServerPlayer* _player, int32_t _id, int _num );

		//获取额外奖励
		void LotteryExtraReward( LC_ServerPlayer* _player, int32_t _id, int _param );

		//获得寻宝的总数量
		int GetLotteryTotalNum(LC_ServerPlayer* _player);
	protected:

	private:
		int m_TotalNum;		//所有抽奖总次数
		mem::map<int32_t, SLotteryData > m_LotteryDataMap;
};

#endif
