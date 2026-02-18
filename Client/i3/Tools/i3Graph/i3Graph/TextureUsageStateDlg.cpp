// TextureUsageStateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "i3GraphDlg.h"
#include "TextureUsageStateDlg.h"
#include ".\textureusagestatedlg.h"
#include "i3Profile.h"
#include "i3Opt.h"

class TexData
{
	UINT32 ObjectID;
	i3Texture *pTexture;

	TexData()
	{
		ObjectID = 0;
		pTexture = NULL;
	}
};

typedef struct _tagTexReplaceInfo
{
	TextureUsageStateDlg *		_pDlg;
	i3List *					_pNewTexList;
	i3List *					_pOldTexList;
} TEX_REPLACE_INFO;

typedef i3Texture* (*TRACE_TEX_CB)( void *pUserData, i3Texture *pTexture );

void TraceSceneTextures( void *pUserData, TRACE_TEX_CB CBFunc )
{
	i3List ElementList;
	ElementList.Clear();
	ElementList.SetOnceMode();

	i3Scene::FindAttrByType( g_pMainWnd->getSceneRoot(), i3TextureBindAttr::GetClassMeta(), &ElementList, TRUE );
	i3Scene::FindNodeByExactType( g_pMainWnd->getSceneRoot(), i3Particle::GetClassMeta(), &ElementList, TRUE );

	INT32 i;

	for( i = 0; i < ElementList.GetCount(); i ++ )
	{
		i3Texture *pTex = NULL;
		i3Texture *pNewTex = NULL;
		i3ElementBase *pElement = (i3ElementBase *)ElementList.GetItem(i);

		if( pElement->IsTypeOf( i3TextureBindAttr::GetClassMeta() ) )
		{
			i3TextureBindAttr *pObj = (i3TextureBindAttr *)pElement;
			pTex = pObj->GetTexture();
			if( pTex == NULL )
			{
				MessageBox( HWND(*g_pMainWnd), "i3TextureBindAttr::GetTexture() returns NULL.", "Warning", MB_OK );
			}
			else
			{
				pNewTex = CBFunc( pUserData, pTex );
				if( pNewTex != NULL )
				{
					pObj->SetTexture( pNewTex );
					pNewTex = NULL;
				}
			}
		}
		else if( pElement->IsExactTypeOf( i3Particle::GetClassMeta() ) )
		{
			i3Particle *pObj = (i3Particle *)pElement;
			pTex = pObj->getTexture();
			if( pTex == NULL )
			{
				MessageBox( HWND(*g_pMainWnd), "i3Particle::GetTexture() returns NULL.", "Warning", MB_OK );
			}
			else
			{
				pNewTex = CBFunc( pUserData, pTex );
				if( pNewTex != NULL )
				{
					pObj->setTexture( pNewTex );
					pNewTex = NULL;
				}
			}

			i3TextureSequence *pTexSeq = pObj->GetTextureSequence();
			if( pTexSeq != NULL )
			{
				//I3ASSERT( pTex == pTexSeq->GetTexture() );
				pTex = pTexSeq->GetTexture();
				if( pTex == NULL )
				{
					MessageBox( HWND(*g_pMainWnd), "i3TextureSequence::GetTexture() returns NULL.", "Warning", MB_OK );
				}
				else
				{
					pNewTex = CBFunc( pUserData, pTex );
					if( pNewTex != NULL )
					{
						pTexSeq->SetTexture( pNewTex );
						pNewTex = NULL;
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
	TextureUsageStateDlg *pDlg = (TextureUsageStateDlg *)pUserData;
	pDlg->AddListItemCB( pTexture );
	return NULL;
}

i3Texture* _ReloadTexturesCB( void *pUserData, i3Texture *pTexture )
{
	TEX_REPLACE_INFO * pInfo = (TEX_REPLACE_INFO *) pUserData;

	TextureUsageStateDlg *pDlg = pInfo->_pDlg;


	INT32 idx = pInfo->_pOldTexList->IndexOf( pTexture);
	
	if( idx != -1)
	{
		i3Texture * pNewTex = (i3Texture *) pInfo->_pNewTexList->Items[ idx];

		return pNewTex;
	}

	return NULL;
}

// TextureUsageStateDlg dialog

IMPLEMENT_DYNAMIC(TextureUsageStateDlg, CDialog)
TextureUsageStateDlg::TextureUsageStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TextureUsageStateDlg::IDD, pParent)
	, m_strDir(_T(""))
	, m_RootDir(_T(""))
{
	m_szPrefix[0] = 0;
}

TextureUsageStateDlg::~TextureUsageStateDlg()
{
}

void TextureUsageStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEXTURE_USAGE_STATE, m_TexListCtrl);
	DDX_Text(pDX, IDC_EDIT_TEXTURE_STATE_PATH, m_strDir );
	DDX_Text(pDX, IDC_EDIT2, m_RootDir);
	DDX_Control(pDX, IDC_ST_TOTALCOUNT, m_TotalCountCtrl);
}

BEGIN_MESSAGE_MAP(TextureUsageStateDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SERCH_FOLDER, OnBnClickedButtonSerchFolder)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_STATE_SAVE, OnBnClickedButtonTextureStateSave)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_STATE_RELOAD, OnBnClickedButtonReloadAll)
	ON_BN_CLICKED(IDC_BUTTON_SET_MIPMAP_FILTER, OnBnClickedButtonSetMipmapFilter)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_MIPMPA_FILTERS, OnBnClickedButtonRemoveMipmpaFilters)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_EXTERN, OnBnClickedButtonMakeExtern)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_INTERNAL, OnBnClickedButtonMakeInternal)
	ON_BN_CLICKED(IDC_BUTTON_SET_COMPRESS, OnBnClickedButtonSetCompress)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_COMPRESS, OnBnClickedButtonRemoveCompress)
	ON_BN_CLICKED(IDC_BTN_SELALL, OnBnClickedBtnSelall)
	ON_BN_CLICKED(IDC_BTN_UNSELALL, OnBnClickedBtnUnselall)
	ON_BN_CLICKED(IDC_BTN_SELDIFFUSE, OnBnClickedBtnSeldiffuse)
	ON_BN_CLICKED(IDC_BTN_SELNORMAL, OnBnClickedBtnSelnormal)
	ON_BN_CLICKED(IDC_BTN_SELSPECULAR, OnBnClickedBtnSelspecular)
	ON_BN_CLICKED(IDC_BTN_SELLUX, OnBnClickedBtnSellux)
	ON_BN_CLICKED(IDC_BTN_RELOAD_TEX_SEARCH, OnBnClickedBtnReloadTexSearch)
