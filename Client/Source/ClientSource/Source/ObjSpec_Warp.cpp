// Prop_LvRes_SceneGraph.cpp : implementation file
//

#include "pch.h"

#if defined(USE_EDITDLG)	// CObjSpec_Warp
#include "ObjSpec_Warp.h"
#include "GameObject.h"
#include "i3Level/i3LevelElement3D.h"
#include "i3Base/string/ext/ftoa.h"

IMPLEMENT_DYNAMIC(CObjSpec_Warp, i3GameObjPropertyDialog)

CObjSpec_Warp::CObjSpec_Warp(CWnd* pParent /*=nullptr*/)
	: i3GameObjPropertyDialog( CObjSpec_Warp::IDD, pParent)
{
}

CObjSpec_Warp::~CObjSpec_Warp()
{
}

void CObjSpec_Warp::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_PB_ED_WARPX,	m_ED_X);
	DDX_Control( pDX, IDC_PB_ED_WARPY,	m_ED_Y);
	DDX_Control( pDX, IDC_PB_ED_WARPZ,	m_ED_Z);
}


BEGIN_MESSAGE_MAP(CObjSpec_Warp, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_PB_ED_WARPX, OnEnKillfocusPbEdWarpx)
	ON_EN_KILLFOCUS(IDC_PB_ED_WARPY, OnEnKillfocusPbEdWarpy)
	ON_EN_KILLFOCUS(IDC_PB_ED_WARPZ, OnEnKillfocusPbEdWarpz)
END_MESSAGE_MAP()


// CObjSpec_Warp message handlers
void CObjSpec_Warp::SetObject( i3ElementBase * pObj)
{
	char conv[128];

	i3GameObjPropertyDialog::SetObject( pObj);

	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectJumper * pJumper = (CGameObjectJumper *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<CGameObjectJumper*>(pJumper));

	i3::ftoa( pJumper->getJumpPos()->x, conv);
	m_ED_X.SetWindowText( conv);

	i3::ftoa( pJumper->getJumpPos()->y, conv);
	m_ED_Y.SetWindowText( conv);

	i3::ftoa( pJumper->getJumpPos()->z, conv);
	m_ED_Z.SetWindowText( conv);

	I3_MUST_RELEASE(pJumper);
}

BOOL CObjSpec_Warp::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	return true;  // return true unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void CObjSpec_Warp::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}

int CObjSpec_Warp::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_Warp::OnEnKillfocusPbEdWarpx()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectJumper * pJumper = (CGameObjectJumper *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<CGameObjectJumper*>(pJumper));

	if( m_ED_X.GetModify())
	{
		m_ED_X.GetWindowText( conv, sizeof(conv) - 1);

		pJumper->getJumpPos()->x = (REAL32) atof( conv);

		m_ED_X.SetModify( false);
	}

	I3_MUST_RELEASE(pJumper);
}

void CObjSpec_Warp::OnEnKillfocusPbEdWarpy()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectJumper * pJumper = (CGameObjectJumper *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<CGameObjectJumper*>(pJumper));

	if( m_ED_Y.GetModify())
	{
		m_ED_Y.GetWindowText( conv, sizeof(conv) - 1);

		pJumper->getJumpPos()->y = (REAL32) atof( conv);

		m_ED_Y.SetModify( false);
	}

	I3_MUST_RELEASE(pJumper);
}

void CObjSpec_Warp::OnEnKillfocusPbEdWarpz()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectJumper * pJumper = (CGameObjectJumper *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<CGameObjectJumper*>(pJumper));

	if( m_ED_Z.GetModify())
	{
		m_ED_Z.GetWindowText( conv, sizeof(conv) - 1);

		pJumper->getJumpPos()->z = (REAL32) atof( conv);

		m_ED_Z.SetModify( false);
	}

	I3_MUST_RELEASE(pJumper);
}

void CObjSpec_Warp::OnOK()
{
	OnEnKillfocusPbEdWarpx();
	OnEnKillfocusPbEdWarpy();
	OnEnKillfocusPbEdWarpz();

}
void CObjSpec_Warp::OnCancel()
{
}

#endif