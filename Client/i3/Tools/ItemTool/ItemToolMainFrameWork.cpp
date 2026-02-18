#include "StdAfx.h"
#include "ItemToolMainFrameWork.h"
#include "UI/UIMain.h"
#include "MainFrm.h"
#include "i3Base/string/ext/extract_filename.h"

I3_CLASS_INSTANCE(cItemToolMainFrameWork); //, cItemToolFrameWork)

cItemToolMainFrameWork::cItemToolMainFrameWork(void)
:	m_pUIMain(NULL)
,	m_pSceneGraphInfo(NULL)
{
}

cItemToolMainFrameWork::~cItemToolMainFrameWork(void)
{
	I3_SAFE_RELEASE(m_pUIMain);
}



 
 void cItemToolMainFrameWork::OnCreate( void )
 {
 	cItemToolFrameWork::OnCreate();
 
 	i3Camera* pCamera = GetDefaultCamera();
 	pCamera->SetFar(50000);
 	pCamera->SetNear(10);
 	m_pUIMain	= cUIMain::new_object();
 	m_pUIMain->Create( Get2DRoot() );
 }
 
 BOOL cItemToolMainFrameWork::Create( i3Viewer * pViewer)
 {
	 I3ASSERT( pViewer != NULL);

	 // Scene Graph
	 InitSceneGraph();

	 // Viewer
	 {
		// pViewer->SetFramework( this);

		 m_pViewer = pViewer;

		 //m_pViewer->SetSceneGraphContext( m_pSgContext);

		 //CHAR_SOLO_RENDER
		// m_pViewer->SetPlayerSceneGraphContext( m_pPlayerSgContext );

		 {
			 COLOR col;

			 i3Color::Set( &col, (UINT8) 0, 0, 0, 255);
			 m_pViewer->GetRenderContext()->SetClearColor( &col);
		 }
	 }

	 // Default 3D Camera
	 {
		 m_p3DCamera = i3Camera::new_object();
		 m_p3DCamera->SetEnable( TRUE);

		 //Get3DRoot()->AddChild( m_p3DCamera);
		 m_pSgContext->setCurrentCamera( m_p3DCamera);
	}

	 // Resource Manager
	 {
		 m_pResManager = i3ResourceManager::new_object();
		 I3ASSERT( m_pResManager != NULL);

		 m_pResManager->BindFramework( this);

		 AddChild( m_pResManager);
	 }

	 // Effect Manager
	 {
		 m_pEffectManager = i3EffectManager::new_object();
		 I3ASSERT( m_pEffectManager != NULL);

		 m_pEffectManager->BindFramework( this);

		 // 기본 초기화
		 m_pEffectManager->Create();

		 AddChild( m_pEffectManager);
	 }

// 	 {
// 		 m_pUIManager = i3UIManager::new_object();
// 
// 		 m_pUIManager->Create( this);
// 		 m_pUIManager->SetViewer( pViewer);
// 
// 		 AddChild( m_pUIManager);
// 
// 		 setInputDispatchManager( m_pUIManager);
// 	 }
// 
// 	 _CreateUI(true);

	 BuildSceneStruct();

	 OnCreate();

	 return TRUE;
 }


const REAL32	RateRenderView	= 0.3f;
const REAL32	RateRenderUI	= 1.0f - RateRenderView;

void cItemToolMainFrameWork::OnSize( POINT2D * pSize )
{
	cItemToolFrameWork::OnSize(pSize);

	if( pSize && m_pUIMain )
		m_pUIMain->SetSize(*pSize);
}


void cItemToolMainFrameWork::OnLButtonDown( UINT32 nFlag, POINT point )
{
	if( m_pUIMain )
		m_pUIMain->OnLButtonDown(point);
}


