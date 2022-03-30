#ifndef _IP_INPUT_MANAGER_H
#define _IP_INPUT_MANAGER_H

#include "MG_MessageHandlerInterface.h"
#include "InputDef.h"

class IP_InputManager : public Utility::UT_Singleton<IP_InputManager>,
						public MG_MessageHandlerInterface,
						public Memory::MM_BaseObject
						
{
public:

	IP_InputManager();
	~IP_InputManager();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	bool Init(HWND hWnd,HINSTANCE hI);
	bool UnInit(void);
	bool KeyWasPressed(NiInputKeyboard::KeyCode eKey);
	bool KeyWasReleased(NiInputKeyboard::KeyCode eKey);
	bool KeyIsDown(NiInputKeyboard::KeyCode eKey);
	void injectKeyDown(NiInputKeyboard::KeyCode eKey);
	void injectKeyUp(NiInputKeyboard::KeyCode eKey);
	void resetKeyState();

	LPARAM GetMousePosition(void);
	bool	MouseWasPressed(int idx);
	bool	MouseWasReleased(int idx);
	bool	MouseIsDown(int idx);

protected:
	void  _event_Win32Msg( MG_MessageBase& rkMessage );
	void  _event_TimerTick(MG_MessageBase& rkMessage);
	
	NiInputSystemPtr m_spNiInputSystem;

	CKeyboardInputData	m_kbData;		//键盘按键状态，需要经过UI过滤处理
	CMouseInputData		m_msData;		//鼠标按键状态
};

//NiInputKeyboard::KeyCode equal to DIK_CODE
unsigned int	VirtualKeyToDIK( WPARAM wp, LPARAM lp );

#endif