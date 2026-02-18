#include "pch.h"
#include "Avatar.h"
#include "AvatarDefine.h"
#include "AvatarViewerFramework.h"
#include "GlobalVar.h"
#include "i3Scene/i3TextureBindAttr.h"
#include "MainFrm.h"
#include "i3Base/itl/range/generic_empty.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string.h"

I3_CLASS_INSTANCE( Avatar); //, i3Chara);

Avatar::Avatar(void) : m_pBody(NULL), m_pTrans(NULL)
	, m_bLoaded(false),	m_nLODConst(1), m_HeadType(-1)
	, m_LoadComplateProc(NULL), m_pRighthandWeapon(NULL), m_pBackpackWeapon(NULL)
{
	m_pHeadBoneRef = i3BoneRef::new_object();
	m_pFaceBoneRef = i3BoneRef::new_object();
	m_pHolsterBoneRef = i3BoneRef::new_object();
	m_pBeltBoneRef = i3BoneRef::new_object();

	m_CharaResType = eCHARA_TYPE_MALE;

	m_pGen = AvatarGen::new_object();
	m_pGen->setLoadStyle( AVATAR_LOAD_STYLE_MERGE);

	m_szPartFullPathTemp[0] = 0;

	for( UINT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		m_pEquipSgInfo[i] = NULL;
	}
}

Avatar::~Avatar(void)
{
	if (m_pHeadBoneRef)
	{
		m_pHeadBoneRef->RemoveFromParent();
		I3_SAFE_RELEASE(m_pHeadBoneRef);
	}
	

	if (m_pFaceBoneRef)
	{
		m_pFaceBoneRef->RemoveFromParent();
		I3_SAFE_RELEASE(m_pFaceBoneRef);
	}

	if( m_pHolsterBoneRef != NULL)
	{
		m_pHolsterBoneRef->RemoveFromParent();
		m_pHolsterBoneRef->RemoveAllChild();
		I3_SAFE_RELEASE( m_pHolsterBoneRef);
	}

	if( m_pBeltBoneRef != NULL)
	{
		m_pBeltBoneRef->RemoveFromParent();
		m_pBeltBoneRef->RemoveAllChild();
		I3_SAFE_RELEASE( m_pBeltBoneRef);
	}

	m_pTrans->RemoveAllChild();
	I3_SAFE_RELEASE(m_pTrans);
	I3_SAFE_RELEASE(m_pBody);
	I3_SAFE_RELEASE(m_pGen);
}

bool Avatar::isPart( EQUIP::ePART part)
{
	return m_pGen->getPartName( part)[0] != 0;
}


void Avatar::setPartName( EQUIP::ePART part, const char* pszName)
{
	if( pszName[0] == 'M')
		m_CharaResType = eCHARA_TYPE_MALE;
	else if( pszName[0] == 'F')
		m_CharaResType = eCHARA_TYPE_FEMALE;
	
	switch( part)
	{
	case EQUIP::ePART_HEADGEAR :	setPartNameByDummy(part, pszName, m_pHeadBoneRef);	break;
	case EQUIP::ePART_FACEGEAR :	setPartNameByDummy(part, pszName, m_pFaceBoneRef);	break;
	case EQUIP::ePART_HOLSTER :		setPartNameByDummy(part, pszName, m_pHolsterBoneRef);	break;
	case EQUIP::ePART_BELT :		setPartNameByDummy(part, pszName, m_pBeltBoneRef);	break;
	default :						m_pGen->SetPartName( part, pszName, false);		break;
	}
#if !defined( CLIENT_SIDE)
	m_pGen->SetPartName( part, pszName, false);
#endif

	m_pGen->SetTeam( m_eTeamType);

	m_bLoaded = false;
}