END_MESSAGE_MAP()

// TextureUsageStateDlg message handlers


BOOL TextureUsageStateDlg::GetTexFileName( char *szOut, i3Texture *pTex, INT32 &nIndex, GET_TEX_NAME_MODE nMode, BOOL bForConvert )
{
	switch( nMode )
	{
	case GET_TEX_NAME_UI:
		{
			if( pTex->IsExternRes() )
			{
				I3ASSERT( pTex->hasName());
				sprintf( szOut, "%s ( Extern )", pTex->GetName() );
				return FALSE; // Extern
			}
			else
			{
				if( pTex->hasName())
				{
					sprintf( szOut, "%s", pTex->GetName() );
				}
				else
				{
					if( pTex->GetLevelCount() > 1 )
					{
						sprintf( szOut, "%s_%d (i3i, i3i)", m_szPrefix, nIndex );
					}
					else
					{
						sprintf( szOut, "%s_%d (tga, i3i)", m_szPrefix, nIndex );
					}
				}
			}
		}
		break;

	case GET_TEX_NAME_SAVE:
		{
			if( pTex->IsExternRes() )
			{
				strcpy( szOut, "" );
				return FALSE; // Extern
			}
			else
			{
				if( pTex->hasName())
				{
					char TempString[256];
					i3String::SplitFileName( pTex->GetName(),TempString,  TRUE); 
					sprintf( szOut, "%s\\%s", m_strDir.GetString(), TempString );
					if( bForConvert )
					{
						i3String::RemoveExt( szOut );
						strcat( szOut, ".tga" );
					}
				}
				else
				{
					if( pTex->GetLevelCount() > 1 )
					{
						sprintf( szOut, "%s\\%s_%d.i3i", m_strDir.GetString(), m_szPrefix, nIndex );
					}
					else
					{
						sprintf( szOut, "%s\\%s_%d.tga", m_strDir.GetString(), m_szPrefix, nIndex );
					}
				}
			}
		}
		break;

	case GET_TEX_NAME_RELOAD:
		{
			if( pTex->IsExternRes() )
			{
				I3ASSERT( pTex->hasName());
				g_pMainWnd->GetAbsolutePath( szOut, pTex->GetName() );
				return FALSE; // Extern
			}
			else
			{
				if( pTex->hasName())
				{
					char TempString[256];
					i3String::SplitFileName( pTex->GetName(),TempString,  TRUE); 
					sprintf( szOut, "%s\\%s", m_strDir.GetString(), TempString );
					i3String::RemoveExt( szOut );
					strcat( szOut, ".i3i" );
				}
				else
				{
					sprintf( szOut, "%s\\%s_%d.i3i", m_strDir.GetString(), m_szPrefix, nIndex );
				}
			}
		}
		break;
	}

	nIndex++;
	return TRUE; // Export 대상임.
}

