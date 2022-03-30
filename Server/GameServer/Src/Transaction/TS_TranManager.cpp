#include "TS_TranManager.h"
using namespace Utility;

//////////////////////////////////////////////////////////////////////////////////////
TS_TranBase* TS_TranManager::RegisteTran(uint32_t uiTranID, uint32_t uiTranType, uint32_t uiUserID, const unique_id_impl& uiCharID)
{
	if (m_kSequnceTranMap.find(uiTranID) != m_kSequnceTranMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to register the transaction. The transaction already exists, Type: %d, Sequnce: %d", uiTranType, uiTranID);
		return NULL;
	}

	TS_TranBase* pkTran = m_kTranPool.AllocTran(uiTranType);
	if (!pkTran)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to create transaction, Type: %d, Sequnce: %d", uiTranType, uiTranID);
		return NULL;
	}

	pkTran->SetID(uiTranID);
	m_kSequnceTranMap[uiTranID] = pkTran;
	pkTran->SetOwnerID(uiUserID, uiCharID);
	return pkTran;
}

bool TS_TranManager::UnRegisteTran(uint32_t uiTranID)
{
	bool bResult = false;
	SequnceTranMapIter it = m_kSequnceTranMap.find(uiTranID);

	if (it == m_kSequnceTranMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"Unregister transaction failed. The transaction does not exist£¬Transaction Sequnce: %d", uiTranID);
	}
	else
	{
		m_kTranPool.FreeTran(it->second);
		m_kSequnceTranMap.erase(it);
		bResult = true;
	}
	return bResult;
}

TS_TranBase* TS_TranManager::GetTran(uint32_t uiTranID)
{
	TS_TranBase* pkTran = NULL;
	SequnceTranMapIter it = m_kSequnceTranMap.find(uiTranID);
	if (it != m_kSequnceTranMap.end())
	{
		pkTran = it->second;
	}
	return pkTran;
}

void TS_TranManager::Update()
{
	for (SequnceTranMapIter it = m_kSequnceTranMap.begin(); it != m_kSequnceTranMap.end(); ++it)
	{
		TS_TranBase* pkTran = it->second;
		if (pkTran)
		{
			if (TRAN_STATUS_TIMEOUT == pkTran->Update())
			{
				UnRegisteTran(pkTran->GetID());
				break;
			}
		}
	}
}
