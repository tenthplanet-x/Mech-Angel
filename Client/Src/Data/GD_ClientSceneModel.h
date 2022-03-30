#pragma once
//客户端场景模型
#include "GD_SceneModel.h"
//#include "TS_BoundingVolume.h"


struct ModelInfo
{
	NiPoint3   kPos;
	NiMatrix3  kRot;
	float      fScale;
	int        iIndex;
	StringType kResName;
	bool       bPick;
	bool       bWalkable;
	bool       bRenderReflect;

    int        kPLID[MAX_DYNAMIC_POINT_LIGHT];
	mem::vector<NiTexturePtr> spLightMapTexList;
};

class GD_ClientSceneModel : public Data::GD_SceneModel
{
public:
    GD_ClientSceneModel(void);
	virtual ~GD_ClientSceneModel(void);

	void GetModelType(bool& bSkinned , bool& bGlow);
	bool SetTargetForNiPick(NiPick* pkPick);
	bool SetTargetForPathPick( NiPick* pkPick );
    bool IsSceneEffectModel(void) { return (_isInitedWithLoaded() && (m_pkModelContent != NULL) && ( m_bSceneFx || m_bSceneModelFx )); }
    bool IsWalkableModel() { return m_bWalkable; }
    bool IsPickableModel() { return m_bIsClickable; }
	bool IsRenderReflect() { return m_bRenderReflect; }
	//不需要Cull的静态模型使用的接口，可以减少Cull的开销
	void AddToVisibleArray( NiVisibleArray& rkVisibleSet );
	void EnableTransparent( bool bEnable );
	bool IsTransparent() const{ return m_bTransparent; }
	bool IsSoftParticle() const;
	void UpdateAlphaBlend();

	void SetExtraModelInfo( const ModelInfo& rkModelInfo );

	Terrain::TS_BoundingVolume& GetBoundingBox(){ return m_kWorldBound; }

protected:
    virtual bool _doLoad(const StringType& strModFileName, int iActiveModelContentIndex);
	virtual void _doInitAfterLoad(void);
	virtual void _doPreRelease(void);

    virtual bool _doIsUpdatable(float fTime);

	bool                    m_bPickFlag;
	int                     m_nPointLightID;
	mem::vector<NiTexturePtr> m_spLightMapTexList;
	int                     m_vPLID[MAX_DYNAMIC_POINT_LIGHT];
	NiColor                 m_vPLColor[MAX_DYNAMIC_POINT_LIGHT];
	NiPoint3                m_vPLPos[MAX_DYNAMIC_POINT_LIGHT];
	NiPoint3                m_vPLParam[MAX_DYNAMIC_POINT_LIGHT];
	bool                    m_bIsClickable;
	NiAVObject*             m_pkPathPickRoot;
	Terrain::TS_BoundingVolume m_kWorldBound;
	bool                       m_bTransparent;
	bool                       m_bSceneFx;
	bool                       m_bWalkable;
	bool                       m_bRenderReflect;
	bool                       m_bSceneModelFx;
	bool                       m_bSceneParticleSystem;

private:
    float m_fLastUpdateTime;
};

typedef mem::vector<GD_ClientSceneModel*>    SE_SceneModelVector;
typedef SE_SceneModelVector::iterator	     SE_SceneModelVectorIter;
