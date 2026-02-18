#if !defined( __I3_SHADER_PARAM_H)
#define __I3_SHADER_PARAM_H

#include "i3Texture.h"

class i3RenderContext;

#define	I3G_SHADER_PMASK_MATRIX_WORLD		0x80000000
#define	I3G_SHADER_PMASK_MATRIX_BONE		0x40000000
#define I3G_SHADER_PMASK_MATRIX_VIEW		0x20000000
#define	I3G_SHADER_PMASK_MATRIX_PROJECT		0x10000000
#define I3G_SHADER_PMASK_LIGHT0				0x08000000
#define I3G_SHADER_PMASK_LIGHT1				0x04000000
#define I3G_SHADER_PMASK_MATERIAL			0x02000000
#define I3G_SHADER_PMASK_AMBIENT			0x01000000
#define I3G_SHADER_PMASK_COLOR				0x00800000
#define	I3G_SHADER_PMASK_TEX_DIFFUSE		0x00400000
#define I3G_SHADER_PMASK_TEX_LUX			0x00200000
#define I3G_SHADER_PMASK_TEX_SPECULAR		0x00100000
#define I3G_SHADER_PMASK_TEX_NORMAL			0x00080000
#define I3G_SHADER_PMASK_TEX_REFLECT		0x00040000
#define I3G_SHADER_PMASK_TEX_REFLECT_MASK	0x00020000
#define I3G_SHADER_PMASK_TEX_EMISSIVE		0x00010000
#define I3G_SHADER_PMASK_FRESNEL			0x00000800
#define I3G_SHADER_PMASK_ALL				0xFFFF0000

#define I3G_SHADER_PMASK_TEX_ALL			(	I3G_SHADER_PMASK_TEX_DIFFUSE | \
												I3G_SHADER_PMASK_TEX_LUX | \
												I3G_SHADER_PMASK_TEX_SPECULAR | \
												I3G_SHADER_PMASK_TEX_NORMAL | \
												I3G_SHADER_PMASK_TEX_REFLECT | \
												I3G_SHADER_PMASK_TEX_REFLECT_MASK | \
												I3G_SHADER_PMASK_TEX_EMISSIVE )


class I3_EXPORT_GFX i3ShaderParam : public i3NamedElement
{
	I3_CLASS_DEFINE( i3ShaderParam);

public:
	enum SEMANTIC
	{
		SEMANTIC_CONSTANT					= 0,

		SEMANTIC_MATRIX_WORLD,
		SEMANTIC_MATRIX_BONE,
		SEMANTIC_MATRIX_VIEW,
		SEMANTIC_MATRIX_INV_VIEW,
		SEMANTIC_MATRIX_WORLDVIEW,
		SEMANTIC_MATRIX_VIEWPROJECT,
		SEMANTIC_MATRIX_PROJECT,
		SEMANTIC_MATRIX_FULL,

		SEMANTIC_AMBIENT,
		SEMANTIC_COLOR,

		SEMANTIC_LIGHT_DIRECTION_0,
		SEMANTIC_LIGHT_POSITION_0,
		SEMANTIC_LIGHT_DIFFUSE_0,
		SEMANTIC_LIGHT_SPECULAR_0,
		SEMANTIC_LIGHT_MAP_0,
		SEMANTIC_LIGHT_FACTOR_0,

		SEMANTIC_SHADOWMAP_MATRIX_0,
		SEMANTIC_SHADOWMAP_FACTOR_0,
		SEMANTIC_SHADOWMAP_0,

		SEMANTIC_LIGHT_DIRECTION_1,
		SEMANTIC_LIGHT_POSITION_1,
		SEMANTIC_LIGHT_DIFFUSE_1,
		SEMANTIC_LIGHT_SPECULAR_1,
		SEMANTIC_LIGHT_MAP_1,
		SEMANTIC_LIGHT_FACTOR_1,

		SEMANTIC_SHADOWMAP_MATRIX_1,
		SEMANTIC_SHADOWMAP_FACTOR_1,
		SEMANTIC_SHADOWMAP_1,

		SEMANTIC_MATERIAL_EMISSIVE,

		SEMANTIC_BONE_COUNT,

		SEMANTIC_CAMERA_POSITION,
		SEMANTIC_CAMERA_DIRECTION,

		SEMANTIC_DIFFUSEMAP,
		SEMANTIC_SPECULARMAP,
		SEMANTIC_NORMALMAP,
		SEMANTIC_EMISSIVEMAP,
		SEMANTIC_REFLECTMAP,
		SEMANTIC_REFLECTMASKMAP,

		SEMANTIC_LUXMAP,

		SEMANTIC_FRESNEL_IOR,
		SEMANTIC_FRESNEL_COLOR,

		SEMANTIC_NUMS	
	};

	enum TARGET
	{
		TARGET_VERTEXSHADER,
		TARGET_PIXELSHADER
	};

protected:
	// Persistent Members
	SEMANTIC	m_Semantic;
	UINT32		m_UpdateMask;
	TARGET		m_Target;
	INT32		m_Index;

	// Volatile Members
	void *		m_hHandle;

#if defined( I3G_DX)
	D3DXCONSTANT_DESC		m_Desc;

	void		_UpdateFloat( REAL32 * pVal, INT32 RegIdx, INT32 RegCnt);
	void		_UpdateInt( INT32 * pVal, INT32 RegIdx, INT32 RegCnt);
	void		_UpdateBOOL( BOOL * pVal, INT32 RegIdx, INT32 RegCnt);
#endif

protected:
	UINT32		_CalcDataSize(void);
	void		_PrepareBuffer(void);

public:
	i3ShaderParam(void);
	virtual ~i3ShaderParam(void);

	SEMANTIC	getSemantic(void)					{ return m_Semantic; }
	void		setSemantic( SEMANTIC sem);

	UINT32		getUpdateMask(void)					{ return m_UpdateMask; }
	void		setUpdateMask( UINT32 mask)			{ m_UpdateMask = mask; }
	void		addUpdateMask( UINT32 mask)			{ m_UpdateMask |= mask; }
	void		removeUpdateMask( UINT32 mask)		{ m_UpdateMask &= ~mask; }

	TARGET		getTarget(void)						{ return m_Target; }
	void		setTarget( TARGET target)			{ m_Target = target; }

	INT32		getIndex(void)						{ return m_Index; }
	void		setIndex( INT32 idx)				{ m_Index = idx; }

	void *		getHandle(void)						{ return m_hHandle; }
	void		setHandle( void * hndl)				{ m_hHandle = hndl; }

	#if defined( I3G_DX)
	bool		Init( ID3DXConstantTable * pTable);
	#endif

	void		Update( RT_MATRIX * pMatrix);
	void		Update( RT_MATRIX * pMatrix, INT32 count);
	void		Update( REAL32 val);
	void		Update( INT32 val);
	void		Update( bool val);
	void		Update( VEC2D * pVec);
	void		Update( VEC3D * pVec);
	void		Update( VEC4D * pVec);
	void		Update( COLORREAL * pColor);
	void		Update( i3Texture * pTex, bool bProjective = false);
	#if defined( I3G_DX)
	void		Update( I3G_TEXTURE_BIND_CONTEXT * pTexCtx);
	#endif

	static SEMANTIC		GetSemanticByVarName( const char * pszName);
	
	virtual void	Update( i3RenderContext * pCtx);

	virtual	INT32	OnQueryDefinition( char * pszBuf, INT32 len);

	virtual	UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
