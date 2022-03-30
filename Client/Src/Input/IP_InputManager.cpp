#include "ClientPCH.h"
#include "MG_MessageBase.h"
#include "IP_InputManager.h"
#include "UI_Cursor.h"
#include "RG_RenderManager.h"

using namespace Utility;
//-------------------------------------------------------
IP_InputManager::IP_InputManager()
: m_spNiInputSystem(NULL)
{
}
//-------------------------------------------------------
IP_InputManager::~IP_InputManager()
{
	T_ASSERT(!m_spNiInputSystem && "UnInit InputManager First");
}
//-------------------------------------------------------
bool IP_InputManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK,_event_TimerTick)
	MESSAGE_HANDLE(MGT_WIN32_MESSAGE,_event_Win32Msg)
	END_MESSAGE_HANDLE
	return true;
}
//-------------------------------------------------------
bool IP_InputManager::Init(HWND hWnd,HINSTANCE hI)
{
	T_ASSERT(!m_spNiInputSystem && "Duplicate InputManager Init");

	m_kbData.reset();

	NiRendererPtr pkNiRenderer = NiRenderer::GetRenderer();
	if(!pkNiRenderer)
		return false;

	NiDI8InputSystem::DI8CreateParams* pkParams = NiNew NiDI8InputSystem::DI8CreateParams();
	m_assert(pkParams);

	pkParams->SetKeyboardUsage(NiInputSystem::FOREGROUND | NiInputSystem::NONEXCLUSIVE);
	pkParams->SetRenderer(pkNiRenderer);
	pkParams->SetMouseUsage(NiInputSystem::FOREGROUND | NiInputSystem::NONEXCLUSIVE);
	pkParams->SetGamePadCount(0);
	pkParams->SetAxisRange(-100, +100);
	pkParams->SetOwnerWindow(hWnd);
	pkParams->SetOwnerInstance(hI);

	//< RGClient.dll在ie11下可能因为权限不足，导致NiInputSystem::Create返回NULL
	m_spNiInputSystem = NiInputSystem::Create(pkParams);
	if(m_spNiInputSystem)
	{
		m_spNiInputSystem->OpenMouse();
		m_spNiInputSystem->OpenKeyboard();

	}
	NiDelete pkParams;
	return true;
}
//-------------------------------------------------------
bool IP_InputManager::UnInit(void)
{
	m_spNiInputSystem = NULL;
	return true;
}
//------------------------------------------------------------------------
bool IP_InputManager::KeyWasPressed(NiInputKeyboard::KeyCode eKey)
{
#ifndef	T6_CLIENT
	NiInputKeyboardPtr spInputKeyboard = m_spNiInputSystem->GetKeyboard();

	return spInputKeyboard->KeyIsDown(eKey);
#else
	return (m_kbData.getKeyState(eKey) & KEY_BIT_STATE::TRG) != 0;
#endif
}
bool IP_InputManager::KeyWasReleased(NiInputKeyboard::KeyCode eKey)
{
#ifndef	T6_CLIENT
	NiInputKeyboardPtr spInputKeyboard = m_spNiInputSystem->GetKeyboard();

	return spInputKeyboard->KeyWasReleased(eKey);
#else
	return (m_kbData.getKeyState(eKey) & KEY_BIT_STATE::RLS) != 0;
#endif
}
bool IP_InputManager::KeyIsDown(NiInputKeyboard::KeyCode eKey)
{
#ifndef	T6_CLIENT
	NiInputKeyboardPtr spInputKeyboard = m_spNiInputSystem->GetKeyboard();

	return spInputKeyboard->KeyIsDown(eKey);
#else
	return (m_kbData.getKeyState(eKey) & KEY_BIT_STATE::PAD) != 0;
#endif
}
void IP_InputManager::injectKeyDown(NiInputKeyboard::KeyCode eKey)
{
	m_kbData.injectKeyDown(eKey);
}
void IP_InputManager::injectKeyUp(NiInputKeyboard::KeyCode eKey)
{
	m_kbData.injectKeyUp(eKey);
}

