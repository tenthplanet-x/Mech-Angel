#pragma once

/************************************************************************/
/* ���ǣ���ݼ��ַ��� -> ��ݼ������ת��                        */
/************************************************************************/

#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "MM_MemoryAllocatorConfig.h"


struct IP_ShortcutKeyCode : public Memory::MM_BaseObject
{
private:
	StringType  sShortcutSlotValue1;		// ������ַ���ֵ	Ex: 870001, VirtualKey:Shift:Ctrl:Alt:1
	StringType  sShortcutSlotValue2;		// ������ַ���ֵ ÿ������������������

public:
	const int	iShortcutSlotID;			// ��Ӧ�Ŀ�ݼ�λ�òۣ�����
	struct Node
	{
		int		iVirtualKey;	// Windows Virtual Key
		int		iScancode;		// ����ɨ����
		bool	bUseShift;
		bool    bUseCtrl;
		bool    bUseAlt;
	};
	Node	    k1, k2;

public:
	explicit IP_ShortcutKeyCode( int iSlotID );
	void  Update(const StringType & sSlotValue1, const StringType & sSlotValue2);
};

/************************************************************************/

class LC_ShortcutKeyAsset;
class IP_ShortcutKeyCodeMgr : public Utility::UT_Singleton<IP_ShortcutKeyCodeMgr>
						    , public Memory::MM_BaseObject
{
public:
	IP_ShortcutKeyCodeMgr();
	virtual ~IP_ShortcutKeyCodeMgr();
	DECL_RTTI_NAME( IP_ShortcutKeyCodeMgr )

	void Update(LC_ShortcutKeyAsset *pkAssets);

	bool StartTraceShotcutKey(int iSlotID);
	bool StopTraceShotcutKey(int iSlotID);
	IP_ShortcutKeyCode * FindShotcutKey(int iSlotID);

private:
	typedef mem::vector<IP_ShortcutKeyCode *>		ShortcutKeyCodeList;
	ShortcutKeyCodeList		m_KeyCodes;
};

/************************************************************************/

// wasd��ݼ����ã��ڿ�ݼ��ʲ��е�����
#define SHOTCUT_SLOT_ID_W 18
#define SHOTCUT_SLOT_ID_S 19
#define SHOTCUT_SLOT_ID_A 20
#define SHOTCUT_SLOT_ID_D 21

// ҵ�����
class IP_ShotcutKeyCodeHelpper
{
public:
	static bool KeyIsDown(int iSlotID, bool bCheckAuxKey = false);	// AuxKey : Ctrl, Shift, Alt
	static bool KeyWasReleased(int iSlotID);

	static void WASD_IsDown(bool & u, bool & d, bool & l, bool & r);
	static void WASD_WasReleased(bool & u, bool & d, bool & l, bool & r);
};
