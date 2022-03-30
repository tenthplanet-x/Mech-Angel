#pragma once
template<class T>
struct ChunkHeightData
{
	ChunkHeightData()
	{
		nFieldX = nFieldY = 0;
		pfHeights = NULL;
		pkNormals = NULL;
	}
	int nFieldX;
	int nFieldY;
	T*    pfHeights;
	NiPoint3* pkNormals;
};

template<class T>
class SE_CompactHeightField : public Memory::MM_BaseObject
{
public:
	SE_CompactHeightField();
	~SE_CompactHeightField();
	void Init( int nScale );
	void UnInit();

	float    GetHeight(float fX, float fY);
	float    GetHeight( float fX , float fY , float fPrevHeight );
	float    GetHeight(int iIndexX, int iIndexY);
	NiPoint3 GetNormal(int iIndexX, int iIndexY);
	void     GetNormal(float fX, float fY, NiPoint3& rkNormal);

	bool     Fill(unsigned int nX , unsigned int nY, 
		unsigned int nHeightFieldX , unsigned int nHeightFieldY ,
		T* pfHeights , NiPoint3* pkNormals , int iDataWidth );

	void     Remove( unsigned int nX , unsigned int nY );
private:
	mem::vector< ChunkHeightData<T> > m_kChunkData;
	float                             m_fGridInvWidth;
	int						          m_nSamplePerChunk;
	int                               m_iWidth;
	int                               m_iChunkCountX;
};
//////////////////////////////////////////////////////////////////////////
template<class T>
SE_CompactHeightField<T>::SE_CompactHeightField()
{
	m_fGridInvWidth = 0.0f;
	m_nSamplePerChunk = 0;
	m_iWidth = 0;
	m_iChunkCountX = 0;
}
template<class T>
SE_CompactHeightField<T>::~SE_CompactHeightField()
{

}
template<class T>
void SE_CompactHeightField<T>::UnInit()
{
	
}
template<class T>
void SE_CompactHeightField<T>::Init( int nScale )
{
	m_iWidth = nScale * Terrain::CHUNK_VERTEX_COUNT * GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_MAP + 1;
	m_fGridInvWidth = nScale * GfxGetWorldConfig().CFG_WORLD_GRID_INVWIDTH;
	m_nSamplePerChunk = nScale * Terrain::CHUNK_VERTEX_COUNT + 1;
	m_iChunkCountX =  GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_MAP;
	m_kChunkData.resize( m_iChunkCountX * m_iChunkCountX  );
}

const float cfTERRAIN_LOW_BOARDER = -5000.0f;

static void FindOutlier( float height[4] , float fPrevHeight )
{
	mem::vector<float> a;
	int n = 4;
	if ( fPrevHeight != 0 )
	{
		n = 8;	
	}
	a.resize(n);
	for ( int i = 0 ; i < n ;i++ )
	{
		if ( i < 4 )
		{
			a[i] = height[i];
		}
		else
		{
			a[i] = fPrevHeight;
		}
	}
	std::sort( a.begin() , a.end() );
	float q[5];
	for ( int i = 1 ; i < 4 ; i++ )
	{
		q[i] = 0.0f;
		
		float k = ( i / 4.0f ) * ( n - 1 ) + 1.0f;
		int ik = (int)floor(k) - 1;
		T_ASSERT( ik >=0 && ik <= n - 2 );
		float f = k - floor(k) ;
		q[i] = a[ik] + ( f * ( a[ik+1] - a[ik] ) );
	}
	q[0] = a[0];
	q[4] = a[n-1];
	float fIQR = q[3] - q[1];

	float fLow = q[1] - 2.0f*fIQR - 10.0f;
	float fHigh = q[3] + 2.0f * fIQR + 10.0f;

	for ( int i = 0 ; i < 4 ; i++ )
	{
		if ( height[i] < fLow || height[i] > fHigh )
		{
			height[i] = cfTERRAIN_LOW_BOARDER;
		}
	}
	/*
	Find the kth smallest member in the array of values, where:
	k=(quart/4)*(n-1))+1

	If k is not an integer, truncate it but store the fractional portion (f) for use in step 3.
	quart = value between 0 and 4 depending on which quartile
	you want to find.
	n     = number of values in the array

	Find the smallest data point in the array of values that is greater than the kth smallest, the (k+1)th smallest member.
	Interpolate between the kth smallest and the (k+1)th smallest values:
	Output = a[k]+(f*(a[k+1]-a[k]))

	a[k]   = the kth smallest<BR/>
	a[k+1] = the k+1th smallest

	Find the inter quartile range, which is IQR = Q3 - Q1, where Q3 is the third quartile and Q1 is the first quartile. Then find these two numbers:

	a) Q1 - 1.5*IQR
	b) Q3 + 1.5*IQR

	Any observation that is below a) or above b) can be considered an outlier.
	*/
}

