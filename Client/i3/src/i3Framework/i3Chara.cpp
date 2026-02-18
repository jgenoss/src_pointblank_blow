#include "i3FrameworkPCH.h"
#include "i3Chara.h"
#include "i3GameResChara.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3Chara);

i3Chara::~i3Chara( void)
{
	I3_SAFE_RELEASE( m_pSgInfo);
}

void i3Chara::Create( i3Framework * pFramework, i3GameResChara * pRes, int nPVType)
{
	if (nPVType != -1)
		m_nPVType = nPVType;

	//CHAR_SOLO_RENDER
	//일단 꺼둔다..
	m_nPVType = -1;

	i3Chara * pSrcChara = pRes->getChara();
	I3ASSERT( pSrcChara != nullptr);

	i3GameNode * pChild = nullptr;
	i3GameNode* pNewChild = nullptr;

	RemoveAllChild();

	pChild = pSrcChara->getFirstChild();
	while( pChild != nullptr)
	{
//		const char* szname = pChild->GetName();				// 쓰이지 않으므로 우선 주석(2012.04.10.수빈)
		pNewChild = (i3GameNode *) i3GameNode::CreateClone( pChild, I3_COPY_INSTANCE);

		I3_GAMENODE_ADDCHILD( this, pNewChild);

		pChild = pChild->getNext();
	}

	pSrcChara->CopyTo( this, I3_COPY_INSTANCE);

	i3GameObj::BindSgRes( pFramework, (i3GameResSceneGraph *) pRes);

	i3Chara::OnInitAfterLoad();

#if defined( I3_DEBUG)
// 	CreateRenderableBoneRef();
// 	CreateRenderableBone();
// 	EnableRenderBone( false);
#endif
}

void i3Chara::Create( i3Node * pSg)
{
	i3Node * pParent = getAttachNode();

	pParent->RemoveAllChild();
	pParent->AddChild( pSg);

	//i3GameObj::BindSgRes( nullptr, nullptr);
}

i3AIContext *	i3Chara::FindAI( const char * pszAIPath)
{
	char szName[32];
	INT32 idx;
	i3GameNode * pCurNode = this;

	idx = 0;

	while( *pszAIPath)
	{
		switch( *pszAIPath)
		{
			case ':' :
				szName[idx] = 0;
				
				// AI 검색
				pCurNode = pCurNode->FindChildByName( szName, false);

				if( pCurNode == nullptr)
				{
					/*i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_WARN);
					i3Error::Log( "Chara Name (%s) Cannot find AI (%s)", GetName(), szName);*/
					return nullptr;
				}				
				
				idx = 0;
				break;

			default :
				szName[idx] = *pszAIPath;
				idx++;
				break;
		}

		pszAIPath++;
	}

	szName[idx] = 0;

	if( idx > 0)
	{
		pCurNode = pCurNode->FindChildByName( szName, false);
		if( pCurNode == nullptr)
		{
			return nullptr;
		}				
	}

	// State 검사
	I3ASSERT( i3::same_of<i3AIContext*>(pCurNode));

	return  (i3AIContext *) pCurNode;
}

// Child로 있을 AI 및 AIState에 대한 설정.
//
// ex) "Lower:Stand:Idle"
//
void i3Chara::SetCurAIState( const char * pszAIPath)
{
	char szName[32];
	INT32 idx;
	i3GameNode * pCurNode = this;

	idx = 0;

	while( *pszAIPath)
	{
		switch( *pszAIPath)
		{
			case ':' :
				szName[idx] = 0;
				
				// AI 검색
				pCurNode = pCurNode->FindChildByName( szName);
				I3ASSERT( pCurNode != nullptr);
				
				idx = 0;
				break;

			default :
				szName[idx] = *pszAIPath;
				idx++;
				break;
		}

		pszAIPath++;
	}

	szName[idx] = 0;

	// State 검사
	I3ASSERT( i3::same_of<i3AIContext* >(pCurNode));
	I3ASSERT( idx > 0);

	{
		i3AIContext * pCtx = (i3AIContext *) pCurNode;
		INT32 idx = pCtx->FindAIStateByName( szName);
		i3AIState * pState;

		I3ASSERT( idx != 0);

		pState = pCtx->getAIState( idx);

		pCtx->setCurrentAIState( pState);
	}
}

