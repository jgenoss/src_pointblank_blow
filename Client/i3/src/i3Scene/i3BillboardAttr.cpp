#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3BillboardAttr.h"

I3_CLASS_INSTANCE( i3BillboardAttr);

#define			UPDATE_BLOCK_UNIT	512

#if defined( I3G_PSP)
	#define			INDEXED_IMPLEMENT	0
#else
	#define			INDEXED_IMPLEMENT	1
#endif

#define		BILLBOARD_LOCK			(I3G_LOCK_NOOVERWRITE)

i3BillboardAttr::i3BillboardAttr(void)
{
	SetID( I3_ATTRID_BILLBOARD);
}

i3BillboardAttr::~i3BillboardAttr(void)
{
	I3_SAFE_RELEASE( m_pVA );
	I3_SAFE_RELEASE( m_pIA );
	I3MEM_SAFE_FREE_POINTER( m_pBillboards ); 
}

bool i3BillboardAttr::Create( UINT32 Count, I3SG_BILLBOARD_SPACE Space, bool bUV, bool bColor, bool bAlwaysUpdate)
{
	m_Count = Count;
	m_Space = Space;
	m_bAlwaysUpdate = true;

	// Vertex format
	{
		m_Format.SetHasPosition( true);
		m_Format.SetHasColor(bColor);
		
		if( bUV)
		{
			m_Format.SetTextureCoordSetCount( 1);
			m_Format.SetTextureCoordCount( 2);
		}

		if( Space == I3SG_BILLBOARD_SPACE_SCREEN)
			m_Format.SetIsTransformed( true);
	}

	// Billboard Information
	{
		UINT32 i;		

		m_pBillboards	= (I3SG_BILLBOARD_INFO *) i3MemAlloc( sizeof(I3SG_BILLBOARD_INFO) * m_Count);

		I3ASSERT( m_pBillboards != nullptr);

		for( i = 0; i < m_Count; i++)
		{
			m_pBillboards[i].m_bEnable = false;
			m_pBillboards[i].m_EditMask = I3SG_BILLBOARD_EDIT_ENABLE;
		}
	}

	_CreateVertex();

	m_bEditted = true;

	return true;
}

