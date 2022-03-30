#include "EntrustTask.h"
#include "LC_ServerPlayer.h"
#include "MG_GameProtocol.h"
#include "LC_Helper.h"
#include "EntrustTaskConfigManager.h"
#include "CF_RuneList.h"
#include "../Buddy/BuddyInstance.h"
#include "char_data.pb.h"
#include "GlobalSettings.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;
using namespace Utility;

CEntrustTask::CEntrustTask()
{
	m_Num = 0;
	m_CostNum = 0;
	int m_Flag = 0;
}

CEntrustTask::~CEntrustTask()
{
	if( !m_TaskList.empty() )
	{
		m_TaskList.clear();
	}

	if( !m_AccpetTaskMap.empty() )
	{
		m_AccpetTaskMap.clear();
	}

	if( !m_UsedMap.empty() )
	{
		m_UsedMap.clear();
	}
}

bool CEntrustTask::SaveData( char_data_define::PBEntrustTaskData* _pData )
{
	if( !_pData )
	{
		return false;
	}
	_pData->set_num( m_Num );
	_pData->set_flag( m_Flag );
	_pData->set_costnum( m_CostNum );

	for( mem::list<int>::iterator it = m_TaskList.begin(); it != m_TaskList.end(); ++it )
	{
		_pData->add_tasks( (*it) );
	}

	for( mem::map<int,int>::iterator it = m_UsedMap.begin(); it != m_UsedMap.end(); ++it )
	{
		char_data_define::PBKeyValue* p = _pData->add_useds();
		if( p )
		{
			p->set_key( it->first );
			p->set_value( it->second );
		}
	}

	for( mem::map<int32_t, SEntrustTaskData>::iterator it = m_AccpetTaskMap.begin(); 
			it != m_AccpetTaskMap.end(); ++it )
	{
		char_data_define::PBEntrustInfo* p = _pData->add_accepttasks();
		if( p )
		{
			p->set_index( it->second.m_Index );
			p->set_id( it->second.m_Id );
			p->set_endtime( it->second.m_EndTime );
			for( int k = 0; k < it->second.m_Vec.size(); ++k )
			{
				p->add_vec( it->second.m_Vec[k] );
			}
		}
	}
	return true;
}

bool CEntrustTask::LoadData( const char_data_define::PBEntrustTaskData& _data )
{
	if( !m_TaskList.empty() )
	{
		m_TaskList.clear();
	}

	if( !m_UsedMap.empty() )
	{
		m_UsedMap.clear();
	}

	if( !m_AccpetTaskMap.empty() )
	{
		m_AccpetTaskMap.clear();
	}

	m_Num = _data.num();
	m_Flag = _data.flag();
	m_CostNum = _data.costnum();
	for( int i = 0; i < _data.tasks_size(); ++i )
	{
		int id = _data.tasks(i);
		m_TaskList.push_back( id );
	}

	for( int i = 0; i < _data.useds_size(); ++i )
	{
		int key = _data.useds(i).key();
		int value = _data.useds(i).value();
		m_UsedMap[key] = value;
	}

	for( int i = 0; i < _data.accepttasks_size(); ++i )
	{
		SEntrustTaskData info;
		info.m_Index = _data.accepttasks(i).index();
		info.m_Id = _data.accepttasks(i).id();
		info.m_EndTime = _data.accepttasks(i).endtime();
		for( int k = 0; k < _data.accepttasks(i).vec_size(); ++k )
		{
			info.m_Vec.push_back( _data.accepttasks(i).vec(k) );
		}
		m_AccpetTaskMap[info.m_Index] = info;
	}
	return true;
}

void CEntrustTask::ResetData( LC_ServerPlayer* _player )
{
	if( !_player )
	{
		return;
	}

	m_Num = 0;
	m_CostNum = 0;
	MG_SC_UpdateEntrustTaskNum_Ret sendMsg;
	sendMsg.m_Num = 0;
	_player->SendMsgToClient( MGPT_SC_UpdateEntrustTaskNum_Ret, &sendMsg );
}

void CEntrustTask::SendEntrustTask( LC_ServerPlayer* _player )
{
	if( !_player )
	{
		return;
	}

	if( m_Flag == 0 )
	{
		bool b = RandomEntrustTask();
		if( b )
		{
			m_Flag = 1;
		}
	}

	MG_SC_SendEntrustTask_Ret sendMsg;
	for( mem::map<int32_t, SEntrustTaskData>::iterator it = m_AccpetTaskMap.begin(); 
			it != m_AccpetTaskMap.end(); ++it )
	{
		EntrustTask_Base data;
		EntrustDataToBase( it->second, data );
		sendMsg.m_AccpetedTasks.push_back( data );
	}

	for( mem::list<int>::iterator it = m_TaskList.begin(); it != m_TaskList.end(); ++it )
	{
		sendMsg.m_Tasks.push_back( (*it) );
	}

	sendMsg.m_Num = m_Num;

	_player->SendMsgToClient( MGPT_SC_SendEntrustTask_Ret, &sendMsg );
}

