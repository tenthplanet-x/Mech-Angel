#ifndef __BUDDY_SCHEME_H__
#define __BUDDY_SCHEME_H__

#include "SystemError.h"
#include "IdDefine.h"
#include "LC_Define.h"
#include "SystemDefine.h"
#include "char_data.pb.h"
#include "MG_ProtocolStruct.h"

using namespace GameLogic;
using namespace Protocol;
class LC_ServerPlayer;
class BuddySchemeMgr
{
public:
	BuddySchemeMgr();
	~BuddySchemeMgr();

	typedef mem::vector<PS_SingleSchemeData> SchemeData;
	typedef mem::map<uint32_t, SchemeData> PlayerSchemeDataMap;

	void Clear();

	void SaveData(char_data_define::activate_scheme_data* pkData);
	void LoadData(const char_data_define::activate_scheme_data& data);

	//客户端请求设置特殊地图的初始阵容
	void SetSpecialMapInitBuddyActivateScheme_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType);
	//客户端请求设置阵容数据
	void SetBuddyActivateScheme_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, const std::vector<PS_SingleSchemeData>& schemeData);
	//设置出战阵容
	void SetBuddyActivateScheme(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, const mem::vector<int32_t>& buddyVec);
	//设置当前阵容
	ResultType SetCurrentScheme(LC_ServerPlayer* pkOwnerPlayer, int32_t index, int32_t buddyID);
	//同步玩家的阵容
	void SyncBuddyActivateScheme(LC_ServerPlayer* pkOwnerPlayer);
	//通过地图id和组id，获得配置的阵容信息
	int32_t GetBuddySchemeData(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, mem::vector<int>& buddyIDVec);
	//通过地图id，获得配置的队伍id
	void GetMapSchemeGroupData(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, mem::vector<uint32_t>& groupIDVec);
	//获得buddy所有出战阵容
	void GetAllSchemeData(PS_Player_SchemeData& schemeData);
	//填充robot阵容数据
	void SetRobotSchemeData(const Utility::UT_SIMDataList& data);

	//是否需要切换出战阵容
	bool CheckNeedSwitchBuddyOffensiveScheme(uint32_t mapID, uint32_t groupID);
	bool CheckSchemeReady(uint32_t mapID, uint32_t groupID, uint32_t schemeType);
	ResultType CheckModifyCurrentScheme(LC_ServerPlayer* pkOwnerPlayer, int32_t index, int32_t buddyID);

	//当前用的阵容信息
	void GetCurrentSchemeInfo(uint32_t& schemeID, uint32_t& groupID);
	void SetCurrentSchemeInfo(uint32_t schemeID, uint32_t groupID);

	//设置当前被挑战即将进入的地图id
	void SetBeChallangeMapID(uint32_t mapID);
	uint32_t GetBeChallangeMapID();

	ResultType SetMapInitScheme(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, std::vector<PS_SingleSchemeData>& schemeData);
	void SetRobotSchemeDataByMapID(uint32_t mapID, const SchemeData& schemeData);
protected:

private:
	ResultType SetAllSchemeDataBySchemeID(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, const std::vector<PS_SingleSchemeData>& schemeData);
	void SetSchemeData(LC_ServerPlayer* pkOwnerPlayer, uint32_t schemeType, uint32_t schemeID, uint32_t groupID, uint32_t index, int32_t buddyID, bool updateToGroup);
    ResultType CheckSchemeDataVaild_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, const std::vector<PS_SingleSchemeData>& schemeData);
	ResultType CheckSchemeDataVaild(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, const mem::vector<int32_t>& buddyVec);
	bool GetPlayerSchemeData(uint32_t mapID, uint32_t schemeType, int32_t& schemeID, BuddySchemeMgr::PlayerSchemeDataMap::iterator*);
	//更新阵容到group
	void UpdateSchemeDataToGroup(LC_ServerPlayer* pkOwnerPlayer, uint32_t schemeID, uint32_t schemeType);
private:
	uint32_t m_nBeChallengeMapID;//当前被挑战即将进入的地图的阵容 不存db
	uint32_t m_nCurrentSchemeID;//现在出战的阵容地图（INIT_SCHEME_ID，表示玩家设置的野外阵容）
	uint32_t m_nCurrentSchemeGroupID;//现在出战的阵容组id（INIT_SCHEME_GROUPID，表示玩家设置的野外阵容组）
	PlayerSchemeDataMap m_mPlayerOffensiveSchemeDataMap;//玩家配置的地图出战阵容 key:schemeID
	PlayerSchemeDataMap m_mPlayerDefenceSchemeDataMap;//玩家配置的地图防守阵容 key:schemeID
};

#endif //__ACT_ACTIVITY_ASSET_H__