void TextureUsageStateDlg::AddListItemCB( i3Texture *pTex )
{
	m_TexIDList.Add( pTex );
	m_TexList.Add( pTex );
}

BOOL TextureUsageStateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	{
		LVCOLUMN col;
		memset( &col, 0, sizeof( LVCOLUMN ) );

		int n = 0;

		col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		col.fmt = LVCFMT_RIGHT;
		col.pszText = "Texture Name (Save,Reload Ext)";
		//col.cx = 280;
		col.cx = 200;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = "Usage";
		col.cx = 80;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = "Width";
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);
		
		col.pszText = "Height";
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = "PersistDepth";
		col.cx = 80;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = "Depth";
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = "Compressed";
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = "Level ";
		col.cx = 50;
		m_TexListCtrl.InsertColumn( n++, &col);

		col.pszText = "Size ";
		col.cx = 80;
		m_TexListCtrl.InsertColumn( n++, &col);

		m_TexListCtrl.SetExtendedStyle( LVS_EX_CHECKBOXES);
	}

	if(g_pMainWnd->getSceneRoot() == NULL)
	{
		return FALSE;
	}

	m_RootDir = g_pMainWnd->GetRootDir();

	i3Profile pf( this );
	m_strDir = pf.GetString( "LastTextureSaveDir", "" );

	m_TexList.SetOnceMode();

	MakeTexList();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TextureUsageStateDlg::MakeTexList()
{
	m_TexIDList.Clear();
	m_TexList.Clear();
	m_TexListCtrl.DeleteAllItems();

	m_nIndex = 0;
	TraceSceneTextures( this, _AddListItemCB );

	{
		char conv[256];

		sprintf( conv, "Total : %d textures", m_TexList.GetCount());

		m_TotalCountCtrl.SetWindowText( conv);
	}

	for( int i = 0; i < m_TexList.GetCount(); i++ )
	{
		i3Texture *pTex = (i3Texture *)m_TexList.GetItem( i );

		char Temp[256]= "";
		I3ASSERT( pTex != NULL );

		//int i = m_TexListCtrl.GetItemCount();
		int n = 1;

		// Get Export File Name
		GetTexFileName( Temp, pTex, m_nIndex, GET_TEX_NAME_UI );
		m_TexListCtrl.InsertItem( i, Temp);

		m_TexListCtrl.SetItemData( i, (DWORD_PTR) pTex);

		if( pTex->getTextureFlag() & I3I_TEXFLAG_NORMAL)
			strcpy( Temp, "Normal");
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_SPECULAR)
			strcpy( Temp, "Specular");
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_REFLECT)
			strcpy( Temp, "Reflect");
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_REFLECT_MASK)
			strcpy( Temp, "ReflectMask");
		else if( pTex->getTextureFlag() & I3I_TEXFLAG_LUX)
			strcpy( Temp, "Lux");
		else 
			strcpy( Temp, "Diffuse");

		m_TexListCtrl.SetItemText( i, n++, Temp );

		sprintf(Temp,"%d", pTex->GetWidth());
		m_TexListCtrl.SetItemText( i, n++, Temp );

		sprintf(Temp,"%d", pTex->GetHeight());
		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED)
		{
			switch( pTex->GetPersistFormat())
			{
				case I3G_IMAGE_FORMAT_DXT1_X	:	sprintf( Temp, "DXT1 (No Alpha)");	break;
				case I3G_IMAGE_FORMAT_DXT1		:	sprintf( Temp, "DXT1");	break;
				case I3G_IMAGE_FORMAT_DXT2		:	sprintf( Temp, "DXT2");	break;
				case I3G_IMAGE_FORMAT_DXT3		:	sprintf( Temp, "DXT3");	break;
				case I3G_IMAGE_FORMAT_DXT4		:	sprintf( Temp, "DXT4");	break;
				case I3G_IMAGE_FORMAT_DXT5		:	sprintf( Temp, "DXT5");	break;
			}
		}
		else
		{
			sprintf(Temp,"%d", i3Gfx::GetImageFormatBitCount( pTex->GetPersistFormat()));	
		}
		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->getSurfaceCount() > 0)
		{
			sprintf(Temp,"%d", pTex->GetBitsPerPixel());			
		}
		else
		{
			sprintf(Temp,"-1");
		}

		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->GetPersistFormat() & (I3G_IMAGE_FORMAT_MASK_ZLIB | I3G_IMAGE_FORMAT_MASK_COMPRESSED))
		{
			m_TexListCtrl.SetItemText( i, n++, "O" );
		}
		else
		{
			m_TexListCtrl.SetItemText( i, n++, "X" );
		}

		sprintf(Temp,"%d", pTex->GetLevelCount());
		m_TexListCtrl.SetItemText( i, n++, Temp );

		if( pTex->getSurfaceCount() > 0)
		{
			i3String::ValueToStr( pTex->GetDataSize(), Temp, 256 );
		}
		else
		{
			sprintf(Temp,"-1");
		}
		
		//sprintf(Temp,"%d", pTex->GetDataSize());
		m_TexListCtrl.SetItemText( i, n++, Temp );
	}
}

