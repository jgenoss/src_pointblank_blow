// i3TDKSpec_Transform.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Transform.h"
#include ".\i3tdkspec_transform.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_Transform dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Transform, i3TDKDialogBase)

i3TDKSpec_Transform::~i3TDKSpec_Transform()
{
	I3_SAFE_RELEASE( m_pTrans);
}

void i3TDKSpec_Transform::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TDK_ED_POSX, m_PosX_Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_POSY, m_PosY_Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_POSZ, m_PosZ_Ctrl);

	DDX_Control(pDX, IDC_TDK_ED_ROTX, m_RotX_Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_ROTY, m_RotY_Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_ROTZ, m_RotZ_Ctrl);

	DDX_Control(pDX, IDC_TDK_ED_SCALEX, m_ScaleX_Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_SCALEY, m_ScaleY_Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_SCALEZ, m_ScaleZ_Ctrl);

	DDX_Control(pDX, IDC_TDK_ED_ANIM, m_ED_Anim);
}

void i3TDKSpec_Transform::SetObject( i3ElementBase * pObj)
{
	TCHAR conv[256];

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pTrans, ((i3Transform *) pObj));

	if( m_pTrans != nullptr)
	{
		MATRIX * m = m_pTrans->GetMatrix();

		VEC3D pos;
		VEC3D scale;
		REAL32 angles[3];

		i3Vector::Copy(&pos, i3Matrix::GetPos(m));
		i3Matrix::GetScale(m, &scale);
		i3Matrix::DecomposeEulerAngleXYZ(m, angles);

		i3::ftoa(pos.x, conv);
		m_PosX_Ctrl.SetWindowText(conv);

		i3::ftoa(pos.y, conv);
		m_PosY_Ctrl.SetWindowText(conv);

		i3::ftoa(pos.z, conv);
		m_PosZ_Ctrl.SetWindowText(conv);

		i3::ftoa(I3_RAD2DEG(angles[0]), conv);
		m_RotX_Ctrl.SetWindowText(conv);

		i3::ftoa(I3_RAD2DEG(angles[1]), conv);
		m_RotY_Ctrl.SetWindowText(conv);

		i3::ftoa(I3_RAD2DEG(angles[2]), conv);
		m_RotZ_Ctrl.SetWindowText(conv);

		i3::ftoa(scale.x, conv);
		m_ScaleX_Ctrl.SetWindowText(conv);

		i3::ftoa(scale.y, conv);
		m_ScaleY_Ctrl.SetWindowText(conv);

		i3::ftoa(scale.z, conv);
		m_ScaleZ_Ctrl.SetWindowText(conv);

		if(m_pCB_setTransPos)
			m_pCB_setTransPos(m);
		// Animation
		if( m_pTrans->IsAnimated() && (m_pTrans->getAnimation(0) != nullptr))
		{
			i3Animation * pAnim = m_pTrans->getAnimation(0);

#ifdef _UNICODE
			i3::stack_wstring wstrAniName;	i3::mb_to_utf16(pAnim->GetName(), wstrAniName);
			const wchar_t* szAniName = wstrAniName.c_str();
#else
			const char* szAniName = pAnim->GetName();
#endif
			m_ED_Anim.SetWindowText( szAniName );
		}
	}
	else
	{
		m_PosX_Ctrl.SetWindowText( _T("") );
		m_PosY_Ctrl.SetWindowText( _T("") );
		m_PosZ_Ctrl.SetWindowText( _T("") );
		m_RotX_Ctrl.SetWindowText( _T("") );
		m_RotY_Ctrl.SetWindowText( _T("") );
		m_RotZ_Ctrl.SetWindowText( _T("") );
		m_ScaleX_Ctrl.SetWindowText( _T("") );
		m_ScaleY_Ctrl.SetWindowText( _T("") );
		m_ScaleZ_Ctrl.SetWindowText( _T("") );
	}

	m_ED_Anim.EnableWindow( m_pTrans != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Transform, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BTN_ANIM, OnBnClickedBtnAnim)

	ON_EN_KILLFOCUS(IDC_TDK_ED_POSX, SetModifiedTransform)
	ON_EN_KILLFOCUS(IDC_TDK_ED_POSY, SetModifiedTransform)
	ON_EN_KILLFOCUS(IDC_TDK_ED_POSZ, SetModifiedTransform)

	ON_EN_KILLFOCUS(IDC_TDK_ED_ROTX, SetModifiedTransform)
	ON_EN_KILLFOCUS(IDC_TDK_ED_ROTY, SetModifiedTransform)
	ON_EN_KILLFOCUS(IDC_TDK_ED_ROTZ, SetModifiedTransform)

	ON_EN_KILLFOCUS(IDC_TDK_ED_SCALEX, SetModifiedTransform)
	ON_EN_KILLFOCUS(IDC_TDK_ED_SCALEY, SetModifiedTransform)
	ON_EN_KILLFOCUS(IDC_TDK_ED_SCALEZ, SetModifiedTransform)
