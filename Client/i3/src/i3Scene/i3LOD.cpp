#include "i3SceneDef.h"
#include "i3LOD.h"
#include "i3SceneUtil.h"
#include "i3BoneRef.h"
#include "i3Geometry.h"
#include "i3AttrSet.h"

#include "i3LightingEnableAttr.h"
#include "i3ColorAttr.h"
#include "i3ZTestEnableAttr.h"
#include "i3ZWriteEnableAttr.h"
#include "i3FaceCullModeAttr.h"
#include "i3ZBiasAttr.h"
#include "i3Base/string/ext/generic_string_cat.h"

I3_CLASS_INSTANCE( i3LOD);


i3LOD::~i3LOD(void)
{
	I3_SAFE_RELEASE( m_pSkel);
#if defined( I3_DEBUG)
	if( m_pShapeNode != nullptr)
	{
		m_pShapeNode->SetOwner( nullptr);
	}
#endif
	I3_SAFE_RELEASE( m_pShapeNode);
	I3_SAFE_RELEASE( m_pBoneMap);
}

void i3LOD::SetBoneMapTable( INT32 * pTable, UINT32 Count)
{
	I3ASSERT( Count == (UINT32) getBoneCount());

	if( m_pBoneMap == nullptr)
	{
		m_pBoneMap = i3MemoryBuffer::new_object();
		I3ASSERT( m_pBoneMap != nullptr);
	}

	m_pBoneMap->Create( sizeof(INT32) * Count);

	if( pTable == nullptr)
		return;

	i3mem::Copy( m_pBoneMap->getBuffer(), pTable, Count * sizeof(INT32));
}

INT32 i3LOD::hasShapeSet(void)
{
	INT32 i;
	INT32 cnt = 0;

	if( m_pSkel != nullptr)
	{
		for( i = 0; i < m_pSkel->getBoneCount(); i++)
		{
			if( m_pSkel->isShapeSet(i))
				cnt++;
		}
	}

	return cnt;
}

void i3LOD::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	I3ASSERT( i3::same_of<i3LOD*>(pDest));

	i3LOD * pLOD = (i3LOD *) pDest;

	if( method == I3_COPY_REF)
	{
		pLOD->setSkeleton( getSkeleton());
		pLOD->setShapeNode( getShapeNode());
	}
	else
	{
		if( pLOD->m_pSharedSkel == nullptr)
		{
			if( getSkeleton() != nullptr)
			{
				i3Skeleton * pSkel = i3Skeleton::new_object_ref();

				getSkeleton()->CopyTo( pSkel, method);

				pLOD->setSkeleton( pSkel);
			}
		}
		else
		{
			pLOD->setSkeleton( pLOD->m_pSharedSkel);
		}

		if( getShapeNode() != nullptr)
		{
			i3Node * pNode = i3Scene::CreateClone( getShapeNode(), method);

			pLOD->setShapeNode( pNode);
		}
	}

	if( m_pBoneMap != nullptr)
	{
		pLOD->setBoneMap( m_pBoneMap);
	}
}

