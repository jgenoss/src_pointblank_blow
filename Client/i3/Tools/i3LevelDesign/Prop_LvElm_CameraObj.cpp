
// Prop_LvElm_CameraObj.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_CameraObj.h"
#include "MainFrm.h"

struct I3LV_IMAGE_FORMAT_INFO
{
	I3G_IMAGE_FORMAT fmt;
	char			szFmtString[64];
};

static I3LV_IMAGE_FORMAT_INFO s_ImgFmtInfo[] = 
{
	{I3G_IMAGE_FORMAT_I4,  			"IDX4"},

	// 8bit formats
	{ I3G_IMAGE_FORMAT_I8,  		"IDX8"},
	{ i3G_IMAGE_FORMAT_L8,  		"LUX8"},
	{ i3G_IMAGE_FORMAT_A8,  		"ALPHA8"},

	// 16 bit formats
	{ I3G_IMAGE_FORMAT_RGB_565,  	"RGB16_565"},
	{ I3G_IMAGE_FORMAT_XRGB_1555,  	"XRGB16_1555"},
	{ I3G_IMAGE_FORMAT_ARGB_1555,  	"ARGB16_1555"},
	{ I3G_IMAGE_FORMAT_ABGR_4444,  	"RGBA16_4444"},
	{ I3G_IMAGE_FORMAT_XBGR_4444,  	"RGBX16_4444"},
	{ I3G_IMAGE_FORMAT_XRGB_4444,  	"XRGB16_4444"},
	{ I3G_IMAGE_FORMAT_ARGB_4444,  	"ARGB16_4444"},

	// 24 bit formats
	{ I3G_IMAGE_FORMAT_RGB_888,  	"RGB24_888"},
	{ I3G_IMAGE_FORMAT_BGR_888,  	"BGR24_888"},

	// 32 bit format
	{ I3G_IMAGE_FORMAT_RGBX_8888,  	"RGBX32_8888"},
	{ I3G_IMAGE_FORMAT_BGRX_8888,  	"XRGB32_8888"},
	{ I3G_IMAGE_FORMAT_RGBA_8888,  	"RGBA32_8888"},
	{ I3G_IMAGE_FORMAT_BGRA_8888,  	"ARGB32_8888"},

	// -------------------------
	// Bump-map
	// -------------------------
	{ I3G_IMAGE_FORMAT_VU_88,  		"VU_88"},
	{ I3G_IMAGE_FORMAT_QWVU_8888,  	"QWVU_8888"},

	// -------------------------
	// Compressed formats
	// -------------------------
	{ I3G_IMAGE_FORMAT_DXT1_X,  	"DXT1_X"},
	{ I3G_IMAGE_FORMAT_DXT1,  		"DXT1"},
	{ I3G_IMAGE_FORMAT_DXT2,  		"DXT2"},
	{ I3G_IMAGE_FORMAT_DXT3,  		"DXT3"},
	{ I3G_IMAGE_FORMAT_DXT4,  		"DXT4"},
	{ I3G_IMAGE_FORMAT_DXT5,  		"DXT5"},

	// -------------------------
	// HDR
	// -------------------------
	{ I3G_IMAGE_FORMAT_ABGR_16F ,  	"RGBA64_16F"},
	{ I3G_IMAGE_FORMAT_ABGR_32F ,  	"RGBA128_32F"},
	{ I3G_IMAGE_FORMAT_ABGR_16 ,  	"RGBA64_16"},
	{ I3G_IMAGE_FORMAT_GR_16F ,  	"GR32_16F"},
	{ I3G_IMAGE_FORMAT_R_32F ,  	"R32_32F"},
	{ I3G_IMAGE_FORMAT_GR_32F ,  	"GR64_32F"},

	// -------------------------
	// depth, stencil, vertex buffers
	// -------------------------
	{ I3G_IMAGE_FORMAT_D16,  		"D16"},

	{ I3G_IMAGE_FORMAT_D24S8,  		"D24S8"},
	{ I3G_IMAGE_FORMAT_D24X8,  		"D24X8"},
	{ I3G_IMAGE_FORMAT_D32F,  		"D32F"},
	{ I3G_IMAGE_FORMAT_D24FS8 ,  	"D24FS8"},
	{ I3G_IMAGE_FORMAT_D32,  		"D32"},
};

INT32 getImageFormatCount(void)
{
	return sizeof( s_ImgFmtInfo) / sizeof(I3LV_IMAGE_FORMAT_INFO);
}

