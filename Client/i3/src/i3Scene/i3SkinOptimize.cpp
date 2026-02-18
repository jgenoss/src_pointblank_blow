#include "i3SceneDef.h"
#include "i3SkinOptimize.h"

/*
//#define I3_DUMP_SKINOPT					// Dump

#include "./i3Gfx/i3GfxGlobalVariable.h"

I3_CLASS_INSTANCE( i3SkinOptimize, i3NamedElement );

i3SkinOptimize::i3SkinOptimize(void)
{
	m_Type					= I3_SKINNING_OPT_REGULAR | I3_SKINNING_OPT_CUTWEIGHT;
	m_MaxBoneMatrixCount	= 0;
	m_MaxBlendIndexCount	= 0;
	m_Thresh				= 0.0f;

	m_pVertexBlendBuffer	= nullptr;
	m_nInitialBufferIndex	= 0;
}

i3SkinOptimize::~i3SkinOptimize(void)
{
	_RemoveAllOptGroupList();

	_RemoveAllPrimOrder();

	_RemoveAllVertexBlendList();

	_RemoveAllIndexOrder();
}

void i3SkinOptimize::_RemoveAllOptGroupList( void )
{	
	INT32 i;
	I3PRIMITIVE_GROUPINFO * pInfo;

	for( i = 0; i < m_OptGroupList.GetCount(); i++ )
	{
		pInfo = _getOptGroup( i );

		I3MEM_SAFE_FREE_POINTER( pInfo );
	}

	m_OptGroupList.Clear();
}

void i3SkinOptimize::_RemoveAllPrimOrder( void )
{
	m_PrimitiveOrder.Clear();
}

void i3SkinOptimize::_RemoveAllVertexBlendList( void )
{	
	I3MEM_SAFE_FREE_POINTER( m_pVertexBlendBuffer );

	if( m_VertexBlendList.GetCount() > m_nInitialBufferIndex)
	{
		for( INT32 i = m_nInitialBufferIndex; i < m_VertexBlendList.GetCount(); i++)
		{
			i3VertexBlendBuffer * pBuf = (i3VertexBlendBuffer*)m_VertexBlendList.Items[i];

			I3MEM_SAFE_FREE_POINTER( pBuf);
		}
	}
	
	m_VertexBlendList.Clear();
}

void i3SkinOptimize::_BuildPrimOrder( i3VertexArray * pVA, i3IndexArray * pIA, INT32 nPrimCount )
{
	INT32 i;

	if( pIA == nullptr )
	{
		m_PrimitiveOrder.SetCount( pVA->GetCount() );

		for( i = 0; i < _getPrimitiveCount(); i++ )
			_setPrimitive( i, (UINT32) i );
	}
	else
	{
		pIA->Lock();

		m_PrimitiveOrder.SetCount(nPrimCount*3);
		for( i = 0; i < _getPrimitiveCount(); i++ )
			_setPrimitive(i, (UINT32) pIA->GetIndex( i ) );

		pIA->Unlock();
	}
}

void i3SkinOptimize::_CreateVertexBlendBuffer( INT32 nCount )
{
	INT32	i;

	if( nCount > 0 )
	{
		m_VertexBlendList.SetCount( nCount );

		m_pVertexBlendBuffer = (i3VertexBlendBuffer*) i3MemAlloc( sizeof(i3VertexBlendBuffer) * nCount );
		I3ASSERT( m_pVertexBlendBuffer );

		for( i = 0; i < nCount; i++ )
		{	
			m_pVertexBlendBuffer[ i ].m_VertexIndex	= i;
			m_pVertexBlendBuffer[ i ].m_Group		= -1;

			m_VertexBlendList.SetItem( i, &m_pVertexBlendBuffer[ i ] );
		}
	}

	m_nInitialBufferIndex = nCount;
}

void i3SkinOptimize::_BuildBlendIndexOrder( i3VertexArray * pVA )
{
	UINT32	i, j;
	i3VertexBlendBuffer * pBuffer;
	
	for( i = 0; i < pVA->GetCount(); i++ )
	{
		pBuffer = _getVertexBlendBuffer( i );

		memset( pBuffer->m_BlendIndex, 0xFF, sizeof(UINT8) * getMaxIndex() );

		for( j = 0; j < (UINT32) pVA->GetBlendIndexCount(); j++)
			pBuffer->m_BlendIndex[ j ] = (UINT8) pVA->GetBlendIndex( j, i );
	}
}

void i3SkinOptimize::_BuildBlendWeightOrder( i3VertexArray * pVA )
{
	UINT32	i, j;
	i3VertexBlendBuffer * pBuffer;
	REAL32	TotalW;

	if( getMaxIndex() > pVA->GetBlendWeightCount() )
	{
		for( i = 0; i < pVA->GetCount(); i++ )
		{
			pBuffer = _getVertexBlendBuffer( i );

			TotalW = 0.0f;
			
			memset( pBuffer->m_BlendWeight, 0, sizeof( REAL32 ) * getMaxIndex() );

			for( j = 0; j < (UINT32) pVA->GetBlendWeightCount(); j++ )
			{
				pBuffer->m_BlendWeight[ j ] = pVA->GetBlendWeight( j, i );
				TotalW += pBuffer->m_BlendWeight[ j ];
			}
			pBuffer->m_BlendWeight[ pVA->GetBlendWeightCount() ] = 1.0f - TotalW;
		}
	}
	else
	{
		// 실제 데이터보다 적은 경우에는 큰값 위주로 넣는다.
		for( i = 0; i < pVA->GetCount(); i++ )
		{
			pBuffer = _getVertexBlendBuffer( i );

			TotalW = 0.0f;
			
			memset( pBuffer->m_BlendWeight, 0, sizeof( REAL32 ) * getMaxIndex() );

			for( j = 0; j < (UINT32) pVA->GetBlendWeightCount(); j++ )
			{
				if( j < (UINT32) (getMaxIndex() - 1) )
				{
					pBuffer->m_BlendWeight[ j ] = pVA->GetBlendWeight( j, i );

					TotalW += pBuffer->m_BlendWeight[ j ];
				}
			}
			pBuffer->m_BlendWeight[ getMaxIndex() - 1 ] = 1.0f - TotalW;
		}
	}
}

BOOL i3SkinOptimize::_CheckBoneMatrix( List * pMatrixMap, UINT32 vIdx )
{
	INT32	i;
	UINT8	BlendIndex;
	
	for( i = 0; i < getMaxIndex(); i++ )
	{	
		BlendIndex = (UINT8) _getBlendIndex( i, vIdx );

		if( BlendIndex == (UINT8) 0xFF )
			continue;

		if( pMatrixMap->IndexOf( (void*) BlendIndex ) == 0xFFFFFFFF )
		{	
			if( pMatrixMap->GetCount() < getMaxMatrix() )
			{
				// 공유하지 않는 경우 매트릭스맵에 추가
				pMatrixMap->Add( (void*) BlendIndex );
			}
			else
			{
				// 추가할 공간이 없다?
				pMatrixMap->Add( (void*) BlendIndex );
				return FALSE;
			}
		}
	}

	return TRUE;
}

void i3SkinOptimize::_CalcThreshWeight( void )
{
	UINT32	i;
	INT32	j, k;
	INT32	count;
	REAL32	f;

	for( i = 0; i < (UINT32) _getVertexBlendListCount(); i++ )
	{
		// Weight값들에 대해 절삭값 비교
		count = 0;
		for( j = 0; j < getMaxIndex(); j++ )
		{	
			if( _getBlendIndex( j, i ) == 0xFF )
				continue;

			if( _getBlendWeight( j, i ) <= m_Thresh )
			{
				_setBlendIndex( j, i, 0xFF );
				//_setBlendWeight( j, i, 0.0f );
				continue;
			}

			count++;
		}

		// 절삭 처리 검사
		for( j = 0; j < getMaxIndex(); j++ )
		{
			if( _getBlendWeight( j, i ) <= m_Thresh && _getBlendWeight( j, i ) != 0.0f )
			{
				f = i3Math::div( _getBlendWeight( j, i ), (REAL32)(count) );				

				// 절삭되지 않는 본들의 weight값을 조정해준다.
				for( k = 0; k < getMaxIndex(); k++ )
				{
					if( _getBlendWeight( k, i) > m_Thresh )
						_setBlendWeight( k, i, f + _getBlendWeight( k, i ) );
				}

				// 절삭되는 본의 weight값을 지운다.
				_setBlendIndex( j, i, 0xFF );
				_setBlendWeight( j, i, 0.0f );
			}
		}
	}
}

INT32 i3SkinOptimize::_CheckVertexBlendWithVertexArray( UINT32 gIndex, UINT32 vIndex )
{
	INT32	i, j, k;
	BOOL	bCheck, bGroupCheck;
	UINT8	tempIndex;
	REAL32	tempWeight;
	
	i3VertexBlendBuffer * pVertexBuffer;
	I3PRIMITIVE_GROUPINFO * pGroupInfo = _getOptGroup( gIndex );

	I3ASSERT( pGroupInfo );
	
	// 버택스리스트에서 검사
	for( i = 0; i < _getVertexBlendListCount(); i++ )
	{
		bCheck = FALSE;
		bGroupCheck = FALSE;
		pVertexBuffer = _getVertexBlendBuffer( i );
		I3ASSERT( pVertexBuffer );

		// 같은 버택스만 검사
		if( pVertexBuffer->m_VertexIndex != vIndex )
			continue;

		// 그룹이 같다면 해당 버택스를 넘겨준다.
		if( pVertexBuffer->m_Group == (INT32) gIndex )
			return i;

		for( j = 0; j < getMaxIndex(); j++ )
		{
			if( pVertexBuffer->m_BlendIndex[ j ] == 0xFF )
				continue;

			for( k = 0; k < pGroupInfo->m_BoneMatrixCount; k++ )
			{	
				if( pVertexBuffer->m_BlendIndex[ j ] == pGroupInfo->m_BoneMatrix[ k ] )
				{
					if( j == k )
						bCheck = TRUE;
					else
					{
						// 블랜드 위치가 틀리다
						bCheck = FALSE;
					}

					break;
				}
			}
			if( !bCheck )
				break;
		}

		// 해당 버택스가 동일하다
		if( bCheck )
		{
			if( pVertexBuffer->m_Group == -1 )
				pVertexBuffer->m_Group = gIndex;
			return i;
		}
		else
		{
			// 그룹이 등록되어 있지 않다면 해당 그룹으로 기본 등록 하고 블랜드 값을 바꿔준다.
			if( pVertexBuffer->m_Group == -1 )
			{
	#if 0
				I3TRACE( "Change vertex : %d, group %d { %d, %d, %d, %d }\t->\t",
					vIndex, gIndex, _getBlendIndex( 0, i ), _getBlendIndex( 1, i ), _getBlendIndex( 2, i ), _getBlendIndex( 3, i ) );
	#endif
				for( k = 0; k < pGroupInfo->m_BoneMatrixCount; k++ )
				{
					if( pGroupInfo->m_BoneMatrix[ k ] == 0xFF )
						continue;

					for( j = 0; j < getMaxIndex(); j++ )
					{
						tempIndex = _getBlendIndex( j, i );

						if( tempIndex == 0xFF )
							continue;

						if( tempIndex == pGroupInfo->m_BoneMatrix[ k ] )
						{
							_setBlendIndex( j, i, _getBlendIndex( k, i ) );
							_setBlendIndex( k, i, tempIndex );

							tempWeight = _getBlendWeight( j, i );
							I3ASSERT( tempWeight > 0.0f );
							_setBlendWeight( j, i, _getBlendWeight( k, i ) );
							_setBlendWeight( k, i, tempWeight );
						}
					}
				}

	#if 0
				I3TRACE( " { %d, %d, %d, %d }, { %.2f, %.2f, %.2f, %.2f }\n", 
					_getBlendIndex( 0, i ), _getBlendIndex( 1, i ), _getBlendIndex( 2, i ), _getBlendIndex( 3, i ),
					_getBlendWeight( 0, i ), _getBlendWeight( 1, i ), _getBlendWeight( 2, i ), _getBlendWeight( 3, i ) );
	#endif


				pVertexBuffer->m_Group = gIndex;
				return i;
			}
		}
	}
		
	return -1;
}


void i3SkinOptimize::_ReplaceBlendWeight( I3G_PRIMITIVE PrimType )
{
	INT32	i, j, k;
	UINT8	BlendIndex;
	UINT32	PrimIndex;
	UINT32	vIndex;
	INT32	idx;
#if 0
	INT32	nCount = 0;
#endif;
	i3VertexBlendBuffer * pBuffer;
	INT32 nPrimCount = 0;

	BinList	MatrixIndex;

	I3PRIMITIVE_GROUPINFO * pInfo;

	// 인덱스 카운터 보다 세팅할 수 있는 본이 많으면 위치변환을 해줄 필요가 없다.
	if( getMaxMatrix() > getMaxIndex() )
		return;
	
	
	for( i = 0; i < m_OptGroupList.GetCount(); i++ )
	{	
		pInfo = (I3PRIMITIVE_GROUPINFO * ) m_OptGroupList.Items[ i ];

		I3ASSERT( pInfo );
		
		if( pInfo->m_BoneMatrixCount > getMaxIndex() )
			continue;

		if( PrimType == I3G_PRIM_TRILIST )
		{
			nPrimCount = pInfo->m_PrimCount * 3;
		}
		else if( PrimType == I3G_PRIM_TRISTRIP )
		{
			nPrimCount = pInfo->m_PrimCount;
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "i3SkinOptimize::_ReplaceWeight() - Invalid primitive type. ( %d )", PrimType );
		}

		// 임시 매트릭스 맵을 생성
		MatrixIndex.Clear();
		for( j = 0; j < pInfo->m_BoneMatrixCount; j++ )
			MatrixIndex.Add( (void*) pInfo->m_BoneMatrix[j] );

		for( j = 0; j < nPrimCount; j++ )
		{	
			PrimIndex = (UINT16)(pInfo->m_StartIndex + j);

			vIndex = _getPrimitive( PrimIndex );

			idx = _CheckVertexBlendWithVertexArray( i, vIndex );

			if( idx == -1 )
			{
				// 새로 만들어야 하는 버택스
				pBuffer = (i3VertexBlendBuffer*) i3MemAlloc( sizeof(i3VertexBlendBuffer) );
				
				memset( pBuffer->m_BlendIndex, 0xFF, 4 );
				memset( pBuffer->m_BlendWeight, 0, sizeof(REAL32) * getMaxIndex() );
				
				pBuffer->m_Group = i;
					
				for( k = 0; k < getMaxIndex(); k++ )
				{
					// 본 인덱스를 찾고 해당 위치에 weight를 넣는다.
					BlendIndex = _getBlendIndex( k, vIndex );
					if( BlendIndex == (UINT8) 0xFF )
						continue;
					idx = MatrixIndex.IndexOf( (void*) BlendIndex );
					I3ASSERT( idx != 0xFFFFFFFF );
					
					pBuffer->m_BlendWeight[ idx ] = _getBlendWeight( k, vIndex );
					pBuffer->m_BlendIndex[ idx ] = BlendIndex;
				}

				// 프리미티브 인덱스의 버택스값을 새로 추가한 녀석으로 바꿔준다.
				pBuffer->m_VertexIndex = vIndex;
				_setPrimitive( PrimIndex, (UINT32) _getVertexBlendListCount() );

				// 버택스를 추가
				_addVertexBlendList( pBuffer );

			}
			else
				_setPrimitive( PrimIndex, idx );
		}	// for()
	}
}

INT32 i3SkinOptimize::_FindVertexBlendWhichWithBuffer( UINT8 Blend, I3PRIMITIVE_GROUPINFO * pInfo )
{
	INT32	i;
	UINT8	BoneIndex;

	for( i = 0; i < pInfo->m_BoneMatrixCount; i++ )
	{
		BoneIndex = pInfo->m_BoneMatrix[ i ];

		if( BoneIndex == Blend )
			return i;
	}

	return -1;
}

// 본 팔레트에 따라 버택스의 블랜드인덱스의 값을 바꿔준다.
void i3SkinOptimize::_ReplaceBonePallete( I3G_PRIMITIVE PrimType )
{
	INT32	i, j, k;
	UINT32	idx;
	UINT8	BlendIndex;
	
	UINT32	Prim = 0;

	INT32	Finder;
	I3PRIMITIVE_GROUPINFO* pInfo;
	I3PRIMITIVE_GROUPINFO* pFindGroup;
	i3VertexBlendBuffer * pBuffer;
	i3VertexBlendBuffer * pNewBuffer;

	for( i = 0; i < _getVertexBlendListCount(); i++ )
	{
		pBuffer = _getVertexBlendBuffer( i );
		pBuffer->m_Group = -1;
	}

	for( i = 0; i < m_OptGroupList.GetCount(); i++ )
	{
		pInfo = _getOptGroup( i );
		I3ASSERT( pInfo );

		if( PrimType == I3G_PRIM_TRILIST )
		{
			for( j = 0; j < pInfo->m_PrimCount * 3; j++ )
			{
				idx = pInfo->m_StartIndex + j;
				Prim = _getPrimitive( idx );

				pBuffer = _getVertexBlendBuffer( Prim );

				if( pBuffer->m_Group == -1 )
				{
					// 그룹 설정이 안되어 있다면 해당 그룹
					for( k = 0; k < getMaxIndex(); k++ )
					{
						BlendIndex = _getBlendIndex( k, Prim );
						if( BlendIndex == 0xFF )
							continue;
						
						Finder = _FindVertexBlendWhichWithBuffer( BlendIndex, pInfo );

						I3ASSERT( Finder != -1 );
						_setBlendIndex( k, Prim, (UINT8)Finder );

					}
					pBuffer->m_Group = i;
				}
				else if( pBuffer->m_Group != i )
				{
					// 그룹이 다른 경우 새로 버택스를 만들고 새 그룹에 맞춰 세팅해준다.
					pNewBuffer = (i3VertexBlendBuffer*) i3MemAlloc( sizeof( i3VertexBlendBuffer ) );
					I3ASSERT( pNewBuffer );

					_addVertexBlendList( pNewBuffer );

					pNewBuffer->m_VertexIndex = Prim;
					
					memset( pNewBuffer->m_BlendIndex, 0xFF, sizeof(UINT8) * getMaxIndex() );
					for( k = 0; k < getMaxIndex(); k++ )
					{	
						BlendIndex = _getBlendIndex( k, Prim );
						if( BlendIndex == 0xFF )
							continue;
						pFindGroup = _getOptGroup( pBuffer->m_Group );
						Finder = _FindVertexBlendWhichWithBuffer( pFindGroup->m_BoneMatrix[ BlendIndex ], pInfo );

						I3ASSERT( Finder != -1 );
						_setBlendIndex( k, _getVertexBlendListCount() - 1, (UINT8) Finder );
					}

					memcpy( pNewBuffer->m_BlendWeight, pBuffer->m_BlendWeight, sizeof(REAL32) * getMaxIndex() );

					_setPrimitive( idx, _getVertexBlendListCount() - 1 );

					pNewBuffer->m_Group = i;
				}
			}
		}
		else if( PrimType == I3G_PRIM_TRISTRIP )
		{
			for( j = 0; j < pInfo->m_PrimCount; j++ )
			{
				idx = pInfo->m_StartIndex + j;
				Prim = _getIndexOrder( idx );
				pBuffer = _getVertexBlendBuffer( Prim );

				if( pBuffer->m_Group == -1 )
				{
					for( k = 0; k < getMaxIndex(); k++ )
					{
						if( _getBlendIndex( k, Prim ) == 0xFF )
							continue;
						Finder = _FindVertexBlendWhichWithBuffer( _getBlendIndex( k, Prim ), pInfo );

						I3ASSERT( Finder != -1 );
						_setBlendIndex( k, Prim, (UINT8)Finder );
					}
					pBuffer->m_Group = i;
				}
				else if( pBuffer->m_Group != i )
				{
					// 그룹이 다른 경우 새로 버택스를 만들고 새 그룹에 맞춰 세팅해준다.
					pNewBuffer = (i3VertexBlendBuffer*) i3MemAlloc( sizeof( i3VertexBlendBuffer ) );
					I3ASSERT( pNewBuffer );

					_addVertexBlendList( pNewBuffer );

					pNewBuffer->m_VertexIndex = Prim;

					memset( pNewBuffer->m_BlendIndex, 0xFF, sizeof(UINT8) * getMaxIndex() );
					for( k = 0; k < getMaxIndex(); k++ )
					{	
						BlendIndex = _getBlendIndex( k, Prim );
						if( BlendIndex == 0xFF )
							continue;
						pFindGroup = _getOptGroup( pBuffer->m_Group );
						Finder = _FindVertexBlendWhichWithBuffer( pFindGroup->m_BoneMatrix[ BlendIndex ], pInfo );

						I3ASSERT( Finder != -1 );
						_setBlendIndex( k, _getVertexBlendListCount() - 1, (UINT8) Finder );
					}

					memcpy( pNewBuffer->m_BlendWeight, pBuffer->m_BlendWeight, sizeof(REAL32) * getMaxIndex() );

					_setIndexOrder( idx, _getVertexBlendListCount() - 1 );

					pNewBuffer->m_Group = i;
				}
			}
		}
	}
}

void i3SkinOptimize::OptimizeTriList( void )
{
	UINT32	i;
	INT32	j;
	UINT32	vi1, vi2, vi3;
	INT32	idx;
	UINT8	BlendIndex;
	INT32	nCount;
	INT32	startIndex;
	BOOL	bCheck;

	List	MatrixMap;
	List	BlendMap;
	List	tempBlendMap;

	I3PRIMITIVE_GROUPINFO * pGroupArray;

	startIndex = 0;
	nCount = 0;
	bCheck = FALSE;

	for( i = 0; i < (UINT32) (_getPrimitiveCount() / 3); i++ )
	{
		vi1 = _getPrimitive( i*3 );
		vi2 = _getPrimitive( i*3+1 );
		vi3 = _getPrimitive( i*3+2 );
		
		// 기본 매트릭스맵을 만든다.
		MatrixMap.Clear();
		for( j = 0; j < getMaxIndex(); j++ )
		{
			BlendIndex = _getBlendIndex( j, vi1 );
			if( BlendIndex == 0xFF )
				continue;

			if( MatrixMap.IndexOf( (void*) BlendIndex ) == 0xFFFFFFFF )
				MatrixMap.Add( (void*) BlendIndex );
		}

		// 공유하는 본이나 매트릭스맵에 추가 가능한지 여부 판단
		if( _CheckBoneMatrix( &MatrixMap, vi2 ) )
		{
			if( _CheckBoneMatrix( &MatrixMap, vi3 ) )
			{
				nCount++;
				bCheck = FALSE;

				// 임시 매트릭스맵을 만든다.
				tempBlendMap.Clear();
				for( j = 0; j < BlendMap.GetCount(); j++ )
					tempBlendMap.Add( BlendMap.Items[ j ] );
				
				// 블랜드맵에 현재 매트릭스 맵이 추가 가능한지 판단
				for( j = 0; j < MatrixMap.GetCount(); j++ )
				{
					idx = BlendMap.IndexOf( MatrixMap.Items[ j ] );
					if( idx == 0xFFFFFFFF )
					{	
						if( BlendMap.GetCount() >= getMaxMatrix() )
						{
							// 추가할 수 없다
							bCheck = TRUE;
							break;
						}
						else
						{
							// 없다면 블랜드맵에 추가
							BlendMap.Add( MatrixMap.Items[ j ] );
						}
					}
				}

				// 매트릭스맵 리스트에 없으면 추가
				if( bCheck )
				{	
					pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
					I3ASSERT( pGroupArray );
					
					// 인덱스 리스트를 저장해둔다.
					pGroupArray->m_StartIndex = startIndex;
					pGroupArray->m_PrimCount = nCount-1;
					pGroupArray->m_BoneMatrixCount = tempBlendMap.GetCount();
					
					for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
						pGroupArray->m_BoneMatrix[ j ] = (UINT8) tempBlendMap.Items[ j ];
					
					// 그룹에 추가
					m_OptGroupList.Add( pGroupArray );

					startIndex = i*3;
					i--;

					nCount = 0;
					BlendMap.Clear();
				}
			}
			else
			{	
				// 옵티마이즈 불가
#if defined( I3_DEBUG )
				I3TRACE("UnOptList : %d\n", i*3 );
#endif
				// 이전 리스트까지 그룹해준다.
				if( BlendMap.GetCount() > 0 )
				{
					pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
					I3ASSERT( pGroupArray );

					// 인덱스 리스트를 저장해둔다.
					pGroupArray->m_StartIndex = startIndex;
					pGroupArray->m_PrimCount = nCount-1;
					pGroupArray->m_BoneMatrixCount = BlendMap.GetCount();
					
					for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
						pGroupArray->m_BoneMatrix[ j ] = (UINT8) BlendMap.Items[ j ];

					// 그룹에 추가
					m_OptGroupList.Add( pGroupArray );

					nCount = 0;
					BlendMap.Clear();
				}

				// 옵티마이즈 안되는 녀석을 넣어준다.
				pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
				I3ASSERT( pGroupArray );

				pGroupArray->m_StartIndex = i*3;
				pGroupArray->m_PrimCount = 1;
				pGroupArray->m_BoneMatrixCount = MatrixMap.GetCount();
				
				for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
					pGroupArray->m_BoneMatrix[ j ] = (UINT8) MatrixMap.Items[ j ];

				startIndex = (i + 1) * 3;
			}
		}
		else
		{
			// 옵티마이즈 불가
#if defined( I3_DEBUG )
			I3TRACE( "UnOptList : %d\n", i*3 );
#endif
			if( BlendMap.GetCount() > 0 )
			{
				pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
				I3ASSERT( pGroupArray );

				// 인덱스 리스트를 저장해둔다.
				pGroupArray->m_StartIndex = startIndex;
				pGroupArray->m_PrimCount = nCount-1;
				pGroupArray->m_BoneMatrixCount = BlendMap.GetCount();
				
				for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
					pGroupArray->m_BoneMatrix[ j ] = (UINT8) BlendMap.Items[ j ];

				// 그룹에 추가
				m_OptGroupList.Add( pGroupArray );

				nCount = 0;
				BlendMap.Clear();
			}

			// 옵티마이즈 안되는 녀석을 넣어준다.
			pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
			I3ASSERT( pGroupArray );

			pGroupArray->m_StartIndex = i*3;
			pGroupArray->m_PrimCount = 1;

			// 나머지 버택스에 대한 매트릭스도 넣어줘야된다.
			_CheckBoneMatrix( &MatrixMap, vi3 );

			pGroupArray->m_BoneMatrixCount = MatrixMap.GetCount();
			
			for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
				pGroupArray->m_BoneMatrix[ j ] = (UINT8) MatrixMap.Items[ j ];

			startIndex = (i + 1) * 3;
		}
	}

	if( BlendMap.GetCount() > 0 )
	{
		pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
		I3ASSERT( pGroupArray );

		pGroupArray->m_StartIndex = startIndex;
		pGroupArray->m_PrimCount = nCount;
		pGroupArray->m_BoneMatrixCount = BlendMap.GetCount();
		
		for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
			pGroupArray->m_BoneMatrix[ j ] = (UINT8) BlendMap.Items[ j ];
		
		// 그룹에 추가
		m_OptGroupList.Add( pGroupArray );
	}
}

void i3SkinOptimize::OptimizeTriStrip( void )
{
	UINT32	i;
	INT32	j;
	UINT32	idx;			// 버택스 인덱스
	UINT32	startIndex;
	INT32	nCount;
	INT32	startCount;
	UINT32	tempIndex1, tempIndex2;

	List	VertexMap;
	List	PrimOrder;
	List	MatrixMap;
	List	tempMap;
	
	I3PRIMITIVE_GROUPINFO * pGroupArray;

	startIndex = 0;
	nCount = 0;
	startCount = 0;
	
	for( i = 0; i < (UINT32) _getPrimitiveCount() ; i++ )
	{
		idx = _getPrimitive( i );
		
		nCount++;
		startCount++;

		// 현재 매트릭스맵을 버퍼에 담아둔다.
		tempMap.Clear();
		tempMap.SetCount( MatrixMap.GetCount() );
		for( j = 0; j < MatrixMap.GetCount(); j++ )
			tempMap.SetItem( j, (void*) MatrixMap.Items[ j ] );

		// 같은 본이 있는지 체크하여 없으면 추가
		if( !_CheckBoneMatrix( &MatrixMap, idx ) )
		{	
			// 매트릭스맵에 추가할 공간이 없는 경우
			// 매트릭스 맵을 리스트에 넣는다.
			pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof(I3PRIMITIVE_GROUPINFO) );
			I3ASSERT( pGroupArray );

			pGroupArray->m_StartIndex = startIndex;
			
			if( startCount > 3 )
			{
				pGroupArray->m_BoneMatrixCount = tempMap.GetCount();

				for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
					pGroupArray->m_BoneMatrix[ j ] = (UINT8) tempMap.Items[ j ];
				
	#if 0
				I3TRACE( "Divide by Bone : %d\n", i );
	#endif		
				startIndex = _getIndexOrderCount();
					
				tempIndex1 = _getPrimitive( i - 2 );
				tempIndex2 = _getPrimitive( i - 1 );

				if( tempIndex1 == tempIndex2 || tempIndex1 == idx || tempIndex2 == idx )
				{
					if( i & 1 )
					{
						pGroupArray->m_PrimCount = nCount - 2;
						i -= 2;
						nCount = 0;
					}
					else
					{
						pGroupArray->m_PrimCount = nCount - 2;
						i -= 3;
						nCount = 0;
					}
				}
				else
				{
					if( i & 1 )
					{
						pGroupArray->m_PrimCount = nCount - 1;
						_addIndexOrder( _getPrimitive( i - 2 ) );
						i -= 3;
						nCount = 1;
					}
					else
					{
						pGroupArray->m_PrimCount = nCount - 1;
						i -= 3;
						nCount = 0;
					}
				}
			}
			else
			{	
				// 옵티마이즈가 안되는 경우
#if defined( I3_DEBUG )
				I3TRACE( "none Optimize strip : %d, nCount : %d, sCount : %d\n", i, nCount, startCount );
#endif
				pGroupArray->m_PrimCount = 3 + nCount - startCount;
				_addIndexOrder( idx );

				for( j = 0; j < 3 - startCount; j++ )
				{
					idx = _getPrimitive( i + j + 1 );
					_CheckBoneMatrix( &MatrixMap, idx );
					_addIndexOrder( idx );
				}
				
				pGroupArray->m_BoneMatrixCount = MatrixMap.GetCount();

				for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
					pGroupArray->m_BoneMatrix[ j ] = (UINT8) MatrixMap.Items[ j ];

				startIndex = _getIndexOrderCount();
				
				if( i & 1 )
				{	
					i -= startCount - 2;
					_addIndexOrder( _getPrimitive( i ) ) ;
					i--;
					nCount = 1;
				}
				else
				{
					i -= startCount - 3;
					nCount = 0;
				}
			}

			startCount = 0;

			m_OptGroupList.Add( pGroupArray );
			
			MatrixMap.Clear();

			continue;
		}

		_addIndexOrder( idx );
	}

	if( MatrixMap.GetCount() > 0 )
	{
		pGroupArray = (I3PRIMITIVE_GROUPINFO*) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
		I3ASSERT( pGroupArray );

		pGroupArray->m_StartIndex = startIndex;
		pGroupArray->m_PrimCount = nCount ;
		pGroupArray->m_BoneMatrixCount = MatrixMap.GetCount();

		for( j = 0; j < pGroupArray->m_BoneMatrixCount; j++ )
			pGroupArray->m_BoneMatrix[ j ] = (UINT8)(MatrixMap.Items[ j ]);
		
		m_OptGroupList.Add( pGroupArray );
	}
}

void i3SkinOptimize::GetVertexArray( i3VertexArray * pSrcVA, i3VertexArray * pDestVA )
{
	UINT32		i;
	INT32		j;
	INT8		tempBlendIndexCount;
	VEC3D		Vec;
	COLORREAL	Color;
	VEC2D		Vec2;

	UINT8		bone;

	i3VertexFormat * pFormat;

	pFormat = pSrcVA->GetFormat();
	tempBlendIndexCount = pFormat->GetBlendIndexCount();

	if( getMaxMatrix() <= getMaxIndex() )
	{
		pFormat->SetBlendIndexCount( (INT8) getMaxIndex() );
		pFormat->SetBlendWeightCount( (INT8) getMaxIndex() - 1 );
	}

	pDestVA->Create( pFormat, (UINT32) _getVertexBlendListCount(), pSrcVA->GetUsageFlag() );

	pFormat->SetBlendIndexCount( tempBlendIndexCount );
	pFormat->SetBlendWeightCount( tempBlendIndexCount - 1);

	pSrcVA->Lock();
	pDestVA->Lock();

	if( pSrcVA->HasPosition() )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
		{
			pSrcVA->GetPosition( _getVertexIndex( i ), &Vec );
			pDestVA->SetPosition( i, &Vec );
		}
	}

	if( pSrcVA->HasNormal() )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
		{
			pSrcVA->GetNormal( _getVertexIndex( i ), &Vec );
			pDestVA->SetNormal( i, &Vec );
		}
	}

	if( pSrcVA->HasColor() )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
		{
			pSrcVA->GetColor( _getVertexIndex( i ), &Color);
			pDestVA->SetColor( i, &Color );
		}
	}
	
	for( j = 0; j < (INT32)pFormat->GetTextureCoordSetCount(); j++ )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
		{
			pSrcVA->GetTextureCoord( j, _getVertexIndex( i ), &Vec2 );
			pDestVA->SetTextureCoord( j, i, &Vec2 );
		}
	}

	for( j = 0; j < pDestVA->GetBlendIndexCount(); j++ )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
		{
			bone = _getBlendIndex( j, i );
			if( bone == 0xFF )
				bone = 0;
			pDestVA->SetBlendIndex( j, i, bone );
		}
	}

	for( j = 0; j < pDestVA->GetBlendWeightCount(); j++ )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
			pDestVA->SetBlendWeight( j, i, _getBlendWeight( j, i ) );
	}

	if( pSrcVA->HasTangent() )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
		{
			pSrcVA->GetTangent( _getVertexIndex( i ), &Vec );
			pDestVA->SetTangent( i, &Vec );
		}
	}

	if( pSrcVA->HasBinormal() )
	{
		for( i = 0; i < pDestVA->GetCount(); i++ )
		{
			pSrcVA->GetBinormal( _getVertexIndex( i ), &Vec );
			pDestVA->SetBinormal( i, &Vec );
		}
	}

	pSrcVA->Unlock();
	pDestVA->Unlock();
}

void i3SkinOptimize::GetIndexArray( i3IndexArray * pSrcIA, i3IndexArray * pDestIA )
{
	UINT32	i;

	if( pSrcIA )
	{
		if( _getIndexOrderCount() > 0 )
			pDestIA->Create( _getIndexOrderCount(), pSrcIA->GetUsageFlag() );
		else
			pDestIA->Create( _getPrimitiveCount(), pSrcIA->GetUsageFlag() );

		pDestIA->Lock();

		if( _getIndexOrderCount() > 0  )
		{
			for( i = 0; i < pDestIA->GetCount(); i++ )
				pDestIA->SetIndex( i, _getIndexOrder( i ) );
		}
		else
		{
			for( i = 0; i < pDestIA->GetCount(); i++ )
				pDestIA->SetIndex( i, _getPrimitive( i ) );
		}

		
		pDestIA->Unlock();
	}
}

void i3SkinOptimize::GetOptimizedData( List * pList )
{
	UINT32	i;
	
	I3PRIMITIVE_GROUPINFO * pGroupData;
	I3PRIMITIVE_GROUPINFO * pInfo;
	INT32	nCount = m_OptGroupList.GetCount();

	I3ASSERT( pList );

	pList->SetCount( nCount );

	// 그룹 인포를 저장한다.
	for( i = 0; i < (UINT32) m_OptGroupList.GetCount(); i++ )
	{
		pInfo = (I3PRIMITIVE_GROUPINFO * ) m_OptGroupList.Items[ i ];
		I3ASSERT( pInfo );

		// 저장할 데이터
		pGroupData = (I3PRIMITIVE_GROUPINFO *) i3MemAlloc( sizeof( I3PRIMITIVE_GROUPINFO ) );
		I3ASSERT( pGroupData );
		pGroupData->m_StartIndex		= pInfo->m_StartIndex;
		pGroupData->m_PrimCount			= pInfo->m_PrimCount;
		pGroupData->m_BoneMatrixCount	= pInfo->m_BoneMatrixCount;

		I3ASSERT( pGroupData->m_BoneMatrixCount > 0 );
		// 본 매트릭스들을 저장
		memcpy( pGroupData->m_BoneMatrix, pInfo->m_BoneMatrix, sizeof(UINT8) * pGroupData->m_BoneMatrixCount );

		pList->SetItem( i, pGroupData );
	}
}

// 인덱스가 Primitive에 Optimize되어 있다.
// 각 조건에서 해당되면 그 버텍스의 웨이트값을 리스트 수에 맞춰 조절한다.
// 매트릭스맵 리스트는 그래픽카드가 지원하는 MaxVertexBlendMatrices에 따라 채워준다.
BOOL i3SkinOptimize::BuildOptimizedData( I3G_PRIMITIVE type, i3VertexArray * pVA, i3IndexArray * pIA, INT32 BlendIndexCount, INT32 BoneMatrixCount, INT32 nPrimCount )
{
	if( pVA->GetBlendIndexCount() == 0)
		return FALSE;				// 스키닝이 아니다.

	if( pVA->GetBlendIndexCount() > BlendIndexCount )
		return FALSE;				// 실질 데이터가 더 큼으로 옵티마이즈시 데이터 손실이 발생하여 소프트블랜딩을 한다.

	if( BlendIndexCount < 2 )
		return FALSE;				// 옵티마이즈가 아니다.

	setMaxIndex( BlendIndexCount );
	if( BoneMatrixCount > BlendIndexCount )
		setMaxMatrix( BoneMatrixCount );
	else
		setMaxMatrix( BlendIndexCount );

	// 일단 초기화
	_RemoveAllOptGroupList();
	_RemoveAllPrimOrder();
	_RemoveAllVertexBlendList();
	_RemoveAllIndexOrder();

	pVA->Lock();

	// 실제 그려지는 버택스의 순서에 따라 배열한다.
	_BuildPrimOrder( pVA, pIA, nPrimCount);

	// 임시 버퍼 생성( 버택스를 복사해둔다... )
	{
		_CreateVertexBlendBuffer( pVA->GetCount() );
		// 블랜드 인덱스맵을 만든다.
		_BuildBlendIndexOrder( pVA );
		// 블랜드 웨이트맵을 만든다.
		_BuildBlendWeightOrder( pVA );
	}

	pVA->Unlock();

	// Bone weight 절삭값 처리를 한다.
	if( getType() & I3_SKINNING_OPT_CUTWEIGHT )
		_CalcThreshWeight( );

	// 3 버택스에 대한 매트릭스맵 세팅 및 인덱스 그룹 생성
	if( type == I3G_PRIM_TRILIST )
		OptimizeTriList( );
	else if( type == I3G_PRIM_TRISTRIP )
		OptimizeTriStrip( );
	
	// 버텍스의 본 매트릭스 및 웨이트를 매트릭스 맵의 데이터로 바꿔준다.
	_ReplaceBlendWeight( type );

	// 인덱스를 그룹에 맞춰 바꿔준다.
	_ReplaceBonePallete( type );

#if defined( I3_DEBUG )
	_CheckWeightDump( type );
#endif

	return TRUE;
}

UINT32 i3SkinOptimize::OnSave( i3ResourceFile * pResFile )
{
	INT32 Rc, Result = 0;
	INT32 i;
	i3Stream * pStream = pResFile->GetStream();

	I3PRIMITIVE_GROUPINFO * pGroupInfo;

	I3PERSIST_PRIMITIVEINFO Info;

	memcpy( Info.m_ID, "VBGQ", 4 );
	Info.m_GroupCount	= m_OptGroupList.GetCount();

	Rc = pStream->Write( &Info, sizeof( I3PERSIST_PRIMITIVEINFO ) );
	if( Rc == STREAM_ERR )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write Skinning Info.");
		return STREAM_ERR;
	}
	Result += Rc;

	I3PRIMITIVE_GROUPINFO Group;
	for( i = 0; m_OptGroupList.GetCount(); i++ )
	{
		pGroupInfo = (I3PRIMITIVE_GROUPINFO*) m_OptGroupList.Items[ i ];

		Group.m_StartIndex				= pGroupInfo->m_StartIndex;
		Group.m_PrimCount				= pGroupInfo->m_PrimCount;
		Group.m_BoneMatrixCount		= pGroupInfo->m_BoneMatrixCount;

		memcpy( Group.m_BoneMatrix, pGroupInfo->m_BoneMatrix, sizeof( UINT8 ) * pGroupInfo->m_BoneMatrixCount );

		Rc = pStream->Write( &Group, sizeof( I3PRIMITIVE_GROUPINFO ) );
		if( Rc == STREAM_ERR )
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write Skinning group info." );
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3SkinOptimize::OnLoad( i3ResourceFile * pResFile )
{
	INT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	I3PERSIST_PRIMITIVEINFO		Info;

	
	Rc = pStream->Read( &Info, sizeof( I3PERSIST_PRIMITIVEINFO ) );
	if( Rc == STREAM_ERR )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read skinning info." );
		return STREAM_ERR;
	}
	Result += Rc;

	if( i3String::Compare( (const char*)Info.m_ID, "VBGQ" ) )
	{
		I3PRINTLOG(I3LOG_FATAL, "Invalid file.");
		return STREAM_ERR;
	}
	
	return Result;
}

BOOL i3SkinOptimize::_CheckWeightDump( I3G_PRIMITIVE PrimType )
{
	INT32 i, j, k, idx;
	UINT32 Prim;
	UINT8 BlendIndex;
	REAL32 TotalBlendWeight;

	I3PRIMITIVE_GROUPINFO * pInfo;
	i3VertexBlendBuffer * pBuffer;

	INT32 nPrimCount = 0;

	
	for( i = 0; i < m_OptGroupList.GetCount(); i++ )
	{
		pInfo = _getOptGroup( i );
		I3ASSERT( pInfo );

		if( PrimType == I3G_PRIM_TRILIST )
		{
			nPrimCount = pInfo->m_PrimCount * 3;
		}
		else if( PrimType == I3G_PRIM_TRISTRIP )
		{
			nPrimCount = pInfo->m_PrimCount;
		}

		for( j = 0; j < nPrimCount; j++ )
		{
			idx = pInfo->m_StartIndex + j;
			Prim = _getPrimitive( idx );

			pBuffer = _getVertexBlendBuffer( Prim );

			TotalBlendWeight	= 0.f;

			for( k = 0; k < getMaxIndex(); k++ )
			{
				BlendIndex = _getBlendIndex( k, Prim );
				if( BlendIndex == 0xFF )
					continue;
				TotalBlendWeight += pBuffer->m_BlendWeight[ k ];
			}

			if( k > 1)
			{
				pBuffer->m_BlendWeight[ k - 1] += 1.0f - TotalBlendWeight;
			}
		}
	}

#if defined( I3_DEBUG) && defined( I3_DUMP_SKINOPT )
	Dump();
#endif
		

	return TRUE;
}


#if defined( I3_DEBUG )
void i3SkinOptimize::Dump( void )
{
	INT32	i, j;
	I3PRIMITIVE_GROUPINFO * pInfo;
	INT32	tOptCount = 0;
	INT32	tOptGroupCount = 0;

	I3TRACE("====================================================================================================\n");
	I3TRACE("=										Optimize Vertex Skinning									  \n");
	I3TRACE("====================================================================================================\n");
	if( !(getType() & I3_SKINNING_OPT_REGULAR) )
		I3TRACE("== Optimize None.\n");
	else
	{
		I3TRACE("Optimize level : ");
		if( getType() & I3_SKINNING_OPT_CUTWEIGHT )
			I3TRACE("Cut weight,");

		I3TRACE("\n");
	}

	for( i = 0; i < m_OptGroupList.GetCount(); i++)
	{
		pInfo = ( I3PRIMITIVE_GROUPINFO* ) m_OptGroupList.Items[ i ];
		if( pInfo )
		{
			tOptGroupCount++;
			tOptCount += pInfo->m_PrimCount;
		}
	}

	// Mapping
	I3TRACE("==------------------------------------------------------------------------------------------------==\n");
	I3TRACE("==\tGroup .no\t\tMatrix Index\t\tStart Index\t\tIndex Count\t\n");
	for( i = 0; i < m_OptGroupList.GetCount(); i++)
	{
		pInfo = ( I3PRIMITIVE_GROUPINFO* ) m_OptGroupList.Items[ i ];

		if( pInfo == nullptr )
			continue;

		I3TRACE("==\t\t%d\t\t\t{", i);
		
		for( j = 0; j < pInfo->m_BoneMatrixCount; j++ )
		{
			I3TRACE("%d,\t", pInfo->m_BoneMatrix[ j ] );
		}

		I3TRACE("}\t%d\t\t%d\t\t==\n", pInfo->m_StartIndex, pInfo->m_PrimCount );

		tOptCount++;
	}

	I3TRACE("==------------------------------------------------------------------------------------------------==\n");
	I3TRACE("== Optimized Group Count : %d\n", tOptGroupCount );

	I3TRACE("====================================================================================================\n");
}
#endif

*/