void i3LOD::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( getSkeleton() != nullptr)
		getSkeleton()->OnBuildObjectList( List);

	if( getShapeNode() != nullptr)
		getShapeNode()->OnBuildObjectList( List);

	if( m_pBoneMap != nullptr)
		m_pBoneMap->OnBuildObjectList( List);

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 SKINLOD
	{
		UINT8		m_ID[4] = { 'S', 'L', 'D', '1' };
		OBJREF		m_pSkel = 0;
		OBJREF		m_pShapeNode = 0;
		UINT32		m_BoneMapCount = 0;
		UINT8		pad[12] = { 0 };
	};

	struct ALIGN8 LOD
	{
		UINT8		m_ID[4] = { 'B', 'L', 'D', '1' };
		OBJREF64	m_pSkel = 0;
		OBJREF64	m_pShapeNode = 0;
		OBJREF64	m_pBoneMap = 0;
		UINT8		pad[12] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3LOD::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::LOD data;

	data.m_pSkel		= (OBJREF64) pResFile->GetObjectPersistID( getSkeleton());
	data.m_pShapeNode	= (OBJREF64) pResFile->GetObjectPersistID( getShapeNode());
	data.m_pBoneMap		= (OBJREF64) pResFile->GetObjectPersistID( m_pBoneMap);

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3LOD::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT8 id[4];

	Rc = pStream->Read(id, 4);
	I3_CHKIO(Rc);
	Result = 0;

	pStream->SetPosition(-4, STREAM_CURRENT);

	if (memcmp(id, "BLD1", 4) == 0)
	{
		// Body LOD (new version)
		pack::LOD data;

		Rc = pStream->Read(&data, sizeof(data));
		I3_CHKIO(Rc);
		Result += Rc;

		if (data.m_pSkel != 0)
		{
			i3Skeleton * pSkel = (i3Skeleton *)pResFile->FindObjectByID((OBJREF)data.m_pSkel);
			I3ASSERT(pSkel != nullptr);

			setSkeleton(pSkel);
		}

		if (data.m_pShapeNode != 0)
		{
			i3Node * pNode = (i3Node *)pResFile->FindObjectByID((OBJREF)data.m_pShapeNode);
			I3ASSERT(pNode != nullptr);

			setShapeNode(pNode);
		}

		if (data.m_pBoneMap != 0)
		{
			i3MemoryBuffer * pBuf = (i3MemoryBuffer *)pResFile->FindObjectByID((OBJREF)data.m_pBoneMap);
			I3ASSERT(pBuf != nullptr);

			setBoneMap(pBuf);
		}
	}
	else
	{
		pack::SKINLOD data;

		I3TRACE("[NOTICE] !!!!!!  Old skin lod data is used. !!!!!!! \n");

		Rc = pStream->Read(&data, sizeof(data));
		I3_CHKIO(Rc);
		Result += Rc;

		if (data.m_pSkel != 0)
		{
			i3Skeleton * pSkel = (i3Skeleton *)pResFile->FindObjectByID(data.m_pSkel);
			I3ASSERT(pSkel != nullptr);

			setSkeleton(pSkel);
		}

		if (data.m_pShapeNode != 0)
		{
			i3Node * pNode = (i3Node *)pResFile->FindObjectByID(data.m_pShapeNode);
			I3ASSERT(pNode != nullptr);

			setShapeNode(pNode);
		}

		if (data.m_BoneMapCount > 0)
		{
			SetBoneMapTable(nullptr, data.m_BoneMapCount);
			Rc = pStream->Read(m_pBoneMap->getBuffer(), sizeof(INT32) * data.m_BoneMapCount);
			I3_CHKIO(Rc);
			Result += Rc;
		}
	}

	return Result;
}

bool i3LOD::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3PersistantElement::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	//pXML->addAttr("ID", "BLD1");
 	pXML->addAttr("SkeletonID", (OBJREF64)pFile->GetObjectPersistID(getSkeleton()));
 	pXML->addAttr("ShapeNodeID", (OBJREF64)pFile->GetObjectPersistID(getShapeNode()));
 	pXML->addAttr("BoneMapID", (OBJREF64)pFile->GetObjectPersistID(m_pBoneMap));

	return true;
}
bool i3LOD::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3PersistantElement::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	//i3::rc_string strID = pXML->getAttr("ID");

	//if (memcmp(strID.c_str(), "BLD1", 4) == 0)
	{
		OBJREF64 temp;
		pXML->getAttr("SkeletonID", &temp);
		if (temp != 0)
		{
			i3Skeleton * pSkel = (i3Skeleton *)pFile->FindObjectByID((OBJREF)temp);
			I3ASSERT(pSkel != nullptr);

			setSkeleton(pSkel);
		}

		pXML->getAttr("ShapeNodeID", &temp);
		if (temp != 0)
		{
			i3Node * pNode = (i3Node *)pFile->FindObjectByID((OBJREF)temp);
			I3ASSERT(pNode != nullptr);

			setShapeNode(pNode);
		}

		pXML->getAttr("BoneMapID", &temp);
		if (temp != 0)
		{
			i3MemoryBuffer * pBuf = (i3MemoryBuffer *)pFile->FindObjectByID((OBJREF)temp);
			I3ASSERT(pBuf != nullptr);

			setBoneMap(pBuf);
		}
	}

	return true;
}

void i3LOD::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	if( getSkeleton() != nullptr)
	{
		getSkeleton()->OnInstancing( pInfo);
	}

	if( getShapeNode() != nullptr)
	{
		getShapeNode()->OnInstancing( pInfo);
	}
}

#if defined( I3_DEBUG)

