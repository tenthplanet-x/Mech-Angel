//���ϵͳ
#ifndef _H_RedPoint_H_
#define _H_RedPoint_H_
#pragma once

#include "LC_Define.h"
#include "char_data.pb.h"
#include "MG_ProtocolStruct.h"

class LC_ServerPlayer;
//�ͻ�����Ҫ�����������һЩ���λ
class CRedPoint
{
public:
	CRedPoint();
	~CRedPoint();

	//�����ݵ�DB
	void SaveData(char_data_define::PBRedPoint* pkData);
	//��DB��������
	void LoadData(const char_data_define::PBRedPoint& data);

	void SetRedPointFlag(LC_ServerPlayer* pkPlayer, int32_t param, bool flag);
	void ResetRedPointFlag(LC_ServerPlayer* pkPlayer);
	void SyncAllRedPointFlag(LC_ServerPlayer* pkPlayer);
	void SyncRedPointFlag(LC_ServerPlayer* pkPlayer, int32_t param, bool flag);
private:
	mem::vector<Protocol::KeyValue_Base_Bool> m_RedPointRecord;
};

#endif