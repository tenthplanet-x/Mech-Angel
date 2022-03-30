#include "ClientPCH.h"
#include "UT_Vec.h"
#include "IP_ShortcutKeyCode.h"
#include "LC_ShortcutKeyAsset.h"
#include "IP_InputManager.h"
#include "CM_KeyTranslate.h"

IP_ShortcutKeyCode::IP_ShortcutKeyCode(int iSlotID)
: iShortcutSlotID(iSlotID)
{
	::memset(&k1, 0, sizeof(k1));
	::memset(&k2, 0, sizeof(k2));
}

void IP_ShortcutKeyCode::Update(const StringType & sSlotValue1, const StringType & sSlotValue2)
{
	if (this->sShortcutSlotValue1 != sSlotValue1)
	{
		this->sShortcutSlotValue1 = sSlotValue1;

		int iKey = UT_BaseStringUtility::toInt(sSlotValue1);
		int iVK  = iKey / 10000;

		// VirtualKey
		this->k1.iVirtualKey = iVK;

		// VirtualKey -> Scancode
		this->k1.iScancode = iVK ? GUISys::GetCodeMap((UCHAR)iVK)->KeyCode : 0;

		this->k1.bUseAlt   = ((iKey / 10) % 10) != 0;
		this->k1.bUseCtrl  = ((iKey / 100) % 10) != 0;
		this->k1.bUseShift = ((iKey / 1000) % 10) != 0;
	}
	if (this->sShortcutSlotValue2 != sSlotValue2)
	{
		this->sShortcutSlotValue2 = sSlotValue2;

		int iKey = UT_BaseStringUtility::toInt(sSlotValue2);
		int iVK  = iKey / 10000;

		// VirtualKey
		this->k2.iVirtualKey = iVK;

		// VirtualKey -> Scancode
		this->k2.iScancode = iVK ? GUISys::GetCodeMap((UCHAR)iVK)->KeyCode : 0;

		this->k2.bUseAlt   = ((iKey / 10) % 10) != 0;
		this->k2.bUseCtrl  = ((iKey / 100) % 10) != 0;
		this->k2.bUseShift = ((iKey / 1000) % 10) != 0;
	}
}

/************************************************************************/

IP_ShortcutKeyCodeMgr::IP_ShortcutKeyCodeMgr()
{
	// 业务代码 放这里不太合适
	// 可以不调用Stop注销 留给析构函数去销毁
	this->StartTraceShotcutKey(SHOTCUT_SLOT_ID_W);
	this->StartTraceShotcutKey(SHOTCUT_SLOT_ID_S);
	this->StartTraceShotcutKey(SHOTCUT_SLOT_ID_A);
	this->StartTraceShotcutKey(SHOTCUT_SLOT_ID_D);
}

IP_ShortcutKeyCodeMgr::~IP_ShortcutKeyCodeMgr()
{
	// clear all
	for(ShortcutKeyCodeList::iterator it = m_KeyCodes.begin(); it != m_KeyCodes.end(); ++it)
	{
		IP_ShortcutKeyCode * p = (*it);
		T_DELETE_D p;
	}
	m_KeyCodes.clear();
}

void IP_ShortcutKeyCodeMgr::Update(LC_ShortcutKeyAsset *pkAssets)
{
	for(ShortcutKeyCodeList::iterator it = m_KeyCodes.begin(); it != m_KeyCodes.end(); ++it)
	{
		IP_ShortcutKeyCode * p = (*it);
		LC_ShortcutKeyAssetEntry * pkShortcutKey = pkAssets->FindKeyDataBySlotID(p->iShortcutSlotID);
		if (pkShortcutKey != NULL)
			p->Update(pkShortcutKey->GetKey1(), pkShortcutKey->GetKey2());
	}
}

bool IP_ShortcutKeyCodeMgr::StartTraceShotcutKey(int iSlotID)
{
	// already existed?
	for(ShortcutKeyCodeList::iterator it = m_KeyCodes.begin(); it != m_KeyCodes.end(); ++it)
	{
		IP_ShortcutKeyCode * p = (*it);
		if (p->iShortcutSlotID == iSlotID)
			return true;
	}

	IP_ShortcutKeyCode * p = T_NEW_D IP_ShortcutKeyCode(iSlotID);
	m_KeyCodes.push_back(p);
	return true;
}

bool IP_ShortcutKeyCodeMgr::StopTraceShotcutKey(int iSlotID)
{
	// existed?
	for(ShortcutKeyCodeList::iterator it = m_KeyCodes.begin(); it != m_KeyCodes.end(); ++it)
	{
		IP_ShortcutKeyCode * p = (*it);
		if (p->iShortcutSlotID == iSlotID)
		{
			T_DELETE_D p;

			m_KeyCodes.erase(it);
			return true;
		}
	}
	return false;
}

IP_ShortcutKeyCode * IP_ShortcutKeyCodeMgr::FindShotcutKey(int iSlotID)
{
	// existed?
	for(ShortcutKeyCodeList::iterator it = m_KeyCodes.begin(); it != m_KeyCodes.end(); ++it)
	{
		IP_ShortcutKeyCode * p = (*it);
		if (p->iShortcutSlotID == iSlotID)
			return p;
	}
	return NULL;
}

/************************************************************************/