i3BoneRef * i3LOD::_CreateRenderableBoneRef( INT32 iBoneIdx, const char * pszBoneName)
{
	INT32 i;
	char szNodeName[ MAX_NAME];
	VEC3D vPos;
	COLORREAL color;
	i3VertexFormat format;

	i3BoneRef * pBoneRef = i3BoneRef::new_object_ref();
	I3ASSERT( pBoneRef != nullptr);

	pBoneRef->setBoneIndex( iBoneIdx);
	pBoneRef->SetName( pszBoneName);

	format.SetHasPosition( true);
	format.SetHasColor( true);

	for( i = 0; i < 3; i++)
	{
		// Geometry
		i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();

		i3VertexArray * pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, 2, 0);

		{
			pVA->Lock();

			i3Vector::Set( &vPos, 0.f, 0.f, 0.f);
			pVA->SetPosition( 0, &vPos);
			
			i3Vector::Set( &vPos, (REAL32)(i%3==0)*m_rRenderableBoneAxisLen, (REAL32)(i%3==1)*m_rRenderableBoneAxisLen, (REAL32)(i%3==2)*m_rRenderableBoneAxisLen);
			i3Color::Set( &color, (REAL32)(i%3==0)*0.5f + 0.5f, (REAL32)(i%3==1)*0.5f + 0.5f, (REAL32)(i%3==2)*0.5f+0.5f, 0.6f);

			pVA->SetPosition( 1, &vPos);
			pVA->SetColor( 0, &color);
			pVA->SetColor( 1, &color);

			pVA->Unlock();
		}

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0, false );

		i3Geometry * pGeo = i3Geometry::new_object_ref();

		if( i == 0 )		i3::safe_string_copy( szNodeName, "renderable_AxisX\0", sizeof( szNodeName));
		else if( i == 1 )	i3::safe_string_copy( szNodeName, "renderable_AxisY\0", sizeof( szNodeName));
		else				i3::safe_string_copy( szNodeName, "renderable_AxisZ\0", sizeof( szNodeName));

		pGeo->SetName( szNodeName);

		pGeo->AppendGeometryAttr( pGeoAttr);

		pBoneRef->AddChild( pGeo);
	}

	return pBoneRef;
}

void i3LOD::CreateRenderableBoneAxis( REAL32 rAxisLen)
{
	INT32 i;
	char	szBoneRefName[ MAX_NAME];

	I3ASSERT( getShapeNode() != nullptr);

	m_rRenderableBoneAxisLen = rAxisLen;
	
	i3Node * pFindNode = i3Scene::FindNodeByName( getShapeNode(), "Debug_RotateAxis");
	if( pFindNode == nullptr)
	{
		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->SetName( "Debug_RotateAxis");
		pAttrSet->AddFlag( I3_NODEFLAG_VOLATILE);	// 저장하지 않습니다.

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			COLOR c;
			i3Color::Set( &c, (UINT8) 255, 255, 255, 255);
			pAttr->Set( &c);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
			pAttr->Set( I3G_FACE_CULL_FRONT);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
			pAttr->setBias( -0.99f);
			pAttrSet->AddAttr( pAttr);
		}

		INT32 iBoneCount = getBoneCount();

		if( getSkeleton() != nullptr)
		{
			for( i = 0; i < iBoneCount; i++)
			{
				i3Bone * pBone = getSkeleton()->getBone( i);
				i3::safe_string_copy( szBoneRefName, pBone->getNameString(), sizeof( szBoneRefName) );

				i3::generic_string_cat( szBoneRefName, "_Ref");

				i3BoneRef * pBoneRef = _CreateRenderableBoneRef( i, szBoneRefName);
				pAttrSet->AddChild( pBoneRef);
			}
		}

		m_pRenderableBoneAxis = pAttrSet;
		I3_SAFE_ADDREF( m_pRenderableBoneAxis );

		getShapeNode()->AddChild( m_pRenderableBoneAxis);
	}
}

void i3LOD::EnableRenderableBoneAxis( INT32 iBoneIdx, bool bEnable)
{
	char szBoneRefName[ MAX_NAME] = {0,};

	if( m_pRenderableBoneAxis != nullptr)
	{
		if( getSkeleton() != nullptr)
		{
			i3::safe_string_copy( szBoneRefName, getSkeleton()->getBone( iBoneIdx)->getNameString(), sizeof( szBoneRefName));
		}

		i3::generic_string_cat( szBoneRefName, "_Ref");

		i3BoneRef * pBoneRef = (i3BoneRef*) i3Scene::FindNodeByName( m_pRenderableBoneAxis, szBoneRefName);

		if( pBoneRef != nullptr)
		{
			I3ASSERT( i3::same_of<i3BoneRef*>(pBoneRef));
			if( bEnable)	pBoneRef->RemoveFlag( I3_NODEFLAG_DISABLE);
			else			pBoneRef->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}
}

void i3LOD::EnableAllRenderableBoneAxis( bool bEnable)
{
	INT32 i;
	INT32 iBoneCount = getBoneCount();

	for( i = 0; i < iBoneCount; i++)
	{
		EnableRenderableBoneAxis( i, bEnable);
	}
}

void i3LOD::ReleaseRenderableBoneAxis( void)
{
	I3_SAFE_NODE_RELEASE( m_pRenderableBoneAxis);
}

#endif



