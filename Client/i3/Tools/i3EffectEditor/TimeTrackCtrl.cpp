// TimeTrackCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "TimeTrackCtrl.h"
#include "i3EffectEditorDlg.h"
#include "EditEffectSound.h"
#include "i3Base/string/ext/make_relative_path.h"

// CTimeTrackCtrl

IMPLEMENT_DYNAMIC(CTimeTrackCtrl, i3TDKTimeSeqCtrl)
CTimeTrackCtrl::CTimeTrackCtrl()
{
}

CTimeTrackCtrl::~CTimeTrackCtrl()
{
}

static void SetParticleAsFountain( i3ParticleInfo * pInfo)
{
	pInfo->SetType( i3ParticleInfo::PARTICLE_BILLBOARD);
	pInfo->SetLifeTime( 0.0f);
	pInfo->SetObjectLifeTime( 0.7f);
	pInfo->SetMassMin( 0.14f);
	pInfo->SetMassMax( 0.3f);
	pInfo->SetMaxObjectCount( 100);
	pInfo->SetMaxObjectCapacity( 100);
	pInfo->SetInitialObjectCount( 5);
	pInfo->SetEmitCount( 2);
	pInfo->SetEmitInterval( 10.0f * 0.001f);
	pInfo->SetVolatile( FALSE);
	pInfo->SetEmitType( i3ParticleInfo::EMIT_CONE);
	pInfo->SetEmitDirection( 0.0f, 1.0f, 0.0f);
	pInfo->SetEmitAngle( 13.0f);
	
	pInfo->SetPositionStyle( I3_PARTICLE_PARAM_STYLE_GRAVITY | I3_PARTICLE_PARAM_STYLE_CLAMP);
	pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_NONE);
	pInfo->SetPositionInit( 0, 24.0f);
	pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_COS);
	pInfo->SetPositionFactor( 1, -0.04f);
	pInfo->SetPositionMin( 0.0f);
	pInfo->SetPositionMax( 10000.0f);

	pInfo->SetScaleStyle( 0);
	pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_CONSTANT);
	pInfo->SetScaleInit( 0, 0.06f);
	pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_NONE);
	pInfo->SetScaleInit( 1, 0.0f);
	
	pInfo->SetColorStyle( 0);
	pInfo->SetRGBFunc( i3ParticleInfo::FUNC_CONSTANT);
	pInfo->SetRGBInit( 0x00AFAFAF);

	pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_COS);
	pInfo->SetAlphaInit( 0xAF);
}

static void SetParticleAsSmoke( i3ParticleInfo * pInfo)
{
	pInfo->SetType( i3ParticleInfo::PARTICLE_POINTSPRITE);
	pInfo->SetLifeTime( 0.0f);
	pInfo->SetObjectLifeTime( 5.0f);
	pInfo->SetMassMin( 0.001f);
	pInfo->SetMassMax( 0.002f);
	pInfo->SetMaxObjectCount( 100);
	pInfo->SetMaxObjectCapacity( 100);
	pInfo->SetInitialObjectCount( 5);
	pInfo->SetEmitCount( 1);
	pInfo->SetEmitInterval( 50.0f * 0.001f);
	pInfo->SetVolatile( FALSE);
	pInfo->SetEmitType( i3ParticleInfo::EMIT_CONE);
	pInfo->SetEmitDirection( 0.0f, 1.0f, 0.0f);
	pInfo->SetEmitAngle( 20.0f);
	
	pInfo->SetPositionStyle( 0);
	pInfo->SetPositionUpdateInterval( 0.0f); //0.16666666666666666666666666666667f);
	pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_CONSTANT);
	pInfo->SetPositionInit( 0, 4.0f);

	pInfo->SetScaleStyle( I3_PARTICLE_PARAM_STYLE_RANDOM);

	pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_NONE);
	pInfo->SetScaleInit( 0, 1.0f);
	
	pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_CONSTANT);
	pInfo->SetScaleInit( 1, 0.8f);
	
	pInfo->SetColorStyle( 0);
	pInfo->SetRGBFunc( i3ParticleInfo::FUNC_CONSTANT);
	pInfo->SetRGBInit( 0x00AFAFAF);

	pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_COS);
	pInfo->SetAlphaInit( 0xAF);
}