INT32 getImageFormatStringList( i3::vector<i3::rc_string>& List)
{
	

	for( INT32 i = 0;i < getImageFormatCount(); ++i)
	{
		List.push_back( s_ImgFmtInfo[i].szFmtString);
	}

	return (INT32)List.size();
}

INT32 getFormatIndex( I3G_IMAGE_FORMAT fmt)
{
	for( INT32 i = 0;i < getImageFormatCount(); ++i)
	{
		if( s_ImgFmtInfo[i].fmt == fmt)
			return i;
	}

	return -1;
}

I3G_IMAGE_FORMAT getFormatByIdx( INT32 idx)
{
	if( idx > -1 && getImageFormatCount() > idx)
	{
		return s_ImgFmtInfo[idx].fmt;
	}
	else
		return I3G_IMAGE_FORMAT_NONE;
}

// CProp_LvElm_CameraObj dialog

IMPLEMENT_DYNAMIC(CProp_LvElm_CameraObj, i3TDKDialogBase)
CProp_LvElm_CameraObj::CProp_LvElm_CameraObj(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_CameraObj::IDD, pParent)
{
	m_pCamera = NULL;
}

CProp_LvElm_CameraObj::~CProp_LvElm_CameraObj()
{
}

void CProp_LvElm_CameraObj::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_CAMERA_FOV, m_edFOV);
	DDX_Control(pDX, IDC_ED_CAMERA_NEAR, m_edNear);
	DDX_Control(pDX, IDC_ED_CAMERA_FAR, m_edFar);
	DDX_Control(pDX, IDC_ED_CAMERA_WIDTH, m_edWidth);
	DDX_Control(pDX, IDC_ED_CAMERA_HEIGHT, m_edHeight);
	DDX_Control(pDX, IDC_CB_CAMERA_MODE, m_cbMode);
	DDX_Control(pDX, IDC_CHK_CAMERA_ENABLE, m_chkEnable);
	DDX_Control(pDX, IDC_ED_CAMERA_UPDATEINTERVAL, m_edUpdateInterval);
	DDX_Control(pDX, IDC_CB_CAMERA_RT_WIDTH, m_cbCamRT_Width);
	DDX_Control(pDX, IDC_CB_CAMERA_RT_HEIGHT, m_cbCamRT_Height);
	DDX_Control(pDX, IDC_CB_CAMERA_RT_COLOR_FORMAT, m_cbCamRT_Color_Format);
	DDX_Control(pDX, IDC_CB_CAMERA_RT_DEPTH_FORMAT, m_cbCamRT_Depth_Format);
}


BEGIN_MESSAGE_MAP(CProp_LvElm_CameraObj, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_ED_CAMERA_FOV, OnEnChangeEdCameraFov)
	ON_EN_KILLFOCUS(IDC_ED_CAMERA_NEAR, OnEnChangeEdCameraNear)
	ON_EN_KILLFOCUS(IDC_ED_CAMERA_FAR, OnEnChangeEdCameraFar)
	ON_EN_KILLFOCUS(IDC_ED_CAMERA_WIDTH, OnEnChangeEdCameraWidth)
	ON_EN_KILLFOCUS(IDC_ED_CAMERA_HEIGHT, OnEnChangeEdCameraHeight)
	ON_CBN_SELCHANGE(IDC_CB_CAMERA_MODE, OnCbnSelchangeCbCameraMode)
	ON_BN_CLICKED(IDC_CHK_CAMERA_ENABLE, OnBnClickedChkCameraEnable)
	ON_EN_KILLFOCUS(IDC_ED_CAMERA_UPDATEINTERVAL, OnEnChangeEdCameraUpdateinterval)
	ON_CBN_SELCHANGE(IDC_CB_CAMERA_RT_WIDTH, OnCbnSelchangeCbCameraRtWidth)
	ON_CBN_SELCHANGE(IDC_CB_CAMERA_RT_HEIGHT, OnCbnSelchangeCbCameraRtHeight)
	ON_CBN_SELCHANGE(IDC_CB_CAMERA_RT_COLOR_FORMAT, OnCbnSelchangeCbCameraRtColorFormat)
	ON_CBN_SELCHANGE(IDC_CB_CAMERA_RT_DEPTH_FORMAT, OnCbnSelchangeCbCameraRtDepthFormat)
END_MESSAGE_MAP()


// CProp_LvElm_CameraObj message handlers

void CProp_LvElm_CameraObj::OnOK()
{
}

void CProp_LvElm_CameraObj::OnCancel()
{
}

