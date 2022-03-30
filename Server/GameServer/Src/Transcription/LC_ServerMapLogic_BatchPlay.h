#ifndef _LC_SERVERMAPLOGIC_BATCH_H
#define _LC_SERVERMAPLOGIC_BATCH_H

/***************************************************************
	Author:				sunny
	Time:				2014-07-08
	FileName:			LC_ServerMapLogic_Batch
	FileExt:			.h
	Porpose:			批次管理器
****************************************************************/
#include "LC_TranscriptionDataDefine.h"
#include "LC_TranscriptionDataType.h"
#include "LC_Server_TranscriptionDungeon_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"

#include "MG_Stream.h"

using namespace GameLogic;
using namespace DungeonConfig;

#define SPAN_INFO_ELITE_MINOR 2
#define SPAN_INFO_ELITE 1
#define SPAN_INFO_MONSTER 0



enum MapLogicFinishEvent
{
	MapLogicFinishEvent_Time = 0, //时间驱动 value1 多长时间
	MapLogicFinishEvent_MonsterCount = 1, //怪物数量 value1 怪物数量
	MapLogicFinishEvent_SpecialMonster = 2, //指定怪死亡 value1 怪物类型,value2 剩余数量
	MapLogicFinishEvent_MonsterHP = 3, //指定怪血量 value1 怪物类型 value2 怪物血量
	MapLogicFinishEvent_Finish_Immediately = 4, //立即结束 无参数
	MapLogicFinishEvent_Finish_Dialogue = 5, //对话结束,value1 超时时间
	MapLogicFinishEvent_Target_Die = 6, //目标死亡
	MapLogicFinishEvent_EnterLogicArea = 7, //进入逻辑区域,value1 区域ID
	MapLogicFinishEvent_FinishTask = 8,//相位副本任务
};

enum MapLogicStartEvent
{
	MapLogicStartEvent_PlayerEnter = 0, //第一个玩家进入 无参数
	MapLogicStartEvent_EnterDoor = 1, //进门 value1 门序号
	MapLogicStartEvent_EnterLogicArea = 2, //进入指定逻辑区域 value1 区域号
	MapLogicStartEvent_Immediately = 3, //立即开始
};

enum MapLogicEventHandle
{
	MapLogicEventHandle_None = 0, //无事
	MapLogicEventHandle_TransAll = 1, //传送所有人 value1 位置
	MapLogicEventHandle_Cutscenes = 2, //过场动画 value1 charid,value2 时间
	MapLogicEventHandle_TransLogicArea = 3, //传送指定逻辑区域的人 value1 传送位置 value2 指定逻辑区域ID（不是该ID的传送）
	MapLogicEventHandle_CloseDoor = 4, //关门 value1 门序号
	MapLogicEventHandle_SpanDialogueNpc = 5, //刷对话NPC value1 NPC类型, value2 位置
	MapLogicEventHandle_Dialogue = 6, //直接对话 value1 对话的Id
	MapLogicEventHandle_GenPortal = 7, //刷传送门，value1 chartype，value2 位置，value3 下个副本resid
	MapLogicEventHandle_OpenDoor = 8, //开门 value1 门序号
	MapLogicEventHandle_StageProgressNotify = 9, //value1 进度序号,value2 进度状态（0 开始，1 中间，2 结束）
	MapLogicEventHandle_MsgNotify = 10, //value1 消息号，value2 0 当前副本 1 全服，value3 频道
	MapLogicEventHandle_ClearNPC = 11, //value1 清理类型(0 所有怪包括中立和建筑,连门一并移除），1特定阵营，2特定类型),value2 上面各种类型对应的值，value3 上面各种类型对应的值，value4 上面各种类型对应的值，value5 上面各种类型对应的值
	MapLogicEventHandle_SetTransPoint = 12, //value1 (0 不传送，1 随机传送，2 指定传送点),value2 x,value3 y
};

struct LC_ServerMapLogic_Spawn_Base : public ::Memory::MM_BaseObject
{
	DECL_RTTI_NAME(LC_ServerMapLogic_Spawn_Base)
	int Flag;
	int TypeId;
	PosAndLoad Pos;
	int Count;
	int Angel;

	LC_ServerMapLogic_Spawn_Base(int i_Flag): Flag(i_Flag), TypeId(0), Count(0), Angel(-1) {}
};

struct LC_ServerMapLogic_Spawn_Monster: public LC_ServerMapLogic_Spawn_Base
{
	LC_ServerMapLogic_Spawn_Monster(): LC_ServerMapLogic_Spawn_Base(SPAN_INFO_MONSTER) {}
};

struct LC_ServerMapLogic_Spawn_Elite: public LC_ServerMapLogic_Spawn_Base
{
	EliteSkillRuleSetType* m_skillrule;
	LC_ServerMapLogic_Spawn_Elite(): LC_ServerMapLogic_Spawn_Base(SPAN_INFO_ELITE), m_skillrule(NULL) {}
};

