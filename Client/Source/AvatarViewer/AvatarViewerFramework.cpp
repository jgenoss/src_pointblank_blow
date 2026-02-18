#include "pch.h"
#include "AvatarViewerFramework.h"
#include "GlobalVar.h"
#include "AvatarManager.h"
#include "MainFrm.h"

#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/substr.h"
#include "i3Base/string/ext/resolve_path_from_environment_variable.h"
#include "i3Base/string/ext/get_exec_path.h"

I3_CLASS_INSTANCE( AvatarViewerFramework); //, i3ViewerFramework);

AvatarViewerFramework::AvatarViewerFramework(void) : m_Team(TEAM_RED)
{
	m_EditMode = I3_VIEWER_EDIT_MODE_NORMAL;
	m_pReflectMapAttr = NULL;

	m_pNormalViewRoot = NULL;

	m_pInterLinkNode = NULL;

	m_pSelNode = NULL;
	m_pIOManager = NULL;

	m_pAvatar = NULL;
	m_pTrans = NULL;
	m_pAvatarManager = NULL;
}

AvatarViewerFramework::~AvatarViewerFramework(void)
{
	I3_SAFE_NODE_RELEASE( m_pInterLinkNode);
	I3_SAFE_RELEASE( m_pReflectMapAttr);

	I3_SAFE_RELEASE( m_pNormalViewRoot);

	if( m_pAvatar != NULL)
	{
		RemoveChild( m_pAvatar);
		m_pAvatar->Release();
		m_pAvatar = NULL;
	}

	if( m_pAvatarManager != NULL)
	{
		m_pAvatarManager->Terminate();

		m_pAvatarManager->Release();
		m_pAvatarManager = NULL;
	}

	I3_SAFE_RELEASE( m_pIOManager);

	removeAllInitBone();

	I3_SAFE_RELEASE(m_pTrans);
}

void AvatarViewerFramework::OnCreate(void)
{
	//i3Framework::OnCreate();
	i3ViewerFramework::OnCreate();

	i3ResourceFile::setSearchExternRefEnable( true);

	{
		m_pIOManager = i3SceneIOManager::new_object();
		m_pIOManager->Create();
	}

	// Default ReflectMap
	{
		m_pReflectMapAttr = i3ReflectMapBindAttr::new_object();

		m_pAttrSet->AddAttr( m_pReflectMapAttr);
	}

	{
		m_pInterLinkNode = i3Node::new_object();

		m_pInterLinkNode->SetFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);
		m_pInterLinkNode->SetName( "_InterLinkNode_");
	}

	ApplyOption();

	/*{
		m_pAvatar = Avatar::NewObject();

		AddChild( m_pAvatar);
	}*/
	//i3SceneIOQueue* pIOQueue = i3SceneIOQueue::NewObject();
	g_pAvatarManager = m_pAvatarManager = cAvatarManager::new_object();
	m_pAvatarManager->Create( "ThreadTest");
	//m_pAvatarManager->PushAvatar(0);

	m_pNormalViewRoot = i3Node::new_object();

	SetSceneGraph(m_pNormalViewRoot);

	//m_CamFOV = GetCurrentCamera()->GetFOV();
	m_CamFOV = 70.f;	// 게임 디폴트
	GetCurrentCamera()->SetFOV(m_CamFOV);
}

void AvatarViewerFramework::OnDestroy(void)
{
	g_pOption->m_LightTheta		= m_LightTheta;
	g_pOption->m_LightRho		= m_LightRho;
	g_pOption->m_LightDist		= m_LightDist;
}

