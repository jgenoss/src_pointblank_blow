// i3TDKSpec_Body.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Body.h"
#include "i3TDKDlg_Skeleton.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_Body dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Body, i3TDKDialogBase)

i3TDKSpec_Body::~i3TDKSpec_Body()
{
	I3_SAFE_RELEASE( m_pBody);
}

void i3TDKSpec_Body::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_AUTOLOD, m_CHK_AutoLOD);
	DDX_Control(pDX, IDC_TDK_ED_SLOPE, m_ED_Slope);
	DDX_Control(pDX, IDC_TDK_ED_CONST, m_ED_Const);
	DDX_Control(pDX, IDC_TDK_ED_ANIM, m_ED_Anim);
	DDX_Control(pDX, IDC_TDK_BTN_FRAMESKEL, m_BTN_Skel);
	DDX_Control(pDX, IDC_TDK_BTN_ANIM, m_BTN_Anim);
	DDX_Control(pDX, IDC_ST_BONECOUNT, m_ST_BoneCount);
	DDX_Control(pDX, IDC_TDK_ET_ROTATE_X, m_ED_RotateX);
	DDX_Control(pDX, IDC_TDK_ET_ROTATE_Y, m_ED_RotateY);
	DDX_Control(pDX, IDC_TDK_ET_ROTATE_Z, m_ED_RotateZ);
	DDX_Control(pDX, IDC_TDK_ET_ROTATE_W, m_ED_RotateW);
	DDX_Control(pDX, IDC_TDK_ET_POS_X, m_ED_PosX);
	DDX_Control(pDX, IDC_TDK_ET_POS_Y, m_ED_PosY);
	DDX_Control(pDX, IDC_TDK_ET_POS_Z, m_ED_PosZ);
	DDX_Control(pDX, IDC_TDK_SPIN_ROTATE_X, m_SP_RotateX);
	DDX_Control(pDX, IDC_TDK_SPIN_ROTATE_Y, m_SP_RotateY);
	DDX_Control(pDX, IDC_TDK_SPIN_ROTATE_Z, m_SP_RotateZ);
	DDX_Control(pDX, IDC_TDK_SPIN_POS_X, m_SP_PosX);
	DDX_Control(pDX, IDC_TDK_SPIN_POS_Y, m_SP_PosY);
	DDX_Control(pDX, IDC_TDK_SPIN_POS_Z, m_SP_PosZ);
	//DDX_Control(pDX, IDC_TDK_BTN_ROTATEINIT, m_BTN_RotateInit);
}

void i3TDKSpec_Body::SetObject( i3ElementBase * pObj)
{
	TCHAR conv[256];

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pBody, ((i3Body *) pObj));

	m_CHK_AutoLOD.EnableWindow( m_pBody != nullptr);
	m_ED_Slope.EnableWindow( m_pBody != nullptr);
	m_ED_Const.EnableWindow( m_pBody != nullptr);
	m_ED_Anim.EnableWindow( m_pBody != nullptr);
	m_BTN_Skel.EnableWindow( m_pBody != nullptr);
	m_BTN_Anim.EnableWindow( m_pBody != nullptr);

	((CButton *)GetDlgItem(IDC_TDK_CHECK_USEOBJECT))->SetCheck(false);

	MATRIX* mt = GetRootMatrix();

	if( mt )
	{
		if( !m_pOldObj )
		{
			i3Matrix::Copy(&m_matOri, mt);
		}
		else
		{
			if( m_pOldObj != pObj )
			{
				i3Matrix::Copy(&m_matOri, mt);
				i3Quat::Identity(&m_Quat);
			}
		}
	}

	if( m_pBody != nullptr)
	{
		// Auto LOD
		if( m_pBody->getStyle() & I3BS_LOD_AUTO)
			m_CHK_AutoLOD.SetCheck( BST_CHECKED);
		else
			m_CHK_AutoLOD.SetCheck( BST_UNCHECKED);

		// Slope
		i3::ftoa( m_pBody->getSlope(), conv);
		m_ED_Slope.SetWindowText( conv);
		
		// Const
		i3::ftoa( m_pBody->getConst(), conv);
		m_ED_Const.SetWindowText( conv);

		// Skeleton
		i3::snprintf( conv, _countof(conv), _T("(%d) bones"), GetTotalBoneCount());
		m_ST_BoneCount.SetWindowText( conv);

		i3Vector::Zero(&m_Pos);
		SetTextRotate(true);
		SetTextPosition();

		CalculateDist();
		
		// Animation
		{
			i3Animation * pAnim = m_pBody->GetCurrentAnim();

			if( pAnim == nullptr)
			{
				m_ED_Anim.SetWindowText( _T("(none)") );
			}
			else
			{
				if( pAnim->IsExternRes())
				{
#ifdef _UNICODE
					i3::stack_wstring wstrAniName;	i3::mb_to_utf16(pAnim->GetName(), wstrAniName);
					const wchar_t* szAniName = wstrAniName.c_str();
#else
					const char* szAniName = pAnim->GetName();
#endif
					m_ED_Anim.SetWindowText( szAniName );
				}
				else
				{
					m_ED_Anim.SetWindowText( _T("Internal animation") );
				}
			}
		}
	}

	m_pOldObj = pObj;
}

