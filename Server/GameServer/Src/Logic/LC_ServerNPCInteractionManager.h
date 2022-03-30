#pragma once

#include "LC_NPCInteractionManager.h"

class LC_ServerNPC;
class LC_ServerPlayer;
class LC_ServerNPCInteractionManager : public GameLogic::LC_NPCInteractionManager
{
public:
	LC_ServerNPCInteractionManager();
	virtual ~LC_ServerNPCInteractionManager();

	ResultType	NPCInteraction(LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		CheckInteractionState(LC_ServerNPC* pkNPC);
	GameLogic::ResultInfo& GetResultInfo(){return m_kResultInfo;};
private:
	void		_handleOPList(GameLogic::InteractionOPList* pkOPList,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_unhandleOPList(GameLogic::InteractionOPList* pkOPList,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_changeOPList(GameLogic::InteractionOPList* pkOPList,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	ResultType	_checkOPList(GameLogic::InteractionOPList* pkOPList,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_handleOPData(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_unhandleOPData(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_changeOPData(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	ResultType	_checkOPData(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_executeDeleteItemOP(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	ResultType	_checkDeleteItemOP(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_executeAddItemOP(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_executeShowMessageOP(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_executeSpawnNPCOP(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);
	void		_executeAddStateOP(const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer);

	GameLogic::ResultInfo	m_kResultInfo;
};