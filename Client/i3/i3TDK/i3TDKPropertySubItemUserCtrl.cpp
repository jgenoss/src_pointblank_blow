#include "stdafx.h"
#include "i3TDKPropertySubItemUserCtrl.h"

I3_CLASS_INSTANCE( i3TDKPropertySubItemUserCtrl);

i3TDKPropertySubItemUserCtrl::i3TDKPropertySubItemUserCtrl( void)
{
	Reset();
}

i3TDKPropertySubItemUserCtrl::i3TDKPropertySubItemUserCtrl( SubItemUserCtrlInitStruct * pInitStruct)
{
	Create( pInitStruct);
}

BOOL i3TDKPropertySubItemUserCtrl::Create( SubItemUserCtrlInitStruct * pInitStruct)
{
	Reset();

	SubItemInitStruct init;
	init._id = pInitStruct->_id;
	init._row = pInitStruct->_row;
	init._column = pInitStruct->_column;
	init._style = pInitStruct->_style;   
	init._label = pInitStruct->_label;
	init._data = pInitStruct->_data;

	SetCallOnDraw( pInitStruct->_pCallOnDraw);
	SetCallOnHit( pInitStruct->_pCallOnHit);
	SetCallOnUpdate( pInitStruct->_pCallOnUpdate);
	SetCallOnRelease( pInitStruct->_pCallOnRelease);

	return i3TDKPropertySubItem::Create( &init);
}

void i3TDKPropertySubItemUserCtrl::Reset()
{
	i3TDKPropertySubItem::Reset();

	m_pParent = nullptr;
	m_nCtrlID = 0;

	m_pCallOnDraw = nullptr;
	m_pCallOnHit = nullptr;
	m_pCallOnUpdate = nullptr;
	m_pCallOnRelease = nullptr;
}

void i3TDKPropertySubItemUserCtrl::CopyTo( i3TDKPropertySubItemUserCtrl * pSubItem)
{
	SetCallOnDraw( pSubItem->GetCallOnDraw());
	SetCallOnHit( pSubItem->GetCallOnHit());
	SetCallOnUpdate( pSubItem->GetCallOnUpdate());
	SetCallOnRelease( pSubItem->GetCallOnRelease());

	i3TDKPropertySubItem::CopyTo( pSubItem);
}

void i3TDKPropertySubItemUserCtrl::UpdateMessage( CWnd * pUserCtrl, const TCHAR* pszLabel)
{
	if( m_pParent)
	{
		//	부모 윈도우에세 LVN_ENDLABELEDIT 이벤트를 보낸다.
		LV_DISPINFO		lvdi;
		HWND	hWnd = nullptr;
		UINT32	nID = 0;

		if( m_pParent)
			hWnd = m_pParent->GetSafeHwnd();

		if( pUserCtrl)
			nID = pUserCtrl->GetDlgCtrlID();

		lvdi.hdr.hwndFrom	= hWnd;
		lvdi.hdr.idFrom		= nID;
		lvdi.hdr.code		= LVN_ENDLABELEDIT;

		lvdi.item.mask		= LVIF_TEXT;
		lvdi.item.iItem		= m_nRowIdx;
		lvdi.item.iSubItem	= m_nColumnIdx;
		lvdi.item.pszText	= (TCHAR*) pszLabel;
		lvdi.item.cchTextMax= i3::generic_string_size( pszLabel);

		//	  
		m_pParent->SendMessage( WM_NOTIFY, 0, (LPARAM) &lvdi); 
	}
	else
	{
		//	Parent 가 없는 경우 SubItem 만 업데이트한다.
		SubItemUpdateInfo updateInfo;
		
		updateInfo._nRowIdx		= m_nRowIdx;
		updateInfo._nColumnIdx	= m_nColumnIdx;
		updateInfo._pszText		= pszLabel;

		OnUpdate( &updateInfo);
	}
}

void i3TDKPropertySubItemUserCtrl::OnRelease( void)
{
	if( m_pCallOnRelease)
	{
		CallOnReleaseRef ref;
		
		ref.pUserCtrl = this;

		m_pCallOnRelease( &ref);	// 사용자 등록 Release 함수를 호출한다.
	}
}

void i3TDKPropertySubItemUserCtrl::OnUpdate( SubItemUpdateInfo * pInfo)
{	
	if( m_pCallOnUpdate)
	{
		CallOnUpdateRef ref;

		ref.pUserCtrl = this;
		ref.pInfo = pInfo;

		m_pCallOnUpdate( &ref);		// 사용자 등록 Update 함수를 호출한다.
	}

	i3TDKPropertySubItem::OnUpdate( pInfo);
}

void i3TDKPropertySubItemUserCtrl::OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet)
{
	if( m_pCallOnDraw)
	{
		CallOnDrawRef ref;
		
		ref.pUserCtrl = this;
		ref.lpDrawItemStruct = lpDrawItemStruct;
		ref.pRect = pRect;
		ref.pColorSet = pColorSet;

		m_pCallOnDraw( &ref);		// 사용자 등록 Draw 함수를 호출한다.
		return;
	}

	i3TDKPropertySubItem::OnDraw( lpDrawItemStruct, pRect, pColorSet);
}

void i3TDKPropertySubItemUserCtrl::OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle)
{
	m_pParent = pParentWnd;

	if( m_pCallOnHit)
	{
		CallOnHitRef ref;		

		ref.pUserCtrl = this;
		ref.pParentWnd = pParentWnd;
		ref.pRect = pRect;
		ref.dwStyle = dwStyle;

		m_pCallOnHit( &ref);		// 사용자 등록 Hit 함수를 호출한다.
	}
}
