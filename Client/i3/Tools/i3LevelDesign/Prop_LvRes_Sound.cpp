// Prop_LvRes_Sound.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvRes_Sound.h"

// CProp_LvRes_Sound dialog

IMPLEMENT_DYNAMIC(CProp_LvRes_Sound, i3TDKDialogBase)
CProp_LvRes_Sound::CProp_LvRes_Sound(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvRes_Sound::IDD, pParent)
{
	m_pResSound = NULL;
}

CProp_LvRes_Sound::~CProp_LvRes_Sound()
{
	I3_SAFE_RELEASE( m_pResSound);
}

void CProp_LvRes_Sound::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_PROP_LVRES_SOUND, m_listSoundInfo);
}


BEGIN_MESSAGE_MAP(CProp_LvRes_Sound, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CProp_LvRes_Sound message handlers
void CProp_LvRes_Sound::SetObject( i3ElementBase * pObj)
{
	if( (pObj != NULL) && !i3::same_of<i3LevelResSound*>(pObj)) //->IsExactTypeOf( i3LevelResSound::static_meta()) == FALSE)
	{
		I3TRACE( "pObj is not a Instance of i3LevelResSound\n");

		I3_SAFE_RELEASE( m_pResSound);
	}
	else
	{
		I3_REF_CHANGE( m_pResSound, (i3LevelResSound*) pObj);
	}

	_UpdateResInfo();
}

void CProp_LvRes_Sound::_UpdateResInfo( void)
{
	INT32	nIndex				= 0;
	char	conv[MAX_PATH];

	if( m_pResSound == NULL)
	{
		sprintf( conv, "%s", "");

		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);

		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
		m_listSoundInfo.SetItemText( nIndex++, 1, conv);

		return ;
	}

	sprintf( conv, "%s", m_pResSound->getPath());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "0x%08X", (unsigned int)m_pResSound->getHashCode());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	{
		UINT32 nResType = m_pResSound->getResType();
		switch( nResType)
		{
			case	I3LV_TYPE_PRIMITIVE:	sprintf( conv, "%s", "I3LV_TYPE_PRIMITIVE");	break;
			case	I3LV_TYPE_CHARA:		sprintf( conv, "%s", "I3LV_TYPE_CHARA");		break;
			case	I3LV_TYPE_WORLD:		sprintf( conv, "%s", "I3LV_TYPE_WORLD");		break;
			case	I3LV_TYPE_OBJECT:		sprintf( conv, "%s", "I3LV_TYPE_OBJECT");		break;
			case	I3LV_TYPE_EFFECT:		sprintf( conv, "%s", "I3LV_TYPE_EFFECT");		break;
			case	I3LV_TYPE_LIGHT:		sprintf( conv, "%s", "I3LV_TYPE_LIGHT");		break;
			case	I3LV_TYPE_CAMERA:		sprintf( conv, "%s", "I3LV_TYPE_CAMERA");		break;
			case	I3LV_TYPE_TEXTURE:		sprintf( conv, "%s", "I3LV_TYPE_TEXTURE");		break;
			case	I3LV_TYPE_ANIMATION:	sprintf( conv, "%s", "I3LV_TYLE_ANIMATION");	break;
			case	I3LV_TYPE_SOUND:		sprintf( conv, "%s", "I3LV_TYPE_SOUND");		break;
			case	I3LV_TYPE_SCENEGRAPH:	sprintf( conv, "%s", "I3LV_TYPE_SCENEGRAPH");	break;
			default:
				sprintf( conv, "%s", "Unknown Type");	break;

		}

		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
	}

	sprintf( conv, "%08X", m_pResSound->getResState());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%08X", m_pResSound->getFormat());
	{
		UINT32 nFormat = m_pResSound->getFormat();
		switch( nFormat)
		{
			case I3SND_CMN_FORMAT_WAV			:sprintf( conv, "%s", "I3SND_CMN_FORMAT_WAV");		break;
			case I3SND_CMN_FORMAT_VAG			:sprintf( conv, "%s", "I3SND_CMN_FORMAT_VAG");		break;
			case I3SND_CMN_FORMAT_I3SND			:sprintf( conv, "%s", "I3SND_CMN_FORMAT_I3SND");	break;
			case I3SND_CMN_FORMAT_MP3			:sprintf( conv, "%s", "I3SND_CMN_FORMAT_MP3");		break;
			case I3SND_CMN_FORMAT_OGG			:sprintf( conv, "%s", "I3SND_CMN_FORMAT_OGG");		break;
			case I3SND_CMN_FORMAT_UNKNOWN		:sprintf( conv, "%s", "I3SND_CMN_FORMAT_UNKNOWN");	break;
			default:
				sprintf( conv, "%s", "Unknown Format");	break;
		}

		m_listSoundInfo.SetItemText( nIndex++, 1, conv);
	}

	sprintf( conv, "%d ", m_pResSound->getChannels());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResSound->getSampleRate());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResSound->getBitsPerSample());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResSound->getDataSize());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResSound->getBlockAlign());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResSound->getAvgBytesPerSec());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%08f", m_pResSound->getDuration());
	m_listSoundInfo.SetItemText( nIndex++, 1, conv);
}

BOOL CProp_LvRes_Sound::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	m_listSoundInfo.InsertColumn( 0, "Content", 0, 128);
	m_listSoundInfo.InsertColumn( 1, "Value", 0, 512);

	// TODO:  Add extra initialization here
	INT32	nIndex = 0;

	nIndex = m_listSoundInfo.InsertItem( nIndex,	 "파일 경로",		0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "해쉬 코드",		0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "리소스 종류",		0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "리소스 상태",		0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "데이터 포맷",		0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "채널수",			0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "초당 샘플링횟수", 0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "샘플당 비트 수",0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "데이터 크기",		0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "블럭당 바이트수", 0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "초당 바이트수",	0);
	nIndex = m_listSoundInfo.InsertItem( nIndex + 1, "총 재생시간(초)",		0);

	_UpdateResInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvRes_Sound::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_listSoundInfo.m_hWnd))
	{
		m_listSoundInfo.SetWindowPos( NULL, 4, 4, cx - 8, cy - 8, SWP_NOACTIVATE | SWP_NOZORDER);
		m_listSoundInfo.SetColumnWidth( 1, cx - 8 - 128);
	}
}

int CProp_LvRes_Sound::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	RECT rt;
	rt.left = rt.top = 4;
	rt.right = rt.bottom = 768;

	m_listSoundInfo.Create( 0, rt, this, 101);

	return 0;
}