void TextureUsageStateDlg::OnBnClickedButtonSerchFolder()
{
	// TODO: Add your control notification handler code here
	i3TDKFolderSelectDialog dlg;
	if( dlg.Execute( g_pMainWnd->m_hWnd ) )
	{
		m_strDir = dlg.GetSelectedFolderPath();

		i3Profile pf( this );
		pf.WriteString( "LastTextureSaveDir", LPCTSTR( m_strDir));

		UpdateData(FALSE);
	}
}

void TextureUsageStateDlg::OnBnClickedButtonTextureStateSave()
{
	UpdateData( TRUE);

	if(m_strDir.GetLength() == 0) 
	{
		MessageBox("Please Select Folder");
		return;
	}

	m_nIndex = 0;
	for( int i = 0; i < m_TexList.GetCount(); i++ )
	{
		i3Texture *pTex = (i3Texture *)m_TexList.GetItem( i );

		char szPath[MAX_PATH];

		if( m_TexListCtrl.GetCheck( i) == FALSE)
			continue;

		if( GetTexFileName( szPath, pTex, m_nIndex, GET_TEX_NAME_SAVE ) )
		{
			i3ImageFile file;
			if( ! file.Save( szPath, pTex ) )
			{
				m_strMsg.Format( "Could not save file. \"%s\"", szPath );
				MessageBox( LPCTSTR( m_strMsg ) );
				return;
			}
		}
	}

	m_strMsg.Format( "%d files saved.", m_nIndex );
	MessageBox( LPCTSTR( m_strMsg ) );
}

int TextureUsageStateDlg::GetTexIndex( i3Texture *pTex )
{
	int i;

	for( i = 0; i < m_TexList.GetCount(); i++ )
	{
		void * p = m_TexList.GetItem( i );
		if( pTex == p )
		{
			return i;
		}
	}

	I3ASSERT_0;
	return -1;
}

i3Texture* TextureUsageStateDlg::ReloadTexturesCB( i3Texture *pTex )
{
	i3Texture *pTexID = (i3Texture *)m_TexIDList.GetItem( m_nIndex );
	m_nIndex++;

	int n = GetTexIndex( pTexID );
	I3ASSERT( 0 <= n && n < m_NewTexList.GetCount() );

	i3Texture *pNewTex = (i3Texture *)m_NewTexList.GetItem( n );

	return pNewTex;
}

