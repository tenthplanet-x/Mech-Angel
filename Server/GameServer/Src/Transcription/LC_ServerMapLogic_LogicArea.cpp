#include "LC_ServerMapLogic_LogicArea.h"
#include "ajson.hpp"
#include "UT_DirConfig.h"
#include "UT_Log.h"
#include "UT_Func.h"

using namespace boost::ajson;

AJSON_CODE(LogicAreaUnit,(LogicAreaId)(BuffId)(Multiple))
AJSON_CODE(LogicArea_Config,(LogicArea))
//////////////////////////////////////////////////////////////////////////////
bool LogicArea_Config::Init(const StringType& configFileName)
{
	StringType pkConfigFileName = TRANSCRIPTION_CONFIG_DIR + configFileName;
	pkConfigFileName = Utility::getFilePath(pkConfigFileName);
	StringType errstr;
	bool ret= load_from_file((*this),(char *)pkConfigFileName.c_str(),errstr);
	if(!ret)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE,LOG_SWITCH_SYSTEM,"load jsonfile error!%s,%s",configFileName.c_str(),errstr.c_str());
	}
	AreaMap.clear();
	for(size_t i = 0; i < LogicArea.size(); i++)
	{
		LogicAreaUnit* PUnit = &(LogicArea[i]);
		AreaMap[PUnit->LogicAreaId] = PUnit;
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
bool LC_ServerMapLogic_LogicArea::Init(LC_ServerMapLogic_Place* place, LogicArea_Config* config)
{
	if (place == NULL || config == NULL)
	{
		return false;
	}
	bool ret = LC_ServerMapLogic_PlayBase::Init(place, PlayType_LogicArea);
	if (!ret)
	{
		return false;
	}
	m_Config = config;
	return true;
}

void LC_ServerMapLogic_LogicArea::UnInit()
{
	End();
	LC_ServerMapLogic_PlayBase::UnInit();
}

bool LC_ServerMapLogic_LogicArea::Start()
{
	return true;
}

void LC_ServerMapLogic_LogicArea::End()
{
	
}

bool LC_ServerMapLogic_LogicArea::Update(float fCurrentTime, float fDeltaTime)
{
	return true;
}

void LC_ServerMapLogic_LogicArea::EventNotify(MapLogicEvent& kEvent)
{

}

void LC_ServerMapLogic_LogicArea::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	if (NULL == player || NULL == m_Config || NULL == GetPlace())
	{
		return;
	}

	switch(kEvent.Type())
	{
	case PlayerEvent_Enter:
		{
			LogicAreaMapType::iterator it = m_Config->AreaMap.find(player->GetCurrentLogicAreaID());
			if (it != m_Config->AreaMap.end())
			{
				LogicAreaUnit* pLogicUnit = it->second;
				if(pLogicUnit != NULL)
				{
					player->ActiveSkillState(pLogicUnit->BuffId, 0, 0, (float)pLogicUnit->Multiple);
				}
			}
		}
		break;
	case PlayerEvent_Leave:
		{
			LogicAreaMapType::iterator beg = m_Config->AreaMap.begin();
			for (; beg != m_Config->AreaMap.end(); ++beg)
			{
				LogicAreaUnit* pLogicUnit = beg->second;
				if(pLogicUnit != NULL)
				{
					player->DeactiveSkillState(pLogicUnit->BuffId);
				}
			}
		}
		break;
	case PlayerEvent_ChangeLogicArea:
		{
			int prearea = kEvent.Param(0);
			int curarea = kEvent.Param(1);
			{
				LogicAreaMapType::iterator it = m_Config->AreaMap.find(prearea);
				if (it != m_Config->AreaMap.end())
				{
					LogicAreaUnit* pLogicUnit = it->second;
					if(pLogicUnit != NULL)
					{
						int buddyID = player->GetBuddyID();
						player->GetStateMap()->RemoveSkillStateInStateMap(pLogicUnit->BuffId);
					}
				}
			}
			{
				LogicAreaMapType::iterator it = m_Config->AreaMap.find(curarea);
				if (it != m_Config->AreaMap.end())
				{
					LogicAreaUnit* pLogicUnit = it->second;
					if(pLogicUnit != NULL)
					{
						player->GetStateMap()->AddSkillStateToStateMap(pLogicUnit->BuffId, 0, 0, (float)pLogicUnit->Multiple);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}
