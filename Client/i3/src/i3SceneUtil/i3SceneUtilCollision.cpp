#include "i3SceneUtil.h"

#define SceneUtilBuildLog				if(logger) logger->Log

using namespace i3SceneUtil;


BuildLogger::BuildLogger(DWORD logFlag)
{
	m_flag = logFlag;
	Reset();
}

void BuildLogger::SetSection(const char* fmt, ...)
{
	m_curLogSection[0] = 0;

	va_list arg;
	va_start(arg, fmt);

	vsprintf_s(m_curLogSection, fmt, arg);
	va_end(arg);
}

void BuildLogger::Log(DWORD flag, const char* reason, const char* dst, const char* fmt, ...)
{
	if( m_curLogSection[0] == 0)
		return;

	if(flag && !(m_flag & flag))
		return;

	if(flag & ELF_INC_PHYSHAPE_GEN)
		m_numGenColliders++;

	if(flag & ELF_INC_PHYSHAPE_MISS)
		m_numGenFailColliders++;

	if(dst == nullptr || *dst == 0)
		dst = "#NONE#";

	LogMap::iterator x = m_log.find(m_curLogSection);
	if(x == m_log.end())
	{
		LogMap::value_type val(std::string((char*)m_curLogSection), LogRow());
		LogMap::_Pairib rst = m_log.insert(val);
		x = rst.first;
	}

	LogRow& row = (*x).second;

	INT32 lenFmt = strlen(fmt) + 2;
	INT32 bufSiz = lenFmt + 1024;

	char* finalBuf = new char[bufSiz];
	char* buf = new char[bufSiz];

	va_list arg;
	va_start(arg, fmt);

	if(fmt)
		vsprintf_s(buf, bufSiz, fmt, arg);
	else
	{
		buf[0] = 0x32;
		buf[1] = 0;
	}

	va_end(arg);

	sprintf_s(finalBuf, bufSiz, "%d|%s|%s|%s", flag, dst, reason, buf);
	LogRow::value_type val(dst, finalBuf);
	row.insert(val);

	delete [] buf;
	delete [] finalBuf;
}

void BuildLogger::Save(const char* path)
{
	
}

void BuildLogger::ShowMsgBox(HWND hWnd)
{
	if(m_log.empty())
		return;

	i3::string msg;

	LogMap::iterator x = m_log.begin();
	while(x != m_log.end())
	{
		LogRow& row = (*x).second;
		msg += "\n";
		msg += (*x).first.c_str();
		msg += "\n";

		for(LogRow::iterator y=row.begin(); y!=row.end(); y = row.upper_bound((*y).first))
		{
			LogRow::_Pairii rng = row.equal_range((*y).first);

			for(LogRow::iterator z=rng.first; z!=rng.second; z++)
			{
				const char* str = (*z).second.c_str();

				if(str && *str != '0')
				{
					msg += ((*rng.first).first).c_str();
					msg += "\t";

					break;
				}
			}
		}

		msg += "\n";
		x++;
	}

	if(!msg.empty())
		MessageBox(hWnd, msg.c_str(), "ERROR", MB_ICONERROR);
}

void BuildLogger::Reset()
{
	m_log.clear();
	memset(m_curLogSection, 0, sizeof(m_curLogSection));

	m_numGenColliders = 0;
	m_numGenFailColliders = 0;
}

INT32 BuildLogger::GenerateStatusMsg(BuildLogger::MsgStatus* rst, DWORD flag)
{
	INT numAdd = 0;

	LogMap::iterator x = m_log.begin();
	while(x != m_log.end())
	{
		{
			const char* curLogStr = (*x).first.c_str();
			INT32 len = strlen(curLogStr) + 100;
			char* buf = new char[len];
						
			sprintf_s(buf, len, "@ %s", curLogStr);
			rst->push_back(std::string(buf));
			delete [] buf;
		}		

		LogRow& row = (*x).second;

		for(LogRow::iterator y=row.begin(); y!=row.end(); y = row.upper_bound((*y).first))
		{
			LogRow::_Pairii rng = row.equal_range((*y).first);
			for(LogRow::iterator z=rng.first; z!=rng.second; z++)
			{
				const char* str = (*z).second.c_str();

				if(str && *str != '0')
				{	
					char numBuf[4096]={0};
					const char* d = str; INT32 dindex = 0;
					while(d && *d != 0 && *d != '|')
					{
						numBuf[dindex++] = *d;
						d++;
					}

 					DWORD parsedFlag = (DWORD)atoi(numBuf);

					if(parsedFlag & flag)
					{
						const char* c = str;

						while(c && *c != 0 && *c != '|')
							c++;
						if(c && *c) c++;

						INT32 bufsiz = strlen(str)+100;
						char* buf = new char[bufsiz];

//						if(c)
//						{
							strcpy_s(buf, bufsiz, c);
							char* f = buf;

							while(f && *f != 0 && *f != '|')
								f++;

							while(f && *f != 0 && *f != '|')
								f++;

							if(f) *f = 0;
//						}
//						else
//						{
//							sprintf_s(buf, bufsiz, "[%s]", str);
//						}

						rst->push_back(buf);
						numAdd++;

						delete [] buf;
					}
				}			
			}
		}
		x++;
	}

	return numAdd;
}

static i3GeometryAttr * _ConvertCollideeMesh( i3CollideeMesh * pMesh, bool bColor)
{
	
	i3GeometryAttr * pGeoAttr;
	i3Vector3Array * pPosArray;
	i3Vector3Array * pNormalArray;
	INT32 i;

	pPosArray = pMesh->GetPositionArray();
	pNormalArray = pMesh->GetNormalArray();

	// GeometryAttr
	{
		pGeoAttr = i3GeometryAttr::new_object_ref();

	}

	// Vertex Array
	{
		i3VertexFormat format;
		i3VertexArray * pVA;

		format.SetHasPosition( true);
		format.SetHasNormal( true);

		if( bColor)
			format.SetHasColor( true);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, pMesh->GetTriangleCount() * 3, 0);

		pVA->Lock();

		/*
		#if defined( I3_FIXED_POINT)
			for( UINT32 i = 0; i < pPosArray->GetVectorCount(); i++)
			{
				VEC3D temp;

				i3Vector::Copy( &temp, pPosArray->GetVectorArray());

				pVA->SetPosition( i, &temp);
			}
		#else
			pVA->SetPositions( 0, pPosArray->GetVectorCount(), pPosArray->GetVectorArray());
		#endif
		*/

		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);

		for( i = 0 ; i < (INT32) pMesh->GetTriangleCount(); i++)
		{
			I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( i);

			pVA->SetPosition( 0 + (i * 3), pPosArray->GetVector( pTri->m_Index[0]));
			pVA->SetPosition( 1 + (i * 3), pPosArray->GetVector( pTri->m_Index[1]));
			pVA->SetPosition( 2 + (i * 3), pPosArray->GetVector( pTri->m_Index[2]));

			pVA->SetNormal( 0 + (i * 3), pNormalArray->GetVector( pTri->m_NormalIndex));
			pVA->SetNormal( 1 + (i * 3), pNormalArray->GetVector( pTri->m_NormalIndex));
			pVA->SetNormal( 2 + (i * 3), pNormalArray->GetVector( pTri->m_NormalIndex));

			if( bColor)
			{
				pVA->SetColor( 0 + (i * 3), &col);
				pVA->SetColor( 1 + (i * 3), &col);
				pVA->SetColor( 2 + (i * 3), &col);
			}
		}

		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pMesh->GetTriangleCount(), 0);
	}

	return pGeoAttr;
}

