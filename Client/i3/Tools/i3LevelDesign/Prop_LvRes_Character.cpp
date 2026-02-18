// Prop_LvRes_Character.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvRes_Character.h"
#include ".\prop_lvres_character.h"

// CProp_LvRes_Character dialog

IMPLEMENT_DYNAMIC(CProp_LvRes_Character, i3TDKDialogBase)
CProp_LvRes_Character::CProp_LvRes_Character(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvRes_Character::IDD, pParent)
{
	m_pResCharacter = NULL;
}

CProp_LvRes_Character::~CProp_LvRes_Character()
{
	I3_SAFE_RELEASE( m_pResCharacter);
}

void CProp_LvRes_Character::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_PROP_LVRES_CHARACTER, m_listCharaInfo);
}


BEGIN_MESSAGE_MAP(CProp_LvRes_Character, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CProp_LvRes_Character message handlers
void CProp_LvRes_Character::SetObject( i3ElementBase * pObj)
{
	if(( pObj != NULL) && !i3::same_of<i3LevelResChara*>(pObj)) //->IsExactTypeOf( i3LevelResChara::static_meta()) == FALSE)
	{
		I3TRACE("pObj is not a Instance of i3LevelResChara\n");

		I3_SAFE_RELEASE( m_pResCharacter);
	}
	else
	{
		I3_REF_CHANGE( m_pResCharacter, (i3LevelResChara *)pObj);
	}

	_UpdateResInfo();
}

void CProp_LvRes_Character::_UpdateResInfo( void)
{
	INT32	nIndex				= 0;
	char	conv[MAX_PATH];

	if( m_pResCharacter == NULL)
	{
		sprintf( conv, "%s", "");
		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
		return;
	}

	sprintf( conv, "%s", m_pResCharacter->getPath());
	m_listCharaInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "0x%08X", (unsigned int)m_pResCharacter->getHashCode());
	m_listCharaInfo.SetItemText( nIndex++, 1, conv);

	{
		UINT32 nResType = m_pResCharacter->getResType();
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

		m_listCharaInfo.SetItemText( nIndex++, 1, conv);
	}

	sprintf( conv, "%08X", m_pResCharacter->getResState());
	m_listCharaInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResCharacter->getLODCount());
	m_listCharaInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResCharacter->getAIStateCount());
	m_listCharaInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResCharacter->getAnimCount());
	m_listCharaInfo.SetItemText( nIndex++, 1, conv);
}

BOOL CProp_LvRes_Character::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	INT32	nIndex = 0;

	// TODO:  Add extra initialization here
	m_listCharaInfo.InsertColumn( 0, "Content", 0, 128);
	m_listCharaInfo.InsertColumn( 1, "Value", 0, 512);

	nIndex = m_listCharaInfo.InsertItem( nIndex, "파일 경로", 0);
	nIndex = m_listCharaInfo.InsertItem( nIndex + 1, "해쉬 코드", 0);
	nIndex = m_listCharaInfo.InsertItem( nIndex + 1, "리소스 종류", 0);
	nIndex = m_listCharaInfo.InsertItem( nIndex + 1, "리소스 상태", 0);
	nIndex = m_listCharaInfo.InsertItem( nIndex + 1, "LOD 갯수", 0);
	nIndex = m_listCharaInfo.InsertItem( nIndex + 1, "AIState 갯수", 0);
	nIndex = m_listCharaInfo.InsertItem( nIndex + 1, "Animation 갯수", 0);

	_UpdateResInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvRes_Character::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_listCharaInfo.m_hWnd))
	{
		m_listCharaInfo.SetWindowPos( NULL, 4, 4, cx - 8, cy - 8, SWP_NOACTIVATE | SWP_NOZORDER);
		m_listCharaInfo.SetColumnWidth( 1, cx - 8 - 128);
	}
}

int CProp_LvRes_Character::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	RECT rt;
	rt.left = rt.top = 4;
	rt.right = rt.bottom = 768;

	m_listCharaInfo.Create( 0, rt, this, 101);
	return 0;
}
