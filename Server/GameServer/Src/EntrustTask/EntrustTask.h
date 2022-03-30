//委派任务
#ifndef _H_EntrustTask_H_
#define _H_EntrustTask_H_
#pragma once

#include "LC_Define.h"

class LC_ServerPlayer;
namespace Protocol
{
	class EntrustTask_Base;
}

namespace char_data_define
{
	class PBEntrustTaskData;
}

struct SEntrustTaskConfig;

struct SEntrustTaskData
{
	SEntrustTaskData() : m_Index( 0 ), m_Id( 0 ), m_EndTime( 0 )
	{
	
	}

	SEntrustTaskData( const SEntrustTaskData& _data )
	{
		m_Index = _data.m_Index;
		m_Id = _data.m_Id;
		m_EndTime = _data.m_EndTime;
		m_Vec = _data.m_Vec;
	}

	void operator=( const SEntrustTaskData& _data )
	{
		m_Index = _data.m_Index;
		m_Id = _data.m_Id;
		m_EndTime = _data.m_EndTime;
		m_Vec = _data.m_Vec;
	}

	int32_t	m_Index;	//任务唯一索引
	int32_t m_Id;		//任务id
	int32_t m_EndTime;	//结束时间	
	mem::vector<int32_t> m_Vec;	//派出的英雄或者宝具
};

class CEntrustTask
{
	public:
		CEntrustTask();
		~CEntrustTask();

		//存储数据
		bool SaveData( char_data_define::PBEntrustTaskData* _pData );

		//读取数据
		bool LoadData( const char_data_define::PBEntrustTaskData& _data );

		//0点重置数据
		void ResetData( LC_ServerPlayer* _player );

		//玩家上线发送委派数据
		void SendEntrustTask( LC_ServerPlayer* _player );

		//接受委派任务
		void AcceptEntrustTask( LC_ServerPlayer* _player, int32_t _id, std::vector<int32_t>& _vec );

		//加速委派任务
		void SpeedUpEntrustTask( LC_ServerPlayer* _player, int32_t _index );

		//领取委派奖励
		void GetAwardEntrustTask( LC_ServerPlayer* _player, std::vector<int32_t>& _vec );

		//刷新委派任务
		void UpdateEntrustTask( LC_ServerPlayer* _player );
	private:
		//刷新任务
		bool RandomEntrustTask( int32_t _star = 0 );

		//SEntrustTaskData转EntrustTask_Base
		void EntrustDataToBase( SEntrustTaskData& _data, Protocol::EntrustTask_Base& _info );

		//判断英雄是否满足条件
		int CheckHeroCondition( LC_ServerPlayer* _player, std::vector<int32_t>& _vec, SEntrustTaskConfig* _pConf );

		//判断宝具是否满足条件
		int CheckRuneCondition( LC_ServerPlayer* _player, std::vector<int32_t>& _vec, SEntrustTaskConfig* _pConf );
	private:
		int32_t m_Num;	//刷新次数	toDB
		int32_t	m_Flag;	//是否第一次随机 0第一次	toDB
		int32_t m_CostNum;	//道具或者金钱刷新次数 toDB
		mem::list<int32_t> m_TaskList;			//可以接受的任务 toDB
		mem::map<int32_t, SEntrustTaskData>	m_AccpetTaskMap;	//已经接受的任务 toDB
		mem::map<int,int> m_UsedMap;		//已经派遣的英雄和宝具对应的任务id toDB
};

#endif
