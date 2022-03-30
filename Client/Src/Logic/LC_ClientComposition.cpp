#include "ClientPCH.h"
#include "LC_ClientComposition.h"

using namespace GameLogic;
using namespace Memory;

//--------------------------------------------------
void LC_CompositionSlotEntry::Reset()
{
	m_lTypeID = 0;
	m_bOperate = false;
}
//-------------------------------------------------------
LC_CompositionSlotEntry& LC_CompositionSlotEntry::operator = (LC_CompositionSlotEntry& rkEntry)
{
	SetTypeID(rkEntry.GetTypeID());
	SetOperate(rkEntry.GetOperate());
	return *this;
}
//--------------------------------------------------
LC_ClientComposition::LC_ClientComposition(LC_PlayerBase* pkOnwer)
: MM_BaseObject()
, m_pkOwner(pkOnwer)
{
	Clear();
}
//--------------------------------------------------
LC_ClientComposition::~LC_ClientComposition()
{
	Clear();
}
//--------------------------------------------------
int LC_ClientComposition::GetSlotCount()
{
	return MAX_SLOT_COUNT;
}
//--------------------------------------------------
void LC_ClientComposition::Clear()
{
	for(int i = 0; i < MAX_SLOT_COUNT; i++)
	{
		LC_CompositionSlotEntry& rkEntry = m_akCompositionSlots[i];
		rkEntry.Reset();
	}
}
//--------------------------------------------------
LC_CompositionSlotEntry* LC_ClientComposition::GetCompositionSlotEntry(int iSlotIndex)
{
	if(iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "试图访问非法的快捷位");
		return NULL;
	}

	return &(m_akCompositionSlots[iSlotIndex]);
}
//--------------------------------------------------
void LC_ClientComposition::CompositionSlotPackage(int iSlotIndex,long lTypeID)
{
	if(iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR,"试图访问非法的快捷位");
		return;
	}

	LC_CompositionSlotEntry& rkEntry = m_akCompositionSlots[iSlotIndex];
	rkEntry.SetTypeID(lTypeID);
	rkEntry.SetOperate(true);

	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_COMPOSITION_SLOT,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_COMPOSITION_SLOT);
	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_BACKPACKAGE,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_BACKPACKAGE);
}
//---------------------------------------------------------------------------
void LC_ClientComposition::CompositionSlotActive(int iSlotIndex)
{
	if (iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR,"试图访问非法的快捷位");
		return;
	}
	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_COMPOSITION_SLOT,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_COMPOSITION_SLOT);
}
//---------------------------------------------------------------------------
void LC_ClientComposition::CompositionSlotRemove(int iSlotIndex)
{
	if (iSlotIndex < 0 || iSlotIndex >= MAX_SLOT_COUNT)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR,"试图访问非法的快捷位");
		return;
	}
	m_akCompositionSlots[iSlotIndex].Reset();
	m_pkOwner->SetDirtyFlag(DIRTY_FLAG_COMPOSITION_SLOT,true);
	m_pkOwner->AddDirtyFlagRevisionID(DIRTY_FLAG_COMPOSITION_SLOT);
}