#include "Lottery.h"
#include "LC_ServerPlayer.h"
#include "LC_Helper.h"
#include "char_data.pb.h"
#include "LotteryConfigManager.h"
#include "LC_Define.h"
#include "LC_GameEventManager.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Utility;

CLottery::CLottery()
{
	m_TotalNum = 0;
}

CLottery::~CLottery()
{
	if( !m_LotteryDataMap.empty() )
	{
		m_LotteryDataMap.clear();
	}
}

bool CLottery::SaveData( char_data_define::PBLotteryData* _pData )
{
	if( !_pData )
	{
		return false;
	}

	_pData->set_totalnum( m_TotalNum );

	for( mem::map<int, SLotteryData >::iterator it = m_LotteryDataMap.begin(); 
			it != m_LotteryDataMap.end(); ++it )
	{
		char_data_define::PBLotteryInfo* p = _pData->add_info();
		if( p )
		{
			p->set_id( it->first );
			p->set_free( it->second.m_Free );
			p->set_daycount( it->second.m_DayCount );
			p->set_totalcount( it->second.m_TotalCount );
			p->set_rewardnum( it->second.m_RewardNum );
			for( mem::set<int>::iterator temp = it->second.m_Status.begin(); 
					temp != it->second.m_Status.end(); ++temp )
			{
				p->add_status( *temp );
			}
		}
	}

	return true;
}

bool CLottery::LoadData( const char_data_define::PBLotteryData& _data )
{
	m_TotalNum = _data.totalnum();
	for( int i = 0; i < _data.info_size(); ++i )
	{
	    const char_data_define::PBLotteryInfo& info = _data.info(i);
		SLotteryData data;
		int id = info.id();
		data.m_Free = info.free();
		data.m_DayCount = info.daycount();
		data.m_TotalCount = info.totalcount();
		data.m_RewardNum = info.rewardnum();
		for( int temp = 0; temp < info.status_size(); ++temp )
		{
			data.m_Status.insert( info.status(temp) );
		}
		m_LotteryDataMap[id] = data;
	}
	return true;
}

void CLottery::ResetData( LC_ServerPlayer* _player )
{
	if( !_player )
	{
		return;
	}

	for( mem::map<int, SLotteryData >::iterator it = m_LotteryDataMap.begin(); 
			it != m_LotteryDataMap.end(); ++it )
	{
		it->second.m_DayCount = 0;
		it->second.m_Free = true;
	}

	SendLotteryData( _player );
}

void CLottery::SendLotteryData( LC_ServerPlayer* _player )
{
	if( !_player )
	{
		return;
	}

	MG_SC_SyncLottery_Ret sendMsg;
	sendMsg.m_TotalNum = m_TotalNum;
	for( mem::map<int, SLotteryData >::iterator it = m_LotteryDataMap.begin(); 
			it != m_LotteryDataMap.end(); ++it )
	{
		Lottery_Base data;
		data.m_Id = it->first;
		data.m_Free = it->second.m_Free;
		data.m_DayCount = it->second.m_DayCount;
		data.m_TotalCount = it->second.m_TotalCount;
		data.m_RewardNum = it->second.m_RewardNum;
		data.m_Status.assign( it->second.m_Status.begin(), it->second.m_Status.end() );
		sendMsg.m_LotteryVec.push_back( data );
	}

	_player->SendMsgToClient( MGPT_SC_SyncLottery_Ret, &sendMsg );
}

