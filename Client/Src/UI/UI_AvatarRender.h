//-----------------------------------------------------------------------------
/** Avatar绘制管理器 **/
//-----------------------------------------------------------------------------
#pragma once

#include "UI_AvatarDefine.h"
#include "IG_Image.h"
#include "RG_Dx9MsaaRenderedTex.h"
//-----------------------------------------------------------------------------

/* Avatar在UI系统下的转义定义 */
typedef GUISys::IG_MaterialPtr		GUI_MaterialPtr;
typedef GUISys::IG_ImagePtr			GUI_ImagePtr;
typedef mem::vector<GUI_ImagePtr>	GUI_ImageList;
/* Avatar在render系统下的转义定义 */
typedef Render::RG_Dx9MsaaRenderedTex RG_RenderTexture;	

//-----------------------------------------------------------------------------
class UI_AvatarRender : public Memory::MM_BaseObject
{
	//---------------------------
	//* 统一shader参数
	//---------------------------
	struct ShaderParams 
	{
	public:

		ShaderParams();

		void		PreRender();
		void		PostRender();

		void		SetupGlobalParams();
		void		SetupLocalParams(size_t paramIndex);

	private:

		//**当前索引**//
		int			m_iCurrParamIndex;

		//**avatar绘制用**//
		NiColor		m_kFogColor;
		NiColorA	m_vDepthFogParam;
		NiColorA	m_vHeightFogParam;

		float		m_fCharacterRimLightPower[3];
		float		m_fCharacterIllumBias[3];
		float		m_fCharacterAmbientPower[3];
		float		m_fCharacterSunPower[3];
		float		m_fCharacterMatEmmisive[3];
		float		m_fCharacterSelfIllumination[3];
		float		m_fCharacterSpecularPower[3];
		float		m_fCharacterSpecularMultiplier[3];

		NiColor		m_vCharacterAmbientColor[3];
		NiColor		m_vCharacterRimLightColor[3];
		NiColor		m_vCharacterDiffuseLightColor[3];

		NiPoint3	m_vCharacterLightWorldDir[3];

		//**恢复参数时使用**//
		NiColor		m_kFogColorRestore;
		NiColorA	m_vDepthFogParamRestore;
		NiColorA	m_vHeightFogParamRestore;

		float		m_fCharacterRimLightPowerRestore;
		float		m_fCharacterIllumBiasRestore;
		float		m_fCharacterAmbientPowerRestore;
		float		m_fCharacterSunPowerRestore;
		float		m_fCharacterMatEmmisiveRestore;
		float		m_fCharacterSelfIlluminationRestore;
		float		m_fCharacterSpecularPowerRestore;
		float		m_fCharacterSpecularMultiplierRestore;

		NiColor		m_vCharacterAmbientColorRestore;
		NiColor		m_vCharacterRimLightColorRestore;
		NiColor		m_vCharacterDiffuseLightColorRestore;

		NiPoint3	m_vCharacterLightWorldDirRestore;
	};

	//---------------------------
	//* 绘制物件组
	//---------------------------
	struct ObjectGroup : public MM_BaseObject
	{
		enum 
		{
			BIT_STATE_NONE			= 0x01,
			BIT_STATE_UPDATE		= 0x02,
			BIT_STATE_EXPIRED	= 0x04,
		};

		typedef mem::vector<AvatarElementKeyId>	 ObjectQueue;

	public:

		ObjectGroup(const StringType& strGroupName);
		const StringType& GetName() const { return m_strGroupName; }

		void		Update(UI_AvatarRender* render, float time, float timeDelta);
		void		Render(UI_AvatarRender* render, RENDER_CLICK_INDEX rdClickIdx);

		void		ConfigObjectsShader(UI_AvatarRender* render, RENDER_CLICK_INDEX rdClickIdx, bool bPre);

		bool		Active(UI_AvatarRender* render);
		bool		Deactive(UI_AvatarRender* render);
		inline bool	IsActive() const { return m_bActived; }

		void		AddObject(AvatarElementKeyId keyId, bool isAddBackGround = false);
		void		RemoveObject(AvatarElementKeyId keyId);
		void		RemoveAllObject();

		inline void ResetTimeForDiscard() { m_fWaitingTimeForDiscard = 0.0f; }
		inline void WaitingForDiscard( float fDelta ) { (m_fWaitingTimeForDiscard += fDelta); } 
		inline float GetTimeForDiscard() const { return m_fWaitingTimeForDiscard; }

		const ObjectQueue& GetMainQueue() const { return m_vMainQueue; }
		const ObjectQueue& GetBackGroundQueue() const { return m_vBackGroundQueue; }
		inline bool IsEmpty() { return m_vMainQueue.empty() && m_vBackGroundQueue.empty(); }

