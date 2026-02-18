// MatrixDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Matrix.h"
#include "MatrixDlg.h"
#include ".\matrixdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMatrixDlg 대화 상자



CMatrixDlg::CMatrixDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMatrixDlg::IDD, pParent)
	, m_S1_00(_T("1"))
	, m_S1_01(_T("0"))
	, m_S1_02(_T("0"))
	, m_S1_03(_T("0"))
	, m_S1_10(_T("0"))
	, m_S1_11(_T("1"))
	, m_S1_12(_T("0"))
	, m_S1_13(_T("0"))
	, m_S1_20(_T("0"))
	, m_S1_21(_T("0"))
	, m_S1_22(_T("1"))
	, m_S1_23(_T("0"))
	, m_S1_30(_T("0"))
	, m_S1_31(_T("0"))
	, m_S1_32(_T("0"))
	, m_S1_33(_T("1"))

	, m_S2_00(_T("1"))
	, m_S2_01(_T("0"))
	, m_S2_02(_T("0"))
	, m_S2_03(_T("0"))
	, m_S2_10(_T("0"))
	, m_S2_11(_T("1"))
	, m_S2_12(_T("0"))
	, m_S2_13(_T("0"))
	, m_S2_20(_T("0"))
	, m_S2_21(_T("0"))
	, m_S2_22(_T("1"))
	, m_S2_23(_T("0"))
	, m_S2_30(_T("0"))
	, m_S2_31(_T("0"))
	, m_S2_32(_T("0"))
	, m_S2_33(_T("1"))

	, m_D_00(_T("1"))
	, m_D_01(_T("0"))
	, m_D_02(_T("0"))
	, m_D_03(_T("0"))
	, m_D_10(_T("0"))
	, m_D_11(_T("1"))
	, m_D_12(_T("0"))
	, m_D_13(_T("0"))
	, m_D_20(_T("0"))
	, m_D_21(_T("0"))
	, m_D_22(_T("1"))
	, m_D_23(_T("0"))
	, m_D_30(_T("0"))
	, m_D_31(_T("0"))
	, m_D_32(_T("0"))
	, m_D_33(_T("1"))
	, m_Prefix1(_T(""))
	, m_Prefix2(_T(""))
	, m_X1(_T(""))
	, m_Y1(_T(""))
	, m_Z1(_T(""))
	, m_X2(_T(""))
	, m_Y2(_T(""))
	, m_Z2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMatrixDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SRC00, m_S1_00);
	DDX_Text(pDX, IDC_SRC01, m_S1_01);
	DDX_Text(pDX, IDC_SRC02, m_S1_02);
	DDX_Text(pDX, IDC_SRC03, m_S1_03);
	DDX_Text(pDX, IDC_SRC10, m_S1_10);
	DDX_Text(pDX, IDC_SRC11, m_S1_11);
	DDX_Text(pDX, IDC_SRC12, m_S1_12);
	DDX_Text(pDX, IDC_SRC13, m_S1_13);
	DDX_Text(pDX, IDC_SRC20, m_S1_20);
	DDX_Text(pDX, IDC_SRC21, m_S1_21);
	DDX_Text(pDX, IDC_SRC22, m_S1_22);
	DDX_Text(pDX, IDC_SRC23, m_S1_23);
	DDX_Text(pDX, IDC_SRC30, m_S1_30);
	DDX_Text(pDX, IDC_SRC31, m_S1_31);
	DDX_Text(pDX, IDC_SRC32, m_S1_32);
	DDX_Text(pDX, IDC_SRC33, m_S1_33);

	DDX_Text(pDX, IDC_SRC2_00, m_S2_00);
	DDX_Text(pDX, IDC_SRC2_01, m_S2_01);
	DDX_Text(pDX, IDC_SRC2_02, m_S2_02);
	DDX_Text(pDX, IDC_SRC2_03, m_S2_03);
	DDX_Text(pDX, IDC_SRC2_10, m_S2_10);
	DDX_Text(pDX, IDC_SRC2_11, m_S2_11);
	DDX_Text(pDX, IDC_SRC2_12, m_S2_12);
	DDX_Text(pDX, IDC_SRC2_13, m_S2_13);
	DDX_Text(pDX, IDC_SRC2_20, m_S2_20);
	DDX_Text(pDX, IDC_SRC2_21, m_S2_21);
	DDX_Text(pDX, IDC_SRC2_22, m_S2_22);
	DDX_Text(pDX, IDC_SRC2_23, m_S2_23);
	DDX_Text(pDX, IDC_SRC2_30, m_S2_30);
	DDX_Text(pDX, IDC_SRC2_31, m_S2_31);
	DDX_Text(pDX, IDC_SRC2_32, m_S2_32);
	DDX_Text(pDX, IDC_SRC2_33, m_S2_33);

	DDX_Text(pDX, IDC_DEST00, m_D_00);
	DDX_Text(pDX, IDC_DEST01, m_D_01);
	DDX_Text(pDX, IDC_DEST02, m_D_02);
	DDX_Text(pDX, IDC_DEST03, m_D_03);
	DDX_Text(pDX, IDC_DEST10, m_D_10);
	DDX_Text(pDX, IDC_DEST11, m_D_11);
	DDX_Text(pDX, IDC_DEST12, m_D_12);
	DDX_Text(pDX, IDC_DEST13, m_D_13);
	DDX_Text(pDX, IDC_DEST20, m_D_20);
	DDX_Text(pDX, IDC_DEST21, m_D_21);
	DDX_Text(pDX, IDC_DEST22, m_D_22);
	DDX_Text(pDX, IDC_DEST23, m_D_23);
	DDX_Text(pDX, IDC_DEST30, m_D_30);
	DDX_Text(pDX, IDC_DEST31, m_D_31);
	DDX_Text(pDX, IDC_DEST32, m_D_32);
	DDX_Text(pDX, IDC_DEST33, m_D_33);
	DDX_Text(pDX, IDC_ED_PREFIX1, m_Prefix1);
	DDX_Text(pDX, IDC_ED_PREFIX2, m_Prefix2);
	DDX_Text(pDX, IDC_ED_X1, m_X1);
	DDX_Text(pDX, IDC_ED_Y1, m_Y1);
	DDX_Text(pDX, IDC_ED_Z1, m_Z1);
	DDX_Text(pDX, IDC_ED_X2, m_X2);
	DDX_Text(pDX, IDC_ED_Y2, m_Y2);
	DDX_Text(pDX, IDC_ED_Z2, m_Z2);
}

