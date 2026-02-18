#include "stdafx.h"
#include "dlgColorOptionApply.h"
#include "i3Base/string/ext/atof.h"


BEGIN_MESSAGE_MAP(dlgColorOptionApply, CDialog)
	ON_BN_CLICKED(IDC_CHK_COLOR, OnBnClickedColorCheck)
	ON_BN_CLICKED(IDC_CHK_DIFFUSE, OnBnClickedDiffCheck)
	ON_BN_CLICKED(IDC_CHK_SPECULAR, OnBnClickedSpecCheck)
	ON_BN_CLICKED(IDC_CHK_EMISSION, OnBnClickedEmisCheck)
	ON_BN_CLICKED(IDC_CHK_SHINE, OnBnClickedShineCheck)
END_MESSAGE_MAP()

dlgColorOptionApply::dlgColorOptionApply() : CDialog(dlgColorOptionApply::IDD)
{

}

dlgColorOptionApply::~dlgColorOptionApply()
{

}

void dlgColorOptionApply::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_COLOR, m_ST_ColPH);
	DDX_Control(pDX, IDC_STATIC_DIFFUSE, m_ST_DiffPH);
	DDX_Control(pDX, IDC_STATIC_SPECULAR, m_ST_SpecPH);
	DDX_Control(pDX, IDC_STATIC_EMISSION, m_ST_EmisPH);
	DDX_Control(pDX, IDC_EDIT_SHININESS, m_ShininessCtrl);

	DDX_Control(pDX, IDC_CHK_COLOR, m_ColorCheck);
	DDX_Control(pDX, IDC_CHK_DIFFUSE, m_DiffCheck);
	DDX_Control(pDX, IDC_CHK_SPECULAR, m_SpecCheck);
	DDX_Control(pDX, IDC_CHK_EMISSION, m_EmisCheck);
	DDX_Control(pDX, IDC_CHK_SHINE, m_ShineCheck);
}

BOOL dlgColorOptionApply::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rt;

	m_ST_ColPH.GetWindowRect(&rt);
	ScreenToClient(&rt);
	m_ColorCtrl.Create(WS_CHILD | WS_VISIBLE, rt, this, 100);
	m_ColorCtrl.SetWindowPos(NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	m_ColorCtrl.EnableWindow(TRUE);

	m_ST_DiffPH.GetWindowRect(&rt);
	ScreenToClient(&rt);
	m_DiffCtrl.Create(WS_CHILD | WS_VISIBLE, rt, this, 101);
	m_DiffCtrl.SetWindowPos(NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	m_ST_SpecPH.GetWindowRect(&rt);
	ScreenToClient(&rt);
	m_SpecCtrl.Create(WS_CHILD | WS_VISIBLE, rt, this, 102);
	m_SpecCtrl.SetWindowPos(NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	m_ST_EmisPH.GetWindowRect(&rt);
	ScreenToClient(&rt);
	m_EmisCtrl.Create(WS_CHILD | WS_VISIBLE, rt, this, 103);
	m_EmisCtrl.SetWindowPos(NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	m_ColorCheck.SetCheck(FALSE);
	m_DiffCheck.SetCheck(FALSE);
	m_SpecCheck.SetCheck(FALSE);
	m_EmisCheck.SetCheck(FALSE);
	m_ShineCheck.SetCheck(FALSE);

	COLOR color;

	m_ColorCtrl.EnableWindow(FALSE);
	m_DiffCtrl.EnableWindow(FALSE);

	i3Color::Set(&color, 0.0f, 0.0f, 0.0f, 0.0f);
	m_SpecCtrl.setColor(&color);
	m_SpecCtrl.EnableWindow(FALSE);

	m_EmisCtrl.EnableWindow(FALSE);

	m_ShininessCtrl.SetWindowText("128.0");
	m_ShininessCtrl.EnableWindow(FALSE);

	return TRUE;
}

void dlgColorOptionApply::OnOK()
{
	Apply();

	CDialog::OnOK();
}

void dlgColorOptionApply::OnCancel()
{
	CDialog::OnCancel();
}

void dlgColorOptionApply::Apply()
{
	if(m_pRoot != NULL)
	{
		i3::vector<i3RenderAttr*> materialList;
		
		//컬러 옵션이 켜져 있으므로, 컬러 어트리뷰트를 찾는다.
		if(m_ColorCheck.GetCheck() == BST_CHECKED)
		{
			i3::vector<i3RenderAttr *> colorList;

			i3Scene::FindAttrByExactType(m_pRoot, i3ColorAttr::static_meta(), colorList, true);

			for(INT32 i = 0; i < (INT32)colorList.size(); i++)
			{
				i3ColorAttr * pColorAttr = (i3ColorAttr *)colorList[i];
				pColorAttr->Set(m_ColorCtrl.getColor());
			}
		}

		i3Scene::FindAttrByExactType(m_pRoot, i3MaterialAttr::static_meta(), materialList, true);

		for(INT32 i = 0; i < (INT32)materialList.size(); i++)
		{
			i3MaterialAttr * pAttr = (i3MaterialAttr *)materialList[i];

			if(m_DiffCheck.GetCheck() == BST_CHECKED)
			{
				pAttr->SetDiffuse(m_DiffCtrl.getColor());
			}

			if(m_SpecCheck.GetCheck() == BST_CHECKED)
			{
				pAttr->SetSpecular(m_SpecCtrl.getColor());
			}

			if(m_EmisCheck.GetCheck() == BST_CHECKED)
			{
				pAttr->SetEmissive(m_EmisCtrl.getColor());
			}

			if(m_ShineCheck.GetCheck() == BST_CHECKED)
			{
				char conv[260];
				m_ShininessCtrl.GetWindowText( conv, _countof(conv) - 1);

				REAL32 val = (REAL32)i3::atof(conv);
				pAttr->SetShininess(val);
			}

		}
	}
}

void dlgColorOptionApply::Execute(i3Node * pRoot)
{
	I3ASSERT(pRoot != NULL);

	m_pRoot = pRoot;
	
	DoModal();
}

void dlgColorOptionApply::OnBnClickedColorCheck()
{
	BOOL bValid;

	if(m_ColorCheck.GetCheck() == BST_CHECKED)
		bValid = TRUE;
	else
		bValid = FALSE;

	m_ColorCtrl.EnableWindow(bValid);
}

void dlgColorOptionApply::OnBnClickedDiffCheck()
{
	BOOL bValid;

	if(m_DiffCheck.GetCheck() == BST_CHECKED)
		bValid = TRUE;
	else
		bValid = FALSE;

	m_DiffCtrl.EnableWindow(bValid);
}

void dlgColorOptionApply::OnBnClickedSpecCheck()
{
	BOOL bValid;

	if(m_SpecCheck.GetCheck() == BST_CHECKED)
		bValid = TRUE;
	else
		bValid = FALSE;

	m_SpecCtrl.EnableWindow(bValid);
}

void dlgColorOptionApply::OnBnClickedEmisCheck()
{
	BOOL bValid;

	if(m_EmisCheck.GetCheck() == BST_CHECKED)
		bValid = TRUE;
	else
		bValid = FALSE;

	m_EmisCtrl.EnableWindow(bValid);
}


void dlgColorOptionApply::OnBnClickedShineCheck()
{
	BOOL bValid;

	if(m_ShineCheck.GetCheck() == BST_CHECKED)
		bValid = TRUE;
	else
		bValid = FALSE;

	m_ShininessCtrl.EnableWindow(bValid);
}
