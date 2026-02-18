// i3TDKDlgTextureTool.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgTextureTool.h"
#include ".\i3tdkdlgtexturetool.h"
#include "i3TDK_Dlg_SelectImageExt.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/make_relative_path.h"

#include "i3Base/string/system/is_file.h"

// i3TDKDlgTextureTool dialog

IMPLEMENT_DYNAMIC(i3TDKDlgTextureTool, CDialog)

void i3TDKDlgTextureTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_TEX_LIST, m_TexListCtrl);
	DDX_Control(pDX, IDC_TDK_ST_TOTALCNT, m_TotalCountCtrl);
	DDX_Control(pDX, IDC_TDK_ED_WORKDIR, m_ED_WorkDir);
	DDX_Control(pDX, IDC_TDK_ST_IMAGEVIEWPH, m_ST_ImageViewPH);
}

bool i3TDKDlgTextureTool::Execute( i3Node * pNode)
{
	m_pRoot = pNode;

	i3TDK::SetResInstance();

	INT32 rv = DoModal();

	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL)
		return false;

	return true;
}


class TexData
{
	UINT32 ObjectID;
	i3Texture *pTexture;

	TexData()
	{
		ObjectID = 0;
		pTexture = nullptr;
	}
};

struct TEX_REPLACE_INFO
{
	i3TDKDlgTextureTool *		_pDlg = nullptr;
	i3::vector<i3Texture*>*		_pNewTexList = nullptr;
	i3::vector<i3Texture*>*		_pOldTexList = nullptr;
} ;

void i3TDKDlgTextureTool::TraceSceneTextures( void *pUserData, TRACE_TEX_CB CBFunc )
{
	TEX_REPLACE_INFO * pInfo = (TEX_REPLACE_INFO*) pUserData;
	i3TDKDlgTextureTool * pDlg = (i3TDKDlgTextureTool *) pInfo->_pDlg;


	i3::vector<i3ElementBase*> ElementList;
//	ElementList.SetOnceMode();
	{
		
		i3::vector<i3RenderAttr*>	AttrList;
		i3Scene::FindAttrByType( pDlg->getRoot(), i3TextureBindAttr::static_meta(), AttrList, TRUE );
		i3::vector<i3Node*>	NodeList;
		i3Scene::FindNodeByExactType( pDlg->getRoot(), i3Particle::static_meta(), NodeList, TRUE );
		
		ElementList.insert(ElementList.end(), AttrList.begin(), AttrList.end() );
		ElementList.insert(ElementList.end(), NodeList.begin(), NodeList.end() );
		
		i3::vu::remove_duplicates(ElementList);
	}

	for(size_t i = 0; i < ElementList.size(); i ++ )
	{
		i3Texture *pTex = nullptr;
		i3Texture *pNewTex = nullptr;
		i3ElementBase *pElement = ElementList[i];

		if( i3::kind_of<i3TextureBindAttr*>(pElement))
		{
			i3TextureBindAttr *pObj = (i3TextureBindAttr *)pElement;
			pTex = pObj->GetTexture();
			if( pTex == nullptr )
			{
				pDlg->MessageBox( _T("i3TextureBindAttr::GetTexture() returns nullptr."), _T("Warning"), MB_OK );
			}
			else
			{
				pNewTex = CBFunc( pUserData, pTex );
				if( pNewTex != nullptr )
				{
					pObj->SetTexture( pNewTex );
					pNewTex = nullptr;
				}
			}
		}
		else if( i3::same_of<i3Particle*>(pElement))
		{
			i3Particle *pObj = (i3Particle *)pElement;

			pTex = pObj->getTexture();
			if (pObj->GetParticleInfo()->GetType() != i3ParticleInfo::PARTICLE_TYPE::PARTICLE_SCENE)
			{
				if (pTex == nullptr)
				{
					pDlg->MessageBox(_T("i3Particle::GetTexture() returns nullptr."), _T("Warning"), MB_OK);
				}
				else
				{
					pNewTex = CBFunc(pUserData, pTex);
					if (pNewTex != nullptr)
					{
						pObj->setTexture(pNewTex);
						pNewTex = nullptr;
					}
				}
			}

			i3TextureSequence *pTexSeq = pObj->GetTextureSequence();
			if( pTexSeq != nullptr )
			{
				//I3ASSERT( pTex == pTexSeq->GetTexture() );
				pTex = pTexSeq->GetTexture();
				if( pTex == nullptr )
				{
					pDlg->MessageBox( _T("i3TextureSequence::GetTexture() returns nullptr."), _T("Warning"), MB_OK );
				}
				else
				{
					pNewTex = CBFunc( pUserData, pTex );
					if( pNewTex != nullptr )
					{
						pTexSeq->SetTexture( pNewTex );
						pNewTex = nullptr;
					}
				}
			}
		}
		else
		{
			I3ASSERT_0;
		}
	}
}

