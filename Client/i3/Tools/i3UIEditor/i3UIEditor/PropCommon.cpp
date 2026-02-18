// PropCommon.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "PropCommon.h"
#include "GlobalVariable.h"


// CPropCommon dialog

IMPLEMENT_DYNAMIC(CPropCommon, CDialog)
CPropCommon::CPropCommon(CWnd* pParent /*=NULL*/)
	: CDialog(CPropCommon::IDD, pParent)
{
	m_pControl  = NULL;
	m_pShape	= NULL;
}

CPropCommon::~CPropCommon()
{
}

void CPropCommon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_COMMON_POSX, m_CtrlPosX);
	DDX_Control(pDX, IDC_PROP_COMMON_POSY, m_CtrlPosY);
	DDX_Control(pDX, IDC_PROP_COMMON_WIDTH, m_CtrlWidth);
	DDX_Control(pDX, IDC_PROP_COMMON_HEIGHT, m_CtrlHeight);
	DDX_Control(pDX, IDC_PROP_COMMON_NAME, m_Control_ID);
}


BEGIN_MESSAGE_MAP(CPropCommon, CDialog)
	ON_EN_CHANGE(IDC_PROP_COMMON_POSX, OnEnChangePropCommonPosx)
	ON_EN_CHANGE(IDC_PROP_COMMON_NAME, OnEnChangePropCommonName)
END_MESSAGE_MAP()


// CPropCommon message handlers
void CPropCommon::SetShapeInfo( i3UIImage * pImg)
{
	char conv[256] = {0,};

	m_pShape	= pImg;
	m_pControl	= NULL;

	if( m_pShape == NULL )
	{
		sprintf(conv, "%d",0);

		m_CtrlPosX.SetWindowText(conv);
		m_CtrlPosY.SetWindowText(conv);
		m_CtrlWidth.SetWindowText(conv);
		m_CtrlHeight.SetWindowText(conv);
	}
	else
	{
		i3String::ftoa( m_pShape->m_nX, conv);
		m_CtrlPosX.SetWindowText(conv);

		i3String::ftoa( m_pShape->m_nY, conv);
		m_CtrlPosY.SetWindowText(conv);

		i3String::ftoa( m_pShape->m_nWidth, conv);
		m_CtrlWidth.SetWindowText(conv);

		i3String::ftoa( m_pShape->m_nHeight, conv);
		m_CtrlHeight.SetWindowText(conv);
	}

	m_CtrlPosX.EnableWindow( m_pShape != NULL );
	m_CtrlPosY.EnableWindow( m_pShape != NULL );
	m_CtrlWidth.EnableWindow( m_pShape != NULL );
	m_CtrlHeight.EnableWindow( m_pShape != NULL );

	UpdateData( FALSE);
}

void	CPropCommon::SetCommonInfo(i3UIControl * pCtrl)
{
	char conv[256] = {0,};

	m_pControl	= pCtrl;
	m_pShape	= NULL;

	if( pCtrl == NULL )
	{
		sprintf(conv, "%d",0);

		m_CtrlPosX.SetWindowText(conv);
		m_CtrlPosY.SetWindowText(conv);
		m_CtrlWidth.SetWindowText(conv);
		m_CtrlHeight.SetWindowText(conv);

		m_Control_ID.SetWindowText("");
	}
	else
	{
		i3String::ftoa( pCtrl->getPositionX(), conv);
		m_CtrlPosX.SetWindowText(conv);

		i3String::ftoa( pCtrl->getPositionY(), conv);
		m_CtrlPosY.SetWindowText(conv);

		i3String::ftoa( pCtrl->getWidth(), conv);
		m_CtrlWidth.SetWindowText(conv);

		i3String::ftoa( pCtrl->getHeight(), conv);
		m_CtrlHeight.SetWindowText(conv);


		m_Control_ID.SetWindowText( m_pControl->GetName());
	}

	m_CtrlPosX.EnableWindow( m_pControl != NULL );
	m_CtrlPosY.EnableWindow( m_pControl != NULL );
	m_CtrlWidth.EnableWindow( m_pControl != NULL );
	m_CtrlHeight.EnableWindow( m_pControl != NULL );

	UpdateData( FALSE);
}

void CPropCommon::UpdateInfo()
{
	char conv[256];
	if( m_pControl != NULL)
	{
		i3String::ftoa( m_pControl->getPositionX(), conv);
		m_CtrlPosX.SetWindowText(conv);

		i3String::ftoa( m_pControl->getPositionY(), conv);
		m_CtrlPosY.SetWindowText(conv);

		i3String::ftoa( m_pControl->getWidth(), conv);
		m_CtrlWidth.SetWindowText(conv);

		i3String::ftoa( m_pControl->getHeight(), conv);
		m_CtrlHeight.SetWindowText(conv);
	}
	else if( m_pShape != NULL)
	{
		i3String::ftoa( m_pShape->m_nX, conv);
		m_CtrlPosX.SetWindowText(conv);

		i3String::ftoa( m_pShape->m_nY, conv);
		m_CtrlPosY.SetWindowText(conv);

		i3String::ftoa( m_pShape->m_nWidth, conv);
		m_CtrlWidth.SetWindowText(conv);

		i3String::ftoa( m_pShape->m_nHeight, conv);
		m_CtrlHeight.SetWindowText(conv);
	}
}

BOOL CPropCommon::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetCommonInfo( NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropCommon::OnOK()
{
	char conv[256];

	REAL32 rX, rY;
	INT32  nWidth, nHeight;

	UpdateData( TRUE);

	// TODO: Add your specialized code here and/or call the base class
	m_CtrlPosX.GetWindowText(conv, sizeof(conv) -1);
	rX = (REAL32)atof(conv);

	m_CtrlPosY.GetWindowText(conv, sizeof(conv) -1);
	rY = (REAL32)atof(conv);

	m_CtrlWidth.GetWindowText(conv, sizeof(conv) -1);
	nWidth = atoi( conv);

	m_CtrlHeight.GetWindowText(conv, sizeof(conv) -1);
	nHeight = atoi( conv);

	if( m_pControl != NULL)
	{
		m_pControl->setPosition( rX, rY);
		
		if (m_pControl->IsTypeOf(i3UIControl::GetClassMeta()))
		{	
			// 쉐이프가 있을때만, 적용
			if (((i3UIControl*)m_pControl)->GetControlData()->m_pShape)
			{
				((i3UIControl*)m_pControl)->setSize( (REAL32) nWidth, (REAL32) nHeight);

				i3TDK::Update(m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_PROPERTY, m_pControl);
			}
		}

		{
			char szTemp[1024]= {0,};

			if( m_Control_ID.GetModify())
			{
				m_Control_ID.GetWindowText( szTemp, 1024);
				m_pControl->SetName( szTemp);

				i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_PROPERTY, m_pControl );
			}
		}
	}
	else if( m_pShape != NULL)
	{
		m_pShape->m_nX = rX;
		m_pShape->m_nY = rY;
		m_pShape->setWidth( (REAL32)nWidth);
		m_pShape->setHeight( (REAL32)nHeight);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_PROPERTY, m_pShape );
	}

	UpdateInfo();
}

void CPropCommon::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if( (m_pControl == NULL) && (m_pShape == NULL))	return ;

	UpdateInfo();
}

void CPropCommon::OnEnChangePropCommonPosx()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CPropCommon::OnEnChangePropCommonName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	
}