INT32 i3TDKSpec_Body::GetTotalBoneCount(void)
{
	I3ASSERT( m_pBody != nullptr);
	
	if( m_pBody->getFrameLOD() == nullptr)
		return 0;

	i3LOD * pLOD = m_pBody->getFrameLOD();

	if( pLOD->getSkeleton() == nullptr)
		return 0;

	i3Skeleton * pSkel = pLOD->getSkeleton();

	return pSkel->getBoneCount();
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Body, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_TDK_AUTOLOD, OnBnClickedTdkAutolod)
	ON_EN_KILLFOCUS(IDC_TDK_ED_SLOPE, OnEnKillfocusTdkEdSlope)
	ON_EN_KILLFOCUS(IDC_TDK_ED_CONST, OnEnKillfocusTdkEdConst)
	ON_BN_CLICKED(IDC_TDK_BTN_FRAMESKEL, OnBnClickedTdkBtnFrameskel)
	ON_BN_CLICKED(IDC_TDK_BTN_ANIM, OnBnClickedTdkBtnAnim)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TDK_SPIN_ROTATE_X, &i3TDKSpec_Body::OnDeltaposSpinRotateX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TDK_SPIN_ROTATE_Y, &i3TDKSpec_Body::OnDeltaposSpinRotateY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TDK_SPIN_ROTATE_Z, &i3TDKSpec_Body::OnDeltaposSpinRotateZ)

	ON_NOTIFY(UDN_DELTAPOS, IDC_TDK_SPIN_POS_X, &i3TDKSpec_Body::OnDeltaposSpinPosX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TDK_SPIN_POS_Y, &i3TDKSpec_Body::OnDeltaposSpinPosY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TDK_SPIN_POS_Z, &i3TDKSpec_Body::OnDeltaposSpinPosZ)

	//ON_BN_CLICKED(IDC_TDK_BTN_ROTATEINIT, &i3TDKSpec_Body::OnBnClickedTdkBtnRotateinit)
	ON_BN_CLICKED(IDC_TDK_CHECK_USEOBJECT, &i3TDKSpec_Body::OnBnClickedTdkCheckboxUseRotate)
	ON_BN_CLICKED(IDC_BTN_SKELETONBONE, &i3TDKSpec_Body::OnBnClickedBtnSkeletonbone)
END_MESSAGE_MAP()


// i3TDKSpec_Body message handlers

BOOL i3TDKSpec_Body::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Body::OnOK()
{
}

void i3TDKSpec_Body::OnCancel()
{
}