bool IP_ShotcutKeyCodeHelpper::KeyIsDown(int iSlotID, bool bCheckAuxKey)
{
	IP_ShortcutKeyCodeMgr & keycodeMgr = IP_ShortcutKeyCodeMgr::GetSingleton();
	IP_ShortcutKeyCode * pkKey = keycodeMgr.FindShotcutKey( iSlotID );
	if (pkKey == NULL)
		return false;

	IP_InputManager & input = IP_InputManager::GetSingleton();
	if (!bCheckAuxKey)
	{
		if (input.KeyIsDown((NiInputKeyboard::KeyCode)pkKey->k1.iScancode))
			return true;
		if (input.KeyIsDown((NiInputKeyboard::KeyCode)pkKey->k1.iScancode))
			return true;
	}
	else
	{
		bool bShiftDown = input.KeyIsDown(NiInputKeyboard::KEY_LSHIFT) || input.KeyIsDown(NiInputKeyboard::KEY_RSHIFT);
		bool bCtrlDown = input.KeyIsDown(NiInputKeyboard::KEY_LCONTROL) || input.KeyIsDown(NiInputKeyboard::KEY_RCONTROL);
		bool bAltDown = input.KeyIsDown(NiInputKeyboard::KEY_LMENU) || input.KeyIsDown(NiInputKeyboard::KEY_RMENU);

		if (   input.KeyIsDown((NiInputKeyboard::KeyCode)pkKey->k1.iScancode)
			&& (!pkKey->k1.bUseShift || bShiftDown)
			&& (!pkKey->k1.bUseCtrl  || bCtrlDown)
			&& (!pkKey->k1.bUseAlt   || bAltDown)
			)
			return true;
		if (   input.KeyIsDown((NiInputKeyboard::KeyCode)pkKey->k2.iScancode)
			&& (!pkKey->k2.bUseShift || bShiftDown)
			&& (!pkKey->k2.bUseCtrl  || bCtrlDown)
			&& (!pkKey->k2.bUseAlt   || bAltDown)
			)
			return true;
	}
	return false;
}

bool IP_ShotcutKeyCodeHelpper::KeyWasReleased(int iSlotID)
{
	IP_ShortcutKeyCodeMgr & keycodeMgr = IP_ShortcutKeyCodeMgr::GetSingleton();
	IP_ShortcutKeyCode * pkKey = keycodeMgr.FindShotcutKey( iSlotID );
	if (pkKey == NULL)
		return false;

	IP_InputManager & input = IP_InputManager::GetSingleton();
	if (input.KeyWasReleased((NiInputKeyboard::KeyCode)pkKey->k1.iScancode))
		return true;
	if (input.KeyWasReleased((NiInputKeyboard::KeyCode)pkKey->k1.iScancode))
		return true;
	return false;
}

static UT_Vec2Int _getScancode(IP_ShortcutKeyCode * pkSKC)
{
	UT_Vec2Int k(0, 0);
	if (pkSKC != NULL)
	{
		k.x = pkSKC->k1.iScancode;
		k.y = pkSKC->k2.iScancode;
	}
	return k;
}

void IP_ShotcutKeyCodeHelpper::WASD_IsDown(bool & u, bool & d, bool & l, bool & r)
{
	IP_ShortcutKeyCodeMgr & keycodeMgr = IP_ShortcutKeyCodeMgr::GetSingleton();
	IP_ShortcutKeyCode * pkW = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_W);
	IP_ShortcutKeyCode * pkS = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_S);
	IP_ShortcutKeyCode * pkA = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_A);
	IP_ShortcutKeyCode * pkD = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_D);

	UT_Vec2Int v2W = _getScancode(pkW);
	UT_Vec2Int v2S = _getScancode(pkS);
	UT_Vec2Int v2A = _getScancode(pkA);
	UT_Vec2Int v2D = _getScancode(pkD);

	IP_InputManager & input = IP_InputManager::GetSingleton();
	u = input.KeyIsDown((NiInputKeyboard::KeyCode)v2W.x) || input.KeyIsDown((NiInputKeyboard::KeyCode)v2W.y);
	d = input.KeyIsDown((NiInputKeyboard::KeyCode)v2S.x) || input.KeyIsDown((NiInputKeyboard::KeyCode)v2S.y);
	l = input.KeyIsDown((NiInputKeyboard::KeyCode)v2A.x) || input.KeyIsDown((NiInputKeyboard::KeyCode)v2A.y);
	r = input.KeyIsDown((NiInputKeyboard::KeyCode)v2D.x) || input.KeyIsDown((NiInputKeyboard::KeyCode)v2D.y);
}

void IP_ShotcutKeyCodeHelpper::WASD_WasReleased(bool & u, bool & d, bool & l, bool & r)
{
	IP_ShortcutKeyCodeMgr & keycodeMgr = IP_ShortcutKeyCodeMgr::GetSingleton();
	IP_ShortcutKeyCode * pkW = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_W);
	IP_ShortcutKeyCode * pkS = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_S);
	IP_ShortcutKeyCode * pkA = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_A);
	IP_ShortcutKeyCode * pkD = keycodeMgr.FindShotcutKey( SHOTCUT_SLOT_ID_D);

	UT_Vec2Int v2W = _getScancode(pkW);
	UT_Vec2Int v2S = _getScancode(pkS);
	UT_Vec2Int v2A = _getScancode(pkA);
	UT_Vec2Int v2D = _getScancode(pkD);

	IP_InputManager & input = IP_InputManager::GetSingleton();
	u = input.KeyWasReleased((NiInputKeyboard::KeyCode)v2W.x) || input.KeyWasReleased((NiInputKeyboard::KeyCode)v2W.y);
	d = input.KeyWasReleased((NiInputKeyboard::KeyCode)v2S.x) || input.KeyWasReleased((NiInputKeyboard::KeyCode)v2S.y);
	l = input.KeyWasReleased((NiInputKeyboard::KeyCode)v2A.x) || input.KeyWasReleased((NiInputKeyboard::KeyCode)v2A.y);
	r = input.KeyWasReleased((NiInputKeyboard::KeyCode)v2D.x) || input.KeyWasReleased((NiInputKeyboard::KeyCode)v2D.y);
}
