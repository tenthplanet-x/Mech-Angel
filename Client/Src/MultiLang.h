
#ifndef _CLIENT_MULTI_LANGUAGE_DEFINE_H
#define _CLIENT_MULTI_LANGUAGE_DEFINE_H 


#define LANGUAGE_MODE_CHS	0	//< ��������
#define LANGUAGE_MODE_TW	1	//< ��������
#define LANGUAGE_MODE_VN	2	//< Խ��
#define LANGUAGE_MODE_KR	3	//< ����


#ifdef _TAIWAN
	#define LANGUAGE_MODE		LANGUAGE_MODE_TW
#else
	#define LANGUAGE_MODE		LANGUAGE_MODE_CHS
#endif


#if LANGUAGE_MODE == LANGUAGE_MODE_TW
	#define CLIENTAPP_FLASH_TEXT_0	L"�����d %d K���ٶ� %d K/s"
	#define CLIENTAPP_FLASH_TEXT_1	L"��ʼ������..."
	#define CLIENTAPP_FLASH_TEXT_2	L"��ʼ��΢�˹������������ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_3	L"��ʼ�����ñ�Ո���ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_4	L"��ʼ���؂����ϵ�y..."
	#define CLIENTAPP_FLASH_TEXT_5	L"��ʼ����Ⱦ����..."
	#define CLIENTAPP_FLASH_TEXT_6	L"��ʼ�������c��ɫ������..."
	#define CLIENTAPP_FLASH_TEXT_7	L"��ʼ��߉݋������..."
	#define CLIENTAPP_FLASH_TEXT_8	L"��ʼ���_���YԴ..."
	#define CLIENTAPP_FLASH_TEXT_9	L"��ʼ��UI�YԴ��Ո���ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_10	L"��ʼ�������_����Ո���ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_11	L"��ʼ������������..."
#else
	#define CLIENTAPP_FLASH_TEXT_0	L"������ %d K���ٶ� %d K/s"
	#define CLIENTAPP_FLASH_TEXT_1	L"��ʼ������..."
	#define CLIENTAPP_FLASH_TEXT_2	L"��ʼ��΢�˹������������ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_3	L"��ʼ�����ñ������ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_4	L"��ʼ���ر�����ϵͳ..."
	#define CLIENTAPP_FLASH_TEXT_5	L"��ʼ����Ⱦ����..."
	#define CLIENTAPP_FLASH_TEXT_6	L"��ʼ���������ɫ������..."
	#define CLIENTAPP_FLASH_TEXT_7	L"��ʼ���߼�������..."
	#define CLIENTAPP_FLASH_TEXT_8	L"��ʼ���ű���Դ..."
	#define CLIENTAPP_FLASH_TEXT_9	L"��ʼ��UI��Դ�������ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_10	L"��ʼ�����ܽű��������ĵȴ�..."
	#define CLIENTAPP_FLASH_TEXT_11	L"��ʼ������������..."
#endif

#endif