template<class T>
float SE_CompactHeightField<T>::GetHeight(float fX, float fY)
{
	float fIndexX = fX * m_fGridInvWidth, 
		fIndexY = fY * m_fGridInvWidth;
	int nIndexX = Utility::Float2Int(floor(fIndexX)), 
		nIndexY = Utility::Float2Int(floor(fIndexY));
	/*-------------------------
	------- - ----x------ + --
	|  p0           p1
	-
	|
	y
	|
	|  p2           p3
	+
	--------------------------*/

	float afPHeight[4];
	bool bUseNormalHeight = false;
	float fNormalHeight = 0.0f;
	float fHeight = 0.0f;
	int nCount = 0;

	for ( int y = 0 ; y < 2 ; y++ )
	{
		for ( int x = 0 ; x < 2 ; x++ )
		{
			float & h = afPHeight[x + y * 2];
			h = cfTERRAIN_LOW_BOARDER;

			int nRelativePX = nIndexX + x, 
				nRelativePY = nIndexY + y;
			
			h = GetHeight( nRelativePX , nRelativePY );

			if ( h > cfTERRAIN_LOW_BOARDER )
			{
				fNormalHeight = h;
				fHeight += fNormalHeight;
				nCount++;
			}
			else
			{
				bUseNormalHeight = true;
			}
		}
	}
	if ( bUseNormalHeight )
	{
		return fNormalHeight;
	}
	fHeight = fHeight / nCount;
	FindOutlier( afPHeight , 0 );
	for ( int i = 0 ; i < 4 ; i++ )
	{
		if ( afPHeight[i] == cfTERRAIN_LOW_BOARDER )
		{
			for ( int j = 0 ; j < 4 ; j++ )
			{
				if ( afPHeight[j] == cfTERRAIN_LOW_BOARDER )
				{
					continue;
				}
				afPHeight[i] = afPHeight[j];
				break;
			}	
		}
	}
	
	float du = fIndexX - nIndexX;
	float dv = fIndexY - nIndexY;
	fHeight = 0.0f;
	if(du > dv)
	{ 
		// ÓÒ±ß
		fHeight = afPHeight[0] + du * (afPHeight[1] - afPHeight[0]) + dv * (afPHeight[3] - afPHeight[1]);
	}
	else
	{ 
		// ×ó±ß
		fHeight = afPHeight[0] + du * (afPHeight[3] - afPHeight[2]) + dv * (afPHeight[2] - afPHeight[0]);
	}
	return fHeight;
}

