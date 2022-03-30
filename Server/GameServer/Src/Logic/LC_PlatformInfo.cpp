#include "LC_PlatformInfo.h"

CPlatformInfo::CPlatformInfo() : m_nPlatformType(0), m_nLoginPlatformPage(0)
{

}

CPlatformInfo::~CPlatformInfo()
{
}

void CPlatformInfo::LoadData(const ::char_data_define::PBPlatform_New& data)
{
	SetPlatformType(data.platformtype());
}

void CPlatformInfo::SaveData(::char_data_define::PBPlatform_New* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	pkData->set_platformtype(m_nPlatformType);
}

uint16_t CPlatformInfo::GetPlatformType()
{
	return m_nPlatformType;
}

void CPlatformInfo::SetPlatformType(uint16_t platformType)
{
	if(0 != m_nPlatformType)
	{
		return;
	}

	m_nPlatformType = platformType;
}

uint16_t CPlatformInfo::GetLoginPlatformPage()
{
	return m_nLoginPlatformPage;
}

void CPlatformInfo::SetLoginPlatformPage(uint16_t loginPage)
{
	m_nLoginPlatformPage = loginPage;
}

bool CPlatformInfo::CheckPlatformTypeMatch(uint16_t platformType)
{
	return m_nPlatformType == platformType;
}

bool CPlatformInfo::CheckLoginPlatformPageMatch(uint16_t loginPage)
{
	return m_nLoginPlatformPage == loginPage;
}