void i3Chara::ReturnInstance( void)
{
	i3GameObj::ReturnInstance();

	//I3_SAFE_RELEASE( m_pSgInfo);
}

void i3Chara::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameObj::OnUpdate( rDeltaSeconds);

	if( isCharaStyle( I3_CHARA_STYLE_HUMAN))
	{
		// Thread에 영향을 받는 리소스를 업데이트
		i3VertexArrayDX::UpdateForThread();
		i3IndexArray::UpdateForThread();
		i3TextureDX::UpdateForThread();
		i3Surface::UpdateForThread();
	}
}

void i3Chara::StopAnimation( i3Animation * pAnim)
{
	m_pSceneObj->StopAnim( pAnim);
}

bool i3Chara::PlayAnimation( i3Animation * pAnim, I3_ANIM_BLEND_MODE nMode, 
							 REAL32 fStart, REAL32 fInterval, REAL32 fTimeScale, REAL32 rManualClampTime, I3_ANIM_PLAY_MODE destPlayMode,
								I3_ANIM_PLAY_MODE srcPlayMode)
{
	if( pAnim == nullptr) return false;
	// revision 2503
	if( m_pSceneObj == nullptr)	return false;

	m_pSceneObj->PlayAnim( pAnim, nMode, fStart, fInterval, fTimeScale, rManualClampTime, destPlayMode, srcPlayMode);
	
	return true;	
}

void i3Chara::OnChangeAIState( i3AIContext * pCtx)
{
}

void i3Chara::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameObj::CopyTo( pDest, method);

	i3Chara * pChara = (i3Chara *) pDest;

	// Scene Graph
	if( getSceneGraphInfo() != nullptr)
	{
		pChara->setSceneGraphInfo( getSceneGraphInfo());
	}
}

void i3Chara::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( getSceneGraphInfo() != nullptr)
	{
		getSceneGraphInfo()->OnBuildObjectList( List);
	}

	i3GameObj::OnBuildObjectList( List);	
}

void i3Chara::_Rec_InitAIContext( i3GameNode * pThis)
{
	i3GameNode * pChild = pThis->getFirstChild();

	while( pChild != nullptr)
	{
		if( i3::same_of<i3AIContext* >(pChild))
		{
			i3AIContext * pCtx = (i3AIContext *) pChild;

			pCtx->setOwner( this);
		}

		_Rec_InitAIContext( pChild);

		pChild = pChild->getNext();
	}
}

void i3Chara::OnInitAfterLoad(void)
{
	_Rec_InitAIContext( this);
	
	i3GameObj::OnInitAfterLoad();
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 CHARACTER
	{
		UINT8		m_ID[4] = { 'C', 'H', 'R', '1' };
		OBJREF64	m_pSgInfo = 0;
		UINT32		m_CharaState = 0;

		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3Chara::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::CHARACTER data;

	Result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_pSgInfo = (OBJREF64) pResFile->GetObjectPersistID( getSceneGraphInfo());
	data.m_CharaState = getCharaState();

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3Chara::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::CHARACTER data;

	Result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pSgInfo != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo *) pResFile->FindObjectByID( (OBJREF) data.m_pSgInfo);

		if( pInfo != nullptr)
			setSceneGraphInfo( pInfo);
	}

	setCharaState( data.m_CharaState);

	return Result;
}

#if defined( I3_DEBUG)

