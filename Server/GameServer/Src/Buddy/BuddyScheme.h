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

	//�ͻ����������������ͼ�ĳ�ʼ����
	void SetSpecialMapInitBuddyActivateScheme_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType);
	//�ͻ�������������������
	void SetBuddyActivateScheme_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, const std::vector<PS_SingleSchemeData>& schemeData);
	//���ó�ս����
	void SetBuddyActivateScheme(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, const mem::vector<int32_t>& buddyVec);
	//���õ�ǰ����
	ResultType SetCurrentScheme(LC_ServerPlayer* pkOwnerPlayer, int32_t index, int32_t buddyID);
	//ͬ����ҵ�����
	void SyncBuddyActivateScheme(LC_ServerPlayer* pkOwnerPlayer);
	//ͨ����ͼid����id��������õ�������Ϣ
	int32_t GetBuddySchemeData(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, mem::vector<int>& buddyIDVec);
	//ͨ����ͼid��������õĶ���id
	void GetMapSchemeGroupData(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, mem::vector<uint32_t>& groupIDVec);
	//���buddy���г�ս����
	void GetAllSchemeData(PS_Player_SchemeData& schemeData);
	//���robot��������
	void SetRobotSchemeData(const Utility::UT_SIMDataList& data);

	//�Ƿ���Ҫ�л���ս����
	bool CheckNeedSwitchBuddyOffensiveScheme(uint32_t mapID, uint32_t groupID);
	bool CheckSchemeReady(uint32_t mapID, uint32_t groupID, uint32_t schemeType);
	ResultType CheckModifyCurrentScheme(LC_ServerPlayer* pkOwnerPlayer, int32_t index, int32_t buddyID);

	//��ǰ�õ�������Ϣ
	void GetCurrentSchemeInfo(uint32_t& schemeID, uint32_t& groupID);
	void SetCurrentSchemeInfo(uint32_t schemeID, uint32_t groupID);

	//���õ�ǰ����ս��������ĵ�ͼid
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
	//�������ݵ�group
	void UpdateSchemeDataToGroup(LC_ServerPlayer* pkOwnerPlayer, uint32_t schemeID, uint32_t schemeType);
private:
	uint32_t m_nBeChallengeMapID;//��ǰ����ս��������ĵ�ͼ������ ����db
	uint32_t m_nCurrentSchemeID;//���ڳ�ս�����ݵ�ͼ��INIT_SCHEME_ID����ʾ������õ�Ұ�����ݣ�
	uint32_t m_nCurrentSchemeGroupID;//���ڳ�ս��������id��INIT_SCHEME_GROUPID����ʾ������õ�Ұ�������飩
	PlayerSchemeDataMap m_mPlayerOffensiveSchemeDataMap;//������õĵ�ͼ��ս���� key:schemeID
	PlayerSchemeDataMap m_mPlayerDefenceSchemeDataMap;//������õĵ�ͼ�������� key:schemeID
};

#endif //__ACT_ACTIVITY_ASSET_H__