static i3GeometryAttr * _ConvertCollideeLine( i3CollideeMesh * pMesh)
{
	
	i3GeometryAttr * pGeoAttr;
	i3Vector3Array * pPosArray;
	INT32 i;

	pPosArray = pMesh->GetPositionArray();

	// GeometryAttr
	{
		pGeoAttr = i3GeometryAttr::new_object_ref();

	}

	// Vertex Array
	{
		i3VertexFormat format;
		i3VertexArray * pVA;

		format.SetHasPosition( true);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, pPosArray->GetVectorCount(), 0);

		pVA->Lock();

		#if defined( I3_FIXED_POINT)
			for( UINT32 i = 0; i < pPosArray->GetVectorCount(); i++)
			{
				VEC3D temp;

				i3Vector::Copy( &temp, pPosArray->GetVector( i));
				pVA->SetPosition( i, &temp);
			}
		#else
			pVA->SetPositions( 0, pPosArray->GetVectorCount(), pPosArray->GetVectorArray());
		#endif

		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, pMesh->GetTriangleCount() * 3, 0);
	}

	// Index Array
	{
		i3IndexArray * pIA;

		pIA = i3IndexArray::new_object_ref();

		pIA->Create( pMesh->GetTriangleCount() * 6, 0, pGeoAttr->GetVertexArray()->GetCount());

		pIA->Lock();

		for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
		{
			I3_COLLIDEE_TRI32 * pTri;

			pTri = pMesh->GetTriangle( i);

			pIA->SetIndex( (i * 6) + 0, pTri->m_Index[0]);
			pIA->SetIndex( (i * 6) + 1, pTri->m_Index[1]);

			pIA->SetIndex( (i * 6) + 2, pTri->m_Index[1]);
			pIA->SetIndex( (i * 6) + 3, pTri->m_Index[2]);

			pIA->SetIndex( (i * 6) + 4, pTri->m_Index[2]);
			pIA->SetIndex( (i * 6) + 5, pTri->m_Index[0]);
		}

		pIA->Unlock();

		pGeoAttr->SetIndexArray( pIA);
	}

	return pGeoAttr;
}

static i3GeometryAttr * _ConvertCollideeNormal( i3CollideeMesh * pMesh)
{
	
	i3GeometryAttr * pGeoAttr;
	i3Vector3Array * pPosArray;
	i3Vector3Array * pNormalArray;
	INT32 i;

	pPosArray = pMesh->GetPositionArray();
	pNormalArray = pMesh->GetNormalArray();

	// GeometryAttr
	{
		pGeoAttr = i3GeometryAttr::new_object_ref();
	}

	// Vertex Array
	{
		i3VertexFormat format;
		i3VertexArray * pVA;

		format.SetHasPosition( true);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, pMesh->GetTriangleCount() * 2, 0);

		pVA->Lock();

		{
			VEC3D * pPos1, * pPos2, * pPos3;
			VEC3D * pNorm;
			VEC3D mid;

			for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
			{
				I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( i);

				pPos1 = pPosArray->GetVector( pTri->m_Index[0]);
				pPos2 = pPosArray->GetVector( pTri->m_Index[1]);
				pPos3 = pPosArray->GetVector( pTri->m_Index[2]);
				pNorm = pNormalArray->GetVector( pTri->m_NormalIndex);

				i3Vector::Add( &mid, pPos1, pPos2);
				i3Vector::Add( &mid, &mid, pPos3);
				i3Vector::Scale( &mid, &mid, 1.0f / 3.0f);

				#if defined( I3_FIXED_POINT)
					VEC3D pos;

					i3Vector::Copy( &pos, &mid);
					pVA->SetPosition( (i * 2), &pos);
				#else
					pVA->SetPosition( (i * 2), &mid);
				#endif

				i3Vector::ScaleAdd( &mid, pNorm, 1.0f);

				#if defined( I3_FIXED_POINT)
					i3Vector::Copy( &pos, &mid);
					pVA->SetPosition( (i * 2) + 1, &pos);
				#else
					pVA->SetPosition( (i * 2) + 1, &mid);
				#endif
				
			}
		}

		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, pMesh->GetTriangleCount(), 0);
	}

	return pGeoAttr;
}

static i3Node * _ConvertColliderMesh( i3CollideeMesh * pCollidee, bool bSurface, bool bLine, bool bNormal, bool bColor)
{
	i3Geometry * pGeo;

	pGeo = i3Geometry::new_object_ref();

	{
		i3GeometryAttr * pGeoAttr;

		if( bSurface)
		{
			pGeoAttr = _ConvertCollideeMesh( (i3CollideeMesh *) pCollidee, bColor);

			pGeo->AppendGeometryAttr( pGeoAttr);
		}

		if( bLine)
		{
			pGeoAttr = _ConvertCollideeLine( (i3CollideeMesh *) pCollidee);

			pGeo->AppendGeometryAttr( pGeoAttr);
		}

		if( bNormal)
		{
			pGeoAttr = _ConvertCollideeNormal( (i3CollideeMesh *) pCollidee);

			pGeo->AppendGeometryAttr( pGeoAttr);
		}
	}

	return pGeo;
}

static i3Node * _ConvertCollideeLine( i3CollideeLine * pLine)
{
	i3VertexArray	* pArray;
	i3Node * pNode = i3SceneUtil::BuildLine( pArray, 1);

	pArray->Lock();
	pArray->SetPosition( 0, pLine->GetStart());
	pArray->SetPosition( 1, pLine->GetEnd());
	pArray->Unlock();

	return pNode;
}

static i3Node * _ConvertSphereCollider( i3CollideeSphere * pSphere, bool bSurface, bool bLine)
{
	i3Node * pNode;
	i3Transform * pTrans;
	REAL32 sz;

	pNode = i3SceneUtil::BuildSphereMesh( 1.0f, bSurface, bLine);

	pTrans = i3Transform::new_object_ref();

	sz = pSphere->GetRadius() * 2.0f;
	i3Matrix::SetScale( pTrans->GetMatrix(), sz, sz, sz);

	#if defined( I3_FIXED_POINT)
		FIXED32_VEC3D ct;

		i3Vector::Copy( &ct, pSphere->GetCenter());
		i3Matrix::PostTranslate( pTrans->GetMatrix(), &ct);
	#else
		i3Matrix::PostTranslate( pTrans->GetMatrix(), pSphere->GetCenter());
	#endif

	pTrans->AddChild( pNode);

	I3_MUST_RELEASE( pNode);

	return pTrans;
}

static i3Node * _ConvertCapsuleCollider( i3CollideeCapsule * pCapsule, bool bSurface, bool bLine)
{
	VEC3D vAxis;

	i3Node * pNode = i3SceneUtil::BuildCapsuleMesh( pCapsule->getRadius(), pCapsule->getLength(), bSurface, bLine);

	i3Transform * pTrans = i3Transform::new_object_ref();
	REAL32 sz = pCapsule->getRadius() * 2.0f;
	i3Matrix::SetScale( pTrans->GetMatrix(), sz, sz, sz);
	
	//	기준을 캡슐의 Start로 이동
	i3Matrix::SetPos( pTrans->GetMatrix(), 0.f, (pCapsule->getLength() * 0.5f), 0.f, 1.f);		//	Start를 원점으로 

	REAL32 fTheta = i3Vector::Dot( &I3_YAXIS, pCapsule->getDirection());
	REAL32 fRot = i3Math::acos( fTheta);		//	회전값

	i3Vector::Cross( &vAxis, &I3_YAXIS, pCapsule->getDirection());		//	회전축
	i3Matrix::PostRotateAxis( pTrans->GetMatrix(), &vAxis, fRot);

#if defined( I3_FIXED_POINT)
	VEC3D vec;  

	i3Vector::Copy( &vec, pCapsule->getStart());
	i3Matrix::PostTranslate( pTrans->GetMatrix(), &vec);

#else
	i3Matrix::PostTranslate( pTrans->GetMatrix(), pCapsule->getStart());

#endif

	pTrans->AddChild( pNode);  
	I3_MUST_RELEASE( pNode);

    return pTrans;	
}

I3_EXPORT_SGUT 
i3Node *	i3SceneUtil::BuildCollideeMesh( i3Collidee * pColl, bool bSurface, bool bLine, bool bNormal, bool bColor)
{
	i3Node * pNewNode = nullptr;

	if( i3::same_of<i3CollideeMesh* >(pColl))
	{
		pNewNode = _ConvertColliderMesh( (i3CollideeMesh *) pColl, bSurface, bLine, bNormal, bColor);
	}
	else if( i3::same_of<i3CollideeSphere* >(pColl))
	{
		pNewNode = _ConvertSphereCollider( (i3CollideeSphere *) pColl, bSurface, bLine);
	}
	else if( i3::same_of<i3CollideeLine* >(pColl))
	{
		pNewNode = _ConvertCollideeLine( (i3CollideeLine *) pColl);
	}
	else if( i3::same_of<i3CollideeCapsule* >(pColl))
	{
		pNewNode = _ConvertCapsuleCollider( (i3CollideeCapsule *) pColl, bSurface, bLine);
	}

	return pNewNode;
}

