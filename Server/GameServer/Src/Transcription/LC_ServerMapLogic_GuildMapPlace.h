#pragma once

#include "LC_ServerMapLogic_Place.h"

//公会秘境
class LC_ServerMapLogic_GuildMapPlace : public LC_ServerMapLogic_Place
{
public:
	LC_ServerMapLogic_GuildMapPlace();
	~LC_ServerMapLogic_GuildMapPlace();

	virtual void				Init();
	virtual void				UnInit();
	void							Clear();

	virtual int					GetLeftTime();
	virtual void				Update(float fCurrentTime, float fDeltaTime);

	virtual void				ForceUpdateDamageData(LC_ServerPlayer* pkPlayer, bool syncClient);
	virtual void				ForceUpdateAllPlayerDamageData();
	virtual void				SettleGuildMapBoss(int bossID);

	virtual void				SetGuildID(uint64_t guildID);
	virtual uint64_t			GetGuildID();

	virtual void				SetPlaceInvaildTime(int32_t time);
	virtual int32_t			GetPlaceInvaildTime();

private:
	int32_t		m_nInvaildTime;//副本结束时间
	uint64_t	m_nGuildID;//所属公会id
};