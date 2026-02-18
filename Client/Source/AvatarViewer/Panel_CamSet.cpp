// Panel_CamSet.cpp : implementation file
//

#include "pch.h"
#include "AvatarViewer.h"
#include "Panel_CamSet.h"
#include "i3Base/itl/range/generic_empty.h"
#include "i3Base/string/ext/contain_string.h"

IMPLEMENT_DYNAMIC(CPanel_CamSet, CDockablePane)

CPanel_CamSet::CPanel_CamSet()
{
}

CPanel_CamSet::~CPanel_CamSet()
{
}


BEGIN_MESSAGE_MAP(CPanel_CamSet, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_UPDATE_COMMAND_UI(IDC_BTN_CAM_APPLY, OnUpdateBtn)
	ON_BN_CLICKED( IDC_BTN_CAM_APPLY,OnBtnClick)
END_MESSAGE_MAP()

void CPanel_CamSet::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
}

int CPanel_CamSet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if( m_btnApply.Create( "Apply", WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_BTN_CAM_APPLY ) == FALSE)
	{
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_btnApply.SetWindowPos(NULL, rectClient.left, rectClient.top , rectClient.Width(), 30, SWP_NOACTIVATE | SWP_NOZORDER);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		I3TRACE("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pTrans = new CMFCPropertyGridProperty("Translation");
	{
		CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty( "X", (_variant_t) 0.f, "Translation X value");
		pTrans->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( "Y", (_variant_t) 0.f, "Translation Y value");
		pTrans->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( "Z", (_variant_t) 0.f, "Translation Z value");
		pTrans->AddSubItem(pProp);
	}
	m_wndPropList.AddProperty(pTrans);

	CMFCPropertyGridProperty* pRot = new CMFCPropertyGridProperty("Rotation");
	{
		CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty( "Theta", (_variant_t) 0.f, "Theta value");
		pRot->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( "Rho", (_variant_t) 0.f, "Rho value");
		pRot->AddSubItem(pProp);
	}
	m_wndPropList.AddProperty(pRot);

	{
		REAL32 fov = I3_RAD2DEG(g_pFramework->GetCurrentCamera()->GetFOV());
		CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty( "FOV", (_variant_t) fov, "FOV value");
		m_wndPropList.AddProperty( pProp);
	}

	i3Vector::Copy( &m_vTranslation, &g_pOption->m_vTranslation);
	i3Vector::Copy( &m_vRotation, &g_pOption->m_vRotation);

	SetCamera();
	
	return 0;
}

void CPanel_CamSet::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	
	if (GetSafeHwnd() == NULL)
		return;

	CRect rectClient;
	GetClientRect(rectClient);

	m_btnApply.SetWindowPos(NULL, rectClient.left, rectClient.top , rectClient.Width(), 30, SWP_NOACTIVATE | SWP_NOZORDER);
	
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + 30, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	
}

void CPanel_CamSet::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

LRESULT CPanel_CamSet::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty * pProp = (CMFCPropertyGridProperty*)lparam;

	if (pProp == NULL ) return 0;

	GetPropItem( pProp );

	return 1;
}

void CPanel_CamSet::Init()
{
	int nPropCnt = m_wndPropList.GetPropertyCount();
	for( int nProp = 0 ; nProp < nPropCnt ; ++nProp )
	{
		CMFCPropertyGridProperty * pProp = m_wndPropList.GetProperty( nProp );
		int nSubItemCnt = pProp->GetSubItemsCount();
		for( INT32 nSubItem = 0; nSubItem < nSubItemCnt; ++nSubItem )
		{
			CMFCPropertyGridProperty * pSubItem = pProp->GetSubItem( nSubItem );
			if( pSubItem != NULL)
				SetPropItem( pSubItem );
		}
	}
}