static void _AddRenderableShapeSet( i3PhysixShapeSet * pShapeSet, i3Node * pParent, bool bAutoAttr, i3::vector<I3_VISIBLE_COLL_MESH_INFO*>* pInfoList, COLOR * pCol)
{
	INT32 i, j, k;
	NxU32 numTrigs = 0;
	const NxTriangle32* trigs = nullptr;
	const NxPoint* verts = nullptr;
	const NxPoint* normals = nullptr;
	i3AttrSet * pAttrSet = nullptr;
	COLORREAL col;

	if( pCol != nullptr)
		i3Color::Set( &col, pCol);
	else
	{
		if( bAutoAttr)
			i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
		else
			i3Color::Set( &col, (UINT8) 105, 151, 30, 96);
	}

	for( i = 0; i < pShapeSet->getNxShapeCount(); i++)
	{
		NxShape * pShape = pShapeSet->getNxShape( i);

		i3Node * pNewNode = i3Node::new_object_ref();
		pNewNode->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);

		if( bAutoAttr)
		{
			pAttrSet = i3AttrSet::new_object_ref();
			pAttrSet->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);
			pNewNode->AddChild( pAttrSet);
			
			{
				i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
				pAttr->Set(true);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
				pAttr->SetSource( I3G_BLEND_SRCALPHA);
				pAttr->SetDestination( I3G_BLEND_ONE);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
				pAttr->Set(false);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
				pAttr->Set(false);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();

				pAttr->Set(false);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

				if(!pCol)
					i3Color::Set(&col, (UINT8) (i3Math::Rand()%250 + 5), i3Math::Rand()%250 + 5, i3Math::Rand()%250 + 5, 128);

				pAttr->Set(&col);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();

				pAttr->setBias( -0.0002f);
				pAttrSet->AddAttr( pAttr);
			}
		}

		MATRIX localTm;
		NxMat34 localPos = pShape->getLocalPose();

		NXU::Copy( (MATRIX*) &localTm, (NxMat34*) & localPos);


		i3VertexFormat format;

		format.SetHasPosition( true );
		format.SetHasNormal( true );
		format.SetHasColor( true );

		switch( pShape->getType() )
		{
		case NX_SHAPE_CONVEX :
			{
				NxConvexMesh & pConvex = ((NxConvexShape *)pShape)->getConvexMesh();
				numTrigs = pConvex.getCount(0, NX_ARRAY_TRIANGLES);
				trigs = (const NxTriangle32*) pConvex.getBase(0, NX_ARRAY_TRIANGLES);
				verts = (const NxPoint*) pConvex.getBase(0, NX_ARRAY_VERTICES);
				normals = (const NxPoint*) pConvex.getBase(0, NX_ARRAY_NORMALS);
			}
			break;
		case NX_SHAPE_MESH :
			{
				NxTriangleMesh & pMesh = ((NxTriangleMeshShape *)pShape)->getTriangleMesh();
				numTrigs = pMesh.getCount(0, NX_ARRAY_TRIANGLES);
				trigs = (const NxTriangle32*) pMesh.getBase(0, NX_ARRAY_TRIANGLES);
				verts = (const NxPoint*) pMesh.getBase(0, NX_ARRAY_VERTICES);
				normals = (const NxPoint*) pMesh.getBase(0, NX_ARRAY_NORMALS);
			}
			break;
		}

		i3VertexArray * pVA = nullptr;

		switch( pShape->getType() )
		{
			default :
				I3TRACE( "INVALID SHAPESET Type : %d\n", pShape->getType());
				I3ASSERT_0;
				break;

			case NX_SHAPE_CONVEX :
			case NX_SHAPE_MESH :
				{
					pVA = i3VertexArray::new_object_ref();

					pVA->Create( &format, numTrigs * 3, 0);
					pVA->setRestoreType( I3G_RESTORE_MEM);

					pVA->Lock();
					
					VEC3D Pos, Normal;

					for( j = 0; j < (INT32) numTrigs; j++ )
					{
						for( k = 0; k < 3; k++ )
						{
							setX( &Pos, verts[ trigs[j].v[k] ].x );
							setY( &Pos, verts[ trigs[j].v[k] ].y );
							setZ( &Pos, verts[ trigs[j].v[k] ].z );

							i3Vector::TransformCoord( &Pos, &Pos, &localTm);

							setX( &Normal, normals[trigs[j].v[k]].x );
							setY( &Normal, normals[trigs[j].v[k]].y );
							setZ( &Normal, normals[trigs[j].v[k]].z );

							i3Vector::TransformNormal( &Normal, &Normal, &localTm);

							pVA->SetPosition( (j*3) + k, &Pos );
							pVA->SetNormal( (j*3) + k, &Normal );
							pVA->SetColor( (j*3) + k, &col);
						}
					}

					pVA->Unlock();
				}
				break;

			case NX_SHAPE_BOX :
				{
					NxBoxShape * pBoxShape = (NxBoxShape *) pShape;
					VEC3D vmin, vmax;
					NxVec3 sz = pBoxShape->getDimensions();

					i3Vector::Set( &vmin, -sz.x, -sz.y, -sz.z);
					i3Vector::Set( &vmax, sz.x, sz.y, sz.z);

					pVA = i3VertexArray::CreateBox( &vmin, &vmax, true, false, &col);

					VEC3D vecBox[ 36];
					pVA->Lock();
					pVA->GetPositions( 0, 36, vecBox);
					for( INT32 j = 0; j < 36; j++)
					{
						i3Vector::TransformCoord( &vecBox[ j], &vecBox[ j], &localTm);
					}
					pVA->SetPositions( 0, 36, vecBox);
					pVA->Unlock();

					numTrigs = 12;
				}
				break;

			case NX_SHAPE_SPHERE :
				{
					NxSphereShape * pSphereShape = (NxSphereShape*) pShape;
					i3CollideeSphere Sphere;

					NxSphere worldSphere;
					pSphereShape->getWorldSphere(worldSphere);
					
					Sphere.SetCenter( worldSphere.center.x, worldSphere.center.y, worldSphere.center.z );
					Sphere.SetRadius( worldSphere.radius );

					i3Node * pNode = _ConvertSphereCollider( &Sphere, false, true );
					pNode->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);
					if( pParent != nullptr)
					{
						I3_MUST_RELEASE( pNewNode);
						pParent->AddChild( pNode);
					}
					return;
				}
				break;

			case NX_SHAPE_PLANE :
				{
					NxPlaneShape * pPlaneShape = (NxPlaneShape*) pShape;
					NxPlane Plane = pPlaneShape->getPlane();

					VEC3D vPos;
					i3Vector::Set( &vPos, Plane.normal.x, Plane.normal.y, Plane.normal.z );
					i3Vector::Scale( &vPos, &vPos, Plane.d );

					VEC3D vmin, vmax;
					i3Vector::Set( &vmin, getX(&vPos)-(1.f-Plane.normal.x), getY(&vPos)-(1.f-Plane.normal.y), getZ(&vPos)-(1.f-Plane.normal.z) );
					i3Vector::Set( &vmax, getX(&vPos)+(1.f-Plane.normal.x), getY(&vPos)+(1.f-Plane.normal.y), getZ(&vPos)+(1.f-Plane.normal.z) );

					pVA = i3VertexArray::CreateBox( &vmin, &vmax, false, false);
					
					numTrigs = 12;
				}
				break;
		}
	

		// Geometry
		{
			i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, numTrigs );
			//pGeoAttr->SetIndexArray( pIA);

			i3Geometry * pGeo = i3Geometry::new_object_ref();
			pGeo->AppendGeometryAttr( pGeoAttr);

			if( bAutoAttr)
			{
				pAttrSet->AddChild( pGeo );
				//위에서 넣어주고 있어요
				//pNewNode->AddChild( pAttrSet );
			}
			else
			{
				pNewNode->AddChild( pGeo);
			}

			if( pInfoList != nullptr)
			{
				I3_VISIBLE_COLL_MESH_INFO * pInfo = (I3_VISIBLE_COLL_MESH_INFO *) i3MemAlloc( sizeof(I3_VISIBLE_COLL_MESH_INFO));

				pInfo->m_pShapeSet = pShapeSet;
				pInfo->m_pGeo = pGeo;
				pInfo->m_pGeoAttr = pGeoAttr;

				pInfoList->push_back( pInfo);
			}
		}

		if( pParent != nullptr)
		{
			pParent->AddChild( pNewNode);
		}
	}
}

