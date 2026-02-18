// PropLight.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "PropLight.h"
#include "ColorDlg.h"
#include ".\proplight.h"


// CPropLight dialog

IMPLEMENT_DYNAMIC(CPropLight, i3TDKDialogBase)
CPropLight::CPropLight(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPropLight::IDD, pParent)
{
	i3Color::Set( &m_ColAmbient, 0.0f, 0.0f, 0.0f, 0.0f);
	i3Color::Set( &m_ColDiffuse, 0.0f, 0.0f, 0.0f, 0.0f);
	i3Color::Set( &m_ColSpecular, 0.0f, 0.0f, 0.0f, 0.0f);
}

CPropLight::~CPropLight()
{
}

void CPropLight::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_AMBIENT, m_stAmbient);
	DDX_Control(pDX, IDC_STATIC_DIFFUSE, m_stDiffuse);
	DDX_Control(pDX, IDC_STATIC_SPECULAR, m_stSpecular);
	DDX_Control(pDX, IDC_BTN_SEL_AMBIENT, m_btnSelAmbient);
	DDX_Control(pDX, IDC_BTN_SEL_DIFFUSE, m_btnSelDiffuse);
	DDX_Control(pDX, IDC_BTN_SEL_SPECULAR, m_btnSelSpecular);
	DDX_Control(pDX, IDC_CB_LIGHT_TYPE, m_cbLightType);
	DDX_Control(pDX, IDC_ED_ATTENUATION_X, m_edAttenuation_X);
	DDX_Control(pDX, IDC_ED_ATTENUATION_Y, m_edAttenuation_Y);
	DDX_Control(pDX, IDC_ED_ATTENUATION_Z, m_edAttenuation_Z);
	DDX_Control(pDX, IDC_ED_CUTOFFANGLE, m_edCutoffAngle);
	DDX_Control(pDX, IDC_ED_FALLOFFRATE, m_edFalloffRate);
	DDX_Control(pDX, IDC_ED_RANGE, m_edRange);
	DDX_Control(pDX, IDC_SPOTLIGHT_ANGLE, m_edSpotlight_Angle);
}

BEGIN_MESSAGE_MAP(CPropLight, i3TDKDialogBase)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_SEL_AMBIENT, OnBnClickedBtnSelAmbient)
	ON_BN_CLICKED(IDC_BTN_SEL_DIFFUSE, OnBnClickedBtnSelDiffuse)
	ON_BN_CLICKED(IDC_BTN_SEL_SPECULAR, OnBnClickedBtnSelSpecular)
	ON_CBN_SELCHANGE(IDC_CB_LIGHT_TYPE, OnCbnSelchangeCbLightType)
	ON_EN_KILLFOCUS(IDC_ED_ATTENUATION_X, OnEnKillfocusEdAttenuationX)
	ON_EN_KILLFOCUS(IDC_ED_ATTENUATION_Y, OnEnKillfocusEdAttenuationY)
	ON_EN_KILLFOCUS(IDC_ED_ATTENUATION_Z, OnEnKillfocusEdAttenuationZ)
	ON_EN_KILLFOCUS(IDC_ED_CUTOFFANGLE, OnEnKillfocusEdCutoffangle)
	ON_EN_KILLFOCUS(IDC_ED_FALLOFFRATE, OnEnKillfocusEdFalloffrate)
	ON_EN_KILLFOCUS(IDC_ED_RANGE, OnEnKillfocusEdRange)
	ON_EN_KILLFOCUS(IDC_SPOTLIGHT_ANGLE, OnEnKillfocusSpotlightAngle)
END_MESSAGE_MAP()

// CPropLight message handlers
void CPropLight::SetObject( i3ElementBase * pObj)
{
	if( m_pLight == pObj)
		return ;

	m_pLight = NULL;

	if( !i3::same_of<i3LevelLight*>(pObj)) //->IsExactTypeOf( i3LevelLight::static_meta()) == FALSE )
	{
		m_btnSelAmbient.EnableWindow( FALSE);
		m_btnSelDiffuse.EnableWindow( FALSE);
		m_btnSelSpecular.EnableWindow( FALSE);
		return ;
	}

	SetLight( (i3LevelLight*)pObj);
}

void CPropLight::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_hwndFrom != m_hWnd)
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
		{
			if( (pInfo->m_pObject != NULL) && i3::kind_of<i3LevelLight*>(pInfo->m_pMeta)) //->IsTypeOf( i3LevelLight::static_meta()) )
			{
				SetLight( m_pLight);
			}
		}
	}
}

LRESULT CPropLight::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}


