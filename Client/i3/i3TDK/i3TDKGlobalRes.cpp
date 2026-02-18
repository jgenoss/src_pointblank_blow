#include "stdafx.h"
#include "i3TDKGlobalRes.h"
#include "i3TDKMeta.h"
#include "i3TDKUtilGDI.h"
#include "resource.h"
#include "i3TDKGlobalVariable.h"

#define		SAFE_LOADBMP( a, id)	\
	{\
		(a) = ::LoadBitmap( g_hInstTDK, MAKEINTRESOURCE( id));\
		I3ASSERT( (a) != nullptr); \
	}

//I3_CLASS_INSTANCE( i3TDKGlobalRes, i3ElementBase);
I3_CLASS_INSTANCE( i3TDKGlobalRes);

static void addIcon( CImageList * pImageList, INT32 id)
{
	HICON hIcon;

	hIcon = ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));
	pImageList->Add( hIcon);
	::DestroyIcon( hIcon);
}

i3TDKGlobalRes::i3TDKGlobalRes(void)
{
	g_pTDKGlobalRes = this;

	SAFE_LOADBMP( m_hbmpCaptionBack,			IDB_CAPTION_BACK);
	SAFE_LOADBMP( m_hbmpCaptionBackLeft,		IDB_CAPTION_BACK_LEFT);
	SAFE_LOADBMP( m_hbmpCaptionBackRight,		IDB_CAPTION_BACK_RIGHT);
	SAFE_LOADBMP( m_hbmpCaptionGrip,			IDB_CAPTION_GRIP);
	SAFE_LOADBMP( m_hbmpCaptionClose,			IDB_CAPTION_CLOSE);
	SAFE_LOADBMP( m_hbmpCaptionCollapse,		IDB_CAPTION_COLLAPSE);
	SAFE_LOADBMP( m_hbmpCaptionExpand,			IDB_CAPTION_EXPAND);

	SAFE_LOADBMP( m_hbmpSelCaptionBack,			IDB_SELECTED_CAPTION_BACK);
	SAFE_LOADBMP( m_hbmpSelCaptionBackLeft,		IDB_SELECTED_CAPTION_BACK_LEFT);
	SAFE_LOADBMP( m_hbmpSelCaptionBackRight,	IDB_SELECTED_CAPTION_BACK_RIGHT);
	SAFE_LOADBMP( m_hbmpSelCaptionGrip,			IDB_SELECTED_CAPTION_GRIP);
	SAFE_LOADBMP( m_hbmpSelCaptionClose,		IDB_SELECTED_CAPTION_CLOSE);
	SAFE_LOADBMP( m_hbmpSelCaptionCollapse,		IDB_SELECTED_CAPTION_COLLAPSE);
	SAFE_LOADBMP( m_hbmpSelCaptionExpand,		IDB_SELECTED_CAPTION_EXPAND);

	SAFE_LOADBMP( m_hbmpFloatGrip,				IDB_FLOAT_GRIP);

	SAFE_LOADBMP( m_hbmpTabBackLeft,			IDB_TAB_BACK_LEFT);
	SAFE_LOADBMP( m_hbmpTabBackMid,				IDB_TAB_BACK_MID);
	SAFE_LOADBMP( m_hbmpTabBackRight,			IDB_TAB_BACK_RIGHT);

	SAFE_LOADBMP( m_hbmpTabForeLeft,			IDB_TAB_SEL_LEFT);
	SAFE_LOADBMP( m_hbmpTabForeMid,				IDB_TAB_SEL_MID);
	SAFE_LOADBMP( m_hbmpTabForeRight,			IDB_TAB_SEL_RIGHT);

	SAFE_LOADBMP( m_hbmpEditBack,				IDB_EDIT_BACK);

	InitSysColors();

	// Cursor
	{
		m_hCursorArrow = ::LoadCursor( nullptr, IDC_ARROW);

		m_hCursorHand = ::LoadCursor( nullptr, IDC_HAND);

		m_hCursorNoDrop = ::LoadCursor( nullptr, IDC_NO);

		m_hCursorSizeNS = ::LoadCursor( nullptr, IDC_SIZENS);

		m_hCursorSizeWE = ::LoadCursor( nullptr, IDC_SIZEWE);
	}

	// Image List
	{
		m_pTDKImageList = new CImageList;
		m_pTDKImageList->Create( 16, 16, ILC_COLOR32 | ILC_MASK, 32, 0);

		addIcon( m_pTDKImageList, IDI_ANIM);
		addIcon( m_pTDKImageList, IDI_ANIM_UNSEL);
		addIcon( m_pTDKImageList, IDI_ATTR);
		addIcon( m_pTDKImageList, IDI_ATTR_UNSEL);
		addIcon( m_pTDKImageList, IDI_BONE);
		addIcon( m_pTDKImageList, IDI_BONE_UNSEL);
		addIcon( m_pTDKImageList, IDI_CAMERA);
		addIcon( m_pTDKImageList, IDI_CAMERA_UNSEL);
		addIcon( m_pTDKImageList, IDI_GEOMETRY);
		addIcon( m_pTDKImageList, IDI_GEOMETRY_UNSEL);
		addIcon( m_pTDKImageList, IDI_NODE);
		addIcon( m_pTDKImageList, IDI_NODE_UNSEL);
		addIcon( m_pTDKImageList, IDI_RESOURCE);
		addIcon( m_pTDKImageList, IDI_RESOURCE_UNSEL);
		addIcon( m_pTDKImageList, IDI_SOUND);
		addIcon( m_pTDKImageList, IDI_SOUND_UNSEL);
		addIcon( m_pTDKImageList, IDI_TEXTURE);
		addIcon( m_pTDKImageList, IDI_TEXTURE_UNSEL);
		addIcon( m_pTDKImageList, IDI_BLANK);
	}
}

