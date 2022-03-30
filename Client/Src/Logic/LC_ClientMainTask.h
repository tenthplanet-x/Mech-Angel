#ifndef _LC_CLIENT_MAINTASK_H
#define _LC_CLIENT_MAINTASK_H

class LC_ClientMainTask : public GameLogic::LC_MainTask
{
public:
	LC_ClientMainTask();
	virtual ~LC_ClientMainTask();

	virtual bool    Init(Utility::LS_LuaState* pkLuaState,const CSVFile::CF_MainTaskList::DataEntry* pkData);
	virtual bool    UnInit();

};

#endif