static i3Node* GenerateVisColliderHolder(i3Node* src, const char* desc=nullptr)
{
	i3Node* rst = nullptr;

	if(i3::kind_of<i3Transform*>(src))
	{
		rst = i3TransformRef::new_object_ref();
		i3Transform* tm = (i3Transform*)src;

		((i3TransformRef*)rst)->SetTransform( tm);
	}
	else if( i3::kind_of<i3Body*>(src))
	{
		i3Body * pBody = (i3Body *) src;

		if( pBody->getFrameLOD() != nullptr)
		{
			i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

			if( pSkel != nullptr)
			{
				i3Bone * pBone = pSkel->getBone(0);

				if( pBone != nullptr)
				{
					i3Transform * pTrans = i3Transform::new_object_ref();

					pTrans->SetMatrix( pBone->getMatrix());

					rst = pTrans;
				}
			}
		}
		
	}
	
	if( rst == nullptr)
	{
		rst = i3Node::new_object_ref();
	}

	if(src)
	{
		if(desc)
		{
			char buf[MAX_PATH]={0};
			sprintf_s(buf, MAX_PATH, "%s@%s", src->hasName() ? src->GetName() : "no name", desc);

			rst->SetName(buf);
		}
		else
		{
			if(src->hasName())
				rst->SetName(src->GetName());
		}
	}

	return rst;
}

static std::string GenerateMsgStr(const char* fmt, ...)
{
	char buf[1024] = {0};
	va_list arg;
	va_start(arg, fmt);

	vsprintf_s(buf, fmt, arg);
	va_end(arg);

	if(buf[0])
		return std::string(buf);

	return std::string();
}

static i3Node * Rec_BuildVisibleCollider(i3Node * pNode, i3Node * pParent, bool bLine, bool bNormal, 
	bool bAutoAttr, i3::vector<I3_VISIBLE_COLL_MESH_INFO*>* pInfoList, COLOR * pCol, 
	i3SceneUtil::BuildLogger* logger, INT32 testOnly, RenColliderColorTable* colorTable)
{
	i3Node * curVisibleGen = nullptr;	// 이번호출에 생성된 비저블노드
	INT32 i;
	UINT32 j;

	bool physShapeGenerated = false;
	const char* physShapeGenerateClassName = nullptr;
	std::string phyShapeGenMsg;


	if(!pParent)
	{		
		I3ASSERT(pParent);
		SceneUtilBuildLog(0, "NOPARENT", pNode->GetName(), "INFO! - Called with no parent node..");
		return nullptr;
	}

	if(i3::same_of<i3ColliderSet*>(pNode))
	{
		i3ColliderSet * pColSet = (i3ColliderSet *)pNode;
		INT32 numCollider = pColSet->GetColliderCount();

		physShapeGenerateClassName = "i3ColliderSet";

		if(numCollider > 0)
		{	
			curVisibleGen = GenerateVisColliderHolder(pNode);

			for(i=0; i<pColSet->GetColliderCount(); i++) 
			{
				i3Collider * pCollider = pColSet->GetCollider(i);
				i3Node* into = i3SceneUtil::BuildCollideeMesh(pCollider->GetCollidee(), false, bLine, bNormal);

				if(into)
				{
					curVisibleGen->AddChild(into);
					physShapeGenerated = true;					
				}
			}
		}
	}
	else if( i3::same_of<i3Transform*>(pNode))
	{
		curVisibleGen = GenerateVisColliderHolder(pNode);
	}
	else if( i3::same_of<i3Transform2*>(pNode))  // 새로운 컬리젼.. 추가 by 순구
	{			
		i3Transform2 * pSrcTrans = (i3Transform2 *)pNode;
		i3PhysixShapeSet * pShapeSet = pSrcTrans->getShapeSet();

		physShapeGenerateClassName = "i3Transform2";

		if(pShapeSet)
		{
			curVisibleGen = GenerateVisColliderHolder(pNode);

			// testOnly==1일경우는 i3OptiExp2맥스익스포터에서 렌더링셰이프생성하지않고
			// 단지 PhysX셰이프생성되었는지 검사용도로 쓰입니다.
			if(!testOnly) 
			{
				I3COLOR* clr = pCol;
				if(colorTable)
					clr = (I3COLOR*)&colorTable->GetColorByNodePrefix(pSrcTrans->GetName());
	
				_AddRenderableShapeSet(pShapeSet, curVisibleGen, bAutoAttr, pInfoList, clr);
			}
			physShapeGenerated = true;
		}
	}
	else if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		i3ShapeSetContainer * pSrc = (i3ShapeSetContainer *)pNode;
		i3PhysixShapeSet * pShapeSet = pSrc->getShapeSet();

		physShapeGenerateClassName = "i3ShapeSetContainer";

		if(pShapeSet)
		{	
			curVisibleGen = GenerateVisColliderHolder(pNode);

			if(!testOnly)
			{
				I3COLOR* clr = pCol;
				if(colorTable)
					clr = (I3COLOR*)&colorTable->GetColorByNodePrefix(pSrc->GetName());
	
				_AddRenderableShapeSet( pShapeSet, curVisibleGen, bAutoAttr, pInfoList, clr);
			}
			physShapeGenerated = true;
		}
	}
	else if( i3::kind_of<i3Body*>(pNode))
	{	
		NxU32 numTrigs;
		const NxTriangle32* trigs;
		const NxPoint* verts;
		const NxPoint* normals;

		i3Body * pBody = (i3Body *) pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

		physShapeGenerateClassName = "i3Body";

		if(pSkel->getBoneCount() > 0)
			curVisibleGen = GenerateVisColliderHolder(pNode);
		
		for(i=0; i<pSkel->getBoneCount(); i++)
		{
			if(!pSkel->isShapeSet(i))
			{
				phyShapeGenMsg += GenerateMsgStr("1|%s|i3SkeletonBone has not shape set bone=%d,bonename=%s",
					pNode->GetName(), i, pSkel->getName(i));
				continue;
			}

			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i);

			if(!pShapeSet)
			{
				phyShapeGenMsg += GenerateMsgStr("1|%s|i3SkeletonBone has not i3PhysixShape bone=%d,bonename=%s",
					pNode->GetName(), i, pSkel->getName(i));
				continue;
			}

			NxActor * pActor = pShapeSet->getNxActor();

			NxShape * pShape = pActor->getShapes()[0];
			if(!pShape)
			{
				phyShapeGenMsg += GenerateMsgStr("1|%s|i3SkeletonBone has not NxShape bone=%d,bonename=%s",
					pNode->GetName(), i, pSkel->getName(i));
				continue;
			}

			MATRIX localTm;
			NxMat34 localPos = pShape->getLocalPose();

			NXU::Copy( (MATRIX*) &localTm, (NxMat34*) & localPos);

			physShapeGenerated = true;

			if(!testOnly)
			{
				switch(pShape->getType())
				{
				case NX_SHAPE_CONVEX :
					{
						NxConvexMesh& pConvex = ((NxConvexShape *)pShape)->getConvexMesh();

						numTrigs = pConvex.getCount(0, NX_ARRAY_TRIANGLES);
						trigs = (const NxTriangle32*) pConvex.getBase(0, NX_ARRAY_TRIANGLES);
						verts = (const NxPoint*) pConvex.getBase(0, NX_ARRAY_VERTICES);
						normals = (const NxPoint*) pConvex.getBase(0, NX_ARRAY_NORMALS);
	
						i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
						{
							i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
							COLORREAL color;	
		
							if(colorTable)
							{
								I3COLOR rstClr = colorTable->GetColorByNodePrefix(pShape->getName());
								i3Color::Set(&color, &rstClr);	
							}
							else
								i3Color::Set( &color, 0.5f, 0.8f, 0.4f, 0.5f );
	
							pAttr->Set( &color );
							pAttrSet->AddAttr( pAttr);
						}
		
						{
							i3TextureEnableAttr *pAttr = i3TextureEnableAttr::new_object_ref();
							pAttr->Set(false);
							pAttrSet->AddAttr( pAttr );
						}
	
						i3VertexFormat format;
		
						format.SetHasPosition( true );
						format.SetHasNormal( true );
						format.SetHasColor( false );	
	
						i3VertexArray * pVA = i3VertexArray::new_object_ref();
						pVA->Create( &format, numTrigs * 3, 0);
						pVA->setRestoreType( I3G_RESTORE_MEM);
	
						pVA->Lock();
						
						VEC3D Pos, Normal;
						for(j=0; j < numTrigs; j++)
						{
							for( INT32 k = 0; k < 3; k++ )
							{
								setX( &Pos, verts[ trigs[j].v[k] ].x );
								setY( &Pos, verts[ trigs[j].v[k] ].y );
								setZ( &Pos, verts[ trigs[j].v[k] ].z );
	
								setX( &Normal, normals[trigs[j].v[k]].x );
								setY( &Normal, normals[trigs[j].v[k]].y );
								setZ( &Normal, normals[trigs[j].v[k]].z );

								i3Vector::TransformCoord( &Pos, &Pos, &localTm);
								i3Vector::TransformNormal( &Normal, &Normal, &localTm);
	
								pVA->SetPosition( (j * 3) + k, &Pos );
								pVA->SetNormal( (j * 3) + k, &Normal );
							}
						}
						
						pVA->Unlock();
	
	
						{
							i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();
							pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, numTrigs );
							//pGeoAttr->SetIndexArray( pIA);
	
							i3Geometry* pGeo = i3Geometry::new_object_ref();
							pGeo->AppendGeometryAttr(pGeoAttr);
							pAttrSet->AddChild(pGeo);
						}
	
						curVisibleGen->AddChild(pAttrSet);
					}
					break;
	
				case NX_SHAPE_CAPSULE :
					{
						NxCapsuleShape * pCapsule = pShape->isCapsule();
						REAL32	rRadius = pCapsule->getRadius();
						REAL32	rHeight = pCapsule->getHeight();
						COLORREAL color; 
						
						if(colorTable)
						{
							I3COLOR rstClr = colorTable->GetColorByNodePrefix(pShape->getName());
							i3Color::Set(&color, &rstClr);
						}
						else
							i3Color::Set(&color, 0.8f, 0.3f, 0.7f, 0.5f );
	
						i3Node* into = i3SceneUtil::BuildCapsuleMesh(rRadius, rHeight, true, false, 20, 10, nullptr,
							&color, &color);
	
						if(into)
						{
							i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
							into->AddChild(pAttrSet);
							curVisibleGen->AddChild(into);							
						}
					}
					break;
	
				default:
					break;
				}
			}
		}
	}
	else if( i3::kind_of<i3BoneRef*>(pNode))
	{		
		curVisibleGen = GenerateVisColliderHolder(pNode);
	}
	else if(i3::kind_of<i3OcTreeNode*>(pNode))
	{
		curVisibleGen = GenerateVisColliderHolder(pNode);

		for(i=0; i<8; i++)
		{
			i3OcTreeNode* pChild = ((i3OcTreeNode*)pNode)->getChildOctree((i3OcTreeNode::DIVIDE_NODE)i);
			if(pChild)
			{
				Rec_BuildVisibleCollider(pChild, curVisibleGen, bLine, bNormal, bAutoAttr,
					pInfoList, pCol, logger, testOnly, colorTable);
			}
		}
	}

	//----------------------------------------------------------------------------------------------//
	if(!curVisibleGen)
		curVisibleGen = GenerateVisColliderHolder(pNode, "i3SceneUtilGenDummy");

	pParent->AddChild(curVisibleGen);

	if(logger)
	{
		if(physShapeGenerateClassName)
		{
			SceneUtilBuildLog(physShapeGenerated ? BuildLogger::ELF_INC_PHYSHAPE_GEN : BuildLogger::ELF_INC_PHYSHAPE_MISS,
				physShapeGenerateClassName, pNode->GetName(), phyShapeGenMsg.c_str());
		}
	}

	// Recursive call
	UINT32 childCount = pNode->GetChildCount();

	for(i=0; i<(INT32)childCount; i++)
	{
		i3Node * pChild = pNode->GetChild( i);		
		Rec_BuildVisibleCollider(pChild, curVisibleGen, bLine, bNormal, 
			bAutoAttr, pInfoList, pCol, logger, testOnly, colorTable);
	}

	return curVisibleGen;
}

