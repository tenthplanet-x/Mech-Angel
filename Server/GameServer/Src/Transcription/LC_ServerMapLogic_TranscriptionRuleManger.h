/******************************************************
Author:				sunny
CreateTime:			2013.9.23   9:21
FileName:			LC_TranscriptionRuleManger
FileExt:			.h
******************************************************/

#ifndef _LC_TRANSCRIPTIONRULE_H
#define _LC_TRANSCRIPTIONRULE_H
using namespace CSVFile;
class LC_ServerMapLogic_TranscriptionRuleManger:public  Memory::MM_BaseObject, public UT_Singleton<LC_ServerMapLogic_TranscriptionRuleManger>
{
public:

	LC_ServerMapLogic_TranscriptionRuleManger();
	virtual ~LC_ServerMapLogic_TranscriptionRuleManger();
	CF_TranscriptionRule::DataEntry* GetRule(int resId);
	ResultType CheckEnter(GameLogic::LC_PlayerBase* pkPlayer,CF_TranscriptionRule::DataEntry* rule);
	ResultType CheckEnter(object_id_type playerID,int level,CF_TranscriptionRule::DataEntry* rule);
	ResultType CheckLeave(GameLogic::LC_PlayerBase* pkPlayer,CF_TranscriptionRule::DataEntry* rule);

};
#endif