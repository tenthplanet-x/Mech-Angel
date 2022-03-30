#include "ClientPCH.h"
#include "MG_MessageBase.h"

//-----------------------------------------------------
MG_MessageBase::MG_MessageBase()
: m_eMsgType(MGT_INVALID)
, m_bMsgHandled(false)
, m_lParam1(0)
, m_lParam2(0)
, m_lParam3(0)
, m_lParam4(0)
, m_lParam5(0)
, m_lParam6(0)
, m_llParam(0)
{
}
//-----------------------------------------------------
MG_MessageBase::~MG_MessageBase()
{
}
//-----------------------------------------------------
MG_MessageBase::MG_MessageBase(const MG_MessageBase& rkMessage)
{
	m_eMsgType = rkMessage.m_eMsgType;
	m_bMsgHandled = rkMessage.m_bMsgHandled;
	m_lParam1 = rkMessage.m_lParam1;
	m_lParam2 = rkMessage.m_lParam2;
	m_lParam3 = rkMessage.m_lParam3;
	m_lParam4 = rkMessage.m_lParam4;
	m_lParam5 = rkMessage.m_lParam5;
	m_lParam6 = rkMessage.m_lParam6;
	m_llParam = rkMessage.m_llParam;
}
//-----------------------------------------------------
void MG_MessageBase::Reset()
{
	m_eMsgType = MGT_INVALID;
	m_lParam1 = 0;
	m_lParam2 = 0;
	m_lParam3 = 0;
	m_lParam4 = 0;
	m_lParam5 = 0;
	m_lParam6 = 0;
	m_llParam = 0;
}
