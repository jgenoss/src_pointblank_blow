// Prop_LvRes_SceneGraph.cpp : implementation file
//
#include "pch.h"
#if defined(USE_EDITDLG)
#include "ObjSpec_RespawnHelicopter.h"
#include "GameObject.h"
#include ".\objspec_respawnhelicopter.h"
#include "i3Level/i3LevelElement3D.h"


IMPLEMENT_DYNAMIC(CObjSpec_RespawnHelicopter, i3GameObjPropertyDialog)

CObjSpec_RespawnHelicopter::CObjSpec_RespawnHelicopter(CWnd* pParent /*=nullptr*/)
	: i3GameObjPropertyDialog( CObjSpec_RespawnHelicopter::IDD, pParent)
{
}

CObjSpec_RespawnHelicopter::~CObjSpec_RespawnHelicopter()
{
}

void CObjSpec_RespawnHelicopter::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_EDIT_TEAM_INDEX, m_ED_TeamIdx);
}


BEGIN_MESSAGE_MAP(CObjSpec_RespawnHelicopter, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_EDIT_TEAM_INDEX, OnEnKillfocusPbEdTeamIdx)
END_MESSAGE_MAP()


// CObjSpec_RespawnHelicopter message handlers
void CObjSpec_RespawnHelicopter::SetObject( i3ElementBase * pObj)
{
	char conv[128];

	i3GameObjPropertyDialog::SetObject( pObj);
	
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectRespawnHelicopter * pObject = (CGameObjectRespawnHelicopter *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<CGameObjectRespawnHelicopter*>(pObject));

	itoa( pObject->getTeamIdx(), conv, 128);
	m_ED_TeamIdx.SetWindowText( conv);

	I3_MUST_RELEASE(pObject);
}

BOOL CObjSpec_RespawnHelicopter::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	i3TDK::RegisterUpdate( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_EDIT);

	return true;  // return true unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void CObjSpec_RespawnHelicopter::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}

int CObjSpec_RespawnHelicopter::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_RespawnHelicopter::OnOK()
{
}

void CObjSpec_RespawnHelicopter::OnCancel()
{
}

void CObjSpec_RespawnHelicopter::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( getObject() == nullptr)
		return;
}

LRESULT CObjSpec_RespawnHelicopter::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3GameObjPropertyDialog::WindowProc(message, wParam, lParam);
}

void CObjSpec_RespawnHelicopter::OnEnKillfocusPbEdTeamIdx()
{
	char conv[128];

	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectRespawnHelicopter * pObject = (CGameObjectRespawnHelicopter *) pElm->CreateGameInstance(); 

	I3ASSERT( i3::same_of<CGameObjectRespawnHelicopter*>(pObject));

	{
		m_ED_TeamIdx.GetWindowText( conv, sizeof(conv) - 1);

		pObject->setTeamIdx( atoi( conv));
	}

	I3_MUST_RELEASE(pObject);
}

#endif
