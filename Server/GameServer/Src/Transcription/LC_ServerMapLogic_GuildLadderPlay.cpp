#include "LC_ServerMapLogic_GuildLadderPlay.h"

LC_ServerMapLogic_GuildLadderPlay::LC_ServerMapLogic_GuildLadderPlay()
{

}

bool LC_ServerMapLogic_GuildLadderPlay::Init(LC_ServerMapLogic_Place* place)
{
	SetPlace(place);
	return true;
}

LC_ServerMapLogic_GuildLadderPlay::~LC_ServerMapLogic_GuildLadderPlay()
{

}

bool LC_ServerMapLogic_GuildLadderPlay::Start()
{
	return true;
}
void LC_ServerMapLogic_GuildLadderPlay::End()
{
	
}

bool LC_ServerMapLogic_GuildLadderPlay::Update(float fCurrentTime, float fDeltaTime)
{
	return true;
}

void LC_ServerMapLogic_GuildLadderPlay::EventNotify(MapLogicEvent& kEvent)
{

}
void LC_ServerMapLogic_GuildLadderPlay::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	IF_A_NA_PLAYER(player)return;
	if (kEvent.Type() == PlayerEvent_Enter)
	{
		player->SetServerViewState(LC_ServerPlayer::SERVER_STATE_INWORLD);
	}
	else if (kEvent.Type() == PlayerEvent_Leave)
	{

	}
	else if (kEvent.Type() == PlayerEvent_Death)
	{

	}
}

bool LC_ServerMapLogic_GuildLadderPlay::CheckEnd()
{
	return false;
}

bool LC_ServerMapLogic_GuildLadderPlay::CheckRun()
{
	return true;
}