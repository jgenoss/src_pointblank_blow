// Prop_LvRes_SceneGraph.cpp : implementation file
//

#include "pch.h"

#if defined(USE_EDITDLG)// CObjSpec_TargetObject dialog
#include "ObjSpec_TargetObject.h"
#include "GameObject.h"
#include "StageObject\\TargetObject.h"
#include ".\objspec_targetobject.h"
#include "i3Level/i3LevelElement3D.h"
#include "i3Base/string/ext/ftoa.h"

IMPLEMENT_DYNAMIC(CObjSpec_TargetObject, i3GameObjPropertyDialog)

CObjSpec_TargetObject::CObjSpec_TargetObject(CWnd* pParent /*=nullptr*/)
	: i3GameObjPropertyDialog( CObjSpec_TargetObject::IDD, pParent)
{
}

CObjSpec_TargetObject::~CObjSpec_TargetObject()
{
}

void CObjSpec_TargetObject::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_TYPE,				m_CB_Type);
	DDX_Control( pDX, IDC_APPEAR,			m_CB_Appear);
	DDX_Control( pDX, IDC_ED_OFFSET,		m_ED_OffsetTime);
	DDX_Control( pDX, IDC_ED_STARTTIME,		m_ED_StartTime);
	DDX_Control( pDX, IDC_ED_KEEPTIME,		m_ED_KeepTime);
	DDX_Control( pDX, IDC_ED_DISAPPEARTIME,	m_ED_EndTime);
}


BEGIN_MESSAGE_MAP(CObjSpec_TargetObject, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_TYPE, OnCbnSelchangeType)
	ON_CBN_SELCHANGE(IDC_APPEAR, OnCbnSelchangeAppear)
	ON_EN_KILLFOCUS(IDC_ED_OFFSET, OnEnKillfocusEdOffset)
	ON_EN_KILLFOCUS(IDC_ED_STARTTIME, OnEnKillfocusEdStarttime)
	ON_EN_KILLFOCUS(IDC_ED_KEEPTIME, OnEnKillfocusEdKeeptime)
	ON_EN_KILLFOCUS(IDC_ED_DISAPPEARTIME, OnEnKillfocusEdDisappeartime)
END_MESSAGE_MAP()


// CObjSpec_TargetObject message handlers
void CObjSpec_TargetObject::SetObject( i3ElementBase * pObj)
{
	i3GameObjPropertyDialog::SetObject( pObj);

	i3LevelElement3D *	pElm = (i3LevelElement3D *) pObj;

	TargetObject * pTarget = (TargetObject *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<TargetObject*>(pTarget));

	char conv[128];

	// Type
	switch( pTarget->getType())
	{
		case TargetObject::TYPE_ENEMY :		m_CB_Type.SetCurSel( 0);	break;
		case TargetObject::TYPE_FRIEND :	m_CB_Type.SetCurSel( 1);	break;
		default :							m_CB_Type.SetCurSel( 2);	break;
	}

	// Appear Type
	switch( pTarget->getAppearType())
	{
		case TargetObject::APT_POPUP :		m_CB_Appear.SetCurSel( 0);	break;
		case TargetObject::APT_SLIDE_LEFT :	m_CB_Appear.SetCurSel( 1);	break;
		default :							m_CB_Appear.SetCurSel( 2);	break;
	}

	// Offset Time
	i3::ftoa( pTarget->getOffsetTime(), conv);
	m_ED_OffsetTime.SetWindowText( conv);

	// Start Time
	i3::ftoa( pTarget->getStartTime(), conv);
	m_ED_StartTime.SetWindowText( conv);

	// Keep Time
	i3::ftoa( pTarget->getKeepTime(), conv);
	m_ED_KeepTime.SetWindowText( conv);

	// End Time
	i3::ftoa( pTarget->getEndTime(), conv);
	m_ED_EndTime.SetWindowText( conv);

	I3_MUST_RELEASE(pTarget);
}

BOOL CObjSpec_TargetObject::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	return true;  // return true unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void CObjSpec_TargetObject::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}


void CObjSpec_TargetObject::OnOK()
{
}

void CObjSpec_TargetObject::OnCancel()
{
}

void CObjSpec_TargetObject::OnCbnSelchangeType()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();

	TargetObject * pTarget = (TargetObject *) pElm->CreateGameInstance();
	
	I3ASSERT( i3::same_of<TargetObject*>(pTarget));

	switch( m_CB_Type.GetCurSel())
	{
		case 0 :		// Àû
			pTarget->setType( TargetObject::TYPE_ENEMY);
			break;

		case 1 :		// ¾Æ±º
			pTarget->setType( TargetObject::TYPE_FRIEND);
			break;

		default :		// RANDOM
			pTarget->setType( TargetObject::TYPE_RANDOM);
			break;
	}

	I3_MUST_RELEASE(pTarget);
}

void CObjSpec_TargetObject::OnCbnSelchangeAppear()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();

	TargetObject * pTarget = (TargetObject *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<TargetObject*>(pTarget));

	switch( m_CB_Appear.GetCurSel())
	{
		case 0 :		// Popup
			pTarget->setAppearType( TargetObject::APT_POPUP);
			break;

		case 1 :		// Slide_Left
			pTarget->setAppearType( TargetObject::APT_SLIDE_LEFT);
			break;

		default :		// Slide_Right
			pTarget->setAppearType( TargetObject::API_SLIDE_RIGHT);
			break;
	}

	I3_MUST_RELEASE(pTarget);
}

void CObjSpec_TargetObject::OnEnKillfocusEdOffset()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();

	TargetObject * pTarget = (TargetObject *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<TargetObject*>(pTarget));

	char conv[256];

	m_ED_OffsetTime.GetWindowText( conv, sizeof(conv) - 1);

	pTarget->setOffsetTime( (REAL32) atof( conv));

	I3_MUST_RELEASE(pTarget);
}

void CObjSpec_TargetObject::OnEnKillfocusEdStarttime()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();

	TargetObject * pTarget = (TargetObject *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<TargetObject*>(pTarget));

	char conv[256];

	m_ED_StartTime.GetWindowText( conv, sizeof(conv) - 1);
	
	pTarget->setStartTime( (REAL32) atof( conv));

	I3_MUST_RELEASE(pTarget);
}

void CObjSpec_TargetObject::OnEnKillfocusEdKeeptime()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();

	TargetObject * pTarget = (TargetObject *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<TargetObject*>(pTarget));

	char conv[256];
	
	m_ED_KeepTime.GetWindowText( conv, sizeof(conv) - 1);

	pTarget->setKeepTime( (REAL32) atof( conv));

	I3_MUST_RELEASE(pTarget);
}

void CObjSpec_TargetObject::OnEnKillfocusEdDisappeartime()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();

	TargetObject * pTarget = (TargetObject *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<TargetObject*>(pTarget));

	char conv[ 256];

	m_ED_EndTime.GetWindowText( conv, sizeof(conv) - 1);

	pTarget->setEndTime( (REAL32) atof( conv));
	I3_MUST_RELEASE(pTarget);
}

#endif