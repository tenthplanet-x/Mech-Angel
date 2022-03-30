#pragma once

#include "UT_ComplexData.h"
#include "CF_FacebookReward.h"
#include "char_data.pb.h"

class LC_ServerPlayer;
class LC_FacebookRecord
{
public:
	LC_FacebookRecord() {}
	void DailyReset() { m_kDrawCountDaily.clear(); }

	void Load(const char_data_define::char_detail_info& info);
	void Save(char_data_define::char_detail_info& info);
	int32_t GetDrawCount(int32_t iType, int32_t iID, bool bDaily);
	void DrawReward(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iID, int32_t iCount = 1);
	bool CheckCanDraw(int32_t iType, int32_t iID);

private:
	mem::map<int32_t, mem::map<int32_t, int32_t> > m_kDrawCount;
	mem::map<int32_t, mem::map<int32_t, int32_t> > m_kDrawCountDaily;
};

class LC_FBRewardEntry : public Memory::MM_BaseObject
{
public:
	LC_FBRewardEntry() : _iType(0), _iID(0), _iCount(0), _bDaily(false) {}

	bool Init(CSVFile::CF_FacebookReward::DataEntry* pkEntry);

	int32_t _iType;
	int32_t _iID;
	int32_t _iCount;
	bool _bDaily;
	StringType _sTitle;
	StringType _sContent;
	Utility::UT_SIMDataList _kRewards;
};

class LC_FacebookRewardManager : public Memory::MM_BaseObject
	, public Utility::UT_Singleton<LC_FacebookRewardManager>
{
public:
	LC_FacebookRewardManager() {}
	~LC_FacebookRewardManager() {};

	void Init();
	void Unit();

	LC_FBRewardEntry* GetFBRewardEntry(int32_t iType, int32_t iID)
	{
		mem::map<int32_t, mem::map<int32_t, LC_FBRewardEntry> >::iterator it =
			m_kFBRewardEntryMap.find(iType);
		if (it == m_kFBRewardEntryMap.end())
		{
			return NULL;
		}
		mem::map<int32_t, LC_FBRewardEntry>::iterator sit = it->second.find(iID);
		return (sit != it->second.end()) ? &sit->second : NULL;
	}

protected:
	mem::map<int32_t, mem::map<int32_t, LC_FBRewardEntry> > m_kFBRewardEntryMap;
};
