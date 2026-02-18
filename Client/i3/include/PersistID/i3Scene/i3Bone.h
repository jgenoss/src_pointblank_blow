#if !defined( __I3_BONE_H)
#define __I3_BONE_H

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

typedef struct ALIGN8 _tagi3Bone
{
	char					m_szName[ MAX_NAME];
	INT32					m_ParentIndex;
	INT32					temp[3];
	MATRIX					m_Matrix;
	UINT32					m_Depth;
	INT32					temp2[3];

public:
	char *					getName(void)					{ return m_szName; }
	void					setName( char * pszName)		{ i3String::Copy( m_szName, pszName); }

	INT32					getParentIndex(void)			{ return m_ParentIndex; }
	void					setParentIndex(INT32 idx)		{ m_ParentIndex = idx; }

	MATRIX *				getMatrix(void)					{ return &m_Matrix; }
	void					setMatrix( MATRIX * pMtx)		{ i3Matrix::Copy( &m_Matrix, pMtx); }

	UINT32					getDepth(void)					{ return m_Depth; }
	void					setDepth( UINT32 depth)			{ m_Depth = depth; }
} i3Bone;

#if defined( I3_COMPILER_VC)
#pragma pack( pop )
#endif

#endif