void CEntrustTask::AcceptEntrustTask( LC_ServerPlayer* _player, int32_t _id, std::vector<int32_t>& _vec )
{
	if( !_player )
	{
		return;
	}

	CEntrustTaskConfigManager* pMgr = CEntrustTaskConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return;
	}

	MG_SC_AccpetEntrustTask_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;

	int curTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	//检查数据是否合法
	SEntrustTaskConfig* pConf = pMgr->GetEnstrustTaskConfig( _id );
	if( !pConf )
	{
		//数据异常
		sendMsg.result = RE_EntrustTask_DataIllegal;
		_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
		return;
	}

	//是否从未接任务中获取的
	mem::list<int32_t>::iterator iter = std::find( m_TaskList.begin(), m_TaskList.end(), _id );
	if( iter == m_TaskList.end() )
	{
		//数据异常
		sendMsg.result = RE_EntrustTask_TaskIllegal;
		_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
		return;
	}

	//标记错误
	if( m_Flag == 0 )
	{
		sendMsg.result = RE_EntrustTask_DataError;
		_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
		return;
	}

	//判断英雄或者宝具是否被占用
	if( _vec.empty() )
	{
		sendMsg.result = RE_EntrustTask_TaskError;
		_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
		return;
	}

	for( int i = 0; i < _vec.size(); ++i )
	{
		mem::map<int,int>::iterator it = m_UsedMap.find( _vec[i] );
		if( it != m_UsedMap.end() )
		{
			mem::map<int32_t, SEntrustTaskData>::iterator tempIt = m_AccpetTaskMap.find( it->second );
			if( tempIt == m_AccpetTaskMap.end() )
			{
				sendMsg.result = RE_EntrustTask_TaskNoExist;
				_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
				return;
			}
			//判断时间
			if( curTime < tempIt->second.m_EndTime )
			{
				sendMsg.result = RE_EntrustTask_Used;
				_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
				return;
			}
		}
	}

	//判断条件是否满足
	ResultType result = RE_SUCCESS;
	if( pConf->type == 1 )
	{
		//英雄
		result = CheckHeroCondition( _player, _vec, pConf );
	}
	else if( pConf->type == 2)
	{
		//宝具
		result = CheckRuneCondition( _player, _vec, pConf );
	}
	else
	{
		sendMsg.result = RE_EntrustTask_TypeError;
		_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
		return;
	}

	if( result != RE_SUCCESS )
	{
		_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
		return;
	}
	
	//判断消耗是否足够
	int costType = pConf->acceptCost.key;
	int costNum = pConf->acceptCost.value;
	bool b = _player->IsCashEnough( costNum, costType );
	if( !b )
	{
		sendMsg.result = RE_EntrustTask_AcceptNoCost;
		_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );
		return;
	}
	//委派任务--埋点日志
	vector<int> nParams; 
	nParams.clear();
	nParams.push_back(_id);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, WEIPAITASK_OPERATE, OPERATE_LOGTYPE1); 
	_player->ReduceCash( costNum, (CashType)costType, SYS_FUNC_TP_AcceptEntrustTask, 0,nLogCode, nParams);

	//操作成功
	m_TaskList.erase( iter );
	SEntrustTaskData data;
	data.m_Index = (++m_Flag);
	data.m_Id = _id;
	data.m_EndTime = curTime + pConf->needTime;
	data.m_Vec.assign( _vec.begin(), _vec.end() );
	m_AccpetTaskMap[data.m_Index] = data;

	for( int i = 0; i < _vec.size(); ++i )
	{
		m_UsedMap[_vec[i]] = data.m_Index;
	}

	EntrustDataToBase( data, sendMsg.m_Task );
	_player->SendMsgToClient( MGPT_SC_AccpetEntrustTask_Ret, &sendMsg );

	{
		//成就系统
		AchievementEvent kEvent;
		kEvent.SetNum(1);
		_player->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ACCEPT_ENTRUSTTASK_COUNT, kEvent);
	}
}

