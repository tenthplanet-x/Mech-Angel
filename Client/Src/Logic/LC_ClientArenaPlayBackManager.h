#pragma once
//竞技场回放管理

#include "MG_MessageHandlerInterface.h"
#include "LC_GameEvent.h"
class NW_Game2ClientSession;
namespace GameLogic 
{
	class LC_ClientArenaPlayBackManager : public Utility::UT_Singleton<LC_ClientArenaPlayBackManager>,
		public MG_MessageHandlerInterface,
		public Memory::MM_BaseObject
	{
	public:
		LC_ClientArenaPlayBackManager();
		~LC_ClientArenaPlayBackManager();
		DECL_RTTI_NAME( LC_ClientArenaPlayBackManager )


		void					Init();
		void					UnInit();
		void					ReUse();
		void					DestoryAll();
		bool					CheckFileValid(int64_t id);
		void					Packet(uint16_t iType, char* pMsg, uint16_t wSize, float deltime);
		void					FillRuneLevelInfoPB(LC_ClientGamePlayer* player, ::google::protobuf::RepeatedField< ::google::protobuf::int32 > & item_info, ::google::protobuf::RepeatedField< ::google::protobuf::int32 > & level_info);

		void					SetUniqueId(int64_t id) { m_nUniqueId = id; };
		bool					LoadStream();
		bool					LoadFile(unique_id_type uiID);
		bool					NewLoadFile(int userId,int64_t nFightId);
		void					SaveFile(int64_t uID);
		void					Begin();

		void					Update(float fCurrentTime,float fDeltaTime); 
		virtual bool			RecvMessage(MG_MessageBase& rkMessage);

		object_id_type			GetRealID(object_id_type id);
		object_id_type			GetTemID(object_id_type id);

		object_id_type			_ObjectIDTransform(object_id_type id);
		void					SetPlayerMap(LC_ClientNetPlayer* player, bool bLeft);
		void					ErasePlayer(object_id_type objID);
		bool					CheckPlayerSideByOwnerID(int ownerid);
		bool					CheckPlayerSideByID(object_id_type objID);
		bool					IsPlayerInList(object_id_type id);

		int						GetPlayerCount(bool bLeft);
		LC_ClientNetPlayer*		GetNetPlayerBySideAndIndex(bool bLeft, int index);

		std::stringstream&		GetContentStream() { return m_sContentStream; }

	private:
		void					_event_TimerTick(MG_MessageBase& rkMessage);
		void					_ParsePacket(int iType, char* pMsg, uint16_t wSize);

		void					_handlerRltGameEvent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
		void					HandlerGameEvent(GameLogic::LC_GameEvent& rGameEvent,StringType& rEventStringParam);
		void					_handlerTranscriptionEvent(GameLogic::LC_GameEvent& rGameEvent);
		void					_handlerSyncGamePlayerAIControlLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
		void					_handlerSCSetActivateBuddyReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

		msg_define::arenaplayback_info				m_playbackInfoMsg;
		int64_t										m_nUniqueId;
		int											m_isign;
		float										m_fBeginTime;
		mem::map<object_id_type, object_id_type>	m_mobjid;
		mem::vector<LC_ClientNetPlayer*>			m_mPlayerLeft;
		mem::vector<LC_ClientNetPlayer*>			m_mPlayerRight;

		std::stringstream							m_sContentStream;		//缓存未序列化字符串

		

	};
};