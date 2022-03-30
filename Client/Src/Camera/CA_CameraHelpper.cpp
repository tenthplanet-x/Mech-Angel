#include "ClientPCH.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "CA_CameraHelpper.h"

//////////////////////////////////////////////////////////////////////////

bool CA_CameraHelp::GamePlayer::GetPosition(UT_Vec3Int & pos)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if (pkGamePlayer)
	{
		pos = pkGamePlayer->GetVisibleLocation();
		return true;
	}
	return false;
}

bool CA_CameraHelp::GamePlayer::GetNiPosition(NiPoint3 & pos)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if (pkGamePlayer)
	{
		const UT_Vec3Int & v3i = pkGamePlayer->GetVisibleLocation();
		pos = UT_EngineMath::ConvertVec3IntToNiPoint3(v3i);
		return true;
	}
	return false;
}

bool CA_CameraHelp::GamePlayer::SetAIControl(bool b)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if (pkGamePlayer)
	{
		pkGamePlayer->SetAIControl(b);
		return true;
	}
	return false;
}