void AvatarViewerFramework::OnUpdate( REAL32 tm)
{
	//m_pAvatar->OnUpdate(tm);

	static INT32 nCnt = 0;
	static INT32 nLine = 0;
	if ( m_pAvatarManager && m_pAvatarManager->GetRecvCnt() > 0)
	{
		m_pAvatarManager->PushWriteCnt();
		Avatar * pAvatar = m_pAvatarManager->GetAvatar();
		if (pAvatar) 
		{
			if( m_pAvatar != NULL)
			{
				m_pNormalViewRoot->RemoveChild( m_pAvatar->getSceneObject());
				I3_SAFE_RELEASE( m_pAvatar);
			}

			g_pAvatar = m_pAvatar = pAvatar;
			m_pAvatar->LoadComplate();
			m_pNormalViewRoot->AddChild( m_pAvatar->getSceneObject());

			SetSceneGraph(m_pNormalViewRoot);
			
 			char szAnimationPath[ MAX_PATH];

			i3::safe_string_copy( szAnimationPath, i3ResourceFile::GetWorkingFolder(), sizeof( szAnimationPath));

			if( pAvatar->GetResType() == Avatar::eCHARA_TYPE_MALE)
			{
				i3::generic_string_cat( szAnimationPath, "/Chara/D-Fox/3PV/Common/UI_Ceremony.i3a");
			}
			else if( pAvatar->GetResType() == Avatar::eCHARA_TYPE_FEMALE)
			{
				i3::generic_string_cat( szAnimationPath, "/Chara/REBEL_Female/3PV/Common/UI_Ceremony.i3a");
			}
			else
			{
				i3::generic_string_cat( szAnimationPath, "/Chara/D-Fox/3PV/Common/UI_Ceremony.i3a");
			}

			LoadAnimation(pAvatar, szAnimationPath);

			TEAM_TYPE eType = TEAM_BLUE;
			g_pOption->m_eTeamColor == TEAM_BLUE ? eType = TEAM_BLUE : eType = TEAM_RED;

			CMainFrame* pMain = ((CMainFrame*)AfxGetApp()->GetMainWnd());
			if (pMain)
				pMain->SetTeamType(eType);
			g_pOption->m_eTeamColor = TEAM_NONE;

			GetCurrentCamera()->SetFOV( m_CamFOV);

		}
	}
	i3ViewerFramework::OnUpdate( tm);

 	if (GetAsyncKeyState(VK_F1))
	{
		m_pAvatarManager->PushAvatar(nCnt);
	}
	if (GetAsyncKeyState(VK_F2))
	{
		for (size_t i = 0; i < m_vAvatarList.size(); ++i)
		{
			//m_vAvatarList[i]->GetBody()->setCurrentLevel(0);
			//m_vAvatarList[i]->SetLOD(0);

		}
	}
	if (GetAsyncKeyState(VK_F3))
	{
		for (size_t i = 0; i < m_vAvatarList.size(); ++i)
		{
			//m_vAvatarList[i]->GetBody()->setCurrentLevel(1);
			//m_vAvatarList[i]->SetLOD(1);

		}
	}
	if (GetAsyncKeyState(VK_F4))
	{
		for (size_t i = 0; i < m_vAvatarList.size(); ++i)
		{
			//m_vAvatarList[i]->GetBody()->setCurrentLevel(2);
			//m_vAvatarList[i]->SetLOD(2);

		}
	}
	if (GetAsyncKeyState(VK_F5))
	{
		for (size_t i = 0; i < m_vAvatarList.size(); ++i)
		{
			//m_vAvatarList[i]->GetBody()->setCurrentLevel(3);
			//m_vAvatarList[i]->SetLOD(3);

		}
	}
}

void AvatarViewerFramework::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	i3ViewerFramework::OnMouseWheel( nFlag, zDelta, point);

	CMainFrame* pMain = ((CMainFrame*)AfxGetApp()->GetMainWnd());
	if( pMain->m_bCam1PVMode == false && m_pAvatar != NULL)
	{
		MATRIX * pCamMtx = GetCurrentCamera()->GetMatrix();
		REAL32 dist;
		
		dist = i3Vector::Length( i3Matrix::GetPos( pCamMtx));
		
		if( dist < 3.f)
		{
			m_pAvatar->SetLOD( 1);
		}
		else if( dist < 6.f)
		{
			m_pAvatar->SetLOD( 2);
		}
		else
		{
			m_pAvatar->SetLOD( 3);
		}
	}
}

