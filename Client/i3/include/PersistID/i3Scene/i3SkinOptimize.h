#if !defined( __I3_SKIN_OPTIMIZE_H )
#define __I3_SKIN_OPTIMIZE_H

//	Skinning optimize
//	make : 2006. 1. 26. by dfly79
//

#define	I3_SKINNING_OPT_MAXMATRIX			20

#define I3_SKINNING_OPT_NONE				0x00000000		// 스킨이 아니다.
#define I3_SKINNING_OPT_MINIMUM				0x00000001
#define	I3_SKINNING_OPT_REGULAR				0x00000002		// 기본적으로 최적화를 실행
#define	I3_SKINNING_OPT_MAXIMUM				0x00000004		// 최대 옵티마이즈
#define I3_SKINNING_OPT_CUTWEIGHT			0x00000008		// 버택스의 절삭값에 따른 매트릭스 상쇠 적용

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct ALIGN4 _tagI3PRIMITIVE_GROUPINFO
{
	INT32	m_StartIndex;				// 인덱스의 시작 위치
	INT32	m_PrimCount;				// 인덱스 카운터
	
	INT32	m_BoneMatrixCount;			// 사용된 매트릭스 수
	UINT8	m_BoneMatrix[ I3_SKINNING_OPT_MAXMATRIX ];			// 본 매트릭스 인덱스
} I3PRIMITIVE_GROUPINFO;

typedef struct ALIGN4 _tagI3PERSIST_PRIMITIVEINFO
{
	UINT8	m_ID[4];
	INT32	m_GroupCount;

} I3PERSIST_PRIMITIVEINFO;

// 임시 버퍼용
typedef struct ALIGN4 _tagi3VertexBlendBuffer
{
	UINT32	m_VertexIndex;
	INT32	m_Group;
	UINT8	m_BlendIndex[ 4 ];
	REAL32	m_BlendWeight[ 4 ];
} i3VertexBlendBuffer;

#if defined( I3_COMPILER_VC )
#pragma pack( pop )
#endif

class I3_EXPORT_SCENE i3SkinOptimize : public i3NamedElement
{
	I3_CLASS_DEFINE( i3SkinOptimize );

protected:
	UINT32		m_Type;
	REAL32		m_Thresh;			// 절삭 값

	// 가공할 데이터
	i3List		m_OptGroupList;		// 본 매트릭스에 맞춰진 버택스 그룹리스트
	i3List		m_VertexBlendList;	// i3VertexBlendBuffer 새로 생성할 버택스 데이터

	// 임시 데이터
	i3BinList	m_PrimitiveOrder;	// 인덱스로부터 뽑은 버택스 어레이
	i3BinList	m_IndexOrder;
	
	INT32		m_MaxBoneMatrixCount;	// 하드웨어에서 지원하는 본 메트릭스 카운트
	INT32		m_MaxBlendIndexCount;	// 하드웨어에서 지원하는 블랜드 인덱스 수

	i3VertexBlendBuffer	* m_pVertexBlendBuffer;
	INT32		m_nInitialBufferIndex;	// 초기 버퍼 생성에서 사용된 버퍼의 갯수
										// Optimizing도중 새로 생성된 vertexblendbuffer의 release를 위해 사용됩니다.

	void		_RemoveAllOptGroupList( void );
	void		_RemoveAllPrimOrder( void );
	void		_RemoveAllVertexBlendList( void );
	void		_RemoveAllIndexOrder( void )				{ m_IndexOrder.Clear(); }
	
	// 인덱스에 대한 버택스 나열
	void		_BuildPrimOrder( i3VertexArray * pVA, i3IndexArray * pIA );
	void		_BuildBlendIndexOrder( i3VertexArray * pVA );
	void		_BuildBlendWeightOrder( i3VertexArray * pVA );
	void		_CreateVertexBlendBuffer( INT32	nCount );
	
	// 절삭값 처리
	void		_CalcThreshWeight( void );
	// Bone Matrix의 인덱스만 검색
	BOOL		_CheckBoneMatrix( i3List * pMatrixMap, UINT32 vIdx );
		
	// 옵티마이즈된 그룹에 대한 버택스의 본 매트릭스 재조정 ( TriList )
	void		_ReplaceBlendWeight( I3G_PRIMITIVE PrimType );
	
	INT32		_CheckVertexBlendWithVertexArray( UINT32 gIndex, UINT32 vIndex );

	BOOL		_CheckWeightDump( I3G_PRIMITIVE PrimType );
	
