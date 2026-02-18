#if !defined( __I3_GFX_CAPS_H)
#define __I3_GFX_CAPS_H

#include "i3Base.h"
#include "i3GfxDefine.h"
#include "i3Wmi.h"

#define		I3GFXCAPS_TEXFUNC_ADD				0x00000001
#define		I3GFXCAPS_TEXFUNC_MODULATE			0x00000002
#define		I3GFXCAPS_TEXFUNC_SPECULAR_HILIT	0x00000004

enum I3G_GPU_CLASS
{
	I3G_GPU_CLASS_NON_SUPPORT = -1,

	I3G_GPU_CLASS_F,					// Shader Model 2.0을 겨우 지원함.
										// TexFill:~1700

	I3G_GPU_CLASS_E,					// TexFill:~2400

	I3G_GPU_CLASS_D,					// Shader Model 3.0을 겨우 지원함
										// TexFill:~4000

	I3G_GPU_CLASS_C,					// TexFill:~6000
	I3G_GPU_CLASS_B,					// TexFill:~10000
	I3G_GPU_CLASS_A,					// 
};

enum I3G_GPU_VENDOR
{
	I3G_GPU_VENDOR_NA		= 0,
	I3G_GPU_VENDOR_STANDARD,
	I3G_GPU_VENDOR_NVIDIA,
	I3G_GPU_VENDOR_ATI,
};

// Apapter VendorID
#define _NVIDIA_APAPTER		0x000010de
#define _ATI_APAPTER			0x00001002

struct ADAPTER_INFO
{
	char					m_szGPUName[256] = { 0 };				// 그래픽 카드 이름
	I3G_GPU_VENDOR			m_AdapterVendor = I3G_GPU_VENDOR_NA;	// 그래픽 카드 제조사
	UINT32					m_AdapterID = 0;						// 그래픽 카드 ID
};

struct I3G_DISPLAY_MODE_INFO
{
	INT32				m_Width = 0;
	INT32				m_Height = 0;
	I3G_IMAGE_FORMAT	m_Format = I3G_IMAGE_FORMAT_NONE;
	REAL32				m_RefreshRate = 0.0f;
};

struct I3G_AA_INFO
{
	INT32				m_Sampling = 0;
	INT32				m_QualityCount = 0;
};

class I3_EXPORT_GFX i3GfxCaps : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GfxCaps, i3ElementBase);
protected:

	INT32			m_MaxBoneMatrixCount;		// [initialize at constructor]
	INT32			m_MaxBlendIndexCount;		// [initialize at constructor]
	REAL32			m_MaxPointSize;				// [initialize at constructor]
	bool			m_bPerVertexPointSize;		// [initialize at constructor]
	INT32			m_MaxTextureStage;			// [initialize at constructor]
	UINT32			m_TexFuncBits = 0;			
	bool			m_bAlphaClutEnable;			// [initialize at constructor]
	UINT32			m_PS_NumInstructionSlots;	// [initialize at constructor]

	INT32			m_DefaultRefreshRate;		// [initialize at constructor]
	
	INT32			m_MaxVSVersionMajor;		// [initialize at constructor]
	INT32			m_MaxVSVersionMinor;		// [initialize at constructor]
	INT32			m_MaxPSVersionMajor;		// [initialize at constructor]
	INT32			m_MaxPSVersionMinor;		// [initialize at constructor]

	I3G_GPU_CLASS	m_GPUClass;					// [initialize at constructor]

	i3::rc_string	m_strVertexShaderProfile;	// 이 멤버변수들은 리터럴타입이 전혀 문제가 되지 않지만,
	i3::rc_string	m_strPixelShaderProfile;	// 전체적인 코드 일관성을 위해 const char*에서 rc_string으로 일단 바꾼다.

	ADAPTER_INFO	m_ApapterInfo;

	bool			m_bShaderEnable = true;
	bool			m_bGammaControlEnable;	// [initialize at constructor]
	INT32			m_MaxVertexShaderConst;	// [initialize at constructor]
	bool			m_bRegisterSwizzle;		// [initialize at constructor]
	bool			m_bTexInstructionLimit;	// [initialize at constructor]

	bool			m_bEnableUBYTE4;		// [initialize at constructor]

	INT32			m_MaxAnisotropy;		// [initialize at constructor]

	INT32			m_DPI;					// [initialize at constructor]

	UINT32			m_VRAMSize;				// [initialize at constructor]

	UINT32			m_uiMaxVertexIndex;		// [initialize at constructor]