void AvatarViewerFramework::_GetFullTransform( i3Node * pNode, MATRIX * pMtx)
{
	i3::vector<i3Node*> parentList;

	pNode = pNode->GetParent();

	while((pNode != NULL))
	{
		parentList.push_back( pNode);
		pNode = pNode->GetParent();
	}


	i3Body * pBody = NULL;

	i3Matrix::Identity( pMtx);

	for(size_t i = 0; i < parentList.size(); i++)
	{
		pNode = parentList[i];

		if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::GetClassMeta()))
		{
			i3Transform * pTrans = (i3Transform *) pNode;

			i3Matrix::Mul( pMtx, pMtx, pTrans->GetMatrix());
		}
		else if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::GetClassMeta()))
		{
		}
	}
}

void AvatarViewerFramework::SelectNode( i3Node * pNode)
{
	//i3Transform2::GetClassMeta()->IsTypeOf( i3RenderAttr::GetClassMeta());

	m_pSelNode = pNode;
}

void AvatarViewerFramework::SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize)
{
	i3Light * pLight = m_pLightAttr->GetLightObject();

	pLight->SetDiffuse( pDiff);
	pLight->SetSpecular( pSpec);
	pLight->setShadowMapSize( shadowSize);

	if( pLight->getStyle() & I3G_LIGHT_STYLE_SHADOWMAP )
	{
		pLight->CreateShadowMap();
	}
}

bool AvatarViewerFramework::CloseI3S(void)
{
	m_pSelNode = NULL;

	I3_SAFE_RELEASE( m_pNormalViewRoot);

	RefreshViewer();

	removeAllInitBone();

	return true;
}

void AvatarViewerFramework::RefineSg( bool bResetView)
{
	i3Node * pRoot = NULL;

	m_pSelNode = NULL;

	m_pInterLinkNode->RemoveFromParent();
	m_pInterLinkNode->RemoveAllChild();

	// CommonSg가 있는 경우, CommonSg.를 상위로, InstanceSg를 하위로 붙여주어야 올바른 Visual이 나온다.
	pRoot = m_pAvatar->getSceneObject();
	//GetPlayer3DRoot()->AddChild(m_pAvatar->getSceneObject());
	if( pRoot != NULL)
	{
		//pRoot->Instantiate();
	}

	// Loading된 Sg.의 Physics 상태를 Kinematic으로 설정.
	//SetDynamicState( false);

	//if (m_pNormalViewRoot)
	//	m_pTrans->RemoveChild(m_pNormalViewRoot);
	I3_REF_CHANGE( m_pNormalViewRoot, pRoot);

	//m_pNormalViewRoot = pRoot;

	SetSceneGraph(m_pNormalViewRoot);
	// Bone들의 초기값을 저장해둔다.
// 	BuildInitBone();
// 
// 	RefreshViewer();
// 
// 	UpdateBound();

	//LoadAnimation("D:\\pb15\\PointBlank_15\\Media\\Chara\\D-Fox\\3PV\\Common\\UI_Ceremony.i3a");
}

bool AvatarViewerFramework::SaveI3S( const char * pszPath)
{
	i3SceneFile file;
	i3SceneGraphInfo * pSgInfo = i3SceneGraphInfo::new_object();

	pSgInfo->setInstanceSg( m_pAvatar->getSceneObject());

	file.setSceneGraphInfo( pSgInfo);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		pSgInfo->Release();
		return false;
	}

	pSgInfo->Release();

	return true;
}

void AvatarViewerFramework::SaveProject( const char * pszPath)
{
	i3IniParser parser;

	if( parser.Create( pszPath) == FALSE)
	{
		I3PRINTLOG(I3LOG_WARN, "Couldn't save file: %s\n", pszPath);
		return;
	}

	parser.WriteSectionName("PartsList");
	//parser.WriteValue( "SectorCount", (INT32)getCount());

	char conv[MAX_PATH];
	for( INT32 i = 0 ; i < EQUIP::ePART_COUNT; i++)
	{
		sprintf( conv, "%s", AVATAR::getPartName((EQUIP::ePART)i));

		parser.WriteValue( conv, g_pAvatar->getPartName( (EQUIP::ePART) i).c_str() );
	}

	parser.Close();
}

