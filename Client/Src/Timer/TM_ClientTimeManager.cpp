#include "ClientPCH.h"

#include "MG_MessageBase.h"
#include "TM_ClientTimeManager.h"
#include "UT_ConfigFile.h"

using namespace Utility;
//---------------------------------------------------
TM_ClientTimeManager::TM_ClientTimeManager()
: UT_TimeManager()
{
	m_uiCurrentSystemTime = 0;
	m_uiServerTime = 0;
	m_uiServerOpenTime = 0;
	m_uiServerConflateTime = 0;

	::memset(m_pfFrameTime, 0, sizeof(m_pfFrameTime));
	m_uiCurrentFrame = 0;

	m_bSmoothTime = g_settings.LOD_TIME_SMOOTH;
	m_bWindowsActive = true;
	m_bUpdate = false;

#ifdef CLIENT_DLL
	/// �����ѭ���ĵ���Ƶ�����ⲿ������ƣ��ҹ̶�Ϊ����60֡����TimerMgr��֡�ʱ��������
	/// ������Tick()��ʱ��m_bUpdate���ж�Ϊfalse��������һ֡�������¡�һֱ֡��return���������ʱ��ͻ���֡��ֻ���ⲿ����Ƶ�ʵ�һ������
	this->SetFrameRate(120.0f);
#else
	this->SetFrameRate(g_settings.PROFILE_MAX_FPS);
#endif

	// ����timeGetTime()��ʱ�����ܾ�ȷ
	::timeBeginPeriod(1);
}
//---------------------------------------------------
TM_ClientTimeManager::~TM_ClientTimeManager()
{
	::timeEndPeriod(1);
}
//---------------------------------------------------
void TM_ClientTimeManager::SetFrameRate( float f )
{
	m_fLogicFrameRate		= f;
	m_fUpdateFreq			= 1.0f / m_fLogicFrameRate;
	m_fLostFocusUpdateFreq	= 2.0f / m_fLogicFrameRate ;
}
//---------------------------------------------------
void TM_ClientTimeManager::SyncServerTime(uint32_t uiServerTime)
{
	m_uiServerTime = uiServerTime;
	m_uiCurrentSystemTime = time(0);
}
//---------------------------------------------------
bool TM_ClientTimeManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	BEGIN_MESSAGE_HANDLE
	//MESSAGE_HANDLE(MGT_INPUT_KEY_F9,_event_InputKeyF9)
	//MESSAGE_HANDLE(MGT_INPUT_KEY_W,_event_InputKeyW)
	MESSAGE_HANDLE(MGT_WINDOWS_LOST_FOCUS,_event_LostFocus)
	END_MESSAGE_HANDLE
	return true;
}
//---------------------------------------------------
void TM_ClientTimeManager::Tick()
{
	float fLastTickTime = m_fLastTickTime;
	float fDeltaTime = 0.0f;

	uint64_t uiLastSystemTime = m_uiCurrentSystemTime;
	UT_TimeManager::Tick();

	m_uiServerTime += m_uiCurrentSystemTime - uiLastSystemTime;

	if ( m_bSmoothTime )
	{
		m_pfFrameTime[ m_uiCurrentFrame ] = m_fDeltaTime;
		m_uiCurrentFrame = ( m_uiCurrentFrame + 1 ) % MAX_BUFFERED_FRAMES;
		int nTotalBuffer = 0;
		float fTotalTime = 0.0f;
		for ( int i = 0 ; i < MAX_BUFFERED_FRAMES ; i++ )
		{
			if ( m_pfFrameTime[i] != 0.0f )
			{
				nTotalBuffer++;
				fTotalTime += m_pfFrameTime[i];
			}
		}
		fDeltaTime = fTotalTime / nTotalBuffer;
	}
    else
    {
        fDeltaTime = m_fDeltaTime;
    }
	float fUpdateFreq = m_fUpdateFreq;
	//��ʱ�رշǽ�����£��۲�ͻ��˿��������Ƿ���ʧ
	//if ( g_settings.LOD_LOST_FOCUS_UPDATE)
	//{
	//	fUpdateFreq = m_bWindowsActive ? m_fUpdateFreq : m_fLostFocusUpdateFreq;
	//}
    if (fDeltaTime < fUpdateFreq)
	{
		m_fLastTickTime = fLastTickTime;
		m_bUpdate = false;
	}
	else
	{
		this->IncFrameNo();
		SendMessage(CreateMsg_TIMER_TICK(m_fCurrentTime, fDeltaTime));
		m_bUpdate = true;
	}
}
//---------------------------------------------------
void TM_ClientTimeManager::_event_InputKeyF9( MG_MessageBase& rkMessage )
{
	if(m_eTimerState == TS_START)
		PauseTimer();
	else
		PlayTimer();
}
//---------------------------------------------------
void TM_ClientTimeManager::_event_InputKeyW(MG_MessageBase& rkMessage)
{
	if(m_eTimerState == TS_PAUSE)
	{
		m_fCurrentTime += 0.01f;
		m_fDeltaTime = 0.01f;
		SendMessage(CreateMsg_TIMER_TICK(m_fCurrentTime, m_fDeltaTime));
	}
}
//---------------------------------------------------
void TM_ClientTimeManager::_event_LostFocus( MG_MessageBase& rkMessage )
{
	m_bWindowsActive = bool(rkMessage.m_lParam1 > 0);
}