void Avatar::setPartNameByDummy( EQUIP::ePART part, const char * pszPath, i3BoneRef* pBoneRef)
{
	char szPath[ MAX_PATH];
	char szFullPath[ MAX_PATH];

 	i3SceneFile file;

	const char * pszPart = AVATAR::getPartName( part).c_str();
	i3::snprintf( szPath, _countof( szPath), "Avatar/%s/%s/%s.i3s", pszPart, pszPath, pszPath);

	if( i3System::isAbsolutePath( szPath))
	{
		i3::safe_string_copy( szFullPath, szPath, _countof( szFullPath));
	}
	else
	{
		if (i3::generic_empty(i3ResourceFile::GetWorkingFolder() ) )
			i3::snprintf( szFullPath, _countof( szFullPath), "%s", szPath);
		else
			i3::snprintf( szFullPath, _countof( szFullPath), "%s/%s", i3ResourceFile::GetWorkingFolder(), szPath);
	}

	if( i3System::IsFile( szFullPath))
	{
		file.Load(szFullPath);

		i3Node* pSg = file.getSceneGraphInfo()->getInstanceSg();
		i3Scene::ModifyDynamicState( pSg, I3_PHYSIX_STATE_KINEMATIC, 0);

		i3TextureBindAttr * pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pSg, i3TextureBindAttr::static_meta());

		if( pAttr != NULL)
		{
			for(INT32 view = AVATAR::VIEW_1P; view <= AVATAR::VIEW_3P; view++)
			{
				AVATAR::SetTexturePathName( pAttr->GetTexture()->GetName(), (AVATAR::VIEW) view, (EQUIP::ePART) part);
			}
		}
		pBoneRef->RemoveAllChild();
		pBoneRef->AddChild(pSg);

		// 장비 파츠는 일단 세팅해놓고..
		// SafeMacroTexture 생성시 사용
		/*m_pGen->setSgInfo( part, 1, file.getSceneGraphInfo());
		m_pGen->setSgInfo( part, 2, file.getSceneGraphInfo());
		m_pGen->setSgInfo( part, 3, file.getSceneGraphInfo());*/

		I3_REF_CHANGE( m_pEquipSgInfo[ part], file.getSceneGraphInfo());

#if 0
		m_pGen->_BuildMacroTex(true);

		pAttr->SetModified(true);
		pAttr->SetTexture( m_pGen->getDiffuseTexture());

		pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pSg, i3NormalMapBindAttr::static_meta());
		if( pAttr != NULL)
		{
			pAttr->SetModified(true);
			pAttr->SetTexture( m_pGen->getNormalMap());
		}

		pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pSg, i3SpecularMapBindAttr::static_meta());
		if( pAttr != NULL)
		{
			pAttr->SetModified(true);
			pAttr->SetTexture( m_pGen->getSpecularMap());
		}

		pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pSg, i3ReflectMaskMapBindAttr::static_meta());
		if( pAttr != NULL)
		{
			pAttr->SetModified(true);
			pAttr->SetTexture( m_pGen->getReflectionMaskMap());
		}

		VEC2D offset, scale;
		const AVATAR::TEXINFO * pTexInfo = AVATAR::getPartTexInfo( AVATAR::VIEW_3P, part);

		offset.x = (REAL32) pTexInfo->m_X / AVT_TEX_WIDTH;
		offset.y = (REAL32) pTexInfo->m_Y / AVT_TEX_HEIGHT;

		scale.x = (REAL32) pTexInfo->m_W / AVT_TEX_WIDTH;
		scale.y = (REAL32) pTexInfo->m_H / AVT_TEX_HEIGHT;

		// Texture Coords.
		i3::vector<i3RenderAttr*> list;
		i3Scene::FindAttrByExactType( pSg, i3GeometryAttr::static_meta(), list);

		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3GeometryAttr * pGeoAttr = static_cast<i3GeometryAttr*>(list.at(i));

			if( !pGeoAttr->IsModified())
			{
				i3VertexArray * pVA = pGeoAttr->GetVertexArray();
				I3ASSERT( pVA->Lock() );

				for( UINT32 j = 0; j < pVA->GetCount(); j++)
				{
					VEC2D vec2;

					pVA->GetTextureCoord( 0, j, &vec2);

					// Normalize
					if( vec2.x < 0.0f)
						vec2.x = 1.0f + vec2.x;

					if( vec2.y < 0.0f)
						vec2.y = 1.0f + vec2.y;

					i3Vector::Mul( &vec2, &vec2, &scale);
					i3Vector::Add( &vec2, &vec2, &offset);
					pVA->SetTextureCoord( 0, j, &vec2);
				}

				pVA->Unlock();
			}
		}

