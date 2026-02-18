// i3SpriteAttr.h
#if !defined( I3_Sprite_ATTR_H)
#define I3_Sprite_ATTR_H

#include "i3RenderAttr.h"

enum I3SG_SPRITE_SPACE
{
	I3SG_SPRITE_SPACE_WORLD		=	0,
	I3SG_SPRITE_SPACE_SCREEN	=	1,
};

#define I3SG_SPRITE_EDIT_POSITION		0x00000001
#define I3SG_SPRITE_EDIT_UV				0x00000002
#define I3SG_SPRITE_EDIT_COLOR			0x00000004
#define I3SG_SPRITE_EDIT_ROTATION		0x00000010
#define I3SG_SPRITE_EDIT_FLIP			0x00000040

typedef struct ALIGN4 _tagSpriteInfo
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
} I3SG_SPRITE_INFO;

class I3_EXPORT_SCENE i3SpriteAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3SpriteAttr);
protected:
	bool					m_bFaceToCamera;
	UINT32					m_Count;
	i3VertexArray *			m_pVA;
	i3IndexArray *			m_pIA;
	UINT32					m_PrimCount;

	BOOL					m_bEditted;
	BOOL					m_bAlwaysUpdate;

	I3SG_SPRITE_INFO *		m_pSprites;
	//INT32					m_SpritesIdx; 

	RT_MATRIX				m_InvViewMatrix;

	i3Shader *				m_pShader;

protected:

	void		ApplyAlwaysUpdate( i3RenderContext * pContext);
	void		ApplyEdittedUpdate( i3RenderContext * pContext);


public:
	i3SpriteAttr(void);
	virtual ~i3SpriteAttr(void);

	BOOL		Create( UINT32 Count, BOOL bUV = TRUE, BOOL bColor = FALSE, BOOL bNormalMap = FALSE, BOOL bAlwaysUpdate = FALSE);

	void		setFaceToCameraEnable( bool bFlag);
	bool		getFaceToCameraEnable(void)						{ return m_bFaceToCamera; }

	void		SetEnable( BOOL bFlag);

	void		SetEnable( UINT32 Index, BOOL bFlag);
	BOOL		GetEnable( UINT32 Index)					{ I3_BOUNDCHK( Index, m_Count); return m_pSprites[Index].m_bEnable; }

	void		SetCenter( UINT32 Index, RT_VEC3D * pVec);
	void		SetCenter( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z);
	RT_VEC3D *	GetCenter( UINT32 Index)					{ I3_BOUNDCHK( Index, m_Count); return & m_pSprites[Index].m_Center; }

	void		GetLeftTop( UINT32 Index, RT_VEC3D * pVec);
	void		GetRightBottom( UINT32 Index, RT_VEC3D * pVec);

	void		SetRotation( UINT32 Index,RT_REAL32 rotation);
	void		SetFlip( UINT32 Index, BOOL FlipX, BOOL FlipY );
	BOOL		IsFlipX( UINT32 Index )						{ I3_BOUNDCHK( Index, m_Count); return m_pSprites[Index].m_bFlipX; }
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

	void		SetInverseViewMatrix( RT_MATRIX * pMatrix)
	{
		i3Matrix::Copy( &m_InvViewMatrix, pMatrix);
	}

	virtual	void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif
