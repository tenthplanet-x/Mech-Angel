#include "MC_BroadCastMessageSender.h"

bool PlayerFastPropertyChangeBroadCastMessageSender::IsDataChange(LC_ServerPlayer* playerInFov)
{
	if (playerInFov == NULL)
	{
		return false;
	}
	MC_BroadCastDataDirtyFlag& rDirtyFlag = playerInFov->GetMessageDirtyFlag().GetBroadCastFlag();
	bool bDirty = rDirtyFlag.IsBroadCastFlag(m_nDirtyFlag) || rDirtyFlag.IsMoveSpeedChange();

	LC_ActorAttributeMap* pkAttrMap = playerInFov->GetAttributeMap();
	if(pkAttrMap != NULL)
	{
		bDirty = bDirty || pkAttrMap->IsAttributeValueDirty(E_DirtyGroup_Property);
	}
	return bDirty;
}

void PlayerFastPropertyChangeBroadCastMessageSender::ClearDataChange(LC_ServerPlayer* playerInFov)
{
	if (playerInFov == NULL)
	{
		return;
	}
	MC_BroadCastDataDirtyFlag& rDirtyFlag = playerInFov->GetMessageDirtyFlag().GetBroadCastFlag();
	rDirtyFlag.ClearBroadCastFlag(m_nDirtyFlag);
	rDirtyFlag.ClearSpeed();

	LC_ActorAttributeMap* pkAttrMap = playerInFov->GetAttributeMap();
	if(pkAttrMap != NULL)
	{
		pkAttrMap->ClearAttributeValueDirty(E_DirtyGroup_Property);
	}
}