#ifndef PLAYERSTATEACTION_H_YTUT46464
#define PLAYERSTATEACTION_H_YTUT46464

#include "SM_StateMachine.h"

StateMachine::StateProcessType ProcessPlayerStateTestFight(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
StateMachine::StateProcessType ProcessPlayerStateTestFight_Orig(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
StateMachine::StateProcessType ProcessPlayerStateTestFight_AsBuddy(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
StateMachine::StateProcessType ProcessPlayerStateTestFollow(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime , int Distance);

StateMachine::StateProcessType ProcessPlayerStateTestTalk(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);

StateMachine::StateProcessType ProcessPlayerStateTestWander(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);

void ProcessPlayerStatePositiveActive(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);

void SetPlayerLastIdleTime(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime);

#endif
