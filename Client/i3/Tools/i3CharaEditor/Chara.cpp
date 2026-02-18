#include "stdafx.h"
#include "Chara.h"
#include "GlobalVar.h"

#include "i3Base/itl/vector_util.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/ext/extract_filename.h"

#include "i3Base/i3XMLFile.h"
I3_CLASS_INSTANCE( CChara); //, i3ResourceObject);

CChara::CChara(void)
{
	m_pChara = i3Chara::new_object();

	m_pBody = i3Body::new_object();
	m_pBody->AddFlag( I3_NODEFLAG_DISABLE);

	m_pRenderCollRoot = NULL;
	m_pRotateAxis = NULL;

	{
		i3AttrSet *		pAttrSet = i3AttrSet::new_object_ref();

		{
			pAttrSet->SetPriority( 0);

			{
				m_pRenderModeAttr = i3RenderModeAttr::new_object_ref();
				m_pRenderModeAttr->Set( I3G_RENDER_POLYGON);

				pAttrSet->AddAttr( m_pRenderModeAttr);
			}
			m_pChara->getAttachNode()->AddChild( pAttrSet);
		}
		
		{
			MATRIX mtx;

			m_pRotateAxis = i3Transform::new_object();
			pAttrSet->AddChild( m_pRotateAxis);
			m_pRotateAxis->AddFlag( I3_NODEFLAG_CONTROL);

			m_pRotateAxis->SetName( "AxisRotate");
			i3Matrix::SetRight(		&mtx,	-1.0f,	0.0f,	0.0f,	0.0f);
			i3Matrix::SetUp(		&mtx,	0.0f,	0.0f,	1.0f,	0.0f);
			i3Matrix::SetAt(		&mtx,	0.0f,	1.0f,	0.0f,	0.0f);
			i3Matrix::SetPos(		&mtx,	0.0f,	0.0f,	0.0f,	1.0f);

			m_pRotateAxis->SetMatrix( &mtx);

			{
				i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
				
				pInfo->setInstanceSg( m_pRotateAxis);
				pInfo->SetExternRes( TRUE);
				m_pChara->setSceneGraphInfo( pInfo);
			}

			m_pRotateAxis->AddChild( m_pBody);
		}

		/*
		{
			i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
			
			pInfo->setInstanceSg( m_pBody);
			pInfo->SetExternRes( TRUE);
			m_pChara->setSceneGraphInfo( pInfo);
		}
		pAttrSet->AddChild( m_pBody);
		*/
	}

	/*
	{
		m_pRenderCollRoot = i3Node::new_object_ref();
		m_pRenderCollRoot->SetName( "Rendera Collider Root");
		m_pRenderCollRoot->AddFlag( I3_NODEFLAG_VOLATILE);

		m_pBody->AddChild( m_pRenderCollRoot);
	}
	*/

	m_Slope = m_pBody->getSlope();
	m_Const = m_pBody->getConst();

	memset(m_FilePath,0,sizeof(m_FilePath));
}

CChara::~CChara(void)
{
	RemoveAllSceneGraph();
	RemoveAllLOD( true);
	RemoveAllAnim();
	RemoveAllSound();
	RemoveAllAI();

	I3_SAFE_RELEASE( m_pChara);
	I3_SAFE_RELEASE( m_pBody);
	I3_SAFE_RELEASE( m_pRotateAxis);
}

void CChara::setSlope( REAL32 fVal)
{
	m_Slope = fVal;
	m_pBody->setSlope( fVal);
}

void CChara::setConst( REAL32 fVal)
{
	m_Const = fVal;
	m_pBody->setConst( fVal);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
void	CChara::AddSceneGraph( i3SceneGraphInfo * pInfo)
{
	I3ASSERT( pInfo != NULL);

	pInfo->AddRef();

	m_SgList.push_back( pInfo);

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pInfo);
}

void	CChara::RemoveSceneGraph( i3SceneGraphInfo * pInfo)
{
	INT32 idx;

	idx = FindSceneGraph( pInfo);
	I3ASSERT( idx != -1);

	RemoveSceneGraph( idx);
}

void	CChara::RemoveSceneGraph( INT32 idx)
{
	I3ASSERT( idx < (INT32)m_SgList.size());

	i3SceneGraphInfo * pInfo = (i3SceneGraphInfo *) getSceneGraph( idx);

	// 해당 Scene-Graph를 사용하는 LOD도 제거해야 한다.
	{
		i3::vector<CLOD*> List;
		

		FindLODBySg( pInfo, List);

		for(size_t i = 0; i < List.size(); i++)
		{
			CLOD * pLOD = List[i];
			RemoveLOD( pLOD);						
		}
				
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pInfo);

	pInfo->Release();

	i3::vu::erase_index(m_SgList, idx);	
}

void	CChara::RemoveAllSceneGraph(void)
{
	INT32 i = 0;
	//i3SceneGraphInfo * pInfo;

	//for( i = 0; i < getSceneGraphCount(); i++)
	while( getSceneGraphCount() > 0)
	{
		RemoveSceneGraph( i);

		//pInfo = getSceneGraph( i);
		//pInfo->Release();
	}

	m_SgList.clear();

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, i3SceneGraphInfo::static_meta());
}

INT32	CChara::FindSceneGraph( i3SceneGraphInfo * pInfo)
{
	return i3::vu::int_index_of(m_SgList, pInfo);
}

INT32	CChara::FindSceneGraphByPath( const char * pszPath)
{
	INT32 i;
	i3SceneGraphInfo * pInfo;

	for( i = 0; i < getSceneGraphCount(); i++)
	{
		pInfo = getSceneGraph( i);

		if(i3::generic_is_iequal( pszPath, pInfo->GetName()) )
			return i;
	}

	return -1;
}

bool CChara::LoadSceneGraph( const char * pszPath)
{
	i3SceneFile file;
	i3SceneGraphInfo * pInfo;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		return false;
	}

	pInfo = file.getSceneGraphInfo();
	if( pInfo == NULL)
	{
		return false;
	}

	// 이름을 지정...
	// 항상 Working Folder를 기준으로 한, Relative Path가
	// 이름으로 지정되어야 한다.
	{
		char szRelPath[ MAX_PATH];

		i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szRelPath );
		i3::make_unix_path( szRelPath);

		pInfo->SetName( szRelPath);
	}

	pInfo->SetExternRes( TRUE);

	AddSceneGraph( pInfo);

	// 이에 해당하는 LOD를 자동으로 추가한다.
	{
		AutoGenLOD( pInfo);
	}

	return true;
}


void CChara::AutoGenLOD( i3SceneGraphInfo * pInfo)
{
	INT32 i;
	CLOD * pLOD;

	i3Body * pBody = (i3Body *) i3Scene::FindNodeByType( pInfo->getInstanceSg(), i3Body::static_meta(), true);

	if( pBody == NULL)
	{
		pLOD = CLOD::new_object_ref();
		pLOD->SetName( pInfo->GetName());

		pLOD->SetSceneGraph( pInfo, 0, true);
		AddLOD( pLOD);
	}
	else
	{
		for( i = 0; i < (INT32) pBody->getLODCount(); i++)
		{
			pLOD = CLOD::new_object_ref();
			pLOD->SetName( pInfo->GetName());

			pLOD->SetSceneGraph( pInfo, i, true);
			AddLOD( pLOD);
		}
	}
}