static void SetParticleAsDust( i3ParticleInfo * pInfo)
{
	pInfo->SetType( i3ParticleInfo::PARTICLE_POINTSPRITE);
	pInfo->SetLifeTime( 0.0f);
	pInfo->SetObjectLifeTime( 5.0f);
	pInfo->SetMassMin( 0.001f);
	pInfo->SetMassMax( 0.002f);
	pInfo->SetMaxObjectCount( 30);
	pInfo->SetMaxObjectCapacity( 30);
	pInfo->SetInitialObjectCount( 30);
	pInfo->SetEmitCount( 50);
	pInfo->SetEmitInterval( 50.0f * 0.001f);
	pInfo->SetVolatile( TRUE);
	pInfo->SetEmitType( i3ParticleInfo::EMIT_DISK);
	pInfo->SetEmitDirection( 1.0f, 0.3f, 0.0f);
	pInfo->SetEmitAngle( 10.0f);
	
	pInfo->SetPositionStyle( I3_PARTICLE_PARAM_STYLE_CLAMP);
	pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_NONE);
	pInfo->SetPositionInit( 0, 20.0f);
	pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_COS);
	pInfo->SetPositionFactor( 1, -30.0f);
	pInfo->SetPositionMin( 0.004f);
	pInfo->SetPositionMax( 10.0f);

	pInfo->SetScaleStyle( I3_PARTICLE_PARAM_STYLE_RANDOM);

	pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_NONE);
	pInfo->SetScaleInit( 0, 0.6f);
	
	pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_CONSTANT);
	pInfo->SetScaleInit( 1, 0.6f);
	
	pInfo->SetColorStyle( 0);
	pInfo->SetRGBFunc( i3ParticleInfo::FUNC_CONSTANT);
	pInfo->SetRGBInit( 0x00AFAFAF);

	pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_COS);
	pInfo->SetAlphaInit( 0xAF);
}

void CTimeTrackCtrl::OnNewParticle( PARTICLE_TYPE type)
{
	i3TimeSequenceInfo * pNew;
	REAL32 startTime;
	char conv[256];
	i3Particle * pParticle;
	static char s_szParticleName[][32] =
	{
		"Fountain%d",
		"Smoke%d",
		"Dust%d",
		"Snow%d",
		"Rain%d",
		"Splinter%d",
	};

	startTime = GetTotalDuration();

	pNew = i3TimeSequenceInfo::new_object_ref();

	pNew->SetStartTime( startTime);
	pNew->SetDuration( 1.0f);

	{
		pParticle = i3Particle::new_object_ref();

		sprintf( conv, s_szParticleName[type], m_pTimeSeq->GetSequenceCount());
		pParticle->SetName( conv);

		{
			i3ParticleInfo * pInfo;

			pInfo = i3ParticleInfo::new_object_ref();

			pParticle->SetParticleInfo( pInfo);

			switch( type)
			{
				case PARTICLE_FOUNTAIN :	SetParticleAsFountain( pInfo);	break;
				case PARTICLE_SMOKE :		SetParticleAsSmoke( pInfo);	break;
				case PARTICLE_DUST :		SetParticleAsDust( pInfo);	break;
				case PARTICLE_SNOW :		break;
				case PARTICLE_RAIN :		break;
				case PARTICLE_SPLINTERS :	break;
			}
		}

		pParticle->Create();

		pNew->SetNode( pParticle);
	}

	AddSequence( pNew);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pNew);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pParticle);
}

void CTimeTrackCtrl::OnNewSound(void)
{
	CFileDialog	Dlg( TRUE, "WAV", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Sound File(*.WAV)|*.wav||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	{
		i3Sound * pSound;
		i3SoundNode * pNode;
		i3SoundPlayInfo * pInfo;
		i3SoundFile file;
		char szRelPath[MAX_PATH];

		pSound = file.Load( LPCTSTR( Dlg.GetPathName()));
		if( pSound == NULL)
		{
			I3PRINTLOG(I3LOG_WARN, "%s Sound 파일을 읽을 수 없다. 잘 좀 골라바~~", LPCTSTR( Dlg.GetPathName()));
			return;
		}
		pSound->SetExternRes( TRUE);

		// Working Folder에서의 Relative Path로 조정한다.
		{
			i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR( Dlg.GetPathName()), szRelPath  );

			pSound->SetName( szRelPath);
		}

		pInfo = i3SoundPlayInfo::new_object_ref();
		pInfo->setSound( pSound);
		pInfo->SetStyle( I3SND_PROP_TYPE_EFFECT | I3SND_RT_3DMODE_WORLD_SPACE);
		pSound->Release();

		pNode = i3SoundNode::new_object_ref();
		pNode->setSoundPlayInfo( pInfo);
		pNode->SetName( szRelPath);

		{
			i3TimeSequenceInfo * pTimeInfo = i3TimeSequenceInfo::new_object_ref();

			pTimeInfo->SetNode( pNode);

			AddSequence( pTimeInfo);
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pTimeInfo);
		}

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pNode);
	}
}

