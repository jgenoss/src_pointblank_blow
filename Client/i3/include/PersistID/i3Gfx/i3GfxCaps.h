#if !defined( __I3_GFX_CAPS_H)
#define __I3_GFX_CAPS_H

#include "i3Base.h"
#include "i3GfxDefine.h"

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
	I3G_GPU_VENDOR_UNKNOWN = 0,
	I3G_GPU_VENDOR_NVIDIA,
	I3G_GPU_VENDOR_ATI,
};

// Apapter VendorID
#define _NVIDIA_APAPTER		0x000010de
#define _ATI_APAPTER			0x00001002

typedef struct
{
	char					m_szGPUName[256];		// 그래픽 카드 이름
	I3G_GPU_VENDOR			m_AdapterVendor;		// 그래픽 카드 제조사
	UINT32					m_AdapterID;			// 그래픽 카드 ID
}ADAPTER_INFO;

typedef struct
{
	INT32				m_Width;
	INT32				m_Height;
	I3G_IMAGE_FORMAT	m_Format;
	REAL32				m_RefreshRate;
} I3G_DISPLAY_MODE_INFO;

typedef struct _tagAAInfo
{
	INT32				m_Sampling;
	INT32				m_QualityCount;
} I3G_AA_INFO;

class I3_EXPORT_GFX i3GfxCaps : public i3ElementBase
{
	I3_CLASS_DEFINE( i3GfxCaps);
protected:

	INT32			m_MaxBoneMatrixCount;
	INT32			m_MaxBlendIndexCount;
	REAL32			m_MaxPointSize;
	BOOL			m_bPerVertexPointSize;
	INT32			m_MaxTextureStage;
	UINT32			m_TexFuncBits;
	BOOL			m_bAlphaClutEnable;
	UINT32			m_PS_NumInstructionSlots;
	
	INT32			m_MaxVSVersionMajor;
	INT32			m_MaxVSVersionMinor;
	INT32			m_MaxPSVersionMajor;
	INT32			m_MaxPSVersionMinor;

	I3G_GPU_CLASS	m_GPUClass;

	char *			m_pszVertexShaderProfile;
	char *			m_pszPixelShaderProfile;

	ADAPTER_INFO	m_ApapterInfo;

	BOOL			m_bShaderEnable;
	BOOL			m_bGammaControlEnable;
	INT32			m_MaxVertexShaderConst;
	bool			m_bRegisterSwizzle;
	bool			m_bTexInstructionLimit;

	bool			m_bEnableUBYTE4;

	INT32			m_MaxAnisotropy;

#if defined(I3_WIPI) || defined(I3_WINCE)
	GLint		m_CompressedTexCount;
	GLint		m_CompressedTexFormat[32];
#endif

public:
	i3GfxCaps(void);
	virtual ~i3GfxCaps(void);

	INT32		GetMaxBoneMatrixCount(void)					{ return m_MaxBoneMatrixCount; }
	INT32		GetMaxBlendIndexCount(void)					{ return m_MaxBlendIndexCount; }
	REAL32		GetMaxPointSize(void)						{ return m_MaxPointSize; }
	BOOL		IsPerVertexPointSize(void)					{ return m_bPerVertexPointSize; }
	INT32		GetMaxTextureStage(void)					{ return m_MaxTextureStage; }
	BOOL		CanTextureFuncAdd(void)						{ return (m_TexFuncBits & I3GFXCAPS_TEXFUNC_ADD) != 0; }
	BOOL		CanTextureFuncModulate(void)				{ return (m_TexFuncBits & I3GFXCAPS_TEXFUNC_MODULATE) != 0; }
	BOOL		CanTextureFuncSpecularHighlight(void)		{ return (m_TexFuncBits & I3GFXCAPS_TEXFUNC_SPECULAR_HILIT) != 0; }
	BOOL		IsEnableAlphaCLUT(void)						{ return m_bAlphaClutEnable; }
	UINT32		getPSMaxInstructionSlot(void)				{ return m_PS_NumInstructionSlots; }
	bool		CanRegisterSwizzle(void)					{ return m_bRegisterSwizzle; }
	bool		IsTexInstructionLimit(void)					{ return m_bTexInstructionLimit; }

	bool		isSupported_UBYTE4(void)					{ return m_bEnableUBYTE4; }

	char *		getVertexShaderProfile(void)				{ return m_pszVertexShaderProfile; }
	INT32		getVertexShaderVersionMajor(void)			{ return m_MaxVSVersionMajor; }
	INT32		getVertexShaderVersionMinor(void)			{ return m_MaxVSVersionMinor; }
	char *		getPixelShaderProfile(void)					{ return m_pszPixelShaderProfile; }
	INT32		getPixelShaderVersionMajor(void)			{ return m_MaxPSVersionMajor; }
	INT32		getPixelShaderVersionMinor(void)			{ return m_MaxPSVersionMinor; }

	I3G_GPU_VENDOR	getGPUVendor(void)						{ return m_ApapterInfo.m_AdapterVendor; }
	void			setGPUVendor( I3G_GPU_VENDOR vendor)	{ m_ApapterInfo.m_AdapterVendor = vendor; }
	char *		getGPUName(void)							{ return m_ApapterInfo.m_szGPUName;		}
	I3G_GPU_CLASS	getGPUClass(void)						{ return m_GPUClass; }

	static		I3G_GPU_CLASS	DetermineGPUClass(void);

#if defined(I3G_DX)
	BOOL		getEnableGamma(void)						{ return m_bGammaControlEnable;	}
	BOOL		getEnableShader(void)						{ return m_bShaderEnable;		}
	void		SetEnableShader(BOOL bFlag);

	INT32		getDisplayModeCount( I3G_IMAGE_FORMAT fmt);
	void		getDisplayMode( INT32 idx, I3G_IMAGE_FORMAT fmt, I3G_DISPLAY_MODE_INFO * pInfo);

	INT32		getMaxAnisotropy(void)						{ return m_MaxAnisotropy; }

	void		buildAAList( I3G_IMAGE_FORMAT fmt, BOOL bWindowed, i3List * pList);
#endif

#if defined(I3_WIPI) || defined(I3_WINCE)
	BOOL		CanCompressTexFormat(INT32 format);
#endif
};

#endif