BEGIN_MESSAGE_MAP(CMatrixDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CALC, OnBnClickedBtnCalc)
//	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
//ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
//ON_EN_CHANGE(IDC_ED_PREFIX1, OnEnChangeEdPrefix1)
//ON_EN_CHANGE(IDC_ED_PREFIX1, OnEnChangeEdPrefix1)
//ON_EN_CHANGE(IDC_ED_PREFIX4, OnEnChangeEdPrefix4)
ON_BN_CLICKED(IDC_BTN_IDENTITY, OnBnClickedBtnIdentity)
ON_BN_CLICKED(IDC_BTN_IDENTITY2, OnBnClickedBtnIdentity2)
ON_BN_CLICKED(IDC_BTN_PREFIX1, OnBnClickedBtnPrefix1)
ON_BN_CLICKED(IDC_BTN_PREFIX2, OnBnClickedBtnPrefix2)
END_MESSAGE_MAP()


// CMatrixDlg 메시지 처리기

BOOL CMatrixDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CMatrixDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CMatrixDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CMatrixDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CMatrixDlg::IsDigitString( CString * pStr)
{
	char * pTemp = pStr->GetBuffer();

	if( *pTemp == 0)
		return true;

	while( *pTemp)
	{
		if( ((*pTemp < '0') || (*pTemp > '9')) && (*pTemp != '.'))
			return false;

		pTemp++;
	}

	return true;
}

