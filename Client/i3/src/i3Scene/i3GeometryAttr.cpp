#include "i3SceneDef.h"
#include "i3GeometryAttr.h"
#include "i3Gfx.h"
#include "i3SkinBuilder.h"
#include "i3GeometryResolver.h"

#include "../include/i3Gfx/i3GfxGlobalVariable.h"
#if defined( I3G_DX)
#include "../include/i3Gfx/i3GfxGlobalVariableDX.h"
#include "../include/i3Gfx/i3VertexArrayDX.h"
#include "../include/i3Gfx/i3GfxUtilDX.h"
#endif

#include "i3SkinMeshAABBProcessor.h"
#include "i3Math/structure/aabb.h"

#include "i3Math/structure/mat4.h"
#include "i3Math/structure/vec3_function.h"
#include "i3Math/octree/occ_quad_set.h"

I3_CLASS_INSTANCE( i3GeometryAttr);

#if (defined( I3G_OGLES) && !defined(I3G_G10))
#define INVALID_HMACRO		0xFFFFFFFF
#endif


i3GeometryAttr::i3GeometryAttr(void)
{
	SetID( I3_ATTRID_GEOMETRY);
}

i3GeometryAttr::~i3GeometryAttr(void)
{
	
	#if defined( I3G_DX)
	I3_SAFE_RELEASE_NODBG( m_pD3DXMesh);
	#endif

	I3_SAFE_RELEASE( m_pVA );
	I3_SAFE_RELEASE( m_pIA );

	//I3MEM_SAFE_FREE( m_pPrimLength, m_PrimLengthIdx );
	I3MEM_SAFE_FREE_POINTER( m_pPrimLength ); 

	I3_SAFE_RELEASE( m_pBlendMatrixList);

	#if (defined( I3G_OGLES)  && !defined(I3G_G10))
	if( m_hMacro != INVALID_HMACRO)
	{
		I3OGL::glexDeleteMacros( 1, (GLuint *) &m_hMacro);

		m_hMacro = INVALID_HMACRO;
	}
	#endif

	// skin optimized data by 정순구 2006. 3. 10
	I3_SAFE_RELEASE( m_pPrimVA );
	I3_SAFE_RELEASE( m_pPrimIA );
	
	_DestroyPrimGroup();
}

void i3GeometryAttr::_DestroyPrimGroup(void)
{
	if( m_pPrimGroup )
	{
		I3_DRAW_GROUP * pInfo;

		if( m_pPrimGroup.use_count() == 1)
		{
			for(size_t i = 0; i < m_pPrimGroup->size(); i++ )
			{
				pInfo = (*m_pPrimGroup)[ i ];
				I3MEM_SAFE_FREE_POINTER( pInfo );
			}
			m_pPrimGroup->clear();
		}
		
		m_pPrimGroup.reset();
//		I3_SAFE_RELEASE( m_pPrimGroup );
	}
}


void i3GeometryAttr::SetVertexArray( i3VertexArray * pVA, bool bBuildSkin)
{
	I3_REF_CHANGE( m_pVA, pVA);

	if( m_pVA != nullptr)
	{
		if( !i3::same_of<i3VertexArrayMem*>(m_pVA) && bBuildSkin)
			OptimizeForSkin();
	}
}

void i3GeometryAttr::Transform( MATRIX * pMat)
{
	i3VertexArray * pNewVA;

	pNewVA = i3VertexArray::new_object_ref();

	pNewVA->Create( m_pVA->GetFormat(), m_pVA->GetCount(), m_pVA->GetUsageFlag());

	m_pVA->Transform( pMat, pNewVA);

	SetVertexArray( pNewVA);

	return; 
}

void i3GeometryAttr::SetVertexArray( i3VertexArray * pVA, I3G_PRIMITIVE prim, UINT32 primcount, UINT32 startIdx, bool bBuildSkin)
{
	m_PrimType = prim;

	m_PrimCount = primcount;

	m_StartIdx = startIdx;

	//I3MEM_SAFE_FREE( m_pPrimLength, m_PrimLengthIdx );
	I3MEM_SAFE_FREE_POINTER( m_pPrimLength ); 

	switch( prim)
	{
		case I3G_PRIM_LINESTRIP :
		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :
			
			//m_PrimLengthIdx = i3MemAlloc( sizeof(UINT32) * primcount); 
			m_pPrimLength	= (UINT32 *) i3MemAlloc( sizeof(UINT32) * primcount); 

			if( prim == I3G_PRIM_LINESTRIP)
				m_VertexMargine = 1;
			else
				m_VertexMargine = 2;
			break;

		default :
			break;
	}

	SetVertexArray( pVA, bBuildSkin);
}

void i3GeometryAttr::SetIndexArray( i3IndexArray * pIA)
{
	I3_REF_CHANGE( m_pIA, pIA);

#ifdef I3G_USAGE_SOFTWAREPROCESSING
	if( m_pIA != nullptr)
	{
		if((m_pVA != nullptr) && (m_pVA->GetUsageFlag() & I3G_USAGE_SOFTWAREPROCESSING))
		{
			m_pIA->SetUsageFlag( pIA->GetUsageFlag() | I3G_USAGE_SOFTWAREPROCESSING);
		}
	}
#endif
}

#if (defined( I3G_OGLES) && !defined(I3G_G10))
void i3GeometryAttr::BuildMacro(void)
{
#ifdef I3_WIPI_KTF_KV3600
	// KV3600에서는 텍스쳐 생성과 매크로를 생성을 섞어서 하게 되면,
	// 매크로 리소스가 텍스쳐 비디오 메모리 영역을 침범해서 텍스쳐가 누수가 되는 버그가 발생한다.
	// - Chaen
	return;
#else
	if((m_pPrimLength == nullptr) || (m_PrimCount == 1))
	{
		i3VertexArrayOGLES * pVAOGL = (i3VertexArrayOGLES *) m_pVA;
		OGL_CHECK( I3OGL::glexGenMacros(1, (GLuint *) &m_hMacro));

		if(m_hMacro == INVALID_HMACRO)
		{
			I3TRACE("warning! Failed to create macro. need more memory.\n");
		}
		else
		{

			// Normal
			if( m_pVA->GetFormat()->GetHasNormal())
			{
				OGL_CHECK( I3OGL::glEnableClientState( GL_NORMAL_ARRAY));
				OGL_CHECK( I3OGL::glNormalPointer( GL_FIXED, 0, pVAOGL->GetNormalPointer()));
			}
			else
				OGL_CHECK( I3OGL::glDisableClientState( GL_NORMAL_ARRAY));

			// Texture Coord.
			if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
			{
				OGL_CHECK( I3OGL::glEnableClientState( GL_TEXTURE_COORD_ARRAY));
				OGL_CHECK( I3OGL::glTexCoordPointer( 2, GL_FIXED, 0, pVAOGL->GetTexCoordPointer()));
			}
			else
				OGL_CHECK( I3OGL::glDisableClientState( GL_TEXTURE_COORD_ARRAY));

			// Color
			if( m_pVA->GetFormat()->GetHasColor())
			{
				OGL_CHECK( I3OGL::glEnableClientState( GL_COLOR_ARRAY));
				OGL_CHECK( I3OGL::glColorPointer( 4, GL_UNSIGNED_BYTE, 0, pVAOGL->GetColorPointer()));
			}
			else
				OGL_CHECK( I3OGL::glDisableClientState( GL_COLOR_ARRAY));

			// Position
			if( m_pVA->GetFormat()->GetHasPosition())
			{
				OGL_CHECK( I3OGL::glEnableClientState( GL_VERTEX_ARRAY));
				OGL_CHECK( I3OGL::glVertexPointer( 3, GL_FIXED, 0, pVAOGL->GetPositionPointer()));
			}
			else
				OGL_CHECK( I3OGL::glDisableClientState( GL_VERTEX_ARRAY));

			if( m_pIA != nullptr)
			{
				UINT16 * pIndex;

				pIndex = ((UINT16 *)m_pIA->GetInstance() + m_StartIdx);
				OGL_CHECK( I3OGL::glexCompileElements( m_hMacro, (int)m_PrimType, m_pIA->GetCount(), GL_UNSIGNED_SHORT, pIndex));
			}
			else
				OGL_CHECK( I3OGL::glexCompileArrays( m_hMacro, (int)m_PrimType, m_StartIdx, pVAOGL->GetCount()));
		}
	}
#endif
}
#endif

