#include "LM_LocaleManager.h"
#include "GameLogicScriptPCH.h"
#include "LC_CombatRecorderManager.h"
#include "LC_Define.h"

LC_CombatRecorderManager::LC_CombatRecorderManager()
	: m_nCurrentWriteIndex(0)
	, m_nCurrentReadIndex(0)
	, m_nOwnerId(0)
	, m_nStatus(false)
	, m_nRank(-1)
	, m_nDataKey(-1)
	, m_nMapType(0)
	, m_nYiZhongQinXiDamage(0)
{

}

LC_CombatRecorderManager::~LC_CombatRecorderManager()
{

}

void LC_CombatRecorderManager::Init( object_id_type nObjId )
{
	m_nOwnerId = nObjId;
	for ( int i = 0; i < MAX_COMBAT_RECORDER_COUNT; i++ )
	{
		m_RecorderData[i].Init(nObjId, i);
	}
}

void LC_CombatRecorderManager::ReUse()
{
	m_nOwnerId = INVALID_ACTOR_ID;
	for ( int i = 0; i < MAX_COMBAT_RECORDER_COUNT; i++ )
	{
		m_RecorderData[i].ReUse();
	}
	m_nCurrentWriteIndex = 0;
	m_nCurrentReadIndex = 0;
	m_nStatus = false;
	m_nRank = -1;
	m_nMapType = 0;
	m_nYiZhongQinXiDamage = 0;
}

void LC_CombatRecorderManager::SetYiZhongQinXiDamage(uint64_t date)
{
	m_nYiZhongQinXiDamage = date;
}

object_id_type LC_CombatRecorderManager::GetOwnerId()
{
	return m_nOwnerId;
}

const LC_CombatRecorder& LC_CombatRecorderManager::GetReadData()
{
	return m_RecorderData[m_nCurrentReadIndex];
}

const LC_CombatRecorder& LC_CombatRecorderManager::GetWriteData()
{
	return m_RecorderData[m_nCurrentWriteIndex];
}

const LC_CombatRecorder& LC_CombatRecorderManager::GetMaxData()
{
	int nIndex = 0;
	if (m_RecorderData[0].GetValue() > m_RecorderData[1].GetValue() )
		nIndex = 0;
	else
		nIndex = 1;
	return m_RecorderData[nIndex];
}

void LC_CombatRecorderManager::ResetDataIndex()
{
	
}

bool LC_CombatRecorderManager::AddValue( uint64_t nInput , bool checkState)
{
	if(checkState && m_nStatus==false)
	{
		return false;
	}

	uint64_t value = m_RecorderData[m_nCurrentWriteIndex].GetValue();
	m_RecorderData[m_nCurrentWriteIndex].AddValue(nInput);

	return true;
}

void LC_CombatRecorderManager::Enable(int32_t nMapType)
{
	m_nStatus = true;
	m_nMapType = nMapType;
	m_RecorderData[0].Enable();
	m_RecorderData[1].Enable();
}

void LC_CombatRecorderManager::Disable()
{
	m_nStatus = false;
	//FinishRecord();
}

bool LC_CombatRecorderManager::CheckEnable()
{
	return m_nStatus;
}

void LC_CombatRecorderManager::FinishRecord()
{
	if(m_nMapType == MT_REANSCRIPTION_RANK)
	{
		SetYiZhongQinXiDamage(GetMaxData().GetValue());
	}

	if ( m_nStatus == false )
		return;
	if (m_RecorderData[0].GetValue() > m_RecorderData[1].GetValue() )
	{
		m_nCurrentReadIndex = 0;
		m_nCurrentWriteIndex = 1;
	}
	else
	{
		m_nCurrentWriteIndex = 0;
		m_nCurrentReadIndex = 1;
	}
	m_RecorderData[m_nCurrentWriteIndex].Restart();
}

int32_t LC_CombatRecorderManager::GetRank()
{
	return m_nRank;
}

void LC_CombatRecorderManager::SetRank( int32_t nRank )
{
	m_nRank = nRank;
}

void LC_CombatRecorderManager::SetDataKey( int32_t nKey )
{
	m_nDataKey = nKey;
}

int32_t LC_CombatRecorderManager::GetDataKey()
{
	return m_nDataKey;
}

int32_t LC_CombatRecorderManager::GetMapType()
{
	return m_nMapType;
}

uint64_t LC_CombatRecorderManager::GetYiZhongQinXiDamage()
{
	return m_nYiZhongQinXiDamage;
}

void LC_CombatRecorderManager::Save(char_data_define::combat_recorder & data)
{
	data.set_data_key(GetDataKey());
	data.set_value(GetMaxData().GetValue());
	data.set_yizhongqinxidamage(GetYiZhongQinXiDamage());
}

void LC_CombatRecorderManager::Load(const char_data_define::combat_recorder & data)
{
	SetDataKey(data.data_key());
	AddValue(data.value(), false);
	SetYiZhongQinXiDamage(data.yizhongqinxidamage());
}