void i3Chara::_CreateRenderableTransform( i3Transform * pAttachNode)
{
	char szName[ MAX_NAME];
	i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
	I3ASSERT( pAttrSet != nullptr);

	pAttrSet->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL | I3_NODEFLAG_TRANSPARENT | I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
	i3::snprintf( szName, sizeof( szName), "Debug_RT_%d", m_RenderableTransformList.size());
	pAttrSet->SetName( szName);

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set(false);
		pAttrSet->AddAttr( pAttr);
	}

	{
		COLOR color;
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

		i3Color::Set( &color, (UINT8)0, 255, 0, 255);
		pAttr->Set( &color);
		pAttrSet->AddAttr( pAttr);
	}
	
	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set(false);
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set(false);
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
		pAttr->setBias( 0.f);
		pAttrSet->AddAttr( pAttr);
	}

	i3Node * pMesh = i3SceneUtil::BuildBoxMesh( 1.0f, false);
	pAttrSet->AddChild( pMesh);
	I3_SAFE_RELEASE( pMesh);

	MATRIX mtx;
	i3Transform * pTrans = i3Transform::new_object_ref();
	
	i3Matrix::SetScale( &mtx, 0.01f, 0.01f, 0.01f);
	pTrans->SetMatrix( &mtx);
	pTrans->AddChild( pAttrSet);
	pTrans->AddFlag( I3_NODEFLAG_VOLATILE);

	pAttachNode->AddChild( pTrans);
	
	m_RenderableTransformList.push_back( pTrans);
}

void i3Chara::_FindAndCreateRenderableBone( i3Node * pRoot)
{
	INT32 i;

	// Transform을 찾는다.
	if( i3::kind_of<i3Transform*>(pRoot))
	{
		// 마지막 Child인가?
		if( pRoot->GetChildCount() == 0)
		{
			// 부모는 BoneRef인가?
			i3Node * pParent = pRoot->GetParent();
			while( pParent != nullptr)
			{
				if( i3::same_of<i3BoneRef* >(pParent))
				{
					// Create Renderable bone for transform
					_CreateRenderableTransform( (i3Transform*)pRoot);
					break;
				}

				pParent = pParent->GetParent();
			}
		}
	}

	
	for( i = 0; i < pRoot->GetChildCount(); i++)
	{
		_FindAndCreateRenderableBone( pRoot->GetChild( i));
	}
}

void i3Chara::CreateRenderableBoneRef( void)
{
	if( getSceneObject()->GetBody() == nullptr)
		return;

	m_RenderableTransformList.clear();

	_FindAndCreateRenderableBone( getSceneObject()->GetBody());
}

void i3Chara::CreateRenderableBone( void)
{
	INT32 i, j;

	if( getSceneObject()->GetBody() == nullptr)
		return;

	if( getSceneObject()->GetBody()->getFrameLOD() == nullptr)
		return;

	i3Skeleton * pFrameSkel = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
	I3ASSERT( pFrameSkel != nullptr);

	m_pRenderableBone = i3AttrSet::new_object_ref();
	I3ASSERT( m_pRenderableBone);

	m_pRenderableBone->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL | I3_NODEFLAG_TRANSPARENT | I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
	m_pRenderableBone->SetName( "Debug_RenderableBone");

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pRenderableBone->AddAttr( pAttr);
	}

	{
		COLOR color;
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

		i3Color::Set( &color, (UINT8)255, 255, 255, 255);
		pAttr->Set( &color);
		m_pRenderableBone->AddAttr( pAttr);
	}
	
	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pRenderableBone->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pRenderableBone->AddAttr( pAttr);
	}

	{
		i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
		pAttr->setBias( 0.f);
		m_pRenderableBone->AddAttr( pAttr);
	}
	
	m_iRenderableBoneCount = pFrameSkel->getBoneCount();
	
	for( i = 0; i < m_iRenderableBoneCount; i++)
	{
		REAL32 rMaxLen = 0.f;
		VEC3D vDir;

		for( j = 0; j < m_iRenderableBoneCount; j++)
		{
			if( i == j)
				continue;

			if( pFrameSkel->getBone( j)->getParentIndex() == i)
			{
				i3Bone * pChildBone = pFrameSkel->getBone( j);

				REAL32 rLen = i3Vector::Length( i3Matrix::GetPos( pChildBone->getMatrix()) );

				if( rMaxLen < rLen)
				{
					i3Vector::Copy( &vDir, i3Matrix::GetPos( pChildBone->getMatrix()));
					rMaxLen = rLen;
				}
			}
		}

		i3BoneRef * pBoneTrans = i3BoneRef::new_object_ref();
		I3ASSERT( pBoneTrans != nullptr);

		pBoneTrans->SetName( pFrameSkel->getBone( i)->getNameString());
		pBoneTrans->AddFlag( I3_NODEFLAG_VOLATILE);
		pBoneTrans->setBoneIndex( i);

		CreateRenderDiamond( pBoneTrans, &vDir, (rMaxLen == 0.f));

		m_pRenderableBone->AddChild( pBoneTrans);
	}

	getSceneObject()->GetBody()->AddChild( m_pRenderableBone);
}

