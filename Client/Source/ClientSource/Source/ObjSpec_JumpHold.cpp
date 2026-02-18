#include "pch.h"

#if defined(USE_EDITDLG)// CObjSpec_JumpHold

#include "ObjSpec_JumpHold.h"
#include "GameObject.h"
#include "i3Level/i3LevelElement3D.h"
#include "i3Base/string/ext/ftoa.h"

IMPLEMENT_DYNAMIC(CObjSpec_JumpHold, i3GameObjPropertyDialog)

CObjSpec_JumpHold::CObjSpec_JumpHold(CWnd* pParent /*=NULL*/)
	: i3GameObjPropertyDialog(CObjSpec_JumpHold::IDD, pParent)
{
}

CObjSpec_JumpHold::~CObjSpec_JumpHold()
{
}

void CObjSpec_JumpHold::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_JUMP_POWER,	m_ED_JumpPower);
	DDX_Control(pDX, IDC_JUMP_HEIGHT,	m_ED_JumpHeight);
	DDX_Control(pDX, IDC_JUMP_DIG,		m_ED_JumpDig);
	DDX_Control(pDX, IDC_JUMP_ANIM_SPEED, m_ED_JumpAnimSpeed);
}


BEGIN_MESSAGE_MAP(CObjSpec_JumpHold, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_JUMP_POWER,			OnEnKillfocusPbEdJumpPower)
	ON_EN_KILLFOCUS(IDC_JUMP_HEIGHT,		OnEnKillfocusPbEdJumpHeight)
	ON_EN_KILLFOCUS(IDC_JUMP_DIG,			OnEnKillfocusPbEdJumpDig)
	ON_EN_KILLFOCUS(IDC_JUMP_ANIM_SPEED,	OnEnKillfocusPbEdJumpAnimSpeed)
END_MESSAGE_MAP()


// CObjSpec_JumpHold message handlers
void CObjSpec_JumpHold::SetObject(i3ElementBase * pObj)
{
	char conv[128];

	i3GameObjPropertyDialog::SetObject(pObj);

	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectJumpHold * pJumpHold = (CGameObjectJumpHold *)pElm->CreateGameInstance();

	I3ASSERT(i3::same_of<CGameObjectJumpHold*>(pJumpHold));

	i3::ftoa(pJumpHold->getJumpPower(), conv);
	m_ED_JumpPower.SetWindowText(conv);

	i3::ftoa(pJumpHold->getJumpHeight(), conv);
	m_ED_JumpHeight.SetWindowText(conv);

	i3::ftoa(pJumpHold->getJumpDig(), conv);
	m_ED_JumpDig.SetWindowText(conv);

	i3::ftoa(pJumpHold->getJumpAnimSpeed(), conv);
	m_ED_JumpAnimSpeed.SetWindowText(conv);

	I3_MUST_RELEASE(pJumpHold);
}

BOOL CObjSpec_JumpHold::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CObjSpec_JumpHold::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}

int CObjSpec_JumpHold::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_JumpHold::OnEnKillfocusPbEdJumpPower()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectJumpHold * pJumpHold = (CGameObjectJumpHold *)pElm->CreateGameInstance();

	//	I3ASSERT( pRPG7Box != NULL);
	I3ASSERT(i3::same_of<CGameObjectJumpHold*>(pJumpHold));

	if (m_ED_JumpPower.GetModify())
	{
		m_ED_JumpPower.GetWindowText(conv, sizeof(conv) - 1);

		pJumpHold->setJumpPower((REAL32)atof(conv));

		m_ED_JumpPower.SetModify(FALSE);
	}

	I3_MUST_RELEASE(pJumpHold);
}

void CObjSpec_JumpHold::OnEnKillfocusPbEdJumpHeight()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectJumpHold * pJumpHold = (CGameObjectJumpHold *)pElm->CreateGameInstance();

	//	I3ASSERT( pRPG7Box != NULL);
	I3ASSERT(i3::same_of<CGameObjectJumpHold*>(pJumpHold));

	if (m_ED_JumpHeight.GetModify())
	{
		m_ED_JumpHeight.GetWindowText(conv, sizeof(conv) - 1);

		pJumpHold->setJumpHeight((REAL32)atof(conv));

		m_ED_JumpHeight.SetModify(FALSE);
	}

	I3_MUST_RELEASE(pJumpHold);
}

void CObjSpec_JumpHold::OnEnKillfocusPbEdJumpDig()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectJumpHold * pJumpHold = (CGameObjectJumpHold *)pElm->CreateGameInstance();

	I3ASSERT(i3::same_of<CGameObjectJumpHold*>(pJumpHold));

	if (m_ED_JumpDig.GetModify())
	{
		m_ED_JumpDig.GetWindowText(conv, sizeof(conv) - 1);

		pJumpHold->setJumpDig((REAL32)atof(conv));

		m_ED_JumpDig.SetModify(FALSE);
	}

	I3_MUST_RELEASE(pJumpHold);
}

void CObjSpec_JumpHold::OnEnKillfocusPbEdJumpAnimSpeed()
{
	char conv[128];
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectJumpHold * pJumpHold = (CGameObjectJumpHold *)pElm->CreateGameInstance();

	I3ASSERT(i3::same_of<CGameObjectJumpHold*>(pJumpHold));

	if (m_ED_JumpAnimSpeed.GetModify())
	{
		m_ED_JumpAnimSpeed.GetWindowText(conv, sizeof(conv) - 1);

		pJumpHold->setJumpAnimSpeed((REAL32)atof(conv));

		m_ED_JumpAnimSpeed.SetModify(FALSE);
	}

	I3_MUST_RELEASE(pJumpHold);
}


void CObjSpec_JumpHold::OnOK()
{
	OnEnKillfocusPbEdJumpPower();
	OnEnKillfocusPbEdJumpHeight();
	OnEnKillfocusPbEdJumpDig();
	OnEnKillfocusPbEdJumpAnimSpeed();
}
void CObjSpec_JumpHold::OnCancel()
{
}

#endif