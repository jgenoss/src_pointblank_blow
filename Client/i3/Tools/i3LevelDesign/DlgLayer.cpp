// DlgLayer.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgLayer.h"
#include ".\dlglayer.h"


enum ELM_ICON_IDX
{
	EII_CAMERA = 0,
	EII_CHARA,
	EII_OBJECT,
	EII_RESPAWN,
	EII_LIGHT_DIR,
	EII_LIGHT_POINT,
	EII_LIGHT_SPOT,
	EII_BOX,
	EII_CYLINDER,
	EII_SPHERE,
	EII_WORLD,

	EII_MAX
};

static INT32 s_IconTable[ EII_MAX] =
{
	IDI_CAMERA,					// EII_CAMERA = 0,
	IDI_TDK_CHARACTER,			// EII_CHARA,
	IDI_TDK_OBJECT,				// EII_OBJECT,
	IDI_TDK_RESPAWN,			// EII_RESPAWN,
	IDI_TDK_LIGHT_DIRECTIONAL,	// EII_LIGHT_DIR,
	IDI_TDK_LIGHT_POINT,		// EII_LIGHT_POINT,
	IDI_TDK_LIGHT_SPOT,			// EII_LIGHT_SPOT,
	IDI_TDK_BOX,				// EII_BOX,
	IDI_TDK_CYLINDER,			// EII_CYLINDER,
	IDI_TDK_SPHERE,				// EII_SPHERE,
	IDI_TDK_WORLD,				// EII_WORLD,
};


// CDlgLayer dialog

IMPLEMENT_DYNAMIC(CDlgLayer, CDialog)
CDlgLayer::CDlgLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLayer::IDD, pParent)
{
	m_bDrag = false;
}

CDlgLayer::~CDlgLayer()
{
}

void CDlgLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_NEWLAYER, m_BTN_New);
	DDX_Control(pDX, IDC_BTN_DELETELAYER, m_BTN_Delete);
	DDX_Control(pDX, IDC_LAYER_LIST, m_LayerListCtrl);
	DDX_Control(pDX, IDC_ELM_LIST, m_ElmListCtrl);
	DDX_Control(pDX, IDC_BTN_SELECT_ELM, m_BTN_Select);
}

void CDlgLayer::UpdateAllLayer(void)
{
	m_LayerListCtrl.DeleteAllItems();

	INT32 i, idx;
	i3LevelLayer * pLayer;

	if( g_pScene == NULL)
		return;

	for( i = 0; i < g_pScene->getLayerCount(); i++)
	{
		pLayer = g_pScene->getLayer( i);

		idx = m_LayerListCtrl.InsertItem( i, pLayer->GetName(), 0);

		UpdateLayer( idx, pLayer);
	}
}

void CDlgLayer::UpdateLayer( INT32 idx, i3LevelLayer * pLayer)
{
	LVITEM item;

	item.mask = LVIF_IMAGE;
	item.iItem = idx;
	
	if( pLayer->getState() & I3LV_LAYER_STATE_SELECTED)
		item.iImage = 1;
	else
		item.iImage = 0;

	item.lParam = (LPARAM) pLayer;
	
	m_LayerListCtrl.SetItem( &item);

	m_LayerListCtrl.SetItemData( idx, (DWORD_PTR) pLayer);

	if( pLayer->getState() & I3LV_LAYER_STATE_HIDED)
		m_LayerListCtrl.SetItemText( idx, 1, "-");
	else
		m_LayerListCtrl.SetItemText( idx, 1, "○");

	if( pLayer->getState() & I3LV_LAYER_STATE_FREEZED)
		m_LayerListCtrl.SetItemText( idx, 2, "○");
	else
		m_LayerListCtrl.SetItemText( idx, 2, "-");

}