#endif
	}
}

void Avatar::SetDummyBoneRef(const char* szBoneName, i3BoneRef* destBoneRef)
{
	I3ASSERT(szBoneName != "");

	i3LOD* pLOD = getSceneObject()->GetBody()->getFrameLOD();

	INT32 idx = pLOD->getSkeleton()->FindBoneByName(szBoneName);

	if (destBoneRef)
	{
		destBoneRef->setBoneIndex(idx);
		destBoneRef->SetName(szBoneName);

		m_pBody->AddChild(destBoneRef);
	}
}

static INT32		s_Count = 0;

void Avatar::OnUpdate( REAL32 tm)
{
	i3Chara::OnUpdate( tm);

	if( m_pGen != NULL)
	{
		// Avatar Gen의 Update 호출.
		// 여기에서는 항상 호출하고 있지만, 사실 Loading이 일어나고 있지 않다면 호출될 필요없다.
		m_pGen->UpdateInThread();

		// 모든 Loading이 끝나고, Geometry 및 Texture에 대한 재구성이 모두 완료되면
		// isReadyScene() 함수가 true를 반환한다.
		if( m_pGen->isReadyScene())		
		{
			bool bPreCreated = false;

			//게임내 회전값으로 셋팅..
			if (m_pTrans == NULL)
			{
				m_pTrans = i3Transform::new_object();

				m_pBody = m_pGen->GetGeneratedBody( true);
				m_pBody->setConst(1.0f);

				m_pTrans->AddChild(m_pBody);
				m_pBody->AddRef();

				MATRIX matRes;
				i3Matrix::Identity(&matRes);
				m_pTrans->SetMatrix(&matRes);
				MATRIX mat;
				i3Matrix::SetRotateX(&mat,I3_DEG2RAD(-90.0f));
				i3Matrix::Mul(&matRes,&matRes,&mat);

				i3Matrix::SetRotateY(&mat,I3_DEG2RAD(-180.0f));
				i3Matrix::Mul(&matRes,&matRes,&mat);
				m_pTrans->SetMatrix(&matRes);

				m_pTrans->AddRef();
				i3Chara::Create(m_pTrans);
				m_pTrans->Release();
			}
			else
			{
				m_pTrans->RemoveAllChild();
	
				m_pBody = m_pGen->GetGeneratedBody( true);
				m_pBody->setConst(1.0f);

				m_pTrans->AddChild(m_pBody);
				m_pBody->AddRef();
			}


			getSceneObject()->SetBody(m_pBody);

			m_pBody->setConst(1.0f);

			m_pHeadBoneRef->RemoveFromParent();
			m_pFaceBoneRef->RemoveFromParent();
			m_pHolsterBoneRef->RemoveFromParent();
			m_pBeltBoneRef->RemoveFromParent();
			
			//바디에 본 더미를 셋팅한다..
			SetDummyBoneRef("Bone_HeadGearAttach", m_pHeadBoneRef);
			SetDummyBoneRef("Bone_FaceGearAttach", m_pFaceBoneRef);
			SetDummyBoneRef("Bone_HolsterAttach", m_pHolsterBoneRef);
			SetDummyBoneRef("Bone_BeltAttach", m_pBeltBoneRef);

			SetDynamicState(false);

			this->Instantiate();

			SetChangeCampTexture(m_eTeamType);

			// 머리 상태에 따라 Visible

			m_bLoaded = true;
		}
	}
}

INT32 Avatar::GetLOD(void)
{
	if( m_pBody == NULL)
		return 0;

	return (INT32) m_pBody->getConst();
}

