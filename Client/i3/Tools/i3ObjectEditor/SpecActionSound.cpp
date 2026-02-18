// SpecActionSound.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecActionSound.h"
#include "GlobalVar.h"
#include "MainFrm.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecActionSound dialog

IMPLEMENT_DYNAMIC(CSpecActionSound, i3TDKDialogBase)
CSpecActionSound::CSpecActionSound(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecActionSound::IDD, pParent)
	, m_edDefaultVolume(100)
	, m_edListenerVolume(100)
	, m_edMinDistance(0.1f)
	, m_edMaxDistance(100.0f)
	, m_edInsideConeAngle(360)
	, m_edOutsideConeAngle(360)
	, m_edOutsideVolume(100)
{
	m_pActionSound	= NULL;
	m_pPlayInfo		= NULL;
}

CSpecActionSound::~CSpecActionSound()
{
	I3_SAFE_RELEASE( m_pActionSound);
	I3_SAFE_RELEASE( m_pPlayInfo);
}

void CSpecActionSound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SOUND_PATH, m_edSoundPath);
	DDX_Text(pDX, IDC_EDIT_SOUND_VOLUME, m_edDefaultVolume);
	DDV_MinMaxInt(pDX, m_edDefaultVolume, 0, 100);
	DDX_Text(pDX, IDC_EDIT_SOUND_LISTENERVOLUME, m_edListenerVolume);
	DDV_MinMaxInt(pDX, m_edListenerVolume, 0, 100);
	DDX_Text(pDX, IDC_EDIT_SOUND_MINDIST, m_edMinDistance);
	DDV_MinMaxFloat(pDX, m_edMinDistance, 0.1f, 1000000.0f);
	DDX_Text(pDX, IDC_EDIT_SOUND_MAXDIST, m_edMaxDistance);
	DDV_MinMaxFloat(pDX, m_edMaxDistance, 1.0, 100000000.0f);
	DDX_Text(pDX, IDC_EDIT_SOUND_INSIDEANGLE, m_edInsideConeAngle);
	DDV_MinMaxInt(pDX, m_edInsideConeAngle, 0, 360);
	DDX_Text(pDX, IDC_EDIT_SOUND_OUTSIDEANGLE, m_edOutsideConeAngle);
	DDV_MinMaxInt(pDX, m_edOutsideConeAngle, 0, 360);
	DDX_Text(pDX, IDC_EDIT_SOUND_OUTSIDEVOLUME, m_edOutsideVolume);
	DDV_MinMaxInt(pDX, m_edOutsideVolume, 0, 100);
	DDX_Control(pDX, IDC_EDIT_SOUND_DIRECTION_X, m_edDirection_X);
	DDX_Control(pDX, IDC_EDIT_SOUND_DIRECTION_Y, m_edDirection_Y);
	DDX_Control(pDX, IDC_EDIT_SOUND_DIRECTION_Z, m_edDirection_Z);
	DDX_Control(pDX, IDC_EDIT_SOUND_VOLUME, m_ctrlDefaultVolume);
	DDX_Control(pDX, IDC_EDIT_SOUND_LISTENERVOLUME, m_ctrlListenerVolume);
	DDX_Control(pDX, IDC_EDIT_SOUND_MINDIST, m_ctrlMinDistance);
	DDX_Control(pDX, IDC_EDIT_SOUND_MAXDIST, m_ctrlMaxDistance);
	DDX_Control(pDX, IDC_EDIT_SOUND_INSIDEANGLE, m_ctrlInsideConeAngle);
	DDX_Control(pDX, IDC_EDIT_SOUND_OUTSIDEANGLE, m_ctrlOutsideConeAngle);
	DDX_Control(pDX, IDC_EDIT_SOUND_OUTSIDEVOLUME, m_ctrlOutsideVolume);
	DDX_Control(pDX, IDC_CHK_MUTE_AT_MAXDISTANCE, m_btnMuteAtMaxDistance);
	DDX_Control(pDX, IDC_CHK_STOP_ON_RESET, m_btnStopOnReset);
	DDX_Control(pDX, IDC_CHK_REPEAT, m_btnRepeat);
	DDX_Control(pDX, IDC_CHK_ATTACH, m_chkAttach);
	DDX_Control(pDX, IDC_CB_BONELIST, m_cbBoneList);
	DDX_Control(pDX, IDC_CHK_SOUND_RANDOM_ENABLE, m_chkRandomEnable);
	DDX_Control(pDX, IDC_ED_SOUND_RANDOM_FACTOR, m_edRandomFactor);
}


