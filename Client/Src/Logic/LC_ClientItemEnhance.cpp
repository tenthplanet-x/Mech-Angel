#include "ClientPCH.h"
#include "LC_ClientItemEnhance.h"

using namespace GameLogic;
using namespace Memory;
//--------------------------------------------------
void LC_EnhanceSlotEntry::Reset()
{
	m_lTypeID = 0;
	m_bOperate = false;
}
//-------------------------------------------------------
LC_EnhanceSlotEntry& LC_EnhanceSlotEntry::operator = (LC_EnhanceSlotEntry& rkEntry)
{
	SetTypeID(rkEntry.GetTypeID());
	SetOperate(rkEntry.GetOperate());
	return *this;
}
//--------------------------------------------------
LC_ClientItemEnhance::LC_ClientItemEnhance(LC_PlayerBase* pkOnwer)
: MM_BaseObject()
, m_pkOwner(pkOnwer)
{
	Clear();
}
//--------------------------------------------------
LC_ClientItemEnhance::~LC_ClientItemEnhance()
{
	Clear();
}
//--------------------------------------------------
int LC_ClientItemEnhance::GetSlotCount()
{
	return MAX_SLOT_COUNT;
}
//--------------------------------------------------
void LC_ClientItemEnhance::Clear()
{
	for(int i = 0; i < MAX_SLOT_COUNT; i++)
	{
		LC_EnhanceSlotEntry& rkEntry = m_akEnhanceSlots[i];
		rkEntry.Reset();
	}
}
//--------------------------------------------------
LC_EnhanceSlotEntry* LC_ClientItemEnhance::GetEnhanceSlotEntry(int iSlotIndex)
{
	if(iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "试图访问非法的快捷位");
		return NULL;
	}

	return &(m_akEnhanceSlots[iSlotIndex]);
}
//--------------------------------------------------
void LC_ClientItemEnhance::EnhanceSlotPackage(int iSlotIndex,long lTypeID)
{
	if(iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR,"试图访问非法的快捷位");
		return;
	}

	LC_EnhanceSlotEntry& rkEntry = m_akEnhanceSlots[iSlotIndex];
	rkEntry.SetTypeID(lTypeID);
	rkEntry.SetOperate(true);

	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_ENHANCE_SLOT,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_ENHANCE_SLOT);
	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_BACKPACKAGE,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_BACKPACKAGE);
}
//---------------------------------------------------------------------------
void LC_ClientItemEnhance::EnhanceSlotActive(int iSlotIndex)
{
	if (iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR,"试图访问非法的快捷位");
		return;
	}
	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_ENHANCE_SLOT,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_ENHANCE_SLOT);
}
//---------------------------------------------------------------------------
void LC_ClientItemEnhance::EnhanceSlotRemove(int iSlotIndex)
{
	if (iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR,"试图访问非法的快捷位");
		return;
	}
	m_akEnhanceSlots[iSlotIndex].Reset();
	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_ENHANCE_SLOT,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_ENHANCE_SLOT);
}