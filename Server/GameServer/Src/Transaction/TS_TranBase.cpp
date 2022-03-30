#include "TS_TranBase.h"
#include "SystemDefine.h"
#include "UT_Log.h"
#include "UT_TimeManager.h"
#include "UT_Timer.h"

using namespace Utility;

TS_AtomOperate::TS_AtomOperate()
{
	Reset();
}

void TS_AtomOperate::Reset()
{
	SetResultID(RE_FAIL);
	SetUserID(INVALID_USER_ID);
	SetCharID(INVALID_ACTOR_ID);
	SetContentFlag(false);
}

//////////////////////////////////////////////////////////////////////////

TS_TranBase::TS_TranBase()
: m_uiID(0)
, m_uiStep(0)
, m_pPostCommit(NULL)
{}


TS_TranBase::~TS_TranBase() 
{
	if (m_pPostCommit)
	{
		T_DELETE_D(m_pPostCommit);
		m_pPostCommit = NULL;
	}
}

ResultType TS_TranBase::HandlerMsg(Protocol::SS_MG_HEAD& rkHead, Protocol::MG_BODY* pkMsg)
{
	StepAtomMapIter it = m_kStepTranMap.find(m_uiStep);

	if ( it == m_kStepTranMap.end() )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "There is a logic problem with the transaction! The step could not be found %d", m_uiStep);
		return RE_FAIL;
	}

	TS_AtomOperate* pkAtom = it->second;
	if (NULL == pkAtom)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " There is a logic problem with the transaction! The step could not be found %d", m_uiStep);
		return RE_FAIL;
	}

	{
		pkAtom->SetUserID(rkHead.m_uiUserID);
		pkAtom->SetCharID(rkHead.m_uiCharID);
		emTranOperate eResult = pkAtom->Execute(pkMsg, GetID());

		if( eResult == TRAN_OP_NEXT )
		{
			++m_uiStep;
			m_fTranHandleTime = GET_PROCESS_TIME;
			if (m_uiStep>=m_kStepTranMap.size())
			{
				if (m_pPostCommit)
				{
					(*m_pPostCommit)();
				}
			}
			return RE_SUCCESS;
		}
		else
		{
			pkAtom->RespondRltMsg();
			_rollbackAllStep();
		}
	}

	return RE_FAIL;
}

enTranStatus TS_TranBase::Update()
{
	// 60s ºó³¬Ê±
	if ( GET_PROCESS_TIME - m_fTranHandleTime > 60.0f )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Update Transaction Timeout, cur step:%u", m_uiStep);

		StepAtomMapIter it = m_kStepTranMap.find(m_uiStep);
		if ( it == m_kStepTranMap.end() )
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Update Transaction Logic Error! illgal step:%d", m_uiStep);
			return TRAN_STATUS_TIMEOUT;
		}

		TS_AtomOperate* pkAtom = it->second;
		if (NULL == pkAtom)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Update Transaction Logic Error, illgal step %d", m_uiStep);
			return TRAN_STATUS_TIMEOUT;
		}

//		if (pkAtom)
		{
			pkAtom->RespondRltMsg();
			_rollbackAllStep();

			return TRAN_STATUS_TIMEOUT;
		}
	}

	return TRAN_STATUS_RUNNING;
}

void TS_TranBase::SetOwnerID(uint32_t uiUserID, const unique_id_impl& uiCharID)
{
	StepAtomMapIter IterBe = m_kStepTranMap.begin();
	StepAtomMapIter IterEd = m_kStepTranMap.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		TS_AtomOperate* pkAtom = IterBe->second;
		if(pkAtom)
		{
			pkAtom->SetUserID(uiUserID);
			pkAtom->SetCharID(uiCharID);
		}
	}
}



void TS_TranBase::_rollbackAllStep()
{
	do 
	{
		TS_AtomOperate* pkOpera = m_kStepTranMap[m_uiStep];
		if(NULL == pkOpera)
		{
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR, "TS_TranBase Rollback, err step");
			return;
		}

		pkOpera->Rollback();

		if (m_uiStep == 0)
		{
			break;
		}
		else
		{
			--m_uiStep;
		}
	} while (m_uiStep >= 0);
}

void TS_TranBase::Reset()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		if (it->second)
		{
			it->second->Reset();
		}
	}

	m_uiStep = 0;

	if (m_pPostCommit)
	{
		T_DELETE_D(m_pPostCommit);
		m_pPostCommit = NULL;
	}

	m_fTranHandleTime = GET_PROCESS_TIME;
}
