#ifndef _LC_CLIENT_LOGIC_MANAGER_H
#define _LC_CLIENT_LOGIC_MANAGER_H
//�ͻ����߼�������
#include "LC_LogicManagerBase.h"
#include "MG_MessageHandlerInterface.h"
#include "UT_Timer.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_ClientShotActivity.h"
#include "LC_ClientRumorManager.h"
#include "LC_ClientOpenAbilityManager.h"
#include "LC_ClientSoulRefineAsset.h"
using namespace GameLogic;
using namespace GameStory;

class LC_ClientGamePlayer;
class LC_ClientNetPlayerManager;
class LC_ClientLocalNPCManager;
class LC_ClientTranscriptionInfoManager;
class LC_ClientGameStoryManager;
namespace GameStory
{
	class GameStoryCommonManager;
}
class LC_ClientNPC;
class Utility::UT_TimerManager;
namespace Skill
{
	class SK_ClientSkillEventManager;
}

class LC_ClientLogicManager : public GameLogic::LC_LogicManagerBase,
							  public MG_MessageHandlerInterface
{
public:
	LC_ClientLogicManager();
	virtual ~LC_ClientLogicManager();

	virtual void Init();
	virtual void UnInit();
	
	virtual bool RecvMessage(MG_MessageBase& rkMessage); 
	
	void					SetOperationPlayerID(object_id_type lID);
	object_id_type			GetOperationPlayerID(){return m_lOperationPlayerID;};

	object_id_type			GetInputControlActorID() { return m_lInputControlActorID; }
	void					SetInputControlActorID(object_id_type lID, bool bMsg = true);
	
	const unique_id_impl&   GetOperationCharID() {return m_lOperationCharID;}
	void					SetOperationCharID(const unique_id_impl& CharID){m_lOperationCharID = CharID;}
	LC_ClientGamePlayer*	GetOperationPlayer();

	LC_ClientNetPlayerManager*	GetNetPlayerManager();
   // LC_ClientLocalNPCManager*   GetLocalNPCManager(void) const { return m_pkLocalNpcManager; }
	LC_ClientTranscriptionInfoManager* GetTranscriptionInfoManager(){return m_pkTranscriptionManager;}
	LC_ClientGameStoryManager*		   GetGameStoryManager(){return m_pkGameStoyManager;}
	LC_ClientOpenAbilityManager*		GetClientOpenAbilityManager(){return m_pkClientOpenAbilityManager;}
	LC_ClientRumorManager*			GetClientRumorManager(){return m_pkClientRumorManager;}
	GameStoryCommonManager*			GetGameStoryCommonManager(){return m_pkGameStoryCommonManager;}
	//��������ң��������
	LC_PlayerBase*              FindPlayer(object_id_type lPlayerID);


	/*
	*Gameplayer��NetPlayer���õĽӿڣ�
	*�ͻ��˴�������Player�࣬Ϊ�˱�������ظ���ʱ�����ýӿڷ���LogicManager�У��պ���Ҫ�ع��ⲿ�ִ���
	*/
	//���¿ͻ�����ҵ�ս��״̬
	void UpdatePlayerFightState(LC_PlayerBase* pkPlayer);
	//���¿ͻ�����ҵ���װ�Ҽ�
	void UpdateExtraClothing(LC_PlayerBase* pkPlayer, long lCurrentHeadEquipID, long lCurrentBodyEquipID, long lCurrentFootEquipID);
	int  GetFunctionNPCObjectType(LC_ClientNPC* pkNPC);

	/*
	*�ͻ���GamePlayer��NetPlayer��NPC���ýӿڣ���������ظ�
	*/
	//��������
	void FreezeAnimation(LC_ActorBase* pkActor);
	void UnFreezeAnimation(LC_ActorBase* pkActor);

	//static float m_fBeginRecord;
protected:
	void		_eventHeadNPCHead(MG_MessageBase& rkMessage);
	void		_event_TimerTick(MG_MessageBase& rkMessage);
	void		_event_InputKeyAdd(MG_MessageBase& rkMessage);
	void		_event_InputKey2(MG_MessageBase& rkMessage);
	void		_event_InputKey3(MG_MessageBase& rkMessage);

	object_id_type						m_lOperationPlayerID;
	object_id_type						m_lInputControlActorID;
	unique_id_impl						m_lOperationCharID;
	LC_ClientNetPlayerManager*			m_pkNetPlayerManager;
   // LC_ClientLocalNPCManager*           m_pkLocalNpcManager;
	Skill::SK_ClientSkillEventManager*	m_pkSkillEventManager;

	LC_ClientTranscriptionInfoManager*	m_pkTranscriptionManager;
	LC_ClientGameStoryManager*			m_pkGameStoyManager;
	LC_ClientOpenAbilityManager*		m_pkClientOpenAbilityManager;
	LC_ClientRumorManager*				m_pkClientRumorManager;
	GameStoryCommonManager*				m_pkGameStoryCommonManager;
#ifdef _MAP_VIEWER_BUILD
	int			m_iCreatedViewActorID;			
#endif
};

#define CLIENT_GET_LOGIC_MANAGER				(LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr
#define CLIENT_GET_GAMEPLAYER_MANAGER			(LC_ClientGamePlayerManager*)LC_ClientLogicManager::GetSingletonPtr()->GetGamePlayerManager
#define CLIENT_GET_NPC_MANAGER					(LC_ClientNPCManager*)LC_ClientLogicManager::GetSingletonPtr()->GetNPCManager
//#define CLIENT_GET_LOCAL_NPC_MANAGER            ((LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr())->GetLocalNPCManager
#define CLIENT_GET_LOCAL_NPC_MANAGER            CLIENT_GET_NPC_MANAGER
#define CLIENT_GET_NETPLAYER_MANAGER			((LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr())->GetNetPlayerManager
#define CLIENT_GET_WORLD_MANAGER				(LC_ClientWorldManager*)LC_ClientLogicManager::GetSingletonPtr()->GetWorldManager
#define CLIENT_GET_PLACEHOLDER_MANAGER				(LC_ClientPlaceHolderManager*)LC_ClientLogicManager::GetSingletonPtr()->GetPlaceHolderManager
#define CLIENT_GET_PLAYER_GROUP_MANAGER			(LC_ClientPlayerGroupManager*)LC_ClientLogicManager::GetSingletonPtr()->GetPlayerGroupManager

#define CLIENT_GET_OPENABIBITY_MANAGER			((LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr())->GetClientOpenAbilityManager
#define CLIENT_GET_TRANSCRIPTION_INFO_MANAGER	((LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr())->GetTranscriptionInfoManager
#define CLIENT_GET_GAMESTORY_MANAGER			 ((LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr())->GetGameStoryManager
#define CLIENT_GET_RUMOR_MANAGER				 ((LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr())->GetClientRumorManager
#endif // _LC_CLIENT_LOGIC_MANAGER_H
