#ifndef __BUDDY_INSTANCE_DATA_SERVER_PLAYER_H__
#define __BUDDY_INSTANCE_DATA_SERVER_PLAYER_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"

#include "MC_DataDirtyFlag.h"
#include "LC_TranscriptionDataType.h"
#include "LC_Define.h"
#include "LC_SocietyStruct.h"
#include "LC_PackStruct.h"
#include "LC_ItemBase.h"
#include "LC_PlayerBase.h"
#include "LC_WorldLocationAsset.h"
#include "LC_ResourceRecord.h"
#include "LC_ResourceAlert.h"
#include "LC_FacebookReward.h"
#include "NW_ServerSession.h"

#include "UT_ComplexData.h"

#include "MG_Game2DB.h"
#include "MG_ItemProtocol.h"
#include "char_data.pb.h"
#include "msg.pb.h"

#include "MG_BuddyProtocol.h"

#include "UT_TimeManager.h"
#include "UT_LinePath.h"
#include "UT_GameServerPackFilter.h"

#include "publish_define.h"

#include "ACT_SecretTreasure.h"
#include "ACT_ActivityAsset.h"

#include "CF_Buddy.h"

class LC_ServerPlayer;

class BuddyInstanceData_ServerPlayer //: public GameLogic::LC_ServerPlayer
{
public:
	BuddyInstanceData_ServerPlayer(void){}
	~BuddyInstanceData_ServerPlayer() {};

	void Init(){}
	void Clear(void){}

	//int LoadFromDB();
	//int LoadFromTemplate(int32_t nTemplateId);

	//const BuddyInstance* Get();
	//int SaveData();
	// DBLogic
protected:
	friend class BuddyInstanceData;
	friend class BuddyInstance;
	attr_value_type		m_lHPFromDB;
	attr_value_type		m_lMPFromDB;
};

#endif //__ACT_ACTIVITY_ASSET_H__
