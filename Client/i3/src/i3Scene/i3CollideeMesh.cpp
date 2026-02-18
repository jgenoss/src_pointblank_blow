#include "i3SceneDef.h"
#include "i3CollideeMesh.h"
#include "i3CollideeLine.h"
#include "i3CollideeSphere.h"

I3_CLASS_INSTANCE( i3CollideeMesh);

i3CollideeMesh::~i3CollideeMesh(void)
{
	I3_SAFE_RELEASE( m_pPos);
	I3_SAFE_RELEASE( m_pNormal);

	if((m_pTri != nullptr) && (m_bRefTri == false))
	{
		I3MEM_SAFE_FREE_POINTER(m_pTri); 
	}

	removeAllReflectMap();

	m_TriCount = 0;
}

void i3CollideeMesh::SetPositionCount( INT32 count)
{
	if( m_pPos == nullptr)
	{
		m_pPos = i3Vector3Array::new_object();
	}

	m_pPos->SetVectorCount( count);
}

void i3CollideeMesh::SetPositionArray( i3Vector3Array * pArray)
{
	I3_REF_CHANGE( m_pPos, pArray);
}

void i3CollideeMesh::SetNormalCount( INT32 count)
{
	if( m_pNormal == nullptr)
	{
		m_pNormal = i3Vector3Array::new_object();
	}

	m_pNormal->SetVectorCount( count);
}

void i3CollideeMesh::SetNormalArray( i3Vector3Array * pArray)
{
	I3_REF_CHANGE( m_pNormal, pArray);
}

void i3CollideeMesh::SetTriangleCount( INT32 count)
{
	/*
	if( m_pTri != nullptr)
	{
		i3mem::Free( m_TriIdx );
		m_pTri = nullptr;
	}
	*/
	I3MEM_SAFE_FREE_POINTER( m_pTri ); 

	m_TriCount = count;
	
	if( count > 0)
	{
		//m_TriIdx = i3MemAlloc( sizeof(I3_COLLIDEE_TRI32) * count);
		m_pTri = (I3_COLLIDEE_TRI32 *) i3MemAlloc( sizeof(I3_COLLIDEE_TRI32) * count);
		I3ASSERT( m_pTri );
	}
}

void i3CollideeMesh::Transform( MATRIX * pMatrix, i3Collidee * pCol)
{
	UINT32 i;
	i3CollideeMesh * pDest = (i3CollideeMesh *) pCol;

	I3ASSERT( i3::same_of<i3CollideeMesh*>(pCol));

	if( pDest->GetPositionArray() == nullptr)
	{
		// 아직 생성되지 않았기 때문에..
		pDest->SetTerrainType( GetTerrainType());
		pDest->SetTrigger( GetTrigger());
		pDest->SetPositionCount( GetPositionCount());
		pDest->SetNormalCount( GetNormalCount());
		pDest->m_pTri = m_pTri;
		pDest->m_bRefTri = true;
		pDest->m_TriCount = m_TriCount;
	}

	// Transform Positions
	if( m_pPos != nullptr)
	{
		VEC3D * pSrcVec, * pDestVec;
		VEC3D vmin, vmax;

		pSrcVec = GetPositionArray()->GetVectorArray();
		pDestVec = pDest->GetPositionArray()->GetVectorArray();

		for( i = 0; i < m_pPos->GetVectorCount(); i++)
		{
			i3Vector::TransformCoord( pDestVec, pSrcVec, pMatrix);

			if( i == 0)
			{
				i3Vector::Copy( &vmin, pDestVec);
				i3Vector::Copy( &vmax, pDestVec);
			}
			else
			{
				i3Vector::Minimize( &vmin, &vmin, pDestVec);
				i3Vector::Maximize( &vmax, &vmax, pDestVec);
			}

			pSrcVec++;
			pDestVec++;
		} 

		i3Vector::Add( &(pDest->m_Center), &vmin, &vmax);
		i3Vector::Scale( &(pDest->m_Center), &(pDest->m_Center), 0.5f);

		i3Vector::Sub( &vmin, &vmin, &pDest->m_Center);
		pDest->m_Radius = i3Vector::Length( &vmin);
	}

	// Transform Normals
	if( m_pNormal != nullptr)
	{
		VEC3D * pSrcVec, * pDestVec;

		pSrcVec = GetNormalArray()->GetVectorArray();
		pDestVec = pDest->GetNormalArray()->GetVectorArray();

		for( i = 0; i < m_pNormal->GetVectorCount(); i++)
		{
			i3Vector::TransformNormal( pDestVec, pSrcVec, pMatrix);
			pSrcVec++;
			pDestVec++;
		}
	}

}

