#ifndef _LC_PLATFORMINFO_H_
#define _LC_PLATFORMINFO_H_

#include "LC_Define.h"
#include "char_data.pb.h"

class CPlatformInfo
{
public:
	CPlatformInfo();
	~CPlatformInfo();

	void LoadData(const ::char_data_define::PBPlatform_New& data);
	void SaveData(::char_data_define::PBPlatform_New* pkData);

	uint16_t GetPlatformType();
	void SetPlatformType(uint16_t platformType);

	uint16_t GetLoginPlatformPage();
	void SetLoginPlatformPage(uint16_t loginPage);

	bool CheckPlatformTypeMatch(uint16_t platformType);
	bool CheckLoginPlatformPageMatch(uint16_t loginPage);
protected:
private:
	uint16_t m_nPlatformType;//登录平台
	uint16_t m_nLoginPlatformPage;//登录平台的方式
};

#endif