//�ͻ���ϵͳ��ʱ��
#ifndef _TM_CLIENT_TIME_MANAGER_H
#define _TM_CLIENT_TIME_MANAGER_H

#include "MG_MessageHandlerInterface.h"

class TM_ClientTimeManager : public Utility::UT_TimeManager,
							 public MG_MessageHandlerInterface
{
public:
	TM_ClientTimeManager();
	virtual ~TM_ClientTimeManager();

	virtual bool	RecvMessage(MG_MessageBase& rkMessage);
	virtual void	Tick();
			void	SetFrameRate(float f);
	bool			EnableUpdate(){ return m_bUpdate; }
	void			SyncServerTime(uint32_t uiServerTime);
	void			SyncServerOpenTime(uint32_t uiOpenTime) {m_uiServerOpenTime = uiOpenTime;}
	void			SyncServerConflateTime(uint32_t uiConflateTime) {m_uiServerConflateTime = uiConflateTime;}
	virtual int64_t	GetServerTime(){return m_uiServerTime;}
	virtual int64_t GetServerOpenTime(){return m_uiServerOpenTime;}
	virtual int64_t GetServerConflateTime(){return m_uiServerConflateTime;}

protected:
	void _event_InputKeyF9(MG_MessageBase& rkMessage);
	void _event_InputKeyW(MG_MessageBase& rkMessage);
	void _event_LostFocus(MG_MessageBase& rkMessage);

	enum { MAX_BUFFERED_FRAMES = 15 };

	bool			m_bSmoothTime;			//< ƽ��֡��
	bool			m_bWindowsActive;		//< ���㴰��
	bool			m_bUpdate;				//< ������ѭ��
	unsigned int	m_uiCurrentFrame;
	float			m_pfFrameTime[MAX_BUFFERED_FRAMES];
	float			m_fLogicFrameRate;		//< �߼�֡��
	float			m_fUpdateFreq;			//< ���¼�� 1.0 / m_fLogicFrameRate
	float			m_fLostFocusUpdateFreq; //< �ǽ�����¼��
	int64_t			m_uiServerTime;
	int64_t			m_uiServerOpenTime;
	int64_t			m_uiServerConflateTime;
};


#endif