void AvatarViewerFramework::LoadProject( const char * pszPath)
{
	i3IniParser parser;

	if( parser.OpenFromFile( pszPath) == FALSE)
	{
		I3PRINTLOG(I3LOG_WARN, "Couldn't load file: %s\n", pszPath);
		return;
	}

	parser.ReadSection("PartsList");
	//parser.WriteValue( "SectorCount", (INT32)getCount());

	char conv[MAX_PATH];
	char conv2[MAX_PATH];
	vPartsList list;

	for( INT32 i = 0 ; i < EQUIP::ePART_COUNT; i++)
	{
		sprintf( conv, "%s", AVATAR::getPartName((EQUIP::ePART)i));
		parser.GetString( conv, conv2, sizeof(conv) );

		i3::string str( conv2);

		list.push_back( str);
	}

	parser.Close();

	ReloadParts( list, m_Team);
}

bool	sFindPartsResFolder( char * pszOut, const char * pszMainFolder, const char * pszPartName, const char Filter)
{
	::BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
	if (FALSE == bDir)
	{
		i3ResourceFile::SetWorkingFolder("");
		return false;
	}

	bool bFind = false;
	char szSearchFolder[ MAX_PATH];

	i3::safe_string_copy( szSearchFolder, i3ResourceFile::GetWorkingFolder(), sizeof( szSearchFolder));
	i3::generic_string_cat( szSearchFolder, "/");
	i3::generic_string_cat( szSearchFolder, pszMainFolder);
	i3::generic_string_cat( szSearchFolder, "/");
	i3::generic_string_cat( szSearchFolder, pszPartName );
	// 파츠에서 아무것이라 찾아서 설정하자.
	i3::generic_string_cat( szSearchFolder, "/*.*");

	BOOL bF = TRUE;
	WIN32_FIND_DATA findData;
	HANDLE h = ::FindFirstFile( szSearchFolder, &findData);

	while( bF)
	{
		bF = ::FindNextFile( h, &findData);

		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if( i3::contain_string( findData.cFileName, ".svn") >= 0 ||
				i3::contain_string( findData.cFileName, ".") >= 0 ||
				i3::contain_string( findData.cFileName, "..") >= 0 ||
				i3::contain_string( findData.cFileName, "Texture") >= 0 ||
				i3::contain_string( findData.cFileName, "Res") >= 0 )
			{
				continue;
			}

			if( findData.cFileName[0] == Filter)
			{
				i3::safe_string_copy( pszOut, findData.cFileName, MAX_PATH);
				bFind = true;
				break;
			}
		}
	}

	::FindClose( h);

	return bFind;
}