void CEntrustTask::SpeedUpEntrustTask( LC_ServerPlayer* _player, int32_t _index )
{
	if( !_player )
	{
		return;
	}

	CEntrustTaskConfigManager* pMgr = CEntrustTaskConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return;
	}

	int curTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	MG_SC_SpeedUpEntrustTask_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;

	//验证任务是否合法
	mem::map<int32_t, SEntrustTaskData>::iterator it = m_AccpetTaskMap.find( _index );
	if( it == m_AccpetTaskMap.end() )
	{
		//任务不存在
		sendMsg.result = RE_EntrustTask_DataIllegal;
		_player->SendMsgToClient( MGPT_SC_SpeedUpEntrustTask_Ret, &sendMsg );
		return;
	}

	if( curTime >= it->second.m_EndTime )
	{
		//时间已经完成
		sendMsg.result = RE_EntrustTask_SpeedUpTimeError;
		_player->SendMsgToClient( MGPT_SC_SpeedUpEntrustTask_Ret, &sendMsg );
		return;
	}

	//判断消耗是否足够
	SEntrustTaskConfig* pConf = pMgr->GetEnstrustTaskConfig( it->second.m_Id );
	if( !pConf )
	{
		//数据异常
		sendMsg.result = RE_EntrustTask_DataIllegal;
		_player->SendMsgToClient( MGPT_SC_SpeedUpEntrustTask_Ret, &sendMsg );
		return;
	}

	int costType = pConf->speedupCost.key;
	int costNum = pConf->speedupCost.value;
	bool b = _player->IsCashEnough( costNum, costType );
	if( !b )
	{
		sendMsg.result = RE_EntrustTask_SpeedUpNoCost;
		_player->SendMsgToClient( MGPT_SC_SpeedUpEntrustTask_Ret, &sendMsg );
		return;
	}
	_player->ReduceCash( costNum, (CashType)costType, SYS_FUNC_TP_SpeedUpEntrustTask, 0);

	//操作成功
	it->second.m_EndTime = 0;
	EntrustDataToBase( it->second, sendMsg.m_Task );
	_player->SendMsgToClient( MGPT_SC_SpeedUpEntrustTask_Ret, &sendMsg );
}

void CEntrustTask::GetAwardEntrustTask( LC_ServerPlayer* _player, std::vector<int32_t>& _vec )
{
	if( !_player )
	{
		return;
	}

	CEntrustTaskConfigManager* pMgr = CEntrustTaskConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return;
	}

	if( _vec.empty() )
	{
		return;
	}

	MG_SC_GetAwardEntrustTask_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;

	int curTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	mem::map<int32_t, SItemConfig> addMap;	//要增加的道具
	//埋点
	vector<int> nParams; 
	nParams.clear();
	//验证任务是否合法
	for( int i = 0; i < _vec.size(); ++i )
	{
		mem::map<int32_t, SEntrustTaskData>::iterator it = m_AccpetTaskMap.find( _vec[i] );
		if( it == m_AccpetTaskMap.end() )
		{
			//任务不存在
			sendMsg.result = RE_EntrustTask_TaskIllegal;
			_player->SendMsgToClient( MGPT_SC_GetAwardEntrustTask_Ret, &sendMsg );
			return;
		}

		if( curTime < it->second.m_EndTime )
		{
			//时间未到
			sendMsg.result = RE_EntrustTask_RewardTimeError;
			_player->SendMsgToClient( MGPT_SC_GetAwardEntrustTask_Ret, &sendMsg );
			return;
		}

		SEntrustTaskConfig* pConf = pMgr->GetEnstrustTaskConfig( it->second.m_Id );
		if( !pConf )
		{
			//数据异常
			sendMsg.result = RE_EntrustTask_DataIllegal;
			_player->SendMsgToClient( MGPT_SC_GetAwardEntrustTask_Ret, &sendMsg );
			return;
		}

		//整合可以获取的奖励
		for( int t = 0; t < pConf->rewardVec.size(); ++t )
		{
			SItemConfig data( pConf->rewardVec[t].id, pConf->rewardVec[t].num, pConf->rewardVec[t].bind );
			int key = data.id * 10 + data.bind;
			mem::map<int32_t, SItemConfig>::iterator tempIt = addMap.find( key );
			if( tempIt != addMap.end() )
			{
				tempIt->second.num += data.num;
			}
			else
			{
				addMap[key].id = data.id;
				addMap[key].num = data.num;
				addMap[key].bind = data.bind;
			}
		}
		nParams.push_back(_vec[i]);
	}
	
	Utility::UT_SIMDataList kItems;
	for ( mem::map<int32_t, SItemConfig>::iterator it = addMap.begin(); 
			it != addMap.end(); ++it  )
	{
		UT_SIMDataInfo data;
		data.SetItemInfo( it->second.id, it->second.num, it->second.bind );
		kItems.push_back( data );
	}
	//判断背包能否放入
	if( LC_Helper::CanAddItemEx( _player, kItems, CIET_NONE ) != RE_SUCCESS )
	{
		sendMsg.result = RE_ITEM_REWARD_PACKAGE_FULL;
		_player->SendMsgToClient( MGPT_SC_GetAwardEntrustTask_Ret, &sendMsg );
		return;
	}

	//移除任务状态
	for( int i = 0; i < _vec.size(); ++i )
	{
		mem::map<int32_t, SEntrustTaskData>::iterator it = m_AccpetTaskMap.find( _vec[i] );
		if( it == m_AccpetTaskMap.end() )
		{
			sendMsg.result = RE_EntrustTask_DataError;
			_player->SendMsgToClient( MGPT_SC_GetAwardEntrustTask_Ret, &sendMsg );
			return;
		}

		//移除英雄或者宝具占用
		for( int k = 0; k < it->second.m_Vec.size(); ++k )
		{
			int id = it->second.m_Vec[k];

			mem::map<int,int>::iterator iter = m_UsedMap.find( id );
			if( iter != m_UsedMap.end() )
			{
				m_UsedMap.erase( iter );
			}
		}

		//移除任务
		m_AccpetTaskMap.erase( it );
	}
	//委派任务--埋点日志
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, WEIPAITASK_OPERATE, OPERATE_LOGTYPE3); 
	//获取奖励
	PackOperateInfoList kItemOPs;
	LC_Helper::BuildAddItemOPs(kItems, kItemOPs, SYS_FUNC_TP_GetAwardEntrustTask, 0);
	LC_Helper::DoItemOPs(_player, kItemOPs, SYS_FUNC_TP_GetAwardEntrustTask, 0, true, true,nLogCode, nParams);
	
	//操作成功
	sendMsg.m_Indexs.assign( _vec.begin(), _vec.end() );
	_player->SendMsgToClient( MGPT_SC_GetAwardEntrustTask_Ret, &sendMsg );
}

