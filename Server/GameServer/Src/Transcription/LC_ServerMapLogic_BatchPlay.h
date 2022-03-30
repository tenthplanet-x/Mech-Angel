#ifndef _LC_SERVERMAPLOGIC_BATCH_H
#define _LC_SERVERMAPLOGIC_BATCH_H

/***************************************************************
	Author:				sunny
	Time:				2014-07-08
	FileName:			LC_ServerMapLogic_Batch
	FileExt:			.h
	Porpose:			���ι�����
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
	MapLogicFinishEvent_Time = 0, //ʱ������ value1 �೤ʱ��
	MapLogicFinishEvent_MonsterCount = 1, //�������� value1 ��������
	MapLogicFinishEvent_SpecialMonster = 2, //ָ�������� value1 ��������,value2 ʣ������
	MapLogicFinishEvent_MonsterHP = 3, //ָ����Ѫ�� value1 �������� value2 ����Ѫ��
	MapLogicFinishEvent_Finish_Immediately = 4, //�������� �޲���
	MapLogicFinishEvent_Finish_Dialogue = 5, //�Ի�����,value1 ��ʱʱ��
	MapLogicFinishEvent_Target_Die = 6, //Ŀ������
	MapLogicFinishEvent_EnterLogicArea = 7, //�����߼�����,value1 ����ID
	MapLogicFinishEvent_FinishTask = 8,//��λ��������
};

enum MapLogicStartEvent
{
	MapLogicStartEvent_PlayerEnter = 0, //��һ����ҽ��� �޲���
	MapLogicStartEvent_EnterDoor = 1, //���� value1 �����
	MapLogicStartEvent_EnterLogicArea = 2, //����ָ���߼����� value1 �����
	MapLogicStartEvent_Immediately = 3, //������ʼ
};

enum MapLogicEventHandle
{
	MapLogicEventHandle_None = 0, //����
	MapLogicEventHandle_TransAll = 1, //���������� value1 λ��
	MapLogicEventHandle_Cutscenes = 2, //�������� value1 charid,value2 ʱ��
	MapLogicEventHandle_TransLogicArea = 3, //����ָ���߼�������� value1 ����λ�� value2 ָ���߼�����ID�����Ǹ�ID�Ĵ��ͣ�
	MapLogicEventHandle_CloseDoor = 4, //���� value1 �����
	MapLogicEventHandle_SpanDialogueNpc = 5, //ˢ�Ի�NPC value1 NPC����, value2 λ��
	MapLogicEventHandle_Dialogue = 6, //ֱ�ӶԻ� value1 �Ի���Id
	MapLogicEventHandle_GenPortal = 7, //ˢ�����ţ�value1 chartype��value2 λ�ã�value3 �¸�����resid
	MapLogicEventHandle_OpenDoor = 8, //���� value1 �����
	MapLogicEventHandle_StageProgressNotify = 9, //value1 �������,value2 ����״̬��0 ��ʼ��1 �м䣬2 ������
	MapLogicEventHandle_MsgNotify = 10, //value1 ��Ϣ�ţ�value2 0 ��ǰ���� 1 ȫ����value3 Ƶ��
	MapLogicEventHandle_ClearNPC = 11, //value1 ��������(0 ���йְ��������ͽ���,����һ���Ƴ�����1�ض���Ӫ��2�ض�����),value2 ����������Ͷ�Ӧ��ֵ��value3 ����������Ͷ�Ӧ��ֵ��value4 ����������Ͷ�Ӧ��ֵ��value5 ����������Ͷ�Ӧ��ֵ
	MapLogicEventHandle_SetTransPoint = 12, //value1 (0 �����ͣ�1 ������ͣ�2 ָ�����͵�),value2 x,value3 y
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
	int32_t m_StatCatagory;//�˺�ͳ������
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

	//����
	bool CheckTarget(void);
	bool SpanTarget(TargetEntry& target);
	LC_ServerNPC* GetTarget();
	void ClearTarget();
	void __sendTargetHp();

	//����
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
	GameLogic::LC_IDSet m_eliteset;//ˢ��Ӣ�ֵ�����
	MapLogic_NormalPlayState m_State;
	//��������
	int32_t m_StageProgress;
	int32_t m_StageProgressState;
	int32_t m_SaveBatchIndex;
	bool m_Pass;
	float m_TargetTime;
	bool m_TargetRaid;
	int m_TargetOrg;
	//��;����
	mem::set<int> m_AllSettlePlayer;
	bool m_NeedSettle;
	LC_ServerGuildInfo*	m_pGuildInfo;

	int64_t  m_UserData;
};
#endif