void CLottery::LotteryDraw( LC_ServerPlayer* _player, int32_t _id, int _num )
{
	if( !_player )
	{
		return;
	}

	CLotteryConfigManager* pMgr = CLotteryConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return; 
	}

	LC_PackAsset* pkPackAsset = _player->GetPackAsset();
	if( !pkPackAsset)
	{
		return;
	}
	
	MG_SC_Lottery_Draw_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;

	//判断寻宝是否存在
	SLotteryConfig* conf = pMgr->GetLotteryConfig( _id );
	if( !conf )
	{
		sendMsg.result = RE_Lottery_NoId;
		_player->SendMsgToClient( MGPT_SC_Lottery_Draw_Ret, &sendMsg );
		return;
	}

	//判断时间是否有效
	if(-1!=conf->m_Schedule && !UT_ServerHelper::CheckActivityVaild( conf->m_Schedule ) )
	{
		sendMsg.result = RE_Lottery_TimeInvailid;
		_player->SendMsgToClient( MGPT_SC_Lottery_Draw_Ret, &sendMsg );
		return; 
	}

	//校正参数_num
	if( _num < 0 )
	{
		_num = 1;
	}
	else if( _num > 1 )
	{
		_num = conf->m_MaxNum;
	}

	SLotteryData& playerData = m_LotteryDataMap[_id];

	//判断今日抽奖次数
	if( _num != 1 || !playerData.m_Free )
	{
		if( playerData.m_DayCount + _num > conf->m_DayMaxNum )
		{
			sendMsg.result = RE_LOTTERY_ERROR_2;
			_player->SendMsgToClient( MGPT_SC_Lottery_Draw_Ret, &sendMsg );
			return;
		}
	}

	//判断背包是否足够
	if( pkPackAsset->GetPackEmptyEntryCount(PACK_TYPE_BASIC) < _num )
	{
		sendMsg.result = RE_ITEM_PACKAGE_FULL;
		_player->SendMsgToClient( MGPT_SC_Lottery_Draw_Ret, &sendMsg );
		return;
	}

	bool freeFlag = false;	//是否免费抽取
	int consumType = 0;	//1道具 2钻石 其他免费
	int consumNum = conf->m_Cost * _num;	//消耗的钻石数量
	int itemNum = _num;	//消耗的道具数
	Utility::UT_SIMDataList consumItem = conf->m_ItemCost;	//消耗的道具
	for( Utility::UT_SIMDataList::iterator it = consumItem.begin();
		 it != consumItem.end(); ++it )
	{
		(*it).ItemCount( itemNum );
	}

	//判断是1次抽取还是10连抽,1次抽取，判断免费状态
	if( _num == 1 )
	{
		if( playerData.m_Free )
		{
			freeFlag = true;
			playerData.m_Free = false;
			goto FreeOperator;
		}
	}
	else
	{
		//10连抽需要判断vip折扣 有vip等级判断vip折扣,再判断道具或者钻石消耗是否足够
		//得到vip参数
		int vipLevel = _player->GetVIPAsset().GetVIPLevel();
		if( vipLevel >= 0 )
		{
			LC_VIPCmnManager* vipMgr = LC_VIPCmnManager::GetSingletonPtr();
			if( vipMgr )
			{
				LC_VIPProfitDataEntry* p = vipMgr->GetVIPProfitDataEntry( conf->m_VipItem );
				if( p )
				{
					itemNum = p->GetValue( vipLevel, false );
					for( Utility::UT_SIMDataList::iterator it = consumItem.begin();
						 it != consumItem.end(); ++it )
					{
						(*it).ItemCount( itemNum );
					}
				}

				p = vipMgr->GetVIPProfitDataEntry( conf->m_VipCost );
				if( p )
				{
					consumNum = p->GetValue( vipLevel, false );
				}
			}
		}
	}

	//判断消耗是否足够，先消耗道具，再消耗钻石
	for( Utility::UT_SIMDataList::iterator it = consumItem.begin();
		 it != consumItem.end(); ++it )
	{
		ResultType res = _player->TryDeleteItem( (*it).GetItemId(), (*it).GetItemNum() ) ;
		if( res == RE_SUCCESS )
		{
			consumType = 1;	//消耗道具
		}
		else
		{
			consumType = 2;	//消耗钻石
			//道具不足，需要消耗金钱
			bool b = _player->IsCashEnough( consumNum, CT_UNBIND_YUANBAO );
			if( !b )
			{
				sendMsg.result = RE_UNBIND_YUANBAO_NOT_ENOUGH;
				_player->SendMsgToClient( MGPT_SC_Lottery_Draw_Ret, &sendMsg );
				return;
			}
		}
	}

