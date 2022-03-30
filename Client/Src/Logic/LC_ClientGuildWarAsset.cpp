#include "ClientPCH.h"
#include "LC_ClientGuildWarAsset.h"
#include "LC_GuildWarAssert.h"

namespace GameLogic
{

	LC_ClientGuildWarAsset::LC_ClientGuildWarAsset(void)
		:m_iTargetPosX(0)
		,m_iTargetPosY(0)
		,m_iDirectionFxID(0)
		,m_eType(0)
		,m_iPowerPlayer(0)
	{
	}

	LC_ClientGuildWarAsset::~LC_ClientGuildWarAsset(void)
	{
	}

	void LC_ClientGuildWarAsset::SetTargetPos(int iPosX, int iPosY)
	{
		m_iTargetPosX = iPosX;
		m_iTargetPosY = iPosY;		
	}

	void LC_ClientGuildWarAsset::Reuse()
	{
		LC_GuildWarAssert::Reuse();

		m_eType = WarType_None;
		m_iTargetPosX = 0;
		m_iTargetPosY = 0;
		m_iPowerPlayer = 0;
	}
}