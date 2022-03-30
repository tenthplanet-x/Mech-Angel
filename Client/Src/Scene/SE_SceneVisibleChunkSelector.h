#ifndef _SE_SCENE_VISIBLE_CHUNK_SELECTOR_H
#define _SE_SCENE_VISIBLE_CHUNK_SELECTOR_H


class SE_SceneWorldMap;
class SE_SceneVisibleChunkSelector : public Memory::MM_BaseObject
{
public:
	SE_SceneVisibleChunkSelector(SE_SceneWorldMap* pkSelectMap,float fSelectionRadius);
	~SE_SceneVisibleChunkSelector();

	void SelectVisibleChunks(NiPoint3 kSelectCenter);
	void SelectVisibleChunks( const NiPoint3& rkCenter , bool bBB  );
	void SelectVisibleChunks(const NiPoint2& rkCenter , NiCamera* pkCamera );
	void SetSelectionRadius( float fR );
	float GetSelectionRadius(){ return m_fSelectionRadius; }

protected:
	SE_SceneVisibleChunkSelector();

	SE_SceneWorldMap* m_pkSelectMap;
	float		      m_fSelectionRadius;	
};



#endif