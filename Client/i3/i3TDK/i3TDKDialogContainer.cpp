// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKDialogContainer.h"
#include "resource.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKEditCtrl.h"
#include "i3TDKSpec_Node.h"

#define		BTN_BACK_CX			30
#define		BTN_BACK_CY			17

// i3TDKDialogContainer dialog


bool	operator<( const I3TDK_SPECDLG_INFO& p1, const I3TDK_SPECDLG_INFO& p2)
{
	return (INT32)p1.m_pMeta < (INT32)p2.m_pMeta;
}


IMPLEMENT_DYNAMIC(i3TDKDialogContainer, i3TDKDialogBase)

i3TDKDialogContainer::~i3TDKDialogContainer()
{
	delete m_pBTN_Back;			// m_pBTN_Back소거가 전혀 없어서 우선 소멸자에서 소거처리합니다.(2012.09.07.수빈)
	
	RemoveAllSpecDialog();
}

void	i3TDKDialogContainer::AddSpecDialog( i3ClassMeta * pMeta, i3GameObjPropertyDialog * pSpecDlg, UINT32 mask, bool bInternal)
{
	I3TDK_SPECDLG_INFO * pInfo;

	pInfo = (I3TDK_SPECDLG_INFO *) i3MemAlloc( sizeof(I3TDK_SPECDLG_INFO));
	I3ASSERT( pInfo != nullptr);

	pInfo->m_bInternal = bInternal;
	pInfo->m_pMeta = pMeta;
	pInfo->m_pDlg = pSpecDlg;
	pInfo->m_PopupMask = mask;

	{
		CRect rt;

		pSpecDlg->GetWindowRect( &rt);

		pInfo->m_DefWidth = rt.Width();
		pInfo->m_DefHeight = rt.Height();
	}

	pSpecDlg->ShowWindow( SW_HIDE);
	pSpecDlg->SetParent( this);

	i3TDK::RegisterUpdate( m_hWnd, pMeta, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC | I3_TDK_UPDATE_REMOVE);

	m_List.insert(pInfo);
}

void	i3TDKDialogContainer::RemoveAllSpecDialog(void)
{
	
	I3TDK_SPECDLG_INFO * pInfo;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pInfo = m_List.get_vector()[i];

		if( (pInfo->m_bInternal == true) && (pInfo->m_pDlg != nullptr))
		{
			delete pInfo->m_pDlg;
		}

		i3MemFree( pInfo);
	}

	m_List.clear();
}

i3ClassMeta * i3TDKDialogContainer::OnQuerySourceObject( i3ElementBase * pElm)
{
	if( pElm == nullptr)
		return nullptr;

	return pElm->meta();
}

void i3TDKDialogContainer::_UpdateBackButton(void)
{
	if( m_PopList.size() > 0)
	{
		m_pBTN_Back->EnableWindow( TRUE);
	}
	else
	{
		m_pBTN_Back->EnableWindow( FALSE);
	}
}


void i3TDKDialogContainer::_PopSpecDialog(void)
{
	i3GameObjPropertyDialog * pNewSpec = nullptr;

	if( m_PopList.size() <= 0)
		return;

	if( m_pCurDlg != nullptr)
	{
		m_pCurDlg->ShowWindow( SW_HIDE);
		m_pCurDlg = nullptr;
	}

	pNewSpec = m_PopList[ m_PopList.size() - 1];

	m_PopList.pop_back();

	if( pNewSpec != nullptr)
	{
		m_pCurDlg = pNewSpec;

		//INT32 y = BTN_BACK_CY + 3 + 3;

		m_pCurDlg->ShowWindow( SW_SHOW);
	}

	_UpdateBackButton();
}