BOOL CProp_LvElm_CameraObj::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	char	conv[MAX_PATH];
	INT32	size = 32;
	for( INT32 i = 0; i< 6; ++i)
	{
		sprintf( conv, "%d", size);
		m_cbCamRT_Width.AddString( conv) ;
		m_cbCamRT_Height.AddString( conv);

		size *= 2;
	}

	i3::vector<i3::rc_string> fmtString;

	INT32 cnt = getImageFormatStringList( fmtString);
	for( INT32 i = 0;i < cnt; ++i)
	{
		m_cbCamRT_Color_Format.AddString( fmtString[i].c_str());
		m_cbCamRT_Depth_Format.AddString( fmtString[i].c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvElm_CameraObj::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	i3LevelCameraObject * pLvCamObj = (i3LevelCameraObject *)getObject();
	if( pLvCamObj != NULL)
	{
		i3CameraObject * pCamObj = pLvCamObj->getCameraObject();
		if( pCamObj != NULL)
		{
			m_pCamera = pCamObj->getCamera();
		}
	}

	UpdateInfo();
}

void CProp_LvElm_CameraObj::UpdateInfo(void)
{
	BOOL bEnable = TRUE;
	if( m_pCamera == NULL)
	{
		bEnable = FALSE;
	}

	m_edFOV.EnableWindow( bEnable);
	m_edNear.EnableWindow( bEnable);
	m_edFar.EnableWindow( bEnable);
	m_edWidth.EnableWindow( bEnable);
	m_edHeight.EnableWindow( bEnable);
	m_edUpdateInterval.EnableWindow( bEnable);
	m_cbMode.EnableWindow( bEnable);
	m_chkEnable.EnableWindow( bEnable);

	if( bEnable == FALSE)
		return;

	char conv[MAX_PATH];
	sprintf( conv, "%f", I3_RAD2DEG(m_pCamera->GetFOV()));
	m_edFOV.SetWindowText( conv);

	sprintf( conv, "%f", m_pCamera->GetNear());
	m_edNear.SetWindowText( conv);

	sprintf( conv, "%f", m_pCamera->GetFar());
	m_edFar.SetWindowText( conv);

	sprintf( conv, "%f", m_pCamera->GetWindowWidth());
	m_edWidth.SetWindowText( conv);

	sprintf( conv, "%f", m_pCamera->GetWindowHeight());
	m_edHeight.SetWindowText( conv);

	sprintf( conv, "%f", m_pCamera->getUpdateInterval());
	m_edUpdateInterval.SetWindowText( conv);

	m_cbMode.SetCurSel( (INT32)m_pCamera->GetMode());
	m_chkEnable.SetCheck( (m_pCamera->GetEnable())? BST_CHECKED : BST_UNCHECKED);

	{
		i3LevelCameraObject * pLVCamObj = (i3LevelCameraObject*)getObject();
		i3CameraObject * pCamObj = pLVCamObj->getCameraObject();

		INT32 width				= (INT32)pCamObj->getRTWidth();
		INT32 i = 0;
		for( i = 0; i < 6; ++i)
		{
			if( width == 32 << i)
			{
				m_cbCamRT_Width.SetCurSel( i);
				break;
			}
		}

		if( i == 6 )
		{
			I3PRINTLOG(I3LOG_WARN, "렌더타겟의 크기가 규정에 맞지 않아	최소사이즈로 설정합니다");
			m_cbCamRT_Width.SetCurSel( 0);
			pCamObj->setRTWidth((REAL32)32.0f);
		}

		INT32 Height				= (INT32)pCamObj->getRTHeight();
		for( i = 0; i < 6; ++i)
		{
			if( Height == 32 << i)
			{
				m_cbCamRT_Height.SetCurSel( i);
				break;
			}
		}

		if( i == 6 )
		{
			I3PRINTLOG(I3LOG_WARN, "렌더타겟의 크기가 규정에 맞지 않아	최소사이즈로 설정합니다");
			m_cbCamRT_Height.SetCurSel( 0);
			pCamObj->setRTHeight((REAL32)32.0f);
		}

		I3G_IMAGE_FORMAT fmtColor	= pCamObj->getColorRTFormat();
		INT32 idx = getFormatIndex( fmtColor);
		if( idx != -1)
		{
			m_cbCamRT_Color_Format.SetCurSel( getFormatIndex( fmtColor));
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN, "렌더타겟의 이미지 포맷이 설정되지않아 Default설정을 따릅니다( Default: XRGB32_8888)");
			m_cbCamRT_Color_Format.SetCurSel( getFormatIndex( I3G_IMAGE_FORMAT_BGRX_8888));
			pCamObj->setColorRTFormat( I3G_IMAGE_FORMAT_BGRX_8888);
		}

		I3G_IMAGE_FORMAT fmtDepth	= pCamObj->getDepthRTFormat();
		idx = getFormatIndex( fmtDepth);
		if( idx != -1)
		{
			m_cbCamRT_Depth_Format.SetCurSel( getFormatIndex( fmtDepth));
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN, "렌더타겟의 이미지 포맷이 설정되지않아 Default설정을 따릅니다( Default: D16)");
			m_cbCamRT_Depth_Format.SetCurSel( getFormatIndex( I3G_IMAGE_FORMAT_D16));
			pCamObj->setDepthRTFormat( I3G_IMAGE_FORMAT_D16);
		}
	}
}