template<class T>
float SE_CompactHeightField<T>::GetHeight(float fX, float fY,float fPrevHeight )
{
	float fIndexX = fX * m_fGridInvWidth, 
		fIndexY = fY * m_fGridInvWidth;
	int nIndexX = Utility::Float2Int(floor(fIndexX)), 
		nIndexY = Utility::Float2Int(floor(fIndexY));
	/*-------------------------
	------- - ----x------ + --
	|  p0           p1
	-
	|
	y
	|
	|  p2           p3
	+
	--------------------------*/

	float afPHeight[4];
	bool bUseNormalHeight = false;
	float fNormalHeight = 0.0f;
	float fHeight = 0.0f;
	int nCount = 0;

	for ( int y = 0 ; y < 2 ; y++ )
	{
		for ( int x = 0 ; x < 2 ; x++ )
		{
			float & h = afPHeight[x + y * 2];
			h = cfTERRAIN_LOW_BOARDER;

			int nRelativePX = nIndexX + x, 
				nRelativePY = nIndexY + y;
			
			h = GetHeight( nRelativePX , nRelativePY );

			if ( h > cfTERRAIN_LOW_BOARDER )
			{
				fNormalHeight = h;
				fHeight += fNormalHeight;
				nCount++;
			}
			else
			{
				bUseNormalHeight = true;
			}
		}
	}
	if ( bUseNormalHeight )
	{
		return fNormalHeight;
	}
	fHeight = fHeight / nCount;
	FindOutlier( afPHeight , fPrevHeight );
	for ( int i = 0 ; i < 4 ; i++ )
	{
		if ( afPHeight[i] == cfTERRAIN_LOW_BOARDER )
		{
			bool bFound = false;
			for ( int j = 0 ; j < 4 ; j++ )
			{
				if ( afPHeight[j] == cfTERRAIN_LOW_BOARDER )
				{
					continue;
				}
				afPHeight[i] = afPHeight[j];
				break;
				bFound = true;
			}
			if ( !bFound )
			{
				afPHeight[i] = fPrevHeight;
			}
		}
	}
	
	float du = fIndexX - nIndexX;
	float dv = fIndexY - nIndexY;
	fHeight = 0.0f;
	if(du > dv)
	{ 
		// ÓÒ±ß
		fHeight = afPHeight[0] + du * (afPHeight[1] - afPHeight[0]) + dv * (afPHeight[3] - afPHeight[1]);
	}
	else
	{ 
		// ×ó±ß
		fHeight = afPHeight[0] + du * (afPHeight[3] - afPHeight[2]) + dv * (afPHeight[2] - afPHeight[0]);
	}
	return fHeight;
}

template<class T>
float SE_CompactHeightField<T>::GetHeight(int iIndexX, int iIndexY)
{
	if(iIndexX >= 0 && iIndexX < m_iWidth && iIndexY >= 0 && iIndexY < m_iWidth)
	{
		int nX = iIndexX / ( m_nSamplePerChunk - 1 );
		int nY = iIndexY / ( m_nSamplePerChunk - 1 );

		if ( nX < 0 || nX >= m_iChunkCountX || nY < 0 || nY >= m_iChunkCountX )
		{
			return 0.0f;
		}
		ChunkHeightData<T>& rkData = m_kChunkData[ nX + nY * m_iChunkCountX ];
		if ( rkData.pfHeights == NULL )
		{
			return 0.0f;
		}
		T* pfHeight = rkData.pfHeights;

		if ( iIndexX < rkData.nFieldX || iIndexY < rkData.nFieldY )
		{
			return 0.0f;
		}

		if ( iIndexX - rkData.nFieldX >= m_nSamplePerChunk || 
			 iIndexY - rkData.nFieldY >= m_nSamplePerChunk )
		{
			return 0.0f;
		}

		float fHeight = (float)pfHeight[( iIndexY - rkData.nFieldY ) * m_nSamplePerChunk 
			+ iIndexX - rkData.nFieldX ];
		return fHeight;
	}
	return 0.0f;
}