void CDlgLayer::SetCurLayer( INT32 idx)
{
	i3LevelLayer * pLayer;

	INT32 i;
	
	for( i = 0; i < m_LayerListCtrl.GetItemCount(); i++)
	{
		pLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( i);

		if( idx == i)
		{
			pLayer->addState( I3LV_LAYER_STATE_SELECTED);

			g_pScene->setCurrentLayer( pLayer);
		}
		else
		{
			pLayer->removeState( I3LV_LAYER_STATE_SELECTED);
		}

		UpdateLayer( i, pLayer);
	}
}

void CDlgLayer::ToggleShow( INT32 idx)
{
	i3LevelLayer * pLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( idx);

	if( pLayer->getState() & I3LV_LAYER_STATE_HIDED)
		pLayer->Show( TRUE);
	else
		pLayer->Show( FALSE);

	UpdateLayer( idx, pLayer);
}

void CDlgLayer::ToggleFreeze( INT32 idx)
{
	i3LevelLayer * pLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( idx);

	if( pLayer->getState() & I3LV_LAYER_STATE_FREEZED)
		pLayer->Freeze( FALSE);
	else
		pLayer->Freeze( TRUE);

	UpdateLayer( idx, pLayer);
}

INT32	CDlgLayer::_getElmIcon( i3LevelElement3D * pElm)
{
	if( i3::kind_of<i3LevelCamera*>(pElm)) //->IsTypeOf( i3LevelCamera::static_meta()))
	{
		return EII_CAMERA;
	}
	else if( i3::kind_of<i3LevelChara*>(pElm)) //->IsTypeOf( i3LevelChara::static_meta()))
	{
		return EII_CHARA;
	}
	else if( i3::kind_of<i3LevelObject*>(pElm)) //->IsTypeOf( i3LevelObject::static_meta()))
	{
		return EII_OBJECT;
	}
	/*
	else if( i3::same_of<i3LevelPrimitiveRespawn*>(pElm)) //->IsExactTypeOf( i3LevelPrimitiveRespawn::static_meta()))
	{
		return EII_RESPAWN;
	}
	*/
	else if( i3::kind_of<i3LevelLight*>(pElm)) //->IsTypeOf( i3LevelLight::static_meta()))
	{
		i3LevelLight * pLight = (i3LevelLight *) pElm;

		switch( pLight->getLightObject()->getLightAttr()->getLightType())
		{
			case I3G_LIGHT_SPOT :		return EII_LIGHT_SPOT;
			case I3G_LIGHT_POINT :		return EII_LIGHT_POINT;
			default :					return EII_LIGHT_DIR;
		}
	}
	else if( i3::kind_of<i3LevelWorld*>(pElm)) //->IsTypeOf( i3LevelWorld::static_meta()))
	{
		return EII_WORLD;
	}
	else if( i3::same_of<i3LevelPrimitive_Sphere*>(pElm)) //->IsExactTypeOf( i3LevelPrimitive_Sphere::static_meta()))
	{
		return EII_SPHERE;
	}
	
	return EII_BOX;
}

void CDlgLayer::UpdateAllElements(void)
{
	m_ElmListCtrl.DeleteAllItems();

	INT32 i, idx;
	i3LevelLayer * pLayer;

	idx = getSelectedLayer();
	if( idx == -1)
		return;

	pLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( idx);

	I3TRACE( "GET(%d) : %p\n", idx, pLayer);

	for( i = 0; i < pLayer->getElementCount(); i++)
	{
		i3LevelElement3D * pElm = pLayer->getElement( i);

		idx = m_ElmListCtrl.InsertItem( i, pElm->GetName(), _getElmIcon( pElm));

		UpdateElement( idx, pElm);
	}
}

void CDlgLayer::UpdateElement( INT32 idx, i3LevelElement3D * pElm)
{
	m_ElmListCtrl.SetItemData( idx, (DWORD_PTR) pElm);

	// Type
	if( pElm->getInstanceClassName()[0] != 0)
		m_ElmListCtrl.SetItemText( idx, 1, pElm->getInstanceClassName());
	else
		m_ElmListCtrl.SetItemText( idx, 1, pElm->meta()->class_name().c_str());
}