void CPropLight::SetLight( i3LevelLight * pLight)
{
    m_pLight = pLight;

	char	conv[MAX_PATH];
	i3LightAttr * pLightAttr = m_pLight->getLightAttr();
	if( pLightAttr == NULL)
		return;

	{
		INT32 nCurLightType = (INT32)pLightAttr->getLightType();
		m_cbLightType.SetCurSel( nCurLightType);
		m_cbLightType.UpdateData();
	}

	{
		//i3Color::Set( &m_ColAmbient, pLightAttr->getAmbient());
		i3Color::Set( &m_ColDiffuse, pLightAttr->getDiffuse());
		i3Color::Set( &m_ColSpecular, pLightAttr->getSpecular());
	}

	{
		m_edAttenuation_X.EnableWindow( FALSE);
		m_edAttenuation_Y.EnableWindow( FALSE);
		m_edAttenuation_Z.EnableWindow( FALSE);
		/*
		VEC3D * pAttenuation = pLightAttr->GetAttenuation();

		sprintf( conv, "%f", getX( pAttenuation));
		m_edAttenuation_X.SetWindowText( (LPCTSTR)conv);

		sprintf( conv, "%f", getY( pAttenuation));
		m_edAttenuation_Y.SetWindowText( (LPCTSTR)conv);

		sprintf( conv, "%f", getZ( pAttenuation));
		m_edAttenuation_Z.SetWindowText( (LPCTSTR)conv);
		*/
	}

	{
		sprintf( conv, "%f", pLightAttr->getOuterAngle());
		m_edCutoffAngle.SetWindowText( (LPCTSTR)conv);

		//sprintf( conv, "%f", pLightAttr->GetFalloffRate());
		//m_edFalloffRate.SetWindowText( (LPCTSTR)conv);
		m_edFalloffRate.EnableWindow( FALSE);

		sprintf( conv, "%f", pLightAttr->getRange());
		m_edRange.SetWindowText( (LPCTSTR)conv);

		sprintf( conv, "%f", pLightAttr->getInnerAngle());
		m_edSpotlight_Angle.SetWindowText( (LPCTSTR)conv);
	}

	Invalidate();
}

void CPropLight::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect Rect;
	COLORREF Color;	
	HBRUSH OldBrush, NewBrush;

	//Ambient
	{
		UINT8 R = (UINT8)( i3Color::GetR( &m_ColAmbient) * 255.0f);
		UINT8 G = (UINT8)( i3Color::GetG( &m_ColAmbient) * 255.0f);
		UINT8 B = (UINT8)( i3Color::GetB( &m_ColAmbient) * 255.0f);

		Color = RGB( R, G, B);
		NewBrush = CreateSolidBrush(Color);
		
		ASSERT(NewBrush);

		OldBrush = (HBRUSH) dc.SelectObject(NewBrush);

		m_stAmbient.GetWindowRect( &Rect);
		ScreenToClient( &Rect);
		dc.Rectangle(&Rect);
		dc.SelectObject(OldBrush);
		DeleteObject(NewBrush);
	}

		//Ambient
	{
		UINT8 R = (UINT8)( i3Color::GetR( &m_ColDiffuse) * 255.0f);
		UINT8 G = (UINT8)( i3Color::GetG( &m_ColDiffuse) * 255.0f);
		UINT8 B = (UINT8)( i3Color::GetB( &m_ColDiffuse) * 255.0f);

		Color = RGB( R, G, B);
		NewBrush = CreateSolidBrush(Color);
		
		ASSERT(NewBrush);

		OldBrush = (HBRUSH) dc.SelectObject(NewBrush);

		m_stDiffuse.GetWindowRect( &Rect);
		ScreenToClient( &Rect);
		dc.Rectangle(&Rect);
		dc.SelectObject(OldBrush);
		DeleteObject(NewBrush);
	}

		//Ambient
	{
		UINT8 R = (UINT8)( i3Color::GetR( &m_ColSpecular) * 255.0f);
		UINT8 G = (UINT8)( i3Color::GetG( &m_ColSpecular) * 255.0f);
		UINT8 B = (UINT8)( i3Color::GetB( &m_ColSpecular) * 255.0f);

		Color = RGB( R, G, B);
		NewBrush = CreateSolidBrush(Color);
		
		ASSERT(NewBrush);

		OldBrush = (HBRUSH) dc.SelectObject(NewBrush);

		m_stSpecular.GetWindowRect( &Rect);
		ScreenToClient( &Rect);
		dc.Rectangle(&Rect);
		dc.SelectObject(OldBrush);
		DeleteObject(NewBrush);
	}
}

void CPropLight::OnBnClickedBtnSelAmbient()
{
	// TODO: Add your control notification handler code here
	i3TDKColorPicker ColorPicker; 

	if(ColorPicker.Execute( &m_ColAmbient) == true)
	{
		//Save Color Proeprty

		i3Color::Set( &m_ColAmbient, ColorPicker.getSelectedColor());

		//m_pLight->getLightAttr()->setAmbient( &m_ColAmbient);

		i3LevelRes * pRes = m_pLight->getRes();

		pRes->addResState( I3LV_RES_STATE_MODIFIED);

		Invalidate();
	}
}

void CPropLight::OnBnClickedBtnSelDiffuse()
{
	// TODO: Add your control notification handler code here
	i3TDKColorPicker ColorPicker; 

	if(ColorPicker.Execute( &m_ColDiffuse) == true)
	{
		//Save Color Proeprty
		i3Color::Set( &m_ColDiffuse, ColorPicker.getSelectedColor());

		m_pLight->getLightAttr()->setDiffuse( &m_ColDiffuse);

		i3LevelRes * pRes = m_pLight->getRes();

		pRes->addResState( I3LV_RES_STATE_MODIFIED);

		Invalidate();
	}
}

