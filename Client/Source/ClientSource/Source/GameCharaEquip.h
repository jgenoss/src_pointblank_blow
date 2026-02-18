#if !defined( CGAME_CHARA_EQUIP_H)
#define CGAME_CHARA_EQUIP_H

#include "GameCharaEquipDef.h"
#include "EquipInfo.h"
#include "GameCharaTextureDef.h"

class CGameCharaBase;

class CGameCharaEquip : public i3GameObj
{
	I3_CLASS_DEFINE( CGameCharaEquip, i3GameObj);

protected:
	EQUIP::ePART		m_EquipPart = EQUIP::ePART_UNKNOWN;	//	장비 부위
	UINT32				m_ID = 0;			//	장비 번호

	// Attribute
	const CEquipInfo *	m_pInfo = nullptr;

	REAL32				m_rDefenceRate = 0.0f;

	i3Node *			m_pAttachDummy = nullptr;
	i3Node *			m_pParentScene = nullptr;		//	렌더링 해줄 곳	
	i3GameNode *		m_pOwner = nullptr;			//	Update 해줄 곳
	i3Body *			m_pBody = nullptr;

	bool				m_bAttached = false;
	bool				m_bOwnerChara = false;
	REAL32				m_fDropElapseTime = 0.0f;
	REAL32				m_fAlphaElapseTime = 0.0f;
	REAL32				m_fAlphaVal = 0.0f;

	//	Material
	i3AttrSet *			m_pAttrSet = nullptr;
	i3MaterialAttr *	m_pMaterialAttr = nullptr;
	i3BlendEnableAttr *	m_pBlendEnable = nullptr;	

	//	TextureBind
	CharaTexBindSet		m_TexBindSet[ CHARA_3PV_LOD_COUNT];		// 실제 모델링의 3PV LOD의 갯수가 반드시 일치해야 합니다.

	i3PhysixShapeSet *	m_pContactHitBox = nullptr;

	i3::vector<I3_PHYSIX_CONTACT_REPORT_PTR>	m_vecContactReport;

	UINT16				m_nCollisionGroup = 0;

	bool				m_bModified = false;		// 텍스쳐 및 VA 수정이 된 경우 true

public:
	INT32				m_DropCount = 0;

protected:
	void	_QueContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport );
	void	_ParseContactReport( void );

public:
	virtual ~CGameCharaEquip( void);

	virtual void		Create( i3GameResSceneGraph * pRes, T_ItemID ItemID);
	virtual void		Attach( i3Node * pSceneNode, i3GameNode * pOwner, bool bTakeChara = false);
	virtual void		Detach( void);
	virtual void		Reset( void);
	virtual void		DumpState( void);
	virtual bool		IsDropObj() override { return m_bAttached; }
	virtual void		ReturnInstance(void) override;
	virtual void		OnBindResource(void) override;
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool		OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param, I3_EVT_CODE code) override;

public:
	void				SetCollisionGroup( UINT16 nGroup, UINT32 nMask );

	void				calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio = 1.0f) { m_pBody->CalcLOD( pPos, pCamPos, fFOVRatio); }
	void				setCurLOD( INT32 lv)				{ m_pBody->setCurrentLevel( lv);		}
	
    bool				IsAttached( void)	{ return m_bAttached;			}
	INT32				GetID( void)		{ return m_ID;				}
	i3GameNode *		GetOwner( void)		{ return m_pOwner;				}
	i3Node *			GetAttachDummy( void)			{ return m_pAttachDummy;		}
	void				SetEquipPart( EQUIP::ePART part)	{ m_EquipPart = part;			}
	EQUIP::ePART		GetEquipPart( void);
	bool				IsPhysixPart(void);

	void				SetBlendAlpha( REAL32 val);
	void				SetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular);

	void				Instantiate( void);

	const CEquipInfo *	GetInfo( void)		{ return m_pInfo; }

	bool				IsModified( void)		{ return m_bModified; }
	void				SetModified( bool bVal)	{ m_bModified = bVal; }

public:
	void				ReloadTransform();
	bool				SetEquipMatrix(INT32 idx, VEC3D * pVecPos);
};

#endif	//CGAME_CHARA_EQUIP_H