i3Node * i3SceneUtil::BuildRendableCollider(i3Node * pSrc, i3Node* pParent, bool bLine, bool bNormal, 
	bool bAutoAttr, i3::vector<I3_VISIBLE_COLL_MESH_INFO*>* pInfoList, COLOR * pCol, bool bRemoveVolatile,
	BuildLogger* logger, INT32 testOnly, RenColliderColorTable* colorTable)
{
	if( bRemoveVolatile)
		RemoveNodeByFlag( pSrc, I3_NODEFLAG_VOLATILE);

	i3Node* rstNode = nullptr;
	bool retParent = false;

	// pParent가 있을경우엔 생성되는 컬리전노드들을 자식노드로 넣는다.
	// 없을경우엔 새로 노드를생성하고 그노드를 리턴한다.
	if(!pParent)
	{
		pParent = i3Node::new_object(); // ref 1
		retParent = true;
	}

	rstNode = Rec_BuildVisibleCollider(pSrc, pParent, bLine, bNormal, bAutoAttr, pInfoList, pCol, 
		logger, testOnly, colorTable);

	if(retParent)
		rstNode = pParent;

	return rstNode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
static i3Node * Rec_BuildVisibleBound( i3Node * pNode)
{
	INT32 i;
	
	if( pNode->GetBound() != nullptr)
	{
		i3BoundBox * pBox = pNode->GetBound();

		VEC3D vmin, vmax;
		i3Vector::Copy( &vmin, pBox->GetMin());
		i3Vector::Copy( &vmax, pBox->GetMax());

		i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();
		i3VertexArray * pVA = i3VertexArray::new_object_ref();
		i3VertexFormat fmt;

		fmt.SetHasPosition();
		pVA->Create( &fmt, 24, 0);

		pVA->Lock();

		{
			VEC3D pos[24];

			// 사각 윗면
			i3Vector::Set( &pos[ 0],		vmin.x, vmax.y, vmin.z);
			i3Vector::Set( &pos[ 1],		vmin.x, vmax.y, vmax.z);

			i3Vector::Set( &pos[ 2],		vmin.x, vmax.y, vmax.z);
			i3Vector::Set( &pos[ 3],		vmax.x, vmax.y, vmax.z);

			i3Vector::Set( &pos[ 4],		vmax.x, vmax.y, vmax.z);
			i3Vector::Set( &pos[ 5],		vmax.x, vmax.y, vmin.z);

			i3Vector::Set( &pos[ 6],		vmax.x, vmax.y, vmin.z);
			i3Vector::Set( &pos[ 7],		vmin.x, vmax.y, vmin.z);

			// 사각 아랫면
			i3Vector::Set( &pos[ 8],		vmin.x, vmin.y, vmin.z);
			i3Vector::Set( &pos[ 9],		vmin.x, vmin.y, vmax.z);

			i3Vector::Set( &pos[10],		vmin.x, vmin.y, vmax.z);
			i3Vector::Set( &pos[11],		vmax.x, vmin.y, vmax.z);

			i3Vector::Set( &pos[12],		vmax.x, vmin.y, vmax.z);
			i3Vector::Set( &pos[13],		vmax.x, vmin.y, vmin.z);

			i3Vector::Set( &pos[14],		vmax.x, vmin.y, vmin.z);
			i3Vector::Set( &pos[15],		vmin.x, vmin.y, vmin.z);

			// 세로 이음선
			i3Vector::Set( &pos[16],		vmin.x, vmin.y, vmin.z);
			i3Vector::Set( &pos[17],		vmin.x, vmax.y, vmin.z);

			i3Vector::Set( &pos[18],		vmin.x, vmin.y, vmax.z);
			i3Vector::Set( &pos[19],		vmin.x, vmax.y, vmax.z);

			i3Vector::Set( &pos[20],		vmax.x, vmin.y, vmin.z);
			i3Vector::Set( &pos[21],		vmax.x, vmax.y, vmin.z);

			i3Vector::Set( &pos[22],		vmax.x, vmin.y, vmax.z);
			i3Vector::Set( &pos[23],		vmax.x, vmax.y, vmax.z);

			pVA->SetPositions( 0, 24, pos);
		}

		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 12);

		i3Geometry * pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();

		pAttrSet->AddFlag( I3_NODEFLAG_VOLATILE);
		pAttrSet->AddChild( pGeo);

		if( pNode->GetParent() != nullptr)
			pNode->GetParent()->AddChild( pAttrSet);
		else
			pNode->AddChild( pAttrSet);

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSet->AddAttr( pAttr);
		}
		
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			COLORREAL col;

			i3Color::Set( &col,		(i3Math::Randf() * 0.5f) + 0.4f,
									(i3Math::Randf() * 0.5f) + 0.4f,
									(i3Math::Randf() * 0.5f) + 0.4f,
									0.5f);

			pAttr->Set( &col);
			pAttrSet->AddAttr( pAttr);
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		Rec_BuildVisibleBound( pChild);
	}

	return pNode;
}