class LC_ServerMapLogic_Place;
class LC_ServerMapLogic_BatchPlay;
class LC_ServerMapLogic_Batch : public ::Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME(LC_ServerMapLogic_Batch)
	enum BatchState
	{
		BATCHNONE,
		BATCHPREPARE,
		BATCHNORMAL,
		BATCHFINISH
	};

	typedef mem::vector<LC_ServerMapLogic_Spawn_Base*> SpawnList;
	typedef SpawnList::iterator SpawnListIt;
	typedef mem::map<int, SpawnList> SpawnMap;
	typedef SpawnMap::iterator SpawnMapIt;
public:
	LC_ServerMapLogic_Batch();
	virtual ~LC_ServerMapLogic_Batch();
	virtual bool init(const DungeonConfig::BatchEntry& Data, int index, bool belite, LC_ServerMapLogic_BatchPlay* parent);
	virtual void uninit();
	virtual bool Start(float fCurrentTime);
	virtual void End();
	virtual bool GenMonsters();
	virtual bool DoStart();
	virtual bool DoFinish();
	virtual void CheckFinish(float fCurrentTime, float fDeltaTime);
	virtual void update(float fCurrentTime, float fDeltaTime);
	int get_Index() {return m_curIndex;};
	BatchState get_state() {return m_state;};
	virtual void EventNotify(MapLogicEvent& kEvent);
	void SetTaskLogicFinish();
private:
	LC_ServerMapLogic_Place* GetPlace();
protected:
	int m_curIndex;
	SpawnMap m_spanmap;
	float m_starttime;
	float m_delaytime;
	BatchState m_state;
	DungeonConfig::Event m_Finishevent;
	DungeonConfig::Event m_Finishhandle;
	DungeonConfig::Event m_Starthandle;
	DungeonConfig::EventListenerList m_Listeners;
	LC_ServerMapLogic_BatchPlay* m_parent;
	int m_focusorz;
	float m_logicdelayTime;
	int32_t m_StatCatagory;//伤害统计类型
};

class LC_ServerGuildInfo;
class LC_ServerMapLogic_BatchPlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME(LC_ServerMapLogic_BatchPlay)
	typedef mem::vector<LC_ServerMapLogic_Batch*> BatchSlotType;
public:
	LC_ServerMapLogic_BatchPlay();
	virtual ~LC_ServerMapLogic_BatchPlay();
	bool Init(LC_ServerMapLogic_Place* place, Dungeon_Config* config, bool settle);
	virtual void UnInit();
	virtual bool Start();
	virtual void End();
	virtual bool Update(float fCurrentTime, float fDeltaTime);
	virtual void EventNotify(MapLogicEvent& kEvent);
	virtual void PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent);
	virtual bool CheckEnd();
	virtual bool CheckRun();
	virtual void Save(Protocol::MG_OUTPUT_CDR& rkStream);
	virtual void Load(Protocol::MG_INPUT_CDR& rkStream);
	void SetGuild(LC_ServerGuildInfo* pGuild);
	void Settle();
	bool PlayerSettle(object_id_type playerid);
	inline bool CheckProcess(void){return m_State == MapLogic_NormalPlayState_Run;}
	inline MapLogic_NormalPlayState GetState(void){return m_State;	}
	void SetStageProgress(int index, int state);
	void GetStageProgress(int& index, int& state) {index = m_StageProgress; state = m_StageProgressState;};
	Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS* progress2proto(Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS& msg);
	void SetResult(bool pass);
	inline bool GetResult(void)	{return m_Pass;}
	bool CheckReachEvent(bool finish, DungeonConfig::Event& e);

	//塔防
	bool CheckTarget(void);
	bool SpanTarget(TargetEntry& target);
	LC_ServerNPC* GetTarget();
	void ClearTarget();
	void __sendTargetHp();

	//批次
	int GetBatchPosSize();
	bool GetBatchPos(int index, PosAndLoad& pos);
	int GetBatchMonsterSize();
	bool GetBatchMonster(int index, mem::vector<MonsterEntry>& monsters);
	bool GetBatch(int index, DungeonConfig::BatchEntry& batch);
	void GetNotifyMsg(int index, StringType& msg);
	int GetBatchEliteMonsterSize();
	bool GetBatcElitehMonster(int index, int& elite);
	EliteSkillRuleSetType* GetBatchEliteSkillRule();
	bool BatchPlayEventHandle(DungeonConfig::Event& handledata, MapLogicEvent* pkSrc=NULL);

	inline bool CheckPlayerSettle(object_id_type playerid){return m_AllSettlePlayer.count(playerid) > 0;}
	inline bool GetSettle(void){return m_NeedSettle;}
private:
	void __doStart();
	void __checkStart();
	void __checkEnd();

private:
	Dungeon_Config* m_Config;
	BatchSlotType m_BatchSlot;
	int m_CurBatchIndex;
	int m_TotalBatchSize;
	GameLogic::LC_IDSet m_eliteset;//刷精英怪的批次
	MapLogic_NormalPlayState m_State;
	//副本进度
	int32_t m_StageProgress;
	int32_t m_StageProgressState;
	int32_t m_SaveBatchIndex;
	bool m_Pass;
	float m_TargetTime;
	bool m_TargetRaid;
	int m_TargetOrg;
	//中途结算
	mem::set<int> m_AllSettlePlayer;
	bool m_NeedSettle;
	LC_ServerGuildInfo*	m_pGuildInfo;

	int64_t  m_UserData;
};
#endif