BEGIN_MESSAGE_MAP(CDlgLayer, CDialog)
ON_WM_SIZE()
ON_BN_CLICKED(IDC_BTN_NEWLAYER, OnBnClickedBtnNew)
ON_BN_CLICKED(IDC_BTN_DELETELAYER, OnBnClickedBtnDelete)
ON_WM_SHOWWINDOW()
ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LAYER_LIST, OnLvnEndlabeleditLayerList)
ON_NOTIFY(NM_DBLCLK, IDC_LAYER_LIST, OnNMDblclkLayerList)
ON_NOTIFY(NM_CLICK, IDC_LAYER_LIST, OnNMClickLayerList)
ON_NOTIFY(LVN_BEGINDRAG, IDC_ELM_LIST, OnLvnBegindragElmList)
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_BN_CLICKED(IDC_BTN_SELECT_ELM, &CDlgLayer::OnBnClickedBtnSelectElm)
ON_NOTIFY(NM_SETFOCUS, IDC_ELM_LIST, &CDlgLayer::OnNMSetfocusElmList)
ON_NOTIFY(NM_RCLICK, IDC_ELM_LIST, &CDlgLayer::OnNMRClickElmList)
ON_NOTIFY(NM_DBLCLK, IDC_ELM_LIST, &CDlgLayer::OnNMDblclkElmList)
END_MESSAGE_MAP()


// CDlgLayer message handlers
static INT32 addIcon( CImageList * pList, INT32 id)
{
	HICON hIcon	= ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));

	INT32 nIndex = pList->Add( hIcon);

	::DestroyIcon( hIcon);

	return nIndex;
}

BOOL CDlgLayer::OnInitDialog()
{
	HICON hIcon;
	INT32 i;

	CDialog::OnInitDialog();

	{
		hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICO_NEW), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		m_BTN_New.SetIcon( hIcon);

		hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICO_DELETE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		m_BTN_Delete.SetIcon( hIcon);

		hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICO_SEL), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		m_BTN_Select.SetIcon( hIcon);
	}

	{
		m_ImageList.Create( 16, 16, ILC_COLOR32 | ILC_MASK, 0, 2);

		hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICO_BLANK), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		m_ImageList.Add( hIcon);

		hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICO_CURRENT_LAYER), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		m_ImageList.Add( hIcon);
	}

	{
		m_ElmImageList.Create( 16, 16, ILC_COLOR32 | ILC_MASK, EII_MAX, 0);

		for( i = 0; i < EII_MAX; i++)
		{
			addIcon( &m_ElmImageList, s_IconTable[i]);
		}
	}

	{
		m_LayerListCtrl.InsertColumn( 0, "Layer", LVCFMT_LEFT, 100);
		m_LayerListCtrl.InsertColumn( 1, "Show", LVCFMT_LEFT, 60);
		m_LayerListCtrl.InsertColumn( 2, "Freeze", LVCFMT_LEFT, 80);
		m_LayerListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LayerListCtrl.SetImageList( &m_ImageList, LVSIL_SMALL);
	}

	{
		m_ElmListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 180);
		m_ElmListCtrl.InsertColumn( 1, "Type", LVCFMT_LEFT, 130);
		m_ElmListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_ElmListCtrl.SetImageList( &m_ElmImageList, LVSIL_SMALL);
	}

	{
		m_hCursorArrow = LoadCursor(NULL, IDC_ARROW); // MAKEINTRESOURCE(IDC_ARROW));
		m_hCursorNo = LoadCursor(NULL, IDC_NO); // MAKEINTRESOURCE(IDC_NO));
	}

	{
		m_Tooltip.Create( this);

		//m_Tooltip.AddTool( this, "Layer 편집기");
		m_Tooltip.AddTool( &m_BTN_New,			"새 레이어 생성");
		m_Tooltip.AddTool( &m_BTN_Delete,		"레이어 삭제");
		m_Tooltip.AddTool( &m_BTN_Select,		"객체 선택");


		m_Tooltip.Activate( TRUE);
	}

	{
		_PlaceControl( -1, -1);
	}

	UpdateAllLayer();

	i3TDK::RegisterUpdate( m_hWnd, i3LevelScene::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_ADD | I3_TDK_UPDATE_REMOVE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLayer::OnOK()
{
}

void CDlgLayer::OnCancel()
{
	ShowWindow( SW_HIDE);
}

void CDlgLayer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		UpdateAllLayer();
	}
}