void i3Chara::CreateRenderDiamond( i3Node * pAttachNode, VEC3D * pDir, bool bLeaf)
{
	char szNodeName[ MAX_NAME];
	VEC3D vPos;
	COLORREAL color;
	i3VertexFormat format;

	format.SetHasPosition( true);
	format.SetHasColor( true);

	{
		// Geometry
		i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();

		i3VertexArray * pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, 2, 0);

		{
			pVA->Lock();

			i3Vector::Set( &vPos, 0.f, 0.f, 0.f);
			pVA->SetPosition( 0, &vPos);
			
			if( bLeaf )
			{
				i3Vector::Set( &vPos, 0.05f, 0.f, 0.f);
				i3Color::Set( &color, 0.5f, 0.5f, 0.8f, 0.8f);
			}
			else
			{
				i3Vector::Copy( &vPos, pDir);
				i3Color::Set( &color, 0.5f, 0.5f, 0.5f, 0.8f);
			}

			pVA->SetPosition( 1, &vPos);
			pVA->SetColor( 0, &color);
			pVA->SetColor( 1, &color);

			pVA->Unlock();
		}

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0, false );

		i3Geometry * pGeo = i3Geometry::new_object_ref();

		i3::string_ncopy_nullpad( szNodeName, "renderable_Bone\0", MAX_NAME);

		pGeo->SetName( szNodeName);

		pGeo->AppendGeometryAttr( pGeoAttr);

		pAttachNode->AddChild( pGeo);

		{
			i3Transform * pTransScale = i3Transform::new_object_ref();
			i3Matrix::Identity( pTransScale->GetMatrix());
			i3Matrix::SetScale( pTransScale->GetMatrix(), 0.015f, 0.015f, 0.015f);
			i3Color::Set( &color, 0.9f, 0.3f, 0.3f, 0.8f);

			i3Node * pSphereMesh = i3SceneUtil::BuildSphereMesh( 1.0f, true, true, 6, 3, &color, &color, &color);

			pTransScale->AddChild( pSphereMesh);
			pTransScale->AddFlag( I3_NODEFLAG_VOLATILE);

			I3_SAFE_RELEASE( pSphereMesh);

			pAttachNode->AddChild( pTransScale);
		}
	}

}


void i3Chara::EnableRenderBone( bool bEnable)
{
	
	if( m_pRenderableBone == nullptr)
	{
		CreateRenderableBoneRef();
		CreateRenderableBone();
	}

	if( bEnable)
	{
		m_pRenderableBone->RemoveFlag( I3_NODEFLAG_DISABLE);
		for(size_t i = 0; i < m_RenderableTransformList.size(); i++)
		{
			i3Node * pNode = m_RenderableTransformList[ i];
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
		}
	}
	else
	{
		m_pRenderableBone->AddFlag( I3_NODEFLAG_DISABLE);
		for(size_t i = 0; i < m_RenderableTransformList.size(); i++)
		{
			i3Node * pNode = m_RenderableTransformList[ i];
			pNode->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}

	m_bEnableRenderBone = bEnable;
}


#endif