i3Node * i3SceneUtil::BuildVisibleBound( i3Node * pSrc)
{
	//RemoveNodeByFlag( pSrc, I3_NODEFLAG_VOLATILE);

	return Rec_BuildVisibleBound( pSrc);
}

i3Node *	_BuildGeoInstance( i3Geometry * pGeo, bool bLine, UINT8 alpha)
{
	i3Node * pRoot;
	INT32 i;

	pRoot = i3Node::new_object_ref();
	pRoot->SetName( pGeo->GetNameString());

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		i3Geometry * pNewGeo;
		i3GeometryAttr * pGeoAttr;
		i3RenderAttr * pAttr;

		pAttr = pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr*>(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->SetName( pGeo->GetNameString());
		pRoot->AddChild( pAttrSet);

		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			COLOR col;

			col.r = (UINT8) ((i3Math::Rand() % 128) + 92);
			col.g = (UINT8) ((i3Math::Rand() % 128) + 92);
			col.b = (UINT8) ((i3Math::Rand() % 128) + 92);
			col.a = alpha;

			pAttr->Set( &col);

			pAttrSet->AddAttr( pAttr);
		}

		pNewGeo = i3Geometry::new_object_ref();
		pAttrSet->AddChild( pNewGeo);

		pNewGeo->AppendGeometryAttr( pGeoAttr);
	}

	return pRoot;
}

I3_EXPORT_SGUT 
i3Node *	i3SceneUtil::BuildInstanceMesh( i3Node * pSrc, bool bLine, UINT8 alpha)
{
	i3Node * pNode;

	if( i3::same_of<i3Geometry* >(pSrc))
	{
		pNode = _BuildGeoInstance( (i3Geometry *) pSrc, bLine, alpha);
	}
	else if( i3::kind_of<i3Transform*>(pSrc))
	{
		i3Transform * pTrans = i3Transform::new_object_ref();

		pTrans->SetMatrix( ((i3Transform *) pSrc)->GetMatrix());

		pNode = pTrans;
	}
	else
	{
		pNode = i3Node::new_object_ref();
	}

	if( pSrc->hasName())
		pNode->SetName( pSrc->GetNameString());

	INT32 i;
	i3Node * pChild, * pNewChild;

	for( i = 0; i < pSrc->GetChildCount(); i++)
	{
		pChild = pSrc->GetChild( i);

		pNewChild = BuildInstanceMesh( pChild, bLine, alpha);

		pNode->AddChild( pNewChild);
	}

	if( i3::same_of<i3BinaryPartitionNode*>(pSrc))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pSrc;

		// Front
		pChild = pBSP->getFront();

		if( pChild != nullptr)
		{
			pNewChild = BuildInstanceMesh( pChild, bLine, alpha);

			pNode->AddChild( pNewChild);
		}

		// Back
		pChild = pBSP->getBack();

		if( pChild != nullptr)
		{
			pNewChild = BuildInstanceMesh( pChild, bLine, alpha);

			pNode->AddChild( pNewChild);
		}
	}
	else if( i3::same_of<i3OcTreeNode* >(pSrc))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pSrc;

		for( i = 0; i < 8; i++)
		{
			i3OcTreeNode * pChildOctree = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChildOctree == nullptr)
				continue;

			pNewChild = BuildInstanceMesh( pChildOctree, bLine, alpha);

			pNode->AddChild( pNewChild);
		}
	}

	return pNode;
}

i3Node *	_BuildGeoWire( i3Geometry * pGeo, COLOR * pCol)
{
	i3Node * pRoot;
	INT32 i;

	pRoot = i3Node::new_object_ref();
	pRoot->SetName( pGeo->GetNameString());

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		i3Geometry * pNewGeo;
		i3GeometryAttr * pGeoAttr;
		i3RenderAttr * pAttr;

		pAttr = pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr*>(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->SetName( pGeo->GetNameString());
		pRoot->AddChild( pAttrSet);

		{
			i3RenderModeAttr * pAttr = i3RenderModeAttr::new_object_ref();

			pAttr->Set( I3G_RENDER_LINE);

			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			COLOR col;

			if( pCol == nullptr)
			{
				col.r = (UINT8) ((i3Math::Rand() % 128) + 92);
				col.g = (UINT8) ((i3Math::Rand() % 128) + 92);
				col.b = (UINT8) ((i3Math::Rand() % 128) + 92);
				col.a = 255;
			}
			else
			{
				i3Color::Copy( &col, pCol);
			}

			pAttr->Set( &col);

			pAttrSet->AddAttr( pAttr);
		}

		pNewGeo = i3Geometry::new_object_ref();
		pAttrSet->AddChild( pNewGeo);


		// Line
		pNewGeo->AppendGeometryAttr( pGeoAttr);
	}

	return pRoot;
}

I3_EXPORT_SGUT 
i3Node *	i3SceneUtil::BuildWireMesh( i3Node * pSrc, COLOR * pCol)
{
	i3Node * pNode;

	if( i3::same_of<i3Geometry*>(pSrc))
	{
		pNode = _BuildGeoWire( (i3Geometry *) pSrc, pCol);
	}
	else if( i3::kind_of<i3Transform*>(pSrc))
	{
		i3Transform * pTrans = i3Transform::new_object_ref();

		pTrans->SetMatrix( ((i3Transform *) pSrc)->GetMatrix());

		pNode = pTrans;
	}
	else
	{
		pNode = i3Node::new_object_ref();
	}

	if( pSrc->hasName())
		pNode->SetName( pSrc->GetNameString());

	INT32 i;
	i3Node * pChild, * pNewChild;

	for( i = 0; i < pSrc->GetChildCount(); i++)
	{
		pChild = pSrc->GetChild( i);

		pNewChild = BuildWireMesh( pChild, pCol);

		pNode->AddChild( pNewChild);
	}

	if( i3::same_of<i3BinaryPartitionNode*>(pSrc))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pSrc;

		// Front
		pChild = pBSP->getFront();

		if( pChild != nullptr)
		{
			pNewChild = BuildWireMesh( pChild, pCol);

			pNode->AddChild( pNewChild);
		}

		// Back
		pChild = pBSP->getBack();

		if( pChild != nullptr)
		{
			pNewChild = BuildWireMesh( pChild, pCol);

			pNode->AddChild( pNewChild);
		}
	}
	else if( i3::same_of<i3OcTreeNode* >(pSrc))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pSrc;

		for( i = 0; i < 8; i++)
		{
			i3OcTreeNode * pChildOctree = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChildOctree == nullptr)
				continue;

			pNewChild = BuildWireMesh( pChildOctree, pCol);

			pNode->AddChild( pNewChild);
		}
	}

	return pNode;
}