void CChara::_OptBoneRef(void)
{
	INT32 boneIdx;
	CLOD * pLOD;
	i3BoneRef * pBoneRef;
	char conv[256];
	i3::vector<i3Node*>	List;
	i3::vector<i3BoneRef*> BoneRefList;

	// 기존 LOD에 붙어 있던 i3BoneRef Node를 검색한다.
	for(INT32 i = 0; i < getLODCount(); i++)
	{
		pLOD = getLOD( i);

		List.clear();
		i3Scene::FindNodeByExactType( pLOD->getLOD()->getShapeNode(), i3BoneRef::static_meta(), List);

		for(size_t j = 0; j < List.size(); j++)
		{
			pBoneRef = (i3BoneRef *) List[j];

			if( pBoneRef->getBoneIndex() != -1)
			{
				i3Bone * pBone;

				i3Skeleton * pSkel = pLOD->getLOD()->getSkeleton();

				if( pBoneRef->getBoneIndex() >= pSkel->getBoneCount() )
				{
					pBoneRef->setBoneIndex( -1);
					continue;
				}

				pBone = pSkel->getBone( pBoneRef->getBoneIndex());

				boneIdx = m_pBody->FindBoneByName( pBone->getName() );
				if( boneIdx == -1)
				{
					boneIdx = m_pBody->FindBoneByName( pBoneRef->GetName());
					I3ASSERT( boneIdx != -1 );
					pBone = pSkel->getBone( boneIdx );
				}

				pBoneRef->setBoneIndex( boneIdx);

				if( pBoneRef->hasName() == false)
				{
					sprintf( conv, "BoneRef(%s)", pBone->getName());
					pBoneRef->SetName( conv);
				}

				// 해당 본에 어테치되는 본이 중계해주는 Transform이 있어야 합니다. 정순구
				// 예) Character의 HandDummy + Weapon 				
				if( pBoneRef->GetChildCount() == 0 )
				{
					i3Transform * pTrans = i3Transform::new_object_ref();
					pTrans->SetName( pBone->getName() );
					pTrans->SetMatrix( pBone->getMatrix() );
					pTrans->SetFlag( I3_NODEFLAG_STATIC );
					pBoneRef->AddChild( pTrans );
				}
				
			}

			// BoneRef 아래에 Geometry가 있는지 확인한다.
			// 만약 있다면, Non-Skinning Geometry로 간주하고, 이동 대상에서 제외시킨다.
			{
				i3Geometry * pGeo;

				pGeo = (i3Geometry *) i3Scene::FindNodeByType( pBoneRef, i3Geometry::static_meta());

				if( pGeo == NULL)
				{
					BoneRefList.push_back( pBoneRef);
				}
			}
		}
	}

	// 검색된 i3BoneRef들을 원래 있던 자리에서 제거한다.
	for(size_t i = 0; i < BoneRefList.size(); i++)
	{
		pBoneRef = BoneRefList[i];

		pBoneRef->AddRef();			// free되지 않도록 하기 위한 조치
		pBoneRef->RemoveFromParent();

		m_pBody->AddChild( pBoneRef);
		pBoneRef->Release();
	}

	// pBoneRef동일한 Bone Index를 가진 것이 있는지 확인하고, 있다면 없앤다.
	for(INT32 i = 0; i < m_pBody->GetChildCount(); i++)
	{
		i3Node * pChild = m_pBody->GetChild( i);

		if( !i3::kind_of<i3BoneRef*>(pChild)) //->IsTypeOf( i3BoneRef::static_meta()) == FALSE)
			continue;

		pBoneRef = (i3BoneRef *) pChild;

		for(INT32 j = i + 1; j < m_pBody->GetChildCount(); j++)
		{
			i3Node * pChild2 = m_pBody->GetChild( j);

			if( !i3::kind_of<i3BoneRef*>(pChild2)) //->IsTypeOf( i3BoneRef::static_meta()) == FALSE)
				continue;
			
			i3BoneRef * pBoneRef2 = (i3BoneRef *) pChild2;
			
			if( i3::generic_is_iequal( pBoneRef->GetName(), pBoneRef2->GetName()) &&
				(pBoneRef->getBoneIndex() == pBoneRef2->getBoneIndex()))
			{
				if( pBoneRef->GetChildCount() == 0 )
				{
					I3TRACE( "Remove BoneRef (%d, %s)\n", pBoneRef->getBoneIndex(), pBoneRef->GetName());

					m_pBody->RemoveChild( pBoneRef );

					break;
				}

				I3TRACE( "Remove BoneRef (%d, %s)\n", pBoneRef2->getBoneIndex(), pBoneRef2->GetName());

				m_pBody->RemoveChild( pBoneRef2);
				j--;
			}
		}
	}
}

//
// 리빌드 과정 중 동일한 메모리 데이터를 가진 이미지를 합치는 옵티마이저가 디폴트로 작동...
// 이 부분은 텍스쳐 교환 모듈 편집에서는 오히려 방해가 되기 때문에 끄거나, 작동방식을 조금 변경해야할 것임. (2015.04.28.수빈)
//

void CChara::RebuildLOD(void)
{
	INT32 i;
	i3Skeleton * pSkel = NULL;

	m_pBody->RemoveAllLOD();
	m_pBody->RemoveAllChild();

	//SortLOD();

	for( i = 0; i < getLODCount(); i++)
	{
		CLOD * pLOD = getLOD( i);

		m_pBody->AddLOD( pLOD->getLOD());

#if defined( I3_DEBUG )
		pSkel = pLOD->getLOD()->getSkeleton();
		if( pSkel == NULL )
			continue;
		I3TRACE( "%d\n", pSkel->isShapeSet(1));
#endif
	}

	if( getLODCount() == 0)
	{
		m_pBody->AddFlag( I3_NODEFLAG_DISABLE);
	}
	else
	{
		m_pBody->RemoveFlag( I3_NODEFLAG_DISABLE);
	}

	m_pBody->Prepare();

#if defined( I3_DEBUG )
	if( getLODCount() >= 2)
	{
		CLOD * pLOD = getLOD( 1);
		pSkel = pLOD->getLOD()->getSkeleton();
		if( pSkel != NULL )
		{
			I3TRACE( "1:%d\n", pSkel->isShapeSet(1));
		}
	}
#endif

	// 중복되는 i3BoneRef가 있을 수 있다.
	_OptBoneRef();

#if defined( I3_DEBUG )
	if( getLODCount() >= 2)
	{
		CLOD * pLOD = getLOD( 1);

		pSkel = pLOD->getLOD()->getSkeleton();
		if( pSkel != NULL )
		{
			I3TRACE( "2:%d\n", pSkel->isShapeSet(1));
		}
	}
#endif

	m_pChara->getSceneObject()->BuildBoneList();

	{
		i3BoundBox * pBox = i3BoundBox::new_object_ref();

		m_pBody->GetWrappingBound( pBox);

		m_pBody->SetBound( pBox);
	}

#if defined( I3_DEBUG )
	if( getLODCount() >= 2)
	{
		CLOD * pLOD = getLOD( 1);

		pSkel = pLOD->getLOD()->getSkeleton();
		if( pSkel != NULL )
		{
			I3TRACE( "3:%d\n", pSkel->isShapeSet(1));
		}
	}
#endif

	// LOD 간 Animation을 공유시키기 위한 Bone Map을 생성한다.
	{
		i3OptBuildLODBoneMap opt;

		opt.Trace( m_pBody);
	}

#if defined( I3_DEBUG )
	if( getLODCount() >= 2)
	{
		CLOD * pLOD = getLOD( 1);

		pSkel = pLOD->getLOD()->getSkeleton();
		if( pSkel != NULL )
		{
			I3TRACE( "4:%d\n", pSkel->isShapeSet(1));
		}
	}
#endif

//	// 동일 Texture의 공유		( 이 부분은 Export시 옵션으로 일단 뺀다.. 하지만, 비가역적으로 동작하므로
//	{								한번 동작하면 원상 회복되지 못할 것이다.. 2015.04.28.수빈)
//		i3OptShareTexture opt;
//
//		opt.Trace( m_pBody);
//	}

	//	PhysX를 off (on시에 애니메이션 되지 않는다)
	if( getLODCount() > 0)
	{
		m_pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
		m_pChara->setDynamicActiveState( false);
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_SCENE, this);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
void	CChara::AddLOD( CLOD * pLOD)
{
	I3ASSERT( pLOD != NULL);

	pLOD->AddRef();

	m_LODList.push_back( pLOD);

	RebuildLOD();

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pLOD);
}

