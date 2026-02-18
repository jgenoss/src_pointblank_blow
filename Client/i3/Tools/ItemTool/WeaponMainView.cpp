// WeaponMainView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WeaponMainView.h"
#include "DlgWeaponEditor.h"

IMPLEMENT_DYNAMIC(cWeaponMainView, CStatic)

BEGIN_MESSAGE_MAP(cWeaponMainView, CStatic)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()

cWeaponMainView::cWeaponMainView()
:	m_pParent(NULL)
{

}

void cWeaponMainView::DoDataExchange(CDataExchange* pDX)
{
	CStatic::DoDataExchange(pDX);
}

void cWeaponMainView::OnSetFocus(CWnd* pOldWnd)
{
	CStatic::OnSetFocus(pOldWnd);
	m_pParent->OnManualSetFocus();

	I3TRACE("cWeaponMainView SerFocus\n");
}

void cWeaponMainView::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	if (m_pParent)
		m_pParent->OnManualOnSize(cx,cy);

}