void CProp_LvElm_CameraObj::OnEnChangeEdCameraFov()
{
	if( m_pCamera != NULL && (m_edFOV.GetModify() == TRUE))
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());

		char conv[MAX_PATH];
		m_edFOV.GetWindowText( conv, MAX_PATH - 1);

		REAL32 fov = (REAL32)atof( conv);

		m_pCamera->SetFOV( I3_DEG2RAD(fov));

		i3LevelCameraObject * pLvCamObj = (i3LevelCameraObject *)getObject();
		if( pLvCamObj != NULL)
		{
			pLvCamObj->_UpdateFrustum();
		}

		m_edFOV.SetModify( FALSE);
	}
}

void CProp_LvElm_CameraObj::OnEnChangeEdCameraNear()
{
	if( m_pCamera != NULL && (m_edNear.GetModify() == TRUE))
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());

		char conv[MAX_PATH];
		m_edNear.GetWindowText( conv, MAX_PATH - 1);
		
		REAL32 rNear = (REAL32)atof( (const char*)conv);

		m_pCamera->SetNear( rNear);

		i3LevelCameraObject * pLvCamObj = (i3LevelCameraObject *)getObject();
		if( pLvCamObj != NULL)
		{
			pLvCamObj->_UpdateFrustum();
		}

		m_edNear.SetModify( FALSE);
	}
}

void CProp_LvElm_CameraObj::OnEnChangeEdCameraFar()
{
	if( m_pCamera != NULL && (m_edFar.GetModify() == TRUE))
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());

		char conv[MAX_PATH];
		m_edFar.GetWindowText( conv, MAX_PATH - 1);

		REAL32	rFar = (REAL32)atof( (const char*)conv);
		m_pCamera->SetFar( rFar);

		i3LevelCameraObject * pLvCamObj = (i3LevelCameraObject *)getObject();
		if( pLvCamObj != NULL)
		{
			pLvCamObj->_UpdateFrustum();
		}

		m_edFar.SetModify( FALSE);
	}
}

void CProp_LvElm_CameraObj::OnEnChangeEdCameraWidth()
{
	if( m_pCamera != NULL && (m_edWidth.GetModify() == TRUE))
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());

		char conv[MAX_PATH];
		m_edWidth.GetWindowText( conv, MAX_PATH - 1);

		REAL32 rWidth = (REAL32)atof( (const char*)conv);

		m_pCamera->SetWindowSize( rWidth, m_pCamera->GetWindowHeight());

		m_edWidth.SetModify( FALSE);
	}
}

void CProp_LvElm_CameraObj::OnEnChangeEdCameraHeight()
{
	if( m_pCamera != NULL && (m_edHeight.GetModify() == TRUE))
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());

		char conv[MAX_PATH];
		m_edHeight.GetWindowText( conv, MAX_PATH - 1);

		REAL32 rHeight = (REAL32)atof( (const char*)conv);

		m_pCamera->SetWindowSize( m_pCamera->GetWindowWidth(), rHeight);

		i3LevelCameraObject * pLvCamObj = (i3LevelCameraObject *)getObject();
		if( pLvCamObj != NULL)
		{
			pLvCamObj->_UpdateFrustum();
		}

		m_edHeight.SetModify( FALSE);
	}
}

void CProp_LvElm_CameraObj::OnCbnSelchangeCbCameraMode()
{
	if( m_pCamera != NULL)
	{
		INT32 ncur = (INT32)m_pCamera->GetMode();
		INT32 sel = m_cbMode.GetCurSel();

		if( ncur != sel)
		{
			i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());
			m_pCamera->SetMode( (i3Camera::I3_CAMERA_MODE)sel);
		}
	}
}

