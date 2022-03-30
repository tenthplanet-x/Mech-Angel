#ifndef __BUDDY_INSTANCE_DATA_ATTACKER_H__
#define __BUDDY_INSTANCE_DATA_ATTACKER_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"
#include "LC_LogicObject.h"

class	LC_TargetMap;

#include "char_data.pb.h"

class LC_ServerPlayer;
class BuddyInstanceData_Attacker //: public GameLogic::LC_ServerPlayer
{
public:
	BuddyInstanceData_Attacker(void){}
	~BuddyInstanceData_Attacker() {};

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
};

#endif //__ACT_ACTIVITY_ASSET_H__