void	CChara::RemoveLOD( CLOD * pLOD)
{
	INT32 idx;

	idx = FindLOD( pLOD);
	I3ASSERT( idx != -1);

	RemoveLOD( idx);
}

void	CChara::RemoveLOD( INT32 idx, bool bFromDestruct)
{
	I3ASSERT( idx < (INT32)m_LODList.size());

	CLOD * pLOD = (CLOD *) getLOD( idx);

	i3::vu::erase_index(m_LODList,idx);		//.Delete( idx);	

	if( bFromDestruct == false)
	{
		RebuildLOD();

		i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pLOD);
	}

	pLOD->Release();
}

void	CChara::RemoveAllLOD( bool bFromDestruct)
{
	INT32 i = 0;
	//CLOD * pLOD;

	//for( i = 0; i < getLODCount(); i++)
	while( getLODCount() > 0)
	{
		RemoveLOD( i, bFromDestruct);
	}

	m_LODList.clear();

	//RebuildLOD();

	if( bFromDestruct == false)
	{
		i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, CLOD::static_meta());
	}
}

INT32	CChara::FindLOD( CLOD * pLOD)
{
	return i3::vu::int_index_of(m_LODList, pLOD);
}

INT32	CChara::FindLODByPath( const char * pszPath)
{
	INT32 i;
	CLOD * pLOD;

	for( i = 0; i < getLODCount(); i++)
	{
		pLOD = getLOD( i);

		if(i3::generic_is_iequal( pszPath, pLOD->GetName()) )
			return i;
	}

	return -1;
}

void CChara::FindLODBySg( i3SceneGraphInfo * pInfo, i3::vector<CLOD*>& List)
{
	INT32 i;
	CLOD * pLOD;

	for( i = 0; i < getLODCount(); i++)
	{
		pLOD = getLOD( i);

		if( pLOD->getSceneGraph() == pInfo)
			List.push_back( pLOD);
	}
}

void CChara::SortLOD(void)
{
	INT32 i, j;
	CLOD * pLOD1, * pLOD2;

	for( i = 0; i < getLODCount() - 1; i++)
	{
		pLOD1 = getLOD( i);

		for( j = 1 + 1; j < getLODCount(); j++)
		{
			pLOD2 = getLOD( j);

			if( pLOD1->getTriCount() < pLOD2->getTriCount())
			{
				m_LODList[i] = pLOD2;
				m_LODList[j] = pLOD1;

				pLOD1 = pLOD2;
			}
		}
	}

	// Sg 정리
	m_pBody->RemoveAllLOD();

	for( i = 0; i < getLODCount(); i++)
	{
		pLOD1 = getLOD( i);

		m_pBody->AddLOD( pLOD1->getLOD());
	}
}

void CChara::MoveLOD( INT32 idx, INT32 off)
{
	INT32 idx2;
	
	idx2 = idx + off;
	if( idx2 < 0)
		idx2 = 0;
	if( idx2 > (INT32)m_LODList.size() - 1)
		idx2 = (INT32)m_LODList.size() - 1;

	if( idx2 == idx)
		return;

	CLOD* pTemp = m_LODList[idx];
	m_LODList[idx] = m_LODList[idx2];
	m_LODList[idx2] = pTemp;
}


void CChara :: SetFilePath(char* Path)
{
	strcpy(m_FilePath,Path);
}
const char* CChara :: GetFilePath()
{
	return m_FilePath;
}
/////////////////////////////////////////////////////////////////////////////////////////
//
void	CChara::AddSound( i3Sound * pSound)
{
	I3ASSERT( pSound != NULL);

	if( FindSoundByPath( pSound->GetName()) > -1)
	{
		CharaEditorNotifyBox( NULL, "The sound file is already exist (%s)\n", pSound->GetName());
		return;
	}

	pSound->AddRef();

	m_SoundList.push_back( pSound);

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pSound);
}

void	CChara::RemoveSound( i3Sound * pSound)
{
	INT32 idx;

	idx = FindSound( pSound);
	I3ASSERT( idx != -1);

	RemoveSound( idx);
}

void	CChara::RemoveSound( INT32 idx)
{
	I3ASSERT( idx < (INT32)m_SoundList.size());

	i3Sound * pSound = (i3Sound *) getSound( idx);

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pSound);

	pSound->Release();
	i3::vu::erase_index(m_SoundList, idx);	
}

void	CChara::RemoveAllSound(void)
{
	INT32 i;
	i3Sound * pSound;

	for( i = 0; i < getSoundCount(); i++)
	{
		pSound = getSound( i);

		pSound->Release();
	}

	m_SoundList.clear();

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, i3Sound::static_meta());
}

INT32	CChara::FindSound( i3Sound * pSound)
{
	return i3::vu::int_index_of(m_SoundList, pSound);
}

INT32	CChara::FindSoundByPath( const char * pszPath)
{
	INT32 i;

	for( i = 0; i < getSoundCount(); i++)
	{
		i3Sound * pSnd;

		pSnd = getSound(i);

		if( i3::generic_is_iequal( pszPath, pSnd->GetName()) )
			return i;
	}

	return -1;
}

