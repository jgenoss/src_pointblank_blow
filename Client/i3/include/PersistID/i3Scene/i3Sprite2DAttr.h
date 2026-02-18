// i3SpriteAttr.h
#if !defined( I3_SPRITE_2D_ATTR_H)
#define I3_SPRITE_2D_ATTR_H

#include "i3RenderAttr.h"

typedef struct ALIGN4 _tagSprite2DInfo
{
	BOOL			m_bEnable;
	RT_VEC3D		m_Center;
	RT_REAL32		m_Width;
	RT_REAL32		m_Height;
	COLOR			m_Color;
	RT_REAL32		m_Rotation;
	RT_VEC2D		m_UV[2];
	BOOL			m_bFlipX;
	BOOL			m_bFlipY;
	UINT32			m_EditMask;
} I3SG_SPRITE2D_INFO;

class I3_EXPORT_SCENE i3Sprite2DAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3Sprite2DAttr);
protected:
	i3RenderContext *		m_pContext;
	UINT32					m_Count;
	i3VertexArray *			m_pVA;
	i3IndexArray *			m_pIA;
	UINT32					m_PrimCount;
	i3Shader *				m_pShader;
	i3VertexFormat			m_format;

	BOOL					m_bEditted;
	BOOL					m_bAlwaysUpdate;

	I3SG_SPRITE2D_INFO *	m_pSprites;
//	INT32					m_SpritesIdx; 

	INT32					m_OddEven;
	INT32					m_HalfVertexCount;

	// Vertex Bank
	BOOL					m_bUseVertexBank;
	VERTEX_BANK_ELEMENT*	m_pVBElement;

protected:

public:
	i3Sprite2DAttr(void);
	virtual ~i3Sprite2DAttr(void);

	BOOL		Create( UINT32 Count, BOOL bUV = TRUE, BOOL bColor = FALSE, BOOL bUseVertexBank = FALSE);

	void		SetEnable( BOOL bFlag);

	void		SetEnable( UINT32 Index, BOOL bFlag);
	BOOL		GetEnable( UINT32 Index)					{ I3_BOUNDCHK( Index, m_Count); return m_pSprites[Index].m_bEnable; }

	void		SetCenter( UINT32 Index, RT_VEC3D * pVec);
	void		SetCenter( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z);
	RT_VEC3D *	GetCenter( UINT32 Index)					{ I3_BOUNDCHK( Index, m_Count); return & m_pSprites[Index].m_Center; }

	void		GetLeftTop( UINT32 Index, RT_VEC3D * pVec);
	void		SetLeftTop( UINT32 Index, RT_VEC3D * pVec);
	void		GetRightBottom( UINT32 Index, RT_VEC3D * pVec);

	void		SetRotation( UINT32 Index,RT_REAL32 rotation);
	void		SetFlip( UINT32 Index, BOOL FlipX, BOOL FlipY );
	BOOL		IsFlipX( UINT32 Index )						{I3_BOUNDCHK( Index, m_Count);  return m_pSprites[Index].m_bFlipX; }
	BOOL		IsFlipY( UINT32 Index )						{ I3_BOUNDCHK( Index, m_Count); return m_pSprites[Index].m_bFlipY; }

	void		SetSize( UINT32 Index, RT_VEC2D* pVec);
	void		SetSize( UINT32 Index, RT_REAL32 Width, RT_REAL32 Height);
	RT_VEC2D *	GetSize( UINT32 Index);

	void		SetTextureCoord( UINT32 Index, RT_VEC2D * pMin, RT_VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, RT_REAL32 u1, RT_REAL32 v1, RT_REAL32 u2, RT_REAL32 v2);
	RT_VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)	{ I3_BOUNDCHK( Index, m_Count); return & m_pSprites[Index].m_UV[which]; }

	void		SetColor( COLOR * pColor);
	void		SetColor( UINT32 Index, COLOR * pColor);
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void		SetColor( UINT32 Index, UINT32 rgba);
	COLOR *		GetColor( UINT32 Index)							{ I3_BOUNDCHK( Index, m_Count); return & m_pSprites[Index].m_Color; }

	void		ConnectVertexBank( UINT32 nCount);
	void		CheckVisibleCount( UINT32* pCount);
	void		CheckHideSprite(void);

	virtual	void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
