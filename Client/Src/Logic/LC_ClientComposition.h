//客户端合成类
#ifndef _LC_CLIENT_COMPOSITION_H
#define _LC_CLIENT_COMPOSITION_H

#include "LC_PlayerBase.h"
//由于会暴露给Lua,注意命名规范
class LC_CompositionSlotEntry : public Memory::MM_BaseObject
{
public:
	void Reset();
	DECL_RTTI_NAME( LC_CompositionSlotEntry )
		MEMBER_LONG(TypeID)				//类型ID 可能为物品或技能
		MEMBER_BOOL(Operate)			//是否可操作
		LC_CompositionSlotEntry& operator = (LC_CompositionSlotEntry& pkEntry);
};

class LC_ClientComposition :public Memory::MM_BaseObject
{
public:
	LC_ClientComposition(GameLogic::LC_PlayerBase* pkOnwer);
	virtual ~LC_ClientComposition();
	DECL_RTTI_NAME( LC_ClientComposition )

		enum 
	{
		MAX_SLOT_COUNT = 3,
	};

	int  GetSlotCount();
	void Clear();

	LC_CompositionSlotEntry* GetCompositionSlotEntry(int iSlotIndex);

	void CompositionSlotPackage(int iSlotIndex,long lTypeID);
	void CompositionSlotActive(int iSlotIndex);
	void CompositionSlotRemove(int iSlotIndex);

protected:
	LC_CompositionSlotEntry		m_akCompositionSlots[MAX_SLOT_COUNT];
	GameLogic::LC_PlayerBase*	m_pkOwner;
};

#endif