#if !defined( __CLOTH_FRAMEWORK_H)
#define __CLOTH_FRAMEWORK_H

#define			DELTA_TIME				0.6f

#define			VSTATE_SELECTED		0x00000001
#define			VSTATE_CULL			0x00000002
#define			VSTATE_PARTICLE		0x00000004

#define			SSTATE_SELECTED		0x00000001

#define			SELMASK_PARTICLE	0x00000001
#define			SELMASK_SPRING		0x00000002

typedef struct _tagCollisionMeshInfo
{
	MATRIX				m_Matrix;
	i3CollideeMesh *	m_pMesh;
	i3Geometry *		m_pGeometry;
	i3GeometryAttr *	m_pGeoAttr;
	INT32 *				m_pVtxIndexTable;
	i3Sprite2D *		m_pSprite;
	UINT32	*			m_pVertexState;
	UINT32	*			m_pSpringState;
	i3PhysicsParticle **	m_ppParticle;
} COLL_MESH_INFO;

class CClothFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( CClothFramework);
protected:
	i3ClothObject *	m_pCloth;

	i3OverrideAttrSet *		m_pOverrideRoot;
	i3AttrSet *		m_pVertexRoot;
	i3AttrSet *		m_pSpringRoot;
	i3VertexArray *	m_pSpringVA;

	i3List			m_CollMeshList;

	i3Timer		*	m_pTimer;

	CRect			m_OldDragRect;
	bool			m_bSelectDrag;

	COLL_MESH_INFO *	m_pTargetInfo;

	i3TextureEnableAttr *		m_pTextureEnableAttr;
	i3LightingEnableAttr *		m_pLightingEnableAttr;
	i3RenderModeAttr *			m_pRenderModeAttr;
	i3FaceCullModeAttr *		m_pFaceCullModeAttr;
	i3ColorAttr *				m_pColorAttr;

	bool						m_bFreeze;
	bool						m_bSimulation;

	UINT32						m_SelectMask;

protected:
	void			_RemoveAllCollideeMesh(void);

	void			_Reset(void);
	void			_BuildCollisionMesh(void);
	void			_BuildClothParticle(void);
	void			_BuildSpringGeometry( COLL_MESH_INFO * pInfo);

	void			_UpdateVertexPos(void);

	void			_DrawDragStart( POINT pt);
	void			_DrawDrag( POINT pt);
	void			_DrawDragEnd( POINT pt);

	void			_UnselectAllVertex(void);
	void			_SelectVertex( CRect * pRect, bool bSelect);
	INT32			_FindSelectMesh(void);

	void			_SetFreezeMode( bool bFreeze);

	void			_SelectAllSpring(void);
	void			_SelectSpring( CRect * pRect, bool bSelect);
	void			_UnselectAllSpring(void);
	void			_ReverseSelectSpring(void);
	void			_SelectSpring(void);
	void			_DeleteSelectedSpring(void);

	void			_SelectAllParticle(void);
	void			_SelectParticle( CRect * pRect, bool bSelect );
	void			_UnselectAllParticle(void);
	void			_ReverseSelectParticle(void);
	void			_DeleteSelectedParticle(void);

public:
	CClothFramework(void);
	virtual ~CClothFramework(void);

	i3AttrSet *		getVertexRoot(void)				{ return m_pVertexRoot; }
	i3AttrSet *		getSpringRoot(void)				{ return m_pSpringRoot; }

	i3ClothObject * getCloth(void)					{ return m_pCloth; }

	void			setSimulation( bool bValue )	{ m_bSimulation = bValue; }

	UINT32			getSelectMask(void)				{ return m_SelectMask; }
	void			addSelectMask( UINT32 mask)		{ m_SelectMask |= mask; }
	void			removeSelectMask( UINT32 mask)	{ m_SelectMask &= ~mask; }

	void			GetSelectedSpring( i3List * pList);
	void			GetSelectedParticle( i3List * pList);

	void			Reset(void);
	void			SetSceneGraph( i3Node * pSG);

	virtual void	OnCreate(void);
	virtual void	OnChangeScene(void);
	virtual void	OnRender(void);

	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes);

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point);

	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
};

#endif
