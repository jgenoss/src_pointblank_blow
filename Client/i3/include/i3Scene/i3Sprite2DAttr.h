// i3SpriteAttr.h
#if !defined( I3_SPRITE_2D_ATTR_H)
#define I3_SPRITE_2D_ATTR_H

#include "i3RenderAttr.h"

struct ALIGN4 I3SG_SPRITE2D_INFO
{
	bool		m_bEnable = false;
	VEC3D		m_Center;
	REAL32		m_Width = 0.0f;
	REAL32		m_Height = 0.0f;
	COLOR		m_Color;
	REAL32		m_Rotation = 0.0f;
	VEC2D		m_UV[2];
	bool		m_bFlipX = false;
	bool		m_bFlipY = false;
	UINT32		m_EditMask = 0;
} ;

enum I3_SPRITE2D_COORD_TYPE
{
	I3_SPRITE2D_COORD_UNIFORM = 0,
	I3_SPRITE2D_COORD_PIXEL,
};

class I3_EXPORT_SCENE i3Sprite2DAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3Sprite2DAttr, i3RenderAttr);
protected:
	i3VertexFormat			m_format;
	i3RenderContext *		m_pContext = nullptr;
	i3VertexArray *			m_pVA = nullptr;
	i3IndexArray *			m_pIA = nullptr;
	i3Shader *				m_pShader = nullptr;
	I3SG_SPRITE2D_INFO *	m_pSprites = nullptr;
	UINT32					m_Count = 0;
	UINT32					m_PrimCount = 0;
	bool					m_bEditted = true;
	bool					m_bAlwaysUpdate = true;
	INT32					m_OddEven = 0;
	INT32					m_HalfVertexCount = 0;

	I3_SPRITE2D_COORD_TYPE	m_CoordType = I3_SPRITE2D_COORD_UNIFORM;

	// Vertex Bank
	bool					m_bUseVertexBank = false;
	VERTEX_BANK_ELEMENT*	m_pVBElement = nullptr;

protected:
	void		_CreateVertex(void);

public:
	i3Sprite2DAttr(void);
	virtual ~i3Sprite2DAttr(void);

	bool		Create( UINT32 Count, bool bUV = true, bool bColor = false, bool bUseVertexBank = false);

	void		SetEnable( bool bFlag);

	void		SetEnable( UINT32 Index, bool bFlag);
	bool		GetEnable( UINT32 Index)					{ I3ASSERT( Index < m_Count); return m_pSprites[Index].m_bEnable; }

	void		SetCenter( UINT32 Index, VEC3D * pVec);
	void		SetCenter( UINT32 Index, REAL32 x, REAL32 y, REAL32 z);
	VEC3D *	GetCenter( UINT32 Index)					{ I3ASSERT( Index < m_Count); return & m_pSprites[Index].m_Center; }

	void		GetLeftTop( UINT32 Index, VEC3D * pVec);
	void		SetLeftTop( UINT32 Index, VEC3D * pVec);
	void		GetRightBottom( UINT32 Index, VEC3D * pVec);

	void		SetRotation( UINT32 Index,REAL32 rotation);
	void		SetFlip( UINT32 Index, bool FlipX, bool FlipY );
	bool		IsFlipX( UINT32 Index )						{ I3ASSERT( Index < m_Count);  return m_pSprites[Index].m_bFlipX; }
	bool		IsFlipY( UINT32 Index )						{ I3ASSERT( Index < m_Count); return m_pSprites[Index].m_bFlipY; }

	void		SetSize( UINT32 Index, VEC2D* pVec);
	void		SetSize( UINT32 Index, REAL32 Width, REAL32 Height);
	VEC2D *	GetSize( UINT32 Index);

	void		SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2);
	VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)	{ I3ASSERT( Index < m_Count); return & m_pSprites[Index].m_UV[which]; }

	void		SetColor( COLOR * pColor);
	void		SetColor( UINT32 Index, COLOR * pColor);
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void		SetColor( UINT32 Index, UINT32 rgba);
	COLOR *		GetColor( UINT32 Index)							{ I3ASSERT( Index < m_Count); return & m_pSprites[Index].m_Color; }

	I3_SPRITE2D_COORD_TYPE		getCoordType(void)				{ return m_CoordType; }
	void						setCoordType( I3_SPRITE2D_COORD_TYPE type)
	{
		m_CoordType = type;
	}

	void		ConnectVertexBank( UINT32 nCount);
	void		CheckVisibleCount( UINT32* pCount);
	void		CheckHideSprite(void);

	virtual	void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool	IsSame(i3RenderAttr * pAttr) override { return false; }

	virtual void	OnLostDevice( bool bLostDevice) override;
	virtual void	OnRevive( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#if defined (I3_DEBUG)
	void	DumpVB(void)										{ m_pVA->Dump();												}
#endif
};

#endif