void TextureUsageStateDlg::OnBnClickedButtonReloadAll()
{
	UpdateData( TRUE);

	if(m_strDir.GetLength() == 0) 
	{
		MessageBox("Please Select Folder");
		return;
	}

	m_NewTexList.Clear();

	i3Texture *pTex;
	i3Texture *pNewTex;
	char szPath[MAX_PATH];
	i3List OldTexList;

	m_nIndex = 0;

	int i = 0;
	for( i = 0; i < m_TexList.GetCount(); i++ )
	{
		pTex = (i3Texture *)m_TexList.GetItem( i );

		if( m_TexListCtrl.GetCheck( i) == FALSE)
			continue;

		OldTexList.Add( pTex);

		GetTexFileName( szPath, pTex, m_nIndex, GET_TEX_NAME_RELOAD );

		i3ImageFile file;
		pNewTex = file.Load( szPath );
		if( pNewTex == NULL )
		{
			m_strMsg.Format( "Could not load image. \"%s\"", szPath );
			MessageBox( LPCTSTR(m_strMsg), "Error" );
			return;
		}

		pNewTex->setTextureFlag( pTex->getTextureFlag());

		if( pTex->IsExternRes() )
		{
			char szName[MAX_PATH];
			g_pMainWnd->GetRelativePath( szName, szPath );

			pNewTex->SetName( szName );
			pNewTex->SetExternRes();
		}

		m_NewTexList.Add( pNewTex );
	}

	m_nIndex = 0;

	{
		TEX_REPLACE_INFO info;

		info._pDlg = this;
		info._pNewTexList = &m_NewTexList;
		info._pOldTexList = &OldTexList;

		TraceSceneTextures( &info, _ReloadTexturesCB );
	}

	m_NewTexList.SafeReleaseAll();

	MakeTexList();

	m_strMsg.Format( "%d file(s) reloaded.", OldTexList.GetCount() );
	MessageBox( LPCTSTR(m_strMsg) );
}

void TextureUsageStateDlg::OnBnClickedButtonSetMipmapFilter()
{
	i3List FilterList;
	FilterList.SetOnceMode();

	i3Scene::FindAttrByExactType( g_pMainWnd->getSceneRoot(), i3TextureFilterAttr::GetClassMeta(), &FilterList, TRUE);

	for( int i = 0; i < FilterList.GetCount(); i++)
	{
		i3TextureFilterAttr* pTexFilter;
		pTexFilter = (i3TextureFilterAttr*)FilterList.Items[i];			

		pTexFilter->SetMagFilter(I3G_TEXTURE_FILTER_LINEAR);
		pTexFilter->SetMinFilter(I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR);

		//pTexFilter->SetMagFilter(I3G_TEXTURE_FILTER_POINT);
		//pTexFilter->SetMinFilter(I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT);
	}
}

void TextureUsageStateDlg::OnBnClickedButtonRemoveMipmpaFilters()
{
	i3List FilterList;
	FilterList.SetOnceMode();

	i3Scene::FindAttrByExactType(g_pMainWnd->getSceneRoot(), i3TextureFilterAttr::GetClassMeta(), &FilterList, TRUE);

	for( int i = 0; i < FilterList.GetCount(); i++)
	{
		i3TextureFilterAttr* pTexFilter;
		pTexFilter = (i3TextureFilterAttr*)FilterList.Items[i];			

		pTexFilter->SetMagFilter(I3G_TEXTURE_FILTER_LINEAR);
		pTexFilter->SetMinFilter(I3G_TEXTURE_FILTER_LINEAR);
	}
}

void TextureUsageStateDlg::OnBnClickedButtonMakeExtern()
{
	int i, nIndex = 0;
	for( i = 0; i < m_TexList.GetCount(); i++ )
	{
		i3Texture *pTex = (i3Texture *)m_TexList.GetItem( i );

		char szPath[MAX_PATH];
		if( GetTexFileName( szPath, pTex, nIndex, GET_TEX_NAME_RELOAD ) )
		{
			char szName[MAX_PATH];
			g_pMainWnd->GetRelativePath( szName, szPath );

			pTex->SetName( szName );
			pTex->SetExternRes();
		}
	}

	OnBnClickedButtonReloadAll();

	MakeTexList();
}

void TextureUsageStateDlg::OnBnClickedButtonMakeInternal()
{
	int i, nIndex = 0;
	for( i = 0; i < m_TexList.GetCount(); i++ )
	{
		i3Texture *pTex = (i3Texture *)m_TexList.GetItem( i );

		char szPath[MAX_PATH];
		if( ! GetTexFileName( szPath, pTex, nIndex, GET_TEX_NAME_RELOAD ) )
		{
			pTex->SetName( "" );
			pTex->SetExternRes( FALSE );
		}
	}

	MakeTexList();
}

void TextureUsageStateDlg::OnBnClickedButtonSetCompress()
{
	i3OptCompressImage opt;

	INT32 i;

	for( i = 0; i < m_TexList.GetCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexList.Items[i];

		opt.CompressImage( pTex);
	}

	MakeTexList();
}

void TextureUsageStateDlg::OnBnClickedButtonRemoveCompress()
{
	i3OptCompressImage opt;

	INT32 i;

	for( i = 0; i < m_TexList.GetCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexList.Items[i];

		opt.DecompressImage( pTex);
	}

	MakeTexList();
}