LRESULT i3TDKSpec_Body::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Body::OnBnClickedTdkAutolod()
{
	if( m_pBody == nullptr)
		return;

	if( m_CHK_AutoLOD.GetCheck() == BST_CHECKED)
		m_pBody->addStyle( I3BS_LOD_AUTO);
	else
		m_pBody->removeStyle( I3BS_LOD_AUTO);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);
}

void i3TDKSpec_Body::OnEnKillfocusTdkEdSlope()
{
	if( m_pBody == nullptr)
		return;

	TCHAR conv[512];

	m_ED_Slope.GetWindowText( conv, _countof( conv));
	m_pBody->setSlope( (REAL32) i3::atof( conv));

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);
}

void i3TDKSpec_Body::OnEnKillfocusTdkEdConst()
{
	if( m_pBody == nullptr)
		return;

	TCHAR conv[512];

	m_ED_Const.GetWindowText( conv, _countof( conv));
	m_pBody->setConst( (REAL32) i3::atof( conv));

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);
}

void i3TDKSpec_Body::OnBnClickedTdkBtnFrameskel()
{
	if( m_pBody == nullptr)
		return;

	if( m_pBody->getFrameLOD() == nullptr)
		return;

	i3LOD * pLOD = m_pBody->getFrameLOD();

	if( pLOD->getSkeleton() == nullptr)
		return;

	{
		i3TDKDlg_Skeleton dlg;

		dlg.Execute( pLOD->getSkeleton());
	}
}

void i3TDKSpec_Body::OnBnClickedBtnSkeletonbone()
{
	I3TRACE("Skeleton Bone\n");

	// body와 LOD와 Skeleton 이 있어야 생성이 가능함.
	if( m_pBody == nullptr)
		return;

	if( m_pBody->getFrameLOD() == nullptr)
		return;

	i3LOD * pLOD = m_pBody->getFrameLOD();

	if( pLOD->getSkeleton() == nullptr)
		return;

	{
		m_pBody->ToggleRenderBone();
	}
}

void i3TDKSpec_Body::OnBnClickedTdkBtnAnim()
{
	if( m_pBody == nullptr)
		return;

	CFileDialog	Dlg( TRUE, _T("i3a"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("I3Engine Animation File(*.i3a)|*.i3a||"), this);

	if( Dlg.DoModal() != IDOK)
		return;

#ifdef _UNICODE
	i3::stack_string strDlgPathName;	i3::utf16_to_mb(Dlg.GetPathName(), strDlgPathName);
#else
	i3::stack_string strDlgPathName = (const char*)Dlg.GetPathName();
#endif

	i3Animation * pAnim = i3Animation::LoadFromFile(strDlgPathName.c_str());

	if( pAnim == nullptr)
	{
		NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", LPCTSTR( Dlg.GetPathName()));
		return;
	}

//	pAnim->AddRef();			// i3Animation::LoadFromFile()내부에 AddRef()추가되었으므로 여기서 주석처리(2012.09.11.수빈)

	m_pBody->PlayAnim( pAnim);

	I3_MUST_RELEASE(pAnim);

#ifdef _UNICODE
	i3::stack_wstring wstrAnimName;	i3::mb_to_utf16(pAnim->GetName(), wstrAnimName);
	const wchar_t* szAnimName = wstrAnimName.c_str();
#else
	const char* szAnimName = pAnim->GetName();
#endif

	m_ED_Anim.SetWindowText( szAnimName );
}

void i3TDKSpec_Body::OnDeltaposSpinRotateX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	MATRIX MatX, MatY, MatZ, MatOut;
	
	if( pNMUpDown->iDelta < 0 )
	{
		UpdateRotate(VEC3D(1.0f, 0.0f, 0.0f));
	}
	else
	{
		UpdateRotate(VEC3D(-1.0f, 0.0f, 0.0f));
	}

	SetTextRotate();	

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);

	*pResult = 0;
}

void i3TDKSpec_Body::OnDeltaposSpinRotateY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( pNMUpDown->iDelta < 0 )
	{
		UpdateRotate(VEC3D(0.0f, 1.0f, 0.0f));
	}
	else
	{
		UpdateRotate(VEC3D(0.0f, -1.0f, 0.0f));
	}

	SetTextRotate();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);
	
	*pResult = 0;
}

void i3TDKSpec_Body::OnDeltaposSpinRotateZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( pNMUpDown->iDelta < 0 )
	{
		UpdateRotate(VEC3D(0.0f, 0.0f, 1.0f));
	}
	else
	{
		UpdateRotate(VEC3D(0.0f, 0.0f, -1.0f));
	}

	SetTextRotate();


	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);
	
	*pResult = 0;
}

void i3TDKSpec_Body::OnDeltaposSpinPosX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	MATRIX MatX, MatY, MatZ, MatOut;
	
	if( pNMUpDown->iDelta < 0 )
	{
		UpdateTranslate(VEC3D(0.01f, 0.0f, 0.0f));
	}
	else
	{
		UpdateTranslate(VEC3D(-0.01f, 0.0f, 0.0f));
	}

	SetTextPosition();		

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);

	*pResult = 0;
}

void i3TDKSpec_Body::OnDeltaposSpinPosY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	MATRIX MatX, MatY, MatZ, MatOut;
	
	if( pNMUpDown->iDelta < 0 )
	{
		UpdateTranslate(VEC3D(0.0f, 0.01f, 0.0f));
	}
	else
	{
		UpdateTranslate(VEC3D(0.0f, -0.01f, 0.0f));
	}

	SetTextPosition();		

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);

	*pResult = 0;
}

void i3TDKSpec_Body::OnDeltaposSpinPosZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	MATRIX MatX, MatY, MatZ, MatOut;
	
	if( pNMUpDown->iDelta < 0 )
	{
		UpdateTranslate(VEC3D(0.0f, 0.0f, 0.01f));
	}
	else
	{
		UpdateTranslate(VEC3D(0.0f, 0.0f, -0.01f));
	}

	SetTextPosition();	

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBody);



	*pResult = 0;
}

//오브젝트 BoneMatrix 얻기
MATRIX* i3TDKSpec_Body::GetRootMatrix()
{
	if (m_pBody == nullptr)
		return nullptr;
	
	i3LOD * pLOD = nullptr;
	
	if( m_pBody->GetCurrentAnim() )
	{
		m_pBody->StopAnim();
	}

	pLOD = m_pBody->getFrameLOD();

	if( pLOD != nullptr)
	{
		i3Skeleton * pSkel = pLOD->getSkeleton();

		if( pSkel )
		{
			return pSkel->getMatrix( 0);
		}
	}

	
	return nullptr;
}

//오브젝트 회전 업데이트
void i3TDKSpec_Body::UpdateRotate(VEC3D vecAxis)
{
	MATRIX* mt = GetRootMatrix();
//	CWnd* ptrWnd = GetFocus();

	bool bCheck = ((CButton *)GetDlgItem(IDC_TDK_CHECK_USEOBJECT))->GetCheck() != FALSE;
	if( mt )
	{
		MATRIX rot;

		if( bCheck)
		{
			QUATERNION Quat;

			i3Quat::RotateAxis( &Quat, &vecAxis, I3_DEG2RAD(REAL32(2.0)));
			i3Matrix::SetRotateQuat( &rot, &Quat);
		}

		i3Matrix::Mul(mt, mt, &rot);
	}
}

//오브젝트 위치 업데이트
void i3TDKSpec_Body::UpdateTranslate(VEC3D vecPos)
{
	MATRIX* mt = GetRootMatrix();
	//	CWnd* ptrWnd = GetFocus();

	bool bCheck = ((CButton *)GetDlgItem(IDC_TDK_CHECK_USEOBJECT))->GetCheck() != FALSE;
	if( mt )
	{
		MATRIX rot;

		if( bCheck)
		{
			i3Matrix::SetTranslate( &rot, &vecPos);
		}

		i3Matrix::Mul(mt, mt, &rot);
		m_Pos += vecPos ;
	}
}

void i3TDKSpec_Body::OnBnClickedTdkCheckboxUseRotate()
{
	bool bCheck = ((CButton *)GetDlgItem(IDC_TDK_CHECK_USEOBJECT))->GetCheck() != FALSE;

	UseObjectRotate(bCheck);
}

//오브젝트 회전기능 사용유무 체크
void i3TDKSpec_Body::UseObjectRotate(bool bCheck)
{
	MATRIX* mt = GetRootMatrix();

	if( bCheck )
	{
		i3Matrix::Copy(mt, &m_matOri);

		i3Matrix::PostRotateYDeg(mt, 90.0f);
		i3Matrix::PostRotateZDeg(mt, 90.0f);

		i3BoundBox* pBox = m_pBody->GetBound();
		if( pBox )
		{
			VEC3D vec;
			pBox->GetCenter(&vec); 
			
			m_OffsetPos.x = vec.y;
			m_OffsetPos.y = -vec.z;
			m_OffsetPos.z = vec.x;
			MATRIX tempMat;

			i3Matrix::SetTranslate( &tempMat, &m_OffsetPos);

			i3Matrix::Mul(mt, mt, &tempMat);
		}

		SetTextRotate();
		//m_BTN_RotateInit.EnableWindow(true);
		m_SP_RotateX.EnableWindow(true);
		m_SP_RotateY.EnableWindow(true);
		m_SP_RotateZ.EnableWindow(true);
		m_SP_PosX.EnableWindow(true);
		m_SP_PosY.EnableWindow(true);
		m_SP_PosZ.EnableWindow(true);
	}
	else
	{
		i3Matrix::Copy(mt, &m_matOri);
		//m_BTN_RotateInit.EnableWindow(false);
		m_SP_RotateX.EnableWindow(false);
		m_SP_RotateY.EnableWindow(false);
		m_SP_RotateZ.EnableWindow(false);
		m_SP_PosX.EnableWindow(false);
		m_SP_PosY.EnableWindow(false);
		m_SP_PosZ.EnableWindow(false);
		//OnBnClickedTdkBtnRotateinit();
		i3Vector::Zero(&m_Pos);
		SetTextRotate(true);
		SetTextPosition();
	}
}

//회전EditBox 업데이트
void i3TDKSpec_Body::SetTextRotate(bool bZero)
{
	i3Quat::Identity(&m_Quat);

	if( !bZero )
	{
		MATRIX* mt = GetRootMatrix();
		i3Matrix::GetRotateQuat(&m_Quat, mt);
	}

	TCHAR conv[512];
	i3::ftoa( m_Quat.x, conv, 5);
	m_ED_RotateX.SetWindowText( conv);

	i3::ftoa( m_Quat.y, conv, 5);
	m_ED_RotateY.SetWindowText( conv);

	i3::ftoa( m_Quat.z, conv, 5);
	m_ED_RotateZ.SetWindowText( conv);

	i3::ftoa( m_Quat.w, conv, 5);
	m_ED_RotateW.SetWindowText( conv);
}

//위치EditBox 업데이트
void i3TDKSpec_Body::SetTextPosition()
{

	TCHAR conv[512];
	i3::ftoa( m_Pos.x, conv, 5);
	m_ED_PosX.SetWindowText( conv);

	i3::ftoa( m_Pos.y, conv, 5);
	m_ED_PosY.SetWindowText( conv);

	i3::ftoa( m_Pos.z, conv, 5);
	m_ED_PosZ.SetWindowText( conv);
}

//오브젝트의 바운드박스를 이용하여 카메라거리를 계산한다
void i3TDKSpec_Body::CalculateDist()
{
	i3BoundBox* pBox = m_pBody->GetBound();

	if( pBox )
	{
		VEC3D vec;
		i3Vector::Sub( &vec, pBox->GetMax(), pBox->GetMin());

		m_fDist = i3Math::Mul( i3Vector::Length( &vec), 1.3f);
	}
	else
	{
		m_fDist = 0.f;
	}
}