LRESULT CDlgLayer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgLayer::_PlaceControl( INT32 cx, INT32 cy)
{
	CRect rt;
	INT32 split;

	if( cx == -1)
	{
		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	split = cx >> 1;

	// Layer List Ctrl
	{
		m_LayerListCtrl.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_LayerListCtrl.SetWindowPos( NULL, 0, 0, split - rt.left, cy - rt.top - 3, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

		m_ElmListCtrl.SetWindowPos( NULL, split + 3, rt.top, cx - split - 6, cy - rt.top - 3, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CDlgLayer::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LayerListCtrl.m_hWnd))
	{
		_PlaceControl( cx, cy);
	}
}

void CDlgLayer::OnBnClickedBtnNew()
{
	i3LevelLayer * pNew;
	char conv[256];

	pNew = i3LevelLayer::new_object();

	g_pScene->GetUniqueLayerName( NULL,  conv);
	pNew->SetName( conv);

	g_pScene->addLayer( pNew);

	INT32 idx = m_LayerListCtrl.InsertItem( m_LayerListCtrl.GetItemCount(), conv, 0);

	UpdateLayer( idx, pNew);
}

void CDlgLayer::OnBnClickedBtnDelete()
{
	// 현재 선택된 Layer를 가져온다.
	INT32 idxSel = getSelectedLayer();
	i3LevelLayer * pSelLayer, * pCurLayer;
	INT32 i;

	if( idxSel == -1)
		return;

	pSelLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( idxSel);
	pCurLayer = g_pScene->getCurrentLayer();

	if( pSelLayer == pCurLayer)
	{
		// Current Layer가 삭제되는 경우라면 다른 Layer를 새로운 Current Layer로
		// 설정해준다.
		if( g_pScene->getLayerCount() <= 1)
		{
			AfxMessageBox( "Layer는 최소한 1개 이상 있어야 합니다. 삭제할 수 없습니다.");
			return;
		}
		else
		{
			if( idxSel > 0)
				pCurLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( 0);
			else
				pCurLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( 1);

			g_pScene->setCurrentLayer( pCurLayer);
		}
	}

	// SelLayer의 모든 Element들을 Current Layer로 이동시킨다.
	{
		for( i = 0; i < pSelLayer->getElementCount(); i++)
		{
			i3LevelElement3D * pElm = pSelLayer->getElement( i);

			pCurLayer->AddElement( pElm);
		}

		pSelLayer->RemoveAllElement();
	}

	g_pScene->removeLayer( pSelLayer);

	UpdateAllLayer();

}

void CDlgLayer::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow)
		UpdateAllLayer();
}

void CDlgLayer::OnLvnEndlabeleditLayerList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if( pDispInfo->item.pszText == NULL)
	{
		*pResult = 0;
		return;
	}
		
	i3LevelLayer * pLayer = (i3LevelLayer *) pDispInfo->item.lParam;

	if( g_pScene->FindLayerByName( pDispInfo->item.pszText) != -1)
	{
		NotifyBox( m_hWnd, "%s Layer 명은 이미 사용 중입니다.", pDispInfo->item.pszText);
		pDispInfo->item.pszText = NULL;

		*pResult = 0;
	}
	else
	{
		pLayer->SetName( pDispInfo->item.pszText);
		*pResult = 1;
	}
}

