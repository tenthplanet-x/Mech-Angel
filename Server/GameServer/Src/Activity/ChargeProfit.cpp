#include "ChargeProfit.h"
#include "LC_ServerPlayer.h"
#include "LC_Helper.h"
#include "char_data.pb.h"
#include "ChargeProfitConfigManager.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Utility;

CChargeProfit::CChargeProfit()
{
	m_ChargeProfitMap.clear();
}

CChargeProfit::~CChargeProfit()
{
	m_ChargeProfitMap.clear();
}

bool CChargeProfit::SaveData( char_data_define::PBChargeProfitData* _pData )
{
	if( !_pData )
	{
		return false;
	}

	for( mem::map<int, SChargeProfit >::iterator it = m_ChargeProfitMap.begin(); 
			it != m_ChargeProfitMap.end(); ++it )
	{
		char_data_define::PBChargeProfitInfo* p = _pData->add_info();
		if( p )
		{
			p->set_id( it->first );
			p->set_totalnum( it->second.m_DayCount );
			p->set_time( it->second.m_Timestamp );
			for( mem::map<int,int>::iterator temp = it->second.m_Status.begin(); 
					temp != it->second.m_Status.end(); ++temp )
			{
				char_data_define::PBKeyValue* k = p->add_status();
				if( k )
				{
					k->set_key( temp->first );
					k->set_value( temp->second );
				}
			}
		}
	}

	return true;
}

bool CChargeProfit::LoadData( const char_data_define::PBChargeProfitData& _data )
{
	for( int i = 0; i < _data.info_size(); ++i )
	{
	    const char_data_define::PBChargeProfitInfo& info = _data.info(i);
		SChargeProfit data;
		int id = info.id();
		data.m_DayCount = info.totalnum();
		data.m_Timestamp = info.time();
		for( int temp = 0; temp < info.status_size(); ++temp )
		{
			int key = info.status(temp).key();
			int value = info.status(temp).value();
			data.m_Status[key] = value;
		}
		m_ChargeProfitMap[id] = data;
	}
	return true;
}

void CChargeProfit::ResetData( LC_ServerPlayer* _player )
{
	if( !_player )
	{
		return;
	}

	CChargeProfitConfigManager* pMgr = CChargeProfitConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return;
	}

	for( mem::map<int, SChargeProfit >::iterator it = m_ChargeProfitMap.begin(); 	it != m_ChargeProfitMap.end(); ++it )
	{
		SChargeProfitConfig* pkConfig = pMgr->GetChargeProfitConfig(it->first);
		if(NULL == pkConfig)
		{
			continue;
		}

		switch(pkConfig->m_Type)
		{
		case Charge_Type_1:
			{
				it->second.m_DayCount = 0;
			}			
			break;
		default:
			return;
		}
	}
	
	if( !m_ChargeProfitMap.empty() )
	{
		SendChargeProfitData( _player );
	}
}

void CChargeProfit::SendChargeProfitData( LC_ServerPlayer* _player )
{
	if( !_player )
	{
		return;
	}

	MG_SC_SynChargeProfit_Ret sendMsg;
	for( mem::map<int, SChargeProfit >::iterator it = m_ChargeProfitMap.begin(); 
			it != m_ChargeProfitMap.end(); ++it )
	{
		ChargeProfit_Base data;
		data.m_Id = it->first;
		data.m_Timestamp = it->second.m_Timestamp;
		data.m_TotalNum = it->second.m_DayCount;
		for( mem::map<int,int>::iterator temp = it->second.m_Status.begin(); 
				temp != it->second.m_Status.end(); ++temp )
		{
			data.m_Status[temp->first] = temp->second;
		}
		sendMsg.m_ChargeProfitVec.push_back( data );
	}

	_player->SendMsgToClient( MGPT_SC_SynChargeProfit_Ret, &sendMsg );
}

