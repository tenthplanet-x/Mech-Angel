#ifndef __BUDDY_CHAIN_H__
#define __BUDDY_CHAIN_H__

#include "SystemError.h"
#include "IdDefine.h"
#include "SystemDefine.h"
#include <map>
#include "char_data.pb.h"
#include "MG_BuddyProtocol.h"


class BuddyChain
{
public:
	BuddyChain();
	~BuddyChain();
public:
	void Init();
	void Save(char_data_define::buddy_chain_data& data);
	void Load(const char_data_define::buddy_chain_data& data);
	bool SetIndex( int16_t nIndex );
	//void SetCount( int16_t nCount );
	int16_t GetCount();
	int32_t Add( int16_t nIndex, int32_t nBuddyId );
	int32_t Del( int16_t nIndex, int32_t& nOrigBuddyId );
	int32_t Get( int16_t nIndex );
	bool Check( int32_t nBuddyId );
	int32_t Switch( int16_t nIndex, int32_t nBuddyId, int32_t& nOrigBuddyId );
	void SetProtocolData(Protocol::MG_SC_BuddyChainSync& sync);
private:
	bool CheckData(int32_t nBuddyId);
private:
	int16_t m_nEnabledCount;
	int32_t m_nBuddyIds[MAX_BUDDY_COUNT];
};


#endif //__ACT_ACTIVITY_ASSET_H__
