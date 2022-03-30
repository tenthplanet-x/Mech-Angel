//-----------------------------------------------------------------------------
/** Avatar���ƹ����� **/
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------

/* Avatar����click*/
class RG_AvatarRenderClick;

/* AvatarԪ�� */
class UI_AvatarManager;
class UI_AvatarRender;

/* AvatarԪ�صĶ��� */
struct AvatarElement;
typedef AvatarElement* AvatarElementPtr;

struct AvatarBullet;

enum RENDER_CLICK_INDEX
{
	RENDER_CLICK_ORIGIN			= 1,
	RENDER_CLICK_SMALL			= 1 << 1,
	RENDER_CLICK_NODEPTHBUFFER	= 1 << 2
};

//-----------------------------------------------------------------------------

/* ���ƽ�ɫcharType��һ��avatar�����µ����� */
typedef int AvatarElementSceneIndex;
typedef mem::map<AvatarElementSceneIndex,long> AvatarElementSceneMap;

/* ����ʵ���������������µ�ͳһ���� */
typedef uint64_t AvatarElementKeyId;
typedef mem::map<AvatarElementKeyId, AvatarElementPtr> AvatarElementMap;
typedef mem::vector<AvatarElementKeyId>	AvatarElementKeyIdList;
typedef mem::vector<AvatarBullet*> AvatarBulletList;

//-----------------------------------------------------------------------------

/* ����ֵ���� */
#define Invalid_AvatarElementKeyId		0
#define Invalid_AvatarElementSceneIndex 0
#define Invalid_AvatarElementCharTypeId -1
#define Invalid_AvatarElementCharIndex  -1






//-----------------------------------------------------------------------------
