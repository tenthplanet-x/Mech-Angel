#include "LC_ServerPlayerRecoverTimes.h"
#include "LC_ServerPlayer.h"
#include "MG_SyncProtocol.h"
#include "LC_GameEventManager.h"

CRecoverTimes::CRecoverTimes(uint32_t revertTime, uint32_t revertMaxTimes, int32_t dropType, int32_t broadCastID) 
: m_nNextRevertTime(0), m_nEndRevertTime(0), m_nRevertTime(revertTime), m_nRevertMaxTimes(revertMaxTimes), m_nType(dropType), m_nBroadCastID(broadCastID)
{
}

CRecoverTimes::~CRecoverTimes()
{

}

void CRecoverTimes::CheckRecover(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	assert(0 != m_nRevertTime);

	//未到下次恢复时间，不恢复
	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(0==m_nNextRevertTime || currentTime < m_nNextRevertTime || 0==m_nRevertTime)
	{
		return;
	}

	//次数达到最大限制，不需要恢复
	int32_t times = GetCurrentTimes(pkOwnerPlayer);
	if(times >= m_nRevertMaxTimes)
	{
		return;
	}

	uint32_t deltaTime = currentTime-m_nNextRevertTime;
	int32_t addTimes = deltaTime/m_nRevertTime + 1;
	if(addTimes > 0)
	{
		AddTimes(pkOwnerPlayer, addTimes);
	}
}

bool CRecoverTimes::AddTimes(LC_ServerPlayer* pkOwnerPlayer, int times,bool checkMax)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	int32_t oldTimes = GetCurrentTimes(pkOwnerPlayer);
	int32_t newTimes = oldTimes+times;
	if(checkMax)
	{
		newTimes = newTimes>m_nRevertMaxTimes ? m_nRevertMaxTimes : newTimes;
	}

	int32_t deltaTimes = newTimes-oldTimes;
	if(deltaTimes <= 0)
	{
		return false;
	}

	int remainTimes = m_nRevertMaxTimes-newTimes > 0 ? m_nRevertMaxTimes-newTimes : 0;
	if(0 == remainTimes)
	{
		m_nNextRevertTime = 0;
		m_nEndRevertTime = 0;
	}
	else if(remainTimes > 0)
	{
		if(0 == m_nNextRevertTime)
		{
			m_nNextRevertTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		}

		//道具增加的次数，不影响恢复次数的倒计时
		if(checkMax)
		{
			m_nNextRevertTime += m_nRevertTime*times;
		}
		m_nEndRevertTime = m_nNextRevertTime+(m_nRevertTime*(remainTimes-1));
	}

	return DoAddTimes(pkOwnerPlayer, deltaTimes);
}

bool CRecoverTimes::ReduceTimes(LC_ServerPlayer* pkOwnerPlayer, int times)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	int32_t oldTimes = GetCurrentTimes(pkOwnerPlayer);
	if(oldTimes < times)
	{
		return false;
	}

	//扣除次数后，是否会触发恢复次数
	int deltaTimes = oldTimes - times - m_nRevertMaxTimes;
	if(deltaTimes < 0)
 	{
		//更新最后一次恢复时间戳
		if(0 == m_nNextRevertTime)
		{
			int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
			m_nNextRevertTime = currentTime+ m_nRevertTime;
			m_nEndRevertTime = currentTime + (m_nRevertTime*(-deltaTimes));
		}
		else
		{
			m_nEndRevertTime = m_nEndRevertTime + (m_nRevertTime*times);
		}
	}

	bool tag = DoReduceTimes(pkOwnerPlayer, times);
	if(tag && m_nType == RecoverType_BossDrop)
	{
		LC_GameEventManager* pkGameEventManager = LC_GameEventManager::GetSingletonPtr();
		{
			LC_GameEvent* pkGameEvent = pkGameEventManager->AllocGameEvent();
			if (NULL != pkGameEvent)
			{
				pkGameEvent->SetEventType(GLET_KILLBOSS_DROPTIMES);
				pkGameEvent->SetEventParam(EP_KILLBOSS_DROP_PLAYER_ID, pkOwnerPlayer->GetID());
				pkGameEvent->SetEventParam(EP_KILLBOSS_DROP_MAPID, pkOwnerPlayer->GetMapResID());
				pkGameEventManager->InsertGameEvent(pkGameEvent);
			}
		}
	}
	return tag;
}
void CRecoverTimes::BroadCastRecoverTimesMsg(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	if(0 != m_nBroadCastID)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(m_nBroadCastID);
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkOwnerPlayer->GetCitizenship(), pkOwnerPlayer);
	}	
}
//////////////////////
CDropTimes::CDropTimes(uint32_t revertTime, uint32_t revertMaxTimes, int32_t dropType, int32_t broadCastID) : CRecoverTimes(revertTime, revertMaxTimes, dropType, broadCastID), m_nDropTimes(0)
{

}

