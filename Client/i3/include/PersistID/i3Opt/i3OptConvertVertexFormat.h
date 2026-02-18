#if !defined( __I3_OPT_CONVERT_VERTEX_FORMAT_H)
#define __I3_OPT_CONVERT_VERTEX_FORMAT_H

#include "i3Base.h"
#include "i3Scene.h"
#include "i3SceneOptimizer.h"

class i3OptConvertTextureFormat;

class I3_EXPORT_OPT i3OptConvertVertexFormat : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptConvertVertexFormat);
protected:
	BOOL			m_bConvertPos;
	I3VF_TYPES		m_PosType;

	BOOL			m_bConvertNormal;
	I3VF_TYPES		m_NormalType;

	BOOL			m_bConvertColor;
	I3VF_TYPES		m_ColorType;

	BOOL			m_bConvertWeight;
	I3VF_TYPES		m_WeightType;

	BOOL			m_bConvertTexCoord;
	I3VF_TYPES		m_TexCoordType;

	INT32			m_ColorOrder;

	INT32			m_Platform;

	i3OptConvertTextureFormat*	m_pTextureConvertFormat;

	REAL32			m_rTestMin_U;
	REAL32			m_rTestMax_U;
	REAL32			m_rTestMin_V;
	REAL32			m_rTestMax_V;

protected:
	void		CopyVertexArray( i3VertexArray * pDest, i3VertexArray * pSrc, REAL32 sx, REAL32 sy, REAL32 sz, VEC3D * pOffset);
	
	I3VF_TYPES	_getTypes( char * pszValue);	//	Add by KOMET
public:
	i3OptConvertVertexFormat(void);
	virtual ~i3OptConvertVertexFormat(void);

	void		SetPositionConvertEnable( BOOL bFlag)	{ m_bConvertPos = bFlag; }
	BOOL		GetPositionConvertEnable(void)			{ return m_bConvertPos; }

	void		SetPositionType( I3VF_TYPES type)		{ m_PosType = type; }
	I3VF_TYPES	GetPositionType(void)					{ return m_PosType; }

	void		SetNormalConvertEnable( BOOL bFlag)		{ m_bConvertNormal = bFlag; }
	BOOL		GetNormalConvertEnable(void)			{ return m_bConvertNormal; }

	void		SetNormalType( I3VF_TYPES type)			{ m_NormalType = type; }
	I3VF_TYPES	GetNormalType(void)						{ return m_NormalType; }

	void		SetColorConvertEnable( BOOL bFlag)		{ m_bConvertColor = bFlag; }
	BOOL		GetColorConvertEnable(void)				{ return m_bConvertColor; }

	void		SetColorType( I3VF_TYPES type)			{ m_ColorType = type; }
	I3VF_TYPES	GetColorType(void)						{ return m_ColorType; }

	void		SetWeightConvertEnable( BOOL bFlag)		{ m_bConvertWeight = bFlag; }
	BOOL		GetWeightConvertEnable(void)			{ return m_bConvertWeight; }

	void		SetWeightType( I3VF_TYPES type)			{ m_WeightType = type; }
	I3VF_TYPES	GetWeightType(void)						{ return m_WeightType; }

	void		SetTextureCoordConvertEnable( BOOL bFlag)	{ m_bConvertTexCoord = bFlag; }
	BOOL		GetTextureCoordConvertEnable(void)			{ return m_bConvertTexCoord; }

	void		SetTextureCoordType( I3VF_TYPES type)	{ m_TexCoordType = type; }
	I3VF_TYPES	GetTextureCoordType(void)				{ return m_TexCoordType; }

	INT32		GetColorOrder(void)						{ return m_ColorOrder; }
	void		SetColorOrder( INT32 order)				{ m_ColorOrder = order; }

	INT32		GetTargetPlatform(void)					{ return m_Platform; }
	void		SetTargetPlatform( INT32 platform)		{ m_Platform = platform; }

	virtual void	Trace( i3Node * pRoot);

	virtual void	SetProperty( char * pszFieldName, char * pszValue);
};

#endif