void i3CollideeMesh::addReflectMap( i3TextureCube * pTex)
{
	I3_MUST_ADDREF( pTex);
	m_ReflectMapList.push_back( pTex);
}

void i3CollideeMesh::removeAllReflectMap(void)
{
	INT32 i;
	i3TextureCube * pTex;

	for( i = 0; i < getReflectMapCount(); i++)
	{
		pTex = getReflectMap( i);

		I3_MUST_RELEASE( pTex);
	}

	m_ReflectMapList.clear();
}

void i3CollideeMesh::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pPos != nullptr)
	{
		m_pPos->OnBuildObjectList( List);
	}

	if( m_pNormal != nullptr)
	{
		m_pNormal->OnBuildObjectList( List);
	}

	INT32 i;

	for( i = 0; i < getReflectMapCount(); i++)
	{
		i3TextureCube * pTex = getReflectMap( i);

		pTex->OnBuildObjectList( List);
	}

	i3Collidee::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 COLLIDEE_TRI
	{
		UINT16			m_Index[4] = { 0 };
	};

	struct ALIGN4 COLLIDEE_TRI2
	{
		UINT16		m_Index[4] = { 0 };
		UINT8		m_Reverb = 0;					// 0 - No Reverb, 1, 2, 3 ..
		INT8		m_Elasticity = 0;				// -128 ~ 127
		UINT8		m_Terrain = 0;
		UINT8		pad = 0;
	};

	struct ALIGN4 COLLIDEE_TRI3
	{
		UINT16		m_Index[4] = { 0 };
		UINT8		m_Reverb = 0;					// 0 - No Reverb, 1, 2, 3 ..
		INT8		m_Elasticity = 0;				// -128 ~ 127
		UINT8		m_Terrain = 0;
		UINT8		pad = 0;
		UINT8		m_Color[4] = { 0 };
	};

	struct ALIGN4 COLLIDEE_TRI4
	{
		UINT32		m_Index[4] = { 0 };
		UINT8		m_Reverb = 0;					// 0 - No Reverb, 1, 2, 3 ..
		INT8		m_Elasticity = 0;				// -128 ~ 127
		UINT8		m_Terrain = 0;
		INT8		m_ReflectMapIndex = 0;
		UINT8		m_Color[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#if defined( I3_COMPILER_VC)
#pragma pack( push, 2)
#endif

namespace pack
{
	struct ALIGN2 COLLIDEE_MESH
	{
		UINT16		m_Version = 5;
		OBJREF		m_pVertex = 0;
		OBJREF		m_pNormal = 0;
		UINT32		m_TriCount = 0;
	};

	struct ALIGN2 COLLIDEE_MESH_HEADER
	{
		UINT32		m_ReflectMapCount = 0;
		UINT32		pad[7] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3CollideeMesh::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
		
	UINT32 i;

	Result = i3Collidee::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::COLLIDEE_MESH data;	
	data.m_pVertex =	(OBJREF) pResFile->GetObjectPersistID( m_pPos);
	data.m_pNormal =	(OBJREF) pResFile->GetObjectPersistID( m_pNormal);
	data.m_TriCount =	m_TriCount;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);

	pack::COLLIDEE_MESH_HEADER header;
	header.m_ReflectMapCount =			getReflectMapCount();

	Rc = pResFile->Write( &header, sizeof(header));
	I3_CHKIO( Rc);

	// Triangles
	for( i = 0; i < m_TriCount; i++)
	{
		pack::COLLIDEE_TRI4 tri;

		tri.m_Index[0]			= m_pTri[i].m_Index[0];
		tri.m_Index[1]			= m_pTri[i].m_Index[1];
		tri.m_Index[2]			= m_pTri[i].m_Index[2];
		tri.m_Index[3]			= m_pTri[i].m_NormalIndex;
		tri.m_Terrain			= m_pTri[i].m_Terrain;
		tri.m_Reverb			= m_pTri[i].m_Reverb;
		tri.m_Elasticity		= m_pTri[i].m_Elasticity;
		tri.m_Color[0]			= m_pTri[i].m_Color[0];
		tri.m_Color[1]			= m_pTri[i].m_Color[1];
		tri.m_Color[2]			= m_pTri[i].m_Color[2];
		tri.m_Color[3]			= m_pTri[i].m_Color[3];
		tri.m_ReflectMapIndex	= m_pTri[i].m_ReflectMapIndex;

		Rc = pStream->Write( &tri, sizeof(pack::COLLIDEE_TRI4));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeMesh::OnSave()", "Could not write triangle data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Reflect Map Reference
	for( i = 0; i < (UINT32) getReflectMapCount(); i++)
	{
		OBJREF ref =	(OBJREF) pResFile->GetObjectPersistID( getReflectMap(i));

		Rc = pResFile->Write( &ref, sizeof(ref));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3CollideeMesh::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::COLLIDEE_MESH data;
	pack::COLLIDEE_MESH_HEADER header;
	UINT32 i;

	Result = i3Collidee::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	{
		i3Vector3Array * pArray;

		Rc = pResFile->Read( &data, sizeof(data));
		I3_CHKIO( Rc);

		// Position Array
		if( data.m_pVertex != 0)
		{
			pArray = (i3Vector3Array *) pResFile->FindObjectByID( data.m_pVertex);
			I3ASSERT( pArray != nullptr);

			SetPositionArray( pArray);

			VEC3D vmin, vmax;

			for( i = 0; i < pArray->GetVectorCount(); i++)
			{
				if( i == 0)
				{
					i3Vector::Copy( &vmin, pArray->GetVector( i));
					i3Vector::Copy( &vmax, pArray->GetVector( i));
				}
				else
				{
					i3Vector::Minimize( &vmin, &vmin, pArray->GetVector( i));
					i3Vector::Maximize( &vmax, &vmax, pArray->GetVector( i));
				}
			}

			i3Vector::Add( &m_Center, &vmin, &vmax);
			i3Vector::Scale( &m_Center, &m_Center, 0.5f);

			i3Vector::Sub( &vmin, &vmin, &m_Center);
			m_Radius = i3Vector::Length( &vmin);
		}

		// Normal Array
		if( data.m_pNormal != 0)
		{
			pArray = (i3Vector3Array *) pResFile->FindObjectByID( data.m_pNormal);
			I3ASSERT( pArray != nullptr);

			SetNormalArray( pArray);
		}

		// Triangle Count
		SetTriangleCount( data.m_TriCount);
	}

	if( data.m_Version > 4)
	{
		Rc = pResFile->Read( &header, sizeof(header));
		I3_CHKIO( Rc);
	}
	else
	{
		header.m_ReflectMapCount = 0;
	}

	// Triangles
	switch( data.m_Version)
	{
		case 1 :
			for( i = 0; i < m_TriCount; i++)
			{
				pack::COLLIDEE_TRI tri;

				Rc = pStream->Read( &tri, sizeof(pack::COLLIDEE_TRI));
				if( Rc == STREAM_ERR)
				{
					i3Log( "i3CollideeMesh::OnLoad()", "Could not read triangle data.");
					return STREAM_ERR;
				}
				Result += Rc;

				m_pTri[i].m_Index[0] = tri.m_Index[0];
				m_pTri[i].m_Index[1] = tri.m_Index[1];
				m_pTri[i].m_Index[2] = tri.m_Index[2];
				m_pTri[i].m_NormalIndex = tri.m_Index[3];
			}
			break;

		case 2 :
			for( i = 0; i < m_TriCount; i++)
			{
				pack::COLLIDEE_TRI2 tri;

				Rc = pStream->Read( &tri, sizeof(pack::COLLIDEE_TRI2));
				if( Rc == STREAM_ERR)
				{
					i3Log( "i3CollideeMesh::OnLoad()", "Could not read triangle data.");
					return STREAM_ERR;
				}
				Result += Rc;

				m_pTri[i].m_Index[0] = tri.m_Index[0];
				m_pTri[i].m_Index[1] = tri.m_Index[1];
				m_pTri[i].m_Index[2] = tri.m_Index[2];
				m_pTri[i].m_NormalIndex = tri.m_Index[3];
				m_pTri[i].m_Terrain	= tri.m_Terrain;
				m_pTri[i].m_Reverb	= tri.m_Reverb;
				m_pTri[i].m_Elasticity = tri.m_Elasticity;
			}
			break;

		case 3 :
			for( i = 0; i < m_TriCount; i++)
			{
				pack::COLLIDEE_TRI3 tri;

				Rc = pStream->Read( &tri, sizeof(pack::COLLIDEE_TRI3));
				if( Rc == STREAM_ERR)
				{
					i3Log( "i3CollideeMesh::OnLoad()", "Could not read triangle data.");
					return STREAM_ERR;
				}
				Result += Rc;

				m_pTri[i].m_Index[0] = tri.m_Index[0];
				m_pTri[i].m_Index[1] = tri.m_Index[1];
				m_pTri[i].m_Index[2] = tri.m_Index[2];
				m_pTri[i].m_NormalIndex = tri.m_Index[3];
				m_pTri[i].m_Terrain	= tri.m_Terrain;
				m_pTri[i].m_Reverb	= tri.m_Reverb;
				m_pTri[i].m_Elasticity = tri.m_Elasticity;
				m_pTri[i].m_Color[0]	= tri.m_Color[0];
				m_pTri[i].m_Color[1]	= tri.m_Color[1];
				m_pTri[i].m_Color[2]	= tri.m_Color[2];
				m_pTri[i].m_Color[3]	= tri.m_Color[3];
			}
			break;

		default:
			for( i = 0; i < m_TriCount; i++)
			{
				pack::COLLIDEE_TRI4 tri;

				Rc = pStream->Read( &tri, sizeof(pack::COLLIDEE_TRI4));
				if( Rc == STREAM_ERR)
				{
					i3Log( "i3CollideeMesh::OnLoad()", "Could not read triangle data.");
					return STREAM_ERR;
				}
				Result += Rc;

				m_pTri[i].m_Index[0]		= tri.m_Index[0];
				m_pTri[i].m_Index[1]		= tri.m_Index[1];
				m_pTri[i].m_Index[2]		= tri.m_Index[2];
				m_pTri[i].m_NormalIndex		= tri.m_Index[3];
				m_pTri[i].m_Terrain			= tri.m_Terrain;
				m_pTri[i].m_Reverb			= tri.m_Reverb;
				m_pTri[i].m_Elasticity		= tri.m_Elasticity;
				m_pTri[i].m_Color[0]		= tri.m_Color[0];
				m_pTri[i].m_Color[1]		= tri.m_Color[1];
				m_pTri[i].m_Color[2]		= tri.m_Color[2];
				m_pTri[i].m_Color[3]		= tri.m_Color[3];
				m_pTri[i].m_ReflectMapIndex	= tri.m_ReflectMapIndex;
			}
			break;
	}

	for( i = 0; i < header.m_ReflectMapCount; i++)
	{
		OBJREF ref;

		Rc = pResFile->Read( &ref, sizeof(ref));
		I3_CHKIO( Rc);

		i3TextureCube * pTex = (i3TextureCube *) pResFile->FindObjectByID( ref);
		
		addReflectMap( pTex);
	}

	return Result;
}

UINT32 i3CollideeMesh::TestLine( i3CollideeLine * pLine, I3_COLLISION_RESULT_CONTEXT * pCtx)
{
	UINT32 i;
	UINT32 count = 0;
	VEC3D * pPos1, *pPos2, *pPos3, * pNormal;
	I3_COLLIDEE_TRI32* pTri;
	I3_COLLISION_RESULT * pResult;
	bool bRet;

    for( i = 0; i < m_TriCount; i++)
	{
        pTri = GetTriangle(i);

		pPos1 = m_pPos->GetVector( pTri->m_Index[0]);
		pPos2 = m_pPos->GetVector( pTri->m_Index[1]);
		pPos3 = m_pPos->GetVector( pTri->m_Index[2]);

		if( pCtx->m_Style & I3_COLLISION_STYLE_BOTH_SIDE )
		{
			bRet = i3Math::TestTriPointBothSide( pPos1, pPos2, pPos3, pLine->GetStart(), pLine->GetDir() );
		}
		else
		{
#if 0
			VEC3D temp;
			i3Vector::Scale( &temp, pLine->GetDir(), -1.0f);
			bRet = i3Math::TestTriPoint( pPos1, pPos2, pPos3, pLine->GetEnd(), &temp );
#else
			bRet = i3Math::TestTriPoint( pPos1, pPos3, pPos2, pLine->GetStart(), pLine->GetDir() );
#endif
		}

		if( bRet )
		{
			REAL32 t;
			VEC3D pc;

			pNormal = m_pNormal->GetVector( pTri->m_NormalIndex);

			if( IntersectTriLine( pLine->GetStart(), pLine->GetEnd(), pNormal, pPos1, &pc, &t))
			{
				if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
				{
					if( t < pCtx->m_LastT)
					{
						pResult = pCtx->m_pResult;

						i3Vector::Copy( & pResult->m_Intersect, &pc);
						i3Vector::Copy( & pResult->m_Normal, pNormal);

						pResult->m_T = pCtx->m_LastT = t;

						pResult->m_pSrc = this;
						pResult->m_pDest = pLine;
						pResult->m_pTri = pTri;

						pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

						count = pCtx->m_Count = 1;
					}
				}
				else
				{
					if((pCtx->m_Count + 1) > pCtx->m_LimitCount)
					{
						// 허용된 Result의 개수를 넘어섰다. 더 이상 계산할 필요가 없다.
						return count;
					}

					pResult = pCtx->m_pResult + pCtx->m_Count;

					if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_INTERSECT )
						i3Vector::Copy( & pResult->m_Intersect, &pc);

					if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_NORMAL )
						i3Vector::Copy( & pResult->m_Normal, pNormal);

					pResult->m_T = t;
					pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

					pResult->m_pSrc = this;
					pResult->m_pDest = pLine;
					pResult->m_pTri = pTri;

					count++;
					pCtx->m_Count++;
				}
			}
		}
	}

	return count;
}

UINT32 i3CollideeMesh::TestSphere( i3CollideeSphere * pCollidee, I3_COLLISION_RESULT_CONTEXT * pCtx)
{
	UINT32 i, c = 0;
	I3_COLLIDEE_TRI32* pTri;
	VEC3D* pPos1, *pPos2, *pPos3, * pSwap;
	VEC3D* pN;
	VEC3D  Temp, ProjCenter;
	REAL32 dot, dist1, dist2, dist3;
	I3_COLLISION_RESULT * pResult;

    for( i = 0; i < m_TriCount; i++)
	{
        pTri = GetTriangle(i);
		pPos1 = m_pPos->GetVector(pTri->m_Index[0]);
		pPos2 = m_pPos->GetVector(pTri->m_Index[1]);
		pPos3 = m_pPos->GetVector(pTri->m_Index[2]);
		pN = m_pNormal->GetVector(pTri->m_NormalIndex);
	
		i3Vector::Sub( &Temp, pCollidee->GetCenter(), pPos1);
		dot = i3Vector::Dot(&Temp, pN);

		// Triangle 평면과 Sphere의 거리 측정.
		if( i3Math::abs( dot) < pCollidee->GetRadius() )
		{
			// Sphere의 중앙점을 Triangle Plane에 Projection했을 때의 점을 구하고,
			// 이 점이 Triangle의 안에 포함하는지를 확인한다. 
			{
				i3Vector::Scale( &ProjCenter, pN, -dot);
				i3Vector::Add( &ProjCenter, &ProjCenter, pCollidee->GetCenter());

				if( i3Math::TestTriPoint( pPos1, pPos2, pPos3, &ProjCenter, pN))
				{
					REAL32 t = i3Math::div( dot, pCollidee->GetRadius());

					if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
					{
						pResult = pCtx->m_pResult;

						if( t <= pCtx->m_LastT)
						{
							i3Vector::Copy( & pResult->m_Intersect, &ProjCenter);
							i3Vector::Copy( & pResult->m_Normal, pN);
							pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

							pResult->m_pSrc = this;
							pResult->m_pDest = pCollidee;
							pResult->m_pTri = pTri;

							pCtx->m_LastT = pResult->m_T = t;
							pCtx->m_Count = c = 1;
						}
					}
					else
					{
						if( (pCtx->m_Count + 1) > pCtx->m_LimitCount)
							return c;

						pResult = pCtx->m_pResult + pCtx->m_Count;

						i3Vector::Copy( & pResult->m_Intersect, &ProjCenter);
						i3Vector::Copy( & pResult->m_Normal, pN);
						pResult->m_T = t;
						pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;
						
						pResult->m_pSrc = this;
						pResult->m_pDest = pCollidee;
						pResult->m_pTri = pTri;

						c++;
						pCtx->m_Count++;
					}
					continue;
				}
			}

			// Edge 검사.
			// 검사 회수를 줄이기 위해, 각 Edge 중에 Sphere와 가장 근접한 것을 구하여
			// 그것과 직선-점 교차 계산을 한다.
			{
				VEC3D Int1, Int2;
				REAL32 t1, t2;
				bool bR1, bR2, bHit;

				i3Vector::Sub( &Temp, pPos1, pCollidee->GetCenter());
				dist1 = i3Vector::LengthSq( &Temp);

				i3Vector::Sub( &Temp, pPos2, pCollidee->GetCenter());
				dist2 = i3Vector::LengthSq( &Temp);

				i3Vector::Sub( &Temp, pPos3, pCollidee->GetCenter());
				dist3 = i3Vector::LengthSq( &Temp);

				if( dist1 < dist2)
				{
					if( dist1 < dist3)
					{	// dist1이 가장 작다.
						// pPos1-pPos2, pPos1-pPos3 검사
					}
					else
					{	// dist3가 가장 작다.
						// pPos3-pPos1, pPos3-pPos2 검사
						pSwap = pPos3;	pPos3 = pPos1;	pPos1 = pSwap;
					}
				}
				else
				{
					if( dist2 < dist3)
					{	// dist2가 가장 작다.
						pSwap = pPos2;	pPos2 = pPos1;	pPos1 = pSwap;
					}
					else
					{	// dist3가 가장 작다.
						pSwap = pPos3;	pPos3 = pPos1;	pPos1 = pSwap;
					}
				}

				// 1.
				bR1 = i3Math::IntersectLineToSphere( pPos1, pPos2, pCollidee->GetCenter(), pCollidee->GetSquareRadius(), &Int1, &t1);
				bR2 = i3Math::IntersectLineToSphere( pPos1, pPos3, pCollidee->GetCenter(), pCollidee->GetSquareRadius(), &Int2, &t2);
				bHit = bR1 || bR2;

				if( bHit)
				{
					if( bR1 == true)
					{
						i3Vector::Sub( &Temp, pCollidee->GetCenter(), &Int1);
						t1 = i3Vector::Length( &Temp);

						if( bR2 == true)
						{
							i3Vector::Sub( &Temp, pCollidee->GetCenter(), &Int2);
							t2 = i3Vector::Length( &Temp);

							if( t2 < t1)
							{
								i3Vector::Copy( &Int1, &Int2);
								t1 = t2;
							}
						}
					}
					else
					{
						// R2 Hit
						i3Vector::Copy( &Int1, &Int2);

						i3Vector::Sub( &Temp, pCollidee->GetCenter(), &Int1);
						t1 = i3Vector::Length( &Temp);
					}

					if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
					{
						if( t1 <= pCtx->m_LastT)
						{
							pResult = pCtx->m_pResult;

							i3Vector::Copy( & pResult->m_Intersect, &Int1);
							i3Vector::Copy( & pResult->m_Normal, pN);
							pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

							pResult->m_pSrc = this;
							pResult->m_pDest = pCollidee;
							pResult->m_pTri = pTri;

							pCtx->m_LastT = pResult->m_T = t1;
							pCtx->m_Count = c = 1;
						}
					}
					else
					{
						if( (pCtx->m_Count + 1) > pCtx->m_LimitCount)	// Result 개수 제한 초과
							return c;

						pResult = pCtx->m_pResult + pCtx->m_Count;

						i3Vector::Copy( &pResult->m_Intersect, &Int1);
						i3Vector::Copy( & pResult->m_Normal, pN);
						pResult->m_T = t1;
						pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

						pResult->m_pSrc = this;
						pResult->m_pDest = pCollidee;
						pResult->m_pTri = pTri;

						c++;
						pCtx->m_Count++;
					}
					continue;
				}
			}
		}
	}

	return c;
}

UINT32 i3CollideeMesh::TestCylinder( i3CollideeCylinder * pCollidee, I3_COLLISION_RESULT * pResult, UINT32 ResultLimit, UINT32 Style)
{
	/*
	UINT32 i, c = 0;
	I3_COLLIDEE_TRI32* pTri;
	VEC3D* pPos1, *pPos2, *pPos3;
	VEC3D* pN;
	REAL32 dot;
	VEC3D end;

	i3Vector::Scale( &end, pCollidee->GetDirection(), pCollidee->GetLength());
	i3Vector::Add( &end, &end, pCollidee->GetStart());

    for( i = 0; i < m_TriCount; i++)
	{
        pTri = GetTriangle(i);
		pPos1 = m_pPos->GetVector(pTri->m_Index[0]);
		pPos2 = m_pPos->GetVector(pTri->m_Index[1]);
		pPos3 = m_pPos->GetVector(pTri->m_Index[2]);
		pN = m_pNormal->GetVector(pTri->m_NormalIndex);

		dot = i3Vector::Dot( pN, pCollidee->GetDirection());
	
		if( dot == 0.0f)
		{
			// Cylinder의 축과 Triangle이 평행하다.
			//i3Math::GetPerpendicularPoint( pCollidee->GetStart(), &end, pPos1, 
		}
		else
		{
		}
	}

	return c;
	*/
	return 0;
}

UINT32 i3CollideeMesh::TestCapsule( i3CollideeCapsule * pCollidee, I3_COLLISION_RESULT_CONTEXT * pCtx)
{
	//	기본적으로 TestSphere()와 같으며 최초로 캡슐과의 교점을 구해 Center로 사용한다.
	UINT32 i, c = 0;
	I3_COLLIDEE_TRI32* pTri;
	VEC3D* pPos1, *pPos2, *pPos3, * pSwap;
	VEC3D* pN;
	VEC3D  Temp, ProjCenter, vIntersect, norm;
	REAL32 dot, dist1, dist2, dist3, t;
	I3_COLLISION_RESULT * pResult;

	//	먼저 박스의 Radius로 캡슐과 충돌 체크를 해서 교점을 구한다.
	if( !i3Math::IntersectCapsuleToSphere( pCollidee->getStart(), pCollidee->getDirection(), pCollidee->getLength(), pCollidee->getRadius(), 
											GetCenter(), GetRadius(), &vIntersect, &norm, &t)) 
	{
		return c;
	}

    for( i = 0; i < m_TriCount; i++)
	{
        pTri = GetTriangle(i);
		pPos1 = m_pPos->GetVector(pTri->m_Index[0]);
		pPos2 = m_pPos->GetVector(pTri->m_Index[1]);
		pPos3 = m_pPos->GetVector(pTri->m_Index[2]);
		pN = m_pNormal->GetVector(pTri->m_NormalIndex);
	
		i3Vector::Sub( &Temp, &vIntersect, pPos1); 
		dot = i3Vector::Dot(&Temp, pN);

		// Triangle 평면과 Sphere의 거리 측정.
		if( i3Math::abs( dot) <= pCollidee->getRadius() )
		{
			// Sphere의 중앙점을 Triangle Plane에 Projection했을 때의 점을 구하고,
			// 이 점이 Triangle의 안에 포함하는지를 확인한다. 
			{
				i3Vector::Scale( &ProjCenter, pN, -dot);
				i3Vector::Add( &ProjCenter, &ProjCenter, &vIntersect);

				if( i3Math::TestTriPoint( pPos1, pPos2, pPos3, &ProjCenter, pN))
				{
					REAL32 t = i3Math::div( dot, pCollidee->getRadius());

					if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
					{
						pResult = pCtx->m_pResult;

						if( t <= pCtx->m_LastT)
						{
							i3Vector::Copy( & pResult->m_Intersect, &ProjCenter);
							i3Vector::Copy( & pResult->m_Normal, pN);
							pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

							pResult->m_pSrc = this;
							pResult->m_pDest = pCollidee;
							pResult->m_pTri = pTri;

							pCtx->m_LastT = pResult->m_T = t;
							pCtx->m_Count = c = 1;
						}
					}
					else
					{
						if( (pCtx->m_Count + 1) > pCtx->m_LimitCount)
							return c;

						pResult = pCtx->m_pResult + pCtx->m_Count;

						i3Vector::Copy( & pResult->m_Intersect, &ProjCenter);
						i3Vector::Copy( & pResult->m_Normal, pN);
						pResult->m_T = t;
						pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;
						
						pResult->m_pSrc = this;
						pResult->m_pDest = pCollidee;
						pResult->m_pTri = pTri;

						c++;
						pCtx->m_Count++;
					}
					continue;
				}
			}

			// Edge 검사.
			// 검사 회수를 줄이기 위해, 각 Edge 중에 Sphere와 가장 근접한 것을 구하여
			// 그것과 직선-점 교차 계산을 한다.
			{
				VEC3D Int1, Int2;
				REAL32 t1 = 0.f; 
				REAL32 t2 = 0.f;
				bool bR1, bR2, bHit;

				i3Vector::Sub( &Temp, pPos1, &vIntersect);
				dist1 = i3Vector::LengthSq( &Temp);

				i3Vector::Sub( &Temp, pPos2, &vIntersect);
				dist2 = i3Vector::LengthSq( &Temp);

				i3Vector::Sub( &Temp, pPos3, &vIntersect);
				dist3 = i3Vector::LengthSq( &Temp);

				if( dist1 < dist2)
				{
					if( dist1 < dist3)
					{	// dist1이 가장 작다.
						// pPos1-pPos2, pPos1-pPos3 검사
					}
					else
					{	// dist3가 가장 작다.
						// pPos3-pPos1, pPos3-pPos2 검사
						pSwap = pPos3;	pPos3 = pPos1;	pPos1 = pSwap;
					}
				}
				else
				{
					if( dist2 < dist3)
					{	// dist2가 가장 작다.
						pSwap = pPos2;	pPos2 = pPos1;	pPos1 = pSwap;
					}
					else
					{	// dist3가 가장 작다.
						pSwap = pPos3;	pPos3 = pPos1;	pPos1 = pSwap;
					}
				}

				// 1.
				bR1 = i3Math::IntersectLineToSphere( pPos1, pPos2, &vIntersect, pCollidee->getSquareRadius(), &Int1, &t1);
				bR2 = i3Math::IntersectLineToSphere( pPos1, pPos3, &vIntersect, pCollidee->getSquareRadius(), &Int2, &t2);
				bHit = bR1 || bR2;

				if( bR1 == true)
				{
					if( (t1 > t2) && (bR2 == true))
					{
						// R2 (pPos1 - pPos3) hit
						i3Vector::Copy( &Int1, &Int2);
						t1 = t2;
					}
				}
				else
				{
					if( bR2 == true)
					{
						// R2 Hit
						i3Vector::Copy( &Int1, &Int2);
						t1 = t2;
					}
				}

				if( bHit)
				{
					if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
					{
						if( t1 <= pCtx->m_LastT)
						{
							pResult = pCtx->m_pResult;

							i3Vector::Copy( & pResult->m_Intersect, &Int1);
							i3Vector::Copy( & pResult->m_Normal, pN);
							pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

							pResult->m_pSrc = this;
							pResult->m_pDest = pCollidee;
							pResult->m_pTri = pTri;

							pCtx->m_LastT = pResult->m_T = t1;
							pCtx->m_Count = c = 1;
						}
					}
					else
					{
						if( (pCtx->m_Count + 1) > pCtx->m_LimitCount)	// Result 개수 제한 초과
							return c;

						pResult = pCtx->m_pResult + pCtx->m_Count;

						i3Vector::Copy( &pResult->m_Intersect, &Int1);
						i3Vector::Copy( & pResult->m_Normal, pN);
						pResult->m_T = t1;
						pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

						pResult->m_pSrc = this;
						pResult->m_pDest = pCollidee;
						pResult->m_pTri = pTri;

						c++;
						pCtx->m_Count++;
					}
					continue;
				}
			}
		}
	}

	return c;
}


