#ifndef _LC_SERVERPLAYERMAPINFO_H_
#define _LC_SERVERPLAYERMAPINFO_H_

#include "MG_ProtocolStruct.h"
#include "LC_Define.h"

using namespace Protocol;
class LC_ServerPlayer;
//玩家设置的，与地图有关的数据
class CPlayerMapInfo
{
public:
	CPlayerMapInfo();
	~CPlayerMapInfo();

	void SaveData(char_data_define::PBPlayerMapInfo* pkData);
	void LoadData(char_data_define::PBPlayerMapInfo data);

	void SetMapBossFlag(LC_ServerPlayer* pkOwnerPlayer, PB_ServerPlayerBossFlagData& data);
	void SyncMapBossFlagInfo(LC_ServerPlayer* pkOwnerPlayer);
protected:

public:
	mem::vector<PB_ServerPlayerBossFlagData> m_vServerPlayerBossFlagInfo;//黄金首领关注状态
};

#endif