void CDlgLayer::OnNMDblclkLayerList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;

	GetCursorPos( &pt);

	m_LayerListCtrl.ScreenToClient( &pt);

	LVHITTESTINFO info;

	info.pt = pt;

	INT32 idx = m_LayerListCtrl.SubItemHitTest( &info);

	if( idx != -1)
	{
		if( info.flags & LVHT_ONITEMSTATEICON)
		{
			// Set current layer
			SetCurLayer( idx);
		}
		else
		{
			switch( info.iSubItem)
			{
				case 0 :			// Set current layer
					SetCurLayer( idx);
					break;

				case 1 :			// Show/Hide Toggle
					ToggleShow( idx);
					break;

				case 2 :			// Freeze/Unfreeze Toggle
					ToggleFreeze( idx);
					break;
			}
		}
	}

	*pResult = 0;
}

INT32	CDlgLayer::getSelectedLayer(void)
{
	POSITION pos = m_LayerListCtrl.GetFirstSelectedItemPosition();

	if( pos == NULL)
		return -1;

	INT32 idx = m_LayerListCtrl.GetNextSelectedItem( pos);

	return idx;
}

void CDlgLayer::OnNMClickLayerList(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateAllElements();

	*pResult = 0;
}

void CDlgLayer::OnLvnBegindragElmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CPoint pt, pt2;
	int cxCursor, cyCursor;

	cxCursor = 10;
	cyCursor = 10;

	GetCursorPos( &pt);
	
	SetCapture();

	m_bDrag = true;

	m_pDragImage = m_ElmListCtrl.CreateDragImage( pNMLV->iItem, &pt2);

	m_pDragImage->DragShowNolock( FALSE);
	m_pDragImage->BeginDrag( 0, CPoint( -cxCursor, -cyCursor));
	m_pDragImage->DragEnter( NULL, pt);

	*pResult = 0;
}

void CDlgLayer::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		CPoint pt = point;
		CPoint screenPt;

		ClientToScreen( &pt);

		screenPt = pt;

		m_LayerListCtrl.ScreenToClient( &pt);

		INT32 idx = m_LayerListCtrl.HitTest( pt);

		if( idx != -1)
		{
			SetCursor( m_hCursorArrow);
		}
		else
		{
			SetCursor( m_hCursorNo);
		}

		m_pDragImage->DragMove( screenPt);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgLayer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		ReleaseCapture();

		m_bDrag = false;

		m_pDragImage->DragLeave( &m_ElmListCtrl);
		m_pDragImage->EndDrag();

		delete m_pDragImage;

		DropElm( point);
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgLayer::_getSelectedElement( i3::vector<i3LevelElement3D *>& List)
{
	POSITION pos;
	INT32 idx;
	i3LevelElement3D * pElm;

	pos = m_ElmListCtrl.GetFirstSelectedItemPosition();

	while( pos != NULL)
	{
		idx = m_ElmListCtrl.GetNextSelectedItem( pos);

		pElm = (i3LevelElement3D *) m_ElmListCtrl.GetItemData( idx);

		List.push_back( pElm);
	}
}

void CDlgLayer::DropElm( CPoint pt)
{
	// Drop한 위치의 Layer에 Elm를 추가.
	ClientToScreen( &pt);

	m_LayerListCtrl.ScreenToClient( &pt);

	INT32 idx = m_LayerListCtrl.HitTest( pt);

	if( idx == -1)
		return;
	
	i3LevelLayer * pLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( idx);

	i3::vector<i3LevelElement3D *>	list;
	
	_getSelectedElement( list);

	for(size_t i = 0; i < list.size(); i++)
	{
		i3LevelElement3D * pElm = list[i];

		pElm->setLayer( pLayer);

		// Show state
		if( pLayer->getState() & I3LV_LAYER_STATE_HIDED)
			pElm->Disable();
		else
			pElm->Enable();

		// Freeze state
		if( pLayer->getState() & I3LV_LAYER_STATE_FREEZED)
			pElm->Freeze();
		else
			pElm->Unfreeze();
	}
	
	UpdateAllElements();
}

