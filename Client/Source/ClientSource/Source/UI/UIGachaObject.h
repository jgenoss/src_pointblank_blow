#if !defined( __GACHA_OBJECT_H)
#define __GACHA_OBJECT_H

#include "UIGachaDef.h"
#include "ItemImageMgr.h"


//enum GACHA_WEAPON_ANI_TYPE
//{
//	GACHA_ANI_MOVE,
//	GACHA_ANI_SHAKE,
//};
//
//struct AttrAni
//{
//	REAL32 origin_pos_x, origin_pos_y, origin_pos_z;
//	REAL32 target_pos_x, target_pos_y, target_pos_z;
//	REAL32 play_time,	accumulateTime;
//	WeaponBase *			_weaponBase;
//	INT8					_type;
//	bool					_invert;
//
//	AttrAni()	{	_weaponBase = nullptr;	_type = -1;	_invert = false; play_time = 0.0f; accumulateTime = 0.0f;	}
//	~AttrAni()	{	_weaponBase = nullptr;	_type = -1;	_invert = false;}
//	void setAttr(WeaponBase * pBase, GACHA_WEAPON_ANI_TYPE anitype)
//	{
//		_weaponBase	= pBase;
//		_type		= (INT8)anitype;
//		VEC3D * pVec = i3Matrix::GetPos(_weaponBase->GetMatrix());
//		origin_pos_x = pVec->x;
//		origin_pos_y = pVec->y;
//		origin_pos_z = pVec->z;
//	}
//	void setTargetPos(REAL32 x, REAL32 y, REAL32 z)
//	{
//		target_pos_x = x;
//		target_pos_y = y;
//		target_pos_z = z;
//	}
//	void setInvert(bool bInvert){	_invert = bInvert;	}
//	void ClearData()
//	{
//		if(_weaponBase)
//		{
//			_weaponBase->SetPos(origin_pos_x,origin_pos_y,origin_pos_z);
//			_weaponBase = nullptr;
//		}
//		origin_pos_x = 0.0f;	origin_pos_y = 0.0f;	origin_pos_z = 0.0f;
//		target_pos_x = 0.0f;	target_pos_y = 0.0f;	target_pos_z = 0.0f;
//		_type		= -1;
//	}
//};
//
//struct SLOT_COMMON_ANI
//{
//	virtual void operator()(AttrAni* pAttr, REAL32 ratio) = 0;
//};
//
//struct SLOT_WEAPON_ANI : public SLOT_COMMON_ANI
//{
//	bool invert;
//	virtual void operator()( AttrAni* pAttr, REAL32 ratio);
//
//};
//
//struct aniManager
//{
//	//typedef i3::vector<AttrAni*> slotAttList;
//	AttrAni*	mSlotAniList[GACHA_POS_MAX];
//	REAL32		m_accumulate_time;
//	REAL32		m_play_time;
//	SLOT_WEAPON_ANI		m_ani;
//
//	aniManager();
//	~aniManager();
//
//	void	setAttr(size_t pos, AttrAni * pAni)		{	mSlotAniList[pos] = pAni;	}
//	void	setInvert(bool bInvert);
//	void	moveTarget(size_t pos);
//	void	moveOrigin(size_t pos);
//
//	SLOT_COMMON_ANI * getType(GACHA_WEAPON_ANI_TYPE t);
//	void	OnUpdate(REAL32 rDeltaTime);
//	void	OnStart(REAL32 play_time);
//	void	Clear();
//
//};



class GachaObject : public i3Object
{
	I3_CLASS_DEFINE( GachaObject, i3Object);
public:
	GachaObject();
	~GachaObject();

	virtual void	Create( i3Framework * pFramework, i3GameResObject * pRes) override;
	virtual void	OnUpdate( REAL32 tm) override;
	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
public:
	void			InitObject(i3GameResObject*);
	void			InitAnimation();
	//void			AddWeaponToPos(GACHA_WEAPON_POS pos, T_ItemID id);
	void			SetGachaAnimState(GACHA_ANIM_STATE eState,REAL32 rStartTime, I3_ANIM_PLAY_MODE t1, I3_ANIM_PLAY_MODE t2);
	void			DeleteWeaponData(void);
	//void			AttachNodeToObject(WeaponBase * pWeapon, i3Node * pSceneNode, i3GameNode * pOwner );

	void			AddItemToPos(GACHA_WEAPON_POS pos, T_ItemID id, i3UI3DViewBox* viewBox);

	//aniManager*		getAniManager()				{ return &m_AniManager;					}
	//i3Node*			getWeaposPos()				{ return m_pWeaponPos[0];				}
	//WeaponBase*		getWeaposBase()				{ return m_pWeapon[0];					}
	i3Animation*	getCurrentAni()				{ return m_pAnim[m_eGachaAnimState];	}
	i3Animation*	getAni(GACHA_ANIM_STATE t)	{ return m_pAnim[t];					}

	//void			AttachToDualNode(WeaponBase * pBase, i3Node * pSceneNode);
	//void			setWeaponPosition(WeaponBase * pWeapon);

	//void			SetRotateX();
	//void			setWeaponSRT(WeaponBase * pBase);
	//void			SetRotateZ();
	void			ClearData(int _pos);

private:

	i3Texture*		_getItemTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect, i3UI3DViewBox* viewBox);
	i3Texture*		_getAvatarTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect, i3UI3DViewBox* viewBox);
	i3Texture*		_getWeaponTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect, i3UI3DViewBox* viewBox);

	void	SetAvatarTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect);
	void	SetItemTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect);
	void	SetWeaponTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect);

private:
	i3Animation*		m_pAnim[GACHA_CASE_MAX];
	i3Node*				m_pWeaponPos[GACHA_POS_MAX];
	i3Node*				m_pDualWeaponPos[GACHA_POS_MAX];
	i3Node*				m_pEquipNode;

	i3Texture*			m_pTexture;
	i3UIButtonImageSet*	m_ImgBox;
	i3::vector<i3UIButtonImageSet*> m_ImgBoxList;
	//i3Quad*				m_ItemImg[GACHA_POS_MAX];
	i3Sprite*			m_ItemImg[GACHA_POS_MAX];
	I3VT_TEXINFO*		m_TexInfo[GACHA_POS_MAX];

	GACHA_STATE			m_pGachaState;
	GACHA_ANIM_STATE	m_eGachaAnimState;
	REAL32				m_rStateTime;
	REAL32				m_rDuration;
	T_ItemID			 m_ItemID[GACHA_POS_MAX];
	//WeaponBase *		m_pWeapon[GACHA_POS_MAX];

	//aniManager			m_AniManager;
	//AttrAni				m_AniAttr[GACHA_POS_MAX];

	//i3Node*				m_pWeaponDummyPos;
};

#endif