BEGIN_MESSAGE_MAP(CSpecActionSound, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BTN_SELECT_PATH, OnBnClickedBtnSelectPath)
	ON_EN_CHANGE(IDC_EDIT_SOUND_MAXDIST, OnEnChangeEditSoundMaxdist)
	ON_EN_CHANGE(IDC_EDIT_SOUND_DIRECTION_X, OnEnChangeEditSoundDirectionX)
	ON_EN_CHANGE(IDC_EDIT_SOUND_DIRECTION_Y, OnEnChangeEditSoundDirectionY)
	ON_EN_CHANGE(IDC_EDIT_SOUND_DIRECTION_Z, OnEnChangeEditSoundDirectionZ)
	ON_EN_CHANGE(IDC_EDIT_SOUND_INSIDEANGLE, OnEnChangeEditSoundInsideangle)
	ON_EN_CHANGE(IDC_EDIT_SOUND_LISTENERVOLUME, OnEnChangeEditSoundListenervolume)
	ON_EN_CHANGE(IDC_EDIT_SOUND_MINDIST, OnEnChangeEditSoundMindist)
	ON_EN_CHANGE(IDC_EDIT_SOUND_OUTSIDEANGLE, OnEnChangeEditSoundOutsideangle)
	ON_EN_CHANGE(IDC_EDIT_SOUND_OUTSIDEVOLUME, OnEnChangeEditSoundOutsidevolume)
	ON_EN_CHANGE(IDC_EDIT_SOUND_VOLUME, OnEnChangeEditSoundVolume)
	ON_BN_CLICKED(IDC_CHK_MUTE_AT_MAXDISTANCE, OnBnClickedChkMuteAtMaxdistance)
	ON_BN_CLICKED(IDC_CHK_STOP_ON_RESET, OnBnClickedChkStopOnReset)
	ON_BN_CLICKED(IDC_CHK_REPEAT, OnBnClickedChkRepeat)
	ON_BN_CLICKED(IDC_CHK_ATTACH, OnBnClickedChkAttach)
	ON_CBN_SELCHANGE(IDC_CB_BONELIST, OnCbnSelchangeCbBoneList)
	ON_BN_CLICKED(IDC_CHK_SOUND_RANDOM_ENABLE, OnBnClickedCheckSndRandomEnable)
	ON_EN_CHANGE(IDC_ED_SOUND_RANDOM_FACTOR, OnEnChangeEditSndRandomFactor)
END_MESSAGE_MAP()


// CSpecActionSound message handlers

