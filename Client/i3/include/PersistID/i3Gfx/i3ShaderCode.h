#if !defined( __I3_SHADER_CODE_H)
#define __I3_SHADER_CODE_H

#include "i3ShaderDefine.h"
#include "i3Light.h"

class I3_EXPORT_GFX i3ShaderCode
{
protected:
	UINT64			m_Code;

public:
	i3ShaderCode(void);

	////////////////////////
	// Diffuse Map
	inline
	bool				getDiffuseMapEnable(void)			{ return (m_Code & I3G_SHADER_FLAG_DIFFUSE_MAP) != 0; }

	inline
	void				setDiffuseMapEnable( bool bFlag)	{ bFlag ? (m_Code |= I3G_SHADER_FLAG_DIFFUSE_MAP) : (m_Code &= ~I3G_SHADER_FLAG_DIFFUSE_MAP); }

	////////////////////////
	// Specular Map
	inline
	bool				getSpecularMapEnable(void)			{ return (m_Code & I3G_SHADER_FLAG_SPECULAR_MAP) != 0; }

	inline
	void				setSpecularMapEnable( bool bFlag)	{ bFlag ? (m_Code |= I3G_SHADER_FLAG_SPECULAR_MAP) : (m_Code &= ~I3G_SHADER_FLAG_SPECULAR_MAP); }

	////////////////////////
	// Reflect
	inline
	bool				getReflectMapEnable(void)			{ return (m_Code & I3G_SHADER_FLAG_REFLECT_MAP) != 0; }

	inline
	void				setReflectMapEnable( bool bFlag)	{ bFlag ? (m_Code |= I3G_SHADER_FLAG_REFLECT_MAP) : (m_Code &= ~I3G_SHADER_FLAG_REFLECT_MAP); }

	////////////////////////
	// Reflect Mask
	inline
	bool				getReflectMaskMapEnable(void)		{ return (m_Code & I3G_SHADER_FLAG_REFLECT_MASK_MAP) != 0; }

	inline
	void				setReflectMaskMapEnable( bool bFlag)	{ bFlag ? (m_Code |= I3G_SHADER_FLAG_REFLECT_MASK_MAP) : (m_Code &= ~I3G_SHADER_FLAG_REFLECT_MASK_MAP); }

	////////////////////////
	// Emissive Map
	inline
	bool				getEmissiveMapEnable(void)			{ return (m_Code & I3G_SHADER_FLAG_EMISSIVE_MAP) != 0; }

	inline
	void				setEmissiveMapEnable( bool bFlag)	{ bFlag ? (m_Code |= I3G_SHADER_FLAG_EMISSIVE_MAP) : (m_Code &= ~I3G_SHADER_FLAG_EMISSIVE_MAP); }

	////////////////////////
	// Normal Map
	inline
	bool				getNormalMapEnable(void)			{ return (m_Code & I3G_SHADER_FLAG_NORMAL_MAP) != 0; }

	inline
	void				setNormalMapEnable( bool bFlag)		{ bFlag ? (m_Code |= I3G_SHADER_FLAG_NORMAL_MAP) : (m_Code &= ~I3G_SHADER_FLAG_NORMAL_MAP); }

	////////////////////////
	// Per-Pixel Lighting
	inline
	bool				isPPLShader(void)					{ return (m_Code & I3G_SHADER_FLAG_PPL) != 0; }

	inline
	void				setPPLShader( bool bFlag)			{ bFlag ? (m_Code |= I3G_SHADER_FLAG_PPL) : (m_Code &= ~I3G_SHADER_FLAG_PPL); }

	////////////////////////
	// Vertex Color
	inline
	bool				getVertexColorEnable(void)			{ return (m_Code & I3G_SHADER_FLAG_VERTEX_COLOR) != 0; }

	inline
	void				setVertexColorEnable( bool bFlag)	{ bFlag ? (m_Code |= I3G_SHADER_FLAG_VERTEX_COLOR) : (m_Code &= ~I3G_SHADER_FLAG_VERTEX_COLOR); }

	////////////////////////
	// Lux-Map
	inline
	bool				getLuxMapEnable(void)				{ return (m_Code & I3G_SHADER_FLAG_LUX_MAP) != 0; }

	inline
	void				setLuxMapEnable( bool bFlag)		{ bFlag ? (m_Code |= I3G_SHADER_FLAG_LUX_MAP) : (m_Code &= ~I3G_SHADER_FLAG_LUX_MAP); }

	// Lighting
	inline
	bool				getLightingEnable(void)				{ return (m_Code & I3G_SHADER_FLAG_LIGHTING) != 0; }