void Avatar::SetLOD( INT32 lod)
{
	m_nLODConst = lod;
	if( m_pBody != NULL)
	{
		m_pBody->setConst( (REAL32) lod);
		m_pBody->setSlope( 0.0f);

		if (lod >= 1)
			lod -= 1;

		for (UINT32 i = 0; i < m_vEquipBody.size(); ++i)
		{
			if (m_vEquipBody[i])
			{
				i3Body* pBody = m_vEquipBody[i];
				pBody->setConst( (REAL32) lod);
				pBody->setSlope( 0.0f);
			}
		}

		INT32 txLOD = (lod<2)?0:lod-1;

		i3::vector<i3RenderAttr*> list;
		i3Scene::FindAttrByType( m_pBody, i3TextureBindAttr::static_meta(), list);
		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3TextureBindAttr* pBinder = static_cast<i3TextureBindAttr*>(list.at(i));
			if( pBinder->GetTexture() != NULL)
			{
				i3TextureDX * pTex = static_cast<i3TextureDX*>(pBinder->GetTexture());
				pTex->GetDXImage()->SetLOD( txLOD);
			}
		}
	}
}

void Avatar::SetChangeCampTexture(TEAM_TYPE eCamp)
{
	UINT32 i;

	I3ASSERT( m_pBody != NULL);

	if( eCamp > TEAM_NONE)
	{
		m_eTeamType = eCamp;
		m_pGen->SetTeam( eCamp);
	}

	i3::vector<i3Node*> nodeList;
	i3::vector<i3RenderAttr*> attrlist;

	for( i = 0; i < m_pBody->getLODCount(); i++)
	{
		i3LOD * pLOD = m_pBody->getLOD( i);
		if( pLOD != NULL)
		{
			i3Scene::FindNodeByExactType( pLOD->getShapeNode(), i3AttrSet::static_meta(), nodeList);
		}
	}

	char szTemp[ MAX_PATH];
	for( i = 0; i < nodeList.size(); i++)
	{
		i3Node * pNode = nodeList.at( i);

		i3::safe_string_copy( szTemp, pNode->GetName(), sizeof( szTemp));
		i3::to_upper( szTemp);

		if( i3::contain_string( szTemp, "TEAMBAND_") >= 0)
		{
			if( eCamp > TEAM_NONE)
				pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
			else
				pNode->AddFlag( I3_NODEFLAG_DISABLE);

			i3Scene::FindAttrByExactType( pNode, i3TextureBindAttr::static_meta(), attrlist);
		}
	}

	for( i = 0; i < attrlist.size(); i++)
	{
		i3TextureBindAttr * pAttr = (i3TextureBindAttr*) attrlist.at( i);
		I3ASSERT( pAttr != NULL);
		if( pAttr->GetTexture() != NULL)
		{
			i3Texture * pTex = m_pGen->LoadTex( pAttr->GetTexture()->GetName());
			if( pTex != NULL)
				pAttr->SetTexture( pTex);
		}
	}

	nodeList.clear();
	attrlist.clear();
}

void Avatar::SetChangePartEachLOD(INT32 idx,TEAM_TYPE eCamp)
{
	/*std::string strCampName;

	if (eCamp == E_CAMP_TEXTURE_BLUE)
	{
		strCampName = "_BLUE.i3i";
	}
	else if (eCamp == E_CAMP_TEXTURE_RED)
	{
		strCampName = "_RED.i3i";
	}


	i3LOD* pCurrLOD = m_pBody->getLOD(idx);
	i3Node* pShape = pCurrLOD->getShapeNode();
	i3Texture* pTex = NULL;

	i3ResourceManager* pManager = g_pFramework->GetResourceManager();

	INT32 part, x, y, w, h, view;
	const AVATAR::TEXINFO * pTexInfo;
	bool bValid = false;

	INT32 scaleFactor = 1;

	i3Texture* pDiffTexture = m_pGen->GetTex_Diff();

	for( part = 0; part < EQUIP::ePART_COUNT; part++)
	{
		for( view = AVATAR::VIEW_1P; view <= AVATAR::VIEW_3P; view++)
		{
			pTexInfo = AVATAR::getPartTexInfo( (AVATAR::VIEW) view, (EQUIP::ePART) part);

			if( pTexInfo->m_W <= 0)
				continue;

			if (pTexInfo->m_szPathName)
			{
				std::string szName = FindTextureNameByMeta(pTexInfo->m_szPathName);

				x = pTexInfo->m_X / scaleFactor;
				y = pTexInfo->m_Y / scaleFactor;
				w = pTexInfo->m_W / scaleFactor;
				h = pTexInfo->m_H / scaleFactor;

				szName = szName + strCampName;

				pTex = pManager->LoadTexture(szName.c_str());

				if (pTex)
				{
					m_pGen->CopyTex(pTex, pDiffTexture, x, y, w, h, "Diffuse");
					pTex->Release();

					I3TRACE("=================\n");
					I3TRACE("Change Texture%s\n",szName.c_str());
					I3TRACE("=================\n");
				}
			}
		}
	}


	i3TextureBindAttr * pBindAttr = (i3TextureBindAttr * )i3Scene::FindAttrByExactType(pShape, i3TextureBindAttr::GetClassMeta());
	pBindAttr->SetTexture(pDiffTexture);*/
}

