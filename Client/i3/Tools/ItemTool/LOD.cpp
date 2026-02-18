#include "stdafx.h"
#include "LOD.h"
#include "MyOptScene2Body.h"

I3_CLASS_INSTANCE( CLOD); //, i3ResourceObject);

CLOD::CLOD(void)
{
	m_pSg = NULL;
	m_BoneCount = 0;
	m_TriCount = 0;
	m_SgStyle = 0;
	m_Level = 0;
	m_bGather = false;

	m_pLOD = i3LOD::new_object();
}

CLOD::~CLOD(void)
{
	I3_SAFE_RELEASE( m_pSg);
	I3_SAFE_RELEASE( m_pLOD);
}

INT32 CLOD::_GetTriangleCount( i3GeometryAttr * pGeoAttr)
{
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	i3IndexArray * pIA = pGeoAttr->GetIndexArray();
	INT32 i, count;

	if( pGeoAttr->GetPrimitiveType() == I3G_PRIM_TRILIST)
		return pGeoAttr->GetPrimitiveCount();

	if( pGeoAttr->GetPrimitiveLengthArray() != NULL)
	{
		if( pIA != NULL)
		{
			if( pGeoAttr->GetPrimitiveCount() > 1)
			{
				count = 0;

				for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
				{
					count += pGeoAttr->GetPrimitiveLength( i);
				}
			}
			else
			{
				return pIA->GetCount() - 2;
			}
		}
		else
		{
			if( pGeoAttr->GetPrimitiveCount() > 1)
			{
				count = 0;

				for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
				{
					count += pGeoAttr->GetPrimitiveLength( i);
				}
			}
			else
			{
				return pVA->GetCount() - 2;
			}
		}
	}

	if( pIA != NULL)
		return pIA->GetCount() - 2;

	return pVA->GetCount() - 2;
}

void CLOD::SetSceneGraph( i3SceneGraphInfo * pInfo, INT32 Level, bool bGather)
{
	INT32 j;

	I3_REF_CHANGE( m_pSg, pInfo);

	m_TriCount = 0;
	m_BoneCount = 0;
	m_SgStyle = 0;
	m_Level = Level;
	m_bGather = bGather;

	{
		i3Body * pBody = (i3Body *) i3Scene::FindNodeByType( pInfo->getInstanceSg(), i3Body::static_meta(), true);

#if TRUE
		// 아래는 옛날 형식이면 바꾸냐는 질문이 나오지만
		// 안바꾸면 크래쉬가 나오기 때문에 무조건 바꾸도록 수정합니다.
		// 아래 코드를 지우지 않는 이유는 정확히 무슨 의미의 코드가 무엇인지 몰라서입니다,

		if( pBody == NULL)
		{
  			cMyOptScene2Body opt;
  			opt.Optimize( pInfo);
  			pInfo->setInstanceSg( opt.getTraceRoot());
		}
#else
		if( pBody == NULL)
		{
			int id = AfxMessageBox( "이거 LOD 설정이 불가능한 옛날 형식이다. 바꿔서라도 계속해?", MB_YESNO);

			if( id == IDNO)
				return;

			i3OptScene2Body opt;

			opt.Optimize( pInfo);

			pInfo->setInstanceSg( opt.getTraceRoot());

			/*
			{
				i3SceneFile file;

				file.SetSceneGraph( opt.GetSceneRoot());

				file.Save( "d:\\test.i3s");
			}
			*/
		}
#endif
	}

	// Bone 개수
	{
		i3::vector<i3Node*> List;

		// i3Transform Node에 대한 검색
		{
			if( m_pSg->getCommonSg() != NULL)
			{
				i3Scene::FindNodeByType( m_pSg->getCommonSg(), i3Transform::static_meta(), List);
			}

			if( m_pSg->getInstanceSg() != NULL)
			{
				i3Scene::FindNodeByType( m_pSg->getInstanceSg(), i3Transform::static_meta(), List);
			}

			m_BoneCount += (INT32)List.size();

			List.clear();
		}

		// i3Body Node의 검색
		{
			if( m_pSg->getCommonSg() != NULL)
			{
				i3Scene::FindNodeByType( m_pSg->getCommonSg(), i3Body::static_meta(), List);
			}

			if( m_pSg->getInstanceSg() != NULL)
			{
				i3Scene::FindNodeByType( m_pSg->getInstanceSg(), i3Body::static_meta(), List);
			}

			if( List.size() > 0)
			{
				m_SgStyle |= SG_STYLE_SKIN;
			}

			for(size_t i = 0; i < List.size(); i++)
			{
				i3Body * pBody = (i3Body *) List[i];
				i3LOD * pTargetLOD = pBody->getLOD( m_Level);

				pTargetLOD->CopyTo( m_pLOD, I3_COPY_INSTANCE);

				for( j = 0; j < (INT32) pBody->getLODCount(); j++)
				{
					i3LOD * pLOD = pBody->getLOD( j);

					if( pLOD->getSkeleton() != NULL)
					{
						m_BoneCount += pLOD->getSkeleton()->getBoneCount();
					}
				}
			}

			if( bGather)
			{
				for( size_t i = 0; i < List.size(); i++)
				{
					i3Body * pBody = (i3Body *) List[i];

					// i3Body Node 아래에 있는 여타 노드를 모두 Level로 지정된 SkinLOD Node 아래로 옮긴다.
					for( j = 0; j < pBody->GetChildCount(); j++)
					{
						i3Node * pChild = pBody->GetChild( j);
						i3Node * pClone;
						
						pClone = i3Scene::CreateClone( pChild, I3_COPY_INSTANCE);
						m_pLOD->getShapeNode()->AddChild( pClone);
					}
				}
			}
		}
	}

	// Triangle 개수
	{
		i3::vector<i3Node*> List;

		if( m_pSg->getCommonSg() != NULL)
		{
			i3Scene::FindNodeByType( m_pSg->getCommonSg(), i3Geometry::static_meta(), List);
		}

		if( m_pSg->getInstanceSg() != NULL)
		{
			i3Scene::FindNodeByType( m_pSg->getInstanceSg(), i3Geometry::static_meta(), List);
		}

		for( size_t i = 0; i < List.size(); i++)
		{
			i3Geometry * pGeo = (i3Geometry *) List[i];

			for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
			{
				i3RenderAttr * pAttr = pGeo->GetGeometryAttr( j);

				if( !i3::same_of<i3GeometryAttr*>(pAttr)) //->IsExactTypeOf( i3GeometryAttr::GetClassMeta()) == FALSE)
					continue;

				i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

				m_TriCount += _GetTriangleCount( pGeoAttr);
			}
		}
	}
}