void i3GeometryAttr::_UpdateLights( i3RenderContext * pCtx)
{
	INT32 i;
	i3Light * pLight;
	VEC3D center;
	REAL32 radius = 0.f;

	pCtx->resetAffectLight();

	if( m_pVA != nullptr)
	{
		radius = m_pVA->getBoundSphereRadius();
		i3Vector::Add( &center, m_pVA->getBoundSphereCenter(), i3Matrix::GetPos( pCtx->GetWorldMatrix()));
	}
	else if( m_pPrimVA != nullptr)
	{
		radius = m_pPrimVA->getBoundSphereRadius();
		i3Vector::Add( &center, m_pPrimVA->getBoundSphereCenter(), i3Matrix::GetPos( pCtx->GetWorldMatrix()));
	}
	

	// Render Context에 등록되어 있는 Light들 중에 현재 Rendering하려는
	// VertexArray에 근접한 것이 있다면 Affected Light에 추가한다.
	for( i = 0; i < pCtx->getLightCount(); i++)
	{
		pLight = pCtx->getLight( i);

		if( pLight->isState( I3G_LIGHT_STATE_DISABLE))
			continue;

		if( pLight->GetLightType() == I3G_LIGHT_DIRECTIONAL)
		{
			pLight->setSqDistance( 0.0f);
			pCtx->addAffectLight( pLight);
		}
		else
		{
			if( pLight->isAffect( &center, radius))
			{
				pCtx->addAffectLight( pLight);
			}
		}
	}
}

void i3GeometryAttr::Apply( i3RenderContext * pContext)
{
	INT32	j;
	MATRIX *	pBoneTable[256];
	i3VertexArray * pVA = m_pVA;
	i3IndexArray * pIA = GetIndexArray();
	I3_DRAW_GROUP	* pGroup;

	/*if( m_pVA == nullptr)
		return;*/

	if( m_PrimCount <= 0)
		return;

	if( pContext->GetLightingEnable())
	{
		_UpdateLights( pContext);
	}

#if (defined( I3G_OGLES) && !defined(I3G_G10))
	if(m_hMacro == INVALID_HMACRO)
	{
		BuildMacro();
	}

	if( m_hMacro != INVALID_HMACRO)
	{
		I3OGL::glexDrawMacros( m_hMacro);
		return;
	}
#endif

	if( m_pD3DXMesh != nullptr)
	{
		if( m_pVA->GetBlendIndexCount() > 0)
		{
			if( getVertexBlendMethod() == I3G_VERTEXBLEND_SHADER )
			{
				pContext->SetSoftwareVertexBlendEnable(false);
				pContext->SetVertexBlendEnable(true);
			}
			else
			{
				i3GfxCaps * pCaps = g_pRenderContext->GetCaps();

				if( m_pBlendMatrixList->getBoneCount() <= pCaps->GetMaxBoneMatrixCount() )
				{	
					pContext->SetSoftwareVertexBlendEnable(false);
					pContext->SetVertexBlendEnable(false);
				}
				else
				{	
					// 소프트 블랜딩을 해야된다.
					pContext->SetSoftwareVertexBlendEnable(true);
					pContext->SetVertexBlendEnable(true);
					
				}
			}

			if( m_pBlendMatrixList != nullptr)
			{
				pContext->SetVertexBlendWeightCount( m_pVA->GetBlendWeightCount() + 1);

				for( j = 0; j < m_pBlendMatrixList->getBoneCount(); j++ )
				{
					pBoneTable[j] = m_pBlendMatrixList->getTransformedMatrix( j );
				}

				pContext->SetVertexBlendMatricesByRef( pBoneTable, m_pBlendMatrixList->getBoneCount() );
			}
		}

		pContext->SetVertexArray( m_pVA);
		pContext->SetIndexArray( m_pIA);

		if( m_pD3DPMesh != nullptr)
		{
			m_pDrawShader = pContext->DrawDXMesh( (LPD3DXMESH)m_pD3DPMesh, m_pDrawShader, 0, m_pD3DPMesh->GetNumFaces());
		}
		else
		{
			m_pDrawShader = pContext->DrawDXMesh( m_pD3DXMesh, m_pDrawShader, 0, m_pD3DXMesh->GetNumFaces());
		}

		if( m_pVA->GetBlendIndexCount() > 0)
		{
			pContext->SetVertexBlendWeightCount( 0);
			pContext->SetSoftwareVertexBlendEnable(false);
			pContext->SetVertexBlendEnable(false);
		}
	}
	else
	{
		if( m_pPrimGroup && (m_pPrimVA != nullptr) )
		{
			// 2006. 2. 15. by dfly79
			// Non-Indexed Vertex Blend function & Indexed Vertex Blend function
			INT32	idx;
			INT32 startIdx, primCount;
		
			pContext->SetVertexArray( m_pPrimVA );

			if( m_pPrimIA )
				pContext->SetIndexArray ( m_pPrimIA );
			
			for(size_t i = 0; i < m_pPrimGroup->size(); i++ )
			{
				pGroup = (*m_pPrimGroup)[i];
				
				startIdx = pGroup->m_idxStart;
				primCount = pGroup->m_PrimCount;

				I3ASSERT( m_pBlendMatrixList != nullptr);

				if( getVertexBlendMethod() == I3G_VERTEXBLEND_SHADER )
				{
					pContext->SetVertexBlendEnable(true);
				}
				else
				{
					i3GfxCaps * pCaps = g_pRenderContext->GetCaps();

					if( pGroup->m_BoneCount <= pCaps->GetMaxBoneMatrixCount() )
					{	
						pContext->SetSoftwareVertexBlendEnable(false);
						pContext->SetVertexBlendEnable(false);
					}
					else
					{	
						// 소프트 블랜딩을 해야된다.
						pContext->SetSoftwareVertexBlendEnable(true);
						pContext->SetVertexBlendEnable(true);
						
					}
				}

				{
					pContext->SetVertexBlendWeightCount( m_pPrimVA->GetBlendWeightCount() + 1 );

					for( j = 0; j < pGroup->m_BoneCount; j++ )
					{
						idx = pGroup->m_Bone[ j ];
						I3ASSERT( idx != 0xFF);
						I3ASSERT( idx < m_pBlendMatrixList->getBoneCount() );

						pBoneTable[j] = m_pBlendMatrixList->getTransformedMatrix( idx);
					}

					pContext->SetVertexBlendMatricesByRef( pBoneTable, pGroup->m_BoneCount);
				}

				// Strip | Fan이 아니다.
				if( m_pPrimIA == nullptr)
					m_pDrawShader = pContext->DrawPrim( m_pDrawShader, m_PrimType, startIdx, primCount);
				else
					m_pDrawShader = pContext->DrawIndexedPrim( m_pDrawShader, m_PrimType, startIdx, primCount);

#if defined( I3_DEBUG)
				if( m_pOwner != nullptr)
					pContext->AddDrawPrimitiveList( ((i3NamedElement*)m_pOwner)->GetName(), startIdx, primCount);
#endif
			}

			pContext->SetVertexBlendWeightCount( 0);
			pContext->SetSoftwareVertexBlendEnable(false);
			pContext->SetVertexBlendEnable(false);

			return;
		}

		if( m_pVA == nullptr)
			return;

		if( m_pVA->GetBlendIndexCount() > 0)
		{
			pContext->SetSoftwareVertexBlendEnable(true);
			pContext->SetVertexBlendEnable(true);
			pContext->SetVertexBlendWeightCount( m_pVA->GetBlendWeightCount() + 1);

			I3ASSERT( m_pBlendMatrixList != nullptr);

			//m_pBlendMatrixList->Apply( pContext);

			for( j = 0; j < m_pBlendMatrixList->getBoneCount(); j++ )
				pBoneTable[j] = m_pBlendMatrixList->getTransformedMatrix( j );

			pContext->SetVertexBlendMatricesByRef( pBoneTable, m_pBlendMatrixList->getBoneCount() );
		}

		pContext->SetVertexArray( pVA);

		if( m_pPrimLength == nullptr)
		{
			// Strip | Fan이 아니다.
			if( pIA == nullptr)
			{	
				m_pDrawShader = pContext->DrawPrim( m_pDrawShader, m_PrimType, m_StartIdx, m_PrimCount);
			}
			else
			{
				pContext->SetIndexArray( pIA);
				m_pDrawShader = pContext->DrawIndexedPrim( m_pDrawShader, m_PrimType, m_StartIdx, m_PrimCount);
			}

#if defined( I3_DEBUG)
			if( m_pOwner != nullptr)
				pContext->AddDrawPrimitiveList( ((i3NamedElement*)m_pOwner)->GetName(), m_StartIdx, m_PrimCount);
#endif
		}
		else
		{
			// Strip 또는 Fan 이다.

			if( pIA == nullptr)
			{
				if( m_PrimCount > 1)
				{
					m_pDrawShader = pContext->DrawStripPrim( m_pDrawShader, m_PrimType, m_StartIdx, m_pPrimLength, m_PrimCount);

#if defined( I3_DEBUG)
					pContext->AddDrawPrimitiveList( ((i3NamedElement*)m_pOwner)->GetName(), m_StartIdx, m_PrimCount);
#endif
				}
				else
				{
					if( GetPrimitiveType() == I3G_PRIM_LINESTRIP)
						m_pDrawShader = pContext->DrawPrim( m_pDrawShader, m_PrimType, m_StartIdx, pVA->GetCount() - 1);
					else
						m_pDrawShader = pContext->DrawPrim( m_pDrawShader, m_PrimType, m_StartIdx, pVA->GetCount() - 2);
#if defined( I3_DEBUG)
					if( m_pOwner != nullptr)
						pContext->AddDrawPrimitiveList( ((i3NamedElement*)m_pOwner)->GetName(), m_StartIdx, pVA->GetCount() - 2);
#endif
				}
			}
			else
			{
				pContext->SetIndexArray( pIA);

				if( m_PrimCount > 1)
				{
					m_pDrawShader = pContext->DrawStripIndexedPrim( m_pDrawShader, m_PrimType, m_StartIdx, m_pPrimLength, m_PrimCount);
#if defined( I3_DEBUG)
					if( m_pOwner != nullptr)
						pContext->AddDrawPrimitiveList( ((i3NamedElement*)m_pOwner)->GetName(), m_StartIdx, m_PrimCount);
#endif
				}
				else
				{
					if( GetPrimitiveType() == I3G_PRIM_LINESTRIP)
						m_pDrawShader = pContext->DrawIndexedPrim( m_pDrawShader, m_PrimType, m_StartIdx, m_pIA->GetCount() - 1);
					else
						m_pDrawShader = pContext->DrawIndexedPrim( m_pDrawShader, m_PrimType, m_StartIdx, m_pIA->GetCount() - 2);
#if defined( I3_DEBUG)
					if( m_pOwner != nullptr)
						pContext->AddDrawPrimitiveList( ((i3NamedElement*)m_pOwner)->GetName(), m_StartIdx, pVA->GetCount() - 2);
#endif
				}
			}
		}

		if( pVA->GetBlendIndexCount() > 0)
		{
			pContext->SetVertexBlendWeightCount( 0);
			pContext->SetSoftwareVertexBlendEnable(false);
			pContext->SetVertexBlendEnable(false);
		}
	}
}

