#include "stdafx.h"
#include "resource.h"
#include "i3Export.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/remove_ext.h"

static HWND		s_hwndDlg;
//static i3::vector<StdMat*> s_MaterialList;

static void _Rec_TraverseMtl( Mtl * pMtl, i3::vector<Mtl*>& List)
{
	INT32 i;
	Mtl * pSubMtl;

	if( pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0))
		List.push_back( pMtl);

	for( i = 0; i < pMtl->NumSubMtls(); i++)
	{
		pSubMtl = pMtl->GetSubMtl( i);

		if( pSubMtl == NULL)
			continue;

		_Rec_TraverseMtl( pSubMtl, List);
	}
}

static void _Rec_FindTex( INode * pNode, i3::vector<Mtl*>& List)
{
	Mtl * pMtl = pNode->GetMtl();

	if( pMtl != NULL)
	{
		_Rec_TraverseMtl( pMtl, List);
	}

	INT32 i;
	INode * pChild;

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		pChild = pNode->GetChildNode( i);

		_Rec_FindTex( pChild, List);
	}
}

static void	_OnInitDialog( HWND hwnd)
{
	HWND hwndCtrl;

	CenterWindow( hwnd, GetParent( hwnd));

	s_hwndDlg = hwnd;

	hwndCtrl = ::GetDlgItem( hwnd, IDC_TEX_LIST);
	I3ASSERT( hwndCtrl != NULL);

	// Initialize
	{
		LVCOLUMN col;

		col.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		col.pszText		= "Name";
		col.fmt			= LVCFMT_CENTER;
		col.cx			= 380;
		ListView_InsertColumn( hwndCtrl, 0, &col);

		col.pszText		= "Width";
		col.cx			= 50;
		ListView_InsertColumn( hwndCtrl, 1, &col);

		col.pszText		= "Height";
		col.cx			= 50;
		ListView_InsertColumn( hwndCtrl, 2, &col);
	}

	Interface * ip = GetCOREInterface();

	{
		INT32 j;
		i3::vector<Texmap*> bmpList;
		i3::vector<Mtl*> MaterialList;

//		s_MaterialList.SetOnceMode( TRUE);
//		bmpList.SetOnceMode( TRUE);

		_Rec_FindTex( ip->GetRootNode(), MaterialList);
		
		i3::vu::remove_duplicates(MaterialList);			// 중복노드를 나중에 제거..(2012.05.28.수빈)

		for(size_t i = 0; i < MaterialList.size(); i++)
		{
			StdMat * pMtl = static_cast<StdMat*>(MaterialList[i]);

			// Ambient
			for( j = ID_AM; j <= ID_DP; j++)
			{
				if( pMtl->MapEnabled( j))
				{
					Texmap * pMap = pMtl->GetSubTexmap( j);

					if( pMap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
					{
						bmpList.push_back( pMap);
						//BitmapTex * pBmpTex = (BitmapTex *) pMap;
					}

					for( INT32 k = 0; k < pMap->NumSubTexmaps(); k++)
					{
						Texmap * pSubMap = pMap->GetSubTexmap( j);

						if( pSubMap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
							bmpList.push_back( pSubMap);
					}
				}

			}
		}
		
		i3::vu::remove_duplicates(bmpList);

		// Bitmap 정보를 ListView에 추가
		for(size_t i = 0; i < bmpList.size(); i++)
		{
			BitmapTex * pTex = (BitmapTex *) bmpList[i];
			Bitmap * pBmp = pTex->GetBitmap( (TimeValue) 0);
			LVITEM item;
			char conv[128];

			item.mask = LVIF_COLUMNS | LVIF_PARAM | LVIF_TEXT;
			item.iSubItem = 0;
			item.iItem = i;
			item.lParam = (LPARAM) pTex;

			if( pBmp != NULL)
			{
				BitmapStorage * pStorage = (BitmapStorage *) pBmp->Storage();
				BitmapInfo * pInfo = & pStorage->bi;

				item.pszText = (LPSTR) pInfo->Name();
			}
			else
			{
				item.pszText = pTex->GetMapName();
			}

			INT32 idx = ListView_InsertItem( hwndCtrl, &item);

			if( pBmp != NULL)
			{
				sprintf( conv, "%d", pBmp->Width());
				ListView_SetItemText( hwndCtrl, idx, 1, conv);

				sprintf( conv, "%d", pBmp->Height());
				ListView_SetItemText( hwndCtrl, idx, 2, conv);
			}
			else
			{
				ListView_SetItemText( hwndCtrl, idx, 1, "-");
				ListView_SetItemText( hwndCtrl, idx, 2, "-");
			}
		}
	}
}

static void ConvertTGA( HWND hwnd)
{
	INT32 i, cnt;
	BitmapTex * pTex;
	LVITEM item;
	char szExt[128], szName[MAX_PATH];
	HWND hwndCtrl = ::GetDlgItem( hwnd, IDC_TEX_LIST);

	cnt = ListView_GetItemCount( hwndCtrl);

	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.iSubItem = 0;
	item.cchTextMax = sizeof(szName) - 1;
	item.pszText = szName;
	
	for( i = 0; i < cnt; i++)
	{
		item.iItem = i;

		ListView_GetItem( hwndCtrl, &item);

		pTex = (BitmapTex *) item.lParam;

		if( pTex->GetBitmap((TimeValue) 0) == NULL)
			continue;

		i3::extract_fileext( szName, szExt);

		if( i3::generic_is_iequal( szExt, "TGA") )
			continue;

		Bitmap * pBitmap = pTex->GetBitmap( (TimeValue) 0);

		i3Texture * pI3Tex = i3Export::_ConvertBitmap( pBitmap);

		if( pI3Tex == NULL)
			continue;

		// TGA 파일로 저장
		{
			char szPath[ MAX_PATH];

			strcpy( szPath, szName);
			i3::remove_ext( szPath);

			strcat( szPath, ".TGA");

			i3ImageFile file;

			if( file.Save( szPath, pI3Tex) == FALSE)
				continue;

			pI3Tex->Release();

			pTex->SetMapName( szPath);
			pTex->ReloadBitmapAndUpdate();
		}

		Bitmap * pNewBitmap = pTex->GetBitmap( (TimeValue) 0);
	}
}

static void _OnCommand( HWND hwnd, UINT code, UINT id, HWND hwndCtrl)
{
	switch( id)
	{
		case IDOK  :
			ConvertTGA( hwnd);

		case IDCANCEL :
			EndDialog( hwnd, id);
			break;
	}
}

BOOL CALLBACK _DlgConvertTGAProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg)
	{	
		case WM_INITDIALOG :			_OnInitDialog( hwnd);	break;
		case WM_COMMAND :				_OnCommand( hwnd, HIWORD( wParam), LOWORD( wParam), (HWND) lParam);	break;
	}

	return FALSE;
}