END_MESSAGE_MAP()


// i3TDKSpec_Transform message handlers

BOOL i3TDKSpec_Transform::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Transform::OnOK()
{
}

void i3TDKSpec_Transform::OnCancel()
{
}

LRESULT i3TDKSpec_Transform::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Transform::SetModifiedTransform()
{
	//9개 중 어느 박스가 수정되어도 전부 다시 읽어서 transform 세팅을 해준다.
	if(m_pTrans == nullptr)
		return; 

	TCHAR conv[256];

	VEC3D pos;
	REAL32 angle[3];
	VEC3D scale;
	VEC3D tempScale;

	m_PosX_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	pos.x = (REAL32)i3::atof(conv);
	m_PosX_Ctrl.SetModify( FALSE);

	m_PosY_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	pos.y = (REAL32)i3::atof(conv);
	m_PosY_Ctrl.SetModify( FALSE);

	m_PosZ_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	pos.z = (REAL32)i3::atof(conv);
	m_PosZ_Ctrl.SetModify( FALSE);

	m_RotX_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	angle[0] = I3_DEG2RAD((REAL32)i3::atof(conv));
	m_RotX_Ctrl.SetModify( FALSE);

	m_RotY_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	angle[1] = I3_DEG2RAD((REAL32)i3::atof(conv));
	m_RotY_Ctrl.SetModify( FALSE);

	m_RotZ_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	angle[2] = I3_DEG2RAD((REAL32)i3::atof(conv));
	m_RotZ_Ctrl.SetModify( FALSE);

	m_ScaleX_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	scale.x = (REAL32)i3::atof(conv); 
	m_ScaleX_Ctrl.SetModify( FALSE);

	m_ScaleY_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	scale.y = (REAL32)i3::atof(conv);
	m_ScaleY_Ctrl.SetModify( FALSE);

	m_ScaleZ_Ctrl.GetWindowText( conv, _countof(conv) - 1);
	scale.z = (REAL32)i3::atof(conv);
	m_ScaleZ_Ctrl.SetModify( FALSE);

	i3Matrix::ComposeEulerAngleXYZ(m_pTrans->GetMatrix(), angle);

	i3Matrix::PostScale(m_pTrans->GetMatrix(), scale.x, scale.y, scale.z);

	i3Matrix::SetPos(m_pTrans->GetMatrix(), pos.x, pos.y, pos.z);

	if( m_pCB_setTransPos != nullptr)
	{
		m_pCB_setTransPos( m_pTrans->GetMatrix());
	}
}

void i3TDKSpec_Transform::OnBnClickedBtnAnim()
{
	if( m_pTrans == nullptr)
		return;

	CFileDialog	Dlg( TRUE, _T("i3a"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("I3Engine Animation File(*.i3a)|*.i3a||"), this);

	if( Dlg.DoModal() != IDOK)
		return;

	i3::stack_string strDlgPathName;
#ifdef _UNICODE
	i3::utf16_to_mb(Dlg.GetPathName(), strDlgPathName);
	
#else
	strDlgPathName = Dlg.GetPathName();
#endif
	i3Animation * pAnim = i3Animation::LoadFromFile(strDlgPathName.c_str());

	if( pAnim == nullptr)
	{
		NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", strDlgPathName.c_str());
		return;
	}

//	pAnim->AddRef();		// i3Animation::LoadFromFile()에 AddRef()들어있어서 여기서는 주석처리..(2012.09.11.수빈)

	// Animation의 Track 중에서 i3Transform의 이름과 같은 Track을 찾는다.
	for( INT32 i = 0; i < pAnim->GetTrackCount(); i++)
	{
		if( i3::generic_is_iequal( pAnim->GetTrackBoneName( i), m_pTrans->GetName()) )
		{
			m_pTrans->SetAnimation( pAnim, i);		// Animation 설정.
			I3_MUST_RELEASE(pAnim);
			return;
		}
	}

	NotifyBox( m_hWnd, "%s Animation 파일에는 %s 이름의 Track이 없습니다.", LPCTSTR( Dlg.GetPathName()), m_pTrans->GetName());

	I3_MUST_RELEASE(pAnim);
}
