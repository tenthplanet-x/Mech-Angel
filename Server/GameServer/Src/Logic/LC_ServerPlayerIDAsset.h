#ifndef _LC_SERVERVPLAYERIDASSET_H
#define _LC_SERVERVPLAYERIDASSET_H

#include "char_data.pb.h"
#include "LC_Define.h"
class LC_ServerPlayer;
//Ò»Ð©id¼ÇÂ¼
class CIDAsset
{
public:
	CIDAsset();
	~CIDAsset();

	void LoadData(const char_data_define::PBIDAsset& data);
	void SaveData(char_data_define::PBIDAsset* pkData);

	void SyncIDAsset(LC_ServerPlayer* pkOwnerPlayer);
	void AddIDByType(LC_ServerPlayer* pkOwnerPlayer, int32_t type, int32_t value);
protected:

private:
	bool CheckIDTypeVaild(int32_t type);
	mem::map<int32_t, mem::set<int32_t> > m_IDInfoMap;
};

#endif
