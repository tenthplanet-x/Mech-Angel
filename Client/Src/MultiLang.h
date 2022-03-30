
#ifndef _CLIENT_MULTI_LANGUAGE_DEFINE_H
#define _CLIENT_MULTI_LANGUAGE_DEFINE_H 


#define LANGUAGE_MODE_CHS	0	//< 简体中文
#define LANGUAGE_MODE_TW	1	//< 繁体中文
#define LANGUAGE_MODE_VN	2	//< 越南
#define LANGUAGE_MODE_KR	3	//< 韩国


#ifdef _TAIWAN
	#define LANGUAGE_MODE		LANGUAGE_MODE_TW
#else
	#define LANGUAGE_MODE		LANGUAGE_MODE_CHS
#endif


#if LANGUAGE_MODE == LANGUAGE_MODE_TW
	#define CLIENTAPP_FLASH_TEXT_0	L"已下載 %d K，速度 %d K/s"
	#define CLIENTAPP_FLASH_TEXT_1	L"初始化引擎..."
	#define CLIENTAPP_FLASH_TEXT_2	L"初始化微端管理器，请耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_3	L"初始化配置表，請耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_4	L"初始化必備管理系統..."
	#define CLIENTAPP_FLASH_TEXT_5	L"初始化渲染管理..."
	#define CLIENTAPP_FLASH_TEXT_6	L"初始化場景與角色管理器..."
	#define CLIENTAPP_FLASH_TEXT_7	L"初始化邏輯管理器..."
	#define CLIENTAPP_FLASH_TEXT_8	L"初始化腳本資源..."
	#define CLIENTAPP_FLASH_TEXT_9	L"初始化UI資源，請耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_10	L"初始化技能腳本，請耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_11	L"初始化交互管理器..."
#else
	#define CLIENTAPP_FLASH_TEXT_0	L"已下载 %d K，速度 %d K/s"
	#define CLIENTAPP_FLASH_TEXT_1	L"初始化引擎..."
	#define CLIENTAPP_FLASH_TEXT_2	L"初始化微端管理器，请耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_3	L"初始化配置表，请耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_4	L"初始化必备管理系统..."
	#define CLIENTAPP_FLASH_TEXT_5	L"初始化渲染管理..."
	#define CLIENTAPP_FLASH_TEXT_6	L"初始化场景与角色管理器..."
	#define CLIENTAPP_FLASH_TEXT_7	L"初始化逻辑管理器..."
	#define CLIENTAPP_FLASH_TEXT_8	L"初始化脚本资源..."
	#define CLIENTAPP_FLASH_TEXT_9	L"初始化UI资源，请耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_10	L"初始化技能脚本，请耐心等待..."
	#define CLIENTAPP_FLASH_TEXT_11	L"初始化交互管理器..."
#endif

#endif