FreeOperator:
	//抽取奖励
	Utility::UT_SIMDataList rewardItem;	//奖励的道具
	bool bReward = pMgr->GetLotteryPondReward( rewardItem, _id, playerData.m_TotalCount, _num, freeFlag );
	if( !bReward )
	{
		sendMsg.result = RE_Lottery_DrawVailid;
		_player->SendMsgToClient( MGPT_SC_Lottery_Draw_Ret, &sendMsg );
		return;
	}
	//宝具召唤
	int nLogCode = 0;
	if(_num == 1)
	{
		if(_id == 1)
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY8_OPERATE, OPERATE_LOGTYPE1);
		}
		else
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY9_OPERATE, OPERATE_LOGTYPE1);
		}
	}
	else
	{
		if(_id == 1)
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY8_OPERATE, OPERATE_LOGTYPE2);
		}
		else
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY9_OPERATE, OPERATE_LOGTYPE2);
		}
			
	}
	//扣除消耗
	if( consumType == 1 )
	{
		for( Utility::UT_SIMDataList::iterator it = consumItem.begin();
			 it != consumItem.end(); ++it )
		{
			_player->DeleteItem( (*it).GetItemId(), (*it).GetItemNum(), nLogCode) ;
		}
	}
	else if( consumType == 2 )
	{
		_player->ReduceCash( consumNum, CT_UNBIND_YUANBAO, SYS_FUNC_TP_LotteryDraw, 0, nLogCode);
		//增加寻宝消耗的钻石数量记录
		_player->AddLotteryConsumeUnBindYuanBao( consumNum );
	}

	//奖励放入背包
	PackOperateInfoList kItemOPs;
	LC_Helper::BuildAddItemOPs(rewardItem, kItemOPs, SYS_FUNC_TP_LotteryDraw, 0);
	LC_Helper::DoItemOPs(_player, kItemOPs, SYS_FUNC_TP_LotteryDraw, 0, true, true, nLogCode);

	

	//组织数据
	if( !freeFlag )
	{
		//不是免费抽取
		playerData.m_DayCount += _num;	
	}
	playerData.m_TotalCount += _num;
	m_TotalNum += _num;
	//抽奖事件派发
	AchievementEvent LotteryEvent;
	LotteryEvent.SetNum(_num);
	_player->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_LOTTERY, LotteryEvent);

	//任务系统
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent)
	{
		pkGameEvent->SetEventType(GLET_ACTI_STAT_OP);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
		pkGameEvent->SetEventParam(EP_ACTOP_PLAYER_ID, _player->GetID());
		pkGameEvent->SetEventParam(EP_ACTOP_ACTITYPE, Activity::ACT_SCHEDULE_TP_LOTTERY);
		pkGameEvent->SetEventParam(EP_ACTOP_CATEGORY, Activity::ACT_SCHEDULE_TP_LOTTERY);

		pkGameEvent->SetEventParam(EP_ACTOP_OPCODE, Activity::ACT_COMMON_OP_DRAW);
		pkGameEvent->SetEventParam(EP_ACTOP_COUNT, _num);
		pkGameEvent->SetEventParam(EP_ACTOP_MAP_ID, _player->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
	}

	//发送数据
	sendMsg.m_Id = _id;
	sendMsg.m_DayCount = playerData.m_DayCount;
	sendMsg.m_TotalCount = playerData.m_TotalCount;
	sendMsg.m_Free = playerData.m_Free;
	sendMsg.m_TotalNum = m_TotalNum;
	for( Utility::UT_SIMDataList::iterator it = rewardItem.begin();
		 it != rewardItem.end(); ++it )
	{
		KeyValue_Base base;
		base.key = (*it).GetItemId();
		base.value = (*it).GetItemNum();
		sendMsg.m_ItemVec.push_back( base );

		int broadId = (*it).ParamC();
		if( broadId != 0 )
		{
			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			noticeFmort.SetMsgID( broadId );
			noticeFmort.AddString( _player->GetOwnerCharName() );
			noticeFmort.AddActivityId( _id );
			noticeFmort.AddItemQuality( base.key );
			noticeFmort.AddItemName( base.key );
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, _player->GetCitizenship());
		}
	}
	_player->SendMsgToClient( MGPT_SC_Lottery_Draw_Ret, &sendMsg );
}

