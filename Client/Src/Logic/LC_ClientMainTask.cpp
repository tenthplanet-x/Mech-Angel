#include "ClientPCH.h"
#include "LC_ClientMainTask.h"
#include "LC_ClientTask.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace Utility;
using namespace Script;

//------------------------------------------------------------------
LC_ClientMainTask::LC_ClientMainTask()
: LC_MainTask()
{

}
//------------------------------------------------------------------
LC_ClientMainTask::~LC_ClientMainTask()
{
}
//------------------------------------------------------------------
bool LC_ClientMainTask::Init(LS_LuaState* pkLuaState,const CSVFile::CF_MainTaskList::DataEntry* pkData)
{
	m_kMainTaskDescriptionEntry.SetMainTaskID(pkData->_iID);
	m_kMainTaskDescriptionEntry.SetMainTaskName(pkData->_strName);
	m_kMainTaskDescriptionEntry.SetMainTaskDescription(pkData->_strDescription);
	m_kMainTaskDescriptionEntry.SetMainTaskFinishDescription(pkData->_strFinishDescription);
	return true;
}
//------------------------------------------------------------------
bool LC_ClientMainTask::UnInit()
{
	return true;
}