BEGIN_MESSAGE_MAP(CTimeTrackCtrl, i3TDKTimeSeqCtrl)
END_MESSAGE_MAP()



// CTimeTrackCtrl message handlers

void CTimeTrackCtrl::OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx)
{
	if( idx != -1)
	{
		i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence( idx);
		i3Node * pNode = pInfo->GetNode();
		if( pInfo != NULL && i3::same_of<i3SoundNode*>(pNode)) //->IsExactTypeOf( i3SoundNode::static_meta()))
		{
			pPopup->AppendMenu( MF_STRING, ID_TIMETRACKMENU_SETTOSOUND,		"Edit Sound");
			return;
		}
	}

	{
		pPopup->AppendMenu( MF_STRING, ID_NEWPARTICLE_FOUNTAIN,		"New Particle (Fountain)");
		pPopup->AppendMenu( MF_STRING, ID_NEWPARTICLE_SMOKE,		"New Particle (Smoke)");
		pPopup->AppendMenu( MF_STRING, ID_NEWPARTICLE_DUST,			"New Particle (Dust)");
		pPopup->AppendMenu( MF_STRING, ID_NEWPARTICLE_SHOW,			"New Particle (Snow)");
		pPopup->AppendMenu( MF_STRING, ID_NEWPARTICLE_RAIN,			"New Particle (Rain)");
		pPopup->AppendMenu( MF_STRING, ID_NEWPARTICLE_SPLINTERS,	"New Particle (Splinters)");
		pPopup->AppendMenu( MF_SEPARATOR, 0, "-");
		pPopup->AppendMenu( MF_STRING, ID_TIMETRACKMENU_NEWSCENE,	"New Scene-Graph");
		pPopup->AppendMenu( MF_STRING, ID_TIMETRACKMENU_NEWSOUND,	"New Sound");
	}
}

void	CTimeTrackCtrl::OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx)
{
	switch( cmd)
	{
		case ID_NEWPARTICLE_FOUNTAIN :		OnNewParticle( PARTICLE_FOUNTAIN);	break;
		case ID_NEWPARTICLE_SMOKE :			OnNewParticle( PARTICLE_SMOKE);	break;
		case ID_NEWPARTICLE_DUST :			OnNewParticle( PARTICLE_DUST);	break;

		case ID_NEWPARTICLE_SHOW :
		case ID_NEWPARTICLE_RAIN :
		case ID_NEWPARTICLE_SPLINTERS :
			break;

		case ID_TIMETRACKMENU_NEWSCENE :
			break;

		case ID_TIMETRACKMENU_NEWSOUND :	OnNewSound();		break;
		case ID_TIMETRACKMENU_SETTOSOUND:	OnEditSound( idx);	break;
	}
}

void	CTimeTrackCtrl::OnChangeItemEnable( i3TimeSequenceInfo * pInfo)
{
	
}

void	CTimeTrackCtrl::OnEditSound( INT32 idx)
{
	if( idx == -1)
		return;

	i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence( idx);
	if( pInfo == NULL)
		return;

	i3Node * pNode = pInfo->GetNode();
	if( pNode == NULL)
		return;

	if( !i3::same_of<i3SoundNode*>(pNode)) //->IsExactTypeOf( i3SoundNode::static_meta()) == FALSE)
		return;

	i3SoundNode * pSndNode = (i3SoundNode*)pNode;

	CEditEffectSound SndDlg;
	SndDlg.SetObject( pSndNode);

	if( SndDlg.DoModal() == IDOK)
	{
		if( pSndNode->getSoundPlayInfo()->GetLoopMode() == I3SND_RT_LOOPMODE_ONESHOT)
		{
			pInfo->SetDuration( pSndNode->getSoundPlayInfo()->getSound()->GetDuration());
		}
		else
		{	
			pInfo->SetDuration( pSndNode->getLoopCount() * pSndNode->getSoundPlayInfo()->getSound()->GetDuration());
		}	
	}
}

INT32 CTimeTrackCtrl::FindParticle( i3Particle * pParticle)
{
	INT32 i;
	i3TimeSequenceInfo * pInfo;

	for( i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = (i3TimeSequenceInfo *) m_pTimeSeq->GetSequence( i);

		if( pInfo->GetNode() == pParticle)
			return i;
	}

	return i;
}


void CTimeTrackCtrl::OnChangeItemOrder(void)
{
	g_pMainWnd->ReorderSequence();
}