void CSpecActionSound::SetObject( i3ElementBase * pObj)
{
	i3ActionPlaySound * pAction = (i3ActionPlaySound *)pObj;

	I3_REF_CHANGE( m_pActionSound, pAction);

	if( m_pActionSound != NULL)
	{
		i3SoundNode * pSndNode = pAction->getSoundNode();
		if( (pSndNode == NULL) || (pSndNode->getSoundPlayInfo() == NULL))
		{
			UpdateVariables( NULL);
		}
		else
		{
			i3SoundPlayInfo * pInfo = pSndNode->getSoundPlayInfo();

			UpdateVariables( pInfo);
		}
	}
	else
	{
		UpdateVariables( NULL);
	}

	if( pAction->getAttachEnable())
	{
		m_chkAttach.SetCheck( BST_CHECKED);
	}
	else
	{
		m_chkAttach.SetCheck( BST_UNCHECKED);
	}

	// Bone 이름 추가
	{
		CMainFrame * pFrameWnd = (CMainFrame *) AfxGetMainWnd();

		INT32 idxState = pFrameWnd->getSelectedState();
		//if( idxState == -1) idxState = 0;
		if( idxState != -1)
		{
			CObjectState * pState = g_pObject->getState( idxState);

			i3::vector<i3Node*> list;

			i3SceneGraphInfo * pSgInfo = pState->getSgResource();

			while((pSgInfo == NULL) && (idxState > 0))
			{
				idxState --;
				pState = g_pObject->getState( idxState);

				pSgInfo = pState->getSgResource();
			}

			m_cbBoneList.ResetContent();

			if( pSgInfo != NULL)
			{
				i3Scene::FindNodeByType( pSgInfo->getInstanceSg(), i3Transform::static_meta(), list);

				for( size_t i = 0; i < list.size(); i++)
				{
					i3Transform * pTrans = (i3Transform *) list[i];

					if( (pTrans->hasName()) && (pTrans->IsFlag( I3_NODEFLAG_VOLATILE) == FALSE) && (pTrans->IsFlag( I3_NODEFLAG_CONTROL) == FALSE))
					{
						m_cbBoneList.AddString( pTrans->GetName());
					}
				}
			}

			// Attached Bone Name
			if( pAction->getAttachBoneName() != NULL)
			{
				INT32 idx;

				idx = m_cbBoneList.FindString( 0, pAction->getAttachBoneName());

				m_cbBoneList.SetCurSel( idx);
			}
			else
			{
				m_cbBoneList.SetCurSel( -1);
			}
		}
	}

	if( pAction->isRandomEnable())
	{
		char conv[MAX_PATH];

		m_chkRandomEnable.SetCheck(BST_CHECKED);

		sprintf( conv, "%d", pAction->getRandomFactor());

		m_edRandomFactor.SetWindowText( conv);
	}
	else
	{
		m_chkRandomEnable.SetCheck(BST_UNCHECKED);

		m_edRandomFactor.SetWindowText( "Not Available");
	}
}

void CSpecActionSound::UpdateVariables( i3SoundPlayInfo * pInfo)
{
	I3_REF_CHANGE( m_pPlayInfo, pInfo);

	char conv[MAX_PATH];
	if( pInfo == NULL)
	{
		sprintf( conv, "No Sound Selected");

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
		m_edDirection_Y .SetWindowText( conv);

		m_btnMuteAtMaxDistance.SetCheck( BST_UNCHECKED);
		m_btnStopOnReset.SetCheck( BST_UNCHECKED);
		m_chkAttach.SetCheck( BST_UNCHECKED);
		m_cbBoneList.SetCurSel( -1);
	}
	else
	{
		m_edSoundPath.SetWindowText( pInfo->getSound()->GetName());

		::itoa( pInfo->GetDefaultVolume(), conv, 10);
		m_ctrlDefaultVolume.SetWindowText( conv);

		::itoa( pInfo->getListenerVolume(), conv, 10);
		m_ctrlListenerVolume.SetWindowText( conv);

		i3::ftoa( pInfo->getMinDistance(), conv);
		m_ctrlMinDistance.SetWindowText( conv);

		i3::ftoa( pInfo->getMaxDistance(), conv);
		m_ctrlMaxDistance.SetWindowText( conv);

		::itoa( pInfo->getInsideConeAngle(), conv, 10);
		m_ctrlInsideConeAngle.SetWindowText( conv);

		::itoa( pInfo->getOutsideConeAngle(), conv, 10);
		m_ctrlOutsideConeAngle.SetWindowText( conv);

		::itoa( pInfo->getOutsideVolume(), conv, 10);
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

		if( pInfo->isStopOnReset())
		{
			m_btnStopOnReset.SetCheck( BST_CHECKED);
		}
		else
		{
			m_btnStopOnReset.SetCheck( BST_UNCHECKED);
		}

		if( pInfo->GetLoopMode() & I3SND_RT_LOOPMODE_LOOPING)
		{
			m_btnRepeat.SetCheck( BST_CHECKED);
		}
		else
		{
			m_btnRepeat.SetCheck( BST_UNCHECKED);
		}
	}
}

