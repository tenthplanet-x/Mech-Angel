#include "ClientPCH.h"
#include "UI_LogicClass.h"
#include "WT_RootWidget.h"
#include "WT_Widget.h"

using namespace GUISys;
UI_LogicClass::UI_LogicClass():m_sWidgetPath(""),m_bIsOpen(false),m_pWidget(NULL)
{
	 
}
UI_LogicClass::~UI_LogicClass()
{

}
bool UI_LogicClass::AttachWidget( const char * WidgetPath )
{
	WT_Widget *pWidget =NULL;
	pWidget=GUISys::CUIRootWin::GetSingletonPtr()->FindRootWidgetByID(WidgetPath);
	if(pWidget)
	{
		m_pWidget = pWidget;
		return true;
	}
	GfxWriteDefaultLog(LOG_ERROR_TAG,"AttachWidget error in UILogic" );
	return false;
}

StringType UI_LogicClass::GetWidgetPath()
{
	return m_sWidgetPath;
}

void UI_LogicClass::SetWidgetPath( StringType &sPath )
{
	if(sPath!="")
		m_sWidgetPath = sPath;
}
bool UI_LogicClass::Open()
{
	if(!m_pWidget)
	{
		AttachWidget(m_sWidgetPath.c_str());
	}
	if(m_pWidget)
	{
		m_pWidget->ShowWidget(true);
	}
	m_bIsOpen = true;
	return true;
}

bool UI_LogicClass::Close()
{
	if(!m_pWidget)
	{
		AttachWidget(m_sWidgetPath.c_str());
	}
	if(m_pWidget)
	{
		m_pWidget->ShowWidget(false);
	}
	m_bIsOpen = false;
	return true;
}
bool UI_LogicClass::IsOpen()
{
	if(!m_pWidget)
	{
		AttachWidget(m_sWidgetPath.c_str());
	}
	if (m_pWidget)
	{
		return m_pWidget->IsVisible() ;
	}
	return m_bIsOpen;
}