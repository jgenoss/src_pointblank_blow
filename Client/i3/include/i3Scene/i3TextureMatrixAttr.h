#if !defined( I3SG_TEXTURE_MATRIX_ATTR_H)
#define I3SG_TEXTURE_MATRIX_ATTR_H

#include "i3RenderAttr.h"

enum EUVANIM_TYPE
{
	EUVANIM_NONE = 0,
	EUVANIM_LINEAR,
	EUVANIM_SINUSOIDAL, 

	EUVANIM_COUNT
};

class I3_EXPORT_SCENE i3TextureMatrixAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextureMatrixAttr, i3RenderAttr);
protected:
	MATRIX			m_Matrix;
	bool				m_bProjective = false;
	INT32				m_OutputElementCount = 0;
	I3G_TEXTURE_BIND	m_Bind = I3G_TEXTURE_BIND_DIFFUSE;

	//UV Animation
	EUVANIM_TYPE		m_eUVAnimU = EUVANIM_NONE;
	EUVANIM_TYPE		m_eUVAnimV = EUVANIM_NONE;
	
	REAL32				m_fUB1 = 0.0f, m_fUB2 = 0.0f;
	REAL32				m_fVB1 = 0.0f, m_fVB2 = 0.0f;
	
	REAL32 accTmU = 0.0f;
	REAL32 accTmV = 0.0f;


public:
	i3TextureMatrixAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND idx)		{ m_Bind = idx; }
	I3G_TEXTURE_BIND	GetBindType(void)						{ return m_Bind; }

	MATRIX *		GetMatrix(void)							{ return &m_Matrix; }
	void			SetMatrix( MATRIX * pMatrix)			{ i3Matrix::Copy( &m_Matrix, pMatrix); }

	bool			IsProjective(void)						{ return m_bProjective; }
	void			SetProjectiveState( bool bFlag)			{ m_bProjective = bFlag; }

	void			SetOutputElementCount( INT32 Count)		{ m_OutputElementCount = Count; }
	INT32			GetOutputElementCount(void)				{ return m_OutputElementCount; }

	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual void	Apply( i3RenderContext * pContext) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;


	//UV Animation
	EUVANIM_TYPE	GetUVAnimTypeU( void)				{	return m_eUVAnimU;	}
	void			SetUVAnimTypeU( EUVANIM_TYPE type)	{	m_eUVAnimU = type;	}

	EUVANIM_TYPE	GetUVAnimTypeV( void)				{	return m_eUVAnimV;	}
	void			SetUVAnimTypeV( EUVANIM_TYPE type)	{	m_eUVAnimV = type;	}

	void			SetUB1( REAL32 val)		{	m_fUB1 = val;	}
	void			SetUB2( REAL32 val)		{	m_fUB2 = val;	}
	void			SetVB1( REAL32 val)		{	m_fVB1 = val;	}
	void			SetVB2( REAL32 val)		{	m_fVB2 = val;	}

	REAL32			GetUB1(	void)			{	return m_fUB1;	}
	REAL32			GetUB2(	void)			{	return m_fUB2;	}
	REAL32			GetVB1(	void)			{	return m_fVB1;	}
	REAL32			GetVB2(	void)			{	return m_fVB2;	}
	
	void			ApplyUVAnim( REAL32 tm);


	#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
	#endif
};

#endif

