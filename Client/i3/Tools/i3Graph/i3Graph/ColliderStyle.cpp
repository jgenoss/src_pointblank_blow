// ColliderStyle.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "ColliderStyle.h"
#include ".\colliderstyle.h"


// cColliderStyle dialog

IMPLEMENT_DYNAMIC(cColliderStyle, CDialog)
cColliderStyle::cColliderStyle(CWnd* pParent /*=NULL*/)
	: CDialog(cColliderStyle::IDD, pParent)
	, m_bDist(FALSE)
	, m_bPhys(FALSE)
	, m_bInter(FALSE)
	, m_bNor(FALSE)
	, m_bVel(FALSE)
{
	m_pColliderSet = NULL; 
}

cColliderStyle::~cColliderStyle()
{
}

void cColliderStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_DIST, m_bDist);
	DDX_Check(pDX, IDC_CHECK_PHYS, m_bPhys);
	DDX_Check(pDX, IDC_CHECK_INTER, m_bInter);
	DDX_Check(pDX, IDC_CHECK_NORMAL, m_bNor);
	DDX_Check(pDX, IDC_CHECK_VEL, m_bVel);
}


BEGIN_MESSAGE_MAP(cColliderStyle, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL cColliderStyle::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	if(m_pColliderSet == NULL)return FALSE; 
	
	INT32 Style = m_pColliderSet->GetStyle(); 

	if( Style & I3_COLLISION_STYLE_DISTANCE_CHECK	) m_bDist	= TRUE;
	if( Style & I3_COLLISION_STYLE_PHYSICS			) m_bPhys	= TRUE;
	if( Style & I3_COLLISION_STYLE_CALC_INTERSECT	) m_bInter	= TRUE;
	if( Style & I3_COLLISION_STYLE_CALC_NORMAL		) m_bNor	= TRUE;
	if( Style & I3_COLLISION_STYLE_VELOCITY			) m_bVel	= TRUE;
		
	UpdateData(FALSE);
	return TRUE; 
}

// cColliderStyle message handlers

void cColliderStyle::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_Style = 0; 
	if(m_bDist	== TRUE )m_Style =m_Style | I3_COLLISION_STYLE_DISTANCE_CHECK; 
	if(m_bPhys	== TRUE )m_Style =m_Style | I3_COLLISION_STYLE_PHYSICS; 
	if(m_bInter	== TRUE )m_Style =m_Style | I3_COLLISION_STYLE_CALC_INTERSECT; 
	if(m_bNor	== TRUE )m_Style =m_Style | I3_COLLISION_STYLE_CALC_NORMAL; 
	if(m_bVel	== TRUE )m_Style =m_Style | I3_COLLISION_STYLE_VELOCITY; 
	OnOK();
}
