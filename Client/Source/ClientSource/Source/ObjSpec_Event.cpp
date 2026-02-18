// Prop_LvRes_SceneGraph.cpp : implementation file
//
#include "pch.h"
#if defined(USE_EDITDLG)

#include "StageObject/ObjectEvent.h"
#include "BattleServerContext.h"

#include "GameObject.h"
#include "ObjSpec_Event.h"
#include "i3Level/i3LevelElement3D.h"


IMPLEMENT_DYNAMIC(CObjSpec_Event, i3GameObjPropertyDialog)

CObjSpec_Event::CObjSpec_Event(CWnd* pParent /*=nullptr*/)
: i3GameObjPropertyDialog( CObjSpec_Event::IDD, pParent)
{
}

CObjSpec_Event::~CObjSpec_Event()
{
}

void CObjSpec_Event::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_EDIT_TYPE_INDEX, m_ED_TeamIdx);
}


BEGIN_MESSAGE_MAP(CObjSpec_Event, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_EDIT_TYPE_INDEX, OnEnKillfocusPbEdTeamIdx)
END_MESSAGE_MAP()


// CObjSpec_Event message handlers
void CObjSpec_Event::SetObject( i3ElementBase * pObj)
{
	char conv[128];

	i3GameObjPropertyDialog::SetObject( pObj);

	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectEvent * pObject = (CGameObjectEvent *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<CGameObjectEvent*>(pObject));

	itoa( (INT32)pObject->getEventType(), conv, 128);
	m_ED_TeamIdx.SetWindowText( conv);

	I3_MUST_RELEASE(pObject);
}

BOOL CObjSpec_Event::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	i3TDK::RegisterUpdate( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_EDIT);

	return true;  // return true unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void CObjSpec_Event::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}

int CObjSpec_Event::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_Event::OnOK()
{
}

void CObjSpec_Event::OnCancel()
{
}

void CObjSpec_Event::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( getObject() == nullptr)
		return;
}

LRESULT CObjSpec_Event::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3GameObjPropertyDialog::WindowProc(message, wParam, lParam);
}

void CObjSpec_Event::OnEnKillfocusPbEdTeamIdx()
{
	char conv[128];

	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectEvent * pObject = (CGameObjectEvent *) pElm->CreateGameInstance(); 

	I3ASSERT( i3::same_of<CGameObjectEvent*>(pObject));

	{
		m_ED_TeamIdx.GetWindowText( conv, sizeof(conv) - 1);

		pObject->setEventType( atoi( conv));
	}

	I3_MUST_RELEASE(pObject);
}

#endif
