#ifndef _LC_CLIENT_ACTOR_OPERATION_MANAGER_H
#define _LC_CLIENT_ACTOR_OPERATION_MANAGER_H

class LC_ClientActorOperationManager : public GameLogic::LC_ActorOperationManagerBase
{
public:
	LC_ClientActorOperationManager();
	~LC_ClientActorOperationManager();

protected:
	//virtual ResultType _setTextSandCompositionOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeTextSandCompositionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetTextSandCompositionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//virtual ResultType _setAddTextToEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeAddTextToEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetAddTextToEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//virtual ResultType _setRemoveTextFromEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeRemoveTextFromEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetRemoveTextFromEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//virtual ResultType _setSandTextInEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeSandTextInEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetSandTextInEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//virtual ResultType _setAppendAttributeOnEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeAppendAttributeOnEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetAppendAttributeOnEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);
	
	virtual ResultType _setDecompositionEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);
	virtual ResultType _executeDecompositionEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _resetDecompositionEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//virtual ResultType _setProduceHobbyEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeProduceHobbyEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetProduceHobbyEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);
	
	//virtual ResultType _setHobbyRecastEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);	
	//virtual ResultType _executeHobbyRecastEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetHobbyRecastEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);
	
	//virtual ResultType _setHobbyEnhanceEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);	
	//virtual ResultType _executeHobbyEnhanceEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetHobbyEnhanceEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);


	//爱好镶嵌
	//virtual ResultType _setHobbyEmbedSkillOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeHobbyEmbedSkillOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetHobbyEmbedSkillOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);


	//好友召唤
	virtual ResultType _checkSummorFriendOperationExecute(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _setSummorFriendOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);
	virtual ResultType _executeSummorFriendOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _resetSummorFriendOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);


	//爱好反镶嵌
	//virtual ResultType _setHobbyUnEmbedSkillOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal);
	//virtual ResultType _executeHobbyUnEmbedSkillOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetHobbyUnEmbedSkillOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	// 时装合成
	//virtual ResultType _executeBuildFashionOperation(GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _setBuildFashionOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);
	//virtual ResultType _resetBuildFashionOperation(GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//挂机
	virtual ResultType _setHookOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);	
	virtual ResultType _executeHookOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _resetHookOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);


	//护身符属性重铸、锁定、解锁
	//virtual ResultType _setAmuletOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParamIndex,long lValue);
	//virtual ResultType _executeAmuletOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	//virtual ResultType _resetAmuletOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//真合成
	virtual ResultType _setItemCompositionOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);
	virtual ResultType _executeItemCompositionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _resetItemCompositionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//生产
	virtual ResultType _setProductionOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);
	virtual ResultType _executeProductionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _resetProductionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//令牌合成
	virtual ResultType _executeTokenCompositeOperation(GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _setTokenCompositeOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);
	virtual ResultType _resetTokenCompositeOperation(GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);

	//兑换
	virtual ResultType _setExchangeOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);
	virtual ResultType _executeExchangeOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType _resetExchangeOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters);
	virtual ResultType _checkExchangeOperationExecute( GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer );
	virtual ResultType _checkSetExchangeOperationParameter( GameLogic::LC_ActorOperationEntry* pkOperation, GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal);

};

#endif