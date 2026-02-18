#pragma once

#include "UIItemSlot.h"


enum UI_TEXTURE_TYPE
{
	UI_TEX_TYPE_WEAPON	= 0,

	UI_TEX_TYPE_ITEM_COUNT,
	UI_TEX_TYPE_ITEM_PERIOD,

	UI_TEX_TYPE_BERET,
	UI_TEX_TYPE_HEAD,
	UI_TEX_TYPE_MASK,

	UI_TEX_TYPE_CHARA,

	UI_TEX_MAX
};


class cUISlotList : public i3GameNode
{
	I3_CLASS_DEFINE(cUISlotList, i3GameNode);

public:
	cUISlotList(void);
	virtual ~cUISlotList(void);

	bool Create(i3Node* pSceneNode);

	bool SetUITexture( UI_TEXTURE_TYPE textype, UINT32 nIndex, i3Texture* pTex );
	void SetSize(INT32 nX, INT32 nY, UINT32 nWidth, UINT32 nHeight);
	void ResizeUI(void);

	void ResetItemList(INT32 nStartIndex);
	void SelectSlot(const POINT& point);

	INT32 GetScrollCount(void) const;
	INT32 GetColPage(void) const;


protected:
	void	_Destroy(void);
	void	_SetSelectedSlot(bool bSelected);

	bool	_UITexWeapon( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex );
	bool	_UITexGoods( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex );
	bool	_UITexEquip( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex, int nType);
	bool	_UITexChara( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex );

	void	_CalcUISize(REAL32& SlotWidth, REAL32& SlotHeight, 
		REAL32& SpriteWidth, REAL32& SpriteHeight, REAL32& ImageHeight);


private:
	enum { SLOT_ROW_MAX	= 10,
		SLOT_COL_MAX	= 10,
		SLOT_COUNT_MAX	= SLOT_ROW_MAX * SLOT_COL_MAX };

private:
	cUIItemSlot		m_Slot[ SLOT_COUNT_MAX ];

	INT32			m_Row;
	INT32			m_ColPage;

	INT32			m_StartIndex;
	INT32			m_SelectIndex;

	REAL32			m_PosX, m_PosY;
	REAL32			m_Width, m_Height;
	REAL32			m_SlotWidth, m_SlotHeight;
	REAL32			m_RateX, m_RateY;

	typedef std::vector<i3Texture*> vTexture;
	vTexture		m_vTexture[UI_TEX_MAX];

	i3Texture*		m_pBgTexture;
};