void i3BillboardAttr::_CreateVertex(void)
{
	m_pVA = i3VertexArray::new_object();

	if( m_bAlwaysUpdate)
	{
		// 항상 Update를 하는 동작 모드에서는 Vertex Buffer의 크기를 단위 Block의 크기의 2배를
		// 준비해, Double Buffer 방식으로 Update 및 Rendering을 한다.

		#if INDEXED_IMPLEMENT
			// Vertex Array
			{
				// 한 Billboard 당 4개의 Vertex를 필요로 한다.
				m_pVA->Create( &m_Format, (m_Count << 2), (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE) );
			}

			// Index Array
			{
				m_pIA = i3IndexArray::new_object();
				m_pIA->setRestoreType( I3G_RESTORE_MEM);

				if( (m_Count << 2) > 0xFFFF)
				{
					m_pIA->Create( (m_Count * 6) , (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE), I3G_IDX_32BITS);
				}
				else
				{
					m_pIA->Create( (m_Count * 6) , (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE), I3G_IDX_16BITS);
				}
			}
		#else
			m_pVA->Create( &m_Format, (m_Count * 6), (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
		#endif
	}
	else
	{
		// 변경된 경우에만 Update하는 동작 모드에서는 Vertex Buffer를 Billboard의 개수만큼
		// 미리 준비해둔다.

		#if INDEXED_IMPLEMENT		
			// Vertex Array
			{
				m_pVA->Create( &m_Format, m_Count << 2, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			}

			// Index Array
			{
				m_pIA = i3IndexArray::new_object();
				m_pIA->setRestoreType( I3G_RESTORE_MEM);

				if( (m_Count << 2) > 0xFFFF)
				{
					m_pIA->Create( m_Count * 6, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE), I3G_IDX_32BITS);
				}
				else
				{
					m_pIA->Create( m_Count * 6, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE), I3G_IDX_16BITS);
				}
			}
		#else
			m_pVA->Create( &m_Format, m_Count * 6, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
		#endif
	}

}

inline
void _setPositions( i3VertexArray * pVA, I3SG_BILLBOARD_INFO * pBillboard, UINT32 vIdx, VEC3D* Ay)
{
	REAL32 cy;
	VEC3D By;

	cy = i3Math::Mul( pBillboard->m_Height, 0.5f);
	i3Vector::Scale( &By, Ay, cy);

	#if INDEXED_IMPLEMENT
		VEC3D _pos[4];

		//   [0]           [1]
		//   +-------------+
		//   |             |
		//   +c            |
		//   |             |
		//   +-------------+
		//   [2]           [3]

		// 0 2 1 - 1 2 3
		
		i3Vector::Sub( &_pos[0], & pBillboard->m_Start,	&By);
		i3Vector::Sub( &_pos[1], & pBillboard->m_End,	&By);
		i3Vector::Add( &_pos[2], & pBillboard->m_Start,	&By);
		i3Vector::Add( &_pos[3], & pBillboard->m_End,	&By);

		pVA->SetPositions( vIdx, 4, _pos);
	#else
		VEC3D _pos[6];

		i3Vector::Sub( &_pos[0], & pBillboard->m_Start,	&By);
		i3Vector::Add( &_pos[1], & pBillboard->m_Start,	&By);
		i3Vector::Sub( &_pos[2], & pBillboard->m_End,	&By);
		i3Vector::Sub( &_pos[3], & pBillboard->m_End,	&By);
		i3Vector::Add( &_pos[4], & pBillboard->m_Start,	&By);
		i3Vector::Add( &_pos[5], & pBillboard->m_End,	&By);

		pVA->SetPositions( vIdx, 6, _pos);
	#endif
}

inline
void _setUV( i3VertexArray * pVA, I3SG_BILLBOARD_INFO * pBillboard, UINT32 vIdx, I3SG_BILLBOARD_SPACE space, bool bRotateUV)
{
	REAL32 u1, v1, u2, v2;

	u1 = i3Vector::GetU( &pBillboard->m_UV[0]);
	v1 = i3Vector::GetV( &pBillboard->m_UV[0]);
	u2 = i3Vector::GetU( &pBillboard->m_UV[1]);
	v2 = i3Vector::GetV( &pBillboard->m_UV[1]);

	#if defined( I3G_DX) || defined( I3G_XBOX)
	// BUG
	if( space == I3SG_BILLBOARD_SPACE_SCREEN)
	{
		REAL32 swap = v1;
		v1 = v2;
		v2 = swap;
	}
	#endif

	#if INDEXED_IMPLEMENT
	{
		VEC2D _uv[4];

		if( bRotateUV)
		{
			i3Vector::Set( &_uv[0], u1, v2);
			i3Vector::Set( &_uv[1], u2, v2);
			i3Vector::Set( &_uv[2], u1, v1);
			i3Vector::Set( &_uv[3], u2, v1);
		}
		else
		{
			i3Vector::Set( &_uv[0], u1, v2);
			i3Vector::Set( &_uv[1], u1, v1);
			i3Vector::Set( &_uv[2], u2, v2);
			i3Vector::Set( &_uv[3], u2, v1);
		}

		pVA->SetTextureCoords( 0, vIdx, 4, _uv);
	}
	#else
	{
		VEC2D _uv[6];

		i3Vector::Set( &_uv[0], u1, v2);
		i3Vector::Set( &_uv[1], u2, v2);
		i3Vector::Set( &_uv[2], u1, v1);
		i3Vector::Set( &_uv[3], u1, v1);
		i3Vector::Set( &_uv[4], u2, v2);
		i3Vector::Set( &_uv[5], u2, v1);

		pVA->SetTextureCoords( 0, vIdx, 6, _uv);
	}
	#endif
}

inline
void _setColor( i3VertexArray * pVA, I3SG_BILLBOARD_INFO * pBillboard, UINT32 vIdx)
{
	#if INDEXED_IMPLEMENT
		pVA->SetColor( vIdx, 4, &pBillboard->m_Color);
	#else
		pVA->SetColor( vIdx, 6, &pBillboard->m_Color);
	#endif
}

void i3BillboardAttr::ApplyAlwaysUpdate( i3RenderContext * pContext)
{
	UINT32 primcount, i;
	UINT32 vLockStart, vIdx;
	I3SG_BILLBOARD_INFO * pBillboard;
	bool bLocked = false;

	pBillboard = m_pBillboards;

	primcount = 0;
	vLockStart = 0;
	vIdx = 0;

	#if INDEXED_IMPLEMENT
		UINT32 iLockStart, iIdx;
		UINT16	Indices[6];

		iLockStart = 0;
		iIdx = 0;	
	#endif

	for( i = 0; i < m_Count; i++, pBillboard++)
	{
		if( pBillboard->m_bEnable == false)
			continue;

		if( bLocked == false)
		{
			UINT32 count = MIN( m_Count, m_Count - i);

			#if INDEXED_IMPLEMENT
				m_pVA->Lock( BILLBOARD_LOCK, vLockStart, count * 4 );
				m_pIA->Lock( iLockStart, count * 6, BILLBOARD_LOCK);
			#else
				m_pVA->Lock( BILLBOARD_LOCK, vLockStart, count * 6);
			#endif

			bLocked = true;
		}

		// 임의의 축과 카메라축과 고정
		VEC3D vec, dir;
		{
			i3Vector::Sub( &dir, &pBillboard->m_End, &pBillboard->m_Start);
			if( i3Vector::LengthSq(&dir) > I3_EPS)
			{
				i3Vector::Normalize( &dir, &dir);
				i3Vector::Cross( &vec, &dir, &m_InvViewAt);
				i3Vector::Normalize( &vec, &vec);
			}

			_setPositions( m_pVA, pBillboard, vIdx, &vec);
		}
		

		// UV
		if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			_setUV( m_pVA, pBillboard, vIdx, m_Space, m_bRotateUV);
		}

		// Color
		if( m_pVA->GetFormat()->GetHasColor())
			_setColor( m_pVA, pBillboard, vIdx);

		// Index
		#if INDEXED_IMPLEMENT
		{
			Indices[0] = (UINT16)(vIdx);
			Indices[1] = (UINT16)(vIdx + 2);
			Indices[2] = (UINT16)(vIdx + 1);
			Indices[3] = (UINT16)(vIdx + 1);
			Indices[4] = (UINT16)(vIdx + 2);
			Indices[5] = (UINT16)(vIdx + 3);

			m_pIA->SetIndices( iIdx, 6, Indices);
		}
		#endif

		
		primcount ++;

		#if INDEXED_IMPLEMENT
			vIdx += 4;
			iIdx += 6;
		#else
			vIdx += 6;
		#endif

		if( primcount >= m_Count)
		{
			m_pVA->Unlock();
			pContext->SetVertexArray( m_pVA);

			#if INDEXED_IMPLEMENT
				m_pIA->Unlock();
				pContext->SetIndexArray( m_pIA);
				m_pShader = pContext->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, iLockStart, primcount << 1);
			#else
				m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, vLockStart, primcount << 1);
			#endif

			primcount = 0;
			bLocked = false;

			if( vIdx >= m_pVA->GetCount())
			{
				vLockStart = 0;
				vIdx = 0;
				#if INDEXED_IMPLEMENT
					iLockStart = 0;
					iIdx = 0;
				#endif
			}
			else
			{
				vLockStart = vIdx;
				#if INDEXED_IMPLEMENT
					iLockStart = iIdx;
				#endif
			}
		}
	}

	// Render rest of Billboards
	if( primcount > 0)
	{
		if( bLocked)
			m_pVA->Unlock();

		pContext->SetVertexArray( m_pVA);

		#if INDEXED_IMPLEMENT
			if( bLocked)
				m_pIA->Unlock();

			pContext->SetIndexArray( m_pIA);
			m_pShader = pContext->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, iLockStart, primcount << 1);
		#else
			m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, vLockStart, primcount << 1);
		#endif
	}


}

