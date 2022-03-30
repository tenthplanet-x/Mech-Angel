#ifndef _LC_ACHIEVEMENTMANAGER_H
#define _LC_ACHIEVEMENTMANAGER_H


class LC_AchievementManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_AchievementManager>
{
	typedef mem::map<int32_t,const CSVFile::CF_AchievementList::DataEntry*> AchievementListMap;
public:
	LC_AchievementManager();
	~LC_AchievementManager();

	DECL_RTTI_NAME(LC_AchievementManager);

	void	Init();
	void	ResetData();
	StringType GetTaskTraceAchievement(GameLogic::LC_PlayerBase* pkPlayer);
	int64_t		GetItemCondition(int iID);
	long		GetLinkID(int iID);
	int         GetPreAchievementID(int iID);
	int         GetPreTaskID(int iID);
	StringType		GetItemReward(int iID);
	int			GetAchievePoint(int iID);
	StringType	GetAchieveName(int iID);
	int			GetAchieveCharLevel(int iID);
	StringType  GetAchievedetail(int iID);
	int         GetSubClass(int iID);
	int         GetAchievementTypeID(int iID);
	int32_t GetAchieveProgress(GameLogic::LC_PlayerBase* pkPlayer, int iID);
	bool IsFinishedAchieve(GameLogic::LC_PlayerBase* pkPlayer, int iID);
	StringType GetAchievePointInfo();
	StringType GetAchieveSevenDaysTotalReward(GameLogic::LC_PlayerBase* pkPlayer);

private:
	mem::map<int32_t,AchievementListMap> AchievementIdMap;//_iAchievementType==2 <_iSubclass,<_lID,entry>> AchievementList表 这个容器只保存任务追踪类型的成就
	
};
#endif