bool CChara::LoadSound( const char * pszPath)
{
	// 이름을 지정...
	// 항상 Working Folder를 기준으로 한, Relative Path가
	// 이름으로 지정되어야 한다.
	i3Sound * pSound;
	i3SoundFile file;

	pSound = file.Load( pszPath);
	if( pSound == NULL)
	{
		I3PRINTLOG(I3LOG_WARN, "%s Sound 파일을 읽어 들일 수 없습니다.", pszPath);
		return false;
	}

	pSound->SetExternRes( TRUE);

	{
		char szRelPath[ MAX_PATH];

		i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szRelPath );
		i3::make_unix_path( szRelPath);

		pSound->SetName( szRelPath);
	}

	AddSound( pSound);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
void	CChara::AddAI( AI * pParentAI, AI * pAI, bool bAddToParent)
{
	i3AI * pi3AI;

	I3ASSERT( pAI != NULL);
	pAI->AddRef();

	m_AIList.push_back( pAI);

	if( pAI->getAIContext() == NULL)
	{
		pi3AI = i3AI::new_object_ref();
		pi3AI->SetName( pAI->GetName());

		pAI->SetAI( pi3AI);
	}

	pAI->getAIContext()->setOwner( m_pChara);
	pAI->getAIContext()->SetName( pAI->GetName());

	if( bAddToParent)
	{
		if( pParentAI == NULL)
		{
			// Character에 직접 붙인다.
			m_pChara->AddChild( pAI->getAIContext());
		}
		else
		{
			// 다른 AI Context의 Child로 붙인다.
			pParentAI->getAIContext()->AddChild( pAI->getAIContext());
		}
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pAI);
}

void	CChara::CMP_Remove( AI * pAI)
{
//	i3AI * pi3AI;

	I3ASSERT( pAI != NULL);
	
	for(INT32 i = 0; i< INT32(m_AIList.size()) ; i++)
	{
		AI* _ai = m_AIList.at(i); 
		if( 0 == strcmp( _ai->GetName(), pAI->GetName()))
		{
			RemoveAI(i);	--i;		// 루프가 이상한 상태로 일단 이렇게 교정..
		}

	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pAI);
}

void	CChara::FullPath_Remove(const char *FullPath)
{
	//	i3AI * pi3AI;
	i3::string path = FullPath;
	i3::string path2;
	size_t _npos1 = path.find("/");
	path.assign(path, _npos1, path.size());


	for (INT32 i = 0; i < INT32(m_AIList.size()); i++)
	{
		char CurFullPath[256] = { 0, };
		AI* _ai = m_AIList.at(i);
		_ai->GetFullAIPath(CurFullPath, true);

		path2 = CurFullPath;
		_npos1 = path2.find("/");
		path2.assign(path2, _npos1, path2.size());

		if (0 == strcmp(path.c_str(), path2.c_str()))
		{
			RemoveAI(i);	--i;		// 루프가 이상한 상태로 일단 이렇게 교정..
		}

	}

	//i3TDK::Update(NULL, I3_TDK_UPDATE_ADD, pAI);
}

void	CChara::RemoveAI( AI * pAI)
{
	INT32 idx;

	idx = FindAI( pAI);
	I3ASSERT( idx != -1);

	RemoveAI( idx);
}

void	CChara::_GetAllDescendantAI( i3AIContext * pAICtx, i3::vector<AI*>& List)
{
	i3GameNode * pTemp;

	pTemp = pAICtx->getFirstChild();

	while( pTemp != NULL)
	{
		if( i3::kind_of<i3AIContext*>(pTemp)) //->IsTypeOf( i3AIContext::static_meta()))
		{
			INT32 idx = FindAIByAIContext( (i3AIContext *) pTemp);
			I3ASSERT( idx != -1);

			AI * pAI = getAI( idx);

			I3ASSERT( pAI->getAIContext() != NULL);

			_GetAllDescendantAI( pAI->getAIContext(), List);

			List.push_back( pAI);
		}

		pTemp = pTemp->getNext();
	}
}

void	CChara::RemoveAI( INT32 idx)
{
	i3::vector<AI*>	list;
	AI * pTempAI;
	I3ASSERT( idx < (INT32)m_AIList.size());

	AI * pAI = (AI *) getAI( idx);

	if( pAI->getAI() != NULL)
	{
		i3GameNode * pParent = pAI->getAIContext()->getParent();

		if( pParent != NULL)
			pParent->RemoveChild( pAI->getAIContext());
	}

	// pAI 뿐만 아니라, 모든 Child AI들을 제거해 주어야 하기 때문에...
	{
		_GetAllDescendantAI( pAI->getAIContext(), list);

		list.push_back( pAI);

		for(size_t i = 0; i < list.size(); i++)
		{
			pTempAI = list[i];

			i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pTempAI);

			pTempAI->Release();
			i3::vu::remove(m_AIList, pTempAI);
		}
	}
}

void	CChara::RemoveAllAI(void)
{
	INT32 i;
	AI * pAI;

	for( i = 0; i < getAICount(); i++)
	{
		pAI = getAI( i);

		pAI->Release();
	}

	m_AIList.clear();

	m_pChara->RemoveAllChild();

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, AI::static_meta());
}

INT32	CChara::FindAI( AI * pAI)
{
	return i3::vu::int_index_of(m_AIList, pAI);
}

INT32	CChara::FindAIByPath( const char * pszPath)
{
	INT32 i;
	AI * pAIInfo;

	for( i = 0; i < getAICount(); i++)
	{
		pAIInfo = getAI( i);

		if(i3::generic_is_iequal( pszPath, pAIInfo->GetName()) )
			return i;
	}

	return -1;
}

INT32	CChara::FindAIByAIContext( i3AIContext * pCtx)
{
	INT32 i;
	AI * pAIInfo;

	for( i = 0; i < getAICount(); i++)
	{
		pAIInfo = getAI( i);

		if( pAIInfo->getAIContext() == pCtx)
			return i;
	}

	return -1;
}

bool CChara::LoadAI( const char * pszPath)
{
	// 이름을 지정...
	// 항상 Working Folder를 기준으로 한, Relative Path가
	// 이름으로 지정되어야 한다.
	char szRelPath[ MAX_PATH];
	AI * pAI;

	{
		i3ResourceFile file;

		if( file.Load( pszPath) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not read %s\n", pszPath);
			return false;
		}

		pAI = (AI *) file.getKeyObject();
		if( !i3::kind_of<AI*>(pAI)) //->IsTypeOf( AI::static_meta()) )
		{
			I3PRINTLOG(I3LOG_WARN, "%s 파일은 올바른 AI 파일이 아닙니다.", pszPath);
			return false;
		}
	

		i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szRelPath );
		i3::make_unix_path( szRelPath);

		pAI->SetName( szRelPath);

		AddAI( NULL, pAI);
	}
	return true;
}

//#define TEST_POS