typedef struct _CollBoneInfo
{
	char		m_szStartBone[64];
	char		m_szEndBone[64];
	REAL32		m_Size;
	REAL32		m_StartOffset;
	REAL32		m_EndOffset;
	INT32		m_Type;
} COLL_BONE_INFO;

static COLL_BONE_INFO s_CollBoneInfo[] =
{
	{	"Neck",				"Head",				0.3f,	0.0f,	0.0f,	0},		// A
	{	"",					"",					0.0f,	0.0f,	0.0f,	-1},

	{	"Spine3",			"R Clavicle",		0.3f,	-0.3f,	-0.3f,	1},		// B
	{	"Spine3",			"L Clavicle",		0.3f,	0.3f,	0.3f,	1},		// C

	{	"Pelvis",			"Spine2",			0.3f,	-0.2f,	-0.3f,	1},		// D
	{	"Pelvis",			"Spine2",			0.3f,	0.2f,	0.3f,	1},		// E

	{	"",					"",					0.0f,	0.0f,	0.0f,	-1},
};

void CLOD::BuildCollider( void)
{
}

void CLOD::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	CLOD * pLOD = (CLOD *) pDest;

	pLOD->SetSceneGraph( getSceneGraph(), m_Level, false);
}

void CLOD::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pSg != NULL)
	{
		m_pSg->OnBuildObjectList( List);
	}

	i3ResourceObject::OnBuildObjectList( List);
}

#pragma pack( push, 8)

typedef struct _tagPersistLOD
{
	UINT8		m_ID[4];
	OBJREF64	m_pSg;
	UINT32		m_Level;
	UINT32		m_bGather;
	UINT32		pad[32];
} PERSIST_LOD;

#pragma pack( pop)

UINT32 CLOD::OnSave( i3ResourceFile * pResFile)
{
	PERSIST_LOD data;
	UINT32 Rc, Result;

	Result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( Result);

	i3mem::Fill( &data, 0, sizeof(data));
	i3mem::Copy( data.m_ID, "LOD1", 4);
	data.m_pSg = (OBJREF64) pResFile->GetObjectPersistID( getSceneGraph());
	data.m_Level = m_Level;
	data.m_bGather = (UINT32) m_bGather;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 CLOD::OnLoad( i3ResourceFile * pResFile)
{
	PERSIST_LOD data;
	UINT32 Rc, Result;

	Result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pSg != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo *) pResFile->FindObjectByID( (OBJREF) data.m_pSg);
		I3ASSERT( pInfo != NULL);

		//I3_REF_CHANGE( m_pSg, pInfo);
		SetSceneGraph( pInfo, data.m_Level, data.m_bGather != 0);
	}

	return Result;
}

