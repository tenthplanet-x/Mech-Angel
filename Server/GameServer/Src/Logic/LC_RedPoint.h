//红点系统
#ifndef _H_RedPoint_H_
#define _H_RedPoint_H_
#pragma once

#include "LC_Define.h"
#include "char_data.pb.h"
#include "MG_ProtocolStruct.h"

class LC_ServerPlayer;
//客户端需要服务器保存的一些标记位
class CRedPoint
{
public:
	CRedPoint();
	~CRedPoint();

	//存数据到DB
	void SaveData(char_data_define::PBRedPoint* pkData);
	//从DB加载数据
	void LoadData(const char_data_define::PBRedPoint& data);

	void SetRedPointFlag(LC_ServerPlayer* pkPlayer, int32_t param, bool flag);
	void ResetRedPointFlag(LC_ServerPlayer* pkPlayer);
	void SyncAllRedPointFlag(LC_ServerPlayer* pkPlayer);
	void SyncRedPointFlag(LC_ServerPlayer* pkPlayer, int32_t param, bool flag);
private:
	mem::vector<Protocol::KeyValue_Base_Bool> m_RedPointRecord;
};

#endif