void CMatrixDlg::Mul( CString * pOut, CString * pIn1, CString * pIn2, bool bReset)
{
	CString temp;
	BOOL bDigit1, bDigit2;
	float val1, val2;

	bDigit1 = IsDigitString( pIn1);
	bDigit2 = IsDigitString( pIn2);

	if( bDigit1 )
		val1 = (float) atof( LPCTSTR( *pIn1));

	if( bDigit2 )
		val2 = (float) atof( LPCTSTR( *pIn2));

	if( bReset)
	{
		pOut->Empty();
	}

	if( bDigit1 && (val1 == 0.0f))
		return;

	if( bDigit2 && (val2 == 0.0f))
		return;

	if( bDigit1 && bDigit2)
	{
		// 양쪽 모두 숫자이기 때문에 실제 산술 연산을 한다.
		val1 *= val2;

		temp.Format( "%f", val1 * val2);
	}
	else
	{
		// 문자열 Concaternation으로 처리한다.

		if( bDigit1 && (val1 == 1.0f))
		{
			temp = *pIn2;
		}
		else if( bDigit2 && (val2 == 1.0f))
		{
			temp = *pIn1;
		}
		else
		{
			temp = *pIn1 + "*" + *pIn2;
		}
	}


	if( pOut->GetLength() > 0)
	{
		*pOut += " ";
	}

	*pOut += temp;
}

typedef struct _tagInfo
{
	CString * m_pStr;
	BOOL m_bDigit;
	BOOL m_bProcessed;
	float m_val;
} TAGINFO;

void CMatrixDlg::Add( CString * pOut, CString * pIn1, CString * pIn2, CString * pIn3, CString * pIn4, bool bReset)
{
	TAGINFO Info[4];
	CString temp;
	INT32 i;
	BOOL bDigit = FALSE;
	BOOL bString = FALSE;
	BOOL bFirst = TRUE;
	float val = 0.0f;

	memset( Info, 0, sizeof(Info));

	Info[0].m_pStr = pIn1;
	Info[1].m_pStr = pIn2;
	Info[2].m_pStr = pIn3;
	Info[3].m_pStr = pIn4;

	for( i = 0; i < 4; i++)
	{
		Info[i].m_bDigit = IsDigitString( Info[i].m_pStr);

		if( Info[i].m_bDigit )
		{
			if( Info[i].m_pStr->GetLength() > 0)
				bDigit = TRUE;

			Info[i].m_val = (float) atof( LPCTSTR( * (Info[i].m_pStr)));
			val += Info[i].m_val;
		}
	}

	if( bReset)
	{
		pOut->Empty();
	}
	else if( pOut->GetLength() > 0)
	{
		*pOut += " ";
	}

	for( i = 0; i < 4; i++)
	{
		if( Info[i].m_bDigit == FALSE)
		{
			if( bFirst == FALSE)
			{
				*pOut += " + ";
			}
			else
			{
				bFirst = FALSE;
			}

			*pOut += *Info[i].m_pStr + " ";
			bString = TRUE;
		}
	}

	if( bDigit || (bString == FALSE))
	{
		// 숫자 부분이 있다면...
		temp.Format( "%f", val);

		*pOut += temp;
	}
}

