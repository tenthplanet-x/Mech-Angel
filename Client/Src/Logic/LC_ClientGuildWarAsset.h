#ifndef _LC_CLIENT_GUILD_WAR_ASSET_H
#define _LC_CLIENT_GUILD_WAR_ASSET_H

namespace GameLogic
{
	class LC_ClientGuildWarAsset : public LC_GuildWarAssert
	{
	public:
		LC_ClientGuildWarAsset(void);
		~LC_ClientGuildWarAsset(void);

		virtual void Reuse();

		int GetType(void){return m_eType;}
		void SetType(int iType){m_eType = iType;}
		int GetTargetPosX(void){return m_iTargetPosX;}
		int GetTargetPosY(void){return m_iTargetPosY;}
		void SetTargetPos(int iPosX, int iPosY);
		int GetDirectionFxID(void){return m_iDirectionFxID;}
		void SetDirectionFxID(int iFxID){m_iDirectionFxID = iFxID;}
		void SetPowerPlayer(unique_id_type iUserID){m_iPowerPlayer = iUserID;}
		unique_id_type GetPowerPlayer()const{return m_iPowerPlayer;}

	private:
		int m_iTargetPosX;
		int m_iTargetPosY;
		int m_iDirectionFxID;
		int m_eType;
		unique_id_type	m_iPowerPlayer;
	};
}

#endif