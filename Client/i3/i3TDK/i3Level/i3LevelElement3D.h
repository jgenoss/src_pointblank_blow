#if !defined( __I3_LEVEL_ELEMENT_3D_H)
#define __I3_LEVEL_ELEMENT_3D_H

#include "i3LevelResSceneGraph.h"
#include "i3LevelPropertyContainer.h"

#include "i3LevelLayer.h"
#include "i3LevelGroup.h"

#include "i3Base/string/ext/string_ncopy_nullpad.h"

// Level Style의 형식
// 0xHHHHLLLL
//
// HHHH 영역의 Bit Flag들은 저장되지 않습니다.
// 
#define I3_LEVEL_STYLE_VOLATILE			0x00000001
#define I3_LEVEL_STYLE_CONTROL			0x00000002
#define	I3_LEVEL_STYLE_UNSELECT			0x00000004
#define I3_LEVEL_STYLE_UNFREEZE			0x00000008
#define I3_LEVEL_STYLE_NOMOVE			0x00000010
#define I3_LEVEL_STYLE_NOSCALE			0x00000020
#define I3_LEVEL_STYLE_NOROTATE_X		0x00000040
#define I3_LEVEL_STYLE_NOROTATE_Y		0x00000080
#define I3_LEVEL_STYLE_NOROTATE_Z		0x00000100
#define I3_LEVEL_STYLE_UNREMOVABLE		0x00000200
#define	I3_LEVEL_STYLE_INIT_DISABLE		0x00000400
#define I3_LEVEL_STYLE_TOUCH_BBOXONLY	0x00000800

#define	I3_LEVEL_STYLE_NO_FILERES		0x80000000

#define I3_LEVEL_STYLE_NOROTATE		(I3_LEVEL_STYLE_NOROTATE_X | I3_LEVEL_STYLE_NOROTATE_Y | I3_LEVEL_STYLE_NOROTATE_Z)
#define	I3_LEVEL_LOAD_MASK				0x0000FFFF

#define I3_LEVEL_STATE_DISABLE			0x00000001
#define I3_LEVEL_STATE_SELECTED			0x00000002
#define I3_LEVEL_STATE_FROZEN			0x00000004
#define I3_LEVEL_STATE_HIDE				0x00000008
#define I3_LEVEL_STATE_LOCKED			0x00000010
#define I3_LEVEL_STATE_FAILED_BINDRES	0x00000020
#define	I3_LEVEL_STATE_COLLISION_EDIT	0x00000040
#define	I3_LEVEL_STATE_COLLISION_VIEW	0x00000080
#define	I3_LEVEL_STATE_TARGETED			0x00000100
#define I3_LEVEL_STATE_ATTACHED			0x00010000

enum I3LV_DYNAMIC_STATE
{
	I3LV_DYNAMIC_NONE	= 0,
	I3LV_DYNAMIC_DYNAMIC,
	I3LV_DYNAMIC_KINEMATIC,
	I3LV_DYNAMIC_STATIC,
};

enum PICK_TYPE
{
	PICK_SELECT = 0,
	PICK_UNSELECT,
	PICK_TARGET,
	PICK_UNTARGET
};

enum	SEL_MODE
{
	SEL_EXCLUSIVE,
	SEL_INCLUSIVE,
	SEL_ALTERNATIVE,
};

class I3_EXPORT_TDK i3LevelElement3D : public i3LevelPropertyContainer
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelElement3D, i3LevelPropertyContainer);

	friend	class i3LevelLayer;
	friend	class i3LevelGroup;

protected:
	// Persist Members
	UINT32				m_Style = 0;
	I3LV_DYNAMIC_STATE	m_DynamicState = I3LV_DYNAMIC_STATIC;
	I3_NETWORK_GAMEOBJECT_TYPE	m_NetworkSyncMode = I3_NETWORK_GAMEOBJECT_TYPE_NONE;
	i3Node *			m_pSymbol = nullptr;

	VEC3D				m_OriginalScale;

	// Volatile Members
	UINT32				m_State = 0;
	UINT32				m_TypeMask = 0;
	i3AttrSet *			m_pRoot;					// [initialize at constructor]
	i3SceneObject *		m_pScene;					// [initialize at constructor]
	i3Transform *		m_pSelBox;					// [initialize at constructor]
	i3Transform *		m_pTargetBox;				// [initialize at constructor]
	i3AttrSet *			m_pVisibleCollRoot = nullptr;
	i3ColorAttr *		m_pVisibleCollColorAttr = nullptr;
	

	VEC3D				m_BoundMin;
	VEC3D				m_BoundMax;

	MATRIX				m_SavedMatrix;

	i3ColorAttr *		m_pColorAttr;				// [initialize at constructor]
	i3MaterialAttr *	m_pMaterialAttr;			// [initialize at constructor]
	i3BlendEnableAttr *	m_pBlendEnableAttr;			// [initialize at constructor]
	i3ZWriteEnableAttr *m_pZWriteEnableAttr;		// [initialize at constructor]
	
	i3::vector<i3GeometryAttr*>	m_SelGeoAttrList;

	i3LevelRes *		m_pRes = nullptr;
	char				m_szResPath[MAX_PATH] = { 0 };
	char 				m_szInstanceClass[128] = { 0 };

	i3::vector<I3_VISIBLE_COLL_MESH_INFO*>	m_ShapeSetInfoList;

	i3LevelLayer *		m_pLayer = nullptr;
	i3LevelGroup *		m_pGroup = nullptr;

	i3::vector<i3LevelElement3D*>			m_TargetList;

