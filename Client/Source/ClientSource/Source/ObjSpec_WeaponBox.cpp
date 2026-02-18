// Prop_LvRes_SceneGraph.cpp : implementation file
//

#include "pch.h"

#if defined(USE_EDITDLG)// CObjSpec_WeaponBox

#include "ObjSpec_WeaponBox.h"
#include "GameObject.h"
#include "i3Level/i3LevelElement3D.h"
#include "i3Base/string/ext/ftoa.h"

IMPLEMENT_DYNAMIC(CObjSpec_WeaponBox, i3GameObjPropertyDialog)

CObjSpec_WeaponBox::CObjSpec_WeaponBox(CWnd* pParent /*=nullptr*/)
	: i3GameObjPropertyDialog( CObjSpec_WeaponBox::IDD, pParent)
{
}

CObjSpec_WeaponBox::~CObjSpec_WeaponBox()
{
}

void CObjSpec_WeaponBox::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_ED_RPG7_RESPAWN_TIME,	m_ED_RespawnTime);
	DDX_Control( pDX, IDC_ED_RPG7_GROUP_INDEX, m_ED_GroupIdx);
	DDX_Control( pDX, IDC_WEAPON_BOX_COMBO, m_CB_WeaponType);
	
}


BEGIN_MESSAGE_MAP(CObjSpec_WeaponBox, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_ED_RPG7_RESPAWN_TIME, OnEnKillfocusPbEdRespawnTime)
	ON_EN_KILLFOCUS(IDC_ED_RPG7_GROUP_INDEX,	OnEnKillfocusPbEdRPG7GroupIdx)
	ON_CBN_SELCHANGE(IDC_WEAPON_BOX_COMBO, OnCbnSelchangeWeaponType)
END_MESSAGE_MAP()




// CObjSpec_WeaponBox message handlers
void CObjSpec_WeaponBox::SetObject( i3ElementBase * pObj)
{
	char conv[128];

	i3GameObjPropertyDialog::SetObject( pObj);

	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectWeaponBox * pWeaponBox = (CGameObjectWeaponBox *) pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<CGameObjectWeaponBox*>(pWeaponBox));

	i3::ftoa( pWeaponBox->getRPG7RespawnTime(), conv);
	m_ED_RespawnTime.SetWindowText( conv);

	itoa( pWeaponBox->getRPG7GroupIdx(), conv, 128);
	m_ED_GroupIdx.SetWindowText( conv);

	switch( pWeaponBox->GetWeaponType())
	{
		case WT_RPG7_OUTPOST :		m_CB_WeaponType.SetCurSel( 0);	break;
		case WT_RPG7_BOSS :			m_CB_WeaponType.SetCurSel( 1);	break;
		case WT_K201 :				m_CB_WeaponType.SetCurSel( 2);	break;
		case WT_CHINESE_CLEAVER	:	m_CB_WeaponType.SetCurSel( 3);	break;
		case WT_CROWNWATERMELON	:	m_CB_WeaponType.SetCurSel( 4);	break;
		
		default :									I3ASSERT_0;					break;
	}
	
	I3_MUST_RELEASE(pWeaponBox);
}

void CObjSpec_WeaponBox::OnCbnSelchangeWeaponType()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectWeaponBox * pWeaponBox = (CGameObjectWeaponBox *) pElm->CreateGameInstance(); 

	I3ASSERT( i3::same_of<CGameObjectWeaponBox*>(pWeaponBox));

	switch( m_CB_WeaponType.GetCurSel())
	{
		case 0 :		
			pWeaponBox->SetWeaponType( WT_RPG7_OUTPOST);
			break;
		case 1 :		
			pWeaponBox->SetWeaponType( WT_RPG7_BOSS);
			break;
		case 2 :		
			pWeaponBox->SetWeaponType( WT_K201);
			break;
		case 3 :		
			pWeaponBox->SetWeaponType( WT_CHINESE_CLEAVER);
			break;
		case 4 :		
			pWeaponBox->SetWeaponType( WT_CROWNWATERMELON);
			break;
	}

	I3_MUST_RELEASE(pWeaponBox);
}

BOOL CObjSpec_WeaponBox::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	return true;  // return true unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void CObjSpec_WeaponBox::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}

int CObjSpec_WeaponBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_WeaponBox::OnEnKillfocusPbEdRespawnTime()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectWeaponBox * pRPG7Box = (CGameObjectWeaponBox *) pElm->CreateGameInstance(); 

	I3ASSERT( i3::same_of<CGameObjectWeaponBox*>(pRPG7Box));

	if( m_ED_RespawnTime.GetModify())
	{
		m_ED_RespawnTime.GetWindowText( conv, sizeof(conv) - 1);

		pRPG7Box->setRPG7RespawnTime( (REAL32) atof( conv));

		m_ED_RespawnTime.SetModify( false);
	}

	I3_MUST_RELEASE(pRPG7Box);
}

void CObjSpec_WeaponBox::OnEnKillfocusPbEdRPG7GroupIdx()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	if( pElm == nullptr)
		return;

	CGameObjectWeaponBox * pRPG7Box = (CGameObjectWeaponBox *) pElm->CreateGameInstance(); 

	I3ASSERT( i3::same_of<CGameObjectWeaponBox*>(pRPG7Box));

	if( m_ED_GroupIdx.GetModify())
	{
		m_ED_GroupIdx.GetWindowText( conv, sizeof(conv) - 1);

		pRPG7Box->setRPG7GroupIdx( (INT32) atoi( conv));

		m_ED_GroupIdx.SetModify( false);
	}

	I3_MUST_RELEASE(pRPG7Box);
}


void CObjSpec_WeaponBox::OnOK()
{
	OnEnKillfocusPbEdRespawnTime();
	OnEnKillfocusPbEdRPG7GroupIdx();
}
void CObjSpec_WeaponBox::OnCancel()
{
}

#endif