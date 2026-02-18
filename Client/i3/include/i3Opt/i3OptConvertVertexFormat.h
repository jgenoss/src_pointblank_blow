#if !defined( __I3_OPT_CONVERT_VERTEX_FORMAT_H)
#define __I3_OPT_CONVERT_VERTEX_FORMAT_H

#include "i3Base.h"
#include "i3Scene.h"
#include "i3SceneOptimizer.h"

class i3OptConvertTextureFormat;

class I3_EXPORT_OPT i3OptConvertVertexFormat : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptConvertVertexFormat, i3SceneOptimizer);
protected:
	bool			m_bConvertPos;			// [initialize at constructor]
	I3VF_TYPES		m_PosType;				// [initialize at constructor]

	bool			m_bConvertNormal;		// [initialize at constructor]
	I3VF_TYPES		m_NormalType;			// [initialize at constructor]

	bool			m_bConvertColor;		// [initialize at constructor]
	I3VF_TYPES		m_ColorType;			// [initialize at constructor]

	bool			m_bConvertWeight;		// [initialize at constructor]
	I3VF_TYPES		m_WeightType;			// [initialize at constructor]

	bool			m_bConvertTexCoord;		// [initialize at constructor]
	I3VF_TYPES		m_TexCoordType;			// [initialize at constructor]

	INT32			m_ColorOrder = I3VF_COLORORDER_ARGB;

	INT32			m_Platform = I3VF_PLATFORM_NA;

	i3OptConvertTextureFormat*	m_pTextureConvertFormat = nullptr;

	REAL32			m_rTestMin_U = 1000.0f;
	REAL32			m_rTestMax_U = -1000.0f;
	REAL32			m_rTestMin_V = 1000.0f;
	REAL32			m_rTestMax_V = -1000.0f;

protected:
	void		CopyVertexArray( i3VertexArray * pDest, i3VertexArray * pSrc, REAL32 sx, REAL32 sy, REAL32 sz, VEC3D * pOffset);
	
	I3VF_TYPES	_getTypes( char * pszValue);	//	Add by KOMET
public:
	i3OptConvertVertexFormat(void);

	void		SetPositionConvertEnable( bool bFlag)	{ m_bConvertPos = bFlag; }
	bool		GetPositionConvertEnable(void)			{ return m_bConvertPos; }

	void		SetPositionType( I3VF_TYPES type)		{ m_PosType = type; }
	I3VF_TYPES	GetPositionType(void)					{ return m_PosType; }

	void		SetNormalConvertEnable( bool bFlag)		{ m_bConvertNormal = bFlag; }
	bool		GetNormalConvertEnable(void)			{ return m_bConvertNormal; }

	void		SetNormalType( I3VF_TYPES type)			{ m_NormalType = type; }
	I3VF_TYPES	GetNormalType(void)						{ return m_NormalType; }

	void		SetColorConvertEnable( bool bFlag)		{ m_bConvertColor = bFlag; }
	bool		GetColorConvertEnable(void)				{ return m_bConvertColor; }

	void		SetColorType( I3VF_TYPES type)			{ m_ColorType = type; }
	I3VF_TYPES	GetColorType(void)						{ return m_ColorType; }

	void		SetWeightConvertEnable( bool bFlag)		{ m_bConvertWeight = bFlag; }
	bool		GetWeightConvertEnable(void)			{ return m_bConvertWeight; }

	void		SetWeightType( I3VF_TYPES type)			{ m_WeightType = type; }
	I3VF_TYPES	GetWeightType(void)						{ return m_WeightType; }

	void		SetTextureCoordConvertEnable( bool bFlag)	{ m_bConvertTexCoord = bFlag; }
	bool		GetTextureCoordConvertEnable(void)			{ return m_bConvertTexCoord; }

	void		SetTextureCoordType( I3VF_TYPES type)	{ m_TexCoordType = type; }
	I3VF_TYPES	GetTextureCoordType(void)				{ return m_TexCoordType; }

	INT32		GetColorOrder(void)						{ return m_ColorOrder; }
	void		SetColorOrder( INT32 order)				{ m_ColorOrder = order; }

	INT32		GetTargetPlatform(void)					{ return m_Platform; }
	void		SetTargetPlatform( INT32 platform)		{ m_Platform = platform; }

	virtual void	Trace( i3Node * pRoot) override;

	virtual void	SetProperty( char * pszFieldName, char * pszValue) override;
};

#endif