std::string Avatar::FindTextureNameByMeta(std::string szPathName)
{
	std::string szPureFileName;
	INT32 nStartPos = 0;
	INT32 nEndPos = szPathName.rfind('_');
	szPureFileName = szPathName.substr(nStartPos, nEndPos);
	return szPureFileName;
}


void Avatar::SetDynamicState( bool bFlag)
{
	if( getSceneObject() == NULL)
		return;

	UINT32 addState, remState;

	addState = remState = 0;

	if( bFlag)
	{
		remState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}
	else
	{
		addState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}

	VEC3D vel;

	i3Vector::Set( &vel, 0.0f, 0.0f, 0.0f);

	i3Scene::ModifyDynamicState( getSceneObject(), addState, remState);
	i3Scene::SetDynamicVelocity( getSceneObject(), NULL, &vel, &vel);
}

void Avatar::TransformPart( const EQUIP::ePART ePart, const VEC4D& vTransform )
{
	i3BoneRef* pBoneRef = NULL;
	switch( ePart )
	{
	case EQUIP::ePART_HEADGEAR :{ pBoneRef = m_pHeadBoneRef; }break;
	case EQUIP::ePART_FACEGEAR :{ pBoneRef = m_pFaceBoneRef; }break;
	case EQUIP::ePART_HOLSTER :	{ pBoneRef = m_pHolsterBoneRef; }break;
	case EQUIP::ePART_BELT :	{ pBoneRef = m_pBeltBoneRef; }break;
	}
	if( NULL == pBoneRef )
		return ;

	m_vEquipBody.resize(EQUIP::ePART_COUNT);

	i3Node * pNode = pBoneRef->GetChild(0);
	if( NULL == pNode )
		return ;

	MATRIX matRes, matScale, matTrans;
	i3Matrix::Identity(&matScale);
	i3Matrix::Identity(&matTrans);
	i3Matrix::Identity(&matRes);

	REAL32 fScale = vTransform.w;
	i3Matrix::SetScale(&matScale, fScale,fScale,fScale );
	i3Matrix::SetTranslate(&matTrans, vTransform.x, vTransform.y, vTransform.z);
	i3Matrix::Mul(&matRes,&matScale,&matTrans);

	if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::GetClassMeta() ) )
	{
		i3Transform * pTrans = (i3Transform *)pNode;
		if( pTrans )
			pTrans->SetMatrix(&matRes);
	}
	else if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::GetClassMeta() ) )
	{
		i3Body * pBody = ( i3Body * )pNode;
		if( NULL == pBody ) return ;

		m_vEquipBody[ePart] = pBody;

		i3Transform * pTrans = i3Transform::new_object();

		pTrans->AddChild( pBody );
		pTrans->SetMatrix( &matRes );

		pBoneRef->RemoveAllChild();
		pBoneRef->AddChild( pTrans );
		I3_SAFE_RELEASE( pTrans );
	}

	SetChangeCampTexture(m_eTeamType);

	INT32 nLOD = m_nLODConst;
	if (nLOD >= 1)
		nLOD -= 1;

	m_vEquipBody[ePart]->setConst( (REAL32) nLOD);
	m_vEquipBody[ePart]->setSlope( 0.0f);
}