i3TDKGlobalRes::~i3TDKGlobalRes(void)
{
	if( m_pTDKImageList != nullptr)
	{
		delete m_pTDKImageList;
		m_pTDKImageList = nullptr;
	}
}

void i3TDKGlobalRes::_initColor( I3TDK_SYS_COLOR index, COLORREF color)
{
	m_SysColor[ index].m_Color = color;
	m_SysColor[ index].m_hPen = CreatePen( PS_SOLID, 1, color);
	m_SysColor[ index].m_hBrush = CreateSolidBrush( color);
}

void i3TDKGlobalRes::InitSysColors(void)
{
	_initColor(	I3TDK_COLOR_BACK_APP_BKGND,			RGB( 70, 70, 70));
	_initColor(	I3TDK_COLOR_FORE_APP_BKGND,			RGB( 105, 105, 105));

	_initColor(	I3TDK_COLOR_BACK_BORDER,			RGB( 56, 56, 56));
	_initColor(	I3TDK_COLOR_FORE_BORDER,			RGB( 74, 74, 74));

	_initColor(	I3TDK_COLOR_BACK_CAPTION_TEXT,		RGB( 34, 34, 34));
	_initColor(	I3TDK_COLOR_FORE_CAPTION_TEXT,		RGB( 219, 219, 219));

	_initColor(	I3TDK_COLOR_TEXT_NORMAL,			RGB( 208, 208, 208));
	_initColor(	I3TDK_COLOR_TEXT_HILIGHT,			RGB( 253, 159, 2));
	_initColor(	I3TDK_COLOR_TEXT_DISABLE,			RGB( 128, 128, 128));

	_initColor(	I3TDK_COLOR_BACK_3D_LIT,			RGB( 104, 104, 104));
	_initColor(	I3TDK_COLOR_BACK_3D_LIT_DARK,		RGB( 91, 91, 91));
	_initColor(	I3TDK_COLOR_BACK_3D_FACE,			RGB( 78, 78, 78));
	_initColor(	I3TDK_COLOR_BACK_3D_SHADOW,			RGB( 63, 63, 63));
	_initColor(	I3TDK_COLOR_BACK_3D_SHADOW_DARK,	RGB( 46, 46, 46));

	_initColor(	I3TDK_COLOR_FORE_3D_LIT,			RGB( 138, 138, 138));
	_initColor(	I3TDK_COLOR_FORE_3D_LIT_DARK,		RGB( 121, 121, 121));
	_initColor(	I3TDK_COLOR_FORE_3D_FACE,			RGB( 105, 105, 105));
	_initColor(	I3TDK_COLOR_FORE_3D_SHADOW,			RGB( 75, 75, 75));
	_initColor(	I3TDK_COLOR_FORE_3D_SHADOW_DARK,	RGB( 48, 48, 48));

	_initColor(	I3TDK_COLOR_FLOAT_BORDER,			RGB( 0, 0, 0));
	_initColor( I3TDK_COLOR_FLOAT_BORDER_FACE,		RGB( 148, 148, 148));

	_initColor(	I3TDK_COLOR_BACK_WHITE,				RGB( 255, 255, 255));
	_initColor( I3TDK_COLOR_TEXT_BLACK,				RGB( 0, 0, 0));
}

BOOL i3TDKGlobalRes::Init(void)
{
	g_pTDKGlobalRes = i3TDKGlobalRes::new_object();

	return TRUE;
}

BOOL i3TDKGlobalRes::Close(void)
{
	I3_SAFE_RELEASE( g_pTDKGlobalRes);

	return TRUE;
}

HICON	i3TDK::LoadIcon( UINT32 id)
{
	HICON hIcon;

	SetResInstance();

	hIcon = ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));

	RestoreResInstance();

	return hIcon;
}

static			HINSTANCE				s_hInstOld = nullptr;

void i3TDK::SetResInstance(void)
{
	HINSTANCE hCur = AfxGetResourceHandle();
	
	s_hInstOld = nullptr;

	if( hCur == g_hInstTDK)
		return;

	{
		s_hInstOld = hCur;

		AfxSetResourceHandle( g_hInstTDK);
	}
}

void i3TDK::RestoreResInstance(void)
{
	if( s_hInstOld != nullptr)
		AfxSetResourceHandle( s_hInstOld);
}