bool i3GeometryAttr::IsSame( i3RenderAttr * pAttr)
{
	i3GeometryAttr * pAttr2 = (i3GeometryAttr *) pAttr;

	return ((m_pVA == pAttr2->m_pVA) &&
			(m_PrimType == pAttr2->m_PrimType) &&
			(m_PrimCount == pAttr2->m_PrimCount) &&
			(m_StartIdx == pAttr2->m_StartIdx));
}

void i3GeometryAttr::SetPrimitiveLength( UINT32 Idx, UINT32 Length)
{
	I3ASSERT( m_pPrimLength != nullptr);

	m_pPrimLength[Idx] = Length;
}

UINT32 i3GeometryAttr::GetPrimitiveLength( UINT32 Idx)
{
	I3ASSERT( m_pPrimLength != nullptr);

	return m_pPrimLength[ Idx];
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED GEOMETRYATTR_INFO
	{
		UINT8		m_PrimType = 0;
		UINT32		m_PrimCount = 0;
		UINT32		m_StartIndex = 0;
		OBJREF		m_VA = 0;
		OBJREF		m_IA = 0;
		UINT32		m_PrimLengthSz = 0;
	};

	struct PACKED GEOMETRYATTR_INFO2
	{
		INT8		m_ID[4] = { 'G', 'E', 'O', '2' };
		UINT8		m_PrimType = 0;
		UINT32		m_PrimCount = 0;
		UINT32		m_StartIndex = 0;
		OBJREF		m_VA = 0;
		OBJREF		m_IA = 0;
		UINT32		m_PrimLengthSz = 0;

		i3::pack::VEC3D		m_BoundMin;
		i3::pack::VEC3D		m_BoundMax;
		UINT8		m_GeoStyle = 0;
		UINT8		pad8[3] = { 0 };
		UINT32		pad[3] = { 0 };

		void		CopyFrom(GEOMETRYATTR_INFO * pSrc)
		{
			m_PrimType = pSrc->m_PrimType;
			m_PrimCount = pSrc->m_PrimCount;
			m_StartIndex = pSrc->m_StartIndex;
			m_VA = pSrc->m_VA;
			m_IA = pSrc->m_IA;
			m_PrimLengthSz = pSrc->m_PrimLengthSz;
		}
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

void i3GeometryAttr::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	List.push_back( m_pVA);

	if( m_pIA != nullptr)
		List.push_back( m_pIA);

	i3RenderAttr::OnBuildObjectList( List);
}

UINT32 i3GeometryAttr::OnSave( i3ResourceFile * pFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pFile->GetStream();
	pack::GEOMETRYATTR_INFO2 Info;
	
	Info.m_PrimCount = m_PrimCount;
	Info.m_PrimType = (UINT8) i3Gfx::Convert( GetPrimitiveType());
	Info.m_StartIndex = m_StartIdx;
	Info.m_VA = (OBJREF) pFile->GetObjectPersistID( m_pVA);
	Info.m_IA = (OBJREF) pFile->GetObjectPersistID( m_pIA);
	Info.m_GeoStyle = m_GeoStyle;

	if( isValidBound() == false)
	{
		CalcBoundBox( &m_BoundMin, &m_BoundMax);
	}

	i3Vector::Copy( &Info.m_BoundMin, &m_BoundMin);
	i3Vector::Copy( &Info.m_BoundMax, &m_BoundMax);

	if( m_pPrimLength != nullptr)
	{
		Info.m_PrimLengthSz = sizeof(UINT32) * m_PrimCount;
	}
	
	Rc = pStream->Write( &Info, sizeof( Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GeometryAttr::OnSave()", "Could not write geometry attribute information.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Primitive Length
	if( m_pPrimLength != nullptr)
	{
		Rc = pStream->Write( m_pPrimLength, Info.m_PrimLengthSz);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3GeometryAttr::OnSave()", "Could not write geometry primitive length information.");
			return STREAM_ERR;
		}

		Result += Rc;
	}

	return Result;
}

void _GetVertexDesc( i3VertexFormat * pFormat, D3DVERTEXELEMENT9 * pDecl)
{
	INT32 idx = 0;
	INT32 stride = 0;
	INT32 nOffsetPos, nOffsetRHW, nOffsetWeight,nOffsetBlendIndex, nOffsetNormal, nOffsetTangent, nOffsetBinormal, nOffsetColor;

	INT32 nOffsetTexCoord[4] = { 0,};

	nOffsetPos= nOffsetRHW= nOffsetWeight=nOffsetBlendIndex= nOffsetNormal= nOffsetTangent= nOffsetBinormal = 0;

	// Position (X, Y, Z) Component
	{
		nOffsetPos = 0;

		if( pFormat->GetHasPosition())
		{
			stride += sizeof(REAL32) * 3;

			pDecl[idx].Stream = 0;
			pDecl[idx].Offset = (WORD) nOffsetPos;
			pDecl[idx].Type = D3DDECLTYPE_FLOAT3;
			pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
			pDecl[idx].Usage = D3DDECLUSAGE_POSITION;
			pDecl[idx].UsageIndex = 0;
			idx++;
		}
		else
		{
			I3ASSERT_0;
		}
	}

	// RHW(Reciprocal Homogeneous W) Component
	{
		nOffsetRHW = stride;

		if( pFormat->IsTransformed() && !pFormat->GetHasNormal())
		{
			stride += sizeof(REAL32);

			pDecl[idx - 1].Type = D3DDECLTYPE_FLOAT4;
			pDecl[idx - 1].Usage = D3DDECLUSAGE_POSITIONT;
		}
	}

	// Blend Weight Component
	{
		nOffsetWeight = stride;

		if( pFormat->GetBlendWeightCount() > 0)
		{
			stride += sizeof(REAL32) * pFormat->GetBlendWeightCount();

			pDecl[idx].Stream = 0;
			pDecl[idx].Offset = (WORD) nOffsetWeight;
			pDecl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + pFormat->GetBlendWeightCount() - 1);
			pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
			pDecl[idx].Usage = D3DDECLUSAGE_BLENDWEIGHT;
			pDecl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// Blend Matrix Index Component
	{
		i3GfxCaps * pCaps = i3RenderContext::GetCaps();

		nOffsetBlendIndex = stride;

		if( pFormat->GetBlendIndexCount() > 0)
		{
			if( pCaps->isSupported_UBYTE4())
			{
				stride += sizeof(UINT32);

				pDecl[idx].Stream = 0;
				pDecl[idx].Offset = (WORD) nOffsetBlendIndex;
				pDecl[idx].Type = D3DDECLTYPE_UBYTE4;
				pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
				pDecl[idx].Usage = D3DDECLUSAGE_BLENDINDICES;
				pDecl[idx].UsageIndex = 0;
				idx++;
			}
			else
			{
				stride += sizeof(REAL32) * pFormat->GetBlendIndexCount();
				
				pDecl[idx].Stream = 0;
				pDecl[idx].Offset = (WORD) nOffsetBlendIndex;
				pDecl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + pFormat->GetBlendIndexCount() - 1);
				pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
				pDecl[idx].Usage = D3DDECLUSAGE_BLENDINDICES;
				pDecl[idx].UsageIndex = 0;
				idx++;
			}
		}
	}

	// Normal Component
	{
		nOffsetNormal = stride;

		if( pFormat->GetHasNormal())
		{
			stride += sizeof(REAL32) * 3;

			pDecl[idx].Stream = 0;
			pDecl[idx].Offset = (WORD) nOffsetNormal;
			pDecl[idx].Type = D3DDECLTYPE_FLOAT3;
			pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
			pDecl[idx].Usage = D3DDECLUSAGE_NORMAL;
			pDecl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// Vertex Diffuse Color Component
	{
		nOffsetColor = stride;

		if( pFormat->GetHasColor())
		{
			stride += sizeof(UINT32);

			pDecl[idx].Stream = 0;
			pDecl[idx].Offset = (WORD) nOffsetColor;
			pDecl[idx].Type = D3DDECLTYPE_D3DCOLOR;
			pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
			pDecl[idx].Usage = D3DDECLUSAGE_COLOR;
			pDecl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// Texture Coord. Set Components
	{
		UINT32 i;

		for( i = 0; i < (UINT32)pFormat->GetTextureCoordSetCount(); i++)
		{
			nOffsetTexCoord[i] = stride;

			stride += sizeof(REAL32) * pFormat->GetTextureCoordCount();

			pDecl[idx].Stream = 0;
			pDecl[idx].Offset = (WORD) nOffsetTexCoord[i];
			pDecl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + (pFormat->GetTextureCoordCount() - 1));
			pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
			pDecl[idx].Usage = D3DDECLUSAGE_TEXCOORD;
			pDecl[idx].UsageIndex = (BYTE) i;
			idx++;
		}
	}

	// Tangent
	{
		nOffsetTangent = stride;

		if( pFormat->GetHasTangent())
		{
			stride += sizeof(REAL32) * 3;

			pDecl[idx].Stream = 0;
			pDecl[idx].Offset = (WORD) nOffsetTangent;
			pDecl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT3);
			pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
			pDecl[idx].Usage = D3DDECLUSAGE_TANGENT;
			pDecl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// Binormal
	{
		nOffsetBinormal = stride;

		if( pFormat->GetHasBinormal())
		{
			stride += sizeof(REAL32) * 3;

			pDecl[idx].Stream = 0;
			pDecl[idx].Offset = (WORD) nOffsetBinormal;
			pDecl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT3);
			pDecl[idx].Method = D3DDECLMETHOD_DEFAULT;
			pDecl[idx].Usage = D3DDECLUSAGE_BINORMAL;
			pDecl[idx].UsageIndex = 0;
			idx++;
		}
	}

	pDecl[idx].Stream = 0xFF;
	pDecl[idx].Offset = 0;
	pDecl[idx].Type = D3DDECLTYPE_UNUSED;
	pDecl[idx].Method = 0;
	pDecl[idx].Usage = 0;
	pDecl[idx].UsageIndex = 0;
}

UINT32 i3GeometryAttr::OnLoad( i3ResourceFile * pFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pFile->GetStream();
	pack::GEOMETRYATTR_INFO2 Info;
	i3VertexArray * pVA = nullptr;
	i3IndexArray * pIA = nullptr;
	INT8	id[4];

	Rc = pFile->Read( id, 4);
	I3_CHKIO( Rc);

	pStream->SetPosition( -4, STREAM_CURRENT);

	if( memcmp( id, "GEO2", 4) != 0)
	{
		// Old Version
		pack::GEOMETRYATTR_INFO oldInfo;

		Rc = pStream->Read( &oldInfo, sizeof( oldInfo));
		I3_CHKIO( Rc);
		Result += Rc;

		Info.CopyFrom( &oldInfo);
	}
	else
	{
		Rc = pFile->Read( &Info, sizeof( Info));
		I3_CHKIO( Rc);
		Result += Rc;

		m_GeoStyle = Info.m_GeoStyle;
	}

	setBound(Info.m_BoundMin, Info.m_BoundMax);

	if( Info.m_VA != 0)
	{
		pVA = (i3VertexArray *) pFile->FindObjectByID( Info.m_VA);
		I3ASSERT( pVA);

		SetVertexArray( pVA, i3Gfx::Convert( (I3G_PERSIST_PRIMITIVE) Info.m_PrimType), Info.m_PrimCount, Info.m_StartIndex, false);
	}

	if( Info.m_IA != 0)
	{
		pIA = (i3IndexArray *) pFile->FindObjectByID( Info.m_IA);
		I3ASSERT( pIA);

		SetIndexArray( pIA);
	}

	if( Info.m_PrimLengthSz > 0)
	{
		Rc = pStream->Read( m_pPrimLength, Info.m_PrimLengthSz);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3GeometryAttr::OnLoad()", "Could not read geometry primitive length information.");
			return STREAM_ERR;
		}

		Result += Rc;
	}

	if( pVA != nullptr)
	{
		INT32 primCount = pVA->GetCount();

		// Bug fix!!!!
		// 
		// IndexArray의 개수와 Primitive Count의 개수가 Match되지 않는 경우가 발생.
		// 현재 원인은 파악되지 않음. 확인 필요.
		if( GetPrimitiveLengthArray() == nullptr)
		{
			if( pIA != nullptr)
				primCount = i3Gfx::CalcPrimCount( GetPrimitiveType(), pIA->GetCount());
			else
				primCount = i3Gfx::CalcPrimCount( GetPrimitiveType(), pVA->GetCount());

			if( primCount < (INT32) GetPrimitiveCount())
			{
				I3PRINTLOG(I3LOG_NOTICE,  "Geometry의 Primitive Count와 Vertex(Index) 개수가 일치하지 않습니다. (%d, %d)", 
					primCount, GetPrimitiveCount());

				SetPrimitiveCount( primCount);
			}
		}
	}

	//I3ASSERT( isValidBound());

	OptimizeForSkin();

	if (m_pVA->GetFormat()->GetBlendIndexCount() <= 0)
	{
		if (!m_matOBBPtr)
			_MakeOBBSet();
	}

	return Result;
}

#if defined( I3G_DX)
void i3GeometryAttr::SetProgressiveRate( INT32 nRate)
{
	if( m_pD3DPMesh == nullptr)
		return;

	if( nRate == m_nRate)
		return;

	INT32 diff = m_nMaxCount - m_nMinCount;

	m_nRate = MINMAX( 0, nRate, 100);

	diff = MINMAX( m_nMinCount, m_nMinCount + (INT32)(diff * ((REAL32)m_nRate * 0.01f)), m_nMaxCount);

	HRESULT hr = m_pD3DPMesh->SetNumVertices( diff);
	if( FAILED( hr))
	{
		I3TRACE("fail SetNumVertices\n");
	}
}
#endif


void i3GeometryAttr::OptimizeForSkin(void)
{
	INT32 nBlendIndexCount, nBoneMatrixCount;

	if( i3::same_of<i3VertexArrayMem*>(m_pVA))
		return;

	if (m_pVA->GetFormat()->GetBlendIndexCount() <= 0)
	{
		return;
	}

	nBlendIndexCount = 4;
	nBoneMatrixCount = 20;

	BuildSkin( nBlendIndexCount, nBoneMatrixCount );	

	m_AABBProcessorPtr = i3::make_shared<i3SkinMeshAABBProcessor>(this);

}

void	i3GeometryAttr::CreateSkinMeshAABBProcessor()
{
	if (m_pVA == nullptr) // 이 경우 처리가 안됩니다..
		return;

	if (!m_AABBProcessorPtr)
		m_AABBProcessorPtr = i3::make_shared<i3SkinMeshAABBProcessor>(this);
}

void	i3GeometryAttr::CreateSkinMeshAABBProcessor(i3VertexArray* pExternVA)
{
	if (pExternVA == nullptr) // 이 경우 처리가 안됩니다..
		return;

	if (!m_AABBProcessorPtr)
		m_AABBProcessorPtr = i3::make_shared<i3SkinMeshAABBProcessor>(pExternVA);
}


//
// 2003.11.10
// 이진균
//
// 현재 Version(0.1)에서는 Skinning Animation을 Software적으로 처리한다.
//
// Blend Matrix의 계산 법은 아래와 같다.
//
// Blend Matrix = Inverse Joint Transform * Bone Matrix
// Bone Matrix = Joint Transform * Accumulated Parent Transform
//
/*
void i3GeometryAttr::CalcBlendedVertex(void)
{
	INT32 i, j;
	i3VertexFormat * pFormat = m_pVA->GetFormat();
	VEC3D vec, vec2, temp;
	REAL32 weight, accmweight;

	m_pVA->Lock( I3G_LOCK_READONLY);

	m_pRenderVA->Lock( 0);
	
	for( i = 0; i < (INT32) m_pVA->GetCount(); i++)
	{
		m_pVA->GetPosition( i, &vec);

		i3Vector::Set( &vec2, 0.0f, 0.0f, 0.0f);
		accmweight = 0.0f;

		for( j = 0; j < pFormat->GetBlendIndexCount(); j++)
		{
			UINT8 Idx = (UINT8) m_pVA->GetBlendIndex( j, i);

			weight = m_pVA->GetBlendWeight( j, i);

			if( weight > I3_EPS)
			{
				// 이 후 Version에서는 Vertex Array를 일괄 Transform 처리하는
				// 함수를 제작하여 한꺼번에 처리할 계획
				// 그 함수는 Platform에 따라 Assembly로 제작될 수 있다.
				i3Vector::TransformCoord( &temp, &vec, g_pLastBlendMatrixList->GetMatrix( Idx));
				i3Vector::Scale( &temp, &temp, weight);
				i3Vector::Add( &vec2, &vec2, &temp);

				// Normal도 계산해야 한다.
			}

			accmweight += weight;
			if( accmweight >= 1.0f)
				break;
		}

		m_pRenderVA->SetPosition( i, &vec2);
	}

	m_pVA->Unlock();
	m_pRenderVA->Unlock();
}
*/

bool i3GeometryAttr::isValidBound(void)
{
	/*VEC3D diff;

	i3Vector::Sub( &diff, &m_BoundMax, &m_BoundMin);

	return (diff.x != 0.0f) || (diff.y != 0.0f) || (diff.z != 0.0f);*/
	return m_bValidBoundBox;
}

void i3GeometryAttr::setBound( const VEC3D * pMin, const VEC3D * pMax)
{
	i3Vector::Copy( &m_BoundMin, (VEC3D *) pMin);
	i3Vector::Copy( &m_BoundMax, (VEC3D *) pMax);

	VEC3D diff;
	i3Vector::Sub( &diff, &m_BoundMax, &m_BoundMin);
	m_bValidBoundBox = (diff.x != 0.0f) || (diff.y != 0.0f) || (diff.z != 0.0f);
}

void i3GeometryAttr::setBound(const i3::pack::VEC3D & Min, const i3::pack::VEC3D & Max)
{
	i3Vector::Copy(&m_BoundMin, &Min);
	i3Vector::Copy(&m_BoundMax, &Max);

	VEC3D diff;
	i3Vector::Sub(&diff, &m_BoundMax, &m_BoundMin);
	m_bValidBoundBox = (diff.x != 0.0f) || (diff.y != 0.0f) || (diff.z != 0.0f);
}

void i3GeometryAttr::RefreshBound(void)
{
	CalcBoundBox( &m_BoundMin, &m_BoundMax);

	VEC3D diff;
	i3Vector::Sub( &diff, &m_BoundMax, &m_BoundMin);
	m_bValidBoundBox = (diff.x != 0.0f) || (diff.y != 0.0f) || (diff.z != 0.0f);
}

void i3GeometryAttr::CalcBoundBox( VEC3D * pMin, VEC3D * pMax)
{
	INT32 i, j, VPP = 3;
	VEC3D p[3];
	i3GeometryResolver resolver;

	if( m_pVA == nullptr)
	{
		i3Vector::Set( pMin, 0.0f, 0.0f, 0.0f);
		i3Vector::Set( pMax, 0.0f, 0.0f, 0.0f);
		return;
	}

	resolver.setGeometryAttr( this);

	switch( resolver.getPrimitiveType())
	{
		case I3G_PRIM_LINELIST :
		case I3G_PRIM_LINESTRIP :
			VPP = 2;
			break;
	}

	for( i = 0; i < resolver.getPrimitiveCount(); i++)
	{
		resolver.getPositions( i, p);

		for( j = 0; j < VPP; j++)
		{
			if( (i == 0) && (j == 0))
			{
				*pMin = *pMax = p[j];
			}
			else
			{
				*pMin = VEC3D::Minimize( *pMin, p[j]);
				*pMax = VEC3D::Maximize( *pMax, p[j]);
			}
		}
	}
}

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max


namespace
{
// 참고 소스 코드 : http://gamekernel.googlecode.com/hg/kgraphics/math/Covariance.cpp
//					https://gamekernel.googlecode.com/hg/kgraphics/math/OBB.cpp

//	OBB Matrix 구하기...  AABB + 회전행렬성분 3개로 정리할까 하다가, 중심값과 스케일 반영후 회전행렬 적용하면 순서가 잘못되 중심값 변경이 되는 실수가 생기므로..
//  OBB Matrix에 스케일,회전,포지션을 모두 저장하는 쪽으로 변경...
//-------------------------------------------------------------------------------
// @ ComputeCovarianceMatrix()
//-------------------------------------------------------------------------------
// Computes and returns the real, symmetric covariance matrix, along with the
// mean vector.
//-------------------------------------------------------------------------------
	void ComputeCovarianceMatrix(i3::mat4& C, i3::vec3& mean, const i3::vec3* points, unsigned int numPoints)
	{
		//ASSERT(numPoints > 1);

		mean.x = 0.f;		mean.y = 0.f;		mean.z = 0.f;

		// compute the mean (the centroid of the points)
		unsigned int i;
		for (i = 0; i < numPoints; i++)
			mean += points[i];

		mean /= (float)numPoints;

		// compute the (co)variances
		float varX = 0.0f;
		float varY = 0.0f;
		float varZ = 0.0f;
		float covXY = 0.0f;
		float covXZ = 0.0f;
		float covYZ = 0.0f;

		i3::vec3 diff;

		for (i = 0; i < numPoints; i++)
		{
			diff = points[i];
			diff -= mean;

			varX += diff.x * diff.x;
			varY += diff.y * diff.y;
			varZ += diff.z * diff.z;
			covXY += diff.x * diff.y;
			covXZ += diff.x * diff.z;
			covYZ += diff.y * diff.z;
		}

		// divide all of the (co)variances by n - 1 
		// (skipping the division if n = 2, since that would be division by 1.0
		if (numPoints > 2)
		{
			const float normalize = (float)(numPoints - 1);
			varX /= normalize;
			varY /= normalize;
			varZ /= normalize;
			covXY /= normalize;
			covXZ /= normalize;
			covYZ /= normalize;
		}

		// pack values into the covariance matrix, which is symmetric
		C._14 = 0.f;	C._24 = 0.f;	C._34 = 0.f;	C._44 = 1.f;
		C._41 = 0.f;	C._42 = 0.f;	C._43 = 0.f;

		C._11 = varX;	C._22 = varY;		C._33 = varZ;
		C._21 = C._12 = covXY;
		C._31 = C._13 = covXZ;
		C._23 = C._32 = covYZ;
				
	}  // End of ComputeCovarianceMatrix()


	   //-------------------------------------------------------------------------------
	   // @ SymmetricHouseholder3x3()
	   //-------------------------------------------------------------------------------
	   // 
	   //-------------------------------------------------------------------------------
	static void SymmetricHouseholder3x3(const i3::mat4& M, i3::mat4& Q,
		i3::vec3& diag, i3::vec3& subd)
	{
		// Computes the Householder reduction of M, computing:
		//
		// T = Q^t M Q
		//
		//   Input:   
		//     symmetric 3x3 matrix M
		//   Output:  
		//     orthogonal matrix Q
		//     diag, diagonal entries of T
		//     subd, lower-triangle entries of T (T is symmetric)

		// T will be stored as follows (because it is symmetric and tridiagonal):
		//
		// | diag[0]  subd[0]  0       |
		// | subd[0]  diag[1]  subd[1] |
		// | 0        subd[1]  diag[2] |

		diag[0] = M(0, 0); // in both cases below, T(0,0) = M(0,0) 
		subd[2] = 0; // T is to be a tri-diagonal matrix - the (2,0) and (0,2) 
					 // entries must be zero, so we don't need subd[2] for this step

					 // so we know that T will actually be:
					 //
					 // | M(0,0)   subd[0]  0       |
					 // | subd[0]  diag[1]  subd[1] |
					 // | 0        subd[1]  diag[2] |

					 // so the only question remaining is the lower-right block and subd[0]

		if (fabs(M(2, 0)) < I3_EPS)
		{
			// if M(2,0) (and thus M(0,2)) is zero, then the matrix is already in
			// tridiagonal form.  As such, the Q matix is the identity, and we
			// just extract the diagonal and subdiagonal components of T as-is
			
			// identity mat
			i3::mat4 matIdentity(	1.f, 0.f, 0.f, 0.f,
									0.f, 1.f, 0.f, 0.f,
									0.f, 0.f, 1.f, 0.f,
									0.f, 0.f, 0.f, 1.f);

			Q = matIdentity;
					

			// so we see that T will actually be:
			//
			// | M(0,0)  M(1,0)  0      |
			// | M(1,0)  M(1,1)  M(2,1) |
			// | 0       M(2,1)  M(2,2) |
			diag[1] = M(1, 1);
			diag[2] = M(2, 2);

			subd[0] = M(1, 0);
			subd[1] = M(2, 1);
		}
		else
		{
			// grab the lower triangle of the matrix, minus a, which we don't need
			// (see above)
			// |       |
			// | b d   |
			// | c e f |
			const float b = M(1, 0);
			const float c = M(2, 0);
			const float d = M(1, 1);
			const float e = M(2, 1);
			const float f = M(2, 2);

			// normalize b and c to unit length and store in u and v
			// we want the lower-right block we create to be orthonormal
			const float L = sqrtf(b * b + c * c);
			const float u = b / L;
			const float v = c / L;

			Q._11 = 1.f;		Q._12 = 0.f;		Q._13 = 0.f;	Q._14 = 0.f;	// RIGHT		Vector3(1.0f, 0.0f, 0.0f)
			Q._21 = 0.f;		Q._22 = u;			Q._23 = v;		Q._24 = 0.f;	// UP			Vector3(0.0f, u, v)
			Q._31 = 0.f;		Q._32 = v;			Q._33 = -u;		Q._34 = 0.f;	// AT			Vector3(0.0f, v, -u)
			Q._41 = 0.f;		Q._42 = 0.f;		Q._43 = 0.f;	Q._44 = 1.f;	
			
			float q = 2 * u * e + v * (f - d);
			diag[1] = d + v * q;
			diag[2] = f - v * q;

			subd[0] = L;
			subd[1] = e - u * q;

			// so we see that T will actually be:
			//
			// | M(0,0)  L       0     |
			// | L       d+c*q   e-b*q |
			// | 0       e-b*q   f-c*q |
		}
	}  // End of SymmetricHouseholder3x3


	   //-------------------------------------------------------------------------------
	   // @ QLAlgorithm()
	   //-------------------------------------------------------------------------------
	   // 
	   //-------------------------------------------------------------------------------
	static int QLAlgorithm(i3::mat4& M, i3::vec3& diag, i3::vec3& subd)
	{
		// QL iteration with implicit shifting to reduce matrix from tridiagonal
		// to diagonal

		int L;
		for (L = 0; L < 3; L++)
		{
			// As this is an iterative process, we need to keep a maximum number of
			// iterations, just in case something is wrong - we cannot afford to
			// loop forever
			const int maxIterations = 32;

			int iter;
			for (iter = 0; iter < maxIterations; iter++)
			{
				int m;
				for (m = L; m <= 1; m++)
				{
					float dd = fabs(diag[m]) + fabs(diag[m + 1]);
					if (fabs(subd[m]) + dd == dd)
						break;
				}
				if (m == L)
					break;

				float g = (diag[L + 1] - diag[L]) / (2 * subd[L]);
				float r = sqrtf(g*g + 1);
				if (g < 0)
					g = diag[m] - diag[L] + subd[L] / (g - r);
				else
					g = diag[m] - diag[L] + subd[L] / (g + r);
				float s = 1, c = 1, p = 0;
				for (int i = m - 1; i >= L; i--)
				{
					float f = s*subd[i], b = c*subd[i];
					if (fabs(f) >= fabs(g))
					{
						c = g / f;
						r = sqrtf(c*c + 1);
						subd[i + 1] = f*r;
						c *= (s = 1 / r);
					}
					else
					{
						s = f / g;
						r = sqrtf(s*s + 1);
						subd[i + 1] = g*r;
						s *= (c = 1 / r);
					}
					g = diag[i + 1] - p;
					r = (diag[i] - g)*s + 2 * b*c;
					p = s*r;
					diag[i + 1] = g + p;
					g = c*r - b;

					for (int k = 0; k < 3; k++)
					{
						f = M(k, i + 1);
						M(k, i + 1) = s*M(k, i) + c*f;
						M(k, i) = c*M(k, i) - s*f;
					}
				}
				diag[L] -= p;
				subd[L] = g;
				subd[m] = 0;
			}

			// exceptional case - matrix took more iterations than should be 
			// possible to move to diagonal form
			if (iter == maxIterations)
				return 0;
		}

		return 1;
	}  // End of QLAlgorithm

	   //-------------------------------------------------------------------------------
	   // @ GetRealSymmetricEigenvectors()
	   //-------------------------------------------------------------------------------
	   // 
	   //-------------------------------------------------------------------------------
	void GetRealSymmetricEigenvectors(i3::vec3& v1, i3::vec3& v2, i3::vec3& v3,
		const i3::mat4& A)
	{
		i3::vec3 eigenvalues;
		i3::vec3 lowerTriangle;
		i3::mat4 Q;

		SymmetricHouseholder3x3(A, Q, eigenvalues, lowerTriangle);
		QLAlgorithm(Q, eigenvalues, lowerTriangle);

		// Sort the eigenvalues from greatest to smallest, and use these indices
		// to sort the eigenvectors
		int v1Index, v2Index, v3Index;

		if (eigenvalues[0] > eigenvalues[1])
		{
			if (eigenvalues[1] > eigenvalues[2])
			{
				v1Index = 0;
				v2Index = 1;
				v3Index = 2;
			}
			else if (eigenvalues[2] > eigenvalues[0])
			{
				v1Index = 2;
				v2Index = 0;
				v3Index = 1;
			}
			else
			{
				v1Index = 0;
				v2Index = 2;
				v3Index = 1;
			}
		}
		else // eigenvalues[1] >= eigenvalues[0]
		{
			if (eigenvalues[0] > eigenvalues[2])
			{
				v1Index = 1;
				v2Index = 0;
				v3Index = 2;
			}
			else if (eigenvalues[2] > eigenvalues[1])
			{
				v1Index = 2;
				v2Index = 1;
				v3Index = 0;
			}
			else
			{
				v1Index = 1;
				v2Index = 2;
				v3Index = 0;
			}
		}
		

		// Sort the eigenvectors into the output vectors
		v1.x = Q.m[0][v1Index];		v1.y = Q.m[1][v1Index];		v1.z = Q.m[2][v1Index];		// Q.GetColumn(v1Index);
		v2.x = Q.m[0][v2Index];		v2.y = Q.m[1][v2Index];		v2.z = Q.m[2][v2Index];		// Q.GetColumn(v2Index);
		v3.x = Q.m[0][v3Index];		v3.y = Q.m[1][v3Index];		v3.z = Q.m[2][v3Index];		// Q.GetColumn(v3Index);

		i3::vec3 v2_v3_cross;
		i3::vec3_cross(v2_v3_cross, v2, v3);
		
		// If the resulting eigenvectors are left-handed, negate the 
		// min-eigenvalue eigenvector to make it right-handed

		if (i3::vec3_dot(v1, v2_v3_cross) < 0.f)
			v3 *= -1.f;										// 	v3 = -v3;

	}  // End of GetRealSymmetricEigenvectors

	void MakeOBB(i3::mat4& outOBB, const i3::vec3* points, unsigned int nPoints)
	{
		
		i3::vec3 centroid;

		// compute covariance matrix
		i3::mat4 C;
		ComputeCovarianceMatrix(C, centroid, points, nPoints);

		// get basis vectors
		i3::vec3 basis[3];
		GetRealSymmetricEigenvectors(basis[0], basis[1], basis[2], C);

	//	i3::vec3_normalize_self(basis[0]);
	//	i3::vec3_normalize_self(basis[1]);
	//	i3::vec3_normalize_self(basis[2]);

		outOBB._14 = 0.f;	outOBB._24 = 0.f;	outOBB._34 = 0.f;	outOBB._44 = 1.f;

		reinterpret_cast<i3::vec3&>(outOBB._11) = basis[0];
		reinterpret_cast<i3::vec3&>(outOBB._21) = basis[1];
		reinterpret_cast<i3::vec3&>(outOBB._31) = basis[2];

	//	outOBB._14 = outOBB._24 = outOBB._34 = 0.f;
	//	outOBB._41 = outOBB._42 = outOBB._43 = 0.f;
	//	outOBB._44 = 1.f;
		
	//	mRotation.SetColumns(basis[0], basis[1], basis[2]);

		i3::vec3 min_val(FLT_MAX, FLT_MAX, FLT_MAX);
		i3::vec3 max_val(FLT_MIN, FLT_MIN, FLT_MIN);

		// compute min, max projections of box on axes
		// for each point do 
		unsigned int i;
		i3::vec3	 diff;

		for (i = 0; i < nPoints; ++i)
		{
			diff = points[i]; 		diff -= centroid;

			for (int j = 0; j < 3; ++j)
			{
				float length = i3::vec3_dot(diff, basis[j]);

				if (length > max_val[j])
				{
					max_val[j] = length;
				}
				else if (length < min_val[j])
				{
					min_val[j] = length;
				}
			}
		}

		// compute center, extents
		i3::vec3 center = centroid;
		i3::vec3 extent_mul2;

		for (i = 0; i < 3; ++i)
		{
			center += 0.5f*(min_val[i] + max_val[i])*basis[i];
			extent_mul2[i] = (max_val[i] - min_val[i]);			// * 0.5f
		}

//		outAABB.min() = center;		outAABB.min() -= extent;
//		outAABB.max() = center;		outAABB.max() += extent;

		reinterpret_cast<i3::vec3&>(outOBB._11) *= extent_mul2.x;
		reinterpret_cast<i3::vec3&>(outOBB._21) *= extent_mul2.y;
		reinterpret_cast<i3::vec3&>(outOBB._31) *= extent_mul2.z;
		reinterpret_cast<i3::vec3&>(outOBB._41) = center;

	}

	void	OBBTransformToAABB(const i3::mat4& mat_obb, const i3::mat4& mat_trans, i3::aabb& out)
	{
		i3::mat4 mat_trans2;

		i3::matrix_multiply(mat_obb, mat_trans, mat_trans2);

		// 먼저 center를 mat에 넣어 좌표 변경... Vec3TransformCoord함수를 호출해도 되지만, 일단 직접 곱하기로 했다..
		i3::vec3 result_center;
		result_center.x = mat_trans2.m[3][0];
		result_center.y = mat_trans2.m[3][1];
		result_center.z = mat_trans2.m[3][2];

		// extent의 경우 mat의 회전축과 연동되는데, 이때 각 axis축의 해당x,y,z성분이 음이라도 양값이 되도록 fabs처리를 한다...
		// Vec3TransformNormal과 유사하지만, 축성분이 음이면 모두 양값처리한다..
		i3::vec3 semiLength;
		semiLength.x = (fabs(mat_trans2.m[0][0]) + fabs(mat_trans2.m[1][0]) + fabs(mat_trans2.m[2][0])) * 0.5f;
		semiLength.y = (fabs(mat_trans2.m[0][1]) + fabs(mat_trans2.m[1][1]) + fabs(mat_trans2.m[2][1])) * 0.5f;
		semiLength.z = (fabs(mat_trans2.m[0][2]) + fabs(mat_trans2.m[1][2]) + fabs(mat_trans2.m[2][2])) * 0.5f;

		// 이제 min, max값을 구할수 있는데, 변환 중심점에 구해진 반길이 벡터를 빼거나 더하면 된다.
		out.min() = result_center;		out.min() -= semiLength;
		out.max() = result_center;		out.max() += semiLength;
	}

}

#pragma pop_macro("max")
#pragma pop_macro("min")

bool	i3GeometryAttr::_MakeOBBSet()
{
	if (m_pVA == nullptr)
		return false;
	
	INT32 VPP = 3;
	VEC3D p[3];
	i3GeometryResolver resolver;

	bool bNoRefCount = (GetRefCount() == 0);			// 처음부터 레퍼런스카운트를 갖지 못한 치명적인 경우를 처리..

	if (bNoRefCount)
		this->AddRef();

	resolver.setGeometryAttr(this);

	switch (resolver.getPrimitiveType())
	{
	case I3G_PRIM_LINELIST:
	case I3G_PRIM_LINESTRIP:
		VPP = 2;
		break;
	}
	
	const INT32 primCount = resolver.getPrimitiveCount();
	
	i3::vector<i3::vec3>	vecPoints(resolver.getPrimitiveCount() * VPP);
	INT32 vertCount = 0;

	for (INT32 i = 0; i < primCount; i++)
	{
		resolver.getPositions(i, p);

		for (INT32 j = 0; j < VPP; j++, ++vertCount)
		{
			i3::vec3& v = vecPoints[vertCount];
			v = reinterpret_cast<i3::vec3&>(p[j]);
		}
	}

	resolver.setGeometryAttr(nullptr);	

	if (bNoRefCount)
		this->_internal_SetRefCount(0);

	m_matOBBPtr = i3::make_shared<i3::mat4a>();

	MakeOBB(*m_matOBBPtr, &vecPoints[0], vecPoints.size());
	
	return true;
}

void	i3GeometryAttr::CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, const MATRIX* matWorld, bool twoSideEnabled)
{
	if (m_pVA == nullptr)			// 망...
		return;

	if (m_PrimType != I3G_PRIM_TRILIST)		// 이 경우만 지원하도록 한다...
		return;

	VEC3D vPos;

	const UINT32 numVert = m_pVA->GetCount();
	i3::vector<i3::vec3>	vecPos(numVert);

	bool isUnlockNeed = false;

	if (m_pVA->IsLocked() == false)
	{
		m_pVA->Lock();	isUnlockNeed = true;
	}

	for (UINT32 i = 0; i < numVert; ++i)
	{
		m_pVA->GetPosition(i, &vPos);			// 이건 로컬...
		i3Vector::TransformCoord(&vPos, &vPos, matWorld);
		vecPos[i] = i3::vec3(vPos.x, vPos.y, vPos.z);
	}
	
	if (isUnlockNeed)
		m_pVA->Unlock();
	
	if (m_pIA == nullptr)
	{
		occMeshSet->add_meshes(&vecPos[0], vecPos.size(), twoSideEnabled);
	}
	else
	{
		isUnlockNeed = false;

		if (m_pIA->IsLocked() == false)
		{
			m_pIA->Lock();	isUnlockNeed = true;
		}

		const UINT32 numVertIdx = m_pIA->GetCount();
		i3::vector<i3::uint32_t>	vecPosIdx(numVertIdx);

		for (UINT32 i = 0; i < numVertIdx; ++i)
		{
			UINT32 vertIdx = m_pIA->GetIndex(i);
			vecPosIdx[i] = vertIdx;
		}

		if (isUnlockNeed)
			m_pIA->Unlock();
		
		occMeshSet->add_meshes(&vecPosIdx[0], numVertIdx / 3, &vecPos[0], vecPos.size(), twoSideEnabled);
	}

}




void	i3GeometryAttr::CalcUpdateAABB(i3::aabb& outAABB, MATRIX* inputMatWorld, i3BoneMatrixListAttr* inputMatAttr) 
{
	if (m_AABBProcessorPtr != nullptr)
	{
		I3ASSERT(inputMatAttr != nullptr);
		m_AABBProcessorPtr->CalcWorldAABB(inputMatAttr, outAABB);
	}
	else
	{
		bool bValidBound = true;

		if (!m_matOBBPtr)
		{
			bValidBound = _MakeOBBSet();
		}
	
		if (bValidBound)
		{
			OBBTransformToAABB(*m_matOBBPtr, reinterpret_cast<const i3::mat4&>(*inputMatWorld), outAABB);
		}
		else
		{
			outAABB.reset();
		}
		
//		i3::aabb localAABB(m_BoundMin.x, m_BoundMin.y, m_BoundMin.z, m_BoundMax.x, m_BoundMax.y, m_BoundMax.z);
//		if (m_bValidBoundBox)
//			localAABB.transform(outAABB, reinterpret_cast<const i3::mat4&>(*inputMatWorld));
//		else
//			outAABB.reset();
	}
}






void i3GeometryAttr::BuildSkin( INT32 nBlendIndexCount, INT32 nBoneMatrixCount )
{
	i3SkinBuilder	builder;

	// 스키닝 옵티마이즈
	builder.Build( this);

	{
		_DestroyPrimGroup();

		m_pPrimGroup = builder.getGroupList();		// 쉐어드포인터의 대입이다.

		if( builder.getVertexArray() != nullptr)
		{
			I3_REF_CHANGE( m_pPrimVA, builder.getVertexArray());
			builder.getVertexArray()->setRestoreType( I3G_RESTORE_MEM);
		}

		#if defined( I3_DEBUG)
		m_pPrimVA->SetName( m_pVA->getResourceLoadPath());
		#endif
	}
}

void i3GeometryAttr::setGroupList( I3_DRAW_GROUP_LIST_PTR * group)
{
	m_pPrimGroup = *group;		// 쉐어드포인터의 대입이다.
}

void i3GeometryAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	// Vertex Array는 Clone을 생성하지 않고 공유한다.
	i3RenderAttr::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3GeometryAttr*>(pDest));

	i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pDest;

	pGeoAttr->SetVertexArray( GetVertexArray(), GetPrimitiveType(), GetPrimitiveCount(), GetStartIndex(), false);
	pGeoAttr->SetIndexArray( GetIndexArray());

	if( GetPrimitiveLengthArray() != nullptr)
	{
		UINT32 i;

		for( i = 0; i < GetPrimitiveCount(); i++)
		{
			pGeoAttr->SetPrimitiveLength( i, GetPrimitiveLength( i));
		}
	}

	pGeoAttr->m_VertexBlendMethod = getVertexBlendMethod();
	pGeoAttr->m_VertexMargine = m_VertexMargine;
	
	pGeoAttr->m_pBlendMatrixList = m_pBlendMatrixList;
	I3_SAFE_ADDREF(pGeoAttr->m_pBlendMatrixList);

	if( m_pPrimGroup != nullptr)
	{
		pGeoAttr->m_pPrimGroup = m_pPrimGroup;
//		m_pPrimGroup->AddRef();			// 셰어드포인터에서는 필요없음..
	}

	if( m_pPrimVA != nullptr)
	{
		pGeoAttr->m_pPrimVA = m_pPrimVA;
		I3_MUST_ADDREF(m_pPrimVA);
	}

	if( m_pPrimIA != nullptr)
	{
		pGeoAttr->m_pPrimIA = m_pPrimIA;
		I3_MUST_ADDREF(m_pPrimIA);
	}

	pGeoAttr->m_AABBProcessorPtr = this->m_AABBProcessorPtr;		// 신규기능..이 경우 공유가능하므로 공유시킨다..
	pGeoAttr->m_matOBBPtr = this->m_matOBBPtr;
}

void i3GeometryAttr::OnLostDevice( bool bLostDevice)
{
	//if( bLostDevice)
	/*{
		I3_SAFE_RELEASE( m_pPrimVA);
		I3_SAFE_RELEASE( m_pPrimIA);

		_DestroyPrimGroup();
	}*/

	m_pDrawShader = nullptr;
}

void i3GeometryAttr::OnRevive( i3RenderContext * pCtx)
{
	if((m_pVA != nullptr) && (m_pPrimVA == nullptr))
	{
		OptimizeForSkin();
	}
}

#if defined( I3_DEBUG)
void i3GeometryAttr::Dump(void)
{
	I3TRACE( "%s : %08X -  (%d)\n\n", meta()->class_name(), this, m_PrimCount);
}
#endif