void Avatar::SaveMacroTex( const char * pszPath)
{
	for( UINT32 part = 0; part < EQUIP::ePART_COUNT; part++)
	{
		if( m_pEquipSgInfo[ part] != NULL)
		{
			for( UINT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
				m_pGen->setSgInfo( (EQUIP::ePART)part, lod, m_pEquipSgInfo[ part]);
		}
	}
	m_pGen->_BuildMacroTex(true);
	m_pGen->SaveMacroTex( pszPath);
}

void Avatar::ConvertSGbyMacroTexture(void)
{
}

Avatar * Avatar::ChangeHeadPart( const INT32& headType)
{
	char szName[ 260];
	char szFullPath[ 260];

	if( headType != -1 && m_HeadType != headType)
	{
		i3::generic_string_copy( szName, getPartName((EQUIP::ePART_CHARA)).c_str());

		INT32 pos = i3::contain_string( szName, "_X");
		if( pos < 0)
			pos = i3::contain_string( szName, "_Y");
		if( pos < 0)
			pos = i3::contain_string( szName, "_Z");

		if( pos < 0)
			pos = i3::generic_string_size( szName) + 1;

		szName[ pos] = 0;

		if( headType == 1)
			i3::generic_string_cat( szName, "_Y");
		else if( headType == 2)
			i3::generic_string_cat( szName, "_Z");
		else
			i3::generic_string_cat( szName, "_X");

		i3::sprintf( szFullPath, "Avatar/Mask/%s", szName);

		Avatar * pNewAvatar = g_pFramework->LoadPart( szFullPath, m_eTeamType, (headType!=3));
		pNewAvatar->m_HeadType = headType;

		return pNewAvatar;
	}

	return NULL;
}

void Avatar::LoadComplate( void)
{
	if( i3::generic_string_size( m_szPartFullPathTemp) > 0)
		g_pFramework->LoadPart( m_szPartFullPathTemp, m_eTeamType);

	if( m_LoadComplateProc)
		m_LoadComplateProc( m_LoadComplatePart, &m_LoadComplatePartvTransform);
	else
		TransformPart( m_LoadComplatePart, m_LoadComplatePartvTransform);

	// 무기 붙이기
	LoadWeapon( "Weapon/AK-47_DualMag/AK-47_DualMag.I3S");
}


void Avatar::LoadWeapon( const char * pszPath)
{
	if( m_pBody == NULL)
		return;

	i3SceneFile file;

	if( file.Load( pszPath) != STREAM_ERR)
	{
		i3Node * pNode = file.getSceneGraphInfo()->getInstanceSg();
		i3Scene::ModifyDynamicState( pNode, I3_PHYSIX_STATE_DISABLE, 0);

		i3Node * pInstance = i3Scene::CreateClone( pNode, I3_COPY_INSTANCE);

		if( m_pRighthandWeapon != NULL)
		{
			m_pRighthandWeapon->RemoveFromParent();
		}
		m_pRighthandWeapon = pNode;
		m_pRighthandWeapon->AddFlag( I3_NODEFLAG_DISABLE);
		
		i3::vector<i3Node*> list;
		i3Scene::FindNodeByName( m_pBody, "WeaponPointDummyRight", list);

		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3Node * pDummy = list.at(i);
			if( i3::same_of< i3Transform*>(pDummy))
			{
				pDummy->AddChild( pNode);
				break;
			}
		}

		list.clear();
		i3Scene::FindNodeByName( m_pBody, "WeaponPointDummyBack", list);
		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3Node * pDummy = list.at(i);
			if( i3::same_of< i3Transform*>(pDummy))
			{
				pDummy->AddChild( pInstance);
				break;
			}
		}

		if( m_pBackpackWeapon != NULL)
		{
			m_pBackpackWeapon->RemoveFromParent();
		}
		m_pBackpackWeapon = pInstance;
		m_pBackpackWeapon->AddFlag( I3_NODEFLAG_DISABLE);
	}
}