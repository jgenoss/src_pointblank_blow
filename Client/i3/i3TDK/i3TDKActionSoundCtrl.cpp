// i3TDKActionSoundCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKActionSoundCtrl.h"
#include ".\i3tdkactionsoundctrl.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"

// i3TDKActionSoundCtrl message handlers
IMPLEMENT_DYNAMIC(i3TDKActionSoundCtrl, i3TDKDialogBase)

i3TDKActionSoundCtrl::~i3TDKActionSoundCtrl()
{
	I3_SAFE_RELEASE( m_pActionSound);
	I3_SAFE_RELEASE( m_pPlayInfo);
}

void i3TDKActionSoundCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SND_RES, m_edSoundPath);
	DDX_Text(pDX, IDC_EDIT_SND_VOLUME, m_edDefaultVolume);
	DDV_MinMaxInt(pDX, m_edDefaultVolume, 0, 100);
	DDX_Text(pDX, IDC_EDIT_SND_LISTENERVOLUME, m_edListenerVolume);
	DDV_MinMaxInt(pDX, m_edListenerVolume, 0, 100);
	DDX_Text(pDX, IDC_EDIT_SND_MINDIST, m_edMinDistance);
	DDV_MinMaxFloat(pDX, m_edMinDistance, 0.1f, 1000000.0f);
	DDX_Text(pDX, IDC_EDIT_SND_MAXDIST, m_edMaxDistance);
	DDV_MinMaxFloat(pDX, m_edMaxDistance, 1.0, 100000000.0f);
	DDX_Text(pDX, IDC_EDIT_SND_INSIDEANGLE, m_edInsideConeAngle);
	DDV_MinMaxInt(pDX, m_edInsideConeAngle, 0, 360);
	DDX_Text(pDX, IDC_EDIT_SND_OUTSIDEANGLE, m_edOutsideConeAngle);
	DDV_MinMaxInt(pDX, m_edOutsideConeAngle, 0, 360);
	DDX_Text(pDX, IDC_EDIT_SND_OUTSIDEVOLUME, m_edOutsideVolume);
	DDV_MinMaxInt(pDX, m_edOutsideVolume, 0, 100);
	DDX_Control(pDX, IDC_EDIT_SND_DIRECTION_X, m_edDirection_X);
	DDX_Control(pDX, IDC_EDIT_SND_DIRECTION_Y, m_edDirection_Y);
	DDX_Control(pDX, IDC_EDIT_SND_DIRECTION_Z, m_edDirection_Z);
	DDX_Control(pDX, IDC_EDIT_SND_VOLUME, m_ctrlDefaultVolume);
	DDX_Control(pDX, IDC_EDIT_SND_LISTENERVOLUME, m_ctrlListenerVolume);
	DDX_Control(pDX, IDC_EDIT_SND_MINDIST, m_ctrlMinDistance);
	DDX_Control(pDX, IDC_EDIT_SND_MAXDIST, m_ctrlMaxDistance);
	DDX_Control(pDX, IDC_EDIT_SND_INSIDEANGLE, m_ctrlInsideConeAngle);
	DDX_Control(pDX, IDC_EDIT_SND_OUTSIDEANGLE, m_ctrlOutsideConeAngle);
	DDX_Control(pDX, IDC_EDIT_SND_OUTSIDEVOLUME, m_ctrlOutsideVolume);
	DDX_Control(pDX, IDC_CHECK_SND_MUTEAT_MAXDIST, m_btnMuteAtMaxDistance);
	DDX_Control(pDX, IDC_CHECK_SND_RANDOM_ENABLE, m_chkRandomEnable);
	DDX_Control(pDX, IDC_EDIT_SND_RANDOM_FACTOR, m_edRandomFactor);
}


BEGIN_MESSAGE_MAP(i3TDKActionSoundCtrl, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_SND_SELRES, OnBnClickedBtnSelectPath)
	ON_EN_CHANGE(IDC_EDIT_SND_MAXDIST, OnEnChangeEditSoundMaxdist)
	ON_EN_CHANGE(IDC_EDIT_SND_DIRECTION_X, OnEnChangeEditSoundDirectionX)
	ON_EN_CHANGE(IDC_EDIT_SND_DIRECTION_Y, OnEnChangeEditSoundDirectionY)
	ON_EN_CHANGE(IDC_EDIT_SND_DIRECTION_Z, OnEnChangeEditSoundDirectionZ)
	ON_EN_CHANGE(IDC_EDIT_SND_INSIDEANGLE, OnEnChangeEditSoundInsideangle)
	ON_EN_CHANGE(IDC_EDIT_SND_LISTENERVOLUME, OnEnChangeEditSoundListenervolume)
	ON_EN_CHANGE(IDC_EDIT_SND_MINDIST, OnEnChangeEditSoundMindist)
	ON_EN_CHANGE(IDC_EDIT_SND_OUTSIDEANGLE, OnEnChangeEditSoundOutsideangle)
	ON_EN_CHANGE(IDC_EDIT_SND_OUTSIDEVOLUME, OnEnChangeEditSoundOutsidevolume)
	ON_EN_CHANGE(IDC_EDIT_SND_VOLUME, OnEnChangeEditSoundVolume)
	ON_BN_CLICKED(IDC_CHECK_SND_MUTEAT_MAXDIST, OnBnClickedChkMuteAtMaxdistance)
	ON_BN_CLICKED(IDC_COMPLETE, OnBnClickedOk2)
	ON_BN_CLICKED(IDC_CHECK_SND_RANDOM_ENABLE, OnBnClickedCheckSndRandomEnable)
	ON_EN_CHANGE(IDC_EDIT_SND_RANDOM_FACTOR, OnEnChangeEditSndRandomFactor)
END_MESSAGE_MAP()


// i3TDKActionSoundCtrl message handlers

void i3TDKActionSoundCtrl::SetObject( i3ElementBase * pObj)
{
	i3ActionPlaySound * pAction = (i3ActionPlaySound *)pObj;

	I3_REF_CHANGE( m_pActionSound, pAction);

	if( m_pActionSound != nullptr)
	{
		i3SoundNode * pSndNode = pAction->getSoundNode();
		if( (pSndNode == nullptr) || (pSndNode->getSoundPlayInfo() == nullptr))
		{
			UpdateVariables( nullptr);
		}
		else
		{
			i3SoundPlayInfo * pInfo = pSndNode->getSoundPlayInfo();

			UpdateVariables( pInfo);
		}
	}
	else
	{
		UpdateVariables( nullptr);
	}
}

void i3TDKActionSoundCtrl::UpdateVariables( i3SoundPlayInfo * pInfo)
{
	I3_REF_CHANGE( m_pPlayInfo, pInfo);

	TCHAR conv[MAX_PATH];
	if( pInfo == nullptr)
	{
		i3::snprintf( conv, _countof(conv), _T("No Sound Selected") );

		m_edSoundPath.SetWindowText( conv);

		m_edDefaultVolume = 100;
		m_edListenerVolume = 100;
		m_edMinDistance	= 0.1f;
		m_edMaxDistance = 10000000.0f;
		m_edInsideConeAngle  = 360;
		m_edOutsideConeAngle  = 360;
		m_edOutsideVolume	= 100;
		
		i3::ftoa( 0.0f, conv);
		m_edDirection_X.SetWindowText( conv);
		m_edDirection_Z.SetWindowText( conv);

		i3::ftoa( 1.0f, conv);
		m_edDirection_Y.SetWindowText( conv);

		m_btnMuteAtMaxDistance.SetCheck( BST_UNCHECKED);

		m_chkRandomEnable.SetCheck( BST_UNCHECKED);

		i3::snprintf( conv, _countof(conv), _T("1") );
		m_edRandomFactor.SetWindowText( conv);
	}
	else
	{
#ifdef _UNICODE
		i3::stack_wstring strSoundName;	i3::mb_to_utf16(pInfo->getSound()->GetNameString(), strSoundName);
#else
		const i3::rc_string& strSoundName = pInfo->getSound()->GetNameString();
#endif
		m_edSoundPath.SetWindowText( strSoundName.c_str());

		i3::itoa( pInfo->GetDefaultVolume(), conv, _countof(conv));
		m_ctrlDefaultVolume.SetWindowText( conv);

		i3::itoa( pInfo->getListenerVolume(), conv, _countof(conv));
		m_ctrlListenerVolume.SetWindowText( conv);

		i3::ftoa( pInfo->getMinDistance(), conv);
		m_ctrlMinDistance.SetWindowText( conv);

		i3::ftoa( pInfo->getMaxDistance(), conv);
		m_ctrlMaxDistance.SetWindowText( conv);

		i3::itoa( pInfo->getInsideConeAngle(), conv, _countof(conv));
		m_ctrlInsideConeAngle.SetWindowText( conv);

		i3::itoa( pInfo->getOutsideConeAngle(), conv, _countof(conv));
		m_ctrlOutsideConeAngle.SetWindowText( conv);

		i3::itoa( pInfo->getOutsideVolume(), conv, _countof(conv) );
		m_ctrlOutsideVolume.SetWindowText( conv);

		i3::ftoa( getX( pInfo->getDirection()), conv);
		m_edDirection_X.SetWindowText( conv);

		i3::ftoa( getY( pInfo->getDirection()), conv);
		m_edDirection_Y.SetWindowText( conv);

		i3::ftoa( getZ( pInfo->getDirection()), conv);
		m_edDirection_Z.SetWindowText( conv);

		if( pInfo->isMuteAtMaxDistance())
		{
			m_btnMuteAtMaxDistance.SetCheck( BST_CHECKED);
		}
		else
		{
			m_btnMuteAtMaxDistance.SetCheck( BST_UNCHECKED);
		}

		if( m_pActionSound->isRandomEnable())
		{
			m_chkRandomEnable.SetCheck( BST_CHECKED);
		}
		else
		{
			m_chkRandomEnable.SetCheck( BST_UNCHECKED);
		}

		i3::snprintf( conv, _countof(conv), _T("%d"), m_pActionSound->getRandomFactor());
		m_edRandomFactor.SetWindowText( conv);
	}
}

void i3TDKActionSoundCtrl::OnCancel()
{
}

void i3TDKActionSoundCtrl::OnOK()
{
	
}

void i3TDKActionSoundCtrl::SetAction( i3Action * pAction)
{
	if( !i3::kind_of<i3ActionPlaySound*>(pAction))
		return;

	i3ActionPlaySound * pSndAction = (i3ActionPlaySound *)pAction;
	I3_REF_CHANGE( m_pActionSound, pSndAction);
}

void i3TDKActionSoundCtrl::OnBnClickedBtnSelectPath()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, _T("wav"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("pcm riff wave File(*.wav)|*.wav||"), this);

	if( Dlg.DoModal() != IDOK)	return;

	i3SoundFile File;

	i3::stack_string strFileName;
#ifdef _UNICODE
	i3::utf16_to_mb((const wchar_t*)Dlg.GetPathName(), strFileName);
#else
	strFileName = (const char*)Dlg.GetPathName();
#endif

	i3Sound * pNewSndSource = File.Load(strFileName.c_str());
	if( pNewSndSource == nullptr)
	{
		return ;
	}

	I3ASSERT( m_pActionSound != nullptr);

	if( m_pPlayInfo == nullptr)
	{
		m_pPlayInfo = i3SoundPlayInfo::new_object();

		m_pPlayInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
		m_pPlayInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);

		i3SoundNode * pNode = i3SoundNode::new_object_ref();

		pNode->setSoundPlayInfo( m_pPlayInfo);

		m_pActionSound->setSoundNode( pNode);
	}

	m_pPlayInfo->setSound( pNewSndSource);

	char conv[MAX_PATH], szRelPath[MAX_PATH];
#ifdef _UNICODE
	i3::stack_string strDlgPathName;	i3::utf16_to_mb(Dlg.GetPathName(), strDlgPathName);
#else
	i3::stack_string strDlgPathName = (const char*)Dlg.GetPathName();
#endif
	i3::snprintf( conv, _countof(conv), strDlgPathName);
	i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), conv, szRelPath);

	pNewSndSource->SetName( szRelPath);
	pNewSndSource->SetExternRes();

	UpdateVariables( m_pPlayInfo);
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundMaxdist()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_ctrlMaxDistance.GetWindowText( conv, _countof(conv) -1);

		m_pPlayInfo->SetDistanceInfo( (REAL32)i3::atof( conv), m_pPlayInfo->getMinDistance(), m_pPlayInfo->getVolumeType());
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundDirectionX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		VEC3D vDirection;
		i3Vector::Copy( &vDirection, m_pPlayInfo->getDirection());

		TCHAR conv[MAX_PATH];
		
		m_edDirection_X.GetWindowText( conv, _countof(conv) - 1);

		i3Vector::SetX( &vDirection, (REAL32)i3::atof( conv));

		m_pPlayInfo->setDirection( &vDirection);
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundDirectionY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		VEC3D vDirection;
		i3Vector::Copy( &vDirection, m_pPlayInfo->getDirection());

		TCHAR conv[MAX_PATH];
		
		m_edDirection_Y.GetWindowText( conv, _countof(conv) - 1);

		i3Vector::SetY( &vDirection, (REAL32)i3::atof( conv));

		m_pPlayInfo->setDirection( &vDirection);
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundDirectionZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		VEC3D vDirection;
		i3Vector::Copy( &vDirection, m_pPlayInfo->getDirection());

		TCHAR conv[MAX_PATH];
		
		m_edDirection_Z.GetWindowText( conv, _countof(conv) - 1);

		i3Vector::SetZ( &vDirection, (REAL32)i3::atof( conv));

		m_pPlayInfo->setDirection( &vDirection);
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundInsideangle()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_ctrlInsideConeAngle.GetWindowText( conv, _countof(conv) -1);

		m_pPlayInfo->setInsideConeAngle( (INT32)i3::atoi( conv));
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundListenervolume()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_ctrlListenerVolume.GetWindowText( conv, _countof(conv) -1);

		m_pPlayInfo->setListenerVolume( (INT32)i3::atoi( conv));
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundMindist()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_ctrlMinDistance.GetWindowText( conv, _countof(conv) -1);

		m_pPlayInfo->SetDistanceInfo( m_pPlayInfo->getMaxDistance(), (REAL32)i3::atof( conv), m_pPlayInfo->getVolumeType());
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundOutsideangle()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_ctrlOutsideConeAngle.GetWindowText( conv, _countof(conv) -1);

		m_pPlayInfo->setOutsideConeAngle( (INT32)i3::atoi( conv));
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundOutsidevolume()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_ctrlOutsideVolume.GetWindowText( conv, _countof(conv) -1);

		m_pPlayInfo->setOutsideVolume( (UINT8)i3::atoi( conv));
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSoundVolume()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_ctrlDefaultVolume.GetWindowText( conv, _countof(conv) -1);

		m_pPlayInfo->SetDefaultVolume( (INT32)i3::atoi( conv));
	}
}

void i3TDKActionSoundCtrl::OnBnClickedChkMuteAtMaxdistance()
{
	// TODO: Add your control notification handler code here
	if( m_pPlayInfo != nullptr)
	{
		if( m_btnMuteAtMaxDistance.GetCheck() == BST_CHECKED)
		{
			m_pPlayInfo->setMuteAtMaxDistance(true);
		}
		else
		{
			m_pPlayInfo->setMuteAtMaxDistance(false);
		}
	}
}


void i3TDKActionSoundCtrl::OnBnClickedOk2()
{
	// TODO: Add your control notification handler code here
	i3TDKDialogBase::OnOK();
}

BOOL i3TDKActionSoundCtrl::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	if( m_pActionSound != nullptr)
	{
		i3SoundNode * pSndNode = m_pActionSound->getSoundNode();
		if( (pSndNode == nullptr) || (pSndNode->getSoundPlayInfo() == nullptr))
		{
			UpdateVariables( nullptr);
		}
		else
		{
			i3SoundPlayInfo * pInfo = pSndNode->getSoundPlayInfo();

			UpdateVariables( pInfo);
		}
	}
	else
	{
		UpdateVariables( nullptr);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKActionSoundCtrl::OnBnClickedCheckSndRandomEnable()
{
	// TODO: Add your control notification handler code here
	if( m_pActionSound != nullptr)
	{
		m_pActionSound->setRandomEnable( m_chkRandomEnable.GetCheck() == BST_CHECKED);
	}
}

void i3TDKActionSoundCtrl::OnEnChangeEditSndRandomFactor()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pActionSound != nullptr)
	{
		TCHAR conv[MAX_PATH];
		m_edRandomFactor.GetWindowText( conv, _countof(conv) -1);

		m_pActionSound->setRandomFactor( (UINT16)i3::atoi( conv));
	}
}