void IP_InputManager::resetKeyState()
{
	m_kbData.reset();
}
//-------------------------------------------------------
void IP_InputManager::_event_TimerTick( MG_MessageBase& rkMessage )
{
	T_PROFILE("Update::IP_InputManager");
	if (m_spNiInputSystem != NULL)
		m_spNiInputSystem->UpdateAllDevices();
	
//	NiInputMousePtr spInputMouse = m_spNiInputSystem->GetMouse();
//	T_ASSERT(spInputMouse && "NULL Input Mouse");
	
	NiInputKeyboardPtr spInputKeyboard = (m_spNiInputSystem != NULL) ? m_spNiInputSystem->GetKeyboard() : NULL;
	T_ASSERT(spInputKeyboard && "NULL Input Keyboard");

	m_kbData.update();
	m_msData.update();

	//辅助键状态
	long	wParam = 0;
	if (this->MouseIsDown(0))		wParam |= MK_LBUTTON;
	if (this->MouseIsDown(1))		wParam |= MK_RBUTTON;
	if (this->MouseIsDown(2))		wParam |= MK_MBUTTON;

	if (spInputKeyboard != NULL)
	{
	if (spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_LCONTROL))	wParam |= MK_CONTROL;
	if (spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_LMENU))		wParam |= MK_ALT;
	if (spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_LSHIFT))	wParam |= MK_SHIFT;

	//按键触发
	if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_1))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_1));
	else if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_2))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_2));
	else if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_3))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_3));
	else if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_9))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_9));
	else if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ADD))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_ADD));
    else if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_MULTIPLY))
        SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_NUMPAD_MUL));

	if(spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_UP))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_UP));
	else if(spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_DOWN))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_DOWN));

	if(spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_LEFT))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_LEFT));
	else if(spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_RIGHT))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_RIGHT));

	if (spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_COMMA) )
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_COMMA));
	else if (spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_PERIOD) )
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_PERIOD));

	if ( spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_PRIOR))
	   SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_PAGE_PRIOIR));
	else if ( spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_NEXT) )
	   SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_PAGE_NEXT));

	if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_F9) || spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_F9))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_F9));
	if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_F11) || spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_F11))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_F11));

	//alt + v
	if(spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_LMENU) && spInputKeyboard->KeyIsDown(NiInputKeyboard::KEY_V))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_ALT_V));
	else if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_LMENU) || spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_RMENU))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_ALT));
	if(spInputKeyboard->KeyWasPressed(NiInputKeyboard::KEY_LCONTROL))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_LCTRL_PRESSED));
	if (spInputKeyboard->KeyWasReleased(NiInputKeyboard::KEY_LCONTROL))
		SendMessage(CreateMsg_KEY_MSG(MGT_INPUT_KEY_LCTRL_RELEASE));
	}

	//MouseMove
	SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_MOVE,m_msData.getDx(),m_msData.getDy(),wParam));
}
//-------------------------------------------------------
void IP_InputManager::_event_Win32Msg( MG_MessageBase& rkMessage )
{
	UINT msg = rkMessage.m_uiParam1;
	WPARAM wParam = rkMessage.m_uiParam2;
	LPARAM lParam = rkMessage.m_uiParam3;
	float fCurrentTime = GET_PROCESS_TIME;
	
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);
			m_msData.injectLButtonDown();
	
			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_LBUTTON_DOWN,CurrentX,CurrentY,(long)wParam),true);
		}
		break;
	case WM_LBUTTONUP:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);
			m_msData.injectLButtonUp();
	
			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_LBUTTON_UP,CurrentX,CurrentY,(long)wParam),true);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);
			m_msData.injectLButtonDown();

			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_LBUTTON_DBCLK,CurrentX,CurrentY,(long)wParam),true);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);
			m_msData.injectRButtonDown();

			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_RBUTTON_DOWN,CurrentX,CurrentY,(long)wParam),true);
		}
		break;
	case WM_RBUTTONUP:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);
			m_msData.injectRButtonUp();

			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_RBUTTON_UP,CurrentX,CurrentY,(long)wParam),true);
		}
		break;
	case WM_MBUTTONDOWN:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);
			m_msData.injectMButtonDown();

			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_MBUTTON_DOWN,CurrentX,CurrentY,long(wParam)),true);
		}
		break;
	case WM_MBUTTONUP:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);
			m_msData.injectMButtonUp();

			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_MBUTTON_UP, CurrentX,CurrentY,long(wParam)),true);
		};
		break;
	case WM_MOUSEMOVE:
		{
			unsigned short CurrentX = LOWORD(lParam);
			unsigned short CurrentY = HIWORD(lParam);
			m_msData.injectMove(CurrentX, CurrentY, 0);

			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_MOVE,CurrentX,CurrentY,long(wParam)));
		}
		break;
	case WM_MOUSEWHEEL:
		{
			unsigned short CurrentX = m_msData.getDx();
			unsigned short CurrentY = m_msData.getDy();

			SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_WHEEL,CurrentX,CurrentY,(long)wParam));
		}
		break;
	
	case WM_SETCURSOR:
		{
			GUISys::UI_Cursor* pkCursor = GUISys::UI_Cursor::GetCursor();
			if(pkCursor)
			{
				pkCursor->UpdateScreenGeometry();
			}
		}
		break;

    case WM_HOTKEY:
        {
            unsigned short usSrcKey = HIWORD(lParam);
            unsigned short usCombKey = LOWORD(lParam);

            if (usSrcKey == VK_SNAPSHOT)
            {
                if (RG_RenderManager::GetSingletonPtr() != NULL)
                {
                    RG_RenderManager::GetSingleton().Snapshot(usCombKey != MOD_ALT, false);
                }
            }
        }
		break;

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			::ClipCursor(NULL);
			this->m_kbData.reset();
		}
		break;

	default:
		break;
	}
}

//-------------------------------------------------------
LPARAM IP_InputManager::GetMousePosition(void)
{
	int x = m_msData.getDx();
	int y = m_msData.getDy();
	return MAKELONG(x, y);
}

bool IP_InputManager::MouseWasPressed(int idx)
{
	unsigned int v = m_msData.getKeyState(idx);
	return (v & KEY_BIT_STATE::TRG) != 0;
}

bool IP_InputManager::MouseWasReleased(int idx)
{
	unsigned int v = m_msData.getKeyState(idx);
	return (v & KEY_BIT_STATE::RLS) != 0;
}

bool IP_InputManager::MouseIsDown(int idx)
{
	unsigned int v = m_msData.getKeyState(idx);
	return (v & KEY_BIT_STATE::PAD) != 0;
}

//-------------------------------------------------------
unsigned int	VirtualKeyToDIK( WPARAM wp, LPARAM lp )
{
	unsigned int sk = (((unsigned int)lp >> 16) & 0x00ff);
	unsigned int exk = ((unsigned int)lp >> 24) & 1;

	if (wp == VK_PAUSE || wp == VK_NUMLOCK)
	{
		exk = !exk;
	}

	return sk + (exk << 7);
}