void TextureUsageStateDlg::OnBnClickedBtnSelall()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		m_TexListCtrl.SetCheck( i, TRUE);
	}
}

void TextureUsageStateDlg::OnBnClickedBtnUnselall()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		m_TexListCtrl.SetCheck( i, FALSE);
	}
}

void TextureUsageStateDlg::OnBnClickedBtnSeldiffuse()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() == 0));
	}
}

void TextureUsageStateDlg::OnBnClickedBtnSelnormal()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() & I3I_TEXFLAG_NORMAL) != 0);
	}
}

void TextureUsageStateDlg::OnBnClickedBtnSelspecular()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() & I3I_TEXFLAG_SPECULAR) != 0);
	}
}

void TextureUsageStateDlg::OnBnClickedBtnSellux()
{
	INT32 i;

	for( i = 0; i < m_TexListCtrl.GetItemCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) m_TexListCtrl.GetItemData( i);

		m_TexListCtrl.SetCheck( i, (pTex->getTextureFlag() & I3I_TEXFLAG_LUX) != 0);
	}
}

typedef struct _tagTexFindInfo
{
	char *		m_pszName;
	char		m_szPath[ MAX_PATH];
	INT32		m_FindCount;
} TEX_FIND_INFO;

static BOOL _FindTexFile( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData)
{
	TEX_FIND_INFO * pInfo = (TEX_FIND_INFO *) pUserData;

	if( i3String::Compare( pFileInfo->cFileName, pInfo->m_pszName) == 0)
	{
		pInfo->m_FindCount++;
		i3String::Format( pInfo->m_szPath, sizeof(pInfo->m_szPath), "%s/%s", pszPath, pFileInfo->cFileName);
	}

	return TRUE;
}

void TextureUsageStateDlg::OnBnClickedBtnReloadTexSearch()
{
	UpdateData( TRUE);

	if(m_strDir.GetLength() == 0) 
	{
		MessageBox("Please Select Folder");
		return;
	}

	m_NewTexList.Clear();

	i3Texture *pTex;
	i3Texture *pNewTex;
	char szPath[MAX_PATH];
	i3List OldTexList;
	TEX_FIND_INFO info;

	m_nIndex = 0;

	int i = 0;
	for( i = 0; i < m_TexList.GetCount(); i++ )
	{
		pTex = (i3Texture *)m_TexList.GetItem( i );

		if( m_TexListCtrl.GetCheck( i) == FALSE)
			continue;

		//GetTexFileName( szPath, pTex, m_nIndex, GET_TEX_NAME_RELOAD );
		{
			// 이름을 분리해 낸다.
			char szName[ MAX_PATH], szFull[MAX_PATH];

			i3String::SplitFileName( pTex->GetName(), szName, FALSE);
			sprintf( szFull, "%s.i3i", szName);

			info.m_pszName = szFull;
			info.m_szPath[0] = 0;
			info.m_FindCount = 0;

			i3System::LookForFiles( (LPCTSTR) m_strDir, szFull, (FINDFILEPROC) _FindTexFile, &info);

			if( info.m_FindCount == 0)
			{
				NotifyBox( m_hWnd, "%s 파일을 찾을 수 없습니다.", szFull);
				continue;
			}

			strcpy( szPath, info.m_szPath);
		}

		OldTexList.Add( pTex);

		i3ImageFile file;
		pNewTex = file.Load( szPath );
		if( pNewTex == NULL )
		{
			m_strMsg.Format( "Could not load image. \"%s\"", szPath );
			MessageBox( LPCTSTR(m_strMsg), "Error" );
			return;
		}

		pNewTex->setTextureFlag( pTex->getTextureFlag());

		if( pTex->IsExternRes() )
		{
			char szName[MAX_PATH];
			g_pMainWnd->GetRelativePath( szName, szPath );

			pNewTex->SetName( szName );
			pNewTex->SetExternRes();
		}

		m_NewTexList.Add( pNewTex );
	}

	m_nIndex = 0;

	{
		TEX_REPLACE_INFO info;

		info._pDlg = this;
		info._pNewTexList = &m_NewTexList;
		info._pOldTexList = &OldTexList;

		TraceSceneTextures( &info, _ReloadTexturesCB );
	}

	m_NewTexList.SafeReleaseAll();

	MakeTexList();

	m_strMsg.Format( "%d file(s) reloaded.", OldTexList.GetCount() );
	MessageBox( LPCTSTR(m_strMsg) );	
}