	// 본 팔레트 분할에 따른 인덱스 재배치
	void		_ReplaceBonePallete( I3G_PRIMITIVE PrimType );

	INT32		_FindVertexBlendWhichWithBuffer( UINT8 Blend, I3PRIMITIVE_GROUPINFO * pInfo );

	INT32		_getOptGroupCount( void )					{ return m_OptGroupList.GetCount(); }
	I3PRIMITIVE_GROUPINFO*	_getOptGroup( INT32 idx )		{ return (I3PRIMITIVE_GROUPINFO*) m_OptGroupList.GetItem( idx ); }

	INT32		_getVertexBlendListCount(void)				{ return m_VertexBlendList.GetCount(); }
	i3VertexBlendBuffer * _getVertexBlendBuffer( INT32 idx )	{ return (i3VertexBlendBuffer*) m_VertexBlendList.GetItem( idx ); }
	void		_addVertexBlendList( i3VertexBlendBuffer * pBuffer ) { m_VertexBlendList.Add( (void*) pBuffer ); }
	UINT32		_getVertexIndex( INT32 idx )				{ return (_getVertexBlendBuffer( idx ))->m_VertexIndex; }

	UINT32		_getPrimitive( INT32 idx )					{ return (UINT32) m_PrimitiveOrder.GetItem( idx ); }
	void		_setPrimitive( INT32 idx, UINT32 value )	{ m_PrimitiveOrder.SetItem( idx, (void*) value ); }
	INT32		_getPrimitiveCount( void )					{ return m_PrimitiveOrder.GetCount(); }

	UINT8		_getBlendIndex( UINT32 which, INT32 idx )	{ return (_getVertexBlendBuffer( idx ))->m_BlendIndex[ which ]; }
	void		_setBlendIndex( UINT32 which, INT32 idx, UINT8 value )	{ (_getVertexBlendBuffer( idx ))->m_BlendIndex[ which ] = value; }
	
	REAL32		_getBlendWeight( UINT32 which, INT32 idx )	{ return (_getVertexBlendBuffer( idx ))->m_BlendWeight[which]; }
	void		_setBlendWeight( UINT32 which, INT32 idx, REAL32 value )	{ (_getVertexBlendBuffer( idx ))->m_BlendWeight[which] = value; }
		
	UINT32		_getIndexOrder( INT32 idx )					{ return (UINT32) m_IndexOrder.GetItem( idx ); }
	void		_setIndexOrder( INT32 idx, UINT32 value )	{ m_IndexOrder.SetItem( idx, (void*) value ); }
	INT32		_getIndexOrderCount( void )					{ return m_IndexOrder.GetCount(); }
	void		_addIndexOrder( UINT32 value )				{ m_IndexOrder.Add( (void*) value ); }
	void		_setIndexOrderCount( INT32 count )			{ m_IndexOrder.SetCount( count ); }
	
	void		OptimizeTriList( void );
	void		OptimizeTriStrip( void );

public:
	UINT32		getType( void )					{ return m_Type; }
	void		setType( UINT32 type )			{ m_Type = type; }
	void		addType( UINT32 type )			{ m_Type |= type; }
	void		removeType( UINT32 type )		{ m_Type &= ~type; }

	INT32		getMaxMatrix( void )			{ return m_MaxBoneMatrixCount; }
	void		setMaxMatrix( INT32 value )		{ m_MaxBoneMatrixCount = value; }

	INT32		getMaxIndex( void )				{ return m_MaxBlendIndexCount; }
	void		setMaxIndex( INT32 value )		{ m_MaxBlendIndexCount = value; }
	
	REAL32		getThresh( void )				{ return m_Thresh; }
	void		setThresh( REAL32 th )			{ m_Thresh = th; }

public:
	i3SkinOptimize(void);
	virtual ~i3SkinOptimize(void);

	BOOL		BuildOptimizedData( I3G_PRIMITIVE type, i3VertexArray * pVA, i3IndexArray * pIA,
		INT32 BlendIndexCount, INT32 BoneMatrixCount );

	void		GetVertexArray( i3VertexArray * pSrcVA, i3VertexArray * pDestVA );
	void		GetIndexArray( i3IndexArray * pSrcIA, i3IndexArray * pDestIA );
	void		GetOptimizedData( i3List * pList );
	
	virtual UINT32 OnSave( i3ResourceFile * pResFile );
	virtual UINT32 OnLoad( i3ResourceFile * pResFile );

#if defined( I3_DEBUG )
	void		Dump( void );
#endif
	
};

#endif	// __I3_SKIN_OPTIMIZE_H