void cItemToolMainFrameWork::OnMouseWheel( UINT nFlag, short zDelta, POINT point )
{
	cItemToolFrameWork::OnMouseWheel( nFlag, zDelta, point );

	CMainFrame*		pFrame	= AfxGetMainFrame();

	if( pFrame )
	{
		cMainRenderView&	View	= pFrame->GetMainRenderView();

		View.AddScrollBarPos( -(zDelta / 120) );
	}
}

void cItemToolMainFrameWork::OnUpdate( RT_REAL32 timeDelta)
{
	cItemToolFrameWork::OnUpdate(timeDelta);
	if (m_pUIMain)
		m_pUIMain->OnUpdate(timeDelta);
}

bool cItemToolMainFrameWork::LoadFile( LPCSTR pszPath, LPCSTR pszExt )
{
	if( pszPath == NULL || pszExt == NULL )
		return false;


	if( i3::generic_is_iequal(pszExt, "i3s")  )
	{
		return _LoadI3S( pszPath );
	}
	else if( i3::generic_is_iequal(pszExt, "i3uie")  
		|| i3::generic_is_iequal(pszExt, "i3uis")  )
	{
		return _LoadI3UI( pszPath );		
	}
	else if(i3::generic_is_iequal(pszExt, "i3i") )
	{
		if( m_pUIMain )
			return m_pUIMain->LoadFile( pszPath );
	}

	return false;
}


bool cItemToolMainFrameWork::LoadTexture(LPCSTR pPath, const int& nPef)
{
	if( pPath == NULL || m_pUIMain == NULL )
		return false;

	ePefType pef = (ePefType)nPef;
	switch(pef)
	{
	case E_PEF_WEAPON:
		{
			LoadCurrentPefTexture(STR_GUI_WEAPONSHAPE_PATH, "Weapon_Select", UI_TEX_TYPE_WEAPON);
		}
		break;
	case E_PEF_GOODS:
		{
			LoadCurrentPefTexture(STR_GUI_ITEM_PATH, "item_cash_A", UI_TEX_TYPE_ITEM_COUNT);
			LoadCurrentPefTexture(STR_GUI_ITEM_PATH, "item_cash_B", UI_TEX_TYPE_ITEM_PERIOD);
		}
		break;
	case E_PEF_EQUIPMENT:
		{
			// version 1.0 or 1.1 
			LoadCurrentPefTexture(STR_GUI_ITEM_PATH, "item_Beret", UI_TEX_TYPE_BERET);
			LoadCurrentPefTexture(STR_GUI_ITEM_PATH, "item_head", UI_TEX_TYPE_HEAD);
			LoadCurrentPefTexture(STR_GUI_ITEM_PATH, "item_mask", UI_TEX_TYPE_MASK);
		}
		break;
	case E_PEF_CHARA:
		{
			LoadCurrentPefTexture(STR_GUI_ITEM_PATH, "item_cha", UI_TEX_TYPE_CHARA);
		}
		break;
	}

	return true;
}

void cItemToolMainFrameWork::LoadCurrentPefTexture(const CString& strPath, const CString& strKey, UI_TEXTURE_TYPE textype)
{
	std::vector<CString> vDestFileList;
	GUTIL::GetFileListFromFolder(vDestFileList, strPath, strKey);

	for (UINT32 ui = 0; ui < vDestFileList.size(); ++ui)
	{
		CString strFullPath = strPath + "\\" + vDestFileList[ui];
		i3Texture*	pTexture = GetResourceManager()->LoadTexture(strFullPath);

		if( pTexture == NULL )
		{
			CString		strMsg("텍스쳐 로딩 실패\n");

			strMsg		+= "TexPath";
			AfxMessageBox(strMsg);
		}
		else
			m_pUIMain->SetUITexture( textype, ui, pTexture );
	}
}



