#ifndef _LC_SERVERPLAYER_RECOVERTIMES_H_
#define _LC_SERVERPLAYER_RECOVERTIMES_H_

#include "LC_Define.h"
#include "SystemError.h"
#include "char_data.pb.h"

class LC_ServerPlayer;
class CRecoverTimes
{
public:
	CRecoverTimes(uint32_t revertTime, uint32_t revertMaxTimes, int32_t dropType, int32_t broadCastID);
	~CRecoverTimes();

	void CheckRecover(LC_ServerPlayer* pkOwnerPlayer);
	bool AddTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times, bool checkMax=true);
	bool ReduceTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times=1);
	void BroadCastRecoverTimesMsg(LC_ServerPlayer* pkOwnerPlayer);

	virtual int32_t GetCurrentTimes(LC_ServerPlayer* pkOwnerPlayer) { return 0; }
	virtual bool DoAddTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times) {return false;}
	virtual bool DoReduceTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times=1) {return false;}

protected:
	uint32_t m_nRevertTime;//恢复间隔时间
	uint32_t m_nRevertMaxTimes;//恢复上限
	uint32_t m_nNextRevertTime;//下一次恢复的时间
	uint32_t m_nEndRevertTime;//最后一次恢复的时间
	int32_t m_nType;//类型
	int32_t m_nBroadCastID;//
};

class CDropTimes : public CRecoverTimes
{
public:
	CDropTimes(uint32_t revertTime, uint32_t revertMaxTimes, int32_t dropType, int32_t broadCastID=0);
	~CDropTimes();

	void LoadData(const ::char_data_define::PBRecoverTimesData& data);
	void SaveData(::char_data_define::PBRecoverTimesData* pkData);

	virtual int32_t GetCurrentTimes(LC_ServerPlayer* pkOwnerPlayer);
	virtual bool DoAddTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times);
	virtual bool DoReduceTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times=1);

	bool SyncBossDropInfo(LC_ServerPlayer* pkOwnerPlayer);
private:
	int32_t m_nDropTimes;//掉落次数
};

class CYuanZhengPoint : public CRecoverTimes
{
public:
	CYuanZhengPoint(uint32_t revertTime, uint32_t revertMaxTimes, int32_t dropType, int32_t broadCastID=0);
	~CYuanZhengPoint();

	void LoadData(const ::char_data_define::PBRecoverTimesData& data);
	void SaveData(::char_data_define::PBRecoverTimesData* pkData);

	virtual int32_t GetCurrentTimes(LC_ServerPlayer* pkOwnerPlayer);
	virtual bool DoAddTimes(LC_ServerPlayer* pkOwnerPlayer, int times);
	virtual bool DoReduceTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t times=1);
	bool SyncYuanZhengPointInfo(LC_ServerPlayer* pkOwnerPlayer);
};
#endif