void CLottery::LotteryExtraReward( LC_ServerPlayer* _player, int32_t _id, int _param )
{
	if( !_player )
	{
		return;
	}

	CLotteryConfigManager* pMgr = CLotteryConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return; 
	}

	LC_PackAsset* pkPackAsset = _player->GetPackAsset();
	if( !pkPackAsset)
	{
		return;
	}

	MG_SC_Lottery_ExtraReward_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;
	sendMsg.m_Id = _id;
	
	//判断寻宝是否存在
	SLotteryConfig* conf = pMgr->GetLotteryConfig( _id );
	if( !conf )
	{
		sendMsg.result = RE_Lottery_NoId;
		_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
		return;
	}
	
	//判断时间是否有效
	if(-1!=conf->m_ExtraSchedule && !UT_ServerHelper::CheckActivityVaild( conf->m_ExtraSchedule ) )
	{
		sendMsg.result = RE_Lottery_TimeInvailid;
		_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
		return; 
	}
	
	//判断背包是否足够
	if( pkPackAsset->GetPackEmptyEntryCount(PACK_TYPE_BASIC) < 1 )
	{
		sendMsg.result = RE_ITEM_PACKAGE_FULL;
		_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
		return;
	}

	SLotteryData& playerData = m_LotteryDataMap[_id];

	Utility::UT_SIMDataList rewardItem;	//奖励的道具

	if( conf->m_Category == 1 )
	{
		if(0 == conf->m_CostNum)
		{
			sendMsg.result = RE_Lottery_Reward_Error;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}
		
		//水晶球奖励
		//判断是否满足条件
		int32_t remainNum = playerData.m_TotalCount-playerData.m_RewardNum;
		int32_t remainTimes = remainNum / conf->m_CostNum;
		if(remainTimes < 1)
		{
			sendMsg.result = RE_Lottery_Reward_NoNum;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}

		int level = _player->GetControllerLevel();
		int result = pMgr->GetLotteryItemReward( rewardItem, conf->m_ItemRewardId, level, remainTimes );
		if( result != RE_SUCCESS )
		{
			sendMsg.result = result;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}

		//组织数据
		playerData.m_RewardNum += remainTimes*conf->m_CostNum;
		sendMsg.m_Param = playerData.m_RewardNum;
	}
	else if( conf->m_Category == 2 )
	{
		//次数奖励
		SLotteryRewardConfig* pReward = pMgr->GetLotteryRewardConfig( _id );
		if( !pReward )
		{
			sendMsg.result = RE_Lottery_Reward_NoId;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}

		if( pReward->m_NumVec.size() != pReward->m_RewardVec.size() )
		{
			sendMsg.result = RE_Lottery_Reward_SizeError;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}

		if( _param >= pReward->m_NumVec.size() )
		{
			sendMsg.result = RE_Lottery_Reward_ParamError;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}

		//判断是否已经领取
		mem::set<int32_t>::iterator it = playerData.m_Status.find( _param );
		if( it != playerData.m_Status.end() )
		{
			sendMsg.result = RE_Lottery_Reward_Used;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}

		//判断抽奖次数是否大于领取条件
		if( playerData.m_TotalCount < pReward->m_NumVec[_param] )
		{
			sendMsg.result = RE_Lottery_Reward_NoCount;
			_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
			return;
		}

		//得到奖励
		rewardItem.push_back( pReward->m_RewardVec[_param] );

		//组织数据
		playerData.m_Status.insert( _param );
		sendMsg.m_Param = _param;
	}
	//宝具宝藏/神秘抽奖
	int nLogCode = 0;
	if(_id == 1)
	{
		nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY8_OPERATE, OPERATE_LOGTYPE3);
	}
	else
	{
		nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY9_OPERATE, OPERATE_LOGTYPE3);
	}

	
	//奖励放入背包
	PackOperateInfoList kItemOPs;
	LC_Helper::BuildAddItemOPs(rewardItem, kItemOPs, SYS_FUNC_TP_LotteryDraw, 0);
	LC_Helper::DoItemOPs(_player, kItemOPs, SYS_FUNC_TP_LotteryReward, 0, true, true, nLogCode);

	//发送数据
	for( Utility::UT_SIMDataList::iterator it = rewardItem.begin();
		 it != rewardItem.end(); ++it )
	{
		KeyValue_Base base;
		base.key = (*it).GetItemId();
		base.value = (*it).GetItemNum();
		sendMsg.m_ItemVec.push_back( base );
	}
	_player->SendMsgToClient( MGPT_SC_Lottery_ExtraReward_Ret, &sendMsg );
}

int CLottery::GetLotteryTotalNum(LC_ServerPlayer* _player)
{
	return m_TotalNum;
}