CDropTimes::~CDropTimes()
{

}

void CDropTimes::LoadData(const ::char_data_define::PBRecoverTimesData& data)
{
	m_nDropTimes = data.times();
	m_nNextRevertTime = data.nextreverttime();
	m_nEndRevertTime = data.endreverttime();
}

void CDropTimes::SaveData(::char_data_define::PBRecoverTimesData* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	pkData->set_times(m_nDropTimes);
	pkData->set_nextreverttime(m_nNextRevertTime);
	pkData->set_endreverttime(m_nEndRevertTime);
}

int32_t CDropTimes::GetCurrentTimes(LC_ServerPlayer* pkOwnerPlayer)
{
	return m_nDropTimes;
}

bool CDropTimes::DoAddTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	m_nDropTimes += times;

	SyncBossDropInfo(pkOwnerPlayer);

	return true;
}

bool CDropTimes::DoReduceTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	if(m_nDropTimes < 1)
	{
		return false;
	}

	m_nDropTimes -= 1;

	return SyncBossDropInfo(pkOwnerPlayer);
}

bool CDropTimes::SyncBossDropInfo(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	MG_SyncBossDropInfo msg;
	msg.m_nType = m_nType;
	msg.m_nDropTimes = m_nDropTimes;
	msg.m_nNextRevertTime = m_nNextRevertTime;
	msg.m_nEndRevertTime = m_nEndRevertTime;
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCBOSSDROPINFO, &msg);

	return true;
}
///////////////////////////////////////////////////////////////
CYuanZhengPoint::CYuanZhengPoint(uint32_t revertTime, uint32_t revertMaxTimes, int32_t dropType, int32_t broadCastID)  : CRecoverTimes(revertTime, revertMaxTimes, dropType, broadCastID)
{

}

CYuanZhengPoint::~CYuanZhengPoint()
{

}

void CYuanZhengPoint::LoadData(const ::char_data_define::PBRecoverTimesData& data)
{
	m_nNextRevertTime = data.nextreverttime();
	m_nEndRevertTime = data.endreverttime();
}

void CYuanZhengPoint::SaveData(::char_data_define::PBRecoverTimesData* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	pkData->set_nextreverttime(m_nNextRevertTime);
	pkData->set_endreverttime(m_nEndRevertTime);
}

int32_t CYuanZhengPoint::GetCurrentTimes(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return 0;
	}
	return pkOwnerPlayer->GetCashType28().GetMoney();
}

bool CYuanZhengPoint::DoAddTimes(LC_ServerPlayer* pkOwnerPlayer, int times)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	bool tag = pkOwnerPlayer->AddCash(times, CT_CASH_TYPE_28, 0, 0);
	if(tag)
	{
		SyncYuanZhengPointInfo(pkOwnerPlayer);
	}

	return tag;
}

bool CYuanZhengPoint::DoReduceTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	LC_Money* pkMoney = NULL;
	pkOwnerPlayer->GetCashByCashType(CT_CASH_TYPE_28, &pkMoney);
	if(NULL == pkMoney)
	{
		return false;
	}
	
	if(pkMoney->GetMoney() < times)
	{
		return false;
	}

	pkMoney->SetMoney(pkMoney->GetMoney() - times);
	SyncYuanZhengPointInfo(pkOwnerPlayer);	
	return true;
}

bool CYuanZhengPoint::SyncYuanZhengPointInfo(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	MG_SyncYuanZhengRevertInfo msg;
	msg.m_nNextRevertTime = m_nNextRevertTime;
	msg.m_nEndRevertTime = m_nEndRevertTime;
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCYUANZHENGREVERTINFO, &msg);

	return true;
}

