// Prop_LvRes_Texture.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvRes_Texture.h"

// CProp_LvRes_Texture dialog

IMPLEMENT_DYNAMIC(CProp_LvRes_Texture, i3TDKDialogBase)
CProp_LvRes_Texture::CProp_LvRes_Texture(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvRes_Texture::IDD, pParent)
{
	m_pLvResTexture = NULL;
}

CProp_LvRes_Texture::~CProp_LvRes_Texture()
{
	I3_SAFE_RELEASE( m_pLvResTexture);
}

void CProp_LvRes_Texture::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_PROP_LVRES_TEXTURE, m_listTextureInfo);
}


BEGIN_MESSAGE_MAP(CProp_LvRes_Texture, i3TDKDialogBase)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CProp_LvRes_Texture message handlers
void CProp_LvRes_Texture::SetObject( i3ElementBase * pObj)
{
	if( !i3::same_of<i3LevelResTexture*>(pObj)) //->IsExactTypeOf( i3LevelResTexture::static_meta()) == FALSE)
	{
		I3TRACE( "pObj is not i3LevelResTexture's Instance\n");

		I3_SAFE_RELEASE( m_pLvResTexture);
	}
	else
	{
		I3_REF_CHANGE( m_pLvResTexture, (i3LevelResTexture *)pObj);
	}

	_UpdateResInfo();
}

void CProp_LvRes_Texture::_UpdateResInfo( void)
{
	INT32	nIndex				= 0;
	char	conv[MAX_PATH];

	if( m_pLvResTexture == NULL)
	{
		sprintf( conv, "%s", "");
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
		m_listTextureInfo.SetItemText( nIndex++, 1, conv);

		return ;
	}

	sprintf( conv, "%s", m_pLvResTexture->getPath());
	m_listTextureInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "0x%08X", (unsigned int)m_pLvResTexture->getHashCode());		// 당췌 어떤걸 출력하는지 모르겠지만..일단 실제로 포인터주소를 하는 것으로 간주해봄..
	m_listTextureInfo.SetItemText( nIndex++, 1, conv);

	{
		UINT32 nResType = m_pLvResTexture->getResType();
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
			default:	sprintf( conv, "%s", "Unknown Type");	break;
		}

		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
	}

	sprintf( conv, "%08X", m_pLvResTexture->getResState());
	m_listTextureInfo.SetItemText( nIndex++, 1, conv);

	{
		I3G_IMAGE_FORMAT	nFormat = m_pLvResTexture->getFormat();
		switch( nFormat)
		{
			case I3G_IMAGE_FORMAT_NONE:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_NONE");	break;
			case I3G_IMAGE_FORMAT_DEFAULT_COLOR:	sprintf( conv, "%s", "I3G_IMAGE_FORMAT_DEFAULT_COLOR");	break;

			// 4~8 bit format
			case I3G_IMAGE_FORMAT_I4:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_I4");	break;
			case I3G_IMAGE_FORMAT_I8:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_I8");	break;
			case i3G_IMAGE_FORMAT_L8:				sprintf( conv, "%s", "i3G_IMAGE_FORMAT_L8");	break;
			case i3G_IMAGE_FORMAT_A8:				sprintf( conv, "%s", "i3G_IMAGE_FORMAT_A8");	break;

			// 16 bit formats
			case I3G_IMAGE_FORMAT_RGB_565:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_RGB_565");	break;
			case I3G_IMAGE_FORMAT_BGR_565:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_BGR_565");	break;
			case I3G_IMAGE_FORMAT_XRGB_1555:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_XRGB_1555");	break;
			case I3G_IMAGE_FORMAT_ARGB_1555:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_ARGB_1555");	break;
			case I3G_IMAGE_FORMAT_XBGR_1555:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_XBGR_1555");	break;
			case I3G_IMAGE_FORMAT_ABGR_1555:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_ABGR_1555");	break;
			case I3G_IMAGE_FORMAT_XRGB_4444:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_XRGB_4444");	break;
			case I3G_IMAGE_FORMAT_ARGB_4444:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_ARGB_4444");	break;
			case I3G_IMAGE_FORMAT_XBGR_4444:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_XBGR_4444");	break;
			case I3G_IMAGE_FORMAT_ABGR_4444:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_ABGR_4444");	break;

			// 24 bit formats
			case I3G_IMAGE_FORMAT_RGB_888:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_RGB_888");	break;
			case I3G_IMAGE_FORMAT_BGR_888:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_BGR_888");	break;

			// 32 bit formats
			case I3G_IMAGE_FORMAT_RGBX_8888:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_RGBX_8888");	break;
			case I3G_IMAGE_FORMAT_BGRX_8888:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_BGRX_8888");	break;
			case I3G_IMAGE_FORMAT_RGBA_8888:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_RGBA_8888");	break;
			case I3G_IMAGE_FORMAT_BGRA_8888:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_BGRA_8888");	break;

			// -------------------------
			// Bump-map
			// -------------------------
			case I3G_IMAGE_FORMAT_VU_88:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_VU_88");	break;
			case I3G_IMAGE_FORMAT_QWVU_8888:		sprintf( conv, "%s", "I3G_IMAGE_FORMAT_QWVU_8888");	break;

			// -------------------------
			// Compressed formats
			// -------------------------
			case I3G_IMAGE_FORMAT_DXT1_X:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_DXT1_X");	break;
			case I3G_IMAGE_FORMAT_DXT1:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_DXT1");	break;
			case I3G_IMAGE_FORMAT_DXT2:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_DXT2");	break;
			case I3G_IMAGE_FORMAT_DXT3:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_DXT3");	break;
			case I3G_IMAGE_FORMAT_DXT4:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_DXT4");	break;
			case I3G_IMAGE_FORMAT_DXT5:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_DXT5");	break;

			// -------------------------
			// HDR
			// -------------------------
			case I3G_IMAGE_FORMAT_ABGR_16F:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_ABGR_16F");	break;
			case I3G_IMAGE_FORMAT_ABGR_32F:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_ABGR_32F");	break;
			case I3G_IMAGE_FORMAT_ABGR_16:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_ABGR_16");	break;
			case I3G_IMAGE_FORMAT_GR_16F:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_GR_16F");	break;
			case I3G_IMAGE_FORMAT_R_32F:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_R_32F");	break;
			case I3G_IMAGE_FORMAT_GR_32F:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_GR_32F");	break;

			// -------------------------
			// depth, stencil, vertex buffers
			// -------------------------
			case I3G_IMAGE_FORMAT_D16:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_D16");	break;
			case I3G_IMAGE_FORMAT_D24S8:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_D24S8");	break;
			case I3G_IMAGE_FORMAT_D24X8:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_D24X8");	break;
			case I3G_IMAGE_FORMAT_D32:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_D32");	break;
			case I3G_IMAGE_FORMAT_D32F:				sprintf( conv, "%s", "I3G_IMAGE_FORMAT_D32F");	break;
			case I3G_IMAGE_FORMAT_D24FS8:			sprintf( conv, "%s", "I3G_IMAGE_FORMAT_D24FS8");	break;

			default:
				sprintf( conv, "%s", "Unknown format");	break;
		}

		m_listTextureInfo.SetItemText( nIndex++, 1, conv);
	}

	sprintf( conv, "%d", m_pLvResTexture->getWidth());
	m_listTextureInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pLvResTexture->getHeight());
	m_listTextureInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pLvResTexture->getMipmapCount());
	m_listTextureInfo.SetItemText( nIndex++, 1, conv);

	//그냥 paint를하게되면 list의 업데이트 되지 않는 부분은 바탕으로 덮여쓰여지게됩니다. 따라서 Thumbnail크기만큼만 무효화
	CRect rt( 4, 4, 68, 68);
	InvalidateRect( &rt);
}