template<class T>
NiPoint3 SE_CompactHeightField<T>::GetNormal(int iIndexX, int iIndexY)
{
	NiPoint3 kNormal = NiPoint3::UNIT_Z;
	if(iIndexX >= 0 && iIndexX < m_iWidth && iIndexY >= 0 && iIndexY < m_iWidth)
	{
		int nX = iIndexX / ( m_nSamplePerChunk - 1 );
		int nY = iIndexY / ( m_nSamplePerChunk - 1 );

		if ( nX < 0 || nX >= m_iChunkCountX || nY < 0 || nY >= m_iChunkCountX )
		{
			return kNormal;
		}
		ChunkHeightData<T>& rkData = m_kChunkData[ nX + nY * m_iChunkCountX ];
		if ( rkData.pkNormals == NULL )
		{
			return kNormal;
		}
		NiPoint3* pkNormal = rkData.pkNormals;

		if ( iIndexX < rkData.nFieldX || iIndexY < rkData.nFieldY )
		{
			return kNormal;
		}

		if ( iIndexX - rkData.nFieldX >= m_nSamplePerChunk || 
			iIndexY - rkData.nFieldY >= m_nSamplePerChunk )
		{
			return kNormal;
		}

		NiPoint3 kNormal = pkNormal[( iIndexY - rkData.nFieldY ) * m_nSamplePerChunk 
			+ iIndexX - rkData.nFieldX ];
		return kNormal;
	}
	return kNormal;
}

template<class T>
void SE_CompactHeightField<T>::GetNormal(float fX, float fY, NiPoint3& rkNormal)
{
	float fIndexX = fX * m_fGridInvWidth, fIndexY = fY * m_fGridInvWidth;
	int nIndexX = Utility::Float2Int(floor(fIndexX)), nIndexY = Utility::Float2Int(floor(fIndexY));

	int nX = nIndexX / ( m_nSamplePerChunk - 1 );
	int nY = nIndexY / ( m_nSamplePerChunk - 1 );

	if ( nX < 0 || nX >= m_iChunkCountX || nY < 0 || nY >= m_iChunkCountX )
	{
		rkNormal = NiPoint3::UNIT_Z;
		return;
	}
	ChunkHeightData<T>& rkData = m_kChunkData[ nX + nY * m_iChunkCountX ];
	if ( rkData.pkNormals == NULL )
	{
		rkNormal = NiPoint3::UNIT_Z;
		return;
	}
	/*NiPoint3 kNormal = GetNormal( nIndexX , nIndexY );
	rkNormal = kNormal;
	return;*/
	bool bUseNormal = false;

	NiPoint3 kPNormal[4];
	for (int y = 0 ; y < 2 ; ++y)
	{
		for (int x = 0 ; x < 2 ; ++x)
		{
			int nPX = nIndexX + x, nPY = nIndexY + y;
			kPNormal[x + y * 2] = GetNormal( nPX , nPY );
			if ( kPNormal[x + y * 2].z == -1.0f )
			{
				bUseNormal = true;
			}
		}
	}
	//if ( bUseNormal )
	//{
	//	/*for ( int i = 0 ; i < 4; i++ )
	//	{
	//		if ( kPNormal[i].z != -1.0f )
	//		{
	//			rkNormal = kPNormal[i];
	//			return;
	//		}
	//	}*/
	//	rkNormal = NiPoint3::UNIT_Z;
	//	return;
	//}

	float du = fIndexX - nIndexX;
	float dv = fIndexY - nIndexY;
	// ÓÒ±ß
	if(du > dv)
	{ 
		rkNormal = (kPNormal[0]) + du * ((kPNormal[1]) - (kPNormal[0])) + dv * ((kPNormal[3]) - (kPNormal[1]));
		rkNormal.Unitize();
	}
	// ×ó±ß
	else
	{ 
		rkNormal = (kPNormal[0]) + du * ((kPNormal[3]) - (kPNormal[2])) + dv * ((kPNormal[2]) - (kPNormal[0]));
		rkNormal.Unitize();
	}
}