void CChargeProfit::ChargeProfitReward( LC_ServerPlayer* _player, int32_t _id, int _param )
{
	if( !_player )
	{
		return;
	}
	
	CChargeProfitConfigManager* pMgr = CChargeProfitConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return;
	}

	MG_SC_ChargeProfit_Reward_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;

	//验证数据是否合法
	SChargeProfitConfig* conf = pMgr->GetChargeProfitConfig( _id );
	if( !conf )
	{
		//数据异常
		sendMsg.result = RE_ChargeProfit_NoId;
		_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
		return;
	}

	if( conf->m_CondtionVec.size() != conf->m_LimitNumVec.size() ||
		conf->m_CondtionVec.size() != conf->m_RewardVec.size() )
	{
		sendMsg.result = RE_ChargeProfit_SizeError;
		_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
		return; 
	}
	
	//判断时间是否有效（展示时间内，都可以领奖）
	if( !UT_ServerHelper::CheckActivityVaild( conf->m_FinishSchedule ) )
	{
		sendMsg.result = RE_ChargeProfit_TimeInvailid;
		_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
		return; 
	}

	//验证param是否有效
	if( _param >= conf->m_CondtionVec.size() )
	{
		sendMsg.result = RE_ChargeProfit_ParamError;
		_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
		return; 
	}

	SChargeProfit& playerData = m_ChargeProfitMap[_id];
	//判断能不能领取
	mem::map<int,int>::iterator it = playerData.m_Status.find( _param );
	if( it == playerData.m_Status.end() )
	{
		sendMsg.result = RE_ChargeProfit_TimeError;
		_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
		return;
	}

	//判断是否已经领取过了
	if( it->second == 2 )
	{
		sendMsg.result = RE_ChargeProfit_Used;
		_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
		return;
	}

	//判断背包能否放入
	Utility::UT_SIMDataList& kItems = conf->m_RewardVec[_param];
	if( LC_Helper::CanAddItemEx( _player, kItems, CIET_NONE ) != RE_SUCCESS )
	{
		sendMsg.result = RE_ITEM_REWARD_PACKAGE_FULL;
		_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
		return;
	}
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(_id);
	//邂逅好礼
	int nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY20_OPERATE, OPERATE_LOGTYPE1);
	//获取奖励
	PackOperateInfoList kItemOPs;
	LC_Helper::BuildAddItemOPs(kItems, kItemOPs, SYS_FUNC_TP_ChargeProfitReward, 0);
	LC_Helper::DoItemOPs(_player, kItemOPs, SYS_FUNC_TP_ChargeProfitReward, 0, true, true, nLogCode,nParams);
	
	//组织数据
	playerData.m_Status[_param] = 2;

	//发送数据
	sendMsg.m_Id = _id;
	sendMsg.m_Param.key = _param;
	sendMsg.m_Param.value = 2;
	_player->SendMsgToClient( MGPT_SC_ChargeProfit_Reward_Ret, &sendMsg );
}

void CChargeProfit::ModifyValue( LC_ServerPlayer* _player, int32_t cashType, int _value, int type )
{
	if( !_player )
	{
		return;
	}

	CChargeProfitConfigManager* pMgr = CChargeProfitConfigManager::GetSingletonPtr();
	if( !pMgr )
	{
		return; 
	}

	bool b = false;
	mem::map<int,int> timeMap; 
	pMgr->GetChargeProfitScheduleByCashType( cashType, type, timeMap );
	for(mem::map<int,int>::iterator it = timeMap.begin(); it != timeMap.end(); ++it)
	{
		if( UT_ServerHelper::CheckActivityVaild( it->second ) )
		{
			SChargeProfit& playerData = m_ChargeProfitMap[it->first];
			playerData.m_DayCount += _value;
			b = true;
			SChargeProfitConfig* conf = pMgr->GetChargeProfitConfig( it->first );
			if( conf )
			{
				if( conf->m_CondtionVec.size() != conf->m_LimitNumVec.size() || conf->m_CondtionVec.size() != conf->m_RewardVec.size() )
				{
					continue;
				}

				int param = (int)playerData.m_Status.size();

				switch(conf->m_Type)
				{
				case Charge_Type_1:
					{
						if( param < conf->m_CondtionVec.size() )
						{
							int curTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
							if(playerData.m_DayCount >= conf->m_CondtionVec[param] && curTime > playerData.m_Timestamp)
							{
								playerData.m_Timestamp = (int32_t)LC_Helper::GetTimeStampInCurDay(0, 0, 0) + 24 * 3600;
								playerData.m_Status[param] = 1;
							}
						}
					}
					break;
				case Charge_Type_2:
					{
						for(uint32_t i=param; i<conf->m_CondtionVec.size(); ++i)
						{
							if(playerData.m_DayCount >= conf->m_CondtionVec[i])
							{
								playerData.m_Status[i] = 1;
							}
						}
					}
					break;
				case Charge_Type_3:
					{
						_player->UpdateRankInfoToRedis(type, false, it->first, playerData.m_DayCount);
					}
					break;
				}
			}
		}
	}

	if( b )
	{
		SendChargeProfitData( _player );
	}
}

void CChargeProfit::FillCanChargeProfitReward(int id, Utility::UT_SIMDataList& rewardItemList)
{
	rewardItemList.clear();
	rewardItemList.reserve(10);

	CChargeProfitConfigManager* pMgr = CChargeProfitConfigManager::GetSingletonPtr();
	SChargeProfitConfig* conf = pMgr->GetChargeProfitConfig(id);
	if(NULL == conf)
	{
		return;
	}

	mem::map<int, SChargeProfit>::iterator findIt = m_ChargeProfitMap.find(id);
	if(findIt == m_ChargeProfitMap.end())
	{
		return;
	}

	for(mem::map<int,int>::iterator it=findIt->second.m_Status.begin(); it!=findIt->second.m_Status.end(); ++it)
	{
		if(1==it->second && conf->m_RewardVec.size()>it->first)
		{
			rewardItemList.insert(rewardItemList.end(), conf->m_RewardVec[it->first].begin(), conf->m_RewardVec[it->first].end());
		}
	}
}

void CChargeProfit::ResetChargeProfitInfo(int id)
{
	mem::map<int, SChargeProfit>::iterator findIt = m_ChargeProfitMap.find(id);
	if(findIt == m_ChargeProfitMap.end())
	{
		return;
	}

	m_ChargeProfitMap.erase(findIt);
}