Avatar * AvatarViewerFramework::LoadPart( const char * pszPath, TEAM_TYPE team, bool bVisible)
{
	::BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
	if (FALSE == bDir)
	{
		i3ResourceFile::SetWorkingFolder("");
		return NULL;
	}
	i3::vector<i3::rc_string> list;
	EQUIP::ePART part = EQUIP::ePART_UNKNOWN;
	const char * pszStr;
	INT32 i;
	char szRelPath[ MAX_PATH];

	i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), pszPath, szRelPath);
	i3::resolve_path_token( szRelPath, list);

	if( list.size() <= 0)
		return NULL;

	// Avatar Path 확인
	pszStr = list[0].c_str();
	if( i3::generic_is_iequal( pszStr, "Avatar") == false)
	{
		AfxMessageBox( "올바른 아바타 파츠가 아닙니다.");
		return NULL;
	}

	// Part 확인
	pszStr = list[1].c_str();
	for( i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		if( list[1] == AVATAR::getPartName( (EQUIP::ePART) i))
		{
			part = (EQUIP::ePART) i;
			break;
		}
	}

	if( part == EQUIP::ePART_UNKNOWN)
	{
		AfxMessageBox( "파츠의 종류를 확인할 수 없는 경로입니다.");
		return NULL;
	}

	char szName[MAX_PATH];

	pszStr = list.back().c_str();

	i3::safe_string_copy( szName, pszStr, _countof( szName));
	i3::remove_ext( szName);

	// 마지막의 _LOD# Postfix 제거
	INT32 pos = i3::contain_string(szName,"_LOD");

	char szChangePartsName[256] = {0,};

	if (pos != -1)
		i3::substr(szChangePartsName, szName,0,pos);
	else
		i3::safe_string_copy( szChangePartsName, szName, sizeof( szChangePartsName));

	// Part 이름 분리
	if( (AVATAR::getPartStyle( part) & AVT_PART_STYLE_BODY) > 0)
	{	
		vPartsList partlist;

		CMainFrame * pMainFrm = (CMainFrame*)(::AfxGetMainWnd());
		CEquipData * pEquipData = pMainFrm->m_Panel_Part.GetEquipData();

		i3::string str;

		// 이전에 사용된 파츠를 제사용
		if( m_pAvatar != NULL)
		{
			if( part < EQUIP::ePART_CHARA)
				m_pAvatar->setPartName( EQUIP::ePART_SKIN, "");

			for (int i = 0; i < EQUIP::ePART_COUNT; ++i)
			{
				if( m_pAvatar->getPartName((EQUIP::ePART)i).size() > 0)
					str = m_pAvatar->getPartName((EQUIP::ePART)i);
				else
				{
					if( i != EQUIP::ePART_SKIN &&
						sFindPartsResFolder( szName, list[0].c_str(), AVATAR::getPartName( (EQUIP::ePART) i).c_str(), szChangePartsName[0] ) )
					{
						str = szName;
					}
					else
						str = "";
				}

				partlist.push_back( str);
			}
		}
		else
		{
			// 기본이 설정되지 않았다면 만들어 주어야한다.

			for (int i = 0; i < EQUIP::ePART_COUNT; ++i)
			{
				if( i != EQUIP::ePART_SKIN && 
					(AVATAR::getPartStyle( (EQUIP::ePART)i) & AVT_PART_STYLE_BODY) > 0)
				{	
					if( sFindPartsResFolder( szName, list[0].c_str(), AVATAR::getPartName( (EQUIP::ePART) i).c_str(), szChangePartsName[0] ) )
					{
						const char * pszTeamPart = pEquipData->GetTeamPart( szName, pMainFrm->GetTeamType());
						if( pszTeamPart != NULL)
							str = pszTeamPart;
						else
							str = szName;
					}
					else
						str = "";
				}
				else
					str = "";

				partlist.push_back( str);
			}
		}

		if( i3::generic_string_size( szChangePartsName) > 0 && bVisible)
		{
			partlist[(INT32)part] = szChangePartsName;

			for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
			{
				if( i == part)
					continue;

				if( szChangePartsName[0] == 'M')
				{
					if( partlist[ i].c_str()[0] == 'F')
					{
						if( sFindPartsResFolder( szName, list[0].c_str(), AVATAR::getPartName( (EQUIP::ePART) i).c_str(), 'M' ) )
						{
							const char * pszTeamPart = pEquipData->GetTeamPart( szName, pMainFrm->GetTeamType());
							if( pszTeamPart != NULL)
								partlist[ i] = pszTeamPart;
							else
								partlist[ i] = szName;
						}
					}
				}
				else if( szChangePartsName[0] == 'F')
				{
					if( partlist[ i].c_str()[0] == 'M')
					{
						if( sFindPartsResFolder( szName, list[0].c_str(), AVATAR::getPartName( (EQUIP::ePART) i).c_str(), 'F' ))
						{
							const char * pszTeamPart = pEquipData->GetTeamPart( szName, pMainFrm->GetTeamType());
							if( pszTeamPart != NULL)
								partlist[ i] = pszTeamPart;
							else
								partlist[ i] = szName;
						}
					}
				}
			}
		}
		else
		{
			partlist[(INT32)part] = "";
		}

		return m_pAvatarManager->PushAvatar(0, partlist, team);
	}
	else
	{
		// 장비입니다.
		if( m_pAvatar != NULL)
		{
			m_pAvatar->setPartName( part, szChangePartsName);
		}
	}

	return NULL;
}

