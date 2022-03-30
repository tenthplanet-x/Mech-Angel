#pragma once
#include "MM_BaseObject.h"
#include "MC_MessageChannel.h"
#include "MC_BroadCastMessageSender.h"
#include "MC_UniCastMessageSender.h"

class MC_MessageChannel;

class MC_Message2ClientManager : public Memory::MM_BaseObject
{
public:
	MC_Message2ClientManager(void);
	~MC_Message2ClientManager(void);

	void Init();
	void Fint();
	void Update(float currentTime);
	void AddChannel(int idx);
private:
	MC_MessageChannelPtrVector	m_kChannelPtrVec;

	PlayerHPMPChangeBroadCastMessageSender			m_HPMPBroadCastSender;
	PlayerPVEStateChangeBroadCastMessageSender		m_PVEStateBroadCastSender;
	PlayerStateChangeBroadCastMessageSender			m_stateBroadCastSender;
	PlayerLocationChangeBroadCastMessageSender		m_locationBroadCastSender;
	PlayerAngleChangeBroadCastMessageSender		m_angleBroadCastSender;
	
	PlayerPropertyChangeBroadCastMessageSender		m_propertyBroadCastSender;
	PlayerFastPropertyChangeBroadCastMessageSender	m_propertyFastBroadCastSender;
	PlayerExtPropertyChangeBroadCastMessageSender	m_propertyExtBroadCastSender;
	PlayerControllerPropertyChangeBroadCastMessageSender	m_controllerPropertyBroadCastSender;

	PlayerSocityChangeBroadCastMessageSender		m_societyBroadCastSender;
	PlayerEquipChangeBroadCastMessageSender			m_equipBroadCastSender;
	PlayerSkillStateChangeBroadCastMessageSender	m_skillStateBroadCastSender;
	PlayerSubordinateChangeBroadCastMessageSender	m_subordinateBroadCastSender;

	PlayerEnterOrLeaveUniCastMessageSender			m_playerEnterOrLeaveUniCastSender;
	PlayerSyncAllPlayerMessageSender				m_syncAllplayerUniCastSender;

	NpcEnterOrLeaveUniCastMessageSender				m_npcEnterOrLeaveUniCastSender;
	PlayerSyncAllNPCMessageSender					m_syncAllNpcUniCastSender;
	NPCPropertyChangeBroadCastMessageSender			m_npcPropertyBroadCastMessageSender;
	NPCLocationChangeBroadCastMessageSender			m_npcLocationBroadCastMessageSender;
	NPCStateChangeBroadCastMessageSender			m_npcStateBroadCastMessageSender;
	NPCAngleChangeBroadCastMessageSender			m_npcAngleBroadCastMessageSender;
	NPCSkillStateChangeBroadCastMessageSender		m_npcSkillStateBroadCastMessageSender;
	NPCInteractionChangeBroadCastMessageSender		m_npcInteractionBroadCastMessageSender;

};