void i3BillboardAttr::ApplyEdittedUpdate( i3RenderContext * pContext)
{
	UINT32 i, vIdx;
	I3SG_BILLBOARD_INFO * pBillboard = m_pBillboards;
	bool bLocked = false;

	#if INDEXED_IMPLEMENT
		UINT16	Indices[6];
		UINT32 iIdx;
	#endif

	if(m_Count <= 0)
		return;

	if( m_bEditted)
	{
		m_PrimCount = 0;
		vIdx = 0;
		iIdx = 0;

		for( i = 0; i < m_Count; i++, pBillboard++)
		{
			if( pBillboard->m_bEnable == false)
				continue;

			if( !bLocked)
			{
				bLocked = true;

				m_pVA->Lock( BILLBOARD_LOCK);

			#if INDEXED_IMPLEMENT
				m_pIA->Lock( 0, 0, BILLBOARD_LOCK);
			#endif
			}

			// 임의의 축과 카메라축과 고정
			VEC3D vec, dir;
			{
				i3Vector::Sub( &dir, &pBillboard->m_End, &pBillboard->m_Start);
				i3Vector::Normalize( &dir, &dir);
				i3Vector::Cross( &vec, &dir, &m_InvViewAt);
				i3Vector::Normalize( &vec, &vec);
			}
			_setPositions( m_pVA, pBillboard, vIdx, &vec);
			
			// UV
			if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
			{
				_setUV( m_pVA, pBillboard, vIdx, m_Space, m_bRotateUV);
			}

			// Color
			if( m_pVA->GetFormat()->GetHasColor())
				_setColor( m_pVA, pBillboard, vIdx);
			
			// Index
			#if INDEXED_IMPLEMENT
			{
				Indices[0] = (UINT16)(vIdx);
				Indices[1] = (UINT16)(vIdx + 2);
				Indices[2] = (UINT16)(vIdx + 1);
				Indices[3] = (UINT16)(vIdx + 1);
				Indices[4] = (UINT16)(vIdx + 2);
				Indices[5] = (UINT16)(vIdx + 3);

				m_pIA->SetIndices( iIdx, 6, Indices);
			}
			#endif

			m_PrimCount++;
			
			#if INDEXED_IMPLEMENT
				vIdx += 4;
				iIdx += 6;
			#else
				vIdx += 6;
			#endif
			
		}

		m_bEditted = false;
	}

	if( m_PrimCount > 0)
	{
		if( bLocked)
			m_pVA->Unlock();

		pContext->SetVertexArray( m_pVA);

		#if INDEXED_IMPLEMENT
			if( bLocked)
				m_pIA->Unlock();

			pContext->SetIndexArray( m_pIA);
			m_pShader = pContext->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, 0, m_PrimCount << 1);
		#else
			m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, 0, m_PrimCount << 1);
		#endif
	}
}