i3Texture* _AddListItemCB( void *pUserData, i3Texture *pTexture )
{
	TEX_REPLACE_INFO * pInfo = (TEX_REPLACE_INFO *) pUserData;
	i3TDKDlgTextureTool *pDlg = (i3TDKDlgTextureTool *)pInfo->_pDlg;

	pDlg->AddListItemCB( pTexture );

	return nullptr;
}

i3Texture* _ReloadTexturesCB( void *pUserData, i3Texture *pTexture )
{
	TEX_REPLACE_INFO * pInfo = (TEX_REPLACE_INFO *) pUserData;

	//i3TDKDlgTextureTool *pDlg = pInfo->_pDlg;


	size_t idx =  i3::vu::index_of(*pInfo->_pOldTexList, pTexture);
	
	if( idx != pInfo->_pOldTexList->size() )
	{
		i3Texture * pNewTex = (i3Texture *) (*pInfo->_pNewTexList)[ idx];

		return pNewTex;
	}

	return nullptr;
}

BOOL i3TDKDlgTextureTool::GetTexFileName( TCHAR* szOut, const INT32 nOutMax, i3Texture *pTex, INT32 &nIndex, GET_TEX_NAME_MODE nMode, BOOL bForConvert )
{
	TCHAR szPath[ MAX_PATH];

	m_ED_WorkDir.GetWindowText( szPath, _countof( szPath));

	switch( nMode )
	{
	case GET_TEX_NAME_UI:
		{
			if( pTex->IsExternRes() )
			{
				I3ASSERT( pTex->hasName());
				i3::snprintf( szOut, nOutMax, _T("%s ( Extern )"), pTex->GetName() );
				return FALSE; // Extern
			}
			else
			{
				if( pTex->hasName())
				{
					i3::snprintf( szOut, nOutMax, _T("%s"), pTex->GetName() );
				}
				else
				{
					if( pTex->GetLevelCount() > 1 )
					{
						i3::snprintf( szOut, nOutMax, _T("%s_%d (i3i, i3i)"), m_szPrefix, nIndex );
					}
					else
					{
						i3::snprintf( szOut, nOutMax, _T("%s_%d (tga, i3i)"), m_szPrefix, nIndex );
					}
				}
			}
		}
		break;

	case GET_TEX_NAME_SAVE:
		{
			if( pTex->IsExternRes() )
			{
				i3::generic_string_copy( szOut, _T("") );
				return FALSE; // Extern
			}
			else
			{
				if( pTex->hasName())
				{
					TCHAR TempString[256];

				//	i3String::SplitFileName( pTex->GetName(), TempString,  TRUE); 
#ifdef _UNICODE
					i3::stack_wstring texName;	i3::mb_to_utf16(pTex->GetNameString(), texName);
#else
					const i3::rc_string& texName = pTex->GetNameString();
#endif
					i3::extract_filename( texName, TempString);

					i3::snprintf( szOut, nOutMax, _T("%s\\%s"), szPath, TempString );

					{
						i3::remove_ext( szOut );

						i3::snprintf( TempString, _countof( TempString), _T("%s.%s"), szOut, m_szExt);
						i3::safe_string_copy( szOut, TempString, nOutMax);
					}
				}
				else
				{
					i3::snprintf( szOut, nOutMax, _T("%s\\%s_%d.%s"), szPath, m_szPrefix, nIndex, m_szExt);
				}
			}
		}
		break;

	case GET_TEX_NAME_RELOAD:
		{
			if( pTex->IsExternRes() )
			{
				I3ASSERT( pTex->hasName());

				i3::snprintf( szOut, nOutMax, _T("%s\\%s"), g_pTDKEnvSet->getCurWorkDir(), pTex->GetName());

				return FALSE; // Extern
			}
			else
			{
				if( pTex->hasName())
				{
					TCHAR TempString[256];

				//	i3String::SplitFileName( pTex->GetName(),TempString,  TRUE); 
#ifdef _UNICODE
					i3::stack_wstring texName;	i3::mb_to_utf16(pTex->GetNameString(), texName);
#else
					const i3::rc_string& texName = pTex->GetNameString();
#endif

					i3::extract_filename( texName, TempString);

					i3::snprintf( szOut, nOutMax, _T("%s\\%s"), szPath, TempString );
					i3::remove_ext( szOut );

					i3::safe_string_cat( szOut, _T(".i3i"), nOutMax);
				}
				else
				{
					i3::snprintf( szOut, nOutMax, _T("%s\\%s_%d.i3i"), szPath, m_szPrefix, nIndex );
				}
			}
		}
		break;
	}

	nIndex++;
	return TRUE; // Export 대상임.
}