Avatar * AvatarViewerFramework::ReloadParts( vPartsList &partlist, TEAM_TYPE team)
{
	//if( m_pAvatar != NULL)
	{
		return m_pAvatarManager->PushAvatar(0, partlist, team);
	}

	return NULL;
}

bool	AvatarViewerFramework::LoadAnimation(Avatar* pAvatar, const char * pszPath)
{
	i3Animation * pAnim = i3Animation::LoadFromFile( pszPath);

	if( pAnim == NULL)
		return false;

	if( pAvatar == NULL)
		pAvatar = m_pAvatar;

	if( pAvatar == NULL)
	{
		I3_SAFE_RELEASE( pAnim);
		return false;
	}

	i3Node * pRoot = pAvatar->getSceneObject();

	i3Body * pBody = (i3Body*) i3Scene::FindNodeByExactType( pRoot, i3Body::static_meta());

	if( pBody == NULL)
		return false;

	pAnim->AddRef();
	pBody->PlayAnim( pAnim);

	I3_SAFE_RELEASE( pAnim);

	return true;
}

void AvatarViewerFramework::ApplyOptionColors(void)
{
	i3RenderContext * pCtx = GetViewer()->GetRenderContext();

	// Background Color
	{
		COLOR col;

		i3Color::Set( &col, & g_pOption->m_BackColor);

		pCtx->SetClearColor( &col);
	}

	// Ambient Color
	{
		getSgContext()->setAmbient( & g_pOption->m_Ambient);
	}

	// Light
	{
		COLORREAL	color;

		i3Color::Scale( &color, & g_pOption->m_LightDiffuse, g_pOption->m_LightIntensity);
		m_pLightAttr->setDiffuse( &color);

		i3Color::Scale( &color, & g_pOption->m_LightSpecular, g_pOption->m_LightIntensity);
		m_pLightAttr->setSpecular( &color);

		// Transform
		m_LightTheta	= g_pOption->m_LightTheta;
		m_LightRho		= g_pOption->m_LightRho;
		m_LightDist		= g_pOption->m_LightDist;

		UpdateLightTx();
	}
}

bool AvatarViewerFramework::ApplyOptionReflectMap(void)
{
	i3TextureCube * pTex;
	i3ImageFile file;
	char szFull[ MAX_PATH];

	if( g_pOption->m_szReflectMap[0] == 0)
	{
		i3::string str = "$(I3_ROOT)/Bin/res";
		i3::string szPath;

		i3::resolve_path_from_environment_variable( str, szPath);

		sprintf( szFull, "%s/defl_cubemap.dds", szPath.c_str());

		if( i3System::IsFile( szFull) == FALSE)
		{
			i3::get_exec_path((char*) szPath.c_str());
			sprintf( szFull, "%s/res/defl_cubemap.dds", szPath.c_str());
		}
	}
	else
	{
		i3::safe_string_copy( szFull, g_pOption->m_szReflectMap, MAX_PATH );
	}

	pTex = (i3TextureCube *) file.Load( szFull);

	if( pTex != NULL)
	{
		m_pReflectMapAttr->SetTexture( pTex);

		pTex->Release();
	}
	else
	{
		NotifyBox( NULL, "%s 파일을 읽어들이지 못했습니다. 파일에 문제가 있거나, 지원하지 않는 형식(HDRI 포함)의 이미지일 수 있습니다.", szFull);
		return false;
	}

	return true;
}

void AvatarViewerFramework::ApplyOptionShadow(void)
{
	i3Light * pLight = m_pLightAttr->GetLightObject();

	if( g_pOption->m_bShadowEnable)
	{
		// Shadow On
		pLight->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);

		if( pLight->getShadowMapSize() != g_pOption->m_ShadowSize)
			pLight->setShadowMapSize( g_pOption->m_ShadowSize);		// ShadowMap Size
	}
	else
	{
		// Shadow Off
		pLight->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);
	}
}