		inline void	SetInUpdateList( bool bInUpdateList )
		{
			if ( bInUpdateList )
			{
				m_eUpdateState = BIT_STATE_UPDATE;
			}
			else
			{
				if ( IsInUpdateList() )
					m_eUpdateState = 0;
			}
		}
		inline void	SetInExpiredList( bool bInExpiredList )
		{
			if ( bInExpiredList )
			{
				m_eUpdateState = BIT_STATE_EXPIRED;
			}
			else
			{
				if ( IsInExpiredList() )
					m_eUpdateState = 0;
			}
		}
		inline bool IsInUpdateList() const { return m_eUpdateState == BIT_STATE_UPDATE; }
		inline bool IsInExpiredList() const { return m_eUpdateState == BIT_STATE_EXPIRED; }
		
		
	private:

		StringType	m_strGroupName;
		
		bool		m_bActived;
		bool		m_bMainQueueDirty;
		bool		m_bBackGroundQueueDirty;

		byte		m_eUpdateState;
		float		m_fWaitingTimeForDiscard;
		
		ObjectQueue	m_vMainQueue;
		ObjectQueue	m_vBackGroundQueue;
	};

	typedef mem::map<StringType, ObjectGroup*> ObjectGroups;
	typedef mem::list<ObjectGroup*> ObjectGroupList;

	//---------------------------
	//* 物件深度排序器
	//---------------------------
	struct ObjectDepthSorter : public Memory::MM_BaseObject
	{
	public:

		ObjectDepthSorter( UI_AvatarManager* m_pkManager );
		~ObjectDepthSorter();

		void Sort(ObjectGroup::ObjectQueue& objectQueue);

	private:

		void collectDepthItems(ObjectGroup::ObjectQueue& objectQueue);

		float chooseDepthSortPivot(int left, int right) const;
		void sortDepthItems(int left, int right, ObjectGroup::ObjectQueue& objectQueue);

	private:

		size_t		m_iNumDepthItems;
		size_t		m_iMaxDepthItems;
		float*		m_pfDepthItems;

		UI_AvatarManager* m_pkManager;
	};

	//---------------------------

public:

	UI_AvatarRender(UI_AvatarManager* pkManager);
	~UI_AvatarRender();

	inline bool isInitSuccess() { return m_bInitSuccess; }

//**********************************************
// ObjectGroup
//**********************************************
public:

	// active & decative
	void ActiveObjects(const StringType& strGroupName);
	void DeactiveObjects(const StringType& strGroupName);
	bool IsObjectsActived(const StringType& strGroupName);
	inline bool IsAnyObjectActived() const { return !m_kObjectGroupUpdateList.empty(); }

	// add & remove
	void AddObject(AvatarElementKeyId keyId, const StringType& strGroupName, bool isAddBackGround = false);
	void RemoveObject(AvatarElementKeyId keyId, const StringType& strGroupName);
	void RemoveAllObjects();

	// update & render 
	void UpdateObjects(float fTime, float fDeltaTime);
	void RenderObjects(RG_AvatarRenderClick* renderClick);

	void ConfigObjectsShader(RENDER_CLICK_INDEX rdClickIdx, bool bPre);

	// expired list
	void RemoveAllExpiredObjectGroup();
	void RemoveExpiredObjectGroup( int iRemoveCount );

	inline void SetObjectGroupExpiredLifeScale( float fTimeScale ) { 
		m_fObjectGroupExpiredLifeScale = fTimeScale;
		m_fObjectGroupExpiredRealLife = m_fObjectGroupExpiredLife * fTimeScale;
	};

private:

	ObjectGroup* _createObjectGroup(const StringType& strGroupName);
	ObjectGroup* _retrieveObjectGroup(const StringType& strGroupName);

	void _destroyAllObjectGroup();
	void _destroyExpiredObjectGroup( ObjectGroup* objectGroup );

	void _addToObjectGroupUpdateList(ObjectGroup* objectGroup);
	void _removeFromObjectGroupUpdateList(ObjectGroup* objectGroup);
	void _clearObjectGroupUpdateList();

	void _addToObjectGroupExpiredList(ObjectGroup* objectGroup);
	void _removeFromObjectGroupExpiredList(ObjectGroup* objectGroup);
	void _operateObjectGroupExpiredList( int iMaxDisardCount );
	void _clearObjectGroupExpiredList();
	void _updateObjectGroupExpiredList(float fTimeDelta);

//**********************************************
// Sorter
//**********************************************
private:

	NiCameraPtr _createCamera();
	void _destroyCamera(NiCameraPtr& camera);

//**********************************************
// Camera
//**********************************************
private:

	ObjectDepthSorter* _createDepthSorter();
	void _destroyDepthSorter(ObjectDepthSorter*& pkSorter);

//**********************************************
// UIImage&UIMaterial
//**********************************************
public:

	void CreateImage(const StringType& sTextureName, int iX, int iY, int iWidth, int iHeight, bool bSmall = false);

	void DestroyMaterial();
	void DestroyAllImages();

private:

	GUI_MaterialPtr _createGUIMaterial(RG_RenderTexture& srcRenderTexture, const StringType& materialName);
	void _destroyGUIMaterial(GUI_MaterialPtr& destMaterialPtr);

	void _createGUIImage(GUI_MaterialPtr& spMaterial, const StringType& sTextureName, int iX, int iY, int iWidth, int iHeight);
	void _destroyAllGUIImage();

//**********************************************
// RenderTexture& RenderTarget
//**********************************************
public:

	bool IsRenderTextureMsaaEnable();
	void ResetRenderTargetGroup(RG_AvatarRenderClick* renderClick, RENDER_CLICK_INDEX rdClickIdx=RENDER_CLICK_ORIGIN);

	static unsigned int GetRenderTargetWidth();
	static unsigned int GetRenderTargetHeight();

	static unsigned int GetSmallRenderTargetWidth();
	static unsigned int GetSmallRenderTargetHeight();

private:

	bool _createRenderTexture(RG_RenderTexture& renderTexture, const char * sTextureName, int iTextureWidth, int iTextureHeight, bool bDepthBuffer=true);
	void _destroyRenderTexture(RG_RenderTexture& renderTexture);

	NiRenderTargetGroupPtr _createRenderTarget(RG_RenderTexture& renderTexture);
	void _destroyRenderTarget(NiRenderTargetGroupPtr& renderTargetPtr);

//**********************************************
// UIAvatarManager CallBack
//**********************************************
public:

	void onActiveObject( AvatarElementKeyId keyId );
	void onDeactiveObject( AvatarElementKeyId keyId );
	void onUpdateObject( AvatarElementKeyId keyId, float fTime, float deltaTime );
	void onConfigObjectShader( AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx, bool bPre);
	
	void onClearGroupDirty( ObjectGroup::ObjectQueue &objectQueue );
	void onDestroyExpiredGroup( const ObjectGroup::ObjectQueue &objectQueue1, const ObjectGroup::ObjectQueue &objectQueue2 );

	void onConfigCamera( AvatarElementKeyId keyId);
	void onConfigShader( AvatarElementKeyId keyId );

	void onRenderSolidPass( AvatarElementKeyId keyId);
	void onRenderTransparentPass( AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx);

	bool IsShouldRender(AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx);
	bool WorldPtToScreenPt(const NiPoint3& kPos, float& fX, float& fY);
public:

	RG_RenderTexture * getRenderTexture() {return &m_kRenderTexture;}
	RG_RenderTexture * getRenderSmallTexture() {return &m_kSmallRenderTexture;}
	RG_RenderTexture * getRenderTextureColored() {return &m_kNoDepthBufferRenderTexture;}
	void	HandleReset(void);		// 处理设备丢失

//**********************************************
// Members
//**********************************************
protected:

	UI_AvatarManager*		m_pkManager;
	bool					m_bInitSuccess;

	NiDX9RendererPtr		m_spRenderer;
	NiCameraPtr				m_spCamera;
	NiCullingProcess		m_kCuller;
	NiVisibleArray			m_kVisibleSet;

	ObjectGroups			m_kObjectGroups;
	ObjectGroupList			m_kObjectGroupUpdateList;
	ObjectGroupList			m_kObjectGroupExpiredList;

	float					m_fObjectGroupExpiredLife;
	float					m_fObjectGroupExpiredLifeScale;
	float					m_fObjectGroupExpiredRealLife;

	ObjectDepthSorter*		m_pkObjectDepthSorter;
	//ObjectShaderSorter	m_kObjectsShaderSorter;
	
	NiRenderTargetGroupPtr	m_spRenderTarget;
	NiRenderTargetGroupPtr	m_spSmallRenderTarget;
	NiRenderTargetGroupPtr	m_spNoDepthBufferRenderTarget;

	RG_RenderTexture		m_kRenderTexture;
	RG_RenderTexture		m_kSmallRenderTexture;
	RG_RenderTexture		m_kNoDepthBufferRenderTexture;

	bool					m_bIsMsaaRenderTexture;
	
	GUI_ImageList			m_kGuiImageSet;
	GUI_MaterialPtr			m_spGuiMaterial;
	GUI_MaterialPtr			m_spGuiMaterialSmall;
	GUI_MaterialPtr			m_spGuiMaterialDepthBuffer;

	ShaderParams			m_kShaderParams;
};
//-----------------------------------------------------------------------------