/////////////////////////////////////////////////////////////////////////////////////////
//
void CChara::_AppendCollider( INT32 boneIdx, i3Geometry * pGeo, MATRIX * pMtx)
{
	i3BoneRef * pBoneRef;
	i3ColliderSet * pCollSet;
	i3Collider * pCollider;
	i3CollideeCapsule * pColl;
	i3GeometryAttr * pGeoAttr;
	VEC3D vZero;
#if defined( TEST_POS)
	i3Collider * pColliderSphere;
	i3CollideeSphere *	pCollSphere;
#endif

	if( pGeo->GetGeometryAttrCount() <= 0)
		return;

	i3Vector::Set( &vZero, 0.0f, 0.0f, 0.0f);

	pGeoAttr = (i3GeometryAttr *) pGeo->GetGeometryAttr( 0);

	{
		pBoneRef = i3BoneRef::new_object_ref();
		pBoneRef->setBoneIndex( boneIdx);
	}

	{
		pCollSet = i3ColliderSet::new_object_ref();
		pCollSet->SetName( pGeo->GetName());
		pBoneRef->AddChild( pCollSet);
	}

	{
		pCollider = i3Collider::new_object_ref();
		pCollSet->AddCollider( pCollider);
	}

	{
		pColl = i3CollideeCapsule::new_object_ref();
		pCollider->SetCollidee( pColl);

		pColl->setStart( &vZero);
	}

#if defined( TEST_POS)
	{
		pCollSphere = i3CollideeSphere::new_object_ref();
		pCollSphere->SetCenter( &vZero);
		pCollSphere->SetRadius( 1.1f);

		pColliderSphere = i3Collider::new_object_ref();
		pColliderSphere->SetCollidee( pCollSphere);
		pCollSet->AddCollider( pColliderSphere);
		
	}
#endif

	// Capsule의 방향은 Z 축으로 한다.
	{
		VEC3D dir;
		REAL32 len;

		len = i3Vector::Normalize( &dir, i3Matrix::GetAt( pMtx));

		#if 1
			pColl->setDirection( &dir);
		#else
			VEC3D vtmp;

			i3Vector::Scale( &vtmp, &dir, -1.0f);
			pColl->setDirection( &vtmp);
		#endif

		// Length
		{
			I3TRACE( "%s - %f\n", pGeo->GetName(), len);

			pColl->setLength( len * 0.1f);
		}

		// Radius
		{
			VEC3D right;

			i3Vector::Copy( &right, i3Matrix::GetRight( pMtx));
			pColl->setRadius( i3Vector::Length( &right) * 0.1f * 0.5f);
		}
	}

	#if defined( TEST_POS)
	pCollSphere->SetRadius( 0.02f);
	#endif
}

void CChara::BuildCollider( const char * pszPath)
{
	INT32 boneIdx;
	i3Node * pLoadedSg;
	i3::vector<i3Node*> GeoList;

	{
		i3SceneFile file;

		if( file.Load( pszPath) == STREAM_ERR)
		{
			return;
		}

		pLoadedSg = file.GetSceneGraph();
		pLoadedSg->AddRef();
	}

	// i3Geometry Node를 검색한다.
	{
		i3Scene::FindNodeByExactType( pLoadedSg, i3Geometry::static_meta(), GeoList);

		for(size_t i = 0; i < GeoList.size(); i++)
		{
			i3Node * pParent;
			i3Geometry * pGeo = (i3Geometry *) GeoList[i];
			MATRIX mtx;

			i3Matrix::Identity( &mtx);

			// Attach된 Bone을 찾는다.
			// 이때, Attach Bone보다 하위에 있는 Transform도 추적해둔다.
			for( pParent = pGeo->GetParent(); pParent != NULL; pParent = pParent->GetParent())
			{
				if( pParent->hasName() == false)
					continue;

				if( i3::kind_of<i3Transform*>(pParent)) //->IsTypeOf( i3Transform::static_meta()))
				{
					boneIdx = m_pBody->FindBoneByName( pParent->GetName());
					if( boneIdx != -1)
					{
						// Attach Bone을 찾았다.
						_AppendCollider( boneIdx, pGeo, &mtx);
						break;
					}
					else
					{
						i3Transform * pTrans = (i3Transform *) pParent;

						i3Matrix::Mul( &mtx, &mtx, pTrans->GetMatrix());
					}
				}
			}

		}
	}

	pLoadedSg->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
bool CChara::Export( const char * pszPath, const char * pszI3SName, char* resultMsg)
{
	char szMessage[256] = "";
	char szPath[MAX_PATH];
	char szRelPath[MAX_PATH], szFull[MAX_PATH];
	
	// 저장되는 AI에 I3_GAMENODE_STATE_LOCKED 상태가 있으면 안되기 때문에...
	{
		for(INT32 i = 0; i < getAICount(); i++)
		{
			AI * pAI = getAI( i);

			pAI->getAIContext()->removeGNodeState( I3_GAMENODE_STATE_LOCKED);
		}
	}

	// Optimize Animation 
	if( g_pOption->m_bOptAnim && getAnimCount() > 0)
	{
		OptimizeAnim();
	}

	//	Generate AnimPack
	if( g_pOption->m_bGetAnimPack && getAnimCount() > 0)
	{
		i3::change_file_ext_copy(szPath, pszPath, "i3AnimPack");

		BuildAnimPackFile(szPath);
	}


	// 이미지 머징은 Export시 옵션으로 변경.. 하지만, 비가역적으로 동작하므로
	//				한번 동작하면 원상 회복되지 못할 것이다.. 2015.04.28.수빈)
	if (g_pOption->m_bRemoveSameImage)
	{
		i3OptShareTexture opt;
		opt.Trace( m_pBody);
	}

	// i3SceneGraphInfo에 적절한 이름을 기록한다.
	{
		i3::extract_directoryname(pszPath, szPath); 
		i3::make_relative_path( g_pOption->m_szWorkPath, szPath, szRelPath );

		sprintf( szFull, "%s\\%s.I3S", szRelPath, pszI3SName);

		m_pChara->getSceneGraphInfo()->SetName( szFull);
	
		// I3S의 저장
		if( g_pOption->m_bExportI3S)
		{
			i3Node * pOldSg = m_pChara->getSceneGraphInfo()->getInstanceSg();

			pOldSg->AddRef();

			if( g_pOption->m_bRotateAxis == FALSE)
			{
				m_pChara->getSceneGraphInfo()->setInstanceSg( m_pBody);
			}
			else
			{
				m_pChara->getSceneGraphInfo()->setInstanceSg( m_pRotateAxis);
			}


			// Animation 제거
			{
				i3OptRemoveAnim optAnim;
				optAnim.Trace( m_pChara->getSceneGraphInfo()->getInstanceSg());
			}

			// Vertex Color 제거
			{
				i3OptRemoveVertexComponent opt;
				opt.SetRemoveColor( TRUE);
				opt.Trace( m_pChara->getSceneGraphInfo()->getInstanceSg());
			}

			// Attr optimize
			{
				i3OptRenAttr optAttr;
				optAttr.setSg( m_pChara->getSceneGraphInfo());
				optAttr.Trace( m_pChara->getSceneGraphInfo()->getInstanceSg());
			}

			// 바운딩 박스 구하는 루틴을 이쪽으로 옮김... 현재 i3OptRenAttr의 옵티마이즈시 바운드박스까지 최적화되는 옵션이 추가되버린 상태.( 그리고 이 바운드박스 최적화옵션에는 버그가 존재하는데...버그성구현이 의도적으로 처리되는 부분이 염려되어..순서만 변경하는 것으로 일단락 )
			// 2015.09.21.수빈..
			// 바운드 박스 설정
			if (m_pBody->GetBound() == NULL)
			{
				i3BoundBox * pBox = i3BoundBox::new_object_ref();
				m_pBody->GetWrappingBound(pBox);
				m_pBody->SetBound(pBox);
			}
			
			// AttrSet에서 EnableAttr을 앞에 두도록 처리해본다..
			{
				i3Scene::SortAttrFromEnableAttrToBindAttr(m_pChara->getSceneGraphInfo()->getInstanceSg());
			}

			{
				i3OptSetupCenterMass opt;
				opt.Trace( m_pChara->getSceneGraphInfo()->getInstanceSg());
			}

			//if( g_pOption->m_bGenMipmap)
			{
				// Mipmap Generation
				i3OptMipmapGen opt;

				opt.setMinWidth( 32);
				opt.setMinHeight( 32);
				opt.setFilter( i3OptMipmapGen::FILTER_D3DX_TRIANGLE);
				opt.Trace( m_pChara->getSceneGraphInfo()->getInstanceSg());
			}

			//if( g_pOption->m_bConvertDXT)
			{
				i3OptConvertDXTTexture opt;

				opt.setDitherEnable( false);
				opt.setQuality( i3OptConvertDXTTexture::QUALITY_HIGHEST);

				//if( g_pOption->m_bDXTAlpha )
					opt.setTargetImageFormat( I3G_IMAGE_FORMAT_DXT5 );
				//else
				//	opt.setTargetImageFormat( I3G_IMAGE_FORMAT_DXT1_X );//I3G_IMAGE_FORMAT_DXT3);
				
				opt.Trace( m_pChara->getSceneGraphInfo()->getInstanceSg());
			}

			//	Extern texture
			//if( g_pOption->m_bExternTexture)
			{				
				i3SceneGraphInfo * pSceneInfo = m_pChara->getSceneGraphInfo();
				
				sprintf( szPath, "%s\\%s", g_pOption->m_szWorkPath, szFull);
				if( pSceneInfo != NULL)
				{
					char szBase[256] = "";
					i3OptExternRef optExtern;
					i3SceneFile sceneFile;

					pSceneInfo->AddRef();

					i3::safe_string_copy( szBase, szFull, 256 );					
					i3::remove_slash_filename(szBase);

					optExtern.setBasePath( szBase);
					optExtern.setExternTextureEnable(TRUE);

					//	common scene graph
					if( pSceneInfo->getCommonSg() != NULL)
					{
						optExtern.Trace( pSceneInfo->getCommonSg());
					}

					//	Instance scene graph
					if( pSceneInfo->getInstanceSg() != NULL)
					{
						optExtern.Trace( pSceneInfo->getInstanceSg());
					}

					sceneFile.setSceneGraphInfo( pSceneInfo);
					sceneFile.SetOverwriteExternalResStatus(true);	

					if( sceneFile.Save( szPath) == STREAM_ERR)
					{
						pSceneInfo->Release();

						sprintf( szMessage, "%s 파일을 저장할 수 없습니다.", szPath);
						
						goto LABEL_FAILED;
					} 
					else
					{
						pSceneInfo->Release();
					}
				}
				else
				{
					sprintf( szMessage, "%s 파일을 저장할 수 없습니다.", szPath);
						
					goto LABEL_FAILED;
				}
			}

			m_pChara->getSceneGraphInfo()->setInstanceSg( pOldSg);
			pOldSg->Release();
		}
	}

	// C Source Generation
	if( g_pOption->m_bGenC)
	{
		sprintf( szFull, "%s\\%s", g_pOption->m_szWorkPath, szRelPath);
		ExportC( szFull);
	}
	
	// I3CHR 저장
	if( g_pOption->m_bExportI3CHR)
	{
		i3ResourceFile file;

		file.setKeyObject( m_pChara);

		if( file.Save( pszPath) == STREAM_ERR)
		{
			sprintf( szMessage, "%s 파일을 저장할 수 없습니다.", pszPath);
						
			goto LABEL_FAILED;
		}
	}

	{
		
		for(INT32 i = 0; i < getAICount(); i++)
		{
			AI * pAI = getAI( i);

			pAI->getAIContext()->addGNodeState( I3_GAMENODE_STATE_LOCKED);
		}
	}

	if(resultMsg)
		strcpy(resultMsg, "Export 완료");
	else
		AfxMessageBox( "Export 완료!");

	return true;

LABEL_FAILED:

	if(resultMsg)
	{
		strcpy(resultMsg, szMessage);
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, szMessage);
		AfxMessageBox(szMessage);
	}

	return false;
}