void CMatrixDlg::CalcMatrix(void)
{
	CString tmp1, tmp2, tmp3, tmp4;

	UpdateData( TRUE);

	//////////////////////////////////////////////////////////////////////////////
	// d00 = (s1_00 * s2_00) + (s1_01 * s2_10) + (s1_02 *s2_20) + (s1_03 * s2_30)
	Mul( &tmp1,		&m_S1_00, &m_S2_00, true);
	Mul( &tmp2,		&m_S1_01, &m_S2_10, true);
	Mul( &tmp3,		&m_S1_02, &m_S2_20, true);
	Mul( &tmp4,		&m_S1_03, &m_S2_30, true);
	Add( &m_D_00, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d01 = (s1_00 * s2_01) + (s1_01 * s2_11) + (s1_02 *s2_21) + (s1_03 * s2_31)
	Mul( &tmp1,		&m_S1_00, &m_S2_01, true);
	Mul( &tmp2,		&m_S1_01, &m_S2_11, true);
	Mul( &tmp3,		&m_S1_02, &m_S2_21, true);
	Mul( &tmp4,		&m_S1_03, &m_S2_31, true);
	Add( &m_D_01, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d02 = (s1_00 * s2_02) + (s1_01 * s2_12) + (s1_02 *s2_22) + (s1_03 * s2_32)
	Mul( &tmp1,		&m_S1_00, &m_S2_02, true);
	Mul( &tmp2,		&m_S1_01, &m_S2_12, true);
	Mul( &tmp3,		&m_S1_02, &m_S2_22, true);
	Mul( &tmp4,		&m_S1_03, &m_S2_32, true);
	Add( &m_D_02, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d03 = (s1_00 * s2_03) + (s1_01 * s2_13) + (s1_02 *s2_23) + (s1_03 * s2_33)
	Mul( &tmp1,		&m_S1_00, &m_S2_03, true);
	Mul( &tmp2,		&m_S1_01, &m_S2_13, true);
	Mul( &tmp3,		&m_S1_02, &m_S2_23, true);
	Mul( &tmp4,		&m_S1_03, &m_S2_33, true);
	Add( &m_D_03, &tmp1, &tmp2, &tmp3, &tmp4, true);


	//////////////////////////////////////////////////////////////////////////////
	// d10 = (s1_10 * s2_00) + (s1_11 * s2_10) + (s1_12 *s2_20) + (s1_13 * s2_30)
	Mul( &tmp1,		&m_S1_10, &m_S2_00, true);
	Mul( &tmp2,		&m_S1_11, &m_S2_10, true);
	Mul( &tmp3,		&m_S1_12, &m_S2_20, true);
	Mul( &tmp4,		&m_S1_13, &m_S2_30, true);
	Add( &m_D_10, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d11 = (s1_00 * s2_01) + (s1_01 * s2_11) + (s1_02 *s2_21) + (s1_03 * s2_31)
	Mul( &tmp1,		&m_S1_10, &m_S2_01, true);
	Mul( &tmp2,		&m_S1_11, &m_S2_11, true);
	Mul( &tmp3,		&m_S1_12, &m_S2_21, true);
	Mul( &tmp4,		&m_S1_13, &m_S2_31, true);
	Add( &m_D_11, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d12 = (s1_00 * s2_02) + (s1_01 * s2_12) + (s1_02 *s2_22) + (s1_03 * s2_32)
	Mul( &tmp1,		&m_S1_10, &m_S2_02, true);
	Mul( &tmp2,		&m_S1_11, &m_S2_12, true);
	Mul( &tmp3,		&m_S1_12, &m_S2_22, true);
	Mul( &tmp4,		&m_S1_13, &m_S2_32, true);
	Add( &m_D_12, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d13 = (s1_00 * s2_03) + (s1_01 * s2_13) + (s1_02 *s2_23) + (s1_03 * s2_33)
	Mul( &tmp1,		&m_S1_10, &m_S2_03, true);
	Mul( &tmp2,		&m_S1_11, &m_S2_13, true);
	Mul( &tmp3,		&m_S1_12, &m_S2_23, true);
	Mul( &tmp4,		&m_S1_13, &m_S2_33, true);
	Add( &m_D_13, &tmp1, &tmp2, &tmp3, &tmp4, true);


	//////////////////////////////////////////////////////////////////////////////
	// d20 = (s1_00 * s2_00) + (s1_01 * s2_10) + (s1_02 *s2_20) + (s1_03 * s2_30)
	Mul( &tmp1,		&m_S1_20, &m_S2_00, true);
	Mul( &tmp2,		&m_S1_21, &m_S2_10, true);
	Mul( &tmp3,		&m_S1_22, &m_S2_20, true);
	Mul( &tmp4,		&m_S1_23, &m_S2_30, true);
	Add( &m_D_20, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d21 = (s1_00 * s2_01) + (s1_01 * s2_11) + (s1_02 *s2_21) + (s1_03 * s2_31)
	Mul( &tmp1,		&m_S1_20, &m_S2_01, true);
	Mul( &tmp2,		&m_S1_21, &m_S2_11, true);
	Mul( &tmp3,		&m_S1_22, &m_S2_21, true);
	Mul( &tmp4,		&m_S1_23, &m_S2_31, true);
	Add( &m_D_21, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d22 = (s1_00 * s2_02) + (s1_01 * s2_12) + (s1_02 *s2_22) + (s1_03 * s2_32)
	Mul( &tmp1,		&m_S1_20, &m_S2_02, true);
	Mul( &tmp2,		&m_S1_21, &m_S2_12, true);
	Mul( &tmp3,		&m_S1_22, &m_S2_22, true);
	Mul( &tmp4,		&m_S1_23, &m_S2_32, true);
	Add( &m_D_22, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d23 = (s1_00 * s2_03) + (s1_01 * s2_13) + (s1_02 *s2_23) + (s1_03 * s2_33)
	Mul( &tmp1,		&m_S1_20, &m_S2_03, true);
	Mul( &tmp2,		&m_S1_21, &m_S2_13, true);
	Mul( &tmp3,		&m_S1_22, &m_S2_23, true);
	Mul( &tmp4,		&m_S1_23, &m_S2_33, true);
	Add( &m_D_23, &tmp1, &tmp2, &tmp3, &tmp4, true);


	//////////////////////////////////////////////////////////////////////////////
	// d30 = (s1_00 * s2_00) + (s1_01 * s2_10) + (s1_02 *s2_20) + (s1_03 * s2_30)
	Mul( &tmp1,		&m_S1_30, &m_S2_00, true);
	Mul( &tmp2,		&m_S1_31, &m_S2_10, true);
	Mul( &tmp3,		&m_S1_32, &m_S2_20, true);
	Mul( &tmp4,		&m_S1_33, &m_S2_30, true);
	Add( &m_D_30, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d31 = (s1_00 * s2_01) + (s1_01 * s2_11) + (s1_02 *s2_21) + (s1_03 * s2_31)
	Mul( &tmp1,		&m_S1_30, &m_S2_01, true);
	Mul( &tmp2,		&m_S1_31, &m_S2_11, true);
	Mul( &tmp3,		&m_S1_32, &m_S2_21, true);
	Mul( &tmp4,		&m_S1_33, &m_S2_31, true);
	Add( &m_D_31, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d32 = (s1_00 * s2_02) + (s1_01 * s2_12) + (s1_02 *s2_22) + (s1_03 * s2_32)
	Mul( &tmp1,		&m_S1_30, &m_S2_02, true);
	Mul( &tmp2,		&m_S1_31, &m_S2_12, true);
	Mul( &tmp3,		&m_S1_32, &m_S2_22, true);
	Mul( &tmp4,		&m_S1_33, &m_S2_32, true);
	Add( &m_D_32, &tmp1, &tmp2, &tmp3, &tmp4, true);

	// d33 = (s1_00 * s2_03) + (s1_01 * s2_13) + (s1_02 *s2_23) + (s1_03 * s2_33)
	Mul( &tmp1,		&m_S1_30, &m_S2_03, true);
	Mul( &tmp2,		&m_S1_31, &m_S2_13, true);
	Mul( &tmp3,		&m_S1_32, &m_S2_23, true);
	Mul( &tmp4,		&m_S1_33, &m_S2_33, true);
	Add( &m_D_33, &tmp1, &tmp2, &tmp3, &tmp4, true);

	UpdateData( FALSE);
}

void CMatrixDlg::OnBnClickedBtnCalc()
{
	CalcMatrix();
}

void CMatrixDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CMatrixDlg::OnOK()
{
	CalcMatrix();
}

void CMatrixDlg::OnBnClickedBtnIdentity()
{
	UpdateData( TRUE);

	m_S1_00 = "1";
	m_S1_01 = "0";
	m_S1_02 = "0";
	m_S1_03 = "0";

	m_S1_10 = "0";
	m_S1_11 = "1";
	m_S1_12 = "0";
	m_S1_13 = "0";

	m_S1_20 = "0";
	m_S1_21 = "0";
	m_S1_22 = "1";
	m_S1_23 = "0";

	m_S1_30 = "0";
	m_S1_31 = "0";
	m_S1_32 = "0";
	m_S1_33 = "1";

	UpdateData( FALSE);
}

void CMatrixDlg::OnBnClickedBtnIdentity2()
{
	UpdateData( TRUE);

	m_S2_00 = "1";
	m_S2_01 = "0";
	m_S2_02 = "0";
	m_S2_03 = "0";

	m_S2_10 = "0";
	m_S2_11 = "1";
	m_S2_12 = "0";
	m_S2_13 = "0";

	m_S2_20 = "0";
	m_S2_21 = "0";
	m_S2_22 = "1";
	m_S2_23 = "0";

	m_S2_30 = "0";
	m_S2_31 = "0";
	m_S2_32 = "0";
	m_S2_33 = "1";

	UpdateData( FALSE);
}


void CMatrixDlg::OnBnClickedBtnPrefix1()
{
	UpdateData( TRUE);

	m_S1_00.Format( "%s00", LPCTSTR( m_Prefix1));
	m_S1_01.Format( "%s01", LPCTSTR( m_Prefix1));
	m_S1_02.Format( "%s02", LPCTSTR( m_Prefix1));
	m_S1_03.Format( "%s03", LPCTSTR( m_Prefix1));

	m_S1_10.Format( "%s10", LPCTSTR( m_Prefix1));
	m_S1_11.Format( "%s11", LPCTSTR( m_Prefix1));
	m_S1_12.Format( "%s12", LPCTSTR( m_Prefix1));
	m_S1_13.Format( "%s13", LPCTSTR( m_Prefix1));

	m_S1_20.Format( "%s20", LPCTSTR( m_Prefix1));
	m_S1_21.Format( "%s21", LPCTSTR( m_Prefix1));
	m_S1_22.Format( "%s22", LPCTSTR( m_Prefix1));
	m_S1_23.Format( "%s23", LPCTSTR( m_Prefix1));

	m_S1_30.Format( "%s30", LPCTSTR( m_Prefix1));
	m_S1_31.Format( "%s31", LPCTSTR( m_Prefix1));
	m_S1_32.Format( "%s32", LPCTSTR( m_Prefix1));
	m_S1_33.Format( "%s33", LPCTSTR( m_Prefix1));

	UpdateData( FALSE);	
}

void CMatrixDlg::OnBnClickedBtnPrefix2()
{
	UpdateData( TRUE);

	m_S2_00.Format( "%s00", LPCTSTR( m_Prefix2));
	m_S2_01.Format( "%s01", LPCTSTR( m_Prefix2));
	m_S2_02.Format( "%s02", LPCTSTR( m_Prefix2));
	m_S2_03.Format( "%s03", LPCTSTR( m_Prefix2));

	m_S2_10.Format( "%s10", LPCTSTR( m_Prefix2));
	m_S2_11.Format( "%s11", LPCTSTR( m_Prefix2));
	m_S2_12.Format( "%s12", LPCTSTR( m_Prefix2));
	m_S2_13.Format( "%s13", LPCTSTR( m_Prefix2));

	m_S2_20.Format( "%s20", LPCTSTR( m_Prefix2));
	m_S2_21.Format( "%s21", LPCTSTR( m_Prefix2));
	m_S2_22.Format( "%s22", LPCTSTR( m_Prefix2));
	m_S2_23.Format( "%s23", LPCTSTR( m_Prefix2));

	m_S2_30.Format( "%s30", LPCTSTR( m_Prefix2));
	m_S2_31.Format( "%s31", LPCTSTR( m_Prefix2));
	m_S2_32.Format( "%s32", LPCTSTR( m_Prefix2));
	m_S2_33.Format( "%s33", LPCTSTR( m_Prefix2));

	UpdateData( FALSE);	
}