void i3BillboardAttr::Apply( i3RenderContext * pContext)
{
	I3ASSERT( m_pBillboards != nullptr);
	I3ASSERT( m_pVA != nullptr);

	#if INDEXED_IMPLEMENT
	I3ASSERT( m_pIA != nullptr);
	#endif

	if( m_bAlwaysUpdate)
	{
		ApplyAlwaysUpdate( pContext);
	}
	else
	{
		ApplyEdittedUpdate( pContext);
	}
}

void i3BillboardAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	if( i3::same_of< i3BillboardAttr*>(pObj))
	{
		i3BillboardAttr * pAttr = (i3BillboardAttr *) pObj;

		if( m_pVA != nullptr)
		{
			bool bUV, bColor;

			pAttr->m_Space = m_Space;
			pAttr->m_Count = m_Count;
			pAttr->m_PrimCount = m_PrimCount;

			if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
				bUV = true;
			else
				bUV = false;

			bColor = m_pVA->GetFormat()->GetHasColor();

			pAttr->Create( m_Count, m_Space, bUV, bColor);

			memcpy( pAttr->m_pBillboards, m_pBillboards, sizeof(I3SG_BILLBOARD_INFO) * m_Count);
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED Billboard_HEADER
	{
		UINT8			m_Space = 0;
		UINT8			m_bAlwaysUpdate = 0;
		UINT8			m_bUV = 0;
		UINT8			m_bColor = 0;
		UINT32			m_Count = 0;
	};

	struct PACKED Billboard_INFO
	{
		i3::pack::VEC3D		m_Start;
		i3::pack::VEC3D		m_End;
		REAL32				m_Height = 0.0f;
		i3::pack::VEC2D		m_MinUV;
		i3::pack::VEC2D		m_MaxUV;
		UINT32				m_Color = 0;
		UINT8				m_bEnable = 0;
		UINT8				m_Reserved[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3BillboardAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::Billboard_INFO		Info;
	UINT32 i;
	I3SG_BILLBOARD_INFO * pBillboard = m_pBillboards;

	// Header
	{
		pack::Billboard_HEADER		Header;

		Header.m_Space			= (UINT8) m_Space;
		Header.m_bAlwaysUpdate	= (UINT8) m_bAlwaysUpdate;
		Header.m_bUV			= (UINT8) (m_pVA->GetFormat()->GetTextureCoordSetCount() > 0);
		Header.m_bColor			= (UINT8) m_pVA->GetFormat()->GetHasColor();
		Header.m_Count			= m_Count;

		Rc = pStream->Write( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3BillboardAttr::OnSave()", "Could not write Billboard header.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Info.m_Start.x = i3Vector::GetX( &pBillboard->m_Start);
		Info.m_Start.y = i3Vector::GetY( &pBillboard->m_Start);
		Info.m_Start.z = i3Vector::GetZ( &pBillboard->m_Start);

		Info.m_End.x = i3Vector::GetX( &pBillboard->m_End);
		Info.m_End.y = i3Vector::GetY( &pBillboard->m_End);
		Info.m_End.z = i3Vector::GetZ( &pBillboard->m_End);

		Info.m_Height = pBillboard->m_Height;

		Info.m_MinUV.x = i3Vector::GetU( &pBillboard->m_UV[0]);
		Info.m_MinUV.y = i3Vector::GetV( &pBillboard->m_UV[0]);
		Info.m_MaxUV.x = i3Vector::GetU( &pBillboard->m_UV[1]);
		Info.m_MaxUV.y = i3Vector::GetV( &pBillboard->m_UV[1]);

		Info.m_bEnable = (UINT8) pBillboard->m_bEnable;

		Info.m_Color = i3Color::ConvertRGBA32( & pBillboard->m_Color);

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3BillboardAttr::OnSave()", "Could not write Billboard information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3BillboardAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3BillboardAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::Billboard_INFO		Info;
	UINT32 i;

	// Header
	{
		pack::Billboard_HEADER		Header;

		Rc = pStream->Read( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3BillboardAttr::OnLoad()", "Could not read Billboard header.");
			return STREAM_ERR;
		}
		Result += Rc;

		Create( Header.m_Count, (I3SG_BILLBOARD_SPACE) Header.m_Space, Header.m_bUV != 0, 
			Header.m_bColor != 0, Header.m_bAlwaysUpdate != 0);
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Rc = pStream->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3BillboardAttr::OnLoad()", "Could not read Billboard information.");
			return STREAM_ERR;
		}
		Result += Rc;

		SetStart( i, Info.m_Start.x, Info.m_Start.y, Info.m_Start.z);
		SetEnd( i, Info.m_End.x, Info.m_End.y, Info.m_End.z);
		SetSize( i, Info.m_Height);

		SetEnable( i, Info.m_bEnable != 0);

		if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			SetTextureCoord( i, Info.m_MinUV.x, Info.m_MinUV.y, Info.m_MaxUV.x, Info.m_MaxUV.y);
		}

		if( m_pVA->GetFormat()->GetHasColor())
		{
			COLOR col;

			i3Color::ConvertRGBA32( Info.m_Color, &col);

			SetColor( i, &col);
		}
	}

	return Result;
}

void i3BillboardAttr::OnLostDevice( bool bLostDevice)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);
}

void i3BillboardAttr::OnRevive( i3RenderContext * pCtx)
{
	_CreateVertex();

	m_pShader = nullptr;
	//ApplyAlwaysUpdate( pCtx);
}
