// i3DecalAttr.h
#if !defined( I3_DECAL_ATTR_H)
#define I3_DECAL_ATTR_H

#include "i3RenderAttr.h"
#include "i3PhysixShapeSet.h"

//--------------------------------------------------------------------------------------------------------------------------------
//	DecalAttr
//	작업자 : 정순구
//	데칼은 Physix의 BoxShape을 사용합니다. 그리고 ContactReport를 받기 위해 EventReceiver를 설정해야 합니다.

enum I3SG_DECAL_SPACE
{
	I3SG_DECAL_SPACE_WORLD		=	0,
	I3SG_DECAL_SPACE_SCREEN	=	1,
};

#define I3SG_DECAL_EDIT_POSITION		0x00000001
#define I3SG_DECAL_EDIT_UV				0x00000002
#define I3SG_DECAL_EDIT_COLOR			0x00000004
#define I3SG_DECAL_EDIT_ROTATION		0x00000010
#define I3SG_DECAL_EDIT_FLIP			0x00000040

#define I3SG_DECAL_MAX_VERTICES			256

typedef struct ALIGN4 _tagDECALInfo
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

	i3PhysixShapeSet *	m_pHitBoxShapeSet;
	VEC3D			m_pPoints[ I3SG_DECAL_MAX_VERTICES ];
	VEC2D			m_pUVs[ I3SG_DECAL_MAX_VERTICES ];
	INT32			m_nHitCount;

} I3SG_DECAL_INFO;

class I3_EXPORT_SCENE i3DecalAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3DecalAttr);
protected:
	bool					m_bFaceToCamera;
	UINT32					m_Count;
	i3VertexArray *			m_pVA;
	i3IndexArray *			m_pIA;
	UINT32					m_PrimCount;

	BOOL					m_bUpdatted;
	BOOL					m_bEditted;
	BOOL					m_bAlwaysUpdate;

	I3SG_DECAL_INFO *		m_pDecals;
	
	RT_MATRIX				m_InvViewMatrix;

	INT32					m_nMaxVertices;
	REAL32					m_rDecalEpsilon;

protected:
	void		ApplyAlwaysUpdate( i3RenderContext * pContext);
	void		ApplyEdittedUpdate( i3RenderContext * pContext);

public:
	UINT32		getDecalCount( void )							{ return m_Count; }
	bool		getFaceToCameraEnable(void)						{ return m_bFaceToCamera; }
	BOOL		GetEnable( UINT32 Index)						{ return m_pDecals[Index].m_bEnable; }
	RT_VEC3D *	GetCenter( UINT32 Index)						{ return & m_pDecals[Index].m_Center; }

	BOOL		IsFlipX( UINT32 Index )							{ return m_pDecals[Index].m_bFlipX; }
	BOOL		IsFlipY( UINT32 Index )							{ return m_pDecals[Index].m_bFlipY; }

	RT_VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)	{ return & m_pDecals[Index].m_UV[which]; }

	COLOR *		GetColor( UINT32 Index)							{ return & m_pDecals[Index].m_Color; }

	void		SetInverseViewMatrix( RT_MATRIX * pMatrix)		{	i3Matrix::Copy( &m_InvViewMatrix, pMatrix);	}

	i3PhysixShapeSet * getShapeSet( UINT32 Index )				{ return m_pDecals[Index].m_pHitBoxShapeSet; }

	void		SetHitOn( UINT32 Index )
	{
		if( m_pDecals[Index].m_pHitBoxShapeSet != NULL )
		{
			m_pDecals[Index].m_pHitBoxShapeSet->modifyDynamicState( 0, I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_COLLISION );
		}
	}

	void		SetHitOff( UINT32 Index )
	{
		if( m_pDecals[Index].m_pHitBoxShapeSet != NULL )
		{
			m_pDecals[Index].m_pHitBoxShapeSet->modifyDynamicState( I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_COLLISION , 0 );
		}
	}

public:
	i3DecalAttr(void);
	virtual ~i3DecalAttr(void);

	BOOL		Create( UINT32 Count, BOOL bUV = TRUE, BOOL bColor = FALSE, BOOL bNormalMap = FALSE, BOOL bAlwaysUpdate = FALSE);

	void		setFaceToCameraEnable( bool bFlag);
	void		SetEnable( BOOL bFlag);

	void		SetEnable( UINT32 Index, BOOL bFlag);

	void		SetCenter( UINT32 Index, RT_VEC3D * pVec);
	void		SetCenter( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z);

	void		GetLeftTop( UINT32 Index, RT_VEC3D * pVec);
	void		GetRightBottom( UINT32 Index, RT_VEC3D * pVec);

	void		SetRotation( UINT32 Index,RT_REAL32 rotation);
	void		SetFlip( UINT32 Index, BOOL FlipX, BOOL FlipY );

	void		SetSize( UINT32 Index, RT_VEC2D* pVec);
	void		SetSize( UINT32 Index, RT_REAL32 Width, RT_REAL32 Height);
	RT_VEC2D *	GetSize( UINT32 Index);

	void		SetTextureCoord( UINT32 Index, RT_VEC2D * pMin, RT_VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, RT_REAL32 u1, RT_REAL32 v1, RT_REAL32 u2, RT_REAL32 v2);

	void		SetColor( COLOR * pColor);
	void		SetColor( UINT32 Index, COLOR * pColor);
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void		SetColor( UINT32 Index, UINT32 rgba);

	void		ApplyDrawUpdate( i3RenderContext * pContext );
	void		SetEventOwner( i3EventReceiver * pOwner );

	virtual	void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif