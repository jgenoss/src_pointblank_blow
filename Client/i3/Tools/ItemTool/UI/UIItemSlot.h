#pragma once

class cUIItemSlot
{
public:
	cUIItemSlot(void);
	~cUIItemSlot(void);

	bool Create(i3Node* pSceneNode);
	void Destroy(void);

	void RecreateFont(INT32 nSize);

	void SetRect(REAL32 X, REAL32 Y, REAL32 Width, REAL32 Height, REAL32 ImageRate, REAL32 rRateWidth, REAL32 rRateHeight);
	void SetRectImage(REAL32 x, REAL32 y, REAL32 width, REAL32 height, REAL32 SpriteWidth, REAL32 SpriteHeight);

	void SetData(i3Texture* pTexture, CRect& rect, const char* pszName)
	{
		SetTexture(pTexture, rect);
		SetText(pszName);
	}
	void	SetTexture(i3Texture* pTexture, CRect& rect);
	void	SetDisableUI(void)			{ if( m_pSpriteImage ) m_pSpriteImage->SetEnable(0, FALSE);	}
	void	SetText(const char* pszName);
	void	SetBgTexture(i3Texture* pTexture, CRect& rect);
	void	SetBgColor(bool bSelected);


	void SetVisible(BOOL bVisible);
protected:
	i3Sprite2D*			m_pBgSprite;
	i3Sprite2D*			m_pSpriteImage;
	i3TextNodeDX2*		m_pTextName;
	i3TextNodeDX2*		m_pBgTextName;
	COLOR				m_BgColor;
	i3Node*				m_pSlotNode;
};
