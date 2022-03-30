#pragma once

#include "LC_ServerMapLogic_Place.h"

//异种侵袭
class LC_ServerMapLogic_RankPlace : public LC_ServerMapLogic_Place
{
public:
	LC_ServerMapLogic_RankPlace();
	~LC_ServerMapLogic_RankPlace();

	virtual void				Init();
	virtual void				UnInit();
	void							Clear();
	virtual void				SetPlaceState(PlaceLogicState flag);
	virtual void				Settle(int storyID, float countDownTime, bool success=false);

private:
	bool m_bSettle;
};

//异种降临
class LC_ServerMapLogic_GuildStoryMapPlace : public LC_ServerMapLogic_Place
{
public:
	LC_ServerMapLogic_GuildStoryMapPlace();
	~LC_ServerMapLogic_GuildStoryMapPlace();

	virtual void				Init();
	virtual void				UnInit();
	void							Clear();

	virtual void				SetPlaceState(PlaceLogicState flag);
	virtual void				ForceUpdateDamageData(LC_ServerPlayer* pkPlayer, bool syncClient);
	virtual void				SetGuildID(uint64_t guildID);
	virtual uint64_t			GetGuildID();
	virtual attr_value_type GetTotalDamage();

	virtual void				UpdatePlayerDamage(LC_ActorBase* pkPlayer, attr_value_type damage);
	virtual attr_value_type GetPlayerDamage(uint64_t playerID);

	void							UpdateAllDamageData();
protected:
private:
	uint64_t	m_nGuildID;//所属公会id
};