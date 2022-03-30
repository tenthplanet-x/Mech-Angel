#ifndef _LC_SERVERMAPLOGIC_PLAYMANAGER_H
#define _LC_SERVERMAPLOGIC_PLAYMANAGER_H

//Íæ·¨¹ÜÀíÆ÷
class LC_ServerMapLogic_Place;
class LC_ServerMapLogic_PlayManager: public  Memory::MM_BaseObject
{
public:
	static ResultType AssemblePlay(LC_ServerMapLogic_Place* theplace);
private:
	static ResultType AssembleNormalRaid(LC_ServerMapLogic_Place* theplace);
	static ResultType AssembleNormal(LC_ServerMapLogic_Place* theplace);
	static ResultType AssembleGuildWar(LC_ServerMapLogic_Place* theplace);
	static ResultType AssembleSoloDule(LC_ServerMapLogic_Place* theplace, GameLogic::SOLO_DULE_TYPE eSoloType);
	static ResultType AssemblePVP(LC_ServerMapLogic_Place* theplace);
	static ResultType AssembleGuildLadderWar(LC_ServerMapLogic_Place* theplace);
	static ResultType AssembleSkyWar(LC_ServerMapLogic_Place* theplace);
	static ResultType AssembleSummitTeamWar(LC_ServerMapLogic_Place* theplace);
	static ResultType AssembleSummitWheelWar(LC_ServerMapLogic_Place* theplace);
};
#endif