template<class T>
bool SE_CompactHeightField<T>::Fill( unsigned int nX , unsigned int nY, 
								 unsigned int nHeightFieldX , unsigned int nHeightFieldY , 
								 T* pfHeights , NiPoint3* pkNormals , int iDataWidth )
{
	if (nX >= m_iChunkCountX || nY >= m_iChunkCountX )
	{
		return false;
	}
	if ( iDataWidth != m_nSamplePerChunk )
	{
		Remove( nX , nY );
		return false;
	}
	ChunkHeightData<T>& rkData = m_kChunkData[ nX + nY * m_iChunkCountX ];
	rkData.nFieldX = nHeightFieldX;
	rkData.nFieldY = nHeightFieldY;
	rkData.pfHeights = pfHeights;
	rkData.pkNormals = pkNormals;
	return true;
}

template<class T>
void SE_CompactHeightField<T>::Remove( unsigned int nX , unsigned int nY )
{
	if (nX >= m_iChunkCountX || nY >= m_iChunkCountX )
	{
		return ;
	}
	ChunkHeightData<T>& rkData = m_kChunkData[ nX + nY * m_iChunkCountX ];
	rkData.pfHeights = NULL;
	rkData.pkNormals = NULL;
}

class SE_ActorShadowField : public Memory::MM_BaseObject
{
    struct ChunkLightMapData
    {
        unsigned int nChunkIndexX;
        unsigned int nChunkIndexY;
        unsigned char* pkLightMap;

        unsigned int nChunkLightMapX;
        unsigned int nChunkLightMapY;

        ChunkLightMapData(void)
            : nChunkIndexX(0)
            , nChunkIndexY(0)
            , pkLightMap(NULL)

            , nChunkLightMapX(0)
            , nChunkLightMapY(0)
        {
        }
    };

    typedef mem::vector<ChunkLightMapData> ChunkLightMapDataArray;

public:
	SE_ActorShadowField();
	~SE_ActorShadowField();

	void Init();
	void UnInit();

    void AddChunkLightMap(unsigned int nChunckIndexX, unsigned int nChunckIndexY, unsigned char* pkChunkLightMap, 
        unsigned int nLightMapSize);
    void RemoveChunkLightMap(unsigned int nChunckIndexX, unsigned int nChunckIndexY);

    float GetShadowAmount(float fWorldX, float fWorldY)
    {
        unsigned int nChunkIndexX = (unsigned int)(fWorldX * m_fCDW);
        unsigned int nChunkIndexY = (unsigned int)(fWorldY * m_fCDW);
        if (nChunkIndexX >= m_nWorldChunckCountX || nChunkIndexY >= m_nWorldChunckCountX)
        {
            return 1.0f;
        }

        ChunkLightMapData& rkLightMap = m_kChunkLightMapsInWorld[nChunkIndexY * m_nWorldChunckCountX + nChunkIndexX];
        if (rkLightMap.pkLightMap == NULL)
        {
            return 1.0f;
        }

        unsigned int nLightMapRelativeX = (unsigned int)(fWorldX * m_fWDW) - rkLightMap.nChunkLightMapX;
        unsigned int nLightMapRelativeY = (unsigned int)(fWorldY * m_fWDW) - rkLightMap.nChunkLightMapY;

        unsigned int nOffset = nLightMapRelativeY * 4 * m_nChunkLightMapWidth +  nLightMapRelativeX * 4 + 3;
        if (nOffset >= m_nChunkLightMapSize)
        {
            return 1.0f;
        }
        return rkLightMap.pkLightMap[nOffset] * INV_255;
    }

private:
    static const float      INV_255;

    ChunkLightMapDataArray  m_kChunkLightMapsInWorld;

    unsigned int            m_nWorldChunckCountX;
    unsigned int            m_nChunkLightMapWidth;
    unsigned int            m_nWorldLightMapWidth;

    float                   m_fWDW;
    float                   m_fCDW;

    unsigned int            m_nChunkLightMapSize;
};
