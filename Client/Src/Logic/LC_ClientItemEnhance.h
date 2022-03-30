//ǿ��UI
#ifndef _LC_CLIENT_ITEM_ENHANCE_H
#define _LC_CLIENT_ITEM_ENHANCE_H

#include "LC_PlayerBase.h"
//���ڻᱩ¶��Lua,ע�������淶
class LC_EnhanceSlotEntry : public Memory::MM_BaseObject
{
public:
	void Reset();
	DECL_RTTI_NAME( LC_EnhanceSlotEntry )
	MEMBER_LONG(TypeID)				//����ID ����Ϊ��Ʒ����
	MEMBER_BOOL(Operate)			//�Ƿ�ɲ���
	LC_EnhanceSlotEntry& operator = (LC_EnhanceSlotEntry& pkEntry);
};

class LC_ClientItemEnhance :public Memory::MM_BaseObject
{
public:
	LC_ClientItemEnhance(GameLogic::LC_PlayerBase* pkOnwer);
	virtual ~LC_ClientItemEnhance();
	DECL_RTTI_NAME( LC_ClientItemEnhance )

	enum 
	{
		MAX_SLOT_COUNT = 4,
	};

	int  GetSlotCount();
	void Clear();

	LC_EnhanceSlotEntry* GetEnhanceSlotEntry(int iSlotIndex);

	void EnhanceSlotPackage(int iSlotIndex,long lTypeID);
	void EnhanceSlotActive(int iSlotIndex);
	void EnhanceSlotRemove(int iSlotIndex);

protected:
	LC_EnhanceSlotEntry		m_akEnhanceSlots[MAX_SLOT_COUNT];
	GameLogic::LC_PlayerBase*	m_pkOwner;
};

#endif