#if defined(I3_WIPI) || defined(I3_WINCE)
	GLint		m_CompressedTexCount;
	GLint		m_CompressedTexFormat[32];
#endif

	i3Wmi			m_Wmi;

protected:
	UINT32		_GetVRAMSize(void);

public:
	i3GfxCaps(void);

	INT32		GetMaxBoneMatrixCount(void)					{ return m_MaxBoneMatrixCount; }
	INT32		GetMaxBlendIndexCount(void)					{ return m_MaxBlendIndexCount; }
	REAL32		GetMaxPointSize(void)						{ return m_MaxPointSize; }
	bool		IsPerVertexPointSize(void)					{ return m_bPerVertexPointSize; }
	INT32		GetMaxTextureStage(void)					{ return m_MaxTextureStage; }
	bool		CanTextureFuncAdd(void)						{ return (m_TexFuncBits & I3GFXCAPS_TEXFUNC_ADD) != 0; }
	bool		CanTextureFuncModulate(void)				{ return (m_TexFuncBits & I3GFXCAPS_TEXFUNC_MODULATE) != 0; }
	bool		CanTextureFuncSpecularHighlight(void)		{ return (m_TexFuncBits & I3GFXCAPS_TEXFUNC_SPECULAR_HILIT) != 0; }
	bool		IsEnableAlphaCLUT(void)						{ return m_bAlphaClutEnable; }
	UINT32		getPSMaxInstructionSlot(void)				{ return m_PS_NumInstructionSlots; }
	bool		CanRegisterSwizzle(void)					{ return m_bRegisterSwizzle; }
	bool		IsTexInstructionLimit(void)					{ return m_bTexInstructionLimit; }

	bool		isSupported_UBYTE4(void)					{ return m_bEnableUBYTE4; }

	const char *	getVertexShaderProfile(void) const		{ return m_strVertexShaderProfile.c_str(); }
	INT32		getVertexShaderVersionMajor(void)			{ return m_MaxVSVersionMajor; }
	INT32		getVertexShaderVersionMinor(void)			{ return m_MaxVSVersionMinor; }
	const char *	getPixelShaderProfile(void)	const		{ return m_strPixelShaderProfile.c_str(); }
	INT32		getPixelShaderVersionMajor(void)			{ return m_MaxPSVersionMajor; }
	INT32		getPixelShaderVersionMinor(void)			{ return m_MaxPSVersionMinor; }

	UINT32		getVRAMSize(void)							{ return m_VRAMSize; }

	I3G_GPU_VENDOR	getGPUVendor(void)						{ return m_ApapterInfo.m_AdapterVendor; }
	void			setGPUVendor( I3G_GPU_VENDOR vendor)	{ m_ApapterInfo.m_AdapterVendor = vendor; }
	char *		getGPUName(void)							{ return m_ApapterInfo.m_szGPUName;		}
	I3G_GPU_CLASS	getGPUClass(void)						{ return m_GPUClass; }

	static		I3G_GPU_CLASS	DetermineGPUClass(void);

	UINT32		GetMaxVertexIndex()							{ return m_uiMaxVertexIndex; }
	I3G_GPU_CLASS		CheckGPUClass(const I3G_GPU_CLASS gpuClass);

#if defined(I3G_DX)
	bool		getEnableGamma(void)						{ return m_bGammaControlEnable;	}
	bool		getEnableShader(void)						{ return m_bShaderEnable;		}
	void		SetEnableShader(bool bFlag);

	INT32		getDisplayModeCount( I3G_IMAGE_FORMAT fmt);
	void		getDisplayMode( INT32 idx, I3G_IMAGE_FORMAT fmt, I3G_DISPLAY_MODE_INFO * pInfo);

	INT32		getDefaultRefreshRate(void)					{ return m_DefaultRefreshRate; }

	INT32		getMaxAnisotropy(void)						{ return m_MaxAnisotropy; }

	INT32		getDisplayDPI(void)							{ return m_DPI; }

	void		buildAAList( I3G_IMAGE_FORMAT fmt, bool bWindowed, i3::vector<I3G_AA_INFO*>& List);
#endif

#if defined(I3_WIPI) || defined(I3_WINCE)
	bool		CanCompressTexFormat(INT32 format);
#endif
};

#endif