void CPanel_CamSet::GetPropItem( CMFCPropertyGridProperty* pProp )
{
	if( NULL == pProp )return;
	CString temp = pProp->GetDescription();
	COleVariant rVariant = pProp->GetValue();	
	if( i3::generic_is_iequal( temp.GetString(), "Translation X value" ) ) m_vTranslation.x  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), "Translation Y value" ) ) m_vTranslation.y  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), "Translation Z value" ) ) m_vTranslation.z  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), "Theta value" ) ) m_vRotation.x  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), "Rho value" ) ) m_vRotation.y  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), "FOV value" ) ) m_rFOV  = rVariant.fltVal;
}

void CPanel_CamSet::SetPropItem( CMFCPropertyGridProperty* pProp )
{
	if( NULL == pProp )return;
	CString temp = pProp->GetDescription();
	pProp->Enable(TRUE);
	if(i3::generic_is_iequal( temp.GetString(), "Translation X value" ) ) { pProp->SetValue( (_variant_t) m_vTranslation.x ); }
	else if(i3::generic_is_iequal( temp.GetString(), "Translation Y value" ) ) { pProp->SetValue( (_variant_t) m_vTranslation.y ); }
	else if(i3::generic_is_iequal( temp.GetString(), "Translation Z value" ) ) { pProp->SetValue( (_variant_t) m_vTranslation.z ); }
	else if(i3::generic_is_iequal( temp.GetString(), "Theta value" ) ) { pProp->SetValue( (_variant_t) m_vRotation.x ); }
	else if(i3::generic_is_iequal( temp.GetString(), "Rho value" ) ) { pProp->SetValue( (_variant_t) m_vRotation.y ); }
	else if( i3::generic_is_iequal( temp.GetString(), "FOV value" ) ) { pProp->SetValue( (_variant_t) m_rFOV ); }
}

void CPanel_CamSet::SetCamera( void)
{
	m_vTranslation.x = getX( i3Matrix::GetPos( g_pFramework->GetCurrentCamera()->GetMatrix()));
	m_vTranslation.y = getY( i3Matrix::GetPos( g_pFramework->GetCurrentCamera()->GetMatrix()));
	m_vTranslation.z = getZ( i3Matrix::GetPos( g_pFramework->GetCurrentCamera()->GetMatrix()));

	VEC3D vAt;
	i3Vector::Copy( &vAt, i3Matrix::GetAt( g_pFramework->GetCurrentCamera()->GetMatrix()));
	m_vRotation.x = I3_RAD2DEG( atan2f( getX( &vAt), getZ( &vAt)));

	REAL32 fLen;
	fLen = i3Math::Sqrt( (getX(&vAt) * getX(&vAt)) + (getZ(&vAt) * getZ(&vAt)) );
	m_vRotation.y = I3_RAD2DEG( atan2f(getY(&vAt), fLen));

	m_rFOV = I3_RAD2DEG( g_pFramework->GetCurrentCamera()->GetFOV());

	Init();
}

void CPanel_CamSet::OnUpdateBtn(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CPanel_CamSet::OnBtnClick(void)
{
	ApplyCamera();
}

void CPanel_CamSet::ApplyCamera( void)
{
	I3ASSERT( g_pFramework->GetCurrentCamera() != NULL);
	i3Matrix::Identity( g_pFramework->GetCurrentCamera()->GetMatrix());
	i3Matrix::SetRotateYDeg( g_pFramework->GetCurrentCamera()->GetMatrix(), m_vRotation.x);
	i3Matrix::PostRotateAxis( g_pFramework->GetCurrentCamera()->GetMatrix(), i3Matrix::GetRight( g_pFramework->GetCurrentCamera()->GetMatrix()), I3_DEG2RAD(-m_vRotation.y));// + 0.15f);
	VEC3D * pCamPos = i3Matrix::GetPos( g_pFramework->GetCurrentCamera()->GetMatrix());
	i3Vector::Add( pCamPos, pCamPos, &m_vTranslation);

	g_pFramework->GetCurrentCamera()->SetFOV( I3_DEG2RAD( m_rFOV));
}
