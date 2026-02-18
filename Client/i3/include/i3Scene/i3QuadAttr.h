// i3QuadAttr.h
#if !defined( I3_QUAD_ATTR_H)
#define I3_QUAD_ATTR_H

#include "i3RenderAttr.h"

enum I3SG_QUAD_SPACE
{
	I3SG_QUAD_SPACE_WORLD		=	0,
	I3SG_QUAD_SPACE_SCREEN	=	1,
};

#define I3SG_QUAD_EDIT_POSITION			0x00000001
#define I3SG_QUAD_EDIT_UV				0x00000002
#define I3SG_QUAD_EDIT_COLOR			0x00000004
#define I3SG_QUAD_EDIT_ROTATION			0x00000010

enum QUAD_ROTATION
{
	QUAD_ROT_0,
	QUAD_ROT_90,
	QUAD_ROT_180,
	QUAD_ROT_270,
};

struct ALIGN4 I3SG_QUAD_INFO
{
	bool		m_bEnable = false;
	VEC3D		m_Center;
	REAL32		m_Width = 0.0f;
	REAL32		m_Height = 0.0f;
	COLOR		m_Color;
	QUAD_ROTATION	m_Rotation;
	VEC2D		m_UV[2];
	bool		m_bFlipX = false;
	bool		m_bFlipY = false;
	UINT32		m_EditMask = 0;
};

class I3_EXPORT_SCENE i3QuadAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3QuadAttr, i3RenderAttr);
protected:
	I3SG_QUAD_SPACE			m_Space = I3SG_QUAD_SPACE_WORLD;
	UINT32					m_Count = 0;
	i3VertexArray *			m_pVA = nullptr;
	i3IndexArray *			m_pIA = nullptr;
	UINT32					m_PrimCount = 0;
	i3Shader *				m_pShader = nullptr;
	i3VertexFormat			m_Format;

	bool					m_bEditted = true;
	bool					m_bAlwaysUpdate = false;

	I3SG_QUAD_INFO *		m_pQuads = nullptr;

	MATRIX				m_InvViewMatrix;
	UINT32					m_nBufferIndex = 0;

protected:
	void		_CreateVertex(void);
	void		ApplyAlwaysUpdate( i3RenderContext * pContext);
	void		ApplyEdittedUpdate( i3RenderContext * pContext);

public:
	i3QuadAttr(void);
	virtual ~i3QuadAttr(void);

	bool		Create( UINT32 Count, I3SG_QUAD_SPACE Space, bool bUV = true, bool bColor = false, bool bAlwaysUpdate = false);

	i3VertexArray *	GetVertexArray(void)					{ return m_pVA; }

	void		SetEnable( UINT32 Index, bool bFlag);
	bool		GetEnable( UINT32 Index)					{ return m_pQuads[Index].m_bEnable; }

	void		SetCenter( UINT32 Index, VEC3D * pVec);
	void		SetCenter( UINT32 Index, REAL32 x, REAL32 y, REAL32 z);
	VEC3D *	GetCenter( UINT32 Index)					{ return & m_pQuads[Index].m_Center; }

	void		GetLeftTop( UINT32 Index, VEC3D * pVec);
	void		GetRightBottom( UINT32 Index, VEC3D * pVec);

	void		SetRotation( UINT32 Index, QUAD_ROTATION Rot );
	void		SetFlip( UINT32 Index, bool FlipX, bool FlipY );

	void		SetSize( UINT32 Index, VEC2D* pVec);
	void		SetSize( UINT32 Index, REAL32 Width, REAL32 Height);
	void 		GetSize( UINT32 Index, VEC2D * pVec);

	void		SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2);
	VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)	{ return & m_pQuads[Index].m_UV[which]; }

	void		SetColor( UINT32 Index, COLOR * pColor);
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void		SetColor( UINT32 Index, UINT32 rgba);
	COLOR *		GetColor( UINT32 Index)							{ return & m_pQuads[Index].m_Color; }

	void		SetInverseViewMatrix( MATRIX* pMat)
	{
		i3Matrix::Copy( &m_InvViewMatrix, pMat);
	}

	virtual	void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;

	virtual void	OnLostDevice( bool bLostDevice) override;
	virtual void	OnRevive( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
