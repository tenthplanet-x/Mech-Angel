#include "ClientPCH.h"

#include "LC_ClientCombatRecordManager.h"
#include "LC_ClientLogicManager.h"


using namespace GameLogic;


LC_ClientCombatRecordManager::LC_ClientCombatRecordManager()
{

}

LC_ClientCombatRecordManager::~LC_ClientCombatRecordManager()
{

}

void LC_ClientCombatRecordManager::Init()
{
	Reset();
}

void LC_ClientCombatRecordManager::Unit()
{
	Reset();
}

void LC_ClientCombatRecordManager::Reset()
{
	m_kPlayerCombatDataMap.clear();
	m_kMonsterCombatDataMap.clear();
}

bool LC_ClientCombatRecordManager::RecvMessage(MG_MessageBase& rkMessage)
{
	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_COMBATRECORD_DAMAGE, _event_ReceiveDamage)
	MESSAGE_HANDLE(MGT_COMBATRECORD_HEAL, _event_DoHeal)
	END_MESSAGE_HANDLE

	return true;
}
void LC_ClientCombatRecordManager::_event_ReceiveDamage(const MG_MessageBase& rkMessage)
{
	const object_id_type lSrcID = rkMessage.m_lParam1;
	const object_id_type lDstID = rkMessage.m_lParam2;
	const int64_t lFinalDamage = rkMessage.m_llParam;

	CombatRecord* pkRecord = NULL;

	if(IS_NPC_ID(lSrcID))
	{
		pkRecord = &m_kMonsterCombatDataMap[lSrcID];
	}
	else if(IS_PLAYER_CLASS_ID(lSrcID))
	{
		pkRecord = &m_kPlayerCombatDataMap[lSrcID];
	}

	if(pkRecord != NULL)
	{
		pkRecord->iDamage += lFinalDamage;
		_doInitCharType(pkRecord, lSrcID);
	}

	pkRecord = NULL;

	if(IS_NPC_ID(lDstID))
	{
		pkRecord = &m_kMonsterCombatDataMap[lDstID];
	}
	else if(IS_PLAYER_CLASS_ID(lDstID))
	{
		pkRecord = &m_kPlayerCombatDataMap[lDstID];
	}

	if(pkRecord != NULL)
	{
		pkRecord->iHurt += lFinalDamage;
		_doInitCharType(pkRecord, lDstID);
	}
}

void LC_ClientCombatRecordManager::_event_DoHeal(const MG_MessageBase& rkMessage)
{
	const object_id_type lSrcID = rkMessage.m_lParam1;
	const int64_t lFinalHeal = rkMessage.m_llParam;

	CombatRecord* pkRecord = NULL;

	if(IS_NPC_ID(lSrcID))
	{
		pkRecord = &m_kMonsterCombatDataMap[lSrcID];
	}
	else if(IS_PLAYER_CLASS_ID(lSrcID))
	{
		pkRecord = &m_kPlayerCombatDataMap[lSrcID];
	}

	if(pkRecord != NULL)
	{
		pkRecord->iHeal += lFinalHeal;
		_doInitCharType(pkRecord, lSrcID);
	}
}

StringType LC_ClientCombatRecordManager::GetPlayerCombatInfo()
{
	return _getCombatData(m_kPlayerCombatDataMap, true);
}

StringType LC_ClientCombatRecordManager::GetMonsterCombatInfo()
{
	return _getCombatData(m_kMonsterCombatDataMap, false);
}

int LC_ClientCombatRecordManager::GetSelfMvpActiveIdx(LC_ClientGamePlayer* pkPlayer)
{
	int iIdx = -1;
	int64_t iDamage = 0;
	CombatRecordMapConstIter iter = m_kPlayerCombatDataMap.begin();
	for( ; iter!=m_kPlayerCombatDataMap.end(); ++iter)
	{
		const CombatRecord& record = iter->second;
		const int iActiveIdx = pkPlayer->GetBuddyIndex(iter->first);
		if(iActiveIdx >= 0 && record.iDamage > iDamage)
		{
			iDamage = record.iDamage;
			iIdx = iActiveIdx;
		}
	}

	return iIdx;
}

StringType LC_ClientCombatRecordManager::_getCombatData(const CombatRecordMap& recordMap, bool bPlayer)
{
	StringStreamType sStream;
	CombatRecordMapConstIter iter = recordMap.begin();
	for( ; iter!=recordMap.end(); ++iter)
	{
		const CombatRecord& record = iter->second;
		sStream << iter->first << '#' << record.iCharType << '#';
		sStream  << record.iDamage << '#' << record.iHeal << '#' << record.iHurt << '#';

		const CF_CharType::DataEntry *pkData = NULL;
		if(!bPlayer)
		{
			pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(record.iCharType);
		}

		if(pkData != NULL)
		{
			sStream << pkData->_lLevel << '#' << pkData->_kCharName << '#';
			sStream << Int2String(pkData->_elementdamagetype) << '#' << pkData->_kTalkHeadIcon;
		}
		else
		{
			sStream << record.iLevel << "#0#0#";
		}
		sStream << '|';
	}

	return sStream.str();
}

void LC_ClientCombatRecordManager::_doInitCharType(CombatRecord* pkRecord, object_id_type objID)
{
	if(pkRecord->iCharType == 0)
	{
		LC_ActorBase* pkDstActor = (LC_ActorBase*)(LC_ClientLogicManager::GetSingletonPtr()->GetWorldManager()->FindObject(objID));
		if(pkDstActor != NULL)
		{
			pkRecord->iCharType = pkDstActor->GetCharType();
			pkRecord->iLevel = pkDstActor->GetLevel();
		}
	}
}