void CEntrustTask::UpdateEntrustTask( LC_ServerPlayer* _player )
{
	if( !_player )
	{
		return;
	}

	MG_SC_UpdateEntrustTask_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;

	int	 consumType = 1;
	if( m_Num >= GetGlobalSetting.EntrustTaskMaxRefreshNum )
	{
		//没有免费次数，优先消耗道具，再消耗金钱
		ResultType res = _player->TryDeleteItem( GetGlobalSetting.EntrustTaskRefreshItemId, 1) ;
		if( res != RE_SUCCESS )
		{
			int num = GetGlobalSetting.EntrustTaskUpdateCostNum;
			if( num <= 0 )
			{
				num = 100;
			}
			//道具不足，需要消耗金钱
			bool b = _player->IsCashEnough( num, CT_UNBIND_YUANBAO );
			if( !b )
			{
				sendMsg.result = RE_UNBIND_YUANBAO_NOT_ENOUGH;
				_player->SendMsgToClient( MGPT_SC_UpdateEntrustTask_Ret, &sendMsg );
				return;
			}
			consumType = 2;
		}
	}

	m_CostNum += 1;

	int32_t taskStar = 0;
	//判断是否有黄金特权 
	uint32_t res = LC_Helper::GetVipPrivilegeTimesByType( VIP_PC_EntrustTask_Num, _player );
	if(res > 0)
	{
		taskStar = 4;
		if( m_CostNum == 3 )
		{
			taskStar = 5;
		}
	}

	bool b = RandomEntrustTask( taskStar );
	if( !b )
	{
		sendMsg.result = RE_EntrustTask_DataError;
		_player->SendMsgToClient( MGPT_SC_UpdateEntrustTask_Ret, &sendMsg );
		m_CostNum -= 1;
		return;
	}
	//委派任务--埋点日志
	vector<int> nParams; 
	nParams.clear();
	nParams.push_back(m_CostNum);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, WEIPAITASK_OPERATE, OPERATE_LOGTYPE2); 
	if( consumType == 1 )
	{
		//消耗道具
		_player->DeleteItem( GetGlobalSetting.EntrustTaskRefreshItemId, 1, nLogCode, nParams);
	}
	else
	{
		//消耗金钱
		int num = GetGlobalSetting.EntrustTaskUpdateCostNum;
		_player->ReduceCash(num, CT_UNBIND_YUANBAO, SYS_FUNC_TP_UPDATEENTRUSTTASK, 0,nLogCode, nParams);
	}

	//操作成功
	m_Num += 1;	//次数增加
	for( mem::list<int>::iterator it = m_TaskList.begin(); it != m_TaskList.end(); ++it )
	{
		sendMsg.m_Tasks.push_back( (*it) );
	}
	sendMsg.m_Num = m_Num;
	_player->SendMsgToClient( MGPT_SC_UpdateEntrustTask_Ret, &sendMsg );
}