bool CChara::ExportC( const char * pszOnlyPath)
{
	INT32 i;
	AI * pAI;

	for( i = 0; i < getAICount(); i++)
	{
		pAI = getAI( i);

		pAI->ExportC( pszOnlyPath);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
void CChara::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	// Scene Graph
	for( i = 0; i < getSceneGraphCount(); i++)
	{
		i3SceneGraphInfo * pInfo = getSceneGraph( i);

		pInfo->OnBuildObjectList( List);
	}

	// LOD
	for( i = 0; i < getLODCount(); i++)
	{
		CLOD * pLOD = getLOD( i);

		pLOD->OnBuildObjectList( List);
	}

	// Animation
	for( i = 0; i < getAnimCount(); i++)
	{
		i3Animation * pAnim = getAnim(i);

		pAnim->OnBuildObjectList( List);
	}

	// Sound
	for( i = 0; i < getSoundCount(); i++)
	{
		i3Sound * pSound = getSound( i);

		pSound->OnBuildObjectList( List);
	}

	// AI
	for( i = 0; i < getAICount(); i++)
	{
		AI * pAI = getAI( i);

		pAI->OnBuildObjectList( List);
	}

	i3ResourceObject::OnBuildObjectList( List);
}

#pragma pack( push, 8)

namespace pack
{
	struct CHARA
	{
		UINT8		m_ID[4] = { 'C', 'H', 'R', '1' };
		REAL32		m_Slope = 0.0f;
		REAL32		m_Const = 0.0f;
		OBJREF		m_pCollRoot = 0;
		UINT32		pad[61] = { 0 };
	};
}

#pragma pack( pop)

UINT32 CChara::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::CHARA data;

	Result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_Slope = getSlope();
	data.m_Const = getConst();
	data.m_pCollRoot = NULL;
	
	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	// Scene Graph
	Rc = SaveFromListToResourceFile(m_SgList, pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	// LOD
	Rc = SaveFromListToResourceFile(m_LODList, pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	// Animation
	Rc = SaveFromListToResourceFile(m_AnimList, pResFile);		// m_AnimList.SaveTo( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	// Sound
	Rc = SaveFromListToResourceFile(m_SoundList, pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	// AI
	Rc = SaveFromListToResourceFile(m_AIList, pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 CChara::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::CHARA data;
	

	Result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	// Scene Graph
	Rc = LoadFromResourceFileToList(m_SgList, pResFile, true);
	I3_CHKIO( Rc);
	Result += Rc;

	// LOD
	{
		i3::vector<CLOD*> tempList;

		Rc = LoadFromResourceFileToList(tempList, pResFile, true); //	tempList.LoadFrom( pResFile, true);
		I3_CHKIO( Rc);
		Result += Rc;
		
		for(size_t i = 0; i < tempList.size(); i++)
		{
			CLOD * pLOD = tempList[i];
			AddLOD( pLOD);
			pLOD->Release();
		}
		tempList.clear();
	}

	// Animation
	Rc = LoadFromResourceFileToList(m_AnimList, pResFile, true);								// m_AnimList.LoadFrom( pResFile, true);
	I3_CHKIO( Rc);
	Result += Rc;

	// 읽어들이지 못한 Animation은 제거한다.
	{
		for(size_t i = 0; i < m_AnimList.size(); i++)
		{
			if( m_AnimList[i] == NULL)
			{
				m_AnimList.erase(m_AnimList.begin() + i);
				i--;
			}
		}
	}

	// Sound
	Rc = LoadFromResourceFileToList(m_SoundList, pResFile, true);
	I3_CHKIO( Rc);
	Result += Rc;

	// 읽어들이지 못한 Sound는 제거한다.
	{
		for(size_t i = 0; i < m_SoundList.size(); i++)
		{
			if( m_SoundList[i] == NULL)
			{
				i3::vu::erase_index(m_SoundList, i);
				i--;
			}
		}
	}

	// AI
	i3::vector<AI*>	tempList;

	Rc = LoadFromResourceFileToList(tempList, pResFile, true);	//	tempList.LoadFrom( pResFile, true);
	I3_CHKIO( Rc);
	Result += Rc;

	m_AIList.resize( (INT32)tempList.size());

	for(size_t i = 0; i < tempList.size(); i++)
	{
		AI * pAI = tempList[i];

		if( pAI->getAIContext()->getParent() == NULL)
		{
			m_pChara->AddChild( pAI->getAIContext());
		}

		pAI->getAIContext()->setOwner( m_pChara);

		m_AIList[ i] = pAI;
		//pAI->AddRef();
	}

	setSlope( data.m_Slope);
	setConst( data.m_Const);

	if( hasName())
	{
		m_pChara->SetName( GetName());
	}

	return Result;
}

bool CChara::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	// Scene Graph
	SaveFromListToXML(m_SgList, "ScenGraphList", pFile, pXML, true);

	// LOD
	SaveFromListToXML(m_LODList, "LODList", pFile, pXML, true);

	// Animation
	SaveFromListToXML(m_AnimList, "AnimationList", pFile, pXML, true);

	// Sound
	SaveFromListToXML(m_SoundList, "SoundList", pFile, pXML, true);

	// AI
	SaveFromListToXML(m_AIList, "AIList", pFile, pXML, true);


	pXML->addAttr("Slope", getSlope());
	pXML->addAttr("Const", getConst());

	return true;
}

bool CChara::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	// Scene Graph
	LoadFromXMLToList(m_SgList, "ScenGraphList", pFile, pXML, true);


	// LOD
	{
		i3::vector<CLOD*> tempList;

		LoadFromXMLToList(tempList, "LODList", pFile, pXML, true); //	tempList.LoadFrom( pResFile, true);

		for (size_t i = 0; i < tempList.size(); i++)
		{
			CLOD * pLOD = tempList[i];
			AddLOD(pLOD);
			pLOD->Release();
		}
		tempList.clear();
	}

	// Animation
	LoadFromXMLToList(m_AnimList, "AnimationList", pFile, pXML, true);								// m_AnimList.LoadFrom( pResFile, true);

	// 읽어들이지 못한 Animation은 제거한다.
	{
		for (size_t i = 0; i < m_AnimList.size(); i++)
		{
			if (m_AnimList[i] == NULL)
			{
				m_AnimList.erase(m_AnimList.begin() + i);
				i--;
			}
		}
	}

	// Sound
	LoadFromXMLToList(m_SoundList, "SoundList", pFile, pXML, true);

	// 읽어들이지 못한 Sound는 제거한다.
	{
		for (size_t i = 0; i < m_SoundList.size(); i++)
		{
			if (m_SoundList[i] == NULL)
			{
				i3::vu::erase_index(m_SoundList, i);
				i--;
			}
		}
	}

	// AI
	i3::vector<AI*>	tempList;

	LoadFromXMLToList(tempList, "AIList", pFile, pXML, true);	//	tempList.LoadFrom( pResFile, true);

	m_AIList.resize((INT32)tempList.size());

	for (size_t i = 0; i < tempList.size(); i++)
	{
		AI * pAI = tempList[i];

		if (pAI->getAIContext()->getParent() == NULL)
		{
			m_pChara->AddChild(pAI->getAIContext());
		}

		pAI->getAIContext()->setOwner(m_pChara);

		m_AIList[i] = pAI;
		//pAI->AddRef();
	}

	REAL32 slope;
	pXML->getAttr("Slope", &slope);
	setSlope(slope);
	
	REAL32 __const;
	pXML->getAttr("Const", &__const);
	setConst(__const);

	if (hasName())
	{
		m_pChara->SetName(GetName());
	}

	return true;
}

void CChara::CheckBones(void)
{
	INT32 i, j;
	i3LOD * pFrameLOD, * pLOD;
	i3Skeleton * pFrameSkel, * pSkel;
	i3Bone * pBone1, * pBone2;

	if( m_pBody->getLODCount() < 2)
		return;

	I3TRACE( "------------------ Analyze LOD --------------------\n");

	pFrameLOD = m_pBody->getFrameLOD();
	pFrameSkel = pFrameLOD->getSkeleton();

	for( i = 0; i < (INT32) m_pBody->getLODCount(); i++)
	{
		pLOD = m_pBody->getLOD( i);

		if( pLOD == pFrameLOD)
		{
			I3TRACE( "Frame LOD : %d\n", i);
			continue;
		}

		pSkel = pLOD->getSkeleton();

		if( pSkel->getBoneCount() != pFrameSkel->getBoneCount())
		{
			I3TRACE( "%d LOD의 Bone Hierarchy 구조가 Frame LOD와 다릅니다.");
			continue;
		}

		for( j = 0; j < pSkel->getBoneCount(); j++)
		{
			pBone1 = pFrameSkel->getBone( j);
			pBone2 = pSkel->getBone( j);

			if( i3::generic_is_iequal( pBone1->getName(), pBone2->getName()) == false)
			{
				I3TRACE( "두 Bone의 이름이 일치하지 않음 : LOD(%d)[%s] <-> Frame LOD[%s]\n", i, pBone2->getName(), pBone1->getName());
			}

			I3TRACE( "--- LOD:%d [%s]\n", i, pBone2->getName());
			i3Matrix::Dump( pBone2->getMatrix());
			if( i3Matrix::IsSame( pBone1->getMatrix(), pBone2->getMatrix()) == false)
			{
				I3TRACE( "두 Bone의 값이 일치하지 않음 : ...\n");
				I3TRACE( "LOD(%d) --\n", i);
				i3Matrix::Dump( pBone2->getMatrix());
				I3TRACE( "Frame Bone --\n");
				i3Matrix::Dump( pBone1->getMatrix());
			}
		}
	}
	
	I3TRACE( "----------------------------------------------------\n");
}

void CChara::DumpAI(void)
{
	char temp[256] = "";
	INT32 nAICount = 0;
	INT32 nAIStateCount = 0;

	i3FileStream file;
	DeleteFile("DumpAI.txt");
	if( !file.Create("DumpAI.txt", STREAM_READ | STREAM_WRITE))
	{		
		sprintf( temp, "Dump에 실패했습니다. DumpAI.txt 파일을 생성할 수 없습니다.");

		I3PRINTLOG(I3LOG_WARN, temp);
		AfxMessageBox( temp);

		return;
	}

	for(INT32 i=0; i<getAICount(); i++)
	{
		nAICount++;

		i3::vector<i3::rc_string> list;
		nAIStateCount += DumpAIState( getAI(i)->getAIContext(), list);

		for(size_t cnt=0; cnt<list.size(); cnt++)
		{
			file.WriteLine( list[cnt].c_str());
		}
		list.clear();
	}
 
	file.WriteLine("=======================================================\r\n");	

	sprintf( temp, "Name : %s\r\n", m_pChara->GetName());
	file.WriteLine(temp);

	I3ASSERT( nAICount == getAICount());	
	sprintf( temp, "Total AI count : %d\r\n", nAICount);
	file.WriteLine(temp);

	sprintf( temp, "Total AIState count : %d\r\n", nAIStateCount);
	file.WriteLine(temp);

	sprintf( temp, "Total Animation count : %d\r\n", getAnimCount());
	file.WriteLine(temp);

	file.WriteLine("=======================================================\r\n");

	file.Close();

	//	덤프파일을 메모장에 띄운다.
	WinExec("NotePad.exe DumpAI.txt", SW_SHOWNORMAL);
}

INT32 CChara::DumpAIState( i3AIContext * pAIContext, i3::vector<i3::rc_string>& List)
{
	char temp[256] = "";
	char szMessage[256] = "";	
	char szAIName[256] = "";
	INT32 nAIStateCount = 0;	

	if( pAIContext == NULL )		return 0;

	if( pAIContext->hasName())
	{
		i3::safe_string_copy( temp, pAIContext->GetName(), 256 );
		sprintf( szAIName, pAIContext->GetName());
	}

	//	부모 AIContext 이름을 가져온다.	
	i3GameNode * pParent = pAIContext->getParent();
	while( pParent != NULL)
	{
		if( i3::same_of<i3AIContext*>(pParent)) //->IsExactTypeOf( i3AIContext::static_meta()))
		{
			if( pParent->hasName())
			{
				sprintf( szAIName, "%s:%s", pParent->GetName(), temp);
				i3::safe_string_copy( temp, szAIName, 256 );
			}

			pParent = pParent->getParent();

			continue;
		}
		else
			break;
	}

	i3::generic_string_cat( szAIName, ":");

	i3AI * pAI = pAIContext->getAI();
	for(INT32 i = 0; i < pAI->getAIStateCount(); i++)
	{
		char szInfo[1024] = "";
		i3AIState * pAIState = pAI->getAIState(i);

		char * pszAIStateName = "(null)";
		char * pszAnimName = "(null)";

		if( pAIState->GetName())
			pszAIStateName = (char*)pAIState->GetName();

		if( pAIState->getAnim() != NULL)
		{
			if( pAIState->getAnim()->hasName())
			{
				pszAnimName = (char*)pAIState->getAnim()->GetName();

				//sprintf( szInfo, "--- Duration %f / Scele %f / Blend %f",
				//								pAIState->getDuration(),
				//								pAIState->getTimeScale(),
				//								pAIState->getBlendTime());
			} 
		}
	
		//	AIState 정보 출력
		sprintf( szMessage, "[%s%s] %s %s\r\n", szAIName, pszAIStateName, pszAnimName, szInfo);

		List.push_back(szMessage);					// szMessage, i3::generic_string_size( szMessage));

		nAIStateCount++;
	}

	return nAIStateCount;
}

void CChara::ConvertToAnimTemplate( bool bSetAnimToNull)
{
	INT32 i;
	AI * pAI;

	for( i = 0; i < getAICount(); i++)
	{
		pAI = getAI( i);

		pAI->ConvertToAnimTemplate( bSetAnimToNull);
	}
}

bool CChara::CopyRefFileTo(const char* mediaPath, const char* chrEditorPath, 
	const char* copyToPath)
{
	i3ResourceFile file;
	char pszPath[ MAX_PATH];
	sprintf_s(pszPath, MAX_PATH, "%s/%s", mediaPath ? mediaPath : ".", chrEditorPath);
	
	if(file.Load(pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "ERR! - failed to load %s", pszPath);
		return false;
	}

	{
		CChara * pChara;
		pChara = (CChara *)file.getKeyObject();

		if(!i3::kind_of<CChara*>(pChara))
		{
			I3PRINTLOG(I3LOG_WARN, "ERR! - %s is invalid file \n", pszPath);
			return false;
		}

		// Scene Graph
		INT32 i=0;

		typedef i3::vector<i3::string> i3StrVec;
		i3StrVec resFiles;
		
		char szi3sName[MAX_PATH] = {0};
		strcpy_s(szi3sName, MAX_PATH, chrEditorPath);
		char* p = strrchr(szi3sName, '.');

		if(p)
			strcpy(p, ".i3s");
		else
			strcat(szi3sName, ".i3s");

		resFiles.push_back(szi3sName);

		// i3CHR내용

		// LOD
		for( i = 0; i < pChara->getLODCount(); i++)
		{
			CLOD * pLOD = pChara->getLOD(i);
			resFiles.push_back(pLOD->GetName());
		}

		// Animation
		for( i = 0; i < pChara->getAnimCount(); i++)
		{
			i3Animation * pAnim = pChara->getAnim(i);
			resFiles.push_back(pAnim->GetName());
		}

		// Sound
		for( i = 0; i < pChara->getSoundCount(); i++)
		{
			i3Sound * pSound = pChara->getSound( i);
			resFiles.push_back(pSound->GetName());
		}

		// Texture

		//// AI
		//for(i=0; i< pChara->getAICount(); i++)
		//{
		//	AI* pAI = pChara->getAI(i);
		//	resFiles.push_back(pAI->GetName());
		//}
		
		for(i3StrVec::size_type k=0; k<resFiles.size(); k++)
		{
			char src[MAX_PATH] = {0};
			sprintf_s(src, MAX_PATH, "%s/%s", mediaPath, resFiles[k].c_str());

			char dst[MAX_PATH] = {0};
			sprintf_s(dst, MAX_PATH, "%s/%s", copyToPath, resFiles[k].c_str());

			char copyDir[MAX_PATH] = {0};
			i3::extract_directoryname(dst, copyDir);

			bool copyOk = false;
			if(i3ResourceManager::MakePathDirectory(copyDir))
			{
				if(0 != CopyFileEx(src, dst, NULL, NULL, NULL, 0))
					copyOk = true;
				else
				{
					OutputDebugString(src);
					OutputDebugString(" is failed to copy!!! \n");
				}
			}			
		}

	}

	return true;
}