	inline
	void				setLightingEnable( bool bFlag)		{ bFlag ? (m_Code |= I3G_SHADER_FLAG_LIGHTING) : (m_Code &= ~I3G_SHADER_FLAG_LIGHTING); }

	// Fresnel
	inline
	bool				getFresnelEnable(void)				{ return (m_Code & I3G_SHADER_FLAG_FRESNEL) != 0; }

	inline
	void				setFresnelEnable( bool bFlag)		{ bFlag ? (m_Code |= I3G_SHADER_FLAG_FRESNEL) : (m_Code &= ~I3G_SHADER_FLAG_FRESNEL); }

	////////////////////////
	// Light Calc. Model
	inline
	I3G_SHADER_FLAG_LIGHT_MODEL		getLightModel( INT32 idx)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));

		return (I3G_SHADER_FLAG_LIGHT_MODEL) ((m_Code >> (I3G_SHADER_SHIFT_LIGHT_MODEL + (idx * I3G_SHADER_SHIFT_LIGHT_ALL))) & I3G_SHADER_MASK_LIGHT_MODEL);
	}

	inline
	void							setLightModel( INT32 idx, I3G_SHADER_FLAG_LIGHT_MODEL model)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));
		register INT32 shift = (I3G_SHADER_SHIFT_LIGHT_MODEL + (idx * I3G_SHADER_SHIFT_LIGHT_ALL));

		m_Code = (m_Code & ~(I3G_SHADER_MASK_LIGHT_MODEL << shift)) | (model << shift);
	}

	////////////////////////
	// Light Type
	inline
	I3G_SHADER_FLAG_LIGHT_TYPE		getLightType( INT32 idx)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));
		register INT32 shift = (I3G_SHADER_SHIFT_LIGHT_TYPE + (idx * I3G_SHADER_SHIFT_LIGHT_ALL));

		return (I3G_SHADER_FLAG_LIGHT_TYPE) ((m_Code >> shift) & I3G_SHADER_MASK_LIGHT_TYPE);
	}

	inline
	void							setLightType( INT32 idx, I3G_SHADER_FLAG_LIGHT_TYPE type)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));
		register INT32 shift = (I3G_SHADER_SHIFT_LIGHT_TYPE + (idx * I3G_SHADER_SHIFT_LIGHT_ALL));

		m_Code = (m_Code & ~(I3G_SHADER_MASK_LIGHT_TYPE << shift)) | (type << shift);
	}

	////////////////////////
	// Textured Light
	inline
	bool							getTexturedLight( INT32 idx)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));

		return (m_Code >> (I3G_SHADER_SHIFT_TEXTURE_LIGHT + (idx * I3G_SHADER_SHIFT_LIGHT_ALL))) & I3G_SHADER_MASK_TEXTURE_LIGHT;
	}

	void							setTexturedLight( INT32 idx, bool bFlag)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));
		register INT32 shift = (I3G_SHADER_SHIFT_TEXTURE_LIGHT + (idx * I3G_SHADER_SHIFT_LIGHT_ALL));
		INT32 type = (bFlag == true);

		m_Code = (m_Code & ~(I3G_SHADER_MASK_TEXTURE_LIGHT << shift)) | (type << shift);
	}

	////////////////////////
	// Shadow Type
	I3G_SHADER_FLAG_SHADOW			getShadowType( INT32 idx)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));

		return (I3G_SHADER_FLAG_SHADOW) ((m_Code >> (I3G_SHADER_SHIFT_SHADOW + (idx * I3G_SHADER_SHIFT_LIGHT_ALL))) & I3G_SHADER_MASK_SHADOW);
	}

	void							setShadowType( INT32 idx, I3G_SHADER_FLAG_SHADOW type)
	{
		I3ASSERT( (idx >= 0) && (idx <= 1));
		register INT32 shift = (I3G_SHADER_SHIFT_SHADOW + (idx * I3G_SHADER_SHIFT_LIGHT_ALL));

		m_Code = (m_Code & ~(I3G_SHADER_MASK_SHADOW << shift)) | (type << shift);
	}

	void							setForceShadowMapEnable( bool bFlag)
	{
		UINT32 mask = (I3G_SHADER_MASK_SHADOW << I3G_SHADER_SHIFT_SHADOW);

		if( bFlag == false)
		{
			m_Code &= ~( ((UINT64) mask << I3G_SHADER_SHIFT_LIGHT_ALL) | mask);
		}
	}


	UINT64							getCode(void)				{ return m_Code; }
	void							setCode( UINT64 code)		{ m_Code = code; }

	void							setLights( i3Light * pLight1 = NULL, i3Light * pLight2 = NULL);

	void							GenDefName( char * pszName);
	void							SetName( char * pszName);
};

#endif