//
// RenColliderColorTable
//

I3COLOR RenColliderColorTable::ms_default_HitColor[I3_TERRAIN_TYPE_COUNT] =
{
	I3COLOR( 255,255,255,128 ),/*I3_TERRAIN_NA*/

	I3COLOR( 255,0,0,128 ),	/*I3_TERRAIN_CONCRETE*/		
	I3COLOR( 236,161,51,128 ), /*I3_TERRAIN_STEEL*/			
	I3COLOR( 236,243,51,128 ), /*I3_TERRAIN_GROUND*/			
	I3COLOR( 153,39,51,128 ),	/*I3_TERRAIN_WOOD*/			
	I3COLOR( 236,243,51,128 ), /*I3_TERRAIN_SNOW*/			

	I3COLOR( 255,255,255,128 ),/*I3_TERRAIN_WATER_DEEP*/
	I3COLOR( 255,255,255,128 ),/*I3_TERRAIN_WATER_SHALLOW*/

	I3COLOR( 236,243,51,128 ), /*I3_TERRAIN_WET_CLAY*/		
	I3COLOR( 32,243,51,128 ),	/*I3_TERRAIN_GRASS*/			

	I3COLOR( 255,0,0,128 ),	/*I3_TERRAIN_MARBLE*/			

	I3COLOR( 32,243,51,128 ),	/*I3_TERRAIN_FALLLEAF*/		

	I3COLOR( 255,0,0,128 ),	/*I3_TERRAIN_CONCRETE_THIN*/	
	I3COLOR( 236,161,51,128 ), /*I3_TERRAIN_STEEL_THIN*/		
	I3COLOR( 153,39,51,128 ),	/*I3_TERRAIN_WOOD_THIN*/		
	I3COLOR( 255,0,0,128 ),	/*I3_TERRAIN_MARBLE_THIN*/	

	I3COLOR( 238,238,238,128 ),/*I3_TERRAIN_PAPER*/			 
	I3COLOR( 154,39,255,128 ), /*I3_TERRAIN_GLASS*/			

	I3COLOR( 154,161,173,128 ),/*I3_TERRAIN_PLASTIC*/		 
	I3COLOR( 032,243,255,128 ),/*I3_TERRAIN_RUBBER*/			 
	I3COLOR( 238,238,238,128 ),/*I3_TERRAIN_CLOTH*/			 

	I3COLOR( 236,243,51,128 ), /*I3_TERRAIN_GROUND_THIN*/	
	I3COLOR( 236,243,51,128 ), /*I3_TERRAIN_SNOW_THIN*/		
	I3COLOR( 236,243,51,128 ), /*I3_TERRAIN_WET_CLAY_THIN*/	

	I3COLOR( 32,243,51,128 ),	/*I3_TERRAIN_GRASS_THIN*/		
	I3COLOR( 238,238,238,128 ),/*I3_TERRAIN_PAPER_THIN*/		 
	I3COLOR( 32,243,51,128 ),	/*I3_TERRAIN_FALLLEAF_THIN*/	

	I3COLOR( 154,39,255,128 ), /*I3_TERRAIN_GLASS_THIN*/		
	I3COLOR( 154,161,173,128 ),/*I3_TERRAIN_PLASTIC_THIN*/	 

	I3COLOR( 032,243,255,128 ),/*I3_TERRAIN_RUBBER_THIN*/	
	I3COLOR( 238,238,238,128 ),/*I3_TERRAIN_CLOTH_THIN*/		 
	I3COLOR( 236,161,51,128 ), /*I3_TERRAIN_BARBEDWIRE*/	

	I3COLOR( 255,255,255,128 ),/*I3_TERRAIN_BLOOD*/
	I3COLOR( 255,255,255,128 ),/*I3_TERRAIN_TEMP1*/
	I3COLOR( 255,255,255,128 ),/*I3_TERRAIN_TEMP2*/

	I3COLOR( 255,255,255,128 ),/*I3_TERRAIN_CLOTH_NO_DECAL*/

	I3COLOR( 236,161,51,128 ), /*I3_TERRAIN_STEEL_WIRE*/
	I3COLOR( 153,39,51,128 ),	/*I3_TERRAIN_WOOD_WIRE*/
	I3COLOR( 154,161,173,128 ), /*I3_TERRAIN_PLASTIC_WIRE*/
	I3COLOR( 0, 0, 0, 128)			/*I3_TERRAIN_NONE*/
};

RenColliderColorTable::RenColliderColorTable(const char* iniPath)
{
	m_cate = ECATE_DEFAULT;

	if(iniPath)
		strcpy_s(m_iniFileName, MAX_PATH, iniPath);
	else
	{
		char filePath[MAX_PATH]={0};
		GetModuleFileName(nullptr, filePath, sizeof(filePath));

		char* p = strrchr(filePath, '\\');

		if(p != nullptr)
			*p = 0;

		sprintf_s(m_iniFileName, MAX_PATH, "%s\\_RenColColor.ini", filePath);
	}

	SetDefault();

	Reload(m_iniFileName);	
}

RenColliderColorTable::~RenColliderColorTable()
{
}

struct RenColliderClrTbl_DefaultColor
{
	char		m_prefix[MAX_PATH];
	I3COLOR		m_clr;

};

static RenColliderClrTbl_DefaultColor gs_defaultHitColor[] =
{
	{ "DEFAULT",					I3COLOR( 255,255,255,128) },
	{ "I3_TERRAIN_CONCRETE",		I3COLOR( 255,0,0,192	) },			
	{ "I3_TERRAIN_MARBLE",			I3COLOR( 255,0,0,192	) },			
	{ "I3_TERRAIN_CONCRETE_THIN",	I3COLOR( 255,0,0,100	) },	
	{ "I3_TERRAIN_MARBLE_THIN",		I3COLOR( 255,0,0,100	) },		
	{ "I3_TERRAIN_STEEL",			I3COLOR( 236,161,51,192 ) },			
	{ "I3_TERRAIN_STEEL_THIN",		I3COLOR( 236,161,51,100 ) },	
	{ "I3_TERRAIN_BARBEDWIRE",		I3COLOR( 236,161,51,100 ) },	
	{ "I3_TERRAIN_STEEL_WIRE",		I3COLOR( 236,161,51,100 ) },	
	{ "I3_TERRAIN_GROUND",			I3COLOR( 236,243,51,192 ) },		
	{ "I3_TERRAIN_SNOW",			I3COLOR( 236,243,51,192 ) },			
	{ "I3_TERRAIN_WET_CLAY",		I3COLOR( 236,243,51,192 ) },		
	{ "I3_TERRAIN_GROUND_THIN",		I3COLOR( 236,243,51,100 ) },	
	{ "I3_TERRAIN_SNOW_THIN",		I3COLOR( 236,243,51,100 ) },		
	{ "I3_TERRAIN_WET_CLAY_THIN",	I3COLOR( 236,243,51,100 ) },	
	{ "I3_TERRAIN_GRASS",			I3COLOR( 32,243,51,192 ) },			
	{ "I3_TERRAIN_FALLLEAF",		I3COLOR( 32,243,51,192 ) },		
	{ "I3_TERRAIN_GRASS_THIN",		I3COLOR( 32,243,51,100 ) },		
	{ "I3_TERRAIN_FALLLEAF_THIN",	I3COLOR( 32,243,51,100 ) },	
	{ "I3_TERRAIN_PAPER",			I3COLOR( 238,238,238,192 ) },		
	{ "I3_TERRAIN_CLOTH",			I3COLOR( 238,238,238,192 ) },		
	{ "I3_TERRAIN_PAPER_THIN",		I3COLOR( 238,238,238,100 ) },	
	{ "I3_TERRAIN_CLOTH_THIN",		I3COLOR( 238,238,238,100 ) },	
	{ "I3_TERRAIN_WOOD",			I3COLOR( 153,39,51,192 ) },			
	{ "I3_TERRAIN_WOOD_THIN",		I3COLOR( 153,39,51,100 ) },		
	{ "I3_TERRAIN_WOOD_WIRE",		I3COLOR( 153,39,51,100 ) },		
	{ "I3_TERRAIN_GLASS",			I3COLOR( 154,39,255,192 ) },			
	{ "I3_TERRAIN_GLASS_THIN",		I3COLOR( 154,39,255,100 ) },	
	{ "I3_TERRAIN_PLASTIC",			I3COLOR( 154,161,173,192 ) },		
	{ "I3_TERRAIN_PLASTIC_THIN",	I3COLOR( 154,161,173,100 ) },	
	{ "I3_TERRAIN_PLASTIC_WIRE",	I3COLOR( 154,161,173,100 ) },	
	{ "I3_TERRAIN_RUBBER",			I3COLOR( 32,243,255,192 ) },		
	{ "I3_TERRAIN_RUBBER_THIN",		I3COLOR( 032,243,255,100 ) },	
	{ "I3_TERRAIN_NONE",			I3COLOR(0,0,0,192) }
};