bool cItemToolMainFrameWork::_LoadI3S( LPCSTR pszPath)
{
	i3SceneFile file;

	i3::make_unix_path(const_cast<char*>(pszPath));

	//media위치를 기준으로 로드될 파일의 폴더를 알아낸다.
	std::string strPath, strFolder;
	strPath = pszPath;
	INT32 nPos = strPath.rfind("Media");
	strFolder = strPath.substr(nPos+6,strPath.length());
	nPos = strFolder.find("/");
	strFolder = strFolder.substr(0,nPos);

	if( file.Load( pszPath) == STREAM_ERR)
	{
		return false;
	}

	i3SceneGraphInfo * pSgInfo = file.getSceneGraphInfo();
	
	m_pSceneGraphInfo = pSgInfo->getInstanceSg();

#define		OUTPUT_RENDERTARGET
#ifdef		OUTPUT_RENDERTARGET
	//if( m_pUIMain )
	//	m_pUIMain->Set3DView(pSgInfo);

	i3Node*		pResource	= NULL;

	if( pSgInfo != NULL)
	{
		if( pSgInfo->getCommonSg() != NULL)
		{
			if( pSgInfo->getInstanceSg() != NULL)
			{
				i3Node * pAttachNode = pSgInfo->getCommonSg();

				while( pAttachNode->GetChildCount() > 0)
					pAttachNode = pAttachNode->GetChild( pAttachNode->GetChildCount() - 1);
			}
			pResource = pSgInfo->getCommonSg();
		}
		else
		{
			pResource = pSgInfo->getInstanceSg();
		}
	}

	if( pResource != NULL)
		pResource->Instantiate();

	UINT32 addState, remState;

	addState = remState = 0;

//  	if(strFolder == "Chara")
//  	{
//  		remState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
//  	}
//  	else
 	{
		addState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}

	VEC3D vel;

	i3Vector::Set( &vel, 0.0f, 0.0f, 0.0f);

	i3Scene::ModifyDynamicState( pResource, addState, remState);
	i3Scene::SetDynamicVelocity( pResource, NULL, &vel, &vel);

	i3Transform* pTrans = i3Transform::new_object_ref();
	pTrans->AddChild(pResource);

	VEC3D vec;
	vec.x = 0.0f; vec.y = 0.0f, vec.z = 0.0f;

	if (strFolder == "Weapon")
	{
		SetMatrixMesh(pTrans, 0.0f, 0.0f, -90.0f,vec);
	}
	else if (strFolder == "Chara")
	{
		vec.x = 0.0f; vec.y = -0.8f, vec.z = -0.2f;

		SetMatrixMesh(pTrans, 0.0f, 0.0f, 0.0f,vec);
	}
	else if (strFolder == "Equip")
	{
		vec.x = 0.0f; vec.y = 0.1f, vec.z = -0.3f;

		SetMatrixMesh(pTrans, 0.0f, -90.0f, -180.0f,vec);
	}

	g_pItemSCFramework->SetSceneGraph(pTrans);

#else
	I3_REF_CHANGE( m_pSgInfo, pSgInfo);
	RefineSg( true);
#endif

	//SetGlowEnable( g_pOption->m_bGlow);

	RESET_PROFILE();
	REPORT_PROFILE();

	return true;
}
void cItemToolMainFrameWork::SetMatrixMesh(i3Transform* pTrans, float fX, float fY, float fZ, VEC3D vPos)
{
	MATRIX mat, matRes;
	i3Matrix::Identity(&mat);
	i3Matrix::Identity(&matRes);

	i3Matrix::SetRotateX(&mat,I3_DEG2RAD(fX));
	i3Matrix::Mul(&matRes,&matRes,&mat);

	i3Matrix::SetRotateX(&mat,I3_DEG2RAD(fY));
	i3Matrix::Mul(&matRes,&matRes,&mat);

	i3Matrix::SetRotateY(&mat,I3_DEG2RAD(fZ));
	i3Matrix::Mul(&matRes,&matRes,&mat);

	i3Matrix::Identity(&mat);
	i3Matrix::SetPos(&mat,&vPos);
	i3Matrix::Mul(&matRes,&matRes,&mat);

	pTrans->SetMatrix(&matRes);
}

