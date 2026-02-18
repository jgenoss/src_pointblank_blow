#if !defined( __EDGE_TRAILER_H)
#define __EDGE_TRAILER_H

#include "Weapon/WeaponBase.h"

#define TPIS_VALID		0x00000001

struct TRAILPOSINFO
{
	UINT32		m_State = 0;
	REAL32		m_time = 0.0f;
	VEC3D		m_Pos0;
	VEC3D		m_Pos1;
} ;

class CEdgeTrailer : public i3GameObj
{
	I3_CLASS_DEFINE( CEdgeTrailer, i3GameObj);
protected:
	i3AttrSet *			m_pAttrSet = nullptr;
	i3Geometry *		m_pGeo = nullptr;
	i3GeometryAttr *	m_pGeoAttr = nullptr;
	i3VertexArray *		m_pVA = nullptr;
	i3IndexArray *		m_pIA = nullptr;

	i3Shader		*	m_pShader = nullptr;
	VEC4D				m_vRefractiveIndex = { 1.33333f, 0.0f, 0.0f, 0.0f };

	i3TextureBindAttr * m_pTexBindAttr = nullptr;

	TRAILPOSINFO *		m_pTrails = nullptr;
	INT32				m_idxHead = 0;
	INT32				m_idxTail = 0;

	INT32				m_MaxCount = 0;
	INT32				m_PatchCount = 0;

	WeaponBase *		m_pWeapon = nullptr;
	INT32				m_idxBoneStart = -1;
	INT32				m_idxBoneEnd = -1;

	REAL32				m_rElapsedStopTime = 0.0f;
	bool				m_bEnableStop = false;

protected:
	void				_CreateVertex(void);

public:
	virtual ~CEdgeTrailer(void);

	WeaponBase *		getBindedWeapon(void)				{ return m_pWeapon; }

	bool				Create( INT32 cnt = 20);
	bool				Bind( WeaponBase * pWeapon);
	void				Reset(void);

	void				Start( WeaponBase * pWeapon);
	void				Stop(void);

	virtual void		OnUpdate( REAL32 tm) override;

	void				RotateSph(REAL32 delta);
	virtual bool		OnLostDevice( bool bLostDevice) override;
	virtual bool		OnRevive( i3RenderContext * pCtx) override;

//	void				RotateSph(REAL32 delta);
};

#endif