void CSpecActionSound::OnCancel()
{
}

void CSpecActionSound::OnOK()
{
}

void CSpecActionSound::OnBnClickedBtnSelectPath()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, "wav", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"pcm riff wave File(*.wav)|*.wav||", this);

	if( Dlg.DoModal() != IDOK)	return;

	i3SoundFile File;
	char szRelPath[MAX_PATH];

	i3Sound * pNewSndSource = File.Load( LPCTSTR( Dlg.GetPathName()));
	if( pNewSndSource == NULL)
	{
		return ;
	}

	I3ASSERT( m_pActionSound != NULL);

	if( m_pPlayInfo == NULL)
	{
		m_pPlayInfo = i3SoundPlayInfo::new_object();

		m_pPlayInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
		m_pPlayInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);

		i3SoundNode * pNode = i3SoundNode::new_object_ref();

		pNode->setSoundPlayInfo( m_pPlayInfo);

		m_pActionSound->setSoundNode( pNode);
	}

	// Working Folder에서의 Relative Path로 조정한다.
	{
		pNewSndSource->SetExternRes( TRUE);

		i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR( Dlg.GetPathName()), szRelPath );

		pNewSndSource->SetName( szRelPath);
	}

	m_pPlayInfo->setSound( pNewSndSource);

	UpdateVariables( m_pPlayInfo);
}

void CSpecActionSound::OnEnChangeEditSoundMaxdist()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		char conv[MAX_PATH];
		m_ctrlMaxDistance.GetWindowText( conv, MAX_PATH -1);

		m_pPlayInfo->SetDistanceInfo( (REAL32)atof( conv), m_pPlayInfo->getMinDistance(), m_pPlayInfo->getVolumeType());
	}
}

void CSpecActionSound::OnEnChangeEditSoundDirectionX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		VEC3D vDirection;
		i3Vector::Copy( &vDirection, m_pPlayInfo->getDirection());

		char conv[MAX_PATH];
		
		m_edDirection_X.GetWindowText( conv, MAX_PATH - 1);

		i3Vector::SetX( &vDirection, (REAL32)atof( conv));

		m_pPlayInfo->setDirection( &vDirection);
	}
}

void CSpecActionSound::OnEnChangeEditSoundDirectionY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		VEC3D vDirection;
		i3Vector::Copy( &vDirection, m_pPlayInfo->getDirection());

		char conv[MAX_PATH];
		
		m_edDirection_Y.GetWindowText( conv, MAX_PATH - 1);

		i3Vector::SetY( &vDirection, (REAL32)atof( conv));

		m_pPlayInfo->setDirection( &vDirection);
	}
}

void CSpecActionSound::OnEnChangeEditSoundDirectionZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		VEC3D vDirection;
		i3Vector::Copy( &vDirection, m_pPlayInfo->getDirection());

		char conv[MAX_PATH];
		
		m_edDirection_Z.GetWindowText( conv, MAX_PATH - 1);

		i3Vector::SetZ( &vDirection, (REAL32)atof( conv));

		m_pPlayInfo->setDirection( &vDirection);
	}
}

void CSpecActionSound::OnEnChangeEditSoundInsideangle()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		char conv[MAX_PATH];
		m_ctrlInsideConeAngle.GetWindowText( conv, MAX_PATH -1);

		m_pPlayInfo->setInsideConeAngle( (INT32)atoi( conv));
	}
}

void CSpecActionSound::OnEnChangeEditSoundListenervolume()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		char conv[MAX_PATH];
		m_ctrlListenerVolume.GetWindowText( conv, MAX_PATH -1);

		m_pPlayInfo->setListenerVolume( (INT32)atoi( conv));
	}
}

void CSpecActionSound::OnEnChangeEditSoundMindist()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		char conv[MAX_PATH];
		m_ctrlMinDistance.GetWindowText( conv, MAX_PATH -1);

		m_pPlayInfo->SetDistanceInfo( m_pPlayInfo->getMaxDistance(), (REAL32)atof( conv), m_pPlayInfo->getVolumeType());
	}
}

void CSpecActionSound::OnEnChangeEditSoundOutsideangle()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		char conv[MAX_PATH];
		m_ctrlOutsideConeAngle.GetWindowText( conv, MAX_PATH -1);

		m_pPlayInfo->setOutsideConeAngle( (INT32)atoi( conv));
	}
}

void CSpecActionSound::OnEnChangeEditSoundOutsidevolume()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		char conv[MAX_PATH];
		m_ctrlOutsideVolume.GetWindowText( conv, MAX_PATH -1);

		m_pPlayInfo->setOutsideVolume( (UINT8)(INT32)atoi( conv));
	}
}

void CSpecActionSound::OnEnChangeEditSoundVolume()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		char conv[MAX_PATH];
		m_ctrlDefaultVolume.GetWindowText( conv, MAX_PATH -1);

		m_pPlayInfo->SetDefaultVolume( (INT32)atoi( conv));
	}
}

void CSpecActionSound::OnBnClickedChkMuteAtMaxdistance()
{
	// TODO: Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		if( m_btnMuteAtMaxDistance.GetCheck() == BST_CHECKED)
		{
			m_pPlayInfo->setMuteAtMaxDistance( TRUE);
		}
		else
		{
			m_pPlayInfo->setMuteAtMaxDistance( FALSE);
		}
	}
}

void CSpecActionSound::OnBnClickedChkStopOnReset()
{
	if( m_pPlayInfo != NULL)
	{
		if( m_btnStopOnReset.GetCheck() == BST_CHECKED)
		{
			m_pPlayInfo->setStopOnReset( true);
		}
		else
		{
			m_pPlayInfo->setStopOnReset( false);
		}
	}
}

void CSpecActionSound::OnBnClickedChkRepeat()
{
	// TODO: Add your control notification handler code here
	if( m_pPlayInfo != NULL)
	{
		if( m_btnRepeat.GetCheck() == BST_CHECKED)
		{
			m_pPlayInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
		}
		else
		{
			m_pPlayInfo->setLoopMode( I3SND_RT_LOOPMODE_ONESHOT);
		}
	}
}

void CSpecActionSound::OnBnClickedChkAttach()
{
	if( m_pActionSound == NULL)
		return;

	if( m_pActionSound->getAttachEnable() != ( m_chkAttach.GetCheck() == BST_CHECKED))
	{
		m_pActionSound->setAttachEnable( (bool)(m_chkAttach.GetCheck() == BST_CHECKED));
	}
}

void CSpecActionSound::OnCbnSelchangeCbBoneList()
{
	if( m_pActionSound == NULL)
		return;

	INT32 idx = m_cbBoneList.GetCurSel();

	if( idx != -1)
	{
		char conv[MAX_PATH];

		m_cbBoneList.GetLBText( idx, conv);

		m_pActionSound->SetAttachBoneName( conv);
	}
}

void CSpecActionSound::OnBnClickedCheckSndRandomEnable()
{
	// TODO: Add your control notification handler code here
	if( m_pActionSound != NULL)
	{
		m_pActionSound->setRandomEnable( m_chkRandomEnable.GetCheck() == BST_CHECKED);
	}
}

void CSpecActionSound::OnEnChangeEditSndRandomFactor()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_pActionSound != NULL)
	{
		char conv[MAX_PATH];
		m_edRandomFactor.GetWindowText( conv, MAX_PATH -1);

		m_pActionSound->setRandomFactor( (UINT16)(INT32)atoi( conv));
	}
}

