#ifndef _LC_SERVERMAPLOGIC_AUTOSPAWNPLAY_H
#define _LC_SERVERMAPLOGIC_AUTOSPAWNPLAY_H

/***************************************************************
	Author:				sunny
	Time:				2014-07-25	
	FileName:			LC_ServerMapLogic_AutoSpawnPlay
	FileExt:			.h
	Porpose:			Ñ­»·Ë¢¹ÖÍæ·¨
****************************************************************/
#include "LC_ServerMapLogic_MonsterConfig.h"
#include "LC_ServerMapLogic_PlayBase.h"
using namespace GameLogic;
using namespace DungeonConfig;

class LC_ServerMapLogic_AutoSpawnPlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME( LC_ServerMapLogic_AutoSpawnPlay )
public:
	LC_ServerMapLogic_AutoSpawnPlay();
	virtual ~LC_ServerMapLogic_AutoSpawnPlay();
	bool Init(LC_ServerMapLogic_Place* place,DungeonConfigData::AutoSpawnMonsterDataMap& config);
	virtual void UnInit();
	virtual bool Start();
	virtual void End();
	virtual bool Update(float fCurrentTime, float fDeltaTime);
	virtual bool CheckEnd();
	virtual bool CheckRun();

	void SetFirstTime(int mintime,int maxtime);
	void SetIntervalTime(int mintime,int maxtime);
	void ActiveSpawnPoint(int lSpawnPointID, object_id_type id);
private:
	DungeonConfigData::AutoSpawnMonsterDataMap m_Config;
	MapLogic_NormalPlayState m_State;
	float m_fLastAutoSpawnTime;

	int m_FirstSpawnTimeMin;
	int m_FirstSpawnTimeMax;
	
	int m_IntervalSpawnTimeMin;
	int m_IntervalSpawnTimeMax;
};
#endif