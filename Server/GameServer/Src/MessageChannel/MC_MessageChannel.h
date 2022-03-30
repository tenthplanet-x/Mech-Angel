#pragma once
#include "MM_BaseObject.h"
#include <vector>
#include "MC_PlayerFovVector.h"
#include "LC_ServerPlayer.h"

class MC_IUniCastMessageSender;
class MC_IGridPacketPlayerDataMessageSender;
class MC_IAllPacketPlayerDataMessageSender;
class MC_IAllPacketNPCDataMessageSender;
class LC_ServerPlayer;


typedef mem::vector<MC_IUniCastMessageSender*> UniCastMessageSenderContainer;
typedef mem::vector<MC_IGridPacketPlayerDataMessageSender*> GridPacketPlayerDataMessageSenderContainer;
typedef mem::vector<MC_IAllPacketPlayerDataMessageSender*> AllPacketPlayerDataMessageSenderContainer;
typedef mem::vector<MC_IAllPacketNPCDataMessageSender*> AllPacketNPCDataMessageSenderContainer;

class MC_MessageChannel : public Memory::MM_BaseObject
{
public:
	MC_MessageChannel(void)
		:m_lastTime(0.0f)
		,m_interval(0.1f)
	{}

	MC_MessageChannel(float interval);

	~MC_MessageChannel(void);

	inline void Init(float interval)
	{
		m_interval = interval;
	}

	inline float Interval(void) const
	{
		return m_interval;
	}

	void AddMessageSender(MC_IUniCastMessageSender* sender);
	void AddMessageSender(MC_IGridPacketPlayerDataMessageSender* sender);
	void AddMessageSender(MC_IAllPacketPlayerDataMessageSender* sender);
	void AddMessageSender(MC_IAllPacketNPCDataMessageSender* sender);

	void Update(float fCurrentTime);
private:
	bool NeedPacketPlayerMessage();
	bool NeedPacketNPCMessage();

	void UnicastMessage(MC_PlayerFovVector& playerVector);
	void BroadCastGridPlayerMessage(MC_PlayerFovVector& playerVector);
	void BroadCastAllPlayerMessage(MC_PlayerFovVector& playerVector);
	void BroadCastAllNPCMessage(MC_PlayerFovVector& playerVector);

private:
	float m_lastTime;
	float m_interval;

	UniCastMessageSenderContainer m_unicastMessages;

	GridPacketPlayerDataMessageSenderContainer m_gridPlayerMessages;

	AllPacketPlayerDataMessageSenderContainer m_allPacketPlayerMessages;

	AllPacketNPCDataMessageSenderContainer m_allPacketNPCMessages;

	MC_PlayerFovVector m_playerVector;
};
typedef mem::vector<MC_MessageChannel*> MC_MessageChannelPtrVector;