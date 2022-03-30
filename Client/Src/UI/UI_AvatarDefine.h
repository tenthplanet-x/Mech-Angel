//-----------------------------------------------------------------------------
/** Avatar绘制管理器 **/
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------

/* Avatar绘制click*/
class RG_AvatarRenderClick;

/* Avatar元素 */
class UI_AvatarManager;
class UI_AvatarRender;

/* Avatar元素的定义 */
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

/* 绘制角色charType在一个avatar场景下的索引 */
typedef int AvatarElementSceneIndex;
typedef mem::map<AvatarElementSceneIndex,long> AvatarElementSceneMap;

/* 绘制实例在整个管理器下的统一索引 */
typedef uint64_t AvatarElementKeyId;
typedef mem::map<AvatarElementKeyId, AvatarElementPtr> AvatarElementMap;
typedef mem::vector<AvatarElementKeyId>	AvatarElementKeyIdList;
typedef mem::vector<AvatarBullet*> AvatarBulletList;

//-----------------------------------------------------------------------------

/* 特殊值定义 */
#define Invalid_AvatarElementKeyId		0
#define Invalid_AvatarElementSceneIndex 0
#define Invalid_AvatarElementCharTypeId -1
#define Invalid_AvatarElementCharIndex  -1






//-----------------------------------------------------------------------------