void CProp_LvRes_Texture::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if( m_pLvResTexture == NULL)
	{
		HBRUSH brush		= (HBRUSH)::GetStockObject( WHITE_BRUSH);
		HBRUSH hOldBrush	= (HBRUSH)::SelectObject( dc.m_hDC, brush);

		::Rectangle( dc.m_hDC, 4, 4, 68, 68);

		::SelectObject( dc.m_hDC, hOldBrush);

		return;
	}

	i3TDK::StretchBlt( dc.m_hDC, 4, 4, m_pLvResTexture->getIconWidth(),
									m_pLvResTexture->getIconHeight(),
									m_pLvResTexture->getBitmap(),
									0,0,
									m_pLvResTexture->getIconWidth(),
									m_pLvResTexture->getIconHeight());
}

void CProp_LvRes_Texture::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_listTextureInfo.m_hWnd))
	{
		CRect rt;
		rt.SetRect( 0, 
					64 + 8, 
					cx - 4,
					cy - 64 - 8 - 4);

		m_listTextureInfo.SetWindowPos( NULL, rt.left, rt.top, rt.right, rt.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
		m_listTextureInfo.SetColumnWidth( 1, cx - 4 - 128);
	}
}

BOOL CProp_LvRes_Texture::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	m_listTextureInfo.InsertColumn( 0, "Content", 0, 128);
	m_listTextureInfo.InsertColumn( 1, "Value", 0, 512);

	// TODO:  Add extra initialization here
	INT32	nIndex = 0;

	nIndex = m_listTextureInfo.InsertItem( nIndex	 , "파일 경로",			0);
	nIndex = m_listTextureInfo.InsertItem( nIndex + 1, "해쉬 코드", 0);
	nIndex = m_listTextureInfo.InsertItem( nIndex + 1, "리소스 타입", 0);
	nIndex = m_listTextureInfo.InsertItem( nIndex + 1, "리소스 상태",0);
	nIndex = m_listTextureInfo.InsertItem( nIndex + 1, "Format",		0);
	nIndex = m_listTextureInfo.InsertItem( nIndex + 1, "Width",			0);
	nIndex = m_listTextureInfo.InsertItem( nIndex + 1, "Height",		0);
	nIndex = m_listTextureInfo.InsertItem( nIndex + 1, "Mipmap Count",	0);

	_UpdateResInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CProp_LvRes_Texture::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	RECT rt;
	rt.left = rt.top = 4;
	rt.right = rt.bottom = 768;

	m_listTextureInfo.Create( 0, rt, this, 101);
	return 0;
}