bool CEntrustTask::RandomEntrustTask( int32_t _star )
{
	CEntrustTaskConfigManager* pMgr = CEntrustTaskConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return false;
	}
	return pMgr->GetRandomEnstrustTask( m_TaskList, GetGlobalSetting.EntrustTaskMaxTaskNum, _star );
}

void CEntrustTask::EntrustDataToBase( SEntrustTaskData& _data, Protocol::EntrustTask_Base& _info )
{
	_info.m_Index = _data.m_Index;
	_info.m_Id = _data.m_Id;
	_info.m_EndTime = _data.m_EndTime;
	_info.m_Vec.assign( _data.m_Vec.begin(), _data.m_Vec.end() );
}

int CEntrustTask::CheckHeroCondition( LC_ServerPlayer* _player, std::vector<int32_t>& _vec, SEntrustTaskConfig* _pConf )
{
	if( !_player || !_pConf )
	{
		return RE_FAIL;
	}

	int32_t maxLevel = 0;	//英雄的最大星级等级
	mem::set<int32_t> elementSet;	//英雄的元素类型集合
	for( int i = 0; i < _vec.size(); ++i )
	{
		BuddyInstance* pBuddy = _player->GetBuddyInstanceByID( _vec[i] );
		if( !pBuddy )
		{
			return RE_EntrustTask_HeroNoExist;
		}
		//获取星级
		int32_t starLevel = pBuddy->GetStarLevel();
		if( starLevel > maxLevel )
		{
			maxLevel = starLevel;
		}

		//获取元素类型
		CF_Buddy::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(_vec[i]);
		if( !pkData )
		{
			return RE_EntrustTask_HeroNoExist;
		}
		elementSet.insert( pkData->job );
	}

	//判断条件
	for( int i = 0; i < _pConf->conditionVec.size(); ++i )
	{
		int key = _pConf->conditionVec[i].key; 			//条件id
		int value = _pConf->conditionVec[i].value; 		//值
		if( key == 1 )
		{
			//判断星级
			if( maxLevel < value )
			{
				return RE_EntrustTask_ConditionError;
			}
		}
		else if( key == 2 )
		{
			//判断元素是否符合
			mem::set<int32_t>::iterator it = elementSet.find( value );
			if( it == elementSet.end() )
			{
				return RE_EntrustTask_ConditionError;
			}
		}
	}

	return RE_SUCCESS;
}

int CEntrustTask::CheckRuneCondition( LC_ServerPlayer* _player, std::vector<int32_t>& _vec, SEntrustTaskConfig* _pConf )
{
	if( !_player || !_pConf )
	{
		return RE_FAIL;
	}

	int32_t maxLevel = 0;	//宝具的最大星级等级
	int32_t maxQuality = 0;	//宝具的最大品质
	for( int i = 0; i < _vec.size(); ++i )
	{
		const LC_BackPackEntry* pRune = _player->GetItemEntryByItemID( _vec[i] );
		if( !pRune )
		{
			return RE_EntrustTask_RuneNoExist;
		}

		//获取星级
		int32_t starLevel = pRune->GetBackPackValueByType(LCSPET_EQUIP_STAR);
		if( starLevel > maxLevel )
		{
			maxLevel = starLevel;
		}

		//获取品质
		CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(_vec[i]);
		if( !pkData )
		{
			return RE_EntrustTask_RuneNoExist;
		}
		if( pkData->_iGrade > maxQuality )
		{
			maxQuality = pkData->_iGrade;
		}
	}

	//判断条件
	for( int i = 0; i < _pConf->conditionVec.size(); ++i )
	{
		int key = _pConf->conditionVec[i].key; 			//条件id
		int value = _pConf->conditionVec[i].value; 		//值
		if( key == 1 )
		{
			//判断星级
			if( maxLevel < value )
			{
				return RE_EntrustTask_ConditionError;
			}
		}
		else if( key == 2 )
		{
			//判断品质
			if( maxQuality < value )
			{
				return RE_EntrustTask_ConditionError;
			}
		}
	}

	return RE_SUCCESS;
}
