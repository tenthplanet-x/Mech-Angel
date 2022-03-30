#include "MC_Message2ClientManager.h"

#include "UT_GameServerProfile.h"

float channelUpdates[] = {0.1f, 0.5f, 0.8f, 1.3f, 2.1f, 3.4f, 5.5f};
enum {
	MC_CHANNEL_IMM = 0,
	MC_CHANNEL_FAST,
	//MC_CHANNEL_MEDIUM,
	//MC_CHANNEL_SLOW,
	//MC_CHANNEL_LAZY,
	//MC_CHANNEL_DULL,
	MC_CHANNEL_MAX,
};

MC_Message2ClientManager::MC_Message2ClientManager(void)
{
}

MC_Message2ClientManager::~MC_Message2ClientManager(void)
{
	
}

void MC_Message2ClientManager::Init()
{
 	size_t _sz = sizeof(channelUpdates)/sizeof(float);
	_sz = _sz > MC_CHANNEL_MAX ? MC_CHANNEL_MAX : _sz;
 	m_kChannelPtrVec.resize(_sz, NULL);
	for (size_t i=0; i<_sz; ++i)
	{
		m_kChannelPtrVec[i] = T_NEW_D MC_MessageChannel(channelUpdates[i]);
		AddChannel((int)i);
	}
}

void MC_Message2ClientManager::Fint()
{
	MC_MessageChannelPtrVector::iterator it = m_kChannelPtrVec.begin();
	for (; it!=m_kChannelPtrVec.end(); ++it)
	{
		T_SAFE_DELETE(*it);
	}
	m_kChannelPtrVec.clear();
}

void MC_Message2ClientManager::Update( float currentTime )
{
	char buf[255] = {0};
	MC_MessageChannelPtrVector::iterator it = m_kChannelPtrVec.begin();
	for (; it!=m_kChannelPtrVec.end(); ++it)
	{
		MC_MessageChannel* pkChannel = *it;
		if (NULL==pkChannel)
		{
			continue;
		}
		//sprintf(buf, "MC_Message2ClientManager Channel[%f] Update", pkChannel->Interval());
		//UT_SERVER_PROFILE(kProfile, buf, 0.003f);
		pkChannel->Update(currentTime);
	}
}

void MC_Message2ClientManager::AddChannel(int idx)
{
	if (idx<0 || idx >= (int)m_kChannelPtrVec.size())
	{
		return;
	}

	MC_MessageChannel* channel = m_kChannelPtrVec[idx];
	if(channel == NULL)
	{
		return;
	}

	switch(idx)
	{
	case MC_CHANNEL_IMM: //0.3
		{
			channel->AddMessageSender(&m_locationBroadCastSender);
			channel->AddMessageSender(&m_angleBroadCastSender);
			channel->AddMessageSender(&m_npcLocationBroadCastMessageSender);
			channel->AddMessageSender(&m_npcStateBroadCastMessageSender);
			channel->AddMessageSender(&m_npcAngleBroadCastMessageSender);
			channel->AddMessageSender(&m_npcEnterOrLeaveUniCastSender);
		}
		break;
	case MC_CHANNEL_FAST: //0.5
		{
			channel->AddMessageSender(&m_HPMPBroadCastSender);
	//	}
	//	break;
	//case MC_CHANNEL_MEDIUM: //0.8
	//	{
			channel->AddMessageSender(&m_propertyBroadCastSender);
			channel->AddMessageSender(&m_controllerPropertyBroadCastSender);
			channel->AddMessageSender(&m_propertyFastBroadCastSender);
			channel->AddMessageSender(&m_propertyExtBroadCastSender);
			channel->AddMessageSender(&m_npcPropertyBroadCastMessageSender);
			//uni
			channel->AddMessageSender(&m_playerEnterOrLeaveUniCastSender);
			//channel->AddMessageSender(&m_npcEnterOrLeaveUniCastSender);
	//	}
	//	break;
	//case MC_CHANNEL_SLOW: //1.3
	//	{
			channel->AddMessageSender(&m_PVEStateBroadCastSender);
			channel->AddMessageSender(&m_stateBroadCastSender);
			channel->AddMessageSender(&m_npcInteractionBroadCastMessageSender);
			channel->AddMessageSender(&m_equipBroadCastSender);
			channel->AddMessageSender(&m_skillStateBroadCastSender);
			channel->AddMessageSender(&m_npcSkillStateBroadCastMessageSender);
	//	}
	//	break;
	//case MC_CHANNEL_LAZY: //2.1
	//	{
			channel->AddMessageSender(&m_societyBroadCastSender);
			channel->AddMessageSender(&m_subordinateBroadCastSender);	

			//uni
			channel->AddMessageSender(&m_syncAllplayerUniCastSender);
	//	}
	//	break;
	//case MC_CHANNEL_DULL: //3.4
	//	{

		}
		break;
	default:
		break;
	}
}