void i3TDKDlgTextureTool::AddListItemCB( i3Texture *pTex )
{
	m_TexIDList.push_back( pTex );
	i3::vu::push_back_once(m_TexList, pTex );
}

void i3TDKDlgTextureTool::MakeTexList()
{
	m_TexIDList.clear();
	m_TexList.clear();
	m_TexListCtrl.DeleteAllItems();

	m_nIndex = 0;

	TEX_REPLACE_INFO info;

	info._pDlg = this;

	TraceSceneTextures( &info, _AddListItemCB );

	{
		TCHAR conv[256];

		i3::snprintf( conv, _countof(conv), _T("Total : %d textures"), m_TexList.size());

		m_TotalCountCtrl.SetWindowText( conv);
	}

	for( size_t i = 0; i < m_TexList.size(); i++ )
	{
		i3Texture *pTex = m_TexList[i];

		TCHAR Temp[256]= _T("");
		I3ASSERT( pTex != nullptr );

		//int i = m_TexListCtrl.GetItemCount();
		int n = 1;

		// Get Export File Name
		GetTexFileName( Temp, _countof(Temp), pTex, m_nIndex, GET_TEX_NAME_UI );
		m_TexListCtrl.InsertItem( i, Temp);

		m_TexListCtrl.SetItemData( i, (DWORD_PTR) pTex);

		if( pTex->getTextureFlag() & I3I_TEXFLAG_NORMAL)
			i3::safe_string_copy( Temp, _T("Normal"), _countof(Temp));
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_SPECULAR)
			i3::safe_string_copy( Temp, _T("Specular"), _countof(Temp));
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_REFLECT)
			i3::safe_string_copy( Temp, _T("Reflect"), _countof(Temp));
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_REFLECT_MASK)
			i3::safe_string_copy( Temp, _T("ReflectMask"), _countof(Temp));
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_LUX)
			i3::safe_string_copy( Temp, _T("Lux"), _countof(Temp));
		else 
			i3::safe_string_copy( Temp, _T("Diffuse"), _countof(Temp));

		m_TexListCtrl.SetItemText( i, n++, Temp );

		i3::snprintf(Temp, _countof(Temp), _T("%d"), pTex->GetWidth());
		m_TexListCtrl.SetItemText( i, n++, Temp );

		i3::snprintf(Temp, _countof(Temp), _T("%d"), pTex->GetHeight());
		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED)
		{
			switch( pTex->GetPersistFormat())
			{
				case I3G_IMAGE_FORMAT_DXT1_X	:	i3::snprintf( Temp, _countof(Temp), _T("DXT1 (No Alpha)") );	break;
				case I3G_IMAGE_FORMAT_DXT1		:	i3::snprintf( Temp, _countof(Temp), _T("DXT1"));	break;
				case I3G_IMAGE_FORMAT_DXT2		:	i3::snprintf( Temp, _countof(Temp), _T("DXT2"));	break;
				case I3G_IMAGE_FORMAT_DXT3		:	i3::snprintf( Temp, _countof(Temp), _T("DXT3"));	break;
				case I3G_IMAGE_FORMAT_DXT4		:	i3::snprintf( Temp, _countof(Temp), _T("DXT4"));	break;
				case I3G_IMAGE_FORMAT_DXT5		:	i3::snprintf( Temp, _countof(Temp), _T("DXT5"));	break;
			}
		}
		else
		{
			i3::snprintf( Temp, _countof(Temp), _T("%d"), i3Gfx::GetImageFormatBitCount( pTex->GetPersistFormat()));	
		}
		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->getSurfaceCount() > 0)
		{
			i3::snprintf( Temp, _countof(Temp), _T("%d"), pTex->GetBitsPerPixel());			
		}
		else
		{
			i3::snprintf( Temp, _countof(Temp), _T("-1"));
		}

		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->GetPersistFormat() & (I3G_IMAGE_FORMAT_MASK_ZLIB | I3G_IMAGE_FORMAT_MASK_COMPRESSED))
		{
			m_TexListCtrl.SetItemText( i, n++, _T("O") );
		}
		else
		{
			m_TexListCtrl.SetItemText( i, n++, _T("X") );
		}

		i3::snprintf( Temp, _countof(Temp), _T("%d"), pTex->GetLevelCount());

		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->getSurfaceCount() > 0)
		{
			i3::integral_to_comma_str( pTex->GetDataSize(), Temp, _countof(Temp) );
		}
		else
		{
			i3::snprintf(Temp, _countof(Temp), _T("-1") );
		}
		
		//sprintf(Temp,"%d", pTex->GetDataSize());
		m_TexListCtrl.SetItemText( i, n++, Temp );
	}
}


