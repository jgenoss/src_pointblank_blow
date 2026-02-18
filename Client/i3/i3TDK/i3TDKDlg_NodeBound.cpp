
#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlg_NodeBound.h"
#include "i3Base/string/ext/atof.h"


IMPLEMENT_DYNAMIC(i3TDKDlg_NodeBound, CDialog)

void i3TDKDlg_NodeBound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_BR_MIN_X, m_ED_MinX);
	DDX_Control(pDX, IDC_EDIT_BR_MIN_Y, m_ED_MinY);
	DDX_Control(pDX, IDC_EDIT_BR_MIN_Z, m_ED_MinZ);
	DDX_Control(pDX, IDC_EDIT_BR_MAX_X, m_ED_MaxX);
	DDX_Control(pDX, IDC_EDIT_BR_MAX_Y, m_ED_MaxY);
	DDX_Control(pDX, IDC_EDIT_BR_MAX_Z, m_ED_MaxZ);

	if(m_pNode && !BoxInitFlag)
	{
		BoxInitFlag = true;
		TCHAR conv[256] = {0,};
		Min = *m_pNode->GetBound()->GetMin();
		Max = *m_pNode->GetBound()->GetMax();
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), Min.x);
		m_ED_MinX.SetWindowText(conv);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), Min.y);
		m_ED_MinY.SetWindowText(conv);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), Min.z);
		m_ED_MinZ.SetWindowText(conv);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), Max.x);
		m_ED_MaxX.SetWindowText(conv);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), Max.y);
		m_ED_MaxY.SetWindowText(conv);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), Max.z);
		m_ED_MaxZ.SetWindowText(conv);
	}
}

bool i3TDKDlg_NodeBound::Execute( i3Node * pNode)
{
	m_pNode = pNode;
	if(m_pNode)
	{
		

		i3TDK::SetResInstance();
		INT32 rv = DoModal();
		i3TDK::RestoreResInstance();

		if( rv == IDOK)
		{
			(*m_pNode->GetBound()->GetMin()) = Min;
			(*m_pNode->GetBound()->GetMax()) = Max;
			return true;
		}
	}
	return false;
}

void i3TDKDlg_NodeBound :: OnEnChangeBoundVal()
{	
	if(m_pNode)
	{
		CString str = _T("");
		TCHAR conv[256] = {0,};
		m_ED_MinX.GetWindowText(str);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), i3::atof(str.GetString()));
		Min.x = (REAL32)i3::atof(conv);

		m_ED_MinY.GetWindowText(str);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), i3::atof(str.GetString()));
		Min.y = (REAL32)i3::atof(conv);

		m_ED_MinZ.GetWindowText(str);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), i3::atof(str.GetString()));
		Min.z = (REAL32)i3::atof(conv);

		m_ED_MaxX.GetWindowText(str);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), i3::atof(str.GetString()));
		Max.x = (REAL32)i3::atof(conv);

		m_ED_MaxY.GetWindowText(str);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), i3::atof(str.GetString()));
		Max.y = (REAL32)i3::atof(conv);

		m_ED_MaxZ.GetWindowText(str);
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%.2f"), i3::atof(str.GetString()));
		Max.z = (REAL32)i3::atof(conv);


	}
}
BEGIN_MESSAGE_MAP(i3TDKDlg_NodeBound, CDialog)
	ON_EN_CHANGE(IDC_EDIT_BR_MIN_X, &i3TDKDlg_NodeBound::OnEnChangeBoundVal)
	ON_EN_CHANGE(IDC_EDIT_BR_MIN_Y, &i3TDKDlg_NodeBound::OnEnChangeBoundVal)
	ON_EN_CHANGE(IDC_EDIT_BR_MIN_Z, &i3TDKDlg_NodeBound::OnEnChangeBoundVal)
	ON_EN_CHANGE(IDC_EDIT_BR_MAX_X, &i3TDKDlg_NodeBound::OnEnChangeBoundVal)
	ON_EN_CHANGE(IDC_EDIT_BR_MAX_Z, &i3TDKDlg_NodeBound::OnEnChangeBoundVal)
	ON_EN_CHANGE(IDC_EDIT_BR_MAX_Y, &i3TDKDlg_NodeBound::OnEnChangeBoundVal)
END_MESSAGE_MAP()


// CChild1 메시지 처리기입니다.