void CProp_LvElm_CameraObj::OnBnClickedChkCameraEnable()
{
	if( m_pCamera != NULL)
	{
		BOOL bCurEnable = m_pCamera->GetEnable();
		BOOL bEnable = (BOOL)(m_chkEnable.GetCheck() == BST_CHECKED);

		if( bCurEnable != bEnable)
		{
			i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());

			m_pCamera->SetEnable( m_chkEnable.GetCheck() == BST_CHECKED);
		}
	}
}

void CProp_LvElm_CameraObj::OnEnChangeEdCameraUpdateinterval()
{
	if( m_pCamera != NULL  && (m_edUpdateInterval.GetModify() == TRUE))
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)getObject());

		char conv[MAX_PATH];
		m_edUpdateInterval.GetWindowText( conv, MAX_PATH - 1);

		REAL32 rUpdateInterval = (REAL32)atof( (const char*)conv);

		m_pCamera->setUpdateInterval(rUpdateInterval);

		m_edUpdateInterval.SetModify( FALSE);
	}
}

void CProp_LvElm_CameraObj::OnCbnSelchangeCbCameraRtWidth()
{
	// TODO: Add your control notification handler code here
	i3LevelCameraObject * pLVCamObj = (i3LevelCameraObject *)getObject();
	if( pLVCamObj != NULL)
	{
		i3CameraObject * pCamObj = pLVCamObj->getCameraObject();
		if( pCamObj != NULL)
		{
			INT32 cursize = (INT32)pCamObj->getRTWidth();

			INT32 getCurSel = m_cbCamRT_Width.GetCurSel();
			INT32 size = 32 << getCurSel;

			if( cursize != size)
			{
				i3LevelUndo::ADD_Edit( (i3LevelElement3D*)pLVCamObj);

				pCamObj->setRTWidth((REAL32)size);
			}
		}
	}
}

void CProp_LvElm_CameraObj::OnCbnSelchangeCbCameraRtHeight()
{
	// TODO: Add your control notification handler code here
	i3LevelCameraObject * pLvCamObj = (i3LevelCameraObject *)getObject();
	if( pLvCamObj != NULL)
	{
		i3CameraObject * pCamObj = pLvCamObj->getCameraObject();
		if( pCamObj != NULL)
		{
			INT32 cursize = (INT32)pCamObj->getRTHeight();

			INT32 getCurSel = m_cbCamRT_Height.GetCurSel();
			INT32 size = 32 << getCurSel;

			if( cursize != size)
			{
				i3LevelUndo::ADD_Edit( (i3LevelElement3D*)pLvCamObj);

				pCamObj->setRTHeight((REAL32)size);
			}
		}
	}
}

void CProp_LvElm_CameraObj::OnCbnSelchangeCbCameraRtColorFormat()
{
	// TODO: Add your control notification handler code here
	i3LevelCameraObject * pLvCamObj = (i3LevelCameraObject *)getObject();
	if( pLvCamObj != NULL)
	{
		i3CameraObject * pCamObj = pLvCamObj->getCameraObject();
		if( pCamObj != NULL)
		{
			I3G_IMAGE_FORMAT curfmt = pCamObj->getColorRTFormat();

			INT32 sel = m_cbCamRT_Color_Format.GetCurSel();

			I3G_IMAGE_FORMAT fmt = getFormatByIdx( sel);

			if( curfmt != fmt)
			{
				i3LevelUndo::ADD_Edit( (i3LevelElement3D*)pLvCamObj);
				pCamObj->setColorRTFormat( fmt);
			}
		}
	}
}

void CProp_LvElm_CameraObj::OnCbnSelchangeCbCameraRtDepthFormat()
{
	// TODO: Add your control notification handler code here
	i3LevelCameraObject * pLVCamObj = (i3LevelCameraObject*)getObject();

	if( pLVCamObj != NULL)
	{
		i3CameraObject * pCamObj = pLVCamObj->getCameraObject();
		if( pCamObj != NULL)
		{
			I3G_IMAGE_FORMAT curfmt = pCamObj->getDepthRTFormat();

			INT32 sel = m_cbCamRT_Depth_Format.GetCurSel();

			I3G_IMAGE_FORMAT fmt = getFormatByIdx( sel);

			if( curfmt != fmt)
			{
				i3LevelUndo::ADD_Edit( (i3LevelElement3D*)pLVCamObj);

				pCamObj->setDepthRTFormat( fmt);
			}
		}
	}
}