void	i3TDKDialogContainer::PopupDialog( i3ElementBase * pObj, bool bPush)
{
	INT32 idx;
	CRect rect;
	i3GameObjPropertyDialog * pNewSpec = nullptr;
	i3ClassMeta * pMeta;

	pMeta	= OnQuerySourceObject( pObj);
	if((pMeta == nullptr) && (pObj != nullptr))
		pMeta = pObj->meta();

	if( pMeta != nullptr)
	{
		idx = FindSpecDialog( pMeta, 0xFFFFFFFF);
		if( idx == -1)
		{
			idx = FindSpecDialog( pObj->meta(), 0xFFFFFFFF);
		}

		if( idx != -1)
		{
			pNewSpec = GetSpecDialog( idx)->m_pDlg;
		}
	}
	else
	{
		m_PopList.clear();
	}

	if( pNewSpec != m_pCurDlg)
	{
		if( m_pCurDlg != nullptr)
		{
			if( bPush)
			{
				m_PopList.push_back( m_pCurDlg);
			}

			m_pCurDlg->ShowWindow( SW_HIDE);
			m_pCurDlg = nullptr;
		}

		if( pNewSpec != nullptr)
		{
			m_pCurDlg = pNewSpec;

			m_pCurDlg->SetObject( pObj);

			INT32 y = BTN_BACK_CY + 3 + 3;

			m_pCurDlg->SetWindowPos( nullptr, 0, y, m_Width, m_Height - y , SWP_NOACTIVATE | SWP_NOZORDER);
			m_pCurDlg->ShowWindow( SW_SHOW);
		}
	}
	else if( m_pCurDlg != nullptr)
	{
		m_pCurDlg->SetObject( pObj);
	}

	_UpdateBackButton();
}

INT32 i3TDKDialogContainer::FindSpecDialog( i3ClassMeta * pMeta, UINT32 mask)
{
	I3TDK_SPECDLG_INFO * pInfo;
	

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pInfo = m_List.get_vector()[i];

		if( pMeta->kind_of( pInfo->m_pMeta) && (pInfo->m_PopupMask & mask))
			return i;
	}

	return -1;
}

BEGIN_MESSAGE_MAP(i3TDKDialogContainer, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// i3TDKDialogContainer message handlers
void i3TDKDialogContainer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	m_Width = cx;
	m_Height = cy;

	if( m_pCurDlg != nullptr)
	{
		m_pCurDlg->SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);	
	}
}

bool i3TDKDialogContainer::_isDescendantWindow( HWND hwnd)
{
	while( hwnd != nullptr)
	{
		if( hwnd == m_hWnd)
			return true;

		hwnd = ::GetParent( hwnd);
	}

	return false;
}


void i3TDKDialogContainer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & (I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC))
	{
		INT32 idx = FindSpecDialog( pInfo->m_pMeta, pInfo->m_Event);

		if( idx != -1)
		{
			if( _isDescendantWindow( pInfo->m_hwndFrom))
				PopupDialog( pInfo->m_pObject, true);
			else
				PopupDialog( pInfo->m_pObject, false);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		INT32 idx = FindSpecDialog( pInfo->m_pMeta, pInfo->m_Event);

		if( idx != -1)
		{
			I3TDK_SPECDLG_INFO * pDlgInfo = m_List.get_vector()[idx];

			if((m_pCurDlg != nullptr) && (m_pCurDlg == pDlgInfo->m_pDlg))
			{
				if( pInfo->m_pObject != nullptr)
				{
					if( m_pCurDlg->getObject() == pInfo->m_pObject)
						PopupDialog( nullptr, false);
				}
				else
				{
					PopupDialog( nullptr, false);
				}
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		PopupDialog( nullptr, false);
	}
}

LRESULT i3TDKDialogContainer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

BOOL i3TDKDialogContainer::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_pBTN_Back = new CButton;

		CRect rect;

		rect.SetRect( 3, 3, 3 + BTN_BACK_CX, 3 + BTN_BACK_CY);

		m_pBTN_Back->Create( _T("▲"), WS_CHILD | WS_VISIBLE, rect, this, 3);
	}

	i3TDK::RegisterUpdate( m_hWnd, nullptr, I3_TDK_UPDATE_SCENE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL i3TDKDialogContainer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT id = LOWORD( wParam);
	UINT code = HIWORD( wParam);
//	HWND hwndCtrl = (HWND) lParam;

	if((code == BN_CLICKED) && (id == 3))
	{
		_PopSpecDialog();
	}

	return i3TDKDialogBase::OnCommand(wParam, lParam);
}