void cItemToolMainFrameWork::SetBeretTexture(const std::string& strDiffuse, const std::string& strNormal, const std::string strSpecular,
											 bool bforced,
											 const std::string& strSrcDiffuse, const std::string& strSrcNormal, const std::string strSrcSpecular)

{
	if (NULL == m_pSceneGraphInfo)
		return;

//	i3TextureBindAttr* pTextureAttr = NULL;

	if (i3::kind_of<i3Body*>(m_pSceneGraphInfo))
	{
		i3LOD* pLOD = ((i3Body*)m_pSceneGraphInfo)->getLOD(0);
		if (pLOD)
		{
			i3::vector<i3Node*>	AttrSetList;
			i3Scene::FindNodeByExactType(pLOD->getShapeNode(), i3AttrSet::static_meta(), AttrSetList, true);
			for ( size_t i = 0 ; i < AttrSetList.size() ; ++i )
			{
//				char szTextureFIle[256] = "";
//				i3Texture * pTex = NULL;
				i3AttrSet *	pAttrSet = (i3AttrSet *)AttrSetList[i];
				
				if (!strDiffuse.empty())
					SetChangeTexture(pAttrSet, i3TextureBindAttr::static_meta(),		strSrcDiffuse,	strDiffuse,		bforced);
				if (!strNormal.empty())
					SetChangeTexture(pAttrSet, i3NormalMapBindAttr::static_meta(),		strSrcNormal,	strNormal,		bforced);
				if (!strSpecular.empty())
					SetChangeTexture(pAttrSet, i3SpecularMapBindAttr::static_meta(),	strSrcSpecular, strSpecular,	bforced);

				
			}
		}
	}
}

void cItemToolMainFrameWork::SetChangeTexture(i3AttrSet*	pAttrSet,i3ClassMeta* pMeta, std::string szSrcTexture, std::string szDestTexture, bool bForced )
{
	i3Texture * pTex = NULL;
	i3TextureBindAttr* pTextureAttr = (i3TextureBindAttr*)i3Scene::FindAttrByExactType(pAttrSet, pMeta, true);
	
	if ( pTextureAttr )
	{
		if (NULL != pTextureAttr->GetTexture())
		{
			char Name[256] = {};
		//	i3String::SplitFileName(pTextureAttr->GetTexture()->getResourceLoadPath(), Name, TRUE);
			i3::extract_filename(pTextureAttr->GetTexture()->getResourceLoadPath(), Name);

			if (Name == szSrcTexture  && !szDestTexture.empty() || bForced == true)
			{
				pTex = g_pMainSCFramework->GetResourceManager()->LoadTexture( szDestTexture.c_str());
				if(pTex)
				{
					pTextureAttr->SetTexture( pTex );
					pTex->Release();
				}
			}
		}
	}
}

bool cItemToolMainFrameWork::_LoadI3UI( LPCSTR pszPath )
{
	/*I3_SAFE_RELEASE( m_pUI );

	m_pUI	= cUIBase::new_object();

	if( m_pUI )
	{
		m_pUI->OnCreate( this );
		m_pUI->AddScene( pszPath );
		m_pUI->SetEnable(TRUE);

		return true;
	}*/

	return false;
}


void cItemToolMainFrameWork::ResetItemList( INT32 nStartIndex )
{
	if(m_pUIMain)
		m_pUIMain->ResetItemList(nStartIndex);
}

void cItemToolMainFrameWork::ResizeUI( void )
{
	if(m_pUIMain)
		m_pUIMain->ResizeUI();
}


INT32 cItemToolMainFrameWork::GetScrollCount( void ) const
{
	if(m_pUIMain)
		return m_pUIMain->GetScrollCount();

	return 0;
}

INT32 cItemToolMainFrameWork::GetColPage( void ) const
{
	if(m_pUIMain)
		return m_pUIMain->GetColPage();

	return 0;
}