const static INT32 gs_numDefaultHitColor = sizeof(gs_defaultHitColor) / sizeof(gs_defaultHitColor[0]);


void RenColliderColorTable::SetDefault()
{

	for(INT32 i=0; i<ECATE_MAX; i++)
	{
		m_hmapPrefixColor[i].clear();
		m_data[i].clear();

		switch(i)
		{
		case ECATE_SM:
			{
				m_defaultClr[i].r = 0; m_defaultClr[i].g = 255; m_defaultClr[i].b = 0; m_defaultClr[i].a = 128;
			}
			break;

		case ECATE_HIT:
			{
				m_data[i].resize(I3_TERRAIN_TYPE_COUNT);

				for(INT32 k=0; k<I3_TERRAIN_TYPE_COUNT; k++)
				{
					m_data[i][k].m_nodePrefix = i3TerrainTypeToNodeNamePrefix((I3_TERRAIN_TYPE)k);

					for(INT32 m=0; m<gs_numDefaultHitColor; m++)
					{
						if(!stricmp(gs_defaultHitColor[m].m_prefix, i3TerrainTypeToStr((I3_TERRAIN_TYPE)k)))
							m_data[i][k].m_clr = gs_defaultHitColor[m].m_clr;
					}

					PrefixColorHMap::value_type val(i3TerrainTypeToStr((I3_TERRAIN_TYPE)k), &m_data[i][k]);
					m_hmapPrefixColor[i].insert(val);
				}
			}
			break;

		default:
			{
				m_defaultClr[i].r = 255; m_defaultClr[i].g = 255; m_defaultClr[i].b = 255; m_defaultClr[i].a = 128;
			}
			break;
		}
	}
}

bool RenColliderColorTable::Reload(const char* path)
{
	INT32 err = 0;

	if(_access(path, 0) == -1)
		err = 1;

	const char* sectionName[ECATE_MAX] = { nullptr, "HIT", "SM" };

	if(!err)
	{
		size_t numHitParsed = 0;
		char keyName[32];
		DWORD no = 0;
		char buf[1024];

		// parse [HIT] section
		for(INT32 i=0; i<ECATE_MAX; i++)
		{
			if(i==0)
				continue;

			no = 0;

			for(;;)
			{
				sprintf_s(keyName, 32, "%03d", no);
				DWORD retSiz = GetPrivateProfileString(sectionName[i], keyName, nullptr, 
					buf, 1024, m_iniFileName);

				if(retSiz < 1)
					break;

				I3_TERRAIN_TYPE type;
				I3COLOR clr;
				char name[MAX_PATH];

				if(ParseSectionRow_NameColor(name, clr, buf))
				{
					if(no == 0)
					{
						m_defaultClr[i] = clr;
					}
					else
					{
						if(i == ECATE_HIT)
						{
							type = i3TerrainTypeNameToType(name);

							if(type != I3_TERRAIN_NA)
							{
								numHitParsed++;
								m_data[i][type].m_clr = clr;
							}
							else
							{
								I3PRINTLOG(I3LOG_NOTICE, "Error in parsing : section: [HIT] , list: %s ", buf);
							}
						}
						else
						{
							;
						}
					}
				}
				else
				{
					I3PRINTLOG(I3LOG_NOTICE, "Error in parsing : section: [HIT] , list: %s ", buf);
				}

				no++;
			}

		}
	}	

	if(err)
		SetDefault();

	return err == 0 ? true : false;
}

const I3COLOR& RenColliderColorTable::GetColorByNodePrefix(const char* prefix, ECATE cate)
{
	if(cate == ECATE_CUR)
		cate = m_cate;

	Data* dat = GetByNodePrefix(prefix, cate);
	return dat ? dat->m_clr : m_defaultClr[cate];
}

RenColliderColorTable::Data* RenColliderColorTable::GetByNodePrefix(const char* nodeName, ECATE cate)
{
	if(!nodeName)
		return nullptr;

	if(cate == ECATE_CUR)
		cate = m_cate;

	TerTypeDataVec::iterator x = m_data[cate].begin();
	while(x != m_data[cate].end())
	{	
		const char* src = (*x).m_nodePrefix.c_str();				
		if(!strnicmp(src, nodeName, strlen(src)))
			return &(*x);

		x++;
	};	

	return nullptr;
}

bool RenColliderColorTable::ParseSectionRow_NameColor(char* rstBuf, I3COLOR& rstClr, const char* src)
{
	// I3_TERRAIN_CONCRETE,255,0,0,128
	const char* c = src;

	typedef std::vector<std::string>  StrVec;
	StrVec tokenVec;

	char token[MAX_PATH];
	INT32 cur=0;

	for(;;)
	{
		if(*c == ',' || *c == 0)
		{
			if(cur > 0)
			{
				token[cur] = 0;
				tokenVec.push_back(token);
				cur = 0;
			}

			if(*c == 0)
				break;

			c++;
			continue;
		}

		if(cur < MAX_PATH - 1)			// 하나 더 감소시켜야... token[MAX_PATH]가 되지 않음...  (2015.04.06.수빈)
			token[cur++] = *c;

		c++;
	}

	if(tokenVec.size() < 5)
		return false;

	if(rstBuf)
	{
		strcpy_s(rstBuf, MAX_PATH, tokenVec[0].c_str());
		strupr(rstBuf);
	}

	rstClr.r = (BYTE)(atoi(tokenVec[1].c_str()) & 0xff);
	rstClr.g = (BYTE)(atoi(tokenVec[2].c_str()) & 0xff);
	rstClr.b = (BYTE)(atoi(tokenVec[3].c_str()) & 0xff);
	rstClr.a = (BYTE)(atoi(tokenVec[4].c_str()) & 0xff);

	return true;
}

char* RenColliderColorTable::ToUpperStr(const char* in)
{
	if(!in)
		return nullptr;

	strcpy_s(m_upperBuf, MAX_PATH, in);
	strupr(m_upperBuf);

	return m_upperBuf;
}

void RenColliderColorTable::GenerateDefaultIni(const char* path)
{
	char filePath[MAX_PATH];

	if(path)
	{
		sprintf_s(filePath, MAX_PATH, "%s\\_RenColColor.ini", path);
	}
	else
	{
		char modulePath[MAX_PATH];
		GetModuleFileName(nullptr, modulePath, sizeof(modulePath));
		char* p = strrchr(modulePath, '\\');

		if (p != nullptr)
			*p = 0;

		sprintf_s(filePath, MAX_PATH, "%s\\_RenColColor.ini", modulePath);
	}

	char key[MAX_PATH];
	char val[MAX_PATH];

	WritePrivateProfileSection("SM", nullptr, filePath);
	WritePrivateProfileString("SM", "000", "DEFAULT,0,255,0,128", filePath);

	WritePrivateProfileSection("HIT", nullptr, filePath);
	for(INT32 i=0; i<gs_numDefaultHitColor; i++)
	{	
		sprintf_s(key, MAX_PATH, "%03d", i);
		sprintf_s(val, MAX_PATH, "%s,%d,%d,%d,%d", gs_defaultHitColor[i].m_prefix, 
			gs_defaultHitColor[i].m_clr.r,
			gs_defaultHitColor[i].m_clr.g,
			gs_defaultHitColor[i].m_clr.b,
			gs_defaultHitColor[i].m_clr.a);

		WritePrivateProfileString("HIT",key,val,filePath );
	}
}
