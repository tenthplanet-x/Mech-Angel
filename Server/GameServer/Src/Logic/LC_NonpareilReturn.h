#ifndef _LC_NONPAREILRETURN_H_
#define _LC_NONPAREILRETURN_H_

#include "LC_Define.h"
#include "char_data.pb.h"

#include "MG_ProtocolStruct.h"
#include "MG_SyncProtocol.h"
#include "CF_NonpareilItem.h"

using namespace Protocol;
using namespace CSVFile;
class LC_ServerPlayer;
class CNonpareilReturn
{
public:
	void Clear();

	CNonpareilReturn();
	~CNonpareilReturn();

	void SaveData(char_data_define::PBNonpareilReturnInfo* pkData);
	void LoadData(const char_data_define::PBNonpareilReturnInfo& data);

	void BuyNonpareilItem(LC_ServerPlayer* pkOwnerPlayer, int32_t id);
	void GetNonpareilReturn(LC_ServerPlayer* pkOwnerPlayer, int32_t id);
	void SyncNonpareilRecordAsset(LC_ServerPlayer* pkOwnerPlayer);
protected:
private:
	std::vector<KeyValue_Base_Bool> m_NonpareilReturnInfo;                  //key:NonpareilItem表的id  value:是否领过返利 true:领过 false:未领过
};

#endif