void AvatarViewerFramework::ApplyOption(void)
{
	// Option에 맞추어 필요한 설정을 한다.
	{
		ApplyOptionColors();

		// Shadow
		ApplyOptionShadow();
	}

	// Grid
	SetGridShow( g_pOption->m_bGridShow == TRUE);

	ApplyOptionReflectMap();

	// Texture quality
	this->GetViewer()->GetRenderContext()->getGfxOption()->setTextureQuality( 0);
}

void AvatarViewerFramework::setEditMode( I3_VIEWER_EDIT_MODE mode)
{
	m_EditMode = mode;

	switch( mode)
	{
		case I3_VIEWER_EDIT_MODE_NORMAL :
			break;

		case I3_VIEWER_EDIT_MODE_LIGHT :
			break;
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, g_pOption);
}

typedef struct _tagIsolateInfo
{
	i3Node *		m_pNode;
	bool			m_bStatus;
} ISOLATE_INFO;

static I3SG_TRAVERSAL_RESULT _IsolateNodeProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	ISOLATE_INFO * pInfo = (ISOLATE_INFO *) pData;

	bool bVisible = ! pInfo->m_bStatus;

	// 부모 노드에서 대상 Node가 있는지 확인.
	{
		i3Node * pParent = pNode;

		while( pParent != NULL)
		{
			if( pParent == pInfo->m_pNode)
			{
				bVisible = true;
				break;
			}

			pParent = pParent->GetParent();
		}
	}

	if( bVisible)
		pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
	else
		pNode->AddFlag( I3_NODEFLAG_DISABLE);

	return I3SG_TRAVERSAL_CONTINUE;
}

void AvatarViewerFramework::_SetIsolateMode( bool bFlag)
{
	if( m_pNormalViewRoot == NULL)
		return;

	ISOLATE_INFO info;

	info.m_pNode = m_pSelNode;
	info.m_bStatus = bFlag;

	i3Scene::Traverse( m_pNormalViewRoot, _IsolateNodeProc, &info);

	// 부모부터 상위 Root까지 모두 Enable
	i3Node * pTemp = m_pSelNode;
	
	while( pTemp != NULL)
	{
		pTemp->RemoveFlag( I3_NODEFLAG_DISABLE);

		pTemp = pTemp->GetParent();
	}
}

void AvatarViewerFramework::RefreshViewer(void)
{
	_SetIsolateMode( false);

	//GetPlayer3DRoot()->AddChild(m_pNormalViewRoot);]
// 	if (m_pTrans == NULL)
// 	{
// 		
// 		m_pTrans = i3Transform::NewObject();
// 		SetSceneGraph(m_pTrans);
// 	}
// 
// 	m_pTrans->RemoveAllChild();
// 	m_pTrans->AddChild(m_pNormalViewRoot);
// 
// 	MATRIX matRes;
// 	i3Matrix::Identity(&matRes);
// 	MATRIX mat;
// 	i3Matrix::SetRotateX(&mat,I3_DEG2RAD(-90.0f));
// 	i3Matrix::Mul(&matRes,&matRes,&mat);
// 
// 	i3Matrix::SetRotateY(&mat,I3_DEG2RAD(-180.0f));
// 	i3Matrix::Mul(&matRes,&matRes,&mat);
// 
// 	m_pTrans->SetMatrix(&matRes);
	SetSceneGraph(m_pNormalViewRoot);
	

	i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, g_pOption);
}

void AvatarViewerFramework::SetGlowEnable( BOOL bFlag)
{
	/*if( isEnable_DownScale() != bFlag) 
	{
		ToggleRasterizeChain_DownScale();
	}

	if( isEnable_BrightPassExtract() != bFlag) 
	{
		ToggleRasterizeChain_BrightPassExtract();
	}

	if( isEnable_GaussianBlur() != bFlag) 
	{
		ToggleRasterizeChain_GaussianBlur();
	}

	if( isEnable_BloomCombine() != bFlag) 
	{
		ToggleRasterizeChain_BloomCombine();
	}*/
}

