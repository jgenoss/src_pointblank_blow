#include "stdafx.h"
#include "i3TDKPropertySubItemFilePath.h"
#include "i3TDKFolderSelectDialog.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/generic_string_cat.h"

I3_CLASS_INSTANCE( i3TDKPropertySubItemFilePath);

i3TDKPropertySubItemFilePath::i3TDKPropertySubItemFilePath( void)
{
	Reset();
}

i3TDKPropertySubItemFilePath::i3TDKPropertySubItemFilePath( SubItemFilePathInitStruct * pInitStruct)
{
	Create( pInitStruct);
}


BOOL i3TDKPropertySubItemFilePath::Create( SubItemFilePathInitStruct * pInitStruct)
{	
	Reset();

	SubItemInitStruct init;
	init._id = pInitStruct->_id;
	init._row = pInitStruct->_row;
	init._column = pInitStruct->_column;
	init._style = pInitStruct->_style;
	init._label = pInitStruct->_label;
	init._data = pInitStruct->_data;

	m_DialogStyle = pInitStruct->_DialogStyle;

	if( pInitStruct->_szRelativePath)
		i3::safe_string_copy( m_szRelativePath, pInitStruct->_szRelativePath, 256);
	else
		memset( m_szRelativePath, 0, sizeof( m_szRelativePath));

	if( pInitStruct->_szDialogName)
		i3::safe_string_copy( m_szDialogName, pInitStruct->_szDialogName, 256);
	else
		memset( m_szDialogName, 0, sizeof( m_szDialogName));

	return i3TDKPropertySubItem::Create( &init);
}

void i3TDKPropertySubItemFilePath::Reset()
{
	i3TDKPropertySubItem::Reset();

	memset( m_szRelativePath, 0, sizeof( m_szRelativePath));
	memset( m_szDialogName, 0, sizeof( m_szDialogName));	

	m_DialogStyle = 0;
}

void i3TDKPropertySubItemFilePath::CopyTo( i3TDKPropertySubItemFilePath * pSubItem)
{
	pSubItem->SetRelativePath( GetRelativePath());
	pSubItem->SetDialogName( GetDialogName());

	i3TDKPropertySubItem::CopyTo( pSubItem);
}

void i3TDKPropertySubItemFilePath::OnRelease( void)
{
	i3TDKPropertySubItem::OnRelease();
}

void i3TDKPropertySubItemFilePath::OnUpdate( SubItemUpdateInfo * pInfo)
{
	//	Label 업데이트
	SetLabel( pInfo->_pszText);
}

void i3TDKPropertySubItemFilePath::OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet)
{
	i3TDKPropertySubItem::OnDraw( lpDrawItemStruct, pRect, pColorSet); 
}

void i3TDKPropertySubItemFilePath::OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle)
{
	const TCHAR* pszFullPath = nullptr;
	TCHAR szLabel[256] = _T("");
	TCHAR szBuffer[256] = _T("");

	if( m_Style & FILEPATH_STYLE_RELATIVE_PATH)			//	상대 경로 지정시
		i3::safe_string_copy( szLabel, m_szRelativePath, 256);

	i3::generic_string_cat( szLabel, GetLabel());

	//	폴더 선택
	if( m_Style & FILEPATH_STYLE_ONLY_FOLDER)
	{
		i3TDKFolderSelectDialog Dlg;
		
		if( Dlg.Execute( pParentWnd->GetSafeHwnd(), m_szDialogName, m_DialogStyle | BIF_RETURNONLYFSDIRS, 0, 0, szLabel) == FALSE)	return;
		
		pszFullPath = Dlg.GetSelectedFolderPath();
	}
	//	파일 경로 선택
	else
	{
		OPENFILENAME ofn;
		memset( &ofn, 0, sizeof( OPENFILENAME));
		ofn.hwndOwner = pParentWnd->GetSafeHwnd();
		ofn.lStructSize = sizeof( OPENFILENAME);
		ofn.lpstrFile = szBuffer;
		ofn.lpstrInitialDir = szLabel;
		ofn.nMaxFile = 256;
		ofn.lpstrTitle = m_szDialogName;
		ofn.nMaxFileTitle = 256;
		ofn.Flags = m_DialogStyle | OFN_HIDEREADONLY;

		if( GetOpenFileName( &ofn) == 0)	return;

		pszFullPath = szBuffer;
	}

	if( m_Style & FILEPATH_STYLE_RELATIVE_PATH)	//	상대 경로 지정시
	{
		i3::make_relative_path( m_szRelativePath, pszFullPath, szLabel);
	}
	else
	{
		i3::safe_string_copy( szLabel, pszFullPath, 256);
	}

	//	부모 윈도우에세 LVN_ENDLABELEDIT 이벤트를 보낸다.
	LV_DISPINFO		lvdi;
	lvdi.hdr.hwndFrom	= pParentWnd->GetSafeHwnd();
	lvdi.hdr.idFrom		= pParentWnd->GetDlgCtrlID();
	lvdi.hdr.code		= LVN_ENDLABELEDIT;

	lvdi.item.mask		= LVIF_TEXT;
	lvdi.item.iItem		= m_nRowIdx;
	lvdi.item.iSubItem	= m_nColumnIdx;
	lvdi.item.pszText	= szLabel;
	lvdi.item.cchTextMax= i3::generic_string_size( szLabel);

	pParentWnd->SendMessage( WM_NOTIFY, 0, (LPARAM) &lvdi); 

	pParentWnd->SetFocus();	// KillFocus
}

