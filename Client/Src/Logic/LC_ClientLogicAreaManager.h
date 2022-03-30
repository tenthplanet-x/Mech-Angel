#ifndef _LC_CLIENT_LOGIC_AREA_MANAGER_H
#define _LC_CLIENT_LOGIC_AREA_MANAGER_H

#include "LC_LogicAreaManager.h"

using namespace GameLogic;

/************************************************************************/

class  LC_ClientLogicAreaManager: public GameLogic::LC_LogicAreaManager 
{
public:
    LC_ClientLogicAreaManager();
    ~LC_ClientLogicAreaManager();

    virtual void                   Init( int nAmount );
    bool                           LoadClientLogicAreaData( const long lMapID );
};

/************************************************************************/

class  LC_ClientLogicMapManager:public GameLogic::LC_LogicMapManager 
{
public:
    LC_ClientLogicMapManager();
    ~LC_ClientLogicMapManager();

    static LC_ClientLogicMapManager& GetSingleton(void)
    {
        return *(static_cast<LC_ClientLogicMapManager*>(LC_LogicMapManager::GetSingletonPtr()));
    }

    static LC_ClientLogicMapManager* GetSingletonPtr(void)
    {
        return static_cast<LC_ClientLogicMapManager*>(LC_LogicMapManager::GetSingletonPtr());
    }

	const StringType	&		   GetFilterShowAreaName(const StringType &kAreaName);

private:
	virtual void                   Load();
public:
    virtual bool				   AddLogicMapData( int nMapID );

private:
	mem::map<StringType, StringType> m_kUIAreaNameFilterName;
};

#define g_ClientLogicMapManager LC_ClientLogicMapManager::GetSingletonPtr()

#endif