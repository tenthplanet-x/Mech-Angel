#include "BuddyChain.h"
#include "CF_Buddy.h"

#define		LOCKED_POS_ID		-1

BuddyChain::BuddyChain()
{
	Init();
}

BuddyChain::~BuddyChain()
{

}

void BuddyChain::Init()
{
	m_nEnabledCount = 0;
	for ( int i = 0; i < MAX_BUDDY_COUNT; i++ )
		m_nBuddyIds[i] = LOCKED_POS_ID;
}

void BuddyChain::Save(char_data_define::buddy_chain_data& data)
{
	if ( m_nEnabledCount > MAX_BUDDY_COUNT )
		m_nEnabledCount = MAX_BUDDY_COUNT;

	data.set_enabled_slot_count(m_nEnabledCount);
	for ( int i = 0; i < MAX_BUDDY_COUNT; i++ )
	{
		data.mutable_buddy_ids()->Add(m_nBuddyIds[i]);
	}
}

void BuddyChain::Load(const char_data_define::buddy_chain_data& data)
{
	m_nEnabledCount = data.enabled_slot_count();
	int nInitSize = data.buddy_ids_size();
	for ( int i = 0; i < nInitSize; i++ )
	{
		m_nBuddyIds[i] = data.buddy_ids(i);
	}
}

bool BuddyChain::SetIndex( int16_t nIndex )
{
	if ( nIndex < 0 || nIndex >= MAX_BUDDY_COUNT )
		return false;
	if ( m_nBuddyIds[ nIndex ] != LOCKED_POS_ID )
		return false;
	m_nBuddyIds[ nIndex ] = INVALID_ACTOR_ID;
	m_nEnabledCount++;
	return true;
}

int16_t BuddyChain::GetCount()
{
	return m_nEnabledCount;
}


int32_t BuddyChain::Add( int16_t nIndex, int32_t nBuddyId )
{
	if ( nIndex < 0 || nIndex >= MAX_BUDDY_COUNT )
	{
		return RE_BUDDY_CHAIN_ERROR_INVALID_POS_INDEX;
	}

	if ( m_nBuddyIds[nIndex] != INVALID_ACTOR_ID )
	{
		return RE_BUDDY_CHAIN_ERROR_INVALID_SLOT;
	}

	if (CheckData(nBuddyId))
	{
		return RE_BUDDY_CHAIN_ERROR_EXIST_BUDDY;
	}

	m_nBuddyIds[nIndex] = nBuddyId;

	return RE_SUCCESS;
}

int32_t BuddyChain::Del( int16_t nIndex, int32_t& nOrigBuddyId )
{
	if ( nIndex < 0 || nIndex >= MAX_BUDDY_COUNT )
	{
		return RE_BUDDY_CHAIN_ERROR_INVALID_POS_INDEX;
	}

	if ( m_nBuddyIds[nIndex] <= INVALID_ACTOR_ID )
	{
		return RE_BUDDY_CHAIN_ERROR_INVALID_SLOT;
	}

	nOrigBuddyId = m_nBuddyIds[nIndex];
	m_nBuddyIds[nIndex] = INVALID_ACTOR_ID;

	return RE_SUCCESS;
}

int32_t BuddyChain::Get( int16_t nIndex )
{
	if ( nIndex < 0 || nIndex >= MAX_BUDDY_COUNT )
		return INVALID_ACTOR_ID;

	return m_nBuddyIds[ nIndex ];
}

bool BuddyChain::Check( int32_t nBuddyId )
{
	return CheckData(nBuddyId);
}

// Switc的逻辑是，必须是有效的buddy之间做switch。否则，一个叫Add，一个叫Del
int32_t BuddyChain::Switch( int16_t nIndex, int32_t nBuddyId, int32_t& nOrigBuddyId )
{
	if ( nIndex < 0 || nIndex >= m_nEnabledCount )
	{
		return RE_BUDDY_CHAIN_ERROR_INVALID_POS_INDEX;
	}

	if ( m_nBuddyIds[nIndex] == INVALID_ACTOR_ID )
	{
		return RE_BUDDY_CHAIN_ERROR_INVALID_SLOT;
	}

	if (CheckData(nBuddyId))
	{
		return RE_BUDDY_CHAIN_ERROR_EXIST_BUDDY;
	}

	nOrigBuddyId = m_nBuddyIds[nIndex];
	m_nBuddyIds[nIndex] = nBuddyId;

	return RE_SUCCESS;
}

void BuddyChain::SetProtocolData(Protocol::MG_SC_BuddyChainSync& sync)
{
	sync.m_nCount = MAX_BUDDY_COUNT;
	for ( int i = 0; i < MAX_BUDDY_COUNT; i++ )
	{
		sync.m_Data[i] = m_nBuddyIds[i];
	}
}

bool BuddyChain::CheckData(int32_t nBuddyId)
{
	for ( int i = 0; i < MAX_BUDDY_COUNT; i++ )
	{
		if ( m_nBuddyIds[i] == nBuddyId && nBuddyId != INVALID_ACTOR_ID )
			return true;
	}
	return false;
}