void CPropLight::OnBnClickedBtnSelSpecular()
{
	// TODO: Add your control notification handler code here
	i3TDKColorPicker ColorPicker; 

	if(ColorPicker.Execute( &m_ColSpecular) == true)
	{
		//Save Color Proeprty
		i3Color::Set( &m_ColSpecular, ColorPicker.getSelectedColor());

		m_pLight->getLightAttr()->setSpecular( &m_ColSpecular);

		i3LevelRes * pRes = m_pLight->getRes();

		pRes->addResState( I3LV_RES_STATE_MODIFIED);

		Invalidate();
	}
}

void CPropLight::OnOK()
{
}

void CPropLight::OnCancel()
{
}

void CPropLight::OnCbnSelchangeCbLightType()
{
	// TODO: Add your control notification handler code here
	INT32	nCurSel = m_cbLightType.GetCurSel();

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			pLightAttr->setLightType( (I3G_LIGHT_TYPE)nCurSel);
		}
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

void CPropLight::OnEnKillfocusEdAttenuationX()
{
	// TODO: Add your control notification handler code here
	char conv[MAX_PATH];

	m_edAttenuation_X.GetWindowText((LPSTR)conv, MAX_PATH - 1);

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			//VEC3D	vAttenuation;
			//i3Vector::Copy( &vAttenuation, pLightAttr->GetAttenuation());

			//setX( &vAttenuation, (REAL32)atof( conv));

			//pLightAttr->SetAttenuation( &vAttenuation);
		}
	}

	m_edAttenuation_X.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

void CPropLight::OnEnKillfocusEdAttenuationY()
{
	// TODO: Add your control notification handler code here
	char conv[MAX_PATH];

	m_edAttenuation_Y.GetWindowText((LPSTR)conv, MAX_PATH - 1);

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			/*
			VEC3D	vAttenuation;
			i3Vector::Copy( &vAttenuation, pLightAttr->GetAttenuation());

			setY( &vAttenuation, (REAL32)atof( conv));

			//pLightAttr->SetAttenuation( &vAttenuation);
			*/
		}
	}

	m_edAttenuation_Y.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

void CPropLight::OnEnKillfocusEdAttenuationZ()
{
	// TODO: Add your control notification handler code here
	char conv[MAX_PATH];

	m_edAttenuation_Z.GetWindowText((LPSTR)conv, MAX_PATH - 1);

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			/*
			VEC3D	vAttenuation;
			i3Vector::Copy( &vAttenuation, pLightAttr->GetAttenuation());

			setZ( &vAttenuation, (REAL32)atof( conv));

			//pLightAttr->SetAttenuation( &vAttenuation);
			*/
		}
	}

	m_edAttenuation_Z.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

void CPropLight::OnEnKillfocusEdCutoffangle()
{
	// TODO: Add your control notification handler code here
	char conv[MAX_PATH];

	m_edCutoffAngle.GetWindowText((LPSTR)conv, MAX_PATH - 1);

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			pLightAttr->setOuterAngle( (REAL32)atof( conv));
		}
	}

	m_edCutoffAngle.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

void CPropLight::OnEnKillfocusEdFalloffrate()
{
	// TODO: Add your control notification handler code here
	char conv[MAX_PATH];

	m_edFalloffRate.GetWindowText((LPSTR)conv, MAX_PATH - 1);

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			//pLightAttr->SetFalloffRate( (REAL32)atof( conv));
		}
	}

	m_edFalloffRate.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

void CPropLight::OnEnKillfocusEdRange()
{
	// TODO: Add your control notification handler code here
	char conv[MAX_PATH];

	m_edRange.GetWindowText((LPSTR)conv, MAX_PATH - 1);

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			pLightAttr->setRange( (REAL32)atof( conv));
		}
	}

	m_edRange.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

void CPropLight::OnEnKillfocusSpotlightAngle()
{
	// TODO: Add your control notification handler code here
	char conv[MAX_PATH];

	m_edSpotlight_Angle.GetWindowText((LPSTR)conv, MAX_PATH - 1);

	if( m_pLight != NULL)
	{
		i3LightAttr * pLightAttr = m_pLight->getLightAttr();
		if( pLightAttr != NULL)
		{
			pLightAttr->setInnerAngle( (REAL32)atof( conv));
		}
	}

	m_edSpotlight_Angle.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLight);
}

BOOL CPropLight::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	m_cbLightType.AddString( "I3G_LIGHT_NONE");
	m_cbLightType.AddString( "I3G_LIGHT_POINT");
	m_cbLightType.AddString( "I3G_LIGHT_SPOT");
	m_cbLightType.AddString( "I3G_LIGHT_DIRECTIONAL");

	i3Level::RegisterUpdateNotify( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_EDIT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