INIT_BONE_INFO * AvatarViewerFramework::addInitBone( const char * pszBone, MATRIX * pMatrix)
{
	INIT_BONE_INFO * pInfo;

	pInfo = new INIT_BONE_INFO;

	i3::safe_string_copy( pInfo->m_szName, pszBone, 256 );
	i3Matrix::Copy( &pInfo->m_Matrix, pMatrix);

	m_InitBoneList.push_back( pInfo);

	return pInfo;
}

INIT_BONE_INFO * AvatarViewerFramework::findInitBone( const char * pszBone)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		if( i3::generic_is_iequal( pInfo->m_szName, pszBone) )
			return pInfo;
	}

	return NULL;
}

void AvatarViewerFramework::removeAllInitBone(void)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		delete pInfo;
	}

	m_InitBoneList.clear();
}

void AvatarViewerFramework::BuildInitBone(void)
{
	// Sg.를 돌면서 Bone으로 간주되는 노드들의 초기 Matrix를 저장해 둔다.
	removeAllInitBone();

	if( m_pNormalViewRoot != NULL)
	{
		i3Scene::Traverse( m_pNormalViewRoot, _BuildInitBoneProc, this);
	} 
}

I3SG_TRAVERSAL_RESULT AvatarViewerFramework::_BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	AvatarViewerFramework * pFramework = (AvatarViewerFramework *) pUserData;

	if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::GetClassMeta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				pFramework->addInitBone( pSkel->getName( i), pSkel->getMatrix( i));
			}
		}
	}
	else if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		pFramework->addInitBone( pTrans->GetName(), pTrans->GetMatrix());
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void AvatarViewerFramework::ApplyInitBone(void)
{
	// // Sg.를 돌면서 Bone으로 간주되는 노드들오 초기 Matrix를 설정해준다.
	if( m_pNormalViewRoot != NULL)
		i3Scene::Traverse( m_pNormalViewRoot, _ApplyInitBoneProc, this);
}

I3SG_TRAVERSAL_RESULT AvatarViewerFramework::_ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	AvatarViewerFramework * pFramework = (AvatarViewerFramework *) pUserData;

	if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::GetClassMeta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				INIT_BONE_INFO * pInfo = pFramework->findInitBone( pSkel->getName( i));

				if( pInfo != NULL)
					pSkel->setMatrix( i, &pInfo->m_Matrix);
			}
		}
	}
	else if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		INIT_BONE_INFO * pInfo = pFramework->findInitBone( pTrans->GetName());

		if( pInfo != NULL)
			pTrans->SetMatrix( &pInfo->m_Matrix);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void AvatarViewerFramework::SetDynamicState( bool bFlag)
{
	if( m_pNormalViewRoot == NULL)
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

	i3Scene::ModifyDynamicState( m_pNormalViewRoot, addState, remState);
	i3Scene::SetDynamicVelocity( m_pNormalViewRoot, NULL, &vel, &vel);
}

void AvatarViewerFramework::TransformPart( const EQUIP::ePART ePart, const VEC4D& vTransform )
{
	if( NULL != m_pAvatar )
		m_pAvatar->TransformPart( ePart, vTransform );
}

bool AvatarViewerFramework::ChangeHeadPart( const INT32& headType, const EQUIP::ePART ePart, const VEC4D& vTransform, const char * pszPath)
{
	if( NULL != m_pAvatar )
	{
		Avatar * pNewAvatar = m_pAvatar->ChangeHeadPart( headType );
		if( pNewAvatar != NULL)
		{
			if( pszPath != NULL && i3::generic_string_size( pszPath) > 0)
				pNewAvatar->SetLoadComplateProc( NULL, ePart, &vTransform, pszPath);
			else
				pNewAvatar->SetLoadComplateProc( NULL, ePart, &vTransform, m_pAvatar->getLoadComplateParts());
		}
		else
		{
			if( pszPath != NULL && i3::generic_string_size( pszPath) > 0)
				LoadPart( pszPath, m_Team);
			else
				LoadPart( m_pAvatar->getLoadComplateParts(), m_Team);

			m_pAvatar->TransformPart( ePart, vTransform);
		}
		return true;
	}

	return false;
}