protected:
	void				CreateSelectBoxGeometry(void);
	void				_CreateTargetBoxGeometry(void);
	void				_CreateVisibleCollision(void);
	void				_RemoveAllShapeSetInfo(void);
	REAL32				_ScaleAxis(VEC3D * pAxis, REAL32 inc, REAL32 snap);

	void				AdjustPhysixSize(void);

public:
	i3LevelElement3D(void);
	virtual ~i3LevelElement3D(void);

	i3LevelRes *		getRes(void)				{ return m_pRes; }
	const char *		getResPath(void) const;
	virtual void		BindRes( i3LevelRes * pRes);

	i3SceneObject *		GetSceneObject(void)		{ return m_pScene; }
	i3Node *			GetRoot(void)				{ return m_pRoot; }

	i3Node *			GetSymbol(void)				{ return m_pSymbol; }
	virtual void		SetSymbol( i3Node * pSG);

	i3LevelLayer *		getLayer(void)				{ return m_pLayer; }
	void				setLayer( i3LevelLayer * pLayer);

	i3LevelGroup *		getGroup(void)				{ return m_pGroup; }
	void				setGroup( i3LevelGroup * pGrp);

	I3LV_DYNAMIC_STATE	getDynamicState(void)		{ return m_DynamicState; }
	void				setDynamicState( I3LV_DYNAMIC_STATE state)		{ m_DynamicState = state; }

	I3_NETWORK_GAMEOBJECT_TYPE	getNetworkSyncMode(void)			{ return m_NetworkSyncMode; }
	void						setNetworkSyncMode( I3_NETWORK_GAMEOBJECT_TYPE mode)
	{
		m_NetworkSyncMode = mode;
	}

	char *				getInstanceClassName(void)					{ return m_szInstanceClass; }
	void				setInstanceClassName( char * pszName)		
	{ 
		i3::string_ncopy_nullpad( m_szInstanceClass, pszName, sizeof(m_szInstanceClass) - 1); 

		OnChangeInstanceClassName();
	}

	VEC3D *			GetBoundMin(void)			{ return &m_BoundMin; }	VEC3D *			GetBoundMax(void)			{ return &m_BoundMax; }

	virtual void			GetTransformedBoundBox( VEC3D * pMin, VEC3D * pMax);

	void			SetColor( COLOR * pColor);
	void			GetColor( COLOR * pColor);
	
	void			SetMaterialDiffuse( REAL32 r, REAL32 g, REAL32 b, REAL32 a);
	void			SetMaterialDiffuse( COLORREAL * pCol)
	{
		SetMaterialDiffuse( pCol->r, pCol->g, pCol->b, pCol->a);
	}

	COLORREAL *		getMaterialDiffuse(void)	{ return m_pMaterialAttr->GetDiffuse(); }

	void			SetMaterialSpecular( REAL32 r, REAL32 g, REAL32 b);
	COLORREAL *		getMaterialSpecular(void)	{ return m_pMaterialAttr->GetSpecular(); }

	UINT32			GetStyle(void)				{ return m_Style; }
	void			SetStyle( UINT32 style)		{ m_Style = style; }
	void			AddStyle( UINT32 style)		{ m_Style |= style; }
	void			RemoveStyle( UINT32 style)	{ m_Style &= ~style; }
	BOOL			IsStyle( UINT32 style)		{ return (m_Style & style) != 0; }

	UINT32			GetState(void)				{ return m_State; }
	void			SetState( UINT32 state)		{ m_State = state; }
	void			AddState( UINT32 state)		{ m_State |= state; }
	void			RemoveState( UINT32 state)	{ m_State &= ~state; }
	BOOL			IsState( UINT32 state)		{ return (m_State & state) != 0; }

	UINT32			getTypeMask(void)			{ return m_TypeMask; }

	void			Attach( i3Node * pRoot);
	void			Detach(void);

	BOOL			IsEnable(void)				{ return IsState( I3_LEVEL_STATE_DISABLE) == FALSE; }
	void			Enable(void);
	void			Disable(void);

	BOOL			IsSelected(void)			{ return (m_State & I3_LEVEL_STATE_SELECTED) != 0; }
	virtual void	Select(void);
	virtual void	Unselect(void);

	BOOL			IsFreezed(void)				{ return (m_State & I3_LEVEL_STATE_FROZEN) != 0; }
	virtual void	Freeze(void);
	virtual void	Unfreeze(void);

	BOOL			IsTargeted(void)			{ return (m_State & I3_LEVEL_STATE_TARGETED) != 0; }
	virtual void	Targeted(void);
	virtual void	Untargeted(void);

	void			addTarget( i3LevelElement3D * pElm);
	void			removeTarget( i3LevelElement3D * pElm);
	void			removeAllTarget(void);
	INT32			getTargetCount(void)								{ return (INT32)m_TargetList.size(); }
	i3LevelElement3D *	getTarget( INT32 idx)							{ return m_TargetList[idx]; }
	i3LevelElement3D *	FindTargetByGameObj( i3GameObj * pObj);

	void			SetCollisionEditEnable( bool bEdit);
	bool			getCollisionEditEnable(void)	
	{ 
		return IsState( I3_LEVEL_STATE_COLLISION_EDIT) == TRUE;
	}

	void			SetCollisionVisible( bool bView);

	void			addSelectedGeoAttr( i3GeometryAttr * pGeoAttr)			{ i3::vu::push_back_once(m_SelGeoAttrList, pGeoAttr); }

	void			setPos( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Matrix::SetPos( GetTransform(), x, y, z, 1.0f);

		OnMoving();
	}

	void			setPos( VEC3D * pPos)
	{
		i3Matrix::SetPos( GetTransform(), pPos);

		OnMoving();
	}

	VEC3D *			getPos(void)				{ return i3Matrix::GetPos( GetTransform()); }

	void			SetRotateDeg( REAL32 x, REAL32 y, REAL32 z);
	void			SetRotate( REAL32 x, REAL32 y, REAL32 z);
	void			GetRotateAngle( VEC3D * pRot);

	void			SetScale( REAL32 x, REAL32 y, REAL32 z);
	virtual void	SetScale( VEC3D * pScale);
	virtual void	GetScale( VEC3D * pScale);

	MATRIX	*		getSavedMatrix(void)					{ return &m_SavedMatrix;}
	MATRIX *		GetTransform(void)						
	{ 
		if(m_pScene)
			return m_pScene->GetMatrix();

		return nullptr;
	}

	virtual void	SaveMatrix(void);
	virtual void	RestoreMatrix(void);
	virtual void	Moving( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap = 0.0f);
	virtual void	Rotating( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap = 0.0f);
	virtual void	Scaling( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap = 0.0f);

	virtual void	GripDrag( INT32 idxGrip, const VEC3D & scale, REAL32 scaleSnap = 0.0f);
	virtual void	Reset(void);

	virtual void	OnCreate(void);
	virtual void	OnMoving(void);
	virtual void	OnScaling(void);
	virtual void	OnRotating(void);
	virtual void	OnSelected( BOOL bSelected);
	virtual void	OnTargeted( BOOL bTargeted);
	virtual void	OnFrame(void);
	virtual void	OnChangeResource(void);
	virtual void	OnChangeColor(void);
	virtual void	OnChangeInstanceClassName(void);

	virtual bool	OnRemoveIcon( i3LevelControl * pElm);
	virtual void	OnMovingIcon( i3LevelControl * pElm);
	virtual void	OnScalingIcon( i3LevelControl * pElm);
	virtual void	OnRotatingIcon( i3LevelControl * pElm);
	virtual void	OnSelectedIcon( i3LevelControl * pElm);

	virtual REAL32	OnTouch( VEC3D * pStart, VEC3D * pEnd, MATRIX * pViewProj, bool bPick, bool bSelect, SEL_MODE mode);

	virtual void	OnInstantiate();
	virtual void	OnQueryCollisionNode( i3::vector<i3Node*>& List);

	virtual void		OnSetInstanceInfo( i3::pack::GAME_INSTANCE_2 * pInfo);
	virtual i3GameObj *	CreateGameInstance(void);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	OnAttachScene( i3LevelScene * pScene ) override;
	virtual void	OnDetachScene( i3LevelScene * pScene ) override;
};

#endif