BEGIN_MESSAGE_MAP(i3TDKDlgTextureTool, CDialog)
	ON_BN_CLICKED(IDC_TDK_BTN_SELALL, OnBnClickedTdkBtnSelall)
	ON_BN_CLICKED(IDC_TDK_BTN_UNSELALL, OnBnClickedTdkBtnUnselall)
	ON_BN_CLICKED(IDC_TDK_BTN_SELDIFF, OnBnClickedTdkBtnSeldiff)
	ON_BN_CLICKED(IDC_TDK_BTN_SELNORMAL, OnBnClickedTdkBtnSelnormal)
	ON_BN_CLICKED(IDC_TDK_BTN_SELSPEC, OnBnClickedTdkBtnSelspec)
	ON_BN_CLICKED(IDC_TDK_BTN_SELLUX, OnBnClickedTdkBtnSellux)
	ON_BN_CLICKED(IDC_TDK_BTN_WORKDIR, OnBnClickedTdkBtnWorkdir)
	ON_BN_CLICKED(IDC_TDK_BTN_SAVETEX, OnBnClickedTdkBtnSavetex)
	ON_BN_CLICKED(IDC_TDK_BTN_RESTORETEX, OnBnClickedTdkBtnRestoretex)
	ON_BN_CLICKED(IDC_TDK_BTN_RSTRTEX_SEARCH, OnBnClickedTdkBtnRstrtexSearch)
	ON_BN_CLICKED(IDC_TDK_BTN_EXTERNAL, OnBnClickedTdkBtnExternal)
	ON_BN_CLICKED(IDC_TDK_BTN_INTERNAL, OnBnClickedTdkBtnInternal)
	ON_NOTIFY(NM_CLICK, IDC_TDK_TEX_LIST, OnNMClickTdkTexList)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// i3TDKDlgTextureTool message handlers

BOOL i3TDKDlgTextureTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		LVCOLUMN col;
		memset( &col, 0, sizeof( LVCOLUMN ) );

		int n = 0;

		col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		col.fmt = LVCFMT_RIGHT;
		col.pszText = _T("Texture Name (Save,Reload Ext)");
		//col.cx = 280;
		col.cx = 200;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = _T("Usage");
		col.cx = 80;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = _T("Width");
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);
		
		col.pszText = _T("Height");
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = _T("PersistDepth");
		col.cx = 80;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = _T("Depth");
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = _T("Compressed");
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = _T("Level ");
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = _T("Size ");
		col.cx = 80;
		m_TexListCtrl.InsertColumn( n++, &col);

		m_TexListCtrl.SetExtendedStyle( LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	}

	{
		m_ImageCtrl.Create( WS_CHILD | WS_VISIBLE, this, 100);
		
		ReplaceCtrl( -1, -1);
	}

//	m_TexList.SetOnceMode();			// 이 옵션이 발견되는 컨테이너는 push_back_once로 바꿔야함..

	MakeTexList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnSelall()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		m_TexListCtrl.SetCheck( i, TRUE);
	}
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnUnselall()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		m_TexListCtrl.SetCheck( i, FALSE);
	}
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnSeldiff()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() == 0));
	}
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnSelnormal()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() & I3I_TEXFLAG_NORMAL) != 0);
	}
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnSelspec()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() & I3I_TEXFLAG_SPECULAR) != 0);
	}
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnSellux()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() & I3I_TEXFLAG_LUX) != 0);
	}
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnWorkdir()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd ) )
	{
		m_ED_WorkDir.SetWindowText( dlg.GetSelectedFolderPath());
	}
}

void i3TDKDlgTextureTool::SaveTex( INT32 idx)
{
	i3Texture * pTex = nullptr;
	TCHAR szDest[ MAX_PATH];
	bool bCreated = false;

	pTex = m_TexList[idx];

	GetTexFileName( szDest, _countof(szDest), pTex, m_nIndex, GET_TEX_NAME_SAVE);

	if( i3::generic_is_iequal( m_szExt, "TGA") )
	{
		// 압축 해제가 필요할 수 있다.
		if( pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED)
		{
			i3Texture * pNewTex = i3Texture::new_object();

			pNewTex->Create( pTex->GetWidth(), pTex->GetHeight(), 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

			pNewTex->Lock( 0);
			pTex->Lock( 0);

			i3Image_Convert( pTex->GetFormat(), pTex->GetLockedBuffer( 0), pTex->GetWidth(), pTex->GetHeight(), 1, pTex->GetClut(0), 
				pNewTex->GetFormat(), pNewTex->GetLockedBuffer(0), 1, nullptr);

			pNewTex->Unlock(0);
			pTex->Unlock(0);

			pTex = pNewTex;
			bCreated = true;
		}
	}

	{
		i3ImageFile file;

#ifdef _UNICODE
		i3::stack_string strDestFilename;	i3::utf16_to_mb(szDest, strDestFilename);
		const char* szDestFileName = strDestFilename.c_str();
#else
		const char* szDestFileName = szDest;
#endif

		if( file.Save( szDestFileName, pTex) == FALSE)
		{
			NotifyBox( m_hWnd, "Could not save %s file.", szDestFileName);
		}
	}

	if( bCreated)
	{
		I3_MUST_RELEASE(pTex);
	}
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnSavetex()
{
	TCHAR szWorkDir[MAX_PATH];

	{
		i3TDK_Dlg_SelectImageExt dlg;

		if( dlg.Execute() == false)
			return;

		i3::safe_string_copy( m_szExt, dlg.getExt(), MAX_PATH);
	}

	m_ED_WorkDir.GetWindowText( szWorkDir, sizeof(szWorkDir) - 1);
	if( szWorkDir[0] == 0)
	{
		MessageBox( _T("Please select work directory."));
		return;
	}

	m_nIndex = 0;

	for( size_t i = 0; i < m_TexList.size(); i++ )
	{
		//i3Texture *pTex = (i3Texture *)m_TexList.GetItem( i );

		if( m_TexListCtrl.GetCheck( i) == FALSE)
			continue;

		SaveTex( i);
	}

	NotifyBox( m_hWnd, "%d files saved.", m_nIndex );
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnRestoretex()
{
	TCHAR szWorkDir[ MAX_PATH];


	m_ED_WorkDir.GetWindowText( szWorkDir, _countof(szWorkDir));
	if( szWorkDir[0] == 0) 
	{
		MessageBox( _T("Please select work directory.") );
		return;
	}

	m_NewTexList.clear();

	i3Texture *pTex;
	i3Texture *pNewTex;
	TCHAR szPath[MAX_PATH];
	i3::vector<i3Texture*> OldTexList;

	m_nIndex = 0;

	for(size_t i = 0; i < m_TexList.size(); i++ )
	{
		pTex = m_TexList[i];

		if( m_TexListCtrl.GetCheck( i) == FALSE)
			continue;

		OldTexList.push_back( pTex);

		GetTexFileName( szPath, MAX_PATH, pTex, m_nIndex, GET_TEX_NAME_RELOAD );

		if( i3::is_file(szPath))
		{
			i3ImageFile file;

#ifdef _UNICODE
			i3::stack_string strFilePath;		i3::utf16_to_mb(szPath, strFilePath);
			const char* szFilePath = strFilePath.c_str();
#else
			const char* szFilePath = szPath;
#endif
			pNewTex = file.Load( szFilePath );
			if( pNewTex == nullptr )
			{
				NotifyBox( m_hWnd, "Could not load image. \"%s\"", szPath );
				return;
			}

			pNewTex->setTextureFlag( pTex->getTextureFlag());

			if( pTex->IsExternRes() )
			{
				TCHAR szName[MAX_PATH];

				i3::make_relative_path( g_pTDKEnvSet->getCurWorkDir(), szPath, szName);

#ifdef _UNICODE
				i3::stack_string strTexName;		i3::utf16_to_mb(szName, strTexName);
				const char* szTexName = strTexName.c_str();
#else
				const char* szTexName = szName;
#endif
				pNewTex->SetName( szTexName );

				pNewTex->SetExternRes();
			}

			m_NewTexList.push_back( pNewTex );
		}
	}

	m_nIndex = 0;

	{
		TEX_REPLACE_INFO info;

		info._pDlg = this;
		info._pNewTexList = &m_NewTexList;
		info._pOldTexList = &OldTexList;

		TraceSceneTextures( &info, _ReloadTexturesCB );
	}

	// m_NewTexList.SafeReleaseAll();			// 이 함수는 클리어를 포함하고 있다..
	i3::cu::for_each_container(m_NewTexList, i3::fu::mem_fun_safe(&i3Texture::Release));
	m_NewTexList.clear();

	MakeTexList();

	NotifyBox( m_hWnd, "%d file(s) reloaded.", (INT32)OldTexList.size() );
}

struct TEX_FIND_INFO
{
	const char*	m_pszName = nullptr;
	TCHAR		m_szPath[MAX_PATH] = { 0 };
	INT32		m_FindCount = 0;
} ;

static BOOL _FindTexFile( INT32 Level, char * pszPath, WIN32_FIND_DATAA * pFileInfo, void * pUserData)
{
	TEX_FIND_INFO * pInfo = (TEX_FIND_INFO *) pUserData;

	const char* fileInfoFileName = pFileInfo->cFileName;

	if( i3::generic_is_iequal( fileInfoFileName, pInfo->m_pszName) )
	{
		pInfo->m_FindCount++;
		i3::snprintf( pInfo->m_szPath, _countof(pInfo->m_szPath), _T("%s/%s"), pszPath, fileInfoFileName);
	}

	return TRUE;
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnRstrtexSearch()
{
	TCHAR szWorkDir[ MAX_PATH];

	m_ED_WorkDir.GetWindowText( szWorkDir, _countof(szWorkDir));
	if( szWorkDir[0] == 0)
	{
		MessageBox( _T("Please select work directory.") );
		return;
	}

	m_NewTexList.clear();

	i3Texture *pTex;
	i3Texture *pNewTex;
	TCHAR szPath[MAX_PATH];
	i3::vector<i3Texture*> OldTexList;
	TEX_FIND_INFO texFindInfo;

	m_nIndex = 0;

	
	for( size_t i = 0; i < m_TexList.size(); i++ )
	{
		pTex = m_TexList[i];

		if( m_TexListCtrl.GetCheck( i) == FALSE)
			continue;

		{
			// 이름을 분리해 낸다.
			char szName[ MAX_PATH];
			char szFull[MAX_PATH];

		//	i3String::SplitFileName( pTex->GetName(), szName, FALSE);

			i3::extract_filetitle(pTex->GetName(), szName);
			i3::snprintf( szFull, _countof(szFull), "%s.i3i", szName);

			texFindInfo.m_pszName = szFull;
			texFindInfo.m_szPath[0] = 0;
			texFindInfo.m_FindCount = 0;
			
#ifdef _UNICODE
			i3::stack_string strWorkDir;	i3::utf16_to_mb(szWorkDir, strWorkDir);
			const char* szWorkDir2 = strWorkDir.c_str();
#else
			const char* szWorkDir2 = szWorkDir;
#endif
			i3System::LookForFiles( szWorkDir2, szFull, (FINDFILEPROC) _FindTexFile, &texFindInfo, 0);

			if(texFindInfo.m_FindCount == 0)
			{
				// 하위 디렉토리를 검사
				if( ::MessageBox( nullptr, _T("해당 폴더에 파일이 없습니다. 하위 폴더를 검사하시겠습니까?"), _T("주의"), MB_YESNO) == IDYES)
				{
					i3System::LookForFiles( szWorkDir2, szFull, (FINDFILEPROC) _FindTexFile, &texFindInfo);

					if(texFindInfo.m_FindCount == 0)
					{	// 그래도 없으면..
						NotifyBox( m_hWnd, "%s 파일을 찾을 수 없습니다.", szFull);
						continue;
					}
				}
			}

			i3::safe_string_copy( szPath, texFindInfo.m_szPath, _countof(szPath));
		}

		OldTexList.push_back( pTex);

		i3ImageFile file;

#ifdef _UNICODE 
		i3::stack_string strImageFilePath;		i3::utf16_to_mb(szPath, strImageFilePath);
		const char* szImageFilePath = strImageFilePath.c_str();
#else
		const char* szImageFilePath = szPath;
#endif

		pNewTex = file.Load( szImageFilePath );
		if( pNewTex == nullptr )
		{
			NotifyBox( m_hWnd, "Could not load image. \"%s\"", szPath );
			return;
		}

		pNewTex->setTextureFlag( pTex->getTextureFlag());

		if( pTex->IsExternRes() )
		{
			TCHAR szName[MAX_PATH];

			i3::make_relative_path( g_pTDKEnvSet->getCurWorkDir(), szPath, szName);

#ifdef _UNICODE
			i3::stack_string strTexName;		i3::utf16_to_mb(szName, strTexName);
			const char* szTexName = strTexName.c_str();
#else
			const char* szTexName = szName;
#endif

			pNewTex->SetName( szTexName );
			pNewTex->SetExternRes();
		}

		m_NewTexList.push_back( pNewTex );
	}

	m_nIndex = 0;

	{
		TEX_REPLACE_INFO info;

		info._pDlg = this;
		info._pNewTexList = &m_NewTexList;
		info._pOldTexList = &OldTexList;

		TraceSceneTextures( &info, _ReloadTexturesCB );
	}

//	m_NewTexList.SafeReleaseAll();
	i3::cu::for_each_container(m_NewTexList, i3::fu::mem_fun_safe(&i3Texture::Release));
	m_NewTexList.clear();

	MakeTexList();

	NotifyBox( m_hWnd, "%d file(s) reloaded.", OldTexList.size() );
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnExternal()
{
	int nIndex = 0;
	for( size_t i = 0; i < m_TexList.size(); i++ )
	{
		i3Texture *pTex = m_TexList[i];

		TCHAR szPath[MAX_PATH];

		if( GetTexFileName( szPath, MAX_PATH, pTex, nIndex, GET_TEX_NAME_RELOAD ) )
		{
			TCHAR szName[MAX_PATH];

			i3::make_relative_path( g_pTDKEnvSet->getCurWorkDir(), szPath, szName);

#ifdef _UNICODE
			i3::stack_string strTexName;		i3::utf16_to_mb(szName, strTexName);
			const char* szTexName = strTexName.c_str();
#else
			const char* szTexName = szName;
#endif

			pTex->SetName( szTexName );
			pTex->SetExternRes();
		}
	}

	OnBnClickedTdkBtnRestoretex();

	MakeTexList();
}

void i3TDKDlgTextureTool::OnBnClickedTdkBtnInternal()
{
	int nIndex = 0;
	for( size_t i = 0; i < m_TexList.size(); i++ )
	{
		i3Texture *pTex = m_TexList[i];

		TCHAR szPath[MAX_PATH];

		if( ! GetTexFileName( szPath, MAX_PATH, pTex, nIndex, GET_TEX_NAME_RELOAD ) )
		{
			pTex->SetName( "" );
			pTex->SetExternRes( FALSE );
		}
	}

	MakeTexList();
}

INT32 i3TDKDlgTextureTool::getSelectedItem(void)
{
	POSITION pos = m_TexListCtrl.GetFirstSelectedItemPosition();

	if( pos != nullptr)
	{
		return m_TexListCtrl.GetNextSelectedItem( pos);
	}

	return -1;
}

void i3TDKDlgTextureTool::OnNMClickTdkTexList(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idx = getSelectedItem();

	if( idx != -1)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( idx);

		m_ImageCtrl.SetImage( pTex);
	}
	else
	{
		m_ImageCtrl.SetImage( nullptr);
	}

	*pResult = 0;
}

void i3TDKDlgTextureTool::ReplaceCtrl( INT32 cx, INT32 cy)
{
	CRect rt;

	if( cx == -1)
	{
		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	m_ST_ImageViewPH.GetWindowRect( &rt);

	ScreenToClient( &rt);

	m_ImageCtrl.SetWindowPos( nullptr, rt.left, rt.top, cx - rt.left - 6, cy - rt.top - 6, SWP_NOACTIVATE | SWP_NOZORDER);
}

void i3TDKDlgTextureTool::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ImageCtrl.m_hWnd))
	{
		ReplaceCtrl( cx, cy);
	}
}