void CDlgLayer::OnBnClickedBtnSelectElm()
{
	
	i3::vector<i3LevelElement3D *>	list;
	i3LevelElement3D * pElm;

	_getSelectedElement( list);

	if( list.size() <= 0)
	{
		// Layer에서 선택 대상을 고른다.
		i3LevelLayer * pLayer;
		INT32 idx = getSelectedLayer();

		if( idx == -1)
			return;

		// 선택된 Layer의 모든 Element를 선택 대상으로 삼는다.
		pLayer = (i3LevelLayer *) m_LayerListCtrl.GetItemData( idx);

		for(INT32 i = 0; i < pLayer->getElementCount(); i++)
		{
			pElm = pLayer->getElement( i);

			list.push_back( pElm);
		}
	}

	g_pScene->UnselectAll();

	for(size_t i = 0; i < list.size(); i++)
	{
		pElm = list[i];

		g_pScene->SelectElement( pElm);
	}

}

void CDlgLayer::OnNMSetfocusElmList(NMHDR *pNMHDR, LRESULT *pResult)
{

	*pResult = 0;
}

BOOL CDlgLayer::PreTranslateMessage(MSG* pMsg)
{
	if( ::IsWindow( m_Tooltip.m_hWnd))
	{
		m_Tooltip.RelayEvent( pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgLayer::OnNMRClickElmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pInfo = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	{
		CPoint pt;
		CMenu menu, * pPopup;

		GetCursorPos( &pt);

		menu.LoadMenu( IDR_POPUP_LAYER_ELEMENT);

		pPopup = menu.GetSubMenu( 0);

		UINT32 cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

		switch( cmd)
		{
			case ID_OBJECT_DELETE :
				OnDeleteElement();
				break;

			case ID_OBJECT_SELECTFIT:
				OnSelectFit();
				break;

			default:
				break;
		}
	}
	
	*pResult = 0;
}

void CDlgLayer::OnNMDblclkElmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pInfo = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	OnSelectFit();
	
	*pResult = 0;
}

void CDlgLayer::OnDeleteElement(void)
{
	i3::vector<INT32> idxList;

	if( g_pScene == NULL)
		return;

	getSelectedElements( &idxList);

	for( size_t i = 0; i < idxList.size(); i++)
	{
		INT32 idx = idxList[i];

		i3LevelElement3D * pElm = (i3LevelElement3D *) m_ElmListCtrl.GetItemData( idx);	

		g_pScene->RemoveElement( pElm);

		m_ElmListCtrl.DeleteItem( idx);
	}
}

void CDlgLayer::OnSelectFit(void)
{
	if( g_pScene == NULL)
		return;

	g_pScene->UnselectAll();

	i3::vector<INT32> idxList;

	getSelectedElements( &idxList);

	for( size_t i = 0; i < idxList.size(); i++)
	{
		INT32 idx = idxList[i];

		i3LevelElement3D * pElm = (i3LevelElement3D *) m_ElmListCtrl.GetItemData( idx);	

		g_pScene->SelectElement( pElm);
	}

	{
		i3LevelFramework * pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();

		pFramework->Fit();
	}
}

void CDlgLayer::getSelectedElements( i3::vector< INT32> * pIdxList)
{
	INT32 idx;

	POSITION pos = m_ElmListCtrl.GetFirstSelectedItemPosition();

	while( pos != NULL)
	{
		idx = m_ElmListCtrl.GetNextSelectedItem( pos);

		pIdxList->push_back( idx);
	}
}
